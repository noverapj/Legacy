#pragma once


#include "ioWeapon.h"

class ioEffect;

class ioLimitAttachWeapon : public ioWeapon
{
protected:
	ioEffect *m_pFireBall;
	D3DXVECTOR3 m_vMoveDir;

public:
	static ActionStopType m_ActionStopType;
	static ActionStopType m_DieActionStopType;

protected:
	bool CreateFireBall( const ioHashString &szFileName );

public:
	virtual bool InitResource();
	virtual void LoadProperty( ioINILoader &rkLoader );

	virtual void SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage );

	virtual void Process( float fTimePerSec, ioPlayStage *pStage );
	virtual void DestroyResource( ioPlayStage *pStage=NULL );

public:
	virtual D3DXVECTOR3 GetAttackDir( const ioPlayEntity *pEntity ) const;
	virtual const ioOrientBox& GetLocalAttackBox() const;
	virtual WeaponType GetType() const;
	virtual ActionStopType GetActionStopType() const;
	virtual ActionStopType GetDieActionStopType() const;

	virtual bool IsCollision( ioPlayEntity *pEntity, bool bMapCollision=false );

protected:
	virtual const WeaponAttribute* GetAttribute() const;
	virtual void UpdateWorldAttackBox( float fTimePerSec, bool bSetNull );

protected:
	void UpdateCurPosition( ioBaseChar *pOwner );

public:
	ioLimitAttachWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr );
	virtual ~ioLimitAttachWeapon();
};

inline ioLimitAttachWeapon* ToLimitAttachWeapon( ioWeapon *pWeapon )
{
	if( !pWeapon || pWeapon->GetType() != ioWeapon::WT_LIMIT_ATTACH )
		return NULL;

	return dynamic_cast< ioLimitAttachWeapon* >( pWeapon );
}

