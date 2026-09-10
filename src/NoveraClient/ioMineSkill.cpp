

#include "stdafx.h"

#include "ioMineSkill.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "WeaponDefine.h"
#include "ItemDefine.h"
#include "BuffDefine.h"

ioMineSkill::ioMineSkill()
{
	m_dwBuryEndTime = 0;
}

ioMineSkill::ioMineSkill( const ioMineSkill &rhs )
: ioAttackSkill( rhs ),
m_iMaxMineCnt( rhs.m_iMaxMineCnt ),
m_iMineType( rhs.m_iMineType )
{
	m_dwBuryEndTime = 0;
}

ioMineSkill::~ioMineSkill()
{
}

void ioMineSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );

	m_iMaxMineCnt = rkLoader.LoadInt_e( "max_mine_cnt", 0 );
	m_iMineType = rkLoader.LoadInt_e( "mine_type", 0 );
}

ioSkill* ioMineSkill::Clone()
{
	return new ioMineSkill( *this );
}

void ioMineSkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	ioMineWeapon *pMine = ToMineWeapon( pWeapon );
	if( pMine )
	{
		pMine->SetSensingStartTime( m_dwBuryEndTime );
	}

	int iMaxMineCnt = m_iMaxMineCnt;
	ioMineItem *pMineWeapon = ToMineItem( GetOwnerItem() );
	if( pMineWeapon )
	{
		iMaxMineCnt = pMineWeapon->GetMaxBullet();
	}

	ioMineWeapon2 *pMine2 = ToMineWeapon2( pWeapon );
	if( pMine2 )
	{
		pMine2->SetSensingStartTime( m_dwBuryEndTime );
	}

	ioMineWeapon3 *pMine3 = ToMineWeapon3( pWeapon );
	if( pMine3 )
	{
		pMine3->SetSensingStartTime( m_dwBuryEndTime );
	}

	ioMineWeapon4 *pMine4 = ToMineWeapon4( pWeapon );
	if( pMine4 )
	{
		pMine4->SetSensingStartTime( m_dwBuryEndTime );
	}

	g_WeaponMgr.CheckMaxWeaponCnt( pWeapon, iMaxMineCnt );
}

bool ioMineSkill::OnSkillStart( ioBaseChar *pChar )
{	
	ioAttackSkill::OnSkillStart( pChar );
	m_dwBuryEndTime = 0;

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	return true;
}

void ioMineSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	ioAttackSkill::OnProcessState( pChar, pStage );

	if( m_dwBuryEndTime > 0 && m_dwBuryEndTime < FRAMEGETTIME() )
	{
		pChar->SkillFireExplicit( GetAttribute().m_vWeaponInfoList.front(),
								  GetName(),
								  m_dwWeaponBaseIndex,
								  pChar->GetMidPositionByRate() );

		m_dwBuryEndTime = 0;
	}
}

void ioMineSkill::AttackSkillFire( ioBaseChar *pChar )
{
	EnableRefreshFireTimeList( false );	// ioAttackSkill::AttackSkillFire() 이전에 해줘야함

	ioAttackSkill::AttackSkillFire( pChar );

	ioEntityGroup *pGrp = pChar->GetGroup();
	ioHashString szSkillMotion = GetSkillMotionName();

	int iAniID = pGrp->GetAnimationIdx( szSkillMotion );
	if( iAniID != -1 )
	{
		float fFireTime = pChar->GetFirstFireTime( szSkillMotion ) * GetAttribute().m_fAttackAniRate;
		m_dwBuryEndTime = FRAMEGETTIME() + fFireTime;
	}
}

void ioMineSkill::OnSkillEnd( ioBaseChar *pChar )
{
	ioSkill::OnSkillEnd( pChar );

	// 기본무기 장착
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( true );
	}

	m_bSetHandMesh = false;
}


