#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioHeadScaleBuff : public ioBuff
{
protected:
	float m_fCurScaleRate;
	float m_fScaleRate;
	bool m_bRemoveBuffByDrop;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual void ProcessBuff( float fTimePerSec );	
	virtual void EndBuff();

	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );
	
public:
	ioHeadScaleBuff();
	ioHeadScaleBuff( const ioHeadScaleBuff &rhs );
	virtual ~ioHeadScaleBuff();
};

