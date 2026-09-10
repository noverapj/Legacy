

#include "stdafx.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"
#include "ioGrowthUpInfo.h"

#include "ioTargetTeleportSkill2.h"

#include "WeaponDefine.h"

ioTargetTeleportSkill2::ioTargetTeleportSkill2()
{
	ClearData();
}

ioTargetTeleportSkill2::ioTargetTeleportSkill2( const ioTargetTeleportSkill2 &rhs )
: ioNormalSkill( rhs ),
m_bDisableGrowth( rhs.m_bDisableGrowth ),
m_fGuagePerTic( rhs.m_fGuagePerTic ),
m_dwTicTime( rhs.m_dwTicTime ),
m_LoopDelayAnimation( rhs.m_LoopDelayAnimation ),
m_fLoopDelayAnimationRate( rhs.m_fLoopDelayAnimationRate ),
m_JumpLoopDelayAnimation( rhs.m_JumpLoopDelayAnimation ),
m_fJumpLoopDelayAnimationRate( rhs.m_fJumpLoopDelayAnimationRate ),
m_LoopMoveAnimation( rhs.m_LoopMoveAnimation ),
m_NormalTargetSearch( rhs.m_NormalTargetSearch ),
m_JumpTargetSearch( rhs.m_JumpTargetSearch ),
m_fMaxScaleRate( rhs.m_fMaxScaleRate ),
m_fMinScaleRate( rhs.m_fMinScaleRate ),
m_RedNoTargetMarker( rhs.m_RedNoTargetMarker ),
m_RedNormalTargetMarker( rhs.m_RedNormalTargetMarker ),
m_RedSpecialTargetMarker( rhs.m_RedSpecialTargetMarker ),
m_RedFireTargetMarker( rhs.m_RedFireTargetMarker ),
m_BlueNoTargetMarker( rhs.m_BlueNoTargetMarker ),
m_BlueNormalTargetMarker( rhs.m_BlueNormalTargetMarker ),
m_BlueSpecialTargetMarker( rhs.m_BlueSpecialTargetMarker ),
m_BlueFireTargetMarker( rhs.m_BlueFireTargetMarker ),
m_dwNormalTargetDuration( rhs.m_dwNormalTargetDuration ),
m_dwSpecialTargetDuration( rhs.m_dwSpecialTargetDuration ),
m_dwSkillProtectedTime( rhs.m_dwSkillProtectedTime ),
m_SkillMapEffect( rhs.m_SkillMapEffect ),
m_TargetTeleportStartAni( rhs.m_TargetTeleportStartAni ),
m_fTargetTeleportStartAniRate( rhs.m_fTargetTeleportStartAniRate ),
m_TargetTeleportEndAni( rhs.m_TargetTeleportEndAni ),
m_fTargetTeleportEndAniRate( rhs.m_fTargetTeleportEndAniRate ),
m_NoTargetAttackCombo( rhs.m_NoTargetAttackCombo ),
m_NormalTargetAttackCombo( rhs.m_NormalTargetAttackCombo ),
m_SpecialTargetAttackCombo( rhs.m_SpecialTargetAttackCombo ),
m_fTeleportOffSet( rhs.m_fTeleportOffSet )
{
	ClearData();
}

ioTargetTeleportSkill2::~ioTargetTeleportSkill2()
{
}

void ioTargetTeleportSkill2::ClearData()
{
	m_SkillState = SS_NONE;
	m_SkillSubState = SSS_NONE;

	m_CurTargetInfo.Init();

	m_dwChangeSubStateTime = 0;

	// combo attack
	m_dwNextComboTime = 0;
	m_dwComboEndTime = 0;

	m_iCurComboCnt = 0;

	m_bSetHandMesh = false;
	m_bSetMoveAni = false;

	// skill gauge
	m_dwCheckTime = 0;
	m_fTicGaugeRateByGrowth = FLOAT1;
	m_dwTicTimeByGrowth = 0;

	//maker
	m_dwNoTargetMarkerID = 0;

	// protected
	m_dwStartSkillProtectedTime = 0;

	// teleport
	m_dwTargetTeleportStartTime = 0;
	m_dwTargetTeleportEndTime = 0;
}

