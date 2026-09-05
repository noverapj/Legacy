#include "stdafx.h"
//#include "../Window.h"
#include "../MainProcess.h"

#include "../DataBase/DBClient.h"
#include "../DataBase/LogDBClient.h"
#include "../MainServerNode/MainServerNode.h"
#include "../EtcHelpFunc.h"

#include "ioDecorationPrice.h"

#include "User.h"
#include "Room.h"
#include "ServerNodeManager.h"
#include "UserNodeManager.h"
#include "ioEtcItemManager.h"
#include "ioMyLevelMgr.h"
#include "ioPresentHelper.h"

#include "GrowthManager.h"
#include "ioUserGrowthLevel.h"

#include ".\ioetcitem.h"
#include <strsafe.h>
#include "..\Local\ioLocalParent.h"
#include "ioExtraItemInfoManager.h"
#include "TournamentManager.h"
#include "..\Channeling\ioChannelingNodeManager.h"
#include "..\Channeling\ioChannelingNodeParent.h"
#include "ioEventUserNode.h"
#include "ioBingoManager.h"
#include "ioSuperGashaponMgr.h"
#include "ioExerciseCharIndexManager.h"
#include "ioItemRechargeManager.h"
#include "../Filter/WordFilterManager.h"
#include "ioPowerUpManager.h"
#include "MissionManager.h"
#include "GuildRoomsBlockManager.h"
#include "UserTitleInven.h"
#include "UserTitleInfo.h"
#include "TitleData.h"
#include "TitleManager.h"
#include "UserBonusCash.h"

#ifdef OHTG_NEW_RANROM_BOX_20201109
#include "ioRandomBoxManager.h"
#endif //OHTG_NEW_RANROM_BOX_20201109

ioEtcItem::ioEtcItem()
{
	m_dwType   = EIT_NONE;
	m_eUseType = UT_NONE;
	m_iUseValue= 0;
	m_iMaxUse  = 0;
	m_iExtraType = 0;
	m_bSpecialGoods	= false;
	m_bActive = false;

	m_vSubscriptionList.reserve( 10 );
	m_vValueList.reserve( 10 );
	m_vPesoList.reserve( 10 );
	m_vCashList.reserve( 10 );
	m_vMileageList.reserve( 10 );
	m_vBonusPesoList.reserve( 10 );
	m_vActiveList.reserve( 10 );

	m_vCanMortmainList.reserve( 10 );
}

ioEtcItem::~ioEtcItem()
{
	m_vSubscriptionList.clear();
	m_vValueList.clear();
	m_vPesoList.clear();
	m_vCashList.clear();
	m_vMileageList.clear();
	m_vBonusPesoList.clear();
	m_vActiveList.clear();

	m_vCanMortmainList.clear();
}

void ioEtcItem::LoadProperty( ioINILoader &rkLoader )
{
}

int ioEtcItem::GetValue( int iArray ) const
{
	if( !COMPARE( iArray, 0, (int) m_vValueList.size() ) )
		return 0;

	return m_vValueList[iArray];
}

int ioEtcItem::GetPeso( int iArray ) const
{
	if( !COMPARE( iArray, 0, (int) m_vPesoList.size() ) )
		return 0;

	return m_vPesoList[iArray];
}

int ioEtcItem::GetSubscriptionType( int iArray ) const
{
	if( !COMPARE( iArray, 0, (int) m_vSubscriptionList.size() ) )
		return SUBSCRIPTION_NONE;

	return m_vSubscriptionList[iArray];
}

int ioEtcItem::GetCash( int iArray ) const
{
	if( !COMPARE( iArray, 0, (int) m_vCashList.size() ) )
		return 0;

	return m_vCashList[iArray];
}

int ioEtcItem::GetMileage( int iArray ) const
{
	if( !COMPARE( iArray, 0, (int) m_vMileageList.size() ) )
		return 0;

	return m_vMileageList[iArray];
}

int ioEtcItem::GetBonusPeso( int iArray ) const
{
	if( !COMPARE( iArray, 0, (int) m_vBonusPesoList.size() ) )
		return 0;

	return m_vBonusPesoList[iArray];
}

bool ioEtcItem::GetActive( int iArray ) const
{
	if( !COMPARE( iArray, 0, (int) m_vActiveList.size() ) )
		return 0;

	return m_vActiveList[iArray];
}

bool ioEtcItem::IsCanMortmain( int iArray ) const
{
	if( !COMPARE( iArray, 0, (int)m_vCanMortmainList.size() ) )
		return false;

	if( m_vCanMortmainList[iArray] == 0 )
		return false;

	return true;
}

void ioEtcItem::AddSubscription( int iType )
{
	m_vSubscriptionList.push_back( iType );
}

void ioEtcItem::AddValue( int iValue )
{
	m_vValueList.push_back( iValue );
}

void ioEtcItem::AddPeso( int iPeso )
{
	m_vPesoList.push_back( iPeso );
}

void ioEtcItem::AddCash( int iCash )
{
	m_vCashList.push_back( iCash );
}

void ioEtcItem::AddMileage( int iMileage )
{
	m_vMileageList.push_back( iMileage );
}

void ioEtcItem::AddBonusPeso( int iBonusPeso )
{
	m_vBonusPesoList.push_back( iBonusPeso );
}

void ioEtcItem::AddActive( bool bActive )
{
	m_vActiveList.push_back( bActive );
}

void ioEtcItem::AddCanMortmain( int iCanMortmain )
{
	m_vCanMortmainList.push_back( iCanMortmain );
}

void ioEtcItem::OnBuy( User *pUser, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	if( rkSlot.GetUseType() != UT_DATE )
		rkSlot.SetUse( true );
}

bool ioEtcItem::OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	if( pUser == NULL )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Fail - pUser == NULL.(%d)", __FUNCTION__, m_dwType );
		return false;
	}

	if( pUserEtcItem == NULL )
	{
		SP2Packet kReturn( STPK_ETCITEM_USE );
		PACKET_GUARD_bool( kReturn.Write( ETCITEM_USE_EXCEPTION ) );
		pUser->SendMessage( kReturn );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Fail - pUserEtcItem == NULL.(%d)", __FUNCTION__, m_dwType );
		return false;
	}

	int iPrevCount = rkSlot.m_iValue1;
	static DWORDVec vValue;
	vValue.clear();
	vValue.push_back(PRESENT_ETC_ITEM);
	vValue.push_back(rkSlot.m_iType);
	if( !_OnUse( rkPacket, pUser, pUserEtcItem, rkSlot ) )
	{
		// 에러메세지는 _OnUse에서 처리함.
		int iAfterCount = rkSlot.m_iValue1;

		if( iPrevCount > iAfterCount )
		{
			int iUseCount = iPrevCount - iAfterCount;
			vValue.push_back(iUseCount);
			g_MissionMgr.DoTrigger(MISSION_CLASS_ITEM_USE, pUser, vValue);
		}
		
		return false;
	}

	SP2Packet kReturn( STPK_ETCITEM_USE );
	PACKET_GUARD_bool( kReturn.Write( ETCITEM_USE_OK ) );
	PACKET_GUARD_bool( kReturn.Write( rkSlot.m_iType ) );
	PACKET_GUARD_bool( kReturn.Write( rkSlot.m_iValue1 ) );
	PACKET_GUARD_bool( kReturn.Write( rkSlot.m_iValue2 ) );

	pUser->SendMessage( kReturn );

	printf("Use Etc Item2 (%s/%d/%d/%d)\n", pUser->GetPublicID().c_str(), rkSlot.m_iType, rkSlot.m_iValue1, rkSlot.m_iValue2 );

	int iAfterCount = rkSlot.m_iValue1;
	if( iPrevCount > iAfterCount )
	{
		int iUseCount = iPrevCount - iAfterCount;
		vValue.push_back(iUseCount);
		g_MissionMgr.DoTrigger(MISSION_CLASS_ITEM_USE, pUser, vValue);
	}

	return true;
}

bool ioEtcItem::OnSell( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	if( pUser == NULL )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Fail - pUser == NULL.(%d)", __FUNCTION__, m_dwType );
		return false;
	}

	if( pUserEtcItem == NULL )
	{
		SP2Packet kReturn( STPK_ETCITEM_SELL );
		PACKET_GUARD_bool( kReturn.Write(ETCITEM_SELL_EXCEPTION) );
		pUser->SendMessage( kReturn );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Fail - pUserEtcItem == NULL.(%d)", __FUNCTION__, m_dwType );
		return false;
	}

	if( pUser->IsReserveServerMoving() )
	{
		SP2Packet kReturn( STPK_ETCITEM_SELL );
		PACKET_GUARD_bool( kReturn.Write(ETCITEM_SELL_MOVING_SERVER) );
		pUser->SendMessage( kReturn );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Fail Reserve Server moving - %s) %d.",  __FUNCTION__, pUser->GetPublicID().c_str(), rkSlot.m_iType );
		return false;
	}

	if( !IsCanSell() )
	{
		SP2Packet kReturn( STPK_ETCITEM_SELL );
		PACKET_GUARD_bool( kReturn.Write(ETCITEM_SELL_DONT_SELL) );
		pUser->SendMessage( kReturn );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Fail don't sell type - %s) %d.",  __FUNCTION__, pUser->GetPublicID().c_str(), rkSlot.m_iType );
		return false;
	}

	int  iValue1 = 0;
	if( GetType() == EIT_ETC_FRIEND_SLOT_EXTEND )
	{
		iValue1 = rkSlot.GetUse();
	}
	else if( GetType() == EIT_ETC_CHAR_SLOT_EXTEND )
	{
		iValue1 = rkSlot.GetUse();
	}
	else if( GetUseType() == UT_ETERNITY )
	{
		iValue1 = rkSlot.GetUse();
	}
	else if( GetUseType() == UT_TIME )
	{
		iValue1 = rkSlot.GetUse() / 60;
	}
	else if( GetUseType() == UT_COUNT || GetUseType() == UT_ONCE )
	{
		iValue1 = rkSlot.GetUse();
	}
	else if( GetUseType() == UT_DATE )
	{
		CTime kLimitTime( Help::GetSafeValueForCTimeConstructor( rkSlot.GetYear(), rkSlot.GetMonth(), rkSlot.GetDay(), rkSlot.GetHour(), rkSlot.GetMinute(), 0 ) );
		CTimeSpan kGapTime = kLimitTime - CTime::GetCurrentTime();
		iValue1 = max( 0, kGapTime.GetTotalHours() / 24 );
	}
	else 
	{
		SP2Packet kReturn( STPK_ETCITEM_SELL );
		PACKET_GUARD_bool( kReturn.Write(ETCITEM_SELL_EXCEPTION) );
		pUser->SendMessage( kReturn );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Fail - None Use Type.(%d)", __FUNCTION__, m_dwType );
		return false;
	}

	int iResellPeso  = GetSellPeso() * iValue1;
	if( iResellPeso < 0 )
	{
		SP2Packet kReturn( STPK_ETCITEM_SELL );
		PACKET_GUARD_bool( kReturn.Write(ETCITEM_SELL_EXCEPTION) );
		pUser->SendMessage( kReturn );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Fail - ResellPeso Error.(%d:%d)", __FUNCTION__, m_dwType, iResellPeso );
		return false;
	}

	if( pUser->SendBillingAddMileage( PRESENT_ETC_ITEM, m_dwType, 0, iResellPeso , false ) )
	{
		iResellPeso = 0;
	}
	else
	{
		__int64 iPrevMoney = pUser->GetMoney();
		pUser->AddMoney( iResellPeso );
		g_LogDBClient.OnInsertGameLogInfo(LogDBClient::GLT_PESO_GAIN, pUser, 0, 0, LogDBClient::LET_ITEM, LogDBClient::PT_SELL_ETCITEM, PRESENT_ETC_ITEM, rkSlot.m_iType, iResellPeso, NULL);
	}

	// 판매
	{
		SP2Packet kReturn( STPK_ETCITEM_SELL );
		PACKET_GUARD_bool( kReturn.Write(ETCITEM_SELL_OK) );
		PACKET_GUARD_bool( kReturn.Write(rkSlot.m_iType) );
		PACKET_GUARD_bool( kReturn.Write(iResellPeso) );
		PACKET_GUARD_bool( kReturn.Write(pUser->GetMoney()) );
		pUser->SendMessage( kReturn );
	}
	pUserEtcItem->DeleteEtcItem( rkSlot.m_iType, LogDBClient::ET_SELL );
	g_LogDBClient.OnInsertPeso( pUser, iResellPeso, LogDBClient::PT_SELL_ETCITEM );	
	return true;
}

void ioEtcItem::SetSubscription( int iArray , int iType )
{
	if( !COMPARE( iArray, 0, (int) m_vSubscriptionList.size() ) )
		return;

	m_vSubscriptionList[iArray] = iType;
}

void ioEtcItem::SetValue( int iArray , int iValue )
{
	if( !COMPARE( iArray, 0, (int) m_vValueList.size() ) )
		return;

	m_vValueList[iArray] = iValue;
}

void ioEtcItem::SetPeso( int iArray , int iPeso )
{
	if( !COMPARE( iArray, 0, (int) m_vPesoList.size() ) )
		return;

	m_vPesoList[iArray] = iPeso;
}

void ioEtcItem::SetCash( int iArray , int iCash )
{
	if( !COMPARE( iArray, 0, (int) m_vCashList.size() ) )
		return;

	m_vCashList[iArray] = iCash;
}

void ioEtcItem::SetMileage( int iArray , int iMileage )
{
	if( !COMPARE( iArray, 0, (int) m_vMileageList.size() ) )
		return;

	m_vMileageList[iArray] = iMileage;
}

void ioEtcItem::SetBonusPeso( int iArray , int iBonusPeso )
{
	if( !COMPARE( iArray, 0, (int) m_vBonusPesoList.size() ) )
		return;

	m_vBonusPesoList[iArray] = iBonusPeso;
}

void ioEtcItem::SetActive( int iArray, bool bActive )
{
	if( !COMPARE( iArray, 0, (int) m_vActiveList.size() ) )
		return;

	m_vActiveList[iArray] = bActive;
}

void ioEtcItem::SetCanMortmain( int iArray, int iCanMortmain )
{
	if( !COMPARE( iArray, 0, (int)m_vCanMortmainList.size() ) )
		return;

	m_vCanMortmainList[iArray] = iCanMortmain;
}

void ioEtcItem::SetSQLUpdateInfo(bool bVal)
{
	m_bSQLUpdate	= bVal;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemWholeChat::ioEtcItemWholeChat()
{
}

ioEtcItemWholeChat::~ioEtcItemWholeChat()
{

}

bool ioEtcItemWholeChat::_OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	// OnUse에서 체크하므로 pUser 와 pUserEtcItem NULL이 아님을 보장
	ioHashString szChat;
	PACKET_GUARD_bool( rkPacket.Read(szChat) );

	if( szChat.IsEmpty() )
	{
		SP2Packet kReturn( STPK_ETCITEM_USE );
		PACKET_GUARD_bool( kReturn.Write(ETCITEM_USE_EXCEPTION) );
		pUser->SendMessage( kReturn );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Fail - %s",  __FUNCTION__, pUser->GetPublicID().c_str() );
		return false;
	}

	rkSlot.AddUse( -1 );
	if( rkSlot.GetUse() <= 0 )
	{
		pUserEtcItem->DeleteEtcItem( rkSlot.m_iType, LogDBClient::ET_DEL );
	}
	else
	{
		pUserEtcItem->SetEtcItem( rkSlot );
	}
	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Use Etc Item (%s/%d/%d)", pUser->GetPublicID().c_str(), rkSlot.m_iType, rkSlot.m_iValue1 );

	SP2Packet kSPacket( SSTPK_WHOLE_CHAT );
	PACKET_GUARD_bool( kSPacket.Write(pUser->GetPublicID()) );
	PACKET_GUARD_bool( kSPacket.Write(szChat) );

	g_ServerNodeManager.SendMessageAllNode( kSPacket );

	SP2Packet kPacket ( STPK_WHOLE_CHAT );
	PACKET_GUARD_bool( kPacket.Write(pUser->GetPublicID()) );
	PACKET_GUARD_bool( kPacket.Write(szChat) );
	
	g_UserNodeManager.SendMessageAll( kPacket, pUser );

	return true;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemGrowthDown::ioEtcItemGrowthDown()
{
}

ioEtcItemGrowthDown::~ioEtcItemGrowthDown()
{
}

bool ioEtcItemGrowthDown::_OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	// OnUse에서 체크하므로 pUser 와 pUserEtcItem NULL이 아님을 보장

	//
	bool bItem = false;
	int iClassType = 0;
	int iSlot = 0;
	
	PACKET_GUARD_bool( rkPacket.Read(iClassType) );
	PACKET_GUARD_bool( rkPacket.Read(bItem) );
	PACKET_GUARD_bool( rkPacket.Read(iSlot) );

	int iCurLevel = 0;
	int iDownLevel = 1;

	ioUserGrowthLevel *pLevel = pUser->GetUserGrowthLevel();
	if( !pLevel )
	{
		SP2Packet kPacket( STPK_GROWTH_LEVEL_DOWN );
		PACKET_GUARD_bool( kPacket.Write(pUser->GetPublicID()) );
		PACKET_GUARD_bool( kPacket.Write(GROWTH_DOWN_FAIL) );
		pUser->SendMessage( kPacket );
		return false;
	}

	if( bItem )
		iCurLevel = pLevel->GetItemGrowthLevel( iClassType, iSlot, true );
	else
		iCurLevel = pLevel->GetCharGrowthLevel( iClassType, iSlot, true );

	if( iCurLevel <= 0 )
	{
		SP2Packet kPacket( STPK_GROWTH_LEVEL_DOWN );
		PACKET_GUARD_bool( kPacket.Write(pUser->GetPublicID()) );
		PACKET_GUARD_bool( kPacket.Write(GROWTH_DOWN_FAIL) );
		pUser->SendMessage( kPacket );
		return false;
	}

	int iTimeSlot = 0;
	if( bItem )
		iTimeSlot = iSlot + 1;
	else
		iTimeSlot = iSlot + 1 + 4;

	if( pLevel->HasTimeGrowthValue( iClassType, iTimeSlot ) )
	{
		SP2Packet kPacket( STPK_GROWTH_LEVEL_DOWN );
		PACKET_GUARD_bool( kPacket.Write(pUser->GetPublicID()) );
		PACKET_GUARD_bool( kPacket.Write(GROWTH_DOWN_TIME_GROWTH) );
		pUser->SendMessage( kPacket );
		return false;
	}


	// 포인트 처리
	int iCurPoint = pLevel->GetCharGrowthPoint( iClassType );
	int iNeedPoint = g_GrowthMgr.GetGrowthUpNeedPoint( !bItem );
	iCurPoint += iNeedPoint;

	pLevel->SetCharGrowthPoint( iClassType, iCurPoint );

	// 레벨 처리
	if( bItem )
		pLevel->ItemGrowthLevelDown( iClassType, iSlot, iDownLevel );
	else
		pLevel->CharGrowthLevelDown( iClassType, iSlot, iDownLevel );

	// 페소 처리
	__int64 iReturnPeso = g_GrowthMgr.GetGrowthReturnPeso( !bItem, iCurLevel );
	if( iReturnPeso > 0 )
	{
		pUser->AddMoney( iReturnPeso );
		g_LogDBClient.OnInsertPeso( pUser, iReturnPeso, LogDBClient::PT_GROWTH_DOWN );
		g_LogDBClient.OnInsertGameLogInfo(LogDBClient::GLT_PESO_GAIN, pUser, 0, 0, LogDBClient::LET_ITEM, LogDBClient::PT_GROWTH_DOWN, PRESENT_ETC_ITEM, rkSlot.m_iType, iReturnPeso, NULL);
	}

	rkSlot.AddUse( -1 );
	if( rkSlot.GetUse() <= 0 )
	{
		pUserEtcItem->DeleteEtcItem( rkSlot.m_iType, LogDBClient::ET_DEL );
	}
	else
	{
		pUserEtcItem->SetEtcItem( rkSlot );
	}
	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "Use Etc Item (%s/%d/%d)", pUser->GetPublicID().c_str(), rkSlot.m_iType, rkSlot.m_iValue1 );


	// 특별아이템 소모처리를 먼저 발생시켜야 한다
	SP2Packet kReturn( STPK_ETCITEM_USE );
	PACKET_GUARD_bool( kReturn.Write(ETCITEM_USE_OK) );
	PACKET_GUARD_bool( kReturn.Write(rkSlot.m_iType) );
	PACKET_GUARD_bool( kReturn.Write(rkSlot.m_iValue1) );
	PACKET_GUARD_bool( kReturn.Write(rkSlot.m_iValue2) );

	pUser->SendMessage( kReturn );


	SP2Packet kPacket( STPK_GROWTH_LEVEL_DOWN );
	PACKET_GUARD_bool( kPacket.Write(pUser->GetPublicID()) );
	PACKET_GUARD_bool( kPacket.Write(GROWTH_DOWN_SUCESS) );
	PACKET_GUARD_bool( kPacket.Write(iClassType) );
	PACKET_GUARD_bool( kPacket.Write(bItem) );
	PACKET_GUARD_bool( kPacket.Write(iSlot) );
	PACKET_GUARD_bool( kPacket.Write(iCurPoint) );
	PACKET_GUARD_bool( kPacket.Write(iNeedPoint) );
	PACKET_GUARD_bool( kPacket.Write(iDownLevel) );
	PACKET_GUARD_bool( kPacket.Write(pUser->GetMoney()) );

	if( pUser->GetMyRoom() )
	{
		pUser->GetMyRoom()->RoomSendPacketTcp( kPacket );
		pUser->GetMyRoom()->OnModeCharGrowthUpdate( pUser, iClassType, iSlot, bItem, -iDownLevel );
	}
	else
		pUser->SendMessage( kPacket );
	//

	return false; // false 면 OnUse() 여기서 중단
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemFishingBait::ioEtcItemFishingBait()
{
}

ioEtcItemFishingBait::~ioEtcItemFishingBait()
{

}

bool ioEtcItemFishingBait::_OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	// OnUse에서 체크하므로 pUser 와 pUserEtcItem NULL이 아님을 보장
	rkSlot.AddUse( -1 );
	if( rkSlot.GetUse() <= 0 )
	{
		pUserEtcItem->DeleteEtcItem( rkSlot.m_iType, LogDBClient::ET_DEL );
	}
	else
	{
		pUserEtcItem->SetEtcItem( rkSlot );
	}
	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "Use Etc Item (%s/%d/%d)", pUser->GetPublicID().c_str(), rkSlot.m_iType, rkSlot.m_iValue1 );

	return true;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemFishingMoonBait::ioEtcItemFishingMoonBait()
{
}

ioEtcItemFishingMoonBait::~ioEtcItemFishingMoonBait()
{

}

bool ioEtcItemFishingMoonBait::_OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	// OnUse에서 체크하므로 pUser 와 pUserEtcItem NULL이 아님을 보장
	rkSlot.AddUse( -1 );
	if( rkSlot.GetUse() <= 0 )
	{
		pUserEtcItem->DeleteEtcItem( rkSlot.m_iType, LogDBClient::ET_DEL );
	}
	else
	{
		pUserEtcItem->SetEtcItem( rkSlot );
	}
	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "Use Etc Item (%s/%d/%d)", pUser->GetPublicID().c_str(), rkSlot.m_iType, rkSlot.m_iValue1 );

	return true;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemFishingRod::ioEtcItemFishingRod()
{

}

ioEtcItemFishingRod::~ioEtcItemFishingRod()
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemFishingMoonRod::ioEtcItemFishingMoonRod()
{

}

ioEtcItemFishingMoonRod::~ioEtcItemFishingMoonRod()
{

}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 2020-02-17 by bckim, 낚시 시스템 확장
ioEtcItemPrivateFishing::ioEtcItemPrivateFishing()
{

}

ioEtcItemPrivateFishing::~ioEtcItemPrivateFishing()
{

}
// End. 2020-02-17 by bckim, 낚시 시스템 확장

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemFriendSlotExtend::ioEtcItemFriendSlotExtend()
{

}

ioEtcItemFriendSlotExtend::~ioEtcItemFriendSlotExtend()
{

}

bool ioEtcItemFriendSlotExtend::IsBuyCondition( int iUse )
{
	if( iUse >= m_iMaxUse )
		return false;

	return true;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemCharSlotExtend::ioEtcItemCharSlotExtend()
{

}

ioEtcItemCharSlotExtend::~ioEtcItemCharSlotExtend()
{

}

bool ioEtcItemCharSlotExtend::IsBuyCondition( int iUse )
{
	if( iUse >= m_iMaxUse )
		return false;

	return true;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemFishingSlotExtend::ioEtcItemFishingSlotExtend()
{

}

ioEtcItemFishingSlotExtend::~ioEtcItemFishingSlotExtend()
{

}

bool ioEtcItemFishingSlotExtend::IsBuyCondition( int iUse )
{
	if( iUse >= m_iMaxUse )
		return false;

	return true;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemPesoExpBonus::ioEtcItemPesoExpBonus()
{

}

ioEtcItemPesoExpBonus::~ioEtcItemPesoExpBonus()
{

}

bool ioEtcItemPesoExpBonus::IsUpdateTime( Room *pRoom , User *pUser )
{
	if( !pRoom ) 
		return false;

	if( !pRoom->IsCharLimitCheck( pUser ) ) 
		return false;

	if( pRoom->GetModeType() == MT_TRAINING || pRoom->GetModeType() == MT_HEADQUARTERS || pRoom->GetModeType() == MT_HOUSE )
		return false;

	return true;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemExtraItemCompound::ioEtcItemExtraItemCompound()
{
}

ioEtcItemExtraItemCompound::~ioEtcItemExtraItemCompound()
{
}

bool ioEtcItemExtraItemCompound::_OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	// OnUse에서 체크하므로 pUser 와 pUserEtcItem NULL이 아님을 보장
	bool bSuccess = pUser->OnItemCompound( rkPacket, GetType() );
	if( !bSuccess )
	{
		// 실패 정보는 OnItemCompound()에서 처리
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Fail - %s",  __FUNCTION__, pUser->GetPublicID().c_str() );
		return false;
	}

	rkSlot.AddUse( -1 );
	if( rkSlot.GetUse() <= 0 )
	{
		pUserEtcItem->DeleteEtcItem( rkSlot.m_iType, LogDBClient::ET_DEL );
	}
	else
	{
		pUserEtcItem->SetEtcItem( rkSlot );
	}
	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "Use Etc Item (%s/%d/%d)", pUser->GetPublicID().c_str(), rkSlot.m_iType, rkSlot.m_iValue1 );

	return true;
}

////////////////////////////////////////////////////////////
bool ioEtcItemMaterialCompound::_OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	// OnUse에서 체크하므로 pUser 와 pUserEtcItem NULL이 아님을 보장
	bool bSuccess = pUser->OnItemMaterialCompound( rkPacket, GetType() );
	if( !bSuccess )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Fail - %s",  __FUNCTION__, pUser->GetPublicID().c_str() );
		return false;
	}

	rkSlot.AddUse( -1 );
	if( rkSlot.GetUse() <= 0 )
	{
		pUserEtcItem->DeleteEtcItem( rkSlot.m_iType, LogDBClient::ET_DEL );
	}
	else
	{
		pUserEtcItem->SetEtcItem( rkSlot );
	}

	return true;
}
////////////////////////////////////////////////////////////////

ioEtcItemExtraItemCompound2::ioEtcItemExtraItemCompound2()
{
}

ioEtcItemExtraItemCompound2::~ioEtcItemExtraItemCompound2()
{
}

bool ioEtcItemExtraItemCompound2::_OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	// OnUse에서 체크하므로 pUser 와 pUserEtcItem NULL이 아님을 보장
	bool bSuccess = pUser->OnItemCompound( rkPacket, GetType() );
	if( !bSuccess )
	{
		// 실패 정보는 OnItemCompound()에서 처리
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Fail - %s",  __FUNCTION__, pUser->GetPublicID().c_str() );
		return false;
	}

	rkSlot.AddUse( -1 );
	if( rkSlot.GetUse() <= 0 )
	{
		pUserEtcItem->DeleteEtcItem( rkSlot.m_iType, LogDBClient::ET_DEL );
	}
	else
	{
		pUserEtcItem->SetEtcItem( rkSlot );
	}
	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "Use Etc Item (%s/%d/%d)", pUser->GetPublicID().c_str(), rkSlot.m_iType, rkSlot.m_iValue1 );

	return true;
}

///////////
ioEtcItemExtraItemCompound3::ioEtcItemExtraItemCompound3()
{
}

ioEtcItemExtraItemCompound3::~ioEtcItemExtraItemCompound3()
{
}

bool ioEtcItemExtraItemCompound3::_OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	// OnUse에서 체크하므로 pUser 와 pUserEtcItem NULL이 아님을 보장
	bool bSuccess = pUser->OnItemCompound( rkPacket, GetType() );
	if( !bSuccess )
	{
		// 실패 정보는 OnItemCompound()에서 처리
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Fail - %s",  __FUNCTION__, pUser->GetPublicID().c_str() );
		return false;
	}

	rkSlot.AddUse( -1 );
	if( rkSlot.GetUse() <= 0 )
	{
		pUserEtcItem->DeleteEtcItem( rkSlot.m_iType, LogDBClient::ET_DEL );
	}
	else
	{
		pUserEtcItem->SetEtcItem( rkSlot );
	}
	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "Use Etc Item (%s/%d/%d)", pUser->GetPublicID().c_str(), rkSlot.m_iType, rkSlot.m_iValue1 );

	return true;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemMultipleCompound::ioEtcItemMultipleCompound()
{
}

ioEtcItemMultipleCompound::~ioEtcItemMultipleCompound()
{
}

bool ioEtcItemMultipleCompound::_OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	// OnUse에서 체크하므로 pUser 와 pUserEtcItem NULL이 아님을 보장
	bool bSuccess = pUser->OnMultipleItemCompound( rkPacket, GetType() );
	if( !bSuccess )
	{
		// 실패 정보는 OnMultipleItemCompound()에서 처리
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Fail - %s",  __FUNCTION__, pUser->GetPublicID().c_str() );
		return false;
	}

	rkSlot.AddUse( -1 );
	if( rkSlot.GetUse() <= 0 )
	{
		pUserEtcItem->DeleteEtcItem( rkSlot.m_iType, LogDBClient::ET_DEL );
	}
	else
	{
		pUserEtcItem->SetEtcItem( rkSlot );
	}
	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "Use Etc Item (%s/%d/%d)", pUser->GetPublicID().c_str(), rkSlot.m_iType, rkSlot.m_iValue1 );

	return true;
}

////////////
ioEtcItemMultipleCompound2::ioEtcItemMultipleCompound2()
{
}

ioEtcItemMultipleCompound2::~ioEtcItemMultipleCompound2()
{
}

bool ioEtcItemMultipleCompound2::_OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	// OnUse에서 체크하므로 pUser 와 pUserEtcItem NULL이 아님을 보장
	bool bSuccess = pUser->OnMultipleItemCompound( rkPacket, GetType() );
	if( !bSuccess )
	{
		// 실패 정보는 OnMultipleItemCompound()에서 처리
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Fail - %s",  __FUNCTION__, pUser->GetPublicID().c_str() );
		return false;
	}

	rkSlot.AddUse( -1 );
	if( rkSlot.GetUse() <= 0 )
	{
		pUserEtcItem->DeleteEtcItem( rkSlot.m_iType, LogDBClient::ET_DEL );
	}
	else
	{
		pUserEtcItem->SetEtcItem( rkSlot );
	}
	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "Use Etc Item (%s/%d/%d)", pUser->GetPublicID().c_str(), rkSlot.m_iType, rkSlot.m_iValue1 );

	return true;
}

////////
ioEtcItemMultipleCompound3::ioEtcItemMultipleCompound3()
{
}

ioEtcItemMultipleCompound3::~ioEtcItemMultipleCompound3()
{
}

bool ioEtcItemMultipleCompound3::_OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	// OnUse에서 체크하므로 pUser 와 pUserEtcItem NULL이 아님을 보장
	bool bSuccess = pUser->OnMultipleItemCompound( rkPacket, GetType() );
	if( !bSuccess )
	{
		// 실패 정보는 OnMultipleItemCompound()에서 처리
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Fail - %s",  __FUNCTION__, pUser->GetPublicID().c_str() );
		return false;
	}

	rkSlot.AddUse( -1 );
	if( rkSlot.GetUse() <= 0 )
	{
		pUserEtcItem->DeleteEtcItem( rkSlot.m_iType, LogDBClient::ET_DEL );
	}
	else
	{
		pUserEtcItem->SetEtcItem( rkSlot );
	}
	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "Use Etc Item (%s/%d/%d)", pUser->GetPublicID().c_str(), rkSlot.m_iType, rkSlot.m_iValue1 );

	return true;
}

////////
ioEtcItemMultipleCompound4::ioEtcItemMultipleCompound4()
{
}

ioEtcItemMultipleCompound4::~ioEtcItemMultipleCompound4()
{
}

bool ioEtcItemMultipleCompound4::_OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	// OnUse에서 체크하므로 pUser 와 pUserEtcItem NULL이 아님을 보장
	bool bSuccess = pUser->OnMultipleItemCompound( rkPacket, GetType() );
	if( !bSuccess )
	{
		// 실패 정보는 OnMultipleItemCompound()에서 처리
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Fail - %s",  __FUNCTION__, pUser->GetPublicID().c_str() );
		return false;
	}

	rkSlot.AddUse( -1 );
	if( rkSlot.GetUse() <= 0 )
	{
		pUserEtcItem->DeleteEtcItem( rkSlot.m_iType, LogDBClient::ET_DEL );
	}
	else
	{
		pUserEtcItem->SetEtcItem( rkSlot );
	}
	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "Use Etc Item (%s/%d/%d)", pUser->GetPublicID().c_str(), rkSlot.m_iType, rkSlot.m_iValue1 );

	return true;
}


////////
ioEtcItemMultipleCompound5::ioEtcItemMultipleCompound5()
{
}

ioEtcItemMultipleCompound5::~ioEtcItemMultipleCompound5()
{
}

bool ioEtcItemMultipleCompound5::_OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	// OnUse에서 체크하므로 pUser 와 pUserEtcItem NULL이 아님을 보장
	bool bSuccess = pUser->OnMultipleItemCompound( rkPacket, GetType() );
	if( !bSuccess )
	{
		// 실패 정보는 OnMultipleItemCompound()에서 처리
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Fail - %s",  __FUNCTION__, pUser->GetPublicID().c_str() );
		return false;
	}

	rkSlot.AddUse( -1 );
	if( rkSlot.GetUse() <= 0 )
	{
		pUserEtcItem->DeleteEtcItem( rkSlot.m_iType, LogDBClient::ET_DEL );
	}
	else
	{
		pUserEtcItem->SetEtcItem( rkSlot );
	}
	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "Use Etc Item (%s/%d/%d)", pUser->GetPublicID().c_str(), rkSlot.m_iType, rkSlot.m_iValue1 );

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemMultipleEqualCompound::ioEtcItemMultipleEqualCompound()
{
}

ioEtcItemMultipleEqualCompound::~ioEtcItemMultipleEqualCompound()
{
}

bool ioEtcItemMultipleEqualCompound::_OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	// OnUse에서 체크하므로 pUser 와 pUserEtcItem NULL이 아님을 보장
	bool bSuccess = pUser->OnMultipleItemCompound( rkPacket, GetType() );
	if( !bSuccess )
	{
		// 실패 정보는 OnMultipleItemCompound()에서 처리
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Fail - %s",  __FUNCTION__, pUser->GetPublicID().c_str() );
		return false;
	}

	rkSlot.AddUse( -1 );
	if( rkSlot.GetUse() <= 0 )
	{
		pUserEtcItem->DeleteEtcItem( rkSlot.m_iType, LogDBClient::ET_DEL );
	}
	else
	{
		pUserEtcItem->SetEtcItem( rkSlot );
	}
	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "Use Etc Item (%s/%d/%d)", pUser->GetPublicID().c_str(), rkSlot.m_iType, rkSlot.m_iValue1 );

	return true;
}

////////
ioEtcItemMultipleEqualCompound2::ioEtcItemMultipleEqualCompound2()
{
}

ioEtcItemMultipleEqualCompound2::~ioEtcItemMultipleEqualCompound2()
{
}

bool ioEtcItemMultipleEqualCompound2::_OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	// OnUse에서 체크하므로 pUser 와 pUserEtcItem NULL이 아님을 보장
	bool bSuccess = pUser->OnMultipleItemCompound( rkPacket, GetType() );
	if( !bSuccess )
	{
		// 실패 정보는 OnMultipleItemCompound()에서 처리
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Fail - %s",  __FUNCTION__, pUser->GetPublicID().c_str() );
		return false;
	}

	rkSlot.AddUse( -1 );
	if( rkSlot.GetUse() <= 0 )
	{
		pUserEtcItem->DeleteEtcItem( rkSlot.m_iType, LogDBClient::ET_DEL );
	}
	else
	{
		pUserEtcItem->SetEtcItem( rkSlot );
	}
	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "Use Etc Item (%s/%d/%d)", pUser->GetPublicID().c_str(), rkSlot.m_iType, rkSlot.m_iValue1 );

	return true;
}

/////////
ioEtcItemMultipleEqualCompound3::ioEtcItemMultipleEqualCompound3()
{
}

ioEtcItemMultipleEqualCompound3::~ioEtcItemMultipleEqualCompound3()
{
}

bool ioEtcItemMultipleEqualCompound3::_OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	// OnUse에서 체크하므로 pUser 와 pUserEtcItem NULL이 아님을 보장
	bool bSuccess = pUser->OnMultipleItemCompound( rkPacket, GetType() );
	if( !bSuccess )
	{
		// 실패 정보는 OnMultipleItemCompound()에서 처리
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Fail - %s",  __FUNCTION__, pUser->GetPublicID().c_str() );
		return false;
	}

	rkSlot.AddUse( -1 );
	if( rkSlot.GetUse() <= 0 )
	{
		pUserEtcItem->DeleteEtcItem( rkSlot.m_iType, LogDBClient::ET_DEL );
	}
	else
	{
		pUserEtcItem->SetEtcItem( rkSlot );
	}
	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "Use Etc Item (%s/%d/%d)", pUser->GetPublicID().c_str(), rkSlot.m_iType, rkSlot.m_iValue1 );

	return true;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemChangeID::ioEtcItemChangeID()
{

}

ioEtcItemChangeID::~ioEtcItemChangeID()
{

}


void ioEtcItemChangeID::OnBuy( User *pUser, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	rkSlot.SetUse( false );
}

