#pragma once
#include "iobuff.h"
class ioAccCriticalRateBuff :
	public ioBuff
{
private:
	float	m_fAddCriticalRate;
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
	ioAccCriticalRateBuff(void);
	ioAccCriticalRateBuff( const ioAccCriticalRateBuff &rhs );
	virtual ~ioAccCriticalRateBuff(void);
};

