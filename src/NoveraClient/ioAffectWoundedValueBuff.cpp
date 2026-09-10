

#include "stdafx.h"

#include "ioBaseChar.h"

#include "ioAffectWoundedValueBuff.h"

ioAffectWoundedValueBuff::ioAffectWoundedValueBuff()
{
	m_iSeedCount = 0;
	m_szExceptCharName.Clear();
}

ioAffectWoundedValueBuff::ioAffectWoundedValueBuff( const ioAffectWoundedValueBuff &rhs )
: ioBuff( rhs ),
m_PushPower( rhs.m_PushPower ),
m_AirPushPower( rhs.m_AirPushPower ),
m_DownPushPower( rhs.m_DownPushPower ),
m_BlowPower( rhs.m_BlowPower ),
m_AirBlowPower( rhs.m_AirBlowPower ),
m_DownBlowPower( rhs.m_DownBlowPower ),
m_BoundJumpPower( rhs.m_BoundJumpPower ),
m_BoundForcePower( rhs.m_BoundForcePower ),
m_BoundGravity( rhs.m_BoundGravity ),
m_BoundDamage( rhs.m_BoundDamage ),
m_Damage( rhs.m_Damage ),
m_FloatJumpPower( rhs.m_FloatJumpPower ),
m_FloatGravity( rhs.m_FloatGravity ),
m_FloatFriction( rhs.m_FloatFriction ),
m_GetUpTimeRate( rhs.m_GetUpTimeRate ),
m_FallDamageRate( rhs.m_FallDamageRate ),
m_bCheckTeamAttack( rhs.m_bCheckTeamAttack ),
m_bExceptTarget( rhs.m_bExceptTarget ),
m_bAllPowerDisableExceptDamage( rhs.m_bAllPowerDisableExceptDamage )
{
	m_iSeedCount = 0;
	m_szExceptCharName.Clear();
}

ioAffectWoundedValueBuff::~ioAffectWoundedValueBuff()
{
}

void ioAffectWoundedValueBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	// push
	m_PushPower.m_fMinRate		= rkLoader.LoadFloat_e( "push_power_min_rate", FLOAT1 );
	m_PushPower.m_fMaxRate		= rkLoader.LoadFloat_e( "push_power_max_rate", FLOAT1 );
	m_AirPushPower.m_fMinRate	= rkLoader.LoadFloat_e( "air_push_min_rate", FLOAT1 );
	m_AirPushPower.m_fMaxRate	= rkLoader.LoadFloat_e( "air_push_max_rate", FLOAT1 );
	m_DownPushPower.m_fMinRate	= rkLoader.LoadFloat_e( "down_push_min_rate", FLOAT1 );
	m_DownPushPower.m_fMaxRate	= rkLoader.LoadFloat_e( "down_push_max_rate", FLOAT1 );

	// blow
	m_BlowPower.m_fMinRate		= rkLoader.LoadFloat_e( "blow_power_min_rate", FLOAT1 );
	m_BlowPower.m_fMaxRate		= rkLoader.LoadFloat_e( "blow_power_max_rate", FLOAT1 );
	m_AirBlowPower.m_fMinRate	= rkLoader.LoadFloat_e( "air_blow_power_min_rate", FLOAT1 );
	m_AirBlowPower.m_fMaxRate	= rkLoader.LoadFloat_e( "air_blow_power_max_rate", FLOAT1 );
	m_DownBlowPower.m_fMinRate	= rkLoader.LoadFloat_e( "down_blow_power_min_rate", FLOAT1 );
	m_DownBlowPower.m_fMaxRate	= rkLoader.LoadFloat_e( "down_blow_power_max_rate", FLOAT1 );

	// bound
	m_BoundJumpPower.m_fMinRate = rkLoader.LoadFloat_e( "bound_jump_power_min_rate", FLOAT1 );
	m_BoundJumpPower.m_fMaxRate = rkLoader.LoadFloat_e( "bound_jump_power_max_rate", FLOAT1 );
	m_BoundForcePower.m_fMinRate= rkLoader.LoadFloat_e( "bound_force_power_min_rate", FLOAT1 );
	m_BoundForcePower.m_fMaxRate= rkLoader.LoadFloat_e( "bound_force_power_max_rate", FLOAT1 );
	m_BoundGravity.m_fMinRate	= rkLoader.LoadFloat_e( "bound_gravity_min_rate", FLOAT1 );
	m_BoundGravity.m_fMaxRate	= rkLoader.LoadFloat_e( "bound_gravity_max_rate", FLOAT1 );
	m_BoundDamage.m_fMinRate	= rkLoader.LoadFloat_e( "bound_damage_min_rate", FLOAT1 );
	m_BoundDamage.m_fMaxRate	= rkLoader.LoadFloat_e( "bound_damage_max_rate", FLOAT1 );

	// damage
	m_Damage.m_fMinRate			= rkLoader.LoadFloat_e( "damage_min_rate", FLOAT1 );
	m_Damage.m_fMaxRate			= rkLoader.LoadFloat_e( "damage_max_rate", FLOAT1 );

	// float
	m_FloatJumpPower.m_fMinRate	= rkLoader.LoadFloat_e( "float_jump_power_min_rate", FLOAT1 );
	m_FloatJumpPower.m_fMaxRate	= rkLoader.LoadFloat_e( "float_jump_power_max_rate", FLOAT1 );
	m_FloatGravity.m_fMinRate	= rkLoader.LoadFloat_e( "float_gravity_min_rate", FLOAT1 );
	m_FloatGravity.m_fMaxRate	= rkLoader.LoadFloat_e( "float_gravity_max_rate", FLOAT1 );
	m_FloatFriction.m_fMinRate	= rkLoader.LoadFloat_e( "float_friction_min_rate", FLOAT1 );
	m_FloatFriction.m_fMaxRate	= rkLoader.LoadFloat_e( "float_friction_max_rate", FLOAT1 );

	// stand_time
	m_GetUpTimeRate.m_fMinRate = rkLoader.LoadFloat_e( "getup_time_min_rate", FLOAT1 );
	m_GetUpTimeRate.m_fMaxRate = rkLoader.LoadFloat_e( "getup_time_max_rate", FLOAT1 );

	// fall_damage
	m_FallDamageRate.m_fMinRate = rkLoader.LoadFloat_e( "fall_damage_min_rate", FLOAT1 );
	m_FallDamageRate.m_fMaxRate = rkLoader.LoadFloat_e( "fall_damage_max_rate", FLOAT1 );

	m_bCheckTeamAttack = rkLoader.LoadBool_e( "check_team_attack", false );

	m_bExceptTarget = rkLoader.LoadBool_e( "except_target", false );
	m_bAllPowerDisableExceptDamage = rkLoader.LoadBool_e( "all_power_disable", false );
}

ioBuff* ioAffectWoundedValueBuff::Clone()
{
	return new ioAffectWoundedValueBuff( *this );
}

void ioAffectWoundedValueBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

	m_iSeedCount = 0;

	SetRandomRate( m_PushPower );
	SetRandomRate( m_AirPushPower );
	SetRandomRate( m_DownPushPower );

	SetRandomRate( m_BlowPower );
	SetRandomRate( m_AirBlowPower );
	SetRandomRate( m_DownBlowPower );

	SetRandomRate( m_BoundJumpPower );
	SetRandomRate( m_BoundForcePower );
	SetRandomRate( m_BoundGravity );
	SetRandomRate( m_BoundDamage );

	SetRandomRate( m_Damage );

	SetRandomRate( m_FloatJumpPower );
	SetRandomRate( m_FloatGravity );
	SetRandomRate( m_FloatFriction );

	SetRandomRate( m_GetUpTimeRate );

	SetRandomRate( m_FallDamageRate );
}

bool ioAffectWoundedValueBuff::ApplyStartBuff(ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;

	return true;
}

