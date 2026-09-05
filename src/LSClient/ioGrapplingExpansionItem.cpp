

#include "stdafx.h"

#include "ioPlayStage.h"
#include "ioNpcChar.h"

#include "ioGrapplingExpansionItem.h"

#include "FindPredicateImpl.h"
#include "ioTryOutCheckBuff2.h"

#include "WeaponDefine.h"

ioGrapplingExpansionItem::ioGrapplingExpansionItem()
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

	m_nCurLoopCnt = 0;
}

ioGrapplingExpansionItem::ioGrapplingExpansionItem( const ioGrapplingExpansionItem &rhs )
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
	m_PushedGrapplingInfo( rhs.m_PushedGrapplingInfo ),
	m_vA_AttackList( rhs.m_vA_AttackList ),
	m_vS_AttackList( rhs.m_vS_AttackList ),
	m_vA_AttackList_CmdFail( rhs.m_vA_AttackList_CmdFail ),
	m_vS_AttackList_CmdFail( rhs.m_vS_AttackList_CmdFail ),
	m_nMaxLoopCnt( rhs.m_nMaxLoopCnt ),
	m_Cmd_a( rhs.m_Cmd_a ),
	m_Cmd_s( rhs.m_Cmd_s ),
	m_dwInputMaxTime( rhs.m_dwInputMaxTime ),
	m_dwInputStartGapTime( rhs.m_dwInputStartGapTime ),
	m_dwInputGapTime( rhs.m_dwInputGapTime ),
	m_stTargetTryoutBuff( rhs.m_stTargetTryoutBuff ),
	m_fGrpplingCharToCharGap( rhs.m_fGrpplingCharToCharGap ),
	m_fGrpplingCharMidPosRate( rhs.m_fGrpplingCharMidPosRate )
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

ioGrapplingExpansionItem::~ioGrapplingExpansionItem()
{
}

