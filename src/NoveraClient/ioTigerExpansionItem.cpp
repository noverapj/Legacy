

#include "stdafx.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"
#include "WeaponDefine.h"
#include "ItemDefine.h"

ioTigerExpansionItem::ioTigerExpansionItem()
{
	m_ChargeState = CS_NONE;
	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_dwDodgeDefenseEnd = 0;

	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;

	m_bCharged = false;
	m_bUsedExtraGauge = false;
}

ioTigerExpansionItem::ioTigerExpansionItem( const ioTigerExpansionItem &rhs )
	: ioWeaponItem( rhs ),
	m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
	m_vExtendAttributeList( rhs.m_vExtendAttributeList ),
	m_vDodgeAniList( rhs.m_vDodgeAniList ),
	m_fDodgeAniRate( rhs.m_fDodgeAniRate ),
	m_fDodgeForce( rhs.m_fDodgeForce ),
	m_fDodgeFrictionRate( rhs.m_fDodgeFrictionRate ),
	m_bEnableExtraGauge( rhs.m_bEnableExtraGauge ),
	m_ExtraGaugeBuffList( rhs.m_ExtraGaugeBuffList ),
	m_dwChargingFullTime( rhs.m_dwChargingFullTime ),
	m_fGatherAniRate( rhs.m_fGatherAniRate ),
	m_GatherAnimation( rhs.m_GatherAnimation ),
	m_GatheringEffect( rhs.m_GatheringEffect ),
	m_GatheringMaxEffect( rhs.m_GatheringMaxEffect ),
	m_GatheringMaxSound( rhs.m_GatheringMaxSound ),
	m_fRunWaitAniRate( rhs.m_fRunWaitAniRate ),
	m_RunWaitAni( rhs.m_RunWaitAni ),
	m_fRunSpeed( rhs.m_fRunSpeed ),
	m_dwRotateTime( rhs.m_dwRotateTime ),
	m_ChargeAttackAttribute( rhs.m_ChargeAttackAttribute ),
	m_GatheringMoveFr( rhs.m_GatheringMoveFr ),
	m_GatheringMoveBack( rhs.m_GatheringMoveBack ),
	m_GatheringMoveLt( rhs.m_GatheringMoveLt ),
	m_GatheringMoveRt( rhs.m_GatheringMoveRt ),
	m_fGatheringMoveAniRate( rhs.m_fGatheringMoveAniRate ),
	m_RunningAni( rhs.m_RunningAni ),
	m_fRunningAniRate( rhs.m_fRunningAniRate ),
	m_fMaxRunTimeRate( rhs.m_fMaxRunTimeRate ),
	m_fMaxRunSpeedRate( rhs.m_fMaxRunSpeedRate ),
	m_dwRunningDuration( rhs.m_dwRunningDuration ),
	m_RunEndAni( rhs.m_RunEndAni ),
	m_fRunEndAniRate( rhs.m_fRunEndAniRate ),
	m_fGatherDefenceJumpPower( rhs.m_fGatherDefenceJumpPower ),
	m_fHalfDefenceJumpPower( rhs.m_fHalfDefenceJumpPower ),
	m_fGatherEndJumpPower( rhs.m_fGatherEndJumpPower ),
	m_fRunJumpPower( rhs.m_fRunJumpPower )
{
	m_ChargeState = CS_NONE;
	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_dwDodgeDefenseEnd = 0;

	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;

	m_bCharged = false;
	m_bUsedExtraGauge = false;
	//////////////////////////////////////////////////////////////////////////
	m_RunState = RS_NONE;
	m_GatheringEffectID = -1;
	m_GatheringMaxEffectID = -1;
	m_bFirstChargeMoveMotion = true;
	m_bSetChargingMoveAni = false;
}

ioTigerExpansionItem::~ioTigerExpansionItem()
{
}

void ioTigerExpansionItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	int i;
	char szBuf[MAX_PATH];
	char szKey[MAX_PATH];

	int iExtendCnt = rkLoader.LoadInt_e( "extend_attack_cnt", 0 );
	m_vExtendAttributeList.clear();
	m_vExtendAttributeList.reserve( iExtendCnt );
	for( i=0 ; i<iExtendCnt ; i++ )
	{
		AttackAttribute kAttribute;
		wsprintf_e( szBuf, "extend_attack0%d", i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader );
		m_vExtendAttributeList.push_back( kAttribute );
	}

	m_AttackReadyAniList.clear();
	for( i=0 ; i<iExtendCnt ; i++ )
	{
		wsprintf_e( szBuf, "attack_ready_animation%d", i+1 );
		rkLoader.LoadString( szBuf, "", szBuf, MAX_PATH );
		m_AttackReadyAniList.push_back( ioHashString(szBuf) );
	}

	//
	m_fDodgeForce = rkLoader.LoadFloat_e( "dodge_force", 0.0f );
	m_fDodgeFrictionRate = rkLoader.LoadFloat_e( "dodge_friction_rate", 0.0f );

	m_fDodgeAniRate = rkLoader.LoadFloat_e( "dodge_ani_rate", FLOAT1 );

	m_vDodgeAniList.clear();
	m_vDodgeAniList.reserve( 4 );

	rkLoader.LoadString_e( "dodge_animation_front", "", szBuf, MAX_PATH );
	m_vDodgeAniList.push_back( ioHashString(szBuf) );
	rkLoader.LoadString_e( "dodge_animation_right", "", szBuf, MAX_PATH );
	m_vDodgeAniList.push_back( ioHashString(szBuf) );
	rkLoader.LoadString_e( "dodge_animation_back", "", szBuf, MAX_PATH );
	m_vDodgeAniList.push_back( ioHashString(szBuf) );
	rkLoader.LoadString_e( "dodge_animation_left", "", szBuf, MAX_PATH );
	m_vDodgeAniList.push_back( ioHashString(szBuf) );

	//
	m_bEnableExtraGauge = rkLoader.LoadBool_e( "extra_gauge_enable", false );

	m_ExtraGaugeBuffList.clear();
	int iBuffCnt = rkLoader.LoadInt_e( "extra_gauge_buff_cnt", 0 );
	for( int i=0; i < iBuffCnt; ++i )
	{
		wsprintf_e( szKey, "extra_gauge_buff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

		m_ExtraGaugeBuffList.push_back( szBuf );
	}
	//////////////////////////////////////////////////////////////////////////
	wsprintf_e( szBuf, "charge_attack" );
	LoadAttackAttribute( szBuf, m_ChargeAttackAttribute, rkLoader );

	m_dwChargingFullTime = rkLoader.LoadInt_e( "full_charge_time", 0 );

	m_fGatherAniRate = rkLoader.LoadFloat_e( "gathering_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "gathering_animation", "", szBuf, MAX_PATH );
	m_GatherAnimation = szBuf;

	rkLoader.LoadString_e( "gathering_effect", "", szBuf, MAX_PATH );
	m_GatheringEffect = szBuf;
	rkLoader.LoadString_e( "gathering_max_effect", "", szBuf, MAX_PATH );
	m_GatheringMaxEffect = szBuf;

	rkLoader.LoadString_e( "gathering_max_sound", "", szBuf, MAX_PATH );
	m_GatheringMaxSound = szBuf;

	// Charge Move Ani
	rkLoader.LoadString_e( "gathering_move_front", "", szBuf, MAX_PATH );
	m_GatheringMoveFr = szBuf;
	rkLoader.LoadString_e( "gathering_move_back", "", szBuf, MAX_PATH );
	m_GatheringMoveBack = szBuf;
	rkLoader.LoadString_e( "gathering_move_right", "", szBuf, MAX_PATH );
	m_GatheringMoveRt = szBuf;
	rkLoader.LoadString_e( "gathering_move_left", "", szBuf, MAX_PATH );
	m_GatheringMoveLt = szBuf;

	m_fGatheringMoveAniRate = rkLoader.LoadFloat_e( "gathering_move_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "charge_run_wait_ani", "", szBuf, MAX_PATH );
	m_RunWaitAni = szBuf;
	m_fRunWaitAniRate = rkLoader.LoadFloat_e( "charge_run_wait_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "charge_running_ani", "", szBuf, MAX_PATH );
	m_RunningAni = szBuf;
	m_fRunningAniRate = rkLoader.LoadFloat_e( "charge_running_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "charge_run_end_ani", "", szBuf, MAX_PATH );
	m_RunEndAni = szBuf;
	m_fRunEndAniRate = rkLoader.LoadFloat_e( "charge_run_end_ani_rate", FLOAT1 );

	m_dwRunningDuration = (DWORD)rkLoader.LoadInt_e( "charge_running_duration", 0 );

	// ChargingRate
	m_fMaxRunTimeRate = rkLoader.LoadFloat_e( "gathering_max_run_time_rate", FLOAT1 );
	m_fMaxRunSpeedRate = rkLoader.LoadFloat_e( "gathering_max_run_speed_rate", FLOAT1 );

	// Run & Rotate
	m_fRunSpeed = rkLoader.LoadFloat_e( "charge_run_speed", 0.0f );
	m_dwRotateTime = (DWORD)rkLoader.LoadInt_e( "charge_run_rotate_speed", 0 );

	m_fRunJumpPower = rkLoader.LoadFloat_e( "run_jump_power", 0.f );
	m_fHalfDefenceJumpPower = rkLoader.LoadFloat_e( "half_charge_defence_jump_power", 0.f );
	m_fGatherDefenceJumpPower = rkLoader.LoadFloat_e( "gether_charge_defence_jump_power", 0.f );

	m_fGatherEndJumpPower = rkLoader.LoadFloat_e( "charge_attack_end_jump_power", 0.f );
}

ioItem* ioTigerExpansionItem::Clone()
{
	return new ioTigerExpansionItem( *this );
}

ioWeaponItem::WeaponSubType ioTigerExpansionItem::GetSubType() const
{
	return WST_TIGER_EXPASION_ITEM;
}

void ioTigerExpansionItem::OnReleased( ioBaseChar *pOwner )
{
	RemoveExtraGaugeBuff( pOwner );
	m_fCurExtraGauge = 0.0f;

	ioAttackableItem::OnReleased( pOwner );
}

void ioTigerExpansionItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	ClearCancelInfo();

	m_bCharged = false;
	m_bFirstChargeMoveMotion = true;

	if( m_GatheringEffectID != -1 )
	{
		pOwner->EndEffect( m_GatheringEffectID, false );
		m_GatheringEffectID = -1;
	}

	if( m_GatheringMaxEffectID != -1 )
	{
		pOwner->EndEffect( m_GatheringMaxEffectID, false );
		m_GatheringMaxEffectID = -1;
	}

	int iExtendCnt = m_vExtendAttributeList.size();
	if( !COMPARE( iCurCombo, 0, iExtendCnt ) )
	{
		ioWeaponItem::SetNormalAttack( pOwner, iCurCombo );

		m_ChargeState = CS_NONE;
		m_iCurCombo = iCurCombo;
	}
	else
	{
		ioEntityGroup *pGrp = pOwner->GetGroup();
		pGrp->ClearAllLoopAni( FLOAT100, true );
		pGrp->ClearAllActionAni( FLOAT100, true );

		int iAniID = pGrp->GetAnimationIdx( m_AttackReadyAniList[iCurCombo] );
		float fTimeRate = (float)m_dwChargeTime / pGrp->GetAnimationFullTime( iAniID );
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

		m_ChargeState = CS_CHARGING;
		m_dwAttackStartTime = FRAMEGETTIME();
		m_iCurCombo = iCurCombo;
		pOwner->SetState( CS_ATTACK );

		DWORD dwTrackingTime = FRAMEGETTIME();
		dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
		pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
	}
}

void ioTigerExpansionItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	switch( m_ChargeState )
	{
	case CS_CHARGING:
		OnCharging( pOwner );
		break;
	case CS_GATHERING:
		OnGathering( pOwner );
		break;
	case CS_RUN:
		OnRunState( pOwner );
		break;
	case CS_ATTACK_FIRE:
		{
			if( m_bCharged )
				pOwner->CheckExtraAniJump();

			if( pOwner->IsNeedProcess() )
			{
				if( m_bCharged )
				{
					if( pOwner->CheckExtendAttackEndJump() )
						return;
				}

				CheckCancelReserve( pOwner );

				if( ProcessCancel(pOwner) )
					return;
			}

			ioWeaponItem::CheckNormalAttack( pOwner );
		}
		break;
	}

	if( m_ChargeState == CS_GATHERING )
		CheckChargingMoveState( pOwner );
}

void ioTigerExpansionItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )	return;

	//	LOG.PrintTimeAndLog( 0, "Charge detected and process attack fire : start : %u, charge time : %u, cur time : %u",
	//			m_dwAttackStartTime, m_dwChargeTime, FRAMEGETTIME() );

	if( pOwner->IsAttackKeyDown() )
	{
		if( m_dwAttackStartTime + m_dwChargingFullTime <= FRAMEGETTIME() )
		{
			SetGatheringState( pOwner );
		}
	}
	else
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
			ChangeToAttackFire( pOwner, true );
		else
			ChangeToAttackFire( pOwner, false );
	}
}


void ioTigerExpansionItem::OnGathering( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )	
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( pOwner->IsAttackKeyDown() )
	{
		DWORD dwGapTime = dwCurTime - m_dwGatheringStartTime;
		float fCurChargeRate = 0.0f;
		if( m_dwGatheringStartTime > 0 && m_dwMaxChargeTime > 0 )
		{
			if( m_dwMaxChargeTime > dwGapTime )
				fCurChargeRate = (float)dwGapTime / m_dwMaxChargeTime;
			else
				fCurChargeRate = FLOAT1;
		}

		m_iMaxBullet = 100;
		m_iCurBullet = 100 * fCurChargeRate;

		if( (m_dwGatheringStartTime+m_dwMaxChargeTime < dwCurTime) &&
			m_GatheringMaxEffectID == -1 )
		{
			SetMaxCharge( pOwner, true );
		}
	}
	else
		SetRunState( pOwner );
}


