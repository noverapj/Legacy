
#include "stdafx.h"

#include "ioBaseChar.h"

#include "ioFigureskatingSkill.h"

#include "ioGrowthUpInfo.h"

#include "WeaponDefine.h"

ioFigureskatingSkill::ioFigureskatingSkill()
{
	ClearData();

	m_bSetHandMesh = false;
}

ioFigureskatingSkill::ioFigureskatingSkill( const ioFigureskatingSkill &rhs )
	: ioAttackSkill( rhs ),
	m_bDisableGrowth( rhs.m_bDisableGrowth ),
	m_fGuagePerTic( rhs.m_fGuagePerTic ),
	m_dwTicTime( rhs.m_dwTicTime ),
	m_dwLoopTime( rhs.m_dwLoopTime ),
	m_bUseFireEnd( rhs.m_bUseFireEnd ),
	m_bDirection( rhs.m_bDirection ),
	m_RunningReadyAni( rhs.m_RunningReadyAni ),
	m_RunningEndAni( rhs.m_RunningEndAni ),
	m_fRunningReadyAniRate( rhs.m_fRunningReadyAniRate ),
	m_fRunningEndAniRate( rhs.m_fRunningEndAniRate ),
	m_fRunSpeed( rhs.m_fRunSpeed ),
	m_dwRotateTime( rhs.m_dwRotateTime ),
	m_bDisableRunColSkip( rhs.m_bDisableRunColSkip ),
	m_iMaxWeaponWay( rhs.m_iMaxWeaponWay ),
	m_BulletInfoList( rhs.m_BulletInfoList ),
	m_szLoopAnimation( rhs.m_szLoopAnimation ),
	m_fLoopAniRate( rhs.m_fLoopAniRate ),
	m_LoopWeaponInfoList( rhs.m_LoopWeaponInfoList ),
	m_dwLoopDuration( rhs.m_dwLoopDuration ),
	m_SecondAttackAttribute( rhs.m_SecondAttackAttribute ),
	m_szLoopEffect( rhs.m_szLoopEffect ),
	m_OwnerBuffList( rhs.m_OwnerBuffList )
{
	ClearData();

	m_bSetHandMesh = false;
}

ioFigureskatingSkill::~ioFigureskatingSkill()
{
}

void ioFigureskatingSkill::ClearData()
{
	// 성장관련 요소
	m_dwCheckTime			= 0;
	m_fTicGaugeRateByGrowth	= FLOAT1;
	m_dwTicTimeByGrowth		= 0;

	m_RunningState = RST_NONE;

	m_dwRunningStartTime = 0;
	m_dwRunningReadyEndTime = 0;
	m_dwRunningEndTime = 0;
	m_dwRunningEndEndTime = 0;

	m_dwFireDuration = 0;

	m_bTargetRot = false;
	m_bLeftRot = false;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	m_iWeaponWay = 0;
	m_vActiveWeapon.clear();
}

void ioFigureskatingSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";
	
	m_bDisableGrowth= rkLoader.LoadBool_e( "disable_growth", false );
	m_fGuagePerTic	= rkLoader.LoadFloat_e( "gauge_per_tick", 0.0f );
	m_dwTicTime		= (DWORD)rkLoader.LoadInt_e( "tick_time", 0 );

	m_dwLoopTime = rkLoader.LoadInt_e( "run_duration", 0 );
	m_bUseFireEnd	 = rkLoader.LoadBool_e( "use_fire_end", false );
	m_bDirection = rkLoader.LoadBool_e( "direction", false );

	rkLoader.LoadString_e( "running_ready_ani", "", szBuf, MAX_PATH );
	m_RunningReadyAni = szBuf;
	m_fRunningReadyAniRate = rkLoader.LoadFloat_e( "running_ready_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "running_end_ani", "", szBuf, MAX_PATH );
	m_RunningEndAni = szBuf;
	m_fRunningEndAniRate = rkLoader.LoadFloat_e( "running_end_ani_rate", FLOAT1 );

	m_fRunSpeed = rkLoader.LoadFloat_e( "run_speed", 0.0f );
	m_dwRotateTime = (DWORD)rkLoader.LoadInt_e( "run_rotate_speed", 0 );

	m_bDisableRunColSkip = rkLoader.LoadBool_e( "disable_run_col_skip", false );

	m_iMaxWeaponWay = rkLoader.LoadInt_e( "max_bullet_cnt", 1 );
	m_iMaxWeaponWay = max( 1, m_iMaxWeaponWay );

	m_BulletInfoList.clear();
	for( int i=0; i < m_iMaxWeaponWay; ++i )
	{
		BulletInfo kInfo;

		wsprintf_e( szKey, "bullet_angle%d", i+1 );
		kInfo.m_fYawAngle = rkLoader.LoadFloat( szKey, 0.0f );

		wsprintf_e( szKey, "bullet_range_offset%d", i+1 );
		kInfo.m_fRangeOffSet = rkLoader.LoadFloat( szKey, 0.0f );

		wsprintf_e( szKey, "bullet_height_offset%d", i+1 );
		kInfo.m_fHeightOffSet = rkLoader.LoadFloat( szKey, 0.0f );

		m_BulletInfoList.push_back( kInfo );
	}

	//Loop Animation
	rkLoader.LoadString_e( "skill_loop_motion", "", szBuf, MAX_PATH );
	m_szLoopAnimation = szBuf;
	m_fLoopAniRate		= rkLoader.LoadFloat_e( "skill_loop_motion_rate", FLOAT1 );
	m_dwLoopDuration	= (DWORD)rkLoader.LoadInt_e( "skill_loop_duration", 0 );
	//m_dwLoopProtectTime	= (DWORD)rkLoader.LoadInt_e( "skill_loop_protect_time", 0 );

	rkLoader.LoadString_e( "skill_loop_effect", "", szBuf, MAX_PATH );
	m_szLoopEffect = szBuf;

	LoadLoopWeaponList( rkLoader );
	
	//Second Attack
	LoadAttackAttribute_e( "loop_second_attack", m_SecondAttackAttribute, rkLoader );

	LoadOwnerBuffList( rkLoader );
}

void ioFigureskatingSkill::LoadOwnerBuffList( ioINILoader &rkLoader )
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

void ioFigureskatingSkill::LoadLoopWeaponList( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];
	char szKey[MAX_PATH];

	int iAttackTypeCnt = rkLoader.LoadInt_e( "loop_attack_type_count", 0 );
	m_LoopWeaponInfoList.clear();
	m_LoopWeaponInfoList.reserve( iAttackTypeCnt );

	for( int i=0 ; i<iAttackTypeCnt ; i++ )
	{
		WeaponInfo kInfo;

		wsprintf_e( szKey, "loop_attack_type%d", i+1 );
		kInfo.m_iWeaponIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "loop_attack_type%d_resist", i+1 );
		kInfo.m_iResistanceIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "loop_attack_type%d_wounded", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_WoundedAnimation = szBuf;

		wsprintf_e( szKey, "loop_attack_type%d_wounded_time", i+1 );
		kInfo.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "loop_attack_type%d_wounded_loop_ani", i+1 );
		kInfo.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );

		m_LoopWeaponInfoList.push_back( kInfo );
	}
}

ioSkill* ioFigureskatingSkill::Clone()
{
	return new ioFigureskatingSkill( *this );
}

bool ioFigureskatingSkill::IsProtected( int iDefenseBreakType ) const
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
	if( m_RunningState == RST_READY )
	{
		// firemotion_e가 없었을 경우
		if( m_dwFireMotionEndTime == 0 )
			return true;

		// firemotion_e가 설정된 경우
		if( m_dwFireMotionEndTime > dwCurTime )
			return true;
	}

	if( (m_RunningState == RST_RUN )
		&& m_dwFireMotionEndTime > dwCurTime )
		return true;

	if( m_RunningState == RST_END )
		return false;

	return false;
}

bool ioFigureskatingSkill::IsEndState() const
{
	if ( m_RunningState == RST_END || m_RunningState == RST_RUN_END )
		return true;

	return false;
}

