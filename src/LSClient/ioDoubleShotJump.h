#pragma once


#include "ioExtendJump.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;

class ioDoubleShotJump : public ioExtendJump
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
	ioDoubleShotJump();
	ioDoubleShotJump( const ioDoubleShotJump &rhs );
	virtual ~ioDoubleShotJump();
};

inline ioDoubleShotJump* ToDoubleShotJump( ioExtendJump *pJump )
{
	if( !pJump || pJump->GetType() != JT_DOUBLE_SHOT_JUMP )
		return NULL;

	return dynamic_cast< ioDoubleShotJump* >( pJump );
}