#pragma once

#include "../io3DEngine/ioOrientBox.h"


#include "ioAttackAttribute.h"
#include "WeaponAttribute.h"
#include "ioEnhanceRate.h"

class ioMovableObject;
class ioCamera;
class ioRenderQueue;
class ioSphere;
class ioAxisAlignBox;
class ioPlayEntity;
class ioBaseChar;
class SP2Packet;
class ioPlayStage;
class ioINILoader;
class ioOpcodeShape;
class ioSceneNode;
class ioEffect;


struct ChangeDirInfo
{
	float		m_fChangeRange;
	float		m_fChangeSpeed;
	float		m_fChangeAngleH;
	float		m_fChangeAngleV;
	DWORD		m_dwCallTime;
	D3DXVECTOR3	m_vFireOffset;

	ChangeDirInfo()
	{
		m_fChangeRange	= 0.0f;
		m_fChangeSpeed	= 0.0f;
		m_fChangeAngleH	= 0.0f;
		m_fChangeAngleV	= 0.0f;
		m_vFireOffset	= ioMath::VEC3_ZERO;
	}
};

typedef std::vector< ChangeDirInfo > ChangeDirInfoList;

class ChangeDirSort : std::binary_function< const ChangeDirInfo&, const ChangeDirInfo&, bool >
{
public:
	bool operator()( const ChangeDirInfo &lhs , const ChangeDirInfo &rhs ) const
	{
		if( lhs.m_fChangeRange < rhs.m_fChangeRange )
		{
			return true;
		}
		else if( lhs.m_fChangeRange == rhs.m_fChangeRange )
		{
			if( lhs.m_fChangeAngleV < rhs.m_fChangeAngleV )
				return true;
		}
		return false;
	}
};

struct CollisionInfo
{
	ioPlayEntity* m_pEntity;
	DWORD m_dwCollisionTime;
};

typedef std::list< CollisionInfo > vCollisionInfoList;

enum DummyAndPushStructType
{
	OBJ_NONE,
	OBJ_DUMMY,
	OBJ_PUSH_STRUCT,
};

struct DummyAndPushStruct
{
	DummyAndPushStructType m_Type;
	DWORD m_dwSubIdx;
	ioHashString m_DummyCharName;

	DummyAndPushStruct()
	{
		m_Type = OBJ_NONE;
		m_dwSubIdx = 0;
	}
};

typedef std::vector< DummyAndPushStruct> DummyAndPushStructList;

struct WeaponAttribute;