bool ioFigureskatingSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_dwPreDelayStartTime > 0 )
		return false;

	switch( m_RunningState )
	{
	case RST_LOOP:
		{
			if ( m_dwLoopStartTime == 0 )
				return true;
		}
		break;
	case RST_READY:
	case RST_RUN:
		{
			if( m_dwRunningEndEndTime == 0 )
				return false;

			if( m_dwRunningEndEndTime + dwActionStop < FRAMEGETTIME() )
				return true;
		}
		break;
	case RST_RUN_END:
		{
			if( m_dwRunningEndEndTime + dwActionStop < FRAMEGETTIME() )
				return true;
		}
		break;
	case RST_END:
		{
			if( m_dwAttackEndTime + dwActionStop < FRAMEGETTIME() )
				return true;
		}
		break;
	}

	return false;
}

bool ioFigureskatingSkill::OnSkillStart( ioBaseChar *pChar )
{
	ioAttackSkill::OnSkillStart( pChar );
	if( !HasSkillMotion() )
		return false;

	pChar->SetSkillProtection();

	m_dwAttackEndTime = 0;

	InitExtraAniJump();
	
	ClearData();

	m_dwCheckTime	= FRAMEGETTIME();

	m_bReduceGauge = false;
	m_bChangeDirRot = false;
	m_dwAniRotateTime = 0;
	m_fAniRotateAngle = 0.0f;

	m_iCurAniRotate = 0;

	m_dwEnableSwitchTime = 0;
	m_dwEnableSemiSwitchTime = 0;

	if( m_dwSwitchSkill > 0 )
		m_dwEnableSwitchTime = FRAMEGETTIME() + m_dwSwitchSkill;
	if( m_dwSemiSwitchSkill > 0 )
		m_dwEnableSemiSwitchTime = FRAMEGETTIME() + m_dwSemiSwitchSkill;

	m_dwSkillStartTime = FRAMEGETTIME();

	ioPlayStage *pStage = g_SkillMaker.GetPlayStage();

	// ExtraAttackForce
	ioGrowthAttackForceUpInfo *pForceInfo = NULL;
	pForceInfo = ToAttackForceUpInfo(GetGrowthUpInfoByType(GT_SKILL_ATTACK_FORCE, pChar->GetCharName()));
	m_fExtraAttackMoveForce = 0.0f;
	m_fExtraAttackMoveFriction = 0.0f;

	if( pForceInfo )
	{
		m_fExtraAttackMoveForce = pForceInfo->GetForceValue(pStage);
		m_fExtraAttackMoveFriction = pForceInfo->GetFrictionValue(pStage);
	}

	SetExtraAttackForce();
	CheckChangeDirRot( pChar );
	SetOwnerBuff( pChar );
	m_bSetHandMesh = false;

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}
	
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

	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();
	if( !StartPreDelayAnimation( pChar ) )	// Delay Animation이 없다
	{
		SetReadyState( pChar );
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

	SetSkillNameChatBubble( pChar );
	
	return true;
}

void ioFigureskatingSkill::SetReadyState( ioBaseChar *pChar )
{
	if( !pChar ) return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_RunningReadyAni );

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	float fTimeRate = FLOAT1;
	if( m_fRunningReadyAniRate > 0.0f )
		fTimeRate = m_fRunningReadyAniRate;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwRunningReadyEndTime = dwCurTime;
	m_dwRunningReadyEndTime += (pGrp->GetAnimationFullTime(iAniID) * fTimeRate);

	m_dwFireMotionEndTime = 0;
	float fMotionEndTime = pGrp->GetAnimationEventTime_e( iAniID, "firemotion_e" ) * fTimeRate;
	if( fMotionEndTime > 0.0f )
	{
		m_dwFireMotionEndTime = dwCurTime;
		m_dwFireMotionEndTime += fMotionEndTime;
	}

	if( !m_bReduceGauge )
	{
		pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
		m_bReduceGauge = true;
	}

	m_RunningState = RST_READY;

	// 사용 아이템 처리
	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}
}

void ioFigureskatingSkill::SetRunState( ioBaseChar *pChar )
{
	if( !m_bReduceGauge && m_SkillUseType != SUT_NORMAL )
	{
		pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
		m_bReduceGauge = true;
	}

	AttackSkillFire( pChar );

	m_dwRunningStartTime = FRAMEGETTIME();
}

