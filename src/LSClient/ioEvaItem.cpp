#include "stdafx.h"
#include "ioEvaItem.h"
#include "ioBaseChar.h"
#include "WeaponDefine.h"
#include "ItemDefine.h"
#include "FindPredicateImpl.h"

ioEvaItem::ioEvaItem()
{
	m_FireState						= FS_NONE;
	m_dwAttackStartTime				= 0;
	m_iCurCombo						= 0;
	m_nNormalExtendAddAttackIndex	= 0;	// 기본 공격과 D~ 공격 섞어서 사용

	// 캔슬 대시
	m_dwMotionEndTime				= 0;
	m_dwCancelDashAttackTime		= 0;
	m_SpecialState					= SS_NONE;

	// D~ 바라보는 방향의 적 조준
	m_bZeroAimState					= false;
	m_fCurAngle						= 0.0f;
	m_bChangeDir					= false;
	m_bAimJumpAttack				= false;

	m_dwFireStateEndTime			= 0;
	m_dwFireStartTime				= 0;
	m_dwFireEndTime					= 0;
	m_dwFireDelayTime				= 0;

	m_dwTrackingEndTime				= 0;
	m_dwAimStartTime				= 0;
	m_dwCheckCoolTime				= 0;
	m_fReloadTimeModifyRate			= FLOAT1;
	m_fFireTimeModifyRate			= FLOAT1;
	m_bAimedState					= false;
	m_dwComboAttackTimeEnd			= 0;		// Charge Combo

	m_dwZeroAimStartTime			= 0;
	m_ZeroAimedEffectID				= -1;
	m_bFirstAimMoveMotion			= true;
	m_bAimMotionSetted				= false;
	m_bAimEffect					= false;
	m_dwCheckAimCancelTime			= 0;

	m_AimStartDirKey				= ioUserKeyInput::DKI_NONE;
	m_CurDirKey						= ioUserKeyInput::DKI_NONE;

	// 조준 상태에서 점프
	m_dwAirJumpTime					= 0;
	m_AimJumpState					= AJS_NONE;

	// 게이지
	m_fCurCoolTime					= 0;

	// 에어리어웨폰 범위 밖에서 A~
	m_SortList.clear();

	//  특수 기능2 : 기본 공격 및 점프 공격 타격 시 서브웨폰 생성
	m_SubWeaponEffectinfoList.clear();
	m_iSubWeaponInfoIndex			= 0;	
	m_iSubWeaponCurAttackIdx		= 0;
	m_dwStartComboTime				= 0;

	// 에어리어웨폰 범위 밖에서 A~ 후 순간이동
	m_dwTeleportEndDelayTime		= 0;
	m_vTargetPos					= D3DXVECTOR3(0.0f,0.0f,0.0f);
	m_vTeleportPos					= D3DXVECTOR3(0.0f,0.0f,0.0f);
}

ioEvaItem::ioEvaItem( const ioEvaItem &rhs )
	: ioWeaponItem( rhs ),
	m_AttackReadyAniList( rhs.m_AttackReadyAniList ),
	m_vExtendAttributeList( rhs.m_vExtendAttributeList ),
	m_fMinRange( rhs.m_fMinRange ),
	m_fAimAngle( rhs.m_fAimAngle ),
	m_fAimRange( rhs.m_fAimRange ),
	m_fMaxAimAngleRate( rhs.m_fMaxAimAngleRate ),
	m_fMaxAimRangeRate( rhs.m_fMaxAimRangeRate ),
	m_fAimRangeInFight( rhs.m_fAimRangeInFight ),
	m_bEnableDownAim( rhs.m_bEnableDownAim ),
	m_NormalCancelDashAttribute( rhs.m_NormalCancelDashAttribute ),	// 캔슬 대시
	m_JumpAimUp( rhs.m_JumpAimUp ),									// D~ 바라보는 방향의 적 조준
	m_JumpAimCenter( rhs.m_JumpAimCenter ),
	m_JumpAimDown( rhs.m_JumpAimDown ),
	m_ZeroAimUp( rhs.m_ZeroAimUp ),
	m_ZeroAimCenter( rhs.m_ZeroAimCenter ),
	m_ZeroAimDown( rhs.m_ZeroAimDown ),
	m_AimUp( rhs.m_AimUp ),
	m_AimCenter( rhs.m_AimCenter ),
	m_AimDown( rhs.m_AimDown ),
	m_dwAimTime( rhs.m_dwAimTime ),
	m_fMarkerRange( rhs.m_fMarkerRange ),
	m_fTargetAimRate( rhs.m_fTargetAimRate ),
	m_fDownTargetAimRate( rhs.m_fDownTargetAimRate ),
	m_vSecondAttributeList( rhs.m_vSecondAttributeList ),
	m_vSecondZeroAttributeList( rhs.m_vSecondZeroAttributeList ),	
	m_bEnableZeroAiming( rhs.m_bEnableZeroAiming ),					// Charge Combo
	m_dwMaxComboDuration( rhs.m_dwMaxComboDuration ),
	m_ZeroAimedEffect( rhs.m_ZeroAimedEffect ),
	m_AimEffect( rhs.m_AimEffect ),
	m_dwZeroAimedCheckTime( rhs.m_dwZeroAimedCheckTime ),
	m_vChargeComboAttListUp( rhs.m_vChargeComboAttListUp ),
	m_vChargeComboAttListCenter( rhs.m_vChargeComboAttListCenter ),
	m_vChargeComboAttListDown( rhs.m_vChargeComboAttListDown ),
	m_fChargeComboAttAngle( rhs.m_fChargeComboAttAngle ),
	m_AimJumpReadyAnimation( rhs.m_AimJumpReadyAnimation ),			// 조준 상태에서 점프
	m_AimJumppingAnimation( rhs.m_AimJumppingAnimation ),
	m_dwMaxCoolTime( rhs.m_dwMaxCoolTime ),
	m_dwNeedCoolTime( rhs.m_dwNeedCoolTime ),
	m_fRecoveryTime( rhs.m_fRecoveryTime ),
	m_bBulletGaugeInit( rhs.m_bBulletGaugeInit ),					// 용병 교체 시 불렛 게이지 초기화 사용 유무
	m_szSpecialCheckBuff( rhs.m_szSpecialCheckBuff ),				// 에어리어웨폰 범위 밖에서 A~
	m_SubWeaponInfoList( rhs.m_SubWeaponInfoList ),					//  특수 기능2 : 기본 공격 및 점프 공격 타격 시 서브웨폰 생성
	m_iMaxSubWeaponCnt(rhs.m_iMaxSubWeaponCnt),
	m_szSubWeaponAttackSoundName(rhs.m_szSubWeaponAttackSoundName),
	m_JumpFireUp(rhs.m_JumpFireUp),
	m_JumpFireCenter(rhs.m_JumpFireCenter),
	m_JumpFireDown(rhs.m_JumpFireDown),
	m_ZeroFireUp(rhs.m_ZeroFireUp),
	m_ZeroFireCenter(rhs.m_ZeroFireCenter),
	m_ZeroFireDown(rhs.m_ZeroFireDown),
	m_FireUp(rhs.m_FireUp),
	m_FireCenter(rhs.m_FireCenter),
	m_FireDown(rhs.m_FireDown),
	m_TeleportMoveStartAction(rhs.m_TeleportMoveStartAction), 		// 에어리어웨폰 범위 밖에서 A~ 후 순간이동
	m_fMoveSpeedRate(rhs.m_fMoveSpeedRate),
	m_TeleportMoveEndAction(rhs.m_TeleportMoveEndAction),
	m_szTeleportStartEffect(rhs.m_szTeleportStartEffect),
	m_szTeleportEndEffect(rhs.m_szTeleportEndEffect),
	m_fTeleportOffset(rhs.m_fTeleportOffset),
	m_fJumpHeightGap(rhs.m_fJumpHeightGap),
	m_TeleportJumpAttribute(rhs.m_TeleportJumpAttribute),
	m_fTeleportJumpPower(rhs.m_fTeleportJumpPower),	
	m_dwTeleportEndDelay(rhs.m_dwTeleportEndDelay)
{
	m_FireState						= FS_NONE;
	m_dwAttackStartTime				= 0;
	m_iCurCombo						= 0;
	m_nNormalExtendAddAttackIndex	= 0;		// 기본 공격과 D~ 공격 섞어서 사용
	m_dwMotionEndTime				= 0;		// 캔슬 대시
	m_dwCancelDashAttackTime		= 0;
	m_SpecialState					= SS_NONE;

	m_bZeroAimState					= false;	// D~ 바라보는 방향의 적 조준
	m_fCurAngle						= 0.0f;
	m_bChangeDir					= false;
	m_bAimJumpAttack				= false;

	m_dwFireStateEndTime			= 0;
	m_dwFireStartTime				= 0;
	m_dwFireEndTime					= 0;
	m_dwFireDelayTime				= 0;

	m_dwTrackingEndTime				= 0;
	m_dwAimStartTime				= 0;
	m_dwCheckCoolTime				= 0;
	m_fReloadTimeModifyRate			= FLOAT1;
	m_fFireTimeModifyRate			= FLOAT1;
	m_bAimedState					= false;

	m_dwComboAttackTimeEnd			= 0;	// Charge Combo	

	m_dwZeroAimStartTime			= 0;
	m_ZeroAimedEffectID				= -1;
	m_bFirstAimMoveMotion			= true;
	m_bAimMotionSetted				= false;
	m_bAimEffect					= false;
	m_dwCheckAimCancelTime			= 0;

	m_AimStartDirKey				= ioUserKeyInput::DKI_NONE;
	m_CurDirKey						= ioUserKeyInput::DKI_NONE;

	// 조준 상태에서 점프
	m_dwAirJumpTime					= 0;
	m_AimJumpState					= AJS_NONE;

	// 게이지
	m_fCurCoolTime					= 0;

	// 에어리어웨폰 범위 밖에서 A~
	m_SortList.clear();

	//  특수 기능2 : 기본 공격 및 점프 공격 타격 시 서브웨폰 생성
	m_SubWeaponEffectinfoList.clear();
	m_iSubWeaponInfoIndex			= 0;	
	m_iSubWeaponCurAttackIdx		= 0;

	m_dwStartComboTime				= 0;

	// 에어리어웨폰 범위 밖에서 A~ 후 순간이동
	m_dwTeleportEndDelayTime		= 0;
	m_vTargetPos					= D3DXVECTOR3(0.0f,0.0f,0.0f);
	m_vTeleportPos					= D3DXVECTOR3(0.0f,0.0f,0.0f);
}

ioEvaItem::~ioEvaItem()
{
}

void ioEvaItem::LoadProperty( ioINILoader &rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];
	char szKey[MAX_PATH];

	// 기본 Charge 준비 단계..
	int iMaxCombo = GetMaxCombo();
	m_AttackReadyAniList.clear();
	for( int i=0; i < iMaxCombo; i++ )
	{
		wsprintf_e( szKey, "attack_ready_animation%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_AttackReadyAniList.push_back( ioHashString(szBuf) );
	}

	// 특별 기본공격 중 캔슬 대시 
	LoadAttackAttribute_e( "normal_cancel_dash", m_NormalCancelDashAttribute, rkLoader );

	// D~ 바라보는 방향의 적 조준
	m_dwAimTime = rkLoader.LoadInt_e( "aim_time", 1000 );
	m_fMinRange = rkLoader.LoadFloat_e( "aim_min_range", 0.0f );
	m_fAimAngle = rkLoader.LoadFloat_e( "aim_angle", 45.0f );
	m_fAimRange = rkLoader.LoadFloat_e( "aim_range", FLOAT1000 );
	m_fMarkerRange = rkLoader.LoadFloat_e( "marker_range", 0.0f );
	m_fAimRangeInFight = rkLoader.LoadFloat_e( "aim_range_in_fight", FLOAT1000 );

	m_fMaxAimRangeRate = rkLoader.LoadFloat_e( "gathering_max_aim_range_rate", FLOAT1 );
	m_fMaxAimAngleRate = rkLoader.LoadFloat_e( "gathering_max_aim_angle_rate", FLOAT1 );

	m_bEnableZeroAiming = rkLoader.LoadBool_e( "enable_zero_aiming", false );
	m_bEnableDownAim = rkLoader.LoadBool_e( "enable_down_aim", false );

	// 조준 상태에서 점프
	rkLoader.LoadString_e( "aim_jump_ready_animation", "", szBuf, MAX_PATH );
	m_AimJumpReadyAnimation = szBuf;
	rkLoader.LoadString_e( "aim_jumpping_animation", "", szBuf, MAX_PATH );
	m_AimJumppingAnimation = szBuf;

	rkLoader.LoadString_e( "jump_aim_up_animation", "", szBuf, MAX_PATH );
	m_JumpAimUp = szBuf;
	rkLoader.LoadString_e( "jump_aim_center_animation", "", szBuf, MAX_PATH );
	m_JumpAimCenter = szBuf;
	rkLoader.LoadString_e( "jump_aim_down_animation", "", szBuf, MAX_PATH );
	m_JumpAimDown = szBuf;

	rkLoader.LoadString_e( "zero_aim_up_animation", "", szBuf, MAX_PATH );
	m_ZeroAimUp = szBuf;
	rkLoader.LoadString_e( "zero_aim_center_animation", "", szBuf, MAX_PATH );
	m_ZeroAimCenter = szBuf;
	rkLoader.LoadString_e( "zero_aim_down_animation", "", szBuf, MAX_PATH );
	m_ZeroAimDown = szBuf;

	rkLoader.LoadString_e( "aim_up_animation", "", szBuf, MAX_PATH );
	m_AimUp = szBuf;
	rkLoader.LoadString_e( "aim_center_animation", "", szBuf, MAX_PATH );
	m_AimCenter = szBuf;
	rkLoader.LoadString_e( "aim_down_animation", "", szBuf, MAX_PATH );
	m_AimDown = szBuf;

	m_fTargetAimRate = rkLoader.LoadFloat_e( "target_aim_rate", FLOAT08 );
	m_fDownTargetAimRate = rkLoader.LoadFloat_e( "down_target_aim_rate", 0.0f );

	rkLoader.LoadString_e( "zero_aimed_effect", "", szBuf, MAX_PATH );
	m_ZeroAimedEffect = szBuf;
	rkLoader.LoadString_e( "aim_effect", "", szBuf, MAX_PATH );
	m_AimEffect = szBuf;
	m_dwZeroAimedCheckTime = rkLoader.LoadInt_e( "zero_aimed_check_time", 1 );

	// Charge Combo Attack
	m_fChargeComboAttAngle = rkLoader.LoadFloat_e( "charge_combo_attack_Angle", 45.0f );
	m_dwMaxComboDuration = (DWORD)rkLoader.LoadInt_e( "charge_combo_max_time", 1 );

	int iComboMax = rkLoader.LoadInt_e( "charge_combo_max_cnt", 1 );
	m_vChargeComboAttListUp.clear();
	m_vChargeComboAttListUp.reserve( iComboMax );
	m_vChargeComboAttListCenter.clear();
	m_vChargeComboAttListCenter.reserve( iComboMax );
	m_vChargeComboAttListDown.clear();
	m_vChargeComboAttListDown.reserve( iComboMax );

	for( int i=0; i < iComboMax; ++i )
	{
		char szBufTmp[MAX_PATH];

		AttackAttribute kAttribute;
		wsprintf_e( szBuf, "charge_combo_attack%d", i+1 );
		LoadAttackAttribute( szBuf, kAttribute, rkLoader );
		m_vChargeComboAttListCenter.push_back( kAttribute );

		wsprintf_e( szBuf, "charge_combo_attack%d_up", i+1 );
		rkLoader.LoadString( szBuf, "", szBufTmp, MAX_PATH );
		kAttribute.m_AttackAnimation = szBufTmp;
		m_vChargeComboAttListUp.push_back( kAttribute );

		wsprintf_e( szBuf, "charge_combo_attack%d_down", i+1 );
		rkLoader.LoadString( szBuf, "", szBufTmp, MAX_PATH );
		kAttribute.m_AttackAnimation = szBufTmp;
		m_vChargeComboAttListDown.push_back( kAttribute );
	}

	// 게이지
	m_dwMaxCoolTime = (DWORD)rkLoader.LoadInt_e( "max_cool_time", 0 );
	m_dwNeedCoolTime = (DWORD)rkLoader.LoadInt_e( "use_cool_time", 0 );
	m_fRecoveryTime = rkLoader.LoadFloat_e( "recover_cool_time", 0.0f );

	// 용병 교체 시 불렛 게이지 초기화 사용 유무
	m_bBulletGaugeInit  = rkLoader.LoadBool_e( "bullet_gauage_init_by_change_char", false );

	// 에어리어웨폰 범위 밖에서 A~
	rkLoader.LoadString_e( "charge_area_name", "", szBuf, MAX_PATH );
	m_szSpecialCheckBuff = szBuf;

	//  특수 기능2 : 기본 공격 및 점프 공격 타격 시 서브웨폰 생성
	m_SubWeaponInfoList.clear();
	m_iMaxSubWeaponCnt = rkLoader.LoadInt_e( "max_sub_weapon_cnt", 0 );
	for ( int i=0; i<m_iMaxSubWeaponCnt; i++ )
	{
		SubWeaponInfo sSubWeaponInfo;
		wsprintf_e( szKey, "sub_weapon%d_effect",i+1);
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		sSubWeaponInfo.m_szWeaponEffect = szBuf;

		wsprintf_e( szKey, "sub_weapon%d_angle", i+1 );
		sSubWeaponInfo.m_fWeaponAngle = rkLoader.LoadFloat( szKey, 0.0f );

		D3DXVECTOR3 vPos;
		wsprintf_e( szKey, "sub_weapon%d_offset_x", i+1 );
		vPos.x = rkLoader.LoadFloat( szKey, 0.0f );
		wsprintf_e( szKey, "sub_weapon%d_offset_y", i+1 );
		vPos.y = rkLoader.LoadFloat( szKey, 0.0f );
		wsprintf_e( szKey, "sub_weapon%d_offset_z", i+1 );
		vPos.z = rkLoader.LoadFloat( szKey, 0.0f );
		sSubWeaponInfo.m_vWeaponPos = vPos;

		wsprintf_e( szKey, "sub_weapon%d_attack_type_count", i+1 );
		int nMaxAttackCnt = rkLoader.LoadInt( szKey, 0 );
		for ( int j=0; j<nMaxAttackCnt; j++ )
		{
			WeaponInfo sWeaponInfo;
			wsprintf_e( szKey, "sub_weapon%d_attack_type%d", i+1, j+1 );
			sWeaponInfo.m_iWeaponIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );
			wsprintf_e( szKey, "sub_weapon%d_attack_type%d_resist", i+1, j+1 );
			sWeaponInfo.m_iResistanceIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );
			wsprintf_e( szKey, "sub_weapon%d_attack_type%d_wounded", i+1, j+1 );
			rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
			sWeaponInfo.m_WoundedAnimation = szBuf;
			wsprintf_e( szKey, "sub_weapon%d_attack_type%d_wounded_time", i+1, j+1 );
			sWeaponInfo.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );
			wsprintf_e( szKey, "sub_weapon%d_attack_type%d_wounded_loop_ani", i+1, j+1 );
			sWeaponInfo.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );

			sSubWeaponInfo.m_WeaponInfoList.push_back( sWeaponInfo );
		}

		m_SubWeaponInfoList.push_back( sSubWeaponInfo );
	}

	rkLoader.LoadString_e( "sub_weapon_attack_sound", "", szBuf, MAX_PATH );
	m_szSubWeaponAttackSoundName = szBuf;

	rkLoader.LoadString_e( "jump_fire_up_animation", "", szBuf, MAX_PATH );
	m_JumpFireUp = szBuf;
	rkLoader.LoadString_e( "jump_fire_center_animation", "", szBuf, MAX_PATH );
	m_JumpFireCenter = szBuf;
	rkLoader.LoadString_e( "jump_fire_down_animation", "", szBuf, MAX_PATH );
	m_JumpFireDown = szBuf;
	rkLoader.LoadString_e( "zero_fire_up_animation", "", szBuf, MAX_PATH );
	m_ZeroFireUp = szBuf;
	rkLoader.LoadString_e( "zero_fire_center_animation", "", szBuf, MAX_PATH );
	m_ZeroFireCenter = szBuf;
	rkLoader.LoadString_e( "zero_fire_down_animation", "", szBuf, MAX_PATH );
	m_ZeroFireDown = szBuf;
	rkLoader.LoadString_e( "fire_up_animation", "", szBuf, MAX_PATH );
	m_FireUp = szBuf;
	rkLoader.LoadString_e( "fire_center_animation", "", szBuf, MAX_PATH );
	m_FireCenter = szBuf;
	rkLoader.LoadString_e( "fire_down_animation", "", szBuf, MAX_PATH );
	m_FireDown = szBuf;
	 
	// 에어리어웨폰 범위 밖에서 순간 이동 A~
	// 텔레포트 대시 시작 애니
	rkLoader.LoadString_e( "teleport_move_animation", "", szBuf, MAX_PATH );
	m_TeleportMoveStartAction = szBuf;
	m_fMoveSpeedRate = rkLoader.LoadFloat_e( "teleport_move_run_max_speed_rate", FLOAT1 );

	// 텔레포트 대시 끝 애니
	LoadAttackAttribute( "teleport_move_end_action", m_TeleportMoveEndAction, rkLoader );

	// 이펙트
	rkLoader.LoadString_e( "teleport_move_start_effect", "", szBuf, MAX_PATH );
	m_szTeleportStartEffect = szBuf;
	rkLoader.LoadString_e( "teleport_move_end_effect", "", szBuf, MAX_PATH );
	m_szTeleportEndEffect = szBuf;

	// 텔레포트 거리
	m_fTeleportOffset = rkLoader.LoadFloat_e( "teleport_move_offset", 0.0f );
	m_fJumpHeightGap = rkLoader.LoadFloat_e( "jump_move_height_gap", 0.0f );

	// 텔레포트 점프 
	LoadAttackAttribute( "teleport_jump_move", m_TeleportJumpAttribute, rkLoader );
	m_fTeleportJumpPower = rkLoader.LoadFloat_e( "teleport_jump_move_power", 0.0f );

	// 텔레포트 이동 후 딜레이
	m_dwTeleportEndDelay = (DWORD)rkLoader.LoadInt( "teleport_move_end_delay", 0 );
}

