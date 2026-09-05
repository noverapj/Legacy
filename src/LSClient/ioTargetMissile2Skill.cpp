
#include "stdafx.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"
#include "ioGrowthUpInfo.h"

#include "ioTargetMissile2Skill.h"

#include "WeaponDefine.h"

ioTargetMissile2Skill::ioTargetMissile2Skill()
{
	ClearData();
}

ioTargetMissile2Skill::ioTargetMissile2Skill( const ioTargetMissile2Skill &rhs )
	: ioNormalSkill( rhs ),
	m_bDisableGrowth( rhs.m_bDisableGrowth ),
	m_fGuagePerTic( rhs.m_fGuagePerTic ),
	m_dwTicTime( rhs.m_dwTicTime ),
	m_dwSkillEndDelay( rhs.m_dwSkillEndDelay ),
	m_szChargeAimFireMotion( rhs.m_szChargeAimFireMotion ),
	m_fChargeAimFireAniTimeRate( rhs.m_fChargeAimFireAniTimeRate ),
	m_dwChargeAimFireEndDelay( rhs.m_dwChargeAimFireEndDelay ),
	m_szChargeAimLoopMotion( rhs.m_szChargeAimLoopMotion ),
	m_dwChargeAimLoopDuration( rhs.m_dwChargeAimLoopDuration ),
	m_fChargeAimLoopMotionRate( rhs.m_fChargeAimLoopMotionRate ),
	m_dwLoopProtectTime( rhs.m_dwLoopProtectTime ),
	m_bDirection( rhs.m_bDirection ),
	m_szNormalDelayAnimation( rhs.m_szNormalDelayAnimation ),
	m_szNormalMoveAnimation( rhs.m_szNormalMoveAnimation ),
	m_szGuideMarker( rhs.m_szGuideMarker ),
	m_szAimMarker( rhs.m_szAimMarker ),
	m_szZeroAimMarker( rhs.m_szZeroAimMarker ),
	m_dwAimDuration( rhs.m_dwAimDuration ),
	m_dwChargeDuration( rhs.m_dwChargeDuration ),
	m_TargetWoundType( rhs.m_TargetWoundType ),
	m_fTargetAngle( rhs.m_fTargetAngle ),
	m_fTargetRange( rhs.m_fTargetRange ),
	m_fTargetMinRange( rhs.m_fTargetMinRange ),
	m_szWeaponEffect( rhs.m_szWeaponEffect ),
	m_iMissileCount( rhs.m_iMissileCount ),
	m_MissileInfoList( rhs.m_MissileInfoList ),
	m_dwUpDownSpeed( rhs.m_dwUpDownSpeed ),
	m_fUpDownOffset( rhs.m_fUpDownOffset ),
	m_AimWeaponInfo( rhs.m_AimWeaponInfo ),
	m_NoAimWeaponInfo( rhs.m_NoAimWeaponInfo ),
	m_AimChargeWeaponInfo( rhs.m_AimChargeWeaponInfo ),  
	m_dwFireGapTime( rhs.m_dwFireGapTime ),
	m_dwSkillProtectTime( rhs.m_dwSkillProtectTime ),
	m_dwChargeActiveCount( rhs.m_dwChargeActiveCount ),
	m_ChargeMissileInfo( rhs.m_ChargeMissileInfo ),
	m_szChargeWeaponEffect( rhs.m_szChargeWeaponEffect ),
	m_OwnerBuffList( rhs.m_OwnerBuffList ),
	m_dwChargeGapTime( rhs.m_dwChargeGapTime ),
	m_vChargeWeaponList( rhs.m_vChargeWeaponList ),
	m_szNormalLoopAnimation( rhs.m_szNormalLoopAnimation ),
	m_fNomalMotionRate( rhs.m_fNomalMotionRate ),
	m_dwNomalLoopDuration( rhs.m_dwNomalLoopDuration )
{
	ClearData();
}

ioTargetMissile2Skill::~ioTargetMissile2Skill()
{
}

void ioTargetMissile2Skill::ClearData()
{
	m_szTargetName.Clear();

	m_bSetMoveAni	= false;
	m_bSetHandMesh	= false;

	m_dwCurSkillProtectTime = 0;

	m_dwMotionEndTime	= 0;
	m_dwMotionStartTime	= 0;

	m_dwAimStartTime	= 0;
	m_dwAimedFireTime = 0;
	m_dwChargeAimStartTime= 0;

	m_dwChargeAimedFireTime	= 0;

	m_dwAimMarkerID		= -1;
	m_dwZeroAimMarkerID	= -1;
	m_dwGuideMakerID	= -1;
	m_WeaponEffectID.clear();

	m_iCurFireWeapon = 0;

	m_fCurChargeRate = 0.0f;

	// 성장관련 요소
	m_dwCheckTime			= 0;
	m_fTicGaugeRateByGrowth	= FLOAT1;
	m_dwTicTimeByGrowth		= 0;

	m_fRadiansForHeight		= 0.0f;

	m_dwLoopStartTime		= 0;
	m_dwProtectEndTime		= 0;

	m_bFire = false;

	m_dwStartChargeTime = 0;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	m_ChargeWeaponEffectID = -1;
	m_bCharge = false;

	m_dwWeaponChargeTime = 0;
	m_iChargeWeaponCnt = 0;
	m_bDamage = false;
	m_dwNomalLoopStartTime = 0;
}

