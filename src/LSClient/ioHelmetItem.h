#pragma once

#include "ioItem.h"

class ioINILoader;
struct RopeSpringParam;

class ioHelmetItem : public ioItem
{
public:
	enum HelmetHair
	{
		HH_NONE      = -1,
		HH_HALF_SKIP =  0,
		HH_ALL_SKIP  =  1,
		HH_BIND_SKIP =  2,
		HH_NO_SKIP   =  3,
	};

	enum InvisibleFaceType
	{
		IFT_NONE,
		IFT_ALL,
		IFT_MALE,
		IFT_FEMALE,
	};

	typedef struct tagHelmetHairInfo
	{
		HelmetHair m_HumanMan;
		HelmetHair m_HumanWoman;

		tagHelmetHairInfo()
		{
			m_HumanMan   = HH_HALF_SKIP;
			m_HumanWoman = HH_HALF_SKIP;
		}
	}HelmetHairInfo;

	enum HelmetFace
	{
		HF_NONE,
		HF_HALF_SKIP,
	};

	typedef struct tagHelmetFaceInfo
	{
		HelmetFace m_HumanMan;
		HelmetFace m_HumanWoman;

		tagHelmetFaceInfo()
		{
			m_HumanMan   = HF_NONE;
			m_HumanWoman = HF_NONE;
		}
	}HelmetFaceInfo;

protected:
	ioHashString m_EquipMesh;
	HelmetHairInfo m_HelmetHairInfo;
	HelmetFaceInfo m_HelmetFaceInfo;

	InvisibleFaceType m_InvisibleFaceType;

	DecoInfoMap  m_EquipMeshMap;

	typedef std::map< DWORD, HelmetHairInfo > HelmetHairMap;
	HelmetHairMap m_ChangeHairMap;

	typedef std::map< DWORD, HelmetFaceInfo > HelmetFaceMap;
	HelmetFaceMap m_ChangeFaceMap;

protected:
	ioHashString m_SpringBiped;
	RopeSpringParam *m_pSpringParam;

protected:       // Decoration Info
	const ioHashString& FindEquipMesh() const;
	HelmetHairInfo FindChangeHair() const;
	HelmetFaceInfo FindChangeFace() const;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ItemType GetType() const;
	virtual ioItem* Clone();
	virtual void SetItemType();

	virtual void OnEquiped( ioBaseChar *pOwner );
	virtual void OnReleased( ioBaseChar *pOwner );
	
public:
	virtual void AddGlowLine( ioBaseChar *pOwner );
	virtual void RemoveGlowLine( ioBaseChar *pOwner );

public:
	const ioHashString& GetEquipMesh( ioBaseChar *pOwner ) const;
	HelmetHair GetChangeHair( bool bMale ) const;
	HelmetFace GetChangeFace( bool bMale ) const;
	void ReCalcVisible( ioBaseChar *pOwner );

public:
	bool HasSpringRope() const;

	const ioHashString& GetSpringBiped() const;
	const RopeSpringParam* GetSpringParam() const;

	InvisibleFaceType GetInvisibleFaceType() const;	
	
public:
	ioHelmetItem();
	ioHelmetItem( const ioHelmetItem &rhs );
	virtual ~ioHelmetItem();
};

inline ioHelmetItem* ToHelmetItem( ioItem *pItem )
{
	if( !pItem || pItem->GetType() != ioItem::IT_HELMET )
		return NULL;

	return dynamic_cast< ioHelmetItem* >( pItem );
}

inline const ioHelmetItem* ToHelmetItemConst( const ioItem *pItem )
{
	if( !pItem || pItem->GetType() != ioItem::IT_HELMET )
		return NULL;

	return dynamic_cast< const ioHelmetItem* >( pItem );
}
