#pragma once

#include "ioExtendDash.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioBaseChar;
class ioPlayStage;

class ioShaolinDash : public ioExtendDash
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
	AttackAttribute m_AttackAttribute_Monk;

	AttackAttribute m_ExtendAttackAttribute;
	AttackAttribute m_ExtendAttackAttribute_Monk;
	//AttackAttribute m_ExtraAttackAttribute;

	DWORD m_dwChargingTime;
	DWORD m_dwChargingStartTime;

	ioHashString m_szChargingAni;
	ioHashString m_szChargingAni_Monk;
	ioHashString m_szGatheringAni;

	CEncrypt<float> m_fChargingSpeedRate;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual DashType GetType() const;
	virtual ioExtendDash* Clone();

	virtual void ApplyDash( ioBaseChar *pOwner, SP2Packet &rkPacket );

	virtual bool IsEnableExtraDashAttack();
	virtual bool IsEnableExtraOtherDashAttack();

public:
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
	ioShaolinDash();
	ioShaolinDash( const ioShaolinDash &rhs );
	virtual ~ioShaolinDash();
};

inline ioShaolinDash* ToShaolinDash( ioExtendDash *pDash )
{
	if( !pDash || pDash->GetType() != DT_SHAOLIN )
		return NULL;

	return dynamic_cast< ioShaolinDash* >( pDash );
}