bool ioEtcItemChangeID::_OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	// OnUse에서 체크하므로 pUser 와 pUserEtcItem NULL이 아님을 보장
	if( pUser->IsReserveServerMoving() )
	{
		SP2Packet kReturn( STPK_ETCITEM_USE );
		PACKET_GUARD_bool( kReturn.Write(ETCITEM_USE_MOVING_SERVER) );
		pUser->SendMessage( kReturn );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Fail Reserve Server moving - %s) %d.",  __FUNCTION__, pUser->GetPublicID().c_str(), rkSlot.m_iType );
		return false;
	}

	if( pUser->IsNewPublicID() )
	{
		SP2Packet kReturn( STPK_ETCITEM_USE );
		PACKET_GUARD_bool( kReturn.Write(ETCITEM_USE_RESERVED_CHANGE_ID) );
		pUser->SendMessage( kReturn );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Fail Reserve change id - %s) %d.",  __FUNCTION__, pUser->GetPublicID().c_str(), rkSlot.m_iType );
		return false;
	}
	ioHashString szNewPublicID;
	PACKET_GUARD_bool( rkPacket.Read(szNewPublicID) );

	if( szNewPublicID.IsEmpty() )
	{
		SP2Packet kReturn( STPK_ETCITEM_USE );
		PACKET_GUARD_bool( kReturn.Write(ETCITEM_USE_EXCEPTION) );
		pUser->SendMessage( kReturn );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Fail - %s) %d.", __FUNCTION__, pUser->GetPublicID().c_str(), rkSlot.m_iType );
		return false;
	}

	if( !g_WordFilter.CheckSpecialLetters(szNewPublicID.c_str(), szNewPublicID.Length()) )
	{
		SP2Packet kReturn( STPK_ETCITEM_USE );
		PACKET_GUARD_bool( kReturn.Write(ETCITEM_USE_WRONG_ID) );
		pUser->SendMessage( kReturn );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Id is wrong. - %s) %d | %s.", __FUNCTION__, pUser->GetPublicID().c_str(), rkSlot.m_iType, szNewPublicID.c_str() );
		return false;
	}

	if( !g_App.IsRightID( szNewPublicID.c_str() ) )
	{
		SP2Packet kReturn( STPK_ETCITEM_USE );
		PACKET_GUARD_bool( kReturn.Write(ETCITEM_USE_WRONG_ID) );
		pUser->SendMessage( kReturn );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Id is wrong. - %s) %d | %s.", __FUNCTION__, pUser->GetPublicID().c_str(), rkSlot.m_iType, szNewPublicID.c_str() );
		return false;
	}

	if( g_App.IsNotMakeID( szNewPublicID.c_str() ) )
	{
		SP2Packet kReturn( STPK_ETCITEM_USE );
		PACKET_GUARD_bool( kReturn.Write(ETCITEM_USE_WRONG_ID) );
		pUser->SendMessage( kReturn );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Id not make. - %s) %d | %s.", __FUNCTION__, pUser->GetPublicID().c_str(), rkSlot.m_iType, szNewPublicID.c_str() );
		return false;
	}

	if( g_UserNodeManager.IsDeveloper( szNewPublicID.c_str() ) )
	{
		SP2Packet kReturn( STPK_ETCITEM_USE );
		PACKET_GUARD_bool( kReturn.Write(ETCITEM_USE_WRONG_ID) );
		pUser->SendMessage( kReturn );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Id is developer. - %s) %d | %s.", __FUNCTION__,  pUser->GetPublicID().c_str(), rkSlot.m_iType, szNewPublicID.c_str() );
		return false;
	}

	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( pLocal && !pLocal->IsRightNewID( szNewPublicID.c_str() ) )
	{
		SP2Packet kReturn( STPK_ETCITEM_USE );
		PACKET_GUARD_bool( kReturn.Write(ETCITEM_USE_WRONG_ID) );
		pUser->SendMessage( kReturn );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Id is wrong(Local). - %s) %d | %s.", __FUNCTION__,  pUser->GetPublicID().c_str(), rkSlot.m_iType, szNewPublicID.c_str() );
		return false;
	}

	// etc 아이템 삭제는 종료시에 처리함.
	pUser->SetNewPublicID( szNewPublicID.c_str() );
	pUser->SetPublicIPForNewPublicID( pUser->GetPublicIP() );
	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "Use Etc Item (%s/%d/%d/%s)", pUser->GetPublicID().c_str(), rkSlot.m_iType, rkSlot.m_iValue1, szNewPublicID.c_str() );
	g_DBClient.OnSelectPublicIDExist( pUser->GetUserDBAgentID(), pUser->GetAgentThreadID(),pUser->GetUserIndex(), szNewPublicID );

	return false; // DB 결과 확인후 완료 메세지 전송하므로 false
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemCustomSound::ioEtcItemCustomSound()
{

}

ioEtcItemCustomSound::~ioEtcItemCustomSound()
{

}

bool ioEtcItemCustomSound::_OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	// OnUse에서 체크하므로 pUser 와 pUserEtcItem NULL이 아님을 보장
	rkSlot.AddUse( -1 );
	if( rkSlot.GetUse() <= 0 )
	{
		pUserEtcItem->DeleteEtcItem( rkSlot.m_iType, LogDBClient::ET_DEL );
	}
	else
	{
		pUserEtcItem->SetEtcItem( rkSlot );
	}
	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "Use Etc Item (%s/%d/%d)", pUser->GetPublicID().c_str(), rkSlot.m_iType, rkSlot.m_iValue1 );

	return true;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemBuyMortmainChar::ioEtcItemBuyMortmainChar()
{

}

ioEtcItemBuyMortmainChar::~ioEtcItemBuyMortmainChar()
{

}

bool ioEtcItemBuyMortmainChar::SetUsed( User *pUser, ioUserEtcItem *pUserEtcItem )
{
	if( !pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pUser == NULL", __FUNCTION__ );
		return false;
	}

	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pUserEtcItem == NULL", __FUNCTION__ );
		return false;
	}

	ioUserEtcItem::ETCITEMSLOT kSlot;
	if( !pUserEtcItem->GetEtcItem( ioEtcItem::EIT_ETC_BUY_MORTMAIN_CHAR, kSlot ) )
		return false;

	if( kSlot.GetUse() <= 0 )
		return false;

	kSlot.AddUse( -1 );
	if( kSlot.GetUse() <= 0 )
		pUserEtcItem->DeleteEtcItem( ioEtcItem::EIT_ETC_BUY_MORTMAIN_CHAR, LogDBClient::ET_DEL );
	else
		pUserEtcItem->SetEtcItem( kSlot );
	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "%s Use Etc Item (%s/%d/%d)", __FUNCTION__, pUser->GetPublicID().c_str(), kSlot.m_iType, kSlot.m_iValue1 );
	pUser->SaveEtcItem();
	return true;
}

bool ioEtcItemBuyMortmainChar::AddEtcItem( User *pUser, ioUserEtcItem *pUserEtcItem )
{
	if( !pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pUser == NULL", __FUNCTION__ );
		return false;
	}

	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pUserEtcItem == NULL", __FUNCTION__ );
		return false;
	}

	// 현재사용하지 않음.
	//if( pUser->GetGradeLevel() != g_LevelMgr.GetGradeLevelForEtcBonus() )
		return false;
	
	int iAddNum = 1;
	ioUserEtcItem::ETCITEMSLOT kEtcItemSlot;
	pUserEtcItem->GetEtcItem( ioEtcItem::EIT_ETC_BUY_MORTMAIN_CHAR, kEtcItemSlot );

	kEtcItemSlot.m_iType = ioEtcItem::EIT_ETC_BUY_MORTMAIN_CHAR;   
	kEtcItemSlot.AddUse( iAddNum );
	kEtcItemSlot.SetUse( true );

	DWORD dwIndex       = 0;
	int   iArrayInIndex = 0;
	if( pUserEtcItem->AddEtcItem( kEtcItemSlot, false, 0, dwIndex, iArrayInIndex ) )	
	{
		char szItemIndex[MAX_PATH]="";
		if( dwIndex != 0 )
		{
			StringCbPrintf( szItemIndex, sizeof( szItemIndex ), "%u-%u", dwIndex, iArrayInIndex+1 ); // db field는 1부터 이므로 +1
			g_LogDBClient.OnInsertEtc( pUser, GetType(), iAddNum, 0, szItemIndex, LogDBClient::ET_BUY );
		}
		pUser->SaveEtcItem();
	}
	else
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Fail Etcitem : %s(%d)",__FUNCTION__, pUser->GetPublicID().c_str(), pUser->GetUserIndex() );
		return false;
	}

	return true;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemGuildCreate::ioEtcItemGuildCreate()
{

}

ioEtcItemGuildCreate::~ioEtcItemGuildCreate()
{

}


void ioEtcItemGuildCreate::OnBuy( User *pUser, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	rkSlot.SetUse( false );
}

bool ioEtcItemGuildCreate::_OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	// OnUse에서 체크하므로 pUser 와 pUserEtcItem NULL이 아님을 보장
	if( pUser->IsReserveServerMoving() )
	{
		SP2Packet kReturn( STPK_ETCITEM_USE );
		PACKET_GUARD_bool( kReturn.Write(ETCITEM_USE_MOVING_SERVER) );
		pUser->SendMessage( kReturn );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Fail Reserve Server moving - %s) %d.",  __FUNCTION__, pUser->GetPublicID().c_str(), rkSlot.m_iType );
		return false;
	}

	// 길드 생성 프로세스
	ioHashString szGuildName;
	int          iGuildMark = 0;

	PACKET_GUARD_bool( rkPacket.Read(szGuildName) );
	PACKET_GUARD_bool( rkPacket.Read(iGuildMark) );

	if( pUser->IsGuild() )
	{
		//이미 길드원이다.
		SP2Packet kPacket( STPK_CREATE_GUILD );
		PACKET_GUARD_bool( kPacket.Write(CREATE_GUILD_ALREADY_GUILD) );
		pUser->SendMessage( kPacket );

		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "이미 길드원인 유저가 길드 생성 : %s - %s", pUser->GetPublicID().c_str(), szGuildName.c_str() );
		return false;
	}
	else if( !g_App.IsRightID( szGuildName.c_str() ) )
	{
		//잘못된 아이디
		SP2Packet kPacket( STPK_CREATE_GUILD );
		PACKET_GUARD_bool( kPacket.Write(CREATE_GUILD_NAME_FAILED) );
		pUser->SendMessage( kPacket );

		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "잘못된 길드명 전송됨(1) : %s", pUser->GetPublicID().c_str() );
		return false;
	}
	else if( g_App.IsNotMakeID( szGuildName.c_str() ) )
	{
		//잘못된 아이디
		SP2Packet kPacket( STPK_CREATE_GUILD );
		PACKET_GUARD_bool( kPacket.Write(CREATE_GUILD_NAME_FAILED) );
		pUser->SendMessage( kPacket );

		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "잘못된 길드명 전송됨(2) : %s", pUser->GetPublicID().c_str() );
		return false;
	}
	else if( pUser->GetEntryType() == ET_TERMINATION )
	{
		SP2Packet kPacket( STPK_CREATE_GUILD );
		PACKET_GUARD_bool( kPacket.Write(CREATE_GUILD_NONE_FORMALITY) );
		pUser->SendMessage( kPacket );

		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "임시가입만료 유저가 길드 생성 요청 : %s / %d", pUser->GetPublicID().c_str(), pUser->GetEntryType() );
		return false;
	}
/*	   진영에 가입되어있지 않아도 길드 생성 가능. 2012.09.18
	else if( pUser->GetUserCampPos() == CAMP_NONE )
	{
		SP2Packet kPacket( STPK_CREATE_GUILD );
		kPacket << CREATE_GUILD_CAMP_NONE;
		pUser->SendMessage( kPacket );

		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "진영에 소속되지 않은 유저 길드 생성 : %s / %d", pUser->GetPublicID().c_str(), pUser->GetUserCampPos() );
		return false;
	}
*/
	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( pLocal && !pLocal->IsRightNewID( szGuildName.c_str() ) )
	{
		//잘못된 아이디
		SP2Packet kPacket( STPK_CREATE_GUILD );
		PACKET_GUARD_bool( kPacket.Write(CREATE_GUILD_NAME_FAILED) );
		pUser->SendMessage( kPacket );

		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "잘못된 길드명 전송됨(3) : %s", pUser->GetPublicID().c_str() );
		return false;
	}

	DWORD dwTableIndex = 0;
	int   iFieldNum   = 0;
	pUserEtcItem->GetEtcItemIndex( GetType(), dwTableIndex, iFieldNum );
	if( dwTableIndex == 0 || dwTableIndex == ioDBDataController::NEW_INDEX )
	{
		//아직 테이블이 Insert되지 않았다.
		SP2Packet kReturn( STPK_ETCITEM_USE );
		PACKET_GUARD_bool( kReturn.Write(ETCITEM_USE_EXCEPTION) );
		pUser->SendMessage( kReturn );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "아직 길드 생성권이 DB에 Insert되지 않았다 : %s / %d - %d", pUser->GetPublicID().c_str(), dwTableIndex, iFieldNum );
		return false;
	}
	
	//길드 생성 요청
	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "CreateGuild : %s - %s", pUser->GetPublicID().c_str(), szGuildName.c_str() );

	//성공여부를 확인한 다음 권한 아이템 삭제한다.
	char szGuildTitle[GUILD_TITLE_NUMBER_PLUS_ONE] = "";
	if( pLocal )
		pLocal->GetGuildTitle( szGuildTitle, sizeof( szGuildTitle ), szGuildName.c_str() );
	g_DBClient.OnSelectCreateGuild( pUser->GetUserDBAgentID(), pUser->GetAgentThreadID(), pUser->GetUserIndex(), szGuildName, szGuildTitle, iGuildMark, dwTableIndex, iFieldNum, GUILD_MAX_ENTRY_USER );
	return false; 
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemGuildMarkChange::ioEtcItemGuildMarkChange()
{

}

ioEtcItemGuildMarkChange::~ioEtcItemGuildMarkChange()
{

}


void ioEtcItemGuildMarkChange::OnBuy( User *pUser, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	rkSlot.SetUse( false );
}

bool ioEtcItemGuildMarkChange::_OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	// OnUse에서 체크하므로 pUser 와 pUserEtcItem NULL이 아님을 보장
	if( pUser->IsReserveServerMoving() )
	{
		SP2Packet kReturn( STPK_ETCITEM_USE );
		PACKET_GUARD_bool( kReturn.Write(ETCITEM_USE_MOVING_SERVER) );
		pUser->SendMessage( kReturn );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Fail Reserve Server moving - %s) %d.",  __FUNCTION__, pUser->GetPublicID().c_str(), rkSlot.m_iType );
		return false;
	}

	// 길드 마크 변경 프로세스
	ioUserGuild *pUserGuild = pUser->GetUserGuild();
	if( !pUserGuild || !pUser->IsGuild() )
	{
		SP2Packet kPacket( STPK_GUILD_MARK_CHANGE );
		PACKET_GUARD_bool( kPacket.Write(GUILD_MARK_CHANGE_NOT_GUILD) );
		pUser->SendMessage( kPacket );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Fail Reserve Is Not Guild - %s) %d.",  __FUNCTION__, pUser->GetPublicID().c_str(), rkSlot.m_iType );
		return false;
	}
	else if( !pUserGuild->IsGuildMaster() && !pUserGuild->IsGuildSecondMaster() )           
	{
		SP2Packet kPacket( STPK_GUILD_MARK_CHANGE );
		PACKET_GUARD_bool( kPacket.Write(GUILD_MARK_CHANGE_NOT_MASTER) );
		pUser->SendMessage( kPacket );
		return false;
	}

	// 권한 아이템 삭제
	pUserEtcItem->DeleteEtcItem( GetType(), LogDBClient::ET_DEL );
	SP2Packet kSuccess( STPK_ETCITEM_USE );
	PACKET_GUARD_bool( kSuccess.Write(ETCITEM_USE_OK) );
	PACKET_GUARD_bool( kSuccess.Write(GetType()) );
	pUser->SendMessage( kSuccess );

	DWORD dwGuildMark = 0;
	PACKET_GUARD_bool( rkPacket.Read(dwGuildMark) );

	pUser->_OnGuildMarkChange( pUserGuild->GetGuildIndex(), dwGuildMark );
	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "ChangeGuildMark Ok : %s - ( %d : %d )", pUser->GetPublicID().c_str(), pUserGuild->GetGuildIndex(), pUserGuild->GetGuildMark() );

	// DB Update
	g_DBClient.OnUpdateGuildMarkChange( pUser->GetUserDBAgentID(), pUser->GetAgentThreadID(), pUserGuild->GetGuildIndex(), pUserGuild->GetGuildMark() );

	// 메인 서버에 전송
	SP2Packet kMainPacket( MSTPK_GUILD_MARK_CHANGE );
	PACKET_GUARD_bool( kMainPacket.Write(pUserGuild->GetGuildIndex()) );
	PACKET_GUARD_bool( kMainPacket.Write(pUserGuild->GetGuildMark()) );
	g_MainServer.SendMessage( kMainPacket );

	// 현재 접속해있는 길드원에게 알림
	pUserGuild->GuildMarkChangeSync();
	return true; 
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemGuildNameChange::ioEtcItemGuildNameChange()
{

}

ioEtcItemGuildNameChange::~ioEtcItemGuildNameChange()
{

}


void ioEtcItemGuildNameChange::OnBuy( User *pUser, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	rkSlot.SetUse( false );
}

bool ioEtcItemGuildNameChange::_OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	// OnUse에서 체크하므로 pUser 와 pUserEtcItem NULL이 아님을 보장
	if( pUser->IsReserveServerMoving() )
	{
		SP2Packet kReturn( STPK_ETCITEM_USE );
		PACKET_GUARD_bool( kReturn.Write(ETCITEM_USE_MOVING_SERVER) );
		pUser->SendMessage( kReturn );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Fail Reserve Server moving - %s) %d.",  __FUNCTION__, pUser->GetPublicID().c_str(), rkSlot.m_iType );
		return false;
	}

	// 길드명 변경 프로세스
	DWORD dwGuildIndex = 0;
	ioHashString szGuildName;
	PACKET_GUARD_bool( rkPacket.Read(dwGuildIndex) );
	PACKET_GUARD_bool( rkPacket.Read(szGuildName) );

	ioUserGuild *pUserGuild = pUser->GetUserGuild();
	if( !pUserGuild || !pUser->IsGuild() )
	{
		//길드가 없다.
		SP2Packet kPacket( STPK_GUILD_NAME_CHANGE );
		PACKET_GUARD_bool( kPacket.Write(GUILD_NAME_CHANGE_NOT_GUILD) );
		PACKET_GUARD_bool( kPacket.Write(pUser->GetUserIndex()) );
		pUser->SendMessage( kPacket );

		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "길드없는 유저가 길드명 변경 : %s - %d:%s", pUser->GetPublicID().c_str(), dwGuildIndex, szGuildName.c_str() );
		return false;
	}
	else if( !pUserGuild->IsGuildMaster() )
	{
		//길드장이 아니다
		SP2Packet kPacket( STPK_GUILD_NAME_CHANGE );
		PACKET_GUARD_bool( kPacket.Write(GUILD_NAME_CHANGE_NOT_MASTER) );
		PACKET_GUARD_bool( kPacket.Write(pUser->GetUserIndex()) );
		pUser->SendMessage( kPacket );

		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "길드장이 아닌 유저가 길드명 변경 : %s - %d:%s", pUser->GetPublicID().c_str(), dwGuildIndex, szGuildName.c_str() );
		return false;
	}
	else if( !g_App.IsRightID( szGuildName.c_str() ) || g_App.IsNotMakeID( szGuildName.c_str() ) )
	{
		//잘못된 아이디
		SP2Packet kPacket( STPK_GUILD_NAME_CHANGE );
		PACKET_GUARD_bool( kPacket.Write(GUILD_NAME_CHANGE_FAILED) );
		PACKET_GUARD_bool( kPacket.Write(pUser->GetUserIndex()) );
		pUser->SendMessage( kPacket );

		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "잘못된 길드명 전송됨 : %s - %s", pUser->GetPublicID().c_str(), szGuildName.c_str() );
		return false;
	}

	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( pLocal && !pLocal->IsRightNewID( szGuildName.c_str() ) )
	{
		//잘못된 아이디
		SP2Packet kPacket( STPK_GUILD_NAME_CHANGE );
		PACKET_GUARD_bool( kPacket.Write(GUILD_NAME_CHANGE_FAILED) );
		PACKET_GUARD_bool( kPacket.Write(pUser->GetUserIndex()) );
		pUser->SendMessage( kPacket );

		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "잘못된 길드명 전송됨(Local) : %s - %s", pUser->GetPublicID().c_str(), szGuildName.c_str() );
		return false;
	}


	DWORD dwTableIndex = 0;
	int   iFieldNum   = 0;
	pUserEtcItem->GetEtcItemIndex( GetType(), dwTableIndex, iFieldNum );
	if( dwTableIndex == 0 || dwTableIndex == ioDBDataController::NEW_INDEX )
	{
		//아직 테이블이 Insert되지 않았다.
		SP2Packet kReturn( STPK_ETCITEM_USE );
		PACKET_GUARD_bool( kReturn.Write(ETCITEM_USE_EXCEPTION) );
		pUser->SendMessage( kReturn );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "아직 길드명 변경권이 DB에 Insert되지 않았다 : %s / %d - %d", pUser->GetPublicID().c_str(), dwTableIndex, iFieldNum );
		return false;
	}

	//길드명 변경 요청
	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "ChangeGuildName : %s - (%s - > %s)", pUser->GetPublicID().c_str(), pUserGuild->GetGuildName().c_str(), szGuildName.c_str() );

	//성공여부를 확인한 다음 권한 아이템 삭제한다.
	g_DBClient.OnSelectGuildNameChange( pUser->GetUserDBAgentID(), pUser->GetAgentThreadID(), pUser->GetUserIndex(), pUserGuild->GetGuildIndex(), szGuildName, dwTableIndex, iFieldNum );
	return false; 
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemSoldierPackage::ioEtcItemSoldierPackage()
{
	m_iLimitClassTypeNum  = 0;
	m_iActiveFilter		  = AF_ACTIVE;
}

ioEtcItemSoldierPackage::~ioEtcItemSoldierPackage()
{

}


void ioEtcItemSoldierPackage::OnBuy( User *pUser, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	rkSlot.SetUse( false );
}

bool ioEtcItemSoldierPackage::_OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	// OnUse에서 체크하므로 pUser 와 pUserEtcItem NULL이 아님을 보장
	if( pUser->IsReserveServerMoving() )
	{
		SP2Packet kReturn( STPK_ETCITEM_USE );
		PACKET_GUARD_bool( kReturn.Write(ETCITEM_USE_MOVING_SERVER) );
		pUser->SendMessage( kReturn );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Fail Reserve Server moving - %s) %d.",  __FUNCTION__, pUser->GetPublicID().c_str(), rkSlot.m_iType );
		return false;
	}
	
	int iClassType = 0;
	PACKET_GUARD_bool( rkPacket.Read(iClassType) );

	if( !IsRightSoldierCode( iClassType ) )
	{
		SP2Packet kReturn( STPK_ETCITEM_USE );
		PACKET_GUARD_bool( kReturn.Write( ETCITEM_USE_EXCEPTION ) );
		pUser->SendMessage( kReturn );

		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s - not right class type : %d", __FUNCTION__, iClassType );
		return false;
	}

	pUser->OnEtcItemSoldierPackage( rkPacket, m_dwType, iClassType, m_iActiveFilter );
	return false; 
}

bool ioEtcItemSoldierPackage::IsRightSoldierCode( int iClassType )
{
	if( m_vSoldierCode.empty() )
		return true;

	int iMax = m_vSoldierCode.size();
	for (int i = 0; i < iMax ; i++)
	{
		if( iClassType == m_vSoldierCode[i] )
		{
			return true;
		}
	}

	return false;
}

void ioEtcItemSoldierPackage::LoadProperty( ioINILoader &rkLoader )
{
	ioEtcItem::LoadProperty( rkLoader );

	m_iLimitClassTypeNum = rkLoader.LoadInt( "limit_class_num", 0 );
	m_iActiveFilter		 = rkLoader.LoadInt( "limit_active_filter", static_cast<int>( AF_ACTIVE ) );

	// Soldier
	char szKeyName[MAX_PATH]="";
	int iMax  = rkLoader.LoadInt( "max_soldier", 0 );
	m_vSoldierCode.clear();
	m_vSoldierCode.reserve( iMax );
	for (int i = 0; i < iMax ; i++)
	{
		StringCbPrintf( szKeyName, sizeof( szKeyName ), "soldier%d", i+1 );
		int iSoldierCode  = rkLoader.LoadInt( szKeyName, 0 );
		if( iSoldierCode != 0 )
			m_vSoldierCode.push_back( iSoldierCode );
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemDecorationPackage::ioEtcItemDecorationPackage()
{

}

ioEtcItemDecorationPackage::~ioEtcItemDecorationPackage()
{

}


void ioEtcItemDecorationPackage::OnBuy( User *pUser, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	rkSlot.SetUse( false );
}

bool ioEtcItemDecorationPackage::_OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	// OnUse에서 체크하므로 pUser 와 pUserEtcItem NULL이 아님을 보장
	if( pUser->IsReserveServerMoving() )
	{
		SP2Packet kReturn( STPK_ETCITEM_USE );
		PACKET_GUARD_bool( kReturn.Write(ETCITEM_USE_MOVING_SERVER) );
		pUser->SendMessage( kReturn );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Fail Reserve Server moving - %s) %d.",  __FUNCTION__, pUser->GetPublicID().c_str(), rkSlot.m_iType );
		return false;
	}

	pUser->OnEtcItemDecorationPackage( rkPacket, GetType() );
	return false; 
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemGoldMonsterCoin::ioEtcItemGoldMonsterCoin()
{
}

ioEtcItemGoldMonsterCoin::~ioEtcItemGoldMonsterCoin()
{

}

bool ioEtcItemGoldMonsterCoin::_OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	// OnUse에서 체크하므로 pUser 와 pUserEtcItem NULL이 아님을 보장	
	rkSlot.AddUse( -1 );
	if( rkSlot.GetUse() <= 0 )
	{
		pUserEtcItem->DeleteEtcItem( rkSlot.m_iType, LogDBClient::ET_DEL );
	}
	else
	{
		pUserEtcItem->SetEtcItem( rkSlot );
	}
	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "Use Etc Item (%s/%d/%d)", pUser->GetPublicID().c_str(), rkSlot.m_iType, rkSlot.m_iValue1 );

	return true;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemMonsterCoin::ioEtcItemMonsterCoin()
{
}

ioEtcItemMonsterCoin::~ioEtcItemMonsterCoin()
{

}

bool ioEtcItemMonsterCoin::_OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	// 모두 사용해도 삭제하지 않는다.
	// OnUse에서 체크하므로 pUser 와 pUserEtcItem NULL이 아님을 보장	
	if( rkSlot.GetUse() > 1 )
	{
		rkSlot.AddUse( -1 );
		pUserEtcItem->SetEtcItem( rkSlot );
	}
	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "Use Etc Item (%s/%d/%d)", pUser->GetPublicID().c_str(), rkSlot.m_iType, rkSlot.m_iValue1 );
	return true;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemGashapon::ioEtcItemGashapon()
{

}

ioEtcItemGashapon::~ioEtcItemGashapon()
{

}


void ioEtcItemGashapon::OnBuy( User *pUser, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	rkSlot.SetUse( false );
}

bool ioEtcItemGashapon::_OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	// OnUse에서 체크하므로 pUser 와 pUserEtcItem NULL이 아님을 보장
	if( pUser->IsReserveServerMoving() )
	{
		SP2Packet kReturn( STPK_ETCITEM_USE );
		PACKET_GUARD_bool( kReturn.Write(ETCITEM_USE_MOVING_SERVER) );
		pUser->SendMessage( kReturn );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Fail Reserve Server moving - %s) %d.",  __FUNCTION__, pUser->GetPublicID().c_str(), rkSlot.m_iType );
		return false;
	}
	
	short iPresentType   = 0;
	int   iPresentValue1 = 0;
	int   iPresentValue2 = 0;
	int   iPresentValue3 = 0;
	int   iPresentValue4 = 0;
	bool  bWholeAlarm    = false;
	int   iPresentPeso   = 0;
	if( g_PresentHelper.SendGashaponPresent( pUser, GetType(), iPresentType, iPresentValue1, iPresentValue2, iPresentValue3, iPresentValue4, bWholeAlarm, iPresentPeso ) )
	{
		rkSlot.AddUse( -1 );
		if( rkSlot.GetUse() <= 0 )
		{
			pUserEtcItem->DeleteEtcItem( rkSlot.m_iType, LogDBClient::ET_DEL );
		}
		else
		{
			pUserEtcItem->SetEtcItem( rkSlot );
		}

		SP2Packet kReturn( STPK_ETCITEM_USE );
		PACKET_GUARD_bool( kReturn.Write(ETCITEM_USE_OK) );
		PACKET_GUARD_bool( kReturn.Write(rkSlot.m_iType) );
		PACKET_GUARD_bool( kReturn.Write(rkSlot.m_iValue1) );
		PACKET_GUARD_bool( kReturn.Write(rkSlot.m_iValue2) );

		PACKET_GUARD_bool( kReturn.Write(iPresentType) );
		PACKET_GUARD_bool( kReturn.Write(iPresentValue1) );
		PACKET_GUARD_bool( kReturn.Write(iPresentValue2) );
		PACKET_GUARD_bool( kReturn.Write(iPresentValue3) );
		PACKET_GUARD_bool( kReturn.Write(iPresentValue4) );
		PACKET_GUARD_bool( kReturn.Write(bWholeAlarm) );
		PACKET_GUARD_bool( kReturn.Write(iPresentPeso) );

		pUser->SendMessage( kReturn );
		pUser->SendPresentMemory();  // 메모리 선물 전송

		// gashapon present에 관한 전서버 알림
		g_UserNodeManager.AlarmEventInGashaponPresentToAllUsers(pUser, bWholeAlarm, this->GetType(), iPresentType, iPresentValue1, iPresentValue2);
	}
	else
	{
		SP2Packet kReturn( STPK_ETCITEM_USE );
		PACKET_GUARD_bool( kReturn.Write(ETCITEM_USE_EXCEPTION) );
		pUser->SendMessage( kReturn );
		return false;
	}

	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "Use Etc Item (%s/%d/%d)", pUser->GetPublicID().c_str(), rkSlot.m_iType, rkSlot.m_iValue1 );

	return false; // false 면 OnUse() 여기서 중단
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemRandomDecoM::ioEtcItemRandomDecoM()
{
}

ioEtcItemRandomDecoM::~ioEtcItemRandomDecoM()
{
}

void ioEtcItemRandomDecoM::OnBuy( User *pUser, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	rkSlot.SetUse( false );
}

bool ioEtcItemRandomDecoM::_OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	// OnUse에서 체크하므로 pUser 와 pUserEtcItem NULL이 아님을 보장
	if( pUser->IsReserveServerMoving() )
	{
		SP2Packet kReturn( STPK_ETCITEM_USE );
		PACKET_GUARD_bool( kReturn.Write(ETCITEM_USE_MOVING_SERVER) );
		pUser->SendMessage( kReturn );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Fail Reserve Server moving - %s) %d.",  __FUNCTION__, pUser->GetPublicID().c_str(), rkSlot.m_iType );
		return false;
	}

	short iPresentType   = 0;
	int   iPresentValue1 = 0;
	int   iPresentValue2 = 0;
	int   iPresentValue3 = 0;
	int   iPresentValue4 = 0;
	bool  bWholeAlarm    = false;
	int   iPresentPeso   = 0;
	if( g_PresentHelper.SendRandomDecoPresent( pUser, iPresentType, iPresentValue1, iPresentValue2, iPresentValue3, iPresentValue4, bWholeAlarm, iPresentPeso, true ) )
	{

		rkSlot.AddUse( -1 );
		if( rkSlot.GetUse() <= 0 )
		{
			pUserEtcItem->DeleteEtcItem( rkSlot.m_iType, LogDBClient::ET_DEL );
		}
		else
		{
			pUserEtcItem->SetEtcItem( rkSlot );
		}
	
		SP2Packet kReturn( STPK_ETCITEM_USE );
		PACKET_GUARD_bool( kReturn.Write(ETCITEM_USE_OK) );
		PACKET_GUARD_bool( kReturn.Write(rkSlot.m_iType) );
		PACKET_GUARD_bool( kReturn.Write(rkSlot.m_iValue1) );
		PACKET_GUARD_bool( kReturn.Write(rkSlot.m_iValue2) );
		PACKET_GUARD_bool( kReturn.Write(iPresentType) );
		PACKET_GUARD_bool( kReturn.Write(iPresentValue1) );
		PACKET_GUARD_bool( kReturn.Write(iPresentValue2) );
		PACKET_GUARD_bool( kReturn.Write(iPresentValue3) );
		PACKET_GUARD_bool( kReturn.Write(iPresentValue4) );
		PACKET_GUARD_bool( kReturn.Write(bWholeAlarm) );
		PACKET_GUARD_bool( kReturn.Write(iPresentPeso) );

		pUser->SendMessage( kReturn );

		// random deco present에 관한 전서버 알림
		g_UserNodeManager.AlarmEventInRandomDecoPresentToAllUsers(pUser, bWholeAlarm, this->GetType(), iPresentType, iPresentValue1, iPresentValue2);
	}
	else
	{
		SP2Packet kReturn( STPK_ETCITEM_USE );
		PACKET_GUARD_bool( kReturn.Write(ETCITEM_USE_EXCEPTION) );
		pUser->SendMessage( kReturn );
		return false;
	}

	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "Use Etc Item (%s/%d/%d)", pUser->GetPublicID().c_str(), rkSlot.m_iType, rkSlot.m_iValue1 );

	return false; // false 면 OnUse() 여기서 중단
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemRandomDecoW::ioEtcItemRandomDecoW()
{
}

ioEtcItemRandomDecoW::~ioEtcItemRandomDecoW()
{
}

void ioEtcItemRandomDecoW::OnBuy( User *pUser, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	rkSlot.SetUse( false );
}

bool ioEtcItemRandomDecoW::_OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	// OnUse에서 체크하므로 pUser 와 pUserEtcItem NULL이 아님을 보장
	if( pUser->IsReserveServerMoving() )
	{
		SP2Packet kReturn( STPK_ETCITEM_USE );
		PACKET_GUARD_bool( kReturn.Write(ETCITEM_USE_MOVING_SERVER) );
		pUser->SendMessage( kReturn );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Fail Reserve Server moving - %s) %d.",  __FUNCTION__, pUser->GetPublicID().c_str(), rkSlot.m_iType );
		return false;
	}

	short iPresentType   = 0;
	int   iPresentValue1 = 0;
	int   iPresentValue2 = 0;
	int   iPresentValue3 = 0;
	int   iPresentValue4 = 0;
	bool  bWholeAlarm    = false;
	int   iPresentPeso   = 0;
	if( g_PresentHelper.SendRandomDecoPresent( pUser, iPresentType, iPresentValue1, iPresentValue2, iPresentValue3, iPresentValue4, bWholeAlarm, iPresentPeso, false ) )
	{
		rkSlot.AddUse( -1 );
		if( rkSlot.GetUse() <= 0 )
		{
			pUserEtcItem->DeleteEtcItem( rkSlot.m_iType, LogDBClient::ET_DEL );
		}
		else
		{
			pUserEtcItem->SetEtcItem( rkSlot );
		}

		SP2Packet kReturn( STPK_ETCITEM_USE );
		PACKET_GUARD_bool( kReturn.Write(ETCITEM_USE_OK) );
		PACKET_GUARD_bool( kReturn.Write(rkSlot.m_iType) );
		PACKET_GUARD_bool( kReturn.Write(rkSlot.m_iValue1) );
		PACKET_GUARD_bool( kReturn.Write(rkSlot.m_iValue2) );
		PACKET_GUARD_bool( kReturn.Write(iPresentType) );
		PACKET_GUARD_bool( kReturn.Write(iPresentValue1) );
		PACKET_GUARD_bool( kReturn.Write(iPresentValue2) );
		PACKET_GUARD_bool( kReturn.Write(iPresentValue3) );
		PACKET_GUARD_bool( kReturn.Write(iPresentValue4) );
		PACKET_GUARD_bool( kReturn.Write(bWholeAlarm) );
		PACKET_GUARD_bool( kReturn.Write(iPresentPeso) );

		pUser->SendMessage( kReturn );

		// random deco present에 관한 전서버 알림
		g_UserNodeManager.AlarmEventInRandomDecoPresentToAllUsers(pUser, bWholeAlarm, this->GetType(), iPresentType, iPresentValue1, iPresentValue2);
	}
	else
	{
		SP2Packet kReturn( STPK_ETCITEM_USE );
		PACKET_GUARD_bool( kReturn.Write(ETCITEM_USE_EXCEPTION) );
		pUser->SendMessage( kReturn );
		return false;
	}

	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "Use Etc Item (%s/%d/%d)", pUser->GetPublicID().c_str(), rkSlot.m_iType, rkSlot.m_iValue1 );

	return false; // false 면 OnUse() 여기서 중단
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemPackage::ioEtcItemPackage()
{

}

ioEtcItemPackage::~ioEtcItemPackage()
{

}

void ioEtcItemPackage::OnBuy( User *pUser, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	rkSlot.SetUse( false );
}

bool ioEtcItemPackage::_OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	// OnUse에서 체크하므로 pUser 와 pUserEtcItem NULL이 아님을 보장
	if( pUser->IsReserveServerMoving() )
	{
		SP2Packet kReturn( STPK_ETCITEM_USE );
		PACKET_GUARD_bool( kReturn.Write(ETCITEM_USE_MOVING_SERVER) );
		pUser->SendMessage( kReturn );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Fail Reserve Server moving - %s) %d.",  __FUNCTION__, pUser->GetPublicID().c_str(), rkSlot.m_iType );
		return false;
	}

	if( g_PresentHelper.SendEtcItemPackagePresent( pUser, GetType() ) )
	{
		// 권한 아이템 삭제
		pUserEtcItem->DeleteEtcItem( GetType(), LogDBClient::ET_DEL );
	}
	return true; 
}

/////////////////////////////////////////////////////////////////////////////
ioEtcItemDecoUnderwearPackage::ioEtcItemDecoUnderwearPackage()
{

}

ioEtcItemDecoUnderwearPackage::~ioEtcItemDecoUnderwearPackage()
{

}

