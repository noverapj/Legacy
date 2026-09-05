

#include "stdafx.h"

#include "ioIceBuff.h"

#include "ioBaseChar.h"


ioIceBuff::ioIceBuff()
{
}

ioIceBuff::ioIceBuff( const ioIceBuff &rhs )
: ioBuff( rhs ),
 m_BoundWeaponInfo( rhs.m_BoundWeaponInfo ),
 m_fBoundGravityAmt( rhs.m_fBoundGravityAmt )
{
}

ioIceBuff::~ioIceBuff()
{
}

void ioIceBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	m_BoundWeaponInfo.m_iWeaponIdx = rkLoader.LoadInt_e( "bound_attribute_index", 0 );
	m_BoundWeaponInfo.m_iResistanceIdx = rkLoader.LoadInt_e( "bound_attribute_resistance", 0 );

	rkLoader.LoadString_e( "bound_attribute_wound_ani", "", szBuf, MAX_PATH );
	m_BoundWeaponInfo.m_WoundedAnimation = szBuf;

	m_BoundWeaponInfo.m_dwWoundedDuration = rkLoader.LoadInt_e( "bound_attribute_wound_duration", 0 );
	m_BoundWeaponInfo.m_bWoundedLoopAni = rkLoader.LoadBool_e( "bound_attribute_wound_loop_ani", false );

	m_fBoundGravityAmt = rkLoader.LoadFloat_e( "bound_enable_gravity", 0.0f );
}

ioBuff* ioIceBuff::Clone()
{
	return new ioIceBuff( *this );
}

void ioIceBuff::StartBuff( ioBaseChar *pOwner )
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

bool ioIceBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
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

void ioIceBuff::ProcessBuff( float fTimePerSec )
{
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
		{
			if( m_pOwner && m_pOwner->GetState() != CS_ICE_STATE )
			{
				SetReserveEndBuff();
				return;
			}

			if( CheckEnableDelayState() )
				return;

			if( m_pOwner && m_pOwner->IsFloating() )
				return;

			if( m_pOwner && !m_pOwner->IsCheckIceBound() )
				CheckCallingProcess();

			if( GetBuffDuration() == 0 )
				return;

			if( m_dwBuffEndTime <= FRAMEGETTIME() )
			{
				SetReserveEndBuff();
				return;
			}
		}
		break;
	case OT_PASSIVE:
		CheckCallingAreaWeapon();
		break;
	}
}

void ioIceBuff::EndBuff()
{
	if( m_pOwner )
	{
		m_pOwner->GetGroup()->ResumeAllPlayingAnimation();

		if( m_pOwner->GetState() == CS_ICE_STATE )
		{
			if( !m_pOwner->GetBlowStopMotionState() )
			{
				m_pOwner->SetState( CS_DELAY, m_bSendDelayNetWork );
				m_pOwner->SetDashFailStart();
			}
			else
			{
				BlowWoundAniInfo kInfo;
				m_pOwner->SetBlowWoundedState( false, kInfo );
			}
		}
	}

	ioBuff::EndBuff();
}

void ioIceBuff::SetAction()
{
	CheckInvisibleWeapon( false );
	CheckInvisibleCloak( false );
	CheckInvisibleHelmet( false );
	
	CheckChangeSkill( true );
	CheckChangeColor( true );
	CheckBuffDesc();

	if( m_CreatorLookType != CLT_NONE && !m_CreateChar.IsEmpty() )
	{
		if( m_CreateChar != m_pOwner->GetCharName() )
			m_pOwner->RotateByBuffCollision( m_CreateChar, m_CreatorLookType );
	}

	m_pOwner->AttachEffect( m_BuffEffect, GetEffectEventOwner() );

	if( !m_LoopSound.IsEmpty() )
	{
		m_LoopSoundID = m_pOwner->PlaySound( m_LoopSound, true );
		m_bLoopSoundPlayed = true;
	}

	if( m_pOwner->GetState() != CS_DIE &&
		m_pOwner->GetState() != CS_VIEW &&
		m_pOwner->GetState() != CS_OBSERVER &&
		m_pOwner->GetState() != CS_LOADING )
	{
		m_pOwner->SetIceState( m_Name, m_CreateChar, m_BoundWeaponInfo, m_fBoundGravityAmt );
	}

	m_bBuffStart = true;
}

void ioIceBuff::ProcessDelay( float fTimePerSec )
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