#include "stdafx.h"
#include "SpecialGoodsManager.h"

template<> SpecailGoodsManager *Singleton< SpecailGoodsManager >::ms_Singleton = 0;

SpecailGoodsManager::SpecailGoodsManager()
{
	Init();
}

SpecailGoodsManager::~SpecailGoodsManager()
{
	Destroy();
}

void SpecailGoodsManager::Init()
{
	m_vSpecialGoodsList.clear();
	
	m_bOpen	= false;

	//////////////////////////////////////////////////////////////////////////
	//popup
	char szKey[MAX_PATH] = {0,};
	char szBuf[MAX_PATH] = {0,};

	ioINILoader iLoader("config//sp2_popupstore.ini");
	iLoader.SetTitle("Common");

	int iMaxCount = iLoader.LoadInt( "Max", 0 );
	for( int i = 0; i < iMaxCount; ++i )
	{
		sprintf_s( szKey, "Popup%d", i+1 );
		iLoader.SetTitle( szKey );

		sPopupItemInfo popupInfo;		
		popupInfo.bActive = iLoader.LoadBool( "Active", true );
		popupInfo.iIndex = iLoader.LoadInt( "Index", 0 );		
		popupInfo.iPresentType = iLoader.LoadInt( "PresentType", 0 );
		popupInfo.iPresentValue1 = iLoader.LoadInt( "PresentValue1", 0 );
		popupInfo.iPresentValue2 = iLoader.LoadInt( "PresentValue2", 0 );
		popupInfo.iCash = iLoader.LoadInt( "Cash", 0 );
		popupInfo.iPrevCash = iLoader.LoadInt( "PrevCash", 0 );
		popupInfo.iDisCount = iLoader.LoadInt( "Discount", 0 );

		for( int j = 0; j < ePopupConditionMax; ++j )
		{
			sprintf_s( szKey, "Condition%d_Min", j+1 );
			popupInfo.dwConditionMin[j] =iLoader.LoadInt( szKey, 0 );
			sprintf_s( szKey, "Condition%d_Max", j+1 );
			popupInfo.dwConditionMax[j] =iLoader.LoadInt( szKey, 0 );
		}

		m_vecPopupItemInfo.push_back( popupInfo );
	}
}

void SpecailGoodsManager::Destroy()
{
	m_vSpecialGoodsList.clear();
}

SpecailGoodsManager& SpecailGoodsManager::GetSingleton()
{
	return Singleton< SpecailGoodsManager >::GetSingleton();
}


bool SpecailGoodsManager::IsPossibleBuy(DWORD dwItemType)
{
	if( m_vSpecialGoodsList.empty() )
		return false;

	int iSize = m_vSpecialGoodsList.size();

	for( int i = 0; i < iSize; i++ )
	{
		if( m_vSpecialGoodsList[i].iType != dwItemType )
			continue;

		if( m_vSpecialGoodsList[i].iLimitCount > 0 )
			return true;
		else
			return false;
	}

	return false;
}

void SpecailGoodsManager::SetSpecialGoodsList(SP2Packet& kPacket)
{
	ClearSpecialGoodsList();

	int iSize = 0;
	SpecialGoods stGoods;

	PACKET_GUARD_VOID( kPacket.Read(iSize) );

	for( int i = 0; i < iSize; i++ )
	{
		stGoods.Init();

		PACKET_GUARD_VOID( kPacket.Read(stGoods.iType) );
		PACKET_GUARD_VOID( kPacket.Read(stGoods.iLimitCount) );

		m_vSpecialGoodsList.push_back(stGoods);
	}
}

void SpecailGoodsManager::ChangeState(bool bState)
{
	if( m_bOpen != bState )
	{
		ClearSpecialGoodsList();
		m_bOpen = bState;
	}
}

void SpecailGoodsManager::FillGoodsList( SP2Packet& kPacket )
{
	if( m_vSpecialGoodsList.empty() )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[warning][specialshop] goods list is empty" );
		SetOpen(false);
		PACKET_GUARD_VOID(kPacket.Write(0));
		return;
	}

	int iSize = m_vSpecialGoodsList.size();
	PACKET_GUARD_VOID(kPacket.Write(iSize));
	for( int i = 0; i < iSize; i++ )
	{
		PACKET_GUARD_VOID(kPacket.Write(m_vSpecialGoodsList[i].iType));
		PACKET_GUARD_VOID(kPacket.Write(m_vSpecialGoodsList[i].iLimitCount));
	}
}

