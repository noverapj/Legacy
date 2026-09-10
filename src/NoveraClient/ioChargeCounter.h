#pragma once

#include "ioCounterAttack.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;

class ioChargeCounter : public ioCounterAttack
{
public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual CounterAttackType GetType() const;
	virtual ioCounterAttack* Clone();

public:
	virtual bool StartCAT( ioBaseChar *pOwner, int iCounterAttackKeyState );
	virtual void ProcessCAT( ioBaseChar *pOwner );

	virtual void ApplyCAT( ioBaseChar *pOwner, SP2Packet &rkPacket );

public:
	ioChargeCounter();
	ioChargeCounter( const ioChargeCounter &rhs );
	virtual ~ioChargeCounter();
};

