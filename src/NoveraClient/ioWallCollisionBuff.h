#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioWallCollisionBuff : public ioBuff
{
protected:
	ioHashStringVec m_WallCollisionBuff;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );
	virtual void ProcessBuff( float fTimePerSec );

protected:
	bool CheckRestoreStateCheck();

public:
	void CheckWallCollision();

public:
	ioWallCollisionBuff();
	ioWallCollisionBuff( const ioWallCollisionBuff &rhs );
	virtual ~ioWallCollisionBuff();
};


inline ioWallCollisionBuff* ToWallCollisionBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_WALL_COLLISION )
		return NULL;

	return dynamic_cast< ioWallCollisionBuff* >( pBuff );
}


