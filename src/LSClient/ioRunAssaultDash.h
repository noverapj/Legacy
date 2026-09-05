#pragma once

#include "ioExtendDash.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioBaseChar;
class ioPlayStage;

class ioRunAssaultDash : public ioExtendDash
{
public:
	enum RunDashState
	{
		RDS_NONE,
		RDS_CHARGING,
		RDS_ASSAULT,
		RDS_RUN,
		RDS_EXTEND_CHARGING,
		RDS_DRUID_TO_HUMAN,
		RDS_DRUID_TO_BEAR,
	};

	enum DashExtedState
	{
		DES_NONE,
		DES_DRUID,
	};

	enum AssaultType
	{
		AT_DEFAULT,
		AT_DRUID,
	};

protected:
	RunDashState	m_RunDashState;
	AssaultType		m_AssaultType;
	AttackAttribute	m_AttackAttribute;
	DashExtedState	m_DashExtedState;
	DWORD			m_dwActionEndTime;

	float			m_fOffSet;

	ioHashString	m_CircleEffectRed;
	ioHashString	m_CircleEffectBlue;

	ioHashString	m_RunDashChargingAni;
	DWORD			m_dwRunDashChargingTime;
	DWORD			m_dwRunDashChargingStart;

	ioHashString	m_ExtendDashChargingAnimation;
	DWORD			m_dwExtendDashChargingTime;
	DWORD			m_dwExtendDashChargingStart;
	
public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual DashType GetType() const;
	virtual ioExtendDash* Clone();

	virtual bool StartDash( ioBaseChar *pOwner );
	virtual void ProcessDash( ioBaseChar *pOwner );

	virtual void ApplyDash( ioBaseChar *pOwner, SP2Packet &rkPacket );

public:
	void SetExtraInfoToWeapon( ioBaseChar *pOwner, ioPlayStage *pStage, ioWeapon *pWeapon, const ioHashString &szName );

protected:
	void ChangeToCharging( ioBaseChar *pOwner );
	void ChangeToAssault( ioBaseChar *pOwner );
	void ChangeToRun( ioBaseChar *pOwner );
	void ChangeToDruidToHuman( ioBaseChar *pOwner );
	void ChangeToDruidEagle( ioBaseChar *pOwner );
	void ChangeToDruidBearCrouch( ioBaseChar *pOwner );

	void ProcessChargingState( ioBaseChar *pOwner );
	void ProcessAssaultState( ioBaseChar *pOwner );
	void ProcessRunState( ioBaseChar *pOwner );
	void ProcessExtendChargingState( ioBaseChar *pOwner );

public:
	void ChangeToExtendCharging( ioBaseChar *pOwner );

public:
	ioRunAssaultDash();
	ioRunAssaultDash( const ioRunAssaultDash &rhs );
	virtual ~ioRunAssaultDash();
};

inline ioRunAssaultDash* ToRunAssaultDash( ioExtendDash *pDash )
{
	if( !pDash || pDash->GetType() != DT_RUN_ASSAULT )
		return NULL;

	return dynamic_cast< ioRunAssaultDash* >( pDash );
}

