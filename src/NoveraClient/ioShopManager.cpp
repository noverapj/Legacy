#include "stdafx.h"

#include "ioShopManager.h"
#include "TextColorTable.h"

template<> ioShopManager* Singleton< ioShopManager >::ms_Singleton = 0;

ioShopManager::ioShopManager()
{
}

ioShopManager::~ioShopManager()
{
	Destroy();
}

ioShopManager& ioShopManager::GetSingleton()
{
	return Singleton< ioShopManager >::GetSingleton();
}

void ioShopManager::Initialize()
{
	ioINILoader_e kLoader( "config/sp2_shop.ini" );	
	kLoader.SetTitle_e( "common" );

	int iADTextMax		= kLoader.LoadInt_e( "ad_text_max", 0 );
	int iADImageMax		= kLoader.LoadInt_e( "ad_image_max", 0 );
	int iADProductMax	= kLoader.LoadInt_e( "ad_product_max", 0 );
	int iCategoryMax	= kLoader.LoadInt_e( "ad_product_category_max", 0 );

	LoadCategory( kLoader, iCategoryMax );
	LoadADText( kLoader, iADTextMax );
	LoadADImage( kLoader, iADImageMax );
	LoadADProduct( kLoader, iADProductMax );	
	LoadFilter( kLoader );
	LoadUnablePresentBonus( kLoader );
	LoadEnablePresentBonus( kLoader );
}

void ioShopManager::Destroy()
{
	for( ADImageList::iterator iter = m_ADImageList.begin(); iter != m_ADImageList.end(); ++iter )
	{
		ADImage& AD = *iter;
		SAFEDELETE( AD.m_pImage );
	}
}

void ioShopManager::ReLoad()
{
	Initialize();
}