void ioEtcItemDecoUnderwearPackage::OnBuy( User *pUser, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	rkSlot.SetUse( false );
}

bool ioEtcItemDecoUnderwearPackage::_OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	// OnUse에서 체크하므로 pUser 와 pUserEtcItem NULL이 아님을 보장
	if( pUser->IsReserveServerMoving() )
	{
		SP2Packet kReturn( STPK_ETCITEM_USE );
		PACKET_GUARD_bool( kReturn.Write(ETCITEM_USE_MOVING_SERVER) );
		pUser->SendMessage( kReturn );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Fail Reserve Server moving - %s) %d.",  __FUNCTION__, pUser->GetPublicID().c_str(), rkSlot.m_iType );
		return false;
	}

	pUser->OnEtcItemDecorationPackage( rkPacket, GetType() );
	return false; 
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemExcavatingKit::ioEtcItemExcavatingKit()
{
}

ioEtcItemExcavatingKit::~ioEtcItemExcavatingKit()
{

}

bool ioEtcItemExcavatingKit::IsUpdateTime( IN Room *pRoom , IN User *pUser )
{
	if( !pRoom ) 
		return false;

	if( pRoom->GetModeType() != MT_TRAINING )
		return false;

	if( !pUser )
		return false;

	ioUserEtcItem *pUserEtcItem = pUser->GetUserEtcItem();
	if( !pUserEtcItem )
		return false;

	ioUserEtcItem::ETCITEMSLOT kSlot;
	pUserEtcItem->GetEtcItem( GetType(), kSlot );	
	if( !kSlot.IsUse() )
		return false;

	return true;
}

bool ioEtcItemExcavatingKit::_OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	// OnUse에서 체크하므로 pUser 와 pUserEtcItem NULL이 아님을 보장
	if( pUser->IsReserveServerMoving() )
	{
		SP2Packet kReturn( STPK_ETCITEM_USE );
		PACKET_GUARD_bool( kReturn.Write(ETCITEM_USE_MOVING_SERVER) );
		pUser->SendMessage( kReturn );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Fail Reserve Server moving - %s) %d.",  __FUNCTION__, pUser->GetPublicID().c_str(), rkSlot.m_iType );
		return false;
	}

	bool bRequestUse = true;
	PACKET_GUARD_bool( rkPacket.Read(bRequestUse) );

	Room *pRoom = pUser->GetMyRoom();
	if( !pRoom || ( pRoom && pRoom->GetModeType() != MT_TRAINING ) )
	{
		SP2Packet kReturn( STPK_ETCITEM_USE );
		PACKET_GUARD_bool( kReturn.Write(ETCITEM_USE_EXCEPTION) );
		pUser->SendMessage( kReturn );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pRoom == NULL %s) %d.",  __FUNCTION__, pUser->GetPublicID().c_str(), rkSlot.m_iType );
		return false;
	}

	bool bUse = rkSlot.IsUse();
	if( bUse == bRequestUse ) // 이미 적용된 상태
	{
		SP2Packet kReturn( STPK_EXCAVATION_COMMAND );
		PACKET_GUARD_bool( kReturn.Write(EXCAVATION_USE_KIT) );
		PACKET_GUARD_bool( kReturn.Write(pUser->GetPublicID()) );
		PACKET_GUARD_bool( kReturn.Write(GetType()) );
		PACKET_GUARD_bool( kReturn.Write(bUse) );
		PACKET_GUARD_bool( kReturn.Write(false) ); // delete item

		pRoom->RoomSendPacketTcp( kReturn );

		return true;
	}

	bUse = !bUse;
	if( bUse )
	{
		pUser->SetEquipExcavating( true, GetType() );
		pUser->SetExcavatingTime( TIMEGETTIME() );

		// 사용중일때만 time check(IsUpdateTime)가 되므로 SetUse을 먼저 한다.
		rkSlot.SetUse( bUse );
		pUserEtcItem->SetEtcItem( rkSlot );
		
		pUser->StartEtcItemTime( __FUNCTION__ , rkSlot.m_iType );
	}
	else
	{
		g_LogDBClient.OnInsertTime( pUser, LogDBClient::TT_EXCAVATING ); // excating time 초기화전에 함.

		pUser->SetEquipExcavating( false, GetType() );
		pUser->SetExcavatingTime( 0 );

		// 갱신된 경우 유저에게 현재 값을 보내게 되므로 SetUse을 먼저한다.
		rkSlot.SetUse( bUse );
		pUserEtcItem->SetEtcItem( rkSlot );

		pUser->UpdateEtcItemTime( __FUNCTION__, rkSlot.m_iType );

		if( !pUserEtcItem->GetEtcItem( GetType(), rkSlot ) )  // UpdateEtcItemTime()에서 아이템이 삭제되었다면 삭제 처리 메세지가 전송되니 여기서 중단
			return false;
	}

	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "Use Etc Item (%s/%d/%d/%d)", pUser->GetPublicID().c_str(), rkSlot.m_iType, rkSlot.m_iValue1, rkSlot.m_iValue2 );

	SP2Packet kReturn( STPK_EXCAVATION_COMMAND );
	PACKET_GUARD_bool( kReturn.Write(EXCAVATION_USE_KIT) );
	PACKET_GUARD_bool( kReturn.Write(pUser->GetPublicID()) );
	PACKET_GUARD_bool( kReturn.Write(GetType()) );
	PACKET_GUARD_bool( kReturn.Write(rkSlot.IsUse()) );
	PACKET_GUARD_bool( kReturn.Write(false) ); // delete item

	pRoom->RoomSendPacketTcp( kReturn );

	return true;
}

void ioEtcItemExcavatingKit::DeleteWork( User *pUser, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	if( pUser == NULL )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Fail - pUser == NULL.(%d)", __FUNCTION__, GetType() );
		return;
	}

	Room *pRoom = pUser->GetMyRoom();
	if( !pRoom ) 
		return;

	pUser->SetEquipExcavating( false, GetType() );
	pUser->SetExcavatingTime( 0 );

	SP2Packet kReturn( STPK_EXCAVATION_COMMAND );
	PACKET_GUARD_VOID( kReturn.Write(EXCAVATION_USE_KIT) );
	PACKET_GUARD_VOID( kReturn.Write(pUser->GetPublicID()) );
	PACKET_GUARD_VOID( kReturn.Write(GetType()) );
	PACKET_GUARD_VOID( kReturn.Write(false) ); // bUse
	PACKET_GUARD_VOID( kReturn.Write(true) ); // delete item

	pRoom->RoomSendPacketTcp( kReturn );
}

bool ioEtcItemExcavatingKit::LeaveRoomTimeItem( ioUserEtcItem::ETCITEMSLOT &rkSlot, User *pUser )
{
	if( !pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Fail - pUser == NULL.(%d)", __FUNCTION__, GetType() );
		return false;
	}

	if( !rkSlot.IsUse() )
		return false;

	ioUserEtcItem *pUserEtcItem = pUser->GetUserEtcItem();
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Fail - pUserEtcItem == NULL.(%d)", __FUNCTION__, GetType() );
		return false;
	}

	g_LogDBClient.OnInsertTime( pUser, LogDBClient::TT_EXCAVATING ); // excating time 초기화전에 함.
	
	pUser->SetEquipExcavating( false, GetType() );
	pUser->SetExcavatingTime( 0 );

	rkSlot.SetUse( false );
	pUserEtcItem->SetEtcItem( rkSlot );

	return true;
}

void ioEtcItemExcavatingKit::OnBuy( User *pUser, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	// 구매시에 액션이 없다.
}

bool ioEtcItemExcavatingKit::IsBuyCondition( int iUse )
{
	if( iUse >= m_iMaxUse )
		return false;

	return true;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemTradeStateChange::ioEtcItemTradeStateChange()
{
}

ioEtcItemTradeStateChange::~ioEtcItemTradeStateChange()
{
}

bool ioEtcItemTradeStateChange::_OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	// OnUse에서 체크하므로 pUser 와 pUserEtcItem NULL이 아님을 보장
	if( pUser->IsReserveServerMoving() )
	{
		SP2Packet kReturn( STPK_ETCITEM_USE );
		PACKET_GUARD_bool( kReturn.Write(ETCITEM_USE_MOVING_SERVER) );
		pUser->SendMessage( kReturn );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Fail Reserve Server moving - %s) %d.",  __FUNCTION__, pUser->GetPublicID().c_str(), rkSlot.m_iType );
		return false;
	}

	int iSlotIndex = 0;
	PACKET_GUARD_bool( rkPacket.Read(iSlotIndex) );

	ioUserExtraItem *pExtraItem = pUser->GetUserExtraItem();
	if( !pExtraItem )
	{
		SP2Packet kPacket( STPK_TRADE_STATE_CHANGE );
		PACKET_GUARD_bool( kPacket.Write(TRADE_STATE_CHANGE_ERROR) );
		pUser->SendMessage( kPacket );
		return false;
	}

	ioUserExtraItem::EXTRAITEMSLOT rkExtraItem;
	if( !pExtraItem->GetExtraItem( iSlotIndex, rkExtraItem ) )
	{
		// Not Find Error
		SP2Packet kPacket( STPK_TRADE_STATE_CHANGE );
		PACKET_GUARD_bool( kPacket.Write(TRADE_STATE_CHANGE_NO_ITEM) );
		pUser->SendMessage( kPacket );
		return false;
	}

	if( rkExtraItem.m_iTradeState != ioUserExtraItem::EET_NORMAL )
	{
		SP2Packet kPacket( STPK_TRADE_STATE_CHANGE );
		PACKET_GUARD_bool( kPacket.Write(TRADE_STATE_CHANGE_FAIL) );
		pUser->SendMessage( kPacket );
		return false;
	}

	int iCharArray = pUser->FindExtraItemEquipChar( iSlotIndex );
	if( COMPARE( iCharArray, 0, pUser->GetCharCount() ) )
	{
		SP2Packet kPacket( STPK_TRADE_STATE_CHANGE );
		PACKET_GUARD_bool( kPacket.Write(TRADE_STATE_CHANGE_EQUIP_ITEM) );
		pUser->SendMessage( kPacket );
		return false;
	}

	rkSlot.AddUse( -1 );
	if( rkSlot.GetUse() <= 0 )
	{
		pUserEtcItem->DeleteEtcItem( rkSlot.m_iType, LogDBClient::ET_DEL );
	}
	else
	{
		pUserEtcItem->SetEtcItem( rkSlot );
	}
	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "Use Etc Item (%s/%d/%d)", pUser->GetPublicID().c_str(), rkSlot.m_iType, rkSlot.m_iValue1 );


	rkExtraItem.m_iTradeState = ioUserExtraItem::EET_ENABLE;
	pExtraItem->SetExtraItem( rkExtraItem );

	SP2Packet kReturn( STPK_ETCITEM_USE );
	PACKET_GUARD_bool( kReturn.Write(ETCITEM_USE_OK) );
	PACKET_GUARD_bool( kReturn.Write(rkSlot.m_iType) );
	PACKET_GUARD_bool( kReturn.Write(rkSlot.m_iValue1) );
	PACKET_GUARD_bool( kReturn.Write(rkSlot.m_iValue2) );
	pUser->SendMessage( kReturn );

	SP2Packet kPacket( STPK_TRADE_STATE_CHANGE );
	PACKET_GUARD_bool( kPacket.Write(TRADE_STATE_CHANGE_OK) );
	PACKET_GUARD_bool( kPacket.Write(iSlotIndex) );
	pUser->SendMessage( kPacket );

	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemQuestEvent::ioEtcItemQuestEvent()
{

}

ioEtcItemQuestEvent::~ioEtcItemQuestEvent()
{

}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemSilverCoin::ioEtcItemSilverCoin()
{
}

ioEtcItemSilverCoin::~ioEtcItemSilverCoin()
{

}

bool ioEtcItemSilverCoin::_OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "호출되서는 안되는 함수 호출됨 !! Use Etc Item (%s/%d/%d)", pUser->GetPublicID().c_str(), rkSlot.m_iType, rkSlot.m_iValue1 );
	return true;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemMileage::ioEtcItemMileage()
{
}

ioEtcItemMileage::~ioEtcItemMileage()
{

}

bool ioEtcItemMileage::_OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "호출되서는 안되는 함수 호출됨 !! Use Etc Item (%s/%d/%d)", pUser->GetPublicID().c_str(), rkSlot.m_iType, rkSlot.m_iValue1 );
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemBattleRecordInit::ioEtcItemBattleRecordInit()
{

}

ioEtcItemBattleRecordInit::~ioEtcItemBattleRecordInit()
{

}


void ioEtcItemBattleRecordInit::OnBuy( User *pUser, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	rkSlot.SetUse( false );
}

bool ioEtcItemBattleRecordInit::_OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	// OnUse에서 체크하므로 pUser 와 pUserEtcItem NULL이 아님을 보장
	if( pUser->IsReserveServerMoving() )
	{
		SP2Packet kReturn( STPK_ETCITEM_USE );
		PACKET_GUARD_bool( kReturn.Write(ETCITEM_USE_MOVING_SERVER) );
		pUser->SendMessage( kReturn );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Fail Reserve Server moving - %s) %d.",  __FUNCTION__, pUser->GetPublicID().c_str(), rkSlot.m_iType );
		return false;
	}

	// 전투 전적 초기화 - 승/패/킬/데스
	if( !pUser->InitEtcItemUseBattleRecord() )
	{
		SP2Packet kReturn( STPK_ETCITEM_USE );
		PACKET_GUARD_bool( kReturn.Write(ETCITEM_USE_EXCEPTION) );
		pUser->SendMessage( kReturn );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Fail ioEtcItemBattleRecordInit - %s) %d.",  __FUNCTION__, pUser->GetPublicID().c_str(), rkSlot.m_iType );
		return false;
	}

	rkSlot.AddUse( -1 );
	if( rkSlot.GetUse() <= 0 )
	{
		pUserEtcItem->DeleteEtcItem( rkSlot.m_iType, LogDBClient::ET_DEL );
	}
	else
	{
		pUserEtcItem->SetEtcItem( rkSlot );
	}
	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "Use Etc Item (%s/%d/%d)", pUser->GetPublicID().c_str(), rkSlot.m_iType, rkSlot.m_iValue1 );	
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemLadderRecordInit::ioEtcItemLadderRecordInit()
{

}

ioEtcItemLadderRecordInit::~ioEtcItemLadderRecordInit()
{

}


void ioEtcItemLadderRecordInit::OnBuy( User *pUser, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	rkSlot.SetUse( false );
}

bool ioEtcItemLadderRecordInit::_OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	// OnUse에서 체크하므로 pUser 와 pUserEtcItem NULL이 아님을 보장
	if( pUser->IsReserveServerMoving() )
	{
		SP2Packet kReturn( STPK_ETCITEM_USE );
		PACKET_GUARD_bool( kReturn.Write(ETCITEM_USE_MOVING_SERVER) );
		pUser->SendMessage( kReturn );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Fail Reserve Server moving - %s) %d.",  __FUNCTION__, pUser->GetPublicID().c_str(), rkSlot.m_iType );
		return false;
	}

	// 진영 전적 초기화 - 승/패/킬/데스
	if( !pUser->InitEtcItemUseLadderRecord() )
	{
		SP2Packet kReturn( STPK_ETCITEM_USE );
		PACKET_GUARD_bool( kReturn.Write(ETCITEM_USE_EXCEPTION) );
		pUser->SendMessage( kReturn );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Fail InitEtcItemUseLadderRecord - %s) %d.",  __FUNCTION__, pUser->GetPublicID().c_str(), rkSlot.m_iType );
		return false;
	}

	rkSlot.AddUse( -1 );
	if( rkSlot.GetUse() <= 0 )
	{
		pUserEtcItem->DeleteEtcItem( rkSlot.m_iType, LogDBClient::ET_DEL );
	}
	else
	{
		pUserEtcItem->SetEtcItem( rkSlot );
	}
	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "Use Etc Item (%s/%d/%d)", pUser->GetPublicID().c_str(), rkSlot.m_iType, rkSlot.m_iValue1 );	
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemHeroRecordInit::ioEtcItemHeroRecordInit()
{

}

ioEtcItemHeroRecordInit::~ioEtcItemHeroRecordInit()
{

}


void ioEtcItemHeroRecordInit::OnBuy( User *pUser, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	rkSlot.SetUse( false );
}

bool ioEtcItemHeroRecordInit::_OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	// OnUse에서 체크하므로 pUser 와 pUserEtcItem NULL이 아님을 보장
	if( pUser->IsReserveServerMoving() )
	{
		SP2Packet kReturn( STPK_ETCITEM_USE );
		PACKET_GUARD_bool( kReturn.Write(ETCITEM_USE_MOVING_SERVER) );
		pUser->SendMessage( kReturn );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Fail Reserve Server moving - %s) %d.",  __FUNCTION__, pUser->GetPublicID().c_str(), rkSlot.m_iType );
		return false;
	}

	// 래더 전적 초기화 - 승/패/킬/데스/래더포인트 - 누적 : 승/패/킬/데스/래더포인트
	if( !pUser->InitEtcItemUseHeroRecord() )
	{
		SP2Packet kReturn( STPK_ETCITEM_USE );
		PACKET_GUARD_bool( kReturn.Write(ETCITEM_USE_EXCEPTION) );
		pUser->SendMessage( kReturn );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Fail InitEtcItemUseHeroRecord() - %s) %d.",  __FUNCTION__, pUser->GetPublicID().c_str(), rkSlot.m_iType );
		return false;
	}

	rkSlot.AddUse( -1 );
	if( rkSlot.GetUse() <= 0 )
	{
		pUserEtcItem->DeleteEtcItem( rkSlot.m_iType, LogDBClient::ET_DEL );
	}
	else
	{
		pUserEtcItem->SetEtcItem( rkSlot );
	}
	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "Use Etc Item (%s/%d/%d)", pUser->GetPublicID().c_str(), rkSlot.m_iType, rkSlot.m_iValue1 );	
	return true;
}
//////////////////////////////////////////////////////////////////////////
ioEtcItemSkeleton::ioEtcItemSkeleton()
{

}

ioEtcItemSkeleton::~ioEtcItemSkeleton()
{

}

void ioEtcItemSkeleton::OnBuy( User *pUser, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
}

bool ioEtcItemSkeleton::OnUseSwitch( SP2Packet &rkPacket, User *pUser )
{
	if( !pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Fail - pUser == NULL.(%d)", __FUNCTION__, GetType() );
		return false;
	}

	ioUserEtcItem *pUserEtcItem = pUser->GetUserEtcItem();
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Fail - pUserEtcItem == NULL.(%d)", __FUNCTION__, GetType() );
		return false;
	}

	ioUserEtcItem::ETCITEMSLOT kSlot;
	pUserEtcItem->GetEtcItem( GetType(), kSlot );
	if( kSlot.m_iType <= 0 || kSlot.m_iValue1 <= 0 )
	{
		SP2Packet kReturn( STPK_ETCITEM_SWITCH );
		PACKET_GUARD_bool( kReturn.Write(ETCITEM_SWITCH_DONT_HAVE) );
		pUser->SendMessage( kReturn );
		LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "%s Fail - %s) %d don't have value.", __FUNCTION__, pUser->GetPublicID().c_str(), GetType() );
		return false;
	}

	bool bUseOn = false;
	PACKET_GUARD_bool( rkPacket.Read(bUseOn) );
	kSlot.SetUse( bUseOn );
	pUserEtcItem->SetEtcItem( kSlot );

	SP2Packet kPacket( STPK_ETCITEM_SWITCH );
	PACKET_GUARD_bool( kPacket.Write(ETCITEM_SWITCH_OK) );
	PACKET_GUARD_bool( kPacket.Write(GetType()) );
	PACKET_GUARD_bool( kPacket.Write(kSlot.IsUse()) );
	PACKET_GUARD_bool( kPacket.Write(pUser->GetPublicID()) );
	pUser->SendMessage( kPacket );

	Room *pRoom = pUser->GetMyRoom();
	if( pRoom && pRoom->GetRoomStyle() == RSTYLE_PLAZA )
	{
		if( kSlot.IsUse() )
			pUser->StartEtcItemTime( __FUNCTION__, GetType() );
		else
			pUser->UpdateEtcItemTime( __FUNCTION__, GetType() );	

		// 유저들에게 알려야한다.
		pRoom->RoomSendPacketTcp( kPacket, pUser );
	}
	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "Switch Etc Item (%s/%d/%d/%d)", pUser->GetPublicID().c_str(), kSlot.m_iType, kSlot.m_iValue1, kSlot.m_iValue2 );	

	// 링크된 다른 아이템은 해제한다.
	if( bUseOn )
	{
		static IntVec vLinkItem;
		vLinkItem.clear();

		vLinkItem.push_back( EIT_ETC_SKELETON_BIG );
		vLinkItem.push_back( EIT_ETC_SKELETON_BIGHEAD );
		vLinkItem.push_back( EIT_ETC_SKELETON_SMALL );
		for(int i = 0;i < (int)vLinkItem.size();i++)
		{
			if( GetType() == vLinkItem[i] ) continue;
			if( !pUserEtcItem->GetEtcItem( vLinkItem[i], kSlot ) ) continue;
			if( kSlot.m_iType <= 0 || kSlot.m_iValue1 <= 0 ) continue;
			if( !kSlot.IsUse() ) continue;

			kSlot.SetUse( false );
			pUserEtcItem->SetEtcItem( kSlot );
			if( pRoom && pRoom->GetRoomStyle() == RSTYLE_PLAZA )
				pUser->UpdateEtcItemTime( __FUNCTION__, kSlot.m_iType );
		}
	}
	return true;
}

bool ioEtcItemSkeleton::IsUpdateTime( IN Room *pRoom , IN User *pUser )
{
	if( !pRoom ) 
		return false;

	if( pRoom->GetModeType() != MT_TRAINING )
		return false;

	if( !pUser )
		return false;

	ioUserEtcItem *pUserEtcItem = pUser->GetUserEtcItem();
	if( !pUserEtcItem )
		return false;

	ioUserEtcItem::ETCITEMSLOT kSlot;
	pUserEtcItem->GetEtcItem( GetType(), kSlot );	
	if( !kSlot.IsUse() )
		return false;

	return true;
}

void ioEtcItemSkeleton::DeleteWork( User *pUser, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	if( pUser == NULL )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Fail - pUser == NULL.(%d)", __FUNCTION__, GetType() );
		return;
	}

	Room *pRoom = pUser->GetMyRoom();
	if( !pRoom ) return;

	// 아이템 삭제 알림
	SP2Packet kReturn( STPK_ETCITEM_SKELETON_DELETE );
	PACKET_GUARD_VOID( kReturn.Write(rkSlot.m_iType) );
	PACKET_GUARD_VOID( kReturn.Write(pUser->GetPublicID()) );
	pRoom->RoomSendPacketTcp( kReturn );
}

bool ioEtcItemSkeleton::LeaveRoomTimeItem( ioUserEtcItem::ETCITEMSLOT &rkSlot, User *pUser )
{
	if( !pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Fail - pUser == NULL.(%d)", __FUNCTION__, GetType() );
		return false;
	}

	if( !rkSlot.IsUse() )
		return false;

	ioUserEtcItem *pUserEtcItem = pUser->GetUserEtcItem();
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Fail - pUserEtcItem == NULL.(%d)", __FUNCTION__, GetType() );
		return false;
	}

	return true;
}

bool ioEtcItemSkeleton::IsBuyCondition( int iUse )
{
	if( iUse >= m_iMaxUse )
		return false;

	return true;
}
//////////////////////////////////////////////////////////////////////////
ioEtcItemMotion::ioEtcItemMotion()
{

}

ioEtcItemMotion::~ioEtcItemMotion()
{

}

void ioEtcItemMotion::OnBuy( User *pUser, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
}

bool ioEtcItemMotion::IsBuyCondition( int iUse )
{
	if( iUse >= m_iMaxUse )
		return false;

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemCustomItemSkin::ioEtcItemCustomItemSkin()
{

}

ioEtcItemCustomItemSkin::~ioEtcItemCustomItemSkin()
{

}

bool ioEtcItemCustomItemSkin::_OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	// OnUse에서 체크하므로 pUser 와 pUserEtcItem NULL이 아님을 보장
	if( pUser->IsReserveServerMoving() )
	{
		SP2Packet kReturn( STPK_ETCITEM_USE );
		PACKET_GUARD_bool( kReturn.Write(ETCITEM_USE_MOVING_SERVER) );
		pUser->SendMessage( kReturn );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Fail Reserve Server moving - %s) %d.",  __FUNCTION__, pUser->GetPublicID().c_str(), rkSlot.m_iType );
		return false;
	}

    // 장비 텍스쳐 변경 프로세스
	int iSlotIndex = 0;
	DWORD dwMaleCustom = 0;
	DWORD dwFemaleCustom = 0;

	PACKET_GUARD_bool( rkPacket.Read(iSlotIndex) );
	PACKET_GUARD_bool( rkPacket.Read(dwMaleCustom) );
	PACKET_GUARD_bool( rkPacket.Read(dwFemaleCustom) );

	ioUserExtraItem *pUserExtraItem = pUser->GetUserExtraItem();
	if( !pUserExtraItem )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Fail NULL Pointer - %s) %d.",  __FUNCTION__, pUser->GetPublicID().c_str(), rkSlot.m_iType );
		return false;
	}

	ioUserExtraItem::EXTRAITEMSLOT kExtraItem;
	if( !pUserExtraItem->GetExtraItem( iSlotIndex, kExtraItem ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s None Item - %s) %d.",  __FUNCTION__, pUser->GetPublicID().c_str(), rkSlot.m_iType );
		return false;
	}

	kExtraItem.m_dwMaleCustom	 = dwMaleCustom;
	kExtraItem.m_dwFemaleCustom  = dwFemaleCustom;
	pUserExtraItem->SetExtraItem( kExtraItem );
	pUser->SaveExtraItem();
	pUser->AllCharReEquipDBItem( kExtraItem.m_iIndex );

	// 권한 아이템
	rkSlot.AddUse( -1 );
	if( rkSlot.GetUse() <= 0 )
	{
		pUserEtcItem->DeleteEtcItem( rkSlot.m_iType, LogDBClient::ET_DEL );
	}
	else
	{
		pUserEtcItem->SetEtcItem( rkSlot );
	}
	pUser->SaveEtcItem();
	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "%s Success - %s) %d : %d(%d:%d).",  __FUNCTION__, pUser->GetPublicID().c_str(), rkSlot.m_iType, kExtraItem.m_iIndex, kExtraItem.m_dwMaleCustom, kExtraItem.m_dwFemaleCustom  );
	
	char szItemIndex[MAX_PATH]="";
	if( iSlotIndex != 0 )
	{
		StringCbPrintf( szItemIndex, sizeof( szItemIndex ), "%u-%u", iSlotIndex, 0 ); // db field는 1부터 이므로 +1
		g_LogDBClient.OnInsertExtraItem( pUser, kExtraItem.m_iItemCode, kExtraItem.m_iReinforce, 0, 0, 0, kExtraItem.m_PeriodType, kExtraItem.m_dwMaleCustom, kExtraItem.m_dwFemaleCustom, szItemIndex, LogDBClient::ERT_CUSTOM_ADD );
	}
	return true; 
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemCustomItemSkinTest::ioEtcItemCustomItemSkinTest()
{

}

ioEtcItemCustomItemSkinTest::~ioEtcItemCustomItemSkinTest()
{

}

bool ioEtcItemCustomItemSkinTest::_OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	// 클라이언트에서만 하므로 할게없음
	return true; 
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemCostumItemSkin::ioEtcItemCostumItemSkin()
{

}

ioEtcItemCostumItemSkin::~ioEtcItemCostumItemSkin()
{

}

bool ioEtcItemCostumItemSkin::_OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	// OnUse에서 체크하므로 pUser 와 pUserEtcItem NULL이 아님을 보장
	if( pUser->IsReserveServerMoving() )
	{
		SP2Packet kReturn( STPK_ETCITEM_USE );
		PACKET_GUARD_bool( kReturn.Write(ETCITEM_USE_MOVING_SERVER) );
		pUser->SendMessage( kReturn );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Fail Reserve Server moving - %s) %d.",  __FUNCTION__, pUser->GetPublicID().c_str(), rkSlot.m_iType );
		return false;
	}

	// 장비 텍스쳐 변경 프로세스
	int		iSlotIndex = 0;
	DWORD	dwMaleCustom = 0;
	DWORD	dwFemaleCustom = 0;

	PACKET_GUARD_bool( rkPacket.Read(iSlotIndex) );
	PACKET_GUARD_bool( rkPacket.Read(dwMaleCustom) );
	PACKET_GUARD_bool( rkPacket.Read(dwFemaleCustom) );

	ioUserCostume *pUserCostumeItem = pUser->GetUserCostume();
	if( !pUserCostumeItem )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Fail NULL Pointer - %s) %d.",  __FUNCTION__, pUser->GetPublicID().c_str(), rkSlot.m_iType );
		return false;
	}

	Costume *pCostume = pUserCostumeItem->GetCostume( iSlotIndex );
	pCostume->SetMaleCustom( dwMaleCustom );
	pCostume->SetFemaleCustom( dwFemaleCustom );
	pUserCostumeItem->ReleaseCostume( pCostume );

	// 권한 아이템
	rkSlot.AddUse( -1 );
	if( rkSlot.GetUse() <= 0 )
	{
		pUserEtcItem->DeleteEtcItem( rkSlot.m_iType, LogDBClient::ET_DEL );
	}
	else
	{
		pUserEtcItem->SetEtcItem( rkSlot );
	}
	pUser->SaveEtcItem();  //코스튬 아이템을 적용하자!!!
	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "%s Success - %s) %d : %d(%d:%d).",  __FUNCTION__, pUser->GetPublicID().c_str(), rkSlot.m_iType, pCostume->GetCostumeIndex() , pCostume->GetMaleCustom(), pCostume->GetFemaleCustom()  );

	char szItemIndex[MAX_PATH]="";
	if( iSlotIndex != 0 )
	{
		StringCbPrintf( szItemIndex, sizeof( szItemIndex ), "%u-%u", iSlotIndex, 0 ); // db field는 1부터 이므로 +1
		//g_LogDBClient.OnInsertCostumeInfo( pUser, )
		//g_LogDBClient.OnInsertExtraItem( pUser, kExtraItem.m_iItemCode, kExtraItem.m_iReinforce, 0, 0, 0, kExtraItem.m_PeriodType, kExtraItem.m_dwMaleCustom, kExtraItem.m_dwFemaleCustom, szItemIndex, LogDBClient::ERT_CUSTOM_ADD );
	}
	return true; 
}
//////////////////////////////////////////////////////////////////////////
ioEtcItemBlock::ioEtcItemBlock()
{

}

ioEtcItemBlock::~ioEtcItemBlock()
{

}

void ioEtcItemBlock::OnBuy( User *pUser, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemExtraItemGrowthCatalyst::ioEtcItemExtraItemGrowthCatalyst()
{
}

ioEtcItemExtraItemGrowthCatalyst::~ioEtcItemExtraItemGrowthCatalyst()
{
}

bool ioEtcItemExtraItemGrowthCatalyst::_OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	// OnUse에서 체크하므로 pUser 와 pUserEtcItem NULL이 아님을 보장
	// UserNode에서 장비 성장 처리
	if( !pUser->OnItemGrowthCatalyst( rkPacket, GetType() ) )
	{
		return false;
	}

	//
	rkSlot.AddUse( -1 );
	if( rkSlot.GetUse() <= 0 )
	{
		pUserEtcItem->DeleteEtcItem( rkSlot.m_iType, LogDBClient::ET_DEL );
	}
	else
	{
		pUserEtcItem->SetEtcItem( rkSlot );
	}
	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "Use Etc Item (%s/%d/%d)", pUser->GetPublicID().c_str(), rkSlot.m_iType, rkSlot.m_iValue1 );
	return true;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemLuckyCoin::ioEtcItemLuckyCoin()
{
}

ioEtcItemLuckyCoin::~ioEtcItemLuckyCoin()
{

}

void ioEtcItemLuckyCoin::LoadProperty( ioINILoader &rkLoader )
{
	ioEtcItem::LoadProperty( rkLoader );
	
	// 초기화.
	m_ExchangePresent.clear();

	char szKey[MAX_PATH], szBuf[MAX_PATH];
	int iMaxExchangePresent = rkLoader.LoadInt( "max_present", 0 );
	for(int i = 0;i < iMaxExchangePresent;i++)
	{
		ExchangePresent kPresent;

		sprintf_s( szKey, "present%d_index", i + 1 );
		kPresent.m_iIndex = rkLoader.LoadInt( szKey, 0 );

		sprintf_s( szKey, "present%d_need_coin", i + 1 );
		kPresent.m_iNeedLuckyCoin = rkLoader.LoadInt( szKey, 0 );

		sprintf_s( szKey, "present%d_type", i + 1 );
		kPresent.m_iPresentType = rkLoader.LoadInt( szKey, 0 );

		sprintf_s( szKey, "present%d_state", i + 1 );
		kPresent.m_iPresentState = rkLoader.LoadInt( szKey, 0 );

		sprintf_s( szKey, "present%d_ment", i + 1 );
		kPresent.m_iPresentMent = rkLoader.LoadInt( szKey, 0 );

		sprintf_s( szKey, "present%d_period", i + 1 );
		kPresent.m_iPresentPeriod = rkLoader.LoadInt( szKey, 0 );

		sprintf_s( szKey, "present%d_value1", i + 1 );
		kPresent.m_iPresentValue1 = rkLoader.LoadInt( szKey, 0 );

		sprintf_s( szKey, "present%d_value2", i + 1 );
		kPresent.m_iPresentValue2 = rkLoader.LoadInt( szKey, 0 );

		m_ExchangePresent.push_back( kPresent );
	}
}

ioEtcItemLuckyCoin::ExchangePresent ioEtcItemLuckyCoin::GetExchangePresent( int iIndex )
{
	int iPresentSize = m_ExchangePresent.size();
	for(int i = 0;i < iPresentSize;i++)
	{
		ExchangePresent &rkPresent = m_ExchangePresent[i];
		if( iIndex == rkPresent.m_iIndex )
			return rkPresent;
	}

	ExchangePresent kNonePresent;
	return kNonePresent;
}

bool ioEtcItemLuckyCoin::_OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	int iSelectIndex = 0;
	PACKET_GUARD_bool( rkPacket.Read(iSelectIndex) );

	ExchangePresent kPresent = GetExchangePresent( iSelectIndex );
	if( kPresent.m_iIndex == -1 )
    {
		// 없는 상품
		SP2Packet kPacket( STPK_ETCITEM_LUCKY_COIN_RESULT );
		PACKET_GUARD_bool( kPacket.Write(ETCITEM_LUCKY_COIN_RESULT_NONE_ITEM) );
		pUser->SendMessage( kPacket );
		return false;
    }

	if( kPresent.m_iNeedLuckyCoin <= 0 )
	{
		// 없는 상품
		SP2Packet kPacket( STPK_ETCITEM_LUCKY_COIN_RESULT );
		PACKET_GUARD_bool( kPacket.Write(ETCITEM_LUCKY_COIN_RESULT_NONE_ITEM) );
		pUser->SendMessage( kPacket );
		return false;
	}

	if( rkSlot.GetUse() < kPresent.m_iNeedLuckyCoin )
	{
		// 코인 부족
		SP2Packet kPacket( STPK_ETCITEM_LUCKY_COIN_RESULT );
		PACKET_GUARD_bool( kPacket.Write(ETCITEM_LUCKY_COIN_RESULT_NONE_COIN) );
		pUser->SendMessage( kPacket );
		return false;
	}

	// 특별 감소 먼저 전송
	rkSlot.AddUse( -kPresent.m_iNeedLuckyCoin );
	if( rkSlot.GetUse() <= 0 )
	{
		pUserEtcItem->DeleteEtcItem( rkSlot.m_iType, LogDBClient::ET_DEL );
	}
	else
	{
		pUserEtcItem->SetEtcItem( rkSlot );
	}
	
	SP2Packet kReturn( STPK_ETCITEM_USE );
	PACKET_GUARD_bool( kReturn.Write(ETCITEM_USE_OK) );
	PACKET_GUARD_bool( kReturn.Write(rkSlot.m_iType) );
	PACKET_GUARD_bool( kReturn.Write(rkSlot.m_iValue1) );
	PACKET_GUARD_bool( kReturn.Write(rkSlot.m_iValue2) );
	pUser->SendMessage( kReturn );
	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "Use Etc Item (%s/%d/%d)", pUser->GetPublicID().c_str(), rkSlot.m_iType, rkSlot.m_iValue1 );

	// 특별 사용 결과 전송
	CTimeSpan cPresentGapTime( kPresent.m_iPresentPeriod, 0, 0, 0 );
	CTime kPresentTime = CTime::GetCurrentTime() + cPresentGapTime;
	pUser->AddPresentMemory( g_MainServer.GetSendID(), kPresent.m_iPresentType, kPresent.m_iPresentValue1, kPresent.m_iPresentValue2, 
		kPresent.m_iPresentValue3, kPresent.m_iPresentValue4, kPresent.m_iPresentMent, kPresentTime, kPresent.m_iPresentState );
	g_LogDBClient.OnInsertPresent( 0, g_MainServer.GetSendID(), g_App.GetPublicIP().c_str(), pUser->GetUserIndex(), kPresent.m_iPresentType, kPresent.m_iPresentValue1, 
		kPresent.m_iPresentValue2, kPresent.m_iPresentValue3, kPresent.m_iPresentValue4, LogDBClient::PST_RECIEVE, "LuckyCoin" );
	pUser->SendPresentMemory();

	SP2Packet kPacket( STPK_ETCITEM_LUCKY_COIN_RESULT );
	PACKET_GUARD_bool( kPacket.Write(ETCITEM_LUCKY_COIN_RESULT_OK) );
	PACKET_GUARD_bool( kPacket.Write(GetType()) );
	PACKET_GUARD_bool( kPacket.Write(iSelectIndex) );
	pUser->SendMessage( kPacket );	
	return false;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemCompoundEx::ioEtcItemCompoundEx()
{
	m_iCompoundSuccessRand = 0;
}

ioEtcItemCompoundEx::~ioEtcItemCompoundEx()
{
}

void ioEtcItemCompoundEx::LoadProperty( ioINILoader &rkLoader )
{
	ioEtcItem::LoadProperty( rkLoader );

	m_iCompoundSuccessRand = rkLoader.LoadInt( "success_rand", 0 );
}