ioItem* ioEvaItem::Clone()
{
	return new ioEvaItem( *this );
}

ioWeaponItem::WeaponSubType ioEvaItem::GetSubType() const
{
	return WST_EVA_ITEM;
}

void ioEvaItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{
	if( !pOwner ) return;

	pOwner->SetTargetMarkerState( ioTargetMarker::MS_NONE );

	// ClearKeyReserve 초기화
	ClearCancelInfo();

	int iComboCnt = GetMaxCombo();
	if( !COMPARE( iCurCombo, 0, iComboCnt ) )
	{
		ioWeaponItem::SetNormalAttack( pOwner, iCurCombo );		
		m_FireState		= FS_NONE;
		m_iCurCombo		= iCurCombo;
		m_nNormalExtendAddAttackIndex	= 0;
	}
	else
	{
		ioEntityGroup *pGrp = pOwner->GetGroup();
		pGrp->ClearAllLoopAni( FLOAT100, true );
		pGrp->ClearAllActionAni( FLOAT100, true );

		int iAniID = pGrp->GetAnimationIdx( m_AttackReadyAniList[iCurCombo] );
		float fTimeRate = (float)m_dwChargeTime / pGrp->GetAnimationFullTime( iAniID );
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

		m_nNormalExtendAddAttackIndex	= 0;
		m_fCurAngle						= 0.0f;
		m_bChangeDir					= false;
		m_bAimJumpAttack				= false;

		m_FireState						= FS_CHARGING;
		m_dwAttackStartTime				= FRAMEGETTIME();
		m_iCurCombo						= iCurCombo;
		DWORD dwTrackingTime			= FRAMEGETTIME();

		dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;
		pOwner->SetAutoTargetTrackingEndTime( dwTrackingTime );

		pOwner->SetState( CS_ATTACK );
	}

}

void ioEvaItem::CheckNormalAttack( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_FireState )
	{
		case FS_CHARGING:
		{
			OnCharging( pOwner );
			break;
		}

		case FS_NORMAL_ATTACK:
		{
			if( pOwner->IsNeedProcess() )
			{			
				CheckCancelReserveByConditionalSpecialDash( pOwner ); // m_bEnableDashState 체크 

				if( ProcessCancelBySpeicalDash( pOwner ) )
				{
					return;
				}

				CheckCancelReserve( pOwner );

				if( ProcessCancel(pOwner) )
					return;
			}

			ioWeaponItem::CheckNormalAttack( pOwner );
			break;
		}
		case FS_AIMING:
		{
			OnAiming( pOwner );
			break;
		}

		case FS_AIM_WAIT:
		{
			OnAimWait( pOwner );
			break;
		}

		case FS_AIMED:
		{
			OnAimed( pOwner );
			break;
		}

		case FS_NORMAL_FIRE:
		case FS_AIMED_FIRE:
		{
			OnFire( pOwner );
			break;
		}
		case FS_COMBO_FIRE:
		{
			OnCombo( pOwner );
			break;
		}

		case FS_NONE:
		{
			break;
		}
	}

	// 조준 상태에서 방향키
	if( m_FireState == FS_AIMING ||
		m_FireState == FS_AIM_WAIT ||
		m_FireState == FS_AIMED )
	{
		if( pOwner->IsNeedProcess() && pOwner->ChangeDirectionExplicitIfKeyChanged( false ) )
		{
			if( m_FireState != FS_AIMING )
			{
				m_AimedTarget.Clear();
				m_bAimedBall = false;

				pOwner->SetTargetMarkerTarget( m_AimedTarget, m_bAimedBall );
				ChangeToAiming( pOwner );

				// 패킷 완료
				if( pOwner->IsNeedSendNetwork() )
				{
					SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
					FillItemGaugePacketHeader( pOwner, kPacket );
					kPacket << false;	//AimJump
					kPacket << (int)m_FireState;
					kPacket << m_iCurCombo;
					P2PNetwork::SendToAllPlayingUser( kPacket );
				}
			}
		}
	}

	if( pOwner->IsCanAttackMove() && m_FireState != FS_AIMING &&	m_FireState != FS_AIM_WAIT && m_FireState != FS_AIMED )
	{
		pOwner->SetAttackMoveEnable( false );
	}

	if( m_FireState != FS_AIMED && m_FireState != FS_AIMING && m_FireState != FS_AIM_WAIT && m_dwZeroAimStartTime > 0 )
	{
		m_dwZeroAimStartTime = 0;
	}

	if( !m_bZeroAimState )
	{
		if( m_ZeroAimedEffectID != -1 )
		{
			pOwner->EndEffect( m_ZeroAimedEffectID, false );
			m_ZeroAimedEffectID = -1;
		}
	}
	else if( m_bEnableZeroAiming && m_bZeroAimState )
	{
		if( m_ZeroAimedEffectID == -1 )
		{
			ioEffect *pEffect = pOwner->AttachEffect( m_ZeroAimedEffect );
			if( pEffect )
			{
				m_ZeroAimedEffectID = pEffect->GetUniqueID();
			}
		}
	}

	if( pOwner->GetState() == CS_ATTACK )
	{
		UpdateAimDir( pOwner );
	}
}

void ioEvaItem::OnCharging( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )	return;

	if( pOwner->IsAttackKeyDown() )
	{
		if( m_dwAttackStartTime + m_dwChargeTime <= FRAMEGETTIME() )
		{
			ChangeToAiming( pOwner );	
			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
				FillItemGaugePacketHeader( pOwner, kPacket );
				kPacket << false;	//AimJump
				kPacket << (int)m_FireState;
				kPacket << m_iCurCombo;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	}
	else
	{
		pOwner->SetChargingState( false );
		ChangeToNormalAttack( pOwner );
	}
}

void ioEvaItem::ChangeToAiming( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	pOwner->StopAutoTargetTracking();

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_bEnableZeroAiming )
	{
		if( m_bZeroAimState )
		{
			pOwner->SetTargetMarkerState( ioTargetMarker::MS_ZERO_AIMED );
		}
		else
		{
			pOwner->SetTargetMarkerState( ioTargetMarker::MS_READY );
		}
	}
	else
	{
		SetZeroAim( pOwner, false );
		pOwner->SetTargetMarkerState( ioTargetMarker::MS_READY );
	}

	if( m_bEnableZeroAiming )
	{
		if( m_dwZeroAimStartTime == 0 )
			m_dwZeroAimStartTime = dwCurTime;
	}
	else
	{
		m_dwZeroAimStartTime = 0;
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	if( pOwner->IsJumpState() )
		pGrp->SetLoopAni( m_JumpAimCenter, FLOAT100 );
	else
		pGrp->SetLoopAni( m_AimCenter, FLOAT100 );

	m_FireState = FS_AIMING;
	pOwner->SetAttackMoveEnable( false );
	pOwner->ChangeDirectionExplicitIfKeyChanged( true );
	
	m_AimStartDirKey = pOwner->GetCurDirKey();

	m_bFirstAimMoveMotion = true;
	m_bAimMotionSetted = true;

	D3DXQUATERNION qtRot = pOwner->GetTargetRot();
	m_vStartDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

	m_AimedTarget.Clear();
	m_bAimedBall = false;

	m_bSetAimedDir = false;
	m_bAimedState = false;

	if( !m_bAimEffect )
	{
		pOwner->AttachEffect( m_AimEffect );
		m_bAimEffect = true;
	}

	g_TutorialMgr.ActionChangeToAiming();
}

void ioEvaItem::SetZeroAim( ioBaseChar *pOwner, bool bSet )
{
	if( bSet )
	{
		m_bZeroAimState = true;
		pOwner->SetTargetMarkerState( ioTargetMarker::MS_ZERO_AIMED );

		if( m_ZeroAimedEffectID != -1 )
		{
			pOwner->EndEffect( m_ZeroAimedEffectID, false );
			m_ZeroAimedEffectID = -1;
		}

		ioEffect *pEffect = pOwner->AttachEffect( m_ZeroAimedEffect );
		if( pEffect )
			m_ZeroAimedEffectID = pEffect->GetUniqueID();

		ioEntityGroup *pGrp = pOwner->GetGroup();
		pGrp->ClearAllLoopAni( FLOAT100, true );

		pGrp->SetLoopAni( m_ZeroAimUp, 0.0f, 0.0f );
		pGrp->SetLoopAni( m_ZeroAimDown, 0.0f, 0.0f );
		pGrp->SetLoopAni( m_ZeroAimCenter, 0.0f, FLOAT1 );

		m_bFirstAimMoveMotion = true;
	}
	else
	{
		m_bZeroAimState = false;
		m_dwZeroAimStartTime = 0;

		if( m_ZeroAimedEffectID != -1 )
		{
			pOwner->EndEffect( m_ZeroAimedEffectID, false );
			m_ZeroAimedEffectID = -1;
		}
	}
}

void ioEvaItem::OnAiming( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )
	{
		return;
	}

	if( !pOwner->IsAttackKeyDown() && pOwner->GetState() != CS_JUMP )	// 버튼 뗌, 일반 공격으로 전환
	{
		// 게이지 체크
		if( m_fCurCoolTime >= m_dwNeedCoolTime )
		{
			// 있으면 
			ChangeToNormalFire( pOwner );
		}
		else
		{
			// 게이지가 부족하면 딜레이 상태로 전환한다. 
			pOwner->SetState( CS_DELAY );
		}

		return;
	}

	if( !SetAimJumpState( pOwner ) )
	{
		return;
	}

	// 무기스킬 사용 +S
	if( pOwner->GetState() != CS_JUMP && pOwner->IsDefenseKeyDown() )
	{
		if( IsCanFire(0) )
		{
			int iSkillNum = pOwner->GetSkillKeyInput();
			if( iSkillNum == 0 && !pOwner->IsBuffLimitSkill() && pOwner->SetUseSkill(0, SUT_NORMAL))
			{
				m_FireState = FS_NONE;
				return;
			}
		}
	}

	if( m_bEnableZeroAiming )
	{
		bool bZeroAim = CheckZeroAim( pOwner );

		if( bZeroAim && pOwner->IsNeedSendNetwork() )
		{
			// 패킷 수정완료
			SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << (int)m_FireState;
			kPacket << m_iCurCombo;
			kPacket << m_AimedTarget;
			kPacket << m_bAimedBall;
			kPacket << m_bZeroAimState;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}			

		if( bZeroAim )
		{
			g_TutorialMgr.ActionZeroAimed();
		}
		else
		{
			g_TutorialMgr.ActionAimed();
		}
	}

	ioHashString szTarget;
	m_AimedTarget.Clear();
	m_bAimedBall = false;

	float fAimAngle = GetChargeAimAngle();
	float fAimRange = GetChargeAimRange();

	if( pOwner->CheckAimedTargetByBow( fAimAngle, m_fMinRange, fAimRange, szTarget, m_bEnableDownAim ) )
		m_AimedTarget = szTarget;

	// 타겟 비교
	if( CompareAimedTarget( pOwner ) )
	{
		m_AimedTarget.Clear();
		m_bAimedBall = true;
	}

	if( !m_AimedTarget.IsEmpty() || m_bAimedBall )
	{
		ChangeToAimWaitState( pOwner, m_AimedTarget );	// 타겟 세팅됨
	}
}

