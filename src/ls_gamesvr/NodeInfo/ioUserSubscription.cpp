
#include "stdafx.h"

//#include "../Window.h"
#include "../MainProcess.h"
#include "../MainProcess.h"
#include "../QueryData/QueryResultData.h"
#include "../DataBase/DBClient.h"
#include "../DataBase/LogDBClient.h"

#include "../EtcHelpFunc.h"

#include "Room.h"
#include "ioItemInfoManager.h"
#include "ioDecorationPrice.h"
#include "ioEtcItemManager.h"
#include "ioExtraItemInfoManager.h"
#include "ioItemInitializeControl.h"
#include "User.h"

#include "ioUserPresent.h"
#include "ioUserSubscription.h"
#include "iomedaliteminfomanager.h"
#include "ioAlchemicMgr.h"

#include <strsafe.h>
#include "../Local/ioLocalParent.h"
#include "../MainServerNode/MainServerNode.h"

ioUserSubscription::ioUserSubscription()
{
	Initialize( NULL );
}

ioUserSubscription::~ioUserSubscription()
{
	m_vSubscriptionList.clear();
}

void ioUserSubscription::Initialize( User *pUser )
{
	m_pUser = pUser;
	m_vSubscriptionList.clear();
	m_vTempMemoryData.clear();
	m_dwLastDBIndex = 0;
}

bool ioUserSubscription::CheckExistSubscriptionData( DWORD dwIndex, const ioHashString& szSuscriptionID )
{
	SubscriptionData &rkData = GetSubscriptionData( dwIndex, szSuscriptionID );
	if( rkData.m_iPresentType == 0 )
		return false;

	return true;
}

bool ioUserSubscription::CheckSubscriptionState( DWORD dwIndex, const ioHashString& szSuscriptionID )
{
	SubscriptionData &rkData = GetSubscriptionData( dwIndex, szSuscriptionID );
	if( rkData.m_iSubscriptionState == SUBSCRIPTION_STATE_NORMAL )
		return true;

	return false;
}

ioUserSubscription::SubscriptionData& ioUserSubscription::GetSubscriptionData( DWORD dwIndex, const ioHashString& szSuscriptionID )
{
	int iSize = m_vSubscriptionList.size();
	for(int i = 0;i < iSize;i++)
	{
		SubscriptionData &rkData = m_vSubscriptionList[i];
		if( rkData.m_dwIndex != dwIndex ) continue;
		if( rkData.m_szSubscriptionID != szSuscriptionID ) continue;

		return rkData;
	}

	static SubscriptionData kTemp;
	return kTemp;
}

bool ioUserSubscription::DeleteSubscriptionData( DWORD dwIndex, const ioHashString& szSuscriptionID, DeleteSubscriptionType eDeleteSubscriptionType )
{
	int iSize = m_vSubscriptionList.size();
	for(int i = 0;i < iSize;i++)
	{
		SubscriptionData &rkData = m_vSubscriptionList[i];
		if( rkData.m_dwIndex != dwIndex ) continue;
		if( rkData.m_szSubscriptionID != szSuscriptionID ) continue;

		DWORD dwAgentServerID = 0;
		DWORD dwAggentThreadID= 0;
		if( m_pUser )
		{
			dwAgentServerID = m_pUser->GetUserDBAgentID();
			dwAggentThreadID= m_pUser->GetAgentThreadID();

			char szNote[MAX_PATH]="";
			if( eDeleteSubscriptionType == DST_TIMEOVER )
				StringCbPrintf( szNote, sizeof( szNote ), "TimeOver Index:%d", dwIndex );
			else if( eDeleteSubscriptionType == DST_RECV )
				StringCbPrintf( szNote, sizeof( szNote ), "Recv Index:%d", dwIndex );
			else if( eDeleteSubscriptionType == DST_RETR )
				StringCbPrintf( szNote, sizeof( szNote ), "Retr Index:%d", dwIndex );
			else
				StringCbPrintf( szNote, sizeof( szNote ), "Index:%d", dwIndex );
		}

		// DB에서 제거 처리 필요
		g_DBClient.OnSubscriptionDataDelete( m_pUser->GetUserDBAgentID(), m_pUser->GetAgentThreadID(), rkData.m_dwIndex );

		m_vSubscriptionList.erase( m_vSubscriptionList.begin() + i );
		return true;
	}
	return false;
}

bool ioUserSubscription::CompareSubscriptionData( SubscriptionData &kData )
{
	int iSize = m_vSubscriptionList.size();
	for(int i = 0;i < iSize;i++)
	{
		SubscriptionData &rkData = m_vSubscriptionList[i];
		if( rkData.m_dwIndex == kData.m_dwIndex )
			return true;
	}
	return false;
}

bool ioUserSubscription::CheckLimitData( DWORD dwIndex, const ioHashString& szSuscriptionID )
{
	SubscriptionData &rkData = GetSubscriptionData( dwIndex, szSuscriptionID );
	if( rkData.m_iPresentType == 0 || rkData.m_dwLimitDate == 0 )
		return false;
	
	SHORT iYear   = (SHORT)( 2000 + ( rkData.m_dwLimitDate / 100000000 ) );
	SHORT iMonth  = (SHORT)( ( rkData.m_dwLimitDate % 100000000 ) / 1000000 );
	SHORT iDay    = (SHORT)( ( rkData.m_dwLimitDate % 1000000 ) / 10000 );
	SHORT iHour   = (SHORT)( ( rkData.m_dwLimitDate % 10000 ) / 100 );
	SHORT iMinute = (SHORT)( rkData.m_dwLimitDate % 100 );
	CTime cLimitDate( Help::GetSafeValueForCTimeConstructor( iYear, iMonth, iDay, iHour, iMinute, 0 ) );

	if( CTime::GetCurrentTime() >= cLimitDate )
		return false;

	return true;
}

