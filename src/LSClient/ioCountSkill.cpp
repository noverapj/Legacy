

#include "stdafx.h"

#include "ioBaseChar.h"
#include "ioGrowthUpInfo.h"

#include "ioCountSkill.h"

#include "ItemDefine.h"

ioCountSkill::ioCountSkill()
{
	m_dwPreDelayStartTime = 0;
	m_dwFireMotionEndTime = 0;
	m_dwAttackEndTime = 0;

	m_fExtraAttackMoveForce = 0.0f;
	m_fExtraAttackMoveFriction = 0.0f;

	m_bRefreshFireTimeList = true;

	m_iCurUseCount = 0;
	m_fCurRecoveryCount = 0;
}

ioCountSkill::ioCountSkill( const ioCountSkill &rhs )
: ioSkill( rhs ),
  m_AttackAttribute( rhs.m_AttackAttribute ),
  m_DownAttackAttribute( rhs.m_DownAttackAttribute ),
  m_JumpAttackAttribute( rhs.m_JumpAttackAttribute ),
  m_bStateEscapeOnStand( rhs.m_bStateEscapeOnStand ),
  m_bStateEscapeOnAir( rhs.m_bStateEscapeOnAir ),
  m_bStateEscapeOnDown( rhs.m_bStateEscapeOnDown ),
  m_iMaxUseCount( rhs.m_iMaxUseCount ),
  m_iNeedCount( rhs.m_iNeedCount ),
  m_fDefaultRecoveryCount( rhs.m_fDefaultRecoveryCount ),
  m_fDelayRunRecoveryCountRate( rhs.m_fDelayRunRecoveryCountRate ),
  m_fEtcRecoveryCountRate( rhs.m_fEtcRecoveryCountRate ),
  m_iDefaultRecoveryCountTic( rhs.m_iDefaultRecoveryCountTic ),
  m_bReduceOnParent( rhs.m_bReduceOnParent )
{
	m_dwPreDelayStartTime = 0;
	m_dwFireMotionEndTime = 0;
	m_dwAttackEndTime = 0;

	m_fExtraAttackMoveForce = 0.0f;
	m_fExtraAttackMoveFriction = 0.0f;

	m_bRefreshFireTimeList = true;
}

ioCountSkill::~ioCountSkill()
{
	InitExtraAniJump();
}

void ioCountSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioSkill::LoadProperty( rkLoader );

	LoadNormalAttribute( rkLoader );
	LoadDownAttribute( rkLoader );
	LoadJumpAttribute( rkLoader );

	m_bStateEscapeOnStand = rkLoader.LoadBool_e( "state_escape_on_stand", false );
	m_bStateEscapeOnAir = rkLoader.LoadBool_e( "state_escape_on_air", false );
	m_bStateEscapeOnDown = rkLoader.LoadBool_e( "state_escape_on_down", false );

	m_iMaxUseCount = rkLoader.LoadInt_e( "max_use_count", 100 );
	m_iNeedCount = rkLoader.LoadInt_e( "need_count", 1 );

	m_fDefaultRecoveryCount = rkLoader.LoadFloat_e( "default_recovery_count", 0.1f );
	m_fDelayRunRecoveryCountRate = rkLoader.LoadFloat_e( "delayrun_recovery_count_rate", FLOAT1 );
	m_fEtcRecoveryCountRate = rkLoader.LoadFloat_e( "etc_recovery_count_rate", FLOAT1 );
	m_iDefaultRecoveryCountTic = rkLoader.LoadFloat_e( "default_recovery_count_tic", 100 );

	m_bReduceOnParent = rkLoader.LoadBool_e( "reduce_gauge_on_parent", false );
}