bool ioEtcItemCompoundEx::_OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	// OnUse에서 체크하므로 pUser 와 pUserEtcItem NULL이 아님을 보장
	// UserNode에서 장비 레벨업
	if( !pUser->OnItemCompoundEx( rkPacket, GetType(), m_iCompoundSuccessRand ) )
	{
		return false;
	}

	//
	rkSlot.AddUse( -1 );
	if( rkSlot.GetUse() <= 0 )
	{
		pUserEtcItem->DeleteEtcItem( rkSlot.m_iType, LogDBClient::ET_DEL );
	}
	else
	{
		pUserEtcItem->SetEtcItem( rkSlot );
	}
	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "Use Etc Item (%s/%d/%d)", pUser->GetPublicID().c_str(), rkSlot.m_iType, rkSlot.m_iValue1 );
	return true;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemRainbowMixer::ioEtcItemRainbowMixer()
{
}

ioEtcItemRainbowMixer::~ioEtcItemRainbowMixer()
{

}

void ioEtcItemRainbowMixer::LoadProperty( ioINILoader &rkLoader )
{
	ioEtcItem::LoadProperty( rkLoader );

	// 초기화.
	m_MixerPresent.clear();

	char szKey[MAX_PATH], szBuf[MAX_PATH];
	int iMaxExchangePresent = rkLoader.LoadInt( "max_present", 0 );
	for(int i = 0;i < iMaxExchangePresent;i++)
	{
		MixerPresent kPresent;

		sprintf_s( szKey, "present%d_type", i + 1 );
		kPresent.m_iPresentType = rkLoader.LoadInt( szKey, 0 );

		sprintf_s( szKey, "present%d_state", i + 1 );
		kPresent.m_iPresentState = rkLoader.LoadInt( szKey, 0 );

		sprintf_s( szKey, "present%d_ment", i + 1 );
		kPresent.m_iPresentMent = rkLoader.LoadInt( szKey, 0 );

		sprintf_s( szKey, "present%d_period", i + 1 );
		kPresent.m_iPresentPeriod = rkLoader.LoadInt( szKey, 0 );

		sprintf_s( szKey, "present%d_value1", i + 1 );
		kPresent.m_iPresentValue1 = rkLoader.LoadInt( szKey, 0 );

		sprintf_s( szKey, "present%d_value2", i + 1 );
		kPresent.m_iPresentValue2 = rkLoader.LoadInt( szKey, 0 );

		m_MixerPresent.push_back( kPresent );
	}
}

ioEtcItemRainbowMixer::MixerPresent ioEtcItemRainbowMixer::GetMixerPresent( int iIndex )
{
	if( !COMPARE( iIndex, 0, (int)m_MixerPresent.size() ) )
	{
		MixerPresent kNonePresent;
		return kNonePresent;
	}

	return m_MixerPresent[iIndex];
}

bool ioEtcItemRainbowMixer::_OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	int iSelectIndex = 0;
	PACKET_GUARD_bool( rkPacket.Read(iSelectIndex) );

	MixerPresent kPresent = GetMixerPresent( iSelectIndex );
	if( kPresent.m_iPresentType == 0 )
	{
		// 없는 상품
		SP2Packet kPacket( STPK_ETCITEM_RAINBOW_MIXER_RESULT );
		PACKET_GUARD_bool( kPacket.Write(ETCITEM_RAINBOW_MIXER_RESULT_NONE_ITEM) );
		pUser->SendMessage( kPacket );
		return false;
	}

	int iMaxEtcItem = 0;
	PACKET_GUARD_bool( rkPacket.Read(iMaxEtcItem) );
	// 3 / 4 / 7 개의 특별 아이템을 사용한다.
	enum { TRINGLE = 3, RECTANGLE = 4, ALL = 7, };
	
	if( iMaxEtcItem != TRINGLE && iMaxEtcItem != RECTANGLE && iMaxEtcItem != ALL )
	{
		// 잘못된 도형
		SP2Packet kPacket( STPK_ETCITEM_RAINBOW_MIXER_RESULT );
		PACKET_GUARD_bool( kPacket.Write(ETCITEM_RAINBOW_MIXER_RESULT_NONE_MIXER) );
		pUser->SendMessage( kPacket );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioEtcItemRainbowMixer ETCITEM_RAINBOW_MIXER_RESULT_NONE_MIXER : %s - %d", pUser->GetPublicID().c_str(), iMaxEtcItem );
		return false;
	}
	/*
	if( iMaxEtcItem <= 0 || iMaxEtcItem > ALL )
	{
		// 잘못된 도형
		SP2Packet kPacket( STPK_ETCITEM_RAINBOW_MIXER_RESULT );
		PACKET_GUARD_bool( kPacket.Write(ETCITEM_RAINBOW_MIXER_RESULT_NONE_MIXER) );
		pUser->SendMessage( kPacket );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioEtcItemRainbowMixer ETCITEM_RAINBOW_MIXER_RESULT_NONE_MIXER : %s - %d", pUser->GetPublicID().c_str(), iMaxEtcItem );
		return false;
	}
	*/
	int i = 0;
	bool bEtcItemOk = true;
	static DWORDVec kEtcItemList;
	kEtcItemList.clear();

	for(i = 0;i < iMaxEtcItem;i++)
	{
		DWORD dwEtcItem;
		PACKET_GUARD_bool( rkPacket.Read(dwEtcItem) );
		kEtcItemList.push_back( dwEtcItem );
		ioUserEtcItem::ETCITEMSLOT kSlot;
		if( pUserEtcItem->GetEtcItem( dwEtcItem, kSlot ) )
		{
			if( kSlot.GetUse() <= 0 )
				bEtcItemOk = false;
		}
		else
		{
			bEtcItemOk = false;
		}

		if( !bEtcItemOk )
			break;
	}

	if( !bEtcItemOk )
	{
		// 아이템 부족
		SP2Packet kPacket( STPK_ETCITEM_RAINBOW_MIXER_RESULT );
		PACKET_GUARD_bool( kPacket.Write(ETCITEM_RAINBOW_MIXER_RESULT_NONE_ETCITEM) );
		pUser->SendMessage( kPacket );
		return false;
	}

	// 특별 감소 먼저 전송
	SP2Packet kReturn( STPK_ETCITEM_USE );
	PACKET_GUARD_bool( kReturn.Write(ETCITEM_USE_OK) );
	PACKET_GUARD_bool( kReturn.Write(GetType()) );
	PACKET_GUARD_bool( kReturn.Write((int)kEtcItemList.size()) );

	for(i = 0;i < (int)kEtcItemList.size();i++)
	{
		ioUserEtcItem::ETCITEMSLOT kSlot;
		if( pUserEtcItem->GetEtcItem( kEtcItemList[i], kSlot ) )
		{
			kSlot.AddUse( -1 );
			if( kSlot.GetUse() <= 0 )
			{
				pUserEtcItem->DeleteEtcItem( kSlot.m_iType, LogDBClient::ET_DEL );
			}
			else
			{
				pUserEtcItem->SetEtcItem( kSlot );
			}
			PACKET_GUARD_bool( kReturn.Write(kSlot.m_iType) );
			PACKET_GUARD_bool( kReturn.Write(kSlot.m_iValue1) );
			PACKET_GUARD_bool( kReturn.Write(kSlot.m_iValue2) );
			LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "Use Etc Item (%s/%d/%d)", pUser->GetPublicID().c_str(), kSlot.m_iType, kSlot.m_iValue1 );
		}
	}
	pUser->SendMessage( kReturn );

	// 특별 사용 결과 전송
	CTimeSpan cPresentGapTime( kPresent.m_iPresentPeriod, 0, 0, 0 );
	CTime kPresentTime = CTime::GetCurrentTime() + cPresentGapTime;
	pUser->AddPresentMemory( g_MainServer.GetSendID(), kPresent.m_iPresentType, kPresent.m_iPresentValue1, kPresent.m_iPresentValue2, 
							 kPresent.m_iPresentValue3, kPresent.m_iPresentValue4, kPresent.m_iPresentMent, kPresentTime, kPresent.m_iPresentState );
	g_LogDBClient.OnInsertPresent( 0, g_MainServer.GetSendID(), g_App.GetPublicIP().c_str(), pUser->GetUserIndex(), kPresent.m_iPresentType, kPresent.m_iPresentValue1, 
									  kPresent.m_iPresentValue2, kPresent.m_iPresentValue3, kPresent.m_iPresentValue4, LogDBClient::PST_RECIEVE, "RainbowMixer" );
	pUser->SendPresentMemory();

	SP2Packet kPacket( STPK_ETCITEM_RAINBOW_MIXER_RESULT );
	PACKET_GUARD_bool( kPacket.Write(ETCITEM_RAINBOW_MIXER_RESULT_OK) );
	PACKET_GUARD_bool( kPacket.Write(GetType()) );
	PACKET_GUARD_bool( kPacket.Write(iSelectIndex) );

	pUser->SendMessage( kPacket );	
	return false;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemLostSagaMixer::ioEtcItemLostSagaMixer()
{
}

ioEtcItemLostSagaMixer::~ioEtcItemLostSagaMixer()
{

}

bool ioEtcItemLostSagaMixer::_OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	int iSelectIndex = 0;
	PACKET_GUARD_bool( rkPacket.Read(iSelectIndex) );

	MixerPresent kPresent = GetMixerPresent( iSelectIndex );
	if( kPresent.m_iPresentType == 0 )
	{
		// 없는 상품
		SP2Packet kPacket( STPK_ETCITEM_LOSTSAGA_MIXER_RESULT );
		PACKET_GUARD_bool( kPacket.Write(ETCITEM_LOSTSAGA_MIXER_RESULT_NONE_ITEM) );
		pUser->SendMessage( kPacket );
		return false;
	}

	int iMaxEtcItem = 0;
	PACKET_GUARD_bool( rkPacket.Read(iMaxEtcItem) );
	// 6 개의 특별 아이템을 사용한다.
	enum { ALL = 6, };
	if( iMaxEtcItem != ALL )
	{
		// 잘못된 도형
		SP2Packet kPacket( STPK_ETCITEM_LOSTSAGA_MIXER_RESULT );
		PACKET_GUARD_bool( kPacket.Write(ETCITEM_LOSTSAGA_MIXER_RESULT_NONE_MIXER) );
		pUser->SendMessage( kPacket );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioEtcItemLostSagaMixer ETCITEM_LOSTSAGA_MIXER_RESULT_NONE_MIXER : %s - %d", pUser->GetPublicID().c_str(), iMaxEtcItem );
		return false;
	}

	int i = 0;
	bool bEtcItemOk = true;
	static DWORDVec kEtcItemList;
	kEtcItemList.clear();

	for(i = 0;i < iMaxEtcItem;i++)
	{
		DWORD dwEtcItem = 0;
		PACKET_GUARD_bool( rkPacket.Read(dwEtcItem) );

		kEtcItemList.push_back( dwEtcItem );
		ioUserEtcItem::ETCITEMSLOT kSlot;
		if( pUserEtcItem->GetEtcItem( dwEtcItem, kSlot ) )
		{
			if( kSlot.GetUse() <= 0 )
				bEtcItemOk = false;
		}
		else
		{
			bEtcItemOk = false;
		}

		if( !bEtcItemOk )
			break;
	}

	if( !bEtcItemOk )
	{
		// 아이템 부족
		SP2Packet kPacket( STPK_ETCITEM_LOSTSAGA_MIXER_RESULT );
		PACKET_GUARD_bool( kPacket.Write(ETCITEM_LOSTSAGA_MIXER_RESULT_NONE_ETCITEM) );
		pUser->SendMessage( kPacket );
		return false;
	}

	// 특별 감소 먼저 전송
	SP2Packet kReturn( STPK_ETCITEM_USE );
	PACKET_GUARD_bool( kReturn.Write(ETCITEM_USE_OK) );
	PACKET_GUARD_bool( kReturn.Write(GetType()) );
	PACKET_GUARD_bool( kReturn.Write((int)kEtcItemList.size()) );
	
	for(i = 0;i < (int)kEtcItemList.size();i++)
	{
		ioUserEtcItem::ETCITEMSLOT kSlot;
		if( pUserEtcItem->GetEtcItem( kEtcItemList[i], kSlot ) )
		{
			kSlot.AddUse( -1 );
			if( kSlot.GetUse() <= 0 )
			{
				pUserEtcItem->DeleteEtcItem( kSlot.m_iType, LogDBClient::ET_DEL );
			}
			else
			{
				pUserEtcItem->SetEtcItem( kSlot );
			}
			PACKET_GUARD_bool( kReturn.Write(kSlot.m_iType) );
			PACKET_GUARD_bool( kReturn.Write(kSlot.m_iValue1) );
			PACKET_GUARD_bool( kReturn.Write(kSlot.m_iValue2) );
			LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "Use Etc Item (%s/%d/%d)", pUser->GetPublicID().c_str(), kSlot.m_iType, kSlot.m_iValue1 );
		}
	}
	pUser->SendMessage( kReturn );

	// 특별 사용 결과 전송
	CTimeSpan cPresentGapTime( kPresent.m_iPresentPeriod, 0, 0, 0 );
	CTime kPresentTime = CTime::GetCurrentTime() + cPresentGapTime;
	pUser->AddPresentMemory( g_MainServer.GetSendID(), kPresent.m_iPresentType, kPresent.m_iPresentValue1, kPresent.m_iPresentValue2, 
							 kPresent.m_iPresentValue3, kPresent.m_iPresentValue4, kPresent.m_iPresentMent, kPresentTime, kPresent.m_iPresentState );
	g_LogDBClient.OnInsertPresent( 0, g_MainServer.GetSendID(), g_App.GetPublicIP().c_str(), pUser->GetUserIndex(), kPresent.m_iPresentType, kPresent.m_iPresentValue1, 
									  kPresent.m_iPresentValue2, kPresent.m_iPresentValue3, kPresent.m_iPresentValue4, LogDBClient::PST_RECIEVE, "LostSagaMixer" );
	pUser->SendPresentMemory();

	SP2Packet kPacket( STPK_ETCITEM_LOSTSAGA_MIXER_RESULT );
	PACKET_GUARD_bool( kPacket.Write(ETCITEM_LOSTSAGA_MIXER_RESULT_OK) );
	PACKET_GUARD_bool( kPacket.Write(GetType()) );
	PACKET_GUARD_bool( kPacket.Write(iSelectIndex) );
	pUser->SendMessage( kPacket );	
	return false;
}
//////////////////////////////////////////////////////////////////////////
ioEtcItemTimeGashapon::ioEtcItemTimeGashapon()
{
	m_bSequenceOrder = false;
	m_bRealTimeCheck = false;
	m_iRepeatMinute	 = 0;
}

ioEtcItemTimeGashapon::~ioEtcItemTimeGashapon()
{
}

void ioEtcItemTimeGashapon::LoadProperty( ioINILoader &rkLoader )
{
	ioEtcItem::LoadProperty( rkLoader );

	m_bRealTimeCheck = rkLoader.LoadBool( "real_time_check", false );
	m_iRepeatMinute  = rkLoader.LoadInt( "repeat_minute", 0 );
	m_bSequenceOrder = rkLoader.LoadBool( "sequence_order", false );

	m_ExtendDataList.clear();
	char szKey[MAX_PATH] = "";
	int iMaxExtendData = rkLoader.LoadInt( "max_check_minute", 0 );
	if( iMaxExtendData == 0 )
		m_bSequenceOrder = false;
	
	for( int i = 0; i < iMaxExtendData; ++i )
	{
		ExtendData kData;
		
		sprintf_s( szKey, "check_minute%d", i + 1 );
		kData.m_iUseMinute = rkLoader.LoadInt( szKey, 0 );
		
		sprintf_s( szKey, "check_etc_item_type%d", i + 1 );
		kData.m_iMagicCode = rkLoader.LoadInt( szKey, 0 );

		m_ExtendDataList.push_back( kData );
	}
}

void ioEtcItemTimeGashapon::OnBuy( User *pUser,  ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	if( m_bSequenceOrder )
	{
		if( !m_ExtendDataList.empty() )
		{
			ExtendData kData = *m_ExtendDataList.begin();
			rkSlot.SetDateExcludeValue3( kData.m_iUseMinute, 0 );
		}
	}
	else
	{
		if( !m_ExtendDataList.empty() )
		{
			/************************************************************************/
			/* ExtendData에 값이 세팅되면 유지한 시간에 따라 선물이 바뀌고 사용시      */
			/* 특별아이템에서 삭제된며 획득시 사용 불가이다.						    */
			/************************************************************************/
			rkSlot.SetDateExcludeValue2( m_iRepeatMinute );
		}
	}
}

bool ioEtcItemTimeGashapon::_OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	int iClientExcludeValue2 = 0;
	PACKET_GUARD_bool( rkPacket.Read(iClientExcludeValue2) ); // 클라이언트에서 확인한 시간

	if( m_bSequenceOrder )
	{
		UseSequnceOrder( pUser, pUserEtcItem, rkSlot, iClientExcludeValue2 );
	}
	else
	{
		UseDefault( pUser, pUserEtcItem, rkSlot, iClientExcludeValue2 );
	}

	return false; // false 면 OnUse() 여기서 중단
}

void ioEtcItemTimeGashapon::UseDefault( User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot, int iClientExcludeValue2 )
{
	if( m_ExtendDataList.empty() )
	{
		/************************************************************************/
		/* RepeatMinute마다 선물 획득 가능한 아이템								*/
		/************************************************************************/
		// 사용 확인
		if( rkSlot.GetDateExcludeValue2() > 0 )
		{
			// 시간이 아직 남아있다. 예외 알림
			SP2Packet kReturn( STPK_ETCITEM_USE );
			PACKET_GUARD_VOID( kReturn.Write(ETCITEM_USE_EXCEPTION) );
			pUser->SendMessage( kReturn );
			return;
		}

		// 다음 체크 시간 갱신
		rkSlot.SetDateExcludeValue2( m_iRepeatMinute );
		pUserEtcItem->SetEtcItem( rkSlot );
		// 가챠 지급
		LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "Use Etc Item (%s/%d/%d/%d)", pUser->GetPublicID().c_str(), rkSlot.m_iType, rkSlot.m_iValue1, rkSlot.m_iValue2 );

		SendPresent( pUser, GetType(), rkSlot );
	}
	else
	{
		/************************************************************************/
		/* ExtendData에 값이 세팅되면 유지한 시간에 따라 선물이 바뀌고 사용시      */
		/* 특별아이템에서 삭제된다                                               */
		/************************************************************************/

		// 클라이언트 시간이 느리거나 같으면 클라이언트 시간 적용
		int iClientKeep = m_iRepeatMinute - iClientExcludeValue2;
		int iServerKeep = m_iRepeatMinute - rkSlot.GetDateExcludeValue2();
		int iKeepMinute = iServerKeep;
		if( iClientKeep < iServerKeep + 2 )   // 2분정도는 시간차로 평가
			iKeepMinute = iClientKeep;

		// 어떤 아이템 지급?
		int iPresentCode = -1;
		int iUseMinuteSize = m_ExtendDataList.size();
		for( int i = 0; i < iUseMinuteSize; i++ )
		{
			ExtendData &rkData = m_ExtendDataList[i];
			if( iKeepMinute >= rkData.m_iUseMinute )
			{
				iPresentCode = rkData.m_iMagicCode;				
			}
		}

		// 사용 확인
		if( iPresentCode == -1 )
		{
			// 시간이 아직 남아있다. 예외 알림
			SP2Packet kReturn( STPK_ETCITEM_USE );
			PACKET_GUARD_VOID( kReturn.Write(ETCITEM_USE_EXCEPTION) );
			pUser->SendMessage( kReturn );
			return;
		}
		LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "Use Etc Item (%s/%d/%d/%d)", pUser->GetPublicID().c_str(), rkSlot.m_iType, rkSlot.m_iValue1, rkSlot.m_iValue2 );

		SendPresent( pUser, iPresentCode, rkSlot );

		// 아이템 삭제
		pUserEtcItem->DeleteEtcItem( rkSlot.m_iType, LogDBClient::ET_DEL );
	}
}

void ioEtcItemTimeGashapon::UseSequnceOrder( User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot, int iClientExcludeValue2 )
{
	int iIndex = rkSlot.GetDateExcludeValue3State();
	int iNext  = iIndex +1;
	int iRealTime = rkSlot.GetDateExcludeValue3Time();
	
	// 2분정도는 시간차로 평가
	if( abs(iRealTime) > 2 )
	{
		LOG.PrintTimeAndLog(0 ,"%s time err : %d", __FUNCTION__, iRealTime );
		
		SP2Packet kReturn( STPK_ETCITEM_USE );
		PACKET_GUARD_VOID( kReturn.Write(ETCITEM_USE_EXCEPTION) );
		return;
	}

	if( (int)m_ExtendDataList.size() <= iIndex )
	{
		SP2Packet kReturn( STPK_ETCITEM_USE );
		PACKET_GUARD_VOID( kReturn.Write(ETCITEM_USE_EXCEPTION) );

		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s index overflow", __FUNCTION__ );
		pUserEtcItem->DeleteEtcItem( rkSlot.m_iType, LogDBClient::ET_DEL );
		return;
	}

	ExtendData kData = m_ExtendDataList[iIndex];

	//다음 시간으로 갱신
	if( iNext < (int)m_ExtendDataList.size() )
	{
		ExtendData kNextData = m_ExtendDataList[iNext];
		rkSlot.SetDateExcludeValue3( kNextData.m_iUseMinute, iNext );
		pUserEtcItem->SetEtcItem( rkSlot );
	}
	else
	{
		rkSlot.SetDateExcludeValue3( 0, 0 );
		pUserEtcItem->SetEtcItem( rkSlot );
	}

	SendPresent( pUser, kData.m_iMagicCode, rkSlot );

	if( iNext == (int)m_ExtendDataList.size() )
		pUserEtcItem->DeleteEtcItem( rkSlot.m_iType, LogDBClient::ET_DEL );
}

void ioEtcItemTimeGashapon::SendPresent( User *pUser, DWORD dwEtcItemType, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	// 선물 지급
	short iPresentType   = 0;
	int   iPresentValue1 = 0;
	int   iPresentValue2 = 0;
	int   iPresentValue3 = 0;
	int   iPresentValue4 = 0;
	bool  bWholeAlarm    = false;
	int   iPresentPeso   = 0;
	if( g_PresentHelper.SendGashaponPresent( pUser, dwEtcItemType, iPresentType, iPresentValue1, iPresentValue2, iPresentValue3, iPresentValue4, bWholeAlarm, iPresentPeso ) )
	{
		SP2Packet kReturn( STPK_ETCITEM_USE );
		PACKET_GUARD_VOID( kReturn.Write(ETCITEM_USE_OK) );
		PACKET_GUARD_VOID( kReturn.Write(rkSlot.m_iType) );
		PACKET_GUARD_VOID( kReturn.Write(rkSlot.m_iValue1) );
		PACKET_GUARD_VOID( kReturn.Write(rkSlot.m_iValue2) );
		PACKET_GUARD_VOID( kReturn.Write(iPresentType) );
		PACKET_GUARD_VOID( kReturn.Write(iPresentValue1) );
		PACKET_GUARD_VOID( kReturn.Write(iPresentValue2) );
		PACKET_GUARD_VOID( kReturn.Write(iPresentValue3) );
		PACKET_GUARD_VOID( kReturn.Write(iPresentValue4) );
		PACKET_GUARD_VOID( kReturn.Write(bWholeAlarm) );
		PACKET_GUARD_VOID( kReturn.Write(iPresentPeso) );

		pUser->SendMessage( kReturn );
		pUser->SendPresentMemory();  // 메모리 선물 전송		

		// gashapon present에 관한 전서버 알림
		g_UserNodeManager.AlarmEventInGashaponPresentToAllUsers(pUser, bWholeAlarm, this->GetType(), iPresentType, iPresentValue1, iPresentValue2);
	}
	else
	{
		LOG.PrintTimeAndLog(0, "%s - %d, %d, %d, %d", __FUNCTION__, dwEtcItemType, iPresentType, iPresentValue1, iPresentValue2 );
		SP2Packet kReturn( STPK_ETCITEM_USE );
		PACKET_GUARD_VOID( kReturn.Write(ETCITEM_USE_EXCEPTION) );
		pUser->SendMessage( kReturn );
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemGoldBox::ioEtcItemGoldBox()
{
}

ioEtcItemGoldBox::~ioEtcItemGoldBox()
{

}

bool ioEtcItemGoldBox::_OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	int iSelectIndex = 0;
	PACKET_GUARD_bool( rkPacket.Read(iSelectIndex) );

	MixerPresent kPresent = GetMixerPresent( iSelectIndex );
	if( kPresent.m_iPresentType == 0 )
	{
		// 없는 상품
		SP2Packet kPacket( STPK_ETCITEM_GOLD_BOX_RESULT );
		PACKET_GUARD_bool( kPacket.Write(ETCITEM_GOLD_BOX_RESULT_NONE_ITEM) );
		pUser->SendMessage( kPacket );
		return false;
	}

	int iMaxEtcItem = 0;
	PACKET_GUARD_bool( rkPacket.Read(iMaxEtcItem) );
	// 1 개의 특별 아이템을 사용한다.
	enum { ALL = 1, };
	if( iMaxEtcItem != ALL )
	{
		// 잘못된 도형
		SP2Packet kPacket( STPK_ETCITEM_GOLD_BOX_RESULT );
		PACKET_GUARD_bool( kPacket.Write(ETCITEM_GOLD_BOX_RESULT_NONE_MIXER) );
		pUser->SendMessage( kPacket );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioEtcItemGoldBox ETCITEM_GOLD_BOX_RESULT_NONE_MIXER : %s - %d", pUser->GetPublicID().c_str(), iMaxEtcItem );
		return false;
	}

	int i = 0;
	bool bEtcItemOk = true;
	static DWORDVec kEtcItemList;
	kEtcItemList.clear();

	for(i = 0;i < iMaxEtcItem;i++)
	{
		DWORD dwEtcItem = 0;
		PACKET_GUARD_bool( rkPacket.Read(dwEtcItem) );

		kEtcItemList.push_back( dwEtcItem );
		ioUserEtcItem::ETCITEMSLOT kSlot;
		if( pUserEtcItem->GetEtcItem( dwEtcItem, kSlot ) )
		{
			if( kSlot.GetUse() <= 0 )
				bEtcItemOk = false;
		}
		else
		{
			bEtcItemOk = false;
		}

		if( !bEtcItemOk )
			break;
	}

	if( !bEtcItemOk )
	{
		// 아이템 부족
		SP2Packet kPacket( STPK_ETCITEM_GOLD_BOX_RESULT );
		PACKET_GUARD_bool( kPacket.Write(ETCITEM_GOLD_BOX_RESULT_NONE_ETCITEM) );
		pUser->SendMessage( kPacket );
		return false;
	}

	// 사용 아이템
	rkSlot.AddUse( -1 );
	if( rkSlot.GetUse() <= 0 )
	{
		pUserEtcItem->DeleteEtcItem( rkSlot.m_iType, LogDBClient::ET_DEL );
	}
	else
	{
		pUserEtcItem->SetEtcItem( rkSlot );
	}
	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "Use Etc Item1 (%s/%d/%d)", pUser->GetPublicID().c_str(), rkSlot.m_iType, rkSlot.m_iValue1 );

	// 특별 감소 먼저 전송
	SP2Packet kReturn( STPK_ETCITEM_USE );
	PACKET_GUARD_bool( kReturn.Write(ETCITEM_USE_OK) );
	PACKET_GUARD_bool( kReturn.Write(rkSlot.m_iType) );
	PACKET_GUARD_bool( kReturn.Write(rkSlot.m_iValue1) );
	PACKET_GUARD_bool( kReturn.Write(rkSlot.m_iValue2) );
	PACKET_GUARD_bool( kReturn.Write((int)kEtcItemList.size()) );

	for(i = 0;i < (int)kEtcItemList.size();i++)
	{
		ioUserEtcItem::ETCITEMSLOT kSlot;
		if( pUserEtcItem->GetEtcItem( kEtcItemList[i], kSlot ) )
		{
			kSlot.AddUse( -1 );
			if( kSlot.GetUse() <= 0 )
			{
				pUserEtcItem->DeleteEtcItem( kSlot.m_iType, LogDBClient::ET_DEL );
			}
			else
			{
				pUserEtcItem->SetEtcItem( kSlot );
			}
			PACKET_GUARD_bool( kReturn.Write(kSlot.m_iType) );
			PACKET_GUARD_bool( kReturn.Write(kSlot.m_iValue1) );
			PACKET_GUARD_bool( kReturn.Write(kSlot.m_iValue2) );

			LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "Use Etc Item2 (%s/%d/%d)", pUser->GetPublicID().c_str(), kSlot.m_iType, kSlot.m_iValue1 );
		}
	}
	pUser->SendMessage( kReturn );

	// 특별 사용 결과 전송
	CTimeSpan cPresentGapTime( kPresent.m_iPresentPeriod, 0, 0, 0 );
	CTime kPresentTime = CTime::GetCurrentTime() + cPresentGapTime;
	pUser->AddPresentMemory( g_MainServer.GetSendID(), kPresent.m_iPresentType, kPresent.m_iPresentValue1, kPresent.m_iPresentValue2, 
							 kPresent.m_iPresentValue3, kPresent.m_iPresentValue4, kPresent.m_iPresentMent, kPresentTime, kPresent.m_iPresentState );
	g_LogDBClient.OnInsertPresent( 0, g_MainServer.GetSendID(), g_App.GetPublicIP().c_str(), pUser->GetUserIndex(), kPresent.m_iPresentType, kPresent.m_iPresentValue1, 
								  kPresent.m_iPresentValue2, kPresent.m_iPresentValue3, kPresent.m_iPresentValue4, LogDBClient::PST_RECIEVE, "GoldBox" );
	pUser->SendPresentMemory();

	SP2Packet kPacket( STPK_ETCITEM_GOLD_BOX_RESULT );
	PACKET_GUARD_bool( kPacket.Write(ETCITEM_GOLD_BOX_RESULT_OK) );
	PACKET_GUARD_bool( kPacket.Write(GetType()) );
	PACKET_GUARD_bool( kPacket.Write(iSelectIndex) );
	pUser->SendMessage( kPacket );	
	return false;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool      ioEtcItemSoldierSelector::m_bRandomize = false;
IORandom  ioEtcItemSoldierSelector::m_Random;

ioEtcItemSoldierSelector::ioEtcItemSoldierSelector()
{
	if( !m_bRandomize ) // 1번만 호출
	{
		m_Random.Randomize();
		m_bRandomize = true;
	}

	m_dwTotalRate    = 0;
	m_iPresentMent   = 0;
	m_iPresentPeriod = 0;
}

ioEtcItemSoldierSelector::~ioEtcItemSoldierSelector()
{
	m_vRandomTime.clear();
	m_vSoldierCode.clear();
}

void ioEtcItemSoldierSelector::LoadProperty( ioINILoader &rkLoader )
{
	ioEtcItem::LoadProperty( rkLoader );

	m_iPresentPeriod  = rkLoader.LoadInt( "present_period", 0 );
	m_iPresentMent    = rkLoader.LoadInt( "present_ment", 0 );


	// Soldier
	char szKeyName[MAX_PATH]="";
	int iMax  = rkLoader.LoadInt( "max_soldier", 0 );
	m_vSoldierCode.clear();
	m_vSoldierCode.reserve( iMax );
	for (int i = 0; i < iMax ; i++)
	{
		StringCbPrintf( szKeyName, sizeof( szKeyName ), "soldier%d", i+1 );
		int iSoldierCode  = rkLoader.LoadInt( szKeyName, 0 );
		if( iSoldierCode != 0 )
			m_vSoldierCode.push_back( iSoldierCode );
	}

	// Time
	iMax = rkLoader.LoadInt( "max_time", 0 );
	m_vRandomTime.clear();
	m_vRandomTime.reserve( iMax );
	m_dwTotalRate = 0;

	for( int j=0; j < iMax; ++j )
	{
		RandomTime kTime;

		StringCbPrintf( szKeyName, sizeof( szKeyName ), "time%d_rate", j+1 );
		kTime.m_iRate = rkLoader.LoadInt( szKeyName, 0 );

		StringCbPrintf( szKeyName, sizeof( szKeyName ), "time%d_value", j+1 );
		kTime.m_iTime = rkLoader.LoadInt( szKeyName, 0 );

		m_dwTotalRate += kTime.m_iRate;
		m_vRandomTime.push_back( kTime );
	}
}

void ioEtcItemSoldierSelector::OnBuy( User *pUser, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	rkSlot.SetUse( false );
}

bool ioEtcItemSoldierSelector::_OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	// OnUse에서 체크하므로 pUser 와 pUserEtcItem NULL이 아님을 보장
	if( pUser->IsReserveServerMoving() )
	{
		SP2Packet kReturn( STPK_ETCITEM_USE );
		PACKET_GUARD_bool( kReturn.Write(ETCITEM_USE_MOVING_SERVER) );
		pUser->SendMessage( kReturn );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Fail Reserve Server moving - %s) %d.",  __FUNCTION__, pUser->GetPublicID().c_str(), rkSlot.m_iType );
		return false;
	}

	// error check
	int iClassType = 0;
	PACKET_GUARD_bool( rkPacket.Read(iClassType) );
	
	bool bRight = true;
	bRight = IsRightSoldierCode( iClassType );

	int iSoldierTime = 0;
	if( bRight )
	{
		iSoldierTime = GetSoldierTime();
		if( iSoldierTime == -1 )
			bRight = false;
	}

	if( !bRight )
	{
		SP2Packet kReturn( STPK_ETCITEM_USE );
		PACKET_GUARD_bool( kReturn.Write(ETCITEM_USE_EXCEPTION) );
		pUser->SendMessage( kReturn );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s %s %d [%d]Error Soldier Code, Time[%d]", __FUNCTION__, pUser->GetPublicID().c_str(), pUser->GetUserIndex(), iClassType, rkSlot.m_iType );
		return false;
	}

	// delete
	pUserEtcItem->DeleteEtcItem( GetType(), LogDBClient::ET_DEL );

	// present
	CTimeSpan cPresentGapTime( m_iPresentPeriod, 0, 0, 0 );
	CTime kPresentTime = CTime::GetCurrentTime() + cPresentGapTime;
	pUser->AddPresentMemory( g_MainServer.GetSendID(), PRESENT_SOLDIER, iClassType, iSoldierTime, 0, 0, m_iPresentMent, kPresentTime, 0 );
	g_LogDBClient.OnInsertPresent( 0, g_MainServer.GetSendID(), g_App.GetPublicIP().c_str(), pUser->GetUserIndex(), PRESENT_SOLDIER, iClassType, 
		                           iSoldierTime, 0, 0, LogDBClient::PST_RECIEVE, "SoldierSelector" );
	pUser->SendPresentMemory();

	SP2Packet kReturn( STPK_ETCITEM_USE );
	PACKET_GUARD_bool( kReturn.Write(ETCITEM_USE_OK) );
	PACKET_GUARD_bool( kReturn.Write(GetType()) );
	PACKET_GUARD_bool( kReturn.Write(iClassType) );
	PACKET_GUARD_bool( kReturn.Write(iSoldierTime) );
	pUser->SendMessage( kReturn );

	return false; 
}

bool ioEtcItemSoldierSelector::IsRightSoldierCode( int iClassType )
{
	if( m_vSoldierCode.empty() )
		return true;
		
	int iMax = m_vSoldierCode.size();
	for (int i = 0; i < iMax ; i++)
	{
		if( iClassType == m_vSoldierCode[i] )
		{
			return true;
		}
	}

	return false;
}

int ioEtcItemSoldierSelector::GetSoldierTime()
{
	int iCurRand     = 0;
	int iTargetRand  = m_Random.Random( m_dwTotalRate );
	int iMax = m_vRandomTime.size();
	for( int i=0; i < iMax; ++i )
	{
		int iItemRand = m_vRandomTime[i].m_iRate;
		if( COMPARE( iTargetRand, iCurRand, iCurRand + iItemRand ) )
		{
			return m_vRandomTime[i].m_iTime;
		}

		iCurRand += iItemRand;
	}

	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "%s:%d:%d", __FUNCTION__, iTargetRand, iCurRand );
	return -1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool      ioEtcItemFourExtraCompound::m_bRandomize = false;
IORandom  ioEtcItemFourExtraCompound::m_Random;
IORandom  ioEtcItemFourExtraCompound::m_SucessRandom;

ioEtcItemFourExtraCompound::ioEtcItemFourExtraCompound()
{
	if( !m_bRandomize ) // 1번만 호출
	{
		m_Random.Randomize();
		m_SucessRandom.Randomize();
		m_bRandomize = true;
	}

	m_dwTotalRate    = 0;
	m_dwSuccessRate  = 0;
	m_iPresentMent   = 0;
	m_iPresentPeriod = 0;
}

ioEtcItemFourExtraCompound::~ioEtcItemFourExtraCompound()
{
	m_vRandomTime.clear();
	m_vSoldierCode.clear();
}

void ioEtcItemFourExtraCompound::LoadProperty( ioINILoader &rkLoader )
{
	ioEtcItem::LoadProperty( rkLoader );

	m_iPresentPeriod  = rkLoader.LoadInt( "present_period", 0 );
	m_iPresentMent    = rkLoader.LoadInt( "present_ment", 0 );

	// Seccess
	m_dwSuccessRate = rkLoader.LoadInt( "success_rate", 0 );

	// Soldier
	char szKeyName[MAX_PATH]="";
	int iMax  = rkLoader.LoadInt( "max_soldier", 0 );
	m_vSoldierCode.clear();
	m_vSoldierCode.reserve( iMax );
	for (int i = 0; i < iMax ; i++)
	{
		StringCbPrintf( szKeyName, sizeof( szKeyName ), "soldier%d", i+1 );
		int iSoldierCode  = rkLoader.LoadInt( szKeyName, 0 );
		if( iSoldierCode != 0 )
			m_vSoldierCode.push_back( iSoldierCode );
	}

	// Time
	iMax = rkLoader.LoadInt( "max_time", 0 );
	m_vRandomTime.clear();
	m_vRandomTime.reserve( iMax );
	m_dwTotalRate = 0;

	for( int j=0; j < iMax; ++j )
	{
		RandomTime kTime;

		StringCbPrintf( szKeyName, sizeof( szKeyName ), "time%d_rate", j+1 );
		kTime.m_iRate = rkLoader.LoadInt( szKeyName, 0 );

		StringCbPrintf( szKeyName, sizeof( szKeyName ), "time%d_value", j+1 );
		kTime.m_iTime = rkLoader.LoadInt( szKeyName, 0 );

		m_dwTotalRate += kTime.m_iRate;
		m_vRandomTime.push_back( kTime );
	}
}

