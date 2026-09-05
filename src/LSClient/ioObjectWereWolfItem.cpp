

#include "stdafx.h"

#include "ioObjectWereWolfItem.h"
#include "WeaponDefine.h"
#include "ItemDefine.h"

ioObjectWereWolfItem::ioObjectWereWolfItem()
{
	m_ChargeState = CS_NONE;
	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;

	m_bCharged = false;
	m_bCheckReleaseWereWolf = false;
	m_iUseSylphidType = ST_NotUse;
	m_bUseSpeedChange = false;

	m_JumpGatheringEffectID = -1;
	m_JumpGatheringMaxEffectID = -1;
	m_bSetJumpGatheringMoveAni = false;
	m_bJumpGatherMax = false;

	m_fJumpAttackAirPowerRate = 0.0f;
	m_fDummyReleaseAirPowerRate = 0.0f;
}

ioObjectWereWolfItem::ioObjectWereWolfItem( const ioObjectWereWolfItem &rhs )
: ioObjectItem( rhs ),
  m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
  m_vExtendAttributeList( rhs.m_vExtendAttributeList ),
  m_vChargeComboAttList( rhs.m_vChargeComboAttList ),
  m_GatherAnimation( rhs.m_GatherAnimation ),
  m_fGatherAniRate( rhs.m_fGatherAniRate ),
  m_bEnableMaxCharge( rhs.m_bEnableMaxCharge ),
  m_WolfDefenseType( rhs.m_WolfDefenseType ),
  m_szLandAreaWeapon( rhs.m_szLandAreaWeapon ),
  m_szSpecialAttackAreaWeapon( rhs.m_szSpecialAttackAreaWeapon ),
  m_bEnableSylphid( rhs.m_bEnableSylphid ),
  m_SylphidInfo( rhs.m_SylphidInfo ),
  m_bEnableSpeedChange( rhs.m_bEnableSpeedChange ),
  m_bEnableDefenseRelease( rhs.m_bEnableDefenseRelease ),
  m_bEnableJumpCharge( rhs.m_bEnableJumpCharge ),
  m_fChangeSpeed( rhs.m_fChangeSpeed ),
  m_dwJumpChargeTime( rhs.m_dwJumpChargeTime ),
  m_dwMaxComboDuration( rhs.m_dwMaxComboDuration ),
  m_JumpGatherAnimation( rhs.m_JumpGatherAnimation ),
  m_fJumpGatherAniRate( rhs.m_fJumpGatherAniRate ),
  m_JumpGatheringEffect( rhs.m_JumpGatheringEffect ),
  m_JumpGatheringMaxEffect( rhs.m_JumpGatheringMaxEffect ),
  m_JumpGatheringMaxSound( rhs.m_JumpGatheringMaxSound ),
  m_JumpGatheringMoveAnimation( rhs.m_JumpGatheringMoveAnimation ),
  m_dwJumpGatheringMaxTime( rhs.m_dwJumpGatheringMaxTime ),
  m_dwJumpChargeAttackLoopTime( rhs.m_dwJumpChargeAttackLoopTime ),
  m_dwJumpChargeAttackLoopRateTime( rhs.m_dwJumpChargeAttackLoopRateTime ),
  m_fJumpChargeRunSpeed( rhs.m_fJumpChargeRunSpeed ),
  m_dwRotateTime( rhs.m_dwRotateTime ),
  m_JumpChargeActionBuffList( rhs.m_JumpChargeActionBuffList ),
  m_JumpChargeAttack( rhs.m_JumpChargeAttack ),
  m_JumpChargeMaxAttack( rhs.m_JumpChargeMaxAttack ),
  m_JumpChargeEndAttack( rhs.m_JumpChargeEndAttack ),
  m_JumpChargeMaxEndAttack( rhs.m_JumpChargeMaxEndAttack ),
  m_JumpChargeEnd( rhs.m_JumpChargeEnd ),
  m_fJumpAttackAirPower( rhs.m_fJumpAttackAirPower ),
  m_fJumpAttackAirForcePower( rhs.m_fJumpAttackAirForcePower ),
  m_fDummyReleaseAirPower( rhs.m_fDummyReleaseAirPower ),
  m_fDummyReleaseAirForcePower( rhs.m_fDummyReleaseAirForcePower )
{
	m_ChargeState = CS_NONE;
	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;

	m_bCharged = false;
	m_bCheckReleaseWereWolf = false;
	m_iUseSylphidType = ST_NotUse;
	m_bUseSpeedChange = false;

	m_JumpGatheringEffectID = -1;
	m_JumpGatheringMaxEffectID = -1;
	m_bSetJumpGatheringMoveAni = false;
	m_bJumpGatherMax = false;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_bTargetRot = false;
	m_bLeftRot = false;

	m_fJumpAttackAirPowerRate = 0.0f;
	m_fDummyReleaseAirPowerRate = 0.0f;
}

ioObjectWereWolfItem::~ioObjectWereWolfItem()
{
}

