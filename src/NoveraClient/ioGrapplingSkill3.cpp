

#include "stdafx.h"

#include "ioNpcChar.h"
#include "ioPlayStage.h"

#include "ioGrapplingSkill3.h"

#include "WeaponDefine.h"

ioGrapplingSkill3::ioGrapplingSkill3()
{
	ClearData();
}

ioGrapplingSkill3::ioGrapplingSkill3( const ioGrapplingSkill3 &rhs )
: ioAttackSkill( rhs ),
 m_SkillLoopMotion( rhs.m_SkillLoopMotion ),
 m_fSkillLoopMotionRate( rhs.m_fSkillLoopMotionRate ),
 m_dwSkillLoopDuration( rhs.m_dwSkillLoopDuration ),
 m_fTargetLoopGapRange( rhs.m_fTargetLoopGapRange ),
 m_fCharHeightRate( rhs.m_fCharHeightRate ),
 m_fCharJumpPower( rhs.m_fCharJumpPower ),
 m_vComboList( rhs.m_vComboList ),
 m_vComboWoundList( rhs.m_vComboWoundList ),
 m_dwSkillProtectTime( rhs.m_dwSkillProtectTime ),
 m_dwEnableComboTime( rhs.m_dwEnableComboTime ),
 m_bUseEndOtherAttack( rhs.m_bUseEndOtherAttack ),
 m_EndOtherAttack( rhs.m_EndOtherAttack ),
 m_EndOtherWoundAni( rhs.m_EndOtherWoundAni ),
 m_bEnableLoopMove( rhs.m_bEnableLoopMove ),
 m_fRunSpeed( rhs.m_fRunSpeed ),
 m_dwRotateTime( rhs.m_dwRotateTime ),
 m_AutoComboType( rhs.m_AutoComboType ),
 m_bUseTelelportTarget( rhs.m_bUseTelelportTarget ),
 m_TeleportTargetColType( rhs.m_TeleportTargetColType ),
 m_TeleportTargetWoundType( rhs.m_TeleportTargetWoundType ),
 m_fTeleportTargetRange( rhs.m_fTeleportTargetRange ),
 m_fTeleportTargetAngle( rhs.m_fTeleportTargetAngle ),
 m_fTeleportTargetOffSet( rhs.m_fTeleportTargetOffSet ),
 m_fTeleportTargetHeightOffSet( rhs.m_fTeleportTargetHeightOffSet ),
 m_fTeleportTargetUpHeight( rhs.m_fTeleportTargetUpHeight ),
 m_fTeleportTargetUnderHeight( rhs.m_fTeleportTargetUnderHeight ),
 m_fTeleportTargetWidth( rhs.m_fTeleportTargetWidth ),
 m_fTeleportOffSet( rhs.m_fTeleportOffSet ),

 m_EndOtherAttackType( rhs.m_EndOtherAttackType ),
 m_SkillLoopType( rhs.m_SkillLoopType ),
 m_dwSkillSwingLoopDuration( rhs.m_dwSkillSwingLoopDuration ),
 m_dwEnableSwingTime( rhs.m_dwEnableSwingTime ),
 m_dwSwingLoopRotateTime( rhs.m_dwSwingLoopRotateTime ),
 m_bReverseSwingLoopRotate( rhs.m_bReverseSwingLoopRotate ),
 m_bEnableSwingLoopMove( rhs.m_bEnableSwingLoopMove ),
 m_SkillSwingEndMotion( rhs.m_SkillSwingEndMotion ),
 m_fSkillSwingEndMotionRate( rhs.m_fSkillSwingEndMotionRate ),
 m_SkillSwingedMotion( rhs.m_SkillSwingedMotion ),
 m_fSkillSwingedMotionRate( rhs.m_fSkillSwingedMotionRate ),
 m_SkillSwingGrapplingInfo( rhs.m_SkillSwingGrapplingInfo ),
 m_fTargetSwingLoopGapRange( rhs.m_fTargetSwingLoopGapRange ),
 m_fCharSwingHeightRate( rhs.m_fCharSwingHeightRate ),
 m_RotateWeaponInfoList( rhs.m_RotateWeaponInfoList ),
 m_SkillEndTargetKillingBuffList( rhs.m_SkillEndTargetKillingBuffList )
{
	ClearData();
}

ioGrapplingSkill3::~ioGrapplingSkill3()
{
}

void ioGrapplingSkill3::ClearData()
{
	m_SkillState = SS_NONE;

	m_fCurChargeRate = 0.0f;
	
	m_dwSkillLoopStartTime	= 0;
	m_dwSkillLoopEndTime	= 0;

	m_GrapplingTargetName.Clear();

	m_bSetHandMesh	= false;
	m_bUsedEndOtherAttack = false;

	m_iCurLinearCombo = 0;
	m_dwNextComboTime = 0;
	m_dwCurSkillProtectTime = 0;

	m_bTargetRot = false;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	m_TeleportTargetName.Clear();
	m_vTeleportTargetPos = ioMath::VEC3_ZERO;

	m_vCurMoveDir = ioMath::VEC3_ZERO;

	m_iRotateWeaponCurCnt = 0;
	m_vecRotateWeaponIndex.clear();

	m_dwSkillSwingLoopStartTime = 0;
}

