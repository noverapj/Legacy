#pragma once

#include "ioObjectItem.h"
#include "ioAttackAttribute.h"
#include "ioSpecialStateBase.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioThunderBirdObjectItem : public ioObjectItem
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

	enum SpecialAttackState
	{
		SAS_NONE,
		SAS_MOVE,
		SAS_BOMB,
		SAS_END,
	};

protected:
	WeaponItemState m_WeaponState;
	SpecialAttackState m_SpecialAttackState;

	DWORD m_dwAttackStartTime;

	DWORD m_dwCheckProtectTime;
	CEncrypt<int> m_nNormalCombo;
	ioUserKeyInput::DirKeyInput m_StartDirKey;
	bool m_bSetDelay;

	DWORD m_dwCheckReloadTime;

protected:
	DWORD m_dwProtectTime;
	ioHashStringVec m_AttackReadyAniList;	
	float m_fNormalUseGauge;

	ioHashString m_szChargeDelay;
	float m_fChargeDelayRate;
	ioHashString m_szChargeFrontMove;
	float m_fChargeFrontMoveRate;
	ioHashString m_szChargeBackMove;
	float m_fChargeBackMoveRate;
	ioHashString m_szChargeRightMove;
	float m_fChargeRightMoveRate;
	ioHashString m_szChargeLeftMove;
	float m_fChargeLeftMoveRate;

	WeaponInfo m_ChargeFireInfo;

	DWORD m_dwReloadTime;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();
	virtual ObjectSubType GetObjectSubType() const;

public:
	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );
	virtual bool SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType );
	virtual void ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap );

	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );

	virtual void OnEquiped( ioBaseChar *pOwner );
	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );
	void RecvSpecialState( ioBaseChar *pOwner, SP2Packet &rkPacket );

public:
	virtual bool IsProtectState();
	virtual bool IsOneTimeWeaponItem() const;
	virtual bool IsEnableDash() const;
	virtual bool IsEnableDefense() const;
	
protected:
	void CheckCharging( ioBaseChar *pOwner );
	void SetNormalAttackState( ioBaseChar *pOwner );

public:
	void SetSpecialState( ioBaseChar *pOwner );
	void DecreaseGauge( ioBaseChar *pOwner, float fUseGauge );
	bool CheckGauge( ioBaseChar *pOwner );

public:
	void Init();
	void ProcessSpecialState( ioBaseChar *pOwner );
	bool IsEnableMoveState();

protected:
	void SetBombState( ioBaseChar *pOwner );
	void SetMoveState( ioBaseChar *pOwner );
	void CheckMoveAni( ioBaseChar *pOwner );
	void SetSpecialEndState( ioBaseChar *pOwner );
	void CreateWeapon( ioBaseChar *pOwner );

protected:
	bool CheckInputKey( ioBaseChar *pOwner );

public:
	bool CheckThunderBirdGauge( ioBaseChar *pOwner );

public:
	ioThunderBirdObjectItem();
	ioThunderBirdObjectItem( const ioThunderBirdObjectItem &rhs );
	virtual ~ioThunderBirdObjectItem();
};

inline ioThunderBirdObjectItem* ToThunderBirdObjectItem( ioItem *pItem )
{
	ioObjectItem *pObject = ToObjectItem( pItem );
	if( !pObject || pObject->GetObjectSubType() != ioObjectItem::OST_THUNDER_BIRD )
		return NULL;

	return dynamic_cast< ioThunderBirdObjectItem* >( pItem );
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
class ioThunderBirdObjectSpecialState : public ioSpecialStateBase
{
public:
	virtual void Initialize( ioBaseChar* pOwner, CharState eCurrState );
	virtual void Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState );

	virtual const char* GetStateName(){ return "CS_THUNDER_BIRD_OBJECT_SPECIAL"; }
	virtual const CharState GetState(){ return CS_THUNDER_BIRD_OBJECT_SPECIAL; }
	virtual bool IsEnableBlowWounded( ioBaseChar *pOwner ){ return false; }	

public:
	virtual void CheckSpecialState( ioBaseChar* pOwner );
	virtual void CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight );
	virtual void ProcessKeyInput( ioBaseChar* pOwner );
	
public:
	ioThunderBirdObjectSpecialState();
	virtual ~ioThunderBirdObjectSpecialState();
};

inline ioThunderBirdObjectSpecialState* ToThunderBirdObjectSpecialState( ioSpecialStateBase *pState )
{
	if( !pState || pState->GetState() != CS_THUNDER_BIRD_OBJECT_SPECIAL )
		return NULL;

	return dynamic_cast< ioThunderBirdObjectSpecialState* >( pState );
}