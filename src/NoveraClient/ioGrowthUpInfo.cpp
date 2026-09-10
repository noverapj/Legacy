
#include "stdafx.h"

#include "ioGrowthUpInfo.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

ioGrowthUpInfo::ioGrowthUpInfo()
{
	m_iCurLevel = 0;
	m_nCurItemReinForceLv = 0;
}

ioGrowthUpInfo::~ioGrowthUpInfo()
{
	m_iCurLevel = 0;
}

void ioGrowthUpInfo::LoadProperty( ioINILoader &rkLoader )
{
}

void ioGrowthUpInfo::SetCurLevel( int iLevel )
{
	m_iCurLevel = iLevel;
	m_iCurLevel = max( 0, m_iCurLevel );
}


/*void ioGrowthUpInfo::SetItemReinForceLv( int nLv )
{
	m_nCurItemReinForceLv = nLv;
}*/

//////////////////////////////////////////////////////////////////////////////////////////
ioGrowthNormalUpInfo::ioGrowthNormalUpInfo()
: ioGrowthUpInfo()
{
}

ioGrowthNormalUpInfo::~ioGrowthNormalUpInfo()
{
}

void ioGrowthNormalUpInfo::LoadProperty( ioINILoader &rkLoader )
{
	ioGrowthUpInfo::LoadProperty( rkLoader );

	m_fMaxValue = rkLoader.LoadFloat_e( "max_value", 0.0f );

	m_fMaxValue_Section1 = rkLoader.LoadFloat_e( "max_value_section1", 0.0f );
	m_fMaxValue_Section2 = rkLoader.LoadFloat_e( "max_value_section2", 0.0f );
	m_fMaxValue_Section3 = rkLoader.LoadFloat_e( "max_value_section3", 0.0f );

	m_nMaxValue_SectionGap1 = rkLoader.LoadInt_e( "max_value_section1_gap", 0 );
	m_nMaxValue_SectionGap2 = rkLoader.LoadInt_e( "max_value_section2_gap", 0 );

	//m_nNormalItemMaxReinForceLv = rkLoader.LoadInt_e( "max_normal_reinforce", 1000 );
}

float ioGrowthNormalUpInfo::GetValue( ioPlayStage *pStage )
{
#ifdef BALANCE_RENEWAL
	int iCurMax = g_GrowthInfoMgr.GetMaxLevel();
	if( iCurMax == 0 )
		return m_fMaxValue_Section1;

	float fCurValue = 0.f;
	if( m_iCurLevel <= m_nMaxValue_SectionGap1 )
	{
		fCurValue = m_fMaxValue_Section1 / iCurMax;
		fCurValue *= m_iCurLevel;
	}
	else if( m_iCurLevel <= m_nMaxValue_SectionGap2 )
	{
		fCurValue = m_fMaxValue_Section1 / iCurMax;
		fCurValue *= m_nMaxValue_SectionGap1;

		float fCurValue2 = m_fMaxValue_Section2 / iCurMax;
		fCurValue2 *= (m_iCurLevel - m_nMaxValue_SectionGap1 );

		fCurValue += fCurValue2;
	}
	else if( m_iCurLevel > m_nMaxValue_SectionGap2 )
	{
		fCurValue = m_fMaxValue_Section1 / iCurMax;
		fCurValue *= m_nMaxValue_SectionGap1;

		float fCurValue2 = m_fMaxValue_Section2 / iCurMax;
		fCurValue2 *= (m_nMaxValue_SectionGap2 - m_nMaxValue_SectionGap1);

		float fCurValue3 = m_fMaxValue_Section3 / iCurMax;
		fCurValue3 *= (m_iCurLevel - m_nMaxValue_SectionGap2 );

		fCurValue = fCurValue + fCurValue2 + fCurValue3;
	}
	else
		return m_fMaxValue_Section1;

	//아이템 강화( 25강 이후 )
	
	//fCurValue += g_GrowthInfoMgr.GetRebalanceHightLevelValue( m_nCurItemReinForceLv );	//100.f;

	if( !pStage || !pStage->CheckRoomGrowthUse() )
		fCurValue = 0.f;

	return fCurValue; 
#else
	int iCurMax = g_GrowthInfoMgr.GetMaxLevel();
	if( iCurMax == 0 )
		return m_fMaxValue;

	float fCurValue = m_fMaxValue / iCurMax;

	if( pStage && pStage->CheckRoomGrowthUse() )
		fCurValue *= m_iCurLevel;
	else
		fCurValue = 0.0f;

	return fCurValue; 
#endif
}


//////////////////////////////////////////////////////////////////////////////////////////
ioGrowthAttackUpInfo::ioGrowthAttackUpInfo()
: ioGrowthUpInfo()
{
}

ioGrowthAttackUpInfo::~ioGrowthAttackUpInfo()
{
}

