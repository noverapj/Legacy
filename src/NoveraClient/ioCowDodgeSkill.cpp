

#include "stdafx.h"

#include "ioCowDodgeSkill.h"

#include "ioBaseChar.h"


ioCowDodgeSkill::ioCowDodgeSkill()
{
	m_fExtraMoveForce = 0.0f;

	m_bSetHandMesh = false;
}

ioCowDodgeSkill::ioCowDodgeSkill( const ioCowDodgeSkill &rhs )
: ioNormalSkill( rhs ),
 m_bUseAttack( rhs.m_bUseAttack ),
 m_AttackAttribute( rhs.m_AttackAttribute ),
 m_fMoveForce( rhs.m_fMoveForce ),
 m_fMoveFriction( rhs.m_fMoveFriction ),
 m_bFrontDir( rhs.m_bFrontDir )
{
	m_fExtraMoveForce = 0.0f;

	m_bSetHandMesh = false;
}

ioCowDodgeSkill::~ioCowDodgeSkill()
{
}

void ioCowDodgeSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioNormalSkill::LoadProperty( rkLoader );

	char szKey[MAX_PATH], szBuf[MAX_PATH];

	m_bUseAttack = rkLoader.LoadBool_e( "use_attack", false );

	m_fMoveForce = rkLoader.LoadFloat_e( "move_force", 0.0f );
	m_fMoveFriction = rkLoader.LoadFloat_e( "move_friction", FLOAT1 );
	m_bFrontDir = rkLoader.LoadBool_e( "move_front_dir", false );

	// Action Attack
	m_AttackAttribute.Init();
	m_AttackAttribute.m_AttackAnimation = m_SkillMotionName;
	m_AttackAttribute.m_fAttackAniRate  = m_fAnimationTimeRate;

	m_AttackAttribute.m_vForceInfoList.clear();

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
}

ioSkill* ioCowDodgeSkill::Clone()
{
	return new ioCowDodgeSkill( *this );
}

bool ioCowDodgeSkill::OnSkillStart( ioBaseChar *pChar )
{	
	if( !HasSkillMotion() )
		return false;

	pChar->SetSkillProtection();

	m_dwEnableSwitchTime = 0;
	m_dwEnableSemiSwitchTime = 0;

	if( m_dwSwitchSkill > 0 )
		m_dwEnableSwitchTime = FRAMEGETTIME() + m_dwSwitchSkill;
	if( m_dwSemiSwitchSkill > 0 )
		m_dwEnableSemiSwitchTime = FRAMEGETTIME() + m_dwSemiSwitchSkill;

	m_dwSkillStartTime = FRAMEGETTIME();

	ioPlayStage *pStage = g_SkillMaker.GetPlayStage();

	// PreDelay때문에 한프레임 늦게 실제 방향적용이 되는데, 1초메세지에 의해 고스트쪽에서 방향이 틀어져 반대로 이동할수도 있다.
	// 때문에 여기서 저장
	m_vMoveDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

	pChar->AttachEffect( m_SkillEffectName );
	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

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

	return true;
}

void ioCowDodgeSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( m_dwPreDelayStartTime == 0 ) return;

	DWORD dwPreDelayTime = GetPreDelayTime();

	if( m_dwPreDelayStartTime + dwPreDelayTime < FRAMEGETTIME() )
	{
		m_dwPreDelayStartTime = 0;
		EndPreDelayEffect( pChar );
		SetNormalSkill( pChar );
	}
}

void ioCowDodgeSkill::SetNormalSkill( ioBaseChar *pChar )
{
	ioEntityGroup *pGrp = pChar->GetGroup();
	ioHashString szSkillMotion = GetSkillMotionName();

	int iCurSkillAniID = pGrp->GetAnimationIdx( szSkillMotion );
	if( iCurSkillAniID == -1 )	return;

	pChar->AttachEffect( m_SkillEffectName );

	float fAniRate = GetSkillMotionRate();

	float fTimeRate = fAniRate * m_fExtraAniRate;


	if( m_bUseAttack )
	{
		pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex,
											iCurSkillAniID,
											m_AttackAttribute.m_vWeaponInfoList,
											FTT_SKILL_ATTACK,
											fTimeRate,
											0 );
	}
	
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iCurSkillAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );
	
	m_dwMotionStartTime = m_dwMotionEndTime = FRAMEGETTIME();
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iCurSkillAniID ) * fTimeRate;

	m_dwFireMotionEndTime = FRAMEGETTIME();
	m_dwFireMotionEndTime += pGrp->GetAnimationEventTime_e( iCurSkillAniID, "firemotion_e" ) * fTimeRate;

	m_dwEnableReserveTime = FRAMEGETTIME();
	m_dwEnableReserveTime += pGrp->GetAnimationEventTime_e( iCurSkillAniID, "key_reserve" ) * fTimeRate;

	if( m_dwMotionStartTime >= m_dwFireMotionEndTime )
		m_dwFireMotionEndTime = m_dwMotionEndTime;

	SetMoving( pChar );

	pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
	m_bReduceGauge = true;

	// 사용 아이템 처리
	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}
}

void ioCowDodgeSkill::SetMoving( ioBaseChar *pChar )
{
	float fCurForce = m_fMoveForce + m_fExtraMoveForce;

	if( m_bFrontDir )
		pChar->SetForcePower( m_vMoveDir, fCurForce, m_fMoveFriction );
	else
		pChar->SetForcePower( -m_vMoveDir, fCurForce, m_fMoveFriction );
}

void ioCowDodgeSkill::OnSkillEnd( ioBaseChar *pChar )
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
	{
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
	}
}