class ioWeapon
{
public:
	enum WeaponType
	{
		WT_DUMMY,
		WT_CONTACT,
		WT_MISSILE,
		WT_THROW,
		WT_BOMB,
		WT_MINE,
		WT_NUCLEAR,
		WT_FLAME,
		WT_ZONE_EFFECT,
		WT_FLAME_WAVE,
		WT_SPLASH,						// 10
		WT_ARROW,
		WT_BLIZZARD,
		WT_THROWBOMB,
		WT_FLOAT_THROW,
		WT_THROW_RETURN,
		WT_EXPLOSION,
		WT_BOMB_MISSILE,
		WT_CANNON_BALL,
		WT_ROTATE,
		WT_TRACE,						// 20
		WT_BOUNDBOMB,
		WT_TARGET,
		WT_CHAIN,
		WT_BALL_WEAPON,
		WT_IGNORE_MISSILE,
		WT_AREA_EFFECT,
		WT_MAP_ATTACH,
		WT_LIMIT_ATTACH,
		WT_OWNER_ATTACK,
		WT_RUN_CONTACT,					// 30
		WT_WHIP,
		WT_ICE_MISSILE,
		WT_SUMMON_MISSILE,
		WT_GROUND_TRACE,
		WT_CONTROL_RETURN,
		WT_CONTROL_MISSILE,
		WT_TRACE_RETURN,
		WT_TSUNAMI,
		WT_TRACE2,
		WT_NORMAL,						// 40
		WT_ADHESIVE_MISSILE,
		WT_LIMIT_ATTACH2,
		WT_CONTACT2,
		WT_BOMB_MISSILE2,
		WT_ADHESIVE_MISSILE2,
		WT_ZONE_EFFECT2,
		WT_UROBORUS,
		WT_UROBORUS2,
		WT_TRACE3,
		WT_MISSILE2,					// 50
		WT_CALL_ATTRIBUTE_ARROW,
		WT_RETURN_PINGPONG,
		WT_MISSILE3,
		WT_LOBELIA,
		WT_SYLPHID_MISSILE,
		WT_GUIDE,
		WT_BALL2_WEAPON,
		WT_GUIDE2,
		WT_TRACE4,
		WT_FALL_WEAPON,					//60
		WT_TRACE5,
		WT_DOUBLE_ICE_MISSILE_WEAPON,
		WT_SMART_BOAM_WEAPON,
		WT_PULL_WEAPON,
		WT_ADHESIVE_MISSILE3,
		WT_GRAFFITI,
		WT_MINE2,
		WT_TRACE6,
		WT_CONTROL_SWORD,
		WT_PULL_WEAPON2,				//70
		WT_GROUND_TRACE2,
		WT_SELECT_POSITION_MOVE,
		WT_SIMPLE_BOX,
		WT_ADHESIVE_MISSILE4,
		WT_BOMB_MISSILE3,
		WT_TRACE7,
		WT_UROBORUS3,
		WT_LUCIFER_AIR_GRAP,
		WT_TRACE_OWNER,
		WT_IGNORE_MISSILE2,				//80
		WT_ONE_TARGET_WEAPON,
		WT_BOMB_MISSILE4,
		WT_ATTACH_WEAPON,
		WT_RETURN_MISSILE_WEAPON,
		WT_UROBORUS4,
		WT_FIRE_WORKS_STEP1_WEAPON,
		WT_FLOAT_THROW_BOM_WEAPON,
		WT_THROWBOMB2,
		WT_ROTATE_TARGET_WEAPON,
		WT_MINE3,						//90
		WT_RETURN_MISSILE_WEAPON2,
		WT_ZONE_MOVE_WEAPON,
		WT_PRISON_WEAPON,
		WT_UROBORUS5,
		WT_ROTATE2,
		WT_CONTROL_MESH,
		WT_MINE4,
		WT_DESTROY_CALL,
		WT_ATTACH_AREA,
		WT_MISSILE4,					//100
		WT_UROBORUS6,
		WT_ROTATE_ATTACH_CHAR,
		WT_PULL_WEAPON3,
		WT_CONTACT3,
		WT_BOMB_MISSILE5,
		WT_MINE5,
		WT_MISSILE5,
		WT_ROTATE_RETURN_WEAPON,
		WT_PULL_WEAPON4,
		WT_UROBORUS7,					//110
		WT_CONTROL_RETURN2,
		WT_GROUND_TRACE3,
		WT_THROWBOMB3,
		WT_SUMMON_MISSILE2,
		WT_MINE6,
		WT_PRISON_WEAPON2,
		WT_NO_COLLISION_WEAPON,
		WT_MISSILE6,
		WT_CONTINIOUS_CREATE,
		WT_PRISON_WEAPON3
	};

	enum OwnerRotType
	{
		ORT_NONE,
		ORT_NORMAL,
		ORT_STAFF3,
	};

protected:
	ioSceneNode *m_pSceneNode;
	const WeaponAttribute *m_pConstAttr;
	const WeaponAttribute *m_pResistanceAttr;

	ioHashString m_OwnerName;
	ioHashString m_CreateItem;

	CEncrypt<TeamType>	m_TeamType;
	CEncrypt<float>	m_fOwnerDamageRate;
	CEncrypt<float>	m_fOwnerDamageRateNPC;

	CEncrypt<float>	m_fOwnerChargeForceRate;
	CEncrypt<float>	m_fOwnerChargeDamageRate;
	
	CEncrypt<float>	m_fOwnerChargeRangeRate;
	CEncrypt<float>	m_fOwnerChargeSpeedRate;

	CEncrypt<float> m_fOwnerChargeBlowRate;
	CEncrypt<float> m_fOwnerChargeFloatRate;

