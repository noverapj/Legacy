

#include "stdafx.h"

#include "ioChargeContactWeaponItem.h"
#include "WeaponDefine.h"
#include "ItemDefine.h"

ioChargeContactWeaponItem::ioChargeContactWeaponItem()
{
	m_ChargeState = CS_NONE;
	m_WoundedState	= WS_DELAY;
	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_dwDodgeDefenseEnd = 0;

	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;

	m_bCharged = false;
	m_bUsedExtraGauge = false;

	m_dwMotionEndTime = 0;
	m_fCurExpansionChargeAttackEndJumpPower = 0.0f;
	m_dwEnableReaperExpansionTeleportTime = 0;
	m_dwStartWoundedTimeCheck = 0;
	m_dwInputAttackTime = 0;
	m_dwGaugeMaxEffectIndex = 0;
}

ioChargeContactWeaponItem::ioChargeContactWeaponItem( const ioChargeContactWeaponItem &rhs )
: ioWeaponItem( rhs ),
  m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
  m_vExtendAttributeList( rhs.m_vExtendAttributeList ),
  m_vDodgeAniList( rhs.m_vDodgeAniList ),
  m_fDodgeAniRate( rhs.m_fDodgeAniRate ),
  m_fDodgeForce( rhs.m_fDodgeForce ),
  m_fDodgeFrictionRate( rhs.m_fDodgeFrictionRate ),
  m_bEnableExtraGauge( rhs.m_bEnableExtraGauge ),
  m_ExtraGaugeBuffList( rhs.m_ExtraGaugeBuffList ),
  m_ExpansionChargeAttack( rhs.m_ExpansionChargeAttack ),
  m_ExpansionDashChargeAttack( rhs.m_ExpansionDashChargeAttack ),
  m_ExpansionJumpChargeAttack( rhs.m_ExpansionJumpChargeAttack ),
  m_fExpansionChargeAttackEndJumpPower( rhs.m_fExpansionChargeAttackEndJumpPower ),
  m_fExpansionDashChargeAttackEndJumpPower( rhs.m_fExpansionDashChargeAttackEndJumpPower ),
  m_fExpansionJumpChargeAttackEndJumpPower( rhs.m_fExpansionJumpChargeAttackEndJumpPower ),
  m_szExpansionReaperTeleportAni( rhs.m_szExpansionReaperTeleportAni ),
  m_fExpansionReaperTeleportAniRate( rhs.m_fExpansionReaperTeleportAniRate ),
  m_fTeleportTargetRange( rhs.m_fTeleportTargetRange ),
  m_fTeleportTargetAngle( rhs.m_fTeleportTargetAngle ),
  m_TeleportTargetWoundType( rhs.m_TeleportTargetWoundType ),
  m_vNoTargetTeleportOffset( rhs.m_vNoTargetTeleportOffset ),
  m_vTargetTeleportOffset( rhs.m_vTargetTeleportOffset ),
  m_fExpansionReaperTeleportEndJumpPower( rhs.m_fExpansionReaperTeleportEndJumpPower ),
  m_fNeedExtraGauge( rhs.m_fNeedExtraGauge),
  m_bUsingGunTypeBullet( rhs.m_bUsingGunTypeBullet ),
  m_bCheckAnotherState( rhs.m_bCheckAnotherState ),
  m_dwReaperExpansionTeleportTicTime( rhs.m_dwReaperExpansionTeleportTicTime ),
  m_WoundedType( rhs.m_WoundedType ),
  m_CancelDashAttribute( rhs.m_CancelDashAttribute ),
  m_bWoundAfterUseExtendState( rhs.m_bWoundAfterUseExtendState ),
  m_fWoundedDashMoveNeedBullet( rhs.m_fWoundedDashMoveNeedBullet ),
  m_bIgnoreChargeDashCheck( rhs.m_bIgnoreChargeDashCheck ),
  m_bInputDashRotToDirKey( rhs.m_bInputDashRotToDirKey ),
  m_dwWoundedKeyInputWaitTime( rhs.m_dwWoundedKeyInputWaitTime ),
  m_WoundedDashMove( rhs.m_WoundedDashMove ),
  m_WoundedMoveAddAttack( rhs. m_WoundedMoveAddAttack ),
  m_bWoundedDashKeyCheck( rhs.m_bWoundedDashKeyCheck ),
  m_fWoundedDashEnableHeightGap( rhs.m_fWoundedDashEnableHeightGap ),
  m_bUseDefenceCounterAttack( rhs.m_bUseDefenceCounterAttack ),
  m_bUseDefenceCounterAttackEndDash( rhs.m_bUseDefenceCounterAttackEndDash ),
  m_MidAttackType( rhs.m_MidAttackType ),
  m_dwMidChargeTime( rhs.m_dwMidChargeTime ),
  m_MidChargeAttack( rhs.m_MidChargeAttack ),
  m_szMidChargeWoundBuff( rhs.m_szMidChargeWoundBuff ),
  m_szMidChargeDefenceBuff( rhs.m_szMidChargeDefenceBuff ),
  m_bUseExtendDefence( rhs.m_bUseExtendDefence ),
  m_ItemDashType( rhs.m_ItemDashType ),
  m_HawkEyeChargeAttack( rhs.m_HawkEyeChargeAttack ),
  m_fHawkEyeEndJumpPower( rhs.m_fHawkEyeEndJumpPower ),
  m_fHawkEyeCancelJumpPower( rhs.m_fHawkEyeCancelJumpPower ),
  m_GatheringAttack( rhs.m_GatheringAttack ),
  m_GatherAnimation( rhs.m_GatherAnimation ),
  m_fGatherAniRate( rhs.m_fGatherAniRate ),
  m_dwGatheringTime( rhs.m_dwGatheringTime ),
  m_cInputAttributeS( rhs.m_cInputAttributeS ),
  m_GaugeCounterAttack( rhs.m_GaugeCounterAttack ),
  m_szGaugeMaxEffect( rhs.m_szGaugeMaxEffect ),
  m_bStaticGaugeRecovery( rhs.m_bStaticGaugeRecovery ),
  m_bNewJumpAttackColSkip( rhs.m_bNewJumpAttackColSkip )
{
	m_ChargeState = CS_NONE;
	m_WoundedState	= WS_DELAY;
	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_dwDodgeDefenseEnd = 0;

	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;

	m_bCharged = false;
	m_bUsedExtraGauge = false;

	m_dwMotionEndTime = 0;
	m_fCurExpansionChargeAttackEndJumpPower = 0.0f;
	m_dwEnableReaperExpansionTeleportTime = 0;
	m_dwStartWoundedTimeCheck = 0;
	m_dwInputAttackTime = 0;
	m_dwGaugeMaxEffectIndex = 0;
}

ioChargeContactWeaponItem::~ioChargeContactWeaponItem()
{
}

