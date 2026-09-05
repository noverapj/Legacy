#include "../stdafx.h"

#include "../EtcHelpFunc.h"

#include "ServerNodeManager.h"
#include "ExtraItemGrowthCatalystMgr.h"


template<> ExtraItemGrowthCatalystMgr* Singleton< ExtraItemGrowthCatalystMgr >::ms_Singleton = 0;

ExtraItemGrowthCatalystMgr::ExtraItemGrowthCatalystMgr()
{

}

ExtraItemGrowthCatalystMgr::~ExtraItemGrowthCatalystMgr()
{
	ClearData();

	m_vCurrentExtraItemMortmain.clear();
}

ExtraItemGrowthCatalystMgr& ExtraItemGrowthCatalystMgr::GetSingleton()
{
	return Singleton< ExtraItemGrowthCatalystMgr >::GetSingleton();
}

void ExtraItemGrowthCatalystMgr::ClearData()
{
	m_dwLevelUPRandList.clear();
	m_dwReinforceRandList.clear();
	m_vExtraItemMortmainLimit.clear();
}

void ExtraItemGrowthCatalystMgr::LoadINIData( bool bCreate )
{
	ClearData();

	char szKey[MAX_PATH] = "";
	ioINILoader kLoader( "config/sp2_extraitem_growth_catalyst_info.ini" );

	{
		kLoader.SetTitle( "LevelUPRand" );

		int iMaxLevel = kLoader.LoadInt( "MaxLevel", 0 );
		for(int i = 0;i < iMaxLevel;i++)
		{
			sprintf_s( szKey, "LevelRand%d", i + 1 );
			m_dwLevelUPRandList.push_back( kLoader.LoadInt( szKey, 0 ) );
		}
	}

	{
		kLoader.SetTitle( "Reinforce" );

		int iMaxReinforce = kLoader.LoadInt( "MaxReinforce", 0 );
		for(int i = 0;i < iMaxReinforce;i++)
		{
			sprintf_s( szKey, "ReinforceRand%d", i + 1 );
			m_dwReinforceRandList.push_back( kLoader.LoadInt( szKey, 0 ) );
		}
	}

	{
		kLoader.SetTitle( "MortmainLimit" );

		int iMaxItem = kLoader.LoadInt( "MaxItem", 0 );
		for(int i = 0;i < iMaxItem;i++)
		{
			ExtraItemMortmainLimit kData;
			
			sprintf_s( szKey, "ItemCode%d", i + 1 );
			kData.m_dwExtraItemCode = kLoader.LoadInt( szKey, 0 );

			sprintf_s( szKey, "ItemLimit%d", i + 1 );
			kData.m_dwMortmainCount = kLoader.LoadInt( szKey, 0 );

			sprintf_s( szKey, "ItemLimitMinute%d", i + 1 );
			kData.m_dwMortmainMinute = kLoader.LoadInt( szKey, 0 );

			m_vExtraItemMortmainLimit.push_back( kData );
		}
	}

	LOG.PrintTimeAndLog( 0, "ExtraItemGrowthCatalystMgr : LevelUP:%d - Reinforce:%d - MortmainLimit:%d", (int)m_dwLevelUPRandList.size(), (int)m_dwReinforceRandList.size(), (int)m_vExtraItemMortmainLimit.size() );	

	if( bCreate )
	{
		// 저장된 INI Load
		m_vCurrentExtraItemMortmain.clear();

		ioINILoader kLoader2( "config/sp2_extraitem_growth_save.ini" );

		vExtraItemMortmainLimit::iterator iter = m_vExtraItemMortmainLimit.begin();
		for(;iter != m_vExtraItemMortmainLimit.end();++iter)
		{
			ExtraItemMortmainLimit &rkData = *iter;

			sprintf_s( szKey, "%d", rkData.m_dwExtraItemCode );			
			kLoader2.SetTitle( szKey );

			ExtraItemMortmainLimit kCurrentData;
			kCurrentData.m_dwExtraItemCode        = rkData.m_dwExtraItemCode;
			kCurrentData.m_dwMortmainMinute       = GetExtraItemMortmainLimitMinute( rkData.m_dwExtraItemCode );
			kCurrentData.m_dwMortmainCount        = kLoader2.LoadInt( "MortmainCount", 0 );
			kCurrentData.m_dwCurrentMortmainMinue = kLoader2.LoadInt( "MortmainMinute", 0 );
			m_vCurrentExtraItemMortmain.push_back( kCurrentData );
		}
		LOG.PrintTimeAndLog( 0, "ExtraItemGrowthCatalystMgr : Save Data = %d", (int)m_vCurrentExtraItemMortmain.size() );	
	}
}

