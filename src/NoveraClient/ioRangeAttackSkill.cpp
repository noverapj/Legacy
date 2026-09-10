

#include "stdafx.h"

#include "ioRangeAttackSkill.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"


#include "WeaponDefine.h"

ioRangeAttackSkill::ioRangeAttackSkill()
{
	m_dwFireMotionEndTime = 0;
	m_MapEffect = -1;
}

ioRangeAttackSkill::ioRangeAttackSkill( const ioRangeAttackSkill &rhs )
: ioRangeSkill( rhs ),
  m_AttackAttribute( rhs.m_AttackAttribute ),
  m_DownAttackAttribute( rhs.m_DownAttackAttribute ),
  m_JumpAttackAttribute( rhs.m_JumpAttackAttribute ),
  m_bOwnerAttached( rhs.m_bOwnerAttached ),
  m_bOwnerBuffAction( rhs.m_bOwnerBuffAction )
{
	m_dwFireMotionEndTime = 0;
	m_MapEffect = -1;
}

ioRangeAttackSkill::~ioRangeAttackSkill()
{
}

void ioRangeAttackSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioRangeSkill::LoadProperty( rkLoader );

	m_bOwnerAttached = rkLoader.LoadBool_e( "owner_attached", false );
	m_bOwnerBuffAction = rkLoader.LoadBool_e( "use_owner_buff_action", false );

	LoadNormalAttribute( rkLoader );
	LoadDownAttribute( rkLoader );
	LoadJumpAttribute( rkLoader );
}

ioSkill* ioRangeAttackSkill::Clone()
{
	return new ioRangeAttackSkill( *this );
}

bool ioRangeAttackSkill::OnSkillStart( ioBaseChar *pChar )
{
	if( !HasSkillMotion() )
		return false;

	m_dwEnableSwitchTime = 0;
	m_dwEnableSemiSwitchTime = 0;

	if( m_dwSwitchSkill > 0 )
		m_dwEnableSwitchTime = FRAMEGETTIME() + m_dwSwitchSkill;
	if( m_dwSemiSwitchSkill > 0 )
		m_dwEnableSemiSwitchTime = FRAMEGETTIME() + m_dwSemiSwitchSkill;

	m_dwSkillStartTime = FRAMEGETTIME();

	m_vCollisionedList.clear();
	m_vBlockedList.clear();

	pChar->SetSkillProtection();

	m_RangeState = RS_NONE;
	m_bSetHandMesh = false;

	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	if( !StartPreDelayAnimation( pChar ) )	// Delay Animation이 없다
	{
		m_dwPreDelayStartTime = 0;
		EndPreDelayEffect( pChar );
		SetActionState( pChar );
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

		DWORD dwPreDelayTime = GetPreDelayTime();

		if( dwTrackingTime > 0 )
			dwTrackingTime += FRAMEGETTIME();
		else
			dwTrackingTime = FRAMEGETTIME() + dwPreDelayTime;

		if( IsUseAutoTargetValue() )
		{
			pChar->SetAutoTarget( ATT_SKILL );
			pChar->SetAutoTargetTrackingEndTime( dwTrackingTime );
		}
	}

	m_vEffectDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

	CheckSkillDesc( pChar );

	if( m_bInitJumpPower )
	{
		pChar->SetCurJumpPowerAmt( 0.0f );
		pChar->SetGravityAmt( 0.0f );
	}

	SetSkillNameChatBubble( pChar );

	return true;
}

void ioRangeAttackSkill::SetActionState( ioBaseChar *pChar )
{
	ioEntityGroup *pGrp = pChar->GetGroup();
	ioHashString szSkillMotion = GetSkillMotionName();

	int iCurSkillAniID = pGrp->GetAnimationIdx( szSkillMotion );
	
	float fTimeRate = GetAttribute().m_fAttackAniRate;
	DWORD dwPreDelay = GetAttribute().m_dwPreDelay;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	if( iCurSkillAniID == -1 )
	{
		m_dwMotionStartTime = FRAMEGETTIME();
		LOG.PrintTimeAndLog( 0, "ioRangeAttackSkill::SetActionState() - %s, Not Exist ActionAnimation", GetName().c_str() );
	}
	else
	{
		pGrp->SetActionAni( iCurSkillAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );
		m_dwMotionStartTime = FRAMEGETTIME();
		m_dwMotionEndTime = m_dwMotionStartTime + pGrp->GetAnimationFullTime( iCurSkillAniID )*fTimeRate - FLOAT100;


		pChar->SetReservedSliding( GetAttribute().m_vForceInfoList, iCurSkillAniID, fTimeRate, dwPreDelay );
	}

	m_RangeState = RS_ACTION;

	pChar->AttachEffect( m_SkillEffectName );
	pChar->CreateMapEffectBySkill( m_SkillMapEffectName, pChar->GetWorldPosition() );

	// 사용 아이템 처리
	if( m_bUseItemMesh && !m_StartHandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_StartHandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}

	pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
	m_bReduceGauge = true;

	//
	if( m_bOwnerBuffAction )
		SetOwnerBuffList( pChar );
}