void ioChargeContactWeaponItem::LoadProperty( ioINILoader &rkLoader )
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

	m_bEnableExtraGauge = rkLoader.LoadBool_e( "extra_gauge_enable", false );
	m_bStaticGaugeRecovery = rkLoader.LoadBool_e( "static_gauge_recovery", false );
	m_bNewJumpAttackColSkip = rkLoader.LoadBool_e( "new_jump_attack_colskip", false );

	m_ExtraGaugeBuffList.clear();
	int iBuffCnt = rkLoader.LoadInt_e( "extra_gauge_buff_cnt", 0 );
	for( int i=0; i < iBuffCnt; ++i )
	{
		wsprintf_e( szKey, "extra_gauge_buff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

		m_ExtraGaugeBuffList.push_back( szBuf );
	}

	LoadAttackAttribute_e( "expansion_reaper_charge_attack", m_ExpansionChargeAttack, rkLoader );
	LoadAttackAttribute_e( "expansion_reaper_jump_charge_attack", m_ExpansionJumpChargeAttack, rkLoader );
	LoadAttackAttribute_e( "expansion_reaper_dash_charge_attack", m_ExpansionDashChargeAttack, rkLoader );

	m_fExpansionChargeAttackEndJumpPower = rkLoader.LoadFloat_e( "expansion_reaper_charge_attack_end_jump_power", 0.0f );
	m_fExpansionJumpChargeAttackEndJumpPower = rkLoader.LoadFloat_e( "expansion_reaper_jump_charge_end_jump_power", 0.0f );
	m_fExpansionDashChargeAttackEndJumpPower = rkLoader.LoadFloat_e( "expansion_reaper_dash_charge_end_jump_power", 0.0f );

	//사신 진화 텔레포트
	rkLoader.LoadString_e( "expansion_reaper_teleport_ani", "", szBuf, MAX_PATH );
	m_szExpansionReaperTeleportAni = szBuf;
	m_fExpansionReaperTeleportAniRate = rkLoader.LoadFloat_e( "expansion_reaper_teleport_ani_rate", 0.0f );

	m_fTeleportTargetAngle = rkLoader.LoadFloat_e( "teleport_target_angle", 0.0f );
	m_fTeleportTargetRange = rkLoader.LoadFloat_e( "teleport_target_range", 0.0f );
	m_TeleportTargetWoundType = (TargetWoundType)rkLoader.LoadInt_e( "teleport_target_wound_type", TWT_ALL );

	m_vNoTargetTeleportOffset.x = rkLoader.LoadFloat_e( "no_target_teleport_offset_x", 0.0f );
	m_vNoTargetTeleportOffset.y = rkLoader.LoadFloat_e( "no_target_teleport_offset_y", 0.0f );
	m_vNoTargetTeleportOffset.z = rkLoader.LoadFloat_e( "no_target_teleport_offset_z", 0.0f );

	m_vTargetTeleportOffset.x = rkLoader.LoadFloat_e( "target_teleport_offset_x", 0.0f );
	m_vTargetTeleportOffset.y = rkLoader.LoadFloat_e( "target_teleport_offset_y", 0.0f );
	m_vTargetTeleportOffset.z = rkLoader.LoadFloat_e( "target_teleport_offset_z", 0.0f );

	m_fExpansionReaperTeleportEndJumpPower = rkLoader.LoadFloat_e( "expansion_reaper_teleport_end_jump_power", 0.0f );
	m_fNeedExtraGauge = rkLoader.LoadFloat_e( "need_extra_gauge", 0.0f );
	m_bUsingGunTypeBullet = rkLoader.LoadBool_e( "using_guntype_bullet", false );

	m_bCheckAnotherState = rkLoader.LoadBool_e( "check_another_state", false );

	m_dwReaperExpansionTeleportTicTime = (DWORD)rkLoader.LoadInt_e( "reaper_teleport_tic_time", 0 );

	//태권 무도가 진화
	m_WoundedType = (WoundedType)rkLoader.LoadInt_e( "wounded_type", WT_NONE );
	LoadAttackAttribute_e( "cancel_dash", m_CancelDashAttribute, rkLoader );
	m_bWoundAfterUseExtendState = rkLoader.LoadBool_e( "wound_after_use_extend_state", false );
	m_bIgnoreChargeDashCheck = rkLoader.LoadBool_e( "ignore_charge_dash_check", false );
	m_bInputDashRotToDirKey = rkLoader.LoadBool_e( "input_dash_rot_to_dir_key", false );
	m_dwWoundedKeyInputWaitTime = rkLoader.LoadInt_e( "wounded_key_input_wait_time", 0 );
	LoadAttackAttribute_e( "wounded_dash_move", m_WoundedDashMove, rkLoader );
	LoadAttackAttribute_e( "wounded_move_add_attack", m_WoundedMoveAddAttack, rkLoader );
	m_bWoundedDashKeyCheck = rkLoader.LoadBool_e( "wounded_dash_key_check", false );
	m_fWoundedDashMoveNeedBullet = rkLoader.LoadFloat_e( "wounded_dash_move_need_bullet", 0 );
	m_fWoundedDashEnableHeightGap = rkLoader.LoadFloat_e( "wounded_dash_enable_height_gap", 0.0f );

	m_bUseDefenceCounterAttack = rkLoader.LoadBool_e( "use_defence_counter_attack", false );
	m_bUseDefenceCounterAttackEndDash = rkLoader.LoadBool_e( "use_defence_counter_attack_end_dash", false );

	//아이언 나이트 진화
	m_MidAttackType = (MidAttackType)rkLoader.LoadInt_e( "mid_attack_type", MAT_NONE );
	m_ItemDashType = (ItemDashType)rkLoader.LoadInt_e( "item_dash_type", IDT_DEFAULT );
	m_dwMidChargeTime = (DWORD)rkLoader.LoadInt_e( "mid_charge_time", 0 );
	LoadAttackAttribute_e( "mid_charge_attack", m_MidChargeAttack, rkLoader );
	rkLoader.LoadString_e( "mid_charge_wound_buff", "", szBuf, MAX_PATH );
	m_szMidChargeWoundBuff = szBuf;
	rkLoader.LoadString_e( "mid_charge_defence_buff", "", szBuf, MAX_PATH );
	m_szMidChargeDefenceBuff = szBuf;

	m_bUseExtendDefence = rkLoader.LoadBool_e( "use_extend_defence", false );
	LoadAttackAttribute_e( "hawkeye_charge_attack", m_HawkEyeChargeAttack, rkLoader );
	m_fHawkEyeEndJumpPower = rkLoader.LoadFloat_e( "hawkeye_end_jump_power", 0.0f );
	m_fHawkEyeCancelJumpPower = rkLoader.LoadFloat_e( "hawkeye_cancel_jump_power", 0.0f );

	LoadAttackAttribute_e( "gathering_attack", m_GatheringAttack, rkLoader );
	rkLoader.LoadString_e( "gathering_animation", "", szBuf, MAX_PATH );
	m_GatherAnimation = szBuf;
	m_fGatherAniRate = rkLoader.LoadFloat_e( "gathering_ani_rate", FLOAT1 );
	m_dwGatheringTime = (DWORD)rkLoader.LoadInt_e( "gathering_time", 0 );

	LoadAttackAttribute_e( "input_attack_s", m_cInputAttributeS, rkLoader );
	LoadAttackAttribute_e( "gauge_counter_attack", m_GaugeCounterAttack, rkLoader );

	rkLoader.LoadString_e( "gauge_max_effect", "", szBuf, MAX_PATH );
	m_szGaugeMaxEffect = szBuf;
}

ioItem* ioChargeContactWeaponItem::Clone()
{
	return new ioChargeContactWeaponItem( *this );
}

ioWeaponItem::WeaponSubType ioChargeContactWeaponItem::GetSubType() const
{
	return WST_CHARGE_CONTACT;
}

void ioChargeContactWeaponItem::OnReleased( ioBaseChar *pOwner )
{
	RemoveExtraGaugeBuff( pOwner );
	DestroyGaugeMaxEffect();
	m_fCurExtraGauge = 0.0f;

	ioAttackableItem::OnReleased( pOwner );
}

void ioChargeContactWeaponItem::SetNoneState( ioBaseChar* pOwner, int eNewState /*= 0 */ )
{
	m_ChargeState = CS_NONE;
	m_WoundedState	= WS_DELAY;
	m_dwAttackStartTime = 0;

	m_dwDodgeDefenseEnd = 0;

	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;

	m_bCharged = false;

	m_dwMotionEndTime = 0;
	m_fCurExpansionChargeAttackEndJumpPower = 0.0f;
	m_dwEnableReaperExpansionTeleportTime = 0;
	m_dwKeyReserveTime = 0;
	m_bDefenseEnableSet = false;
	m_dwDefenseStartTime = 0;
	m_dwDefenseEndTime = 0;
}

void ioChargeContactWeaponItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	ClearCancelInfo();

	m_bCharged = false;

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

void ioChargeContactWeaponItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	switch( m_ChargeState )
	{
	case CS_CHARGING:
		OnCharging( pOwner );
		break;
	case CS_GATHERING:
		OnGathering( pOwner );
		break;
	case CS_GATHERING_FIRE:
		if( m_dwMotionEndTime < FRAMEGETTIME() )
		{
			pOwner->SetState( CS_DELAY );
			return;
		}
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

				if ( CheckEnableReaperExpansionTeleportState( pOwner ) )
				{
					SetReaperExpansionTeleportState( pOwner );
					return;
				}

				CheckCancelReserveByConditionalSpecialDash( pOwner );
				//Enable GaugeCheck
				if( ProcessCancelBySpeicalDash( pOwner ) )
					return;
			}

			ioWeaponItem::CheckNormalAttack( pOwner );
		}
		break;
	case CS_EXPANSION_REAPER_ATTACK:
		ProcessExpansionReaperAttackState( pOwner );
		break;
	case CS_CANCEL_DASH:
		ProcessCancelDashState( pOwner );
		break;
	case CS_WOUNDED_MOVE:
		ProcessWoundedDashMoveState( pOwner );
		break;
	case CS_WOUNDED_MOVE_ADD_ATTACK:
		ProcessWoundedMoveAddAttackState( pOwner );
		break;
	case CS_MID_CHARGE_ATTACK:
		ProcessMidAttackState( pOwner );
		break;
	case CS_SALADIN_INPUT_ATTACK:
		ProcessInputAttackState( pOwner );
		break;
	}
}

void ioChargeContactWeaponItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )	return;

	if( pOwner->IsAttackKeyDown() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			if( CheckExpansionReaperAttack( pOwner, ERCAT_NORMAL ) )
				ChangeToExpansionReaperAttack( pOwner, ERCAT_NORMAL );
			else if( m_dwGatheringTime != 0 )
				ChangeToGathering( pOwner );
			else
				ChangeToAttackFire( pOwner, true );

			g_TutorialMgr.ActionChargeAttack();
		}
	}
	else
	{
		if ( CheckEnableMidAttack( pOwner ) )
			ChangeToMidAttackState( pOwner );
		else
			ChangeToAttackFire( pOwner, false );
	}
}

void ioChargeContactWeaponItem::OnGathering( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )	return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( pOwner->IsAttackKeyDown() )
	{
		if( m_dwGatheringStartTime + m_dwGatheringTime < dwCurTime )
		{
			ChangeToGatherAttackFire( pOwner );
		}
	}
	else
		ChangeToAttackFire( pOwner, true );
}

