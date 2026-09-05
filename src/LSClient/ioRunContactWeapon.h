#pragma once


#include "ioWeapon.h"

class ioRunContactWeapon : public ioWeapon
{
protected:
	ioOrientBox m_PreAttackBox;
	ioOrientBox m_MidAttackBox;

	bool m_bFirstCheck;

protected:
	enum UpdateState
	{
		CONTACT_CREATED,
		CONTACT_FIRST,
		CONTACT_NORMAL,
	};
	UpdateState m_UpdateState;

public:
	static ActionStopType m_ActionStopType;
	static ActionStopType m_DieActionStopType;

public:
	virtual void SetStartCollisionBox();
	virtual void GetWorldAttackBoxList( std::vector< ioOrientBox > &vBoxList );
	virtual D3DXVECTOR3 GetAttackDir( const ioPlayEntity *pEntity ) const;

public:
	virtual bool IsCollision( ioPlayEntity *pEntity, bool bMapCollision=false );
	virtual WeaponType GetType() const;
	
	virtual ActionStopType GetActionStopType() const;
	virtual ActionStopType GetDieActionStopType() const;

protected:
	virtual void UpdateWorldAttackBox( float fTimePerSec, bool bSetNull );

protected:
	bool CheckCollisionLine( ioPlayEntity *pTarget, const D3DXVECTOR3 &vColPoint );

public:
	ioRunContactWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr );
	virtual ~ioRunContactWeapon();
};

inline ioRunContactWeapon* ToRunContactWeapon( ioWeapon *pWeapon )
{
	if( !pWeapon || pWeapon->GetType() != ioWeapon::WT_RUN_CONTACT )
		return NULL;

	return dynamic_cast< ioRunContactWeapon* >( pWeapon );
}

