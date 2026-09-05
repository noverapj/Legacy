

#include "stdafx.h"

#include "ioBaseChar.h"
#include "ioGrowthUpInfo.h"

#include "ioMultiSkill.h"

ioMultiSkill::ioMultiSkill()
{
	m_dwPreDelayStartTime = 0;
	m_dwFireMotionEndTime = 0;
	m_dwAttackEndTime = 0;

	m_fExtraAttackMoveForce = 0.0f;
	m_fExtraAttackMoveFriction = 0.0f;

	m_bUseExtendSkill = false;
	m_bRefreshFireTimeList = true;

	InitExtraAniJump();

	m_bSetHandMesh = false;

	m_SkillState = SS_NONE;
	m_dwCommandAniStartTime = 0;
}

ioMultiSkill::ioMultiSkill( const ioMultiSkill &rhs )
: ioSkill( rhs ),
  m_ExtendSkillMotionName( rhs.m_ExtendSkillMotionName ),
  m_fExtendAnimationTimeRate( rhs.m_fExtendAnimationTimeRate ),
  m_ExtendDownSkillMotionName( rhs.m_ExtendDownSkillMotionName ),
  m_fExtendDownAnimationTimeRate( rhs.m_fExtendDownAnimationTimeRate ),
  m_ExtendJumpSkillMotionName( rhs.m_ExtendJumpSkillMotionName ),
  m_fExtendJumpAnimationTimeRate( rhs.m_fExtendJumpAnimationTimeRate ),
  m_AttackAttribute( rhs.m_AttackAttribute ),
  m_DownAttackAttribute( rhs.m_DownAttackAttribute ),
  m_JumpAttackAttribute( rhs.m_JumpAttackAttribute ),
  m_ExtendAttackAttribute( rhs.m_ExtendAttackAttribute ),
  m_ExtendDownAttackAttribute( rhs.m_ExtendDownAttackAttribute ),
  m_ExtendJumpAttackAttribute( rhs.m_ExtendJumpAttackAttribute ),
  m_dwCommandTime( rhs.m_dwCommandTime ),
  m_dwDownCommandTime( rhs.m_dwDownCommandTime ),
  m_dwJumpCommandTime( rhs.m_dwJumpCommandTime ),
  m_CommandAnimation( rhs.m_CommandAnimation ),
  m_dwCommandAniTime( rhs.m_dwCommandAniTime ),
  m_DownCommandAnimation( rhs.m_DownCommandAnimation ),
  m_dwDownCommandAniTime( rhs.m_dwDownCommandAniTime ),
  m_JumpCommandAnimation( rhs.m_JumpCommandAnimation ),
  m_dwJumpCommandAniTime( rhs.m_dwJumpCommandAniTime ),
  m_fMultiSkillGravityRate( rhs.m_fMultiSkillGravityRate )
{
	m_dwPreDelayStartTime = 0;
	m_dwFireMotionEndTime = 0;
	m_dwAttackEndTime = 0;

	m_fExtraAttackMoveForce = 0.0f;
	m_fExtraAttackMoveFriction = 0.0f;

	m_bUseExtendSkill = false;
	m_bRefreshFireTimeList = true;

	InitExtraAniJump();

	m_bSetHandMesh = false;

	m_SkillState = SS_NONE;
	m_dwCommandAniStartTime = 0;
}

ioMultiSkill::~ioMultiSkill()
{
}

void ioMultiSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];

	m_fMultiSkillGravityRate = rkLoader.LoadFloat_e( "multi_skill_gravity_rate", FLOAT1 );

	rkLoader.LoadString_e( "command_motion", "", szBuf, MAX_PATH );
	m_CommandAnimation = szBuf;
	m_dwCommandAniTime = rkLoader.LoadInt_e( "command_motion_time", 0 );

	rkLoader.LoadString_e( "down_command_motion", "", szBuf, MAX_PATH );
	m_DownCommandAnimation = szBuf;
	m_dwDownCommandAniTime = rkLoader.LoadInt_e( "down_command_motion_time", 0 );

	rkLoader.LoadString_e( "jump_command_motion", "", szBuf, MAX_PATH );
	m_JumpCommandAnimation = szBuf;
	m_dwJumpCommandAniTime = rkLoader.LoadInt_e( "jump_command_motion_time", 0 );

	LoadNormalAttribute( rkLoader );
	LoadDownAttribute( rkLoader );
	LoadJumpAttribute( rkLoader );
}

