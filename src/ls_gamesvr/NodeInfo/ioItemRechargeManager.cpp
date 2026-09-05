
#include "stdafx.h"

#include "../DataBase/LogDBClient.h"
#include "../EtcHelpFunc.h"
#include "ioItemRechargeManager.h"
#include "UserNodeManager.h"

template<> ioItemRechargeManager* Singleton< ioItemRechargeManager >::ms_Singleton = 0;
ioItemRechargeManager::ioItemRechargeManager()
{
}

ioItemRechargeManager::~ioItemRechargeManager()
{
}

ioItemRechargeManager& ioItemRechargeManager::GetSingleton()
{
	return Singleton< ioItemRechargeManager >::GetSingleton();
}

void ioItemRechargeManager::LoadInIData()
{
	ioINILoader kLoader( "config/sp2_item_recharge.ini" );

	char szKey[MAX_PATH] = "";

	kLoader.SetTitle( "normal" );
	LoadPromotionInfo( kLoader, m_NormalItem );

	kLoader.SetTitle( "extra" );
	LoadPromotionInfo( kLoader, m_ExtraItem );

	kLoader.SetTitle( "rare" );
	LoadPromotionInfo( kLoader, m_RareItem );

	kLoader.SetTitle( "exception" );
	m_ExceptionInfoList.clear();
	int iCnt = kLoader.LoadInt( "exception_count", 0 );
	for( int i=0; i<iCnt; ++i )
	{
		wsprintf( szKey, "exception_item_code_%d", i+1 );
		int iCode = kLoader.LoadInt( szKey, 0 );
		wsprintf( szKey, "exception_promotion_time_%d", i+1 );
		int iLimit = kLoader.LoadInt( szKey, 0 );

		m_ExceptionInfoList.insert( mapExceptionInfo::value_type( iCode, iLimit ) );
	}
}

void ioItemRechargeManager::LoadPromotionInfo( ioINILoader &rkLoader, PromotionInfo &rkInfo )
{
	rkInfo.m_iWeapon = rkLoader.LoadInt( "promotion_weapon_time", 0 );
	rkInfo.m_iArmor  = rkLoader.LoadInt( "promotion_armor_time",  0 );
	rkInfo.m_iHelmet = rkLoader.LoadInt( "promotion_helmet_time", 0 );
	rkInfo.m_iCloak  = rkLoader.LoadInt( "promotion_cloak_time",  0 );
}

int ioItemRechargeManager::GetPromotionTime( int iItemCode )
{
	mapExceptionInfo::iterator iter = m_ExceptionInfoList.find( iItemCode );
	if( iter != m_ExceptionInfoList.end() )
	{
		return iter->second;
	}

	int iGroup = static_cast<int>(iItemCode/100000) + 1;
	int iExtra = static_cast<int>(iItemCode/10000) % 10;
	int iRare  = static_cast<int>(iItemCode/1000) % 10;

	switch( iGroup )
	{
	case 1:
		{
			if( iRare > 0 )
				return m_RareItem.m_iWeapon;
			else if( iExtra > 0 )
				return m_ExtraItem.m_iWeapon;
			else
				return m_NormalItem.m_iWeapon;
		}
	case 2:
		{
			if( iRare > 0 )
				return m_RareItem.m_iArmor;
			else if( iExtra > 0 )
				return m_ExtraItem.m_iArmor;
			else
				return m_NormalItem.m_iArmor;
		}
	case 3:
		{
			if( iRare > 0 )
				return m_RareItem.m_iHelmet;
			else if( iExtra > 0 )
				return m_ExtraItem.m_iHelmet;
			else
				return m_NormalItem.m_iHelmet;
		}
	case 4:
		{
			if( iRare > 0 )
				return m_RareItem.m_iCloak;
			else if( iExtra > 0 )
				return m_ExtraItem.m_iCloak;
			else
				return m_NormalItem.m_iCloak;
		}
	}

	return 0;
}

