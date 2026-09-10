

#include "stdafx.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"
#include "ioGrowthUpInfo.h"

#include "ioCircleMoveWeapon2Skill.h"
#include "ioNoneTraceDummy2.h"
#include "WeaponDefine.h"

ioCircleMoveWeapon2Skill::ioCircleMoveWeapon2Skill()
{
	ClearData();
}

ioCircleMoveWeapon2Skill::ioCircleMoveWeapon2Skill( const ioCircleMoveWeapon2Skill &rhs )
: ioNormalSkill( rhs ),
 m_dwFireGapTime( rhs.m_dwFireGapTime ),
 m_fFireHightOffset( rhs.m_fFireHightOffset ),
 m_fFireFrontOffset( rhs.m_fFireFrontOffset ),
 m_iRandomRange( rhs.m_iRandomRange ),
 m_iTotalFireCount( rhs.m_iTotalFireCount ),
 m_fCircleRange( rhs.m_fCircleRange ),
 m_fCircleMoveSpeed( rhs.m_fCircleMoveSpeed ),
 m_fCircleHeightGap( rhs.m_fCircleHeightGap ),
 m_fCircleOffSet( rhs.m_fCircleOffSet ),
 m_ReadyCircle( rhs.m_ReadyCircle ),
 m_TargetCircleEffect( rhs.m_TargetCircleEffect ),
 m_StartEffect( rhs.m_StartEffect ),
 m_EndEffect( rhs.m_EndEffect ),
 m_RedEffect( rhs.m_RedEffect ),
 m_BlueEffect( rhs.m_BlueEffect ),
 m_AttackAttribute( rhs.m_AttackAttribute ),
 m_EndAttackAttribute( rhs.m_EndAttackAttribute ),
 m_szSkillLoopMotion( rhs.m_szSkillLoopMotion ),
 m_fSkillLoopAniRate( rhs.m_fSkillLoopAniRate ),
 m_dwSkillLoopDuration( rhs.m_dwSkillLoopDuration ),
 m_dwSkillLoopProtectTime( rhs.m_dwSkillLoopProtectTime ),
 m_dwRotateTime( rhs.m_dwRotateTime ),
 m_fWeaponGravityRate( rhs.m_fWeaponGravityRate ),
 m_fWeaponFloatPower( rhs.m_fWeaponFloatPower ),
 m_fWeaponFloatMinPower( rhs.m_fWeaponFloatMinPower ),
 m_fWeaponGapDistance( rhs.m_fWeaponGapDistance ),
 m_fWeaponMinSpeed( rhs.m_fWeaponMinSpeed ),
 m_fRevisionRate( rhs.m_fRevisionRate ),
 m_fUpLimitHeight( rhs.m_fUpLimitHeight ),
 m_fUpLimitRange( rhs.m_fUpLimitRange ),
 m_fCircleSlowDownSpeedRate( rhs.m_fCircleSlowDownSpeedRate ),
 m_fGuagePerTic( rhs.m_fGuagePerTic ),
 m_bDisableGrowth( rhs.m_bDisableGrowth ),
 m_dwTicTime( rhs.m_dwTicTime ),
 m_bSetNoMoveCircle( rhs.m_bSetNoMoveCircle ),
 m_fCircleMaxMoveRange( rhs.m_fCircleMaxMoveRange ),
 m_bSetNoInputKey( rhs.m_bSetNoInputKey ),
 m_FireType( rhs.m_FireType ),
 m_dwAttackKeyInputTime( rhs.m_dwAttackKeyInputTime ),
 m_DummyCharList( rhs.m_DummyCharList ),
 m_vDestroyDummyCharList( rhs.m_vDestroyDummyCharList ),
 m_bDummyLookAtCircle( rhs.m_bDummyLookAtCircle ),
 m_FireAttribute( rhs.m_FireAttribute )
{
	ClearData();
}

ioCircleMoveWeapon2Skill::~ioCircleMoveWeapon2Skill()
{
}

