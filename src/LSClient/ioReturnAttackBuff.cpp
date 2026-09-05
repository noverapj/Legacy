#include "stdafx.h"

#include "ioReturnAttackBuff.h"

#include "ioBaseChar.h"



ioReturnAttackBuff::ioReturnAttackBuff()
{
	m_iReturnAttackNum = 0;
}

ioReturnAttackBuff::ioReturnAttackBuff( const ioReturnAttackBuff &rhs )
: ioBuff( rhs ),
 m_iReturnAttackNum( rhs.m_iReturnAttackNum ),
 m_iReturnAttackNumResist( rhs.m_iReturnAttackNumResist ),
 m_bDisableDownState( rhs.m_bDisableDownState )
{
}

ioReturnAttackBuff::~ioReturnAttackBuff()
{
}

void ioReturnAttackBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	m_iReturnAttackNum = rkLoader.LoadInt_e( "return_attack_attribute", 0 );
	m_iReturnAttackNumResist = rkLoader.LoadInt_e( "return_attack_attribute_resist", 0 );

	m_bDisableDownState = rkLoader.LoadBool_e( "disable_down_state", false );
}

ioBuff* ioReturnAttackBuff::Clone()
{
	return new ioReturnAttackBuff( *this );
}

void ioReturnAttackBuff::StartBuff(ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

	m_pOwner->SetReturnAttackAttribute( m_iReturnAttackNum, m_iReturnAttackNumResist );
}

bool ioReturnAttackBuff::ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	m_pOwner->SetReturnAttackAttribute( m_iReturnAttackNum, m_iReturnAttackNumResist );
	return true;
}

void ioReturnAttackBuff::ProcessBuff( float fTimePerSec )
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

void ioReturnAttackBuff::EndBuff()
{
	if( m_pOwner )
		m_pOwner->SetReturnAttackAttribute( 0, 0 );

	ioBuff::EndBuff();
}

void ioReturnAttackBuff::CheckOwnerStateCheck()
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