void ioObjectWereWolfItem::LoadProperty( ioINILoader &rkLoader )
{
	ioObjectItem::LoadProperty( rkLoader );

	int i;
	char szBuf[MAX_PATH];
	char szKey[MAX_PATH] = "";

	int iExtendCnt = rkLoader.LoadInt_e( "extend_attack_cnt", 0 );
	int iComboMax = rkLoader.LoadInt_e( "charge_combo_max_cnt", 0 );
	m_vExtendAttributeList.clear();
	m_vExtendAttributeList.reserve( iExtendCnt );
	for( i=0 ; i<iExtendCnt ; i++ )
	{
		AttackAttribute kAttribute;
		wsprintf_e( szBuf, "extend_attack0%d", i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader );
		m_vExtendAttributeList.push_back( kAttribute );
	}

	int iReadyCnt = max( iExtendCnt, iComboMax );
	for( i=0 ; i<iReadyCnt ; i++ )
	{
		wsprintf_e( szBuf, "attack_ready_animation%d", i+1 );
		rkLoader.LoadString( szBuf, "", szBuf, MAX_PATH );
		m_AttackReadyAniList.push_back( ioHashString(szBuf) );
	}

	m_vChargeComboAttList.clear();
	m_vChargeComboAttList.reserve( iComboMax );
	for( i=0; i < iComboMax; ++i )
	{
		AttackAttribute kAttribute;
		wsprintf_e( szBuf, "charge_combo_attack%d", i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader );
		m_vChargeComboAttList.push_back( kAttribute );
	}
	m_dwMaxComboDuration = (DWORD)rkLoader.LoadInt_e( "charge_combo_max_time", 1 );

	m_bEnableMaxCharge = rkLoader.LoadBool_e( "enable_max_charge", false );

	rkLoader.LoadString_e( "gathering_animation", "", szBuf, MAX_PATH );
	m_GatherAnimation = szBuf;
	m_fGatherAniRate = rkLoader.LoadFloat_e( "gathering_ani_rate", FLOAT1 );

	m_WolfDefenseType = (WolfDefenseType)rkLoader.LoadInt_e( "wolf_defense_type", WDT_NORMAL );

	m_szLandAreaWeapon.m_iWeaponIdx = rkLoader.LoadInt_e( "land_areaweapon", NO_WEAPON_ATTRIBUTE );
	m_szLandAreaWeapon.m_iResistanceIdx = rkLoader.LoadInt_e( "land_areaweapon_resist", NO_WEAPON_ATTRIBUTE );
	rkLoader.LoadString_e( "land_areaweapon_wounded", "", szBuf, MAX_PATH );
	m_szLandAreaWeapon.m_WoundedAnimation = szBuf;
	m_szLandAreaWeapon.m_dwWoundedDuration = rkLoader.LoadInt_e( "land_areaweapon_wounded_time", 0 );
	m_szLandAreaWeapon.m_bWoundedLoopAni = rkLoader.LoadBool_e( "land_areaweapon_wounded_loop_ani", false );

	m_szSpecialAttackAreaWeapon.m_iWeaponIdx = rkLoader.LoadInt_e( "special_attack_areaweapon", NO_WEAPON_ATTRIBUTE );
	m_szSpecialAttackAreaWeapon.m_iResistanceIdx = rkLoader.LoadInt_e( "special_attack_areaweapon_resist", NO_WEAPON_ATTRIBUTE );
	rkLoader.LoadString_e( "special_attack_areaweapon_wounded", "", szBuf, MAX_PATH );
	m_szSpecialAttackAreaWeapon.m_WoundedAnimation = szBuf;
	m_szSpecialAttackAreaWeapon.m_dwWoundedDuration = rkLoader.LoadInt_e( "special_attack_areaweapon_wounded_time", 0 );
	m_szSpecialAttackAreaWeapon.m_bWoundedLoopAni = rkLoader.LoadBool_e( "special_attack_areaweapon_wounded_loop_ani", false );

	m_dwJumpChargeTime = rkLoader.LoadInt_e( "jump_charge_time", 0 );

	LoadSylphidInfo( rkLoader );

	rkLoader.LoadString_e( "jump_gathering_animation", "", szBuf, MAX_PATH );
	m_JumpGatherAnimation = szBuf;
	rkLoader.LoadString_e( "jump_gathering_effect", "", szBuf, MAX_PATH );
    m_JumpGatheringEffect = szBuf;
	rkLoader.LoadString_e( "jump_gathering_max_effect", "", szBuf, MAX_PATH );
	m_JumpGatheringMaxEffect = szBuf;
	rkLoader.LoadString_e( "jump_gathering_max_sound", "", szBuf, MAX_PATH );
	m_JumpGatheringMaxSound = szBuf;
	rkLoader.LoadString_e( "jump_gathering_move_ani", "", szBuf, MAX_PATH );
	m_JumpGatheringMoveAnimation = szBuf;
	m_fJumpGatherAniRate = rkLoader.LoadFloat_e( "jump_gathering_ani_rate", FLOAT1 );
	m_dwJumpGatheringMaxTime = rkLoader.LoadInt_e( "jump_gathering_max_time", 0 );

	m_dwJumpChargeAttackLoopTime = rkLoader.LoadInt_e( "jump_charge_attack_loop_time", 0 );
	m_dwJumpChargeAttackLoopRateTime = rkLoader.LoadInt_e( "jump_charge_attack_loop_rate_time", 0 );
	m_fJumpChargeRunSpeed = rkLoader.LoadFloat_e( "jump_charge_attack_run_speed", 0 );
	m_dwRotateTime = (DWORD)rkLoader.LoadInt_e( "jump_charge_attack_run_rotate_speed", 0 );
	m_JumpChargeActionBuffList.clear();
	int iCnt = rkLoader.LoadInt_e( "jump_charge_action_buff_list", 0 );
	for( i = 0; i < iCnt; ++i )
	{
		wsprintf_e( szKey, "jump_charge_action_buff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

		m_JumpChargeActionBuffList.push_back( szBuf );
	}
	LoadAttackAttribute( "jump_charge_attack", m_JumpChargeAttack, rkLoader );
	LoadAttackAttribute( "jump_charge_max_attack", m_JumpChargeMaxAttack, rkLoader );
	LoadAttackAttribute( "jump_charge_end_attack", m_JumpChargeEndAttack, rkLoader );
	LoadAttackAttribute( "jump_charge_max_end_attack", m_JumpChargeMaxEndAttack, rkLoader );
	LoadAttackAttribute( "jump_charge_end", m_JumpChargeEnd, rkLoader );
}

void ioObjectWereWolfItem::LoadSylphidInfo( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH] = "";

	m_bEnableSylphid = rkLoader.LoadBool_e( "enable_sylphid", false );
	m_bEnableSpeedChange = rkLoader.LoadBool_e( "enable_speed_change", false );
	m_bEnableDefenseRelease = rkLoader.LoadBool_e( "enable_defense_release", false );
	m_bEnableJumpCharge = rkLoader.LoadBool_e( "enable_jump_charge", false );
	m_fChangeSpeed = rkLoader.LoadFloat_e( "change_speed", 0.f );

	m_SylphidInfo.m_dwDefaultNeed = (DWORD)rkLoader.LoadInt_e( "sylphid_default_need", 0 );
	rkLoader.LoadString_e( "sylphid_default_effect", "", szBuf, MAX_PATH );
	m_SylphidInfo.m_DefaultEffect = szBuf;

	m_SylphidInfo.m_fUpJumpPowerAmt = rkLoader.LoadFloat_e( "sylphid_up_jump_power", 0.0f );
	m_SylphidInfo.m_fUpJumpSpeedRate = rkLoader.LoadFloat_e( "sylphid_up_jump_speed_rate", FLOAT1 );
	m_SylphidInfo.m_dwUpJumpNeed = (DWORD)rkLoader.LoadInt_e( "sylphid_up_jump_need", 0 );
	rkLoader.LoadString_e( "sylphid_up_jump_effect", "", szBuf, MAX_PATH );
	m_SylphidInfo.m_UpJumpEffect = szBuf;

	m_SylphidInfo.m_fDownJumpGrabityRate = rkLoader.LoadFloat_e( "sylphid_down_jump_gravity_rate", FLOAT1 );
	m_SylphidInfo.m_fDownJumpSpeedRate = rkLoader.LoadFloat_e( "sylphid_down_jump_speed_rate", FLOAT1 );
	m_SylphidInfo.m_dwDownJumpNeed = (DWORD)rkLoader.LoadInt_e( "sylphid_down_jump_need", 0 );
	rkLoader.LoadString_e( "sylphid_down_jump_effect", "", szBuf, MAX_PATH );
	m_SylphidInfo.m_DownJumpEffect = szBuf;
}

ioItem* ioObjectWereWolfItem::Clone()
{
	return new ioObjectWereWolfItem( *this );
}

ioObjectItem::ObjectSubType ioObjectWereWolfItem::GetObjectSubType() const
{
	return OST_WEREWOLF;
}

void ioObjectWereWolfItem::OnReleased( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	if( pOwner && !m_CurItemEquipEffect.IsEmpty() )
		pOwner->EndEffect( m_CurItemEquipEffect, false );
	
	ioWereWolfItem *pWereWolfItem = ToWereWolfItem( pOwner->GetWeapon() );
	if ( pWereWolfItem )
		pWereWolfItem->ioAttackableItem::ReleseWereWolfState( pOwner );

	ioBubbleItem *pBubble = ToBubbleItem( pOwner->GetWeapon() );
	if ( pBubble )
		pBubble->ioAttackableItem::ReleseWereWolfState( pOwner );

	ioChangeOnItem* pChangeOn = ToChangeOnItem( pOwner->GetWeapon() );
	if ( pChangeOn )
		pChangeOn->ioAttackableItem::ReleseWereWolfState( pOwner );

	ioDrSpiderItem* pDrSpider = ToDrSpiderItem( pOwner->GetWeapon() );
	if ( pDrSpider )
		pDrSpider->ioAttackableItem::ReleseWereWolfState( pOwner );

	if( m_JumpGatheringEffectID != -1 )
	{
		pOwner->EndEffect( m_JumpGatheringEffectID, false );
		m_JumpGatheringEffectID = -1;
	}
	if( m_JumpGatheringMaxEffectID != -1 )
	{
		pOwner->EndEffect( m_JumpGatheringMaxEffectID, false );
		m_JumpGatheringMaxEffectID = -1;
	}

	int iBuffCnt = m_JumpChargeActionBuffList.size();
	for( int i=0; i < iBuffCnt; ++i )
		pOwner->RemoveBuff( m_JumpChargeActionBuffList[i] );

	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetCurMoveSpeed( 0.f );
}

void ioObjectWereWolfItem::OnEquiped( ioBaseChar *pOwner )
{
	ioObjectItem::OnEquiped( pOwner );

	ioChangeOnItem* pChangeOn = ToChangeOnItem( pOwner->GetWeapon() );
	if ( pChangeOn )
	{
		ioChangeOnItem::WerewolfChangeState eState = pChangeOn->GetWolfChangeState();

		if( eState == ioChangeOnItem::WSC_DELAY )
			ChangeToComboState( pOwner );
		else if( eState == ioChangeOnItem::WSC_DASH )
			ChangeToAttackFire( pOwner, false );
	}
}

void ioObjectWereWolfItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	ClearCancelInfo();

	m_bCharged = false;
	m_bCheckReleaseWereWolf = false;
	m_bSetJumpGatheringMoveAni = false;
	m_bJumpGatherMax = false;
	m_iUseSylphidType = ST_NotUse;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	if( m_JumpGatheringEffectID != -1 )
	{
		pOwner->EndEffect( m_JumpGatheringEffectID, false );
		m_JumpGatheringEffectID = -1;
	}
	if( m_JumpGatheringMaxEffectID != -1 )
	{
		pOwner->EndEffect( m_JumpGatheringMaxEffectID, false );
		m_JumpGatheringMaxEffectID = -1;
	}

	int iExtendCnt = m_vExtendAttributeList.size();
	if( iExtendCnt == 0 )
		iExtendCnt = m_vChargeComboAttList.size();

	if( iExtendCnt == 0 || !COMPARE( iCurCombo, 0, iExtendCnt ) )
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

void ioObjectWereWolfItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_ChargeState )
	{
	case CS_CHARGING:
		OnCharging( pOwner );
		break;
	case CS_GATHERING:
		OnGathering( pOwner );
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
				if( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
				{
					if( IsEnableDefenseRelease() && pOwner->IsDefenseKey() )
						ChangeToRelease( pOwner );
				}
			}

			ioWeaponItem::CheckNormalAttack( pOwner );
		}
		break;
	case CS_COMBO:
		{
			pOwner->CheckExtraAniJump();

			if( pOwner->IsNeedProcess() )
			{
				if( pOwner->CheckExtendAttackEndJump() )
					return;

				CheckCancelReserve( pOwner );

				if( ProcessCancel(pOwner) )
					return;

				OnComboState( pOwner );
			}

			if( pOwner && pOwner->GetState() == CS_ATTACK )
			{
				ioWeaponItem::CheckNormalAttack( pOwner );
			}
		}
		break;
	case CS_JUMP_CHARGING:
		SetJumpGatheringState( pOwner );
		break;
	case CS_JUMP_GATHERING:
		OnJumpGathering( pOwner );
		CheckJumpGatheringMoveState( pOwner );
		break;
	case CS_JUMP_ATTACK:
		if( m_dwJumpChargeEndTime < dwCurTime )
		{
			SetJumpChargeEndAttackState( pOwner, false );
			return;
		}
		else
		{
			if( pOwner->IsAttackKey() && pOwner->IsNeedProcess() )
			{
				SetJumpChargeEndAttackState( pOwner, true );
				return;
			}
			else if( pOwner->IsDefenseKey() && pOwner->IsNeedProcess() )
			{
				SetJumpChargeEndAttackState( pOwner, false );
				return;
			}

			ioPlayStage *pStage = g_ItemMaker.GetPlayStage();
			if( pOwner->IsNeedProcess() && pStage )
				pStage->SetTargetLookAt( true );

			pOwner->SetCurMoveSpeed( m_fJumpChargeRunSpeed );
			CheckKeyInput( pOwner );
			ProcessRotate( pOwner );
		}
		break;
	case CS_JUMP_END_ATTACK:
		if( m_dwJumpChargeEndTime < dwCurTime )
			ioWeaponItem::CheckNormalAttack( pOwner );
		break;
	}
}

void ioObjectWereWolfItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )	return;

	if( pOwner->IsAttackKeyDown() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			switch( m_ExtendChargeType )
			{
			case ECT_WEREWOLF:
				ChangeToGathering( pOwner );
				break;
			case ECT_RIDER:
			case ECT_GRIFFIN:
			case ECT_BUBBLE:
				ChangeToRelease( pOwner );
				break;
			case ECT_CHANGE_ON:
				ChangeToComboState( pOwner );
				break;
			default:
				ChangeToAttackFire( pOwner, false );
				break;
			}
		}
	}
	else
	{
		ChangeToAttackFire( pOwner, false );
	}
}

void ioObjectWereWolfItem::ChangeToAttackFire( ioBaseChar *pOwner, bool bCharged )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	m_bCharged = bCharged;
	ClearCancelInfo();

	pOwner->InitExtendAttackTagInfo();
	if( pOwner->GetState() != CS_ATTACK )
	{
		pOwner->SetCurNormalAttackItem( this );
		pOwner->SetState( CS_ATTACK );
	}

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

			m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate + dwPreDelay;
			if( m_dwKeyReserveTime > 0 )
				m_dwKeyReserveTime += dwCurTime;
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
		}

		pOwner->ApplyNormalAttackResult( m_iCurCombo );

		ioEntityGroup *pGrp = pOwner->GetGroup();
		int iAniID	     = pGrp->GetAnimationIdx( pAttribute->m_AttackAnimation );
		float fTimeRate  = pAttribute->m_fAttackAniRate;
		DWORD dwPreDelay = pAttribute->m_dwPreDelay;

		m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate + dwPreDelay;
		if( m_dwKeyReserveTime > 0 )
			m_dwKeyReserveTime += dwCurTime;
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

