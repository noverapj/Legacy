#pragma once

#include "ioBuff.h"


class ioBaseChar;
class ioINILoader;

class ioBreakFallBuff : public ioBuff
{
protected:
	BreakFallInfo m_BreakFallInfo;

	ioHashString m_BreakFallLandAni;
	float m_fBreakFallLandAniRate;

	float m_fEnableHeightMin;
	float m_fEnableHeightMax;

	CEncrypt<float> m_fJumppingPowerRate;
	CEncrypt<bool> m_bInitialGauge;
	CEncrypt<float> m_fBuffNeedGauge;

	bool m_bBreakFallProtect;
	bool m_bBreakFallState;
	DWORD m_dwBreakFallLandingProtectEndTime;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual void ProcessBuff( float fTimePerSec );	
	virtual void EndBuff();

	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );

	virtual bool IsProtected() const;

protected:
	void SetBreakFallState();
	void SetBreakFallLanding();
	void CheckBreakFall();
	void CheckBreakFallLanding();

public:
	BreakFallInfo GetBreakFallInfo() const;

public:
	ioBreakFallBuff();
	ioBreakFallBuff( const ioBreakFallBuff &rhs );
	virtual ~ioBreakFallBuff();
};

inline ioBreakFallBuff* ToBreakFall( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_BREAK_FALL )
		return NULL;

	return dynamic_cast< ioBreakFallBuff* >( pBuff );
}

inline const ioBreakFallBuff* ToBreakFallConst( const ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_BREAK_FALL )
		return NULL;

	return dynamic_cast< const ioBreakFallBuff* >( pBuff );
}

