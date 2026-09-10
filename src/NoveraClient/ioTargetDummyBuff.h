#pragma once
#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioTargetDummyBuff : public ioBuff
{
public:
	enum SyncState
	{
		SS_CREATE,
		SS_SET_CHAR,
	};
protected:
	bool m_bDisableDownState;

	DWORD m_dwSlerpTime;
	float m_fAlphaRate;
	bool m_bHideMiniMap;
	bool m_bCreatedDummy;
	bool m_bSetChar;
	CEncrypt<bool> m_bHideFootDustEffect;

	DummyCharLoadInfo m_DummyChar;
	DummyCharCreateInfo m_CreateDummy;
	ioHashString m_DestroyDummyChar;

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

protected:
	void CreateDummyChar( ioBaseChar *pOwner );
	void CheckDummyChar();
	void SetCharState( ioBaseChar *pOwner, bool bSend );

public:
	void ApplyExtraBuffInfo( SP2Packet &rkPacket );

public:
	float GetAlphaRate() { return m_fAlphaRate; }

public:
	ioTargetDummyBuff();
	ioTargetDummyBuff( const ioTargetDummyBuff &rhs );
	virtual ~ioTargetDummyBuff();
};

inline ioTargetDummyBuff* ToTargetDummyBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_TARGET_DUMMY )
		return NULL;

	return dynamic_cast< ioTargetDummyBuff* >( pBuff );
}