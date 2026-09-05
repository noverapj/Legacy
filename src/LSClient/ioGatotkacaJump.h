#pragma once

#include "ioAttackAttribute.h"
#include "ioExtendJump.h"
#include "ioGatotkacaItem.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;

class ioGatotkacaJump : public ioExtendJump
{
public:
	enum ChageComboState
	{
		CCS_NONE,
		CCS_NORMAL,
		CCS_CHARGE,
		CCS_DOUBLE,
		CCS_DELAY,
		CCS_FLASH,
		CCS_FLASH_END,
		CCS_DASH,
		CCS_END,
		CCS_NEW_JUMP,
		CCS_CHARGE_FLY,
	};

protected:
	ChageComboState m_ChargeComboState;

	float m_fJumpSpeedRateFR;
	float m_fJumpSpeedRateSI;
	float m_fJumpSpeedRateBG;

	bool m_bUsedComboAttack;
	bool m_bUsedDoubleJump;
	bool m_bUsedFlash;

	float m_fExtraJumpPowerRate;
	DWORD m_dwJumpChargeTime;

	// NormalJump Attack
protected:
	AttributeList m_ComboAttackList;
	int m_iCurComboCnt;

	DWORD m_dwEnableComboTime_S;
	DWORD m_dwEnableComboTime_E;

	bool m_bReserveJumpAttack;

	// Charging Jump Attack
protected:
	ChargeFlyInfo m_AttackChargeFly;

	ioHashString	m_szChargingAni;
	CEncrypt<DWORD>	m_dwChargingTime;
	DWORD			m_dwChargingStartTime;

	CEncrypt<bool> m_bUsedChargeFly;

protected:
	DWORD m_dwFireStartTime;

	// FlashJump Attack
protected:
	CEncrypt<bool> m_bEnableFlashJump;

	ioHashString m_FlashDelayAni;
	CEncrypt<float> m_fFlashDelayRate;

	CEncrypt<float> m_fFlashForce;
	CEncrypt<float> m_fFlashGravityAmt;

	DWORD m_dwFlashDelayEndTime;
	DWORD m_dwFlashEndTime;

	AttackAttribute m_FlashJumpAttack;

	// DoubleJump
protected:
	CEncrypt<bool> m_bEnableDoubleJump;

	float m_fDoubleJumpPower;
	float m_fDoubleJumpForce;
	float m_fDoubleJumpEnableHeight;
	DWORD m_dwDoubleJumpEnableTime;

	float m_fDoubleJumpAniRate;
	ioHashStringVec m_vDoubleJumpAniList;

	int m_iMaxJumpCnt;
	int m_iCurJumpCnt;

	float m_fDoubleJumpSpeedRateFR;
	float m_fDoubleJumpSpeedRateSI;
	float m_fDoubleJumpSpeedRateBG;

	// Jump Dash
protected:
	CEncrypt<bool> m_bEnableJumpDash;
	AttackAttribute m_JumpDashAttack;
	float m_fJumpDashAttackGravity;

	DWORD m_dwActionEndTime;
	bool m_bJumpAttackReserve;

	DWORD m_dwCancelJumpTime;
	bool m_bEnableCancelJump;
	float m_fCancelJumpAmt;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioExtendJump* Clone();
	virtual JumpType GetType() const;
	virtual bool IsEnableJumpDash( ioBaseChar *pOwner );

public:
	virtual void SetJumpping( ioBaseChar *pOwner, bool bFullTime );
	virtual void ProcessJumpping( ioBaseChar *pOwner, float fHeightGap, float fNeedGauge );
	virtual void ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap );
	virtual void ApplyJump( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual DWORD GetChargingTime( ioBaseChar *pChar );

	virtual bool IsCanJumpingSkill( ioBaseChar *pOwner );

	virtual float GetExtraJumpSpeedRate( const D3DXVECTOR3 &vDir, const D3DXQUATERNION &qtRot ) const;
	virtual void SetLandingState( ioBaseChar *pOwner );
	
	int GetCurChargeComboState();

	// Normal JumpAttack
public:
	virtual void CheckComboJumpAttack( ioBaseChar *pOwner );
	void CheckNewComboJumpAttack( ioBaseChar *pOwner );

protected:
	void LoadComboList( ioINILoader &rkLoader );

	void SetComboJumpAttack( ioBaseChar *pOwner );
	void ProcessComboJumpAttack( ioBaseChar *pOwner );

	void SetNewComboJumpAttack( ioBaseChar *pOwner, float fGravityAmt, bool bRefresh );
	void ProcessNewComboJumpAttack( ioBaseChar *pOwner );

	void SetJumpDashAttack( ioBaseChar *pOwner, float fGravityAmt, bool bRefresh );
	void ProcessJumpDash( ioBaseChar *pOwner );

	// gather
protected:
	void SetChargingState( ioBaseChar *pOwner );
	void SetChargeFlyState( ioBaseChar *pOwner );

	void ProcessCharging( ioBaseChar *pOwner );

public:
	inline const bool IsUsedChargeFly() const { return m_bUsedChargeFly; }
	inline void ClearChargeFly()		{ m_bUsedChargeFly = false; }

	// Double Jump
protected:
	bool IsCanDoubleJump( ioBaseChar *pOwner, float fHeightGap );
	void SetDoubleJump( ioBaseChar *pOwner );
	void SetDoubleJumpAni( ioBaseChar *pOwner );

	// Flash Jump
public:
	bool SetFlashJumpAttack( ioBaseChar *pOwner );
	float GetCurFlashForceAmt();
	float GetCurFlashGravityAmt();

	bool IsCanFlashJumpAttack( ioBaseChar *pOwner );

protected:
	void ProcessDelay( ioBaseChar *pOwner );
	void ProcessFlashMove( ioBaseChar *pOwner );
	void ProcessFlashEndMove( ioBaseChar *pOwner );

	// Jump Dash
public:
	bool IsJumpDashState();
	bool IsJumpAttackReserve();

protected:
	bool IsCanJumpDash( ioBaseChar *pOwner, float fHeightGap );
	
	virtual void CheckKeyReserve( ioBaseChar *pOwner );

public:
	ioGatotkacaJump();
	ioGatotkacaJump( const ioGatotkacaJump &rhs );
	virtual ~ioGatotkacaJump();
};

inline ioGatotkacaJump* ToGatotkacaJump( ioExtendJump *pJump )
{
	if( !pJump || pJump->GetType() != JT_GATOTKACA_JUMP )
		return NULL;

	return dynamic_cast< ioGatotkacaJump* >( pJump );
}