void ioTargetTeleportSkill2::LoadProperty( ioINILoader &rkLoader )
{
	ioNormalSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];
	char szKey[MAX_PATH];

	// gauge 
	m_bDisableGrowth = rkLoader.LoadBool_e( "disable_growth", false );
	m_fGuagePerTic = rkLoader.LoadFloat_e( "gauge_per_tick", 0.0f );
	m_dwTicTime = (DWORD)rkLoader.LoadInt_e( "tick_time", 0 );

	//loop motion
	rkLoader.LoadString_e( "delay_animation", "", szBuf, MAX_PATH );	
	m_LoopDelayAnimation = szBuf;
	m_fLoopDelayAnimationRate = rkLoader.LoadFloat_e( "delay_animation_rate", FLOAT1 );

	rkLoader.LoadString_e( "jump_delay_animation", "", szBuf, MAX_PATH );	
	m_JumpLoopDelayAnimation = szBuf;
	m_fJumpLoopDelayAnimationRate = rkLoader.LoadFloat_e( "jump_delay_animation_rate", FLOAT1 );

	rkLoader.LoadString_e( "move_animation", "", szBuf, MAX_PATH );
	m_LoopMoveAnimation	= szBuf;

	// target search range
	m_NormalTargetSearch.Init();
	m_JumpTargetSearch.Init();

	m_NormalTargetSearch.m_TargetWoundType = (TargetWoundType)rkLoader.LoadInt_e( "target_wound_type", TWT_ALL );
	m_NormalTargetSearch.m_fTargetAngle = rkLoader.LoadFloat_e( "target_angle", 0.0f );
	m_NormalTargetSearch.m_fTargetMaxRange = rkLoader.LoadFloat_e( "target_max_range", 0.0f );
	m_NormalTargetSearch.m_fTargetMinRange = rkLoader.LoadFloat_e( "target_min_range", 0.0f );

	m_JumpTargetSearch.m_TargetWoundType = (TargetWoundType)rkLoader.LoadInt_e( "target_jump_wound_type", TWT_ALL );
	m_JumpTargetSearch.m_fTargetAngle = rkLoader.LoadFloat_e( "target_jump_angle", 0.0f );
	m_JumpTargetSearch.m_fTargetMaxRange = rkLoader.LoadFloat_e( "target_jump_max_range", 0.0f );
	m_JumpTargetSearch.m_fTargetMinRange = rkLoader.LoadFloat_e( "target_jump_min_range", 0.0f );

	// attack combo
	m_NoTargetAttackCombo.clear();
	m_NormalTargetAttackCombo.clear();
	m_SpecialTargetAttackCombo.clear();

	int iAttackComboCnt = rkLoader.LoadInt_e( "no_target_combo_cnt", 0 );
	for( int i=0; i < iAttackComboCnt; ++i )
	{
		AttackAttribute kAttack;
		wsprintf_e( szKey, "no_target_combo%d_attack", i+1 );

		LoadAttackAttribute( szKey, kAttack, rkLoader );

		m_NoTargetAttackCombo.push_back( kAttack );
	}

	iAttackComboCnt = rkLoader.LoadInt_e( "normal_target_combo_cnt", 0 );
	for( int i=0; i < iAttackComboCnt; ++i )
	{
		AttackAttribute kAttack;
		wsprintf_e( szKey, "normal_target_combo%d_attack", i+1 );

		LoadAttackAttribute( szKey, kAttack, rkLoader );

		m_NormalTargetAttackCombo.push_back( kAttack );
	}

	iAttackComboCnt = rkLoader.LoadInt_e( "special_target_combo_cnt", 0 );
	for( int i=0; i < iAttackComboCnt; ++i )
	{
		AttackAttribute kAttack;
		wsprintf_e( szKey, "special_target_combo%d_attack", i+1 );

		LoadAttackAttribute( szKey, kAttack, rkLoader );

		m_SpecialTargetAttackCombo.push_back( kAttack );
	}

	// 무적
	m_dwSkillProtectedTime = (DWORD)rkLoader.LoadInt_e( "skill_protect_time", 0 );

	// teleport
	rkLoader.LoadString_e( "teleport_start_ani", "", szBuf, MAX_PATH );
	m_TargetTeleportStartAni = szBuf;
	m_fTargetTeleportStartAniRate = rkLoader.LoadFloat_e( "teleport_start_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "teleport_end_ani", "", szBuf, MAX_PATH );
	m_TargetTeleportEndAni = szBuf;
	m_fTargetTeleportEndAniRate = rkLoader.LoadFloat_e( "teleport_end_ani_rate", FLOAT1 );

	m_fTeleportOffSet = rkLoader.LoadFloat_e( "teleport_offset", 0.0f );

	// target mark
	rkLoader.LoadString_e( "red_no_target_marker", "", szBuf, MAX_PATH );
	m_RedNoTargetMarker = szBuf;
	rkLoader.LoadString_e( "red_nomal_target_marker", "", szBuf, MAX_PATH );
	m_RedNormalTargetMarker = szBuf;
	rkLoader.LoadString_e( "red_special_target_marker", "", szBuf, MAX_PATH );
	m_RedSpecialTargetMarker = szBuf;
	rkLoader.LoadString_e( "red_fire_target_marker", "", szBuf, MAX_PATH );
	m_RedFireTargetMarker = szBuf;

	rkLoader.LoadString_e( "blue_no_target_marker", "", szBuf, MAX_PATH );
	m_BlueNoTargetMarker = szBuf;
	rkLoader.LoadString_e( "blue_nomal_target_marker", "", szBuf, MAX_PATH );
	m_BlueNormalTargetMarker = szBuf;
	rkLoader.LoadString_e( "blue_special_target_marker", "", szBuf, MAX_PATH );
	m_BlueSpecialTargetMarker = szBuf;
	rkLoader.LoadString_e( "blue_fire_target_marker", "", szBuf, MAX_PATH );
	m_BlueFireTargetMarker = szBuf;

	m_dwNormalTargetDuration = (DWORD)rkLoader.LoadInt_e( "normal_target_duration", 0 );
	m_dwSpecialTargetDuration = (DWORD)rkLoader.LoadInt_e( "special_target_duration", 0 );

	m_fMaxScaleRate = rkLoader.LoadFloat_e( "normal_target_max_scale", FLOAT1 );
	m_fMinScaleRate = rkLoader.LoadFloat_e( "normal_target_min_scale", FLOAT1 );

	m_fMaxScaleRate = max( m_fMinScaleRate, m_fMaxScaleRate );
	m_fMinScaleRate = min( m_fMinScaleRate, m_fMaxScaleRate );

	rkLoader.LoadString_e( "skill_map_effect", "", szBuf, MAX_PATH );
	m_SkillMapEffect = szBuf;
}

ioSkill* ioTargetTeleportSkill2::Clone()
{
	return new ioTargetTeleportSkill2( *this );
}

