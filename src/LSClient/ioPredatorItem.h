#pragma once

#include "ioWeaponItem.h"
#include "ioAttackAttribute.h"
#include "ioUserKeyInput.h"
#include "ioSpecialStateBase.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioPredatorItem : public ioWeaponItem
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
	};

	enum SpecialState
	{
		SS_NONE,
		SS_INIT,
		SS_PRE_DELAY,
		SS_FIND_TARGET,
		SS_AIMED_TARGET,
		SS_AIM_ATTACK,
		SS_CHECK_COMBO,
		SS_COMBO_ATTACK,
		SS_CHARGE_ATTACK,
		SS_DASH_ATTACK,
		SS_END,
	};	

protected:
	float m_fNeedGauge;
	ioHashStringVec m_AttackReadyAniList;
	ioHashString m_szWaitPreDelayAni;
	float m_fWaitPreDelayAniRate;

	ioHashString m_szWaitLoopAni;
	ioHashString m_szWaitMoveAni;
	float m_fWaitMoveAniRate;

	ioHashString m_szAimLoopAni;
	ioHashString m_szAimMoveAni_Front;
	ioHashString m_szAimMoveAni_Back;
	ioHashString m_szAimMoveAni_Left;
	ioHashString m_szAimMoveAni_Right;
	float m_fAimMoveAniRate;
	
	float m_fMarkerRange;
	ioHashString m_szReadyMarker;
	ioHashString m_szAimingMarker;
	ioHashString m_szAimedMarker;

	float m_fAimAngle;
	float m_fAimRange;
	float m_fAimMinRange;

	ioHashString m_AimUpAni;
	ioHashString m_AimCenterAni;
	ioHashString m_AimDownAni;

	DWORD m_dwAimBipeMoveTime;
	DWORD m_dwAimBipeReturnTime;

	ioHashString m_AimFireUpAni;
	ioHashString m_AimFireCenterAni;
	ioHashString m_AimFireDownAni;

	AttackAttribute m_AimAttack;

	int m_nMaxSpecialComboCnt;
	AttributeList m_ComboAttackList;
	ioHashStringVec m_ComboFireUpList;
	ioHashStringVec m_ComboFireCenterList;
	ioHashStringVec m_ComboFireDownList;

	ioHashString m_ComboAimUpAni;
	ioHashString m_ComboAimCenterAni;
	ioHashString m_ComboAimDownAni;

	AttackAttribute m_ComboChargeAttack;
	AttackAttribute m_ComboDashAttack;

	DWORD m_dwComboChargeTime;

	ioHashString m_szJumpWaitPreDelayAni;
	float m_fJumpWaitPreDelayAniRate;

	ioHashString m_szJumpWaitLoopAni;
	ioHashString m_szJumpAimLoopAni;

	float m_fJumpMarkerRange;

	float m_fJumpAimAngle;
	float m_fJumpAimRange;
	float m_fJumpAimMinRange;

	ioHashString m_JumpAimUpAni;
	ioHashString m_JumpAimCenterAni;
	ioHashString m_JumpAimDownAni;
	
	ioHashString m_JumpAimFireUpAni;
	ioHashString m_JumpAimFireCenterAni;
	ioHashString m_JumpAimFireDownAni;

	AttackAttribute m_JumpAimAttack;

	int m_nMaxJumpSpecialComboCnt;
	AttributeList m_JumpComboAttackList;
	ioHashStringVec m_JumpComboFireUpList;
	ioHashStringVec m_JumpComboFireCenterList;
	ioHashStringVec m_JumpComboFireDownList;

	ioHashString m_JumpComboAimUpAni;
	ioHashString m_JumpComboAimCenterAni;
	ioHashString m_JumpComboAimDownAni;

	AttackAttribute m_JumpComboChargeAttack;
	AttackAttribute m_JumpComboDashAttack;

	float m_fEndJumpPower;
	DWORD m_dwJumpSpecialStateTime;
	
protected:
	WeaponItemState m_WeaponItemState;
	SpecialState m_SpecialState;

	int m_nNormalCombo;
	DWORD m_dwAttackStartTime;
	DWORD m_dwComboAttackStartTime;
	DWORD m_dwMotionEndTime;
	DWORD m_dwAimTrackingStartTime;
	DWORD m_dwAimTrackingEndTime;
	float m_fCurAimAngle;
	DWORD m_dwFireTime;
	DWORD m_dwEnableReserveKeyTime;
	int m_nCurSpecialComboCnt;
	D3DXVECTOR3	m_vAimedDir;
	
	bool m_bSetMove;

	bool m_bAttackKeyReserved;
	bool m_bDefenseKeyReserved;
	bool m_bJumpKeyReserved;

	bool m_bEnableDownAim;

	ioUserKeyInput::DirKeyInput m_AimStartDirKey;

	bool m_bSpecialStateJump;
	bool m_bSpecialStateMove;
	DWORD m_dwSpecialStateTime;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();
	virtual WeaponSubType GetSubType() const;
	