void ioEtcItemFourExtraCompound::OnBuy( User *pUser, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	rkSlot.SetUse( false );
}

bool ioEtcItemFourExtraCompound::_OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	// OnUse에서 체크하므로 pUser 와 pUserEtcItem NULL이 아님을 보장
	if( pUser->IsReserveServerMoving() )
	{
		SP2Packet kReturn( STPK_ETCITEM_USE );
		PACKET_GUARD_bool( kReturn.Write(ETCITEM_USE_MOVING_SERVER) );
		pUser->SendMessage( kReturn );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Fail Reserve Server moving - %s) %d.",  __FUNCTION__, pUser->GetPublicID().c_str(), rkSlot.m_iType );
		return false;
	}

	// error check
	int iClassType = 0;
	int iExtraItemSlotIdxArray[MAX_EXTRA_ITEM] = { 0, 0, 0, 0 };
	PACKET_GUARD_bool( rkPacket.Read(iClassType) );

	for (int i = 0; i < MAX_EXTRA_ITEM ; i++)
	{
		PACKET_GUARD_bool( rkPacket.Read(iExtraItemSlotIdxArray[i]) );
	}

	bool bRight = true;
	bRight = IsRightSoldierCode( iClassType );

	int iSoldierTime = 0;
	if( bRight )
	{
		iSoldierTime = GetSoldierTime();
		if( iSoldierTime == -1 )
			bRight = false;
	}

	if( bRight )
	{
		bRight = IsRightExtraItem( pUser, iClassType, iExtraItemSlotIdxArray );
	}

	if( !bRight )
	{
		SP2Packet kReturn( STPK_ETCITEM_USE );
		PACKET_GUARD_bool( kReturn.Write(ETCITEM_USE_EXCEPTION) );
		pUser->SendMessage( kReturn );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s %s %d [%d]Error[%d]", __FUNCTION__, pUser->GetPublicID().c_str(), pUser->GetUserIndex(), iClassType, rkSlot.m_iType );
		return false;
	}

	// delete
	ioUserExtraItem *pExtraItem = pUser->GetUserExtraItem();
	for (int i = 0; i < MAX_EXTRA_ITEM ; i++)
	{
		if(!pExtraItem || !pExtraItem->DeleteExtraItem( iExtraItemSlotIdxArray[i] ))
		{
			SP2Packet kReturn( STPK_ETCITEM_USE );
			PACKET_GUARD_bool( kReturn.Write(ETCITEM_USE_EXCEPTION) );
			pUser->SendMessage( kReturn );
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s %s %d [%d]ErrorDelete[%d]", __FUNCTION__, pUser->GetPublicID().c_str(), pUser->GetUserIndex(), iClassType, iExtraItemSlotIdxArray[i] );
			return false;
		}
	}

	// 사용 아이템
	rkSlot.AddUse( -1 );
	if( rkSlot.GetUse() <= 0 )
	{
		pUserEtcItem->DeleteEtcItem( rkSlot.m_iType, LogDBClient::ET_DEL );
	}
	else
	{
		pUserEtcItem->SetEtcItem( rkSlot );
	}
	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "Use Etc Item1 (%s/%d/%d)", pUser->GetPublicID().c_str(), rkSlot.m_iType, rkSlot.m_iValue1 );

	// Success
	bool bSuccess = false;
	DWORD dwRand = m_SucessRandom.Random( MAX_SUCCESS );
	if( dwRand < m_dwSuccessRate )
	{
		// present
		CTimeSpan cPresentGapTime( m_iPresentPeriod, 0, 0, 0 );
		CTime kPresentTime = CTime::GetCurrentTime() + cPresentGapTime;
		pUser->AddPresentMemory( g_MainServer.GetSendID(), PRESENT_SOLDIER, iClassType, iSoldierTime, 0, 0, m_iPresentMent, kPresentTime, 0 );
		g_LogDBClient.OnInsertPresent( 0, g_MainServer.GetSendID(), g_App.GetPublicIP().c_str(), pUser->GetUserIndex(), PRESENT_SOLDIER, iClassType, 
			iSoldierTime, 0, 0, LogDBClient::PST_RECIEVE, "FourExtraCompound" );
		pUser->SendPresentMemory();
		bSuccess = true;
	}

	SP2Packet kReturn( STPK_ETCITEM_USE );
	PACKET_GUARD_bool( kReturn.Write(ETCITEM_USE_OK) );
	PACKET_GUARD_bool( kReturn.Write(rkSlot.m_iType) );
	PACKET_GUARD_bool( kReturn.Write(rkSlot.m_iValue1) );
	PACKET_GUARD_bool( kReturn.Write(rkSlot.m_iValue2) );
	PACKET_GUARD_bool( kReturn.Write(iClassType) );
	PACKET_GUARD_bool( kReturn.Write(bSuccess) );

	if( bSuccess )
		PACKET_GUARD_bool( kReturn.Write(iSoldierTime) );

	for (int i = 0; i < MAX_EXTRA_ITEM ; i++)
	{
		PACKET_GUARD_bool( kReturn.Write(iExtraItemSlotIdxArray[i]) );
	}
	pUser->SendMessage( kReturn );

	return false; 
}

bool ioEtcItemFourExtraCompound::IsRightSoldierCode( int iClassType )
{
	int iMax = m_vSoldierCode.size();
	for (int i = 0; i < iMax ; i++)
	{
		if( m_vSoldierCode[0] == -1 ) // -1이면 모든 용병 지급 가능
			return true;

		if( iClassType == m_vSoldierCode[i] )
		{
			return true;
		}
	}

	return false;
}

int ioEtcItemFourExtraCompound::GetSoldierTime()
{
	int iCurRand     = 0;
	int iTargetRand  = m_Random.Random( m_dwTotalRate );
	int iMax = m_vRandomTime.size();
	for( int i=0; i < iMax; ++i )
	{
		int iItemRand = m_vRandomTime[i].m_iRate;
		if( COMPARE( iTargetRand, iCurRand, iCurRand + iItemRand ) )
		{
			return m_vRandomTime[i].m_iTime;
		}

		iCurRand += iItemRand;
	}

	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "%s:%d:%d", __FUNCTION__, iTargetRand, iCurRand );
	return -1;
}

bool ioEtcItemFourExtraCompound::IsRightExtraItem( User *pUser, int iClassType, int iExtraItemSlotIdxArray[ioEtcItemFourExtraCompound::MAX_EXTRA_ITEM] )
{
	// check exist
	ioUserExtraItem::EXTRAITEMSLOT kExtraItemArray[MAX_EXTRA_ITEM];
	for (int i = 0; i < MAX_EXTRA_ITEM ; i++)
	{
		if( !pUser )
			return false;
		if( !pUser->GetUserExtraItem() )
			return false;
		if( !pUser->GetUserExtraItem()->GetExtraItem( iExtraItemSlotIdxArray[i], kExtraItemArray[i] ) )
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Error1:%d", __FUNCTION__, iExtraItemSlotIdxArray[i] );
			return false;
		}
	}

	// check code
	static IntVec vExtraItemCode;
	vExtraItemCode.clear();

	vExtraItemCode.reserve( MAX_EXTRA_ITEM );
	vExtraItemCode.push_back(  10000+iClassType );
	vExtraItemCode.push_back( 110000+iClassType );
	vExtraItemCode.push_back( 210000+iClassType );
	vExtraItemCode.push_back( 310000+iClassType );

	for (int i = 0; i < MAX_EXTRA_ITEM ; i++)
	{
		int iSize = vExtraItemCode.size();
		for (int j = 0; j < iSize ; j++)
		{
			if( kExtraItemArray[i].m_iItemCode == vExtraItemCode[j] )	
			{
				vExtraItemCode.erase( vExtraItemCode.begin() + j );
				break;
			}
		}
	}

	if( !vExtraItemCode.empty() )
	{
		int iSize = vExtraItemCode.size();
		for (int j = 0; j < iSize ; j++)
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Error2:%d", __FUNCTION__, vExtraItemCode[j] );	
		}
		return false;
	}
	
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemExpandMedalSlot::ioEtcItemExpandMedalSlot()
{
}

ioEtcItemExpandMedalSlot::~ioEtcItemExpandMedalSlot()
{
}

void ioEtcItemExpandMedalSlot::LoadProperty( ioINILoader &rkLoader )
{
	ioEtcItem::LoadProperty( rkLoader );

	// 셋팅 편의상 입력값에서 -1을 해준다.
	m_iUseLevel = rkLoader.LoadInt( "use_level", 0 );
	m_dwLimitTime = rkLoader.LoadInt( "limit_time", 0 );

	switch( m_iUseLevel )
	{
	case 10:
		m_iSlotNumber = SLOT_NUM2;
		break;
	case 20:
		m_iSlotNumber = SLOT_NUM3;
		break;
	case 30:
		m_iSlotNumber = SLOT_NUM4;
		break;
	case 40:
		m_iSlotNumber = SLOT_NUM5;
		break;
	case 50:
		m_iSlotNumber = SLOT_NUM6;
		break;
	default:
		m_iSlotNumber = SLOT_NONE;
	}
}

bool ioEtcItemExpandMedalSlot::_OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	// OnUse에서 체크하므로 pUser 와 pUserEtcItem NULL이 아님을 보장
	if( !pUser->OnExpandMedalSlotOpen( rkPacket, GetType() ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Fail - %s",  __FUNCTION__, pUser->GetPublicID().c_str() );
		return false;
	}

	rkSlot.AddUse( -1 );
	if( rkSlot.GetUse() <= 0 )
	{
		pUserEtcItem->DeleteEtcItem( rkSlot.m_iType, LogDBClient::ET_DEL );
	}
	else
	{
		pUserEtcItem->SetEtcItem( rkSlot );
	}
	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "Use Etc Item (%s/%d/%d)", pUser->GetPublicID().c_str(), rkSlot.m_iType, rkSlot.m_iValue1 );

	return true;
}
//////////////////////////////////////////////////////////////////////////
ioEtcItemSoldierExpBonus::ioEtcItemSoldierExpBonus()
{
	m_iLimitLevel = 0;
}

ioEtcItemSoldierExpBonus::~ioEtcItemSoldierExpBonus()
{
}

void ioEtcItemSoldierExpBonus::LoadProperty( ioINILoader &rkLoader )
{
	ioEtcItem::LoadProperty( rkLoader );

	m_iLimitLevel = rkLoader.LoadInt( "limit_level", 0 );
}

void ioEtcItemSoldierExpBonus::OnBuy( User *pUser,  ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	rkSlot.SetDateExcludeValue2( 0 );
}

bool ioEtcItemSoldierExpBonus::_OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	if( rkSlot.GetDateExcludeValue2() > 0 )
	{
		// 이미 선택한 용병이 있다.
		SP2Packet kReturn( STPK_ETCITEM_USE );
		PACKET_GUARD_bool( kReturn.Write(ETCITEM_USE_EXCEPTION) );
		pUser->SendMessage( kReturn );
		return false;
	}

	int iSelectSoldier = 0;
	PACKET_GUARD_bool( rkPacket.Read(iSelectSoldier) );

	if( pUser->IsCharClassType( iSelectSoldier ) == false )   // 용병 체크
	{
		// 이미 선택한 용병이 있다.
		SP2Packet kReturn( STPK_ETCITEM_USE );
		PACKET_GUARD_bool( kReturn.Write(ETCITEM_USE_CHAR_NONE) );
		pUser->SendMessage( kReturn );
		return false;
	}
	//
	rkSlot.SetDateExcludeValue2( iSelectSoldier );
	pUserEtcItem->SetEtcItem( rkSlot );

	SP2Packet kReturn( STPK_ETCITEM_USE );
	PACKET_GUARD_bool( kReturn.Write(ETCITEM_USE_OK) );
	PACKET_GUARD_bool( kReturn.Write(rkSlot.m_iType) );
	PACKET_GUARD_bool( kReturn.Write(rkSlot.m_iValue1) );
	PACKET_GUARD_bool( kReturn.Write(rkSlot.m_iValue2) );

	pUser->SendMessage( kReturn );
	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "Use Etc Item (%s/%d/%d/%d)", pUser->GetPublicID().c_str(), rkSlot.m_iType, rkSlot.m_iValue1, rkSlot.m_iValue2 );

	char szNote[100]="";
	sprintf_s( szNote, "[%d - %d]Etc", rkSlot.m_iValue1, rkSlot.m_iValue2 );
	g_LogDBClient.OnInsertEtc( pUser, GetType(), iSelectSoldier, 0, szNote, LogDBClient::ET_APPLY );

	return false; // false 면 OnUse() 여기서 중단
}



ioEtcItemConsumption::ioEtcItemConsumption()
{
}

ioEtcItemConsumption::~ioEtcItemConsumption()
{

}

void ioEtcItemConsumption::LoadProperty( ioINILoader &rkLoader )
{
	ioEtcItem::LoadProperty( rkLoader );

	// Present
	char szBuf[MAX_PATH]="";
	rkLoader.LoadString( "slot_buff", "", szBuf, MAX_PATH );
	m_strBuffName = szBuf;
}

bool ioEtcItemConsumption::_OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	Room *pRoom = pUser->GetMyRoom();
	if(!pRoom) return false;

	if( !Help::IsMonsterDungeonMode(pRoom->GetModeType()) &&
		!(pRoom->GetModeType() == MT_RAID))
	{
		SP2Packet kReturn( STPK_ETCITEM_USE );
		PACKET_GUARD_bool( kReturn.Write(ETCITEM_USE_EXCEPTION) );
		pUser->SendMessage( kReturn );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Fail - %s",  __FUNCTION__, pUser->GetPublicID().c_str() );
		return false;
	}

	rkSlot.AddUse( -1 );
	if( rkSlot.GetUse() <= 0 )
	{
		pUserEtcItem->DeleteEtcItem( rkSlot.m_iType, LogDBClient::ET_DEL );
	}
	else
	{
		pUserEtcItem->SetEtcItem( rkSlot );
	}
	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "Use Etc Item (%s/%d/%d)", pUser->GetPublicID().c_str(), rkSlot.m_iType, rkSlot.m_iValue1 );

	// 특별아이템 소모처리를 먼저 발생시켜야 한다
	SP2Packet kReturn( STPK_SLOT_BUFF );
	PACKET_GUARD_bool( kReturn.Write(m_dwType) );
	PACKET_GUARD_bool( kReturn.Write(pUser->GetPublicID()) );
	PACKET_GUARD_bool( kReturn.Write(m_strBuffName) );
	pRoom->RoomSendPacketTcp(kReturn);
	return true;
}



ioEtcItemRevive::ioEtcItemRevive()
{
}

ioEtcItemRevive::~ioEtcItemRevive()
{

}

bool ioEtcItemRevive::_OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	Room *pRoom = pUser->GetMyRoom();

	if(!pRoom) return false;

	if( !Help::IsMonsterDungeonMode(pRoom->GetModeType()) &&
		!(pRoom->GetModeType() == MT_RAID))
	{
		SP2Packet kReturn( STPK_ETCITEM_USE );
		PACKET_GUARD_bool( kReturn.Write(ETCITEM_USE_EXCEPTION) );
		pUser->SendMessage( kReturn );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Fail - %s",  __FUNCTION__, pUser->GetPublicID().c_str() );
		return false;
	}

	rkSlot.AddUse( -1 );
	if( rkSlot.GetUse() <= 0 )
	{
		pUserEtcItem->DeleteEtcItem( rkSlot.m_iType, LogDBClient::ET_DEL );
	}
	else
	{
		pUserEtcItem->SetEtcItem( rkSlot );
	}
	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "Use Etc Item (%s/%d/%d)", pUser->GetPublicID().c_str(), rkSlot.m_iType, rkSlot.m_iValue1 );

	SP2Packet kReturn( STPK_SLOT_BUFF );
	PACKET_GUARD_bool( kReturn.Write(m_dwType) );
	PACKET_GUARD_bool( kReturn.Write(pUser->GetPublicID()) );

	pUser->EquipDBItemToAllChar();
	pUser->FillEquipItemData( kReturn );
	ioCharacter *rkChar = pUser->GetCharacter( pUser->GetSelectChar() ); 
	if(rkChar)
	{
		rkChar->FillEquipAccessoryInfo( kReturn, pUser );
	}
	else
	{
		PACKET_GUARD_bool( kReturn.Write( "" ) );
		for( int i = 0; i < MAX_CHAR_ACCESSORY_SLOT; i++ )
		{
			PACKET_GUARD_bool(kReturn.Write(0));
			PACKET_GUARD_bool(kReturn.Write(0));
			PACKET_GUARD_bool(kReturn.Write(0));
		}
	}
	pUser->FillEquipMedalItem( kReturn );
	
	pRoom->RoomSendPacketTcp(kReturn);
	pRoom->ResetRevive(pUser);  // 부활 메시지 안 날리게 한다.
	

	return true;
}

///////////////////////////////////////////////////////////////////////////
ioEtcItemSelectExtraGashapon::ioEtcItemSelectExtraGashapon()
{
	m_ExtraRandom.Randomize();
	m_SucessRandom.Randomize();

	m_iExtraRareDefaultRate   = 0;
	m_iExtraNormalDefaultRate = 0;
	m_iSelectRate             = 0;
	m_dwExtraTotalRate        = 0;
	m_iExtraItemMachineCode   = 0;
}

ioEtcItemSelectExtraGashapon::~ioEtcItemSelectExtraGashapon()
{
	m_vExtraItemInfo.clear();
}

void ioEtcItemSelectExtraGashapon::LoadProperty( ioINILoader &rkLoader )
{
	ioEtcItem::LoadProperty( rkLoader );

	m_iSelectType = rkLoader.LoadInt( "select_type", ST_RANDOM_ADD );

	// rate
	m_iExtraRareDefaultRate   = rkLoader.LoadInt( "rare_rate", 18 );
	m_iExtraNormalDefaultRate = rkLoader.LoadInt( "normal_rate", 82 );
	m_iSelectRate             = rkLoader.LoadInt( "select_rate", 6 );
	m_iExtraItemMachineCode   = rkLoader.LoadInt( "extraitem_machine_code", 0 );	

	// extra
	m_vExtraItemInfo.clear();

	ioINILoader kLoaderExtra( "config/sp2_extraitem_info.ini" );

	kLoaderExtra.SetTitle( "common_info" );
	int iCnt = kLoaderExtra.LoadInt( "random_machine_cnt", 0 );

	bool bError = true;
	for( int i=0; i < iCnt; i++ )
	{
		char szTitle[MAX_PATH]="";
		StringCbPrintf( szTitle, sizeof( szTitle ), "random_machine%d", i+1 );
		kLoaderExtra.SetTitle( szTitle );
		int iMachineCode = kLoaderExtra.LoadInt( "machine_code", 0 );
		if( iMachineCode == m_iExtraItemMachineCode )
		{
			if( iMachineCode != 0 )
				bError = false;
			break;
		}
	}

	if( bError )
		return; ///////////////////////////////////////////////////////////

	int iItemCnt = kLoaderExtra.LoadInt( "item_cnt", 0 );

	char szKey[MAX_PATH]="";
	for( int i=0; i < iItemCnt; ++i )
	{
		ExtraItemInfo kInfo;

		StringCbPrintf( szKey, sizeof( szKey ), "item%d_code", i+1 );
		kInfo.m_iCode = kLoaderExtra.LoadInt( szKey, 0 );

		StringCbPrintf( szKey, sizeof( szKey ), "item%d_rate", i+1 );
		kInfo.m_iRate = kLoaderExtra.LoadInt( szKey, 0 );

		if( kInfo.m_iRate <= 0 )
			continue;

		if( ( kInfo.m_iCode%10000 ) > 1000 )
			kInfo.m_iRate = m_iExtraRareDefaultRate;
		else
			kInfo.m_iRate = m_iExtraNormalDefaultRate;

		m_vExtraItemInfo.push_back( kInfo );
	}
	//
}

void ioEtcItemSelectExtraGashapon::OnAfterBuy( User *pUser, SP2Packet &rkPacket, int iArray, int iTransactionID, int bUseGoldCash )
{
	if( !pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pUser == NULL.", __FUNCTION__ );
		return;
	}
	
	if( m_vExtraItemInfo.empty() )
	{
		SP2Packet kReturn( STPK_BUY_SELECT_EXTRA_GASHAPON );
		PACKET_GUARD_VOID( kReturn.Write(BUY_SELECT_EXTRA_GASHAPON_EXCEPTION) );
		pUser->SendMessage( kReturn );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Extra Item Info empty.(%d:%s)", __FUNCTION__, pUser->GetUserIndex(), pUser->GetPublicID().c_str() );
		return;
	}

	static IntVec vSelectExtraInt;
	vSelectExtraInt.clear();

	pUser->GetSelectExtraItemCodes( vSelectExtraInt );
	if( vSelectExtraInt.empty() )
	{
		SP2Packet kReturn( STPK_BUY_SELECT_EXTRA_GASHAPON );
		PACKET_GUARD_VOID( kReturn.Write(BUY_SELECT_EXTRA_GASHAPON_EXCEPTION) );
		pUser->SendMessage( kReturn );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s select extra item empty.(%d:%s)", __FUNCTION__, pUser->GetUserIndex(), pUser->GetPublicID().c_str() );
		return;
	}

	if( iArray != vSelectExtraInt.size()-1)
	{
		SP2Packet kReturn( STPK_BUY_SELECT_EXTRA_GASHAPON );
		PACKET_GUARD_VOID( kReturn.Write(BUY_SELECT_EXTRA_GASHAPON_EXCEPTION) );
		pUser->SendMessage( kReturn );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s select extra size wrong.(%d:%s)", __FUNCTION__, pUser->GetUserIndex(), pUser->GetPublicID().c_str() );
		return;
	}

	int iSelectCodeCnt = 0;
	m_dwExtraTotalRate = 0;
	int iSelectSize = vSelectExtraInt.size();
	int iSize = m_vExtraItemInfo.size();
	for (int i = 0; i < iSize ; i++)
	{
		m_vExtraItemInfo[i].m_bActive = true; // 초기화
		m_dwExtraTotalRate += m_vExtraItemInfo[i].m_iRate;

		for (int j = 0; j < iSelectSize ; j++) // select 제거
		{
			if( m_vExtraItemInfo[i].m_iCode == vSelectExtraInt[j] )
			{
				m_vExtraItemInfo[i].m_bActive = false;
				m_dwExtraTotalRate -= m_vExtraItemInfo[i].m_iRate;
				iSelectCodeCnt++;
			}
		}
	}

	if( iSelectCodeCnt != iSelectSize )
	{
		SP2Packet kReturn( STPK_BUY_SELECT_EXTRA_GASHAPON );
		PACKET_GUARD_VOID( kReturn.Write(BUY_SELECT_EXTRA_GASHAPON_EXCEPTION) );
		pUser->SendMessage( kReturn );

		for (int i = 0; i < iSelectSize ; i++)
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s wrong select code %d.(%d:%s)", __FUNCTION__, vSelectExtraInt[i], pUser->GetUserIndex(), pUser->GetPublicID().c_str() );	
		}
		return;
	}

	// select
	static ExtraItemInfoVector vRandomExtraInfo;
	vRandomExtraInfo.clear();

	for (int i = 0; i < iSelectSize ; i++)
	{
		ExtraItemInfo kInfo;
		kInfo.m_iCode = vSelectExtraInt[i];
		kInfo.m_iRate = m_iSelectRate;
		vRandomExtraInfo.push_back( kInfo );
	}

	int iRandomSize = vRandomExtraInfo.size();
	int iCurRate = 0;
	for (int i = 0; i < iRandomSize ; i++)
	{
		iCurRate += vRandomExtraInfo[i].m_iRate;
	}

	int iAddRate = MAX_SUCCESS - iCurRate;
	int iAddSize = MAX_SLOT - vRandomExtraInfo.size();
	int iAddOneRate = 0;

	switch( m_iSelectType )
	{
	case ST_RANDOM_ADD:
		{
			if( iAddRate <= 0 || iAddSize <= 0 )
			{
				SP2Packet kReturn( STPK_BUY_SELECT_EXTRA_GASHAPON );
				PACKET_GUARD_VOID( kReturn.Write(BUY_SELECT_EXTRA_GASHAPON_EXCEPTION) );
				pUser->SendMessage( kReturn );
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s wrong calculate %d:%d.(%d:%s)", __FUNCTION__, iAddRate, iAddSize,  pUser->GetUserIndex(), pUser->GetPublicID().c_str() );	
				return;
			}

			iAddOneRate = iAddRate/iAddSize;


		}
		break;
	case ST_SELECT_ALL:
		{
			if( iAddRate < 0 || iAddSize < 0 )
			{
				SP2Packet kReturn( STPK_BUY_SELECT_EXTRA_GASHAPON );
				PACKET_GUARD_VOID( kReturn.Write(BUY_SELECT_EXTRA_GASHAPON_EXCEPTION) );
				pUser->SendMessage( kReturn );
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s wrong calculate %d:%d.(%d:%s)", __FUNCTION__, iAddRate, iAddSize,  pUser->GetUserIndex(), pUser->GetPublicID().c_str() );
				return;
			}

			if( 0 < iAddRate && 0 < iAddSize )
			{
				iAddOneRate = iAddRate/iAddSize;
			}
			else
			{
				iAddOneRate = 0;
				iAddRate = 0;
				iAddSize = 0;
			}
		}
		break;
	default:
		{
			SP2Packet kReturn( STPK_BUY_SELECT_EXTRA_GASHAPON );
			PACKET_GUARD_VOID( kReturn.Write(BUY_SELECT_EXTRA_GASHAPON_EXCEPTION) );
			pUser->SendMessage( kReturn );
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s wrong select type(%d:%s)", __FUNCTION__, pUser->GetUserIndex(), pUser->GetPublicID().c_str() );
			return;
		}
		break;
	}

	for (int i = 0; i < iAddSize ; i++)
	{
		ExtraItemInfo kInfo;
		kInfo.m_iCode = GetExtraItemCode();
		if( kInfo.m_iCode <= 0 )
			continue;
		kInfo.m_iRate = iAddOneRate;
		vRandomExtraInfo.push_back( kInfo );
	}

	// 나머지 체크해서 더한다.
	int iMax = vRandomExtraInfo.size();
	int iCheckRate = 0;
	for (int i = 0; i < iMax ; i++)
	{
		iCheckRate += vRandomExtraInfo[i].m_iRate;
	}

	int iRemainRate = MAX_SUCCESS - iCheckRate;
	if( iRemainRate < 0 )
	{
		SP2Packet kReturn( STPK_BUY_SELECT_EXTRA_GASHAPON );
		PACKET_GUARD_VOID( kReturn.Write(BUY_SELECT_EXTRA_GASHAPON_EXCEPTION) );
		pUser->SendMessage( kReturn );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s wrong remain rate %d.(%d:%s)", __FUNCTION__, iRemainRate,  pUser->GetUserIndex(), pUser->GetPublicID().c_str() );	
		return;
	}

	if( !vRandomExtraInfo.empty() )
		vRandomExtraInfo[iMax-1].m_iRate += iRemainRate;


	// choice
	int iGetExtraItemCode = 0;
	int iCurRand    = 0;
	int iTargetRand = m_SucessRandom.Random( MAX_SUCCESS ); 
	for (int i = 0; i < (int)vRandomExtraInfo.size() ; i++)
	{
		int iItemRand = vRandomExtraInfo[i].m_iRate;
		if( COMPARE( iTargetRand, iCurRand, iCurRand + iItemRand ) )
		{
			iGetExtraItemCode = vRandomExtraInfo[i].m_iCode;
			break;
		}

		iCurRand += iItemRand;
	}

	// extra set
	int iTradeTypeList = 0;
	ioUserExtraItem::EXTRAITEMSLOT kExtraItem;
	kExtraItem.m_iItemCode   = iGetExtraItemCode;
	kExtraItem.m_iReinforce  = g_ExtraItemInfoMgr.GetRandomReinforce( m_iExtraItemMachineCode, false );
	kExtraItem.m_PeriodType  = ioUserExtraItem::EPT_MORTMAIN;
	kExtraItem.m_iTradeState = g_ExtraItemInfoMgr.GetRandomTradeType( m_iExtraItemMachineCode, iTradeTypeList );
	
	if( !pUser->GetUserExtraItem() )
	{
		SP2Packet kReturn( STPK_BUY_SELECT_EXTRA_GASHAPON );
		PACKET_GUARD_VOID( kReturn.Write(BUY_SELECT_EXTRA_GASHAPON_EXCEPTION) );
		pUser->SendMessage( kReturn );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s User ExtraItem NULL.(%d:%s)", __FUNCTION__ , pUser->GetUserIndex(), pUser->GetPublicID().c_str() );
		return;
	}

	int iPayAmt = GetCash( iArray );
	int iPeriodTime = 0;
	DWORD dwIndex = 0;
	int iArrayIndex = 0;
	int iSlotIndex = pUser->GetUserExtraItem()->AddExtraItem( kExtraItem, true, iPayAmt, LogDBClient::CIT_ETC, DEFAULT_MACHINECODE, iPeriodTime, dwIndex, iArrayIndex );

	ioLocalParent *pLocal =  g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );

	int iPreCash = pUser->GetCash();
	int iPreChannelingCash = pUser->GetChannelingCash();
	
	ioChannelingNodeParent *pNode = g_ChannelingMgr.GetNode( pUser->GetChannelingType() );
	if( !pNode )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pNode == NULL. : %s:%s:%d", __FUNCTION__, pUser->GetPublicID().c_str(), pUser->GetBillingGUID().c_str(), iPayAmt );
		SP2Packet kReturn( STPK_BUY_SELECT_EXTRA_GASHAPON );
		PACKET_GUARD_VOID( kReturn.Write(BUY_SELECT_EXTRA_GASHAPON_EXCEPTION) );
		pUser->SendMessage( kReturn );

		if( pLocal )
			pLocal->SendRefundCash( pUser, iTransactionID, true );
		return;
	}

	if( pLocal )
	{
		if( !pLocal->UpdateOutputCash( pUser , rkPacket, iPayAmt, STPK_BUY_SELECT_EXTRA_GASHAPON, BUY_SELECT_EXTRA_GASHAPON_EXCEPTION ) )
			return;
	}

	int iRealPayAmt = 0;
	/******************************************************************************************/
	if( bUseGoldCash == 1 )
	{

		//HRYOON 보너스 캐쉬 사용
		int iBonusCashSize = 0;
		int iSpendBonusCash = 0;
		PACKET_GUARD_VOID( rkPacket.Read(iBonusCashSize) );
		for( int i = 0; i < iBonusCashSize; i++ )
		{
			int iIndex	= 0;
			int iValue	= 0;
			PACKET_GUARD_VOID( rkPacket.Read(iIndex) );
			PACKET_GUARD_VOID( rkPacket.Read(iValue) );

			iSpendBonusCash += iValue;

			//보너스 캐쉬 사용!
			if( !pUser->m_UserBonusCash.SpendBonusCash(iIndex, iValue, PRESENT_ETC_ITEM, kExtraItem.m_iValue1, kExtraItem.m_iValue2) )
			{

				SP2Packet kReturn( STPK_BUY_SELECT_EXTRA_GASHAPON );
				PACKET_GUARD_VOID( kReturn.Write(ETCITEM_BUY_EXCEPTION) );
				pUser->SendMessage( kReturn );

				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "User::OnEtcItemBuy Spend Bonus Cash Error - guid:%s,publicID:%s(userIndex:%d) Index:%d:money:%d %d",pUser->GetBillingGUID().c_str(), pUser->GetPublicID().c_str(), pUser->GetUserIndex(), iIndex, iValue, kExtraItem.m_iItemCode );


			}
		}

		
		iRealPayAmt = iPayAmt - iSpendBonusCash;
	
		if( iSlotIndex > 0 )
		{
			char szItemIndex[MAX_PATH]="";
			if( dwIndex != 0 )
			{
				if( iRealPayAmt != 0)
				{
					StringCbPrintf( szItemIndex, sizeof( szItemIndex ), "%d.%d.%u-%u", kExtraItem.m_iItemCode,  ( kExtraItem.m_iReinforce*10000 ) + iPeriodTime, dwIndex, iArrayIndex+1 ); // db field는 1부터 이므로 +1
					g_LogDBClient.OnInsertCashItem( pUser, m_dwType , GetValue( iArray ), iRealPayAmt, szItemIndex, LogDBClient::CIT_ETC, pUser->GetBillingGUID().c_str() );
				}
			}
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ExtraItemLOG(ETC) : AddItem(%s:%d) : %d-%d : Cash", pUser->GetPublicID().c_str(), pUser->GetUserIndex(), DEFAULT_MACHINECODE, kExtraItem.m_iItemCode );
		}
		else
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Fail Extraitem2(ETC) : %s(%d)",__FUNCTION__, pUser->GetPublicID().c_str(), pUser->GetUserIndex() );

			SP2Packet kReturn( STPK_BUY_SELECT_EXTRA_GASHAPON );
			PACKET_GUARD_VOID( kReturn.Write(BUY_SELECT_EXTRA_GASHAPON_EXCEPTION) );
			pUser->SendMessage( kReturn );
			return;
		}
	}
	/******************************************************************************************/
	

	__int64 iPreMoney = pUser->GetMoney();
	int iBonusPeso = GetBonusPeso( iArray );
	if( iBonusPeso > 0 )
	{
		pUser->AddMoney( iBonusPeso );
		g_LogDBClient.OnInsertPeso( pUser, iBonusPeso, LogDBClient::PT_BUY_CASH );
		g_LogDBClient.OnInsertGameLogInfo(LogDBClient::GLT_PESO_GAIN, pUser, 0, 0, LogDBClient::LET_ITEM, LogDBClient::PT_BUY_CASH, PRESENT_ETC_ITEM, kExtraItem.m_iItemCode, iBonusPeso, NULL);
	}

	// send
	SP2Packet kReturn( STPK_BUY_SELECT_EXTRA_GASHAPON );
	PACKET_GUARD_VOID( kReturn.Write(BUY_SELECT_EXTRA_GASHAPON_OK) );
	PACKET_GUARD_VOID( kReturn.Write(kExtraItem.m_iItemCode) );
	PACKET_GUARD_VOID( kReturn.Write(kExtraItem.m_iReinforce) );
	PACKET_GUARD_VOID( kReturn.Write(iSlotIndex) );
	PACKET_GUARD_VOID( kReturn.Write(kExtraItem.m_iTradeState) );
	PACKET_GUARD_VOID( kReturn.Write(kExtraItem.m_PeriodType) );
	PACKET_GUARD_VOID( kReturn.Write(kExtraItem.m_iValue1) );
	PACKET_GUARD_VOID( kReturn.Write(kExtraItem.m_iValue2) );
	PACKET_GUARD_VOID( kReturn.Write(kExtraItem.m_dwMaleCustom) );
	PACKET_GUARD_VOID( kReturn.Write(kExtraItem.m_dwFemaleCustom) );
	PACKET_GUARD_VOID( kReturn.Write(iPeriodTime) );
	PACKET_GUARD_VOID( kReturn.Write(pUser->GetMoney()) );
	PACKET_GUARD_VOID( kReturn.Write(pUser->GetCash()) );
	PACKET_GUARD_VOID( kReturn.Write(pUser->GetChannelingCash()) );

	pUser->SendMessage( kReturn );

	pUser->SaveExtraItem();
	pUser->SaveUserData();

	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Success %s:%s:%d:%d:%dCash:%dPrice:%dPreCash:%I64dMoney:%I64dPreMoney:%dBonusPeso:%dPreChannelingCash:%dChannelingCash"
	, __FUNCTION__, pUser->GetPublicID().c_str(), pUser->GetBillingGUID().c_str(), m_dwType, 0, pUser->GetCash(), iPayAmt, iPreCash, pUser->GetMoney(), pUser->GetMoney(), 0, iPreChannelingCash, pUser->GetChannelingCash() );

	if( pLocal )
		pLocal->SendRefundCash( pUser, iTransactionID, false );

	// 구매 선물
	int iServerValue = GetValue( iArray );
	static IntVec vEventTypeVec;
	vEventTypeVec.clear();
	vEventTypeVec.reserve(10);
	pUser->GetEventUserMgr().GetSameClassEventTypeVec( EVT_BUY_ITEM, vEventTypeVec );
	int iESize = vEventTypeVec.size();
	for (int i = 0; i < iESize ; i++)
	{
		BuyItemEventUserNode *pEventNode = static_cast< BuyItemEventUserNode* > ( pUser->GetEventUserMgr().GetEventUserNode( (EventType)vEventTypeVec[i] ) );
		if( pEventNode )
		{
			if ( bUseGoldCash == 1 )
				pEventNode->SendBuyPresent( pUser, false, ioPresentHelper::BT_ETC, m_dwType, iServerValue, true, iPayAmt, iRealPayAmt );
		}
	}
}

int ioEtcItemSelectExtraGashapon::GetExtraItemCode()
{
	int iCurRand     = 0;
	int iTargetRand  = m_ExtraRandom.Random( m_dwExtraTotalRate );
	int iMax = m_vExtraItemInfo.size();
	for( int i=0; i < iMax; ++i )
	{
		if( !m_vExtraItemInfo[i].m_bActive )
			continue;

		int iItemRand = m_vExtraItemInfo[i].m_iRate;
		if( COMPARE( iTargetRand, iCurRand, iCurRand + iItemRand ) )
		{
			m_vExtraItemInfo[i].m_bActive = false;
			m_dwExtraTotalRate -= m_vExtraItemInfo[i].m_iRate;
			return m_vExtraItemInfo[i].m_iCode;
		}

		iCurRand += iItemRand;
	}

	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "%s:%d:%d", __FUNCTION__, iTargetRand, iCurRand );
	return -1;
}

