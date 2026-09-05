
#include "stdafx.h"

#include "ioCostumeShopInfoManager.h"

#include "ioSaleManager.h"

////////////////////////////////////////////////////////////////////////////
CostumeShopInfo::CostumeShopInfo()
{
	Init();
}

CostumeShopInfo::~CostumeShopInfo()
{
	m_CostumeShopSubInfoList.clear();
	m_vNeedPeso.clear();
	m_vPeriod.clear();
}

void CostumeShopInfo::Init()
{
	m_szName.Clear();
	m_szIconName.Clear();
	m_nPriority = -1;
	m_nShopMarkType = -1;
	m_nLimitDate = 0;
	m_nMainManual = 0;
	m_nGroup = 0;
	m_bActive = false;
	m_nCostumeShopCode = 0;
	m_nRecommandedShopOrder = ORDER_DEFAULT;
	m_nNeedLevel = 0;
	m_nSubscriptionType = SUBSCRIPTION_NONE;

	m_CostumeShopSubInfoList.clear();
	m_vNeedPeso.clear();
	m_vPeriod.clear();

	m_nDefaultType = 0;
	m_bSetPreview = false;
	m_bPackage = false;

	m_nNeedCash = 0;
	m_nBonusPeso = 0;
	m_nSellPeso = 0;
}

void CostumeShopInfo::LoadCostumeShopBasicInfo( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];

	rkLoader.LoadString_e( "name", "", szBuf, MAX_PATH );
	m_szName = szBuf;

	rkLoader.LoadString_e( "icon", "", szBuf, MAX_PATH );
	m_szIconName = szBuf;

	m_nPriority = rkLoader.LoadInt_e( "order", -1 );
	m_nShopMarkType = rkLoader.LoadInt_e( "mark", -1 );
	m_nLimitDate = rkLoader.LoadInt_e( "limit_date", -1 );
	m_nMainManual = rkLoader.LoadInt_e( "manual", 0 );
	m_nGroup = rkLoader.LoadInt_e( "group", 0 );
	m_bActive = rkLoader.LoadBool_e( "active", false );	
	m_nCostumeShopCode  = rkLoader.LoadInt_e( "costume_shop_code", 0 );
	m_nGradeType = rkLoader.LoadInt_e( "grade_type", 0 );
	m_nRecommandedShopOrder = rkLoader.LoadInt_e( "recommended_order", ORDER_DEFAULT );
	m_nNeedLevel = rkLoader.LoadInt_e( "need_level", 0 );
	m_nSubscriptionType = rkLoader.LoadInt_e( "subscription_type", SUBSCRIPTION_NONE );

	LoadCostumeShopSubInfo(rkLoader);
}

