#pragma once

#include "ioPlayEntity.h"
#include "ioWorldEventReceiver.h"
#include "ioAttackAttribute.h"
#include "ioUserKeyInput.h"
#include "DummyCharEnumType.h"

class ioPlayStage;
class ioPlayMode;
class ioGaugeHelper;
class ioINILoader;
class ioWeapon;
class SP2Packet;
class ioBaseChar;
class ioAniCollisionBoxGrp;

struct TargetDistInfo
{
	ioHashString	m_TargetName;
	float			m_fLenth;

	TargetDistInfo()
	{
		m_TargetName.Clear();
		m_fLenth = 0.0f;
	}
};
typedef std::vector<TargetDistInfo> TargetDistInfoList;

typedef struct tagDummyAttackInfo
{
	int m_iSubUseType;
	AttackAttribute m_AttackAttribute;

	tagDummyAttackInfo()
	{
		m_iSubUseType = DSUT_NONE;
	}
} DummyAttackInfo;

typedef struct tagDummySkillAniInfo
{
	ioHashString szAniName;
	float		 fAniRate;

	tagDummySkillAniInfo()
	{
		szAniName.Clear();
		fAniRate = 1.0f;
	}
} DummySkillAniInfo;

class ioDummyChar : public ioPlayEntity, public ioWorldEventReceiver
{
public:
	enum DummyCharTraceState
	{
		DCTS_NONE,
		DCTS_DELAY,
		DCTS_SENSING,
		DCTS_TRACING,
		DCTS_ATTACK,
		DCTS_TARGET_JUMP,
		DCTS_JUMP_ATTACK,
		DCTS_TRACING_WAIT,
	};
	enum DummyCharSync
	{
		DUMMY_CHAR_DAMAGE				= 1,
		DUMMY_CHAR_DIE					= 2,
		DUMMY_CHAR_DELAY				= 3,
		DUMMY_CHAR_NONE_DELAY			= 4,
		DUMMY_CHAR_KEY_CHANGE			= 5,
		DUMMY_CHAR_NORMAL_ATT			= 6,
		DUMMY_CHAR_CHARGE_ATT			= 7,
		DUMMY_CHAR_TRACE				= 8,
		DUMMY_CHAR_TRACE_TARGET			= 9,
		DUMMY_CHAR_TRACE_TARGET_ATTACK	= 10,
		DUMMY_CHAR_DUMMY_TOWER			= 11,
		DUMMY_CHAR_APPLY_WEAPON			= 12,
		DUMMY_CHAR_ATTACK_TOWER			= 13,
		DUMMY_CHAR_ATTACK_KEY			= 14,
		DUMMY_CHAR_TRACE_ATTACK_SYNC	= 15,
		DUMMY_CHAR_TURRET_SYNC			= 16,
		DUMMY_CHAR_STRIDER_SYNC			= 17,
		DUMMY_CHAR_STRIDER_SD_SYNC		= 18,
		DUMMY_CHAR_SYNC					= 19,//기존 더미 이외 사용 프로토콜 타입
	};
	enum OwnerAttach2AttackType
	{
		AT_NORMAL,
		AT_AIR,
		AT_DOWN,
	};
	enum CharColType
	{
		CCT_NONE,
		CCT_ALL,
		CCT_TEAM,
		CCT_OWNER,
	};
	enum
	{
		JUMP_POWER = 1750,
		JUMP_SPEED = 1400,
		JUMP_RANGE = 600,
	};
	enum GrassType 
	{
		DGT_NONE,
		DGT_GRASS1,
	};
	enum EntityColSkip
	{
		ECS_NONE,
		ECS_ALL,
		ECS_OWNER,
	};
	enum DummyCharColSkip
	{
		DCCS_NONE,
		DCCS_ALL
	};
	enum ColDieType
	{
		CDT_NONE,
		CDT_WOUND,
	};

protected:
	// Create
	ioHashString m_OwnerName;
	ioHashString m_CreatorName;
	int m_iCreateIndex;
	ioHashString m_DummyCharName;

	ioHashString m_RedTeamEffect;
	ioHashString m_BlueTeamEffect;

	UniqueObjID m_dwTeamEffect;

	bool m_bMale;

	DWORD m_dwCreateTime;
	DWORD m_dwProcessTime;

