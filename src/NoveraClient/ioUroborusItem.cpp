
#include "stdafx.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "ioUroborusItem.h"

#include "WeaponDefine.h"

ioUroborusItem::ioUroborusItem()
{
	m_ChargeState = CS_NONE;
	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_GatheringEffectID = -1;
	m_iCreateWeaponIndex = -1;

	m_dwRetreatEndTime = 0;
	m_dwRetreatForceTime = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;

	m_fCurCoolTime = 0.0f;
	m_GuidEffectID = -1;

	m_bSetUroborusSkill = false;
}

ioUroborusItem::ioUroborusItem( const ioUroborusItem &rhs )
: ioWeaponItem( rhs ),
  m_AttackReadyAni( rhs.m_AttackReadyAni ),
  m_ExtendAttackAttribute( rhs.m_ExtendAttackAttribute ),
  m_GatherAnimation( rhs.m_GatherAnimation ),
  m_szRetreatFr( rhs.m_szRetreatFr ),
  m_fRetreatFrRate( rhs.m_fRetreatFrRate ),
  m_fRetreatForceAmt( rhs.m_fRetreatForceAmt ),
  m_fRetreatForceFric( rhs.m_fRetreatForceFric ),
  m_UpUroborusAni( rhs.m_UpUroborusAni ),
  m_DownUroborusAni( rhs.m_DownUroborusAni ),
  m_fUpUroborusAniRate( rhs.m_fUpUroborusAniRate ),
  m_fDownUroborusAniRate( rhs.m_fDownUroborusAniRate ),
  m_fUroborusMoveSpeed( rhs.m_fUroborusMoveSpeed ),
  m_fUroborusMoveRate( rhs.m_fUroborusMoveRate ),
  m_fUroborusMoveAngle( rhs.m_fUroborusMoveAngle ),
  m_fUroborusMinAngle( rhs.m_fUroborusMinAngle ),
  m_fUroborusMaxAngle( rhs.m_fUroborusMaxAngle ),
  m_fUroborusEndJumpAmt( rhs.m_fUroborusEndJumpAmt ),
  m_dwUroborusAngleTime( rhs.m_dwUroborusAngleTime ),
  m_dwMaxCoolTime( rhs.m_dwMaxCoolTime ),
  m_dwNeedCoolTime( rhs.m_dwNeedCoolTime ),
  m_fRecoveryTime( rhs.m_fRecoveryTime ),
  m_UroborusGuidEffect( rhs.m_UroborusGuidEffect )
{
	m_ChargeState = CS_NONE;
	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_GatheringEffectID = -1;
	m_iCreateWeaponIndex = -1;

	m_dwRetreatEndTime = 0;
	m_dwRetreatForceTime = 0;

	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;

	m_fCurCoolTime = 0.0f;
	m_GuidEffectID = -1;

	m_bSetUroborusSkill = false;
}

ioUroborusItem::~ioUroborusItem()
{
}

void ioUroborusItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	rkLoader.LoadString_e( "attack_ready_animation", "", szBuf, MAX_PATH );
	m_AttackReadyAni = szBuf;

	rkLoader.LoadString_e( "gathering_animation", "", szBuf, MAX_PATH );
	m_GatherAnimation = szBuf;

	//
	m_ExtendAttackAttribute.Init();
	LoadAttackAttribute_e( "extend_attack", m_ExtendAttackAttribute, rkLoader );

	//
	rkLoader.LoadString_e( "retreat_animation_front", "", szBuf, MAX_PATH );
	m_szRetreatFr = szBuf;
	m_fRetreatFrRate = rkLoader.LoadFloat_e( "retreat_animation_front_rate", FLOAT1 );

	m_fRetreatForceAmt = rkLoader.LoadFloat_e( "retreat_force_amt", 0.0f );
	m_fRetreatForceFric = rkLoader.LoadFloat_e( "retreat_force_friction", FLOAT1 );

	//
	rkLoader.LoadString_e( "uroborus_up_ani", "", szBuf, MAX_PATH );
	m_UpUroborusAni = szBuf;
	rkLoader.LoadString_e( "uroborus_down_ani", "", szBuf, MAX_PATH );
	m_DownUroborusAni = szBuf;

	rkLoader.LoadString_e( "uroborus_guid_effect", "", szBuf, MAX_PATH );
	m_UroborusGuidEffect = szBuf;
	
	m_fUpUroborusAniRate = rkLoader.LoadFloat_e( "uroborus_up_ani_rate", FLOAT1 );
	m_fDownUroborusAniRate = rkLoader.LoadFloat_e( "uroborus_down_ani_rate", FLOAT1 );

	m_fUroborusMoveSpeed = rkLoader.LoadFloat_e( "uroborus_move_speed", 0.0f );
	m_fUroborusMoveRate = rkLoader.LoadFloat_e( "uroborus_move_rate", 0.0f );
	m_fUroborusMoveAngle = rkLoader.LoadFloat_e( "uroborus_move_angle", 0.0f );

	m_fUroborusMaxAngle = rkLoader.LoadFloat_e( "uroborus_max_angle", 0.0f );
	m_fUroborusMinAngle = rkLoader.LoadFloat_e( "uroborus_min_angle", 0.0f );

	m_fUroborusEndJumpAmt = rkLoader.LoadFloat_e( "uroborus_end_jump_amt", 0.0f );

	m_dwUroborusAngleTime = (DWORD)rkLoader.LoadInt_e( "uroborus_angle_time", 0 );

	m_dwMaxCoolTime = (DWORD)rkLoader.LoadInt_e( "max_cool_time", 0 );
	m_dwNeedCoolTime = (DWORD)rkLoader.LoadInt_e( "use_cool_time", 0 );
	m_fRecoveryTime = rkLoader.LoadFloat_e( "recover_cool_time", 0.0f );
}

