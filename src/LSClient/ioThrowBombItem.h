#pragma once

#include "ioObjectItem.h"

class ioWeapon;

class ioThrowBombItem : public ioObjectItem
{
protected:
	bool m_bDropBomb;

	ioHashString m_EquipedEffect;
	D3DXVECTOR3 m_vDropMoveDir;
	
public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

public:
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );
	virtual ObjectSubType GetObjectSubType() const;

public:
	virtual void OnEquiped( ioBaseChar *pOwner );
	virtual void OnReleased( ioBaseChar *pOwner );

public:
	void ChangeDropState( const D3DXVECTOR3 &vDropDir );

public:
	ioThrowBombItem();
	ioThrowBombItem( const ioThrowBombItem &rhs );
	virtual ~ioThrowBombItem();
};

inline ioThrowBombItem* ToThrowBombItem( ioItem *pItem )
{
	ioObjectItem *pObject = ToObjectItem( pItem );
	if( !pObject || pObject->GetObjectSubType() != ioObjectItem::OST_THROWBOMB )
		return NULL;

	return dynamic_cast< ioThrowBombItem* >( pObject );
}

