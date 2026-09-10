

#include "stdafx.h"

#include "ioPowerChargeDash.h"
#include "ioTitanItem.h"
#include "ioUroborusWeapon.h"
#include "ioGeomjeItem.h"
#include "ioMirWarrior.h"
#include "ioKungfuMasterItem.h"
#include "ioMagicStaffItem.h"
#include "ioChargeComboItem.h"
#include "ioChargeContactWeaponItem.h"
#include "ioPowerChargeItem3.h"
#include "ioSkyWalkerItem.h"

ioPowerChargeDash::ioPowerChargeDash()
{
	m_DashState = CDS_NONE;
	m_dwChargingStartTime = 0;
	m_dwChargingCancelChargeAttackStartTime = 0;
	m_stAniJump.Clear();
	m_dwActionEndTime = 0;
	m_dwEnableChargingDashAttackTime = 0;
	m_dwEnableDashAttackTime = 0;

	m_eSpecialDashCurrDirkey = ioUserKeyInput::DKI_NONE;
}

ioPowerChargeDash::ioPowerChargeDash( const ioPowerChargeDash &rhs )
: ioExtendDash( rhs ),
 m_AttackAttribute( rhs.m_AttackAttribute ),
 m_ExtendAttackAttribute( rhs.m_ExtendAttackAttribute ),
 m_dwChargingTime( rhs.m_dwChargingTime ),
 m_szChargingAni( rhs.m_szChargingAni ),
 m_fChargingSpeedRate( rhs.m_fChargingSpeedRate ),
 m_DashExtendType( rhs.m_DashExtendType ),
 m_DashTitanExtend( rhs.m_DashTitanExtend ),
 m_bExtraOtherDashAttack( rhs.m_bExtraOtherDashAttack ),
 m_dwCancelChargingTime( rhs.m_dwCancelChargingTime )
{
	m_DashState = CDS_NONE;
	m_dwChargingStartTime = 0;
	m_dwChargingCancelChargeAttackStartTime = 0;
	m_dwEnableChargingDashAttackTime = 0;
	m_dwEnableDashAttackTime = 0;

	m_dwFireStartTime = 0;
	m_dwCreateWeaponIndex = 0;
	m_dwExtendMoveSlidingTime = 0;

	m_dwEnableNinjaDashTime = 0;

	m_stAniJump.Clear();
	m_bDashCancelEnable = false;

	m_dwActionEndTime = 0;

	m_eSpecialDashCurrDirkey = ioUserKeyInput::DKI_NONE;
}

ioExtendDash* ioPowerChargeDash::Clone()
{
	return new ioPowerChargeDash( *this );
}

ioPowerChargeDash::~ioPowerChargeDash()
{
}

DashType ioPowerChargeDash::GetType() const
{
	return DT_POWER_CHARGE;
}

void ioPowerChargeDash::LoadProperty( ioINILoader &rkLoader )
{
	ioExtendDash::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	m_dwChargingTime = rkLoader.LoadInt_e( "dash_charge_time", 0 );
	m_fChargingSpeedRate = rkLoader.LoadFloat_e( "dash_charge_speed_rate", 0.0f );

	rkLoader.LoadString_e( "charging_dash_animation", "", szBuf, MAX_PATH );
	m_szChargingAni = szBuf;

	m_AttackAttribute.Init();
	LoadAttackAttribute_e( "dash_attack", m_AttackAttribute, rkLoader );

	m_ExtendAttackAttribute.Init();
	LoadAttackAttribute_e( "extend_dash_attack", m_ExtendAttackAttribute, rkLoader );

	m_bExtraOtherDashAttack = rkLoader.LoadBool( "enable_extra_other_dash_attack", false );

	//타이탄때 추가
	m_DashExtendType = (DashExtendType)rkLoader.LoadInt( "dash_extend_type", DET_NONE );

	// 캔슬 차징
	m_dwCancelChargingTime = rkLoader.LoadInt_e( "dash_cancel_charge_time", 0 );

	//타이탄
	if( m_DashExtendType == DET_TITAN )
		LoadTitanExtendInfo( rkLoader );
}

void ioPowerChargeDash::LoadTitanExtendInfo( ioINILoader &rkLoader )
{
	LoadAttackAttribute_e( "extend_dash_titan_move", m_DashTitanExtend.attribute , rkLoader );

	m_DashTitanExtend.m_fMoveAngle = rkLoader.LoadFloat_e( "dash_titan_extend_move_fire_angle", 0.f );	
	m_DashTitanExtend.m_fMoveSpeed = rkLoader.LoadFloat_e( "extend_dash_titan_move_speed", FLOAT1 );
	m_DashTitanExtend.m_fMoveRate = rkLoader.LoadFloat_e( "extend_dash_titan_move_rate", FLOAT1 );
}

