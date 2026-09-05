#pragma once

#include "ioWeaponItem.h"
#include "ioAttackAttribute.h"
#include "ioSpecialStateBase.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioDragonNestAssassinItem : public ioWeaponItem
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

	enum DragonNestAssassinSpecialState
	{
		DNASS_None,
		DNASS_Pre_Attack,
		DNASS_Combo_Attack,
		DNASS_Combo_Check,
		DNASS_Combo_Special_Delay,
		DNASS_Combo_Special_Attack,
		DNASS_Combo_Special_End,
		DNASS_Dash_Teleport,
		DNASS_End,
	};
	
protected:	
		
	ioHashStringVec m_AttackReadyAniList;	
	AttackAttribute m_PreAttackAttibute;		
	AttributeList m_vChargeComboAttList;	
	
	AttackAttribute m_ChargeSpecialComboDelayAttibute;
	AttributeList m_vChargeSpecialComboAttList;
	AttackAttribute m_ChargeSpecialComboEndAttibute;

	int m_nMaxCombo;
	int m_nMaxSpecialCombo;
	int m_nStartSpecialCombo;

protected:
	WeaponItemState m_WeaponItemState;
	DragonNestAssassinSpecialState m_DragonNestAssassinSpecialState;

	int m_nNormalCombo;
	DWORD m_dwAttackStartTime;
	DWORD m_dwPreMotionEndTime;
	DWORD m_dwComboEndMotionEndTime;
	DWORD m_dwComboSpecialMotionEndTime;
	
	DWORD m_dwComboReserveTime;

	int m_nCurCombo;
	int m_nCurSpecialCombo;

	ioUserKeyInput::DirKeyInput m_CurDirKey;

	bool m_bAttackKeyReserved;
	bool m_bDefenseKeyReserved;
	bool m_bJumpKeyReserved;		
	
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
	virtual void SetNextItemState( ioBaseChar *pWoundChar, ioBaseChar *pAttacker, ioWeapon* pWeapon, bool bDefense, float fDamage );

protected:
	void Init();
	void LoadAttackProperty( ioINILoader &rkLoader );
	void RecvSpecialState( ioBaseChar *pOwner, SP2Packet &rkPacket );
	void CheckCharging( ioBaseChar *pOwner );
	void SetNormalAttackState( ioBaseChar *pOwner );

public:
	void ClearState( ioBaseChar *pOwner );
	void SetDragonNestAssSpecialState( ioBaseChar *pOwner );
	void ProcessDragonNestAssSpecialState( ioBaseChar *pOwner );
	
protected:
	void SetChargePreAttackState( ioBaseChar *pOwner );
	void SetComboAttackState( ioBaseChar *pOwner );
	void SetComboNormalAttackState( ioBaseChar *pOwner );

protected:
	void SetComboSpecialDelayState( ioBaseChar *pOwner );
	void SetComboSpecialAttackState( ioBaseChar *pOwner );
	void SetComboSpecialEndState( ioBaseChar *pOwner );

protected:
	bool CheckComboInput( ioBaseChar *pOwner );
	void CheckComboSpecialInput( ioBaseChar *pOwner );
	void SetCharRotate( ioBaseChar *pOwner );

protected:
	void ReserveInputKey(ioBaseChar *pOwner );
	void CheckReserveState( ioBaseChar *pOwner );
	void SetReserveAttack( ioBaseChar *pOwner );
	void SetReserveDefence( ioBaseChar *pOwner );
	void SetReserveJump( ioBaseChar *pOwner );

public:
	//void SetDashTeleportState( ioBaseChar *pTarget, ioBaseChar *pAttacker );

public:
	ioDragonNestAssassinItem();
	ioDragonNestAssassinItem( const ioDragonNestAssassinItem &rhs );
	virtual ~ioDragonNestAssassinItem();
};

inline ioDragonNestAssassinItem* ToDragonNestAssassinItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_DRAGONNEST_ASSASASSIN_ITEM )
		return NULL;

	return dynamic_cast< ioDragonNestAssassinItem* >( pWeapon );
}

//-----------------------------------------------------------------------------------------------------------

class ioDragonNestAssSpecialState : public ioSpecialStateBase
{
public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_DRAGONNEST_ASS_SPECIAL"; }
	virtual const CharState GetState(){ return CS_DRAGONNEST_ASS_SPECIAL; }	
	virtual bool IsEnableBlowWounded( ioBaseChar *pOwner ){ return false; }

public:
	virtual void CheckSpecialState( ioBaseChar* pOwner );
	virtual void CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight );

public:
	ioDragonNestAssSpecialState();
	virtual ~ioDragonNestAssSpecialState();
};

inline ioDragonNestAssSpecialState* ToDragonNestAssSpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_DRAGONNEST_ASS_SPECIAL )
		return NULL;

	return dynamic_cast< ioDragonNestAssSpecialState* >( pState );
}