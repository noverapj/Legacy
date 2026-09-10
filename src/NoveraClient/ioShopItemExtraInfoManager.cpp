
#include "stdafx.h"

#include "ioShopItemExtraInfoManager.h"

template<> ioShopItemExtraInfoManager* Singleton< ioShopItemExtraInfoManager >::ms_Singleton = 0;

ioShopItemExtraInfoManager::ioShopItemExtraInfoManager()
{
	ClearData();
}

ioShopItemExtraInfoManager::~ioShopItemExtraInfoManager()
{
	ClearData();
}

ioShopItemExtraInfoManager& ioShopItemExtraInfoManager::GetSingleton()
{
	return Singleton< ioShopItemExtraInfoManager >::GetSingleton();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//데이터 비우기
void ioShopItemExtraInfoManager::ClearData()
{
	int nInfoDataCnt = (int)m_ShopItemExtraInfoData.size();
	for ( int i=0; i<nInfoDataCnt; ++i )
	{
		m_ShopItemExtraInfoData[i].m_MachineList.clear();
		m_ShopItemExtraInfoData[i].m_SkillList.clear();
		m_ShopItemExtraInfoData[i].m_ShopItemMovieInfoList.clear();
	}
	m_ShopItemExtraInfoData.clear();
}

//데이터 읽기
void ioShopItemExtraInfoManager::LoadProperty()
{	
	ioINILoader_e kLoader( "config/sp2_shop_item_extra_info.ini" );

	char szBuf[MAX_PATH] = "";
	char szTitle[MAX_PATH] = "";
	char szGroupName[MAX_PATH] = "";

	kLoader.SetTitle_e( "common" );
	int nGroupCnt = kLoader.LoadInt_e( "group_cnt", 0 );

	for( int i=0; i < nGroupCnt; ++i )
	{
		wsprintf_e( szTitle, "group%d", i );
		kLoader.LoadString( szTitle, "", szBuf, MAX_PATH );

		wsprintf_e( szGroupName, "config/%s", szBuf );
		LoadFile( szGroupName );
	}	
}

void ioShopItemExtraInfoManager::LoadFile( const char* szFilePath )
{
	ioINILoader_e rkLoader( szFilePath );
	
	rkLoader.SetTitle_e( "common" );
	
	char szBuf[MAX_PATH];
	int nMaxInfo = rkLoader.LoadInt_e( "max_shop_item_extra", 0 );
	for ( int i=0; i<nMaxInfo; ++i )
	{
		wsprintf_e( szBuf, "shop_item_extra_info%d", i+1 );
		rkLoader.SetTitle( szBuf );

		ShopItemExtraInfo sShopItemExtraInfo;
		sShopItemExtraInfo.m_nItemType = rkLoader.LoadInt_e( "item_type", -1 );
		sShopItemExtraInfo.m_dwItemCode = (DWORD)rkLoader.LoadInt_e( "item_code", 0 );
		sShopItemExtraInfo.m_bSetPreview = rkLoader.LoadBool_e( "set_preview", false );
		sShopItemExtraInfo.m_bSetChangeSex = rkLoader.LoadBool_e( "set_change_sex", false );
		sShopItemExtraInfo.m_bSetMovie = rkLoader.LoadBool_e( "set_movie", false );
		sShopItemExtraInfo.m_bSetMovieDown = rkLoader.LoadBool_e( "set_movie_down", false );
		
		LoadMovieInfoList( rkLoader, sShopItemExtraInfo.m_ShopItemMovieInfoList );
		LoadMachineList( rkLoader, sShopItemExtraInfo.m_MachineList );
		LoadSkillList( rkLoader, sShopItemExtraInfo.m_SkillList );

		m_ShopItemExtraInfoData.push_back( sShopItemExtraInfo );
	}
}

void ioShopItemExtraInfoManager::LoadMachineList( ioINILoader &rkLoader, IntVec& vList )
{
	char szBuf[MAX_PATH];

	int nMaxCnt = rkLoader.LoadInt_e( "max_machine", 0 );
	vList.clear();
	vList.reserve( nMaxCnt );

	for ( int i=0; i< nMaxCnt; i++ )
	{
		wsprintf_e( szBuf, "machine_code%d", i+1 );
		int nMachineCode = rkLoader.LoadInt( szBuf, 0 );
		vList.push_back( nMachineCode );
	}
}

void ioShopItemExtraInfoManager::LoadSkillList( ioINILoader &rkLoader, IntVec& vList )
{
	char szBuf[MAX_PATH];

	int nMaxCnt = rkLoader.LoadInt_e( "max_sub_item", 0 );
	vList.clear();
	vList.reserve( nMaxCnt );

	for ( int i=0; i< nMaxCnt; i++ )
	{
		wsprintf_e( szBuf, "sub_item_code%d", i+1 );
		int nItemCode = rkLoader.LoadInt( szBuf, 0 );
		vList.push_back( nItemCode );
	}
}

void ioShopItemExtraInfoManager::LoadMovieInfoList( ioINILoader &rkLoader, ShopItemMovieInfoList& vList )
{
	char szBuf[MAX_PATH], szKey[MAX_PATH];

	int nMaxCnt = rkLoader.LoadInt_e( "max_movie", 0 );
	vList.clear();
	vList.reserve( nMaxCnt );

	for ( int i=0; i< nMaxCnt; i++ )
	{
		ShopItemMovieInfo sShopItemMovieInfo;
		wsprintf_e( szKey, "movie%d_dir", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		sShopItemMovieInfo.m_szMovieDir = szBuf;

		wsprintf_e( szKey, "movie%d_file", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		sShopItemMovieInfo.m_szMovieFile = szBuf;

		wsprintf_e( szKey, "movie%d_url", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		sShopItemMovieInfo.m_szMovieUrl = szBuf;

		wsprintf_e( szKey, "movie%d_after_process", i+1 );
		sShopItemMovieInfo.m_bAfterProcess = rkLoader.LoadBool( szKey, false );

		vList.push_back( sShopItemMovieInfo );
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////

ioShopItemExtraInfoManager::ShopItemExtraInfo* ioShopItemExtraInfoManager::FindExtraInfo( int nItemType, int nItemValue, DWORD dwItemCode )
{
	if ( m_ShopItemExtraInfoData.empty() || 
		 !COMPARE( nItemType, PRESENT_DEFAULT, PRESENT_MAX ) ||
		 nItemValue == 0 ||
		 dwItemCode == 0 )
		return NULL;

	int nInfoDataCnt = (int)m_ShopItemExtraInfoData.size();
	for ( int i=0; i< nInfoDataCnt; ++i )
	{
		const ShopItemExtraInfo& sShopExtraInfo = m_ShopItemExtraInfoData[i];
		if ( sShopExtraInfo.m_nItemType == nItemType && 
			 sShopExtraInfo.m_dwItemCode == dwItemCode )
		{
			if ( nItemType == PRESENT_EXTRAITEM_BOX &&
				 !IsApplyMachine( nItemValue, sShopExtraInfo.m_MachineList ) )
				return NULL;
			else
				return &m_ShopItemExtraInfoData[i];
		}
	}

	return NULL;
}

bool ioShopItemExtraInfoManager::IsApplyMachine( int nItemValue, const IntVec& vList )
{
	if ( vList.empty() || nItemValue <= 0 )
		return false;

	for ( int i=0; i< (int)vList.size(); ++i )
	{
		if ( nItemValue == vList[i] )
			return true;
	}

	return false;
}