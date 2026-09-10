

#include "stdafx.h"

#include "ioExtraItemInfoManager.h"

#include "ioSaleManager.h"

////////////////////////////////////////////////////////////////////////////
RandomMachineInfo::RandomMachineInfo()
{
	m_iMachineCode = 0;
	m_iGroup = 1;	// 보급, 무기, 갑옷, 투구, 망토 구분

	m_iNeedCash  = 0;
	m_iBonusPeso = 0;
	m_iSellPeso  = 0;

	m_iNeedLevel = 0;
	m_iPriority = 0;
	m_iRecommandedShopOrder = 0;

	m_iMainManual = 0;

	m_iShopMarkType = 0; // SHOP_MARK_TYPE_NONE;
	m_iLimitDate	= 0;
	

	m_bPackage = false;
	m_bActive  = true;

	m_iSubscriptionType = SUBSCRIPTION_NONE;

	m_eDefaultType = MT_NONE;
}

RandomMachineInfo::~RandomMachineInfo()
{
	m_vItemList.clear();
	m_vNeedPeso.clear();
	m_vPeriod.clear();
}

int RandomMachineInfo::GetNeedPeso( int iArray ) const
{
	if( !COMPARE( iArray, 0, (int)m_vNeedPeso.size() ) )
		return 0;

	return m_vNeedPeso[iArray];
}

int RandomMachineInfo::GetPeriod( int iArray ) const
{
	if( !COMPARE( iArray, 0, (int)m_vPeriod.size() ) )
		return 0;

	return m_vPeriod[iArray];
}

void RandomMachineInfo::SetNeedPeso( int iNeedPeso, int iArray )
{
	if( !COMPARE( iArray, 0, (int)m_vNeedPeso.size() ) )
		return;

	m_vNeedPeso[iArray] = iNeedPeso;
}