	DWORD m_dwDelayStartTime;

	ioAniCollisionBoxGrp *m_pSpecialBox;

	bool m_bFirst;

	bool m_bVisibleMiniMap;
	
	// status
	DummyCharType m_DummyCharType;
	DummyCharState m_State;

	ArmorType m_ArmorType;

	CEncrypt<bool> m_bDisableHPVisible;
	EntityColSkip m_eEntityColSkip;
	CharColType m_CharColSkip;
	DummyCharColSkip m_DummyCharColSkip;

	ioGaugeHelper *m_pGauge;

	DWORD m_dwCheckHPTime;
	DWORD m_dwTicTime;
	DWORD m_dwTicTimeByTraining;
	CEncrypt<float> m_fDecreaseHP;

	ioHashString m_BlockBuff;

	CEncrypt<float> m_fGravityAmt;
	CEncrypt<float> m_fGravityRate;
	CEncrypt<float> m_fCurJumpPowerAmt;
	CEncrypt<float> m_fControlOutRange;
	CEncrypt<float> m_fControlMinRange;

	CEncrypt<bool> m_bNoDamage;
	CEncrypt<bool> m_bGaugeVisibleOnlyOwner;
	CEncrypt<bool> m_bDestroyMapCol;
	CEncrypt<bool> m_bOwnerRotate;
	CEncrypt<bool> m_bOneTimeAttack;

	CEncrypt<bool> m_bPiercingProtected;
	CEncrypt<bool> m_bGrowthUp;

	CEncrypt<float> m_fDamageRate;
	CEncrypt<float> m_fAbsoluteDamage;

	CEncrypt<bool> m_bOwnerAttached;

protected:
	ioHashString m_LastAttacker;

	ioHashString m_DestroyEffect;
	ioHashString m_DummyEffect;

	bool m_bMoveEffectCreated;

// attack
protected:
	typedef std::map< int, DummyAttackInfo > DummyAttackMap;
	DummyAttackMap m_DummyAttackMap;

	int m_iReserveAttackCode;
	FireTimeList m_vAttackFireTimeList;

	ReservedSlidingList m_vReservedSlidingList;

	D3DXVECTOR3	m_vForceDir;
	CEncrypt<float>	m_fForceAmt;
	CEncrypt<float>	m_fForceFrictionRate;
	CEncrypt<bool>	m_bForcePowerLinear;

	DWORD m_dwBaseWeaponIndex;

// animation
protected:
	ioHashString m_StartAnimation;
	float m_fStartAniRate;

	CEncrypt<float> m_fStartAngle;
	CEncrypt<bool> m_bKeepStartAngle;

	ioHashString m_DelayAnimation;
	float m_fDelayAniRate;

	ioHashString m_MoveAnimation;
	float m_fMoveAniRate;

	ioHashString m_DashAnimation;
	float m_fDashAniRate;

	DWORD m_dwStartAniEndTime;
	DWORD m_dwAttackAniEndTime;

	CEncrypt<float>	m_fDummyCharHeightOffset;
// owner offset
protected:
	float m_fOwnerFrontOffset;
	float m_fOwnerHeightOffset;
	float m_fOwnerSideOffset;
// owner trace
protected:
	float m_fFrontOffset;
	float m_fHeightOffset;
	float m_fSideOffset;

// target trace
protected:
	DummyCharTraceState m_TraceState;
	TargetWoundType m_TargetWoundType;

	CEncrypt<DWORD> m_dwDelayDuration;
	CEncrypt<DWORD> m_dwTraceStateDuration;

	CEncrypt<DWORD> m_dwRotateSpeed;
	CEncrypt<float> m_fEnableTraceMoveHeight;
	CEncrypt<float> m_fTraceMoveSpeed;

	CEncrypt<DWORD> m_dwSensingDuration;
	CEncrypt<float> m_fSensingRange;
	CEncrypt<float> m_fSensingRange2;
	CEncrypt<float> m_fSensingAngle;

	CEncrypt<float> m_fTraceSpeed;
	CEncrypt<float> m_fTraceRange;
	CEncrypt<float> m_fFirstTraceRange;
	CEncrypt<DWORD> m_dwTraceDuration;

