#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioStopMotionBuff : public ioBuff
{
public:
	enum StopMotionType
	{
		SMT_NORMAL,
		SMT_WEAK,
		SMT_LOOP_NORMAL,
		SMT_LOOP_WEAK,
	};

protected:
	ioHashString m_ChangeMaterial;
	StopMotionType m_StopMotionType;

	ioHashString m_LoopMotion;
	ioHashString m_CatchLoopMotion;

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
	ioStopMotionBuff();
	ioStopMotionBuff( const ioStopMotionBuff &rhs );
	virtual ~ioStopMotionBuff();
};

