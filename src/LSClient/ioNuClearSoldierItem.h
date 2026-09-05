#pragma once

#include "ioWeaponItem.h"

#include "ioSpecialStateBase.h"

class ioEntityGroup;
class ioMissileWeapon;

// 게이지 : 0 ~ 100 D~ 떼기시 공격스킬이 달라진다. 
struct GaugeInfo
{
	int	m_nMin;
	int m_nMax;

	void Init()
	{
		m_nMin = 0;
		m_nMax = 0;
	}
};
typedef std::vector< GaugeInfo > GaugeInfoList;

class ioNuClearSoldierItem : public ioWeaponItem
{
protected:
	ioHashStringVec		m_AttackReadyAniList;
	AttributeList		m_vExtendAttributeList;
	GaugeInfoList		m_vGaugeInfoList;
	AttackAttribute		m_ExtendZeroAimed;

	SecondAttributeList m_vSecondAttributeList;
	SecondAttributeList m_vSecondZeroAttributeList;

	DWORD				m_dwMaxCoolTime;
	DWORD				m_dwCurCoolTime;
	DWORD				m_dwNeedCoolTime;

	DWORD				m_dwCheckCoolTime;

	float				m_fFireTimeModifyRate;

	DWORD				m_dwAimTime;
	float				m_fAimAngle;
	float				m_fAimRange;
	float				m_fMinRange;

	float				m_fMarkerRange;

	ioHashString		m_AimUp;
	ioHashString		m_AimCenter;
	ioHashString		m_AimDown;

	ioHashString		m_FireUp;
	ioHashString		m_FireCenter;
	ioHashString		m_FireDown;

	ioHashString		m_ZeroAimUp;
	ioHashString		m_ZeroAimCenter;
	ioHashString		m_ZeroAimDown;

	ioHashString		m_ZeroFireUp;
	ioHashString		m_ZeroFireCenter;
	ioHashString		m_ZeroFireDown;

	ioHashString		m_MissFire;
	float				m_fMissFireRate;

protected:
	DWORD				m_dwChargeStartTime;

	DWORD				m_dwFireStartTime;
	DWORD				m_dwFireEndTime;
	DWORD				m_dwFireDelayTime;

	DWORD				m_dwZeroAimedCheckTime;
	DWORD				m_dwZeroAimStartTime;

	bool				m_bZeroAimState;
	bool				m_bEnableZeroAiming;
	bool				m_bEnableDownAim;

	float				m_fZeroAimSpeedRate;

	ioHashString		m_ZeroAimedEffect; // 조준점 생성

protected:
	DWORD				m_dwJumpMarkerDuration;
	DWORD				m_dwJumpMarkerEndTime;
	
protected:
	bool				m_bSetSkillAimed;
	bool				m_bSetAimedDir;


protected:
	float			m_fRange;
	float			m_fMagicCircleRadius;
	float			m_fCircleOffSet;
	float			m_fMoveSpeed;
	float			m_fHeightGap;

	ioHashString	m_GatherEffect;
	ioHashString	m_ChargeAttackDelay;
	D3DXVECTOR3		m_vCreatePos;
	ioHashString	m_SpecialAttackDelay;
	ioHashString	m_ChargeAttackBlueCircle;
	ioHashString	m_ChargeAttackRedCircle;
	UniqueObjID		m_dwCurMapEffect;
	DWORD			m_dwChargeAttackPreDelayStartTime;
	DWORD			m_dwSpecialStartTime;
	int				m_iCurSpecialWeaponCnt;
	int				m_iMaxSpecialWeaponCnt;
	Vector3Vec		m_vSpecialWeaponPosList;
	ioHashString	m_ReadyCircle;
	DWORDVec		m_dwEnableCircleIDList;
	IntVec			m_vSpecialWeaponOffset;
	DWORD			m_dwSpecialEndTime;
	DWORD			m_dwSpecialFireTime;
	DWORD			m_dwSpecialAttackAttribute;
	ioHashString	m_SpecialAttack;
	float			m_fSpecialAttackAniRate;
	ioHashString	m_GatherAnimation;
	ioHashString	m_EnableCircle;
	ioHashString	m_AreaWeaponName;
	DWORD			m_dwChargeAttackFireTime;	
	DWORD			m_dwChargeAttackEndTime;
	DWORD			m_dwPreDelayTime;
	ioHashString	m_ChargeAttackAni;
	float			m_fChargeAttackAniRate;


	ioHashString	m_stExpansionAttackMotion;
	float			m_fExpansionAttackMotionRate;
	DWORD			m_dwExpansionFireMotionEndTime;
	DWORD			m_dwNextExpansionFireTime;
	DWORD			m_dwExpansionAttackTicFirst;
	DWORD			m_dwExpansionAttackTic;
	ioHashString	m_stExpansionAttackEffect_red;
	ioHashString	m_stExpansionAttackEffect_blue;
	WeaponInfo		m_ExtraAttackInfo;
	float			m_fExpansionAttackHeightOffset;

	// 점프공격 대시 후 한번더 점프
	bool			m_bEnableJumpDash;
	float			m_fDashEndJumpPower;
	bool			m_bDashEndJumpState;

