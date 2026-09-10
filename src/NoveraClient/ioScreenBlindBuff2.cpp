

#include "stdafx.h"

#include "ioScreenBlindBuff2.h"

#include "ioBaseChar.h"


ioScreenBlindBuff2::ioScreenBlindBuff2()
{
}

ioScreenBlindBuff2::ioScreenBlindBuff2( const ioScreenBlindBuff2 &rhs )
: ioBuff( rhs ),
 m_ScreenBlindTime( rhs.m_ScreenBlindTime ),
 m_BuffBlind( rhs.m_BuffBlind ),
 m_fBlindRange( rhs.m_fBlindRange )
{
}

ioScreenBlindBuff2::~ioScreenBlindBuff2()
{
}

void ioScreenBlindBuff2::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";

	int iRed, iGreen, iBlue;
	iRed   = rkLoader.LoadInt_e( "buff_color_r", 0 );
	iGreen = rkLoader.LoadInt_e( "buff_color_g", 0 );
	iBlue  = rkLoader.LoadInt_e( "buff_color_b", 0 );
	
	m_BuffBlind.m_dwColor = D3DCOLOR_ARGB( 0, iRed, iGreen, iBlue );
	m_BuffBlind.m_iMaxAlpha = rkLoader.LoadInt_e( "buff_max_alpha", 0 );

	m_ScreenBlindTime.m_dwUpTime     = rkLoader.LoadInt_e( "screen_blind_up_time", 0 );
	m_ScreenBlindTime.m_dwCenterTime = max( 1, rkLoader.LoadInt_e( "screen_blind_center_time", 1 ) );
	m_ScreenBlindTime.m_dwDownTime   = max( 1, rkLoader.LoadInt_e( "screen_blind_down_time", 1 ) );

	m_fBlindRange = rkLoader.LoadFloat_e( "screen_blind_range", 0.0f );
}

ioBuff* ioScreenBlindBuff2::Clone()
{
	return new ioScreenBlindBuff2( *this );
}

void ioScreenBlindBuff2::StartBuff( ioBaseChar *pOwner )
{
	//ioBuff::StartBuff( pOwner );
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

	if( !m_bVisibleBuffEffectOnlyOwner )
		m_pOwner->AttachEffect( m_BuffEffect, GetEffectEventOwner(), !m_bVisibleBuffEffectIndependence );

	if( m_iOperationType != OT_DELAY )
	{
		SetAction();
	}
}

bool ioScreenBlindBuff2::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

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

void ioScreenBlindBuff2::ProcessBuff( float fTimePerSec )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
	{
		SetReserveEndBuff();
		return;
	}

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

void ioScreenBlindBuff2::EndBuff()
{
#ifndef SHIPPING 
	if( Setting::ShowExtraInfo() )
	{
		if( m_iBuffType != BT_CAMERA_BUFF )
			LOG.PrintTimeAndLog( 0, "BuffCheck - %s, EndTime: %d", m_Name.c_str(), FRAMEGETTIME() );
	}
#endif

	CheckInvisibleWeapon( true );
	CheckInvisibleCloak( true );
	CheckInvisibleHelmet( true );
	CheckChangeSkill( false );
	CheckChangeColor( false );

	ioBaseChar *pOwner = GetOwner();
	if( pOwner )
	{
		if( m_bDescEqualDuration )
			pOwner->EndEmoticonByName( m_DescName );

		if( IsBadStateBuff() )
		{
			if( !pOwner->IsOwnerChar() )
				pOwner->SetEmoticon( m_EndEmoticon, EMT_BAD_STATE );
		}
		else
		{
			if( !pOwner->IsOwnerChar() )
				pOwner->SetEmoticon( m_EndEmoticon, EMT_SKILL_BUFF );
		}

		pOwner->EndScreenBlindBuff2();
		
		auto pStage = pOwner->GetCreator();
		if ( pStage )
		{
			pOwner->EndScreenBlind2();
		}
	}

	if( m_bLoopSoundPlayed && !m_LoopSound.IsEmpty() )
	{
		g_SoundMgr.StopSound( m_LoopSound, m_LoopSoundID );
		//g_SoundMgr.StopSound( m_LoopSound, 0 );
	}
	
	m_pOwner->EndEffect( m_BuffEffect, false );
	m_pOwner->AttachEffect( m_BuffEndEffect, GetEffectEventOwner() );
	
	DestroyAreaWeapon();

	CheckBuffEndBuff();
	CheckCallWeaponEnd();
}

void ioScreenBlindBuff2::SetAction()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
	{
		SetReserveEndBuff();
		return;
	}

	if( !pOwner->CheckGrapplingExceptionState() )
	{
		SetReserveEndBuff();
		return;
	}

	CheckInvisibleWeapon( false );
	CheckInvisibleCloak( false );
	CheckInvisibleHelmet( false );

	CheckChangeSkill( true );
	CheckChangeColor( true );
	CheckBuffDesc();

	if( m_CreatorLookType != CLT_NONE && !m_CreateChar.IsEmpty() )
	{
		if( m_CreateChar != pOwner->GetCharName() )
			pOwner->RotateByBuffCollision( m_CreateChar, m_CreatorLookType );
	}

	if( !m_LoopSound.IsEmpty() )
	{
		m_LoopSoundID = pOwner->PlaySound( m_LoopSound, true );
		m_bLoopSoundPlayed = true;
	}
	m_bBuffStart = true;
	
	pOwner->StartScreenBlindBuff2( &m_ScreenBlindTime, &m_BuffBlind, m_fBlindRange );
	
}

void ioScreenBlindBuff2::ProcessDelay( float fTimePerSec )
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

