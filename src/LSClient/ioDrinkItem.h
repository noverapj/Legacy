#pragma once


#include "ioObjectItem.h"

class ioWeapon;

class ioDrinkItem : public ioObjectItem
{
protected:
	ioHashString m_DrinkAnimation;
	
	float m_fDrinkAnimationRate;
	
	DWORD m_dwDrinkAnimationDelay;
	DWORD m_dwDrinkStartTime;
	DWORD m_dwDrinkEndTime;

	bool m_bRandom;

public:	// overide ioItem
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

public:
	virtual ObjectSubType GetObjectSubType() const;

public:
	void SetDrinkState( ioBaseChar *pChar );
	bool ProcessDrinkState( ioBaseChar *pChar );

public:
	ioDrinkItem();
	ioDrinkItem( const ioDrinkItem &rhs );
	virtual ~ioDrinkItem();
};

inline ioDrinkItem* ToDrinkItem( ioItem *pItem )
{
	ioObjectItem *pObject = ToObjectItem( pItem );
	if( !pObject || pObject->GetObjectSubType() != ioObjectItem::OST_DRINK )
		return NULL;

	return dynamic_cast< ioDrinkItem* >( pObject );
}

inline ioDrinkItem* ToDrinkItem( ioObjectItem *pObject )
{
	if( !pObject || pObject->GetObjectSubType() != ioObjectItem::OST_DRINK )
		return NULL;

	return dynamic_cast< ioDrinkItem* >( pObject );
}

