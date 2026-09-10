#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioDummyAttachBuff2 : public ioBuff
{
protected:
	enum BuffState
	{
		BS_None,
		BS_Loop,
		BS_CreateDummy,
	};

protected:
	BuffState m_BuffState;

protected:
	ioHashString m_szDummyName;
	D3DXVECTOR3 m_vDummyOffset;
	ioHashStringVec m_RemoveBuffList;
	
public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();
	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );
	virtual void ApplyExtraBuffInfo( SP2Packet &rkPacket );

protected:
	void LoadRemoveBuffList( ioINILoader &rkLoader );

protected:
	void CreateDummy( ioBaseChar *pOwner );
	void SetDummyPos( ioBaseChar *pOwner );

protected:
	bool SetOwnerState( ioBaseChar *pOwner );
	bool CheckOwnerState( ioBaseChar *pOwner );

protected:
	void ProcessEnd( ioBaseChar *pOwner );

public:
	ioDummyAttachBuff2();
	ioDummyAttachBuff2( const ioDummyAttachBuff2 &rhs );
	virtual ~ioDummyAttachBuff2();
};

inline ioDummyAttachBuff2* ToDummyAttachBuff2( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_DUMMY_ATTACH2 )
		return NULL;

	return dynamic_cast< ioDummyAttachBuff2* >( pBuff );
}