void ioRangeAttackSkill::SetLoopState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();
	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_LoopAnimation );

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	if( iAniID == -1 )
	{
		m_dwEnableReserveTime = dwCurTime;
		LOG.PrintTimeAndLog( 0, "ioRangeAttackSkill::SetLoopState() - %s, Not Exist LoopAnimation",
								 GetName().c_str() );
	}
	else
	{
		pGrp->SetLoopAni( m_LoopAnimation, FLOAT100 );
		m_dwEnableReserveTime = dwCurTime;
		m_dwEnableReserveTime += pGrp->GetAnimationEventTime_e( iAniID, "key_reserve" );
	}

	D3DXVECTOR3 vPos = pChar->GetMidPositionByRate( FLOAT08 );
	vPos += m_vEffectDir * m_fOffSet;

	ioMapEffect *pMapEffect = pChar->CreateMapEffectBySkill( m_RangeEffect, vPos );
	if( pMapEffect )
	{
		m_MapEffect = pMapEffect->GetUniqueID();
	}

	pChar->SkillFireExplicit( GetAttribute().m_vWeaponInfoList.front(),
							  GetName(),
							  m_dwWeaponBaseIndex,
							  vPos );

	m_dwLoopStartTime = dwCurTime;
	m_dwProtectTime = m_dwLoopStartTime + m_dwLoopProtectDuration;

	m_RangeState = RS_LOOP;

	if( m_bSetHandMesh )
	{
		if( !m_StartHandMesh.IsEmpty() )
		{
			pChar->RestoreSkillWeaponMesh( this, m_StartHandMesh, m_bVisibleEquipMesh );
		}

		m_bSetHandMesh = false;
	}

	// 사용 아이템 처리
	if( m_bUseItemMesh && !m_LoopHandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_LoopHandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}
}

void ioRangeAttackSkill::SetEndState( ioBaseChar *pChar )
{
	ioRangeSkill::SetEndState( pChar );

	if( !m_bOwnerBuffAction )
		SetOwnerBuffList( pChar );
}

void ioRangeAttackSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwPreDelayTime = GetPreDelayTime();

	switch( m_RangeState )
	{
	case RS_NONE:
		if( m_dwPreDelayStartTime > 0 && m_dwPreDelayStartTime + dwPreDelayTime < FRAMEGETTIME() )
		{
			m_dwPreDelayStartTime = 0;
			EndPreDelayEffect( pChar );
			SetActionState( pChar );
		}
		break;
	case RS_ACTION:
		if( m_dwMotionStartTime > 0 && m_dwMotionEndTime < FRAMEGETTIME() )
		{
			m_dwMotionStartTime = 0;
			SetLoopState( pChar, pStage );
		}
		break;
	case RS_LOOP:
		ProcessMapEffect( pChar, pStage );

		if( m_dwLoopStartTime > 0 && m_dwLoopStartTime+m_dwLoopTime < FRAMEGETTIME() )
		{
			m_dwLoopStartTime = 0;
			SetEndState( pChar );
		}
		else if( m_dwLoopStartTime == 0 )
		{
			SetEndState( pChar );
		}
		break;
	}
}

void ioRangeAttackSkill::ProcessMapEffect( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( m_MapEffect == -1 ) return;
	if( !m_bOwnerAttached ) return;

	D3DXVECTOR3 vPos = pChar->GetMidPositionByRate( FLOAT08 );
	vPos += m_vEffectDir * m_fOffSet;

	ioMapEffect *pMapEffect = pStage->FindMapEffect( m_MapEffect );
	if( pMapEffect )
	{
		pMapEffect->SetWorldPosition( vPos );
	}
}

void ioRangeAttackSkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
	if( pZone )
	{
		pZone->SetCollisionDir( m_vEffectDir );
		pZone->SetZoneValue( m_fSkillRange, m_fSkillAngle, m_fUpHeight, m_fUnderHeight, m_fWidth, m_bOwnerAttached );
		pZone->SetCollisionWoundType( m_TargetColType, m_TargetWoundType );
		pZone->SetLiveTime( m_dwDuration );
		return;
	}
}

AttackAttribute& ioRangeAttackSkill::GetAttribute()
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

const AttackAttribute& ioRangeAttackSkill::GetAttributeConst() const
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

void ioRangeAttackSkill::LoadNormalAttribute( ioINILoader &rkLoader )
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

void ioRangeAttackSkill::LoadDownAttribute( ioINILoader &rkLoader )
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

void ioRangeAttackSkill::LoadJumpAttribute( ioINILoader &rkLoader )
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