void SpecailGoodsManager::ChangeCurGoodsCount(DWORD dwGoodsCode, int iCurCount)
{
	if( iCurCount < 0 )
		return;

	int iSize = m_vSpecialGoodsList.size();
	for( int i = 0; i < iSize; i++ )
	{
		if( m_vSpecialGoodsList[i].iType != dwGoodsCode )
			continue;

		m_vSpecialGoodsList[i].iLimitCount = iCurCount;
		break;
	}
}

void SpecailGoodsManager::GetPopupStoreIndex( std::vector<int>& iUseVec, std::vector<int>& outSendVec,  DWORD* dwArray )
{
	int nSize = m_vecPopupItemInfo.size();
	for( int i = 0; i < nSize; ++i )
	{
		if( m_vecPopupItemInfo[i].bActive == false )
			continue;

		int iCheckIndex = m_vecPopupItemInfo[i].iIndex;

		if( iCheckIndex == 0 )
			continue;

		bool bUse = false;
		int jSize = iUseVec.size();			
		for( int j = 0; j < jSize; ++j )
		{
			if( iCheckIndex == iUseVec[j] )
			{
				bUse = true;
				break;
			}
		}
		if( bUse )
			continue;

		bool bSkip = false;
		for( int j = 0; j < ePopupConditionMax; ++j )
		{
			if( IsCompareMinMax( m_vecPopupItemInfo[i].dwConditionMin[j], m_vecPopupItemInfo[i].dwConditionMax[j], dwArray[j] ) == false )
			{
				bSkip = true;
				break;
			}
		}
		if( !bSkip )
			outSendVec.push_back( iCheckIndex );
	}
}

bool SpecailGoodsManager::IsCompareMinMax( DWORD dwMin, DWORD dwMax, DWORD dwValue )
{
	if( dwMin != 0 && (dwValue<dwMin) )
		return false;

	if( dwMax != 0 && (dwValue>dwMax) )
		return false;

	return true;
}

bool SpecailGoodsManager::GetBuyPopupItem( int iIndex, int& iPresentType, int& iPresentValue1, int& iPresentValue2, int& iNeedCash )
{
	int iSize = m_vecPopupItemInfo.size();
	for( int i = 0; i < iSize; ++i )
	{
		if( m_vecPopupItemInfo[i].iIndex == iIndex )
		{
			if( m_vecPopupItemInfo[i].bActive == false )
				return false;

			iPresentType = m_vecPopupItemInfo[i].iPresentType;
			iPresentValue1 = m_vecPopupItemInfo[i].iPresentValue1;
			iPresentValue2 = m_vecPopupItemInfo[i].iPresentValue2;
			iNeedCash = m_vecPopupItemInfo[i].iCash;
			return true;
		}
	}

	return false;
}

void SpecailGoodsManager::ReloadINI()
{
	m_vecPopupItemInfo.clear();

	char szKey[MAX_PATH] = {0,};
	char szBuf[MAX_PATH] = {0,};

	ioINILoader iLoader;
	iLoader.ReloadFile( "config//sp2_popupstore.ini" );	
	iLoader.SetTitle("Common");

	int iMaxCount = iLoader.LoadInt( "Max", 0 );
	for( int i = 0; i < iMaxCount; ++i )
	{
		sprintf_s( szKey, "Popup%d", i+1 );
		iLoader.SetTitle( szKey );

		sPopupItemInfo popupInfo;		
		popupInfo.bActive = iLoader.LoadBool( "Active", true );
		popupInfo.iIndex = iLoader.LoadInt( "Index", 0 );		
		popupInfo.iPresentType = iLoader.LoadInt( "PresentType", 0 );
		popupInfo.iPresentValue1 = iLoader.LoadInt( "PresentValue1", 0 );
		popupInfo.iPresentValue2 = iLoader.LoadInt( "PresentValue2", 0 );
		popupInfo.iCash = iLoader.LoadInt( "Cash", 0 );
		popupInfo.iPrevCash = iLoader.LoadInt( "PrevCash", 0 );
		popupInfo.iDisCount = iLoader.LoadInt( "Discount", 0 );

		for( int j = 0; j < ePopupConditionMax; ++j )
		{
			sprintf_s( szKey, "Condition%d_Min", j+1 );
			popupInfo.dwConditionMin[j] =iLoader.LoadInt( szKey, 0 );
			sprintf_s( szKey, "Condition%d_Max", j+1 );
			popupInfo.dwConditionMax[j] =iLoader.LoadInt( szKey, 0 );
		}

		m_vecPopupItemInfo.push_back( popupInfo );
	}
}
