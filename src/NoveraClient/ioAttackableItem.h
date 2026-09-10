#pragma once

#include "ioItem.h"
#include "ioFormalAttackGrp.h"
#include "ioStatePriority.h"

class ioINILoader;
class ioBaseChar;
class ioWeapon;
class SP2Packet;
class ioExtendDash;
class ioExtendJump;
class ioCounterAttack;

typedef struct tagJumpReloadInfo
{
	ioHashString m_szReloadAni;
	float m_fReloadAniRate;
	int m_iReloadCnt;

	tagJumpReloadInfo()
	{
		Init();
	}

	void Init()
	{
		m_szReloadAni.Clear();
		m_fReloadAniRate = FLOAT1;
		m_iReloadCnt = 0;
	}

} JumpReloadInfo;

typedef std::vector<JumpReloadInfo> JumpReloadInfoList;

typedef struct tagSummonDummyChar
{
	int m_iNeedGauge;
	int m_iCurCombo;
	vDummyCharLoadInfoList m_DummyCharList;

	tagSummonDummyChar()
	{
		Init();
	}

	void Init()
	{
		m_DummyCharList.clear();
		m_iNeedGauge = 0;
		m_iCurCombo = 0;
	}

	void InitCombo()
	{
		m_iCurCombo = 0;
	}
} SummonDummyChar;

typedef std::map< int, SummonDummyChar > SummonDummyCharMap;

typedef struct tagMagicElementInfo
{
	AttackAttribute m_FireAttack;
	AttackAttribute m_IceAttack;
	AttackAttribute m_LightAttack;

	tagMagicElementInfo()
	{
		Init();
	}

	void Init()
	{
		m_FireAttack.Init();
		m_IceAttack.Init();
		m_LightAttack.Init();
	}
} MagicElementInfo;

typedef std::map< int, AttackAttribute > ElementAttackMap;

class ioAttackableItem : public ioItem
{
public:
	enum SummonDummyType
	{
		SDT_DELAY,
		SDT_RUN,
		SDT_DASH,
		SDT_DASH_ATT,
		SDT_JUMP,
		SDT_JUMP_ATT,
		SDT_JUMP_LAND,
		SDT_NEW_JUMP_ATT,
		SDT_NORMAL_ATT,
		SDT_EXTEND_ATT,
		SDT_COUNTER,
		SDT_MAX,
	};

	enum MagicElementType
	{
		MET_NONE,
		MET_FIRE,
		MET_ICE,
		MET_LIGHT,
	};

	enum ExtendChargeType
	{
		ECT_NONE		= 0,
		ECT_WEREWOLF	= 1,
		ECT_RIDER		= 2,
		ECT_GRIFFIN		= 3,
		ECT_BUBBLE,
		ECT_CHANGE_ON,
		ECT_DR_SPIDER,
	};

	enum ChangeConditionSpecialDash
	{
		CCSD_NONE,
		CCSD_NORMAL,
		CCSD_EXTEND,
		CCSD_DASH_ATTACK,
		CCSD_JUMP,
		CCSD_COUNTER_ATTACK,
	};

protected:
	ExtendChargeType m_ExtendChargeType;

protected:
	ioFormalAttackGrp m_FormalAttack;
	ioStatePriority m_Priority;

	ioHashString m_DelayAnimation;
	ioHashString m_RunAnimation;
	ioHashString m_DashRunAnimation;

	ioHashString m_JumpReadyAnimation;
	ioHashString m_JumppingAnimation;
	ioHashString m_JumpEndAnimation;
	ioHashString m_ExtendJumppingAnimation;
	ioHashString m_ExtendJumpEndAnimation;

	ioHashString m_DefenseWeakAttackPushBuff;
	
