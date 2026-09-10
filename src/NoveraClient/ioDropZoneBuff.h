#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioDropZoneBuff : public ioBuff
{
protected:
	ioHashString m_StunAnimation;
	ioHashString m_CatchAnimation;

	int m_iEtcStateCnt;

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

public:
	ioDropZoneBuff();
	ioDropZoneBuff( const ioDropZoneBuff &rhs );
	virtual ~ioDropZoneBuff();
};


inline ioDropZoneBuff* ToDropZoneBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_DROP_ZONE )
		return NULL;

	return dynamic_cast< ioDropZoneBuff* >( pBuff );
}


