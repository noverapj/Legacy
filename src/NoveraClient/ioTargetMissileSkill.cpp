

#include "stdafx.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"
#include "ioGrowthUpInfo.h"

#include "ioTargetMissileSkill.h"

#include "WeaponDefine.h"

ioTargetMissileSkill::ioTargetMissileSkill()
{
	ClearData();
}

ioTargetMissileSkill::ioTargetMissileSkill( const ioTargetMissileSkill &rhs )
: ioNormalSkill( rhs ),
  m_bDisableGrowth( rhs.m_bDisableGrowth ),
  m_fGuagePerTic( rhs.m_fGuagePerTic ),
  m_dwTicTime( rhs.m_dwTicTime ),
  m_dwSkillEndDelay( rhs.m_dwSkillEndDelay ),
  m_szZeroAimFireMotion( rhs.m_szZeroAimFireMotion ),
  m_fZeroAimFireAniTimeRate( rhs.m_fZeroAimFireAniTimeRate ),
  m_dwZeroAimFireEndDelay( rhs.m_dwZeroAimFireEndDelay ),
  m_szZeroAimLoopMotion( rhs.m_szZeroAimLoopMotion ),
  m_dwZeroAimLoopDuration( rhs.m_dwZeroAimLoopDuration ),
  m_fZeroAimLoopMotionRate( rhs.m_fZeroAimLoopMotionRate ),
  m_dwLoopProtectTime( rhs.m_dwLoopProtectTime ),
  m_bDirection( rhs.m_bDirection ),
  m_szNormalDelayAnimation( rhs.m_szNormalDelayAnimation ),
  m_szNormalMoveAnimation( rhs.m_szNormalMoveAnimation ),
  m_szGuideMarker( rhs.m_szGuideMarker ),
  m_szAimMarker( rhs.m_szAimMarker ),
  m_szZeroAimMarker( rhs.m_szZeroAimMarker ),
  m_dwAimDuration( rhs.m_dwAimDuration ),
  m_dwZeroAimDuration( rhs.m_dwZeroAimDuration ),
  m_TargetWoundType( rhs.m_TargetWoundType ),
  m_fTargetAngle( rhs.m_fTargetAngle ),
  m_fTargetRange( rhs.m_fTargetRange ),
  m_fTargetMinRange( rhs.m_fTargetMinRange ),
  m_szWeaponEffect( rhs.m_szWeaponEffect ),
  m_iMissileCount( rhs.m_iMissileCount ),
  m_MissileInfoList( rhs.m_MissileInfoList ),
  m_iSelectBaseBullut( rhs.m_iSelectBaseBullut ),
  m_dwUpDownSpeed( rhs.m_dwUpDownSpeed ),
  m_fUpDownOffset( rhs.m_fUpDownOffset ),
  m_AimWeaponInfo( rhs.m_AimWeaponInfo ),
  m_ZeroAimWeaponInfo( rhs.m_ZeroAimWeaponInfo ),
  m_FinishWeaponInfo( rhs.m_FinishWeaponInfo ),  
  m_dwFireGapTime( rhs.m_dwFireGapTime ),
  m_dwSkillProtectTime( rhs.m_dwSkillProtectTime )
{
	ClearData();
}

ioTargetMissileSkill::~ioTargetMissileSkill()
{
}

void ioTargetMissileSkill::ClearData()
{
	m_szTargetName.Clear();
	
	m_bSetMoveAni	= false;
	m_bSetHandMesh	= false;
	
	m_dwCurSkillProtectTime = 0;

	m_dwActionStartTime	= 0;
	m_dwMotionEndTime	= 0;
	m_dwMotionStartTime	= 0;
	
	m_dwAimStartTime	= 0;
	m_dwZeroAimStartTime= 0;

	m_dwZeroAimedFireTime	= 0;

	m_dwAimMarkerID		= -1;
	m_dwZeroAimMarkerID	= -1;
	m_dwGuideMakerID	= -1;
	m_WeaponEffectID.clear();

	m_iCurFireWeapon = -1;

	m_fCurChargeRate = 0.0f;

	// 성장관련 요소
	m_dwCheckTime			= 0;
	m_fTicGaugeRateByGrowth	= FLOAT1;
	m_dwTicTimeByGrowth		= 0;

	m_fRadiansForHeight		= 0.0f;

	m_dwLoopStartTime		= 0;
	m_dwProtectEndTime		= 0;
}

void ioTargetMissileSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioNormalSkill::LoadProperty( rkLoader );

	char szKey[MAX_PATH], szBuf[MAX_PATH];

	m_bDisableGrowth= rkLoader.LoadBool_e( "disable_growth", false );
	m_fGuagePerTic	= rkLoader.LoadFloat_e( "gauge_per_tick", 0.0f );
	m_dwTicTime		= (DWORD)rkLoader.LoadInt_e( "tick_time", 0 );


	// 공격 모션
	m_dwSkillEndDelay = (DWORD)rkLoader.LoadInt_e( "animation_end_delay", 0 );

	// 조준공격모션
	rkLoader.LoadString_e( "zero_aim_fire_motion", "", szBuf, MAX_PATH );		m_szZeroAimFireMotion	= szBuf;
	rkLoader.LoadString_e( "zero_aim_fire_loop_motion", "", szBuf, MAX_PATH );	m_szZeroAimLoopMotion	= szBuf;
	m_fZeroAimFireAniTimeRate	= rkLoader.LoadFloat_e( "zero_aim_fire_motion_time_rate", FLOAT1);
	m_dwZeroAimFireEndDelay		= (DWORD)rkLoader.LoadInt_e( "zero_aim_fire_motion_end_delay", 0 );	
	m_dwZeroAimLoopDuration			= (DWORD)rkLoader.LoadInt_e( "zero_aim_fire_loop_duration", 0 );
	m_fZeroAimLoopMotionRate	= rkLoader.LoadFloat_e( "zero_aim_fire_loop_motion_rate", 0.0f );
	m_dwLoopProtectTime			= (DWORD)rkLoader.LoadInt_e( "zero_aim_fire_loop_protect_time", 0 );


	// 애니메이션
	m_bDirection = rkLoader.LoadBool_e( "direction", false );
	rkLoader.LoadString_e( "delay_animation", "", szBuf, MAX_PATH );	m_szNormalDelayAnimation	= szBuf;
	rkLoader.LoadString_e( "move_animation", "", szBuf, MAX_PATH );	m_szNormalMoveAnimation		= szBuf;


	// 조준마크
	rkLoader.LoadString_e( "guide_marker", "", szBuf, MAX_PATH );		m_szGuideMarker				= szBuf;
	rkLoader.LoadString_e( "aim_marker", "", szBuf, MAX_PATH );		m_szAimMarker				= szBuf;
	rkLoader.LoadString_e( "zero_aim_marker", "", szBuf, MAX_PATH );	m_szZeroAimMarker			= szBuf;
	m_dwAimDuration				= (DWORD)rkLoader.LoadInt_e( "aim_duration", 0 );
	m_dwZeroAimDuration			= (DWORD)rkLoader.LoadInt_e( "zero_aim_duration", 0 );

	// 타겟 검색 범위
	m_TargetWoundType	= (TargetWoundType)rkLoader.LoadInt_e( "target_wound_type", TWT_ALL );
	m_fTargetAngle		= rkLoader.LoadFloat_e( "target_angle", 0.0f );
	m_fTargetRange		= rkLoader.LoadFloat_e( "target_max_range", 0.0f );
	m_fTargetMinRange	= rkLoader.LoadFloat_e( "target_min_range", 0.0f );


	// 미사일 정보
	rkLoader.LoadString_e( "bullet_effect", "", szBuf, MAX_PATH );
	m_szWeaponEffect = szBuf;

	m_MissileInfoList.clear();
	m_iMissileCount = rkLoader.LoadInt_e( "max_bullet_cnt", 1 );
	m_iMissileCount =  max( 1, m_iMissileCount );
	for( int i=0; i<m_iMissileCount; ++i )
	{
		MissileInfo kInfo;

		wsprintf_e( szKey, "bullet_angle%d", i+1 );			kInfo.fAngle = rkLoader.LoadFloat( szKey, 0.0f );
		wsprintf_e( szKey, "bullet_range_offset%d", i+1 );	kInfo.fRangeOffset = rkLoader.LoadFloat( szKey, 0.0f );
		wsprintf_e( szKey, "bullet_height_offset%d", i+1 );	kInfo.fHeightOffset = rkLoader.LoadFloat( szKey, 0.0f );

		m_MissileInfoList.push_back( kInfo );
	}
	m_iSelectBaseBullut = rkLoader.LoadInt_e( "select_base_bullet", 0 );
	m_dwUpDownSpeed = (DWORD)rkLoader.LoadInt_e( "weapon_updown_speed", 1000 );
	m_fUpDownOffset = rkLoader.LoadFloat_e( "weapon_updown_offset", 0.0f);


	// 발사체1 정보
	wsprintf_e( szKey, "aim_attack_attribute_index" );			m_AimWeaponInfo.m_iWeaponIdx = rkLoader.LoadInt( szKey, 0 );
	wsprintf_e( szKey, "aim_attack_attribute_resist_index" );		m_AimWeaponInfo.m_iResistanceIdx = rkLoader.LoadInt( szKey, 0 );
	wsprintf_e( szKey, "aim_attack_wounded_animation" );
	rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );			m_AimWeaponInfo.m_WoundedAnimation = szBuf;
	wsprintf_e( szKey, "aim_attack_wounded_duration" );			m_AimWeaponInfo.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );
	wsprintf_e( szKey, "aim_attack_wounded_loop_ani" );			m_AimWeaponInfo.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );

	// 발사체2 정보
	wsprintf_e( szKey, "zero_aim_attack_attribute_index" );		m_ZeroAimWeaponInfo.m_iWeaponIdx = rkLoader.LoadInt( szKey, 0 );
	wsprintf_e( szKey, "zero_aim_attack_attribute_resist_index" );m_ZeroAimWeaponInfo.m_iResistanceIdx = rkLoader.LoadInt( szKey, 0 );
	wsprintf_e( szKey, "zero_aim_attack_wounded_animation" );	
	rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );			m_ZeroAimWeaponInfo.m_WoundedAnimation = szBuf;
	wsprintf_e( szKey, "zero_aim_attack_wounded_duration" );		m_ZeroAimWeaponInfo.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );
	wsprintf_e( szKey, "zero_aim_attack_wounded_loop_ani" );		m_ZeroAimWeaponInfo.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );

	// 발사체3 정보 
	wsprintf_e( szKey, "finish_attack_attribute_index" );			m_FinishWeaponInfo.m_iWeaponIdx = rkLoader.LoadInt( szKey, 0 );
	wsprintf_e( szKey, "finish_attack_attribute_resist_index" );	m_FinishWeaponInfo.m_iResistanceIdx = rkLoader.LoadInt( szKey, 0 );
	wsprintf_e( szKey, "finish_attack_wounded_animation" );
	rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );			m_FinishWeaponInfo.m_WoundedAnimation = szBuf;
	wsprintf_e( szKey, "finish_attack_wounded_duration" );		m_FinishWeaponInfo.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );
	wsprintf_e( szKey, "finish_attack_wounded_loop_ani" );		m_FinishWeaponInfo.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );


	m_dwFireGapTime = (DWORD)rkLoader.LoadInt_e( "fire_gap_time", 0 );

	m_dwSkillProtectTime = (DWORD)rkLoader.LoadInt_e( "skill_protect_time", 0 );
}

