

#include "stdafx.h"

#include "ioPlayStage.h"
#include "ioNpcChar.h"

#include "ioGrapplingItem.h"

#include "FindPredicateImpl.h"

#include "WeaponDefine.h"

ioGrapplingItem::ioGrapplingItem()
{
	m_ChargeState = CS_NONE;
	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;

	m_bCharged = false;

	m_dwGrapplingPullEndTime = 0;
	m_dwGrapplingMoveEndTime = 0;
	m_dwGrapplingPushEndTime = 0;

	m_bSetGrapplingMoveAni = false;
	m_bTargetRot = false;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	m_dwAniRotateTime = 0;
	m_fAniRotateAngle = 0.0f;

	m_iCurAniRotate = 0;
	m_vAniRotateInfoList.clear();
}

ioGrapplingItem::ioGrapplingItem( const ioGrapplingItem &rhs )
: ioWeaponItem( rhs ),
  m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
  m_vExtendAttributeList( rhs.m_vExtendAttributeList ),
  m_fGrapplingRange( rhs.m_fGrapplingRange ),
  m_fGrapplingMaxAngle( rhs.m_fGrapplingMaxAngle ),
  m_fGrapplingMinAngle( rhs.m_fGrapplingMinAngle ),
  m_fGrapplingMoveSpeedRate( rhs.m_fGrapplingMoveSpeedRate ),
  m_GrapplingPullMotion( rhs.m_GrapplingPullMotion ),
  m_fGrapplingPullMotionRate( rhs.m_fGrapplingPullMotionRate ),
  m_GrapplingPushMotion( rhs.m_GrapplingPushMotion ),
  m_fGrapplingPushMotionRate( rhs.m_fGrapplingPushMotionRate ),
  m_GrapplingMotion( rhs.m_GrapplingMotion ),
  m_GrapplingMoveMotion( rhs.m_GrapplingMoveMotion ),
  m_fGrapplingMoveMotionRate( rhs.m_fGrapplingMoveMotionRate ),
  m_dwGrapplingRotateSpeed( rhs.m_dwGrapplingRotateSpeed ),
  m_fGrapplingGapRange( rhs.m_fGrapplingGapRange ),
  m_GrapplingCancelMotion( rhs.m_GrapplingCancelMotion ),
  m_fGrapplingCancelMotionRate( rhs.m_fGrapplingCancelMotionRate ),
  m_TargetGrapplingInfo( rhs.m_TargetGrapplingInfo ),
  m_PushedGrapplingInfo( rhs.m_PushedGrapplingInfo )
{
	m_ChargeState = CS_NONE;
	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_iCurAirJump = 0;
	m_fAirJumpPowerRate = 0.0f;

	m_bCharged = false;

	m_dwGrapplingPullEndTime = 0;
	m_dwGrapplingMoveEndTime = 0;
	m_dwGrapplingPushEndTime = 0;

	m_bSetGrapplingMoveAni = false;
	m_bTargetRot = false;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	m_dwAniRotateTime = 0;
	m_fAniRotateAngle = 0.0f;

	m_iCurAniRotate = 0;
	m_vAniRotateInfoList.clear();
}

ioGrapplingItem::~ioGrapplingItem()
{
}

void ioGrapplingItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	int i;
	char szBuf[MAX_PATH], szKey[MAX_PATH];

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

	// Grappling
	m_fGrapplingRange = rkLoader.LoadFloat_e( "grappling_target_range", 0.0f );
	m_fGrapplingMaxAngle = rkLoader.LoadFloat_e( "grappling_target_max_angle", 0.0f );
	m_fGrapplingMinAngle = rkLoader.LoadFloat_e( "grappling_target_min_angle", 0.0f );

	// attacker
	rkLoader.LoadString_e( "grappling_pull_motion", "", szBuf, MAX_PATH );
	m_GrapplingPullMotion = szBuf;
	m_fGrapplingPullMotionRate = rkLoader.LoadFloat_e( "grappling_pull_motion_rate", FLOAT1 );

	rkLoader.LoadString_e( "grappling_push_motion", "", szBuf, MAX_PATH );
	m_GrapplingPushMotion = szBuf;
	m_fGrapplingPushMotionRate = rkLoader.LoadFloat_e( "grappling_push_motion_rate", FLOAT1 );

	rkLoader.LoadString_e( "grappling_motion", "", szBuf, MAX_PATH );
	m_GrapplingMotion = szBuf;

	rkLoader.LoadString_e( "grappling_move_motion", "", szBuf, MAX_PATH );
	m_GrapplingMoveMotion = szBuf;
	m_fGrapplingMoveMotionRate = rkLoader.LoadFloat_e( "grappling_move_motion_rate", FLOAT1 );

	rkLoader.LoadString_e( "grappling_cancel_motion", "", szBuf, MAX_PATH );
	m_GrapplingCancelMotion = szBuf;
	m_fGrapplingCancelMotionRate = rkLoader.LoadFloat_e( "grappling_cancel_motion_rate", FLOAT1 );

	// target
	rkLoader.LoadString_e( "target_grappling_pull_motion", "", szBuf, MAX_PATH );
	m_TargetGrapplingInfo.m_TargetGrapplingPullMotion = szBuf;
	m_TargetGrapplingInfo.m_fTargetGrapplingPullMotionRate = rkLoader.LoadFloat_e( "target_grappling_pull_motion_rate", FLOAT1 );

	rkLoader.LoadString_e( "target_grappling_motion", "", szBuf, MAX_PATH );
	m_TargetGrapplingInfo.m_TargetGrapplingMotion = szBuf;

	rkLoader.LoadString_e( "target_grappling_move_motion", "", szBuf, MAX_PATH );
	m_TargetGrapplingInfo.m_TargetGrapplingMoveMotion = szBuf;
	m_TargetGrapplingInfo.m_fTargetGrapplingMoveMotionRate = rkLoader.LoadFloat_e( "target_grappling_move_motion_rate", FLOAT1 );

	rkLoader.LoadString_e( "target_grappling_cancel_motion", "", szBuf, MAX_PATH );
	m_TargetGrapplingInfo.m_TargetGrapplingCancelMotion = szBuf;
	m_TargetGrapplingInfo.m_fTargetGrapplingCancelMotionRate = rkLoader.LoadFloat_e( "target_grappling_cancel_motion_rate", FLOAT1 );

	m_TargetGrapplingInfo.m_dwGrapplingMaxTime = rkLoader.LoadInt_e( "grappling_move_max_time", 0 );
	m_TargetGrapplingInfo.m_dwGrapplingMinTime = rkLoader.LoadInt_e( "grappling_move_min_time", 0 );
	m_TargetGrapplingInfo.m_dwGrapplingFastValue = rkLoader.LoadInt_e( "grappling_move_fast_value", 0 );

	// grappling pushed
	rkLoader.LoadString_e( "grappling_pushed_motion", "", szBuf, MAX_PATH );
	m_PushedGrapplingInfo.m_GrapplingPushedMotion = szBuf;
	m_PushedGrapplingInfo.m_fGrapplingPushedMotionRate = rkLoader.LoadFloat_e( "grappling_pushed_motion_rate", FLOAT1 );

	rkLoader.LoadString_e( "grappling_pushed_loop_motion", "", szBuf, MAX_PATH );
	m_PushedGrapplingInfo.m_GrapplingPushedLoopMotion = szBuf;
	m_PushedGrapplingInfo.m_fGrapplingPushedLoopMotionRate = rkLoader.LoadFloat_e( "grappling_pushed_loop_motion_rate", FLOAT1 );

	rkLoader.LoadString_e( "grappling_pushed_return_motion", "", szBuf, MAX_PATH );
	m_PushedGrapplingInfo.m_GrapplingPushedReturnMotion = szBuf;
	m_PushedGrapplingInfo.m_fGrapplingPushedReturnMotionRate = rkLoader.LoadFloat_e( "grappling_pushed_return_motion_rate", FLOAT1 );

	m_PushedGrapplingInfo.m_dwGrapplingPushedDuration = rkLoader.LoadInt_e( "grappling_pushed_duration", 0 );
	m_PushedGrapplingInfo.m_fGrapplingPushedSpeed = rkLoader.LoadFloat_e( "grappling_pushed_speed", 0.0f );
	m_PushedGrapplingInfo.m_dwGrapplingPushedRotSpeed = rkLoader.LoadInt_e( "grappling_pushed_rotate_speed", 0 );

	m_PushedGrapplingInfo.m_dwGrapplingPushedColTime = rkLoader.LoadInt_e( "grappling_pushed_col_time", 0 );

	int iBuffCnt = rkLoader.LoadInt_e( "grappling_pushed_buff_cnt", 0 );
	m_PushedGrapplingInfo.m_GrapplingPushedBuffList.clear();
	m_PushedGrapplingInfo.m_GrapplingPushedBuffList.reserve( iBuffCnt );
	for( i=0; i < iBuffCnt; ++i )
	{
		wsprintf_e( szKey, "grappling_pushed_buff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

		m_PushedGrapplingInfo.m_GrapplingPushedBuffList.push_back( szBuf );
	}

	iBuffCnt = rkLoader.LoadInt_e( "grappling_col_buff_cnt", 0 );
	m_PushedGrapplingInfo.m_GrapplingColBuffList.clear();
	m_PushedGrapplingInfo.m_GrapplingColBuffList.reserve( iBuffCnt );
	for( i=0; i < iBuffCnt; ++i )
	{
		wsprintf_e( szKey, "grappling_col_buff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

		m_PushedGrapplingInfo.m_GrapplingColBuffList.push_back( szBuf );
	}

	// Grappling etc
	m_dwGrapplingRotateSpeed = rkLoader.LoadInt_e( "grappling_rotate_speed", 0 );
	m_fGrapplingGapRange = rkLoader.LoadFloat_e( "grappling_gap_range", 0.0f );
	m_fGrapplingMoveSpeedRate = rkLoader.LoadFloat_e( "grappling_move_speed_rate", FLOAT1 );
}

ioItem* ioGrapplingItem::Clone()
{
	return new ioGrapplingItem( *this );
}

ioWeaponItem::WeaponSubType ioGrapplingItem::GetSubType() const
{
	return WST_GRAPPLING;
}

void ioGrapplingItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	ClearCancelInfo();

	m_bCharged = false;
	m_bSetGrapplingMoveAni = false;
	m_bTargetRot = false;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	m_GrapplingTargetName.Clear();
	m_CurGrapplingTargetName.Clear();

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

		// Find Grappling Target
		FindGrappling( pOwner );
	}
}

void ioGrapplingItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	switch( m_ChargeState )
	{
	case CS_CHARGING:
		OnCharging( pOwner );
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
}

void ioGrapplingItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )	return;

	if( pOwner->IsAttackKeyDown() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			ChangeToAttackFire( pOwner, true );
			g_TutorialMgr.ActionChargeAttack();
		}
	}
	else
	{
		ChangeToAttackFire( pOwner, false );
	}
}

void ioGrapplingItem::ChangeToAttackFire( ioBaseChar *pOwner, bool bCharged )
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
		m_GrapplingTargetName.Clear();
		m_CurGrapplingTargetName.Clear();

		const AttackAttribute *pAttribute = m_FormalAttack.GetAttribute( AT_NORMAL, m_iCurCombo );
		if( pAttribute )
		{
			pOwner->SetNormalAttackByAttribute( *pAttribute );
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
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioGrapplingItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) return;

	bool bCharged;
	rkPacket >> m_iCurCombo;
	rkPacket >> bCharged;

	D3DXQUATERNION qtRot;
	rkPacket >> qtRot;
	pOwner->SetTargetRot( qtRot );

	ChangeToAttackFire( pOwner, bCharged );
}

void ioGrapplingItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	D3DXQUATERNION qtRot;
	rkPacket >> qtRot;

	m_bTargetRot = false;
	pOwner->SetTargetRot( qtRot );

	int iDir;
	rkPacket >> iDir;

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

void ioGrapplingItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	ioAttackableItem::SetExtraInfoToWeapon( pWeapon, bJumpAttack );

	if( pWeapon && pWeapon->CheckGrapplingState() == GPT_WRESTLING && !m_GrapplingTargetName.IsEmpty() )
	{
		pWeapon->SetGrapplingTargetName( m_GrapplingTargetName );
	}
	else if( pWeapon && pWeapon->CheckGrapplingState() == GPT_WRESTLING_ALL && !m_GrapplingTargetName.IsEmpty() )
	{
		pWeapon->SetGrapplingTargetName( m_GrapplingTargetName );
	}

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

bool ioGrapplingItem::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		switch( m_ChargeState )
		{
		case CS_CHARGING:
			return true;
		case CS_ATTACK_FIRE:
			if( m_bCharged )
				return true;
			break;
		}
	}

	return false;
}

bool ioGrapplingItem::IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck )
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

void ioGrapplingItem::CheckReserve( ioBaseChar *pOwner )
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

bool ioGrapplingItem::ProcessCancel( ioBaseChar *pOwner )
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

bool ioGrapplingItem::SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType )
{
	if( !pOwner ) return false;

	DWORD dwCurTime = FRAMEGETTIME();
	ClearAirJump();

	AttackType eExtendType = AT_JUMP_ATTACK;

	const AttackAttribute *pAttr = GetAttackAttribute( eExtendType );
	if( !pAttr )
	{
		LOG.PrintTimeAndLog( 0, "ioGrapplingItem::SetJumpAttack - AttackAttribute Not Exist" );
		return false;
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	iAniID = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioGrapplingItem::SetJumpAttack - JumpAttack Animation Not Exist" );
		return false;
	}

	fTimeRate   = pAttr->m_fAttackAniRate;
	DWORD dwPreDelay  = pAttr->m_dwPreDelay;

	// For AirJumpPower
	SetAirJump( pOwner, iAniID, fTimeRate );

	return false;
}

void ioGrapplingItem::ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
	if( m_pExtendJump )
	{
		m_pExtendJump->ProcessJumpAttack( pOwner, fHeightGap );
	}

	CheckAirJump( pOwner );
}

void ioGrapplingItem::FindGrappling( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) return;

	m_GrapplingTargetName.Clear();
	FD_GrapplingAutoTarget kAutoPred( pOwner, m_fGrapplingMinAngle, m_fGrapplingMaxAngle, m_fGrapplingRange );

	ioBaseChar *pTarget = pStage->FindUser( kAutoPred );
	if( pTarget )
	{
		m_GrapplingTargetName = pTarget->GetCharName();
	}
}

bool ioGrapplingItem::IsEnableGrapplingState( ioBaseChar *pOwner, const ioHashString &szTarget )
{
	if( !IsChargeAttackState( pOwner ) )
		return false;

	if( m_GrapplingTargetName.IsEmpty() )
		return false;

	if( m_GrapplingTargetName == szTarget )
		return true;

	return false;
}

bool ioGrapplingItem::IsEnableGrapplingMoveState( ioBaseChar *pOwner )
{
	if( !pOwner ) return false;

	if( pOwner->GetState() != CS_GRAPPLING )
		return false;

	switch( m_GrapplingState )
	{
	case GS_MOVE:
		if(	m_CurDirKey != ioUserKeyInput::DKI_NONE )		
			return true;
	}

	return false;
}

