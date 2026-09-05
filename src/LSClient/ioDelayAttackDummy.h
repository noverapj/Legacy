#pragma once
#include "ioDummyChar.h"

class ioPlayStage;
class ioPlayMode;
class ioINILoader;
class SP2Packet;

class ioDelayAttackDummy : public ioDummyChar
{
protected:
	int m_nAttackIndex;
	DWORD m_dwAttackEndDuration;
public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual void SetDieState( bool bEffect, bool bWoundedDie = false );
protected:
	virtual void ProcessState( float fTimePerSec, DWORD dwPreTime );
	virtual void ProcessStartState( float fTimePerSec );
	virtual void ProcessDelayState( float fTimePerSec );
	virtual void ProcessAttackState( float fTimePerSec, DWORD dwPreTime );

protected:
	void LoadInitProperty( ioINILoader &rkLoader );
	void CreateWeapon( int nAttackIndex, const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vDir );
	void FireWeapon();
public:
	ioDelayAttackDummy( ioEntityGroup *pGrp, ioPlayMode *pMode );
	virtual ~ioDelayAttackDummy();
};

inline ioDelayAttackDummy* ToDelayAttackDummy( ioDummyChar *pDummy )
{
	if( !pDummy || pDummy->GetType() != DCT_DELAY_ATTACK )
		return NULL;

	return dynamic_cast< ioDelayAttackDummy* >( pDummy );
}