#pragma once
#include "iobuff.h"
class ioAccSpeedUpDownBuff :
	public ioBuff
{
protected:
	float m_fCurAddSpeed;
	float m_fAddSpeedByAcc;
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
	ioAccSpeedUpDownBuff(void);
	ioAccSpeedUpDownBuff( const ioAccSpeedUpDownBuff &rhs );
	virtual ~ioAccSpeedUpDownBuff(void);
};