void ioTigerExpansionItem::SetMaxCharge( ioBaseChar *pOwner, bool bSend )
{
	pOwner->EndEffect( m_GatheringEffectID, false );
	m_GatheringEffectID = -1;

	ioEffect *pEffect = pOwner->AttachEffect( m_GatheringMaxEffect );
	if( pEffect )
	{
		m_GatheringMaxEffectID = pEffect->GetUniqueID();
	}

	if( !m_GatheringMaxSound.IsEmpty() )
	{
		ioEntityGroup *pGrp = pOwner->GetGroup();
		if( pGrp )
		{
			g_SoundMgr.PlaySound( m_GatheringMaxSound, pGrp->GetParentSceneNode() );
		}
	}

	if( bSend && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << true;		// Max Charge
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


void ioTigerExpansionItem::SetGatheringState( ioBaseChar *pOwner )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	float fCurRate = FLOAT1;
	if( m_fGatherAniRate > 0.0f )
		fCurRate = m_fGatherAniRate;

	pGrp->SetLoopAni( m_GatherAnimation, FLOAT100, FLOAT1, FLOAT1/fCurRate );

	ioEffect *pEffect = pOwner->AttachEffect( m_GatheringEffect );
	if( pEffect )
		m_GatheringEffectID = pEffect->GetUniqueID();


	m_ChargeState = CS_GATHERING;
	m_dwGatheringStartTime = FRAMEGETTIME();

	pOwner->SetAttackMoveEnable( true );
	pOwner->ChangeDirectionExplicitIfDirChanged( true );

	m_ChargeStartDirKey = pOwner->GetCurDirKey();

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << false;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


void ioTigerExpansionItem::SetRunState( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->SetAttackMoveEnable( false );

	if( m_GatheringEffectID != -1 )
	{
		pOwner->EndEffect( m_GatheringEffectID, false );
		m_GatheringEffectID = -1;
	}

	if( m_GatheringMaxEffectID != -1 )
	{
		pOwner->EndEffect( m_GatheringMaxEffectID, false );
		m_GatheringMaxEffectID = -1;
	}

	if( pOwner->IsNeedProcess() )
	{
		m_fCurChargeRate = 0.0f;
		m_fCurChargeRateForSkill = 0.0f;

		DWORD dwGapTime = dwCurTime - m_dwGatheringStartTime;

		if( m_dwGatheringStartTime > 0 && m_dwMaxChargeTime > 0 )
		{
			if( m_dwMaxChargeTime > dwGapTime )
				m_fCurChargeRate = (float)dwGapTime / m_dwMaxChargeTime;
			else
				m_fCurChargeRate = FLOAT1;
		}
	}

	m_dwRunWaitEndTime = 0;
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( pGrp )
	{
		pGrp->ClearAllActionAni( FLOAT100 );
		pGrp->ClearAllLoopAni(  FLOAT100 );

		float fCurRate = FLOAT1;
		if( m_fRunWaitAniRate > 0.0f )
			fCurRate = m_fRunWaitAniRate;

		int iAniID = pGrp->GetAnimationIdx( m_RunWaitAni );
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1 / fCurRate );

		m_dwRunWaitEndTime = dwCurTime + pGrp->GetAnimationFullTime(iAniID) * fCurRate;
	}

	m_ChargeState = CS_RUN;
	m_RunState = RS_WAIT;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		kPacket << m_RunState;
		kPacket << m_fCurChargeRate;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


void ioTigerExpansionItem::OnRunState( ioBaseChar *pOwner )
{
	switch( m_RunState )
	{
	case RS_WAIT:
		OnRunWait( pOwner );
		break;
	case RS_RUN:
		OnRunning( pOwner );
		break;
	case RS_END:
		OnRunEnd( pOwner );
		break;
	case RS_COLLISION:
		OnRunCollision( pOwner );
		break;
	}
}


void ioTigerExpansionItem::ChangeToRunning( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwRunningEndTime = dwCurTime;
	m_fCurRunSpeed = m_fRunSpeed;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( pGrp )
	{
		float fCurChargeRunRate = FLOAT1;
		float fCurMaxRunRate = max( 0.0f, (m_fMaxRunTimeRate - FLOAT1) );
		fCurChargeRunRate += fCurMaxRunRate * m_fCurChargeRate;

		DWORD dwCurDuration = (m_dwRunningDuration * fCurChargeRunRate);
		m_dwRunningEndTime += dwCurDuration;

		float fCurChargeRunSpeedRate = FLOAT1;
		float fCurMaxRunSpeedRate = max( 0.0f, (m_fMaxRunSpeedRate - FLOAT1) );
		fCurChargeRunSpeedRate += fCurMaxRunSpeedRate * m_fCurChargeRate;

		m_fCurRunSpeed = m_fRunSpeed * fCurChargeRunSpeedRate;

		pGrp->ClearAllActionAni( FLOAT100 );
		pGrp->ClearAllLoopAni( FLOAT100 );

		float fCurRate = FLOAT1;
		if( m_fRunningAniRate > 0.0f )
			fCurRate = m_fRunningAniRate;

		float fAniRate = FLOAT1;
		if( m_fRunSpeed > 0.0f )
			fAniRate = m_fCurRunSpeed / m_fRunSpeed;

		fCurRate = fCurRate / fAniRate;

		int iAniID = pGrp->GetAnimationIdx( m_RunningAni );
		pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1 / fCurRate );

		pOwner->RefreshFireTimeList( iAniID, m_ChargeAttackAttribute.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fCurRate, 0 );

		pOwner->IncreaseWeaponIndexBase();

		DWORD dwFireTime = pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fCurRate;
	//	m_dwFireDuration = dwCurDuration - dwFireTime;

		pOwner->CheckCharColSkipTime( iAniID, fCurRate, 0 );
	}

	m_RunState = RS_RUN;
	m_dwRunningStartTime = dwCurTime;
	pOwner->SetTargetRotToRotate( pOwner->GetTargetRot(), true, false );
	//pOwner->SetCharCollisionSkipTime( dwCurTime, m_dwRunningEndTime, m_bOnlyTeamCharColSkip );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		kPacket << m_RunState;
		kPacket << m_fCurChargeRate;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


void ioTigerExpansionItem::ChangeToRunEnd( ioBaseChar *pOwner, bool bJump )
{
	if( !pOwner ) return;

	m_dwRunningEndTime = 0;

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwRunEndTime = dwCurTime;

	m_iCurBullet = 0;

	pOwner->SetChargingState( false );

	if( bJump )
	{
		pOwner->ClearAttackFireTimeAndSkill();
		pOwner->SetCurMoveSpeed( 0.0f );
		pOwner->SetExtendAttackEndJump( m_fGatherEndJumpPower, m_fExtendLandingRate );
		pOwner->ClearDirDoubleClick();
	}
	else
	{
		m_dwKeyReserveTime = 0;

		ioEntityGroup *pGrp = pOwner->GetGroup();
		if( pGrp )
		{
			pOwner->ClearAttackFireTimeAndSkill();
			//pOwner->SetCurMoveSpeed( 0.0f );

			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->ClearAllLoopAni( FLOAT100, true );

			float fCurRate = FLOAT1;
			if( m_fRunEndAniRate > 0.0f )
				fCurRate = m_fRunEndAniRate;

			int iAniID = pGrp->GetAnimationIdx( m_RunEndAni );
			pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1 / fCurRate );

			m_dwRunEndTime += pGrp->GetAnimationFullTime(iAniID) * fCurRate;

			DWORD dwKeyReserve = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fCurRate;
			if( dwKeyReserve > 0 )
				m_dwKeyReserveTime = dwCurTime + dwKeyReserve;
		}
	}

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	pOwner->SetCharCollisionSkipTime( 0, 0 );

	m_RunState = RS_END;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		kPacket << m_RunState;
		kPacket << bJump;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


void ioTigerExpansionItem::OnRunWait( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwRunWaitEndTime > 0 && m_dwRunWaitEndTime < dwCurTime )
	{
		m_dwRunWaitEndTime = 0;
		ChangeToRunning( pOwner );
	}
}


void ioTigerExpansionItem::OnRunning( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwRunningEndTime > 0 && m_dwRunningEndTime < dwCurTime )
	{
		//float fGapHeight = pOwner->GetWorldPosition().y - pOwner->GetBottomHeight();

		if( pOwner->IsFloating() )
			ChangeToRunEnd( pOwner, true );
		else
			ChangeToRunEnd( pOwner, false );

		return;
	}
	else
	{
		ioPlayStage *pStage = g_ItemMaker.GetPlayStage();
		if( pOwner->IsNeedProcess() && pStage )
			pStage->SetTargetLookAt( true );

		pOwner->SetCurMoveSpeed( m_fCurRunSpeed );

		CheckKeyInput( pOwner );
		ProcessRotate( pOwner );
	}
}