void RandomMachineInfo::LoadMachineBasicInfo( ioINILoader &rkLoader )
{
	char szName[MAX_PATH]="", szKey[MAX_PATH]="";
	rkLoader.LoadString_e( "name", "", szName, MAX_PATH );
	if( strcmp( szName , "" ) != 0 )
		m_Name = szName;

	int iMachineCode  = rkLoader.LoadInt_e( "machine_code", -1 );
	if( iMachineCode != -1 )
		m_iMachineCode = iMachineCode;

	int iGroup        = rkLoader.LoadInt_e( "group", -1 );
	if( iGroup != -1 )
		m_iGroup = iGroup;

	int iNeedLevel    = rkLoader.LoadInt_e( "need_level", -1 );
	if( iNeedLevel != -1 )
		m_iNeedLevel = iNeedLevel;

	int iMainManual   = rkLoader.LoadInt_e( "manual", -1 );
	if( iMainManual != -1 )
		m_iMainManual = iMainManual;

	int iPriority     = rkLoader.LoadInt_e( "order", -1 );
	if( iPriority != -1 )
		m_iPriority = iPriority;

	m_iRecommandedShopOrder = rkLoader.LoadInt_e( "recommended_order", ORDER_DEFAULT );

	int iShopMarkType = rkLoader.LoadInt_e( "mark", -1 ); // SHOP_MARK_TYPE_NONE
	if( iShopMarkType != -1 )
		m_iShopMarkType = iShopMarkType;

	m_iLimitDate = rkLoader.LoadInt_e( "limit_date", 0 );

	int iPackage      = rkLoader.LoadInt_e( "package", -1 );
	if( iPackage != -1 )
	{
		if( iPackage == 0 )
			m_bPackage = false;
		else
			m_bPackage = true;
	}

	int iActive       = rkLoader.LoadInt_e( "active", -1 );
	if( iActive != -1 )
	{
		if( iActive == 0 )
			m_bActive = false;
		else
			m_bActive = true;
	}

	int iNeedCash     = rkLoader.LoadInt_e( "need_cash", -1 );
	if( iNeedCash != -1 )
		m_iNeedCash = iNeedCash;

	int iBonusPeso    = rkLoader.LoadInt_e( "bonus_peso", -1 );
	if( iBonusPeso != -1 )
		m_iBonusPeso = iBonusPeso;

	int iSellPeso     = rkLoader.LoadInt_e( "sell_peso", -1 );
	if( iSellPeso != -1 )
		m_iSellPeso = iSellPeso;
	
	rkLoader.LoadString_e( "icon", "", szName, MAX_PATH );
	if( strcmp( szName , "" ) != 0 )
		m_IconName = szName;

	m_iSubscriptionType = rkLoader.LoadInt_e( "subscription_type", SUBSCRIPTION_NONE );
	m_nGradeType  = rkLoader.LoadInt_e( "grade_type", 0 );

	enum 
	{
		MAX_LOOP = 100,
	};

	int iNeedPeso  = 0;
	int iPeriod    = 0;

	for (int i = 0; i < MAX_LOOP ; i++)
	{
		g_SaleMgr.LoadINI( true, rkLoader, ioSaleManager::IT_EXTRA_BOX, m_iMachineCode, i, i );

		StringCbPrintf_e( szKey, sizeof( szKey ), "need_peso%d", i+1 );
		iNeedPeso = rkLoader.LoadInt( szKey, -1 );

		StringCbPrintf_e( szKey, sizeof( szKey ), "period%d", i+1 );
		iPeriod      = rkLoader.LoadInt( szKey, -1 );	

		if( iNeedPeso == -1 && iPeriod == -1 )
			break;

		if( i == 0 )
		{
			m_vNeedPeso.clear();
			m_vPeriod.clear();
		}

		m_vNeedPeso.push_back( iNeedPeso );
		m_vPeriod.push_back( iPeriod );
	}

	// 캐쉬 기간제 날짜 표시 위해서
	if( !m_vPeriod.empty() )
	{
		if( m_vPeriod[0] > 0 )
		{
			
			iPeriod = rkLoader.LoadInt_e( "period_time1_value", -1 ); 
			if( iPeriod != - 1 )
			{
				m_vPeriod.push_back( iPeriod );
			}
		}
	}

	if( !g_ExtraItemInfoMgr.IsTestLOG() )
		return;
	LOG.PrintTimeAndLog( 0, "MachineCode:%d ------------------------", m_iMachineCode );
	LOG.PrintTimeAndLog( 0, "DefaultType:%d", (int) m_eDefaultType );
	LOG.PrintTimeAndLog( 0, "Name:%s", m_Name.c_str() );
	LOG.PrintTimeAndLog( 0, "Group:%d", m_iGroup );
	LOG.PrintTimeAndLog( 0, "NeedLevel:%d", m_iNeedLevel );
	LOG.PrintTimeAndLog( 0, "MainManual:%d", m_iMainManual );
	LOG.PrintTimeAndLog( 0, "Priority:%d", m_iPriority );
	LOG.PrintTimeAndLog( 0, "PRecommandedShopOrder:%d", m_iRecommandedShopOrder );	
	LOG.PrintTimeAndLog( 0, "ShopMarkType:%d", m_iShopMarkType );
	LOG.PrintTimeAndLog( 0, "LimitDate:%d", m_iLimitDate );
	LOG.PrintTimeAndLog( 0, "Package:%d", (int)m_bPackage );
	LOG.PrintTimeAndLog( 0, "Active:%d", (int)m_bActive );
	LOG.PrintTimeAndLog( 0, "NeedCash:%d", m_iNeedCash );
	LOG.PrintTimeAndLog( 0, "BonusPeso:%d", m_iBonusPeso );
	LOG.PrintTimeAndLog( 0, "SellPeso:%d", m_iSellPeso );
	LOG.PrintTimeAndLog( 0, "IconName:%s", m_IconName.c_str() );

	int iSize = m_vNeedPeso.size();
	for (int i = 0; i < iSize ; i++)
	{
		int &riValue = m_vNeedPeso[i];
		LOG.PrintTimeAndLog( 0, "NeedPeso%d:%d", i+1, riValue );
	}

	iSize = m_vPeriod.size();
	for (int i = 0; i < iSize ; i++)
	{
		int &riValue = m_vPeriod[i];
		LOG.PrintTimeAndLog( 0, "Period%d:%d", i+1, riValue );
	}
}

