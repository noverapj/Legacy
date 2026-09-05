
#include "stdafx.h"

#include "ioConcentrateTargetBuff.h"

#include "ioBaseChar.h"

ioConcentrateTargetBuff::ioConcentrateTargetBuff()
{
	m_AttackerEffectID = -1;
	m_TargetEffectID = -1;
}

ioConcentrateTargetBuff::ioConcentrateTargetBuff( const ioConcentrateTargetBuff &rhs )
: ioBuff( rhs ),
m_bDisableDownState( rhs.m_bDisableDownState ),
m_bDisableDownStateByConcentrateAttacker( rhs.m_bDisableDownStateByConcentrateAttacker ),
m_bDisableDropDownState( rhs.m_bDisableDropDownState ),
m_AttackerEffectRed( rhs.m_AttackerEffectRed ),
m_AttackerEffectBlue( rhs.m_AttackerEffectBlue ),
m_TargetEffectRed( rhs.m_TargetEffectRed ),
m_TargetEffectBlue( rhs.m_TargetEffectBlue ),
m_fPushPowerRateByTarget( rhs.m_fPushPowerRateByTarget ),
m_fAirPushPowerRateByTarget( rhs.m_fAirPushPowerRateByTarget ),
m_fDownPushPowerRateByTarget( rhs.m_fDownPushPowerRateByTarget ),
m_fBlowPowerRateByTarget( rhs.m_fBlowPowerRateByTarget ),
m_fAirBlowPowerRateByTarget( rhs.m_fAirBlowPowerRateByTarget ),
m_fDownBlowPowerRateByTarget( rhs.m_fDownBlowPowerRateByTarget ),
m_fPushPowerRateByNoTarget( rhs.m_fPushPowerRateByNoTarget ),
m_fAirPushPowerRateByNoTarget( rhs.m_fAirPushPowerRateByNoTarget ),
m_fDownPushPowerRateByNoTarget( rhs.m_fDownPushPowerRateByNoTarget ),
m_fBlowPowerRateByNoTarget( rhs.m_fBlowPowerRateByNoTarget ),
m_fAirBlowPowerRateByNoTarget( rhs.m_fAirBlowPowerRateByNoTarget ),
m_fDownBlowPowerRateByNoTarget( rhs.m_fDownBlowPowerRateByNoTarget ),
m_fTargetDamageRate( rhs.m_fTargetDamageRate ),
m_fNoTargetDamageRate( rhs.m_fNoTargetDamageRate )
{
	m_AttackerEffectID = -1;
	m_TargetEffectID = -1;
}

ioConcentrateTargetBuff::~ioConcentrateTargetBuff()
{
}

void ioConcentrateTargetBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	m_bDisableDownState  = rkLoader.LoadBool_e( "disable_down_state", false );
	m_bDisableDownStateByConcentrateAttacker = rkLoader.LoadBool_e( "disable_down_state_by_concentrate_attacker", false );
	m_bDisableDropDownState = rkLoader.LoadBool_e( "disable_drop_down_state", false );

	char szBuf[MAX_PATH] = "";

	rkLoader.LoadString_e( "concentrate_attack_effect_red", "", szBuf, MAX_PATH );
	m_AttackerEffectRed = szBuf;

	rkLoader.LoadString_e( "concentrate_attack_effect_blue", "", szBuf, MAX_PATH );
	m_AttackerEffectBlue = szBuf;

	rkLoader.LoadString_e( "concentrate_target_effect_red", "", szBuf, MAX_PATH );
	m_TargetEffectRed = szBuf;

	rkLoader.LoadString_e( "concentrate_target_effect_blue", "", szBuf, MAX_PATH );
	m_TargetEffectBlue = szBuf;

	m_fPushPowerRateByTarget = rkLoader.LoadFloat_e( "push_power_rate_by_target", FLOAT1 );
	m_fAirPushPowerRateByTarget = rkLoader.LoadFloat_e( "air_push_rate_by_target", FLOAT1 );
	m_fDownPushPowerRateByTarget = rkLoader.LoadFloat_e( "down_push_rate_by_target", FLOAT1 );
	m_fBlowPowerRateByTarget = rkLoader.LoadFloat_e( "blow_power_rate_by_target", FLOAT1 );
	m_fAirBlowPowerRateByTarget = rkLoader.LoadFloat_e( "air_blow_power_rate_by_target", FLOAT1 );
	m_fDownBlowPowerRateByTarget = rkLoader.LoadFloat_e( "down_blow_power_rate_by_target", FLOAT1 );

	m_fPushPowerRateByNoTarget = rkLoader.LoadFloat_e( "push_power_rate_by_no_target", FLOAT1 );
	m_fAirPushPowerRateByNoTarget = rkLoader.LoadFloat_e( "air_push_rate_by_no_target", FLOAT1 );
	m_fDownPushPowerRateByNoTarget = rkLoader.LoadFloat_e( "down_push_rate_by_no_target", FLOAT1 );
	m_fBlowPowerRateByNoTarget = rkLoader.LoadFloat_e( "blow_power_rate_by_no_target", FLOAT1 );
	m_fAirBlowPowerRateByNoTarget = rkLoader.LoadFloat_e( "air_blow_power_rate_by_no_target", FLOAT1 );
	m_fDownBlowPowerRateByNoTarget = rkLoader.LoadFloat_e( "down_blow_power_rate_by_no_target", FLOAT1 );

	m_fTargetDamageRate = rkLoader.LoadFloat_e( "target_damage_rate", FLOAT1 );
	m_fNoTargetDamageRate = rkLoader.LoadFloat_e( "no_target_damage_rate", FLOAT1 );
}

