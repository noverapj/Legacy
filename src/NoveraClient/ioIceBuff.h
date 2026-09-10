#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioIceBuff : public ioBuff
{
protected:
	WeaponInfo m_BoundWeaponInfo;
	float m_fBoundGravityAmt;

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
	ioIceBuff();
	ioIceBuff( const ioIceBuff &rhs );
	virtual ~ioIceBuff();
};