void ioObjectWereWolfItem::ChangeToJumpChargeAttackFire( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	if( m_JumpGatheringEffectID != -1 )
	{
		pOwner->EndEffect( m_JumpGatheringEffectID, false );
		m_JumpGatheringEffectID = -1;
	}
	if( m_JumpGatheringMaxEffectID != -1 )
	{
		pOwner->EndEffect( m_JumpGatheringMaxEffectID, false );
		m_JumpGatheringMaxEffectID = -1;
	}	

	AttackAttribute cInfo;
	if( m_bJumpGatherMax && !m_JumpChargeMaxAttack.m_AttackAnimation.IsEmpty() )
		cInfo = m_JumpChargeMaxAttack;
	else
		cInfo = m_JumpChargeAttack;

	int iAniID = pGrp->GetAnimationIdx( cInfo.m_AttackAnimation );
	if( iAniID == -1 )	return;

	int iCnt = m_JumpChargeActionBuffList.size();
	for( int i=0; i < iCnt; ++i )
		pOwner->AddNewBuff( m_JumpChargeActionBuffList[i], pOwner->GetCharName(), "", NULL );

	float fTimeRate  = cInfo.m_fAttackAniRate;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	pOwner->RefreshFireTimeList( iAniID, cInfo.m_vWeaponInfoList, FTT_EXTEND_ATTACK, fTimeRate, 0 );
	
	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwGapTime = dwCurTime - m_dwGatheringStartTime;
	float fCurChargeRate = 0.0f;
	if( m_dwGatheringStartTime > 0 && m_dwJumpGatheringMaxTime > 0 )
	{
		if( m_dwJumpGatheringMaxTime > dwGapTime )
			fCurChargeRate = (float)dwGapTime / m_dwJumpGatheringMaxTime;
		else
			fCurChargeRate = FLOAT1;
	}
	m_fJumpChargeRate = fCurChargeRate;

	DWORD dwLoopTime = m_dwJumpChargeAttackLoopTime + m_dwJumpChargeAttackLoopRateTime * m_fJumpChargeRate;
	m_dwJumpChargeEndTime = dwCurTime + dwLoopTime;

	pOwner->SetCharColSkipSkillTime( dwCurTime, m_dwJumpChargeEndTime );
	pOwner->SetTargetRotToRotate( pOwner->GetTargetRot(), true, false );
	pOwner->SetAttackMoveEnable( false );	

	m_ChargeState = CS_JUMP_ATTACK;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << m_iCurCombo;
		kPacket << m_fJumpChargeRate;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioObjectWereWolfItem::CheckAirJumpAttack(ioBaseChar *pOwner )
{
	// For AirJumpAttackPower
	m_fDummyReleaseAirPowerRate = 0.0f;

	float fJumpPowerAmt = pOwner->GetCurJumpPowerAmt();
	if( fJumpPowerAmt > 0.0f )
	{
		m_fJumpAttackAirPowerRate = fabs( fJumpPowerAmt - pOwner->GetGravityAmt() ) / fJumpPowerAmt;
		m_fJumpAttackAirPowerRate = max( 0.0f, min( FLOAT1, m_fJumpAttackAirPowerRate ) );
		m_fJumpAttackAirPowerRate = FLOAT1 - m_fJumpAttackAirPowerRate;
	}

	if( m_fJumpAttackAirPower >= 0.0f )
	{
		float fCurJumpPower = m_fJumpAttackAirPower*m_fJumpAttackAirPowerRate;
		if( m_fJumpAttackAirPowerRate == 0.0f )
			fCurJumpPower = m_fJumpAttackAirPower;

		pOwner->SetAirJumpPower( fCurJumpPower );

		D3DXVECTOR3 vAttDir =  pOwner->GetTargetRot() * D3DXVECTOR3(0.0f, 0.0f, -FLOAT1);
		if( m_fJumpAttackAirForcePower < 0.0f )
			pOwner->SetForcePower( -vAttDir, fabs(m_fJumpAttackAirForcePower), FLOAT1, true );
		else
			pOwner->SetForcePower( vAttDir, fabs(m_fJumpAttackAirForcePower), FLOAT1, true );
	}
}

void ioObjectWereWolfItem::CheckDummyReleaseJump(ioBaseChar *pOwner )
{
	// For AirJumpAttackPower
	m_fDummyReleaseAirPowerRate = 0.0f;

	float fJumpPowerAmt = pOwner->GetCurJumpPowerAmt();
	if( fJumpPowerAmt > 0.0f )
	{
		m_fDummyReleaseAirPowerRate = fabs( fJumpPowerAmt - pOwner->GetGravityAmt() ) / fJumpPowerAmt;
		m_fDummyReleaseAirPowerRate = max( 0.0f, min( FLOAT1, m_fDummyReleaseAirPowerRate ) );
		m_fDummyReleaseAirPowerRate = FLOAT1 - m_fDummyReleaseAirPowerRate;
	}

	if( m_fDummyReleaseAirPower >= 0.0f )
	{
		float fCurJumpPower = m_fDummyReleaseAirPower*m_fDummyReleaseAirPowerRate;
		if( m_fDummyReleaseAirPowerRate == 0.0f )
			fCurJumpPower = m_fDummyReleaseAirPower;

		pOwner->SetAirJumpPower( fCurJumpPower );

		D3DXVECTOR3 vAttDir =  pOwner->GetTargetRot() * D3DXVECTOR3(0.0f, 0.0f, -FLOAT1);
		if( m_fDummyReleaseAirForcePower < 0.0f )
			pOwner->SetForcePower( -vAttDir, fabs(m_fDummyReleaseAirForcePower), FLOAT1, true );
		else
			pOwner->SetForcePower( vAttDir, fabs(m_fDummyReleaseAirForcePower), FLOAT1, true );
	}
}

void ioObjectWereWolfItem::ChangeToComboState( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	if( pOwner->GetState() != CS_ATTACK )
	{
		pOwner->SetCurNormalAttackItem( this );
		pOwner->SetState( CS_ATTACK );
	}
	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->InitExtendAttackTagInfo();
	ClearCancelInfo();

	m_iChargeCombo = 0;
	m_dwStartComboTime = dwCurTime;

	SetNextComboState( pOwner );
	pOwner->ApplyNormalAttackResult( m_iChargeCombo, true );

	m_ChargeState = CS_COMBO;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << m_iCurCombo;
		kPacket << m_iChargeCombo;
		kPacket << pOwner->GetTargetRot();
		pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	// send 후
	m_iChargeCombo++;
}

void ioObjectWereWolfItem::SetNextComboState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	pOwner->InitActionStopDelay( true );

	ClearKeyReserve();
	DWORD dwCurTime = FRAMEGETTIME();

	if( pOwner->IsNeedProcess() && pOwner->IsSettedDirection() )
	{
		ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();

		pOwner->SetTargetRotToDirKey( eNewDirKey );
	}

	int iExtendCnt = m_vChargeComboAttList.size();
	if( COMPARE( m_iChargeCombo, 0, iExtendCnt ) )
	{
		pOwner->SetAutoTarget( ATT_NORMAL );

		m_szCurAni = m_vChargeComboAttList[m_iChargeCombo].m_AttackAnimation;
		m_fCurAniRate = m_vChargeComboAttList[m_iChargeCombo].m_fAttackAniRate;

		m_dwInputCancelCheckTime = dwCurTime;
		m_dwCancelCheckTime = dwCurTime;

		pOwner->SetNormalAttackByAttribute( m_vChargeComboAttList[m_iChargeCombo] );

		ioEntityGroup *pGrp = pOwner->GetGroup();
		int iAniID	     = pGrp->GetAnimationIdx( m_vChargeComboAttList[m_iChargeCombo].m_AttackAnimation );
		float fTimeRate  = m_vChargeComboAttList[m_iChargeCombo].m_fAttackAniRate;
		DWORD dwPreDelay = m_vChargeComboAttList[m_iChargeCombo].m_dwPreDelay;

		pOwner->SetBaseAttackExtraAniJump( iAniID, fTimeRate, dwPreDelay );
		pOwner->SetExtendAttackEndJumpInfo( iAniID, fTimeRate, dwPreDelay );

		m_dwComboAttackTimeEnd = dwCurTime + pGrp->GetAnimationFullTime(iAniID) * fTimeRate + dwPreDelay;
		m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate + dwPreDelay;
		if( m_dwKeyReserveTime > 0 )
			m_dwKeyReserveTime += dwCurTime;

		DWORD dwTrackingTime = dwCurTime + dwPreDelay;
		dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
		pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
	}
}

void ioObjectWereWolfItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) return;

	int iState;
	rkPacket >> iState;
	rkPacket >> m_iCurCombo;

	switch( iState )
	{
	case CS_GATHERING:
		{
			bool bRelease;
			rkPacket >> bRelease;

			if( !bRelease )
				ChangeToGathering( pOwner );
			else
				m_bCheckReleaseWereWolf = true;
		}
		break;
	case CS_RELEASE:
		{
			m_bCheckReleaseWereWolf = true;
		}
		break;
	case CS_ATTACK_FIRE:
		{
			bool bCharged;
			rkPacket >> bCharged;

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			pOwner->SetTargetRot( qtRot );

			ChangeToAttackFire( pOwner, bCharged );
		}
		break;
	case CS_SPECAIL:
		{
			m_bCheckReleaseWereWolf = true;

			CreateAreaWeapon( pOwner, eSpecial );			

			ioBubbleItem *pBubble = ToBubbleItem( pOwner->GetWeapon() );
			if ( pBubble )
				pBubble->DecreaseExtraNeedGauge();

			pOwner->ChangeDirectionByInputDir( false );
			pOwner->SetState( CS_BUBBLE_OBJECT_SPECIAL, false );
		}
		break;
	case SS_SPEEDCHANGE:
		{
			rkPacket >> m_bUseSpeedChange;
		}
		break;
	case CS_COMBO:
		{
			rkPacket >> m_iChargeCombo;

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			pOwner->SetTargetRotToRotate( qtRot, true );
			ChangeToComboState( pOwner );

			pOwner->ApplyAutoTargetInfo( rkPacket );
		}
		break;
	case CS_JUMP_GATHERING:
		{
			rkPacket >> m_bJumpGatherMax;
			if( m_bJumpGatherMax )
			{
				pOwner->EndEffect( m_JumpGatheringEffectID, false );
				m_JumpGatheringEffectID = -1;

				if( m_JumpGatheringMaxEffectID == -1 )
				{
					ioEffect *pEffect = pOwner->AttachEffect( m_JumpGatheringMaxEffect );
					if( pEffect )
						m_JumpGatheringMaxEffectID = pEffect->GetUniqueID();
				}

				if( !m_JumpGatheringMaxSound.IsEmpty() )
				{
					ioEntityGroup *pGrp = pOwner->GetGroup();
					if( pGrp )
						g_SoundMgr.PlaySound( m_JumpGatheringMaxSound, pGrp->GetParentSceneNode() );
				}
			}
			else
				SetJumpGatheringState( pOwner );
		}
		break;
	case CS_JUMP_ATTACK:
		{
			rkPacket >> m_fJumpChargeRate;

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			pOwner->SetTargetRotToRotate( qtRot, true );

			ChangeToJumpChargeAttackFire( pOwner );
		}
		break;
	case CS_JUMP_END_ATTACK:
		{
			bool bAttack;
			rkPacket >> bAttack;
			rkPacket >> m_bJumpGatherMax;
			SetJumpChargeEndAttackState( pOwner, bAttack );
		}
		break;
	case SS_ROTATE:
		{
			int iDir;
			rkPacket >> iDir;
			rkPacket >> m_bLeftRot;

			if( iDir == ioUserKeyInput::DKI_NONE )
				m_CurDirKey = ioUserKeyInput::DKI_NONE;
			else
			{
				m_bTargetRot = false;
				m_CurDirKey = (ioUserKeyInput::DirKeyInput)iDir;
			}
		}
		break;
	}
}

void ioObjectWereWolfItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) return;

	int iState;
	rkPacket >> iState;

	if( iState == SS_JUMP_D_ATTACK )
	{
		D3DXQUATERNION qtRot;
		D3DXVECTOR3 vPos;
		rkPacket >> qtRot;
		rkPacket >> vPos;

		pOwner->SetTargetRot( qtRot );
		pOwner->SetWorldPosition( vPos );

		//ChangeJump_D_Attack( pOwner );
	}
	else
	{
		rkPacket >> m_iChargeCombo;

		D3DXQUATERNION qtRot;
		rkPacket >> qtRot;
		pOwner->SetTargetRotToRotate( qtRot, true );

		if( iState == CS_COMBO )
			SetNextComboState( pOwner );

		pOwner->ApplyAutoTargetInfo( rkPacket );
	}
}

void ioObjectWereWolfItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
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

bool ioObjectWereWolfItem::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		switch( m_ChargeState )
		{
		case CS_CHARGING:
		case CS_COMBO:
		case CS_JUMP_CHARGING:
		case CS_JUMP_GATHERING:
			return true;
		}
	}

	return false;
}

bool ioObjectWereWolfItem::IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck )
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

void ioObjectWereWolfItem::CheckReserve( ioBaseChar *pOwner )
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

bool ioObjectWereWolfItem::ProcessCancel( ioBaseChar *pOwner )
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
			
			if( pOwner->IsDirKeyDoubleClick() && !pOwner->HasBuff(BT_DISABLE_DASH) )
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

bool ioObjectWereWolfItem::SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType )
{
	if( !pOwner ) return false;

	DWORD dwCurTime = FRAMEGETTIME();
	ClearAirJump();

	AttackType eExtendType = AT_JUMP_ATTACK;

	const AttackAttribute *pAttr = GetAttackAttribute( eExtendType );
	if( !pAttr )
	{
		LOG.PrintTimeAndLog( 0, "ioObjectWereWolfItem::SetJumpAttack - AttackAttribute Not Exist" );
		return false;
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	iAniID = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioObjectWereWolfItem::SetJumpAttack - JumpAttack Animation Not Exist" );
		return false;
	}

	fTimeRate   = pAttr->m_fAttackAniRate;
	DWORD dwPreDelay  = pAttr->m_dwPreDelay;

	SetAirJump( pOwner, iAniID, fTimeRate );

	return false;
}

void ioObjectWereWolfItem::ProcessJumpping( ioBaseChar *pOwner, float fHeightGap )
{
	if( m_pExtendJump )
	{
		m_pExtendJump->ProcessJumpping( pOwner, fHeightGap, m_fJumpNeedGauge );
	}
}

void ioObjectWereWolfItem::ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
	if( m_pExtendJump )
	{
		m_pExtendJump->ProcessJumpAttack( pOwner, fHeightGap );
	}

	CheckAirJump( pOwner );
}

bool ioObjectWereWolfItem::IsOneTimeWeaponItem() const
{
	return false;
}

bool ioObjectWereWolfItem::IsEnableDash() const
{
	return true;
}

bool ioObjectWereWolfItem::IsEnableDefense() const
{
	bool bEnableDefense = false;

	switch( m_WolfDefenseType )
	{
	case WDT_NORMAL:
		bEnableDefense = true;
		break;
	case WDT_DISABLE:
		bEnableDefense = false;
		break;
	}

	return bEnableDefense;
}

void ioObjectWereWolfItem::OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem )
{
	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );

	m_fCurChargeRate = 0.0f;
	m_bSetJumpGatheringMoveAni = false;
	m_bJumpGatherMax = false;
}

void ioObjectWereWolfItem::ChangeToGathering( ioBaseChar *pOwner )
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
		kPacket << false;		// release
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioObjectWereWolfItem::OnGathering( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )	return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( pOwner->IsAttackKeyDown() )
	{
		// MaxCheck
		if( m_bEnableMaxCharge && m_dwGatheringStartTime+m_dwMaxChargeTime < dwCurTime )
		{
			// ReleaseWereWolf
			m_bCheckReleaseWereWolf = true;

			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
				FillItemGaugePacketHeader( pOwner, kPacket );
				kPacket << m_ChargeState;
				kPacket << m_iCurCombo;
				kPacket << true;		// release
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
			return;
		}
	}
	else if( pOwner->IsDefenseKeyDown() )
	{
		if( m_ExtendChargeType == ECT_DR_SPIDER )
			ChangeToRelease( pOwner );
	}
	else
	{
		ChangeToAttackFire( pOwner, true );
	}
}

void ioObjectWereWolfItem::OnComboState( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwComboAttackTimeEnd < dwCurTime )
	{
		ProcessReserveKeyInput( pOwner );
		return;
	}
	else if( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
	{
		CheckReserve( pOwner );

		if( m_iChargeCombo == (int)m_vChargeComboAttList.size() )
			return;

		if( m_dwStartComboTime+m_dwMaxComboDuration > dwCurTime )
		{
			if( pOwner->IsAttackKey() )
			{
				SetNextComboState( pOwner );

				if( pOwner->IsNeedSendNetwork() )
				{
					SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
					FillItemGaugePacketHeader( pOwner, kPacket );
					kPacket << m_ChargeState;
					kPacket << m_iChargeCombo;
					kPacket << pOwner->GetTargetRot();
					pOwner->FillAutoTargetInfo( kPacket );
					P2PNetwork::SendToAllPlayingUser( kPacket );
				}

				m_iChargeCombo++;
			}
			else if( IsEnableDefenseRelease() && pOwner->IsDefenseKey() )
			{
				ChangeToRelease( pOwner );
			}
			else if( m_iSkillInput >= 0 && !pOwner->IsBuffLimitSkill() && pOwner->SetUseSkill( m_iSkillInput, SUT_NORMAL ) )
			{
				m_iSkillInput = -1;
				return;
			}
		}
	}
}

void ioObjectWereWolfItem::OnJumpGathering( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )	return;

	DWORD dwCurTime = FRAMEGETTIME();

	// 점프키 누름. 점프상태 전환
	if( pOwner->IsDirKeyDoubleClick() && !pOwner->HasBuff(BT_DISABLE_DASH) )
	{
		pOwner->ChangeDirectionByInputDir( false );
		pOwner->SetDashState( false );
		pOwner->SendDashState( false );
		return;
	}

	if( pOwner->IsJumpKeyDown() )
	{
		if( (m_dwGatheringStartTime + m_dwJumpGatheringMaxTime < dwCurTime) &&
			m_JumpGatheringMaxEffectID == -1 )
		{
			pOwner->EndEffect( m_JumpGatheringEffectID, false );
			m_JumpGatheringEffectID = -1;
			m_bJumpGatherMax = true;

			ioEffect *pEffect = pOwner->AttachEffect( m_JumpGatheringMaxEffect );
			if( pEffect )
				m_JumpGatheringMaxEffectID = pEffect->GetUniqueID();

			if( !m_JumpGatheringMaxSound.IsEmpty() )
			{
				ioEntityGroup *pGrp = pOwner->GetGroup();
				if( pGrp )
					g_SoundMgr.PlaySound( m_JumpGatheringMaxSound, pGrp->GetParentSceneNode() );
			}

			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
				FillItemGaugePacketHeader( pOwner, kPacket );
				kPacket << m_ChargeState;
				kPacket << m_iCurCombo;
				kPacket << true;		// Max Charge
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	}
	else
	{
		ChangeToJumpChargeAttackFire( pOwner );
	}
}

