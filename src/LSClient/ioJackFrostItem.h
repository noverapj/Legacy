#pragma once

#include "ioWeaponItem.h"

class ioEntityGroup;

class ioJackFrostItem : public ioWeaponItem
{
protected:
	ioHashString m_BranchDelayAni;
	ioHashString m_CreateEffect;

	CEncrypt<int> m_iMaxCreateObjCnt;

	bool m_bImmediatelyEquip;
	DWORD	m_dwChargeStartTime;
	DWORD	m_dwMotionEndTime;
	DWORD	m_dwCreateObjectTime;
	DWORD	m_dwCreateObjectItemCode;
	float	m_fCreateOffset;

	AttributeList m_vExtendAttributeList;
	IntVec m_vCreateObjectIndex;

protected:
	enum MineState
	{
		MS_NONE,
		MS_NORMAL_ATTACK,
		MS_CHARGING,
		MS_CREATE_OBJECT,
		MS_DELETE_OBJECT,
	};

	MineState m_MineState;
	CEncrypt<int> m_iCurCombo;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

public:
	virtual void SetNormalAttack( ioBaseChar *pOwner, int iCurCombo );
	virtual void CheckNormalAttack( ioBaseChar *pOwner );
	virtual void OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket );
	virtual WeaponSubType GetSubType() const;

	virtual bool IsChargeAttackState( ioBaseChar *pOwner );

	void AddCreateObjectItemInfo( int iCreateObjectIndex );

protected:
	void OnCharging( ioBaseChar *pOwner );
	void OnCreateObjectItem( ioBaseChar *pOwner );

protected:
	void ChangeToNormalAttack( ioBaseChar *pOwner );
	void ChangeToCreateObject( ioBaseChar *pOwner );
	void CreateObjectItem( ioBaseChar *pOwner );
	void DeleteObjectItem( ioBaseChar *pOwner );

public:
	virtual int GetMaxBullet();

	const WeaponInfo& GetExtendAttributeIdx() const;

public:
	ioJackFrostItem();
	ioJackFrostItem( const ioJackFrostItem &rhs );
	virtual ~ioJackFrostItem();
};

inline ioJackFrostItem* ToJackFrostItem( ioItem *pItem )
{
	ioWeaponItem *pWeapon = ToWeaponItem( pItem );
	if( !pWeapon || pWeapon->GetSubType() != ioWeaponItem::WST_JACK_FROST_ITEM )
		return NULL;

	return dynamic_cast< ioJackFrostItem* >( pWeapon );
}

