#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioCreateBuff : public ioBuff
{
	ioHashStringVec m_vBuffList;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();

protected:
	bool CreateBuffForCreator();

public:
	ioCreateBuff();
	ioCreateBuff( const ioCreateBuff &rhs );
	virtual ~ioCreateBuff();
};


inline ioCreateBuff* ToCreateBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_CREATE_BUFF )
		return NULL;

	return dynamic_cast< ioCreateBuff* >( pBuff );
}


