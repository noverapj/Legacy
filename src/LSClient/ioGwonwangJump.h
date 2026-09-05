#pragma once

#include "ioExtendJump.h"
#include "ioAttackAttribute.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;

class ioGwonwangJump : public ioExtendJump
{
public:
	enum ComboState
	{
		CST_NONE,
		CST_DASH,
		CST_DASH_END,
		CST_NEW_JUMP,
	};

	enum ProcessJumpState
	{
		PJS_NONE,
		PJS_NORMAL_ATTACK,
		PJS_EXTEND_ATTACK,
		PJS_DASH_ATTACK,
		PJS_DASH_ADD_ATTACK,
		PJS_CHARGING_JUMP_ATTACK,
		PJS_END,
	};

	enum ComboDashType
	{
		CDT_GRAVITY		= 1,
		CDT_NO_GRAVITY	= 2,
	};

	enum SyncState
	{
		SSC_ATTACK,
		SSC_DASH,
		SSC_CANCEL_DASH,	// 캔슬 대시
		SSC_DASH_END,
		SSC_DOUBLE_JUMP,	// 2020-06-19
		SSC_EXTEND_ATTACK,
		SSC_DASH_ADD_ATTACK,
		SSC_CHANGE_ON,
		SS_CHARGING_JUMP,
		SS_CHARGING_JUMP_ATTACK,
	};

protected:
	ComboState m_ComboState;
	ProcessJumpState m_JumpState;

	AttributeList m_ComboAttackList;
	int m_iCurComboCnt;

	DWORD m_dwEnableComboTime_S;
	DWORD m_dwEnableComboTime_E;

	float m_fDoubleJumpSpeedRateFR;
	float m_fDoubleJumpSpeedRateSI;
	float m_fDoubleJumpSpeedRateBG;

	float m_fExtendJumpSpeedRateFR;
	float m_fExtendJumpSpeedRateSI;
	float m_fExtendJumpSpeedRateBG;

	bool m_bReserveJumpAttack;

	float m_fExtraJumpPowerRate;
	DWORD m_dwJumpChargeTime;
	DWORD m_dwChargingStartTime;

	// Dash
	bool m_bEnableJumpDash;
	ComboDashType m_ComboDashType;

	// 점프 대시 공격
	AttackAttribute m_DashJumpAttack;

	// 점프 기본공격 캔슬 대시
	AttackAttribute m_CancelDashJumpAttack;


	
	float m_fComboDashGravityAmt;
	DWORD m_dwComboDashStartTime;
	DWORD m_dwComboDashEndTime;
	DWORD m_dwComboDashEndJumpPower;

	ioHashString m_ComboDashLandAni;
	float m_fComboDashLandAniRate;
	bool m_bComboDashLandEnable;

	bool m_bUsedComboDash;
	bool m_bUsedCancelComboDash;	
	bool m_bUsedComboDashEnd;
	bool m_bUseNewJumpTypeDash;

	// DoubleJump 2020-06-19
protected:
	float m_fDoubleJumpPower;
	float m_fDoubleJumpForce;
	float m_fDoubleJumpEnableHeight;
	DWORD m_dwDoubleJumpEnableTime;

	float m_fDoubleJumpAniRate;
	ioHashStringVec m_vDoubleJumpAniList;

	bool m_bUsedDoubleJump;

	int m_iMaxJumpCnt;
	int m_iCurJumpCnt;


protected:
	TargetWoundType	m_TargetWoundType;
	CEncrypt<float>	m_fMoveRange;
	CEncrypt<float>	m_fMoveHeightRange;

	DWORD m_dwMotionEndTime;
	ioHashString	m_ReturnMark;
	DWORD m_dwReturnMarkDuration;

	ioHashStringVec	m_vTargetNameList;
	ioHashString	m_PreTargetName;
	D3DXVECTOR3		m_vReturnPosition;

protected:
	float m_fChargeJumpPower;
	float m_fChargeGravityRate;
	AttackAttribute m_ExtendLandAttack;

	bool m_bUsedChargeAttack;
	float m_fCurCharMoveSpeed;
	DWORD m_dwChargeAttackEndTime;

// 점프대쉬 이후 Add Attack
private:
	AttributeList	m_vJumpDashAddAttack;
	DWORD			m_dwJumpDashAddAttackReserveTime;
	int				m_iJumpDashAddAttackIndex;
	bool			m_bJumpChargeEnable;
	DWORD			m_dwJumpKeyChargingTime;
	
protected:
	// 공중 잡기
	ItemJumpAttackInfo	m_ItemLandAttackInfo;
	bool				m_bEnableHighJump;
	bool				m_bUseHeighJump;
	bool				m_bUseNormalJumpAttack;