ioItem* ioUroborusItem::Clone()
{
	return new ioUroborusItem( *this );
}

ioWeaponItem::WeaponSubType ioUroborusItem::GetSubType() const
{
	return WST_UROBORUS;
}

void ioUroborusItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	if( !pOwner ) return;

	m_dwKeyReserveTime = 0;

	m_bSetUroborusSkill = false;

	ClearCancelInfo();

	m_iCreateWeaponIndex = -1;

	if( m_GatheringEffectID != -1 )
	{
		pOwner->EndEffect( m_GatheringEffectID, false );
		m_GatheringEffectID = -1;
	}

	if( m_GuidEffectID != -1 )
	{
		pOwner->EndMapEffect( m_GuidEffectID, false );
		m_GuidEffectID = -1;
	}

	if( m_AttackReadyAni.IsEmpty() )
	{
		ioWeaponItem::SetNormalAttack( pOwner, iCurCombo );

		m_ChargeState = CS_NONE;
		m_iCurCombo = iCurCombo;

		pOwner->SetChargingState( false );
	}
	else
	{
		ioEntityGroup *pGrp = pOwner->GetGroup();
		pGrp->ClearAllLoopAni( FLOAT100, true );
		pGrp->ClearAllActionAni( FLOAT100, true );

		int iAniID = pGrp->GetAnimationIdx( m_AttackReadyAni );
		float fTimeRate = (float)m_dwChargeTime / pGrp->GetAnimationFullTime( iAniID );
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

		m_ChargeState = CS_CHARGING;
		m_dwAttackStartTime = FRAMEGETTIME();
		m_iCurCombo = iCurCombo;
		pOwner->SetState( CS_ATTACK );
		pOwner->SetChargingState( true );

		DWORD dwTrackingTime = FRAMEGETTIME();
		dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
		pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
	}
}

void ioUroborusItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	switch( m_ChargeState )
	{
	case CS_CHARGING:
		OnCharging( pOwner );
		break;
	case CS_GATHERING:
		OnGathering( pOwner );
		break;
	case CS_ATTACK_FIRE:
		if( pOwner->IsNeedProcess() )
		{
			CheckCancelReserve( pOwner );

			if( ProcessCancel(pOwner) )
				return;
		}

		OnAttackFire( pOwner );
		break;
	case CS_NORMAL_ATTACK:
		if( pOwner->IsNeedProcess() )
		{
			CheckCancelReserve( pOwner );

			if( ProcessCancel(pOwner) )
				return;
		}

		ioWeaponItem::CheckNormalAttack( pOwner );
		break;
	case CS_RETREAT:
		OnRetreatState( pOwner );
		break;
	}
}

void ioUroborusItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )	return;

	if( pOwner->IsAttackKeyDown() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			ChangeToGathering( pOwner );
		}
	}
	else
	{
		ChangeToAttackFire( pOwner, false );
	}
}

void ioUroborusItem::OnGathering( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	
	DWORD dwCurTime = FRAMEGETTIME();

	CheckCurUroborusAngle( pOwner );
	UpdateGuidEffect( pOwner );

	if( !pOwner->IsNeedProcess() )	return;

	// 공격버튼땜, 발사.
	if( !pOwner->IsAttackKeyDown() )
	{
		ChangeToAttackFire( pOwner, true );
		return;
	}

	// 점프키 누름. 점프상태 전환
	if( pOwner->IsJumpKeyDown() && pOwner->IsCanJumpState() && !pOwner->IsBuffLimitJump() )
	{
		pOwner->SetJumpState();
		return;
	}

	// SD 스킬 입력.
	if( pOwner->IsDefenseKeyDown() )
	{
		int iSkillNum = pOwner->GetSkillKeyInput();
		if(iSkillNum == 0 && !pOwner->IsBuffLimitSkill() && pOwner->SetUseSkill(iSkillNum, SUT_NORMAL))
		{
			m_bSetUroborusSkill = true;
			pOwner->SetChargingState( false );

			if( m_GatheringEffectID != -1 )
			{
				pOwner->EndEffect( m_GatheringEffectID, false );
				m_GatheringEffectID = -1;
			}

			if( m_GuidEffectID != -1 )
			{
				pOwner->EndMapEffect( m_GuidEffectID, false );
				m_GuidEffectID = -1;
			}

			m_ChargeState = CS_NONE;
			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
				FillItemGaugePacketHeader( pOwner, kPacket );
				kPacket << (int)m_ChargeState;
				kPacket << m_iCurCombo;
				kPacket << m_fUroborusCurAngle;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
			return;
		}
	}
	else if( pOwner->IsDirKeyDoubleClick() )
	{
		CheckRetreat( pOwner, true );
		return;
	}
}

