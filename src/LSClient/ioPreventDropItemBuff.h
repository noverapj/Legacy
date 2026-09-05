#pragma once

#include "ioBuff.h"


class ioBaseChar;
class ioINILoader;

class ioPreventDropItemBuff : public ioBuff
{
protected:
	bool m_bDisableDownState;
	bool m_bDisableWoundState;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual void ProcessBuff( float fTimePerSec );	
	virtual void EndBuff();

	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );

protected:
	bool CheckOwnerStateCheck();

public:
	ioPreventDropItemBuff();
	ioPreventDropItemBuff( const ioPreventDropItemBuff &rhs );
	virtual ~ioPreventDropItemBuff();
};

inline ioPreventDropItemBuff* ToPreventDropItemBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_PREVENT_DROP_ITEM )
		return NULL;

	return dynamic_cast< ioPreventDropItemBuff* > ( pBuff );
}
