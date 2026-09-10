#pragma once

#include "ioDummyChar.h"
#include "ioVehicleDriverObjectItem.h"

class ioEntityGroup;
class ioPlayStage;
class ioPlayMode;
class ioGaugeHelper;
class ioINILoader;
class ioWeapon;
class SP2Packet;
class ioBaseChar;
class ioAniCollisionBoxGrp;

class ioDruidEagleDummy : public ioDummyChar
{	
public:
	enum EagleDummyState
	{
		EDS_MOVE,
		EDS_ATTACK,
	};

protected:
	ioHashString	m_UpFlyAnimation;
	float			m_UpFlyAnimationRate;
	ioHashString	m_DownFlyAnimation;
	float			m_DownFlyAnimationRate;

	DWORD			m_dwSpecialStateCheckTime;
	DWORD			m_dwCurrCheckTime;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );

protected:
	virtual void ProcessState( float fTimerPerSec, DWORD dwPreTime );
	virtual void ProcessPos( float fTimePerSec );

	virtual void ProcessDelayState( float fTimePerSec );
	virtual void ProcessMoveState( float fTimePerSec );

public:
	void CheckCratorState( ioBaseChar* pOwner );

public:
	virtual void SetMoveState();
	void SetMoveStateMid();

public:
	void SetFly();
	void SetDownFly();
	void SetUpFly();

public:
	ioDruidEagleDummy( ioEntityGroup *pGrp, ioPlayMode *pMode );
	virtual ~ioDruidEagleDummy();
};

inline ioDruidEagleDummy* ToDruidEagleDummy( ioDummyChar *pDummy )
{
	if( !pDummy || pDummy->GetType() != DCT_DRUID_EAGLE )
		return NULL;

	return dynamic_cast< ioDruidEagleDummy* >( pDummy );
}