#include <stdafx.h>

#include "Practice_4.h"
#include "ioPracticeMode.h"

#include "../../../ioMineWeapon.h"
#include "../../../ioMineItem.h"
#include "../../../GUI/MiniMap.h"


Practice_4::Practice_4()
{
	m_dwCount = 0;
}

Practice_4::~Practice_4()
{
}

void Practice_4::LoadPracticeKit( ioINILoader &rkLoader )
{
	m_PracticeNPCList.clear();

	rkLoader.SetTitle_e( "MinePos" );

	std::vector<D3DXVECTOR3> vMinePosList;

	int iMaxMine = rkLoader.LoadInt_e( "MaxMine", 0 );
	for( int i = 0;i < iMaxMine;i++ )
	{
		char szKey[MAX_PATH];
		D3DXVECTOR3 MinePos;

		sprintf_e( szKey, "Mine%d_pos_x", i + 1 );
		MinePos.x = rkLoader.LoadFloat( szKey, 0.0f );
		sprintf_e( szKey, "Mine%d_pos_z", i + 1 );
		MinePos.z = rkLoader.LoadFloat( szKey, 0.0f );

		vMinePosList.push_back( MinePos );
	}

	int iCharCnt = m_pPracticeMode->GetRecordCharCnt();

	for ( int i = 0; i < iCharCnt; i++ )
	{
		ioBaseChar *pChar =  m_pPracticeMode->GetRecordChar( i );
		if( !pChar )
			continue;
		if( IsNpcChar( pChar ) )
		{
			ioWeaponItem* pWeaponItem = pChar->GetWeapon();
			if( !pWeaponItem )
				return;

			ioMineItem *pMineItem = ToMineItem( pWeaponItem );
			if( !pMineItem )
				return;

			int iSize = vMinePosList.size();
			for ( int i = 0; i < iSize; i++ )
			{
				pChar->NormalAttackFireExplicit( pMineItem->GetExtendAttributeIdx(), ioMath::VEC3_ZERO, vMinePosList[i] );
			}
		}
	}
}

bool Practice_4::ProcessCheckPracticeEnd( DWORD dwPlayStartTime, int &iStar, int &iGrade )
{
	if( !m_pPracticeMode )
		return true;

	DWORD dwTimeGap = FRAMEGETTIME() - dwPlayStartTime;

	if ( m_dwCount < m_dwGradeC )
	{
		iStar = ioPracticeManager::E_GRADE_NONE;
		iGrade = ioPracticeManager::E_GRADE_NONE;
	}
	else if ( m_dwCount < m_dwGradeB )
	{
		iStar = ioPracticeManager::E_GRADE_C;
		iGrade = m_dwGradeC;
	}
	else if ( m_dwCount < m_dwGradeA )
	{
		iStar = ioPracticeManager::E_GRADE_B;
		iGrade = m_dwGradeB;
	}
	else if ( m_dwCount >= m_dwGradeA )
	{
		iStar = ioPracticeManager::E_GRADE_A;
		iGrade = m_dwGradeA;
		return true;
	}

	if( g_PracticeMgr.GetLimitTime( m_pPracticeMode->GetPracticeIndex() ) * 1000 < dwTimeGap )
		return true;

	ioWeaponManager::WeaponIterator kMine = g_WeaponMgr.GetMineWeaponIterator();
	while( kMine.HasMoreElements() )
	{
		ioMineWeapon *pMine = ToMineWeapon( kMine.Next() );
		if( !pMine )
			continue;

		if( pMine->IsLive() )
			return false;
	}

	return true;
}

void Practice_4::UpdateMiniMapSpotPoint()
{
	if( !m_pPracticeMode )
		return;

	MiniMap *pMiniMap = static_cast<MiniMap*>(g_GUIMgr.FindWnd( MINIMAP_WND ));
	if( !pMiniMap )
		return;

	ioWeaponManager::WeaponIterator kMine = g_WeaponMgr.GetMineWeaponIterator();
	while( kMine.HasMoreElements() )
	{
		ioMineWeapon *pMine = ToMineWeapon( kMine.Next() );
		if( !pMine || !pMine->IsLive() )
			continue;

		pMiniMap->AddPoint( pMine->GetPosition().x, pMine->GetPosition().z, 
							MiniMap::SPOT_BOMB, TEAM_NONE, 0 );
	}
}

void Practice_4::CheckWeaponWounded( ioBaseChar *pChar, ioWeapon *pWeapon, float fDamage )
{
	int iSize = AvailableAttackList.size();
	if( iSize == 0 )
		return;

	if( !pChar )
		return;

	if( fDamage != 0 )
		return;

	for ( int i = 0; i < iSize; i++ )
	{
		if( pWeapon->GetAttributeIndex() == AvailableAttackList[i] )
			++m_dwCount;
	}
}

void Practice_4::CheckWeaponDefense( ioBaseChar *pChar, ioWeapon *pWeapon, float fDamage )
{
	int iSize = AvailableAttackList.size();
	if( iSize == 0 )
		return;

	if( !pChar )
		return;

	if( fDamage != 0 )
		return;

	for ( int i = 0; i < iSize; i++ )
	{
		if( pWeapon->GetAttributeIndex() == AvailableAttackList[i] )
			++m_dwCount;
	}
}

void Practice_4::PracticeProgress( int &iProgress, int &iMaxProgress )
{
	iProgress = m_dwCount;
	iMaxProgress = m_dwGradeA;
}