void ioUserSubscription::DBtoSubscriptionData( CQueryResultData *query_data )
{
	if( !m_pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioUserSubscription::DBtoSubscriptionData() User NULL!!"); 
		return;
	}

	//
	LOOP_GUARD();
	vSubscriptionData vNewSubscription;
	while( query_data->IsExist() )
	{		
		SubscriptionData kData;
		query_data->GetValue( kData.m_dwIndex, sizeof( DWORD ) );

		query_data->GetValue( kData.m_iPresentType, sizeof( short ) );
		query_data->GetValue( kData.m_iPresentValue1, sizeof( int ) );
		query_data->GetValue( kData.m_iPresentValue2, sizeof( int ) );
		query_data->GetValue( kData.m_iSubscriptionGold, sizeof( int ) );
		query_data->GetValue( kData.m_iUsedBonusCash, sizeof( int ) );
		query_data->GetValue( kData.m_iSubscriptionState, sizeof( short ) );

		query_data->GetValue( kData.m_szSubscriptionID, 64 );

		DBTIMESTAMP DTS;
		query_data->GetValue( (char*)&DTS, sizeof(DBTIMESTAMP) );
		CTime LimitTime( Help::GetSafeValueForCTimeConstructor( DTS.year, DTS.month, DTS.day, DTS.hour, DTS.minute, DTS.second ) );
		kData.m_dwLimitDate = Help::ConvertCTimeToDate( LimitTime );

		if( !CompareSubscriptionData( kData ) )
		{
			m_dwLastDBIndex = max( m_dwLastDBIndex, kData.m_dwIndex );	// 마지막 DB 인덱스
			vNewSubscription.push_back( kData );
			m_vSubscriptionList.push_back( kData );
		}
	}	
	LOOP_GUARD_CLEAR();

	if( vNewSubscription.empty() )
	{
		SP2Packet kPacket( STPK_SUBSCRIPTION_DATA );
		kPacket << 0;
		m_pUser->SendMessage( kPacket );
		return;
	}

	//
	SP2Packet kPacket( STPK_SUBSCRIPTION_DATA );
	kPacket << (int)vNewSubscription.size();

	vSubscriptionData::iterator iter = vNewSubscription.begin();
	for(;iter != vNewSubscription.end();iter++)
	{
		SubscriptionData &kData = *iter;

		kPacket << kData.m_dwIndex << kData.m_szSubscriptionID << kData.m_iSubscriptionGold << kData.m_iUsedBonusCash;
		kPacket << kData.m_iPresentType << kData.m_iPresentValue1 << kData.m_iPresentValue2
			    << kData.m_dwLimitDate;
	}
	m_pUser->SendMessage( kPacket );
	
	vNewSubscription.clear();
}

DWORD ioUserSubscription::GetLastSubscriptionDBIndex()
{
	return m_dwLastDBIndex;
}

void ioUserSubscription::SubscriptionMileageSend(SubscriptionData &rkData)
{
	if( !m_pUser )
		return;

	DWORD dwItemType	= 3;
	DWORD dwMilegeCode	= 1005633;

	CTimeSpan cPresentGapTime( 15, 0, 0, 0 );
	CTime kPresentTime = CTime::GetCurrentTime() + cPresentGapTime;
	int iSubscriptionGold = 0;
	int iSubscriptionRealGold = rkData.m_iSubscriptionGold;
	int iSubscriptionBonusGold = rkData.m_iUsedBonusCash;
	
	// 보너스 캐시를 통해 구매한 경우
	iSubscriptionGold = iSubscriptionBonusGold + iSubscriptionRealGold;
	
	int iCashBackRatio	= 0;
	int iCashBack		= 0;

	int iMileage;

	CTime cCurTime = CTime::GetCurrentTime();
	if ( Help::ConvertCTimeToDate( cCurTime ) >= Help::ConvertCTimeToDate( g_EventMgr.GetLosaStartDate() ) && 
        ( Help::ConvertCTimeToDate( cCurTime ) < Help::ConvertCTimeToDate( g_EventMgr.GetLosaEndDate() ) ) )
	 {
		int iMilegeRatio	= 0;
		if( m_pUser->IsPCRoomAuthority() )
			iMilegeRatio	= g_EventMgr.GetPcRoomMileageRatio();
		
		iMilegeRatio	+= g_EventMgr.GetLosaMileageRatio();

		if( iSubscriptionGold > 0)
			iMileage = (iSubscriptionGold / 100) * iMilegeRatio;
	 }
	 else
	 {
		 if( m_pUser->IsPCRoomAuthority() )
		 {
			 if( iSubscriptionGold > 0)
				iMileage = (iSubscriptionGold * g_EventMgr.GetPcRoomMileageRatio() ) / 100;
		 }
		 else
		 {
			 if( iSubscriptionGold > 0)
				iMileage = (iSubscriptionGold * g_EventMgr.GetMileageRatio() ) /100;
		 }

	 }

	//캐시백 이벤트
	if( g_EventMgr.isChannelingCashBackOpen() )
	{
		if ( Help::ConvertCTimeToDate( cCurTime ) >= Help::ConvertCTimeToDate( g_EventMgr.GetCashBackStartDate() ) && 
			( Help::ConvertCTimeToDate( cCurTime ) < Help::ConvertCTimeToDate( g_EventMgr.GetCashBackEndDate() ) ) )
		{
			
			iCashBackRatio = g_EventMgr.GetLosaCashBackRatio();

			if( iSubscriptionRealGold > 0 )
				iCashBack = (iSubscriptionRealGold * iCashBackRatio ) / 100;
			else
				iCashBack = 0;

			CTimeSpan cPresentGapTime( 7, 0, 0, 0 );
			CTime kPresentTime = CTime::GetCurrentTime() + cPresentGapTime;


			if( iCashBack > 0 )
			{
				g_DBClient.OnInsertPresentData( m_pUser->GetUserDBAgentID(), m_pUser->GetAgentThreadID(), g_MainServer.GetSendID(), m_pUser->GetPublicID(), PRESENT_BONUS_CASH, iCashBack, 
					g_EventMgr.GetLosaCashBackPeriod(), 0, 0, 17, kPresentTime, 1 );
				
				char szNote[MAX_PATH]="";
				StringCbPrintf( szNote, sizeof( szNote ), "CashBackEvent:%d:%d:%d", PRESENT_BONUS_CASH, iCashBack, g_EventMgr.GetLosaCashBackPeriod() );
				g_LogDBClient.OnInsertPresent( 0, g_MainServer.GetSendID(), g_App.GetPublicIP().c_str(), m_pUser->GetUserIndex(), PRESENT_BONUS_CASH, iCashBack, g_EventMgr.GetLosaCashBackPeriod(),
					0, 0, LogDBClient::PST_RECIEVE, szNote );
			}
			
		}
	}
	// 마일리지가 0이면 선물보내지 않음
	if( iMileage <= 0 )
	{	
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Fail Mileage 0 : userIndex:%d,userID:%s,mileage:%d", __FUNCTION__, m_pUser->GetUserIndex(), m_pUser->GetPublicID().c_str(), iMileage );
		return;
	}

	//int iMileage = iSubscriptionGold / 10;
	g_DBClient.OnInsertPresentData( m_pUser->GetUserDBAgentID(), m_pUser->GetAgentThreadID(), g_MainServer.GetSendID(), m_pUser->GetPublicID(), 3, 1005633, iMileage, 0, 0, 17, kPresentTime, 1 );

	char szNote[MAX_PATH]="";
 	StringCbPrintf( szNote, sizeof( szNote ), "Buy:%d:%d:%d,%d", rkData.m_iPresentType, rkData.m_iPresentValue1, rkData.m_iPresentValue2, iMileage );
	g_LogDBClient.OnInsertPresent( 0, g_MainServer.GetSendID(), g_App.GetPublicIP().c_str(), m_pUser->GetUserIndex(), dwItemType, dwMilegeCode, iMileage, 0, 0, LogDBClient::PST_RECIEVE, szNote );

	m_pUser->_OnSelectPresent( 30 );
}