void ioChargeContactWeaponItem::ChangeToGathering( ioBaseChar *pOwner )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	float fCurRate = FLOAT1;
	if( m_fGatherAniRate > 0.0f )
		fCurRate = m_fGatherAniRate;

	pGrp->SetLoopAni( m_GatherAnimation, FLOAT100, FLOAT1, FLOAT1/fCurRate );

	m_ChargeState = CS_GATHERING;
	m_dwGatheringStartTime = FRAMEGETTIME();

	pOwner->SetAttackMoveEnable( false );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioChargeContactWeaponItem::ChangeToGatherAttackFire( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );

	m_ChargeState = CS_GATHERING_FIRE;

	if( pOwner->IsNeedProcess() )
	{
		ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
		if( eNewDirKey != ioUserKeyInput::DKI_NONE )
			pOwner->SetTargetRotToDirKey( eNewDirKey );
	}

	pOwner->SetAutoTarget( ATT_NORMAL );
	pOwner->SetNormalAttackByAttribute( m_GatheringAttack, true );

	int iAniID = pGrp->GetAnimationIdx( m_GatheringAttack.m_AttackAnimation );
	float fTimeRate = m_GatheringAttack.m_fAttackAniRate;
	DWORD dwPreDelay = m_GatheringAttack.m_dwPreDelay;

	pOwner->SetBaseAttackExtraAniJump( iAniID, fTimeRate, dwPreDelay );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime + ( pGrp->GetAnimationFullTime( iAniID ) * m_GatheringAttack.m_fAttackAniRate );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioChargeContactWeaponItem::ChangeToAttackFire( ioBaseChar *pOwner, bool bCharged )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	m_bCharged = bCharged;

	pOwner->InitExtendAttackTagInfo();

	ClearCancelInfo();

	m_dwEnableReaperExpansionTeleportTime = 0;
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

			if ( m_dwReaperExpansionTeleportTicTime > 0 )
				m_dwEnableReaperExpansionTeleportTime = dwCurTime + dwPreDelay + m_dwReaperExpansionTeleportTicTime;
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
		const AttackAttribute *pAttribute = m_FormalAttack.GetAttribute( AT_NORMAL, m_iCurCombo );
		if( pAttribute )
		{
			pOwner->SetNormalAttackByAttribute( *pAttribute );

			m_szCurAni = pAttribute->m_AttackAnimation;
			m_fCurAniRate = pAttribute->m_fAttackAniRate;

			m_dwInputCancelCheckTime = dwCurTime;
			m_dwCancelCheckTime = dwCurTime;

			ioEntityGroup *pGrp = pOwner->GetGroup();
			int iAniID	     = pGrp->GetAnimationIdx(  pAttribute->m_AttackAnimation );
			float fTimeRate  = pAttribute->m_fAttackAniRate;
			DWORD dwPreDelay = pAttribute->m_dwPreDelay;
			if ( m_dwReaperExpansionTeleportTicTime > 0 )
				m_dwEnableReaperExpansionTeleportTime = dwCurTime + dwPreDelay + m_dwReaperExpansionTeleportTicTime;
		}

		pOwner->ApplyNormalAttackResult( m_iCurCombo );
	}

	m_ChargeState = CS_ATTACK_FIRE;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << m_iCurCombo;
		kPacket << bCharged;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioChargeContactWeaponItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner )
		return;

	int iState;
	rkPacket >> iState;
	switch( iState )
	{
	case CS_ATTACK_FIRE:
		{
			bool bCharged;
			rkPacket >> m_iCurCombo;
			rkPacket >> bCharged;

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			pOwner->SetTargetRot( qtRot );

			ChangeToAttackFire( pOwner, bCharged );
		}
		break;
	case CS_EXPANSION_REAPER_ATTACK:
		{
			int iType;
			rkPacket >> iType;
			ChangeToExpansionReaperAttack( pOwner, (ExpansionReaperChargeAttackType)iType );
		}
		break;
	case CS_REAPER_EXPANSION_STATE:
		SetReaperExpansionTeleportState( pOwner );
		break;
	case CS_REAPER_EXPANSION_TELEPORT:
		{
			D3DXVECTOR3 vNewPos;
			D3DXQUATERNION qtRot;
			rkPacket >> vNewPos;
			rkPacket >> qtRot;

			ioPlayStage *pStage = pOwner->GetCreator();
			if ( !pStage )
				return;

			pOwner->SetWorldPosition( vNewPos );
			pOwner->SetTargetRotAndMoveDirChange( qtRot );
			float fMapHeight = pStage->GetMapHeightOnlyTerrain( vNewPos.x, vNewPos.z );
			if ( vNewPos.y > fMapHeight )
				pOwner->SetExtendAttackEndJump( m_fExpansionReaperTeleportEndJumpPower, FLOAT1 );
			else
				pOwner->SetState( CS_DELAY );
		}
		break;
	case CS_CANCEL_DASH:
		{
			D3DXVECTOR3 vNewPos;
			D3DXQUATERNION qtRot;
			rkPacket >> vNewPos;
			rkPacket >> qtRot;

			ioPlayStage *pStage = pOwner->GetCreator();
			if ( !pStage )
				return;

			pOwner->SetWorldPosition( vNewPos );
			pOwner->SetTargetRotAndMoveDirChange( qtRot );

			int eCurrDir, eCallerType;
			rkPacket >> eCurrDir;
			rkPacket >> eCallerType;
			SetChangeConditionalSpecialDash( pOwner, static_cast<ioUserKeyInput::DirKeyInput>( eCurrDir ), eCallerType );
		}
		break;
	case CS_WOUNDED_MOVE:
		{
			D3DXVECTOR3 vNewPos;
			D3DXQUATERNION qtRot;
			rkPacket >> vNewPos;
			rkPacket >> qtRot;

			ioPlayStage *pStage = pOwner->GetCreator();
			if ( !pStage )
				return;

			pOwner->SetWorldPosition( vNewPos );
			pOwner->SetTargetRotAndMoveDirChange( qtRot );
			ChangeWoundedDashMoveState( pOwner );
		}
		break;
	case CS_WOUNDED_MOVE_ADD_ATTACK:
		{
			D3DXVECTOR3 vNewPos;
			D3DXQUATERNION qtRot;
			rkPacket >> vNewPos;
			rkPacket >> qtRot;

			ioPlayStage *pStage = pOwner->GetCreator();
			if ( !pStage )
				return;

			pOwner->SetWorldPosition( vNewPos );
			pOwner->SetTargetRotAndMoveDirChange( qtRot );
			ChangeWoundedMoveAddAttackState( pOwner );
		}
		break;
	case CS_MID_CHARGE_ATTACK:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			pOwner->SetTargetRotToRotate( qtRot, true, false );
			ChangeToMidAttackState( pOwner );
		}
		break;
	case CS_GATHERING:
		{
			rkPacket >> m_iCurCombo;
			ChangeToGathering( pOwner );
		}
		break;
	case CS_GATHERING_FIRE:
		{
			D3DXVECTOR3 vPos;
			D3DXQUATERNION qtRot;

			rkPacket >> vPos;
			rkPacket >> qtRot;

			pOwner->SetWorldPosition( vPos );
			pOwner->SetTargetRotToRotate( qtRot, true );
			ChangeToGatherAttackFire( pOwner );
		}
		break;
	case CS_HAWKEYE_DASH_ATTACK:
		SetHawkEyeSpecialState( pOwner );
		break;
	case CS_SALADIN_INPUT_ATTACK:
		{
			D3DXVECTOR3 vPos;
			D3DXQUATERNION qtRot;
			rkPacket >> vPos;
			rkPacket >> qtRot;
			pOwner->SetWorldPosition( vPos );
			pOwner->SetTargetRotToRotate( qtRot, true );

			SetInputAttackState( pOwner, false );
		}
		break;
	}
}

void ioChargeContactWeaponItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner )
		return;

	int iState;
	rkPacket >> iState;

	if ( iState == SCT_EXTEND_DEFENSE_END )
	{
		D3DXQUATERNION qtRot;
		rkPacket >> qtRot;

		pOwner->SetTargetRotToRotate( qtRot, true, false );
		DefenseEnd( pOwner );
	}
	else if ( iState == SCT_DEFENCE_COUNTER_ATTACK )
	{
		D3DXQUATERNION qtRot;
		rkPacket >> qtRot;

		pOwner->SetTargetRotToRotate( qtRot, true, false );
		DefenceAttack( pOwner );
	}
	else if ( iState == SCT_MID_CHARGE_ADD_BUFF )
	{
		ioHashString szTarget;
		rkPacket >> szTarget;
		bool bWounded;
		rkPacket >> bWounded;
		ioBaseChar *pWoundChar = pOwner->GetBaseChar( szTarget );
		if ( !pWoundChar )
			return;
		if ( bWounded )
			pWoundChar->AddNewBuff( m_szMidChargeDefenceBuff, pOwner->GetCharName(), GetName(), NULL );
		else
			pWoundChar->AddNewBuff( m_szMidChargeWoundBuff, pOwner->GetCharName(), GetName(), NULL );
	}
	else if ( iState == SCT_CHANGE_DASH_STATE )
	{
		int eCurrDir, eCallerType;
		rkPacket >> eCurrDir;
		rkPacket >> eCallerType;
		SetChangeConditionalSpecialDash( pOwner, static_cast<ioUserKeyInput::DirKeyInput>( eCurrDir ), eCallerType );
	}
	else if( SCT_EXTRA_GAUGE_BUFF == iState )
	{
		rkPacket >> m_bUsedExtraGauge;
		if( m_bUsedExtraGauge )
			SetExtraGaugeBuff( pOwner );
		else
			RemoveExtraGaugeBuff( pOwner );
	}
	else if( SCT_GAUGE_MAX_EFFECT == iState )
		CreateGaugeMaxEffect( pOwner );
}

void ioChargeContactWeaponItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	ioAttackableItem::SetExtraInfoToWeapon( pWeapon, bJumpAttack );

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

bool ioChargeContactWeaponItem::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		if( m_ChargeState == CS_CHARGING )
			return true;
	}

	return false;
}

bool ioChargeContactWeaponItem::IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck )
{
	if( !m_bIgnoreChargeDashCheck )
	{
		if( !IsChargeDash() ) return false;
		if( !m_bCharged ) return false;
	}

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

bool ioChargeContactWeaponItem::SetDodgeDefenseState( ioBaseChar *pOwner )
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

void ioChargeContactWeaponItem::ApplyDodgeDefenseState( ioBaseChar *pOwner, SP2Packet &rkPacket )
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

void ioChargeContactWeaponItem::CheckDodgeDefenseState( ioBaseChar *pOwner )
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

void ioChargeContactWeaponItem::CheckReserve( ioBaseChar *pOwner )
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

bool ioChargeContactWeaponItem::ProcessCancel( ioBaseChar *pOwner )
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

bool ioChargeContactWeaponItem::SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType )
{
	if( !pOwner ) return false;

	DWORD dwCurTime = FRAMEGETTIME();
	ClearAirJump();

	AttackType eExtendType = AT_JUMP_ATTACK;

	const AttackAttribute *pAttr = GetAttackAttribute( eExtendType );
	if( !pAttr )
	{
		LOG.PrintTimeAndLog( 0, "ioChargeContactWeaponItem::SetJumpAttack - AttackAttribute Not Exist" );
		return false;
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	iAniID = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioChargeContactWeaponItem::SetJumpAttack - JumpAttack Animation Not Exist" );
		return false;
	}

	fTimeRate   = pAttr->m_fAttackAniRate;
	DWORD dwPreDelay  = pAttr->m_dwPreDelay;

	// For AirJumpPower
	SetAirJump( pOwner, iAniID, fTimeRate );

	return false;
}