void ioTigerExpansionItem::SetRunJump( ioBaseChar *pOwner, bool bSend )
{
	pOwner->SetJumpPower( m_fRunJumpPower );

	//SendPacket
	if( bSend )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << CS_JUMP;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


void ioTigerExpansionItem::CheckKeyInput( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() ) return;

	if( pOwner->IsJumpKey() && !pOwner->IsFloating() )
	{
		SetRunJump( pOwner, true );
		return;
	}

	if( pOwner->IsAttackKey() || pOwner->IsDefenseKey() )
	{
		ChangeToRunEnd( pOwner, pOwner->IsFloating() );
		return;
	}

	if( pOwner->IsSettedDirection() )
	{
		ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
		if( eNewDirKey != m_CurDirKey )
		{
			float fYawD = 180.0f + 45.0f * ( eNewDirKey - ioUserKeyInput::DKI_UP );
			fYawD = ioMath::ArrangeHead( fYawD );

			float fYawR = DEGtoRAD( fYawD );
			D3DXQUATERNION qtRotate;
			D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

			D3DXVECTOR3 vTargetDir = qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVECTOR3 vDir = pOwner->GetMoveDir();
			D3DXQUATERNION qtCurRot = pOwner->GetTargetRot();

			m_bTargetRot = false;
			m_bLeftRot = false;
			if( D3DXVec3Dot( &vTargetDir, &vDir ) > -FLOAT1 )
			{
				float fCurYaw, fTargetYaw;
				fCurYaw = RADtoDEG( ioMath::QuaterToYaw( qtCurRot ) );
				fTargetYaw = RADtoDEG( ioMath::QuaterToYaw( qtRotate ) );

				fCurYaw = ioMath::ArrangeHead( fCurYaw );
				fTargetYaw = ioMath::ArrangeHead( fTargetYaw );

				float fRotAngle = ioMath::AngleGap( fCurYaw, fTargetYaw );
				if( fRotAngle < 0.0f )
					m_bLeftRot = true;
			}

			m_CurDirKey = eNewDirKey;

			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
				FillItemGaugePacketHeader( pOwner, kPacket );
				kPacket << CS_ROTATE;
				kPacket << (int)m_CurDirKey;
				kPacket << m_bLeftRot;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	}
	else
	{
		if( m_CurDirKey != ioUserKeyInput::DKI_NONE )
		{
			m_CurDirKey = ioUserKeyInput::DKI_NONE;

			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
				FillItemGaugePacketHeader( pOwner, kPacket );
				kPacket << CS_ROTATE;
				kPacket << (int)m_CurDirKey;
				kPacket << m_bLeftRot;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	}

	
}


void ioTigerExpansionItem::ProcessRotate( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( m_dwRotateTime <= 0 ) return;
	if( m_bTargetRot ) return;

	if( m_CurDirKey == ioUserKeyInput::DKI_NONE )
		return;

	float fFrameGap = g_FrameTimer.GetCurFrameGap();
	float fRate = fFrameGap / m_dwRotateTime;
	float fRotateAngle = 360.0f * fRate;

	// 목표에 도달체크 및 처리
	float fYawD = 180.0f + 45.0f * ( m_CurDirKey - ioUserKeyInput::DKI_UP );
	fYawD = ioMath::ArrangeHead( fYawD );

	float fYawR = DEGtoRAD( fYawD );
	D3DXQUATERNION qtRotate;
	D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

	D3DXQUATERNION qtCurRot = pOwner->GetTargetRot();

	float fCurYaw, fTargetYaw;
	fCurYaw = RADtoDEG( ioMath::QuaterToYaw( qtCurRot ) );
	fTargetYaw = RADtoDEG( ioMath::QuaterToYaw( qtRotate ) );

	fCurYaw = ioMath::ArrangeHead( fCurYaw );
	fTargetYaw = ioMath::ArrangeHead( fTargetYaw );

	float fRotAngle = ioMath::AngleGap( fCurYaw, fTargetYaw );
	if( fabs( fRotAngle ) < fRotateAngle )
	{
		m_bTargetRot = true;
		pOwner->SetTargetRotToRotate( qtRotate, true, false );
		return;
	}

	if( m_bLeftRot )
		fRotateAngle *= -1;

	fRotateAngle = ioMath::ArrangeHead( fRotateAngle );
	D3DXVECTOR3 vDir = pOwner->GetMoveDir();

	D3DXQUATERNION qtAngle;
	D3DXQuaternionRotationYawPitchRoll( &qtAngle, DEGtoRAD(fRotateAngle), 0.0f, 0.0f );

	vDir = qtAngle * vDir;
	ioMath::CalcDirectionQuaternion( qtAngle, -vDir );

	pOwner->SetTargetRotToRotate( qtAngle, true, false );
}


void ioTigerExpansionItem::OnRunEnd( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwRunEndTime > 0 && m_dwRunEndTime < dwCurTime )
	{
		m_dwRunEndTime = 0;

		// 예약처리
		pOwner->NormalAttackOver();

		// 공격키가 눌러져 있으면 공격 상태 처리
		if( pOwner->IsAttackKeyDown() && 
			!pOwner->IsJumpKeyDown() &&
			!pOwner->IsDefenseKeyDown() &&
			!pOwner->IsChatModeState(true) )
		{
			ClearState();

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

		ClearState();
		pOwner->SetState( CS_DELAY );
		return;
	}
	else if( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
	{
		// 예약체크
		CheckReserve( pOwner );
	}
}


void ioTigerExpansionItem::OnRunCollision( ioBaseChar *pOwner )
{

}


void ioTigerExpansionItem::ChangeToAttackFire( ioBaseChar *pOwner, bool bCharged )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	m_bCharged = bCharged;

	pOwner->InitExtendAttackTagInfo();

	ClearCancelInfo();

	if( m_bCharged )
	{
		int iExtendCnt = m_vExtendAttributeList.size();
		if( COMPARE( m_iCurCombo, 0, iExtendCnt ) )
		{
			m_szCurAni = m_vExtendAttributeList[m_iCurCombo].m_AttackAnimation;
			m_fCurAniRate = m_vExtendAttributeList[m_iCurCombo].m_fAttackAniRate;

			m_dwInputCancelCheckTime = dwCurTime;
			m_dwCancelCheckTime = dwCurTime;

			pOwner->SetNormalAttackByAttribute( m_vExtendAttributeList[m_iCurCombo] );

			ioEntityGroup *pGrp = pOwner->GetGroup();
			int iAniID	     = pGrp->GetAnimationIdx( m_vExtendAttributeList[m_iCurCombo].m_AttackAnimation );
			float fTimeRate  = m_vExtendAttributeList[m_iCurCombo].m_fAttackAniRate;
			DWORD dwPreDelay = m_vExtendAttributeList[m_iCurCombo].m_dwPreDelay;

			pOwner->SetBaseAttackExtraAniJump( iAniID, fTimeRate, dwPreDelay );
			pOwner->SetExtendAttackEndJumpInfo( iAniID, fTimeRate, dwPreDelay );
		}

		if( iExtendCnt <= m_iCurCombo + 1 )
		{
			pOwner->ApplyNormalAttackResult( m_iCurCombo, true );
		}
		else
		{
			pOwner->ApplyNormalAttackResult( m_iCurCombo );
		}
	}
	else
	{
		m_bSetChargingMoveAni = false;

		const AttackAttribute *pAttribute = m_FormalAttack.GetAttribute( AT_NORMAL, m_iCurCombo );
		if( pAttribute )
		{
			pOwner->SetNormalAttackByAttribute( *pAttribute );

			m_szCurAni = pAttribute->m_AttackAnimation;
			m_fCurAniRate = pAttribute->m_fAttackAniRate;

			m_dwInputCancelCheckTime = dwCurTime;
			m_dwCancelCheckTime = dwCurTime;
		}

		pOwner->ApplyNormalAttackResult( m_iCurCombo );
	}

	m_ChargeState = CS_ATTACK_FIRE;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_iCurCombo;
		kPacket << bCharged;
		kPacket << pOwner->GetTargetRot();
		kPacket << pOwner->GetWorldPosition();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioTigerExpansionItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) 
		return;

	bool bCharged;
	rkPacket >> m_iCurCombo;
	rkPacket >> bCharged;

	D3DXQUATERNION qtRot;
	D3DXVECTOR3 vPos;
	rkPacket >> qtRot;
	rkPacket >> vPos;
	pOwner->SetTargetRot( qtRot );
	pOwner->SetWorldPosition( vPos );

	ChangeToAttackFire( pOwner, bCharged );
}


void ioTigerExpansionItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner )
		return;

	D3DXQUATERNION qtRot;
	D3DXVECTOR3 vPos;

	int iState;
	int iRunState;
	rkPacket >> iState;

	switch( iState )
	{
	case CS_GATHERING:
		{
			bool bMaxCharge;
			rkPacket >> bMaxCharge;
			if( bMaxCharge )
				SetMaxCharge( pOwner, false );
			else
				SetGatheringState( pOwner );
		}
		break;
	case CS_RUN:
		{
			rkPacket >> qtRot;
			rkPacket >> vPos;
			pOwner->SetTargetRot( qtRot );
			pOwner->SetWorldPosition( vPos );

			rkPacket >> iRunState;
			if( iRunState == RS_WAIT )
			{
				rkPacket >> m_fCurChargeRate;
				SetRunState( pOwner );
			}
			else if( iRunState == RS_RUN )
			{
				rkPacket >> m_fCurChargeRate;
				ChangeToRunning( pOwner );
			}
			else if( iRunState == RS_END )
			{
				bool bJump;
				rkPacket >> bJump;
				ChangeToRunEnd( pOwner, bJump );
			}
		}
		break;
	case CS_JUMP:
		SetRunJump( pOwner, false );
		break;
	case CS_ROTATE:
		{
			int iDir;
			rkPacket >> iDir;
			rkPacket >> m_bLeftRot;

			if( iDir == ioUserKeyInput::DKI_NONE )
			{
				m_CurDirKey = ioUserKeyInput::DKI_NONE;
			}
			else
			{
				m_bTargetRot = false;

				switch( iDir )
				{
				case ioUserKeyInput::DKI_UP:
					m_CurDirKey = ioUserKeyInput::DKI_UP;
					break;
				case ioUserKeyInput::DKI_RIGHTUP:
					m_CurDirKey = ioUserKeyInput::DKI_RIGHTUP;
					break;
				case ioUserKeyInput::DKI_RIGHT:
					m_CurDirKey = ioUserKeyInput::DKI_RIGHT;
					break;
				case ioUserKeyInput::DKI_RIGHTDOWN:
					m_CurDirKey = ioUserKeyInput::DKI_RIGHTDOWN;
					break;
				case ioUserKeyInput::DKI_DOWN:
					m_CurDirKey = ioUserKeyInput::DKI_DOWN;
					break;
				case ioUserKeyInput::DKI_LEFTDOWN:
					m_CurDirKey = ioUserKeyInput::DKI_LEFTDOWN;
					break;
				case ioUserKeyInput::DKI_LEFT:
					m_CurDirKey = ioUserKeyInput::DKI_LEFT;
					break;
				case ioUserKeyInput::DKI_LEFTUP:
					m_CurDirKey = ioUserKeyInput::DKI_LEFTUP;
					break;
				default:
					m_CurDirKey = ioUserKeyInput::DKI_NONE;
					break;
				}
			}
		}
		break;
	case CS_DEFENCE_JUMP:
		SetDefenceJump( pOwner, false );
		break;
	case CS_NET_JUMP:
		pOwner->SetExtendAttackEndJump( m_fHalfDefenceJumpPower, m_fExtendLandingRate, false, true, true );
		pOwner->ClearDirDoubleClick();
		break;
	}
	

}


void ioTigerExpansionItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
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

bool ioTigerExpansionItem::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		//if( m_ChargeState == CS_CHARGING )
			//return true;
		switch( m_ChargeState )
		{
		case CS_CHARGING:
		case CS_GATHERING:
		case CS_RUN:
			return true;
		}
	}

	return false;
}

bool ioTigerExpansionItem::IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck )
{
	if( !IsChargeDash() ) return false;
	if( !m_bCharged ) return false;

	if( pOwner->GetState() == CS_ATTACK )
	{
		switch( m_ChargeState )
		{
		case CS_ATTACK_FIRE:
			if( !bCheck )
				return false;

			return true;
		}
	}

	return false;
}

bool ioTigerExpansionItem::SetDodgeDefenseState( ioBaseChar *pOwner )
{
	if( !pOwner ) return false;
	if( !pOwner->IsNeedProcess() ) return false;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return false;

	m_dwKeyReserveTime = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

	int iAniID = -1;
	D3DXVECTOR3 vMoveDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	vMoveDir = -vMoveDir;

	if( pOwner->IsSettedDirection() )
	{
		ioUserKeyInput::DirKeyInput eDir = pOwner->CheckCurDirKey();
		if( eDir != ioUserKeyInput::DKI_NONE )
		{
			float fYawD = 180.0f + 45.0f * ( eDir - ioUserKeyInput::DKI_UP );
			fYawD = ioMath::ArrangeHead( fYawD );

			float fYawR = DEGtoRAD( fYawD );
			D3DXQUATERNION qtRotate;
			D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

			vMoveDir = qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		}
	}

	D3DXVECTOR3 vXZDir( vMoveDir.x, 0.0f, vMoveDir.z );
	D3DXVec3Normalize( &vXZDir, &vXZDir );

	D3DXQUATERNION qtTargetRot;
	ioMath::CalcDirectionQuaternion( qtTargetRot, -vXZDir );

	float fCurYaw, fTargetYaw;
	fTargetYaw = RADtoDEG( ioMath::QuaterToYaw( qtTargetRot ) );
	fTargetYaw = ioMath::ArrangeHead( fTargetYaw );
	fCurYaw = RADtoDEG( ioMath::QuaterToYaw( pOwner->GetTargetRot() ) );
	fCurYaw = ioMath::ArrangeHead( fCurYaw );

	float fAngleGap = ioMath::AngleGap( fCurYaw, fTargetYaw );
	fAngleGap += 360.0f;
	int iTest = (int)fAngleGap % 360;

	float fRate = 0.0f;
	int iIndex = -1;
	int iIndex2 = -1;
	iIndex = iTest / 90;
	iTest = iTest % 90;

	int iCnt = m_vDodgeAniList.size();
	if( COMPARE( iIndex, 0, iCnt ) )
	{
		fRate = (float)iTest /FLOAT90;
		iIndex2 = (iIndex+1) % iCnt;

		if( !m_vDodgeAniList[iIndex].IsEmpty() && !m_vDodgeAniList[iIndex2].IsEmpty() )
		{
			pGrp->ClearAllLoopAni( FLOAT100, true );
			pGrp->ClearAllActionAni( FLOAT100, true );

			if( fRate <= FLOAT05 )
			{
				iAniID = pGrp->GetAnimationIdx( m_vDodgeAniList[iIndex] );
				pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fDodgeAniRate, 0.0f, 0, true );
			}
			else
			{
				iAniID = pGrp->GetAnimationIdx( m_vDodgeAniList[iIndex2] );
				pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fDodgeAniRate, 0.0f, 0, true );
			}
		}
		else if( !m_vDodgeAniList[iIndex].IsEmpty() && m_vDodgeAniList[iIndex2].IsEmpty() )
		{
			pGrp->ClearAllLoopAni( FLOAT100, true );
			pGrp->ClearAllActionAni( FLOAT100, true );

			iAniID = pGrp->GetAnimationIdx( m_vDodgeAniList[iIndex] );
			pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fDodgeAniRate, 0.0f, 0, true );
		}
		else if( !m_vDodgeAniList[iIndex2].IsEmpty() && m_vDodgeAniList[iIndex].IsEmpty() )
		{
			pGrp->ClearAllLoopAni( FLOAT100, true );
			pGrp->ClearAllActionAni( FLOAT100, true );

			iAniID = pGrp->GetAnimationIdx( m_vDodgeAniList[iIndex2] );
			pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fDodgeAniRate, 0.0f, 0, true );
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
	//

	pOwner->SetState( CS_DODGE_DEFENSE );

	pOwner->SetForcePowerAmt( 0.0f );
	if( m_fDodgeForce > 0.0f )
		pOwner->SetForcePower( vMoveDir, fabs(m_fDodgeForce), m_fDodgeFrictionRate );
	else
		pOwner->SetForcePower( -vMoveDir, fabs(m_fDodgeForce), m_fDodgeFrictionRate );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwDodgeDefenseEnd = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * m_fDodgeAniRate;

	m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * m_fDodgeAniRate;
	if( m_dwKeyReserveTime > 0 )
	{
		m_dwKeyReserveTime += dwCurTime;
	}

	pOwner->SetEnableHide( false );
	pOwner->SetEnableChangeTeam( false );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_DODGE_DEFENSE );
		kPacket << pOwner->GetCharName();
		kPacket << pOwner->GetTargetRot();
		kPacket << vMoveDir;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	return true;
}

