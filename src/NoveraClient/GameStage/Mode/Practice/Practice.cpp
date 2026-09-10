#include <stdafx.h>

#include "Practice.h"
#include "ioPracticeMode.h"
#include "../../../ioNpcMgr.h"
#include "../../../GUI/MiniMap.h"

Practice::Practice()
{
	m_pPracticeMode = NULL;
}

Practice::~Practice()
{
	m_pPracticeMode = NULL;
	m_CheckPointList.clear();
	m_PracticeNPCList.clear();
	AvailableAttackList.clear();
}

void Practice::Init( ioPracticeMode* pPracticeMode )
{
	m_pPracticeMode = pPracticeMode;
	m_fExtraRecoveryGaugeRate = 1.f;
	m_CheckPointList.clear();
	m_PracticeNPCList.clear();
	AvailableAttackList.clear();

	if( m_pPracticeMode )
	{
		m_dwGradeA = g_PracticeMgr.GetPracticeGradeA( m_pPracticeMode->GetPracticeIndex() );
		m_dwGradeB = g_PracticeMgr.GetPracticeGradeB( m_pPracticeMode->GetPracticeIndex() );
		m_dwGradeC = g_PracticeMgr.GetPracticeGradeC( m_pPracticeMode->GetPracticeIndex() );
	}
}

void Practice::LoadPracticeINI( ioINILoader &rkLoader )
{
	rkLoader.SetTitle_e( "AvailableAttack" );

	int iMaxAttackCnt = rkLoader.LoadInt_e( "MaxAttack", 0 );
	
	for( int i = 0;i < iMaxAttackCnt;i++ )
	{
		int iAttackIndex = 0;
		char szKey[MAX_PATH];
		sprintf_e( szKey, "AttackIndex_%d", i + 1 );
		iAttackIndex = rkLoader.LoadInt( szKey, 0 );

		AvailableAttackList.push_back( iAttackIndex );
	}

	rkLoader.SetTitle_e( "Custom" );
	m_fExtraRecoveryGaugeRate = rkLoader.LoadFloat( "RecoveryGaugeRate", 1.f );

	LoadCheckPoint( rkLoader );
	LoadPracticeNPC( rkLoader );
	LoadPracticeKit( rkLoader );
}

