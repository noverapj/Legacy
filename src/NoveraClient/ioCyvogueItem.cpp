
#include "stdafx.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "ioCyvogueItem.h"

#include "WeaponDefine.h"
#include "ItemDefine.h"

ioCyvogueItem::ioCyvogueItem()
{
	m_ChargeState = CS_NONE;

	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_CurCyvogueInfo.Init();
	m_fCurCyvogueGauge = 0.0f;

	m_CyvogueState = CFS_NONE;
	m_CuvogueModeChargeState = CMC_NONE;

	m_dwStartChargingTime = 0;
	m_dwModeCancleTime = 0;
	m_bAddEntityAndBuff = false;

	m_vCirclePos = ioMath::VEC3_ZERO;
}

ioCyvogueItem::ioCyvogueItem( const ioCyvogueItem &rhs )
	: ioWeaponItem( rhs ),
	m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
	m_AttackCyvogueInfo( rhs.m_AttackCyvogueInfo ),
	m_fMaxCyvogueGauge( rhs.m_fMaxCyvogueGauge ),
	m_fNeedCyvogueGauge( rhs.m_fNeedCyvogueGauge ),
	m_fIncreaseCyvogueGauge( rhs.m_fIncreaseCyvogueGauge ),
	m_fDecreaseCyvogueGauge( rhs.m_fDecreaseCyvogueGauge ),
	m_fHighDecreaseHeight( rhs.m_fHighDecreaseHeight ),
	m_fHighDecreaseCyvogueGauge( rhs.m_fHighDecreaseCyvogueGauge ),
	//m_fShotDecreaseGauge( rhs.m_fShotDecreaseGauge ),
	m_AimUp_A( rhs.m_AimUp_A ),
	m_AimCenter_A( rhs.m_AimCenter_A ),
	m_AimDown_A( rhs.m_AimDown_A ),
	m_AimUp_S( rhs.m_AimUp_S ),
	m_AimCenter_S( rhs.m_AimCenter_S ),
	m_AimDown_S( rhs.m_AimDown_S ),

	m_stAmodeFireCenter( rhs.m_stAmodeFireCenter ),
	m_stAmodeFireUp( rhs.m_stAmodeFireUp ),
	m_stAmodeFireDown( rhs.m_stAmodeFireDown ),

	m_stSmodeFireCenter( rhs.m_stSmodeFireCenter ),
	m_stSmodeFireUp( rhs.m_stSmodeFireUp ),
	m_stSmodeFireDown( rhs.m_stSmodeFireDown ),

	m_fACircleRangeMin( rhs.m_fACircleRangeMin ),
	m_fACircleRangeMax( rhs.m_fACircleRangeMax ),
	m_fACircleMoveSpeed( rhs.m_fACircleMoveSpeed ),
	m_fACircleHeightGap( rhs.m_fACircleHeightGap ),
	m_fACircleOffSet( rhs.m_fACircleOffSet ),

	m_fSCircleRangeMin( rhs.m_fSCircleRangeMin ),
	m_fSCircleRangeMax( rhs.m_fSCircleRangeMax ),
	m_fSCircleMoveSpeed( rhs.m_fSCircleMoveSpeed ),
	m_fSCircleHeightGap( rhs.m_fSCircleHeightGap ),
	m_fSCircleOffSet( rhs.m_fSCircleOffSet ),

	m_AmodeReadyCircle( rhs.m_AmodeReadyCircle ),
	m_SmodeReadyCircle( rhs.m_SmodeReadyCircle ),
	m_RedEffect( rhs.m_RedEffect ),
	m_BlueEffect( rhs.m_BlueEffect ),

	m_dwChargeCancelTime( rhs.m_dwChargeCancelTime ),
	m_dwCharge_a_mode_time( rhs.m_dwCharge_a_mode_time ),
	m_dwCharge_s_mode_time( rhs.m_dwCharge_s_mode_time),

	m_DmodeAttackAniInfo( rhs.m_DmodeAttackAniInfo ),
	m_fAimAniWeightRate( rhs. m_fAimAniWeightRate ),
	m_fFireAniWeightRate( rhs.m_fFireAniWeightRate ),

	m_vAddMeshList( rhs.m_vAddMeshList ),
	m_vAddBuffList( rhs.m_vAddBuffList ),

	m_iRandomRange( rhs.m_iRandomRange ),
	m_fWeaponGapDistance( rhs.m_fWeaponGapDistance ),
	m_fCircleRange( rhs.m_fCircleRange ),
	m_fWeaponFloatPower( rhs.m_fWeaponFloatPower ),
	m_fWeaponFloatMinPower( rhs.m_fWeaponFloatMinPower ),
	m_fWeaponGravityRate( rhs.m_fWeaponGravityRate ),
	m_fRevisionRate( rhs.m_fRevisionRate ),
	m_fWeaponMinSpeed( rhs.m_fWeaponMinSpeed ),
	m_fUpLimitHeight( rhs.m_fUpLimitHeight ),
	m_fUpLimitRange( rhs.m_fUpLimitRange ),

	m_fDecreaseGauge_Amode( rhs.m_fDecreaseGauge_Amode ),
	m_fDecreaseGauge_Smode( rhs.m_fDecreaseGauge_Smode ),
	m_fDecreaseGauge_Dmode( rhs.m_fDecreaseGauge_Dmode ),

	m_FailMotion( rhs.m_FailMotion ),
	m_fFailMotionRate( rhs.m_fFailMotionRate ),

	m_dwMaxAModeDelayTime( rhs.m_dwMaxAModeDelayTime ),
	m_dwMaxSModeDelayTime( rhs.m_dwMaxSModeDelayTime ),

	m_fGravity( rhs.m_fGravity ),

	m_stTargetCircleA( rhs.m_stTargetCircleA ),
	m_stTargetCircleS( rhs.m_stTargetCircleS ),
	m_dwTargetCircleMaxTime( rhs.m_dwTargetCircleMaxTime )
{
	m_ChargeState = CS_NONE;

	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_CurCyvogueInfo.Init();
	m_fCurCyvogueGauge = 0.0f;

	m_CyvogueState = CFS_NONE;
}

ioCyvogueItem::~ioCyvogueItem()
{
}

void ioCyvogueItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	int iMaxCombo = GetMaxCombo();
	m_AttackReadyAniList.clear();
	for( int i=0; i < iMaxCombo; ++i )
	{
		wsprintf_e( szBuf, "attack_ready_animation%d", i+1 );
		rkLoader.LoadString( szBuf, "", szBuf, MAX_PATH );
		m_AttackReadyAniList.push_back( ioHashString(szBuf) );
	}

	LoadCyvogueInfo( rkLoader );

	m_fMaxCyvogueGauge = rkLoader.LoadFloat_e( "max_gauge", FLOAT100 );
	m_fNeedCyvogueGauge = rkLoader.LoadFloat_e( "need_gauge", FLOAT100 );

	m_fIncreaseCyvogueGauge = rkLoader.LoadFloat_e( "increase_gauge", FLOAT1 );
	m_fDecreaseCyvogueGauge = rkLoader.LoadFloat_e( "decrease_gauge", 2.0f );

	m_fHighDecreaseHeight = rkLoader.LoadFloat_e( "high_decrease_height", FLOAT100 );
	m_fHighDecreaseCyvogueGauge = rkLoader.LoadFloat_e( "high_decrease_gauge", 3.0f );

	//m_fShotDecreaseGauge = rkLoader.LoadFloat_e( "shot_decrease_gauge", FLOAT1 );
	m_fAttackSpeedRate = rkLoader.LoadFloat_e( "attack_max_speed_rate", FLOAT1 );
}

