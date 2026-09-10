#pragma once
#include "ioDummyChar.h"

class ioPlayStage;
class ioPlayMode;
class ioINILoader;
class SP2Packet;

class ioTargetTrace6Dummy : public ioDummyChar
{
	bool m_bCheckDropZoneDie;
public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual void SetDieState( bool bEffect, bool bWoundedDie = false );
protected:
	virtual void ProcessState( float fTimePerSec, DWORD dwPreTime );
	virtual void ProcessStartState( float fTimePerSec );
	virtual void ProcessDelayState( float fTimePerSec );

protected:
	void LoadInitProperty( ioINILoader &rkLoader );
	void CheckCurHp();

public:
	ioTargetTrace6Dummy( ioEntityGroup *pGrp, ioPlayMode *pMode );
	virtual ~ioTargetTrace6Dummy();
};

inline ioTargetTrace6Dummy* ToTargetTrace6Dummy( ioDummyChar *pDummy )
{
	if( !pDummy || pDummy->GetType() != DCT_TARGET_TRACE6 )
		return NULL;

	return dynamic_cast< ioTargetTrace6Dummy* >( pDummy );
}