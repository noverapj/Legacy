#pragma once
#include "iodummychar.h"

class ioPlayStage;
class ioPlayMode;
class ioINILoader;
class SP2Packet;

class ioShieldDomeDummy :	public ioDummyChar
{
protected:
	bool	m_bIsMoving;

	WeaponInfo	m_ShieldWeaponInfo;

	ioHashString	m_szAreaWeaponName;
	ioAreaWeapon*	m_pAreaWeapon;
	D3DXVECTOR3		m_vCreateWeaponOffset;

public:
	virtual	void	LoadProperty( ioINILoader &rkLoader );

protected:
	virtual	void	ProcessStartState( float fTimePerSec );

	virtual	void	ProcessPos( float fTimePerSec );

protected:
	void	CreateAreaWeapon();

public:
	ioShieldDomeDummy( ioEntityGroup *pGrp, ioPlayMode *pMode );
	virtual ~ioShieldDomeDummy(void);
};

inline	ioShieldDomeDummy*	ToShieldDomeDummy( ioDummyChar *pDummy )
{
	if( !pDummy || pDummy->GetType() != DCT_SHIELD_DOME )
		return	NULL;

	return dynamic_cast< ioShieldDomeDummy* >( pDummy );
}