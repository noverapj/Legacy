#pragma once

#include "ioObjectItem.h"
#include "ioSpecialStateBase.h"

class ioWeapon;
class ioBaseChar;

class ioDruidObjectItem : public ioObjectItem
{
public:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_ATTACK_FIRE,
		CS_EXTEND_ATTACK,
		CS_GRAPPED_ATTACK,
		CS_CROUCH_GATHERING,
		CS_CROUCH_ATTACK,
	};

	enum SyncState
	{
		SS_CHARGING,
		SS_ATTACK_FIRE,
		SS_EXTEND_ATTACK,
		SS_GRAPPED_ATTACK,
		SS_CROUCH_GATHERING,
		SS_CROUCH_GATHERING_END_BY_JUMP,
		SS_CROUCH_GATHERING_END_BY_SKILL,
	};

	//그래플링 상태로 넘어올때 D키가 눌려져있는지 여부
	//내부에서 D키 이벤트를 한번 더 체크해야 하기 때문에 D키가 Down 상태라면
	//풀릴떄까지 내부에서 이벤트를 체크하지 않는다.
	enum AttackKeyType
	{
		AKT_NONE,	
		AKT_PRESS,
	};

	typedef ioUserKeyInput::DirKeyInput DIRKEY;

protected:
	ioHashString			m_EquipEffect;

protected:
	ChargeState				m_ChargeState;
	int						m_EquipObjectState;
	AttackKeyType			m_AttackKeyType;

	ioHashStringVec			m_PreAnimationList;
	
	int						m_iCurCombo;
	DWORD					m_dwAttackStartTime;

	AttackAttribute			m_ExtendAttack;
	AttackAttribute			m_GrappedAttack;
	AttackAttribute			m_CrouchAttack;

protected:
	GrapplingState			m_GrapplingState;

	ioHashString			m_GrapplingTargetName;
	ioHashString			m_GrappedTargetName;
		
	ioHashString			m_GrapplingBiteMotion;
	ioHashString			m_GrapplingLoopMotion;
	ioHashString			m_GrapplingPushMotion;
	ioHashString			m_GrapplingCancelMotion;

	float					m_fGrapplingRange;
	float					m_fGrapplingMinAngle;
	float					m_fGrapplingMaxAngle;

	float					m_GrapplingBiteMotionRate;
	float					m_fGrapplingPushMotionRate;
	float					m_fGrapplingCancelMotionRate;
		
	DWORD					m_GrapplingBiteEndTime;
	DWORD					m_dwGrapplingLoopEndTime;
	DWORD					m_dwGrapplingPushEndTime;
	DWORD					m_dwGrapplingCancelEndTime;
		
	GrapplingWoundedInfo	m_TargetGrapplingInfo;
	GrapplingPushedInfo		m_PushedGrapplingInfo;

protected:
	DWORD					m_dwGrapplingRotateSpeed;
	 DIRKEY					m_CurrDirectionKey;
	 bool					m_bTargetRot;
	 float					m_fGrapplingGapRange;

protected:
	ioHashString			m_CrouchGatherAnimation;
	ioHashString			m_CrouchGatherMoveFront;
	ioHashString			m_CrouchGatherMoveBack;
	ioHashString			m_CrouchGatherMoveRight;
	ioHashString			m_CrouchGatherMoveLeft;

	ioHashString			m_CrouchGatherEffect;
	UniqueObjID				m_CrouchGatherEffectID;
	float					m_fCrouchGatherRate;

	bool					m_bFirstFireMoveMotion;
	bool					m_bSetChargingMoveAni;
	DIRKEY					m_FireStartDirKey;

protected:
	float					m_fChangeJumpPower;

protected:
	bool					m_bReserveRelease;
	ioHashStringVec			m_EquipMaleMeshList;
	ioHashStringVec			m_EquipFemaleMeshList;

public:
	void Init();
	void EndEffect( ioBaseChar* pOwner );

	virtual void SetNoneState( ioBaseChar* pOwner, int eNewState = 0 );

public:
	virtual void LoadProperty( ioINILoader &rkLoader );

	void LoadGrappling( ioINILoader &rkLoader );
	void LoadGrapplingTarget( ioINILoader &rkLoader );
	void LoadGrapplingPushed( ioINILoader &rkLoader );
	void LoadCrouchGathering( ioINILoader &rkLoader );
	void LoadMesh( ioINILoader &rkLoader );

	virtual ioItem* Clone();

public:
	void ChangeMesh( ioBaseChar* pOwner );
	void ReleaseMesh( ioBaseChar* pOwner );

public:
	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );	
	virtual void CheckNormalAttack( ioBaseChar *pOwner );

	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );
	virtual ObjectSubType GetObjectSubType() const;