	// 점프 후
	DWORD			m_dwChargingStartTime;	
	DWORD			m_dwJumpChargeTime;
	bool			m_bJumpChargeAttack;

	// cancel dash
	AttackAttribute	m_NormalCancelDashAttribute;
	AttackAttribute	m_JumpCancelDashAttribute;
	DWORD			m_dwMotionEndTime;
	DWORD			m_dwCancelDashAttackTime;
	int				m_iChargeComboDashState;


protected:
	enum AimJumpState
	{
		AJS_NONE,
		AJS_READY,
		AJS_CANCEL,
		AJS_JUMPPING,
	};
	AimJumpState m_AimJumpState;

	float				m_fCurAngle;
	bool				m_bAimedState;
	bool				m_bAimJumpAttack;
	D3DXVECTOR3			m_vStartDir;
	DWORD				m_dwCheckAimCancelTime;
	DWORD				m_dwTrackingEndTime;
	DWORD				m_dwFireStateEndTime;

	ioHashString		m_JumpAimUp;
	ioHashString		m_JumpAimCenter;
	ioHashString		m_JumpAimDown;

	ioHashString		m_JumpFireUp;
	ioHashString		m_JumpFireCenter;
	ioHashString		m_JumpFireDown;

	ioHashString		m_AimJumpReadyAnimation;
	ioHashString		m_AimJumppingAnimation;

protected:
	enum FireState
	{
		FS_NONE,
		FS_CHARGING,
		FS_GATHERING,			
		FS_CHARGE_ATTACK_DELAY,	
		FS_CHARGE_ATTACK,		
		FS_SPECIAL_ATTACK_DELAY,
		FS_SPECIAL_ATTACK,
		FS_SPECIAL_END,	
		FS_FIREEXPANSION_ATTACK,
		FS_NORMAL_ATTACK,
		FS_SPECIAL_DASH,
		FS_AIMING,
		FS_AIM_WAIT,
		FS_AIMED,
		FS_NORMAL_FIRE,
		FS_AIMED_FIRE,
	};

	FireState			m_FireState;
	DWORD				m_dwAimStartTime;
	int					m_iCurCombo;

	D3DXVECTOR3			m_vAimedDir;

	DWORD				m_dwGatheringStartTime;
	bool				m_bSetChargingMoveAni;
	int					m_iMaxBullet;
	int					m_iCurBullet;

	ioHashStringVec		m_vMaxGatheringBuff;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

public:
	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );

public:
	virtual bool IsCanFire( const ioBaseChar* pOwner, int iFireCnt = 0, bool bNormalAttack = false ) const;

	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );
	virtual WeaponSubType GetSubType() const;

public:
	virtual void OnEquiped( ioBaseChar *pOwner );
	virtual void OnReleased( ioBaseChar *pOwner );

public:
	virtual bool CheckAimState(bool bAimMove = false);
	virtual bool SetAimState( ioBaseChar *pChar );

	virtual bool SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType );
	virtual void CheckJumpAttack( ioBaseChar *pOwner );

	virtual bool ProcessAimJump( ioBaseChar *pOwner, float fHeightGap );	// 점프 후 조준 일때 2020-02-11
	virtual void ProcessJumpping( ioBaseChar *pOwner, float fHeightGap );	// 점프공격 대시 이후 한번더 점프 2020-02-03
	virtual void ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap );	// 점프어택 2020-02-04
	virtual void ProcessLanding( ioBaseChar *pOwner );						// 2020-02-06
	virtual bool IsEnableJumpDash( ioBaseChar *pOwner );					// 2020-02-06

	virtual void SetNoneState( ioBaseChar* pOwner, int eNewState = 0 );

	virtual bool IsChargeAttackState( ioBaseChar *pOwner );
	virtual bool IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck );

	virtual void OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem=false );

	virtual float GetAttackSpeedRate() const;

	virtual bool ProcessCancelBySpeicalDash( ioBaseChar *pOwner );

public:
	void ModifyFireTimeRate( float fRate );
	void SetFireStartTime( DWORD dwFireTime );

	virtual int GetCurBullet() { return m_iCurBullet; }
	virtual int GetMaxBullet() { return m_iMaxBullet; }

	bool IsEnableGauge(); 

public:	// Packet
	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket );
	
	void ClearSpecialState( ioBaseChar* pOwner );
	virtual	bool	IsConditionalSpecialDashReserve( ioBaseChar *pOwner );