void ioCyvogueItem::LoadCyvogueInfo( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];
	char szKey[MAX_PATH];
	m_AttackCyvogueInfo.Init();

	//D모드
	wsprintf_e( szBuf, "cyvogue_attack" );
	LoadAttackAttribute( szBuf, m_AttackCyvogueInfo.m_D_Attack, rkLoader );
	rkLoader.LoadString_e( "cyvogue_attack_center", "", szBuf, MAX_PATH );
	m_DmodeAttackAniInfo.m_CenterAttack = szBuf;
	rkLoader.LoadString_e( "cyvogue_attack_front", "", szBuf, MAX_PATH );
	m_DmodeAttackAniInfo.m_FrontAttack = szBuf;
	rkLoader.LoadString_e( "cyvogue_attack_back", "", szBuf, MAX_PATH );
	m_DmodeAttackAniInfo.m_BackAttack = szBuf;
	rkLoader.LoadString_e( "cyvogue_attack_left", "", szBuf, MAX_PATH );
	m_DmodeAttackAniInfo.m_LeftAttack = szBuf;
	rkLoader.LoadString_e( "cyvogue_attack_right", "", szBuf, MAX_PATH );
	m_DmodeAttackAniInfo.m_RightAttack = szBuf;

	wsprintf_e( szBuf, "cyvogue_attack_a_mode" );
	LoadAttackAttribute( szBuf, m_AttackCyvogueInfo.m_A_Attack, rkLoader );
	rkLoader.LoadString_e( "cyvogue_attack_a_mode", "", szBuf, MAX_PATH );
	m_stAmodeFireCenter = szBuf;
	rkLoader.LoadString_e( "cyvogue_attack_a_mode_up", "", szBuf, MAX_PATH );
	m_stAmodeFireUp = szBuf;
	rkLoader.LoadString_e( "cyvogue_attack_a_mode_down", "", szBuf, MAX_PATH );
	m_stAmodeFireDown = szBuf;

	wsprintf_e( szBuf, "cyvogue_attack_s_mode" );
	LoadAttackAttribute( szBuf, m_AttackCyvogueInfo.m_S_Attack, rkLoader );
	rkLoader.LoadString_e( "cyvogue_attack_s_mode", "", szBuf, MAX_PATH );
	m_stSmodeFireCenter = szBuf;
	rkLoader.LoadString_e( "cyvogue_attack_s_mode_up", "", szBuf, MAX_PATH );
	m_stSmodeFireUp = szBuf;
	rkLoader.LoadString_e( "cyvogue_attack_s_mode_down", "", szBuf, MAX_PATH );
	m_stSmodeFireDown = szBuf;

	m_fAimAniWeightRate = rkLoader.LoadFloat_e( "cyvogue_aim_ani_weight_rate", FLOAT05 );
	m_fFireAniWeightRate = rkLoader.LoadFloat_e( "cyvogue_fire_ani_weight_rate", FLOAT05 );
	//////////////////////////////////////////////////////////////////////////
	rkLoader.LoadString_e( "cyvogue_start_ani", "", szBuf, MAX_PATH );
	m_AttackCyvogueInfo.m_StartAni = szBuf;
	m_AttackCyvogueInfo.m_fStartAniRate = rkLoader.LoadFloat_e( "cyvogue_start_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "cyvogue_end_ani", "", szBuf, MAX_PATH );
	m_AttackCyvogueInfo.m_EndAni = szBuf;
	m_AttackCyvogueInfo.m_fEndAniRate = rkLoader.LoadFloat_e( "cyvogue_end_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "cyvogue_delay_ani", "", szBuf, MAX_PATH );
	m_AttackCyvogueInfo.m_DelayAni = szBuf;
	m_AttackCyvogueInfo.m_fDelayAniRate = rkLoader.LoadFloat_e( "cyvogue_delay_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "cyvogue_move_ani_fr", "", szBuf, MAX_PATH );
	m_AttackCyvogueInfo.m_MoveAniFr = szBuf;
	m_AttackCyvogueInfo.m_fMoveAniFrRate = rkLoader.LoadFloat_e( "cyvogue_move_ani_rate_fr", FLOAT1 );

	rkLoader.LoadString_e( "cyvogue_move_ani_bk", "", szBuf, MAX_PATH );
	m_AttackCyvogueInfo.m_MoveAniBk = szBuf;
	m_AttackCyvogueInfo.m_fMoveAniBkRate = rkLoader.LoadFloat_e( "cyvogue_move_ani_rate_bk", FLOAT1 );

	rkLoader.LoadString_e( "cyvogue_move_ani_rt", "", szBuf, MAX_PATH );
	m_AttackCyvogueInfo.m_MoveAniRt = szBuf;
	m_AttackCyvogueInfo.m_fMoveAniRtRate = rkLoader.LoadFloat_e( "cyvogue_move_ani_rate_rt", FLOAT1 );

	rkLoader.LoadString_e( "cyvogue_move_ani_lt", "", szBuf, MAX_PATH );
	m_AttackCyvogueInfo.m_MoveAniLt = szBuf;
	m_AttackCyvogueInfo.m_fMoveAniLtRate = rkLoader.LoadFloat_e( "cyvogue_move_ani_rate_lt", FLOAT1 );
	//////////////////////////////////////////////////////////////////////////
	rkLoader.LoadString_e( "aim_up_animation_a", "", szBuf, MAX_PATH );
	m_AimUp_A = szBuf;
	rkLoader.LoadString_e( "aim_center_animation_a", "", szBuf, MAX_PATH );
	m_AimCenter_A = szBuf;
	rkLoader.LoadString_e( "aim_down_animation_a", "", szBuf, MAX_PATH );
	m_AimDown_A = szBuf;

	rkLoader.LoadString_e( "aim_up_animation_s", "", szBuf, MAX_PATH );
	m_AimUp_S = szBuf;
	rkLoader.LoadString_e( "aim_center_animation_s", "", szBuf, MAX_PATH );
	m_AimCenter_S = szBuf;
	rkLoader.LoadString_e( "aim_down_animation_s", "", szBuf, MAX_PATH );
	m_AimDown_S = szBuf;

	//////////////////////////////////////////////////////////////////////////
	m_fACircleRangeMin = rkLoader.LoadFloat_e( "a_magic_circle_range_min", 0.0f );
	m_fACircleRangeMax = rkLoader.LoadFloat_e( "a_magic_circle_range_max", 0.0f );
	m_fACircleMoveSpeed = rkLoader.LoadFloat_e( "a_magic_circle_move_speed", 0.0f );
	m_fACircleHeightGap = rkLoader.LoadFloat_e( "a_magic_circle_height_gap", 0.0f );
	m_fACircleOffSet = rkLoader.LoadFloat_e( "a_magic_circle_offset", 0.0f );

	m_fSCircleRangeMin = rkLoader.LoadFloat_e( "s_magic_circle_range_min", 0.0f );
	m_fSCircleRangeMax = rkLoader.LoadFloat_e( "s_magic_circle_range_max", 0.0f );
	m_fSCircleMoveSpeed = rkLoader.LoadFloat_e( "s_magic_circle_move_speed", 0.0f );
	m_fSCircleHeightGap = rkLoader.LoadFloat_e( "s_magic_circle_height_gap", 0.0f );
	m_fSCircleOffSet = rkLoader.LoadFloat_e( "s_magic_circle_offset", 0.0f );

	rkLoader.LoadString_e( "a_ready_circle", "", szBuf, MAX_PATH );
	m_AmodeReadyCircle = szBuf;
	rkLoader.LoadString_e( "s_ready_circle", "", szBuf, MAX_PATH );
	m_SmodeReadyCircle = szBuf;

	rkLoader.LoadString_e( "red_team_circle", "", szBuf, MAX_PATH );
	m_RedEffect = szBuf;
	rkLoader.LoadString_e( "blue_team_circle", "", szBuf, MAX_PATH );
	m_BlueEffect = szBuf;
	//////////////////////////////////////////////////////////////////////////
	m_dwChargeCancelTime = (DWORD)rkLoader.LoadInt_e("charge_cancel_time", 0);
	m_dwCharge_a_mode_time   = (DWORD)rkLoader.LoadInt_e("charge_a_mode_time", 0);
	m_dwCharge_s_mode_time   = (DWORD)rkLoader.LoadInt_e("charge_s_mode_time", 0);
	//////////////////////////////////////////////////////////////////////////
	m_vAddMeshList.clear();
	m_vAddBuffList.clear();

	int iMeshCnt = rkLoader.LoadInt_e( "cyvogue_add_mesh_cnt", 0 );
	for( int i=0; i < iMeshCnt; ++i )
	{
		wsprintf_e( szKey, "cyvogue_mesh%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

		ioHashString szMesh = szBuf;
		m_vAddMeshList.push_back( szMesh );
	}

	int iBuffCnt = rkLoader.LoadInt_e( "cyvogue_add_buff_cnt", 0 );
	for( int k=0; k < iBuffCnt; ++k )
	{
		wsprintf_e( szKey, "cyvogue_buff%d", k+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

		ioHashString szBuff = szBuf;
		m_vAddBuffList.push_back( szBuff );
	}

	//////////////////////////////////////////////////////////////////////////
	m_iRandomRange	= rkLoader.LoadFloat_e( "fire_random_range", 0.0f );
	m_fWeaponGapDistance = rkLoader.LoadFloat_e( "weapon_gap_distance", 0.0f );
	m_fCircleRange		= rkLoader.LoadFloat_e( "magic_circle_range", 0.0f );
	m_fWeaponFloatPower = rkLoader.LoadFloat_e( "weapon_float_power", 0.0f );
	m_fWeaponFloatMinPower = rkLoader.LoadFloat_e( "weapon_float_min_power", 0.0f );
	m_fWeaponGravityRate = rkLoader.LoadFloat_e( "weapon_gravity_rate", FLOAT1 );
	m_fRevisionRate = rkLoader.LoadFloat_e( "weapon_revision_rate", 0.0f );
	m_fWeaponMinSpeed = rkLoader.LoadFloat_e( "weapon_min_speed", 0.0f );
	m_fUpLimitHeight = rkLoader.LoadFloat_e( "up_limit_height", 30.0f );
	m_fUpLimitRange = rkLoader.LoadFloat_e( "up_limit_range", 0.0f );
	//////////////////////////////////////////////////////////////////////////
	m_fDecreaseGauge_Amode = rkLoader.LoadFloat_e( "decrease_gauge_a_mode", FLOAT1 );
	m_fDecreaseGauge_Smode = rkLoader.LoadFloat_e( "decrease_gauge_s_mode", FLOAT1 );
	m_fDecreaseGauge_Dmode = rkLoader.LoadFloat_e( "decrease_gauge_d_mode", FLOAT1 );
	//////////////////////////////////////////////////////////////////////////
	rkLoader.LoadString_e( "mode_fail_motion", "", szBuf, MAX_PATH );
	m_FailMotion = szBuf;
	m_fFailMotionRate = rkLoader.LoadFloat_e( "mode_fail_motion_rate", FLOAT1 );
	//////////////////////////////////////////////////////////////////////////
	m_dwMaxAModeDelayTime = (DWORD)rkLoader.LoadInt_e( "max_a_mode_delay_time", 0 );
	m_dwMaxSModeDelayTime = (DWORD)rkLoader.LoadInt_e( "max_s_mode_delay_time", 0 );
	//////////////////////////////////////////////////////////////////////////
	m_fGravity = rkLoader.LoadFloat_e( "mode_gravity", 0.0f );
	//////////////////////////////////////////////////////////////////////////
	rkLoader.LoadString_e( "map_circle_a_mode", "", szBuf, MAX_PATH );
	m_stTargetCircleA = szBuf;
	rkLoader.LoadString_e( "map_circle_s_mode", "", szBuf, MAX_PATH );
	m_stTargetCircleS = szBuf;
	m_dwTargetCircleMaxTime = (DWORD)rkLoader.LoadInt_e( "map_circle_mode_time", 0 );
}

ioItem* ioCyvogueItem::Clone()
{
	return new ioCyvogueItem( *this );
}

ioWeaponItem::WeaponSubType ioCyvogueItem::GetSubType() const
{
	return WST_CYVOGUE;
}

void ioCyvogueItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	if( !pOwner ) 
		return;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_dwKeyReserveTime = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

	m_fCurAngle = 0;

	ClearCyvogueState( pOwner );
	m_CyvogueTargetSkill.Clear();

	int iComboCnt = GetMaxCombo();
	if( !COMPARE( iCurCombo, 0, iComboCnt ) )
	{
		ioWeaponItem::SetNormalAttack( pOwner, iCurCombo );

		m_ChargeState = CS_NONE;
		m_iCurCombo = iCurCombo;

		pOwner->SetAttackMoveEnable( false );
		pOwner->SetChargingState( false );
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
		pOwner->SetAttackMoveEnable( false );
		pOwner->SetChargingState( true );

		/*if( m_bChargeAutoTarget )
		{
			DWORD dwTrackingTime = FRAMEGETTIME();
			dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
			pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
		}
		else*/
		{
			pOwner->StopAutoTargetTracking();
		}
	}
}

void ioCyvogueItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	switch( m_ChargeState )
	{
	case CS_CHARGING:
		OnCharging( pOwner );
		break;
	case CS_NORMAL_ATTACK:
		ioWeaponItem::CheckNormalAttack( pOwner );
		break;
	}
}

void ioCyvogueItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() )	return;

	if( IsEnableCyvogueGauge() && pOwner->IsAttackKeyDown() && !pOwner->IsHasCrown() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			SetCyvogueState( pOwner, m_AttackCyvogueInfo, true );
			return;
		}
	}
	else
	{
		ChangeToNormalAttack( pOwner );
	}
}