ioSkill* ioTargetMissileSkill::Clone()
{
	return new ioTargetMissileSkill( *this );
}

bool ioTargetMissileSkill::OnSkillStart( ioBaseChar *pChar )
{
	if( !HasSkillMotion() )
		return false;

	if( !pChar )
		return false;

	ioNormalSkill::OnSkillStart( pChar );

	m_SkillState	= SS_PREDELAY;

	ClearData();

	m_dwCheckTime	= FRAMEGETTIME();

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

	if( m_dwSkillProtectTime > 0 )
		m_dwCurSkillProtectTime = FRAMEGETTIME() + m_dwSkillProtectTime;

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

void ioTargetMissileSkill::OnSkillEnd( ioBaseChar *pChar )
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

	DestroyGuideMarker( pChar );
	DestroyAimMarker();
	DestroyAllWeaponEffect();

	if( !m_SkillSoundName.IsEmpty() )
	{
		g_SoundMgr.StopSound( m_SkillSoundName, m_SkillSoundID );
		//g_SoundMgr.StopSound( m_SkillSoundName, 0 );
	}

	ClearData();
}

void ioTargetMissileSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

	float	fCurTicGauge	= m_fGuagePerTic / m_fTicGaugeRateByGrowth;
	int		iTickGap		= (int)( dwCurTime - m_dwCheckTime );
	int		iCurTicTime		= m_dwTicTime + m_dwTicTimeByGrowth;

	switch( m_SkillState )
	{
	case SS_PREDELAY:
		{
			DWORD dwPreDelay = GetPreDelayTime();
			if( m_dwPreDelayStartTime + dwPreDelay < dwCurTime )
			{
				m_dwPreDelayStartTime = 0;

				EndPreDelayEffect( pChar );
				SetNormalState( pChar );
				CreateGuideMarker( pChar );
				CreateWeaponEffect( pChar );
			}
		}
		break;
	case SS_NORMAL:
		{
			FindTarget( pChar );
			CheckNormalMoveAni( pChar );
			UpdateGuideMarker( pChar );
			UpdateAimMarker();
			UpdateWeaponEffect( pChar );
			int iCheckInput = CheckInputByAimTime( pChar );
			if( pChar->IsNeedProcess() )
			{
				if( iCheckInput	== CA_ZERO_AIM_FIRE )
				{
					SetZeroAimFireState( pChar );
				}
				else if( iCheckInput == CA_AIM_FIRE )
				{
					SetAimFireState( pChar );
				}
			}
		}
		break;
	case SS_AIM_FIRE:
		{
			UpdateWeaponEffect( pChar );
			ProcessAimFireState( pChar );
		}
		break;
	case SS_ZERO_AIM_FIRE:
		{
			UpdateAimMarker();
			ProcessZeroAimFireState( pChar );
		}
		break;	
	case SS_END:
		break;
	}

	if( m_SkillState != SS_END )
	{
		if( iTickGap >= iCurTicTime )
		{
			m_dwCheckTime = dwCurTime - ( iTickGap - iCurTicTime );

			if( fCurTicGauge != 0.0f )
			{
				// 정조준 발사중에는 종료되지 않도록 예외를 둠.
				if( pChar->ReduceCurSkillGaugeBySkill( GetOwnerItem(), fCurTicGauge ) <= 0.0f )
				{
					if( m_SkillState != SS_ZERO_AIM_FIRE && m_SkillState != SS_END )
					{
						SetEndState( pChar );
						return;
					}
				}
			}
		}
	}
}

void ioTargetMissileSkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	ioTraceWeaponType2	*pTraceWeapon = ToTraceWeaponType2( pWeapon );
	ioMissileWeapon		*pMissileWeapon = ToMissileWeapon( pWeapon );

	if( pTraceWeapon )
	{
		pTraceWeapon->SetTraceTarget( m_szTargetName );
		pTraceWeapon->SetStartPosition( m_MissileInfoList[m_iCurFireWeapon].vPosition, pStage );
	}
	
	if( pMissileWeapon )
	{
		D3DXVECTOR3 vDir = ioMath::VEC3_ZERO;
		D3DXVECTOR3 vPos = m_MissileInfoList[m_iSelectBaseBullut-1].vPosition;

		if( m_szTargetName.IsEmpty() )
		{
			ioMapEffect *pMapEffect = pStage->FindMapEffect( m_dwGuideMakerID );
			if( pMapEffect )
			{
				D3DXVECTOR3 vMarkerPos = pMapEffect->GetWorldPosition();
				vDir = vMarkerPos - vPos;
			}
		}
		else
		{
			ioBaseChar *pTarget = pStage->GetBaseChar( m_szTargetName );
			if( pTarget )
			{
				D3DXVECTOR3 vTargetPos = pTarget->GetWorldPosition();
				vTargetPos.y = pTarget->GetMidHeightByRate();
				vDir = vTargetPos - vPos;
			}
		}
		
		if( vDir != ioMath::VEC3_ZERO )
		{
			D3DXVec3Normalize( &vDir, &vDir );
			pMissileWeapon->SetMoveDir( vDir );
		}
	}
}

void ioTargetMissileSkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	int iType;
	rkPacket >> iType;

	switch( iType )
	{
	case SSC_AIM_FIRE:
		{
			rkPacket >> m_szTargetName;
			SetAimFireState( pChar );
		}
		break;
	case SSC_ZERO_AIM_FIRE:
		{
			rkPacket >> m_szTargetName;
			SetZeroAimFireState( pChar );
		}
		break;
	}
}

bool ioTargetMissileSkill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

bool ioTargetMissileSkill::IsProtected( int iDefenseBreakType ) const
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
	if( m_dwPreDelayStartTime == 0 )
	{
		if( m_dwCurSkillProtectTime > 0 &&
			m_dwCurSkillProtectTime > dwCurTime )
			return true;

		if( m_dwProtectEndTime > 0 &&
			m_dwProtectEndTime > dwCurTime )
			return true;
	}

	

	return false;
}

bool ioTargetMissileSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwPreDelayStartTime > 0 )
		return false;

	switch( m_SkillState )
	{
	case SS_PREDELAY:
	case SS_NORMAL:
	case SS_AIM_FIRE:
	case SS_ZERO_AIM_FIRE:
		return false;
	case SS_END:
		if( m_dwMotionEndTime + dwActionStop < dwCurTime )
		{
			return true;
		}
		return false;
	}

	return false;
}

void ioTargetMissileSkill::SetNormalState( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	m_SkillState = SS_NORMAL;

	m_bSetMoveAni = false;

	ioEntityGroup *pGrp = pChar->GetGroup();

	int iAniID = 0;
	iAniID = pGrp->GetAnimationIdx( m_szNormalDelayAnimation );
	if( iAniID == -1 )
		return;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	pGrp->SetLoopAni( iAniID, FLOAT100 );

	m_dwActionStartTime = dwCurTime;
	m_dwMotionStartTime = dwCurTime;
	m_dwMotionEndTime = 0;

	pChar->SetSkillMoveEnable( true, m_bDirection );
}

void ioTargetMissileSkill::SetZeroAimFireState( ioBaseChar *pChar )
{
	if( m_SkillState == SS_ZERO_AIM_FIRE )	return;
	if( !pChar )	return;

	DWORD dwCurTime = FRAMEGETTIME();

	m_SkillState = SS_ZERO_AIM_FIRE;
	pChar->SetSkillMoveEnable( false, false );

	ioEntityGroup *pGrp = pChar->GetGroup();

	int iCurSkillAniID = pGrp->GetAnimationIdx( m_szZeroAimFireMotion );
	if( iCurSkillAniID == -1 )	return;
	float fAniRate = m_fZeroAimFireAniTimeRate;

	float fTimeRate = fAniRate * m_fExtraAniRate;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iCurSkillAniID, FLOAT100, 5.0f, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	m_dwMotionStartTime = m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iCurSkillAniID );
	m_dwMotionEndTime += m_dwZeroAimFireEndDelay;

	m_dwAimStartTime	= 0;
	m_dwZeroAimStartTime= 0;
	m_dwZeroAimedFireTime = dwCurTime;
	
	if( m_dwLoopProtectTime > 0 )
		m_dwProtectEndTime = m_dwMotionEndTime + m_dwLoopProtectTime;

	m_iCurFireWeapon = 0;

	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();

	float fGauge = pChar->ReduceCurSkillGaugeBySkill( GetOwnerItem(), pChar->GetCurItemSkillGauge( GetOwnerItem()->GetName() ) );

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SSC_ZERO_AIM_FIRE;
		kPacket << m_szTargetName;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioTargetMissileSkill::SetAimFireState( ioBaseChar *pChar )
{
	if( m_SkillState == SS_AIM_FIRE )	return;
	if( !pChar )	return;

	m_SkillState = SS_AIM_FIRE;
	pChar->SetSkillMoveEnable( false, false );

	ioEntityGroup *pGrp = pChar->GetGroup();
	ioHashString szSkillMotion = GetSkillMotionName();

	int iCurSkillAniID = pGrp->GetAnimationIdx( szSkillMotion );
	if( iCurSkillAniID == -1 )	return;

	float fAniRate = GetSkillMotionRate();

	float fTimeRate = fAniRate * m_fExtraAniRate;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iCurSkillAniID, FLOAT100, 5.0f, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	m_dwMotionStartTime = m_dwMotionEndTime = FRAMEGETTIME();
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iCurSkillAniID );
	m_dwMotionEndTime += m_dwSkillEndDelay;

	DestroyAimMarker();

	if( m_iSelectBaseBullut > 0 )
	{
		m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();

		pChar->SkillFireExplicit( m_AimWeaponInfo,
								  GetName(),
								  m_dwWeaponBaseIndex,
								  m_MissileInfoList[m_iSelectBaseBullut-1].vPosition );
	}

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SSC_AIM_FIRE;
		kPacket << m_szTargetName;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

	m_szTargetName.Clear();
}