void ioObjectWereWolfItem::ChangeToRelease( ioBaseChar *pOwner )
{
	// ReleaseWereWolf
	m_ChargeState = CS_RELEASE;
	m_bCheckReleaseWereWolf = true;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioObjectWereWolfItem::CheckLanding( ioBaseChar *pChar )
{
	m_iUseSylphidType = ST_NotUse;
	CreateAreaWeapon( pChar, eLand );

	return ioAttackableItem::CheckLanding( pChar );
}

void ioObjectWereWolfItem::SetJumpping( ioBaseChar *pOwner, bool bFullTime )
{
	m_bUseSpeedChange = false;

	if( bFullTime )
	{
		ioBubbleItem *pBubble = ToBubbleItem( pOwner->GetWeapon() );
		if( pBubble && pBubble->IsEnableGauge() )
		{
			CreateAreaWeapon( pOwner, eSpecial );

			m_ChargeState = CS_SPECAIL;
			m_bCheckReleaseWereWolf = true;	

			pBubble->DecreaseExtraNeedGauge();
			pOwner->ChangeDirectionByInputDir( false );
			pOwner->SetState( CS_BUBBLE_OBJECT_SPECIAL, false );

			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
				FillItemGaugePacketHeader( pOwner, kPacket );
				kPacket << m_ChargeState;
				kPacket << m_iCurCombo;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
			return;
		}

		if( IsEnableJumpCharge() )
		{
			SetJumpChargeState( pOwner );
			return;
		}
	}
	ioAttackableItem::SetJumpping( pOwner, bFullTime );
}

void ioObjectWereWolfItem::CreateAreaWeapon( ioBaseChar *pOwner, int iType )
{
	if( !pOwner ) return;

	if( iType == eLand && m_szLandAreaWeapon.m_iWeaponIdx != 0 )
		pOwner->NormalAttackFireExplicit( m_szLandAreaWeapon );
	else if( m_szSpecialAttackAreaWeapon.m_iWeaponIdx != 0 )
		pOwner->NormalAttackFireExplicit( m_szSpecialAttackAreaWeapon );
}

float ioObjectWereWolfItem::GetAttackSpeedRate() const
{
	if( m_bUseSpeedChange && m_fChangeSpeed != 0.f )
		return m_fChangeSpeed;

	return m_fAttackSpeedRate;
}

void ioObjectWereWolfItem::SetDefenseSpeedChange( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() ) return;

	m_bUseSpeedChange = !m_bUseSpeedChange;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SS_SPEEDCHANGE;
		kPacket << m_iCurCombo;
		kPacket << m_bUseSpeedChange;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioObjectWereWolfItem::SetUseSylphidType( int iType )
{
	m_iUseSylphidType = iType;
}

bool ioObjectWereWolfItem::IsUseGaugeMode()
{
	if( m_bUseSpeedChange )
		return true;
	if( m_iUseSylphidType != ST_NotUse )
		return true;

	return false;
}

DWORD ioObjectWereWolfItem::GetJumpChargeTime( ioBaseChar *pChar ) const
{
	if( m_dwJumpChargeTime != 0 )
		return m_dwJumpChargeTime;

	return ioAttackableItem::GetJumpChargeTime( pChar );
}

void ioObjectWereWolfItem::ChangeToGriffin( ioBaseChar *pOwner )
{
	m_bCheckReleaseWereWolf = true;

	ioChangeOnItem* pChangeOn = ToChangeOnItem( pOwner->GetWeapon() );
	if( pChangeOn )
		pChangeOn->SetChangeToGriffinState( true );
}

void ioObjectWereWolfItem::SetJumpChargeState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	pOwner->SetCurNormalAttackItem( this );
	pOwner->SetState( CS_ATTACK );

	m_ChargeState = CS_JUMP_CHARGING;
	m_bJumpGatherMax = false;
}

void ioObjectWereWolfItem::SetJumpGatheringState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	float fCurRate = FLOAT1;
	if( m_fJumpGatherAniRate > 0.0f )
		fCurRate = m_fJumpGatherAniRate;

	pGrp->SetLoopAni( m_JumpGatherAnimation, FLOAT100, FLOAT1, FLOAT1/fCurRate );
	
	ioEffect *pEffect = pOwner->AttachEffect( m_JumpGatheringEffect );
	if( pEffect )
		m_JumpGatheringEffectID = pEffect->GetUniqueID();

	m_ChargeState = CS_JUMP_GATHERING;
	m_dwGatheringStartTime = FRAMEGETTIME();

	pOwner->SetAttackMoveEnable( true );
}

void ioObjectWereWolfItem::CheckJumpGatheringMoveState( ioBaseChar *pOwner )
{
	if( !pOwner->IsCanAttackMove() ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = 0;
	if( pOwner->IsSettedDirection() )
	{
		if( !m_bSetJumpGatheringMoveAni )
		{
			iAniID = pGrp->GetAnimationIdx(m_JumpGatheringMoveAnimation);
			if( iAniID == -1 )
				return;

			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->ClearAllLoopAni( FLOAT100, true );
			pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/m_fJumpGatherAniRate );

			m_bSetJumpGatheringMoveAni = true;
		}
	}
	else
	{
		if( m_bSetJumpGatheringMoveAni )
		{
			iAniID = pGrp->GetAnimationIdx( m_JumpGatherAnimation );
			if( iAniID == -1 )
				return;

			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->ClearAllLoopAni( FLOAT100, true );
			pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/m_fJumpGatherAniRate );

			m_bSetJumpGatheringMoveAni = false;
		}
	}
}

void ioObjectWereWolfItem::SetJumpChargeEndAttackState( ioBaseChar *pOwner, bool bAttack )
{
	m_dwJumpChargeEndTime = 0;

	if( !pOwner )
		return;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	int iBuffCnt = m_JumpChargeActionBuffList.size();
	for( int i=0; i < iBuffCnt; ++i )
		pOwner->RemoveBuff( m_JumpChargeActionBuffList[i] );

	pOwner->SetMoveDirByRotate( pOwner->GetTargetRot() );
	pOwner->SetCharColSkipSkillTime( 0, 0 );
	pOwner->SetCurMoveSpeed( 0.f );

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	pOwner->InitExtendAttackTagInfo();
	pOwner->ClearReservedSliding();
	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	m_ChargeState = CS_JUMP_END_ATTACK;
	m_dwJumpChargeEndTime = FRAMEGETTIME();

	AttackAttribute cInfo;
	if( !bAttack )
		cInfo = m_JumpChargeEnd;
	else if( m_bJumpGatherMax )
		cInfo = m_JumpChargeMaxEndAttack;
	else
		cInfo = m_JumpChargeEndAttack;

	int iAniID = pGrp->GetAnimationIdx( cInfo.m_AttackAnimation );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/cInfo.m_fAttackAniRate );
				
	pOwner->RefreshFireTimeList( iAniID, cInfo.m_vWeaponInfoList, FTT_EXTEND_ATTACK, cInfo.m_fAttackAniRate, 0 );
	pOwner->CheckCharColSkipTime( iAniID, cInfo.m_fAttackAniRate, cInfo.m_dwPreDelay );
	pOwner->SetReservedSliding( cInfo.m_vForceInfoList, iAniID, cInfo.m_fAttackAniRate, cInfo.m_dwPreDelay );

	m_dwJumpChargeEndTime += ( pGrp->GetAnimationFullTime( iAniID ) * cInfo.m_fAttackAniRate );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << m_iCurCombo;
		kPacket << bAttack;
		kPacket << m_bJumpGatherMax;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioObjectWereWolfItem::CheckKeyInput( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() ) return;

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
				SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
				FillItemGaugePacketHeader( pOwner, kPacket );
				kPacket << SS_ROTATE;
				kPacket << m_iCurCombo;
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
				SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
				FillItemGaugePacketHeader( pOwner, kPacket );
				kPacket << SS_ROTATE;
				kPacket << m_iCurCombo;
				kPacket << (int)m_CurDirKey;
				kPacket << m_bLeftRot;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	}
}

