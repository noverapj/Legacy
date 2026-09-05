#pragma once

#include "ioObjectItem.h"

class ioWeapon;

class ioGalaxyObjectItem : public ioObjectItem
{
protected:
	ioHashString	m_GatherMeteoEffect;
	UniqueObjID		m_GatherMeteoEffectID;
	float			m_fCurrScaleRate;
	float			m_fHeightOffSet;

protected:
	struct GalaxyAttack
	{
		AttackAttribute m_Attribute;
		int m_iRangeMin;
		int m_iRangeMax;

		GalaxyAttack()
		{
			m_Attribute.Init();
			m_iRangeMin	= 0;
			m_iRangeMax	= 0;
		}
	};
	typedef std::vector<GalaxyAttack> GalaxyAttackList;

	GalaxyAttackList	m_NormalAttributeList;
	GalaxyAttackList	m_JumpAttributeList;
	int					m_iCurrFireIndex;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioItem* Clone();

	void LoadNoramlAttribute( ioINILoader &rkLoader );
	void LoadJumpAttribute( ioINILoader &rkLoader );

public:
	inline void SetEquipMeshScaleRate( float fCurrScaleRate ){ m_fCurrScaleRate = fCurrScaleRate; }
	inline void SetCurrFireIndex( int iCurrFireIndex ){ m_iCurrFireIndex = iCurrFireIndex; }
	inline void SetCurrHeightOffSet( float fHeightOffSet ){ m_fHeightOffSet = fHeightOffSet; }	

public:
	virtual void SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack );
	virtual ObjectSubType GetObjectSubType() const;

public:
	virtual void OnEquiped( ioBaseChar *pOwner );
	virtual void OnReleased( ioBaseChar *pOwner );

public:
	virtual const AttackAttribute* GetAttackAttribute( AttackType eType, int iSubIdx = 0 ) const;

	const AttackAttribute* GetCurrNoramlAttackAttribute() const;
	const AttackAttribute* GetCurrJumpAttackAttribute() const;

public:
	virtual void OnProcessState( ioPlayStage *pStage, ioBaseChar *pOwner );

public:
	ioGalaxyObjectItem();
	ioGalaxyObjectItem( const ioGalaxyObjectItem &rhs );
	virtual ~ioGalaxyObjectItem();
};

inline ioGalaxyObjectItem* ToGalaxyObjectItem( ioItem *pItem )
{
	ioObjectItem *pObject = ToObjectItem( pItem );
	if( !pObject || pObject->GetObjectSubType() != ioObjectItem::OST_GALAXY )
		return NULL;

	return dynamic_cast< ioGalaxyObjectItem* >( pObject );
}