void ioCircleMoveWeapon2Skill::LoadProperty( ioINILoader &rkLoader )
{
	ioNormalSkill::LoadProperty( rkLoader );

	char szKey[MAX_PATH], szBuf[MAX_PATH];

	m_dwFireGapTime = (DWORD)rkLoader.LoadInt_e( "fire_gap_time", 0 );
	m_fFireHightOffset	= rkLoader.LoadFloat_e( "fire_height_offset", 0.0f );
	m_fFireFrontOffset	= rkLoader.LoadFloat_e( "fire_front_offset", 0.0f );
	m_iRandomRange	= rkLoader.LoadFloat_e( "fire_random_range", 0.0f );
	m_iTotalFireCount	= rkLoader.LoadFloat_e( "fire_count", 0 );

	m_fCircleRange		= rkLoader.LoadFloat_e( "magic_circle_range", 0.0f );
	m_fCircleMoveSpeed	= rkLoader.LoadFloat_e( "magic_circle_move_speed", 0.0f );
	m_fCircleHeightGap	= rkLoader.LoadFloat_e( "magic_circle_height_gap", 0.0f );
	m_fCircleOffSet		= rkLoader.LoadFloat_e( "magic_circle_offset", 0.0f );

	rkLoader.LoadString_e( "ready_circle", "", szBuf, MAX_PATH );
	m_ReadyCircle = szBuf;

	rkLoader.LoadString_e( "target_circle_effect", "", szBuf, MAX_PATH );
	m_TargetCircleEffect = szBuf;

	rkLoader.LoadString_e( "red_team_circle", "", szBuf, MAX_PATH );
	m_RedEffect = szBuf;
	rkLoader.LoadString_e( "blue_team_circle", "", szBuf, MAX_PATH );
	m_BlueEffect = szBuf;

	rkLoader.LoadString_e( "start_effect", "", szBuf, MAX_PATH );
	m_StartEffect = szBuf;
	rkLoader.LoadString_e( "end_effect", "", szBuf, MAX_PATH );
	m_EndEffect = szBuf;

	// Action Attack
	m_AttackAttribute.Init();
	m_AttackAttribute.m_AttackAnimation = m_SkillMotionName;
	m_AttackAttribute.m_fAttackAniRate  = m_fAnimationTimeRate;
	m_AttackAttribute.m_dwPreDelay	    = rkLoader.LoadInt_e( "animation_pre_delay", 0 );
	m_AttackAttribute.m_dwEndDelay	    = rkLoader.LoadInt_e( "animation_end_delay", 0 );

	rkLoader.LoadString_e( "skill_end_motion", "", szBuf, MAX_PATH );
	m_AttackAttribute.m_AttackEndAnimation = szBuf;
	m_AttackAttribute.m_fAttackEndAniRate = rkLoader.LoadFloat_e( "end_motion_rate", FLOAT1 );

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

	//EndAttackAttribute
	int iEndAttackTypeCnt = rkLoader.LoadInt_e( "end_attack_type_count", 0 );
	m_EndAttackAttribute.m_vWeaponInfoList.clear();
	m_EndAttackAttribute.m_vWeaponInfoList.reserve( iEndAttackTypeCnt );

	for( int i=0 ; i<iEndAttackTypeCnt ; i++ )
	{
		WeaponInfo kInfo;

		wsprintf_e( szKey, "end_attack_type%d", i+1 );
		kInfo.m_iWeaponIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "end_attack_type%d_resist", i+1 );
		kInfo.m_iResistanceIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "end_attack_type%d_wounded", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_WoundedAnimation = szBuf;

		wsprintf_e( szKey, "end_attack_type%d_wounded_time", i+1 );
		kInfo.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "end_attack_type%d_wounded_loop_ani", i+1 );
		kInfo.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );

		m_EndAttackAttribute.m_vWeaponInfoList.push_back( kInfo );
	}

	// Loop Motion
	rkLoader.LoadString_e( "skill_loop_motion", "", szBuf, MAX_PATH );
	m_szSkillLoopMotion = szBuf;
	m_fSkillLoopAniRate = rkLoader.LoadFloat_e( "skill_loop_motion_rate", FLOAT1 );
	m_dwSkillLoopDuration = (DWORD)rkLoader.LoadInt_e( "skill_loop_duration", 0 );
	m_dwSkillLoopProtectTime = (DWORD)rkLoader.LoadInt_e( "skill_loop_protect_time", 0 );

	// Character Rotate
	m_dwRotateTime = (DWORD)rkLoader.LoadInt_e( "rotate_speed", 0 );

	// Weapon
	m_fWeaponGravityRate = rkLoader.LoadFloat_e( "weapon_gravity_rate", FLOAT1 );
	m_fWeaponFloatPower = rkLoader.LoadFloat_e( "weapon_float_power", 0.0f );
	m_fWeaponFloatMinPower = rkLoader.LoadFloat_e( "weapon_float_min_power", 0.0f );
	m_fWeaponMinSpeed = rkLoader.LoadFloat_e( "weapon_min_speed", 0.0f );
	m_fWeaponGapDistance = rkLoader.LoadFloat_e( "weapon_gap_distance", 0.0f );
	m_fRevisionRate = rkLoader.LoadFloat_e( "weapon_revision_rate", 0.0f );

	m_fUpLimitHeight = rkLoader.LoadFloat_e( "up_limit_height", 30.0f );
	m_fUpLimitRange = rkLoader.LoadFloat_e( "up_limit_range", 0.0f );

	m_fCircleSlowDownSpeedRate = rkLoader.LoadFloat_e( "magic_circle_slow_down_speed_rate", FLOAT1 );

	m_fGuagePerTic = rkLoader.LoadFloat_e( "gauge_per_tick", 0.0f );
	m_bDisableGrowth = rkLoader.LoadBool_e( "disable_growth", false );
	m_dwTicTime = (DWORD)rkLoader.LoadInt_e( "tick_time", 0 );

	m_bSetNoMoveCircle = rkLoader.LoadBool_e( "set_no_move_circle", false );
	m_fCircleMaxMoveRange = rkLoader.LoadFloat_e( "magic_circle_max_move_range", 0.0f );
	m_bSetNoInputKey = rkLoader.LoadBool_e( "set_no_input_key", false );

	m_FireType = (FireType)rkLoader.LoadInt_e( "fire_type", FT_AUTO );
	m_dwAttackKeyInputTime = (DWORD)rkLoader.LoadInt_e( "attack_key_input_time", 0 );

	//Dummy
	m_DummyCharList.clear();
	m_vDestroyDummyCharList.clear();

	Help::LoadDummyCharCreateDestroyInfo( rkLoader, m_DummyCharList, m_vDestroyDummyCharList );
	m_bDummyLookAtCircle = rkLoader.LoadBool_e( "dummy_lookat_circle", false );

	LoadAttackAttribute( "fire_attack", m_FireAttribute, rkLoader );
}

void ioCircleMoveWeapon2Skill::ClearData()
{
	m_State = SS_NONE;

	m_dwCurMapEffect		= -1;
	m_dwTargetCircleEffect	= -1;

	m_bSetHandMesh = false;
	m_bReduceGauge = false;
	
	m_dwFireStartTime		= 0;
	m_dwSkillLoopStartTime	= 0;
	m_dwNextFireStartTime	= 0;
	
	m_iCurFireCount = 0;

	m_fFirstTargetDistance = 0.0f;

	m_bTargetRot = false;
	m_bLeftRot = false;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	m_fTicGaugeRateByGrowth = FLOAT1;
	m_dwTicTimeByGrowth = 0;
	m_dwCheckTime = 0;
}