void ioObjectWereWolfItem::ProcessRotate( ioBaseChar *pOwner )
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

//void ioObjectWereWolfItem::ProcessDrSpiderJumpAttack( ioBaseChar *pOwner )
//{
//	if( !pOwner )
//		return;
//
//	// 기본 점프 공격 활성화되어있으면 점프 D 공격은 사용되지 않음
//	if( m_bJumpAttack )
//		return;
//
//	// 점프 공격 딜레이 체크
//	float fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetBottomHeight();
//	if( fHeightGap <= 0.0f )
//	{
//		DWORD dwCurTime = FRAMEGETTIME();
//
//		if( pOwner->IsNeedProcess() && (dwCurTime > m_dwAttackKeyReservTime) )
//			pOwner->SetJumpBoostAttackState(false);
//	}
//
//	DWORD dwCurTime = FRAMEGETTIME();
//
//	if( pOwner->IsAttackKeyDown() )
//	{
//		if( pOwner->IsNeedProcess() && pOwner->GetState() == CS_JUMP && (dwCurTime > m_dwAttackKeyReservTime) )
//		{
//			// 오른쪽 조건문 - 버프로 인한 Y축 높이 제한 확인 함수에서 fasle로 반환 받아야 점프 D 공격이 가능해짐
//			if( Check_D_Attack_Max_Weapon_Cnt(pOwner) && !pOwner->CheckAxisLimitByBuff() )
//				ChangeJump_D_Attack( pOwner );
//		}
//	}
//
//	// 공중에서 D 공격 후 재 공격 가능한지 확인하는 플래그 체크
//	if( pOwner->IsNeedProcess() && (dwCurTime > m_dwAttackKeyReservTime) )
//	{
//		pOwner->SetJumpBoostAttackState(false);
//	}
//}
//////////////////////////////////////////////////////////////////////////////////
ioBubbleObjectSpecialState::ioBubbleObjectSpecialState()
{
}

ioBubbleObjectSpecialState::~ioBubbleObjectSpecialState()
{
}

void ioBubbleObjectSpecialState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	int iAniID = pGrp->GetAnimationIdx( m_szStartAni );
	float fTimeRate = m_fStartAniRate;

	pOwner->InitExtendAttackTagInfo();
	pOwner->ClearReserveKeyInput();
	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	DWORD dwPreDelay = 0;
	pGrp->SetActionAni( iAniID, FLOAT10, FLOAT10, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );

	m_State = GAS_DELAY;

	float fCurGravity = pOwner->GetCharGravity();
	m_fFlightDuration = GetFlightDuration( fCurGravity );

	if( m_fFlightDuration > 0.0f )
	{
		m_fCharMoveSpeed = m_fCircleOffSet / (m_fFlightDuration / FLOAT1000);
	}

	if( !pOwner->CheckMagicCircle() )
	{
		ioHashString szDummyCircle;
		pOwner->CreateMagicCircle( m_ReadyCircle, m_ReadyCircle, szDummyCircle, m_fCircleOffSet );	
	}

	pOwner->UpdateMagicCirclePos( 0.0f, m_fCircleOffSet, m_fCircleHeightGap, true, false );
	m_vCirclePos = pOwner->GetMagicCirclePos( m_fCircleOffSet, false );

	// CharMove
	m_dwActionStartTime = FRAMEGETTIME();
	m_vCharMoveDir = ioMath::VEC3_ZERO;

	D3DXVECTOR3 vDir = m_vCirclePos - pOwner->GetWorldPosition();
	vDir.y = 0.0f;

	float fRate = FLOAT1;
	float fRange = D3DXVec3Length( &vDir );
	if( fRange > 5.0f )
	{
		D3DXVec3Normalize( &m_vCharMoveDir, &vDir );

		D3DXQUATERNION qtRot;
		ioMath::CalcDirectionQuaternion( qtRot, -m_vCharMoveDir );
		pOwner->SetMoveDirByRotate( qtRot );
	}

	pOwner->SetJumpPower( m_fCharJumpPower );
	m_vCircleMoveDir = m_vCharMoveDir;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SPECIAL_STATE_SYNC );
		kPacket << m_pOwner->GetCharName();
		kPacket << pOwner->GetTargetRot();
		kPacket	<< pOwner->GetWorldPosition();
		kPacket << m_State;
		kPacket << m_vCirclePos;
		kPacket << m_vCircleMoveDir;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioBubbleObjectSpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );

	if( pOwner )
		pOwner->DestroyMagicCircle();
}

void ioBubbleObjectSpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	switch( m_State )
	{
	case GAS_DELAY:
	case GAS_MOVE:
		if( CheckLanding( pOwner ) )
		{
			SetEndState( pOwner );
		}
		else
		{
			ioPlayStage *pStage = pOwner->GetCreator();
			if( !pStage )
				return;

			CheckControl( pOwner );
			ProcessControl( pOwner, pStage );

			CheckMove( pOwner, pStage );
			ProcessMove( pOwner, pStage );
		}
		break;
	case GAS_END:
		ProcessEnd( pOwner );
		break;
	}
}

void ioBubbleObjectSpecialState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
	return;
}

void ioBubbleObjectSpecialState::LoadProperty( ioINILoader &rkLoader )
{
	ioSpecialStateBase::LoadProperty( rkLoader );

	char szKey[MAX_PATH], szBuf[MAX_PATH];

	rkLoader.LoadString_e( "start_ani", "", szBuf, MAX_PATH );
	m_szStartAni = szBuf;
	m_fStartAniRate = rkLoader.LoadFloat_e( "start_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "end_ani", "", szBuf, MAX_PATH );
	m_szEndAni = szBuf;
	m_fEndAniRate = rkLoader.LoadFloat_e( "end_ani_rate", FLOAT1 );

	m_fCircleMoveSpeed = rkLoader.LoadFloat_e( "magic_circle_move_speed", 0.0f );
	m_fCircleHeightGap = rkLoader.LoadFloat_e( "magic_circle_height_gap", 0.0f );
	m_fCircleOffSet = rkLoader.LoadFloat_e( "magic_circle_offset", 0.0f );

	rkLoader.LoadString_e( "ready_circle", "", szBuf, MAX_PATH );
	m_ReadyCircle = szBuf;

	m_fCharJumpPower = rkLoader.LoadFloat_e( "skill_jump_power", 0.0f );
	m_fSkillGravityRate = rkLoader.LoadFloat_e( "skill_gravity_rate", 0.0f );

	int iAttackTypeCnt = rkLoader.LoadInt_e( "land_attack_type_count", 1 );
	for( int i=0 ; i<iAttackTypeCnt ; i++ )
	{
		WeaponInfo kInfo;

		wsprintf_e( szKey, "land_attack_type%d", i+1 );
		kInfo.m_iWeaponIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "land_attack_type%d_resist", i+1 );
		kInfo.m_iResistanceIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "land_attack_type%d_wounded", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_WoundedAnimation = szBuf;

		wsprintf_e( szKey, "land_attack_type%d_wounded_time", i+1 );
		kInfo.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "land_attack_type%d_wounded_loop_ani", i+1 );
		kInfo.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );

		m_vWeaponInfoList.push_back( kInfo );
	}
}

float ioBubbleObjectSpecialState::GetFlightDuration( float fCurGravity )
{
	float fTopTime, fTopHeight;
	fTopTime = fTopHeight = 0.0f;

	float fNewGravity = fCurGravity * m_fSkillGravityRate;
	if( fNewGravity > 0.0f )
	{
		fTopTime = m_fCharJumpPower / fNewGravity;
		fTopHeight = (m_fCharJumpPower*fTopTime) - (0.5f*fNewGravity*fTopTime*fTopTime);
	}

	float fResult = 2 * fTopTime * FLOAT1000;

#ifndef SHIPPING 
	if( Setting::ShowExtraInfo() )
	{
		LOG.PrintTimeAndLog( 0, "ioTargetMoveAttackSkill - [CheckValue] TopHeight: %.2f, FlightDuration: %.2f", fTopHeight, fResult );
	}
#endif

	return fResult;
}

