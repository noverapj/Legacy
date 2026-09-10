#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioFrictionUpDownBuff : public ioBuff
{
protected:
	float m_fFrictionRate;
	bool m_bDisableDownState;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );

public :
	virtual ioBuff* Clone();

public:	
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();

	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );

protected:
	void CheckOwnerStateCheck();

public:
	ioFrictionUpDownBuff();
	ioFrictionUpDownBuff( const ioFrictionUpDownBuff &rhs );
	virtual ~ioFrictionUpDownBuff();
};