void ioGrapplingSkill3::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	// 잡고 있는 동작
	rkLoader.LoadString_e( "skill_loop_motion", "", szBuf, MAX_PATH );
	m_SkillLoopMotion = szBuf;

	m_fSkillLoopMotionRate	= rkLoader.LoadFloat_e( "skill_loop_motion_rate", FLOAT1 );
	m_dwSkillLoopDuration	= rkLoader.LoadInt_e( "skill_loop_duration", 0 );

	m_dwEnableComboTime = rkLoader.LoadInt_e( "skill_combo_enable_time", 0 );

	m_fTargetLoopGapRange	= rkLoader.LoadFloat_e( "skill_target_loop_gap_range", 0.0f );

	m_fCharHeightRate	= rkLoader.LoadFloat_e( "char_height_rate", FLOAT05 );
	m_fCharJumpPower	= rkLoader.LoadFloat_e( "skill_jump_power", 0.0f );

	// 
	m_AutoComboType = (AutoComboType)rkLoader.LoadInt_e( "auto_combo_action", ACT_NONE );

	int iComboCnt = rkLoader.LoadInt_e( "linear_combo_cnt", 0 );
	m_vComboList.clear();
	m_vComboWoundList.clear();

	if( iComboCnt > 0 )
	{
		m_vComboList.reserve( iComboCnt );
		m_vComboWoundList.reserve( iComboCnt );

		for( int i=0; i < iComboCnt; ++i )
		{
			AttackAttribute kComboAttack;

			wsprintf_e( szKey, "linear_combo%d", i+1 );
			LoadAttackAttribute( szKey, kComboAttack, rkLoader );

			m_vComboList.push_back( kComboAttack );

			wsprintf_e( szKey, "linear_combo_wound_ani%d", i+1 );
			rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
			m_vComboWoundList.push_back( szBuf );
		}
	}

	m_dwSkillProtectTime = (DWORD)rkLoader.LoadInt_e( "skill_protect_time", 0 );

	//
	m_bUseEndOtherAttack = rkLoader.LoadBool_e( "use_end_other_attack", false );

	LoadAttackAttribute_e( "end_other_attack", m_EndOtherAttack, rkLoader );

	rkLoader.LoadString_e( "end_other_wound_ani", "", szBuf, MAX_PATH );
	m_EndOtherWoundAni = szBuf;

	//
	m_bEnableLoopMove = rkLoader.LoadBool_e( "enable_loop_move", false );

	m_fRunSpeed = rkLoader.LoadFloat_e( "move_speed", 0.0f );
	m_dwRotateTime = (DWORD)rkLoader.LoadInt_e( "move_rotate_speed", 0 );

	//
	int iBuffCnt = rkLoader.LoadInt_e( "swing_cancel_buff_cnt", 0 );
	for( int i=0; i < iBuffCnt; ++i )
	{
		wsprintf_e( szKey, "swing_cancel_buff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

		m_SkillGrapplingInfo.m_SwingCancelBuff.push_back( szBuf );
	}

	iBuffCnt = rkLoader.LoadInt_e( "swing_buff_cnt", 0 );
	for( i=0; i < iBuffCnt; ++i )
	{
		wsprintf_e( szKey, "swing_buff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

		m_SkillGrapplingInfo.m_SwingBuff.push_back( szBuf );
	}

	iBuffCnt = rkLoader.LoadInt_e( "swing_wound_buff_cnt", 0 );
	for( i=0; i < iBuffCnt; ++i )
	{
		wsprintf_e( szKey, "swing_wound_buff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

		m_SkillGrapplingInfo.m_SwingWoundBuff.push_back( szBuf );
	}
	

	//
	m_bUseTelelportTarget = rkLoader.LoadBool_e( "use_teleport_target", false );
	m_TeleportTargetColType = (TargetColType)rkLoader.LoadInt_e( "teleport_target_collision_type", TCT_SPHERE );
	m_TeleportTargetWoundType = (TargetWoundType)rkLoader.LoadInt_e( "teleport_target_wound_type", TWT_ALL );

	m_fTeleportTargetRange = rkLoader.LoadFloat_e( "teleport_target_range", 0.0f );
	m_fTeleportTargetAngle = rkLoader.LoadFloat_e( "teleport_target_angle", 0.0f );
	m_fTeleportTargetOffSet = rkLoader.LoadFloat_e( "teleport_target_offset", 0.0f );
	m_fTeleportTargetHeightOffSet = rkLoader.LoadFloat_e( "teleport_target_height_offset", 0.0f );

	m_fTeleportTargetUpHeight = rkLoader.LoadFloat_e( "teleport_target_up_height", 0.0f );
	m_fTeleportTargetUpHeight = max( FLOAT1, m_fTeleportTargetUpHeight );
	m_fTeleportTargetUnderHeight = rkLoader.LoadFloat_e( "teleport_target_under_height", 0.0f );
	m_fTeleportTargetUnderHeight = max( FLOAT1, m_fTeleportTargetUnderHeight );
	m_fTeleportTargetWidth = rkLoader.LoadFloat_e( "teleport_target_width", 0.0f );

	m_fTeleportOffSet = rkLoader.LoadFloat_e( "teleport_offset", 0.0f );


	//추가 yong jun
	m_EndOtherAttackType = (EndOtherAttackType)rkLoader.LoadInt_e( "end_other_attack_type", EOAT_LINEAR );
	m_dwSkillSwingLoopDuration = (DWORD)rkLoader.LoadInt_e( "swing_loop_duration", 0);
	m_dwEnableSwingTime = (DWORD)rkLoader.LoadInt_e( "enable_swing_time", 0 );
	m_dwSwingLoopRotateTime = (DWORD)rkLoader.LoadInt_e( "swing_loop_rotate_speed", 0 );
	m_bReverseSwingLoopRotate = rkLoader.LoadBool_e("reverse_swing_rotate", false );

	rkLoader.LoadString_e( "swing_end_motion", "", szBuf, MAX_PATH );
	m_SkillSwingEndMotion = szBuf;
	m_fSkillSwingEndMotionRate = rkLoader.LoadFloat_e( "swing_end_motion_rate", 0.0f );

	m_SkillSwingGrapplingInfo.Init();
	m_SkillSwingGrapplingInfo.m_bReverseTargetRot = rkLoader.LoadBool_e( "target_swing_reverse_rotate", false );
	m_SkillSwingGrapplingInfo.m_GrapplingColType = (GrapplingColType)rkLoader.LoadInt_e( "swing_grappling_enable_col", GCT_ALL );

	rkLoader.LoadString_e( "swing_target_grappling_motion", "", szBuf, MAX_PATH );
	m_SkillSwingGrapplingInfo.m_GrapplingLoopMotion = szBuf;
	m_SkillSwingGrapplingInfo.m_fGrapplingLoopMotionRate = rkLoader.LoadFloat_e( "swing_target_grappling_motion_rate", FLOAT1 );

	iBuffCnt = rkLoader.LoadInt_e( "swing_loop_cancel_buff_cnt", 0 );
	for( int i=0; i < iBuffCnt; ++i )
	{
		wsprintf_e( szKey, "swing_loop_cancel_buff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

		m_SkillSwingGrapplingInfo.m_SwingCancelBuff.push_back( szBuf );
	}

	iBuffCnt = rkLoader.LoadInt_e( "swing_loop_buff_cnt", 0 );
	for( i=0; i < iBuffCnt; ++i )
	{
		wsprintf_e( szKey, "swing_loop_buff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

		m_SkillSwingGrapplingInfo.m_SwingBuff.push_back( szBuf );
	}

	iBuffCnt = rkLoader.LoadInt_e( "swing_loop_wound_buff_cnt", 0 );
	for( i=0; i < iBuffCnt; ++i )
	{
		wsprintf_e( szKey, "swing_loop_wound_buff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

		m_SkillSwingGrapplingInfo.m_SwingWoundBuff.push_back( szBuf );
	}

	m_fTargetSwingLoopGapRange	= rkLoader.LoadFloat_e( "skill_target_swing_loop_gap_range", 0.0f );

	m_fCharSwingHeightRate	= rkLoader.LoadFloat_e( "char_swing_height_rate", FLOAT05 );

	
	
	int iMaxRotateWeaponCnt = rkLoader.LoadInt_e( "max_rotate_weapon_cnt", 1 );
	m_RotateWeaponInfoList.clear();
	m_RotateWeaponInfoList.reserve( iMaxRotateWeaponCnt );
	for( int i=0; i < iMaxRotateWeaponCnt; ++i )
	{
		BulletInfo kInfo;

		wsprintf_e( szKey, "rotate_weapon_angle%d", i+1 );
		kInfo.m_fYawAngle = rkLoader.LoadFloat( szKey, 0.0f );

		wsprintf_e( szKey, "rotate_weapon_range_offset%d", i+1 );
		kInfo.m_fRangeOffSet = rkLoader.LoadFloat( szKey, 0.0f );

		wsprintf_e( szKey, "rotate_weapon_height_offset%d", i+1 );
		kInfo.m_fHeightOffSet = rkLoader.LoadFloat( szKey, 0.0f );

		m_RotateWeaponInfoList.push_back( kInfo );
	}

	m_bEnableSwingLoopMove = rkLoader.LoadBool_e( "enable_swing_loop_move", false );

	m_SkillLoopType = (SkillLoopType)rkLoader.LoadInt_e("skill_loop_type", SLT_STOP );
	
	rkLoader.LoadString_e( "skill_swinged_motion", "", szBuf, MAX_PATH );
	m_SkillSwingedMotion = szBuf;
	m_fSkillSwingedMotionRate = rkLoader.LoadFloat_e( "skill_swinged_motion_rate", FLOAT1 );

	m_SkillEndTargetKillingBuffList.clear();
	int iCnt = rkLoader.LoadInt_e( "target_killing_buff_cnt", 0 );
	for( int i=0; i < iCnt; ++i )
	{
		wsprintf_e( szKey, "target_killing_buff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		ioHashString szBuffName = szBuf;

		if( !szBuffName.IsEmpty() )
		{
			int iType = g_BuffMaker.GetBuffType( szBuffName );
			if( iType != BT_NONE )
			{
				ioBuffInfo kBuffInfo( szBuffName, iType );
				m_SkillEndTargetKillingBuffList.push_back( kBuffInfo );
			}
		}
	}
}

ioSkill* ioGrapplingSkill3::Clone()
{
	return new ioGrapplingSkill3( *this );
}

bool ioGrapplingSkill3::OnSkillStart( ioBaseChar *pChar )
{
	if( !pChar )
		return false;

	if( !HasSkillMotion() )
		return false;

	ClearData();

	if( !ioAttackSkill::OnSkillStart( pChar ) )
		return false;

	pChar->SetSkillProtection();
	
	m_bChangeDirRot = false;
	m_bReduceGauge = false;
	m_dwAttackEndTime = 0;

	InitExtraAniJump();

	m_dwAniRotateTime = 0;
	m_fAniRotateAngle = 0.0f;

	m_iCurAniRotate = 0;

	m_dwEnableSwitchTime = 0;
	m_dwEnableSemiSwitchTime = 0;

	m_SkillSoundID = 0;

	m_bChangeGrapplingTargetPos = true;

	if( m_dwSwitchSkill > 0 )
		m_dwEnableSwitchTime = FRAMEGETTIME() + m_dwSwitchSkill;
	if( m_dwSemiSwitchSkill > 0 )
		m_dwEnableSemiSwitchTime = FRAMEGETTIME() + m_dwSemiSwitchSkill;

	m_dwSkillStartTime = FRAMEGETTIME();

	ioPlayStage *pStage = g_SkillMaker.GetPlayStage();

	// ExtraAttackForce
	m_fExtraAttackMoveForce = 0.0f;
	m_fExtraAttackMoveFriction = 0.0f;

	CheckChangeDirRot( pChar );
	CheckInitMoveSpeed( pChar );

	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();
	if( !StartPreDelayAnimation( pChar ) )	// Delay Animation이 없다
	{
		SetActionSkill( pChar );
	}
	else
	{
		DWORD dwTrackingTime = 0;
		ioEntityGroup *pGrp = pChar->GetGroup();
		if( pGrp )
		{
			ioHashString szPreDelay = GetPreDelayAniName();

			int iAniID = pGrp->GetAnimationIdx( szPreDelay );
			if( iAniID != -1 )
				dwTrackingTime = pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" );
		}

		DWORD dwPreDelay = GetPreDelayTime();

		if( dwTrackingTime > 0 )
			dwTrackingTime += FRAMEGETTIME();
		else
			dwTrackingTime = FRAMEGETTIME() + dwPreDelay;

		if( IsUseAutoTargetValue() )
		{
			pChar->SetAutoTarget( ATT_SKILL );
			pChar->SetAutoTargetTrackingEndTime( dwTrackingTime );
		}
	}

	CheckSkillDesc( pChar );

	if( m_bInitJumpPower )
	{
		pChar->SetCurJumpPowerAmt( 0.0f );
		pChar->SetGravityAmt( 0.0f );
	}

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	m_dwCurSkillProtectTime = 0;
	if( m_dwSkillProtectTime > 0 )
		m_dwCurSkillProtectTime = FRAMEGETTIME() + m_dwSkillProtectTime;

	SetSkillNameChatBubble( pChar );

	m_dwSkillSwingLoopStartTime = 0;
	if( pChar )
		m_vCurMoveDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

	m_iRotateWeaponCurCnt = 0;
	m_vecRotateWeaponIndex.clear();
	
	return true;
}

void ioGrapplingSkill3::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_SkillState )
	{
	case SS_NONE:
		{
			DWORD dwPreDelay = GetPreDelayTime();
			if( m_dwPreDelayStartTime + dwPreDelay < dwCurTime )
			{
				m_dwPreDelayStartTime = 0;

				SetActionSkill( pChar );
				EndPreDelayEffect( pChar );
			}
		}
		break;
	case SS_ACTION:
		break;
	case SS_AUTO_COMBO:
		{
			// Target 유효 체크
			ioBaseChar *pTarget = pStage->GetBaseChar( m_GrapplingTargetName );
			if( pTarget )
			{
				if( pTarget->GetState() != CS_GRAPPLING_WOUNDED_SKILL )
				{
					SetSkillEndState( pChar, true );
					return;
				}
			}
			else
			{
				SetSkillEndState( pChar, true );
				return;
			}

			SetNextLinearCombo( pChar, false, false );
			/*
			if( m_dwMotionEndTime < dwCurTime )
			{
				SetNextLinearCombo( pChar, false );
			}
			*/
		}
		break;
	case SS_LOOP:
		{
			// Target 유효 체크
			ioBaseChar *pTarget = pStage->GetBaseChar( m_GrapplingTargetName );
			if( pTarget )
			{
				if( m_dwSkillLoopStartTime+g_App.GetGrapplingCheckDelay() < dwCurTime &&
					pTarget->GetState() != CS_GRAPPLING_WOUNDED_SKILL )
				{
					SetSkillEndState( pChar, true );
					return;
				}
			}
			else
			{
				SetSkillEndState( pChar, true );
				return;
			}

			if( m_AutoComboType == ACT_NONE && pChar->IsNeedProcess() && m_dwSkillLoopStartTime + m_dwEnableComboTime < dwCurTime )
			{
				if( pChar->IsAttackKey() )
				{
					SetNextLinearCombo( pChar, false );
					return;
				}
				else if( m_bUseEndOtherAttack && pChar->IsDefenseKey() )
				{				
					m_bUsedEndOtherAttack = true;
					SetNextLinearCombo( pChar, true );
					return;
				}
			}

			if( m_dwSkillLoopEndTime < dwCurTime )
			{
				if( m_AutoComboType != ACT_NONE )
					SetSkillAutoComboState( pChar );
				else
					SetSkillEndState( pChar, false );

				return;
			}

			if( m_bEnableLoopMove )
			{
				CheckKeyInput( pChar );
				ProcessRotate( pChar );

				if( m_CurDirKey != ioUserKeyInput::DKI_NONE )
					pChar->SetCurMoveSpeed( m_fRunSpeed );
			}
		}
		break;
	case SS_COMBO:
		{
			if( m_dwEnableReserveTime > 0 && m_dwEnableReserveTime < dwCurTime )
			{
				ioBaseChar *pTarget = pStage->GetBaseChar( m_GrapplingTargetName );
				if( pTarget )
				{
					if( pTarget->GetState() != CS_GRAPPLING_WOUNDED_SKILL )
					{
						SetSkillEndState( pChar, true );
						return;
					}
					else if( pTarget->GetGrapplingAttacker() != pChar->GetCharName() )
					{
						SetSkillEndState( pChar, true );
						return;
					}
				}
				else
				{
					SetSkillEndState( pChar, true );
					return;
				}
			}

			CheckLinearCombo( pChar );

			if( m_bEnableLoopMove )
			{
				CheckKeyInput( pChar );
				ProcessRotate( pChar );
			}

			CheckAirJump( pChar );
		}
		break;
	case SS_SWINGLOOP:
		{
			// Target 유효 체크
			ioBaseChar *pTarget = pStage->GetBaseChar( m_GrapplingTargetName );
			if( pTarget )
			{
				if( m_dwSkillSwingLoopStartTime+g_App.GetGrapplingCheckDelay() < dwCurTime &&
					pTarget->GetState() != CS_GRAPPLING_WOUNDED_SKILL )
				{
					SetSwingSkillEndState( pChar, true );
					return;
				}
			}
			else
			{
				SetSwingSkillEndState( pChar, true );
				return;
			}

			// 시간만료 체크
			if( pChar->IsNeedProcess() && m_dwSkillSwingLoopStartTime + m_dwSkillSwingLoopDuration < dwCurTime )
			{
				pChar->SetTargetRotToRotate( pChar->GetTargetRot(), true, false );
				SetSwingSkillEndState( pChar, true );
				return;
			}

			// 액션키 체크
			if( pChar->IsNeedProcess() && m_dwSkillSwingLoopStartTime+m_dwEnableSwingTime < dwCurTime )
			{
				int iSlot = GetOwnerItem()->GetType() - 1;
				if( pChar->GetSkillKeyInput() == iSlot || pChar->IsDefenseKey() || pChar->IsAttackKey() )
				{
					pChar->SetTargetRotToRotate( pChar->GetTargetRot(), true, false );
					SetSwingSkillEndState( pChar, true );
					return;
				}
			}
			if(m_bEnableSwingLoopMove)
				CheckSwingMove( pChar );
			ProcessSwingLoopRotate( pChar );
		}

		
		break;
	case SS_END:
		break;
	case SS_SWINGEND:
		break;
	}
}

void ioGrapplingSkill3::OnSkillEnd( ioBaseChar *pChar )
{
	ioAttackSkill::OnSkillEnd( pChar );

	ioPlayStage *pStage = g_SkillMaker.GetPlayStage();
	if ( !pStage )
		return;

	if ( !m_GrapplingTargetName.IsEmpty() )
	{
		ioBaseChar *pTarget = pStage->GetBaseChar( m_GrapplingTargetName );
		if( pTarget )
		{
			int iCnt = m_SkillEndTargetKillingBuffList.size();
			for(int i=0; i < iCnt; ++i )
				pTarget->RemoveBuff( m_SkillEndTargetKillingBuffList[i].m_BuffName );
		}
	}
	ClearData();

	// 기본무기 장착
	if( m_bUseItemMesh )
		pChar->ShowWeaponItemMesh( true );

	// 스킬 중간 동작에 사용된 메쉬들 제거
	if( !m_HandMesh.IsEmpty() )
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );

	m_vCurMoveDir = ioMath::VEC3_ZERO;

	m_iRotateWeaponCurCnt = 0;

	m_vecRotateWeaponIndex.clear();
}

void ioGrapplingSkill3::AttackSkillFire( ioBaseChar *pChar )
{
	if( !pChar ) return;
	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( GetAttribute().m_AttackAnimation );
	if( iAniID == -1 )	return;

	pChar->AttachEffect( m_SkillEffectName );

	DWORD dwCurTime = FRAMEGETTIME();

	float fTimeRate  = GetAttackTimeRate();
	DWORD dwPreDelay = GetAttribute().m_dwPreDelay;

	pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex,
										iAniID,
										GetAttribute().m_vWeaponInfoList,
										FTT_SKILL_ATTACK,
										fTimeRate,
										dwPreDelay );

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	m_dwMotionStartTime = m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwMotionEndTime += dwPreDelay + GetAttribute().m_dwEndDelay;

	DWORD dwFireStartTime;
	dwFireStartTime = m_dwFireMotionEndTime = dwCurTime;

	dwFireStartTime += dwPreDelay;
	dwFireStartTime += pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;

	//
	SetExtraAniJump( pChar, iAniID, fTimeRate );

	//
	m_dwAniRotateTime = 0;
	m_fAniRotateAngle = 0.0f;
	m_vAniRotateInfoList.clear();

	AniEventConstPtrList vAniRotateList;
	pGrp->GetAnimationEventConstPtrList( iAniID, "ani_rotate", vAniRotateList );

	AniEventConstPtrList::iterator iter_r = vAniRotateList.begin();
	while( iter_r != vAniRotateList.end() )
	{
		AniRotateInfo kInfo;

		kInfo.m_dwTime = (*iter_r)->fEventTime * fTimeRate;

		if( kInfo.m_dwTime > 0 )
			kInfo.m_dwTime += dwCurTime;
		else
			kInfo.m_dwTime = 0;

		if( !(*iter_r)->szSubInfo.IsEmpty() )
		{
			const StringVector &vParam = ioStringConverter::Split( (*iter_r)->szSubInfo.c_str(), " " );
			if( vParam.size() == 1 )
			{
				kInfo.m_fAngle = ioStringConverter::ParseFloat( vParam[0].c_str() );
			}
		}

		m_vAniRotateInfoList.push_back(kInfo);

		++iter_r;
	}

	if( !m_vAniRotateInfoList.empty() )
	{
		m_iCurAniRotate = 0;
		m_dwAniRotateTime = m_vAniRotateInfoList[m_iCurAniRotate].m_dwTime;
		m_fAniRotateAngle = m_vAniRotateInfoList[m_iCurAniRotate].m_fAngle;
	}
	//

	// Col Skip
	pChar->CheckCharColSkipSkillTime( iAniID, fTimeRate, dwPreDelay );

	m_dwFireMotionEndTime += dwPreDelay;
	m_dwFireMotionEndTime += pGrp->GetAnimationEventTime_e( iAniID, "firemotion_e" ) * fTimeRate;

	m_dwEnableReserveTime = dwCurTime;
	m_dwEnableReserveTime += pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;

	if( dwFireStartTime >= m_dwFireMotionEndTime )
	{
		m_dwFireMotionEndTime = m_dwMotionEndTime;
	}

	if( GetAttribute().m_AttackEndAnimation.IsEmpty() )
	{
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
	}
	else
	{
		pGrp->SetActionAni( iAniID, FLOAT100, 0.0f, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

		float fWaitTime = pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + dwPreDelay;
		float fEndTimeRate = GetAttribute().m_fAttackEndAniRate;

		pGrp->SetActionAni( GetAttribute().m_AttackEndAnimation,
							0.0f, FLOAT100,
							1.0f, FLOAT1/fEndTimeRate,
							fWaitTime, 0.0f, true );

		int iEndAniID = pGrp->GetAnimationIdx( GetAttribute().m_AttackEndAnimation );
		m_dwMotionEndTime += pGrp->GetAnimationFullTime( iEndAniID ) * fEndTimeRate;
	}

	// 예외처리
	m_dwAttackEndTime = 0;

	pChar->SetReservedSliding( GetAttribute().m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	m_SkillState = SS_ACTION;
	
	m_GrapplingTargetName.Clear();

	// 사용 아이템 처리
	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}

	if( !m_bReduceGauge )
	{
		pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
		m_bReduceGauge = true;
	}
}

void ioGrapplingSkill3::SetSkillLoopState( ioBaseChar *pChar )
{
	if( !pChar ) return;
	if( m_SkillState != SS_ACTION )
		return;
	
	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp ) return;

	m_dwSkillLoopStartTime = FRAMEGETTIME();
	m_SkillState = SS_LOOP;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	float fAniRate = FLOAT1;
	if( m_fSkillLoopMotionRate > 0.0f )
		fAniRate = m_fSkillLoopMotionRate;

	int iAniID = pGrp->GetAnimationIdx( m_SkillLoopMotion );
	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fAniRate );

	if( m_fCharJumpPower > 0.0f )
		pChar->SetJumpPower( m_fCharJumpPower );

	pChar->SetForcePowerAmt( 0.0f );

	m_dwSkillLoopEndTime = m_dwSkillLoopStartTime;
	m_dwSkillLoopEndTime += m_dwSkillLoopDuration;
}

void ioGrapplingSkill3::SetSkillAutoComboState( ioBaseChar *pChar )
{
	if( m_SkillState != SS_ACTION && m_SkillState != SS_LOOP )
		return;

	pChar->SetForcePowerAmt( 0.0f );
	m_SkillState = SS_AUTO_COMBO;
	return;
}

void ioGrapplingSkill3::SetSkillEndState( ioBaseChar *pChar, bool bSendNet )
{
	if( !pChar ) return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	m_dwMotionStartTime = m_dwMotionEndTime = dwCurTime;
	m_dwFireMotionEndTime = dwCurTime;
	m_dwAttackEndTime = dwCurTime;
	m_dwMotionEndTime += 100;
	m_dwEnableReserveTime = 0;

	m_SkillState = SS_END;

	if( bSendNet && pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SSC_END;
		kPacket << pChar->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioGrapplingSkill3::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	if( pWeapon && pWeapon->CheckGrapplingState() == GPT_WRESTLING && !m_GrapplingTargetName.IsEmpty() )
		pWeapon->SetGrapplingTargetName( m_GrapplingTargetName );
	else if( pWeapon && pWeapon->CheckGrapplingState() == GPT_WRESTLING_ALL && !m_GrapplingTargetName.IsEmpty() )
		pWeapon->SetGrapplingTargetName( m_GrapplingTargetName );
	else if( pWeapon && pWeapon->CheckGrapplingState() == GPT_NO_CHANGE && !m_GrapplingTargetName.IsEmpty() )
		pWeapon->SetGrapplingTargetName( m_GrapplingTargetName );
	else if( pWeapon && pWeapon->CheckGrapplingState() == GPT_NO_CHANGE2 && !m_GrapplingTargetName.IsEmpty() )
		pWeapon->SetGrapplingTargetName( m_GrapplingTargetName );
	else if( pWeapon && pWeapon->CheckGrapplingState() == GPT_EXCEPT && !m_GrapplingTargetName.IsEmpty() )
		pWeapon->SetGrapplingTargetName( m_GrapplingTargetName );

	ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
	if( pZone )
	{
		ioBaseChar *pChar = pZone->GetOwner();
		if( pChar )
		{
			D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			pZone->SetCollisionDir( vDir );
			pZone->SetZoneValue( false, true );
		}
	}

	ioRotationWeapon *pRotation = ToRotationWeapon( pWeapon );
	if( pRotation )
	{
		if( m_RotateWeaponInfoList.size() < (UINT)m_iRotateWeaponCurCnt )
			return;

		float fCurAngle = m_RotateWeaponInfoList[m_iRotateWeaponCurCnt].m_fYawAngle;
		float fCurRange = m_RotateWeaponInfoList[m_iRotateWeaponCurCnt].m_fRangeOffSet;
		float fCurHeight = m_RotateWeaponInfoList[m_iRotateWeaponCurCnt].m_fHeightOffSet;

		fCurAngle = ioMath::ArrangeHead( fCurAngle );

		D3DXVECTOR3 vMoveDir;
		D3DXQUATERNION qtAngle;
		D3DXQuaternionRotationYawPitchRoll( &qtAngle, DEGtoRAD(fCurAngle), 0.0f, 0.0f );

		D3DXQUATERNION qtRotate = pWeapon->GetOwner()->GetTargetRot();
		vMoveDir = qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		vMoveDir = qtAngle * vMoveDir;
		D3DXVec3Normalize( &vMoveDir, &vMoveDir );

		D3DXVECTOR3 vPos = pWeapon->GetOwner()->GetMidPositionByRate();
		if( fCurRange > 0.0f )
			vPos += vMoveDir*fCurRange;

		if( fCurHeight > 0.0f )
			vPos.y += fCurHeight;

		pWeapon->SetStartPosition( vPos, pStage );
		pWeapon->SetMoveDir( vMoveDir );

		m_vecRotateWeaponIndex.push_back( pRotation->GetWeaponIndex() );
		m_iRotateWeaponCurCnt++;
	}
}

bool ioGrapplingSkill3::SetSkillGrapplingState( ioBaseChar *pChar, const ioHashString &szTargetName )
{
	if( !pChar ) return false;
	
	
	if ( m_EndOtherAttackType == EOAT_SWING )
	{
		//마샬야츠 도복
		//자이언트 스윙을 하기 위해서 상태 및 타입 추가
		if( m_SkillState != SS_ACTION && m_SkillState != SS_COMBO )
			return false;
	}
	else if( m_SkillState != SS_ACTION )
			return false;
	// 충돌처리 후 체크된 타겟을 m_GrapplingTargetName로 지정한다.
	m_GrapplingTargetName = szTargetName;
	
	ioBaseChar *pTarget = pChar->GetBaseChar( m_GrapplingTargetName );
	if( !pTarget ) return false;

	if( m_AutoComboType == ACT_NORMAL )
	{
		pTarget->SetTargetRotToRotate( pChar->GetTargetRot(), true, false );
		pTarget->SetSkillGrapplingState( pChar->GetCharName(), GetSkillWoundGrapplingInfo(), GetName() );

		SetSkillAutoComboState( pChar );
	}
	else
	{
		if ( m_EndOtherAttackType == EOAT_LINEAR )
			SetSkillLoopState( pChar );
		else if ( m_EndOtherAttackType == EOAT_SWING )
		{
			if ( !m_bUsedEndOtherAttack )
				SetSkillLoopState( pChar );
			else
				SetSwingState( pChar );
		}
		
	}

	
	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SSC_LOOP;
		kPacket << pChar->GetTargetRot();
		kPacket << szTargetName;
		kPacket << m_bUsedEndOtherAttack;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	// for NPC
	bool bNPC = false;
	bool bAttackerNPC = false;
	if( ToNpcChar( pTarget ) )
	{
		bNPC = true;
	}

	if( ToNpcChar( pChar ) )
	{
		bAttackerNPC = true;
	}

	// local or 본부 or NPC
	if( (m_AutoComboType != ACT_NORMAL) && (!P2PNetwork::IsNetworkPlaying() || !pChar->GetCreator()->IsNetworkMode() || (bNPC || bAttackerNPC)) )
	{
		if ( m_bUsedEndOtherAttack && m_EndOtherAttackType == EOAT_SWING )
		{
			pTarget->SetTargetRotToRotate( pChar->GetTargetRot(), true, false );
			pTarget->SetSkillGrapplingState( pChar->GetCharName(), m_SkillSwingGrapplingInfo, GetName() );
		}
		else
		{
			pTarget->SetTargetRotToRotate( pChar->GetTargetRot(), true, false );
			pTarget->SetSkillGrapplingState( pChar->GetCharName(), GetSkillWoundGrapplingInfo(), GetName() );
		}
		
	}
	return true;
}

D3DXVECTOR3 ioGrapplingSkill3::GetGrapplingTargetPos( ioBaseChar *pChar )
{
	if( m_SkillState == SS_SWINGLOOP )
	{
		if( m_fTargetSwingLoopGapRange > 0.0f )
		{
			D3DXVECTOR3 vPos = pChar->GetMidPositionByRate( m_fCharSwingHeightRate );
			D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVec3Normalize( &vDir, &vDir );

			if( m_fTargetSwingLoopGapRange > 0.0f )
				vPos += m_fTargetSwingLoopGapRange * vDir;

			return vPos;
		}
	}
	else
	{
		if( m_fTargetLoopGapRange > 0.0f )
		{
			D3DXVECTOR3 vPos = pChar->GetMidPositionByRate( m_fCharHeightRate );
			D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVec3Normalize( &vDir, &vDir );

			if( m_fTargetLoopGapRange > 0.0f )
				vPos += m_fTargetLoopGapRange * vDir;

			return vPos;
		}
	}
	

	return ioMath::VEC3_ZERO;
}

bool ioGrapplingSkill3::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwPreDelayStartTime > 0 )
		return false;

	switch( m_SkillState )
	{
	case SS_LOOP:
	case SS_AUTO_COMBO:
		return false;
	case SS_ACTION:
	case SS_COMBO:
	case SS_END:
	case SS_SWINGEND:
		if( m_dwMotionEndTime + dwActionStop < dwCurTime )
		{
			return true;
		}
		return false;
	}

	return false;
}

