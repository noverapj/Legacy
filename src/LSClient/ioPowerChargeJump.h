#pragma once

#include "ioAttackAttribute.h"
#include "ioExtendJump.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;

class ioPowerChargeJump : public ioExtendJump
{
public:
	enum PowerChargeJumpState
	{
		PJS_NONE,
		PJS_CHARGING,
		PJS_GATHERING,
		PJS_NORMAL_ATTACK,
		PJS_EXTEND_ATTACK,
		PJS_DASH,
		PJS_DASH_END,
		PJS_NEW_JUMP,
	};

	enum LocalDashType
	{
		LDT_GRAVITY		= 1,
		LDT_NO_GRAVITY	= 2,
	};

protected:
	PowerChargeJumpState m_JumpState;

	AttackAttribute m_JumpAttack;
	AttackAttribute m_ExtendJumpAttack;
	AttackAttribute m_LandJumpAttack;

	bool m_bUseLandJumpAttack;
	bool m_bUseExtendJumpAttack;
	bool m_bEnableReservedSliding;

	DWORD m_dwChargingStartTime;
	DWORD m_dwChargingTime;
	ioHashString m_szChargingAni;
	ioHashString m_szGahteringAni;

	CEncrypt<float> m_fGravityAmt;

	DWORD m_dwNormalAttackEndTime;

	float m_fExtraJumpPowerRate;
	DWORD m_dwJumpChargeTime;

	// Dash
	bool m_bEnableJumpDash;
	LocalDashType m_DashType;

	AttackAttribute m_DashJumpAttack;

	float m_fDashGravityAmt;
	DWORD m_dwDashStartTime;
	DWORD m_dwDashEndTime;

	ioHashString m_DashLandAni;
	float m_fDashLandAniRate;

	bool m_bUseNewJumpTypeDash;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioExtendJump* Clone();
	virtual JumpType GetType() const;

public:
	virtual void SetJumpping( ioBaseChar *pOwner, bool bFullTime );
	virtual void ProcessJumpping( ioBaseChar *pOwner, float fHeightGap, float fNeedGaguge );
	virtual void ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap );
	virtual void ApplyJump( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual void SetLandingState( ioBaseChar *pOwner );

	virtual DWORD GetChargingTime( ioBaseChar *pChar );

	bool SetPowerChargeJumpAttack( ioBaseChar *pOwner );
	bool IsUseLandJumpAttack();
	bool IsUseExtendJumpAttack();

	float GetCurGravityAmt();

protected:
	void ProcessCharging( ioBaseChar *pOwner );
	void ProcessGathering( ioBaseChar *pOwner );
	
	void ChangeToGathering( ioBaseChar *pOwner );
	void ChangeToNormalJumpAttack( ioBaseChar *pOwner );
	void ChangeToPowerChargeJumpAttack( ioBaseChar *pOwner );

// dash
public:
	virtual bool IsEnableJumpLandEndDash();

	void ProcessDash( ioBaseChar *pOwner );

	const ioHashString& GetDashLandAni() const { return m_DashLandAni; }
	float GetDashLandAniRate() const { return m_fDashLandAniRate; }

	virtual bool IsEnableJumpDash( ioBaseChar *pOwner );
	bool IsDashState();

	const PowerChargeJumpState GetJumpState() { return m_JumpState; }

protected:
	bool IsCanDash( ioBaseChar *pOwner, float fHeightGap );
	void SetDash( ioBaseChar *pOwner, float fHeightGap );

	void SetNewJumpTypeDashAttack( ioBaseChar *pOwner, DWORD dwEndTime, DWORD dwReserveTime );

public:
	ioPowerChargeJump();
	ioPowerChargeJump( const ioPowerChargeJump &rhs );
	virtual ~ioPowerChargeJump();
};

inline ioPowerChargeJump* ToPowerChargeJump( ioExtendJump *pJump )
{
	if( !pJump || pJump->GetType() != JT_POWER_CHARGE_JUMP )
		return NULL;

	return dynamic_cast< ioPowerChargeJump* >( pJump );
}