void ioUserSubscription::SubscriptionRecv( IN SP2Packet &rkRecvPacket, OUT SP2Packet &rkPacket )
{
	DWORD dwIndex;
	ioHashString szSubscriptionID;
	rkRecvPacket >> dwIndex;
	rkRecvPacket >> szSubscriptionID;

	if( !m_pUser )
	{
		rkPacket << SUBSCRIPTION_RECV_EXCEPTION << dwIndex << szSubscriptionID;
		return;
	}

	SubscriptionData &rkData = GetSubscriptionData( dwIndex, szSubscriptionID );
	if( rkData.m_iPresentType == 0 )
	{
		rkPacket << SUBSCRIPTION_RECV_NONE_INDEX << dwIndex << szSubscriptionID;
		return;
	}

	switch( rkData.m_iPresentType )
	{
	case PRESENT_SOLDIER:
		{
			int iResult = m_pUser->SetPresentChar( rkData.m_iPresentValue1, rkData.m_iPresentValue2 );
			switch( iResult )
			{
			case PRESENT_RECV_ALREADY_CHAR:
				rkPacket << SUBSCRIPTION_RECV_ALREADY_CHAR;
				break;
			case PRESENT_RECV_CHAR_CREATE_DELAY:
				rkPacket << SUBSCRIPTION_RECV_CHAR_CREATE_DELAY;
				break;
			case PRESENT_RECV_CHAR_SLOT_FULL:
				rkPacket << SUBSCRIPTION_RECV_CHAR_SLOT_FULL;
				break;

#ifdef ARENA_MODE_BY_BCKIM			// 2018-07-25 by bckim, 아레나 모드 추가		SubscriptionRecv
			case PRESENT_RECV_EXCEPTION:
				rkPacket << SUBSCRIPTION_RECV_EXCEPTION;
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"[ARENA_MODE]+[SubscriptionRecv]+SUBSCRIPTION_RECV_EXCEPTION");
				break;			
#endif
			case PRESENT_RECV_OK:
				{
					g_LogDBClient.OnInsertSubscription( m_pUser, rkData.m_dwIndex, rkData.m_iPresentType, rkData.m_iPresentValue1, rkData.m_iPresentValue2,
														rkData.m_iSubscriptionGold, rkData.m_szSubscriptionID, rkData.m_dwLimitDate, LogDBClient::SST_RECIVE );

					SubscriptionMileageSend(rkData);

					rkPacket << SUBSCRIPTION_RECV_OK << dwIndex << szSubscriptionID;
					DeleteSubscriptionData( rkData.m_dwIndex, rkData.m_szSubscriptionID, DST_RECV );
				}
				break;
			}
		}
		return;	
	case PRESENT_DECORATION:
		{
			ITEMSLOT kSlot;
			kSlot.m_item_type = rkData.m_iPresentValue1;
			kSlot.m_item_code = rkData.m_iPresentValue2;

			ioInventory *pInventory = m_pUser->GetInventory();
			if( pInventory->IsSlotItem( kSlot ) )       // 이미 있는 치장 받음				
			{
				rkPacket << SUBSCRIPTION_RECV_ALREADY_DECO << dwIndex << szSubscriptionID;
			}
			else 
			{
				DWORD dwDecoSlotIndex = 0;
				int   iSlotArray  = 0;

				if( pInventory->IsFull() )
				{
					PACKET_GUARD_VOID( rkPacket.Write( SUBSCRIPTION_RECV_MAX_COUNT ) );
					PACKET_GUARD_VOID( rkPacket.Write( dwIndex ) );
					PACKET_GUARD_VOID( rkPacket.Write( szSubscriptionID ) );
					return;
				}

				pInventory->AddSlotItem( kSlot, false, 0, LogDBClient::DT_PRESENT, dwDecoSlotIndex, iSlotArray );

				if( dwDecoSlotIndex != 0 && dwDecoSlotIndex != ioInventory::NEW_INDEX )
				{
					char szItemIndex[MAX_PATH]="";
					StringCbPrintf( szItemIndex, sizeof( szItemIndex ), "%u-%u", dwDecoSlotIndex, iSlotArray+1 ); // db field는 1부터 이므로 +1
					g_LogDBClient.OnInsertDeco( m_pUser, kSlot.m_item_type, kSlot.m_item_code, 0, szItemIndex, LogDBClient::DT_PRESENT );
				}

				// 종족 치장을 구매하면 랜덤 치장 아이템을 지급한다.
				if( kSlot.m_item_type % 1000 == UID_KINDRED )
				{
					CHARACTER kCharInfo;
					kCharInfo.m_class_type = kSlot.m_item_type/100000;
					kCharInfo.m_sex        = 2; // 1남자, 2여자
					kCharInfo.m_face       = g_DecorationPrice.GetDefaultDecoCode( 1, UID_FACE, m_pUser->GetPrivateID().GetHashCode() + kCharInfo.m_class_type + UID_FACE, kCharInfo.m_class_type ); // 0남자, 1여자
					kCharInfo.m_hair       = g_DecorationPrice.GetDefaultDecoCode( 1, UID_HAIR, m_pUser->GetPrivateID().GetHashCode() + kCharInfo.m_class_type + UID_HAIR, kCharInfo.m_class_type );
					kCharInfo.m_skin_color = g_DecorationPrice.GetDefaultDecoCode( 1, UID_SKIN_COLOR, m_pUser->GetPrivateID().GetHashCode() + kCharInfo.m_class_type + UID_SKIN_COLOR, kCharInfo.m_class_type );
					kCharInfo.m_hair_color = g_DecorationPrice.GetDefaultDecoCode( 1, UID_HAIR_COLOR, m_pUser->GetPrivateID().GetHashCode() + kCharInfo.m_class_type + UID_HAIR_COLOR, kCharInfo.m_class_type );	
					kCharInfo.m_underwear  = g_DecorationPrice.GetDefaultDecoCode( 1, UID_UNDERWEAR, m_pUser->GetPrivateID().GetHashCode() + kCharInfo.m_class_type + UID_UNDERWEAR, kCharInfo.m_class_type );	
					m_pUser->SetDefaultDecoItem( kCharInfo );
				}

				g_LogDBClient.OnInsertSubscription( m_pUser, rkData.m_dwIndex, rkData.m_iPresentType, rkData.m_iPresentValue1, rkData.m_iPresentValue2,
													rkData.m_iSubscriptionGold, rkData.m_szSubscriptionID, rkData.m_dwLimitDate, LogDBClient::SST_RECIVE );

				rkPacket << SUBSCRIPTION_RECV_OK << dwIndex << szSubscriptionID << kSlot.m_item_type << kSlot.m_item_code;

				ioUserGrowthLevel *pGrowthLevel = m_pUser->GetUserGrowthLevel();
				if( pGrowthLevel )
					pGrowthLevel->AddCharGrowthPointByDecoWoman( kSlot.m_item_type, kSlot.m_item_code );

				DeleteSubscriptionData( rkData.m_dwIndex, rkData.m_szSubscriptionID, DST_RECV );
			}			
		}
		return;
	case PRESENT_ETC_ITEM:
		{	
			int iResult = m_pUser->SetPresentEtcItem( rkData.m_iPresentValue1, rkData.m_iPresentValue2 );
			if( iResult == SUBSCRIPTION_RECV_OK )
			{
				g_LogDBClient.OnInsertSubscription( m_pUser, rkData.m_dwIndex, rkData.m_iPresentType, rkData.m_iPresentValue1, rkData.m_iPresentValue2,
													rkData.m_iSubscriptionGold, rkData.m_szSubscriptionID, rkData.m_dwLimitDate, LogDBClient::SST_RECIVE );
			}

			rkPacket << iResult << dwIndex << szSubscriptionID;
			DeleteSubscriptionData( rkData.m_dwIndex, rkData.m_szSubscriptionID, DST_RECV );
		}
		return;
	case PRESENT_PESO:
		{
			__int64 iPrevMoney = m_pUser->GetMoney();
			m_pUser->AddMoney( rkData.m_iPresentValue1 );

			g_LogDBClient.OnInsertPeso( m_pUser, rkData.m_iPresentValue1, LogDBClient::PT_RECV_PRESENT );

			g_LogDBClient.OnInsertGameLogInfo(LogDBClient::GLT_PESO_GAIN, m_pUser, 0, 0, LogDBClient::LET_ITEM, LogDBClient::PT_RECV_PRESENT, PRESENT_PESO, 0, rkData.m_iPresentValue1, NULL);

			g_LogDBClient.OnInsertSubscription( m_pUser, rkData.m_dwIndex, rkData.m_iPresentType, rkData.m_iPresentValue1, rkData.m_iPresentValue2,
												rkData.m_iSubscriptionGold, rkData.m_szSubscriptionID, rkData.m_dwLimitDate, LogDBClient::SST_RECIVE );

			rkPacket << SUBSCRIPTION_RECV_OK << dwIndex << szSubscriptionID << m_pUser->GetMoney();
			DeleteSubscriptionData( rkData.m_dwIndex, rkData.m_szSubscriptionID, DST_RECV );
		}
		return;
	case PRESENT_EXTRAITEM:
		{
			int iResult = m_pUser->SetPresentExtraItem( rkData.m_iPresentValue1,																		// 장비 코드
														( rkData.m_iPresentValue2 % PRESENT_EXTRAITEM_DIVISION_1 ) / PRESENT_EXTRAITEM_DIVISION_2,	// 장비 성장값
														rkData.m_iPresentValue2 % PRESENT_EXTRAITEM_DIVISION_2,										// 장비 기간
														rkData.m_iPresentValue2 / PRESENT_EXTRAITEM_DIVISION_1,										// 성장 촉진 타입
														0, 0,																						// 장비 스킨값
														0 );																						// 장비 맨트타입
			if( iResult == SUBSCRIPTION_RECV_OK )
			{
				g_LogDBClient.OnInsertSubscription( m_pUser, rkData.m_dwIndex, rkData.m_iPresentType, rkData.m_iPresentValue1, rkData.m_iPresentValue2,
													rkData.m_iSubscriptionGold, rkData.m_szSubscriptionID, rkData.m_dwLimitDate, LogDBClient::SST_RECIVE );
			}
			rkPacket << iResult << dwIndex << szSubscriptionID;
			DeleteSubscriptionData( rkData.m_dwIndex, rkData.m_szSubscriptionID, DST_RECV );
		}
		return;
	case PRESENT_EXTRAITEM_BOX:
		{
			int  iPesoArray = ( ( rkData.m_iPresentValue2/100 )%100 ) - 1;
			bool bCash = false;
			if( ( rkData.m_iPresentValue2%100 ) == 1 )
				bCash = true;
			int iResult = m_pUser->SetPresentExtraItemBox( rkData.m_iPresentValue1, iPesoArray, bCash );	// 장비보급함 번호
			if( iResult == SUBSCRIPTION_RECV_OK )
			{
				g_LogDBClient.OnInsertSubscription( m_pUser, rkData.m_dwIndex, rkData.m_iPresentType, rkData.m_iPresentValue1, rkData.m_iPresentValue2,
													rkData.m_iSubscriptionGold, rkData.m_szSubscriptionID, rkData.m_dwLimitDate, LogDBClient::SST_RECIVE );
			}
			rkPacket << iResult << dwIndex << szSubscriptionID;
			DeleteSubscriptionData( rkData.m_dwIndex, rkData.m_szSubscriptionID, DST_RECV );
		}
		return;
	case PRESENT_RANDOM_DECO:
		{
			int iSelectClassType = 0;
			rkRecvPacket >> iSelectClassType;

			// 보유용병 체크
			if( !m_pUser->IsClassTypeExceptExercise( iSelectClassType ) )
			{
				rkPacket << SUBSCRIPTION_RECV_EXCEPTION << dwIndex << szSubscriptionID;
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioUserSubscription::SubscriptionRecv None Char1 : %s - %d", m_pUser->GetPublicID().c_str(), rkData.m_iPresentType );
				return;
			}

			int iCharArray = m_pUser->GetCharArrayByClass( iSelectClassType );
			ioCharacter *rkChar = m_pUser->GetCharacter( iCharArray );
			if( rkChar == NULL )
			{
				rkPacket << SUBSCRIPTION_RECV_EXCEPTION << dwIndex << szSubscriptionID;
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioUserSubscription::SubscriptionRecv None Char2 : %s - %d", m_pUser->GetPublicID().c_str(), rkData.m_iPresentType );
				return;
			}

			if( !rkChar->IsActive() )
			{
				rkPacket << SUBSCRIPTION_RECV_EXCEPTION << dwIndex << szSubscriptionID;
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioUserSubscription::SubscriptionRecv None Char3 : %s - %d", m_pUser->GetPublicID().c_str(), rkData.m_iPresentType );
				return;
			}
			//

			// 선택된 용병의 치장으로 변신!
			int iItemType = iSelectClassType * 100000 + ( rkData.m_iPresentValue1 % 100000 ); // m_iPresentValue1에 class 타입이 들어갈 수 있으므로 % 100000 삭제한다. 
			int iItemCode = rkData.m_iPresentValue2;

			ITEMSLOT kSlot;
			kSlot.m_item_type = iItemType;
			kSlot.m_item_code = iItemCode;


			ioInventory *pInventory = m_pUser->GetInventory();
			if( pInventory->IsSlotItem( kSlot ) )       // 이미 있는 치장 받음
			{
				rkPacket << SUBSCRIPTION_RECV_ALREADY_DECO << dwIndex << szSubscriptionID;
				return;
			}
			else 
			{
				// 템 성별 구분...
				int iItemSexType = (rkData.m_iPresentValue1 % 100000) / 1000;
				if( iItemSexType == 1 )		// 여캐용 템이면 여캐 있는지 체크 필요
				{
					ITEMSLOT kSexSlot;
					const int iSexType = 0;
					kSexSlot.m_item_type = ( iSelectClassType*100000 ) + ( iSexType * 1000 ) + UID_KINDRED; // 해당 종족에 여자 치장
					kSexSlot.m_item_code = RDT_HUMAN_WOMAN;

					if( !pInventory->IsSlotItem( kSexSlot ) )
					{
						rkPacket << SUBSCRIPTION_RECV_EXCEPTION << dwIndex << szSubscriptionID;
						LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioUserSubscription::SubscriptionRecv None Sex : %s - %d", m_pUser->GetPublicID().c_str(), rkData.m_iPresentType );
						return;
					}
				}

				// 성별 치장은 바로 장착. 나머지는 현재 설정된 성별과 템 성별이 같을때만 장착.
				bool bEqual = false;
				if( (kSlot.m_item_type % 1000 == UID_KINDRED) || (rkChar->GetCharInfo().m_sex - 1) == iItemSexType )
					bEqual = true;

				// 성별 교체
				if( !bEqual )
				{
					ITEMSLOT kTempSlot;
					if( iItemSexType == 1 )
					{
						kTempSlot.m_item_type = ( iSelectClassType*100000 ) + UID_KINDRED;
						kTempSlot.m_item_code = RDT_HUMAN_WOMAN;
					}
					else
					{
						kTempSlot.m_item_type = ( iSelectClassType*100000 ) + UID_KINDRED;
						kTempSlot.m_item_code = RDT_HUMAN_MAN;
					}

					if( !pInventory->IsSlotItem( kTempSlot ) )
					{
						rkPacket << SUBSCRIPTION_RECV_EXCEPTION << dwIndex << szSubscriptionID;
						return;
					}

					if( rkChar->SetCharDecoration( kTempSlot.m_item_type, kTempSlot.m_item_code ) )
					{
						pInventory->SetEquipItem( kTempSlot.m_item_type, kTempSlot.m_item_code);

						const CHARACTER &rkCharInfo = rkChar->GetCharInfo();
						CHARACTER rkChangeInfo = rkCharInfo;
						pInventory->GetEquipItemCode( rkChangeInfo );
						rkChar->SetChangeKindred( rkChangeInfo, m_pUser->GetPrivateID().GetHashCode() );
					}
					else
					{
						rkPacket << SUBSCRIPTION_RECV_EXCEPTION << dwIndex << szSubscriptionID;
						return;
					}
				}

				if( pInventory->IsFull() )
				{
					PACKET_GUARD_VOID( rkPacket.Write( SUBSCRIPTION_RECV_MAX_COUNT ) );
					PACKET_GUARD_VOID( rkPacket.Write( dwIndex ) );
					PACKET_GUARD_VOID( rkPacket.Write( szSubscriptionID ) );
					return;
				}

				// 새로운 치장템 장착
				if( !rkChar->SetCharDecoration( kSlot.m_item_type, kSlot.m_item_code ) )
				{
					rkPacket << SUBSCRIPTION_RECV_EXCEPTION << dwIndex << szSubscriptionID;
					LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioUserSubscription::SubscriptionRecv SetDecoFail : %s - %d", m_pUser->GetPublicID().c_str(), rkData.m_iPresentType );
					return;
				}

				DWORD dwDecoSlotIndex = 0;
				int   iSlotArray  = 0;

				pInventory->AddSlotItem( kSlot, false, 0, LogDBClient::DT_PRESENT, dwDecoSlotIndex, iSlotArray );
	

				pInventory->SetEquipItem( kSlot.m_item_type, kSlot.m_item_code );

				if( dwDecoSlotIndex != 0 && dwDecoSlotIndex != ioInventory::NEW_INDEX )
				{
					char szItemIndex[MAX_PATH]="";
					StringCbPrintf( szItemIndex, sizeof( szItemIndex ), "%u-%u", dwDecoSlotIndex, iSlotArray+1 ); // db field는 1부터 이므로 +1
					g_LogDBClient.OnInsertDeco( m_pUser, kSlot.m_item_type, kSlot.m_item_code, 0, szItemIndex, LogDBClient::DT_PRESENT );
				}

				// 종족 치장을 구매하면 랜덤 치장 아이템을 지급한다.
				if( kSlot.m_item_type % 1000 == UID_KINDRED )
				{
					CHARACTER kCharInfo;
					kCharInfo.m_class_type = kSlot.m_item_type/100000;
					kCharInfo.m_sex        = 2; // 1남자, 2여자
					kCharInfo.m_face       = g_DecorationPrice.GetDefaultDecoCode( 1, UID_FACE, m_pUser->GetPrivateID().GetHashCode() + kCharInfo.m_class_type + UID_FACE, kCharInfo.m_class_type ); // 0남자, 1여자
					kCharInfo.m_hair       = g_DecorationPrice.GetDefaultDecoCode( 1, UID_HAIR, m_pUser->GetPrivateID().GetHashCode() + kCharInfo.m_class_type + UID_HAIR, kCharInfo.m_class_type );
					kCharInfo.m_skin_color = g_DecorationPrice.GetDefaultDecoCode( 1, UID_SKIN_COLOR, m_pUser->GetPrivateID().GetHashCode() + kCharInfo.m_class_type + UID_SKIN_COLOR, kCharInfo.m_class_type );
					kCharInfo.m_hair_color = g_DecorationPrice.GetDefaultDecoCode( 1, UID_HAIR_COLOR, m_pUser->GetPrivateID().GetHashCode() + kCharInfo.m_class_type + UID_HAIR_COLOR, kCharInfo.m_class_type );	
					kCharInfo.m_underwear  = g_DecorationPrice.GetDefaultDecoCode( 1, UID_UNDERWEAR, m_pUser->GetPrivateID().GetHashCode() + kCharInfo.m_class_type + UID_UNDERWEAR, kCharInfo.m_class_type );	
					m_pUser->SetDefaultDecoItem( kCharInfo );
					rkChar->SetCharAllDecoration( *pInventory );
				}

				g_LogDBClient.OnInsertSubscription( m_pUser, rkData.m_dwIndex, rkData.m_iPresentType, rkData.m_iPresentValue1, rkData.m_iPresentValue2,
													rkData.m_iSubscriptionGold, rkData.m_szSubscriptionID, rkData.m_dwLimitDate, LogDBClient::SST_RECIVE );

				DWORD dwCharIndex = rkChar->GetCharIndex();
				const CHARACTER &rCharinfo = rkChar->GetCharInfo();

				rkPacket << SUBSCRIPTION_RECV_OK << dwIndex << szSubscriptionID << kSlot.m_item_type << kSlot.m_item_code << dwCharIndex << (CHARACTER)rCharinfo << bEqual;

				ioUserGrowthLevel *pGrowthLevel = m_pUser->GetUserGrowthLevel();
				if( pGrowthLevel )
					pGrowthLevel->AddCharGrowthPointByDecoWoman( kSlot.m_item_type, kSlot.m_item_code );

				DeleteSubscriptionData( rkData.m_dwIndex, rkData.m_szSubscriptionID, DST_RECV );

				m_pUser->SaveInventory();
				m_pUser->SaveUserData();

				if( rkChar && m_pUser->GetMyRoom() )
				{
					m_pUser->GetMyRoom()->OnModeCharDecoUpdate( m_pUser, rkChar );
				}
			}		
		}
		return;
	case PRESENT_GRADE_EXP:
		{
			int iPrevGradeLevel = m_pUser->GetGradeLevel();
			int iPrevGradeExp   = m_pUser->GetGradeExpert();
			
			if( m_pUser->AddGradeExp( rkData.m_iPresentValue1 ) )
				m_pUser->SendGradeSync();

			g_LogDBClient.OnInsertSubscription( m_pUser, rkData.m_dwIndex, rkData.m_iPresentType, rkData.m_iPresentValue1, rkData.m_iPresentValue2,
												rkData.m_iSubscriptionGold, rkData.m_szSubscriptionID, rkData.m_dwLimitDate, LogDBClient::SST_RECIVE );

			rkPacket << SUBSCRIPTION_RECV_OK << dwIndex << szSubscriptionID << m_pUser->GetGradeLevel() << m_pUser->GetGradeExpert();
			DeleteSubscriptionData( rkData.m_dwIndex, rkData.m_szSubscriptionID, DST_RECV );
		}
		return;
	case PRESENT_MEDALITEM:
		{
			int iResult = m_pUser->SetPresentMedalItem( rkData.m_iPresentValue1,  // 메달타입
				                                        rkData.m_iPresentValue2); // 메달사용기간
			if( iResult == SUBSCRIPTION_RECV_OK )
			{
				g_LogDBClient.OnInsertSubscription( m_pUser, rkData.m_dwIndex, rkData.m_iPresentType, rkData.m_iPresentValue1, rkData.m_iPresentValue2,
													rkData.m_iSubscriptionGold, rkData.m_szSubscriptionID, rkData.m_dwLimitDate, LogDBClient::SST_RECIVE );
			}
			rkPacket << iResult << dwIndex << szSubscriptionID;
			DeleteSubscriptionData( rkData.m_dwIndex, rkData.m_szSubscriptionID, DST_RECV );
		}
		return;
	case PRESENT_ALCHEMIC_ITEM:
		{
			int iResult = m_pUser->SetPresentAlchemicItem( rkData.m_iPresentValue1,	// code
														   rkData.m_iPresentValue2 );	// count
			if( iResult == SUBSCRIPTION_RECV_OK )
			{
				g_LogDBClient.OnInsertSubscription( m_pUser, rkData.m_dwIndex, rkData.m_iPresentType, rkData.m_iPresentValue1, rkData.m_iPresentValue2,
													rkData.m_iSubscriptionGold, rkData.m_szSubscriptionID, rkData.m_dwLimitDate, LogDBClient::SST_RECIVE );
			}
			rkPacket << iResult << dwIndex << szSubscriptionID;
			DeleteSubscriptionData( rkData.m_dwIndex, rkData.m_szSubscriptionID, DST_RECV );
		}
		return;
	default:
		{
			rkPacket << SUBSCRIPTION_RECV_EXCEPTION << dwIndex << szSubscriptionID;
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioUserSubscription::SubscriptionRecv None Type : %s - %d", m_pUser->GetPublicID().c_str(), rkData.m_iPresentType );
		}
		return;
	}
}