void ioCountSkill::LoadNormalAttribute( ioINILoader &rkLoader )
{
	m_AttackAttribute.Init();
	m_AttackAttribute.m_AttackAnimation = m_SkillMotionName;
	m_AttackAttribute.m_fAttackAniRate  = m_fAnimationTimeRate;

	char szBuf[MAX_PATH];
	rkLoader.LoadString_e( "skill_end_motion", "", szBuf, MAX_PATH );

	m_AttackAttribute.m_AttackEndAnimation = szBuf;
	m_AttackAttribute.m_fAttackEndAniRate  = rkLoader.LoadFloat_e( "skill_end_time_rate", FLOAT1 );

	m_AttackAttribute.m_dwPreDelay	    = rkLoader.LoadInt_e( "animation_pre_delay", 0 );
	m_AttackAttribute.m_dwEndDelay	    = rkLoader.LoadInt_e( "animation_end_delay", 0 );

	char szKey[MAX_PATH];

	m_AttackAttribute.m_vForceInfoList.clear();
	int iForceCnt = rkLoader.LoadInt_e( "attack_force_cnt", 0 );
	if( iForceCnt > 0 )
		m_AttackAttribute.m_vForceInfoList.reserve( iForceCnt );

	for( int i=0; i < iForceCnt; ++i )
	{
		ForceInfo kInfo;

		wsprintf_e( szKey, "attack_force_move%d", i+1 );
		kInfo.m_fForceMovePower = rkLoader.LoadFloat( szKey, 0.0f );

		wsprintf_e( szKey, "attack_force_friction%d", i+1 );
		kInfo.m_fForceMoveFriction = rkLoader.LoadFloat( szKey, FLOAT1 );

		wsprintf_e( szKey, "attack_force_angle%d", i+1 );
		kInfo.m_fForceAngle = rkLoader.LoadFloat( szKey, 0.0f );

		m_AttackAttribute.m_vForceInfoList.push_back( kInfo );
	}

	int iAttackTypeCnt = rkLoader.LoadInt_e( "attack_type_count", 0 );
	m_AttackAttribute.m_vWeaponInfoList.clear();
	m_AttackAttribute.m_vWeaponInfoList.reserve( iAttackTypeCnt );

	for( int i=0 ; i<iAttackTypeCnt ; i++ )
	{
		WeaponInfo kInfo;

		wsprintf_e( szKey, "attack_type%d", i+1 );
		kInfo.m_iWeaponIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "attack_type%d_resist", i+1 );
		kInfo.m_iResistanceIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "attack_type%d_wounded", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_WoundedAnimation = szBuf;

		wsprintf_e( szKey, "attack_type%d_wounded_time", i+1 );
		kInfo.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "attack_type%d_wounded_loop_ani", i+1 );
		kInfo.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );

		m_AttackAttribute.m_vWeaponInfoList.push_back( kInfo );
	}
}

void ioCountSkill::LoadDownAttribute( ioINILoader &rkLoader )
{
	m_DownAttackAttribute.Init();
	m_DownAttackAttribute.m_AttackAnimation = m_DownSkillMotionName;
	m_DownAttackAttribute.m_fAttackAniRate  = m_fDownAnimationTimeRate;

	char szBuf[MAX_PATH];
	rkLoader.LoadString_e( "down_skill_end_motion", "", szBuf, MAX_PATH );

	m_DownAttackAttribute.m_AttackEndAnimation = szBuf;
	m_DownAttackAttribute.m_fAttackEndAniRate  = rkLoader.LoadFloat_e( "down_skill_end_time_rate", FLOAT1 );

	m_DownAttackAttribute.m_dwPreDelay = rkLoader.LoadInt_e( "down_animation_pre_delay", 0 );
	m_DownAttackAttribute.m_dwEndDelay = rkLoader.LoadInt_e( "down_animation_end_delay", 0 );

	char szKey[MAX_PATH];

	m_DownAttackAttribute.m_vForceInfoList.clear();
	int iForceCnt = rkLoader.LoadInt_e( "down_attack_force_cnt", 0 );
	if( iForceCnt > 0 )
		m_DownAttackAttribute.m_vForceInfoList.reserve( iForceCnt );

	for( int i=0; i < iForceCnt; ++i )
	{
		ForceInfo kInfo;

		wsprintf_e( szKey, "down_attack_force_move%d", i+1 );
		kInfo.m_fForceMovePower = rkLoader.LoadFloat( szKey, 0.0f );

		wsprintf_e( szKey, "down_attack_force_friction%d", i+1 );
		kInfo.m_fForceMoveFriction = rkLoader.LoadFloat( szKey, FLOAT1 );

		wsprintf_e( szKey, "down_attack_force_angle%d", i+1 );
		kInfo.m_fForceAngle = rkLoader.LoadFloat( szKey, 0.0f );

		m_DownAttackAttribute.m_vForceInfoList.push_back( kInfo );
	}

	int iAttackTypeCnt = rkLoader.LoadInt_e( "down_attack_type_count", 1 );
	m_DownAttackAttribute.m_vWeaponInfoList.clear();
	m_DownAttackAttribute.m_vWeaponInfoList.reserve( iAttackTypeCnt );

	for( int i=0 ; i<iAttackTypeCnt ; i++ )
	{
		WeaponInfo kInfo;

		wsprintf_e( szKey, "down_attack_type%d", i+1 );
		kInfo.m_iWeaponIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "down_attack_type%d_resist", i+1 );
		kInfo.m_iResistanceIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "down_attack_type%d_wounded", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_WoundedAnimation = szBuf;

		wsprintf_e( szKey, "down_attack_type%d_wounded_time", i+1 );
		kInfo.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "down_attack_type%d_wounded_loop_ani", i+1 );
		kInfo.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );

		m_DownAttackAttribute.m_vWeaponInfoList.push_back( kInfo );
	}
}

