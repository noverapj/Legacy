

#include "stdafx.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"
#include "ioGrowthUpInfo.h"

#include "ioTargetDebuffSkill6.h"

#include "WeaponDefine.h"

ioTargetDebuffSkill6::ioTargetDebuffSkill6()
{
	ClearData();
}

ioTargetDebuffSkill6::ioTargetDebuffSkill6( const ioTargetDebuffSkill6 &rhs )
	: ioNormalSkill( rhs ),
	m_bDisableGrowth( rhs.m_bDisableGrowth ),
	m_fGuagePerTic( rhs.m_fGuagePerTic ),
	m_dwTicTime( rhs.m_dwTicTime ),
	m_LoopDelayAnimation( rhs.m_LoopDelayAnimation ),
	m_LoopMoveAnimation( rhs.m_LoopMoveAnimation ),
	m_TargetWoundType( rhs.m_TargetWoundType ),
	m_fTargetAngle( rhs.m_fTargetAngle ),
	m_fTargetMaxRange( rhs.m_fTargetMaxRange ),
	m_fTargetMinRange( rhs.m_fTargetMinRange ),
	m_dwTargetMaxCnt( rhs.m_dwTargetMaxCnt ),
	m_szRedGuideMarker( rhs.m_szRedGuideMarker ),
	m_szBlueGuideMarker( rhs.m_szBlueGuideMarker ),
	m_szRedAimMarker( rhs.m_szRedAimMarker ),
	m_szBlueAimMarker( rhs.m_szBlueAimMarker ),
	m_szRedChargeMarker( rhs.m_szRedChargeMarker ),
	m_szBlueChargeMarker( rhs.m_szBlueChargeMarker ),
	m_szRedFireMarker( rhs.m_szRedFireMarker ),
	m_szBlueFireMarker( rhs.m_szBlueFireMarker ),
	m_fScalingRate( rhs.m_fScalingRate ),
	m_fCurScaleRate( rhs.m_fCurScaleRate ),
	m_fScalingSpeed( rhs.m_fScalingSpeed ),
	m_dwKeyDurationTime( rhs.m_dwKeyDurationTime ),
	m_AimAirAttribute( rhs.m_AimAirAttribute ),
	m_AimLandAttribute( rhs.m_AimLandAttribute ),
	m_TargetAimAttackBuffList( rhs.m_TargetAimAttackBuffList ),
	m_bSkillZeroAimAttackGauge( rhs.m_bSkillZeroAimAttackGauge ),
	m_bSkillAimAttackGauge( rhs.m_bSkillAimAttackGauge ),
	m_dwAimMarkerChargeEnableCnt( rhs.m_dwAimMarkerChargeEnableCnt ),
	m_dwSkillProtectedTime( rhs.m_dwSkillProtectedTime ),
	m_SkillMapEffect( rhs.m_SkillMapEffect ),
	m_fCharEffectOffset( rhs.m_fCharEffectOffset ),
	m_fTargetShowRange( rhs.m_fTargetShowRange ),

	m_fStartAngle( rhs. m_fStartAngle ),
	m_fStartMaxRange( rhs.m_fStartMaxRange ),
	m_dwStartDuration( rhs.m_dwStartDuration ),
	m_MoveAni( rhs.m_MoveAni ),
	m_fMoveAniRate( rhs.m_fMoveAniRate ),
	m_fMoveSpeed( rhs.m_fMoveSpeed ),
	m_fUpDownSpeed( rhs.m_fUpDownSpeed ),

	m_dwSkillLifeTime( rhs.m_dwSkillLifeTime ),

	m_fOffsetLook1( rhs.m_fOffsetLook1 ),
	m_fOffsetY1( rhs.m_fOffsetY1 ),
	m_fOffsetX1( rhs.m_fOffsetX1 ),

	m_fOffsetLook2( rhs.m_fOffsetLook2 ),
	m_fOffsetY2( rhs.m_fOffsetY2 ),
	m_fOffsetX2( rhs.m_fOffsetX2 ),

	m_AimTargetAttackType( rhs.m_AimTargetAttackType ),
	m_AimAttackAttribute( rhs.m_AimAttackAttribute ),
	m_fMoveAttackSpeed( rhs.m_fMoveAttackSpeed ),

	m_NextJumppingAnimation( rhs.m_NextJumppingAnimation ),
	m_NextJumppingAniRate( rhs.m_NextJumppingAniRate ),
	m_fJumpAngle( rhs.m_fJumpAngle ),
	m_fJumpMaxRange( rhs.m_fJumpMaxRange ),
	m_dwJumppingDuration( rhs.m_dwJumppingDuration ),
	m_fEnableJumppingHeight( rhs.m_fEnableJumppingHeight ),
	m_fOwnerDefaultAttackAngle( rhs.m_fOwnerDefaultAttackAngle ),
	m_dwEnableAimAttackTime( rhs.m_dwEnableAimAttackTime ),
	m_fMaxMoveDist( rhs.m_fMaxMoveDist ),
	m_fReduceAttackSkillGauge( rhs.m_fReduceAttackSkillGauge ),
	m_fExtraDecreaseGauge( rhs.m_fExtraDecreaseGauge ),
	m_fGaugeDecreaseHeight( rhs.m_fGaugeDecreaseHeight ),

	m_fNoTragetAimOffset( rhs.m_fNoTragetAimOffset ),
	m_bUpDownCheckByAttackKey( rhs.m_bUpDownCheckByAttackKey ),
	m_iMaxAttackCount( rhs.m_iMaxAttackCount ),
	m_dwExtraDecreaseGaugeTic( rhs.m_dwExtraDecreaseGaugeTic ),
	m_bRemoveAllGauge ( rhs.m_bRemoveAllGauge  ),
	m_vNoneTargetAimOffset( rhs.m_vNoneTargetAimOffset ),

	m_dwRotateTime( rhs.m_dwRotateTime )
{
	m_CurDirKey = ioUserKeyInput::DKI_NONE;
	m_PreDirKey = ioUserKeyInput::DKI_NONE;

	m_bSetUpKey = false;
	m_bSetDownKey = false;

	ClearData();
}

ioTargetDebuffSkill6::~ioTargetDebuffSkill6()
{
}

void ioTargetDebuffSkill6::ClearData()
{
	m_TargetList.clear();

	m_bSetHandMesh = false;
	m_bSetMoveAni = false;
	m_bFireMaker = false;

	// 성장관련 요소
	m_dwCheckTime = 0;
	m_fTicGaugeRateByGrowth = FLOAT1;
	m_dwTicTimeByGrowth = 0;

	//maker
	m_dwGuidMaker = - 1;	
	m_dwMarkCurCnt = 0;

	m_dwCurScaleStartTime = 0;

	m_dwSkillAimMotionEnd = 0;

	m_dwFireStartTime = 0;

	//m_dwSetLoopCnt = -1;
	m_dwCurAimMarkerChargeCnt = 0;

	m_dwStartSkillProtectedTime = 0;

	m_szTargetName.Clear();

	ResetSendList();

	m_vOwnerAttackDir = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );

	m_fCurJumpRange = 0;

	m_dwAimAttackStartTime = 0;

	m_fCurMoveDist = 0;
	m_fCurExtraDecreaseGauge = 0;
	m_OnlyOneTarget.Clear();

	m_NoTragetAimEffectID = -1;

	m_iCurAttackCount = 0;
	m_dwExtraDecreaseGaugeTicTime = 0;
	m_dwStartAttackTime = 0;
	m_dwNoneTargetAimEffectID = -1;
}

