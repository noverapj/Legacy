
#include "stdafx.h"

#include "ioChangeOnItem.h"
#include "WeaponDefine.h"
#include "ItemDefine.h"
#include "BuffDefine.h"
#include "FindPredicateImpl.h"
#include "ioDruidEagleDummy.h"

ioChangeOnItem::ioChangeOnItem()
{
	m_ChargeState = CS_NONE;
	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

	m_iCurSpecialAttCnt = 0;
	m_fCurChangeDamage = 0.f;
}

ioChangeOnItem::ioChangeOnItem( const ioChangeOnItem &rhs )
: ioWeaponItem( rhs ),
  m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
  m_iMaxSpecialAttCnt( rhs.m_iMaxSpecialAttCnt ),
  m_iNeedSpecialAttCnt( rhs.m_iNeedSpecialAttCnt ),
  m_fMaxChangeDamage( rhs.m_fMaxChangeDamage ),
  m_fDecreaseChangeDamage( rhs.m_fDecreaseChangeDamage ),
  m_fFlyDecreaseChangeDamage( rhs.m_fFlyDecreaseChangeDamage ),
  m_fDownDecreaseChangeDamage( rhs.m_fDownDecreaseChangeDamage ),
  m_fUseGaugeDecreaseChangeDamage( rhs.m_fUseGaugeDecreaseChangeDamage ),
  m_fChangeWerewolfEndJumpPower( rhs.m_fChangeWerewolfEndJumpPower ),
  m_szReleaseWereWolfAni( rhs.m_szReleaseWereWolfAni ),
  m_fReleaseWereWolfAniRate( rhs.m_fReleaseWereWolfAniRate ),
  m_GriffinDummyCharName( rhs.m_GriffinDummyCharName ),
  m_dwFlyChargeTime( rhs.m_dwFlyChargeTime ),
  m_dwWeaponIdx( rhs.m_dwWeaponIdx ),
  m_fReadyMarkerOffset( rhs.m_fReadyMarkerOffset ),
  m_fCheckHeightMin( rhs.m_fCheckHeightMin ),
  m_fTargetMaxRange( rhs.m_fTargetMaxRange ),
  m_fTargetMinAngle( rhs.m_fTargetMinAngle ),
  m_fTargetMaxAngle( rhs.m_fTargetMaxAngle ),
  m_dwTargetingTime( rhs.m_dwTargetingTime ),
  m_FindTargetEagleFlyInfo( rhs.m_FindTargetEagleFlyInfo ),
  m_TargetAttackEagleFlyInfo( rhs.m_TargetAttackEagleFlyInfo ),
  m_UpFlyInfo( rhs.m_UpFlyInfo ),
  m_fUpAngle( rhs.m_fUpAngle ),
  m_fFlyEndJumpPower( rhs.m_fFlyEndJumpPower ),
  m_GrapplingInfo( rhs.m_GrapplingInfo ),
  m_dwGrapChargeMaxTime( rhs.m_dwGrapChargeMaxTime ),
  m_GrapAttackAttribute( rhs.m_GrapAttackAttribute ),
  m_fGrpplingCharToCharGap( rhs.m_fGrpplingCharToCharGap ),
  m_fGrpplingCharMidPosRate( rhs.m_fGrpplingCharMidPosRate ),
  m_WerewolfDummyCharName( rhs.m_WerewolfDummyCharName )
{
	m_ChargeState = CS_NONE;
	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_iCurSpecialAttCnt = 0;
	m_fCurChangeDamage = 0.f;

	m_bChangeToGriffin = false;

	m_fCurrFlyRange = 0.f;
	m_dwStartTime = 0;
	m_dwCharingStartTime = 0;
	m_dwTargetingStartTime = 0;
	m_dwChangeWereWolfTime = 0;
	m_dwCurrWeaponIdx = 0;
	m_GriffinDummyCharIndex = 0;
	m_iWerewolfDummyCharIndex = 0;

	m_vMoveDir = ioMath::VEC3_ZERO;
	m_eDirKey = ioUserKeyInput::DKI_NONE;
	m_AttackKeyType = AKT_NONE;
	m_UpDownKey = UDK_NONE;
}

ioChangeOnItem::~ioChangeOnItem()
{
}

void ioChangeOnItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	int i;
	char szBuf[MAX_PATH];

	int iExtendCnt = rkLoader.LoadInt_e( "extend_attack_cnt", 0 );

	m_AttackReadyAniList.clear();
	for( i=0 ; i<iExtendCnt ; i++ )
	{
		wsprintf_e( szBuf, "attack_ready_animation%d", i+1 );
		rkLoader.LoadString( szBuf, "", szBuf, MAX_PATH );
		m_AttackReadyAniList.push_back( ioHashString(szBuf) );
	}

	m_iMaxSpecialAttCnt = rkLoader.LoadInt_e( "max_special_att_cnt", 0 );
	m_iNeedSpecialAttCnt = rkLoader.LoadInt_e( "need_special_att_cnt", 0 );
	
	m_fMaxChangeDamage = rkLoader.LoadFloat_e( "max_change_damage", 0.0f );
	m_fDecreaseChangeDamage = rkLoader.LoadFloat_e( "decrease_change_damage", 0.0f );
	m_fDownDecreaseChangeDamage = rkLoader.LoadFloat_e( "down_decrease_change_damage", 0.0f );
	m_fFlyDecreaseChangeDamage = rkLoader.LoadFloat_e( "fly_decrease_change_damage", 0.0f );
	m_fUseGaugeDecreaseChangeDamage = rkLoader.LoadFloat_e( "use_gauge_decrease_change_damage", 0.0f );
	m_fChangeWerewolfEndJumpPower = rkLoader.LoadFloat_e( "change_werewolf_end_jump_power", 1000.0f );

	rkLoader.LoadString_e( "release_werewolf_ani", "", szBuf, MAX_PATH );
	m_szReleaseWereWolfAni = szBuf;
	m_fReleaseWereWolfAniRate = rkLoader.LoadFloat_e( "release_werewolf_ani_rate", 1.f );

	rkLoader.LoadString_e( "werewolf_dummy_name", "", szBuf, MAX_PATH );
	m_WerewolfDummyCharName		= szBuf;
	rkLoader.LoadString_e( "griffin_dummy_name", "", szBuf, MAX_PATH );
	m_GriffinDummyCharName		= szBuf;	

	m_dwFlyChargeTime			= rkLoader.LoadInt_e( "fly_charge_time", 0 );
	m_dwWeaponIdx				= rkLoader.LoadInt_e( "target_fly_attack_weapon", 0 );
	
	m_fCheckHeightMin			= rkLoader.LoadFloat_e( "height_check_min", 0.0f );
	m_fFlyEndJumpPower			= rkLoader.LoadFloat_e( "fly_change_end_jump_power", 1000.0f );

	rkLoader.LoadString_e( "fly_attack_grap_loop_ani", "", szBuf, MAX_PATH );
	m_GrapplingInfo.m_GrapplingLoopMotion = szBuf;
	rkLoader.LoadString_e("fly_attack_grap_loop2_ani", "", szBuf, MAX_PATH );
	m_GrapplingInfo.m_GrapplingLoopMotion2 = szBuf;
	m_GrapplingInfo.m_fGrapplingLoopMotionRate = rkLoader.LoadFloat_e("fly_attack_grap_loop_aniRate", 0 );

	m_dwGrapChargeMaxTime = rkLoader.LoadInt_e( "fly_grap_charging_max_time", 0 );
	m_fGrpplingCharToCharGap	= rkLoader.LoadFloat_e( "fly_attack_grap_target_loop_gap_range", 0.0f );
	m_fGrpplingCharMidPosRate	= rkLoader.LoadFloat_e( "fly_attack_grap_char_height_rate", FLOAT05 );

	LoadAttackAttribute( "fly_grap_attack", m_GrapAttackAttribute, rkLoader );
	LoadTargetMarker( rkLoader );
	LoadFindFly( rkLoader );
	LoadTargetFly( rkLoader );
	LoadUpFly( rkLoader );
}

