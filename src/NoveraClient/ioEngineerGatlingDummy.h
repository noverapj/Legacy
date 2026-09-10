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

class ioEngineerGatlingDummy : public ioDummyChar
{	
public:
	enum GatlingState
	{
		GS_START,
		GS_DEALY,
		GS_CHARGING,
		GS_GATLING_ATTACK,
		GS_BEAM_ATTACK,
		GS_SHOCK_ATTACK,
		GS_AIR_TRACE_ATTACK,
		GS_END,
	};

protected:
	GatlingState	m_GatlingState;

	ioHashString	m_szChargingAnimation;
	float			m_fChargingAnimationRate;

protected:
	ioHashString	m_szGatlingAttackAnimation;
	float			m_fGatlingAttackAnimationRate;

protected:
	ioHashString	m_szBeamAttackAnimation;
	float			m_fBeamAttackAnimationRate;

protected:
	ioHashString	m_szShockAttackAnimation;
	float			m_fShockAttackAnimationRate;

protected:
	ioHashString	m_szAirTraceAttackAnimation;
	float			m_fAirTraceAttackAnimationRate;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );

protected:
	virtual void ProcessState( float fTimerPerSec, DWORD dwPreTime );
	virtual void ProcessPos( float fTimePerSec );

protected:
	void ProcessStartState( float fTimePerSec );
	void ProcessDelayState( float fTimePerSec );
	void ProcessChargingState( float fTimePerSec );

protected:
	void SetActionAni( ioHashString szAnimation, float fAniRate );
	void SetLoopAni( ioHashString szAnimation, float fAniRate );

public:
	virtual int  DontMoveEntityLevel() const;

public:
	void SetLoopDelayState();
	void SetChargingState();
	void SetGatlingAttackState();
	void SetBeamAttackState();
	void SetShockAttackState();
	void SetAirTraceAttack();

public:
	ioEngineerGatlingDummy( ioEntityGroup *pGrp, ioPlayMode *pMode );
	virtual ~ioEngineerGatlingDummy();
};

inline ioEngineerGatlingDummy* ToEngineerGatlingDummy( ioDummyChar *pDummy )
{
	if( !pDummy || pDummy->GetType() != DCT_ENGINEER_GATLING )
		return NULL;

	return dynamic_cast< ioEngineerGatlingDummy* >( pDummy );
}