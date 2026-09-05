#include "StdAfx.h"
#include "ioAccSpeedUpDownBuff.h"

ioAccSpeedUpDownBuff::ioAccSpeedUpDownBuff()
{
	m_fCurAddSpeed = 0.0f;
	m_fExtraSpeed = 0.0f;
	m_bDisableDownState = false;
	m_fAddSpeedByAcc = 0.0f;
}

ioAccSpeedUpDownBuff::ioAccSpeedUpDownBuff( const ioAccSpeedUpDownBuff &rhs )
	: ioBuff( rhs ),
	m_fDashSpeedRate(rhs.m_fDashSpeedRate ),
	m_bDisableDownState(rhs.m_bDisableDownState),
	m_bCheckOwnerDropZoneDownState( rhs.m_bCheckOwnerDropZoneDownState )
{
	m_fCurAddSpeed = 0.0f;
	m_fExtraSpeed = 0.0f;
	m_fAddSpeedByAcc = 0.0f;
}

ioAccSpeedUpDownBuff::~ioAccSpeedUpDownBuff()
{
}

void ioAccSpeedUpDownBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	m_fDashSpeedRate = rkLoader.LoadFloat_e( "speed_dash_rate", FLOAT1 );
	m_bDisableDownState  = rkLoader.LoadBool_e( "disable_down_state", false );
	m_bCheckOwnerDropZoneDownState = rkLoader.LoadBool_e( "check_owner_drop_zone_down_state", false );
}

ioBuff* ioAccSpeedUpDownBuff::Clone()
{
	return new ioAccSpeedUpDownBuff( *this );
}

void ioAccSpeedUpDownBuff::StartBuff(ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );
	ioItem *pItem= GetOwnerItem();
	if ( !pItem )
	{
		SetReserveEndBuff();
		return;
	}

	float fNum = 0.f;
	ioSkill *pSkill = pOwner->GetEquipedSkill( m_SkillName );
	if( pSkill )
		fNum =  pSkill->GetAccessoryRandomNum() / FLOAT1000;

	if ( fNum <= 0.0f )
	{
		SetReserveEndBuff();
		return;
	}

	m_fAddSpeedByAcc = fNum;
	if( !m_ItemName.IsEmpty() )
	{
		ioGrowthNormalUpInfo *pUpInfo = ToNormalUpInfo(GetGrowthUpInfoForItem(GT_SPEED_BUFF_UP));
		if( pUpInfo )
		{
			ioPlayStage *pStage = g_ItemMaker.GetPlayStage();
			m_fExtraSpeed = pUpInfo->GetValue(pStage);
		}
	}

	m_fCurAddSpeed = m_fAddSpeedByAcc+m_fExtraSpeed;
	m_pOwner->SetExtraMoveSpeed( m_fCurAddSpeed, m_fDashSpeedRate );
}

bool ioAccSpeedUpDownBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	ioItem *pItem= GetOwnerItem();
	if ( !pItem )
	{
		SetReserveEndBuff();
		return false;
	}

	float fNum = 0.f;
	ioSkill *pSkill = pOwner->GetEquipedSkill( m_SkillName );
	if( pSkill )
		fNum =  pSkill->GetAccessoryRandomNum() / FLOAT1000;

	if ( fNum <= 0.0f )
	{
		SetReserveEndBuff();
		return false;
	}

	m_fAddSpeedByAcc = fNum;
	m_fCurAddSpeed = m_fAddSpeedByAcc+m_fExtraSpeed;
	m_pOwner->SetExtraMoveSpeed( m_fCurAddSpeed, m_fDashSpeedRate );
	return true;
}

void ioAccSpeedUpDownBuff::ProcessBuff( float fTimePerSec )
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
	case OT_ACTIVE:
		{
			if ( !m_bCheckOwnerDropZoneDownState )
				ioBuff::ProcessBuff( fTimePerSec );
			else
				ProcessDropZoneCheckBuff( fTimePerSec, GetOwner() );
		}

		break;
	case OT_PASSIVE:
		CheckCallingAreaWeapon();
		break;
	}
}

void ioAccSpeedUpDownBuff::EndBuff()
{
	m_pOwner->RemoveExtraMoveSpeed( m_fCurAddSpeed );

	if( m_pOwner->GetState() == CS_DELAY )
	{
		m_pOwner->SetState( CS_DELAY, m_bSendDelayNetWork );
	}
	else if( m_pOwner->GetState() == CS_RUN )
	{
		m_pOwner->SetState( CS_RUN );
	}
	else if( m_pOwner->GetState() == CS_DASH )
	{
		m_pOwner->SetState( CS_DASH );
	}
	else if( m_pOwner->GetState() == CS_EXTEND_DASH )
	{
		m_pOwner->CheckRefreshExtendDashState();
	}

	ioBuff::EndBuff();
}

void ioAccSpeedUpDownBuff::GetBuffInfo( SP2Packet &rkPacket )
{
	ioBuff::GetBuffInfo( rkPacket );

	rkPacket << m_fExtraSpeed;
}

void ioAccSpeedUpDownBuff::ApplyBuffInfo( SP2Packet &rkPacket )
{
	rkPacket >> m_fExtraSpeed;
}

void ioAccSpeedUpDownBuff::CheckOwnerStateCheck()
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

void ioAccSpeedUpDownBuff::ProcessDropZoneCheckBuff( float fTimePerSec, ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	CheckCallingProcess();

	if( GetBuffDuration() == 0 )
	{		
		return;
	}

	if( m_dwBuffEndTime <= FRAMEGETTIME() && pOwner->GetState() != CS_DROP_ZONE_DOWN )
	{		
		SetReserveEndBuff();
		return;
	}
}