void ioMultiSkill::LoadNormalAttribute( ioINILoader &rkLoader )
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

		m_AttackAttribute.m_vForceInfoList.push_back( kInfo );
	}

	int iAttackTypeCnt = rkLoader.LoadInt_e( "attack_type_count", 1 );
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


	// Extend
	m_dwCommandTime = rkLoader.LoadInt_e( "command_enable_time", 0 );

	rkLoader.LoadString_e( "extend_skill_motion", "", szBuf, MAX_PATH );
	m_ExtendSkillMotionName = szBuf;
	m_fExtendAnimationTimeRate = rkLoader.LoadFloat_e( "extend_animation_time_rate", FLOAT1 );

	m_ExtendAttackAttribute.m_AttackAnimation = m_ExtendSkillMotionName;
	m_ExtendAttackAttribute.m_fAttackAniRate  = m_fExtendAnimationTimeRate;

	rkLoader.LoadString_e( "extend_skill_end_motion", "", szBuf, MAX_PATH );

	m_ExtendAttackAttribute.m_AttackEndAnimation = szBuf;
	m_ExtendAttackAttribute.m_fAttackEndAniRate  = rkLoader.LoadFloat_e( "extend_skill_end_time_rate", FLOAT1 );

	m_ExtendAttackAttribute.m_dwPreDelay	    = rkLoader.LoadInt_e( "extend_animation_pre_delay", 0 );
	m_ExtendAttackAttribute.m_dwEndDelay	    = rkLoader.LoadInt_e( "extend_animation_end_delay", 0 );

	m_ExtendAttackAttribute.m_vForceInfoList.clear();
	iForceCnt = rkLoader.LoadInt_e( "extend_attack_force_cnt", 0 );
	if( iForceCnt > 0 )
		m_ExtendAttackAttribute.m_vForceInfoList.reserve( iForceCnt );

	for( int i=0; i < iForceCnt; ++i )
	{
		ForceInfo kInfo;

		wsprintf_e( szKey, "extend_attack_force_move%d", i+1 );
		kInfo.m_fForceMovePower = rkLoader.LoadFloat( szKey, 0.0f );

		wsprintf_e( szKey, "extend_attack_force_friction%d", i+1 );
		kInfo.m_fForceMoveFriction = rkLoader.LoadFloat( szKey, FLOAT1 );

		m_ExtendAttackAttribute.m_vForceInfoList.push_back( kInfo );
	}

	iAttackTypeCnt = rkLoader.LoadInt_e( "extend_attack_type_count", 1 );
	m_ExtendAttackAttribute.m_vWeaponInfoList.clear();
	m_ExtendAttackAttribute.m_vWeaponInfoList.reserve( iAttackTypeCnt );

	for( int i=0 ; i<iAttackTypeCnt ; i++ )
	{
		WeaponInfo kInfo;

		wsprintf_e( szKey, "extend_attack_type%d", i+1 );
		kInfo.m_iWeaponIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "extend_attack_type%d_resist", i+1 );
		kInfo.m_iResistanceIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "extend_attack_type%d_wounded", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_WoundedAnimation = szBuf;

		wsprintf_e( szKey, "extend_attack_type%d_wounded_time", i+1 );
		kInfo.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "extend_attack_type%d_wounded_loop_ani", i+1 );
		kInfo.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );

		m_ExtendAttackAttribute.m_vWeaponInfoList.push_back( kInfo );
	}
}

