#pragma once

#include "ioAttackAttribute.h"
#include "ioExtendJump.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;

class ioJoroExpansionJump : public ioExtendJump
{
public:
	enum ChageComboState
	{
		CCS_NONE,
		CCS_NORMAL,
		CCS_CHARGE,
		CCS_EXTEND_ATTACK,
		CCS_DOUBLE,
		CCS_DELAY,
		CCS_FLASH,
		CCS_FLASH_END,
		CCS_DASH,
		CCS_EXTEND_COMBO,
		CCS_NEW_JUMP,
		CCS_NEW_JUMP_CHARGE,
	};

	enum ComboDashType
	{
		CDT_GRAVITY		= 1,
		CDT_NO_GRAVITY	= 2,
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

protected:
	DWORD m_dwChargingStartTime;
	DWORD m_dwChargingTime;

	ioHashStringVec m_ChargingAniList;

protected:
	DWORD m_dwFireStartTime;
	DWORD m_dwCreateWeaponIndex;

	UniqueObjID m_GuidEffectID;

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
	//////////////////////////////////////////////////////////////////////////
	//홍길동 초월 각성때 chargecobo3 기능 추가함
	ComboDashType m_ComboDashType;

	AttackAttribute m_DashJumpAttack;
	float m_fJumpDashGravityAmt;

	ioHashString m_JumpDashLandAni;
	float m_fJumpDashLandAniRate;

	bool m_bUseNewJumpTypeDash;

	bool m_bUseJumpDashRotate;

	bool m_bUseDoubleJumpNewJumpDash;


	//공중 1타후 뉴점프대쉬가 가능한지옵션
	//1타는 하드 코딩
	bool m_bUseAttackAfterNewJumpDash;
	//////////////////////////////////////////////////////////////////////////


	CEncrypt<bool> m_bEnableJumpDash;

	float m_fJumpDashAniRate;
	ioHashStringVec m_vJumpDashAniList;

	ForceInfoList m_vForceInfoList;
	DWORD m_dwActionEndTime;

	bool m_bDashEnd;
	bool m_bJumpAttackReserve;

	// charge attack
protected:
	CEncrypt<bool> m_bUsedChargeAttack;

	CEncrypt<float> m_fChargeGravityRate;
	CEncrypt<float> m_fChargeJumpPower;
	CEncrypt<float> m_fChargeBaseRange;

	float m_fCurCharMoveSpeed;

	D3DXVECTOR3 m_vCharMoveDir;
	DWORD m_dwFlightTime;

	bool m_bEnableJumpState;
	ioHashString m_szCurAni;
	float m_fCurAniRate;
	DWORD m_dwInputCancelCheckTime;

	//메이드
	//weapon에 있는 점프 max숫자만큼 뛰어도 마지막 타는 charge공격은 나가게 셋팅하는거
protected:
	bool m_bMaxJumpLastChargeAttack;

	// jump extend type
protected:
	// extend combo
	AttributeList m_ExtendAttackList;
	AttackAttribute m_ExtendLandAttack;


public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioExtendJump* Clone();
	virtual JumpType GetType() const;

public:
	virtual void SetJumpping( ioBaseChar *pOwner, bool bFullTime );
	virtual void ProcessJumpping( ioBaseChar *pOwner, float fHeightGap, float fNeedGauge );
	virtual void ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap );
	virtual void ApplyJump( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void OnEndExtendJump( ioBaseChar *pOwner );

	virtual DWORD GetChargingTime( ioBaseChar *pChar );

	virtual bool IsCanJumpingSkill( ioBaseChar *pOwner );

	virtual float GetExtraJumpSpeedRate( const D3DXVECTOR3 &vDir, const D3DXQUATERNION &qtRot ) const;
	virtual void SetLandingState( ioBaseChar *pOwner );

	bool IsNoFlashProcessMove();
	bool IsNoProcessGravity();
	int GetCurChargeComboState();

	virtual bool IsEnableJumpDash( ioBaseChar *pOwner );

	virtual bool IsCanJumpAttack( ioBaseChar *pOwner );

	// Normal JumpAttack
public:
	virtual void CheckComboJumpAttack( ioBaseChar *pOwner );

protected:
	void LoadComboList( ioINILoader &rkLoader );

	void SetComboJumpAttack( ioBaseChar *pOwner );
	void ProcessComboJumpAttack( ioBaseChar *pOwner );

	void SetChargingState( ioBaseChar *pOwner, bool bException = false );
	void ProcessCharging( ioBaseChar *pOwner );

	void CheckCancelReserve( ioBaseChar *pOwner );
	bool ProcessCancel( ioBaseChar *pOwner );

	// charge attack
protected:
	void ChangeToPowerChargeJumpAttack( ioBaseChar *pOwner, bool bSendNet = true );
	void PowerChargeMove( ioBaseChar *pChar, ioPlayStage *pStage );
	float GetFlightDuration( float fCurGravity );

public:
	inline float GetChargeGravityRate() const { return m_fChargeGravityRate; }
	inline DWORD GetFlightTime() const { return m_dwFlightTime; }

	bool IsChargeAttackState();
	inline const ioHashString& GetChargeLandAni() const { return m_ExtendLandAttack.m_AttackAnimation; }
	inline float GetChargeLandAniRate() const { return m_ExtendLandAttack.m_fAttackAniRate; }

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
	bool IsCanJumpDashAfterNormalAttack( ioBaseChar *pOwner, float fHeightGap );
	int GetJumpDashAni( ioBaseChar *pOwner );

	void ProcessJumpDash( ioBaseChar *pOwner );

	virtual void CheckKeyReserve( ioBaseChar *pOwner );

	void SetNewJumpTypeDashAttack( ioBaseChar *pOwner, DWORD dwEndTime, DWORD dwReserveTime );

	bool SetDashAttack( ioBaseChar *pOwner,  OUT int &iAniID, OUT float &fTimeRate );
	bool CheckGaugeJump( ioBaseChar *pOwner );
	void DecreaseItemGauge( ioBaseChar *pOwner );


	// extend combo
protected:
	bool CheckEnableExtendCombo( ioBaseChar *pOwner );
	void SetExtendComboState( ioBaseChar *pOwner, bool bRefresh );

	void CheckExtendComboAttack( ioBaseChar *pOwner );
	void ProcessExtendComboAttack( ioBaseChar *pOwner );


	//메이드때 추가
public:
	bool IsCanMaxJumpAndChargeAttack();

public:
	const AttackAttribute& GetLandAttackAttribute() const  { return m_ExtendLandAttack; }

public:
	ioJoroExpansionJump();
	ioJoroExpansionJump( const ioJoroExpansionJump &rhs );
	virtual ~ioJoroExpansionJump();
};

inline ioJoroExpansionJump* ToJoroExpansionJump( ioExtendJump *pJump )
{
	if( !pJump || pJump->GetType() != JT_JORO_EXPANSION_JUMP )
		return NULL;

	return dynamic_cast< ioJoroExpansionJump* >( pJump );
}

