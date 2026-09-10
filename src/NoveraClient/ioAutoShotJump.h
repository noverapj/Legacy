#pragma once


#include "ioExtendJump.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;

class ioAutoShotJump : public ioExtendJump
{
protected:
	DWORD m_dwJumpChargeTime;
	float m_fExtraJumpPowerRate;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioExtendJump* Clone();
	virtual JumpType GetType() const;

public:
	virtual void SetJumpping( ioBaseChar *pOwner, bool bFullTime );
	virtual DWORD GetChargingTime( ioBaseChar *pChar );

public:
	ioAutoShotJump();
	ioAutoShotJump( const ioAutoShotJump &rhs );
	virtual ~ioAutoShotJump();
};