void ioChargeContactWeaponItem::ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
	if( m_pExtendJump )
	{
		m_pExtendJump->ProcessJumpAttack( pOwner, fHeightGap );
	}

	CheckAirJump( pOwner );
}

void ioChargeContactWeaponItem::SetExtraGaugeTypeByWeapon( ioBaseChar *pOwner, int iType, int iWGT, float fDamage )
{
	if( !pOwner )
		return;

	if( !m_bEnableExtraGauge )
		return;

	float fTypeRate = 0.0f;
	switch( iType )
	{
	case EGT_ATTACK:
		if( iWGT == WGT_CHARGE_CONTACT || iWGT == WGT_SALADIN_EXPAND )
		{
			if( m_bUsedExtraGauge )
				fTypeRate = m_fExtraGaugeAttackDec;
			else
				fTypeRate = m_fExtraGaugeAttack;
		}
		break;
	case EGT_ATTACK_DEF:
		if( iWGT == WGT_CHARGE_CONTACT || iWGT == WGT_SALADIN_EXPAND )
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

	float fCurGauge = 0.f;
	if( m_bStaticGaugeRecovery )
		fCurGauge = fTypeRate;
	else
		fCurGauge = fTypeRate * fDamage;

	if( fTypeRate <= 0.0f )
		return;

	if( m_bUsedExtraGauge )
	{
		m_fCurExtraGauge -= fCurGauge;
		m_fCurExtraGauge = max( 0.0f, m_fCurExtraGauge );

		if( m_fCurExtraGauge <= 0.0f )
			RemoveExtraGaugeBuff( pOwner );
		if( m_fCurExtraGauge < m_fMaxExtraGauge )
			DestroyGaugeMaxEffect();
	}
	else
	{
		m_fCurExtraGauge += fCurGauge;
		m_fCurExtraGauge = min( m_fCurExtraGauge, m_fMaxExtraGauge );

		if( m_fCurExtraGauge >= m_fMaxExtraGauge )
		{
			SetExtraGaugeBuff( pOwner );
			CreateGaugeMaxEffect( pOwner );
		}
	}
}

void ioChargeContactWeaponItem::SetUseExtraGauge( float fAmt )
{
	if( !m_bUsedExtraGauge )
		return;

	m_fCurExtraGauge -= fAmt;
	m_fCurExtraGauge = max( 0.0f, m_fCurExtraGauge );
}

void ioChargeContactWeaponItem::DecreaseGauge( float fAmt )
{
	m_fCurExtraGauge -= fAmt;
	m_fCurExtraGauge = max( 0.0f, m_fCurExtraGauge );
}

int ioChargeContactWeaponItem::GetMaxBullet()
{
	return (int)m_fMaxExtraGauge;
}

int ioChargeContactWeaponItem::GetCurBullet()
{
	float fCurCoolTime = m_fCurExtraGauge;
	fCurCoolTime = max( 0, min( fCurCoolTime, m_fMaxExtraGauge ) );

	return (int)fCurCoolTime;
}

void ioChargeContactWeaponItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	float fCurBullet = iCurBullet;
	fCurBullet = max( 0, min( fCurBullet, m_fMaxExtraGauge ) );

	m_fCurExtraGauge = fCurBullet;
}

int ioChargeContactWeaponItem::GetNeedBullet()
{
	return m_fNeedExtraGauge;
}

bool ioChargeContactWeaponItem::UsingGunTypeBullet()
{
	return m_bUsingGunTypeBullet;
}

void ioChargeContactWeaponItem::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

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

	if( pOwner->IsSystemState() )
		return;

	if ( CheckAnotherState( pOwner ) )
		return;
	
	if( m_bUsedExtraGauge )
	{
		float fGauge = m_fExtraGaugeBaseDec * fTimePerSec;
		m_fCurExtraGauge -= fGauge;
		m_fCurExtraGauge = max( 0.0f, m_fCurExtraGauge );

		if( m_fCurExtraGauge <= 0.0f )
			RemoveExtraGaugeBuff( pOwner );
		if( m_fCurExtraGauge < m_fMaxExtraGauge )
			DestroyGaugeMaxEffect();
	}
	else
	{
		float fGauge = m_fExtraGaugeBaseInc * fTimePerSec;
		m_fCurExtraGauge += fGauge;
		m_fCurExtraGauge = min( m_fCurExtraGauge, m_fMaxExtraGauge );

		if( m_fCurExtraGauge >= m_fMaxExtraGauge )
		{
			SetExtraGaugeBuff( pOwner );
			CreateGaugeMaxEffect( pOwner );
		}
	}
}

bool ioChargeContactWeaponItem::CheckAnotherState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return false;

	if ( !m_bCheckAnotherState )
		return false;

	int iState = pOwner->GetState();
	switch( iState )
	{
	case CS_DELAY:
	case CS_DASH:
	case CS_RUN:
		return false;
	}

	return true;
}

void ioChargeContactWeaponItem::SetExtraGaugeBuff( ioBaseChar *pOwner )
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

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );		
		kPacket << SCT_EXTRA_GAUGE_BUFF;
		kPacket << m_bUsedExtraGauge;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioChargeContactWeaponItem::RemoveExtraGaugeBuff( ioBaseChar *pOwner )
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

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );		
		kPacket << SCT_EXTRA_GAUGE_BUFF;
		kPacket << m_bUsedExtraGauge;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioChargeContactWeaponItem::CheckExpansionReaperAttack( ioBaseChar *pOwner, ExpansionReaperChargeAttackType eType )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return false;

	if ( eType == ERCAT_NORMAL && !m_ExpansionChargeAttack.m_AttackAnimation.IsEmpty() )
		return true;
	else if ( eType == ERCAT_JUMP && !m_ExpansionJumpChargeAttack.m_AttackAnimation.IsEmpty() )
		return true;
	else if ( eType == ERCAT_DASH && !m_ExpansionDashChargeAttack.m_AttackAnimation.IsEmpty() )
		return true;

	return false;
}

void ioChargeContactWeaponItem::ChangeToExpansionReaperAttack( ioBaseChar *pOwner, ExpansionReaperChargeAttackType eType )
{
	if ( !pOwner )
		return;
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if ( !pGrp )
		return;

	if ( pOwner->GetState() != CS_ATTACK )
	{
		pOwner->SetState( CS_ATTACK );
		pOwner->SetCurNormalAttackItem( this );
	}

	if ( pOwner->IsNeedProcess() )
	{
		if ( pOwner->IsSettedDirection() )
			pOwner->SetTargetRotToDirKey( pOwner->CheckCurDirKey() );
		else
			pOwner->SetTargetRotToRotate( pOwner->GetTargetRot(), true );
	}

	ClearKeyReserve();
	pOwner->InitExtendAttackTagInfo();
	pOwner->InitActionStopDelay( true );
	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetCurMoveSpeed( 0.0f );

	AttackAttribute rkAttri;
	m_fCurExpansionChargeAttackEndJumpPower = 0.0f;
	if ( eType == ERCAT_NORMAL )
	{
		rkAttri = m_ExpansionChargeAttack;
		m_fCurExpansionChargeAttackEndJumpPower = m_fExpansionChargeAttackEndJumpPower;
	}
	else if ( eType == ERCAT_JUMP )
	{
		rkAttri = m_ExpansionJumpChargeAttack;
		m_fCurExpansionChargeAttackEndJumpPower = m_fExpansionJumpChargeAttackEndJumpPower;
	}
	else if ( eType == ERCAT_DASH )
	{
		rkAttri = m_ExpansionDashChargeAttack;
		m_fCurExpansionChargeAttackEndJumpPower = m_fExpansionDashChargeAttackEndJumpPower;
	}

	int iAniID = pGrp->GetAnimationIdx( rkAttri.m_AttackAnimation );
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "is not exist ani motion" );
		return;
	}

	m_ChargeState = CS_EXPANSION_REAPER_ATTACK;

	float fTimeRate = rkAttri.m_fAttackAniRate;
	if ( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwPreDelay = rkAttri.m_dwPreDelay;
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	if( !rkAttri.m_vWeaponInfoList.empty() )
	{
		pOwner->RefreshFireTimeList( iAniID,
			rkAttri.m_vWeaponInfoList,
			FTT_NORMAL_ATTACK,
			fTimeRate,
			dwPreDelay );
	}

	m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwMotionEndTime += dwPreDelay + rkAttri.m_dwEndDelay;

	// Col Skip
	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( m_dwKeyReserveTime > 0 )
		m_dwKeyReserveTime += dwPreDelay + dwCurTime;

	if ( m_dwReaperExpansionTeleportTicTime > 0 )
		m_dwEnableReaperExpansionTeleportTime = dwCurTime + dwPreDelay + m_dwReaperExpansionTeleportTicTime;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

	pOwner->SetReservedSliding( rkAttri.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	SetCurAttackAniForInputJump( rkAttri.m_AttackAnimation, fTimeRate );

	pOwner->SetAutoTarget( ATT_NORMAL );
	DWORD dwTrackingTime = dwCurTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	pOwner->SetBaseAttackExtraAniJump( iAniID, fTimeRate, rkAttri.m_dwEndDelay );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << (int)eType;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioChargeContactWeaponItem::ProcessExpansionReaperAttackState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	if ( pOwner->IsNeedProcess() )
	{
		if ( CheckEnableReaperExpansionTeleportState( pOwner ) )
		{
			SetReaperExpansionTeleportState( pOwner );
			return;
		}

		if( m_dwMotionEndTime < FRAMEGETTIME() )
		{
			pOwner->SetExtendAttackEndJump( m_fCurExpansionChargeAttackEndJumpPower, FLOAT1 );
			return;
		}
	}

	pOwner->CheckExtraAniJump();
}

bool ioChargeContactWeaponItem::CheckEnableReaperExpansionTeleportState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return false;

	if ( m_dwEnableReaperExpansionTeleportTime <= 0 )
		return false;

	if ( m_dwEnableReaperExpansionTeleportTime >= FRAMEGETTIME() )
		return false;

	if ( !pOwner->IsDefenseKey() )
		return false;

	if ( m_fCurExtraGauge < GetNeedBullet() )
		return false;

	return true;
}

