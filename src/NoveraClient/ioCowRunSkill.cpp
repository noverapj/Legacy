

#include "stdafx.h"

#include "ioBaseChar.h"

#include "ioCowRunSkill.h"

#include "ioGrowthUpInfo.h"

ioCowRunSkill::ioCowRunSkill()
{
	m_fCurRunSpeed = 0.0f;
}

ioCowRunSkill::ioCowRunSkill( const ioCowRunSkill &rhs )
: ioAttackSkill( rhs ),
m_dwRunStateTime( rhs.m_dwRunStateTime ),
m_fRunSpeed( rhs.m_fRunSpeed ),
m_bUseFireEnd( rhs.m_bUseFireEnd )
{
	m_fCurRunSpeed = 0.0f;
}

ioCowRunSkill::~ioCowRunSkill()
{
}

void ioCowRunSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	m_dwRunStateTime = rkLoader.LoadInt_e( "run_duration", 2000 );
	m_fRunSpeed		 = rkLoader.LoadFloat_e( "run_speed", 400.0f );
	m_bUseFireEnd	 = rkLoader.LoadBool_e( "use_fire_end", false );
}

ioSkill* ioCowRunSkill::Clone()
{
	return new ioCowRunSkill( *this );
}

void ioCowRunSkill::AttackSkillFire( ioBaseChar *pChar )
{
	ioEntityGroup *pGrp = pChar->GetGroup();
	ioHashString szSkillMotion = GetSkillMotionName();
	
	int iCurAniID = pGrp->GetAnimationIdx( szSkillMotion );
	if( iCurAniID == -1 )	return;

	float fTimeRate = GetAttackTimeRate();

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetLoopAni( iCurAniID, FLOAT100, FLOAT1, FLOAT1/fTimeRate );

	m_dwMotionStartTime = FRAMEGETTIME();
	m_dwMotionEndTime = m_dwMotionStartTime + m_dwRunStateTime;

	m_dwFireMotionEndTime = FRAMEGETTIME();
	m_dwFireMotionEndTime += pGrp->GetAnimationEventTime_e( iCurAniID, "firemotion_e" ) * fTimeRate;

	m_dwEnableReserveTime = FRAMEGETTIME();
	m_dwEnableReserveTime += pGrp->GetAnimationEventTime_e( iCurAniID, "key_reserve" ) * fTimeRate;

	if( !m_bUseFireEnd )
		m_dwFireMotionEndTime = m_dwMotionEndTime;

	pChar->RefreshFireTimeListWithBase( m_dwWeaponBaseIndex,
										iCurAniID,
										GetAttribute().m_vWeaponInfoList,
										FTT_SKILL_ATTACK,
										fTimeRate,
										0 );
	
	pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
	m_bReduceGauge = true;

	float fValue = 0.0f;
	m_fCurRunSpeed = m_fRunSpeed;

	ioGrowthNormalUpInfo *pUpInfo = ToNormalUpInfo(GetGrowthUpInfoByType(GT_COWRUN_SPEED, pChar->GetCharName()));
	if( pUpInfo )
	{
		ioPlayStage *pStage = g_SkillMaker.GetPlayStage();
		fValue = pUpInfo->GetValue(pStage);
	}

	m_fCurRunSpeed += fValue;
}

void ioCowRunSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( m_dwPreDelayStartTime == 0 )
	{
		pChar->SetCurMoveSpeed( m_fCurRunSpeed );
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

void ioCowRunSkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	if( !m_bUseFireEnd )
	{
		pWeapon->SetLiveTime( m_dwRunStateTime );
	}

	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );
}



