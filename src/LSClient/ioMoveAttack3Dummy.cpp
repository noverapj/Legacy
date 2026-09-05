
#include "stdafx.h"

#include "ioMoveAttack3Dummy.h"
#include "BuffDefine.h"

ioMoveAttack3Dummy::ioMoveAttack3Dummy( ioEntityGroup *pGrp, ioPlayMode *pMode )
	: ioDummyChar( pGrp, pMode )
{
	m_MoveState  = MS_WAIT;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	m_vTargetList.clear();

	m_vTargetPos = ioMath::VEC3_ZERO;
	m_bCreateWeapon = false;
	m_dwFireStartTime = 0;
	m_dwBaseWeaponIndex	= 0;
}

ioMoveAttack3Dummy::~ioMoveAttack3Dummy()
{
}

void ioMoveAttack3Dummy::LoadProperty( ioINILoader &rkLoader )
{
	ioDummyChar::LoadProperty( rkLoader );

	m_fMoveSpeed  = rkLoader.LoadFloat_e( "move_speed", FLOAT1 );
	LoadAttrbuteProperty( rkLoader, "magicengineer" );
}

void ioMoveAttack3Dummy::LoadAttrbuteProperty( ioINILoader &rkLoader, const char* szTitle )
{
	char szKey[MAX_PATH];
	wsprintf_e( szKey, "%s_fire_weapon_max", szTitle );
	int iMax = rkLoader.LoadInt( szKey, 0 );
	for( int i = 0; i < iMax; ++i )
	{
		MEDummyAttackInfo Info;
		wsprintf_e( szKey, "%s_fire_weapon%d", szTitle, i + 1 );
		Info.m_dwWeaponIndex = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "%s_fire_weapon%d_fire_time", szTitle, i + 1 );
		Info.m_dwFireTime = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "%s_fire_weapon%d_offset_x", szTitle, i + 1 );
		Info.m_vOffset.x = rkLoader.LoadFloat( szKey, 0.0f );

		wsprintf_e( szKey, "%s_fire_weapon%d_offset_y", szTitle, i + 1 );
		Info.m_vOffset.y = rkLoader.LoadFloat( szKey, 0.0f );

		wsprintf_e( szKey, "%s_fire_weapon%d_offset_z", szTitle, i + 1 );
		Info.m_vOffset.z = rkLoader.LoadFloat( szKey, 0.0f );

		m_MEDummyAttackInfoList.push_back( Info );
	}
}

void ioMoveAttack3Dummy::ProcessState( float fTimePerSec, DWORD dwPreTime )
{
	if( !m_bCreateWeapon )
	{
		m_bCreateWeapon = true;
		m_State = DCS_MOVE;

		SetMoveState();
	}

	ProcessMoveState( fTimePerSec );
}

void ioMoveAttack3Dummy::ProcessMoveState( float fTimePerSec )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	D3DXVECTOR3 vPos = GetWorldPosition();
	D3DXQUATERNION qtRot;

	switch( m_MoveState )
	{
	case MS_WAIT:
		break;
	case MS_MOVE:
		{
			float fMoveSpeed = m_fMoveSpeed * fTimePerSec;
			if( ( m_fNeedMoveLength - fMoveSpeed ) > 0.f )
			{
				m_fNeedMoveLength -= fMoveSpeed;
			}
			else
			{
				fMoveSpeed = m_fNeedMoveLength;
				SetFireState();	
			}
			
			bool bCol = false;

			D3DXVECTOR3 vMove;
			vMove = m_vTargetPos - GetWorldPosition();
			D3DXVec3Normalize( &vMove, &vMove );
			vMove = fMoveSpeed * vMove;

			if( pStage && pStage->ReCalculateMoveVectorByTerrain( this, &vMove, bCol ) )
			{
				Translate( vMove );
			}
			if( bCol )
				SetFireState();	

			float fMapHeight = GetBottomHeight();
			if( GetWorldPosition().y <= fMapHeight )
			{
				D3DXVECTOR3 vPos = GetWorldPosition();
				vPos.y = fMapHeight;
				SetWorldPosition( vPos );
				return;
			}
		}
		break;
	case MS_FIRE:
		{
			if( m_dwFireStartTime == 0 )
			{
				SetWaitState();
				return;
			}

			for( MEDummyAttackInfoList::const_iterator iter = m_CurrAttackList.begin(); iter != m_CurrAttackList.end(); )
			{
				const MEDummyAttackInfo& Info = *iter;
				if( Info.m_dwFireTime < FRAMEGETTIME() )
				{
					D3DXVECTOR3 vDir = GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
					D3DXVec3Normalize( &vDir, &vDir );

					D3DXVECTOR3 vSide;
					D3DXVec3Cross( &vSide, &vDir, &ioMath::UNIT_Y );
					D3DXVECTOR3 vPos = GetWorldPosition();
					vPos	+= vDir * Info.m_vOffset.z;
					vPos.y	+= Info.m_vOffset.y;
					vPos	+= vSide * Info.m_vOffset.x;

					FireTime kFireTime;
					kFireTime.dwStart				= FRAMEGETTIME();
					kFireTime.iAttributeIdx			= Info.m_dwWeaponIndex;
					kFireTime.iResistanceIdx		= 0;
					kFireTime.szWoundedAni			= "";
					kFireTime.dwWoundedAniDuration	= 0;
					kFireTime.bLoopWoundedAni		= false;
					kFireTime.dwWeaponIdx			= m_dwBaseWeaponIndex++;
					kFireTime.eFireTimeType			= FTT_DUMMY_CHAR_ATTACK;

					pOwner->ExtendFireExplicit( kFireTime, vPos, vDir, "" );

					iter = m_CurrAttackList.erase( iter );
				}
				else
				{
					++iter;
				}
			}
		}
		break;
	}
}

void ioMoveAttack3Dummy::SetWaitState()
{
	m_MoveState = MS_WAIT;
	m_fNeedMoveLength = 0.f;
	m_dwFireStartTime = 0;
	m_dwBaseWeaponIndex = 0;
}

void ioMoveAttack3Dummy::SetFireState()
{
	if( m_MEDummyAttackInfoList.size() == 0 )
	{
		SetWaitState();
		return;
	}
	m_MoveState = MS_FIRE;
	m_fNeedMoveLength = 0.f;
	m_dwFireStartTime = FRAMEGETTIME();
	m_dwBaseWeaponIndex	= GetOwner()->IncreaseWeaponIndexBase();

	m_CurrAttackList.clear();
	for( MEDummyAttackInfoList::const_iterator iter = m_MEDummyAttackInfoList.begin(); iter != m_MEDummyAttackInfoList.end(); ++iter )
	{
		MEDummyAttackInfo Info = *iter;

		//발사 타임 조정
		Info.m_dwFireTime += m_dwFireStartTime;
		m_CurrAttackList.push_back( Info );
	}
}

bool ioMoveAttack3Dummy::IsActivateWeapon()
{
	if( m_State == DCS_DELAY )
		return false;

	return true;
}

void ioMoveAttack3Dummy::SetMovePosition( const D3DXVECTOR3& vTargetPos )
{
	m_vTargetPos = vTargetPos;

	D3DXVECTOR3 vPos = GetWorldPosition();

	vPos = m_vTargetPos - vPos;

	m_fNeedMoveLength = D3DXVec3Length( &vPos );
	m_MoveState = MS_MOVE;
}