	CEncrypt<bool> m_bCharColOnlyAttack;
	CEncrypt<float> m_fTraceAttackRange;
	CEncrypt<float> m_fTraceAttackAngle;
	CEncrypt<int> m_iTraceNormalAttack;

	D3DXVECTOR3 m_vTracingMoveDir;
	CEncrypt<DWORD> m_dwSensingStartTime;
	CEncrypt<DWORD> m_dwTracingEndTime;

	ioHashString m_TargetTraceName;

	ioHashString m_SensingAni;
	float m_fSensingAniRate;

	ioHashString m_TracingAni;
	float m_fTracingAniRate;

	//악어(DCT_TARGET_TRACE5)
protected:
	DWORD m_dwLifeTime;
	DWORD m_dwStartTime;
	std::vector<ioHashString> m_vTargetedChar;
	int   m_nCanEatCnt;

	int m_iColDieType;

public:
	bool AddWoundedTargetName( const ioHashString& stCharName );

// extra ani jump
protected:
	DWORD m_dwExtraAniJumpTime;
	float m_fExtraAniJumpPower;

	int m_iCurExtraAniJump;
	vExtraAniJumpList m_vExtraAniJumpList;
	CEncrypt<bool> m_bFindTargetAttack;
// owner control
protected:
	CEncrypt<bool>	m_bDisableRecalcByTerrain;
	CEncrypt<float>	m_fCameraHeight;
	CEncrypt<float> m_fMoveSpeed;
	CEncrypt<float> m_fMoveFriction;
	CEncrypt<float> m_fMoveSpeedOnCharging;
	CEncrypt<float> m_fMoveFrictionOnCharging;
	CEncrypt<float> m_fUpDownSpeed;
	CEncrypt<float> m_fMinHeight;
	CEncrypt<float> m_fMaxHeight;
	CEncrypt<DWORD> m_dwFireDuration;

	bool m_bSetUpKey;
	bool m_bSetDownKey;

	ioUserKeyInput::DirKeyInput m_CurDirKey;

	DWORD m_dwAttackKeyCheckTime;
	DWORD m_dwAttackChargeTime;
	DWORD m_dwAttackChargeRepeatTime;

	int m_iControlNormalAttack;

	WeaponInfo m_ControlChargeWeaponInfo;
	int m_iChargeWeaponIndex;

	ioHashString m_ControlSkillName;

	float m_fCurMoveSpeed;
	float m_fTestValue;

	CEncrypt<bool> m_bAutoDescent;
	CEncrypt<bool> m_bAttackToDestroyDummy;

	// OwnerAttach2
	CEncrypt<float>	m_fFindTargetRange;
	CEncrypt<float>	m_fFindTargetAngle;

	CEncrypt<float> m_fFindTargetMinAngle;
	CEncrypt<float> m_fFindTargetMaxAngle;
	
	CEncrypt<bool>	m_bDisableColGrap;
	D3DXVECTOR3		m_vAttackDir;	
	DWORD			m_dwNextFireTime;

	struct Attach2AttackInfo
	{
		DWORD	m_dwFireGapTime;
		int		m_iAutoFireCode;
		void Init()
		{
			m_dwFireGapTime = 0;
			m_iAutoFireCode = 0;
		}
	};
	typedef std::vector<Attach2AttackInfo> AttackInfoList;
	AttackInfoList m_AttackInfoList;
	OwnerAttach2AttackType m_AttachAttackType;

	CEncrypt<float>	m_fTargetAimRate;
	CEncrypt<float>	m_fDownTargetAimRate;

	// Target Trace 3
	CEncrypt<int>	m_iMaxFireCount;
	int				m_iCurFireCount;

	ioHashStringVec	m_vTargetList;
	TargetDistInfoList	m_vTargetInfoList;
	ioHashString	m_szCurTargetName;
	ioHashString	m_szPreTargetName;

	CEncrypt<float>	m_fDelayMoveRange;
	float			m_fCurMoveRange;

	DWORD			m_dwSeed;
	bool			m_bEnableAttack;

	// Sound..
	ioHashString	m_szControlAttackSound;
	bool			m_bSoundLoop;
	UniqueObjID     m_AttackSoundID;

	ioHashString	m_szBGM;
	bool			m_bBGMLoop;
	UniqueObjID m_BGMID;

//for ioSummonWoodSkill
protected:
	bool m_bAreadyPierceStart;
	ioHashString m_WoodGrowthUpEffect;
	DWORD m_dwWoodGrowthUpEffect;
	