void ioAffectWoundedValueBuff::ProcessBuff( float fTimePerSec )
{
	switch( m_iOperationType )
	{
	case OT_SWITCH:
		if( !CheckSwitchType( fTimePerSec ) )
		{
			m_pOwner->CheckUseBuffSkill( m_Name, false );
			SetReserveEndBuff();
			return;
		}

		CheckCallingProcess();
		break;
	case OT_DELAY:
	case OT_ACTIVE:
		{
			DWORD dwCurTime = FRAMEGETTIME();

			if ( m_bGetupStateForceEnd )
			{
				if ( m_dwReserveGetupEndTime <= 0 && m_pOwner->GetBlowWoundedState() == BW_GETUP )
					m_dwReserveGetupEndTime = m_pOwner->GetGetupDurationTime();

				if ( m_dwReserveGetupEndTime > 0 && m_dwReserveGetupEndTime < dwCurTime )
				{
					SetReserveEndBuff();
					return;
				}
			}

			if( CheckEnableDelayState() )
				return;

			CheckCallingProcess();

			if( GetBuffDuration() == 0 )
				return;

			if( m_dwBuffEndTime <= dwCurTime )
			{
				m_pOwner->CheckUseBuffSkill( m_Name, false );
				SetReserveEndBuff();
				return;
			}
		}
		break;
	case OT_PASSIVE:
		CheckCallingAreaWeapon();
		break;
	}
}

void ioAffectWoundedValueBuff::GetBuffInfo( SP2Packet &rkPacket )
{
	ioBuff::GetBuffInfo(rkPacket);

	rkPacket << GetPushPowerRate();
	rkPacket << GetAirPushPowerRate();
	rkPacket << GetDownPushPowerRate();
	rkPacket << GetBlowPowerRate();
	rkPacket << GetAirBlowPowerRate();
	rkPacket << GetDownBlowPowerRate();
	rkPacket << GetBoundJumpPowerRate();
	rkPacket << GetBoundForcePowerRate();
	rkPacket << GetBoundGravityRate();
	rkPacket << GetBoundDamageRate();
	rkPacket << GetDamageRate();	
	rkPacket << GetFloatJumpPowerRate();
	rkPacket << GetFloatGravityRate();
	rkPacket << GetFloatFrictionRate();
	rkPacket << GetGetUpRate();
	rkPacket << GetFallDamageRate();
}

void ioAffectWoundedValueBuff::ApplyBuffInfo( SP2Packet &rkPacket )
{
	ioBuff::ApplyBuffInfo(rkPacket);

	rkPacket >> m_PushPower.m_fRate;
	rkPacket >> m_AirPushPower.m_fRate;
	rkPacket >> m_DownPushPower.m_fRate;
	rkPacket >> m_BlowPower.m_fRate;
	rkPacket >> m_AirBlowPower.m_fRate;
	rkPacket >> m_DownBlowPower.m_fRate;
	rkPacket >> m_BoundJumpPower.m_fRate;
	rkPacket >> m_BoundForcePower.m_fRate;
	rkPacket >> m_BoundGravity.m_fRate;
	rkPacket >> m_BoundDamage.m_fRate;
	rkPacket >> m_Damage.m_fRate;
	rkPacket >> m_FloatJumpPower.m_fRate;
	rkPacket >> m_FloatGravity.m_fRate;
	rkPacket >> m_FloatFriction.m_fRate;
	rkPacket >> m_GetUpTimeRate.m_fRate;
	rkPacket >> m_FallDamageRate.m_fRate;
}

void ioAffectWoundedValueBuff::EndBuff()
{
	ioBuff::EndBuff();
}

void ioAffectWoundedValueBuff::SetRandomRate( AffectRate &rkAffectRate )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	DWORD dwSeed = pOwner->GetRandomSeed();
	float fMin = rkAffectRate.m_fMinRate;
	float fMax = rkAffectRate.m_fMaxRate;

	if( fMin > fMax )
	{
		rkAffectRate.m_fRate = FLOAT1;
		return;
	}

	int iMin = (int)( rkAffectRate.m_fMinRate * FLOAT100 );
	int iMax = (int)( rkAffectRate.m_fMaxRate * FLOAT100 );

	IORandom random;
	random.SetRandomSeed( dwSeed + m_iSeedCount );
	int iRand = random.Random( iMax - iMin );
	iRand += iMin;

	rkAffectRate.m_fRate = (float)( iRand * 0.01f );
	m_iSeedCount++;
}

bool ioAffectWoundedValueBuff::IsExceptChar( ioHashString szChar )
{
	if( m_bExceptTarget && !m_szExceptCharName.IsEmpty() )
	{
		if( m_szExceptCharName == szChar )
			return true;
	}

	return false;
}