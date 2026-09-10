#pragma once

#include "ioExtendJump.h"
#include "ioAttackAttribute.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;

class ioComboJump : public ioExtendJump
{
public:
	enum ComboState
	{
		CST_NONE,
		CST_DASH,
		CST_DASH_END,
		CST_NEW_JUMP,
	};

	enum ComboDashType
	{
		CDT_GRAVITY		= 1,
		CDT_NO_GRAVITY	= 2,
	};

	enum SyncState
	{
		SSC_ATTACK,
		SSC_ATTACK_BRANCH_ANI,
		SSC_DASH,
		SSC_DOUBLE_JUMP,
		SCC_DASH_CANCEL,
	};

	// JumpAttack Branch ani 추가로 인한 스텝 플래그 추가
	enum JumpAttackStep
	{
		JAS_NONE,
		JAS_JUMP,
		JAS_JUMP_ATTACK_BRANCH,
		JAS_JUMP_ATTACK
	};

protected:
	ComboState m_ComboState;

	AttributeList m_ComboAttackList;
	AttributeList m_ComboExtendAttackList;
	AttributeList* m_pCurrentComboAttackList;
	int m_iCurComboCnt;
	bool m_bUseExtendAttack;

	DWORD m_dwEnableComboTime_S;
	DWORD m_dwEnableComboTime_E;

	float m_fComboJumpSpeedRateFR;
	float m_fComboJumpSpeedRateSI;
	float m_fComboJumpSpeedRateBG;

	bool m_bReserveJumpAttack;

	float m_fExtraJumpPowerRate;
	DWORD m_dwJumpChargeTime;

	// Dash
	bool m_bEnableJumpDash;
	ComboDashType m_ComboDashType;

	AttackAttribute m_DashJumpAttack;
	
	float m_fComboDashGravityAmt;
	DWORD m_dwComboDashStartTime;
	DWORD m_dwComboDashEndTime;

	ioHashString m_ComboDashLandAni;
	float m_fComboDashLandAniRate;

	bool m_bUseNewJumpTypeDash;

	bool m_bCheckJumpLimitCnt;
	bool m_bEnableDashCancelJump;
	bool m_bUsedJumpDashCancel;

	float m_fJumpDashCancelJumpPower;

protected:
	float m_fDoubleJumpPower;
	float m_fDoubleJumpForce;
	float m_fDoubleJumpEnableHeight;
	DWORD m_dwDoubleJumpEnableTime;

	float m_fDoubleJumpAniRate;
	ioHashStringVec m_vDoubleJumpAniList;

	bool m_bUsedDoubleJump;

	bool m_bUseHighJumpAnimation;
	ioHashString m_szHighJumpAnimation;

	int m_iMaxJumpCnt;
	int m_iCurJumpCnt;

	// JumpAttack branch ani
	bool m_bJumpAttackBranchAni;
	JumpAttackStep m_JumpAttackStep;
	DWORD m_dwJumpAttackStartTime;
	DWORD m_dwJumpAttackTime;
	DWORD m_dwJumpAttackBranchDurationTime;
	ioHashString m_szJumpAttackBranchAni;

	// DrSpider 더미 탑승 점프 공격 시 air_jump
	bool m_bEnableAttackAirJump;

	float m_fAttackAirJumpPower;
	float m_fAttackAirForcePower;
	float m_fAttackAirJumpPowerRate;

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

	virtual bool IsEnableJumpDash( ioBaseChar *pOwner );
	bool IsComboDashState();

	float GetCurGravityAmt();

	virtual void SetLandingState( ioBaseChar *pOwner );

	bool IsCanDoubleJump( ioBaseChar *pOwner, float fHeightGap, bool bCharge );
	void SetDoubleJumpAni( ioBaseChar *pOwner );
	void SetDoubleJump( ioBaseChar *pOwner );

protected:
	bool SetCombo_Branch_JumpAttack( ioBaseChar *pOwner );
	void SetComboJumpAttack( ioBaseChar *pOwner );
	void LoadComboList( ioINILoader &rkLoader );

	bool IsCanComboDash( ioBaseChar *pOwner, float fHeightGap );
	void SetComboDash( ioBaseChar *pOwner, float fHeightGap );

	void SetNewJumpTypeDashAttack( ioBaseChar *pOwner, DWORD dwEndTime, DWORD dwReserveTime );
	// 점프 공격 시 item.ini의 점프쪽 셋팅 파일에서 읽어들인 값에 따라 air_jump동작 되도록하는 함수
	void ExcuteAirJump( ioBaseChar* pOwner );

public:
	ioComboJump();
	ioComboJump( const ioComboJump &rhs );
	virtual ~ioComboJump();
};

inline ioComboJump* ToComboJump( ioExtendJump *pJump )
{
	if( !pJump || pJump->GetType() != JT_COMBO_JUMP )
		return NULL;

	return dynamic_cast< ioComboJump* >( pJump );
}