void ioGrowthAttackUpInfo::LoadProperty( ioINILoader &rkLoader )
{
	ioGrowthUpInfo::LoadProperty( rkLoader );

	m_fDamageRate = rkLoader.LoadFloat_e( "max_damage_rate", 0.0f );

	m_fDamageRate_Section1 = rkLoader.LoadFloat_e( "max_damage_rate_section1", 0.0f );
	m_fDamageRate_Section2 = rkLoader.LoadFloat_e( "max_damage_rate_section2", 0.0f );
	m_fDamageRate_Section3 = rkLoader.LoadFloat_e( "max_damage_rate_section3", 0.0f );

	m_nDamageRate_Section1_gap = rkLoader.LoadFloat_e( "max_damage_rate_section1_gap", 0.0f );
	m_nDamageRate_Section2_gap = rkLoader.LoadFloat_e( "max_damage_rate_section2_gap", 0.0f );

	m_fPushRate = rkLoader.LoadFloat_e( "max_push_rate", 0.0f );
	m_fAirPushRate = rkLoader.LoadFloat_e( "max_air_push_rate", 0.0f );

	m_fBlowRate = rkLoader.LoadFloat_e( "max_blow_rate", 0.0f );
	m_fAirBlowRate = rkLoader.LoadFloat_e( "max_air_blow_rate", 0.0f );
}

float ioGrowthAttackUpInfo::GetDamageRate( ioPlayStage *pStage )
{
#ifdef BALANCE_RENEWAL
	int iCurMax = g_GrowthInfoMgr.GetMaxLevel();
	if( iCurMax == 0 )
		return m_fDamageRate_Section1;

	float fCurDamage = 0.f;
	if( m_iCurLevel <= m_nDamageRate_Section1_gap )
	{
		fCurDamage = m_fDamageRate_Section1 / iCurMax;
		fCurDamage *= m_iCurLevel;
	}
	else if( m_iCurLevel <= m_nDamageRate_Section2_gap )
	{
		fCurDamage = m_fDamageRate_Section1 / iCurMax;
		fCurDamage *= m_nDamageRate_Section1_gap;

		float fCurDamage2 = m_fDamageRate_Section2 / iCurMax;
		fCurDamage2 *= (m_iCurLevel - m_nDamageRate_Section1_gap );

		fCurDamage += fCurDamage2;
	}
	else if( m_iCurLevel > m_nDamageRate_Section2_gap )
	{
		fCurDamage = m_fDamageRate_Section1 / iCurMax;
		fCurDamage *= m_nDamageRate_Section1_gap;

		float fCurDamage2 = m_fDamageRate_Section2 / iCurMax;
		fCurDamage2 *= ( m_nDamageRate_Section2_gap - m_nDamageRate_Section1_gap );

		float fCurDamage3 = m_fDamageRate_Section3 / iCurMax;
		fCurDamage3 *= ( m_iCurLevel - m_nDamageRate_Section2_gap );

		fCurDamage = fCurDamage + fCurDamage2 + fCurDamage3;
	}
	else 
		return m_fDamageRate_Section1;

	if( !pStage || !pStage->CheckRoomGrowthUse() )
		fCurDamage = 0.0f;

	return fCurDamage;
#else
	int iCurMax = g_GrowthInfoMgr.GetMaxLevel();
	if( iCurMax == 0 )
		return m_fDamageRate;

	float fCurDamage = m_fDamageRate / iCurMax;

	if( pStage && pStage->CheckRoomGrowthUse() )
		fCurDamage *= m_iCurLevel;
	else
		fCurDamage = 0.0f;

	return fCurDamage;
#endif
}

float ioGrowthAttackUpInfo::GetPushRate(ioPlayStage *pStage)
{
	int iCurMax = g_GrowthInfoMgr.GetMaxLevel();
	if( iCurMax == 0 )
		return m_fPushRate;

	float fPushRate = m_fPushRate / iCurMax;

	if( pStage && pStage->CheckRoomGrowthUse() )
		fPushRate *= m_iCurLevel;
	else
		fPushRate = 0.0f;

	return fPushRate;
}

float ioGrowthAttackUpInfo::GetAirPushRate(ioPlayStage *pStage)
{
	int iCurMax = g_GrowthInfoMgr.GetMaxLevel();
	if( iCurMax == 0 )
		return m_fAirPushRate;

	float fAirPushRate = m_fAirPushRate / iCurMax;

	if( pStage && pStage->CheckRoomGrowthUse() )
		fAirPushRate *= m_iCurLevel;
	else
		fAirPushRate = 0.0f;

	return fAirPushRate;
}

float ioGrowthAttackUpInfo::GetBlowRate(ioPlayStage *pStage)
{
	int iCurMax = g_GrowthInfoMgr.GetMaxLevel();
	if( iCurMax == 0 )
		return m_fBlowRate;

	float fBlowRate = m_fBlowRate / iCurMax;

	if( pStage && pStage->CheckRoomGrowthUse() )
		fBlowRate *= m_iCurLevel;
	else
		fBlowRate = 0.0f;

	return fBlowRate;
}

float ioGrowthAttackUpInfo::GetAirBlowRate(ioPlayStage *pStage)
{
	int iCurMax = g_GrowthInfoMgr.GetMaxLevel();
	if( iCurMax == 0 )
		return m_fAirBlowRate;

	float fAirBlowRate = m_fAirBlowRate / iCurMax;

	if( pStage && pStage->CheckRoomGrowthUse() )
		fAirBlowRate *= m_iCurLevel;
	else
		fAirBlowRate = 0.0f;

	return fAirBlowRate;
}

