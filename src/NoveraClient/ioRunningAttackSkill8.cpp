

#include "stdafx.h"

#include "ioBaseChar.h"

#include "ioRunningAttackSkill8.h"

#include "ioGrowthUpInfo.h"

#include "WeaponDefine.h"

ioRunningAttackSkill8::ioRunningAttackSkill8()
{
	ClearData();

	m_bSetHandMesh = false;
}

ioRunningAttackSkill8::ioRunningAttackSkill8( const ioRunningAttackSkill8 &rhs )
: ioAttackSkill( rhs ),
 m_dwLoopTime( rhs.m_dwLoopTime ),
 m_bUseFireEnd( rhs.m_bUseFireEnd ),
 m_bDirection( rhs.m_bDirection ),
 m_ActionBuffList( rhs.m_ActionBuffList ),
 m_RunningReadyAni( rhs.m_RunningReadyAni ),
 m_RunningEndAni( rhs.m_RunningEndAni ),
 m_fRunningReadyAniRate( rhs.m_fRunningReadyAniRate ),
 m_fRunningEndAniRate( rhs.m_fRunningEndAniRate ),
 m_fRunSpeed( rhs.m_fRunSpeed ),
 m_dwRotateTime( rhs.m_dwRotateTime ),
 m_dwRunEndEnableTime( rhs.m_dwRunEndEnableTime ),
 m_bDisableRunColSkip( rhs.m_bDisableRunColSkip ),
 m_LastThrowAttack( rhs.m_LastThrowAttack ),
 m_bUseRunningJump( rhs.m_bUseRunningJump ),
 m_fRunningJumpAmt( rhs.m_fRunningJumpAmt ),
 m_RunExtraAttackType( rhs.m_RunExtraAttackType ),
 m_RunExtraAttack( rhs.m_RunExtraAttack ),
 m_dwExtraAttackDuration( rhs.m_dwExtraAttackDuration ),
 m_fRunExtraAttackOffsetFr( rhs.m_fRunExtraAttackOffsetFr ),
 m_fRunExtraAttackOffsetHe( rhs.m_fRunExtraAttackOffsetHe ),
 m_fRunExtraAttackOffsetSi( rhs.m_fRunExtraAttackOffsetSi ),
 m_fRunExtraAttackAngle( rhs.m_fRunExtraAttackAngle ),
 m_iMaxRunningColJumpCnt( rhs.m_iMaxRunningColJumpCnt ),
 m_RunningJumpLandAttack( rhs.m_RunningJumpLandAttack ),
 m_bUseAttackKey( rhs.m_bUseAttackKey ),
 m_fSpeedIncreaseTic( rhs.m_fSpeedIncreaseTic ),
 m_dwSpeedIncreaseTicTime( rhs.m_dwSpeedIncreaseTicTime ),
 m_fMinSpeed( rhs.m_fMinSpeed ),
 m_fMaxSpeed( rhs.m_fMaxSpeed ),
 m_TimeWeaponList( rhs.m_TimeWeaponList ),
 m_iTimeAttackHeightOffset( rhs.m_iTimeAttackHeightOffset ),
 m_fRunningEndSpeed( rhs.m_fRunningEndSpeed ),
 m_eRunningActionEndType( rhs.m_eRunningActionEndType ),
 m_dwSkillProtectTime( rhs.m_dwSkillProtectTime )
{
	ClearData();

	m_bSetHandMesh = false;
}

ioRunningAttackSkill8::~ioRunningAttackSkill8()
{
}

void ioRunningAttackSkill8::ClearData()
{
	m_RunningState = RST_NONE;

	m_dwRunningStartTime = 0;
	m_dwRunningReadyEndTime = 0;
	m_dwRunningEndTime = 0;
	m_dwRunningEndEndTime = 0;

	m_dwFireDuration = 0;

	m_bTargetRot = false;
	m_bLeftRot = false;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	//
	m_dwExtraAttackCheckTime = 0;
	m_iCurBlazeCnt = 1;

	m_iCurRunningJumpCnt = 0;

	m_fCurSpeed = 0;

	m_dwNextSpeedIncreaseTime = 0;
	m_iCurrTimeWeaponIndex	  = 0;

	m_bAlready_Max_Speed = false;
}

