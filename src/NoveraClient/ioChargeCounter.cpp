

#include "stdafx.h"

#include "ioBaseChar.h"
#include "ioChargeCounter.h"
#include "ItemDefine.h"

ioChargeCounter::ioChargeCounter()
{
}

ioChargeCounter::ioChargeCounter( const ioChargeCounter &rhs )
: ioCounterAttack( rhs )
{
}

ioChargeCounter::~ioChargeCounter()
{
}

void ioChargeCounter::LoadProperty( ioINILoader &rkLoader )
{
	ioCounterAttack::LoadProperty( rkLoader );
}

ioCounterAttack* ioChargeCounter::Clone()
{
	return new ioChargeCounter( *this );
}

CounterAttackType ioChargeCounter::GetType() const
{
	return CAT_CHARGE;
}

void ioChargeCounter::ApplyCAT( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
}

bool ioChargeCounter::StartCAT( ioBaseChar *pOwner, int iCounterAttackKeyState )
{
	if( !pOwner )
		return false;

	if ( iCounterAttackKeyState == CAKS_ATTACK_KEY )
	{
		ioAttackableItem *pItem = pOwner->GetPriorityItem( SPT_DEFENSE );
		ioChargeContactWeaponItem2 *pChargeItem2 = ToChargeContactWeaponItem2( pItem );
		if( pChargeItem2 )
			pChargeItem2->SetChargeAttackByCounter( pOwner );
		return true;
	}
	else
		return false;
	
}

void ioChargeCounter::ProcessCAT( ioBaseChar *pOwner )
{
}

