#pragma once
#include "iobuff.h"
class ioAccCriticalDamageRateBuff :
	public ioBuff
{
private:
	float	m_fAddCriticalDamageRate;
	bool	m_bDisableDownState;
public :
	virtual ioBuff* Clone();
	virtual void LoadProperty( ioINILoader &rkLoader );

public:	
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();

	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );
protected:
	void CheckOwnerStateCheck();
public:
	ioAccCriticalDamageRateBuff(void);
	ioAccCriticalDamageRateBuff( const ioAccCriticalDamageRateBuff &rhs );
	virtual ~ioAccCriticalDamageRateBuff(void);
};

