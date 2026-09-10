#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioCheckJumpBuff : public ioBuff
{
protected:
	ioHashStringVec m_OwnerBuffList;
	int m_nMaxCnt;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();

protected:
	void LoadOwnerBuffList( ioINILoader &rkLoader );
	void SetOwnerBuffList( ioBaseChar *pOwner );
	bool CheckBuff( ioBaseChar *pOwner );

public:
	ioCheckJumpBuff();
	ioCheckJumpBuff( const ioCheckJumpBuff &rhs );
	virtual ~ioCheckJumpBuff();
};

inline ioCheckJumpBuff* ToCheckJumpBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_CHECK_JUMP )
		return NULL;

	return dynamic_cast< ioCheckJumpBuff* >( pBuff );
}