

#include "stdafx.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "ioRunAssaultDash.h"


#include "ItemDefine.h"
#include "WeaponDefine.h"

ioRunAssaultDash::ioRunAssaultDash()
{
	m_RunDashState				= RDS_NONE;

	m_dwActionEndTime			= 0;
	m_dwReserveEnableTime		= 0;
	m_dwRunDashChargingStart	= 0;
	m_dwExtendDashChargingStart	= 0;
	m_iSkillInput				= -1;

	m_bEnableDefenseState		= false;
	m_bEnableAttackState		= false;
	m_bEnableJumpState			= false;
}

ioRunAssaultDash::ioRunAssaultDash( const ioRunAssaultDash &rhs )
: ioExtendDash( rhs ),
 m_AttackAttribute( rhs.m_AttackAttribute ),
 m_fOffSet( rhs.m_fOffSet ),
 m_CircleEffectRed( rhs.m_CircleEffectRed ),
 m_CircleEffectBlue( rhs.m_CircleEffectBlue ),
 m_RunDashChargingAni( rhs.m_RunDashChargingAni ),
 m_dwRunDashChargingTime( rhs.m_dwRunDashChargingTime ),
 m_ExtendDashChargingAnimation( rhs.m_ExtendDashChargingAnimation ),
 m_dwExtendDashChargingTime( rhs.m_dwExtendDashChargingTime ),
 m_DashExtedState( rhs.m_DashExtedState ),
 m_AssaultType( rhs.m_AssaultType )
{
	m_RunDashState				= RDS_NONE;

	m_dwActionEndTime			= 0;
	m_dwReserveEnableTime		= 0;
	m_dwRunDashChargingStart	= 0;
	m_dwExtendDashChargingStart	= 0;

	m_iSkillInput				= -1;
	
	m_bEnableDefenseState		= false;
	m_bEnableAttackState		= false;
	m_bEnableJumpState			= false;
}

ioExtendDash* ioRunAssaultDash::Clone()
{
	return new ioRunAssaultDash( *this );
}

ioRunAssaultDash::~ioRunAssaultDash()
{
}

DashType ioRunAssaultDash::GetType() const
{
	return DT_RUN_ASSAULT;
}

void ioRunAssaultDash::LoadProperty( ioINILoader &rkLoader )
{
	ioExtendDash::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	m_DashExtedState = static_cast<DashExtedState>( rkLoader.LoadInt_e( "dash_extend_type", 0 ) );
	m_AssaultType = static_cast<AssaultType>( rkLoader.LoadInt_e( "assault_type", 0 ) );

	rkLoader.LoadString_e( "circle_effect_red", "", szBuf, MAX_PATH );
	m_CircleEffectRed = szBuf;
	rkLoader.LoadString_e( "circle_effect_blue", "", szBuf, MAX_PATH );
	m_CircleEffectBlue = szBuf;

	m_fOffSet = rkLoader.LoadFloat_e( "circle_offset", 0.0f );

	rkLoader.LoadString_e( "run_assault_charging_ani", "", szBuf, MAX_PATH );
	m_RunDashChargingAni	= szBuf;
	m_dwRunDashChargingTime	= (DWORD)rkLoader.LoadInt_e( "run_assault_charging_time", 0 );
	LoadAttackAttribute_e( "assault_dash_attack", m_AttackAttribute, rkLoader );

	rkLoader.LoadString_e( "extend_dash_charging_animation", "", szBuf, MAX_PATH );
	m_ExtendDashChargingAnimation	= szBuf;
	m_dwExtendDashChargingTime		= (DWORD)rkLoader.LoadInt_e( "extend_dash_charging_time", 0 );
}

bool ioRunAssaultDash::StartDash( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	ioExtendDash::StartDash( pOwner );

	m_dwActionEndTime = 0;

	pOwner->InitExtendAttackTagInfo();
	pOwner->SetState( CS_EXTEND_DASH );

	ChangeToCharging( pOwner );

	return true;
}

void ioRunAssaultDash::ChangeToCharging( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	m_dwRunDashChargingStart	= FRAMEGETTIME();
	m_RunDashState				= RDS_CHARGING;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	int iAniID = pGrp->GetAnimationIdx( m_RunDashChargingAni );

	pGrp->SetLoopAni( iAniID, FLOAT100 );
}

