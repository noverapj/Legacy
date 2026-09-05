#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioStealthBuff : public ioBuff
{
public:
	virtual ioBuff* Clone();

public:
	virtual void ProcessBuff( float fTimePerSec );	

public:
	ioStealthBuff();
	ioStealthBuff( const ioStealthBuff &rhs );
	virtual ~ioStealthBuff();
};

