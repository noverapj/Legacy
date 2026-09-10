
#include "stdafx.h"

#include "ioChangeTeamBuff.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"


ioChangeTeamBuff::ioChangeTeamBuff()
{
	m_BuffEffectID = -1;
}

ioChangeTeamBuff::ioChangeTeamBuff( const ioChangeTeamBuff &rhs )
: ioBuff( rhs ),
 m_bEnableTargetAim( rhs.m_bEnableTargetAim ),
 m_iEndType( rhs.m_iEndType ),
 m_fExtraRate( rhs.m_fExtraRate ),
 m_bChangeTeamInMinimap( rhs.m_bChangeTeamInMinimap )
{
	m_BuffEffectID = -1;
}

ioChangeTeamBuff::~ioChangeTeamBuff()
{
}

void ioChangeTeamBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	m_bEnableTargetAim = rkLoader.LoadBool_e( "enable_target_aim", false );
	m_iEndType = (EndType)rkLoader.LoadInt_e( "enable_end_type", ET_DEFAULT );

	m_bChangeTeamInMinimap = rkLoader.LoadBool_e( "change_team_minimap", false );

	m_fExtraRate = rkLoader.LoadFloat_e( "extra_rate", FLOAT1 );
}

ioBuff* ioChangeTeamBuff::Clone()
{
	return new ioChangeTeamBuff( *this );
}

void ioChangeTeamBuff::StartBuff( ioBaseChar *pOwner )
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
	if ( m_iEndType == ET_NOCHANGE )
		CheckingOwnerChar( pOwner );
	StartOtherBuff();
}

bool ioChangeTeamBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
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

void ioChangeTeamBuff::ProcessBuff( float fTimePerSec )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
	{
		SetReserveEndBuff();
		return;
	}

	if( m_iOperationType == OT_DELAY )
	{
		if ( m_iEndType != ET_NOCHANGE )
		{	
			if( m_bBuffStart )
			{
				if( !pOwner->IsCanChangeTeam() )
				{
					pOwner->CheckUseBuffSkill( m_Name, false );
					SetReserveEndBuff();
					return;
				}
			}
		}
	}
	else
	{
		switch( m_iEndType )
		{
		case ET_DEFAULT:
			{
				if( !pOwner->IsCanChangeTeam() )
				{
					pOwner->CheckUseBuffSkill( m_Name, false );
					SetReserveEndBuff();
					return;
				}
			}
			break;
		case ET_NODASH:
			{
				if( !pOwner->IsCanChangeTeamEx() )
				{
					pOwner->CheckUseBuffSkill( m_Name, false );
					SetReserveEndBuff();
					return;
				}
			}
			break;
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

void ioChangeTeamBuff::EndBuff()
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
		if( m_BuffEffectID != -1)
			pOwner->EndEffect( m_BuffEffectID, false );

		pOwner->AttachEffect( m_BuffEndEffect, GetEffectEventOwner() );

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

		pOwner->CheckGaugeTeam( false );
	}

	if( m_bLoopSoundPlayed && !m_LoopSound.IsEmpty() )
	{
		g_SoundMgr.StopSound( m_LoopSound, m_LoopSoundID );
		//g_SoundMgr.StopSound( m_LoopSound, 0 );
	}

	DestroyAreaWeapon();

	CheckBuffEndBuff();
	CheckCallWeaponEnd();
	EndOtherBuff();
}

void ioChangeTeamBuff::SetAction()
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

	ioEffect *pEffect = pOwner->AttachEffect( m_BuffEffect, GetEffectEventOwner() );
	if(pEffect)
		m_BuffEffectID = pEffect->GetUniqueID();

	if( !m_LoopSound.IsEmpty() )
	{
		m_LoopSoundID = pOwner->PlaySound( m_LoopSound, true );
		m_bLoopSoundPlayed = true;
	}

	pOwner->CheckGaugeTeam( true );
	m_bBuffStart = true;
}

void ioChangeTeamBuff::ProcessDelay( float fTimePerSec )
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

void ioChangeTeamBuff::CheckingOwnerChar( ioBaseChar *pOwner )
{
	if ( !pOwner || m_CreateChar.IsEmpty() )
		return;
	ioPlayStage *pStage = pOwner->GetCreator();
	ioBaseChar* pCreater = pStage->GetBaseChar( m_CreateChar );
	if ( !pCreater )
		return;
	const BaseCharList charlist = pStage->GetBaseCharList();
	for ( int i=0 ; i< (int)charlist.size() ; ++i)
	{
		if ( charlist[i]->IsOwnerChar() )
		{
			if ( charlist[i]->GetTeam() == pCreater->GetTeam() )
				SetReserveEndBuff();
		}
	}
}