bool ioItemRechargeManager::SetRechargeExtraItem( User *pUser, int iSlotIndex, int iItemCode, int iRechargeTime, DWORD dwEtcItemCode )
{
	ioUserExtraItem *pExtraItem = pUser->GetUserExtraItem();
	if( !pExtraItem )
	{
		SP2Packet kPacket( STPK_EXTRA_ITEM_RECHARGE_TIME );
		kPacket << EXTRA_ITEM_RECHARGE_TIME_FAIL;
		pUser->SendMessage( kPacket );
		return false;
	}

	ioUserExtraItem::EXTRAITEMSLOT rkExtraItem;
	if( !pExtraItem->GetExtraItem( iSlotIndex, rkExtraItem ) )
	{
		SP2Packet kPacket( STPK_EXTRA_ITEM_RECHARGE_TIME );
		kPacket << EXTRA_ITEM_RECHARGE_TIME_FAIL_UNKNOWN_ITEM;
		pUser->SendMessage( kPacket );
		return false;
	}

	if( rkExtraItem.m_iItemCode != iItemCode )
	{
		SP2Packet kPacket( STPK_EXTRA_ITEM_RECHARGE_TIME );
		kPacket << EXTRA_ITEM_RECHARGE_TIME_FAIL_UNKNOWN_ITEM;
		pUser->SendMessage( kPacket );
		return false;
	}

	CTime CurrentTime( Help::GetSafeValueForCTimeConstructor( rkExtraItem.GetYear(), rkExtraItem.GetMonth(), rkExtraItem.GetDay(), rkExtraItem.GetHour(), rkExtraItem.GetMinute(), 0 ) );
	CTimeSpan rechargeTime( 0, iRechargeTime, 0, 0 );
	CTime ResultTime = CurrentTime + rechargeTime;

	CTimeSpan cRemainTime = ResultTime - CTime::GetCurrentTime();
	int iLimitHour = cRemainTime.GetTotalSeconds() / 3600;

	int iPromotionTime = GetPromotionTime( iItemCode );
	if( iPromotionTime <= 0 )
	{
		SP2Packet kPacket( STPK_EXTRA_ITEM_RECHARGE_TIME );
		kPacket << EXTRA_ITEM_RECHARGE_TIME_FAIL_IMPOSSIBLE;
		pUser->SendMessage( kPacket );
		return false;
	}

	if( iLimitHour >= iPromotionTime )
	{
		rkExtraItem.m_PeriodType = ioUserExtraItem::EPT_MORTMAIN;

		char szItemIndex[MAX_PATH]="";
		StringCbPrintf( szItemIndex, sizeof( szItemIndex ), "%d-%d", dwEtcItemCode, iRechargeTime );
		g_LogDBClient.OnInsertExtraItem( pUser, rkExtraItem.m_iItemCode, rkExtraItem.m_iReinforce, 0, 0, 0, 0, 0, 0, szItemIndex, LogDBClient::ERT_RECHARGE_ITEM );
	}

	rkExtraItem.SetDate( ResultTime.GetYear(), ResultTime.GetMonth(), ResultTime.GetDay(), ResultTime.GetHour(), ResultTime.GetMinute() );
	pExtraItem->SetExtraItem( rkExtraItem );
	pUser->SaveExtraItem();

	SP2Packet kPacket( STPK_EXTRA_ITEM_RECHARGE_TIME );
	kPacket << EXTRA_ITEM_RECHARGE_TIME_SUCCESS;
	kPacket << iSlotIndex;
	kPacket << rkExtraItem.m_iValue1;
	kPacket << rkExtraItem.m_iValue2;
	kPacket << rkExtraItem.m_PeriodType;
	kPacket << iRechargeTime;
	pUser->SendMessage( kPacket );

	// etc item, accessary recharge process 관련 전서버 알림
	g_UserNodeManager.AlarmEventOfItemRechargeProcessToAllUsers(pUser, rkExtraItem.m_PeriodType, dwEtcItemCode, rkExtraItem.m_iItemCode);

	return true;
}





//////////////////////////////////////////////////////////////////////////
// 악세사리 충전기~~ by jal
//////////////////////////////////////////////////////////////////////////


template<> ioAccRechargeManager* Singleton< ioAccRechargeManager >::ms_Singleton = 0;
ioAccRechargeManager::ioAccRechargeManager()
{
}

ioAccRechargeManager::~ioAccRechargeManager()
{
}

ioAccRechargeManager& ioAccRechargeManager::GetSingleton()
{
	return Singleton< ioAccRechargeManager >::GetSingleton();
}

void ioAccRechargeManager::LoadInIData()
{
	ioINILoader kLoader( "config/sp2_item_recharge.ini" );

	char szKey[MAX_PATH] = "";

	kLoader.SetTitle( "normal" );
	LoadPromotionInfo( kLoader, m_NormalItem );

	kLoader.SetTitle( "exception" );
	m_ExceptionInfoList.clear();
	int iCnt = kLoader.LoadInt( "exception_count", 0 );
	for( int i=0; i<iCnt; ++i )
	{
		wsprintf( szKey, "exception_item_code_%d", i+1 );
		int iCode = kLoader.LoadInt( szKey, 0 );
		wsprintf( szKey, "exception_promotion_time_%d", i+1 );
		int iLimit = kLoader.LoadInt( szKey, 0 );

		m_ExceptionInfoList.insert( mapExceptionInfo::value_type( iCode, iLimit ) );
	}
}