void ioUserSubscription::SubscriptionRetr( DWORD dwIndex, const ioHashString& szSuscriptionID, int iGold, SP2Packet &rkPacket, bool bDefaultGold )
{
	if( !m_pUser )
	{
		rkPacket << SUBSCRIPTION_RETR_EXCEPTION << 1;
		return;
	}

	SubscriptionData &rkData = GetSubscriptionData( dwIndex, szSuscriptionID );
	if( rkData.m_iPresentType == 0 )
	{
		rkPacket << SUBSCRIPTION_RETR_NONE_INDEX << dwIndex << szSuscriptionID;
		return;
	}

	int iResultGold = iGold;
	if( bDefaultGold )
		iResultGold = rkData.m_iSubscriptionGold;

	g_LogDBClient.OnInsertSubscription( m_pUser, rkData.m_dwIndex, rkData.m_iPresentType, rkData.m_iPresentValue1, rkData.m_iPresentValue2,
										iResultGold, rkData.m_szSubscriptionID, rkData.m_dwLimitDate, LogDBClient::SST_DELETE );

	rkPacket << SUBSCRIPTION_RETR_OK << dwIndex << szSuscriptionID << iResultGold;
	DeleteSubscriptionData( rkData.m_dwIndex, rkData.m_szSubscriptionID, DST_RETR );

	return;
}