void ioPowerChargeDash::SetPowerDash( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	DWORD dwDelayTime = 0;
	DWORD dwCurTime = FRAMEGETTIME();

	m_DashState = CDS_CHARGING;
	m_dwChargingStartTime = dwCurTime;
	m_dwReserveEnableTime = 0;
	m_bDashCancelEnable = false;

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

bool ioPowerChargeDash::CheckPowerDash( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_DashState )
	{
	case CDS_CHARGING:
		ProcessCharging( pOwner );
		break;
	case CDS_TITAN:
		ProcessTitanExtendAttack( pOwner );
		break;
	case CDS_TITAN_MOVE_ANI:
		ProcessTitanExtendMoveAni( pOwner );
		break;
	}
	if( m_DashState == CDS_NORMAL_ATTACK || m_DashState == CDS_EXTEND_ATTACK )
	{
		CheckAniJump_Input_Jump_S( pOwner );
		return true;
	}

	return false;
}

void ioPowerChargeDash::ProcessExtraDash( ioBaseChar *pOwner )
{
	switch( m_DashExtendType )
	{
	case DET_GEOMJE:
		{
			ioGeomjeItem* pGeomJe = ToGeomjeItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
			if( pGeomJe )
			{
				pGeomJe->CheckPowerDash( pOwner );
			}
		}
		break;
	case DET_KUNGFU:
		if( m_bDashCancelEnable )
			CheckCancel( pOwner );
		break;
	case DET_MAGICSTAFF:
		if( pOwner->IsNeedProcess() )
		{
			ioMagicStaffItem *pStaff = ToMagicStaffItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
			if( pStaff && pStaff->CheckInputAttack() )
			{
				if( pOwner->IsJumpKey() )
				{
					pStaff->SetInputAttackState( pOwner, true, ioMagicStaffItem::IAS_DASH_A );
					return;
				}
				else if( pOwner->IsDefenseKey() )
				{
					pStaff->SetInputAttackState( pOwner, true, ioMagicStaffItem::IAS_DASH_S );
					return;
				}
			}
		}
		break;
	case DET_WILDBOXER:
		CheckCancelBySpeicalDash( pOwner );
		break;
	case DET_SALADIN:
		if( pOwner->IsNeedProcess() )
		{
			ioChargeContactWeaponItem *pChargeContact = ToChargeContactWeaponItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
			if( pChargeContact && pChargeContact->CheckInputAttack() )
			{
				if( pOwner->IsDefenseKey() )
				{
					pChargeContact->SetInputAttackState( pOwner, true );
					return;
				}
			}
		}
		break;
	case DET_SKY_WALKER:
		{
			DWORD dwCurTime = FRAMEGETTIME();
			ioSkyWalkerItem *pSkyWalkerItem = ToSkyWalkerItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
			bool bCharging = false;
			if( pSkyWalkerItem )
			{
				if( pOwner->IsAttackKeyDown() )
				{
					// 액션 시간 지나면 나가기
					if( m_dwActionEndTime < dwCurTime )
					{
						m_dwEnableDashAttackTime = 0;
						m_dwEnableChargingDashAttackTime = 0;
						m_dwChargingCancelChargeAttackStartTime = 0;
						return;
					}

					if( m_dwEnableChargingDashAttackTime > 0 && dwCurTime > m_dwEnableChargingDashAttackTime && m_dwActionEndTime > dwCurTime )
					{
						bCharging = true;
						if( m_dwChargingCancelChargeAttackStartTime == 0 )
						{
							m_dwChargingCancelChargeAttackStartTime = FRAMEGETTIME();
						}
						DWORD dwChargingAttEnableTime = m_dwChargingCancelChargeAttackStartTime+m_dwCancelChargingTime;
						if( m_dwChargingCancelChargeAttackStartTime > 0 && m_dwChargingCancelChargeAttackStartTime+m_dwCancelChargingTime < dwCurTime )
						{
							CheckCancelReserveByConditionalSkyWalkerSpecialDash( pOwner );
							ChangeToExtendAttack( pOwner, true );
							m_dwEnableDashAttackTime = 0;
							m_dwEnableChargingDashAttackTime = 0;
							m_dwChargingCancelChargeAttackStartTime = 0;
							PowerDashEnd( pOwner );
							return;
						}
						// 액션 시간 지나면 나가기
						if( m_dwActionEndTime < dwCurTime )
						{
							m_dwEnableDashAttackTime = 0;
							m_dwEnableChargingDashAttackTime = 0;
							m_dwChargingCancelChargeAttackStartTime = 0;
							return;
						}
					}
					else
					{
						if( m_dwEnableDashAttackTime > 0 && dwCurTime > m_dwEnableDashAttackTime && m_dwActionEndTime < dwCurTime )
						{
							m_dwEnableDashAttackTime = 0;
							m_dwEnableChargingDashAttackTime = 0;
							m_dwChargingCancelChargeAttackStartTime = 0;
						}
					}
				}
				else
				{
					m_dwChargingCancelChargeAttackStartTime = 0;

					if( m_dwActionEndTime < dwCurTime )
					{
						m_dwEnableDashAttackTime = 0;
						m_dwEnableChargingDashAttackTime = 0;
						m_dwChargingCancelChargeAttackStartTime = 0;
						return;
					}
				}
			}
		}
		break;
	}
}

