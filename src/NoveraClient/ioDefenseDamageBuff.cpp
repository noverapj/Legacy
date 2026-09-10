
#include "stdafx.h"

#include "ioDefenseDamageBuff.h"

#include "ioBaseChar.h"


ioDefenseDamageBuff::ioDefenseDamageBuff()
{
}

ioDefenseDamageBuff::ioDefenseDamageBuff( const ioDefenseDamageBuff &rhs )
: ioBuff( rhs ),
m_fDefenseMaxRate( rhs.m_fDefenseMaxRate ),
m_bDisableDownState( rhs.m_bDisableDownState ),
m_DefenseType( rhs.m_DefenseType )
{
	m_fOutLineR = rhs.m_fOutLineR;
	m_fOutLineG = rhs.m_fOutLineG;
	m_fOutLineB = rhs.m_fOutLineB;
	m_fOutLineAlpha = rhs.m_fOutLineAlpha;
	m_fOutLineThickness = rhs.m_fOutLineThickness;
	m_bEnableOutLine = rhs.m_bEnableOutLine;
}

ioDefenseDamageBuff::~ioDefenseDamageBuff()
{
}

void ioDefenseDamageBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	m_DefenseType = (DefenseType)rkLoader.LoadInt_e( "defense_rate_type", DT_SKILL_GAUGE );

	m_fDefenseMaxRate = rkLoader.LoadFloat_e( "denfense_max_rate", 0.0f );
	
	m_bEnableOutLine = rkLoader.LoadBool_e( "outline_enable", false );
	m_fOutLineR = rkLoader.LoadFloat_e( "outline_color_red", FLOAT1 );
	m_fOutLineG = rkLoader.LoadFloat_e( "outline_color_green", FLOAT1 );
	m_fOutLineB = rkLoader.LoadFloat_e( "outline_color_blue", 0.0f );
	m_fOutLineAlpha = rkLoader.LoadFloat_e( "outline_color_alpha", FLOAT05 );
	m_fOutLineThickness = rkLoader.LoadFloat_e( "outline_thickness", 2.0f );

	m_bDisableDownState = rkLoader.LoadBool_e( "disable_down_state", false );
}

ioBuff* ioDefenseDamageBuff::Clone()
{
	return new ioDefenseDamageBuff( *this );
}

void ioDefenseDamageBuff::StartBuff( ioBaseChar *pOwner )
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

	DWORD dwCurDuration = GetBuffDuration() * pOwner->GetMemberBalanceRateForStateProtectionTime( GetTypeID() );
	m_dwBuffEndTime = m_dwBuffStartTime + dwCurDuration;
	m_dwCheckTime = dwCurTime;
	
	m_bLive = true;

	if( m_iOperationType != OT_DELAY )
	{
		SetAction();
	}
}

bool ioDefenseDamageBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
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

	m_iCurActiveCount = m_iMaxActiveCount;

	if( m_iOperationType != OT_DELAY )
	{
		SetAction();
	}

	return true;
}

void ioDefenseDamageBuff::EndBuff()
{
	if( m_bEnableOutLine )
	{
		m_pOwner->RestoreOutLineChange( COT_BUFF_PROTECTION );
	}

	ioBuff::EndBuff();
}

void ioDefenseDamageBuff::ProcessBuff( float fTimePerSec )
{
	if( CheckOwnerStateCheck() )
		return;

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
}

void ioDefenseDamageBuff::SetAction()
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

	if( m_bEnableOutLine )
	{
		D3DCOLORVALUE kColor;
		Help::InitColorValue( kColor, m_fOutLineR, m_fOutLineG, m_fOutLineB, m_fOutLineAlpha );
		m_pOwner->ApplyOutLineChange( COT_BUFF_PROTECTION, kColor, m_fOutLineThickness );
	}

	m_bBuffStart = true;
}

void ioDefenseDamageBuff::ProcessDelay( float fTimePerSec )
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

float ioDefenseDamageBuff::GetDefenseRate()
{
	float fCurDefenseRate = 0.0f;

	ioBaseChar *pCreator = GetCreator();
	ioItem *pOwnerItem = GetOwnerItem();
	if( pCreator && pOwnerItem )
	{
		float fMax = pOwnerItem->GetMaxSkillGauge( pCreator->GetCharName() );
		float fCur = pOwnerItem->GetCurSkillGuage();

		if( fMax > 0.0f )
		{
			float fRate = fCur / fMax;
			fRate = max( 0.0f, min(fRate, FLOAT1) );
			if( fRate > 0.0f )
			{
				float fBaseRate = max( 0.0f, min(m_fDefenseMaxRate, FLOAT1) );
				float fGapRate = (1.0f-fBaseRate);
				
				fCurDefenseRate = FLOAT1 - (fGapRate*fRate);
				fCurDefenseRate = max( 0.0f, min(fCurDefenseRate, FLOAT1) );
			}
		}
	}

	if( fCurDefenseRate > 0.0f )
		return fCurDefenseRate;

	return FLOAT1;
}

bool ioDefenseDamageBuff::CheckOwnerStateCheck()
{
	if( !m_pOwner ) return false;

	CharState eState = m_pOwner->GetState();

	if( m_bDisableDownState )
	{
		if( eState == CS_FROZEN && m_pOwner->GetBlowFrozenState() )
		{
			SetReserveEndBuff();
			return true;
		}
		else if( eState == CS_BLOW_WOUNDED && !m_pOwner->IsFloatingState() )
		{
			SetReserveEndBuff();
			return true;
		}
		else if( eState == CS_BOUND_BLOW_WOUNDED && !m_pOwner->IsFloatingState() )
		{
			SetReserveEndBuff();
			return true;
		}
		else if( eState == CS_ETC_STATE && m_pOwner->IsApplyDownState(false) )
		{
			SetReserveEndBuff();
			return true;
		}
		else if( eState == CS_STOP_MOTION && m_pOwner->GetBlowStopMotionState() )
		{
			SetReserveEndBuff();
			return true;
		}
		else if( eState == CS_DROP_ZONE_DOWN )
		{
			SetReserveEndBuff();
			return true;
		}
		else if( eState == CS_ICE_STATE && m_pOwner->GetBlowStopMotionState() )
		{
			SetReserveEndBuff();
			return true;
		}
	}

	return false;
}


