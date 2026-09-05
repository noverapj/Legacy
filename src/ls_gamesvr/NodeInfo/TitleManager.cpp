#include "stdafx.h"
#include "User.h"
#include "UserTitleInfo.h"
#include "TitleData.h"
#include "TitleManager.h"

template<> TitleManager *Singleton< TitleManager >::ms_Singleton = 0;

TitleManager::TitleManager()
{
	Init();
}

TitleManager::~TitleManager()
{
	Destroy();
}

TitleManager& TitleManager::GetSingleton()
{
	return Singleton< TitleManager >::GetSingleton();
}

void TitleManager::Init()
{
	m_vPremiumTable.clear();
	m_mTitleTable.clear();

	m_iAddPremiumTime	= 0;
	m_iCheckTime		= 0;
}

void TitleManager::Destroy()
{
	mTitleTable_iter it = m_mTitleTable.begin();

	for(	; it != m_mTitleTable.end(); it++ )
	{
		TitleData* pInfo	= it->second;
		if( pInfo )
			delete pInfo;
	}

	m_mTitleTable.clear();
}

BOOL TitleManager::LoadINI(BOOL bReload)
{
	Init();

	char szKey[MAX_PATH] = "";
	ioINILoader kLoader;

	kLoader.ReloadFile( "config/sp2_title.ini" );

	kLoader.SetTitle( "common" );

	m_iAddPremiumTime	= kLoader.LoadInt("add_time", 0);
	m_iCheckTime		= kLoader.LoadInt("check_time", 0);

	for( int i = 0; i < 1000; i++ )
	{
		wsprintf( szKey, "title%d", i+1 );
		kLoader.SetTitle( szKey );

		DWORD dwCode	= kLoader.LoadInt("code", 0);
		if( 0 == dwCode )
			break;

		int iType			= kLoader.LoadInt("type", 0);
		DWORD dwValue		= kLoader.LoadInt("value", 0);
		int iLevel			= kLoader.LoadInt("max_level", 0);
		int iPrecedeCode	= kLoader.LoadInt("precede_code", 0);
		int iPremium		= kLoader.LoadInt("premium_type", 0);

		TitleData* pData	= new TitleData;
		if( !pData )
			return FALSE;

		pData->CreateTitleByDesignated((TitleTriggerClasses)iType, (TitleTriggerClasses)iPremium, dwCode, dwValue, iLevel, iPrecedeCode);
		m_mTitleTable.insert( std::make_pair(dwCode, pData) );
	}

	for( int i = 0; i < 500; i++ )
	{
		wsprintf( szKey, "premium%d", i+1 );
		kLoader.SetTitle( szKey );
		IntVec vLevelInfo;

		for( int j = 0 ; j < 100 ; j++ )
		{
			wsprintf( szKey, "level%d", j+1 );
			int iInfo		= kLoader.LoadInt(szKey, 0);
			if( 0 == iInfo )
			{
				if( !vLevelInfo.empty() )
					m_vPremiumTable.push_back(vLevelInfo);

				break;
			}

			vLevelInfo.push_back(iInfo);
		}
	}

	return TRUE;
}

TitleData* TitleManager::GetTitleData(const DWORD dwCode)
{
	mTitleTable_iter it	= m_mTitleTable.find(dwCode);
	if( it == m_mTitleTable.end() )
		return NULL;

	return it->second;
}

BOOL TitleManager::CheckAchevement(UserTitleInfo* pInfo, User* pUser, __int64 iValue)
{
	if( !pInfo )
		return FALSE;

	if( !pUser )
		return FALSE;

	//Ini 데이터 트리거를 Get 하고 Trigger 호출.
	TitleData* pTileData = GetTitleData(pInfo->GetCode());
	if( !pTileData )
		return FALSE;

	return pTileData->DoTrigger(pInfo, iValue);
}

BOOL TitleManager::IsComplete(UserTitleInfo* pInfo)
{
	if( !pInfo )
		return FALSE;

	TitleData* pTileData = GetTitleData(pInfo->GetCode());
	if( !pTileData )
		return FALSE;

	return pTileData->IsComplete(pInfo->GetCurValue());
}

BOOL TitleManager::IsComplete(const DWORD dwCode, const __int64 iValue)
{
	TitleData* pTileData = GetTitleData(dwCode);
	if( !pTileData )
		return FALSE;

	return pTileData->IsComplete(iValue);
}

BOOL TitleManager::IsLevelUp(const DWORD dwCode, const int iLevel, const DWORD dwValue)
{
	//일단 프리미엄 타입은.. 하나로 고정..

	for( int i = 0; i < 1; i++ )
	{
		IntVec& vInfo = m_vPremiumTable[i];
		if( vInfo.size() < iLevel )
			return FALSE;

		for( int j = 0; j < (int)vInfo.size(); j++ )
		{
			if( iLevel == j )
			{
				if( vInfo[j] <= dwValue )
					return TRUE;

				return FALSE;
			}
		}
	}

	return FALSE;
}

BOOL TitleManager::IsAccumulateData(TitleTriggerClasses eClass)
{
	if( TITLE_CONSUME_ACCUMULATE_GOLD == eClass )
		return TRUE;
	else if( TITLE_CONSUME_PRESENT_GOLD == eClass )
		return TRUE;
	else if( TITLE_CONSUME_ACCUMULATE_PESO == eClass )
		return TRUE;

	return FALSE;
}

void TitleManager::FillAccumulateTitleInfo(UserTitleInven* pInfo)
{
	if( !pInfo )
		return;

	mTitleTable_iter it = m_mTitleTable.begin();

	for( ; it != m_mTitleTable.end(); it++ )
	{
		TitleData* pData = it->second;
		if( !pData )
			continue;

		if( IsAccumulateData(pData->GetClass()) )
		{
			//인벤에 있는지 확인. 없으면 생성.
			if( !pInfo->GetTitle(pData->GetCode()) )
			{
				pInfo->AddTitle(pData->GetCode(), 0, TITLE_DISABLE);
			}
		}
	}
}

int	TitleManager::GetMaxPremiumLevel(const DWORD dwCode)
{
	/*TitleData* pTileData = GetTitleData(dwCode);
	if( !pTileData )
		return -1;
*/
	IntVec& vInfo = m_vPremiumTable[0];

	return vInfo.size();
}

void TitleManager::GetTargetClassTitleInfo(TitleTriggerClasses eClass, IntVec& vData)
{
	mTitleTable_iter it = m_mTitleTable.begin();

	for( ; it != m_mTitleTable.end(); it++ )
	{
		TitleData* pInfo = it->second;
		if( pInfo )
		{
			if( pInfo->GetClass() == eClass )
				vData.push_back(pInfo->GetCode());
		}
	}
}