bool ioTargetTeleportSkill2::OnSkillStart( ioBaseChar *pChar )
{
	if( !pChar )
		return false;

	ClearData();

	//
	pChar->SetSkillProtection();

	m_dwEnableSwitchTime = 0;
	m_dwEnableSemiSwitchTime = 0;

	m_SkillSoundID = 0;

	if( m_dwSwitchSkill > 0 )
		m_dwEnableSwitchTime = FRAMEGETTIME() + m_dwSwitchSkill;
	if( m_dwSemiSwitchSkill > 0 )
		m_dwEnableSemiSwitchTime = FRAMEGETTIME() + m_dwSemiSwitchSkill;

	m_dwSkillStartTime = FRAMEGETTIME();

	if( !StartPreDelayAnimation( pChar ) )	// Delay Animation이 없다
	{
		SetNormalSkill( pChar );
	}

	CheckSkillDesc( pChar );

	if( m_bInitJumpPower )
	{
		pChar->SetCurJumpPowerAmt( 0.0f );
		pChar->SetGravityAmt( 0.0f );
	}

	SetSkillNameChatBubble( pChar );
	//

	m_SkillState = SS_PRE;
	m_dwCheckTime = FRAMEGETTIME();

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
	
	return true;
}

void ioTargetTeleportSkill2::OnSkillEnd( ioBaseChar *pChar )
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

	ioPlayStage* pStage = g_SkillMaker.GetPlayStage();
	DestroyAlllMaker( pStage );

	ClearData();
}

void ioTargetTeleportSkill2::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar || !pStage )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	float fCurTicGauge = m_fGuagePerTic / m_fTicGaugeRateByGrowth;
	int iTickGap = (int)( dwCurTime - m_dwCheckTime );
	int iCurTicTime = m_dwTicTime + m_dwTicTimeByGrowth;

	switch( m_SkillState )
	{
	case SS_PRE:
		{
			DWORD dwPreDelay = GetPreDelayTime();
			if( m_dwPreDelayStartTime + dwPreDelay < dwCurTime )
			{
				m_dwPreDelayStartTime = 0;

				EndPreDelayEffect( pChar );
				SetNormalSkill( pChar );
			}
		}
		break;
	case SS_LOOP:
		{
			// skillgauge 체크
			if( !CheckSkillGauge( pChar ) )
			{
				SetEndState( pChar, pStage );
				return;
			}

			// 이동 모션 체크
			CheckNormalMoveAni( pChar );

			// target check
			UpdateTargetList( pChar, pStage );
			FindTarget( pChar, pStage );

			// substate 체크
			CheckSkillSubState( pChar );

			// marker 체크
			UpdateMarker( pChar, pStage );
		
			// Key 입력 체크
			if( pChar->IsNeedProcess() )
			{
				if( pChar->IsAttackKey() )
					SetTargetTeleportStart( pChar, pStage );
				else if( pChar->IsDefenseKey() )
					ChangeTarget( pChar, pStage );
			}
		}
		break;
	case SS_TELEPORT_S:
		OnTargetTeleportStart( pChar, pStage );
		break;
	case SS_TELEPORT_E:
		OnTargetTeleportEnd( pChar, pStage );
		break;
	case SS_NO_TARGET_FIRE:
	case SS_NORMAL_FIRE:
	case SS_SPECIAL_FIRE:
		{
			if( m_dwComboEndTime > 0 && m_dwComboEndTime < dwCurTime )
			{
				SetEndState( pChar, pStage );
				return;
			}
			else
			{
				OnTargetComboState( pChar, pStage );
			}
		}
		break;
	case SS_END:
		break;
	}
}

bool ioTargetTeleportSkill2::CheckSkillGauge( ioBaseChar *pChar )
{
	if( !pChar )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();

	float fCurTicGauge = m_fGuagePerTic / m_fTicGaugeRateByGrowth;
	int iTickGap = (int)( dwCurTime - m_dwCheckTime );
	int iCurTicTime = m_dwTicTime + m_dwTicTimeByGrowth;

	if( iTickGap >= iCurTicTime )
	{
		m_dwCheckTime = dwCurTime - ( iTickGap - iCurTicTime );

		if( fCurTicGauge != 0.0f )
		{				
			if( pChar->ReduceCurSkillGaugeBySkill( GetOwnerItem(), fCurTicGauge ) <= 0.0f )
			{
				return false;
			}
		}
	}

	return true;
}

void ioTargetTeleportSkill2::CheckSkillSubState( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( m_CurTargetInfo.m_TargetName.IsEmpty() )
	{
		if( m_SkillSubState != SSS_NONE )
		{
			ChangeSkillSubState( pChar, SSS_NONE );
		}
	}
	else
	{
		switch( m_SkillSubState )
		{
		case SSS_NONE:
			ChangeSkillSubState( pChar, SSS_NORMAL_TARGET );
			break;
		case SSS_NORMAL_TARGET:
			if( m_dwChangeSubStateTime + m_dwNormalTargetDuration < dwCurTime )
			{
				ChangeSkillSubState( pChar, SSS_SPECIAL_TARGET );
				ChangeTargetMarker( pChar );
			}
			break;
		case SSS_SPECIAL_TARGET:
			if( m_dwChangeSubStateTime + m_dwSpecialTargetDuration < dwCurTime )
			{
				ChangeSkillSubState( pChar, SSS_NORMAL_TARGET );
				ChangeTargetMarker( pChar );
			}
			break;
		}
	}
}

void ioTargetTeleportSkill2::ChangeSkillSubState( ioBaseChar *pChar, SkillSubState eState )
{
	if( !pChar )
		return;

	m_SkillSubState = eState;
	m_dwChangeSubStateTime = FRAMEGETTIME();
}