void ioChangeOnItem::LoadTargetMarker( ioINILoader &rkLoader )
{
	m_fReadyMarkerOffset	= rkLoader.LoadFloat_e( "ready_marker_offset", 0.0f );
	m_fTargetMaxRange		= rkLoader.LoadFloat_e( "target_find_max_range", 0.0f );
	m_fTargetMaxAngle		= rkLoader.LoadFloat_e( "target_find_max_angle", 0.0f );
	m_fTargetMinAngle		= rkLoader.LoadFloat_e( "target_find_min_angle", 0.0f );
	m_dwTargetingTime		= rkLoader.LoadInt_e( "target_duration", 0 );
}

void ioChangeOnItem::LoadFindFly( ioINILoader &rkLoader )
{
	m_FindTargetEagleFlyInfo.m_fMaxRange				= FLT_MAX;
	m_FindTargetEagleFlyInfo.m_fFlyAccelSpeed			= rkLoader.LoadFloat_e( "target_find_fly_accel_speed", 0 );
	m_FindTargetEagleFlyInfo.m_fFlyMinSpeed				= rkLoader.LoadFloat_e( "target_find_fly_min_speed", 0 );
	m_FindTargetEagleFlyInfo.m_fFlyMaxSpeed				= rkLoader.LoadFloat_e( "target_find_fly_max_speed", 0 );
	m_FindTargetEagleFlyInfo.m_dwFlyMinSpeedDuration	= rkLoader.LoadInt_e( "target_find_fly_min_speed_duration", 0 );
}

void ioChangeOnItem::LoadTargetFly( ioINILoader &rkLoader )
{
	m_TargetAttackEagleFlyInfo.m_fFlyAccelSpeed			= rkLoader.LoadFloat_e( "target_fly_accel_speed", 0 );
	m_TargetAttackEagleFlyInfo.m_fFlyMinSpeed			= rkLoader.LoadFloat_e( "target_fly_min_speed", 0 );
	m_TargetAttackEagleFlyInfo.m_fFlyMaxSpeed			= rkLoader.LoadFloat_e( "target_fly_max_speed", 0 );
	m_TargetAttackEagleFlyInfo.m_dwFlyMinSpeedDuration	= rkLoader.LoadInt_e( "target_fly_min_speed_duration", 0 );
}

void ioChangeOnItem::LoadUpFly( ioINILoader &rkLoader )
{
	m_UpFlyInfo.m_fMaxRange				= rkLoader.LoadFloat_e( "up_fly_max_range", 0 );
	m_UpFlyInfo.m_fFlyAccelSpeed		= rkLoader.LoadFloat_e( "up_fly_accel_speed", 0 );
	m_UpFlyInfo.m_fFlyMinSpeed			= rkLoader.LoadFloat_e( "up_fly_min_speed", 0 );
	m_UpFlyInfo.m_fFlyMaxSpeed			= rkLoader.LoadFloat_e( "up_fly_max_speed", 0 );
	m_fUpAngle							= rkLoader.LoadFloat_e( "up_fly_angle", 0 );
	m_UpFlyInfo.m_dwFlyMinSpeedDuration	= rkLoader.LoadInt_e( "up_fly_min_speed_duration", 0 );
}

ioItem* ioChangeOnItem::Clone()
{
	return new ioChangeOnItem( *this );
}

ioWeaponItem::WeaponSubType ioChangeOnItem::GetSubType() const
{
	return WST_CHANGE_ON_ITEM;
}

void ioChangeOnItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	ClearCancelInfo();

	int iReadyCnt = m_AttackReadyAniList.size();
	if( !COMPARE( iCurCombo, 0, iReadyCnt ) )
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

void ioChangeOnItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	switch( m_ChargeState )
	{
	case CS_CHARGING:
		OnCharging( pOwner );
		break;
	case CS_ATTACK_FIRE:
		{
			if( pOwner->IsNeedProcess() )
			{
				CheckCancelReserve( pOwner );

				if( ProcessCancel(pOwner) )
					return;
			}

			ioWeaponItem::CheckNormalAttack( pOwner );
		}
		break;
	}
}

void ioChangeOnItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )	return;

	if( pOwner->IsAttackKeyDown() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			ChangeToRiding( pOwner, WSC_DELAY );
		}
	}
	else
	{
		ChangeToAttackFire( pOwner );
	}
}

void ioChangeOnItem::ChangeToAttackFire( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->InitExtendAttackTagInfo();
	ClearCancelInfo();

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

	m_ChargeState = CS_ATTACK_FIRE;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << m_iCurCombo;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioChangeOnItem::ChangeToRiding( ioBaseChar *pOwner, WerewolfChangeState eState )
{
	if( !pOwner )	return;

	if( pOwner->CheckChangeWereWolfState( ECT_BUBBLE ) && CheckChangeWereWolfState( pOwner ) )
		ChangeToWereWolfState( pOwner, eState );
	else
		ChangeToAttackFire( pOwner );
}

void ioChangeOnItem::ChangeToWereWolfState( ioBaseChar *pOwner, WerewolfChangeState eState )
{
	int iCurIndex = g_DummyCharMgr.GetCurLocalIndex();
	if( !CreateWerewolfDummy( pOwner, iCurIndex, eState ) )
		iCurIndex = 0;

	SetChangeWereWolfState( pOwner, eState );
	
	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_WEREWOLF_STATE );
		kPacket << pOwner->GetCharName();
		kPacket << CHANGE_ON_SET;
		kPacket << m_fMaxChangeDamage;
		kPacket << (int)eState;
		kPacket << iCurIndex;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioChangeOnItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) return;

	int iState;
	rkPacket >> iState;
	rkPacket >> m_iCurCombo;

	switch( iState )
	{
	case CS_ATTACK_FIRE:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			pOwner->SetTargetRot( qtRot );

			ChangeToAttackFire( pOwner );
		}
		break;
	}
}

void ioChangeOnItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	D3DXVECTOR3 vPos;
	D3DXQUATERNION qtRot;
	rkPacket >> vPos >> qtRot;

	pOwner->SetWorldPosition( vPos );
	pOwner->SetWorldOrientation( qtRot );

	int eState;
	rkPacket >> eState;

	switch( eState )
	{
	case SS_TARGET_FIND:
		ChangeToFindTarget( pOwner );
		break;
	case SS_UPDATE_TARGET:
		{
			ioHashString szTarget;
			rkPacket >> szTarget;

			if( pOwner->GetCreator() )
			{
				ioBaseChar* pTarget = pOwner->GetCreator()->GetBaseChar( szTarget );
				if( pTarget )
					ChangeTarget( pOwner, pTarget );
			}
		}
		break;
	case SS_REMOVE_TARGET:
		RemoveTarget( pOwner );
		break;
	case SS_TARGET_ATTACK_PRE:
		{
			ioHashString szCharName;
			rkPacket >> szCharName;

			byte eKey;
			rkPacket >> eKey;

			ChangeToTargetAttackPre( pOwner, pOwner->GetCreator(), szCharName, static_cast<ioUserKeyInput::DirKeyInput>( eKey ) );
		}
		break;
	case SS_TARGET_ATTACK:
		ChangeToTargetAttack( pOwner );
		break;
	case SS_UPDOWN_KEY:
		{
			byte eKey;
			rkPacket >> eKey;
			m_UpDownKey = static_cast<UpDownKey>( eKey );

			int eDirKey;
			rkPacket >> eDirKey;
			m_eDirKey = static_cast<DIRKEY>( eDirKey );
		}
		break;
	case SS_CHANGE_BEAR:
		ChangeToObjectEquip( pOwner );
		break;
	case SS_END:
		ChangeToEnd( pOwner );
		break;
	case SS_GRAP_DELAY:
		{
			rkPacket >> m_GrapplingTargetName;
			SetItemGrapplingState( pOwner, m_GrapplingTargetName );
		}
		break;
	case SS_GRAP_ATTACK:
		ChangeFlyGrapAttack( pOwner );
		break;
	}
}

void ioChangeOnItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	ioAttackableItem::SetExtraInfoToWeapon( pWeapon, bJumpAttack );

	ioBaseChar *pChar = pWeapon->GetOwner();
	ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );

	if( pChar && pZone )
	{
		if( bJumpAttack )
		{
			ioFlashJump *pFlashJump = ToFlashJump( GetCurExtendJump() );
			if( pFlashJump && pFlashJump->IsFlashJumpState() )
			{
				D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
				pZone->SetCollisionDir( vDir );
				pZone->SetZoneValue( true, false );
			}
		}
		else
		{
			D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			pZone->SetCollisionDir( vDir );
			pZone->SetZoneValue( false, false );
		}
	}
}

