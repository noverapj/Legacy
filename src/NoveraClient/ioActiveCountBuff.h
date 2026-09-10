#pragma once

#include "ioBuff.h"


class ioBaseChar;
class ioINILoader;

class ioActiveCountBuff : public ioBuff
{
protected:
	CEncrypt<bool> m_bDisableDownState;
	CEncrypt<bool> m_bDisableDropDownState;

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
	void SetActiveCount(int iCount );

public:
	ioActiveCountBuff();
	ioActiveCountBuff( const ioActiveCountBuff &rhs );
	virtual ~ioActiveCountBuff();
};

inline ioActiveCountBuff* ToActiveCountBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_ACTIVE_COUNT )
		return NULL;

	return dynamic_cast< ioActiveCountBuff* > ( pBuff );
}