void ioCountSkill::LoadJumpAttribute( ioINILoader &rkLoader )
{
	m_JumpAttackAttribute.Init();
	m_JumpAttackAttribute.m_AttackAnimation = m_JumpSkillMotionName;
	m_JumpAttackAttribute.m_fAttackAniRate  = m_fJumpAnimationTimeRate;

	char szBuf[MAX_PATH];
	rkLoader.LoadString_e( "jump_skill_end_motion", "", szBuf, MAX_PATH );

	m_JumpAttackAttribute.m_AttackEndAnimation = szBuf;
	m_JumpAttackAttribute.m_fAttackEndAniRate  = rkLoader.LoadFloat_e( "jump_skill_end_time_rate", FLOAT1 );

	m_JumpAttackAttribute.m_dwPreDelay = rkLoader.LoadInt_e( "jump_animation_pre_delay", 0 );
	m_JumpAttackAttribute.m_dwEndDelay = rkLoader.LoadInt_e( "jump_animation_end_delay", 0 );

	char szKey[MAX_PATH];

	m_JumpAttackAttribute.m_vForceInfoList.clear();
	int iForceCnt = rkLoader.LoadInt_e( "jump_attack_force_cnt", 0 );
	if( iForceCnt > 0 )
		m_JumpAttackAttribute.m_vForceInfoList.reserve( iForceCnt );

	for( int i=0; i < iForceCnt; ++i )
	{
		ForceInfo kInfo;

		wsprintf_e( szKey, "jump_attack_force_move%d", i+1 );
		kInfo.m_fForceMovePower = rkLoader.LoadFloat( szKey, 0.0f );

		wsprintf_e( szKey, "jump_attack_force_friction%d", i+1 );
		kInfo.m_fForceMoveFriction = rkLoader.LoadFloat( szKey, FLOAT1 );

		wsprintf_e( szKey, "jump_attack_force_angle%d", i+1 );
		kInfo.m_fForceAngle = rkLoader.LoadFloat( szKey, 0.0f );

		m_JumpAttackAttribute.m_vForceInfoList.push_back( kInfo );
	}

	int iAttackTypeCnt = rkLoader.LoadInt_e( "jump_attack_type_count", 1 );
	m_JumpAttackAttribute.m_vWeaponInfoList.clear();
	m_JumpAttackAttribute.m_vWeaponInfoList.reserve( iAttackTypeCnt );

	for( int i=0 ; i<iAttackTypeCnt ; i++ )
	{
		WeaponInfo kInfo;

		wsprintf_e( szKey, "jump_attack_type%d", i+1 );
		kInfo.m_iWeaponIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "jump_attack_type%d_resist", i+1 );
		kInfo.m_iResistanceIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "jump_attack_type%d_wounded", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_WoundedAnimation = szBuf;

		wsprintf_e( szKey, "jump_attack_type%d_wounded_time", i+1 );
		kInfo.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "jump_attack_type%d_wounded_loop_ani", i+1 );
		kInfo.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );

		m_JumpAttackAttribute.m_vWeaponInfoList.push_back( kInfo );
	}
}