ioSkill* ioCircleMoveWeapon2Skill::Clone()
{
	return new ioCircleMoveWeapon2Skill( *this );
}

bool ioCircleMoveWeapon2Skill::OnSkillStart( ioBaseChar *pChar )
{	
	if( !HasSkillMotion() )
		return false;

	ClearData();

	m_dwEnableSwitchTime = 0;
	m_dwEnableSemiSwitchTime = 0;

	if( m_dwSwitchSkill > 0 )
		m_dwEnableSwitchTime = FRAMEGETTIME() + m_dwSwitchSkill;
	if( m_dwSemiSwitchSkill > 0 )
		m_dwEnableSemiSwitchTime = FRAMEGETTIME() + m_dwSemiSwitchSkill;

	m_dwSkillStartTime = FRAMEGETTIME();

	pChar->AttachEffect( m_SkillEffectName );

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	if( !StartPreDelayAnimation( pChar ) )	// Delay Animation이 없다
		return false;

	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();

	pChar->SetSkillProtection();

	SetCameraBuff( pChar );

	CheckSkillDesc( pChar );

	if( m_bInitJumpPower )
	{
		pChar->SetCurJumpPowerAmt( 0.0f );
		pChar->SetGravityAmt( 0.0f );
	}

	m_dwCheckTime = FRAMEGETTIME();

	// 성장관련 요소 추가
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

	SetSkillNameChatBubble( pChar );

	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}

	return true;
}

void ioCircleMoveWeapon2Skill::OnSkillEnd( ioBaseChar *pChar )
{
	ioNormalSkill::OnSkillEnd( pChar );

	// 기본무기 장착
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( true );
	}

	// 스킬 중간 동작에 사용된 메쉬들 제거
	m_bSetHandMesh = false;
	if( !m_HandMesh.IsEmpty() )
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );

	EndCameraBuff( pChar );

	pChar->SetMagicCircleTarget( "" );

	//DestroyMapEffect
	if( m_dwCurMapEffect != -1 )
		pChar->EndMapEffect( m_dwCurMapEffect, false );

	if( m_dwTargetCircleEffect != -1 )
		pChar->EndMapEffect( m_dwTargetCircleEffect, false );

	m_dwCurMapEffect = -1;
	m_dwTargetCircleEffect = -1;

	pChar->DestroyMagicCircle();

	m_dwMotionEndTime = 0;
	m_dwMotionStartTime = 0;
	m_State = SS_NONE;

	g_DummyCharMgr.DestroyDummyCharByNameList( pChar, m_vDestroyDummyCharList );

	ClearData();
}

void ioCircleMoveWeapon2Skill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();
	float fCurTicGauge = m_fGuagePerTic / m_fTicGaugeRateByGrowth;
	int iTickGap = (int)( dwCurTime - m_dwCheckTime );
	int iCurTicTime = m_dwTicTime + m_dwTicTimeByGrowth;

	float fValue = 0.0f;
	ioGrowthNormalUpInfo *pUpInfo = ToNormalUpInfo( GetGrowthUpInfoByType(GT_MAGIC_CIRCLE_RANGE, pChar->GetCharName()) );
	if( pUpInfo )
	{
		fValue = pUpInfo->GetValue(pStage);
	}

	float fCurRange = m_fCircleRange + fValue;

	switch( m_State )
	{
	case SS_NONE:
		{
			DWORD dwPreDelay = GetPreDelayTime();
			if( m_dwPreDelayStartTime + dwPreDelay < dwCurTime )
			{
				m_dwPreDelayStartTime = 0;

				SetNormalSkill( pChar );
				EndPreDelayEffect( pChar );
				CreateDummyChar( pChar, pStage );
			}
		}
		break;
	case SS_ACTION:
		{
			if( m_dwMotionEndTime < dwCurTime )
			{
				SetLoopState( pChar, pStage );
			}
		}
		break;
	case SS_LOOP:
		{
			if( m_dwSkillLoopDuration > 0 && m_dwSkillLoopStartTime + m_dwSkillLoopDuration < dwCurTime )
			{
				SetEndState( pChar );
				return;
			}
			
			if ( !m_bSetNoInputKey )
				CheckKeyInput( pChar );

			ProcessRotate( pChar );
			OnLoopState( pChar, pStage );
			ProcessDummyRotate( pChar, pStage );
			if ( m_FireType == FT_AUTO )
				AttackFire( pChar );
			else if ( m_FireType == FT_ATTACK_KEY )
				AttackFireByKeyInput( pChar );
			else if ( m_FireType == FT_FIRE_ANI && pChar->IsAttackKey() )
				SetFireState( pChar );
		}
		break;
	case SS_FIRE_ACTION:
		{
			if ( m_dwMotionEndTime > 0 && m_dwMotionEndTime < FRAMEGETTIME() )
			{
				SetLoopState( pChar, pStage );
				return;
			}

			if ( m_dwFireStartTime > 0 && m_dwFireStartTime < FRAMEGETTIME() )
			{
				m_dwFireStartTime = 0;
				CreateWeapon( pChar );
			}

			ProcessRotate( pChar );
			OnLoopState( pChar, pStage );
			ProcessDummyRotate( pChar, pStage );
		}
		break;
	case SS_END:
		break;
	}

	if( m_State != SS_END )
	{
		if( iTickGap >= iCurTicTime )
		{
			m_dwCheckTime = dwCurTime - ( iTickGap - iCurTicTime );

			if( fCurTicGauge != 0.0f )
			{				
				if( pChar->ReduceCurSkillGaugeBySkill( GetOwnerItem(), fCurTicGauge ) <= 0.0f )
				{
					SetEndState( pChar );					
				}
			}
		}
	}
}