void ioGrapplingSkill3::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if( !pChar ) return;
	if( !pStage ) return;

	int iState;
	rkPacket >> iState;

	switch( iState )
	{
	case SSC_LOOP:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			pChar->SetTargetRotToRotate( qtRot, true, false );

			ioHashString szTargetName;
			rkPacket >> m_GrapplingTargetName;

			rkPacket >> m_bUsedEndOtherAttack;
			ioBaseChar *pTarget = pStage->GetBaseChar( m_GrapplingTargetName );

			if( pTarget && pTarget->IsNeedProcess() )
			{
				if ( m_bUsedEndOtherAttack && m_EndOtherAttackType == EOAT_SWING)
				{
					pTarget->SetSkillGrapplingState( pChar->GetCharName(), m_SkillSwingGrapplingInfo, GetName() );
				}
				else
					pTarget->SetSkillGrapplingState( pChar->GetCharName(), GetSkillWoundGrapplingInfo(), GetName() );
			}

			if( m_AutoComboType == ACT_NORMAL )
				SetSkillAutoComboState( pChar );
			else
			{
				if ( m_EndOtherAttackType == EOAT_LINEAR )
					SetSkillLoopState( pChar );
				else if ( m_EndOtherAttackType == EOAT_SWING )
				{
					if ( !m_bUsedEndOtherAttack )
						SetSkillLoopState( pChar );
					else
						SetSwingState( pChar );
				}
			}
		}
		break;
	case SSC_END:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			pChar->SetTargetRotToRotate( qtRot, true, false );

			if( m_SkillState != SS_END )
			{
				SetSkillEndState( pChar, false );
			}
		}
		break;
	case SSC_COMBO:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			pChar->SetTargetRotToRotate( qtRot, true, false );

			bool bOther;
			rkPacket >> m_iCurLinearCombo;
			rkPacket >> bOther;

			SetNextLinearCombo( pChar, bOther );

			if( IsUseAutoTargetValue() )
			{
				pChar->ApplyAutoTargetInfo( rkPacket );
			}
		}
		break;
	case SSC_DIR:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			pChar->SetTargetRotToRotate( qtRot, true, false );

			int iDir;
			rkPacket >> iDir;
			m_CurDirKey = (ioUserKeyInput::DirKeyInput)iDir;

			m_bTargetRot = false;
		}
		break;
	case SSC_TELEPORT:
		{
			rkPacket >> m_TeleportTargetName;
			rkPacket >> m_vTeleportTargetPos;
			rkPacket >> m_qtTeleportTargetRot;

			if( !m_TeleportTargetName.IsEmpty() )
			{
				pChar->SetWorldPosition( m_vTeleportTargetPos );
				pChar->SetTargetRotToRotate( m_qtTeleportTargetRot, true );

				AttackSkillFire( pChar );
			}
		}
		break;
	case SSC_SWINGEND:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			pChar->SetTargetRotToRotate( qtRot, true, false );
			ioBaseChar *pTarget = pStage->GetBaseChar( m_GrapplingTargetName );

			if( pTarget && pTarget->IsNeedProcess() )
			{
				pTarget->SetGrapplingWoundSwing( pChar->GetCharName(), m_SkillSwingGrapplingInfo,
				m_SkillSwingedMotion, m_fSkillSwingedMotionRate );
			}
			if( m_SkillState != SS_SWINGEND )
			{
				SetSwingSkillEndState( pChar, false );
			}
		}
		break;
	}
}