void ioTargetMissileSkill::SetEndState( ioBaseChar *pChar )
{
	if( m_SkillState == SS_END )	return;

	m_SkillState = SS_END;

	DestroyGuideMarker( pChar );
	DestroyAimMarker();
	DestroyAllWeaponEffect();
}

void ioTargetMissileSkill::SetLoopMotion( ioBaseChar *pChar )
{
	if( !pChar )	return;
	if( m_dwLoopStartTime > 0 )	return;

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwLoopStartTime = dwCurTime;

	ioEntityGroup *pGrp = pChar->GetGroup();

	int iCurSkillAniID = pGrp->GetAnimationIdx( m_szZeroAimLoopMotion );
	if( iCurSkillAniID == -1 )	return;

	float fAniRate = FLOAT1;
	if( m_fZeroAimLoopMotionRate > 0.0f )
		fAniRate = m_fZeroAimLoopMotionRate;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetLoopAni( iCurSkillAniID, FLOAT100, FLOAT1, FLOAT1/fAniRate );

	m_dwMotionStartTime = m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += m_dwZeroAimLoopDuration;
}

void ioTargetMissileSkill::ProcessAimFireState( ioBaseChar *pChar )
{
	if( !pChar )	return;
	
	if( m_dwMotionEndTime < FRAMEGETTIME() )
	{
		SetNormalState( pChar );
		m_szTargetName.Clear();
	}
}

void ioTargetMissileSkill::ProcessZeroAimFireState( ioBaseChar *pChar )
{
	if( !pChar )				return;
	if( m_iCurFireWeapon == -1 )return;

	if( m_dwLoopStartTime == 0 && m_dwMotionEndTime < FRAMEGETTIME() )
	{
		SetLoopMotion( pChar );
	}

	if( m_dwZeroAimedFireTime != 0 && m_dwZeroAimedFireTime + m_dwFireGapTime < FRAMEGETTIME() )
	{
		// 충돌체 생성.
		DWORD dwWeaponIndex = m_dwWeaponBaseIndex + m_iCurFireWeapon;
		
		WeaponInfo	Info;
		int iWeaponCount = m_MissileInfoList.size() - 1;
		if( m_iCurFireWeapon < iWeaponCount )
			Info = m_ZeroAimWeaponInfo;
		else
			Info = m_FinishWeaponInfo;
		
		pChar->SkillFireExplicit( Info,	GetName(), dwWeaponIndex,
								  m_MissileInfoList[m_iCurFireWeapon].vPosition );

		m_dwZeroAimedFireTime = FRAMEGETTIME();

		DestroyWeaponEffect( m_iCurFireWeapon );

		m_iCurFireWeapon++;	
	}

	if( m_iCurFireWeapon == m_iMissileCount )
	{
		DestroyAimMarker();
		m_szTargetName.Clear();
		
		m_dwZeroAimedFireTime = 0;
		if( m_dwLoopStartTime + m_dwZeroAimLoopDuration < FRAMEGETTIME() )
			SetEndState( pChar );
	}
}

void ioTargetMissileSkill::CheckNormalMoveAni( ioBaseChar* pChar )
{
	if( !pChar )	return;

	DWORD dwCurTime = FRAMEGETTIME();
	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = 0;

	if( pChar->IsSettedDirection() )
	{
		if( !m_bSetMoveAni )
		{
			iAniID = pGrp->GetAnimationIdx( m_szNormalMoveAnimation );
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
			iAniID = pGrp->GetAnimationIdx( m_szNormalDelayAnimation );
			if( iAniID == -1 )
				return;

			pGrp->ClearAllActionAni( FLOAT100, true );
			pGrp->ClearAllLoopAni( FLOAT100, true );

			pGrp->SetLoopAni( iAniID, FLOAT100 );
			m_bSetMoveAni = false;
		}
		
		ioPlayStage *pStage = pChar->GetCreator();
		if( pStage && !m_szTargetName.IsEmpty() )
		{
			ioBaseChar *pTarget = pStage->GetBaseChar( m_szTargetName );
			if( pTarget )
				pChar->SetTargetRotToTargetPos( pTarget->GetWorldPosition(), true, false );
		}
	}
}

int ioTargetMissileSkill::CheckInputByAimTime( ioBaseChar* pChar )
{
	if( !pChar )	return CA_NONE;

	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwAimStartTime != 0 && m_dwAimStartTime + m_dwAimDuration > dwCurTime )
	{
		if( m_dwMotionEndTime < dwCurTime && pChar->IsAttackKey() )
		{
			m_dwAimStartTime = 0;
			return CA_AIM_FIRE;
		}
	}
	else if( m_dwZeroAimStartTime != 0 && m_dwZeroAimStartTime + m_dwZeroAimDuration > dwCurTime )
	{
		if( m_dwMotionEndTime < dwCurTime && pChar->IsAttackKey() )
		{
			m_dwZeroAimStartTime = 0;
			return CA_ZERO_AIM_FIRE;
		}
	}
	else if( m_dwAimStartTime != 0 && m_dwAimStartTime + m_dwAimDuration <= dwCurTime )
	{
		m_dwAimStartTime = 0;
		m_dwZeroAimStartTime = dwCurTime;
		
		if( m_dwMotionEndTime < dwCurTime && pChar->IsAttackKey() )
		{
			return CA_AIM_FIRE;
		}
		return CA_NONE;
	}
	else if( m_dwZeroAimStartTime != 0 && m_dwZeroAimStartTime + m_dwZeroAimDuration <= dwCurTime )
	{
		m_dwZeroAimStartTime = 0;
		m_dwAimStartTime = dwCurTime;
		
		if( m_dwMotionEndTime < dwCurTime && pChar->IsAttackKey() )
		{
			return CA_ZERO_AIM_FIRE;
		}
		return CA_NONE;
	}
	else if( m_dwMotionEndTime < dwCurTime && pChar->IsAttackKey() )
	{
		return CA_AIM_FIRE;
	}
	
	return CA_NONE;
}