void ioPowerChargeDash::ProcessExtraDashByNoNeedProcess( ioBaseChar *pOwner )
{
	switch( m_DashExtendType )
	{
	case DET_GEOMJE:
		{
			ioGeomjeItem* pGeomJe = ToGeomjeItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
			if( pGeomJe )
			{
				pGeomJe->CheckPowerDash( pOwner );
			}
		}
		break;
	}
}

void ioPowerChargeDash::ProcessCharging( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	m_dwEnableNinjaDashTime = 0;

	DWORD dwCurTime = FRAMEGETTIME();
	if( pOwner->IsAttackKeyDown() )
	{
		if( m_dwChargingStartTime+m_dwChargingTime < dwCurTime )
		{
			switch( m_DashExtendType )
			{
			case DET_TITAN:
				if( CheckIsEnableTitanExtendAttack( pOwner ) )
					SetTitanExtendAttack( pOwner );
				else
					ChangeToNormalAttack( pOwner );
				break;
			case DET_MIR_WARRIOR:
				if( CheckIsEnableMirExtendAttack( pOwner ) )
					SetMirExtendAttack( pOwner );
				else
					ChangeToNormalAttack( pOwner );
				break;
			case DET_WILDBOXER:
				if( CheckIsEnableWildBoxerExtendAttack( pOwner ) )
					SetWildBoxerExtendAttack( pOwner );
				else
					ChangeToNormalAttack( pOwner );
				break;
			case DET_HAWKEYE_EXPANSION:
				SetHawkEyeExtendAttack( pOwner );
				break;
			case DET_SMILE_JOKER:
				if( CheckIsEnableSmileJokerAttack( pOwner ) )
					SetSmileJokerAttack( pOwner );
				else
					ChangeToExtendAttack( pOwner, false );
				break;
			case DET_SKY_WALKER:
				{
					CheckCancelReserveByConditionalSkyWalkerSpecialDash( pOwner );
					ChangeToExtendAttack( pOwner, false );
				}
				break;
			default: 
				{
					ChangeToExtendAttack( pOwner, false );
					return;
				}
			}			
		}
	}
	else
	{
		if( m_DashExtendType == DET_SMILE_JOKER && CheckIsEnableSmileJokerAttack( pOwner ) )
			SetSmileJokerAttack( pOwner );
		else
			ChangeToNormalAttack( pOwner );

		return;
	}
}

bool ioPowerChargeDash::CheckIsEnableTitanExtendAttack( ioBaseChar *pOwner )
{
	ioItem *pItem = pOwner->GetEquipedItem( ES_WEAPON );
	ioTitanItem *pTitanItem = ToTitanWeaponItem( pItem );
	if( pTitanItem && !pOwner->IsHasCrown() )
		return true;

	return false;
}

void ioPowerChargeDash::SetTitanExtendAttack( ioBaseChar *pOwner )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	ioItem *pItem = pOwner->GetEquipedItem( ES_WEAPON );
	ioTitanItem *pTitanItem = ToTitanWeaponItem( pItem );
	if( !pTitanItem )
		return;

	const AttackAttribute& pAttr = m_ExtendAttackAttribute;
	DWORD dwCurTime = FRAMEGETTIME();
	m_dwFireStartTime = 0;
	m_dwCreateWeaponIndex = 0;

	int iAniID = pGrp->GetAnimationIdx( pAttr.m_AttackAnimation );
	float fTimeRate = pAttr.m_fAttackAniRate;
	DWORD dwPreDelay = pAttr.m_dwPreDelay;

	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pOwner->SetAutoTarget( ATT_JUMP );

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );
	pOwner->ClearReservedSliding();
	pOwner->SetCurMoveSpeed( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );

	float fFire = pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;
	m_dwFireStartTime = dwPreDelay + dwCurTime + fFire;

	m_DashState = CDS_TITAN;

	DWORD dwTrackingTime = dwCurTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;

	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_DASH );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << m_DashState;
		kPacket << pOwner->GetTargetRot();
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioPowerChargeDash::CheckIsEnableWildBoxerExtendAttack( ioBaseChar *pOwner )
{
	ioItem *pItem = pOwner->GetEquipedItem( ES_WEAPON );
	ioChargeComboItem *pChargeComboItem = ToChargeComboItem( pItem );
	if( pChargeComboItem && !pOwner->IsHasCrown() )
		return true;

	return false;
}

void ioPowerChargeDash::SetWildBoxerExtendAttack( ioBaseChar *pOwner )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	ioItem *pItem = pOwner->GetEquipedItem( ES_WEAPON );
	ioChargeComboItem *pChargeComboItem = ToChargeComboItem( pItem );
	if( !pChargeComboItem )
		return;

	pChargeComboItem->SetWildBoxerSpecialState( pOwner );
}

void ioPowerChargeDash::SetHawkEyeExtendAttack( ioBaseChar *pOwner )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	ioItem *pItem = pOwner->GetEquipedItem( ES_WEAPON );
	ioChargeContactWeaponItem *pChargeContact = ToChargeContactWeaponItem( pItem );
	if( !pChargeContact )
		return;

	pChargeContact->SetHawkEyeSpecialState( pOwner );
}