void ioFigureskatingSkill::AttackSkillFire( ioBaseChar *pChar )
{
	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( GetAttribute().m_AttackAnimation );
	if( iAniID == -1 )	return;

	float fTimeRate  = GetAttackTimeRate();

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	pChar->AttachEffect( m_SkillEffectName );
	pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex,
		iAniID,
		GetAttribute().m_vWeaponInfoList,
		FTT_SKILL_ATTACK,
		fTimeRate,
		0 );


	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionStartTime = m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += m_dwLoopTime;
	m_dwRunningEndTime = m_dwMotionEndTime;

	if( !m_bDisableRunColSkip )
		pChar->SetCharColSkipSkillTime( dwCurTime, m_dwMotionEndTime );

	m_dwFireMotionEndTime = dwCurTime;
	m_dwFireMotionEndTime += pGrp->GetAnimationEventTime_e( iAniID, "firemotion_e" ) * fTimeRate;

	if( !m_bUseFireEnd )
		m_dwFireMotionEndTime = m_dwMotionEndTime;

	DWORD dwFireTime = pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;
	m_dwFireDuration = m_dwLoopTime - dwFireTime;


	pChar->SetReservedSliding( GetAttribute().m_vForceInfoList, iAniID, fTimeRate, 0 );

	m_RunningState = RST_RUN;
}

void ioFigureskatingSkill::SetRunEndState( ioBaseChar *pChar )
{
	if( !pChar ) return;
	
	g_WeaponMgr.DestroyReserveContactWeapon( pChar->GetCharName() );
	g_WeaponMgr.DestroyReserveZoneWeapon( pChar->GetCharName() );

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_RunningEndAni );

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	pChar->InitActionStopDelay( true );

	float fTimeRate = FLOAT1;
	if( m_fRunningEndAniRate > 0.0f )
		fTimeRate = m_fRunningEndAniRate;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwRunningEndEndTime = dwCurTime;
	m_dwRunningEndEndTime += (pGrp->GetAnimationFullTime(iAniID) * fTimeRate);

	m_dwEnableReserveTime = dwCurTime;
	m_dwEnableReserveTime += pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;

	pChar->SetSkillMoveEnable( false, false );
	pChar->SetMoveDirByRotate( pChar->GetTargetRot() );
	pChar->SetCharColSkipSkillTime( 0, 0 );

	m_RunningState = RST_RUN_END;
}

void ioFigureskatingSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

	float	fCurTicGauge	= m_fGuagePerTic / m_fTicGaugeRateByGrowth;
	int		iTickGap		= (int)( dwCurTime - m_dwCheckTime );
	int		iCurTicTime		= m_dwTicTime + m_dwTicTimeByGrowth;

	switch( m_RunningState )
	{
	case RST_NONE:
		{
			DWORD dwPreDelay = GetPreDelayTime();
			if( m_dwPreDelayStartTime + dwPreDelay < dwCurTime )
			{
				m_dwPreDelayStartTime = 0;
				EndPreDelayEffect( pChar );

				if( m_SkillUseType == SUT_NORMAL )
					SetReadyState( pChar );
				else
					SetRunState( pChar );
			}
		}
		break;
	case RST_READY:
		{
			// 예외처리 : 스킬이 시작한 직후와 끝나기 직전 일정 시간동안 입력을 받지 않는다.
			//            초근거리에서 스킬이 발동하는 걸 막기 위함.

			// ...

			if( m_dwRunningReadyEndTime < dwCurTime )
			{
				m_dwRunningReadyEndTime = 0;
				SetRunState( pChar );
				return;
			}
		}
		break;
	case RST_RUN:
		{
			if( m_dwRunningEndTime < dwCurTime )
			{
				m_dwRunningEndTime = 0;
				SetRunEndState( pChar );
				return;
			}

			CheckKeyInput( pChar );
			ProcessRotate( pChar );

			pChar->SetCurMoveSpeed( m_fRunSpeed );

			if ( pChar->IsNeedProcess() && pChar->IsAttackKey() )
			{
				SetLoopState( pChar );
			}
		}
		break;
	case RST_LOOP:
		{
			if ( m_dwLoopStartTime > 0 && m_dwLoopStartTime + m_dwLoopDuration < dwCurTime )
			{
				SetEndState( pChar );
			}

			if ( pChar->IsNeedProcess() && pChar->IsAttackKey() )
			{
				m_dwLoopStartTime = 0;
				SetAttackState( pChar );
			}
		}
	case RST_ATTACK:
		{
			if ( m_dwAttackEndTime > 0 && m_dwAttackEndTime < dwCurTime )
			{
				m_RunningState = RST_END;
			}			
		}
	case RST_END:
		{

		}
	}

	if( m_RunningState != RST_END )
	{
		if( iTickGap >= iCurTicTime )
		{
			m_dwCheckTime = dwCurTime - ( iTickGap - iCurTicTime );

			if( fCurTicGauge != 0.0f )
			{
				if( pChar->ReduceCurSkillGaugeBySkill( GetOwnerItem(), fCurTicGauge ) <= 0.0f )
				{
					SetEndState( pChar );
					return;
				}
			}
		}
	}
}