void ioEvaItem::OnAimWait( ioBaseChar *pOwner )
{
	if( !pOwner->IsNeedProcess() )
		return;

	if( !pOwner->IsAttackKeyDown() && pOwner->GetState() != CS_JUMP )	// 버튼 뗌, 일반 공격으로 전환
	{
		// 게이지 체크
		if( m_fCurCoolTime >= m_dwNeedCoolTime )
		{
			// 있으면 
			ChangeToNormalFire( pOwner );
		}
		else
		{
			// 게이지가 부족하면 딜레이 상태로 전환한다. 
			pOwner->SetState( CS_DELAY );
		}
		return;
	}

	if( !SetAimJumpState( pOwner ) )
	{
		return;
	}

	// 무기스킬 사용 +S
	if( pOwner->GetState() != CS_JUMP && pOwner->IsDefenseKeyDown() )
	{
		if( IsCanFire(0) )
		{
			int iSkillNum = pOwner->GetSkillKeyInput();
			if( iSkillNum == 0 && !pOwner->IsBuffLimitSkill() && pOwner->SetUseSkill(0, SUT_NORMAL))
			{
				m_FireState = FS_NONE;
				return;
			}
		}
	}

	DWORD dwTimeGap = FRAMEGETTIME() - m_dwAimStartTime;
	if( pOwner->GetState() != CS_JUMP && dwTimeGap > m_dwAimTime )	// 조준 완료
	{
		ChangeToAimedState( pOwner );

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << (int)m_FireState; // FS_AIMED
			kPacket << m_iCurCombo;
			kPacket << m_AimedTarget;
			kPacket << m_bAimedBall;
			kPacket << false;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
	else if( pOwner->GetState() == CS_JUMP && dwTimeGap > m_dwAimTime/2 )	// 조준 완료
	{
		ChangeToAimedState( pOwner );

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
			FillItemGaugePacketHeader( pOwner, kPacket );
			kPacket << (int)m_FireState;
			kPacket << m_iCurCombo;
			kPacket << m_AimedTarget;
			kPacket << m_bAimedBall;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
	else
	{
		if( !CheckCurrentTargetValid( pOwner, true ) )
		{
			m_dwTrackingEndTime = FRAMEGETTIME();

			// 대상을 제거한후 조준대기로 넘겨야함
			m_AimedTarget.Clear();
			m_bAimedBall = false;

			pOwner->SetTargetMarkerTarget( m_AimedTarget, m_bAimedBall );
			ChangeToAiming( pOwner );
		}
	}
}

void ioEvaItem::ChangeToAimedState( ioBaseChar *pOwner )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();

	if( m_bZeroAimState )
	{
		if( !pGrp->GetLoopInPlay( m_ZeroAimUp ) )
		{
			pGrp->SetLoopAni( m_ZeroAimUp, 0.0f, 0.0f );
		}

		if( !pGrp->GetLoopInPlay( m_ZeroAimDown ) )
		{
			pGrp->SetLoopAni( m_ZeroAimDown, 0.0f, 0.0f );
		}

		if( !pGrp->GetLoopInPlay( m_ZeroAimCenter ) )
		{
			pGrp->SetLoopAni( m_ZeroAimCenter, 0.0f, FLOAT1 );
		}
	}
	else if( pOwner->GetState() != CS_JUMP )
	{
		pGrp->SetLoopAni( m_AimUp, 0.0f, 0.0f );
		pGrp->SetLoopAni( m_AimDown, 0.0f, 0.0f );

		if( !pGrp->GetLoopInPlay( m_AimCenter ) )
		{
			pGrp->SetLoopAni( m_AimCenter, 0.0f, FLOAT1 );
		}
	}
	else
	{
		pGrp->SetLoopAni( m_JumpAimUp, 0.0f, 0.0f );
		pGrp->SetLoopAni( m_JumpAimDown, 0.0f, 0.0f );

		if( !pGrp->GetLoopInPlay( m_JumpAimCenter ) )
		{
			pGrp->SetLoopAni( m_JumpAimCenter, 0.0f, FLOAT1 );
		}
	}

	m_FireState = FS_AIMED;

	if( m_dwZeroAimStartTime == 0 && !m_bZeroAimState )
	{
		m_dwZeroAimStartTime = FRAMEGETTIME();
	}

	pOwner->SetTargetMarkerTarget( m_AimedTarget, m_bAimedBall );

	if( m_bZeroAimState )
	{
		pOwner->SetTargetMarkerState( ioTargetMarker::MS_ZERO_AIMED );
	}
	else
	{
		pOwner->SetTargetMarkerState( ioTargetMarker::MS_AIMED );
	}

	if( CheckCurrentTargetValid( pOwner, false ) )
	{
		TrackingTarget( pOwner );
	}
}

void ioEvaItem::OnAimed( ioBaseChar *pOwner )
{
	if( pOwner->IsNeedProcess() )
	{
		if( pOwner->GetState() != CS_JUMP && !m_bZeroAimState )
		{
			bool bZeroAim = CheckZeroAim( pOwner );
			if( bZeroAim && pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
				FillItemGaugePacketHeader( pOwner, kPacket );
				kPacket << (int)m_FireState;
				kPacket << m_iCurCombo;
				kPacket << m_AimedTarget;
				kPacket << m_bAimedBall;
				kPacket << m_bZeroAimState;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}

			if( bZeroAim )
			{
				g_TutorialMgr.ActionZeroAimed();
			}
			else
			{
				g_TutorialMgr.ActionAimed();
			}
		}

		if( !pOwner->IsAttackKeyDown() && pOwner->GetState() != CS_JUMP )
		{
			// 방향전환 변경시 
			bool bNormalFire = false;
			if( m_bChargeCharRotate && !CheckCurrentTargetValid( pOwner, true ) )
			{
				bNormalFire = true;
			}

			if( bNormalFire )
			{
				m_bAimedBall = false;
				m_bAimedState = false;
				if( m_fCurCoolTime >= m_dwNeedCoolTime )
				{
					// 있으면 
					ChangeToNormalFire( pOwner );
				}
				else
				{
					// 게이지가 부족하면 딜레이 상태로 전환한다. 
					pOwner->SetState( CS_DELAY );
				}
				return;
			}
			else
			{
				// 게이지 체크
				if( m_fCurCoolTime >= m_dwNeedCoolTime )
				{
					// 있으면 
					ChangeToAimedFire( pOwner );
					if( pOwner->IsNeedSendNetwork() )
					{
						SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
						FillItemGaugePacketHeader( pOwner, kPacket );
						kPacket << (int)m_FireState;
						kPacket << m_iCurCombo;
						kPacket << m_AimedTarget;
						kPacket << m_bAimedBall;
						kPacket << m_vAimedDir;
						kPacket << m_bSetAimedDir;
						kPacket << m_bSetSkillAimed;
						kPacket << pOwner->GetTargetRot();
						P2PNetwork::SendToAllPlayingUser( kPacket );
					}
					g_TutorialMgr.ActionAimedFireDeveloperK(m_AimedTarget);
				}
				else
				{
					// 게이지가 부족하면 딜레이 상태로 전환한다. 
					pOwner->SetState( CS_DELAY );
				}
				
			}
		}
		else	// AIMED FIRE..
		{
			if( !SetAimJumpState( pOwner ) )
				return;

			if( CheckCurrentTargetValid( pOwner, true ) )
			{
				TrackingTarget( pOwner );
			
				// 무기스킬 사용 +S
				if( pOwner->GetState() != CS_JUMP && pOwner->IsDefenseKeyDown() )
				{
					if( IsCanFire(0) )
					{
						int iSkillNum = pOwner->GetSkillKeyInput();
						if(iSkillNum == 0 && !pOwner->IsBuffLimitSkill() && pOwner->SetUseSkill(0, SUT_NORMAL))
						{
							m_bSetSkillAimed = true;
							m_FireState = FS_NONE;
							pOwner->SetChargingState( false );
							if( pOwner->IsNeedSendNetwork() )
							{
								SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
								FillItemGaugePacketHeader( pOwner, kPacket );
								kPacket << (int)m_FireState;
								kPacket << m_iCurCombo;
								kPacket << m_AimedTarget;
								kPacket << m_bAimedBall;
								kPacket << m_vAimedDir;
								kPacket << m_bSetAimedDir;
								kPacket << m_bSetSkillAimed;
								P2PNetwork::SendToAllPlayingUser( kPacket );
							}							
							return;
						}
					}
				}
			}
			else
			{
				m_dwTrackingEndTime = FRAMEGETTIME();

				// 대상을 제거한후 조준대기로 넘겨야함
				m_AimedTarget.Clear();
				m_bAimedBall = false;

				pOwner->SetTargetMarkerTarget( m_AimedTarget, m_bAimedBall );
				ChangeToAiming( pOwner );

				if( pOwner->IsNeedSendNetwork() )
				{
					SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
					FillItemGaugePacketHeader( pOwner, kPacket );
					kPacket << false;	//AimJump
					kPacket << (int)m_FireState;
					kPacket << m_iCurCombo;
					P2PNetwork::SendToAllPlayingUser( kPacket );
				}
			}
		}
	}
	else
	{
		if( CheckCurrentTargetValid( pOwner, false ) )
		{
			TrackingTarget( pOwner );
		}
	}
}

void ioEvaItem::ChangeToAimedFire( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	// 기본 공격과 D~ 공격 섞어서 사용
	int iExtendAttackCnt = m_vChargeComboAttListCenter.size();
	if( iExtendAttackCnt <= m_nNormalExtendAddAttackIndex )	{	return;	}

	m_dwAirJumpTime = 0;
	
	if( pOwner->GetState() == CS_JUMP )	{	return;		}
	CheckChargeRate();
	
	const AttackAttribute *pAttr = GetCorrectDirAttibute( pOwner );
	if( !pAttr )	{	return;	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	int	  iAniID	  = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "ioEvaItem::ChangeToAimedFire - %s Animation is Not Exist", pAttr->m_AttackAnimation.c_str() );
		return;
	}

	float fTimeRate   = pAttr->m_fAttackAniRate;
	DWORD dwPreDelay  = pAttr->m_dwPreDelay;
	m_dwFireDelayTime = pAttr->m_dwEndDelay;

	pOwner->RefreshFireTimeList( iAniID,
							     pAttr->m_vWeaponInfoList,
							     FTT_NORMAL_ATTACK,
							     fTimeRate,
							     dwPreDelay );

	pOwner->IncreaseWeaponIndexBase();

	m_dwAirJumpTime = pGrp->GetAnimationEventTime_e( iAniID, "air_jump" ) * fTimeRate;
	if( m_dwAirJumpTime > 0 )
		m_dwAirJumpTime += FRAMEGETTIME();

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	m_dwFireStartTime = FRAMEGETTIME();
	m_dwFireEndTime = pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + dwPreDelay;

	m_dwFireStateEndTime = pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;
	m_dwFireStateEndTime += FRAMEGETTIME();

	//yunga
	m_dwComboAttackTimeEnd = dwCurTime + pGrp->GetAnimationFullTime(iAniID) * fTimeRate + dwPreDelay;
	m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate + dwPreDelay;
	if( m_dwKeyReserveTime > 0 )
	{
		m_dwKeyReserveTime += dwCurTime;
	}


	ioBaseChar *pTarget = pOwner->GetBaseChar( m_AimedTarget );
	if( pTarget )
	{
		pOwner->SetTargetRotToTargetPos( pTarget, true );
	}

	float fWeight = GetAimedUpDownWeight( m_vAimedDir, 85.0f );
	{
		if( m_vAimedDir.y >= 0.0f )
		{
			AimedFireToUp( pGrp, fWeight, fTimeRate, dwPreDelay, true, false );
		}
		else
		{
			AimedFireToDown( pGrp, fWeight, fTimeRate, dwPreDelay, true, false );
		}
	}

	pOwner->SetReservedSliding( pAttr->m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	if( m_AimedTarget.IsEmpty() )
	{
		pOwner->SetTargetMarkerState( ioTargetMarker::MS_NONE );
	}

	m_FireState = FS_AIMED_FIRE;

	// 기본 공격과 D~ 공격 섞어서 사용
	if( iExtendAttackCnt <= m_nNormalExtendAddAttackIndex + 1 )
	{
		pOwner->ApplyNormalAttackResult( m_nNormalExtendAddAttackIndex, true );
	}
	else
	{
		pOwner->ApplyNormalAttackResult( m_nNormalExtendAddAttackIndex );
	}

	pOwner->SetChargingState( false );

	DWORD dwComboTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" )*fTimeRate;
	pOwner->SetComboCheckTime( dwComboTime, true );

	// 기본공격과 특별 공격을 섞어서 사용 가능
	m_nNormalExtendAddAttackIndex++;

	//  특수 기능2 : 서브 웨폰 발사
	SetSubWeaponAttackState( pOwner );
}

bool ioEvaItem::CheckZeroAim( ioBaseChar *pOwner )
{
	if( m_bZeroAimState ) return false;

	if( m_dwZeroAimStartTime == 0 ) return false;

	if( m_dwZeroAimStartTime+m_dwZeroAimedCheckTime > FRAMEGETTIME() )
	{
		return false;
	}

	SetZeroAim( pOwner, true );
	return true;
}

void ioEvaItem::ChangeToAimWaitState( ioBaseChar *pOwner, const ioHashString &szTarget )
{
	m_dwAimStartTime = FRAMEGETTIME();
	m_FireState = FS_AIM_WAIT;

	pOwner->SetTargetMarkerTarget( m_AimedTarget, m_bAimedBall );
	pOwner->SetTargetMarkerState( ioTargetMarker::MS_AIMING );
	
	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << false;	//AimJump
		kPacket << (int)m_FireState;
		kPacket << m_iCurCombo;
		kPacket << m_nNormalExtendAddAttackIndex;
		kPacket << m_AimedTarget;
		kPacket << m_bAimedBall;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioEvaItem::CheckChargeRate()
{
	m_fCurChargeRate = 0.0f;

	if( m_bEnablePowerCharge && m_dwZeroAimStartTime > 0 )
	{
		DWORD dwCurTime = FRAMEGETTIME();
		DWORD dwGapTime = dwCurTime - m_dwZeroAimStartTime;

		if( m_dwZeroAimedCheckTime > 0 )
		{
			if( m_dwZeroAimedCheckTime > dwGapTime )
			{
				m_fCurChargeRate = (float)dwGapTime / m_dwZeroAimedCheckTime;
			}
			else
			{
				m_fCurChargeRate = FLOAT1;
			}
		}
	}
}

AttackAttribute* ioEvaItem::GetCorrectDirAttibute( ioBaseChar *pOwner )
{
	if( !COMPARE( m_nNormalExtendAddAttackIndex, 0, (int)m_vChargeComboAttListCenter.size() ) )
	{
		return NULL;
	}

	if( m_AimedTarget.IsEmpty() )	
	{
		bool bUp = false; //타겟이 나보다 위에 있다.

		D3DXVECTOR3 vPosSrc = pOwner->GetWorldPosition();
		ioBaseChar *pTarget = pOwner->GetBaseChar( m_AimedTarget );
		if( pTarget == NULL)
		{
			return &m_vChargeComboAttListCenter[m_nNormalExtendAddAttackIndex];
		}

		D3DXVECTOR3 vPosDest = pTarget->GetWorldPosition();

		if( (vPosDest.y - vPosSrc.y) > 0 )
		{
			bUp = true;
		}

		D3DXVECTOR3 v1 = vPosDest - vPosSrc;
		D3DXVECTOR3 v2 = pOwner->GetTargetRot() * D3DXVECTOR3( 0, 0, -1 );

		D3DXVec3Normalize( &v1, &v1);
		D3DXVec3Normalize( &v2, &v2);

		float fDotValue = (float)D3DXVec3Dot( &v1, &v2 );
		float fAngle = RADtoDEG( acosf( fDotValue ) );

		if( fAngle > m_fChargeComboAttAngle )
		{
			if( bUp )
			{
				return &m_vChargeComboAttListUp[m_nNormalExtendAddAttackIndex];
			}
			else
			{
				return &m_vChargeComboAttListDown[m_nNormalExtendAddAttackIndex];
			}
		}

	}
	
	return &m_vChargeComboAttListCenter[m_nNormalExtendAddAttackIndex];
}


void ioEvaItem::ChangeToNormalFire( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	// 기본 공격과 D~ 공격 섞어서 사용
	int iExtendAttackCnt = m_vChargeComboAttListCenter.size();
	if( iExtendAttackCnt <= m_nNormalExtendAddAttackIndex )	{	return;	}

	if( pOwner->GetState() == CS_JUMP )	{	return;	}

	CheckChargeRate();

	m_dwAirJumpTime = 0;

	m_AimedTarget.Clear();
	
	const AttackAttribute *pAttr = GetCorrectDirAttibute( pOwner );
	ioEntityGroup *pGrp = pOwner->GetGroup();
	int	  iAniID	  = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	float fTimeRate   = pAttr->m_fAttackAniRate * m_fFireTimeModifyRate;
	DWORD dwPreDelay  = pAttr->m_dwPreDelay;
	m_dwFireDelayTime = pAttr->m_dwEndDelay;

	pOwner->RefreshFireTimeList( iAniID,
							     pAttr->m_vWeaponInfoList,
							     FTT_NORMAL_ATTACK,
							     fTimeRate,
							     dwPreDelay );

	pOwner->IncreaseWeaponIndexBase();

	m_dwAirJumpTime = pGrp->GetAnimationEventTime_e( iAniID, "air_jump" ) * fTimeRate;
	if( m_dwAirJumpTime > 0 )
	{
		m_dwAirJumpTime += FRAMEGETTIME();
	}

	m_dwFireStateEndTime = pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;
	m_dwFireStateEndTime += FRAMEGETTIME();

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	m_dwFireStartTime = FRAMEGETTIME();
	m_dwFireEndTime = pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + dwPreDelay;
	if( pAttr->m_AttackEndAnimation.IsEmpty() )
	{
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
	}
	else
	{
		pGrp->SetActionAni( iAniID, FLOAT100, 0.0f, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

		float fEndAniRate = pAttr->m_fAttackEndAniRate;
		pGrp->SetActionAni( pAttr->m_AttackEndAnimation,
							0.0f, FLOAT100,
							1.0f, FLOAT1/fEndAniRate,
							m_dwFireEndTime, 0.0f, true );

		int iEndAniID = pGrp->GetAnimationIdx( pAttr->m_AttackEndAnimation );
		m_dwFireDelayTime += pGrp->GetAnimationFullTime( iEndAniID ) * fEndAniRate;
	}

	//yunga
	m_dwComboAttackTimeEnd = dwCurTime + pGrp->GetAnimationFullTime(iAniID) * fTimeRate + dwPreDelay;
	m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate + dwPreDelay;
	if( m_dwKeyReserveTime > 0 )
	{
		m_dwKeyReserveTime += dwCurTime;
	}
	//////////////////////////////////////////////////////////////////////////


	pOwner->SetReservedSliding( pAttr->m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	pOwner->SetTargetMarkerState( ioTargetMarker::MS_NONE );
	m_FireState = FS_NORMAL_FIRE;

	// 기본 공격과 D~ 공격 섞어서 사용
	if( iExtendAttackCnt <= m_iCurCombo + 1 )
	{
		pOwner->ApplyNormalAttackResult( m_iCurCombo, true );
	}
	else
	{
		pOwner->ApplyNormalAttackResult( m_iCurCombo );
	}	

	pOwner->SetChargingState( false );

	DWORD dwComboTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" )*fTimeRate;
	pOwner->SetComboCheckTime( dwComboTime, true );
	
	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << false;
		kPacket << (int)m_FireState;	// FS_NORMAL_FIRE
		kPacket << m_iCurCombo;
		kPacket << pOwner->GetTargetRot();
		kPacket << m_nNormalExtendAddAttackIndex;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	// 기본공격과 특별 공격을 섞어서 사용 가능
	m_nNormalExtendAddAttackIndex++;

	g_TutorialMgr.ActionNormalFire();
}

bool ioEvaItem::CompareAimedTarget( ioBaseChar *pOwner )
{
	if( !pOwner ) return false;

	ioBall *pBall = pOwner->FindBall();
	if( !pBall ) 
		return false;

	float fAimRange = GetChargeAimRange();
	float fAimAngle = GetChargeAimAngle();

	ioBaseChar *pChar = pOwner->GetBaseChar( m_AimedTarget );

	D3DXVECTOR3 vDiff;
	D3DXVECTOR3 vOwnerPos = pOwner->GetWorldPosition();
	D3DXVECTOR3 vCharDir  = pOwner->GetMoveDir();

	float fHalfCosine = cosf( DEGtoRAD( fAimAngle ) * FLOAT05 );

	float fLimitDistSq = fAimRange * fAimRange;
	float fMinDistSq = m_fMinRange * m_fMinRange;

	float fBallDist = std::numeric_limits<float>::infinity();
	float fCharDist = std::numeric_limits<float>::infinity();

	// Ball
	if( pBall )
	{
		vDiff = pBall->GetWorldPosition() - vOwnerPos;
		float fCurDistSq = D3DXVec3LengthSq( &vDiff );

		if( (fLimitDistSq >= fCurDistSq) && (fMinDistSq <= fCurDistSq) )
		{
			vDiff.y = 0.0f;
			D3DXVec3Normalize( &vDiff, &vDiff );

			if( D3DXVec3Dot( &vCharDir, &vDiff ) >= fHalfCosine )
				return true;
		}
	}

	return false;
}

bool ioEvaItem::SetAimJumpState( ioBaseChar *pChar )
{
	if( pChar->GetState() == CS_JUMP )
		return true;	

	// 점프 타입 비교 해서 아니라면 기본 점프(ioEvaComboJump)에서 처리를 한다.
	if( GetJumpType() != JT_AIM_JUMP && pChar->IsJumpKeyPress() && pChar->IsCanJumpState() && !pChar->IsBuffLimitJump() )
	{
		pChar->SetTargetMarkerState( ioTargetMarker::MS_NONE );
		pChar->SetJumpState();
		m_FireState = FS_NONE;
		g_TutorialMgr.ActionAimingCancel();
		return false;
	}

	// 조준상태에서 점프 시
	if( pChar->IsJumpKeyPress() )
	{
		if( m_dwCheckAimCancelTime == 0 )
		{
			if( m_pExtendJump && GetJumpType() == JT_AIM_JUMP )
			{
				// AimJump Ready
				SetAimJumpReady( pChar );
			}
		}
	}

	// 조준상태에서 점프 후 m_dwCheckAimCancelTime 값을 체크
	if( m_dwCheckAimCancelTime > 0 )
	{		
		if( pChar->IsJumpKeyDown() && !pChar->IsHasCrown() )
		{
			if( m_dwCheckAimCancelTime < FRAMEGETTIME() )
			{
				if( m_pExtendJump && GetJumpType() == JT_AIM_JUMP )
				{
					// 조준해제
					SetAimJumpping( pChar, true );
					return false;
				}
			}
		}
		else
		{
			// AimJump Set
			SetAimJumpping( pChar );
			return false;
		}
	}

	return true;
}

void ioEvaItem::SetAimJumpReady( ioBaseChar *pChar )
{
	m_dwCheckAimCancelTime = FRAMEGETTIME();
	m_dwCheckAimCancelTime += GetJumpChargeTime( pChar );

	ioEntityGroup *pGrp = pChar->GetGroup();
	pGrp->ClearAllActionAni( FLOAT100, true );

	if( m_AimJumpReadyAnimation.IsEmpty() )
		pGrp->SetActionAni( pChar->GetJumpReadyAnimationIdx(), FLOAT100, FLOAT100, FLOAT1, FLOAT1, 0.0f, 0.0f, true );
	else
		pGrp->SetActionAni( m_AimJumpReadyAnimation, FLOAT100, FLOAT100, FLOAT1, FLOAT1, 0.0f, 0.0f, true );

	m_AimJumpState = AJS_READY;

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pChar, kPacket );
		kPacket << true;		// AimJumpState
		kPacket << (int)m_AimJumpState;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioEvaItem::SetAimJumpping( ioBaseChar *pChar, bool bFullTime )
{
	m_dwCheckAimCancelTime = 0;
	ioEntityGroup *pGrp = pChar->GetGroup();
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->SetLoopAni( m_JumpAimCenter, FLOAT100, FLOAT1 );
	pGrp->SetLoopAni( m_JumpAimUp, 0.0f, 0.0f );
	pGrp->SetLoopAni( m_JumpAimDown, 0.0f, 0.0f );

	if( m_bZeroAimState )
	{
		if( !m_bEnableZeroAiming )
		{
			switch( m_FireState )
			{
			case FS_AIMING:
			case FS_AIM_WAIT:
				pChar->SetTargetMarkerState( ioTargetMarker::MS_READY );
				break;
			case FS_AIMED:
				pChar->SetTargetMarkerState( ioTargetMarker::MS_AIMED );
				break;
			}

			SetZeroAim( pChar, false );
		}
	}
	pChar->SetAimJumpState( bFullTime );

	m_AimJumpState = AJS_JUMPPING;

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pChar, kPacket );
		kPacket << true;		// AimJumpState
		kPacket << (int)m_AimJumpState;
		kPacket << bFullTime;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioEvaItem::SetAimJumpCancel( ioBaseChar *pChar )
{
	m_dwCheckAimCancelTime = 0;
	m_FireState = FS_NONE;

	pChar->SetTargetMarkerState( ioTargetMarker::MS_NONE );
	pChar->SetState( CS_DELAY );

	if( m_ZeroAimedEffectID != -1 )
	{
		pChar->EndEffect( m_ZeroAimedEffectID, false );
		m_ZeroAimedEffectID = -1;
	}

	m_bSetAimedDir = false;
	m_bAimedState = false;
	m_AimJumpState = AJS_CANCEL;

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pChar, kPacket );
		kPacket << true;		// AimJumpState
		kPacket << (int)m_AimJumpState;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

bool ioEvaItem::CheckAimState(bool bAimMove)
{
	switch( m_FireState )
	{
	case FS_AIMING:
	case FS_AIM_WAIT:
	case FS_AIMED:
		return true;
	}

	return false;
}

bool ioEvaItem::SetAimState( ioBaseChar *pChar )
{
	if( pChar->GetState() == CS_JUMP && CheckAimState() )
	{
		m_dwCheckAimCancelTime = 0;
		pChar->GetGroup()->ClearAllActionAni( FLOAT100, true );
		pChar->SetState( CS_ATTACK );

		m_bFirstAimMoveMotion = true;
		m_bAimMotionSetted = true;

		if( m_FireState == FS_AIMED || m_FireState == FS_AIMING )
		{
			m_dwZeroAimStartTime = FRAMEGETTIME();
		}
	}
	else
	{
		ChangeToAiming( pChar );

		if( !pChar->IsJumpState() )
		{
			pChar->SetState( CS_ATTACK );
		}
	}

	pChar->SetChargingState( true );
	m_bAimJumpAttack = false;

	return true;
}

void ioEvaItem::SetNoneState( ioBaseChar* pOwner, int eNewState /* = 0 */ )
{
	m_bAimedBall			= false;
	m_bSetSkillAimed		= false;
	m_bSetAimedDir			= false;
	m_bAimedState			= false;
	m_bZeroAimState			= false;
	m_dwZeroAimStartTime	= 0;
	m_FireState				= FS_NONE;
}

bool ioEvaItem::CheckLanding( ioBaseChar *pChar )
{
	// 조준상태에서 점프 했을 경우에 체크 true 이면 계속 유지 한다.
	if( !CheckAimState() )
	{
		pChar->SetTargetMarkerState( ioTargetMarker::MS_NONE );
		pChar->GetGroup()->ReleaseAllExtraRotate();
		SetNoneState( pChar );
		return true;
	}

	ioEntityGroup *pGrp = pChar->GetGroup();
	pGrp->ClearAllLoopAni( 0.0f, true );

	pGrp->SetLoopAni( m_AimCenter, 0.0f );
	pGrp->SetLoopAni( m_AimUp, 0.0f, 0.0f );
	pGrp->SetLoopAni( m_AimDown, 0.0f, 0.0f );
	pChar->GetGroup()->ReleaseAllExtraRotate();
	return false;
}

void ioEvaItem::CheckAimJumpState( ioBaseChar *pChar )
{
	switch( m_FireState )
	{
		case FS_AIMING:
		{
			OnAiming( pChar );
			break;
		}

		case FS_AIM_WAIT:
		{
			OnAimWait( pChar );
			break;
		}

		case FS_AIMED:
		{
			OnAimed( pChar );
			break;
		}

		case FS_NORMAL_FIRE:
		case FS_AIMED_FIRE:
		{
			OnJumpFire( pChar );
			break;
		}
	}

	if( m_FireState != FS_AIMED && m_FireState != FS_AIMING &&
		m_FireState != FS_AIM_WAIT && m_dwZeroAimStartTime > 0 )
	{
		m_dwZeroAimStartTime = 0;
	}

	if( m_bZeroAimState )
	{
		pChar->EndEffect( m_ZeroAimedEffect );
	}

	UpdateAimDir( pChar );
}

void ioEvaItem::ChangeToComboState( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	m_dwStartComboTime = dwCurTime;

	m_FireState = FS_COMBO_FIRE;
}

void ioEvaItem::OnFire( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( m_bAimEffect && m_dwFireStateEndTime < dwCurTime )
	{
		m_bAimEffect = false;
		pOwner->EndEffect( m_AimEffect, false );		
	}

	if( m_dwKeyReserveTime && m_dwKeyReserveTime < dwCurTime )
	{
		ChangeToComboState( pOwner );
	}
}

void ioEvaItem::OnJumpFire( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwAttackOverTime = m_dwFireStartTime + m_dwFireEndTime + m_dwFireDelayTime;

	if( dwCurTime >= dwAttackOverTime )
	{
		m_FireState = FS_NONE;
		pOwner->SetMoveDirByRotate( pOwner->GetTargetRot() );
	}
}

void ioEvaItem::OnCombo( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	if( !CheckCurrentTargetValid(pOwner, true ) )
	{
		pOwner->SetTargetMarkerState( ioTargetMarker::MS_NONE );
		m_AimedTarget.Clear();
	}

	if( !pOwner->IsNeedProcess() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	// D~ 콤보 이후에 아무 반응이 없거나, 더 이상 콤보가 max 일 경우에
	if( pOwner->CheckDefaultNormalAttack( m_dwFireStartTime, m_dwFireEndTime, m_dwFireDelayTime ) )
	{
		m_FireState = FS_NONE;
		return;
	}

	if( m_dwKeyReserveTime && m_dwKeyReserveTime < dwCurTime )
	{
		CheckReserve( pOwner );

		// 공격 및 게이지 체크
		if( m_nNormalExtendAddAttackIndex >= (int)m_vChargeComboAttListCenter.size() ||  m_fCurCoolTime < m_dwNeedCoolTime )
		{			
			pOwner->SetChargingState( false );
			pOwner->SetTargetMarkerState( ioTargetMarker::MS_NONE );
			return;
		}

		//  한발 쏘고 차징 스타트 콤보 시작 타임(m_dwStartComboTime) + 다음 콤보시간(m_dwMaxComboDuration)
		if( (m_dwStartComboTime + m_dwMaxComboDuration) > dwCurTime )
		{
			if( pOwner->IsAttackKey() )
			{				
				// 기본 공격과 D~ 공격 섞어서 사용
				pOwner->InitAttackReserve();
				pOwner->ClearReserveKeyInput();

				// 다음 콤보 		
				ChangeToNextComboState( pOwner );
			}
		}
		else
		{
			pOwner->SetState( CS_DELAY );
		}
	}
}

bool ioEvaItem::CheckDashInput( ioBaseChar *pOwner )
{
	if( !pOwner )						{	return false;	}
	if( !pOwner->IsNeedProcess() )		{	return false;	}
	if( pOwner->IsDirKeyDoubleClick() )	{	return true;	}

	return false;
}

void ioEvaItem::ChangeToNextComboState( ioBaseChar *pOwner )
{
	DWORD dwCurTime = FRAMEGETTIME();

	int iExtendAttackCnt = m_vChargeComboAttListCenter.size();

	m_dwAirJumpTime = 0;
	
	if( pOwner->GetState() == CS_JUMP )	{	return;	}
	
	if( pOwner->IsNeedProcess() )
	{
		ioUserKeyInput::DirKeyInput eDir = pOwner->CheckCurDirKey();
		if(	eDir != ioUserKeyInput::DKI_NONE && pOwner->IsNeedProcess() )
		{
			float fYawD = 180.0f + 45.0f * ( eDir - ioUserKeyInput::DKI_UP );
			fYawD = ioMath::ArrangeHead( fYawD );

			float fYawR = DEGtoRAD( fYawD );
			D3DXQUATERNION qtRotate;
			D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

			pOwner->SetTargetRotToRotate( qtRotate, true );
		}
	}

	m_dwAirJumpTime = 0;
	m_AimedTarget.Clear();

	float fAimAngle = GetChargeAimAngle();
	float fAimRange = GetChargeAimRange();
	ioHashString szTarget;

	if( pOwner->CheckAimedTargetByBow( fAimAngle, m_fMinRange, fAimRange, szTarget, m_bEnableDownAim ) )
		m_AimedTarget = szTarget;

	// 타겟 비교
	if( CompareAimedTarget( pOwner ) )
	{
		m_AimedTarget.Clear();
		m_bAimedBall = true;
	}

	if( !m_AimedTarget.IsEmpty() )
	{
		pOwner->SetTargetMarkerTarget( m_AimedTarget, m_bAimedBall );
		pOwner->SetTargetMarkerState( ioTargetMarker::MS_AIMED );
	}

	TrackingTarget( pOwner );

	CheckChargeRate();

	const AttackAttribute *pAttr = GetCorrectDirAttibute( pOwner );

	if( m_iCurCombo >= (int)m_vChargeComboAttListCenter.size() )
	{
		return;
	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	int	  iAniID	  = pGrp->GetAnimationIdx( pAttr->m_AttackAnimation );
	float fTimeRate   = pAttr->m_fAttackAniRate;
	DWORD dwPreDelay  = pAttr->m_dwPreDelay;
	m_dwFireDelayTime = pAttr->m_dwEndDelay;

	pOwner->RefreshFireTimeList( iAniID,
							     pAttr->m_vWeaponInfoList,
							     FTT_NORMAL_ATTACK,
							     fTimeRate,
							     dwPreDelay );

	pOwner->IncreaseWeaponIndexBase();

	m_dwAirJumpTime = pGrp->GetAnimationEventTime_e( iAniID, "air_jump" ) * fTimeRate;
	if( m_dwAirJumpTime > 0 )
		m_dwAirJumpTime += FRAMEGETTIME();

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	m_dwFireStartTime = FRAMEGETTIME();
	m_dwFireEndTime = pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + dwPreDelay;

	m_dwFireStateEndTime = pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;
	m_dwFireStateEndTime += FRAMEGETTIME();

	m_dwComboAttackTimeEnd = dwCurTime + pGrp->GetAnimationFullTime(iAniID) * fTimeRate + dwPreDelay;
	m_dwKeyReserveTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate + dwPreDelay;
	if( m_dwKeyReserveTime > 0 )
	{
		m_dwKeyReserveTime += dwCurTime;
	}

	ioBaseChar *pTarget = pOwner->GetBaseChar( m_AimedTarget );
	if( pTarget )
	{
		pOwner->SetTargetRotToTargetPos( pTarget, true );
	}

	float fWeight = GetAimedUpDownWeight( m_vAimedDir, 85.0f );
	{
		if( m_vAimedDir.y >= 0.0f )
		{
			AimedFireToUp( pGrp, fWeight, fTimeRate, dwPreDelay, true, false );
		}
		else
		{
			AimedFireToDown( pGrp, fWeight, fTimeRate, dwPreDelay, true, false );
		}
	}

	pOwner->SetReservedSliding( pAttr->m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	if( m_AimedTarget.IsEmpty() )
	{
		pOwner->SetTargetMarkerState( ioTargetMarker::MS_NONE );
	}

	m_FireState = FS_AIMED_FIRE;

	pOwner->SetChargingState( true );

	DWORD dwComboTime = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" )*fTimeRate;
	pOwner->SetComboCheckTime( dwComboTime, true );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << false;
		kPacket << (int)FS_COMBO_FIRE;
		kPacket << m_iCurCombo;
		kPacket << m_nNormalExtendAddAttackIndex;
		kPacket << pOwner->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	// 기본공격과 특별 공격을 섞어서 사용 가능
	m_nNormalExtendAddAttackIndex++;

	//  특수 기능2 : 서브 웨폰 발사
	SetSubWeaponAttackState( pOwner );
}

void ioEvaItem::ChangeToNormalAttack( ioBaseChar *pOwner )
{
	if( !pOwner ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	ClearCancelInfo();

	const AttackAttribute *pAttribute = m_FormalAttack.GetAttribute( AT_NORMAL, m_iCurCombo );
	if( pAttribute )
	{
		pOwner->SetNormalAttackByAttribute( *pAttribute );

		m_szCurAni					= pAttribute->m_AttackAnimation;
		m_fCurAniRate				= pAttribute->m_fAttackAniRate;
		m_dwInputCancelCheckTime	= dwCurTime;
		m_dwCancelCheckTime			= dwCurTime;
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

void ioEvaItem::OnItemGaugeOK( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner ) return;

	int iFireState;
	bool bZeroAimState;
	rkPacket >> iFireState;
	rkPacket >> m_iCurCombo;

	rkPacket >> m_AimedTarget;
	rkPacket >> m_bAimedBall;	

	switch( iFireState )
	{
		case FS_NORMAL_ATTACK:
		{
			ChangeToNormalAttack( pOwner );
			break;
		}
		case FS_CHARGING:
		{
			pOwner->NormalAttackOver();
			break;
		}

		case FS_AIMING:
		{
			rkPacket >> bZeroAimState;
			if( bZeroAimState )
				SetZeroAim( pOwner, bZeroAimState );
			break;
		}
		case FS_AIMED:
		{
			rkPacket >> bZeroAimState;
			if( bZeroAimState )
				SetZeroAim( pOwner, bZeroAimState );
			else
				ChangeToAimedState( pOwner );
			break;
		}

		case FS_AIMED_FIRE:
		{
			rkPacket >> m_vAimedDir;
			rkPacket >> m_bSetAimedDir;
			rkPacket >> m_bSetSkillAimed;
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			pOwner->SetTargetRot( qtRot );
			ChangeToAimedFire( pOwner );
			break;
		}

		case FS_NONE:
		{
			rkPacket >> m_vAimedDir;
			rkPacket >> m_bSetAimedDir;
			rkPacket >> m_bSetSkillAimed;
			break;
		}

		case FS_TELEPORT:
		{			
			TeleportMoveStartDash( pOwner );
			break;
		}
	}
}

void ioEvaItem::OnItemGaugeCancel( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	if( !pOwner )
	{
		return;
	}

	// 조준 상태에서 점프
	bool bAimJumpState;
	rkPacket >> bAimJumpState;

	if( bAimJumpState )
	{
		int iAimState;
		rkPacket >> iAimState;

		bool bFullTime = false;

		switch( iAimState )
		{
			case AJS_READY:
			{
				SetAimJumpReady( pOwner );
				break;
			}
			case AJS_JUMPPING:
			{
				rkPacket >> bFullTime;
				SetAimJumpping( pOwner, bFullTime );
				break;
			}

			case AJS_CANCEL:
			{
				SetAimJumpCancel( pOwner );
				break;
			}
		}
		return;
	}


	int nState;
	rkPacket >> nState;
	rkPacket >> m_iCurCombo;

	switch( nState )
	{
		case SS_SPECIAL_DASH:
		{
			D3DXVECTOR3 vWorldPos;
			rkPacket >> vWorldPos;
			pOwner->SetWorldPosition( vWorldPos);

			int	iCurDir, iCallerType;
			rkPacket >> iCurDir;
			rkPacket >> iCallerType;

			SetChangeConditionalSpecialDash( pOwner, (ioUserKeyInput::DirKeyInput)iCurDir, iCallerType );
			break;
		}

		case FS_AIMING:
		{
			// 1. FS_AIM_WAIT 중에 Lost Target
			// 2. FS_AIMED 중에 Lost Target
			m_dwTrackingEndTime = FRAMEGETTIME();

			// 대상을 제거한후 조준대기로 넘겨야함
			m_AimedTarget.Clear();
			m_bAimedBall = false;

			pOwner->SetTargetMarkerTarget( m_AimedTarget, m_bAimedBall );
			ChangeToAiming( pOwner );
			break;
		}
		case FS_AIM_WAIT:
		{
			rkPacket >> m_nNormalExtendAddAttackIndex;
			rkPacket >> m_AimedTarget;
			rkPacket >> m_bAimedBall;
			m_dwAimStartTime = FRAMEGETTIME();
			break;
		}

		case FS_NORMAL_FIRE:
		{			
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			pOwner->SetTargetRot( qtRot );

			rkPacket >> m_nNormalExtendAddAttackIndex;
			// 1. FS_AIMING 중에 Key off
			// 2. FS_AIM_WAIT 중에 key off
			ChangeToNormalFire( pOwner );
			break;
		}

		case FS_COMBO_FIRE:
		{
			rkPacket >> m_nNormalExtendAddAttackIndex;

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			pOwner->SetTargetRot( qtRot );
			ChangeToNextComboState( pOwner );
			break;
		}

		case FS_SUB_WEAPON:
		{
			int iSubWeaponInfoIndex;
			rkPacket >> iSubWeaponInfoIndex;
			CreateSubWeaponEffect( pOwner, iSubWeaponInfoIndex );
			break;
		}

		case FS_CANCEL_DASH: // 캔슬 텔레포트
		{
			int iDirKey = 0;;
			rkPacket >> iDirKey;

			// 에어리어웨폰 안에 있을 경우 텔레포트 한다.
			ioEvaDash* pEvaDash = ToEvaDash( GetCurExtendDash() );
			if( pEvaDash )
			{
				ClearCancelInfo();
				pOwner->ClearDirDoubleClick();
				pOwner->ClearReserveKeyInput();
				pOwner->ClearReservedSliding();
				pOwner->SetCurJumpPowerAmt( 0.0f );
				pOwner->SetForcePowerAmt( 0.0f );
				pOwner->SetGravityAmt( 0.0f );
				pOwner->SetChargingState( false );

				float fYawD = 180.0f + 45.0f * ( iDirKey - ioUserKeyInput::DKI_UP );
				fYawD = ioMath::ArrangeHead( fYawD );

				float fYawR = DEGtoRAD( fYawD );
				D3DXQUATERNION qtRotate;
				D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

				pOwner->SetTargetRotToRotate( qtRotate, true );

				pEvaDash->StartDash( pOwner );
			}
			break;
		}
	}
}

bool ioEvaItem::ProcessCancelBySpeicalDash( ioBaseChar *pOwner )
{
	if( m_szCurAni.IsEmpty() )			{	return false;	}
	if( m_dwInputCancelCheckTime == 0 )	{	return false;	}
	if( !pOwner )						{	return false;	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )	{	return false;	}

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
			// 에어리어웨폰 안에 있을 경우 텔레포트 한다.
			ioEvaDash* pEvaDash = ToEvaDash( GetCurExtendDash() );
			if( pEvaDash && pEvaDash->IsCanEvaTeleportDash( pOwner ) && pOwner->IsNeedProcess() && m_ConditionalSpecialDashKey != ioUserKeyInput::DKI_NONE )
			{
				float fYawD = 180.0f + 45.0f * ( m_ConditionalSpecialDashKey - ioUserKeyInput::DKI_UP );
				fYawD = ioMath::ArrangeHead( fYawD );

				float fYawR = DEGtoRAD( fYawD );
				D3DXQUATERNION qtRotate;
				D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

				pOwner->SetTargetRotToRotate( qtRotate, true );

				pEvaDash->StartDash( pOwner );

				if ( pOwner->IsNeedSendNetwork() )
				{
					SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
					FillItemGaugePacketHeader( pOwner, kPacket );
					kPacket << false;
					kPacket << (int)FS_CANCEL_DASH;
					kPacket << m_iCurCombo;
					kPacket << (int)m_ConditionalSpecialDashKey;
					P2PNetwork::SendToAllPlayingUser( kPacket );
				}

				ClearCancelInfo();
				pOwner->ClearDirDoubleClick();
				pOwner->ClearReserveKeyInput();
				pOwner->ClearReservedSliding();
				pOwner->SetCurJumpPowerAmt( 0.0f );
				pOwner->SetForcePowerAmt( 0.0f );
				pOwner->SetGravityAmt( 0.0f );
				pOwner->SetChargingState( false );

				return true;
			}

			SetChangeConditionalSpecialDash( pOwner, m_ConditionalSpecialDashKey, ioAttackableItem::CCSD_NORMAL );
			return true;
		}
	}

	return false;
}

bool ioEvaItem::IsConditionalSpecialDashReserve( ioBaseChar *pOwner )
{
	if( m_SpecialState == SS_SPECIAL_DASH )
	{
		return	false;
	}

	return	true;
}

void ioEvaItem::SetChangeConditionalSpecialDash( ioBaseChar *pOwner, ioUserKeyInput::DirKeyInput eCurrDir, int eCallerType )
{
	if( !pOwner || eCurrDir == ioUserKeyInput::DKI_NONE )	{	return;	}

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )	{	return;	}

	int iAniID = pGrp->GetAnimationIdx( m_NormalCancelDashAttribute.m_AttackAnimation );
	if ( iAniID == -1 )	{	return;	}

	DWORD dwPreDelay = m_NormalCancelDashAttribute.m_dwPreDelay;
	DWORD dwEndDelay = m_NormalCancelDashAttribute.m_dwEndDelay;
	DWORD dwStartTime = FRAMEGETTIME() + dwPreDelay;

	ClearCancelInfo();
	pOwner->ClearDirDoubleClick();
	pOwner->ClearReserveKeyInput();
	pOwner->ClearReservedSliding();
	pOwner->SetCurJumpPowerAmt( 0.0f );
	pOwner->SetForcePowerAmt( 0.0f );
	pOwner->SetGravityAmt( 0.0f );
	pOwner->SetChargingState( false );

	float fYawD = 180.0f + 45.0f * ( eCurrDir - ioUserKeyInput::DKI_UP );
	fYawD = ioMath::ArrangeHead( fYawD );

	float fYawR = DEGtoRAD( fYawD );
	D3DXQUATERNION qtRotate;
	D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );


	pOwner->SetTargetRotToRotate( qtRotate, true, false );

	float fTimeRate = m_NormalCancelDashAttribute.m_fAttackAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pGrp->ClearAllActionAni( 0.0f, true );
	pGrp->ClearAllLoopAni( 0.0f, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

	pOwner->SetReservedSliding( m_NormalCancelDashAttribute.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	DWORD dwCurBaseIndex = pOwner->IncreaseWeaponIndexBase();
	pOwner->RefreshFireTimeListWithBase( dwCurBaseIndex,
		iAniID,
		m_NormalCancelDashAttribute.m_vWeaponInfoList,
		FTT_NORMAL_ATTACK,
		fTimeRate,
		dwPreDelay );

	pOwner->CheckCharColSkipTime( iAniID, fTimeRate, dwPreDelay );

	m_dwMotionEndTime = dwStartTime + pGrp->GetAnimationFullTime(iAniID) * fTimeRate + dwEndDelay;

	m_dwKeyReserveTime	= 0;
	DWORD dwKeyReserve = pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;
	if( dwKeyReserve > 0 )
		m_dwKeyReserveTime += dwStartTime;

	DWORD dwInputSpecialDashTime = pGrp->GetAnimationEventTime_e( iAniID, "input_dash_s" ) * fTimeRate;
	if( dwInputSpecialDashTime > 0 )
		m_dwCancelDashAttackTime = dwStartTime + dwInputSpecialDashTime;

	m_SpecialState	= SS_SPECIAL_DASH;
	pOwner->SetState( CS_EVA_SPECIAL );
	if ( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );	
		kPacket << false;
		kPacket << m_SpecialState;
		kPacket << m_iCurCombo;
		kPacket << pOwner->GetWorldPosition();
		kPacket << static_cast<int>( eCurrDir );
		kPacket << eCallerType;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

// ioBaseChar::CheckEnableDashCheck() 함수를 체크 하기 위해서는 m_CurDashType == DT_EVA 추가해줘야 함 
// 또한 ioAttackableItem 기본은 무조건 false 리턴시킨다.
bool ioEvaItem::IsEnableAttackDashCheck( ioBaseChar *pOwner, bool bCheck )
{
	// bCheck : ioBaseChar:: CheckEnableDashCheck() 함수에서는 true
	// bCheck : ioBaseChar::CheckNormalAttackState() 함수에서는 false
	if ( !pOwner )
		return	false;
	
	// 기본공격중 일때 m_bEnableDashCheck 상태 값을 변경하여 ioBaseChar::ProcessKeyInput() 함수에서 체크 중..
	CharState charstate = pOwner->GetState();
	if( pOwner->GetState() == CS_ATTACK )
	{
		switch( m_FireState )
		{
			case FS_NORMAL_ATTACK:
			{
				if( bCheck ) // ioBaseChar::CheckEnableDashCheck() 함수에서 bCheck = true 경우 처리 ( 주로 캔슬 대시 할때 )
				{
					return true;
				}

				break;
			}
			case FS_AIMING:
			case FS_AIM_WAIT:
			case FS_AIMED:
			case FS_COMBO_FIRE:
			{
				return true;
			}
		}
	}

	return	false;
}

void ioEvaItem::ChangeReserveKeyInputState( ioBaseChar *pOwner, int iCurCombo /* = 0 */ )
{
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
				if ( !COMPARE( iCurCombo, 0, GetMaxCombo() ) )
					iCurCombo = 0;
				pOwner->SetNormalAttack( iCurCombo );
			}
		}
		return;
	}

	// 대기
	pOwner->SetState( CS_DELAY );
}

void ioEvaItem::ProcessSpecialState( ioBaseChar *pOwner )
{
	if( !pOwner )
	{
		return;
	}

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
	{
		return;
	}

	DWORD dwCurTime = FRAMEGETTIME();
	switch( m_SpecialState )
	{
		case SS_SPECIAL_DASH:
		{
			ProcessSpecialDash( pOwner );
			break;
		}

		case SS_TELEPORT_START:
		{
			CheckTeleportState( pOwner );
			m_AimedTarget.Clear();
			break;
		}

		case SS_TELEPORT_END:
		{
			// 2020-11-20
			m_SpecialState = SS_NONE;
			pOwner->SetState( CS_DELAY );
			m_dwMotionEndTime = 0;
			m_FireState = FS_NONE;
			break;
		}
		
		case SS_TELEPORT_END_ACTION:
		{
			ioPlayStage *pStage = pOwner->GetCreator();
			pStage->CreateMapEffect( m_szTeleportEndEffect, m_vTeleportPos, ioMath::UNIT_ALL );

			ioPlayStage* pPlayStage = pOwner->GetCreator();
			float fMapHeight = pPlayStage->GetMapHeightOnlyTerrain( m_vTeleportPos.x, m_vTeleportPos.z );
			if ( m_vTeleportPos.y - fMapHeight >= m_fJumpHeightGap )
			{
				m_SpecialState = SS_OTHER_ACTION;
				SetOtherAction( pOwner );
			}
			else
			{
				ioEntityGroup *pGrp = pOwner->GetGroup();
				int iAniID = pGrp->GetAnimationIdx( m_TeleportMoveEndAction.m_AttackAnimation );
				if ( iAniID == -1 )
				{
					pOwner->SetState( CS_DELAY );
					return;
				}

				DWORD dwCurTime = FRAMEGETTIME();
				DWORD dwPreDelay = m_TeleportMoveEndAction.m_dwPreDelay;
				float fTimeRate  = m_TeleportMoveEndAction.m_fAttackAniRate;

				pGrp->ClearAllLoopAni( FLOAT100, true );
				pGrp->ClearAllActionAni( FLOAT100, true );
				pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );	

				pOwner->RefreshFireTimeList( iAniID, m_TeleportMoveEndAction.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay, true );

				m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + dwPreDelay;
				m_dwKeyReserveTime = dwCurTime + pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate + dwPreDelay;
				m_dwTeleportEndDelayTime = dwCurTime + m_dwTeleportEndDelay;
				m_SpecialState = SS_TELEPORT_END;
			}
			break;
		}
	}
}

void ioEvaItem::ClearAttackState( ioBaseChar *pOwner )
{

}

void ioEvaItem::ProcessSpecialDash( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	DWORD	dwCurTime	= FRAMEGETTIME();

	if( m_dwMotionEndTime < dwCurTime )
	{
		m_FireState		= FS_NONE;
		m_SpecialState	= SS_NONE;
		ChangeReserveKeyInputState( pOwner );

		return;
	}

	if ( m_dwKeyReserveTime > 0 && m_dwKeyReserveTime < dwCurTime )
	{
		CheckReserve( pOwner );
	}
}

bool ioEvaItem::CheckCurrentTargetValid( ioBaseChar *pOwner, bool bAngleAlsoCheck )
{
	ioBall *pBall = pOwner->FindBall();
	ioBaseChar *pTarget = pOwner->GetBaseChar( m_AimedTarget );
	if( !pTarget && !m_bAimedBall )
		return false;
	else if( !pTarget && !pBall )
	{
		m_bAimedBall = false;
		return false;
	}

	D3DXVECTOR3 vTargetPos;
	if( pTarget )
	{
		if( !pTarget->IsEnableAimTarget(m_bEnableDownAim) )
			return false;

		if( pTarget->HasHideBuff() )
			return false;

		if( pTarget->IsChatModeState( false ) )
			return false;

		vTargetPos = pTarget->GetWorldPosition();
	}
	else if( m_bAimedBall && pBall )
	{
		vTargetPos = pBall->GetWorldPosition();
	}

	float fAimRange = GetChargeAimRange();
	float fAimAngle = GetChargeAimAngle();

	D3DXVECTOR3 vDiff = vTargetPos - pOwner->GetWorldPosition();
	if( D3DXVec3LengthSq( &vDiff ) > fAimRange * fAimRange )
		return false;

	if( D3DXVec3LengthSq( &vDiff ) < m_fMinRange * m_fMinRange )
		return false;

	if( bAngleAlsoCheck )
	{
		vDiff.y = 0.0f;
		D3DXVec3Normalize( &vDiff, &vDiff );

		if( m_bChargeCharRotate )
		{
			D3DXVECTOR3 vCharDir;
			D3DXQUATERNION qtRot = pOwner->GetTargetRot();
			vCharDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

			float fHalfCosine = cosf( DEGtoRAD( m_fAimAngle * FLOAT05 ) );
			if( D3DXVec3Dot( &vCharDir, &vDiff ) < fHalfCosine )
			{
				return false;
			}
		}
		else
		{
			float fHalfCosine = cosf( DEGtoRAD( fAimAngle * FLOAT05 ) );
			if( D3DXVec3Dot( &m_vStartDir, &vDiff ) < fHalfCosine )
				return false;
		}
	}

	return true;
}

void ioEvaItem::UpdateAimDir( ioBaseChar *pOwner, bool bJumpAim  )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	ioBiped *pBiped = pGrp->GetBiped("Bip01 Spine1");
	if(!pBiped)	return;

	DWORD dwCurTime = FRAMEGETTIME();
	bool bStateCheck = false;
	switch( m_FireState )
	{
	case FS_AIMING:
	case FS_AIM_WAIT:
	case FS_AIMED:
	case FS_AIMED_FIRE:
		bStateCheck = true;
	}

	if( bJumpAim )
		bStateCheck = true;

	ioBall *pBall = pOwner->FindBall();
	ioBaseChar *pTarget = pOwner->GetBaseChar( m_AimedTarget );
	if( !bStateCheck )
	{
		m_fCurAngle = 0.0f;
		pBiped->ReleaseExtraRotation();
		return;
	}

	if( !pTarget && m_bChargeCharRotate && m_bChangeDir )
	{
		m_fCurAngle = 0.0f;
		pBiped->ReleaseExtraRotation();
		return;
	}

	D3DXVECTOR3 vTargetPos;
	if( pTarget )
	{
		vTargetPos = pTarget->GetWorldPosition();
	}
	else if( m_bAimedBall && pBall )
	{
		vTargetPos = pBall->GetWorldPosition();
	}

	float fRotAngle = 0.0f;

	if( m_bAimJumpAttack || m_FireState == FS_AIMED_FIRE )
	{
		if( m_dwFireStateEndTime < dwCurTime )
		{
			float fTotal, fGap;
			DWORD dwEndTime = m_dwFireStartTime + m_dwFireEndTime;
			fTotal = fGap = 0.0f;

			if( dwEndTime > m_dwFireStateEndTime )
			{
				fTotal = (float)(dwEndTime - m_dwFireStateEndTime);
			}

			if( dwEndTime > dwCurTime )
			{
				fGap = (float)(dwEndTime - dwCurTime);
			}

			float fRate = 0.0f;
			if( fTotal > 0.0f )
			{
				fRate = fGap / fTotal;
			}

			fRotAngle = m_fCurAngle * fRate;
		}
		else
		{
			fRotAngle = m_fCurAngle;
		}
	}
	else
	{
		bool bTarget = true;
		if( pTarget && !pTarget->IsEnableAimTarget(m_bEnableDownAim) )
		{
			bTarget = false;
		}
		else if( m_bAimedBall && !pBall )
		{
			bTarget = false;
		}
		else if( !pTarget && !m_bAimedBall )
		{
			bTarget = false;
		}

		if( !bTarget )
		{
			float fTotal, fGap;
			DWORD dwEndTime = m_dwTrackingEndTime + 500;
			fTotal = fGap = 0.0f;

			fTotal = FLOAT500;
			if( dwEndTime > dwCurTime )
			{
				fGap = (float)(dwEndTime - dwCurTime);
			}
			else
			{
				m_dwTrackingEndTime = 0;
			}

			if( m_dwTrackingEndTime == 0 )
			{
				m_fCurAngle = 0.0f;
				pBiped->ReleaseExtraRotation();
				return;
			}

			float fRate = 0.0f;
			if( fTotal > 0.0f )
			{
				fRate = fGap / fTotal;
			}

			fRotAngle = m_fCurAngle * fRate;
		}
		else
		{
			D3DXVECTOR3 vSightDir = vTargetPos - pOwner->GetWorldPosition();
			D3DXVec3Normalize( &vSightDir, &vSightDir );

			D3DXVECTOR3 vXZDir( vSightDir.x, 0.0f, vSightDir.z );
			D3DXVec3Normalize( &vXZDir, &vXZDir );

			D3DXQUATERNION qtTargetRot;
			ioMath::CalcDirectionQuaternion( qtTargetRot, -vXZDir );

			float fCurYaw, fTargetYaw;
			fTargetYaw = RADtoDEG( ioMath::QuaterToYaw( qtTargetRot ) );
			fTargetYaw = ioMath::ArrangeHead( fTargetYaw );
			fCurYaw = RADtoDEG( ioMath::QuaterToYaw(pOwner->GetTargetRot()) );
			fCurYaw = ioMath::ArrangeHead( fCurYaw );

			m_fCurAngle = DEGtoRAD( ioMath::AngleGap( fCurYaw, fTargetYaw ) );

			float fAimTimeRate = FLOAT1;
			DWORD dwTimeGap = dwCurTime - m_dwAimStartTime;
			if( dwTimeGap < m_dwAimTime )
			{
				fAimTimeRate = (float)dwTimeGap / (float)m_dwAimTime;
			}
			
			fRotAngle = m_fCurAngle * fAimTimeRate;
		}
	}

	D3DXQUATERNION qtExtraRot;
	D3DXQuaternionRotationYawPitchRoll( &qtExtraRot, 0.0f, fRotAngle, 0.0f );
	pBiped->SetExtraRotation( qtExtraRot, true );
}

void ioEvaItem::OnEquiped( ioBaseChar *pOwner )
{
	ioWeaponItem::OnEquiped( pOwner );

	pOwner->SetTargetMarker( ioTargetMarker::MT_BOW_ITEM,
		m_szReadyMarker, m_szAimingMarker, m_szAimedMarker, m_szZeroAimedMarker, "" );

	pOwner->SetTargetMarkerRange( m_fMarkerRange );

	m_dwCheckCoolTime = FRAMEGETTIME();
}

void ioEvaItem::OnReleased( ioBaseChar *pOwner )
{
	pOwner->SetTargetMarker( ioTargetMarker::MT_NONE, "", "", "", "", "" );

	m_fReloadTimeModifyRate = FLOAT1;
	m_fFireTimeModifyRate = FLOAT1;

	//  특수 기능2 : 기본 공격 및 점프 공격 타격 시 서브웨폰
	EndWeaponEffec( pOwner );

	ioWeaponItem::OnReleased( pOwner );
}

ioHashString ioEvaItem::FindAimTarget( ioBaseChar *pOwner, const ioHashString& stChecker )
{
	ioHashString szTarget;
	m_AimedTarget.Clear();

	float fAimAngle = GetChargeAimAngle();
	float fAimRange = GetChargeAimRange();

	if( pOwner->CheckAimedTargetByBow( fAimAngle, m_fMinRange, fAimRange, szTarget, m_bEnableDownAim ) )
	{
		if( stChecker.IsEmpty() )
		{
			m_AimedTarget = szTarget;
		}
		else if( !stChecker.IsEmpty() && stChecker == szTarget )
		{
			m_AimedTarget = szTarget;
		}
		else 
		{
			m_AimedTarget.Clear();
			
		}
	}

	return m_AimedTarget;
}

float ioEvaItem::GetChargeAimAngle()
{
	if( m_fCurChargeRate > 0.0f )
	{
		float fCurAimAngleRate = FLOAT1;
		float fCurMaxAimAngle = max( 0.0f, (m_fMaxAimAngleRate - FLOAT1) );

		fCurAimAngleRate += fCurMaxAimAngle * m_fCurChargeRate;

		float fCurAngle = m_fAimAngle * fCurAimAngleRate;
		return fCurAngle;
	}

	return m_fAimAngle;
}

float ioEvaItem::GetChargeAimRange()
{
	if( m_fCurChargeRate > 0.0f )
	{
		float fCurAimRangeRate = FLOAT1;
		float fCurMaxAimRange = max( 0.0f, (m_fMaxAimRangeRate - FLOAT1) );

		fCurAimRangeRate += fCurMaxAimRange * m_fCurChargeRate;

		float fCurRange = m_fAimRange * fCurAimRangeRate;
		if( m_FireState == FS_COMBO_FIRE )
		{
			fCurRange = m_fAimRangeInFight * fCurAimRangeRate;
		}

		return fCurRange;
	}

	if( m_FireState == FS_COMBO_FIRE )
	{
		return m_fAimRangeInFight;
	}

	return m_fAimRange;
}

void ioEvaItem::TrackingTarget( ioBaseChar *pOwner, const ioHashString& stUp, const ioHashString& stCenter, const ioHashString& stDown )
{
	bool bJumpState = false;
	if( pOwner->IsJumpState() )
	{
		bJumpState = true;
	}

	ioBall *pBall = pOwner->FindBall();
	ioBaseChar *pTarget = pOwner->GetBaseChar( m_AimedTarget );

	ioEntityGroup *pGrp = pOwner->GetGroup();
	ioAnimation *pUp, *pCenter, *pDown;

	if( bJumpState )
	{
		pUp = pGrp->GetLoopInPlay( stUp.IsEmpty() ? m_JumpAimUp : stUp );
		pCenter = pGrp->GetLoopInPlay( stCenter.IsEmpty() ?	m_JumpAimCenter : stCenter );
		pDown = pGrp->GetLoopInPlay( stDown.IsEmpty() ? m_JumpAimDown : stDown );
	}
	else if( m_bZeroAimState )
	{
		pUp = pGrp->GetLoopInPlay( m_ZeroAimUp );
		pCenter = pGrp->GetLoopInPlay( m_ZeroAimCenter );
		pDown = pGrp->GetLoopInPlay( m_ZeroAimDown );
	}
	else
	{
		pUp = pGrp->GetLoopInPlay( m_AimUp );
		pCenter = pGrp->GetLoopInPlay( m_AimCenter );
		pDown = pGrp->GetLoopInPlay( m_AimDown );
	}

	D3DXVECTOR3 vAimedDir(0, 0, 0);
	if( pTarget )
		vAimedDir = pTarget->GetMidPositionByRate() - pOwner->GetMidPositionByRate();
	else if( m_bAimedBall && pBall )
		vAimedDir = pBall->GetMidPositionByRate() - pOwner->GetMidPositionByRate();
	else
		vAimedDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

	//////////////////////////////////////////////////////////////////////////
	if( pTarget && !bJumpState )
	{
		D3DXVECTOR3 vTargetPos;
		vTargetPos = pTarget->GetMidPositionByRate();
		pOwner->SetTargetRotToTargetPos( vTargetPos, true, false );
	}
	//////////////////////////////////////////////////////////////////////////

	D3DXVec3Normalize( &vAimedDir, &vAimedDir );
	float fAniWeight = GetAimedUpDownWeight( vAimedDir, 85.0f );

	m_vAimedDir = vAimedDir;
	m_bSetAimedDir = true;
	m_bAimedState = true;

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

bool ioEvaItem::CheckDownAimState( ioBaseChar *pTarget )
{
	if( !pTarget ) return false;

	CharState eState = pTarget->GetState();

	if( eState == CS_FROZEN && pTarget->GetBlowFrozenState() && m_bEnableDownAim )
		return true;

	if( eState == CS_BLOW_WOUNDED && !pTarget->IsFloatingState() && m_bEnableDownAim )
		return true;

	if( eState == CS_BOUND_BLOW_WOUNDED && !pTarget->IsFloatingState() && m_bEnableDownAim )
		return true;

	if( eState == CS_ETC_STATE && pTarget->IsApplyDownState(false) && m_bEnableDownAim )
		return true;

	if( eState == CS_USING_SKILL && pTarget->IsApplyDownState(false) && m_bEnableDownAim )
		return true;

	if( eState == CS_STOP_MOTION && pTarget->GetBlowStopMotionState() && m_bEnableDownAim )
		return true;

	return false;
}


D3DXVECTOR3 ioEvaItem::ReCalcMoveDir( ioBaseChar *pOwner, ioWeapon *pWeapon, bool bSkillTest )
{
	ioBaseChar *pTarget = pOwner->GetBaseChar( m_AimedTarget );
	if( !pTarget && !m_bAimedBall )
		return m_vAimedDir;

	ioBall *pBall = pOwner->FindBall();
	if( !pTarget && !pBall ) return m_vAimedDir;

	D3DXVECTOR3 vTargetPos;
	D3DXVECTOR3 vWeaponPos = pWeapon->GetPosition();

	if( pTarget )
	{
		if( CheckDownAimState( pTarget ) )
			vTargetPos = pTarget->GetMidPositionByRate( m_fDownTargetAimRate );
		else
			vTargetPos = pTarget->GetMidPositionByRate( m_fTargetAimRate );
	}
	else if( m_bAimedBall && pBall )
		vTargetPos = pBall->GetMidPositionByRate();

	D3DXVECTOR3 vAimedDir =  vTargetPos - vWeaponPos;
	D3DXVec3Normalize( &vAimedDir, &vAimedDir );
	
	if( bSkillTest == false )
	{
		vAimedDir.x = m_vAimedDir.x;
		vAimedDir.z = m_vAimedDir.z;
	}

	D3DXVec3Normalize( &vAimedDir, &vAimedDir );

	return vAimedDir;
}

float ioEvaItem::GetAimedUpDownWeight( const D3DXVECTOR3 &vAimDir, float fMaxAngle )
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

bool ioEvaItem::IsChargeAttackState( ioBaseChar *pOwner )
{
	if( pOwner->GetState() == CS_ATTACK )
	{
		if( m_FireState == FS_CHARGING ||
			m_FireState == FS_AIMING ||
			m_FireState == FS_AIM_WAIT ||
			m_FireState == FS_AIMED ||
			m_FireState == FS_AIMED_FIRE ||
			m_FireState == FS_NORMAL_FIRE ||
			m_FireState == FS_COMBO_FIRE 
			)
			return true;
	}

	return false;
}

void ioEvaItem::SetExtraInfoToWeapon( ioWeapon *pWeapon, bool bJumpAttack )
{
	ioAttackableItem::SetExtraInfoToWeapon( pWeapon, bJumpAttack );

	ioArrowWeapon *pArrow = ToArrowWeapon( pWeapon );
	ioMissileWeapon *pMissile = ToMissileWeapon( pWeapon );
	ioReturnPingpongWeapon *pReturnPingpong = ToReturnPingpongWeapon( pWeapon );
	ioZoneEffectWeapon * pZoneEffect = ToZoneEffectWeapon( pWeapon );
	ioTraceWeaponType2 *pTrace = ToTraceWeaponType2( pWeapon );

	// 게이지 감소
	if( pArrow || pMissile || pReturnPingpong || pTrace )
	{
		DecreaseCoolTime();
	}


	D3DXVECTOR3 vAimedDir = m_vAimedDir;
	if( pZoneEffect )
	{
		ioBaseChar *pChar = pZoneEffect->GetOwner();
		if( pChar )
		{
			D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			pZoneEffect->SetCollisionDir( vDir );
			pZoneEffect->SetZoneValue( true, false );
		}
	}

	// 웨폰 발사시 지정해줌 2020-11-20
	//if( pTrace && !m_AimedTarget.IsEmpty() )
	//{
	//	pTrace->SetTraceTarget( m_AimedTarget );
	//}

	if( bJumpAttack )
	{
		if( !m_AimedTarget.IsEmpty() || m_bAimedState )
		{
			vAimedDir = ReCalcMoveDir( pWeapon->GetOwner(), pWeapon );
			pWeapon->SetMoveDir( vAimedDir );
		}

		m_bSetAimedDir = false;

		if( pMissile && m_vSecondAttributeList.size() > 0 )
		{
			pMissile->SetSecondAttribute( m_vSecondAttributeList );
		}
	}
	else
	{
		if( m_FireState == FS_AIMED_FIRE && !m_AimedTarget.IsEmpty() )
		{
			vAimedDir = ReCalcMoveDir( pWeapon->GetOwner(), pWeapon );
			pWeapon->SetMoveDir( vAimedDir );
			m_bSetAimedDir = false;
		}

		if( m_bZeroAimState )
		{
			if( pMissile && m_vSecondZeroAttributeList.size() > 0 )
			{
				pMissile->SetSecondAttribute( m_vSecondZeroAttributeList );
			}
		}
		else
		{
			if( pMissile && m_vSecondAttributeList.size() > 0 )
			{
				pMissile->SetSecondAttribute( m_vSecondAttributeList );
			}
		}

		if( m_fCurChargeRate > 0.0f )
		{
			float fCurDamageRate, fCurForceRate, fCurRangeRate, fCurSpeedRate, fCurBlowRate, fCurFloatRate;
			float fCurLiveTimeRate;
			fCurDamageRate = fCurForceRate = fCurRangeRate = fCurSpeedRate = fCurBlowRate = fCurFloatRate = FLOAT1;
			fCurLiveTimeRate = FLOAT1;

			float fCurMaxDamage = max( 0.0f, (m_fMaxDamageRate - FLOAT1) );
			float fCurMaxForce = max( 0.0f, (m_fMaxForceRate - FLOAT1) );
			float fCurMaxBlow = max( 0.0f, (m_fMaxBlowRate - FLOAT1) );

			float fCurMaxRange = max( 0.0f, (m_fMaxRangeRate - FLOAT1) );
			float fCurMaxSpeed = max( 0.0f, (m_fMaxSpeedRate - FLOAT1) );
			float fCurMaxFloat = max( 0.0f, (m_fMaxFloatRate - FLOAT1) );
			float fCurMaxLiveTime = max( 0.0f, (m_fMaxLiveTimeRate - FLOAT1) );

			fCurDamageRate += fCurMaxDamage * m_fCurChargeRate;
			fCurForceRate += fCurMaxForce * m_fCurChargeRate;
			fCurBlowRate += fCurMaxBlow * m_fCurChargeRate;

			fCurRangeRate += fCurMaxRange * m_fCurChargeRate;
			fCurSpeedRate += fCurMaxSpeed * m_fCurChargeRate;
			fCurFloatRate += fCurMaxFloat * m_fCurChargeRate;

			fCurLiveTimeRate += fCurMaxLiveTime * m_fCurChargeRate;

			pWeapon->SetOwnerChargeDamageRate( fCurDamageRate );
			pWeapon->SetOwnerChargeForceRate( fCurForceRate );
			pWeapon->SetOwnerChargeBlowRate( fCurBlowRate );

			pWeapon->SetOwnerChargeRangeRate( fCurRangeRate );
			pWeapon->SetOwnerChargeSpeedRate( fCurSpeedRate );
			pWeapon->SetOwnerChargeFloatRate( fCurFloatRate );
			
			pWeapon->SetOwnerChargeLiveTimeRate( fCurLiveTimeRate );
		}
	}

	m_fCurChargeRate = 0.0f;
	m_dwAimStartTime = 0;
}

void ioEvaItem::SetExtraGaugeTypeByWeapon( ioBaseChar *pOwner, int iType, int iWGT, float fDamage )
{
	float fGauge = 0.0f;
	switch( iType )
	{
		case EGT_ATTACK:
		case EGT_ATTACK_DEF:
		{
			// 2020-11-19 자신인지체크
			if( pOwner->IsNeedProcess() )
			{
				//  특수 기능2 : 기본 공격 및 점프 공격 타격 시 서브웨폰 생성
				int iSubWeaponInfoIndex = 0;
				for( iSubWeaponInfoIndex = 0; iSubWeaponInfoIndex < (int)m_SubWeaponEffectinfoList.size(); iSubWeaponInfoIndex++ )
				{
					if( m_SubWeaponEffectinfoList[iSubWeaponInfoIndex].m_dwSubWeaponEffectIndex == -1 )
					{
						break;	
					}
				}

				CreateSubWeaponEffect( pOwner, iSubWeaponInfoIndex );
			}
			break;
		}
	}
}

// 게이지
void ioEvaItem::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner )	{	return;		}

	// Check Bullet
	if( m_dwMaxCoolTime <= m_fCurCoolTime )
	{
		return;
	}

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();
	float fGauge = m_fRecoveryTime * fTimePerSec;

	m_fCurCoolTime += fGauge;
	m_fCurCoolTime = (float)min( m_fCurCoolTime, (float)m_dwMaxCoolTime );
}

int ioEvaItem::GetMaxBullet()
{
	return (int)m_dwMaxCoolTime;
}

int ioEvaItem::GetNeedBullet()
{
	return (int)m_dwNeedCoolTime;
}

int ioEvaItem::GetCurBullet()
{
	DWORD dwCurCoolTime = m_fCurCoolTime;
	dwCurCoolTime = max( 0, min( dwCurCoolTime, m_dwMaxCoolTime ) );

	return (int)dwCurCoolTime;
}

void ioEvaItem::SetCurBullet( int iCurBullet )
{
	// 용병 교체 시 불렛 게이지 초기화 사용 유무
	if( m_bBulletGaugeInit )
	{
		return;
	}

	if( iCurBullet < 0 )
		return;

	DWORD dwCurBullet = iCurBullet;
	dwCurBullet = max( 0, min( dwCurBullet, m_dwMaxCoolTime ) );

	m_fCurCoolTime = (float)dwCurBullet;

	this->GetName();
}

void ioEvaItem::InitCoolTime()
{
	m_fCurCoolTime = 0.0f;
}

void ioEvaItem::MaxCoolTime()
{
	m_fCurCoolTime = (float)m_dwMaxCoolTime;
}

void ioEvaItem::DecreaseCoolTime()
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

// 에어리어웨폰 범위 밖에서 A~
void ioEvaItem::SetJumpping( ioBaseChar *pOwner, bool bFullTime )
{
	if ( bFullTime )
	{
		//ChangeToSpecialAttack( pOwner, CS_NEPHILIM_BOMB_STATE, CS_BOMB_ATTACK, false );
		// 에어리어웨폰 밖에 있을 때 해당 적의 앞으로 텔레포트 하는데 여러개 생성시 가까운 적으로 이동한다.		
		if( ChangeToChargeJump( pOwner ) )
		{
			return;
		}
	}

	ioAttackableItem::SetJumpping( pOwner, bFullTime );
}

// 에어리어웨폰 범위 밖에서 A~
bool ioEvaItem::ChangeToChargeJump( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;
	
	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )	{	return false;	}

	// 2020-11-20
	if( pOwner->GetState() == CS_EVA_SPECIAL )
	{
		return true;
	}

	ioAreaWeapon* pAreaWeapon = g_AreaWeaponMgr.FindAreaWeaponInRangeByName( m_szSpecialCheckBuff, pOwner->GetCharName(), pOwner->GetWorldPosition() );
	if( pAreaWeapon )
	{
		// 에어리어웨폰 범위 안에서는 기본 점프시킨다. 
		return false;
	}
	
	// 에어리어웨폰 범위 밖에 있으면 텔레포트 시킨다.
	std::vector< ioAreaWeapon* > vAreaWeaponList;
	vAreaWeaponList.clear();
	g_AreaWeaponMgr.FindAllAreaWeaponByName( m_szSpecialCheckBuff, vAreaWeaponList );
	if( vAreaWeaponList.empty() )
	{
		// 생성된 에어리어웨폰이 없으면 기본 점프
		return false;
	}

	SortTargetList( pOwner, vAreaWeaponList );
	int nTargetSortCnt = (int)m_SortList.size();
	for ( int i=0; i<nTargetSortCnt; i++ )
	{
		ioAreaWeapon *pAreaWeapon = m_SortList[i].pAreaWeapon;
		if( pAreaWeapon )
		{
			// 자신이 생성한 에어리어웨폰 인지 검사
			if( pAreaWeapon->GetOwnerName() == pOwner->GetCharName() )
			{
				// 에어리어 웨폰으로 하지 않고 타겟으로 한다.
				m_AimedTarget = pAreaWeapon->GetTargetName();
				TeleportMoveStartDash( pOwner );
				return true;
			}
		}
	}

	return false;
}

//타겟 리스트 가까운 순서로 정렬
void ioEvaItem::SortTargetList( ioBaseChar *pOwner, const std::vector< ioAreaWeapon* > &TargetList )
{
	if( !pOwner || TargetList.empty() )	
		return;

	m_SortList.erase( m_SortList.begin(), m_SortList.end() );
	m_SortList.reserve( TargetList.size() );

	D3DXVECTOR3 vDiff;
	D3DXVECTOR3 vOwnerPos = pOwner->GetWorldPosition();
	
	std::vector< ioAreaWeapon* >::const_iterator iter;
	for( iter=TargetList.begin(); iter!=TargetList.end(); ++iter )
	{
		ioAreaWeapon *pAreaWeapon = *iter;
		if( pAreaWeapon )
		{
			vDiff = pAreaWeapon->GetStartPos() - vOwnerPos;
			float fCurDistSq = D3DXVec3LengthSq( &vDiff );

			DistInfo sDist;
			sDist.pAreaWeapon = pAreaWeapon;
			sDist.fDistSq = fCurDistSq;
			m_SortList.push_back( sDist );
		}
	}

	std::sort( m_SortList.begin(), m_SortList.end() );
}

void ioEvaItem::CreateSubWeaponEffect( ioBaseChar *pOwner, int iSubWeaponInfoIndex )
{
	if ( !pOwner )										{	return;	}
	if( m_iSubWeaponInfoIndex >= m_iMaxSubWeaponCnt )	{	return;	}

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )										{	return;	}
	if( m_SubWeaponInfoList.empty() )					{	return;	}


	// 하나 생성 
	D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate() + pOwner->GetWorldOrientation() * m_SubWeaponInfoList[iSubWeaponInfoIndex].m_vWeaponPos;
	DWORD dwEffectID = CreateEffect( pOwner, m_SubWeaponInfoList[iSubWeaponInfoIndex].m_szWeaponEffect, vPos, D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 ) );
	SubWeaponEffectInfo subweaponeffectinfo;
	subweaponeffectinfo.m_dwSubWeaponEffectIndex = dwEffectID;
	
	int nSize = (int)m_SubWeaponEffectinfoList.size();
	if( nSize <= iSubWeaponInfoIndex  )
	{
		m_SubWeaponEffectinfoList.push_back( subweaponeffectinfo );
	}
	else
	{
		m_SubWeaponEffectinfoList[iSubWeaponInfoIndex].m_dwSubWeaponEffectIndex = dwEffectID;
	}
	
	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_CANCEL );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << false;	//AimJump
		kPacket << (int)FS_SUB_WEAPON;
		kPacket << m_iCurCombo;
		kPacket << iSubWeaponInfoIndex;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	++m_iSubWeaponInfoIndex;
}