void ioMultiSkill::LoadDownAttribute( ioINILoader &rkLoader )
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


	// Extend
	m_dwDownCommandTime = rkLoader.LoadInt_e( "down_command_enable_time", 0 );

	rkLoader.LoadString_e( "extend_down_skill_motion", "", szBuf, MAX_PATH );
	m_ExtendDownSkillMotionName = szBuf;
	m_fExtendDownAnimationTimeRate = rkLoader.LoadFloat_e( "extend_down_animation_time_rate", FLOAT1 );

	m_ExtendDownAttackAttribute.m_AttackAnimation = m_ExtendDownSkillMotionName;
	m_ExtendDownAttackAttribute.m_fAttackAniRate  = m_fExtendDownAnimationTimeRate;

	rkLoader.LoadString_e( "extend_down_skill_end_motion", "", szBuf, MAX_PATH );

	m_ExtendDownAttackAttribute.m_AttackEndAnimation = szBuf;
	m_ExtendDownAttackAttribute.m_fAttackEndAniRate  = rkLoader.LoadFloat_e( "extend_down_skill_end_time_rate", FLOAT1 );

	m_ExtendDownAttackAttribute.m_dwPreDelay = rkLoader.LoadInt_e( "extend_down_animation_pre_delay", 0 );
	m_ExtendDownAttackAttribute.m_dwEndDelay = rkLoader.LoadInt_e( "extend_down_animation_end_delay", 0 );

	m_ExtendDownAttackAttribute.m_vForceInfoList.clear();
	iForceCnt = rkLoader.LoadInt_e( "extend_down_attack_force_cnt", 0 );
	if( iForceCnt > 0 )
		m_ExtendDownAttackAttribute.m_vForceInfoList.reserve( iForceCnt );

	for( int i=0; i < iForceCnt; ++i )
	{
		ForceInfo kInfo;

		wsprintf_e( szKey, "extend_down_attack_force_move%d", i+1 );
		kInfo.m_fForceMovePower = rkLoader.LoadFloat( szKey, 0.0f );

		wsprintf_e( szKey, "extend_down_attack_force_friction%d", i+1 );
		kInfo.m_fForceMoveFriction = rkLoader.LoadFloat( szKey, FLOAT1 );

		m_ExtendDownAttackAttribute.m_vForceInfoList.push_back( kInfo );
	}

	iAttackTypeCnt = rkLoader.LoadInt_e( "extend_down_attack_type_count", 1 );
	m_ExtendDownAttackAttribute.m_vWeaponInfoList.clear();
	m_ExtendDownAttackAttribute.m_vWeaponInfoList.reserve( iAttackTypeCnt );

	for( int i=0 ; i<iAttackTypeCnt ; i++ )
	{
		WeaponInfo kInfo;

		wsprintf_e( szKey, "extend_down_attack_type%d", i+1 );
		kInfo.m_iWeaponIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "extend_down_attack_type%d_resist", i+1 );
		kInfo.m_iResistanceIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "extend_down_attack_type%d_wounded", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_WoundedAnimation = szBuf;

		wsprintf_e( szKey, "extend_down_attack_type%d_wounded_time", i+1 );
		kInfo.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "extend_down_attack_type%d_wounded_loop_ani", i+1 );
		kInfo.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );

		m_ExtendDownAttackAttribute.m_vWeaponInfoList.push_back( kInfo );
	}
}

void ioMultiSkill::LoadJumpAttribute( ioINILoader &rkLoader )
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


	// Extend
	m_dwJumpCommandTime = rkLoader.LoadInt_e( "jump_command_enable_time", 0 );

	rkLoader.LoadString_e( "extend_jump_skill_motion", "", szBuf, MAX_PATH );
	m_ExtendJumpSkillMotionName = szBuf;
	m_fExtendJumpAnimationTimeRate = rkLoader.LoadFloat_e( "extend_jump_animation_time_rate", FLOAT1 );

	m_ExtendJumpAttackAttribute.m_AttackAnimation = m_ExtendJumpSkillMotionName;
	m_ExtendJumpAttackAttribute.m_fAttackAniRate  = m_fExtendJumpAnimationTimeRate;

	rkLoader.LoadString_e( "extend_jump_skill_end_motion", "", szBuf, MAX_PATH );

	m_ExtendJumpAttackAttribute.m_AttackEndAnimation = szBuf;
	m_ExtendJumpAttackAttribute.m_fAttackEndAniRate  = rkLoader.LoadFloat_e( "extend_jump_skill_end_time_rate", FLOAT1 );

	m_ExtendJumpAttackAttribute.m_dwPreDelay = rkLoader.LoadInt_e( "extend_jump_animation_pre_delay", 0 );
	m_ExtendJumpAttackAttribute.m_dwEndDelay = rkLoader.LoadInt_e( "extend_jump_animation_end_delay", 0 );

	m_ExtendJumpAttackAttribute.m_vForceInfoList.clear();
	iForceCnt = rkLoader.LoadInt_e( "extend_jump_attack_force_cnt", 0 );
	if( iForceCnt > 0 )
		m_ExtendJumpAttackAttribute.m_vForceInfoList.reserve( iForceCnt );

	for( int i=0; i < iForceCnt; ++i )
	{
		ForceInfo kInfo;

		wsprintf_e( szKey, "extend_jump_attack_force_move%d", i+1 );
		kInfo.m_fForceMovePower = rkLoader.LoadFloat( szKey, 0.0f );

		wsprintf_e( szKey, "extend_jump_attack_force_friction%d", i+1 );
		kInfo.m_fForceMoveFriction = rkLoader.LoadFloat( szKey, FLOAT1 );

		m_ExtendJumpAttackAttribute.m_vForceInfoList.push_back( kInfo );
	}

	iAttackTypeCnt = rkLoader.LoadInt_e( "extend_jump_attack_type_count", 1 );
	m_ExtendJumpAttackAttribute.m_vWeaponInfoList.clear();
	m_ExtendJumpAttackAttribute.m_vWeaponInfoList.reserve( iAttackTypeCnt );

	for( int i=0 ; i<iAttackTypeCnt ; i++ )
	{
		WeaponInfo kInfo;

		wsprintf_e( szKey, "extend_jump_attack_type%d", i+1 );
		kInfo.m_iWeaponIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "extend_jump_attack_type%d_resist", i+1 );
		kInfo.m_iResistanceIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "extend_jump_attack_type%d_wounded", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_WoundedAnimation = szBuf;

		wsprintf_e( szKey, "extend_jump_attack_type%d_wounded_time", i+1 );
		kInfo.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "extend_jump_attack_type%d_wounded_loop_ani", i+1 );
		kInfo.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );

		m_ExtendJumpAttackAttribute.m_vWeaponInfoList.push_back( kInfo );
	}
}

