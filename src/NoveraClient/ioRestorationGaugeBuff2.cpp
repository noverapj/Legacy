

#include "stdafx.h"

#include "ioBaseChar.h"

#include "ioRestorationGaugeBuff2.h"

ioRestorationGaugeBuff2::ioRestorationGaugeBuff2()
{
}

ioRestorationGaugeBuff2::ioRestorationGaugeBuff2( const ioRestorationGaugeBuff2 &rhs )
: ioBuff( rhs ),
  m_fRestorationGaugeRate( rhs.m_fRestorationGaugeRate )
{
}

ioRestorationGaugeBuff2::~ioRestorationGaugeBuff2()
{
}

void ioRestorationGaugeBuff2::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	m_fRestorationGaugeRate = rkLoader.LoadFloat_e( "restoration_gauge_rate", 0.0f );
}

ioBuff* ioRestorationGaugeBuff2::Clone()
{
	return new ioRestorationGaugeBuff2( *this );
}

void ioRestorationGaugeBuff2::StartBuff( ioBaseChar *pOwner )
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
		m_dwBuffStartTime += m_dwDelayDuration;

	m_dwBuffEndTime = m_dwBuffStartTime + GetBuffDuration();
	m_dwCheckTime = dwCurTime;
	
	m_bLive = true;

	if( m_iOperationType != OT_DELAY )
	{
		SetAction();
	}
}

bool ioRestorationGaugeBuff2::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	m_pOwner = pOwner;
	DWORD dwGlobalTimeGap = g_GlobalTimer.CalcGlobalGapTime( dwGlobalTime );

	if( m_iOperationType != OT_SWITCH )
	{
		if( dwGlobalTimeGap > dwGapTime )
			return false;

		dwGapTime -= dwGlobalTimeGap;
		m_dwBuffEndTime = FRAMEGETTIME() + dwGapTime;
		m_dwBuffStartTime = m_dwBuffEndTime - GetBuffDuration();
	}

	m_dwCheckTime = FRAMEGETTIME();
	m_bLive = true;

	if( m_iOperationType != OT_DELAY )
	{
		SetAction();
	}

	return true;
}

void ioRestorationGaugeBuff2::ProcessBuff( float fTimePerSec )
{
	switch( m_iOperationType )
	{
	case OT_ACTIVE:
	case OT_PASSIVE:
	case OT_SWITCH:
		SetReserveEndBuff();
		break;
	case OT_DELAY:
		ProcessDelay( fTimePerSec );
		break;
	}
}

void ioRestorationGaugeBuff2::SetAction()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
	{
		SetReserveEndBuff();
		return;
	}

	CheckInvisibleWeapon( false );
	CheckInvisibleCloak( false );
	CheckInvisibleHelmet( false );
	CheckChangeColor( true );

	CheckChangeSkill( true );

	CheckBuffDesc();

	if( m_CreatorLookType != CLT_NONE && !m_CreateChar.IsEmpty() )
	{
		if( m_CreateChar != pOwner->GetCharName() )
			pOwner->RotateByBuffCollision( m_CreateChar, m_CreatorLookType );
	}

	pOwner->AttachEffect( m_BuffEffect, GetEffectEventOwner() );

	m_bBuffStart = true;

	pOwner->SetRestorationGaugeByRate( m_fRestorationGaugeRate );
}

void ioRestorationGaugeBuff2::ProcessDelay( float fTimePerSec )
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

	if( m_bBuffStart )
	{
		SetReserveEndBuff();
		return;
	}
}