DWORD ioEvaItem::CreateEffect( ioBaseChar *pOwner, ioHashString& szEffect, D3DXVECTOR3& vPos, D3DXVECTOR3& vScale )
{
	if ( !pOwner || szEffect.IsEmpty() )
		return -1;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )	{	return -1;	}

	ioMapEffect *pMaker = pStage->CreateMapEffect( szEffect, vPos, vScale );
	if ( pMaker )	{	return pMaker->GetUniqueID();	}

	return -1;
}

void ioEvaItem::DestroyEffect( ioBaseChar *pOwner, DWORD& dwEffectID )
{
	if ( !pOwner || dwEffectID == -1 )	{	return;	}

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )	{	return;	}

	pStage->DestroyMapEffect( dwEffectID );
	dwEffectID = -1;
}

void ioEvaItem::UpdateSubWeaponEffect( ioBaseChar *pOwner )
{
	if ( !pOwner )	{	return;	}

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )	{	return;	}

	int nEffectCnt = (int)m_SubWeaponEffectinfoList.size();
	if ( nEffectCnt > 0 && nEffectCnt <= (int)m_SubWeaponEffectinfoList.size() )
	{
		for ( int i=0; i<nEffectCnt; i++ )
		{
			DWORD dwEffectID = m_SubWeaponEffectinfoList[i].m_dwSubWeaponEffectIndex;
			ioMapEffect *pSubWeaponEffect = pStage->FindMapEffect( dwEffectID );
			if ( pSubWeaponEffect )
			{
				D3DXVECTOR3 vNewPos = pOwner->GetMidPositionByRate() + ( pOwner->GetWorldOrientation() * m_SubWeaponInfoList[i].m_vWeaponPos );
				D3DXQUATERNION qtRotate = pOwner->GetWorldOrientation();

				pSubWeaponEffect->SetWorldPosition( vNewPos );
				pSubWeaponEffect->SetWorldOrientation( qtRotate );
			}
		}
	}
}