void ioChargeContactWeaponItem::SetReaperExpansionTeleportState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if ( !pGrp )
		return;

	pOwner->SetState( CS_REAPER_EXPANSION );

	ClearKeyReserve();
	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetCurMoveSpeed( 0.0f );
	pOwner->SetJumpPower( 0.0f );
	pOwner->SetCurJumpPowerAmt( 0.0f);
	pOwner->SetGravityAmt( 0.0f );

	int iAniID = pGrp->GetAnimationIdx( m_szExpansionReaperTeleportAni );
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "is not exist ani motion" );
		return;
	}

	float fTimeRate = m_fExpansionReaperTeleportAniRate;
	if ( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	DWORD dwCurTime = FRAMEGETTIME();
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	// Col Skip
	pOwner->CheckCharColSkipTime( iAniID, fTimeRate );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << CS_REAPER_EXPANSION_STATE;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	m_bUsedExtraGauge = true;
	SetUseExtraGauge( m_fNeedExtraGauge );
	m_bUsedExtraGauge = false;
}

void ioChargeContactWeaponItem::ProcessSpecialState( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

	if( m_dwMotionEndTime && m_dwMotionEndTime < FRAMEGETTIME() )
	{
		m_dwMotionEndTime = 0;
		SetTeleport( pOwner );
		return;
	}
}

void ioChargeContactWeaponItem::ProcessHawkEyeSpecialState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	pOwner->CheckExtraAniJump();

	if( !pOwner->IsNeedProcess() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwMotionEndTime && m_dwMotionEndTime < FRAMEGETTIME() )
	{
		m_dwMotionEndTime = 0;
		pOwner->SetExtendAttackEndJump( m_fHawkEyeEndJumpPower, FLOAT1 );
		pOwner->ClearAttackFireTimeAndSkill();
	}
	else if( m_dwKeyReserveTime < dwCurTime && pOwner->IsJumpKey() )
	{
		pOwner->SetExtendAttackEndJump( m_fHawkEyeCancelJumpPower, FLOAT1 );
		pOwner->ClearAttackFireTimeAndSkill();
	}
}

