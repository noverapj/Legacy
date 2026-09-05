#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioGaugeStealBuff : public ioBuff
{
protected:
	float m_fStealRate;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();

	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );

public:
	ioGaugeStealBuff();
	ioGaugeStealBuff( const ioGaugeStealBuff &rhs );
	virtual ~ioGaugeStealBuff();
};

inline ioGaugeStealBuff* ToGaugeStealBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_GAUGE_STEAL )
		return NULL;

	return dynamic_cast< ioGaugeStealBuff* >( pBuff );
}

