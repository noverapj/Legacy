

#include "stdafx.h"

#include "ioBaseChar.h"
#include "ioWeakMagicBuff.h"

ioWeakMagicBuff::ioWeakMagicBuff()
{
}

ioWeakMagicBuff::ioWeakMagicBuff( const ioWeakMagicBuff &rhs )
: ioBuff( rhs ),
 m_fWeakRate( rhs.m_fWeakRate ),
 m_iMagicType( rhs.m_iMagicType ),
 m_bDisableDownState( rhs.m_bDisableDownState ),
 m_ElementProtectionType( rhs.m_ElementProtectionType )
{
}

ioWeakMagicBuff::~ioWeakMagicBuff()
{
}

ioBuff* ioWeakMagicBuff::Clone()
{
	return new ioWeakMagicBuff( *this );
}

void ioWeakMagicBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	m_iMagicType = (MagicType)rkLoader.LoadInt_e( "magic_element_type", 0 );
	m_fWeakRate = rkLoader.LoadFloat_e( "weak_rate", 0.0f );
	m_bDisableDownState  = rkLoader.LoadBool_e( "disable_down_state", false );
	m_ElementProtectionType = (ElementProtectionType)rkLoader.LoadInt_e( "magic_protection_type", EPT_WEAK_RATE );
}

void ioWeakMagicBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

	pOwner->SetWeakMagic( m_Name, m_fWeakRate, m_iMagicType, m_ElementProtectionType );
}

bool ioWeakMagicBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	pOwner->SetWeakMagic( m_Name, m_fWeakRate, m_iMagicType, m_ElementProtectionType );
	return true;
}

void ioWeakMagicBuff::ProcessBuff( float fTimePerSec )
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

void ioWeakMagicBuff::EndBuff()
{
	m_pOwner->RemoveWeakMagic( m_Name );
	ioBuff::EndBuff();
}

void ioWeakMagicBuff::CheckOwnerStateCheck()
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