const ioHashString& ioMultiSkill::GetSkillMotionName() const
{
	switch( m_SkillUseType )
	{
	case SUT_NORMAL:
		if( m_bUseExtendSkill )
			return m_ExtendSkillMotionName;

		return m_SkillMotionName;
	case SUT_DOWN:
		{
			if( m_bUseExtendSkill )
			{
				if( !m_ExtendDownSkillMotionName.IsEmpty() )
					return m_ExtendDownSkillMotionName;
			}
			else
			{
				if( !m_DownSkillMotionName.IsEmpty() )
					return m_DownSkillMotionName;
			}
		}
		return m_SkillMotionName;
	case SUT_JUMP:
		{
			if( m_bUseExtendSkill )
			{
				if( !m_ExtendJumpSkillMotionName.IsEmpty() )
					return m_ExtendJumpSkillMotionName;
			}
			else
			{
				if( !m_JumpSkillMotionName.IsEmpty() )
					return m_JumpSkillMotionName;
			}
		}
		return m_SkillMotionName;
	}

	return m_SkillMotionName;
}

bool ioMultiSkill::HasSkillMotion() const
{
	return !GetSkillMotionName().IsEmpty();
}

float ioMultiSkill::GetSkillMotionRate() const
{
	switch( m_SkillUseType )
	{
	case SUT_NORMAL:
		if( m_bUseExtendSkill )
			return m_fExtendAnimationTimeRate;

		return m_fAnimationTimeRate;
	case SUT_DOWN:
		{
			if( m_bUseExtendSkill )
			{
				if( !m_ExtendDownSkillMotionName.IsEmpty() )
					return m_fExtendDownAnimationTimeRate;
			}
			else
			{
				if( !m_DownSkillMotionName.IsEmpty() )
					return m_fDownAnimationTimeRate;
			}
		}
		return m_fAnimationTimeRate;
	case SUT_JUMP:
		{
			if( m_bUseExtendSkill )
			{
				if( !m_ExtendJumpSkillMotionName.IsEmpty() )
					return m_fExtendJumpAnimationTimeRate;
			}
			else
			{
				if( !m_JumpSkillMotionName.IsEmpty() )
					return m_fJumpAnimationTimeRate;
			}
		}
		return m_fAnimationTimeRate;
	}

	return m_fAnimationTimeRate;
}