void ioCircleMoveWeapon2Skill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	SetChargingRateForWeapon( pWeapon );
	
	ioZoneEffectWeapon *pZoneEffect = ToZoneEffectWeapon( pWeapon );

	if( pZoneEffect )
	{
		ioBaseChar *pChar = pZoneEffect->GetOwner();
		if( pChar )
		{
			D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			pZoneEffect->SetCollisionDir( vDir );
			pZoneEffect->SetZoneValue( false, true );
		}
	}
	else
	{
		D3DXVECTOR3 vDir = ioMath::VEC3_ZERO;
		float fSpeed = 0.0f;
		float fFloat = 0.0f;

		pWeapon->SetMoveDir( m_vWeaponMoveInfo.m_vMoveDir );
		pWeapon->SetMoveSpeed( m_vWeaponMoveInfo.m_fMoveSpeed );
		pWeapon->SetFloatPower( m_vWeaponMoveInfo.m_fFloatPower );

		ioFloatThrowWeapon *pFloatWeapon = ToFloatThrowWeapon(pWeapon);
		if( pFloatWeapon )
		{
			pFloatWeapon->SetGravityRate( m_fWeaponGravityRate );
		}
	}
}

void ioCircleMoveWeapon2Skill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	if ( !pChar || !pStage )
		return;

	BYTE btState;
	rkPacket >> btState;

	if ( btState == SST_ROTATE )
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

			m_CurDirKey = ioUserKeyInput::DirKeyInput( iDir );
		}
	}
	else if ( btState == SST_ATTACK_KEY_INPUT )
	{
		D3DXQUATERNION qtRot;
		rkPacket >> qtRot;
		pChar->SetTargetRotToRotate( qtRot, true, false );

		rkPacket >> m_vCirclePos;

		//Weapon Attack
		D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVECTOR3 vFirePos = pChar->GetWorldPosition();
		vFirePos.y += m_fFireHightOffset;
		vFirePos += ( vDir * m_fFireFrontOffset );

		int iSize = (int)m_AttackAttribute.m_vWeaponInfoList.size();
		for( int i=0; i<iSize; i++ )
		{
			CheckWeaponFloatInfo( pChar );			

			pChar->SkillFireExplicit( m_AttackAttribute.m_vWeaponInfoList[i],
				GetName(),
				m_dwWeaponBaseIndex + m_iCurFireCount,
				vFirePos );

			m_iCurFireCount++;
		}
	}
	else if ( btState == SST_CREAT_DUMMY )
	{
		g_DummyCharMgr.DestroyDummyCharByNameList( pChar, m_vDestroyDummyCharList );

		D3DXQUATERNION qtRot;
		rkPacket >> qtRot;
		pChar->SetTargetRotToRotate( qtRot, true, false );

		int iCreateCnt;
		rkPacket >> iCreateCnt;

		for( int i=0; i < iCreateCnt; ++i )
		{
			int iCurIndex;
			ioHashString szDummyCharName;
			float fStartAngle;
			D3DXVECTOR3 vTargetPos;

			rkPacket >> szDummyCharName >> iCurIndex;
			rkPacket >> fStartAngle;
			rkPacket >> vTargetPos;

			m_dwFireStartTime = 0;

			ioDummyChar *pDummy = pStage->CreateDummyChar( szDummyCharName, iCurIndex,
				pChar->GetCharName(),
				vTargetPos,
				fStartAngle,
				0, false );

			if( pDummy )
				pDummy->SetControlSkillName( GetName() );

			DummyCharCreateInfo kInfo;
			kInfo.m_DummyCharName = szDummyCharName;
			kInfo.m_iDummyCharIdx = iCurIndex;
			kInfo.m_fDummyCharStartAngle = fStartAngle;
			kInfo.m_vTargetPos = vTargetPos;

			m_CreateDummyList.push_back( kInfo );
		}
	}
	else if ( btState == SST_FIRE_ACTION )
	{
		D3DXQUATERNION qtRot;
		rkPacket >> qtRot;
		pChar->SetTargetRotToRotate( qtRot, true, false );

		rkPacket >> m_vCirclePos;
		SetFireState( pChar );
	}
}

bool ioCircleMoveWeapon2Skill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

bool ioCircleMoveWeapon2Skill::IsProtected( int iDefenseBreakType ) const
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

	if( m_dwPreDelayStartTime == 0 )
	{
		if( m_dwFireMotionEndTime > FRAMEGETTIME() )
			return true;
	}

	if( m_State == SS_LOOP )
	{
		if( m_dwSkillLoopProtectTime > 0 &&
			m_dwSkillLoopStartTime + m_dwSkillLoopProtectTime > FRAMEGETTIME() )
		{
			return true;
		}
	}

	return false;
}

bool ioCircleMoveWeapon2Skill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	switch( m_State )
	{
	case SS_NONE:
	case SS_ACTION:
	case SS_LOOP:
		break;
	case SS_END:
		if( m_dwMotionEndTime > FRAMEGETTIME() )
			return false;

		return true;
	}
	return false;
}

bool ioCircleMoveWeapon2Skill::StartPreDelayAnimation( ioBaseChar *pChar )
{
	if( !ioNormalSkill::StartPreDelayAnimation( pChar ) )
		return false;

	return true;
}

