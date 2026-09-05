#pragma once

#include "ioWeaponItem.h"
#include "ioAttackAttribute.h"
#include "ioSpecialStateBase.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioJumperItem : public ioWeaponItem
{
public:
	enum AttackState
	{
		AS_NONE,
		AS_CHARGING,		
		AS_ATTACK_FIRE,
		AS_EXTEND_ATTACK,
		AS_GRAPPING_PRE,		
		AS_GRAPPING_MOVE,
		AS_CANCEL_TELEPORT_PRE,
		AS_CANCEL_TELEPORT,
		AS_TELEPORT,
		AS_TELEPORT_END,
	};

	enum MoveState
	{
		MS_MOVE,	//이동중
		MS_LANDING,	//이동중 지형과 충돌
		MS_END,		//이동 완료
	};

	enum ExtendType
	{
		ET_NORMAL,
		ET_DASH,
		ET_JUMP,
	};

protected:
	AttackState					m_AttackState;
		
	DWORD						m_dwAttackStartTime;	
	int							m_iCurCombo;
	int							m_iCurReserveCombo;
	CEncrypt<float>				m_fNeedGauge;
	CEncrypt<float>				m_fNeedCancelGauge;

	ioHashStringVec				m_PreAnimationList;

protected:	
	AttackAttribute				m_ExtendAttackAttribute;
	AttackAttribute				m_ExtendJumpAttackAttribute;
	DWORD						m_dwExtendAttackEndTime;

	float						m_fExtendAttackNextJumpAmt;

protected:
	ioHashString				m_GrapperPreAnimation;
	float						m_GrapperPreAnimationRate;
	DWORD						m_dwGrapperPreEndTime;

protected:
	ioHashString				m_GrapperMoveAnimation;
	float						m_GrapperMoveAnimationRate;

protected:
	ioHashString				m_GrapplingTargetName;
	GrapplingSkillWoundedInfo	m_GrapplingWounded;

	float						m_fGrapplingOffsetGapRange;
	float						m_fGrapplingOffsetHeightRate;

	float						m_fGrapAfterMoveSpeed;
	float						m_fCurrGrapAfterMove;

protected:
	float						m_fInPortalOffset;
	float						m_fOutPortalOffset;
	float						m_fInPortalHeightOffset;
	float						m_fOutPortalHeightOffset;
	float						m_fCancelDashTeleport;

	ioHashString				m_szInPortalMapEffect;
	ioHashString				m_szOutPortalMapEffect;
	UniqueObjID					m_InPortalID;
	UniqueObjID					m_OutPortalID;

	D3DXVECTOR3					m_vTeleportPos;

protected:
	ioHashString				m_CancelDashTeleportStartAnimation;
	float						m_CancelDashTeleportStartAnimationRate;
	ioHashString				m_CancelDashTeleportEndAnimation;
	float						m_CancelDashTeleportEndAnimationRate;

	DWORD						m_dwCancelDashTeleportStartEndTime;
	DWORD						m_dwCancelDashTeleportEndEndTime;
	int							m_eCurrCallerType;
	ioUserKeyInput::DirKeyInput m_eCurrKey;

protected:
	AttackAttribute				m_TeleportEndAttribute;
	float						m_fTeleportAttackNextJumpAmt;
	DWORD						m_dwTeleportEndTime;

protected:
	float						m_fCancelDashEndNextJumpAmt;
	float						m_fCancelDashEndJumpJudgment;

public:
	void Init();

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

	void LoadGrapplingTargetProperty( IN ioINILoader& rkLoader, OUT GrapplingSkillWoundedInfo& rkInfo );
	void ClearSpecial( ioBaseChar* pOwner, CharState eNewState );
	void DestroyMapEffect( ioPlayStage* pStage, ioBaseChar* pOwner );

protected:
	int SetActionAni( IN ioBaseChar* pOwner, IN const ioHashString& szAnimation, IN float fTimeRate, IN DWORD dwPreDelay, OUT DWORD& dwEndTime, OUT DWORD& dwKeyReserve );
	void SetAttack( IN ioBaseChar* pOwner, IN const AttackAttribute* pAttribute, OUT DWORD& dwAttackEnd, OUT DWORD& dwReserve );
	
public:
	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );
	