void RandomMachineInfo::LoadMachineRandomInfo( ioINILoader &rkLoader, bool bItemLoadAll )
{
	char szKey[MAX_PATH] = "";
	int iCnt = rkLoader.LoadInt_e( "item_cnt", 0 );
	for( int i=0; i < iCnt; ++i )
	{
		ItemListInfo kInfo;

		wsprintf_e( szKey, "item%d_rate", i+1 );
		if( rkLoader.LoadInt( szKey, 0 ) == 0 )
			continue;

		wsprintf_e( szKey, "item%d_code", i+1 );
		kInfo.m_iItemCode = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "item%d_mark", i+1 );
		kInfo.m_iShopMarkType = rkLoader.LoadInt( szKey, 0 );  // SHOP_MARK_TYPE_NONE

		bool bNewItem = false;
		if( !bItemLoadAll )
		{
			bool bExist = false;
			for(ItemListInfoVector::iterator iter = m_vItemList.begin(); iter != m_vItemList.end(); ++iter)
			{
			    ItemListInfo &rkInfo = (*iter);
				if( rkInfo.m_iItemCode != kInfo.m_iItemCode  )
					continue;

				rkInfo.m_iShopMarkType	= kInfo.m_iShopMarkType;
				bExist = true;
				break;
			}

			if( !bExist )
				bNewItem = true;
		}

		if( bItemLoadAll || bNewItem )
			m_vItemList.push_back( kInfo );
	}

	if( !g_ExtraItemInfoMgr.IsTestLOG() )
		return;

	int iArray = 0;
	for(ItemListInfoVector::iterator iter = m_vItemList.begin(); iter != m_vItemList.end(); ++iter)
	{
		ItemListInfo &rkInfo = (*iter);
		iArray++;
		LOG.PrintTimeAndLog( 0, "Item%d:%d:%d", iArray, rkInfo.m_iItemCode, rkInfo.m_iShopMarkType );
	}
}

///////////////////////////////////////////////////////////////////////////
template<> ioExtraItemInfoManager* Singleton< ioExtraItemInfoManager >::ms_Singleton = 0;

ioExtraItemInfoManager::ioExtraItemInfoManager()
{
	m_bTestRandomItem = false;
}

ioExtraItemInfoManager::~ioExtraItemInfoManager()
{
	ClearAllInfo();
}

