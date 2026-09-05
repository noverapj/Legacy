#pragma once


#include "ioWeapon.h"
#include "ioAttackAttribute.h"

class ioEffect;

class ioBlizzardWeapon : public ioWeapon
{
protected:
	ioEffect *m_pBlizzard;

public:
	static ActionStopType m_ActionStopType;
	static ActionStopType m_DieActionStopType;

public:
	float GetMoveSpeed();

protected:
	bool CreateBlizzard( const ioHashString &szFileName );

public:
	virtual bool InitResource();
	virtual void DestroyResource( ioPlayStage *pStage=NULL );
	virtual void LoadProperty( ioINILoader &rkLoader );

	virtual void SetLiveTime( DWORD dwTime );

	virtual void Process( float fTimePerSec, ioPlayStage *pStage );

public:
	virtual const ioOrientBox& GetLocalAttackBox() const;

	virtual WeaponType GetType() const;
	virtual ActionStopType GetActionStopType() const;
	virtual ActionStopType GetDieActionStopType() const;

	virtual bool IsCollision( ioPlayEntity *pEntity, bool bMapCollision=false );

protected:
	virtual void UpdateWorldAttackBox( float fTimePerSec, bool bSetNull );

protected:
	bool CheckCollisionLine( ioPlayEntity *pTarget, const D3DXVECTOR3 &vColPoint );

public:
	ioBlizzardWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr );
	virtual ~ioBlizzardWeapon();
};

inline ioBlizzardWeapon* ToBlizzardWeapon( ioWeapon *pWeapon )
{
	if( !pWeapon || pWeapon->GetType() != ioWeapon::WT_BLIZZARD )
		return NULL;

	return dynamic_cast< ioBlizzardWeapon* >( pWeapon );
}