	// 점프 차지
	ioHashString		m_stChargingAni;
	// 점프 차지 공격
	AttackAttribute		m_JumpChargeAttribute;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioExtendJump* Clone();
	virtual JumpType GetType() const;

public:
	virtual void SetJumpping( ioBaseChar *pOwner, bool bFullTime );
	virtual void ProcessJumpping( ioBaseChar *pOwner, float fHeightGap, float fNeedGaguge );
	virtual void ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap );
	virtual void ApplyJump( ioBaseChar *pOwner, SP2Packet &rkPacket );

	void ProcessComboDash( ioBaseChar *pOwner );

	virtual DWORD GetChargingTime( ioBaseChar *pChar );
	virtual void CheckComboJumpAttack( ioBaseChar *pOwner );
	
	virtual float GetExtraJumpSpeedRate( const D3DXVECTOR3 &vDir, const D3DXQUATERNION &qtRot ) const;
	virtual bool IsCanJumpingSkill( ioBaseChar *pOwner );

	virtual bool IsEnableJumpLandEndDash();

	const ioHashString& GetComboDashLandAni() const { return m_ComboDashLandAni; }
	float GetComboDashLandAniRate() const { return m_fComboDashLandAniRate; }
	bool IsComboDashLandEnable();
	bool IsComboDashLanded();
	void SetComboDashLand( ioBaseChar *pOwner, float fHeightGap );

	virtual bool IsEnableJumpDash( ioBaseChar *pOwner );
	bool IsComboDashState();

	float GetCurGravityAmt();

	virtual void SetLandingState( ioBaseChar *pOwner );

protected:
	void SetComboJumpAttack( ioBaseChar *pOwner );
	void LoadComboList( ioINILoader &rkLoader );

	bool IsCanComboDash( ioBaseChar *pOwner, float fHeightGap );
	void SetComboDash( ioBaseChar *pOwner, float fHeightGap );	

	// 더블 점프 2020-06-19
	bool IsCanDoubleJump( ioBaseChar *pOwner, float fHeightGap, bool bCharge );
	void SetDoubleJumpAni( ioBaseChar *pOwner );
	void SetDoubleJump( ioBaseChar *pOwner );


	bool IsCanJumpCharge( ioBaseChar *pOwner, float fHeightGap );
	void SetJumpChargingState( ioBaseChar *pOwner );
	bool CheckJumpCharging( ioBaseChar *pOwner );
	bool CheckAliceGun( ioBaseChar *pOwner );
	void SetNewJumpTypeDashAttack( ioBaseChar *pOwner, DWORD dwEndTime, DWORD dwReserveTime );
	void SetJumpDashAddAttack( ioBaseChar *pOwner, int iAttackIndex );
	void ChangeToJumpAttack( ioBaseChar *pOwner );

public:
	inline int GetCurrComboCnt(){ return m_iCurComboCnt; }

protected:
	void ChangeToPowerChargeJumpAttack( ioBaseChar *pOwner );
	void SetAutomatonState( ioBaseChar *pOwner );
	void SetChangeOnGriffinState( ioBaseChar *pOwner );

	void SetHanzoState( ioBaseChar *pOwner );

	void SetMidnightState( ioBaseChar* pOwner );

	// 점프 D~ 공중 잡기
	void SetChargingJumpState( ioBaseChar* pOwner );
	void SetChargeJumpAttack( ioBaseChar* pOwner );

public:
	bool IsEnableLandAttack();
	bool IsUsedHighJump();
	bool IsJumpAttackReserve();

public:
	inline float GetChargeGravityRate() const { return m_fChargeGravityRate; }
	bool IsChargeAttacked();
	bool IsComboDash()					{ return m_bUsedComboDash; }
	bool IsChargeEnd();
	bool IsCanMoveState();

public:
	void ResetJumpState();


public:
	inline const ioHashString& GetChargeLandAni() const { return m_ExtendLandAttack.m_AttackAnimation; }
	inline float GetChargeLandAniRate() const { return m_ExtendLandAttack.m_fAttackAniRate; }
	const AttackAttribute& GetLandAttackAttribute() const  { return m_ExtendLandAttack; }

public:
	ioGwonwangJump();
	ioGwonwangJump( const ioGwonwangJump &rhs );
	virtual ~ioGwonwangJump();
};

inline ioGwonwangJump* ToGwonwangJump( ioExtendJump *pJump )
{
	if( !pJump || pJump->GetType() != JT_GWONWANG_JUMP )
		return NULL;

	return dynamic_cast< ioGwonwangJump* >( pJump );
}

