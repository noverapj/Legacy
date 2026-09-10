#pragma once


#include "ioBuff.h"

class ioINILoader;
class ioBaseChar;

class ioSpecialGaugeBuff : public ioBuff
{
protected:
	SpecialGaugeType m_SpecialGaugeType;
	CEncrypt<float> m_fSpecialGauge;

public:
	virtual ioBuff* Clone();
	virtual void LoadProperty( ioINILoader &rkLoader );

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual void ProcessBuff( float fTimePerSec );
	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );

public:
	ioSpecialGaugeBuff();
	ioSpecialGaugeBuff( const ioSpecialGaugeBuff &rhs );
	virtual ~ioSpecialGaugeBuff();
};