void ioFigureskatingSkill::OnSkillEnd( ioBaseChar *pChar )
{
	ioAttackSkill::OnSkillEnd( pChar );
	// 기본무기 장착
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( true );
	}

	if( !m_HandMesh.IsEmpty() )
	{
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
	}
	m_bSetHandMesh = false;

	RemoveExtraAttackForce();
	EndPreDelayEffect( pChar );

	pChar->EndEffect( m_SkillEffectName );
	RemoveOwnerBuff( pChar );
	m_bReduceGauge = false;
	pChar->SetSkillMoveEnable( false, false );
	pChar->SetMoveDirByRotate( pChar->GetTargetRot() );
	pChar->RestoreSkillProtection();
	pChar->SetCharColSkipSkillTime( 0, 0 );

	m_SkillUseType = SUT_NORMAL;

	if( !m_SkillEndEffectName.IsEmpty() )
		pChar->AttachEffect( m_SkillEndEffectName );

	if( !m_szLoopEffect.IsEmpty() )
		pChar->EndEffect( m_szLoopEffect );

	ClearData();
}

void ioFigureskatingSkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );

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
	else if( !m_bUseFireEnd )
	{
		pWeapon->SetLiveTime( m_dwFireDuration );
	}

	ioRotationWeapon *pRotation = ToRotationWeapon( pWeapon );
	if( pRotation )
	{
		if( m_iMaxWeaponWay <= m_iWeaponWay )
			return;

		float fCurAngle = 0.0f;
		float fCurRange = 0.0f;
		float fCurHeight = 0.0f;

		fCurAngle = m_BulletInfoList[m_iWeaponWay].m_fYawAngle;
		fCurRange = m_BulletInfoList[m_iWeaponWay].m_fRangeOffSet;
		fCurHeight = m_BulletInfoList[m_iWeaponWay].m_fHeightOffSet;

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

		m_iWeaponWay++;
		m_vActiveWeapon.push_back( pWeapon->GetWeaponIndex() );
	}

	ioZoneEffect2Weapon *pZone2 = ToZoneEffect2Weapon( pWeapon );
	if( pZone2 )
	{
		ioBaseChar *pChar = pZone2->GetOwner();
		if( pChar )
		{
			D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			pZone2->SetCollisionDir( vDir );
			pZone2->SetZoneValue( false, true );
		}
	}
}