	CEncrypt<float> m_fOwnerChargeScaleRate;
	CEncrypt<float> m_fOwnerChargeExplosionRate;

	CEncrypt<float> m_fOwnerChargeWoundedTimeRate;

	CEncrypt<float>	m_fOwnerChargeLiveTimeRate;

	CEncrypt<float>	m_fOwnerChargeBuffLiveTimeRate;

	CEncrypt<float> m_fEnhancePushPowerRate;
	CEncrypt<float> m_fEnhanceBlowPowerRate;
	CEncrypt<float> m_fEnhanceAirPushPowerRate;
	CEncrypt<float> m_fEnhanceAirBlowPowerRate;

	// Growth
	CEncrypt<float> m_fGrowthMoveRange;
	CEncrypt<float> m_fGrowthExplosionRange;

	CEncrypt<float> m_fGrowthAttackRate;
	CEncrypt<float> m_fGrowthPushRate;
	CEncrypt<float> m_fGrowthAirPushRate;
	CEncrypt<float> m_fGrowthBlowRate;
	CEncrypt<float> m_fGrowthAirBlowRate;

	AniCollisionType m_CollisionType;
	DWORD m_dwWeaponPeculiarity;
	DWORD m_dwWeaponIdx;

	bool m_bLoopWoundedAni;
	ioHashString m_WoundedAnimation;
	DWORD m_dwWoundedDuration;

	DWORD	m_dwCreatedTime;
	DWORD	m_dwLiveTime;

	DWORD   m_dwNetSyncTime;

	vCollisionInfoList m_vCollisionInfoList;

	ioOrientBox m_WorldAttackBox;
	ioOrientBox m_WorldAttackBoxForMap;
	D3DXVECTOR3 m_vPreWorldPosition;

	bool m_bBuffWeapon;
	bool m_bBuffWeapon2;
	bool m_bUseSkill;
	bool m_bJumpAttack;

	bool	m_bGameOwnerWeapon;
	bool	m_bLoopSoundPlayed;
	bool	m_bLive;
	bool	m_bFloatStateBreak;

	bool m_bEnableWoundSkill;

	int m_iStructIndex;
	bool m_bStructWeapon;

	int m_iMachineIndex;
	bool m_bMachineWeapon;

	int m_iDummyIndex;
	bool m_bDummyCharWeapon;

	ChangeJumpType m_ChangeJumppingType;
	ChangeNextItemType m_ChangeNextItemType;
	ChangeNextSkillType m_ChangeNextSkillType;

	CEncrypt<float> m_fChangeJumppingPowerRate;
	CEncrypt<int> m_iChangeJumppingCnt;

	CEncrypt<int> m_iIgnoreProtect;

	bool m_bResistance;
	CEncrypt<float> m_fWeakRate;
	DWORD m_dwWeakElement;

	bool m_bCheckOnlyDownAttack;
	bool m_bOnlyDownAttack;

	// For Piercing
	bool m_bSetCollision;
	DWORD m_dwPiercingEndTime;

	// For Collision Block
	ioHashString m_CharCollisionBlockBuff;

	ioHashString m_CreateBuffName;

	CEncrypt<bool> m_bCollisionCancel;
	CEncrypt<bool> m_bNoWoundedWeapon;
	CEncrypt<bool> m_bSkillDependency;

	GrapplingType m_CheckGrapplingType;

	WeaponInfoList m_vCreateWeaponList;

	//
	ioHashString m_GrapplingTargetName;

	//
	bool m_bMoveDelay;
	CEncrypt<DWORD> m_dwMoveDelayTime;

	FloatVec	m_vRandomDamageList;

	// 단순히 값을 받기위한 임시 변수
	DWORD	m_dwActionStopDuration;
	float	m_fActionStopLooseRate;

	//
	UniqueObjID m_WeaponLoopSoundID;

	//
	CEncrypt<bool> m_bSetSylphid;

	// 유령상태관련 예외
	IgnoreGhostStateType	m_IgnoreGhostState;

	CEncrypt<bool> m_bIgnoreMapCollision;
	CEncrypt<bool> m_bIgnoreMapCollisionEffect;