bool ioEtcItemSelectExtraGashapon::_OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	int iSelectCount	= 0;
	static IntVec vSelectExtraInt;
	vSelectExtraInt.clear();

	PACKET_GUARD_bool(rkPacket.Read(iSelectCount));
	if( iSelectCount <= 0 )
		return false;

	MAX_GUARD(iSelectCount, MAX_SELECT_EXTRA_ITEM_CODE);
	if( 0 == iSelectCount )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[warning][etcitem]Wrong select count : [%lu])", pUser->GetUserIndex() );	
		return false;
	}

	for( int i = 0; i < iSelectCount; i++ )
	{
		int iCode = 0;
		PACKET_GUARD_bool(rkPacket.Read(iCode));
		if( iCode <= 0 )
			continue;

		vSelectExtraInt.push_back(iCode);
	}

	if( vSelectExtraInt.empty() )
		return false;

	int iSelectCodeCnt = 0;
	m_dwExtraTotalRate = 0;
	int iSelectSize = vSelectExtraInt.size();
	int iSize = m_vExtraItemInfo.size();
	for (int i = 0; i < iSize ; i++)
	{
		m_vExtraItemInfo[i].m_bActive = true; // 초기화
		m_dwExtraTotalRate += m_vExtraItemInfo[i].m_iRate;

		for (int j = 0; j < iSelectSize ; j++) // select 제거
		{
			if( m_vExtraItemInfo[i].m_iCode == vSelectExtraInt[j] )
			{
				m_vExtraItemInfo[i].m_bActive = false;
				m_dwExtraTotalRate -= m_vExtraItemInfo[i].m_iRate;
				iSelectCodeCnt++;
			}
		}
	}

	if( iSelectCodeCnt != iSelectSize )
	{
		SP2Packet kReturn( STPK_USE_SELECT_GASHAPON );
		PACKET_GUARD_bool(kReturn.Write(SELECT_GASHAPON_EXCEPTION));
		pUser->SendMessage( kReturn );

		for (int i = 0; i < iSelectSize ; i++)
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[warning][etcitem]Wrong select code : [%lu] [%d]", pUser->GetUserIndex(), vSelectExtraInt[i]);	
		}
		return false;
	}

	// select
	static ExtraItemInfoVector vRandomExtraInfo;
	vRandomExtraInfo.clear();

	for (int i = 0; i < iSelectSize ; i++)
	{
		ExtraItemInfo kInfo;
		kInfo.m_iCode = vSelectExtraInt[i];
		kInfo.m_iRate = m_iSelectRate;
		vRandomExtraInfo.push_back( kInfo );
	}

	int iRandomSize = vRandomExtraInfo.size();
	int iCurRate = 0;
	for (int i = 0; i < iRandomSize ; i++)
	{
		iCurRate += vRandomExtraInfo[i].m_iRate;
	}

	int iAddRate = MAX_SUCCESS - iCurRate;
	int iAddSize = MAX_SLOT - vRandomExtraInfo.size();
	int iAddOneRate = 0;

	switch( m_iSelectType )
	{
	case ST_RANDOM_ADD:
		{
			if( iAddRate <= 0 || iAddSize <= 0 )
			{
				SP2Packet kReturn( STPK_USE_SELECT_GASHAPON );
				PACKET_GUARD_bool(kReturn.Write(SELECT_GASHAPON_EXCEPTION));
				pUser->SendMessage( kReturn );
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[warning][etcitem]Wrong calculate : [%lu] [%d] [%d]", pUser->GetUserIndex(), iAddRate, iAddSize );	
				return false;
			}
			iAddOneRate = iAddRate / iAddSize;

		}
		break;
	case ST_SELECT_ALL:
		{
			if( iAddRate < 0 || iAddSize < 0 )
			{
				SP2Packet kReturn( STPK_USE_SELECT_GASHAPON );
				PACKET_GUARD_bool(kReturn.Write(SELECT_GASHAPON_EXCEPTION));
				pUser->SendMessage( kReturn );
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[warning][etcitem]Wrong calculate : [%lu] [%d] [%d]", pUser->GetUserIndex(), iAddRate, iAddSize );
				return false;
			}

			if( 0 < iAddRate && 0 < iAddSize )
			{
				iAddOneRate = iAddRate/iAddSize;
			}
			else
			{
				iAddOneRate = 0;
				iAddRate = 0;
				iAddSize = 0;
			}
		}
		break;
	default:
		{
			SP2Packet kReturn( STPK_USE_SELECT_GASHAPON );
			PACKET_GUARD_bool(kReturn.Write(SELECT_GASHAPON_EXCEPTION));
			pUser->SendMessage( kReturn );
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[warning][etcitem]Wrong select type : [%lu] [%d]", pUser->GetUserIndex(), m_iSelectType );
			return false;
		}
		break;
	}

	for (int i = 0; i < iAddSize ; i++)
	{
		ExtraItemInfo kInfo;
		kInfo.m_iCode = GetExtraItemCode();
		if( kInfo.m_iCode <= 0 )
			continue;
		kInfo.m_iRate = iAddOneRate;
		vRandomExtraInfo.push_back( kInfo );
	}

	// 나머지 체크해서 더한다.
	int iMax = vRandomExtraInfo.size();
	int iCheckRate = 0;
	for (int i = 0; i < iMax ; i++)
	{
		iCheckRate += vRandomExtraInfo[i].m_iRate;
	}

	int iRemainRate = MAX_SUCCESS - iCheckRate;
	if( iRemainRate < 0 )
	{
		SP2Packet kReturn( STPK_USE_SELECT_GASHAPON );
		PACKET_GUARD_bool(kReturn.Write(SELECT_GASHAPON_EXCEPTION));
		pUser->SendMessage( kReturn );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[warning][etcitem]Wrong remain rate : [%d] [%d]", pUser->GetUserIndex(), iRemainRate );	
		return false;
	}

	if( !vRandomExtraInfo.empty() )
		vRandomExtraInfo[iMax-1].m_iRate += iRemainRate;


	// choice
	int iGetExtraItemCode = 0;
	int iCurRand    = 0;
	int iTargetRand = m_SucessRandom.Random( MAX_SUCCESS ); 
	for (int i = 0; i < (int)vRandomExtraInfo.size() ; i++)
	{
		int iItemRand = vRandomExtraInfo[i].m_iRate;
		if( COMPARE( iTargetRand, iCurRand, iCurRand + iItemRand ) )
		{
			iGetExtraItemCode = vRandomExtraInfo[i].m_iCode;
			break;
		}

		iCurRand += iItemRand;
	}

	// extra set
	int iTradeTypeList = 0;
	ioUserExtraItem::EXTRAITEMSLOT kExtraItem;
	kExtraItem.m_iItemCode   = iGetExtraItemCode;
	kExtraItem.m_iReinforce  = g_ExtraItemInfoMgr.GetRandomReinforce( m_iExtraItemMachineCode, false );
	kExtraItem.m_PeriodType  = ioUserExtraItem::EPT_MORTMAIN;
	kExtraItem.m_iTradeState = g_ExtraItemInfoMgr.GetRandomTradeType( m_iExtraItemMachineCode, iTradeTypeList );

	ioUserExtraItem* pUserExtraItem = pUser->GetUserExtraItem();
	if( !pUserExtraItem )
		return false;

	int iPeriodTime = 0;
	DWORD dwIndex = 0;
	int iArrayIndex = 0;
	int iSlotIndex = pUserExtraItem->AddExtraItem(kExtraItem, true, 0, LogDBClient::ERT_PRESENT, DEFAULT_MACHINECODE, iPeriodTime, dwIndex, iArrayIndex);

	if( iSlotIndex <= 0 )
	{
		SP2Packet kReturn( STPK_BUY_SELECT_EXTRA_GASHAPON );
		PACKET_GUARD_bool( kReturn.Write(BUY_SELECT_EXTRA_GASHAPON_EXCEPTION) );
		pUser->SendMessage( kReturn );
		return false;
	}

	//사용 수량 변경
	rkSlot.AddUse(-1);
	if( rkSlot.GetUse() <= 0 )
	{
		pUserEtcItem->DeleteEtcItem( rkSlot.m_iType, LogDBClient::ET_DEL );
	}
	else
	{
		pUserEtcItem->SetEtcItem( rkSlot );
	}

	//뽑은정보 send
	SP2Packet kReturn( STPK_USE_SELECT_GASHAPON );
	PACKET_GUARD_bool(kReturn.Write(EXTRA_SELECT_GASHAPON));
	PACKET_GUARD_bool(kReturn.Write(kExtraItem.m_iItemCode));
	PACKET_GUARD_bool(kReturn.Write(kExtraItem.m_iReinforce));
	PACKET_GUARD_bool(kReturn.Write(iSlotIndex));
	PACKET_GUARD_bool(kReturn.Write(kExtraItem.m_iTradeState));
	PACKET_GUARD_bool(kReturn.Write(kExtraItem.m_PeriodType));
	PACKET_GUARD_bool(kReturn.Write(kExtraItem.m_iValue1));
	PACKET_GUARD_bool(kReturn.Write(kExtraItem.m_iValue2));
	PACKET_GUARD_bool(kReturn.Write(kExtraItem.m_dwMaleCustom));
	PACKET_GUARD_bool(kReturn.Write(kExtraItem.m_dwFemaleCustom));
	PACKET_GUARD_bool(kReturn.Write(iPeriodTime));
	pUser->SendMessage( kReturn );


	pUser->SaveEtcItem();
	pUser->SaveExtraItem();

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemPreSetPackage::ioEtcItemPreSetPackage()
{
}

ioEtcItemPreSetPackage::~ioEtcItemPreSetPackage()
{

}

void ioEtcItemPreSetPackage::OnBuy( User *pUser, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	rkSlot.SetUse( false );
}

bool ioEtcItemPreSetPackage::_OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	// OnUse에서 체크하므로 pUser 와 pUserEtcItem NULL이 아님을 보장
	if( pUser->IsReserveServerMoving() )
	{
		SP2Packet kReturn( STPK_ETCITEM_USE );
		PACKET_GUARD_bool( kReturn.Write(ETCITEM_USE_MOVING_SERVER) );
		pUser->SendMessage( kReturn );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Fail Reserve Server moving - %s) %d.",  __FUNCTION__, pUser->GetPublicID().c_str(), rkSlot.m_iType );
		return false;
	}
	
	pUser->OnEtcItemPreSetPackage( m_iClassType, m_iLimitTime, m_vItemSlot, m_dwType, false );
	return false; 
}

bool ioEtcItemPreSetPackage::SetPreSetPackage( User *pUser )
{
	return pUser->OnEtcItemPreSetPackage( m_iClassType, m_iLimitTime, m_vItemSlot, m_dwType, true );
}

void ioEtcItemPreSetPackage::LoadProperty( ioINILoader &rkLoader )
{
	ioEtcItem::LoadProperty( rkLoader );

	m_iLimitTime = rkLoader.LoadInt( "preset_period", 0 );

	m_iClassType = rkLoader.LoadInt( "preset_class", 0 );

	m_vItemSlot.clear();

	// 성별
	ITEMSLOT kSlotKindred;
	kSlotKindred.m_item_type = (m_iClassType * 100000) + (RDT_HUMAN_MAN * 1000) + UID_KINDRED;
	kSlotKindred.m_item_code = rkLoader.LoadInt( "preset_kindred", 0 );

	m_vItemSlot.push_back( kSlotKindred );

	// 헤어
	ITEMSLOT kSlotHair;
	kSlotHair.m_item_type = (m_iClassType * 100000) + (kSlotKindred.m_item_code * 1000) + UID_HAIR;
	kSlotHair.m_item_code = rkLoader.LoadInt( "preset_hair", 0 );

	m_vItemSlot.push_back( kSlotHair );

	// 헤어 컬러
	ITEMSLOT kSlotHairColor;
	kSlotHairColor.m_item_type = (m_iClassType * 100000) + (kSlotKindred.m_item_code * 1000) + UID_HAIR_COLOR;
	kSlotHairColor.m_item_code = rkLoader.LoadInt( "preset_hair_color", 0 );

	m_vItemSlot.push_back( kSlotHairColor );

	// 얼굴
	ITEMSLOT kSlotFace;
	kSlotFace.m_item_type = (m_iClassType * 100000) + (kSlotKindred.m_item_code * 1000) + UID_FACE;
	kSlotFace.m_item_code = rkLoader.LoadInt( "preset_face", 0 );

	m_vItemSlot.push_back( kSlotFace );

	// 피부색
	ITEMSLOT kSlotSkin;
	kSlotSkin.m_item_type = (m_iClassType * 100000) + (kSlotKindred.m_item_code * 1000) + UID_SKIN_COLOR;
	kSlotSkin.m_item_code = rkLoader.LoadInt( "preset_skin_color", 0 );

	m_vItemSlot.push_back( kSlotSkin );

	// 속옷
	ITEMSLOT kSlotUnder;
	kSlotUnder.m_item_type = (m_iClassType * 100000) + (kSlotKindred.m_item_code * 1000) + UID_UNDERWEAR;
	kSlotUnder.m_item_code = rkLoader.LoadInt( "preset_underwear", 0 );

	m_vItemSlot.push_back( kSlotUnder );
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemGrowthAllDown::ioEtcItemGrowthAllDown()
{
}

ioEtcItemGrowthAllDown::~ioEtcItemGrowthAllDown()
{
}

bool ioEtcItemGrowthAllDown::_OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	// OnUse에서 체크하므로 pUser 와 pUserEtcItem NULL이 아님을 보장

	int iClassType = 0;
	PACKET_GUARD_bool( rkPacket.Read(iClassType) );

	// exception
	if( !pUser->IsCharClassType( iClassType ) )
	{
		SP2Packet kPacket( STPK_GROWTH_ALL_LEVEL_DOWN );
		PACKET_GUARD_bool( kPacket.Write(pUser->GetPublicID()) );
		PACKET_GUARD_bool( kPacket.Write(GROWTH_ALL_LEVEL_DOWN_FAIL) );
		pUser->SendMessage( kPacket );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s ClassType Error %d.(%d:%s)", __FUNCTION__, iClassType, pUser->GetUserIndex(), pUser->GetPublicID().c_str() );
		return false;
	}

	ioUserGrowthLevel *pLevel = pUser->GetUserGrowthLevel();
	if( !pLevel )
	{
		SP2Packet kPacket( STPK_GROWTH_ALL_LEVEL_DOWN );
		PACKET_GUARD_bool( kPacket.Write(pUser->GetPublicID()) );
		PACKET_GUARD_bool( kPacket.Write(GROWTH_ALL_LEVEL_DOWN_FAIL) );
		pUser->SendMessage( kPacket );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pLevel == NULL.(%d:%s)", __FUNCTION__, pUser->GetUserIndex(), pUser->GetPublicID().c_str() );
		return false;
	}

	int iTotalLevel = 0;
	int iCharLevels[MAX_CHAR_GROWTH]={ 0, 0, 0, 0 };
	for( int i = 0; i < MAX_CHAR_GROWTH; i++ )
	{
		iCharLevels[i] = pLevel->GetCharGrowthLevel( iClassType, i, true );
		iTotalLevel += iCharLevels[i];
	}

	int iItemLevels[MAX_ITEM_GROWTH]={ 0, 0, 0, 0 };
	for( int i = 0; i < MAX_ITEM_GROWTH; i++ )
	{
		iItemLevels[i] = pLevel->GetItemGrowthLevel( iClassType, i, true );
		iTotalLevel += iItemLevels[i];
	}

	if( iTotalLevel <= 0 )
	{
		SP2Packet kPacket( STPK_GROWTH_ALL_LEVEL_DOWN );
		PACKET_GUARD_bool( kPacket.Write(pUser->GetPublicID()) );
		PACKET_GUARD_bool( kPacket.Write(GROWTH_ALL_LEVEL_DOWN_FAIL) );
		pUser->SendMessage( kPacket );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Error TotalLevel == %d.(%d:%s)", __FUNCTION__, iTotalLevel, pUser->GetUserIndex(), pUser->GetPublicID().c_str() );
		return false;
	}

	for (int i = TIG_WEAPON; i < TIG_DROP+1 ; i++)
	{
		if( pLevel->HasTimeGrowthValue( iClassType, i ) )
		{
			SP2Packet kPacket( STPK_GROWTH_ALL_LEVEL_DOWN );
			PACKET_GUARD_bool( kPacket.Write(pUser->GetPublicID()) );
			PACKET_GUARD_bool( kPacket.Write(GROWTH_ALL_LEVEL_DOWN_TIME_GROWTH) );
			pUser->SendMessage( kPacket );
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Error Time Growthing.(%d:%s)", __FUNCTION__, pUser->GetUserIndex(), pUser->GetPublicID().c_str() );
			return false;
		}	
	}

	// 페소 처리
	__int64 iReturnPeso = 0;
	for( int i = 0; i < MAX_CHAR_GROWTH; i++ )
	{
		for (int j = 1; j < iCharLevels[i]+1 ; j++)
		{
			iReturnPeso += g_GrowthMgr.GetGrowthReturnAllPeso( true, j );	
		}
	}
	for( int i = 0; i < MAX_ITEM_GROWTH; i++ )
	{
		for (int j = 1; j < iItemLevels[i]+1 ; j++)
		{
			iReturnPeso += g_GrowthMgr.GetGrowthReturnAllPeso( false, j );	
		}
	}
#if !defined (_LSWC)
	if( iReturnPeso >= g_GrowthMgr.GetReturnAllLimitPeso() )
	{
		SP2Packet kPacket( STPK_GROWTH_ALL_LEVEL_DOWN );
		PACKET_GUARD_bool( kPacket.Write(pUser->GetPublicID()) );
		PACKET_GUARD_bool( kPacket.Write(GROWTH_ALL_LEVEL_DOWN_FAIL) );
		pUser->SendMessage( kPacket );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Error Return Peso.(%d:%s) [%I64d]", __FUNCTION__, pUser->GetUserIndex(), pUser->GetPublicID().c_str(), iReturnPeso );
		return false;
	}
#endif

	if( iReturnPeso > 0 )
	{
		pUser->AddMoney( iReturnPeso );
		g_LogDBClient.OnInsertPeso( pUser, iReturnPeso, LogDBClient::PT_GROWTH_ALL_DOWN );
		g_LogDBClient.OnInsertGameLogInfo(LogDBClient::GLT_PESO_GAIN, pUser, 0, 0, LogDBClient::LET_ITEM, LogDBClient::PT_GROWTH_ALL_DOWN, PRESENT_ETC_ITEM, rkSlot.m_iType, iReturnPeso, NULL);
	}

	// 사용한 육성 초기화
	for( int i = 0; i < MAX_CHAR_GROWTH; i++ )
	{
		if( iCharLevels[i] <= 0 )
			continue;
		pLevel->CharGrowthLevelDown( iClassType, i, iCharLevels[i] );
	}

	for( int i = 0; i < MAX_ITEM_GROWTH; i++ )
	{
		if( iItemLevels[i] <= 0 )
			continue;
		pLevel->ItemGrowthLevelDown( iClassType, i, iItemLevels[i] );
	}

	// 남은 포인트 복원
	int iClassLevel = pUser->GetClassLevelByType(iClassType,true);
	int iTotalPoint = g_GrowthMgr.CheckCurTotalGrowthPoint(iClassLevel);
	    // 여자치장을 가지고 있으면 기본 강화갯수를 더준다.
	ITEMSLOT kSlot;
	const int iSexType = 0; // 0 : 남자 , 1 : 여자
	kSlot.m_item_type = ( iClassType*100000 ) + ( iSexType * 1000 ) + UID_KINDRED; // 해당 종족에 여자 치장
	kSlot.m_item_code = RDT_HUMAN_WOMAN;
	ioInventory *pInventory = pUser->GetInventory();
	if( pInventory && pInventory->IsSlotItem( kSlot ) )
		iTotalPoint += g_GrowthMgr.GetWomanTotalGrowthPoint();

	pLevel->SetCharGrowthPoint( iClassType, iTotalPoint );

	// etc setting
	rkSlot.AddUse( -1 );
	if( rkSlot.GetUse() <= 0 )
	{
		pUserEtcItem->DeleteEtcItem( rkSlot.m_iType, LogDBClient::ET_DEL );
	}
	else
	{
		pUserEtcItem->SetEtcItem( rkSlot );
	}
	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Use Etc Item (%s/%d/%d)[%I64d:%d]", pUser->GetPublicID().c_str(), rkSlot.m_iType, rkSlot.m_iValue1, iReturnPeso, iTotalLevel );

	// save
	pUser->SaveGrowth();
	pUser->SaveUserData();
	pUser->SaveEtcItem();

	// 특별아이템 소모처리를 먼저 발생시켜야 한다
	SP2Packet kReturn( STPK_ETCITEM_USE );
	PACKET_GUARD_bool( kReturn.Write(ETCITEM_USE_OK) );
	PACKET_GUARD_bool( kReturn.Write(rkSlot.m_iType) );
	PACKET_GUARD_bool( kReturn.Write(rkSlot.m_iValue1) );
	PACKET_GUARD_bool( kReturn.Write(rkSlot.m_iValue2) );
	pUser->SendMessage( kReturn );

	// send
	SP2Packet kPacket( STPK_GROWTH_ALL_LEVEL_DOWN );
	PACKET_GUARD_bool( kPacket.Write(pUser->GetPublicID()) );
	PACKET_GUARD_bool( kPacket.Write(GROWTH_ALL_LEVEL_DOWN_SUCESS) );
	PACKET_GUARD_bool( kPacket.Write(iClassType) );

	for( int i = 0; i < MAX_CHAR_GROWTH; i++ )
		PACKET_GUARD_bool( kPacket.Write(iCharLevels[i]) );
	for( int i = 0; i < MAX_ITEM_GROWTH; i++ )
		PACKET_GUARD_bool( kPacket.Write(iItemLevels[i]) );

	PACKET_GUARD_bool( kPacket.Write(iTotalPoint) );
	PACKET_GUARD_bool( kPacket.Write(iReturnPeso) );
	PACKET_GUARD_bool( kPacket.Write(pUser->GetMoney()) );

	if( pUser->GetMyRoom() )
	{
		pUser->GetMyRoom()->RoomSendPacketTcp( kPacket );

		for( int i = 0; i < MAX_CHAR_GROWTH; i++ )
		{
			if( iCharLevels[i] <= 0 )
				continue;
			pUser->GetMyRoom()->OnModeCharGrowthUpdate( pUser, iClassType, i, false, -iCharLevels[i] );
		}
		for( int i = 0; i < MAX_ITEM_GROWTH; i++ )
		{
			if( iItemLevels[i] <= 0 )
				continue;
			pUser->GetMyRoom()->OnModeCharGrowthUpdate( pUser, iClassType, i, true, -iItemLevels[i] );
		}
	}
	else
		pUser->SendMessage( kPacket );
	//

	return false; // false 면 OnUse() 여기서 중단
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemPrizeItem::ioEtcItemPrizeItem()
{
}

ioEtcItemPrizeItem::~ioEtcItemPrizeItem()
{
}

void ioEtcItemPrizeItem::LoadProperty( ioINILoader &rkLoader )
{
	ioEtcItem::LoadProperty( rkLoader );

	char szKey[MAX_PATH]="", szBuf[MAX_PATH]="";

	m_PrizeDataVec.clear();

	int iPrizeCnt = rkLoader.LoadInt( "prize_cnt", 0 );
	for( int i=0; i < iPrizeCnt; ++i )
	{
		PrizeData kData;
		
		wsprintf( szKey, "prize%d_type", i+1 );
		kData.m_iPresentType = rkLoader.LoadInt( szKey, 0 );
		wsprintf( szKey, "prize%d_ment", i+1 );
		kData.m_iPresentMent = rkLoader.LoadInt( szKey, 0 );
		wsprintf( szKey, "prize%d_period", i+1 );
		kData.m_iPresentPeriod = rkLoader.LoadInt( szKey, 0 );

		wsprintf( szKey, "prize%d_value1", i+1 );
		kData.m_iPresentValue1 = rkLoader.LoadInt( szKey, 0 );
		wsprintf( szKey, "prize%d_value2", i+1 );
		kData.m_iPresentValue2 = rkLoader.LoadInt( szKey, 0 );
		wsprintf( szKey, "prize%d_value3", i+1 );
		kData.m_iPresentValue3 = rkLoader.LoadInt( szKey, 0 );
		wsprintf( szKey, "prize%d_value4", i+1 );
		kData.m_iPresentValue4 = rkLoader.LoadInt( szKey, 0 );

		m_PrizeDataVec.push_back( kData );
	}
}

bool ioEtcItemPrizeItem::_OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	// OnUse에서 체크하므로 pUser 와 pUserEtcItem NULL이 아님을 보장
	rkSlot.AddUse( -1 );
	if( rkSlot.GetUse() <= 0 )
	{
		pUserEtcItem->DeleteEtcItem( rkSlot.m_iType, LogDBClient::ET_DEL );
	}
	else
	{
		pUserEtcItem->SetEtcItem( rkSlot );
	}
	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "Use Etc Item (%s/%d/%d)", pUser->GetPublicID().c_str(), rkSlot.m_iType, rkSlot.m_iValue1 );	
	if( _OnPrizeSend( pUser ) )
		pUser->SendPresentMemory();

	return true;
}

bool ioEtcItemPrizeItem::_OnPrizeSend( User *pUser )
{
	if( pUser == NULL ) return false;

	// 특별 사용 결과 전송 - 선물의 Send는 함수 호출 구문에서 처리
	int iSize = m_PrizeDataVec.size();
	for( int i=0; i<iSize; ++i )
	{
		PrizeData kData = m_PrizeDataVec[i];

		CTimeSpan cPresentGapTime( kData.m_iPresentPeriod, 0, 0, 0 );
		CTime kPresentTime = CTime::GetCurrentTime() + cPresentGapTime;
		pUser->AddPresentMemory( g_MainServer.GetSendID(), kData.m_iPresentType,
								 kData.m_iPresentValue1, kData.m_iPresentValue2, kData.m_iPresentValue3, kData.m_iPresentValue4,
								 kData.m_iPresentMent, kPresentTime, ioUserPresent::PRESENT_STATE_NEW );

		g_LogDBClient.OnInsertPresent( 0, g_MainServer.GetSendID(), g_App.GetPublicIP().c_str(), pUser->GetUserIndex(), kData.m_iPresentType, kData.m_iPresentValue1, 
										kData.m_iPresentValue2, kData.m_iPresentValue3, kData.m_iPresentValue4, LogDBClient::PST_RECIEVE, "CustomTournamentReward" );
	}
	return true;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemTournamentCreate::ioEtcItemTournamentCreate()
{

}

ioEtcItemTournamentCreate::~ioEtcItemTournamentCreate()
{

}


void ioEtcItemTournamentCreate::OnBuy( User *pUser, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	rkSlot.SetUse( false );
}

bool ioEtcItemTournamentCreate::_OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	// OnUse에서 체크하므로 pUser 와 pUserEtcItem NULL이 아님을 보장
	if( pUser->IsReserveServerMoving() )
	{
		SP2Packet kReturn( STPK_ETCITEM_USE );
		PACKET_GUARD_bool( kReturn.Write(ETCITEM_USE_MOVING_SERVER) );
		pUser->SendMessage( kReturn );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Fail Reserve Server moving - %s) %d.",  __FUNCTION__, pUser->GetPublicID().c_str(), rkSlot.m_iType );
		return false;
	}

	// 유저 대회 데이터 분석
	// 대회 시작 날짜 == 생성 날짜
	CTime cCurrentTime = CTime::GetCurrentTime();
	DWORD dwStartDate = Help::ConvertCTimeToDate( cCurrentTime );

	// 대회 종료 날짜 : 유저가 세팅하지 않았으면 2주간 진행
	DWORD dwEndDate = 0;
	PACKET_GUARD_bool( rkPacket.Read(dwEndDate) );

	if( dwEndDate == 0 )
	{
		CTimeSpan cGapTime( 14, 0, 0, 0 );       // 2주
		CTime cEndTime = cCurrentTime + cGapTime;
		dwEndDate = Help::ConvertCTimeToDate( cEndTime );
	}

	// 대회 정보
	ioHashString kTourName;
	SHORT MaxRound = 0;
	DWORD dwBannerLargeIndex = 0;
	DWORD dwBannerSmallIndex = 0;
	int iModeBattleType = 0;
	BYTE MaxPlayer = 0;
	BYTE RoundType = 0;
	DWORD dwAppDate = 0;
	DWORD dwDelayDate = 0;
	
	PACKET_GUARD_bool( rkPacket.Read(kTourName) );
	PACKET_GUARD_bool( rkPacket.Read(MaxRound) );
	PACKET_GUARD_bool( rkPacket.Read(dwBannerLargeIndex) );
	PACKET_GUARD_bool( rkPacket.Read(dwBannerSmallIndex) );
	PACKET_GUARD_bool( rkPacket.Read(iModeBattleType) );
	PACKET_GUARD_bool( rkPacket.Read(MaxPlayer) );
	PACKET_GUARD_bool( rkPacket.Read(RoundType) );
	PACKET_GUARD_bool( rkPacket.Read(dwAppDate) );
	PACKET_GUARD_bool( rkPacket.Read(dwDelayDate) );

	// 대회 라운드 시간
	DWORD dwRoundDate1 = 0 , dwRoundDate2 = 0, dwRoundDate3 = 0, dwRoundDate4 = 0, dwRoundDate5 = 0, dwRoundDate6 = 0, dwRoundDate7 = 0, dwRoundDate8 = 0, dwRoundDate9 = 0, dwRoundDate10 = 0;
	PACKET_GUARD_bool( rkPacket.Read(dwRoundDate1) );
	PACKET_GUARD_bool( rkPacket.Read(dwRoundDate2) );
	PACKET_GUARD_bool( rkPacket.Read(dwRoundDate3) );
	PACKET_GUARD_bool( rkPacket.Read(dwRoundDate4) );
	PACKET_GUARD_bool( rkPacket.Read(dwRoundDate5) );
	PACKET_GUARD_bool( rkPacket.Read(dwRoundDate6) );
	PACKET_GUARD_bool( rkPacket.Read(dwRoundDate7) );
	PACKET_GUARD_bool( rkPacket.Read(dwRoundDate8) );
	PACKET_GUARD_bool( rkPacket.Read(dwRoundDate9) );
	PACKET_GUARD_bool( rkPacket.Read(dwRoundDate10) );

	if( !g_App.IsRightID( kTourName.c_str() ) )
	{
		//잘못된 아이디
		SP2Packet kPacket( STPK_TOURNAMENT_CUSTOM_CREATE );
		PACKET_GUARD_bool( kPacket.Write(TOURNAMENT_CUSTOM_CREATE_NAME_FAILED) );
		pUser->SendMessage( kPacket );

		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "잘못된 대회명 전송됨(1) : %s", pUser->GetPublicID().c_str() );
		return false;
	}
	else if( g_App.IsNotMakeID( kTourName.c_str() ) )
	{
		//잘못된 아이디
		SP2Packet kPacket( STPK_TOURNAMENT_CUSTOM_CREATE );
		PACKET_GUARD_bool( kPacket.Write(TOURNAMENT_CUSTOM_CREATE_NAME_FAILED) );
		pUser->SendMessage( kPacket );

		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "잘못된 대회명 전송됨(2) : %s", pUser->GetPublicID().c_str() );
		return false;
	}

	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( pLocal && !pLocal->IsRightNewID( kTourName.c_str() ) )
	{
		//잘못된 아이디
		SP2Packet kPacket( STPK_TOURNAMENT_CUSTOM_CREATE );
		PACKET_GUARD_bool( kPacket.Write(TOURNAMENT_CUSTOM_CREATE_NAME_FAILED) );
		pUser->SendMessage( kPacket );

		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "잘못된 대회명 전송됨(3) : %s", pUser->GetPublicID().c_str() );
		return false;
	}

	g_DBClient.OnInsertTournamentCustomAdd( pUser->GetUserDBAgentID(), pUser->GetAgentThreadID(), pUser->GetUserIndex(), pUser->GetPublicID(), GetType(), dwStartDate, dwEndDate,
											TournamentManager::TYPE_CUSTOM, TournamentManager::STATE_TEAM_APP, kTourName, MaxRound, dwBannerLargeIndex, dwBannerSmallIndex, iModeBattleType,
											MaxPlayer, RoundType, dwAppDate, dwDelayDate, dwRoundDate1, dwRoundDate2, dwRoundDate3, dwRoundDate4, dwRoundDate5, dwRoundDate6, dwRoundDate7,
											dwRoundDate8, dwRoundDate9, dwRoundDate10 );

	// 특별아이템 처리 - DB에서 실패한다면 해당 특별 아이템을 선물로 지급한다.
	pUserEtcItem->DeleteEtcItem( GetType(), LogDBClient::ET_DEL );
	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "Use Etc Item (%s/%d/%d) (%s : %d ~ %d/%d/%d/%d/%d/%d/%d)", pUser->GetPublicID().c_str(), rkSlot.m_iType, rkSlot.m_iValue1,
										 kTourName.c_str(), dwStartDate, dwEndDate, (int)MaxRound, iModeBattleType, (int)MaxPlayer, (int)RoundType, dwBannerLargeIndex, dwBannerSmallIndex );

	return true; 
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemTournamentPremiumCreate::ioEtcItemTournamentPremiumCreate()
{

}

ioEtcItemTournamentPremiumCreate::~ioEtcItemTournamentPremiumCreate()
{

}


void ioEtcItemTournamentPremiumCreate::OnBuy( User *pUser, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	rkSlot.SetUse( false );
}

bool ioEtcItemTournamentPremiumCreate::_OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	// OnUse에서 체크하므로 pUser 와 pUserEtcItem NULL이 아님을 보장
	if( pUser->IsReserveServerMoving() )
	{
		SP2Packet kReturn( STPK_ETCITEM_USE );
		PACKET_GUARD_bool( kReturn.Write(ETCITEM_USE_MOVING_SERVER) );
		pUser->SendMessage( kReturn );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Fail Reserve Server moving - %s) %d.",  __FUNCTION__, pUser->GetPublicID().c_str(), rkSlot.m_iType );
		return false;
	}

	// 유저 대회 데이터 분석 - 커스텀 배너 가능
	
	return true; 
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemClover::ioEtcItemClover()
{
}

ioEtcItemClover::~ioEtcItemClover()
{
}

bool ioEtcItemClover::_OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "호출되서는 안되는 함수 호출됨 !! Use Etc Item (%s/%d/%d)", pUser->GetPublicID().c_str(), rkSlot.m_iType, rkSlot.m_iValue1 );
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemAddCash::ioEtcItemAddCash()
{
	m_iGetCashNum = 0;
}

ioEtcItemAddCash::~ioEtcItemAddCash()
{

}

void ioEtcItemAddCash::LoadProperty( ioINILoader &rkLoader )
{
	ioEtcItem::LoadProperty( rkLoader );
	m_iGetCashNum = rkLoader.LoadInt( "get_cash_num", 0 );
}

bool ioEtcItemAddCash::_OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	// OnUse에서 체크하므로 pUser 와 pUserEtcItem NULL이 아님을 보장
	ioChannelingNodeParent *pNode = g_ChannelingMgr.GetNode( pUser->GetChannelingType() );
	if( !pNode )
	{
		SP2Packet kReturn( STPK_ETCITEM_USE );
		PACKET_GUARD_bool( kReturn.Write(ETCITEM_USE_EXCEPTION) );
		pUser->SendMessage( kReturn );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pNode == NULL - %s) %d.",  __FUNCTION__, pUser->GetPublicID().c_str(), rkSlot.m_iType );
		return false;
	}

	pNode->SendAddCash( pUser, m_iGetCashNum, m_dwType );

	return true;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemTournamentCoin::ioEtcItemTournamentCoin()
{
}

ioEtcItemTournamentCoin::~ioEtcItemTournamentCoin()
{

}

bool ioEtcItemTournamentCoin::_OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "호출되서는 안되는 함수 호출됨 !! Use Etc Item (%s/%d/%d)", pUser->GetPublicID().c_str(), rkSlot.m_iType, rkSlot.m_iValue1 );
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemRouletteCoin::ioEtcItemRouletteCoin()
{
}

ioEtcItemRouletteCoin::~ioEtcItemRouletteCoin()
{
}

bool ioEtcItemRouletteCoin::_OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "호출되서는 안되는 함수 호출됨 !! Use Etc Item (%s/%d/%d)", pUser->GetPublicID().c_str(), rkSlot.m_iType, rkSlot.m_iValue1 );
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemBingoItem::ioEtcItemBingoItem()
{
}

ioEtcItemBingoItem::~ioEtcItemBingoItem()
{
}

bool ioEtcItemBingoItem::_OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "호출되서는 안되는 함수 호출됨 !! Use Etc Item (%s/%d/%d)", pUser->GetPublicID().c_str(), rkSlot.m_iType, rkSlot.m_iValue1 );
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemBingoNumberGashapon::ioEtcItemBingoNumberGashapon()
{
}

ioEtcItemBingoNumberGashapon::~ioEtcItemBingoNumberGashapon()
{
}

bool ioEtcItemBingoNumberGashapon::_OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	// table값 가져오고
	int iMaxNumber = g_BingoMgr.GetBingoMaxNumber();

	// rand
	int resultValue = rand() % iMaxNumber + 1;

	// Get : Bingo
	ioBingo* pBingo = pUser->GetBingo();

	// Choice Number
	pBingo->ChoiceNumber( resultValue );

	rkSlot.AddUse( -1 );
	if( rkSlot.GetUse() <= 0 )
	{
		pUserEtcItem->DeleteEtcItem( rkSlot.m_iType, LogDBClient::ET_DEL );
	}
	else
	{
		pUserEtcItem->SetEtcItem( rkSlot, LogDBClient::ET_USE );
	}

	// Send
	SP2Packet kReturn( STPK_ETCITEM_USE );
	PACKET_GUARD_bool( kReturn.Write(ETCITEM_USE_OK) );
	PACKET_GUARD_bool( kReturn.Write(rkSlot.m_iType) );
	PACKET_GUARD_bool( kReturn.Write(rkSlot.m_iValue1) );
	PACKET_GUARD_bool( kReturn.Write(rkSlot.m_iValue2) );
	PACKET_GUARD_bool( kReturn.Write(static_cast< BYTE >( resultValue )) ); // 숫자 한개.

	pUser->SendMessage( kReturn );

	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemBingoShuffleNumber::ioEtcItemBingoShuffleNumber()
{
}

ioEtcItemBingoShuffleNumber::~ioEtcItemBingoShuffleNumber()
{
}

bool ioEtcItemBingoShuffleNumber::_OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	// Get : Bingo
	ioBingo* pBingo = pUser->GetBingo();

	// Random Shuffle
	if( !pBingo->RandomShuffleNumber() )
		return false;

	rkSlot.AddUse( -1 );
	if( rkSlot.GetUse() <= 0 )
	{
		pUserEtcItem->DeleteEtcItem( rkSlot.m_iType, LogDBClient::ET_DEL );
	}
	else
	{
		pUserEtcItem->SetEtcItem( rkSlot, LogDBClient::ET_USE );
	}

	// Send
	SP2Packet kReturn( STPK_ETCITEM_USE );
	PACKET_GUARD_bool( kReturn.Write(ETCITEM_USE_OK) );
	PACKET_GUARD_bool( kReturn.Write(rkSlot.m_iType) );
	PACKET_GUARD_bool( kReturn.Write(rkSlot.m_iValue1) );
	PACKET_GUARD_bool( kReturn.Write(rkSlot.m_iValue2) );

	// Declare) array Number
	BYTE arrayNumber[ ioBingo::MAX ][ ioBingo::MAX ] = { 0, };

	// Get : Number
	pBingo->GetBingoNumberData( arrayNumber );

	// Set Number
	for( int i = 0 ; i < ioBingo::MAX ; ++i )
	{
		for( int j = 0 ; j < ioBingo::MAX ; ++j )
		{
			PACKET_GUARD_bool( kReturn.Write(arrayNumber[ i ][ j ]) );
		}
	}

	pUser->SendMessage( kReturn );

	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemBingoShuffleRewardItem::ioEtcItemBingoShuffleRewardItem()
{
}

