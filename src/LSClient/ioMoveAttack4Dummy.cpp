
#include "stdafx.h"

#include "ioMoveAttack4Dummy.h"
#include "BuffDefine.h"

ioMoveAttack4Dummy::ioMoveAttack4Dummy( ioEntityGroup *pGrp, ioPlayMode *pMode )
	: ioDummyChar( pGrp, pMode )
{
	m_MoveState  = MS_WAIT;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	m_vTargetList.clear();

	m_vTargetPos = ioMath::VEC3_ZERO;
	m_bCreateWeapon = false;
	m_dwFireStartTime = 0;
	m_dwBaseWeaponIndex	= 0;

	m_iCurCombo = 0;

	m_vDisappearAttackIdx.clear();
}

ioMoveAttack4Dummy::~ioMoveAttack4Dummy()
{
}

void ioMoveAttack4Dummy::LoadProperty( ioINILoader &rkLoader )
{
	ioDummyChar::LoadProperty( rkLoader );

	m_fMoveSpeed  = rkLoader.LoadFloat_e( "move_speed", FLOAT1 );
	LoadAttrbuteProperty( rkLoader, "magicengineer" );

	int iAttackCnt = rkLoader.LoadInt( "dummy_attack_cnt", 0 );

	char szBuf[MAX_PATH] = "";
	
	for( int i = 0; i < iAttackCnt; i++ )
	{
		wsprintf_e( szBuf, "dummy_attack%d_disappear", i+1 );
		bool bDisappearAfterAttack = rkLoader.LoadBool( szBuf, false );
		if( bDisappearAfterAttack )
			m_vDisappearAttackIdx.push_back( i + 1 );
	}

	float fAttackStateGravityAmt = rkLoader.LoadFloat( "dummy_attack_state_gravity_amt", FLOAT1 );
	m_fAttackStateGravityAmt = fAttackStateGravityAmt;
}