	//
	CEncrypt<bool> m_bEndEffectGrace;

	//데미지가 들어가도 ActionDelay를 주던 공식을 패스하낟.
	bool m_bPassCalcDamageActionStopDelay;


	//예약으로 만들어진 웨폰인가?
	bool m_bReserveCallWeapon;


	//웨폰 uniq_id(함정전문가때)
	int m_nUniqID;

protected:
	ioHashString m_AreaWeaponName;

	DWORD m_dwAttachAreaWeaponTime;
	DWORD m_dwAttachAreaWeaponIndex;

protected:
	static CEncrypt<float> m_fPushPowerCtrlValue;
	static CEncrypt<float> m_fBlowPowerCtrlValue;
	static CEncrypt<float> m_fAirPushPowerCtrlValue;
	static CEncrypt<float> m_fAirBlowPowerCtrlValue;

	struct ConstPowerValue
	{
		float m_fPushConst;
		float m_fBlowConst;
		float m_fAirPushConst;
		float m_fAirBlowConst;
	};

	static ConstPowerValue m_ConstPowerValue[BTT_MAX_TYPE];

protected:
	DummyAndPushStructList m_DummyAndPushStructDestroyList;
	bool m_bTeamAttack;

protected:
	bool m_bSetPush;
	float m_fEnablePushPowerRate;

protected:
	int		m_iPushStructType;
	float	m_fPushStructDamgeRate;
	bool	m_bPushStructOnlyAttack;

protected:
	struct DeadCallAreaInfo
	{
		ioHashString	CallAreaName;
		DWORD			dwReservedTime;

		DeadCallAreaInfo()
		{
			dwReservedTime = 0;
		}
	};
	typedef std::vector<DeadCallAreaInfo> DeadCallAreaInfoList;		
	DeadCallAreaInfoList m_DeadCallAreaWeaponNameList;

protected:
	CEncrypt<bool>	m_bDestroyByCharacterImmediatelyChange;

public:
	static void InitStaticValue( ioINILoader &rkLoader );

public:
	virtual bool IsAlreadyCollisioned( ioPlayEntity *pEntity );
	virtual void AddCollisionEntity( ioPlayEntity *pEntity );

	void UpdatePreWorldPosition();
	bool GetCalcActionDelayPassType(){ return m_bPassCalcDamageActionStopDelay; }

public:
	virtual void SetOwnerDamageRate( float fRate, float fNPCRate = FLOAT1 );

	virtual void SetOwnerChargeDamageRate( float fRate );
	virtual void SetOwnerChargeForceRate( float fRate );

	virtual void SetOwnerChargeRangeRate( float fRate );
	virtual void SetOwnerChargeSpeedRate( float fRate );

	virtual void SetOwnerChargeBlowRate( float fRate );
	virtual void SetOwnerChargeFloatRate( float fRate );

	virtual void SetOwnerChargeScaleRate( float fRate );
	virtual void SetOwnerChargeExplosionRate( float fRate );

	virtual void SetOwnerChargeWoundedTimeRate( float fRate );

	virtual void SetOwnerChargeLiveTimeRate( float fRate );

	virtual void SetOwnerChargeBuffLiveTimeRate( float fRate );
	
	void SetGameOwnerWeapon( bool bOwner );
	
	virtual void SetWeaponIndex( DWORD dwIndex );
	void SetWoundedLoopAni( bool bLoopAni ) { m_bLoopWoundedAni = bLoopAni; }
	void SetWoundedAnimation( const ioHashString &szAni ) { m_WoundedAnimation = szAni; }
	void SetWoundedDuration( DWORD dwDuration ) { m_dwWoundedDuration = dwDuration; }
	
	void SetAniCollisionType( AniCollisionType eType );
	
	void EnhancePushPower( float fPush, float fBlow, float fAirPush, float fAirBlow );