//////////////////////////////////////////////////////////////////////////////////////////
ioGrowthBlowPushUpInfo::ioGrowthBlowPushUpInfo()
: ioGrowthUpInfo()
{
}

ioGrowthBlowPushUpInfo::~ioGrowthBlowPushUpInfo()
{
}

void ioGrowthBlowPushUpInfo::LoadProperty( ioINILoader &rkLoader )
{
	ioGrowthUpInfo::LoadProperty( rkLoader );

	m_fPushRate = rkLoader.LoadFloat_e( "max_push_rate", 0.0f );
	m_fBlowRate = rkLoader.LoadFloat_e( "max_blow_rate", 0.0f );
}

float ioGrowthBlowPushUpInfo::GetBlowRate(ioPlayStage *pStage)
{
	int iCurMax = g_GrowthInfoMgr.GetMaxLevel();
	if( iCurMax == 0 )
		return m_fBlowRate;

	float fBlowRate = m_fBlowRate / iCurMax;

	if( pStage && pStage->CheckRoomGrowthUse() )
		fBlowRate *= m_iCurLevel;
	else
		fBlowRate = 0.0f;

	return fBlowRate;
}

float ioGrowthBlowPushUpInfo::GetPushRate(ioPlayStage *pStage)
{
	int iCurMax = g_GrowthInfoMgr.GetMaxLevel();
	if( iCurMax == 0 )
		return m_fPushRate;

	float fPushRate = m_fPushRate / iCurMax;
	if( pStage && pStage->CheckRoomGrowthUse() )
		fPushRate *= m_iCurLevel;
	else
		fPushRate = 0.0f;

	return fPushRate;
}

//////////////////////////////////////////////////////////////////////////////////////////

ioGrowthTeleportUpInfo::ioGrowthTeleportUpInfo()
: ioGrowthUpInfo()
{
}

ioGrowthTeleportUpInfo::~ioGrowthTeleportUpInfo()
{
}

void ioGrowthTeleportUpInfo::LoadProperty( ioINILoader &rkLoader )
{
	ioGrowthUpInfo::LoadProperty( rkLoader );

	m_fAniRate = rkLoader.LoadFloat_e( "max_ani_rate", 0.0f );
	m_fEndAniRate = rkLoader.LoadFloat_e( "max_end_ani_rate", 0.0f );
}

float ioGrowthTeleportUpInfo::GetAniRate(ioPlayStage *pStage)
{
	int iCurMax = g_GrowthInfoMgr.GetMaxLevel();
	if( iCurMax == 0 )
		return m_fAniRate;

	float fAniRate = m_fAniRate / iCurMax;

	if( pStage && pStage->CheckRoomGrowthUse() )
		fAniRate *= m_iCurLevel;
	else
		fAniRate = 0.0f;

	return fAniRate;
}

float ioGrowthTeleportUpInfo::GetEndAniRate(ioPlayStage *pStage)
{
	int iCurMax = g_GrowthInfoMgr.GetMaxLevel();
	if( iCurMax == 0 )
		return m_fEndAniRate;

	float fEndAniRate = m_fEndAniRate / iCurMax;
	
	if( pStage && pStage->CheckRoomGrowthUse() )
		fEndAniRate *= m_iCurLevel;
	else
		fEndAniRate = 0.0f;

	return fEndAniRate;
}


//////////////////////////////////////////////////////////////////////////////////////////

ioGrowthAttackForceUpInfo::ioGrowthAttackForceUpInfo()
: ioGrowthUpInfo()
{
}

ioGrowthAttackForceUpInfo::~ioGrowthAttackForceUpInfo()
{
}

void ioGrowthAttackForceUpInfo::LoadProperty( ioINILoader &rkLoader )
{
	ioGrowthUpInfo::LoadProperty( rkLoader );

	m_fForceValue = rkLoader.LoadFloat_e( "max_force_value", 0.0f );
	m_fFrictionValue = rkLoader.LoadFloat_e( "max_friction_value", 0.0f );
}

float ioGrowthAttackForceUpInfo::GetForceValue(ioPlayStage *pStage)
{
	int iCurMax = g_GrowthInfoMgr.GetMaxLevel();
	if( iCurMax == 0 )
		return m_fForceValue;

	float fForce = m_fForceValue / iCurMax;

	if( pStage && pStage->CheckRoomGrowthUse() )
		fForce *= m_iCurLevel;
	else
		fForce = 0.0f;

	return fForce;
}

float ioGrowthAttackForceUpInfo::GetFrictionValue(ioPlayStage *pStage)
{
	int iCurMax = g_GrowthInfoMgr.GetMaxLevel();
	if( iCurMax == 0 )
		return m_fFrictionValue;

	float fFriction = m_fFrictionValue / iCurMax;

	if( pStage && pStage->CheckRoomGrowthUse() )
		fFriction *= m_iCurLevel;
	else
		fFriction = 0.0f;

	return fFriction;
}


//////////////////////////////////////////////////////////////////////////////////////////
