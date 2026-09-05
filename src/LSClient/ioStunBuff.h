#pragma once

#include "ioBuff.h"

class ioINILoader;
class ioBaseChar;

class ioStunBuff : public ioBuff
{
protected:
	ioHashString m_StunAnimation;
	ioHashString m_CatchAnimation;

	ioHashString m_CMSMAnimation;
	ioHashString m_CMSFAnimation;
	ioHashString m_CFSMAnimation;
	ioHashString m_CFSFAnimation;

	DWORD m_BuffEffectID;

	int m_iEtcStateCnt;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();

	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );

protected:
	virtual void SetAction();
	virtual void ProcessDelay( float fTimePerSec );

public:
	ioStunBuff();
	ioStunBuff( const ioStunBuff &rhs );
	virtual ~ioStunBuff();
};