bool ioChangeOnItem::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		if( m_ChargeState == CS_CHARGING )
			return true;
	}

	return false;
}

void ioChangeOnItem::CheckReserve( ioBaseChar *pOwner )
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

bool ioChangeOnItem::ProcessCancel( ioBaseChar *pOwner )
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

bool ioChangeOnItem::SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType )
{
	if( !pOwner ) return false;

	DWORD dwCurTime = FRAMEGETTIME();
	ClearAirJump();

	AttackType eExtendType = AT_JUMP_ATTACK;

	const AttackAttribute *pAttr = GetAttackAttribute( eExtendType );
	if( !pAttr )
	{
		LOG.PrintTimeAndLog( 0, "ioChangeOnItem::SetJumpAttack - AttackAttribute Not Exist" );
		return false;
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	iAniID = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioChangeOnItem::SetJumpAttack - JumpAttack Animation Not Exist" );
		return false;
	}

	fTimeRate   = pAttr->m_fAttackAniRate;
	DWORD dwPreDelay  = pAttr->m_dwPreDelay;

	// For AirJumpPower
	SetAirJump( pOwner, iAniID, fTimeRate );

	return false;
}

void ioChangeOnItem::ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
	if( m_pExtendJump )
	{
		m_pExtendJump->ProcessJumpAttack( pOwner, fHeightGap );
	}

	CheckAirJump( pOwner );
}

void ioChangeOnItem::CheckIncreaseChangeDamage( ioBaseChar *pOwner )
{
	if( m_bUsedWereWolf )
		return;

	int iState = pOwner->GetState();
	if( iState != CS_DELAY && iState != CS_RUN && iState != CS_DASH )
		return;

	if( m_iCurSpecialAttCnt >= m_iMaxSpecialAttCnt )
		return;

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();
	float fGauge = m_fExtraGaugeBaseInc * fTimePerSec;

	m_fCurExtraGauge += fGauge;
	m_fCurExtraGauge = (float)min( m_fCurExtraGauge, (float)m_fMaxExtraGauge );

	if( m_fCurExtraGauge >= m_fMaxExtraGauge )
	{
		m_iCurSpecialAttCnt++;
		m_iCurSpecialAttCnt = max( 0, min(m_iCurSpecialAttCnt, m_iMaxSpecialAttCnt) );

		m_fCurExtraGauge = 0.f;
	}
}

void ioChangeOnItem::DecreaseChangeDamage( ioBaseChar *pOwner )
{
	if( !m_bUsedWereWolf && m_GriffinDummyCharIndex == 0 )
		return;

	CharState eState = pOwner->GetState();
	switch( eState )
	{
	case CS_FROZEN:
	case CS_ICE_STATE:
	case CS_STOP_MOTION:
		return;
	}

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();
	float fCurDecrease = 0.0f;
	if( m_bUsedWereWolf && m_dwChangeWereWolfTime > 0 )
	{
		ioObjectWereWolfItem *pWereWolfObjItem = ToObjectWereWolfItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
		if( pWereWolfObjItem && pWereWolfObjItem->IsUseGaugeMode() )
			fCurDecrease = m_fUseGaugeDecreaseChangeDamage * fTimePerSec;
		else if( CheckOwnerDownState(pOwner) && m_fDownDecreaseChangeDamage > 0.0f )
			fCurDecrease = m_fDownDecreaseChangeDamage * fTimePerSec;
		else
			fCurDecrease = m_fDecreaseChangeDamage * fTimePerSec;

		m_fCurChangeDamage -= fCurDecrease;
		m_fCurChangeDamage = (float)max( 0, min( m_fCurChangeDamage, m_fMaxChangeDamage ) );
	}
	else if( m_GriffinDummyCharIndex != 0 )
	{
		if( m_fFlyDecreaseChangeDamage != 0.f )
			fCurDecrease = m_fFlyDecreaseChangeDamage * fTimePerSec;
		else
			fCurDecrease = m_fDecreaseChangeDamage * fTimePerSec;

		m_fCurChangeDamage -= fCurDecrease;
		m_fCurChangeDamage = (float)max( 0, min( m_fCurChangeDamage, m_fMaxChangeDamage ) );
	}
	else
	{
		m_fCurChangeDamage = 0.0f;
	}
}

void ioChangeOnItem::ReleseWereWolfState( ioBaseChar *pChar )
{
	ioAttackableItem::ReleseWereWolfState( pChar );

	if( pChar && m_dwWereWolfObjectItem > 0 )
	{
		ioObjectItem *pObjectItem = pChar->GetObject();
		if( pObjectItem && pObjectItem->GetItemCode() == m_dwWereWolfObjectItem )
		{
			//애니메이션 초기화
			if( pChar->GetState() == CS_DELAY )
			{
				pChar->ReleaseObjectItem( __FUNCTION__, CS_DELAY );
			}
			else if( pChar->GetState() == CS_RUN )
			{
				pChar->ReleaseObjectItem( __FUNCTION__, CS_RUN );
			}
			else if( pChar->GetState() == CS_DASH )
			{
				pChar->ReleaseObjectItem( __FUNCTION__, CS_DASH );
			}
			else if( pChar->GetState() == CS_JUMP && !m_bChangeToGriffin )
			{
				pChar->ReleaseObjectItem( __FUNCTION__, CS_FALL );
			}
			else
			{
				pChar->ReleaseObjectItem( __FUNCTION__ );
			}
		}
	}
	m_dwChangeWereWolfTime = 0;

	if( pChar && m_iWerewolfDummyCharIndex > 0 )
	{
		ioDummyChar *pDummy = g_DummyCharMgr.FindDummyChar( pChar, m_iWerewolfDummyCharIndex );
		if( pDummy )
			pDummy->SetDieState( true );

		m_iWerewolfDummyCharIndex = 0;
	}
}

void ioChangeOnItem::ReleseWereWolfStateByOwner( ioBaseChar *pChar )
{
	ReleseWereWolfState( pChar );

	// add buff
	int iBuffCnt = m_vReleaseWereWolfBuffList.size();
	for( int i=0; i < iBuffCnt; ++i )
	{
		ioHashString szBuffName = m_vReleaseWereWolfBuffList[i];
		pChar->AddNewBuff( szBuffName, pChar->GetCharName(), GetName(), NULL );
	}

	// change state & motion
	pChar->SetWereWolfState( m_ReleaseWereWolfAttack.m_AttackAnimation, m_ReleaseWereWolfAttack.m_fAttackAniRate );

	pChar->SetCurJumpPowerAmt( 0.0f );
	pChar->SetForcePowerAmt( 0.0f );
	pChar->SetGravityAmt( 0.0f );

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_ReleaseWereWolfAttack.m_AttackAnimation );
	float fTimeRate = m_ReleaseWereWolfAttack.m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pChar->RefreshFireTimeList( iAniID, m_ReleaseWereWolfAttack.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, 0 );
	pChar->IncreaseWeaponIndexBase();
	pChar->SetReservedSliding( m_ReleaseWereWolfAttack.m_vForceInfoList, iAniID, fTimeRate, 0 );
	pChar->SetBaseAttackExtraAniJump( iAniID, fTimeRate, 0 );
	pChar->SetExtendAttackEndJumpInfo( iAniID, fTimeRate, 0 );
}

void ioChangeOnItem::ReleseWereWolfStateByGauge( ioBaseChar *pChar )
{
	ReleseWereWolfState( pChar );
	
	if( m_szReleaseWereWolfAni.IsEmpty() )
		return;

	// change state & motion
	pChar->SetWereWolfState( m_szReleaseWereWolfAni, m_fReleaseWereWolfAniRate );

	pChar->SetCurJumpPowerAmt( 0.0f );
	pChar->SetForcePowerAmt( 0.0f );
	pChar->SetGravityAmt( 0.0f );

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_szReleaseWereWolfAni );
	float fTimeRate = m_fReleaseWereWolfAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pChar->SetBaseAttackExtraAniJump( iAniID, fTimeRate, 0 );
	pChar->SetExtendAttackEndJumpInfo( iAniID, fTimeRate, 0 );
}

