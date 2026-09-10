#pragma once

class ioINILoader;
class ioBaseChar;

#include "ioBuff.h"

class ioPushBackBuff : public ioBuff
{
protected:
	ioHashString m_PushBackAnimation;
	float m_fPushBackTimeRate;
	int m_iEtcStateCnt;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();

	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );

public:
	ioPushBackBuff();
	ioPushBackBuff( const ioPushBackBuff &rhs );
	virtual ~ioPushBackBuff();
};

