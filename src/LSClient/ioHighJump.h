#pragma once

#include "ioExtendJump.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;

class ioHighJump : public ioExtendJump
{
protected:
	float m_fExtraJumpPowerRate;
	DWORD m_dwJumpChargeTime;

	CEncrypt<bool> m_bUsedHighJump;
	CEncrypt<bool> m_bUseLandJumpAttack;
	CEncrypt<float> m_fGravityAmt;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioExtendJump* Clone();
	virtual JumpType GetType() const;

public:
	virtual void SetJumpping( ioBaseChar *pOwner, bool bFullTime );
	virtual DWORD GetChargingTime( ioBaseChar *pChar );
	virtual void SetLandingState( ioBaseChar *pOwner );

public:
	bool IsUseLandJumpAttack();
	float GetCurGravityAmt();

public:
	ioHighJump();
	ioHighJump( const ioHighJump &rhs );
	virtual ~ioHighJump();
};

inline ioHighJump* ToHighJump( ioExtendJump *pJump )
{
	if( !pJump || pJump->GetType() != JT_HIGH_JUMP )
		return NULL;

	return dynamic_cast< ioHighJump* >( pJump );
}