bool ioChangeOnItem::CheckChangeWereWolfState( ioBaseChar *pChar )
{
	if( !pChar )
		return false;
	if( !m_bEnableWereWolf )
		return false;
	if( m_bUsedWereWolf )
		return false;

	if( m_iCurSpecialAttCnt < m_iNeedSpecialAttCnt )
		return false;

	return true;
}

bool ioChangeOnItem::IsEnableGauge()
{
	if( m_iCurSpecialAttCnt < m_iNeedSpecialAttCnt )
		return false;

	return true;
}

void ioChangeOnItem::SetChangeWereWolfState( ioBaseChar *pChar, WerewolfChangeState eState )
{
	if( !pChar ) return;

	pChar->SetCurJumpPowerAmt( 0.0f );
	pChar->SetForcePowerAmt( 0.0f );
	pChar->SetGravityAmt( 0.0f );
	pChar->SetInvisibleState( true, true, true );

	m_ChangeState = eState;
	if( eState == WSC_GRIFFIN )
		pChar->SetWereWolfState( m_ChangeWereWolfAttack.m_AttackAnimation, m_ChangeWereWolfAttack.m_fAttackAniRate );
	else
		pChar->SetWereWolfState( m_ChangeWereWolfAttack.m_AttackAnimation, 0.f );

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_ChangeWereWolfAttack.m_AttackAnimation );
	float fTimeRate = m_ChangeWereWolfAttack.m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pChar->RefreshFireTimeList( iAniID, m_ChangeWereWolfAttack.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, 0 );
	pChar->IncreaseWeaponIndexBase();
	pChar->SetReservedSliding( m_ChangeWereWolfAttack.m_vForceInfoList, iAniID, fTimeRate, 0 );

	// add mesh
	if( pChar->IsMale() )
	{
		int iMeshCnt = m_vWereWolfMeshList.size();
		for( int i=0; i < iMeshCnt; ++i )
		{
			ioHashString szMeshName = m_vWereWolfMeshList[i];
			pChar->AddEntity( szMeshName );
		}
	}
	else
	{
		int iMeshCnt = m_vWereWolfMeshListW.size();
		for( int i=0; i < iMeshCnt; ++i )
		{
			ioHashString szMeshName = m_vWereWolfMeshListW[i];
			pChar->AddEntity( szMeshName );
		}
	}

	// add buff
	int iBuffCnt = m_vWereWolfBuffList.size();
	for( int j=0; j < iBuffCnt; ++j )
	{
		ioHashString szBuffName = m_vWereWolfBuffList[j];
		pChar->AddNewBuff( szBuffName, pChar->GetCharName(), GetName(), NULL );
	}

	// create werewolf objectitem
	ioPlayStage *pStage = g_ItemMaker.GetPlayStage();
	if( pStage && m_dwWereWolfObjectItem > 0 )
	{
		if( pChar->IsNeedSendNetwork() )
		{
			pChar->GetEquipSlot()->SetEquipWaitSlot( ES_OBJECT, true );

			SP2Packet kPacket( CTPK_CREATE_OBJECTITEM );
			kPacket << pChar->GetWorldPosition();
			kPacket << m_dwWereWolfObjectItem;
			kPacket << GetName();
			kPacket << pChar->GetCharName();
			kPacket << true;
			kPacket << static_cast<int>( ioObjectItem::OCT_SOILDER );
			TCPNetwork::SendToServer( kPacket );
		}
		else if( !P2PNetwork::IsNetworkPlaying() || !pStage->IsNetworkMode() )
		{
			ioItem *pItem = pStage->CreateItem( m_dwWereWolfObjectItem );
			if( pItem )
			{
				pItem->ReLoadProperty();			

				ioObjectItem *pObject = ToObjectItem( pItem );
				if( pObject )
				{
					pObject->SetObjectCreateItem( GetName() );
					pObject->SetObejctCreateType( ioObjectItem::OCT_SOILDER );
				}
				pChar->EquipItem( pItem );
			}
		}
	}

	m_bUsedWereWolf = true;
	m_dwChangeWereWolfTime = FRAMEGETTIME();
	DecreaseExtraNeedGauge();
}

bool ioChangeOnItem::CheckOwnerDownState( ioBaseChar *pOwner )
{
	if( !pOwner ) return false;

	if( pOwner->GetHP().m_fCurValue == 0.0f )
		return true;

	return false;
}

void ioChangeOnItem::OnReleased( ioBaseChar *pOwner )
{
	ReleseWereWolfState( pOwner );

	ioAttackableItem::OnReleased( pOwner );
}

void ioChangeOnItem::OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem )
{
	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );

	m_fCurChargeRate = 0.0f;
}

void ioChangeOnItem::ApplyChangeWereWolfState( ioBaseChar *pChar )
{
	if( !pChar ) return;

	// change state & motion
	pChar->SetWereWolfState( m_ChangeWereWolfAttack.m_AttackAnimation, m_ChangeWereWolfAttack.m_fAttackAniRate );
	pChar->SetCurJumpPowerAmt( 0.0f );
	pChar->SetForcePowerAmt( 0.0f );
	pChar->SetGravityAmt( 0.0f );

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_ChangeWereWolfAttack.m_AttackAnimation );
	float fTimeRate = m_ChangeWereWolfAttack.m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pChar->RefreshFireTimeList( iAniID, m_ChangeWereWolfAttack.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, 0 );
	pChar->IncreaseWeaponIndexBase();
	pChar->SetReservedSliding( m_ChangeWereWolfAttack.m_vForceInfoList, iAniID, fTimeRate, 0 );

	// add mesh
	if( pChar->IsMale() )
	{
		int iMeshCnt = m_vWereWolfMeshList.size();
		for( int i=0; i < iMeshCnt; ++i )
		{
			ioHashString szMeshName = m_vWereWolfMeshList[i];
			pChar->AddEntity( szMeshName );
		}
	}
	else
	{
		int iMeshCnt = m_vWereWolfMeshListW.size();
		for( int i=0; i < iMeshCnt; ++i )
		{
			ioHashString szMeshName = m_vWereWolfMeshListW[i];
			pChar->AddEntity( szMeshName );
		}
	}

	// create werewolf objectitem
	ioPlayStage *pStage = g_ItemMaker.GetPlayStage();
	if( pStage && m_dwWereWolfObjectItem > 0 )
	{
		if( pChar->IsNeedSendNetwork() )
		{
			pChar->GetEquipSlot()->SetEquipWaitSlot( ES_OBJECT, true );

			SP2Packet kPacket( CTPK_CREATE_OBJECTITEM );
			kPacket << pChar->GetWorldPosition();
			kPacket << m_dwWereWolfObjectItem;
			kPacket << GetName();
			kPacket << pChar->GetCharName();
			kPacket << true;
			kPacket << static_cast<int>( ioObjectItem::OCT_SOILDER );
			TCPNetwork::SendToServer( kPacket );
		}
		else if( !P2PNetwork::IsNetworkPlaying() || !pStage->IsNetworkMode() )
		{
			ioItem *pItem = pStage->CreateItem( m_dwWereWolfObjectItem );
			if( pItem )
			{
				pItem->ReLoadProperty();
				
				ioObjectItem *pObject = ToObjectItem( pItem );
				if( pObject )
				{
					pObject->SetObjectCreateItem( GetName() );
					pObject->SetObejctCreateType( ioObjectItem::OCT_SOILDER );
				}
				pChar->EquipItem( pItem );
			}
		}
	}

	m_bUsedWereWolf = true;
	m_dwChangeWereWolfTime = FRAMEGETTIME();
}

void ioChangeOnItem::SetExtraGaugeTypeByWeapon( ioBaseChar *pOwner, int iType, int iWGT, float fDamage )
{
	if( !pOwner )
		return;

	if( !m_bUsedWereWolf )
		return;

	float fTypeRate = 0.0f;
	switch( iType )
	{
	case EGT_ATTACK:
		fTypeRate = m_fExtraGaugeAttack;
		break;
	case EGT_ATTACK_DEF:
	case EGT_WOUNDED:
	case EGT_WOUNDED_DEF:
		break;
	}

	if( fTypeRate <= 0.0f )
		return;

	m_fCurChangeDamage += fTypeRate;
	m_fCurChangeDamage = min( m_fCurChangeDamage, m_fMaxChangeDamage );
}

