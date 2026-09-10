#pragma once

#include "ioItem.h"

struct RopeSpringParam;

class ioCloakItem : public ioItem
{
protected:
	ioHashString m_EquipMesh;
	DecoInfoMap  m_EquipMeshMap;

	ioHashString m_SpringBiped;
	RopeSpringParam *m_pSpringParam;
	
protected:       // Decoration Info
	const ioHashString& FindEquipMesh() const;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ItemType GetType() const;
	virtual ioItem* Clone();
	virtual void SetItemType();
	
public:
	virtual void AddGlowLine( ioBaseChar *pOwner );
	virtual void RemoveGlowLine( ioBaseChar *pOwner );

public:
	bool HasSpringRope() const;

	const ioHashString& GetEquipMesh( ioBaseChar *pOwner ) const;
	const ioHashString& GetSpringBiped() const;
	const RopeSpringParam* GetSpringParam() const;

public:
	ioCloakItem();
	ioCloakItem( const ioCloakItem &rhs );
	virtual ~ioCloakItem();
};

inline ioCloakItem* ToCloakItem( ioItem *pItem )
{
	if( !pItem || pItem->GetType() != ioItem::IT_CLOAK )
		return NULL;

	return dynamic_cast< ioCloakItem* >( pItem );
}

inline const ioCloakItem* ToCloakItemConst( const ioItem *pItem )
{
	if( !pItem || pItem->GetType() != ioItem::IT_CLOAK )
		return NULL;

	return dynamic_cast< const ioCloakItem* >( pItem );
}

