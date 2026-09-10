
#include "stdafx.h"

#include "ioEngineerGatlingDummy.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "WeaponDefine.h"
#include "ItemDefine.h"

ioEngineerGatlingDummy::ioEngineerGatlingDummy( ioEntityGroup *pGrp, ioPlayMode *pMode )
	: ioDummyChar( pGrp, pMode )
{
	m_GatlingState = GS_DEALY;
}

ioEngineerGatlingDummy::~ioEngineerGatlingDummy()
{
}

void ioEngineerGatlingDummy::LoadProperty( ioINILoader &rkLoader )
{
	ioDummyChar::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";

	//Charging
	rkLoader.LoadString_e( "charging_animation", "", szBuf, MAX_PATH );
	m_szChargingAnimation = szBuf;	
	m_fChargingAnimationRate = rkLoader.LoadFloat_e( "charging_animation_rate", FLOAT1 );

	//Bullet Attack	
	rkLoader.LoadString_e( "gatling_attack_animation", "", szBuf, MAX_PATH );
	m_szGatlingAttackAnimation = szBuf;	
	m_fGatlingAttackAnimationRate = rkLoader.LoadFloat_e( "gatling_attack_animation_rate", FLOAT1 );

	//Beam Attack
	rkLoader.LoadString_e( "beam_attack_animation", "", szBuf, MAX_PATH );
	m_szBeamAttackAnimation = szBuf;
	m_fBeamAttackAnimationRate = rkLoader.LoadFloat_e( "beam_attack_animation_rate", FLOAT1 );

	//ShockAttack
	rkLoader.LoadString_e( "shock_attack_animation", "", szBuf, MAX_PATH );
	m_szShockAttackAnimation = szBuf;
	m_fShockAttackAnimationRate = rkLoader.LoadFloat_e( "shock_attack_animation_rate", FLOAT1 );

	//Air Trace Attack
	rkLoader.LoadString_e( "air_trace_attack_animation", "", szBuf, MAX_PATH );
	m_szAirTraceAttackAnimation = szBuf;
	m_fAirTraceAttackAnimationRate = rkLoader.LoadFloat_e( "air_trace_attack_animation_rate", FLOAT1 );
}

void ioEngineerGatlingDummy::ProcessState( float fTimePerSec, DWORD dwPreTime )
{
	switch( m_GatlingState )
	{
	case GS_START:
		ProcessStartState( fTimePerSec );
		break;
	case GS_DEALY:
		ProcessDelayState( fTimePerSec );
		break;
	}

	ProcessPos( fTimePerSec );
	ProcessEffectUpdate( fTimePerSec );
}

void ioEngineerGatlingDummy::ProcessPos( float fTimePerSec )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	SetWorldPosition( pOwner->GetWorldPosition() );
	SetWorldOrientation( pOwner->GetTargetRot() );
}

void ioEngineerGatlingDummy::ProcessStartState( float fTimePerSec )
{
	if( m_dwStartAniEndTime < FRAMEGETTIME() )
	{
		SetLoopDelayState();
	}
	else if( m_dwStartAniEndTime == 0 )
	{
		SetDieState( true );
	}
}

void ioEngineerGatlingDummy::SetActionAni( ioHashString szAnimation, float fAniRate )
{
	int iAniID = m_pGroup->GetAnimationIdx( szAnimation );
	if( iAniID != -1 )
	{
		m_pGroup->ClearAllActionAni( FLOAT10, true );
		m_pGroup->ClearAllLoopAni( FLOAT10, true );
		m_pGroup->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1 / fAniRate );
	}
}

void ioEngineerGatlingDummy::SetLoopAni( ioHashString szAnimation, float fAniRate )
{
	int iAniID = m_pGroup->GetAnimationIdx( szAnimation );
	if( iAniID != -1 )
	{
		m_pGroup->ClearAllActionAni( FLOAT10, true );
		m_pGroup->ClearAllLoopAni( FLOAT10, true );
		m_pGroup->SetLoopAni( iAniID, FLOAT10, FLOAT1, FLOAT1/fAniRate );
	}
}

void ioEngineerGatlingDummy::SetLoopDelayState()
{
	m_GatlingState = GS_DEALY;
	SetLoopAni( m_DelayAnimation, m_fDelayAniRate );
}

void ioEngineerGatlingDummy::ProcessChargingState( float fTimePerSec )
{

}

void ioEngineerGatlingDummy::ProcessDelayState( float fTimePerSec )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	if( pOwner->GetState() != CS_USING_SKILL )
	{
		SetDieState( true );
	}
}

void ioEngineerGatlingDummy::SetChargingState()
{
	m_GatlingState = GS_CHARGING;
	SetLoopAni( m_szChargingAnimation, m_fChargingAnimationRate );
}

void ioEngineerGatlingDummy::SetGatlingAttackState()
{
	m_GatlingState = GS_GATLING_ATTACK;
	SetActionAni( m_szGatlingAttackAnimation, m_fGatlingAttackAnimationRate );
}

void ioEngineerGatlingDummy::SetBeamAttackState()
{
	m_GatlingState = GS_BEAM_ATTACK;
	SetActionAni( m_szBeamAttackAnimation, m_fBeamAttackAnimationRate );
}

void ioEngineerGatlingDummy::SetShockAttackState()
{
	m_GatlingState = GS_SHOCK_ATTACK;
	SetActionAni( m_szShockAttackAnimation, m_fShockAttackAnimationRate );
}

void ioEngineerGatlingDummy::SetAirTraceAttack()
{
	m_GatlingState = GS_AIR_TRACE_ATTACK;
	SetActionAni( m_szAirTraceAttackAnimation, m_fShockAttackAnimationRate );
}

int ioEngineerGatlingDummy::DontMoveEntityLevel() const
{
	return 3;
}