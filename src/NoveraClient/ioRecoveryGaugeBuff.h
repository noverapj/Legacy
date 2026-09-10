#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioRecoveryGaugeBuff : public ioBuff
{
protected:
	float m_fRate;
	bool m_bRemoveBuffByDrop;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual void StartBuffWithAniTime( ioBaseChar *pOwner, DWORD dwAniTime );
	virtual void ProcessBuff( float fTimePerSec );

	virtual void EndBuff();
	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );

public:
	ioRecoveryGaugeBuff();
	ioRecoveryGaugeBuff( const ioRecoveryGaugeBuff &rhs );
	virtual ~ioRecoveryGaugeBuff();
};