void ioCircleMoveWeapon2Skill::SetNormalSkill( ioBaseChar *pChar )
{
	ioEntityGroup *pGrp = pChar->GetGroup();
	ioHashString szSkillMotion = GetSkillMotionName();

	int iCurSkillAniID = pGrp->GetAnimationIdx( szSkillMotion );
	if( iCurSkillAniID == -1 )	return;

	float fAniRate = GetSkillMotionRate();

	float fTimeRate = fAniRate * m_fExtraAniRate;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	
	pGrp->SetActionAni( iCurSkillAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionStartTime = dwCurTime;
	m_dwMotionEndTime = m_dwMotionStartTime + pGrp->GetAnimationFullTime( iCurSkillAniID ) * fTimeRate;
	m_dwMotionEndTime += m_dwAnimationEndDelay;

	m_dwFireMotionEndTime = dwCurTime;
	m_dwFireMotionEndTime += pGrp->GetAnimationEventTime_e( iCurSkillAniID, "firemotion_e" ) * fTimeRate;

	pChar->AttachEffect( m_StartEffect );

	if( !m_bReduceGauge )
	{
		pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
		m_bReduceGauge = true;
	}

	m_State = SS_ACTION;
}

void ioCircleMoveWeapon2Skill::SetLoopState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	m_State = SS_LOOP;

	if( !pChar->CheckMagicCircle() )
	{
		ioHashString szDummyCircle;
		pChar->CreateMagicCircle( m_ReadyCircle, m_ReadyCircle, szDummyCircle, m_fCircleOffSet );

		if ( m_bSetNoMoveCircle )
		{
			pChar->CheckMagicCircleVisible( false );
			pChar->SetMagicCirclePos( false, pChar->GetMagicCirclePos() );
		}
	}

	m_vCirclePos = pChar->GetMagicCirclePos();

	if ( m_dwSkillLoopStartTime == 0 )
		m_dwSkillLoopStartTime = FRAMEGETTIME();
	m_dwFireStartTime = FRAMEGETTIME();
	m_dwNextFireStartTime = m_dwFireStartTime + m_dwFireGapTime;

	ioEntityGroup *pGrp = pChar->GetGroup();

	int iAniID = pGrp->GetAnimationIdx( m_szSkillLoopMotion );
	if( iAniID == -1 )	return;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetLoopAni( iAniID, FLOAT100, FLOAT1, FLOAT1/m_fSkillLoopAniRate );
}

void ioCircleMoveWeapon2Skill::SetEndState( ioBaseChar *pChar )
{
	m_State = SS_END;

	pChar->SetMagicCircleTarget( "" );
	pChar->DestroyMagicCircle();

	ioEntityGroup *pGrp = pChar->GetGroup();

	int iAniID = pGrp->GetAnimationIdx( m_AttackAttribute.m_AttackEndAnimation );
	if( iAniID == -1 )	return;

	float fTimeRate = m_AttackAttribute.m_fAttackEndAniRate;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwMotionStartTime = dwCurTime;
	m_dwMotionEndTime = m_dwMotionStartTime + pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
}

void ioCircleMoveWeapon2Skill::OnLoopState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	// 뱡향 변화 체크
	pChar->CheckChangeDirectionForMagicCircle();

	D3DXVECTOR3 vCirclePos = pChar->GetMagicCirclePos();
	D3DXVECTOR3 vCharPos = pChar->GetWorldPosition();
	D3DXVECTOR3 vDistance = vCirclePos - vCharPos;
	vDistance.y = 0;

	float fDistance = D3DXVec3Length( &vDistance );

	if ( m_fCircleMaxMoveRange > 0.0f && fDistance >= m_fCircleMaxMoveRange )
	{
		SetEndState( pChar );
		return;
	}

	D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	
	D3DXVECTOR3 vTargetPos = vCharPos;
	vTargetPos += vDir * fDistance;
	
	vDir *= m_fCircleMoveSpeed *g_FrameTimer.GetSecPerFrame();
	vTargetPos += vDir;

	float fMapHeight = pStage->GetMapHeight( vTargetPos.x, vTargetPos.z );
	if( fMapHeight > 0.0f )
		vTargetPos.y = fMapHeight;

	pChar->SetMagicCirclePos( false, vTargetPos );
	m_vCirclePos = vTargetPos;
}

void ioCircleMoveWeapon2Skill::ProcessRotate( ioBaseChar *pChar )
{
	if( !pChar ) return;

	D3DXVECTOR3 vCirclrPos = pChar->GetMagicCirclePos();
	D3DXVECTOR3 vOwnerPos = pChar->GetWorldPosition();
	D3DXVECTOR3 vDiff = vCirclrPos - vOwnerPos;
	float fLenth = D3DXVec3Length( &vDiff );
	if( m_fFirstTargetDistance == 0.0f )
	{
		m_fFirstTargetDistance = fLenth;
	}

	float fRotateRate = FLOAT1;
	if( m_fFirstTargetDistance !=  fLenth )	
	{		
		fRotateRate = m_fFirstTargetDistance / fLenth * m_fCircleSlowDownSpeedRate;
	}

	if( m_dwRotateTime <= 0 ) return;

	if( m_CurDirKey == ioUserKeyInput::DKI_NONE )
		return;

	float fFrameGap = g_FrameTimer.GetCurFrameGap();
	float fRate = fFrameGap / m_dwRotateTime;
	float fRotateAngle = 360.0f * fRate * fRotateRate;

	// 목표에 도달체크 및 처리
	float fYawD = 180.0f + 45.0f * ( m_CurDirKey - ioUserKeyInput::DKI_UP );
	fYawD = ioMath::ArrangeHead( fYawD );

	float fYawR = DEGtoRAD( fYawD );
	D3DXQUATERNION qtRotate;
	D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

	D3DXQUATERNION qtCurRot = pChar->GetTargetRot();

	float fCurYaw, fTargetYaw;
	fCurYaw = RADtoDEG( ioMath::QuaterToYaw( qtCurRot ) );
	fTargetYaw = RADtoDEG( ioMath::QuaterToYaw( qtRotate ) );

	fCurYaw = ioMath::ArrangeHead( fCurYaw );
	fTargetYaw = ioMath::ArrangeHead( fTargetYaw );

	float fRotAngle = ioMath::AngleGap( fCurYaw, fTargetYaw );
	if( fabs( fRotAngle ) < fRotateAngle )
	{
		m_bTargetRot = true;
		pChar->SetTargetRotToRotate( qtRotate, true, false );
		return;
	}

	if( m_bLeftRot )
		fRotateAngle *= -1;

	fRotateAngle = ioMath::ArrangeHead( fRotateAngle );
	D3DXVECTOR3 vDir = pChar->GetMoveDir();

	D3DXQUATERNION qtAngle;
	D3DXQuaternionRotationYawPitchRoll( &qtAngle, DEGtoRAD(fRotateAngle), 0.0f, 0.0f );

	vDir = qtAngle * vDir;
	ioMath::CalcDirectionQuaternion( qtAngle, -vDir );

	pChar->SetTargetRotToRotate( qtAngle, true, false );
}

