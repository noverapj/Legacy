
#include "stdafx.h"

#include "ioAutomatonItem.h"
#include "WeaponDefine.h"
#include "ItemDefine.h"
#include "BuffDefine.h"
#include "ioNpcChar.h"

ioAutomatonItem::ioAutomatonItem()
{
	m_ChargeState = CS_NONE;
	m_WoundedState	= WS_DELAY;

	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;
	m_nAttackCnt = 0;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	m_AttackState = AAS_NONE;
	m_fFlyCurRange = 0.0f;
	m_CurAttackInfo.Init();

	m_dwAttackStartTime = 0;
	m_dwFireMotionTime = 0;

	m_Target.Clear();

	m_vFlyMoveDir = ioMath::VEC3_ZERO;

	m_dwWeaponIndex = 0;
	m_dwStartWoundedTimeCheck = 0;
	m_fCurGauge = 0.f;
}

ioAutomatonItem::ioAutomatonItem( const ioAutomatonItem &rhs )
: ioWeaponItem( rhs ),
m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
m_AttackInfo( rhs.m_AttackInfo ),
m_AttackJumpAttributeInfo( rhs.m_AttackJumpAttributeInfo ),
m_fAttackJumpEndJumpPower( rhs.m_fAttackJumpEndJumpPower ),
m_fAirAttackTypeCheckHeight( rhs.m_fAirAttackTypeCheckHeight ),
m_fNextAttackHeightOffset( rhs.m_fNextAttackHeightOffset ),
m_JumpAttackInfo( rhs.m_JumpAttackInfo ),
m_stLockControlBuff( rhs.m_stLockControlBuff ),
m_szAttackStartBuff( rhs.m_szAttackStartBuff ),
m_dwAttachWeapon( rhs.m_dwAttachWeapon ),
m_fGrapReturnDirRate( rhs.m_fGrapReturnDirRate ),
m_fGrapReturnPosAddY( rhs.m_fGrapReturnPosAddY ),
m_fGrapReturnCheckLenth( rhs.m_fGrapReturnCheckLenth ),
m_fJumpGrapReturnDirRate( rhs.m_fJumpGrapReturnDirRate ),
m_fJumpGrapReturnPosAddY( rhs.m_fJumpGrapReturnPosAddY ),
m_fJumpGrapReturnCheckLenth( rhs.m_fJumpGrapReturnCheckLenth ),
m_szWaitLoopAni( rhs.m_szWaitLoopAni ),
m_szWaitMoveAni( rhs.m_szWaitMoveAni ),
m_AttackHitedAniCenter( rhs.m_AttackHitedAniCenter ),
m_AttackHitedAniUp( rhs.m_AttackHitedAniUp ),
m_AttackHitedAniDown( rhs.m_AttackHitedAniDown ),
m_fWaitMoveAniRate( rhs.m_fWaitMoveAniRate ),
m_fAttackHitedAniRate( rhs.m_fAttackHitedAniRate ),
m_dwWaitMoveAniTime( rhs.m_dwWaitMoveAniTime ),
m_dwAttackTargetMoveTime( rhs.m_dwAttackTargetMoveTime ),
m_GrapplingAniName( rhs.m_GrapplingAniName ),
m_fGrapAniTimeRate( rhs.m_fGrapAniTimeRate ),
m_fGrapplingJumpPower( rhs.m_fGrapplingJumpPower ),
m_fGrpplingCharToCharGap( rhs.m_fGrpplingCharToCharGap ),
m_fGrpplingCharMidPosRate( rhs.m_fGrpplingCharMidPosRate ),
m_dwGrapWeaponIdx( rhs.m_dwGrapWeaponIdx ),
m_GrapplingInfo( rhs.m_GrapplingInfo ),
m_fSpecialAttackSpeed( rhs.m_fSpecialAttackSpeed ),
m_dwRotateTime( rhs.m_dwRotateTime ),
m_LandAttackAttribute( rhs.m_LandAttackAttribute ),
m_dwWoundedKeyInputWaitTime( rhs.m_dwWoundedKeyInputWaitTime ),
m_fWoundedDashEnableHeightGap( rhs.m_fWoundedDashEnableHeightGap ),
m_szWoundedDashBuffName( rhs.m_szWoundedDashBuffName ),
m_fMaxGauge( rhs.m_fMaxGauge ),
m_fNeedGauge( rhs.m_fNeedGauge ),
m_fWoundedDashNeedGauge( rhs.m_fWoundedDashNeedGauge ),
m_fIncreaseGauge( rhs.m_fIncreaseGauge )
{
	m_ChargeState = CS_NONE;
	m_WoundedState	= WS_DELAY;

	m_dwAttackStartTime = 0;
	m_iCurCombo = 0;
	m_nAttackCnt =0;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	m_AttackState = AAS_NONE;
	m_fFlyCurRange = 0.0f;
	m_dwAttackStartTime = 0;
	m_CurAttackInfo.Init();
	m_Target.Clear();

	m_vFlyMoveDir = ioMath::VEC3_ZERO;

	m_dwMotionEndTime = 0;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_dwWeaponIndex = 0;
	m_bSetMove = false;

	m_dwStartWoundedTimeCheck = 0;
	m_fCurGauge = 0.f;
}

ioAutomatonItem::~ioAutomatonItem()
{
}

void ioAutomatonItem::LoadProperty( ioINILoader &rkLoader )
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
	m_fGrapReturnDirRate = rkLoader.LoadFloat_e( "attack_grap_returnPos_Rate", 0 );	
	m_fGrapReturnPosAddY = rkLoader.LoadFloat_e( "attack_grap_returnPos_Y_Add", 0 );
	m_fGrapReturnCheckLenth = rkLoader.LoadFloat_e("attack_grap_returnPos_CheckLength", 0);
	m_fJumpGrapReturnDirRate = rkLoader.LoadFloat_e( "jump_attack_grap_returnPos_Rate", 0 );	
	m_fJumpGrapReturnPosAddY = rkLoader.LoadFloat_e( "jump_attack_grap_returnPos_Y_Add", 0 );
	m_fJumpGrapReturnCheckLenth = rkLoader.LoadFloat_e("jump_attack_grap_returnPos_CheckLength", 0);

	rkLoader.LoadString_e( "wait_loop_ani", "", szBuf, MAX_PATH );
	m_szWaitLoopAni = szBuf;
	rkLoader.LoadString_e( "wait_move_ani", "", szBuf, MAX_PATH );
	m_szWaitMoveAni = szBuf;
	m_fWaitMoveAniRate = rkLoader.LoadFloat_e( "wait_move_ani_rate", FLOAT1 );
	m_dwWaitMoveAniTime = rkLoader.LoadInt_e( "wait_move_ani_time", 0 );
	m_dwAttackTargetMoveTime = rkLoader.LoadInt_e( "attack_target_move_time", 1000 );

	rkLoader.LoadString_e( "jump_attack_grap_ani", "", szBuf, MAX_PATH );
	m_GrapplingAniName = szBuf;
	m_fGrapAniTimeRate = rkLoader.LoadFloat_e( "jump_attack_grap_ani_time_rate", FLOAT1 );
	m_fGrapplingJumpPower = rkLoader.LoadFloat_e( "jump_attack_grap_jump_power", 0 );
	m_fGrpplingCharToCharGap	= rkLoader.LoadFloat_e( "jump_attack_grap_target_loop_gap_range", 0.0f );
	m_fGrpplingCharMidPosRate	= rkLoader.LoadFloat_e( "jump_attack_grap_char_height_rate", FLOAT05 );
	m_dwGrapWeaponIdx = rkLoader.LoadInt_e("jump_attack_grap_weaponIdx", 0);
	m_fSpecialAttackSpeed = rkLoader.LoadFloat_e( "special_Attack_speed", FLOAT500 );
	m_dwRotateTime = (DWORD)rkLoader.LoadInt_e( "fly_rotate_speed", 0 );

	rkLoader.LoadString_e( "jump_attack_grap_loop_ani", "", szBuf, MAX_PATH );
	m_GrapplingInfo.m_GrapplingLoopMotion = szBuf;
	rkLoader.LoadString_e("jump_attack_grap_loop2_ani", "", szBuf, MAX_PATH );
	m_GrapplingInfo.m_GrapplingLoopMotion2 = szBuf;
	m_GrapplingInfo.m_fGrapplingLoopMotionRate = rkLoader.LoadFloat_e("jump_attack_grap_loop_aniRate", 0 );

	rkLoader.LoadString_e( "jump_land_attack_ani", "", szBuf, MAX_PATH );
	m_LandAttackAttribute.m_AttackAnimation = szBuf;
	WeaponInfo weapon;
	weapon.m_iWeaponIdx = rkLoader.LoadInt_e( "jump_land_attack", 0);
	m_LandAttackAttribute.m_vWeaponInfoList.push_back( weapon );

	m_dwWoundedKeyInputWaitTime = rkLoader.LoadInt_e( "wounded_key_input_wait_time", 0 );
	m_fWoundedDashEnableHeightGap = rkLoader.LoadFloat_e( "wounded_dash_enable_height_gap", 0.0f );
	rkLoader.LoadString_e( "wounded_dash_buff", "", szBuf, MAX_PATH );
	m_szWoundedDashBuffName = szBuf;

	m_fMaxGauge = rkLoader.LoadFloat_e( "max_gauge", FLOAT100 );
	m_fNeedGauge = rkLoader.LoadFloat_e( "need_gauge", FLOAT100 );
	m_fWoundedDashNeedGauge = rkLoader.LoadFloat_e( "wounded_dash_need_gauge", 0.f );
	if( m_fWoundedDashNeedGauge == 0.f )
		m_fWoundedDashNeedGauge = m_fNeedGauge * 2.f;

	m_fIncreaseGauge = rkLoader.LoadFloat_e( "increase_gauge", FLOAT1 );

	LoadAttackInfo( rkLoader );
}