void ioChargeContactWeaponItem::SetTeleport( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	ioHashString szTarget;
	FindAttTarget( pOwner, szTarget );
	D3DXVECTOR3 vNewPos = pOwner->GetWorldPosition() + pOwner->GetWorldOrientation() * m_vNoTargetTeleportOffset;
	ioBaseChar *pTarget = pOwner->GetBaseChar( szTarget );
	if( pTarget )
	{
		vNewPos = pTarget->GetWorldPosition() + pTarget->GetWorldOrientation() * m_vTargetTeleportOffset;
		D3DXVECTOR3 vAimedDir = pTarget->GetMidPositionByRate() - vNewPos;
		vAimedDir.y=0;
		D3DXVec3Normalize( &vAimedDir, &vAimedDir );
		pOwner->SetTargetRotToDir( vAimedDir, true );
	}

	float fMapHeight = pStage->GetMapHeightOnlyTerrain( vNewPos.x, vNewPos.z );
	if( vNewPos.y <= fMapHeight )
		vNewPos.y = fMapHeight;

	pOwner->SetWorldPosition( vNewPos );

	if( vNewPos.y > fMapHeight )
		pOwner->SetExtendAttackEndJump( m_fExpansionReaperTeleportEndJumpPower, FLOAT1 );
	else
		pOwner->SetState( CS_DELAY );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << CS_REAPER_EXPANSION_TELEPORT;
		kPacket << vNewPos;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioChargeContactWeaponItem::FindAttTarget( ioBaseChar *pOwner, ioHashString& szTarget )
{
	szTarget.Clear();
	if( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	ioFootballMode *pFootBall = ToFootballMode( pStage->GetPlayMode() );
	if( pFootBall && pFootBall->IsDisableAttackMode() )
		return;

	if( pOwner->GetExperienceMode() != EMS_NONE )
		return;

	D3DXVECTOR3 vCurPos = pOwner->GetMidPositionByRate();
	D3DXVECTOR3 vDiff= ioMath::VEC3_ZERO;

	float fCurAngle = ioMath::ArrangeHead( m_fTeleportTargetAngle );
	float fHalfValue = cosf( DEGtoRAD(fCurAngle*0.5f) );
	float fTargetRangeSq = m_fTeleportTargetRange * m_fTeleportTargetRange;

	const BaseCharList &rkCharList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;

	float fCurGap = -FLOAT1;
	for( iter=rkCharList.begin() ; iter!=rkCharList.end() ; ++iter )
	{
		ioBaseChar *pTarget = *iter;
		if( !pTarget ) continue;

		if( pTarget->GetTeam() == pOwner->GetTeam() ) continue;
		if( pTarget->GetCharName() == GetOwnerName() ) continue;
		if( pTarget->GetExperienceMode() != EMS_NONE ) continue;

		if( pTarget->IsSystemState() )
			continue;
		if( !pTarget->IsEnableTarget( false ) )
			continue;
		if( !pTarget->CheckTargetWoundType( m_TeleportTargetWoundType ) )
			continue;

		D3DXVECTOR3 vTargetPos = pTarget->GetMidPositionByRate();
		vDiff = vTargetPos - vCurPos;
		float fDiffRangeSq = D3DXVec3LengthSq( &vDiff );

		// 각도 체크
		if( fCurAngle != 0.0f )
		{
			D3DXQUATERNION qtRot = pOwner->GetTargetRot();
			D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVec3Normalize( &vDir, &vDir );
			D3DXVECTOR3 vDiff2 = vDiff;
			vDiff2.y = 0.0f;
			D3DXVec3Normalize( &vDiff2, &vDiff2 );

			float fCurValue = D3DXVec3Dot( &vDir, &vDiff2 );
			if( fCurValue < fHalfValue )
				continue;
		}
		// 거리 체크
		if( fDiffRangeSq < fTargetRangeSq )
		{
			if( fCurGap == -FLOAT1 )
			{
				fCurGap = fDiffRangeSq;
				szTarget = pTarget->GetCharName();
			}
			else if( fCurGap >= 0.0f && fDiffRangeSq < fCurGap )
			{
				fCurGap = fDiffRangeSq;
				szTarget = pTarget->GetCharName();
			}
		}
	}
}

//Dash
void ioChargeContactWeaponItem::CheckExtendDashState( ioBaseChar *pOwner )
{
	if( m_pExtendDash )
	{
		ioChargeComboDash* pChargeDash = ToChargeComboDash( m_pExtendDash );
		if( pChargeDash && IsConditionalSpecialDashReserve( pOwner ) )
			pChargeDash->CheckCancelReserveByConditionalSpecialDash( pOwner );
		m_pExtendDash->ProcessDash( pOwner );
	}
}

bool ioChargeContactWeaponItem::IsConditionalSpecialDashReserve( ioBaseChar *pOwner )
{
	switch( m_ChargeState )
	{
	case CS_CANCEL_DASH:
		return false;
	}

	return true;
}

bool ioChargeContactWeaponItem::ProcessCancelBySpeicalDash( ioBaseChar *pOwner )
{
	if( m_szCurAni.IsEmpty() )
		return false;

	if( m_dwInputCancelCheckTime == 0 )
		return false;

	if( !pOwner )
		return false;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	int iAniID = pGrp->GetAnimationIdx( m_szCurAni );			
	DWORD dwInputSpecialDashTime = (DWORD)pGrp->GetAnimationEventTime_e( iAniID, "input_dash_s" );
	if( 0 < dwInputSpecialDashTime )
	{
		dwInputSpecialDashTime *= m_fCurAniRate;
		dwInputSpecialDashTime += m_dwInputCancelCheckTime;		
	}

	if( 0 < dwInputSpecialDashTime && dwCurTime > dwInputSpecialDashTime )
	{
		if( m_bEnableDashState && IsConditionalSpecialDashReserve( pOwner ) )
		{
			SetChangeConditionalSpecialDash( pOwner, m_ConditionalSpecialDashKey, ioAttackableItem::CCSD_NORMAL );
			return true;
		}
	}

	return false;
}

void ioChargeContactWeaponItem::SetChangeConditionalSpecialDash( ioBaseChar *pOwner, ioUserKeyInput::DirKeyInput eCurrDir, int eCallerType )
{
	if ( m_ItemDashType == IDT_DEFAULT )
	{
		SetChangeConditionalDefaultSpecialDash( pOwner, eCurrDir, eCallerType );
		return;
	}
	else if ( m_ItemDashType == IDT_KNIGHT )
	{
		SetChangeConditionalKinghtSpecialDash( pOwner, eCurrDir, eCallerType );
		return;
	}
}

void ioChargeContactWeaponItem::SetChangeConditionalDefaultSpecialDash( ioBaseChar *pOwner, ioUserKeyInput::DirKeyInput eCurrDir, int eCallerType )
{
	if( !pOwner || eCurrDir == ioUserKeyInput::DKI_NONE )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	int iAniID = pGrp->GetAnimationIdx( m_CancelDashAttribute.m_AttackAnimation );
	if ( iAniID == -1 )
		return;

	DWORD dwPreDelay = m_CancelDashAttribute.m_dwPreDelay;
	DWORD dwEndDelay = m_CancelDashAttribute.m_dwEndDelay;
	DWORD dwStartTime = FRAMEGETTIME() + dwPreDelay;

	ClearCancelInfo();
	pOwner->ClearDirDoubleClick();
	pOwner->ClearReserveKeyInput();
	pOwner->ClearReservedSliding();
	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );
	pOwner->SetChargingState( false );

	if ( pOwner->GetState() != CS_ATTACK )
		pOwner->SetState( CS_ATTACK );

	pOwner->SetCurNormalAttackItem( this );

	m_ChargeState = CS_CANCEL_DASH;

	if ( pOwner->IsNeedProcess() && m_bInputDashRotToDirKey )
	{
		float fYawD = 180.0f + 45.0f * ( eCurrDir - ioUserKeyInput::DKI_UP );
		fYawD = ioMath::ArrangeHead( fYawD );

		float fYawR = DEGtoRAD( fYawD );
		D3DXQUATERNION qtRotate;
		D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

		pOwner->SetTargetRotToRotate( qtRotate, true, false );
	}

	float fTimeRate = m_CancelDashAttribute.m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pGrp->ClearAllActionAni( 0.0f, true );
	pGrp->ClearAllLoopAni( 0.0f, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

	pOwner->SetReservedSliding( m_CancelDashAttribute.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	DWORD dwCurBaseIndex = pOwner->IncreaseWeaponIndexBase();
	pOwner->RefreshFireTimeListWithBase( dwCurBaseIndex,
		iAniID,
		m_CancelDashAttribute.m_vWeaponInfoList,
		FTT_NORMAL_ATTACK,
		fTimeRate,
		dwPreDelay );

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	m_dwMotionEndTime = dwStartTime + pGrp->GetAnimationFullTime(iAniID) * fTimeRate + dwEndDelay;

	DWORD dwKeyReserve = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( dwKeyReserve > 0 )
		m_dwKeyReserveTime += dwStartTime;

	pOwner->SetAutoTarget( ATT_NORMAL );

	DWORD dwTrackingTime = dwStartTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );		
		kPacket << m_ChargeState;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		kPacket << static_cast<int>( eCurrDir );
		kPacket << eCallerType;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioChargeContactWeaponItem::SetChangeConditionalKinghtSpecialDash( ioBaseChar *pOwner, ioUserKeyInput::DirKeyInput eCurrDir, int eCallerType )
{
	if( !pOwner || eCurrDir == ioUserKeyInput::DKI_NONE )
		return;

	ClearCancelInfo();
	pOwner->ClearDirDoubleClick();
	pOwner->ClearReserveKeyInput();
	pOwner->ClearReservedSliding();
	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );
	pOwner->SetChargingState( false );
	pOwner->SetTargetRotToDirKey( eCurrDir );
	pOwner->SetDashState();

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );		
		kPacket << SCT_CHANGE_DASH_STATE;
		kPacket << static_cast<int>( eCurrDir );
		kPacket << eCallerType;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioChargeContactWeaponItem::ProcessCancelDashState( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_dwMotionEndTime < dwCurTime )
	{
		ProcessReserveKeyInput( pOwner );
		return;
	}

	if ( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
		CheckReserve( pOwner );
}

//Wounded Check
void ioChargeContactWeaponItem::CallItemProcess( ioBaseChar* pOwner )
{
	if( !pOwner->IsNeedProcess() )
		return;

	if( pOwner->GetState() != CS_WOUNDED && pOwner->GetState() != CS_BLOW_WOUNDED && pOwner->GetState() != CS_BOUND_BLOW_WOUNDED && pOwner->GetState() != CS_BLOW_EXTEND_WOUND )
	{
		m_WoundedState	= WS_DELAY;
	}
}

bool ioChargeContactWeaponItem::CheckWoundAfterUseExtendState( ioBaseChar* pOwner, BlowTargetType eType )
{
	if ( m_WoundedType == WT_NONE )
		return false;

	if( !pOwner || !pOwner->IsNeedProcess() )
		return false;

	if( pOwner->IsPrisonerMode() )
		return false;

	if( pOwner->IsCatchMode() )
		return false;

	if ( m_WoundedType == WT_FIGHTER )
		CheckWoundFighterAfter( pOwner, eType );

	return false;
}

void ioChargeContactWeaponItem::CheckWoundFighterAfter( ioBaseChar* pOwner, BlowTargetType eType )
{
	if ( pOwner->GetState() != CS_WOUNDED )
		return;

	switch( m_WoundedState )
	{
		case WS_DELAY:
			{
				m_dwStartWoundedTimeCheck = FRAMEGETTIME();
				m_WoundedState = WS_TIME_CONDITION_CHECK;
			}
			break;
		case WS_TIME_CONDITION_CHECK:
			if ( m_dwStartWoundedTimeCheck + m_dwWoundedKeyInputWaitTime > FRAMEGETTIME() )
			{
				if( IsCanWoundedDashMove( pOwner ) )
				{
					ChangeWoundedDashMoveState( pOwner );
					return;
				}
			}
			break;
	}
}

bool ioChargeContactWeaponItem::IsCanWoundedDashMove( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return false;

	if ( !pOwner->IsDirKeyDoubleClick() )
		return false;

	if ( pOwner->GetBlowWoundedState() == BW_BLOW_ENDED )
		return false;

	float fCharHeight = pOwner->GetWorldPosition().y;
	float fBottomHeight = pOwner->GetBottomHeight();

	if( pOwner->CheckGhostState() )
		fBottomHeight = pOwner->GetPreHeightForGhostState();

	if( fCharHeight - fBottomHeight > m_fWoundedDashEnableHeightGap )
		return false;

	if ( m_bEnableExtraGauge && m_fWoundedDashMoveNeedBullet > 0 )
	{
		if ( m_fCurExtraGauge < m_fWoundedDashMoveNeedBullet )
			return false;
	}

	 return true;
}


void ioChargeContactWeaponItem::ChangeWoundedDashMoveState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	int iAniID = pGrp->GetAnimationIdx( m_WoundedDashMove.m_AttackAnimation );
	if ( iAniID == -1 )
		return;

	DWORD dwPreDelay = m_WoundedDashMove.m_dwPreDelay;
	DWORD dwEndDelay = m_WoundedDashMove.m_dwEndDelay;
	DWORD dwStartTime = FRAMEGETTIME() + dwPreDelay;
	m_dwKeyReserveTime = 0;
	ClearCancelInfo();
	pOwner->ClearDirDoubleClick();
	pOwner->ClearReserveKeyInput();
	pOwner->ClearReservedSliding();
	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );
	pOwner->SetChargingState( false );

	if ( pOwner->GetState() != CS_ATTACK )
		pOwner->SetState( CS_ATTACK );

	pOwner->SetCurNormalAttackItem( this );

	m_ChargeState = CS_WOUNDED_MOVE;

	if( pOwner->IsNeedProcess() && pOwner->IsSettedDirection() )
	{
		ioUserKeyInput::DirKeyInput eDir = pOwner->CheckCurDirKey();
		if( eDir != ioUserKeyInput::DKI_NONE )
		{
			float fYawD = 180.0f + 45.0f * ( eDir - ioUserKeyInput::DKI_UP );
			fYawD = ioMath::ArrangeHead( fYawD );

			float fYawR = DEGtoRAD( fYawD );
			D3DXQUATERNION qtRotate;
			D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

			pOwner->SetTargetRotToRotate( qtRotate, true, false );
		}
	}

	float fTimeRate = m_WoundedDashMove.m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pGrp->ClearAllActionAni( 0.0f, true );
	pGrp->ClearAllLoopAni( 0.0f, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

	pOwner->SetReservedSliding( m_WoundedDashMove.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	DWORD dwCurBaseIndex = pOwner->IncreaseWeaponIndexBase();
	pOwner->RefreshFireTimeListWithBase( dwCurBaseIndex,
		iAniID,
		m_WoundedDashMove.m_vWeaponInfoList,
		FTT_NORMAL_ATTACK,
		fTimeRate,
		dwPreDelay );

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	m_dwMotionEndTime = dwStartTime + pGrp->GetAnimationFullTime(iAniID) * fTimeRate + dwEndDelay;

	DWORD dwKeyReserve = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( dwKeyReserve > 0 )
		m_dwKeyReserveTime += dwStartTime;

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );		
		kPacket << m_ChargeState;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	DecreaseGauge( m_fWoundedDashMoveNeedBullet );
}


void ioChargeContactWeaponItem::ProcessWoundedDashMoveState( ioBaseChar *pOwner )
{
	if ( !pOwner || !pOwner->IsNeedProcess() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_dwMotionEndTime < dwCurTime )
	{
		ProcessReserveKeyInput( pOwner );
		pOwner->SendCurrentPositionToAllUser();
		return;
	}
	else if ( m_dwKeyReserveTime && m_dwKeyReserveTime < dwCurTime )
	{
		if ( pOwner->IsAttackKey() )
		{
			ChangeWoundedMoveAddAttackState( pOwner );
			return;
		}
		CheckReserve( pOwner );
	}
}

void ioChargeContactWeaponItem::ChangeWoundedMoveAddAttackState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	int iAniID = pGrp->GetAnimationIdx( m_WoundedMoveAddAttack.m_AttackAnimation );
	if ( iAniID == -1 )
		return;

	DWORD dwPreDelay = m_WoundedMoveAddAttack.m_dwPreDelay;
	DWORD dwEndDelay = m_WoundedMoveAddAttack.m_dwEndDelay;
	DWORD dwStartTime = FRAMEGETTIME() + dwPreDelay;

	ClearCancelInfo();
	pOwner->ClearDirDoubleClick();
	pOwner->ClearReserveKeyInput();
	pOwner->ClearReservedSliding();
	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );
	pOwner->SetChargingState( false );
	m_dwKeyReserveTime = 0;
	m_ChargeState = CS_WOUNDED_MOVE_ADD_ATTACK;

	if( pOwner->IsNeedProcess() && pOwner->IsSettedDirection() )
	{
		ioUserKeyInput::DirKeyInput eDir = pOwner->CheckCurDirKey();
		if( eDir != ioUserKeyInput::DKI_NONE )
		{
			float fYawD = 180.0f + 45.0f * ( eDir - ioUserKeyInput::DKI_UP );
			fYawD = ioMath::ArrangeHead( fYawD );

			float fYawR = DEGtoRAD( fYawD );
			D3DXQUATERNION qtRotate;
			D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

			pOwner->SetTargetRotToRotate( qtRotate, true, false );
		}
	}

	float fTimeRate = m_WoundedMoveAddAttack.m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pGrp->ClearAllActionAni( 0.0f, true );
	pGrp->ClearAllLoopAni( 0.0f, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

	pOwner->SetReservedSliding( m_WoundedMoveAddAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	DWORD dwCurBaseIndex = pOwner->IncreaseWeaponIndexBase();
	pOwner->RefreshFireTimeListWithBase( dwCurBaseIndex,
		iAniID,
		m_WoundedMoveAddAttack.m_vWeaponInfoList,
		FTT_NORMAL_ATTACK,
		fTimeRate,
		dwPreDelay );

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	m_dwMotionEndTime = dwStartTime + pGrp->GetAnimationFullTime(iAniID) * fTimeRate + dwEndDelay;

	DWORD dwKeyReserve = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( dwKeyReserve > 0 )
		m_dwKeyReserveTime += dwStartTime;

	pOwner->SetAutoTarget( ATT_NORMAL );
	DWORD dwTrackingTime = dwStartTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );		
		kPacket << m_ChargeState;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioChargeContactWeaponItem::ProcessWoundedMoveAddAttackState( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_dwMotionEndTime < dwCurTime )
	{
		ProcessReserveKeyInput( pOwner );
		return;
	}

	if ( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
		CheckReserve( pOwner );
}

bool ioChargeContactWeaponItem::IsUseDefenceCounterAttack()
{
	return m_bUseDefenceCounterAttack;
}

bool ioChargeContactWeaponItem::IsUseDefenceCounterAttackEndDash()
{
	return m_bUseDefenceCounterAttackEndDash;
}

void ioChargeContactWeaponItem::SetDefenceCounterState( ioBaseChar *pOwner )
{
	m_bDefeceCounterAttackState = false;

	if( !pOwner ) 
		return;

	m_dwDefenceDelayEndTime = 0;

	DWORD dwCurTime = FRAMEGETTIME();

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->ClearDirDoubleClick();

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_stDefenceCounterDelayAni );
	float fTimeRate  = m_fDefenceCounterDelayAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	m_dwDefenceDelayEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
}

