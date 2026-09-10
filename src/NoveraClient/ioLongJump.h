#pragma once


#include "ioExtendJump.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;

class ioLongJump : public ioExtendJump
{
protected:
	float m_fExtraForce;
	DWORD m_dwJumpChargeTime;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioExtendJump* Clone();
	virtual JumpType GetType() const;

public:
	virtual void SetJumpping( ioBaseChar *pOwner, bool bFullTime );
	virtual DWORD GetChargingTime( ioBaseChar *pChar );

public:
	ioLongJump();
	ioLongJump( const ioLongJump &rhs );
	virtual ~ioLongJump();
};