void ioUroborusItem::OnAttackFire( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwFireStartTime > 0 && m_dwFireStartTime < dwCurTime )
	{
		m_iCreateWeaponIndex = pOwner->IncreaseWeaponIndexBase();

		const WeaponInfo &rkInfo = m_ExtendAttackAttribute.m_vWeaponInfoList.front();
		D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate();
		D3DXVECTOR3 vDir = CheckUroborusDir( pOwner );

		FireTime kFireTime;
		kFireTime.dwStart = dwCurTime;

		kFireTime.iAttributeIdx = rkInfo.m_iWeaponIdx;
		kFireTime.iResistanceIdx = rkInfo.m_iResistanceIdx;
		kFireTime.szWoundedAni  = rkInfo.m_WoundedAnimation;
		kFireTime.dwWoundedAniDuration = rkInfo.m_dwWoundedDuration;
		kFireTime.bLoopWoundedAni = rkInfo.m_bWoundedLoopAni;

		kFireTime.dwWeaponIdx = m_iCreateWeaponIndex;
		kFireTime.eFireTimeType = FTT_NORMAL_ATTACK;

		pOwner->ExtendFireExplicit( kFireTime, vPos, vDir, GetName() );
		m_dwFireStartTime = 0;

		return;
	}

	if( m_iCreateWeaponIndex == -1 )
		return;

	ioUroborusWeapon *pUroborusWeapon = ToUroborusWeapon( g_WeaponMgr.FindWeapon( pOwner, m_iCreateWeaponIndex ) );
	if( !pUroborusWeapon )
	{
		// End 처리.
		pOwner->SetState( CS_DELAY );
		return;
	}

	if( pUroborusWeapon->IsCanUroborusJump() )
	{
		CheckReserve( pOwner );

		if( pOwner->IsNeedProcess() && pOwner->IsJumpKeyDown() && (m_fCurCoolTime >= m_dwNeedCoolTime) )
		{
			DecreaseCoolTime();
			D3DXVECTOR3 vTargetPos = pUroborusWeapon->GetPosition();
			pOwner->SetUroborusState( m_UpUroborusAni, m_fUpUroborusAniRate, vTargetPos,
									  m_fUroborusMoveSpeed,
									  m_fUroborusMoveAngle,
									  m_fUroborusMoveRate,
									  m_fUroborusEndJumpAmt );

			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
				FillItemGaugePacketHeader( pOwner, kPacket );
				kPacket << UC_SET;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}

			return;
		}
	}
	else if( !pUroborusWeapon->IsUroborusMove() && !pUroborusWeapon->IsUroborusWait() )
	{
		pOwner->NormalAttackOver();

		// 공격키가 눌러져 있으면 공격 상태 처리
		if( pOwner->IsAttackKeyDown() && 
			!pOwner->IsJumpKeyDown() &&
			!pOwner->IsDefenseKeyDown() &&
			!pOwner->IsChatModeState(true) )
		{
			ClearState();

			if( pOwner->IsCanNormalAttack() && !pOwner->IsBuffLimitAttack() )
			{
				pOwner->SetNormalAttack( 0 );
			}
			return;
		}

		// 필살기 예약되었으면 필살기 발동
		if( m_iSkillInput >= 0 && !pOwner->IsBuffLimitSkill() && pOwner->SetUseSkill( m_iSkillInput, SUT_NORMAL ) )
		{
			ClearState();
			m_iSkillInput = -1;
			return;
		}

		// 방어
		if( m_bEnableDefenseState && pOwner->IsEnableDefenseByObject() && !pOwner->IsBuffLimitDefense() )
		{
			ClearState();
			pOwner->SetDefenseState();
			return;
		}

		// 점프
		if( m_bEnableJumpState && pOwner->IsCanJumpState() && !pOwner->IsBuffLimitJump() )
		{
			ClearState();
			pOwner->SetJumpState();
			return;
		}

		// 공격
		if( m_bEnableAttackState )
		{
			ClearState();
			if( pOwner->IsCanNormalAttack() && !pOwner->IsBuffLimitAttack() )
			{
				pOwner->SetNormalAttack( 0 );
			}
			return;
		}

		ClearState();
		pOwner->SetState( CS_DELAY );
		return;
	}
}

