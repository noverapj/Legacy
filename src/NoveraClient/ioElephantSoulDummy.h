#pragma once
#include "ioDummyChar.h"

class ioPlayStage;
class ioPlayMode;
class ioINILoader;
class SP2Packet;

class ioElephantSoulDummy : public ioDummyChar
{	
public:
	enum ElephantDelayState
	{
		EDS_NONE,
		EDS_LOOP,
	};

protected:
	ElephantDelayState	m_DelayState;
	int					m_nAttackIndex;
	int					m_nAttackCnt;
	IntVec				m_vAttackCode;

protected:
	ioHashString		m_LoopAnimation;
	DWORD				m_dwLoopTime;
	DWORD				m_dwLoopEndTime;

protected:
	ioHashString		m_AreaWeaponName;
	DWORD				m_dwAreaweaponIndex;	

public:
	virtual void LoadProperty( ioINILoader &rkLoader );	

protected:
	void LoadInitProperty( ioINILoader &rkLoader );

protected:
	virtual void ProcessState( float fTimePerSec, DWORD dwPreTime );
	virtual void ProcessStartState( float fTimePerSec );
	virtual void ProcessDelayState( float fTimePerSec );

	void ProcessDelayNone( float fTimePerSec );
	void ProcessDelayLoop( float fTimePerSec );

protected:
	void SetDelayLoopState( float fTimePerSec );
	virtual void SetDieState( bool bEffect, bool bWoundedDie = false );

public:
	ioElephantSoulDummy( ioEntityGroup *pGrp, ioPlayMode *pMode );
	virtual ~ioElephantSoulDummy();
};

inline ioElephantSoulDummy* ToElephantSoulDummy( ioDummyChar *pDummy )
{
	if( !pDummy || pDummy->GetType() != DCT_ELEPHANT_SOUL )
		return NULL;

	return dynamic_cast< ioElephantSoulDummy* >( pDummy );
}
