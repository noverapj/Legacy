#pragma once

#include "ioExtendJump.h"
#include "ioAttackAttribute.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;

class ioKisaragiJump : public ioExtendJump
{
public:
	enum KisaragiJumpState
	{
		KJS_NONE	= 0,
		KJS_HIGH	= 1,
		KJS_DOUBLE	= 2,
		KJS_DASH	= 3,
	};

protected:
	KisaragiJumpState m_CurJumpState;

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

// Dash JumpAttack
protected:
	AttackAttribute m_DashJumpAttack;
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
	KisaragiJumpState GetJumpState() { return m_CurJumpState; }

protected:
	bool IsCanDoubleJump( ioBaseChar *pOwner, float fHeightGap );
	void SetDoubleJumpAni( ioBaseChar *pOwner );

	bool IsCanJumpDash( ioBaseChar *pOwner, float fHeightGap );
	void ProcessJumpDash( ioBaseChar *pOwner );
	
	virtual void CheckKeyReserve( ioBaseChar *pOwner );

public:
	ioKisaragiJump();
	ioKisaragiJump( const ioKisaragiJump &rhs );
	virtual ~ioKisaragiJump();
};

inline ioKisaragiJump* ToKisaragiJump( ioExtendJump *pJump )
{
	if( !pJump || pJump->GetType() != JT_KISARAGI_JUMP )
		return NULL;

	return dynamic_cast< ioKisaragiJump* >( pJump );
}