	inline void SetWeaponBySkill( bool bUseSkill ) { m_bUseSkill = bUseSkill; }
	inline void SetWeaponByBuff( bool bBuff ) { m_bBuffWeapon = bBuff; }
	inline void SetWeaponByBuff2( bool bBuff ) { m_bBuffWeapon2 = bBuff; }
	inline void SetWeaponByJump( bool bJump ) { m_bJumpAttack = bJump; }
	inline void SetWeaponByStruct( int iIndex ) { m_iStructIndex = iIndex; }
	inline void SetWeaponByDummyChar( int iIndex ) { m_iDummyIndex = iIndex; }
	inline void SetWeaponByMachine( int iIndex ) { m_iMachineIndex = iIndex; }

	inline bool CheckWeaponBySkill() const { return m_bUseSkill; }
	inline bool CheckWeaponByBuff() const { return m_bBuffWeapon; }
	inline bool CheckWeaponByBuff2() const { return m_bBuffWeapon2; }
	inline bool CheckWeaponByJump() const { return m_bJumpAttack; }
	inline bool CheckWeaponByStruct() const { return m_bStructWeapon; }
	inline bool CheckWeaponByDummyChar() const { return m_bDummyCharWeapon; }
	inline bool CheckWeaponByMachine() const { return m_bMachineWeapon; }
	inline bool CheckWeaponByFloatBreak() const { return m_bFloatStateBreak; }

public:
	void SetPosition( const D3DXVECTOR3 &vPos );
	void SetOrientation( const D3DXQUATERNION &qtRot );
	void SetScale( const D3DXVECTOR3 &vScale );

	virtual void SetGrapplingTargetName( const ioHashString &rkName );
	virtual void SetCreateItem( const ioHashString &rkName );
	virtual void SetTeam( ioPlayStage *pStage, TeamType eType );

	virtual void CallReserveAreaWeapon();
	virtual void SetWeaponDead();
	virtual void PlayExplosionSound();
	virtual void PlayWoundedSound( ioPlayEntity *pEntity );
	virtual void PlayDefensedSound( ioPlayEntity *pEntity );
	virtual void PlayLoopSound();
	virtual void StopLoopSound();

	void SetReserveCallWeapon();

public:
	bool IsTeamAttackZeroDamage( ioBaseChar *pTarget );
	bool IsEnableItemDrop( ioBaseChar *pTarget );

	bool HasFeature( DWORD dwFeature ) const;

	void SetPeculiarity( DWORD dwPeculiarity );
	bool HasPeculiarity( DWORD dwPeculiarity ) const;

	virtual const BuffInfoList& GetTargetAirBuffList() const;
	virtual const BuffInfoList& GetTargetBuffList() const;	
	virtual const BuffInfoList& GetTargetDownBuffList() const;
	virtual const BuffInfoList& GetTargetDefenseBuffList() const;


	virtual const BuffInfoList& GetTargetAttackBuffList() const;	// 2020-02-19
	virtual const BuffInfoList& GetTargetDashBuffList() const;		// 2020-02-19
	bool IsTargetDashBuffActive() const;
	bool IsTargetAttackBuffActive() const;


	virtual const D3DXVECTOR3& GetPosition() const;

	const D3DXQUATERNION& GetOrientation() const;
	const D3DXVECTOR3& GetScale() const;

	inline const D3DXVECTOR3& GetPrePosition() const { return m_vPreWorldPosition; }

	inline bool IsGameOwnerWeapon() const { return m_bGameOwnerWeapon; }
	inline bool IsLive() const { return m_bLive; }
	inline TeamType GetTeam() const { return m_TeamType; }

	inline DWORD GetWeaponIndex() const { return m_dwWeaponIdx; }

	inline bool HasWoundedAnimation() const { return !m_WoundedAnimation.IsEmpty(); }
	inline bool IsLoopWoundedAnimation() const { return m_bLoopWoundedAni; }
	inline const ioHashString& GetWoundedAnimation() const { return m_WoundedAnimation; }
	DWORD GetWoundedDuration();
	
	inline const ioHashString& GetCharCollisionBlockBuff() const { return m_CharCollisionBlockBuff; }

	inline float GetChangeJumppingPowerRate() const { return m_fChangeJumppingPowerRate; }
	inline int GetChangeJumppingLimitCnt() const { return m_iChangeJumppingCnt; }

