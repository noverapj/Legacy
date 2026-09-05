#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioReturnAttackBuff : public ioBuff
{

protected:
	int m_iReturnAttackNum;
	int m_iReturnAttackNumResist;
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
	ioReturnAttackBuff();
	ioReturnAttackBuff( const ioReturnAttackBuff &rhs );
	virtual ~ioReturnAttackBuff();
};
