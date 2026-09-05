#include "stdafx.h"

#include "../DataBase/DBClient.h"
#include "../DataBase/LogDBClient.h"
#include "../EtcHelpFunc.h"
//#include "../Window.h"
#include "../MainProcess.h"

#include "User.h"
#include "UserNodeManager.h"
#include "ServerNodeManager.h"
#include "ioItemInitializeControl.h"
#include <strsafe.h>

template<> ioItemInitializeControl* Singleton< ioItemInitializeControl >::ms_Singleton = 0;
ioItemInitializeControl::ioItemInitializeControl()
{
	m_dwInitProcessCheckMs	= 0;
	m_dwCurrentTime			= 0;

	for( int i = 0 ; i < MAX_COMMON ; ++i )
	{
		m_stItemInitControl[ i ].Init();
	}
}

ioItemInitializeControl::~ioItemInitializeControl()
{
}

ioItemInitializeControl& ioItemInitializeControl::GetSingleton()
{
	return Singleton< ioItemInitializeControl >::GetSingleton();
}

void ioItemInitializeControl::LoadINIData()
{
	for( int i = 0 ; i < MAX_COMMON ; ++i )
	{
		m_stItemInitControl[ i ].Init();
	}

	char szTitle[MAX_PATH] = "";

	ioINILoader kLoader;
	kLoader.ReloadFile( "config/sp2_item_init_control.ini" );

	for ( int count = 0 ; count < 1000 ; count++ )
	{
		for( int loop = 0 ; loop < MAX_COMMON ; ++loop )
		{
			//기존 INI설정과의 일관성 유지를 위해서 count 0일때와 아닐때를 분리하여 처리
			if ( count == 0 )
			{
				if( loop == ETC_COMMON )
					sprintf_s( szTitle, "etc_common" );
				else if( loop == CLOVER_COMMON )
					sprintf_s( szTitle, "clover_common" );
				else if( loop == MILEAGE_COMMON )
					sprintf_s( szTitle, "mileage_common" );
			}
			else
			{
				if( loop == ETC_COMMON )
					sprintf_s( szTitle, "etc_common%d", count + 1 );
				else if( loop == CLOVER_COMMON ) 
					sprintf_s( szTitle, "clover_common%d", count + 1 );
				else if( loop == MILEAGE_COMMON )
					sprintf_s( szTitle, "mileage_common%d", count + 1 );
			}

			kLoader.SetTitle(szTitle);
			if ( kLoader.LoadInt( "init_year", 0 ) == 0 ) continue;

			m_stItemInitControl[ loop ].m_dwInitYear  = kLoader.LoadInt( "init_year", 0 );
			m_stItemInitControl[ loop ].m_dwInitMonth = kLoader.LoadInt( "init_month", 0 );
			m_stItemInitControl[ loop ].m_dwInitDay   = kLoader.LoadInt( "init_day", 0 );
			m_stItemInitControl[ loop ].m_dwInitHour  = kLoader.LoadInt( "init_hour", 0 );
			m_stItemInitControl[ loop ].m_dwRotateDay = kLoader.LoadInt( "init_rotate_day", 0 );
			m_dwInitProcessCheckMs = kLoader.LoadInt( "init_process_check_ms", 0 );

			int iMaxCnt = kLoader.LoadInt( "max_cnt", 0 );
			for(int i = 0;i < iMaxCnt;i++)
			{
				char szKey[MAX_PATH] = "";
				sprintf_s( szKey, "etcitem_%d", i + 1 );

				m_stItemInitControl[ loop ].m_InitEtcItemList.push_back( kLoader.LoadInt( szKey, 0 ) );
			}

			if( m_stItemInitControl[ loop ].m_dwInitYear == 0 ||
				m_stItemInitControl[ loop ].m_dwRotateDay == 0 ||
				m_stItemInitControl[ loop ].m_dwInitHour == 0 )
				return;

			LOOP_GUARD();
			CTime cCurTime = CTime::GetCurrentTime();
			CTime cBeforeTime = CTime( Help::GetSafeValueForCTimeConstructor( m_stItemInitControl[ loop ].m_dwInitYear,
				m_stItemInitControl[ loop ].m_dwInitMonth,
				m_stItemInitControl[ loop ].m_dwInitDay,
				m_stItemInitControl[ loop ].m_dwInitHour, 0, 0 ) );	
			CTimeSpan cAddDay( m_stItemInitControl[ loop ].m_dwRotateDay, 0, 0, 0 );
			while( true )
			{
				if( cCurTime < cBeforeTime + cAddDay )
					break;

				cBeforeTime += cAddDay;
			}
			LOOP_GUARD_CLEAR();
			m_stItemInitControl[ loop ].m_BeforeTime = cBeforeTime;
			m_stItemInitControl[ loop ].m_AfterTime  = m_stItemInitControl[ loop ].m_BeforeTime + cAddDay;

			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[info][Initcontrol]Load INI data() : Before[%d.%d.%d.%d] - After[%d.%d.%d.%d]", 
				m_stItemInitControl[ loop ].m_BeforeTime.GetYear(), m_stItemInitControl[ loop ].m_BeforeTime.GetMonth(),
				m_stItemInitControl[ loop ].m_BeforeTime.GetDay(), m_stItemInitControl[ loop ].m_BeforeTime.GetHour(),
				m_stItemInitControl[ loop ].m_AfterTime.GetYear(), m_stItemInitControl[ loop ].m_AfterTime.GetMonth(),
				m_stItemInitControl[ loop ].m_AfterTime.GetDay(), m_stItemInitControl[ loop ].m_AfterTime.GetHour() );
		}
	}
}