void ioTargetDebuffSkill6::LoadProperty( ioINILoader &rkLoader )
{
	ioNormalSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	m_bDisableGrowth = rkLoader.LoadBool_e( "disable_growth", false );
	m_fGuagePerTic = rkLoader.LoadFloat_e( "gauge_per_tick", 0.0f );
	m_dwTicTime = (DWORD)rkLoader.LoadInt_e( "tick_time", 0 );

	m_dwKeyDurationTime = rkLoader.LoadInt_e( "skill_zero_aim_attack_duration", 0 );
	m_bSkillZeroAimAttackGauge = rkLoader.LoadBool_e( "skill_zero_aim_attack_reduce_gauge", 1 );

	wsprintf_e( szBuf, "skill_aim_air_attack");
	LoadAttackAttribute( szBuf, m_AimAirAttribute, rkLoader );

	wsprintf_e( szBuf, "skill_aim_land_attack");
	LoadAttackAttribute( szBuf, m_AimLandAttribute, rkLoader );

	m_bSkillAimAttackGauge = rkLoader.LoadBool_e( "skill_aim_attack_reduce_gauge", 1 );

	m_dwAimMarkerChargeEnableCnt = rkLoader.LoadInt_e( "charge_marker_enable_cnt", 1000 );
	m_dwAimMarkerChargeEnableCnt = max( m_dwAimMarkerChargeEnableCnt, 1 );

	m_dwSkillProtectedTime = rkLoader.LoadFloat_e( "skill_protect_time", 0 );

	// target search range
	m_TargetWoundType = (TargetWoundType)rkLoader.LoadInt_e( "target_wound_type", TWT_ALL );
	m_fTargetAngle = rkLoader.LoadFloat_e( "target_angle", 0.0f );
	m_fTargetMaxRange = rkLoader.LoadFloat_e( "target_max_range", 0.0f );
	m_fTargetMinRange = rkLoader.LoadFloat_e( "target_min_range", 0.0f );
	m_dwTargetMaxCnt = rkLoader.LoadInt_e( "attack_count", 0 );

	m_fTargetShowRange = rkLoader.LoadFloat_e( "target_show_range", 0.0f );

	//loop motion
	rkLoader.LoadString_e( "delay_animation", "", szBuf, MAX_PATH );	
	m_LoopDelayAnimation = szBuf;
	rkLoader.LoadString_e( "move_animation", "", szBuf, MAX_PATH );
	m_LoopMoveAnimation	= szBuf;

	// aim mark
	rkLoader.LoadString_e( "red_guide_marker", "", szBuf, MAX_PATH );
	m_szRedGuideMarker = szBuf;
	rkLoader.LoadString_e( "blue_guide_marker", "", szBuf, MAX_PATH );
	m_szBlueGuideMarker = szBuf;

	rkLoader.LoadString_e( "red_aim_marker", "", szBuf, MAX_PATH );
	m_szRedAimMarker = szBuf;
	rkLoader.LoadString_e( "blue_aim_marker", "", szBuf, MAX_PATH );
	m_szBlueAimMarker = szBuf;

	rkLoader.LoadString_e( "red_charge_marker", "", szBuf, MAX_PATH );
	m_szRedChargeMarker = szBuf;
	rkLoader.LoadString_e( "blue_charge_marker", "", szBuf, MAX_PATH );
	m_szBlueChargeMarker = szBuf;

	rkLoader.LoadString_e( "red_fire_marker", "", szBuf, MAX_PATH );
	m_szRedFireMarker = szBuf;	
	rkLoader.LoadString_e( "blue_fire_marker", "", szBuf, MAX_PATH );
	m_szBlueFireMarker = szBuf;	

	//scale info
	m_fScalingRate = rkLoader.LoadFloat_e( "charge_scaling_rate", 0.0f );
	m_fCurScaleRate = m_fScalingRate;
	m_fScalingSpeed = rkLoader.LoadFloat_e( "charge_scaling_speed", 0.0f );


	rkLoader.LoadString_e( "skill_map_effect", "", szBuf, MAX_PATH );
	m_SkillMapEffect = szBuf;

	m_fCharEffectOffset = rkLoader.LoadFloat_e( "skill_map_effect_offset", 0.0f );

	LoadBuffList( rkLoader );

	//////////////////////////////////////////////////////////////////////////
	m_fStartAngle = rkLoader.LoadFloat_e( "fly_start_angle", 0.0f );
	m_fStartMaxRange = rkLoader.LoadFloat_e( "fly_start_max_range", 0.0f );
	m_dwStartDuration = (DWORD)rkLoader.LoadInt_e( "fly_start_duration", 0 );

	m_fMoveSpeed = rkLoader.LoadFloat_e( "fly_move_speed", 0.0f );
	m_fUpDownSpeed = rkLoader.LoadFloat_e( "fly_updown_speed", 0.0f );

	m_dwSkillLifeTime = (DWORD)rkLoader.LoadInt_e( "life_time", 0 );

	m_fOffsetLook1	= rkLoader.LoadFloat_e( "weapon_off_look1", 0.0f );
	m_fOffsetY1		= rkLoader.LoadFloat_e( "weapon_off_y1", 0.0f );
	m_fOffsetX1		= rkLoader.LoadFloat_e( "weapon_off_x1", 0.0f );
	m_fOffsetLook2	= rkLoader.LoadFloat_e( "weapon_off_look2", 0.0f );
	m_fOffsetY2		= rkLoader.LoadFloat_e( "weapon_off_y2", 0.0f );
	m_fOffsetX2		= rkLoader.LoadFloat_e( "weapon_off_x2", 0.0f );

	m_AimTargetAttackType = (AimTargetAttackType)rkLoader.LoadInt_e( "aim_target_attack_type", ATAT_FIRE );
	wsprintf_e( szBuf, "skill_aim_move_attack");
	LoadAttackAttribute( szBuf, m_AimAttackAttribute, rkLoader );
	m_fMoveAttackSpeed = rkLoader.LoadFloat_e( "aim_target_move_attack_speed", 0.0f );
	m_fEnableJumppingHeight = rkLoader.LoadFloat_e( "enable_jumpping_height", 0.0f );

	//Jump State
	rkLoader.LoadString_e( "next_jumpping_motion", "", szBuf, MAX_PATH );	
	m_NextJumppingAnimation = szBuf;
	m_NextJumppingAniRate = rkLoader.LoadFloat_e( "next_action_ani_rate", 0.0f );

	m_fJumpAngle = rkLoader.LoadFloat_e( "jump_angle", 0.0f );
	m_fJumpMaxRange = rkLoader.LoadFloat_e( "jump_range", 0.0f );
	m_dwJumppingDuration = (DWORD)rkLoader.LoadInt_e( "jump_duration", 0 );

	m_fOwnerDefaultAttackAngle = rkLoader.LoadFloat_e( "owner_default_attack_angle", 0.0f );
	m_dwEnableAimAttackTime = (DWORD)rkLoader.LoadInt_e( "enable_attack_time", 0 );
	m_fMaxMoveDist = rkLoader.LoadFloat_e( "attack_max_range", 0.0f );
	m_fReduceAttackSkillGauge = rkLoader.LoadFloat_e( "reduce_attack_skill_gauge", 0.0f );
	m_fExtraDecreaseGauge = rkLoader.LoadFloat_e( "extra_decrease_gauge", 0.0f );
	m_dwExtraDecreaseGaugeTic = (DWORD)rkLoader.LoadInt_e( "extra_decrease_gauge_tic_time", 0 );
	m_fGaugeDecreaseHeight = rkLoader.LoadFloat_e( "decrease_gauge_height", 0.0f );

	m_fNoTragetAimOffset = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	m_fNoTragetAimOffset.x = rkLoader.LoadFloat_e( "no_target_aim_offset_x", 0.0f );
	m_fNoTragetAimOffset.y = rkLoader.LoadFloat_e( "no_target_aim_offset_y", 0.0f );
	m_fNoTragetAimOffset.z = rkLoader.LoadFloat_e( "no_target_aim_offset_z", 0.0f );
	m_bUpDownCheckByAttackKey = rkLoader.LoadBool_e( "updown_check_by_attack_key", false );

	m_iMaxAttackCount = rkLoader.LoadFloat_e( "max_attack_count", 0 );
	m_bRemoveAllGauge  = rkLoader.LoadBool_e( "remove_all_gauge", false );
	
	m_vNoneTargetAimOffset.x = rkLoader.LoadFloat_e( "none_target_aim_offset_x", 0.0f );
	m_vNoneTargetAimOffset.y = rkLoader.LoadFloat_e( "none_target_aim_offset_y", 0.0f );
	m_vNoneTargetAimOffset.z = rkLoader.LoadFloat_e( "none_target_aim_offset_z", 0.0f );
	//////////////////////////////////////////////////////////////////////////
	m_dwRotateTime = rkLoader.LoadInt_e( "fly_rotate_time", 0 );
}

void ioTargetDebuffSkill6::LoadBuffList( ioINILoader &rkLoader )
{
	int i, iCount;
	ioBuffInfo BuffInfo;
	char szTitle[MAX_PATH], szBuff[MAX_PATH];

	m_TargetAimAttackBuffList.clear();
	iCount = rkLoader.LoadInt_e( "skill_zero_aim_attack_buff_cnt", 0 );
	for( i=0; i<iCount; ++i )
	{
		wsprintf_e( szTitle, "skill_zero_aim_attack_buff%d", i+1 );
		rkLoader.LoadString( szTitle, "", szBuff, MAX_PATH );

		BuffInfo.m_BuffName = szBuff;

		if( !BuffInfo.m_BuffName.IsEmpty() )
		{			
			BuffInfo.m_iBuffType = g_BuffMaker.GetBuffType( BuffInfo.m_BuffName );			
			if( BuffInfo.m_iBuffType != BT_NONE )
			{	
				m_TargetAimAttackBuffList.push_back( BuffInfo );
			}
		}
	}
}

ioSkill* ioTargetDebuffSkill6::Clone()
{
	return new ioTargetDebuffSkill6( *this );
}

bool ioTargetDebuffSkill6::OnSkillStart( ioBaseChar *pChar )
{
	m_bSkillInJump = false;

	if( !pChar )
		return false;

	ioNormalSkill::OnSkillStart( pChar );

	ClearData();

	m_SkillState = SS_START;
	m_dwCheckTime = FRAMEGETTIME();
	m_dwFlyStartTime = FRAMEGETTIME();

	if( m_SkillUseType == SUT_JUMP )
		m_bSkillInJump = true;

	pChar->AttachEffect( m_SkillEffectName );

	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}

	SetCameraBuff( pChar );

	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();

	pChar->SetSkillMoveEnable( false, false );

	// 성장관련 요소 추가.
	if( !m_bDisableGrowth )
	{
		ioPlayStage *pStage = pChar->GetCreator();
		if( pStage )
		{
			ioGrowthNormalUpInfo *pUpInfo = NULL;

			pUpInfo = ToNormalUpInfo( GetGrowthUpInfoByType(GT_SKILL_COOL_TIME, pChar->GetCharName()) );
			if( pUpInfo )
				m_fTicGaugeRateByGrowth = pUpInfo->GetValue(pStage);

			m_fTicGaugeRateByGrowth = min( m_fTicGaugeRateByGrowth, FLOAT100 );
			m_fTicGaugeRateByGrowth = FLOAT1 + ( m_fTicGaugeRateByGrowth/FLOAT100 );


			pUpInfo = ToNormalUpInfo( GetGrowthUpInfoByType(GT_BUFF_TIC_TIME, pChar->GetCharName()) );
			if( pUpInfo )
				m_dwTicTimeByGrowth = pUpInfo->GetValue(pStage);			
		}
	}

	if( !m_bReduceGauge )
	{
		float fCurGauge = pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
		m_bReduceGauge = true;
	}

	ioPlayStage* pStage = g_SkillMaker.GetPlayStage();
	if( !StartPreDelayAnimation( pChar ) )	// Delay Animation이 없다
	{	
		SetDelayState( pChar, pStage, true );
		CreateGuideMarker( pChar, pStage );
	}else if( m_bSkillInJump )	
	{
		SetDelayState( pChar, pStage, true );
		CreateGuideMarker( pChar, pStage );
	}

	m_fFlyCurRange = 0;
	m_bSipping = true;

	m_dwStartSkillProtectedTime = FRAMEGETTIME();

	return true;
}

void ioTargetDebuffSkill6::OnSkillEnd( ioBaseChar *pChar )
{
	ioNormalSkill::OnSkillEnd( pChar );

	// 기본무기 장착
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( true );
	}

	// 스킬 중간 동작에 사용된 메쉬들 제거
	if( !m_HandMesh.IsEmpty() )
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );

	EndCameraBuff( pChar );

	pChar->SetSkillMoveEnable( false, false );

	if( !m_SkillSoundName.IsEmpty() )
	{
		g_SoundMgr.StopSound( m_SkillSoundName, m_SkillSoundID );		
	}

	//인피갑옷은 도중에 스킬이 끝나고 스킬게이지가 다 줄지 않는 상태
	//발키리 SD는 스킬이 끝나면 모든 게이지를 없애야하므로 해당 값 추가
	if ( m_bRemoveAllGauge )
		pChar->SetCurItemSkillGauge( GetOwnerItem()->GetName(), 0.0f );
	

	ioPlayStage* pStage = g_SkillMaker.GetPlayStage();
	DestroyAllMarker( pChar, pStage );

	if ( m_NoTragetAimEffectID != -1 )
	{
		ioMapEffect *pMapEffect = pStage->FindMapEffect( m_NoTragetAimEffectID );
		if ( pMapEffect )
		{
			pChar->EndMapEffect( m_NoTragetAimEffectID, false );
			m_NoTragetAimEffectID = -1;
		}
	}

	if ( m_dwNoneTargetAimEffectID != -1 )
	{
		ioMapEffect *pMapEffect = pStage->FindMapEffect( m_dwNoneTargetAimEffectID );
		if ( pMapEffect )
		{
			pChar->EndMapEffect( m_dwNoneTargetAimEffectID, false );
			m_dwNoneTargetAimEffectID = -1;
		}
	}
	ClearData();
}