void ioGrapplingExpansionItem::LoadProperty( ioINILoader &rkLoader )
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
	//////////////////////////////////////////////////////////////////////////
	m_nMaxLoopCnt = rkLoader.LoadInt_e( "max_grap_loop_cnt", 0 );

	for( i = 0; i < m_nMaxLoopCnt; i++ )
	{
		tagAttackInfo Info_a;

		wsprintf_e( szKey, "grap_a_gap%d", i+1 );
		Info_a.m_fGapRange = rkLoader.LoadFloat( szKey, 0.f );

		wsprintf_e( szKey, "grap_a_attack%d", i+1 );
		LoadAttackAttribute( szKey, Info_a.m_LandAttack, rkLoader );
		
		wsprintf_e( szKey, "grap_a_attacker_motion%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		Info_a.m_stAttackMotion = szBuf;

		wsprintf_e( szKey, "grap_a_attacker_motion_rate%d", i+1 );
		Info_a.m_fAttackMotionRate = rkLoader.LoadFloat( szKey, FLOAT1 );

		wsprintf_e( szKey, "grap_a_target_motion%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		Info_a.m_stTargetMotion = szBuf;

		wsprintf_e( szKey, "grap_a_target_motion_rate%d", i+1 );
		Info_a.m_fTargetMotionRate = rkLoader.LoadFloat( szKey, FLOAT1 );
		m_vA_AttackList.push_back( Info_a );

		tagAttackInfo Info_s;

		wsprintf_e( szKey, "grap_s_gap%d", i+1 );
		Info_s.m_fGapRange = rkLoader.LoadFloat( szKey, 0.f );

		wsprintf_e( szKey, "grap_s_attack%d", i+1 );
		LoadAttackAttribute( szKey, Info_s.m_LandAttack, rkLoader );

		wsprintf_e( szKey, "grap_s_attacker_motion%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		Info_s.m_stAttackMotion = szBuf;

		wsprintf_e( szKey, "grap_s_attacker_motion_rate%d", i+1 );
		Info_s.m_fAttackMotionRate = rkLoader.LoadFloat( szKey, FLOAT1 );

		wsprintf_e( szKey, "grap_s_target_motion%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		Info_s.m_stTargetMotion = szBuf;

		wsprintf_e( szKey, "grap_s_target_motion_rate%d", i+1 );
		Info_s.m_fTargetMotionRate = rkLoader.LoadFloat( szKey, FLOAT1 );
		m_vS_AttackList.push_back( Info_s );
		//////////////////////////////////////////////////////////////////////////
		tagAttackInfo Info_a_fail;

		wsprintf_e( szKey, "fail_grap_a_gap%d", i+1 );
		Info_a_fail.m_fGapRange = rkLoader.LoadFloat( szKey, 0.f );

		wsprintf_e( szKey, "fail_grap_a_attack%d", i+1 );
		LoadAttackAttribute( szKey, Info_a_fail.m_LandAttack, rkLoader );

		wsprintf_e( szKey, "fail_grap_a_attacker_motion%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		Info_a_fail.m_stAttackMotion = szBuf;

		wsprintf_e( szKey, "fail_grap_a_attacker_motion_rate%d", i+1 );
		Info_a_fail.m_fAttackMotionRate = rkLoader.LoadFloat( szKey, FLOAT1 );

		wsprintf_e( szKey, "fail_grap_a_target_motion%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		Info_a_fail.m_stTargetMotion = szBuf;

		wsprintf_e( szKey, "fail_grap_a_target_motion_rate%d", i+1 );
		Info_a_fail.m_fTargetMotionRate = rkLoader.LoadFloat( szKey, FLOAT1 );
		m_vA_AttackList_CmdFail.push_back( Info_a_fail );

		tagAttackInfo Info_s_fail;

		wsprintf_e( szKey, "fail_grap_s_gap%d", i+1 );
		Info_s_fail.m_fGapRange = rkLoader.LoadFloat( szKey, 0.f );

		wsprintf_e( szKey, "fail_grap_s_attack%d", i+1 );
		LoadAttackAttribute( szKey, Info_s_fail.m_LandAttack, rkLoader );

		wsprintf_e( szKey, "fail_grap_s_attacker_motion%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		Info_s_fail.m_stAttackMotion = szBuf;

		wsprintf_e( szKey, "fail_grap_s_attacker_motion_rate%d", i+1 );
		Info_s_fail.m_fAttackMotionRate = rkLoader.LoadFloat( szKey, FLOAT1 );

		wsprintf_e( szKey, "fail_grap_s_target_motion%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		Info_s_fail.m_stTargetMotion = szBuf;

		wsprintf_e( szKey, "fail_grap_s_target_motion_rate%d", i+1 );
		Info_s_fail.m_fTargetMotionRate = rkLoader.LoadFloat( szKey, FLOAT1 );
		m_vS_AttackList_CmdFail.push_back( Info_s_fail );
	}

	for( i = 0; i < m_nMaxLoopCnt; i++ )
	{
		//Cmd
		ioHashString stValue;
		wsprintf_e( szKey, "cmd_a_%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_COMMAND_CNT );
		stValue = szBuf;
		m_Cmd_a.push_back( stValue );

		wsprintf_e( szKey, "cmd_s_%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_COMMAND_CNT );
		stValue = szBuf;
		m_Cmd_s.push_back( stValue );
	}

	m_dwInputMaxTime = rkLoader.LoadInt_e( "input_max_time", 0 );
	m_dwInputStartGapTime = rkLoader.LoadInt_e( "input_start_gap_time", 0 );
	m_dwInputGapTime = rkLoader.LoadInt_e( "input_gap_time", 0 );

	//Buff
	rkLoader.LoadString( "target_tryout_buff", "", szBuf, MAX_PATH );
	m_stTargetTryoutBuff = szBuf;

	//jumpAttack
	m_fGrpplingCharToCharGap	= rkLoader.LoadFloat_e( "lucifer_jump_attack_grap_target_loop_gap_range", 0.0f );
	m_fGrpplingCharMidPosRate	= rkLoader.LoadFloat_e( "lucifer_jump_attack_grap_char_height_rate", FLOAT05 );
}

ioItem* ioGrapplingExpansionItem::Clone()
{
	return new ioGrapplingExpansionItem( *this );
}

ioWeaponItem::WeaponSubType ioGrapplingExpansionItem::GetSubType() const
{
	return WST_GRAPPLING_EXPANSION_ITEM;
}

void ioGrapplingExpansionItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	ClearCancelInfo();

	m_nCurLoopCnt = 0;

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

	m_bEndReady = false;
	m_bSuccessCmdKey = false;
}

void ioGrapplingExpansionItem::CheckNormalAttack( ioBaseChar *pOwner )
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

void ioGrapplingExpansionItem::OnCharging( ioBaseChar *pOwner )
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

void ioGrapplingExpansionItem::ChangeToAttackFire( ioBaseChar *pOwner, bool bCharged )
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
		kPacket << (int)GIS_NORMAL_ATTACK;
		kPacket << m_iCurCombo;
		kPacket << bCharged;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioGrapplingExpansionItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) return;

	int nState;
	rkPacket >> nState;

	switch( nState )
	{
	case GIS_NORMAL_ATTACK:
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
	case GIS_A_M:
		{
			D3DXVECTOR3 vPos;
			D3DXQUATERNION qtRot;
			rkPacket >> m_nCurLoopCnt;
			rkPacket >> vPos;
			rkPacket >> qtRot;
			
			pOwner->SetWorldPosition( vPos );
			pOwner->SetTargetRot( qtRot );

			ChangeGrappling_A_AttackMotion( pOwner );
		}
		break;
	case GIS_S_M:
		{
			D3DXVECTOR3 vPos;
			D3DXQUATERNION qtRot;
			rkPacket >> m_nCurLoopCnt;
			rkPacket >> vPos;
			rkPacket >> qtRot;

			pOwner->SetWorldPosition( vPos );
			pOwner->SetTargetRot( qtRot );

			ChangeGrappling_S_AttackMotion( pOwner );
		}
		break;
	case GIS_L_A:
		{
			D3DXVECTOR3 vPos;
			D3DXQUATERNION qtRot;
			rkPacket >> m_nCurLoopCnt;
			rkPacket >> m_bSuccessCmdKey;
			rkPacket >> vPos;
			rkPacket >> qtRot;

			pOwner->SetWorldPosition( vPos );
			pOwner->SetTargetRot( qtRot );

			ChangeGrapplingLandAttack( pOwner );
		}
		break;
	}
	
}

void ioGrapplingExpansionItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	int nState;
	rkPacket >> nState;

	D3DXQUATERNION qtRot;
	rkPacket >> qtRot;

	D3DXVECTOR3 vWorldPos;
	rkPacket >> vWorldPos;

	m_bTargetRot = false;
	pOwner->SetTargetRot( qtRot );
	pOwner->SetWorldPosition( vWorldPos);

	if( nState == GICS_GRAP_JUMP )
	{
		rkPacket >> m_GrapplingTargetName;

		SetItemGrapplingState( pOwner, m_GrapplingTargetName );
		return;
	}

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

	//if( m_CurDirKey != ioUserKeyInput::DKI_NONE )
		//pOwner->SetCurMoveSpeed( fMoveSpeed );
	pOwner->SetCurMoveSpeed( FLOAT360 );
	//else
		//pOwner->SetCurMoveSpeed( 0.f );
}

void ioGrapplingExpansionItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
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

bool ioGrapplingExpansionItem::IsChargeAttackState( ioBaseChar *pOwner )
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

bool ioGrapplingExpansionItem::IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck )
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

void ioGrapplingExpansionItem::CheckReserve( ioBaseChar *pOwner )
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

bool ioGrapplingExpansionItem::ProcessCancel( ioBaseChar *pOwner )
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

bool ioGrapplingExpansionItem::SetJumpAttack( ioBaseChar *pOwner, int iAniID, float fTimeRate, AttackType eType )
{
	if( !pOwner ) return false;

	DWORD dwCurTime = FRAMEGETTIME();
	ClearAirJump();

	AttackType eExtendType = AT_JUMP_ATTACK;

	const AttackAttribute *pAttr = GetAttackAttribute( eExtendType );
	if( !pAttr )
	{
		LOG.PrintTimeAndLog( 0, "ioGrapplingExpansionItem::SetJumpAttack - AttackAttribute Not Exist" );
		return false;
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	iAniID = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioGrapplingExpansionItem::SetJumpAttack - JumpAttack Animation Not Exist" );
		return false;
	}

	fTimeRate   = pAttr->m_fAttackAniRate;
	DWORD dwPreDelay  = pAttr->m_dwPreDelay;

	// For AirJumpPower
	SetAirJump( pOwner, iAniID, fTimeRate );

	return false;
}

void ioGrapplingExpansionItem::ProcessJumpAttack( ioBaseChar *pOwner, float fHeightGap )
{
	if( m_pExtendJump )
	{
		m_pExtendJump->ProcessJumpAttack( pOwner, fHeightGap );
	}

	CheckAirJump( pOwner );
}

void ioGrapplingExpansionItem::FindGrappling( ioBaseChar *pOwner )
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

bool ioGrapplingExpansionItem::IsEnableGrapplingState( ioBaseChar *pOwner, const ioHashString &szTarget )
{
	if( !IsChargeAttackState( pOwner ) )
		return false;

	if( m_GrapplingTargetName.IsEmpty() )
		return false;

	if( m_GrapplingTargetName == szTarget )
		return true;

	return false;
}

bool ioGrapplingExpansionItem::IsEnableGrapplingMoveState( ioBaseChar *pOwner )
{
	if( !pOwner ) return false;

	if( pOwner->GetState() != CS_GRAPPLING_EXPANSION )
		return false;

	switch( m_GrapplingState )
	{
	case GS_MOVE:
		if(	m_CurDirKey != ioUserKeyInput::DKI_NONE )		
			return true;
	}

	return false;
}

void ioGrapplingExpansionItem::SetGrapplingState( ioBaseChar *pOwner, const ioHashString &szTarget )
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

	pOwner->SetState( CS_GRAPPLING_EXPANSION );

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

void ioGrapplingExpansionItem::ChangeGrapplingMove( ioBaseChar *pOwner, bool bRotate )
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

	if( bRotate )
		pOwner->SetTargetRotToRotate( qtTargetRot, true, false );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_GRAPPLING_SYNC );
		kPacket << pOwner->GetCharName();
		kPacket << GRAPPLING_ATTACKER;
		kPacket << GS_MOVE;
		kPacket << qtCurRot;
		kPacket << bRotate;
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

void ioGrapplingExpansionItem::ChangeGrapplingPush( ioBaseChar *pOwner )
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

void ioGrapplingExpansionItem::ChangeGrapplingCancel( ioBaseChar *pOwner )
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

	ioBaseChar *pTarget = pOwner->GetBaseChar( m_CurGrapplingTargetName );
	if( pTarget )
		pTarget->ChangeWoundGrapplingCancelState();

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


void ioGrapplingExpansionItem::ChangeGrappling_A_AttackMotion( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) 
		return;

	ioBaseChar *pTarget = pOwner->GetBaseChar( m_CurGrapplingTargetName );
	if ( !pTarget )
	{
		pOwner->SetState( CS_DELAY );
		return;
	}

	if(	pOwner->IsNeedProcess() && ( !pTarget || pTarget->GetState() != CS_GRAPPLING_WOUNDED || pTarget->GetGrapplingAttacker() != pOwner->GetCharName() ) )
	{
		pOwner->SetState( CS_DELAY );
		return;
	}

	m_PreDirKey = ioUserKeyInput::DKI_NONE;

	/*if( m_GrapplingState == GS_ATTACK_MOTION )
		return;*/

	memset( m_CurCmdInfo, 0, MAX_COMMAND_CNT );

	m_nCurLoopCnt++;
	m_iCurCmdIndex = 0;

	m_GrapAttackType = GAT1_MOTION;

	DWORD dwCurTime = FRAMEGETTIME();

	m_dwInputCheckTime = dwCurTime + m_dwInputStartGapTime;
	m_dwInputStartTime = m_dwInputCheckTime;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	ioHashString stMotion = m_vA_AttackList[ m_nCurLoopCnt - 1 ].m_stAttackMotion;
	float fRate = m_vA_AttackList[ m_nCurLoopCnt - 1 ].m_fAttackMotionRate;

	int iAniID = pGrp->GetAnimationIdx( stMotion );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fRate );

	m_GrapplingState = GS_ATTACK_MOTION;
	m_dwGrapplingAttackEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fRate;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)GIS_A_M;
		kPacket << m_nCurLoopCnt - 1;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	// for NPC
	bool bNPC = false;
	if( ToNpcChar( pTarget ) )
	{
		bNPC = true;
	}

	pTarget->AddNewBuff( m_stTargetTryoutBuff, pOwner->GetCharName(), "", NULL );

	// local or 훈련소 or NPC
	/*if( !P2PNetwork::IsNetworkPlaying() || !pOwner->GetCreator()->IsNetworkMode() || bNPC )
	{
		if( pTarget->GetState() == CS_GRAPPLING_WOUNDED )
		{
			pTarget->ChangeWoundGrapplingMoveState();
		}
	}*/

	pGrp = pTarget->GetGroup();
	if( !pGrp )
	{
		LOG.PrintTimeAndLog( 0, "ChangeGrappling AttackMotion Error _ 1");
		return;
	}

	pTarget->SetWoundGrapplingState( GS_ATTACK_MOTION );

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	stMotion = m_vA_AttackList[ m_nCurLoopCnt - 1 ].m_stTargetMotion;
	fRate = m_vA_AttackList[ m_nCurLoopCnt - 1 ].m_fTargetMotionRate;

	iAniID = pGrp->GetAnimationIdx( stMotion );
	
	if( iAniID <= 0 )
	{
		LOG.PrintTimeAndLog( 0, "ChangeGrappling AttackMotion Error _ 2");
	}

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fRate );

	m_bEndReady = false;
	m_bSuccessCmdKey = false;
}