public:
	virtual void SetNormalAttack( ioBaseChar *pOwner, int nCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );
	
	virtual bool IsChargeAttackState( ioBaseChar *pOwner );
	virtual bool IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck );

	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );	
	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );

protected:
	void RecvSpecialState( ioBaseChar *pOwner, SP2Packet &rkPacket );

public:
	virtual void UpdateExtraData( ioBaseChar *pOwner );
	virtual void SetUseExtraGauge( float fAmt );
	virtual int GetMaxBullet();
	virtual int GetCurBullet();
	virtual void SetCurBullet( int nCurBullet );
	virtual int GetNeedBullet();

protected:
	void Init();
	void CheckCharging( ioBaseChar *pOwner );
	void SetNormalAttackState( ioBaseChar *pOwner );

public:
	void ClearState( ioBaseChar *pOwner );
	void SetPredatorSpecialState( ioBaseChar *pOwner, bool bJump );
	void ProcessPredatorSpecialState( ioBaseChar *pOwner );
	ioPredatorItem::SpecialState GetSpecialState();
	bool IsJumpSpecialState();

protected:
	void CheckInputKey( ioBaseChar *pOwner );
	void CheckComboInputKey( ioBaseChar *pOwner );
	bool CheckInputDash( ioBaseChar *pOwner );

protected:
	void SetPreDelayState( ioBaseChar *pOwner );
	void SetFindTargetState( ioBaseChar *pOwner, bool bSendNetwork );
	void SetAimedTargetState( ioBaseChar *pOwner );
	void SetAimedFireState( ioBaseChar *pOwner, const AttackAttribute& Attack );
	void SetAimedFireAni( ioBaseChar *pOwner, const ioHashString& stUp, const ioHashString& stCenter, const ioHashString& stDown );
	void SetComboAttackState( ioBaseChar *pOwner, const AttributeList& ComboList );
	void SetCheckComboState( ioBaseChar *pOwner );
	void SetComboSpecialState( ioBaseChar *pOwner, const AttackAttribute& Special, SpecialState eSpecialState, bool bRemoveMarker );
		
public:
	bool IsEnableMoveState();

protected:
	void CheckFindTargetMoveAni( ioBaseChar *pOwner );
	void CheckAimedTargetMoveAni( ioBaseChar *pOwner );

protected:
	bool FindTarget( ioBaseChar *pOwner );
	bool CheckTargetValidate( ioBaseChar *pOwner );

protected:
	void ProcessTargetTracking( ioBaseChar *pOwner, const ioHashString& stUp, const ioHashString& stCenter, const ioHashString& stDown );
	float GetAimedUpDownWeight( const D3DXVECTOR3 &vAimDir, float fMaxAngle );
	void UpdateAimDir( ioBaseChar *pOwner );

protected:
	void AimedFireToUp( ioEntityGroup *pGrp, ioHashString szFireUp, ioHashString szFireCenter, float fWeight, float fTimeRate, DWORD dwPreDelay, bool bHold );
	void AimedFireToDown( ioEntityGroup *pGrp, ioHashString szFireCenter, ioHashString szFireDown, float fWeight, float fTimeRate, DWORD dwPreDelay, bool bHold );
	D3DXVECTOR3 ReCalcMoveDir( ioBaseChar *pOwner, ioWeapon *pWeapon );

protected:
	void ReserveInputKey(ioBaseChar *pOwner );
	void CheckReserveState( ioBaseChar *pOwner );
	void SetReserveAttack( ioBaseChar *pOwner );
	void SetReserveDefence( ioBaseChar *pOwner );
	void SetReserveJump( ioBaseChar *pOwner );

public:
	ioPredatorItem();
	ioPredatorItem( const ioPredatorItem &rhs );
	virtual ~ioPredatorItem();
};

inline ioPredatorItem* ToPredatorItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_PREDATOR_ITEM )
		return NULL;

	return dynamic_cast< ioPredatorItem* >( pItem );
}

//-----------------------------------------------------------------------------------------------------------

class ioPredatorSpecialState : public ioSpecialStateBase
{
public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_PREDATOR_SPECIAL"; }
	virtual const CharState GetState(){ return CS_PREDATOR_SPECIAL; }	
	virtual bool IsEnableBlowWounded( ioBaseChar *pOwner );

public:
	virtual void CheckSpecialState( ioBaseChar* pOwner );
	virtual void CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight );
	virtual void ProcessKeyInput( ioBaseChar* pOwner );
	virtual bool IsEnableDashState( ioBaseChar* pOwner );
	virtual bool IsNoDropState( ioBaseChar* pOwner );

public:
	ioPredatorSpecialState();
	virtual ~ioPredatorSpecialState();
};

inline ioPredatorSpecialState* ToPredatorSpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_PREDATOR_SPECIAL )
		return NULL;

	return dynamic_cast< ioPredatorSpecialState* >( pState );
}