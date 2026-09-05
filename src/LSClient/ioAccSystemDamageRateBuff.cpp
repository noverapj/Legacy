#include "StdAfx.h"
#include "ioAccSystemDamageRateBuff.h"

ioAccSystemDamageRateBuff::ioAccSystemDamageRateBuff()
{
}

ioAccSystemDamageRateBuff::ioAccSystemDamageRateBuff( const ioAccSystemDamageRateBuff &rhs )
	: ioBuff( rhs ),
	m_RateType( rhs.m_RateType ),
	m_fMaxDamageRate( rhs.m_fMaxDamageRate ),
	m_fMinDamageRate( rhs.m_fMinDamageRate ),
	m_bDisableDownState( rhs.m_bDisableDownState )
{
}

ioAccSystemDamageRateBuff::~ioAccSystemDamageRateBuff()
{
}

void ioAccSystemDamageRateBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	m_RateType			 = (RATETPYE)rkLoader.LoadInt_e( "system_damage_rate_type", RT_FIXED_RATE );

	m_fMaxDamageRate	 = rkLoader.LoadFloat_e( "system_max_damage_rate", FLOAT1 );
	m_fMinDamageRate	 = rkLoader.LoadFloat_e( "system_min_damage_rate", 0.0f );

	m_bDisableDownState  = rkLoader.LoadBool_e( "disable_down_state", false );
}

ioBuff* ioAccSystemDamageRateBuff::Clone()
{
	return new ioAccSystemDamageRateBuff( *this );
}

void ioAccSystemDamageRateBuff::StartBuff(ioBaseChar *pOwner )
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
	m_fDamageRateByAcc = fNum;
	
}

bool ioAccSystemDamageRateBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
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
	m_fDamageRateByAcc = fNum;

	return true;
}

void ioAccSystemDamageRateBuff::ProcessBuff( float fTimePerSec )
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
		ioBuff::ProcessBuff( fTimePerSec );
		break;
	case OT_PASSIVE:
		CheckCallingAreaWeapon();
		break;
	}
}

float ioAccSystemDamageRateBuff::GetSystemDamageRate()
{
	switch( m_RateType )
	{
	case RT_RANDOM_RATE:				
		return m_fMinDamageRate + (float)rand() / ( (float)RAND_MAX / ( m_fMaxDamageRate - m_fMinDamageRate ) );		
	}

	return m_fDamageRateByAcc;
}

void ioAccSystemDamageRateBuff::EndBuff()
{
	ioBuff::EndBuff();
}

void ioAccSystemDamageRateBuff::CheckOwnerStateCheck()
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