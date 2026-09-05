#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioBurnBuff : public ioBuff
{
protected:
	ioHashString m_AddEntity;

	DWORD m_dwMaxTime;
	DWORD m_dwMinTime;
	DWORD m_dwMaxTimeByNpc;
	DWORD m_dwMinTimeByNpc;
	DWORD m_dwMaxTimeByBossNpc;
	DWORD m_dwMinTimeByBossNpc;

	DWORD m_dwTeamMaxTime;
	DWORD m_dwTeamMinTime;

	bool m_bEnableTimeEnd;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();

	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );

protected:
	DWORD GetMaxTime();
	DWORD GetMinTime();

public:
	ioBurnBuff();
	ioBurnBuff( const ioBurnBuff &rhs );
	virtual ~ioBurnBuff();
};

