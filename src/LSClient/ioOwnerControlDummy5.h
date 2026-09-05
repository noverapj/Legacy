#pragma once
#include "ioDummyChar.h"

class ioOwnerControlDummy5 :public ioDummyChar
{
protected:
	DWORD	m_dwAttackStartTime;
	DWORD	m_dwAttackTicTime;
protected:
	void ProcessStartState();

public:
	virtual void LoadProperty( ioINILoader &rkLoader );

protected:
	virtual void ProcessState( float fTimerPerSec, DWORD dwPreTime );
	virtual void ProcessDelayState( float fTimePerSec );
	virtual void ProcessMoveState( float fTimePerSec );
	virtual void ProcessAttackState( float fTimePerSec, DWORD dwPreTime );
	virtual void ProcessPos( float fTimePerSec );

	virtual void DropZoneCheck();

	virtual int  DontMoveEntityLevel() const;
	virtual bool IsMountingAvailable() const;
	virtual bool IsNowMountEnable() const;
	virtual bool IsMountAvailable() const;
public:
	ioOwnerControlDummy5( ioEntityGroup *pGrp, ioPlayMode *pMode );
	~ioOwnerControlDummy5(void);
};

inline ioOwnerControlDummy5* ToOwnerControlDummy5( ioDummyChar *pDummy )
{
	if( !pDummy || pDummy->GetType() != DCT_OWNER_CONTROL5 )
		return NULL;

	return dynamic_cast< ioOwnerControlDummy5* >( pDummy );
}