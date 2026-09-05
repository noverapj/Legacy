#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioCreateAreaWeaponBuff : public ioBuff
{
protected:
	ioHashString m_szFindAreaWeapon;
	ioHashString m_szCreateAreaWeapon;
	DWORD m_dwCreateAreaWeaponTime;
	bool  m_bKeyControl;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();
	virtual void ApplyExtraBuffInfo( SP2Packet &rkPacket );

protected:
	void CreateAreaWeapon( ioBaseChar *pOwner );
		
public:
	ioCreateAreaWeaponBuff();
	ioCreateAreaWeaponBuff( const ioCreateAreaWeaponBuff &rhs );
	virtual ~ioCreateAreaWeaponBuff();
};

inline ioCreateAreaWeaponBuff* ToCreateAreaWeaponBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_CREATE_AREA_WEAPON )
		return NULL;

	return dynamic_cast< ioCreateAreaWeaponBuff* >( pBuff );
}