void ioAccRechargeManager::LoadPromotionInfo( ioINILoader &rkLoader, PromotionInfo &rkInfo )
{
	rkInfo.nRing = rkLoader.LoadInt( "promotion_ring_time", 0 );
	rkInfo.nNecklace  = rkLoader.LoadInt( "promotion_necklace_time",  0 );
	rkInfo.nBracelet = rkLoader.LoadInt( "promotion_bracelet_time", 0 );
}

int ioAccRechargeManager::GetPromotionTime( int iItemCode )
{
	mapExceptionInfo::iterator iter = m_ExceptionInfoList.find( iItemCode );
	if( iter != m_ExceptionInfoList.end() )
	{
		return iter->second;
	}

	int iGroup = static_cast<int>(iItemCode/1000000);

	switch( iGroup )
	{
	case 1:
		return m_NormalItem.nRing;
	case 2:
		return m_NormalItem.nNecklace;
	case 3:
		return m_NormalItem.nBracelet;
	}

	return 0;
}

bool ioAccRechargeManager::SetRechargeAccessory( User *pUser, int iSlotIndex, int iItemCode, int iRechargeTime, DWORD dwEtcItemCode )
{
	if(pUser == NULL)
		return false;

	ioUserAccessory *pAcc = pUser->GetUserAccessory();

	if( !pAcc )
	{
		SP2Packet kPacket( STPK_ACCESSORY_RECHARGE_TIME );
		kPacket << EXTRA_ITEM_RECHARGE_TIME_FAIL;
		pUser->SendMessage( kPacket );
		return false;
	}

	Accessory* pEquipAccessory = pAcc->GetAccessory(iSlotIndex);
	if(pEquipAccessory == NULL)
	{
		SP2Packet kPacket( STPK_ACCESSORY_RECHARGE_TIME );
		kPacket << EXTRA_ITEM_RECHARGE_TIME_FAIL_UNKNOWN_ITEM;
		pUser->SendMessage( kPacket );
		return false;
	}

	if( pEquipAccessory->GetAccessoryCode() != iItemCode)
	{
		SP2Packet kPacket( STPK_ACCESSORY_RECHARGE_TIME );
		kPacket << EXTRA_ITEM_RECHARGE_TIME_FAIL_UNKNOWN_ITEM;
		pUser->SendMessage( kPacket );
		return false;
	}
	
	CTime kCurTime = CTime::GetCurrentTime();
	CTime CurAccTime( Help::GetSafeValueForCTimeConstructor( pEquipAccessory->GetYear(), pEquipAccessory->GetMonth(), pEquipAccessory->GetDay(), pEquipAccessory->GetHour(), pEquipAccessory->GetMinute(), 0 ) );
	
	CTimeSpan kRemainTime = CurAccTime - kCurTime;
	CTimeSpan rechargeTime( 0, iRechargeTime, 0, 0 );
	CTime ResultTime;
	if( kRemainTime.GetTotalMinutes() > 0 )
		ResultTime = CurAccTime + rechargeTime;
	else
		ResultTime = kCurTime + rechargeTime;

	int iPromotionTime = GetPromotionTime( iItemCode );
	if( iPromotionTime <= 0 )
	{
		SP2Packet kPacket( STPK_ACCESSORY_RECHARGE_TIME );
		kPacket << EXTRA_ITEM_RECHARGE_TIME_FAIL_IMPOSSIBLE;
		pUser->SendMessage( kPacket );
		return false;
	}

	pEquipAccessory->SetDate(ResultTime.GetYear(), ResultTime.GetMonth(), ResultTime.GetDay(), ResultTime.GetHour(), ResultTime.GetMinute());
	
	pAcc->EquipAccessory(pEquipAccessory->GetWearingClass(), pEquipAccessory);

	SP2Packet kPacket( STPK_ACCESSORY_RECHARGE_TIME );
	kPacket << EXTRA_ITEM_RECHARGE_TIME_SUCCESS;
	kPacket << iSlotIndex;
	kPacket << pEquipAccessory->GetYearMonthDayValue() ;
	kPacket << pEquipAccessory->GetHourMinute();
	kPacket << pEquipAccessory->GetPeriodType();
	kPacket << iRechargeTime;
	pUser->SendMessage( kPacket );

	// etc item, accessary recharge process 관련 전서버 알림
	g_UserNodeManager.AlarmEventOfItemRechargeProcessToAllUsers(pUser, pEquipAccessory->GetPeriodType(), dwEtcItemCode, pEquipAccessory->GetAccessoryCode());

	return true;
}