void ioTargetMissileSkill::FindTarget( ioBaseChar *pOwner )
{
	if( !pOwner )	return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )	return;

	ioHashString	szTarget;
	float			fPreLengthSq = m_fTargetRange * m_fTargetRange;

	D3DXQUATERNION qtRot = pOwner->GetTargetRot();
	D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vDir, &vDir );

	const BaseCharList &rkCharList = pStage->GetBaseCharList();

	BaseCharList::const_iterator iter;
	for( iter=rkCharList.begin() ; iter!=rkCharList.end() ; ++iter )
	{
		ioBaseChar *pCurChar = *iter;

		if( pCurChar == pOwner )
			continue;

		if( pCurChar->GetTeam() == pOwner->GetTeam() )
			continue;

		if( !IsEnableTargetState( pCurChar, true ) )
			continue;

		D3DXVECTOR3 vDiff = pCurChar->GetMidPositionByRate() - pOwner->GetMidPositionByRate();
		float fLengthSq = D3DXVec3LengthSq( &vDiff );

		float fMaxRangeSq = m_fTargetRange * m_fTargetRange;
		float fMinRangeSq = m_fTargetMinRange * m_fTargetMinRange;
		if( !COMPARE( fLengthSq, fMinRangeSq, fMaxRangeSq ) )
			continue;

		D3DXVec3Normalize( &vDiff, &vDiff );
		float fValue = D3DXVec3Dot( &vDir, &vDiff );
		float fAngleGap = RADtoDEG( acosf(fValue) );
		if( fAngleGap >= m_fTargetAngle )
			continue;

		if( m_szTargetName == pCurChar->GetCharName() )
		{
			return;
		}

		if( fLengthSq < fPreLengthSq )
		{
			fPreLengthSq = fLengthSq;
			szTarget = pCurChar->GetCharName();
		}		
	}

	DestroyAimMarker();

	m_szTargetName = szTarget;
	if( !m_szTargetName.IsEmpty() )
	{
		m_dwAimStartTime = FRAMEGETTIME();
		CreateAimMarker();
	}
	else
	{
		m_dwAimStartTime = 0;
		m_dwZeroAimStartTime = 0;
	}
}

bool ioTargetMissileSkill::IsEnableTargetState( ioBaseChar *pTarget, bool bCheckTargetWound )
{
	if( !pTarget )
		return false;

	if( pTarget->IsSystemState() )
		return false;
			
	if( !pTarget->IsEnableTarget() )
		return false;

	ioPlayStage *pStage = pTarget->GetCreator();
	if( pStage )
	{
		ioPlayMode *pMode = pStage->GetPlayMode();
		if( pMode && pMode->GetModeType() == MT_FOOTBALL )
			return false;
	}

	if( bCheckTargetWound )
	{
		if( !pTarget->CheckTargetWoundType(m_TargetWoundType) )
			return false;
	}

	return true;
}

void ioTargetMissileSkill::CreateGuideMarker( ioBaseChar *pChar )
{
	if( !pChar )					return;
	if( m_dwGuideMakerID != -1 )	return;
	if( m_dwAimMarkerID != -1 )		return;
	if( m_dwZeroAimMarkerID != -1 )	return;
	if( !pChar->IsNeedProcess() )	return;

	// 표적 가이드 이펙트
	ioPlayStage *pStage = pChar->GetCreator();

	if( pStage )
	{
		D3DXVECTOR3 vScale( FLOAT1, FLOAT1, FLOAT1 );
		D3DXVECTOR3 vPos = pChar->GetWorldPosition();
		D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		vPos.y = pChar->GetMidHeightByRate();
		vPos += ( vDir * m_fTargetRange );

		ioMapEffect *pMapEffect = pStage->CreateMapEffect( m_szGuideMarker, vPos, vScale );
		if( pMapEffect )
		{
			m_dwGuideMakerID = pMapEffect->GetUniqueID();

			D3DXQUATERNION qtCharRot;
			ioMath::CalcDirectionQuaternion( qtCharRot, -vDir );
			pMapEffect->SetWorldOrientation( qtCharRot );
		}
	}
}