void ioTargetMissile2Skill::LoadProperty( ioINILoader &rkLoader )
{
	ioNormalSkill::LoadProperty( rkLoader );
	LoadOwnerBuffList( rkLoader );
	LoadChargeWeaponList( rkLoader );
	char szKey[MAX_PATH], szBuf[MAX_PATH];

	m_bDisableGrowth= rkLoader.LoadBool_e( "disable_growth", false );
	m_fGuagePerTic	= rkLoader.LoadFloat_e( "gauge_per_tick", 0.0f );
	m_dwTicTime		= (DWORD)rkLoader.LoadInt_e( "tick_time", 0 );


	// 공격 모션
	m_dwSkillEndDelay = (DWORD)rkLoader.LoadInt_e( "animation_end_delay", 0 );

	// 조준공격모션
	rkLoader.LoadString_e( "charge_aim_fire_motion", "", szBuf, MAX_PATH );	
	m_szChargeAimFireMotion	= szBuf;
	rkLoader.LoadString_e( "charge_aim_fire_loop_motion", "", szBuf, MAX_PATH );
	m_szChargeAimLoopMotion	= szBuf;
	m_fChargeAimFireAniTimeRate	= rkLoader.LoadFloat_e( "charge_aim_fire_motion_time_rate", FLOAT1);
	m_dwChargeAimFireEndDelay = (DWORD)rkLoader.LoadInt_e( "charge_aim_fire_motion_end_delay", 0 );	
	m_dwChargeAimLoopDuration = (DWORD)rkLoader.LoadInt_e( "charge_aim_fire_loop_duration", 0 );
	m_fChargeAimLoopMotionRate	= rkLoader.LoadFloat_e( "charge_aim_fire_loop_motion_rate", 0.0f );
	m_dwLoopProtectTime	= (DWORD)rkLoader.LoadInt_e( "charge_aim_fire_loop_protect_time", 0 );


	// 애니메이션
	m_bDirection = rkLoader.LoadBool_e( "direction", false );
	rkLoader.LoadString_e( "delay_animation", "", szBuf, MAX_PATH ); m_szNormalDelayAnimation	= szBuf;
	
	rkLoader.LoadString_e( "move_animation", "", szBuf, MAX_PATH );	m_szNormalMoveAnimation		= szBuf;

	rkLoader.LoadString_e( "nomal_animation", "", szBuf, MAX_PATH ); m_szNormalLoopAnimation	= szBuf;
	m_fNomalMotionRate	= rkLoader.LoadFloat_e( "nomal_animation_rate", 0.0f );
	m_dwNomalLoopDuration	= (DWORD)rkLoader.LoadInt_e( "nomal_loop_duration", 0 );

	// 조준마크
	rkLoader.LoadString_e( "guide_marker", "", szBuf, MAX_PATH );		m_szGuideMarker				= szBuf;
	rkLoader.LoadString_e( "aim_marker", "", szBuf, MAX_PATH );		m_szAimMarker				= szBuf;
	rkLoader.LoadString_e( "zero_aim_marker", "", szBuf, MAX_PATH );	m_szZeroAimMarker			= szBuf;
	m_dwAimDuration				= (DWORD)rkLoader.LoadInt_e( "aim_duration", 0 );
	m_dwChargeDuration		= (DWORD)rkLoader.LoadInt_e( "charge_duration", 0 );

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
	
	m_dwUpDownSpeed = (DWORD)rkLoader.LoadInt_e( "weapon_updown_speed", 1000 );
	m_fUpDownOffset = rkLoader.LoadFloat_e( "weapon_updown_offset", 0.0f);


	// 발사체1 정보
	wsprintf_e( szKey, "no_aim_attack_attribute_index" );		m_NoAimWeaponInfo.m_iWeaponIdx = rkLoader.LoadInt( szKey, 0 );
	wsprintf_e( szKey, "no_aim_attack_attribute_resist_index" );m_NoAimWeaponInfo.m_iResistanceIdx = rkLoader.LoadInt( szKey, 0 );
	wsprintf_e( szKey, "no_aim_attack_wounded_animation" );	
	rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );			m_NoAimWeaponInfo.m_WoundedAnimation = szBuf;
	wsprintf_e( szKey, "no_aim_attack_wounded_duration" );		m_NoAimWeaponInfo.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );
	wsprintf_e( szKey, "no_aim_attack_wounded_loop_ani" );		m_NoAimWeaponInfo.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );

	// 발사체2 정보
	wsprintf_e( szKey, "aim_attack_attribute_index" );			m_AimWeaponInfo.m_iWeaponIdx = rkLoader.LoadInt( szKey, 0 );
	wsprintf_e( szKey, "aim_attack_attribute_resist_index" );	m_AimWeaponInfo.m_iResistanceIdx = rkLoader.LoadInt( szKey, 0 );
	wsprintf_e( szKey, "aim_attack_wounded_animation" );
	rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );			m_AimWeaponInfo.m_WoundedAnimation = szBuf;
	wsprintf_e( szKey, "aim_attack_wounded_duration" );			m_AimWeaponInfo.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );
	wsprintf_e( szKey, "aim_attack_wounded_loop_ani" );			m_AimWeaponInfo.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );
		
	m_dwFireGapTime = (DWORD)rkLoader.LoadInt_e( "fire_gap_time", 0 );

	m_dwSkillProtectTime = (DWORD)rkLoader.LoadInt_e( "skill_protect_time", 0 );

	m_dwChargeActiveCount = (DWORD)rkLoader.LoadInt_e("charge_active_count", 0 );

	m_ChargeMissileInfo.fAngle = rkLoader.LoadFloat_e( "charge_bullet_angle", 0.0f );
	m_ChargeMissileInfo.fRangeOffset = rkLoader.LoadFloat_e( "charge_bullet_range_offset", 0.0f );
	m_ChargeMissileInfo.fHeightOffset = rkLoader.LoadFloat_e( "charge_bullet_height_offset", 0.0f );

	// 미사일 정보
	rkLoader.LoadString_e( "charge_bullet_effect", "", szBuf, MAX_PATH );
	m_szChargeWeaponEffect = szBuf;

	m_dwChargeGapTime = (DWORD)rkLoader.LoadInt_e( "charge_gap_time", 0 );
}
void ioTargetMissile2Skill::LoadChargeWeaponList( ioINILoader &rkLoader )
{
	int iCnt = rkLoader.LoadInt_e( "charge_bullet_cnt", 0 );
	m_vChargeWeaponList.clear();

	if( iCnt <= 0 ) return;
	m_vChargeWeaponList.reserve( iCnt );

	char szTitle[MAX_PATH], szBuf[MAX_PATH];
	for( int i=0; i < iCnt; ++i )
	{
		WeaponInfo kInfo;
		// 발사체3 정보 
		wsprintf_e( szTitle, "aim_charge_attack_attribute_index%d", i+1 );
		kInfo.m_iWeaponIdx = rkLoader.LoadInt( szTitle, 0 );
		wsprintf_e( szTitle, "aim_charge_attack_attribute_resist_index%d", i+1 );
		kInfo.m_iResistanceIdx = rkLoader.LoadInt( szTitle, 0 );
		wsprintf_e( szTitle, "aim_charge_attack_wounded_animation%d", i+1 );
		rkLoader.LoadString( szTitle, "", szBuf, MAX_PATH );
		kInfo.m_WoundedAnimation = szBuf;
		wsprintf_e( szTitle, "aim_charge_attack_wounded_duration%d", i+1 );
		kInfo.m_dwWoundedDuration = rkLoader.LoadInt( szTitle, 0 );
		wsprintf_e( szTitle, "aim_charge_attack_wounded_loop_ani%d", i+1 );
		kInfo.m_bWoundedLoopAni = rkLoader.LoadBool( szTitle, false );

		m_vChargeWeaponList.push_back( kInfo );
	}
	
}
void ioTargetMissile2Skill::LoadOwnerBuffList( ioINILoader &rkLoader )
{
	int iCnt = rkLoader.LoadInt_e( "owner_buff_cnt", 0 );
	m_OwnerBuffList.clear();

	if( iCnt <= 0 ) return;
	m_OwnerBuffList.reserve( iCnt );

	char szTitle[MAX_PATH], szBuf[MAX_PATH];
	for( int i=0; i < iCnt; ++i )
	{
		wsprintf_e( szTitle, "owner_buff%d", i+1 );
		rkLoader.LoadString( szTitle, "", szBuf, MAX_PATH );

		ioBuffInfo tBuffInfo;
		tBuffInfo.m_BuffName = szBuf;

		if( !tBuffInfo.m_BuffName.IsEmpty() )
		{
			tBuffInfo.m_iBuffType = g_BuffMaker.GetBuffType( tBuffInfo.m_BuffName );
			if( tBuffInfo.m_iBuffType != BT_NONE )
			{
				m_OwnerBuffList.push_back( tBuffInfo );
			}
		}
	}
}