protected:
	void OnCharging( ioBaseChar *pOwner );

	void ChangeToNormalAttack( ioBaseChar *pOwner );	// 기본 공격 

	void SetNewJumpTypeDashAttack( ioBaseChar *pOwner, DWORD dwEndTime, DWORD dwReserveTime );	// 점프후 대시 공격 


	void OnGathering( ioBaseChar *pOwner );	
	void ChangeToChargeAttack( ioBaseChar *pOwner );
	void ChangeToSpecialDelayState( ioBaseChar *pOwner );
	void AddSpecialAttackPosition( ioBaseChar *pChar, ioPlayStage *pStage );
	void ChangeToSpecialAttackState( ioBaseChar *pOwner );
	void ChangeToSpecialEndState( ioBaseChar *pOwner );
	void CreateWeapon( ioBaseChar *pChar );
	void ChangeToGathering( ioBaseChar *pOwner );
	void CheckDecreaseGauge();


	void OnAiming( ioBaseChar *pOwner );
	void OnAimWait( ioBaseChar *pOwner );
	void OnAimed( ioBaseChar *pOwner );
	void OnFire( ioBaseChar *pOwner );
	void OnJumpFire( ioBaseChar *pOwner );

protected:
	void ChangeToAiming( ioBaseChar *pOwner );
	void ChangeToNormalFire( ioBaseChar *pOwner = NULL, D3DXVECTOR3 vPos = D3DXVECTOR3( 0.f, 0.f, 0.f) );
	void ChangeToAimWaitState( ioBaseChar *pOwner, const ioHashString &szTarget );
	void ChangeToAimedState( ioBaseChar *pOwner );
	void ChangeToAimedFire( ioBaseChar *pOwner );

	void FireExpansionAttack( ioBaseChar *pOwner, D3DXVECTOR3 vPos );


	virtual	void SetChangeConditionalSpecialDash( ioBaseChar *pOwner, ioUserKeyInput::DirKeyInput eCurrDir, int eCallerType );
	void ProcessSpecialDash( ioBaseChar* pOwner );
	void ChangeReserveKeyInputState( ioBaseChar *pOwner, int iCurCombo = 0 );

protected:
	bool CheckCurrentTargetValid( ioBaseChar *pOwner, bool bAngleAlsoCheck );
	void TrackingTarget( ioBaseChar *pOwner );

	float GetAimedUpDownWeight( const D3DXVECTOR3 &vAimDir, float fMaxAngle );

	bool CheckZeroAim( ioBaseChar *pOwner );
	void SetZeroAim( ioBaseChar *pOwner, bool bSet );
	
	void AimedFireToUp( ioEntityGroup *pGrp,
						float fWeight,
						float fTimeRate,
						DWORD dwPreDelay,
						bool bHold );

	void AimedFireToDown( ioEntityGroup *pGrp,
						  float fWeight,
						  float fTimeRate,
						  DWORD dwPreDelay,
						  bool bHold );

public:
	void SetAimedSkillUse( bool bSkillUsed = false );
	void SetAimedDirSet( bool bAimedDirSet = false );
	D3DXVECTOR3 ReCalcMoveDir( ioBaseChar *pOwner, ioWeapon *pWeapon );

	bool IsAimedState();

	inline bool GetAimedSkillUse() const { return m_bSetSkillAimed; }
	inline bool GetAimedDirSet() const { return m_bSetAimedDir; }
	inline const D3DXVECTOR3& GetAimedDir() const { return m_vAimedDir; }

public:
	virtual const ioHashString& GetJumppingAnimation() const;
	virtual void CheckAimJumpState( ioBaseChar *pChar );
	virtual bool CheckLanding( ioBaseChar *pChar );

protected:
	bool SetAimJumpState( ioBaseChar *pChar );

	void SetAimJumpReady( ioBaseChar *pChar );
	void SetAimJumpCancel( ioBaseChar *pChar );
	void SetAimJumpping( ioBaseChar *pChar, bool bFullTime=false );

	void UpdateAimDir( ioBaseChar *pOwner );

	bool CompareAimedTarget( ioBaseChar *pOwner );

public:
	void InitCoolTime();
	void MaxCoolTime();
	void DecreaseCoolTime();

public:	// 2020-01-29
	float GetCircleRadius();
	void ClearAttackState( ioBaseChar *pOwner );
	void ProcessSpecialState( ioBaseChar *pOwner );
	void CreateArea( ioBaseChar *pChar );
	void SetChargeAttackState( ioBaseChar *pChar );

public:
	ioNuClearSoldierItem();
	ioNuClearSoldierItem( const ioNuClearSoldierItem &rhs );
	virtual ~ioNuClearSoldierItem();
};

inline ioNuClearSoldierItem* ToNuClearSoldierItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_NUCLEARSOLDIER_ITEM )
		return NULL;

	return dynamic_cast< ioNuClearSoldierItem* >( pWeapon );
}
//////////////////////////////////////////////////////////////////////////	
class ioNuClearSoldierSpecialState : public ioSpecialStateBase
{
public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_NUCLEARSOLDIER_SPECIAL"; }
	virtual const CharState GetState(){ return CS_NUCLEARSOLDIER_SPECIAL; }	
	virtual void CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight );

public:
	virtual void CheckSpecialState( ioBaseChar* pOwner );

public:
	ioNuClearSoldierSpecialState();
	virtual ~ioNuClearSoldierSpecialState();
};

inline ioNuClearSoldierSpecialState* ToNuClearSoldierSpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_NUCLEARSOLDIER_SPECIAL )
		return NULL;

	return dynamic_cast< ioNuClearSoldierSpecialState* >( pState );
}