bool ioChangeOnItem::IsRidingState()
{
	if( IsWereWolfState() )
		return true;

	return false;
}

bool ioChangeOnItem::IsGriffinState()
{
	if( m_GriffinDummyCharIndex != 0 )
		return true;

	return false;
}

bool ioChangeOnItem::IsEnableActionStopDelay()
{
	if( IsRidingState() )
		return false;

	return true;
}

bool ioChangeOnItem::CheckReleaseWereWolfState( ioBaseChar *pChar )
{
	if( !m_bUsedWereWolf )
	{
		if( pChar && m_dwWereWolfObjectItem > 0 )
		{
			ioObjectItem *pObjectItem = pChar->GetObject();
			if( pObjectItem && pObjectItem->GetItemCode() == m_dwWereWolfObjectItem )
				ReleseWereWolfState( pChar );
		}
		return false;
	}
	if( GetCurChangeDamage() <= 0 )
	{
		ReleseWereWolfStateByGauge( pChar );
		return true;
	}

	if( pChar )
	{
		int iState = pChar->GetState();
		switch( iState )
		{
		case CS_WOUNDED:
		case CS_BLOW_WOUNDED:
		case CS_ETC_STATE:
		case CS_BOUND_BLOW_WOUNDED:
		case CS_BLOW_EXTEND_WOUND:
		case CS_BLOW_DASH_ATTACK:
		case CS_SKIP_STUN:
		case CS_GRAPPLING_WOUNDED:
		case CS_GRAPPLING_WOUNDED_SKILL:
		case CS_GRAPPLING_WOUNDED_SWING:
		case CS_GRAPPLING_WOUNDED_ITEM:
		case CS_GRAPPLING_WOUNDED_BUFF:
		case CS_GRAPPLING_PUSHED_BUFF:
		case CS_WARP_STATE:
		case CS_FROZEN:
		case CS_CLOSEORDER:
		case CS_FLOAT_STATE:
		case CS_ICE_STATE:
		case CS_UROBORUS:
		case CS_PANIC_MOVE:
		case CS_MAGIC_STUN:
		case CS_LOCKUP_BUFF:
		case CS_ADHESIVE_BUFF:
		case CS_WIND_MAGNETIC:
		case CS_GHOST_STEALER_STATE:
		case CS_TITAN_EXTEND_MOVE:
			ReleseWereWolfState( pChar );
			return true;
		default:
			break;
		}

		if( pChar->IsCatchMode() )
			return true;
	}

	return false;
}

void ioChangeOnItem::ReleaseObjectState( ioBaseChar *pOwner, ioObjectItem* pReleasedObject )
{
	if( pOwner && m_iWerewolfDummyCharIndex > 0 )
	{
		ioDummyChar *pDummy = g_DummyCharMgr.FindDummyChar( pOwner, m_iWerewolfDummyCharIndex );
		if( pDummy )
			pDummy->SetDieState( true );

		m_iWerewolfDummyCharIndex = 0;
	}
	m_ChangeState = WSC_DELAY;
	pOwner->SetInvisibleState( false, true );

	if( pOwner->IsInvisibleFace() )
		pOwner->ShowFaceMesh( false );

	if( m_bChangeToGriffin && pOwner->GetState() == CS_JUMP )
		pOwner->SetState( CS_CHANGE_ON_GRIFFIN_FLY );
}

void ioChangeOnItem::SetChangeToGriffinState( bool bChange )
{
	m_bChangeToGriffin = bChange;
}

int ioChangeOnItem::GetMaxBullet()
{
	return m_iMaxSpecialAttCnt;
}

int ioChangeOnItem::GetCurBullet()
{
	return m_iCurSpecialAttCnt;
}

int ioChangeOnItem::GetNeedBullet()
{
	return m_iNeedSpecialAttCnt;
}

void ioChangeOnItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	iCurBullet = max( 0, min( iCurBullet, m_iMaxSpecialAttCnt ) );
	m_iCurSpecialAttCnt = iCurBullet;
}

void ioChangeOnItem::DecreaseExtraNeedGauge()
{
	if( m_iCurSpecialAttCnt >= m_iMaxSpecialAttCnt )
		m_fCurExtraGauge = 0.0f;

	m_fCurChangeDamage = m_fMaxChangeDamage;
	m_iCurSpecialAttCnt -= m_iNeedSpecialAttCnt;
	m_iCurSpecialAttCnt = max( 0, min(m_iCurSpecialAttCnt, m_iMaxSpecialAttCnt) );
}

bool ioChangeOnItem::SetItemGrapplingState( ioBaseChar *pChar, const ioHashString &szTargetName )
{
	if( !pChar ) return false;

	// 충돌처리 후 체크된 타겟을 m_GrapplingTargetName로 지정한다.
	m_GrapplingTargetName = szTargetName;

	ChangeFlyGrapDelay( pChar );

	ioBaseChar *pTarget = pChar->GetBaseChar( m_GrapplingTargetName );
	if( !pTarget ) return false;

	pTarget->SetItemGrapplingState( pChar->GetCharName(), m_GrapplingInfo );
	return true;
}

void ioChangeOnItem::ProcessFlyState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if( !pOwner->IsAttackKeyDown() )
		m_AttackKeyType = AKT_NONE;

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->SetInvisibleState( true, true, true );

	switch( m_SpecialFlyState )
	{
	case SFS_TARGET_FIND:
		OnFindTarget( pOwner );
		break;
	case SFS_TARGET_ATTACK_PRE:
		OnTargetAttackPre( pOwner );
		break;
	case SFS_TARGET_ATTACK:
		OnTargetAttack( pOwner );
		break;
	case SFS_UP:
		OnUp( pOwner );
		break;
	case SFS_GRAP_DELAY:
		{
			if( pOwner->IsNeedProcess() && ( !CheckGrapplingTargetValidate( pOwner ) || GetCurChangeDamage() <= 0 ) )
			{
				ChangeToEnd( pOwner );
				return;
			}

			ProcessKeyCheck( pOwner );

			//방향키 입력 갱신
			pOwner->SetTargetRotToDirKey( m_eDirKey );
			m_vMoveDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVec3Normalize( &m_vMoveDir, &m_vMoveDir );
			if( ProcessFlyMove( pOwner, m_FindTargetEagleFlyInfo, m_dwStartTime ) == MS_END )
			{
				ChangeToEnd( pOwner );
				return;
			}
			if( pOwner->IsNeedProcess() )
			{
				if( pOwner->IsAttackKey() )
				{
					m_SpecialFlyState = SFS_GRAP_CHARGING;
					m_dwStartTime = dwCurTime;
					return;
				}
			}
		}
		break;
	case SFS_GRAP_CHARGING:
		if( pOwner->IsNeedProcess() )
		{
			if( !CheckGrapplingTargetValidate( pOwner ) )
			{
				ChangeToEnd( pOwner );
				return;
			}

			if( !pOwner->IsAttackKeyDown() )
			{
				ChangeFlyGrapAttack( pOwner );
				return;
			}

			if( m_dwStartTime + m_dwGrapChargeMaxTime < dwCurTime )
			{
				ChangeToObjectEquip( pOwner );
				return;
			}
		}
		if( ProcessFlyMove( pOwner, m_FindTargetEagleFlyInfo, m_dwStartTime ) == MS_END )
		{
			ChangeToEnd( pOwner );
			return;
		}
		break;
	case SFS_GRAP_ATTACK:
		if( dwCurTime > m_dwMotionEndTime )
		{
			ChangeToEnd( pOwner );
			return;
		}
		break;
	}
}