ioSkill* ioTargetMissile2Skill::Clone()
{
	return new ioTargetMissile2Skill( *this );
}

bool ioTargetMissile2Skill::OnSkillStart( ioBaseChar *pChar )
{
	if( !HasSkillMotion() )
		return false;

	if( !pChar )
		return false;

	ioNormalSkill::OnSkillStart( pChar );

	m_SkillState = SS_DELAY;

	ClearData();
	m_iCurActiveCount = m_iMaxActiveCount;
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
	AddOwnerBuff( pChar );

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

void ioTargetMissile2Skill::OnSkillEnd( ioBaseChar *pChar )
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
	RemoveOwnerBuff( pChar );
	DestroyGuideMarker( pChar );
	DestroyAimMarker();
	DestroyAllWeaponEffect();
	DestroyChargeWeaponEffect();

	if( !m_SkillSoundName.IsEmpty() )
	{
		g_SoundMgr.StopSound( m_SkillSoundName, m_SkillSoundID );
		//g_SoundMgr.StopSound( m_SkillSoundName, 0 );
	}

	pChar->ReduceNeedGaugeBySkill( this, GetOwnerItem()->GetMaxSkillGauge( pChar->GetCharName() ) );

	ClearData();
}

void ioTargetMissile2Skill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

	float	fCurTicGauge	= m_fGuagePerTic / m_fTicGaugeRateByGrowth;
	int		iTickGap		= (int)( dwCurTime - m_dwCheckTime );
	int		iCurTicTime		= m_dwTicTime + m_dwTicTimeByGrowth;

	switch( m_SkillState )
	{
	case SS_DELAY:
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
			CheckSetNomalAni( pChar );
			FindTarget( pChar );
			CheckNormalMoveAni( pChar );
			UpdateGuideMarker( pChar );
			UpdateAimMarker();
			UpdateWeaponEffect( pChar );
			CheckDamage( pChar );
			int iCheckInput = CheckInputByAimTime( pChar );

			if( pChar->IsNeedProcess() )
			{
				if( iCheckInput == CA_AIM_FIRE )
				{
					SetAimFireState( pChar, pStage );
				}
				else if ( iCheckInput == CA_LOOP )
				{
					SetLoopState( pChar );
					CreateChargeWeaponEffect( pChar );

					if( pChar->IsNeedSendNetwork() )
					{
						SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
						kPacket << pChar->GetCharName();
						kPacket << GetName();
						kPacket << SSC_LOOP;
						P2PNetwork::SendToAllPlayingUser( kPacket );
					}
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
	case SS_LOOP:
		{
			CheckKeyInput( pChar );
			FindTarget( pChar );
			
			ioPlayStage *pStage = pChar->GetCreator();
			if( pStage && !m_szTargetName.IsEmpty() )
			{
				ioBaseChar *pTarget = pStage->GetBaseChar( m_szTargetName );
				if( pTarget )
					pChar->SetTargetRotToTargetPos( pTarget->GetWorldPosition(), true, false );
			}

			UpdateGuideMarker( pChar );
			UpdateAimMarker();
			UpdateChargeWeaponEffect( pChar );
			
			if( pChar->IsNeedProcess() )
			{
				CheckChargeWeaponTime( pChar );
				if ( m_bCharge )
				{				
					SetChargeAimFireState( pChar, pStage );
				}			
			}
		}
		break;
	case SS_CHARGE_AIM_FIRE:
		{
			UpdateAimMarker();
			UpdateChargeWeaponEffect( pChar );
			ProcessChargeAimFireState( pChar );
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
					if( m_SkillState != SS_CHARGE_AIM_FIRE && m_SkillState != SS_END && m_SkillState != SS_LOOP )
					{
						SetEndState( pChar );
						return;
					}
				}
			}
		}
	}
}

