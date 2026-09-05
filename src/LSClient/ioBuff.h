#pragma once

#include "BuffTypeID.h"
#include "ioAttackAttribute.h"
#include "ioSkill.h"

class ioINILoader;
class ioBaseChar;
class ioGrowthUpInfo;
class SP2Packet;
class ioItem;
class ioEffect;

typedef std::list<const ioBuff* const> ConstBuffList;

struct CallAreaWeapon
{
	ioHashString m_AreaWeaponName;
	DWORD m_dwAreaWeaponCallTime;

	CallAreaWeapon()
	{
		m_dwAreaWeaponCallTime = 0;
	}
};

typedef std::vector< CallAreaWeapon > CallAreaWeaponList;

struct CallAttribute
{
	WeaponInfo m_WeaponInfo;
	DWORD m_dwAttributeCallTime;
	float m_fAttributeCallAngle;
	float m_fAttributeCallRange;
	int   m_iRandomMinTime;
	int   m_iRandomMaxTime;
	DWORD m_dwRandomTime;

	CallAttribute()
	{
		m_dwAttributeCallTime = 0;
		m_iRandomMinTime = -1;
		m_iRandomMaxTime = -1;
		m_dwRandomTime = 0;
	}
};

typedef std::vector< CallAttribute > CallAttributeList;

class CallAttributeSort : std::binary_function< const CallAttribute&, const CallAttribute&, bool >
{
public:
	bool operator()( const CallAttribute &lhs , const CallAttribute &rhs ) const
	{
		if( lhs.m_dwAttributeCallTime < rhs.m_dwAttributeCallTime )
		{
			return true;
		}

		return false;
	}
};

struct CallBuff
{
	ioHashString m_szBuffName;
	DWORD m_dwBuffCallTime;

	CallBuff()
	{
		m_dwBuffCallTime = 0;
	}
};

typedef std::vector< CallBuff > CallBuffList;

class CallBuffSort : std::binary_function< const CallBuff&, const CallBuff&, bool >
{
public:
	bool operator()( const CallBuff &lhs , const CallBuff &rhs ) const
	{
		if( lhs.m_dwBuffCallTime < rhs.m_dwBuffCallTime )
		{
			return true;
		}
		else if( lhs.m_dwBuffCallTime == rhs.m_dwBuffCallTime )
		{
			int iCmpValue = stricmp( lhs.m_szBuffName.c_str(), rhs.m_szBuffName.c_str() );     
			if( iCmpValue < 0 )
				return true;
		}
		return false;
	}
};

class ioBuff
{
public:
	enum OperationType
	{
		OT_PASSIVE,
		OT_ACTIVE,
		OT_SWITCH,
		OT_DELAY,
		OT_UNLIMITED_PASSIVE,		// 제한없는 패시브 (ex: 티르의 갑옷)
	};

	enum CreatorLookType
	{
		CLT_NONE,
		CLT_CREATOR,
		CLT_CREATOR_REVERSE,
	};

	enum GoodBadType
	{
		GBT_NONE,
		GBT_BAD,
		GBT_GOOD,
	};

private:
	// For Active
	CEncrypt<DWORD> m_dwBuffDuration;          //유저의 버프 시간
	bool			m_bBuffDuration;		   //유저의 버프 시간 체크 2020-08-18
	CEncrypt<DWORD> m_dwBuffDurationByNpc;     //몬스터의 버프 시간 - ex)개발자K 제외
	CEncrypt<DWORD> m_dwBuffDurationByBossNpc; //보스 몬스터의 버프 시간 - ex)개발자K 제외
	CEncrypt<DWORD> m_dwNPCModeBuffDuration;          //유저의 버프 시간

	CEncrypt<DWORD> m_dwFCModeBuffDuration;          // 파이트 클럽 버프 시간
	CEncrypt<DWORD> m_dwFCModeTickTime;

