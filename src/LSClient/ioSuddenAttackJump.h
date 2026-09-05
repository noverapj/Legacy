#pragma once

#include "ioAttackAttribute.h"
#include "ioExtendJump.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;

class ioSuddenAttackJump : public ioExtendJump
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
		CCS_NEW_JUMP,
		CCS_DASH_END,
		CCS_EXTEDN2_AIM,
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
	AttributeList m_ChargeComboAttackList;
	int m_iCurComboCnt;

	DWORD m_dwEnableComboTime_S;
	DWORD m_dwEnableComboTime_E;

	bool m_bReserveJumpAttack;

protected:
	DWORD m_dwChargingStartTime;
	DWORD m_dwChargingTime;

	ioHashStringVec m_ChargingAniList;
	ioHashString    m_Extend1ChargingAni;
	ioHashString    m_Extend2ChargingAni;

protected:
	DWORD m_dwFireStartTime;

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
	CEncrypt<bool> m_bEnableJumpDash;

	ComboDashType m_ComboDashType;
	AttackAttribute m_DashJumpAttack;

	float m_fJumpDashGravityAmt;
	DWORD m_dwJumpDashStartTime;
	DWORD m_dwJumpDashEndTime;

	ioHashString m_JumpDashLandAni;
	float m_fJumpDashLandAniRate;

	bool m_bUseNewJumpTypeDash;


	// charge attack
protected:
	CEncrypt<bool> m_bUsedChargeAttack;

	CEncrypt<float> m_fChargeGravityRate;
	CEncrypt<float> m_fChargeJumpPower;
	CEncrypt<float> m_fChargeBaseRange;

	float m_fCurCharMoveSpeed;

	D3DXVECTOR3 m_vCharMoveDir;
	DWORD m_dwFlightTime;

	ioHashString m_ChargeAttackLandAni;
	float m_fChargeAttackLandAniRate;


	//서든아이템 변신 모드값 저장
	int m_nWeaponItemMode;

protected:
	ioHashString m_stExtend1_Aim_Up;
	ioHashString m_stExtend1_Aim_Center;
	ioHashString m_stExtend1_Aim_Down;

	ioHashString m_stExtend1_Aim_Up_Fire;
	ioHashString m_stExtend1_Aim_Center_Fire;
	ioHashString m_stExtend1_Aim_Down_Fire;

	ioHashString m_stExtend2_Aim_Up;
	ioHashString m_stExtend2_Aim_Center;
	ioHashString m_stExtend2_Aim_Down;

	ioHashString m_stExtend2_Aim_Up_Fire;
	ioHashString m_stExtend2_Aim_Center_Fire;
	ioHashString m_stExtend2_Aim_Down_Fire;

	AttributeList m_Extend1ModeAttList;
	AttributeList m_Extend2ModeAttList;

	int				m_iChargeCombo;//총알 cnt
	bool			m_bExtend2FireReady;

	//AttackAttribute m_Extend1ModeAtt;
	//AttackAttribute m_Extend2ModeAtt;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioExtendJump* Clone();
	virtual JumpType GetType() const;

public:
	virtual void SetJumpping( ioBaseChar *pOwner, bool bFullTime );
	virtual void ProcessJumpping( ioBaseChar *pOwner, float fHeightGap, float fNeedGauge );
	virtual void ProcessLanding( ioBaseChar *pOwner );
	virtual void ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap );
	virtual void ApplyJump( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void OnEndExtendJump( ioBaseChar *pOwner );

	virtual DWORD GetChargingTime( ioBaseChar *pChar );

	virtual bool IsCanJumpingSkill( ioBaseChar *pOwner );

	virtual float GetExtraJumpSpeedRate( const D3DXVECTOR3 &vDir, const D3DXQUATERNION &qtRot ) const;
	virtual void SetLandingState( ioBaseChar *pOwner );
		
	int GetCurChargeComboState();

	// Normal JumpAttack
public:
	virtual void CheckComboJumpAttack( ioBaseChar *pOwner );

protected:
	void LoadComboList( ioINILoader &rkLoader );
	void LoadChargeComboList( ioINILoader &rkLoader );

	void SetComboJumpAttack( ioBaseChar *pOwner );

	void SetNormalModeAttack( ioBaseChar *pOwner );
	void SetExtend1ModeAttack( ioBaseChar *pOwner );
	void SetExtend2ModeAttack( ioBaseChar *pOwner );

	void ProcessComboJumpAttack( ioBaseChar *pOwner );

	void SetChargingState( ioBaseChar *pOwner );
	void ProcessCharging( ioBaseChar *pOwner );

	void SetAimingStateExtend2( ioBaseChar *pOwner );

	// charge attack
protected:
	void ChangeToPowerChargeJumpAttack( ioBaseChar *pOwner );
	void PowerChargeMove( ioBaseChar *pChar, ioPlayStage *pStage );
	float GetFlightDuration( float fCurGravity );

public:
	inline float GetChargeGravityRate() const { return m_fChargeGravityRate; }
	inline DWORD GetFlightTime() const { return m_dwFlightTime; }

	bool IsChargeAttackState();
	inline const ioHashString& GetChargeLandAni() const { return m_ChargeAttackLandAni; }
	inline float GetChargeLandAniRate() const { return m_fChargeAttackLandAniRate; }

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

	void AimedFireToUp( ioEntityGroup *pGrp,
		float fWeight,
		float fTimeRate,
		DWORD dwPreDelay,
		bool bJump,
		bool bHold, const ioHashString& stUpAni, const ioHashString& stCenterAni );
	void AimedFireToDown( ioEntityGroup *pGrp,
		float fWeight,
		float fTimeRate,
		DWORD dwPreDelay,
		bool bJump,
		bool bHold, const ioHashString& stDownAni, const ioHashString& stCenterAni );

	// Jump Dash
public:
	bool IsJumpDashState();
	bool IsJumpAttackReserve();

	const ioHashString& GetJumpDashLandAni() const { return m_JumpDashLandAni; }
	float GetJumpDashLandAniRate() const { return m_fJumpDashLandAniRate; }

	float GetCurGravityAmt();

	virtual bool IsEnableJumpLandEndDash();

protected:
	bool IsCanJumpDash( ioBaseChar *pOwner, float fHeightGap );

	void SetJumpDash( ioBaseChar *pOwner, float fHeightGap );
	void SetNewJumpTypeDashAttack( ioBaseChar *pOwner, DWORD dwEndTime, DWORD dwReserveTime );

	void ProcessJumpDash( ioBaseChar *pOwner );

protected:
	void TrackingExtend1( ioBaseChar *pChar );
	void TrackingExtend2( ioBaseChar *pChar );

	void ProcessAimExtend2( ioBaseChar *pChar );

public:
	ioSuddenAttackJump();
	ioSuddenAttackJump( const ioSuddenAttackJump &rhs );
	virtual ~ioSuddenAttackJump();
};

inline ioSuddenAttackJump* ToSuddenAttackJump( ioExtendJump *pJump )
{
	if( !pJump || pJump->GetType() != JT_SUDDEN_ATTACK_JUMP )
		return NULL;

	return dynamic_cast< ioSuddenAttackJump* >( pJump );
}

