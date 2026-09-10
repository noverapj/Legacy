#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioLimitMoveBuff : public ioBuff
{
protected:
	bool m_bDisableDownState;

	float m_fExtraSpeed;

	float m_fAddSpeed;
	float m_fCurSpeed;

	ioHashString m_DelayAnimation;
	ioHashString m_MoveAnimation;

	ioHashString m_JumpReadyAni;
	ioHashString m_JumppingAni;
	ioHashString m_JumpEndAni;
	float m_fJumpEndAniRate;

	bool m_bSetDontCheckUseBuff;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual void ProcessBuff( float fTimePerSec );

	virtual void EndBuff();
	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );

	virtual void GetBuffInfo( SP2Packet &rkPacket );
	virtual void ApplyBuffInfo( SP2Packet &rkPacket );

	bool IsCheckUseBuff() const;

protected:
	void CheckOwnerStateCheck();

public:
	ioLimitMoveBuff();
	ioLimitMoveBuff( const ioLimitMoveBuff &rhs );
	virtual ~ioLimitMoveBuff();
};

inline ioLimitMoveBuff* ToLimitMoveBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_LIMIT_MOVE )
		return NULL;

	return dynamic_cast< ioLimitMoveBuff* >( pBuff );
}