bool ioGrapplingSkill3::IsProtected( int iDefenseBreakType ) const
{
	if( DBT_NONE != (DefenseBreakType)m_ProtectCancelType )
	{
		if( iDefenseBreakType == m_ProtectCancelType )   // 스킬 무적 파괴
			return false;
	}

	DWORD dwCurTime = FRAMEGETTIME();

	if( IsPreDelayProtect() )
	{
		if( m_dwPreDelayStartTime != 0 )
			return true;
	}

	switch( m_SkillState )
	{
	case SS_ACTION:
		return true;
	case SS_LOOP:
	case SS_AUTO_COMBO:
	case SS_COMBO:
		if( m_dwCurSkillProtectTime < dwCurTime )
			return false;
		break;
	case SS_END:
	case SS_SWINGEND:
		return false;
	}

	return true;
}

bool ioGrapplingSkill3::IsEnableReserve() const
{
	return false;
}

bool ioGrapplingSkill3::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

bool ioGrapplingSkill3::IsComboState()
{
	if( m_SkillState == SS_COMBO )
		return true;

	return false;
}

void ioGrapplingSkill3::SetNextLinearCombo( ioBaseChar *pChar, bool bOther, bool bSendNet )
{
	if( !pChar )
		return;

	m_dwEnableReserveTime = 0;
	m_SkillState = SS_COMBO;

	if( !COMPARE( m_iCurLinearCombo, 0, (int)m_vComboList.size() ) )
		return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	AttackAttribute kAttack = m_vComboList[m_iCurLinearCombo];

	if( bOther )
		kAttack = m_EndOtherAttack;

	int iAniID = pGrp->GetAnimationIdx( kAttack.m_AttackAnimation );
	if( iAniID == -1 )	return;

	pChar->InitActionStopDelay( true );

	float fTimeRate = kAttack.m_fAttackAniRate;
	DWORD dwPreDelay = kAttack.m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	if( IsUseAutoTargetValue() )
	{
		pChar->SetAutoTarget( ATT_SKILL );
	}

	if( m_iCurLinearCombo == 0 )
		m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();

	pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex+m_iCurLinearCombo,
										iAniID,
										kAttack.m_vWeaponInfoList,
										FTT_SKILL_ATTACK,
										fTimeRate,
										dwPreDelay );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwMotionEndTime += dwPreDelay + kAttack.m_dwEndDelay;

	DWORD dwFireStartTime;
	dwFireStartTime = dwCurTime;

	dwFireStartTime += dwPreDelay;
	dwFireStartTime += pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;

	m_dwEnableReserveTime = dwCurTime;
	m_dwEnableReserveTime += pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;

	if( kAttack.m_AttackEndAnimation.IsEmpty() )
	{
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );
	}
	else
	{
		pGrp->SetActionAni( iAniID, FLOAT100, 0.0f, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );

		float fWaitTime = pGrp->GetAnimationFullTime( iAniID ) * fTimeRate + dwPreDelay;
		float fEndTimeRate = kAttack.m_fAttackEndAniRate;

		pGrp->SetActionAni( kAttack.m_AttackEndAnimation,
							0.0f, FLOAT100,
							1.0f, FLOAT1/fEndTimeRate,
							fWaitTime, 0.0f, true );

		int iEndAniID = pGrp->GetAnimationIdx( kAttack.m_AttackEndAnimation );
		m_dwMotionEndTime += pGrp->GetAnimationFullTime( iEndAniID ) * fEndTimeRate;
	}

	// 예외처리
	m_dwAttackEndTime = m_dwMotionEndTime;
	m_dwMotionEndTime += 100;

	//
	DWORD dwTrackingTime = dwCurTime + dwPreDelay;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;

	if( IsUseAutoTargetValue() )
	{
		pChar->SetAutoTargetTrackingEndTime( dwTrackingTime );
	}

	pChar->SetReservedSliding( kAttack.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	// For AirJumpPower
	SetAirJump( pChar, iAniID, fTimeRate );

	ioBaseChar *pTarget = pChar->GetBaseChar( m_GrapplingTargetName );
	if( pTarget && pTarget->GetState() == CS_GRAPPLING_WOUNDED_SKILL )
	{
		ioEntityGroup *pTargetGrp = pTarget->GetGroup();

		if( bOther )
		{
			if( pTargetGrp && !m_EndOtherWoundAni.IsEmpty() )
			{
				pTargetGrp->ClearAllLoopAni( FLOAT100, true );
				pTargetGrp->ClearAllActionAni( FLOAT100, true );

				pTargetGrp->SetLoopAni( m_EndOtherWoundAni, FLOAT100 );
			}
		}
		else
		{
			if( COMPARE( m_iCurLinearCombo, 0, (int)m_vComboWoundList.size() ) )
			{
				if( pTargetGrp && !m_vComboWoundList[m_iCurLinearCombo].IsEmpty() )
				{
					pTargetGrp->ClearAllLoopAni( FLOAT100, true );
					pTargetGrp->ClearAllActionAni( FLOAT100, true );

					pTargetGrp->SetLoopAni( m_vComboWoundList[m_iCurLinearCombo], FLOAT100 );
				}
			}
		}
	}

	if( bSendNet && pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SSC_COMBO;
		kPacket << pChar->GetTargetRot();
		kPacket << m_iCurLinearCombo;
		kPacket << bOther;

		if( IsUseAutoTargetValue() )
		{
			pChar->FillAutoTargetInfo( kPacket );
		}
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	m_iCurLinearCombo++;
}

void ioGrapplingSkill3::CheckLinearCombo( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	if( !pChar->IsNeedProcess() )
		return;

	if( m_bUsedEndOtherAttack )
		return;

	if( m_dwPreDelayStartTime != 0 )
		return;

	if( !COMPARE( m_iCurLinearCombo, 0, (int)m_vComboList.size() ) )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( m_AutoComboType != ACT_NONE )
	{
		if( m_dwEnableReserveTime > 0 && m_dwEnableReserveTime < dwCurTime )
		{
			bool bReserveSkill = false;
			int iSkillNum = pChar->GetSkillKeyInput();
			if( iSkillNum >= 0 )
			{
				bReserveSkill = true;
			}

			if( !bReserveSkill )
			{
				if( m_bUseEndOtherAttack && pChar->IsDefenseKey() )
				{
					m_bUsedEndOtherAttack = true;
					SetNextLinearCombo( pChar, true );
					return;
				}
			}
		}

		if( m_dwMotionEndTime < dwCurTime )
		{
			SetNextLinearCombo( pChar, false );
		}

		return;
	}

	if( m_dwEnableReserveTime == 0 )
		return;

	if( m_dwEnableReserveTime < dwCurTime )
	{
		bool bReserveSkill = false;
		int iSkillNum = pChar->GetSkillKeyInput();
		if( iSkillNum >= 0 )
		{
			bReserveSkill = true;
		}

		if( pChar->IsAttackKey() && !bReserveSkill )
		{
			SetNextLinearCombo( pChar, false );
			return;
		}
		else if( m_bUseEndOtherAttack && pChar->IsDefenseKey() )
		{				
			m_bUsedEndOtherAttack = true;
			SetNextLinearCombo( pChar, true );
			return;
		}
	}
}

bool ioGrapplingSkill3::IsJumpState() const
{
	if( m_SkillState == SS_COMBO )
		return true;

	return false;
}

void ioGrapplingSkill3::CheckKeyInput( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() ) return;

	ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
	if( eNewDirKey != m_CurDirKey )
	{
		m_bTargetRot = false;
		m_CurDirKey = eNewDirKey;

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
			kPacket << pOwner->GetCharName();
			kPacket << GetName();
			kPacket << SSC_DIR;
			kPacket << pOwner->GetTargetRot();
			kPacket << (int)m_CurDirKey;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioGrapplingSkill3::ProcessRotate( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( m_dwRotateTime <= 0 ) return;
	if( m_bTargetRot ) return;

	if( m_CurDirKey == ioUserKeyInput::DKI_NONE )
		return;

	float fFrameGap = g_FrameTimer.GetCurFrameGap();
	float fRate = fFrameGap / m_dwRotateTime;
	float fRotateAngle = 360.0f * fRate;
	D3DXVECTOR3 vCurMoveDir = ioMath::VEC3_ZERO;
	
	if ( m_SkillLoopType == SLT_STOP )
	{
		D3DXQUATERNION qtCurRot = pOwner->GetTargetRot();
		vCurMoveDir = qtCurRot * D3DXVECTOR3(0.0f, 0.0f, -FLOAT1);
	}
	else if ( m_SkillLoopType == SLT_NORMAL)
	{
		if ( m_vCurMoveDir == ioMath::VEC3_ZERO)
			return;
		vCurMoveDir = m_vCurMoveDir;
	}
	
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
		if ( m_SkillLoopType == SLT_STOP )
		{
			pOwner->SetTargetRotToRotate( qtRotate, true );
		}
		else if ( m_SkillLoopType == SLT_NORMAL)
		{
			m_vCurMoveDir = vTargetDir;

			pOwner->SetMoveDirByRotate( qtRotate );
		}
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

	if ( m_SkillLoopType == SLT_STOP )
	{
		pOwner->SetTargetRotToRotate( qtNewRot, true );
	}
	else if ( m_SkillLoopType == SLT_NORMAL)
	{
		m_vCurMoveDir = vNewDir;

		pOwner->SetMoveDirByRotate( qtRotate );
	}
}

void ioGrapplingSkill3::CheckTeleportTarget( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	
	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage ) return;

	BaseCharList vTargetList;
	BaseCharDistanceSqList vNewCharList;

	D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate();
	D3DXVECTOR3 vSightDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vSightDir, &vSightDir );

	if( m_fTeleportTargetOffSet != 0.0f )
		vPos += vSightDir * m_fTeleportTargetOffSet;

	// 충돌타입에 따라 리스트 가져오기
	switch( m_TeleportTargetColType )
	{
	case TCT_SPHERE:
		pOwner->GetTargetCharListBySphere( vPos, m_fTeleportTargetRange, m_fTeleportTargetAngle, vTargetList );
		break;
	case TCT_CYLINDER:
		pOwner->GetTargetCharListByCylinder( vPos, m_fTeleportTargetRange, m_fTeleportTargetAngle, m_fTeleportTargetUpHeight, m_fTeleportTargetUnderHeight, vTargetList );
		break;
	case TCT_BOX:
		// 사각형의 center을 vPos로 설정하게 되므로 여기서 캐릭터앞쪽에 생성하고 높이를 보정한다.
		vPos += vSightDir * m_fTeleportTargetRange * FLOAT05; 
		D3DXVECTOR3 vOffset( 0.0f, ( ( m_fTeleportTargetUpHeight + m_fTeleportTargetUnderHeight ) * FLOAT05 ) - m_fTeleportTargetUpHeight, 0.0f );
		vPos -= vOffset;
		pOwner->GetTargetCharListByBox( vPos, m_fTeleportTargetRange, m_fTeleportTargetWidth, m_fTeleportTargetUpHeight, m_fTeleportTargetUnderHeight, vTargetList );
		break;
	}

	if( pOwner->GetExperienceMode() == EMS_EXPERIENCE )
		vTargetList.clear();

	BaseCharList::iterator iter;
	for( iter = vTargetList.begin(); iter != vTargetList.end(); ++iter )
	{
		ioBaseChar *pTarget = (*iter);

		if( !pTarget )
			continue;

		if( pOwner == pTarget )
			continue;

		if( pTarget->IsSystemState() )
			continue;
		
		if( !pTarget->IsEnableTarget() ) 
			continue;

		// 쓰러짐 관련 체크
		if( m_TeleportTargetWoundType == TWT_AIR_WOUND_EXCEPT ) // 쓰러짐중 공중상태는 패스
		{
			if( pTarget->IsApplyDownState() && !pTarget->IsApplyDownState(false) )
				continue;
		}
		else if( m_TeleportTargetWoundType == TWT_DOWN_WOUND_EXCEPT ) // 쓰러짐중 바닥상태는 패스
		{
			if( pTarget->IsApplyDownState(false) )
				continue;
		}
		else if( m_TeleportTargetWoundType == TWT_ALL_WOUND_EXCEPT )
		{
			if( pTarget->IsApplyDownState() )
				continue;
		}
		else if( m_TeleportTargetWoundType == TWT_FLOATING_STATE )	// 공중에 뜬 상태가 아니면 패스
		{
			if( !pTarget->IsFloating() )
				continue;
		}
		else if( m_TeleportTargetWoundType == TWT_DOWN_WOUNDED ) //바닥에 쓰러져 있지 않거나 공중에 뜬 상태면 패스
		{
			if( !pTarget->IsApplyDownState() || pTarget->IsFloating() )
				continue;
		}

		D3DXVECTOR3 vDiff = pTarget->GetWorldPosition() - pOwner->GetWorldPosition();
		float fDistanceSq = D3DXVec3LengthSq( &vDiff );

		BaseCharDistanceSq kTarget;
		kTarget.m_Name = pTarget->GetCharName();
		kTarget.m_fDistanceSq = fDistanceSq;

		vNewCharList.push_back( kTarget );
	}

	if( vNewCharList.empty() )
	{
		m_vTeleportTargetPos = ioMath::VEC3_ZERO;
		m_TeleportTargetName.Clear();
	}
	else
	{
		std::sort( vNewCharList.begin(), vNewCharList.end(), BaseCharDistanceSqSort2() );

		ioHashString szCurTarget;
		szCurTarget = vNewCharList.front().m_Name;

		ioBaseChar *pTarget = pStage->GetBaseChar( szCurTarget );
		if( pTarget )
		{
			m_TeleportTargetName = szCurTarget;

			D3DXVECTOR3 vDir = pTarget->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVec3Normalize( &vDir, &vDir );

			m_vTeleportTargetPos = pTarget->GetWorldPosition() - (vDir * m_fTeleportOffSet);

			if( m_vTeleportTargetPos != ioMath::VEC3_ZERO )
			{
				pOwner->SetWorldPosition( m_vTeleportTargetPos );

				D3DXVECTOR3 vDiff = pTarget->GetWorldPosition() - m_vTeleportTargetPos;
				vDiff.y = 0.0f;
				D3DXVec3Normalize( &vDiff, &vDiff );

				 if( vDiff != ioMath::VEC3_ZERO )
				 {
					D3DXQUATERNION qtRotate;
					ioMath::CalcDirectionQuaternion( qtRotate, -vDiff );

					m_qtTeleportTargetRot = qtRotate;
					pOwner->SetTargetRotToRotate( m_qtTeleportTargetRot, true );
				 }

				if( pStage && pOwner->IsOwnerChar() )
					pStage->SetCameraToOwnerExplicit( pOwner );
			}
		}
		else
		{
			m_vTeleportTargetPos = ioMath::VEC3_ZERO;
			m_TeleportTargetName.Clear();
		}
	}
}

