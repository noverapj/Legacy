

#include "stdafx.h"

#include "ioLandStunBuff.h"

#include "ioBaseChar.h"


ioLandStunBuff::ioLandStunBuff()
{
}

ioLandStunBuff::ioLandStunBuff( const ioLandStunBuff &rhs )
: ioBuff( rhs ),
 m_StunBuff( rhs.m_StunBuff )
{
	m_fOutLineR = rhs.m_fOutLineR;
	m_fOutLineG = rhs.m_fOutLineG;
	m_fOutLineB = rhs.m_fOutLineB;
	m_fOutLineAlpha = rhs.m_fOutLineAlpha;
	m_fOutLineThickness = rhs.m_fOutLineThickness;
	m_bEnableOutLine = rhs.m_bEnableOutLine;
	m_bDisableDownState = rhs.m_bDisableDownState;
}

ioLandStunBuff::~ioLandStunBuff()
{
}

void ioLandStunBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	m_bEnableOutLine = rkLoader.LoadBool_e( "outline_enable", false );
	m_fOutLineR = rkLoader.LoadFloat_e( "outline_color_red", FLOAT1 );
	m_fOutLineG = rkLoader.LoadFloat_e( "outline_color_green", FLOAT1 );
	m_fOutLineB = rkLoader.LoadFloat_e( "outline_color_blue", 0.0f );
	m_fOutLineAlpha = rkLoader.LoadFloat_e( "outline_color_alpha", FLOAT05 );
	m_fOutLineThickness = rkLoader.LoadFloat_e( "outline_thickness", 2.0f );
	m_bDisableDownState  = rkLoader.LoadBool_e( "disable_down_state", false );

	char szBuf[MAX_PATH] = "";
	rkLoader.LoadString_e( "stun_buff", "", szBuf, MAX_PATH );
	m_StunBuff = szBuf;
}

ioBuff* ioLandStunBuff::Clone()
{
	return new ioLandStunBuff( *this );
}

void ioLandStunBuff::StartBuff( ioBaseChar *pOwner )
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

	if( m_iOperationType != OT_DELAY )
	{
		SetAction();
	}
}

bool ioLandStunBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
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

	if( m_iOperationType != OT_DELAY )
	{
		SetAction();
	}

	return true;
}

void ioLandStunBuff::SetExtraDurationByGrowth( float fDuration )
{
	m_dwExtraDurationByGrowth = 0;

	if( m_bDisableGrowth )
		return;

	if( GetBuffDuration() == 0 ) return;
	if( fDuration == 0.0f ) return;

	m_dwExtraDurationByGrowth = (DWORD)fDuration;

	DWORD dwCurDuration = GetBuffDuration() * m_pOwner->GetMemberBalanceRateForStateProtectionTime( GetTypeID() );
	m_dwBuffEndTime = m_dwBuffStartTime + dwCurDuration + m_dwExtraDurationByGrowth;
}

void ioLandStunBuff::SetAction()
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

	m_pOwner->SetLandStunBuff( m_Name, m_StunBuff );
}

void ioLandStunBuff::EndBuff()
{
	ioBaseChar *pOwner = GetOwner();

	if( pOwner )
	{
		if( m_bEnableOutLine )
			pOwner->RestoreOutLineChange( COT_BUFF_PROTECTION );

		pOwner->RemoveLandStunBuff( m_Name );
	}

	ioBuff::EndBuff();
}

void ioLandStunBuff::ProcessBuff( float fTimePerSec )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
	{
		SetReserveEndBuff();
		return;
	}

	CheckOwnerStateCheck();

	switch( m_iOperationType )
	{
	case OT_SWITCH:
		if( !CheckSwitchType( fTimePerSec ) )
		{
			pOwner->CheckUseBuffSkill( m_Name, false );
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

void ioLandStunBuff::ProcessDelay( float fTimePerSec )
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

void ioLandStunBuff::CheckOwnerStateCheck()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return;

	bool bReserveEnd = false;
	CharState eState = pOwner->GetState();

	if( m_bDisableDownState )
	{
		if( eState == CS_FROZEN && pOwner->GetBlowFrozenState() )
			bReserveEnd = true;
		else if( eState == CS_BLOW_WOUNDED && !pOwner->IsFloatingState() )
			bReserveEnd = true;
		else if( eState == CS_BOUND_BLOW_WOUNDED && !pOwner->IsFloatingState() )
			bReserveEnd = true;
		else if( eState == CS_ETC_STATE && pOwner->IsApplyDownState(false) )
			bReserveEnd = true;
		else if( eState == CS_STOP_MOTION && pOwner->GetBlowStopMotionState() )
			bReserveEnd = true;
		else if( eState == CS_DROP_ZONE_DOWN )
			bReserveEnd = true;
		else if( eState == CS_ICE_STATE && pOwner->GetBlowStopMotionState() )
			bReserveEnd = true;
	}

	if( bReserveEnd )
	{
		pOwner->CheckUseBuffSkill( m_Name, false );
		SetReserveEndBuff();
	}
}