void ioBubbleObjectSpecialState::CheckControl( ioBaseChar *pChar )
{
	if( !pChar ) return;
	if( !pChar->IsNeedProcess() ) return;

	bool bUpdate = false;
	if( m_State == GAS_DELAY )
	{
		if( pChar->IsSettedDirection() )
		{
			ioUserKeyInput::DirKeyInput eDir = pChar->CheckCurDirKey();
			if( eDir != ioUserKeyInput::DKI_NONE )
			{
				float fYawD = 180.0f + 45.0f * ( eDir - ioUserKeyInput::DKI_UP );
				fYawD = ioMath::ArrangeHead( fYawD );

				float fYawR = DEGtoRAD( fYawD );
				D3DXQUATERNION qtRotate;
				D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

				m_vCircleMoveDir = qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			}

			m_State = GAS_MOVE;
			bUpdate = true;
		}
	}
	else if( m_State == GAS_MOVE )
	{
		if( !pChar->IsSettedDirection() )
		{
			m_State = GAS_DELAY;
			bUpdate = true;
		}
		else
		{
			if( pChar->IsChangeDirection() )
			{
				ioUserKeyInput::DirKeyInput eDir = pChar->CheckCurDirKey();
				if( eDir != ioUserKeyInput::DKI_NONE )
				{
					float fYawD = 180.0f + 45.0f * ( eDir - ioUserKeyInput::DKI_UP );
					fYawD = ioMath::ArrangeHead( fYawD );

					float fYawR = DEGtoRAD( fYawD );
					D3DXQUATERNION qtRotate;
					D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

					m_vCircleMoveDir = qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
				}

				bUpdate = true;
			}
		}
	}

	if( bUpdate && pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SPECIAL_STATE_SYNC );
		kPacket << m_pOwner->GetCharName();
		kPacket << pChar->GetTargetRot();
		kPacket	<< pChar->GetWorldPosition();
		kPacket << m_State;
		kPacket << m_vCirclePos;
		kPacket << m_vCircleMoveDir;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioBubbleObjectSpecialState::ProcessControl( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( m_State == GAS_MOVE && m_fCircleMoveSpeed > 0.0f )
	{
		D3DXVECTOR3 vPrePos, vCurPos, vPos;
		vPos = m_vCircleMoveDir;
		vPos *= m_fCircleMoveSpeed * g_FrameTimer.GetSecPerFrame();

		vPrePos = vCurPos = m_vCirclePos;
		vCurPos += vPos;

		float fMapHeight = pStage->GetMapHeight( vCurPos.x, vCurPos.z, pChar, false );
		if( fMapHeight > 0.0f )
			vCurPos.y = fMapHeight;

		m_vCirclePos = vCurPos;

		if( pChar )
			pChar->SetMagicCirclePos( true, m_vCirclePos );
	}
}

void ioBubbleObjectSpecialState::CheckMove( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();
	D3DXVECTOR3 vDir = m_vCirclePos - pChar->GetWorldPosition();
	vDir.y = 0.0f;

	float fRate = FLOAT1;
	float fRange = D3DXVec3Length( &vDir );
	if( fRange > 5.0f )
	{
		float fGapTime = m_fFlightDuration - (dwCurTime - m_dwActionStartTime);

		float fNewSpeed = m_fCurCharMoveSpeed;
		if( fGapTime > 0 )
		{
			fNewSpeed = fRange / (float)(fGapTime / FLOAT1000);
		}

		m_fCurCharMoveSpeed = fNewSpeed;

		D3DXVec3Normalize( &m_vCharMoveDir, &vDir );

		D3DXQUATERNION qtRot;
		ioMath::CalcDirectionQuaternion( qtRot, -m_vCharMoveDir );
		pChar->SetMoveDirByRotate( qtRot );
	}
}

void ioBubbleObjectSpecialState::ProcessMove( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar ) return;
	if( !pStage ) return;
	if( m_fCurCharMoveSpeed == 0.0f ) return;

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	bool bCurMove = false;
	D3DXVECTOR3 vTotalMove( 0.0f, 0.0f, 0.0f );

	float fCurTimeGap = fTimePerSec * FLOAT1000;
	while( fCurTimeGap > 0.0f )
	{
		float fDivideTimePerSec = min( fCurTimeGap, MIN_FRAME_TIME ) * 0.001f;
		fCurTimeGap -= MIN_FRAME_TIME;

		D3DXVECTOR3 vMoveAmt = m_vCharMoveDir * m_fCurCharMoveSpeed * fDivideTimePerSec;
		if( vMoveAmt != ioMath::VEC3_ZERO )
		{
			vTotalMove += vMoveAmt;
			bCurMove = true;
		}
	}

	if( bCurMove )
	{
		bool bCol = false;
		if( pStage->ReCalculateMoveVectorByTerrain( pChar, &vTotalMove, bCol ) )
		{
			pChar->Translate( vTotalMove );
		}
	}

	D3DXVECTOR3 vPos = pChar->GetWorldPosition();
	if( pChar->CheckAxisLimit( &vPos ) )
		pChar->SetWorldPosition( vPos );
}

bool ioBubbleObjectSpecialState::CheckLanding( ioBaseChar *pChar )
{
	if( !pChar ) return true;

	DWORD dwCurTime = FRAMEGETTIME();

	float fHeightGap = 0.0f;
	if( pChar->CheckGhostState() )
		fHeightGap = pChar->GetWorldPosition().y - pChar->GetPreHeightForGhostState();
	else
		fHeightGap = pChar->GetWorldPosition().y - pChar->GetBottomHeight();
	
	if( fHeightGap > 0.0f || !pChar->IsDownState() )
	{
		return false;
	}

	return true;
}

void ioBubbleObjectSpecialState::SetEndState( ioBaseChar *pChar )
{
	m_State = GAS_END;
	DWORD dwCurTime = FRAMEGETTIME();

	pChar->DestroyMagicCircle();

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iCurSkillAniID = pGrp->GetAnimationIdx( m_szEndAni );
	if( iCurSkillAniID == -1 )	return;

	float fCurEndRate = m_fEndAniRate;
	if( fCurEndRate <= 0.0f )
		fCurEndRate = FLOAT1;
	
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iCurSkillAniID, FLOAT100, FLOAT100, FLOAT1/fCurEndRate );

	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iCurSkillAniID ) * fCurEndRate;
	m_dwActionStartTime = dwCurTime + pChar->GetFirstFireTime( m_szEndAni ) * fCurEndRate;
}

void ioBubbleObjectSpecialState::ProcessEnd( ioBaseChar *pChar )
{
	if( !pChar ) return;
	if( m_State != GAS_END ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwActionStartTime != 0 && m_dwActionStartTime < dwCurTime && !m_vWeaponInfoList.empty() )
	{
		pChar->NormalAttackFireExplicit( m_vWeaponInfoList.front() );
		m_dwActionStartTime = 0;
	}
	else if( m_dwMotionEndTime < dwCurTime )
		pChar->SetState( CS_DELAY );
}

void ioBubbleObjectSpecialState::ApplyStateSync( SP2Packet &rkPacket, ioBaseChar* pOwner )
{
	ioSpecialStateBase::ApplyStateSync( rkPacket, pOwner );

	D3DXQUATERNION qtRot;
	rkPacket >> qtRot;
	pOwner->SetTargetRotToRotate( qtRot, true );

	D3DXVECTOR3 vPos;
	rkPacket >> vPos;
	pOwner->SetWorldPosition( vPos );

	int eState;
	rkPacket >> eState;

	switch( eState )
	{
	case GAS_DELAY:
	case GAS_MOVE:
		{
			m_State = (GardenerAttackState)eState;
			rkPacket >> m_vCirclePos;
			rkPacket >> m_vCircleMoveDir;
		}
		break;
	}
}