AttackAttribute& ioMultiSkill::GetAttribute()
{
	switch( m_SkillUseType )
	{
	case SUT_NORMAL:
		if( m_bUseExtendSkill )
			return m_ExtendAttackAttribute;
		return m_AttackAttribute;
	case SUT_DOWN:
		{
			if( m_bUseExtendSkill )
			{
				if( !m_ExtendDownAttackAttribute.m_AttackAnimation.IsEmpty() )
					return m_ExtendDownAttackAttribute;
			}
			else
			{
				if( !m_DownAttackAttribute.m_AttackAnimation.IsEmpty() )
					return m_DownAttackAttribute;
			}
		}
		return m_AttackAttribute;
	case SUT_JUMP:
		{
			if( m_bUseExtendSkill )
			{
				if( !m_ExtendJumpAttackAttribute.m_AttackAnimation.IsEmpty() )
					return m_ExtendJumpAttackAttribute;
			}
			else
			{
				if( !m_JumpAttackAttribute.m_AttackAnimation.IsEmpty() )
					return m_JumpAttackAttribute;
			}
		}
		return m_AttackAttribute;
	}

	return m_AttackAttribute;
}

const AttackAttribute& ioMultiSkill::GetAttributeConst() const
{
	switch( m_SkillUseType )
	{
	case SUT_NORMAL:
		if( m_bUseExtendSkill )
			return m_ExtendAttackAttribute;
		return m_AttackAttribute;
	case SUT_DOWN:
		{
			if( m_bUseExtendSkill )
			{
				if( !m_ExtendDownAttackAttribute.m_AttackAnimation.IsEmpty() )
					return m_ExtendDownAttackAttribute;
			}
			else
			{
				if( !m_DownAttackAttribute.m_AttackAnimation.IsEmpty() )
					return m_DownAttackAttribute;
			}
		}
		return m_AttackAttribute;
	case SUT_JUMP:
		{
			if( m_bUseExtendSkill )
			{
				if( !m_ExtendJumpAttackAttribute.m_AttackAnimation.IsEmpty() )
					return m_ExtendJumpAttackAttribute;
			}
			else
			{
				if( !m_JumpAttackAttribute.m_AttackAnimation.IsEmpty() )
					return m_JumpAttackAttribute;
			}
		}
		return m_AttackAttribute;
	}

	return m_AttackAttribute;
}

const ioHashString& ioMultiSkill::GetCommandAnimation() const
{
	switch( m_SkillUseType )
	{
	case SUT_NORMAL:
		return m_CommandAnimation;
	case SUT_DOWN:
		{
			if( !m_DownCommandAnimation.IsEmpty() )
				return m_DownCommandAnimation;
		}
		return m_CommandAnimation;
	case SUT_JUMP:
		{
			if( !m_JumpCommandAnimation.IsEmpty() )
				return m_JumpCommandAnimation;
		}
		return m_CommandAnimation;
	}

	return m_CommandAnimation;
}

DWORD ioMultiSkill::GetCommandAniTime() const
{
	switch( m_SkillUseType )
	{
	case SUT_NORMAL:
		return m_dwCommandAniTime;
	case SUT_DOWN:
		return m_dwDownCommandAniTime;
	case SUT_JUMP:
		return m_dwJumpCommandAniTime;
	}

	return m_dwCommandTime;
}

DWORD ioMultiSkill::GetCommandTime() const
{
	switch( m_SkillUseType )
	{
	case SUT_NORMAL:
		return m_dwCommandTime;
	case SUT_DOWN:
		return m_dwDownCommandTime;
	case SUT_JUMP:
		return m_dwJumpCommandTime;
	}

	return m_dwCommandTime;
}

ioSkill* ioMultiSkill::Clone()
{
	return new ioMultiSkill( *this );
}

int ioMultiSkill::GetTypeID() const
{
	return ST_MULTI;
}

bool ioMultiSkill::IsCanUseSkill( ioBaseChar *pChar, bool bHelpMsg )
{
	ioAttackableItem *pAttackable = ToAttackableItem( GetOwnerItem() );
	if( pAttackable )
	{
		ioHashString szSkillMotion = GetSkillMotionName();

		int iFireCnt = pChar->GetAnimationEventCnt( szSkillMotion, "fire_s" );
		if( !pAttackable->IsCanFire( pChar ) )
			return false;
	}

	if( !CheckNeedGauge( pChar, bHelpMsg ) )
		return false;

	return true;
}