void ioGrapplingItem::SetGrapplingState( ioBaseChar *pOwner, const ioHashString &szTarget )
{
	if( !pOwner ) return;
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	float fAniRate = FLOAT1;
	if( m_fGrapplingPullMotionRate > 0.0f )
		fAniRate = m_fGrapplingPullMotionRate;

	int iAniID = pGrp->GetAnimationIdx( m_GrapplingPullMotion );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fAniRate );

	m_GrapplingState = GS_PULL;
	m_dwGrapplingPullEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fAniRate;

	m_GrapplingTargetName.Clear();
	m_CurGrapplingTargetName = szTarget;

	SetAniRotate( pOwner, iAniID, fAniRate );

	pOwner->SetState( CS_GRAPPLING );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_GRAPPLING_SYNC );
		kPacket << pOwner->GetCharName();
		kPacket << GRAPPLING_ATTACKER;
		kPacket << GS_PULL;
		kPacket << szTarget;
		kPacket << GetItemCode();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioGrapplingItem::ChangeGrapplingMove( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	pGrp->ClearAllActionAni( FLOAT10, true );
	pGrp->ClearAllLoopAni( FLOAT10, true );

	int iAniID = pGrp->GetAnimationIdx( m_GrapplingMotion );
	pGrp->SetLoopAni( iAniID, FLOAT10 );

	m_GrapplingState = GS_MOVE;
	m_dwGrapplingMoveEndTime = dwCurTime + m_TargetGrapplingInfo.m_dwGrapplingMaxTime;

	D3DXQUATERNION qtCurRot = pOwner->GetTargetRot();

	D3DXQUATERNION qtRot;
	D3DXQuaternionRotationAxis( &qtRot, &ioMath::UNIT_Y, DEGtoRAD(180.0f) );
	D3DXVECTOR3 vMoveDir = qtRot * pOwner->GetMoveDir();

	D3DXQUATERNION qtTargetRot;
	ioMath::CalcDirectionQuaternion( qtTargetRot, -vMoveDir );

	pOwner->SetTargetRotToRotate( qtTargetRot, true, false );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_GRAPPLING_SYNC );
		kPacket << pOwner->GetCharName();
		kPacket << GRAPPLING_ATTACKER;
		kPacket << GS_MOVE;
		kPacket << qtCurRot;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	ioBaseChar *pTarget = pOwner->GetBaseChar( m_CurGrapplingTargetName );
	if( !pTarget ) return;

	// for NPC
	bool bNPC = false;
	if( ToNpcChar( pTarget ) )
	{
		bNPC = true;
	}

	// local or 훈련소 or NPC
	if( !P2PNetwork::IsNetworkPlaying() || !pOwner->GetCreator()->IsNetworkMode() || bNPC )
	{
		if( pTarget->GetState() == CS_GRAPPLING_WOUNDED )
		{
			pTarget->ChangeWoundGrapplingMoveState();
		}
	}
}

void ioGrapplingItem::ChangeGrapplingPush( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	float fAniRate = FLOAT1;
	if( m_fGrapplingPushMotionRate > 0.0f )
		fAniRate = m_fGrapplingPushMotionRate;

	int iAniID = pGrp->GetAnimationIdx( m_GrapplingPushMotion );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fAniRate );

	m_GrapplingState = GS_PUSH;
	m_dwGrapplingPushEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fAniRate;

	m_GrapplingTargetName.Clear();

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_GRAPPLING_SYNC );
		kPacket << pOwner->GetCharName();
		kPacket << GRAPPLING_ATTACKER;
		kPacket << GS_PUSH;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	ioBaseChar *pTarget = pOwner->GetBaseChar( m_CurGrapplingTargetName );
	if( !pTarget ) return;

	// for NPC
	bool bNPC = false;
	if( ToNpcChar( pTarget ) )
	{
		bNPC = true;
	}

	// local or 훈련소 or NPC
	if( !P2PNetwork::IsNetworkPlaying() || !pOwner->GetCreator()->IsNetworkMode() || bNPC )
	{
		ioBaseChar *pTarget = pOwner->GetBaseChar( m_CurGrapplingTargetName );
		if( pTarget->GetState() == CS_GRAPPLING_WOUNDED )
		{
			pTarget->SetTargetRotToRotate( pOwner->GetTargetRot(), true, false );
			pTarget->SetGrapplingPushedState( GetPushedGrapplingInfo() );
		}
	}
}