void ioRunningAttackSkill8::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	m_dwLoopTime = rkLoader.LoadInt_e( "run_duration", 0 );
	m_bUseFireEnd	 = rkLoader.LoadBool_e( "use_fire_end", false );
	m_bDirection = rkLoader.LoadBool_e( "direction", false );

	rkLoader.LoadString_e( "running_ready_ani", "", szBuf, MAX_PATH );
	m_RunningReadyAni = szBuf;
	m_fRunningReadyAniRate = rkLoader.LoadFloat_e( "running_ready_ani_rate", FLOAT1 );

	rkLoader.LoadString_e( "running_end_ani", "", szBuf, MAX_PATH );
	m_RunningEndAni = szBuf;
	m_fRunningEndAniRate = rkLoader.LoadFloat_e( "running_end_ani_rate", FLOAT1 );

	m_dwRunEndEnableTime = (DWORD)rkLoader.LoadInt_e( "run_end_enable_time", 0 );

	rkLoader.LoadString_e( "running_throw_ani", "", szBuf, MAX_PATH );
	m_LastThrowAttack.m_AttackAnimation = szBuf;
	m_LastThrowAttack.m_fAttackAniRate = rkLoader.LoadFloat_e( "running_throw_ani_rate", FLOAT1 );

	WeaponInfo weaponInfo;
	weaponInfo.m_iWeaponIdx = rkLoader.LoadInt_e( "running_attack_throw_type", 0 );
	if( weaponInfo.m_iWeaponIdx )
		m_LastThrowAttack.m_vWeaponInfoList.push_back( weaponInfo );

	int nForceCnt = rkLoader.LoadInt_e( "running_throw_force_cnt", 0 );
	for( int i = 0; i < nForceCnt; i++ )
	{
		ForceInfo forceInfo;
		wsprintf_e( szKey, "running_throw_force_move%d", i+1 );
		forceInfo.m_fForceMovePower = rkLoader.LoadInt( szKey, 0 );
		wsprintf_e( szKey, "running_throw_force_friction%d", i+1 );
		forceInfo.m_fForceMoveFriction = rkLoader.LoadInt( szKey, 0 );
		m_LastThrowAttack.m_vForceInfoList.push_back( forceInfo );
	}

		
	m_fRunSpeed				= rkLoader.LoadFloat_e( "run_speed", 0.0f );
	m_fRunningEndSpeed		= rkLoader.LoadFloat_e( "end_run_speed", m_fRunSpeed );
	m_dwRotateTime			= (DWORD)rkLoader.LoadInt_e( "run_rotate_speed", 0 );
	m_bDisableRunColSkip	= rkLoader.LoadBool_e( "disable_run_col_skip", false );

	// buff
	m_ActionBuffList.clear();
	int iCnt = rkLoader.LoadInt_e( "action_buff_list", 0 );
	for( int i=0; i < iCnt; ++i )
	{
		wsprintf_e( szKey, "action_buff%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

		m_ActionBuffList.push_back( szBuf );
	}

	//
	m_bUseRunningJump = rkLoader.LoadBool_e( "use_running_jump", false );
	m_fRunningJumpAmt = rkLoader.LoadFloat_e( "running_jump_amt", 0.0f );

	m_iMaxRunningColJumpCnt = rkLoader.LoadInt_e( "running_col_jump_max_cnt", 0 );

	m_RunExtraAttackType = (RunningExtraAttackType)rkLoader.LoadInt_e( "run_extra_type", RET_NONE );

	m_dwExtraAttackDuration = (DWORD)rkLoader.LoadInt_e( "run_extra_attack_duration", 0 );

	wsprintf_e( szKey, "run_extra_attack_type" );
	m_RunExtraAttack.m_iWeaponIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

	wsprintf_e( szKey, "run_extra_attack_type_resist" );
	m_RunExtraAttack.m_iResistanceIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

	wsprintf_e( szKey, "run_extra_attack_type_wounded" );
	rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
	m_RunExtraAttack.m_WoundedAnimation = szBuf;

	wsprintf_e( szKey, "run_extra_attack_type_wounded_time" );
	m_RunExtraAttack.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );

	wsprintf_e( szKey, "run_extra_attack_type_wounded_loop_ani" );
	m_RunExtraAttack.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );

	m_fRunExtraAttackOffsetFr = rkLoader.LoadFloat_e( "run_extra_attack_offset_fr", 0.0f );
	m_fRunExtraAttackOffsetHe = rkLoader.LoadFloat_e( "run_extra_attack_offset_he", 0.0f );
	m_fRunExtraAttackOffsetSi = rkLoader.LoadFloat_e( "run_extra_attack_offset_si", 0.0f );

	m_fRunExtraAttackAngle = rkLoader.LoadFloat_e( "run_extra_attack_angle", 0.0f );

	//
	wsprintf_e( szKey, "running_jump_land_attack_type" );
	m_RunningJumpLandAttack.m_iWeaponIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

	wsprintf_e( szKey, "running_jump_land_attack_type_resist" );
	m_RunningJumpLandAttack.m_iResistanceIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

	wsprintf_e( szKey, "running_jump_land_attack_type_wounded" );
	rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
	m_RunningJumpLandAttack.m_WoundedAnimation = szBuf;

	wsprintf_e( szKey, "running_jump_land_attack_type_wounded_time" );
	m_RunningJumpLandAttack.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );

	wsprintf_e( szKey, "running_jump_land_attack_type_wounded_loop_ani" );
	m_RunningJumpLandAttack.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );

	//추가 yongjun
	m_bUseAttackKey				= rkLoader.LoadBool_e( "use_attack_key", false );
	m_fSpeedIncreaseTic			= rkLoader.LoadFloat_e( "speed_increase_tic", 0.0f );
	m_dwSpeedIncreaseTicTime	= (DWORD)rkLoader.LoadInt_e( "speed_increase_tic_time", 0);
	m_fMinSpeed					= rkLoader.LoadFloat_e( "min_speed", 0.0f );
	m_fMaxSpeed					= rkLoader.LoadFloat_e( "max_speed", 0.0f );
	m_eRunningActionEndType		= (RunningActionEndType)rkLoader.LoadInt_e( "run_end_key_type", (int)RAET_ALL );
	m_dwSkillProtectTime		= rkLoader.LoadInt_e( "skill_protect_time", 0 );

	//
	int iCount = rkLoader.LoadInt_e( "time_attack_count", 0 );
	for( int i = 0; i < iCount; ++i )
	{
		TimeWeapon Time;
		sprintf_s( szKey, "time_attack%d_time", i+1 );
		Time.dwTime = rkLoader.LoadInt( szKey, 0 );
		
		sprintf_s( szKey, "time_attack%d", i+1 );
		LoadWeaponInfo( szKey, Time.Info, rkLoader );
		
		m_TimeWeaponList.push_back( Time );
	}

	m_iTimeAttackHeightOffset = rkLoader.LoadInt_e( "time_attack_height_offset", 0 );
}

