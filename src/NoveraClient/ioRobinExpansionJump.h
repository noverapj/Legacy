#pragma once

#include "ioExtendJump.h"
#include "ioAttackAttribute.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;

class ioRobinExpansionJump : public ioExtendJump
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
		PJS_AIM,
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

	ioHashString	m_stFireUpAni;
	ioHashString	m_stFireCenterAni;
	ioHashString	m_stFireDownAni;

	float			m_fMarkerRange;

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
	virtual bool CheckUseExtendJump( ioBaseChar* pOwner );

protected:
	void SetComboJumpAttack( ioBaseChar *pOwner );
	void LoadComboList( ioINILoader &rkLoader );

	bool IsCanComboDash( ioBaseChar *pOwner );
	void SetComboDash( ioBaseChar *pOwner );

	bool IsCanDoubleJump( ioBaseChar *pOwner, float fHeightGap );
	void SetDoubleJumpAni( ioBaseChar *pOwner );

	void SetNewJumpTypeDashAttack( ioBaseChar *pOwner, DWORD dwEndTime, DWORD dwReserveTime );

	//°øÁß Â÷Áö °ø°Ý
	void SetChargeComboJumpAttack( ioBaseChar *pOwner );

protected:
	void ProcessAim( ioBaseChar *pOwner );

	void ChangeToJumpAttack( ioBaseChar *pOwner );
	void ChangeNextChargeComboState( ioBaseChar *pOwner );

public:
	ioRobinExpansionJump();
	ioRobinExpansionJump( const ioRobinExpansionJump &rhs );
	virtual ~ioRobinExpansionJump();
};

inline ioRobinExpansionJump* ToRobinExpansionJump( ioExtendJump *pJump )
{
	if( !pJump || pJump->GetType() != JT_ROBIN_EXPANSION_JUMP )
		return NULL;

	return dynamic_cast< ioRobinExpansionJump* >( pJump );
}