	CEncrypt<DWORD> m_dwTDModeTickTime;			// MonsterSurvival, TowerDefense buff time

	CEncrypt<DWORD> m_dwTrainingModeBuffDuration;          // 광장 버프 시간

	CEncrypt<BOOL>	m_bCallErased;
	CEncrypt<BOOL>  m_bNoEndEffect;

	//Accessory
	CEncrypt<int>  m_iBuffCategory;

protected:
	ioHashString m_GroupName;
	ioHashString m_TitleName;

	ioHashString m_Name;
	ioHashString m_ItemName;
	ioHashString m_SkillName;
	ioHashString m_EndEmoticon;

	ioBaseChar *m_pOwner;
	//ioItem *m_pOwnerItem;

	// AreaWeapon 아군 이브(자신 제외)가 생성한 다른 에어리어웨폰에 영향X 2020-11-02
	bool m_bIsDuplicateBuff;
	bool m_bChangeWeaponDirToTargetRot;
	ioHashString m_OwnerName;		// buff가 걸린 캐릭터
	ioHashString m_CreateChar;		// buff를 생성한 캐릭터
	DWORD        m_dwCreateWeaponItemCode;
	DWORD m_dwCreateWeaponIndex;
	DWORD m_dwCreateAreawaponIndex;

	// For desc
	ioHashString m_DescName;
	ioHashString m_Desc;
	ioHashString m_DescIcon;
	bool         m_bDescEqualDuration; // true : buff와 emoticon 표시 시간이 같다.

	// For Delay
	bool m_bBuffStart;
	DWORD m_dwDelayDuration;

	// For Switch
	CEncrypt<float> m_fGaugePerTick;	
	CEncrypt<DWORD> m_dwTickTime;
	CEncrypt<DWORD> m_dwCheckTime;

	CEncrypt<DWORD> m_dwBuffStartTime;
	CEncrypt<DWORD> m_dwBuffEndTime;

	//UJ 130829, 최적화: HasBuff 조회가 매우 느리기 때문에 CEncrypt 쓰지않는다, 
	// 어차피 이값은 readonly라서 변조보호의 의미가 크지 않음, 이 값을 개개별로 메모리탐색하느니 ini 위조등이 빠르다
	//CEncrypt<int> m_iBuffType;
	int m_iBuffType;
	int m_iOperationType;
	int m_iCameraType;

	// Exception List
	ioHashStringVec m_KillingList;
	ioHashStringVec m_SuicideList;

	// Remove Type List
	IntVec m_RemoveByTypeList;

	// For Calling Buff
	CallBuffList m_CallBuffList;

	ioHashString m_ExceptTargetBuff;
	ioHashStringVec m_ExceptCallBuffList;

	bool m_bPassEndBuffCall;
	bool m_bCallExistAttacker;
	CallAttributeList m_CallAttributeList;

	IntVec m_vWeaponIndexList;

	CEncrypt<float> m_fCallAttributeEnableHeight;

	CEncrypt<bool> m_bUseCallAttWeaponCol;

	// For Calling AreaWeapon
	CallAreaWeapon m_CallAreaWeapon;
	DWORD m_dwAreaWeaponIndex;

	// Start Other Buff
	ioHashString m_OtherBuff;
	// For BuffEnd Buff
	ioHashString m_BuffEndBuff;
	ioHashString m_BuffEndBuff2;

	// For Invisible Weapon
	bool m_bInvisibleWeapon;
	bool m_bInvisibleCloak;
	bool m_bInvisibleHelmet;
	bool m_bInvisibleHead;

	bool m_bInvisibleState;

	DWORD m_dwInvisibleWeaponID;
	DWORD m_dwInvisibleHelmetID;
	DWORD m_dwInvisibleCloakID;

	DWORD m_dwStopPassiveSkillID;

	// For Growth
	bool m_bDisableGrowth;
	CEncrypt<float> m_fTicGaugeRateByGrowth;
	CEncrypt<DWORD> m_dwTicTimeByGrowth;
	DWORD m_dwExtraDurationByGrowth;