void ioUroborusItem::ChangeToGathering( ioBaseChar *pOwner )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	int iAniID = pGrp->GetAnimationIdx( m_GatherAnimation );
	float fCurRate = FLOAT1;

	m_fUroborusCurAngle = m_fUroborusMinAngle;
	m_bSetUroborusSkill = false;

	if( m_dwUroborusAngleTime > 0 )
	{
		float fAniFull = pGrp->GetAnimationFullTime( iAniID );
		if( fAniFull > 0.0f )
			fCurRate = (float)m_dwUroborusAngleTime / fAniFull;
	}

	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fCurRate );
	
	m_ChargeState = CS_GATHERING;
	m_dwGatheringStartTime = FRAMEGETTIME();

	// Effect
	if( m_GatheringEffectID != -1 )
	{
		pOwner->EndEffect( m_GatheringEffectID, false );
		m_GatheringEffectID = -1;
	}

	if( m_GuidEffectID != -1 )
	{
		pOwner->EndMapEffect( m_GuidEffectID, false );
		m_GuidEffectID = -1;
	}

	CheckCurUroborusAngle( pOwner );

	D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate();
	ioMapEffect *pMapEffect = pOwner->CreateMapEffectBySkill( m_UroborusGuidEffect, vPos );
	if( pMapEffect )
	{
		m_GuidEffectID = pMapEffect->GetUniqueID();
		
		UpdateGuidEffect( pOwner );
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioUroborusItem::ChangeToAttackFire( ioBaseChar *pOwner, bool bCharged )
{
	if( !pOwner )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	ClearCancelInfo();

	m_dwFireStartTime = 0;

	pOwner->SetChargingState( false );

	if( m_GatheringEffectID != -1 )
	{
		pOwner->EndEffect( m_GatheringEffectID, false );
		m_GatheringEffectID = -1;
	}

	if( m_GuidEffectID != -1 )
	{
		pOwner->EndMapEffect( m_GuidEffectID, false );
		m_GuidEffectID = -1;
	}

	m_dwCancelCheckTime = dwCurTime;
	m_dwInputCancelCheckTime = dwCurTime;

	if( bCharged )
	{
		ioEntityGroup *pGrp = pOwner->GetGroup();

		int iAniID = pGrp->GetAnimationIdx( m_ExtendAttackAttribute.m_AttackAnimation );
		float fTimeRate = m_ExtendAttackAttribute.m_fAttackAniRate;
		if( fTimeRate <= 0.0f )
			fTimeRate = FLOAT1;

		DWORD dwPreDelay = m_ExtendAttackAttribute.m_dwPreDelay;

		pGrp->ClearAllLoopAni( 30, true );
		pGrp->ClearAllActionAni( 30.0f, true );

		if( m_ExtendAttackAttribute.m_AttackEndAnimation.IsEmpty() )
		{
			pGrp->SetActionAni( iAniID, 30.0f, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
		}
		else
		{
			pGrp->SetActionAni( iAniID, 30.0f, 0.0f, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );
			float fWaitTime = pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + dwPreDelay;

			pGrp->SetActionAni( m_ExtendAttackAttribute.m_AttackEndAnimation,
								0.0f, FLOAT100,
								1.0f, FLOAT1/m_ExtendAttackAttribute.m_fAttackEndAniRate,
								fWaitTime, 0.0f, true );
		}

		pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );
		pOwner->SetReservedSliding( m_ExtendAttackAttribute.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

		float fFire = pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;
		m_dwFireStartTime = dwPreDelay + dwCurTime + fFire;

		m_dwKeyReserveTime = 0;
		float fReserve = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
		if( fReserve > 0.0f )
			m_dwKeyReserveTime = dwPreDelay + dwCurTime + fReserve;

		m_szCurAni = m_ExtendAttackAttribute.m_AttackAnimation;
		m_fCurAniRate = fTimeRate;

		pOwner->ApplyNormalAttackResult( m_iCurCombo, true );

		m_ChargeState = CS_ATTACK_FIRE;
	}
	else
	{
		const AttackAttribute *pAttribute = m_FormalAttack.GetAttribute( AT_NORMAL, m_iCurCombo );
		if( pAttribute )
		{
			pOwner->SetNormalAttackByAttribute( *pAttribute );

			m_szCurAni = pAttribute->m_AttackAnimation;
			m_fCurAniRate = pAttribute->m_fAttackAniRate;
		}

		pOwner->ApplyNormalAttackResult( m_iCurCombo );

		m_ChargeState = CS_NORMAL_ATTACK;
	}

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << m_iCurCombo;
		kPacket << m_fUroborusCurAngle;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioUroborusItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	int iState;
	rkPacket >> iState;
	rkPacket >> m_iCurCombo;

	switch( iState )
	{
	case CS_NONE:
		{
			rkPacket >> m_fUroborusCurAngle;

			pOwner->SetChargingState( false );

			if( m_GatheringEffectID != -1 )
			{
				pOwner->EndEffect( m_GatheringEffectID, false );
				m_GatheringEffectID = -1;
			}

			if( m_GuidEffectID != -1 )
			{
				pOwner->EndMapEffect( m_GuidEffectID, false );
				m_GuidEffectID = -1;
			}

			m_ChargeState = CS_NONE;
		}
		break;
	case CS_GATHERING:
		ChangeToGathering( pOwner );
		break;
	case CS_ATTACK_FIRE:
		ChangeToAttackFire( pOwner, true );
		rkPacket >> m_fUroborusCurAngle;
		break;
	case CS_NORMAL_ATTACK:
		ChangeToAttackFire( pOwner, false );
		rkPacket >> m_fUroborusCurAngle;
		break;
	case CS_RETREAT:
		{
			ioHashString szAni;
			float fAniRate;
			D3DXVECTOR3 vDir;

			rkPacket >> szAni >> fAniRate >> vDir;

			ChangeToRetreatState( pOwner, szAni, fAniRate, vDir );
		}
		break;
	}
}

void ioUroborusItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	int iSyncType;
	rkPacket >> iSyncType;

	switch( iSyncType )
	{
	case UC_SET:
		{
			if( m_iCreateWeaponIndex == -1 )
				return;

			ioUroborusWeapon *pUroborusWeapon = ToUroborusWeapon( g_WeaponMgr.FindWeapon( pOwner, m_iCreateWeaponIndex ) );
			if( !pUroborusWeapon )
			{
				// End 처리.
				pOwner->SetState( CS_DELAY );
				return;
			}

			D3DXVECTOR3 vTargetPos = pUroborusWeapon->GetPosition();
			pOwner->SetUroborusState( m_UpUroborusAni, m_fUpUroborusAniRate, vTargetPos,
									  m_fUroborusMoveSpeed,
									  m_fUroborusMoveAngle,
									  m_fUroborusMoveRate,
									  m_fUroborusEndJumpAmt );
		}
		break;
	}
}

void ioUroborusItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	ioAttackableItem::SetExtraInfoToWeapon( pWeapon, bJumpAttack );

	//
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

bool ioUroborusItem::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		switch( m_ChargeState )
		{
		case CS_CHARGING:
		case CS_GATHERING:
		case CS_ATTACK_FIRE:
			return true;
		}
	}

	return false;
}

