#pragma once

#include "ioAttackAttribute.h"
#include "ioExtendJump.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;

class ioFlashJump : public ioExtendJump
{
public:
	enum FlashJumpState
	{
		FJS_NONE,
		FJS_DELAY,
		FJS_FLASH,
		FJS_NORMAL,
		FJS_DOUBLE,
	};

protected:
	FlashJumpState m_FlashJumpState;

	ioHashString m_FlashDelayAni;
	CEncrypt<float> m_fFlashDelayRate;

	CEncrypt<float> m_fFlashForce;
	CEncrypt<float> m_fFlashGravityAmt;

	DWORD m_dwFlashDelayEndTime;
	DWORD m_dwFlashEndTime;

// FlashJump Attack
protected:
	AttackAttribute m_FlashJumpAttack;

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

	int m_iCurChangeJumpCnt;

// etc
protected:
	bool m_bUseNewJumpTypeFlash;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioExtendJump* Clone();
	virtual JumpType GetType() const;

public:
	virtual void SetJumpping( ioBaseChar *pOwner, bool bFullTime );
	virtual void ProcessJumpping( ioBaseChar *pOwner, float fHeightGap, float fNeedGauge );
	virtual void ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap );
	virtual void ApplyJump( ioBaseChar *pOwner, SP2Packet &rkPacket );

	bool SetFlashJumpAttack( ioBaseChar *pOwner );
	float GetCurFlashForceAmt();
	float GetCurFlashGravityAmt();

	virtual void SetLandingState( ioBaseChar *pOwner );

	bool IsNoFlashProcessMove();
	bool IsCanFlashJumpAttack( ioBaseChar *pOwner );
	bool IsFlashJumpState();

protected:
	void ProcessDelay( ioBaseChar *pOwner );
	void ProcessFlashMove( ioBaseChar *pOwner );
	void ProcessNormalMove( ioBaseChar *pOwner );

	bool IsCanDoubleJump( ioBaseChar *pOwner, float fHeightGap );
	void SetDoubleJumpAni( ioBaseChar *pOwner );

	void SetNewJumpTypeFlashAttack( ioBaseChar *pOwner, DWORD dwEndTime, DWORD dwReserveTime );

public:
	ioFlashJump();
	ioFlashJump( const ioFlashJump &rhs );
	virtual ~ioFlashJump();
};

inline ioFlashJump* ToFlashJump( ioExtendJump *pJump )
{
	if( !pJump || pJump->GetType() != JT_FLASH_JUMP )
		return NULL;

	return dynamic_cast< ioFlashJump* >( pJump );
}

