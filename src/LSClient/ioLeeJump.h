#pragma once

#include "ioExtendJump.h"
#include "ioAttackAttribute.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;

class ioLeeJump : public ioExtendJump
{
public:
	enum ComboState
	{
		CST_NONE,
		CST_DASH,
		//CST_DASH_ADD_ATTACK,
		CST_DASH_END,
		CST_SPECIAL_DASH,
		CST_SPECIAL_DASH_END,
		CST_NEW_JUMP,
		CST_NEW_JUMP_ADD,
		CST_NEW_JUMP_END,
	};

	enum ProcessJumpState
	{
		PJS_NONE,
		PJS_NORMAL_ATTACK,
		PJS_NORMAL_ATTACKING,
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
		SSC_DASH_ADD,
		SSC_SPECIAL_DASH,
		SSC_SPECIAL_DASH_END,
		SSC_SPECIAL_DASH_CANCEL,
		SSC_DOUBLE_JUMP,
		SSC_CHANGE_TELEPORT,
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

	AttackAttribute m_DashJumpAttack;
	AttackAttribute m_DashJumpAddAttack;
	AttackAttribute m_SpecialDashJumpAttack;

	float m_fComboDashGravityAmt;
	DWORD m_dwComboDashStartTime;
	DWORD m_dwComboDashEndTime;
	DWORD m_dwComboDashReserveTime;

	DWORD m_dwSpecialDashStartTime;
	DWORD m_dwSpecialDashEndTime;


	ioHashString m_ComboDashLandAni;
	float m_fComboDashLandAniRate;

	bool m_bUseNewJumpTypeDash;

	float m_fSpecialLastMoveSpeed;

	float m_fDashAngleVert;
	float m_fDashAngleHorz;

	DWORD m_dwCancleKeyReserveTime;

	float m_fSpecialDashEndJumpPower;

	ioHashString m_stSpecialDashBuff;
	ioHashString m_stSpecialDashCancelBuff;

	ioHashString m_stSpecialDashEndJump;
	float m_fSpecialDashEndJumpRate;

	// DoubleJump
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

	bool m_bKeyUp;

protected:
	DWORD m_dwMotionEndTime;

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

protected:
	void SetComboJumpAttack( ioBaseChar *pOwner );
	void LoadComboList( ioINILoader &rkLoader );

	bool IsCanComboDash( ioBaseChar *pOwner, float fHeightGap );
	void SetComboDash( ioBaseChar *pOwner, float fHeightGap );
	void SetComboDashAddAttack( ioBaseChar *pOwner );
	void SetSpecailDash( ioBaseChar *pOwner, float fHeightGap );

	bool IsCanDoubleJump( ioBaseChar *pOwner, float fHeightGap );
	void SetDoubleJumpAni( ioBaseChar *pOwner );

	void SetNewJumpTypeDashAttack( ioBaseChar *pOwner, DWORD dwEndTime, DWORD dwReserveTime );
	void RotateMoveDir( ioBaseChar* pOwner, float fAngleH, float fAngleV );

	void SetCancelAttack( ioBaseChar *pOwner );

	bool IsCanLeeSpecailDash( ioBaseChar *pOwner );
	void DecreaseLeeItemGauge( ioBaseChar *pOwner );

	//////////////////////////////////////////////////////////////////////////
	void SetChargingState( ioBaseChar *pOwner, bool bException = false );
	void ProcessCharging( ioBaseChar *pOwner );

	void _ProcessNewJumpAttack( ioBaseChar *pOwner );

protected:

	void ChangeToJumpAttack( ioBaseChar *pOwner );
	void SetSpecialDashEnd( ioBaseChar *pOwner );

public:
	ioLeeJump();
	ioLeeJump( const ioLeeJump &rhs );
	virtual ~ioLeeJump();
};

inline ioLeeJump* ToLeeJump( ioExtendJump *pJump )
{
	if( !pJump || pJump->GetType() != JT_LEE_JUMP )
		return NULL;

	return dynamic_cast< ioLeeJump* >( pJump );
}