ioSkill* ioRunningAttackSkill8::Clone()
{
	return new ioRunningAttackSkill8( *this );
}

bool ioRunningAttackSkill8::IsProtected( int iDefenseBreakType ) const
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

	if( 0 < m_dwSkillProtectTime )
	{
		if(	FRAMEGETTIME() < m_dwSkillStartTime + m_dwSkillProtectTime )
			return true;
		else
			return false;
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

	if( (m_RunningState == RST_RUN || m_RunningState == RST_RUN_JUMP)
		&& m_dwFireMotionEndTime > dwCurTime )
		return true;

	if( m_RunningState == RST_END )
		return false;
	
	return false;
}

bool ioRunningAttackSkill8::IsEndState() const
{
	if( m_RunningState == RST_END )
		return true;

	return false;
}

bool ioRunningAttackSkill8::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_dwPreDelayStartTime > 0 )
		return false;

	if( m_dwRunningEndEndTime == 0 )
		return false;

	if( m_dwRunningEndEndTime + dwActionStop < FRAMEGETTIME() )
		return true;

	return false;
}

bool ioRunningAttackSkill8::OnSkillStart( ioBaseChar *pChar )
{
	if( !HasSkillMotion() )
		return false;

	pChar->SetSkillProtection();

	m_dwAttackEndTime = 0;

	InitExtraAniJump();

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

	ClearData();

	ioPlayStage *pStage = g_SkillMaker.GetPlayStage();

	// ExtraAttackForce
	ioGrowthAttackForceUpInfo *pForceInfo = NULL;
	pForceInfo = ToAttackForceUpInfo( GetGrowthUpInfoByType( GT_SKILL_ATTACK_FORCE, pChar->GetCharName() ) );
	m_fExtraAttackMoveForce		= 0.0f;
	m_fExtraAttackMoveFriction	= 0.0f;

	if( pForceInfo )
	{
		m_fExtraAttackMoveForce		= pForceInfo->GetForceValue(pStage);
		m_fExtraAttackMoveFriction	= pForceInfo->GetFrictionValue(pStage);
	}

	SetExtraAttackForce();
	CheckChangeDirRot( pChar );

	m_bSetHandMesh = false;

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
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

	m_dwNextSpeedIncreaseTime = 0;
	m_fCurSpeed = m_fMinSpeed;

	return true;
}

void ioRunningAttackSkill8::SetReadyState( ioBaseChar *pChar )
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

