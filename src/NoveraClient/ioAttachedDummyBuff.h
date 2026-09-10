#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioAttachedDummyBuff : public ioBuff
{
protected:
	ioHashString m_szAttachedAni;
	ioHashString m_szFindDummy;
	D3DXVECTOR3 m_vDummyOffset;

	bool m_bDisableDownState;

	ioHashStringVec m_OwnerBuffList;
	ioHashStringVec m_MapCollisionBuff;

	bool m_bSetEndJump;
	float m_fEndJumpPower;

	bool m_bColSkip;

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

protected:
	void SetAttachState( ioBaseChar *pOwner );
	bool CheckAttachPos( ioBaseChar *pOwner );
	bool CheckOwnerStateCheck( ioBaseChar *pOwner );
	void ProcessEnd( ioBaseChar *pOwner );

public:
	bool IsCharCollisionSkipState();
	void CheckWallCollision();

public:
	ioAttachedDummyBuff();
	ioAttachedDummyBuff( const ioAttachedDummyBuff &rhs );
	virtual ~ioAttachedDummyBuff();
};

inline ioAttachedDummyBuff* ToAttachedDummyBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_ATTACHED_DUMMY )
		return NULL;

	return dynamic_cast< ioAttachedDummyBuff* >( pBuff );
}