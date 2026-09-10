#pragma once

#include "ioWeaponItem.h"
#include "ioAttackAttribute.h"
#include "ioSpecialStateBase.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioChargeContactWeaponItem : public ioWeaponItem
{
public:
	enum ExpansionReaperChargeAttackType
	{
		ERCAT_NONE,
		ERCAT_NORMAL,
		ERCAT_DASH,
		ERCAT_JUMP,
	};
protected:
	ioHashStringVec m_AttackReadyAniList;
	AttributeList m_vExtendAttributeList;

protected:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_GATHERING,
		CS_ATTACK_FIRE,
		CS_GATHERING_FIRE,
		CS_EXPANSION_REAPER_ATTACK,

		CS_REAPER_EXPANSION_STATE,
		CS_REAPER_EXPANSION_TELEPORT,

		CS_CANCEL_DASH,
		CS_WOUNDED_MOVE,
		CS_WOUNDED_MOVE_ADD_ATTACK,

		CS_MID_CHARGE_ATTACK,
		CS_HAWKEYE_DASH_ATTACK,
		CS_SALADIN_INPUT_ATTACK,
	};

	enum WoundedState
	{
		WS_DELAY,
		WS_TIME_CONDITION_CHECK,
	};

	enum WoundedType
	{
		WT_NONE,
		WT_FIGHTER,
	};

	enum MidAttackType
	{
		MAT_NONE,
		MAT_KNIGHT,
	};

	enum ItemDashType
	{
		IDT_DEFAULT,
		IDT_KNIGHT,
	};

	enum SyncCancelType
	{
		SCT_EXTEND_DEFENSE_END,
		SCT_DEFENCE_COUNTER_ATTACK,
		SCT_MID_CHARGE_ADD_BUFF,
		SCT_CHANGE_DASH_STATE,
		SCT_EXTRA_GAUGE_BUFF,
		SCT_GAUGE_MAX_EFFECT,
	};

	ChargeState			m_ChargeState;
	WoundedState		m_WoundedState;
	WoundedType			m_WoundedType;
	MidAttackType		m_MidAttackType;
	ItemDashType		m_ItemDashType;
	DWORD				m_dwAttackStartTime;
	int					m_iCurCombo;

	bool				m_bCharged;

	//
	CEncrypt<float>		m_fDodgeForce;
	CEncrypt<float>		m_fDodgeFrictionRate;

	CEncrypt<float>		m_fDodgeAniRate;
	ioHashStringVec		m_vDodgeAniList;

	DWORD				m_dwDodgeDefenseEnd;

protected:
	CEncrypt<bool>		m_bEnableExtraGauge;
	CEncrypt<bool>		m_bStaticGaugeRecovery;
	CEncrypt<bool>		m_bNewJumpAttackColSkip;
	CEncrypt<bool>		m_bUsedExtraGauge;
	ioHashStringVec		m_ExtraGaugeBuffList;

	//사신 진화 차지 어택
	DWORD				m_dwMotionEndTime;
	AttackAttribute		m_ExpansionChargeAttack;
	AttackAttribute		m_ExpansionDashChargeAttack;
	AttackAttribute		m_ExpansionJumpChargeAttack;
	float				m_fExpansionChargeAttackEndJumpPower;
	float				m_fExpansionDashChargeAttackEndJumpPower;
	float				m_fExpansionJumpChargeAttackEndJumpPower;
	float				m_fCurExpansionChargeAttackEndJumpPower;

	//사신 진화 텔레포트
	ioHashString		m_szExpansionReaperTeleportAni;
	float				m_fExpansionReaperTeleportAniRate;

	float				m_fTeleportTargetRange;
	float				m_fTeleportTargetAngle;
	TargetWoundType		m_TeleportTargetWoundType;
	D3DXVECTOR3			m_vNoTargetTeleportOffset;
	D3DXVECTOR3			m_vTargetTeleportOffset;

	DWORD				m_dwEnableReaperExpansionTeleportTime;
	DWORD				m_dwReaperExpansionTeleportTicTime;
	float				m_fExpansionReaperTeleportEndJumpPower;

	float				m_fNeedExtraGauge;
	bool				m_bUsingGunTypeBullet;
	bool				m_bCheckAnotherState;

	//태권 무도가 진화
	//Cancel Dash
	AttackAttribute		m_CancelDashAttribute;
	bool				m_bWoundAfterUseExtendState;
	bool				m_bIgnoreChargeDashCheck;
	bool				m_bInputDashRotToDirKey;
	//Wounded After
	DWORD				m_dwStartWoundedTimeCheck;
	DWORD				m_dwWoundedKeyInputWaitTime;
	AttackAttribute		m_WoundedDashMove;
	AttackAttribute		m_WoundedMoveAddAttack;
	bool				m_bWoundedDashKeyCheck;
	float				m_fWoundedDashMoveNeedBullet;
	float				m_fWoundedDashEnableHeightGap;

	//Count Attack
	bool				m_bUseDefenceCounterAttack;
	bool				m_bUseDefenceCounterAttackEndDash;
	bool				m_bDefeceCounterAttackState;