	ioHashString m_PierceStartSound;
	UniqueObjID m_PierceStartSoundID;

	FireTime m_PierceWeapon;
	bool m_bEffectVisible;

protected:
	ioHashString m_szPreEffect;
	UniqueObjID m_PreEffectID;
	DWORD m_dwPreDuration;

//for Rush
protected:
	WeaponInfoList m_AttachWeaponInfoList;
	DWORDVec m_vAttachWeaponIndexList;

protected:
	CEncrypt<DWORD> m_dwDummyTowerCheckGapTime;
	CEncrypt<float> m_fDummyTowerCheckRange;

	ioHashString m_DummyTowerDummyName;
	CEncrypt<int> m_iDummyTowerDummyNum;

	DWORD m_dwDummyTowerCheckTime;

//for AttackTower (디스트로이어 ASD에서 시작됨)
protected:
	DWORD m_dwAttackTowerTime;
	int	  m_nAttackTowerAttackCnt;

	float m_fWeaponOffset_Front_Back;
	float m_fWeaponOffset_Up_Down;

//for 미카엘
protected:
	ioHashString m_stNoOverLapTargetName;

//for CurseDoll
	float m_fCurseDollCheckLandHeight;
	DWORD m_dwCurseDollDamageTime; //ini
	DWORD m_dwCurseDollCheckDamageTime;
	bool  m_bCurseDollLandState;
	bool  m_bCurseDollAutoAttackDisable;
	ioHashString m_stTargetName;

	CEncrypt<DWORD> m_dwDropZoneLiveTime;
	DWORD           m_dwDropZoneCheckStartTime;

	// JumpAttack
	float m_fJumpSpeed;
	float m_fCurDummyJumpPower;

	CEncrypt<float> m_fJumpAttackRate;
	CEncrypt<float> m_fJumpAttackAccurate;
	CEncrypt<float> m_fMinRange;
	
	// BuffSkip
	std::vector<ioHashString> m_vSkipBuffList;
	
	CEncrypt<bool> m_bCheckWounedState;
	CEncrypt<bool> m_bTraceOwnerSync;
	CEncrypt<bool> m_bDummyDeadDestoryWeapon;

protected:
	CEncrypt<bool> m_bMountEnable;
	CEncrypt<bool> m_bCurMountEnable;
	bool m_bOwnerMountEnable;

	MountedEntityList m_vMountEntityList;
	MountedWeaponList m_vMountWeaponList;

	ioHashString m_KillerChar;

	CEncrypt<bool> m_bDisableMapCollision;
	CEncrypt<bool> m_bDiableOwnerWoundDestroy;
	bool m_bNoChangeEffectHeight;

	bool m_bStartAttchWeapon;
	bool m_bNoGravityMove;
	bool m_bIgnoreChargeWeaponDead;

	WeaponInfoList m_EndWeaponInfoList;
	bool		   m_bAlreadyCallEndWeapon;
	bool		   m_bEndWeaponAttachOwner;
	bool m_bIsNoCheckPreCollision;
	// 로켓 런처 용병이 사용하는 DCT_OWNER_CONTROL6 타입 더미 맵 & 푸시 스트럭트 충돌 처리 안함
	bool			m_bCheckMapCollision;
	bool			m_bCheckStructCollision;

	bool			m_bSyncForOtherUser;

protected:
	int	m_iCurDummySkillAniCode;
	typedef std::map< int, DummySkillAniInfo > DummySkillAniInfoMap;
	DummySkillAniInfoMap m_DummySkillAniInfoMap;

public:
	void SetCurseDollTargetName( const ioHashString& stName );

public:
	void InitResource();
	virtual void InitDummyCharInfo(	int iIndex,
							DWORD dwCreateGapTime,
							const D3DXVECTOR3 &vPos,
							const D3DXVECTOR3 &vScale,
							float fStartAngle=0.0f, bool bNetWork = false );

public:
	virtual void LoadProperty( ioINILoader &rkLoader );

	virtual void Update( float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime );
	virtual void UpdateAfter( float fFrameGap );
	virtual void NeedUpdateBottomHeight();