void ioExtraItemInfoManager::LoadRandomMachineInfo()
{
	ClearAllInfo();

	ioINILoader_e kLoader( "config/sp2_extraitem_info.ini" );
	kLoader.SetTitle_e( "common_info" );

	m_iLevelLimitConst = kLoader.LoadInt_e( "item_limit_const", 0 );
	m_fMortmainItemSell = kLoader.LoadFloat_e( "mortmain_item_sell", 0.0f );
	m_fMortmainItemSellRate = kLoader.LoadFloat_e( "mortmain_item_sell_rate", FLOAT1 );
	m_fMortmainItemSellMax = kLoader.LoadFloat_e( "mortmain_item_sell_max", 0.0f );
	m_fTimeItemSell = kLoader.LoadFloat_e( "time_item_sell", 0.0f );
	m_fTimeItemSellRate = kLoader.LoadFloat_e( "time_item_sell_rate", FLOAT1 );
	m_fTimeItemSellMax = kLoader.LoadFloat_e( "time_item_sell_max", 0.0f );
	m_bTestRandomItem = kLoader.LoadBool_e( "test_random_item", false );

	int iCnt = kLoader.LoadInt_e( "random_machine_cnt", 0 );
	m_MachineInfoList.reserve( iCnt );

	enum 
	{
		MAX_LOOP = 100,
	};
	char szTitle[MAX_PATH]="";
	for (int i = 0; i < MAX_LOOP ; i++)
	{
		StringCbPrintf_e( szTitle, sizeof( szTitle ), "default%d", i+1 );
		kLoader.SetTitle( szTitle );

		int iDefaultType = kLoader.LoadInt_e( "type", 0 );
		if( iDefaultType == 0 )
			break;

		RandomMachineInfo *pInfo = new RandomMachineInfo;
		if( !pInfo )
		{
			LOG.PrintTimeAndLog( 0, "ioExtraItemInfoManager::LoadRandomMachineInfo - %s pInfo == NULL.", kLoader.GetTitle() );
			break;
		}
		pInfo->m_eDefaultType = (RandomMachineInfo::MachineType) iDefaultType;
		pInfo->LoadMachineBasicInfo( kLoader );
		pInfo->LoadMachineRandomInfo( kLoader, true );
		m_DefaultMachineInfoList.push_back( pInfo );
	}

	for( int i=0; i < iCnt; i++ )
	{
		wsprintf_e( szTitle, "random_machine%d", i+1 );
		kLoader.SetTitle( szTitle );

		int iMachineCode = kLoader.LoadInt_e( "machine_code", 0 );
		if( iMachineCode == 0 )
		{
			continue;
		}

		RandomMachineInfo *pInfo = new RandomMachineInfo;
		if( !pInfo )
		{
			LOG.PrintTimeAndLog( 0, "ioExtraItemInfoManager::LoadRandomMachineInfo - %s pInfo == NULL.", kLoader.GetTitle() );
			break;
		}

		CopyDefaultMachineBasicInfo( kLoader, pInfo );
		SetAutoMachineBasicInfo( kLoader, pInfo );
		pInfo->LoadMachineBasicInfo( kLoader );
		bool bLoaded = CopyDefaultMachineRandomInfo( kLoader, pInfo );
		pInfo->LoadMachineRandomInfo( kLoader, !bLoaded );

		m_MachineInfoList.push_back( pInfo );
		m_InfoMap.insert( MachineInfoMap::value_type( pInfo->m_iMachineCode, pInfo ) );
	}
}

void ioExtraItemInfoManager::ClearAllInfo()
{
	m_InfoMap.clear();

	RandomMachineInfoVector::iterator iter1, iEnd1;
	iEnd1 = m_MachineInfoList.end();
	for( iter1=m_MachineInfoList.begin() ; iter1!=iEnd1 ; ++iter1 )
	{
		delete *iter1;
	}
	m_MachineInfoList.clear();

	RandomMachineInfoVector::iterator iter2, iEnd2;
	iEnd2 = m_DefaultMachineInfoList.end();
	for( iter2=m_DefaultMachineInfoList.begin() ; iter2!=iEnd2 ; ++iter2 )
	{
		delete *iter2;
	}
	m_DefaultMachineInfoList.clear();
}

RandomMachineInfo* ioExtraItemInfoManager::GetRandomMachineInfoByArray( int iArray )
{
	int iSize = m_MachineInfoList.size();

	if( COMPARE( iArray, 0, iSize ) )
		return m_MachineInfoList[iArray];

	return NULL;
}

RandomMachineInfo* ioExtraItemInfoManager::GetRandomMachineInfo( int iCode )
{
	MachineInfoMap::iterator iter = m_InfoMap.find( iCode );
	if( iter != m_InfoMap.end() )
	{
		return iter->second;
	}

	return NULL;
}

int ioExtraItemInfoManager::GetRadomMachineCnt()
{
	int iSize = m_MachineInfoList.size();

	return iSize;
}

