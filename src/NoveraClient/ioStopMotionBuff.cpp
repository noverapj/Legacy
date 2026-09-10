

#include "stdafx.h"

#include "ioStopMotionBuff.h"

#include "ioBaseChar.h"


ioStopMotionBuff::ioStopMotionBuff()
{
}

ioStopMotionBuff::ioStopMotionBuff( const ioStopMotionBuff &rhs )
: ioBuff( rhs ),
 m_ChangeMaterial( rhs.m_ChangeMaterial ),
 m_StopMotionType( rhs.m_StopMotionType ),
 m_LoopMotion( rhs.m_LoopMotion ),
 m_CatchLoopMotion( rhs.m_CatchLoopMotion )
{
}

ioStopMotionBuff::~ioStopMotionBuff()
{
}

void ioStopMotionBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";

	m_StopMotionType = (StopMotionType)rkLoader.LoadInt_e( "stop_motion_type", SMT_NORMAL );

	rkLoader.LoadString_e( "change_material", "", szBuf, MAX_PATH );
	m_ChangeMaterial = szBuf;

	rkLoader.LoadString_e( "loop_ani", "", szBuf, MAX_PATH );
	m_LoopMotion = szBuf;

	rkLoader.LoadString_e( "catch_loop_ani", "", szBuf, MAX_PATH );
	m_CatchLoopMotion = szBuf;
}

ioBuff* ioStopMotionBuff::Clone()
{
	return new ioStopMotionBuff( *this );
}

void ioStopMotionBuff::StartBuff( ioBaseChar *pOwner )
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

bool ioStopMotionBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
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

void ioStopMotionBuff::ProcessBuff( float fTimePerSec )
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
			if( m_pOwner && m_pOwner->GetState() != CS_STOP_MOTION )
			{
				SetReserveEndBuff();
				return;
			}

			if( CheckEnableDelayState() )
				return;

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

void ioStopMotionBuff::EndBuff()
{
	if( m_pOwner )
	{
		if( !m_ChangeMaterial.IsEmpty() )
		{
			Help::RestoreMaterial( m_pOwner->GetGroup(), Setting::SoftSkinning() );
			m_pOwner->ApplyModifiedMaterial();
		}

		//Help::RestoreMaterialTex( m_pOwner->GetGroup() );

		m_pOwner->GetGroup()->ResumeAllPlayingAnimation();
		m_pOwner->RestoreAllEquipEffect();
		m_pOwner->RestroeAllEquipItemCustomize();

		if( m_pOwner->GetState() == CS_STOP_MOTION )
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

void ioStopMotionBuff::SetAction()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	CheckInvisibleWeapon( false );
	CheckInvisibleCloak( false );
	CheckInvisibleHelmet( false );

	CheckChangeSkill( true );

	CheckChangeColor( true );
	CheckBuffDesc();

	if( m_CreatorLookType != CLT_NONE && !m_CreateChar.IsEmpty() )
	{
		if( m_CreateChar != pOwner->GetCharName() )
			m_pOwner->RotateByBuffCollision( m_CreateChar, m_CreatorLookType );
	}

	pOwner->AttachEffect( m_BuffEffect, GetEffectEventOwner() );

	if( !m_LoopSound.IsEmpty() )
	{
		m_LoopSoundID = pOwner->PlaySound( m_LoopSound, true );
		m_bLoopSoundPlayed = true;
	}

	if( pOwner->GetState() != CS_DIE &&
		pOwner->GetState() != CS_VIEW &&
		pOwner->GetState() != CS_OBSERVER &&
		pOwner->GetState() != CS_LOADING )
	{
		pOwner->SetStopMotion( m_StopMotionType );
		pOwner->ClearAllEquipEffect();

		if( !m_ChangeMaterial.IsEmpty() )
		{
			Help::ChangeMaterial( m_pOwner->GetGroup(), m_ChangeMaterial, Setting::SoftSkinning() );
		}

		if( m_StopMotionType == SMT_LOOP_NORMAL || m_StopMotionType == SMT_LOOP_WEAK )
		{
			ioEntityGroup *pGrp = pOwner->GetGroup();
			if( pGrp )
			{
				pGrp->ClearAllActionAni( FLOAT100, true );
				pGrp->ClearAllLoopAni( FLOAT100, true );

				if( !pOwner->IsPrisonerMode() && !pOwner->IsCatchMode() )
					pGrp->SetLoopAni( m_LoopMotion, FLOAT100 );
				else
					pGrp->SetLoopAni( m_CatchLoopMotion, FLOAT100 );
			}
		}
	}

	m_bBuffStart = true;
}

void ioStopMotionBuff::ProcessDelay( float fTimePerSec )
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