void ioTigerExpansionItem::ApplyDodgeDefenseState( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	D3DXVECTOR3 vMoveDir;
	rkPacket >> vMoveDir;

	m_dwKeyReserveTime = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

	int iAniID = -1;
	D3DXVECTOR3 vXZDir( vMoveDir.x, 0.0f, vMoveDir.z );
	D3DXVec3Normalize( &vXZDir, &vXZDir );

	D3DXQUATERNION qtTargetRot;
	ioMath::CalcDirectionQuaternion( qtTargetRot, -vXZDir );

	float fCurYaw, fTargetYaw;
	fTargetYaw = RADtoDEG( ioMath::QuaterToYaw( qtTargetRot ) );
	fTargetYaw = ioMath::ArrangeHead( fTargetYaw );
	fCurYaw = RADtoDEG( ioMath::QuaterToYaw( pOwner->GetTargetRot() ) );
	fCurYaw = ioMath::ArrangeHead( fCurYaw );

	float fAngleGap = ioMath::AngleGap( fCurYaw, fTargetYaw );
	fAngleGap += 360.0f;
	int iTest = (int)fAngleGap % 360;

	float fRate = 0.0f;
	int iIndex = -1;
	int iIndex2 = -1;
	iIndex = iTest / 90;
	iTest = iTest % 90;

	int iCnt = m_vDodgeAniList.size();
	if( COMPARE( iIndex, 0, iCnt ) )
	{
		fRate = (float)iTest /FLOAT90;
		iIndex2 = (iIndex+1) % iCnt;

		if( !m_vDodgeAniList[iIndex].IsEmpty() && !m_vDodgeAniList[iIndex2].IsEmpty() )
		{
			pGrp->ClearAllActionAni( FLOAT100, true );

			if( fRate <= FLOAT05 )
			{
				iAniID = pGrp->GetAnimationIdx( m_vDodgeAniList[iIndex] );
				pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fDodgeAniRate, 0.0f, 0, true );
			}
			else
			{
				iAniID = pGrp->GetAnimationIdx( m_vDodgeAniList[iIndex2] );
				pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fDodgeAniRate, 0.0f, 0, true );
			}
		}
		else if( !m_vDodgeAniList[iIndex].IsEmpty() && m_vDodgeAniList[iIndex2].IsEmpty() )
		{
			pGrp->ClearAllActionAni( FLOAT100, true );

			iAniID = pGrp->GetAnimationIdx( m_vDodgeAniList[iIndex] );
			pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fDodgeAniRate, 0.0f, 0, true );
		}
		else if( !m_vDodgeAniList[iIndex2].IsEmpty() && m_vDodgeAniList[iIndex].IsEmpty() )
		{
			pGrp->ClearAllActionAni( FLOAT100, true );

			iAniID = pGrp->GetAnimationIdx( m_vDodgeAniList[iIndex2] );
			pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fDodgeAniRate, 0.0f, 0, true );
		}
		else
		{
			return;
		}
	}
	else
	{
		return;
	}
	//

	pOwner->SetState( CS_DODGE_DEFENSE );

	pOwner->SetForcePowerAmt( 0.0f );
	if( m_fDodgeForce > 0.0f )
		pOwner->SetForcePower( vMoveDir, fabs(m_fDodgeForce), m_fDodgeFrictionRate );
	else
		pOwner->SetForcePower( -vMoveDir, fabs(m_fDodgeForce), m_fDodgeFrictionRate );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwDodgeDefenseEnd = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * m_fDodgeAniRate;

	m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * m_fDodgeAniRate;
	if( m_dwKeyReserveTime > 0 )
	{
		m_dwKeyReserveTime += dwCurTime;
	}

	pOwner->SetEnableHide( false );
	pOwner->SetEnableChangeTeam( false );
}

