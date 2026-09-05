

#include "stdafx.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "ioMagicStaffItem2.h"

#include "ioExtendJump.h"



#include "WeaponDefine.h"
#include "ItemDefine.h"

ioMagicStaffItem2::ioMagicStaffItem2()
{
	m_dwTimeGap = 0;
	m_dwChargeStartTime = 0;
	m_iCurCombo = 0;
	m_FireState = FS_NONE;
	m_dwBaseWeaponIndex = 0;

	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;
	m_vAirJumpTimeList.clear();
}

ioMagicStaffItem2::ioMagicStaffItem2( const ioMagicStaffItem2 &rhs )
: ioWeaponItem( rhs ),
 m_dwEnableChargeTime( rhs.m_dwEnableChargeTime ),
 m_PreGatherAnimation( rhs.m_PreGatherAnimation ),
 m_GatherAnimation( rhs.m_GatherAnimation ),
 m_GatherEffect( rhs.m_GatherEffect ),
 m_fMoveSpeed( rhs.m_fMoveSpeed ),
 m_fRange( rhs.m_fRange ),
 m_fHeightGap( rhs.m_fHeightGap ),
 m_ReadyCircle( rhs.m_ReadyCircle ),
 m_EnableCircle( rhs.m_EnableCircle ),
 m_fMagicCircleRadius( rhs.m_fMagicCircleRadius ),
 m_fSkillGaugeRate( rhs.m_fSkillGaugeRate ),
 m_GatheringAttribute( rhs.m_GatheringAttribute ),
 m_AreaWeaponName( rhs.m_AreaWeaponName ),
 m_ReadyEffect( rhs.m_ReadyEffect ),
 m_EnableChrageCircle( rhs.m_EnableChrageCircle )
{
	m_dwTimeGap = 0;
	m_dwChargeStartTime = 0;
	m_iCurCombo = 0;
	m_FireState = FS_NONE;
	m_dwBaseWeaponIndex = 0;
	m_dwReadyEffectID	= -1;
	m_dwEnableChrageCircleID	= -1;

	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;
	m_vAirJumpTimeList.clear();
}

ioMagicStaffItem2::~ioMagicStaffItem2()
{
	m_vAirJumpTimeList.clear();
}

void ioMagicStaffItem2::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	m_dwEnableChargeTime = (DWORD)rkLoader.LoadInt_e( "enable_charge_time", 0 );

	rkLoader.LoadString_e( "pre_gather_animation", "", szBuf, MAX_PATH );
	m_PreGatherAnimation = szBuf;
	rkLoader.LoadString_e( "gather_animation", "", szBuf, MAX_PATH );
	m_GatherAnimation = szBuf;
	rkLoader.LoadString_e( "gather_effect", "", szBuf, MAX_PATH );
	m_GatherEffect = szBuf;
	rkLoader.LoadString_e( "gather_ready_effect", "", szBuf, MAX_PATH );
	m_ReadyEffect = szBuf;	

	rkLoader.LoadString_e( "ready_circle", "", szBuf, MAX_PATH );
	m_ReadyCircle = szBuf;
	rkLoader.LoadString_e( "enable_circle", "", szBuf, MAX_PATH );
	m_EnableCircle = szBuf;
	rkLoader.LoadString_e( "enable_charge_circle", "", szBuf, MAX_PATH );
	m_EnableChrageCircle = szBuf;

	m_fMagicCircleRadius = rkLoader.LoadFloat_e( "magic_circle_radius", FLOAT100 );
	m_fRange = rkLoader.LoadFloat_e( "magic_circle_range", FLOAT1000 );
	m_fMoveSpeed = rkLoader.LoadFloat_e( "magic_circle_move_speed", 350.0f );
	m_fHeightGap = rkLoader.LoadFloat_e( "magic_circle_height_gap", 20.0f );
	
	m_fSkillGaugeRate = rkLoader.LoadFloat_e( "skill_gauge_rate", FLOAT1 );

	wsprintf_e( szBuf, "gathering_attack" );
	LoadAttackAttribute( szBuf, m_GatheringAttribute, rkLoader );

	rkLoader.LoadString_e( "area_weapon_name", "", szBuf, MAX_PATH );
	m_AreaWeaponName = szBuf;
}

