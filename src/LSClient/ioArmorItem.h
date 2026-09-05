#pragma once


class ioINILoader;
struct RopeSpringParam;

#include "ioAttackableItem.h"

class ioArmorItem : public ioAttackableItem
{
protected:
	ioHashString m_BodyMesh;
	ioHashString m_HandMesh;
	
	DecoInfoMap m_BodyMeshMap;
	DecoInfoMap m_HandMeshMap;

	int m_iArmorSoundType;

	ArmorType m_ArmorType;

protected:
	ioHashString m_SpringBiped;
	RopeSpringParam *m_pSpringParam;

protected:       // Decoration Info
	const ioHashString& FindBodyMesh() const;
	const ioHashString& FindHandMesh() const;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ItemType GetType() const;
	virtual ioItem* Clone();
	virtual void SetItemType();

public:
	virtual void AddGlowLine( ioBaseChar *pOwner );
	virtual void RemoveGlowLine( ioBaseChar *pOwner );

public:
	bool HasHandMesh( ioBaseChar *pOwner ) const;
	const ioHashString& GetBodyMesh( ioBaseChar *pOwner ) const;
	const ioHashString& GetHandMesh( ioBaseChar *pOwner ) const;
	int GetArmorSoundType();
	ArmorType GetArmorType() const;

public:
	bool HasSpringRope() const;
	const ioHashString& GetSpringBiped() const;
	const RopeSpringParam* GetSpringParam() const;

public:
	virtual void SetExtraGaugeTypeByWeapon( ioBaseChar *pOwner, int iType, int iWGT, float fDamage );

public:
	ioArmorItem();
	ioArmorItem( const ioArmorItem &rhs );
	virtual ~ioArmorItem();
};

inline ioArmorItem* ToArmorItem( ioItem *pItem )
{
	if( !pItem || pItem->GetType() != ioItem::IT_ARMOR )
		return NULL;

	return dynamic_cast< ioArmorItem* >( pItem );
}

inline const ioArmorItem* ToArmorItemConst( const ioItem *pItem )
{
	if( !pItem || pItem->GetType() != ioItem::IT_ARMOR )
		return NULL;

	return dynamic_cast< const ioArmorItem* >( pItem );
}
