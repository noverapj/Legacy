#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioGravityUpDownBuff : public ioBuff
{
protected:
	float m_fGravityRate;
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
	ioGravityUpDownBuff();
	ioGravityUpDownBuff( const ioGravityUpDownBuff &rhs );
	virtual ~ioGravityUpDownBuff();
};