void ioRunAssaultDash::ChangeToAssault( ioBaseChar *pOwner )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_AttackAttribute.m_AttackAnimation );

	float fTimeRate = m_AttackAttribute.m_fAttackAniRate;

	m_dwActionEndTime		= 0;
	m_dwReserveEnableTime	= 0;

	m_iSkillInput			= -1;

	m_bEnableDefenseState	= false;
	m_bEnableAttackState	= false;
	m_bEnableJumpState		= false;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pOwner->RefreshFireTimeList( iAniID,
								 m_AttackAttribute.m_vWeaponInfoList,
								 FTT_EXTEND_DASH_ATTACK,
								 fTimeRate,
								 m_AttackAttribute.m_dwPreDelay );

	pOwner->IncreaseWeaponIndexBase();
	pOwner->SetNoWoundState( false );

	pOwner->InitExtendAttackTagInfo();
	pOwner->SetState( CS_EXTEND_DASH );
	pOwner->SetAutoTarget( ATT_DASH );
	pGrp->SetActionAni( iAniID,	FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, (float)m_AttackAttribute.m_dwPreDelay, true );

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwDuration = m_AttackAttribute.m_dwPreDelay + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + m_AttackAttribute.m_dwEndDelay;

	CheckKeyReserveTime( pOwner, iAniID, fTimeRate );

	m_dwActionEndTime = dwCurTime;
	m_dwActionEndTime += dwDuration;

	pOwner->SetReservedSliding( m_AttackAttribute.m_vForceInfoList, iAniID, fTimeRate, m_AttackAttribute.m_dwPreDelay );

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, m_AttackAttribute.m_dwPreDelay );
	pOwner->SetBaseAttackExtraAniJump( iAniID, fTimeRate, m_AttackAttribute.m_dwPreDelay );

	DWORD dwTrackingTime = FRAMEGETTIME() + m_AttackAttribute.m_dwPreDelay;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	pOwner->ReduceCurSkillGaugeBySkill( NULL, m_fDashExtraGauge );

	m_RunDashState = RDS_ASSAULT;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_DASH );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << m_RunDashState;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioRunAssaultDash::ChangeToRun( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	m_RunDashState = RDS_RUN;
	pOwner->SetRunDashState();

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_DASH );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << m_RunDashState;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioRunAssaultDash::ChangeToExtendCharging( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	m_dwExtendDashChargingStart	= FRAMEGETTIME();
	m_RunDashState				= RDS_EXTEND_CHARGING;

	pOwner->InitExtendAttackTagInfo();
	pOwner->SetState( CS_EXTEND_DASH );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	int iAniID = pGrp->GetAnimationIdx( m_ExtendDashChargingAnimation );

	pGrp->SetLoopAni( iAniID, FLOAT100 );
}

void ioRunAssaultDash::SetExtraInfoToWeapon( ioBaseChar *pOwner, ioPlayStage *pStage, ioWeapon *pWeapon, const ioHashString &szName )
{
	// CheckPos : 이펙트 출력위치
	D3DXVECTOR3 vOffSetDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	vOffSetDir = m_fOffSet * vOffSetDir;

	D3DXVECTOR3 vCreatePos = pOwner->GetMidPositionByRate();

	if( m_fOffSet > 0.0f )
		vCreatePos += vOffSetDir;
	vCreatePos.y = pStage->GetMapHeightNoneEntity( vCreatePos.x, vCreatePos.z );

	// Effect
	ioHashString szEffectName;
	TeamType eTeam = pOwner->GetTeam();
	
	if( eTeam == TEAM_RED )
		szEffectName = m_CircleEffectRed;
	else if( eTeam == TEAM_BLUE )
		szEffectName = m_CircleEffectBlue;
	else
	{
		if( pOwner->GetSingleConvertTeam() == TEAM_BLUE )
			szEffectName = m_CircleEffectBlue;
		else
			szEffectName = m_CircleEffectRed;
	}

	if( !szEffectName.IsEmpty() )
		pStage->CreateMapEffect( szEffectName, vCreatePos, D3DXVECTOR3(1.0f, FLOAT1, FLOAT1) );

	// CheckPos : 실제 Weapon 생성 위치
	float fGap = pOwner->GetMidHeightByRate() - pOwner->GetBottomHeight();
	vCreatePos.y += fGap;
	
	pWeapon->SetCreateItem( szName );

	if( pWeapon->GetType() != ioWeapon::WT_CONTACT && pWeapon->GetType() != ioWeapon::WT_CONTACT2 )
		pWeapon->SetStartPosition( vCreatePos, pStage );

	ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
	if( pZone )
	{
		ioBaseChar *pChar = pZone->GetOwner();
		if( pChar )
		{
			D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			pZone->SetCollisionDir( vDir );
			pZone->SetZoneValue( true, false );
		}
	}
}

