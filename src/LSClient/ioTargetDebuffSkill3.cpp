

#include "stdafx.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"
#include "ioGrowthUpInfo.h"

#include "ioTargetDebuffSkill3.h"

#include "WeaponDefine.h"

ioTargetDebuffSkill3::ioTargetDebuffSkill3()
{
	ClearData();
}

ioTargetDebuffSkill3::ioTargetDebuffSkill3( const ioTargetDebuffSkill3 &rhs )
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
m_iMaxTargetCnt( rhs.m_iMaxTargetCnt ),
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
m_NoTargetFireInfo( rhs.m_NoTargetFireInfo ),
m_NormalTargetFireInfo( rhs.m_NormalTargetFireInfo ),
m_SpecialTargetFireInfo( rhs.m_SpecialTargetFireInfo ),
m_dwSkillProtectedTime( rhs.m_dwSkillProtectedTime ),
m_SkillMapEffect( rhs.m_SkillMapEffect )
{
	ClearData();
}

ioTargetDebuffSkill3::~ioTargetDebuffSkill3()
{
}

void ioTargetDebuffSkill3::ClearData()
{
	m_SkillState = SS_NONE;
	m_SkillSubState = SSS_NONE;

	m_TargetInfoMap.clear();

	m_dwChangeSubStateTime = 0;
	m_dwFireStartTime = 0;
	m_dwFireEndTime = 0;

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
}

void ioTargetDebuffSkill3::LoadProperty( ioINILoader &rkLoader )
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
	rkLoader.LoadString_e( "move_animation", "", szBuf, MAX_PATH );
	m_LoopMoveAnimation	= szBuf;

	// target cnt
	m_iMaxTargetCnt = rkLoader.LoadInt_e( "max_target_cnt", 0 );

	// target search range
	m_TargetWoundType = (TargetWoundType)rkLoader.LoadInt_e( "target_wound_type", TWT_ALL );
	m_fTargetAngle = rkLoader.LoadFloat_e( "target_angle", 0.0f );
	m_fTargetMaxRange = rkLoader.LoadFloat_e( "target_max_range", 0.0f );
	m_fTargetMinRange = rkLoader.LoadFloat_e( "target_min_range", 0.0f );

	// fire info
	LoadAttackAttribute_e( "no_target_attack", m_NoTargetFireInfo.m_TargetAttack, rkLoader );

	int iFireBuffCnt = rkLoader.LoadInt_e( "no_target_buff_cnt", 0 );
	for( int i=0; i < iFireBuffCnt; ++i )
	{
		wsprintf_e( szKey, "no_target_buff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

		m_NoTargetFireInfo.m_vTargetBuffList.push_back( szBuf );
	}

	LoadAttackAttribute_e( "normal_target_attack", m_NormalTargetFireInfo.m_TargetAttack, rkLoader );

	iFireBuffCnt = rkLoader.LoadInt_e( "normal_target_buff_cnt", 0 );
	for( int i=0; i < iFireBuffCnt; ++i )
	{
		wsprintf_e( szKey, "nomal_target_buff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

		m_NormalTargetFireInfo.m_vTargetBuffList.push_back( szBuf );
	}

	LoadAttackAttribute_e( "special_target_attack", m_SpecialTargetFireInfo.m_TargetAttack, rkLoader );

	iFireBuffCnt = rkLoader.LoadInt_e( "special_target_buff_cnt", 0 );
	for( int i=0; i < iFireBuffCnt; ++i )
	{
		wsprintf_e( szKey, "special_target_buff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

		m_SpecialTargetFireInfo.m_vTargetBuffList.push_back( szBuf );
	}

	// 무적
	m_dwSkillProtectedTime = (DWORD)rkLoader.LoadInt_e( "skill_protect_time", 0 );

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

ioSkill* ioTargetDebuffSkill3::Clone()
{
	return new ioTargetDebuffSkill3( *this );
}

bool ioTargetDebuffSkill3::OnSkillStart( ioBaseChar *pChar )
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

void ioTargetDebuffSkill3::OnSkillEnd( ioBaseChar *pChar )
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

void ioTargetDebuffSkill3::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
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
			if( pChar->IsAttackKey() && pChar->IsNeedProcess() )
			{
				SetTargetFireState( pChar, pStage );
			}
		}
		break;
	case SS_NO_TARGET_FIRE:
	case SS_NORMAL_FIRE:
	case SS_SPECIAL_FIRE:
		{
			if( m_dwFireEndTime < dwCurTime )
			{
				SetEndState( pChar, pStage );
				return;
			}
			else
			{
				OnTargetFireState( pChar, pStage );
			}
		}
		break;
	case SS_END:
		break;
	}
}