void ioChangeOnItem::OnFindTarget( ioBaseChar* pOwner )
{
	ProcessKeyCheck( pOwner );

	//방향키 입력 갱신
	pOwner->SetTargetRotToDirKey( m_eDirKey );
	m_vMoveDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &m_vMoveDir, &m_vMoveDir );

	int eState = ProcessFlyMove( pOwner, m_FindTargetEagleFlyInfo, m_dwStartTime );
	if( eState == MS_MOVE || eState == MS_COLL )
	{	
		if( !pOwner->IsNeedProcess() )
			return;

		//아이템 게이지가 0 이상이면 타겟을 찾는다.
		if( CheckItemGauge( pOwner ) )
		{
			UpdateTarget( pOwner );
			
			//m_AttackKeyType 타입 사용에 대한 부분은 헤더 참조
			if( m_AttackKeyType == AKT_NONE )
			{
				if( pOwner->IsAttackKeyDown() )
				{
					ChangeToTargetAttackPre( pOwner, pOwner->GetCreator(), m_TargetName, ioUserKeyInput::DKI_NONE );
					return;
				}
			}
		}
		//아이템 게이지가 0이되면 종료
		else
		{	
			ChangeToEnd( pOwner );
		}
	}
}

void ioChangeOnItem::OnTargetAttackPre( ioBaseChar* pOwner )
{
	int eState = ProcessFlyMove( pOwner, m_FindTargetEagleFlyInfo, m_dwStartTime );
	if( !pOwner->IsNeedProcess() )
		return;

	if( eState == MS_END )
	{
		ChangeToEnd( pOwner );
		return;
	}

	if( pOwner->IsAttackKeyDown() )
	{
		if( m_dwCharingStartTime + m_dwFlyChargeTime <= FRAMEGETTIME() )
		{
			ChangeToObjectEquip( pOwner );
		}
	}
	else
	{
		ChangeToTargetAttack( pOwner );
	}
}

void ioChangeOnItem::OnTargetAttack( ioBaseChar* pOwner )
{
	int eState = ProcessFlyMove( pOwner, m_TargetAttackEagleFlyInfo, m_dwStartTime );
	if( eState == MS_END )
	{
		ClearWeapon( pOwner );
		ChangeToUp( pOwner );
	}
}

void ioChangeOnItem::OnUp( ioBaseChar* pOwner )
{
	if( ProcessFlyMove( pOwner, m_UpFlyInfo, m_dwStartTime ) == MS_END )
	{
		ChangeToEnd( pOwner );
	}
}

