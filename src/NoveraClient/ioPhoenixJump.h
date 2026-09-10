#pragma once
#include "ioextendjump.h"
#include "ioAttackAttribute.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;

class ioPhoenixJump :	public ioExtendJump
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
		PJS_HALF_CHARGE,
		PJS_DASH,
		PJS_AIM,
		PJS_FIRE
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
		SSC_CHANGE_AIM,
		SSC_CHANGE_JUMP_ATTACK,
		SSC_FIRE,
		SSC_FIRE_END,
		SSC_HALF_CHARGE_ATTACK,
		SSC_COMBO_CHARGE_DASH,
		SSC_FULL_CHARGE_ATTACK
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

	int m_iCurAirJumpCnt;
	std::list<float> m_vAirJumpTimeList;

	float m_fAirJumpPower;
	float m_fAirForcePower;
	float m_fAirJumpPowerRate;

	bool m_bReserveJumpAttack;

	float m_fExtraJumpPowerRate;
	DWORD m_dwJumpChargeTime;
	DWORD m_dwChargingStartTime;
	DWORD m_dwFullJumpChargeTime;
	DWORD m_dwChargeComboMaintainTime;
	DWORD m_dwChargeComboStartTime;
	DWORD m_dwJumpHalfChargeTime;
	DWORD m_dwHalfChargeEndTime;

	bool m_bUsedChargeAttack;

	// Dash
	bool m_bEnableJumpDash;
	ComboDashType m_ComboDashType;

	AttackAttribute m_DashJumpAttack;
	AttackAttribute m_JumpComboChargeDash;
	AttackAttribute m_JumpHalfChargeAttack;

	DWORD m_dwEnableReserveKeyTime;
	DWORD m_dwMotionEndTime;
	DWORD m_dwDashMotionPreTime;

	float m_fComboDashGravityAmt;
	DWORD m_dwComboDashStartTime;
	DWORD m_dwComboDashEndTime;

	ioHashString m_ComboDashLandAni;
	float m_fComboDashLandAniRate;

	bool m_bUseNewJumpTypeDash;

	float m_fJumpDashEndJumpPower;
	float m_fJumpDashEndLandingRate;

	DWORD m_dwDashCheckEnableTime;

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

	bool m_bUsedChargeFire;
	bool m_bUsedDash;

	//chargeCombo
protected:
	ioHashString m_JumpAimUp;
	ioHashString m_JumpAimCenter;
	ioHashString m_JumpAimDown;

	AttributeList	m_vChargeComboAttList;
	D3DXVECTOR3		m_vAimedDir;

	bool			m_bFirstAimBullet;//Ã¹¹ß
	ioHashString	m_AimedTarget;
	ioHashString	m_FirstAimedTarget;//Ã·ÇÁÈÄ Ã³À½ ½ºÄËµÈ Å¸°Ù
	int				m_iChargeCombo;//ÃÑ¾Ë cnt
	int				m_iMaxChargeCombo;

	ioHashString	m_stFireUpAni;
	ioHashString	m_stFireCenterAni;
	ioHashString	m_stFireDownAni;

	float			m_fMarkerRange;
	ioHashString	m_szReadyMarker;
	ioHashString	m_szAimingMarker;
	ioHashString	m_szAimedMarker;

	DWORD			m_dwKeyReserveTime;
	DWORD			m_dwFireStartTime;
	DWORD			m_dwFireEndTime;

	bool			m_bCheckFirstTarget;

	struct JumpMotionInfo
	{
		ioHashString m_FireUp;
		ioHashString m_FireCenter;
		ioHashString m_FireDown;
	};
	typedef std::vector< JumpMotionInfo > JumpMotionInfoList;
	JumpMotionInfoList m_JumpMotionInfoList;

	void SetChangeFireMotion( ioBaseChar *pOwner);
	float GetAimedUpDownWeight( const D3DXVECTOR3 &vAimDir, float fMaxAngle );
	void AimedFireToUp( ioEntityGroup *pGrp,
		float fWeight,
		float fTimeRate,
		DWORD dwPreDelay,
		bool bJump,
		bool bHold );
	void AimedFireToDown( ioEntityGroup *pGrp,
		float fWeight,
		float fTimeRate,
		DWORD dwPreDelay,
		bool bJump,
		bool bHold );

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioExtendJump* Clone();
	virtual JumpType GetType() const;

public:
	virtual void SetJumpping( ioBaseChar *pOwner, bool bFullTime );
	virtual void ProcessJumpping( ioBaseChar *pOwner, float fHeightGap, float fNeedGaguge );
	virtual void ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap );
	virtual void ApplyJump( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void InitJump();

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

	void ChangeToAimState( ioBaseChar *pOwner, bool bAimed = false );
	virtual bool CheckUseExtendJump( ioBaseChar *pOwner );

	void ChangeToFire( ioBaseChar *pOwner );
	void ChangeToDashState( ioBaseChar *pOwner );
	void ChangeToHalfChargeState( ioBaseChar *pOwner );

	bool IsChargeComboAttack( DWORD dwWeaponIdx );

	bool IsChargeAttackState();

protected:
	void SetComboJumpAttack( ioBaseChar *pOwner );
	void LoadComboList( ioINILoader &rkLoader );

	bool IsCanComboDash( ioBaseChar *pOwner );
	void SetComboDash( ioBaseChar *pOwner );

	bool IsCanDoubleJump( ioBaseChar *pOwner, float fHeightGap );
	void SetDoubleJumpAni( ioBaseChar *pOwner );

	void SetNewJumpTypeDashAttack( ioBaseChar *pOwner, DWORD dwEndTime, DWORD dwReserveTime );

protected:
	void ProcessAim( ioBaseChar *pOwner );
	void ProcessDashState( ioBaseChar *pOwner );
	void ProcessFire( ioBaseChar* pOwner );
	void ProcessHalfCharge( ioBaseChar* pOwner );

	void ChangeToJumpAttack( ioBaseChar *pOwner );
	void ChangeNextChargeComboState( ioBaseChar *pOwner );

public:
	ioPhoenixJump(void);
	ioPhoenixJump( const ioPhoenixJump &rhs );
	virtual ~ioPhoenixJump(void);
};

inline ioPhoenixJump* ToPhoenixJump( ioExtendJump *pJump )
{
	if( !pJump || pJump->GetType() != JT_PHOENIX_JUMP )
		return NULL;

	return dynamic_cast< ioPhoenixJump* >( pJump );
}