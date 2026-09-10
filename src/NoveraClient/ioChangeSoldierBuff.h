#pragma once	

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioChangeSoldierBuff : public ioBuff
{
protected:
	bool m_bDisableDownState;
	bool m_bUseWeaponEquipMesh;

	ioHashString	m_EnableEffect;
	DWORD			m_dwEnableEffectID;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();

protected:
	void ProcessPassive();

protected:
	bool CheckOwnerStateCheck();
	bool CheckRestoreStateCheck();

public:
	bool	EnableChangeSoldier();
	bool	IsWoundedState();

public:
	ioChangeSoldierBuff();
	ioChangeSoldierBuff( const ioChangeSoldierBuff &rhs );
	virtual ~ioChangeSoldierBuff();
};


inline ioChangeSoldierBuff* ToChangeSoldierBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_CHANGE_SOLDIER )
		return NULL;

	return dynamic_cast< ioChangeSoldierBuff* >( pBuff );
}


