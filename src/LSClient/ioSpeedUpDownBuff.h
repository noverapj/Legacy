#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioSpeedUpDownBuff : public ioBuff
{

protected:
	float m_fCurAddSpeed;
	float m_fAddSpeed;
	float m_fDashSpeedRate;

	float m_fExtraSpeed;
	bool m_bDisableDownState;

	bool m_bCheckOwnerDropZoneDownState;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );

public :
	virtual ioBuff* Clone();

public:	
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();

	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );

	virtual void GetBuffInfo( SP2Packet &rkPacket );
	virtual void ApplyBuffInfo( SP2Packet &rkPacket );

protected:
	void CheckOwnerStateCheck();
	void ProcessDropZoneCheckBuff( float fTimePerSec, ioBaseChar *pOwner );

public:
	ioSpeedUpDownBuff();
	ioSpeedUpDownBuff( const ioSpeedUpDownBuff &rhs );
	virtual ~ioSpeedUpDownBuff();
};

