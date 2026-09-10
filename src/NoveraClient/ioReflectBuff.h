#pragma once

#include "ioBuff.h"


class ioBaseChar;
class ioINILoader;

class ioReflectBuff : public ioBuff
{
protected:
	ReflectInfo m_ReflectInfo;

	bool m_bDisableDownState;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual void ProcessBuff( float fTimePerSec );	
	virtual void EndBuff();

	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );

protected:
	void CheckOwnerStateCheck();

public:
	ioReflectBuff();
	ioReflectBuff( const ioReflectBuff &rhs );
	virtual ~ioReflectBuff();
};