void ioFigureskatingSkill::CheckKeyInput( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( !pOwner->IsNeedProcess() ) return;

	if( pOwner->IsSettedDirection() )
	{
		ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
		if( eNewDirKey != m_CurDirKey )
		{
			float fYawD = 180.0f + 45.0f * ( eNewDirKey - ioUserKeyInput::DKI_UP );
			fYawD = ioMath::ArrangeHead( fYawD );

			float fYawR = DEGtoRAD( fYawD );
			D3DXQUATERNION qtRotate;
			D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

			D3DXVECTOR3 vTargetDir = qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVECTOR3 vDir = pOwner->GetMoveDir();
			D3DXQUATERNION qtCurRot = pOwner->GetTargetRot();

			m_bTargetRot = false;
			m_bLeftRot = false;
			if( D3DXVec3Dot( &vTargetDir, &vDir ) > -FLOAT1 )
			{
				float fCurYaw, fTargetYaw;
				fCurYaw = RADtoDEG( ioMath::QuaterToYaw( qtCurRot ) );
				fTargetYaw = RADtoDEG( ioMath::QuaterToYaw( qtRotate ) );

				fCurYaw = ioMath::ArrangeHead( fCurYaw );
				fTargetYaw = ioMath::ArrangeHead( fTargetYaw );

				float fRotAngle = ioMath::AngleGap( fCurYaw, fTargetYaw );
				if( fRotAngle < 0.0f )
					m_bLeftRot = true;
			}

			m_CurDirKey = eNewDirKey;

			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
				kPacket << pOwner->GetCharName();
				kPacket << GetName();
				kPacket << SST_ROTATE;
				kPacket << (int)m_CurDirKey;
				kPacket << m_bLeftRot;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	}
	else
	{
		if( m_CurDirKey != ioUserKeyInput::DKI_NONE )
		{
			m_CurDirKey = ioUserKeyInput::DKI_NONE;

			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
				kPacket << pOwner->GetCharName();
				kPacket << GetName();
				kPacket << SST_ROTATE;
				kPacket << (int)m_CurDirKey;
				kPacket << m_bLeftRot;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	}
}

void ioFigureskatingSkill::ProcessRotate( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( m_dwRotateTime <= 0 ) return;
	if( m_bTargetRot ) return;

	if( m_CurDirKey == ioUserKeyInput::DKI_NONE )
		return;

	float fFrameGap = g_FrameTimer.GetCurFrameGap();
	float fRate = fFrameGap / m_dwRotateTime;
	float fRotateAngle = 360.0f * fRate;

	// 목표에 도달체크 및 처리
	float fYawD = 180.0f + 45.0f * ( m_CurDirKey - ioUserKeyInput::DKI_UP );
	fYawD = ioMath::ArrangeHead( fYawD );

	float fYawR = DEGtoRAD( fYawD );
	D3DXQUATERNION qtRotate;
	D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

	D3DXQUATERNION qtCurRot = pOwner->GetTargetRot();

	float fCurYaw, fTargetYaw;
	fCurYaw = RADtoDEG( ioMath::QuaterToYaw( qtCurRot ) );
	fTargetYaw = RADtoDEG( ioMath::QuaterToYaw( qtRotate ) );

	fCurYaw = ioMath::ArrangeHead( fCurYaw );
	fTargetYaw = ioMath::ArrangeHead( fTargetYaw );

	float fRotAngle = ioMath::AngleGap( fCurYaw, fTargetYaw );
	if( fabs( fRotAngle ) < fRotateAngle )
	{
		m_bTargetRot = true;
		pOwner->SetTargetRotToRotate( qtRotate, true, false );
		return;
	}

	if( m_bLeftRot )
		fRotateAngle *= -1;

	fRotateAngle = ioMath::ArrangeHead( fRotateAngle );
	D3DXVECTOR3 vDir = pOwner->GetMoveDir();

	D3DXQUATERNION qtAngle;
	D3DXQuaternionRotationYawPitchRoll( &qtAngle, DEGtoRAD(fRotateAngle), 0.0f, 0.0f );

	vDir = qtAngle * vDir;
	ioMath::CalcDirectionQuaternion( qtAngle, -vDir );

	pOwner->SetTargetRotToRotate( qtAngle, true, false );
}

void ioFigureskatingSkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	int iType;
	rkPacket >> iType;

	switch( iType )
	{
	case SST_ROTATE:
		{
			int iDir;
			rkPacket >> iDir;
			rkPacket >> m_bLeftRot;

			DWORD dwCurTime = FRAMEGETTIME();
			int iAniID = 0;

			if( iDir == ioUserKeyInput::DKI_NONE )
			{
				m_CurDirKey = ioUserKeyInput::DKI_NONE;
			}
			else
			{
				m_bTargetRot = false;
				m_CurDirKey = (ioUserKeyInput::DirKeyInput)iDir;
			}
		}
		break;
	case SST_LOOP:
		{
			SetLoopState( pChar );
		}
		break;
	case SST_ATTACK:
		{
			SetAttackState( pChar );
		}
		break;
	}
}

void ioFigureskatingSkill::SetLoopState( ioBaseChar *pChar )
{
	if ( !pChar ) return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_szLoopAnimation );
	if( iAniID == -1 )	return;

	float fTimeRate  = m_fLoopAniRate;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	pChar->EndEffect( m_SkillEffectName );
	pChar->AttachEffect( m_szLoopEffect );
	pChar->InitActionStopDelay( false );
	ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pChar, m_dwWeaponBaseIndex);
	if( pWeapon )
	{
		pWeapon->SetWeaponDead();
	}

	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();

	pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex,
										iAniID,
										m_LoopWeaponInfoList,
										FTT_SKILL_ATTACK,
										fTimeRate,
										0 );

	DWORD dwFireTime = pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;
	
	DWORD dwCurTime = FRAMEGETTIME();
	m_dwLoopStartTime = dwCurTime;

	pChar->SetSkillMoveEnable( false, false );
	
	m_RunningState = RST_LOOP;

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SST_LOOP;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioFigureskatingSkill::SetEndState( ioBaseChar *pChar )
{
	if( m_RunningState == RST_END )	return;

	m_RunningState = RST_END;
}