void ioTargetTeleportSkill2::UpdateMarker( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar ) return;
	if( !pStage ) return;

	if( m_CurTargetInfo.m_TargetName.IsEmpty() )
	{
		if( m_dwNoTargetMarkerID > 0 )
			UpdateNoTargetMarker( pChar, pStage );
		else
			CreateNoTargetMarker( pChar, pStage );
	}
	else
	{
		if( m_dwNoTargetMarkerID > 0 )
		{
			pStage->DestroyMapEffect( m_dwNoTargetMarkerID );
			m_dwNoTargetMarkerID = 0;
		}

		ioBaseChar *pTarget = pStage->GetBaseChar( m_CurTargetInfo.m_TargetName );
		UpdateTargetMarker( pTarget, m_CurTargetInfo.m_dwTargetMarkerID, pStage );
	}

	UpdateTargetMarkerScale( pChar, pStage );
}

void ioTargetTeleportSkill2::CreateNoTargetMarker( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar || !pStage )
		return;

	if( m_dwNoTargetMarkerID > 0 )
	{
		pStage->DestroyMapEffect( m_dwNoTargetMarkerID );
		m_dwNoTargetMarkerID = 0;
	}

	D3DXVECTOR3 vScale( FLOAT1, FLOAT1, FLOAT1 );
	D3DXVECTOR3 vPos = pChar->GetMidPositionByRate();
	D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

	TargetSearchInfo kSearchInfo;
	GetTargetSearchInfo( kSearchInfo );
	
	vPos += ( vDir * kSearchInfo.m_fTargetMaxRange );
	
	ioMapEffect *pMapEffect = NULL;

	TeamType eTeam = pChar->GetSingleConvertTeam();
	if( eTeam == TEAM_RED && !m_RedNoTargetMarker.IsEmpty() )
	{
		pMapEffect = pStage->CreateMapEffect( m_RedNoTargetMarker, vPos, vScale );
	}
	else if( !m_BlueNoTargetMarker.IsEmpty() )
	{
		pMapEffect = pStage->CreateMapEffect( m_BlueNoTargetMarker, vPos, vScale );
	}

	if( pMapEffect )
	{
		m_dwNoTargetMarkerID = pMapEffect->GetUniqueID();

		D3DXQUATERNION qtCharRot;
		ioMath::CalcDirectionQuaternion( qtCharRot, -vDir );
		pMapEffect->SetWorldOrientation( qtCharRot );
	}
}

void ioTargetTeleportSkill2::UpdateNoTargetMarker( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar || !pStage )
		return;

	if( m_dwNoTargetMarkerID <= 0 )
		return;

	D3DXVECTOR3 vScale( FLOAT1, FLOAT1, FLOAT1 );
	D3DXVECTOR3 vPos = pChar->GetMidPositionByRate();
	D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	
	TargetSearchInfo kSearchInfo;
	GetTargetSearchInfo( kSearchInfo );

	vPos += ( vDir * kSearchInfo.m_fTargetMaxRange );
	
	ioMapEffect *pMapEffect = pStage->FindMapEffect( m_dwNoTargetMarkerID );
	if( pMapEffect )
	{
		pMapEffect->SetWorldPosition( vPos );

		D3DXQUATERNION qtCharRot;
		ioMath::CalcDirectionQuaternion( qtCharRot, -vDir );
		pMapEffect->SetWorldOrientation( qtCharRot );
	}
}

void ioTargetTeleportSkill2::ChangeTargetMarker( ioBaseChar *pChar )
{
	ioPlayStage *pStage = g_SkillMaker.GetPlayStage();
	if( !pChar || !pStage )
		return;

	ioHashString szCurMarker;
	TeamType eTeam = pChar->GetSingleConvertTeam();
	switch( m_SkillSubState )
	{
	case SSS_NONE:
		if( eTeam == TEAM_RED )
			szCurMarker = m_RedNormalTargetMarker;
		else
			szCurMarker = m_BlueNormalTargetMarker;
		break;
	case SSS_NORMAL_TARGET:
		if( eTeam == TEAM_RED )
			szCurMarker = m_RedNormalTargetMarker;
		else
			szCurMarker = m_BlueNormalTargetMarker;
		break;
	case SSS_SPECIAL_TARGET:
		if( eTeam == TEAM_RED )
			szCurMarker = m_RedSpecialTargetMarker;
		else
			szCurMarker = m_BlueSpecialTargetMarker;
		break;
	}

	pStage->DestroyMapEffect( m_CurTargetInfo.m_dwTargetMarkerID );

	ioBaseChar *pTarget = pStage->GetBaseChar( m_CurTargetInfo.m_TargetName );
	if( pTarget )
	{
		D3DXVECTOR3 vScale( FLOAT1, FLOAT1, FLOAT1 );
		D3DXVECTOR3 vPos = pTarget->GetMidPositionByRate();

		ioMapEffect *pMapEffect = pStage->CreateMapEffect( szCurMarker, vPos, vScale );
		if( pMapEffect )
		{
			m_CurTargetInfo.m_dwTargetMarkerID = pMapEffect->GetUniqueID();
		}
	}
}

void ioTargetTeleportSkill2::UpdateTargetMarker( ioBaseChar *pTarget, UniqueObjID eEffectID, ioPlayStage *pStage )
{
	if( !pStage )
		return;

	if( pTarget )
	{
		ioMapEffect *pMapEffect = pStage->FindMapEffect( eEffectID );
		if( pMapEffect )
		{
			pMapEffect->SetWorldPosition( pTarget->GetMidPositionByRate() );
		}
	}
	else
	{
		pStage->DestroyMapEffect( eEffectID );
	}
}