	// For ChangeMaterial
	bool m_bChangeColor;
	int m_iChangeColorIndex;
	D3DCOLORVALUE m_ChangeColor;

	ioHashString m_BuffEffect;
	CEncrypt<bool> m_bVisibleBuffEffectOnlyOwner;
	CEncrypt<bool> m_bVisibleBuffEffectIndependence;

	ioHashString m_BuffEndEffect;
	ioHashString m_LoopSound;
	UniqueObjID m_LoopSoundID;

	ioHashString m_PassiveBuffEffect;
	DWORD m_dwPassiveBuffIdleTime;
	DWORD m_dwPassiveBuffEffectStartTime;

	// for accumlate speed
	CEncrypt<float> m_fCurAccumSpeedRate;
	CEncrypt<float> m_fMinAccumSpeedRate;
	CEncrypt<float> m_fMaxAccumSpeedRate;

	//같은 종류의 버프가 중복 될 수 있게하는 옵션(윤태철)
	bool	m_bCurBuffAddEnable;

protected:	//버프속성
	bool m_bEnableDelayState;
	bool m_bDisableChange;
	bool m_bBadStateBuff;
	bool m_bDisableBoss;
	CEncrypt<bool> m_bEnableWoundCancel;
	CEncrypt<bool> m_bIgnoreDefenseBreakBuff;
	CEncrypt<bool> m_bIgnoreProtectBuff;
	CEncrypt<bool> m_bEnablePossession;
	
	bool m_bEnableOverlapCnt;
	bool m_bOverlapCreatorCompare;

	GoodBadType m_GoodBadType;

protected:	//버프상태
	CEncrypt<bool> m_bLive;
	CEncrypt<bool> m_bPassive;
	CEncrypt<bool> m_bPassiveSkillGauageReduce;
	CEncrypt<bool> m_bTeamAttack;
	CEncrypt<bool> m_bCreatorLook;
	CEncrypt<bool> m_bLoopSoundPlayed;
	//UJ 130829, 최적화: 가뜩이나 collision 느리기 때문에 CEncrypt 쓰지않는다, 
	// 어차피 이값은 readonly라서 변조보호의 의미가 크지 않음, 이 값을 개개별로 메모리탐색하느니 ini 위조등이 빠르다
	bool m_bCharCollisionSkip;
	CEncrypt<bool> m_bCancelBuff;
	CEncrypt<bool> m_bNotTargetNpc;		//NPC 대상으로 버프를 걸 수 있는지 여부

	CreatorLookType m_CreatorLookType;

protected:
	bool m_bEnableSkillState;

	//윤태철 ( SetState( CS_DELAY ) 에서 위치 정보를 네트웍으로 보낼지 값-> 해당 구현은 각자
protected:
	bool m_bSendDelayNetWork;

protected:	//액티브카운트
	CEncrypt<int>	m_iMaxActiveCount;
	int				m_iCurActiveCount;

protected:
	bool	m_bRemoveEquipSkillBuff;

	CEncrypt<bool> m_bReduceAllGaugeOnBuffEnd;

	bool m_bEnableEquipItemGaugeUpdate;

protected:
	bool m_bPetForceHide;
	bool m_bWaitSelfRemove;
	bool m_bGetupStateForceEnd;
	DWORD m_dwReserveGetupEndTime;
	bool m_bEndAfterAction;
	bool m_bPassiveActivated;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

	void SetGroupName( const ioHashString& szGroupName );
	void SetTitleName( const ioHashString& szTitleName );

	void ReLoadProperty();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual void StartBuffWithAniTime( ioBaseChar *pOwner, DWORD dwAniTime );
	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();
	virtual void CancelBuff();

	virtual void GetBuffInfo( SP2Packet &rkPacket );
	virtual void ApplyBuffInfo( SP2Packet &rkPacket );

