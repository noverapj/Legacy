#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioTrapEyeBuff : public ioBuff
{
public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();	

public:
	ioTrapEyeBuff();
	ioTrapEyeBuff( const ioTrapEyeBuff &rhs );
	virtual ~ioTrapEyeBuff();
};


inline ioTrapEyeBuff* ToTrapEyeBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_TRAP_EYE_BUFF )
		return NULL;

	return dynamic_cast< ioTrapEyeBuff* >( pBuff );
}


