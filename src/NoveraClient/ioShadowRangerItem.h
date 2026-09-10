#pragma once

#include "ioWeaponItem.h"
#include "ioAttackAttribute.h"
#include "ioSpecialStateBase.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioShadowRangerItem : public ioWeaponItem
{
public:
	enum WeaponItemState
	{
		WS_NONE,
		WS_CHECK_CHARGING,
		WS_NORMAL_ATTACK,
		WS_END,
		WS_SPECIAL_STATE_INIT,
		WS_SPECIAL_STATE_PROCESS,
		WS_COUNT_STATE_PROCESS,
	};

	enum SpecialState
	{
		SS_None,
		SS_Charge_Attack,
		SS_Charge_Weapon,
		SS_Teleport_Move_Predelay,
		SS_Teleport_Move_Ready,
		SS_Teleport_Move,
		SS_Teleport_Combo_Attack,
		SS_Special_Dash,
		SS_End,
		SS_End_Jump,
	};

	enum DefenceCounterState
	{
		DCS_None,
		DCS_Delay,
		DCS_Move,
		DCS_Combo,
		DCS_End,
		DCS_End_Jump,
	};

	struct DistInfo
	{
		ioHashString szCharName;
		float fDistSq;

		bool operator<( const DistInfo &rhs ) const
		{
			return fDistSq < rhs.fDistSq;
		}
	};
	typedef std::vector< DistInfo > SortList;

protected:
	float m_fMaxGauge;
	float m_fNeedGauge;
	float m_fIncreaseGauge;	

	ioHashStringVec m_AttackReadyAniList;	
	AttackAttribute m_ChargeAttack;
	
	float m_fChargeAttackTargetRange;
	float m_fChargeAttackTargetAngle;
	int m_nChargeAttackTargetCnt;

	ioHashString m_szChargeAimedEffect;
	D3DXVECTOR3 m_vAimedEffectOffset;
	
	float m_fTeleportMoveTargetRange;
	float m_fTeleportMoveTargetAngle;
	ioHashString m_szTeleportTargetCheckBuff;
	D3DXVECTOR3 m_vTeleportSuccessOffset;
	D3DXVECTOR3 m_vTeleportFailOffset;

	DWORD m_dwCounterChargeTime;
	int m_nCounterMaxCombo;
	AttackAttribute m_CounterDelay;
	AttackAttribute m_CounterMove;
	AttributeList m_CounterComboAttList;
	D3DXVECTOR3 m_vCounterOffset;

	float m_fEndJumpPower;
	float m_fJumpHeightGap;

	bool m_bEnableDefenceCounter;

	AttackAttribute m_NormalPreDelay;
	AttackAttribute m_NormalMoveAttack;
	AttributeList m_NormalMoveCombo;

	AttackAttribute m_JumpPreDelay;
	AttackAttribute m_JumpMoveAttack;
	AttributeList m_JumpMoveCombo;

	ioHashString m_szSpecialDashFront;
	ioHashString m_szSpecialDashBack;
	ioHashString m_szSpecialDashRt;
	ioHashString m_szSpecialDashLt;
	
	float m_fSpecialDashFrontRate;
	float m_fSpecialDashBackRate;
	float m_fSpecialDashRtRate;
	float m_fSpecialDashLtRate;	
	float m_fSpecialDashForceAmt;
	float m_fSpecialDashForceFric;

protected:
	DWORD m_dwAttackStartTime;
	DWORD m_dwMotionEndTime;
	float m_fCurGauge;

	WeaponItemState m_WeaponItemState;
	SpecialState m_SpecialState;
	DefenceCounterState m_DefenceCounterState;
	SortList m_SortList;

	int m_nNormalCombo;

	DWORD m_dwEnableReserveKeyTime;
	DWORD m_dwWeaponCreateTime;

	int m_nCurTeleportCombo;
	int m_nTeleportMaxCombo;

	DWORDVec m_ChargeAttackEffectIDList;

	AttackAttribute m_TeleportPreDelay;
	AttackAttribute m_TeleportAttribute;
	AttributeList m_TeleportComboList;

	bool m_bAttackKeyReserved;
	bool m_bDefenseKeyReserved;
	bool m_bJumpKeyReserved;

	int m_nCurCounterCombo;

	DWORD m_dwSpecialDashForceTime;
	D3DXVECTOR3	m_vSpecialDashForceDir;

	bool m_bIsTeleportFloat;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();
	virtual WeaponSubType GetSubType() const;

public:
	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );
	
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );
	
	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual bool IsChargeAttackState( ioBaseChar *pOwner );

public:
	void Init();

	virtual int GetMaxBullet();
	virtual int GetCurBullet();
	virtual void SetCurBullet( int iCurBullet );
	virtual void InitExtraGauge();
	virtual int GetNeedBullet();

	virtual void UpdateExtraData( ioBaseChar *pOwner );

	void DecreaseGauge( float fUseGauge );
	bool IsEnableGauge( float fUseGauge );

