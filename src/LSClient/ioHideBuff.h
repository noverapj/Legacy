#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioHideBuff : public ioBuff
{
protected:
	float m_fHideRate;
	float m_fAddSpeed;
	float m_fCurSpeed;
	DWORD m_dwHidingTime;

	bool m_bSetVisible;
	DWORD m_dwStartTime;

	ioHashString m_DelayAnimation;
	ioHashString m_MoveAnimation;

	bool m_bNoCheckCharState;
	bool m_bEnableDash;
	bool m_bEnableJummpingHide;
	bool m_bEnableSpecialMotionHide;
protected:
	bool CheckJummpingState( ioBaseChar *pOwner );
public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual void ProcessBuff( float fTimePerSec );

	virtual void EndBuff();
	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );

public:
	ioHideBuff();
	ioHideBuff( const ioHideBuff &rhs );
	virtual ~ioHideBuff();
};

