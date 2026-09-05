#pragma once

#include "ioBuff.h"


class ioBaseChar;
class ioINILoader;

class ioAccumulateBuff : public ioBuff
{
protected:
	int m_iAccumulateCnt;

	int m_iImmuneCnt;
	ioHashString m_szImmuneBuff;

	bool m_bImmuneBuff;
	bool m_bDisableDownState;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual void ProcessBuff( float fTimePerSec );	
	virtual void EndBuff();

	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );

	virtual void AccumulateBuff();

	virtual void GetBuffInfo( SP2Packet &rkPacket );
	virtual void ApplyBuffInfo( SP2Packet &rkPacket );

protected:
	bool CheckOwnerStateCheck();

public:
	ioAccumulateBuff();
	ioAccumulateBuff( const ioAccumulateBuff &rhs );
	virtual ~ioAccumulateBuff();
};

