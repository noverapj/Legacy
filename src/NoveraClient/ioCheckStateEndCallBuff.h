#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;
class ioEffect;

class ioCheckStateEndCallBuff : public ioBuff
{
public:
	enum BuffState
	{
		BS_WAIT,
		BS_END,
	};

protected:
	BuffState m_BuffState;
	int m_nCheckState;
	bool m_bSetWait;
	ioHashStringVec m_OwnerCallBuffList;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();

protected:
	void LoadOwnerCallBuffList( ioINILoader &rkLoader );
	void SetOwnerCallBuffList( ioBaseChar *pOwner );

protected:
	void CheckCharState( ioBaseChar *pOwner );
	bool IsEnableCall( ioBaseChar *pOwner );

public:
	bool IsWait();
	void SetCallState( ioBaseChar *pOwner );

public:
	ioCheckStateEndCallBuff();
	ioCheckStateEndCallBuff( const ioCheckStateEndCallBuff &rhs );
	virtual ~ioCheckStateEndCallBuff();
};

inline ioCheckStateEndCallBuff* ToCheckStateEndCallBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_CHECK_STATE_END_CALL_BUFF )
		return NULL;

	return dynamic_cast< ioCheckStateEndCallBuff* >( pBuff );
}