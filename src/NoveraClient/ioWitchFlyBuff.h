#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioWitchFlyBuff : public ioBuff
{
protected:
	WitchFlyInfo m_AttackWitchFly;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();

	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );

public:
	ioWitchFlyBuff();
	ioWitchFlyBuff( const ioWitchFlyBuff &rhs );
	virtual ~ioWitchFlyBuff();
};


inline ioWitchFlyBuff* ToWitchFlyBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_WITCH_FLY_STATE )
		return NULL;

	return dynamic_cast< ioWitchFlyBuff* >( pBuff );
}