//진화 아이언 나이트
	//Mid Charge
	DWORD				m_dwMidChargeTime;
	AttackAttribute		m_MidChargeAttack;
	ioHashString		m_szMidChargeWoundBuff;
	ioHashString		m_szMidChargeDefenceBuff;

	bool				m_bUseExtendDefence;
	bool				m_bDefenseEnableSet;
	DWORD				m_dwDefenseStartTime;
	DWORD				m_dwDefenseEndTime;

	AttackAttribute		m_HawkEyeChargeAttack;
	float				m_fHawkEyeEndJumpPower;
	float				m_fHawkEyeCancelJumpPower;

	AttackAttribute		m_GatheringAttack;
	ioHashString m_GatherAnimation;
	float m_fGatherAniRate;
	DWORD m_dwGatheringTime;
	DWORD m_dwGatheringStartTime;

	DWORD m_dwInputAttackTime;
	AttackAttribute m_cInputAttributeS;

	AttackAttribute m_GaugeCounterAttack;

	ioHashString m_szGaugeMaxEffect;
	DWORD m_dwGaugeMaxEffectIndex;

public:
	virtual bool SetExtendDefenseState( ioBaseChar *pOwner );
	virtual void CheckExtendDefenseState( ioBaseChar *pOwner );

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

public:
	virtual void OnReleased( ioBaseChar *pOwner );

	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );

	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual bool IsChargeAttackState( ioBaseChar *pOwner );
	virtual bool IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck );

	virtual bool SetDodgeDefenseState( ioBaseChar *pOwner );
	virtual void CheckDodgeDefenseState( ioBaseChar *pOwner );
	virtual void ApplyDodgeDefenseState( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual bool SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType );
	virtual void ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap );

public:
	virtual int GetMaxBullet();
	virtual int GetCurBullet();
	virtual void SetCurBullet( int iCurBullet );
	virtual int GetNeedBullet();
	bool UsingGunTypeBullet();

	virtual void SetExtraGaugeTypeByWeapon( ioBaseChar *pOwner, int iType, int iWGT, float fDamage );
	virtual void SetUseExtraGauge( float fAmt );
	virtual void UpdateExtraData( ioBaseChar *pOwner );

	bool IsEnableExtraGauge() const { return m_bEnableExtraGauge; }
	bool IsUsedExtraGauge() const { return m_bUsedExtraGauge; }

	void ClearInputAttackInfo();
	void SetInputAttackInfo( ioBaseChar *pOwner, int iAniID, float fTimeRate );
	void SetInputAttackState( ioBaseChar *pOwner, bool bSendNet );
	bool CheckInputAttack();
	void ProcessInputAttackState( ioBaseChar *pOwner );

	bool IsEnableGaugeCounter();
	void SetGaugeCounter( ioBaseChar *pOwner );

	void CreateGaugeMaxEffect( ioBaseChar *pChar );
	void DestroyGaugeMaxEffect();

	bool IsNewJumpAttackColSkip()		{ return m_bNewJumpAttackColSkip; }

protected:
	void DecreaseGauge( float fAmt );
	void SetExtraGaugeBuff( ioBaseChar *pOwner );
	void RemoveExtraGaugeBuff( ioBaseChar *pOwner );

public:
	virtual WeaponSubType GetSubType() const;

protected:
	void OnCharging( ioBaseChar *pOwner );
	void OnGathering( ioBaseChar *pOwner );
	void ChangeToAttackFire( ioBaseChar *pOwner, bool bCharged );
	void ChangeToGatherAttackFire( ioBaseChar *pOwner );

	virtual void CheckReserve( ioBaseChar *pOwner );
	virtual bool ProcessCancel( ioBaseChar *pOwner );

public:
	bool CheckExpansionReaperAttack( ioBaseChar *pOwner, ExpansionReaperChargeAttackType eType );
	void ChangeToExpansionReaperAttack( ioBaseChar *pOwner, ExpansionReaperChargeAttackType eType );

	void SetHawkEyeSpecialState( ioBaseChar *pOwner );
	void ChangeToGathering( ioBaseChar *pOwner );

protected:
	void ProcessExpansionReaperAttackState( ioBaseChar *pOwner );

	void FindAttTarget( ioBaseChar *pOwner, ioHashString& szTarget );
	void SetTeleport( ioBaseChar *pOwner );

public:
	bool CheckEnableReaperExpansionTeleportState( ioBaseChar *pOwner );
	void SetReaperExpansionTeleportState( ioBaseChar *pOwner );
	void ProcessSpecialState( ioBaseChar *pOwner );
	void ProcessHawkEyeSpecialState( ioBaseChar *pOwner );

protected:
	bool CheckAnotherState( ioBaseChar *pOwner );

	virtual void SetChangeConditionalDefaultSpecialDash( ioBaseChar *pOwner, ioUserKeyInput::DirKeyInput eCurrDir, int eCallerType );
	virtual void SetChangeConditionalKinghtSpecialDash( ioBaseChar *pOwner, ioUserKeyInput::DirKeyInput eCurrDir, int eCallerType );

