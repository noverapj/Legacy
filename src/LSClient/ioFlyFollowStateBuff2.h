#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioFlyFollowStateBuff2 : public ioBuff
{
private:
	enum BuffState
	{
		BS_NONE,
		BS_MOVE,
		BS_WAIT,
		BS_END,
	};

private:
	BuffState		m_BuffState;
	ioHashString	m_MoveAni;
	float			m_fMoveAniRate;
	DWORD			m_dwMoveDuration;
	DWORD			m_dwMovingEndTime;

	ioHashString	m_WaitAni;
	float			m_fWaitAniRate;
	DWORD			m_dwWaitDuraion;
	DWORD			m_dwWaitEndTime;
	
	D3DXVECTOR3		m_vAxisScale;
	D3DXVECTOR3		m_vBeforeScale;

	D3DXVECTOR3		m_vLookAt;
	D3DXVECTOR3		m_vNorLookAt;
	float			m_fFollowSpeed;

	bool			m_bSetEndJump;
	float			m_fEndJumpPower;

	bool			m_bCharCol;


private:
	bool CheckLive();

	void SetBuffMoveState( ioBaseChar *pOwner );
	void ProcessMoveState( float fTimePerSec );
	void ProcessMove( ioBaseChar *pOnwer, float fTimePerSec );

	void SetBuffWaitState( ioBaseChar *pOwner );
	void ProcessWaitState( float fTimePerSec );

	void ClearBuffState();
	
public:
	bool IsCharCollisionSkipState();
	

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();

	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );

public:
	ioFlyFollowStateBuff2();
	ioFlyFollowStateBuff2( const ioFlyFollowStateBuff2 &rhs );
	virtual ~ioFlyFollowStateBuff2();
};


inline ioFlyFollowStateBuff2* ToFlyFollowBuff2( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_FLY_FOLLOW_STATE2 )
		return NULL;

	return dynamic_cast< ioFlyFollowStateBuff2* >( pBuff );
}