protected:
	void LoadAttackProperty( ioINILoader &rkLoader );
	void RecvSpecialState( ioBaseChar *pOwner, SP2Packet &rkPacket );
	void CheckCharging( ioBaseChar *pOwner );
	void SetNormalAttackState( ioBaseChar *pOwner );

public:
	void ClearState( ioBaseChar *pOwner );
	void SetSpecialState( SpecialState eType );
	void SetShadowRangerSpecialState( ioBaseChar *pOwner );
	void ProcessShadowRangerSpecialState( ioBaseChar *pOwner );

protected:
	bool FindTarget( ioBaseChar *pOwner, bool bCheckBuff, float fColRange, float fColAngle );
	void SortTargetList( ioBaseChar *pOwner, const BaseCharList &TargetList, bool bCheckBuff );
	bool IsEnableTarget( ioBaseChar *pOwner, ioBaseChar *pTarget, bool bCheckBuff );	

protected:
	void SetChargeAttackState( ioBaseChar *pOwner );
	void SetTeleportPreDelayState( ioBaseChar *pOwner );
	void SetTeleportMoveState( ioBaseChar *pOwner );
	void SetTeleportComboAttackState( ioBaseChar *pOwner );

protected:
	void TeleportPreDelay( ioBaseChar *pOwner );
	void TeleportMove( ioBaseChar *pOwner );
	bool IsFloatState( ioBaseChar *pOwner );

public:
	void SetCurTeleport( const AttackAttribute& MoveAttribute, const AttributeList& ComboList, bool bIsTeleportFloat );
	bool IsNoDropState();

protected:
	void CreateChargeWeapon( ioBaseChar *pOwner );
	bool CheckTeleportComboInput( ioBaseChar *pOwner );
	
protected:
	DWORD CreateMapEffect( ioBaseChar *pOwner, ioHashString& szEffect, D3DXVECTOR3& vPos, D3DXVECTOR3& vScale );
	void DestroyMapEffect( ioBaseChar *pOwner, DWORD& dwEffectID );
	void DestroyAllMapEffect( ioBaseChar *pOwner );

protected:
	void ReserveInputKey(ioBaseChar *pOwner, bool bEnableAttack, bool bEnableDefence, bool bEnableJump );
	void CheckReserveState( ioBaseChar *pOwner, bool bCounter );
	void SetReserveAttack( ioBaseChar *pOwner );
	void SetReserveDefence( ioBaseChar *pOwner );
	void SetReserveJump( ioBaseChar *pOwner );

public:
	virtual bool IsConditionalSpecialDashReserve( ioBaseChar *pOwner );
	virtual void SetChangeConditionalSpecialDash( ioBaseChar *pOwner, ioUserKeyInput::DirKeyInput eCurrDir, int eCallerType );

protected:
	void ChangeConditionSpecialDash( ioBaseChar *pOwner, const ioHashString &szAni, float fAniRate, D3DXVECTOR3 vDir );

public:
	virtual void SetDefenceCounterState( ioBaseChar *pOwner );
	virtual bool IsUseDefenceCounterAttack();
	virtual void ProcessDefenceCounterAttack( ioBaseChar *pOwner );

protected:
	void LoadCountProperty( ioINILoader &rkLoader );
	void RecvCounterState( ioBaseChar *pOwner, SP2Packet &rkPacket );

protected:
	void SetDefenceCounterMoveState( ioBaseChar *pOwner );
	void SetDefenceCounterComboState( ioBaseChar *pOwner );
	void CheckCounterCharging( ioBaseChar *pOwner );
	bool CheckCounterComboInput( ioBaseChar *pOwner );

public:
	ioShadowRangerItem();
	ioShadowRangerItem( const ioShadowRangerItem &rhs );
	virtual ~ioShadowRangerItem();
};

inline ioShadowRangerItem* ToShadowRangerItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_SHADOWRANGER_ITEM )
		return NULL;

	return dynamic_cast< ioShadowRangerItem* >( pWeapon );
}

//-----------------------------------------------------------------------------------------------------------

class ioShadowRangerSpecialState : public ioSpecialStateBase
{
public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_SHADOW_RANGER_SPECIAL"; }
	virtual const CharState GetState(){ return CS_SHADOW_RANGER_SPECIAL; }	
	virtual bool IsEnableBlowWounded( ioBaseChar *pOwner ){ return false; }

public:
	virtual void CheckSpecialState( ioBaseChar* pOwner );
	virtual void CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight );
	virtual bool IsNoDropState( ioBaseChar* pOwner ) { return true; }
	virtual void GetGravity( ioBaseChar* pOwner, float &fCurGravity );

public:
	ioShadowRangerSpecialState();
	virtual ~ioShadowRangerSpecialState();
};

inline ioShadowRangerSpecialState* ToShadowRangerSpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_SHADOW_RANGER_SPECIAL )
		return NULL;

	return dynamic_cast< ioShadowRangerSpecialState* >( pState );
}