bool ioUroborusItem::IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck )
{
	if( !IsChargeDash() ) return false;
	if( !bCheck ) return false;

	if( pOwner->GetState() == CS_ATTACK )
	{
		switch( m_ChargeState )
		{
		case CS_GATHERING:
			return true;
		}
	}

	return false;
}

bool ioUroborusItem::IsEnableAttackRotateCheck( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		switch( m_ChargeState )
		{
		case CS_GATHERING:
			return true;
		}
	}

	return false;
}

void ioUroborusItem::OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem )
{
	pOwner->SetChargingState( false );

	if( m_GatheringEffectID != -1 )
	{
		pOwner->EndEffect( m_GatheringEffectID, false );
		m_GatheringEffectID = -1;
	}

	if( m_GuidEffectID != -1 )
	{
		pOwner->EndMapEffect( m_GuidEffectID, false );
		m_GuidEffectID = -1;
	}
}

void ioUroborusItem::GetAutoTargetValue( ioBaseChar *pOwner,
											 float &fRange, float &fMinAngle, float &fMaxAngle,
											 AutoTargetType eType )
{
	fRange = m_fAutoTargetRange;
	fMinAngle = m_fAutoTargetMinAngle;
	fMaxAngle = m_fAutoTargetMaxAngle;
}

