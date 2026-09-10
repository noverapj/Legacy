#pragma once

#include "ioWeaponItem.h"
#include "ioAttackAttribute.h"
#include "ioSpecialStateBase.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioDruidItem : public ioWeaponItem
{
public:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_ATTACK_FIRE,
		CS_EQUIP_OBJECT,
		CS_HALF_ATTACK_FIRE,
	};

	enum SyncType
	{
		ST_ATTACK,
		ST_HALF_ATTACK,
		ST_OBJECT_EQUIP,
		ST_OBJECT_RELEASE,
	};

	enum EquipObjectState
	{
		EOS_NONE,
		EOS_NORMAL,
		EOS_DASH,
		EOS_JUMP,
	};

protected:
	ChargeState			m_ChargeState;
	EquipObjectState	m_EquipObjectState;

	DWORD				m_dwHalfChargeTime;
	AttackAttribute		m_HalfAttack;

	DWORD				m_dwAttackStartTime;
	int					m_iCurCombo;

	float				m_fNeedGauge;
	float				m_fNeedExtendGauge;
	float				m_fExtraGaugeJumpInc;

	DWORD				m_dwObjectItem;
	AttackAttribute		m_EquipAttribute;
	AttackAttribute		m_DashEquipAttribute;
	AttackAttribute		m_JumpEquipAttribute;

	//D꾹 관련
	ioHashStringVec		m_PreGatherAnimationList;
	ioHashString		m_szRleaseObjectEffect;

	float				m_fHighDecreaseHeight;
	float				m_fHighDecreaseGauge;

public:
	void Init();

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

public:
	virtual void OnReleased( ioBaseChar *pOwner );

	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );

	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual bool IsChargeAttackState( ioBaseChar *pOwner );
	virtual bool IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck );

	virtual bool SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType );
	virtual void ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap );

	virtual void SetUseExtraGauge( float fAmt );
	virtual void UpdateExtraData( ioBaseChar *pOwner );
	virtual void SetExtraGaugeTypeByWeapon( ioBaseChar *pOwner, int iType, int iWGT, float fDamage );

public:
	virtual WeaponSubType GetSubType() const;

protected:
	void OnCharging( ioBaseChar *pOwner );

	void ChangeToAttackFire( ioBaseChar *pOwner );
	void ChangeToHalfAttackFire( ioBaseChar *pOwner );
	bool ChangeToObjectState( ioBaseChar* pOwner, EquipObjectState eState );

public:
	void ChangeEquipObjectState( ioBaseChar* pOwner, EquipObjectState eState = EOS_NORMAL );

public:	
	virtual int GetMaxBullet();	
	virtual int GetCurBullet();
	virtual void SetCurBullet( int iCurBullet );
	virtual int GetNeedBullet();
	int GetNeedExtendBullet();

public:
	virtual void EquipObjectItemByAttakable( ioBaseChar* pOwner, ioItem* pObjectItem );
	virtual void ReleaseObjectItemByAttack( ioBaseChar* pOwner, ioItem* pObjectItem );

public:
	bool IsEquipObject( ioBaseChar *pOwner );	
	void ReleaseObject( ioBaseChar *pOwner, bool bNeedProcess = true );
	virtual void ReleaseObjectState( ioBaseChar* pOwner, ioObjectItem* pReleasedObject );

public:
	void FillDruidInfo( ioBaseChar *pOwner, SP2Packet &rkPacket );
	void SetDruidInfo( ioBaseChar *pOwner, SP2Packet &rkPacket );

public:
	ioDruidItem();
	ioDruidItem( const ioDruidItem &rhs );
	virtual ~ioDruidItem();
};

inline ioDruidItem* ToDruidItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_DRUID )
		return NULL;

	return dynamic_cast< ioDruidItem* >( pItem );
}


//-----------------------------------------------------------------------------------------------------------

typedef ioUserKeyInput::DirKeyInput DIRKEY;