void ioTargetDebuffSkill6::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

	float fCurTicGauge = m_fGuagePerTic / m_fTicGaugeRateByGrowth;
	int iTickGap = (int)( dwCurTime - m_dwCheckTime );
	int iCurTicTime = m_dwTicTime + m_dwTicTimeByGrowth;

	switch( m_SkillState )
	{
	case SS_START:
		{
			DWORD dwPreDelay = GetPreDelayTime();
			pChar->SetCurMoveSpeed( 0 );
			
			if( !ProcessFlyStart( pChar ) )
			{
				//m_dwPreDelayStartTime = 0;
				m_dwStartSkillProtectedTime = FRAMEGETTIME();

				EndPreDelayEffect( pChar );
				SetDelayState( pChar, pStage, false );
				CreateGuideMarker( pChar, pStage );
			}
		}
		break;
	case SS_DELAY:
		{	
			if( (m_dwSkillLifeTime + m_dwFlyStartTime) < dwCurTime )
			{
				SetEndState( pChar, pStage );
				return;
			}

			CheckNormalMoveAni( pChar );
			UpdateGuideMarker( pChar, pStage );
			UpdateScaleInfo( pChar, pStage );

			if( pChar->IsNeedProcess() )
			{
				ResetSendList();
				FindTarget( pChar, pStage );
				UpdateTargetList( pChar, pStage );
			}

			if( pChar->IsNeedSendNetwork() )
				SendList( pChar, pStage );

			if( pChar->IsAttackKey() && pChar->IsNeedProcess() )
				SetAimFireState( pChar, pStage );

			if( m_bFireMaker && m_dwCurScaleStartTime + m_dwKeyDurationTime < dwCurTime )
				m_bFireMaker = false;
			

			ProcessFlyMove( pChar );
			// CheckKeyInput 내부에서 상태가 바뀔수 있다
			CheckKeyInput( pChar );

			ProcessRotate( pChar );
		}
		break;
	case SS_AIM_FIRE:
		{
			OnAimFireState( pChar, pStage, dwCurTime );

			ProcessFlyMove( pChar );
			CheckKeyInput( pChar );
			ProcessRotate( pChar );

			if( 0 < m_dwSkillAimMotionEnd && m_dwSkillAimMotionEnd < dwCurTime )	
				SetDelayState( pChar, pStage, true );
		}
		break;
	case SS_JUMP_STATE:
		{
			if( m_dwMotionEndTime < dwCurTime )
				SetDelayState( pChar, pStage, true );
			else if ( !ProcessJumpping( pChar, pStage ) )
				SetDelayState( pChar, pStage, true );
		}
		break;
	case SS_END:
		{

		}
		break;
	}

	if( m_SkillState != SS_END )
	{
		//첫번째 타격 이후는 게이지 감소를 하지 않음
		//if( m_dwSetLoopCnt <= 0 )
		{
			if( iTickGap >= iCurTicTime )
			{
				m_dwCheckTime = dwCurTime - ( iTickGap - iCurTicTime );

				if( fCurTicGauge != 0.0f )
				{
					if ( m_fExtraDecreaseGauge > 0.0f && m_fCurExtraDecreaseGauge > 0.0f )
						fCurTicGauge += ( m_fCurExtraDecreaseGauge / m_fTicGaugeRateByGrowth );
					if( pChar->ReduceCurSkillGaugeBySkill( GetOwnerItem(), fCurTicGauge ) <= 0.0f )
					{
						if( //m_SkillState != SS_ZERO_AIM_FIRE && 
							m_SkillState != SS_AIM_FIRE &&
							//m_SkillState != SS_AIM_MOVE_ATTACK &&
							m_SkillState != SS_JUMP_STATE &&
							m_SkillState != SS_END )
						{
							SetEndState( pChar, pStage );
							if ( m_fExtraDecreaseGauge > 0.0f )
							{
								if ( pChar->IsNeedSendNetwork())
								{
									SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
									kPacket << pChar->GetCharName();
									kPacket << GetName();
									kPacket << SSC_END;
									P2PNetwork::SendToAllPlayingUser( kPacket );
								}
							}
						}
					}
				}
			}
		}
	}
}


bool ioTargetDebuffSkill6::ProcessFlyStart( ioBaseChar *pOwner )
{
	if( !pOwner )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	D3DXVECTOR3 vTargetDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vTargetDir, &vTargetDir );

	D3DXVECTOR3 vAxis;
	D3DXVec3Cross( &vAxis, &vTargetDir, &ioMath::UNIT_Y );
	D3DXVec3Normalize( &vAxis, &vAxis );

	D3DXQUATERNION qtNewRot;
	float fCurAngle = ioMath::ArrangeHead( m_fStartAngle );
	D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fCurAngle) );

	D3DXVECTOR3 vMoveDir = qtNewRot * vTargetDir;
	D3DXVec3Normalize( &vMoveDir, &vMoveDir );

	if( vMoveDir == ioMath::VEC3_ZERO )
		return false;

	if( m_dwStartDuration == 0 )
		return false;

	float fCurSpeed = m_fStartMaxRange / (float)m_dwStartDuration * FLOAT1000;
	fCurSpeed = max( 0.0f, fCurSpeed );

	float fDistance = fCurSpeed * fTimePerSec;

	bool bRangeLimit = false;
	D3DXVECTOR3 vMoveAmt( 0.0f, 0.0f, 0.0f );
	if( m_fFlyCurRange+fDistance < m_fStartMaxRange )
	{
		bRangeLimit = true;
		vMoveAmt = vMoveDir * fDistance;

		m_fFlyCurRange += fDistance;
	}
	else
	{
		bRangeLimit = false;

		float fGapDistance = m_fStartMaxRange - m_fFlyCurRange;
		vMoveAmt = vMoveDir * fGapDistance;
	}

	bool bCol = false;
	ioPlayStage *pStage = pOwner->GetCreator();
	if( pStage && pStage->ReCalculateMoveVectorByTerrain( pOwner, &vMoveAmt, bCol ) )
	{
		pOwner->Translate( vMoveAmt );
	}

	D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
	if( pOwner->CheckAxisLimit( &vPos ) )
		pOwner->SetWorldPosition( vPos );

	return bRangeLimit;
}


void ioTargetDebuffSkill6::ProcessFlyMove( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	DWORD	dwCurTime = FRAMEGETTIME();

	D3DXVECTOR3 vMoveDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vMoveDir, &vMoveDir );

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	// Dir Move
	/*if( m_CurDirKey != ioUserKeyInput::DKI_NONE )
	{
		float fCurMoveSpeed = m_fMoveSpeed * fTimePerSec;
		D3DXVECTOR3 vMoveAmt = vMoveDir * fCurMoveSpeed;

		bool bCol = false;
		if( pStage->ReCalculateMoveVectorByTerrain( pOwner, &vMoveAmt, bCol ) )
			pOwner->Translate( vMoveAmt );
	}*/

	pOwner->SetCurMoveSpeed( m_fMoveSpeed );

	D3DXVECTOR3 vCurPos = pOwner->GetWorldPosition();
	float fMapHeight = pStage->GetMapHeight( vCurPos.x, vCurPos.z, pOwner, false, pOwner->GetMidHeightByRate(1.0f) );


	// UpDown Move
	if( m_bSetUpKey || m_bSetDownKey )
	{
		float fCurHeightAmt = 0.0f;
		float fCurUpDownSpeed = m_fUpDownSpeed * fTimePerSec;

		if( m_bSetUpKey )
			fCurHeightAmt += fCurUpDownSpeed;
		else if( m_bSetDownKey )
			fCurHeightAmt -= fCurUpDownSpeed;

		vCurPos.y += fCurHeightAmt;
	}


	vCurPos.y = max( vCurPos.y, fMapHeight + 11.0f );

	pStage->CheckFlyMapLimit( &vCurPos );
	pStage->CheckAxisLimit( &vCurPos );
	pOwner->SetWorldPosition( vCurPos );

	if ( m_fExtraDecreaseGauge > 0.0f )
	{
		float fDist = vCurPos.y - fMapHeight;
		if ( fDist >= m_fGaugeDecreaseHeight )
		{
			if ( m_dwExtraDecreaseGaugeTicTime + m_dwExtraDecreaseGaugeTic < dwCurTime )
			{
				m_dwExtraDecreaseGaugeTicTime = dwCurTime;
				m_fCurExtraDecreaseGauge += m_fExtraDecreaseGauge;
			}
			
		}
		else
			m_fCurExtraDecreaseGauge = 0.0f;
	}

	return;
}


void ioTargetDebuffSkill6::CheckKeyInput( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() )
		return;

	bool bChangeDir = false;
	bool bChangeUpDown = false;

	// Check Dir
	ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
	if( eNewDirKey != m_CurDirKey )
	{
		bChangeDir = true;

		m_PreDirKey = m_CurDirKey;
		m_CurDirKey = eNewDirKey;
	}

	if ( m_bUpDownCheckByAttackKey )
	{
		if ( pOwner->IsAttackKeyDown() )
			return;
	}
	// Check UpDown
	if( m_bSetUpKey )
	{
		if( !pOwner->IsJumpKeyDown() )
		{
			bChangeUpDown = true;
			m_bSetUpKey = false;

			if( pOwner->IsDefenseKeyDown() )
				m_bSetDownKey = true;
		}
	}
	else if( m_bSetDownKey )
	{
		if( !pOwner->IsDefenseKeyDown() )
		{
			bChangeUpDown = true;
			m_bSetDownKey = false;

			if( pOwner->IsJumpKeyDown() )
				m_bSetUpKey = true;
		}
	}
	else
	{
		if( pOwner->IsDefenseKeyDown() )
		{
			bChangeUpDown = true;
			m_bSetUpKey = false;
			m_bSetDownKey = true;
		}

		if( pOwner->IsJumpKeyDown() )
		{
			bChangeUpDown = true;
			m_bSetUpKey = true;
			m_bSetDownKey = false;
		}
	}

	if( bChangeDir || bChangeUpDown )
	{
		//ChangeFlyMove( pOwner );

		if( pOwner->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
			kPacket << pOwner->GetCharName();
			kPacket << GetName();
			kPacket << SSC_ROTATE;
			kPacket << bChangeDir;
			kPacket << bChangeUpDown;
			kPacket << pOwner->GetTargetRot();
			kPacket << (int)m_CurDirKey;
			kPacket << m_bSetUpKey;
			kPacket << m_bSetDownKey;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}


void ioTargetDebuffSkill6::ChangeFlyMove( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	//DWORD dwCurTime = FRAMEGETTIME();

	// KeyDir
	ioUserKeyInput::DirKeyInput eDir = m_CurDirKey;

	int iAniID = 0;
	float fTimeRate = 0;
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	if( eDir == ioUserKeyInput::DKI_NONE )
	{
		iAniID = pGrp->GetAnimationIdx( m_LoopDelayAnimation );
		fTimeRate = FLOAT100;
		pGrp->SetLoopAni( iAniID, FLOAT100 );//, FLOAT1, FLOAT1/fTimeRate );
	}
	else
	{
		iAniID = pGrp->GetAnimationIdx( m_LoopMoveAnimation );
		fTimeRate = FLOAT100;
		pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fTimeRate );
	}
}


