

#include "stdafx.h"

#include "ioStateEscapeSkill.h"

#include "ioBaseChar.h"


ioStateEscapeSkill::ioStateEscapeSkill()
{
}

ioStateEscapeSkill::ioStateEscapeSkill( const ioStateEscapeSkill &rhs )
: ioNormalSkill( rhs )
{
}

ioStateEscapeSkill::~ioStateEscapeSkill()
{
}

void ioStateEscapeSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioNormalSkill::LoadProperty( rkLoader );
}

ioSkill* ioStateEscapeSkill::Clone()
{
	return new ioStateEscapeSkill( *this );
}

bool ioStateEscapeSkill::OnSkillStart( ioBaseChar *pChar )
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

	pChar->AttachEffect( m_SkillEffectName );

	if( !StartPreDelayAnimation( pChar ) )	// Delay AnimationÀÌ ¾ø´Ù
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

void ioStateEscapeSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
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

void ioStateEscapeSkill::SetNormalSkill( ioBaseChar *pChar )
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

	m_dwFireMotionEndTime = FRAMEGETTIME();
	m_dwFireMotionEndTime += pGrp->GetAnimationEventTime_e( iCurSkillAniID, "firemotion_e" ) * fTimeRate;

	m_dwEnableReserveTime = FRAMEGETTIME();
	m_dwEnableReserveTime += pGrp->GetAnimationEventTime_e( iCurSkillAniID, "key_reserve" ) * fTimeRate;

	if( m_dwMotionStartTime >= m_dwFireMotionEndTime )
		m_dwFireMotionEndTime = m_dwMotionEndTime;

	pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
	m_bReduceGauge = true;
}