void ioAutomatonItem::LoadAttackInfo( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";
	
	m_AttackInfo.Init();

	// full charging
	rkLoader.LoadString_e( "full_attack_actionAni_Center", "", szBuf, MAX_PATH );
	m_AttackInfo.m_StartAniCenter = szBuf;
	rkLoader.LoadString_e( "full_attack_actionAni_Up", "", szBuf, MAX_PATH );
	m_AttackInfo.m_StartAniUp = szBuf;
	rkLoader.LoadString_e( "full_attack_actionAni_Down", "", szBuf, MAX_PATH );
	m_AttackInfo.m_StartAniDown = szBuf;

	//시작
	wsprintf_e( szKey, "full_attack" );
	LoadAttackAttribute( szKey, m_AttackInfo.m_FirstAttack, rkLoader );

	// 타겟 대상 검색용
	m_AttackInfo.m_TargetWoundType = (TargetWoundType)rkLoader.LoadInt_e( "full_target_wound_type", TWT_ALL );
	m_AttackInfo.m_fTargetAngle = rkLoader.LoadFloat_e( "full_target_angle", 0.0f );
	m_AttackInfo.m_fTargetRange = rkLoader.LoadFloat_e( "full_target_range", 0.0f );
	m_AttackInfo.m_fTargetUpHeight = rkLoader.LoadFloat_e( "full_target_up_height", 0.0f );
	m_AttackInfo.m_fTargetDownHeight = rkLoader.LoadFloat_e( "full_target_down_height", 0.0f );

	//잡고 있을때 모션 정보와 기다리는 시간
	rkLoader.LoadString_e( "full_delay_attack_actionAni_Center", "", szBuf, MAX_PATH );
	m_AttackInfo.m_AttackLoopAniCenter = szBuf;
	rkLoader.LoadString_e( "full_delay_attack_actionAni_Up", "", szBuf, MAX_PATH );
	m_AttackInfo.m_AttackLoopAniUp = szBuf;
	rkLoader.LoadString_e( "full_delay_attack_actionAni_Down", "", szBuf, MAX_PATH );
	m_AttackInfo.m_AttackLoopAniDown = szBuf;

	rkLoader.LoadString_e( "full_delay_attack_hitedAni_Center", "", szBuf, MAX_PATH );
	m_AttackHitedAniCenter = szBuf;
	rkLoader.LoadString_e( "full_delay_attack_hitedAni_Up", "", szBuf, MAX_PATH );
	m_AttackHitedAniUp = szBuf;
	rkLoader.LoadString_e( "full_delay_attack_hitedAni_Down", "", szBuf, MAX_PATH );
	m_AttackHitedAniDown = szBuf;

	m_AttackInfo.m_fAttackLoopAniRate = rkLoader.LoadFloat_e( "full_delay_attack_actionAni_Rate", FLOAT1 );
	m_fAttackHitedAniRate = rkLoader.LoadFloat_e( "full_delay_attack_hitedAni_Rate", FLOAT1 );
	m_AttackInfo.m_dwWaitMaxDelayTime = (DWORD)rkLoader.LoadInt_e( "full_delay_attack_max_wait_time", 0 );

	//공격끝 모션	
	rkLoader.LoadString_e( "full_attack_end_motion", "", szBuf, MAX_PATH );
	m_AttackInfo.m_AttackEndAni = szBuf;
	m_AttackInfo.m_fAttackEndAniRate = rkLoader.LoadFloat_e( "full_attack_end_motion_rate", FLOAT1 );

	// D~잡고 추가타 정보
	int nMaxAddAttack = rkLoader.LoadInt_e( "full_add_attack_cnt", 1 );
	for( int i=0; i < nMaxAddAttack; ++i )
	{
		AttackAttribute kAirAttribute;

		wsprintf_e( szBuf, "full_add_air_attack%d", i+1 );
		LoadAttackAttribute( szBuf,kAirAttribute, rkLoader );
		m_AttackInfo.m_vExtendPlusAirAttributeList.push_back( kAirAttribute );
	}
	LoadAttackAttribute_e( "full_jump_air_attack", m_AttackJumpAttributeInfo, rkLoader );
	m_fAttackJumpEndJumpPower = rkLoader.LoadFloat_e( "full_jump_air_attack_end_jump_power", 0.f );

	m_fAirAttackTypeCheckHeight = rkLoader.LoadFloat_e( "air_attack_type_check_height", 0.f );
	m_fNextAttackHeightOffset = rkLoader.LoadFloat_e( "next_attack_height_offset", 0.f );

	m_JumpAttackInfo.Init();

	//시작
	wsprintf_e( szBuf, "jump_full_attack" );
	LoadAttackAttribute( szBuf, m_JumpAttackInfo.m_FirstAttack, rkLoader );

	rkLoader.LoadString_e( "jump_full_attack_actionAni_Center", "", szBuf, MAX_PATH );
	m_JumpAttackInfo.m_StartAniCenter = szBuf;
	rkLoader.LoadString_e( "jump_full_attack_actionAni_Up", "", szBuf, MAX_PATH );
	m_JumpAttackInfo.m_StartAniUp = szBuf;
	rkLoader.LoadString_e( "jump_full_attack_actionAni_Down", "", szBuf, MAX_PATH );
	m_JumpAttackInfo.m_StartAniDown = szBuf;

	m_JumpAttackInfo.m_fAttackMoveAngle = rkLoader.LoadFloat_e( "jump_full_attack_move_angle",  0 );
	m_JumpAttackInfo.m_TargetWoundType = (TargetWoundType)rkLoader.LoadInt_e( "jump_full_target_wound_type", TWT_ALL );
	m_JumpAttackInfo.m_fTargetAngle = rkLoader.LoadFloat_e( "jump_full_target_angle", 0.0f );
	m_JumpAttackInfo.m_fTargetRange = rkLoader.LoadFloat_e( "jump_full_target_range", 0.0f );
	m_JumpAttackInfo.m_fTargetUpHeight = rkLoader.LoadFloat_e( "jump_full_target_up_height", 0.0f );
	m_JumpAttackInfo.m_fTargetDownHeight = rkLoader.LoadFloat_e( "jump_full_target_down_height", 0.0f );
	m_JumpAttackInfo.m_bLandCheck = rkLoader.LoadBool_e( "jump_full_move_landcheck", false );
	m_JumpAttackInfo.m_fEndJumpPower = rkLoader.LoadFloat_e( "jump_full_attack_move_end_jump_power", 0.f );
	m_JumpAttackInfo.m_fAttackMoveSpeed = rkLoader.LoadFloat_e( "jump_full_attack_move_speed", 0.f );

	rkLoader.LoadString_e( "jump_full_delay_attack_actionAni_Center", "", szBuf, MAX_PATH );
	m_JumpAttackInfo.m_AttackLoopAniCenter = szBuf;
	rkLoader.LoadString_e( "jump_full_delay_attack_actionAni_Up", "", szBuf, MAX_PATH );
	m_JumpAttackInfo.m_AttackLoopAniUp = szBuf;
	rkLoader.LoadString_e( "jump_full_delay_attack_actionAni_Down", "", szBuf, MAX_PATH );
	m_JumpAttackInfo.m_AttackLoopAniDown = szBuf;
	m_AttackInfo.m_fAttackLoopAniRate = rkLoader.LoadFloat_e( "jump_full_delay_attack_actionAni_Rate", FLOAT1 );

	//이동공격
	wsprintf_e( szKey, "jump_full_move_attack" );
	LoadAttackAttribute( szKey, m_JumpAttackInfo.m_MoveAttackInfo, rkLoader );
	
	nMaxAddAttack = rkLoader.LoadInt_e( "jump_full_add_attack_cnt", 1 );
	for( int i=0; i < nMaxAddAttack; ++i )
	{
		AttackAttribute kAirAttribute;

		wsprintf_e( szBuf, "jump_full_add_air_attack%d", i+1 );
		LoadAttackAttribute( szBuf,kAirAttribute, rkLoader );
		m_JumpAttackInfo.m_vExtendPlusAirAttributeList.push_back( kAirAttribute );
	}

	rkLoader.LoadString_e( "lock_control_buff", "", szBuf, MAX_PATH );
	m_stLockControlBuff = szBuf;
	rkLoader.LoadString_e( "attack_start_buff", "", szBuf, MAX_PATH );
	m_szAttackStartBuff = szBuf;
	m_dwAttachWeapon = (DWORD)rkLoader.LoadInt_e( "attach_weapon", 0 );
}

ioItem* ioAutomatonItem::Clone()
{
	return new ioAutomatonItem( *this );
}

ioWeaponItem::WeaponSubType ioAutomatonItem::GetSubType() const
{
	return WST_AUTOMATON_ITEM;
}

void ioAutomatonItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	if( !pOwner ) return;

	m_fCurChargeRate = 0.0f;
	m_fCurChargeRateForSkill = 0.0f;

	m_dwKeyReserveTime = 0;

	m_iSkillInput = -1;
	m_bEnableDefenseState = false;
	m_bEnableAttackState = false;
	m_bEnableJumpState = false;

	m_fFlyCurRange = 0.0f;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	ClearAttackState( pOwner );

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

		if( m_bChargeAutoTarget )
		{
			DWORD dwTrackingTime = FRAMEGETTIME();
			dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
			pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );
		}
		else
		{
			pOwner->StopAutoTargetTracking();
		}
	}
}

void ioAutomatonItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	switch( m_ChargeState )
	{
	case CS_CHARGING:
		OnCharging( pOwner );
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
	}
}

void ioAutomatonItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return;

	if( !pOwner->IsNeedProcess() )	
		return;

	if( pOwner->IsAttackKeyDown() && !pOwner->IsHasCrown() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			if( IsEnableGauge( m_fNeedGauge ) )
				SetSpecailAttackState( pOwner, true );
			else
				ChangeToNormalAttack( pOwner );

			return;
		}
	}
	else
	{
		ChangeToNormalAttack( pOwner );
	}
}

void ioAutomatonItem::ChangeToNormalAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->InitExtendAttackTagInfo();
	ClearCancelInfo();

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

		SetCurAttackAniForInputJump( pAttribute->m_AttackAnimation, pAttribute->m_fAttackAniRate );
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

void ioAutomatonItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	int iState;
	D3DXVECTOR3 vPos;
	D3DXQUATERNION qRot;

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

void ioAutomatonItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner )
		return;

	int iState;
	D3DXVECTOR3 vPos;
	D3DXQUATERNION qtRot;

	rkPacket >> iState;
	rkPacket >> vPos;
	rkPacket >> qtRot;

	pOwner->SetWorldPosition( vPos );
	pOwner->SetTargetRotToRotate( qtRot, true );

	switch( iState )
	{
	case AAS_START:
		{
			rkPacket >> m_vFlyMoveDir;
			rkPacket >> m_Target;

			SetSpecailAttackState( pOwner, false );
		}
		break;
	case AAS_JUMP_START:
		{
			rkPacket >> m_vFlyMoveDir;
			rkPacket >> m_Target;

			SetJumpSpecialState( pOwner, false );
		}
		break;

	case AAS_ATTACK_HITED:
		{
			rkPacket >> m_Target;

			m_AttackState = (AutomatonAttackState)iState;
		}
		break;

	case AAS_ATTACK_PRE_ADD:
		{
			rkPacket >> m_nCurAddAttackCnt;
			rkPacket >> m_vUroborusPos;

			ChangeAttackPreAdd( pOwner );
		}
		break;
	case AAS_ATTACK_ADD:
		{
			rkPacket >> m_nCurAddAttackCnt;

			ChangeAttackAdd( pOwner, false );
		}
		break;
	case AAS_JUMP_ATTACK_ADD:
		ChangeJumpAttackAdd( pOwner );
		break;
	case AAS_ATTACK_JUMP:
		ChangeAttackJump( pOwner );
		break;
	case AAS_JUMP_ATTACK_MOVE:
		ChangeJumpAttackMove( pOwner );
		break;
	case AAS_JUMP_ATTACK_MOVE_END:
		SetMoveAttackEndJump( pOwner );
		break;
	case AAS_JUMP_ATTACK_MOVE_GRAP_ATTACK:
		StartGrappAttack( pOwner );
		break;
	case AAS_JUMP_ATTACK_MOVE_GRAP:
		{
			rkPacket >> m_GrapplingTargetName;
			SetItemGrapplingState( pOwner, m_GrapplingTargetName );
		}
		break;
	case AAS_ATTACK_END:
		SetChangeAttackEndState( pOwner );
		break;
	case AAS_END:
		SetChangeEndState( pOwner, false );
		break;
	case ASS_WOUNDED_DASH:
		{
			D3DXVECTOR3 vDir;
			rkPacket >> vDir;

			pOwner->SetInputDir( vDir );
			ChangeWoundedDashMoveState( pOwner, false );
		}
		break;
	case AAS_ATTACK_WALK:
		{
			rkPacket >> m_Target;
			SetChangeAttackWalkState( pOwner );
		}
		break;
	}
}

void ioAutomatonItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	ioAttackableItem::SetExtraInfoToWeapon( pWeapon, bJumpAttack );

	ioUroborusWeapon3 *pUroborus3 = ToUroborusWeapon3( pWeapon );
	if( pUroborus3 )
	{
		m_dwWeaponIndex = pWeapon->GetWeaponIndex();

		ioBaseChar *pChar = pWeapon->GetOwner();
		if( pChar )
		{
			D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0, 0, -1 );
			D3DXVec3Normalize( &vDir, &vDir );

			if( !m_Target.IsEmpty() )
			{
				ioBaseChar *pTargetChar = pChar->GetBaseChar( m_Target );
				if( pTargetChar )
				{
					vDir = pTargetChar->GetWorldPosition() - pChar->GetWorldPosition();
					D3DXVec3Normalize( &vDir, &vDir );
				}
			}	
			
			pUroborus3->SetMoveDir( vDir );
		}
	}

	ioUroborusWeapon7 *pUroborus7 = ToUroborusWeapon7( pWeapon );
	if( pUroborus7 )
	{
		m_dwWeaponIndex = pWeapon->GetWeaponIndex();

		ioBaseChar *pChar = pWeapon->GetOwner();
		if( pChar )
		{
			D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0, 0, -1 );
			D3DXVec3Normalize( &vDir, &vDir );

			if( !m_Target.IsEmpty() )
			{
				ioBaseChar *pTargetChar = pChar->GetBaseChar( m_Target );
				if( pTargetChar )
				{
					vDir = pTargetChar->GetWorldPosition() - pChar->GetWorldPosition();
					D3DXVec3Normalize( &vDir, &vDir );
				}
			}
			else
				vDir = m_vFlyMoveDir;

			pUroborus7->SetMoveDir( vDir );
		}
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

	ioOwnerAttackWeapon* pOwnerWepon = ToOwnerAttackWeapon(pWeapon);
	if( pOwnerWepon && !m_Target.IsEmpty() )
	{
		ioBaseChar *pChar = pWeapon->GetOwner();
		if( pChar )
		{
			ioPlayStage* pStage = pChar->GetCreator();
			if( pStage )
				pOwnerWepon->SetTargetName( m_Target, pStage );
		}
		
	}
}

bool ioAutomatonItem::IsChargeAttackState( ioBaseChar *pOwner )
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

void ioAutomatonItem::OnEndExtendAttack( ioBaseChar *pOwner, bool bReleaseItem )
{
	m_ChargeState = CS_NONE;

	pOwner->SetAttackMoveEnable( false );
	pOwner->SetChargingState( false );
	pOwner->ClearDirDoubleClick();

	m_fCurChargeRate = 0.0f;
}

bool ioAutomatonItem::IsChargeAutoTarget( ioBaseChar *pOwner )
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

