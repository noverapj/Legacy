#pragma once

#include "ioObjectItem.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioObjectWeaponItem : public ioObjectItem
{
public:
	enum ChargeComboType
	{
		CCT_NONE	  = 0,
		CCT_NORMAL	  = 1,
		CCT_COMBO	  = 2,		
	};

	enum ChargeState
	{
		CS_NONE,
		CS_CHARGING,
		CS_ATTACK_FIRE,
		CS_COMBO,
	};

protected:
	ChargeState m_ChargeState;
	ChargeComboType m_ChargeComboType;

	ioHashStringVec m_AttackReadyAniList;
	AttributeList m_vChargeComboAttList;
	AttributeList m_vExtendAttributeList;
	AttributeList m_vUseCountAttributeList;

	DWORD m_dwAttackStartTime;
	int m_iCurCombo;

	CEncrypt<int> m_iChargeCombo;
	CEncrypt<DWORD> m_dwMaxComboDuration;
	CEncrypt<DWORD> m_dwStartComboTime;
	CEncrypt<DWORD> m_dwComboAttackTimeEnd;

	bool m_bCharged;
	CEncrypt<bool> m_bEnableObjectDefense;
	CEncrypt<bool> m_bEnableObjectDash;

	DWORD m_dwEquipTime;
	CEncrypt<DWORD> m_dwProtectTime;

	bool bEnableReturnJumpping;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

	virtual void OnEquiped( ioBaseChar *pOwner );

public:
	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );

	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual void OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual bool IsChargeAttackState( ioBaseChar *pOwner );
	virtual bool IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck );

	virtual bool SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType );
	virtual void ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap );

	virtual bool IsProtectState();

public:
	virtual ObjectSubType GetObjectSubType() const;

	virtual bool IsOneTimeWeaponItem() const;
	virtual bool IsEnableDash() const;
	virtual bool IsEnableDefense() const;
	virtual void CheckAttackCount( ioBaseChar *pOwner );
	bool IsEnableReturnJumpping()	{ return bEnableReturnJumpping; }

protected:
	void OnCharging( ioBaseChar *pOwner );
	void ChangeToAttackFire( ioBaseChar *pOwner, bool bCharged );
	void ChangeToUseCountAttackState( ioBaseChar *pOwner );

	void ChangeToComboState( ioBaseChar *pOwner );
	void SetNextComboState( ioBaseChar *pOwner );
	void OnComboState( ioBaseChar *pOwner );

	virtual void CheckReserve( ioBaseChar *pOwner );
	virtual bool ProcessCancel( ioBaseChar *pOwner );

public:
	ioObjectWeaponItem();
	ioObjectWeaponItem( const ioObjectWeaponItem &rhs );
	virtual ~ioObjectWeaponItem();
};

inline ioObjectWeaponItem* ToObjectWeaponItem( ioItem *pItem )
{
	ioObjectItem *pObject = ToObjectItem( pItem );
	if( !pObject || pObject->GetObjectSubType() != ioObjectItem::OST_CONTACT_WEAPON )
		return NULL;

	return dynamic_cast< ioObjectWeaponItem* >( pItem );
}