void ioTigerExpansionItem::CheckDodgeDefenseState( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwDodgeDefenseEnd > dwCurTime )
	{
		if( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
		{
			// 예약체크
			CheckReserve( pOwner );
		}
	}
	else
	{
		// 공격키가 눌러져 있으면 공격 상태 처리
		if( pOwner->IsAttackKeyDown() && 
			!pOwner->IsJumpKeyDown() &&
			!pOwner->IsDefenseKeyDown() &&
			!pOwner->IsChatModeState(true) )
		{
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
		if( m_iSkillInput >= 0 && !pOwner->IsBuffLimitSkill() && pOwner->SetUseSkill( m_iSkillInput, SUT_NORMAL ) )
		{
			m_iSkillInput = -1;
			return;
		}

		// 방어
		if( m_bEnableDefenseState && pOwner->IsEnableDefenseByObject() && !pOwner->IsBuffLimitDefense() )
		{
			pOwner->SetDefenseState();
			return;
		}

		// 점프
		if( m_bEnableJumpState && pOwner->IsCanJumpState() && !pOwner->IsBuffLimitJump() )
		{
			pOwner->SetJumpState();
			return;
		}

		// 공격
		if( m_bEnableAttackState )
		{
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

		// 대기
		pOwner->SetState( CS_DELAY );
	}
}

void ioTigerExpansionItem::CheckReserve( ioBaseChar *pOwner )
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

bool ioTigerExpansionItem::ProcessCancel( ioBaseChar *pOwner )
{
	if( m_szCurAni.IsEmpty() )
		return false;

	if( !pOwner ) return false;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return false;

	int iAniID = pGrp->GetAnimationIdx( m_szCurAni );

	std::list<float> vTimeList;
	pGrp->GetAnimationEventTimeList_e( iAniID, "input_jump_s", vTimeList );

	if( vTimeList.empty() )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	std::list< float >::iterator iter = vTimeList.begin();
	while( iter != vTimeList.end() )
	{
		DWORD dwTime = (DWORD)*iter;
		dwTime *= m_fCurAniRate;

		dwTime += m_dwInputCancelCheckTime;

		if( m_dwCancelCheckTime > 0 && COMPARE( dwTime, m_dwCancelCheckTime, dwCurTime ) )
		{
			m_dwCancelCheckTime = dwTime+1;

			if( m_bCharged && pOwner->IsDirKeyDoubleClick() && !pOwner->HasBuff(BT_DISABLE_DASH) )
			{
				pOwner->ChangeDirectionByInputDir( false );
				pOwner->SetDashState( false );
				pOwner->SendDashState( false );

				return true;
			}
			else if( m_bEnableJumpState && !pOwner->IsBuffLimitJump() )
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

bool ioTigerExpansionItem::SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType )
{
	if( !pOwner ) return false;

	DWORD dwCurTime = FRAMEGETTIME();
	ClearAirJump();

	AttackType eExtendType = AT_JUMP_ATTACK;

	const AttackAttribute *pAttr = GetAttackAttribute( eExtendType );
	if( !pAttr )
	{
		LOG.PrintTimeAndLog( 0, "ioTigerExpansionItem::SetJumpAttack - AttackAttribute Not Exist" );
		return false;
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	iAniID = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioTigerExpansionItem::SetJumpAttack - JumpAttack Animation Not Exist" );
		return false;
	}

	fTimeRate   = pAttr->m_fAttackAniRate;
	DWORD dwPreDelay  = pAttr->m_dwPreDelay;

	// For AirJumpPower
	SetAirJump( pOwner, iAniID, fTimeRate );

	return false;
}

void ioTigerExpansionItem::ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
	if( m_pExtendJump )
	{
		m_pExtendJump->ProcessJumpAttack( pOwner, fHeightGap );
	}

	CheckAirJump( pOwner );
}

void ioTigerExpansionItem::SetExtraGaugeTypeByWeapon( ioBaseChar *pOwner, int iType, int iWGT, float fDamage )
{
	if( !pOwner )
		return;

	if( !m_bEnableExtraGauge )
		return;

	float fTypeRate = 0.0f;
	switch( iType )
	{
	case EGT_ATTACK:
		if( iWGT == WGT_CHARGE_CONTACT )
		{
			if( m_bUsedExtraGauge )
				fTypeRate = m_fExtraGaugeAttackDec;
			else
				fTypeRate = m_fExtraGaugeAttack;
		}
		break;
	case EGT_ATTACK_DEF:
		if( iWGT == WGT_CHARGE_CONTACT )
		{
			if( m_bUsedExtraGauge )
				fTypeRate = m_fExtraGaugeAttackDefDec;
			else
				fTypeRate = m_fExtraGaugeAttackDef;
		}
		break;
	case EGT_WOUNDED:
		if( m_bUsedExtraGauge )
			fTypeRate = m_fExtraGaugeWoundDec;
		else
			fTypeRate = m_fExtraGaugeWound;
		break;
	case EGT_WOUNDED_DEF:
		if( m_bUsedExtraGauge )
			fTypeRate = m_fExtraGaugeWoundDefDec;
		else
			fTypeRate = m_fExtraGaugeWoundDef;
		break;
	}

	float fCurGauge = fTypeRate * fDamage;
	if( fTypeRate <= 0.0f )
		return;

	if( m_bUsedExtraGauge )
	{
		m_fCurExtraGauge -= fCurGauge;
		m_fCurExtraGauge = max( 0.0f, m_fCurExtraGauge );

		if( m_fCurExtraGauge <= 0.0f )
			RemoveExtraGaugeBuff( pOwner );
	}
	else
	{
		m_fCurExtraGauge += fCurGauge;
		m_fCurExtraGauge = min( m_fCurExtraGauge, m_fMaxExtraGauge );

		if( m_fCurExtraGauge >= m_fMaxExtraGauge )
			SetExtraGaugeBuff( pOwner );
	}
}

void ioTigerExpansionItem::SetUseExtraGauge( float fAmt )
{
	if( !m_bUsedExtraGauge )
		return;

	m_fCurExtraGauge -= fAmt;
	m_fCurExtraGauge = max( 0.0f, m_fCurExtraGauge );
}

int ioTigerExpansionItem::GetMaxBullet()
{
	return (int)m_fMaxExtraGauge;
}

int ioTigerExpansionItem::GetCurBullet()
{
	float fCurCoolTime = m_fCurExtraGauge;
	fCurCoolTime = max( 0, min( fCurCoolTime, m_fMaxExtraGauge ) );

	return (int)fCurCoolTime;
}

void ioTigerExpansionItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	float fCurBullet = iCurBullet;
	fCurBullet = max( 0, min( fCurBullet, m_fMaxExtraGauge ) );

	m_fCurExtraGauge = fCurBullet;
}

void ioTigerExpansionItem::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if( !m_bEnableExtraGauge )
		return;

	// check owner state
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();
	if( m_bUsedExtraGauge )
	{
		if( m_fExtraGaugeBaseDec <= 0.0f )
			return;
	}
	else
	{
		if( m_fExtraGaugeBaseInc <= 0.0f )
			return;
	}

	if( m_bUsedExtraGauge )
	{
		float fGauge = m_fExtraGaugeBaseDec * fTimePerSec;
		m_fCurExtraGauge -= fGauge;
		m_fCurExtraGauge = max( 0.0f, m_fCurExtraGauge );

		if( m_fCurExtraGauge <= 0.0f )
			RemoveExtraGaugeBuff( pOwner );
	}
	else
	{
		float fGauge = m_fExtraGaugeBaseInc * fTimePerSec;
		m_fCurExtraGauge += fGauge;
		m_fCurExtraGauge = min( m_fCurExtraGauge, m_fMaxExtraGauge );

		if( m_fCurExtraGauge >= m_fMaxExtraGauge )
			SetExtraGaugeBuff( pOwner );
	}
}

void ioTigerExpansionItem::SetExtraGaugeBuff( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if( m_ExtraGaugeBuffList.empty() )
		return;

	int iBuffCnt = m_ExtraGaugeBuffList.size();
	for( int i=0; i < iBuffCnt; ++i )
	{
		pOwner->AddNewBuff( m_ExtraGaugeBuffList[i], pOwner->GetCharName(), "", NULL );
	}

	m_bUsedExtraGauge = true;
}

void ioTigerExpansionItem::RemoveExtraGaugeBuff( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if( m_ExtraGaugeBuffList.empty() )
		return;

	int iBuffCnt = m_ExtraGaugeBuffList.size();
	for( int i=0; i < iBuffCnt; ++i )
	{
		pOwner->RemoveBuff( m_ExtraGaugeBuffList[i] );
	}

	m_bUsedExtraGauge = false;
}


void ioTigerExpansionItem::OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem )
{
	m_ChargeState = CS_NONE;
	m_RunState = RS_NONE;

	m_iMaxBullet = 100;
	m_iCurBullet = 0;

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );
	m_bSetChargingMoveAni = false;

	pOwner->ClearDirDoubleClick();

	m_fCurChargeRate = 0.0f;

	if( m_GatheringEffectID != -1 )
	{
		pOwner->EndEffect( m_GatheringEffectID, false );
		m_GatheringEffectID = -1;
	}

	if( m_GatheringMaxEffectID != -1 )
	{
		pOwner->EndEffect( m_GatheringMaxEffectID, false );
		m_GatheringMaxEffectID = -1;
	}

	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	pOwner->SetCharCollisionSkipTime( 0, 0 );
}