void ioAutomatonItem::GetAutoTargetValue( ioBaseChar *pOwner,
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

void ioAutomatonItem::SetSpecailAttackState( ioBaseChar *pOwner, bool bSendNet )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	if( pOwner->IsNeedProcess() )
		m_vFlyMoveDir = ioMath::VEC3_ZERO;

	m_nAttackCnt = 0;
	m_CurAttackInfo = m_AttackInfo;
	pOwner->SetState( CS_AUTOMATON_SPECIAL );
	CheckDecreaseGauge( m_fNeedGauge );

	if( pOwner->IsNeedProcess() )
	{
		FindFlyAttTarget( pOwner );
		if( CompareTarget( pOwner ) )
		{
			m_Target.Clear();
			m_bAimedBall = true;
		}
	}
	
	if( pOwner->IsNeedProcess() )
	{
		bool bTargetOk = false;
		D3DXVECTOR3 vTargetPos;
		D3DXVECTOR3 vDir;

		ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
		if( eNewDirKey != ioUserKeyInput::DKI_NONE )
			pOwner->SetTargetRotToDirKey( eNewDirKey );

		if( m_bAimedBall )
		{
			ioBall *pBall = pOwner->FindBall();
			if( pBall )
			{
				bTargetOk = true;
				vDir = pBall->GetMidPositionByRate() - pOwner->GetMidPositionByRate();
			}
		}
		else
		{
			ioBaseChar *pTarget = pOwner->GetBaseChar( m_Target );
			if( pTarget )			
			{
				bTargetOk = true;
				vDir = pTarget->GetMidPositionByRate(m_CurAttackInfo.m_fTargetHeightRate) - pOwner->GetMidPositionByRate(m_CurAttackInfo.m_fAttackerHeightRate);
			}
		}

		if( !bTargetOk )
		{
			vTargetPos = pOwner->GetMidPositionByRate();

			D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVec3Normalize( &vDir, &vDir );

			D3DXVECTOR3 vAxis;
			D3DXVec3Cross( &vAxis, &vDir, &D3DXVECTOR3( 0.0f, -FLOAT1, 0.0f ) );

			D3DXQUATERNION qtRot;
			D3DXQuaternionRotationAxis( &qtRot, &vAxis, DEGtoRAD(m_CurAttackInfo.m_fAttackMoveAngle) );
			vDir = qtRot * vDir;
			D3DXVec3Normalize( &vDir, &vDir );
			m_vFlyMoveDir = vDir;
		}
		else
		{
			float fRange = D3DXVec3Length( &vDir );
			D3DXVec3Normalize( &vDir, &vDir );

			D3DXVECTOR3 vRotDir = vDir;
			vRotDir.y = 0.0f;
			D3DXVec3Normalize( &vRotDir, &vRotDir );

			D3DXQUATERNION qtNewRot;
			ioMath::CalcDirectionQuaternion( qtNewRot, -vRotDir );
			pOwner->SetTargetRotToRotate( qtNewRot, true );

			m_vFlyMoveDir = vDir;
		}
	}

	ChangeAttackStart( pOwner );
	m_fFlyCurRange = 0.0f;
	m_nCurAddAttackCnt = 0;

	if( bSendNet && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_AttackState;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		kPacket << m_vFlyMoveDir;
		kPacket << m_Target;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioAutomatonItem::SetJumpSpecialState( ioBaseChar *pOwner, bool bSendNet )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	if( pOwner->IsNeedProcess() )
		m_vFlyMoveDir = ioMath::VEC3_ZERO;

	m_nAttackCnt = 0;
	m_CurAttackInfo = m_JumpAttackInfo;
	pOwner->SetState( CS_AUTOMATON_SPECIAL );
	CheckDecreaseGauge( m_fNeedGauge );

	if( pOwner->IsNeedProcess() )
	{
		FindFlyAttTarget( pOwner );
		if( CompareTarget( pOwner ) )
		{
			m_Target.Clear();
			m_bAimedBall = true;
		}
	}
	
	if( pOwner->IsNeedProcess() )
	{
		bool bTargetOk = false;
		D3DXVECTOR3 vTargetPos;
		D3DXVECTOR3 vDir;

		ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
		if( eNewDirKey != ioUserKeyInput::DKI_NONE )
			pOwner->SetTargetRotToDirKey( eNewDirKey );

		if( m_bAimedBall )
		{
			ioBall *pBall = pOwner->FindBall();
			if( pBall )
			{
				bTargetOk = true;
				vDir = pBall->GetMidPositionByRate() - pOwner->GetMidPositionByRate();
			}
		}
		else
		{
			ioBaseChar *pTarget = pOwner->GetBaseChar( m_Target );
			if( pTarget )			
			{
				bTargetOk = true;
				vDir = pTarget->GetMidPositionByRate(m_CurAttackInfo.m_fTargetHeightRate) - pOwner->GetMidPositionByRate(m_CurAttackInfo.m_fAttackerHeightRate);
			}
		}

		if( !bTargetOk )
		{
			vTargetPos = pOwner->GetMidPositionByRate();

			D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVec3Normalize( &vDir, &vDir );

			D3DXVECTOR3 vAxis;
			D3DXVec3Cross( &vAxis, &vDir, &D3DXVECTOR3( 0.0f, -FLOAT1, 0.0f ) );

			D3DXQUATERNION qtRot;
			D3DXQuaternionRotationAxis( &qtRot, &vAxis, DEGtoRAD(m_CurAttackInfo.m_fAttackMoveAngle) );
			vDir = qtRot * vDir;
			D3DXVec3Normalize( &vDir, &vDir );
			m_vFlyMoveDir = vDir;
		}
		else
		{
			float fRange = D3DXVec3Length( &vDir );
			D3DXVec3Normalize( &vDir, &vDir );

			D3DXVECTOR3 vRotDir = vDir;
			vRotDir.y = 0.0f;
			D3DXVec3Normalize( &vRotDir, &vRotDir );

			D3DXQUATERNION qtNewRot;
			ioMath::CalcDirectionQuaternion( qtNewRot, -vRotDir );
			pOwner->SetTargetRotToRotate( qtNewRot, true );

			m_vFlyMoveDir = vDir;
		}
	}

	ChangeJumpAttackStart( pOwner );
	m_fFlyCurRange = 0.0f;
	m_nCurAddAttackCnt = 0;

	if( bSendNet && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_AttackState;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		kPacket << m_vFlyMoveDir;
		kPacket << m_Target;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioAutomatonItem::ChangeAttackStart( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	m_AttackState = AAS_START;
	m_nCurAddAttackCnt = 0;

	pOwner->InitExtendAttackTagInfo();

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	//트랙킹
	ChangeTrackingUD_Fire( pOwner );

	//상중하 다 시간은 같다.
	int iAniID	     = pGrp->GetAnimationIdx( m_CurAttackInfo.m_StartAniCenter );

	DWORD dwCurBaseIndex = pOwner->IncreaseWeaponIndexBase();
	DWORD dwPreDelay = m_CurAttackInfo.m_FirstAttack.m_dwPreDelay;
	pOwner->RefreshFireTimeListWithBase( dwCurBaseIndex,
		iAniID,
		m_CurAttackInfo.m_FirstAttack.m_vWeaponInfoList,
		FTT_NORMAL_ATTACK,
		m_CurAttackInfo.m_FirstAttack.m_fAttackAniRate,
		dwPreDelay );

	m_dwKeyReserveTime = FRAMEGETTIME() + (pGrp->GetAnimationEventTime( iAniID, "key_reserve" ) * m_CurAttackInfo.m_FirstAttack.m_fAttackAniRate);
	m_dwAttackStartTime = FRAMEGETTIME(); 
	m_dwMotionEndTime = m_dwAttackStartTime;
	m_dwFireMotionTime = m_dwAttackStartTime;
	m_dwMotionEndTime += ( pGrp->GetAnimationFullTime( iAniID ) * m_CurAttackInfo.m_FirstAttack.m_fAttackAniRate );
	m_dwMotionEndTime += m_CurAttackInfo.m_FirstAttack.m_dwEndDelay;
}

void ioAutomatonItem::ChangeJumpAttackStart( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	m_AttackState = AAS_JUMP_START;
	m_nCurAddAttackCnt = 0;

	pOwner->InitExtendAttackTagInfo();
	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	//트랙킹
	ChangeTrackingUD_Fire( pOwner );

	//상중하 다 시간은 같다.
	int iAniID	     = pGrp->GetAnimationIdx( m_CurAttackInfo.m_StartAniCenter );

	DWORD dwCurBaseIndex = pOwner->IncreaseWeaponIndexBase();
	DWORD dwPreDelay = m_CurAttackInfo.m_FirstAttack.m_dwPreDelay;
	pOwner->RefreshFireTimeListWithBase( dwCurBaseIndex,
		iAniID,
		m_CurAttackInfo.m_FirstAttack.m_vWeaponInfoList,
		FTT_NORMAL_ATTACK,
		m_CurAttackInfo.m_FirstAttack.m_fAttackAniRate,
		dwPreDelay );

	m_dwKeyReserveTime = FRAMEGETTIME() + (pGrp->GetAnimationEventTime( iAniID, "key_reserve" ) * m_CurAttackInfo.m_FirstAttack.m_fAttackAniRate);
	m_dwAttackStartTime = FRAMEGETTIME(); 
	m_dwMotionEndTime = m_dwAttackStartTime;
	m_dwFireMotionTime = m_dwAttackStartTime;
	m_dwMotionEndTime += ( pGrp->GetAnimationFullTime( iAniID ) * m_CurAttackInfo.m_FirstAttack.m_fAttackAniRate );
	m_dwMotionEndTime += m_CurAttackInfo.m_FirstAttack.m_dwEndDelay;
}

void ioAutomatonItem::ChangeAttackHited( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	pOwner->ClearReservedSliding();
	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	ioBaseChar* pTarget = pOwner->GetBaseChar( m_Target );
	if( !pTarget )
		return;

	if( m_AttackState == AAS_JUMP_START )
		m_AttackState = AAS_JUMP_ATTACK_HITED;
	else
	{
		m_AttackState = AAS_ATTACK_HITED;

		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_AttackState;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		kPacket << m_Target;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioAutomatonItem::SetAirFloatStateChange( ioBaseChar *pOwner )
{
	ioBaseChar* pTarget = pOwner->GetBaseChar( m_Target );
	if( pTarget )
	{
		ioHitCountHoldBuff* pBuff1 = ToHitCountHoldBuff( pTarget->GetBuff( BT_HIT_COUNT_HOLD, pOwner->GetCharName() ) );
		if( pBuff1 )
			pBuff1->SetNotUseFloatState( true );

		ioHitCountHoldBuff2* pBuff2 = ToHitCountHoldBuff2( pTarget->GetBuff( BT_HIT_COUNT_HOLD2, pOwner->GetCharName() ) );
		if( pBuff2 )
			pBuff2->SetNotUseFloatState( true );
	}
}

void ioAutomatonItem::ChangeAttackPreAdd( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	if( (int)m_CurAttackInfo.m_vExtendPlusAirAttributeList.size() <= m_nCurAddAttackCnt )
		return;

	pOwner->ChangeDirectionByInputDir( false );

	ioBaseChar* pTargetDest = pOwner->GetBaseChar( m_Target );
	ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pOwner, m_dwWeaponIndex );
	ioUroborusWeapon3 *pUroborus3 =	ToUroborusWeapon3( pWeapon );
	if( !pUroborus3 || !pTargetDest )
		return;

	if( pOwner->IsNeedProcess() )
	{
		D3DXVECTOR3 vReturnPos;

		D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0, 0, -1 );
		D3DXVec3Normalize( &vDir, &vDir );
		vReturnPos = pOwner->GetWorldPosition() + vDir*m_fGrapReturnDirRate;
		float fMapHeight = pStage->GetMapHeight( vReturnPos.x, vReturnPos.z ) + m_fGrapReturnPosAddY;
		float fHeight = pTargetDest->GetWorldPosition().y;
		if( fMapHeight > fHeight )
			vReturnPos.y = fMapHeight;
		else
			vReturnPos.y = fHeight;

		m_vUroborusPos = vReturnPos;
	}
	D3DXVECTOR3 vDiff = pOwner->GetWorldPosition() - m_vUroborusPos;
	D3DXVec3Normalize( &vDiff, &vDiff );
	pTargetDest->SetTargetRotToDir( vDiff, true );

	pUroborus3->SetRotateMoveState( m_vUroborusPos, true );

	m_AttackState = AAS_ATTACK_PRE_ADD;
	m_dwMotionEndTime = FRAMEGETTIME() + m_dwAttackTargetMoveTime;

	if( pOwner->IsNeedProcess() )
	{
		//패킷을 보내자
		m_CurDirKey = pOwner->CheckCurDirKey();

		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_AttackState;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		kPacket << m_nCurAddAttackCnt;
		kPacket << m_vUroborusPos;

		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioAutomatonItem::ChangeAttackAdd( ioBaseChar *pOwner, bool bSendNet )
{
	if( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( (int)m_CurAttackInfo.m_vExtendPlusAirAttributeList.size() <= m_nCurAddAttackCnt )
		return;

	SetAirFloatStateChange( pOwner );

	ioBaseChar* pTargetDest = pOwner->GetBaseChar( m_Target );
	BuffStateChange( pOwner, pTargetDest );

	pOwner->ClearReservedSliding();
	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	int iAniID = 0;
	float fTimeRate = FLOAT1;
	DWORD dwPreDelay = 0;

	iAniID	    = pGrp->GetAnimationIdx( m_CurAttackInfo.m_vExtendPlusAirAttributeList[m_nCurAddAttackCnt].m_AttackAnimation );
	fTimeRate	= m_CurAttackInfo.m_vExtendPlusAirAttributeList[m_nCurAddAttackCnt].m_fAttackAniRate;
	dwPreDelay	= m_CurAttackInfo.m_vExtendPlusAirAttributeList[m_nCurAddAttackCnt].m_dwPreDelay;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	m_AttackState = AAS_ATTACK_ADD;

	DWORD dwCurBaseIndex = pOwner->IncreaseWeaponIndexBase();

	pOwner->RefreshFireTimeListWithBase( dwCurBaseIndex,
			iAniID,
			m_CurAttackInfo.m_vExtendPlusAirAttributeList[m_nCurAddAttackCnt].m_vWeaponInfoList,
			FTT_NORMAL_ATTACK,
			fTimeRate,
			dwPreDelay );

	m_dwMotionEndTime = dwCurTime;
	m_dwFireMotionTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwFireMotionTime += pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;

	if( bSendNet )
	{
		//패킷을 보내자
		m_CurDirKey = pOwner->CheckCurDirKey();

		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_AttackState;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		kPacket << m_nCurAddAttackCnt;
	
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
	m_nCurAddAttackCnt++;
}

void ioAutomatonItem::ChangeJumpAttackAdd( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	SetAirFloatStateChange( pOwner );

	ioBaseChar* pTargetDest = pOwner->GetBaseChar( m_Target );
	BuffStateChange( pOwner, pTargetDest );

	pOwner->ClearReservedSliding();
	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	int iAniID = 0;
	float fTimeRate = FLOAT1;
	DWORD dwPreDelay = 0;

	iAniID	    = pGrp->GetAnimationIdx( m_CurAttackInfo.m_vExtendPlusAirAttributeList[m_nCurAddAttackCnt].m_AttackAnimation );
	fTimeRate	= m_CurAttackInfo.m_vExtendPlusAirAttributeList[m_nCurAddAttackCnt].m_fAttackAniRate;
	dwPreDelay	= m_CurAttackInfo.m_vExtendPlusAirAttributeList[m_nCurAddAttackCnt].m_dwPreDelay;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	m_AttackState = AAS_JUMP_ATTACK_ADD;

	DWORD dwCurBaseIndex = pOwner->IncreaseWeaponIndexBase();

	pOwner->RefreshFireTimeListWithBase( dwCurBaseIndex,
			iAniID,
			m_CurAttackInfo.m_vExtendPlusAirAttributeList[m_nCurAddAttackCnt].m_vWeaponInfoList,
			FTT_NORMAL_ATTACK,
			fTimeRate,
			dwPreDelay );

	m_dwMotionEndTime = dwCurTime;
	m_dwFireMotionTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwFireMotionTime += pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;
	m_dwKeyReserveTime += pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;

	if( pOwner->IsNeedProcess() )
	{
		m_CurDirKey = pOwner->CheckCurDirKey();

		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_AttackState;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
	
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
	m_nCurAddAttackCnt++;
}

void ioAutomatonItem::ChangeJumpAttackMove( ioBaseChar *pOwner )
{
	m_bMoveIsDone = false;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	SetAirFloatStateChange( pOwner );

	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	ioBaseChar* pTargetDest = pOwner->GetBaseChar( m_Target );
	ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pOwner, m_dwWeaponIndex );
	ioUroborusWeapon7 *pUroborus7 =	ToUroborusWeapon7( pWeapon );
	if( !pUroborus7 )
	{
		SetChangeEndState( pOwner, true );
		return;
	}

	int iAniID	     = pGrp->GetAnimationIdx( m_CurAttackInfo.m_MoveAttackInfo.m_AttackAnimation );
	float fTimeRate  = m_CurAttackInfo.m_MoveAttackInfo.m_fAttackAniRate;
	pGrp->SetActionAni( iAniID, FLOAT10, FLOAT10, FLOAT1, FLOAT1/fTimeRate, 0.0f, m_CurAttackInfo.m_MoveAttackInfo.m_dwPreDelay, true );

	m_AttackState = AAS_JUMP_ATTACK_MOVE;

	DWORD dwCurBaseIndex = pOwner->IncreaseWeaponIndexBase();
	DWORD dwPreDelay = m_CurAttackInfo.m_MoveAttackInfo.m_dwPreDelay;
	pOwner->RefreshFireTimeListWithBase( dwCurBaseIndex,
		iAniID,
		m_CurAttackInfo.m_MoveAttackInfo.m_vWeaponInfoList,
		FTT_NORMAL_ATTACK,
		m_CurAttackInfo.m_MoveAttackInfo.m_fAttackAniRate,
		dwPreDelay );

	//SendPacket
	if( pOwner->IsNeedProcess() )
	{
		//패킷을 보내자
		m_CurDirKey = pOwner->CheckCurDirKey();

		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_AttackState;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();

		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioAutomatonItem::ChangeAttackJump( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	if( (int)m_CurAttackInfo.m_vExtendPlusAirAttributeList.size() <= m_nCurAddAttackCnt )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	SetAirFloatStateChange( pOwner );

	pOwner->ClearReservedSliding();
	pGrp->ClearAllLoopAni( FLOAT10, true );
	pGrp->ClearAllActionAni( FLOAT10, true );

	int iAniID = 0;
	float fTimeRate = FLOAT1;
	DWORD dwPreDelay = 0;

	iAniID	    = pGrp->GetAnimationIdx( m_AttackJumpAttributeInfo.m_AttackAnimation );
	fTimeRate	= m_AttackJumpAttributeInfo.m_fAttackAniRate;
	dwPreDelay	= m_AttackJumpAttributeInfo.m_dwPreDelay;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	m_AttackState = AAS_ATTACK_JUMP;

	DWORD dwCurBaseIndex = pOwner->IncreaseWeaponIndexBase();

	pOwner->RefreshFireTimeListWithBase( dwCurBaseIndex,
			iAniID,
			m_AttackJumpAttributeInfo.m_vWeaponInfoList,
			FTT_NORMAL_ATTACK,
			fTimeRate,
			dwPreDelay );

	m_dwMotionEndTime = dwCurTime;
	m_dwFireMotionTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwFireMotionTime += pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;

	if( pOwner->IsNeedProcess() )
	{
		//패킷을 보내자
		m_CurDirKey = pOwner->CheckCurDirKey();

		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_AttackState;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
	
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
	m_nCurAddAttackCnt++;
}

void ioAutomatonItem::ProcessSpecailState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_AttackState )
	{
	case AAS_START:	
	case AAS_JUMP_START:
		if( pOwner->IsNeedProcess() && (dwCurTime > m_dwMotionEndTime) )
			SetChangeEndState( pOwner, true );
		break;

	case AAS_ATTACK_HITED:
		if( pOwner->IsNeedSendNetwork() || !pOwner->GetCreator()->IsNetworkMode() || !(P2PNetwork::IsNetworkPlaying()) )
		{
			WaitTargetReturn( pOwner );
		}
		if( !IsTargetAttackEnable( pOwner, true ) )
		{
			SetChangeEndState( pOwner, true );
			return;
		}
		break;

	case AAS_JUMP_ATTACK_HITED:
		if( pOwner->IsNeedProcess() && pOwner->IsAttackKey() )
		{
			ChangeJumpAttackAdd( pOwner );
			return;
		}
		if( pOwner->IsNeedProcess() && (dwCurTime > m_dwMotionEndTime) )
			SetChangeEndState( pOwner, true );
		break;

	case AAS_ATTACK_WALK:
		if( !IsTargetAttackEnable( pOwner, true ) )
		{
			SetChangeEndState( pOwner, true );
			return;
		}
		if( m_dwAttackWaitLoopEndTime < dwCurTime )
		{
			SetChangeEndState( pOwner, true );
			return;
		}
		if( pOwner->IsNeedProcess() && pOwner->IsAttackKey() )
		{
			ChangeAttackPreAdd( pOwner );

			ioBaseChar* pTargetDest = pOwner->GetBaseChar( m_Target );
			if( pTargetDest && !pTargetDest->HasBuff( pOwner->GetCharName(), m_szAttackStartBuff ) )
				pTargetDest->AddNewBuff( m_szAttackStartBuff, pOwner->GetCharName(), GetName(), NULL );

			return;
		}
		CheckFindTargetMoveAni( pOwner );
		break;

	case AAS_ATTACK_PRE_ADD:
		if( !IsTargetAttackEnable( pOwner, m_nCurAddAttackCnt == 0 ) )
		{
			SetChangeEndState( pOwner, true );
			return;
		}
		if( dwCurTime > m_dwMotionEndTime )
		{
			SetChangeEndState( pOwner, true );
			return;
		}
		if( pOwner->IsNeedSendNetwork() || !pOwner->GetCreator()->IsNetworkMode() || !(P2PNetwork::IsNetworkPlaying()) )
		{
			WaitTargetAttackAdd( pOwner );
		}
		break;

	case AAS_ATTACK_ADD:
		if( !IsTargetAttackEnable( pOwner, false ) )
		{
			SetChangeEndState( pOwner, true );
			return;
		}
		if( dwCurTime > m_dwMotionEndTime )
		{
			ioBaseChar* pTargetDest = pOwner->GetBaseChar( m_Target );

			if( (int)m_CurAttackInfo.m_vExtendPlusAirAttributeList.size() <= m_nCurAddAttackCnt ||
				pTargetDest->GetWorldPosition().y + m_fNextAttackHeightOffset < pOwner->GetWorldPosition().y )
				SetChangeEndState( pOwner, true );
			else
				SetChangeAttackLoopState( pOwner );
		}
		if( dwCurTime > m_dwFireMotionTime && m_dwFireMotionTime != 0 )
		{
			m_dwFireMotionTime = 0;

			ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pOwner, m_dwWeaponIndex );
			ioUroborusWeapon3 *pUroborus3 =	ToUroborusWeapon3( pWeapon );
			if( pUroborus3 )
			{
				pUroborus3->SetRotateAttackState( true );
				pUroborus3->SetTraceTargetName( m_Target );
			}
		}
		break;

	case AAS_JUMP_ATTACK_ADD:
		if( dwCurTime > m_dwMotionEndTime )
			SetChangeEndState( pOwner, true );
		else if( pOwner->IsNeedProcess() && m_dwKeyReserveTime < dwCurTime && pOwner->IsJumpKey() )
		{
			ChangeJumpAttackMove(pOwner);
			return;
		}
		break;

	case AAS_ATTACK_JUMP:
		if( dwCurTime > m_dwMotionEndTime )
		{
			ClearAttackState( pOwner );
			pOwner->SetSKillEndJumpState( m_fAttackJumpEndJumpPower, false, true, false, true );
		}
		if( dwCurTime > m_dwFireMotionTime && m_dwFireMotionTime != 0 )
		{
			m_dwFireMotionTime = 0;

			ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pOwner, m_dwWeaponIndex );
			ioUroborusWeapon3 *pUroborus3 =	ToUroborusWeapon3( pWeapon );
			if( pUroborus3 )
				pUroborus3->SetEndState( false, false, false );
		}
		break;

	case AAS_ATTACK_LOOP:
		if( !IsTargetAttackEnable( pOwner, false ) )
		{
			SetChangeEndState( pOwner, true );
			return;
		}
		if( pOwner->IsNeedProcess() && pOwner->IsAttackKey() )
		{
			ChangeAttackPreAdd( pOwner );
			return;
		}
		if( pOwner->IsNeedProcess() && pOwner->IsJumpKey() )
		{
			ChangeAttackJump( pOwner );
			return;
		}
		if( pOwner->IsNeedProcess() && dwCurTime > m_dwAttackWaitLoopEndTime )
		{
			SetChangeAttackEndState( pOwner );
			return;
		}
		TrackingTarget( pOwner, m_CurAttackInfo.m_AttackLoopAniUp, m_CurAttackInfo.m_AttackLoopAniCenter, m_CurAttackInfo.m_AttackLoopAniDown );
		break;

	case AAS_JUMP_ATTACK_MOVE:
		if(!m_bMoveIsDone && !Process_A_AttackMove( pOwner ) )
		{
			m_bMoveIsDone = true;
			if( pOwner->IsNeedProcess() )
			{
				D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
				float fHeight = pStage->GetMapHeight( vPos.x, vPos.z );
				if( (fHeight+m_fAirAttackTypeCheckHeight) < vPos.y )
				{
					SetMoveAttackEndJump( pOwner );
					return;
				}
				else
				{
					SetChangeEndState( pOwner, true );
					return;
				}
			}
			
		}
		break;

	case AAS_JUMP_ATTACK_MOVE_GRAP_ATTACK:
		if( m_dwMotionEndTime && m_dwMotionEndTime < dwCurTime )
		{
			SetMoveAttackEndJump( pOwner );
		}
		break;
	case AAS_JUMP_ATTACK_MOVE_GRAP:
		ProcessFlyMove( pOwner );
		ProcessRotate( pOwner );
		if( CheckLanding( pOwner ) )
			StartLandAttack( pOwner );
		break;

	case AAS_ATTACK_END:
		if( dwCurTime > m_dwMotionEndTime )
			SetChangeEndState( pOwner, false );
		break;

	case AAS_END:
		break;
	}
	
	if( m_AttackState == AAS_ATTACK_HITED || m_AttackState == AAS_ATTACK_ADD || 
		m_AttackState == AAS_ATTACK_LOOP )
		CheckAttackHited( pOwner );

	if( m_AttackState == AAS_ATTACK_LOOP )
		ProcessLookRotate( pOwner );
}

bool ioAutomatonItem::IsTargetAttackEnable( ioBaseChar *pOwner, bool bCheckState )
{
	ioBaseChar* pTargetDest = pOwner->GetBaseChar( m_Target );
	if( !pTargetDest )
		return false;

	if( bCheckState && pTargetDest->GetState() != CS_FLOAT_STATE )
		return false;
	if( pTargetDest->GetState() == CS_USING_SKILL )
		return false;
	if( pTargetDest->GetState() == CS_MEPHISTO_STATE )
		return false;

	ioBuff* pControlBuff = pTargetDest->GetBuff( m_stLockControlBuff, pOwner->GetCharName() );
	if( !pControlBuff )
		return false;
	
	return true;
}

bool ioAutomatonItem::ProcessFlyMove( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return false;

	D3DXVECTOR3 vMoveDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vMoveDir, &vMoveDir );

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	// Dir Move
	if( m_CurDirKey != ioUserKeyInput::DKI_NONE )
	{
		float fCurMoveSpeed = m_fSpecialAttackSpeed * fTimePerSec;
		D3DXVECTOR3 vMoveAmt = vMoveDir * fCurMoveSpeed;

		bool bCol = false;
		if( pStage->ReCalculateMoveVectorByTerrain( pOwner, &vMoveAmt, bCol ) )
			pOwner->Translate( vMoveAmt );
	}
	return true;
}

void ioAutomatonItem::ProcessRotate( ioBaseChar *pOwner )
{
	if( m_dwRotateTime == 0 )
		return;

	if( m_CurDirKey == ioUserKeyInput::DKI_NONE )
		return;

	float fFrameGap = g_FrameTimer.GetCurFrameGap();
	float fRate = fFrameGap / m_dwRotateTime;
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

	if( fDotValue >= 0.98f )
	{
		pOwner->SetTargetRotToRotate( qtRotate, true );
		return;
	}

	D3DXVECTOR3 vAxis;
	if( fDotValue <= -0.98f )
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

bool ioAutomatonItem::IsEnableMoveState()
{
	switch ( m_AttackState )
	{
	case AAS_ATTACK_WALK:
		return true;
	}

	return false;
}

void ioAutomatonItem::WaitTargetReturn( ioBaseChar *pOwner )
{
	if( !m_Target.IsEmpty() )
	{
		ioPlayStage *pStage = pOwner->GetCreator();
		if( pStage )
		{
			ioBaseChar *pTarget = pStage->GetBaseChar( m_Target );
			if( pTarget )
			{
				D3DXVECTOR3 vCurPos = m_vUroborusPos;
				D3DXVECTOR3 vTargetPos = pTarget->GetMidPositionByRate();
				D3DXVECTOR3	vDiff = vTargetPos - vCurPos;
				float fDiffRangeSq = D3DXVec3Length( &vDiff );
				if( fDiffRangeSq < m_fGrapReturnCheckLenth )
				{
					SetChangeAttackWalkState( pOwner );
				}
			}
		}
	}
}

void ioAutomatonItem::WaitTargetAttackAdd( ioBaseChar *pOwner )
{
	if( !m_Target.IsEmpty() )
	{
		ioPlayStage *pStage = pOwner->GetCreator();
		if( pStage )
		{
			ioBaseChar *pTarget = pStage->GetBaseChar( m_Target );
			if( pTarget )
			{
				D3DXVECTOR3 vCurPos = m_vUroborusPos;
				D3DXVECTOR3 vTargetPos = pTarget->GetMidPositionByRate();
				D3DXVECTOR3	vDiff = vTargetPos - vCurPos;
				float fDiffRangeSq = D3DXVec3Length( &vDiff );
				if( fDiffRangeSq < m_fGrapReturnCheckLenth )
				{
					ChangeAttackAdd( pOwner, true );
				}
			}
		}
	}
}

void ioAutomatonItem::ProcessLookRotate( ioBaseChar *pOwner )
{
	ioBaseChar *pTarget = pOwner->GetBaseChar( m_Target );
	if( pTarget )
	{
		D3DXVECTOR3 vDiff = pTarget->GetWorldPosition() - pOwner->GetWorldPosition();
		D3DXVec3Normalize( &vDiff, &vDiff );
		pOwner->SetTargetRotToDir( vDiff, true );
	}
}

bool ioAutomatonItem::Process_A_AttackMove( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return false;

	if( m_Target.IsEmpty() )
		return false;
	
	ioBaseChar *pTarget = pStage->GetBaseChar( m_Target );
	if( !pTarget )
		return false;

	D3DXVECTOR3 vCurPos = pOwner->GetMidPositionByRate();
	D3DXVECTOR3 vTargetPos = pTarget->GetMidPositionByRate();

	D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0, 0, -1 );
	D3DXVec3Normalize( &vDir, &vDir );
	vTargetPos -= vDir*m_fJumpGrapReturnDirRate;
	vTargetPos += D3DXVECTOR3(0, m_fJumpGrapReturnPosAddY, 0 );

	D3DXVECTOR3	vDiff = vTargetPos - vCurPos;
	float fDiffRangeSq = D3DXVec3Length( &vDiff );
	if( fDiffRangeSq < m_fJumpGrapReturnCheckLenth )
	{
		StartGrappAttack( pOwner );
		return true;
	}

	DWORD dwCurTime = FRAMEGETTIME();
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	D3DXVECTOR3 vMoveDir = vDiff;
	D3DXVec3Normalize( &vMoveDir, &vMoveDir );

	if( vMoveDir == ioMath::VEC3_ZERO )
		return false;

	float fCurSpeed = m_CurAttackInfo.m_fAttackMoveSpeed;
	fCurSpeed = max( 0.0f, fCurSpeed );

	float fDistance = fCurSpeed * fTimePerSec;
	D3DXVECTOR3 vMoveAmt = vMoveDir * fDistance;

	bool bCol = false;
	if( pStage && pStage->ReCalculateMoveVectorByTerrain( pOwner, &vMoveAmt, bCol ) )
	{
		float fPreHeight = pOwner->GetBottomHeight();
		pOwner->Translate( vMoveAmt );
		float fMoveHeight = pOwner->GetBottomHeight();
		if( fPreHeight > 0.0f && fMoveHeight <= FLOAT1 )
		{
			D3DXVECTOR3 vCurPos = pOwner->GetWorldPosition();

			float fTopHeight = pStage->GetMapHeight( vCurPos.x,
				vCurPos.z,
				pOwner,
				false,
				pOwner->GetMidHeightByRate( FLOAT1 ) );

			if( fTopHeight > FLOAT1 )
			{
				vCurPos.y = max( fTopHeight, fPreHeight );
				pOwner->SetWorldPosition( vCurPos );
			}
		}
	}

	D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
	float fPreHeight = pOwner->GetBottomHeight();
	bool bJump = false;

	if( vPos.y < fPreHeight )
	{
		vPos.y = fPreHeight;
		bJump = true;
	}

	if( pOwner->CheckAxisLimit( &vPos ) )
		pOwner->SetWorldPosition( vPos );

	if(pOwner->IsNeedProcess() && bJump && m_CurAttackInfo.m_bLandCheck )
	{
		SetMoveAttackEndJump( pOwner );
		return true;
	}

	return true;
}

void ioAutomatonItem::StartGrappAttack( ioBaseChar *pOwner )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_GrapplingAniName );
	if( iAniID == -1 )	return;

	pOwner->IncreaseWeaponIndexBase();
	DWORD dwCurBaseIndex = pOwner->GetWeaponIndexBase();

	AttackAttribute attackAttribute;
	WeaponInfo weaponInfo;
	weaponInfo.m_iWeaponIdx = m_dwGrapWeaponIdx;
	attackAttribute.m_vWeaponInfoList.push_back( weaponInfo );

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pOwner->RefreshFireTimeListWithBase( dwCurBaseIndex,
			iAniID,
			attackAttribute.m_vWeaponInfoList,
			FTT_NORMAL_ATTACK,
			m_fGrapAniTimeRate,
			0 );

	m_AttackState = AAS_JUMP_ATTACK_MOVE_GRAP_ATTACK;

	m_dwMotionEndTime = FRAMEGETTIME();
	m_dwMotionEndTime += ( pGrp->GetAnimationFullTime( iAniID ) * m_fGrapAniTimeRate );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1/m_fGrapAniTimeRate);

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_AttackState;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioAutomatonItem::SetItemGrapplingState( ioBaseChar *pChar, const ioHashString &szTargetName )
{
	if( !pChar ) return false;

	// 충돌처리 후 체크된 타겟을 m_GrapplingTargetName로 지정한다.
	m_GrapplingTargetName = szTargetName;

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp ) 
		return false;
	
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	float fAniRate = FLOAT1;
	int iAniID = pGrp->GetAnimationIdx( m_GrapplingInfo.m_GrapplingLoopMotion );

	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, m_GrapplingInfo.m_fGrapplingLoopMotionRate );
	pChar->SetJumpPower( m_fGrapplingJumpPower );
	m_AttackState = AAS_JUMP_ATTACK_MOVE_GRAP;

	if( pChar->GetState() != CS_AUTOMATON_SPECIAL )
		pChar->SetState( CS_AUTOMATON_SPECIAL );

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pChar, kPacket );
		kPacket << m_AttackState;
		kPacket << pChar->GetWorldPosition();
		kPacket << pChar->GetTargetRot();
		kPacket << m_GrapplingTargetName;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	ioBaseChar *pTarget = pChar->GetBaseChar( m_GrapplingTargetName );
	if( !pTarget ) return false;

	// for NPC
	bool bNPC = false;
	if( ToNpcChar( pTarget ) )
	{
		bNPC = true;
	}

	pTarget->SetItemGrapplingState( pChar->GetCharName(), m_GrapplingInfo );
	return true;
}