public:
	void ChangeToExtendAttackFire( ioBaseChar *pOwner, FireTimeType eType );
	bool SetItemGrapplingState( ioBaseChar *pOwner, const ioHashString &szTargetName );

	virtual void SetChangeConditionalSpecialDash( ioBaseChar *pOwner, ioUserKeyInput::DirKeyInput eCurrDir, int eCallerType );
	void ReserveAttackComoboCount( ioBaseChar *pOwner, int eCallerType );

	void ChangeToCancelTeleport( ioBaseChar *pOwner );

	void ChangeToCancelTeleportAfter( ioBaseChar *pOwner );
	void ChangeToCancelTeleportAfterNormalAttack( ioBaseChar *pOwner, ioUserKeyInput::DirKeyInput eCurrKey );
	void ChangeToCancelTeleportAfterDashAttack( ioBaseChar *pOwner, ioUserKeyInput::DirKeyInput eCurrKey );
	void ChangeToCancelTeleportAfterJumpAttack( ioBaseChar *pOwner, ioUserKeyInput::DirKeyInput eCurrKey );	
	
	D3DXVECTOR3 GetGrapplingTargetPos( ioBaseChar *pOwner );

protected:
	void ChangeToAttackFire( ioBaseChar *pOwner );
	void ChangeToGrapplingMove( ioBaseChar *pOwner );		
	ioJumperItem::MoveState ProcessMove( IN ioBaseChar *pOwner, IN const D3DXVECTOR3& vMoveDir, IN float fMoveSpeed, IN float fMoveMaxRange, IN bool bCheckLanding, OUT float& fCurrRange );

	void ChangeToTeleport( ioBaseChar *pOwner );
	void ChangeToTeleportEnd( ioBaseChar *pOwner );

protected:
	void MakePortal( ioPlayStage* pStage, ioBaseChar *pOwner );
	D3DXVECTOR3 CalcPortalPos( ioPlayStage* pStage, ioBaseChar *pOwner, float fHeight, float fOffset, float fHeightOffset );
	void MakeMapEffect( IN ioPlayStage* pStage, IN ioBaseChar *pOwner, IN const ioHashString& szEffectName, IN D3DXVECTOR3 vPos, OUT UniqueObjID& ObjID );

public:
	void CheckSpecialState( ioBaseChar *pOwner );
	virtual void CheckExtendDashState( ioBaseChar *pOwner );

protected:
	void OnNormalAttack( ioBaseChar *pOwner );
	void OnCharging( ioBaseChar *pOwner );
	void OnExtendAttackFire( ioBaseChar *pOwner );
	void OnGrapplingPre( ioBaseChar *pOwner );
	void OnGrapplingMove( ioBaseChar *pOwner );
	void OnTeleport( ioPlayStage* pStage, ioBaseChar *pOwner );
	void OnCancelTeleportPre( ioBaseChar *pOwner );
	void OnCancelTeleport( ioBaseChar *pOwner );

public:
	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual bool IsConditionalSpecialDashReserve( ioBaseChar *pOwner );
	virtual bool IsChargeAttackState( ioBaseChar *pOwner );
	virtual bool IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck );
	
	virtual void SetUseExtraGauge( float fAmt );
	virtual void UpdateExtraData( ioBaseChar *pOwner );	
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );

public:
	virtual WeaponSubType GetSubType() const;

public:
	ioJumperItem::AttackState GetAttackState(){ return m_AttackState; }

public:	
	virtual int GetMaxBullet();	
	virtual int GetCurBullet();
	virtual void SetCurBullet( int iCurBullet );
	virtual int GetNeedBullet();
	int GetNeedCancelBullet();

public:
	ioJumperItem();
	ioJumperItem( const ioJumperItem &rhs );
	virtual ~ioJumperItem();
};

inline ioJumperItem* ToJumperItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_JUMPER_ITEM )
		return NULL;

	return static_cast< ioJumperItem* >( pItem );
}


//-----------------------------------------------------------------------------------------------------------

class ioJumperSpecialSpState : public ioSpecialStateBase
{
public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_JUMPER_SPECIAL"; }
	virtual const CharState GetState(){ return CS_JUMPER_SPECIAL; }
	
public:
	virtual void CheckSpecialState( ioBaseChar* pOwner );
	
	virtual bool ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime );
	virtual void ProcessKeyInput( ioBaseChar* pOwner );
	virtual void CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight );

public:	
	virtual bool IsSendControlInfoToUser( ioBaseChar* pOwner ){ return false; }	
	

public:
	bool ProcessForceMove( ioPlayStage* pStage, ioBaseChar* pOwner, D3DXVECTOR3 vDir, float fTimePerSec );

public:
	ioJumperSpecialSpState();
	virtual ~ioJumperSpecialSpState();
};

inline ioJumperSpecialSpState* ToJumperSpecialSpState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_JUMPER_SPECIAL )
		return NULL;

	return static_cast< ioJumperSpecialSpState* >( pState );
}