void ioEvaItem::EndWeaponEffec( ioBaseChar *pOwner )
{	
	if( !pOwner )						{	return;		}

	for( int i = 0; i < (int)m_SubWeaponEffectinfoList.size(); i++ )
	{
		if( m_SubWeaponEffectinfoList[i].m_dwSubWeaponEffectIndex != -1 )
		{
			DestroyEffect( pOwner, m_SubWeaponEffectinfoList[i].m_dwSubWeaponEffectIndex );
		}
	}

	m_SubWeaponEffectinfoList.clear();
	m_iSubWeaponInfoIndex = 0;

}

void ioEvaItem::EndWeaponEffectInfoList( ioPlayStage *pStage )
{
	if ( !pStage )
		return;

	std::vector< SubWeaponEffectInfo >::iterator iter = m_SubWeaponEffectinfoList.begin();
	for( ; iter != m_SubWeaponEffectinfoList.end() ; ++iter )
	{
		SubWeaponEffectInfo subweaponeffectInfo = *iter;
		ioMapEffect* pMapEffect = pStage->FindMapEffect( subweaponeffectInfo.m_dwSubWeaponEffectIndex );
		if ( pMapEffect )
			pMapEffect->EndEffectForce();
	}

	m_SubWeaponEffectinfoList.clear();
	m_iSubWeaponInfoIndex = 0;

}