D3DXVECTOR3 ioAutomatonItem::GetGrapplingTargetPos( ioBaseChar *pChar )
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

void ioAutomatonItem::SetMoveAttackEndJump( ioBaseChar *pOwner )
{
	RemoveWeapon( pOwner );

	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );
	pOwner->SetCurMoveSpeed( 0.f );
	pOwner->SetSKillEndJumpState( m_CurAttackInfo.m_fEndJumpPower, false, true, false, true );

	if( pOwner && pOwner->IsNeedProcess() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << AAS_JUMP_ATTACK_MOVE_END;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioAutomatonItem::ProcessWeaponLine( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	if( m_Target.IsEmpty() || m_dwWeaponIndex == 0 )
		return;
	
	ioBaseChar* pTargetDest = pChar->GetBaseChar( m_Target );
	ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pChar, m_dwWeaponIndex );
	ioUroborusWeapon3 *pUroborus3 =	ToUroborusWeapon3( pWeapon );
	if( !pUroborus3 )
		return;

	if( pTargetDest && IsLockState(pChar, pTargetDest) )
	{
		ioEntityGroup *pGrp = pTargetDest->GetGroup();
		if( pGrp )
		{
			ioBiped *pBiped = pGrp->GetBiped("Bip01 Spine1");
			if( pBiped )
			{
				D3DXVECTOR3 vBipPos = pBiped->GetDerivedPosition();
				pUroborus3->SetPosition( vBipPos );
			}			
		}
	}
}