void ioPowerChargeDash::ChangeToNormalAttack( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	m_dwEnableNinjaDashTime = 0;

	m_DashState = CDS_NORMAL_ATTACK;
	pOwner->SetDashAttackByAttribute( m_AttackAttribute );

	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = 0;
	if( pGrp )
	{
		m_dwReserveEnableTime = 0;
		m_stAniJump = m_AttackAttribute.m_AttackAnimation;
		m_fAniJumpRate = m_AttackAttribute.m_fAttackAniRate;
		m_dwActionStartTime = FRAMEGETTIME();
		m_bDashCancelEnable = true;

		iAniID = pGrp->GetAnimationIdx( m_stAniJump );
		CheckKeyReserveTime( pOwner, iAniID, m_fAniJumpRate );

		if( m_fAniJumpRate <= 0.0f )
			m_fAniJumpRate = FLOAT1;

		DWORD dwCurTime = FRAMEGETTIME();
		m_dwActionEndTime = FRAMEGETTIME() + ( pGrp->GetAnimationFullTime( iAniID ) * m_fAniJumpRate );
	}

	//진화 닌자
	switch( m_DashExtendType )
	{
	case DET_NINJA_EXPANSION:
		{
			ioEntityGroup *pGroup = pOwner->GetGroup();
			DWORD dwEnableDashTime = 0;
			if( pGroup )
			{
				int iAniID = pGroup->GetAnimationIdx( m_AttackAttribute.m_AttackAnimation );
				if( iAniID != -1 ) 
				{
					dwEnableDashTime = pGroup->GetAnimationEventTime_e( iAniID, "ninja_dash" ) * m_AttackAttribute.m_fAttackAniRate;
					if( dwEnableDashTime > 0)
						m_dwEnableNinjaDashTime = FRAMEGETTIME() + dwEnableDashTime;
				}
			}
		}
		break;
	case DET_GEOMJE:
		{
			ioGeomjeItem* pGeomJe = ToGeomjeItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
			if( pGeomJe )
			{
				pGeomJe->SetDashAttack( pOwner, 0 );
			}
		}
		break;
	case DET_MAGICSTAFF:
		{
			ioMagicStaffItem *pStaff = ToMagicStaffItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
			if( pStaff )
				pStaff->SetInputAttackInfo( pOwner, iAniID, m_fAniJumpRate );
		}
		break;
	case DET_SALADIN:
		{
			ioChargeContactWeaponItem *pChargeContact = ToChargeContactWeaponItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
			if( pChargeContact )
				pChargeContact->SetInputAttackInfo( pOwner, iAniID, m_fAniJumpRate );
		}
		break;
	case DET_SKY_WALKER:
		{
			ioEntityGroup *pGroup = pOwner->GetGroup();
			if( pGroup )
			{
				int iAniID = pGroup->GetAnimationIdx( m_AttackAttribute.m_AttackAnimation );
				if( iAniID != -1 ) 
				{
					m_dwEnableChargingDashAttackTime = FRAMEGETTIME() + pGroup->GetAnimationEventTime_e( iAniID, "input_charge_s" ) * m_AttackAttribute.m_fAttackAniRate;
				}
			}
		}
		break;
	}

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

void ioPowerChargeDash::ChangeToExtendAttack( ioBaseChar *pOwner, bool bChainExtendAttack )
{
	if( !pOwner ) return;

	m_dwEnableNinjaDashTime = 0;
	m_DashState = CDS_EXTEND_ATTACK;
	bool bSetAutoTarget = true;
	if( m_DashExtendType == DET_SKY_WALKER )
		bSetAutoTarget = false;

	pOwner->SetDashAttackByAttribute( m_ExtendAttackAttribute, bSetAutoTarget );

	m_stAniJump = m_ExtendAttackAttribute.m_AttackAnimation;
	m_fAniJumpRate = m_ExtendAttackAttribute.m_fAttackAniRate;
	m_dwActionStartTime = FRAMEGETTIME();
	m_bDashCancelEnable = true;

	ioEntityGroup *pGroup = pOwner->GetGroup();
	if( pGroup )
	{
		DWORD dwCurTime = FRAMEGETTIME();
		int iAniID	     = pGroup->GetAnimationIdx( m_stAniJump );
		m_dwActionEndTime = FRAMEGETTIME() + ( pGroup->GetAnimationFullTime( iAniID ) * m_fAniJumpRate );
	}

	switch( m_DashExtendType )
	{
	case DET_NINJA_EXPANSION: 	//진화 닌자
		{
			DWORD dwEnableDashTime = 0;
			if( pGroup )
			{
				int iAniID = pGroup->GetAnimationIdx( m_ExtendAttackAttribute.m_AttackAnimation );
				if( iAniID != -1 ) 
				{
					dwEnableDashTime = pGroup->GetAnimationEventTime_e( iAniID, "ninja_dash" ) * m_ExtendAttackAttribute.m_fAttackAniRate;
					if( dwEnableDashTime )
						m_dwEnableNinjaDashTime = FRAMEGETTIME() + dwEnableDashTime;
				}
			}
		}
		break;
	case DET_GEOMJE:
		{
			ioGeomjeItem* pGeomJe = ToGeomjeItem( pOwner->GetPriorityItem( SPT_DASH ) );
			if( pGeomJe )
			{
				pGeomJe->SetExtendDashAttack( pOwner, 0 );
			}
		}
		break;
	case DET_SALADIN:
		{
			ioChargeContactWeaponItem *pChargeContact = ToChargeContactWeaponItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
			if( pChargeContact )
				pChargeContact->ClearInputAttackInfo();
		}
		break;
	case DET_SKY_WALKER:
		{
			DWORD dwEnableDashTime = 0;
			if( pGroup )
			{
				int iAniID = pGroup->GetAnimationIdx( m_ExtendAttackAttribute.m_AttackAnimation );
				if( iAniID != -1 ) 
				{
					dwEnableDashTime = pGroup->GetAnimationEventTime_e( iAniID, "cancel_attack" );
					if( dwEnableDashTime )
					{
						ioSkyWalkerItem *pSkyWalkerItem = ToSkyWalkerItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
						if( pSkyWalkerItem )
						{
							pSkyWalkerItem->SetDashAttackTimeInfo( FRAMEGETTIME() + dwEnableDashTime, m_dwActionEndTime );
						}
					}
					if( m_eSpecialDashCurrDirkey != ioUserKeyInput::DKI_NONE )
						pOwner->SetTargetRotToDirKey( m_eSpecialDashCurrDirkey );
					m_eSpecialDashCurrDirkey = ioUserKeyInput::DKI_NONE;
				}
			}
		}
		break;
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_EXTEND_DASH );
		kPacket << pOwner->GetCharName();
		kPacket << (int)GetType();
		kPacket << pOwner->GetHP().m_fCurValue;
		kPacket << CDS_EXTEND_ATTACK;
		kPacket << bChainExtendAttack;
		kPacket << (int)m_eSpecialDashCurrDirkey;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioPowerChargeDash::SetTitanExtendMoveAni( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	int iAniID = pGrp->GetAnimationIdx( m_DashTitanExtend.attribute.m_AttackAnimation );
	if( iAniID == -1 )
		return;

	pOwner->RefreshFireTimeList( iAniID, m_DashTitanExtend.attribute.m_vWeaponInfoList, FTT_JUMP_ATTACK, m_DashTitanExtend.attribute.m_fAttackAniRate,
		m_DashTitanExtend.attribute.m_dwPreDelay);

	pOwner->IncreaseWeaponIndexBase();

	DWORD dwTime = (DWORD)pGrp->GetAnimationEventTime_e( iAniID, "fire_sliding" ) * m_DashTitanExtend.attribute.m_fAttackAniRate;

	m_dwExtendMoveSlidingTime = dwCurTime + dwTime;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	float fTimeRate = m_DashTitanExtend.attribute.m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pGrp->SetActionAni( m_DashTitanExtend.attribute.m_AttackAnimation, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	m_DashState = CDS_TITAN_MOVE_ANI;
}

void ioPowerChargeDash::ProcessTitanExtendMoveAni( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	ioUroborusWeapon *pUroborusWeapon = ToUroborusWeapon( g_WeaponMgr.FindWeapon( pOwner, m_dwCreateWeaponIndex ) );
	if( !pUroborusWeapon )
	{
		// End 처리.
		TitanExtendMoveFail( pOwner, false );
		return;
	}

	if( dwCurTime > m_dwExtendMoveSlidingTime )
	{
		ioEntityGroup *pGrp = pOwner->GetGroup();
		if( !pGrp )
			return;

		int iAniID = pGrp->GetAnimationIdx( m_DashTitanExtend.attribute.m_AttackAnimation );

		pOwner->CheckCharColSkipTime( iAniID, m_DashTitanExtend.attribute.m_fAttackAniRate, 0 );

		D3DXVECTOR3 vTargetPos = pUroborusWeapon->GetPosition();
		pOwner->SetTitanExtendMoveState( m_DashTitanExtend.attribute.m_AttackAnimation, m_DashTitanExtend.attribute.m_fAttackAniRate,
			vTargetPos, m_DashTitanExtend.m_fMoveSpeed, m_DashTitanExtend.m_fMoveRate, false );

		return;
	}
}

void ioPowerChargeDash::ProcessTitanExtendAttack(  ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	pOwner->SetCurMoveSpeed( 0.0f );

	const AttackAttribute& pAttr = m_ExtendAttackAttribute;
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwFireStartTime > 0 && m_dwFireStartTime < dwCurTime )
	{
		m_dwCreateWeaponIndex = pOwner->IncreaseWeaponIndexBase();

		const WeaponInfo &rkInfo = pAttr.m_vWeaponInfoList.front();
		D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate();
		D3DXVECTOR3 vDir = CheckTitanExtendDir( pOwner );

		FireTime kFireTime;
		kFireTime.dwStart = dwCurTime;

		kFireTime.iAttributeIdx = rkInfo.m_iWeaponIdx;
		kFireTime.iResistanceIdx = rkInfo.m_iResistanceIdx;
		kFireTime.szWoundedAni  = rkInfo.m_WoundedAnimation;
		kFireTime.dwWoundedAniDuration = rkInfo.m_dwWoundedDuration;
		kFireTime.bLoopWoundedAni = rkInfo.m_bWoundedLoopAni;

		kFireTime.dwWeaponIdx = m_dwCreateWeaponIndex;
		kFireTime.eFireTimeType = FTT_JUMP_ATTACK;

		pOwner->ExtendFireExplicit( kFireTime, vPos, vDir, "" );
		m_dwFireStartTime = 0;

		return;
	}

	if( m_dwCreateWeaponIndex == 0 )
		return;

	ioUroborusWeapon *pUroborusWeapon = ToUroborusWeapon( g_WeaponMgr.FindWeapon( pOwner, m_dwCreateWeaponIndex ) );
	if( !pUroborusWeapon )
	{
		// End 처리.
		TitanExtendMoveFail( pOwner, false );
		return;
	}

	if( pUroborusWeapon->IsCanUroborusJump() )
	{
		ioItem *pItem = pOwner->GetEquipedItem( ES_WEAPON );
		ioTitanItem *pTitanItem = ToTitanWeaponItem( pItem );
		if( !pTitanItem )
			return;

		if( pOwner->IsNeedProcess() && pTitanItem->IsEnableGauge() && pOwner->IsJumpKeyDown() )
		{
			pTitanItem->DecreaseBullet();

			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_EXTEND_DASH );
				kPacket << pOwner->GetCharName();
				kPacket << (int)GetType();
				kPacket << pOwner->GetHP().m_fCurValue;
				kPacket << CDS_TITAN_MOVE_RESERVED;
				kPacket << pOwner->GetTargetRot();
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
			SetTitanExtendMoveAni( pOwner );
			return;
		}
	}
	else if( !pUroborusWeapon->IsUroborusMove() && !pUroborusWeapon->IsUroborusWait() )
	{
		// End 처리
		if( pOwner->IsNeedProcess() )
		{
			TitanExtendMoveFail( pOwner, true );
			return;
		}
	}
}

bool ioPowerChargeDash::IsPowerChargingState()
{
	if( m_DashState == CDS_NONE )
	{
		return false;
	}

	return true;
}

void ioPowerChargeDash::PowerDashEnd( ioBaseChar *pOwner )
{
	m_DashState = CDS_NONE;
	m_dwChargingStartTime = 0;
	m_dwReserveEnableTime = 0;
	m_dwChargingCancelChargeAttackStartTime = 0;
	m_dwEnableDashAttackTime = 0;

	m_eSpecialDashCurrDirkey = ioUserKeyInput::DKI_NONE;
}

void ioPowerChargeDash::ApplyDash( ioBaseChar *pOwner, SP2Packet &rkPacket )
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
	case CDS_NORMAL_ATTACK:
		ChangeToNormalAttack( pOwner );
		break;
	case CDS_EXTEND_ATTACK:
		{
			bool bChainExtendAttack = false;
			int iSpecialDashCurrDirkey = 0;
			rkPacket >> bChainExtendAttack;
			rkPacket >> iSpecialDashCurrDirkey;
			m_eSpecialDashCurrDirkey = (ioUserKeyInput::DirKeyInput)iSpecialDashCurrDirkey;

			if( m_DashExtendType == DET_SKY_WALKER )
			{
				if( m_eSpecialDashCurrDirkey != ioUserKeyInput::DKI_NONE )
					pOwner->SetTargetRotToDirKey( m_eSpecialDashCurrDirkey );
			}

			ChangeToExtendAttack( pOwner, bChainExtendAttack );

			if( bChainExtendAttack )
				PowerDashEnd( pOwner );
		}
		break;
	case CDS_TITAN:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			pOwner->SetTargetRot( qtRot );
			SetTitanExtendAttack( pOwner );
		}
		break;
	case CDS_TITAN_MOVE_STATE:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			D3DXVECTOR3 vTargetPos ;
			rkPacket >> vTargetPos;

			pOwner->SetTitanExtendMoveState( m_DashTitanExtend.attribute.m_AttackAnimation, m_DashTitanExtend.attribute.m_fAttackAniRate,
				vTargetPos, m_DashTitanExtend.m_fMoveSpeed, m_DashTitanExtend.m_fMoveRate, false );

			ioEntityGroup *pGrp = pOwner->GetGroup();
			int iAniID = pGrp->GetAnimationIdx( m_DashTitanExtend.attribute.m_AttackAnimation );

			pOwner->RefreshFireTimeList( iAniID, m_DashTitanExtend.attribute.m_vWeaponInfoList, FTT_JUMP_ATTACK, m_DashTitanExtend.attribute.m_fAttackAniRate,
				m_DashTitanExtend.attribute.m_dwPreDelay);

			pOwner->IncreaseWeaponIndexBase();
		}
		break;
	case CDS_TITAN_MOVE_FAIL:
		TitanExtendMoveFail( pOwner, false );
		break;
	case CDS_TITAN_MOVE_RESERVED:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			pOwner->SetTargetRot( qtRot );
			SetTitanExtendMoveAni( pOwner );
		}
		break;
	}
}