	inline int IsIgnoreProtect() const { return m_iIgnoreProtect; }

	inline void SetCollisionCancel( bool bCancel ) { m_bCollisionCancel = bCancel; }
	inline bool IsCollisionCancel() const { return m_bCollisionCancel; }
	inline bool IsNoWoundedWeapon() const { return m_bNoWoundedWeapon; }

	inline bool CheckWoundSkill() const { return m_bEnableWoundSkill; }

	inline const ioHashString& GetGrapplingTargetName() const { return m_GrapplingTargetName; }
	bool IsUseGrapplingTarget();

	bool IsNoExplosionEffectWithChar() const;

	int GetAttributeIndex() const;
	int GetResistanceAttributeIndex() const;
	int GetShakeCamera() const;

	bool IsChangeJumpping( bool bDefense, bool bBlowWounded );
	bool IsChangeNextItemType( bool bDefense );
	bool IsChangeNextSkillType( bool bDefense );

	ChangeNextItemType GetChangeNextItemType();

	const ioHashString& GetExplosionEffectName() const;
	const ioHashString& GetExtraExplosionEffectName() const;
	const ioHashString& GetWeaponEndEffect() const;
	
	virtual BlowTargetType GetBlowType() const;
	CharPiercingType GetPiercingType() const;
	DefenseBreakType GetDefenseBreakType() const;
	DefenseSuccessType GetDefenseSuccessType() const;
	EnableSideType GetEnableSideType() const;
	EscapeCatchType GetEnableEscapeCatchType() const;
	
	bool IsDefenseBreakBuff() const;
	const ioHashString& GetDefenseBreakBuff() const;

	bool IsSelectCollisionBuff() const;
	bool IsSelectCollisionBuffReverse() const;

	DWORD GetWeaponPeculiarity() const;
	DWORD GetWeaponFeature() const;

	virtual float GetPushPower( float fDamage ) const;
	virtual float GetBlowPower( float fDamage ) const;
	virtual float GetAirPushPower( float fDamage ) const;
	virtual float GetAirBlowPower( float fDamage ) const;
	virtual float GetDefensePushPower( float fDamage ) const;

	float GetBallPushPower( float fDamage ) const;
	float GetBallBlowPower( float fDamage ) const;
	float GetBallAirPushPower( float fDamage ) const;
	float GetBallAirBlowPower( float fDamage ) const;

	float GetGangsiPushPower( float fDamage ) const;
	float GetGangsiBlowPower( float fDamage ) const;
	float GetGangsiAirPushPower( float fDamage ) const;
	float GetGangsiAirBlowPower( float fDamage ) const;

	float GetDownStateBlowPower();
	float GetDownStatePushPower();

	float GetBoundBlowPower( float fDamage ) const;
	float GetBoundPushPower( float fDamage ) const;
	float GetBoundBlowGravityRate() const;
	float GetBoundDamage( ioPlayEntity *pTarget );

	float GetDefenseDamageRate() const;
	float GetDefenseDamageRate2() const;

	const ioHashString& GetExtendWoundAni() const;
	DWORD GetExtendWoundDuration() const;
	float GetExtendWoundBlowPower( float fDamage ) const;
	float GetExtendWoundPushPower( float fDamage ) const;

	float CheckDamageIncreaseBuff( ioBaseChar *pChar );
	float CheckPushIncreaseBuff( ioBaseChar *pChar );
	float CheckBlowIncreaseBuff( ioBaseChar *pChar );

	const BlowWoundAniInfo& GetBlowWoundAniInfo() const;

	AttackStatType GetStatType();
	AttackMethodType GetAttackMethodType();
	int GetExtraGaugeType();
	bool IsEnablePossession();

	float GetDefenseEnableRate() const;

	inline const ioHashString& GetOwnerName() const { return m_OwnerName; }
	inline const ioHashString& GetCreateItem() const { return m_CreateItem; }
	inline const int GetWeaponByDummyChar() { return m_iDummyIndex; }
	const ioHashString& GetWeaponBySkillName() const;
	ioBaseChar* GetOwner() const;
	bool IsExperienceOwner();

