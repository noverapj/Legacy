#pragma once

#include "ioWeaponItem.h"
#include "ioAttackAttribute.h"
#include "ioSpecialStateBase.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioWarpItem : public ioWeaponItem
{
public:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_NORMAL_ATTACK,
		CS_TELEPORT,
		CS_TELEPORT_ATTACK,
		CS_TELEPORT_ATTACKSYNC,
		CS_TELEPORT_WARP,
		CS_GRAPPING_PRE,
		CS_GRAPPING_MOVE,
		CS_TELEPORT_WARP_ATTACK,
		CS_TELEPORT_PUSH,
		CS_TELEPORT_PUSH_ATTACK,
		CS_DASH_TELEPORT,
		CS_COUNTER_TELEPORT,
		CS_END,
	};

protected:
	enum MoveState
	{
		MS_MOVE,	//이동중
		MS_LANDING,	//이동중 지형과 충돌
		MS_END,		//이동 완료
	};

	ioHashStringVec m_AttackReadyAniList;
	ChargeState m_ChargeState;
	DWORD m_dwAttackStartTime;
	DWORD m_dwMotionEndTime;
	DWORD m_dwReserveAttackTime;
	bool m_bReserveAttack;
	bool m_bReserveJump;
	ioUserKeyInput::DirKeyInput m_DashDir;
	int m_iCurCombo;
	int m_iTeleportAttackCombo;

	CEncrypt<float> m_fMaxGauge;
	CEncrypt<float> m_fNeedGauge;
	CEncrypt<float> m_fIncreaseGauge;
	CEncrypt<float> m_fCurGauge;

	ioHashString m_szTeleportStartMotion;
	ioHashString m_szTeleportJumpStartMotion;
	ioHashString m_szTeleportStartEffect;
	ioHashString m_szTeleportEndEffect;
	ioHashString m_szDashTeleportStartMotion;
	ioHashString m_szDashTeleportStartEffect;
	ioHashString m_szDashTeleportEndEffect;
	ioHashString m_szCounterTeleportStartMotion;
	ioHashString m_szCounterTeleportStartEffect;
	ioHashString m_szCounterTeleportEndEffect;
	float m_fTeleportMotionRate;
	float m_fDashTeleportMotionRate;
	float m_fCounterTeleportMotionRate;
	float m_fTeleportTargetMaxRange;
	float m_fTeleportMaxAngle;
	float m_fTeleportMinAngle;
	float m_fTeleportOffset;
	float m_fTeleportJumpPower;
	float m_fTeleportNoTarget;
	float m_fJumpHeightGap;
	D3DXVECTOR3 m_vTargetPos;
	D3DXVECTOR3 m_vTeleportPos;
	bool m_bTeleportAfterJump;

	GrapplingSkillWoundedInfo m_GrapplingWounded;
	ioHashString m_GrapplingTargetName;
	ioHashString m_GrapperPreAnimation;
	ioHashString m_GrapperMoveAnimation;
	float m_GrapperPreAnimationRate;
	float m_GrapperMoveAnimationRate;
	float m_fGrapplingOffsetGapRange;
	float m_fGrapplingOffsetHeightRate;
	float m_fGrapAfterMoveSpeed;
	float m_fCurrGrapAfterMove;
	DWORD m_dwGrapperPreEndTime;

	float m_fInPortalOffset;
	float m_fOutPortalOffset;
	float m_fInPortalHeightOffset;
	float m_fOutPortalHeightOffset;
	ioHashString m_szInPortalMapEffect;
	ioHashString m_szOutPortalMapEffect;
	UniqueObjID m_InPortalID;
	UniqueObjID m_OutPortalID;

	float m_fPushOffset;
	float m_fPushHeightOffset;
	D3DXVECTOR3 m_vPushPos;

	ioHashString m_szDashWoundName;

	bool m_bDefenceCounterAttackState;

	AttackAttribute m_TeleportMove;
	AttackAttribute m_TeleportJump;
	AttackAttribute m_TeleportWarp;
	AttackAttribute m_TeleportWarpJump;
	AttackAttribute	m_TeleportWarpEnd;
	AttackAttribute m_TeleportPush;
	AttackAttribute m_TeleportPushEnd;
	AttackAttribute m_TeleportPushJump;
	AttackAttribute m_TeleportPushJumpEnd;
	std::vector<AttackAttribute> m_TeleportAttackAttribute;
	std::vector<AttackAttribute> m_TeleportJumpAttackAttribute;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

	void LoadGrapplingTargetProperty( IN ioINILoader& rkLoader, OUT GrapplingSkillWoundedInfo& rkInfo );
	void Init();

