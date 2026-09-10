

#include "stdafx.h"

#include "ioBaseChar.h"


#include "ioWeaponActionSkill.h"

ioWeaponActionSkill::ioWeaponActionSkill()
{
	m_bSetHandMesh = false;
	m_dwWeaponActionTime = 0;
}

ioWeaponActionSkill::ioWeaponActionSkill( const ioWeaponActionSkill &rhs )
: ioNormalSkill( rhs )
{
	m_bSetHandMesh = false;
	m_dwWeaponActionTime = 0;
}

ioWeaponActionSkill::~ioWeaponActionSkill()
{
}

void ioWeaponActionSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioNormalSkill::LoadProperty( rkLoader );
}

ioSkill* ioWeaponActionSkill::Clone()
{
	return new ioWeaponActionSkill( *this );
}

bool ioWeaponActionSkill::OnSkillStart( ioBaseChar *pChar )
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

	pChar->SetSkillProtection();
	m_dwWeaponActionTime = 0;

	ioPlayStage *pStage = g_SkillMaker.GetPlayStage();

	pChar->AttachEffect( m_SkillEffectName );

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

void ioWeaponActionSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwPreDelayStartTime == 0 )
	{
		if( m_dwWeaponActionTime > 0 && m_dwWeaponActionTime < dwCurTime )
		{
			SetWeaponAction( pChar );
			m_dwWeaponActionTime = 0;
		}

		return;
	}

	DWORD dwPreDelayTime = GetPreDelayTime();

	if( m_dwPreDelayStartTime + dwPreDelayTime < dwCurTime )
	{
		m_dwPreDelayStartTime = 0;
		EndPreDelayEffect( pChar );
		SetNormalSkill( pChar );
	}
}

void ioWeaponActionSkill::SetNormalSkill( ioBaseChar *pChar )
{
	ioEntityGroup *pGrp = pChar->GetGroup();
	ioHashString szSkillMotion = GetSkillMotionName();

	int iCurSkillAniID = pGrp->GetAnimationIdx( szSkillMotion );
	if( iCurSkillAniID == -1 )	return;

	pChar->AttachEffect( m_SkillEffectName );

	float fAniRate = GetSkillMotionRate();
	float fTimeRate = fAniRate * m_fExtraAniRate;

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iCurSkillAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );
	
	m_dwMotionStartTime = m_dwMotionEndTime = FRAMEGETTIME();
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iCurSkillAniID ) * fTimeRate;

	DWORD dwActionTime = pGrp->GetAnimationEventTime_e( iCurSkillAniID, "fire_s" ) * fTimeRate;
	m_dwWeaponActionTime = m_dwMotionStartTime + dwActionTime;

	m_dwFireMotionEndTime = FRAMEGETTIME();
	m_dwFireMotionEndTime += pGrp->GetAnimationEventTime_e( iCurSkillAniID, "firemotion_e" ) * fTimeRate;

	m_dwEnableReserveTime = FRAMEGETTIME();
	m_dwEnableReserveTime += pGrp->GetAnimationEventTime_e( iCurSkillAniID, "key_reserve" ) * fTimeRate;

	if( m_dwMotionStartTime >= m_dwFireMotionEndTime )
		m_dwFireMotionEndTime = m_dwMotionEndTime;

	
	pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
	m_bReduceGauge = true;

	// 사용 아이템 처리
	if( !m_bSetHandMesh && m_bUseItemMesh && !m_HandMesh.IsEmpty() )
	{
		pChar->ChangeSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );
		m_bSetHandMesh = true;
	}
}

void ioWeaponActionSkill::SetWeaponAction( ioBaseChar *pChar )
{
	if( !pChar ) return;

	g_WeaponMgr.CheckMapAttachWeaponAction( pChar->GetCharName() );
}

void ioWeaponActionSkill::OnSkillEnd( ioBaseChar *pChar )
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

bool ioWeaponActionSkill::IsCanUseSkill( ioBaseChar *pChar, bool bHelpMsg )
{
	if( !ioNormalSkill::IsCanUseSkill( pChar, bHelpMsg ) )
		return false;

	if( g_WeaponMgr.GetCurMapAttachWeaponCnt(pChar) <= 0 )
		return false;

	return true;
}