	ElementType GetElementType();

	int GetBlockingRate() const;
	int GetNpcBlockingRate( ioBaseChar *pTarget );
	int GetGangsiBlockingRate( ioBaseChar *pTarget );
	int GetFCBlockingRate( ioBaseChar *pTarget );

	float GetOwnerChargeBuffDurationRate();

	// defense cancel
	bool HasDefenseCancelAni();
	const ioHashString& GetDefenseCancelAni() const;
	float GetDefenseCancelAniRate() const;

public:
	virtual DWORD GetCollisionEnableTime() const;

	void SetCreatedTime( DWORD dwTime ) { m_dwCreatedTime = dwTime; }
	inline DWORD GetCreateTime() const { return m_dwCreatedTime; }
	
	void  SetActionStopDuration( DWORD dwDuration, float fLooseRate );
	inline DWORD GetActionStopDuration() const { return m_dwActionStopDuration; }
	inline float GetActionStopLooseRate() const { return m_fActionStopLooseRate; }

	virtual int SetSylphid();

	virtual void ApplyExtraReserveWeaponByWeaponInfo( ioWeapon* pWeapon );

public:
	void SetGrowthAttackRate( float fAttackRate, float fPushRate, float fAirPushRate, float fBlowRate, float fAirBlowRate );
	inline float GetGrowthAttackRate() const { return m_fGrowthAttackRate; }

	void SetGrowthMoveRange( float fRange );
	void SetGrowthExplosionRange( float fRange );

	DWORD GetIgnoreWeaponTime();

public:
	inline int GetAniCollisionType() const { return m_CollisionType; }
	float GetExtraDamageRate();

	void CheckOnlyDownAttack( ioBaseChar *pOwner );
	inline bool IsOnlyDownAttack() const { return m_bOnlyDownAttack; }

public:
	virtual bool InitResource();
	virtual void DestroyResource( ioPlayStage *pStage=NULL );

	virtual void LoadProperty( ioINILoader &rkLoader );
	void LoadDummyAndPushStructDestroyList( ioINILoader &rkLoader  );

	virtual void SetResistanceAttr( int iResistanceIdx );

	virtual void SetStartCollisionBox();
	virtual void SetLiveTime( DWORD dwTime );
	virtual void SetMoveDir( const D3DXVECTOR3 &vMoveDir );
	virtual void SetMoveSpeed( float fSpeed );
	virtual void SetFloatPower( float fFloatPower );
	virtual void SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage );

	virtual void Process( float fTimePerSec, ioPlayStage *pStage );
	virtual bool IsCollision( ioPlayEntity *pEntity, bool bMapCollision=false );
	virtual bool TestMapCollision( ioOpcodeShape *pShape, ioPlayStage *pStage, const D3DXMATRIX* pShapeWorldMat );

	virtual void CheckPreCollision( ioPlayEntity *pTarget, ioPlayStage *pStage );
	virtual void CheckStructAndDummy( ioPlayEntity *pTarget, ioPlayStage *pStage );

	virtual float GetDamage( ioPlayEntity *pTarget );
	virtual float GetBaseDamage( ioPlayEntity *pTarget );
	virtual float GetPushAngle() const;

	virtual int IsResistanceMagic( ioBaseChar *pChar );
	virtual int IsResistanceBomb( ioBaseChar *pChar );
	virtual bool IsCanMineExplosion();

	virtual GrapplingType CheckGrapplingState();

