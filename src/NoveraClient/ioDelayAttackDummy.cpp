#include "StdAfx.h"
#include "ioDelayAttackDummy.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

ioDelayAttackDummy::ioDelayAttackDummy( ioEntityGroup *pGrp, ioPlayMode *pMode )
	: ioDummyChar( pGrp, pMode )
{
	
}


ioDelayAttackDummy::~ioDelayAttackDummy(void)
{
}

void ioDelayAttackDummy::LoadProperty( ioINILoader &rkLoader )
{
	ioDummyChar::LoadProperty( rkLoader );

	LoadInitProperty( rkLoader );
}

void ioDelayAttackDummy::LoadInitProperty( ioINILoader &rkLoader )
{
	m_nAttackIndex = rkLoader.LoadInt_e( "destroy_attack_type", NO_WEAPON_ATTRIBUTE );
	m_dwAttackEndDuration = (DWORD)rkLoader.LoadInt_e( "attack_end_duration", 0 );
}

void ioDelayAttackDummy::ProcessState( float fTimePerSec, DWORD dwPreTime )
{
	switch( m_State )
	{
	case DCS_START:
		ProcessStartState( fTimePerSec );
		break;
	case DCS_DELAY:
		ProcessDelayState( fTimePerSec );
		break;
	case DCS_ATTACK:
		CheckReservedSliding();
		CheckExtraAniJump();
		ProcessAttackState( fTimePerSec, dwPreTime );
		break;
	}

	if( m_dwLifeTime && (DWORD)(m_dwStartTime + m_dwLifeTime) < FRAMEGETTIME() && m_State != DCS_ATTACK )
		SendDieState( true );

	ProcessPos( fTimePerSec );
	ProcessEffectUpdate( fTimePerSec );
}

void ioDelayAttackDummy::ProcessStartState( float fTimePerSec )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwStartAniEndTime == 0 || m_dwStartAniEndTime < dwCurTime )
	{
		SetDelayState( false );
		return;
	}
}

void ioDelayAttackDummy::ProcessDelayState( float fTimePerSec )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwDelayStartTime > 0 && m_dwDelayStartTime + m_dwDelayDuration < dwCurTime )
	{
		m_dwDelayStartTime = 0;
		m_iReserveAttackCode = m_iTraceNormalAttack;
		SetAttackState();
	}
}

void ioDelayAttackDummy::ProcessAttackState( float fTimePerSec, DWORD dwPreTime )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	D3DXVECTOR3 vCurDir = GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vCurDir, &vCurDir );

	D3DXVECTOR3 vCurPos = GetMidPositionByRate();

	DWORD dwLastFireTime = dwPreTime;
	FireTimeList::iterator iter = m_vAttackFireTimeList.begin();
	while( iter != m_vAttackFireTimeList.end() )
	{
		const FireTime &rkFire = *iter;

		DWORD dwFireTime = rkFire.dwStart;
		if( COMPARE( dwFireTime, dwPreTime, dwCurTime ) )
		{
			if( rkFire.iAttributeIdx != NO_WEAPON_ATTRIBUTE )
			{
				ioWeapon *pWeapon = NULL;
				pWeapon = pOwner->ExtendFireExplicit( rkFire, vCurPos, vCurDir, "" );
			}

			iter = m_vAttackFireTimeList.erase( iter );
		}
		else
		{
			++iter;
		}
	}

	//
	if( m_dwAttackAniEndTime == 0 || m_dwAttackAniEndTime + m_dwAttackEndDuration < dwCurTime )
	{
		SetDieState( true );
		return;
	}
}

void ioDelayAttackDummy::SetDieState( bool bEffect, bool bWoundedDie /*= false */ )
{
	ioDummyChar::SetDieState( bEffect, bWoundedDie );
	FireWeapon();
}

void ioDelayAttackDummy::CreateWeapon( int nAttackIndex, const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vDir )
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

void ioDelayAttackDummy::FireWeapon()
{
	if ( m_nAttackIndex == NO_WEAPON_ATTRIBUTE )
		return;

	D3DXVECTOR3 vDir = GetWorldOrientation() * D3DXVECTOR3(0.0f,0.0f,-FLOAT1);
	D3DXVec3Normalize( &vDir, &vDir);
	D3DXVECTOR3 vPos = GetWorldPosition();

	CreateWeapon( m_nAttackIndex, vPos, vDir );
}