void ioAutomatonItem::ClearAttackState( ioBaseChar *pOwner )
{
	m_AttackState = AAS_NONE;
	m_dwAttackStartTime = 0;
	m_CurAttackInfo.Init();
	m_vFlyMoveDir = ioMath::VEC3_ZERO;

	ioBaseChar* pTarget = pOwner->GetBaseChar( m_Target );
	if( pTarget )
	{
		ioHitCountHoldBuff*		pBuff1 = ToHitCountHoldBuff( pTarget->GetBuff( BT_HIT_COUNT_HOLD, pTarget->GetCharName() ) );
		ioHitCountHoldBuff2*	pBuff2 = ToHitCountHoldBuff2( pTarget->GetBuff( BT_HIT_COUNT_HOLD2, pTarget->GetCharName() ) );
		ioBuff* pControlBuff = pTarget->GetBuff( m_stLockControlBuff, pOwner->GetCharName() );
		ioBuff* pStartBuff = pTarget->GetBuff( m_szAttackStartBuff, pOwner->GetCharName() );

		if( pBuff1 )
			pBuff1->SetReserveEndBuff();
		if( pBuff2 )
			pBuff2->SetReserveEndBuff();
		if( pControlBuff )
			pControlBuff->SetReserveEndBuff();
		if( pStartBuff )
			pStartBuff->SetReserveEndBuff();
	}
	m_Target.Clear();
	m_dwMotionEndTime = 0;

	ioWeapon *pWeapon = g_WeaponMgr.FindWeapon(pOwner, m_dwAttachWeaponIndex);
	if( pWeapon )
	{
		pWeapon->SetWeaponDead();
		m_dwAttachWeaponIndex = 0;
	}
	RemoveWeapon(pOwner);
}

bool ioAutomatonItem::IsFlyCharCollisionSkipState()
{
	switch( m_AttackState )
	{
	case AAS_START:
	case AAS_JUMP_START:
		return true;
	}

	return false;
}

void ioAutomatonItem::OnEquiped( ioBaseChar *pOwner )
{
	ioWeaponItem::OnEquiped( pOwner );
}

void ioAutomatonItem::OnReleased( ioBaseChar *pOwner )
{
	ClearAttackState( pOwner );
	m_fCurGauge = 0.f;

	if( pOwner )
		pOwner->InitCurSkillEndNextLandRate();

	ioAttackableItem::OnReleased( pOwner );
}

bool ioAutomatonItem::IsEnableTargetState( ioBaseChar *pTarget )
{
	if( !pTarget )
		return false;

	if( pTarget->IsSystemState() )
		return false;

	if( !pTarget->IsEnableTarget() )
		return false;

	// 쓰러짐 관련 체크
	if( !pTarget->CheckTargetWoundType( m_CurAttackInfo.m_TargetWoundType ) )
		return false;

	return true;
}

void ioAutomatonItem::FindFlyAttTarget( ioBaseChar *pOwner )
{
	m_bAimedBall = false;
	m_Target.Clear();

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

	float fCurAngle = ioMath::ArrangeHead( m_CurAttackInfo.m_fTargetAngle );
	float fHalfValue = cosf( DEGtoRAD(fCurAngle*0.5f) );
	float fTargetRangeSq = m_CurAttackInfo.m_fTargetRange * m_CurAttackInfo.m_fTargetRange;

	const BaseCharList &rkCharList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;

	ioHashString szTarget;
	float fCurGap = -FLOAT1;
	for( iter=rkCharList.begin() ; iter!=rkCharList.end() ; ++iter )
	{
		ioBaseChar *pTarget = *iter;
		if( !pTarget ) continue;

		if( pTarget->GetTeam() == pOwner->GetTeam() ) continue;
		if( pTarget->GetCharName() == GetOwnerName() ) continue;
		if( pTarget->GetExperienceMode() != EMS_NONE ) continue;

		if( !IsEnableTargetState( pTarget ) )
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

		// 높이 체크
		float fHeightGap = vTargetPos.y - vCurPos.y;
		if( fHeightGap > 0.0f && fHeightGap > m_CurAttackInfo.m_fTargetUpHeight )
			continue;

		if( fHeightGap < 0.0f && fabs(fHeightGap) > m_CurAttackInfo.m_fTargetDownHeight )
			continue;

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

	if( !szTarget.IsEmpty() )
	{
		ioBaseChar *pTarget = pStage->GetBaseChar( szTarget );
		if( pTarget )
		{
			m_Target = szTarget;
		}
	}
}

bool ioAutomatonItem::CompareTarget( ioBaseChar *pOwner )
{
	if( !pOwner ) return false;
	
	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return false;

	ioBall *pBall = pOwner->FindBall();
	if( !pBall ) return false;

	D3DXVECTOR3 vCurPos = pOwner->GetMidPositionByRate();
	D3DXVECTOR3 vDiff= ioMath::VEC3_ZERO;

	float fCurAngle = ioMath::ArrangeHead( m_CurAttackInfo.m_fTargetAngle );
	float fHalfValue = cosf( DEGtoRAD(fCurAngle*0.5f) );
	float fTargetRangeSq = m_CurAttackInfo.m_fTargetRange * m_CurAttackInfo.m_fTargetRange;

	D3DXVECTOR3 vTargetPos = pBall->GetMidPositionByRate();
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
			return false;
	}

	// 높이 체크
	float fHeightGap = vTargetPos.y - vCurPos.y;
	if( fHeightGap > 0.0f && fHeightGap > m_CurAttackInfo.m_fTargetUpHeight )
		return false;

	if( fHeightGap < 0.0f && fabs(fHeightGap) > m_CurAttackInfo.m_fTargetDownHeight )
		return false;

	// 거리 체크
	if( fDiffRangeSq >= fTargetRangeSq )
		return false;

	return true;
}

bool ioAutomatonItem::IsBallTarget()
{
	return m_bAimedBall;
}

bool ioAutomatonItem::SetTarget( const ioHashString &szTargetName, ioBaseChar *pOwner )
{
	//명중 
	m_Target = szTargetName;
	ChangeAttackHited( pOwner );
	return true;
}

void ioAutomatonItem::RemoveWeapon( ioBaseChar *pOwner )
{
	ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pOwner, m_dwWeaponIndex );
	if( pWeapon )
	{
		ioUroborusWeapon3 *pUroborus3 =	ToUroborusWeapon3( pWeapon );
		if( pUroborus3 )
			pUroborus3->SetWeaponDead();

		ioUroborusWeapon7 *pUroborus7 =	ToUroborusWeapon7( pWeapon );
		if( pUroborus7 )
			pUroborus7->SetEndState( false, false, false, true );
	}
}

void ioAutomatonItem::CheckAttackHited( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioBaseChar* pTarget = pOwner->GetBaseChar(m_Target);
	if( !pTarget )
	{
		SetChangeEndState( pOwner, false );
		return;
	}
	if( !IsLockState(pOwner, pTarget) )
	{
		SetChangeEndState( pOwner, false );
		return;
	}
}

