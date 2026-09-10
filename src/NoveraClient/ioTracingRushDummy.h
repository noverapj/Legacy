#pragma once
#include "ioDummyChar.h"

class ioPlayStage;
class ioPlayMode;
class ioINILoader;
class SP2Packet;

class ioTracingRushDummy : public ioDummyChar
{	
public:
	enum TraceState
	{
		TS_SENSING,
		TS_TRACE,
	};

protected:	
	TraceState	m_TraceState;
	D3DXVECTOR3	m_vPrePos;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );	

protected:
	void LoadInitProperty( ioINILoader &rkLoader );

protected:
	virtual void ProcessState( float fTimePerSec, DWORD dwPreTime );
	virtual void ProcessStartState( float fTimePerSec );
	virtual void ProcessMoveState( float fTimePerSec );

public:
	virtual void ApplyDummyCharSync( ioPlayStage *pStage, SP2Packet &rkPacket );

protected:
	void SetSensingState( ioPlayStage *pStage, bool bSendNetwork );
	void SetTracingState( ioPlayStage *pStage, bool bSendNetwork );

protected:
	void ProcessSensing( float fTimePerSec );
	void ProcessTracing( float fTimePerSec, ioPlayStage *pStage );

public:
	ioTracingRushDummy( ioEntityGroup *pGrp, ioPlayMode *pMode );
	virtual ~ioTracingRushDummy();
};

inline ioTracingRushDummy* ToTracingRushDummy( ioDummyChar *pDummy )
{
	if( !pDummy || pDummy->GetType() != DCT_TRACING_RUSH )
		return NULL;

	return dynamic_cast< ioTracingRushDummy* >( pDummy );
}
