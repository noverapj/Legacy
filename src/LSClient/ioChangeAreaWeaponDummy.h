#pragma once
#include "iodummychar.h"

class ioChangeAreaWeaponDummy :	public ioDummyChar
{
protected:
	enum	DummyState
	{
		DS_START	= 100,
		DS_DELAY,
		DS_DIE
	};

	enum	ReactionType
	{
		RT_OWNER,
		RT_TEAM,
		RT_ENEMY,
		RT_ALL
	};

protected:
	DummyState	m_DummyState;

	DWORD	m_AreaWeaponFirstIdx;
	DWORD	m_AreaWeaponSecondIdx;
	ioHashString	m_szAreaWeaponFirst;
	ioHashString	m_szAreaWeaponSecond;

	ReactionType	m_ReactionType;
	bool	m_bIsReactionWeapon;
	DWORD	m_dwReactionWeaponIdx;

	bool	m_bNotUseTranslateByConveyer;

public:
	virtual	void	LoadProperty( ioINILoader &rkLoader );

public:
	void	ProcessStartState( ioBaseChar* pOwner );
	void	ProcessDelayState( ioBaseChar* pOwner );

public:
	virtual	void	ApplyDummyCharSync( ioPlayStage *pStage, SP2Packet &rkPacket );

	virtual	void	SetDieState( bool bEffect, bool bWoundedDie = false );

	virtual	void	ProcessState( float fTimerPerSec, DWORD dwPreTime );

	virtual	void	TranslateByConveyer( const D3DXVECTOR3 &vMove );

	virtual bool	IsCollisionAvailableWeapon( ioWeapon *pWeapon );
	virtual	ApplyWeaponExp	ApplyWeapon( ioWeapon *pWeapon );

protected:
	void	SetDummyDieState( ioBaseChar* pOwner );

public:
	ioChangeAreaWeaponDummy( ioEntityGroup *pGrp, ioPlayMode *pMode );
	virtual ~ioChangeAreaWeaponDummy(void);
};

inline	ioChangeAreaWeaponDummy*	ToChangeAreaWeaponDummy( ioDummyChar *pDummy )
{
	if( !pDummy || pDummy->GetType() != DCT_CHANGE_AREA_WEAPON )
		return	NULL;

	return	dynamic_cast< ioChangeAreaWeaponDummy* >( pDummy );
}