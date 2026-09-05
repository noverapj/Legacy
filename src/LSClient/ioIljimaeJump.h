#pragma once

#include "ioExtendJump.h"
#include "ioAttackAttribute.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;

class ioIljimaeJump : public ioExtendJump
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
		PJS_TELEPORT,
		PJS_EXTEND_ATTACK,
		PJS_DASH_ATTACK,
		PJS_DASH_ADD_ATTACK,
		PJS_AZAZEL,
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
		SSC_DASH_END,
		SSC_DOUBLE_JUMP,
		SSC_CHANGE_TELEPORT,
		SSC_TELEPORT,
		SSC_EXTEND_ATTACK,
		SSC_DASH_ADD_ATTACK,
		SSC_FLY,
		SSC_DEFENSE,
		SSC_RELEASE_DEFENSE,
		SSC_AZAZEL,
		SSC_CHANGE_ON,
	};

protected:
	ComboState m_ComboState;
	ProcessJumpState m_JumpState;

	AttributeList m_ComboAttackList;
	int m_iCurComboCnt;

	DWORD m_dwEnableComboTime_S;
	DWORD m_dwEnableComboTime_E;
	DWORD m_dwNightmareKeyTime;

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
	
	float m_fComboDashGravityAmt;
	DWORD m_dwComboDashStartTime;
	DWORD m_dwComboDashEndTime;
	DWORD m_dwComboDashEndJumpPower;

	ioHashString m_ComboDashLandAni;
	float m_fComboDashLandAniRate;
	bool m_bComboDashLandEnable;

	bool m_bUsedComboDash;
	bool m_bUsedComboDashEnd;
	bool m_bUseNewJumpTypeDash;

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

	// Teleport
protected:
	ioHashString	m_TeleportMotion;
	CEncrypt<float>	m_fTeleportMotionRate;
	ioHashString	m_TeleportEndMotion;
	CEncrypt<float>	m_fTeleportEndMotionRate;

	TargetWoundType	m_TargetWoundType;

	CEncrypt<float>	m_fTeleportRange;
	CEncrypt<float>	m_fTeleportAngle;
	CEncrypt<float>	m_fMoveRange;
	CEncrypt<float>	m_fMoveHeightRange;

	DWORD m_dwMotionEndTime;
	ioHashString	m_ReturnMark;
	DWORD m_dwReturnMarkDuration;

	ioHashStringVec	m_vTargetNameList;
	ioHashString	m_PreTargetName;
	D3DXVECTOR3		m_vReturnPosition;

	CEncrypt<bool> m_bUsedTeleport;
	bool m_bUsedNightmare;

	bool m_bUsedAzazelAttack;
	DWORD m_dwAzazelFireTime;
	float m_fAzazelRange;
	float m_fAzazelAngle;
	float m_fAzazelEndJumpAmt;
	ioHashString m_AzazelAimedTarget;
	AttackAttribute m_AzazelExtendAttack;
	D3DXVECTOR3 m_vAzazelAttackOffset;

protected:
	float m_fChargeJumpPower;
	float m_fChargeGravityRate;
	AttackAttribute m_ExtendLandAttack;

	bool m_bUsedChargeAttack;
	float m_fCurCharMoveSpeed;
	DWORD m_dwChargeAttackEndTime;

// 점프대쉬 이후 Add Attack
private:
	AttributeList m_vJumpDashAddAttack;
	DWORD m_dwJumpDashAddAttackReserveTime;
	int m_iJumpDashAddAttackIndex;

	bool m_bEnableDefense;
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
	float m_fClearJumpPowerStartDefence;

	bool m_bJumpChargeEnable;
	DWORD m_dwJumpKeyChargingTime;

	// 괴도 새 날리기
	std::vector< StTopMoveInfo > m_vLupinBirdMoveNormalInfo;
	StTopMoveInfo	m_LupinBirdMoveInfo;
	AttackAttribute m_LupinBird_Attack;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	void LoadLupinBirdInfo( ioINILoader &rkLoader );				// 괴도 새 날리기 (점프 공격) 정보 로드
	virtual ioExtendJump* Clone();
	virtual JumpType GetType() const;

