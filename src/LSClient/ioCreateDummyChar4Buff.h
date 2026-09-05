#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioCreateDummyChar4Buff : public ioBuff
{
public:
	enum BuffState
	{
		BS_READY,
		BS_CREATE_DUMMY,
	};

protected:
	BuffState m_BuffState;

	bool m_bDisableDownState;
	bool m_bUseWeaponEquipMesh;

	ioHashString m_szActionAnimation;
	CEncrypt<float> m_fActionAniRate;

	vDummyCharLoadInfoList m_DummyCharList;
	ioHashStringVec m_vDestroyDummyCharList;

	DWORD m_dwMotionEndTime;
	DWORD m_dwFireMotionEndTime;
	DWORD m_dwCreateTime;

	int   m_nDummyIndex;

	bool	m_bSendCreateDummy;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();
	virtual void GetBuffInfo( SP2Packet &rkPacket );
	virtual void ApplyBuffInfo( SP2Packet &rkPacket );

public:
	void ApplyExtraBuffInfo( SP2Packet &rkPacket );

protected:
	void CreateDummyChar( ioBaseChar *pOwner );

public:
	bool IsProtected();

public:
	ioCreateDummyChar4Buff();
	ioCreateDummyChar4Buff( const ioCreateDummyChar4Buff &rhs );
	virtual ~ioCreateDummyChar4Buff();
};


inline ioCreateDummyChar4Buff* ToCreateDummyChar4Buff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_CREATE_DUMMY_CHAR4 )
		return NULL;

	return dynamic_cast< ioCreateDummyChar4Buff* >( pBuff );
}


