#pragma once

#include "ioCounterAttack.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;

class ioDefaultCounter : public ioCounterAttack
{
	bool	m_bLastJump;
	float	m_fLastJumpPower;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual CounterAttackType GetType() const;
	virtual ioCounterAttack* Clone();

public:
	virtual bool StartCAT( ioBaseChar *pOwner, int iCounterAttackKeyState );
	virtual void ProcessCAT( ioBaseChar *pOwner );

	virtual void ApplyCAT( ioBaseChar *pOwner, SP2Packet &rkPacket );

public:
	ioDefaultCounter();
	ioDefaultCounter( const ioDefaultCounter &rhs );
	virtual ~ioDefaultCounter();
};