public:
	virtual void OnEquiped( ioBaseChar *pOwner );
	virtual void OnReleased( ioBaseChar *pOwner );

	virtual bool IsOneTimeWeaponItem() const;
	virtual bool IsEnableDash() const;
	virtual bool IsEnableDefense() const;

public:
	void ChangeToEquipObjectState( int eState );
	void ChangeToAttackFire( ioBaseChar *pOwner );
	void ChangeToExtendAttackFire( ioBaseChar *pOwner );
	void ChangeGrapplingLoop( ioBaseChar *pOwner );
	void ChangeGrapplingPush( ioBaseChar *pOwner );
	void ChangeGrapplingCancel( ioBaseChar *pOwner );
	void ChangeToGrappedAttackFire( ioBaseChar *pOwner );
	void ChangeToCrouchGathering( ioBaseChar *pOwner );
	void ChangeToCrouchAttackFire( ioBaseChar *pOwner );
	void ChangeToHuman( ioBaseChar *pOwner );
	void ChangeToEagle( ioBaseChar *pOwner );
	void ChangeToCrouchGatherEndByJump( ioBaseChar *pOwner );
	void ChangeToCrouchGatherEndBySkill( ioBaseChar *pOwner );

protected:
	void CheckGrapplingMoveKey( ioBaseChar *pOwner );
	void CheckGrapplingMoveDir( ioBaseChar *pOwner );

protected:
	void OnCharging( ioBaseChar *pOwner );
	void OnGrapplingBite( ioBaseChar *pOwner );
	void OnGrapplingCancel( ioBaseChar *pOwner );
	void OnGrapplingLoop( ioBaseChar *pOwner );
	void OnGrapplingPush( ioBaseChar *pOwner );

public:
	void OnCrouchGathering( ioBaseChar *pOwner );
	void OnCrouchChargingMove( ioBaseChar *pOwner );

public:
	void FindGrappling( ioBaseChar *pOwner );
	bool IsEnableGrapplingState( ioBaseChar *pOwner, const ioHashString &szTarget );
	void SetGrapplingState( ioBaseChar *pOwner, const ioHashString &szTarget );
	void GetGrapplingDirection( OUT int &CurDirection );
	const GrapplingWoundedInfo& GetTargetGrapplingInfo() const;
	const GrapplingPushedInfo& GetPushedGrapplingInfo() const;
	float GetGrapplingGapRange() const;
	virtual void SetEndGrappling( ioBaseChar *pOwner );

public:
	bool CheckGrapplingCancel( ioBaseChar *pOwner );
	void CheckGrapplingState( ioBaseChar *pOwner );

public:
	bool IsReleaseItem( const ioBaseChar* pOwner ) const;
	virtual bool IsCanFire( const ioBaseChar* pOwner, int iFireCnt = 0, bool bNormalAttack = false ) const;
	virtual bool IsChargeAttackState( ioBaseChar *pOwner );
	virtual bool IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck );

public:
	virtual bool CheckReleaseProcess( ioPlayStage *pStage, ioBaseChar *pOwner );
	virtual bool CheckReleaseByNormalAttack( ioPlayStage *pStage, ioBaseChar *pOwner );
	bool CheckReleaseBear( ioPlayStage *pStage, ioBaseChar *pOwner );

public:
	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket );

public:
	ioDruidObjectItem();
	ioDruidObjectItem( const ioDruidObjectItem &rhs );
	virtual ~ioDruidObjectItem();
};

inline ioDruidObjectItem* ToDruidObjectItem( ioItem *pItem )
{
	ioObjectItem *pObject = ToObjectItem( pItem );
	if( !pObject || pObject->GetObjectSubType() != ioObjectItem::OST_DRUID )
		return NULL;

	return dynamic_cast< ioDruidObjectItem* >( pObject );
}


inline const ioDruidObjectItem* ToDruidObjectItemConst( const ioItem *pItem )
{
	const ioObjectItem *pObject = ToObjectItemConst( pItem );
	if( !pObject || pObject->GetObjectSubType() != ioObjectItem::OST_DRUID )
		return NULL;

	return dynamic_cast< const ioDruidObjectItem* >( pObject );
}

//-----------------------------------------------------------------------------------------------------------

class ioDruidCrouchGatheringState : public ioSpecialStateBase
{
public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_DRUID_CROUCH_GATHERING_STATE"; }
	virtual const CharState GetState(){ return CS_DRUID_CROUCH_GATHERING_STATE; }
	virtual void CheckSpecialState( ioBaseChar* pOwner );
	virtual void ProcessKeyInput( ioBaseChar* pOwner );

public:
	ioDruidCrouchGatheringState();
	virtual ~ioDruidCrouchGatheringState();
};

inline ioDruidCrouchGatheringState* ToDruidCrouchGatheringState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_DRUID_CROUCH_GATHERING_STATE )
		return NULL;

	return dynamic_cast< ioDruidCrouchGatheringState* >( pState );
}