ioItem* ioMagicStaffItem2::Clone()
{
	return new ioMagicStaffItem2( *this );
}

void ioMagicStaffItem2::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	m_FireState = FS_NONE;

	if( iCurCombo == 0 )
	{
		m_FireState = FS_CHARGING;
		m_dwChargeStartTime = FRAMEGETTIME();
		m_iCurCombo = iCurCombo;

		pOwner->SetState( CS_ATTACK );
		pOwner->SetChargingState( true );

		ioEntityGroup *pGrp = pOwner->GetGroup();
		pGrp->ClearAllActionAni( FLOAT100, true );
		pGrp->ClearAllLoopAni( FLOAT100, true );
		
		int iAniID = pGrp->GetAnimationIdx( m_PreGatherAnimation );
		float fTimeRate = (float)m_dwChargeTime / pGrp->GetAnimationFullTime( iAniID );
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );
	}
	else
	{
		ioAttackableItem::SetNormalAttack( pOwner, iCurCombo );
		m_FireState = FS_NONE;
	}
}

void ioMagicStaffItem2::ChangeToGathering( ioBaseChar *pOwner )
{
	pOwner->StopAutoTargetTracking();

	ioEntityGroup *pGrp = pOwner->GetGroup();
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->SetLoopAni( m_GatherAnimation, FLOAT100 );
	pOwner->AttachEffect( m_GatherEffect );

	ioHashString szDummyCircle;
	pOwner->CreateMagicCircle( m_ReadyCircle, m_EnableCircle, szDummyCircle, m_fMagicCircleOffSet );

	m_FireState = FS_GATHERING;
	m_dwGatheringStartTime = FRAMEGETTIME();
	
	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_FireState;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioMagicStaffItem2::CheckNormalAttack( ioBaseChar *pOwner )
{
	switch( m_FireState )
	{
	case FS_CHARGING:
		OnCharging( pOwner );
		break;
	case FS_NORMAL_ATTACK:
		ioAttackableItem::CheckNormalAttack( pOwner );
		break;
	case FS_GATHERING:
		OnGathering( pOwner );
		break;
	case FS_GATHERING_ATTACK:
		OnGatheringAttack( pOwner );
		break;
	}
}

void ioMagicStaffItem2::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )
		return;

	//if( m_dwChargeStartTime + m_dwEnableChargeTime < FRAMEGETTIME() )
		//return;

	if( pOwner->IsAttackKeyDown() )
	{
		if( m_dwChargeStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			pOwner->EndEffect( m_GatherEffect );
			ChangeToGathering( pOwner );
		}
	}
	else
	{
		pOwner->SetChargingState( false );
		ChangeToNormalAttack( pOwner );
	}
}