	virtual void Translate( const D3DXVECTOR3 &vMove );
	virtual void TranslateByConveyer( const D3DXVECTOR3 &vMove );
	virtual void TranslateByWorldEntity( const D3DXVECTOR3 &vMove );

	virtual ioWorldEventReceiver* GetWorldEventReceiver();

	virtual void OnTelepotation( const D3DXVECTOR3 &vTargetPos );
	virtual void OnFireZoneDamage( float fDamage, const ioHashString &szBuff );
	virtual void OnPoisonZoneDamage();

	virtual PlaySubType GetSubType() const;
	virtual ArmorType GetArmorType() const;

	virtual void MapCollisionPushingTranslate( const D3DXVECTOR3 &vMoveAmt );

	virtual bool TestMapCollision( ioOpcodeShape *pShape, ioPlayStage *pStage, const D3DXMATRIX* pShapeWorldMat );
	virtual void CheckEntityCollision( ioPlayEntity *pEntity, bool bDefense );

	//Test 윤태철
	virtual int GetDummyGrassType();

protected:
	virtual void UpdateBottomHeight() const;

public:
	virtual int  DontMoveEntityLevel() const;

	virtual bool IsCollisionAvailableWeapon( ioWeapon *pWeapon );
	virtual bool IsEntityCollisionSkipState( ioGameEntity* pColEntity ) const;
	virtual bool IsNeedCheckMapCollision() const;
	virtual bool IsMountingAvailable() const;
	virtual bool IsMountAvailable() const;
	virtual bool IsNowMountEnable() const;
	virtual bool IsCharCollisionSkipState( TeamType eTeamType, int iState=0 ) const;
	virtual bool IsActivateWeapon();
	virtual bool IsDummyVsDummyCollisionSkipState( const ioDummyChar *pDummy );
	bool IsDummyCharCollisionSkipState( const ioBaseChar *pChar );	
	bool IsVisibleMiniMap() { return m_bVisibleMiniMap; }

public:
	void SetHP( float fCurHP, float fMaxHP );

	virtual void UpdateGauge();
	virtual void RenderGauge();

protected:
	virtual void ProcessState( float fTimerPerSec, DWORD dwPreTime );
	virtual void ProcessPos( float fTimePerSec );

	virtual void ProcessStartState( float fTimePerSec );
	virtual void ProcessDelayState( float fTimePerSec );
	virtual void ProcessMoveState( float fTimePerSec );
	virtual void ProcessDashState( float fTimePerSec );
	virtual void ProcessAttackState( float fTimePerSec, DWORD dwPreTime );

protected:
	void ProcessForceMove( float fTimePerSec );

	void ProcessNoneDelayState( float fTimePerSec );
	void ProcessLoopAttackState( float fTimePerSec, DWORD dwPreTime );
	void ProcessDieState( float fTimePerSec );
	void ProcessTraceState( float fTimePerSec, DWORD dwPreTime );

	void ProcessTraceMoving( ioPlayStage *pStage, float fMoveAmt, float fTimePerSec );
	void ProcessTraceMovingNoneTerrain( ioPlayStage *pStage, float fMoveAmt, float fTimePerSec );
	void ProcessTraceSensing( float fTimePerSec );
	void ProcessTraceTracing( float fTimePerSec );
	void ProcessTraceTracingNoneTerrain( float fTimePerSec );
	void ProcessTraceAttack( float fTimePerSec, DWORD dwPreTime );

	void ProcessTraceTracingWait( float fTimePerSec, DWORD dwPreTime );


	void ProcessPiercingState( float fTimePerSec );
	void ProcessEffectUpdate( float fTimePerSec );

	bool IsEnableTargetState( ioBaseChar *pChar );
	void FindTraceTarget( ioPlayStage *pStage );
	void CheckTraceTarget( ioPlayStage *pStage );
	
	ioHashString GetTraceTarget( ioPlayStage *pStage, bool bSecond );
	ioHashString GetTraceTargetNoOverlap( ioPlayStage *pStage, bool bSecond );//중복없이 찾는..

	void CheckTraceAttack();
	void CheckTraceStateEnd();
	void CheckDummyTower();
	void CheckAttackTower();
	void CheckDieByHp();
	void CheckCurseDoll();