void ioUroborusItem::OnRetreatState( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwRetreatEndTime > 0 && m_dwRetreatEndTime < dwCurTime )
	{
		pOwner->NormalAttackOver();

		// 공격키가 눌러져 있으면 공격 상태 처리
		if( pOwner->IsAttackKeyDown() && 
			!pOwner->IsJumpKeyDown() &&
			!pOwner->IsDefenseKeyDown() &&
			!pOwner->IsChatModeState(true) )
		{
			ClearState();

			if( pOwner->IsCanNormalAttack() && !pOwner->IsBuffLimitAttack() )
			{
				pOwner->SetNormalAttack( 0 );
			}
			return;
		}

		// 필살기 예약되었으면 필살기 발동
		if( m_iSkillInput >= 0 && !pOwner->IsBuffLimitSkill() && pOwner->SetUseSkill( m_iSkillInput, SUT_NORMAL ) )
		{
			ClearState();
			m_iSkillInput = -1;
			return;
		}

		// 방어
		if( m_bEnableDefenseState && pOwner->IsEnableDefenseByObject() && !pOwner->IsBuffLimitDefense() )
		{
			ClearState();
			pOwner->SetDefenseState();
			return;
		}

		// 점프
		if( m_bEnableJumpState && pOwner->IsCanJumpState() && !pOwner->IsBuffLimitJump() )
		{
			ClearState();
			pOwner->SetJumpState();
			return;
		}

		// 공격
		if( m_bEnableAttackState )
		{
			ClearState();
			if( pOwner->IsCanNormalAttack() && !pOwner->IsBuffLimitAttack() )
			{
				pOwner->SetNormalAttack( 0 );
			}
			return;
		}

		ClearState();
		pOwner->SetState( CS_DELAY );
		return;
	}
	else
	{
		if( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
		{
			CheckReserve( pOwner );
		}

		if( m_dwRetreatForceTime > 0 && m_dwRetreatForceTime < dwCurTime )
		{
			m_dwRetreatForceTime = 0;
			pOwner->SetForcePower( m_vForceDir, m_fRetreatForceAmt, m_fRetreatForceFric );
		}
	}
}

void ioUroborusItem::ChangeToRetreatState( ioBaseChar *pOwner, const ioHashString &szAni, float fAniRate, D3DXVECTOR3 vDir )
{
	if( !pOwner ) return;
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->ClearDirDoubleClick();
	pOwner->ClearReservedSliding();

	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetChargingState( false );

	m_dwKeyReserveTime = 0;

	m_dwRetreatEndTime = 0;
	m_dwRetreatForceTime = 0;

	float fCurRate = fAniRate;
	if( fCurRate <= 0.0f )
		fCurRate = FLOAT1;

	m_ChargeState = CS_RETREAT;

	int iAniID = pGrp->GetAnimationIdx( szAni );
	pGrp->ClearAllActionAni( 0.0f, true );
	pGrp->ClearAllLoopAni( 0.0f, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fCurRate );

	m_dwRetreatEndTime = dwCurTime + pGrp->GetAnimationFullTime(iAniID) * fCurRate;

	DWORD dwTime = (DWORD)pGrp->GetAnimationEventTime_e( iAniID, "fire_sliding" );
	m_dwRetreatForceTime = dwCurTime + dwTime;

	m_vForceDir = vDir;

	DWORD dwKeyReserve = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fCurRate;
	if( dwKeyReserve > 0 )
		m_dwKeyReserveTime = dwCurTime + dwKeyReserve;

	pOwner->ApplyNormalAttackResult( m_iCurCombo );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_ChargeState;
		kPacket << m_iCurCombo;
		kPacket << szAni;
		kPacket << fCurRate;
		kPacket << vDir;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioUroborusItem::CheckRetreat( ioBaseChar *pOwner, bool bFront )
{
	if( !pOwner ) return false;

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->SetChargingState( false );

	if( m_GatheringEffectID != -1 )
	{
		pOwner->EndEffect( m_GatheringEffectID, false );
		m_GatheringEffectID = -1;
	}

	if( m_GuidEffectID != -1 )
	{
		pOwner->EndMapEffect( m_GuidEffectID, false );
		m_GuidEffectID = -1;
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = 0;

	// KeyDir
	ioUserKeyInput::DirKeyInput eDir = pOwner->CheckCurDirKey();
	float fYawD = 180.0f + 45.0f * ( eDir - ioUserKeyInput::DKI_UP );
	fYawD = ioMath::ArrangeHead( fYawD );

	D3DXQUATERNION qtRotate;
	D3DXQuaternionRotationYawPitchRoll( &qtRotate, DEGtoRAD( fYawD ), 0.0f, 0.0f );
	D3DXVECTOR3 vSightDir = qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVECTOR3 vMoveDir( vSightDir.x, 0.0f, vSightDir.z );
	D3DXVec3Normalize( &vMoveDir, &vMoveDir );

	ChangeToRetreatState( pOwner, m_szRetreatFr, m_fRetreatFrRate, vMoveDir );

	return true;
}

void ioUroborusItem::CheckReserve( ioBaseChar *pOwner )
{
	bool bReserveSkill = false;
	int iSkillNum = pOwner->GetSkillKeyInput();
	if( iSkillNum >= 0 )
	{
		m_iSkillInput = iSkillNum;
		bReserveSkill = true;
	}

	if( pOwner->IsDefenseKeyDown() && !m_bEnableDefenseState && !bReserveSkill )
	{
		m_bEnableDefenseState = true;
		m_bEnableAttackState = false;
		m_bEnableJumpState = false;

		m_iSkillInput = -1;
	}
	else if( pOwner->IsAttackKey() && !m_bEnableAttackState && !bReserveSkill )
	{
		m_bEnableAttackState = true;
		m_bEnableDefenseState = false;
		m_bEnableJumpState = false;

		m_iSkillInput = -1;
	}
	else if( pOwner->IsJumpKey() && !m_bEnableJumpState && !bReserveSkill )
	{
		m_bEnableJumpState = true;
		m_bEnableAttackState = false;
		m_bEnableDefenseState = false;
		m_iSkillInput = -1;
	}
}

void ioUroborusItem::ClearState()
{
	m_ChargeState = CS_NONE;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;
}

bool ioUroborusItem::SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType )
{
	if( !pOwner ) return false;

	DWORD dwCurTime = FRAMEGETTIME();
	ClearAirJump();

	AttackType eExtendType = AT_JUMP_ATTACK;

	const AttackAttribute *pAttr = GetAttackAttribute( eExtendType );
	if( !pAttr )
	{
		LOG.PrintTimeAndLog( 0, "ioUroborusItem::SetJumpAttack - AttackAttribute Not Exist" );
		return false;
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	iAniID = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioUroborusItem::SetJumpAttack - JumpAttack Animation Not Exist" );
		return false;
	}

	fTimeRate   = pAttr->m_fAttackAniRate;
	DWORD dwPreDelay  = pAttr->m_dwPreDelay;

	// For AirJumpPower
	SetAirJump( pOwner, iAniID, fTimeRate );

	return false;
}

void ioUroborusItem::ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
	if( m_pExtendJump )
	{
		m_pExtendJump->ProcessJumpAttack( pOwner, fHeightGap );
	}

	CheckAirJump( pOwner );
}