void ioCyvogueItem::ChangeToNormalAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	//
	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	const AttackAttribute *pAttribute = m_FormalAttack.GetAttribute( AT_NORMAL, m_iCurCombo );
	if( pAttribute )
	{
		if( !m_bChargeAutoTarget )
		{
			pOwner->SetAutoTarget( ATT_NORMAL );
		}

		pOwner->SetNormalAttackByAttribute( *pAttribute );
	}

	pOwner->ApplyNormalAttackResult( m_iCurCombo );

	m_ChargeState = CS_NORMAL_ATTACK;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_ChargeState;
		kPacket << m_iCurCombo;
		if( !m_bChargeAutoTarget )
			pOwner->FillAutoTargetInfo( kPacket );
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioCyvogueItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	int iState;
	rkPacket >> iState;
	rkPacket >> m_iCurCombo;

	switch( iState )
	{
	case CS_NONE:
		pOwner->SetAttackMoveEnable( false );
		pOwner->SetChargingState( false );

		m_ChargeState = CS_NONE;
		break;
	case CS_NORMAL_ATTACK:
		if( !m_bChargeAutoTarget )
			pOwner->ApplyAutoTargetInfo( rkPacket );
		ChangeToNormalAttack( pOwner );
		break;
	}
}

void ioCyvogueItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	int iSyncType;
	rkPacket >> iSyncType;

	switch( iSyncType )
	{
	case SST_EFFECT:
		{
			float fGauge;
			rkPacket >> fGauge;

			//SetEnableCyvogueEffect( pOwner );
			SetCurCyvogueGauge( fGauge );
		}
		break;
	case SST_CHANGE_STATE:
		{
			CheckCyvogueState( pOwner, rkPacket );
		}
		break;
	default: break;
	}
}

void ioCyvogueItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	ioAttackableItem::SetExtraInfoToWeapon( pWeapon, bJumpAttack );

	if( m_CyvogueState == CFS_FIRE_A_MODE )//|| m_CyvogueState == CFS_FIRE_S_MODE )
	{
		if( m_vCirclePos != ioMath::VEC3_ZERO )
		{
			D3DXVECTOR3 vStartPos =	pWeapon->GetPosition();
			D3DXVECTOR3 vDir = m_vCirclePos - vStartPos;
			D3DXVec3Normalize( &vDir, &vDir );
			pWeapon->SetMoveDir(vDir );
		}
	}

	if( m_CyvogueState == CFS_FIRE_S_MODE )
	{
		CheckWeaponFloatInfo( pWeapon->GetOwner(), pWeapon->GetPosition() );
		m_iCurFireCount++;

		D3DXVECTOR3 vDir = ioMath::VEC3_ZERO;
		float fSpeed = 0.0f;
		float fFloat = 0.0f;

		pWeapon->SetMoveDir( m_vWeaponMoveInfo.m_vMoveDir );
		pWeapon->SetMoveSpeed( m_vWeaponMoveInfo.m_fMoveSpeed );
		pWeapon->SetFloatPower( m_vWeaponMoveInfo.m_fFloatPower );

		ioFloatThrowWeapon *pFloatWeapon = ToFloatThrowWeapon(pWeapon);
		if( pFloatWeapon )
		{
			pFloatWeapon->SetGravityRate( m_fWeaponGravityRate );
		}
	}
}

bool ioCyvogueItem::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		switch( m_ChargeState )
		{
		case CS_CHARGING:
			return true;
		}
	}

	return false;
}

void ioCyvogueItem::OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem )
{
	m_ChargeState = CS_NONE;

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );

	pOwner->ClearDirDoubleClick();

	m_fCurChargeRate = 0.0f;
}

bool ioCyvogueItem::IsChargeAutoTarget( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	if( pOwner->GetState() != CS_ATTACK )
		return false;

	if( m_fCurChargeRate == 0.0f )
		return false;

	if( m_fMaxTargetRangeRate == 0.0f || m_fMaxTargetAngleRate == 0.0f )
		return false;

	return true;
}

void ioCyvogueItem::GetAutoTargetValue( ioBaseChar *pOwner,
	float &fRange, float &fMinAngle, float &fMaxAngle,
	AutoTargetType eType )
{
	if( IsChargeAutoTarget(pOwner) )
	{
		float fCurTargetRangeRate, fCurTargetAngleRate;
		fCurTargetRangeRate = fCurTargetAngleRate = FLOAT1;

		float fCurMaxTargetRange = max( 0.0f, (m_fMaxTargetRangeRate - FLOAT1) );
		float fCurMaxTargetAngle = max( 0.0f, (m_fMaxTargetAngleRate - FLOAT1) );

		fCurTargetRangeRate += fCurMaxTargetRange * m_fCurChargeRate;
		fCurTargetAngleRate += fCurMaxTargetAngle * m_fCurChargeRate;

		fRange = m_fAutoTargetRange * fCurTargetRangeRate;
		fMinAngle = m_fAutoTargetMinAngle / fCurTargetAngleRate;
		fMaxAngle = m_fAutoTargetMaxAngle / fCurTargetAngleRate;
	}
	else
	{
		switch( eType )
		{
		case ATT_NORMAL:
			fRange = m_fAutoTargetRange;
			fMinAngle = m_fAutoTargetMinAngle;
			fMaxAngle = m_fAutoTargetMaxAngle;
			return;
		case ATT_DASH:
			if( m_fDashAutoTargetRange == 0.0f )
				break;

			fRange = m_fDashAutoTargetRange;
			fMinAngle = m_fDashAutoTargetMinAngle;
			fMaxAngle = m_fDashAutoTargetMaxAngle;
			return;
		case ATT_JUMP:
			if( m_fJumpAutoTargetRange == 0.0f )
				break;

			fRange = m_fJumpAutoTargetRange;
			fMinAngle = m_fJumpAutoTargetMinAngle;
			fMaxAngle = m_fJumpAutoTargetMaxAngle;
			return;
		case ATT_COUNTER:
			if( m_fCountAutoTargetRange == 0.0f )
				break;

			fRange = m_fCountAutoTargetRange;
			fMinAngle = m_fCountAutoTargetMinAngle;
			fMaxAngle = m_fCountAutoTargetMaxAngle;
			return;
		}


		fRange = m_fAutoTargetRange;
		fMinAngle = m_fAutoTargetMinAngle;
		fMaxAngle = m_fAutoTargetMaxAngle;

		return;
	}
}

