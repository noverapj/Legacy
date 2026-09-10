#pragma once

#include "ioWearItem.h"

class ioINILoader;

class ioCloakWearItem :	public ioWearItem
{
public:
	enum CloakWearType
	{
		CWT_NONE,
		CWT_FLAG
	};

protected:
	int m_iCloakWearType;

	ioHashString m_EquipMeshRed;
	ioHashString m_EquipMeshBlue;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual WearSubType GetSubType() const;
	virtual ioItem* Clone();

	virtual void OnEquiped( ioBaseChar *pOwner );

public:
	int GetCloakWearItemType();

public:
	ioCloakWearItem(void);
	ioCloakWearItem( const ioCloakWearItem &rhs );
	virtual ~ioCloakWearItem(void);
};

inline ioCloakWearItem* ToCloakWearItem( ioItem *pItem )
{
	ioWearItem *pWear = ToWearItem( pItem );
	if( !pWear || pWear->GetSubType() != ioWearItem::WRT_CLOAK )
		return NULL;

	return dynamic_cast< ioCloakWearItem* >( pItem );
}

inline const ioCloakWearItem* ToCloakWearItemConst( const ioItem *pItem )
{
	const ioWearItem *pWear = ToWearItemConst( pItem );
	if( !pItem || pWear->GetSubType() != ioWearItem::WRT_CLOAK )
		return NULL;

	return dynamic_cast< const ioCloakWearItem* >( pItem );
}



