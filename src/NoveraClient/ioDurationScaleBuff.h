#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioDurationScaleBuff : public ioBuff
{
protected:
	int m_iGrade;
	float m_fScaleValue;
	float m_fScaleBalance;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

protected:
	float GetScaleByPlayLevel( int iPlayLevel );
public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual void EndBuff();
	virtual void ProcessBuff( float fTimePerSec );

	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );

public:
	ioDurationScaleBuff();
	ioDurationScaleBuff( const ioDurationScaleBuff &rhs );
	virtual ~ioDurationScaleBuff();
};