void ioTargetTeleportSkill2::UpdateTargetMarkerScale( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pStage )
		return;

	if( m_SkillSubState != SSS_NORMAL_TARGET )
		return;

	if( m_dwNormalTargetDuration <= 0 )
		return;

	if( m_CurTargetInfo.m_TargetName.IsEmpty() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	float fMaxGapScaleRate = m_fMaxScaleRate - m_fMinScaleRate;

	float fCurGapTime = dwCurTime - m_dwChangeSubStateTime;
	float fCurTimeRate = fCurGapTime / (float)m_dwNormalTargetDuration;

	float fCurScaleRate = m_fMaxScaleRate - (fMaxGapScaleRate*fCurTimeRate);
	D3DXVECTOR3 vScale( fCurScaleRate, fCurScaleRate, fCurScaleRate );
	
	ioMapEffect *pMapEffect = pStage->FindMapEffect( m_CurTargetInfo.m_dwTargetMarkerID );
	if( pMapEffect )
	{
		ioEffect* pEffect = pMapEffect->GetEffect();
		if( pEffect )
		{
			pEffect->GetParentNode()->SetScale( vScale );
		}
	}
}

void ioTargetTeleportSkill2::DestroyAlllMaker( ioPlayStage *pStage )
{
	if( !pStage )
		return;

	if( m_dwNoTargetMarkerID > 0 )
	{
		pStage->DestroyMapEffect( m_dwNoTargetMarkerID );
		m_dwNoTargetMarkerID = 0;
	}

	if( m_CurTargetInfo.m_TargetName.IsEmpty() )
		return;

	if( m_CurTargetInfo.m_dwTargetMarkerID > 0 )
	{
		pStage->DestroyMapEffect( m_CurTargetInfo.m_dwTargetMarkerID );
		m_CurTargetInfo.m_dwTargetMarkerID = 0;
	}
}

void ioTargetTeleportSkill2::SetNormalSkill( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	m_SkillState = SS_LOOP;
	m_SkillSubState = SSS_NONE;

	int iAniID = 0;
	iAniID = pGrp->GetAnimationIdx( GetLoopDelayAni() );
	if( iAniID == -1 )
		return;

	float fTimeRate = GetLoopDelayAniRate();
	if( fTimeRate <= FLOAT1 )
		fTimeRate = FLOAT1;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fTimeRate );
	m_dwMotionEndTime = 0;

	pChar->SetSkillMoveEnable( true, true );

	m_bSetMoveAni = false;

	ioPlayStage *pStage = g_SkillMaker.GetPlayStage();
	if( pStage )
	{
		CreateNoTargetMarker( pChar, pStage );
	}
}

void ioTargetTeleportSkill2::CheckNormalMoveAni( ioBaseChar* pChar )
{
	if( !pChar ) return;

	if( m_SkillUseType != SUT_NORMAL )
		return;

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

void ioTargetTeleportSkill2::FindTarget( ioBaseChar *pChar, ioPlayStage* pStage )
{
	if( !pChar ) return;
	if( !pStage ) return;

	if( !m_CurTargetInfo.m_TargetName.IsEmpty() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	ioHashString szCurMarker;
	TeamType eTeam = pChar->GetSingleConvertTeam();

	switch( m_SkillSubState )
	{
	case SSS_NONE:
		if( eTeam == TEAM_RED )
			szCurMarker = m_RedNormalTargetMarker;
		else
			szCurMarker = m_BlueNormalTargetMarker;
		break;
	case SSS_NORMAL_TARGET:
		if( eTeam == TEAM_RED )
			szCurMarker = m_RedNormalTargetMarker;
		else
			szCurMarker = m_BlueNormalTargetMarker;
		break;
	case SSS_SPECIAL_TARGET:
		if( eTeam == TEAM_RED )
			szCurMarker = m_RedSpecialTargetMarker;
		else
			szCurMarker = m_BlueSpecialTargetMarker;
		break;
	}

	BaseCharDistanceSqList vNewCharList;
	FindTargetList( pChar, pStage, vNewCharList );

	D3DXVECTOR3 vScale( FLOAT1, FLOAT1, FLOAT1 );
	D3DXVECTOR3 vTargetPos = ioMath::VEC3_ZERO;

	if( !vNewCharList.empty() )
	{
		std::sort( vNewCharList.begin(), vNewCharList.end(), BaseCharDistanceSqSort2() );

		// add target
		m_CurTargetInfo.m_TargetName = vNewCharList[0].m_Name;

		ioMapEffect *pMapEffect = pStage->CreateMapEffect( szCurMarker, vTargetPos, vScale );
		if( pMapEffect )
		{
			m_CurTargetInfo.m_dwTargetMarkerID = pMapEffect->GetUniqueID();
		}
	}
}

void ioTargetTeleportSkill2::FindTargetList( ioBaseChar *pChar, ioPlayStage* pStage, OUT BaseCharDistanceSqList &vTargetList )
{
	vTargetList.clear();

	if( !pChar || !pStage )
		return;

	D3DXVECTOR3 vScale( FLOAT1, FLOAT1, FLOAT1 );
	D3DXVECTOR3 vTargetPos = ioMath::VEC3_ZERO;
	D3DXVECTOR3 vDiff;
	float fLengthSq = 0.0f;

	const BaseCharList &rkCharList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter = rkCharList.begin();
	while( iter != rkCharList.end() )
	{
		ioBaseChar *pTarget = *iter;

		if( pTarget && pTarget != pChar )
		{
			if( pTarget->GetTeam() != pChar->GetTeam() &&
				IsEnableTargetState( pTarget, true ) &&
				CheckRangeAngle( pChar, pTarget ) )
			{
				vDiff = pTarget->GetMidPositionByRate() - pChar->GetMidPositionByRate();
				fLengthSq = D3DXVec3LengthSq( &vDiff );

				vTargetPos = pTarget->GetMidPositionByRate();

				BaseCharDistanceSq kDistanceInfo;
				kDistanceInfo.m_Name = pTarget->GetCharName();
				kDistanceInfo.m_fDistanceSq = fLengthSq;

				vTargetList.push_back( kDistanceInfo );
			}
		}

		++iter;
	}
}

void ioTargetTeleportSkill2::UpdateTargetList( ioBaseChar* pOwner, ioPlayStage* pStage )
{
	if( !pOwner || !pStage )
		return;

	D3DXQUATERNION qtRot = pOwner->GetTargetRot();
	D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vDir, &vDir );

	if( !m_CurTargetInfo.m_TargetName.IsEmpty() )
	{
		ioBaseChar* pTarget = pStage->GetBaseChar( m_CurTargetInfo.m_TargetName );
		//타겟 유효성 체크
		if( pTarget )
		{
			if( !IsEnableTargetState( pTarget, false ) )
			{
				if( m_CurTargetInfo.m_dwTargetMarkerID > 0 )
				{
					pStage->DestroyMapEffect( m_CurTargetInfo.m_dwTargetMarkerID );
				}

				m_CurTargetInfo.Init();
			}
			else
			{
				if( !CheckRangeAngle( pOwner, pTarget ) )
				{
					if( m_CurTargetInfo.m_dwTargetMarkerID > 0 )
					{
						pStage->DestroyMapEffect( m_CurTargetInfo.m_dwTargetMarkerID );
					}

					m_CurTargetInfo.Init();
				}
			}
		}
		else
		{
			if( m_CurTargetInfo.m_dwTargetMarkerID > 0 )
			{
				pStage->DestroyMapEffect( m_CurTargetInfo.m_dwTargetMarkerID );
			}

			m_CurTargetInfo.Init();
		}
	}
}

bool ioTargetTeleportSkill2::CheckRangeAngle( ioBaseChar *pChar, ioBaseChar *pTarget )
{
	if( !pChar || !pTarget )
		return false;

	TargetSearchInfo kSearchInfo;
	GetTargetSearchInfo( kSearchInfo );

	D3DXQUATERNION qtRot = pChar->GetTargetRot();
	D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vDir, &vDir );


	D3DXVECTOR3 vDiff = pTarget->GetMidPositionByRate() - pChar->GetMidPositionByRate();
	float fLengthSq = D3DXVec3LengthSq( &vDiff );
	float fMaxRangeSq = kSearchInfo.m_fTargetMaxRange * kSearchInfo.m_fTargetMaxRange;
	float fMinRangeSq = kSearchInfo.m_fTargetMinRange * kSearchInfo.m_fTargetMinRange;

	D3DXVec3Normalize( &vDiff, &vDiff );
	float fValue = D3DXVec3Dot( &vDir, &vDiff );
	float fAngleGap = RADtoDEG( acosf(fValue) );

	//Range Test & Angle Test
	if( COMPARE( fLengthSq, fMinRangeSq, fMaxRangeSq ) && fAngleGap <= kSearchInfo.m_fTargetAngle )
		return true;

	return false;
}

