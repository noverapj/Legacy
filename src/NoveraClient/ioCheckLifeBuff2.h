#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioCheckLifeBuff2 : public ioBuff
{
protected:
	ioHashString m_szCheckBuff;
	float m_fCheckDist;
	ioHashString m_szCreaterBuff;
	bool m_bSetCreaterBuff;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();

protected:
	bool IsCheckBuff();

public:
	ioCheckLifeBuff2();
	ioCheckLifeBuff2( const ioCheckLifeBuff2 &rhs );
	virtual ~ioCheckLifeBuff2();
};


inline ioCheckLifeBuff2* ToCheckLifeBuff2( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_CHECK_LIFE2 )
		return NULL;

	return dynamic_cast< ioCheckLifeBuff2* >( pBuff );
}