// 이펙트
void ioEvaItem::CallItemProcess( ioBaseChar* pOwner )
{
	if( !pOwner )						{	return;		}

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
	{
		return;
	}

	// 라운드가 끝나면 웨폰 삭제 2020-11-18
	if( pOwner->GetState() == CS_ENDMOTION )
	{
		EndWeaponEffectInfoList( pStage );	
	}

	//  특수 기능2 : 기본 공격 및 점프 공격 타격 시 서브웨폰 생성
	UpdateSubWeaponEffect( pOwner );
}

void ioEvaItem::OnReleasedByEndGame( ioBaseChar *pOwner )
{
	ioPlayStage *pStage = g_ItemMaker.GetPlayStage();
	EndWeaponEffectInfoList( pStage );	
}

void ioEvaItem::SetSubWeaponAttackState( ioBaseChar *pOwner, int iSubWeaponCurAttackIdx )
{
	if ( !pOwner )
	{
		return;
	}

	// 무조건 타겟이 있을 경우에만 서브 웨폰 발사한다.
	if( m_AimedTarget.IsEmpty() ) return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;
	
	// 이펙트
	if( !COMPARE( iSubWeaponCurAttackIdx, 0, (int)m_SubWeaponEffectinfoList.size() ) )
	{
		return;
	}

	// 서브웨폰 
	if( !COMPARE( iSubWeaponCurAttackIdx, 0, (int)m_SubWeaponInfoList.size() ) )
	{
		return;
	}

	for( iSubWeaponCurAttackIdx = 0; iSubWeaponCurAttackIdx < (int)m_SubWeaponEffectinfoList.size(); iSubWeaponCurAttackIdx++ )
	{
		if( m_SubWeaponEffectinfoList[iSubWeaponCurAttackIdx].m_dwSubWeaponEffectIndex != -1 )
		{
			D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate();
			D3DXQUATERNION qtRot = pOwner->GetWorldOrientation();
			D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVECTOR3 vWeaponPos = vPos + qtRot * m_SubWeaponInfoList[iSubWeaponCurAttackIdx].m_vWeaponPos;	
			D3DXVec3Normalize( &vDir, &vDir );

			WeaponInfoList weaponInfoList = m_SubWeaponInfoList[iSubWeaponCurAttackIdx].m_WeaponInfoList;
			for( int i = 0; i < (int)weaponInfoList.size(); i++ )
			{
				FireTime kFireTime;
				kFireTime.dwStart = FRAMEGETTIME() + weaponInfoList[i].m_dwCallTime;
				kFireTime.iAttributeIdx = weaponInfoList[i].m_iWeaponIdx;
				kFireTime.iResistanceIdx = weaponInfoList[i].m_iResistanceIdx;
				kFireTime.szWoundedAni  = weaponInfoList[i].m_WoundedAnimation;
				kFireTime.dwWoundedAniDuration = weaponInfoList[i].m_dwWoundedDuration;
				kFireTime.bLoopWoundedAni = weaponInfoList[i].m_bWoundedLoopAni;
				kFireTime.eFireTimeType = FTT_BUFF_ATTACK2;
				kFireTime.dwWeaponIdx = pOwner->IncreaseWeaponIndexBase()+1;

				// 바닥에서 조준상태에서 서브웨폰 발사 시 
				ioWeapon *pWeapon = pOwner->ExtendFireExplicit( kFireTime, vWeaponPos, vDir, "" );

				if ( !m_AimedTarget.IsEmpty() )
				{
					ioTraceWeapon *pTraceWeapon = ToTraceWeapon( pWeapon );
					if ( pTraceWeapon )
						pTraceWeapon->SetTraceTarget( m_AimedTarget );
				}
			}

			DestroyEffect( pOwner, m_SubWeaponEffectinfoList[iSubWeaponCurAttackIdx].m_dwSubWeaponEffectIndex );	

			pOwner->PlaySound( m_szSubWeaponAttackSoundName );

			if ( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_APPLY_EXTRA_BUFF_INFO );
				kPacket << pOwner->GetCharName();
				kPacket << GetName();
				kPacket << m_AimedTarget;
				kPacket << iSubWeaponCurAttackIdx;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}

			--m_iSubWeaponInfoIndex;

			break;	
		}
	}
}