void ioTargetDebuffSkill6::StartScaleInfo()
{
	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwCurScaleStartTime == 0 )
	{		
		m_fCurScaleRate = m_fScalingRate;
		m_dwCurScaleStartTime = dwCurTime;
	}
}

void ioTargetDebuffSkill6::ResetScaleInfo( DWORD dwCurTime )
{
	m_fCurScaleRate = m_fScalingRate;
	m_dwCurScaleStartTime = dwCurTime;
}


void ioTargetDebuffSkill6::UpdateScaleInfo( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( m_bFireMaker )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	m_fCurScaleRate -= (fTimePerSec * m_fScalingSpeed);

	if( m_fCurScaleRate < FLOAT1 )
	{
		m_fCurScaleRate = m_fScalingRate;
		m_dwCurScaleStartTime = dwCurTime;

		m_bFireMaker = true;
		m_dwCurAimMarkerChargeCnt++;
	}	
}

void ioTargetDebuffSkill6::SetAimFireState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar ) return;
	if( !pStage ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_dwEnableAimAttackTime > 0 && m_dwStartAttackTime + m_dwEnableAimAttackTime > dwCurTime )
		return;

	m_dwStartAttackTime = dwCurTime;
	m_SkillState = SS_AIM_FIRE;
	DestroyAllMarker( pChar, pStage );

	ioEntityGroup *pGrp = pChar->GetGroup();

	int iAniID = 0;
	iAniID = pGrp->GetAnimationIdx(	m_AimAirAttribute.m_AttackAnimation );
	if( iAniID == -1 )
		return;

	float fTimeRate = m_AimAirAttribute.m_fAttackAniRate * m_fExtraAniRate;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	m_dwSkillAimMotionEnd = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	m_dwFireMotionEndTime = dwCurTime;
	m_dwFireMotionEndTime += pGrp->GetAnimationEventTime_e( iAniID, "firemotion_e" ) * fTimeRate;

	if( m_dwFireStartTime == 0 )
	{
		m_dwFireStartTime = dwCurTime;
		m_dwFireStartTime += pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;
	}

	pChar->SetSkillMoveEnable( false, false );

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SSC_AIM_FIRE;
		kPacket << (int)m_TargetList.size();
		TargetList::iterator target_iter = m_TargetList.begin();
		for( ; target_iter != m_TargetList.end(); ++target_iter )
		{
			ioHashString stName = (*target_iter).second.m_szTargetName;
			bool bAir = (*target_iter).second.m_bAir;
			kPacket << stName;
			kPacket << bAir;
		}
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	/*if( m_bSkillAimAttackGauge )
		pChar->ReduceCurSkillGaugeBySkill( GetOwnerItem(), pChar->GetCurItemSkillGauge( GetOwnerItem()->GetName() ) );*/
	
	//if ( m_AimTargetAttackType == ATAT_TARGET_FIRE )
	m_iCurAttackCount++;
}

void ioTargetDebuffSkill6::OnAimFireState( ioBaseChar *pChar, ioPlayStage *pStage, DWORD dwCurTime )
{	
	if( !pChar )
		return;

	if( !pStage )
		return;

	int iWeaponCnt = 0;
	if( 0 < m_dwFireStartTime && m_dwFireStartTime < dwCurTime )
	{
		if( !m_TargetList.empty() )
		{
			TargetList::iterator target_iter = m_TargetList.begin();
			m_CurFireIter = m_TargetList.begin();

			for( ; target_iter != m_TargetList.end(); ++target_iter )
			{
				m_pTargetChar = NULL;
				ioBaseChar* pTarget = pStage->GetBaseChar( (*target_iter).second.m_szTargetName );
				
				if( pTarget )
				{
					m_pTargetChar = pTarget;
					m_vAttackDir = pTarget->GetWorldPosition() - pChar->GetWorldPosition();
					D3DXVec3Normalize( &m_vAttackDir, &m_vAttackDir );
					m_szTargetName = (*target_iter).second.m_szTargetName;

					WeaponInfoList::iterator iter;
					if( (*target_iter).second.m_bAir )
					{
						iter = m_AimAirAttribute.m_vWeaponInfoList.begin();
						for( ; iter != m_AimAirAttribute.m_vWeaponInfoList.end(); ++iter )
						{
							D3DXVECTOR3 vFirePos = pChar->GetWorldPosition();
							if ( m_AimTargetAttackType == ATAT_TARGET_FIRE )
								vFirePos = pTarget->GetWorldPosition();

							ioWeapon* pWeapon = pChar->SkillFireExplicit( (*iter),
								GetName(),
								m_dwWeaponBaseIndex+iWeaponCnt,
								vFirePos, m_vAttackDir );
							if( pWeapon )
							{
								ioTraceWeapon* pTraceWeapon = ToTraceWeapon( pWeapon );
								if( pTraceWeapon )
									pTraceWeapon->SetUseChangeDir( false );
							}
							iWeaponCnt++;
						}
					}
					else
					{
						iter = m_AimLandAttribute.m_vWeaponInfoList.begin();
						for( ; iter != m_AimLandAttribute.m_vWeaponInfoList.end(); ++iter )
						{
							D3DXVECTOR3 vFirePos = pChar->GetWorldPosition();
							if ( m_AimTargetAttackType == ATAT_TARGET_FIRE )
								vFirePos = pTarget->GetWorldPosition();

							ioWeapon* pWeapon = pChar->SkillFireExplicit( (*iter),
								GetName(),
								m_dwWeaponBaseIndex+iWeaponCnt,
								vFirePos, m_vAttackDir );
							
							if( pWeapon )
							{
								ioTraceWeapon* pTraceWeapon = ToTraceWeapon( pWeapon );
								if( pTraceWeapon )
									pTraceWeapon->SetUseChangeDir( false );
							}

							iWeaponCnt++;
						}
					}

					
				}
			}
		}
		else
		{
			//int iWeaponCnt = 0;
			WeaponInfoList::iterator iterAir = m_AimAirAttribute.m_vWeaponInfoList.begin();
			WeaponInfoList::iterator iterLand = m_AimLandAttribute.m_vWeaponInfoList.begin();
			m_vAttackDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			m_szTargetName.Clear();
			m_pTargetChar = 0;
			for( ; iterAir != m_AimAirAttribute.m_vWeaponInfoList.end(); ++iterAir )
			{
				D3DXVECTOR3 vFirePos = pChar->GetWorldPosition();
				if ( m_AimTargetAttackType == ATAT_TARGET_FIRE )
				{
					ioMapEffect *pMapEffect = pStage->FindMapEffect( m_dwNoneTargetAimEffectID );
					if ( pMapEffect )
					{
						vFirePos = pMapEffect->GetWorldPosition();

						pChar->EndMapEffect( m_dwNoneTargetAimEffectID, false );
						m_dwNoneTargetAimEffectID = -1;
					}
				}

				pChar->SkillFireExplicit( (*iterAir),
					GetName(),
					m_dwWeaponBaseIndex+iWeaponCnt,
					vFirePos );

				iWeaponCnt++;
			}

			for( ; iterLand != m_AimLandAttribute.m_vWeaponInfoList.end(); ++iterLand )
			{
				D3DXVECTOR3 vFirePos = pChar->GetWorldPosition();
				if ( m_AimTargetAttackType == ATAT_TARGET_FIRE )
				{
					ioMapEffect *pMapEffect = pStage->FindMapEffect( m_dwNoneTargetAimEffectID );
					if ( pMapEffect )
					{
						vFirePos = pMapEffect->GetWorldPosition();

						pChar->EndMapEffect( m_dwNoneTargetAimEffectID, false );
						m_dwNoneTargetAimEffectID = -1;
					}
				}

				pChar->SkillFireExplicit( (*iterLand),
					GetName(),
					m_dwWeaponBaseIndex+iWeaponCnt,
					vFirePos );

				iWeaponCnt++;
			}
		}

		m_dwFireStartTime = 0;
	}
}

void ioTargetDebuffSkill6::CreateGuideMarker( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar ) return;
	if( !pStage ) return;


	D3DXVECTOR3 vScale( FLOAT1, FLOAT1, FLOAT1 );
	D3DXVECTOR3 vPos = pChar->GetWorldPosition();
	D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

	vPos.y = pChar->GetMidHeightByRate();
	//vPos += ( vDir * m_fTargetMaxRange );
	vPos += ( vDir * m_fTargetShowRange );

	ioMapEffect *pMapEffect = NULL;

	if( pChar->IsNeedProcess() ) 
	{
		pMapEffect = pStage->CreateMapEffect( m_szBlueGuideMarker, vPos, vScale );
	}
	else
	{
		if( pChar->GetTeam() == TEAM_BLUE )
			pMapEffect = pStage->CreateMapEffect( m_szBlueGuideMarker, vPos, vScale );
		else
			pMapEffect = pStage->CreateMapEffect( m_szRedGuideMarker, vPos, vScale );
	}

	if( pMapEffect )
	{
		m_dwGuidMaker = pMapEffect->GetUniqueID();

		D3DXQUATERNION qtCharRot;
		ioMath::CalcDirectionQuaternion( qtCharRot, -vDir );
		pMapEffect->SetWorldOrientation( qtCharRot );
	}
}