void ioRunningAttackSkill8::SetRunState( ioBaseChar *pChar )
{
	if( !m_bReduceGauge && m_SkillUseType != SUT_NORMAL )
	{
		pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
		m_bReduceGauge = true;
	}

	if( pChar->IsNeedProcess() && !m_ActionBuffList.empty() )
	{
		pChar->SetRandomSeed( 0 );

		bool bResult = false;
		ioHashStringVec vBuffList;

		ioHashString szItemName;
		ioItem *pItem = GetOwnerItem();
		if( pItem )
		{
			szItemName = pItem->GetName();
		}

		float fValue = 0.0f;
		ioGrowthNormalUpInfo *pUpInfo = ToNormalUpInfo( GetGrowthUpInfoByType(GT_BUFF_DURATION, pChar->GetCharName()) );
		if( pUpInfo )
		{
			ioPlayStage *pStage = g_SkillMaker.GetPlayStage();
			fValue = pUpInfo->GetValue(pStage);
		}

		int iCnt = m_ActionBuffList.size();
		for( int i=0; i < iCnt; ++i )
		{
			ioHashString szBuffName = m_ActionBuffList[i];

			if( pChar->IsHasCrown() && m_bDisableKingSkill )
				continue;
			else if( pChar->HasBuff( BT_ABSOLUTE_PROTECTION ) )
				continue;

			if( !szBuffName.IsEmpty() )
			{
				ioBuff *pBuff = pChar->AddNewBuffWithAnimateTime( szBuffName, pChar->GetCharName(), szItemName, this, 0 );
				if( pBuff )
				{
					pBuff->SetExtraDurationByGrowth( fValue );

					bResult = true;
					vBuffList.push_back( szBuffName );
				}
			}
		}

		if( bResult )
		{
			int iCurBuffCnt = vBuffList.size();
			if( pChar->IsNeedSendNetwork() )
			{
				if( Help::CheckEnableTimeGapUser() )
				{
					LOG.PrintTimeAndLog( 0, "PC -SkillAddBuff3 : %s, %d", GetName().c_str(), FRAMEGETTIME() );
				}
				SP2Packet kPacket( CUPK_ADD_NEW_BUFF );
				kPacket << pChar->GetCharName();
				kPacket << pChar->GetCharName();
				kPacket << szItemName;
				kPacket << pChar->GetRandomSeed();
				kPacket << false;					// 타격횟수 체크
				kPacket << 0.0f;
				kPacket << fValue;
				kPacket << 0.0f;
				kPacket << COWT_NONE;				// Check CloseOrderBuff(12.07.13 수정)
				kPacket << iCurBuffCnt;

				for( i=0; i < iCurBuffCnt; i++ )
				{
					kPacket << vBuffList[i];
				}

				kPacket << false;					// Use Force

				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	}

	AttackSkillFire( pChar );

	m_dwRunningStartTime = FRAMEGETTIME();

	m_dwNextSpeedIncreaseTime = FRAMEGETTIME();
}

void ioRunningAttackSkill8::AttackSkillFire( ioBaseChar *pChar )
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

	if( m_SkillUseType == SUT_JUMP )
		m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	else
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

	if( m_SkillUseType == SUT_JUMP )
		m_RunningState = RST_RUN_JUMP;
	else
		m_RunningState = RST_RUN;

	// run extra attack
	switch( m_RunExtraAttackType )
	{
	case RET_BLAZE:
	case RET_FIRE_BALL:
		{
			if( m_RunExtraAttack.m_iWeaponIdx > 0 )
				m_dwExtraAttackCheckTime = dwCurTime;
			else
				m_dwExtraAttackCheckTime = 0;
		}
		break;
	}
}


void ioRunningAttackSkill8::ProcessSkillExplicitFire( ioBaseChar *pChar )
{
	if( !pChar->IsNeedProcess() )
		return;
	
	if( COMPARE( m_iCurrTimeWeaponIndex, 0, (int)m_TimeWeaponList.size() ) )
	{		
		if( m_TimeWeaponList[m_iCurrTimeWeaponIndex].dwTime < FRAMEGETTIME() - m_dwRunningStartTime )
		{
			D3DXVECTOR3 vPos = pChar->GetWorldPosition();
			vPos.y += m_iTimeAttackHeightOffset;

			AttackSkillExplicitFire( pChar, m_iCurrTimeWeaponIndex, vPos );

			m_iCurrTimeWeaponIndex++;
		}
	}
}

void ioRunningAttackSkill8::AttackSkillExplicitFire( ioBaseChar *pChar, int iWeaponIndex, D3DXVECTOR3 vPos ) 
{
	if( !COMPARE( iWeaponIndex, 0, (int)m_TimeWeaponList.size() ) )
		return;

	const AttackAttribute& rkAttack = m_TimeWeaponList[iWeaponIndex].Info;
	for( WeaponInfoList::const_iterator iter = rkAttack.m_vWeaponInfoList.begin(); iter != rkAttack.m_vWeaponInfoList.end(); ++iter )
	{		
		pChar->SkillFireExplicit( *iter, GetName(), pChar->IncreaseWeaponIndexBase(), vPos );
	}

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << SST_SKILL_EXPLICIT_FIRE;
		kPacket << iWeaponIndex;
		kPacket << vPos;
		kPacket << pChar->GetTargetRot();
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioRunningAttackSkill8::SetLastThrowState( ioBaseChar *pChar )
{
	if( !pChar ) return;

	// clear buff
	int iBuffCnt = m_ActionBuffList.size();
	for( int i=0; i < iBuffCnt; ++i )
	{
		pChar->RemoveBuff( m_ActionBuffList[i] );
	}

	g_WeaponMgr.DestroyReserveContactWeapon( pChar->GetCharName() );
	g_WeaponMgr.DestroyReserveZoneWeapon( pChar->GetCharName() );

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_LastThrowAttack.m_AttackAnimation );

	pGrp->ClearAllActionAni( FLOAT10, true );
	pGrp->ClearAllLoopAni( FLOAT10, true );

	//pChar->InitActionStopDelay( true );

	float fTimeRate = FLOAT1;
	if( m_LastThrowAttack.m_fAttackAniRate > 0.0f )
		fTimeRate = m_LastThrowAttack.m_fAttackAniRate;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );
	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();

	pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex, iAniID, m_LastThrowAttack.m_vWeaponInfoList,
		FTT_SKILL_ATTACK, fTimeRate, 0 );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwRunningEndEndTime = dwCurTime;
	m_dwRunningEndEndTime += (pGrp->GetAnimationFullTime(iAniID) * fTimeRate);

	pChar->SetReservedSliding( m_LastThrowAttack.m_vForceInfoList, iAniID, fTimeRate, 0 );

	//////////////////////////////////////////////////////////////////////////
	DWORD dwTrackingTime = 0;
	if( pGrp )
	{
		int iAniID = pGrp->GetAnimationIdx( m_LastThrowAttack.m_AttackAnimation );
		if( iAniID != -1 )
			dwTrackingTime = pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" );
	}

	dwTrackingTime += FRAMEGETTIME();
	
	if( IsUseAutoTargetValue() )
	{
		pChar->SetAutoTarget( ATT_SKILL );
		pChar->SetAutoTargetTrackingEndTime( dwTrackingTime );
	}
	//////////////////////////////////////////////////////////////////////////

	m_RunningState = RST_THROW;
}


