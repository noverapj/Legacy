

#include "stdafx.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "ioShaolinDash.h"


#include "ItemDefine.h"

ioShaolinDash::ioShaolinDash()
{
	m_DashState = CDS_NONE;
	m_dwChargingStartTime = 0;
}

ioShaolinDash::ioShaolinDash( const ioShaolinDash &rhs )
: ioExtendDash( rhs ),
 m_AttackAttribute( rhs.m_AttackAttribute ),
 m_AttackAttribute_Monk( rhs.m_AttackAttribute_Monk ),
 m_ExtendAttackAttribute( rhs.m_ExtendAttackAttribute ),
 m_ExtendAttackAttribute_Monk( rhs.m_ExtendAttackAttribute_Monk ),
 //m_ExtraAttackAttribute( rhs.m_ExtraAttackAttribute ),
 m_dwChargingTime( rhs.m_dwChargingTime ),
 m_szChargingAni( rhs.m_szChargingAni ),
 m_szChargingAni_Monk( rhs.m_szChargingAni_Monk ),
 m_szGatheringAni( rhs.m_szGatheringAni ),
 m_fChargingSpeedRate( rhs.m_fChargingSpeedRate )
{
	m_DashState = CDS_NONE;
	m_dwChargingStartTime = 0;
}

ioExtendDash* ioShaolinDash::Clone()
{
	return new ioShaolinDash( *this );
}

ioShaolinDash::~ioShaolinDash()
{
}

DashType ioShaolinDash::GetType() const
{
	return DT_SHAOLIN;
}

void ioShaolinDash::LoadProperty( ioINILoader &rkLoader )
{
	ioExtendDash::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	m_dwChargingTime = rkLoader.LoadInt_e( "dash_charge_time", 0 );
	m_fChargingSpeedRate = rkLoader.LoadFloat_e( "dash_charge_speed_rate", 0.0f );

	rkLoader.LoadString_e( "charging_dash_animation", "", szBuf, MAX_PATH );
	m_szChargingAni = szBuf;

	rkLoader.LoadString_e( "charging_dash_monk_animation", "", szBuf, MAX_PATH );
	m_szChargingAni_Monk = szBuf;

	rkLoader.LoadString_e( "gathering_dash_animation", "", szBuf, MAX_PATH );
	m_szGatheringAni = szBuf;

	m_AttackAttribute.Init();
	LoadAttackAttribute_e( "dash_attack", m_AttackAttribute, rkLoader );

	m_AttackAttribute_Monk.Init();
	LoadAttackAttribute_e( "dash_monk_attack", m_AttackAttribute_Monk, rkLoader );


	m_ExtendAttackAttribute.Init();
	LoadAttackAttribute_e( "extend_dash_attack", m_ExtendAttackAttribute, rkLoader );

	m_ExtendAttackAttribute_Monk.Init();
	LoadAttackAttribute_e( "extend_dash_monk_attack", m_ExtendAttackAttribute_Monk, rkLoader );

	//m_ExtraAttackAttribute.Init();
	//LoadAttackAttribute_e( "extra_dash_attack", m_ExtraAttackAttribute, rkLoader );
}

