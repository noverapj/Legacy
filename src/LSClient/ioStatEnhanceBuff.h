#pragma once

#include "ioBuff.h"


class ioBaseChar;
class ioINILoader;

class ioStatEnhanceBuff : public ioBuff
{
protected:
	Stat m_EnhanceStat;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual void EndBuff();

	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );

public:
	ioStatEnhanceBuff();
	ioStatEnhanceBuff( const ioStatEnhanceBuff &rhs );
	virtual ~ioStatEnhanceBuff();
};