void ioMagicStaffItem2::OnGathering( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if( !pOwner->IsAttackKeyDown() )	// 버튼 뗌
	{
		if( m_dwChargeStartTime + m_dwEnableChargeTime <= FRAMEGETTIME() )
			ChangeToGatheringAttack( pOwner );
		else
			SetCancelFire( pOwner );
			//SetNormalAttack( pOwner, m_iCurCombo );
		
		return;
	}

	float fCurRange = m_fRange;
	if( pOwner->IsDefenseKeyDown() )
	{
		if( pOwner->GetUseEnableSkill( 0, SUT_NORMAL ) )
		{
			pOwner->UpdateMagicCirclePos( m_fMoveSpeed, fCurRange, m_fHeightGap, true, true );
		}

		int iSkillNum = pOwner->GetSkillKeyInput();
		if( iSkillNum == 0 && !pOwner->IsBuffLimitSkill() && pOwner->SetUseSkill(0, SUT_NORMAL) )
		{
			m_FireState = FS_NONE;
			pOwner->EndEffect( m_GatherEffect );
			return;
		}
	}

	//Gather Gauge
	if( pOwner->CheckMagicCircle() )
	{
		// 뱡향 변화 체크
		pOwner->CheckChangeDirectionForMagicCircle();

		// MagicCircle 위치 갱신
		if( pOwner->GetUseEnableSkill( 0, SUT_NORMAL ) )
			pOwner->UpdateMagicCirclePosAll( m_fMoveSpeed, fCurRange, m_fHeightGap, true );
		else
			pOwner->UpdateMagicCirclePosAll( m_fMoveSpeed, fCurRange, m_fHeightGap, false );

		// MagicCircle 위치방향으로 캐릭터 회전
		D3DXVECTOR3 vTargetPos = pOwner->GetMagicCirclePos();
		pOwner->SetTargetRotToTargetPos( vTargetPos, true, false );

		if( !m_EnableChrageCircle.IsEmpty() &&
			m_dwChargeStartTime + m_dwEnableChargeTime <= FRAMEGETTIME() )
		{
			D3DXVECTOR3 vPos = pOwner->GetMagicCirclePos( m_fMagicCircleOffSet, true );
			ioPlayStage *pStage = pOwner->GetCreator();
			if( pStage )
			{
				if( m_dwEnableChrageCircleID == -1 )
				{
					D3DXVECTOR3 vScale = D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 );
					ioMapEffect *pMapEffect = pStage->CreateMapEffect( m_EnableChrageCircle, vPos, vScale );
					if( pMapEffect )
					{
						m_dwEnableChrageCircleID = pMapEffect->GetUniqueID();
					}		
				}
				else
				{
					ioMapEffect *pMapEffect = pStage->FindMapEffect( m_dwEnableChrageCircleID );
					if( pMapEffect )
						pMapEffect->SetWorldPosition( vPos );
				}
			}
		}
	}
}

void ioMagicStaffItem2::ChangeToNormalAttack( ioBaseChar *pOwner )
{
	const AttackAttribute *pAttribute = m_FormalAttack.GetAttribute( AT_NORMAL, m_iCurCombo );
	if( pAttribute )
	{
		pOwner->SetNormalAttackByAttribute( *pAttribute );
	}

	pOwner->ApplyNormalAttackResult( m_iCurCombo );
	m_FireState = FS_NORMAL_ATTACK;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_FireState;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioMagicStaffItem2::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	int iFireState;
	rkPacket >> iFireState;
	rkPacket >> m_iCurCombo;

	switch( iFireState )
	{
	case FS_NORMAL_ATTACK:
		ioAttackableItem::SetNormalAttack( pOwner, m_iCurCombo );
		m_FireState = FS_NORMAL_ATTACK;
		break;
	case FS_GATHERING:
		pOwner->EndEffect( m_GatherEffect );
		ChangeToGathering( pOwner );
		break;
	case FS_GATHERING_ATTACK:
		ChangeToGatheringAttack( pOwner );
		break;
	case FS_NONE:
		m_FireState = FS_NONE;

		pOwner->EndEffect( m_GatherEffect );
		pOwner->SetState( CS_DELAY );
		break;
	}
}

void ioMagicStaffItem2::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
}

ioWeaponItem::WeaponSubType ioMagicStaffItem2::GetSubType() const
{
	return WST_MAGIC_STAFF2;
}

void ioMagicStaffItem2::OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem )
{
	pOwner->EndEffect( m_GatherEffect );
	if( m_dwReadyEffectID != -1 )
	{
		pOwner->EndMapEffect( m_dwReadyEffectID );
		m_dwReadyEffectID = -1;
	}

	if( m_dwEnableChrageCircleID != -1 )
	{
		pOwner->EndMapEffect( m_dwEnableChrageCircleID );
		m_dwEnableChrageCircleID = -1;
	}
}

float ioMagicStaffItem2::GetCircleRadius()
{
	return m_fMagicCircleRadius;
}

bool ioMagicStaffItem2::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		if( m_FireState == FS_GATHERING || m_FireState == FS_CHARGING )
			return true;
	}

	return false;
}