	virtual void AccumulateBuff();

	// 버프는 살아있지만 다른 이유에 의해 버프효과가 잠시 정지된 상태
	virtual bool IsLiveButStopped() const;
	
	virtual bool IsCanJumpAttack() const;

	bool IsEanableCurBuffAdd();

protected:
	virtual void SetAction();
	virtual void ProcessDelay( float fTimePerSec );

protected:
	void  SetBuffDuration( DWORD dwBuffDuration );
	DWORD GetBuffDuration();

public:
	void  SetBuffDurationByRate( float fRate );
	void  SetBuffDurationAndEndTime( DWORD dwBuffDuration );

public:
	void SetTypeID( int iType );

	void SetOwnerItemName( const ioHashString &szName );
	void SetOwnerSkill( ioSkill *pSkill );

	bool SetPassiveBuff( bool bPassive );

	bool IsSwitchType();
	bool IsPassiveType();
	bool IsUnlimitedPassiveType();			// 제한없는 패시브인지 확인 (ex: 티르의 갑옷)

	CharBuffInfo GetBuffInfoByChangeChar();

	// For Growth
	void SetTicTimeByGrowth( float fTicTime );
	void SetTicGaugeRateByGrowth( float fRate );
	virtual void SetExtraDurationByGrowth( float fDuration );

	ioGrowthUpInfo* GetGrowthUpInfoForItem( int iType );

	void SetReserveEndBuff();

	void ThisCallAndErase( bool b );

	void PlayPassiveBuffEffect();

protected:
	bool CheckEnableDelayState();
	void CheckCallingBuff();
	void CheckExceptCallingBuff();
	void CheckCallingAttribute();
	void CallingAttribute();
	void CheckCallingAreaWeapon();

	void CheckCallingProcess();

	void CheckInvisibleWeapon( bool bVisible );
	void CheckInvisibleCloak( bool bVisible );
	void CheckInvisibleHelmet( bool bVisible );

	void CheckInvisibleState( bool bVisible );

	void CheckChangeSkill( bool bChange );

	void CheckBuffDesc();
	void CheckChangeColor( bool bChange );

	void StartOtherBuff();
	void EndOtherBuff();
	void CheckBuffEndBuff();
	void CheckBuffEndBuff2();
	void CheckCallWeaponEnd();

	void LoadBuffExceptList( ioINILoader &rkLoader );
	void LoadCallBuffList( ioINILoader &rkLoader );
	void LoadCallAttributeList( ioINILoader &rkLoader );
	void LoadCallAreaWeapon( ioINILoader &rkLoader );

	void DestroyAreaWeapon();

	bool IsRightOwnerItem();

	bool CheckIgnoreProtectCallingBuff( const ioHashString &szBuffName );

	bool CheckSwitchType( float fTimePerSec );
	float GetCurTicGauge();
	int GetCurTicGap();
	int GetCurTicTime();

	void CalcRandAttributeTime( ioBaseChar *pOwner );

public:
	inline void SetTeamAttack( bool bTeamAttack ) { m_bTeamAttack = bTeamAttack; }
	inline int GetTypeID() const { return m_iBuffType; }
	inline const ioHashStringVec& GetExceptKillingList() { return m_KillingList; }
	inline const ioHashStringVec& GetExceptSuicideList() { return m_SuicideList; }
	inline const IntVec& GetRemoveByTypeList() { return m_RemoveByTypeList; }

	inline bool IsLive() const { return m_bLive; }
	inline bool IsBuffDuration()	const {	return m_bBuffDuration;	}	// 2020-08-19
	inline bool IsPassiveBuff() const { return m_bPassive; }
	inline bool isPassiveSkillGauageReduce() const { return m_bPassiveSkillGauageReduce; }
	inline bool IsDisableChange() const { return m_bDisableChange; }
	inline bool IsBadStateBuff() const { return m_bBadStateBuff; }
	inline bool IsDisableBossBuff() const { return m_bDisableBoss; }

