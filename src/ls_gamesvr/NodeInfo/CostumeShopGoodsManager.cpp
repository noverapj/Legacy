#include "stdafx.h"
#include "CostumeShopGoodsManager.h"

CostumeGoodsInfo::CostumeGoodsInfo()
{
	Init();
}

CostumeGoodsInfo::~CostumeGoodsInfo()
{
	Destroy();
}

void CostumeGoodsInfo::Init()
{
	m_iGoodsCode	= 0;
	m_iItemCode		= 0;

	m_vPesoGoodsInfo.clear();
	m_vPesoGoodsInfo.reserve(100);
}

void CostumeGoodsInfo::Destroy()
{
}

void CostumeGoodsInfo::SetPesoGoodsInfo(int iPeriod, int iPeso)
{
	if( 0 >= iPeriod || 0 >= iPeso )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[warning][costumeshop] period and peso value is zero : [%d][%d][%d]", GetGoodsCode(), iPeriod, iPeso );
		return;
	}

	PesoSellInfo stGoods;
	stGoods.iNeedPeso = iPeso;
	stGoods.iPeriod = iPeriod;

	m_vPesoGoodsInfo.push_back(stGoods);
}

int CostumeGoodsInfo::GetPesoGoodsPriceInfo(int iArray)
{
	int iSize = (int)m_vPesoGoodsInfo.size();

	if( iSize <= iArray || iArray < 0 )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[warning][costumeshop] invalid array : [%d][%d]", GetGoodsCode(), iArray );
		return 0;
	}

	PesoSellInfo& stInfo = m_vPesoGoodsInfo[iArray];
	if( 0 >= stInfo.iNeedPeso )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[warning][costumeshop] invalid peso setting : [%d][%d]", GetGoodsCode(), stInfo.iNeedPeso );
		return 0;
	}

	return stInfo.iNeedPeso;
}

int CostumeGoodsInfo::GetPesoGoodsPeriodInfo(int iArray)
{
	int iSize = (int)m_vPesoGoodsInfo.size();

	if( iSize <= iArray || iArray < 0 )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[warning][costumeshop] invalid array : [%d][%d]", GetGoodsCode(), iArray );
		return -1;
	}

	PesoSellInfo& stInfo = m_vPesoGoodsInfo[iArray];
	if( 0 > stInfo.iPeriod )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[warning][costumeshop] invalid period setting : [%d][%d]", GetGoodsCode(), stInfo.iPeriod );
		return -1;
	}

	return stInfo.iPeriod;
}

CostumeGoodsInfo::PesoSellInfo* CostumeGoodsInfo::GetPesoGoodsInfo(int iArray)
{
	int iSize = (int)m_vPesoGoodsInfo.size();
	if( iArray < 0 || iArray >= iSize )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[warning][costumeshop] invalid array : [%d][%d]", GetGoodsCode(), iArray );
		return NULL;
	}

	return &m_vPesoGoodsInfo[iArray];
}