void ioRunningAttackSkill8::SetRunEndStateByJump( ioBaseChar *pChar )
{
	if( !pChar ) return;

	// clear buff
	int iBuffCnt = m_ActionBuffList.size();
	for( int i=0; i < iBuffCnt; ++i )
	{
		pChar->RemoveBuff( m_ActionBuffList[i] );
	}

	g_WeaponMgr.DestroyReserveContactWeapon( pChar->GetCharName() );
	g_WeaponMgr.DestroyReserveZoneWeapon( pChar->GetCharName() );

	pChar->SetSkillMoveEnable( false, false );
	pChar->SetMoveDirByRotate( pChar->GetTargetRot() );
	pChar->SetCharColSkipSkillTime( 0, 0 );

	//pOwner->SetSKillEndJumpState( m_fThunderBirdEndJumpPower, true, false, true, true );

	m_RunningState = RST_END;


}


void ioRunningAttackSkill8::SetRunEndState( ioBaseChar *pChar )
{
	if( !pChar ) return;

	// clear buff
	int iBuffCnt = m_ActionBuffList.size();
	for( int i=0; i < iBuffCnt; ++i )
	{
		pChar->RemoveBuff( m_ActionBuffList[i] );
	}

	g_WeaponMgr.DestroyReserveContactWeapon( pChar->GetCharName() );
	g_WeaponMgr.DestroyReserveZoneWeapon( pChar->GetCharName() );

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_RunningEndAni );

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	
	pChar->SetCurMoveSpeed( m_fRunningEndSpeed );
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

	m_RunningState = RST_END;
}


void ioRunningAttackSkill8::SetLastThrowEndState( ioBaseChar *pChar )
{
	if( !pChar ) return;

	// clear buff
	/*int iBuffCnt = m_ActionBuffList.size();
	for( int i=0; i < iBuffCnt; ++i )
	{
		pChar->RemoveBuff( m_ActionBuffList[i] );
	}

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
	m_dwEnableReserveTime += pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;*/

	pChar->SetSkillMoveEnable( false, false );
	pChar->SetMoveDirByRotate( pChar->GetTargetRot() );
	pChar->SetCharColSkipSkillTime( 0, 0 );

	m_RunningState = RST_END;
}