void ioGrapplingExpansionItem::ChangeGrappling_S_AttackMotion( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) 
		return;

	ioBaseChar *pTarget = pOwner->GetBaseChar( m_CurGrapplingTargetName );
	if ( !pTarget )
	{
		pOwner->SetState( CS_DELAY );
		return;
	}
	if(pOwner->IsNeedProcess() && (	!pTarget || pTarget->GetState() != CS_GRAPPLING_WOUNDED || pTarget->GetGrapplingAttacker() != pOwner->GetCharName() ) )
	{
		pOwner->SetState( CS_DELAY );
		return;
	}

	m_PreDirKey = ioUserKeyInput::DKI_NONE;

	/*if( m_GrapplingState == GS_ATTACK_MOTION )
	return;*/

	memset( m_CurCmdInfo, 0, MAX_COMMAND_CNT );

	m_nCurLoopCnt++;
	m_iCurCmdIndex = 0;

	m_GrapAttackType = GAT2_MOTION;

	DWORD dwCurTime = FRAMEGETTIME();

	m_dwInputCheckTime = dwCurTime + m_dwInputStartGapTime;
	m_dwInputStartTime = m_dwInputCheckTime;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	ioHashString stMotion = m_vS_AttackList[ m_nCurLoopCnt - 1 ].m_stAttackMotion;
	float fRate = m_vS_AttackList[ m_nCurLoopCnt - 1 ].m_fAttackMotionRate;

	int iAniID = pGrp->GetAnimationIdx( stMotion );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fRate );

	m_GrapplingState = GS_ATTACK_MOTION;
	m_dwGrapplingAttackEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fRate;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)GIS_S_M;
		kPacket << m_nCurLoopCnt - 1;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	// for NPC
	bool bNPC = false;
	if( ToNpcChar( pTarget ) )
	{
		bNPC = true;
	}

	pTarget->AddNewBuff( m_stTargetTryoutBuff, pOwner->GetCharName(), "", NULL );

	// local or 훈련소 or NPC
	/*if( !P2PNetwork::IsNetworkPlaying() || !pOwner->GetCreator()->IsNetworkMode() || bNPC )
	{
		if( pTarget->GetState() == CS_GRAPPLING_WOUNDED )
		{
			pTarget->ChangeWoundGrapplingMoveState();
		}
	}*/

	pGrp = pTarget->GetGroup();
	if( !pGrp ) 
	{
		LOG.PrintTimeAndLog( 0, "ChangeGrappling AttackMotion Error _ 3");
		return;
	}

	pTarget->SetWoundGrapplingState( GS_ATTACK_MOTION );

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	stMotion = m_vS_AttackList[ m_nCurLoopCnt - 1 ].m_stTargetMotion;
	fRate = m_vS_AttackList[ m_nCurLoopCnt - 1 ].m_fTargetMotionRate;

	iAniID = pGrp->GetAnimationIdx( stMotion );

	if( iAniID <= 0 )
	{
		LOG.PrintTimeAndLog( 0, "ChangeGrappling AttackMotion Error _ 4");
	}
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fRate );

	m_bEndReady = false;
	m_bSuccessCmdKey = false;
}


