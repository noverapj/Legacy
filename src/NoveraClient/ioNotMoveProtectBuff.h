#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioNotMoveProtectBuff : public ioBuff
{
protected:
	ioHashString m_ChangeMaterial;

	ioHashString m_LoopAnimation;
	ioHashString m_LoopEffect;
	bool m_bSetState;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();

	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );

public:
	ioNotMoveProtectBuff();
	ioNotMoveProtectBuff( const ioNotMoveProtectBuff &rhs );
	virtual ~ioNotMoveProtectBuff();
};


inline ioNotMoveProtectBuff* ToNotMoveProtectBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_NOT_MOVE_PROTECT )
		return NULL;

	return dynamic_cast< ioNotMoveProtectBuff* >( pBuff );
}


