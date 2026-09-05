

#include "stdafx.h"

#include "ioWoundColorChangeBuff.h"

#include "ioBaseChar.h"



ioWoundColorChangeBuff::ioWoundColorChangeBuff()
{
	m_dwWoundStartTime = 0;
}

ioWoundColorChangeBuff::ioWoundColorChangeBuff( const ioWoundColorChangeBuff &rhs )
: ioBuff( rhs ),
 m_dwWoundEffectDuration( rhs.m_dwWoundEffectDuration )
{
	m_dwWoundStartTime = 0;
}

ioWoundColorChangeBuff::~ioWoundColorChangeBuff()
{
}

void ioWoundColorChangeBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	m_dwWoundEffectDuration = (DWORD)rkLoader.LoadInt_e( "wound_effect_duration", 0 );
}

ioBuff* ioWoundColorChangeBuff::Clone()
{
	return new ioWoundColorChangeBuff( *this );
}

void ioWoundColorChangeBuff::StartBuff( ioBaseChar *pOwner )
{
#ifndef SHIPPING 
	if( Setting::ShowExtraInfo() )
	{
		LOG.PrintTimeAndLog( 0, "BuffCheck - %s, StartTime: %d", m_Name.c_str(), FRAMEGETTIME() );
	}
#endif

	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	m_pOwner = pOwner;
	m_OwnerName = pOwner->GetCharName();

	m_dwBuffStartTime = dwCurTime;
	if( m_iOperationType == OT_DELAY )
	{
		m_dwBuffStartTime += m_dwDelayDuration;
	}

	DWORD dwCurDuration = GetBuffDuration() * pOwner->GetMemberBalanceRateForStateProtectionTime( GetTypeID() );
	m_dwBuffEndTime = m_dwBuffStartTime + dwCurDuration;
	m_dwCheckTime = dwCurTime;
	
	m_bLive = true;
	m_dwWoundStartTime = 0;

	if( m_iOperationType != OT_DELAY )
	{
		SetAction();
	}
}

bool ioWoundColorChangeBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	m_pOwner = pOwner;
	DWORD dwGlobalTimeGap = g_GlobalTimer.CalcGlobalGapTime( dwGlobalTime );

	if( m_iOperationType != OT_SWITCH )
	{
		if( dwGlobalTimeGap > dwGapTime )
			return false;

		dwGapTime -= dwGlobalTimeGap;
		m_dwBuffEndTime = FRAMEGETTIME() + dwGapTime;

		DWORD dwCurDuration = GetBuffDuration() * pOwner->GetMemberBalanceRateForStateProtectionTime( GetTypeID() );
		m_dwBuffStartTime = m_dwBuffEndTime - dwCurDuration;
	}

	m_dwCheckTime = FRAMEGETTIME();
	m_bLive = true;
	m_dwWoundStartTime = 0;

	if( m_iOperationType != OT_DELAY )
	{
		SetAction();
	}

	return true;
}

void ioWoundColorChangeBuff::SetAction()
{
	CheckInvisibleWeapon( false );
	CheckInvisibleCloak( false );
	CheckInvisibleHelmet( false );

	CheckChangeSkill( true );

	CheckBuffDesc();

	if( m_CreatorLookType != CLT_NONE && !m_CreateChar.IsEmpty() )
	{
		if( m_CreateChar != m_pOwner->GetCharName() )
			m_pOwner->RotateByBuffCollision( m_CreateChar, m_CreatorLookType );
	}

	if( !m_LoopSound.IsEmpty() )
	{
		m_LoopSoundID = m_pOwner->PlaySound( m_LoopSound, true );
		m_bLoopSoundPlayed = true;
	}

	m_bBuffStart = true;
}

void ioWoundColorChangeBuff::EndBuff()
{
	ioBuff::EndBuff();
}

void ioWoundColorChangeBuff::ProcessBuff( float fTimePerSec )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_iOperationType )
	{
	case OT_SWITCH:
		if( !CheckSwitchType( fTimePerSec ) )
		{
			SetReserveEndBuff();
			return;
		}

		CheckCallingProcess();
		break;
	case OT_DELAY:
		ProcessDelay( fTimePerSec );
		break;
	case OT_ACTIVE:
		ioBuff::ProcessBuff( fTimePerSec );
		break;
	case OT_PASSIVE:
		CheckCallingAreaWeapon();
		break;
	}

	//
	if( m_dwWoundStartTime > 0 && m_dwWoundEffectDuration > 0 )
	{
		if( m_dwWoundStartTime+m_dwWoundEffectDuration < dwCurTime )
		{
			m_dwWoundStartTime = 0;

			CheckChangeColor( false );
			m_pOwner->EndEffect( m_BuffEffect, false );
		}
	}
}

void ioWoundColorChangeBuff::ProcessDelay( float fTimePerSec )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( CheckEnableDelayState() )
		return;

	if( GetBuffDuration() == 0 )
	{
		if( !m_bBuffStart && m_dwBuffStartTime < dwCurTime )
			SetAction();
		return;
	}

	if( !m_bBuffStart && m_dwBuffStartTime < dwCurTime )
		SetAction();

	if( m_dwBuffEndTime <= dwCurTime )
	{
		SetReserveEndBuff();
		return;
	}

	CheckCallingProcess();
}

void ioWoundColorChangeBuff::ApplyWoundState()
{
	DWORD dwCurTime = FRAMEGETTIME();
	
	m_dwWoundStartTime = dwCurTime;

	CheckChangeColor( true );

	m_pOwner->EndEffect( m_BuffEffect, false );
	m_pOwner->AttachEffect( m_BuffEffect, GetEffectEventOwner() );
}



