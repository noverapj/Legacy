#pragma once

#include "ioWeaponItem.h"
#include "ioAttackAttribute.h"
#include "ioSpecialStateBase.h"
#include "ioSoldierStateCacheManager.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioResearcher : public ioWeaponItem
{
public:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_CHARGING_ATTACK,
		CS_ATTACK_FIRE,
		CS_EXTEND_FIRE,
		CS_OVERHEAT_FIRE,
		CS_GATHERING,
	};

	enum GatheringState
	{
		GS_FIRE,
		GS_OVERHEAT,
	};
	
	enum UpDownState
	{
		UDS_UP,
		UDS_DOWN,
	};

	enum MoveState
	{
		MS_MOVE,	//이동중
		MS_LANDING,	//이동중 지형과 충돌
		MS_END,		//이동 완료
	};

	enum RearState
	{
		RS_NONE,
		RS_DASH,
		RS_JUMP,
		RS_OVER_HEAT_ATTACK,
	};

	enum SyncState
	{
		SS_ATTACK_FIRE,
		SS_CHARGING_ATTACK_FIRE,
		SS_CHARGING_ATTACK_CANCEL,
		SS_GATHERING,
		SS_GATHERING_DIR,
		SS_CHARGE_EFFECT,
		SS_MAX_EFFECT,		
		SS_EXTEND_ATTACK,
		SS_CHANGE_REAR_FLY,
		SS_OVER_HEAT_END,
	};	

	struct ExtendAttackInfo
	{
		float			m_fGaugeMin;
		float			m_fGaugeMax;
		float			m_fAddGauge;

		ioHashString	m_Effect;

		AttackAttribute	m_ExtendAttack;
	};
	typedef std::vector<ExtendAttackInfo> ExtendAttackInfoList;
	typedef ioUserKeyInput::DirKeyInput DirKey;

protected:
	ChargeState					m_ChargeState;
	GatheringState				m_GatheringState;
	RearState					m_RearState;

	DWORD						m_dwAttackStartTime;
	int							m_iCurCombo;
	CEncrypt<float>				m_fNeedGauge;

protected:
	ioHashString				m_PreGatherAnimation;	
	ioHashString				m_GatherEffect;
	ioHashString				m_GatherMaxEffect;
	ioHashString				m_GuideEffect;
	ioHashString				m_OverHeatEffect;
	UniqueObjID					m_GatherEffectID;
	UniqueObjID					m_GatherMaxEffectID;
	UniqueObjID					m_GuideEffectID;
	UniqueObjID					m_OverHeatEffectID;

protected:
	AttackAttribute				m_ChargingAttackAttribute;
	DWORD						m_dwChargingAttackEndTime;

protected:
	ExtendAttackInfoList		m_ExtendAttackInfoList;
	AttackAttribute				m_OverHeatAttackAttribute;
	float						m_fOverHeatChekHeight;

protected:
	ioHashString				m_UpGatherAnimation;
	ioHashString				m_CenterGatherAnimation;
	ioHashString				m_DownGatherAnimation;

	ioHashString				m_UpExtendAttackAnimation;
	ioHashString				m_DownExtendAttackAnimation;

	float						m_fCurrMotionRate;
	float						m_fVertUpLimitAngle;
	float						m_fVertDownLimitAngle;

	DWORD						m_dwHorzRotateWeight;
	DWORD						m_dwVertRotateWeight;
	
	DWORD						m_dwExtendAttackEndTime;
	DWORD						m_dwOverHeatAttackEndTime;

	DWORD						m_dwCurrExtendAttackIndex;

	D3DXVECTOR3					m_vAttackDir;
	ioUserKeyInput::DirKeyInput	m_CurDirKey;

protected:
	ioHashStringVec				m_OverHeatMaleMeshList;
	ioHashStringVec				m_OverHeatFemaleMeshList;
	ioHashStringVec				m_OverHeatBuffList;

protected:
	ValkyrieFlyInfo				m_ChangeToDashRearFlyInfo;
	ValkyrieFlyInfo				m_ChangeToJumpRearFlyInfo;

	float						m_fRearDashGauge;
	float						m_fRearJumpGauge;

	DWORD						m_dwCurrWeaponIdx;
	DWORD						m_dwUpStartTime;
	float						m_fUpCurrRange;