void ioRunningAttackSkill8::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

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
	case RST_RUN_JUMP:
		if( m_dwRunningEndTime < dwCurTime )
		{
			m_dwRunningEndTime = 0;
			SetRunEndStateByJump( pChar );
			return;
		}
		break;
	case RST_RUN:
		{
			if( m_dwRunningEndTime < dwCurTime )
			{
				m_dwRunningEndTime = 0;
				//SetRunEndState( pChar );
				SetLastThrowState( pChar );
				if( pChar->IsNeedSendNetwork() )
				{
					SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
					kPacket << pChar->GetCharName();
					kPacket << GetName();
					kPacket << SST_RUN;
					kPacket << (int)m_RunningState;
					kPacket << false;
					kPacket << (int)(ioUserKeyInput::DKI_NONE);
					P2PNetwork::SendToAllPlayingUser( kPacket );
				}
				return;
			}
			else
			{
				// RunningStop
				RunningActionType eActionType = (RunningActionType)CheckRunningActionType( pChar );
				if( eActionType == RAT_STOP )
				{
					if( !m_LastThrowAttack.m_AttackAnimation.IsEmpty() && pChar->IsAttackKey() )
					{
						ioUserKeyInput::DirKeyInput eDir = pChar->CheckCurDirKey();
						bool bChangeDir = (eDir != ioUserKeyInput::DKI_NONE) ? true : false;

						if( bChangeDir )
						{
							float fYawD = 180.0f + 45.0f * ( eDir - ioUserKeyInput::DKI_UP );
							fYawD = ioMath::ArrangeHead( fYawD );

							D3DXQUATERNION qtRotate;
							D3DXQuaternionRotationYawPitchRoll( &qtRotate, DEGtoRAD( fYawD ), 0.0f, 0.0f );

							pChar->SetTargetRotAndMoveDirChange( qtRotate );
						}

						SetLastThrowState( pChar );
						if( pChar->IsNeedSendNetwork() )
						{
							SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
							kPacket << pChar->GetCharName();
							kPacket << GetName();
							kPacket << SST_RUN;
							kPacket << (int)m_RunningState;
							kPacket << bChangeDir;
							kPacket << (int)eDir;
							P2PNetwork::SendToAllPlayingUser( kPacket );
						}
						return;
					}

					m_dwRunningEndTime = 0;
					SetRunEndState( pChar );

					if( pChar->IsNeedSendNetwork() )
					{
						SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
						kPacket << pChar->GetCharName();
						kPacket << GetName();
						kPacket << SST_RUN;
						kPacket << (int)m_RunningState;
						P2PNetwork::SendToAllPlayingUser( kPacket );
					}
					return;
				}
				else if( eActionType == RAT_JUMP && !pChar->IsFloating() )
				{
					pChar->SetCurJumpPowerAmt( 0.0f );
					pChar->SetForcePowerAmt( 0.0f );
					pChar->SetGravityAmt( 0.0f );

					m_RunningState = RST_RUN_JUMP;
					pChar->SetJumpPower( m_fRunningJumpAmt );
	
					if( pChar->IsNeedSendNetwork() )
					{
						SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
						kPacket << pChar->GetCharName();
						kPacket << GetName();
						kPacket << SST_JUMP;
						kPacket << (int)m_RunningState;
						P2PNetwork::SendToAllPlayingUser( kPacket );
					}
				}
				else if( m_RunningState == RST_RUN_JUMP && !pChar->IsFloating() )
				{
					if( m_RunExtraAttackType == RET_FIRE_BALL && m_RunningJumpLandAttack.m_iWeaponIdx > 0 )
					{
						D3DXVECTOR3 vPos = pChar->GetWorldPosition();
						pChar->SkillFireExplicit( m_RunningJumpLandAttack,
												  GetName(),
												  m_dwWeaponBaseIndex+m_iCurBlazeCnt,
												  vPos );

						m_iCurBlazeCnt++;
					}
					
					m_RunningState = RST_RUN;
				}
				else if( eActionType == RAT_ATTACKKEY )
					CheckAttackKey( pChar );
				
				CheckKeyInput( pChar );
				ProcessRotate( pChar );
				ProcessExtraAttack( pChar );
				ProcessSkillExplicitFire( pChar );

				float fCurSpeed = 0.0f;
				if ( m_fSpeedIncreaseTic != 0.0f )
				{
					fCurSpeed = m_fCurSpeed;
				}
				else
					fCurSpeed = m_fRunSpeed;
				
				pChar->SetCurMoveSpeed(  fCurSpeed );
			}
		}
		break;
	case RST_THROW:
		{
			if( m_dwRunningEndEndTime < dwCurTime )
			{
				SetLastThrowEndState( pChar );
				return;
			}
		}
		break;
	}
}

