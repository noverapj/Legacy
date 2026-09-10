#pragma once

#include "ioAttackAttribute.h"
#include "ioExtendJump.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;

class ioTraceJump : public ioExtendJump
{
public:
	enum ChageComboState
	{
		CCS_NONE,
		CCS_NORMAL,
		CCS_CHARGE,
		CCS_EXTEND_ATTACK,
		CCS_DOUBLE,
		CCS_DASH,
		CCS_EXTEND_COMBO,
		CCS_NEW_JUMP,
		CCS_DASH_END,
		CCS_ALL_FIRE,
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
	//게이지 소비에 따른 AttackAttribute를 소환하자
	AttributeList m_TraceComboAttackList;
	AttributeList m_ChargeComboAttackList;
	AttributeList m_AllFireAttack;

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

	ioHashStringVec m_vJumpDashAniList;

	float m_fJumpDashGravityAmt;
	DWORD m_dwJumpDashStartTime;
	DWORD m_dwJumpDashEndTime;

	float m_fDashEndJumpPower;

	ioHashString m_JumpDashLandAni;
	float m_fJumpDashLandAniRate;

	bool m_bUseNewJumpTypeDash;

	//S키로 공격
	DWORD m_dwAllFireAttackStartTime;
	DWORD m_dwAllFireAttackEndTime;


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

	// jump extend type
protected:

	// extend combo
	AttributeList m_ExtendAttackList;


	//착지를 하지 않으면 계속 dash를 사용 가능해서 추가함
	bool m_bLanded;
	bool m_bFirstJump;
	int  m_nDecGaugeCnt;

	//weapon에 있는 점프 max숫자만큼 뛰어도 마지막 타는 charge공격은 나가게 셋팅하는거
protected:
	bool m_bMaxJumpLastChargeAttack;

	float m_fNeedDashGauge;
	float m_fNeedJumpGauge;
	float m_fNeedChagingGauge;
	float m_fNeedAllFireGauge;

	DWORD m_dwDashKeyReserveTime;
	bool  m_bDefenseKey;
	bool  m_bAttackKey;

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

	virtual bool IsEnableJumpDash( ioBaseChar *pOwner );

	int GetCurChargeComboState();

	// Normal JumpAttack
public:
	virtual void CheckComboJumpAttack( ioBaseChar *pOwner );

protected:
	void LoadTraceComboList( ioINILoader &rkLoader );
	void LoadChargeComboList( ioINILoader &rkLoader );
	void LoadAllFireComboList( ioINILoader &rkLoader );

	void SetComboJumpAttack( ioBaseChar *pOwner );
	void SetComboChargeJumpAttack( ioBaseChar *pOwner );
	void ProcessComboJumpAttack( ioBaseChar *pOwner );

	void SetChargingState( ioBaseChar *pOwner, bool bException = false );
	void ProcessCharging( ioBaseChar *pOwner );

	// charge attack
protected:
	void ChangeChargeJumpAttack( ioBaseChar *pOwner );
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
	int GetJumpDashAni( ioBaseChar *pOwner );

	void SetJumpDash( ioBaseChar *pOwner, float fHeightGap );
	void SetNewJumpTypeDashAttack( ioBaseChar *pOwner, DWORD dwEndTime, DWORD dwReserveTime );

	void ProcessJumpDash( ioBaseChar *pOwner );

	bool IsCanAllFireAttack( ioBaseChar *pOwner );
	void SetAllFireAttack( ioBaseChar *pOwner );


	// extend combo
protected:
	bool CheckEnableExtendCombo( ioBaseChar *pOwner );
	void SetExtendComboState( ioBaseChar *pOwner, bool bRefresh );

	void CheckExtendComboAttack( ioBaseChar *pOwner );
	void ProcessExtendComboAttack( ioBaseChar *pOwner );

public:
	bool IsCanMaxJumpAndChargeAttack( ioBaseChar *pOwner );


public:
	ioTraceJump();
	ioTraceJump( const ioTraceJump &rhs );
	virtual ~ioTraceJump();
};

inline ioTraceJump* ToTraceJump( ioExtendJump *pJump )
{
	if( !pJump || pJump->GetType() != JT_TRACE_JUMP )
		return NULL;

	return dynamic_cast< ioTraceJump* >( pJump );
}