void ioTigerExpansionItem::CheckChargingMoveState( ioBaseChar *pOwner )
{
	/*if( !pOwner->IsNeedProcess() )
		return;*/

	if( !pOwner->IsCanAttackMove() )	
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = 0;

	if( pOwner->IsSettedDirection() )
	{
		bool bChangeDir = pOwner->IsChangeDirection();
		if( bChangeDir || m_bFirstChargeMoveMotion )
		{
			/*int iAniID = 0;
			iAniID = pGrp->GetAnimationIdx(m_GatheringMoveFr);
			pGrp->ClearLoopAni( iAniID, FLOAT100 );
			iAniID = pGrp->GetAnimationIdx(m_GatheringMoveBack);
			pGrp->ClearLoopAni( iAniID, FLOAT100 );
			iAniID = pGrp->GetAnimationIdx(m_GatheringMoveLt);
			pGrp->ClearLoopAni( iAniID, FLOAT100 );
			iAniID = pGrp->GetAnimationIdx(m_GatheringMoveRt);
			pGrp->ClearLoopAni( iAniID, FLOAT100 );*/
			
			pGrp->ClearAllActionAni( 100, true );
			pGrp->ClearAllLoopAni( 100, true );

			m_bSetChargingMoveAni = false;
			m_bFirstChargeMoveMotion = false;

			if( m_bChargeCharRotate )
			{
				pGrp->SetLoopAni( m_GatheringMoveFr, FLOAT100, FLOAT1, FLOAT1/m_fGatheringMoveAniRate );
			}
			else
			{
				ioUserKeyInput::DirKeyInput eXformDir;
				eXformDir = pOwner->GetXformDir( m_ChargeStartDirKey );

				switch( eXformDir )
				{
				case ioUserKeyInput::DKI_UP:
					pGrp->SetLoopAni( m_GatheringMoveFr, FLOAT100, FLOAT1, FLOAT1/m_fGatheringMoveAniRate );
					break;
				case ioUserKeyInput::DKI_RIGHTUP:
				case ioUserKeyInput::DKI_RIGHT:
				case ioUserKeyInput::DKI_RIGHTDOWN:
					pGrp->SetLoopAni( m_GatheringMoveRt, FLOAT100, FLOAT1, FLOAT1/m_fGatheringMoveAniRate );
					break;
				case ioUserKeyInput::DKI_DOWN:
					pGrp->SetLoopAni( m_GatheringMoveBack, FLOAT100, FLOAT1, FLOAT1/m_fGatheringMoveAniRate );
					break;
				case ioUserKeyInput::DKI_LEFTDOWN:
				case ioUserKeyInput::DKI_LEFT:
				case ioUserKeyInput::DKI_LEFTUP:
					pGrp->SetLoopAni( m_GatheringMoveLt, FLOAT100, FLOAT1, FLOAT1/m_fGatheringMoveAniRate );
					break;
				}
			}
		}
	}
	else
	{
		if( !m_bSetChargingMoveAni )
		{
			/*int iAniID = 0;
			iAniID = pGrp->GetAnimationIdx(m_GatheringMoveFr);
			pGrp->ClearLoopAni( iAniID, FLOAT100, true );
			iAniID = pGrp->GetAnimationIdx(m_GatheringMoveBack);
			pGrp->ClearLoopAni( iAniID, FLOAT100, true );
			iAniID = pGrp->GetAnimationIdx(m_GatheringMoveLt);
			pGrp->ClearLoopAni( iAniID, FLOAT100, true );
			iAniID = pGrp->GetAnimationIdx(m_GatheringMoveRt);
			pGrp->ClearLoopAni( iAniID, FLOAT100, true );*/

			pGrp->ClearAllActionAni( 100, true );
			pGrp->ClearAllLoopAni( 100, true );

			float fCurRate = FLOAT1;
			if( m_fGatherAniRate > 0.0f )
				fCurRate = m_fGatherAniRate;

			pGrp->SetLoopAni( m_GatherAnimation, FLOAT100, FLOAT1, FLOAT1/fCurRate );

			m_bSetChargingMoveAni = true;
		}
	}
}

void ioTigerExpansionItem::ClearState()
{
	m_ChargeState = CS_NONE;
	m_RunState = RS_NONE;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;
}


void ioTigerExpansionItem::SetNextItemState( ioBaseChar *pWoundChar, ioBaseChar *pAttacker, ioWeapon *pWeapon, bool bDefense, float fDamage )
{
	if( !bDefense )
		return;

	if( pWoundChar->IsNeedProcess() )
	{
		if( pWeapon->GetChangeNextItemType() == CNIT_TIGER1 )
		{
			if( !pWoundChar->IsNeedSendNetwork() )
			{
				pAttacker->SetExtendAttackEndJump( m_fHalfDefenceJumpPower, m_fExtendLandingRate, false, true, true );
				pAttacker->ClearDirDoubleClick();
				return;
			}
			else
			{
				SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
				FillItemGaugePacketHeader( pAttacker, kPacket );
				kPacket << CS_NET_JUMP;
				P2PNetwork::SendToUser( pAttacker->GetCharName(), kPacket );

				return;
			}
		}
	}

	if( pWeapon->GetChangeNextItemType() == CNIT_TIGER2 )
	{
		SetDefenceJump( pAttacker, true );
		return;
	}

}


void ioTigerExpansionItem::SetDefenceJump( ioBaseChar *pOwner, bool bSend )
{
	pOwner->SetCurMoveSpeed( 0.0f );
	pOwner->SetJumpPower( m_fGatherDefenceJumpPower );

	if( bSend )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << CS_DEFENCE_JUMP;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


bool ioTigerExpansionItem::IsNoDropState( ioBaseChar *pOwner )
{
	if( m_RunState == RS_RUN )
		return true;

	return false;
}