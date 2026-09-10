

#include "stdafx.h"

#include "ioDefenseAttackSkill.h"
#include "ioBaseChar.h"

ioDefenseAttackSkill::ioDefenseAttackSkill()
{
	m_bSetHandMesh = false;
	m_SkillState = SS_NONE;
}

ioDefenseAttackSkill::ioDefenseAttackSkill( const ioDefenseAttackSkill &rhs )
: ioAttackSkill( rhs ),
 m_DefenseLoopAni( rhs.m_DefenseLoopAni ),
 m_fDefenseLoopAniRate( rhs.m_fDefenseLoopAniRate )
{
	m_bSetHandMesh = false;
	m_SkillState = SS_NONE;
}

ioDefenseAttackSkill::~ioDefenseAttackSkill()
{
}

void ioDefenseAttackSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";

	rkLoader.LoadString_e( "defense_loop_ani", "", szBuf, MAX_PATH );
	m_DefenseLoopAni = szBuf;
	m_fDefenseLoopAniRate = rkLoader.LoadFloat_e( "defense_loop_ani_rate", FLOAT1 );
}

ioSkill* ioDefenseAttackSkill::Clone()
{
	return new ioDefenseAttackSkill( *this );
}

bool ioDefenseAttackSkill::OnSkillStart( ioBaseChar *pChar )
{
	if( !HasSkillMotion() )
		return false;

	pChar->SetSkillProtection();

	m_bReduceGauge = false;
	m_dwAttackEndTime = 0;

	InitExtraAniJump();

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

	m_bSetHandMesh = false;
	m_SkillState = SS_NONE;

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	m_dwWeaponBaseIndex = pChar->IncreaseWeaponIndexBase();
	if( !StartPreDelayAnimation( pChar ) )	// Delay Animation이 없다
	{
		SetDefenseLoopState( pChar );
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

void ioDefenseAttackSkill::AttackSkillFire( ioBaseChar *pChar )
{
	ioAttackSkill::AttackSkillFire( pChar );

	SetSkillActionState( pChar );
}

void ioDefenseAttackSkill::SetSkillActionState( ioBaseChar *pChar )
{
	m_SkillState = SS_FIRE;
}

void ioDefenseAttackSkill::OnSkillEnd( ioBaseChar *pChar )
{
	ioAttackSkill::OnSkillEnd( pChar );
	
	if( GetOwnerItem() )
		GetOwnerItem()->ClearCurChargeRateForSkill();

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
}

bool ioDefenseAttackSkill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}

void ioDefenseAttackSkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	SetChargingRateForWeapon( pWeapon );
}

void ioDefenseAttackSkill::SetDefenseLoopState( ioBaseChar *pChar )
{
	if( !pChar ) return;

	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_DefenseLoopAni );
	float fTimeRate = m_fDefenseLoopAniRate;
	if( fTimeRate <= 0.0f )
		fTimeRate = FLOAT1;

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );

	pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

	m_dwMotionStartTime = m_dwMotionEndTime = FRAMEGETTIME();
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iAniID ) * fTimeRate;

	m_SkillState = SS_LOOP;

	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}

	if( !m_bReduceGauge )
	{
		pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
		m_bReduceGauge = true;
	}
}

bool ioDefenseAttackSkill::IsCanDefenseStateSkill()
{
	switch( m_SkillState )
	{
	case SS_LOOP:
		return true;
	}

	return false;
}

void ioDefenseAttackSkill::SetDefenseStateSkill( ioBaseChar *pChar )
{
	if( !pChar ) return;

	AttackSkillFire( pChar );
}

bool ioDefenseAttackSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwPreDelayStartTime > 0 )
		return false;

	switch( m_SkillState )
	{
	case SS_LOOP:
	case SS_FIRE:
		if( m_dwMotionEndTime + dwActionStop < dwCurTime )
			return true;

		return false;
	}

	return false;
}

bool ioDefenseAttackSkill::IsProtected( int iDefenseBreakType ) const
{
	if( m_bDisableSkillProtect )
		return false;

	if( DBT_NONE != (DefenseBreakType)m_ProtectCancelType )
	{
		if( iDefenseBreakType == m_ProtectCancelType )   // 스킬 무적 파괴
			return false;
	}

	DWORD dwCurTime = FRAMEGETTIME();

	if( IsPreDelayProtect() )
	{
		if( m_dwPreDelayStartTime != 0 )
			return true;
	}

	switch( m_SkillState )
	{
	case SS_LOOP:
		return false;
	case SS_FIRE:
		if( m_dwFireMotionEndTime > dwCurTime )
			return true;

		return false;
	}

	return false;
}

void ioDefenseAttackSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_SkillState )
	{
	case SS_NONE:
		{
			DWORD dwPreDelay = GetPreDelayTime();
			if( m_dwPreDelayStartTime + dwPreDelay < dwCurTime )
			{
				m_dwPreDelayStartTime = 0;
				EndPreDelayEffect( pChar );

				SetDefenseLoopState( pChar );
			}
		}
		break;
	case SS_LOOP:
		break;
	case SS_FIRE:
		break;
	}
}



