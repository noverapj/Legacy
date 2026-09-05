#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioCallByStateBuff : public ioBuff
{
protected:
	bool m_bDisableDownState;
	bool m_bUseWeaponEquipMesh;

	bool m_bStartOnDelay;
	bool m_bStartOnRun;
	bool m_bStartOnDash;
	bool m_bStartOnJump;
	bool m_bStartOnAttack;
	bool m_bStartOnDefense;
	bool m_bStartOnWound;
	bool m_bStartOnGetUp;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();

protected:
	void SetAction();
	bool CheckOwnerStateCheck();
	bool CheckRestoreStateCheck();
	void CheckCallByState();
	void ProcessDelay();

public:
	ioCallByStateBuff();
	ioCallByStateBuff( const ioCallByStateBuff &rhs );
	virtual ~ioCallByStateBuff();
};


inline ioCallByStateBuff* ToCallByStateBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_CALL_BY_STATE )
		return NULL;

	return dynamic_cast< ioCallByStateBuff* >( pBuff );
}

