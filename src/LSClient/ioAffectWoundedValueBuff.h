#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioAffectWoundedValueBuff : public ioBuff
{
public:
	struct AffectRate
	{
		float m_fMinRate;
		float m_fMaxRate;
		float m_fRate;
		AffectRate()
		{
			m_fMinRate = FLOAT1;
			m_fMaxRate = FLOAT1;
			m_fRate = FLOAT1;
		}
	};

protected:
	// push
	AffectRate m_PushPower;
	AffectRate m_AirPushPower;
	AffectRate m_DownPushPower;

	// blow
	AffectRate m_BlowPower;
	AffectRate m_AirBlowPower;
	AffectRate m_DownBlowPower;

	// bound
	AffectRate m_BoundJumpPower;
	AffectRate m_BoundForcePower;
	AffectRate m_BoundGravity;
	AffectRate m_BoundDamage;

	// damage
	AffectRate m_Damage;

	// float
	AffectRate m_FloatJumpPower;
	AffectRate m_FloatGravity;
	AffectRate m_FloatFriction;

	// stand_time
	AffectRate m_GetUpTimeRate;

	// falldamage
	AffectRate m_FallDamageRate;

	CEncrypt<bool> m_bCheckTeamAttack;

	DWORD m_iSeedCount;

	bool m_bExceptTarget;
	ioHashString m_szExceptCharName;

	bool m_bAllPowerDisableExceptDamage;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void GetBuffInfo( SP2Packet &rkPacket );
	virtual void ApplyBuffInfo( SP2Packet &rkPacket );
	virtual void EndBuff();

public:
	const float GetPushPowerRate()		{ return m_PushPower.m_fRate; }
	const float GetAirPushPowerRate()	{ return m_AirPushPower.m_fRate; }
	const float GetDownPushPowerRate()	{ return m_DownPushPower.m_fRate; }

	const float GetBlowPowerRate()		{ return m_BlowPower.m_fRate; }
	const float GetAirBlowPowerRate()	{ return m_AirBlowPower.m_fRate; }
	const float GetDownBlowPowerRate()	{ return m_DownBlowPower.m_fRate; }

	const float GetBoundJumpPowerRate()	{ return m_BoundJumpPower.m_fRate; }
	const float GetBoundForcePowerRate(){ return m_BoundForcePower.m_fRate; }
	const float GetBoundGravityRate()	{ return m_BoundGravity.m_fRate; }
	const float GetBoundDamageRate()	{ return m_BoundDamage.m_fRate; }

	const float GetDamageRate()			{ return m_Damage.m_fRate; }

	const float GetFloatJumpPowerRate()	{ return m_FloatJumpPower.m_fRate; }
	const float GetFloatGravityRate()	{ return m_FloatGravity.m_fRate; }
	const float GetFloatFrictionRate()	{ return m_FloatFriction.m_fRate; }

	const float GetGetUpRate()			{ return m_GetUpTimeRate.m_fRate; }

	const float GetFallDamageRate()		{ return m_FallDamageRate.m_fRate; }

	const bool CheckTeamAttack() { return m_bCheckTeamAttack; }

	const bool GetAllPowerDisable() { return m_bAllPowerDisableExceptDamage; }

protected:
	void SetRandomRate( AffectRate &rkAffectRate );

public:
	inline void SetExceptCharName( ioHashString szExceptCharName )		{ m_szExceptCharName = szExceptCharName; }
	bool IsExceptChar( ioHashString szChar );

public:
	ioAffectWoundedValueBuff();
	ioAffectWoundedValueBuff( const ioAffectWoundedValueBuff &rhs );
	virtual ~ioAffectWoundedValueBuff();
};


inline ioAffectWoundedValueBuff* ToAffectWoundedValue( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_AFFECT_WOUNDED_VALUE )
		return NULL;

	return dynamic_cast< ioAffectWoundedValueBuff* >( pBuff );
}