void ioMagicStaffItem2::UpdateSkillGauge( ioBaseChar *pOwner, float fRate, bool bUpdate )
{
	if( m_dwTicCheckTime == 0 )
		return;

	float fRecoveryGauge = 0.0f;

	if( pOwner->GetState() == CS_RUN ||
		pOwner->GetState() == CS_DELAY ||
		pOwner->GetState() == CS_SPECIAL_MOTION ||
		pOwner->GetState() == CS_ETCITEM_MOTION ||
		IsCanUpdateGauge(pOwner) )
	{
		fRecoveryGauge = m_fDefaultGauge * pOwner->GetDelayRunGaugeRate();
	}
	else if( pOwner->GetState() == CS_DIE ||
			 pOwner->GetState() == CS_VIEW ||
			 pOwner->GetState() == CS_LOADING ||
			 pOwner->GetState() == CS_OBSERVER )
	{
		bUpdate = false;
	}
	else
	{
		fRecoveryGauge = m_fDefaultGauge * pOwner->GetEtcGaugeRate();
	}


	DWORD dwCurTime = FRAMEGETTIME();
	int iCurRecoveryTic = (int)pOwner->GetDefaultRecoveryGaugeTic();

	float fPreGauge = m_fCurSkillGauge;
	float fNewRecovery = fRecoveryGauge * fRate;

	int iTickGap = (int)( dwCurTime - m_dwTicCheckTime );
	if( iTickGap >= iCurRecoveryTic )
	{
		while( iTickGap >= iCurRecoveryTic )
		{
			if( bUpdate )
			{
				m_fCurSkillGauge = min( m_fCurSkillGauge+fNewRecovery, FLOAT100 );
			}

			iTickGap -= iCurRecoveryTic;
		}

		m_dwTicCheckTime = dwCurTime - iTickGap;
	}

//#ifndef SHIPPING 
//	if( Setting::ShowExtraInfo() )
//	{
//		float fMaxGauge = GetMaxSkillGauge( pOwner->GetCharName() );
//
//		if( fMaxGauge < FLOAT100 )
//		{
//			if( fPreGauge == 0.0f && m_fCurSkillGauge > 0.0f )
//			{
//				LOG.PrintTimeAndLog( 0, "SkillCheck - %s, MaxGauge: %f", GetSkillName().c_str(), fMaxGauge );
//				LOG.PrintTimeAndLog( 0, "SkillCheck - %s, RecoveryStart: %d", GetSkillName().c_str(), FRAMEGETTIME() );
//			}
//
//			if( fPreGauge < fMaxGauge && m_fCurSkillGauge >= fMaxGauge )
//			{
//				LOG.PrintTimeAndLog( 0, "SkillCheck - %s, RecoveryEnd: %d", GetSkillName().c_str(), FRAMEGETTIME() );
//			}
//		}
//	}
//#endif

	if( fRate > 0.0f )
		m_bUpdateEnableSkillGauge = bUpdate;
	else
		m_bUpdateEnableSkillGauge = false;
}

bool ioMagicStaffItem2::IsEnableExtraGauge( ioBaseChar *pChar )
{
	if( m_bGaugeUpdateChargeAttack && IsChargeAttackState( pChar ) )
		return false;

	return true;
}