void ioRunningAttackSkill8::OnSkillEnd( ioBaseChar *pChar )
{
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

	// clear buff
	int iBuffCnt = m_ActionBuffList.size();
	for( int i=0; i < iBuffCnt; ++i )
	{
		pChar->RemoveBuff( m_ActionBuffList[i] );
	}

	RemoveExtraAttackForce();
	EndPreDelayEffect( pChar );

	pChar->EndEffect( m_SkillEffectName );

	m_bReduceGauge = false;
	pChar->SetSkillMoveEnable( false, false );
	pChar->SetMoveDirByRotate( pChar->GetTargetRot() );
	pChar->RestoreSkillProtection();
	pChar->SetCharColSkipSkillTime( 0, 0 );

	m_SkillUseType = SUT_NORMAL;

	if( !m_SkillEndEffectName.IsEmpty() )
		pChar->AttachEffect( m_SkillEndEffectName );
}

void ioRunningAttackSkill8::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
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

			if( m_RunningState == RST_RUN_JUMP )
				pZone->SetZoneValue( false, true );
			else
				pZone->SetZoneValue( true, true );
		}
	}
	else if( !m_bUseFireEnd )
	{
		pWeapon->SetLiveTime( m_dwFireDuration );
	}
}

void ioRunningAttackSkill8::CheckKeyInput( ioBaseChar *pOwner )
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

void ioRunningAttackSkill8::ProcessRotate( ioBaseChar *pOwner )
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

void ioRunningAttackSkill8::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	int iType;
	rkPacket >> iType;

	if( iType == SST_ROTATE )
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
		}
	}
	else if( iType == SST_RUN )
	{
		int iStateType;
		bool bChangeDir;
		int eDir;

		rkPacket >> iStateType;

		switch( iStateType )
		{
		case RST_THROW:
			{
				rkPacket >> bChangeDir;
				rkPacket >> eDir;

				if( bChangeDir )
				{
					float fYawD = 180.0f + 45.0f * ( eDir - ioUserKeyInput::DKI_UP );
					fYawD = ioMath::ArrangeHead( fYawD );

					D3DXQUATERNION qtRotate;
					D3DXQuaternionRotationYawPitchRoll( &qtRotate, DEGtoRAD( fYawD ), 0.0f, 0.0f );

					pChar->SetTargetRotAndMoveDirChange( qtRotate );
				}

				SetLastThrowState( pChar );
				break;
			}
		case RST_END:
			{
				m_dwRunningEndTime = 0;
				SetRunEndState( pChar );
				break;
			}
		}
	}
	else if( iType == SST_JUMP )
	{
		pChar->SetCurJumpPowerAmt( 0.0f );
		pChar->SetForcePowerAmt( 0.0f );
		pChar->SetGravityAmt( 0.0f );

		m_RunningState = RST_RUN_JUMP;
		pChar->SetJumpPower( m_fRunningJumpAmt );
	}
	else if ( iType == SST_INCREASE_RUN_SPEED )
	{
		rkPacket >> m_fCurSpeed;
	}
	else if( iType == SST_SKILL_EXPLICIT_FIRE )
	{	
		int iWeaponIndex	= 0;
		D3DXVECTOR3 vPos	= ioMath::VEC3_ZERO;
		D3DXQUATERNION qRot	= ioMath::QUAT_IDENTITY;

		rkPacket >> iWeaponIndex;
		rkPacket >> vPos;
		rkPacket >> qRot;

		pChar->SetTargetRot( qRot );
		AttackSkillExplicitFire( pChar, iWeaponIndex, vPos );
	}
}

int ioRunningAttackSkill8::CheckRunningActionType( ioBaseChar *pOwner )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return RAT_NONE;

	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwRunningStartTime+m_dwRunEndEnableTime > dwCurTime )
		return RAT_NONE;

	if( m_bUseRunningJump && pOwner->IsJumpKey() )
		return RAT_JUMP;

	if ( m_bUseAttackKey && pOwner->IsAttackKey() )
		return RAT_ATTACKKEY;

	switch( m_eRunningActionEndType )
	{
	case RAET_ALL:
		{
			if( pOwner->IsAttackKey() || pOwner->IsDefenseKey() || pOwner->IsJumpKey() )
				return RAT_STOP;
		}
		break;
	case REAT_ATTACK:
		{
			if( pOwner->IsAttackKey() )
				return RAT_STOP;
		}
		break;
	case REAT_DEFENCE:
		{
			if( pOwner->IsDefenseKey() )
				return RAT_STOP;
		}
		break;
	case REAT_JUMP:
		{
			if( pOwner->IsJumpKey() )
				return RAT_STOP;
		}
		break;
	case REAT_NOT_ATTACK:
		{
			if( pOwner->IsDefenseKey() || pOwner->IsJumpKey() )
				return RAT_STOP;
		}
		break;
	case REAT_NOT_DEFENCE:
		{
			if( pOwner->IsAttackKey() ||  pOwner->IsJumpKey() )
				return RAT_STOP;
		}
		break;
	case REAT_NOT_JUMP:
		{
			if( pOwner->IsAttackKey() || pOwner->IsDefenseKey() )
				return RAT_STOP;
		}
		break;
	}

	return RAT_NONE;
}

