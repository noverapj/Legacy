

#include "stdafx.h"

#include "ioCurseBuff.h"

#include "ioBaseChar.h"



ioCurseBuff::ioCurseBuff()
{
}

ioCurseBuff::ioCurseBuff( const ioCurseBuff &rhs )
: ioBuff( rhs ),
m_CurseRollingAni( rhs.m_CurseRollingAni ),
m_fCurseRollingAniRate( rhs.m_fCurseRollingAniRate ),
m_vCurseForceInfoList( rhs.m_vCurseForceInfoList )
{
	m_fOutLineR = rhs.m_fOutLineR;
	m_fOutLineG = rhs.m_fOutLineG;
	m_fOutLineB = rhs.m_fOutLineB;
	m_fOutLineAlpha = rhs.m_fOutLineAlpha;
	m_fOutLineThickness = rhs.m_fOutLineThickness;
	m_bEnableOutLine = rhs.m_bEnableOutLine;
	m_bDisableDownState = rhs.m_bDisableDownState;
}

ioCurseBuff::~ioCurseBuff()
{
}

void ioCurseBuff::LoadProperty( ioINILoader &rkLoader )
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

	rkLoader.LoadString_e( "curse_rolling_ani", "", szBuf, MAX_PATH );
	m_CurseRollingAni = szBuf;
	m_fCurseRollingAniRate = rkLoader.LoadFloat_e( "curse_rolling_ani_rate", FLOAT1 );

	m_vCurseForceInfoList.clear();
	int iForceCnt = rkLoader.LoadInt_e( "curse_force_cnt", 0 );
	if( iForceCnt > 0 )
	{
		m_vCurseForceInfoList.reserve( iForceCnt );
	}

	for( int i=0; i < iForceCnt; ++i )
	{
		ForceInfo kInfo;

		wsprintf_e( szBuf, "curse_force_move%d", i+1 );
		kInfo.m_fForceMovePower = rkLoader.LoadFloat( szBuf, 0.0f );

		wsprintf_e( szBuf, "curse_force_friction%d", i+1 );
		kInfo.m_fForceMoveFriction = rkLoader.LoadFloat( szBuf, FLOAT1 );

		m_vCurseForceInfoList.push_back( kInfo );
	}
}

ioBuff* ioCurseBuff::Clone()
{
	return new ioCurseBuff( *this );
}

void ioCurseBuff::StartBuff( ioBaseChar *pOwner )
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

bool ioCurseBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
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

void ioCurseBuff::SetExtraDurationByGrowth( float fDuration )
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

void ioCurseBuff::SetAction()
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
	m_pOwner->SetCurseRollingInfo( m_CurseRollingAni, m_fCurseRollingAniRate, m_vCurseForceInfoList );

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

void ioCurseBuff::EndBuff()
{
	if( m_bEnableOutLine )
	{
		m_pOwner->RestoreOutLineChange( COT_BUFF_PROTECTION );
	}

	ioBuff::EndBuff();
}

void ioCurseBuff::ProcessBuff( float fTimePerSec )
{
	CheckOwnerStateCheck();

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

void ioCurseBuff::ProcessDelay( float fTimePerSec )
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

void ioCurseBuff::CheckOwnerStateCheck()
{
	if( !m_pOwner ) return;

	CharState eState = m_pOwner->GetState();

	if( m_bDisableDownState )
	{
		if( eState == CS_FROZEN && m_pOwner->GetBlowFrozenState() )
			SetReserveEndBuff();
		else if( eState == CS_BLOW_WOUNDED && !m_pOwner->IsFloatingState() )
			SetReserveEndBuff();
		else if( eState == CS_BOUND_BLOW_WOUNDED && !m_pOwner->IsFloatingState() )
			SetReserveEndBuff();
		else if( eState == CS_ETC_STATE && m_pOwner->IsApplyDownState(false) )
			SetReserveEndBuff();
		else if( eState == CS_STOP_MOTION && m_pOwner->GetBlowStopMotionState() )
			SetReserveEndBuff();
		else if( eState == CS_DROP_ZONE_DOWN )
			SetReserveEndBuff();
		else if( eState == CS_ICE_STATE && m_pOwner->GetBlowStopMotionState() )
			SetReserveEndBuff();
	}
}