void ioGrapplingExpansionItem::ChangeGrapplingLandAttack( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) 
		return;

	if( m_GrapAttackType == GAT1_MOTION )
		m_GrapAttackType = GAT1_LAND_ATTACK;
	else
		m_GrapAttackType = GAT2_LAND_ATTACK;

	DWORD dwCurTime = FRAMEGETTIME();

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	m_bEndReady = false;

	if( pOwner->IsNeedProcess() )
	{
		m_bSuccessCmdKey = false;
		if( m_bIsCmd_A_Routine )
		{
			if( strcmp( m_Cmd_a[m_nCurLoopCnt-1].c_str(), m_CurCmdInfo ) == 0 )
				m_bSuccessCmdKey = true;
			else
				m_bEndReady = true;
		}
		else
		{
			if( strcmp( m_Cmd_s[m_nCurLoopCnt-1].c_str(), m_CurCmdInfo ) == 0 )
				m_bSuccessCmdKey = true;
			else
				m_bEndReady = true;
		}
		

		ioBaseChar* pTarget = pOwner->GetBaseChar( m_CurGrapplingTargetName );
		if( pTarget )
		{
			ioBuff *pTargetBuff = pTarget->GetBuff( m_stTargetTryoutBuff, pOwner->GetCharName() );
			if( !pTargetBuff )
				m_bSuccessCmdKey = false;
		}
	}

	ioBaseChar* pTarget = pOwner->GetBaseChar( m_CurGrapplingTargetName );
	if( pTarget )
	{
		ioBuff *pTargetBuff = pTarget->GetBuff( m_stTargetTryoutBuff, pOwner->GetCharName() );
		if( pTargetBuff )
		{
			ioTryOutCheckBuff2* pCheckBuff = ToTryOutCheckBuff2( pTargetBuff );
			if( pCheckBuff )
				pCheckBuff->ResetCount();
		}
	}

	ioHashString stMotion;
	float fRate;

	m_GrapplingState = GS_LAND_ATTACK;
	int iAniID = -1;

	//막타
	/*if( m_nCurLoopCnt >= (int)m_vA_AttackList.size() )
		m_bEndReady = true;*/

	if( m_bSuccessCmdKey  )
	{
		if( m_GrapAttackType == GAT1_LAND_ATTACK )
		{
			stMotion = m_vA_AttackList[ m_nCurLoopCnt - 1 ].m_LandAttack.m_AttackAnimation;
			fRate = m_vA_AttackList[ m_nCurLoopCnt - 1 ].m_LandAttack.m_fAttackAniRate;
		}
		else
		{
			stMotion = m_vS_AttackList[ m_nCurLoopCnt - 1 ].m_LandAttack.m_AttackAnimation;
			fRate = m_vS_AttackList[ m_nCurLoopCnt - 1 ].m_LandAttack.m_fAttackAniRate;
		}

		iAniID = pGrp->GetAnimationIdx( stMotion );
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fRate );

		pOwner->IncreaseWeaponIndexBase();
		DWORD dwCurBaseIndex = pOwner->GetWeaponIndexBase();

		if( m_GrapAttackType == GAT1_LAND_ATTACK )
		{
			pOwner->RefreshFireTimeListWithBase( dwCurBaseIndex,
				iAniID,
				m_vA_AttackList[ m_nCurLoopCnt - 1 ].m_LandAttack.m_vWeaponInfoList,
				FTT_EXTEND_ATTACK,
				fRate,
				m_vA_AttackList[ m_nCurLoopCnt - 1 ].m_LandAttack.m_dwPreDelay );
		}
		else
		{
			pOwner->RefreshFireTimeListWithBase( dwCurBaseIndex,
				iAniID,
				m_vS_AttackList[ m_nCurLoopCnt - 1 ].m_LandAttack.m_vWeaponInfoList,
				FTT_EXTEND_ATTACK,
				fRate,
				m_vS_AttackList[ m_nCurLoopCnt - 1 ].m_LandAttack.m_dwPreDelay );
		}
	}
	else
	{
		m_bEndReady = true;

		if( m_GrapAttackType == GAT1_LAND_ATTACK )
		{
			stMotion = m_vA_AttackList_CmdFail[ m_nCurLoopCnt - 1 ].m_LandAttack.m_AttackAnimation;
			fRate = m_vA_AttackList_CmdFail[ m_nCurLoopCnt - 1 ].m_LandAttack.m_fAttackAniRate;
		}
		else
		{
			stMotion = m_vS_AttackList_CmdFail[ m_nCurLoopCnt - 1 ].m_LandAttack.m_AttackAnimation;
			fRate = m_vS_AttackList_CmdFail[ m_nCurLoopCnt - 1 ].m_LandAttack.m_fAttackAniRate;
		}

		iAniID = pGrp->GetAnimationIdx( stMotion );
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fRate );

		pOwner->IncreaseWeaponIndexBase();
		DWORD dwCurBaseIndex = pOwner->GetWeaponIndexBase();

		if( m_GrapAttackType == GAT1_LAND_ATTACK )
		{
			pOwner->RefreshFireTimeListWithBase( dwCurBaseIndex,
				iAniID,
				m_vA_AttackList_CmdFail[ m_nCurLoopCnt - 1 ].m_LandAttack.m_vWeaponInfoList,
				FTT_EXTEND_ATTACK,
				fRate,
				m_vA_AttackList_CmdFail[ m_nCurLoopCnt - 1 ].m_LandAttack.m_dwPreDelay );
		}
		else
		{
			pOwner->RefreshFireTimeListWithBase( dwCurBaseIndex,
				iAniID,
				m_vS_AttackList_CmdFail[ m_nCurLoopCnt - 1 ].m_LandAttack.m_vWeaponInfoList,
				FTT_EXTEND_ATTACK,
				fRate,
				m_vS_AttackList_CmdFail[ m_nCurLoopCnt - 1 ].m_LandAttack.m_dwPreDelay );
		}
	}

	if( m_nCurLoopCnt >= (int)m_vA_AttackList.size() )
		m_bEndReady = true;

	m_dwGrapplingAttackEndTime = dwCurTime;
	m_dwGrapplingAttackEndTime += pGrp->GetAnimationFullTime( iAniID ) * fRate;

	//SendPacket
	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)GIS_L_A;
		kPacket << m_nCurLoopCnt;
		kPacket << m_bSuccessCmdKey;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}