bool ioMultiSkill::OnSkillStart( ioBaseChar *pChar )
{
	if ( m_bPetForceHide )
	{
		ioPet *pPet = g_PetMgr.FindPet( pChar );
		if ( pPet )
			pPet->SetForceHide( true );
	}

	m_SkillState = SS_NONE;

	pChar->SetSkillProtection();
	
	m_dwAttackEndTime = 0;

	InitExtraAniJump();

	m_dwAniRotateTime = 0;
	m_fAniRotateAngle = 0.0f;

	m_iCurAniRotate = 0;

	m_bUseExtendSkill = false;
	m_dwCommandAniStartTime = 0;

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

	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();
	if( !StartCommandAnimation( pChar ) )
	{
		LOG.PrintTimeAndLog( 0, "ioMultiSkill::OnSkillStart is Not Command motion" );
		return false;
	}
	else
	{
		DWORD dwTrackingTime = 0;
		ioEntityGroup *pGrp = pChar->GetGroup();
		if( pGrp )
		{
			int iAniID = pGrp->GetAnimationIdx( GetCommandAnimation() );
			if( iAniID != -1 )
				dwTrackingTime = pGrp->GetAnimationEventTime_e( iAniID, "tracking_e" );
		}

		DWORD dwPreDelay = GetPreDelayTime();

		if( dwTrackingTime > 0 )
			dwTrackingTime += FRAMEGETTIME();
		else
			dwTrackingTime = FRAMEGETTIME() + m_dwCommandTime;

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

	m_bSetHandMesh = false;

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	SetSkillNameChatBubble( pChar );
	
	return true;
}

void ioMultiSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_SkillState )
	{
	case SS_COMMAND:
		{
			if( pChar && pChar->IsNeedProcess() && m_dwCommandAniStartTime + GetCommandTime() < dwCurTime )
			{
				// command check
				if( pChar->IsAttackKey() )
				{
					if( !StartPreDelayAnimation( pChar ) )
						AttackSkillFire( pChar );

					if( pChar->IsNeedSendNetwork() )
					{

						SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
						kPacket << pChar->GetCharName();
						kPacket << GetName();
						kPacket << m_bUseExtendSkill;
						P2PNetwork::SendToAllPlayingUser( kPacket );
					}

					return;
				}
				else if( pChar->IsDefenseKey() )
				{
					m_bUseExtendSkill = true;

					if( !StartPreDelayAnimation( pChar ) )
						AttackSkillFire( pChar );

					if( pChar->IsNeedSendNetwork() )
					{
						SP2Packet kPacket( CUPK_SKILL_EXTRA_INFO );
						kPacket << pChar->GetCharName();
						kPacket << GetName();
						kPacket << m_bUseExtendSkill;
						P2PNetwork::SendToAllPlayingUser( kPacket );
					}

					return;
				}
			}

			DWORD dwCommandAniTime = GetCommandAniTime();
			if( m_dwCommandAniStartTime + dwCommandAniTime < dwCurTime )
			{
				if( !StartPreDelayAnimation( pChar ) )
				{
					AttackSkillFire( pChar );
				}
			}
		}
		break;
	case SS_PREDELAY:
		{
			DWORD dwPreDelay = GetPreDelayTime();

			if( m_dwPreDelayStartTime + dwPreDelay < dwCurTime )
			{
				m_dwPreDelayStartTime = 0;

				AttackSkillFire( pChar );
				EndPreDelayEffect( pChar );
			}
		}
		break;
	case SS_ACTION:
		{
			CheckExtraAniJump( pChar );
			CheckAniRotate( pChar );
		}
		break;
	}
}

void ioMultiSkill::OnSkillEnd( ioBaseChar *pChar )
{
	RemoveExtraAttackForce();
	EndPreDelayEffect( pChar );

	pChar->EndEffect( m_SkillEffectName );

	m_bUseExtendSkill = false;
	m_bReduceGauge = false;
	pChar->SetMoveDirByRotate( pChar->GetTargetRot() );
	pChar->RestoreSkillProtection();

	m_SkillUseType = SUT_NORMAL;
	
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

	m_SkillState = SS_NONE;

	if( !m_SkillEndEffectName.IsEmpty() )
		pChar->AttachEffect( m_SkillEndEffectName );

	if ( m_bPetForceHide )
	{
		ioPet *pPet = g_PetMgr.FindPet( pChar );
		if ( pPet )
			pPet->SetForceHide( false );
	}
}

bool ioMultiSkill::StartCommandAnimation( ioBaseChar *pChar )
{
	m_dwPreDelayStartTime = 0;

	if( GetCommandAnimation().IsEmpty() )
		return false;

	ioEntityGroup *pGrp = pChar->GetGroup();
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetLoopAni( GetCommandAnimation(), FLOAT100 );

	m_dwCommandAniStartTime = FRAMEGETTIME();
	m_SkillState = SS_COMMAND;

	return true;
}

