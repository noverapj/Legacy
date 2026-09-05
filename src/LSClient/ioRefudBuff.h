#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioRefudBuff : public ioBuff
{
protected:
	CEncrypt<float> m_fExtraRate;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );

	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();

public:
	const float GetExtraRate() const { return m_fExtraRate; }

public:
	ioRefudBuff();
	ioRefudBuff( const ioRefudBuff &rhs );
	virtual ~ioRefudBuff();
};

inline ioRefudBuff* ToRefudBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_REFUD_GAUGE )
		return NULL;

	return dynamic_cast< ioRefudBuff* >( pBuff );
}