AttackAttribute& ioCountSkill::GetAttribute()
{
	switch( m_SkillUseType )
	{
	case SUT_NORMAL:
		return m_AttackAttribute;
	case SUT_DOWN:
		if( !m_DownAttackAttribute.m_AttackAnimation.IsEmpty() )
			return m_DownAttackAttribute;
		return m_AttackAttribute;
	case SUT_JUMP:
		if( !m_JumpAttackAttribute.m_AttackAnimation.IsEmpty() )
			return m_JumpAttackAttribute;
		return m_AttackAttribute;
	}

	return m_AttackAttribute;
}

const AttackAttribute& ioCountSkill::GetAttributeConst() const
{
	switch( m_SkillUseType )
	{
	case SUT_NORMAL:
		return m_AttackAttribute;
	case SUT_DOWN:
		if( !m_DownAttackAttribute.m_AttackAnimation.IsEmpty() )
			return m_DownAttackAttribute;
		return m_AttackAttribute;
	case SUT_JUMP:
		if( !m_JumpAttackAttribute.m_AttackAnimation.IsEmpty() )
			return m_JumpAttackAttribute;
		return m_AttackAttribute;
	}

	return m_AttackAttribute;
}

ioSkill* ioCountSkill::Clone()
{
	return new ioCountSkill( *this );
}

int ioCountSkill::GetTypeID() const
{
	return ST_COUNT;
}

bool ioCountSkill::IsCanUseSkill( ioBaseChar *pChar, bool bHelpMsg )
{
	ioAttackableItem *pAttackable = ToAttackableItem( GetOwnerItem() );
	if( pAttackable )
	{
		ioHashString szSkillMotion = GetSkillMotionName();

		if( !pAttackable->IsCanFire( pChar ) )
			return false;
	}

	if( m_iCurUseCount < m_iNeedCount )
		return false;

	return true;
}

bool ioCountSkill::OnSkillStart( ioBaseChar *pChar )
{
	if( !pChar )
		return false;

	if ( m_bPetForceHide )
	{
		ioPet *pPet = g_PetMgr.FindPet( pChar );
		if ( pPet )
			pPet->SetForceHide( true );
	}

	if( !m_bNoCheckSkillmotion && !HasSkillMotion() )
		return false;

	pChar->SetSkillProtection();
	
	m_bChangeDirRot = false;
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
	CheckInitMoveSpeed( pChar );

	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();
	if( !StartPreDelayAnimation( pChar ) )	// Delay AnimationÀÌ ¾ø´Ù
	{
		AttackSkillFire( pChar );
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

	if( m_bReduceOnParent )
		ReduceNeedCount( m_iNeedCount );

	return true;
}

void ioCountSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( m_dwPreDelayStartTime == 0 )
	{
		CheckExtraAniJump( pChar );
		CheckAniRotate( pChar );
		return;
	}

	DWORD dwPreDelay = GetPreDelayTime();

	if( m_dwPreDelayStartTime + dwPreDelay < FRAMEGETTIME() )
	{
		m_dwPreDelayStartTime = 0;

		AttackSkillFire( pChar );
		EndPreDelayEffect( pChar );
	}
}