void ioGrapplingExpansionItem::CheckGrapplingState( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	if( m_bEndReady && m_GrapplingState != GS_LAND_ATTACK )
	{
		pOwner->SetState( CS_DELAY, false );
		return;
	}

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
		if( pOwner->IsNeedProcess() )
		{
			/*if( m_nCurLoopCnt > 0 && m_nCurLoopCnt >= (int)m_vA_AttackList.size() )
			{
				ChangeGrapplingPush( pOwner );
				return;
			}*/

			if( m_nCurLoopCnt == 0 && pOwner->IsJumpKeyDown() && (int)m_vA_AttackList.size() )
			{
				ChangeGrappling_A_AttackMotion( pOwner );
				m_bIsCmd_A_Routine = true;
				return;
			}
			else if( m_nCurLoopCnt == 0 && pOwner->IsDefenseKeyDown() && (int)m_vS_AttackList.size() )
			{
				ChangeGrappling_S_AttackMotion( pOwner );
				m_bIsCmd_A_Routine = false;
				return;
			}

			if( m_nCurLoopCnt >= 1 && m_nCurLoopCnt <= (int)m_Cmd_a.size() )
			{
				if( m_bIsCmd_A_Routine )
				{
					ioHashString stCmd = m_Cmd_a[m_nCurLoopCnt-1];

						if( strcmp( stCmd.c_str(), m_CurCmdInfo ) == 0 )
						{
							ChangeGrappling_A_AttackMotion( pOwner );
							return;
						}
				}
				else
				{
					ioHashString stCmd = m_Cmd_s[m_nCurLoopCnt-1];

					if( strcmp( stCmd.c_str(), m_CurCmdInfo ) == 0 )
					{
						ChangeGrappling_S_AttackMotion( pOwner );
						return;
					}
				}
			}
			

			//A,S키후 커맨드가 없다면
			if( m_nCurLoopCnt >= 1 )
			{
				//타겟 버프가 없다면 cancel
				/*ioBaseChar *pTarget = pOwner->GetBaseChar( m_CurGrapplingTargetName );
				if( pTarget )
				{
					ioBuff *pTargetBuff = pTarget->GetBuff( m_stTargetTryoutBuff, pOwner->GetCharName() );
					if( pTargetBuff == 0 )
					{
						ChangeGrapplingCancel( pOwner );
						return;
					}
				}

				//있다면 던지기
				ChangeGrapplingPush( pOwner );*/
				pOwner->SetState( CS_DELAY );
				return;
			}
		}
		
		OnGrapplingMove( pOwner );
		break;
	case GS_ATTACK_MOTION:
		if( pOwner->IsNeedProcess() )
			CheckCmdInput( pOwner );

		//if( m_GrapAttackType == GAT1_MOTION || m_GrapAttackType == GAT2_MOTION)
		OnGrapplingAttackMotion( pOwner );
		/*else
			OnGrapplingLandAttack( pOwner );*/

		break;
	case GS_LAND_ATTACK:
		OnGrapplingLandAttack( pOwner );
		break;
	case GS_PUSH:
		OnGrapplingPush( pOwner );
		break;
	case GS_CANCEL:
		OnGrapplingCancel( pOwner );
		break;
	}
}


