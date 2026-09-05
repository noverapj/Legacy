#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioWeakMagicBuff : public ioBuff
{
protected:
	float m_fWeakRate;
	MagicType m_iMagicType;
	bool m_bDisableDownState;
	ElementProtectionType m_ElementProtectionType;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );

	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();

protected:
	void CheckOwnerStateCheck();

public:
	ioWeakMagicBuff();
	ioWeakMagicBuff( const ioWeakMagicBuff &rhs );
	virtual ~ioWeakMagicBuff();
};