ioBuff* ioConcentrateTargetBuff::Clone()
{
	return new ioConcentrateTargetBuff( *this );
}

void ioConcentrateTargetBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );
}

bool ioConcentrateTargetBuff::ApplyStartBuff(ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	m_AttackerEffectID = -1;
	m_TargetEffectID = -1;

	return true;
}

void ioConcentrateTargetBuff::ProcessBuff( float fTimePerSec )
{
	if( CheckOwnerStateCheck() )
		return;

	ioBaseChar *pTarget = m_pOwner->GetBaseChar( m_TargetName );
	if( !pTarget )
	{
		m_pOwner->CheckUseBuffSkill( m_Name, false );
		SetReserveEndBuff();
		return;
	}
	else
	{
		CharState eState = pTarget->GetState();
		if( eState == CS_DIE || eState == CS_READY )
		{
			m_pOwner->CheckUseBuffSkill( m_Name, false );
			SetReserveEndBuff();
			return;
		}
	}

	switch( m_iOperationType )
	{
	case OT_SWITCH:
		if( !CheckSwitchType( fTimePerSec ) )
		{
			if( GetOwner() && CheckRestoreStateCheck() )
				GetOwner()->SetState( CS_DELAY, m_bSendDelayNetWork );

			m_pOwner->CheckUseBuffSkill( m_Name, false );
			SetReserveEndBuff();
			return;
		}

		CheckCallingProcess();
		break;
	case OT_DELAY:
	case OT_ACTIVE:
		{
			if( CheckEnableDelayState() )
				return;

			CheckCallingProcess();

			if( GetBuffDuration() == 0 )
				return;

			if( m_dwBuffEndTime <= FRAMEGETTIME() )
			{
				if( CheckRestoreStateCheck() )
				{
					m_pOwner->SetState( CS_DELAY, m_bSendDelayNetWork );
				}

				m_pOwner->CheckUseBuffSkill( m_Name, false );
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

bool ioConcentrateTargetBuff::CheckOwnerStateCheck()
{
	if( !m_pOwner ) return false;

	CharState eState = m_pOwner->GetState();

	bool bDown = false;
	if( m_bDisableDownState )
	{
		if( eState == CS_FROZEN && m_pOwner->GetBlowFrozenState() )
			bDown = true;
		else if( eState == CS_BLOW_WOUNDED && !m_pOwner->IsFloatingState() )
			bDown = true;
		else if( eState == CS_BOUND_BLOW_WOUNDED && !m_pOwner->IsFloatingState() )
			bDown = true;
		else if( eState == CS_ETC_STATE && m_pOwner->IsApplyDownState(false) )
			bDown = true;
		else if( eState == CS_STOP_MOTION && m_pOwner->GetBlowStopMotionState() )
			bDown = true;
		else if( eState == CS_DROP_ZONE_DOWN )
			bDown = true;
		else if( eState == CS_ICE_STATE && m_pOwner->GetBlowStopMotionState() )
			bDown = true;

		if( bDown )
		{
			if( CheckRestoreStateCheck() )
			{
				m_pOwner->SetState( CS_DELAY, m_bSendDelayNetWork );
			}

			m_pOwner->CheckUseBuffSkill( m_Name, false );
			SetReserveEndBuff();
			return true;
		}
	}

	if( m_bDisableDownStateByConcentrateAttacker && m_pOwner->GetLastAttackerName() == m_TargetName )
	{
		if( eState == CS_FROZEN && m_pOwner->GetBlowFrozenState() )
			bDown = true;
		else if( eState == CS_BLOW_WOUNDED && !m_pOwner->IsFloatingState() )
			bDown = true;
		else if( eState == CS_BOUND_BLOW_WOUNDED && !m_pOwner->IsFloatingState() )
			bDown = true;
		else if( eState == CS_ETC_STATE && m_pOwner->IsApplyDownState(false) )
			bDown = true;
		else if( eState == CS_STOP_MOTION && m_pOwner->GetBlowStopMotionState() )
			bDown = true;
		else if( eState == CS_DROP_ZONE_DOWN )
			bDown = true;
		else if( eState == CS_ICE_STATE && m_pOwner->GetBlowStopMotionState() )
			bDown = true;

		if( bDown )
		{
			if( CheckRestoreStateCheck() )
			{
				m_pOwner->SetState( CS_DELAY, m_bSendDelayNetWork );
			}

			m_pOwner->CheckUseBuffSkill( m_Name, false );
			SetReserveEndBuff();
			return true;
		}
	}

	bool bWound = false;
	if( m_bEnableWoundCancel )
	{
		if( eState == CS_BLOW_WOUNDED )
			bWound = true;
		else if( eState == CS_BOUND_BLOW_WOUNDED )
			bWound = true;
		else if( eState == CS_WOUNDED )
			bWound = true;
		else if( eState == CS_ETC_STATE )
			bWound = true;

		if( bWound )
		{
			m_pOwner->CheckUseBuffSkill( m_Name, false );
			SetReserveEndBuff();
			return true;
		}
	}

	if( m_bDisableDropDownState )
	{
		if( eState == CS_DROP_ZONE_DOWN )
		{
			m_pOwner->CheckUseBuffSkill( m_Name, false );
			SetReserveEndBuff();
			return true;
		}
	}

	return false;
}

bool ioConcentrateTargetBuff::CheckRestoreStateCheck()
{
	if( !m_pOwner ) return false;

	CharState eState = m_pOwner->GetState();

	switch( eState )
	{
	case CS_DELAY:
	case CS_RUN:
	case CS_DASH:
		return true;
	}

	return false;
}

void ioConcentrateTargetBuff::EndBuff()
{
	ioBuff::EndBuff();
	RemoveConcentrateChar();
}

void ioConcentrateTargetBuff::GetBuffInfo( SP2Packet &rkPacket )
{
	ioBuff::GetBuffInfo( rkPacket );

	rkPacket << m_AttackerName;
	rkPacket << m_TargetName;
}

void ioConcentrateTargetBuff::ApplyBuffInfo( SP2Packet &rkPacket )
{
	rkPacket >> m_AttackerName;
	rkPacket >> m_TargetName;

	SetConcentrateCharName( m_AttackerName, m_TargetName );
}

void ioConcentrateTargetBuff::SetConcentrateCharName( const ioHashString &szAttackerName, const ioHashString &szTargetName )
{
	m_AttackerName = szAttackerName;
	m_TargetName = szTargetName;

	m_AttackerEffectID = -1;
	m_TargetEffectID = -1;

	ioBaseChar *pOwner = GetOwner();
	if( pOwner )
	{
		ioEffect *pEffect = NULL;
		if( pOwner->GetTeam() == TEAM_BLUE  )
			pEffect = pOwner->AttachEffect( m_AttackerEffectBlue );
		else if( pOwner->GetTeam() == TEAM_RED )
			pEffect = pOwner->AttachEffect( m_AttackerEffectRed );
		else
		{
			if( pOwner->GetSingleConvertTeam() == TEAM_BLUE )
				pEffect = pOwner->AttachEffect( m_AttackerEffectBlue );
			else
				pEffect = pOwner->AttachEffect( m_AttackerEffectRed );
		}

		if( pEffect )
			m_AttackerEffectID = pEffect->GetUniqueID();

		ioBaseChar *pTarget = pOwner->GetBaseChar( szTargetName );
		if( pTarget && ( pTarget->IsNeedProcess() || pOwner->IsNeedProcess() ) )
		{
			pEffect = NULL;
			if( pTarget->GetTeam() == TEAM_BLUE )
				pEffect = pTarget->AttachEffect( m_TargetEffectBlue );
			else if( pTarget->GetTeam() == TEAM_RED )
				pEffect = pTarget->AttachEffect( m_TargetEffectRed );
			else
			{
				if( pTarget->GetSingleConvertTeam() == TEAM_BLUE )
					pEffect = pTarget->AttachEffect( m_TargetEffectBlue );
				else
					pEffect = pTarget->AttachEffect( m_TargetEffectRed );
			}

			if( pEffect )
				m_TargetEffectID = pEffect->GetUniqueID();
		}
	}
}

void ioConcentrateTargetBuff::RemoveConcentrateChar()
{
	ioBaseChar *pOwner = GetOwner();

	if( pOwner )
	{
		pOwner->EndEffect( m_AttackerEffectID );
		m_AttackerEffectID = -1;

		ioBaseChar *pTarget = pOwner->GetBaseChar( m_TargetName );
		if( pTarget )
		{
			pTarget->EndEffect( m_TargetEffectID );
			m_TargetEffectID = -1;
		}

		m_AttackerName.Clear();
		m_TargetName.Clear();
	}
}

float ioConcentrateTargetBuff::GetPushPowerRateByTarget( ioBaseChar *pOwner, ioBaseChar *pTarget )
{
	if( !pOwner || !pTarget )
		return FLOAT1;

	if( pTarget->GetCharName() == m_TargetName )
	{
		if( pOwner->IsFloatingState() )
		{
			return m_fAirPushPowerRateByTarget;
		}
		else if( pOwner->GetState() == CS_BLOW_WOUNDED || pOwner->GetState() == CS_BOUND_BLOW_WOUNDED || pOwner->GetState() == CS_BLOW_EXTEND_WOUND )
		{
			return m_fDownPushPowerRateByTarget;
		}
		else
		{
			return m_fPushPowerRateByTarget;
		}
	}
	else
	{
		if( pOwner->IsFloatingState() )
		{
			return m_fAirPushPowerRateByNoTarget;
		}
		else if( pOwner->GetState() == CS_BLOW_WOUNDED || pOwner->GetState() == CS_BOUND_BLOW_WOUNDED || pOwner->GetState() == CS_BLOW_EXTEND_WOUND )
		{
			return m_fDownPushPowerRateByNoTarget;
		}
		else
		{
			return m_fPushPowerRateByNoTarget;
		}
	}

	return FLOAT1;
}

float ioConcentrateTargetBuff::GetBlowPowerRateByTarget( ioBaseChar *pOwner, ioBaseChar *pTarget )
{
	if( !pOwner || !pTarget )
		return FLOAT1;

	if( pTarget->GetCharName() == m_TargetName )
	{
		if( pOwner->IsFloatingState() )
		{
			return m_fAirBlowPowerRateByTarget;
		}
		else if( pOwner->GetState() == CS_BLOW_WOUNDED || pOwner->GetState() == CS_BOUND_BLOW_WOUNDED || pOwner->GetState() == CS_BLOW_EXTEND_WOUND )
		{
			return m_fDownBlowPowerRateByTarget;
		}
		else
		{
			return m_fBlowPowerRateByTarget;
		}
	}
	else
	{
		if( pOwner->IsFloatingState() )
		{
			return m_fAirBlowPowerRateByNoTarget;
		}
		else if( pOwner->GetState() == CS_BLOW_WOUNDED || pOwner->GetState() == CS_BOUND_BLOW_WOUNDED || pOwner->GetState() == CS_BLOW_EXTEND_WOUND )
		{
			return m_fDownBlowPowerRateByNoTarget;
		}
		else
		{
			return m_fBlowPowerRateByNoTarget;
		}
	}

	return FLOAT1;
}
