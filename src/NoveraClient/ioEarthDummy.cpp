#include "StdAfx.h"
#include "ioEarthDummy.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

ioEarthDummy::ioEarthDummy( ioEntityGroup *pGrp, ioPlayMode *pMode )
	: ioDummyChar( pGrp, pMode )
{
	m_MoveState = MS_JUMP;
	m_nAttackCnt = 0;
}


ioEarthDummy::~ioEarthDummy(void)
{
}

void ioEarthDummy::LoadProperty( ioINILoader &rkLoader )
{
	ioDummyChar::LoadProperty( rkLoader );

	LoadInitProperty( rkLoader );
}

void ioEarthDummy::LoadInitProperty( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH] = "";

	m_nAttackIndex = rkLoader.LoadInt_e( "destroy_attack_type", NO_WEAPON_ATTRIBUTE );

	m_fFloatingPower = rkLoader.LoadFloat_e( "floating_power", 0.0f );
	m_fFloatingGravityRate = rkLoader.LoadFloat_e( "floating_gravity_rate", 0.0f );

	m_fJumpPower = rkLoader.LoadFloat_e( "jump_power", 0.0f );
	m_fJumpGravityRate = rkLoader.LoadFloat_e( "jump_gravity_rate", 0.0f );

	int iAttackCnt = rkLoader.LoadInt_e( "dummy_attack_cnt", 0 );

	for( int i=0 ; i<iAttackCnt ; i++ )
	{
		wsprintf_e( szBuf, "dummy_attack%d_code", i+1 );
		int iAttackCode = rkLoader.LoadInt( szBuf, 0 );
		m_vAttackCode.push_back( iAttackCode );
	}
}

void ioEarthDummy::ProcessState( float fTimePerSec, DWORD dwPreTime )
{
	switch( m_State )
	{
	case DCS_START:
		ProcessStartState( fTimePerSec );
		break;
	case DCS_DELAY:
		ProcessDelayState( fTimePerSec );
		break;
	case DCS_MOVE:
		ProcessMoveState( fTimePerSec );
		break;
	case DCS_ATTACK:
		ProcessAttackState( fTimePerSec, dwPreTime );
		break;
	case DCS_DIE:
		ProcessDieState( fTimePerSec );
		break;
	}

	if( m_dwLifeTime && (DWORD)(m_dwStartTime + m_dwLifeTime) < FRAMEGETTIME() && m_State != DCS_ATTACK )
		SendDieState( true );

	//ProcessPos( fTimePerSec );	
	ProcessEffectUpdate( fTimePerSec );
}

void ioEarthDummy::ProcessStartState( float fTimePerSec )
{
	DWORD dwCurTime = FRAMEGETTIME();
	ioBaseChar* pOwner = GetOwner();
	if( !pOwner ) return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage ) return;

	D3DXVECTOR3 vPos = GetWorldPosition();
	float fMapHeight = pStage->GetMapHeight( vPos.x, vPos.z );
	if ( vPos.y < fMapHeight )
	{
		vPos.y = fMapHeight;
		SetWorldPosition( vPos );
	}


	if( m_dwStartAniEndTime == 0 || m_dwStartAniEndTime < dwCurTime )
	{
		SetDelayState( false );
		return;
	}
}

void ioEarthDummy::ProcessDelayState( float fTimePerSec )
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( (int)m_vAttackCode.size() <= m_nAttackCnt )
	{
		SetDieState( true );
		return;
	}

	if( m_dwDelayStartTime > 0 && m_dwDelayStartTime + m_dwDelayDuration < dwCurTime )
	{
		m_dwDelayStartTime = 0;
		m_iReserveAttackCode = m_vAttackCode[m_nAttackCnt++];
		SetAttackState();
		//SetAttachWeapon();
	}
}

void ioEarthDummy::ProcessMoveState( float fTimePerSec )
{
	DWORD dwCurTime = FRAMEGETTIME();

	D3DXVECTOR3 vPrePos = GetWorldPosition();
	D3DXVECTOR3 vCurPos = vPrePos;
	float fMapHeight = GetBottomHeight();
	D3DXVECTOR3 vCurDir = GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vCurDir, &vCurDir );

	float fCurGravity = m_pModeParent->GetCreator()->GetRoomGravity() * fTimePerSec;

	m_fGravityAmt = 0.0f;
	switch( m_MoveState )
	{
	case MS_JUMP:
		{
			vCurPos.y += ( m_fJumpPower * fTimePerSec );
			m_fJumpPower -= fCurGravity * m_fJumpGravityRate;
			if ( m_fJumpPower < 0.0f )
			{
				m_MoveState = MS_SLIDING;
			}
		}
		break;
	case MS_SLIDING:
		{
			vCurPos += vCurDir * ( m_fFloatingPower * fTimePerSec  );
			vCurPos.y -= fCurGravity * m_fFloatingGravityRate;

			if( vCurPos.y <= fMapHeight )
			{
				vCurPos.y = fMapHeight;
				m_fGravityAmt = 0.0f;
				m_fCurJumpPowerAmt = 0.0f;
				SetDieState( true );
			}
		}
		break;
	}

	SetWorldPosition( vCurPos );
}

void ioEarthDummy::SetDieState( bool bEffect, bool bWoundedDie /*= false */ )
{
	m_MoveState = MS_JUMP;
	ioDummyChar::SetDieState( bEffect, bWoundedDie );
	FireWeapon();
}

void ioEarthDummy::CreateWeapon( int nAttackIndex, const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vDir )
{
	ioBaseChar *pOwner = GetOwner();

	if ( !pOwner )
		return;

	m_dwBaseWeaponIndex = pOwner->GetWeaponIndexBase() + 1;

	FireTime kFireTime;
	kFireTime.dwStart = FRAMEGETTIME();

	kFireTime.iAttributeIdx = nAttackIndex;
	kFireTime.iResistanceIdx = 0;
	kFireTime.szWoundedAni  = "";
	kFireTime.dwWoundedAniDuration = 0;
	kFireTime.bLoopWoundedAni = false;
	kFireTime.dwWeaponIdx = m_dwBaseWeaponIndex;
	kFireTime.eFireTimeType = FTT_DUMMY_CHAR_ATTACK;

	pOwner->SetWeaponIndexBase( m_dwBaseWeaponIndex );

	ioWeapon *pWeapon = pOwner->ExtendFireExplicit( kFireTime, vPos, vDir, "" );
	if ( pWeapon )
	{
		pWeapon->SetPosition( vPos );
	}	
}

void ioEarthDummy::FireWeapon()
{
	if ( m_nAttackIndex == NO_WEAPON_ATTRIBUTE )
		return;

	D3DXVECTOR3 vDir = GetWorldOrientation() * D3DXVECTOR3(0.0f,0.0f,-FLOAT1);
	D3DXVec3Normalize( &vDir, &vDir);
	D3DXVECTOR3 vPos = GetWorldPosition();

	CreateWeapon( m_nAttackIndex, vPos, vDir );
}
