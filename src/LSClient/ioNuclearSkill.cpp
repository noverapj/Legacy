

#include "stdafx.h"

#include "ioNuclearSkill.h"

#include "ioBaseChar.h"

#include "WeaponDefine.h"

ioNuclearSkill::ioNuclearSkill()
{
	m_dwNuclearFireTime = 0;
}

ioNuclearSkill::ioNuclearSkill( const ioNuclearSkill &rhs )
: ioAttackSkill( rhs ),
  m_HandSet( rhs.m_HandSet )
{
	m_dwNuclearFireTime = 0;
}

ioNuclearSkill::~ioNuclearSkill()
{
}

void ioNuclearSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];
	rkLoader.LoadString_e( "handset_mesh", "", szBuf, MAX_PATH );
	m_HandSet = szBuf;
}

ioSkill* ioNuclearSkill::Clone()
{
	return new ioNuclearSkill( *this );
}

bool ioNuclearSkill::OnSkillStart( ioBaseChar *pChar )
{
	pChar->ShowWeaponItemMesh( false, true );
	pChar->AddEntity( m_HandSet );

	m_dwNuclearFireTime = 0;

	return ioAttackSkill::OnSkillStart( pChar );
}

void ioNuclearSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	ioAttackSkill::OnProcessState( pChar, pStage );

	if( m_dwNuclearFireTime > 0 && m_dwNuclearFireTime < FRAMEGETTIME() )
	{
		pChar->SkillFireExplicit( GetAttribute().m_vWeaponInfoList.front(),
								  GetName(),
								  m_dwWeaponBaseIndex,
								  pChar->GetWorldPosition() );

		m_dwNuclearFireTime = 0;
	}
}

void ioNuclearSkill::AttackSkillFire( ioBaseChar *pChar )
{
	EnableRefreshFireTimeList( false );	// ioAttackSkill::AttackSkillFire() 이전에 해줘야함

	ioAttackSkill::AttackSkillFire( pChar );

	m_dwNuclearFireTime = FRAMEGETTIME() + GetAttribute().m_dwPreDelay;

	ioEntityGroup *pGrp = pChar->GetGroup();
	ioHashString szSkillMotion = GetSkillMotionName();

	int iAniID = pGrp->GetAnimationIdx( szSkillMotion );
	if( iAniID != -1 )
	{
		float fFireTime = pChar->GetFirstFireTime( szSkillMotion );
		m_dwNuclearFireTime += fFireTime * GetAttribute().m_fAttackAniRate;
	}
}

void ioNuclearSkill::OnSkillEnd( ioBaseChar *pChar )
{
	ioAttackSkill::OnSkillEnd( pChar );

	pChar->DestroyEntity( m_HandSet );
	pChar->ShowWeaponItemMesh( true, true );
}

