#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioAttachedCreatorCharBuff : public ioBuff
{
protected:
	enum
	{
		BS_End,
	};

protected:
	ioHashString m_szAttachedAni;
	float m_fAttachedAniRate;
	D3DXVECTOR3 m_vAttachedOffset;
	bool m_bDisableDownState;
	ioHashStringVec m_OwnerBuffList;
	bool m_bSetEndJump;
	float m_fEndJumpPower;
	bool m_bColSkip;
	bool m_bCreatorState;

protected:
	DWORD m_dwMotionEndTime;

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
	void LoadOwnerBuffList( ioINILoader &rkLoader );
	void SetOwnerBuffList( ioBaseChar *pOwner );

protected:
	void SetAttachState( ioBaseChar *pOwner );
	void SetEndState( ioBaseChar *pOwner );

protected:
	bool CheckAttachPos( ioBaseChar *pOwner );
	bool CheckOwnerStateCheck( ioBaseChar *pOwner );
	void ProcessEnd( ioBaseChar *pOwner );

public:
	bool IsCharCollisionSkipState();

public:
	ioAttachedCreatorCharBuff();
	ioAttachedCreatorCharBuff( const ioAttachedCreatorCharBuff &rhs );
	virtual ~ioAttachedCreatorCharBuff();
};

inline ioAttachedCreatorCharBuff* ToAttachedCreatorCharBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_ATTACHED_CREATOR_CHAR )
		return NULL;

	return dynamic_cast< ioAttachedCreatorCharBuff* >( pBuff );
}