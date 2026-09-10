#pragma once
#include "iobuff.h"
class ioAccGetUpProtectionDelayBuff :
	public ioBuff
{
private:
	DWORD	m_dwGetUpDelayDuration;
	bool	m_bDisableDownState;

	IntVec	m_vEnableModeTypeList;
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
	void CheckGetUpDelayDuration( DWORD& dwDuration, int iMode );
public:
	ioAccGetUpProtectionDelayBuff(void);
	ioAccGetUpProtectionDelayBuff( const ioAccGetUpProtectionDelayBuff &rhs );
	~ioAccGetUpProtectionDelayBuff(void);
};

inline ioAccGetUpProtectionDelayBuff* ToAccGetUpProtectionDelayBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_ACC_GETUP_PROTECTION_DELAY_BUFF )
		return NULL;

	return dynamic_cast< ioAccGetUpProtectionDelayBuff* > ( pBuff );
}