bool ioTargetDebuffSkill3::CheckSkillGauge( ioBaseChar *pChar )
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

void ioTargetDebuffSkill3::CheckSkillSubState( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( m_TargetInfoMap.empty() )
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

void ioTargetDebuffSkill3::ChangeSkillSubState( ioBaseChar *pChar, SkillSubState eState )
{
	if( !pChar )
		return;

	m_SkillSubState = eState;
	m_dwChangeSubStateTime = FRAMEGETTIME();
}

void ioTargetDebuffSkill3::UpdateMarker( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar ) return;
	if( !pStage ) return;

	if( m_TargetInfoMap.empty() )
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

		ioBaseChar *pTarget = NULL;
		TargetInfoMap::iterator iter = m_TargetInfoMap.begin();
		while( iter != m_TargetInfoMap.end() )
		{
			pTarget = pStage->GetBaseChar( iter->second.m_TargetName );
			UpdateTargetMarker( pTarget, iter->second.m_dwTargetMarkerID, pStage );
			++iter;
		}
	}

	UpdateTargetMarkerScale( pChar, pStage );
}

void ioTargetDebuffSkill3::CreateNoTargetMarker( ioBaseChar *pChar, ioPlayStage *pStage )
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
	
	vPos += ( vDir * m_fTargetMaxRange );
	
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

void ioTargetDebuffSkill3::UpdateNoTargetMarker( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar || !pStage )
		return;

	if( m_dwNoTargetMarkerID <= 0 )
		return;

	D3DXVECTOR3 vScale( FLOAT1, FLOAT1, FLOAT1 );
	D3DXVECTOR3 vPos = pChar->GetMidPositionByRate();
	D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	
	vPos += ( vDir * m_fTargetMaxRange );
	
	ioMapEffect *pMapEffect = pStage->FindMapEffect( m_dwNoTargetMarkerID );
	if( pMapEffect )
	{
		pMapEffect->SetWorldPosition( vPos );

		D3DXQUATERNION qtCharRot;
		ioMath::CalcDirectionQuaternion( qtCharRot, -vDir );
		pMapEffect->SetWorldOrientation( qtCharRot );
	}
}

void ioTargetDebuffSkill3::ChangeTargetMarker( ioBaseChar *pChar )
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

	ioBaseChar *pTarget = NULL;
	ioMapEffect *pMapEffect = NULL;
	TargetInfoMap::iterator iter = m_TargetInfoMap.begin();
	while( iter != m_TargetInfoMap.end() )
	{
		pStage->DestroyMapEffect( iter->second.m_dwTargetMarkerID );

		pTarget = pStage->GetBaseChar( iter->second.m_TargetName );
		if( pTarget )
		{
			D3DXVECTOR3 vScale( FLOAT1, FLOAT1, FLOAT1 );
			D3DXVECTOR3 vPos = pTarget->GetMidPositionByRate();

			pMapEffect = pStage->CreateMapEffect( szCurMarker, vPos, vScale );
			if( pMapEffect )
			{
				iter->second.m_dwTargetMarkerID = pMapEffect->GetUniqueID();
			}
		}

		++iter;
	}
}

void ioTargetDebuffSkill3::UpdateTargetMarker( ioBaseChar *pTarget, UniqueObjID eEffectID, ioPlayStage *pStage )
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