void ioCyvogueItem::SetCyvogueState( ioBaseChar *pOwner, const CyvogueFlyInfo &rkFlyInfo, bool bSendNet )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	if( pOwner->IsNeedProcess() )
	{
		D3DXQUATERNION qtRot = pOwner->GetTargetRot();
		pOwner->SetTargetRotToRotate( qtRot, true );
	}

	pOwner->SetState( CS_CYVOGUE_STATE );

	m_CurCyvogueInfo = rkFlyInfo;

	m_bAddEntityAndBuff = false;

	ChangeStartState( pOwner, bSendNet );

	if( bSendNet && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_CHANGE_STATE;
		kPacket << m_CyvogueState;
		kPacket << pOwner->GetTargetRot();
		kPacket << m_fCurCyvogueGauge;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioCyvogueItem::CheckKeyInput( ioBaseChar *pOwner )
{
	bool bChangeDir = false;
	bool bChangeUpDown = false;

	// Check Dir
	ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
	if( eNewDirKey != m_CurDirKey )
	{
		//if( eNewDirKey != ioUserKeyInput::DKI_NONE )
		bChangeDir = true;
		m_CurDirKey = eNewDirKey;
	}

	if( bChangeDir )
		ChangeCyvogueMoveAni( pOwner );
}

ioCyvogueItem::AttackAniDirection ioCyvogueItem::GetAttackAniDirection( ioBaseChar *pOwner )
{
	if( !pOwner )
		return AAD_CENTER;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return AAD_CENTER;

	//None은 에외 처리
	if( m_CurDirKey == ioUserKeyInput::DKI_NONE )
		return AAD_CENTER;

	// KeyDir
	ioUserKeyInput::DirKeyInput eDir = m_CurDirKey;

	float fYawD = 180.0f + 45.0f * ( eDir - ioUserKeyInput::DKI_UP );
	fYawD = ioMath::ArrangeHead( fYawD );

	D3DXQUATERNION qtRotate;
	D3DXQuaternionRotationYawPitchRoll( &qtRotate, DEGtoRAD( fYawD ), 0.0f, 0.0f );
	D3DXVECTOR3 vSightDir = qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVECTOR3 vMoveDir( vSightDir.x, 0.0f, vSightDir.z );
	D3DXVec3Normalize( &vMoveDir, &vMoveDir );

	D3DXQUATERNION qtTargetRot;
	ioMath::CalcDirectionQuaternion( qtTargetRot, -vMoveDir );

	float fCurYaw, fTargetYaw;
	fTargetYaw = RADtoDEG( ioMath::QuaterToYaw( qtTargetRot ) );
	fTargetYaw = ioMath::ArrangeHead( fTargetYaw );
	fCurYaw = RADtoDEG( ioMath::QuaterToYaw( pOwner->GetTargetRot() ) );
	fCurYaw = ioMath::ArrangeHead( fCurYaw );

	float fAngleGap = ioMath::AngleGap( fCurYaw, fTargetYaw );
	fAngleGap += 360.0f;
	int iSide = (int)fAngleGap % 360;

	int iCnt = iSide / 45;
	int iRest = iSide % 45;

	switch( iCnt )
	{
	case 0:
	case 7:
		return AAD_FRONT;
	case 1:
		if( iRest > 15 )
			return AAD_RIGHT;
		
		return AAD_FRONT;
	case 2:
		if( iRest <= 40 )
			return AAD_RIGHT;

		return AAD_BACK;
	case 3:
	case 4:
		return AAD_BACK;
	case 5:
		if( iRest > 5 )
			return AAD_LEFT;
		
		return AAD_BACK;
	case 6:
		if( iRest <= 30 )
			return AAD_LEFT;

		return AAD_FRONT;
	}

	return AAD_CENTER;
}

void ioCyvogueItem::ChangeCyvogueMoveAni( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	//None은 에외 처리
	if( m_CurDirKey == ioUserKeyInput::DKI_NONE )
	{
		pGrp->ClearAllLoopAni( FLOAT100, true );
		pGrp->ClearAllActionAni( FLOAT100, true );

		int iAniID	     = pGrp->GetAnimationIdx( m_CurCyvogueInfo.m_DelayAni );
		float fTimeRate  = m_CurCyvogueInfo.m_fDelayAniRate;

		pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fTimeRate );
		return;
	}


	DWORD dwCurTime = FRAMEGETTIME();

	// KeyDir
	ioUserKeyInput::DirKeyInput eDir = m_CurDirKey;

	float fYawD = 180.0f + 45.0f * ( eDir - ioUserKeyInput::DKI_UP );
	fYawD = ioMath::ArrangeHead( fYawD );

	D3DXQUATERNION qtRotate;
	D3DXQuaternionRotationYawPitchRoll( &qtRotate, DEGtoRAD( fYawD ), 0.0f, 0.0f );
	D3DXVECTOR3 vSightDir = qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVECTOR3 vMoveDir( vSightDir.x, 0.0f, vSightDir.z );
	D3DXVec3Normalize( &vMoveDir, &vMoveDir );

	D3DXQUATERNION qtTargetRot;
	ioMath::CalcDirectionQuaternion( qtTargetRot, -vMoveDir );

	float fCurYaw, fTargetYaw;
	fTargetYaw = RADtoDEG( ioMath::QuaterToYaw( qtTargetRot ) );
	fTargetYaw = ioMath::ArrangeHead( fTargetYaw );
	fCurYaw = RADtoDEG( ioMath::QuaterToYaw( pOwner->GetTargetRot() ) );
	fCurYaw = ioMath::ArrangeHead( fCurYaw );

	float fAngleGap = ioMath::AngleGap( fCurYaw, fTargetYaw );
	fAngleGap += 360.0f;
	int iSide = (int)fAngleGap % 360;

	int iCnt = iSide / 45;
	int iRest = iSide % 45;

	switch( iCnt )
	{
	case 0:
	case 7:
		if( !m_CurCyvogueInfo.m_MoveAniFr.IsEmpty() )
			CheckCyvogueFlyMoveAni( pOwner, m_CurCyvogueInfo.m_MoveAniFr, m_CurCyvogueInfo.m_fMoveAniFrRate, vMoveDir );
		break;
	case 1:
		if( iRest > 15 )
		{
			if( !m_CurCyvogueInfo.m_MoveAniRt.IsEmpty() )
				CheckCyvogueFlyMoveAni( pOwner, m_CurCyvogueInfo.m_MoveAniRt, m_CurCyvogueInfo.m_fMoveAniRtRate, vMoveDir );
		}
		else
		{
			if( !m_CurCyvogueInfo.m_MoveAniFr.IsEmpty() )
				CheckCyvogueFlyMoveAni( pOwner, m_CurCyvogueInfo.m_MoveAniFr, m_CurCyvogueInfo.m_fMoveAniFrRate, vMoveDir );
		}
		break;
	case 2:
		if( iRest <= 40 )
		{
			if( !m_CurCyvogueInfo.m_MoveAniRt.IsEmpty() )
				CheckCyvogueFlyMoveAni( pOwner, m_CurCyvogueInfo.m_MoveAniRt, m_CurCyvogueInfo.m_fMoveAniRtRate, vMoveDir );
		}
		else
		{
			if( !m_CurCyvogueInfo.m_MoveAniBk.IsEmpty() )
				CheckCyvogueFlyMoveAni( pOwner, m_CurCyvogueInfo.m_MoveAniBk, m_CurCyvogueInfo.m_fMoveAniBkRate, vMoveDir );
		}
		break;
	case 3:
	case 4:
		if( !m_CurCyvogueInfo.m_MoveAniBk.IsEmpty() )
			CheckCyvogueFlyMoveAni( pOwner, m_CurCyvogueInfo.m_MoveAniBk, m_CurCyvogueInfo.m_fMoveAniBkRate, vMoveDir );
		break;
	case 5:
		if( iRest > 5 )
		{
			if( !m_CurCyvogueInfo.m_MoveAniLt.IsEmpty() )
				CheckCyvogueFlyMoveAni( pOwner, m_CurCyvogueInfo.m_MoveAniLt, m_CurCyvogueInfo.m_fMoveAniLtRate, vMoveDir );
		}
		else
		{
			if( !m_CurCyvogueInfo.m_MoveAniBk.IsEmpty() )
				CheckCyvogueFlyMoveAni( pOwner, m_CurCyvogueInfo.m_MoveAniBk, m_CurCyvogueInfo.m_fMoveAniBkRate, vMoveDir );
		}
		break;
	case 6:
		if( iRest <= 30 )
		{
			if( !m_CurCyvogueInfo.m_MoveAniLt.IsEmpty() )
				CheckCyvogueFlyMoveAni( pOwner, m_CurCyvogueInfo.m_MoveAniLt, m_CurCyvogueInfo.m_fMoveAniLtRate, vMoveDir );
		}
		else
		{
			if( !m_CurCyvogueInfo.m_MoveAniFr.IsEmpty() )
				CheckCyvogueFlyMoveAni( pOwner, m_CurCyvogueInfo.m_MoveAniFr, m_CurCyvogueInfo.m_fMoveAniFrRate, vMoveDir );
		}
		break;
	}
}

void ioCyvogueItem::ProcessCyvogueState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );

	DWORD dwCurTime = FRAMEGETTIME();

	if( m_bAddEntityAndBuff == false && m_CyvogueState != CFS_START )
	{
		m_bAddEntityAndBuff = true;
		AddEntityAndBuff( pOwner );
	}

	if( !pOwner->IsAttackKeyDown() )
		m_bKeyFlag = true;


	switch( m_CyvogueState )
	{
	case CFS_START:
		if( m_dwMotionEndTime < dwCurTime )
			ChangeDelayState(pOwner, false);

		break;
	case CFS_DELAY:
		if( pOwner->IsNeedProcess() && m_fCurCyvogueGauge <= 0.0f )
		{
			ChangeEndState( pOwner );
			return;
		}

		if( pOwner->IsNeedProcess() && CheckChargingMode( pOwner ) )
			return;

		CheckKeyInput( pOwner );

		break;
	case CFS_A_MODE:
		{
			// 뱡향 변화 체크
			pOwner->CheckChangeDirectionForMagicCircle();

			// MagicCircle 위치 갱신
			pOwner->UpdateMagicCirclePosAll( m_fACircleMoveSpeed, m_fACircleRangeMax, m_fACircleRangeMin, m_fACircleHeightGap, false );

			// MagicCircle 위치방향으로 캐릭터 회전
			D3DXVECTOR3 vTargetPos = pOwner->GetMagicCirclePos();
			pOwner->SetTargetRotToTargetPos( vTargetPos, true, false );

			ChangeTrackingUD(pOwner);

			if( pOwner->IsNeedProcess() && !pOwner->IsJumpKeyDown() )
			{
				ChangeAFireState( pOwner );
				return;
			}

			if( pOwner->IsNeedProcess() && (m_dwModeStartTime + m_dwMaxAModeDelayTime) < dwCurTime )
			{
				ChangeAFireState( pOwner );
				return;
			}
		}
		break;
	case CFS_S_MODE:
		{
			// 뱡향 변화 체크
			pOwner->CheckChangeDirectionForMagicCircle();

			// MagicCircle 위치 갱신
			pOwner->UpdateMagicCirclePosAll( m_fSCircleMoveSpeed, m_fSCircleRangeMax, m_fSCircleRangeMin, m_fACircleHeightGap, false );

			// MagicCircle 위치방향으로 캐릭터 회전
			D3DXVECTOR3 vTargetPos = pOwner->GetMagicCirclePos();
			pOwner->SetTargetRotToTargetPos( vTargetPos, true, false );

			ChangeTrackingUD(pOwner);

			if( pOwner->IsNeedProcess() && !pOwner->IsDefenseKeyDown() )
			{
				ChangeSFireState( pOwner );
				return;
			}

			if( pOwner->IsNeedProcess() && (m_dwModeStartTime + m_dwMaxSModeDelayTime) < dwCurTime )
			{
				ChangeSFireState( pOwner );
				return;
			}
		}
		break;
	case CFS_FAIL_MOTION:
		if( m_dwMotionEndTime && m_dwMotionEndTime < dwCurTime )
			ChangeDelayState( pOwner, false );
		break;
	case CFS_FIRE_A_MODE:
	case CFS_FIRE_S_MODE:
		if( pOwner->IsNeedProcess() && m_dwMotionEndTime < dwCurTime )
			ChangeDelayState( pOwner, true );

		break;
	case CFS_FIRE_D_MODE:
		if( pOwner->IsNeedProcess() && m_dwMotionEndTime && m_dwMotionEndTime < dwCurTime )
			ChangeDelayState(pOwner, true );
	
		break;
	case CFS_END:
		if( m_dwMotionEndTime < dwCurTime )
		{
			pOwner->SetState( CS_DELAY );
			ClearCyvogueState(pOwner);
			return;
		}

		break;
	}

	if( m_dwTargetCircleStartTime && (m_dwTargetCircleStartTime + m_dwTargetCircleMaxTime ) < dwCurTime )
		ReleaseEffect(pOwner);
}