void CostumeShopInfo::LoadCostumeShopSubInfo( ioINILoader &rkLoader )
{
	int nDefaultType = rkLoader.LoadInt_e( "default_type", 0 );
	if ( nDefaultType != 0 )
		m_nDefaultType = nDefaultType;

	bool bSetPreview = rkLoader.LoadBool_e( "set_preview", false );
	if ( bSetPreview )
		m_bSetPreview = bSetPreview;

	bool bPackage = rkLoader.LoadBool_e( "package", false );
	if ( bPackage )
		m_bPackage = bPackage;

	int nNeedCash = rkLoader.LoadInt_e( "need_cash", 0 );
	if ( nNeedCash != 0 )
		m_nNeedCash = nNeedCash;

	int nBonusPeso = rkLoader.LoadInt_e( "bonus_peso", 0 );
	if ( nBonusPeso != 0 )
		m_nBonusPeso = nBonusPeso;

	int nSellPeso = rkLoader.LoadInt_e( "sell_peso", 0 );
	if ( nSellPeso != 0 )
		m_nSellPeso = nSellPeso;
	
	enum 
	{
		MAX_LOOP = 100,
	};

	int nNeedPeso  = 0;
	int nPeriod    = 0;
	
	char szKey[MAX_PATH];
	for (int i=0; i<MAX_LOOP; i++)
	{
		g_SaleMgr.LoadINI( true, rkLoader, ioSaleManager::IT_COSTUME, m_nCostumeShopCode, i, i );

		StringCbPrintf_e( szKey, sizeof( szKey ), "need_peso%d", i+1 );
		nNeedPeso = rkLoader.LoadInt( szKey, -1 );

		StringCbPrintf_e( szKey, sizeof( szKey ), "period%d", i+1 );
		nPeriod      = rkLoader.LoadInt( szKey, -1 );	

		if( nNeedPeso == -1 && nPeriod == -1 )
			break;

		if ( i == 0 )
		{
			m_vNeedPeso.clear();
			m_vPeriod.clear();
		}
		m_vNeedPeso.push_back( nNeedPeso );
		m_vPeriod.push_back( nPeriod );
	}

	// 캐쉬 기간제 날짜 표시 위해서
	if( !m_vPeriod.empty() )
	{
		if( m_vPeriod[0] > 0 )
		{			
			nPeriod = rkLoader.LoadInt_e( "period_time1_value", -1 ); 
			if( nPeriod != - 1 )
				m_vPeriod.push_back( nPeriod );
		}
	}

	int nCnt = rkLoader.LoadInt_e( "item_cnt", 0 );
	for( int i=0; i<nCnt; ++i )
	{
		if ( i == 0 )
			m_CostumeShopSubInfoList.clear();

		CostumeShopSubInfo kInfo;

		wsprintf_e( szKey, "item%d_code", i+1 );
		kInfo.m_nCostumeCode = rkLoader.LoadInt( szKey, 0 );
		
		wsprintf_e( szKey, "item%d_mark", i+1 );
		kInfo.m_nShopMarkType = rkLoader.LoadInt( szKey, 0 );  // SHOP_MARK_TYPE_NONE

		wsprintf_e( szKey, "item%d_limit_date", i+1 );
		kInfo.m_nLimitDate = rkLoader.LoadInt( szKey, 0 );
		
		m_CostumeShopSubInfoList.push_back( kInfo );
	}

	if ( !m_CostumeShopSubInfoList.empty() && m_szIconName.IsEmpty() )
		m_szIconName = g_ItemMaker.GetItemLargeIcon( m_CostumeShopSubInfoList[0].m_nCostumeCode, 0 );
}

int CostumeShopInfo::GetNeedPeso( int nArray ) const
{
	if( !COMPARE( nArray, 0, (int)m_vNeedPeso.size() ) )
		return 0;

	return m_vNeedPeso[nArray];
}

int CostumeShopInfo::GetPeriod( int nArray ) const
{
	if( !COMPARE( nArray, 0, (int)m_vPeriod.size() ) )
		return 0;

	return m_vPeriod[nArray];
}

void CostumeShopInfo::SetNeedPeso( int nNeedPeso, int nArray )
{
	if( !COMPARE( nArray, 0, (int)m_vNeedPeso.size() ) )
		return;

	m_vNeedPeso[nArray] = nNeedPeso;
}

bool CostumeShopInfo::GetCostumeSubInfo( int nArray, CostumeShopSubInfo &sCostumeShopSubInfo )
{
	if ( !COMPARE( nArray, 0, (int)m_CostumeShopSubInfoList.size() ) ) 
		return false;

	sCostumeShopSubInfo = m_CostumeShopSubInfoList[nArray];

	return true;
}

int CostumeShopInfo::GetCostumeSubInfoCnt()
{
	return (int)m_CostumeShopSubInfoList.size();
}

void CostumeShopInfo::SetDefaultData( const CostumeShopInfo* pInfo )
{
	if ( !pInfo )
		return;
	
	m_CostumeShopSubInfoList.clear();
	m_vNeedPeso.clear();
	m_vPeriod.clear();

	m_nDefaultType = pInfo->m_nDefaultType;
	m_bSetPreview = pInfo->m_bSetPreview;
	m_bPackage = pInfo->m_bPackage;
	m_nNeedCash = pInfo->m_nNeedCash;
	m_nBonusPeso = pInfo->m_nBonusPeso;
	m_nSellPeso = pInfo->m_nSellPeso;

	m_CostumeShopSubInfoList = pInfo->m_CostumeShopSubInfoList;
	m_vNeedPeso = pInfo->m_vNeedPeso;
	m_vPeriod = pInfo->m_vPeriod;
}