void ioChangeOnItem::ProcessKeyCheck( ioBaseChar *pOwner, bool bSync /*= true */ )
{
	if( !pOwner->IsNeedProcess() )
		return;

	float fHeightGap = pOwner->GetWorldPosition().y - pOwner->GetBottomHeight();
	UpDownKey eUpDownKey = UDK_NONE;
	if( pOwner->IsJumpKeyDown() )
	{
		eUpDownKey = UDK_UP;
	}
	else if( pOwner->IsDefenseKeyDown() && m_fCheckHeightMin <= fHeightGap )
	{
		eUpDownKey = UDK_DOWN;
	}

	ioUserKeyInput::DirKeyInput eKey = pOwner->CheckCurDirKey();
	bool bDirKeyChange = false;
	if( eKey != m_eDirKey )
	{
		bDirKeyChange = true;
		m_eDirKey = eKey;
	}

	bool bUpDownChange = false;
	if( eUpDownKey != m_UpDownKey )
	{
		bUpDownChange = true;
		m_UpDownKey = eUpDownKey;
	}

	if( bSync && ( bDirKeyChange || bUpDownChange ) && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket	<< pOwner->GetWorldPosition();
		kPacket	<< pOwner->GetWorldOrientation();
		kPacket << (int)SS_UPDOWN_KEY;
		kPacket	<< (byte)m_UpDownKey;
		kPacket << (int)m_eDirKey;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

int ioChangeOnItem::ProcessFlyMove( ioBaseChar *pOwner, const EagleFlyInfo& FlyInfo, DWORD dwStartTime )
{
	if( !pOwner )
		return MS_END;

	D3DXVECTOR3 vUp( 0.0f, FLOAT1, 0.0f );
	D3DXVECTOR3 VDown( 0.0f, -FLOAT1, 0.0f );

	DWORD dwCurTime		= FRAMEGETTIME();
	float fTimePerSec	= g_FrameTimer.GetSecPerFrame();

	//엑셀 가속
	float fCurSpeed = FlyInfo.m_fFlyMinSpeed;
	DWORD dwGapTime = dwCurTime - ( dwStartTime + FlyInfo.m_dwFlyMinSpeedDuration );
	if( dwStartTime + FlyInfo.m_dwFlyMinSpeedDuration < dwCurTime )
	{
		float fTimeRate = (float)dwGapTime / FLOAT1000;
		float fCurAccel = FlyInfo.m_fFlyAccelSpeed * fTimeRate;
		fCurSpeed += fCurAccel;
	}

	//최대 속도 못 넘어가게
	if( fCurSpeed >= FlyInfo.m_fFlyMaxSpeed )
		fCurSpeed = FlyInfo.m_fFlyMaxSpeed;

	float fDistance = fCurSpeed * fTimePerSec;

	//이동 거리 계산 및 이동 종료 체크
	MoveState eRangeLimit = MS_END;
	D3DXVECTOR3 vMoveAmt( 0.0f, 0.0f, 0.0f );
	if( m_fCurrFlyRange + fDistance < FlyInfo.m_fMaxRange )
	{
		eRangeLimit = MS_MOVE;
		vMoveAmt = m_vMoveDir * fDistance;
		m_fCurrFlyRange += fDistance;

		switch( m_UpDownKey )
		{
		case UDK_UP:
			vMoveAmt += vUp * fDistance;
			break;
		case UDK_DOWN:
			vMoveAmt += VDown * fDistance;
			break;
		}
		m_UpDownKey = UDK_NONE;
	}
	else
	{
		eRangeLimit = MS_END;

		float fGapDistance = FlyInfo.m_fMaxRange - m_fCurrFlyRange;
		vMoveAmt = m_vMoveDir * fGapDistance;
	}
	
	bool bCol = false;
	ioPlayStage *pStage = pOwner->GetCreator();
	if( pStage )
	{
		if( pStage->ReCalculateMoveVectorByTerrain( pOwner, &vMoveAmt, bCol ) )
			pOwner->Translate( vMoveAmt );
		else
			return eRangeLimit == MS_END ? MS_END : MS_COLL;
	}

	D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
	if( pOwner->CheckAxisLimit( &vPos ) )
	{
		pOwner->SetWorldPosition( vPos );
	}

	float fHeightGap = pOwner->GetMidPositionByRate().y - pOwner->GetBottomHeight();
	if( fHeightGap <= 15.0f )
	{
		return MS_LANDING;
	}

	return eRangeLimit;
}

bool ioChangeOnItem::CheckItemGauge( ioBaseChar* pOwner )
{
	if( GetCurBullet() <= 0 )
		return false;

	return true;
}

void ioChangeOnItem::UpdateTarget( ioBaseChar* pOwner )
{
	FD_GrapplingAutoTarget kFindPred( pOwner, m_fTargetMinAngle, m_fTargetMaxAngle, m_fTargetMaxRange, false );
	ioBaseChar *pTarget = pOwner->GetCreator()->FindUser( kFindPred );
	if( pTarget )
	{
		//타겟이 바뀌었다면 에임 마커로 교체(갱신)
		if( m_TargetName != pTarget->GetCharName() )
		{
			if( m_dwTargetingStartTime + m_dwTargetingTime < FRAMEGETTIME() )
				ChangeTarget( pOwner, pTarget );
		}
	}
	else
	{
		//타겟이 없다면 레디 마커로 교체
		if( !m_TargetName.IsEmpty() )
		{
			RemoveTarget( pOwner );
		}
	}
}

void ioChangeOnItem::ChangeTarget( ioBaseChar* pOwner, ioBaseChar* pTarget )
{
	m_TargetName			= pTarget->GetCharName();
	m_dwTargetingStartTime	= FRAMEGETTIME();

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket	<< pOwner->GetWorldPosition();
		kPacket	<< pOwner->GetWorldOrientation();
		kPacket << (int)SS_UPDATE_TARGET;
		kPacket << m_TargetName;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioChangeOnItem::RemoveTarget( ioBaseChar* pOwner )
{
	m_TargetName.Clear();

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket	<< pOwner->GetWorldPosition();
		kPacket	<< pOwner->GetWorldOrientation();
		kPacket << (int)SS_REMOVE_TARGET;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioChangeOnItem::ChangeToTargetAttackPre( ioBaseChar* pOwner, ioPlayStage* pStage, const ioHashString& szTargetName, ioUserKeyInput::DirKeyInput eKey )
{
	m_SpecialFlyState		= SFS_TARGET_ATTACK_PRE;
	m_dwTargetingStartTime	= 0;
	m_TargetName			= szTargetName;

	ioBaseChar* pTarget = pStage->GetBaseChar( szTargetName );
	if( pTarget )
	{
		m_vPreDir								= pTarget->GetMidPositionByRate() - pOwner->GetMidPositionByRate();
		m_TargetAttackEagleFlyInfo.m_fMaxRange	= D3DXVec3Length( &m_vPreDir );
	}
	else
	{
		m_vPreDir								= pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		m_TargetAttackEagleFlyInfo.m_fMaxRange	= m_fReadyMarkerOffset;
	}

	if( eKey != ioUserKeyInput::DKI_NONE )
	{
		float fYawD = 180.0f + 45.0f * ( eKey - ioUserKeyInput::DKI_UP );
		fYawD = ioMath::ArrangeHead( fYawD );
		float fYawR = DEGtoRAD( fYawD );
		D3DXQUATERNION qtRotate;
		D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );
		m_vPreDir = qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	}

	D3DXVec3Normalize( &m_vPreDir, &m_vPreDir );
	m_dwCharingStartTime = FRAMEGETTIME();

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket	<< pOwner->GetWorldPosition();
		kPacket	<< pOwner->GetWorldOrientation();
		kPacket << (int)SS_TARGET_ATTACK_PRE;
		kPacket	<< szTargetName;
		kPacket << (byte)eKey;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioChangeOnItem::ChangeToTargetAttack( ioBaseChar* pOwner )
{
	m_SpecialFlyState	= SFS_TARGET_ATTACK;
	m_vMoveDir			= m_vPreDir;

	D3DXVec3Normalize( &m_vMoveDir, &m_vMoveDir );
	
	m_dwStartTime	= FRAMEGETTIME();
	m_fCurrFlyRange	= 0.0f;
	m_TargetName.Clear();

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket	<< pOwner->GetWorldPosition();
		kPacket	<< pOwner->GetWorldOrientation();
		kPacket << (int)SS_TARGET_ATTACK;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	D3DXVECTOR3 vRot = -m_vMoveDir;
	vRot.y = 0.0f;
	D3DXQUATERNION qtRot;
	ioMath::CalcDirectionQuaternion( qtRot, vRot );
	pOwner->SetTargetRot( qtRot );

	ioDruidEagleDummy* pDummy = GetGriffinDummy( pOwner );
	if( pDummy )
		pDummy->SetDownFly();

	m_dwCurrWeaponIdx = pOwner->GetWeaponIndexBase();
	FireTime kFireTime;
	kFireTime.dwStart				= FRAMEGETTIME();
	kFireTime.iAttributeIdx			= m_dwWeaponIdx;
	kFireTime.iResistanceIdx		= 0;
	kFireTime.szWoundedAni			= "";
	kFireTime.dwWoundedAniDuration	= 0;
	kFireTime.bLoopWoundedAni		= false;
	kFireTime.dwWeaponIdx			= m_dwCurrWeaponIdx;
	kFireTime.eFireTimeType			= FTT_EXTEND_ATTACK;

	pOwner->ExtendFireExplicit( kFireTime, pOwner->GetMidPositionByRate(), pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 ), "" );
}

void ioChangeOnItem::ChangeToEnd( ioBaseChar* pOwner )
{
	m_dwStartTime		= FRAMEGETTIME();
	m_SpecialFlyState	= SFS_END;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket	<< pOwner->GetWorldPosition();
		kPacket	<< pOwner->GetWorldOrientation();
		kPacket << (int)SS_END;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioChangeOnItem::ChangeToObjectEquip( ioBaseChar* pOwner )
{
	m_dwStartTime		= FRAMEGETTIME();
	m_SpecialFlyState	= SFS_END;

	ChangeToRiding( pOwner, WSC_GRIFFIN );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket	<< pOwner->GetWorldPosition();
		kPacket	<< pOwner->GetWorldOrientation();
		kPacket << (int)SS_CHANGE_BEAR;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioChangeOnItem::ClearWeapon( ioBaseChar* pOwner )
{
	ioZoneEffectWeapon* pZone = ToZoneEffectWeapon( g_WeaponMgr.FindWeapon( pOwner, m_dwCurrWeaponIdx ) );
	if( pZone )
		pZone->SetWeaponDead();
}

void ioChangeOnItem::ChangeToUp( ioBaseChar* pOwner )
{
	D3DXVECTOR3 vTargetDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vTargetDir, &vTargetDir );

	D3DXVECTOR3 vAxis;
	D3DXVec3Cross( &vAxis, &vTargetDir, &ioMath::UNIT_Y );
	D3DXVec3Normalize( &vAxis, &vAxis );

	D3DXQUATERNION qtNewRot;
	float fCurAngle = ioMath::ArrangeHead( m_fUpAngle );
	D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fCurAngle) );

	m_vMoveDir = qtNewRot * vTargetDir;
	D3DXVec3Normalize( &m_vMoveDir, &m_vMoveDir );

	m_SpecialFlyState	= SFS_UP;
	m_dwStartTime		= FRAMEGETTIME();
	m_fCurrFlyRange		= 0.0f;

	ioDruidEagleDummy* pDummy = GetGriffinDummy( pOwner );
	if( pDummy )
		pDummy->SetUpFly();
}

void ioChangeOnItem::ChangeToFindTarget( ioBaseChar* pOwner )
{
	m_SpecialFlyState	= SFS_TARGET_FIND;
	m_vPreDir			= pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	m_vMoveDir			= m_vPreDir;
	m_dwStartTime		= FRAMEGETTIME();
	m_fCurrFlyRange		= 0.0f;
	
	//더미 애니메이션 변경
	ioDruidEagleDummy* pDummy = GetGriffinDummy( pOwner );
	if( pDummy )
		pDummy->SetFly();

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket	<< pOwner->GetWorldPosition();
		kPacket	<< pOwner->GetWorldOrientation();
		kPacket << (int)SS_TARGET_FIND;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioChangeOnItem::CreateGriffinDummy( ioBaseChar* pOwner )
{
	D3DXVECTOR3 vDir		= pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVECTOR3 vTargetPos	= pOwner->GetMidPositionByRate();
	int iCurIndex			= g_DummyCharMgr.GetCurLocalIndex();
	ioPlayStage *pStage		= pOwner->GetCreator();

	if( pStage )
	{
		ioDummyChar *pDummy = pStage->CreateDummyChar( m_GriffinDummyCharName, iCurIndex, pOwner->GetCharName(), vTargetPos, 0.0f, 0, true );
		if( pDummy )
		{
			pDummy->SetMoveState();
			m_GriffinDummyCharIndex = iCurIndex;
		}
	}
}

void ioChangeOnItem::DestroyGriffinDummy( ioBaseChar* pOwner )
{	
	if( pOwner && m_GriffinDummyCharIndex > 0 )
	{
		ioDummyChar *pDummy = g_DummyCharMgr.FindDummyChar( pOwner, m_GriffinDummyCharIndex );
		if( pDummy )
			pDummy->SetDieState( true );

		m_GriffinDummyCharIndex = 0;
	}
}

ioDruidEagleDummy* ioChangeOnItem::GetGriffinDummy( ioBaseChar* pOwner )
{
	ioDummyChar* pDummyChar = g_DummyCharMgr.FindDummyChar( pOwner, m_GriffinDummyCharIndex );
	return ToDruidEagleDummy( pDummyChar );
}

bool ioChangeOnItem::CreateWerewolfDummy( ioBaseChar *pOwner, int iIndex, WerewolfChangeState eState )
{
	if( !pOwner ) return false;
	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) return false;

	if( pOwner && m_iWerewolfDummyCharIndex > 0 )
	{
		ioDummyChar *pDummy = g_DummyCharMgr.FindDummyChar( pOwner, m_iWerewolfDummyCharIndex );
		if( pDummy )
			pDummy->SetDieState( true );

		m_iWerewolfDummyCharIndex = 0;
	}

	D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVECTOR3 vTargetPos = pOwner->GetWorldPosition();

	ioDummyChar *pDummy = pStage->CreateDummyChar( m_WerewolfDummyCharName, iIndex,
												   pOwner->GetCharName(),
												   vTargetPos,
												   0.0f,
												   0, true );

	if( pDummy )
	{
		m_iWerewolfDummyCharIndex = iIndex;

		if( eState != WSC_GRIFFIN )
			pDummy->SetDelayState( false );

		return true;
	}

	return false;
}

void ioChangeOnItem::SetWerewolfDummyCharIndex( ioBaseChar *pOwner, int iCurIndex )
{
	if( !pOwner ) return;

	ioDummyChar *pDummy = g_DummyCharMgr.FindDummyChar( pOwner, iCurIndex );
	if( pDummy )
	{
		m_iWerewolfDummyCharIndex = iCurIndex;
	}
}

void ioChangeOnItem::InitializeFlyState( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	//더미 생성
	CreateGriffinDummy( pOwner );

	m_SpecialFlyState = SFS_TARGET_FIND;
	m_eDirKey = ioUserKeyInput::DKI_NONE;
	m_UpDownKey = UDK_NONE;
	ProcessKeyCheck( pOwner );

	if( pOwner->IsNeedProcess() )
	{
		FD_GrapplingAutoTarget kFindPred( pOwner, m_fTargetMinAngle, m_fTargetMaxAngle, m_fTargetMaxRange, false );
		ioBaseChar *pTarget = pOwner->GetCreator()->FindUser( kFindPred );

		//시작하자 마자 공격
		if( pTarget )
			ChangeToTargetAttackPre( pOwner, pOwner->GetCreator(), pTarget->GetCharName(), m_eDirKey );
		else
			ChangeToTargetAttackPre( pOwner, pOwner->GetCreator(), "", m_eDirKey );

		ChangeToTargetAttack( pOwner );
	}
	
	//m_AttackKeyType 타입 사용에 대한 부분은 헤더 참조
	if( pOwner->IsAttackKeyDown() )
		m_AttackKeyType = AKT_PRESS;
	else
		m_AttackKeyType = AKT_NONE;

	SetChangeToGriffinState( false );
	DecreaseExtraNeedGauge();
}

void ioChangeOnItem::FinalizeFlyState( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	ClearWeapon( pOwner );
	DestroyGriffinDummy( pOwner );

	if( m_ChangeState != WSC_GRIFFIN )
		pOwner->SetInvisibleState( false, true );

	if( pOwner->IsInvisibleFace() )
		pOwner->ShowFaceMesh( false );
}

bool ioChangeOnItem::IsFlyStateEnd()
{
	 if( m_dwStartTime + 50 < FRAMEGETTIME() && m_SpecialFlyState == SFS_END )
		 return true;

	 return false;
}

void ioChangeOnItem::CheckFallStateChange( ioBaseChar* pOwner, float fBottomHeight )
{	
	if( m_SpecialFlyState == SFS_END )
	{
		if( m_fFlyEndJumpPower > 0.0f )
		{
			pOwner->SetSKillEndJumpState( m_fFlyEndJumpPower );
		}
		else
		{
			float fMinFallinRate = ( pOwner->GetWorldCollisionBox().GetExtents( 1 ) * 2.0f ) * pOwner->GetMinFallHeightRate();
			float fCharPosGap = pOwner->GetWorldPosition().y - fBottomHeight;
			if( fCharPosGap > fMinFallinRate )
				pOwner->SetFallState( true );
		}
	}
}

void ioChangeOnItem::ChangeFlyGrapDelay( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	ioDruidEagleDummy* pDummy = GetGriffinDummy( pChar );
	if( pDummy )
		pDummy->SetFly();
	
	if( pChar->GetState() != CS_CHANGE_ON_GRIFFIN_FLY )
		pChar->SetState( CS_CHANGE_ON_GRIFFIN_FLY );

	m_SpecialFlyState = SFS_GRAP_DELAY;
	m_eDirKey = ioUserKeyInput::DKI_NONE;
	m_dwStartTime = FRAMEGETTIME();

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pChar, kPacket );
		kPacket	<< pChar->GetWorldPosition();
		kPacket	<< pChar->GetWorldOrientation();
		kPacket << SS_GRAP_DELAY;
		kPacket << m_GrapplingTargetName;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioChangeOnItem::CheckGrapplingTargetValidate( ioBaseChar *pOwner )
{
	if( !pOwner ) return false;
	
	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return false;

	ioBaseChar *pTarget = pStage->GetBaseChar( m_GrapplingTargetName );
	if( !pTarget )
		return false;

	if( pTarget->GetState() != CS_GRAPPLING_WOUNDED_ITEM )
		return false;

	return true;
}

void ioChangeOnItem::ChangeFlyGrapAttack( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	pOwner->InitExtendAttackTagInfo();

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	int iAniID = pGrp->GetAnimationIdx( m_GrapAttackAttribute.m_AttackAnimation );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_GrapAttackAttribute.m_fAttackAniRate );
	if( !m_GrapAttackAttribute.m_vWeaponInfoList.empty() )
		pOwner->RefreshFireTimeList( iAniID, m_GrapAttackAttribute.m_vWeaponInfoList, FTT_EXTEND_ATTACK, m_GrapAttackAttribute.m_fAttackAniRate, m_GrapAttackAttribute.m_dwPreDelay );

	m_SpecialFlyState = SFS_GRAP_ATTACK;

	m_dwMotionEndTime = FRAMEGETTIME();
	m_dwMotionEndTime += ( pGrp->GetAnimationFullTime( iAniID ) * m_GrapAttackAttribute.m_fAttackAniRate);
	
	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket	<< pOwner->GetWorldPosition();
		kPacket	<< pOwner->GetWorldOrientation();
		kPacket << SS_GRAP_ATTACK;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

D3DXVECTOR3 ioChangeOnItem::GetGrapplingTargetPos( ioBaseChar *pChar )
{
	D3DXVECTOR3 vPos = pChar->GetMidPositionByRate( m_fGrpplingCharMidPosRate );
	D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vDir, &vDir );

	if( m_fGrpplingCharToCharGap > 0.0f )
		vPos += m_fGrpplingCharToCharGap * vDir;

	return vPos;
}
//-----------------------------------------------------------------------------------------------------------------
ioChangeOnGriffinFlyState::ioChangeOnGriffinFlyState()
{
}

ioChangeOnGriffinFlyState::~ioChangeOnGriffinFlyState()
{
}

void ioChangeOnGriffinFlyState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );

	ioChangeOnItem* pItem = ToChangeOnItem( pOwner->GetWeapon() );
	if( pItem )
		pItem->InitializeFlyState( pOwner );
}