class ioDruidEagleDummy;
class ioDruidEagleFlyState : public ioSpecialStateBase
{
friend class ioDruidItem;
public:
	enum SpecialFlyState
	{
		SFS_TARGET_FIND,
		SFS_TARGET_ATTACK_PRE,
		SFS_TARGET_ATTACK,
		SFS_UP,
		SFS_END,
	};

	enum MoveState
	{
		MS_MOVE,	//이동중
		MS_COLL,	//지형과 충돌
		MS_LANDING,	//바닥과 충돌
		MS_END,		//이동 완료
	};

	enum SyncState
	{
		SS_TARGET_FIND,
		SS_TARGET_ATTACK_PRE,
		SS_TARGET_ATTACK,
		SS_UPDATE_TARGET,
		SS_REMOVE_TARGET,
		SS_UP,
		SS_END,
		SS_CHANGE_BEAR,
		SS_CHANGE_HUMAN,
		SS_UPDOWN_KEY,
	};

	enum UpDownKey
	{
		UDK_NONE,
		UDK_UP,
		UDK_DOWN,
	};

	//특별 상태로 넘어올때 D키가 눌려져있는지 여부
	//내부에서 D키 이벤트를 한번 더 체크해야 하기 때문에 D키가 Down 상태라면
	//풀릴떄까지 내부에서 이벤트를 체크하지 않는다.
	enum AttackKeyType
	{
		AKT_NONE,	
		AKT_PRESS,
	};

	enum AttackType
	{
		AT_NONE,
		AT_NORMAL,
		AT_TRANSFORM_BEAR_ATTACK,
		AT_TRANSFORM_HUMAN_ATTACK,
	};

	struct EagleFlyInfo
	{		
		float	m_fFlyAccelSpeed;
		float	m_fFlyMinSpeed;
		float	m_fFlyMaxSpeed;
		float	m_fMaxRange;
		DWORD	m_dwFlyMinSpeedDuration;

		EagleFlyInfo()
		{			
			m_fFlyAccelSpeed		= 0.0f;
			m_fFlyMinSpeed			= 0.0f;
			m_fFlyMaxSpeed			= 0.0f;
			m_fMaxRange				= 0.0f;

			m_dwFlyMinSpeedDuration	= 0;
		}
	};

protected:
	SpecialFlyState	m_SpecialFlyState;
	AttackKeyType	m_AttackKeyType;
	AttackType		m_AttackType;
	UpDownKey		m_UpDownKey;
	DIRKEY			m_eDirKey;

protected:
	ioHashString	m_DummyCharName;
	int				m_DummyCharIndex;

protected:
	EagleFlyInfo	m_FindTargetEagleFlyInfo;
	EagleFlyInfo	m_TargetAttackEagleFlyInfo;
	EagleFlyInfo	m_TargetAttackAttackEndFlyInfo;
	EagleFlyInfo	m_UpFlyInfo;

protected:
	ioHashString	m_szReadyMarker;
	ioHashString	m_szAimedMarker;
	ioHashString	m_TargetName;

	UniqueObjID		m_ReadyMarkerID;
	UniqueObjID		m_AimedMarkerID;

	float			m_fTargetMaxRange;
	float			m_fTargetMinAngle;
	float			m_fTargetMaxAngle;
	float			m_fReadyMarkerOffset;
	float			m_fUpAngle;

	DWORD			m_dwTargetingTime;
	DWORD			m_dwTargetingStartTime;

protected:
	float			m_fStartAutoTargetRange;
	float			m_fStartAutoTargetMinAngle;
	float			m_fStartAutoTargetMaxAngle;
	float			m_fEndJumpPower;
	float			m_fChangeJumpPower;
	DWORD			m_dwChargeTime;
	DWORD			m_dwWeaponIdx;
	float			m_fCheckHeightMin;

protected:
	float			m_fCurrFlyRange;
	DWORD			m_dwStartTime;
	DWORD			m_dwCharingStartTime;
	DWORD			m_dwCurrWeaponIdx;