void ioGrapplingSkill3::SetActionSkill( ioBaseChar *pOwner )
{
	if( !pOwner  ) return;

	if( m_bUseTelelportTarget && pOwner->IsNeedProcess() )
	{
		CheckTeleportTarget( pOwner );

		AttackSkillFire( pOwner );

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
			kPacket << pOwner->GetCharName();
			kPacket << GetName();
			kPacket << SSC_TELEPORT;
			kPacket << m_TeleportTargetName;
			kPacket << m_vTeleportTargetPos;
			kPacket << m_qtTeleportTargetRot;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
	else
	{
		AttackSkillFire( pOwner );
	}
}

void ioGrapplingSkill3::SetSwingState( ioBaseChar *pChar )
{
	if( !pChar ) return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp ) return;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	float fAniRate = FLOAT1;
	if( m_EndOtherAttack.m_fAttackAniRate > 0.0f )
		fAniRate = m_EndOtherAttack.m_fAttackAniRate;

	int iAniID = pGrp->GetAnimationIdx( m_EndOtherAttack.m_AttackAnimation );
	if( iAniID == -1 )	return;

	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fAniRate );

	m_SkillState = SS_SWINGLOOP;
	m_dwSkillSwingLoopStartTime = FRAMEGETTIME();

	m_CurDirKey = ioUserKeyInput::DKI_NONE;

}