void ioChargeContactWeaponItem::ProcessDefenceCounterAttack( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() ) 
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( !m_bDefeceCounterAttackState )//Delay
	{
		if( dwCurTime > m_dwDefenceDelayEndTime )
		{
			pOwner->SetState( CS_DELAY, true );
			return;
		}

		if( pOwner->IsAttackKey() )
		{
			DefenceAttack( pOwner );
		}
	}
	else//Attack
	{
		if( m_dwDefenceAttackEndTime < dwCurTime )
		{
			pOwner->SetState( CS_DELAY, true );
		}
		else
		{
			CheckCancelReserveByConditionalSpecialDash( pOwner );
			//Enable GaugeCheck
			if( ProcessCancelBySpeicalDash( pOwner ) )
				return;
		}
	}
}

void ioChargeContactWeaponItem::DefenceAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	pOwner->InitExtendAttackTagInfo();
	pOwner->InitActionStopDelay( true );

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_DefenceCounterAttack.m_AttackAnimation );
	float fTimeRate  = m_DefenceCounterAttack.m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	DWORD dwPreDelay = m_DefenceCounterAttack.m_dwPreDelay;
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	DWORD dwCurBaseIndex = pOwner->IncreaseWeaponIndexBase();
	pOwner->RefreshFireTimeListWithBase( dwCurBaseIndex,
		iAniID,
		m_DefenceCounterAttack.m_vWeaponInfoList,
		FTT_NORMAL_ATTACK,
		fTimeRate,
		dwPreDelay );

	pOwner->SetReservedSliding( m_DefenceCounterAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	m_bDefeceCounterAttackState = true;

	m_dwDefenceAttackEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwInputCancelCheckTime = dwCurTime;
	if( pOwner->IsNeedProcess() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SCT_DEFENCE_COUNTER_ATTACK;
		kPacket << pOwner->GetTargetRot();;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioChargeContactWeaponItem::CheckEnableMidAttack( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return false;

	if ( !m_dwMidChargeTime )
		return false;

	if ( m_dwAttackStartTime + m_dwMidChargeTime > FRAMEGETTIME() )
		return false;

	if ( m_MidAttackType == MAT_KNIGHT )
	{
		if ( m_bEnableExtraGauge && m_fCurExtraGauge >= m_fNeedExtraGauge )
			return true;
	}

	return false;
}

void ioChargeContactWeaponItem::ChangeToMidAttackState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	if( m_MidChargeAttack.m_AttackAnimation.IsEmpty() )
		return;

	m_dwMotionEndTime = 0;

	ClearKeyReserve();

	m_ChargeState = CS_MID_CHARGE_ATTACK;

	DWORD dwCurTime = FRAMEGETTIME();
	int iAniID = pGrp->GetAnimationIdx( m_MidChargeAttack.m_AttackAnimation );
	if ( iAniID == -1 )
		return;

	float fTimeRate = m_MidChargeAttack.m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	DWORD dwPreDelay = m_MidChargeAttack.m_dwPreDelay;
	DWORD dwStartTime = dwCurTime + dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

	pOwner->SetReservedSliding( m_MidChargeAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	DWORD dwCurBaseIndex = pOwner->IncreaseWeaponIndexBase();

	pOwner->RefreshFireTimeListWithBase( dwCurBaseIndex,
		iAniID,
		m_MidChargeAttack.m_vWeaponInfoList,
		FTT_EXTEND_ATTACK,
		fTimeRate,
		dwPreDelay );

	m_dwMotionEndTime = dwStartTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwMotionEndTime += m_MidChargeAttack.m_dwEndDelay;

	m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( m_dwKeyReserveTime > 0 )
		m_dwKeyReserveTime += dwStartTime;

	SetCurAttackAniForInputJump( m_MidChargeAttack.m_AttackAnimation, fTimeRate );

	pOwner->SetAutoTarget( ATT_NORMAL );

	DWORD dwTrackingTime = dwCurTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	DecreaseGauge( m_fNeedExtraGauge );
	DestroyGaugeMaxEffect();
}

void ioChargeContactWeaponItem::ProcessMidAttackState( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

	CheckCancelReserve( pOwner );
	if( ProcessCancel(pOwner) )
		return;

	CheckCancelReserveByConditionalSpecialDash( pOwner );
	//Enable GaugeCheck
	if( ProcessCancelBySpeicalDash( pOwner ) )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_dwMotionEndTime < dwCurTime )
	{
		ProcessReserveKeyInput( pOwner );
		pOwner->SendCurrentPositionToAllUser();
		return;
	}

	if ( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
		CheckReserve( pOwner );
}

void ioChargeContactWeaponItem::ProcessInputAttackState( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return;

	CheckCancelReserve( pOwner );
	if( ProcessCancel(pOwner) )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwMotionEndTime < dwCurTime )
	{
		ProcessReserveKeyInput( pOwner );
		pOwner->SendCurrentPositionToAllUser();
		return;
	}

	if( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
		CheckReserve( pOwner );
}

void ioChargeContactWeaponItem::SetNextItemState( ioBaseChar *pWoundChar, ioBaseChar *pAttacker, ioWeapon *pWeapon, bool bDefense, float fDamage )
{
	if ( !pAttacker )
		return;

	if ( m_MidAttackType == MAT_KNIGHT && pWoundChar->IsNeedProcess() )
	{
		if( pWoundChar->IsProtectState() )
			return;

		if ( pWoundChar->HasBuff( BT_STATE_PROTECTION ) || pWoundChar->HasBuff( BT_STATE_PROTECTION_BY_PASSIVE ) )
			return;

		CharState WoundCharState = pWoundChar->GetState();
		bool bWounded = false;
		if ( WoundCharState == CS_EXTEND_DEFENSE || WoundCharState == CS_DEFENSE || WoundCharState == CS_WOUNDED_DEFENSE )
		{
			bWounded = true;
			pWoundChar->AddNewBuff( m_szMidChargeDefenceBuff, pAttacker->GetCharName(), pWeapon->GetCreateItem(), NULL );
		}
		else
			pWoundChar->AddNewBuff( m_szMidChargeWoundBuff, pAttacker->GetCharName(), pWeapon->GetCreateItem(), NULL );

		if( pWoundChar->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
			FillItemGaugePacketHeader( pAttacker, kPacket );
			kPacket << SCT_MID_CHARGE_ADD_BUFF;
			kPacket << pWoundChar->GetCharName();
			kPacket << bWounded;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

bool ioChargeContactWeaponItem::SetExtendDefenseState( ioBaseChar *pOwner )
{
	if ( !pOwner || pOwner->IsPrisonerMode() || pOwner->IsCatchMode() )
		return false;

	if ( !m_bUseExtendDefence )
		return false;

	pOwner->SetState( CS_EXTEND_DEFENSE );
	pOwner->SetChargingState( true );
	pOwner->SetDefenseMoveEnable( false, true );

	m_bDefenseEnableSet = false;
	m_dwDefenseStartTime = FRAMEGETTIME();
	m_dwDefenseEndTime = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

	return true;
}

void ioChargeContactWeaponItem::CheckExtendDefenseState( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( !m_bDefenseEnableSet && m_dwDefenseStartTime+m_dwDefensePreDuration <= dwCurTime )
	{
		m_bDefenseEnableSet = true;
		pOwner->SetDefenseEnable( true );
	}

	if( m_dwDefenseEndTime > 0 )
	{
		if( m_dwDefenseEndTime + m_dwDefenseEndDuration < dwCurTime )
		{
			if( pOwner->IsNeedProcess() )
			{
				if( m_iSkillInput >= 0 && !pOwner->IsBuffLimitSkill() && pOwner->SetUseSkill( m_iSkillInput, SUT_NORMAL ) )
					m_iSkillInput = -1;
				else if( m_bEnableDefenseState && pOwner->IsEnableDefenseByObject() && !pOwner->IsBuffLimitDefense() )
					pOwner->SetDefenseState();
				else if( m_bEnableJumpState && pOwner->IsCanJumpState() && !pOwner->IsBuffLimitJump() )
					pOwner->SetJumpState();
				else if( m_bEnableAttackState )
				{
					if( pOwner->IsCanNormalAttack() && !pOwner->IsBuffLimitAttack() )
						pOwner->SetNormalAttack( 0 );
				}
				else
					pOwner->SetState( CS_DELAY );
			}
			else
				pOwner->SetState( CS_DELAY );
		}
		else
		{
			if( pOwner->IsNeedProcess() )
			{
				bool bReserveSkill = false;
				int iSkillNum = pOwner->GetSkillKeyInput();
				if( iSkillNum >= 0 && !pOwner->IsChatModeState( true ) )
				{
					m_iSkillInput = iSkillNum;
					bReserveSkill = true;
				}

				if( pOwner->IsDefenseKeyDown() && !bReserveSkill && !m_bEnableDefenseState )
				{
					m_bEnableDefenseState = true;
					m_bEnableAttackState = false;
					m_bEnableJumpState = false;
				}
				else if( pOwner->IsAttackKey() && !bReserveSkill && !m_bEnableAttackState )
				{
					m_bEnableAttackState = true;
					m_bEnableDefenseState = false;
					m_bEnableJumpState = false;
				}
				else if( pOwner->IsJumpKey() && !bReserveSkill && !m_bEnableJumpState )
				{
					m_bEnableJumpState = true;
					m_bEnableAttackState = false;
					m_bEnableDefenseState = false;
				}
			}
		}
		return;
	}

	if( !pOwner->IsNeedProcess() ) 
		return;

	if( !m_bDefenseEnableSet )
		return;

	if( m_dwDefenseStartTime+m_dwDefenseDuration > dwCurTime )
		return;

	if( m_dwExtendDefenseWoundedTime > 0 )
	{
		if( m_dwExtendDefenseWoundedTime > dwCurTime )
			return;
		else
			m_dwExtendDefenseWoundedTime = 0;
	}

	if( !pOwner->IsDefenseKeyDown() )
		DefenseEnd( pOwner );
	else if( pOwner->IsAttackKeyDown() && pOwner->GetUseEnableSkill( 0, SUT_NORMAL ) )
	{
		int iSkillNum = pOwner->GetSkillKeyInput();
		if( iSkillNum == 0 && !pOwner->IsBuffLimitSkill() )
		{
			if( pOwner->SetUseSkill(0, SUT_NORMAL) )
				return;

			DefenseEnd( pOwner );
		}
	}
}

void ioChargeContactWeaponItem::DefenseEnd( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	pOwner->SetDefenseMoveEnable( false, false );
	pOwner->SetDefenseEnable( false );

	m_dwDefenseEndTime = FRAMEGETTIME();

	ioEntityGroup *pGrp = pOwner->GetGroup();
	int nAniID = pOwner->GetDelayAnimationIdx();
	if ( nAniID == -1 )
		return;

	pGrp->ClearAllLoopAniExceptOne( nAniID, FLOAT100 );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetLoopAni( nAniID, (float)m_dwDefenseEndDuration );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SCT_EXTEND_DEFENSE_END;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioChargeContactWeaponItem::SetHawkEyeSpecialState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;
	
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;
	
	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) 
		return;

	pOwner->ChangeDirectionByInputDir( true );
	if( pOwner->GetState() != CS_HAWKEYE_SPECIAL )
		pOwner->SetState( CS_HAWKEYE_SPECIAL );

	ClearKeyReserve();

	DWORD dwCurTime = FRAMEGETTIME();
	int iAniID = pGrp->GetAnimationIdx( m_HawkEyeChargeAttack.m_AttackAnimation );
	if ( iAniID == -1 )
		return;

	float fTimeRate = m_HawkEyeChargeAttack.m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	DWORD dwPreDelay = m_HawkEyeChargeAttack.m_dwPreDelay;
	DWORD dwStartTime = dwCurTime + dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

	pOwner->SetReservedSliding( m_HawkEyeChargeAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	DWORD dwCurBaseIndex = pOwner->IncreaseWeaponIndexBase();

	pOwner->RefreshFireTimeListWithBase( dwCurBaseIndex, iAniID, m_HawkEyeChargeAttack.m_vWeaponInfoList, FTT_EXTEND_ATTACK, fTimeRate, dwPreDelay );

	m_dwMotionEndTime = dwStartTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwMotionEndTime += m_HawkEyeChargeAttack.m_dwEndDelay;

	m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( m_dwKeyReserveTime > 0 )
		m_dwKeyReserveTime += dwStartTime;

	SetCurAttackAniForInputJump( m_HawkEyeChargeAttack.m_AttackAnimation, fTimeRate );

	pOwner->SetAutoTarget( ATT_NORMAL );

	DWORD dwTrackingTime = dwCurTime;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
	pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
	pOwner->SetBaseAttackExtraAniJump( iAniID, fTimeRate, dwPreDelay );
	pOwner->SetExtendAttackEndJumpInfo( iAniID, fTimeRate, dwPreDelay );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << CS_HAWKEYE_DASH_ATTACK;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioChargeContactWeaponItem::ClearInputAttackInfo()
{
	m_dwInputAttackTime = 0;
}

void ioChargeContactWeaponItem::SetInputAttackInfo( ioBaseChar *pOwner, int iAniID, float fTimeRate )
{
	m_dwInputAttackTime = 0;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwAniTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( dwAniTime != 0 )
		m_dwInputAttackTime = dwCurTime + dwAniTime;
}

bool ioChargeContactWeaponItem::CheckInputAttack()
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwInputAttackTime > 0 && m_dwInputAttackTime < dwCurTime && 
		!m_cInputAttributeS.m_AttackAnimation.IsEmpty() )
		return true;

	return false;
}

void ioChargeContactWeaponItem::SetInputAttackState( ioBaseChar *pOwner, bool bSendNet )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	m_dwInputAttackTime = 0;

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );
	m_ChargeState = CS_SALADIN_INPUT_ATTACK;

	AttackAttribute cInfo = m_cInputAttributeS;

	if( pOwner->IsNeedProcess() )
	{
		ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
		if( eNewDirKey != ioUserKeyInput::DKI_NONE )
			pOwner->SetTargetRotToDirKey( eNewDirKey );
	}

	DWORD dwCurTime = FRAMEGETTIME();

	m_szCurAni = cInfo.m_AttackAnimation;
	m_dwInputCancelCheckTime = dwCurTime;

	pOwner->SetAutoTarget( ATT_NORMAL );
	pOwner->SetNormalAttackByAttribute( cInfo, true );

	int iAniID = pGrp->GetAnimationIdx( cInfo.m_AttackAnimation );
	float fTimeRate = cInfo.m_fAttackAniRate;
	DWORD dwPreDelay = cInfo.m_dwPreDelay;

	pOwner->SetBaseAttackExtraAniJump( iAniID, fTimeRate, dwPreDelay );

	m_dwMotionEndTime = dwCurTime + ( pGrp->GetAnimationFullTime( iAniID ) * cInfo.m_fAttackAniRate );

	if( bSendNet && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioChargeContactWeaponItem::IsEnableGaugeCounter()
{
	if( m_GaugeCounterAttack.m_AttackAnimation.IsEmpty() )
		return false;

	if( m_bEnableExtraGauge && m_fCurExtraGauge >= m_fNeedExtraGauge )
		return true;

	return false;
}

void ioChargeContactWeaponItem::SetGaugeCounter( ioBaseChar *pOwner )
{
	pOwner->SetCounterAttackByAttribute( m_GaugeCounterAttack );

	DecreaseGauge( m_fNeedExtraGauge );
	DestroyGaugeMaxEffect();
}

void ioChargeContactWeaponItem::CreateGaugeMaxEffect( ioBaseChar *pChar )
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwGaugeMaxEffectIndex == 0 )
	{
		ioEntityGroup *pGrp = pChar->GetGroup();
		if( pGrp )
		{
			ioEffect *pEffect = pGrp->AttachEffect( m_szGaugeMaxEffect, NULL, ioMath::QUAT_IDENTITY );
			if( !pEffect )	
				return;

			m_dwGaugeMaxEffectIndex = pEffect->GetUniqueID();

			if( pChar->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
				FillItemGaugePacketHeader( pChar, kPacket );		
				kPacket << SCT_GAUGE_MAX_EFFECT;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	}
}

void ioChargeContactWeaponItem::DestroyGaugeMaxEffect()
{
	if( m_dwGaugeMaxEffectIndex != 0 )
	{
		ioEffect *pEffect = g_EffectFactory.GetEffect( m_dwGaugeMaxEffectIndex );
		if( pEffect )
			pEffect->EndEffectForce();

		m_dwGaugeMaxEffectIndex = 0;
	}
}
///////////////////////////////////////////////////////////////////////////////
ioReaperExpansionSpecialState::ioReaperExpansionSpecialState()
{
}

ioReaperExpansionSpecialState::~ioReaperExpansionSpecialState()
{
}

void ioReaperExpansionSpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioChargeContactWeaponItem *pChargeContactWeaponItem = ToChargeContactWeaponItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if ( pChargeContactWeaponItem )
	{
		pChargeContactWeaponItem->ProcessSpecialState( pOwner );
	}
	else
	{
		pOwner->SetState( CS_DELAY );
		pOwner->SetFallState( true );
		return;
	}
}

void ioReaperExpansionSpecialState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
}
///////////////////////////////////////////////////////////////////////////////
ioHawkEyeSpecialState::ioHawkEyeSpecialState()
{
}

ioHawkEyeSpecialState::~ioHawkEyeSpecialState()
{
}

void ioHawkEyeSpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioChargeContactWeaponItem *pChargeContactWeaponItem = ToChargeContactWeaponItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if ( pChargeContactWeaponItem )
	{
		pChargeContactWeaponItem->ProcessHawkEyeSpecialState( pOwner );
	}
	else
	{
		pOwner->SetState( CS_DELAY );
		pOwner->SetFallState( true );
		return;
	}
}

void ioHawkEyeSpecialState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
}

void ioHawkEyeSpecialState::ProcessKeyInput( ioBaseChar* pOwner )
{
	ioChargeContactWeaponItem* pContact = ToChargeContactWeaponItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pContact )
		pOwner->ChangeDirectionByInputDir( true );
}