void ioTargetDebuffSkill6::UpdateGuideMarker( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar ) return;
	if( !pStage ) return;

	if( !m_TargetList.empty() )
	{			
		DestroyGuideMarker( pChar );
		return;
	}

	if( m_dwGuidMaker == -1 && m_TargetList.empty() )
	{
		CreateGuideMarker( pChar, pStage );
		return;
	}

	D3DXVECTOR3 vScale( FLOAT1, FLOAT1, FLOAT1 );
	D3DXVECTOR3 vPos = pChar->GetWorldPosition();
	D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	vPos.y = pChar->GetMidHeightByRate();
	//vPos += ( vDir * m_fTargetMaxRange );
	vPos += ( vDir * m_fTargetShowRange );

	ioMapEffect *pMapEffect = pStage->FindMapEffect( m_dwGuidMaker );
	if( pMapEffect )
	{
		pMapEffect->SetWorldPosition( vPos );

		D3DXQUATERNION qtCharRot;
		ioMath::CalcDirectionQuaternion( qtCharRot, -vDir );
		pMapEffect->SetWorldOrientation( qtCharRot );
	}

}

void ioTargetDebuffSkill6::DestroyGuideMarker( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	if( m_dwGuidMaker != -1 )
	{
		pChar->EndMapEffect( m_dwGuidMaker, false );
		m_dwGuidMaker = -1;
	}
}

void ioTargetDebuffSkill6::CreateFireMarker( ioBaseChar *pChar, ioPlayStage *pStage, EffectInfo& pInfo )
{
	if( !pChar ) return;
	if( !pStage ) return;

	ioMapEffect *pMapEffect = pStage->FindMapEffect( pInfo.m_dwFireEffectID );
	if( !pMapEffect )
	{
		D3DXVECTOR3 vScale( FLOAT1, FLOAT1, FLOAT1 );
		D3DXVECTOR3 vPos = pChar->GetWorldPosition();
		D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

		if( pChar->IsNeedProcess() ) 
		{
			pMapEffect = pStage->CreateMapEffect( m_szBlueFireMarker, vPos, vScale );
		}
		else
		{
			if( pChar->GetTeam() == TEAM_BLUE )
				pMapEffect = pStage->CreateMapEffect( m_szBlueFireMarker, vPos, vScale );
			else
				pMapEffect = pStage->CreateMapEffect( m_szRedFireMarker, vPos, vScale );
		}		
		if( pMapEffect )
		{
			pInfo.m_dwFireEffectID = pMapEffect->GetUniqueID();

			D3DXQUATERNION qtCharRot;
			ioMath::CalcDirectionQuaternion( qtCharRot, -vDir );
			pMapEffect->SetWorldOrientation( qtCharRot );
		}
	}
}

void ioTargetDebuffSkill6::UpdateFireMarker( ioBaseChar *pTarget, ioPlayStage *pStage, UniqueObjID dwEffectID )
{
	D3DXVECTOR3 vTargetPos = pTarget->GetWorldPosition();
	vTargetPos.y = pTarget->GetMidHeightByRate();

	ioMapEffect *pMapEffect = pStage->FindMapEffect( dwEffectID );
	if( pMapEffect )
	{
		pMapEffect->SetWorldPosition( vTargetPos );
	}
}

void ioTargetDebuffSkill6::SetDelayState( ioBaseChar *pChar, ioPlayStage *pStage, bool bEraseTarget )
{
	if( !pChar || !pStage )
		return;

	if ( m_iMaxAttackCount > 0 && m_iCurAttackCount >= m_iMaxAttackCount )
	{
		SetEndState( pChar, pStage );
		return;
	}
	m_dwPreDelayStartTime = 0;

	DWORD dwCurTime = FRAMEGETTIME();
	m_SkillState = SS_DELAY;

	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	m_dwCurAimMarkerChargeCnt = 0;

	ioEntityGroup *pGrp = pChar->GetGroup();

	int iAniID = 0;
	iAniID = pGrp->GetAnimationIdx( m_LoopDelayAnimation );
	if( iAniID == -1 )
		return;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	pGrp->SetLoopAni( iAniID, FLOAT100 );
	m_dwMotionEndTime = 0;

	//pChar->SetSkillMoveEnable( true, true );

	m_bSetMoveAni = false;

	if( !pChar->IsNeedProcess() && bEraseTarget )
	{
		//m_TargetList.clear();
		ClearTargetList( pChar, pStage );
		m_dwMarkCurCnt = 0;
	}
	else if( pChar->IsNeedProcess() )
	{
		//m_TargetList.clear();
		ClearTargetList( pChar, pStage );
		m_dwMarkCurCnt = 0;
	}

}

void ioTargetDebuffSkill6::CheckNormalMoveAni( ioBaseChar* pChar )
{
	if( !pChar ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = 0;

	if( pChar->IsSettedDirection() )
	{	
		if( !m_bSetMoveAni )
		{	
			iAniID = pGrp->GetAnimationIdx( m_LoopMoveAnimation );
			if( iAniID == -1 )
				return;

			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->ClearAllLoopAni( FLOAT100, true );

			pGrp->SetLoopAni( iAniID, FLOAT100 );	
			m_bSetMoveAni = true;
		}
	}
	else
	{
		if( m_bSetMoveAni )
		{
			iAniID = pGrp->GetAnimationIdx( m_LoopDelayAnimation );
			if( iAniID == -1 )
				return;

			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->ClearAllLoopAni( FLOAT100, true );

			pGrp->SetLoopAni( iAniID, FLOAT100 );
			m_bSetMoveAni = false;
		}
	}
}

void ioTargetDebuffSkill6::FindTarget( ioBaseChar *pChar, ioPlayStage* pStage )
{
	if( !pChar ) 
		return;
	if( !pStage ) 
		return;


	TargetList::iterator target_iter = m_TargetList.begin();

	for( ; target_iter != m_TargetList.end(); ++target_iter )
	{
		ioHashString stName = (*target_iter).second.m_szTargetName;
		ioBaseChar* pTarget = pChar->GetBaseChar( stName );
		if( pTarget )
		{
			EffectInfo* pInfo = &(*target_iter).second;
			float fMapHeight = pStage->GetMapHeight( pTarget->GetWorldPosition().x, pTarget->GetWorldPosition().z );
			if(	pTarget->GetWorldPosition().y >	(fMapHeight+2.f) )
				pInfo->m_bAir = true;
			else 
				pInfo->m_bAir = false;
		}
	}

	D3DXQUATERNION qtRot = pChar->GetTargetRot();
	D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vDir, &vDir );

	const BaseCharList &rkCharList = pStage->GetBaseCharList();

	BaseCharList::const_iterator iter;
	for( iter=rkCharList.begin() ; iter!=rkCharList.end() ; ++iter )
	{
		ioBaseChar *pTarget = *iter;

		if( pTarget == pChar )
			continue;

		if( pTarget->GetTeam() == pChar->GetTeam() )
			continue;

		if ( pTarget->IsSystemState() )
			continue;

		bool bEnableTarget = false;
		if( IsEnableTargetState( pTarget, true ) )
			bEnableTarget = true;

		DWORD dwCurTime = FRAMEGETTIME();

		D3DXVECTOR3 vDiff = pTarget->GetMidPositionByRate() - pChar->GetMidPositionByRate();
		float fLengthSq = D3DXVec3LengthSq( &vDiff );

		float fMaxRangeSq = m_fTargetMaxRange * m_fTargetMaxRange;
		float fMinRangeSq = m_fTargetMinRange * m_fTargetMinRange;

		//Already Target Find
		bool bFind = false;
		TargetList::iterator iter = m_TargetList.find( pTarget->GetCharName() );
		if( ( m_TargetList.end() != iter ) )
			bFind = true;

		//Range Test
		bool bRangeAvailable = false;
		if( COMPARE( fLengthSq, fMinRangeSq, fMaxRangeSq ) )
			bRangeAvailable = true;

		//Angle Test
		bool bAngleAvailable = false;
		D3DXVec3Normalize( &vDiff, &vDiff );
		float fValue = D3DXVec3Dot( &vDir, &vDiff );
		float fAngleGap = RADtoDEG( acosf(fValue) );
		if( fAngleGap <= m_fTargetAngle )
			bAngleAvailable = true;

		if( bRangeAvailable && bAngleAvailable )
		{	
			if( !bFind )
			{
				if( !bEnableTarget )
					continue;

				//타겟팅 되는 대상이 스킬 사용자에게 타겟팅 됬음을 알림
				if( m_dwMarkCurCnt < m_dwTargetMaxCnt )
				{
					AddTarget( pStage, pChar, pTarget, dwCurTime, m_bFireMaker );
					m_vAddTargetList.push_back( pTarget->GetCharName() );
				}
			}
			else
			{
				//2012. 6. 25 업데이트 부분 분리 => UpdateTargetList()
			}
		}
		else
		{
			//타겟팅에서 제외 되는 대상이 스킬 사용자에게 알림
			if( bFind )
			{				
				m_vRemoveTargetList.push_back( pTarget->GetCharName() );
			}
		}
	}

	if( 0 < m_TargetList.size() )
		StartScaleInfo();
	else
		ResetScaleInfo();
}

void ioTargetDebuffSkill6::AddTarget( ioPlayStage* pStage, ioBaseChar* pOwner, ioBaseChar* pTarget, DWORD dwCurTime, bool bMarker,	bool CheckList )
{
	if( !pTarget )
		return;

	if( !pOwner )
		return;

	//리스트에 이미 존재하면 패스
	TargetList::iterator iter = m_TargetList.find( pTarget->GetCharName() );
	if( ( m_TargetList.end() != iter ) )
		return;

	//Create Effect
	EffectInfo Info;
	Info.m_dwCreateTime = dwCurTime;
	Info.m_szTargetName = pTarget->GetCharName();
	if( m_bFireMaker )
	{
		CreateFireMarker( pOwner, pStage, Info );
	}
	else
	{
		CreateAimMarker( pOwner, pTarget, pStage, Info );
		CreateChargeMarker( pOwner, pTarget, pStage, Info );
	}

	D3DXVECTOR3 vTargetPos = pTarget->GetWorldPosition();
	float fMapHeight = pStage->GetMapHeight( vTargetPos.x, vTargetPos.z );
	if( vTargetPos.y > (fMapHeight + 2.f) )
		Info.m_bAir = true;
	else
		Info.m_bAir = false;

	//Add Effect
	m_TargetList[pTarget->GetCharName()] = Info;
	m_dwMarkCurCnt++;
}