bool ioPowerChargeDash::IsEnableExtraDashAttack()
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

bool ioPowerChargeDash::IsEnableExtraOtherDashAttack()
{
	if ( m_bExtraOtherDashAttack )
		return true;

	switch( m_DashState )
	{
	case CDS_NORMAL_ATTACK:
		return true;
	case CDS_EXTEND_ATTACK:
		return false;
	}

	return false;
}

void ioPowerChargeDash::TitanExtendMoveFail( ioBaseChar *pOwner, bool bSend )
{
	pOwner->SetState( CS_DELAY, false );
}

D3DXVECTOR3	ioPowerChargeDash::CheckTitanExtendDir( ioBaseChar *pOwner )
{
	D3DXVECTOR3 vDir = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	if( !pOwner )
		return vDir;

	D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate();

	D3DXVECTOR3 vSightDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vSightDir, &vSightDir );

	D3DXVECTOR3 vAxis;
	D3DXVec3Cross( &vAxis, &vSightDir, &ioMath::UNIT_Y );

	D3DXQUATERNION qtNewRot;
	float fCurAngle = ioMath::ArrangeHead( m_DashTitanExtend.m_fMoveAngle );
	D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fCurAngle) );

	vDir = qtNewRot * vSightDir;
	D3DXVec3Normalize( &vDir, &vDir );

	return vDir;
}