void ioCyvogueItem::CheckCyvogueState( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner )
		return;

	int iCyvogueState;
	rkPacket >> iCyvogueState;

	D3DXQUATERNION qtRot;
	rkPacket >> qtRot;

	switch( iCyvogueState )
	{
	case CFS_START:
		pOwner->SetTargetRotToRotate( qtRot, true );
		SetCyvogueState( pOwner, m_AttackCyvogueInfo, false );
		break;
	case CFS_DELAY:
		ChangeDelayState(pOwner, false);
		break;
	case CFS_A_MODE:
		ChangeAmodeState(pOwner);
		break;
	case CFS_S_MODE:
		ChangeSmodeState(pOwner);
		break;
	case CFS_FAIL_MOTION:
		ChangeFailMotionState(pOwner);
		break;
	case CFS_FIRE_A_MODE:
		ChangeAFireState(pOwner);
		break;
	case CFS_FIRE_S_MODE:
		ChangeSFireState(pOwner);
		break;
	case CFS_FIRE_D_MODE:
		ChangeDFireState(pOwner);
		break;
	case CFS_END:
		ChangeEndState(pOwner);
		break;
	default: break;
	}
}

int ioCyvogueItem::GetMaxBullet()
{
	return (int)m_fMaxCyvogueGauge;
}

int ioCyvogueItem::GetNeedBullet()
{
	return (int)m_fNeedCyvogueGauge;
}

int ioCyvogueItem::GetCurBullet()
{
	float fCurCoolTime = m_fCurCyvogueGauge;
	fCurCoolTime = max( 0, min( fCurCoolTime, m_fMaxCyvogueGauge ) );

	return (int)fCurCoolTime;
}

void ioCyvogueItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	float fCurBullet = iCurBullet;
	fCurBullet = max( 0, min( fCurBullet, m_fMaxCyvogueGauge ) );

	m_fCurCyvogueGauge = fCurBullet;
}

void ioCyvogueItem::InitCyvogueGauge()
{
	m_fCurCyvogueGauge = 0.0f;
}

void ioCyvogueItem::MaxCyvogueGauge()
{
	m_fCurCyvogueGauge = m_fMaxCyvogueGauge;
}

float ioCyvogueItem::GetCurCyvogueGauge()
{
	return m_fCurCyvogueGauge;
}

void ioCyvogueItem::SetCurCyvogueGauge( float fGauge )
{
	m_fCurCyvogueGauge = fGauge;
	m_fCurCyvogueGauge = (float)max( 0, min( m_fCurCyvogueGauge, m_fMaxCyvogueGauge ) );
}

void ioCyvogueItem::CheckEnableCyvogueGauge( ioBaseChar *pChar )
{
	/*if( m_fCurCyvogueGauge >= m_fNeedCyvogueGauge )
		SetEnableCyvogueEffect( pChar );*/
}

bool ioCyvogueItem::IsEnableCyvogueGauge()
{
	if( m_fCurCyvogueGauge >= m_fNeedCyvogueGauge )
		return true;

	return false;
}

bool ioCyvogueItem::IsCyvogueState()
{
	switch( m_CyvogueState )
	{
	case CFS_DELAY:
		return true;
	}

	return false;
}

void ioCyvogueItem::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	// check owner state
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	if( pOwner->IsSystemState() )
		return;

	switch( pOwner->GetState() )
	{
	case CS_CYVOGUE_STATE:
		{
			if( m_CyvogueState == CFS_A_MODE || m_CyvogueState == CFS_S_MODE || 
				m_CyvogueState == CFS_START || m_CyvogueState == CFS_END ||
				m_CyvogueState == CFS_FIRE_A_MODE || m_CyvogueState == CFS_FIRE_S_MODE )
				return;

			float fGauge = m_fDecreaseCyvogueGauge * fTimePerSec;

			ioPlayStage *pStage = pOwner->GetCreator();
			if( pStage )
			{
				D3DXVECTOR3 vCurPos = pOwner->GetWorldPosition();
				float fMapHeight = pStage->GetMapHeight( vCurPos.x, vCurPos.z, pOwner, false, pOwner->GetMidHeightByRate(1.0f) );
				float fGapHeight = fabs( vCurPos.y - fMapHeight );

				if( m_fHighDecreaseHeight < fGapHeight )
					fGauge = m_fHighDecreaseCyvogueGauge * fTimePerSec;
			}

			m_fCurCyvogueGauge -= fGauge;
			m_fCurCyvogueGauge = max( 0.0f, m_fCurCyvogueGauge );
		}
		break;
	default:
		if( m_fCurCyvogueGauge < m_fMaxCyvogueGauge )
		{
			float fGauge = m_fIncreaseCyvogueGauge * fTimePerSec;

			m_fCurCyvogueGauge += fGauge;
			m_fCurCyvogueGauge = min( m_fCurCyvogueGauge, m_fMaxCyvogueGauge );
		}
		break;
	}
}

void ioCyvogueItem::ClearCyvogueState( ioBaseChar *pOwner )
{
	m_CyvogueState = CFS_NONE;

	// 순서중요
	/*if( pOwner && !m_CurCyvogueInfo.m_CyvogueFlyEffect.IsEmpty() )
	{
		pOwner->EndEffect( m_CurCyvogueInfo.m_CyvogueFlyEffect, false );
	}*/

	ReleaseEffect( pOwner );

	m_dwTargetCircleStartTime = 0;
	m_dwCircleEffectID = 0;

	m_iCurFireCount = 0;

	m_CurCyvogueInfo.Init();
	m_CyvogueTarget.Clear();


	if( pOwner )
	{
		pOwner->DestroyMagicCircle();
		pOwner->SetMoveDirByRotate(pOwner->GetTargetRot());
	}

	EraseEntityAndBuff( pOwner );

	m_bAddEntityAndBuff = false;
}

void ioCyvogueItem::CheckCyvogueFlyMoveAni( ioBaseChar *pOwner, const ioHashString &szAni, float fAniRate, D3DXVECTOR3 vDir )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );

	m_dwKeyReserveTime = 0;
	float fTimeRate = fAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	int iAniID = pGrp->GetAnimationIdx( szAni );
	pGrp->ClearAllActionAni( 0.0f, true );
	pGrp->ClearAllLoopAni( 0.0f, true );
	pGrp->SetLoopAni( iAniID, FLOAT100 );
	//pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );
}

void ioCyvogueItem::OnEquiped( ioBaseChar *pOwner )
{
	ioWeaponItem::OnEquiped( pOwner );
}

void ioCyvogueItem::OnReleased( ioBaseChar *pOwner )
{
//	ReleaseEnableCyvogueEffect( pOwner );
//	ReleaseCyvogueFlyMoveEffect( pOwner );

	ClearCyvogueState( pOwner );
	m_CyvogueTargetSkill.Clear();
	m_fCurCyvogueGauge = 0.0f;

	pOwner->SetTargetMarker( ioTargetMarker::MT_NONE, "", "", "", "", "" );

	ioAttackableItem::OnReleased( pOwner );
}

void ioCyvogueItem::ChangeStartState( ioBaseChar *pOwner, bool bSendNet )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->InitExtendAttackTagInfo();

	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_CurCyvogueInfo.m_StartAni );
	float fTimeRate  = m_CurCyvogueInfo.m_fStartAniRate;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	m_CyvogueState = CFS_START;
	m_CuvogueModeChargeState = CMC_NONE;

	m_dwModeChargeStartTime = 0;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_bKeyFlag = false;
	m_dwMotionEndTime = 0;
	m_vCirclePos = ioMath::VEC3_ZERO;

	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
}

