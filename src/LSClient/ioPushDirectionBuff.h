#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioPushDirectionBuff : public ioBuff
{

	float m_fAngle;
	bool m_bLinerPower;
	float m_fPushPower;
	float m_fBlowPower;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();

protected:
	virtual void SetAction();

public:
	ioPushDirectionBuff();
	ioPushDirectionBuff( const ioPushDirectionBuff &rhs );
	virtual ~ioPushDirectionBuff();

};


inline ioPushDirectionBuff* ToPushDirectionBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_PUSH_DIRECTION )
		return NULL;

	return dynamic_cast< ioPushDirectionBuff* >( pBuff );
}