	inline bool IsInvisibleWeapon() const { return m_bInvisibleWeapon; }

	inline bool IsIgnoreDefenseBreakBuff() const { return m_bIgnoreDefenseBreakBuff; }
	inline bool IsIgnoreProtectBuff() const { return m_bIgnoreProtectBuff; }

	inline const ioHashString& GetName() const { return m_Name; }
	inline const ioHashString& GetSkillName() const { return m_SkillName; }
	inline const ioHashString& GetCreatorName() const { return m_CreateChar; }

	inline bool IsEnableSkillState() const { return m_bEnableSkillState; }
	inline bool IsEnableOverlap() const { return m_bEnableOverlapCnt; }
	inline bool IsOverlapCreatorCompare() const { return m_bOverlapCreatorCompare; }
	inline bool IsEnableWoundCancel() const { return m_bEnableWoundCancel; }
	inline bool IsEnablePossession() const { return m_bEnablePossession; }

	inline float GetSpeedAccumRate() const { return m_fCurAccumSpeedRate; }
	inline GoodBadType GetGoodBadType() const { return m_GoodBadType; }
	inline bool GetInvisibleState(){ return m_bInvisibleState; }
	inline bool GetInvisibleWeapon(){ return m_bInvisibleWeapon; }

	bool HasAreaWeapon() const;

	void SetCreator( const ioHashString &szCreator );
	void SetItemCode( DWORD dwCreateWeaponItemCode );
	virtual void SetWeaponIndex( DWORD dwCreateWeaponIndex );
	void SetAreaWeaponIndex( DWORD dwCreateAreaweaponIndex );

	virtual bool OnReduceGauge() const;

	virtual bool IsCharCollisionSkip() const;
	virtual bool IsCollisionAvailableWeapon() const;
	virtual bool IsProtected() const;
	virtual bool IsRemoveEnable()	{ return true; }

public:
	virtual bool UseActiveCount() const;
	virtual void CheckActiveCount();
	inline const int GetMaxActiveCount() { return m_iMaxActiveCount; }
	inline const int GetCurActiveCount() { return m_iCurActiveCount; }

	void ResetCurActiveCount() { m_iCurActiveCount = 0; }

	inline int IsEnableEquipItemGaugeUpdate() const { return m_bEnableEquipItemGaugeUpdate; }
	
public:
	virtual void CheckRemoveEquipSkillBuff( bool bRemoveEquipSkillBuff );

public:
	virtual void ApplyExtraBuffInfo( SP2Packet &rkPacket );

protected:
	ioBaseChar *GetEffectEventOwner();

public:
	ioBaseChar* GetOwner();
	ioBaseChar* GetCreator();
	ioItem* GetOwnerItem();

	//Accessory
	void SetBuffCategory( int iCategory );

public:
	bool HasExceptSuicideBuff( ioBaseChar* pChar ) const;

	// ioBuff 아군 이브(자신 제외)가 생성한 다른 에어리어웨폰에 영향X 2020-11-02
	bool IsDuplicateBuff( ioBuff *pBuff = NULL );

public:
	void SetPassiveActivated( bool bActivate )	{ m_bPassiveActivated = bActivate; }
	bool GetPassiveActivated()	{ return m_bPassiveActivated; }

public:
	inline int GetBuffStartTime(){ return m_dwBuffStartTime; }
	inline bool IsNotTargetNPC(){ return m_bNotTargetNpc; }

public:
	virtual bool SetBuffGrapplingState( ioBaseChar *pChar, const ioHashString &szTargetName );
	virtual D3DXVECTOR3 GetGrapplingTargetPos( ioBaseChar *pChar );
	virtual bool IsWaitSelfRemove() { return m_bWaitSelfRemove; }

public:
	ioBuff();
	ioBuff( const ioBuff &rhs );
	virtual ~ioBuff();
};
