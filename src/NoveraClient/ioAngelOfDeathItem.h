#pragma once

#include "ioWeaponItem.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioAngelOfDeathItem : public ioWeaponItem
{
protected:
	ioHashStringVec m_AttackReadyAniList;
	AttributeList m_vExtendAttributeList;

protected:
	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_ATTACK_FIRE,
		CS_EXTEND_FIRE,
	};

	ChargeState m_ChargeState;
	DWORD m_dwAttackStartTime;
	int m_iCurCombo;

	bool m_bCharged;
	//bool m_bAreaWeaponCreate;

	DWORD m_dwExtendAttackMotionFullTime;
	DWORD m_dwExtendAttackFireTime;
	ioHashString m_szAreaWeaponName;
	float m_fOffSetAreaWeapon;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

public:
	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );

	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual bool IsChargeAttackState( ioBaseChar *pOwner );

	virtual bool SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType );
	virtual void ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap );	

public:
	virtual WeaponSubType GetSubType() const;

protected:
	void OnCharging( ioBaseChar *pOwner );
	void ChangeToAttackFire( ioBaseChar *pOwner, bool bCharged );

	virtual void CheckReserve( ioBaseChar *pOwner );

	virtual bool ProcessCancel( ioBaseChar *pOwner );

public:
	ioAngelOfDeathItem();
	ioAngelOfDeathItem( const ioAngelOfDeathItem &rhs );
	virtual ~ioAngelOfDeathItem();
};

inline ioAngelOfDeathItem* ToAngelOfDeathItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_ANGEL_OF_DEATH_ITEM )
		return NULL;

	return dynamic_cast< ioAngelOfDeathItem* >( pItem );
}

