#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioFlyFollowStateBuff : public ioBuff
{
protected:
	ioHashString	m_MoveAni;
	float			m_fAniRate;

	D3DXVECTOR3 m_vOffSet;
	float m_fFollowSpeed;
	float m_fEndJumpPower;
	bool  m_bNotJump;

	float m_fCreatorCheckLength;
	bool m_bSendJumpNet;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

	void SetEndJump( bool b );

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();

	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );

public:
	ioFlyFollowStateBuff();
	ioFlyFollowStateBuff( const ioFlyFollowStateBuff &rhs );
	virtual ~ioFlyFollowStateBuff();
};


inline ioFlyFollowStateBuff* ToFlyFollowBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_FLY_FOLLOW_STATE )
		return NULL;

	return dynamic_cast< ioFlyFollowStateBuff* >( pBuff );
}


