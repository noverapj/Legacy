#pragma once

#include "ioBuff.h"


class ioBaseChar;
class ioINILoader;

class ioReverseDirectionBuff : public ioBuff
{
public:
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual void ProcessBuff( float fTimePerSec );	
	virtual void EndBuff();

	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );

public:
	ioReverseDirectionBuff();
	ioReverseDirectionBuff( const ioReverseDirectionBuff &rhs );
	virtual ~ioReverseDirectionBuff();
};