///////////////////////////////////////////////////////////////////////////

template<> ioCostumeShopInfoManager* Singleton< ioCostumeShopInfoManager >::ms_Singleton = 0;

ioCostumeShopInfoManager::ioCostumeShopInfoManager()
{
}

ioCostumeShopInfoManager::~ioCostumeShopInfoManager()
{
	ClearAllInfo();
}

ioCostumeShopInfoManager& ioCostumeShopInfoManager::GetSingleton()
{
	return Singleton< ioCostumeShopInfoManager >::GetSingleton();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioCostumeShopInfoManager::LoadCostumeMachineInfo()
{
	ClearAllInfo();

	char szBuf[MAX_PATH];

	ioINILoader_e kLoader( "config/sp2_costume_shop_info.ini" );
	kLoader.SetTitle_e( "common_info" );

	m_fMortmainItemSellPrice = kLoader.LoadFloat_e( "mortmain_item_sell", 0.0f );
	m_fTimeItemSellPrice = kLoader.LoadFloat_e( "time_item_sell", 0.0f );

	int nDefaultCnt = kLoader.LoadInt_e( "default_cnt", 0 );
	int nCostumeCnt = kLoader.LoadInt_e( "costume_cnt", 0 );

	m_DefaultCostumeShopInfoList.reserve( nDefaultCnt );	
	for( int i=0; i<nDefaultCnt; i++ )
	{
		wsprintf_e( szBuf, "default%d", i+1 );
		kLoader.SetTitle( szBuf );

		CostumeShopInfo *pInfo = new CostumeShopInfo;
		if( !pInfo )
		{
			LOG.PrintTimeAndLog( 0, "ioCostumeShopInfoManager::LoadCostumeMachineInfo - %s pInfo == NULL.", kLoader.GetTitle() );
			break;
		}
		pInfo->LoadCostumeShopSubInfo( kLoader );

		m_DefaultCostumeShopInfoList.push_back( pInfo );
	}

	m_CostumeShopInfoList.reserve( nCostumeCnt );	
	for( int i=0; i<nCostumeCnt; i++ )
	{
		wsprintf_e( szBuf, "costume_machine%d", i+1 );
		kLoader.SetTitle( szBuf );

		int nShopCode = kLoader.LoadInt_e( "costume_shop_code", 0 );
		if( nShopCode == 0 )
			continue;

		CostumeShopInfo *pInfo = new CostumeShopInfo;
		if( !pInfo )
		{
			LOG.PrintTimeAndLog( 0, "ioCostumeShopInfoManager::LoadCostumeMachineInfo - %s pInfo == NULL.", kLoader.GetTitle() );
			break;
		}

		int nDefaultType = kLoader.LoadInt_e( "default_type", 0 );
		if ( nDefaultType > 0 )
			pInfo->SetDefaultData( GetDefaultCostumeMachineInfo( nDefaultType ) );

		pInfo->LoadCostumeShopBasicInfo( kLoader );
		
		m_CostumeShopInfoList.push_back( pInfo );
	}
}

void ioCostumeShopInfoManager::ClearAllInfo()
{
	CostumeShopInfoList::iterator iter = m_DefaultCostumeShopInfoList.begin();
	for( ; iter!=m_DefaultCostumeShopInfoList.end() ; ++iter )
	{
		CostumeShopInfo *pInfo = (*iter);
		SAFEDELETE( pInfo );
	}

	m_DefaultCostumeShopInfoList.clear();

	iter = m_CostumeShopInfoList.begin();
	for( ; iter!=m_CostumeShopInfoList.end() ; ++iter )
	{
		CostumeShopInfo *pInfo = (*iter);
		SAFEDELETE( pInfo );
	}

	m_CostumeShopInfoList.clear();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

int ioCostumeShopInfoManager::CalculateOrder( int nCostumeShopCode )
{
	int nOrder = 0;
	int nSubType = nCostumeShopCode/DEFAULT_BASIC_COSTUME_CODE;

	// 레어 가중치 [X]0000
	if( ( (nCostumeShopCode/1000)%10 ) == 1 )
		nOrder = 10000;
	else
		nOrder = 0;

	// 용병 코드 가중치 0[XXX]0
	nOrder += ( ( nCostumeShopCode%1000 ) * 10 );
	
	// 무, 갑, 투, 망 가중치 0000[X]
	if( nSubType == 0 )
		nOrder += 4;
	else if( nSubType == 1 )
		nOrder += 3;
	else if( nSubType == 2 )
		nOrder += 2;
	else if( nSubType == 3 )
		nOrder += 1;

	return nOrder;
}

CostumeShopInfo* ioCostumeShopInfoManager::GetCostumeMachineInfoByArray( int nArray )
{
	if( COMPARE( nArray, 0, (int)m_CostumeShopInfoList.size() ) )
		return m_CostumeShopInfoList[nArray];

	return NULL;
}

CostumeShopInfo* ioCostumeShopInfoManager::GetCostumeMachineInfo( int nCode )
{
	for ( int i=0; i<(int)m_CostumeShopInfoList.size(); ++i )
	{
		CostumeShopInfo *pInfo = m_CostumeShopInfoList[i];
		if ( pInfo && pInfo->GetCostumeShopCode() == nCode )
			return m_CostumeShopInfoList[i];
	}

	return NULL;
}

CostumeShopInfo* ioCostumeShopInfoManager::GetDefaultCostumeMachineInfo( int nDefaultType )
{
	for ( int i=0; i<(int)m_DefaultCostumeShopInfoList.size(); ++i )
	{
		CostumeShopInfo *pInfo = m_DefaultCostumeShopInfoList[i];
		if ( pInfo && pInfo->GetCostumeDefault() == nDefaultType )
			return m_DefaultCostumeShopInfoList[i];
	}

	return NULL;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ioCostumeShopInfoManager::GetCostumeItemList( int nCostumeShopCode, CostumeShopSubInfoList &rkList )
{
	for ( int i=0; i<(int)m_CostumeShopInfoList.size(); ++i )
	{
		CostumeShopInfo *pInfo = m_CostumeShopInfoList[i];
		if ( pInfo && pInfo-> GetCostumeShopCode() == nCostumeShopCode )
		{
			int nSubInfoCnt = pInfo->GetCostumeSubInfoCnt();
			for( int i=0; i<nSubInfoCnt; ++i )
			{
				CostumeShopSubInfo kInfo;
				if ( !pInfo->GetCostumeSubInfo( i, kInfo ) )
					continue;

				CostumeShopSubInfo kSort;
				kSort.m_nCostumeCode = kInfo.m_nCostumeCode;
				kSort.m_nShopMarkType = kInfo.m_nShopMarkType;

				if( kSort.m_nShopMarkType != SHOP_MARK_TYPE_NONE )
					kSort.m_nOrder = ( kSort.m_nShopMarkType*1000000 ) + kSort.m_nCostumeCode;
				else
					kSort.m_nOrder = CalculateOrder( kSort.m_nCostumeCode );

				rkList.push_back( kSort );
			}
			return true;
		}			
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

int ioCostumeShopInfoManager::GetCostumeMachineCnt()
{
	return (int)m_CostumeShopInfoList.size();;
}

int ioCostumeShopInfoManager::GetCostumeRecommandedCnt()
{
	int iCount = 0;
	CostumeShopInfoList::iterator iter = m_CostumeShopInfoList.begin();
	for( ; iter != m_CostumeShopInfoList.end(); ++iter )
	{
		CostumeShopInfo *pInfo = (*iter);
		if( !pInfo )
			continue;

		if( pInfo->GetRecommandedShopOrder() != ORDER_DEFAULT )
			iCount++;
	}

	return iCount;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

float ioCostumeShopInfoManager::GetResellMortmainCostumePeso()
{
	return m_fMortmainItemSellPrice; 
}

float ioCostumeShopInfoManager::GetResellTimeCostumeTimePeso()
{
	return m_fTimeItemSellPrice; 
}