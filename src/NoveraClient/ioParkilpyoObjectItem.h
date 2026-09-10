#pragma once

#include "ioObjectItem.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioParkilpyoObjectItem : public ioObjectItem
{
public:
	enum WeaponItemState
	{
		WS_NONE,
		WS_CHECK_CHARGING,
		WS_NORMAL_ATTACK,
		WS_CHARGE_ATTACK,
		WS_END,
	};

protected:
	DWORD m_dwProtectTime;
	ioHashStringVec m_AttackReadyAniList;	
	float m_fChargeUseGauge;
	AttackAttribute m_ChargeAttack;
	float m_fEquipJumpPower;

	ioHashStringVec m_EquipMaleMeshList;
	ioHashStringVec m_EquipFemaleMeshList;
	ioHashString m_szEquipEffect;

protected:
	WeaponItemState m_WeaponState;

	DWORD m_dwAttackStartTime;

	DWORD m_dwCheckProtectTime;
	CEncrypt<int> m_nNormalCombo;
	DWORD m_dwMotionEndTime;	
	
public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();
	virtual ObjectSubType GetObjectSubType() const;

public:
	const virtual char* GetAttackStateText() const;

public:
	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );
	virtual bool SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType );
	virtual void ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap );

	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );

	virtual void OnEquiped( ioBaseChar *pOwner );
	virtual void OnReleased( ioBaseChar *pOwner );
	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );
	
public:
	virtual bool IsProtectState();
	virtual bool IsOneTimeWeaponItem() const;
	virtual bool IsEnableDash() const;
	virtual bool IsEnableDefense() const;
	
protected:
	void Init();
	void CheckCharging( ioBaseChar *pOwner );
	void SetNormalAttackState( ioBaseChar *pOwner );
	void ChangeToChargeAttack( ioBaseChar *pOwner );

public:
	void DecreaseGauge( ioBaseChar *pOwner, float fUseGauge );
		
protected:
	void EquipChangeMesh( ioBaseChar *pOwner );
	void ReleaseChangeMesh( ioBaseChar *pOwner );
	
public:
	ioParkilpyoObjectItem();
	ioParkilpyoObjectItem( const ioParkilpyoObjectItem &rhs );
	virtual ~ioParkilpyoObjectItem();
};

inline ioParkilpyoObjectItem* ToParkilpyoObjectItem( ioItem *pItem )
{
	ioObjectItem *pObject = ToObjectItem( pItem );
	if( !pObject || pObject->GetObjectSubType() != ioObjectItem::OST_PARKILPYO )
		return NULL;

	return dynamic_cast< ioParkilpyoObjectItem* >( pItem );
}

