#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioRestorationGaugeBuff2 : public ioBuff
{
protected:
	CEncrypt<float> m_fRestorationGaugeRate;

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
	ioRestorationGaugeBuff2();
	ioRestorationGaugeBuff2( const ioRestorationGaugeBuff2 &rhs );
	virtual ~ioRestorationGaugeBuff2();
};

