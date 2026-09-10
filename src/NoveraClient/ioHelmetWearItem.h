#pragma once

#include "ioWearItem.h"

class ioINILoader;

class ioHelmetWearItem : public ioWearItem
{
public:
	enum HelmetWearHair
	{
		HWH_HALF_SKIP,
		HWH_ALL_SKIP,
		HWH_BIND_SKIP,
		HWH_NO_SKIP,
	};

	enum HelmetWearFace
	{
		HWF_NONE,
		HWF_HALF_SKIP,
	};

protected:
	ioHashString m_EquipMeshRed;
	ioHashString m_EquipMeshBlue;

	HelmetWearHair m_ChangeHair;
	HelmetWearFace m_ChangeFace;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual WearSubType GetSubType() const;
	virtual ioItem* Clone();

	virtual void OnEquiped( ioBaseChar *pOwner );

public:
	inline HelmetWearHair GetChangeHair() const { return m_ChangeHair; }
	inline HelmetWearFace GetChangeFace() const { return m_ChangeFace; }

public:
	ioHelmetWearItem();
	ioHelmetWearItem( const ioHelmetWearItem &rhs );
	virtual ~ioHelmetWearItem();
};

inline ioHelmetWearItem* ToHelmetWearItem( ioItem *pItem )
{
	ioWearItem *pWear = ToWearItem( pItem );
	if( !pWear || pWear->GetSubType() != ioWearItem::WRT_HELMET )
		return NULL;

	return dynamic_cast< ioHelmetWearItem* >( pItem );
}

inline const ioHelmetWearItem* ToHelmetWearItemConst( const ioItem *pItem )
{
	const ioWearItem *pWear = ToWearItemConst( pItem );
	if( !pItem || pWear->GetSubType() != ioWearItem::WRT_HELMET )
		return NULL;

	return dynamic_cast< const ioHelmetWearItem* >( pItem );
}

