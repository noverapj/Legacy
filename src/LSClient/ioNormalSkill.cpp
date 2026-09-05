

#include "stdafx.h"

#include "ioNormalSkill.h"
#include "ItemDefine.h"

ioNormalSkill::ioNormalSkill()
{
	m_dwPreDelayStartTime = 0;
	m_dwFireMotionEndTime = 0;

	InitExtraAniJump();
}

ioNormalSkill::ioNormalSkill( const ioNormalSkill &rhs )
: ioSkill( rhs )
{
	m_dwPreDelayStartTime = 0;
	m_dwFireMotionEndTime = 0;

	InitExtraAniJump();
}

ioNormalSkill::~ioNormalSkill()
{
}

void ioNormalSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioSkill::LoadProperty( rkLoader );
}

ioSkill* ioNormalSkill::Clone()
{
	return new ioNormalSkill( *this );
}

int ioNormalSkill::GetTypeID() const
{
	return ST_NORMAL;
}

bool ioNormalSkill::IsCanUseSkill( ioBaseChar *pChar, bool bHelpMsg )
{
	ioAttackableItem *pAttackable = ToAttackableItem( GetOwnerItem() );
	if( pAttackable )
	{
		if( !pAttackable->IsCanFire( pChar ) )
			return false;
	}

	if( !CheckNeedGauge( pChar, bHelpMsg ) )
		return false;

	return true;
}

bool ioNormalSkill::OnSkillStart( ioBaseChar *pChar )
{
	if ( m_bPetForceHide )
	{
		ioPet *pPet = g_PetMgr.FindPet( pChar );
		if ( pPet )
			pPet->SetForceHide( true );
	}

	if( !m_bNoCheckSkillmotion && !HasSkillMotion() )
		return false;

	pChar->SetSkillProtection();

	m_dwEnableSwitchTime = 0;
	m_dwEnableSemiSwitchTime = 0;

	m_SkillSoundID = 0;

	if( m_dwSwitchSkill > 0 )
		m_dwEnableSwitchTime = FRAMEGETTIME() + m_dwSwitchSkill;
	if( m_dwSemiSwitchSkill > 0 )
		m_dwEnableSemiSwitchTime = FRAMEGETTIME() + m_dwSemiSwitchSkill;

	m_dwSkillStartTime = FRAMEGETTIME();

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

void ioNormalSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	if( m_dwPreDelayStartTime == 0 )
	{
		CheckExtraAniJump( pChar );
		return;
	}

	DWORD dwPreDelayTime = GetPreDelayTime();

	if( m_dwPreDelayStartTime + dwPreDelayTime < FRAMEGETTIME() )
	{
		m_dwPreDelayStartTime = 0;
		EndPreDelayEffect( pChar );

		SetNormalSkill( pChar );
	}
}

void ioNormalSkill::OnSkillEnd( ioBaseChar *pChar )
{
	EndPreDelayEffect( pChar );

	pChar->EndEffect( m_SkillEffectName );

	GetOwnerItem()->ClearTarget();

	m_bReduceGauge = false;
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

void ioNormalSkill::StartPreDelayEffect( ioBaseChar *pChar )
{
	ioSkill::StartPreDelayEffect( pChar );

	if( GetOwnerItem() )
	{
		GetOwnerItem()->AddGlowLine( pChar );
	}
}

void ioNormalSkill::EndPreDelayEffect( ioBaseChar *pChar )
{
	ioSkill::EndPreDelayEffect( pChar );

	if( GetOwnerItem() )
	{
		GetOwnerItem()->RemoveGlowLine( pChar );
	}
}

bool ioNormalSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{
	if( m_dwPreDelayStartTime > 0 )
		return false;

	if( m_dwMotionEndTime + dwActionStop > FRAMEGETTIME() )
		return false;

	return true;
}

void ioNormalSkill::SetNormalSkill( ioBaseChar *pChar )
{
	ioEntityGroup *pGrp = pChar->GetGroup();
	ioHashString szSkillMotion = GetSkillMotionName();

	int iCurSkillAniID = pGrp->GetAnimationIdx( szSkillMotion );
	if( iCurSkillAniID == -1 )	return;

	DWORD dwCurTime = FRAMEGETTIME();
	pChar->AttachEffect( m_SkillEffectName );

	float fAniRate = GetSkillMotionRate();

	float fTimeRate = fAniRate * m_fExtraAniRate;
	
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->SetActionAni( iCurSkillAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate );
	
	m_dwMotionStartTime = m_dwMotionEndTime = dwCurTime;
	m_dwMotionEndTime += pGrp->GetAnimationFullTime( iCurSkillAniID ) * fTimeRate;

	DWORD dwFireStartTime;
	dwFireStartTime = m_dwFireMotionEndTime = dwCurTime;
	dwFireStartTime += pGrp->GetAnimationEventTime_e( iCurSkillAniID, "fire_s" ) * fTimeRate;
	m_dwFireMotionEndTime += pGrp->GetAnimationEventTime_e( iCurSkillAniID, "firemotion_e" ) * fTimeRate;

	m_dwEnableReserveTime = dwCurTime;
	m_dwEnableReserveTime += pGrp->GetAnimationEventTime_e( iCurSkillAniID, "key_reserve" ) * fTimeRate;

	if( dwFireStartTime >= m_dwFireMotionEndTime )
		m_dwFireMotionEndTime = m_dwMotionEndTime;

	//
	SetExtraAniJump( pChar, iCurSkillAniID, fTimeRate );
	
	pChar->ReduceNeedGaugeBySkill( this, GetNeedGauge(pChar->GetCharName()) );
	m_bReduceGauge = true;
}

bool ioNormalSkill::IsAttackEndState() const
{
	return false;
}