void ioMagicStaffItem2::ChangeToGatheringAttack( ioBaseChar *pChar )
{
	if( !pChar ) return;
	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp ) return;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	pChar->EndEffect( m_GatherEffect );

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwGapTime = dwCurTime - m_dwGatheringStartTime;

	if( m_dwGatheringStartTime > 0 && m_dwMaxChargeTime > 0 )
	{
		if( m_dwMaxChargeTime > dwGapTime )
			m_fCurChargeRate = (float)dwGapTime / m_dwMaxChargeTime;
		else
			m_fCurChargeRate = FLOAT1;
	}

	pChar->SetAutoTarget( ATT_NORMAL );

	int iAniID = pGrp->GetAnimationIdx( m_GatheringAttribute.m_AttackAnimation );
	float fAniRate = m_GatheringAttribute.m_fAttackAniRate;
	if( fAniRate <= 0.0f )
		fAniRate = FLOAT1;

	DWORD dwPreDelay = m_GatheringAttribute.m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fAniRate, 0.0f, dwPreDelay, true );

	float fValue = pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fAniRate;
	m_dwFireStartTime = dwCurTime + fValue;

	m_dwFireEndTime = dwCurTime + (pGrp->GetAnimationFullTime( iAniID ) * fAniRate);
	m_dwFireEndTime += m_GatheringAttribute.m_dwEndDelay;

	pChar->SetReservedSliding( m_GatheringAttribute.m_vForceInfoList, iAniID, fAniRate, dwPreDelay );

	m_dwKeyReserveTime = 0;
	float fKeyReserve = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fAniRate;
	if( fKeyReserve > 0.0f )
	{
		m_dwKeyReserveTime = dwCurTime + fKeyReserve;
	}

	m_FireState = FS_GATHERING_ATTACK;

	if( pChar->GetUseEnableSkill( 0, SUT_NORMAL ) )
		pChar->UpdateMagicCirclePos( m_fMoveSpeed, m_fRange, m_fHeightGap, true, true );
	else
		pChar->UpdateMagicCirclePos( m_fMoveSpeed, m_fRange, m_fHeightGap, false, true );

	m_vCirclePos = pChar->GetMagicCirclePos( m_fMagicCircleOffSet, true );

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pChar, kPacket );
		kPacket << (int)m_FireState;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioMagicStaffItem2::OnGatheringAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwFireEndTime < dwCurTime )
	{
		pOwner->NormalAttackOver( false );

		// 공격키가 눌러져 있으면 공격 상태 처리
		if( pOwner->IsAttackKeyDown() && 
			!pOwner->IsJumpKeyDown() &&
			!pOwner->IsDefenseKeyDown() &&
			!pOwner->IsChatModeState(true) )
		{
			SetNoneState( pOwner );

			ioObjectItem *pItem = pOwner->GetObject();
			if( ToDrinkItem( pItem ) )
			{
				pOwner->SetDrinkState();
			}
			else
			{
				if( pOwner->IsCanNormalAttack() && !pOwner->IsBuffLimitAttack() )
				{
					pOwner->SetNormalAttack( 0 );
				}
			}

			return;
		}

		// 필살기 예약되었으면 필살기 발동
		if( m_iSkillInput >= 0 && !pOwner->IsBuffLimitSkill() )
		{
			int iCurInput = m_iSkillInput;
			if( pOwner->SetUseSkill( iCurInput, SUT_NORMAL ) )
			{
				SetNoneState( pOwner );
				return;
			}
		}

		// 방어
		if( m_bEnableDefenseState && pOwner->IsEnableDefenseByObject() && !pOwner->IsBuffLimitDefense() )
		{
			SetNoneState( pOwner );
			pOwner->SetDefenseState();
			return;
		}

		// 점프
		if( m_bEnableJumpState && pOwner->IsCanJumpState() && !pOwner->IsBuffLimitJump() )
		{
			SetNoneState( pOwner );
			pOwner->SetJumpState();
			return;
		}

		// 공격
		if( m_bEnableAttackState )
		{
			SetNoneState( pOwner );
			ioObjectItem *pItem = pOwner->GetObject();

			if( ToDrinkItem( pItem ) )
			{
				pOwner->SetDrinkState();
			}
			else
			{
				if( pOwner->IsCanNormalAttack() && !pOwner->IsBuffLimitAttack() )
				{
					pOwner->SetNormalAttack( 0 );
				}
			}
			return;
		}

		SetNoneState( pOwner );
		pOwner->SetState( CS_DELAY );
		return;
	}

	if( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
	{
		CheckReserve( pOwner );
	}

	if( m_dwFireStartTime == 0 || m_dwFireStartTime >= dwCurTime )
		return;

	D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	vDir.y = 0.0f;
	D3DXVec3Normalize( &vDir, &vDir );

	m_dwBaseWeaponIndex = pOwner->IncreaseWeaponIndexBase();

	int iCnt = m_GatheringAttribute.m_vWeaponInfoList.size();
	for( int i=0; i < iCnt; ++i )
	{
		DWORD dwWeaponIndex = m_dwBaseWeaponIndex + i;
		WeaponInfo &rkInfo = m_GatheringAttribute.m_vWeaponInfoList[i];

		FireTime kFireTime;
		kFireTime.dwStart = dwCurTime;

		kFireTime.iAttributeIdx = rkInfo.m_iWeaponIdx;
		kFireTime.iResistanceIdx = rkInfo.m_iResistanceIdx;
		kFireTime.szWoundedAni  = rkInfo.m_WoundedAnimation;
		kFireTime.dwWoundedAniDuration = rkInfo.m_dwWoundedDuration;
		kFireTime.bLoopWoundedAni = rkInfo.m_bWoundedLoopAni;

		kFireTime.dwWeaponIdx = dwWeaponIndex;
		kFireTime.eFireTimeType = FTT_NORMAL_ATTACK;

		pOwner->ExtendFireExplicit( kFireTime,
									m_vCirclePos,
									vDir,
									GetName() );
	}

	if( !m_AreaWeaponName.IsEmpty() )
	{
		D3DXQUATERNION qtRot;
		ioMath::CalcDirectionQuaternion( qtRot, -vDir );

		ioAreaWeapon *pArea = g_AreaWeaponMgr.CreateAreaWeapon( pOwner->GetCharName(), "", "",
																m_AreaWeaponName,
																m_vCirclePos,
																qtRot,
																ioAreaWeapon::CT_NORMAL );

		if( pArea )
		{
			pArea->SetAreaDirection( vDir );
		}
	}

	ioPlayStage *pStage = pOwner->GetCreator();
	if( pStage && !m_ReadyEffect.IsEmpty() )
	{
		if( m_dwReadyEffectID != -1 )
		{
			pOwner->EndMapEffect( m_dwReadyEffectID );
			m_dwReadyEffectID = -1;
		}

		D3DXVECTOR3 vScale = D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 );
		ioMapEffect *pMapEffect = pStage->CreateMapEffect( m_ReadyEffect, m_vCirclePos, vScale );
		if( pMapEffect )
		{
			m_dwReadyEffectID = pMapEffect->GetUniqueID();
		}
	}

	m_dwFireStartTime = 0;
	pOwner->DestroyMagicCircle();
}