void ioTargetMissile2Skill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	ioTraceWeaponType2	*pTraceWeapon2 = ToTraceWeaponType2( pWeapon );
	ioMissileWeapon		*pMissileWeapon = ToMissileWeapon( pWeapon );
	ioTraceWeapon		*pTraceWeapon = ToTraceWeapon( pWeapon );

	if( pTraceWeapon2 && m_bCharge )
	{
		pTraceWeapon2->SetTraceTarget( m_szTargetName );
		pTraceWeapon2->SetStartPosition( m_ChargeMissileInfo.vPosition, pStage );
	}
	else if( pTraceWeapon2 && !m_bCharge )
	{
		pTraceWeapon2->SetTraceTarget( m_szTargetName );
		pTraceWeapon2->SetStartPosition( m_MissileInfoList[m_iCurFireWeapon].vPosition, pStage );
	}

	if( pTraceWeapon && m_bCharge )
	{
		pTraceWeapon->SetTraceTarget( m_szTargetName );
		pTraceWeapon->SetStartPosition( m_ChargeMissileInfo.vPosition, pStage );
	}
	else if( pTraceWeapon && !m_bCharge )
	{
		pTraceWeapon->SetTraceTarget( m_szTargetName );
		pTraceWeapon->SetStartPosition( m_MissileInfoList[m_iCurFireWeapon].vPosition, pStage );
	}

	if ( pMissileWeapon && m_bCharge )
	{
		D3DXVECTOR3 vDir = ioMath::VEC3_ZERO;
		D3DXVECTOR3 vPos = m_ChargeMissileInfo.vPosition;

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
	else if( pMissileWeapon && !m_bCharge )
	{
		D3DXVECTOR3 vDir = ioMath::VEC3_ZERO;
		D3DXVECTOR3 vPos = m_MissileInfoList[m_iCurFireWeapon].vPosition;

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

void ioTargetMissile2Skill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	int iType;
	rkPacket >> iType;

	switch( iType )
	{
	case SSC_AIM_FIRE:
		{
			rkPacket >> m_szTargetName;
			SetAimFireState( pChar, pStage );
		}
		break;
	case SSC_LOOP:
		{
			SetLoopState( pChar );
			CreateChargeWeaponEffect( pChar );
		}
		break;
	case SSC_KEY:
		{
			D3DXQUATERNION qtRot;
			rkPacket >> qtRot;
			pChar->SetTargetRotToRotate( qtRot, true, false );

			int iDir;
			rkPacket >> iDir;
			m_CurDirKey = (ioUserKeyInput::DirKeyInput)iDir;
		}
		break;
	case SSC_CHARGE_AIM_FIRE:
		{
			rkPacket >> m_szTargetName;
			rkPacket >> m_vChargeWeaponInfo.m_iWeaponIdx;
			rkPacket >> m_bCharge;
			SetChargeAimFireState( pChar, pStage );
		}
		break;
	}
}

bool ioTargetMissile2Skill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

bool ioTargetMissile2Skill::IsProtected( int iDefenseBreakType ) const
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

bool ioTargetMissile2Skill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwPreDelayStartTime > 0 )
		return false;

	switch( m_SkillState )
	{
	case SS_DELAY:
	case SS_NORMAL:
	case SS_AIM_FIRE:
	case SS_CHARGE_AIM_FIRE:
	case SS_LOOP:
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

void ioTargetMissile2Skill::SetNormalState( ioBaseChar *pChar )
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

	float fAniRate = FLOAT1;
	
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fAniRate );

	m_dwMotionStartTime = dwCurTime;
	m_dwMotionEndTime = 0;
	pChar->SetSkillMoveEnable( true, m_bDirection );
}