void ioUserSubscription::SubscriptionDisassemble( IN SP2Packet &rkRecvPacket, OUT SP2Packet &rkPacket )
{
	DWORD dwIndex;
	ioHashString szSubscriptionID;
	rkRecvPacket >> dwIndex;
	rkRecvPacket >> szSubscriptionID;

	if( !m_pUser )
	{
		rkPacket << SUBSCRIPTION_RECV_EXCEPTION << dwIndex << szSubscriptionID;
		return;
	}

	SubscriptionData &rkData = GetSubscriptionData( dwIndex, szSubscriptionID );
	if( rkData.m_iPresentType == 0 )
	{
		rkPacket << SUBSCRIPTION_RECV_NONE_INDEX << dwIndex << szSubscriptionID;
		return;
	}

	switch( rkData.m_iPresentType )
	{
	case PRESENT_SOLDIER:
		{
			int iResult = m_pUser->SetPresentCharDisassemble( rkData.m_iPresentValue1 );
			switch( iResult )
			{
			case PRESENT_RECV_ALREADY_CHAR:
				rkPacket << SUBSCRIPTION_RECV_ALREADY_CHAR;
				break;
			case PRESENT_RECV_CHAR_CREATE_DELAY:
				rkPacket << SUBSCRIPTION_RECV_CHAR_CREATE_DELAY;
				break;
			case PRESENT_RECV_CHAR_SLOT_FULL:
				rkPacket << SUBSCRIPTION_RECV_CHAR_SLOT_FULL;
				break;
			case PRESENT_DISASSEMBLE_OK:
				{
					g_LogDBClient.OnInsertSubscription( m_pUser, rkData.m_dwIndex, rkData.m_iPresentType, rkData.m_iPresentValue1, rkData.m_iPresentValue2,
														rkData.m_iSubscriptionGold, rkData.m_szSubscriptionID, rkData.m_dwLimitDate, LogDBClient::SST_RECIVE );

					SubscriptionMileageSend(rkData);

					rkPacket << SUBSCRIPTION_DISASSEMBLE_OK << dwIndex << szSubscriptionID;
					DeleteSubscriptionData( rkData.m_dwIndex, rkData.m_szSubscriptionID, DST_RECV );
				}
				break;
			}
		}
		return;	
	default:
		{
			rkPacket << SUBSCRIPTION_RECV_EXCEPTION << dwIndex << szSubscriptionID;
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioUserSubscription::SubscriptionDisassemble None Type : %s - %d", m_pUser->GetPublicID().c_str(), rkData.m_iPresentType );
		}
		return;
	}
}

