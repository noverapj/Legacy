#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioHitCountHoldBuff2 : public ioBuff
{
protected:
	CEncrypt<bool> m_bSetFloatState;
	FloatBuffInfo m_FloatBuffInfo;

	ioHashString m_HoldAnimation;
	ioHashString m_HoldAirAnimation;

	bool		m_bUseSuccubus;
	bool		m_bUseHades;
	bool		m_bMidnight;
	bool		m_bNotUseFloatState;

protected:
	CEncrypt<int> m_iMaxHitCount;
	int           m_iCurHitCount;

	CEncrypt<bool> m_bEnableTeamAttack;

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

	void SetFloatState( bool b );
	void SetNotUseFloatState( bool b);

public:
	void IncreaseHitCount();
	const bool IsTeamAttack() { return m_bEnableTeamAttack; }

protected:
	void CheckOwnerState();

public:
	ioHitCountHoldBuff2();
	ioHitCountHoldBuff2( const ioHitCountHoldBuff2 &rhs );
	virtual ~ioHitCountHoldBuff2();
};


inline ioHitCountHoldBuff2* ToHitCountHoldBuff2( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_HIT_COUNT_HOLD2 )
		return NULL;

	return dynamic_cast< ioHitCountHoldBuff2* >( pBuff );
}