void ioTargetMissileSkill::UpdateGuideMarker( ioBaseChar *pChar )
{
	if( !pChar )					return;
	if( !pChar->IsNeedProcess() )	return;

	if( m_dwAimMarkerID != -1 || m_dwZeroAimMarkerID != -1)
	{
		DestroyGuideMarker( pChar );
		return;
	}

	if( m_dwGuideMakerID == -1 )
	{
		CreateGuideMarker( pChar );
		return;
	}

	D3DXVECTOR3 vScale( FLOAT1, FLOAT1, FLOAT1 );
	D3DXVECTOR3 vPos = pChar->GetWorldPosition();
	D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	vPos.y = pChar->GetMidHeightByRate();
	vPos += ( vDir * m_fTargetRange );

	ioPlayStage *pStage = pChar->GetCreator();
	if( pStage )
	{
		ioMapEffect *pMapEffect = pStage->FindMapEffect( m_dwGuideMakerID );
		if( pMapEffect )
		{
			pMapEffect->SetWorldPosition( vPos );

			D3DXQUATERNION qtCharRot;
			ioMath::CalcDirectionQuaternion( qtCharRot, -vDir );
			pMapEffect->SetWorldOrientation( qtCharRot );
		}
	}
}

void ioTargetMissileSkill::DestroyGuideMarker( ioBaseChar *pChar )
{
	if( !pChar )	return;
	if( !pChar->IsNeedProcess() )	return;

	if( m_dwGuideMakerID != -1 )
	{
		pChar->EndMapEffect( m_dwGuideMakerID, false );
		m_dwGuideMakerID = -1;
	}
}

void ioTargetMissileSkill::CreateAimMarker()
{
	if( m_dwAimMarkerID != -1 )
		return;

	ioPlayStage* pStage = g_SkillMaker.GetPlayStage();
	if( pStage )
	{
		ioBaseChar *pTarget = pStage->GetBaseChar( m_szTargetName );
		if( pTarget )
		{
			D3DXVECTOR3 vScale( FLOAT1, FLOAT1, FLOAT1 );
			D3DXVECTOR3 vTargetPos = pTarget->GetWorldPosition();
			vTargetPos.y = pTarget->GetMidHeightByRate();

			ioMapEffect *pMapEffect = pStage->CreateMapEffect( m_szAimMarker, vTargetPos, vScale );
			if( pMapEffect )
			{
				m_dwAimMarkerID = pMapEffect->GetUniqueID();
			}
		}
	}
}

void ioTargetMissileSkill::UpdateAimMarker()
{
	DWORD dwCurTime = FRAMEGETTIME();

	ioPlayStage* pStage = g_SkillMaker.GetPlayStage();
	if( pStage )
	{
		ioBaseChar *pTarget = pStage->GetBaseChar( m_szTargetName );
		if( pTarget )
		{
			D3DXVECTOR3 vScale( FLOAT1, FLOAT1, FLOAT1 );
			D3DXVECTOR3 vTargetPos = pTarget->GetWorldPosition();
			vTargetPos.y = pTarget->GetMidHeightByRate();

			if( m_dwAimStartTime != 0 && m_dwAimStartTime + m_dwAimDuration <= dwCurTime )
			{
				if( m_dwAimMarkerID != -1)
				{
					pTarget->EndMapEffect( m_dwAimMarkerID, false );
					m_dwAimMarkerID = -1;
				}
				if( m_dwZeroAimMarkerID == -1)
				{
					ioMapEffect *pMapEffect = pStage->CreateMapEffect( m_szZeroAimMarker, vTargetPos, vScale );
					if( pMapEffect )
					{
						m_dwZeroAimMarkerID = pMapEffect->GetUniqueID();
					}
				}
			}
			else if( m_dwZeroAimStartTime != 0 && m_dwZeroAimStartTime + m_dwZeroAimDuration <= dwCurTime )
			{
				if( m_dwAimMarkerID == -1 )
				{
					ioMapEffect *pMapEffect = pStage->CreateMapEffect( m_szAimMarker, vTargetPos, vScale );
					if( pMapEffect )
					{
						m_dwAimMarkerID = pMapEffect->GetUniqueID();
					}
				}
				if( m_dwZeroAimMarkerID != -1 )
				{
					pTarget->EndMapEffect( m_dwZeroAimMarkerID, false );
					m_dwZeroAimMarkerID = -1;
				}
			}
			else
			{
				if( m_dwAimMarkerID != -1 )
				{
					ioMapEffect *pMapEffect = pStage->FindMapEffect( m_dwAimMarkerID );
					if( pMapEffect )
					{
						pMapEffect->SetWorldPosition( vTargetPos );
					}
				}
				if( m_dwZeroAimMarkerID != -1 )
				{
					ioMapEffect *pMapEffect = pStage->FindMapEffect( m_dwZeroAimMarkerID );
					if( pMapEffect )
					{
						pMapEffect->SetWorldPosition( vTargetPos );
					}
				}
			}
		}
	}
}

void ioTargetMissileSkill::DestroyAimMarker()
{
	ioPlayStage* pStage = g_SkillMaker.GetPlayStage();
	if( pStage )
	{
		if( m_dwAimMarkerID != -1 )
		{
			ioMapEffect *pAimMarkerEffect = pStage->FindMapEffect( m_dwAimMarkerID );
			if( pAimMarkerEffect )
			{
				pAimMarkerEffect->EndEffectForce();
				m_dwAimMarkerID = -1;
			}
		}
		if( m_dwZeroAimMarkerID != -1)
		{
			ioMapEffect *pZeroAimMarkerEffect = pStage->FindMapEffect( m_dwZeroAimMarkerID );
			if( pZeroAimMarkerEffect )
			{
				pZeroAimMarkerEffect->EndEffectForce();
				m_dwZeroAimMarkerID = -1;
			}
		}

		m_dwAimStartTime		= 0;
		m_dwZeroAimStartTime	= 0;
	}
}