bool CostumeGoodsInfo::IsRightPeriod(int iPeriod)
{
	int iSize = (int)m_vPesoGoodsInfo.size();

	for( int i = 0; i < iSize; i++ )
	{
		if( m_vPesoGoodsInfo[i].iPeriod == iPeriod )
			return true;
	}

	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<> CostumeShopGoodsManager* Singleton< CostumeShopGoodsManager >::ms_Singleton = 0;

CostumeShopGoodsManager& CostumeShopGoodsManager::GetSingleton()
{
	return Singleton< CostumeShopGoodsManager >::GetSingleton();
}


CostumeShopGoodsManager::CostumeShopGoodsManager()
{
	m_bINILoading = false;
	Init();
}

CostumeShopGoodsManager::~CostumeShopGoodsManager()
{
	Destroy();
}

void CostumeShopGoodsManager::Init()
{
	m_mCostumeGoodsMap.clear();
}

void CostumeShopGoodsManager::Destroy()
{
	m_mCostumeGoodsMap.clear();
}

void CostumeShopGoodsManager::LoadINI()
{
	m_bINILoading = true;
	ioINILoader kCostumeShopGoodsLoader;
	kCostumeShopGoodsLoader.ReloadFile( "config\\sp2_costume_shop_goods.ini" );
	LoadCustumeShopGoods(kCostumeShopGoodsLoader);
	m_bINILoading = false;
}

void CostumeShopGoodsManager::LoadCustumeShopGoods( ioINILoader &rkLoader )
{
	Init();

	rkLoader.SetTitle("common_info");
	int iMachineCount	= rkLoader.LoadInt("machine_count", 0);

	char szTitle[MAX_PATH]="";

	for( int i = 0; i < iMachineCount; i++ )
	{
		char szName[MAX_PATH]="";
		CostumeGoodsInfo cGoodsInfo;

		StringCbPrintf( szTitle, sizeof( szTitle ), "costume_machine%d", i+1 );
		rkLoader.SetTitle(szTitle);

		int iMachineCode = rkLoader.LoadInt("machine_code", 0);
		cGoodsInfo.SetGoodsCode(iMachineCode);

		int iItemCode	= rkLoader.LoadInt("item_code", 0);
		cGoodsInfo.SetItemCode(iItemCode);

		int iActive	= rkLoader.LoadInt("active", 1);
		cGoodsInfo.SetActive( iActive == 1 );

   		for( int i = 0; i < 100; i++ )
		{
			//PesoSellInfo stPesoGoodsInfo;

			StringCbPrintf( szName, sizeof(szName), "need_peso%d", i+1 );
			int iNeedPeso = rkLoader.LoadInt(szName, -1);

			StringCbPrintf( szName, sizeof(szName), "period%d", i+1 );
			int iPeriod = rkLoader.LoadInt(szName, -1);

			if( iPeriod < 0 )
				break;

			cGoodsInfo.SetPesoGoodsInfo(iPeriod, iNeedPeso);
		}
		m_mCostumeGoodsMap.insert(make_pair(cGoodsInfo.GetGoodsCode(), cGoodsInfo));
	}
}

CostumeGoodsInfo* CostumeShopGoodsManager::GetCostumeGoodsInfo(int iGoodsCode)
{
	if ( m_bINILoading == true )
		return NULL;

	GoodsList::iterator it = m_mCostumeGoodsMap.find(iGoodsCode);

	if( it == m_mCostumeGoodsMap.end() )
		return NULL;

	return &(it->second);
}

bool CostumeShopGoodsManager::IsRightGoods(int iGoodsCode)
{
	if ( m_bINILoading == true )
		return false;

	GoodsList::iterator it = m_mCostumeGoodsMap.find(iGoodsCode);

	if( it == m_mCostumeGoodsMap.end() )
		return false;
	if( !it->second.GetActive() )
		return false;

	return true;
}

int CostumeShopGoodsManager::GetNeedPeso(int iGoodsCode, int iArray)
{
	if ( m_bINILoading == true )
		return 0;

	CostumeGoodsInfo* cGoodsInfo = GetCostumeGoodsInfo(iGoodsCode);
	if( !cGoodsInfo )
		return 0;

	return cGoodsInfo->GetPesoGoodsPriceInfo(iArray);
}

int CostumeShopGoodsManager::GetPeriod(int iGoodsCode, int iArray)
{
	if ( m_bINILoading == true )
		return -1;

	CostumeGoodsInfo* cGoodsInfo = GetCostumeGoodsInfo(iGoodsCode);
	if( !cGoodsInfo )
		return -1;

	return cGoodsInfo->GetPesoGoodsPeriodInfo(iArray);
}

int CostumeShopGoodsManager::GetItemCode(int iGoodsCode)
{
	if ( m_bINILoading == true )
		return 0;

	GoodsList::iterator it = m_mCostumeGoodsMap.find(iGoodsCode);

	if( it == m_mCostumeGoodsMap.end() )
		return 0;		//없는 상품

	CostumeGoodsInfo& cInfo = it->second;
	return cInfo.GetItemCode();
}

bool CostumeShopGoodsManager::IsRightPeriod(int iGoodsCode, int iPeriod)
{
	if ( m_bINILoading == true )
		return false;

	GoodsList::iterator it = m_mCostumeGoodsMap.find(iGoodsCode);

	if( it == m_mCostumeGoodsMap.end() )
		return false;		//없는 상품

	CostumeGoodsInfo cInfo = it->second;
	
	return cInfo.IsRightPeriod(iPeriod);
}

bool CostumeShopGoodsManager::IsRightArray(int iGoodsCode, int iArray)
{
	if ( m_bINILoading == true )
		return false;

	GoodsList::iterator it = m_mCostumeGoodsMap.find(iGoodsCode);

	if( it == m_mCostumeGoodsMap.end() )
		return false;

	CostumeGoodsInfo& cGoodsInfo = it->second;
	int iGoodsCount = cGoodsInfo.GetGoodsCount();

	if( iArray < 0 || iGoodsCount <= iArray )
		return false;

	return true;
}
