#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioActiveCountBuff2 : public ioBuff
{
protected:
	CEncrypt<bool> m_bDisableDownState;
	CEncrypt<bool> m_bDisableDropDownState;
	bool m_bSetUseGauge;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual void ProcessBuff( float fTimePerSec );	
	virtual void EndBuff();

	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );
	virtual void CheckActiveCount();

protected:
	bool CheckOwnerStateCheck();
	bool CheckRestoreStateCheck();

public:
	ioActiveCountBuff2();
	ioActiveCountBuff2( const ioActiveCountBuff2 &rhs );
	virtual ~ioActiveCountBuff2();
};

inline ioActiveCountBuff2* ToActiveCountBuff2( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_ACTIVE_COUNT2 )
		return NULL;

	return dynamic_cast< ioActiveCountBuff2* > ( pBuff );
}