void ioTargetMissileSkill::CreateWeaponEffect( ioBaseChar *pChar )
{
	if( !pChar )	return;

	ioPlayStage* pStage = g_SkillMaker.GetPlayStage();
	if( pStage )
	{
		for( int i=0; i<m_iMissileCount; ++i )
		{
			D3DXVECTOR3 vScale = pChar->GetWorldScale();
			D3DXVECTOR3 vPos = CalcWeaponEffectPosition( i, pChar );

			if( vPos == ioMath::VEC3_ZERO )
				vPos = pChar->GetWorldPosition();

			ioMapEffect *pMapEffect = pStage->CreateMapEffect( m_szWeaponEffect, vPos, vScale );
			if( pMapEffect )
			{
				m_WeaponEffectID.push_back( pMapEffect->GetUniqueID() );

				D3DXQUATERNION qtCharRot, qtWeaponRot;

				D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
				ioMath::CalcDirectionQuaternion( qtCharRot, -vDir );
				ioMath::CalcDirectionQuaternion( qtWeaponRot, -ioMath::UNIT_Y );

				qtWeaponRot *= qtCharRot;
				pMapEffect->SetWorldOrientation( qtWeaponRot );

				m_MissileInfoList[i].vPosition = vPos;
			}
		}
	}
}

void ioTargetMissileSkill::UpdateWeaponEffect( ioBaseChar *pChar )
{
	if( !pChar )					return;
	if( m_WeaponEffectID.empty() )	return;

	ioPlayStage* pStage = g_SkillMaker.GetPlayStage();

	for( int i=0; i<m_iMissileCount; ++i )
	{
		D3DXVECTOR3 vPos = CalcWeaponEffectPosition( i, pChar );

		if( vPos == ioMath::VEC3_ZERO )
			vPos = pChar->GetWorldPosition();

		ioMapEffect *pMapEffect = pStage->FindMapEffect( m_WeaponEffectID[i] );
		if( pMapEffect )
		{	
			pMapEffect->SetWorldPosition( vPos );

			D3DXQUATERNION qtCharRot, qtWeaponRot;

			D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			ioMath::CalcDirectionQuaternion( qtCharRot, -vDir );
			ioMath::CalcDirectionQuaternion( qtWeaponRot, -ioMath::UNIT_Y );

			qtWeaponRot *= qtCharRot;
			pMapEffect->SetWorldOrientation( qtWeaponRot );

			m_MissileInfoList[i].vPosition = vPos;
		}
	}
}

void ioTargetMissileSkill::DestroyAllWeaponEffect()
{
	if( m_WeaponEffectID.empty() )
		return;

	ioPlayStage* pStage = g_SkillMaker.GetPlayStage();

	int iCnt = m_WeaponEffectID.size();
	for( int i=0; i<iCnt; ++i )
	{
		ioMapEffect *pMapEffect = pStage->FindMapEffect( m_WeaponEffectID[i] );
		if( pMapEffect )
		{
			pMapEffect->EndEffectForce();
		}
	}

	m_WeaponEffectID.clear();
}

void ioTargetMissileSkill::DestroyWeaponEffect( int WeaponNum )
{
	if( m_WeaponEffectID.empty() )
		return;

	if( m_WeaponEffectID.size() < (size_t)WeaponNum )
		return;

	ioPlayStage* pStage = g_SkillMaker.GetPlayStage();

	ioMapEffect *pMapEffect = pStage->FindMapEffect( m_WeaponEffectID[WeaponNum] );
	if( pMapEffect )
	{
		pMapEffect->EndEffectForce();
	}

	m_WeaponEffectID[WeaponNum] = -1;
}

D3DXVECTOR3 ioTargetMissileSkill::CalcWeaponEffectPosition( int iCount, ioBaseChar* pChar )
{
	if( !pChar )
		return ioMath::VEC3_ZERO;

	D3DXVECTOR3	vPos = pChar->GetWorldPosition();
	D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

	// 웨폰 이펙트의 상하 움직임.
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();
	float fRate = (fTimePerSec*1000) / m_dwUpDownSpeed;
	float fAddHeight = 360.0f * fRate;

	m_fRadiansForHeight += fAddHeight;
	if( m_fRadiansForHeight > 360.0f )
		m_fRadiansForHeight = 0.0f;

	float fHeightOffset = sin( DEGtoRAD(m_fRadiansForHeight) );
	fHeightOffset *= m_fUpDownOffset;

	// 일반적으로 스케일은 z, y, z값이 동일하게 가므로 x성분만으로 체크한다.
	float fScaleOffset = pChar->GetWorldScale().x;
	float fCurAngle = 0.0f;
	float fCurRange = 0.0f;
	float fCurHeight = 0.0f;

	fCurAngle = m_MissileInfoList[iCount].fAngle;
	fCurRange = m_MissileInfoList[iCount].fRangeOffset * fScaleOffset;
	fCurHeight = m_MissileInfoList[iCount].fHeightOffset * fScaleOffset;

	D3DXQUATERNION qtAngle;
	D3DXQuaternionRotationYawPitchRoll( &qtAngle, DEGtoRAD(fCurAngle), 0.0f, 0.0f );
	vDir = qtAngle * vDir;

	if( fCurRange > 0.0f )
		vPos += vDir*fCurRange;

	if( fCurHeight > 0.0f )
	{
		vPos.y += fCurHeight + fHeightOffset;
	}

	return vPos;
}