void ioGrapplingExpansionItem::SetEndGrappling( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;
	ioBaseChar *pTarget = pOwner->GetBaseChar( m_GrapplingTargetName );
	if( pTarget )
	{
		ioBuff *pBuff = pTarget->GetBuff( m_stTargetTryoutBuff, pTarget->GetCharName() );
		/*ioTryOutCheckBuff2 *pTryBuff2 = ToTryOutCheckBuff2( pBuff );
		if( pTryBuff2 )
			pTryBuff2->SetReserveEndBuff();*/
		if( pBuff )
			pBuff->SetReserveEndBuff();
	}

	m_bEndReady = false;
	m_bSuccessCmdKey = false;
}


void ioGrapplingExpansionItem::OnGrapplingAttackMotion( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( pOwner->IsNeedProcess() && m_dwGrapplingAttackEndTime && m_dwGrapplingAttackEndTime < dwCurTime )
	{
		m_dwGrapplingAttackEndTime = 0;
		ChangeGrapplingLandAttack( pOwner );
		return;
	}
}


void ioGrapplingExpansionItem::OnGrapplingLandAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwGrapplingAttackEndTime < dwCurTime )
	{
		if( !m_bEndReady )
			m_GrapplingState = GS_MOVE;//ChangeGrapplingMove( pOwner, false );
		else
			m_GrapplingState = GS_END;

		return;
	}
}


void ioGrapplingExpansionItem::OnGrapplingPull( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwGrapplingPullEndTime < dwCurTime )
	{
		// change move state
		ChangeGrapplingMove( pOwner, true );
		return;
	}
}

void ioGrapplingExpansionItem::OnGrapplingMove( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( pOwner->IsNeedProcess() && !pOwner->IsAttackKeyDown() )
	{
		// change push state
		ChangeGrapplingPush( pOwner );
		return;
	}
	else if( pOwner->IsNeedProcess() && m_dwGrapplingMoveEndTime < dwCurTime )
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

void ioGrapplingExpansionItem::OnGrapplingPush( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwGrapplingPushEndTime < dwCurTime )
	{
		// change end state
		pOwner->SetState( CS_DELAY );
		return;
	}
}

void ioGrapplingExpansionItem::OnGrapplingCancel( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwGrapplingCancelEndTime < dwCurTime )
	{
		pOwner->SetState( CS_DELAY );
		return;
	}
}

void ioGrapplingExpansionItem::CheckGrapplingMoveKey( ioBaseChar *pOwner )
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
			kPacket << GICS_DIR;
			kPacket << pOwner->GetTargetRot();
			kPacket << pOwner->GetWorldPosition();
			kPacket << (int)m_CurDirKey;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioGrapplingExpansionItem::CheckGrapplingMoveAni( ioBaseChar *pOwner )
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

void ioGrapplingExpansionItem::CheckGrapplingMoveDir( ioBaseChar *pOwner )
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

