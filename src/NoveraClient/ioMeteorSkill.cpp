

#include "stdafx.h"

#include "ioMeteorSkill.h"

#include "ioBaseChar.h"

#include "WeaponDefine.h"
#include "ItemDefine.h"

ioMeteorSkill::ioMeteorSkill()
{
	m_dwMeteorFireTime = 0;
	m_dwCircleEnd = 0;

	m_fExplosionRange = 0.0f;

	m_dwCurMapEffect = -1;
}

ioMeteorSkill::ioMeteorSkill( const ioMeteorSkill &rhs )
: ioAttackSkill( rhs ),
  m_fCircleOffSet( rhs.m_fCircleOffSet ),
  m_dwCircleDuration( rhs.m_dwCircleDuration ),
  m_RedCircle( rhs.m_RedCircle ),
  m_BlueCircle( rhs.m_BlueCircle )
{
	m_dwMeteorFireTime = 0;
	m_dwCircleEnd = 0;

	m_fExplosionRange = 0.0f;

	m_dwCurMapEffect = -1;
}

ioMeteorSkill::~ioMeteorSkill()
{
}

void ioMeteorSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];
	rkLoader.LoadString_e( "red_team_circle", "", szBuf, MAX_PATH );
	m_RedCircle = szBuf;
	rkLoader.LoadString_e( "blue_team_circle", "", szBuf, MAX_PATH );
	m_BlueCircle = szBuf;

	m_fCircleOffSet = rkLoader.LoadFloat_e( "circle_offset", 0.0f );

	m_dwCircleDuration = rkLoader.LoadInt_e( "circle_duration", 0 );
}

ioSkill* ioMeteorSkill::Clone()
{
	return new ioMeteorSkill( *this );
}

void ioMeteorSkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	ioFlameWeapon *pFlame = ToFlameWeapon( pWeapon );

	if( pFlame && m_fExplosionRange > 0.0f )
	{
		pFlame->SetExplosionRange( m_fExplosionRange );
	}
}

bool ioMeteorSkill::OnSkillStart( ioBaseChar *pChar )
{
	if( !ioAttackSkill::OnSkillStart( pChar ) )
		return false;

	ioMagicStaffItem *pStaff = ToMagicStaffItem( GetOwnerItem() );
	if( pStaff )
	{
		m_fExplosionRange = pStaff->GetCircleRadius();
	}

	ioFireMageExpansionItem *pFireMageExpansionItem = ToFireMageExpansionItem( GetOwnerItem() );
	if( pFireMageExpansionItem )
	{
		m_fExplosionRange = pFireMageExpansionItem->GetCircleRadius();
	}

	//CreateMapEffect
	if( pChar->CheckMagicCircle() )
	{
		pChar->CheckMagicCircleVisible( false );
		pChar->UpdateMagicCirclePos( 0.0f, 0.0f, 0.0f, true, true );
	}

	float fCurOffSet = GetOwnerItem()->GetMagicCircleOffSet();
	if( fCurOffSet == 0.0f )
		fCurOffSet = m_fCircleOffSet;

	m_vCirclePos = pChar->GetMagicCirclePos( fCurOffSet, m_bCircleNoChangeY );
	
	ioMapEffect *pMapEffect = NULL;
	if( pChar->GetTeam() == TEAM_RED )
	{
		pMapEffect = pChar->CreateMapEffectBySkill( m_RedCircle, m_vCirclePos );
	}
	else if( pChar->GetTeam() == TEAM_BLUE )
	{
		pMapEffect = pChar->CreateMapEffectBySkill( m_BlueCircle, m_vCirclePos );
	}
	else
	{
		if( pChar->GetSingleConvertTeam() == TEAM_BLUE )
			pMapEffect = pChar->CreateMapEffectBySkill( m_BlueCircle, m_vCirclePos );
		else
			pMapEffect = pChar->CreateMapEffectBySkill( m_RedCircle, m_vCirclePos );
	}

	if( pMapEffect )
	{
		DWORD dwPreDelayTime = GetPreDelayTime();

		m_dwCurMapEffect = pMapEffect->GetUniqueID();
		m_dwCircleEnd = FRAMEGETTIME() + dwPreDelayTime + m_dwCircleDuration;
	}

	m_dwMeteorFireTime = 0;

	return true;
}

void ioMeteorSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	ioAttackSkill::OnProcessState( pChar, pStage );

	if( m_dwMeteorFireTime > 0 && m_dwMeteorFireTime < FRAMEGETTIME() )
	{
		pChar->SkillFireExplicit( GetAttribute().m_vWeaponInfoList.front(),
								  GetName(),
								  m_dwWeaponBaseIndex,
								  m_vCirclePos );
		
		m_dwMeteorFireTime = 0;
	}

	if( m_dwCircleEnd > 0 && m_dwCircleEnd < FRAMEGETTIME() )
	{
		//DestroyMapEffect
		pChar->EndMapEffect( m_dwCurMapEffect );
		pChar->DestroyMagicCircle();
		m_dwCircleEnd = 0;
	}
}

void ioMeteorSkill::AttackSkillFire( ioBaseChar *pChar )
{
	EnableRefreshFireTimeList( false );	// ioAttackSkill::AttackSkillFire() 이전에 해줘야함

	ioAttackSkill::AttackSkillFire( pChar );

	m_dwMeteorFireTime = FRAMEGETTIME() + GetAttribute().m_dwPreDelay;

	ioEntityGroup *pGrp = pChar->GetGroup();
	ioHashString szSkillMotion = GetSkillMotionName();

	int iAniID = pGrp->GetAnimationIdx( szSkillMotion );
	if( iAniID != -1 )
	{
		float fFireTime = pChar->GetFirstFireTime( szSkillMotion );
		m_dwMeteorFireTime += fFireTime * GetAttribute().m_fAttackAniRate;
	}
}

void ioMeteorSkill::OnSkillEnd( ioBaseChar *pChar )
{
	//DestroyMapEffect
	pChar->EndMapEffect( m_dwCurMapEffect );
	pChar->DestroyMagicCircle();

	ioAttackSkill::OnSkillEnd( pChar );
}

