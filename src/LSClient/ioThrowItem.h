#pragma once

#include "ioObjectItem.h"

class ioWeapon;

class ioThrowItem : public ioObjectItem
{
public:	// overide ioItem
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

public:	// overide ioWeaponItem
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );

public:
	virtual ObjectSubType GetObjectSubType() const;

public:
	ioThrowItem();
	ioThrowItem( const ioThrowItem &rhs );
	virtual ~ioThrowItem();
};

inline ioThrowItem* ToThrowItem( ioItem *pItem )
{
	ioObjectItem *pObject = ToObjectItem( pItem );
	if( !pObject || pObject->GetObjectSubType() != ioObjectItem::OST_THROW )
		return NULL;

	return dynamic_cast< ioThrowItem* >( pObject );
}

