#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioGrapplingPushBuff : public ioBuff
{
protected:
	GrapplingPushedInfo m_PushedGrapplingInfo;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );
	virtual void ProcessBuff( float fTimePerSec );

protected:
	bool CheckOwnerStateCheck();
	bool CheckRestoreStateCheck();

public:
	ioGrapplingPushBuff();
	ioGrapplingPushBuff( const ioGrapplingPushBuff &rhs );
	virtual ~ioGrapplingPushBuff();
};