	void SetTracingState( ioPlayStage *pStage, bool bSendNetwork );
	void SetTracingAttack();

	bool CheckEnableDummySyncByType();

public:
	void SetControlSkillName( const ioHashString &szSkillName );

	void CheckMapCollision( ioPlayStage *pStage );

	void SetVisibleDummy( bool bVisible );
protected:
	// owner_control
	void CheckKeyInputChange();
	bool CheckAttackKeyControl();
	void CheckControlMove( float fTimePerSec );
	void CheckChargeWeaponDead();

	void ProcessDelayOwnerControl( float fTimePerSec );
	void ProcessMoveOwnerControl( float fTimePerSec );
	void ProcessChargeOwnerControl( float fTimePerSec );

public:
	virtual ApplyWeaponExp ApplyWeapon( ioWeapon *pWeapon );
	void OnDamaged( SP2Packet &rkPacket, ioBaseChar *pAttacker );

	virtual void ApplyDummyCharSync( ioPlayStage *pStage, SP2Packet &rkPacket );

	virtual void SetDelayState( bool bNet );
	void SetNoneDelayState( bool bNet );
	virtual void SetMoveState();
	void SetControlAttackState( bool bStart );
	virtual void SetDieState( bool bEffect, bool bWoundedDie = false );
	void SendDieState( bool bEffect );
	void SetTraceState( DummyCharTraceState iTraceState );
	void SetTraceWaitState();
	virtual void SetDashState();

	void SetLoopAttackState();
	void CheckNextLoopAttack();

	void SetPiercingStartState();
	void SetPiercingState();
	void SetExtendDieState( ioPlayStage* pStage, ioBaseChar* pOwner );
	void SetPiercingPreDelayStartState();

	void SetReservedSliding( const ForceInfoList &vForceInfoList,
							 int iAniID,
							 float fTimeRate,
							 DWORD dwPreDely,
							 bool bMoveDir=true );

	void AddReservedSliding( DWORD dwSlidingTime, float fForceAmt, float fFriction, float fAngle, bool bMoveDir );
	void CheckReservedSliding();
	void ClearReservedSliding();

	void SetForcePower( const D3DXVECTOR3 &vForceDir,
						float fForceAmt,
						float fFrictionRate = FLOAT1,
						bool bForceLinear = false );
	
	inline float GetForceAmt() const { return m_fForceAmt; }
	//inline const D3DXVECTOR3& GetForceDir() const { return m_vForceDir; }

	bool CheckEnableAttackState();
	void SetReserveAttackCode( int iCode );
	bool CheckOwnerDestroyState();

	float GetCurAngle();

protected:
	void ApplyDummyCharSyncByType( int iType, SP2Packet &rkPacket, ioPlayStage *pStage );

public:
	void SetOwner( ioHashString szOwnerName );

	ioBaseChar* GetOwner() const;

	inline DummyCharType GetType() const { return m_DummyCharType; }
	inline DummyCharState GetState() const { return m_State; }
	inline const ioHashString& GetOwnerName()   { return m_OwnerName; }
	inline const ioHashString& GetCreatorName() { return m_CreatorName; }
	inline int GetDummyCharIdx() const { return m_iCreateIndex; }
	
	inline DWORD GetCreateTime() const { return m_dwCreateTime; }
	inline const ioHashString& GetDummyCharName() const { return m_DummyCharName; }

	inline void SetMale( bool bMale ) { m_bMale = bMale; }
	inline bool IsMale() const { return m_bMale; }

	inline float GetStartAngle() const { return m_fStartAngle; }

	inline bool isOwnerAttached() const { return m_bOwnerAttached; }

public:
	virtual void SetAttackState();
	void CheckSetAttackState();

protected:
	virtual void DropZoneCheck();
	void CheckDestroyRange();

