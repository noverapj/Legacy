#pragma once

#include "ioBuff.h"

class ioINILoader;
class ioBaseChar;

class ioDisableDashBuff : public ioBuff
{
protected:
	ioHashString m_DashFailAni;
	float m_fDashFailAniRate;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual void ProcessBuff( float fTimePerSec );

	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );

protected:
	virtual void SetAction();
	virtual void ProcessDelay( float fTimePerSec );

public:
	ioDisableDashBuff();
	ioDisableDashBuff( const ioDisableDashBuff &rhs );
	virtual ~ioDisableDashBuff();
};