void ioAutomatonItem::SetChangeEndState( ioBaseChar *pOwner, bool bSend )
{
	ClearAttackState( pOwner );
	pOwner->SetSKillEndJumpState( 0.1f, false, true, false, false );

	if( pOwner->IsNeedSendNetwork() && bSend )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << AAS_END;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioAutomatonItem::SetChangeAttackLoopState( ioBaseChar *pOwner )
{
	if( !pOwner )
	{
		SetChangeEndState( pOwner, false );
		return;
	}

	m_AttackState = AAS_ATTACK_LOOP;
	m_dwAttackWaitLoopEndTime = FRAMEGETTIME();

	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID	     = pGrp->GetAnimationIdx( m_CurAttackInfo.m_AttackLoopAniCenter );
	float fTimeRate  = m_CurAttackInfo.m_fAttackLoopAniRate;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pGrp->SetLoopAni( m_CurAttackInfo.m_AttackLoopAniUp, 0.0f, 0.0f );
	pGrp->SetLoopAni( m_CurAttackInfo.m_AttackLoopAniDown, 0.0f, 0.0f );

	if( !pGrp->GetLoopInPlay( m_CurAttackInfo.m_AttackLoopAniCenter ) )
		pGrp->SetLoopAni( m_CurAttackInfo.m_AttackLoopAniCenter, 0.0f, FLOAT1 );

	m_dwAttackWaitLoopEndTime += m_CurAttackInfo.m_dwWaitMaxDelayTime;
}

void ioAutomatonItem::SetChangeAttackWalkState( ioBaseChar *pOwner )
{
	if( !pOwner )
	{
		SetChangeEndState( pOwner, false );
		return;
	}

	m_AttackState = AAS_ATTACK_WALK;
	m_dwAttackWaitLoopEndTime = FRAMEGETTIME() + m_dwWaitMoveAniTime;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	pOwner->ChangeDirectionByInputDir( false );

	if( pOwner->IsSettedDirection() )
	{
		pGrp->SetLoopAni( m_szWaitMoveAni, FLOAT100, FLOAT1, FLOAT1/m_fWaitMoveAniRate );
		m_bSetMove = true;
	}
	else
	{		
		pGrp->SetLoopAni( m_szWaitLoopAni, FLOAT100 );
		m_bSetMove = false;
	}
	CreateWeapon( pOwner );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_AttackState;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		kPacket << m_Target;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioAutomatonItem::CheckFindTargetMoveAni( ioBaseChar *pOwner )
{
	if( pOwner->IsSettedDirection() )
	{
		bool bChangeDir = pOwner->IsChangeDirection();
		if( bChangeDir )
		{	
			ioEntityGroup *pGrp = pOwner->GetGroup();
			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->ClearAllLoopAni( FLOAT100, true );
			pGrp->SetLoopAni( m_szWaitMoveAni, FLOAT100, FLOAT1, FLOAT1/m_fWaitMoveAniRate );

			m_bSetMove = true;
		}
	}
	else
	{		
		if( m_bSetMove )
		{
			ioEntityGroup *pGrp = pOwner->GetGroup();
			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->ClearAllLoopAni( FLOAT100, true );
			pGrp->SetLoopAni( m_szWaitLoopAni, FLOAT100 );

			m_bSetMove = false;
		}
	}
}

void ioAutomatonItem::SetChangeAttackEndState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	m_AttackState = AAS_ATTACK_END;
	ioEntityGroup *pGrp = pOwner->GetGroup();

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	int iAniID	     = pGrp->GetAnimationIdx( m_CurAttackInfo.m_AttackEndAni );
	float fTimeRate  = m_CurAttackInfo.m_fAttackEndAniRate;
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	m_dwMotionEndTime = FRAMEGETTIME() + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << m_AttackState;
		kPacket << pOwner->GetWorldPosition();
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

float ioAutomatonItem::GetAimedUpDownWeight( const D3DXVECTOR3 &vAimDir, float fMaxAngle )
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

void ioAutomatonItem::ChangeTrackingUD_Fire( ioBaseChar *pOwner )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	D3DXVECTOR3 vAimedDir;
	ioBaseChar *pTarget = pOwner->GetBaseChar( m_Target );
	if( pTarget )
		vAimedDir = pTarget->GetMidPositionByRate() - pOwner->GetMidPositionByRate();
	else
		vAimedDir = m_vFlyMoveDir;

	D3DXVec3Normalize( &vAimedDir, &vAimedDir );

	float fTimeRate  = FLOAT1;
	DWORD dwPreDelay = 0;
	if( m_AttackState == AAS_START || m_AttackState == AAS_JUMP_START )
		fTimeRate = m_CurAttackInfo.m_FirstAttack.m_fAttackAniRate;
	else if( m_AttackState == AAS_ATTACK_HITED )
		fTimeRate = m_fAttackHitedAniRate;
	else if( m_AttackState == AAS_JUMP_ATTACK_HITED )
		fTimeRate = m_CurAttackInfo.m_fAttackLoopAniRate;

	DWORD dwCurTime = FRAMEGETTIME();
	
	float fWeight = GetAimedUpDownWeight( vAimedDir, 85.0f );
	if( vAimedDir.y >= 0.0f )
		AimedFireToUp( pGrp, fWeight, fTimeRate, dwPreDelay, true );
	else
		AimedFireToDown( pGrp, fWeight, fTimeRate, dwPreDelay, true );
}

void ioAutomatonItem::AimedFireToUp( ioEntityGroup *pGrp, float fWeight, float fTimeRate, DWORD dwPreDelay, bool bHold )
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
	if( m_AttackState == AAS_START || m_AttackState == AAS_JUMP_START )
	{
		szFireUp = m_CurAttackInfo.m_StartAniUp;
		szFireCenter = m_CurAttackInfo.m_StartAniCenter;
	}
	else if( m_AttackState == AAS_ATTACK_HITED )
	{
		szFireUp = m_AttackHitedAniUp;
		szFireCenter = m_AttackHitedAniCenter;
	}
	else if( m_AttackState == AAS_JUMP_ATTACK_HITED )
	{
		szFireUp = m_CurAttackInfo.m_AttackLoopAniUp;
		szFireCenter = m_CurAttackInfo.m_AttackLoopAniCenter;
	}
	if( fWeight > 0.0f )
	{
		if( m_AttackState == AAS_START || m_AttackState == AAS_JUMP_START )
			pGrp->SetActionAni( szFireUp,FLOAT100, fFadeOut, fWeight, FLOAT1/fTimeRate,	0.0f, dwPreDelay,	bHold, bAniFx1 );
		else if( m_AttackState == AAS_ATTACK_HITED || m_AttackState == AAS_JUMP_ATTACK_HITED )
			pGrp->SetLoopAni( szFireUp, FLOAT100, fWeight, FLOAT1/fTimeRate );
	}

	if( m_AttackState == AAS_START || m_AttackState == AAS_JUMP_START )
		pGrp->SetActionAni( szFireCenter, FLOAT100, fFadeOut, FLOAT1 - fWeight, FLOAT1/fTimeRate, 0.0f, dwPreDelay, bHold, bAniFx2 );
	else if( m_AttackState == AAS_ATTACK_HITED || m_AttackState == AAS_JUMP_ATTACK_HITED )
		pGrp->SetLoopAni( szFireCenter, FLOAT100, FLOAT1 - fWeight, FLOAT1/fTimeRate );
}

void ioAutomatonItem::AimedFireToDown( ioEntityGroup *pGrp, float fWeight, float fTimeRate, DWORD dwPreDelay, bool bHold )
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
	if( m_AttackState == AAS_START || m_AttackState == AAS_JUMP_START )
	{
		szFireDown = m_CurAttackInfo.m_StartAniDown;
		szFireCenter = m_CurAttackInfo.m_StartAniCenter;
	}
	else if( m_AttackState == AAS_ATTACK_HITED )
	{
		szFireDown = m_AttackHitedAniDown;
		szFireCenter = m_AttackHitedAniCenter;
	}
	else if( m_AttackState == AAS_JUMP_ATTACK_HITED )
	{
		szFireDown = m_CurAttackInfo.m_AttackLoopAniDown;
		szFireCenter = m_CurAttackInfo.m_AttackLoopAniCenter;
	}

	if( fWeight > 0.0f )
	{
		if( m_AttackState == AAS_START || m_AttackState == AAS_JUMP_START )
			pGrp->SetActionAni( szFireCenter, FLOAT100, fFadeOut, FLOAT1 - fWeight, FLOAT1/fTimeRate, 0.0f, dwPreDelay, bHold, bAniFx1 );
		else if( m_AttackState == AAS_ATTACK_HITED || m_AttackState == AAS_JUMP_ATTACK_HITED )
			pGrp->SetLoopAni( szFireCenter, FLOAT100, FLOAT1 - fWeight, FLOAT1/fTimeRate );
	}

	if( m_AttackState == AAS_START || m_AttackState == AAS_JUMP_START )
		pGrp->SetActionAni( szFireDown, FLOAT100, fFadeOut, fWeight, FLOAT1/fTimeRate, 0.0f, dwPreDelay, bHold, bAniFx2 );
	else if( m_AttackState == AAS_ATTACK_HITED || m_AttackState == AAS_JUMP_ATTACK_HITED )
		pGrp->SetLoopAni( szFireDown, FLOAT100, fWeight, FLOAT1/fTimeRate );
}

ioUserKeyInput::DirKeyInput ioAutomatonItem::GetCurKey()
{
	return m_CurDirKey;
}

bool ioAutomatonItem::IsLockState( ioBaseChar *pCreator, ioBaseChar *pTarget )
{
	if( !pCreator || !pTarget )
		return false;

	bool bHas = pTarget->HasBuff( pCreator->GetCharName(), m_stLockControlBuff );
	return bHas;
}

void ioAutomatonItem::BuffStateChange( ioBaseChar *pCreator, ioBaseChar *pTarget )
{
	if( !pTarget || !pCreator )
		return;

	const ioHashString& stCreatorName = pCreator->GetCharName();

	ioHitCountHoldBuff* pControlBuff1 = ToHitCountHoldBuff( pTarget->GetBuff( BT_HIT_COUNT_HOLD, stCreatorName ) );
	if( pControlBuff1 )
	{
		pControlBuff1->SetFloatState( false );
		pControlBuff1->SetReserveEndBuff();
	}

	ioHitCountHoldBuff2* pControlBuff2 = ToHitCountHoldBuff2( pTarget->GetBuff( BT_HIT_COUNT_HOLD2, stCreatorName ) );
	if( pControlBuff2 )
	{
		pControlBuff2->SetFloatState( false );
		pControlBuff2->SetReserveEndBuff();
	}	
}

void ioAutomatonItem::TrackingTarget( ioBaseChar *pOwner, const ioHashString& stUp, const ioHashString& stCenter, const ioHashString& stDown )
{
	ioBall *pBall = pOwner->FindBall();
	ioBaseChar *pTarget = pOwner->GetBaseChar( m_AimedTarget );

	ioEntityGroup *pGrp = pOwner->GetGroup();
	ioAnimation *pUp, *pCenter, *pDown;
		
	pUp = pGrp->GetLoopInPlay( stUp );
	pCenter = pGrp->GetLoopInPlay( stCenter );
	pDown = pGrp->GetLoopInPlay( stDown );

	D3DXVECTOR3 vAimedDir;

	ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pOwner, m_dwWeaponIndex );

	if( pWeapon )
	{
		D3DXVECTOR3 vWeaponPos = pWeapon->GetPosition();
		vAimedDir = vWeaponPos - pOwner->GetMidPositionByRate();
	}
	else
		return;

	D3DXVec3Normalize( &vAimedDir, &vAimedDir );
	float fAniWeight = GetAimedUpDownWeight( vAimedDir, 85.0f );
	if( vAimedDir.y >= 0.0f )	// up, center
	{
		if( !pUp || !pCenter || !pDown )
		{
			return;
		}
		pUp->ForceWeight( fAniWeight );
		pCenter->ForceWeight( FLOAT1 - fAniWeight );
		pDown->ForceWeight( 0.0f );
	}
	else	// center, down
	{
		if( !pUp || !pCenter || !pDown )
		{
			return;
		}
		pUp->ForceWeight( 0.0f );
		pCenter->ForceWeight( FLOAT1 - fAniWeight );
		pDown->ForceWeight( fAniWeight );
	}
}

