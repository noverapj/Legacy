#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioChangeMotionBuff : public ioBuff
{
protected:
	ioHashString m_DelayAnimation;
	ioHashString m_MoveAnimation;

	ioHashString m_JumpReadyAni;
	ioHashString m_JumppingAni;
	ioHashString m_JumpEndAni;
	float m_fJumpEndAniRate;

	bool m_bOneTimeCheck;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual void ProcessBuff( float fTimePerSec );

	virtual void EndBuff();
	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );

public:
	ioChangeMotionBuff();
	ioChangeMotionBuff( const ioChangeMotionBuff &rhs );
	virtual ~ioChangeMotionBuff();
};

