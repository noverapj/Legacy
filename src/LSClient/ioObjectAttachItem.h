#pragma once

#include "ioObjectItem.h"
#include "ioAttackAttribute.h"
#include "ioSpecialStateBase.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioObjectAttachItem : public ioObjectItem
{
public:
	enum WeaponItemState
	{
		WS_NONE,
		WS_CHECK_CHARGING,
		WS_CHECK_NORMAL,
		WS_END,
	};

	enum ChargingState
	{
		CS_NONE,
		CS_DELAY,
		CS_Check_Input,
		CS_Normal_Attack,
		CS_Charge_Attack,
		CS_END,
	};

	enum PacketState
	{
		PS_NORMAL,
		PS_CHARGE,
		PS_CHARGE_END,
	};

protected:
	WeaponItemState m_WeaponState;
	ChargingState m_ChargingState;

	DWORD m_dwAttackStartTime;

	DWORD m_dwProtectTime;
	DWORD m_dwCheckProtectTime;	

	AttackAttribute m_NormalAttackInfo;
	AttackAttribute m_ChargeAttackInfo;

	DWORD m_dwMotionEndTime;

	ioHashString m_szDelayFrontMove;
	ioHashString m_szDelayBackMove;
	ioHashString m_szDelayRightMove;
	ioHashString m_szDelayLeftMove;

	ioHashString m_szCheckBuff;
	
	ioUserKeyInput::DirKeyInput m_StartDirKey;
	bool m_bSetDelay;

	DWORD m_dwEnableInputKeyTime;

	bool m_bCharSkipState;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();
	virtual ObjectSubType GetObjectSubType() const;

public:
	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );
	virtual bool SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType );
	virtual void ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap );

	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void OnEquiped( ioBaseChar *pOwner );
	virtual void OnReleased( ioBaseChar *pOwner );
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );

	virtual bool IsProtectState();

	virtual bool IsAutoTarget() const { return false; }

public:
	virtual bool IsOneTimeWeaponItem() const;
	
public:
	void ProcessState( ioBaseChar *pOwner );
	bool IsCharCollisionSkipState();
	
protected:
	void SetDeleayState( ioBaseChar *pOwner );
	void SetNormalAttackState( ioBaseChar *pOwner );
	void SetChargeAttackState( ioBaseChar *pOwner );
	void SetChargeEndState( ioBaseChar *pOwner );

public:
	void Init();
	bool IsEnableMoveState();

protected:
	void CheckInputKey( ioBaseChar *pOwner );
	void CheckDelayMoveAni( ioBaseChar *pOwner );

public:
	ioObjectAttachItem();
	ioObjectAttachItem( const ioObjectAttachItem &rhs );
	virtual ~ioObjectAttachItem();
};

inline ioObjectAttachItem* ToObjectAttachItem( ioItem *pItem )
{
	ioObjectItem *pObject = ToObjectItem( pItem );
	if( !pObject || pObject->GetObjectSubType() != ioObjectItem::OST_ATTACH )
		return NULL;

	return dynamic_cast< ioObjectAttachItem* >( pItem );
}

//-----------------------------------------------------------------------------------------------------------

class ioObjectAttachSpecialState : public ioSpecialStateBase
{
public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_OBJECT_ATTACH"; }
	virtual const CharState GetState(){ return CS_OBJECT_ATTACH; }
	virtual bool IsEnableBlowWounded( ioBaseChar *pOwner ){ return false; }

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
	virtual void ProcessKeyInput( ioBaseChar* pOwner );

public:
	ioObjectAttachSpecialState();
	virtual ~ioObjectAttachSpecialState();
};

inline ioObjectAttachSpecialState* ToObjectAttachSpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_OBJECT_ATTACH )
		return NULL;

	return dynamic_cast< ioObjectAttachSpecialState* >( pState );
}