	CEncrypt<float> m_fJumpSpeedRate;
	CEncrypt<float> m_fBackJumpSpeedRate;
	CEncrypt<float> m_fJumpPowerRate;
	CEncrypt<float> m_fBlowGetupTimeRate;
	CEncrypt<float> m_fDefenseDamageRate;
	CEncrypt<float> m_fWeakAttackPushRate;
	CEncrypt<float> m_fDashSpeedRate;
	CEncrypt<float> m_fDashAttackSpeedRate;
	CEncrypt<float> m_fDashEndSpeedRate;
	CEncrypt<float> m_fDefenseSpeedRate;
	CEncrypt<float> m_fAttackSpeedRate;
	float m_fJumpNeedGauge;
	float m_fJumpExtraGauge;
	
	CEncrypt<float> m_fExtendLandingRate;
	CEncrypt<float> m_fJumpLandingRate;
	CEncrypt<float> m_fJumpAttackEnableHeight;
	CEncrypt<DWORD> m_dwJumpAttackEnableTime;

	CEncrypt<bool> m_bNewJumpAttackState;
	CEncrypt<float> m_fNewJumpAttackGravity;

	float m_fLandAttackEnableGravity;

	ioHashString m_DefenseAnimation;
	ioHashString m_DefenseWeakWoundedAni;
	ioHashString m_DefenseStrongWoundedAni;
	ioHashString m_DefenseSkillWoundedAni;
	ioHashString m_DefenseEffect;
	bool m_bEnableExtendDefenseEffect;

	DWORD	m_dwDefensePreDuration;
	DWORD	m_dwDefenseEndDuration;
	DWORD	m_dwDefenseDuration;
	DWORD	m_dwDefenseDurationEx;
	DWORD	m_dwExtendDefenseCheckTime;
	DWORD	m_dwExtendDefenseWoundedTime;
	CEncrypt<float>	m_fDefenseWeakWoundedRate;
	CEncrypt<float>	m_fDefenseStrongWoundedRate;
	CEncrypt<float>	m_fDefenseSkillWoundedRate;

	DashType m_DashType;
	JumpType m_JumpType;

	ioExtendDash *m_pExtendDash;
	ioExtendJump *m_pExtendJump;
	ioCounterAttack *m_pCounterAttack;

	CEncrypt<bool> m_bExtraDashAttack;
	CEncrypt<bool> m_bExtraOtherDashAttack;
	CEncrypt<bool> m_bJumpLandEndDash;

	DashAttEndType m_DashAttackEndDash;

	bool m_bEquipedOnHand;
	bool m_bJumpAttack;
	bool m_bJump_D_Attack;
	bool m_bRotateJumpAttack;
	
	bool m_bGaugeUpdateChargeAttack;
	bool m_bGaugeUpdateExtendDefense;

	bool m_bDefenseBackSide;
	bool m_bDisableWoundDefense;

	DWORD m_dwDodgeDefenseCheckTime;
	DWORD m_dwDefenseAttackCheckTime;

	CEncrypt<float> m_fJumpDefensePushRate;

	CEncrypt<bool> m_bExtendDefenseDash;
	CEncrypt<bool> m_bExtendDefenseJump;
	CEncrypt<bool> m_bEnableJumpReload;

protected:
	CEncrypt<bool> m_bChargeCharRotate;
	CEncrypt<bool> m_bChargeCharDash;
	CEncrypt<bool> m_bChargeCharDefense;
	CEncrypt<bool> m_bChargeCharJump;
	
	CEncrypt<bool> m_bDefenseMove;
	CEncrypt<bool> m_bDefenseRoate;

	CEncrypt<bool> m_bChargeAutoTarget;

protected:
	JumpReloadInfoList m_JumpReloadInfoList;
	int m_iCurJumpReloadCnt;
	bool m_bReserveJumpReload;

	DWORD m_dwJRComboEnable_S;
	DWORD m_dwJRComboEnable_E;

protected:
	AttackAttribute m_DefenseAttack;
	DWORD m_dwDefenseAttackEnd;

	DWORD m_dwKeyReserveTime;

	int m_iSkillInput;
	bool m_bEnableDefenseState;
	bool m_bEnableAttackState;
	bool m_bEnableJumpState;
	bool m_bEnableDashState;
	ioUserKeyInput::DirKeyInput m_ConditionalSpecialDashKey;

protected:	// input_jump_s
	DWORD m_dwCancelCheckTime;
	DWORD m_dwInputCancelCheckTime;