D3DXVECTOR3 ioAutomatonItem::CalcReturnUroborusPosition( ioWeapon *pWeapon )
{
	ioBaseChar *pChar = pWeapon->GetOwner();
	D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0, 0, -1 );
	D3DXVec3Normalize( &vDir, &vDir );

	D3DXVECTOR3 vReturnPos;
	vReturnPos = pChar->GetWorldPosition() + vDir*m_fGrapReturnDirRate;
	vReturnPos += D3DXVECTOR3(0, m_fGrapReturnPosAddY, 0 );
	m_vUroborusPos = vReturnPos;

	return vReturnPos;
}

D3DXVECTOR3 ioAutomatonItem::GetUroborusRotateOffset( ioWeapon *pWeapon )
{
	ioBaseChar *pChar = pWeapon->GetOwner();
	D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0, 0, -1 );
	D3DXVec3Normalize( &vDir, &vDir );

	D3DXVECTOR3 vReturnPos;
	vReturnPos = vDir*m_fGrapReturnDirRate;
	vReturnPos += D3DXVECTOR3(0, m_fGrapReturnPosAddY, 0 );

	return vReturnPos;
}

bool ioAutomatonItem::CheckLanding( ioBaseChar *pChar )
{
	if( !pChar ) return true;

	DWORD dwCurTime = FRAMEGETTIME();

	float fHeightGap = 0.0f;
	if( pChar->CheckGhostState() )
		fHeightGap = pChar->GetWorldPosition().y - pChar->GetPreHeightForGhostState();
	else
		fHeightGap = pChar->GetWorldPosition().y - pChar->GetBottomHeight();

	float fHeightGapTarget = fHeightGap;
	ioBaseChar *pTarget = NULL;
	if( !m_GrapplingTargetName.IsEmpty() )
	{
		pTarget = pChar->GetBaseChar( m_GrapplingTargetName );
		if( pTarget ) 
		{
			if( pTarget->CheckGhostState() )
				fHeightGapTarget = pTarget->GetWorldPosition().y - pTarget->GetPreHeightForGhostState();
			else
				fHeightGapTarget = pTarget->GetWorldPosition().y - pTarget->GetBottomHeight();
		}
	}

	if( fHeightGap > 0.0f || !pChar->IsDownState() )
	{
		if( pTarget && pTarget->GetState() == CS_GRAPPLING_WOUNDED_ITEM )
		{
			ioHashString stAttacker = pTarget->GetGrapplingAttacker();
			ioBaseChar *pAttacker = pTarget->GetBaseChar( stAttacker );
			if( pAttacker == pChar )
			{
				if(  fHeightGapTarget <= 0.0f && pChar->IsDownState() )
					return true;
			}	
		}

		return false;
	}

	return true;
}

void ioAutomatonItem::StartLandAttack( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	float fAniRate = FLOAT1;
	int iAniID = pGrp->GetAnimationIdx( m_LandAttackAttribute.m_AttackAnimation );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fAniRate );

	m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fAniRate;

	m_AttackState = AAS_LAND_ATTACK;

	DWORD dwCurBaseIndex = pOwner->GetWeaponIndexBase();
	pOwner->RefreshFireTimeListWithBase( dwCurBaseIndex,
		iAniID,
		m_LandAttackAttribute.m_vWeaponInfoList,
		FTT_NORMAL_ATTACK,
		fAniRate,
		0 );
}

bool ioAutomatonItem::IsLandAttackMotionEnd( DWORD dwActionStop ) const
{
	if( m_dwMotionEndTime + dwActionStop > FRAMEGETTIME() )
		return false;

	return true;
}

void ioAutomatonItem::CreateWeapon( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	DWORD dwWeaponIndex = pOwner->IncreaseWeaponIndexBase();
	D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate();

	FireTime kFireTime;
	kFireTime.dwStart = FRAMEGETTIME();
	kFireTime.dwDuration = 1;
	kFireTime.iAttributeIdx = m_dwAttachWeapon;
	kFireTime.dwWeaponIdx = dwWeaponIndex;
	m_dwAttachWeaponIndex = dwWeaponIndex;

	ioWeapon *pWeapon = pOwner->ExtendFireExplicit( kFireTime, vPos, D3DXVECTOR3( 0.0f, 0.0f, 0.f ), "" );
	ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
	if( pZone )
	{
		pZone->SetAttachTargetName( m_Target );
		pZone->SetIgnoreCollsionChar( m_Target );
	}
}

bool ioAutomatonItem::CheckWoundAfterUseExtendState( ioBaseChar* pOwner, BlowTargetType eType )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return false;

	if( pOwner->IsPrisonerMode() )
		return false;

	if( pOwner->IsCatchMode() )
		return false;

	SetWoundAfterExtendState( pOwner, eType );

	return false;
}

void ioAutomatonItem::SetWoundAfterExtendState( ioBaseChar* pOwner, BlowTargetType eType )
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
					ChangeWoundedDashMoveState( pOwner, true );
					return;
				}
			}
			break;
	}
}

bool ioAutomatonItem::IsCanWoundedDashMove( ioBaseChar *pOwner )
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

	if ( !IsEnableGauge( m_fWoundedDashNeedGauge ) )
		return false;

	return true;
}

void ioAutomatonItem::ChangeWoundedDashMoveState( ioBaseChar *pOwner, bool bSendNet )
{
	CheckDecreaseGauge( m_fWoundedDashNeedGauge );

	if( !pOwner->HasBuff( pOwner->GetCharName(), m_szWoundedDashBuffName ) )
		pOwner->AddNewBuff( m_szWoundedDashBuffName, pOwner->GetCharName(), GetName(), NULL );
	
	if( bSendNet )
	{
		ioUserKeyInput::DirKeyInput eDir = pOwner->CheckCurDirKey();;
		float fYawD = 180.0f + 45.0f * ( eDir - ioUserKeyInput::DKI_UP );
		fYawD = ioMath::ArrangeHead( fYawD );

		D3DXQUATERNION qtRotate;
		D3DXQuaternionRotationYawPitchRoll( &qtRotate, DEGtoRAD( fYawD ), 0.0f, 0.0f );
		D3DXVECTOR3 vSightDir = qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVECTOR3 vInputDir( vSightDir.x, 0.0f, vSightDir.z );
		D3DXVec3Normalize( &vInputDir, &vInputDir );

		pOwner->SetInputDir( vInputDir );

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << ASS_WOUNDED_DASH;
			kPacket << pOwner->GetWorldPosition();
			kPacket << pOwner->GetTargetRot();
			kPacket << vInputDir;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioAutomatonItem::CallItemProcess( ioBaseChar* pOwner )
{
	if( !pOwner->IsNeedProcess() )
		return;

	if( pOwner->GetState() != CS_WOUNDED && pOwner->GetState() != CS_BLOW_WOUNDED && pOwner->GetState() != CS_BOUND_BLOW_WOUNDED && pOwner->GetState() != CS_BLOW_EXTEND_WOUND )
	{
		m_WoundedState	= WS_DELAY;
	}
}

void ioAutomatonItem::CheckDecreaseGauge( float fNeedGauge )
{
	m_fCurGauge -= fNeedGauge;
	m_fCurGauge = (float)max( 0, min( m_fCurGauge, m_fMaxGauge ) );
}

int ioAutomatonItem::GetMaxBullet()
{
	return (int)m_fMaxGauge;
}

int ioAutomatonItem::GetNeedBullet()
{
	return (int)m_fNeedGauge;
}

int ioAutomatonItem::GetCurBullet()
{
	float fCurCoolTime = m_fCurGauge;
	fCurCoolTime = max( 0, min( fCurCoolTime, m_fMaxGauge ) );

	return (int)fCurCoolTime;
}

void ioAutomatonItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	float fCurBullet = iCurBullet;
	fCurBullet = max( 0, min( fCurBullet, m_fMaxGauge ) );

	m_fCurGauge = fCurBullet;
}

bool ioAutomatonItem::IsEnableGauge( float fNeedGauge )
{
	if( fNeedGauge == 0.f )
		fNeedGauge = m_fNeedGauge;

	if( m_fCurGauge >= fNeedGauge )
		return true;

	return false;
}

void ioAutomatonItem::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();
	switch( pOwner->GetState() )
	{
	case CS_DIE:
	case CS_DROP_ZONE_DOWN:
	case CS_READY:
	case CS_VIEW:
	case CS_OBSERVER:
	case CS_LOADING:
	case CS_FISHING:
	case CS_STARTMOTION:
	case CS_ENDMOTION:
	case CS_EXCAVATING:
	case CS_CREATEMOTION:
	case CS_ROULETTE:
		return;
	case CS_DELAY:
	case CS_RUN:
	case CS_DASH:
		{
			ioBuff* pBuff = pOwner->GetBuff( m_szWoundedDashBuffName );
			if( pBuff && pBuff->IsLive() )
				return;

			if( m_fCurGauge < m_fMaxGauge )
			{
				float fGauge = m_fIncreaseGauge * fTimePerSec;

				m_fCurGauge += fGauge;
				m_fCurGauge = min( m_fCurGauge, m_fMaxGauge );
			}
		}
		return;
	}
}
//--------------------------------------------------------------------------------------------------------------
ioAutomatonSpecialState::ioAutomatonSpecialState()
{
}

ioAutomatonSpecialState::~ioAutomatonSpecialState()
{
}

void ioAutomatonSpecialState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );
}

void ioAutomatonSpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );

	ioAutomatonItem *pHades = ToAutomatonItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pHades )
		pHades->ClearAttackState( pOwner );
}

void ioAutomatonSpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioAutomatonItem *pHades = ToAutomatonItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pHades )
		pHades->ProcessSpecailState( pOwner );
	else
	{
		pOwner->SetState( CS_DELAY );
		pOwner->SetFallState( true );
		return;
	}
}

void ioAutomatonSpecialState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
	return;
}

void ioAutomatonSpecialState::ProcessKeyInput( ioBaseChar* pOwner )
{
	ioAutomatonItem *pHades = ToAutomatonItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( !pHades )
		return;

	switch ( pHades->GetSpecialState() )
	{
	case ioAutomatonItem::AAS_ATTACK_WALK:
		pOwner->ChangeDirectionByInputDir( false );
		break;
	}
}

bool ioAutomatonSpecialState::IsCanKeyMove( ioBaseChar* pOwner )
{
	ioAutomatonItem *pHades = ToAutomatonItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pHades )
		return pHades->IsEnableMoveState();

	return false;
}

bool ioAutomatonSpecialState::ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime )
{
	ioAutomatonItem *pHades = ToAutomatonItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( !pHades )
		return true;

	switch ( pHades->GetSpecialState() )
	{
	case ioAutomatonItem::AAS_JUMP_START:
	case ioAutomatonItem::AAS_JUMP_ATTACK_HITED:
		return true;
	case ioAutomatonItem::AAS_LAND_ATTACK:
		if( pHades->IsLandAttackMotionEnd( pOwner->GetActionStopTotalDelay() ) )
		{
			pOwner->SetState( CS_DELAY );
			return true;
		}
		break;
	}
	return false;
}