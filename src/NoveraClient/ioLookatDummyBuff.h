#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;
class ioEffect;

class ioLookatDummyBuff : public ioBuff
{
protected:
	ioHashString m_szDummyName;	

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();

	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );	
	
protected:
	void LookAtDummyPos( D3DXVECTOR3 vPos );

public:
	ioLookatDummyBuff();
	ioLookatDummyBuff( const ioLookatDummyBuff &rhs );
	virtual ~ioLookatDummyBuff();
};