ioEtcItemBingoShuffleRewardItem::~ioEtcItemBingoShuffleRewardItem()
{
}

bool ioEtcItemBingoShuffleRewardItem::_OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	// Get : Bingo
	ioBingo* pBingo = pUser->GetBingo();

	// Choice Number
	pBingo->RandomShufflePresent();

	rkSlot.AddUse( -1 );
	if( rkSlot.GetUse() <= 0 )
	{
		pUserEtcItem->DeleteEtcItem( rkSlot.m_iType, LogDBClient::ET_DEL );
	}
	else
	{
		pUserEtcItem->SetEtcItem( rkSlot, LogDBClient::ET_USE );
	}

	// Declare) array Present
	BYTE arrayPresent[ ioBingo::PRESENT_COUNT ] = { 0, };

	// Get : Present
	pBingo->GetBingoPresentData( arrayPresent );
	

	// Send
	SP2Packet kReturn( STPK_ETCITEM_USE );
	PACKET_GUARD_bool( kReturn.Write(ETCITEM_USE_OK) );
	PACKET_GUARD_bool( kReturn.Write(rkSlot.m_iType) );
	PACKET_GUARD_bool( kReturn.Write(rkSlot.m_iValue1) );
	PACKET_GUARD_bool( kReturn.Write(rkSlot.m_iValue2) );

	for( int i = 0 ; i < ioBingo::PRESENT_COUNT ; ++i )
	{
		PACKET_GUARD_bool( kReturn.Write(arrayPresent[ i ]) );
	}

	pUser->SendMessage( kReturn );

	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemBingoRandomNumberClear::ioEtcItemBingoRandomNumberClear()
{
}

ioEtcItemBingoRandomNumberClear::~ioEtcItemBingoRandomNumberClear()
{
}

bool ioEtcItemBingoRandomNumberClear::_OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	// Get : Bingo
	ioBingo* pBingo = pUser->GetBingo();

	// Choice Number
	int resultValue = pBingo->FreeChoiceNumber();

	rkSlot.AddUse( -1 );
	if( rkSlot.GetUse() <= 0 )
	{
		pUserEtcItem->DeleteEtcItem( rkSlot.m_iType, LogDBClient::ET_DEL );
	}
	else
	{
		pUserEtcItem->SetEtcItem( rkSlot, LogDBClient::ET_USE );
	}

	// Send
	SP2Packet kReturn( STPK_ETCITEM_USE );
	PACKET_GUARD_bool( kReturn.Write(ETCITEM_USE_OK) );
	PACKET_GUARD_bool( kReturn.Write(rkSlot.m_iType) );
	PACKET_GUARD_bool( kReturn.Write(rkSlot.m_iValue1) );
	PACKET_GUARD_bool( kReturn.Write(rkSlot.m_iValue2) );
	PACKET_GUARD_bool( kReturn.Write(static_cast< BYTE >( resultValue )) );		// 숫자 한개.

	pUser->SendMessage( kReturn );

	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemSuperGashapon::ioEtcItemSuperGashapon()
{

}

ioEtcItemSuperGashapon::~ioEtcItemSuperGashapon()
{

}

void ioEtcItemSuperGashapon::OnBuy( User *pUser, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	rkSlot.SetUse( false );
}

bool ioEtcItemSuperGashapon::_OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	// OnUse에서 체크하므로 pUser 와 pUserEtcItem NULL이 아님을 보장
	if( pUser->IsReserveServerMoving() )
	{
		SP2Packet kReturn( STPK_ETCITEM_USE );
		PACKET_GUARD_bool( kReturn.Write(ETCITEM_USE_MOVING_SERVER) );
		pUser->SendMessage( kReturn );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Fail Reserve Server moving - %s) %d.", __FUNCTION__, pUser->GetPublicID().c_str(), rkSlot.m_iType );
		return false;
	}

	int iUseType = 0;
	PACKET_GUARD_bool( rkPacket.Read(iUseType) );

	if( g_SuperGashaponMgr.IsLimitGashapon( GetType() ) )
	{
		if( !g_SuperGashaponMgr.SendSuperGashponLimitCheck( pUser, GetType(), iUseType ) )
		{
			SP2Packet kReturn( STPK_ETCITEM_USE );
			PACKET_GUARD_bool( kReturn.Write(ETCITEM_USE_EXCEPTION) );
			pUser->SendMessage( kReturn );
		}
	}
	else
	{
		rkSlot.AddUse( -1 );
		if( rkSlot.GetUse() <= 0 )
		{
			pUserEtcItem->DeleteEtcItem( rkSlot.m_iType, LogDBClient::ET_DEL );
		}
		else
		{
			pUserEtcItem->SetEtcItem( rkSlot );
		}

		DWORD dwPackageIndex = 0;
		bool bPackageWholeAlarm = false;
		if( g_SuperGashaponMgr.SendSuperGashaponRandPackage( pUser, GetType(), dwPackageIndex, bPackageWholeAlarm ) )
		{
			SP2Packet kReturn( STPK_ETCITEM_USE );
			PACKET_GUARD_bool( kReturn.Write(ETCITEM_USE_OK) );
			PACKET_GUARD_bool( kReturn.Write(rkSlot.m_iType) );
			PACKET_GUARD_bool( kReturn.Write(rkSlot.m_iValue1) );
			PACKET_GUARD_bool( kReturn.Write(rkSlot.m_iValue2) );
			PACKET_GUARD_bool( kReturn.Write((int)SUPER_GASHPON_MAIN) );
			PACKET_GUARD_bool( kReturn.Write(dwPackageIndex) );
			PACKET_GUARD_bool( kReturn.Write(iUseType) );

			pUser->SendMessage( kReturn );
			pUser->SendPresentMemory();  // 메모리 선물 전송

			// super gashapon(etc item) 사용 관련 전서버 알림
			g_UserNodeManager.AlarmEventInSuperGashaponPresentToAllUsers(pUser, bPackageWholeAlarm, (int)SUPER_GASHPON_MAIN, this->GetType(), dwPackageIndex);
		}
		else
		{
			SP2Packet kReturn( STPK_ETCITEM_USE );
			PACKET_GUARD_bool( kReturn.Write(ETCITEM_USE_EXCEPTION) );
			pUser->SendMessage( kReturn );
		}
	}

	return false; // false 면 OnUse() 여기서 중단
}


#ifdef OHTG_NEW_RANROM_BOX_20201109
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemRandomBox::ioEtcItemRandomBox()
{

}

ioEtcItemRandomBox::~ioEtcItemRandomBox()
{

}

void ioEtcItemRandomBox::OnBuy( User *pUser, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	rkSlot.SetUse( false );
}

bool ioEtcItemRandomBox::_OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	// OnUse에서 체크하므로 pUser 와 pUserEtcItem NULL이 아님을 보장
	if( pUser->IsReserveServerMoving() )
	{
		SP2Packet kReturn( STPK_ETCITEM_USE );
		PACKET_GUARD_bool( kReturn.Write(ETCITEM_USE_MOVING_SERVER) );
		pUser->SendMessage( kReturn );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Fail Reserve Server moving - %s) %d.", __FUNCTION__, pUser->GetPublicID().c_str(), rkSlot.m_iType );
		return false;
	}

	int iUseType = 0;
	PACKET_GUARD_bool( rkPacket.Read(iUseType) );

	DWORD dwPackageIndex = 0;
	bool bPackageWholeAlarm = false;
	vector<cBoxInfo> vPackageBox;
	vPackageBox.clear();
	if( g_RandomBoxManager.SendRandomBoxRandPackage( pUser, GetType(), vPackageBox) )
	{
		rkSlot.AddUse( -1 );
		if( rkSlot.GetUse() <= 0 )
		{
			pUserEtcItem->DeleteEtcItem( rkSlot.m_iType, LogDBClient::ET_DEL );
		}
		else
		{
			pUserEtcItem->SetEtcItem( rkSlot );
		}

		int iCount = vPackageBox.size();
		SP2Packet kReturn( STPK_ETCITEM_USE );
		PACKET_GUARD_bool( kReturn.Write(ETCITEM_USE_OK) );
		PACKET_GUARD_bool( kReturn.Write(rkSlot.m_iType) );
		PACKET_GUARD_bool( kReturn.Write(rkSlot.m_iValue1) );
		PACKET_GUARD_bool( kReturn.Write(rkSlot.m_iValue2) );
		PACKET_GUARD_bool( kReturn.Write((int)SUPER_GASHPON_NEW) );
		
		PACKET_GUARD_bool( kReturn.Write(iCount) );
		for(int i = 0; i < iCount; i++)
		{
			PACKET_GUARD_bool( kReturn.Write(vPackageBox[i].dwCategoryIndex) );
			PACKET_GUARD_bool( kReturn.Write(vPackageBox[i].dwPackageIndex) );
		}
		PACKET_GUARD_bool( kReturn.Write(iUseType) );

		pUser->SendMessage( kReturn );
		pUser->SendPresentMemory();  // 메모리 선물 전송

		// super gashapon(etc item) 사용 관련 전서버 알림
		for(int i = 0; i < iCount; i++)
		{
			g_UserNodeManager.AlarmEventInNewSuperGashaponPresentToAllUsers(pUser, vPackageBox[i].bPackageAlarm, (int)SUPER_GASHPON_MAIN, this->GetType(), vPackageBox[i].dwCategoryIndex, vPackageBox[i].dwPackageIndex);
		}
	}
	else
	{
		SP2Packet kReturn( STPK_ETCITEM_USE );
		PACKET_GUARD_bool( kReturn.Write(ETCITEM_USE_EXCEPTION) );
		pUser->SendMessage( kReturn );
		return false ;
	}

	return false; // false 면 OnUse() 여기서 중단
}
#endif //OHTG_NEW_RANROM_BOX_20201109


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemSendPresent::ioEtcItemSendPresent()
{

}

ioEtcItemSendPresent::~ioEtcItemSendPresent()
{

}

void ioEtcItemSendPresent::LoadProperty( ioINILoader &rkLoader )
{
	ioEtcItem::LoadProperty( rkLoader );

	char szKey[MAX_PATH] = "";

	int iCnt = rkLoader.LoadInt( "present_count", 0 );

	m_vPresentInfo.clear();
	m_vPresentInfo.reserve(( iCnt ));

	for( int i=0; i<iCnt; ++i )
	{
		PresentData kInfo;
		kInfo.Init();

		wsprintf( szKey, "present%d_type", i+1 );
		kInfo.m_iPresentType   = rkLoader.LoadInt( szKey, 0 );

		wsprintf( szKey, "present%d_state", i+1 );
		kInfo.m_iPresentState  = rkLoader.LoadInt( szKey, 0 );

		wsprintf( szKey, "present%d_ment", i+1 );
		kInfo.m_iPresentMent   = rkLoader.LoadInt( szKey, 0 );

		wsprintf( szKey, "present%d_period", i+1 );
		kInfo.m_iPresentPeriod = rkLoader.LoadInt( szKey, 0 );

		wsprintf( szKey, "present%d_value1", i+1 );
		kInfo.m_iPresentValue1 = rkLoader.LoadInt( szKey, 0 );

		wsprintf( szKey, "present%d_value2", i+1 );
		kInfo.m_iPresentValue2 = rkLoader.LoadInt( szKey, 0 );

		wsprintf( szKey, "present%d_value3", i+1 );
		kInfo.m_iPresentValue3 = rkLoader.LoadInt( szKey, 0 );

		wsprintf( szKey, "present%d_value4", i+1 );
		kInfo.m_iPresentValue4 = rkLoader.LoadInt( szKey, 0 );

		m_vPresentInfo.push_back(kInfo);
	}
}

bool ioEtcItemSendPresent::_OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	DWORD dwFriendUserIndex;
	ioHashString szFriendName;
	int iArray = 0;

	PACKET_GUARD_bool( rkPacket.Read(dwFriendUserIndex) );
	PACKET_GUARD_bool( rkPacket.Read(szFriendName) );
	PACKET_GUARD_bool( rkPacket.Read(iArray) );

	if( iArray == -1 ||
		iArray >= (int)m_vPresentInfo.size() )
	{
		SP2Packet kPacket( STPK_ETC_ITEM_SEND_PRESENT_FAIL );
		PACKET_GUARD_bool( kPacket.Write(INCORRECT_ITEM) );
		pUser->SendMessage( kPacket );
		return false;
	}

	if( szFriendName.IsEmpty() ||
		szFriendName == pUser->GetPublicID() ||
		g_UserNodeManager.IsDeveloper( szFriendName.c_str() ) )
	{
		SP2Packet kPacket( STPK_ETC_ITEM_SEND_PRESENT_FAIL );
		PACKET_GUARD_bool( kPacket.Write(INCORRECT_USER) );
		pUser->SendMessage( kPacket );
		return false;
	}

	// OnUse에서 체크하므로 pUser 와 pUserEtcItem NULL이 아님을 보장
	rkSlot.AddUse( -1 );
	if( rkSlot.GetUse() <= 0 )
	{
		pUserEtcItem->DeleteEtcItem( rkSlot.m_iType, LogDBClient::ET_DEL );
	}
	else
	{
		pUserEtcItem->SetEtcItem( rkSlot );
	}
	
	PresentData kPresentData = m_vPresentInfo[iArray];
	UserParent* pFriendParent = g_UserNodeManager.GetGlobalUserNode( dwFriendUserIndex );
	if( pFriendParent )
	{
		if( pFriendParent->IsUserOriginal() )
		{
			User* pFriend = dynamic_cast< User* >( pFriendParent );
			if( pFriend )
			{
				CTimeSpan cPresentGapTime( kPresentData.m_iPresentPeriod, 0, 0, 0 );
				CTime kPresentTime = CTime::GetCurrentTime() + cPresentGapTime;
				pFriend->AddPresentMemory( pUser->GetPublicID(), kPresentData.m_iPresentType,
										   kPresentData.m_iPresentValue1, kPresentData.m_iPresentValue2, kPresentData.m_iPresentValue3, kPresentData.m_iPresentValue4,
										   kPresentData.m_iPresentMent, kPresentTime, kPresentData.m_iPresentState );
				g_LogDBClient.OnInsertPresent( pUser->GetUserIndex(), pUser->GetPublicID(), g_App.GetPublicIP().c_str(), pFriend->GetUserIndex(), kPresentData.m_iPresentType,
											   kPresentData.m_iPresentValue1, kPresentData.m_iPresentValue2, kPresentData.m_iPresentValue3, kPresentData.m_iPresentValue4,
											   LogDBClient::PST_RECIEVE, "UsePresentItem" );
				pFriend->SendPresentMemory();
			}
		}
		else
		{
			UserCopyNode *pCopyFriend = dynamic_cast< UserCopyNode* >( pFriendParent );
			if( pCopyFriend )
			{
				ioHashString szLogMent = "UsePresentItem";
				SP2Packet kPacket( SSTPK_ETC_ITEM_SEND_PRESENT );
				PACKET_GUARD_bool( kPacket.Write(pCopyFriend->GetUserIndex()) );
				PACKET_GUARD_bool( kPacket.Write(pUser->GetUserIndex()) );
				PACKET_GUARD_bool( kPacket.Write(pUser->GetPublicID()) );
				PACKET_GUARD_bool( kPacket.Write(g_App.GetPublicIP()) );
				PACKET_GUARD_bool( kPacket.Write(kPresentData.m_iPresentPeriod) );
				PACKET_GUARD_bool( kPacket.Write(kPresentData.m_iPresentType) );
				PACKET_GUARD_bool( kPacket.Write(kPresentData.m_iPresentValue1) );
				PACKET_GUARD_bool( kPacket.Write(kPresentData.m_iPresentValue2) );
				PACKET_GUARD_bool( kPacket.Write(kPresentData.m_iPresentValue3) );
				PACKET_GUARD_bool( kPacket.Write(kPresentData.m_iPresentValue4) );
				PACKET_GUARD_bool( kPacket.Write(kPresentData.m_iPresentMent) );
				PACKET_GUARD_bool( kPacket.Write(kPresentData.m_iPresentState) );
				PACKET_GUARD_bool( kPacket.Write(szLogMent) );

				pCopyFriend->SendMessage( kPacket );
			}
		}
	}
	else
	{
		// 친구가 로그아웃상태라면 DB로..
		CTimeSpan cPresentGapTime( kPresentData.m_iPresentPeriod, 0, 0, 0 );
		CTime kPresentTime = CTime::GetCurrentTime() + cPresentGapTime;
		g_DBClient.OnInsertPresentData( pUser->GetUserDBAgentID(), pUser->GetAgentThreadID(), pUser->GetPublicID(), szFriendName, kPresentData.m_iPresentType, 
										kPresentData.m_iPresentValue1, kPresentData.m_iPresentValue2, kPresentData.m_iPresentValue3, kPresentData.m_iPresentValue4,
										kPresentData.m_iPresentMent, kPresentTime, kPresentData.m_iPresentState );

		g_LogDBClient.OnInsertPresent( pUser->GetUserIndex(), pUser->GetPublicID(), g_App.GetPublicIP().c_str(), dwFriendUserIndex, kPresentData.m_iPresentType,
									   kPresentData.m_iPresentValue1, kPresentData.m_iPresentValue2, kPresentData.m_iPresentValue3, kPresentData.m_iPresentValue4,
									   LogDBClient::PST_RECIEVE, "UsePresentItem" );
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
ioEtcItemSoldierExpAdd::ioEtcItemSoldierExpAdd()
{
	m_iClassType = 0;
	m_iAddExp	 = 0;
}

ioEtcItemSoldierExpAdd::~ioEtcItemSoldierExpAdd()
{
}

void ioEtcItemSoldierExpAdd::LoadProperty( ioINILoader &rkLoader )
{
	ioEtcItem::LoadProperty( rkLoader );

	m_iClassType  = rkLoader.LoadInt( "class_type", 0 );
	m_iAddExp	  = rkLoader.LoadInt( "add_exp", 0 );
	
}

bool ioEtcItemSoldierExpAdd::_OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	// OnUse에서 체크하므로 pUser 와 pUserEtcItem NULL이 아님을 보장
	if( pUser->IsReserveServerMoving() )
	{
		SP2Packet kReturn( STPK_ETCITEM_USE );
		PACKET_GUARD_bool( kReturn.Write(ETCITEM_USE_MOVING_SERVER) );
		pUser->SendMessage( kReturn );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Fail Reserve Server moving - %s) %d.", __FUNCTION__, pUser->GetPublicID().c_str(), rkSlot.m_iType );
		return false;
	}

	if( !pUser->IsClassType( m_iClassType ) )
	{
		SP2Packet kReturn( STPK_ETCITEM_USE );
		PACKET_GUARD_bool( kReturn.Write(ETCITEM_USE_CHAR_NONE) );
		pUser->SendMessage( kReturn );
		return false;
	}

	if( pUser->IsClassTypeExerciseStyle( m_iClassType, EXERCISE_RENTAL ) )
	{
		SP2Packet kReturn( STPK_ETCITEM_USE );
		PACKET_GUARD_bool( kReturn.Write(ETCITEM_USE_CHAR_RENTAL) );
		pUser->SendMessage( kReturn );
		return false;
	}
		
	if( pUser->GetClassLevelByType( m_iClassType, true ) >= 100 )
	{
		SP2Packet kReturn( STPK_ETCITEM_USE );
		PACKET_GUARD_bool( kReturn.Write(ETCITEM_USE_CHAR_LIMIT_LEVEL) );
		pUser->SendMessage( kReturn );
		return false;
	}	

	pUser->AddClassExp( m_iClassType, max( 0, m_iAddExp ) );
	pUser->GradeNClassUPBonus();
	pUserEtcItem->DeleteEtcItem( rkSlot.m_iType, LogDBClient::ET_DEL );

	SP2Packet kReturn( STPK_ETCITEM_USE );
	PACKET_GUARD_bool( kReturn.Write(ETCITEM_USE_OK) );
	PACKET_GUARD_bool( kReturn.Write(rkSlot.m_iType) );
	PACKET_GUARD_bool( kReturn.Write(m_iClassType) );
	PACKET_GUARD_bool( kReturn.Write(m_iAddExp) );
	pUser->SendMessage( kReturn );

	if( pUser->GetSelectClassType() == m_iClassType && pUser->GetModeType() != MT_NONE )
	{
		if( pUser->GetMyRoom() )
		{
			SP2Packet kPacket( STPK_SOLDIER_EXP_ADD );
			PACKET_GUARD_bool( kPacket.Write(pUser->GetPublicID()) );
			PACKET_GUARD_bool( kPacket.Write(m_iClassType) );
			PACKET_GUARD_bool( kPacket.Write(pUser->GetClassLevel( pUser->GetSelectChar(), true )) );
			pUser->GetMyRoom()->RoomSendPacketTcp( kPacket );
		}
		else
		{
			SP2Packet kReturn( STPK_ETCITEM_USE );
			PACKET_GUARD_bool( kReturn.Write(ETCITEM_USE_EXCEPTION) );
			pUser->SendMessage( kReturn );
		}
	}

	return false; // false 면 OnUse() 여기서 중단
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemRecharge::ioEtcItemRecharge()
{
	m_Random.Randomize();
}

ioEtcItemRecharge::~ioEtcItemRecharge()
{

}

void ioEtcItemRecharge::LoadProperty( ioINILoader &rkLoader )
{
	ioEtcItem::LoadProperty( rkLoader );

	char szKey[MAX_PATH] = "";
	m_iMaxInfoCnt = rkLoader.LoadInt( "recharge_count", 0 );

	m_RechargeInfoList.clear();
	m_RechargeInfoList.reserve( m_iMaxInfoCnt );

	for( int i=0; i<m_iMaxInfoCnt; ++i )
	{
		wsprintf( szKey, "recharge_time%d", i+1 );
		int iTime = rkLoader.LoadInt( szKey, -1 );

		wsprintf( szKey, "recharge_rate%d", i+1 );
		float fRate = rkLoader.LoadFloat( szKey, 0.0f );

		float fMinRate, fMaxRate;

		if( i == 0 )
		{
			fMinRate = 0.0f;
			fMaxRate = fRate;
		}
		else
		{
			fMinRate = m_RechargeInfoList[i-1].m_fMaxRate;
			fMaxRate = m_RechargeInfoList[i-1].m_fMaxRate + fRate;
		}

		RechargeInfo kInfo( iTime, fMinRate, fMaxRate );
		m_RechargeInfoList.push_back( kInfo );
	}
}

bool ioEtcItemRecharge::_OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	// OnUse에서 체크하므로 pUser 와 pUserEtcItem NULL이 아님을 보장
	rkSlot.AddUse( -1 );
	if( rkSlot.GetUse() <= 0 )
	{
		pUserEtcItem->DeleteEtcItem( rkSlot.m_iType, LogDBClient::ET_DEL );
	}
	else
	{
		pUserEtcItem->SetEtcItem( rkSlot );
	}

	int iSlotIndex = 0, iItemCode = 0;

	PACKET_GUARD_bool( rkPacket.Read(iSlotIndex) );
	PACKET_GUARD_bool( rkPacket.Read(iItemCode) );

	if( g_ItemRechargeMgr.SetRechargeExtraItem( pUser, iSlotIndex, iItemCode, GetRechargeTime(), m_dwType ) )
	{
		return true;
	}

	return false;
}

int ioEtcItemRecharge::GetRechargeTime()
{
	int iResult = m_Random.Random( 10000 );
	float fRate = static_cast<float>(iResult) / 100.0f;

	int iCnt = m_RechargeInfoList.size();
	for( int i=0; i<iCnt; ++i )
	{
		if( COMPARE( fRate, m_RechargeInfoList[i].m_fMinRate, m_RechargeInfoList[i].m_fMaxRate ) )
			return m_RechargeInfoList[i].m_iPeriod;
	}

	return 0;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcAccessoryRecharge::ioEtcAccessoryRecharge()
{
	m_Random.Randomize();
}

ioEtcAccessoryRecharge::~ioEtcAccessoryRecharge()
{

}

void ioEtcAccessoryRecharge::LoadProperty( ioINILoader &rkLoader )
{
	ioEtcItem::LoadProperty( rkLoader );

	char szKey[MAX_PATH] = "";
	m_iMaxInfoCnt = rkLoader.LoadInt( "recharge_count", 0 );

	m_RechargeInfoList.clear();
	m_RechargeInfoList.reserve( m_iMaxInfoCnt );

	for( int i=0; i<m_iMaxInfoCnt; ++i )
	{
		wsprintf( szKey, "recharge_time%d", i+1 );
		int iTime = rkLoader.LoadInt( szKey, -1 );

		wsprintf( szKey, "recharge_rate%d", i+1 );
		float fRate = rkLoader.LoadFloat( szKey, 0.0f );

		float fMinRate, fMaxRate;

		if( i == 0 )
		{
			fMinRate = 0.0f;
			fMaxRate = fRate;
		}
		else
		{
			fMinRate = m_RechargeInfoList[i-1].m_fMaxRate;
			fMaxRate = m_RechargeInfoList[i-1].m_fMaxRate + fRate;
		}

		RechargeInfo kInfo( iTime, fMinRate, fMaxRate );
		m_RechargeInfoList.push_back( kInfo );
	}
}

bool ioEtcAccessoryRecharge::_OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	// OnUse에서 체크하므로 pUser 와 pUserEtcItem NULL이 아님을 보장
	rkSlot.AddUse( -1 );
	if( rkSlot.GetUse() <= 0 )
	{
		pUserEtcItem->DeleteEtcItem( rkSlot.m_iType, LogDBClient::ET_DEL );
	}
	else
	{
		pUserEtcItem->SetEtcItem( rkSlot );
	}

	int iSlotIndex = 0, iItemCode = 0;

	PACKET_GUARD_bool( rkPacket.Read(iSlotIndex) );
	PACKET_GUARD_bool( rkPacket.Read(iItemCode) );

	if( g_AccRechargeMgr.SetRechargeAccessory( pUser, iSlotIndex, iItemCode, GetRechargeTime(), m_dwType ) )
	{
		return true;
	}

	return false;
}

int ioEtcAccessoryRecharge::GetRechargeTime()
{
	int iResult = m_Random.Random( 10000 );
	float fRate = static_cast<float>(iResult) / 100.0f;

	int iCnt = m_RechargeInfoList.size();
	for( int i=0; i<iCnt; ++i )
	{
		if( COMPARE( fRate, m_RechargeInfoList[i].m_fMinRate, m_RechargeInfoList[i].m_fMaxRate ) )
			return m_RechargeInfoList[i].m_iPeriod;
	}

	return 0;
}

///////////////////////////////////////////////////////////////////////////
ioSelectGashapon::ioSelectGashapon()
{
}

ioSelectGashapon::~ioSelectGashapon()
{
}

void ioSelectGashapon::LoadProperty( ioINILoader &rkLoader )
{
	ioEtcItem::LoadProperty( rkLoader );

}

void ioSelectGashapon::OnAfterBuy( User *pUser, SP2Packet &rkPacket, int iArray, int iTransactionID, int bUseGoldCash )
{
	if( !pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pUser == NULL.", __FUNCTION__ );
		return;
	}

	const SelectGashaponValueVec& vSelect = pUser->GetSelectGashapon();
	int iSelectCount = vSelect.size();

	//선택한 가챠가 없는 경우
	if( iSelectCount <= 0 )
	{
		SP2Packet kPacket( STPK_BUY_SELECT_GASHAPON );
		PACKET_GUARD_VOID( kPacket.Write(BUY_SELECT_GASHAPON_EXCEPTION) );
		pUser->SendMessage( kPacket );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s select item empty.(%d:%s)", __FUNCTION__, pUser->GetUserIndex(), pUser->GetPublicID().c_str() );
		return;
	}

	if( iSelectCount-1 != iArray )
	{
		SP2Packet kPacket( STPK_BUY_SELECT_GASHAPON );
		PACKET_GUARD_VOID( kPacket.Write(BUY_SELECT_GASHAPON_EXCEPTION) );
		pUser->SendMessage( kPacket );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s iArray wroing size.(%d:%s)", __FUNCTION__, pUser->GetUserIndex(), pUser->GetPublicID().c_str() );
		return;
	}

	short m_iType = 0;
	int m_iValue1 = 0;
	int m_iValue2 = 0;
	int iPayAmt = GetCash( iArray );
	
	if( g_PresentHelper.SendRandPresentBySelectGashapon( pUser, GetType(), vSelect, STPK_BUY_SELECT_GASHAPON, BUY_SELECT_GASHAPON_EXCEPTION, m_iType, m_iValue1, m_iValue2 ) )
	{
		ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
		if( OnAfterBuyProcessCash( pUser, pLocal, rkPacket, iArray, iTransactionID, m_iType, m_iValue1, m_iValue2, bUseGoldCash ) )
		{
			//패킷을 보내자
			SP2Packet kReturn( STPK_BUY_SELECT_GASHAPON );
			PACKET_GUARD_VOID( kReturn.Write(BUY_SELECT_GASHAPON_OK) );
			PACKET_GUARD_VOID( kReturn.Write(m_iType) );
			PACKET_GUARD_VOID( kReturn.Write(m_iValue1) );
			PACKET_GUARD_VOID( kReturn.Write(m_iValue2) );
			PACKET_GUARD_VOID( kReturn.Write(pUser->GetMoney()) );
			PACKET_GUARD_VOID( kReturn.Write(pUser->GetCash()) );
			PACKET_GUARD_VOID( kReturn.Write(pUser->GetChannelingCash()) );

			pUser->SendMessage( kReturn );
			pUser->SaveUserData();
			pUser->SendPresentMemory();

			if( pLocal )
				pLocal->SendRefundCash( pUser, iTransactionID, false );

			
		}
	}
}

bool ioSelectGashapon::OnAfterBuyProcessCash( User *pUser, ioLocalParent *pLocal, SP2Packet &rkPacket, int iArray, int iTransactionID, int iType, int iValue1, int iValue2, int bUseGoldCash )
{
	if( !pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pUser == NULL.", __FUNCTION__ );
		return false;
	}

	int iPayAmt = GetCash( iArray );
	int iPreCash = pUser->GetCash();
	int iPreChannelingCash = pUser->GetChannelingCash();
	
	ioChannelingNodeParent *pNode = g_ChannelingMgr.GetNode( pUser->GetChannelingType() );
	if( !pNode )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pNode == NULL. : %s:%s:%d", __FUNCTION__, pUser->GetPublicID().c_str(), pUser->GetBillingGUID().c_str(), iPayAmt );
		SP2Packet kReturn( STPK_BUY_SELECT_EXTRA_GASHAPON );
		PACKET_GUARD_bool( kReturn.Write(BUY_SELECT_EXTRA_GASHAPON_EXCEPTION) );
		pUser->SendMessage( kReturn );

		if( pLocal )
			pLocal->SendRefundCash( pUser, iTransactionID, true );

		return false;
	}

	if( pLocal && !pLocal->UpdateOutputCash( pUser, rkPacket, iPayAmt, STPK_BUY_SELECT_GASHAPON, BUY_SELECT_GASHAPON_EXCEPTION ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s UpdateOutputCash failed : %s:%s:%d", __FUNCTION__, pUser->GetPublicID().c_str(), pUser->GetBillingGUID().c_str(), iPayAmt );
		return false;		
	}


	/******************************************************************************************************/
	int iSpendBonusCash = 0;
	if ( bUseGoldCash == 1 )
	{
		//HRYOON 보너스 캐쉬 사용
		int iBonusCashSize = 0;
		
		PACKET_GUARD_bool( rkPacket.Read(iBonusCashSize) );
		for( int i = 0; i < iBonusCashSize; i++ )
		{
			int iIndex	= 0;
			int iValue	= 0;
			PACKET_GUARD_bool( rkPacket.Read(iIndex) );
			PACKET_GUARD_bool( rkPacket.Read(iValue) );

			iSpendBonusCash += iValue;

			//보너스 캐쉬 사용!
			if( !pUser->m_UserBonusCash.SpendBonusCash(iIndex, iValue, PRESENT_ETC_ITEM, iValue1, iValue2) )
			{

				SP2Packet kReturn( STPK_BUY_SELECT_GASHAPON );
				PACKET_GUARD_bool( kReturn.Write(BUY_SELECT_GASHAPON_EXCEPTION) );
				pUser->SendMessage( kReturn );

				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "User::OnEtcItemBuy Spend Bonus Cash Error - guid:%s,publicID:%s(userIndex:%d) Index:%d:money:%d %d",pUser->GetBillingGUID().c_str(), pUser->GetPublicID().c_str(), pUser->GetUserIndex(), iIndex, iValue1, iValue2 );
				return false;
			}
		}
	}
	/***********************************************************************************/


	__int64 iPreMoney = pUser->GetMoney();
	int iBonusPeso = GetBonusPeso( iArray );
	if( iBonusPeso > 0 )
	{
		pUser->AddMoney( iBonusPeso );
		g_LogDBClient.OnInsertPeso( pUser, iBonusPeso, LogDBClient::PT_BUY_CASH );
		g_LogDBClient.OnInsertGameLogInfo(LogDBClient::GLT_PESO_GAIN, pUser, 0, 0, LogDBClient::LET_ITEM, LogDBClient::PT_BUY_CASH, PRESENT_ETC_ITEM, iValue1, iBonusPeso, NULL);
	}

	if (bUseGoldCash == 1)
	{
		int iRealPayAmt = 0;
		iRealPayAmt = iPayAmt-iSpendBonusCash;

		char szItemIndex[MAX_PATH]="";
		StringCbPrintf( szItemIndex, sizeof( szItemIndex ), "select-gashapon-%u - %d, %d, %d", GetType(), iType, iValue1, iValue2 ); // db field는 1부터 이므로 +1
		if( iRealPayAmt != 0)
		{
			g_LogDBClient.OnInsertCashItem( pUser, GetType(), GetValue( iArray ), iRealPayAmt, szItemIndex, LogDBClient::CIT_ETC, pUser->GetBillingGUID().c_str() );	
		}
		

		// 구매 선물 기능
		int iServerValue = GetValue( iArray );
		static IntVec vEventTypeVec;
		vEventTypeVec.clear();
		vEventTypeVec.reserve(10);
		pUser->GetEventUserMgr().GetSameClassEventTypeVec( EVT_BUY_ITEM, vEventTypeVec );

		int iESize = vEventTypeVec.size();
		for ( int i = 0; i < iESize ; i++ )
		{
			BuyItemEventUserNode *pEventNode = static_cast< BuyItemEventUserNode* > ( pUser->GetEventUserMgr().GetEventUserNode( (EventType)vEventTypeVec[i] ) );
			if( pEventNode )
			{
				if ( bUseGoldCash == 1 )
					pEventNode->SendBuyPresent( pUser, false, ioPresentHelper::BT_ETC, m_dwType, iServerValue, true, iPayAmt, iRealPayAmt );
			}
		}
	}
	
	return true;
}

bool ioSelectGashapon::_OnUse(SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot)
{
	if( !pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[warning][etcitem]None exist user" );
		return false;
	}

	int iSelectCount	= 0;

	PACKET_GUARD_bool(rkPacket.Read(iSelectCount));
	if( iSelectCount <= 0 )
		return false;

	MAX_GUARD(iSelectCount, MAX_SELECT_EXTRA_ITEM_CODE);
	if( 0 == iSelectCount )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[warning][etcitem]Wrong gashapon select count : [%lu])", pUser->GetUserIndex() );	
		return false;
	}

	static SelectGashaponValueVec vSelectInfo;
	vSelectInfo.clear();

	for( int i = 0; i < iSelectCount; i++ )
	{
		SelectGashaponValue stValue;
		PACKET_GUARD_bool( rkPacket.Read(stValue.m_iType) );
		PACKET_GUARD_bool( rkPacket.Read(stValue.m_iValue1) );
		PACKET_GUARD_bool( rkPacket.Read(stValue.m_iValue2) );
		vSelectInfo.push_back(stValue);
	}

	int iSize = (int)vSelectInfo.size();
	for( int i = 0; i < iSize ; i++ )
	{
		for( int j = 0; j < iSize; j++ )
		{
			if( i != j && vSelectInfo[i] == vSelectInfo[j] )
			{
				SP2Packet kReturn( STPK_USE_SELECT_GASHAPON );
				PACKET_GUARD_bool(kReturn.Write(SELECT_GASHAPON_EXCEPTION));
				pUser->SendMessage( kReturn );
		
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[warning][etcitem]Select gashapon info is duplicated : [%lu] [%d]", pUser->GetUserIndex(), vSelectInfo[i] );
				return false;
			}
		}
	}

	short m_iType = 0;
	int m_iValue1 = 0;
	int m_iValue2 = 0;
	if( g_PresentHelper.SendRandPresentBySelectGashapon( pUser, GetType(), vSelectInfo, STPK_USE_SELECT_GASHAPON, SELECT_GASHAPON_EXCEPTION, m_iType, m_iValue1, m_iValue2 ) )
	{
		rkSlot.AddUse(-1);
		if( rkSlot.GetUse() <= 0 )
		{
			pUserEtcItem->DeleteEtcItem( rkSlot.m_iType, LogDBClient::ET_DEL );
		}
		else
		{
			pUserEtcItem->SetEtcItem( rkSlot, LogDBClient::ET_USE );
		}

		//패킷을 보내자
		SP2Packet kReturn( STPK_USE_SELECT_GASHAPON );
		PACKET_GUARD_bool( kReturn.Write(BESIDES_SELECT_GASHAPON) );
		PACKET_GUARD_bool( kReturn.Write(m_iType) );
		PACKET_GUARD_bool( kReturn.Write(m_iValue1) );
		PACKET_GUARD_bool( kReturn.Write(m_iValue2) );
		
		pUser->SendMessage( kReturn );
		pUser->SendPresentMemory();

		return true;
	}

	return false;

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemFixedBingoNumber::ioEtcItemFixedBingoNumber()
{
}

ioEtcItemFixedBingoNumber::~ioEtcItemFixedBingoNumber()
{
}