void ioMagicStaffItem2::SetNoneState( ioBaseChar* pOwner, int eNewState /* = 0 */ )
{
	m_FireState = FS_NONE;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;
}

void ioMagicStaffItem2::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	ioAttackableItem::SetExtraInfoToWeapon( pWeapon, bJumpAttack );

	CheckChargeInfo( pWeapon, bJumpAttack );

	ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
	if( pZone )
	{
		ioBaseChar *pChar = pZone->GetOwner();
		if( pChar )
		{
			D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			pZone->SetCollisionDir( vDir );
			pZone->SetZoneValue( false, false );
		}
	}
}

void ioMagicStaffItem2::SetCancelFire( ioBaseChar *pOwner )
{
	if( !pOwner )	return;

	SetNoneState( pOwner );

	m_dwFireStartTime = 0;
	pOwner->DestroyMagicCircle();

	pOwner->SetState( CS_DELAY );
}

bool ioMagicStaffItem2::SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType )
{
	if( !pOwner ) return false;

	DWORD dwCurTime = FRAMEGETTIME();
	ClearAirJump();
	AttackType eExtendType = AT_JUMP_ATTACK;

	const AttackAttribute *pAttr = GetAttackAttribute( eExtendType );
	if( !pAttr )
	{
		LOG.PrintTimeAndLog( 0, "ioMagicStaffItem2::SetJumpAttack - AttackAttribute Not Exist" );
		return false;
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	iAniID = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioMagicStaffItem2::SetJumpAttack - JumpAttack Animation Not Exist" );
		return false;
	}

	fTimeRate   = pAttr->m_fAttackAniRate;
	DWORD dwPreDelay  = pAttr->m_dwPreDelay;

	// For AirJumpPower
	SetAirJump( pOwner, iAniID, fTimeRate );

	return false;
}

void ioMagicStaffItem2::ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
	if( m_pExtendJump )
	{
		m_pExtendJump->ProcessJumpAttack( pOwner, fHeightGap );
	}

	CheckAirJump( pOwner );
}