void ioTargetMissile2Skill::SetNormalLoopState( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	m_SkillState = SS_NORMAL;

	m_bSetMoveAni = false;

	ioEntityGroup *pGrp = pChar->GetGroup();

	int iAniID = 0;
	iAniID = pGrp->GetAnimationIdx( m_szNormalLoopAnimation );
	if( iAniID == -1 )
		return;

	float fAniRate = FLOAT1;
	if( m_fNomalMotionRate > 0.0f )
		fAniRate = m_fNomalMotionRate;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fAniRate );

	m_dwMotionStartTime = m_dwNomalLoopStartTime = dwCurTime;
	m_dwMotionEndTime = 0;
	pChar->SetSkillMoveEnable( true, m_bDirection );
}

void ioTargetMissile2Skill::CheckSetNomalAni( ioBaseChar *pChar )
{
	if( !pChar )
		return;
	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_dwNomalLoopStartTime > 0 && m_dwNomalLoopStartTime + m_dwNomalLoopDuration < dwCurTime )
	{
		ioEntityGroup *pGrp = pChar->GetGroup();
		int iAniID = 0;

		if( !pChar->IsSettedDirection() )
		{
			if( !m_bSetMoveAni )
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

			m_dwNomalLoopStartTime = 0;
		}
	}
}

void ioTargetMissile2Skill::SetChargeAimFireState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( m_SkillState == SS_CHARGE_AIM_FIRE )	return;
	if( !pChar )	return;

	DWORD dwCurTime = FRAMEGETTIME();

	m_SkillState = SS_CHARGE_AIM_FIRE;
	
	ioEntityGroup *pGrp = pChar->GetGroup();

	int iCurSkillAniID = pGrp->GetAnimationIdx( m_szChargeAimFireMotion );
	if( iCurSkillAniID == -1 )	return;
	float fAniRate = m_fChargeAimFireAniTimeRate;

	float fTimeRate = fAniRate * m_fExtraAniRate;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iCurSkillAniID, FLOAT100, 5.0f, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	m_dwMotionStartTime = m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iCurSkillAniID );
	m_dwMotionEndTime += m_dwChargeAimFireEndDelay;

	m_dwAimStartTime	= 0;
	m_dwChargeAimStartTime= 0;
	m_dwChargeAimedFireTime = dwCurTime;

	if( m_dwLoopProtectTime > 0 )
		m_dwProtectEndTime = m_dwMotionEndTime + m_dwLoopProtectTime;

	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();
	m_bFire = true;

	float fGauge = pChar->ReduceCurSkillGaugeBySkill( GetOwnerItem(), pChar->GetCurItemSkillGauge( GetOwnerItem()->GetName() ) );
	
	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SSC_CHARGE_AIM_FIRE;
		kPacket << m_szTargetName;
		kPacket << m_vChargeWeaponInfo.m_iWeaponIdx;
		kPacket << m_bCharge;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioTargetMissile2Skill::SetAimFireState( ioBaseChar *pChar, ioPlayStage *pStage )
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

	m_dwAimedFireTime = FRAMEGETTIME();
	m_dwAimStartTime = 0;
	m_bFire = true;
	
	DestroyAimMarker();
	
	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SSC_AIM_FIRE;
		kPacket << m_szTargetName;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioTargetMissile2Skill::SetEndState( ioBaseChar *pChar )
{
	if( m_SkillState == SS_END )	return;

	m_SkillState = SS_END;

	DestroyGuideMarker( pChar );
	DestroyAimMarker();
	DestroyAllWeaponEffect();
}