bool ioPowerChargeDash::IsTitanExtendMoveState()
{
	if( m_DashState == CDS_TITAN || m_DashState == CDS_TITAN_MOVE_STATE || m_DashState == CDS_TITAN_MOVE_ANI )
		return true;

	return false;
}

bool ioPowerChargeDash::CheckDbKey( ioBaseChar *pOwner )
{
	if( m_DashExtendType == DET_NINJA_EXPANSION )
	{
		if( pOwner->IsNeedProcess() && pOwner->IsDirKeyDoubleClick() )
		{
			ioUserKeyInput::DirKeyInput eDir = pOwner->CheckCurDirKey();
			if(	eDir != ioUserKeyInput::DKI_NONE && pOwner->IsNeedProcess() )
			{
				float fYawD = 180.0f + 45.0f * ( eDir - ioUserKeyInput::DKI_UP );
				fYawD = ioMath::ArrangeHead( fYawD );

				float fYawR = DEGtoRAD( fYawD );
				D3DXQUATERNION qtRotate;
				D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

				pOwner->SetTargetRotToRotate( qtRotate, true );
			}

			pOwner->SetNinjaExpansionMove( false );
			return true;
		}
		return false;
	}

	return false;
}

bool ioPowerChargeDash::IsEnableDashFromDashAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_DashExtendType == DET_NINJA_EXPANSION && pOwner->IsNeedProcess() )
	{
		if( m_dwEnableNinjaDashTime && m_dwEnableNinjaDashTime < dwCurTime )
			return true;
	}
	else if( m_DashExtendType == DET_WILDBOXER && pOwner->IsNeedProcess() )
		return true;

	return false;
}