	ioHashString m_szCurAni;
	float m_fCurAniRate;

protected:
	vAniRotateInfoList m_vAniRotateInfoList;

	int m_iCurAniRotate;
	DWORD m_dwAniRotateTime;
	float m_fAniRotateAngle;

// air_jump
protected:
	float m_fAirJumpPower;
	float m_fAirForcePower;
	float m_fAirJumpPowerRate;

	std::list<float> m_vAirJumpTimeList;
	int m_iCurAirJump;

protected:
	CEncrypt<DWORD> m_dwMaxChargeTime;

	CEncrypt<float> m_fCurChargeRate;
	CEncrypt<float> m_fCurChargeRateForSkill;

	CEncrypt<float> m_fMaxDamageRate;
	CEncrypt<float> m_fMaxForceRate;
	CEncrypt<float> m_fMaxBlowRate;

	CEncrypt<float> m_fMaxRangeRate;
	CEncrypt<float> m_fMaxSpeedRate;
	CEncrypt<float> m_fMaxChargeAniRate;
	CEncrypt<float> m_fMaxForceSlidingRate;

	CEncrypt<float> m_fMaxTargetRangeRate;
	CEncrypt<float> m_fMaxTargetAngleRate;

	CEncrypt<float> m_fMaxFloatRate;

	CEncrypt<float> m_fMaxScaleRate;
	CEncrypt<float> m_fMaxExplosionRate;
	CEncrypt<float> m_fMaxLiveTimeRate;
	CEncrypt<float> m_fMaxWoundedTimeRate;

// auto target
protected:
	float m_fAutoTargetMinAngle;
	float m_fAutoTargetMaxAngle;
	float m_fAutoTargetRange;

	float m_fDashAutoTargetMinAngle;
	float m_fDashAutoTargetMaxAngle;
	float m_fDashAutoTargetRange;

	float m_fJumpAutoTargetMinAngle;
	float m_fJumpAutoTargetMaxAngle;
	float m_fJumpAutoTargetRange;

	float m_fCountAutoTargetMinAngle;
	float m_fCountAutoTargetMaxAngle;
	float m_fCountAutoTargetRange;


// WereWolf
protected:
	CEncrypt<bool> m_bEnableWereWolf;
	CEncrypt<bool> m_bUsedWereWolf;

	AttackAttribute m_ChangeWereWolfAttack;
	AttackAttribute m_ReleaseWereWolfAttack;

	ioHashStringVec m_vWereWolfMeshList;
	ioHashStringVec m_vWereWolfMeshListW;
	ioHashStringVec m_vWereWolfBuffList;
	ioHashStringVec m_vReleaseWereWolfBuffList;

	CEncrypt<DWORD> m_dwWereWolfObjectItem;
	CEncrypt<DWORD> m_dwChangeWereWolfTime;

//counter attack buff
protected:
	ioHashStringVec	m_vCounterAttackBuff;

// summon dummychar
protected:
	CEncrypt<bool> m_bEnableSummonDummyChar;
	CEncrypt<DWORD> m_dwSummonDummyCharGapTime;
	SummonDummyCharMap m_SummonDummyCharMap;

// extra gauge
protected:
	CEncrypt<float> m_fExtraGaugeAttack;
	CEncrypt<float> m_fExtraGaugeAttackDef;
	CEncrypt<float> m_fExtraGaugeWound;
	CEncrypt<float> m_fExtraGaugeWoundDef;

	CEncrypt<float> m_fExtraGaugeAttackDec;
	CEncrypt<float> m_fExtraGaugeAttackDefDec;
	CEncrypt<float> m_fExtraGaugeWoundDec;
	CEncrypt<float> m_fExtraGaugeWoundDefDec;

	CEncrypt<float> m_fExtraGaugeBaseInc;
	CEncrypt<float> m_fExtraGaugeBaseDec;