void ioTargetDebuffSkill6::RemoveTarget( ioBaseChar* pOwner, ioBaseChar* pTarget, bool CheckList )
{
	if( !pTarget )
		return;

	if( !pOwner )
		return;

	if( CheckList )
	{
		//리스트에 없으면 패스
		TargetList::iterator iter = m_TargetList.find( pTarget->GetCharName() );
		if( ( m_TargetList.end() == iter ) )
			return;
	}

	//Destory Effect
	EffectInfo pInfo = m_TargetList[pTarget->GetCharName()];

	DestroyMarker( pOwner, pInfo.m_dwAimEffectID );
	DestroyMarker( pOwner, pInfo.m_dwChargeEffectID );
	DestroyMarker( pOwner, pInfo.m_dwFireEffectID );
	m_TargetList.erase(pTarget->GetCharName());

	m_dwMarkCurCnt--;
}

void ioTargetDebuffSkill6::RemoveTarget( ioBaseChar* pOwner, ioHashString szTargetName )
{
	//리스트에 없으면 패스
	TargetList::iterator iter = m_TargetList.find( szTargetName );
	if( ( m_TargetList.end() == iter ) )
		return;

	//Destory Effect
	EffectInfo pInfo = m_TargetList[szTargetName];

	DestroyMarker( pOwner, pInfo.m_dwAimEffectID );
	DestroyMarker( pOwner, pInfo.m_dwChargeEffectID );
	DestroyMarker( pOwner, pInfo.m_dwFireEffectID );

	m_TargetList.erase( szTargetName );
	m_dwMarkCurCnt--;
}

void ioTargetDebuffSkill6::UpdateTarget( ioBaseChar* pOwner,
	ioBaseChar* pTarget,
	ioPlayStage* pStage,
	bool bMarker )
{
	if( !pTarget )
		return;

	if( !pOwner )
		return;

	TargetList::iterator iter = m_TargetList.find( pTarget->GetCharName() );
	if( ( m_TargetList.end() == iter ) )
		return;

	EffectInfo pInfo = m_TargetList[pTarget->GetCharName()];
	if( m_bFireMaker )
	{
		DestroyMarker( pOwner, pInfo.m_dwAimEffectID );
		DestroyMarker( pOwner, pInfo.m_dwChargeEffectID );

		//이미 존재한다면 함수 내에서 이펙트를 생성하지 않음
		CreateFireMarker( pOwner, pStage, pInfo );
		UpdateFireMarker( pTarget, pStage, pInfo.m_dwFireEffectID );

		m_TargetList[pTarget->GetCharName()] = pInfo;
	}
	else
	{
		DestroyMarker( pOwner, pInfo.m_dwFireEffectID );

		//이미 존재한다면 함수 내에서 이펙트를 생성하지 않음
		CreateAimMarker( pOwner, pTarget, pStage, pInfo );
		CreateChargeMarker( pOwner, pTarget, pStage, pInfo );

		UpdateAimMarker( pTarget, pStage, pInfo.m_dwAimEffectID );
		UpdateChargeMarker( pTarget, pStage, pInfo.m_dwChargeEffectID );	

		m_TargetList[pTarget->GetCharName()] = pInfo;
	}
}

void ioTargetDebuffSkill6::UpdateTargetList( ioBaseChar* pOwner, ioPlayStage* pStage )
{
	TargetList::iterator target_iter;	
	for( target_iter = m_TargetList.begin(); target_iter != m_TargetList.end(); ++target_iter )
	{
		ioHashString szTargetName = (*target_iter).second.m_szTargetName;
		ioBaseChar* pTarget = pStage->GetBaseChar( szTargetName );

		//타겟 유효성 체크
		if( pTarget )
		{			
			if( IsEnableTargetState( pTarget, false ) )
			{
				UpdateTarget( pOwner, pTarget, pStage, m_bFireMaker );					
			}
			else
			{
				//타겟팅에서 제외 되는 대상이 스킬 사용자에게 알림
				m_vRemoveTargetList.push_back( szTargetName );
			}
		}
		else
		{
			m_vRemoveNonTargetList.push_back( szTargetName );
		}
	}

	//유저가 스테이지에 존재하지만 조건에 맞지 않는 경우
	int iSize = m_vRemoveTargetList.size();
	for( int i = 0; i < iSize; ++i )
	{
		ioHashString szTargetName = m_vRemoveTargetList[i];
		ioBaseChar* pTarget = pStage->GetBaseChar( szTargetName );

		if( pTarget )
		{
			RemoveTarget( pOwner, pTarget );
		}
	}

	//유저가 스테이지에 존재하지 않는 경우
	iSize = m_vRemoveNonTargetList.size();
	for( int i = 0; i < iSize; ++i )
	{		
		ioHashString szTargetName = m_vRemoveNonTargetList[i];
		RemoveTarget( pOwner, szTargetName );
	}
}

void ioTargetDebuffSkill6::ResetSendList()
{
	m_vAddTargetList.clear();
	m_vRemoveTargetList.clear();
	m_vRemoveNonTargetList.clear();
}

void ioTargetDebuffSkill6::SendList(  ioBaseChar* pOwner, ioPlayStage* pStage )
{
	int iAddUserSize = (int)m_vAddTargetList.size();
	int iRemoveUserSize = (int)m_vRemoveTargetList.size();
	int iRemoveNonUserSize = (int)m_vRemoveNonTargetList.size();

	if( iAddUserSize <= 0 && iRemoveUserSize <= 0 && iRemoveNonUserSize <= 0 )
		return;

	SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
	kPacket << pOwner->GetCharName();
	kPacket << GetName();
	kPacket << SSC_UPDATE_TARGETLIST;
	kPacket << iAddUserSize;

	if( 0 < iAddUserSize )
	{
		for( int i = 0 ; i < iAddUserSize; ++i )
			kPacket << m_vAddTargetList[i];
	}

	kPacket << iRemoveUserSize;
	if( 0 < iRemoveUserSize )
	{
		for( int i = 0 ; i < iRemoveUserSize; ++i )
			kPacket << m_vRemoveTargetList[i];
	}

	kPacket << iRemoveNonUserSize;
	if( 0 < iRemoveNonUserSize )
	{
		for( int i = 0 ; i < iRemoveNonUserSize; ++i )
			kPacket << m_vRemoveNonTargetList[i];
	}

	P2PNetwork::SendToAllPlayingUser( kPacket );

}

bool ioTargetDebuffSkill6::IsEnableTargetState( ioBaseChar *pTarget, bool bCheckTargetWound, bool bCheckProtected )
{
	ioPlayStage *pStage = pTarget->GetCreator();
	if( !pStage )
		return false;

	if ( !pTarget->IsEnableTarget() )
		return false;

	if( bCheckProtected )
	{	
		if( pTarget->IsProtectState() )
			return false;

		if( pTarget->IsSkillProtected() )
			return false;
	}

	ioPlayMode *pMode = pStage->GetPlayMode();
	if( pMode && pMode->GetModeType() == MT_FOOTBALL )
		return false;

	if( bCheckTargetWound )
	{
		if( !pTarget->CheckTargetWoundType(m_TargetWoundType) )
			return false;
		// 쓰러짐 관련 체크
	}

	return true;
}

void ioTargetDebuffSkill6::CreateAimMarker( ioBaseChar* pChar, ioBaseChar *pTarget, ioPlayStage* pStage, EffectInfo& pInfo )
{		
	if( !pTarget ) return;

	ioMapEffect *pMapEffect = pStage->FindMapEffect( pInfo.m_dwAimEffectID );
	if( !pMapEffect )
	{
		D3DXVECTOR3 vScale( FLOAT1, FLOAT1, FLOAT1 );
		D3DXVECTOR3 vTargetPos = pTarget->GetWorldPosition();
		vTargetPos.y = pTarget->GetMidHeightByRate();

		if( pChar->GetTeam() == TEAM_BLUE  )
			pMapEffect = pStage->CreateMapEffect( m_szBlueAimMarker, vTargetPos, vScale );
		else if( pChar->GetTeam() == TEAM_RED )
			pMapEffect = pStage->CreateMapEffect( m_szRedAimMarker, vTargetPos, vScale );
		else
		{
			if( pChar->GetSingleConvertTeam() == TEAM_BLUE )
				pMapEffect = pStage->CreateMapEffect( m_szBlueAimMarker, vTargetPos, vScale );
			else
				pMapEffect = pStage->CreateMapEffect( m_szRedAimMarker, vTargetPos, vScale );
		}

		if( pMapEffect )
		{	
			pInfo.m_dwAimEffectID = pMapEffect->GetUniqueID();
		}
	}
}

void ioTargetDebuffSkill6::UpdateAimMarker( ioBaseChar *pTarget, ioPlayStage *pStage, UniqueObjID dwEffectID )
{
	D3DXVECTOR3 vTargetPos = pTarget->GetWorldPosition();
	vTargetPos.y = pTarget->GetMidHeightByRate();

	ioMapEffect *pMapEffect = pStage->FindMapEffect( dwEffectID );
	if( pMapEffect )
	{
		pMapEffect->SetWorldPosition( vTargetPos );
	}

}

void ioTargetDebuffSkill6::DestroyMarker( ioBaseChar *pChar, DWORD& dwMarker )
{	
	if( !pChar )
		return;

	if( dwMarker != -1 )
	{
		pChar->EndMapEffect( dwMarker, false );
		dwMarker = -1;
	}
}

void ioTargetDebuffSkill6::CreateChargeMarker(ioBaseChar *pChar, ioBaseChar *pTarget, ioPlayStage* pStage, EffectInfo& pInfo )
{		
	if( !pTarget ) return;
	if( !pStage ) return;

	ioMapEffect *pMapEffect = pStage->FindMapEffect( pInfo.m_dwChargeEffectID );
	if( !pMapEffect )
	{
		D3DXVECTOR3 vScale( m_fCurScaleRate, m_fCurScaleRate, m_fCurScaleRate );
		D3DXVECTOR3 vTargetPos = pTarget->GetWorldPosition();
		vTargetPos.y = pTarget->GetMidHeightByRate();

		if( pChar->IsNeedProcess() )
		{
			pMapEffect = pStage->CreateMapEffect( m_szBlueChargeMarker, vTargetPos, vScale );
		}
		else
		{
			if( pChar->GetTeam() == TEAM_BLUE )
				pMapEffect = pStage->CreateMapEffect( m_szBlueChargeMarker, vTargetPos, vScale );
			else
				pMapEffect = pStage->CreateMapEffect( m_szRedChargeMarker, vTargetPos, vScale );
		}
		if( pMapEffect )
		{
			pInfo.m_dwChargeEffectID = pMapEffect->GetUniqueID();		
		}
	}
}

