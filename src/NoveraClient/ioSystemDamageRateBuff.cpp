#include "stdafx.h"

#include "ioSystemDamageRateBuff.h"

#include "ioBaseChar.h"

ioSystemDamageRateBuff::ioSystemDamageRateBuff()
{
}

ioSystemDamageRateBuff::ioSystemDamageRateBuff( const ioSystemDamageRateBuff &rhs )
: ioBuff( rhs ),
m_RateType( rhs.m_RateType ),
m_fDamageRate( rhs.m_fDamageRate ),
m_fMaxDamageRate( rhs.m_fMaxDamageRate ),
m_fMinDamageRate( rhs.m_fMinDamageRate ),
m_bDisableDownState( rhs.m_bDisableDownState )
{
}

ioSystemDamageRateBuff::~ioSystemDamageRateBuff()
{
}

void ioSystemDamageRateBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	m_RateType			 = (RATETPYE)rkLoader.LoadInt_e( "system_damage_rate_type", RT_FIXED_RATE );
	m_fDamageRate		 = rkLoader.LoadFloat_e( "system_damage_rate", FLOAT1 );

	m_fMaxDamageRate	 = rkLoader.LoadFloat_e( "system_max_damage_rate", FLOAT1 );
	m_fMinDamageRate	 = rkLoader.LoadFloat_e( "system_min_damage_rate", 0.0f );

	m_bDisableDownState  = rkLoader.LoadBool_e( "disable_down_state", false );
}

ioBuff* ioSystemDamageRateBuff::Clone()
{
	return new ioSystemDamageRateBuff( *this );
}

void ioSystemDamageRateBuff::StartBuff(ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );
}

bool ioSystemDamageRateBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;
		
	return true;
}

void ioSystemDamageRateBuff::ProcessBuff( float fTimePerSec )
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

float ioSystemDamageRateBuff::GetSystemDamageRate()
{
	switch( m_RateType )
	{
	case RT_RANDOM_RATE:				
		return m_fMinDamageRate + (float)rand() / ( (float)RAND_MAX / ( m_fMaxDamageRate - m_fMinDamageRate ) );		
	}

	return m_fDamageRate;
}

void ioSystemDamageRateBuff::EndBuff()
{
	ioBuff::EndBuff();
}

void ioSystemDamageRateBuff::CheckOwnerStateCheck()
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