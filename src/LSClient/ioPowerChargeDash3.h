#pragma once

#include "ioExtendDash.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioBaseChar;
class ioPlayStage;

class ioPowerChargeDash3 : public ioExtendDash
{
public:
	enum ChargeDashState
	{
		CDS_NONE,
		CDS_CHARGING,
		CDS_GATHERING,
		CDS_NORMAL_ATTACK,
		CDS_EXTEND_ATTACK,
	};

protected:
	ChargeDashState m_DashState;

	AttackAttribute m_AttackAttribute;
	AttackAttribute m_ExtendAttackAttribute;

	DWORD m_dwChargingTime;
	DWORD m_dwChargingStartTime;

	ioHashString m_szChargingAni;
	ioHashString m_szGatheringAni;

	CEncrypt<float> m_fChargingSpeedRate;

	BlowDashInfo m_BlowDashInfo;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual DashType GetType() const;
	virtual ioExtendDash* Clone();

	virtual void ApplyDash( ioBaseChar *pOwner, SP2Packet &rkPacket );

	void SetPowerDash( ioBaseChar *pOwner );
	bool CheckPowerDash( ioBaseChar *pOwner );
	bool IsPowerChargingState();

	void PowerDashEnd( ioBaseChar *pOwner );

	inline float GetChargingSpeedRate() const { return m_fChargingSpeedRate; }

protected:
	void ProcessCharging( ioBaseChar *pOwner );
	void ProcessGathering( ioBaseChar *pOwner );

	void ChangeToGathering( ioBaseChar *pOwner );
	void ChangeToNormalAttack( ioBaseChar *pOwner );
	void ChangeToExtendAttack( ioBaseChar *pOwner );

public:
	ioPowerChargeDash3();
	ioPowerChargeDash3( const ioPowerChargeDash3 &rhs );
	virtual ~ioPowerChargeDash3();
};

inline ioPowerChargeDash3* ToPowerChargeDash3( ioExtendDash *pDash )
{
	if( !pDash || pDash->GetType() != DT_POWER_CHARGE3 )
		return NULL;

	return dynamic_cast< ioPowerChargeDash3* >( pDash );
}