bool ioMultiSkill::StartPreDelayAnimation( ioBaseChar *pChar )
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

	m_SkillState = SS_PREDELAY;

	return true;
}

void ioMultiSkill::AttackSkillFire( ioBaseChar *pChar )
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

	m_dwAttackEndTime = pGrp->GetAnimationEventTime_e( iAniID, "attack_end" ) * fTimeRate;
	if( m_dwAttackEndTime > 0 )
		m_dwAttackEndTime += dwCurTime;
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
	pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
	m_bReduceGauge = true;

	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}

	m_SkillState = SS_ACTION;
}

void ioMultiSkill::StartPreDelayEffect( ioBaseChar *pChar )
{
	ioSkill::StartPreDelayEffect( pChar );

	if( GetOwnerItem() )
	{
		GetOwnerItem()->AddGlowLine( pChar );
	}
}

void ioMultiSkill::EndPreDelayEffect( ioBaseChar *pChar )
{
	ioSkill::EndPreDelayEffect( pChar );

	if( GetOwnerItem() )
	{
		GetOwnerItem()->RemoveGlowLine( pChar );
	}
}

void ioMultiSkill::EnableRefreshFireTimeList( bool bRefresh )
{
	m_bRefreshFireTimeList = bRefresh;
}

float ioMultiSkill::GetAttackTimeRate() const
{
	float fTimeRate = GetAttributeConst().m_fAttackAniRate * m_fExtraAniRate;
	return fTimeRate;
}

float ioMultiSkill::GetAttackEndTimeRate() const
{
	return GetAttributeConst().m_fAttackEndAniRate;
}

void ioMultiSkill::ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket )
{
	bool bUseExtend;
	rkPacket >> bUseExtend;

	switch( m_SkillState )
	{
	case SS_COMMAND:
		m_bUseExtendSkill = bUseExtend;
		if( !StartPreDelayAnimation( pChar ) )
		{
			AttackSkillFire( pChar );
		}
		break;
	case SS_PREDELAY:
		m_bUseExtendSkill = bUseExtend;
		break;
	}
}

bool ioMultiSkill::IsUseAutoTargetValue()
{
	return ioSkill::IsUseAutoTargetValue();
}

void ioMultiSkill::SetExtraAttackForce()
{
	ForceInfoList::iterator iter = GetAttribute().m_vForceInfoList.begin();

	while( iter != GetAttribute().m_vForceInfoList.end() )
	{
		(*iter).m_fForceMovePower += m_fExtraAttackMoveForce;
		(*iter).m_fForceMoveFriction += m_fExtraAttackMoveFriction;

		++iter;
	}
}

void ioMultiSkill::RemoveExtraAttackForce()
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

bool ioMultiSkill::IsAttackEndState() const
{
	if( m_dwAttackEndTime > 0 && m_dwAttackEndTime < FRAMEGETTIME() )
	{
		return true;
	}

	return false;
}

bool ioMultiSkill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

bool ioMultiSkill::IsPreDelayTime()
{
	if( m_SkillState == SS_COMMAND || m_SkillState == SS_PREDELAY )
		return true;

	return false;
}

bool ioMultiSkill::IsProtected( int iDefenseBreakType ) const
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

	if( m_SkillState == SS_COMMAND || m_SkillState == SS_PREDELAY )
		return true;

	if( m_SkillState == SS_ACTION )
	{
		if( m_dwFireMotionEndTime > FRAMEGETTIME() )
			return true;
	}

	return false;
}

bool ioMultiSkill::IsPreDelayProtect() const
{
	return true;
}

bool ioMultiSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_SkillState != SS_ACTION )
		return false;

	if( m_dwMotionEndTime + dwActionStop > FRAMEGETTIME() )
		return false;

	return true;
}

bool ioMultiSkill::IsEnableReserve() const
{
	if( m_SkillState != SS_ACTION )
		return false;

	if( m_dwEnableReserveTime == 0 || m_dwMotionStartTime >= m_dwEnableReserveTime )
		return false;

	if( m_dwEnableReserveTime < FRAMEGETTIME() )
		return true;

	return false;
}

float ioMultiSkill::GetSkillGravityRate()
{
	if( m_SkillUseType == SUT_JUMP && IsPreDelayTime() )
		return m_fMultiSkillGravityRate;

	return m_fSkillGravityRate;
}
