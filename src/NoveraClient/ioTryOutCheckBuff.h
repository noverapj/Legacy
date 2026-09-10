#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioTryOutCheckBuff : public ioBuff
{
protected:
	CEncrypt<bool> m_bSetFloatState;
	FloatBuffInfo m_FloatBuffInfo;

	ioHashString m_HoldAnimation;
	ioHashString m_HoldAirAnimation;

	CEncrypt<DWORD>       m_dwKeyInputStartTime;
	CEncrypt<DWORD>		  m_dwKeyInputEndTime;
	bool		m_bKeyInputed;
	int			m_nMaxClickCnt;

	bool		m_bSafe;

protected:
	CEncrypt<int> m_iMaxHitCount;
	int           m_iCurHitCount;

	CEncrypt<bool> m_bEnableTeamAttack;

	bool		  m_bLeftKeyTime;
	int			  m_nClickCnt;

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

	virtual void ApplyExtraBuffInfo(  SP2Packet &rkPacket  );
	void	SendSafeInfo();

public:
	void IncreaseHitCount();
	const bool IsTeamAttack() { return m_bEnableTeamAttack; }

	bool IsShowCheckKey() const;

protected:
	void CheckOwnerState();

public:
	ioTryOutCheckBuff();
	ioTryOutCheckBuff( const ioTryOutCheckBuff &rhs );
	virtual ~ioTryOutCheckBuff();
};


inline ioTryOutCheckBuff* ToTryOutCheckBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_TRY_OUT_CHHECK )
		return NULL;

	return dynamic_cast< ioTryOutCheckBuff* >( pBuff );
}