void ioUserSubscription::AddSubscriptionMemory( DWORD dwUserIndex, const ioHashString &szSubscriptionID, int iSubscriptionGold,
												short iPresentType, int iPresentValue1, int iPresentValue2,
												CTime &rkLimitTime )
{
	SubscriptionData kPresent;
	kPresent.m_dwIndex = PRESENT_INDEX_MEMORY;

	kPresent.m_szSubscriptionID = szSubscriptionID;
	kPresent.m_iSubscriptionGold = iSubscriptionGold;

	kPresent.m_iPresentType    = iPresentType;
	kPresent.m_iPresentValue1  = iPresentValue1;
	kPresent.m_iPresentValue2  = iPresentValue2;
	kPresent.m_dwLimitDate	   = Help::ConvertCTimeToDate( g_ItemInitControl.CheckPresentFixedLimitDate( iPresentType, iPresentValue1, rkLimitTime ) );

	if(m_vSubscriptionList.size() < 100) 
	{
		m_vSubscriptionList.push_back( kPresent );
		m_vTempMemoryData.push_back( kPresent );
	}
}

void ioUserSubscription::SendSubscriptionMemory()
{
	if( m_pUser == NULL ) return;

	int iSendSize = m_vTempMemoryData.size();
	if( iSendSize == 0 ) return;

	SP2Packet kPacket( STPK_SUBSCRIPTION_DATA );
	kPacket << iSendSize;
	vSubscriptionData::iterator iter = m_vTempMemoryData.begin();
	for(iter = m_vTempMemoryData.begin();iter != m_vTempMemoryData.end();iter++)
	{
		SubscriptionData &kData = *iter;
		kPacket << kData.m_dwIndex
				<< kData.m_szSubscriptionID << kData.m_iSubscriptionGold;
		kPacket << kData.m_iPresentType << kData.m_iPresentValue1 << kData.m_iPresentValue2
				<< kData.m_iSubscriptionState << kData.m_dwLimitDate;
	}
	m_pUser->SendMessage( kPacket );
	m_vTempMemoryData.clear();
}