bool ioTargetTeleportSkill2::IsEnableTargetState( ioBaseChar *pTarget, bool bCheckTargetWound, bool bCheckProtected )
{
	if( !pTarget )
		return false;

	if( pTarget->IsSystemState() )
		return false;
			
	if( !pTarget->IsEnableTarget() )
		return false;

	if( bCheckProtected )
	{	
		if( pTarget->IsProtectState() )
			return false;

		if( pTarget->IsSkillProtected() )
			return false;
	}

	ioPlayStage *pStage = pTarget->GetCreator();
	if( pStage )
	{
		ioPlayMode *pMode = pStage->GetPlayMode();
		if( pMode && pMode->GetModeType() == MT_FOOTBALL )
			return false;
	}
	else{
		return false;
	}

	if( bCheckTargetWound )
	{
		// 쓰러짐 관련 체크
		TargetSearchInfo kSearchInfo;
		GetTargetSearchInfo( kSearchInfo );

		if( !pTarget->CheckTargetWoundType(kSearchInfo.m_TargetWoundType) )
			return false;
	}

	return true;
}

void ioTargetTeleportSkill2::SetEndState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	m_SkillState = SS_END;

	DestroyAlllMaker( pStage );
}

void ioTargetTeleportSkill2::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
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
			pZone->SetZoneValue( false, false );
		}
	}
}

void ioTargetTeleportSkill2::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	
	DWORD dwCurTime = FRAMEGETTIME();
	int iType;
	rkPacket >> iType;

	switch( iType )
	{
	case SSC_FIRE:
		{
			int iSubState;
			rkPacket >> iSubState;

			m_SkillSubState = (SkillSubState)iSubState;

			SetTargetComboState( pChar, pStage );
		}
		break;
	case SSC_TELEPORT:
		{
			int iSubState;
			rkPacket >> iSubState;

			m_SkillSubState = (SkillSubState)iSubState;

			SetTargetTeleportStart( pChar, pStage );
		}
		break;
	}
}


bool ioTargetTeleportSkill2::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

bool ioTargetTeleportSkill2::IsProtected( int iDefenseBreakType ) const
{
	if( m_bDisableSkillProtect )
		return false;

	if( DBT_NONE != (DefenseBreakType)m_ProtectCancelType )
	{
		if( iDefenseBreakType == m_ProtectCancelType )   // 스킬 무적 파괴
			return false;
	}

	if( IsPreDelayProtect() )
	{
		if( m_dwPreDelayStartTime != 0 )
			return true;
	}

	DWORD dwCurTime = FRAMEGETTIME();
	
	switch( m_SkillState )
	{
		case SS_LOOP:
			break;
		case SS_TELEPORT_S:
		case SS_TELEPORT_E:
			return true;
		case SS_NO_TARGET_FIRE:
		case SS_NORMAL_FIRE:
		case SS_SPECIAL_FIRE:
			{
				if( m_dwStartSkillProtectedTime > 0 && 
					m_dwStartSkillProtectedTime + m_dwSkillProtectedTime > dwCurTime )
					return true;
			}
			break;
	}
	
	return false;
}

