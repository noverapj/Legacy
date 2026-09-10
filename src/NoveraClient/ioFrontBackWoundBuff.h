#pragma once

#include "ioBuff.h"


class ioBaseChar;
class ioINILoader;

class ioFrontBackWoundBuff : public ioBuff
{
protected:
	FrontBackWoundInfo m_FrontBackWoundInfo;
	bool m_bDisableDownState;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );

public :
	virtual ioBuff* Clone();

public:	
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();

	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );

protected:
	void CheckOwnerStateCheck();

public:
	ioFrontBackWoundBuff();
	ioFrontBackWoundBuff( const ioFrontBackWoundBuff &rhs );
	virtual ~ioFrontBackWoundBuff();
};