void ioShopManager::LoadCategory( ioINILoader& rkLoader, int iMax )
{
	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	m_ADCategoryList.clear();
	for( int i = 0; i < iMax; ++i )
	{
		ADCategory Category;
		sprintf_e( szKey, "ad_product_category%d", i + 1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );		
		Category.m_szName = szBuf;

		IntVec vADProductList;
		sprintf_e( szKey, "ad_product_category%d_value", i + 1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		StringVector kString = ioStringConverter::Split( szBuf, "." );
		for ( unsigned int i = 0 ; i< kString.size(); ++i )
		{
			int iProduct = atoi( kString[i].c_str() );
			if( iProduct > 0 )
			{
				vADProductList.push_back( iProduct );
			}
		}
		Category.m_ADProductIndexList = vADProductList;

		m_ADCategoryList.push_back( Category );
	}
}

void ioShopManager::LoadFilter( ioINILoader& rkLoader )
{
	rkLoader.SetTitle_e( "filter" );
	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	int iMax = rkLoader.LoadInt( "max", 0 );
	m_NewShopFilterInfoList.clear();
	for( int i = 0; i < iMax; ++i )
	{
		NewShopFilterInfo kFilter;

		sprintf_e( szKey, "filter%d_name", i + 1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );		
		kFilter.m_FilterName = szBuf;

		sprintf_e( szBuf, "filter%d_type", i + 1 );
		int eType = rkLoader.LoadInt( szBuf, 0 );
		if( !COMPARE( eType, NSF_NONE, NSF_MAX ) )
		{
			kFilter.m_eFiter = NSF_NONE;
		}
		else
		{
			kFilter.m_eFiter = (NewShopFilter)eType;
		}

		IntVec vList;
		sprintf_e( szKey, "filter%d_tab", i + 1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		StringVector kString = ioStringConverter::Split( szBuf, "." );
		for( unsigned int i = 0; i< kString.size(); ++i )
		{
			int iTab = atoi( kString[i].c_str() );
			if( COMPARE( iTab, NewShopWnd::ID_RECOMMEND_TAB_BTN, NewShopWnd::ID_TAB_MAX ) )
			{
				vList.push_back( iTab );
			}
		}
		kFilter.m_EnableTabList = vList;

		m_NewShopFilterInfoList.push_back( kFilter );
	}
}

void ioShopManager::LoadADText( ioINILoader& rkLoader, int iMax )
{
	char szBuf[MAX_PATH] = "";
	m_ADTextList.clear();
	for( int i = 0; i < iMax; i++ )
	{
		sprintf_e( szBuf, "ADText%d", i + 1 );
		rkLoader.SetTitle( szBuf );

		ADText AD;
		rkLoader.LoadString( "text", "", szBuf, MAX_PATH );
		AD.m_Text = szBuf;

		char *pColor = NULL;
		rkLoader.LoadString( "text_color", "", szBuf, MAX_PATH );		
		AD.m_dwColor = strtoul( szBuf, &pColor, 16 );
		
		AD.m_fADSpeed = rkLoader.LoadFloat_e( "text_speed", 0 );

		LoadADPeriod( rkLoader, AD.m_ADStartPeriod, "start" );
		LoadADPeriod( rkLoader, AD.m_ADEndPeriod, "end" );

		m_ADTextList.push_back( AD );
	}
}

void ioShopManager::LoadADImage( ioINILoader& rkLoader, int iMax )
{
	Destroy();

	char szBuf[MAX_PATH] = "";
	m_ADImageList.clear();
	for( int i = 0; i < iMax; i++ )
	{
		sprintf_e( szBuf, "ADImage%d", i + 1 );
		rkLoader.SetTitle( szBuf );

		ADImage AD;
		rkLoader.LoadString_e( "image_set", "", szBuf, MAX_PATH );
		ioHashString szSet = szBuf;

		rkLoader.LoadString_e( "image_name", "", szBuf, MAX_PATH );
		ioHashString szName = szBuf;
		AD.m_pImage = g_UIImageSetMgr.CreateImage( szSet, szName );
		
		LoadADPeriod( rkLoader, AD.m_ADStartPeriod, "start" );
		LoadADPeriod( rkLoader, AD.m_ADEndPeriod, "end" );
		LoadADProductLink( rkLoader, AD.m_Link );

		AD.m_dwHoldTime		= rkLoader.LoadInt_e( "hold_time", 0 );
		AD.m_dPagingTime	= rkLoader.LoadInt_e( "paging_time", 0 );

		m_ADImageList.push_back( AD );
	}
}

void ioShopManager::LoadADPeriod( ioINILoader& rkLoader, ADPeriod& rkPeriod, const char* szTitle )
{
	char szBuf[MAX_PATH] = "";

	sprintf_e( szBuf, "%s_year", szTitle );
	rkPeriod.m_iYear = rkLoader.LoadInt( szBuf, 0 );

	sprintf_e( szBuf, "%s_month", szTitle );
	rkPeriod.m_iMonth = rkLoader.LoadInt( szBuf, 0 );

	sprintf_e( szBuf, "%s_day", szTitle );
	rkPeriod.m_iDay = rkLoader.LoadInt( szBuf, 0 );

	sprintf_e( szBuf, "%s_hour", szTitle );
	rkPeriod.m_iHour = rkLoader.LoadInt( szBuf, 0 );

	sprintf_e( szBuf, "%s_min", szTitle );
	rkPeriod.m_iMin = rkLoader.LoadInt( szBuf, 0 );
}

void ioShopManager::LoadADProductLink( ioINILoader& rkLoader, ADProductLink& Link )
{
	ADProductType eType = (ADProductType)rkLoader.LoadInt_e( "product_type", 0 );
	if( eType != ADPT_SOLDIER && eType != ADPT_EXTRA && eType != ADPT_DECO && eType != ADPT_ETC && eType != ADPT_COSTUME )
	{
		Link.m_eADProductType = ADPT_NONE;
		LOG.PrintTimeAndLog(0, "[SP2_SHOP_ERROR] : [%d] INVAILD TYPE", (int)eType );
	}
	else
	{
		Link.m_eADProductType = eType;
	}

	Link.m_iTab		= rkLoader.LoadInt_e( "main_tab", 0 );
	Link.m_iSubTab	= rkLoader.LoadInt_e( "sub_tab", 0 );
	Link.m_iValue1	= rkLoader.LoadInt_e( "value1", 0 );
	Link.m_iValue2	= rkLoader.LoadInt_e( "value2", 0 );
	Link.m_iValue3	= rkLoader.LoadInt_e( "value3", 0 );
}

void ioShopManager::LoadADProduct( ioINILoader& rkLoader, int iMax )
{
	char szBuf[MAX_PATH] = "";
	m_ADProductList.clear();
	for( int i = 0; i < iMax; i++ )
	{
		sprintf_e( szBuf, "ADProduct%d", i + 1 );
		rkLoader.SetTitle( szBuf );

		ADProduct Product;
		LoadADProductLink( rkLoader, Product.m_Link );

		m_ADProductList.push_back( Product );
	}
}

void ioShopManager::LoadUnablePresentBonus( ioINILoader& rkLoader )
{
	rkLoader.SetTitle_e( "unable_present_bonus" );
	char szBuf[MAX_PATH], szKey[MAX_PATH];

	m_UnablePresentBonusList.clear();
	int nMaxCnt = rkLoader.LoadInt_e( "exception_cnt", 0 );
	for ( int i=0; i<nMaxCnt; i++ )
	{
		sprintf_e( szKey, "exception_%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_UnablePresentBonusList.push_back( szBuf );
	}

	m_bUnablePresentBonus = rkLoader.LoadBool_e("set_unable_present_bonus", false);
}

void ioShopManager::LoadEnablePresentBonus( ioINILoader& rkLoader )
{
	rkLoader.SetTitle_e( "enable_present_bonus" );
	char szBuf[MAX_PATH], szKey[MAX_PATH];

	m_EnablePresentBonusList.clear();
	int nMaxCnt = rkLoader.LoadInt_e( "inclusion_cnt", 0 );
	for ( int i=0; i<nMaxCnt; i++ )
	{
		sprintf_e( szKey, "inclusion_%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_EnablePresentBonusList.push_back( szBuf );
	}
	
	m_bEnablePresentBonus = rkLoader.LoadBool_e("set_enable_present_bonus", false);
}

int ioShopManager::GetADCategoryCount()
{
	return (int)m_ADCategoryList.size();
}

const ioHashString* ioShopManager::GetADCategoryName( int Index )
{
	if( COMPARE( Index, 0, (int)m_ADCategoryList.size() ) )
		return &m_ADCategoryList[Index].m_szName;

	return NULL;
}

void ioShopManager::GetADCategoryProductList( IN int Index, OUT IntVec& List )
{
	if( COMPARE( Index, 0, (int)m_ADCategoryList.size() ) )
	{
		List = m_ADCategoryList[Index].m_ADProductIndexList;
	}
}

int ioShopManager::GetFilterCount()
{
	return (int)m_NewShopFilterInfoList.size();
}

NewShopFilterInfo* ioShopManager::GetFilter( int Index )
{
	if( COMPARE( Index, 0, (int)m_NewShopFilterInfoList.size() ) )
		return &m_NewShopFilterInfoList[Index];

	return NULL;
}

int ioShopManager::GetTextADCount()
{
	return (int)m_ADTextList.size();
}

bool ioShopManager::HasTextAD( int Index )
{
	if( COMPARE( Index, 0, (int)m_ADTextList.size() ) )
		return true;

	return false;
}

const ADText* ioShopManager::GetTextAD( int Index )
{
	if( HasTextAD( Index ) )
		return &m_ADTextList[Index];

	return NULL;
}

int ioShopManager::GetImageADCount()
{
	return (int)m_ADImageList.size();
}

bool ioShopManager::HasImageAD( int Index )
{
	if( COMPARE( Index, 0, (int)m_ADImageList.size() ) )
		return true;

	return false;
}

const ADImage* ioShopManager::GetImageAD( int Index )
{
	if( HasImageAD( Index ) )
		return &m_ADImageList[Index];

	return NULL;
}

int ioShopManager::GetADProductCount()
{
	return (int)m_ADProductList.size();
}

bool ioShopManager::HasADProduct( int Index )
{
	if( COMPARE( Index, 0, (int)m_ADProductList.size() ) )
		return true;

	return false;
}

const ADProduct* ioShopManager::GetADProduct( int Index )
{
	if( HasADProduct( Index ) )
		return &m_ADProductList[Index];

	return NULL;
}

bool ioShopManager::IsEndAD( const ADPeriod& rkPeriod )
{
	SYSTEMTIME rkServerTime;
	g_MyInfo.GetServerTime( rkServerTime );	

	CTime cCurTime( rkServerTime.wYear, rkServerTime.wMonth, rkServerTime.wDay, rkServerTime.wHour, rkServerTime.wMinute, 0 );
	CTime cEndTime( rkPeriod.m_iYear, rkPeriod.m_iMonth, rkPeriod.m_iDay, rkPeriod.m_iHour, rkPeriod.m_iMin, 0 );

	CTimeSpan Gap = cEndTime - cCurTime;
	LONGLONG iSpan = Gap.GetTimeSpan();
	if ( iSpan <= 0 )
		return true;
	
	return false;
}

bool ioShopManager::IsStartAD( const ADPeriod& rkPeriod )
{	
	SYSTEMTIME rkServerTime;
	g_MyInfo.GetServerTime( rkServerTime );	

	CTime cStartTime( rkPeriod.m_iYear, rkPeriod.m_iMonth, rkPeriod.m_iDay, rkPeriod.m_iHour, rkPeriod.m_iMin, 0 );
	CTime cCurTime( rkServerTime.wYear, rkServerTime.wMonth, rkServerTime.wDay, rkServerTime.wHour, rkServerTime.wMinute, 0 );
	
	CTimeSpan Gap = cCurTime - cStartTime;
	LONGLONG iSpan = Gap.GetTimeSpan();
	if ( 0 <= iSpan )
		return true;

	return false;
}

bool ioShopManager::IsUnablePresentBonus()
{
	return m_bUnablePresentBonus;
}

bool ioShopManager::GetUnablePresentBonus( int nPresentType, int nPresemtValue )
{
	char szBuf[MAX_PATH];
	sprintf_e( szBuf, "%d.%d", nPresentType, nPresemtValue );

	for ( int i=0; i<(int)m_UnablePresentBonusList.size(); i++ )
	{
		if ( m_UnablePresentBonusList[i] == szBuf )
			 return true;
	}

	return false;
}

bool ioShopManager::IsEnablePresentBonus()
{
	return m_bEnablePresentBonus;
}

bool ioShopManager::GetEnablePresentBonus( int nPresentType, int nPresemtValue )
{
	char szBuf[MAX_PATH];
	sprintf_e( szBuf, "%d.%d", nPresentType, nPresemtValue );

	for ( int i=0; i<(int)m_EnablePresentBonusList.size(); i++ )
	{
		if ( m_EnablePresentBonusList[i] == szBuf )
			return true;
	}

	return false;
}