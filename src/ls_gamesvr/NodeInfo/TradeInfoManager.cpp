

#include "stdafx.h"

#include "TradeInfoManager.h"

#include <strsafe.h>

//////////////////////////////////////////////////////////////////////////////////////////

template<> TradeInfoManager* Singleton< TradeInfoManager >::ms_Singleton = 0;

TradeInfoManager::TradeInfoManager()
{
}

TradeInfoManager::~TradeInfoManager()
{
}

void TradeInfoManager::CheckNeedReload()
{
	ioINILoader kLoader;
	kLoader.SetFileName( "config/sp2_trade_info.ini" );
	if( kLoader.ReadBool( "common", "Change", false ) )
	{
		LoadTradeInfo();
	}
}

void TradeInfoManager::LoadTradeInfo()
{
	ioINILoader kLoader;
	kLoader.ReloadFile( "config/sp2_trade_info.ini" );

	kLoader.SetTitle( "common" );
	//kLoader.SaveBool( "Change", false );

	m_iTradePeriod = kLoader.LoadInt( "register_period", 1 );

	m_iRegisterLimitLevel = kLoader.LoadInt( "register_limit_level", 0);
	m_iItemBuyLimitLevel = kLoader.LoadInt( "item_buy_limit_level", 0);
	m_fRegisterTex = kLoader.LoadFloat( "register_tex", 0.0f );
	m_fBuyTex = kLoader.LoadFloat( "buy_tex", 0.0f );
	m_fPCRoomBuyTex	= kLoader.LoadFloat( "pc_room_buy_tex", 0.0f );
	m_fPCRoomRegisterTex	= kLoader.LoadFloat( "pc_room_register_tex", 0.0f );
}

TradeInfoManager& TradeInfoManager::GetSingleton()
{
	return Singleton< TradeInfoManager >::GetSingleton();
}

