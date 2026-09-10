#pragma once
#include "iobuff.h"
class ioAccArmorPentrationBuff :
	public ioBuff
{
private:
	float	m_fAddArmorPentration;
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
	ioAccArmorPentrationBuff(void);
	ioAccArmorPentrationBuff( const ioAccArmorPentrationBuff &rhs );
	~ioAccArmorPentrationBuff(void);
};