void ioItemInitializeControl::Process()
{
	if( TIMEGETTIME() - m_dwCurrentTime < m_dwInitProcessCheckMs ) return;

	m_dwCurrentTime = TIMEGETTIME();

	for( int loopType = 0 ; loopType < MAX_COMMON ; ++loopType )
	{
		if( m_stItemInitControl[ loopType ].m_dwRotateDay > 0 )
		{
			if( CTime::GetCurrentTime() > m_stItemInitControl[ loopType ].m_AfterTime )
			{
				CheckRotateDate( loopType );

				// 타서버에도 알려서 초기화
				SP2Packet kPacket( SSTPK_EVENT_ITEM_INITIALIZE );
				kPacket << loopType;
				g_ServerNodeManager.SendMessageAllNode( kPacket );

				// 접속해있는 유저들 초기화
				g_UserNodeManager.InitUserEventItem( loopType );
			}
		}
	}
}

void ioItemInitializeControl::CheckRotateDate( const int iType )
{
	CTimeSpan cAddDay( m_stItemInitControl[ iType ].m_dwRotateDay, 0, 0, 0 );
	m_stItemInitControl[ iType ].m_BeforeTime += cAddDay;
	m_stItemInitControl[ iType ].m_AfterTime  = m_stItemInitControl[ iType ].m_BeforeTime + cAddDay;

	ioINILoader kLoader( "config/sp2_item_init_control.ini" );
	if( iType == ETC_COMMON )
		kLoader.SetTitle( "etc_common" );
	else if( iType == CLOVER_COMMON )
		kLoader.SetTitle( "clover_common" );
	else if( iType == MILEAGE_COMMON )
		kLoader.SetTitle("mileage_common");

	kLoader.SaveInt( "init_year", m_stItemInitControl[ iType ].m_BeforeTime.GetYear() );
	kLoader.SaveInt( "init_month", m_stItemInitControl[ iType ].m_BeforeTime.GetMonth() );
	kLoader.SaveInt( "init_day", m_stItemInitControl[ iType ].m_BeforeTime.GetDay() );
	kLoader.SaveInt( "init_hour", m_stItemInitControl[ iType ].m_BeforeTime.GetHour() );

	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioItemInitializeControl::CheckRotateDate : Before(%d.%d.%d.%d) - After(%d.%d.%d.%d)", 
							m_stItemInitControl[ iType ].m_BeforeTime.GetYear(), m_stItemInitControl[ iType ].m_BeforeTime.GetMonth(),
							m_stItemInitControl[ iType ].m_BeforeTime.GetDay(), m_stItemInitControl[ iType ].m_BeforeTime.GetHour(),
							m_stItemInitControl[ iType ].m_AfterTime.GetYear(), m_stItemInitControl[ iType ].m_AfterTime.GetMonth(),
							m_stItemInitControl[ iType ].m_AfterTime.GetDay(), m_stItemInitControl[ iType ].m_AfterTime.GetHour() );
}