bool ioExtraItemInfoManager::GetRandomItemList( int iMachineCode, ItemListInfoSortVector &rkList )
{
	MachineInfoMap::iterator iter = m_InfoMap.find( iMachineCode );
	if( iter != m_InfoMap.end() )
	{
		int iCnt = iter->second->m_vItemList.size();
		for( int i=0; i < iCnt; ++i )
		{
			ItemListInfo kInfo;
			kInfo = iter->second->m_vItemList[i];

			ItemListInfoSort kSort;
			kSort.m_iItemCode     = kInfo.m_iItemCode;
			kSort.m_iShopMarkType = kInfo.m_iShopMarkType;
			if( kSort.m_iShopMarkType != SHOP_MARK_TYPE_NONE )
				kSort.m_iOrder = ( kSort.m_iShopMarkType*1000000 ) + kSort.m_iItemCode;
			else
			{
				 kSort.m_iOrder = CalculateOrder( kSort.m_iItemCode );
			}
			rkList.push_back( kSort );
		}
		return true;
	}

	return false;
}


ioExtraItemInfoManager& ioExtraItemInfoManager::GetSingleton()
{
	return Singleton< ioExtraItemInfoManager >::GetSingleton();
}


void ioExtraItemInfoManager::CopyDefaultMachineBasicInfo( IN ioINILoader &rkLoader, OUT RandomMachineInfo *pInfo )
{
	if( !pInfo )
		return;

	RandomMachineInfo::MachineType eType = (RandomMachineInfo::MachineType)rkLoader.LoadInt_e( "type", 0 );
	if( eType == MT_NONE )
		return;

	if( m_DefaultMachineInfoList.empty() )
		return;

	RandomMachineInfo::MachineType eCheckType = RandomMachineInfo::MT_NONE;
	if( COMPARE( (int) eType, RandomMachineInfo::MT_WEAPON, RandomMachineInfo::MT_RARE_ALL+1 ) )
	{
		eCheckType = RandomMachineInfo::MT_ALL;
	}
	else if( eType == RandomMachineInfo::MT_HIGH_EXTRA )
	{
		eCheckType = RandomMachineInfo::MT_HIGH_EXTRA;
	}
	else if( eType == RandomMachineInfo::MT_LIMITED_EXTRA )
	{
		eCheckType = RandomMachineInfo::MT_LIMITED_EXTRA;
	}

	for(RandomMachineInfoVector::iterator iter = m_DefaultMachineInfoList.begin(); iter != m_DefaultMachineInfoList.end(); ++iter)
	{
		RandomMachineInfo *pDefaultInfo = (*iter);
		if( !pDefaultInfo )
			continue;
		if( pDefaultInfo->m_eDefaultType != eCheckType )
			continue; 

		pInfo->m_iMachineCode			= pDefaultInfo->m_iMachineCode;
		pInfo->m_Name					= pDefaultInfo->m_Name;
		pInfo->m_iGroup					= pDefaultInfo->m_iGroup;
		pInfo->m_iNeedLevel				= pDefaultInfo->m_iNeedLevel;
		pInfo->m_iMainManual			= pDefaultInfo->m_iMainManual;
		pInfo->m_iPriority				= pDefaultInfo->m_iPriority;
		pInfo->m_iShopMarkType			= pDefaultInfo->m_iShopMarkType;
		pInfo->m_iLimitDate				= pDefaultInfo->m_iLimitDate;
		pInfo->m_bPackage				= pDefaultInfo->m_bPackage;
		pInfo->m_bActive				= pDefaultInfo->m_bActive;
		pInfo->m_iNeedCash				= pDefaultInfo->m_iNeedCash;
		pInfo->m_iBonusPeso				= pDefaultInfo->m_iBonusPeso;
		pInfo->m_iSellPeso				= pDefaultInfo->m_iSellPeso;
		pInfo->m_IconName				= pDefaultInfo->m_IconName;
		pInfo->m_iRecommandedShopOrder	= pDefaultInfo->m_iRecommandedShopOrder;

		if( !pDefaultInfo->m_vNeedPeso.empty() )
			pInfo->m_vNeedPeso = pDefaultInfo->m_vNeedPeso;
		if( !pDefaultInfo->m_vPeriod.empty() )
			pInfo->m_vPeriod = pDefaultInfo->m_vPeriod;

		return;
	}
}