bool ioEvaItem::IsSubWeaponAttackState()
{
	// 서브 웨폰이 최대인지 검사
	if( m_iSubWeaponInfoIndex >= m_iMaxSubWeaponCnt )
	{
		return true;
	}

	return false;
}

void ioEvaItem::AimedFireToUp( ioEntityGroup *pGrp,
							   float fWeight,
							   float fTimeRate,
							   DWORD dwPreDelay,
							   bool bHold,
							   bool bQuick)
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
	if( m_bAimJumpAttack )
	{
		szFireUp = m_JumpFireUp;
		szFireCenter = m_JumpFireCenter;
	}
	else if( m_bZeroAimState )
	{
		szFireUp = m_ZeroFireUp;
		szFireCenter = m_ZeroFireCenter;
	}
	else
	{
		szFireUp = m_FireUp;
		szFireCenter = m_FireCenter;
	}

	if( fWeight > 0.0f )
	{
		if( bQuick)
			pGrp->SetActionAni( szFireUp,
								0.0f, 0.f,
								fWeight, FLOAT1/fTimeRate,
								0.0f, dwPreDelay,
								bHold, bAniFx1 );
		else
			pGrp->SetActionAni( szFireUp,
								FLOAT100, fFadeOut,
								fWeight, FLOAT1/fTimeRate,
			0.0f, dwPreDelay,
			bHold, bAniFx1 );
	}

	if( bQuick )
		pGrp->SetActionAni( szFireCenter,
							0.0f, 0.0f,
							1.0f - fWeight, FLOAT1/fTimeRate,
							0.0f, dwPreDelay,
							bHold, bAniFx2 );
	else
		pGrp->SetActionAni( szFireCenter,
							FLOAT100, fFadeOut,
							1.0f - fWeight, FLOAT1/fTimeRate,
							0.0f, dwPreDelay,
							bHold, bAniFx2 );
}

