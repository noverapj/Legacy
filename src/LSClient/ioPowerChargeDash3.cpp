

#include "stdafx.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "ioPowerChargeDash3.h"


ioPowerChargeDash3::ioPowerChargeDash3()
{
	m_DashState = CDS_NONE;
	m_dwChargingStartTime = 0;
}

ioPowerChargeDash3::ioPowerChargeDash3( const ioPowerChargeDash3 &rhs )
: ioExtendDash( rhs ),
m_AttackAttribute( rhs.m_AttackAttribute ),
m_ExtendAttackAttribute( rhs.m_ExtendAttackAttribute ),
m_dwChargingTime( rhs.m_dwChargingTime ),
m_szChargingAni( rhs.m_szChargingAni ),
m_szGatheringAni( rhs.m_szGatheringAni ),
m_fChargingSpeedRate( rhs.m_fChargingSpeedRate ),
m_BlowDashInfo( rhs.m_BlowDashInfo )
{
	m_DashState = CDS_NONE;
	m_dwChargingStartTime = 0;
}

ioExtendDash* ioPowerChargeDash3::Clone()
{
	return new ioPowerChargeDash3( *this );
}

ioPowerChargeDash3::~ioPowerChargeDash3()
{
}

DashType ioPowerChargeDash3::GetType() const
{
	return DT_POWER_CHARGE3;
}

void ioPowerChargeDash3::LoadProperty( ioINILoader &rkLoader )
{
	ioExtendDash::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	m_dwChargingTime = rkLoader.LoadInt_e( "dash_charge_time", 0 );
	m_fChargingSpeedRate = rkLoader.LoadFloat_e( "dash_charge_speed_rate", 0.0f );

	rkLoader.LoadString_e( "charging_dash_animation", "", szBuf, MAX_PATH );
	m_szChargingAni = szBuf;

	rkLoader.LoadString_e( "gathering_dash_animation", "", szBuf, MAX_PATH );
	m_szGatheringAni = szBuf;

	m_AttackAttribute.Init();
	LoadAttackAttribute_e( "dash_attack", m_AttackAttribute, rkLoader );

	m_ExtendAttackAttribute.Init();
	LoadAttackAttribute_e( "extend_dash_attack", m_ExtendAttackAttribute, rkLoader );

	//
	m_BlowDashInfo;

	rkLoader.LoadString_e( "blow_dash_down_ani", "", szBuf, MAX_PATH );
	m_BlowDashInfo.m_BlowDashDownAni = szBuf;
	m_BlowDashInfo.m_dwBlowDashDownDuration = (DWORD)rkLoader.LoadInt_e( "blow_dash_down_duration", 0 );

	rkLoader.LoadString_e( "blow_dash_getup_ani", "", szBuf, MAX_PATH );
	m_BlowDashInfo.m_BlowDashGetUpAni = szBuf;
	m_BlowDashInfo.m_fBlowDashGetUpAniRate = rkLoader.LoadFloat_e( "blow_dash_getup_ani_rate", FLOAT1 );

	LoadAttackAttribute_e( "blow_dash_getup_attack", m_BlowDashInfo.m_BlowDashGetUpAttack, rkLoader );
}

void ioPowerChargeDash3::SetPowerDash( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	DWORD dwDelayTime = 0;
	DWORD dwCurTime = FRAMEGETTIME();

	m_DashState = CDS_CHARGING;
	m_dwChargingStartTime = dwCurTime;

	float fTimeRate = pOwner->GetRunDashAniRate();
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->SetLoopAni( m_szChargingAni, FLOAT100, FLOAT1, fTimeRate );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_DASH );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << (int)m_DashState;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioPowerChargeDash3::CheckPowerDash( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_DashState )
	{
	case CDS_CHARGING:
		ProcessCharging( pOwner );
		break;
	case CDS_GATHERING:
		ProcessGathering( pOwner );
		break;
	}

	if( m_DashState == CDS_NORMAL_ATTACK || m_DashState == CDS_EXTEND_ATTACK )
		return true;

	return false;
}

void ioPowerChargeDash3::ProcessCharging( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( pOwner->IsAttackKeyDown() )
	{
		if( m_dwChargingStartTime+m_dwChargingTime < dwCurTime )
		{
			//ChangeToGathering( pOwner );
			ChangeToExtendAttack( pOwner );
			return;
		}
	}
	else
	{
		ChangeToNormalAttack( pOwner );
		return;
	}
}

void ioPowerChargeDash3::ProcessGathering( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if( !pOwner->IsAttackKeyDown() )
	{
		ChangeToExtendAttack( pOwner );
	}
}

void ioPowerChargeDash3::ChangeToGathering( ioBaseChar *pOwner )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	int iAniID = pGrp->GetAnimationIdx( m_szGatheringAni );

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	pGrp->SetLoopAni( iAniID, FLOAT100 );

	m_DashState = CDS_GATHERING;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_DASH );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << (int)m_DashState;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioPowerChargeDash3::ChangeToNormalAttack( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	m_DashState = CDS_NORMAL_ATTACK;
	pOwner->SetDashAttackByAttribute( m_AttackAttribute );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_DASH );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << CDS_NORMAL_ATTACK;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioPowerChargeDash3::ChangeToExtendAttack( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	m_DashState = CDS_EXTEND_ATTACK;
	pOwner->SetBlowDashAttackByAttribute( m_ExtendAttackAttribute, m_BlowDashInfo );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_DASH );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << CDS_EXTEND_ATTACK;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioPowerChargeDash3::IsPowerChargingState()
{
	if( m_DashState == CDS_NONE )
	{
		return false;
	}

	return true;
}

void ioPowerChargeDash3::PowerDashEnd( ioBaseChar *pOwner )
{
	m_DashState = CDS_NONE;
	m_dwChargingStartTime = 0;
}

void ioPowerChargeDash3::ApplyDash( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	DWORD dwDelayTime = 0;
	DWORD dwCurTime = FRAMEGETTIME();

	int iType;
	rkPacket >> iType;

	switch( iType )
	{
	case CDS_CHARGING:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			pOwner->SetTargetRot( qtRot );
			SetPowerDash( pOwner );
		}
		break;
	case CDS_GATHERING:
		ChangeToGathering( pOwner );
		break;
	case CDS_NORMAL_ATTACK:
		ChangeToNormalAttack( pOwner );
		break;
	case CDS_EXTEND_ATTACK:
		ChangeToExtendAttack( pOwner );
		break;
	}
}