protected:
	bool						m_bSkillUseItemMesh;
	bool						m_bBuffSetInvisible;
	bool						m_bObjectUseItemMesh;
	bool						m_bSpeicalMotionMesh;

public:
	void Init();
	virtual void SetNoneState( ioBaseChar* pOwner, int eNewState = 0 );

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

	void LoadMeshProperty( ioINILoader &rkLoader );
	void LoadBuffProperty( ioINILoader &rkLoader );
	void LoadFlyInfo( ioINILoader &rkLoader, const char* szName, ValkyrieFlyInfo& DestInfo );

public:
	virtual void OnEquiped( ioBaseChar *pOwner );
	virtual void OnReleased( ioBaseChar *pOwner );

public:
	void SetAttack( IN ioBaseChar* pOwner, IN const AttackAttribute* pAttribute, IN bool bExtra, IN bool bAutoTarget, OUT DWORD& dwAttackEnd, OUT DWORD& dwReserve, OUT DWORD& dwWeaponIndexBase );
	void SetAttackAttribute( ioBaseChar *pOwner, const AttackAttribute& Attr );

public:
	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );

public:
	ioResearcher::ChargeState GetChargetState(){ return m_ChargeState; }

public:
	void EndGatherEffect( ioBaseChar *pOwner );
	void EndGuideEffect( ioBaseChar *pOwner );
	void EndOverHeatEffect( ioBaseChar *pOwner );

protected:
	void OnNormalAttack( ioBaseChar *pOwner );
	void OnCharging( ioBaseChar *pOwner );
	void OnChargingAttack( ioBaseChar *pOwner );
	void OnExtendAttack( ioBaseChar *pOwner );
	void OnOverHeatAttack( ioBaseChar *pOwner );
	void OnGathering( ioBaseChar *pOwner );
	void CheckMaxEffect( ioBaseChar *pOwner );
	void CheckChargeEffect( ioBaseChar* pOwner );
	bool OnOverHeatCheck( ioBaseChar* pOwner );

protected:
	bool IsLeftRotate( ioBaseChar* pOwner, DirKey eNewDirKey );
	bool IsLimitAngle( float fLimitAngle, float fCurrAngle );
	
	bool CheckDirectionKey( ioBaseChar* pOwner );

	bool ProcessRotateHorz( ioBaseChar *pOwner, float fRotateTime, DirKey eDir, bool bLeft );
	bool ProcessRotateVert( ioBaseChar *pOwner, float fRotateTime, UpDownState eState );
	void ProcessRotateAnimation( ioBaseChar *pOwner );

protected:
	void ChangeToAttackFire( ioBaseChar *pOwner );
	void ChangeToChargingAttackFire( ioBaseChar *pOwner );
	void ChangeToGathering( ioBaseChar *pOwner );	
	void ChangeToExtendAttackFire( ioBaseChar *pOwner );
	void ChangeToExtendAttackFireAnimation( IN ioBaseChar* pOwner, IN const AttackAttribute* pAttribute );
	void ChangeToOverHeatState( ioBaseChar *pOwner );
	void ChangeToOverHeatEndState( ioBaseChar *pOwner );

public:
	void ChangeToRearFlySpecialState( ioBaseChar *pOwner, RearState eState );

public:
	bool IsOverHeatFire(){ return m_ChargeState == CS_OVERHEAT_FIRE; }
	bool IsOverHeat(){ return m_GatheringState > GS_FIRE; }	

	void AttachOverHeatEffect( ioBaseChar* pOwner );
	void ChangeMesh( ioBaseChar* pOwner );	
	void ReleaseMesh( ioBaseChar* pOwner );

	void OnOverHeatAddBuff( ioBaseChar* pOwner );
	void OnOverHeatRemoveBuff( ioBaseChar* pOwner );

protected:
	void SaveGatheringState( ioBaseChar* pOwner );
	void LoadGatheringState( ioBaseChar* pOwner );