void ioChangeOnGriffinFlyState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );

	ioChangeOnItem* pItem = ToChangeOnItem( pOwner->GetWeapon() );
	if( pItem )
		pItem->FinalizeFlyState( pOwner );
}

bool ioChangeOnGriffinFlyState::IsCharCollisionSkipState( const ioBaseChar *const pOwner, DWORD dwStartTime, DWORD dwEndTime, DWORD dwSkipType, bool bTeamOnly, bool bDefense, TeamType eOwnerTeam, TeamType eOtherTeam ) const
{	
	return true;
}

void ioChangeOnGriffinFlyState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioChangeOnItem* pItem = ToChangeOnItem( pOwner->GetWeapon() );
	if( pItem == NULL )
	{
		pOwner->SetState( CS_DELAY );	
		pOwner->SetFallState( true );
		return;
	}
	else
	{
		pItem->ProcessFlyState( pOwner );
	}
}

bool ioChangeOnGriffinFlyState::IsStateEnd( ioBaseChar* pOwner )
{
	ioChangeOnItem* pItem = ToChangeOnItem( pOwner->GetWeapon() );
	if( pItem )
		return pItem->IsFlyStateEnd();

	 return false;
}

void ioChangeOnGriffinFlyState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{	
	ioChangeOnItem* pItem = ToChangeOnItem( pOwner->GetWeapon() );
	if( pItem )
		return pItem->CheckFallStateChange( pOwner, fBottomHeight );
	else
		pOwner->SetState( CS_DELAY );
}

bool ioChangeOnGriffinFlyState::ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime )
{	
	return true;
}