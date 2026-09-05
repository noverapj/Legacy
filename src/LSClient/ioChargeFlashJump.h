#pragma once

#include "ioAttackAttribute.h"
#include "ioExtendJump.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;

class ioChargeFlashJump : public ioExtendJump
{
public:
	enum FlashJumpState
	{
		FJS_NONE,
		FJS_CHARGING,
		FJS_EXTEND_ATTACK,
		FJS_DELAY,
		FJS_FLASH,
		FJS_NORMAL,
		FJS_DOUBLE,
		FJS_DASH,
		FJS_DASH_END,
		FJS_NEW_JUMP,
	};

	enum LocalDashType
	{
		LDT_GRAVITY		= 1,
		LDT_NO_GRAVITY	= 2,
	};

protected:
	FlashJumpState m_FlashJumpState;

	ioHashString m_FlashDelayAni;
	CEncrypt<float> m_fFlashDelayRate;

	CEncrypt<float> m_fFlashForce;
	CEncrypt<float> m_fFlashGravityAmt;

	DWORD m_dwFlashDelayEndTime;
	DWORD m_dwFlashEndTime;

// charge
protected:
	DWORD m_dwChargingStartTime;
	DWORD m_dwChargingTime;
	ioHashString m_szChargingAni;

	bool m_bUseExtendJumpAttack;

// Attack
protected:
	AttackAttribute m_FlashJumpAttack;
	AttackAttribute m_ExtendJumpAttack;

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

	// Dash
	bool m_bEnableJumpDash;
	LocalDashType m_DashType;

	AttackAttribute m_DashJumpAttack;

	float m_fDashGravityAmt;
	DWORD m_dwDashStartTime;
	DWORD m_dwDashEndTime;

	ioHashString m_DashLandAni;
	float m_fDashLandAniRate;

	bool m_bJumpAttackReserve;
	bool m_bUseNewJumpTypeDash;

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

protected:
	void ProcessDelay( ioBaseChar *pOwner );
	void ProcessFlashMove( ioBaseChar *pOwner );
	void ProcessNormalMove( ioBaseChar *pOwner );

	bool IsCanDoubleJump( ioBaseChar *pOwner, float fHeightGap );
	void SetDoubleJumpAni( ioBaseChar *pOwner );

	void SetNewJumpTypeFlashAttack( ioBaseChar *pOwner, DWORD dwEndTime, DWORD dwReserveTime );

public:
	bool SetPowerChargeJumpAttack( ioBaseChar *pOwner );

protected:
	void ProcessCharging( ioBaseChar *pOwner );

	void ChangeToPowerChargeJumpAttack( ioBaseChar *pOwner );

// dash
public:
	virtual bool IsEnableJumpLandEndDash();

	void ProcessDash( ioBaseChar *pOwner );

	const ioHashString& GetDashLandAni() const { return m_DashLandAni; }
	float GetDashLandAniRate() const { return m_fDashLandAniRate; }

	virtual bool IsEnableJumpDash( ioBaseChar *pOwner );
	
	bool IsDashState();
	bool IsJumpAttackReserve();

	float GetCurGravityAmt();

protected:
	bool IsCanDash( ioBaseChar *pOwner, float fHeightGap );
	void SetDash( ioBaseChar *pOwner, float fHeightGap );

	void SetNewJumpTypeDashAttack( ioBaseChar *pOwner, DWORD dwEndTime, DWORD dwReserveTime );

	virtual void CheckKeyReserve( ioBaseChar *pOwner );

public:
	ioChargeFlashJump();
	ioChargeFlashJump( const ioChargeFlashJump &rhs );
	virtual ~ioChargeFlashJump();
};

inline ioChargeFlashJump* ToChargeFlashJump( ioExtendJump *pJump )
{
	if( !pJump || pJump->GetType() != JT_CHARGE_FLASH_JUMP )
		return NULL;

	return dynamic_cast< ioChargeFlashJump* >( pJump );
}