void ioRunningAttackSkill8::ProcessExtraAttack( ioBaseChar *pOwner )
{
	if( !pOwner ) return;
	if( m_dwExtraAttackCheckTime == 0 ) return;

	if( m_RunExtraAttack.m_iWeaponIdx <= 0 )
		return;

	if( m_RunningState != RST_RUN && m_RunningState != RST_RUN_JUMP ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_RunExtraAttackType )
	{
	case RET_BLAZE:
		if( m_dwExtraAttackCheckTime < dwCurTime )
		{
			D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate();
			pOwner->SkillFireExplicit( m_RunExtraAttack,
									   GetName(),
									   m_dwWeaponBaseIndex+m_iCurBlazeCnt,
									   vPos );

			m_iCurBlazeCnt++;
			m_dwExtraAttackCheckTime = dwCurTime + m_dwExtraAttackDuration;
		}
		break;
	case RET_FIRE_BALL:
		if( m_dwExtraAttackCheckTime < dwCurTime )
		{
			D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate();
			D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVec3Normalize( &vDir, &vDir );

			D3DXVECTOR3 vSide;
			D3DXVec3Cross( &vSide, &vDir, &ioMath::UNIT_Y );

			vPos += vDir * m_fRunExtraAttackOffsetFr;
			vPos.y += m_fRunExtraAttackOffsetHe;
			vPos += vSide * m_fRunExtraAttackOffsetSi;

			float fYawD = ioMath::ArrangeHead( m_fRunExtraAttackAngle );

			if( m_fRunExtraAttackAngle != 0.0f )
			{
				float fYawR = DEGtoRAD( fYawD );

				D3DXQUATERNION qtRotate;
				D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );
				vDir = qtRotate * vDir;
			}

			pOwner->SkillFireExplicit( m_RunExtraAttack,
									   GetName(),
									   m_dwWeaponBaseIndex+m_iCurBlazeCnt,
									   vPos, vDir );

			m_iCurBlazeCnt++;
			m_dwExtraAttackCheckTime = dwCurTime + m_dwExtraAttackDuration;
		}
		break;
	}
}

void ioRunningAttackSkill8::SetInfoByApplyWeapon( ioBaseChar *pOwner, ioBaseChar *pTarget, DWORD dwWeaponBaseIndex, bool bBlock )
{
	if( !pOwner )
		return;

	if( !pOwner->IsNeedProcess() )
		return;

	if( m_dwWeaponBaseIndex != dwWeaponBaseIndex )
		return;

	if( (m_RunExtraAttackType == RET_FIRE_BALL) && pOwner->IsFloating() )
	{
		if( m_iMaxRunningColJumpCnt > 0 && m_iCurRunningJumpCnt < m_iMaxRunningColJumpCnt )
		{
			pOwner->SetCurJumpPowerAmt( 0.0f );
			pOwner->SetForcePowerAmt( 0.0f );
			pOwner->SetGravityAmt( 0.0f );

			m_RunningState = RST_RUN_JUMP;
			pOwner->SetJumpPower( m_fRunningJumpAmt );

			m_iCurRunningJumpCnt++;

			if( pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
				kPacket << pOwner->GetCharName();
				kPacket << GetName();
				kPacket << SST_JUMP;
				kPacket << (int)m_RunningState;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//추가 yong jun
void ioRunningAttackSkill8::CheckAttackKey( ioBaseChar *pChar )
{
	if( !pChar && !pChar->IsNeedProcess() )
		return;

	if ( m_fSpeedIncreaseTic == 0.0f )
		return;
	if ( m_bAlready_Max_Speed )
		return;
	if ( m_dwNextSpeedIncreaseTime <= FRAMEGETTIME() )
	{
		m_fCurSpeed += m_fSpeedIncreaseTic;
		if ( m_fCurSpeed >= m_fMaxSpeed )
		{
			m_bAlready_Max_Speed = true;
			m_fCurSpeed = m_fMaxSpeed;
		}
		m_dwNextSpeedIncreaseTime += m_dwSpeedIncreaseTicTime;

		if( pChar->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
			kPacket << pChar->GetCharName();
			kPacket << GetName();
			kPacket << SST_INCREASE_RUN_SPEED;
			kPacket << m_fCurSpeed;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}


bool ioRunningAttackSkill8::IsNoDropState() const
{
	if( m_SkillUseType == SUT_JUMP )
		return true;

	return __super::IsNoDropState();
}


bool ioRunningAttackSkill8::IsAttackEndState() const
{
	if( m_SkillUseType == SUT_JUMP )
	{
		if( m_RunningState == RST_END )
			return true;

		return false;
	}
	
	return __super::IsAttackEndState();
}