void ioRunAssaultDash::ProcessDash( ioBaseChar *pOwner )
{
	switch( m_RunDashState )
	{
	case RDS_CHARGING:
	case RDS_DRUID_TO_BEAR:
		ProcessChargingState( pOwner );
		break;
	case RDS_ASSAULT:
		ProcessAssaultState( pOwner );
		break;
	case RDS_RUN:
		ProcessRunState( pOwner );
		break;
	case RDS_EXTEND_CHARGING:
		ProcessExtendChargingState( pOwner );
		break;
	}
}

void ioRunAssaultDash::ProcessChargingState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if( !pOwner->IsNeedProcess() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( pOwner->IsSettedDirection() )
	{
		if( m_dwRunDashChargingStart + m_dwRunDashChargingTime < dwCurTime )
		{
			ChangeToRun( pOwner );
		}
	}
	else
	{
		switch( m_AssaultType )
		{
		case AT_DRUID:
			ChangeToDruidToHuman( pOwner );
			break;
		default:
			ChangeToAssault( pOwner );
			break;
		}
	}
}

void ioRunAssaultDash::ProcessAssaultState( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwEndTime = m_dwActionEndTime + pOwner->GetActionStopTotalDelay();

	if( dwEndTime > dwCurTime )
	{
		pOwner->CheckExtraAniJump();

		if( m_dwReserveEnableTime > 0 && m_dwReserveEnableTime + pOwner->GetActionStopTotalDelay() < dwCurTime )
		{
			// 예약체크
			CheckKeyReserve( pOwner );
		}
	}
	else
	{
		ProcessKeyReserve( pOwner );
	}
}

void ioRunAssaultDash::ProcessRunState( ioBaseChar *pOwner )
{
}

void ioRunAssaultDash::ProcessExtendChargingState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if( !pOwner->IsNeedProcess() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( pOwner->IsAttackKeyDown() && m_DashExtedState != DES_NONE )
	{
		if( m_dwExtendDashChargingStart + m_dwExtendDashChargingTime < dwCurTime )
		{
			switch( m_DashExtedState )
			{
			case DES_DRUID:
				{
					ChangeToDruidBearCrouch( pOwner );
				}
				break;
			}
		}
	}
	else
	{
		pOwner->SetDashAttack();
	}
}

void ioRunAssaultDash::ChangeToDruidToHuman( ioBaseChar *pOwner )
{
	ioDruidObjectItem* pDruid = ToDruidObjectItem( pOwner->GetPriorityItem( SPT_DASH ) );
	if( pDruid )
	{
		pOwner->ClearDirDoubleClick();
		pDruid->ChangeToHuman( pOwner );
		m_RunDashState = RDS_DRUID_TO_HUMAN;

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_EXTEND_DASH );
			kPacket << pOwner->GetCharName();
			kPacket << (int)GetType();
			kPacket << pOwner->GetHP().m_fCurValue;
			kPacket << m_RunDashState;
			kPacket << pOwner->GetTargetRot();
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioRunAssaultDash::ChangeToDruidBearCrouch( ioBaseChar *pOwner )
{
	ioDruidObjectItem* pDruid = ToDruidObjectItem( pOwner->GetPriorityItem( SPT_DASH ) );
	if( pDruid )
	{
		pDruid->ChangeToCrouchGathering( pOwner );
		m_RunDashState = RDS_DRUID_TO_BEAR;
	}
}

void ioRunAssaultDash::ApplyDash( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner )
		return;

	int iState;
	rkPacket >> iState;

	D3DXQUATERNION qtRot;
	rkPacket >> qtRot;

	pOwner->SetTargetRotToRotate( qtRot, true, false );

	switch( iState )
	{
	case RDS_ASSAULT:
		ChangeToAssault( pOwner );
		break;
	case RDS_RUN:
		ChangeToRun( pOwner );
		break;
	case RDS_DRUID_TO_HUMAN:
		ChangeToDruidToHuman( pOwner );
		break;
	}
}