void ioUserSubscription::LogoutMemorySubscriptionInsert()
{
	if( m_pUser == NULL ) return;

	int iSubscriptionInsert = 0;
	vSubscriptionData::iterator iter = m_vSubscriptionList.begin();
	for(;iter != m_vSubscriptionList.end();iter++)
	{
		SubscriptionData &kData = *iter;
		if( kData.m_dwIndex != PRESENT_INDEX_MEMORY ) continue;
		if( kData.m_iPresentType == 0 ) continue;

		if( kData.m_dwIndex == PRESENT_INDEX_MEMORY )
		{
			iSubscriptionInsert++;

			SHORT iYear   = (SHORT)( 2000 + ( kData.m_dwLimitDate / 100000000 ) );
			SHORT iMonth  = (SHORT)( ( kData.m_dwLimitDate % 100000000 ) / 1000000 );
			SHORT iDay    = (SHORT)( ( kData.m_dwLimitDate % 1000000 ) / 10000 );
			SHORT iHour   = (SHORT)( ( kData.m_dwLimitDate % 10000 ) / 100 );
			SHORT iMinute = (SHORT)( kData.m_dwLimitDate % 100 );
			CTime cLimitDate( Help::GetSafeValueForCTimeConstructor( iYear, iMonth, iDay, iHour, iMinute, 0 ) );

			g_DBClient.OnInsertSubscriptionData( m_pUser->GetUserDBAgentID(), m_pUser->GetAgentThreadID(), m_pUser->GetUserIndex(),
												 kData.m_szSubscriptionID, kData.m_iSubscriptionGold, kData.m_iUsedBonusCash,
												 kData.m_iPresentType, kData.m_iPresentValue1, kData.m_iPresentValue2,
												 kData.m_iSubscriptionState, cLimitDate );

			// 두번 Insert 시키지 않기 위해 메모리에 있는 데이터를 삭제한다.
			kData.m_dwIndex = 0;
			kData.m_szSubscriptionID.Clear();
			kData.m_iPresentType = 0;
		}
	}
	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "LogoutMemorySubscriptionInsert : %s - %d개", m_pUser->GetPublicID().c_str(), iSubscriptionInsert );
}

