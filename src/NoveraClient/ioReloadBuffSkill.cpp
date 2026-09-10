
#include "stdafx.h"

#include "ioReloadBuffSkill.h"
#include "ItemDefine.h"

ioReloadBuffSkill::ioReloadBuffSkill()
{
}

ioReloadBuffSkill::ioReloadBuffSkill( const ioReloadBuffSkill &rhs )
	: ioBuffSkill( rhs ),
m_vForceInfoList( rhs.m_vForceInfoList ),
m_vDownForceInfoList( rhs.m_vDownForceInfoList )
{
}

ioReloadBuffSkill::~ioReloadBuffSkill()
{
}

void ioReloadBuffSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioBuffSkill::LoadProperty( rkLoader );

	m_vForceInfoList.clear();
	int iForceCnt = rkLoader.LoadInt_e( "reload_force_cnt", 0 );
	if( iForceCnt > 0 )
		m_vForceInfoList.reserve( iForceCnt );

	char szBuf[MAX_PATH] = "";
	for( int i=0; i < iForceCnt; ++i )
	{
		ForceInfo kInfo;

		wsprintf_e( szBuf, "reload_force_move%d", i+1 );
		kInfo.m_fForceMovePower = rkLoader.LoadFloat( szBuf, 0.0f );

		wsprintf_e( szBuf, "reload_force_friction%d", i+1 );
		kInfo.m_fForceMoveFriction = rkLoader.LoadFloat( szBuf, FLOAT1 );

		wsprintf_e( szBuf, "reload_force_angle%d", i+1 );
		kInfo.m_fForceAngle = rkLoader.LoadFloat( szBuf, 0.0f );

		m_vForceInfoList.push_back( kInfo );
	}

	m_vDownForceInfoList.clear();
	int iDownForceCnt = rkLoader.LoadInt_e( "down_reload_force_cnt", 0 );
	if( iDownForceCnt > 0 )
		m_vDownForceInfoList.reserve( iForceCnt );

	for( int i=0; i < iDownForceCnt; ++i )
	{
		ForceInfo kInfo;

		wsprintf_e( szBuf, "down_reload_force_move%d", i+1 );
		kInfo.m_fForceMovePower = rkLoader.LoadFloat( szBuf, 0.0f );

		wsprintf_e( szBuf, "down_reload_force_friction%d", i+1 );
		kInfo.m_fForceMoveFriction = rkLoader.LoadFloat( szBuf, FLOAT1 );

		wsprintf_e( szBuf, "down_reload_force_angle%d", i+1 );
		kInfo.m_fForceAngle = rkLoader.LoadFloat( szBuf, 0.0f );

		m_vDownForceInfoList.push_back( kInfo );
	}
}

ioSkill* ioReloadBuffSkill::Clone()
{
	return new ioReloadBuffSkill( *this );
}

void ioReloadBuffSkill::BuffSkillSet( ioBaseChar *pChar )
{
	ioBuffSkill::BuffSkillSet( pChar );

	ioEntityGroup *pGrp = pChar->GetGroup();
	ioHashString szSkillMotion = GetSkillMotionName();

	int iCurSkillAniID = pGrp->GetAnimationIdx( szSkillMotion );
	if( iCurSkillAniID == -1 )	return;

	float fAniRate = GetSkillMotionRate();
	float fTimeRate = fAniRate * m_fExtraAniRate;

	switch( m_SkillUseType )
	{
	case SUT_NORMAL:
		if( !m_vForceInfoList.empty() )
			pChar->SetReservedSliding( m_vForceInfoList, iCurSkillAniID, fTimeRate, 0 );
		break;
	case SUT_DOWN:
		if( !m_vDownForceInfoList.empty() )
			pChar->SetReservedSliding( m_vDownForceInfoList, iCurSkillAniID, fTimeRate, 0 );
		break;
	}

	// Reload
	ioItem *pItem = pChar->GetEquipedItem(ES_WEAPON);
	ioAutoShotItem *pAutoShotItem = ToAutoShotItem( pItem );
	if( pAutoShotItem )
	{
		int iMax = pAutoShotItem->GetMaxBullet();
		pAutoShotItem->SetCurBullet( iMax );
	}

	ioVulcanItem *pVulcanItem = ToVulcanItem( pItem );
	if( pVulcanItem )
	{
		int iMax = pVulcanItem->GetMaxBullet();
		pVulcanItem->SetCurBullet( iMax );
	}
}