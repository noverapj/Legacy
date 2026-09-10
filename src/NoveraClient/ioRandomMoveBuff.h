#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioRandomMoveBuff : public ioBuff
{
protected:
	FloatVec m_AngleList;

	ioHashString m_MoveAni;
	ioHashString m_CatchMoveAni;

	CEncrypt<DWORD> m_dwMinMoveTime;
	CEncrypt<DWORD> m_dwMaxMoveTime;

	CEncrypt<float> m_fMoveSpeed;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();

	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );

public:
	ioRandomMoveBuff();
	ioRandomMoveBuff( const ioRandomMoveBuff &rhs );
	virtual ~ioRandomMoveBuff();
};


inline ioRandomMoveBuff* ToRandomMoveBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_RANDOM_MOVE )
		return NULL;

	return dynamic_cast< ioRandomMoveBuff* >( pBuff );
}


