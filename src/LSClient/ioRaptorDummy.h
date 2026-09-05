#pragma once

#include "ioDummyChar.h"

class ioEntityGroup;
class ioPlayStage;
class ioPlayMode;
class ioGaugeHelper;
class ioINILoader;
class ioWeapon;
class SP2Packet;
class ioBaseChar;
class ioAniCollisionBoxGrp;

class ioRaptorDummy : public ioDummyChar
{
	// RaptorDummy타입을 사용하는 용병 인덱스(이 인덱스로 내부에서 다르게 로직을 추가해야하는 부분때문에 추가)
	enum UseMercenaryType
	{
		UMT_DRAGON_RIDER = 90,
		UMT_DR_SPIDER = 214,
	};
public:
	virtual void LoadProperty( ioINILoader &rkLoader );

protected:
	virtual void ProcessState( float fTimerPerSec, DWORD dwPreTime );
	virtual void ProcessPos( float fTimePerSec );

	virtual void ProcessDelayState( float fTimePerSec );
	virtual void ProcessMoveState( float fTimePerSec );
	virtual void ProcessDashState( float fTimePerSec );

	virtual void ProcessAttackState( float fTimePerSec, DWORD dwPreTime );

public:
	virtual void SetAttackState();

	virtual void SetMoveState();
	virtual void SetDashState();

	void SetMoveStateMid();
	void SetDashStateMid();

public:
	ioRaptorDummy( ioEntityGroup *pGrp, ioPlayMode *pMode );
	virtual ~ioRaptorDummy();
};

inline ioRaptorDummy* ToRaptorDummy( ioDummyChar *pDummy )
{
	if( !pDummy || pDummy->GetType() != DCT_RAPTOR )
		return NULL;

	return dynamic_cast< ioRaptorDummy* >( pDummy );
}