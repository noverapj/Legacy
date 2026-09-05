#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioTryOutCheckBuff2 : public ioBuff
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

	int			m_nRandomMax;
	int			m_nRandomMin;

protected:
	CEncrypt<int> m_iMaxHitCount;
	int           m_iCurHitCount;

	CEncrypt<bool> m_bEnableTeamAttack;

	bool		  m_bLeftKeyTime;
	int			  m_nClickCnt;

	bool		  m_bFree;
	bool		  m_bSafeCheck;

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

	void ResetCount();

public:
	void IncreaseHitCount();
	const bool IsTeamAttack() { return m_bEnableTeamAttack; }

	bool IsShowCheckKey() const;

protected:
	void CheckOwnerState();

public:
	ioTryOutCheckBuff2();
	ioTryOutCheckBuff2( const ioTryOutCheckBuff2 &rhs );
	virtual ~ioTryOutCheckBuff2();
};


inline ioTryOutCheckBuff2* ToTryOutCheckBuff2( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_TRY_OUT_CHHECK2 )
		return NULL;

	return dynamic_cast< ioTryOutCheckBuff2* >( pBuff );
}