int ExtraItemGrowthCatalystMgr::GetExtraItemMortmainLimit( DWORD dwItemCode )
{
	vExtraItemMortmainLimit::iterator iter = m_vExtraItemMortmainLimit.begin();
	for(;iter != m_vExtraItemMortmainLimit.end();++iter)
	{
		ExtraItemMortmainLimit &rkData = *iter;
		if( rkData.m_dwExtraItemCode == dwItemCode )
			return rkData.m_dwMortmainCount;
	}
	return 0;
}

int ExtraItemGrowthCatalystMgr::GetCurrentExtraItemMortmain( DWORD dwItemCode )
{
	vExtraItemMortmainLimit::iterator iter = m_vCurrentExtraItemMortmain.begin();
	for(;iter != m_vCurrentExtraItemMortmain.end();++iter)
	{
		ExtraItemMortmainLimit &rkData = *iter;
		if( rkData.m_dwExtraItemCode == dwItemCode )
			return rkData.m_dwMortmainCount;
	}
	return 0;
}

DWORD ExtraItemGrowthCatalystMgr::GetExtraItemMortmainLimitMinute( DWORD dwItemCode )
{
	vExtraItemMortmainLimit::iterator iter = m_vExtraItemMortmainLimit.begin();
	for(;iter != m_vExtraItemMortmainLimit.end();++iter)
	{
		ExtraItemMortmainLimit &rkData = *iter;
		if( rkData.m_dwExtraItemCode == dwItemCode )
			return rkData.m_dwMortmainMinute;
	}
	return 0;
}

void ExtraItemGrowthCatalystMgr::SaveCurrentExtraItemMortmain( DWORD dwItemCode )
{
	// INI 저장.
	{
		vExtraItemMortmainLimit::iterator iter = m_vCurrentExtraItemMortmain.begin();
		for(;iter != m_vCurrentExtraItemMortmain.end();++iter)
		{
			ExtraItemMortmainLimit &rkData = *iter;
			if( rkData.m_dwExtraItemCode == dwItemCode )
			{
				// 카운트 증가
				rkData.m_dwMortmainCount++;

				// 다음 영구 장비 가능 시간 증가
				if( rkData.m_dwMortmainMinute > 0 )
				{
					CTimeSpan cSpanGap( 0, 0, rkData.m_dwMortmainMinute, 0 );
					CTime     cNextTime = CTime::GetCurrentTime() + cSpanGap;
					rkData.m_dwCurrentMortmainMinue = (cNextTime.GetMonth() * 1000000) + (cNextTime.GetDay() * 10000) + (cNextTime.GetHour() * 100) + (cNextTime.GetMinute());
				}

				char szTitle[MAX_PATH] = "";
				sprintf_s( szTitle, "%d", rkData.m_dwExtraItemCode );

				ioINILoader kLoader;
				kLoader.SetFileName( "config/sp2_extraitem_growth_save.ini" );
				kLoader.SaveInt( szTitle, "MortmainCount", rkData.m_dwMortmainCount );
				kLoader.SaveInt( szTitle, "MortmainMinute", rkData.m_dwCurrentMortmainMinue );
				return;
			}
		}
	}

	{   // 추가 & INI 저장 - 메모리에 로드하지 않는다.

		ExtraItemMortmainLimit kCurrentData;
		kCurrentData.m_dwExtraItemCode = dwItemCode;
		kCurrentData.m_dwMortmainMinute= GetExtraItemMortmainLimitMinute( dwItemCode );
		kCurrentData.m_dwMortmainCount = 1;		    // 1개 생성됨

		// 다음 영구 장비 가능 시간 증가
		if( kCurrentData.m_dwMortmainMinute > 0 )
		{
			CTimeSpan cSpanGap( 0, 0, kCurrentData.m_dwMortmainMinute, 0 );
			CTime     cNextTime = CTime::GetCurrentTime() + cSpanGap;
			kCurrentData.m_dwCurrentMortmainMinue = (cNextTime.GetMonth() * 1000000) + (cNextTime.GetDay() * 10000) + (cNextTime.GetHour() * 100) + (cNextTime.GetMinute());
		}
		m_vCurrentExtraItemMortmain.push_back( kCurrentData );

		char szTitle[MAX_PATH] = "";
		sprintf_s( szTitle, "%d", kCurrentData.m_dwExtraItemCode );

		ioINILoader kLoader;
		kLoader.SetFileName( "config/sp2_extraitem_growth_save.ini" );
		kLoader.SaveInt( szTitle, "MortmainCount", kCurrentData.m_dwMortmainCount );
		kLoader.SaveInt( szTitle, "MortmainMinute", kCurrentData.m_dwCurrentMortmainMinue );
	}
}

