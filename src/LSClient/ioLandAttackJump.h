#pragma once

#include "ioExtendJump.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;

class ioLandAttackJump : public ioExtendJump
{
protected:
	bool m_bEnableHighJump;
	float m_fExtraJumpPowerRate;
	DWORD m_dwJumpChargeTime;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioExtendJump* Clone();
	virtual JumpType GetType() const;

public:
	virtual void SetJumpping( ioBaseChar *pOwner, bool bFullTime );
	virtual DWORD GetChargingTime( ioBaseChar *pChar );

	bool IsUsedHighJump();

public:
	ioLandAttackJump();
	ioLandAttackJump( const ioLandAttackJump &rhs );
	virtual ~ioLandAttackJump();
};

inline ioLandAttackJump* ToLandAttackJump( ioExtendJump *pJump )
{
	if( !pJump || pJump->GetType() != JT_LAND_ATTACK_JUMP )
		return NULL;

	return dynamic_cast< ioLandAttackJump* >( pJump );
};