void ioTargetDebuffSkill6::UpdateChargeMarker( ioBaseChar *pTarget, ioPlayStage *pStage, UniqueObjID dwEffectID )
{
	if( !pTarget ) return;
	if( !pStage ) return;

	float fScaleRate = m_fCurScaleRate;

	D3DXVECTOR3 vScale( fScaleRate, fScaleRate, fScaleRate );
	D3DXVECTOR3 vTargetPos = pTarget->GetWorldPosition();
	vTargetPos.y = pTarget->GetMidHeightByRate();

	ioMapEffect *pMapEffect = pStage->FindMapEffect( dwEffectID );
	if( pMapEffect )
	{
		pMapEffect->SetWorldPosition( vTargetPos );
		ioEffect* pEffect = pMapEffect->GetEffect();
		if( pEffect )
		{
			pEffect->GetParentNode()->SetScale( vScale );
		}
	}
}

void ioTargetDebuffSkill6::UpdateAllMarker( ioBaseChar *pChar, ioPlayStage *pStage )
{
	TargetList::iterator target_iter = m_TargetList.begin();
	for( ; target_iter != m_TargetList.end(); ++target_iter )
	{
		ioBaseChar* pTarget = pStage->GetBaseChar( (*target_iter).second.m_szTargetName );
		if( pTarget )
		{
			UpdateFireMarker( pTarget, pStage, (*target_iter).second.m_dwFireEffectID );
			UpdateAimMarker( pTarget, pStage, (*target_iter).second.m_dwAimEffectID );
			UpdateChargeMarker(  pTarget, pStage, (*target_iter).second.m_dwChargeEffectID );
		}
	}
}

void ioTargetDebuffSkill6::DestroyAllMarker( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar )
		return;

	DestroyGuideMarker( pChar );

	TargetList::iterator iter = m_TargetList.begin();
	for( ; iter != m_TargetList.end(); ++iter )
	{	
		EffectInfo pInfo = (*iter).second;

		if( pInfo.m_dwAimEffectID != -1 )
		{			
			DestroyMarker( pChar, pInfo.m_dwAimEffectID );
			pInfo.m_dwAimEffectID = -1;
		}

		if( pInfo.m_dwChargeEffectID != -1 )
		{
			DestroyMarker( pChar, pInfo.m_dwChargeEffectID );
			pInfo.m_dwChargeEffectID = -1;
		}

		if( pInfo.m_dwFireEffectID != -1 )
		{
			DestroyMarker( pChar, pInfo.m_dwFireEffectID );
			pInfo.m_dwFireEffectID = -1;
		}

		(*iter).second = pInfo;
	}
}

void ioTargetDebuffSkill6::ClearTargetList( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar || !pStage )
		return;

	TargetList::iterator iter = m_TargetList.begin();
	for( ; iter != m_TargetList.end(); ++iter )
	{	
		EffectInfo pInfo = (*iter).second;

		if( pInfo.m_dwAimEffectID != -1 )
		{			
			DestroyMarker( pChar, pInfo.m_dwAimEffectID );
			pInfo.m_dwAimEffectID = -1;
		}

		if( pInfo.m_dwChargeEffectID != -1 )
		{
			DestroyMarker( pChar, pInfo.m_dwChargeEffectID );
			pInfo.m_dwChargeEffectID = -1;
		}

		if( pInfo.m_dwFireEffectID != -1 )
		{
			DestroyMarker( pChar, pInfo.m_dwFireEffectID );
			pInfo.m_dwFireEffectID = -1;
		}

		(*iter).second = pInfo;
	}

	m_TargetList.clear();
}

void ioTargetDebuffSkill6::SetEndState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	m_SkillState = SS_END;

	DestroyAllMarker( pChar, pStage );

	if( m_bSkillInJump )
		pChar->SetCurMoveSpeed( 0.0f );
}

void ioTargetDebuffSkill6::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{	
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	if( !pStage )
		return;

	ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
	if( pZone )
	{
		ioBaseChar *pChar = pZone->GetOwner();
		if( pChar )
		{
			D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			pZone->SetCollisionDir( vDir );
			pZone->SetZoneValue( true, true );
		}
	}

	m_bSipping = !m_bSipping;
}

void ioTargetDebuffSkill6::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{

	DWORD dwCurTime = FRAMEGETTIME();
	int iType;
	rkPacket >> iType;


	switch( iType )
	{
	case SSC_UPDATE_TARGETLIST:
		{
			int iAddUserSize = 0;
			int iRemoveUserSize = 0;
			int iRemoveNonUserSize = 0;

			rkPacket >> iAddUserSize;
			if( 0 < iAddUserSize )
			{
				for( int i = 0; i <iAddUserSize; ++i )
				{
					ioHashString szCharName;
					rkPacket >> szCharName;

					ioBaseChar* pTarget = pStage->GetBaseChar( szCharName );
					if( pTarget )
						AddTarget( pStage, pChar, pTarget, dwCurTime, m_bFireMaker, true );
				}
			}

			rkPacket >> iRemoveUserSize;
			if( 0 < iRemoveUserSize )
			{
				for( int i = 0; i < iRemoveUserSize; ++i ) 
				{
					ioHashString szCharName;
					rkPacket >> szCharName;

					ioBaseChar* pTarget = pStage->GetBaseChar( szCharName );
					if( pTarget )
						RemoveTarget( pChar, pTarget, true );
				}
			}

			rkPacket >> iRemoveNonUserSize;
			if( 0 < iRemoveNonUserSize )
			{
				for( int i = 0; i < iRemoveNonUserSize; ++i ) 
				{
					ioHashString szCharName;
					rkPacket >> szCharName;					
					RemoveTarget( pChar, szCharName );
				}
			}
		}
		break;
	case SSC_AIM_FIRE:
		{
			SetAimFireState( pChar, pStage );
			int nTagetCnt;
			ioHashString stName;
			bool bAir;
		    rkPacket >> nTagetCnt;
			for( int i = 0; i < nTagetCnt; i++ )
			{
				rkPacket >> stName;
				rkPacket >> bAir;

				TargetList::iterator iter;
				iter = m_TargetList.find( stName );
				if( iter != m_TargetList.end() )
				{
					(*iter).second.m_bAir = bAir;
				}
			}
		}
		break;
	case SSC_ROTATE:
		{
			bool bChangeDir, bChangeUpDown;
			rkPacket >> bChangeDir >> bChangeUpDown;

			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;

			int iDir;
			rkPacket >> iDir;

			bool bSetUpKey, bSetDownKey;//, bOptionMove;
			rkPacket >> bSetUpKey >> bSetDownKey;// >> bOptionMove;

			pChar->SetTargetRot( qtRot );

			if( bChangeDir )
			{
				m_PreDirKey = m_CurDirKey;
				m_CurDirKey = (ioUserKeyInput::DirKeyInput)iDir;

				//if( !bOptionMove )
					ChangeFlyMove( pChar );
			}

			if( bChangeUpDown )
			{
				m_bSetUpKey = bSetUpKey;
				m_bSetDownKey = bSetDownKey;
			}
		}
		break;
	/*case SSC_AIM_MOVE_ATTACK:
		{
			rkPacket >> m_vOwnerAttackDir;
			pChar->SetTargetRotToDir( m_vOwnerAttackDir, false );
			SetAimMoveAttackState( pChar, pStage );
		}
		break;*/
	/*case SSC_JUMPPING:
		{
			SetNextJumpState( pChar, pStage );
		}
		break;*/
	case SSC_END:
		{
			SetEndState( pChar, pStage );
		}
		break;
	case SSC_UPDATE_ONLYONE_TARGET:
		{
			ioHashString ioNewTarget;
			rkPacket >> ioNewTarget;
			if( !ioNewTarget.IsEmpty() )
			{
				// 새로운 타겟일때만
				if ( m_OnlyOneTarget != ioNewTarget )
				{
					// 기존 타겟마커 제거
					ioBaseChar* pTarget = pStage->GetBaseChar( m_OnlyOneTarget );
					if ( pTarget )
						RemoveTarget( pChar, pTarget );

					m_OnlyOneTarget = ioNewTarget;
					ioBaseChar* pNewTarget = pStage->GetBaseChar( m_OnlyOneTarget );
					if ( pNewTarget )
					{
						DWORD dwCurTime = FRAMEGETTIME();
						AddTarget( pStage, pChar, pNewTarget, dwCurTime, m_bFireMaker );
					}
				}
			}
			else
			{
				// 공격 가능한 타겟이 존재하지 않으나 기존에 타겟이 있을 경우 기존 타겟 마커 삭제
				if ( !m_OnlyOneTarget.IsEmpty() )
				{
					ioBaseChar* pTarget = pStage->GetBaseChar( m_OnlyOneTarget );
					if ( pTarget )
						RemoveTarget( pChar, pTarget );

					m_OnlyOneTarget.Clear();
				}
			}
		}
		break;
	}
}


bool ioTargetDebuffSkill6::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

bool ioTargetDebuffSkill6::IsProtected( int iDefenseBreakType ) const
{
	if( m_bDisableSkillProtect )
		return false;

	if( DBT_NONE != (DefenseBreakType)m_ProtectCancelType )
	{
		if( iDefenseBreakType == m_ProtectCancelType )   // 스킬 무적 파괴
			return false;
	}

	/*if( IsPreDelayProtect() )
	{
		if( m_dwPreDelayStartTime != 0 )
			return true;
	}*/

	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_SkillState )
	{
	case SS_START:
		return true;

	case SS_DELAY:
		{
			if( 0 < m_dwStartSkillProtectedTime && 
				dwCurTime < m_dwStartSkillProtectedTime + m_dwSkillProtectedTime )
				return true;
		}
		break;
	/*case SS_AIM_FIRE:				
		if( 0 < m_dwStartSkillProtectedTime && 
			dwCurTime < m_dwStartSkillProtectedTime + m_dwSkillProtectedTime )
			return true;
		else
			if( 0 < m_dwFireMotionEndTime && dwCurTime <= m_dwFireMotionEndTime )
				return true;*/

	}

	return false;
}

bool ioTargetDebuffSkill6::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	return false;

	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwPreDelayStartTime > 0 )
		return false;

	switch( m_SkillState )
	{
	case SS_START:
	case SS_DELAY:
//	case SS_ZERO_AIM_FIRE:
	case SS_AIM_FIRE:
		return false;
	case SS_END:
		return true;
	}

	return true;
}

bool ioTargetDebuffSkill6::IsEndState() const
{
	if( m_SkillState == SS_END )
		return true;

	return false;
}

