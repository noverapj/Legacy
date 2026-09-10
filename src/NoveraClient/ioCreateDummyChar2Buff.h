#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioCreateDummyChar2Buff : public ioBuff
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
	virtual void CheckActiveCount();

protected:
	bool CheckOwnerStateCheck();
	bool CheckRestoreStateCheck();

public:
	void ApplyExtraBuffInfo( SP2Packet &rkPacket );

protected:
	void ProcessOwnerState();
	void ProcessReadyState( ioBaseChar *pOwner );
	void ProcessCreateDummyState( ioBaseChar *pOwner );
	void SetReadyState( ioBaseChar *pOwner );
	void SetCreateDummyState( ioBaseChar *pOwner );
	void CreateDummyChar( ioBaseChar *pOwner );

public:
	virtual bool IsProtected();
	bool CheckDontMoveState();

public:
	ioCreateDummyChar2Buff();
	ioCreateDummyChar2Buff( const ioCreateDummyChar2Buff &rhs );
	virtual ~ioCreateDummyChar2Buff();
};


inline ioCreateDummyChar2Buff* ToCreateDummyChar2Buff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_CREATE_DUMMY_CHAR2 )
		return NULL;

	return dynamic_cast< ioCreateDummyChar2Buff* >( pBuff );
}


