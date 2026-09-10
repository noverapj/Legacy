#pragma once

#include "ioWeapon.h"

class ioDestroyCallWeapon : public ioWeapon
{
protected:
	ioEffect	*m_pWeaponEffect;
	ioHashString m_szWeaponRedEffect;
	ioHashString m_szWeaponBlueEffect;
	bool m_bStartInit;
	DWORD m_dwDelayTime;
	DWORD m_dwCheckDelayTime;
	D3DXVECTOR3 m_vMoveDir;
	bool m_bSetItemFunc;

public:
	static ActionStopType m_ActionStopType;
	static ActionStopType m_DieActionStopType;

public:
	virtual bool InitResource();
	virtual void DestroyResource( ioPlayStage *pStage=NULL );

	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual void SetWeaponDead();

	virtual void Process( float fTimePerSec, ioPlayStage *pStage );

	virtual WeaponType GetType() const;
	virtual ActionStopType GetActionStopType() const;
	virtual ActionStopType GetDieActionStopType() const;

	virtual void SetMoveDir( const D3DXVECTOR3 &vMoveDir );
	virtual D3DXVECTOR3 GetMoveDir();

protected:
	bool CreateWeaponEffect( const ioHashString &szFileName );
	void SetItemFunc();

public:
	ioDestroyCallWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr );
	virtual ~ioDestroyCallWeapon();
};

inline ioDestroyCallWeapon* ToDestroyCallWeapon( ioWeapon *pWeapon )
{
	if( !pWeapon || pWeapon->GetType() != ioWeapon::WT_DESTROY_CALL )
		return NULL;

	return dynamic_cast< ioDestroyCallWeapon* >( pWeapon );
}