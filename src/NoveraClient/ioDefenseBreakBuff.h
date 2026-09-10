#pragma once

#include "ioBuff.h"


class ioBaseChar;
class ioINILoader;

class ioDefenseBreakBuff : public ioBuff
{
protected:
	bool m_bDisableDownState;

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
	ioDefenseBreakBuff();
	ioDefenseBreakBuff( const ioDefenseBreakBuff &rhs );
	virtual ~ioDefenseBreakBuff();
};

inline ioDefenseBreakBuff* ToDefenseBreakBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_DEFENSE_BREAK_BUFF )
		return NULL;

	return dynamic_cast< ioDefenseBreakBuff* >( pBuff );
}