public:
	virtual void InitJump();
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

	inline const bool IsUsedTeleport() const { return m_bUsedTeleport; }

	virtual ioHashString GetJumpAniDefenseWounded();
	virtual ioHashString GetJumpAniDefenseCriticalWounded();

	bool IsCanDefense( ioBaseChar *pOwner, float fHeightGap );
	void SetDefense( ioBaseChar *pOwner );
	bool CheckDefenseRelease( ioBaseChar *pOwner );
	void ReleaseDefense( ioBaseChar *pOwner );

	inline bool IsEnableDefense() { return m_bEnableDefense; }
	inline const bool& IsJumpDefenceState() const { return m_bUsedDefense; }
	inline const ioHashString& GetJumpDefenseLandAni() const { return m_DefenseLandAni; }
	inline float GetJumpDefenseLandAniRate() const { return m_fDefenseLandAniRate; }

protected:
	void SetComboJumpAttack( ioBaseChar *pOwner );
	void LoadComboList( ioINILoader &rkLoader );

	bool IsCanComboDash( ioBaseChar *pOwner, float fHeightGap );
	void SetComboDash( ioBaseChar *pOwner, float fHeightGap );
	
	bool IsCanDoubleJump( ioBaseChar *pOwner, float fHeightGap, bool bCharge );
	void SetDoubleJumpAni( ioBaseChar *pOwner );
	void SetDoubleJump( ioBaseChar *pOwner );

	bool IsCanJumpCharge( ioBaseChar *pOwner, float fHeightGap );
	void SetJumpChargingState( ioBaseChar *pOwner );
	bool CheckJumpCharging( ioBaseChar *pOwner );

	void SetLupinBirdNormalAttack( ioBaseChar *pOwner, int nCombo );

	bool CheckAliceGun( ioBaseChar *pOwner );

	void SetNewJumpTypeDashAttack( ioBaseChar *pOwner, DWORD dwEndTime, DWORD dwReserveTime );
	void SetJumpDashAddAttack( ioBaseChar *pOwner, int iAttackIndex );

protected:
	void ProcessTeleport( ioBaseChar *pOwner );

	void ChangeToJumpAttack( ioBaseChar *pOwner );
	void ChangeToTeleport( ioBaseChar *pOwner );

public:
	inline int GetCurrComboCnt(){ return m_iCurComboCnt; }

protected:
	void ChangeToPowerChargeJumpAttack( ioBaseChar *pOwner );
	void SetNightmareState( ioBaseChar *pOwner );

	void SetAzazelState( ioBaseChar *pOwner, bool bAim );
	void SetAzazelAttackState( ioBaseChar *pOwner, bool bSendPacket, D3DXVECTOR3 vPos = D3DXVECTOR3(), D3DXVECTOR3 vDir = D3DXVECTOR3() );
	void SetAzazelPushState( ioBaseChar *pOwner );
	void SetAzazelTeleportState( ioBaseChar *pOwner );
	void ProcessAzazelAttack( ioBaseChar *pOwner );

	void SetAutomatonState( ioBaseChar *pOwner );
	void SetChangeOnGriffinState( ioBaseChar *pOwner );

	void SetHanzoState( ioBaseChar *pOwner );

	void SetMidnightState( ioBaseChar* pOwner );

	void SetTyrState( ioBaseChar* pOwner );

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
	ioIljimaeJump();
	ioIljimaeJump( const ioIljimaeJump &rhs );
	virtual ~ioIljimaeJump();
};

inline ioIljimaeJump* ToIljimaeJump( ioExtendJump *pJump )
{
	if( !pJump || pJump->GetType() != JT_ILJIMAE_JUMP )
		return NULL;

	return dynamic_cast< ioIljimaeJump* >( pJump );
}