bool ioTargetTeleportSkill2::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwPreDelayStartTime > 0 )
		return false;

	switch( m_SkillState )
	{
	case SS_PRE:
	case SS_LOOP:
	case SS_NO_TARGET_FIRE:
	case SS_NORMAL_FIRE:
	case SS_SPECIAL_FIRE:
	case SS_TELEPORT_S:
	case SS_TELEPORT_E:
		return false;
	case SS_END:
		return true;
	}

	return true;
}

void ioTargetTeleportSkill2::SetTargetComboState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar ) return;
	if( !pStage ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	m_iCurComboCnt = 0;

	switch( m_SkillSubState )
	{
	case SSS_NONE:
		m_SkillState = SS_NO_TARGET_FIRE;
		break;
	case SSS_NORMAL_TARGET:
		m_SkillState = SS_NORMAL_FIRE;
		break;
	case SSS_SPECIAL_TARGET:
		m_SkillState = SS_SPECIAL_FIRE;
		break;
	}

	m_dwStartSkillProtectedTime = dwCurTime;

	SetNextTargetComboState( pChar, pStage );

	pChar->SetSkillMoveEnable( false, false );

	if( GetOwnerItem() )
	{
		pChar->SetCurItemSkillGauge( GetOwnerItem()->GetName(), 0.0f );
	}
}

void ioTargetTeleportSkill2::SetNextTargetComboState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar || !pStage )
		return;

	ioItem *pItem = GetOwnerItem();
	if( !pItem )
		return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	AttributeList kComboList;

	switch( m_SkillSubState )
	{
	case SSS_NONE:
		kComboList = m_NoTargetAttackCombo;
		break;
	case SSS_NORMAL_TARGET:
		kComboList = m_NormalTargetAttackCombo;
		break;
	case SSS_SPECIAL_TARGET:
		kComboList = m_SpecialTargetAttackCombo;
		break;
	}

	int iMaxComboCnt = kComboList.size();
	m_dwNextComboTime = 0;

	// combo cnt over check
	if( !COMPARE( m_iCurComboCnt, 0, iMaxComboCnt ) )
	{
		m_iCurComboCnt = 0;
		return;
	}

	int iAniID = 0;
	iAniID = pGrp->GetAnimationIdx( kComboList[m_iCurComboCnt].m_AttackAnimation );
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "NextTargetComboState - Not Exist Ani" );
		return;
	}

	DWORD dwPreDelay = kComboList[m_iCurComboCnt].m_dwPreDelay;
	float fTimeRate = kComboList[m_iCurComboCnt].m_fAttackAniRate * m_fExtraAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	pChar->InitActionStopDelay( true );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	DWORD dwAniTime = pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwNextComboTime = dwCurTime + dwAniTime;

	if( m_iCurComboCnt == 0 )
		m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();

	pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex+m_iCurComboCnt,
										iAniID,
										kComboList[m_iCurComboCnt].m_vWeaponInfoList,
										FTT_SKILL_ATTACK,
										fTimeRate,
										dwPreDelay );

	pChar->SetReservedSliding( kComboList[m_iCurComboCnt].m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	//
	DWORD dwTrackingTime = dwCurTime + dwPreDelay;
	dwTrackingTime += pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" ) * fTimeRate;

	if( IsUseAutoTargetValue() )
	{
		pChar->SetAutoTarget( ATT_SKILL );
		pChar->SetAutoTargetTrackingEndTime( dwTrackingTime );
	}

	m_iCurComboCnt++;

	if( m_iCurComboCnt == iMaxComboCnt )
	{
		m_dwComboEndTime = dwCurTime + dwAniTime;
	}
}

void ioTargetTeleportSkill2::OnTargetComboState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar || !pStage )
		return;

	ioItem *pItem = GetOwnerItem();
	if( !pItem )
		return;

	ioHashString szItemName = pItem->GetName();
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwNextComboTime > 0 && m_dwNextComboTime < dwCurTime )
	{
		SetNextTargetComboState( pChar, pStage );
	}
}

void ioTargetTeleportSkill2::SetTargetTeleportStart( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar || !pStage )
		return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwTargetTeleportStartTime = 0;
	m_dwTargetTeleportEndTime = 0;

	m_SkillState = SS_TELEPORT_S;

	pChar->SetSkillMoveEnable( false, false );
	DestroyAlllMaker( pStage );

	int iAniID = pGrp->GetAnimationIdx( m_TargetTeleportStartAni );
	if( iAniID > -1 )
	{
		pGrp->ClearAllActionAni( FLOAT100, true );
		pGrp->ClearAllLoopAni( FLOAT100, true );

		float fTimeRate = m_fTargetTeleportStartAniRate;
		if( fTimeRate <= 0.0f )
			fTimeRate = FLOAT1;

		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );
		DWORD dwAniTime = pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

		m_dwTargetTeleportStartTime = dwCurTime + dwAniTime;
	}

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SSC_TELEPORT;
		kPacket << m_SkillSubState;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioTargetTeleportSkill2::OnTargetTeleportStart( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwTargetTeleportStartTime == 0 || m_dwTargetTeleportStartTime < dwCurTime )
	{
		SetTargetTeleportEnd( pChar, pStage );
	}
}