void ioUroborusItem::CheckCurUroborusAngle( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if( m_dwUroborusAngleTime == 0 )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwGapTime = dwCurTime - m_dwGatheringStartTime;

	dwGapTime = dwGapTime % m_dwUroborusAngleTime;

	float fGapAngle = fabsf(m_fUroborusMaxAngle - m_fUroborusMinAngle);
	float fHalfTime = (float)m_dwUroborusAngleTime * FLOAT05;

	if( dwGapTime > fHalfTime )
	{
		dwGapTime -= fHalfTime;

		float fRate = (float)dwGapTime / fHalfTime;
		m_fUroborusCurAngle = m_fUroborusMaxAngle - ( fGapAngle * fRate );
	}
	else if( dwGapTime < fHalfTime )
	{
		float fRate = (float)dwGapTime / fHalfTime;
		m_fUroborusCurAngle = m_fUroborusMinAngle + ( fGapAngle * fRate );
	}
	else
	{
		m_fUroborusCurAngle = m_fUroborusMaxAngle;
	}
}

D3DXVECTOR3 ioUroborusItem::CheckUroborusDir( ioBaseChar *pOwner )
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
	float fCurAngle = ioMath::ArrangeHead( m_fUroborusCurAngle );
	D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fCurAngle) );

	vDir = qtNewRot * vSightDir;
	D3DXVec3Normalize( &vDir, &vDir );

	return vDir;
}

D3DXVECTOR3 ioUroborusItem::CheckUroborusDirForSkill( ioBaseChar *pOwner )
{
	D3DXVECTOR3 vDir = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	if( !m_bSetUroborusSkill )
		return vDir;

	vDir = CheckUroborusDir( pOwner );

	return vDir;
}

int ioUroborusItem::GetMaxBullet()
{
	return (int)m_dwMaxCoolTime;
}

int ioUroborusItem::GetNeedBullet()
{
	return (int)m_dwNeedCoolTime;
}

int ioUroborusItem::GetCurBullet()
{
	DWORD dwCurCoolTime = m_fCurCoolTime;
	dwCurCoolTime = max( 0, min( dwCurCoolTime, m_dwMaxCoolTime ) );

	return (int)dwCurCoolTime;
}

void ioUroborusItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	DWORD dwCurBullet = iCurBullet;
	dwCurBullet = max( 0, min( dwCurBullet, m_dwMaxCoolTime ) );

	m_fCurCoolTime = (float)dwCurBullet;
}

void ioUroborusItem::OnReleased( ioBaseChar *pOwner )
{
	InitCoolTime();

	ioAttackableItem::OnReleased( pOwner );
}

void ioUroborusItem::InitCoolTime()
{
	m_fCurCoolTime = 0.0f;
}

void ioUroborusItem::MaxCoolTime()
{
	m_fCurCoolTime = (float)m_dwMaxCoolTime;
}

void ioUroborusItem::DecreaseCoolTime()
{
	if( m_fCurCoolTime < (float)m_dwNeedCoolTime )
	{
		m_fCurCoolTime = 0.0f;
	}
	else
	{
		m_fCurCoolTime -= (float)m_dwNeedCoolTime;
	}
}

