#pragma once

#include "ioObjectItem.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioWeapon;
class ioBaseChar;

class ioObjectCountItem : public ioObjectItem
{
public:
	enum ObjectState
	{
		OS_NONE,
		OS_ATTACK,
		OS_ATTACK_END,
	};

protected:
	CEncrypt<bool> m_bEnableObjectDefense;
	CEncrypt<bool> m_bEnableObjectDash;

	CEncrypt<DWORD> m_dwProtectTime;
	CEncrypt<bool> m_bJumpAttackProtect;
	
	bool m_bSetUseCount;
	bool m_bComboAttack;
	
	std::vector< DWORD > m_ResetTimeList;

protected:
	ObjectState m_ObjectState;

	DWORD m_dwAttackEndTime;
	int m_iCurCombo;
	bool m_bSetJumpAttack;
	DWORD m_dwEquipTime;
	bool m_bFirstAttack;
	
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
	virtual void OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket );
	
	virtual bool IsProtectState();
	virtual void OnEquiped( ioBaseChar *pOwner );
	virtual void OnReleased( ioBaseChar *pOwner );

public:
	virtual bool IsOneTimeWeaponItem() const;
	virtual bool IsEnableDash() const;
	virtual bool IsEnableDefense() const;
	virtual void CheckAttackCount( ioBaseChar *pOwner );
	virtual bool IsAttackComboInit() const;
	virtual void CheckActiveCount( ioBaseChar *pOwner );

protected:
	virtual void CheckReserve( ioBaseChar *pOwner );
	virtual bool ProcessCancel( ioBaseChar *pOwner );

protected:
	void SetAttackState( ioBaseChar *pOwner );
	void SetAttackEndState( ioBaseChar *pOwner );

public:
	ioObjectCountItem();
	ioObjectCountItem( const ioObjectCountItem &rhs );
	virtual ~ioObjectCountItem();
};

inline ioObjectCountItem* ToObjectCountItem( ioItem *pItem )
{
	ioObjectItem *pObject = ToObjectItem( pItem );
	if( !pObject || pObject->GetObjectSubType() != ioObjectItem::OST_COUNT )
		return NULL;

	return dynamic_cast< ioObjectCountItem* >( pItem );
}