void ioMoveAttack4Dummy::LoadAttrbuteProperty( ioINILoader &rkLoader, const char* szTitle )
{
	char szKey[MAX_PATH];
	wsprintf_e( szKey, "%s_fire_weapon_max", szTitle );
	int iMax = rkLoader.LoadInt( szKey, 0 );
	for( int i = 0; i < iMax; ++i )
	{
		ME2DummyAttackInfo Info;
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

void ioMoveAttack4Dummy::ProcessState( float fTimePerSec, DWORD dwPreTime )
{
	//if( !m_bCreateWeapon )
	//{
	//	m_bCreateWeapon = true;
	//	m_State = DCS_MOVE;

	//	SetMoveState();
	//}

	ProcessMoveState( fTimePerSec );
	if( m_bGravity )
		ProcessPos( fTimePerSec );

	if( m_State == DCS_ATTACK )
	{
		CheckReservedSliding();
		CheckExtraAniJump();
		ProcessAttackState( fTimePerSec, dwPreTime );
		if( m_State == DCS_DELAY )
		{
			if( !m_vDisappearAttackIdx.empty() )
			{
				bool bDisappearAfterAttack = false;
				for ( int i = 0; i < (int)m_vDisappearAttackIdx.size(); i++ )
				{
					if( m_vDisappearAttackIdx[ i ] == m_iCurCombo )
					{
						bDisappearAfterAttack = true;
						break;
					}
				}
				if( bDisappearAfterAttack )
					SetDieState( false );
			}
			
		}
		//if( m_iCurCombo > 1 )
		//	ProcessPos( fTimePerSec );
	}

	ProcessForceMove( fTimePerSec );

	D3DXVECTOR3 vCurPos = GetWorldPosition();
	float fMapHeight = GetBottomHeight();
	if( !m_bGravity )
	{
		if( vCurPos.y <= fMapHeight )
			SendDieState( true );
		else
		{
			ioBaseChar *pOwner = GetOwner();
			if(	pOwner )
				SetWorldOrientation( pOwner->GetTargetRot() );
		}
	}
	else
	{
		ioBaseChar *pOwner = GetOwner();
		if(	pOwner )
			SetWorldOrientation( pOwner->GetTargetRot() );
	}
}

void ioMoveAttack4Dummy::ProcessMoveState( float fTimePerSec )
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
		{

		}
		break;
	case MS_MOVE_ATTACK:
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

			for( ME2DummyAttackInfoList::const_iterator iter = m_CurrAttackList.begin(); iter != m_CurrAttackList.end(); )
			{
				const ME2DummyAttackInfo& Info = *iter;
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

void ioMoveAttack4Dummy::ProcessPos( float fTimePerSec )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_State == DCS_START )
	{
		m_dwProcessTime = dwCurTime;
		return;
	}

	ProcessForceMove( fTimePerSec );

	// 아래는 높이값 처리.
	if(m_DummyCharType != DCT_TARGET_TRACE4 && !m_bNoGravityMove )
	{
		D3DXVECTOR3 vPrePos = GetWorldPosition();
		D3DXVECTOR3 vCurPos = vPrePos;
		float fMapHeight = GetBottomHeight();

		DWORD dwGapTime = dwCurTime - m_dwProcessTime;

		//do 
		{
			DWORD dwCurGap = min( dwGapTime, 10 );
			if( dwGapTime >= 10 )
			{
				dwGapTime -= 10;
			}
			else
			{
				dwCurGap = dwGapTime;
				dwGapTime = 0;
			}

			float fNewTimePerSec = (float)dwCurGap / FLOAT1000;

			if( m_fCurJumpPowerAmt > 0.0f )
			{
				vCurPos.y += m_fCurJumpPowerAmt * fNewTimePerSec;
			}

			if( vCurPos.y > fMapHeight )
			{
				float fCurRate = m_fGravityRate;
				if( fCurRate <= 0.0f )
					fCurRate = FLOAT1;

				float fGravityAmt = m_fGravityAmt;
				float fRoomGravity = m_pModeParent->GetCreator()->GetRoomGravity();
				if( m_bGravity )
				{
					//if( m_State != DCS_ATTACK )
					m_fGravityAmt += (m_pModeParent->GetCreator()->GetRoomGravity() * fCurRate ) * fNewTimePerSec;
				}
				else
					m_fGravityAmt += (m_pModeParent->GetCreator()->GetRoomGravity() * fCurRate ) * fNewTimePerSec;

				vCurPos.y -= m_fGravityAmt * fTimePerSec;
				fGravityAmt = m_fGravityAmt;

				if( vCurPos.y <= fMapHeight )
				{
					m_fGravityAmt = 0.0f;
					m_fCurJumpPowerAmt = 0.0f;
				}
			}
		} //while( dwGapTime > 0 );

		if( vCurPos.y < fMapHeight )
		{
			vCurPos.y = fMapHeight;
			m_fGravityAmt = 0.0f;
			m_fCurJumpPowerAmt = 0.0f;
		}

		SetWorldPosition( vCurPos );
	}

	m_dwProcessTime = dwCurTime;
}

void ioMoveAttack4Dummy::SetWaitState()
{
	m_MoveState = MS_WAIT;
	m_fNeedMoveLength = 0.f;
	m_dwFireStartTime = 0;
	m_dwBaseWeaponIndex = 0;

	SetDelayState(true);
}

void ioMoveAttack4Dummy::SetFireState()
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
	for( ME2DummyAttackInfoList::const_iterator iter = m_MEDummyAttackInfoList.begin(); iter != m_MEDummyAttackInfoList.end(); ++iter )
	{
		ME2DummyAttackInfo Info = *iter;

		//발사 타임 조정
		Info.m_dwFireTime += m_dwFireStartTime;
		m_CurrAttackList.push_back( Info );
	}
}

bool ioMoveAttack4Dummy::IsActivateWeapon()
{
	if( m_State == DCS_DELAY )
		return false;

	return true;
}

void ioMoveAttack4Dummy::SetMovePosition( const D3DXVECTOR3& vTargetPos )
{
	m_vTargetPos = vTargetPos;

	D3DXVECTOR3 vPos = GetWorldPosition();

	vPos = m_vTargetPos - vPos;

	m_fNeedMoveLength = D3DXVec3Length( &vPos );
	m_MoveState = MS_MOVE_ATTACK;
}

void ioMoveAttack4Dummy::IncreaseComboCount() 
{ 
	//if( m_iCurCombo  )
	m_iCurCombo++; 
}

void ioMoveAttack4Dummy::SetState( DummyCharState eCharState ) 
{ 
	m_State = eCharState; 
}