	D3DXVECTOR3		m_vPreDir;
	D3DXVECTOR3		m_vMoveDir;

public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

public:
	virtual void LoadProperty( ioINILoader &rkLoader );

	void LoadTargetMarker( ioINILoader &rkLoader );
	void LoadFindFly( ioINILoader &rkLoader );
	void LoadTargetFly( ioINILoader &rkLoader );
	void LoadUpFly( ioINILoader &rkLoader );

public:
	virtual const char* GetStateName(){ return "CS_DRUID_EAGLE_FLY"; }
	virtual const CharState GetState(){ return CS_DRUID_EAGLE_FLY; }
	virtual bool IsSendControlInfoToUser( ioBaseChar* pOwner ){ return false; }	

protected:
	void ChangeToFindTarget( ioBaseChar* pOwner );
	void ChangeToTargetAttackPre( ioBaseChar* pOwner, ioPlayStage* pStage, const ioHashString& szTargetName, AttackType eType, ioUserKeyInput::DirKeyInput eKey );
	void ChangeToTargetAttack( ioBaseChar* pOwner );
	void ChangeToUp( ioBaseChar* pOwner );
	void ChangeToEnd( ioBaseChar* pOwner );
	void ChangeToHumanJumpAttack( ioBaseChar* pOwner );

public:
	void ChangeToBear( ioBaseChar* pOwner );

protected:
	void OnFindTarget( ioBaseChar* pOwner );
	void UpdateTarget( ioBaseChar* pOwner );
	void ChangeTarget( ioBaseChar* pOwner, ioBaseChar* pTarget );
	void RemoveTarget( ioBaseChar* pOwner );


	void OnTargetAttackPre( ioBaseChar* pOwner );
	void OnTargetAttack( ioBaseChar* pOwner );
	void OnUp( ioBaseChar* pOwner );

protected:
	bool CheckItemGauge( ioBaseChar* pOwner );

protected:
	void CreateReadyMarker( ioBaseChar* pOwner );
	void UpdateReadyMarker( ioBaseChar* pOwner );
	void DestroyReadyMarker( ioBaseChar* pOwner );
	void CreateAimedMarker( ioBaseChar* pOwner, ioBaseChar* pTarget );
	void UpdateAimedMarker( ioBaseChar* pOwner, const ioHashString& TargetName );
	void DestroyAimedMarker( ioBaseChar* pOwner );

protected:
	void ClearWeapon( ioBaseChar* pOwner );
	bool CheckWeaponLive( ioBaseChar *pOwner );

protected:
	void CreateDummy( ioBaseChar* pOwner );
	void DestroyDummy( ioBaseChar* pOwner );
	ioDruidEagleDummy* GetDummy( ioBaseChar* pOwner );

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
	virtual bool IsStateEnd( ioBaseChar* pOwner );

protected:
	void ProcessKeyCheck( ioBaseChar *pOwner, bool bSync = true );
	int ProcessFlyMove( ioBaseChar *pOwner, const EagleFlyInfo& FlyInfo, DWORD dwStartTime );

public:
	// 중간 입장 발생시 동기화 선택
	virtual bool IsJoinSync(){ return true; }

	//중간 입장 동기화
	virtual void FillJoinSync(  SP2Packet &rkPacket, ioBaseChar* pOwner );
	virtual void ApplyJoinSync( SP2Packet &rkPacket, ioBaseChar* pOwner );

	//스테이트 내부 동기화
	virtual void ApplyStateSync( SP2Packet &rkPacket, ioBaseChar* pOwner );

public:
	ioDruidEagleFlyState();
	virtual ~ioDruidEagleFlyState();
};

inline ioDruidEagleFlyState* ToDruidEagleFlyState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_DRUID_EAGLE_FLY )
		return NULL;

	return dynamic_cast< ioDruidEagleFlyState* >( pState );
}