void ioUroborusItem::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	// check bullet
	if( m_dwMaxCoolTime <= m_fCurCoolTime )
		return;

	// check owner state
	switch( pOwner->GetState() )
	{
	case CS_DELAY:
	case CS_RUN:
		break;
	default:
		return;
	}

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();
	float fGauge = m_fRecoveryTime * fTimePerSec;

	m_fCurCoolTime += fGauge;
	m_fCurCoolTime = (float)min( m_fCurCoolTime, (float)m_dwMaxCoolTime );
}

void ioUroborusItem::UpdateGuidEffect( ioBaseChar *pOwner )
{
	// GuidEffect
	ioPlayStage *pStage = pOwner->GetCreator();
	if( m_GuidEffectID != -1 && pStage )
	{
		ioMapEffect *pMapEffect = pStage->FindMapEffect( m_GuidEffectID );
		if( pMapEffect )
		{
			pMapEffect->SetWorldPosition( pOwner->GetMidPositionByRate() );

			ioEffect *pEff = pMapEffect->GetEffect();
			if( pEff )
			{
				D3DXVECTOR3 vDir = CheckUroborusDir( pOwner );

				D3DXQUATERNION qtRot = Help::ConvertDirToQuaternion( vDir );
				pEff->GetParentNode()->SetOrientation( qtRot );
			}
		}
	}

}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ioUroborusSpecialState::ioUroborusSpecialState()
{
}

ioUroborusSpecialState::~ioUroborusSpecialState()
{
}

void ioUroborusSpecialState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );
}

void ioUroborusSpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );
}

void ioUroborusSpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	if( m_fUroborusMaxRange < m_fUroborusCurRange )
	{
		pOwner->SetSKillEndJumpState( m_fUroborusEndJumpAmt, false, true, false );
		return;
	}
}

void ioUroborusSpecialState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
	return;
}

bool ioUroborusSpecialState::ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime )
{
	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return true;

	bool bCurMove = false;
	D3DXVECTOR3 vTotalMove( 0.0f, 0.0f, 0.0f );

	float fCurTimeGap = fTimePerSec * FLOAT1000;
	while( fCurTimeGap > 0.0f )
	{
		float fDivideTimePerSec = min( fCurTimeGap, MIN_FRAME_TIME ) * 0.001f;
		fCurTimeGap -= MIN_FRAME_TIME;

		D3DXVECTOR3 vMoveAmt;
		float fFinalMoveAmt = m_fUroborusMoveSpeed * fDivideTimePerSec;
		if( fFinalMoveAmt > 0.0f )	// 감속이 심해서 음수가 나올 경우는 제외시킴
		{
			vMoveAmt = m_UroborusDir * fFinalMoveAmt;
			vTotalMove += vMoveAmt;
			bCurMove = true;
		}
	}

	if( bCurMove )
	{
		float fCurLength = D3DXVec3Length( &vTotalMove );
		m_fUroborusCurRange += fCurLength;

		bool bCol = false;
		if( pStage->ReCalculateMoveVectorByTerrain( pOwner, &vTotalMove, bCol ) )
		{
			float fPreHeight = pOwner->GetBottomHeight();

			pOwner->Translate( vTotalMove );

			if( pOwner->GetForceAmt() != 0.0f )	// 빠른 ForceMove로 경사면을 통과하는 경우에 대한 보강
			{
				float fMoveHeight = pOwner->GetBottomHeight();

				if( fPreHeight > 0.0f && fMoveHeight <= FLOAT1 )
				{
					D3DXVECTOR3 vCurPos = pOwner->GetWorldPosition();

					float fTopHeight = pStage->GetMapHeight( vCurPos.x, vCurPos.z, pOwner, false, pOwner->GetMidHeightByRate( FLOAT1 ) );
					if( fTopHeight > FLOAT1 )
					{
						vCurPos.y = max( fTopHeight, fPreHeight );
						pOwner->SetWorldPosition( vCurPos );
					}
				}
			}
		}

		if( bCol )
		{
			pOwner->SetSKillEndJumpState( 0.1f, false, true, false );
			return true;
		}
	}

	if( pOwner->IsAxisLimit() )
	{
		D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
		pStage->CheckAxisLimit( &vPos );
		pOwner->SetWorldPosition( vPos );
	}

	return true;
}

void ioUroborusSpecialState::SetUroborusInfo( D3DXVECTOR3 vMoveDir, D3DXVECTOR3 vTargetPos, float fMoveSpeed, float fMaxRange, float fEndJumpAmt )
{
	m_UroborusDir = vMoveDir;
	m_UroborusTargetPos = vTargetPos;
	m_fUroborusMoveSpeed = fMoveSpeed;
	m_fUroborusMaxRange = fMaxRange;
	m_fUroborusEndJumpAmt = fEndJumpAmt;
	m_fUroborusCurRange = 0.0f;
}