public:
	virtual void CheckCurAttribute( const ioPlayEntity *pTarget );
	virtual bool CheckItemNotDrop( ioBaseChar *pChar );

	virtual void GetWorldAttackBoxList( std::vector< ioOrientBox > &vBoxList );
	virtual D3DXVECTOR3 GetAttackDir( const ioPlayEntity *pEntity ) const;
	virtual const ioOrientBox& GetLocalAttackBox() const;

	virtual DWORD GetLiveTime();
	virtual D3DXVECTOR3 GetMoveDir();

	virtual void CheckWeaponCollision();

	const ioOrientBox& GetWorldAttackBox( bool bMap = false ) const;

	virtual WeaponType GetType() const = 0;
	virtual ActionStopType GetActionStopType() const = 0;
	virtual ActionStopType GetDieActionStopType() const = 0;

	virtual DWORD GetRefreshDuration();
	virtual DWORD GetIgnoreDuration();
	virtual DWORD GetIgnoreTimeTypeDuration();
	virtual DWORD GetIgnoreTimeType();

	virtual void ApplyWeaponSync( SP2Packet &rkPacket, ioPlayStage *pStage );

	inline void SetCreateBuffName( const ioHashString &szBuffName ) { m_CreateBuffName = szBuffName; }
	inline const ioHashString& GetCreateBuffName() const { return m_CreateBuffName; }
	inline DWORD GetAttachAreaWeaponIndex() const { return m_dwAttachAreaWeaponIndex; }

	virtual bool IsSkillDependency( const ioHashString &szSkillName );
	virtual void SetSkillDependencyAction();

	void AttachAreaWeaponEndState();

protected:
	virtual void UpdateWorldAttackBox( float fTimePerSec, bool bSetNull );
	virtual const WeaponAttribute* GetAttribute() const;
	virtual float CalcFinalDamage();
	virtual float CalcFinalBoundDamage();
	virtual float CalcFinalNPCDamage();
	virtual float CalcFinalNPCBoundDamage();

	virtual bool CheckPiercing( bool bCollision=false );
	virtual void ClearCollisionList();

	virtual void CheckCreateWeaponList();

	void CreateAttachAreaWeapon();
	void UpdateAttachAreaWeapon( ioPlayStage *pStage );
	void DestroyAttachAreaWeapon();

	virtual void CreateCollisionAreaWeapon();

public:
	float GetWereWolfChangeDamage();
	bool IsTeamAttack();
	bool IsWeaponType(int nType);

public:
	inline int GetTargetPushStructType(){ return m_iPushStructType; }
	inline float GetTargetPushStructDamgeRate(){ return m_fPushStructDamgeRate; }	
	inline bool IsOnlyPushStructAttack() { return m_bPushStructOnlyAttack; }

public:
	virtual void SetMapCollision();

public:
	inline IgnoreGhostStateType GetIgnoreGhostState() const { return m_IgnoreGhostState; }

public:
	virtual bool IsIgnoreMapCollision() const;
	virtual bool IsIgnoreTestMapCollision() const;
	virtual bool IsIgnoreMapCollisionEffect() const;

protected:
	virtual void AddEffectEvent( ioEffect *pEffect );

public:
	void SetSceneNode( ioSceneNode *pSNode ) { m_pSceneNode = pSNode; }
	ioSceneNode* GetParentSceneNode() const { return m_pSceneNode; }

public:
	inline bool IsPush() const { return m_bSetPush; }
	inline float GetEnablePushPowerRate() const { return m_fEnablePushPowerRate; }

//방어력 관통
protected:
	float	m_fArmorPenetration;
public:
	inline void SetArmorPenetration( float fArmorPenetration ) { m_fArmorPenetration = fArmorPenetration; }
	inline const float& GetArmorPenetration(){return m_fArmorPenetration;}

//크리티컬
protected:
	float	m_fCriticalRate;
	float	m_fCriticalDamageRate;
public:
	inline void SetCriticalRate( float fCriticalRate ) { m_fCriticalRate = fCriticalRate; }
	inline const float& GetCriticalRate(){return m_fCriticalRate;}
	inline void SetCriticalDamageRate( float fCriticalDamageRate ) { m_fCriticalDamageRate = fCriticalDamageRate; }
	inline const float& GetCriticalDamageRate(){return m_fCriticalDamageRate;}

public:
	inline bool GetDestroyByCharChange() { return m_bDestroyByCharacterImmediatelyChange; }

public:
	ioWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr );
	virtual ~ioWeapon();

	DWORD GetNetSyncTime() const { return m_dwNetSyncTime;}
	void SetNetSyncTime( DWORD dwTime ){ m_dwNetSyncTime = dwTime; }

	void SetUniqID(int nID);
	int GetUniqID();
};