public:
	virtual void SetNoneState( ioBaseChar* pOwner, int eNewState = 0 );

protected:
	//대쉬 공격
	virtual void CheckExtendDashState( ioBaseChar *pOwner );	
	virtual bool IsConditionalSpecialDashReserve( ioBaseChar *pOwner );

	virtual bool ProcessCancelBySpeicalDash( ioBaseChar *pOwner );
	virtual void SetChangeConditionalSpecialDash( ioBaseChar *pOwner, ioUserKeyInput::DirKeyInput eCurrDir, int eCallerType );
	//피격중 특별
	virtual bool IsWoundAfterUseExtendState(){ return m_bWoundAfterUseExtendState; }
	virtual bool CheckWoundAfterUseExtendState( ioBaseChar* pOwner, BlowTargetType eType );
	virtual void CallItemProcess( ioBaseChar* pOwner );
	virtual bool IsEnableWoundedDashCheck( ioBaseChar *pOwner, bool bCheck ) { return m_bWoundedDashKeyCheck; }

	//Counter Attack
	virtual void SetDefenceCounterState( ioBaseChar *pOwner );
	virtual bool IsUseDefenceCounterAttack();
	virtual bool IsUseDefenceCounterAttackEndDash();
	virtual void ProcessDefenceCounterAttack( ioBaseChar *pOwner );

	virtual void SetNextItemState( ioBaseChar *pWoundChar, ioBaseChar *pAttacker, ioWeapon *pWeapon, bool bDefense, float fDamage );

	void CheckWoundFighterAfter( ioBaseChar* pOwner, BlowTargetType eType );

protected:
	void ProcessCancelDashState( ioBaseChar *pOwner );
	
	bool IsCanWoundedDashMove( ioBaseChar *pOwner );
	void ChangeWoundedDashMoveState( ioBaseChar *pOwner );
	void ProcessWoundedDashMoveState( ioBaseChar *pOwner );
	void ChangeWoundedMoveAddAttackState( ioBaseChar *pOwner );
	void ProcessWoundedMoveAddAttackState( ioBaseChar *pOwner );

	void DefenceAttack( ioBaseChar *pOwner );

//아이언나이트 진화
protected:
	bool CheckEnableMidAttack( ioBaseChar *pOwner );
	void ChangeToMidAttackState( ioBaseChar *pOwner );
	void ProcessMidAttackState( ioBaseChar *pOwner );
	void DefenseEnd( ioBaseChar *pOwner );

public:
	ioChargeContactWeaponItem();
	ioChargeContactWeaponItem( const ioChargeContactWeaponItem &rhs );
	virtual ~ioChargeContactWeaponItem();
};

inline ioChargeContactWeaponItem* ToChargeContactWeaponItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_CHARGE_CONTACT )
		return NULL;

	return dynamic_cast< ioChargeContactWeaponItem* >( pItem );
}
///////////////////////////////////////////////////////////////////////////////
//사신 진화
class ioReaperExpansionSpecialState : public ioSpecialStateBase
{
public:
	virtual const char* GetStateName(){ return "CS_REAPER_EXPANSION"; }
	virtual const CharState GetState(){ return CS_REAPER_EXPANSION; }
	virtual bool IsEnableBlowWounded( ioBaseChar *pOwner ){ return false; }
	virtual bool IsNoDropState( ioBaseChar* pOwner ) { return true; }
public:
	virtual void CheckSpecialState( ioBaseChar* pOwner );
	virtual void CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight );

public:
	ioReaperExpansionSpecialState();
	virtual ~ioReaperExpansionSpecialState();
};

inline ioReaperExpansionSpecialState* ToPeaperExpansionSpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_REAPER_EXPANSION )
		return NULL;

	return dynamic_cast< ioReaperExpansionSpecialState* >( pState );
}
///////////////////////////////////////////////////////////////////////////////
class ioHawkEyeSpecialState : public ioSpecialStateBase
{
public:
	virtual const char* GetStateName(){ return "CS_HAWKEYE_SPECIAL"; }
	virtual const CharState GetState(){ return CS_HAWKEYE_SPECIAL; }
	virtual bool IsEnableBlowWounded( ioBaseChar *pOwner ){ return false; }

public:
	virtual void CheckSpecialState( ioBaseChar* pOwner );
	virtual void CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight );
	virtual bool IsCanKeyMove( ioBaseChar* pOwner ) { return true; }
	virtual void ProcessKeyInput( ioBaseChar* pOwner );

public:
	ioHawkEyeSpecialState();
	virtual ~ioHawkEyeSpecialState();
};

inline ioHawkEyeSpecialState* ToHawkEyeSpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_HAWKEYE_SPECIAL )
		return NULL;

	return dynamic_cast< ioHawkEyeSpecialState* >( pState );
}