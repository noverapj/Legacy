#pragma once

#include "ioBuff.h"


class ioBaseChar;
class ioINILoader;

class ioChangeSkinColorBuff : public ioBuff
{
protected:
	bool m_bDisableDownState;
	bool m_bDisableWoundState;
	CEncrypt<int>	m_iSkinColor;
	CEncrypt<int>	m_iOrigSkinColor;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual void ProcessBuff( float fTimePerSec );	
	virtual void EndBuff();

	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );

protected:
	bool CheckOwnerStateCheck();

public:
	ioChangeSkinColorBuff();
	ioChangeSkinColorBuff( const ioChangeSkinColorBuff &rhs );
	virtual ~ioChangeSkinColorBuff();
};