void ioExtraItemInfoManager::SetAutoMachineBasicInfo( IN ioINILoader &rkLoader, OUT RandomMachineInfo *pInfo )
{
	if( !pInfo )
		return;

	RandomMachineInfo::MachineType eType = (RandomMachineInfo::MachineType)rkLoader.LoadInt_e( "type", 0 );
	if( eType != RandomMachineInfo::MT_HIGH_EXTRA &&
		eType != RandomMachineInfo::MT_LIMITED_EXTRA )
		return;

	int iItemCode = rkLoader.LoadInt_e( "item1_code", 0 );
	if( iItemCode == 0 )
		return;

	const ioItem *pItem = g_ItemMaker.GetItemConst( iItemCode, __FUNCTION__ );
	if( !pItem )
		return;
	
	pInfo->m_Name      = pItem->GetName();
	pInfo->m_iGroup    = ( iItemCode/DEFAULT_BASIC_ITEM_CODE ) + 2;
	pInfo->m_iPriority = DEFAULT_BASIC_ITEM_CODE - CalculateOrder( iItemCode );
	pInfo->m_IconName  = pItem->GetItemLargeIconName();
}

bool ioExtraItemInfoManager::CopyDefaultMachineRandomInfo( IN ioINILoader &rkLoader, OUT RandomMachineInfo *pInfo )
{
	if( !pInfo )
		return false;

	RandomMachineInfo::MachineType eType = (RandomMachineInfo::MachineType)rkLoader.LoadInt_e( "type", 0 );
	if( eType == RandomMachineInfo::MT_NONE )
		return false;

	if( m_DefaultMachineInfoList.empty() )
		return false;

	RandomMachineInfo::MachineType eCheckType = RandomMachineInfo::MT_NONE;
	if( COMPARE( (int) eType, RandomMachineInfo::MT_WEAPON, RandomMachineInfo::MT_RARE_ALL+1 ) )
	{
		eCheckType = RandomMachineInfo::MT_ALL;
	}
	else if( eType == RandomMachineInfo::MT_HIGH_EXTRA )
	{
		eCheckType = RandomMachineInfo::MT_HIGH_EXTRA;
	}
	else if( eType == RandomMachineInfo::MT_LIMITED_EXTRA )
	{
		eCheckType = RandomMachineInfo::MT_LIMITED_EXTRA;
	}

	for(RandomMachineInfoVector::iterator iter = m_DefaultMachineInfoList.begin(); iter != m_DefaultMachineInfoList.end(); ++iter)
	{
		RandomMachineInfo *pDefaultList = (*iter);
		if( !pDefaultList )
			continue;
		if( pDefaultList->m_eDefaultType != eCheckType )
			continue;

		// item
		enum { MIN_ITEM_SIZE = 100, };
		pInfo->m_vItemList.clear();
		pInfo->m_vItemList.reserve( MIN_ITEM_SIZE );

		for(ItemListInfoVector::iterator iter2 = pDefaultList->m_vItemList.begin(); iter2 != pDefaultList->m_vItemList.end(); ++iter2)
		{
			ItemListInfo kItem = (*iter2);
			if( !IsRightExtraItem( eType, kItem.m_iItemCode ) )
				continue;
			pInfo->m_vItemList.push_back( kItem );
		}

		return true; //////////////////////////////////////////////////////
	}

	return false;
}

