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

class ioVehicleDummy : public ioDummyChar
{	
protected:
	DriverState		m_eDriverState;

	ioHashString	m_JumpReadyAnimation;
	float			m_fJumpReadyAniRate;

	ioHashString	m_JumppingAnimation;
	float			m_fJumppingAniRate;

	ioHashString	m_JumpLandingAnimation;
	float			m_fJumpLandingAniRate;

	ioHashString	m_DriftLeftAnimation;
	float			m_fDriftLeftAniRate;

	ioHashString	m_DrifRightAnimation;
	float			m_fDrifRightAniRate;

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
	void SetRunState();
	void SetJumpReadyState();
	void SetJummping();
	void SetLanding();
	void SetDrift( bool bLeft );

public:
	ioVehicleDummy( ioEntityGroup *pGrp, ioPlayMode *pMode );
	virtual ~ioVehicleDummy();
};

inline ioVehicleDummy* ToVehicleDummy( ioDummyChar *pDummy )
{
	if( !pDummy || pDummy->GetType() != DCT_VEHICLE )
		return NULL;

	return dynamic_cast< ioVehicleDummy* >( pDummy );
}