void ioGrapplingExpansionItem::GetGrapplingMoveInfo( OUT int &iCurDir, OUT bool &bSetMotion )
{
	iCurDir = m_CurDirKey;
	bSetMotion = m_bSetGrapplingMoveAni;
}


float ioGrapplingExpansionItem::GetGrapplingGapRange()
{
	if( m_GrapplingState == GS_ATTACK_MOTION || m_GrapplingState == GS_LAND_ATTACK || m_GrapplingState == GS_END )
	{
		float fGap = 0.f;
		if( m_GrapAttackType == GAT1_MOTION || m_GrapAttackType == GAT1_LAND_ATTACK )
			fGap = m_vA_AttackList[ m_nCurLoopCnt - 1 ].m_fGapRange;
		else
			fGap = m_vS_AttackList[ m_nCurLoopCnt - 1 ].m_fGapRange;

		return fGap;
	}
	/*else if( m_GrapplingState == GS_LAND_ATTACK )
	{
		float fGap = 0.f;
		if( m_GrapAttackType == GAT1_MOTION || m_GrapAttackType == GAT1_LAND_ATTACK )
			fGap = m_vA_AttackList[ m_nCurLoopCnt - 1 ].m_fGapRange;
		else
			fGap = m_vS_AttackList[ m_nCurLoopCnt - 1 ].m_fGapRange;

		return fGap;
	}*/

	return m_fGrapplingGapRange;
}


/*bool ioGrapplingExpansionItem::CanUseGap()
{
	if( m_GrapplingState == GS_ATTACK_MOTION )
		return true;

	if( m_GrapplingState == GS_LAND_ATTACK && !m_bEndReady )
		return true;

	return false;
}*/


void ioGrapplingExpansionItem::CheckCmdInput( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( (m_dwInputStartTime + m_dwInputMaxTime) < dwCurTime )
		return;

	if( m_dwInputCheckTime > dwCurTime )
		return;

	int iCurMaxCmd = MAX_COMMAND_CNT - 1;
	iCurMaxCmd = max( 0, iCurMaxCmd );

	ioUserKeyInput::DirKeyInput eDirKey = pOwner->GetFourDirKey();
	if( m_PreDirKey == eDirKey )
		return;

	char szChar = '\0';
	switch( eDirKey )
	{
	case ioUserKeyInput::DKI_NONE:
		m_PreDirKey = eDirKey;
		return;
	case ioUserKeyInput::DKI_RIGHTUP:
	case ioUserKeyInput::DKI_RIGHTDOWN:
	case ioUserKeyInput::DKI_LEFTDOWN:
	case ioUserKeyInput::DKI_LEFTUP:
		return;
	case ioUserKeyInput::DKI_UP:
		szChar = '1';
		break;
	case ioUserKeyInput::DKI_RIGHT:
		szChar = '2';
		break;
	case ioUserKeyInput::DKI_DOWN:
		szChar = '3';
		break;
	case ioUserKeyInput::DKI_LEFT:
		szChar = '4';
		break;
	}

	if( !COMPARE( m_iCurCmdIndex, 0, iCurMaxCmd ) )
		return;

	m_CurCmdInfo[m_iCurCmdIndex] = szChar;

	m_PreDirKey = eDirKey;
	m_iCurCmdIndex++;

	m_dwInputCheckTime = dwCurTime+m_dwInputGapTime;

	//LOG.PrintTimeAndLog( 0,"Cmd - %c", szChar );
}


//jump attack
void ioGrapplingExpansionItem::SetJumpLandAttackAttribute( const ItemJumpAttackInfo& info )
{
	m_CurJumpAttackInfo = info;
	m_dwMotionEndTime = 0;
}


bool ioGrapplingExpansionItem::SetItemGrapplingState( ioBaseChar *pChar, const ioHashString &szTargetName )
{
	if( !pChar ) 
		return false;

	// 충돌처리 후 체크된 타겟을 m_GrapplingTargetName로 지정한다.
	m_GrapplingTargetName = szTargetName;
	//-->
	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp ) 
		return false;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	float fAniRate = FLOAT1;
	int iAniID = pGrp->GetAnimationIdx( m_CurJumpAttackInfo.m_GrapplingInfo.m_GrapplingLoopMotion );

	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, m_CurJumpAttackInfo.m_GrapplingInfo.m_fGrapplingLoopMotionRate );
	pChar->SetJumpPower( m_CurJumpAttackInfo.m_fJumpPower );
	m_ChargeState = CS_JUMP_GRAP;

	pChar->SetState( CS_GRAPPLER_JUMP_GRAP );

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pChar, kPacket );
		kPacket << GICS_GRAP_JUMP;
		kPacket << pChar->GetTargetRot();
		kPacket << pChar->GetWorldPosition();
		kPacket << m_GrapplingTargetName;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	ioBaseChar *pTarget = pChar->GetBaseChar( m_GrapplingTargetName );
	if( !pTarget ) 
		return false;

	pTarget->SetItemGrapplingState( pChar->GetCharName(), m_CurJumpAttackInfo.m_GrapplingInfo );
	return true;
}


float ioGrapplingExpansionItem::GetGrapGravity()
{
	return m_CurJumpAttackInfo.m_fGravity;
}

bool ioGrapplingExpansionItem::CheckLanding( ioBaseChar *pChar )

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


