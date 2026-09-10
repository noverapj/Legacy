#pragma once

#include "ioExtendJump.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;

class ioRotateDoubleJump : public ioExtendJump
{
protected:
	float m_fDoubleJumpPower;
	float m_fDoubleJumpForce;
	float m_fDoubleJumpEnableHeight;
	DWORD m_dwDoubleJumpEnableTime;

	float m_fDoubleJumpAniRate;
	ioHashString m_DoubleJumpAni;

	float m_fDoubleJumpSpeedRate;

	bool m_bUsedDoubleJump;
	
	int m_iMaxJumpCnt;
	int m_iCurJumpCnt;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioExtendJump* Clone();
	virtual JumpType GetType() const;

public:
	virtual void SetJumpping( ioBaseChar *pOwner, bool bFullTime );
	virtual void ProcessJumpping( ioBaseChar *pOwner, float fHeightGap, float fNeedGauge );
	virtual void ApplyJump( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual float GetExtraJumpSpeedRate( const D3DXVECTOR3 &vDir, const D3DXQUATERNION &qtRot ) const;
	virtual void SetLandingState( ioBaseChar *pOwner );

protected:
	bool IsCanDoubleJump( ioBaseChar *pOwner, float fHeightGap );

public:
	ioRotateDoubleJump();
	ioRotateDoubleJump( const ioRotateDoubleJump &rhs );
	virtual ~ioRotateDoubleJump();
};