void ioUserSubscription::FillMoveData( SP2Packet &rkPacket )
{
	rkPacket << m_dwLastDBIndex;

	rkPacket << (int)m_vSubscriptionList.size();
	vSubscriptionData::iterator iter = m_vSubscriptionList.begin();
	for(;iter != m_vSubscriptionList.end();iter++)
	{
		SubscriptionData &kData = *iter;		
		rkPacket << kData.m_dwIndex
				 << kData.m_szSubscriptionID << kData.m_iSubscriptionGold << kData.m_iUsedBonusCash;
		rkPacket << kData.m_iPresentType << kData.m_iPresentValue1 << kData.m_iPresentValue2
				 << kData.m_iSubscriptionState << kData.m_dwLimitDate;
	}
}

void ioUserSubscription::ApplyMoveData( SP2Packet &rkPacket, bool bDummyNode )
{
	rkPacket >> m_dwLastDBIndex;

	int iSize;
	rkPacket >> iSize;
	for(int i = 0;i < iSize;i++)
	{
		SubscriptionData kData;		
		rkPacket >> kData.m_dwIndex
				 >> kData.m_szSubscriptionID >> kData.m_iSubscriptionGold >> kData.m_iUsedBonusCash;
		rkPacket >> kData.m_iPresentType >> kData.m_iPresentValue1 >> kData.m_iPresentValue2
				 >> kData.m_iSubscriptionState >> kData.m_dwLimitDate;
		m_vSubscriptionList.push_back( kData );
	}	
}

void ioUserSubscription::SetSubscriptionState( DWORD dwIndex, const ioHashString& szSuscriptionID, short iState )
{
	int iSize = m_vSubscriptionList.size();
	for(int i = 0;i < iSize;i++)
	{
		SubscriptionData &rkData = m_vSubscriptionList[i];
		if( rkData.m_dwIndex != dwIndex ) continue;
		if( rkData.m_szSubscriptionID != szSuscriptionID ) continue;

		rkData.m_iSubscriptionState = iState;

		SHORT iYear   = (SHORT)( 2000 + ( rkData.m_dwLimitDate / 100000000 ) );
		SHORT iMonth  = (SHORT)( ( rkData.m_dwLimitDate % 100000000 ) / 1000000 );
		SHORT iDay    = (SHORT)( ( rkData.m_dwLimitDate % 1000000 ) / 10000 );
		SHORT iHour   = (SHORT)( ( rkData.m_dwLimitDate % 10000 ) / 100 );
		SHORT iMinute = (SHORT)( rkData.m_dwLimitDate % 100 );

		CTime cLimitDate( Help::GetSafeValueForCTimeConstructor( iYear, iMonth, iDay, iHour, iMinute, 0 ) );

		g_DBClient.OnSubseriptionDataUpdate( m_pUser->GetUserDBAgentID(), m_pUser->GetAgentThreadID(),
											 rkData.m_dwIndex, rkData.m_iPresentType, rkData.m_iPresentValue1, rkData.m_iPresentValue2,
											 rkData.m_szSubscriptionID, rkData.m_iSubscriptionGold, rkData.m_iUsedBonusCash, rkData.m_iSubscriptionState, cLimitDate );


		return;
	}
}

int ioUserSubscription::GetSubscriptionDataCnt()
{
	return m_vSubscriptionList.size();
}

void ioUserSubscription::SetRetractGold( DWORD dwIndex, const ioHashString& szSuscriptionID, int iGold )
{
	int iSize = m_vSubscriptionList.size();
	for(int i = 0;i < iSize;i++)
	{
		SubscriptionData &rkData = m_vSubscriptionList[i];
		if( rkData.m_dwIndex != dwIndex ) continue;
		if( rkData.m_szSubscriptionID != szSuscriptionID ) continue;

		rkData.m_iRetractGold = iGold;
		return;
	}

	return;
}

int ioUserSubscription::GetSubscriptionGold( DWORD dwIndex, const ioHashString& szSuscriptionID )
{
	int iSize = m_vSubscriptionList.size();
	for(int i = 0;i < iSize;i++)
	{
		SubscriptionData &rkData = m_vSubscriptionList[i];
		if( rkData.m_dwIndex != dwIndex ) continue;
		if( rkData.m_szSubscriptionID != szSuscriptionID ) continue;
		
		return rkData.m_iSubscriptionGold;
	}

	return 0;
}

int ioUserSubscription::GetRetractGold( DWORD dwIndex, const ioHashString& szSuscriptionID )
{
	int iSize = m_vSubscriptionList.size();
	for(int i = 0;i < iSize;i++)
	{
		SubscriptionData &rkData = m_vSubscriptionList[i];
		if( rkData.m_dwIndex != dwIndex ) continue;
		if( rkData.m_szSubscriptionID != szSuscriptionID ) continue;
		
		return rkData.m_iRetractGold;
	}

	return 0;
}

int ioUserSubscription::GetDBRetractGold( DWORD dwIndex, const ioHashString& szSuscriptionID )
{
	int iSize = m_vSubscriptionList.size();
	for(int i = 0;i < iSize;i++)
	{
		SubscriptionData &rkData = m_vSubscriptionList[i];
		if( rkData.m_dwIndex != dwIndex ) continue;
		if( rkData.m_szSubscriptionID != szSuscriptionID ) continue;

		return rkData.m_iSubscriptionGold;
	}

	return 0;
}

int ioUserSubscription::GetUsedBonusCash(DWORD dwIndex, const ioHashString& szSuscriptionID )
{
	int iSize = m_vSubscriptionList.size();
	for(int i = 0;i < iSize;i++)
	{
		SubscriptionData &rkData = m_vSubscriptionList[i];
		if( rkData.m_dwIndex != dwIndex ) continue;
		if( rkData.m_szSubscriptionID != szSuscriptionID ) continue;

		return rkData.m_iUsedBonusCash;
	}

	return 0;
}
