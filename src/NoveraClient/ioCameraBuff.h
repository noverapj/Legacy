#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioCameraBuff : public ioBuff
{
protected:
	float m_fBuffCamDistance;
	float m_fBuffCamHeight;
	float m_fBuffCamFov;
	DWORD m_dwBuffCamZoomTime;

	bool m_bPriorityOverSkill;
	
public:
	virtual ioBuff* Clone();
	virtual void LoadProperty( ioINILoader &rkLoader );

	virtual void StartBuff( ioBaseChar *pOwner );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();
	
	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );

public:
	ioCameraBuff();
	ioCameraBuff( const ioCameraBuff &rhs );
	virtual ~ioCameraBuff();
};