void ioGrapplingSkill3::ProcessSwingLoopRotate( ioBaseChar *pChar )
{
	if( m_dwSwingLoopRotateTime == 0 || !pChar )
		return;

	// 캐릭터 회전
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	float fRate = (fTimePerSec*1000) / m_dwSwingLoopRotateTime;
	float fRotateAngle = 360.0f * fRate;

	fRotateAngle = ioMath::ArrangeHead( fRotateAngle );

	D3DXQUATERNION	qtNewRot;
	D3DXVECTOR3 vAxis = ioMath::UNIT_Y;

	if( m_bReverseSwingLoopRotate )
		vAxis = -ioMath::UNIT_Y;

	D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fRotateAngle));

	D3DXQUATERNION qtCharRot = pChar->GetTargetRot();
	D3DXVECTOR3 vDir = qtCharRot * D3DXVECTOR3( 0.0, 0.0, -FLOAT1 );
	vDir = qtNewRot * vDir;

	ioMath::CalcDirectionQuaternion( qtNewRot, -vDir );
	pChar->SetTargetRotToRotate( qtNewRot, true, true );
}

void ioGrapplingSkill3::SetSwingSkillEndState( ioBaseChar *pChar, bool bSendNet )
{
	if( !pChar ) return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	float fAniRate = FLOAT1;
	if( m_fSkillSwingEndMotionRate> 0.0f )
		fAniRate = m_fSkillSwingEndMotionRate;

	int iAniID = pGrp->GetAnimationIdx( m_SkillSwingEndMotion );
	if ( iAniID == -1 ) return;
		
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fAniRate );

	m_dwMotionStartTime = m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fAniRate;

	m_dwFireMotionEndTime = dwCurTime;
	m_dwFireMotionEndTime += pGrp->GetAnimationEventTime_e( iAniID, "firemotion_e" ) * fAniRate;

	// Key Reserve
	m_dwEnableReserveTime = dwCurTime;
	m_dwEnableReserveTime += pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" );

	m_SkillState = SS_SWINGEND;

	
	if( bSendNet && pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SSC_SWINGEND;
		kPacket << pChar->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
	

	if ( m_GrapplingTargetName.IsEmpty() )
		return;
	ioBaseChar *pTarget = pChar->GetBaseChar( m_GrapplingTargetName );
	if( !pTarget ) return;

	// for NPC
	bool bNPC = false;
	bool bAttackerNPC = false;
	if( ToNpcChar( pTarget ) )
	{
		bNPC = true;
	}

	if( ToNpcChar( pChar ) )
	{
		bAttackerNPC = true;
	}

	// local or 훈련소 or NPC
	if( !P2PNetwork::IsNetworkPlaying() || !pChar->GetCreator()->IsNetworkMode() || (bNPC || bAttackerNPC) )
	{
		if( pTarget->GetState() == CS_GRAPPLING_WOUNDED_SKILL && pTarget->IsNeedProcess() )
		{
			pTarget->SetTargetRotToRotate( pChar->GetTargetRot(), true, false );
			pTarget->SetGrapplingWoundSwing( pChar->GetCharName(), m_SkillSwingGrapplingInfo,
				 m_SkillSwingedMotion, m_fSkillSwingedMotionRate );
		}
	}
	
	DestroyWeapon( pChar );
}

void ioGrapplingSkill3::CheckSwingMove( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	CheckKeyInput( pChar );
	ProcessRotate( pChar );
	if ( m_fRunSpeed > 0.0f )
		pChar->SetCurMoveSpeed( m_fRunSpeed );
	
}

void ioGrapplingSkill3::DestroyWeapon( ioBaseChar *pChar )
{
	if ( m_vecRotateWeaponIndex.empty() )
		return;
	for ( UINT i=0 ; i < m_vecRotateWeaponIndex.size() ; ++i )
	{
		ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pChar, m_vecRotateWeaponIndex[i] ); 
		if (pWeapon)
		{
			pWeapon->SetWeaponDead();
		}
	}
	
}
