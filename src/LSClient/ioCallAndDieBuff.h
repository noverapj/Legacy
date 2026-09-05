#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioCallAndDieBuff : public ioBuff
{
protected:
	CEncrypt<bool>	m_bDisableDownState;
	CEncrypt<bool>	m_bDisableDropDownState;
	CEncrypt<bool>	m_bUseWeaponEquipMesh;
	CEncrypt<bool>	m_bEscapeCatchState;

	ioHashString    m_stDieCheckBuff;
	ioHashString	m_stDieAndCallBuff;

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

	void CheckBuffState();

public:
	ioCallAndDieBuff();
	ioCallAndDieBuff( const ioCallAndDieBuff &rhs );
	virtual ~ioCallAndDieBuff();
};


inline ioCallAndDieBuff* ToCallAndDieBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_CALL_AND_DIE )
		return NULL;

	return dynamic_cast< ioCallAndDieBuff* >( pBuff );
}