void ioGrapplingItem::ChangeGrapplingCancel( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	if( m_GrapplingState == GS_CANCEL )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	float fAniRate = FLOAT1;
	if( m_fGrapplingCancelMotionRate > 0.0f )
		fAniRate = m_fGrapplingCancelMotionRate;

	int iAniID = pGrp->GetAnimationIdx( m_GrapplingCancelMotion );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fAniRate );

	m_GrapplingState = GS_CANCEL;
	m_dwGrapplingCancelEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fAniRate;

	m_GrapplingTargetName.Clear();

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_GRAPPLING_SYNC );
		kPacket << pOwner->GetCharName();
		kPacket << GRAPPLING_ATTACKER;
		kPacket << GS_CANCEL;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioGrapplingItem::CheckGrapplingState( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	if( m_GrapplingState != GS_CANCEL )
	{
		ioBaseChar *pTarget = pOwner->GetBaseChar( m_CurGrapplingTargetName );
		if( pTarget )
		{
			if( m_GrapplingState == GS_MOVE &&
				(pTarget->GetState() != CS_GRAPPLING_WOUNDED || pTarget->GetWoundGrapplingState() == GS_CANCEL) )
			{
				// cancel
				ChangeGrapplingCancel( pOwner );
				return;
			}
		}
		else
		{
			// cancel
			ChangeGrapplingCancel( pOwner );
			return;
		}
	}

	switch( m_GrapplingState )
	{
	case GS_PULL:
		OnGrapplingPull( pOwner );
		break;
	case GS_MOVE:
		OnGrapplingMove( pOwner );
		break;
	case GS_PUSH:
		OnGrapplingPush( pOwner );
		break;
	case GS_CANCEL:
		OnGrapplingCancel( pOwner );
		break;
	}
}

void ioGrapplingItem::OnGrapplingPull( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwGrapplingPullEndTime < dwCurTime )
	{
		// change move state
		ChangeGrapplingMove( pOwner );
		return;
	}
}

void ioGrapplingItem::OnGrapplingMove( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( pOwner->IsNeedProcess() && !pOwner->IsAttackKeyDown() )
	{
		// change push state
		ChangeGrapplingPush( pOwner );
		return;
	}
	else if( m_dwGrapplingMoveEndTime < dwCurTime )
	{
		// change cancel
		ChangeGrapplingCancel( pOwner );
		return;
	}
	else
	{
		CheckGrapplingMoveKey( pOwner );
		CheckGrapplingMoveDir( pOwner );
		CheckGrapplingMoveAni( pOwner );
	}
}

void ioGrapplingItem::OnGrapplingPush( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwGrapplingPushEndTime < dwCurTime )
	{
		// change end state
		pOwner->SetState( CS_DELAY );
		return;
	}
}

void ioGrapplingItem::OnGrapplingCancel( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwGrapplingCancelEndTime < dwCurTime )
	{
		pOwner->SetState( CS_DELAY );
		return;
	}
}

