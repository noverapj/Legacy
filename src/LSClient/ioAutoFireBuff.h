#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioAutoFireBuff : public ioBuff
{
protected:
	bool m_bDisableDownState;
	bool m_bUseWeaponEquipMesh;
	
	CEncrypt<bool>	m_bFireOnFly;
	DWORD			m_dwCheckFireTime;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();

protected:
	bool CheckOwnerStateCheck();
	bool CheckRestoreStateCheck();

protected:
	void AutoCallAttribute();
	bool CheckFlyState();

public:
	ioAutoFireBuff();
	ioAutoFireBuff( const ioAutoFireBuff &rhs );
	virtual ~ioAutoFireBuff();
};


inline ioAutoFireBuff* ToAutoFireBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_AUTO_FIRE )
		return NULL;

	return dynamic_cast< ioAutoFireBuff* >( pBuff );
}


