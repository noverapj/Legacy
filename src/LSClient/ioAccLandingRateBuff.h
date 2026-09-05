#pragma once
#include "iobuff.h"
class ioAccLandingRateBuff :
	public ioBuff
{
protected:
	float m_fLandingRate;
	float m_fExtendLandingRate;

public:
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();

	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );
public:
	ioAccLandingRateBuff(void);
	ioAccLandingRateBuff( const ioAccLandingRateBuff& rhs );
	virtual ~ioAccLandingRateBuff(void);
};

