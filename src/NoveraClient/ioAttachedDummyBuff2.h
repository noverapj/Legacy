#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioAttachedDummyBuff2 : public ioBuff
{
protected:
	ioHashString m_szAttachedAni;
	ioHashString m_szFindDummy;
	D3DXVECTOR3 m_vDummyOffset;
	D3DXVECTOR3 m_vTeleportOffset;

	bool m_bDisableDownState;

	float m_fEndJumpPower;
	ioHashStringVec m_AddBuffList;
	ioHashStringVec m_RemoveBuffList;

protected:
	bool m_bIsTeleport;

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
	void SetAttachState( ioBaseChar *pOwner );
	bool CheckAttachPos( ioBaseChar *pOwner );
	bool CheckOwnerStateCheck( ioBaseChar *pOwner );
	void ProcessEnd( ioBaseChar *pOwner );

public:
	bool IsCharCollisionSkipState();
	void SetTeleportState();

protected:
	void LoadAddBuffList( ioINILoader &rkLoader );
	void LoadRemoveBuffList( ioINILoader &rkLoader );

public:
	ioAttachedDummyBuff2();
	ioAttachedDummyBuff2( const ioAttachedDummyBuff2 &rhs );
	virtual ~ioAttachedDummyBuff2();
};

inline ioAttachedDummyBuff2* ToAttachedDummyBuff2( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_ATTACHED_DUMMY2 )
		return NULL;

	return dynamic_cast< ioAttachedDummyBuff2* >( pBuff );
}