	CEncrypt<float> m_fMaxExtraGauge;
	CEncrypt<float> m_fCurExtraGauge;

// defence counter attack
	DWORD m_dwDefenceDelayEndTime;
	DWORD m_dwDefenceAttackEndTime;
	ioHashString	m_stDefenceCounterDelayAni;
	CEncrypt<float> m_fDefenceCounterDelayAniRate;

	AttackAttribute m_DefenceCounterAttack;

protected:
	bool m_bEnableJumpLandingExtraState;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );

	void LoadSummonDummyChar( ioINILoader &rkLoader );
	void LoadSummonDummyCharInfo( ioINILoader &rkLoader, int iSubState );

	virtual void OnEquiped( ioBaseChar *pOwner );
	virtual void OnReleased( ioBaseChar *pOwner );

	virtual bool IsCanFire( const ioBaseChar* pOwner, int iFireCnt = 0, bool bNormalAttack = false ) const;

public:
	virtual void CallItemProcess( ioBaseChar* pOwner );

public:
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );
	virtual void CheckChargeInfo( ioWeapon *pWeapon, bool bJumpAttack );

	virtual bool SetExtendDefenseState( ioBaseChar *pOwner );
	virtual void CheckExtendDefenseState( ioBaseChar *pOwner );
	virtual void CheckDefenseMoveAniState( ioBaseChar *pOwner );

	virtual bool SetDodgeDefenseState( ioBaseChar *pOwner );
	virtual void CheckDodgeDefenseState( ioBaseChar *pOwner );
	virtual void ApplyDodgeDefenseState( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual bool SetDefenseAttackState( ioBaseChar *pOwner );
	virtual void CheckDefenseAttackState( ioBaseChar *pOwner );
	virtual void ApplyDefenseAttackState( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual bool SetExtendDashState( ioBaseChar *pOwner );
	virtual void CheckExtendDashState( ioBaseChar *pOwner );
		
	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );

	virtual bool SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType );
	virtual void CheckJumpAttack( ioBaseChar *pOwner );

	virtual bool SetJumpReloadState( ioBaseChar *pOwner );
	virtual bool SetJumpReload( ioBaseChar *pOwner );
	virtual void CheckJumpReloadState( ioBaseChar *pOwner );
	virtual void CheckJumpReloadReserve( ioBaseChar *pOwner );
	virtual void ProcessJumpReloadState( ioBaseChar *pOwner );
	virtual void ApplyJumpReload( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket );
	
	virtual void OnEndExtendDefense( ioBaseChar *pOwner );
	virtual void OnEndNormalAttack( ioBaseChar *pOwner );
	virtual void OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem=false );
	virtual void OnEndExtendJump( ioBaseChar *pOwner );

	void FillItemGaugePacketHeader( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual void FillNormalAttackExtraInfo( SP2Packet &rkPacket );
	virtual void ApplyNormalAttackExtraInfo( SP2Packet &rkPacket );

	virtual bool CheckOnHand() { return false; }
	virtual bool CheckAimState( bool bAimMove = false ) { return false; }
	virtual bool SetAimState( ioBaseChar *pChar ) { return false; }
	virtual bool CheckLanding( ioBaseChar *pChar );
	virtual void SetNoneState( ioBaseChar* pOwner, int eNewState = 0 );
	virtual bool CheckExtendDefenseEnable( ioBaseChar *pChar ) { return true; }
	
	virtual bool IsChargeAttackState( ioBaseChar *pOwner ) { return false; }
	virtual bool IsMoveAttackState( ioBaseChar *pOwner ) { return false; }
	virtual bool IsCanUpdateGauge( ioBaseChar *pOwner );
	virtual bool IsNoGhostWalk( ioBaseChar *pOwner ) { return false; }

	virtual bool IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck ) { return false; }
	virtual bool IsEnableItemDashCheck( ioBaseChar *pOwner, bool bCheck ) { return false; }
	virtual bool IsEnableAttackRotateCheck( ioBaseChar *pOwner ) { return false; }
	virtual bool IsEnableChargeDefenseCheck() { return false; }
	virtual bool IsEnableWoundedDashCheck( ioBaseChar *pOwner, bool bCheck ) { return false; }

	virtual void CheckAimJumpState( ioBaseChar *pChar );

	virtual const ioHashString& GetJumppingAnimation() const;
	virtual const ioHashString& GetExtendJumppingAnimation() const;

	virtual void CheckRetreatState( ioBaseChar *pOwner );

	virtual void SetNextItemState( ioBaseChar *pWoundChar, ioBaseChar *pAttacker, ioWeapon *pWeapon, bool bDefense, float fDamage );
	virtual void WeaponWounded( ioWeapon *pWeapon, ioBaseChar *pWoundChar );

	//돈키호테때 추가
	//웨폰이 맵에 충돌후 죽을때 알려준다.
	virtual void WeaponMapColDead( ioWeapon *pWeapon );

	//이소령때 추가
	virtual void SetDefenceCounterState( ioBaseChar *pOwner );
	virtual bool IsUseDefenceCounterAttack();
	virtual void ProcessDefenceCounterAttack( ioBaseChar *pOwner );
	virtual bool IsUseDefenceCounterAttackEndDash() { return false; }

	// 오멘때 추가
	virtual void StartCounterAttackBuff( ioBaseChar *pOwner );

	//잭때 추가
	virtual void ProcessExtraJump( ioBaseChar *pOwner );

	//레슬러 진화때 추가
	virtual void SetEndGrappling( ioBaseChar *pOwner );

