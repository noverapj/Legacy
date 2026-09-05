#pragma once


#include "ioWeaponItem.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioChargeAreaweaponItem : public ioWeaponItem
{
protected:
	ioHashStringVec m_AttackReadyAniList;
	AttributeList m_vExtendAttributeList;

protected:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_HOLDING,
		CS_HOLDING_END,
		CS_ATTACK_FIRE,
	};

	ChargeState m_ChargeState;
	DWORD m_dwAttackStartTime;
	int m_iCurCombo;

	// Holding
	ioHashString m_AreaWeaponName;
	DWORD m_dwAreaweaponIndex;

	ioHashString m_PreHoldAnimation;

	ioHashString m_HoldAnimation;
	float m_fHoldAniRate;

	DWORD m_dwStartHoldTime;
	CEncrypt<DWORD> m_dwHoldDuration;


public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

public:
	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );
	
	virtual void OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem=false );
	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual bool IsChargeAttackState( ioBaseChar *pOwner );

public:
	virtual WeaponSubType GetSubType() const;

protected:
	void OnCharging( ioBaseChar *pOwner );
	void OnHolding( ioBaseChar *pOwner );

	void ApplyHolding( ioBaseChar *pOwner, SP2Packet &rkPacket );

	void ChangeToHolding( ioBaseChar *pOwner );
	void ChangeToAttackFire( ioBaseChar *pOwner, bool bCharged );

	void CheckHoldEnd( ioBaseChar *pOwner );

public:
	ioChargeAreaweaponItem();
	ioChargeAreaweaponItem( const ioChargeAreaweaponItem &rhs );
	virtual ~ioChargeAreaweaponItem();
};

inline ioChargeAreaweaponItem* ToChargeAreaweaponItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_CHARGE_AREAWEAPON )
		return NULL;

	return dynamic_cast< ioChargeAreaweaponItem* >( pItem );
}