void ioGrapplingItem::CheckGrapplingMoveKey( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() )
		return;

	ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
	if( eNewDirKey != m_CurDirKey )
	{
		m_bTargetRot = false;
		m_CurDirKey = eNewDirKey;

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << pOwner->GetTargetRot();
			kPacket << (int)m_CurDirKey;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioGrapplingItem::CheckGrapplingMoveAni( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = 0;

	if( m_CurDirKey != ioUserKeyInput::DKI_NONE )
	{
		if( !m_bSetGrapplingMoveAni )
		{
			iAniID = pGrp->GetAnimationIdx(m_GrapplingMoveMotion);
			if( iAniID == -1 )
				return;

			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->ClearAllLoopAni( FLOAT100, true );

			float fCurRate = FLOAT1;
			if( m_fGrapplingMoveMotionRate > 0.0f )
				fCurRate = m_fGrapplingMoveMotionRate;

			pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fCurRate );
			m_bSetGrapplingMoveAni = true;
		}
	}
	else
	{
		if( m_bSetGrapplingMoveAni )
		{
			iAniID = pGrp->GetAnimationIdx( m_GrapplingMotion );
			if( iAniID == -1 )
				return;

			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->ClearAllLoopAni( FLOAT100, true );

			pGrp->SetLoopAni( iAniID, FLOAT100 );
			m_bSetGrapplingMoveAni = false;
		}
	}
}

void ioGrapplingItem::CheckGrapplingMoveDir( ioBaseChar *pOwner )
{
	if( m_dwGrapplingRotateSpeed == 0 )
		return;

	if( m_CurDirKey == ioUserKeyInput::DKI_NONE )
		return;

	if( m_bTargetRot )
		return;

	float fFrameGap = g_FrameTimer.GetCurFrameGap();
	float fRate = fFrameGap / m_dwGrapplingRotateSpeed;
	float fRotateAngle = 360.0f * fRate;

	D3DXQUATERNION qtCurRot = pOwner->GetTargetRot();
	D3DXVECTOR3 vCurMoveDir = qtCurRot * D3DXVECTOR3(0.0f, 0.0f, -FLOAT1);
	D3DXVec3Normalize( &vCurMoveDir, &vCurMoveDir );

	// 목표에 도달체크 및 처리
	float fYawD = 180.0f + 45.0f * ( m_CurDirKey - ioUserKeyInput::DKI_UP );
	fYawD = ioMath::ArrangeHead( fYawD );

	float fYawR = DEGtoRAD( fYawD );
	D3DXQUATERNION qtRotate;
	D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

	D3DXVECTOR3 vTargetDir = qtRotate * D3DXVECTOR3(0.0f, 0.0f, -FLOAT1);
	D3DXVec3Normalize( &vTargetDir, &vTargetDir );

	float fDotValue = D3DXVec3Dot( &vCurMoveDir, &vTargetDir );
	float fGapAngle = RADtoDEG( acosf(fDotValue) );

	if( fDotValue >= FLOAT1 )
	{
		pOwner->SetTargetRotToRotate( qtRotate, true );
		return;
	}

	D3DXVECTOR3 vAxis;
	if( fDotValue <= -FLOAT1 )
	{
		vAxis = ioMath::UNIT_Y;
	}
	else
	{
		D3DXVec3Cross( &vAxis, &vTargetDir, &vCurMoveDir );
		D3DXVec3Normalize( &vAxis, &-vAxis );
	}

	D3DXQUATERNION qtNewRot;
	if( fGapAngle < fRotateAngle )
		D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fGapAngle) );
	else
		D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fRotateAngle) );

	D3DXVECTOR3 vNewDir = qtNewRot * vCurMoveDir;
	D3DXVec3Normalize( &vNewDir, &vNewDir );

	ioMath::CalcDirectionQuaternion( qtNewRot, -vNewDir );
	pOwner->SetTargetRotToRotate( qtNewRot, true );
}

void ioGrapplingItem::GetGrapplingMoveInfo( OUT int &iCurDir, OUT bool &bSetMotion )
{
	iCurDir = m_CurDirKey;
	bSetMotion = m_bSetGrapplingMoveAni;
}