void ioTargetDebuffSkill3::UpdateTargetMarkerScale( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pStage )
		return;

	if( m_SkillSubState != SSS_NORMAL_TARGET )
		return;

	if( m_dwNormalTargetDuration <= 0 )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	float fMaxGapScaleRate = m_fMaxScaleRate - m_fMinScaleRate;

	float fCurGapTime = dwCurTime - m_dwChangeSubStateTime;
	float fCurTimeRate = fCurGapTime / (float)m_dwNormalTargetDuration;

	float fCurScaleRate = m_fMaxScaleRate - (fMaxGapScaleRate*fCurTimeRate);
	D3DXVECTOR3 vScale( fCurScaleRate, fCurScaleRate, fCurScaleRate );
	
	ioMapEffect *pMapEffect = NULL;
	TargetInfoMap::iterator iter = m_TargetInfoMap.begin();
	while( iter != m_TargetInfoMap.end() )
	{
		pMapEffect = pStage->FindMapEffect( iter->second.m_dwTargetMarkerID );
		if( pMapEffect )
		{
			ioEffect* pEffect = pMapEffect->GetEffect();
			if( pEffect )
			{
				pEffect->GetParentNode()->SetScale( vScale );
			}
		}

		++iter;
	}
}

void ioTargetDebuffSkill3::DestroyAlllMaker( ioPlayStage *pStage )
{
	if( !pStage )
		return;

	if( m_dwNoTargetMarkerID > 0 )
	{
		pStage->DestroyMapEffect( m_dwNoTargetMarkerID );
		m_dwNoTargetMarkerID = 0;
	}

	if( m_TargetInfoMap.empty() )
		return;

	TargetInfoMap::iterator iter = m_TargetInfoMap.begin();
	while( iter != m_TargetInfoMap.end() )
	{
		pStage->DestroyMapEffect( iter->second.m_dwTargetMarkerID );
		++iter;
	}

	m_TargetInfoMap.clear();
}

void ioTargetDebuffSkill3::SetNormalSkill( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	m_SkillState = SS_LOOP;
	m_SkillSubState = SSS_NONE;

	m_dwStartSkillProtectedTime = dwCurTime;

	int iAniID = 0;
	iAniID = pGrp->GetAnimationIdx( m_LoopDelayAnimation );
	if( iAniID == -1 )
		return;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	pGrp->SetLoopAni( iAniID, FLOAT100 );
	m_dwMotionEndTime = 0;

	pChar->SetSkillMoveEnable( true, true );

	m_bSetMoveAni = false;

	ioPlayStage *pStage = g_SkillMaker.GetPlayStage();
	if( pStage )
	{
		CreateNoTargetMarker( pChar, pStage );
	}
}

void ioTargetDebuffSkill3::CheckNormalMoveAni( ioBaseChar* pChar )
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