//! Etc_Common
void ioItemInitializeControl::CheckInitUserEtcItemByLogin( User *pUser )
{
	if( !pUser ) return;
	if( m_stItemInitControl[ ETC_COMMON ].m_dwRotateDay == 0 ) return;

	DWORD dwInitTime = Help::ConvertCTimeToDate( m_stItemInitControl[ ETC_COMMON ].m_BeforeTime );
	DWORD dwConnectTime = Help::ConvertCTimeToDate( pUser->GetLastLogOutTime() );

	if( dwConnectTime <= dwInitTime )
	{
		// 초기화
		ioUserEtcItem *pUserEtcItem = pUser->GetUserEtcItem();
		if( pUserEtcItem )
		{
			for(int i = 0;i < (int)m_stItemInitControl[ ETC_COMMON ].m_InitEtcItemList.size();i++)
			{
				 pUserEtcItem->DeleteEtcItem( m_stItemInitControl[ ETC_COMMON ].m_InitEtcItemList[i], LogDBClient::ET_DATE_DEL );
			}
		}
	}
}

//! Mileage_Common
void ioItemInitializeControl::CheckInitUserMileageByLogin( User *pUser )
{
	if( !pUser ) return;
	if( m_stItemInitControl[ MILEAGE_COMMON ].m_dwRotateDay == 0 ) return;

	DWORD dwInitTime = Help::ConvertCTimeToDate( m_stItemInitControl[ MILEAGE_COMMON ].m_BeforeTime );
	DWORD dwConnectTime = Help::ConvertCTimeToDate( pUser->GetLastLogOutTime() );

	if( dwConnectTime <= dwInitTime )
	{
		// 초기화
		ioUserEtcItem *pUserEtcItem = pUser->GetUserEtcItem();
		if( pUserEtcItem )
		{
			for(int i = 0;i < (int)m_stItemInitControl[ MILEAGE_COMMON ].m_InitEtcItemList.size();i++)
			{
				pUserEtcItem->DeleteEtcItem( m_stItemInitControl[ MILEAGE_COMMON ].m_InitEtcItemList[i], LogDBClient::ET_DATE_DEL );
			}
		}
	}
}

void ioItemInitializeControl::CheckInitUserEtcItemByPlayer( User *pUser, const int iType )
{
	if( !pUser ) return;
	if( m_stItemInitControl[ iType ].m_dwRotateDay == 0 ) return;

	DWORD dwInitTime = Help::ConvertCTimeToDate( m_stItemInitControl[ iType ].m_BeforeTime );
	DWORD dwConnectTime = Help::ConvertCTimeToDate( pUser->GetLastLogOutTime() );

	if( dwConnectTime <= dwInitTime )
	{
		// 특별 초기화
		ioUserEtcItem *pUserEtcItem = pUser->GetUserEtcItem();
		if( pUserEtcItem )
		{
			for(int i = 0;i < (int)m_stItemInitControl[ iType ].m_InitEtcItemList.size();i++)
			{
				if( pUserEtcItem->DeleteEtcItem( m_stItemInitControl[ iType ].m_InitEtcItemList[i], LogDBClient::ET_DATE_DEL ) )
				{
					SP2Packet kReturn( STPK_ETCITEM_USE );
					kReturn << ETCITEM_USE_OK;
					kReturn << m_stItemInitControl[ iType ].m_InitEtcItemList[i];
					kReturn << 0 << 0;
					pUser->SendMessage( kReturn );
				}
			}
		}

		if( iType == ETC_COMMON )
		{
			// 선물 기간 체크
			ioUserPresent *pUserPresent = pUser->GetUserPresent();
			if( pUserPresent )
			{
				pUserPresent->CheckPresentLimitDate();
			}

			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Call [ETC_COMMON] CheckInitUserEtcItemByPlayer : %s", pUser->GetPublicID().c_str() );
		}
		else if( iType == CLOVER_COMMON )
		{
			// Get Friend
			ioFriend* pFriend = pUser->GetFriend();

			// 받을 수 있는 클로버 갯수 초기화.
			pFriend->InitFriendBeforeReceiveCloverCount();

			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Call [CLOVER_COMMON] CheckInitUserEtcItemByPlayer : %s", pUser->GetPublicID().c_str() );
		}
		else if( iType == MILEAGE_COMMON )
		{
			ioUserPresent *pUserPresent = pUser->GetUserPresent();
			if( pUserPresent )
			{
				pUserPresent->CheckPresentLimitDate();
			}
		}
	}
}