void ioCyvogueItem::ChangeDelayState( ioBaseChar *pOwner, bool bSendNet  )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	if( pOwner->IsNeedProcess() )
	{
		pOwner->ChangeDirectionByInputDir( true );
	}

	pOwner->InitExtendAttackTagInfo();

	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_CurCyvogueInfo.m_DelayAni );
	float fTimeRate  = m_CurCyvogueInfo.m_fDelayAniRate;

	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	m_CyvogueState = CFS_DELAY;
	m_CuvogueModeChargeState = CMC_NONE;
	//////////////////////////////////////////////////////////////////////////
	m_dwModeChargeStartTime = 0;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_bKeyFlag = false;
	m_dwMotionEndTime = 0;
	m_vCirclePos = ioMath::VEC3_ZERO;
	m_dwModeStartTime = 0;

	if( bSendNet && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_CHANGE_STATE;
		kPacket << m_CyvogueState;
		kPacket << pOwner->GetTargetRot();
		kPacket << m_fCurCyvogueGauge;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioCyvogueItem::ChangeAmodeState( ioBaseChar *pOwner )
{
	m_dwModeChargeStartTime = 0;
	m_CuvogueModeChargeState = CMC_NONE;

	m_CyvogueState = CFS_A_MODE;

	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	m_fCurCyvogueGauge -= m_fDecreaseGauge_Amode;

	DWORD dwCurTime = FRAMEGETTIME();
	pOwner->InitExtendAttackTagInfo();

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_AimCenter_A );
	float fTimeRate = m_CurCyvogueInfo.m_fDelayAniRate;
	pGrp->SetLoopAni( m_AimUp_A, 0.0f, 0.0f );
	pGrp->SetLoopAni( m_AimDown_A, 0.0f, 0.0f );

	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	if(	/*pOwner->IsNeedProcess() && */!pOwner->CheckMagicCircle() )
	{
		ioHashString szDummyCircle;
		
		D3DXVECTOR3 vLook = pOwner->GetTargetRot() * D3DXVECTOR3( 0.f, 0.f, -FLOAT1 );
		D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
		vPos += (vLook*m_fACircleOffSet);

		pOwner->CreateMagicCircle( m_AmodeReadyCircle, m_AmodeReadyCircle, szDummyCircle, vPos, 0 );
	}

	m_dwModeStartTime = dwCurTime;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_CHANGE_STATE;
		kPacket << m_CyvogueState;
		kPacket << pOwner->GetTargetRot();
		kPacket << m_fCurCyvogueGauge;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioCyvogueItem::ChangeSmodeState( ioBaseChar *pOwner )
{
	m_dwModeChargeStartTime = 0;
	m_CuvogueModeChargeState = CMC_NONE;

	m_CyvogueState = CFS_S_MODE;

	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	m_fCurCyvogueGauge -= m_fDecreaseGauge_Smode;

	DWORD dwCurTime = FRAMEGETTIME();
	pOwner->InitExtendAttackTagInfo();

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_AimCenter_S );
	float fTimeRate = m_CurCyvogueInfo.m_fDelayAniRate;
	pGrp->SetLoopAni( m_AimUp_S, 0.0f, 0.0f );
	pGrp->SetLoopAni( m_AimDown_S, 0.0f, 0.0f );

	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	if( /*pOwner->IsNeedProcess() &&*/ !pOwner->CheckMagicCircle() )
	{
		ioHashString szDummyCircle;

		D3DXVECTOR3 vLook = pOwner->GetTargetRot() * D3DXVECTOR3( 0.f, 0.f, -FLOAT1 );
		D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
		vPos += (vLook*m_fSCircleOffSet);

		pOwner->CreateMagicCircle( m_SmodeReadyCircle, m_SmodeReadyCircle, szDummyCircle, vPos, 0 );
	}

	m_dwModeStartTime = dwCurTime;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_CHANGE_STATE;
		kPacket << m_CyvogueState;
		kPacket << pOwner->GetTargetRot();
		kPacket << m_fCurCyvogueGauge;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioCyvogueItem::ChangeAFireState( ioBaseChar *pOwner )
{
	if( pOwner->CheckMagicCircle() )
		m_vCirclePos = pOwner->GetMagicCirclePos();

	if( /*pOwner->IsNeedProcess() && */pOwner->CheckMagicCircle() )
		pOwner->DestroyMagicCircle();


	m_dwModeChargeStartTime = 0;
	m_CuvogueModeChargeState = CMC_NONE;

	m_CyvogueState = CFS_FIRE_A_MODE;

	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	pOwner->InitExtendAttackTagInfo();

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_CurCyvogueInfo.m_A_Attack.m_AttackAnimation );
	float fTimeRate  = m_CurCyvogueInfo.m_A_Attack.m_fAttackAniRate;
	DWORD dwPreDelay = m_CurCyvogueInfo.m_A_Attack.m_dwPreDelay;
	DWORD dwEndDelay = m_CurCyvogueInfo.m_A_Attack.m_dwEndDelay;

	pOwner->RefreshFireTimeList( iAniID, m_CurCyvogueInfo.m_A_Attack.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay );
	pOwner->IncreaseWeaponIndexBase();

	ChangeTrackingUD_Fire(pOwner, &m_CurCyvogueInfo.m_A_Attack);

	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate  + dwPreDelay;

	ReleaseEffect( pOwner );
	CreateEffect( pOwner, true,	m_vCirclePos );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_CHANGE_STATE;
		kPacket << m_CyvogueState;
		kPacket << pOwner->GetTargetRot();
		kPacket << m_fCurCyvogueGauge;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioCyvogueItem::ChangeSFireState( ioBaseChar *pOwner )
{
	if( pOwner->CheckMagicCircle() )
		m_vCirclePos = pOwner->GetMagicCirclePos();

	if( /*pOwner->IsNeedProcess() &&*/ pOwner->CheckMagicCircle() )
		pOwner->DestroyMagicCircle();

	m_dwModeChargeStartTime = 0;
	m_CuvogueModeChargeState = CMC_NONE;

	m_CyvogueState = CFS_FIRE_S_MODE;

	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	pOwner->InitExtendAttackTagInfo();

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_CurCyvogueInfo.m_S_Attack.m_AttackAnimation );
	float fTimeRate  = m_CurCyvogueInfo.m_S_Attack.m_fAttackAniRate;
	DWORD dwPreDelay = m_CurCyvogueInfo.m_S_Attack.m_dwPreDelay;
	DWORD dwEndDelay = m_CurCyvogueInfo.m_S_Attack.m_dwEndDelay;

	//m_iCurFireCount = 0;
	pOwner->RefreshFireTimeList( iAniID, m_CurCyvogueInfo.m_S_Attack.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay );
	pOwner->IncreaseWeaponIndexBase();

	ChangeTrackingUD_Fire(pOwner, &m_CurCyvogueInfo.m_S_Attack);
	//pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	//pOwner->SetAutoTarget( ATT_NORMAL );

	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate  + dwPreDelay;

	ReleaseEffect( pOwner );
	CreateEffect( pOwner, false, m_vCirclePos );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_CHANGE_STATE;
		kPacket << m_CyvogueState;
		kPacket << pOwner->GetTargetRot();
		kPacket << m_fCurCyvogueGauge;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioCyvogueItem::ChangeDFireState( ioBaseChar *pOwner )
{
	m_dwModeChargeStartTime = 0;
	m_CuvogueModeChargeState = CMC_NONE;

	m_CyvogueState = CFS_FIRE_D_MODE;

	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	m_fCurCyvogueGauge -= m_fDecreaseGauge_Dmode;
	m_fCurCyvogueGauge = max( 0.f, m_fCurCyvogueGauge );

	DWORD dwCurTime = FRAMEGETTIME();
	pOwner->InitExtendAttackTagInfo();

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	AttackAniDirection eFireDirection = GetAttackAniDirection(pOwner);
	int iAniID = -1;
	if( eFireDirection == ioCyvogueItem::AAD_CENTER )
		iAniID = pGrp->GetAnimationIdx( m_DmodeAttackAniInfo.m_CenterAttack );
	else if( eFireDirection == ioCyvogueItem::AAD_FRONT )
		iAniID = pGrp->GetAnimationIdx( m_DmodeAttackAniInfo.m_FrontAttack );
	else if( eFireDirection == ioCyvogueItem::AAD_BACK )
		iAniID = pGrp->GetAnimationIdx( m_DmodeAttackAniInfo.m_BackAttack );
	else if( eFireDirection == ioCyvogueItem::AAD_LEFT)
		iAniID = pGrp->GetAnimationIdx( m_DmodeAttackAniInfo.m_LeftAttack );
	else if( eFireDirection == ioCyvogueItem::AAD_RIGHT )
		iAniID = pGrp->GetAnimationIdx( m_DmodeAttackAniInfo.m_RightAttack );

	float fTimeRate  = m_CurCyvogueInfo.m_D_Attack.m_fAttackAniRate;
	DWORD dwPreDelay = m_CurCyvogueInfo.m_D_Attack.m_dwPreDelay;
	DWORD dwEndDelay = m_CurCyvogueInfo.m_D_Attack.m_dwEndDelay;

	pOwner->RefreshFireTimeList( iAniID, m_CurCyvogueInfo.m_D_Attack.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay );
	pOwner->IncreaseWeaponIndexBase();

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	//pOwner->SetAutoTarget( ATT_NORMAL );

	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate  + dwPreDelay;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_CHANGE_STATE;
		kPacket << m_CyvogueState;
		kPacket << pOwner->GetTargetRot();
		kPacket << m_fCurCyvogueGauge;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioCyvogueItem::ChangeEndState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->InitExtendAttackTagInfo();

	pOwner->ClearReservedSliding();
	pOwner->SetForcePowerAmt( 0.0f );

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_CurCyvogueInfo.m_EndAni );
	float fTimeRate  = m_CurCyvogueInfo.m_fEndAniRate;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	m_CyvogueState = CFS_END;
	m_CuvogueModeChargeState = CMC_NONE;

	m_dwModeChargeStartTime = 0;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_bKeyFlag = false;
	m_dwMotionEndTime = 0;
	m_vCirclePos = ioMath::VEC3_ZERO;

	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_CHANGE_STATE;
		kPacket << m_CyvogueState;
		kPacket << pOwner->GetTargetRot();
		kPacket << m_fCurCyvogueGauge;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	//pOwner->SetState( CS_DELAY );
	//ClearCyvogueState(pOwner);
}

void ioCyvogueItem::ChangeFailMotionState( ioBaseChar *pOwner )
{
	m_dwModeChargeStartTime = 0;
	m_CuvogueModeChargeState = CMC_NONE;

	m_CyvogueState = CFS_FAIL_MOTION;

	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	pOwner->InitExtendAttackTagInfo();

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_FailMotion );
	float fTimeRate  = m_fFailMotionRate;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fTimeRate );
	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << SST_CHANGE_STATE;
		kPacket << m_CyvogueState;
		kPacket << pOwner->GetTargetRot();
		kPacket << m_fCurCyvogueGauge;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