void ioTargetDebuffSkill3::FindTarget( ioBaseChar *pChar, ioPlayStage* pStage )
{
	if( !pChar ) return;
	if( !pStage ) return;

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

	D3DXQUATERNION qtRot = pChar->GetTargetRot();
	D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vDir, &vDir );

	TargetInfoMap::iterator iter_target;

	const BaseCharList &rkCharList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter = rkCharList.begin();
	while( iter != rkCharList.end() )
	{
		ioBaseChar *pTarget = *iter;

		// targetinfo가 최대값이 되었으면 return
		if( m_iMaxTargetCnt <= (int)m_TargetInfoMap.size() )
			return;

		iter_target = m_TargetInfoMap.find( pTarget->GetCharName() );
		if( iter_target != m_TargetInfoMap.end() )
		{
			++iter;
			continue;
		}

		if( pTarget == pChar )
		{
			++iter;
			continue;
		}

		if( pTarget->GetTeam() == pChar->GetTeam() )
		{
			++iter;
			continue;
		}

		if( pTarget->IsSystemState() )
		{
			++iter;
			continue;
		}
			
		if( !pTarget->IsEnableTarget() )
		{
			++iter;
			continue;
		}

		if( !IsEnableTargetState( pTarget, true ) )
		{
			++iter;
			continue;
		}

		D3DXVECTOR3 vDiff = pTarget->GetMidPositionByRate() - pChar->GetMidPositionByRate();
		float fLengthSq = D3DXVec3LengthSq( &vDiff );

		float fMaxRangeSq = m_fTargetMaxRange * m_fTargetMaxRange;
		float fMinRangeSq = m_fTargetMinRange * m_fTargetMinRange;

		//Range Test
		if( !COMPARE( fLengthSq, fMinRangeSq, fMaxRangeSq ) )
		{
			++iter;
			continue;
		}

		//Angle Test
		D3DXVec3Normalize( &vDiff, &vDiff );
		float fValue = D3DXVec3Dot( &vDir, &vDiff );
		float fAngleGap = RADtoDEG( acosf(fValue) );
		if( fAngleGap > m_fTargetAngle )
		{
			++iter;
			continue;
		}

		// add target
		TargetInfo kInfo;

		kInfo.m_TargetName = pTarget->GetCharName();

		D3DXVECTOR3 vScale( FLOAT1, FLOAT1, FLOAT1 );
		D3DXVECTOR3 vTargetPos = pTarget->GetMidPositionByRate();

		ioMapEffect *pMapEffect = pStage->CreateMapEffect( szCurMarker, vTargetPos, vScale );
		if( pMapEffect )
		{
			kInfo.m_dwTargetMarkerID = pMapEffect->GetUniqueID();
		}

		m_TargetInfoMap.insert( TargetInfoMap::value_type( pTarget->GetCharName(), kInfo ) );

		++iter;
	}
}

void ioTargetDebuffSkill3::UpdateTargetList( ioBaseChar* pOwner, ioPlayStage* pStage )
{
	if( !pOwner || !pStage )
		return;

	ioHashString szTargetName;
	ioBaseChar* pTarget = NULL;

	D3DXQUATERNION qtRot = pOwner->GetTargetRot();
	D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vDir, &vDir );

	TargetInfoMap::iterator iter = m_TargetInfoMap.begin();
	while( iter != m_TargetInfoMap.end() )
	{
		szTargetName = iter->second.m_TargetName;
		pTarget = pStage->GetBaseChar( szTargetName );

		//타겟 유효성 체크
		if( pTarget )
		{
			if( !IsEnableTargetState( pTarget, false ) )
			{
				if( iter->second.m_dwTargetMarkerID > 0 )
				{
					pStage->DestroyMapEffect( iter->second.m_dwTargetMarkerID );
				}

				iter = m_TargetInfoMap.erase( iter );
				continue;
			}
			else
			{
				D3DXVECTOR3 vDiff = pTarget->GetMidPositionByRate() - pOwner->GetMidPositionByRate();
				float fLengthSq = D3DXVec3LengthSq( &vDiff );

				float fMaxRangeSq = m_fTargetMaxRange * m_fTargetMaxRange;
				float fMinRangeSq = m_fTargetMinRange * m_fTargetMinRange;

				//Range Test
				if( !COMPARE( fLengthSq, fMinRangeSq, fMaxRangeSq ) )
				{
					if( iter->second.m_dwTargetMarkerID > 0 )
					{
						pStage->DestroyMapEffect( iter->second.m_dwTargetMarkerID );
					}

					iter = m_TargetInfoMap.erase( iter );
					continue;
				}

				//Angle Test
				D3DXVec3Normalize( &vDiff, &vDiff );
				float fValue = D3DXVec3Dot( &vDir, &vDiff );
				float fAngleGap = RADtoDEG( acosf(fValue) );
				if( fAngleGap > m_fTargetAngle )
				{
					if( iter->second.m_dwTargetMarkerID > 0 )
					{
						pStage->DestroyMapEffect( iter->second.m_dwTargetMarkerID );
					}

					iter = m_TargetInfoMap.erase( iter );
					continue;
				}
			}
		}
		else
		{
			if( iter->second.m_dwTargetMarkerID > 0 )
			{
				pStage->DestroyMapEffect( iter->second.m_dwTargetMarkerID );
			}

			iter = m_TargetInfoMap.erase( iter );
			continue;
		}

		++iter;
	}
}

