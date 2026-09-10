#include "stdafx.h"

#include "ioFrozenBuff.h"


#include "ioNpcChar.h"

#include "ioGrowthUpInfo.h"

ioFrozenBuff::ioFrozenBuff()
{
	m_dwAniEndTime = 0;
	m_dwExtraFrozenDuration = 0;
	m_bSetEffect = false;
	m_bNotFrozen = true;
	m_bRseveSetFrozen = false;
}

ioFrozenBuff::ioFrozenBuff( const ioFrozenBuff &rhs )
: ioBuff( rhs ),
 m_dwMinTime( rhs.m_dwMinTime ),
 m_dwMaxTime( rhs.m_dwMaxTime ),
 m_dwMinTimeByNpc( rhs.m_dwMinTimeByNpc ),
 m_dwMaxTimeByNpc( rhs.m_dwMaxTimeByNpc ),
 m_dwMinTimeByBossNpc( rhs.m_dwMinTimeByBossNpc ),
 m_dwMaxTimeByBossNpc( rhs.m_dwMaxTimeByBossNpc ),
 m_dwTeamMinTime( rhs.m_dwTeamMinTime ),
 m_dwTeamMaxTime( rhs.m_dwTeamMaxTime ),
 m_dwTeamMinTimeByNpc( rhs.m_dwTeamMinTimeByNpc ),
 m_dwTeamMaxTimeByNpc( rhs.m_dwTeamMaxTimeByNpc ),
 m_dwTeamMinTimeByBossNpc( rhs.m_dwTeamMinTimeByBossNpc ),
 m_dwTeamMaxTimeByBossNpc( rhs.m_dwTeamMaxTimeByBossNpc ),
 m_fDecreaseRate( rhs.m_fDecreaseRate ),
 m_FrozenType( rhs.m_FrozenType )
{
	m_dwAniEndTime = 0;
	m_dwExtraFrozenDuration = 0;
	m_bSetEffect = false;
	m_bNotFrozen = true;
	m_bRseveSetFrozen = false;
}

ioFrozenBuff::~ioFrozenBuff()
{
}

ioBuff* ioFrozenBuff::Clone()
{
	return new ioFrozenBuff( *this );
}

void ioFrozenBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	m_dwMaxTime = rkLoader.LoadInt_e( "max_time", 0 );
	m_dwMinTime = rkLoader.LoadInt_e( "min_time", 0 );	
	m_dwMaxTimeByNpc = rkLoader.LoadInt_e( "max_time_npc", m_dwMaxTime );
	m_dwMinTimeByNpc = rkLoader.LoadInt_e( "min_time_npc", m_dwMinTime );
	m_dwMaxTimeByBossNpc = rkLoader.LoadInt_e( "max_time_boss_npc", m_dwMaxTimeByNpc );
	m_dwMinTimeByBossNpc = rkLoader.LoadInt_e( "min_time_boss_npc", m_dwMinTimeByNpc );

	m_dwTeamMaxTime = rkLoader.LoadInt_e( "team_max_time", 0 );
	m_dwTeamMinTime = rkLoader.LoadInt_e( "team_min_time", 0 );
	m_dwTeamMaxTimeByNpc = rkLoader.LoadInt_e( "team_max_time_npc", m_dwTeamMaxTime );
	m_dwTeamMinTimeByNpc = rkLoader.LoadInt_e( "team_min_time_npc", m_dwTeamMinTime );
	m_dwTeamMaxTimeByBossNpc = rkLoader.LoadInt_e( "team_max_time_boss_npc", m_dwTeamMaxTimeByNpc );
	m_dwTeamMinTimeByBossNpc = rkLoader.LoadInt_e( "team_min_time_boss_npc", m_dwTeamMinTimeByNpc );

	m_fDecreaseRate = rkLoader.LoadFloat_e( "decrease_rate", FLOAT1 );

	m_FrozenType = (FrozenType)rkLoader.LoadInt_e( "frozen_type", FZT_NONE );
}

DWORD ioFrozenBuff::GetMaxTime()
{
	ioNpcChar *pNpc = ToNpcChar( m_pOwner );
	if( !pNpc || pNpc->GetNpcType() == NT_DEVELOPER_K )
		return m_dwMaxTime;

	if( pNpc->IsMonsterLastBoss() || pNpc->IsMonsterHalfBoss() )
		return m_dwMaxTimeByBossNpc;
	return m_dwMaxTimeByNpc;
}

DWORD ioFrozenBuff::GetMinTime()
{
	ioNpcChar *pNpc = ToNpcChar( m_pOwner );
	if( !pNpc || pNpc->GetNpcType() == NT_DEVELOPER_K )
		return m_dwMinTime;

	if( pNpc->IsMonsterLastBoss() || pNpc->IsMonsterHalfBoss() )
		return m_dwMinTimeByBossNpc;
	return m_dwMinTimeByNpc;
}

DWORD ioFrozenBuff::GetTeamMaxTime()
{
	ioNpcChar *pNpc = ToNpcChar( m_pOwner );
	if( !pNpc || pNpc->GetNpcType() == NT_DEVELOPER_K )
		return m_dwTeamMaxTime;

	if( pNpc->IsMonsterLastBoss() || pNpc->IsMonsterHalfBoss() )
		return m_dwTeamMaxTimeByBossNpc;
	return m_dwTeamMaxTimeByNpc;
}

DWORD ioFrozenBuff::GetTeamMinTime()
{
	ioNpcChar *pNpc = ToNpcChar( m_pOwner );
	if( !pNpc || pNpc->GetNpcType() == NT_DEVELOPER_K )
		return m_dwTeamMinTime;

	if( pNpc->IsMonsterLastBoss() || pNpc->IsMonsterHalfBoss() )
		return m_dwTeamMinTimeByBossNpc;
	return m_dwTeamMinTimeByNpc;
}