public:
	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );
	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void UpdateExtraData( ioBaseChar *pOwner );
	virtual void OnReleased( ioBaseChar *pOwner );
	virtual bool ProcessCancel( ioBaseChar *pOwner );
	virtual void SetNextItemState( ioBaseChar *pWoundChar, ioBaseChar *pAttacker, ioWeapon *pWeapon, bool bDefense, float fDamage );
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );	

	virtual void SetDefenceCounterState( ioBaseChar *pOwner );
	virtual bool IsUseDefenceCounterAttack();
	virtual void ProcessDefenceCounterAttack( ioBaseChar *pOwner );
	void DefenceCounter( ioBaseChar *pOwner );

	MoveState ProcessMove( IN ioBaseChar *pOwner, IN const D3DXVECTOR3& vMoveDir, IN float fMoveSpeed, IN float fMoveMaxRange, IN bool bCheckLanding, OUT float& fCurrRange );
	ChargeState GetChargeState(){ return m_ChargeState; }

	virtual int GetMaxBullet();
	virtual int GetCurBullet();
	virtual void SetCurBullet( int iCurBullet );
	virtual int GetNeedBullet();
	bool IsEnableGauge();
	bool IsCanKeyMove();

	void SetTeleportAttack( ioBaseChar *pOwner, bool bSendNetwork );
	void SetTeleportWarp( ioBaseChar *pOwner, bool bSendNetwork );
	void SetTeleportPush( ioBaseChar *pOwner, bool bSendNetwork );
	void SetWarpAttack( IN ioBaseChar* pOwner, IN const AttackAttribute* pAttribute, OUT DWORD& dwAttackEnd, OUT DWORD& dwReserve );
	void SetPushAttack( IN ioBaseChar* pOwner, IN const AttackAttribute* pAttribute, OUT DWORD& dwAttackEnd, OUT DWORD& dwReserve );
	int SetActionAni( IN ioBaseChar* pOwner, IN const ioHashString& szAnimation, IN float fTimeRate, IN DWORD dwPreDelay, OUT DWORD& dwEndTime, OUT DWORD& dwKeyReserve );

	bool SetItemGrapplingState( ioBaseChar *pOwner, const ioHashString &szTargetName );
	D3DXVECTOR3 GetGrapplingTargetPos( ioBaseChar *pOwner );

public:
	virtual WeaponSubType GetSubType() const;

	void InitSpecialState( ioBaseChar *pOwner );
	void ClearSpecialState( ioBaseChar *pOwner );
	void ProcessSpecialState( ioBaseChar *pOwner );
	void ChangeToTeleportAttack( ioBaseChar *pOwner, bool bJump );

protected:
	void OnCharging( ioBaseChar *pOwner );
	void ChangeToAttackFire( ioBaseChar *pOwner );
	void ChangeToGrapplingMove( ioBaseChar *pOwner );		
	void ChangeToTeleportWarpAttack( ioBaseChar *pOwner );
	void ChangeToTeleportPushAttack( ioBaseChar *pOwner, bool bSendNetwork );
	void ChangeToDashTeleport( ioBaseChar *pOwner, ioHashString szWoundName );
	void ChangeToCounterTeleport( ioBaseChar *pOwner, ioHashString szAttackerName );
	void SetTeleportSkill( ioBaseChar *pOwner, ioHashString szMotion, float fMotionRate );
	void CheckTeleportState( ioBaseChar *pOwner );
	void ProcessTeleport( ioBaseChar *pOwner );
	void ProcessTeleportAttack( ioBaseChar *pOwner );
	void ProcessToEnd( ioBaseChar *pOwner );
	void ProcessGrapplingPre( ioBaseChar *pOwner );
	void ProcessGrapplingMove( ioBaseChar *pOwner );
	void CheckDashTeleportState( ioBaseChar *pOwner );
	void CheckCounterTeleportState( ioBaseChar *pOwner );

	void CheckDecreaseGauge();
	void SetCurGauge( float fGauge );

	void MakePortal( ioPlayStage* pStage, ioBaseChar *pOwner );
	D3DXVECTOR3 CalcPortalPos( ioPlayStage* pStage, ioBaseChar *pChar, float fHeight, float fOffset, float fHeightOffset );
	void MakeMapEffect( IN ioPlayStage* pStage, IN ioBaseChar *pOwner, IN const ioHashString& szEffectName, IN D3DXVECTOR3 vPos, OUT UniqueObjID& ObjID );
	void DestroyMapEffect( ioPlayStage* pStage, ioBaseChar* pOwner );

	bool CheckLanding( ioBaseChar *pOwner );

public:
	ioWarpItem();
	ioWarpItem( const ioWarpItem &rhs );
	virtual ~ioWarpItem();
};

inline ioWarpItem* ToWarpItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_WARP_ITEM )
		return NULL;

	return dynamic_cast< ioWarpItem* >( pItem );
}

class ioWarpSpecialState : public ioSpecialStateBase
{
public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_WARP_SPECIAL"; }
	virtual const CharState GetState(){ return CS_WARP_SPECIAL; }	
	virtual bool IsEnableBlowWounded( ioBaseChar *pOwner ){ return false; }
	virtual bool IsEnableDashState( ioBaseChar* pOwner );
	virtual bool IsCanKeyMove( ioBaseChar* pOwner );

public:
	virtual void CheckSpecialState( ioBaseChar* pOwner );
	virtual void CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight );
	virtual void ProcessKeyInput( ioBaseChar* pOwner );

public:
	ioWarpSpecialState();
	virtual ~ioWarpSpecialState();
};

inline ioWarpSpecialState* ToWarpSpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_WARP_SPECIAL )
		return NULL;

	return dynamic_cast< ioWarpSpecialState* >( pState );
}