bool ioTargetDebuffSkill3::IsEnableTargetState( ioBaseChar *pTarget, bool bCheckTargetWound, bool bCheckProtected )
{
	if( !pTarget )
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
		if( !pTarget->CheckTargetWoundType(m_TargetWoundType) )
			return false;
	}

	return true;
}

void ioTargetDebuffSkill3::SetEndState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	m_SkillState = SS_END;

	DestroyAlllMaker( pStage );
}

void ioTargetDebuffSkill3::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
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

void ioTargetDebuffSkill3::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
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

			SetTargetFireState( pChar, pStage );
		}
		break;
	}
}


bool ioTargetDebuffSkill3::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

bool ioTargetDebuffSkill3::IsProtected( int iDefenseBreakType ) const
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
			{
				if( m_dwStartSkillProtectedTime > 0 && 
					m_dwStartSkillProtectedTime + m_dwSkillProtectedTime > dwCurTime )
					return true;
			}
			break;			
		case SS_NO_TARGET_FIRE:
		case SS_NORMAL_FIRE:
		case SS_SPECIAL_FIRE:
			if( m_dwFireMotionEndTime > 0 && dwCurTime <= m_dwFireMotionEndTime )
				return true;
			break;
	}
	
	return false;
}

bool ioTargetDebuffSkill3::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
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
		return false;
	case SS_END:
		return true;
	}

	return true;
}

