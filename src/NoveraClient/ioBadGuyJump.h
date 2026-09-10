#pragma once

#include "ioExtendJump.h"
#include "ioAttackAttribute.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;

class ioBadGuyJump : public ioExtendJump
{
public:
	enum BadGuyJumpState
	{
		BJS_NONE	= 0,
		BJS_HIGH	= 1,
		BJS_DOUBLE	= 2,
		BJS_DASH	= 3,
	};

protected:
	BadGuyJumpState m_CurJumpState;

// DoubleJump
protected:
	float m_fDoubleJumpPower;
	float m_fDoubleJumpForce;
	float m_fDoubleJumpEnableHeight;
	DWORD m_dwDoubleJumpEnableTime;

	float m_fDoubleJumpSpeedRateFR;
	float m_fDoubleJumpSpeedRateSI;
	float m_fDoubleJumpSpeedRateBG;

	float m_fDoubleJumpAniRate;
	ioHashStringVec m_vDoubleJumpAniList;

	bool m_bUsedDoubleJump;

	int m_iMaxJumpCnt;
	int m_iCurJumpCnt;

// High
protected:
	float m_fExtraJumpPowerRate;
	DWORD m_dwJumpChargeTime;

// Dash
protected:
	ioHashString m_DashAction;
	float m_fActionTimeRate;

	ForceInfoList m_vForceInfoList;
	DWORD m_dwActionEndTime;

	bool m_bDashEnd;
	bool m_bJumpAttackReserve;

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

	virtual DWORD GetChargingTime( ioBaseChar *pChar );

	virtual bool IsCanJumpAttack( ioBaseChar *pOwner );

	bool IsJumpDashState();
	bool IsJumpAttackReserve();

protected:
	bool IsCanDoubleJump( ioBaseChar *pOwner, float fHeightGap );
	void SetDoubleJumpAni( ioBaseChar *pOwner );

	bool IsCanJumpDash( ioBaseChar *pOwner, float fHeightGap );
	void ProcessJumpDash( ioBaseChar *pOwner );
	
	virtual void CheckKeyReserve( ioBaseChar *pOwner );

public:
	ioBadGuyJump();
	ioBadGuyJump( const ioBadGuyJump &rhs );
	virtual ~ioBadGuyJump();
};

inline ioBadGuyJump* ToBadGuyJump( ioExtendJump *pJump )
{
	if( !pJump || pJump->GetType() != JT_BADGUY_JUMP )
		return NULL;

	return dynamic_cast< ioBadGuyJump* >( pJump );
}

