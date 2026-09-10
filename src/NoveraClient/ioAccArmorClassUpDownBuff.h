#pragma once
#include "iobuff.h"
class ioAccArmorClassUpDownBuff :
	public ioBuff
{
private:
	float	m_fAddExtraArmorClass;
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
	ioAccArmorClassUpDownBuff(void);
	ioAccArmorClassUpDownBuff( const ioAccArmorClassUpDownBuff &rhs );
	virtual ~ioAccArmorClassUpDownBuff(void);
};

