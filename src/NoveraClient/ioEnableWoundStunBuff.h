#pragma once

#include "ioBuff.h"

class ioINILoader;
class ioBaseChar;

class ioEnableWoundStunBuff : public ioBuff
{
protected:
	ioHashString m_StunAnimation;
	ioHashString m_CatchAnimation;

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
	virtual void SetContinueAction();
	virtual void ProcessDelay( float fTimePerSec );

public:
	ioEnableWoundStunBuff();
	ioEnableWoundStunBuff( const ioEnableWoundStunBuff &rhs );
	virtual ~ioEnableWoundStunBuff();
};