void ioCountSkill::OnSkillEnd( ioBaseChar *pChar )
{
	RemoveExtraAttackForce();
	EndPreDelayEffect( pChar );

	pChar->EndEffect( m_SkillEffectName );

	m_bChangeGrapplingTargetPos = true;
	pChar->SetMoveDirByRotate( pChar->GetTargetRot() );
	pChar->RestoreSkillProtection();

	m_SkillUseType = SUT_NORMAL;

	if( !m_SkillEndEffectName.IsEmpty() )
		pChar->AttachEffect( m_SkillEndEffectName );

	ioMechanicsItem *pMechanics = ToMechanicsItem( GetOwnerItem() );
	ioPhantomItem *pPhantom = ToPhantomItem( GetOwnerItem() );
	ioDestroyerItem *pDestroyer = ToDestroyerItem( GetOwnerItem() );

	if( pMechanics )
		pMechanics->ClearMechanicsTargetSkill();
	else if( pPhantom )
		pPhantom->ClearPhantomTargetSkill();
	else if( pDestroyer )
		pDestroyer->ClearDestroyerTargetSkill();

	if ( m_bPetForceHide )
	{
		ioPet *pPet = g_PetMgr.FindPet( pChar );
		if ( pPet )
			pPet->SetForceHide( false );
	}
}

bool ioCountSkill::StartPreDelayAnimation( ioBaseChar *pChar )
{
	ioHashString szPreDelay = GetPreDelayAniName();

	if( szPreDelay.IsEmpty() )
	{
		m_dwPreDelayStartTime = 0;
		return false;
	}

	ioEntityGroup *pGrp = pChar->GetGroup();
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetLoopAni( szPreDelay, FLOAT100 );

	m_dwPreDelayStartTime = FRAMEGETTIME();
	StartPreDelayEffect( pChar );
	
	return true;
}

void ioCountSkill::AttackSkillFire( ioBaseChar *pChar )
{
	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( GetAttribute().m_AttackAnimation );
	if( iAniID == -1 )	return;

	pChar->AttachEffect( m_SkillEffectName );

	float fTimeRate  = GetAttackTimeRate();
	DWORD dwPreDelay = GetAttribute().m_dwPreDelay;

	if( m_bRefreshFireTimeList )
	{
		pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex,
											iAniID,
											GetAttribute().m_vWeaponInfoList,
											FTT_SKILL_ATTACK,
											fTimeRate,
											dwPreDelay );
	}

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	m_dwMotionStartTime = m_dwMotionEndTime = FRAMEGETTIME();
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwMotionEndTime += dwPreDelay + GetAttribute().m_dwEndDelay;

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwFireStartTime;
	dwFireStartTime = m_dwFireMotionEndTime = dwCurTime;

	dwFireStartTime += dwPreDelay;
	dwFireStartTime += pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;

	float fAttackEnd = pGrp->GetAnimationEventTime_e( iAniID, "attack_end" ) * fTimeRate;
	if( fAttackEnd > 0.0f )
		m_dwAttackEndTime = dwCurTime + fAttackEnd;
	else
		m_dwAttackEndTime = 0;

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


	pChar->SetReservedSliding( GetAttribute().m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );
}

void ioCountSkill::StartPreDelayEffect( ioBaseChar *pChar )
{
	ioSkill::StartPreDelayEffect( pChar );

	if( GetOwnerItem() )
	{
		GetOwnerItem()->AddGlowLine( pChar );
	}
}

void ioCountSkill::EndPreDelayEffect( ioBaseChar *pChar )
{
	ioSkill::EndPreDelayEffect( pChar );

	if( GetOwnerItem() )
	{
		GetOwnerItem()->RemoveGlowLine( pChar );
	}
}

void ioCountSkill::EnableRefreshFireTimeList( bool bRefresh )
{
	m_bRefreshFireTimeList = bRefresh;
}

float ioCountSkill::GetAttackTimeRate() const
{
	float fTimeRate = GetAttributeConst().m_fAttackAniRate * m_fExtraAniRate;
	return fTimeRate;
}

float ioCountSkill::GetAttackEndTimeRate() const
{
	return GetAttributeConst().m_fAttackEndAniRate;
}

void ioCountSkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
}

bool ioCountSkill::IsUseAutoTargetValue()
{
	return ioSkill::IsUseAutoTargetValue();
}

void ioCountSkill::SetExtraAttackForce()
{
	ForceInfoList::iterator iter = GetAttribute().m_vForceInfoList.begin();

	while( iter != GetAttribute().m_vForceInfoList.end() )
	{
		(*iter).m_fForceMovePower += m_fExtraAttackMoveForce;
		(*iter).m_fForceMoveFriction += m_fExtraAttackMoveFriction;

		++iter;
	}
}