void ExtraItemGrowthCatalystMgr::GetExtraItemMortmainInfo( DWORD dwItemCode, DWORD &rCount, DWORD &rDate )
{
	rCount = rDate = 0;
	vExtraItemMortmainLimit::iterator iter = m_vCurrentExtraItemMortmain.begin();
	for(;iter != m_vCurrentExtraItemMortmain.end();++iter)
	{
		ExtraItemMortmainLimit &rkData = *iter;
		if( rkData.m_dwExtraItemCode == dwItemCode )
		{
			rCount = rkData.m_dwMortmainCount;
			rDate  = rkData.m_dwCurrentMortmainMinue;
			return;
		}
	}
}

bool ExtraItemGrowthCatalystMgr::IsExtraItemMortmainTimeCheck( DWORD dwItemCode )
{
	vExtraItemMortmainLimit::iterator iter = m_vCurrentExtraItemMortmain.begin();
	for(;iter != m_vCurrentExtraItemMortmain.end();++iter)
	{
		ExtraItemMortmainLimit &rkData = *iter;
		if( rkData.m_dwExtraItemCode == dwItemCode )
		{
			if( rkData.m_dwCurrentMortmainMinue == 0 )
				return false;

			CTime cCurTime = CTime::GetCurrentTime();
			DWORD dwCurTime= (cCurTime.GetMonth() * 1000000) + (cCurTime.GetDay() * 10000) + (cCurTime.GetHour() * 100) + (cCurTime.GetMinute());
			if( dwCurTime <= rkData.m_dwCurrentMortmainMinue )
				return true;
		}
	}
	return false;
}

bool ExtraItemGrowthCatalystMgr::IsExtraItemMortmainCheck( DWORD dwItemCode )
{
	if( IsExtraItemMortmainTimeCheck( dwItemCode ) ) 
		return false;  // 영구 장비 생성 시간 제한

	if( GetCurrentExtraItemMortmain( dwItemCode ) < GetExtraItemMortmainLimit( dwItemCode ) )
	{
		SaveCurrentExtraItemMortmain( dwItemCode );
		return true;
	}
	return false;
}

void ExtraItemGrowthCatalystMgr::MinusExtraItemMortmainCount( DWORD dwItemCode )
{
	vExtraItemMortmainLimit::iterator iter = m_vCurrentExtraItemMortmain.begin();
	for(;iter != m_vCurrentExtraItemMortmain.end();++iter)
	{
		ExtraItemMortmainLimit &rkData = *iter;
		if( rkData.m_dwExtraItemCode != dwItemCode ) continue;
		if( rkData.m_dwMortmainCount == 0 ) continue;

		rkData.m_dwMortmainCount--;
		rkData.m_dwCurrentMortmainMinue = 0;

		char szTitle[MAX_PATH] = "";
		sprintf_s( szTitle, "%d", rkData.m_dwExtraItemCode );

		ioINILoader kLoader;
		kLoader.SetFileName( "config/sp2_extraitem_growth_save.ini" );
		kLoader.SaveInt( szTitle, "MortmainCount", rkData.m_dwMortmainCount );
		kLoader.SaveInt( szTitle, "MortmainMinute", rkData.m_dwCurrentMortmainMinue );

		LOG.PrintTimeAndLog( 0, "MinusExtraItemMortmainCount : 유저 이탈로 영구 장비 생성 안됨 - 실패 처리 : %d", dwItemCode );
		return;
	}
}

void ExtraItemGrowthCatalystMgr::SendLoadData( ServerNode *pServerNode )
{
	SP2Packet kPacket( MSTPK_EXTRAITEM_GROWTH_CATALYST_DATA );

	{
		int iMaxLevel = m_dwLevelUPRandList.size();

		kPacket << iMaxLevel;
		for(int i = 0;i < iMaxLevel;i++)
			kPacket << m_dwLevelUPRandList[i];
	}

	{
		int iMaxReinforce = m_dwReinforceRandList.size();

		kPacket << iMaxReinforce;
		for(int i = 0;i < iMaxReinforce;i++)
			kPacket << m_dwReinforceRandList[i];
	}

	if( pServerNode )
	{	
		// 서버 1개에만 전송
		pServerNode->SendMessage( kPacket );    
	}
	else
	{
		// 전체 서버에 전송
		g_ServerNodeManager.SendMessageAllNode( kPacket );   
	}
}