const ioHashString& ioCyvogueItem::GetCyvogueTargetSkill()
{
	return m_CyvogueTargetSkill;
}

void ioCyvogueItem::ClearCyvogueTargetSkill()
{
	m_CyvogueTargetSkill.Clear();
}

bool ioCyvogueItem::IsBallTarget()
{
	return m_bAimedBall;
}

void ioCyvogueItem::ReleaseBipe( ioBaseChar *pOwner )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	ioBiped *pBiped = pGrp->GetBiped("Bip01 Spine1");
	if(!pBiped)	
		return;

	pBiped->ReleaseExtraRotation();
}

void ioCyvogueItem::ChangeTrackingUD( ioBaseChar *pOwner )
{
	if( !pOwner->CheckMagicCircle() )
		return;

	D3DXVECTOR3 vCirclePos = pOwner->GetMagicCirclePos();

	ioEntityGroup *pGrp = pOwner->GetGroup();
	ioAnimation *pUp, *pCenter, *pDown;

	if( m_CyvogueState == CFS_A_MODE )
	{
		pUp = pGrp->GetLoopInPlay( m_AimUp_A );
		pCenter = pGrp->GetLoopInPlay( m_AimCenter_A );
		pDown = pGrp->GetLoopInPlay( m_AimDown_A );
	}
	else
	{
		pUp = pGrp->GetLoopInPlay( m_AimUp_S );
		pCenter = pGrp->GetLoopInPlay( m_AimCenter_S );
		pDown = pGrp->GetLoopInPlay( m_AimDown_S );
	}
	

	D3DXVECTOR3 vAimedDir;
	vAimedDir = vCirclePos - pOwner->GetMidPositionByRate(m_fAimAniWeightRate);

	D3DXVec3Normalize( &vAimedDir, &vAimedDir );
	float fAniWeight = GetAimedUpDownWeight( vAimedDir, 85.0f );
	if( vAimedDir.y >= 0.0f )	// up, center
	{
		if( !pUp || !pCenter || !pDown )
			return;

		pUp->ForceWeight( fAniWeight );
		pCenter->ForceWeight( FLOAT1 - fAniWeight );
		pDown->ForceWeight( 0.0f );
	}
	else	// center, down
	{
		if( !pUp || !pCenter || !pDown )
			return;
		
		pUp->ForceWeight( 0.0f );
		pCenter->ForceWeight( FLOAT1 - fAniWeight );
		pDown->ForceWeight( fAniWeight );
	}
}

void ioCyvogueItem::ChangeTrackingUD_Fire( ioBaseChar *pOwner, const AttackAttribute *pAttr )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	if( m_vCirclePos == ioMath::VEC3_ZERO )
		return;

	D3DXVECTOR3 vAimedDir;
	vAimedDir = m_vCirclePos - pOwner->GetMidPositionByRate(m_fFireAniWeightRate);

	D3DXVec3Normalize( &vAimedDir, &vAimedDir );

	float fTimeRate  = pAttr->m_fAttackAniRate;
	DWORD dwPreDelay = pAttr->m_dwPreDelay;

	DWORD dwCurTime = FRAMEGETTIME();
	float fWeight = GetAimedUpDownWeight( vAimedDir, 85.0f );

	if( vAimedDir.y >= 0.0f )
		AimedFireToUp( pGrp, fWeight, fTimeRate, dwPreDelay, true );
	else
		AimedFireToDown( pGrp, fWeight, fTimeRate, dwPreDelay, true );
}

void ioCyvogueItem::AimedFireToUp( ioEntityGroup *pGrp, float fWeight, float fTimeRate, DWORD dwPreDelay, bool bHold )
{
	float fFadeOut = FLOAT100;
	if( !bHold )
		fFadeOut = 0.0f;

	bool bAniFx1 = false;
	bool bAniFx2 = true;

	if( fWeight > FLOAT05 )
	{
		bAniFx1 = true;
		bAniFx2 = false;
	}

	ioHashString szFireUp, szFireCenter;
	
	if( m_CyvogueState == CFS_FIRE_A_MODE )
	{
		szFireUp = m_stAmodeFireUp;
		szFireCenter = m_stAmodeFireCenter;
	}
	else if( m_CyvogueState == CFS_FIRE_S_MODE )
	{
		szFireUp = m_stSmodeFireUp;
		szFireCenter = m_stSmodeFireCenter;
	}


	if( fWeight > 0.0f )
	{
		pGrp->SetActionAni( szFireUp,
			FLOAT100, fFadeOut,
			fWeight, FLOAT1/fTimeRate,
			0.0f, dwPreDelay,
			bHold, bAniFx1 );
	}

	pGrp->SetActionAni( szFireCenter,
		FLOAT100, fFadeOut,
		1.0f - fWeight, FLOAT1/fTimeRate,
		0.0f, dwPreDelay,
		bHold, bAniFx2 );
}

void ioCyvogueItem::AimedFireToDown( ioEntityGroup *pGrp, float fWeight, float fTimeRate, DWORD dwPreDelay, bool bHold )
{
	float fFadeOut = FLOAT100;
	if( !bHold )
		fFadeOut = 0.0f;
	bool bAniFx1 = true;
	bool bAniFx2 = false;

	if( fWeight > FLOAT05 )
	{
		bAniFx1 = false;
		bAniFx2 = true;
	}

	ioHashString szFireDown, szFireCenter;

	if( m_CyvogueState == CFS_FIRE_A_MODE )
	{
		szFireDown = m_stAmodeFireDown;
		szFireCenter = m_stAmodeFireCenter;
	}
	else if( m_CyvogueState == CFS_FIRE_S_MODE )
	{
		szFireDown = m_stSmodeFireDown;
		szFireCenter = m_stSmodeFireCenter;
	}

	if( fWeight > 0.0f )
	{
		pGrp->SetActionAni( szFireCenter,
			FLOAT100, fFadeOut,
			1.0f - fWeight, FLOAT1/fTimeRate,
			0.0f, dwPreDelay,
			bHold, bAniFx1 );
	}

	pGrp->SetActionAni( szFireDown,
		FLOAT100, fFadeOut,
		fWeight, FLOAT1/fTimeRate,
		0.0f, dwPreDelay,
		bHold, bAniFx2 );
}

float ioCyvogueItem::GetAimedUpDownWeight( const D3DXVECTOR3 &vAimDir, float fMaxAngle )
{
	D3DXVECTOR3 vXZDir( vAimDir.x, 0.0f, vAimDir.z );
	D3DXVec3Normalize( &vXZDir, &vXZDir );

	float fAngle = D3DXVec3Dot( &vXZDir, &vAimDir );
	fAngle = max( -FLOAT1, min( fAngle, FLOAT1 ) );
	fAngle = acosf( fAngle );
	fAngle = RADtoDEG( fAngle );
	fAngle = max( 0.0f, min( fAngle, fMaxAngle ) );

	float fWeight = fAngle / fMaxAngle;

	return fWeight;
}

bool ioCyvogueItem::CheckChargingMode( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_CuvogueModeChargeState == CMC_NONE )
	{
		//a
		if( pOwner->IsJumpKeyDown() )
		{
			m_dwModeChargeStartTime = dwCurTime;
			m_CuvogueModeChargeState = CMC_A;
			//에니
		}
		//s
		else if( pOwner->IsDefenseKeyDown() )
		{
			m_dwModeChargeStartTime = dwCurTime;
			m_CuvogueModeChargeState = CMC_S;
			//에니
		}
		else if( pOwner->IsAttackKeyDown() && m_bKeyFlag )
		{
			m_dwModeChargeStartTime = dwCurTime;
			m_CuvogueModeChargeState = CMC_CANCEL;
		}
	}
	else if( m_CuvogueModeChargeState == CMC_A )
	{
		if( pOwner->IsJumpKeyDown() )
		{
			if( (m_dwModeChargeStartTime + m_dwCharge_a_mode_time) < dwCurTime )
			{
				if( m_fCurCyvogueGauge >= m_fDecreaseGauge_Amode )
					ChangeAmodeState( pOwner );
				else
					ChangeFailMotionState(pOwner);

				return true;
			}
		}
		else
		{
			m_CuvogueModeChargeState = CMC_NONE;
			return false;
		}

	}
	else if( m_CuvogueModeChargeState == CMC_S )
	{
		if( pOwner->IsDefenseKeyDown() )
		{
			if( (m_dwModeChargeStartTime + m_dwCharge_s_mode_time) < dwCurTime )
			{
				if( m_fCurCyvogueGauge >= m_fDecreaseGauge_Smode )
					ChangeSmodeState( pOwner );
				else
					ChangeFailMotionState(pOwner);

				return true;
			}
		}
		else
		{
			m_CuvogueModeChargeState = CMC_NONE;
			return false;
		}
	}
	else if( m_CuvogueModeChargeState == CMC_CANCEL )
	{
		if( pOwner->IsAttackKeyDown() )
		{
			if( (m_dwModeChargeStartTime + m_dwChargeCancelTime) < dwCurTime )
			{
				ChangeEndState( pOwner );
				return true;
			}
		}
		else
		{
			if( m_fCurCyvogueGauge > 0 )
				ChangeDFireState(pOwner);
			else
				m_CuvogueModeChargeState = CMC_NONE;

		}
	}

	return false;
}

bool ioCyvogueItem::IsEnableMoveState()
{
	if( m_CyvogueState == CFS_DELAY || m_CyvogueState == CFS_FIRE_D_MODE )
		return true;


	return false;
}

float ioCyvogueItem::GetAttackSpeedRate() const
{
	if( m_CyvogueState != CFS_NONE )
		return m_fAttackSpeedRate;

	return FLOAT1;
}