void ioGrapplingExpansionItem::StartLandAttack( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	ClearKeyReserve();
	//m_dwWitchFlyKeyReserveTime = 0;

	float fAniRate = FLOAT1;
	int iAniID = pGrp->GetAnimationIdx( m_CurJumpAttackInfo.m_LandAttackAttribute.m_AttackAnimation );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fAniRate );

	m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fAniRate;

	m_ChargeState = CS_JUMP_GRAP_LAND_ATTACK;

	/*float fKeyReserve = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fAniRate;
	if( fKeyReserve > 0.0f )
		m_dwWitchFlyKeyReserveTime = dwCurTime + fKeyReserve;*/

	DWORD dwCurBaseIndex = pOwner->GetWeaponIndexBase();
	pOwner->RefreshFireTimeListWithBase( dwCurBaseIndex,
		iAniID,
		m_CurJumpAttackInfo.m_LandAttackAttribute.m_vWeaponInfoList,
		FTT_NORMAL_ATTACK,
		fAniRate,
		0 );
}


bool ioGrapplingExpansionItem::IsLandAttackMotionEnd( DWORD dwActionStop ) const
{
	if( m_dwMotionEndTime == 0 )
		return false;

	if( m_dwMotionEndTime + dwActionStop > FRAMEGETTIME() )
		return false;

	return true;
}


D3DXVECTOR3 ioGrapplingExpansionItem::GetGrapplingTargetPos( ioBaseChar *pChar )
{
	if( m_fGrpplingCharToCharGap > 0.0f )
	{
		D3DXVECTOR3 vPos = pChar->GetMidPositionByRate( m_fGrpplingCharMidPosRate );
		D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVec3Normalize( &vDir, &vDir );

		if( m_fGrpplingCharToCharGap > 0.0f )
			vPos += m_fGrpplingCharToCharGap * vDir;

		return vPos;
	}

	return ioMath::VEC3_ZERO;
}

void ioGrapplingExpansionItem::ProcessGrapplerJumpGrapMove( ioBaseChar *pOwner )
{
	if( m_ChargeState  == CS_JUMP_GRAP )
	{
		if( CheckLanding( pOwner ) )
			StartLandAttack( pOwner );
	}
	else if( m_ChargeState == CS_JUMP_GRAP_LAND_ATTACK )
	{
		;
	}
	else 
		assert( false );
}

bool ioGrapplingExpansionItem::ProcessGrapplerFlyLandAttack( ioBaseChar *pOwner )
{
	if( !pOwner ) return false;

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwLooseTime = pOwner->GetActionStopTotalDelay();

	if( m_dwMotionEndTime > 0 && m_dwMotionEndTime+dwLooseTime < dwCurTime )
	{
		ProcessReserveKeyInput( pOwner );
		return false;
	}
	/*else if( m_dwWitchFlyKeyReserveTime > 0 && m_dwWitchFlyKeyReserveTime < dwCurTime )
	{
		CheckReserve( pOwner );
	}*/

	return true;
}


int ioGrapplingExpansionItem::GetState()
{
	return (int)m_ChargeState;
}

bool ioGrapplingExpansionItem::IsCmdCheckEnableTime()
{
	if( m_bEndReady )
		return false;

	if( m_GrapplingState == GS_LAND_ATTACK )
		return true;

	return false;
}

bool ioGrapplingExpansionItem::IsCmdSuccess()
{
	return m_bSuccessCmdKey;
}

//////////////////////////////////////////////////////////////////////////
ioGrapplerJumpAttackSpecialState::ioGrapplerJumpAttackSpecialState()
{
}

ioGrapplerJumpAttackSpecialState::~ioGrapplerJumpAttackSpecialState()
{
}

void ioGrapplerJumpAttackSpecialState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );
}

void ioGrapplerJumpAttackSpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );
}

void ioGrapplerJumpAttackSpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioGrapplingExpansionItem *pGraplerExpansionItem = ToGrapplingExpansionItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pGraplerExpansionItem )
	{
		/*float fHeightGap = GetWorldPosition().y - GetBottomHeight();

		if( m_bUseGhostState )
		{
			if( m_JumpState == JS_JUMP_ATTACK && GetBottomHeight() > 0.0f )
				m_fPreHeightForGhostState = GetBottomHeight();

			fHeightGap = GetWorldPosition().y - m_fPreHeightForGhostState;
		}*/

		pGraplerExpansionItem->ProcessGrapplerJumpGrapMove( pOwner );
	}
	else
	{
		pOwner->SetState( CS_DELAY );
		pOwner->SetFallState( true );
		return;
	}
}

void ioGrapplerJumpAttackSpecialState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
	return;
}

bool ioGrapplerJumpAttackSpecialState::ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime )
{	
	ioGrapplingExpansionItem *pGraplerExpansionItem = ToGrapplingExpansionItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( !pGraplerExpansionItem )
		return false;

	if( !pGraplerExpansionItem->ProcessGrapplerFlyLandAttack( pOwner ) )
		return true;

	/*
	if( pNakoruru->GetWitchFlyState() == ioNakoruruItem::WFS_LAND_ATTACK && pNakoruru->IsLandAttackMotionEnd( m_dwActionStopTotalDelay) )
	{
		SetState( CS_DELAY );
		return;
	}
	*/

	if( pGraplerExpansionItem->GetState() == ioGrapplingExpansionItem::CS_JUMP_GRAP )
		return false;

	return true;
}

void ioGrapplerJumpAttackSpecialState::GetGravity( ioBaseChar* pOwner, float &fCurGravity )
{
	ioGrapplingExpansionItem *pGraplerExpansionItem = ToGrapplingExpansionItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pGraplerExpansionItem && pGraplerExpansionItem->GetState() == ioGrapplingExpansionItem::CS_JUMP_GRAP )
		fCurGravity = pGraplerExpansionItem->GetGrapGravity();
}