void ioFigureskatingSkill::SetAttackState( ioBaseChar *pChar )
{
	if( !pChar )	return;
	if( m_RunningState == RST_END )	return;

	m_RunningState = RST_ATTACK;

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_SecondAttackAttribute.m_AttackAnimation );
	if( iAniID == -1 )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	float fTimeRate  = m_SecondAttackAttribute.m_fAttackAniRate;
	DWORD dwPreDelay = m_SecondAttackAttribute.m_dwPreDelay;
	DWORD dwEndDelay = m_SecondAttackAttribute.m_dwEndDelay;

	m_dwMotionStartTime = m_dwAttackEndTime = dwCurTime;
	m_dwAttackEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwAttackEndTime += dwPreDelay + dwEndDelay;

	pGrp->ClearAllLoopAni( FLOAT100, false );
	pGrp->ClearAllActionAni( FLOAT100, false );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );
	
	auto Iter = m_vActiveWeapon.begin();
	for( ; Iter != m_vActiveWeapon.end(); ++Iter )
	{
		ioWeapon* pWeapon = g_WeaponMgr.FindWeapon( pChar, *Iter );
		ioRotationWeapon* pRotate = ToRotationWeapon( pWeapon );
		if( pRotate )
		{	
			pRotate->SetWeaponDead();
		}
	}

	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();

	pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex,
		iAniID,
		m_SecondAttackAttribute.m_vWeaponInfoList,
		FTT_SKILL_ATTACK,
		fTimeRate,
		dwPreDelay );

	pChar->SetReservedSliding( m_SecondAttackAttribute.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	if( !m_szLoopEffect.IsEmpty() )
		pChar->EndEffect( m_szLoopEffect );

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SST_ATTACK;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}

}

void ioFigureskatingSkill::SetOwnerBuff( ioBaseChar *pChar )
{
	if( !pChar )	return;
	if( m_OwnerBuffList.empty() ) return;

	int iCnt = m_OwnerBuffList.size();
	for( int i = 0; i < iCnt; ++i )
	{
		pChar->AddNewBuff( m_OwnerBuffList[i].m_BuffName, pChar->GetCharName(), "", this );
	}
}

void ioFigureskatingSkill::RemoveOwnerBuff( ioBaseChar *pChar )
{
	if( !pChar )	return;
	if( m_OwnerBuffList.empty() ) return;
	int iCnt = m_OwnerBuffList.size();
	for( int i=0; i<iCnt; ++i)
	{
		pChar->RemoveBuff( m_OwnerBuffList[i].m_BuffName );
	}
}