void ioTargetMissile2Skill::SetLoopState( ioBaseChar *pChar )
{
	if( !pChar )	return;
	if( m_dwLoopStartTime > 0 )	return;

	m_SkillState = SS_LOOP;
	DWORD dwCurTime = FRAMEGETTIME();
	m_dwLoopStartTime = dwCurTime;

	ioEntityGroup *pGrp = pChar->GetGroup();

	int iCurSkillAniID = pGrp->GetAnimationIdx( m_szChargeAimLoopMotion );
	if( iCurSkillAniID == -1 )	return;

	float fAniRate = FLOAT1;
	if( m_fChargeAimLoopMotionRate > 0.0f )
		fAniRate = m_fChargeAimLoopMotionRate;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetLoopAni( iCurSkillAniID, FLOAT100, FLOAT1, FLOAT1/fAniRate );

	m_dwMotionStartTime = m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += m_dwChargeAimLoopDuration;

	pChar->SetSkillMoveEnable( false, true );
	RemoveOwnerBuff( pChar );
	DestroyAllWeaponEffect();

	m_iCurActiveCount = m_dwChargeActiveCount;
}

void ioTargetMissile2Skill::ProcessAimFireState( ioBaseChar *pChar )
{
	if( !pChar )	return;

	if( m_dwMotionEndTime < FRAMEGETTIME() )
	{
		SetNormalState( pChar );
		m_szTargetName.Clear();
	}
	
	if( m_szTargetName.IsEmpty() && m_bFire )
	{
		DWORD dwWeaponIndex = m_dwWeaponBaseIndex + m_iCurFireWeapon;
		
		WeaponInfo	Info = m_NoAimWeaponInfo;

		pChar->SkillFireExplicit( Info,	GetName(), dwWeaponIndex,
			m_MissileInfoList[m_iCurFireWeapon].vPosition );
		
		DestroyWeaponEffect( m_iCurFireWeapon );
		m_iCurFireWeapon++;
		m_iCurActiveCount = m_iMissileCount - m_iCurFireWeapon;
		m_bFire = false;
	}

	if( m_bFire )
	{
		// 충돌체 생성.
		DWORD dwWeaponIndex = m_dwWeaponBaseIndex + m_iCurFireWeapon;

		WeaponInfo	Info = m_AimWeaponInfo;

		pChar->SkillFireExplicit( Info,	GetName(), dwWeaponIndex,
			m_MissileInfoList[m_iCurFireWeapon].vPosition );

		m_dwAimedFireTime = FRAMEGETTIME();

		DestroyWeaponEffect( m_iCurFireWeapon );
		m_iCurFireWeapon++;
		m_iCurActiveCount = m_iMissileCount - m_iCurFireWeapon;
		m_bFire = false;
	}

	if( m_iCurFireWeapon == m_iMissileCount )
	{
		DestroyAimMarker();
		m_szTargetName.Clear();

		SetEndState( pChar );
	}

	if ( !m_bFire && m_dwAimedFireTime > 0 && m_dwAimedFireTime + m_dwFireGapTime < FRAMEGETTIME() )
	{
		SetNormalLoopState( pChar );
	}
}

void ioTargetMissile2Skill::ProcessChargeAimFireState( ioBaseChar *pChar )
{
	if( !pChar ) return;
	
	if( m_szTargetName.IsEmpty() && m_bFire )
	{
		DWORD dwWeaponIndex = m_dwWeaponBaseIndex;

		WeaponInfo	Info = m_vChargeWeaponInfo;
		pChar->SkillFireExplicit( Info,	GetName(), dwWeaponIndex, m_ChargeMissileInfo.vPosition );

		m_iCurFireWeapon = m_iMissileCount;
		m_bFire = false;
	}

	if( m_dwChargeAimedFireTime != 0 && m_dwChargeAimedFireTime + m_dwFireGapTime < FRAMEGETTIME()
		&& m_bFire )
	{
		// 충돌체 생성.
		DWORD dwWeaponIndex = m_dwWeaponBaseIndex;
		WeaponInfo	Info = m_vChargeWeaponInfo;
		pChar->SkillFireExplicit( Info,	GetName(), dwWeaponIndex, m_ChargeMissileInfo.vPosition );
		
		m_iCurFireWeapon = m_iMissileCount;
		m_bFire = false;
	}
	
	if( m_iCurFireWeapon == m_iMissileCount )
	{
		DestroyChargeWeaponEffect();
		DestroyAimMarker();

		m_dwChargeAimedFireTime = 0;
		if( m_dwLoopStartTime + m_dwChargeAimLoopDuration < FRAMEGETTIME() )
			SetEndState( pChar );
	}
}

