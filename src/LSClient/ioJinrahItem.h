#pragma once

#include "ioWeaponItem.h"
#include "ioAttackAttribute.h"
#include "ioSpecialStateBase.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioJinrahItem : public ioWeaponItem
{
public:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_GATHERING,
		CS_NORMAL_ATTACK,
	};
	enum JinrahAttackState
	{
		JS_NONE,
		JS_FIRE,
		JS_COMBO,
		JS_DASH,
		JS_DASHFIRE,
		JS_LAND,
		JS_END,
	};

protected:
	ioHashStringVec m_AttackReadyAniList;

	ChargeState m_ChargeState;
	JinrahAttackState m_JinrahAttackState;

	DWORD m_dwFireSTime;
	DWORD m_dwAttackStartTime;
	DWORD m_dwFullChargeTime;
	int m_iCurCombo;
	int m_iSpecialCombo;

	ioHashString m_szGatheringAni;
	float		 m_fGatheringAniRate;

	ioHashString m_szLandingAni;
	float		 m_fLandingAniRate;

	AttackAttribute m_cFullAttribute;
	AttackAttribute m_cJumpFullAttribute;
	std::vector<AttackAttribute> m_ComboAttackAttribute;
	std::vector<AttackAttribute> m_JumpComboAttackAttribute;
	std::vector<D3DXVECTOR3> m_DashInfo;

	DWORD m_dwDashFireWeaponIndex;
	DWORD m_dwDashFireDelayTime;
	DWORD m_dwDashFireStartTime;

	//Cancel Dash
	AttackAttribute m_CancelDashAttribute;
	AttackAttribute m_JumpCancelDashAttribute;
	float m_fCancelDashEndJumpAmt;
	float m_fCancelDashRandRate;

	bool m_bJump;
	bool m_bReserveAttack;
	bool m_bSpecialAttack;
	bool m_bHalfSpecialAttack;

	DWORD m_dwMotionEndTime;
	DWORD m_dwReserveAttack;
	float m_fComboEndJumpPower;

	float m_fMaxGauge;
	float m_fNeedGauge;
	float m_fIncreaseGauge;
	float m_fCurGauge;
	float m_fJumpHeightGap;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

public:
	virtual void OnEquiped( ioBaseChar *pOwner );
	virtual void OnReleased( ioBaseChar *pOwner );

	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );
	virtual void CheckExtendDashState( ioBaseChar *pOwner );	
	virtual void SetExtraGaugeTypeByWeapon( ioBaseChar *pOwner, int iType, int iWGT, float fDamage );

	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket );

public:
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );
	virtual WeaponSubType GetSubType() const;

protected:
	void OnCharging( ioBaseChar *pOwner );
	void OnGathering( ioBaseChar *pOwner );
	void ChangeToNormalAttack( ioBaseChar *pOwner );
	void ChangeGatheringState( ioBaseChar *pOwner );
	void ChangeReserveKeyInputState( ioBaseChar *pOwner, int iCurCombo = 0 );
	void ChangeToDashFire( ioBaseChar *pOwner );
	void SetDashFire( ioBaseChar *pOwner );

public:
	void SetAttackState( ioBaseChar *pOwner, bool bSendNet, bool bJump );
	void SetHalfAttackState( ioBaseChar *pOwner, bool bSendNet, bool bJump );
	virtual void SetChangeConditionalSpecialDash( ioBaseChar *pOwner, ioUserKeyInput::DirKeyInput eCurrDir, int eCallerType );
	void SetComboAttack( ioBaseChar *pOwner, bool bSendNetwork );

	void ProcessSpecialState( ioBaseChar *pOwner );
	void ProcessCancelDashState( ioBaseChar *pOwner );
	virtual bool ProcessCancelBySpeicalDash( ioBaseChar *pOwner );

	inline const int GetJinrahState() const	{ return (int)m_JinrahAttackState; }

	virtual bool IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck );
	virtual bool IsConditionalSpecialDashReserve( ioBaseChar *pOwner );

protected:
	void SetChangeEndState( ioBaseChar *pOwner, bool bSend );
	void SetChangeLandState( ioBaseChar *pOwner );

	void ProcessComboAttack( ioBaseChar *pOwner );
	void ProcessFireAttack( ioBaseChar *pOwner );
	void ProcessDashFireState( ioBaseChar *pOwner );
	bool ProcessSpeicalFire( ioBaseChar *pOwner );

public:
	virtual int GetMaxBullet();
	virtual int GetNeedBullet();
	virtual int GetCurBullet();
	virtual void SetCurBullet( int iCurBullet );

	virtual void InitExtraGauge();
	virtual void MaxExtraGauge();

	virtual float GetMaxExtraGauge();
	virtual float GetCurExtraGauge();
	virtual void  SetCurExtraGauge( float fGauge );

	void DecreaseGauge();
	bool IsEnableGauge();
	virtual void UpdateExtraData( ioBaseChar *pOwner );

public:
	void ClearAttackState( ioBaseChar *pOwner );
	void ClearJumpSpecial();
	bool IsSpecialAttack()		{ return m_bSpecialAttack; }
	bool IsHalfSpecialAttack()	{ return m_bHalfSpecialAttack; }

public:
	ioJinrahItem();
	ioJinrahItem( const ioJinrahItem &rhs );
	virtual ~ioJinrahItem();
};

inline ioJinrahItem* ToJinrahItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_JINRAH_ITEM )
		return NULL;

	return dynamic_cast< ioJinrahItem* >( pItem );
}
//////////////////////////////////////////////////////////////////////////
class ioJinrahSpecialState : public ioSpecialStateBase
{
public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_JINRAH_SPECIAL"; }
	virtual const CharState GetState(){ return CS_JINRAH_SPECIAL; }	

public:
	virtual void CheckSpecialState( ioBaseChar* pOwner );
	virtual void CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight );
	virtual bool IsEnableDashState( ioBaseChar* pOwner );

public:
	ioJinrahSpecialState();
	virtual ~ioJinrahSpecialState();
};

inline ioJinrahSpecialState* ToJinrahSpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_JINRAH_SPECIAL )
		return NULL;

	return dynamic_cast< ioJinrahSpecialState* >( pState );
}