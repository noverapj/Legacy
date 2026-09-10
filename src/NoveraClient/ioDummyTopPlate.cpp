#include "StdAfx.h"
#include "ioDummyTopPlate.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

ioDummyTopPlate::ioDummyTopPlate( ioEntityGroup *pGrp, ioPlayMode *pMode )
	: ioDummyChar( pGrp, pMode )
{
	m_MoveState = MS_JUMP;
}


ioDummyTopPlate::~ioDummyTopPlate(void)
{
}

void ioDummyTopPlate::LoadProperty( ioINILoader &rkLoader )
{
	ioDummyChar::LoadProperty( rkLoader );

	LoadInitProperty( rkLoader );
}

void ioDummyTopPlate::LoadInitProperty( ioINILoader &rkLoader )
{
	m_nAttackIndex = rkLoader.LoadInt_e( "destroy_attack_type", NO_WEAPON_ATTRIBUTE );

	m_fFloatingPower = rkLoader.LoadFloat_e( "floating_power", 0.0f );
	m_fFloatingGravityRate = rkLoader.LoadFloat_e( "floating_gravity_rate", 0.0f );

	m_fJumpPower = rkLoader.LoadFloat_e( "jump_power", 0.0f );
	m_fJumpGravityRate = rkLoader.LoadFloat_e( "jump_gravity_rate", 0.0f );
}

void ioDummyTopPlate::ProcessState( float fTimePerSec, DWORD dwPreTime )
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
	}

	if( m_dwLifeTime && (DWORD)(m_dwStartTime + m_dwLifeTime) < FRAMEGETTIME() && m_State != DCS_ATTACK )
		SendDieState( true );

	//ProcessPos( fTimePerSec );	
	ProcessEffectUpdate( fTimePerSec );
}

void ioDummyTopPlate::ProcessStartState( float fTimePerSec )
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

void ioDummyTopPlate::ProcessDelayState( float fTimePerSec )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwDelayStartTime > 0 && m_dwDelayStartTime + m_dwDelayDuration < dwCurTime )
	{
		m_dwDelayStartTime = 0;
		SetMoveState();
		SetAttachWeapon();
	}
}

void ioDummyTopPlate::ProcessMoveState( float fTimePerSec )
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

void ioDummyTopPlate::SetDieState( bool bEffect, bool bWoundedDie /*= false */ )
{
	m_MoveState = MS_JUMP;
	ioDummyChar::SetDieState( bEffect, bWoundedDie );
	FireWeapon();
}

void ioDummyTopPlate::CreateWeapon( int nAttackIndex, const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vDir )
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

void ioDummyTopPlate::FireWeapon()
{
	if ( m_nAttackIndex == NO_WEAPON_ATTRIBUTE )
		return;

	D3DXVECTOR3 vDir = GetWorldOrientation() * D3DXVECTOR3(0.0f,0.0f,-FLOAT1);
	D3DXVec3Normalize( &vDir, &vDir);
	D3DXVECTOR3 vPos = GetWorldPosition();

	CreateWeapon( m_nAttackIndex, vPos, vDir );
}