void ioTargetTeleportSkill2::SetTargetTeleportEnd( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar || !pStage )
		return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwTargetTeleportStartTime = 0;
	m_dwTargetTeleportEndTime = 0;

	m_SkillState = SS_TELEPORT_E;

	int iAniID = pGrp->GetAnimationIdx( m_TargetTeleportEndAni );
	if( iAniID > -1 )
	{
		pGrp->ClearAllActionAni( FLOAT100, true );
		pGrp->ClearAllLoopAni( FLOAT100, true );

		float fTimeRate = m_fTargetTeleportEndAniRate;
		if( fTimeRate <= 0.0f )
			fTimeRate = FLOAT1;

		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );
		DWORD dwAniTime = pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

		m_dwTargetTeleportEndTime = dwCurTime + dwAniTime;
	}

	ioBaseChar *pTarget = pStage->GetBaseChar( m_CurTargetInfo.m_TargetName );
	if( pTarget )
	{
		D3DXVECTOR3 vTargetPos = pTarget->GetWorldPosition();

		D3DXVECTOR3 vTargetDir = pTarget->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVec3Normalize( &vTargetDir, &vTargetDir );

		if( vTargetDir != ioMath::VEC3_ZERO )
			vTargetPos += m_fTeleportOffSet * vTargetDir;
		
		pChar->SetWorldPosition( vTargetPos );
		pChar->SetTargetRotToTargetPos( pTarget, true );
	}
}

void ioTargetTeleportSkill2::OnTargetTeleportEnd( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwTargetTeleportEndTime == 0 || m_dwTargetTeleportEndTime < dwCurTime )
	{
		SetTargetComboState( pChar, pStage );
	}
}

void ioTargetTeleportSkill2::ChangeTarget( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar || !pStage )
		return;

	if( m_CurTargetInfo.m_TargetName.IsEmpty() )
		return;

	BaseCharDistanceSqList vNewCharList;
	FindTargetList( pChar, pStage, vNewCharList );

	int iCharListSize = vNewCharList.size();
	if( iCharListSize <= 1 )
		return;

	int iChangeArray = -1;
	for( int i=0; i < iCharListSize; ++i )
	{
		if( vNewCharList[i].m_Name == m_CurTargetInfo.m_TargetName )
		{
			iChangeArray = i+1;
			break;
		}
	}

	if( iChangeArray == -1 )
		return;

	if( iChangeArray >= iCharListSize )
		iChangeArray = 0;

	DWORD dwCurTime = FRAMEGETTIME();

	ioHashString szCurMarker;
	TeamType eTeam = pChar->GetSingleConvertTeam();

	switch( m_SkillSubState )
	{
	case SSS_NONE:
		if( eTeam == TEAM_RED )
			szCurMarker = m_RedNormalTargetMarker;
		else
			szCurMarker = m_BlueNormalTargetMarker;
		break;
	case SSS_NORMAL_TARGET:
		if( eTeam == TEAM_RED )
			szCurMarker = m_RedNormalTargetMarker;
		else
			szCurMarker = m_BlueNormalTargetMarker;
		break;
	case SSS_SPECIAL_TARGET:
		if( eTeam == TEAM_RED )
			szCurMarker = m_RedSpecialTargetMarker;
		else
			szCurMarker = m_BlueSpecialTargetMarker;
		break;
	}

	D3DXVECTOR3 vScale( FLOAT1, FLOAT1, FLOAT1 );
	D3DXVECTOR3 vTargetPos = ioMath::VEC3_ZERO;

	// remove target
	pStage->DestroyMapEffect( m_CurTargetInfo.m_dwTargetMarkerID );

	// add target
	m_CurTargetInfo.m_TargetName = vNewCharList[iChangeArray].m_Name;

	ioMapEffect *pMapEffect = pStage->CreateMapEffect( szCurMarker, vTargetPos, vScale );
	if( pMapEffect )
	{
		m_CurTargetInfo.m_dwTargetMarkerID = pMapEffect->GetUniqueID();
	}
}

const ioHashString& ioTargetTeleportSkill2::GetLoopDelayAni()
{
	if( m_SkillUseType == SUT_JUMP )
		return m_JumpLoopDelayAnimation;

	return m_LoopDelayAnimation;
}

float ioTargetTeleportSkill2::GetLoopDelayAniRate()
{
	if( m_SkillUseType == SUT_JUMP )
		return m_fJumpLoopDelayAnimationRate;

	return m_fLoopDelayAnimationRate;
}

void ioTargetTeleportSkill2::GetTargetSearchInfo( OUT TargetSearchInfo &rkSearchInfo )
{
	rkSearchInfo.Init();

	if( m_SkillUseType == SUT_JUMP )
	{
		rkSearchInfo = m_NormalTargetSearch;
		return;
	}

	rkSearchInfo = m_JumpTargetSearch;
	return;
}

bool ioTargetTeleportSkill2::IsNoDropState() const
{
	switch( m_SkillState )
	{
	case SS_LOOP:
		if( m_SkillUseType == SUT_JUMP )
		{
			return true;
		}
		break;
	case SS_TELEPORT_S:
	case SS_TELEPORT_E:
	case SS_NO_TARGET_FIRE:
	case SS_NORMAL_FIRE:
	case SS_SPECIAL_FIRE:
		return true;
	}

	return false;
}

bool ioTargetTeleportSkill2::IsAttackEndState() const
{	
	if( m_fSkillEndNextJumpAmt == 0.0f )
		return false;

	if( m_SkillState == SS_END )
		return true;

	return false;
}

float ioTargetTeleportSkill2::GetSkillMoveRate() const
{
	if( m_SkillUseType == SUT_JUMP )
		return 0.0f;

	return m_fSkillMoveRate;
}