void Practice::LoadCheckPoint( ioINILoader &rkLoader )
{
	m_CheckPointList.clear();

	rkLoader.SetTitle_e( "CheckPoint" );

	int iMaxPoint = rkLoader.LoadInt_e( "MaxPoint", 0 );
	for( int i = 0;i < iMaxPoint;i++ )
	{
		char szKey[MAX_PATH], szBuf[MAX_PATH];

		CheckPoint kCheckPoint;

		sprintf_e( szKey, "CheckPoint%d_order", i + 1 );
		kCheckPoint.m_iOrder = rkLoader.LoadInt( szKey, 0 );
		sprintf_e( szKey, "CheckPoint%d_range", i + 1 );
		kCheckPoint.m_fRange = rkLoader.LoadFloat( szKey, 50.f );
		sprintf_e( szKey, "CheckPoint%d_pos_x", i + 1 );
		kCheckPoint.m_vCheckPointPos.x = rkLoader.LoadFloat( szKey, 0.f );
		sprintf_e( szKey, "CheckPoint%d_pos_y", i + 1 );
		kCheckPoint.m_vCheckPointPos.y = rkLoader.LoadFloat( szKey, 0.f );
		sprintf_e( szKey, "CheckPoint%d_pos_z", i + 1 );
		kCheckPoint.m_vCheckPointPos.z = rkLoader.LoadFloat( szKey, 0.f );
		
		sprintf_e( szKey, "CheckPoint%d_effect", i + 1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kCheckPoint.m_szEffect = szBuf;	

		m_CheckPointList.push_back( kCheckPoint );
	}

	int iSize = m_CheckPointList.size();
	for ( int i = 0; i < iSize; i++ )
	{
		if( m_CheckPointList[i].m_szEffect.IsEmpty() )
			continue;

		ioMapEffect *pMap = m_pPracticeMode->CreateMapEffect( m_CheckPointList[i].m_szEffect, m_CheckPointList[i].m_vCheckPointPos, ioMath::UNIT_ALL );
		if( pMap )
		{
			pMap->SetRenderQueueGroup( RENDER_GROUP_3 );
			m_CheckPointList[i].m_dwEffectID = pMap->GetUniqueID();

			m_vEffectList.push_back( pMap );
		}
	}
}

void Practice::LoadPracticeNPC( ioINILoader &rkLoader )
{
	m_PracticeNPCList.clear();

	rkLoader.SetTitle_e( "PracticeNPC" );

	int iMaxPoint = rkLoader.LoadInt_e( "MaxNPC", 0 );
	for( int i = 0;i < iMaxPoint;i++ )
	{
		char szKey[MAX_PATH], szBuf[MAX_PATH];

		PracticeNPC kPracticeNPC;

		kPracticeNPC.m_dwMonsterID = i + 1;
		sprintf_e( szKey, "PracticeNPC%d_code", i + 1 );
		kPracticeNPC.m_dwNPCCode = rkLoader.LoadInt( szKey, 0 );
		sprintf_e( szKey, "PracticeNPC%d_team", i + 1 );
		kPracticeNPC.m_eTeamType = (TeamType)rkLoader.LoadInt( szKey, 0 );
		sprintf_e( szKey, "PracticeNPC%d_name", i + 1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kPracticeNPC.m_szName = szBuf;	
		sprintf_e( szKey, "PracticeNPC%d_pos_x", i + 1 );
		kPracticeNPC.m_StartXPos = rkLoader.LoadFloat( szKey, 0.0f );
		sprintf_e( szKey, "PracticeNPC%d_pos_z", i + 1 );
		kPracticeNPC.m_StartZPos = rkLoader.LoadFloat( szKey, 0.0f );
		sprintf_e( szKey, "PracticeNPC%d_prisoner", i + 1 );
		kPracticeNPC.m_bPrisoner = rkLoader.LoadBool( szKey, false );

		m_PracticeNPCList.push_back( kPracticeNPC );
	}

	int iSize = m_PracticeNPCList.size();
	for ( int i = 0; i < iSize; i++ )
	{
		m_pPracticeMode->CreatePracticeNpc( m_PracticeNPCList[i] );
	}	
}

void Practice::LoadPracticeKit( ioINILoader &rkLoader )
{

}

void Practice::ProcessCheckPointCollision()
{
	if( !m_pPracticeMode )
		return;

	int iSize = m_CheckPointList.size();
	if( iSize == 0 )
		return;

	ioBaseChar *pChar = m_pPracticeMode->GetOwnerChar();
	if( !pChar )
		return;

	for ( int i = 0; i < iSize; i++ )
	{
		if( m_CheckPointList[i].m_bCheck == true )
			continue;
	
		D3DXVECTOR3 vDiff = m_CheckPointList[i].m_vCheckPointPos - pChar->GetWorldPosition();

		float fCurDistSq = D3DXVec3LengthSq( &vDiff );		
		float fDistance = sqrt( fCurDistSq );

		if( fDistance < m_CheckPointList[i].m_fRange )
		{
			m_CheckPointList[i].m_bCheck = true;

			if( m_CheckPointList[i].m_dwEffectID != 0 )
				m_pPracticeMode->DestroyMapEffect( m_CheckPointList[i].m_dwEffectID );
		}		
	}
}

void Practice::UpdateMiniMapSpotPoint()
{
	if( !m_pPracticeMode )
		return;

	int iSize = m_CheckPointList.size();
	if( iSize == 0 )
		return;

	ioBaseChar *pChar = m_pPracticeMode->GetOwnerChar();
	if( !pChar )
		return;

	MiniMap *pMiniMap = static_cast<MiniMap*>(g_GUIMgr.FindWnd( MINIMAP_WND ));
	if( !pMiniMap )
		return;

	for ( int i = 0; i < iSize; i++ )
	{
		if( m_CheckPointList[i].m_bCheck == true )
			continue;

		pMiniMap->AddPoint( m_CheckPointList[i].m_vCheckPointPos.x, m_CheckPointList[i].m_vCheckPointPos.z, 
							MiniMap::SPOT_CHECK_POINT, pChar->GetTeam(), 0 );
	}
}

void Practice::EscapePrisonerChar( ioBaseChar *pChar )
{
}

bool Practice::ApplyWeaponWounded( ioBaseChar *pChar, ioWeapon *pWeapon )
{
	int iSize = AvailableAttackList.size();
	if( iSize == 0 )
		return true;

	if( !pChar )
		return true;

	for ( int i = 0; i < iSize; i++ )
	{
		if( pWeapon->GetAttributeIndex() == AvailableAttackList[i] )
			return true;
	}

	return false;
}

void Practice::CheckWeaponWounded( ioBaseChar *pChar, ioWeapon *pWeapon, float fDamage )
{
}

void Practice::CheckWeaponDefense( ioBaseChar *pChar, ioWeapon *pWeapon, float fDamage )
{
}