#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioAirBuff : public ioBuff
{

protected:
	bool m_bDisableDownState;
	bool m_bNoStopMotionCheck;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );

public :
	virtual ioBuff* Clone();

public:	
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();

	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );

	//virtual void GetBuffInfo( SP2Packet &rkPacket );
	//virtual void ApplyBuffInfo( SP2Packet &rkPacket );

protected:
	void CheckOwnerStateCheck();

public:
	ioAirBuff();
	ioAirBuff( const ioAirBuff &rhs );
	virtual ~ioAirBuff();
};



inline ioAirBuff* ToAirBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_AIR_BUFF )
		return NULL;

	return dynamic_cast< ioAirBuff* >( pBuff );
}
