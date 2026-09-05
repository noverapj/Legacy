#pragma once

#include "ioExtendDash.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioBaseChar;
class ioPlayStage;

class ioPowerChargeDash2 : public ioExtendDash
{
public:
	enum ChargeDashState
	{
		CDS_NONE,
		CDS_NORMAL_ATTACK,
		CDS_DEFENSE,
	};

protected:
	ChargeDashState m_DashState;

	AttackAttribute m_AttackAttribute;

	ioHashString m_szDefenseAni;
	CEncrypt<DWORD> m_dwDefenseStartTime;
	CEncrypt<DWORD> m_dwDefenseDuration;
	CEncrypt<float> m_fDefenseeSpeedRate;

	ioHashStringVec m_vDashDefenseBuff;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual DashType GetType() const;
	virtual ioExtendDash* Clone();

	virtual void ApplyDash( ioBaseChar *pOwner, SP2Packet &rkPacket );

	const ioHashStringVec& GetDashDefenseBuff() const { return m_vDashDefenseBuff; }

	void SetPowerDash( ioBaseChar *pOwner );
	bool CheckPowerDash( ioBaseChar *pOwner );
	void PowerDashEnd( ioBaseChar *pOwner );

	float GetCurStateSpeedRate();

	bool IsPowerChargingState();
	bool IsEnableCharDirection();
	virtual bool IsEnableDefense();

protected:
	void ProcessDefenseState( ioBaseChar *pOwner );

	void ChangeToNormalAttack( ioBaseChar *pOwner );
	void ChangeToDefenseState( ioBaseChar *pOwner );

public:
	ioPowerChargeDash2();
	ioPowerChargeDash2( const ioPowerChargeDash2 &rhs );
	virtual ~ioPowerChargeDash2();
};

inline ioPowerChargeDash2* ToPowerChargeDash2( ioExtendDash *pDash )
{
	if( !pDash || pDash->GetType() != DT_POWER_CHARGE2 )
		return NULL;

	return dynamic_cast< ioPowerChargeDash2* >( pDash );
}

inline const ioPowerChargeDash2* ToPowerChargeDash2Const( const ioExtendDash *pDash )
{
	if( !pDash || pDash->GetType() != DT_POWER_CHARGE2 )
		return NULL;

	return dynamic_cast< const ioPowerChargeDash2* >( pDash );
}