void ioItemInitializeControl::CheckPresentFixedLimitDate( short iPresentType, int iPresentValue1, char *szReturn )
{
	if( m_stItemInitControl[ ETC_COMMON ].m_dwRotateDay == 0 )
		return;
	
	if( iPresentType == PRESENT_ETC_ITEM )
	{
		for( int loop = 0 ; loop < MAX_COMMON ; ++loop )
		{
			for(int i = 0;i < (int)m_stItemInitControl[ loop ].m_InitEtcItemList.size();i++)
			{
				if( m_stItemInitControl[ loop ].m_InitEtcItemList[i] == (DWORD)iPresentValue1 )
				{
					sprintf_s( szReturn, sizeof(szReturn), "\'%d-%d-%d %d:%d:%d\'",
						m_stItemInitControl[ loop ].m_AfterTime.GetYear(), m_stItemInitControl[ loop ].m_AfterTime.GetMonth(),
						m_stItemInitControl[ loop ].m_AfterTime.GetDay(), m_stItemInitControl[ loop ].m_AfterTime.GetHour(), 
						m_stItemInitControl[ loop ].m_AfterTime.GetMinute(), m_stItemInitControl[ loop ].m_AfterTime.GetSecond() );
					return;
				}
			}
		}
	}
}

CTime ioItemInitializeControl::CheckPresentFixedLimitDate( short iPresentType, int iPresentValue1, CTime &rkLimitDate )
{
	if( m_stItemInitControl[ ETC_COMMON ].m_dwRotateDay == 0 )
		return rkLimitDate;

	if( iPresentType == PRESENT_ETC_ITEM )
	{
		for( int loop = 0 ; loop < MAX_COMMON ; ++loop )
		{
			for(int i = 0;i < (int)m_stItemInitControl[ loop ].m_InitEtcItemList.size();i++)
			{
				if( m_stItemInitControl[ loop ].m_InitEtcItemList[i] == (DWORD)iPresentValue1 )
				{
					return m_stItemInitControl[ loop ].m_AfterTime;
				}
			}
		}
	}
	return rkLimitDate;
}

//! etc Clover Item
void ioItemInitializeControl::CheckInitUserCloverItemByLogin( User* pUser )
{
	if( !pUser ) return;
	if( m_stItemInitControl[ CLOVER_COMMON ].m_dwRotateDay == 0 ) return;

	DWORD dwInitTime = Help::ConvertCTimeToDate( m_stItemInitControl[ CLOVER_COMMON ].m_BeforeTime );
	DWORD dwConnectTime = Help::ConvertCTimeToDate( pUser->GetLastLogOutTime() );

	if( dwConnectTime <= dwInitTime )
	{
		// etc 클로버 아이템 초기화.
		ioUserEtcItem *pUserEtcItem = pUser->GetUserEtcItem();
		if( pUserEtcItem )
		{
			for(int i = 0;i < (int)m_stItemInitControl[ CLOVER_COMMON ].m_InitEtcItemList.size();i++)
			{
				 pUserEtcItem->DeleteEtcItem( m_stItemInitControl[ CLOVER_COMMON ].m_InitEtcItemList[i], LogDBClient::ET_DATE_DEL );
			}
		}
	}
}

//! Before Receive Clover Count
bool ioItemInitializeControl::CheckInitUserBeforeReceiveCloverByLogin( User* pUser )
{
	if( !pUser )
		return false;
	if( m_stItemInitControl[ CLOVER_COMMON ].m_dwRotateDay == 0 )
		return false;

	DWORD dwInitTime = Help::ConvertCTimeToDate( m_stItemInitControl[ CLOVER_COMMON ].m_BeforeTime );
	DWORD dwConnectTime = Help::ConvertCTimeToDate( pUser->GetLastLogOutTime() );

	if( dwConnectTime <= dwInitTime )
	{
		return true;
	}
	return false;
}

bool ioItemInitializeControl::IsDeletePresentCheck( short iPresentType, int iPresentValue1 )
{
	if( m_stItemInitControl[ ETC_COMMON ].m_dwRotateDay == 0 )
		return false;

	if( iPresentType == PRESENT_ETC_ITEM )
	{
		for( int loop = 0 ; loop < MAX_COMMON ; ++loop )
		{
			for(int i = 0;i < (int)m_stItemInitControl[ loop ].m_InitEtcItemList.size();i++)
			{
				if( m_stItemInitControl[ loop ].m_InitEtcItemList[i] == (DWORD)iPresentValue1 )
				{
					return true;
				}
			}
		}
	}
	return false;
}