bool ioEtcItemFixedBingoNumber::_OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{	
	if( !pUser )
	{
		LOG.PrintTimeAndLog( 0, "%s pUser == NULL.", __FUNCTION__ );
		return false;
	}

	// Get : Bingo
	ioBingo* pBingo = pUser->GetBingo();

	int iChoiceNumber = 0;
	PACKET_GUARD_BOOL( rkPacket.Read( iChoiceNumber ) );

	if( g_BingoMgr.GetBingoMaxNumber() < iChoiceNumber )
	{
		SP2Packet kReturn( STPK_ETCITEM_USE );
		PACKET_GUARD_bool( kReturn.Write(ETCITEM_USE_EXCEPTION) );
		pUser->SendMessage( kReturn );
		LOG.PrintTimeAndLog(0, "%s - user choice value number-ragne overflow ", __FUNCTION__ );
		return false;
		
	}

	// Choice Number
	pBingo->ChoiceNumber( iChoiceNumber );

	rkSlot.AddUse( -1 );
	if( rkSlot.GetUse() <= 0 )
	{
		pUserEtcItem->DeleteEtcItem( rkSlot.m_iType, LogDBClient::ET_DEL );
	}
	else
	{
		pUserEtcItem->SetEtcItem( rkSlot, LogDBClient::ET_USE );
	}

	// Send
	SP2Packet kReturn( STPK_ETCITEM_USE );
	PACKET_GUARD_BOOL( kReturn.Write( ETCITEM_USE_OK ) );
	PACKET_GUARD_BOOL( kReturn.Write( rkSlot.m_iType ) );
	PACKET_GUARD_BOOL( kReturn.Write( rkSlot.m_iValue1 ) );
	PACKET_GUARD_BOOL( kReturn.Write( rkSlot.m_iValue2 ) );
	PACKET_GUARD_BOOL( kReturn.Write( static_cast< BYTE >( iChoiceNumber ) ) );
	pUser->SendMessage( kReturn );

	return false;
}


ioEtcItemPetEgg::ioEtcItemPetEgg()
{
	m_bCashItem = false;
}

ioEtcItemPetEgg::~ioEtcItemPetEgg()
{
}

void ioEtcItemPetEgg::LoadProperty( ioINILoader &rkLoader )
{
	ioEtcItem::LoadProperty( rkLoader );

	m_bCashItem  = rkLoader.LoadBool( "sell_cash", 0 );
}

bool ioEtcItemPetEgg::_OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	if( !pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pUser == NULL.", __FUNCTION__ );
		return false;
	}

	bool iResult = pUser->OnPetEggUse( rkPacket , m_bCashItem, rkSlot.m_iType );

	if( !iResult )
	{
		return false;
	}

	rkSlot.AddUse( -1 );
	if( rkSlot.GetUse() <= 0 )
	{
		pUserEtcItem->DeleteEtcItem( rkSlot.m_iType, LogDBClient::ET_DEL );
	}
	else
	{
		pUserEtcItem->SetEtcItem( rkSlot );
	}

	return true;
 }

ioEtcItemRainbowWholeChat::ioEtcItemRainbowWholeChat()
{
}

ioEtcItemRainbowWholeChat::~ioEtcItemRainbowWholeChat()
{
}

bool ioEtcItemRainbowWholeChat::_OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	// OnUse에서 체크하므로 pUser 와 pUserEtcItem NULL이 아님을 보장
	ioHashString szChat;
	PACKET_GUARD_bool( rkPacket.Read(szChat) );

	if( szChat.IsEmpty() )
	{
		SP2Packet kReturn( STPK_ETCITEM_USE );
		PACKET_GUARD_bool( kReturn.Write(ETCITEM_USE_EXCEPTION) );
		pUser->SendMessage( kReturn );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Fail - %s",  __FUNCTION__, pUser->GetPublicID().c_str() );
		return false;
	}

	rkSlot.AddUse( -1 );
	if( rkSlot.GetUse() <= 0 )
	{
		pUserEtcItem->DeleteEtcItem( rkSlot.m_iType, LogDBClient::ET_DEL );
	}
	else
	{
		pUserEtcItem->SetEtcItem( rkSlot );
	}

	SP2Packet kSPacket( SSTPK_RAINBOW_WHOLE_CHAT );
	PACKET_GUARD_bool( kSPacket.Write(pUser->GetPublicID()) );
	PACKET_GUARD_bool( kSPacket.Write(szChat) );
	g_ServerNodeManager.SendMessageAllNode( kSPacket );

	SP2Packet kPacket ( STPK_RAINBOW_WHOLE_CHAT  );
	PACKET_GUARD_bool( kPacket.Write(pUser->GetPublicID()) );
	PACKET_GUARD_bool( kPacket.Write(szChat) );
	g_UserNodeManager.SendMessageAll( kPacket, pUser );

	return true;
}

////////////////////////////////////////////////소울 스톤//////////////////////////////////////////
ioEtcItemSoulStone::ioEtcItemSoulStone()
{
}

ioEtcItemSoulStone::~ioEtcItemSoulStone()
{

}

bool ioEtcItemSoulStone::_OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	if( !pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pUser == NULL.", __FUNCTION__ );
		return false;
	}

	BYTE byTargetType = 0;
	DWORD dwIndex	= 0;
	int iNeedMaterialCount = 0;

	PACKET_GUARD_bool( rkPacket.Read(byTargetType) );
	PACKET_GUARD_bool( rkPacket.Read(dwIndex) );

	if( byTargetType <= PUTT_NONE || byTargetType >= PUTT_MAX )
	{
		SP2Packet kReturn( STPK_POWER_UP_INFO );
		PACKET_GUARD_bool( kReturn.Write(byTargetType) );
		PACKET_GUARD_bool( kReturn.Write(POWER_UP_EXCEPTION) );
		pUser->SendMessage( kReturn );
		return false;
	}

	//Error 체크
	int iResult = g_PowerUpMgr.TargetPowerUp(pUser, byTargetType, dwIndex, rkSlot.m_iValue1, iNeedMaterialCount );

	if( iResult != POWER_UP_SUCCESS )
	{
		SP2Packet kReturn( STPK_POWER_UP_INFO );
		PACKET_GUARD_bool( kReturn.Write(byTargetType) );
		PACKET_GUARD_bool( kReturn.Write(iResult) );
		pUser->SendMessage( kReturn );
		return false;
	}

	rkSlot.AddUse( -iNeedMaterialCount );
	if( rkSlot.GetUse() <= 0 )
	{
		pUserEtcItem->DeleteEtcItem( rkSlot.m_iType, LogDBClient::ET_DEL );
	}
	else
	{
		int iArray = 0;
		DWORD dwIndex = 0;

		pUserEtcItem->SetEtcItem( rkSlot, dwIndex, iArray );

		char szItemIndex[MAX_PATH]="";
		StringCbPrintf( szItemIndex, sizeof( szItemIndex ), "%u-%u", dwIndex, iArray+1 ); // db field는 1부터 이므로 +1
		g_LogDBClient.OnInsertEtc( pUser, rkSlot.m_iType, iNeedMaterialCount, 0, szItemIndex, LogDBClient::ET_DEL );

	}

	return true;
}

////////////////////////////////////////////////확률 상승 가챠//////////////////////////////////////////
ioRisingGashapon::ioRisingGashapon() :
m_iFocusIndex(0),
	m_iFocusCount(0)
{

}

ioRisingGashapon::~ioRisingGashapon()
{

}

void ioRisingGashapon::LoadProperty( ioINILoader &rkLoader )
{
	ioEtcItem::LoadProperty( rkLoader );
	m_iFocusIndex = rkLoader.LoadInt( "focus_item_index", 0 );
	m_iFocusCount = rkLoader.LoadInt( "focus_item_count", 0 );

}

void ioRisingGashapon::OnAfterBuy( User *pUser, SP2Packet &rkPacket, int iArray, int iTransactionID, int iBuyCash/*=0 */ )
{
	if( !pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pUser == NULL.", __FUNCTION__ );
		return;
	}

	short iType = 0;
	int iValue1 = 0;
	int iValue2 = 0;
	int m_iPresentIndex = 0;
	if( g_PresentHelper.SendRandPresentByRisingGashapon( pUser, GetType(), STPK_BUY_RISING_GASHAPON_RESULT, BUY_RISING_GASHAPON_OK, iType, iValue1, iValue2, m_iPresentIndex ) )
	{
		ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
		if( OnAfterBuyProcessCash( pUser, pLocal, rkPacket, iArray, iTransactionID, iType, iValue1, iValue2  ) )
		{


			//패킷을 보내자
			SP2Packet kReturn( STPK_BUY_RISING_GASHAPON_RESULT );
			PACKET_GUARD_VOID( kReturn.Write(BUY_RISING_GASHAPON_OK) );
			PACKET_GUARD_VOID( kReturn.Write(iType) );
			PACKET_GUARD_VOID( kReturn.Write(iValue1) );
			PACKET_GUARD_VOID( kReturn.Write(iValue2) );
			PACKET_GUARD_VOID( kReturn.Write(pUser->GetMoney()) );
			PACKET_GUARD_VOID( kReturn.Write(pUser->GetCash()) );
			PACKET_GUARD_VOID( kReturn.Write(pUser->GetChannelingCash()) );
			pUser->SendMessage( kReturn );

			pUser->SaveUserData();
			pUser->SendPresentMemory();

			if( pLocal )
				pLocal->SendRefundCash( pUser, iTransactionID, false );

			// 뽑은 아이템 인덱스 추가
			pUser->AddRisingGetIndex(m_iPresentIndex);

			g_LogDBClient.OnInsertGameLogInfo( LogDBClient::GLT_RISING_GASHA, pUser, 0, iValue1, LogDBClient::RGT_BUY_CASH, iType, iValue2, pUser->GetRisingBuyCount(), iBuyCash, NULL );

		}
	}

}

bool ioRisingGashapon::OnAfterBuyProcessCash( User *pUser, ioLocalParent *pLocal, SP2Packet &rkPacket, int iArray, int iTransactionID, int iType, int iValue1, int iValue2 )
{
	if( !pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pUser == NULL.", __FUNCTION__ );
		return false;
	}

	int iPayAmt = GetCash( iArray );
	int iPreCash = pUser->GetCash();
	int iPreChannelingCash = pUser->GetChannelingCash();
	ioChannelingNodeParent *pNode = g_ChannelingMgr.GetNode( pUser->GetChannelingType() );
	if( !pNode )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pNode == NULL. : %s:%s:%d", __FUNCTION__, pUser->GetPublicID().c_str(), pUser->GetBillingGUID().c_str(), iPayAmt );
		SP2Packet kReturn( STPK_BUY_RISING_GASHAPON_RESULT );
		kReturn << BUY_RISING_GASHAPON_EXCEPTION;
		pUser->SendMessage( kReturn );

		if( pLocal )
			pLocal->SendRefundCash( pUser, iTransactionID, true );

		return false;
	}

	if( pLocal && !pLocal->UpdateOutputCash( pUser, rkPacket, iPayAmt, STPK_BUY_RISING_GASHAPON_RESULT, BUY_RISING_GASHAPON_EXCEPTION ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s UpdateOutputCash failed : %s:%s:%d", __FUNCTION__, pUser->GetPublicID().c_str(), pUser->GetBillingGUID().c_str(), iPayAmt );
		return false;		
	}

	__int64 iPreMoney = pUser->GetMoney();
	int iBonusPeso = GetBonusPeso( iArray );
	if( iBonusPeso > 0 )
	{
		pUser->AddMoney( iBonusPeso );
		g_LogDBClient.OnInsertPeso( pUser, iBonusPeso, LogDBClient::PT_BUY_CASH );
	}


	char szItemIndex[MAX_PATH]="";
	StringCbPrintf( szItemIndex, sizeof( szItemIndex ), "rising-gashapon-%u count %d get cash - %d, %d, %d", GetType(), pUser->GetRisingBuyCount() + 1, iType, iValue1, iValue2); // db field는 1부터 이므로 +1
	g_LogDBClient.OnInsertEtc( pUser, GetType(), GetValue( iArray ), iPayAmt, szItemIndex, LogDBClient::ET_BUY );

	if( g_EventMgr.isMileageShopOpen() )
	{
		int imileage = 0;
		if( iPayAmt < 0)
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Fail User Rising Gasha Present : %s:%s:%d:%d[%d]", __FUNCTION__, pUser->GetPublicID().c_str(), pUser->GetBillingGUID().c_str(), iPayAmt, 0, pUser->GetUserIndex() );
			return false;
		}
		else 
		{
			if ( iPayAmt > 0 )
			{
				imileage = ( iPayAmt * g_EventMgr.GetMileageRatio() ) / 100;

				if( !g_PresentHelper.InsertUserPresent( pUser->GetUserDBAgentID(), pUser->GetAgentThreadID(), pUser->GetUserIndex(), pUser->GetPublicID(), pUser->GetPublicIP(), pUser->GetUserIndex(), PRESENT_ETC_ITEM, ioEtcItem::EIT_ETC_MILEAGE_COIN, imileage, false, true, true ) )
				{
					LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Fail User Present : %s:%s:%d:%d[%d:%d]", __FUNCTION__, pUser->GetPublicID().c_str(), pUser->GetBillingGUID().c_str(), iPayAmt, 0, pUser->GetUserIndex(), PRESENT_ETC_ITEM);
					pUser->ClearBillingGUID();
					SP2Packet kReturn( STPK_POPUP_ITEM_BUY_RESULT );
					kReturn.Write(POPUP_ITEM_BILLING_FAIL);
					pUser->SendMessage( kReturn );

					if( pLocal )
						pLocal->SendRefundCash( pUser, iTransactionID, true );
					return false;
				}
				enum { SELECT_CNT = 30, };
				pUser->_OnSelectPresent( SELECT_CNT );
			}
		}
	}

	//char szItemIndex[MAX_PATH]="";
	//StringCbPrintf( szItemIndex, sizeof( szItemIndex ), "rising-gashapon-%u count %d get - %d, %d, %d", GetType(), pUser->GetRisingBuyCount() + 1, iType, iValue1, iValue2 ); // db field는 1부터 이므로 +1
	//g_LogDBClient.OnInsertCashItem( pUser, GetType(), GetValue( iArray ), iPayAmt, szItemIndex, LogDBClient::CIT_ETC );

	return true;
}


////////////////////////////////////////////////////////////////////////////////
ioEtcItemExtraSlotExtend::ioEtcItemExtraSlotExtend()
{

}

ioEtcItemExtraSlotExtend::~ioEtcItemExtraSlotExtend()
{

}

bool ioEtcItemExtraSlotExtend::IsBuyCondition( int iUse )
{
	if( iUse >= m_iMaxUse )
		return false;

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////
ioEtcItemCompound::ioEtcItemCompound()
{
}

ioEtcItemCompound::~ioEtcItemCompound()
{
}

void ioEtcItemCompound::LoadProperty( ioINILoader &rkLoader )
{
	m_iMaterialType			= rkLoader.LoadInt( "material_type", 0 );
	m_iResultGashaponCode	= rkLoader.LoadInt( "result_gashapon_code", 0 );
	m_bLimitedMaterial		= rkLoader.LoadBool( "unlimited_check",1 );
}

bool ioEtcItemCompound::_OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	if( !pUser )
		return false;

	int iType = 0;
	DWORD iMaterialCode1 = 0, iMaterialCode2 = 0, iMaterialCode3 = 0;

	PACKET_GUARD_bool( rkPacket.Read(iType) );
	PACKET_GUARD_bool( rkPacket.Read(iMaterialCode1) );
	PACKET_GUARD_bool( rkPacket.Read(iMaterialCode2) );
	PACKET_GUARD_bool( rkPacket.Read(iMaterialCode3) );

	if( GetMaterialType() != iType )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[warning][etcitem] material type is wrong : [%d][%d][%d]", pUser->GetUserIndex(), GetMaterialType(), iType );
		return false;
	}

	if( PRESENT_MEDALITEM == iType )
	{
		// 메달 자체가 같은 메달은 가질수 없다는 가정으로 인해 동일한 메달을 구별할 고유한 index가 존재 하지 않음.
		int iMedalCount[3] = {1, 1, 1};

		if( iMaterialCode1 == iMaterialCode2 )
		{
			iMedalCount[0]++;
			iMedalCount[1]--;
		}
		if( iMaterialCode1 == iMaterialCode3 )
		{
			iMedalCount[0]++;
			iMedalCount[2]--;
		}

		if( iMedalCount[0] < 3 )
		{
			if( iMaterialCode2 == iMaterialCode3 )
			{
				iMedalCount[1]++;
				iMedalCount[2]--;
			}
		}
		
		//착용중이지 않은 메달 수와 패킷 받은 코드 수 비교.
		for( int i = 0; i < 3; i++ )
		{
			if( iMedalCount[i] != 0 )
			{
				int iCode = 0;

				if( 0 == i )
					iCode = iMaterialCode1;
				else if( 1 == i )
					iCode = iMaterialCode2;
				else
					iCode = iMaterialCode3;

				if( !pUser->IsExistMedalItemAsCount(iCode, IsUsedLimitedMaterial(), IST_RELEASE, iMedalCount[i]) )
					return false;
			}
		}
	}
	else
	{
		if( !pUser->DoHaveAItem(iType, iMaterialCode1, IsUsedLimitedMaterial(), IST_RELEASE) )
			return false;
		if( !pUser->DoHaveAItem(iType, iMaterialCode2, IsUsedLimitedMaterial(), IST_RELEASE) )
			return false;
		if( !pUser->DoHaveAItem(iType, iMaterialCode3, IsUsedLimitedMaterial(), IST_RELEASE) )
			return false;
	}
	
	rkSlot.AddUse( -1 );
	if( rkSlot.GetUse() <= 0 )
		pUserEtcItem->DeleteEtcItem( rkSlot.m_iType, LogDBClient::ET_DEL );
	else
		pUserEtcItem->SetEtcItem( rkSlot );

	//해당 장비 삭제
	if( !pUser->DeleteItem(iType, iMaterialCode1) )
		return false;
	if( !pUser->DeleteItem(iType, iMaterialCode2) )
		return false;
	if( !pUser->DeleteItem(iType, iMaterialCode3) )
		return false;

	//재료 아이템 삭제
	SP2Packet kReturn( STPK_ITEM_COMPOUND );
	PACKET_GUARD_bool( kReturn.Write(ITEM_NEW_MULTIPLE_COMPOUND_OK) );
	PACKET_GUARD_bool( kReturn.Write(iType) );
	PACKET_GUARD_bool( kReturn.Write(iMaterialCode1) );
	PACKET_GUARD_bool( kReturn.Write(iMaterialCode2) );
	PACKET_GUARD_bool( kReturn.Write(iMaterialCode3) );
	pUser->SendMessage( kReturn );

	short iPresentType   = 0;
	int   iPresentValue1 = 0;
	int   iPresentValue2 = 0;
	int   iPresentValue3 = 0;
	int   iPresentValue4 = 0;
	bool  bWholeAlarm    = false;
	int   iPresentPeso   = 0;

	if( g_PresentHelper.SendGashaponPresent( pUser, GetResultGashaponCode(), iPresentType, iPresentValue1, iPresentValue2, iPresentValue3, iPresentValue4, bWholeAlarm, iPresentPeso ) )
	{
		SP2Packet kReturn( STPK_ETCITEM_USE );
		PACKET_GUARD_bool( kReturn.Write(ETCITEM_USE_OK) );
		PACKET_GUARD_bool( kReturn.Write(rkSlot.m_iType) );
		PACKET_GUARD_bool( kReturn.Write(rkSlot.m_iValue1) );
		PACKET_GUARD_bool( kReturn.Write(rkSlot.m_iValue2) );
		PACKET_GUARD_bool( kReturn.Write(iPresentType) );
		PACKET_GUARD_bool( kReturn.Write(iPresentValue1) );
		PACKET_GUARD_bool( kReturn.Write(iPresentValue2) );
		PACKET_GUARD_bool( kReturn.Write(iPresentValue3) );
		PACKET_GUARD_bool( kReturn.Write(iPresentValue4) );
		PACKET_GUARD_bool( kReturn.Write(bWholeAlarm) );
		PACKET_GUARD_bool( kReturn.Write(iPresentPeso) );

		pUser->SendMessage( kReturn );
		pUser->SendPresentMemory();  // 메모리 선물 전송

		// gashapon present에 관한 전서버 알림
		g_UserNodeManager.AlarmEventInGashaponPresentToAllUsers(pUser, bWholeAlarm, this->GetType(), iPresentType, iPresentValue1, iPresentValue2);
	}

	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemHousingBlockItem::ioEtcItemHousingBlockItem()
{

}

ioEtcItemHousingBlockItem::~ioEtcItemHousingBlockItem()
{

}

void ioEtcItemHousingBlockItem::LoadProperty( ioINILoader &rkLoader )
{
	m_dwBlockItemCode			= rkLoader.LoadInt( "block_item_code", 0 );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ioEtcItemCreateGuildHQCreate::ioEtcItemCreateGuildHQCreate()
{

}

ioEtcItemCreateGuildHQCreate::~ioEtcItemCreateGuildHQCreate()
{

}

bool ioEtcItemCreateGuildHQCreate::_OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	if( !pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pUser == NULL.", __FUNCTION__ );
		return false;
	}

	if( !pUserEtcItem )
		return false;

	if( !pUser->IsGuild() )
	{
		SP2Packet kPacket(STPK_GUILD_ROOM_ACTIVE);
		PACKET_GUARD_bool( kPacket.Write(GUILD_ROOM_NOT_GUILD) );
		pUser->SendMessage(kPacket);
		return false;
	}

	ioUserGuild* pGuild	= pUser->GetUserGuild();
	if( !pGuild )
	{
		SP2Packet kPacket(STPK_GUILD_ROOM_ACTIVE);
		PACKET_GUARD_bool( kPacket.Write(GUILD_ROOM_NOT_GUILD) );
		pUser->SendMessage(kPacket);
		return false;
	}

	if( !pGuild->IsGuildMaster() )
	{
		SP2Packet kPacket(STPK_GUILD_ROOM_ACTIVE);
		PACKET_GUARD_bool( kPacket.Write(GUILD_ROOM_NOT_GUILD_MASTER) );
		pUser->SendMessage(kPacket);
		return false;
	}

	if( pGuild->IsActiveGuildRoom() )
	{
		SP2Packet kPacket(STPK_GUILD_ROOM_ACTIVE);
		PACKET_GUARD_bool( kPacket.Write(GUILD_ROOM_ALREADY_ACTIVE) );
		pUser->SendMessage(kPacket);
		return false;
	}

	//기본 블럭 생성.
	g_GuildRoomBlockMgr.ConstructDefaultBlock(pUser->GetGuildIndex());

	pUser->ActiveUserGuildRoom();
	pGuild->NotifyGuildRoomActive(pUser->GetUserIndex());

	rkSlot.AddUse( -1 );
	if( rkSlot.GetUse() <= 0 )
	{
		pUserEtcItem->DeleteEtcItem( rkSlot.m_iType, LogDBClient::ET_DEL );
	}
	else
	{
		int iArray = 0;
		DWORD dwIndex = 0;

		pUserEtcItem->SetEtcItem( rkSlot, dwIndex, iArray );

		char szItemIndex[MAX_PATH]="";
		StringCbPrintf( szItemIndex, sizeof( szItemIndex ), "%u-%u", dwIndex, iArray+1 ); // db field는 1부터 이므로 +1
		g_LogDBClient.OnInsertEtc( pUser, rkSlot.m_iType, 1, 0, szItemIndex, LogDBClient::ET_DEL );

	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemCreateMyHomeCreate::ioEtcItemCreateMyHomeCreate()
{

}

ioEtcItemCreateMyHomeCreate::~ioEtcItemCreateMyHomeCreate()
{

}

bool ioEtcItemCreateMyHomeCreate::_OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	if( !pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pUser == NULL.", __FUNCTION__ );
		return false;
	}

	if( !pUserEtcItem )
		return false;

	// 해당 유저가 한번이라도 개인 본부 open 여부 확인.
	//g_DBClient.IsExistPersonalHQInfo(pUser->GetUserDBAgentID(), pUser->GetAgentThreadID(), pUser->GetUserIndex(), rkSlot.m_iType, rkSlot.m_iValue1, rkSlot.m_iValue2);
	ActiveMyHome(pUser, rkSlot);
	return false;
}

void ioEtcItemCreateMyHomeCreate::OnBuy( User *pUser, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	if( !pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pUser == NULL.", __FUNCTION__ );
		return;
	}

	ActiveMyHome(pUser, rkSlot);
}

void ioEtcItemCreateMyHomeCreate::ActiveMyHome(User *pUser, ioUserEtcItem::ETCITEMSLOT &rkSlot)
{
	// 해당 유저가 한번이라도 개인 본부 open 여부 확인.
	g_DBClient.IsExistPersonalHQInfo(pUser->GetUserDBAgentID(), pUser->GetAgentThreadID(), pUser->GetUserIndex(), rkSlot.m_iType, rkSlot.m_iValue1, rkSlot.m_iValue2);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemTimeCash::ioEtcItemTimeCash()
{
}

ioEtcItemTimeCash::~ioEtcItemTimeCash()
{
}

/////////////////////////////////////////////////////////////////////////////
ioEtcItemTitle::ioEtcItemTitle()
{

}

ioEtcItemTitle::~ioEtcItemTitle()
{

}

void ioEtcItemTitle::LoadProperty( ioINILoader &rkLoader )
{
	m_dwTitleCode			= rkLoader.LoadInt( "title_code", 0 );
}

bool ioEtcItemTitle::_OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	if( !pUser )
		return false;

	TitleData* pData = g_TitleManager.GetTitleData(m_dwTitleCode);
	if( !pData )
	{
		SP2Packet kPacket(STPK_TITLE_ETC_USE);
		PACKET_GUARD_bool( kPacket.Write(TITLE_ETC_USE_EXCEPTION) );
		pUser->SendMessage(kPacket);
		return false;
	}

	UserTitleInven* pInven = pUser->GetUserTitleInfo();
	if( !pInven )
	{
		SP2Packet kPacket(STPK_TITLE_ETC_USE);
		PACKET_GUARD_bool( kPacket.Write(TITLE_ETC_USE_EXCEPTION) );
		pUser->SendMessage(kPacket);
		return false;
	}

	UserTitleInfo* pInfo = pInven->GetTitle(m_dwTitleCode);
	if( pInfo )
	{
		//이미 소지중.
		SP2Packet kPacket(STPK_TITLE_ETC_USE);
		PACKET_GUARD_bool( kPacket.Write(TITLE_ETC_USE_AREADY_EXIST) );
		pUser->SendMessage(kPacket);
		return false;
	}

	//인벤 Insert
	g_DBClient.OnInsertOrUpdateTitleInfo(pUser->GetUserDBAgentID(), pUser->GetAgentThreadID(), pUser->GetUserIndex(), m_dwTitleCode, 0,
													0, 0, 0, TITLE_NEW, TUT_INSERT_ETC);

	rkSlot.AddUse( -1 );
	if( rkSlot.GetUse() <= 0 )
	{
		pUserEtcItem->DeleteEtcItem( rkSlot.m_iType, LogDBClient::ET_DEL );
	}
	else
	{
		int iArray = 0;
		DWORD dwIndex = 0;

		pUserEtcItem->SetEtcItem( rkSlot, dwIndex, iArray );

		char szItemIndex[MAX_PATH]="";
		StringCbPrintf( szItemIndex, sizeof( szItemIndex ), "%u-%u", dwIndex, iArray+1 ); // db field는 1부터 이므로 +1
		g_LogDBClient.OnInsertEtc( pUser, rkSlot.m_iType, 1, 0, szItemIndex, LogDBClient::ET_DEL );

	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
ioEtcItemTitlePremium::ioEtcItemTitlePremium()
{

}

ioEtcItemTitlePremium::~ioEtcItemTitlePremium()
{

}

bool ioEtcItemTitlePremium::_OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	if( !pUser )
		return false;

	DWORD dwTitleCode	= 0;

	PACKET_GUARD_bool( rkPacket.Read(dwTitleCode) );

	//해당 타이틀 확인.
	UserTitleInven* pInven = pUser->GetUserTitleInfo();
	if( !pInven )
		return false;

	UserTitleInfo* pInfo = pInven->GetTitle(dwTitleCode);
	if( !pInfo )
	{
		//실패
		SP2Packet kPacket(STPK_TITLE_PREMIUM);
		PACKET_GUARD_bool( kPacket.Write(SET_PREMIUM_TITLE_NOT_EXIST) );
		pUser->SendMessage(kPacket);
		return false;
	}

	if( !pInfo->IsActiveTitle() )
	{
		//비활성화 칭호
		SP2Packet kPacket(STPK_TITLE_PREMIUM);
		PACKET_GUARD_bool( kPacket.Write(SET_PREMIUM_TITLE_NOT_EXIST) );
		pUser->SendMessage(kPacket);
		return false;
	}

	if( pInfo->IsPremium() )
	{
		//이미 프리미엄
		SP2Packet kPacket(STPK_TITLE_PREMIUM);
		PACKET_GUARD_bool( kPacket.Write(SET_PREMIUM_TITLE_AREADY_ON) );
		pUser->SendMessage(kPacket);
		return false;
	}

	g_DBClient.OnInsertOrUpdateTitleInfo(pUser->GetUserDBAgentID(), pUser->GetAgentThreadID(), pUser->GetUserIndex(), pInfo->GetCode(), pInfo->GetCurValue(),
												pInfo->GetTitleLevel(), TRUE, pInfo->IsEquip(), TITLE_NEW, TUT_PREMIUM);

	rkSlot.AddUse( -1 );
	if( rkSlot.GetUse() <= 0 )
	{
		pUserEtcItem->DeleteEtcItem( rkSlot.m_iType, LogDBClient::ET_DEL );
	}
	else
	{
		int iArray = 0;
		DWORD dwIndex = 0;

		pUserEtcItem->SetEtcItem( rkSlot, dwIndex, iArray );

		char szItemIndex[MAX_PATH]="";
		StringCbPrintf( szItemIndex, sizeof( szItemIndex ), "%u-%u", dwIndex, iArray+1 ); // db field는 1부터 이므로 +1
		g_LogDBClient.OnInsertEtc( pUser, rkSlot.m_iType, 1, 0, szItemIndex, LogDBClient::ET_DEL );

	}

	return true;
}

ioEtcItemOakBarrel::~ioEtcItemOakBarrel()
{
}

bool ioEtcItemOakBarrel::_OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	/*	// 기존 오크통 구문 삭제
	rkSlot.AddUse( -1 );
	if( rkSlot.GetUse() <= 0 )
	{
		pUserEtcItem->DeleteEtcItem( rkSlot.m_iType, LogDBClient::ET_DEL );
	}
	else
	{
		pUserEtcItem->SetEtcItem( rkSlot );
	}
	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "Use Etc Item (%s/%d/%d)", pUser->GetPublicID().c_str(), rkSlot.m_iType, rkSlot.m_iValue1 );

	// 특별아이템 소모처리를 먼저 발생시켜야 한다
	SP2Packet kReturn( STPK_ETCITEM_USE );
	PACKET_GUARD_bool( kReturn.Write(ETCITEM_USE_OK) );
	PACKET_GUARD_bool( kReturn.Write(rkSlot.m_iType) );
	PACKET_GUARD_bool( kReturn.Write(rkSlot.m_iValue1) );
	PACKET_GUARD_bool( kReturn.Write(rkSlot.m_iValue2) );

	pUser->SendMessage( kReturn );
	*/

	// 모두 사용해도 삭제하지 않는다.
	// OnUse에서 체크하므로 pUser 와 pUserEtcItem NULL이 아님을 보장	
	if( rkSlot.GetUse() > 0 )
	{
		rkSlot.AddUse( -1 );
		pUserEtcItem->SetEtcItem( rkSlot );
	}
	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "Use Etc Item (%s/%d/%d/%d)", pUser->GetPublicID().c_str(), rkSlot.m_iType, rkSlot.m_iValue1, rkSlot.m_iValue2 );
	printf("Use Etc Item (%s/%d/%d/%d)\n", pUser->GetPublicID().c_str(), rkSlot.m_iType, rkSlot.m_iValue1, rkSlot.m_iValue2 );

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemPCROOMFishingRod::ioEtcItemPCROOMFishingRod()
{

}

ioEtcItemPCROOMFishingRod::~ioEtcItemPCROOMFishingRod()
{

}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemPCROOMFishingBait::ioEtcItemPCROOMFishingBait()
{
}

ioEtcItemPCROOMFishingBait::~ioEtcItemPCROOMFishingBait()
{

}

bool ioEtcItemPCROOMFishingBait::_OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	// OnUse에서 체크하므로 pUser 와 pUserEtcItem NULL이 아님을 보장
	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemRaidTicket::ioEtcItemRaidTicket()
{

}

ioEtcItemRaidTicket::~ioEtcItemRaidTicket()
{

}

bool ioEtcItemRaidTicket::_OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	// 모두 사용해도 삭제하지 않는다.
	// OnUse에서 체크하므로 pUser 와 pUserEtcItem NULL이 아님을 보장	
	if( rkSlot.GetUse() > 1 )
	{
		rkSlot.AddUse( -1 );
		pUserEtcItem->SetEtcItem( rkSlot );
	}
	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "Use Etc Item (%s/%d/%d)", pUser->GetPublicID().c_str(), rkSlot.m_iType, rkSlot.m_iValue1 );
	return true;
}

void ioEtcItemRaidTicket::OnAfterBuy( User *pUser, SP2Packet &rkPacket, int iArray, int iTransactionID, int bUserBonusCash )
{
	ioEtcItem::OnAfterBuy(pUser, rkPacket, iArray, iTransactionID, 0);
	// 배틀룸에 있을경우 룸메이트 들에세 레이드 티켓 상황을 브로드 캐스트 해주어야함.

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ioEtcItemAccessoryCompose::ioEtcItemAccessoryCompose()
{

}

ioEtcItemAccessoryCompose::~ioEtcItemAccessoryCompose()
{

}

bool ioEtcItemAccessoryCompose::_OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	if( rkSlot.GetUse() > 0 )
	{
		rkSlot.AddUse( -1 );
		if( rkSlot.GetUse() <= 0 )
		{
			pUserEtcItem->DeleteEtcItem( rkSlot.m_iType, LogDBClient::ET_DEL );
		}
		else
		{
			pUserEtcItem->SetEtcItem( rkSlot );
		}
	}
	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "Use Etc Item AccessoryComposer (%s/%d/%d)", pUser->GetPublicID().c_str(), rkSlot.m_iType, rkSlot.m_iValue1 );

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemAddictivePiece::ioEtcItemAddictivePiece()
{

}

ioEtcItemAddictivePiece::~ioEtcItemAddictivePiece()
{

}

bool ioEtcItemAddictivePiece::_OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	rkSlot.AddUse( -1 );
	if( rkSlot.GetUse() <= 0 )
	{
		pUserEtcItem->DeleteEtcItem( rkSlot.m_iType, LogDBClient::ET_DEL );
	}
	else
	{
		pUserEtcItem->SetEtcItem( rkSlot );
	}
	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "Use Etc Item (%s/%d/%d)", pUser->GetPublicID().c_str(), rkSlot.m_iType, rkSlot.m_iValue1 );

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioEtcItemPetFeed::ioEtcItemPetFeed()
{

}

ioEtcItemPetFeed::~ioEtcItemPetFeed()
{

}

void ioEtcItemPetFeed::LoadProperty( ioINILoader &rkLoader )
{
	ioEtcItem::LoadProperty( rkLoader );

	m_iAddExp = rkLoader.LoadInt( "add_pet_exp", 0 );
}

bool ioEtcItemPetFeed::_OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{
	rkSlot.AddUse( -1 );
	if( rkSlot.GetUse() <= 0 )
	{
		pUserEtcItem->DeleteEtcItem( rkSlot.m_iType, LogDBClient::ET_DEL );
	}
	else
	{
		pUserEtcItem->SetEtcItem( rkSlot );
	}
	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "Use Etc Item (%s/%d/%d)", pUser->GetPublicID().c_str(), rkSlot.m_iType, rkSlot.m_iValue1 );

	return true;
}

// 2018-06-18 by bckim, 카드 짝 마추기 이벤트 추가
ioEtcItemCardMatching::~ioEtcItemCardMatching()
{
}

bool ioEtcItemCardMatching::_OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{	
	rkSlot.AddUse( -1 );
	if( rkSlot.GetUse() <= 0 )
	{
		pUserEtcItem->DeleteEtcItem( rkSlot.m_iType, LogDBClient::ET_DEL );
	}
	else
	{
		pUserEtcItem->SetEtcItem( rkSlot );
	}

	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "Use Etc Item (%s/%d/%d/%d)", pUser->GetPublicID().c_str(), rkSlot.m_iType, rkSlot.m_iValue1, rkSlot.m_iValue2 );
	printf("Use Etc Item (%s/%d/%d/%d)\n", pUser->GetPublicID().c_str(), rkSlot.m_iType, rkSlot.m_iValue1, rkSlot.m_iValue2 );

	return true;
}
// End. 2018-06-18 by bckim, 카드 짝 마추기 이벤트 추가


// 2018-08-30 by bckim, 주사위 이벤트 추가
ioEtcItemDiceGame::~ioEtcItemDiceGame()
{
}

bool ioEtcItemDiceGame::_OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot )
{	
	rkSlot.AddUse( -1 );
	if( rkSlot.GetUse() <= 0 )
	{
		pUserEtcItem->DeleteEtcItem( rkSlot.m_iType, LogDBClient::ET_DEL );
	}
	else
	{
		pUserEtcItem->SetEtcItem( rkSlot );
	}

	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "Use Etc Item (%s/%d/%d/%d)", pUser->GetPublicID().c_str(), rkSlot.m_iType, rkSlot.m_iValue1, rkSlot.m_iValue2 );
	printf("Use Etc Item (%s/%d/%d/%d)\n", pUser->GetPublicID().c_str(), rkSlot.m_iType, rkSlot.m_iValue1, rkSlot.m_iValue2 );

	return true;
}
// End. 2018-08-30 by bckim, 주사위 이벤트 추가


// 2019-07-03 by bckim, 파밍모드 추가
bool ioEtcItem::FindSuperGashaponPackageRandomPresent( DWORD dwEtcItemType, DWORD& dwPackageIndex)
{
	g_SuperGashaponMgr.FindSuperGashaponPackageRandomPresent(dwEtcItemType,dwPackageIndex);
	return true;
}
// End. 2019-07-03 by bckim, 파밍모드 추가