	void UpdateOwnerTrace();
	void SetJumpAttackState();
	void FindAttackTheTarget( const ioHashString &szTargetName );

protected:
	void CheckExtraAniJump();

// OwnerAttach2
protected:
	ioBaseChar* CheckEnableTarget();
	bool CheckDownAimState( ioBaseChar *pTarget ) const;
	bool CheckTargetWoundType( ioBaseChar *pTarget ) const;

// Target Trace 3
protected:
	void FindTarget( ioBaseChar *pChar );
	void SetTraceTargetMoveState();
	void ProcessMoveTraceTarget( float fTimePerSec );
	void CheckEnableAttack();
	void SetTraceTargetAttack();
	void ProcessTraceTargetAttack( float fTimePerSec, DWORD dwPreTime );

// Rush
	void ProcessRush( float fTimePerSec );
	void SetAttachWeapon();

public:
	void CheckSpecialCollisionBox( const ioHashString &szFileName );
	virtual ioOrientBox GetAttackCollisionBox( AniCollisionType eType = ACT_NONE, bool bNoScale=false );

	ioEffect* AttachEffect( const ioHashString &rkEffect, ioEntityEvent *pEntityEventOwner );
	void EndEffect( const ioHashString &rkEffect, bool bGrace );

public:
	D3DXVECTOR3 GetCameraPosition();
	bool IsDisableRecalculateByterrain();

public:
	void SetWorldOrientationAndDir( const D3DXQUATERNION &qtRot );

public:
	virtual void AddEntityEvent( ioEntity *pEntity );

protected:
	virtual void SendAttackInfoToSkill();
	UniqueObjID PlaySound( ioBaseChar *pChar, ioHashString &szSoundName, bool bLoop );
	void StopSound( ioHashString &szSoundName, UniqueObjID eSoundID );

public:
	virtual void CheckCollisionWeapon( ioBaseChar *pChar );

public:
	bool IsTargetWoundedState( ioBaseChar* pTarget );
	bool IsNoCheckPreCollision()	{ return m_bIsNoCheckPreCollision; }
	bool CheckMapCollision()	{ return m_bCheckMapCollision; }
	bool CheckStructCollision()	{ return m_bCheckStructCollision; }

protected:
	virtual void TranslateMountEntity( const D3DXVECTOR3& vAmt );

	virtual void ClearMountEntity();
	virtual void UpdateMountedEntityList();
	virtual void UpdateMountedWeaponList();	

	virtual void SetWorldPositionMountEntity( float fHeightAmt );

	virtual void DummyDieClearMountEntity();

public:
	inline const bool GetMountEnable() const { return m_bMountEnable; }

public:
	void SetHide( bool bSetHide );
	bool CheckSyncForOtherUser() { return m_bSyncForOtherUser; }

public:
	virtual bool IsDummyCharMidSync(){ return false; }
	virtual void FillDummyCharMidSync( ioBaseChar* pOwner, SP2Packet &rkPacket );
	virtual void ApplyDummyCharMidSync( ioBaseChar* pOwner, SP2Packet &rkPacket );

public:
	virtual int GetSkillMaxActiveCnt();
	virtual int GetSkillCurActiveCnt();
protected:
	void CallEndWeapon( bool bWoundedDie );

public:
	void SetSkillDummyAniCode( int iSkillDummmyAniCode );
	void SetSkillDummyActionAni( DWORD& dwMotionEndTime, DWORD& dwFireStartTime );
	void SetSkillDummyLoopAni( DWORD& dwFireStartTime );

public:
	DWORD GetLifeTime() { return m_dwLifeTime; }

public:
	ioDummyChar( ioEntityGroup *pGrp, ioPlayMode *pMode );
	virtual ~ioDummyChar();
};

inline ioDummyChar* ToDummyChar( ioGameEntity *pObj )
{
	ioPlayEntity *pPlay = ToPlayEntity( pObj );
	if( !pPlay || pPlay->GetSubType() != ioPlayEntity::PST_DUMMY_CHAR )
		return NULL;

	return dynamic_cast< ioDummyChar* >( pPlay );
}

inline const ioDummyChar* ToDummyCharConst( const ioGameEntity *pObj )
{
	const ioPlayEntity *pPlay = ToPlayEntityConst( pObj );
	if( !pPlay || pPlay->GetSubType() != ioPlayEntity::PST_DUMMY_CHAR )
		return NULL;

	return dynamic_cast< const ioDummyChar* >( pPlay );
}

class DistSort : std::binary_function< const TargetDistInfo&, const TargetDistInfo&, bool >
{
public:
	bool operator()( const TargetDistInfo &lhs , const TargetDistInfo &rhs ) const
	{
		if( lhs.m_fLenth < rhs.m_fLenth )
		{
			return true;
		}

		return false;
	}
};
