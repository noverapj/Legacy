#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;
class ioEffect;

class ioCreateDummyChar8Buff : public ioBuff
{
public:
	enum BuffState
	{
		BS_CreateDummy,
		BS_START,
	};
	
protected:
	ioHashString m_szDummyName;
	D3DXVECTOR3 m_vDummyOffset;
	ioHashStringVec m_OwnerBuffList;

protected:
	BuffState m_BuffState;
	
public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();
	virtual void ApplyExtraBuffInfo( SP2Packet &rkPacket );

protected:
	void CreateDummy( ioBaseChar *pOwner );

public:
	ioCreateDummyChar8Buff();
	ioCreateDummyChar8Buff( const ioCreateDummyChar8Buff &rhs );
	virtual ~ioCreateDummyChar8Buff();
};

inline ioCreateDummyChar8Buff* ToCreateDummyCharBuff8( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_CREATE_DUMMY_CHAR8 )
		return NULL;

	return dynamic_cast< ioCreateDummyChar8Buff* >( pBuff );
}