/*void ioTargetDebuffSkill6::SetAimMoveAttackState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar ) return;
	if( !pStage ) return;
	m_SkillState = SS_AIM_MOVE_ATTACK;

	if ( m_NoTragetAimEffectID != -1 )
	{
		ioMapEffect *pMapEffect = pStage->FindMapEffect( m_NoTragetAimEffectID );
		if ( pMapEffect )
		{
			pChar->EndMapEffect( m_NoTragetAimEffectID, false );
			m_NoTragetAimEffectID = -1;
		}
	}

	DWORD dwCurTime = FRAMEGETTIME();
	ioEntityGroup *pGrp = pChar->GetGroup();

	int iAniID = 0;
	iAniID = pGrp->GetAnimationIdx(	m_AimAttackAttribute.m_AttackAnimation );
	if( iAniID == -1 )
		return;

	float fTimeRate = m_AimAttackAttribute.m_fAttackAniRate * m_fExtraAniRate;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );
	DWORD dwPreDelay = m_AimAttackAttribute.m_dwPreDelay;
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay );
	m_dwSkillAimMotionEnd = dwCurTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	pChar->SetSkillMoveEnable( false, false );

	m_iCurAttackCount++;
	if ( pChar->IsNeedProcess() )
	{
		m_dwAimAttackStartTime = dwCurTime;

		if ( !m_OnlyOneTarget.IsEmpty() )
		{
			ioBaseChar* pTarget = pStage->GetBaseChar( m_OnlyOneTarget );
			if( pTarget )
			{
				m_vOwnerAttackDir = pTarget->GetWorldPosition() - pChar->GetWorldPosition();
				D3DXVec3Normalize( &m_vOwnerAttackDir, &m_vOwnerAttackDir );
			}
		}
		else
		{
			D3DXVECTOR3 vCharDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVec3Normalize( &vCharDir, &vCharDir );

			D3DXVECTOR3 vAxis;
			D3DXVec3Cross( &vAxis, &vCharDir, &-(ioMath::UNIT_Y ) );
			D3DXVec3Normalize( &vAxis, &vAxis );

			D3DXQUATERNION qtNewRot;
			float fCurAngle = ioMath::ArrangeHead( m_fOwnerDefaultAttackAngle );
			D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fCurAngle) );

			m_vOwnerAttackDir = qtNewRot * vCharDir;
			D3DXVec3Normalize( &m_vOwnerAttackDir, &m_vOwnerAttackDir );
		}

		pChar->SetTargetRotToDir( m_vOwnerAttackDir, false );
	}


	m_fCurMoveDist = 0;

	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();
	pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex,
		iAniID,
		m_AimAttackAttribute.m_vWeaponInfoList,
		FTT_SKILL_ATTACK,
		fTimeRate,
		dwPreDelay );

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SSC_AIM_MOVE_ATTACK;
		kPacket << m_vOwnerAttackDir;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	if( m_bSkillAimAttackGauge )
		pChar->ReduceCurSkillGaugeBySkill( GetOwnerItem(), m_fReduceAttackSkillGauge );
}*/

bool ioTargetDebuffSkill6::CheckMove( ioBaseChar *pChar )
{
	if( !pChar ) return false;

	ioPlayStage *pStage = pChar->GetCreator();
	if( !pStage ) return false;

	//max 거리 체크
	if ( m_fMaxMoveDist > 0 && m_fCurMoveDist >= m_fMaxMoveDist )
		return false;

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	bool bCurMove = false;
	D3DXVECTOR3 vTotalMove( 0.0f, 0.0f, 0.0f );

	float fCurTimeGap = fTimePerSec * FLOAT1000;

	D3DXVECTOR3 vDir;
	vDir = m_vOwnerAttackDir;
	D3DXVec3Normalize( &vDir, &vDir );


	while( fCurTimeGap > 0.0f )
	{
		float fDivideTimePerSec = min( fCurTimeGap, MIN_FRAME_TIME ) * 0.001f;
		fCurTimeGap -= MIN_FRAME_TIME;

		D3DXVECTOR3 vMoveAmt;

		if( m_fMoveAttackSpeed > 0.0f && vDir != ioMath::VEC3_ZERO )
		{
			float fFinalMoveAmt = m_fMoveAttackSpeed * fDivideTimePerSec;
			if( fFinalMoveAmt > 0.0f )
			{
				vMoveAmt = vDir * fFinalMoveAmt;
				vTotalMove += vMoveAmt;
				bCurMove = true;
			}
		}

		if( pChar->ProcessForceMove( fDivideTimePerSec, vMoveAmt ) )
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
			float fPreHeight = pChar->GetBottomHeight();

			pChar->Translate( vTotalMove );

			D3DXVECTOR3 vDiff = ioMath::VEC3_ZERO;
			vDiff = vTotalMove - vDiff;
			float fMoveDist = D3DXVec3Length( &vDiff );
			m_fCurMoveDist += fMoveDist;

			if( pChar->GetForceAmt() != 0.0f )	// 빠른 ForceMove로 경사면을 통과하는 경우에 대한 보강
			{
				float fMoveHeight = pChar->GetBottomHeight();

				if( fPreHeight > 0.0f && fMoveHeight <= FLOAT1 )
				{
					D3DXVECTOR3 vCurPos = pChar->GetWorldPosition();

					float fTopHeight = pStage->GetMapHeight( vCurPos.x,
						vCurPos.z,
						pChar,
						false,
						pChar->GetMidHeightByRate( FLOAT1 ) );

					if( fTopHeight > FLOAT1 )
					{
						vCurPos.y = max( fTopHeight, fPreHeight );
						pChar->SetWorldPosition( vCurPos );
					}
				}
			}
		}
		if ( bCol )
			return false;
	}

	D3DXVECTOR3 vPos = pChar->GetWorldPosition();
	float fPreHeight = pChar->GetBottomHeight();

	if( vPos.y < fPreHeight )
	{
		vPos.y = fPreHeight;
		return false;
	}

	if( pStage && pChar->IsAxisLimit() )
	{
		pStage->CheckAxisLimit( &vPos );
		pChar->SetWorldPosition( vPos );
	}

	return true;
}

void ioTargetDebuffSkill6::SetNextSkillState( ioBaseChar *pWounder, ioWeapon *pWeapon /*= NULL */ )
{
	if ( !pWounder || !pWeapon )
		return;

	ioBaseChar *pAttacker = pWeapon->GetOwner();
	if ( !pAttacker )
		return;

	ioPlayStage *pStage = pAttacker->GetCreator();
	if( !pStage )
		return;

	/*SetNextJumpState( pAttacker, pStage );
	if ( pWounder->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pAttacker->GetCharName();
		kPacket << GetName();
		kPacket << SSC_JUMPPING;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}*/
}



bool ioTargetDebuffSkill6::ProcessJumpping( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	D3DXVECTOR3 vTargetDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vTargetDir, &vTargetDir );

	D3DXVECTOR3 vAxis;
	D3DXVec3Cross( &vAxis, &vTargetDir, &ioMath::UNIT_Y );
	D3DXVec3Normalize( &vAxis, &vAxis );

	D3DXQUATERNION qtNewRot;
	float fCurAngle = ioMath::ArrangeHead( m_fJumpAngle );
	D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fCurAngle) );

	D3DXVECTOR3 vMoveDir = qtNewRot * vTargetDir;
	D3DXVec3Normalize( &vMoveDir, &vMoveDir );

	if( vMoveDir == ioMath::VEC3_ZERO )
		return false;

	if( m_dwJumppingDuration == 0 )
		return false;

	float fCurSpeed = m_fJumpMaxRange / (float)m_dwJumppingDuration * FLOAT1000;
	fCurSpeed = max( 0.0f, fCurSpeed );

	float fDistance = fCurSpeed * fTimePerSec;

	bool bRangeLimit = false;
	D3DXVECTOR3 vMoveAmt( 0.0f, 0.0f, 0.0f );
	if( m_fCurJumpRange + fDistance < m_fJumpMaxRange )
	{
		bRangeLimit = true;
		vMoveAmt = vMoveDir * fDistance;

		m_fCurJumpRange += fDistance;
	}
	else
	{
		bRangeLimit = false;

		float fGapDistance = m_fJumpMaxRange - m_fCurJumpRange;
		vMoveAmt = vMoveDir * fGapDistance;
	}

	bool bCol = false;
	if( pStage && pStage->ReCalculateMoveVectorByTerrain( pChar, &vMoveAmt, bCol ) )
		pChar->Translate( vMoveAmt );

	D3DXVECTOR3 vPos = pChar->GetWorldPosition();
	if( pChar->CheckAxisLimit( &vPos ) )
		pChar->SetWorldPosition( vPos );

	return bRangeLimit;
}

void ioTargetDebuffSkill6::SetNoneTargetAimEffect( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if ( !pChar || !pStage || m_AimTargetAttackType != ATAT_TARGET_FIRE )
		return;

	if ( !m_TargetList.empty() )
	{
		ioMapEffect *pMapEffect = pStage->FindMapEffect( m_dwNoneTargetAimEffectID );
		if ( pMapEffect )
		{
			pChar->EndMapEffect( m_dwNoneTargetAimEffectID, false );
			m_dwNoneTargetAimEffectID = -1;
		}
	}
	else
	{
		D3DXVECTOR3 vScale( FLOAT1, FLOAT1, FLOAT1 );
		D3DXVECTOR3 vPos = pChar->GetMidPositionByRate();
		vPos += pChar->GetWorldOrientation() * m_vNoneTargetAimOffset;

		if ( m_dwNoneTargetAimEffectID != -1 )
		{
			ioMapEffect *pMapEffect = pStage->FindMapEffect( m_dwNoneTargetAimEffectID );
			if ( pMapEffect )
				pMapEffect->SetWorldPosition( vPos );
		}
		else
		{
			ioMapEffect *pMapEffect = NULL;
			if( pChar->GetTeam() == TEAM_BLUE )
				pMapEffect = pStage->CreateMapEffect( m_szBlueAimMarker, vPos, vScale );
			else if( pChar->GetTeam() == TEAM_RED )
				pMapEffect = pStage->CreateMapEffect( m_szRedAimMarker, vPos, vScale );
			else
			{
				if( pChar->GetSingleConvertTeam() == TEAM_BLUE )
					pMapEffect = pStage->CreateMapEffect( m_szBlueAimMarker, vPos, vScale );
				else
					pMapEffect = pStage->CreateMapEffect( m_szRedAimMarker, vPos, vScale );
			}

			if ( pMapEffect )
				m_dwNoneTargetAimEffectID = pMapEffect->GetUniqueID();
		}
	}
}

void ioTargetDebuffSkill6::ProcessRotate( ioBaseChar *pOwner )
{
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


bool ioTargetDebuffSkill6::IsNoDropState() const
{
	return true;
}