void ioCountSkill::RemoveExtraAttackForce()
{
	ForceInfoList::iterator iter = GetAttribute().m_vForceInfoList.begin();

	while( iter != GetAttribute().m_vForceInfoList.end() )
	{
		(*iter).m_fForceMovePower -= m_fExtraAttackMoveForce;
		(*iter).m_fForceMoveFriction -= m_fExtraAttackMoveFriction;

		++iter;
	}

	m_fExtraAttackMoveForce = 0.0f;
	m_fExtraAttackMoveFriction = 0.0f;
}

bool ioCountSkill::IsAttackEndState() const
{
	if( m_dwAttackEndTime > 0 && m_dwAttackEndTime < FRAMEGETTIME() )
	{
		return true;
	}

	return false;
}

bool ioCountSkill::IsCanStateEscape() const
{
	if( m_bStateEscapeOnStand || m_bStateEscapeOnAir || m_bStateEscapeOnDown )
		return true;

	return false;
}

bool ioCountSkill::IsCanStateEscapeOnStand( ioBaseChar *pChar ) const
{
	if( !pChar )	return false;

	if( m_bStateEscapeOnStand &&
		!pChar->IsFloatingState() &&
		!pChar->IsApplyDownState(false) &&
		!pChar->IsBlowDownState() )
		return true;

	return false;
}

bool ioCountSkill::IsCanStateEscapeOnAir( ioBaseChar *pChar ) const
{
	if( !pChar )	return false;
	if( !m_bEnableJumpingSkill )	return false;

	if( m_bStateEscapeOnAir && pChar->IsFloating() )
		return true;

	return false;
}

bool ioCountSkill::IsCanStateEscapeOnDown( ioBaseChar *pChar ) const
{
	if( !pChar )	return false;
	if( !m_bEnableDownSkill )	return false;

	if( m_bStateEscapeOnDown && !pChar->IsFloating() &&
		( pChar->IsApplyDownState( false ) || pChar->IsBlowDownState() ) )
		return true;

	return false;
}

bool ioCountSkill::EnableRecoveryCount( ioBaseChar *pChar )
{
	if( !pChar )
		return false;

	if( m_iCurUseCount == m_iMaxUseCount )
		return false;

	CharState eState = pChar->GetState();
	switch( eState )
	{
	case CS_USING_SKILL:
		return false;
	}

	return true;
}

void ioCountSkill::RecoveryCount( ioBaseChar *pChar )
{
	if( !pChar )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	int iTickGap = (int)( dwCurTime - m_dwTicCheckTime );
	if( iTickGap >= m_iDefaultRecoveryCountTic )
	{
		while( iTickGap >= m_iDefaultRecoveryCountTic )
		{
			float fRecovery = m_fDefaultRecoveryCount;
			if( pChar->GetState() == CS_DELAY || pChar->GetState() == CS_RUN )
				fRecovery *= m_fDelayRunRecoveryCountRate;
			else
				fRecovery *= m_fEtcRecoveryCountRate;

			m_fCurRecoveryCount = min( m_fCurRecoveryCount+fRecovery, FLOAT1 );

			iTickGap -= m_iDefaultRecoveryCountTic;
		}

		m_dwTicCheckTime = dwCurTime - iTickGap;
	}

	if( m_fCurRecoveryCount >= FLOAT1 )
	{
		m_fCurRecoveryCount = 0.0f;
		m_iCurUseCount++;
		m_iCurUseCount = min( m_iCurUseCount, m_iMaxUseCount );
	}
}

void ioCountSkill::SetEquip()
{
	m_dwTicCheckTime = FRAMEGETTIME();
	m_iCurUseCount = 0;
	m_fCurRecoveryCount = 0;
}

int ioCountSkill::ReduceNeedCount( int iNeedCount )
{
	m_iCurUseCount -= iNeedCount;
	m_iCurUseCount = max( m_iCurUseCount, 0 );

	return m_iCurUseCount;
}