void ioPowerChargeDash::SetExtraDashAttackInfo(	ioBaseChar *pOwner, int nAni, float fTimeRate, DWORD dwPreDelay, int iCurCombo )
{
	switch( m_DashExtendType )
	{
	case DET_NINJA_EXPANSION:
		{
			m_dwEnableNinjaDashTime = 0;
			if( pOwner && nAni != -1 )
			{
				ioEntityGroup *pGroup = pOwner->GetGroup();
				DWORD dwEnableDashTime = 0;
				dwEnableDashTime = pGroup->GetAnimationEventTime_e( nAni, "ninja_dash" ) * fTimeRate;
				if( dwEnableDashTime )
					m_dwEnableNinjaDashTime = FRAMEGETTIME() + dwEnableDashTime + dwPreDelay;
			}
		}
		break;
	case DET_GEOMJE:
		{
			ioGeomjeItem* pGeomJe = ToGeomjeItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
			if( pGeomJe )
			{
				pGeomJe->SetExtraDashAttack( pOwner, iCurCombo );
			}
		}
		break;
	}
}

bool ioPowerChargeDash::CheckIsEnableMirExtendAttack( ioBaseChar *pOwner )
{	
	ioMirWarrior* pMir = ToMirWarrior( pOwner->GetEquipedItem( ES_WEAPON ) );
	if( pMir && !pOwner->IsHasCrown() && pMir->GetNeedBullet() <= pMir->GetCurBullet() )
		return true;

	return false;
}