protected:
	virtual void ClearKeyReserve();
	virtual void CheckReserve( ioBaseChar *pOwner );
	virtual void ProcessReserveKeyInput( ioBaseChar *pOwner );

	// For Extend Jump
public:
	virtual void SetJumpState();
	virtual void SetJumpping( ioBaseChar *pOwner, bool bFullTime );
	virtual void ProcessJumpping( ioBaseChar *pOwner, float fHeightGap );
	virtual void ProcessJumppingByNoNeedProcess( ioBaseChar *pOwner, float fHeightGap );
	virtual void ProcessLanding( ioBaseChar *pOwner );
	virtual void ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap );
	virtual bool ProcessAimJump( ioBaseChar *pOwner, float fHeightGap );
	virtual void ApplyJump( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void ApplyDash( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void ApplyCounterAttack( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual void CheckComboJumpAttack( ioBaseChar *pOwner );
	virtual bool SetFlashJumpAttack( ioBaseChar *pOwner );
	virtual bool SetComboFlashAttack( ioBaseChar *pOwner );
	virtual bool SetCollectJumpAttack( ioBaseChar *pOwner );
	virtual bool SetPowerChargeJumpAttack( ioBaseChar *pOwner );

	virtual float GetAttackSpeedRate() const;
	virtual float GetExtraJumpSpeedRate( const D3DXVECTOR3 &vDir, const D3DXQUATERNION &qtRot ) const;

	bool IsEnableNewJumpAttack();

	inline float GetJumpBackSpeedRate() const { return m_fBackJumpSpeedRate; }

	ioHashString GetJumpAniDefenseWounded();
	ioHashString GetJumpAniDefenseCriticalWounded();
	
	inline float GetJumpDefensePushRate() const { return m_fJumpDefensePushRate; }

	bool CheckUseExtendJump( ioBaseChar* pOwner );
	bool CheckUseBuffAirDashFromJump() const;
	virtual void SetLandingState( ioBaseChar *pOwner );

	bool CheckEnableLandAttackByGravity( ioBaseChar *pChar ) const;
	inline float GetNewJumpAttackGravity() const { return m_fNewJumpAttackGravity; }

	virtual bool IsEnableExtendJump();
	virtual void RecevieExtendJumpUse( ioBaseChar *pOwner );

public:
	bool HasExtendDash() const;
	bool HasExtendJump() const;
	bool HasCounterAttack() const;

	ioExtendDash* GetCurExtendDash();
	ioExtendJump* GetCurExtendJump();
	ioCounterAttack* GetCurCounterAttack();

	const ioExtendJump* GetCurExtendJumpConst() const { return m_pExtendJump; }
	const ioExtendDash* GetCurExtendDashConst() const { return m_pExtendDash; }
	const ioCounterAttack* GetCurCounterAttackConst() const { return m_pCounterAttack; }

	virtual bool HasAniFile( AttackType eType, int iSubIdx = 0 ) const;
	virtual bool HasEndAniFile( AttackType eType, int iSubIdx = 0 ) const;

	virtual const AttackAttribute* GetAttackAttribute( AttackType eType, int iSubIdx = 0 ) const;
	virtual const ioHashString& GetAniFile( AttackType eType, int iSubIdx = 0 ) const;
	virtual float GetAttackAniRate( AttackType eType, int iSubIdx = 0 ) const;
	virtual const ioHashString& GetEndAniFile( AttackType eType, int iSubIdx = 0 ) const;
	virtual float GetAttackEndAniRate( AttackType eType, int iSubIdx = 0 ) const;

	virtual const WeaponInfoList& GetAttackAttributeIdx( AttackType eType, int iSubIdx = 0 ) const;
	virtual const ForceInfoList& GetForceInfoList( AttackType eType, int iSubIdx = 0 ) const;

	virtual DWORD GetAttackPreDelay( AttackType eType, int iSubIdx = 0 ) const;
	virtual DWORD GetAttackAnimationDelay( AttackType eType, int iSubIdx = 0 ) const;

	float GetWeakAttackPushRate() const;

public:
	int GetPriority( StatePriorityType eType );
	void SetPriority( StatePriorityType eType , int iValue );
	virtual int GetMaxCombo() const;
	virtual int GetExtraDashAttackMax() const;
	int GetExtraOtherDashAttackMax() const;

	virtual DWORD GetJumpChargeTime( ioBaseChar *pChar ) const;

	inline bool IsEnableExtendDefenseEffect() const { return m_bEnableExtendDefenseEffect; }
	inline ioHashString GetDefenseEffect() const { return m_DefenseEffect; }
	virtual ioHashString GetDefenseAnimation() const { return m_DefenseAnimation; }
	inline ioHashString GetDefenseWeakWoundedAni() const { return m_DefenseWeakWoundedAni; }
	inline ioHashString GetDefenseStrongWoundedAni() const { return m_DefenseStrongWoundedAni; }
	inline ioHashString GetDefenseSkillWoundedAni() const { return m_DefenseSkillWoundedAni; }
	inline ioHashString GetDefenseWeakAttackPushBuff() const { return m_DefenseWeakAttackPushBuff; }
	inline bool IsCanDefenseBackSide() const { return m_bDefenseBackSide; }
	inline bool IsDisableWoundDefense() const { return m_bDisableWoundDefense; }

	inline void SetExtendDefenseWoundedTime( DWORD dwTime ) { m_dwExtendDefenseWoundedTime = dwTime; }

	inline DWORD GetDefensePreDuration() const { return m_dwDefensePreDuration; }
	inline DWORD GetDefenseEndDuration() const { return m_dwDefenseEndDuration; }
	inline DWORD GetDefenseDuration() const { return m_dwDefenseDuration; }
	inline DWORD GetDefenseDurationEx() const { return m_dwDefenseDurationEx; }
	inline float GetDefenseWeakWoundedRate() const { return m_fDefenseWeakWoundedRate; }
	inline float GetDefenseStrongWoundedRate() const { return m_fDefenseStrongWoundedRate; }
	inline float GetDefenseSkillWoundedRate() const { return m_fDefenseSkillWoundedRate; }

	inline float GetDefenseDamageRate() const { return m_fDefenseDamageRate; }
	inline float GetDefenseSpeedRate() const { return m_fDefenseSpeedRate; }

	float GetDashSpeedRate();
	float GetDashAttackSpeedRate();

	inline float GetJumpPowerRate() const { return m_fJumpPowerRate; }
	inline DWORD GetExtendDefenseCheckTime() const { return m_dwExtendDefenseCheckTime; }

	inline DWORD GetDodgeDefenseCheckTime() const { return m_dwDodgeDefenseCheckTime; }
	inline DWORD GetDefenseAttackCheckTime() const { return m_dwDefenseAttackCheckTime; }

	inline float GetJumpAttackEnableHeight() const { return m_fJumpAttackEnableHeight; }
	inline DWORD GetJumpAttackEnableTime() const { return m_dwJumpAttackEnableTime; }

	inline bool IsEnableExtendDefDash() const { return m_bExtendDefenseDash; }
	inline bool IsEnableExtendDefJump() const { return m_bExtendDefenseJump; }

public:
	inline bool HasDelayAnimation() const { return !m_DelayAnimation.IsEmpty(); }
	inline bool HasRunAnimation() const { return !m_RunAnimation.IsEmpty(); }
	inline bool HasDashRunAnimation() const { return !m_DashRunAnimation.IsEmpty(); }
	inline bool HasJumpReadyAnimation() const { return !m_JumpReadyAnimation.IsEmpty(); }
	inline bool HasJumppingAnimation() const { return !m_JumppingAnimation.IsEmpty(); }
	inline bool HasExtendJumppingAnimation() const { return !m_ExtendJumppingAnimation.IsEmpty(); }

	virtual const ioHashString& GetDelayAnimation() const { return m_DelayAnimation; }
	virtual const ioHashString& GetRunAnimation() const { return m_RunAnimation; }
	inline const ioHashString& GetDashRunAnimation() const { return m_DashRunAnimation; }
	inline const ioHashString& GetJumpReadyAnimation() const { return m_JumpReadyAnimation; }

	virtual bool HasJumpEndAnimation( bool bUseAttack, ioBaseChar *pOwner ) const;
	virtual const ioHashString& GetJumpEndAnimation( bool bUseAttack, ioBaseChar *pOwner ) const;
	float GetExtendLandingRate( bool bUseAttack, ioBaseChar *pOwner ) const;

	bool IsCanJumpingSkill( ioBaseChar *pOwner );

	inline DashType GetDashType() const { return m_DashType; }
	inline JumpType GetJumpType() const { return m_JumpType; }

	CounterAttackType GetCounterAttackType() const;

	bool IsCanJumpAttack( ioBaseChar *pOwner );
	bool IsCanJump_D_Attack( ioBaseChar *pOwner );
	bool IsCanJumpReload( ioBaseChar *pOwner );
	inline bool IsCanRotateJumpAttack() const { return m_bRotateJumpAttack; }

	bool IsEnableExtraDashAttack();
	bool IsEnableExtraOtherDashAttack();

	bool IsEnableDashAttackEndDash();
	bool IsEnableJumpLandEndDash();

	const DashAttEndType& GetDashAttEndType();

protected:
	void CheckJumpType( char* szJumpType );
	void CheckDashType( char* szDashType );

protected:	// input_jump_s
	virtual void ClearCancelInfo();
	virtual void CheckCancelReserve( ioBaseChar *pOwner );
	virtual void SetCurAttackAniForInputJump( const ioHashString &szAni, float fAniRate );
	virtual bool ProcessCancel( ioBaseChar *pOwner );
	virtual bool ProcessCancelBySpeicalDash( ioBaseChar *pOwner );

public:
	virtual void SetCurAttackAniForInputDashS( const ioHashString &szAni, float fAniRate );
	virtual void CheckCancelReserveByConditionalSpecialDash( ioBaseChar *pOwner );
	virtual bool IsConditionalSpecialDashReserve( ioBaseChar *pOwner );
	virtual void SetChangeConditionalSpecialDash( ioBaseChar *pOwner, ioUserKeyInput::DirKeyInput eCurrDir, int eCallerType );

public:
	void InitAniRotate();
	void SetAniRotate( ioBaseChar *pChar, int iAniID, float fAniRate );
	void CheckAniRotate( ioBaseChar *pChar );

// ChangeState (WereWolf)
public:
	virtual bool CheckChangeWereWolfState( ioBaseChar *pChar );
	virtual void SetChangeWereWolfState( ioBaseChar *pChar );

	virtual bool CheckReleaseWereWolfState( ioBaseChar *pChar );
	virtual void ReleseWereWolfState( ioBaseChar *pChar );
	virtual void ReleseWereWolfStateByOwner( ioBaseChar *pChar );

	virtual void IncreaseChangeDamage( float fDamage, int iType, bool bBuff );
	virtual void IncreaseChangeDamageByWeapon( float fDamage );
	virtual void CheckIncreaseChangeDamage( ioBaseChar *pOwner );
	virtual void DecreaseChangeDamage( ioBaseChar *pOwner );

	bool IsWereWolfState();

	virtual bool IsEnableActionStopDelay();

public:
	virtual void SetExtraGaugeTypeByWeapon( ioBaseChar *pOwner, int iType, int iWGT, float fDamage );
	virtual void SetUseExtraGauge( float fAmt );

	virtual void InitExtraGauge();
	virtual void MaxExtraGauge();

	virtual float GetMaxExtraGauge();
	virtual float GetCurExtraGauge();
	virtual void SetCurExtraGauge( float fGauge );

// summon dummychar
public:
	inline bool IsEnableSummonDummyChar() const { return m_bEnableSummonDummyChar; }
	inline DWORD GetSummonDummyCharGapTime() const { return m_dwSummonDummyCharGapTime; }

	SummonDummyChar* GetSummonDummyCharInfo( int iSubState );

public:
	inline bool IsChargeRotate() const { return m_bChargeCharRotate; }
	inline bool IsChargeDash() const { return m_bChargeCharDash; }
	inline bool IsChargeDefense() const { return m_bChargeCharDefense; }

	void SetChargeRotate(bool b);

public:
	virtual void GetAutoTargetValue( ioBaseChar *pOwner,
									 float &fRange, float &fMinAngle, float &fMaxAngle,
									 AutoTargetType eType );

// air_jump
public:
	virtual void ClearAirJump();
	virtual void SetAirJump( ioBaseChar *pOwner, int iAniID, float fTimeRate );
	virtual void CheckAirJump( ioBaseChar *pOwner );

public:
	virtual bool IsAutoTarget() const { return true; }

public:
	virtual void EquipObjectItemByAttakable( ioBaseChar* pOwner, ioItem* pObjectItem );
	virtual void ReleaseObjectItemByAttack( ioBaseChar* pOwner, ioItem* pObjectItem );

public:
	virtual bool IsBuffFlyChange( ioBaseChar* pOwner );

public:
	const virtual char* GetAttackStateText() const;

public:
	inline bool IsEnableJumpLandingExtraState() const { return m_bEnableJumpLandingExtraState; }
	virtual bool SetJumpLandingExtraState( ioBaseChar *pOwner );

public:
	virtual bool IsWoundAfterUseExtendState(){ return false; }
	virtual bool CheckWoundAfterUseExtendState( ioBaseChar* pOwner, BlowTargetType eType ){ return false; }

	//화학병때 추가
	virtual bool IsCharSkipState( const ioBaseChar* pOwner ){ return false; }

public:
	ioAttackableItem();
	ioAttackableItem( const ioAttackableItem &rhs );
	virtual ~ioAttackableItem();
};

inline ioAttackableItem* ToAttackableItem( ioItem *pItem )
{
	if( !pItem )	return NULL;
	if( pItem->GetType() != ioItem::IT_WEAPON && pItem->GetType() != ioItem::IT_ARMOR )
		return NULL;

	return dynamic_cast< ioAttackableItem* >( pItem );
}

inline const ioAttackableItem* ToAttackableItemConst( const ioItem *pItem )
{
	if( !pItem )	return NULL;
	if( pItem->GetType() != ioItem::IT_WEAPON && pItem->GetType() != ioItem::IT_ARMOR )
		return NULL;

	return dynamic_cast< const ioAttackableItem* >( pItem );
}