void ioCircleMoveWeapon2Skill::CheckKeyInput( ioBaseChar *pOwner )
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
				kPacket << static_cast<BYTE>( SST_ROTATE );
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
				kPacket << static_cast<BYTE>( SST_ROTATE );
				kPacket << (int)m_CurDirKey;
				kPacket << m_bLeftRot;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
	}
}

void ioCircleMoveWeapon2Skill::AttackFire( ioBaseChar *pChar )
{
	if( m_dwFireStartTime <= 0 ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	//m_dwFireStartTime
	D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVECTOR3 vFirePos = pChar->GetWorldPosition();
	vFirePos.y += m_fFireHightOffset;
	vFirePos += ( vDir * m_fFireFrontOffset );

	int iAttCnt = m_AttackAttribute.m_vWeaponInfoList.size();
	if( m_dwNextFireStartTime < dwCurTime )
	{
		for( int i=0; i<iAttCnt; i++ )
		{
			CheckWeaponFloatInfo( pChar );			

			pChar->SkillFireExplicit( m_AttackAttribute.m_vWeaponInfoList[i],
									  GetName(),
									  m_dwWeaponBaseIndex + m_iCurFireCount,
									  vFirePos );

			m_iCurFireCount++;
		}		
		m_dwNextFireStartTime = dwCurTime + m_dwFireGapTime;
	}

	if( m_iCurFireCount >= m_iTotalFireCount*iAttCnt )
		SetEndState( pChar );
}


float ioCircleMoveWeapon2Skill::GetFlightDuration( float fCurGravity, float fCurWeaponFloatPower, float fGapHeight )
{
	float fTopTime, fTopHeight, fGapTime;
	fTopTime = fTopHeight = fGapTime = 0.0f;

	float fNewGravity = fCurGravity;
	if( fNewGravity > 0.0f )
	{
		fTopTime = fCurWeaponFloatPower / fNewGravity;
		//fTopHeight = (fCurWeaponFloatPower*fTopTime) - (0.5f*fNewGravity*fTopTime*fTopTime);
	}

	float fCurFloatPower = -fCurWeaponFloatPower;
	float fCurGapHeight = 0.0f;
	while( fCurGapHeight < fGapHeight )
	{
		fCurGapHeight += fabs(fCurFloatPower * 0.01f);
		fCurFloatPower -= fNewGravity * 0.01;
		fGapTime += 0.01;
	}

	float fResult = 2 * fTopTime * FLOAT1000;
	fResult += fGapTime * FLOAT1000;
	return fResult;
}

float ioCircleMoveWeapon2Skill::GetTargetTime( IN float fCurGravity, IN float fGapHeight, OUT float &fFloatPower )
{
	float fGapTime = 0.0f;
	float fCurFloatPower = 0;
	float fCurGapHeight = 0.0f;

	float fRevisionH = FLOAT100;
	float fTopHeight = fGapHeight + fRevisionH;

	// Top Check
	float fTopTime = 0.0f;
	do
	{
		fTopTime = fGapTime;

		fCurGapHeight += fabs(fCurFloatPower * 0.01f);
		fCurFloatPower += fCurGravity * 0.01;
		fGapTime += 0.01;
	} while( fCurGapHeight < fTopHeight );

	fFloatPower = fCurFloatPower;


	// Target Check
	fGapTime = 0.0f;
	fCurFloatPower = 0;
	fCurGapHeight = 0.0f;

	float fTargetTime = 0.18f;

	fTargetTime += fTopTime;
	fTargetTime *= FLOAT1000;
	return fTargetTime;
}

void ioCircleMoveWeapon2Skill::CheckWeaponFloatInfo( ioBaseChar *pChar )
{
	// CharDir
	D3DXVECTOR3 vCharPos = pChar->GetWorldPosition();
	D3DXVECTOR3 vCharDir = m_vCirclePos - vCharPos;
	vCharDir.y = 0.0f;

	float fFirstRange = D3DXVec3Length( &vCharDir );
	D3DXVec3Normalize( &vCharDir, &vCharDir );

	D3DXQUATERNION qtRot;
	ioMath::CalcDirectionQuaternion( qtRot, -vCharDir );
	pChar->SetTargetRotAndMoveDirChange( qtRot, false );
	pChar->SetWorldOrientation( qtRot );

	// WeaponMove
	m_iCurWeaponCnt = 0;

	IORandom random;
	DWORD dwSeed = 0;
	int iRand = 0;
	dwSeed = pChar->GetRandomSeed();
	random.SetRandomSeed( dwSeed + m_iCurFireCount );

	int iRandX = random.Random( m_iRandomRange*2 ) - m_iRandomRange;
	int iRandZ = random.Random( m_iRandomRange*2 ) - m_iRandomRange;

	D3DXVECTOR3 vTargetPos = m_vCirclePos;

	vTargetPos.x += (float)iRandX;
	vTargetPos.z += (float)iRandZ;

	float fGapDistance = m_fWeaponGapDistance;
	D3DXVECTOR3 vPos = vTargetPos + (vCharDir * fGapDistance);
	D3DXVECTOR3 vDir = vPos - vCharPos;
	vDir.y = 0.0f;

	float fRate = FLOAT1;
	float fRange = D3DXVec3Length( &vDir );
	D3DXVec3Normalize( &vDir, &vDir );

	float fCurWeaponFloatPower = 0.0f;
	float fCurWeaponMoveSpeed = 0.0f;
	if( fRange > 0.0f )
	{
		fRate = fRange / (m_fCircleRange+fGapDistance);

		// CurFloat
		fCurWeaponFloatPower = m_fWeaponFloatPower * fRate;
		fCurWeaponFloatPower = max( fCurWeaponFloatPower, m_fWeaponFloatMinPower );

		float fCurGravity = 0.0f;
		float fWeaponMoveSpeed = 0.0f;
		ioPlayStage *pStage = g_SkillMaker.GetPlayStage();
		if( pStage )
			fCurGravity = pStage->GetRoomGravity() * m_fWeaponGravityRate;

		float fFlightDuration = 0.0f;
		float fGapHeight = pChar->GetMidHeightByRate(m_fRevisionRate) - vCharPos.y;
		float fGapHeight2 = vCharPos.y - m_vCirclePos.y;
		if( fGapHeight2 > 0.0f )
		{
			fGapHeight += fGapHeight2;
			fFlightDuration = GetFlightDuration( fCurGravity, fCurWeaponFloatPower, fGapHeight );

			if( fFlightDuration > 0.0f )
				fWeaponMoveSpeed = (fRange+fGapDistance) / (fFlightDuration / FLOAT1000);

			fCurWeaponMoveSpeed = max( m_fWeaponMinSpeed, fWeaponMoveSpeed );
		}
		else if( COMPARE(fGapHeight2, -m_fUpLimitHeight, -20.0f) && fRange > m_fUpLimitRange )
		{
			float fNewGap = fabs( fGapHeight2 ) - fGapHeight;
			fFlightDuration = GetTargetTime( fCurGravity, fNewGap, fCurWeaponFloatPower );
			fRange -= 35.0f;

			if( fFlightDuration > 0.0f )
				fCurWeaponMoveSpeed = (fRange+fGapDistance) / (fFlightDuration / FLOAT1000);
		}
		else
		{
			fFlightDuration = GetFlightDuration( fCurGravity, fCurWeaponFloatPower, fGapHeight );

			if( fFlightDuration > 0.0f )
				fWeaponMoveSpeed = (fRange+fGapDistance) / (fFlightDuration / FLOAT1000);

			fCurWeaponMoveSpeed = max( m_fWeaponMinSpeed, fWeaponMoveSpeed );
		}


		m_vWeaponMoveInfo.m_vMoveDir = vDir;
		m_vWeaponMoveInfo.m_fMoveSpeed = fCurWeaponMoveSpeed;
		m_vWeaponMoveInfo.m_fFloatPower = fCurWeaponFloatPower;
	}
}

void ioCircleMoveWeapon2Skill::GenerateEndAttack( const D3DXVECTOR3 &vPos, ioBaseChar *pOwner )
{
	if( !pOwner )	return;

	m_dwWeaponBaseIndex = pOwner->IncreaseWeaponIndexBase();

	int iAttCnt = m_EndAttackAttribute.m_vWeaponInfoList.size();
	for( int i=0; i<iAttCnt; i++ )
	{
		pOwner->SkillFireExplicit( m_EndAttackAttribute.m_vWeaponInfoList[i],
								   GetName(),
								   m_dwWeaponBaseIndex+i,
								   vPos );
	}		

}

void ioCircleMoveWeapon2Skill::AttackFireByKeyInput( ioBaseChar *pChar )
{
	if ( !pChar || !pChar->IsNeedProcess() )
		return;

	if ( !pChar->IsAttackKey() )
		return;

	if ( m_dwAttackKeyInputTime == 0 )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_dwNextFireStartTime < dwCurTime )
	{
		m_dwNextFireStartTime = dwCurTime + m_dwAttackKeyInputTime;

		//Weapon Attack
		D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVECTOR3 vFirePos = pChar->GetWorldPosition();
		vFirePos.y += m_fFireHightOffset;
		vFirePos += ( vDir * m_fFireFrontOffset );

		int iSize = (int)m_AttackAttribute.m_vWeaponInfoList.size();
		for( int i=0; i<iSize; i++ )
		{
			CheckWeaponFloatInfo( pChar );			

			pChar->SkillFireExplicit( m_AttackAttribute.m_vWeaponInfoList[i],
				GetName(),
				m_dwWeaponBaseIndex + m_iCurFireCount,
				vFirePos );

			m_iCurFireCount++;
		}

		if( pChar->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
			kPacket << pChar->GetCharName();
			kPacket << GetName();
			kPacket << static_cast<BYTE>( SST_ATTACK_KEY_INPUT );
			kPacket << pChar->GetTargetRot();
			kPacket << m_vCirclePos;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}


void ioCircleMoveWeapon2Skill::SetFireState( ioBaseChar *pChar )
{
	if ( !pChar )
		return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_FireAttribute.m_AttackAnimation );
	if( iAniID == -1 )	
		return;

	m_State = SS_FIRE_ACTION;

	float fTimeRate  = m_FireAttribute.m_fAttackAniRate;
	DWORD dwPreDelay = m_FireAttribute.m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	m_dwMotionStartTime = m_dwMotionEndTime = m_dwFireStartTime = FRAMEGETTIME();
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;
	m_dwMotionEndTime += dwPreDelay + m_FireAttribute.m_dwEndDelay;

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, dwPreDelay, true );

	m_dwFireStartTime += dwPreDelay;
	m_dwFireStartTime += pGrp->GetAnimationEventTime_e( iAniID, "fire_s" ) * fTimeRate;

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwFireMotionEndTime = dwCurTime;
	m_dwFireMotionEndTime += dwPreDelay;
	m_dwFireMotionEndTime += pGrp->GetAnimationEventTime_e( iAniID, "firemotion_e" ) * fTimeRate;

	if( m_dwFireStartTime >= m_dwFireMotionEndTime )
		m_dwFireMotionEndTime = m_dwFireStartTime;

	SetExtraAniJump( pChar, iAniID, fTimeRate );

	// Col Skip
	pChar->CheckCharColSkipSkillTime( iAniID, fTimeRate, dwPreDelay );

	m_dwEnableReserveTime = dwCurTime;
	m_dwEnableReserveTime += pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" ) * fTimeRate;

	pChar->SetReservedSliding( m_FireAttribute.m_vForceInfoList, iAniID, fTimeRate, dwPreDelay );

	SetFireDummy( pChar );

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << static_cast<BYTE>( SST_FIRE_ACTION );
		kPacket << pChar->GetTargetRot();
		kPacket << m_vCirclePos;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioCircleMoveWeapon2Skill::CreateWeapon( ioBaseChar *pChar )
{
	D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVECTOR3 vFirePos = pChar->GetWorldPosition();
	vFirePos.y += m_fFireHightOffset;
	vFirePos += ( vDir * m_fFireFrontOffset );

	int iAttCnt = m_FireAttribute.m_vWeaponInfoList.size();
	for( int i=0; i<iAttCnt; i++ )
	{
		CheckWeaponFloatInfo( pChar );			

		pChar->SkillFireExplicit( m_FireAttribute.m_vWeaponInfoList[i],
			GetName(),
			m_dwWeaponBaseIndex + m_iCurFireCount,
			vFirePos );
		m_iCurFireCount++;
	}
}

void ioCircleMoveWeapon2Skill::CreateDummyChar( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( !pChar || !pChar->IsNeedProcess() ) 
		return;

	if( m_DummyCharList.empty() )
		return;

	g_DummyCharMgr.DestroyDummyCharByNameList( pChar, m_vDestroyDummyCharList );

	//
	m_CreateDummyList.clear();

	D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	D3DXVec3Normalize( &vDir, &vDir );

	D3DXVECTOR3 vSideDir;
	D3DXVec3Cross( &vSideDir, &ioMath::UNIT_Y, &vDir );
	D3DXVec3Normalize( &vSideDir, &vSideDir );

	int iSize = m_DummyCharList.size();
	for( int i=0; i < iSize; ++i )
	{
		ioHashString szDummyCharName = m_DummyCharList[i].m_DummyCharName;
		int iCurIndex = g_DummyCharMgr.GetCurLocalIndex();
		float fOffset = m_DummyCharList[i].m_fDummyCharOffset;
		float fSideOffset = m_DummyCharList[i].m_fDummyCharSideOffset;
		float fStartAngle = m_DummyCharList[i].m_fDummyCharStartAngle;

		D3DXVECTOR3 vTargetPos = pChar->GetWorldPosition() + (fOffset * vDir);
		vTargetPos += fSideOffset * vSideDir;

		ioDummyChar *pDummy = pStage->CreateDummyChar( szDummyCharName, iCurIndex,
			pChar->GetCharName(),
			vTargetPos,
			fStartAngle,
			0, true );

		if( pDummy )
			pDummy->SetControlSkillName( GetName() );

		DummyCharCreateInfo kInfo;
		kInfo.m_DummyCharName = szDummyCharName;
		kInfo.m_iDummyCharIdx = iCurIndex;
		kInfo.m_fDummyCharStartAngle = fStartAngle;
		kInfo.m_vTargetPos = vTargetPos;

		m_CreateDummyList.push_back( kInfo );
	}

	int iCreateCnt = m_CreateDummyList.size();
	if( pChar->IsNeedSendNetwork() && iCreateCnt > 0 )
	{
		SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
		kPacket << pChar->GetCharName();
		kPacket << GetName();
		kPacket << static_cast<BYTE>( SST_CREAT_DUMMY );
		kPacket << pChar->GetTargetRot();

		kPacket << iCreateCnt;

		for( int i=0; i < iCreateCnt; ++i )
		{
			kPacket << m_CreateDummyList[i].m_DummyCharName;
			kPacket << m_CreateDummyList[i].m_iDummyCharIdx;
			kPacket << m_CreateDummyList[i].m_fDummyCharStartAngle;
			kPacket << m_CreateDummyList[i].m_vTargetPos;
		}
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioCircleMoveWeapon2Skill::ProcessDummyRotate( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if ( !m_bDummyLookAtCircle )
		return;
	if ( m_CreateDummyList.empty() )
		return;

	int iSize = m_CreateDummyList.size();
	for( int i=0; i<iSize; ++i )
	{
		ioDummyChar *pDummy = g_DummyCharMgr.FindDummyChar( pChar, m_CreateDummyList[i].m_iDummyCharIdx );
		if( pDummy )
			pDummy->SetWorldOrientation( pChar->GetWorldOrientation() );
	}
}

void ioCircleMoveWeapon2Skill::SetFireDummy( ioBaseChar *pChar )
{
	if ( m_CreateDummyList.empty() )
		return;

	int iSize = m_CreateDummyList.size();
	for( int i=0; i<iSize; ++i )
	{
		ioDummyChar *pDummy = g_DummyCharMgr.FindDummyChar( pChar, m_CreateDummyList[i].m_iDummyCharIdx );
		if( pDummy )
		{
			pDummy->SetWorldOrientation( pChar->GetWorldOrientation() );
			ioNoneTraceDummy2* pNoneTraceDummy2 = ToNoneTraceDummy2( pDummy );
			if ( pNoneTraceDummy2 )
			{
				pNoneTraceDummy2->SetCirclePos( m_vCirclePos );
				pNoneTraceDummy2->SetAttackState();
			}
		}
	}
}