void ioPowerChargeDash::SetMirExtendAttack( ioBaseChar *pOwner )
{
	ioMirWarrior* pMir = ToMirWarrior( pOwner->GetEquipedItem( ES_WEAPON ) );
	if( pMir )
	{
		pMir->SetChangeConditionalSpecialDash( pOwner, pOwner->CheckCurDirKey(), ioAttackableItem::CCSD_DASH_ATTACK );
	}
	else
	{
		ChangeToNormalAttack( pOwner );
	}
}

bool ioPowerChargeDash::CheckAniJump_Input_Jump_S( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return false;

	if( m_stAniJump.IsEmpty() )
		return false;

	int iAniID = pGrp->GetAnimationIdx( m_stAniJump );

	std::list<float> vTimeList;
	pGrp->GetAnimationEventTimeList_e( iAniID, "input_jump_s", vTimeList );

	if( vTimeList.empty() )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	std::list< float >::iterator iter = vTimeList.begin();
	while( iter != vTimeList.end() )
	{
		DWORD dwTime = (DWORD)*iter;
		dwTime *= m_fAniJumpRate;

		dwTime += m_dwActionStartTime;

		if( m_dwReserveEnableTime > 0 && COMPARE( dwTime, m_dwReserveEnableTime, dwCurTime ) )
		{
			if( m_bEnableJumpState && !pOwner->IsBuffLimitJump() )
			{
				pOwner->SetJumpState();
				return true;
			}

			return false;
		}

		if( dwTime > dwCurTime )
			return false;

		iter++;
	}

	return false;
}

bool ioPowerChargeDash::CheckCancelBySpeicalDash( ioBaseChar *pOwner )
{
	if( m_stAniJump.IsEmpty() )
		return false;

	if( m_dwActionStartTime == 0 )
		return false;

	if( !pOwner )
		return false;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return false;

	ioItem *pItem = pOwner->GetEquipedItem( ES_WEAPON );
	ioChargeComboItem *pChargeComboItem = ToChargeComboItem( pItem );
	if( !pChargeComboItem )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	int iAniID = pGrp->GetAnimationIdx( m_stAniJump );			
	DWORD dwInputSpecialDashTime = (DWORD)pGrp->GetAnimationEventTime_e( iAniID, "input_dash_s" );
	if( 0 < dwInputSpecialDashTime )
	{
		dwInputSpecialDashTime *= m_fAniJumpRate;
		dwInputSpecialDashTime += m_dwActionStartTime;		
	}

	if( 0 < dwInputSpecialDashTime && dwCurTime > dwInputSpecialDashTime )
	{
		if( pOwner->IsDirKeyDoubleClick() && pChargeComboItem->IsEnableGauge() )
		{
			m_stAniJump.Clear();
			m_dwActionStartTime = 0;
			pChargeComboItem->SetChangeConditionalSpecialDash( pOwner, pOwner->GetDirKey(), ioAttackableItem::CCSD_NORMAL );
			return true;
		}
	}

	return false;
}

bool ioPowerChargeDash::CheckCancel( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	ioKungfuMasterItem* pKungfu = ToKungfuMasterItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( !pKungfu || !pKungfu->EnableGauge() )
		return false;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return false;

	if( m_stAniJump.IsEmpty() )
		return false;

	int iAniID = pGrp->GetAnimationIdx( m_stAniJump );

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwMacroTime_S = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * m_fAniJumpRate;
	if( dwMacroTime_S == 0 )
		return false;

	if( m_dwActionStartTime + dwMacroTime_S < dwCurTime )
	{
		if( pOwner->IsDefenseKey() )
		{
			pKungfu->SetSleepStart( pOwner, true );
			m_bDashCancelEnable = false;
			return true;
		}
	}
	return false;
}

void ioPowerChargeDash::CheckCancelReserveByConditionalSkyWalkerSpecialDash( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if( m_eSpecialDashCurrDirkey == ioUserKeyInput::DKI_NONE )
	{
		m_bEnableJumpState = false;
		m_bEnableAttackState = false;
		m_bEnableDefenseState = false;		
		m_eSpecialDashCurrDirkey = pOwner->CheckCurDirKey();
		m_iSkillInput = -1;
		pOwner->ClearDirDoubleClick();
	}
}

bool ioPowerChargeDash::CheckIsEnableSmileJokerAttack( ioBaseChar *pOwner )
{
	ioItem *pItem = pOwner->GetEquipedItem( ES_WEAPON );
	ioPowerChargeItem3 *pPowerItem3 = ToPowerChargeItem3(pItem);
	if( pPowerItem3 && pPowerItem3->IsPoseChanged() )
		return true;

	return false;
}

void ioPowerChargeDash::SetSmileJokerAttack( ioBaseChar *pOwner )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	ioItem *pItem = pOwner->GetEquipedItem( ES_WEAPON );
	ioPowerChargeItem3 *pPowerItem3 = ToPowerChargeItem3(pItem);
	if( !pPowerItem3 )
		return;

	pPowerItem3->ChangeToPoseChangeAttack( pOwner );
}