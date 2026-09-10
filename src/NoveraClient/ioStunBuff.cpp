

#include "stdafx.h"

#include "ioStunBuff.h"

#include "ioBaseChar.h"


ioStunBuff::ioStunBuff()
{
	m_BuffEffectID = -1;
}

ioStunBuff::ioStunBuff( const ioStunBuff &rhs )
: ioBuff( rhs ),
m_StunAnimation( rhs.m_StunAnimation ),
m_CatchAnimation( rhs.m_CatchAnimation ),
m_CMSMAnimation( rhs.m_CMSMAnimation ),
m_CMSFAnimation( rhs.m_CMSFAnimation ),
m_CFSMAnimation( rhs.m_CFSMAnimation ),
m_CFSFAnimation( rhs.m_CFSFAnimation )

{
	m_BuffEffectID = -1;
}

ioStunBuff::~ioStunBuff()
{
}

void ioStunBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];
	rkLoader.LoadString_e( "stun_animation", "", szBuf, MAX_PATH );
	m_StunAnimation = szBuf;

	rkLoader.LoadString_e( "stun_catch_animation", "", szBuf, MAX_PATH );
	m_CatchAnimation = szBuf;

	rkLoader.LoadString_e( "creator_male_stun_male_animation", "", szBuf, MAX_PATH );
	m_CMSMAnimation = szBuf;

	rkLoader.LoadString_e( "creator_male_stun_female_animation", "", szBuf, MAX_PATH );
	m_CMSFAnimation = szBuf;

	rkLoader.LoadString_e( "creator_female_stun_male_animation", "", szBuf, MAX_PATH );
	m_CFSMAnimation = szBuf;

	rkLoader.LoadString_e( "creator_female_stun_female_animation", "", szBuf, MAX_PATH );
	m_CFSFAnimation = szBuf;
}

ioBuff* ioStunBuff::Clone()
{
	return new ioStunBuff( *this );
}

void ioStunBuff::StartBuff( ioBaseChar *pOwner )
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

bool ioStunBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
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

void ioStunBuff::ProcessBuff( float fTimePerSec )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
	{
		SetReserveEndBuff();
		return;
	}

	if( m_iOperationType == OT_DELAY )
	{
		if( m_bBuffStart )
		{
			if( pOwner->GetState() != CS_ETC_STATE ||
				pOwner->GetCurEtcStateCnt() != m_iEtcStateCnt )
			{
				SetReserveEndBuff();
				return;
			}
		}
	}
	else
	{
		if( pOwner->GetState() != CS_ETC_STATE ||
			pOwner->GetCurEtcStateCnt() != m_iEtcStateCnt )
		{
			SetReserveEndBuff();
			return;
		}
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

void ioStunBuff::EndBuff()
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

void ioStunBuff::SetAction()
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
		
		ioBaseChar *pCreater = m_pOwner->GetBaseChar( m_CreateChar );
		
		if( !m_pOwner->IsPrisonerMode() && !m_pOwner->IsCatchMode() )
		{
			ioHashString szLoopAnimaion = m_StunAnimation;
			if ( pCreater )
			{
				if ( pCreater->IsMale() && m_pOwner->IsMale() && !m_CMSMAnimation.IsEmpty() )
					szLoopAnimaion = m_CMSMAnimation;
				if ( pCreater->IsMale() && !m_pOwner->IsMale() && !m_CMSFAnimation.IsEmpty() )
					szLoopAnimaion = m_CMSFAnimation;
				if ( !pCreater->IsMale() && m_pOwner->IsMale() && !m_CFSMAnimation.IsEmpty() )
					szLoopAnimaion = m_CFSMAnimation;
				if ( !pCreater->IsMale() && !m_pOwner->IsMale() && !m_CFSFAnimation.IsEmpty() )
					szLoopAnimaion = m_CFSFAnimation;
			}
			pGrp->SetLoopAni( szLoopAnimaion, FLOAT100 );
		}
		else
		{
			ioHashString szLoopAnimaion = m_CatchAnimation;
			if ( pCreater )
			{
				if ( pCreater->IsMale() && m_pOwner->IsMale() && !m_CMSMAnimation.IsEmpty() )
					szLoopAnimaion = m_CMSMAnimation;
				if ( pCreater->IsMale() && !m_pOwner->IsMale() && !m_CMSFAnimation.IsEmpty() )
					szLoopAnimaion = m_CMSFAnimation;
				if ( !pCreater->IsMale() && m_pOwner->IsMale() && !m_CFSMAnimation.IsEmpty() )
					szLoopAnimaion = m_CFSMAnimation;
				if ( !pCreater->IsMale() && !m_pOwner->IsMale() && !m_CFSFAnimation.IsEmpty() )
					szLoopAnimaion = m_CFSFAnimation;
			}
			pGrp->SetLoopAni( szLoopAnimaion, FLOAT100 );
		}

		m_pOwner->SetState( CS_ETC_STATE );
		m_iEtcStateCnt = m_pOwner->GetCurEtcStateCnt();
	}

	m_bBuffStart = true;
}

void ioStunBuff::ProcessDelay( float fTimePerSec )
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