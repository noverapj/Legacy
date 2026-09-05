#pragma once

#include "ioBuff.h"


class ioBaseChar;
class ioINILoader;

class ioDownJumppingBuff : public ioBuff
{
protected:
	CEncrypt<float> m_fJumppingPowerRate;
	CEncrypt<DWORD> m_dwEnableTime;

	CEncrypt<bool> m_bInitialGauge;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual void ProcessBuff( float fTimePerSec );	
	virtual void EndBuff();

	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );

protected:
	void CheckDownJumpping();

public:
	ioDownJumppingBuff();
	ioDownJumppingBuff( const ioDownJumppingBuff &rhs );
	virtual ~ioDownJumppingBuff();
};