void ioFrozenBuff::StartBuff( ioBaseChar *pOwner )
{
#ifndef SHIPPING 
	if( Setting::ShowExtraInfo() )
	{
		LOG.PrintTimeAndLog( 0, "BuffCheck - %s, StartTime: %d", m_Name.c_str(), FRAMEGETTIME() );
	}
#endif

	if( !pOwner )
		return;

	m_pOwner = pOwner;
	m_OwnerName = pOwner->GetCharName();

	CheckInvisibleWeapon( false );
	CheckInvisibleCloak( false );
	CheckInvisibleHelmet( false );

	CheckChangeSkill( true );
	CheckChangeColor( true );
	CheckBuffDesc();

	m_dwBuffEndTime = FRAMEGETTIME() + GetBuffDuration();
	m_bLive = true;
	m_bNotFrozen = true;

	if( !m_pOwner ) return;
	
	if( !m_LoopSound.IsEmpty() )
	{
		m_LoopSoundID = m_pOwner->PlaySound( m_LoopSound, true );
		m_bLoopSoundPlayed = true;
	}

	m_dwAniEndTime = m_pOwner->GetAnimationEndTime();

	bool bPreFrozen = false;
	if( m_pOwner->GetPreState() == CS_FROZEN )
		bPreFrozen = true;

	if( !m_ItemName.IsEmpty() )
	{
		ioGrowthUpInfo *pUpInfo = GetGrowthUpInfoForItem(GT_FORZEN_DURATION);
		ioGrowthNormalUpInfo *pNormalInfo = ToNormalUpInfo( pUpInfo );
		if( pNormalInfo )
		{
			ioPlayStage *pStage = g_ItemMaker.GetPlayStage();
			m_dwExtraFrozenDuration = (DWORD)pNormalInfo->GetValue(pStage);
		}
	}
	/*
	bool bEnableTeamAttack = pOwner->IsEnableTeamAttack();

	if( !bEnableTeamAttack && m_bTeamAttack )
		m_pOwner->SetFrozenState( bPreFrozen, GetTeamMinTime()+m_dwExtraFrozenDuration, GetTeamMaxTime()+m_dwExtraFrozenDuration );
	else
		m_pOwner->SetFrozenState( bPreFrozen, GetMinTime()+m_dwExtraFrozenDuration, GetMaxTime()+m_dwExtraFrozenDuration );
		*/
}

bool ioFrozenBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	m_pOwner = pOwner;

	DWORD dwGlobalTimeGap = g_GlobalTimer.CalcGlobalGapTime( dwGlobalTime );

	if( m_iOperationType != OT_SWITCH )
	{
		if( dwGlobalTimeGap > dwGapTime )
			return false;

		dwGapTime -= dwGlobalTimeGap;
		m_dwBuffEndTime = FRAMEGETTIME() + dwGapTime;
	}

	m_bLive = true;
	m_bNotFrozen = true;

	if( !m_pOwner ) return false;

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

	CheckInvisibleWeapon( false );
	CheckInvisibleCloak( false );
	CheckInvisibleHelmet( false );
	CheckChangeSkill( true );
	CheckChangeColor( true );
	CheckBuffDesc();

	return true;
}

void ioFrozenBuff::ProcessBuff( float fTimePerSec )
{
	if ( !m_bRseveSetFrozen )
	{
		m_bRseveSetFrozen = true;
		m_dwAniEndTime = m_pOwner->GetAnimationEndTime();

		bool bPreFrozen = false;
		if( m_pOwner->GetPreState() == CS_FROZEN )
			bPreFrozen = true;

		bool bEnableTeamAttack = m_pOwner->IsEnableTeamAttack();

		if( !bEnableTeamAttack && m_bTeamAttack )
			m_pOwner->SetFrozenState( bPreFrozen, GetTeamMinTime()+m_dwExtraFrozenDuration, GetTeamMaxTime()+m_dwExtraFrozenDuration );
		else
			m_pOwner->SetFrozenState( bPreFrozen, GetMinTime()+m_dwExtraFrozenDuration, GetMaxTime()+m_dwExtraFrozenDuration );

	}
	switch( m_iOperationType )
	{
	case OT_SWITCH:
		break;
	case OT_DELAY:
	case OT_ACTIVE:
		if( !m_bSetEffect )
		{
			if( m_pOwner->GetBlowFrozenState() || m_dwAniEndTime <= FRAMEGETTIME() )
			{
				m_pOwner->AttachEffect( m_BuffEffect, GetEffectEventOwner() );
				m_bSetEffect = true;
			}
		}

		if( m_pOwner->GetState() != CS_FROZEN || !m_pOwner->GetFrozenState() )
		{
			if( !m_pOwner->GetFrozenState() )
				m_bNotFrozen = false;

			SetReserveEndBuff();
			return;
		}
		break;
	case OT_PASSIVE:
		CheckCallingAreaWeapon();
		break;
	}
}

void ioFrozenBuff::EndBuff()
{
	if( m_pOwner && m_pOwner->GetState() != CS_STOP_MOTION )
		m_pOwner->GetGroup()->ResumeAllPlayingAnimation();

	if( !m_bNotFrozen )
	{
		if( !m_pOwner->GetBlowFrozenState() )
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

	ioBuff::EndBuff();
}

void ioFrozenBuff::GetBuffInfo( SP2Packet &rkPacket )
{
	ioBuff::GetBuffInfo( rkPacket );

	rkPacket << m_dwExtraFrozenDuration;
}

void ioFrozenBuff::ApplyBuffInfo( SP2Packet &rkPacket )
{
	rkPacket >> m_dwExtraFrozenDuration;
}

