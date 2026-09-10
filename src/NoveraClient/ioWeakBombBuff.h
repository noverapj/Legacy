#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioWeakBombBuff : public ioBuff
{
protected:
	float m_fWeakRate;
	BombType m_iBombType;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );

	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();

public:
	ioWeakBombBuff();
	ioWeakBombBuff( const ioWeakBombBuff &rhs );
	virtual ~ioWeakBombBuff();
};