void ioShaolinDash::SetPowerDash( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	ioItem *pItem = pOwner->GetEquipedItem( ES_WEAPON );
	ioShaolinItem *pShaolinItem = ToShaolinItem( pItem );
	if( !pShaolinItem )
		return;

	DWORD dwDelayTime = 0;
	DWORD dwCurTime = FRAMEGETTIME();

	m_DashState = CDS_CHARGING;
	m_dwChargingStartTime = dwCurTime;

	float fTimeRate = pOwner->GetRunDashAniRate();
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	int bNormalTypeMesh = pShaolinItem->GetMeshType() == ioShaolinItem::IMT_NORMAL ? true : false;
	if( bNormalTypeMesh )	
		pGrp->SetLoopAni( m_szChargingAni, FLOAT100, FLOAT1, fTimeRate );
	else
		pGrp->SetLoopAni( m_szChargingAni_Monk, FLOAT100, FLOAT1, fTimeRate );

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

bool ioShaolinDash::CheckPowerDash( ioBaseChar *pOwner )
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

void ioShaolinDash::ProcessCharging( ioBaseChar *pOwner )
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

			ioItem *pItem = pOwner->GetEquipedItem( ES_WEAPON );
			ioShaolinItem *pShaolinItem = ToShaolinItem( pItem );
			if( !pShaolinItem )
				return;

			if( pShaolinItem->GetCurBullet() >= pShaolinItem->GetNeedBullet() )
			{
				pShaolinItem->DecreaseGauge();
				ChangeToExtendAttack( pOwner );
			}
			else
			{
				ChangeToNormalAttack( pOwner );
			}
		}
	}
	else
	{
		ChangeToNormalAttack( pOwner );
	}
}

void ioShaolinDash::ProcessGathering( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if( !pOwner->IsAttackKeyDown() )
	{
		ChangeToExtendAttack( pOwner );
	}
}

void ioShaolinDash::ChangeToGathering( ioBaseChar *pOwner )
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

void ioShaolinDash::ChangeToNormalAttack( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	m_DashState = CDS_NORMAL_ATTACK;

	ioItem *pItem = pOwner->GetEquipedItem( ES_WEAPON );
	ioShaolinItem *pShaolinItem = ToShaolinItem( pItem );
	if( !pShaolinItem )
		return;

	int bNormalTypeMesh = pShaolinItem->GetMeshType() == ioShaolinItem::IMT_NORMAL ? true : false;
	if( bNormalTypeMesh )	
		pOwner->SetDashAttackByAttribute( m_AttackAttribute );
	else
		pOwner->SetDashAttackByAttribute( m_AttackAttribute_Monk );

	if( pOwner->IsNeedProcess() )
		pShaolinItem->ChangeAttackType( pOwner, pShaolinItem->GetMeshType(), true );

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

void ioShaolinDash::ChangeToExtendAttack( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	if( pOwner->IsNeedProcess() )
	{
		ioItem *pItem = pOwner->GetEquipedItem( ES_WEAPON );
		ioShaolinItem *pShaolinItem = ToShaolinItem( pItem );
		if( pShaolinItem )
			pShaolinItem->ChangeToggleAttckType( pOwner );
	}

	m_DashState = CDS_EXTEND_ATTACK;

	ioItem *pItem = pOwner->GetEquipedItem( ES_WEAPON );
	ioShaolinItem *pShaolinItem = ToShaolinItem( pItem );
	if( !pShaolinItem )
		return;

	int bNormalTypeMesh = pShaolinItem->GetMeshType() == ioShaolinItem::IMT_NORMAL ? true : false;
	if( bNormalTypeMesh )	
		pOwner->SetDashAttackByAttribute( m_ExtendAttackAttribute );
	else
		pOwner->SetDashAttackByAttribute( m_ExtendAttackAttribute_Monk );

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

bool ioShaolinDash::IsPowerChargingState()
{
	if( m_DashState == CDS_NONE )
	{
		return false;
	}

	return true;
}

void ioShaolinDash::PowerDashEnd( ioBaseChar *pOwner )
{
	m_DashState = CDS_NONE;
	m_dwChargingStartTime = 0;
}

void ioShaolinDash::ApplyDash( ioBaseChar *pOwner, SP2Packet &rkPacket )
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

bool ioShaolinDash::IsEnableExtraDashAttack()
{
	switch( m_DashState )
	{
	case CDS_NORMAL_ATTACK:
		return true;
	case CDS_EXTEND_ATTACK:
		return false;
	}

	return false;
}

bool ioShaolinDash::IsEnableExtraOtherDashAttack()
{
	switch( m_DashState )
	{
	case CDS_NORMAL_ATTACK:
		return true;
	case CDS_EXTEND_ATTACK:
		return false;
	}

	return false;
}