void ioEvaItem::AimedFireToDown( ioEntityGroup *pGrp,
								 float fWeight,
								 float fTimeRate,
								 DWORD dwPreDelay,
								 bool bHold,
								 bool bQuick)
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
	if( m_bAimJumpAttack )
	{
		szFireDown = m_JumpFireDown;
		szFireCenter = m_JumpFireCenter;
	}
	else if( m_bZeroAimState )
	{
		szFireDown = m_ZeroFireDown;
		szFireCenter = m_ZeroFireCenter;
	}
	else
	{
		szFireDown = m_FireDown;
		szFireCenter = m_FireCenter;
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

void ioEvaItem::TeleportMoveStartDash( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( m_TeleportMoveStartAction.IsEmpty() ) return;
	if( m_FireState != FS_NONE ) 
	{
		return;
	}

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwTeleportEndDelayTime > dwCurTime )
	{
		return;
	} 


	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp ) return;

	int iAniID = pGrp->GetAnimationIdx( m_TeleportMoveStartAction );
	if( iAniID == -1 )
	{
		return;
	}

	m_dwMotionEndTime = 0;

	float fTimeRate = pOwner->GetRunDashAniRate();
	fTimeRate *= m_fDashSpeedRate;

	pGrp->ClearAllLoopAniExceptOne( iAniID, FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, fTimeRate );

	pOwner->InitExtendAttackTagInfo();

	DWORD dwDuration = pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	m_dwMotionEndTime = dwCurTime + dwDuration;

	m_SpecialState = SS_TELEPORT_START;
	pOwner->SetState( CS_EVA_SPECIAL );
	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)FS_TELEPORT;
		kPacket << m_iCurCombo;
		kPacket << m_AimedTarget;
		kPacket << m_bAimedBall;
		kPacket << m_bZeroAimState;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	return;
}

void ioEvaItem::SetOtherAction( ioBaseChar *pOwner )
{
	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_TeleportJumpAttribute.m_AttackAnimation );
	if ( iAniID == -1 )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwPreDelay = m_TeleportJumpAttribute.m_dwPreDelay;
	float fTimeRate  = m_TeleportJumpAttribute.m_fAttackAniRate;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );	

	pOwner->RefreshFireTimeList( iAniID, m_TeleportJumpAttribute.m_vWeaponInfoList, FTT_NORMAL_ATTACK, fTimeRate, dwPreDelay, true );

	m_dwMotionEndTime = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + dwPreDelay;
	m_dwKeyReserveTime = dwCurTime + pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate + dwPreDelay;

	pOwner->SetSKillEndJumpState( m_fTeleportJumpPower, false, false, false );
}

void ioEvaItem::CheckTeleportState( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	m_SpecialState = SS_TELEPORT_END_ACTION;

	pOwner->ChangeDirectionByInputDir( true );

	ioBaseChar *pTarget = pOwner->GetBaseChar( m_AimedTarget );
	if( pTarget )
	{
		D3DXVECTOR3 vOffset = pTarget->GetMoveDir() * m_fTeleportOffset;

		m_vTargetPos = pTarget->GetWorldPosition();
		m_vTeleportPos = m_vTargetPos - vOffset;
	}

	float fMapHeight = pStage->GetMapHeightOnlyTerrain( m_vTeleportPos.x, m_vTeleportPos.z );
	if ( m_vTeleportPos.y - fMapHeight >= m_fJumpHeightGap )
	{
	}
	else 
	{
		if( fMapHeight > m_vTeleportPos.y )
			m_vTeleportPos.y = fMapHeight;
	}

	D3DXVECTOR3 vPrePos = pOwner->GetWorldPosition();

	pOwner->SetWorldPosition( m_vTeleportPos );
	pOwner->SetTargetRotToTargetPos( m_vTargetPos, true );

	// 이펙트
	pStage->CreateMapEffect( m_szTeleportStartEffect, vPrePos, ioMath::UNIT_ALL );
	pStage->CreateMapEffect( m_szTeleportEndEffect, m_vTeleportPos, ioMath::UNIT_ALL );
}

// 서브 웨폰 관전 OR 입장 관련 함수 2020-11-24
bool ioEvaItem::IsEquipObject( ioBaseChar *pOwner )
{
	if( !pOwner )	{	return false;	}

	int nEffectCnt = (int)m_SubWeaponEffectinfoList.size();
	if( nEffectCnt > 0 )
	{
		return true;
	}

	return false;
}

// 2020-11-24
void ioEvaItem::FillEvaSubWeaponInfo( ioBaseChar *pOwner, SP2Packet &rkPacket )
{
	// 현재 본인 
	if( !pOwner )	return;

	int nEffectCnt = (int)m_SubWeaponEffectinfoList.size();
	rkPacket << nEffectCnt;

	for (int i = 0; i < nEffectCnt ; i++)
	{
		rkPacket << m_SubWeaponEffectinfoList[i].m_dwSubWeaponEffectIndex;
	}
}

void ioEvaItem::SetEvaSubWeaponInfoList( ioBaseChar *pOwner, SP2Packet &rkPacket )
{	
	if( !pOwner )	return;

	m_SubWeaponEffectinfoList.clear();
	int iSubWeaponInfoIndex = 0;
	int nEffectCnt = 0;
	rkPacket >> nEffectCnt;
	for ( iSubWeaponInfoIndex=0; iSubWeaponInfoIndex<nEffectCnt; iSubWeaponInfoIndex++ )
	{
		SubWeaponEffectInfo subweaponeffectinfo;
		rkPacket >> subweaponeffectinfo.m_dwSubWeaponEffectIndex;
		m_SubWeaponEffectinfoList.push_back( subweaponeffectinfo );
		if( subweaponeffectinfo.m_dwSubWeaponEffectIndex != -1 )
		{
			CreateSubWeaponEffect( pOwner, iSubWeaponInfoIndex );
		}		
	}

	UpdateSubWeaponEffect( pOwner );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEvaSpecialState::ioEvaSpecialState()
{
}

ioEvaSpecialState::~ioEvaSpecialState()
{
}

void ioEvaSpecialState::Initialize( ioBaseChar* pOwner, CharState eCurrState )
{
	ioSpecialStateBase::Initialize( pOwner, eCurrState );
}

// ioBaseChar::SetState 함수에서 상태 체크, 이전 특별 상태 삭제할때 호출
void ioEvaSpecialState::Finalize( ioBaseChar* pOwner, CharState eCurrState, CharState eNewState )
{
	ioSpecialStateBase::Finalize( pOwner, eCurrState, eNewState );

	ioEvaItem *pEvaItem = ToEvaItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pEvaItem )
	{
		pEvaItem->ClearAttackState( pOwner );
	}
}

// ioBaseChar::ProcessState() 함수에서 디폴트로 함수 계속 호출한다.
void ioEvaSpecialState::CheckSpecialState( ioBaseChar* pOwner )
{
	ioEvaItem *pEvaItem = ToEvaItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( pEvaItem )
	{
		pEvaItem->ProcessSpecialState( pOwner );
	}
	else
	{
		pOwner->SetState( CS_DELAY );
		pOwner->SetFallState( true );
		return;
	}
}

void ioEvaSpecialState::CheckFallStateChange( ioBaseChar* pOwner, float fMinFallHeightRate, float fBottomHeight, bool bBottomMaxCheck, float fMaxBottomHeight )
{
	return;
}

// 특별 상태가 지속되는지 검사해서 아니면 false로 리턴하여 ioBaseChar::ProcessMove()를 실행
bool ioEvaSpecialState::ProcessMove( ioBaseChar* pOwner, float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime )
{
	ioEvaItem *pEvaItem = ToEvaItem( pOwner->GetPriorityItem( SPT_ATTACK ) );
	if( !pEvaItem )
	{
		return false;
	}

	// 일반 공격 중 캔슬 대시
	if( !pEvaItem->IsConditionalSpecialDashReserve( pOwner ) )
	{
		return false;
	}

	return true;
}