void ioTargetMissile2Skill::CheckNormalMoveAni( ioBaseChar* pChar )
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

int ioTargetMissile2Skill::CheckInputByAimTime( ioBaseChar* pChar )
{
	if( !pChar )	return CA_NONE;

	DWORD dwCurTime = FRAMEGETTIME();

	if ( m_dwStartChargeTime == 0 )
	{
		if ( pChar->IsAttackKeyDown() )
		{
			m_dwStartChargeTime = dwCurTime;
		}
	}
	else
	{
		if ( pChar->IsAttackKeyDown() && m_dwStartChargeTime + m_dwChargeDuration < dwCurTime )
		{
			m_dwStartChargeTime = 0;
			m_dwWeaponChargeTime = dwCurTime;
			return CA_LOOP;
		}
		else if( !pChar->IsAttackKeyDown() )
		{
			if ( m_dwMotionEndTime < dwCurTime )
			{
				m_dwAimStartTime = 0;
				m_dwStartChargeTime = 0;
				return CA_AIM_FIRE;
			}			
		}
	}

	return CA_NONE;
}

void ioTargetMissile2Skill::FindTarget( ioBaseChar *pOwner )
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
	}
}

bool ioTargetMissile2Skill::IsEnableTargetState( ioBaseChar *pTarget, bool bCheckTargetWound )
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

void ioTargetMissile2Skill::CreateGuideMarker( ioBaseChar *pChar )
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

void ioTargetMissile2Skill::UpdateGuideMarker( ioBaseChar *pChar )
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

void ioTargetMissile2Skill::DestroyGuideMarker( ioBaseChar *pChar )
{
	if( !pChar )	return;
	if( !pChar->IsNeedProcess() )	return;

	if( m_dwGuideMakerID != -1 )
	{
		pChar->EndMapEffect( m_dwGuideMakerID, false );
		m_dwGuideMakerID = -1;
	}
}

void ioTargetMissile2Skill::CreateAimMarker()
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

void ioTargetMissile2Skill::UpdateAimMarker()
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
			else if( m_dwChargeAimStartTime != 0 && m_dwChargeAimStartTime + m_dwChargeDuration <= dwCurTime )
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

void ioTargetMissile2Skill::DestroyAimMarker()
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
		m_dwChargeAimStartTime	= 0;
	}
}

void ioTargetMissile2Skill::CreateWeaponEffect( ioBaseChar *pChar )
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

void ioTargetMissile2Skill::UpdateWeaponEffect( ioBaseChar *pChar )
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

void ioTargetMissile2Skill::DestroyAllWeaponEffect()
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

void ioTargetMissile2Skill::DestroyWeaponEffect( int WeaponNum )
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

D3DXVECTOR3 ioTargetMissile2Skill::CalcWeaponEffectPosition( int iCount, ioBaseChar* pChar )
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

void ioTargetMissile2Skill::CreateChargeWeaponEffect( ioBaseChar *pChar )
{
	if( !pChar )	return;

	ioPlayStage* pStage = g_SkillMaker.GetPlayStage(); 
	if( pStage )
	{
		D3DXVECTOR3 vScale = pChar->GetWorldScale();
		D3DXVECTOR3 vPos = CalcChargeWeaponEffectPosition( pChar );
		if( vPos == ioMath::VEC3_ZERO )
			vPos = pChar->GetWorldPosition();

		ioMapEffect *pMapEffect = pStage->CreateMapEffect( m_szChargeWeaponEffect, vPos, vScale );
		if( pMapEffect )
		{
			m_ChargeWeaponEffectID = pMapEffect->GetUniqueID();
			D3DXQUATERNION qtCharRot, qtWeaponRot;

			D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			ioMath::CalcDirectionQuaternion( qtCharRot, -vDir );
			ioMath::CalcDirectionQuaternion( qtWeaponRot, -ioMath::UNIT_Y );

			qtWeaponRot *= qtCharRot;
			pMapEffect->SetWorldOrientation( qtWeaponRot );

			m_ChargeMissileInfo.vPosition = vPos;
		}
	}
}

D3DXVECTOR3 ioTargetMissile2Skill::CalcChargeWeaponEffectPosition( ioBaseChar* pChar )
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

	fCurAngle = m_ChargeMissileInfo.fAngle;
	fCurRange = m_ChargeMissileInfo.fRangeOffset * fScaleOffset;
	fCurHeight = m_ChargeMissileInfo.fHeightOffset * fScaleOffset;

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