public:
	void CheckCurrItemtemMeshProcess( ioBaseChar* pOwner );

public:
	void CheckSpecialMotion( ioBaseChar* pOwner );
	void CheckSkillInvisibleWeapon( ioBaseChar* pOwner );
	void CheckEquipOject( ioBaseChar* pOwner );
	void CheckInvisibleWeaponBuff( ioBaseChar* pOwner );

public:
	void ClearSpecialState( ioBaseChar* pOwner );
	void ClearWeapon( ioBaseChar* pOwner );

	void ProcessRearFlySpecialState( ioBaseChar *pOwner, ioPlayStage* pStage );
	ioResearcher::MoveState ProcessMove( ioBaseChar *pOwner, const ValkyrieFlyInfo& rkFlyInfo, const D3DXVECTOR3& vFindDir, DWORD& dwStartTime, float& fCurrRange, bool bCheckLanding );

public:
	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );	
	void FillInfo( ioBaseChar *pOwner, SP2Packet &rkPacket );
	void SetInfo( ioBaseChar *pOwner, SP2Packet &rkPacket );

public:
	virtual bool IsChargeAttackState( ioBaseChar *pOwner );
	virtual bool IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck );
	virtual bool IsEnableJumpKeyEtcAction( ioBaseChar *pOwner );

	virtual void SetUseExtraGauge( float fAmt );
	virtual void UpdateExtraData( ioBaseChar *pOwner );	
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );

public:
	virtual WeaponSubType GetSubType() const;

public:	
	virtual int GetMaxBullet();	
	virtual int GetCurBullet();
	virtual void SetCurBullet( int iCurBullet );
	virtual int GetNeedBullet();

public:
	ioResearcher();
	ioResearcher( const ioResearcher &rhs );
	virtual ~ioResearcher();
};

inline ioResearcher* ToResearcher( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_RESEARCHER )
		return NULL;

	return dynamic_cast< ioResearcher* >( pItem );
}

//-----------------------------------------------------------------------------------------------------------

class ioResearcherRearFlySpecialState : public ioSpecialStateBase
{
public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );
	//virtual bool IsNoDropState( ioBaseChar* pOwner );

	virtual const char* GetStateName(){ return "CS_RESEARCHER_REAR_FLY"; }
	virtual const CharState GetState(){ return CS_RESEARCHER_REAR_FLY; }	
	virtual bool IsSendControlInfoToUser( ioBaseChar* pOwner ){ return false; }	

public:
	virtual bool IsCharCollisionSkipState( const ioBaseChar *const pOwner,
		DWORD dwStartTime,
		DWORD dwEndTime,
		DWORD dwSkipType,
		bool bTeamOnly,
		bool bDefense,
		TeamType eOwnerTeam,
		TeamType eOtherTeam ) const;

	virtual void CheckSpecialState( ioBaseChar* pOwner );
	virtual void CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight );
	virtual bool ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime );

public:
	ioResearcherRearFlySpecialState();
	virtual ~ioResearcherRearFlySpecialState();
};

inline ioResearcherRearFlySpecialState* ToResearcherRearFlySpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_RESEARCHER_REAR_FLY )
		return NULL;

	return dynamic_cast< ioResearcherRearFlySpecialState* >( pState );
}

//-----------------------------------------------------------------------------------------------------------

class ioResearcherCache : public SoldierCache
{
public:
	ioResearcher::GatheringState m_GatheringState;

public:
	virtual SoldierCache::CacheType GetType(){ return CT_RESSEARCHER; }

public:
	ioResearcherCache()
	{
		m_GatheringState = ioResearcher::GS_FIRE;
	};
	ioResearcherCache( ioHashString szOwner ) : SoldierCache( szOwner )
	{
		m_GatheringState = ioResearcher::GS_FIRE;
	}
	virtual ~ioResearcherCache()
	{
	}
};

inline ioResearcherCache* ToResearcherCache( SoldierCache *pCache )
{
	if( !pCache || pCache->GetType() != SoldierCache::CT_RESSEARCHER )
		return NULL;

	return dynamic_cast< ioResearcherCache* >( pCache );
}
