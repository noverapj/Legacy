#pragma once

#include "ioAttackAttribute.h"
#include "ioExtendJump.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;

class ioChargeComboDefenseJump : public ioExtendJump
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
		CCS_DASH,
		CCS_DEFENSE,
		CCS_RELEASE_DEFENSE,
		CCS_EXTEND_COMBO,
	};

	enum JumpExtendType
	{
		JET_NONE,
		JET_EXTEND_COMBO,
	};

protected:
	float	m_fDefenseEnableHeight;
	bool	m_bUsedDefense;
	int		m_iMaxDefenseEnableCount;
	int		m_iCurDefenseEnableCount;

	float	m_fDefenseAniRate;
	DWORD	m_dwDefenseDuration;

	DWORD m_dwDefenseEnableTime;
	DWORD m_dwDefenseReleaseTime;

	ioHashString m_JumpDefenseAnimation;
	ioHashString m_JumpAniDefenseWounded;
	ioHashString m_JumpAniDefenseCriticalWounded;

	ioHashString m_DefenseLandAni;
	float m_fDefenseLandAniRate;

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

	// jump extend type
protected:
	JumpExtendType m_JumpExtendType;


	// extend combo
	AttributeList m_ExtendAttackList;
	AttackAttribute m_ExtendLandAttack;
protected:
	float m_fJumpDefenceGravityRate;
	float m_fClearJumpPowerStartDefence;

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
		
	int GetCurChargeComboState();

	// Normal JumpAttack
public:
	virtual void CheckComboJumpAttack( ioBaseChar *pOwner );

protected:
	void LoadComboList( ioINILoader &rkLoader );

	void SetComboJumpAttack( ioBaseChar *pOwner );
	void ProcessComboJumpAttack( ioBaseChar *pOwner );

	void SetChargingState( ioBaseChar *pOwner );
	void ProcessCharging( ioBaseChar *pOwner );

	// charge attack
protected:
	void ChangeToPowerChargeJumpAttack( ioBaseChar *pOwner );
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
	int GetJumpDashAni( ioBaseChar *pOwner );

	void ProcessJumpDash( ioBaseChar *pOwner );

	virtual void CheckKeyReserve( ioBaseChar *pOwner );

	// extend combo
protected:
	bool CheckEnableExtendCombo( ioBaseChar *pOwner );
	void SetExtendComboState( ioBaseChar *pOwner, bool bRefresh );

	void CheckExtendComboAttack( ioBaseChar *pOwner );
	void ProcessExtendComboAttack( ioBaseChar *pOwner );

public:
	const AttackAttribute& GetLandAttackAttribute() const  { return m_ExtendLandAttack; }

	//defense관련 부분
public:
	virtual ioHashString GetJumpAniDefenseWounded();
	virtual ioHashString GetJumpAniDefenseCriticalWounded();
	inline const ioHashString& GetJumpDefenseLandAni() const { return m_DefenseLandAni; }
	inline float GetJumpDefenseLandAniRate() const { return m_fDefenseLandAniRate; }

	inline const float& GetDefenceGravityRate() const { return m_fJumpDefenceGravityRate; }
	inline const bool& IsJumpDefenceState() const { return m_bUsedDefense; }

protected:
	bool IsCanDefense( ioBaseChar *pOwner, float fHeightGap );

	void SetDefense( ioBaseChar *pOwner );
	bool CheckDefenseRelease( ioBaseChar *pOwner );
	void ReleaseDefense( ioBaseChar *pOwner );

public:
	ioChargeComboDefenseJump();
	ioChargeComboDefenseJump( const ioChargeComboDefenseJump &rhs );
	virtual ~ioChargeComboDefenseJump();
};

inline ioChargeComboDefenseJump* ToChargeComboDefenseJump( ioExtendJump *pJump )
{
	if( !pJump || pJump->GetType() != JT_CHARGE_COMBO_DEFENSE )
		return NULL;

	return dynamic_cast< ioChargeComboDefenseJump* >( pJump );
}