bool ioExtraItemInfoManager::IsRightExtraItem( RandomMachineInfo::MachineType eType, int iItemCode )
{
	if( iItemCode <= 0 )
		return false;

	if( eType == RandomMachineInfo::MT_ALL )
		return true;
	else if( eType == RandomMachineInfo::MT_WEAPON )
	{
		if( (iItemCode/DEFAULT_BASIC_ITEM_CODE) == 0 )
			return true;
	}
	else if( eType == RandomMachineInfo::MT_ARMOR )
	{
		if( (iItemCode/DEFAULT_BASIC_ITEM_CODE) == 1 )
			return true;
	}
	else if( eType == RandomMachineInfo::MT_HELMET )
	{
		if( (iItemCode/DEFAULT_BASIC_ITEM_CODE) == 2 )
			return true;
	}
	else if( eType == RandomMachineInfo::MT_CLOAK )
	{
		if( (iItemCode/DEFAULT_BASIC_ITEM_CODE) == 3 )
			return true;
	}
	else if( eType == RandomMachineInfo::MT_RARE_ALL )
	{
		if( (iItemCode/1000)%10 == 1 )
			return true;
	}
	else if( eType == RandomMachineInfo::MT_RARE_WEAPON )
	{
		if( (iItemCode/1000)%10 == 1 )
		{
			if( (iItemCode/DEFAULT_BASIC_ITEM_CODE) == 0 )
				return true;
		}
	}
	else if( eType == RandomMachineInfo::MT_RARE_ARMOR )
	{
		if( (iItemCode/1000)%10 == 1 )
		{
			if( (iItemCode/DEFAULT_BASIC_ITEM_CODE) == 1 )
				return true;
		}
	}
	else if( eType == RandomMachineInfo::MT_RARE_HELMET )
	{
		if( (iItemCode/1000)%10 == 1 )
		{
			if( (iItemCode/DEFAULT_BASIC_ITEM_CODE) == 2 )
				return true;
		}
	}
	else if( eType == RandomMachineInfo::MT_RARE_CLOAK )
	{
		if( (iItemCode/1000)%10 == 1 )
		{
			if( (iItemCode/DEFAULT_BASIC_ITEM_CODE) == 3 )
				return true;
		}
	}
	else if( eType == RandomMachineInfo::MT_HIGH_EXTRA )
		return false;

	return false;
}

int ioExtraItemInfoManager::CalculateOrder( int iItemCode )
{
	int iOrder = 0;

	// 레어 가중치 [X]0000
	if( ( (iItemCode/1000)%10 ) == 1 )
		iOrder = 10000;
	else
		iOrder = 0;

	// 용병 코드 가중치 0[XXX]0
	iOrder += ( ( iItemCode%1000 ) * 10 );

	// 무, 갑, 투, 망 가중치 0000[X]
	int iType = iItemCode/DEFAULT_BASIC_ITEM_CODE;
	if( iType == 0 )
		iOrder += 4;
	else if( iType == 1 )
		iOrder += 3;
	else if( iType == 2 )
		iOrder += 2;
	else if( iType == 3 )
		iOrder += 1;

	return iOrder;
}

int ioExtraItemInfoManager::GetExtraItemRecommandedCnt()
{
	int iCount = 0;
	RandomMachineInfoVector::iterator iter = m_MachineInfoList.begin();
	for( ; iter != m_MachineInfoList.end(); ++iter )
	{
		RandomMachineInfo *pInfo = (*iter);
		if( !pInfo )
			continue;

		if( pInfo->GetRecommandedShopOrder() != ORDER_DEFAULT )
			iCount++;
	}

	return iCount;
}

float ioExtraItemInfoManager::GetMortmainItemSellPesoReinforce( int nReinforce )
{
	float fSellPeso = m_fMortmainItemSell * ( 1 + ((float)nReinforce/25.0f) * m_fMortmainItemSellRate );

	if ( fSellPeso < m_fMortmainItemSellMax )
		return fSellPeso;

	return m_fMortmainItemSellMax; 
}

float ioExtraItemInfoManager::GetTimeItemSellPesoReinforce( DWORD dwTime, int nReinforce )
{
	float fSellPeso = dwTime * m_fTimeItemSell * ( 1 + ((float)nReinforce/25.0f) * m_fTimeItemSellRate );

	if ( fSellPeso < m_fTimeItemSellMax )
		return fSellPeso;

	return m_fTimeItemSellMax; 
}