void ioTargetDebuffSkill3::SetTargetFireState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar ) return;
	if( !pStage ) return;

	if( m_SkillState != SS_LOOP )
		return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( !pGrp )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	TargetFireInfo kFireInfo;

	switch( m_SkillSubState )
	{
	case SSS_NONE:
		m_SkillState = SS_NO_TARGET_FIRE;
		kFireInfo = m_NoTargetFireInfo;
		break;
	case SSS_NORMAL_TARGET:
		m_SkillState = SS_NORMAL_FIRE;
		kFireInfo = m_NormalTargetFireInfo;
		break;
	case SSS_SPECIAL_TARGET:
		m_SkillState = SS_SPECIAL_FIRE;
		kFireInfo = m_SpecialTargetFireInfo;
		break;
	}

	int iAniID = 0;
	iAniID = pGrp->GetAnimationIdx( kFireInfo.m_TargetAttack.m_AttackAnimation );
	if( iAniID == -1 )
	{
		LOG.PrintTimeAndLog( 0, "TargetFireState - Not Exist Ani" );
		return;
	}

	float fTimeRate = kFireInfo.m_TargetAttack.m_fAttackAniRate * m_fExtraAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	m_dwFireStartTime = dwCurTime;
	m_dwFireStartTime += pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;

	m_dwFireEndTime = dwCurTime;
	m_dwFireEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	
	m_dwFireMotionEndTime = dwCurTime;
	m_dwFireMotionEndTime += pGrp->GetAnimationEventTime_e( iAniID, "firemotion_e" ) * fTimeRate;

	pChar->SetSkillMoveEnable( false, false );

	if( GetOwnerItem() )
	{
		pChar->SetCurItemSkillGauge( GetOwnerItem()->GetName(), 0.0f );
	}

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SSC_FIRE;
		kPacket << m_SkillSubState;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioTargetDebuffSkill3::OnTargetFireState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar || !pStage )
		return;

	ioItem *pItem = GetOwnerItem();
	if( !pItem )
		return;

	ioHashString szItemName = pItem->GetName();
	DWORD dwCurTime = FRAMEGETTIME();

	int iWeaponCnt = 0;
	if( m_dwFireStartTime > 0 && m_dwFireStartTime < dwCurTime )
	{
		TargetFireInfo kFireInfo;
		D3DXVECTOR3 vCreatePos = pChar->GetMidPositionByRate();

		switch( m_SkillSubState )
		{
		case SSS_NONE:
			kFireInfo = m_NoTargetFireInfo;
			break;
		case SSS_NORMAL_TARGET:
			kFireInfo = m_NormalTargetFireInfo;
			break;
		case SSS_SPECIAL_TARGET:
			kFireInfo = m_SpecialTargetFireInfo;
			break;
		}

		ioBaseChar* pTarget = NULL;
		D3DXVECTOR3 vAttackDir = ioMath::VEC3_ZERO;

		if( m_TargetInfoMap.empty() )
		{
			// attack
			WeaponInfoList::iterator weapon_iter = kFireInfo.m_TargetAttack.m_vWeaponInfoList.begin();
			while( weapon_iter != kFireInfo.m_TargetAttack.m_vWeaponInfoList.end() )
			{
				pChar->SkillFireExplicit( (*weapon_iter),
										  GetName(),
										  m_dwWeaponBaseIndex+iWeaponCnt,
										  vCreatePos );

				++iWeaponCnt;
				++weapon_iter;
			}
		}
		else
		{
			TargetInfoMap::iterator iter = m_TargetInfoMap.begin();
			while( iter != m_TargetInfoMap.end() )
			{
				pTarget = pStage->GetBaseChar( iter->second.m_TargetName );

				// remove effect
				if( iter->second.m_dwTargetMarkerID > 0 )
				{
					pStage->DestroyMapEffect( iter->second.m_dwTargetMarkerID );
					iter->second.m_dwTargetMarkerID = 0;
				}

				if( pTarget )
				{
					vAttackDir = pTarget->GetMidPositionByRate() - pChar->GetMidPositionByRate();
					D3DXVec3Normalize( &vAttackDir, &vAttackDir );

					// attack
					WeaponInfoList::iterator weapon_iter = kFireInfo.m_TargetAttack.m_vWeaponInfoList.begin();
					while( weapon_iter != kFireInfo.m_TargetAttack.m_vWeaponInfoList.end() )
					{
						pChar->SkillFireExplicit( (*weapon_iter),
												  GetName(),
												  m_dwWeaponBaseIndex+iWeaponCnt,
												  vCreatePos,
												  vAttackDir );

						++iWeaponCnt;
						++weapon_iter;
					}

					// buff
					int iBuffCnt = kFireInfo.m_vTargetBuffList.size();
					ioHashString szBuffName;
					for( int i=0; i < iBuffCnt; ++i )
					{
						if( IsEnableTargetState( pTarget, false, false ) )
						{
							szBuffName = kFireInfo.m_vTargetBuffList[i];

							if( !szBuffName.IsEmpty() )
							{
								pTarget->AddNewBuff( szBuffName,  pChar->GetCharName(), szItemName, this );
							}
						}
					}

					// add effect
					TeamType eTeam = pChar->GetSingleConvertTeam();
					D3DXVECTOR3 vScale = D3DXVECTOR3(1.0f, FLOAT1, FLOAT1 );
					ioMapEffect* pMapEffect = NULL;

					if( eTeam == TEAM_BLUE && !m_BlueFireTargetMarker.IsEmpty() )
					{
						pMapEffect = pStage->CreateMapEffect( m_BlueFireTargetMarker,
															  pTarget->GetMidPositionByRate(),
															  vScale );
					}
					else if( eTeam == TEAM_RED && !m_RedFireTargetMarker.IsEmpty() )
					{
						pMapEffect = pStage->CreateMapEffect( m_RedFireTargetMarker,
															  pTarget->GetMidPositionByRate(),
															  vScale );
					}

					if( pMapEffect )
					{
						iter->second.m_dwTargetMarkerID = pMapEffect->GetUniqueID();
					}
				}

				++iter;
			}
		}

		m_dwFireStartTime = 0;
	}
}