void ioCyvogueItem::AddEntityAndBuff( ioBaseChar *pOwner )
{
	// add mesh
	int iMeshCnt = m_vAddMeshList.size();
	for( int i=0; i < iMeshCnt; ++i )
	{
		ioHashString szMeshName = m_vAddMeshList[i];
		pOwner->AddEntity( szMeshName );
	}

	// add buff
	int iBuffCnt = m_vAddBuffList.size();
	for( int j=0; j < iBuffCnt; ++j )
	{
		ioHashString szBuffName = m_vAddBuffList[j];
		pOwner->AddNewBuff( szBuffName, pOwner->GetCharName(), GetName(), NULL );
	}
}

void ioCyvogueItem::EraseEntityAndBuff( ioBaseChar *pOwner )
{
	// remove mesh
	int iMeshCnt = m_vAddMeshList.size();
	for( int i=0; i < iMeshCnt; ++i )
	{
		ioHashString szMeshName = m_vAddMeshList[i];
		pOwner->DestroyEntity( szMeshName );
	}

	// remove buff
	int iBuffCnt = m_vAddBuffList.size();
	for( int j=0; j < iBuffCnt; ++j )
	{
		ioHashString szBuffName = m_vAddBuffList[j];
		pOwner->RemoveBuff( szBuffName );
	}
}

void ioCyvogueItem::CheckWeaponFloatInfo( ioBaseChar *pChar, const D3DXVECTOR3& vStartPos )
{
	// CharDir
	D3DXVECTOR3 vCharPos = vStartPos;//pChar->GetWorldPosition();
	D3DXVECTOR3 vCharDir = m_vCirclePos - vCharPos;
	//vCharDir.y = 0.0f;

	float fFirstRange = D3DXVec3Length( &vCharDir );
	D3DXVec3Normalize( &vCharDir, &vCharDir );

	/*D3DXQUATERNION qtRot;
	ioMath::CalcDirectionQuaternion( qtRot, -vCharDir );
	pChar->SetTargetRotAndMoveDirChange( qtRot, false );
	pChar->SetWorldOrientation( qtRot );*/

	IORandom random;
	DWORD dwSeed = 0;
	int iRand = 0;
	dwSeed = pChar->GetRandomSeed();
	random.SetRandomSeed( dwSeed + m_iCurFireCount );

	int iRandX = random.Random( m_iRandomRange*2 ) - m_iRandomRange;
	int iRandZ = random.Random( m_iRandomRange*2 ) - m_iRandomRange;

	D3DXVECTOR3 vTargetPos = m_vCirclePos;

	vTargetPos.x += (float)iRandX;
	vTargetPos.z += (float)iRandZ;

	float fGapDistance = m_fWeaponGapDistance;
	D3DXVECTOR3 vPos = vTargetPos + (vCharDir * fGapDistance);
	D3DXVECTOR3 vDir = vPos - vCharPos;
	//vDir.y = 0.0f;

	float fRate = FLOAT1;
	float fRange = D3DXVec3Length( &vDir );
	D3DXVec3Normalize( &vDir, &vDir );

	float fCurWeaponFloatPower = 0.0f;
	float fCurWeaponMoveSpeed = 0.0f;
	if( fRange > 0.0f )
	{
		fRate = fRange / (m_fCircleRange+fGapDistance);

		// CurFloat
		fCurWeaponFloatPower = m_fWeaponFloatPower * fRate;
		fCurWeaponFloatPower = max( fCurWeaponFloatPower, m_fWeaponFloatMinPower );

		float fCurGravity = 0.0f;
		float fWeaponMoveSpeed = 0.0f;
		ioPlayStage *pStage = g_SkillMaker.GetPlayStage();
		if( pStage )
			fCurGravity = pStage->GetRoomGravity() * m_fWeaponGravityRate;

		float fFlightDuration = 0.0f;
		float fGapHeight = pChar->GetMidHeightByRate(m_fRevisionRate) - vCharPos.y;
		float fGapHeight2 = vCharPos.y - m_vCirclePos.y;
		if( fGapHeight2 > 0.0f )
		{
			fGapHeight += fGapHeight2;
			fFlightDuration = GetFlightDuration( fCurGravity, fCurWeaponFloatPower, fGapHeight );

			if( fFlightDuration > 0.0f )
				fWeaponMoveSpeed = (fRange+fGapDistance) / (fFlightDuration / FLOAT1000);

			fCurWeaponMoveSpeed = max( m_fWeaponMinSpeed, fWeaponMoveSpeed );
		}
		else if( COMPARE(fGapHeight2, -m_fUpLimitHeight, -20.0f) && fRange > m_fUpLimitRange )
		{
			float fNewGap = fabs( fGapHeight2 ) - fGapHeight;
			fFlightDuration = GetTargetTime( fCurGravity, fNewGap, fCurWeaponFloatPower );
			fRange -= 35.0f;

			if( fFlightDuration > 0.0f )
				fCurWeaponMoveSpeed = (fRange+fGapDistance) / (fFlightDuration / FLOAT1000);
		}
		else
		{
			fFlightDuration = GetFlightDuration( fCurGravity, fCurWeaponFloatPower, fGapHeight );

			if( fFlightDuration > 0.0f )
				fWeaponMoveSpeed = (fRange+fGapDistance) / (fFlightDuration / FLOAT1000);

			fCurWeaponMoveSpeed = max( m_fWeaponMinSpeed, fWeaponMoveSpeed );
		}


		m_vWeaponMoveInfo.m_vMoveDir = vDir;
		m_vWeaponMoveInfo.m_fMoveSpeed = fCurWeaponMoveSpeed;
		m_vWeaponMoveInfo.m_fFloatPower = fCurWeaponFloatPower;
	}
}

float ioCyvogueItem::GetFlightDuration( float fCurGravity, float fCurWeaponFloatPower, float fGapHeight )
{
	float fTopTime, fTopHeight, fGapTime;
	fTopTime = fTopHeight = fGapTime = 0.0f;

	float fNewGravity = fCurGravity;
	if( fNewGravity > 0.0f )
	{
		fTopTime = fCurWeaponFloatPower / fNewGravity;
		//fTopHeight = (fCurWeaponFloatPower*fTopTime) - (0.5f*fNewGravity*fTopTime*fTopTime);
	}

	float fCurFloatPower = -fCurWeaponFloatPower;
	float fCurGapHeight = 0.0f;
	while( fCurGapHeight < fGapHeight )
	{
		fCurGapHeight += fabs(fCurFloatPower * 0.01f);
		fCurFloatPower -= fNewGravity * 0.01;
		fGapTime += 0.01;
	}

	float fResult = 2 * fTopTime * FLOAT1000;
	fResult += fGapTime * FLOAT1000;
	return fResult;
}

float ioCyvogueItem::GetTargetTime( IN float fCurGravity, IN float fGapHeight, OUT float &fFloatPower )
{
	float fGapTime = 0.0f;
	float fCurFloatPower = 0;
	float fCurGapHeight = 0.0f;

	float fRevisionH = FLOAT100;
	float fTopHeight = fGapHeight + fRevisionH;

	// Top Check
	float fTopTime = 0.0f;
	do
	{
		fTopTime = fGapTime;

		fCurGapHeight += fabs(fCurFloatPower * 0.01f);
		fCurFloatPower += fCurGravity * 0.01;
		fGapTime += 0.01;
	} while( fCurGapHeight < fTopHeight );

	fFloatPower = fCurFloatPower;


	// Target Check
	fGapTime = 0.0f;
	fCurFloatPower = 0;
	fCurGapHeight = 0.0f;

	float fTargetTime = 0.18f;

	fTargetTime += fTopTime;
	fTargetTime *= FLOAT1000;
	return fTargetTime;
}

float ioCyvogueItem::GetGravity()
{
	return m_fGravity;
}

void ioCyvogueItem::ReleaseEffect( ioBaseChar *pOwner )
{
	if( m_dwCircleEffectID != 0 )
	{
		pOwner->DestroyMapEffect( m_dwCircleEffectID );
		m_dwCircleEffectID = 0;
	}

	m_dwTargetCircleStartTime = 0;
}

void ioCyvogueItem::CreateEffect( ioBaseChar *pOwner, bool bAmode, const D3DXVECTOR3& vPosTarget )
{
	ioMapEffect *pMapEffect = NULL;
	D3DXVECTOR3 vPosStart= pOwner->GetWorldPosition();
	D3DXVECTOR3 vDir( 0.0f, 0.f, FLOAT1 );// = 	vPosTarget - vPosStart;
	D3DXVec3Normalize( &vDir, &vDir );

	if( bAmode )
		pMapEffect = pOwner->CreateMapEffectWithDir( m_stTargetCircleA, vDir );
	else
		pMapEffect = pOwner->CreateMapEffectWithDir( m_stTargetCircleS, vDir );
		
	if( pMapEffect )
	{
		pMapEffect->SetWorldPosition( vPosTarget );
		m_dwCircleEffectID = pMapEffect->GetUniqueID();
		m_dwTargetCircleStartTime = FRAMEGETTIME();
	}
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ioCyvogueSpecialState::ioCyvogueSpecialState()
{
}

ioCyvogueSpecialState::~ioCyvogueSpecialState()
{
}

void ioCyvogueSpecialState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );
}

void ioCyvogueSpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );

	ioCyvogueItem *pCyvogueItem = ToCyvogueItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pCyvogueItem )
		pCyvogueItem->ClearCyvogueState( pOwner );
}

void ioCyvogueSpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioCyvogueItem *pCyvogue = ToCyvogueItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pCyvogue )
		pCyvogue->ProcessCyvogueState( pOwner );
	else
	{
		pOwner->SetState( CS_DELAY );
		return;
	}
}

void ioCyvogueSpecialState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
	return;
}

void ioCyvogueSpecialState::ProcessKeyInput( ioBaseChar* pOwner )
{
	pOwner->ChangeDirectionByInputDir( true );
}

void ioCyvogueSpecialState::GetGravity( ioBaseChar* pOwner, float &fCurGravity )
{
	ioCyvogueItem *pCyvogue = ToCyvogueItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pCyvogue )
		fCurGravity = pCyvogue->GetGravity();
}