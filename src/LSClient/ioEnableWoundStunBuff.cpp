

#include "stdafx.h"

#include "ioEnableWoundStunBuff.h"

#include "ioBaseChar.h"


ioEnableWoundStunBuff::ioEnableWoundStunBuff()
{
	m_BuffEffectID = -1;
}

ioEnableWoundStunBuff::ioEnableWoundStunBuff( const ioEnableWoundStunBuff &rhs )
: ioBuff( rhs ),
m_StunAnimation( rhs.m_StunAnimation ),
m_CatchAnimation( rhs.m_CatchAnimation )
{
	m_BuffEffectID = -1;
}

ioEnableWoundStunBuff::~ioEnableWoundStunBuff()
{
}

void ioEnableWoundStunBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];
	rkLoader.LoadString_e( "stun_animation", "", szBuf, MAX_PATH );
	m_StunAnimation = szBuf;

	rkLoader.LoadString_e( "stun_catch_animation", "", szBuf, MAX_PATH );
	m_CatchAnimation = szBuf;
}

ioBuff* ioEnableWoundStunBuff::Clone()
{
	return new ioEnableWoundStunBuff( *this );
}

void ioEnableWoundStunBuff::StartBuff( ioBaseChar *pOwner )
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

	m_BuffEffectID = -1;
	
	m_bLive = true;

	if( m_iOperationType != OT_DELAY )
	{
		SetAction();
	}
}

bool ioEnableWoundStunBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
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

void ioEnableWoundStunBuff::ProcessBuff( float fTimePerSec )
{
	if( m_pOwner->GetState() != CS_ETC_STATE && 
		m_pOwner->GetState() == CS_DELAY )
	{
		SetContinueAction();
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

void ioEnableWoundStunBuff::EndBuff()
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

	if( m_BuffEffectID != -1) m_pOwner->EndEffect( m_BuffEffectID, false );
	m_pOwner->AttachEffect( m_BuffEndEffect, GetEffectEventOwner() );

	if( m_bDescEqualDuration )
		m_pOwner->EndEmoticonByName( m_DescName );

	if( m_bLoopSoundPlayed && !m_LoopSound.IsEmpty() )
	{
		g_SoundMgr.StopSound( m_LoopSound, m_LoopSoundID );
		//g_SoundMgr.StopSound( m_LoopSound, 0 );
	}

	if( IsBadStateBuff() )
	{
		if( !m_pOwner->IsOwnerChar() )
			m_pOwner->SetEmoticon( m_EndEmoticon, EMT_BAD_STATE );
	}
	else
	{
		if( !m_pOwner->IsOwnerChar() )
			m_pOwner->SetEmoticon( m_EndEmoticon, EMT_SKILL_BUFF );
	}

	DestroyAreaWeapon();

	CheckBuffEndBuff();
	CheckCallWeaponEnd();

	if( m_pOwner->GetState() == CS_ETC_STATE && m_pOwner->GetCurEtcStateCnt() == m_iEtcStateCnt )
	{
		m_pOwner->SetState( CS_DELAY, m_bSendDelayNetWork );
		m_pOwner->SetDashFailStart();
	}
}

void ioEnableWoundStunBuff::SetAction()
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

	ioEffect *pEffect = m_pOwner->AttachEffect( m_BuffEffect, GetEffectEventOwner() );
	if(pEffect)	m_BuffEffectID = pEffect->GetUniqueID();

	if( !m_LoopSound.IsEmpty() )
	{
		m_LoopSoundID = m_pOwner->PlaySound( m_LoopSound, true );
		m_bLoopSoundPlayed = true;
	}

	if( m_pOwner->GetState() != CS_DIE &&
		m_pOwner->GetState() != CS_LOADING &&
		m_pOwner->GetState() != CS_OBSERVER &&
		m_pOwner->GetState() != CS_VIEW )
	{
		ioEntityGroup *pGrp = m_pOwner->GetGroup();
		pGrp->ClearAllActionAni( FLOAT100, true );
		pGrp->ClearAllLoopAni( FLOAT100, true );

		if( !m_pOwner->IsPrisonerMode() && !m_pOwner->IsCatchMode() )
			pGrp->SetLoopAni( m_StunAnimation, FLOAT100 );
		else
			pGrp->SetLoopAni( m_CatchAnimation, FLOAT100 );

		m_pOwner->SetState( CS_ETC_STATE );
		m_iEtcStateCnt = m_pOwner->GetCurEtcStateCnt();
	}

	m_bBuffStart = true;
}

void ioEnableWoundStunBuff::SetContinueAction()
{
	if( m_pOwner->GetState() != CS_DIE &&
		m_pOwner->GetState() != CS_LOADING &&
		m_pOwner->GetState() != CS_OBSERVER &&
		m_pOwner->GetState() != CS_VIEW )
	{
		ioEntityGroup *pGrp = m_pOwner->GetGroup();
		pGrp->ClearAllActionAni( FLOAT100, true );
		pGrp->ClearAllLoopAni( FLOAT100, true );

		if( !m_pOwner->IsPrisonerMode() && !m_pOwner->IsCatchMode() )
			pGrp->SetLoopAni( m_StunAnimation, FLOAT100 );
		else
			pGrp->SetLoopAni( m_CatchAnimation, FLOAT100 );

		m_pOwner->SetState( CS_ETC_STATE );
		m_iEtcStateCnt = m_pOwner->GetCurEtcStateCnt();
	}
}

void ioEnableWoundStunBuff::ProcessDelay( float fTimePerSec )
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