void ioTargetMissile2Skill::UpdateChargeWeaponEffect( ioBaseChar *pChar )
{
	if( !pChar )					return;
	if( m_ChargeWeaponEffectID == -1 )	return;

	ioPlayStage* pStage = g_SkillMaker.GetPlayStage();

	
	D3DXVECTOR3 vPos = CalcChargeWeaponEffectPosition( pChar );

	if( vPos == ioMath::VEC3_ZERO )
		vPos = pChar->GetWorldPosition();

	ioMapEffect *pMapEffect = pStage->FindMapEffect( m_ChargeWeaponEffectID );
	if( pMapEffect )
	{	
		pMapEffect->SetWorldPosition( vPos );

		D3DXQUATERNION qtCharRot, qtWeaponRot;

		D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		ioMath::CalcDirectionQuaternion( qtCharRot, -vDir );
		ioMath::CalcDirectionQuaternion( qtWeaponRot, -ioMath::UNIT_Y );

		qtWeaponRot *= qtCharRot;
		pMapEffect->SetWorldOrientation( qtWeaponRot );

		m_ChargeMissileInfo.vPosition = vPos;
	}	
}

void ioTargetMissile2Skill::CheckKeyInput( ioBaseChar *pChar )
{
	if( !pChar ) return;
	if( !pChar->IsNeedProcess() ) return;

	ioUserKeyInput::DirKeyInput eNewDirKey = pChar->CheckCurDirKey();
	if( eNewDirKey != m_CurDirKey )
	{	
		m_CurDirKey = eNewDirKey;
		pChar->SetTargetRotToDirKey( m_CurDirKey );

		if( pChar->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
			kPacket << pChar->GetCharName();
			kPacket << GetName();
			kPacket << SSC_KEY;
			kPacket << pChar->GetTargetRot();				
			kPacket << (int)m_CurDirKey;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioTargetMissile2Skill::DestroyChargeWeaponEffect()
{
	if( m_ChargeWeaponEffectID == -1 )
		return;

	ioPlayStage* pStage = g_SkillMaker.GetPlayStage();

	ioMapEffect *pMapEffect = pStage->FindMapEffect( m_ChargeWeaponEffectID );
	if( pMapEffect )
	{
		pMapEffect->EndEffectForce();
	}
	m_ChargeWeaponEffectID = -1;
}

bool ioTargetMissile2Skill::IsEndState() const
{
	if ( m_SkillState != SS_END )
		return false;

	return true;
}

void ioTargetMissile2Skill::AddOwnerBuff( ioBaseChar *pChar )
{
	if( !pChar )	return;
	if( m_OwnerBuffList.empty() ) return;

	int iCnt = m_OwnerBuffList.size();
	for( int i = 0; i < iCnt; ++i )
	{
		pChar->AddNewBuff( m_OwnerBuffList[i].m_BuffName, pChar->GetCharName(), "", this );
	}
}

void ioTargetMissile2Skill::RemoveOwnerBuff( ioBaseChar *pChar )
{
	if( !pChar )	return;
	if( m_OwnerBuffList.empty() ) return;
	int iCnt = m_OwnerBuffList.size();

	for( int i=0; i<iCnt; ++i)
	{
		ioBuff* pBuff = pChar->GetBuff(m_OwnerBuffList[i].m_BuffName );
		if( pBuff )
		{
			pBuff->SetReserveEndBuff();
		}
	}
}

void ioTargetMissile2Skill::CheckChargeWeaponTime( ioBaseChar *pChar )
{
	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_dwWeaponChargeTime > 0 && m_dwWeaponChargeTime + m_dwChargeGapTime < dwCurTime )
	{
		m_dwWeaponChargeTime = dwCurTime;
		m_iChargeWeaponCnt++;
		int iCnt = m_vChargeWeaponList.size();
		if ( m_iChargeWeaponCnt >= iCnt )
				m_iChargeWeaponCnt = iCnt - 1;
		if ( !pChar->IsAttackKeyDown() )
		{
			m_vChargeWeaponInfo = m_vChargeWeaponList[m_iChargeWeaponCnt];
			m_bCharge = true;
		}		
	}
	else if ( !pChar->IsAttackKeyDown() )
	{
		m_vChargeWeaponInfo = m_vChargeWeaponList[m_iChargeWeaponCnt];
		m_bCharge = true;
	}
}

void ioTargetMissile2Skill::CheckDamage( ioBaseChar *pChar )
{
	if ( m_bDamage )
	{
		DestroyWeaponEffect( m_iCurFireWeapon );
		m_iCurFireWeapon++;
		m_iCurActiveCount = m_iMissileCount - m_iCurFireWeapon;
		m_bDamage = false;

		if ( m_iCurActiveCount <= 0 )
		{
			SetEndState( pChar );
		}
	}
}

