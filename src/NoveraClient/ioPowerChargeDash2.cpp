

#include "stdafx.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "ioPowerChargeDash2.h"


ioPowerChargeDash2::ioPowerChargeDash2()
{
	m_DashState = CDS_NONE;
	m_dwDefenseStartTime = 0;
}

ioPowerChargeDash2::ioPowerChargeDash2( const ioPowerChargeDash2 &rhs )
: ioExtendDash( rhs ),
 m_AttackAttribute( rhs.m_AttackAttribute ),
 m_szDefenseAni( rhs.m_szDefenseAni ),
 m_dwDefenseDuration( rhs.m_dwDefenseDuration ),
 m_fDefenseeSpeedRate( rhs.m_fDefenseeSpeedRate ),
 m_vDashDefenseBuff( rhs.m_vDashDefenseBuff )
{
	m_DashState = CDS_NONE;
	m_dwDefenseStartTime = 0;
}

ioExtendDash* ioPowerChargeDash2::Clone()
{
	return new ioPowerChargeDash2( *this );
}

ioPowerChargeDash2::~ioPowerChargeDash2()
{
}

DashType ioPowerChargeDash2::GetType() const
{
	return DT_POWER_CHARGE2;
}

void ioPowerChargeDash2::LoadProperty( ioINILoader &rkLoader )
{
	ioExtendDash::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];
	char szKey[MAX_PATH];

	m_dwDefenseDuration = (DWORD)rkLoader.LoadInt_e( "dash_defense_time", 0 );
	m_fDefenseeSpeedRate = rkLoader.LoadFloat_e( "dash_defense_speed_rate", 0.0f );

	rkLoader.LoadString_e( "defense_dash_animation", "", szBuf, MAX_PATH );
	m_szDefenseAni = szBuf;

	m_AttackAttribute.Init();
	LoadAttackAttribute_e( "dash_attack", m_AttackAttribute, rkLoader );

	m_vDashDefenseBuff.clear();
	int iBuffCnt = rkLoader.LoadInt_e( "dash_defense_buff_cnt", 0 );
	if( iBuffCnt > 0 )
	{
		m_vDashDefenseBuff.reserve( iBuffCnt );

		for( int i=0; i < iBuffCnt; ++i )
		{
			wsprintf_e( szKey, "dash_defense_buff%d", i+1 );
			rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

			m_vDashDefenseBuff.push_back( ioHashString( szBuf ) );
		}
	}
}

void ioPowerChargeDash2::SetPowerDash( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	m_dwDefenseStartTime = 0;

	ChangeToDefenseState( pOwner );
}

bool ioPowerChargeDash2::CheckPowerDash( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_DashState )
	{
	case CDS_DEFENSE:
		ProcessDefenseState( pOwner );
		break;
	}

	if( m_DashState == CDS_NORMAL_ATTACK )
		return true;

	return false;
}

void ioPowerChargeDash2::ProcessDefenseState( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwDefenseStartTime+m_dwDefenseDuration < dwCurTime )
	{
		ChangeToNormalAttack( pOwner );
		return;
	}
}

void ioPowerChargeDash2::ChangeToNormalAttack( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	m_DashState = CDS_NORMAL_ATTACK;
	pOwner->SetDashAttackByAttribute( m_AttackAttribute );

	/*
	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_DASH );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << CDS_NORMAL_ATTACK;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
	*/
}

void ioPowerChargeDash2::ChangeToDefenseState( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	m_DashState = CDS_DEFENSE;
	m_dwDefenseStartTime = dwCurTime;

	int iAniID = pGrp->GetAnimationIdx( m_szDefenseAni );

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	float fTimeRate = pOwner->GetRunDashAniRate();
	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, fTimeRate );

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

bool ioPowerChargeDash2::IsPowerChargingState()
{
	if( m_DashState == CDS_NONE )
	{
		return false;
	}

	return true;
}

bool ioPowerChargeDash2::IsEnableCharDirection()
{
	switch( m_DashState )
	{
	case CDS_DEFENSE:
		return false;
	}

	return true;
}

bool ioPowerChargeDash2::IsEnableDefense()
{
	switch( m_DashState )
	{
	case CDS_DEFENSE:
		return true;
	}

	return false;
}

float ioPowerChargeDash2::GetCurStateSpeedRate()
{
	switch( m_DashState )
	{
	case CDS_DEFENSE:
		return m_fDefenseeSpeedRate;
	}

	return 0.0f;
}

void ioPowerChargeDash2::PowerDashEnd( ioBaseChar *pOwner )
{
	m_DashState = CDS_NONE;
	m_dwDefenseStartTime = 0;
}

void ioPowerChargeDash2::ApplyDash( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	int iType;
	rkPacket >> iType;

	switch( iType )
	{
	case CDS_NORMAL_ATTACK:
		ChangeToNormalAttack( pOwner );
		break;
	case CDS_DEFENSE:
		m_dwDefenseStartTime = 0;

		ChangeToDefenseState( pOwner );
		break;
	}
}


