#pragma once

#include "ioWeapon.h"

class ioAttachAeraWeapon : public ioWeapon
{
protected:
	ioEffect	*m_pWeaponEffect;
	ioHashString m_szWeaponRedEffect;
	ioHashString m_szWeaponBlueEffect;
	bool m_bStartInit;

	bool m_bOwnerAttach;

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

protected:
	bool CreateWeaponEffect( const ioHashString &szFileName );
	void ProcessAttachMove( float fTimePerSec, ioPlayStage *pStage );

public:
	void CheckOwnerAttach( bool bAttach );

public:
	ioAttachAeraWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr );
	virtual ~ioAttachAeraWeapon();
};

inline ioAttachAeraWeapon* ToAttachAeraWeapon( ioWeapon *pWeapon )
{
	if( !pWeapon || pWeapon->GetType() != ioWeapon::WT_ATTACH_AREA )
		return NULL;

	return dynamic_cast< ioAttachAeraWeapon* >( pWeapon );
}