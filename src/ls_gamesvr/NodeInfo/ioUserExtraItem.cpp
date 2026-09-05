#include "stdafx.h"

//#include "../Window.h"
#include "../MainProcess.h"
#include "../QueryData/QueryResultData.h"
#include "../DataBase/DBClient.h"
#include "../DataBase/LogDBClient.h"
#include "../Util/cSerialize.h"
#include "../EtcHelpFunc.h"

#include "User.h"
#include "Room.h"
#include "ioEtcItemManager.h"
#include "ioExtraItemInfoManager.h"
#include "ioUserExtraItem.h"
#include <strsafe.h>

ioUserExtraItem::ioUserExtraItem()
{
	Initialize( NULL );
}

ioUserExtraItem::~ioUserExtraItem()
{
	m_vExtraItemList.clear();
}

void ioUserExtraItem::Initialize( User *pUser )
{
	m_pUser = pUser;

	m_iCurMaxIndex			= 0;
	m_iCurPossessionCount	= 0;
	m_iMaxPossessionCount	= 0;
	m_vExtraItemList.clear();
}

void ioUserExtraItem::InsertDBExtraItem( EXTRAITEMDB &kExtraItemDB, bool bBuyCash, int iBuyPrice, int iLogType, int iMachineCode, int iPeriodTime )
{
	if( !m_pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioUserExtraItem::InsertDBEtcItem() User NULL!!"); 
		return;
	}

	cSerialize v_FT;
	v_FT.Write( m_pUser->GetUserIndex() );
	for(int i = 0;i < MAX_SLOT;i++)
	{
		v_FT.Write( kExtraItemDB.m_ExtraItem[i].m_iItemCode );
		v_FT.Write( kExtraItemDB.m_ExtraItem[i].m_iReinforce );
		v_FT.Write( kExtraItemDB.m_ExtraItem[i].m_iIndex );
		v_FT.Write( kExtraItemDB.m_ExtraItem[i].m_iTradeState );
		v_FT.Write( kExtraItemDB.m_ExtraItem[i].m_PeriodType );
		v_FT.Write( kExtraItemDB.m_ExtraItem[i].m_dwMaleCustom );
		v_FT.Write( kExtraItemDB.m_ExtraItem[i].m_dwFemaleCustom );
		v_FT.Write( kExtraItemDB.m_ExtraItem[i].m_dwFailExp );

		SYSTEMTIME sysTime;
		kExtraItemDB.m_ExtraItem[i].GetDate( sysTime );
		v_FT.Write( (uint8*)(&sysTime), sizeof(SYSTEMTIME), TRUE );
	}

	g_DBClient.OnInsertExtraItemData( 
		m_pUser->GetUserDBAgentID(),
		m_pUser->GetAgentThreadID(),
		m_pUser->GetGUID(), 
		m_pUser->GetPublicID(), 
		m_pUser->GetUserIndex(), 
		v_FT, 
		bBuyCash, iBuyPrice, iLogType, iMachineCode, iPeriodTime );
}

bool ioUserExtraItem::DBtoNewIndex( DWORD dwIndex )
{
	if( !m_pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioUserExtraItem::DBtoNewIndex() User NULL!!"); 
		return false;
	}
	
	{	// 빈 인덱스 없으면 패스
		bool bEmptyIndex = false;
		vEXTRAITEMDB::iterator iter, iEnd;
		iEnd = m_vExtraItemList.end();
		for(iter = m_vExtraItemList.begin();iter != iEnd;iter++)
		{
			EXTRAITEMDB &kExtraItemDB = *iter;
			if( kExtraItemDB.m_dwIndex == NEW_INDEX )
			{
				bEmptyIndex = true;
				break;
			}
		}

		if( !bEmptyIndex )
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioUserExtraItem::DBtoNewIndex() None Empty Index : %s - %d", m_pUser->GetPublicID().c_str(), dwIndex  ); 
			return false;
		}
	}

	{	// 이미 보유하고 있는 인덱스라면 다시 가져온다.
		vEXTRAITEMDB::iterator iter, iEnd;
		iEnd = m_vExtraItemList.end();
		for(iter = m_vExtraItemList.begin();iter != iEnd;iter++)
		{
			EXTRAITEMDB &kExtraItemDB = *iter;
			if( kExtraItemDB.m_dwIndex == dwIndex )
			{
				g_DBClient.OnSelectExtraItemIndex( m_pUser->GetUserDBAgentID(), m_pUser->GetAgentThreadID(), m_pUser->GetGUID(), m_pUser->GetPublicID(), m_pUser->GetUserIndex(), false, 0, 0, 0, 0 );
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioUserExtraItem::DBtoNewIndex() Already Index : %s - %d", m_pUser->GetPublicID().c_str(), dwIndex  );
				return false;
			}
		}
	}

	{   // 빈 인덱스에 받은 인덱스 적용
		vEXTRAITEMDB::iterator iter, iEnd;
		iEnd = m_vExtraItemList.end();
		for(iter = m_vExtraItemList.begin();iter != iEnd;iter++)
		{
			EXTRAITEMDB &kExtraItemDB = *iter;
			if( kExtraItemDB.m_dwIndex == NEW_INDEX )
			{
				kExtraItemDB.m_dwIndex = dwIndex;
				LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "ioUserExtraItem::DBtoNewIndex() Add : %s - %d", m_pUser->GetPublicID().c_str(), dwIndex  );
				return true;
			}
		}
	}

	return false;
}

void ioUserExtraItem::DBtoData( CQueryResultData *query_data )
{
	if( !m_pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioUserExtraItem::DBtoData() User NULL!!"); 
		return;
	}

	Initialize( m_pUser );

	LOOP_GUARD();
	while( query_data->IsExist() )
	{		
		EXTRAITEMDB kExtraItemDB;
		PACKET_GUARD_BREAK( query_data->GetValue( kExtraItemDB.m_dwIndex, sizeof(int) ) );
		for(int i = 0;i < MAX_SLOT;i++)
		{
			DBTIMESTAMP dts;

			PACKET_GUARD_BREAK( query_data->GetValue( kExtraItemDB.m_ExtraItem[i].m_iItemCode, sizeof(int) ) );
			PACKET_GUARD_BREAK( query_data->GetValue( kExtraItemDB.m_ExtraItem[i].m_iReinforce, sizeof(int) ) );
			PACKET_GUARD_BREAK( query_data->GetValue( kExtraItemDB.m_ExtraItem[i].m_iIndex, sizeof(int) ) );
			PACKET_GUARD_BREAK( query_data->GetValue( kExtraItemDB.m_ExtraItem[i].m_iTradeState, sizeof(int) ) );
			PACKET_GUARD_BREAK( query_data->GetValue( kExtraItemDB.m_ExtraItem[i].m_PeriodType, sizeof(int) ) );
			PACKET_GUARD_BREAK( query_data->GetValue( kExtraItemDB.m_ExtraItem[i].m_dwMaleCustom, sizeof(DWORD) ) );
			PACKET_GUARD_BREAK( query_data->GetValue( kExtraItemDB.m_ExtraItem[i].m_dwFemaleCustom, sizeof(DWORD) ) );
			PACKET_GUARD_BREAK( query_data->GetValue( kExtraItemDB.m_ExtraItem[i].m_dwFailExp, sizeof(short) ) );
			PACKET_GUARD_BREAK( query_data->GetValue( (char*)&dts, sizeof(DBTIMESTAMP) ) );
			CTime kLimitTime(Help::GetSafeValueForCTimeConstructor(dts.year,dts.month,dts.day, dts.hour,dts.minute,dts.second));
			kExtraItemDB.m_ExtraItem[i].SetDate( kLimitTime.GetYear(), kLimitTime.GetMonth(), kLimitTime.GetDay(), kLimitTime.GetHour(), kLimitTime.GetMinute() );
		
			m_iCurMaxIndex = max( m_iCurMaxIndex, kExtraItemDB.m_ExtraItem[i].m_iIndex );
		} 
		m_vExtraItemList.push_back( kExtraItemDB );
	}	
	LOOP_GUARD_CLEAR();
	if( m_vExtraItemList.empty() ) return;
	
	g_CriticalError.CheckExtraItemTableCount( m_pUser->GetPublicID(), m_vExtraItemList.size() );

	int iItemSize = 0;
	vEXTRAITEMDB::iterator iter, iEnd;
	iEnd = m_vExtraItemList.end();
	for(iter = m_vExtraItemList.begin();iter != iEnd;iter++)
	{
		EXTRAITEMDB &kExtraItemDB = *iter;
		for(int i = 0;i < MAX_SLOT;i++)
		{
			if( kExtraItemDB.m_ExtraItem[i].m_iItemCode > 0 )
			{
				iItemSize++;
			}
		}
	}

	SP2Packet kPacket( STPK_USER_EXTRAITEM_DATA );
	PACKET_GUARD_VOID ( kPacket.Write(iItemSize) );
	{
		iEnd = m_vExtraItemList.end();
		for(iter = m_vExtraItemList.begin();iter != iEnd;iter++)
		{
			EXTRAITEMDB &kExtraItemDB = *iter;
			for(int i = 0;i < MAX_SLOT;i++)
			{
				if( kExtraItemDB.m_ExtraItem[i].m_iItemCode <= 0 )
					continue;

				PACKET_GUARD_VOID( kPacket.Write(kExtraItemDB.m_ExtraItem[i].m_iItemCode ));
				PACKET_GUARD_VOID( kPacket.Write(kExtraItemDB.m_ExtraItem[i].m_iReinforce ));
				PACKET_GUARD_VOID( kPacket.Write(kExtraItemDB.m_ExtraItem[i].m_iIndex ));
				PACKET_GUARD_VOID( kPacket.Write(kExtraItemDB.m_ExtraItem[i].m_iTradeState ));
				PACKET_GUARD_VOID( kPacket.Write(kExtraItemDB.m_ExtraItem[i].m_PeriodType ));
				PACKET_GUARD_VOID( kPacket.Write(kExtraItemDB.m_ExtraItem[i].m_iValue1 ));
				PACKET_GUARD_VOID( kPacket.Write(kExtraItemDB.m_ExtraItem[i].m_iValue2 ));
				PACKET_GUARD_VOID( kPacket.Write(kExtraItemDB.m_ExtraItem[i].m_dwMaleCustom ));
				PACKET_GUARD_VOID( kPacket.Write(kExtraItemDB.m_ExtraItem[i].m_dwFemaleCustom ));
				PACKET_GUARD_VOID( kPacket.Write(kExtraItemDB.m_ExtraItem[i].m_dwFailExp ));
			}
		}
	}	
	m_pUser->SendMessage( kPacket );
}

void ioUserExtraItem::SaveData()
{
	if( m_vExtraItemList.empty() )
		return;

	if( !m_pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioUserExtraItem::SaveData() User NULL!!"); 
		return;
	}

	cSerialize v_FT;

	vEXTRAITEMDB::iterator iter, iEnd;
	iEnd = m_vExtraItemList.end();
	for(iter = m_vExtraItemList.begin();iter != iEnd;iter++)
	{
		EXTRAITEMDB &kExtraItemDB = *iter;
		if( kExtraItemDB.m_bChange )
		{
			v_FT.Reset();
			v_FT.Write( kExtraItemDB.m_dwIndex );
		
			for(int i = 0;i < MAX_SLOT;i++)
			{
				v_FT.Write( kExtraItemDB.m_ExtraItem[i].m_iItemCode );
				v_FT.Write( kExtraItemDB.m_ExtraItem[i].m_iReinforce );
				v_FT.Write( kExtraItemDB.m_ExtraItem[i].m_iIndex );
				v_FT.Write( kExtraItemDB.m_ExtraItem[i].m_iTradeState );
				v_FT.Write( kExtraItemDB.m_ExtraItem[i].m_PeriodType );
				v_FT.Write( kExtraItemDB.m_ExtraItem[i].m_dwMaleCustom );
				v_FT.Write( kExtraItemDB.m_ExtraItem[i].m_dwFemaleCustom );
				v_FT.Write( kExtraItemDB.m_ExtraItem[i].m_dwFailExp );
				SYSTEMTIME sysTime;
				kExtraItemDB.m_ExtraItem[i].GetDate( sysTime );
				v_FT.Write( (uint8*)(&sysTime), sizeof(SYSTEMTIME), TRUE );
			}

			if( kExtraItemDB.m_dwIndex == NEW_INDEX )
			{
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "SaveExtraItem(%s:%d) None Index", m_pUser->GetPublicID().c_str(), kExtraItemDB.m_dwIndex );
				//LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s", szContent );
			}
			else
			{
				g_DBClient.OnUpdateExtraItemData( m_pUser->GetUserDBAgentID(), m_pUser->GetAgentThreadID(), m_pUser->GetPublicID(), kExtraItemDB.m_dwIndex, v_FT );

				kExtraItemDB.m_bChange = false;
				LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "SaveExtraItem(%s:%d)", m_pUser->GetPublicID().c_str(), kExtraItemDB.m_dwIndex );
			}
		}		

	}
}

int ioUserExtraItem::AddExtraItem( IN const EXTRAITEMSLOT &rkNewSlot, IN bool bBuyCash, IN int iBuyPrice, IN int iLogType, IN int iMachineCode, IN int iPeriodTime, OUT DWORD &rdwIndex, OUT int &riArray )
{
	if( rkNewSlot.m_iItemCode == 0 )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioUserExtraItem::AddExtraItem Code is Zero" );
		return 0;
	}
	
	vEXTRAITEMDB::iterator iter, iEnd;
	iEnd = m_vExtraItemList.end();

	// update blank
	for(iter = m_vExtraItemList.begin();iter != iEnd;iter++)
	{
		EXTRAITEMDB &kExtraItemDB = *iter;
		for(int i = 0;i < MAX_SLOT;i++)
		{
			if( kExtraItemDB.m_ExtraItem[i].m_iItemCode == 0 &&
				kExtraItemDB.m_ExtraItem[i].m_iIndex == 0 )
			{
				m_iCurMaxIndex++;

				kExtraItemDB.m_ExtraItem[i] = rkNewSlot;
				kExtraItemDB.m_ExtraItem[i].m_iIndex = m_iCurMaxIndex;
				kExtraItemDB.m_bChange    = true;

				rdwIndex = kExtraItemDB.m_dwIndex;
				riArray  = i;

				AddCurPossessionCount(1);

				return kExtraItemDB.m_ExtraItem[i].m_iIndex;
			}
		}
	}

	// New insert
	m_iCurMaxIndex++;

	EXTRAITEMDB kExtraItemDB;
	kExtraItemDB.m_dwIndex     = NEW_INDEX;
	kExtraItemDB.m_ExtraItem[0]  = rkNewSlot;
	kExtraItemDB.m_ExtraItem[0].m_iIndex = m_iCurMaxIndex;
	m_vExtraItemList.push_back( kExtraItemDB );
	
	InsertDBExtraItem( kExtraItemDB, bBuyCash, iBuyPrice, iLogType, iMachineCode, iPeriodTime );
	
	if( m_pUser )
	{
		g_CriticalError.CheckExtraItemTableCount( m_pUser->GetPublicID(), m_vExtraItemList.size() );
	}

	AddCurPossessionCount(1);

	return kExtraItemDB.m_ExtraItem[0].m_iIndex;
}

void ioUserExtraItem::FillMoveData( SP2Packet &rkPacket )
{
	PACKET_GUARD_VOID( rkPacket.Write((int)m_vExtraItemList.size()) );

	vEXTRAITEMDB::iterator iter, iEnd;
	iEnd = m_vExtraItemList.end();
	for(iter = m_vExtraItemList.begin();iter != iEnd;iter++)
	{
		EXTRAITEMDB &kExtraItemDB = *iter;

		PACKET_GUARD_BREAK( rkPacket.Write( kExtraItemDB.m_dwIndex ) );
		PACKET_GUARD_BREAK( rkPacket.Write( kExtraItemDB.m_bChange ) );

		for(int i = 0;i < MAX_SLOT;i++)
		{
			PACKET_GUARD_BREAK( rkPacket.Write( kExtraItemDB.m_ExtraItem[i].m_iItemCode ) );
			PACKET_GUARD_BREAK( rkPacket.Write( kExtraItemDB.m_ExtraItem[i].m_iReinforce ) );
			PACKET_GUARD_BREAK( rkPacket.Write( kExtraItemDB.m_ExtraItem[i].m_iIndex ) );
			PACKET_GUARD_BREAK( rkPacket.Write( kExtraItemDB.m_ExtraItem[i].m_iTradeState ) );
			PACKET_GUARD_BREAK( rkPacket.Write( kExtraItemDB.m_ExtraItem[i].m_PeriodType ) );
			PACKET_GUARD_BREAK( rkPacket.Write( kExtraItemDB.m_ExtraItem[i].m_iValue1 ) );
			PACKET_GUARD_BREAK( rkPacket.Write( kExtraItemDB.m_ExtraItem[i].m_iValue2 ) );
			PACKET_GUARD_BREAK( rkPacket.Write( kExtraItemDB.m_ExtraItem[i].m_dwMaleCustom ) );
			PACKET_GUARD_BREAK( rkPacket.Write( kExtraItemDB.m_ExtraItem[i].m_dwFemaleCustom ) );
			PACKET_GUARD_BREAK( rkPacket.Write( kExtraItemDB.m_ExtraItem[i].m_dwFailExp ) );
		}
	}
}

void ioUserExtraItem::ApplyMoveData( SP2Packet &rkPacket, bool bDummyNode )
{
	int iSize = 0;
	PACKET_GUARD_VOID( rkPacket.Read(iSize) );

	m_iCurMaxIndex = 0;

	for(int i = 0;i < iSize;i++)
	{
		EXTRAITEMDB kExtraItemDB;

		PACKET_GUARD_BREAK( rkPacket.Read( kExtraItemDB.m_dwIndex ) );
		PACKET_GUARD_BREAK( rkPacket.Read( kExtraItemDB.m_bChange ) );

		for(int j = 0;j < MAX_SLOT;j++)
		{
			PACKET_GUARD_BREAK( rkPacket.Read( kExtraItemDB.m_ExtraItem[j].m_iItemCode ) );
			PACKET_GUARD_BREAK( rkPacket.Read( kExtraItemDB.m_ExtraItem[j].m_iReinforce ) );
			PACKET_GUARD_BREAK( rkPacket.Read( kExtraItemDB.m_ExtraItem[j].m_iIndex ) );
			PACKET_GUARD_BREAK( rkPacket.Read( kExtraItemDB.m_ExtraItem[j].m_iTradeState ) );
			PACKET_GUARD_BREAK( rkPacket.Read( kExtraItemDB.m_ExtraItem[j].m_PeriodType ) );
			PACKET_GUARD_BREAK( rkPacket.Read( kExtraItemDB.m_ExtraItem[j].m_iValue1 ) );
			PACKET_GUARD_BREAK( rkPacket.Read( kExtraItemDB.m_ExtraItem[j].m_iValue2 ) );
			PACKET_GUARD_BREAK( rkPacket.Read( kExtraItemDB.m_ExtraItem[j].m_dwMaleCustom ) );
			PACKET_GUARD_BREAK( rkPacket.Read( kExtraItemDB.m_ExtraItem[j].m_dwFemaleCustom ) );
			PACKET_GUARD_BREAK( rkPacket.Read( kExtraItemDB.m_ExtraItem[j].m_dwFailExp ) );

			m_iCurMaxIndex = max( m_iCurMaxIndex, kExtraItemDB.m_ExtraItem[j].m_iIndex );
		}

		if( kExtraItemDB.m_dwIndex == NEW_INDEX && !bDummyNode )
		{
			g_DBClient.OnSelectExtraItemIndex( m_pUser->GetUserDBAgentID(), m_pUser->GetAgentThreadID(), m_pUser->GetGUID(), m_pUser->GetPublicID(), m_pUser->GetUserIndex(), false, 0, 0, 0, 0 );
		}

		m_vExtraItemList.push_back( kExtraItemDB );
	}

	if( m_pUser )
	{
		g_CriticalError.CheckExtraItemTableCount( m_pUser->GetPublicID(), m_vExtraItemList.size() );
	}
}

bool ioUserExtraItem::GetExtraItem( IN int iSlotIndex, OUT EXTRAITEMSLOT &rkExtraItem )
{
	// 초기화
	rkExtraItem.Init();

	for(vEXTRAITEMDB::iterator iter = m_vExtraItemList.begin(); iter != m_vExtraItemList.end(); ++iter)
	{
	    EXTRAITEMDB &kExtraItemDB = *iter;
		for (int i = 0; i < MAX_SLOT ; i++)
		{
			if( kExtraItemDB.m_ExtraItem[i].m_iIndex == iSlotIndex )
			{
				rkExtraItem = kExtraItemDB.m_ExtraItem[i];
				return true;
			}
		}
	}
	return false;
}

bool ioUserExtraItem::GetExtraItemByItemCode( IN int iItemCode, IN int iItemReinforceLevel )
{
	for(vEXTRAITEMDB::iterator iter = m_vExtraItemList.begin(); iter != m_vExtraItemList.end(); ++iter)
	{
		EXTRAITEMDB &kExtraItemDB = *iter;
		for (int i = 0; i < MAX_SLOT ; i++)
		{
			if( kExtraItemDB.m_ExtraItem[i].m_iItemCode == iItemCode && kExtraItemDB.m_ExtraItem[i].m_iReinforce == iItemReinforceLevel)
				return true;
		}
	}
	return false;
}

bool ioUserExtraItem::GetRowExtraItem( IN DWORD dwIndex, OUT EXTRAITEMSLOT kExtraItem[MAX_SLOT] )
{
	bool bReturn = false;
	for(vEXTRAITEMDB::iterator iter = m_vExtraItemList.begin(); iter != m_vExtraItemList.end(); ++iter)
	{
		EXTRAITEMDB &kExtraItemDB = *iter;
		if( kExtraItemDB.m_dwIndex != dwIndex )
			continue;

		for (int i = 0; i < MAX_SLOT ; i++)
		{
			kExtraItem[i] = kExtraItemDB.m_ExtraItem[i];
			bReturn = true;
		}

		if( bReturn )
			break;
	}

	return bReturn;
}

void ioUserExtraItem::SetExtraItem( const EXTRAITEMSLOT &rkExtraItem )
{
	for(vEXTRAITEMDB::iterator iter = m_vExtraItemList.begin(); iter != m_vExtraItemList.end(); ++iter)
	{
		EXTRAITEMDB &kExtraItemDB = *iter;
		for (int i = 0; i < MAX_SLOT ; i++)
		{
			if( kExtraItemDB.m_ExtraItem[i].m_iIndex == rkExtraItem.m_iIndex )
			{
				kExtraItemDB.m_ExtraItem[i] = rkExtraItem;
				kExtraItemDB.m_bChange = true;
				return;
			}
		}
	}
}

bool ioUserExtraItem::DeleteExtraItem( int iSlotIndex )
{
	for(vEXTRAITEMDB::iterator iter = m_vExtraItemList.begin(); iter != m_vExtraItemList.end(); ++iter)
	{
		EXTRAITEMDB &kExtraItemDB = *iter;
		for (int i = 0; i < MAX_SLOT ; i++)
		{
			if( kExtraItemDB.m_ExtraItem[i].m_iIndex == iSlotIndex )
			{
				if( m_pUser )
				{
					char szItemIndex[MAX_PATH]="";
					StringCbPrintf( szItemIndex, sizeof( szItemIndex ), "%u-%u", kExtraItemDB.m_dwIndex, i+1 ); // db field는 1부터 이므로 +1
					g_LogDBClient.OnInsertExtraItem( m_pUser, kExtraItemDB.m_ExtraItem[i].m_iItemCode, kExtraItemDB.m_ExtraItem[i].m_iReinforce, 0, 0, 0, kExtraItemDB.m_ExtraItem[i].m_PeriodType, kExtraItemDB.m_ExtraItem[i].m_dwMaleCustom, kExtraItemDB.m_ExtraItem[i].m_dwFemaleCustom, szItemIndex, LogDBClient::ERT_DEL );
				}

				kExtraItemDB.m_ExtraItem[i].Init();
				kExtraItemDB.m_bChange = true;

				DecreasePossessionCount(1);
				
				return true;
			}
		}
	}

	return false;
}

void ioUserExtraItem::DeleteExtraItemPassedDate( OUT IntVec &rvIndexVec )
{
	CTime kCurTime = CTime::GetCurrentTime();

	for(vEXTRAITEMDB::iterator iter = m_vExtraItemList.begin(); iter != m_vExtraItemList.end(); ++iter)
	{
		EXTRAITEMDB &kExtraItemDB = *iter;
		for (int i = 0; i < MAX_SLOT ; i++)
		{
			if( kExtraItemDB.m_ExtraItem[i].m_iIndex == 0 && kExtraItemDB.m_ExtraItem[i].m_iItemCode == 0 )
				continue;

			if( (*iter).m_ExtraItem[i].m_PeriodType == EPT_MORTMAIN )		// 무제한은 무시
				continue;

 			CTime kLimitTime( Help::GetSafeValueForCTimeConstructor( kExtraItemDB.m_ExtraItem[i].GetYear(),
																	 kExtraItemDB.m_ExtraItem[i].GetMonth(),
																	 kExtraItemDB.m_ExtraItem[i].GetDay(),
																	 kExtraItemDB.m_ExtraItem[i].GetHour(),
																	 kExtraItemDB.m_ExtraItem[i].GetMinute(), 0 ) );
			CTimeSpan kRemainTime = kLimitTime - kCurTime;

			if( kRemainTime.GetTotalMinutes() > 0 )
				continue;

			if( m_pUser )
			{
				char szItemIndex[MAX_PATH]="";
				StringCbPrintf( szItemIndex, sizeof( szItemIndex ), "%u-%u", kExtraItemDB.m_dwIndex, i+1 ); // db field는 1부터 이므로 +1
				LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "DeleteExtraItemPassedDate / ID:%d(%s) / %d(%d) / %d:%d / INDEX:%d-%d / custom:%d-%d", m_pUser->GetUserIndex(),
																												m_pUser->GetPublicID().c_str(),
																												kExtraItemDB.m_ExtraItem[i].m_iItemCode,
																												kExtraItemDB.m_ExtraItem[i].m_iReinforce,
																												kExtraItemDB.m_ExtraItem[i].m_iValue1,
																												kExtraItemDB.m_ExtraItem[i].m_iValue2,
																												kExtraItemDB.m_dwIndex, i+1,
																												kExtraItemDB.m_ExtraItem[i].m_dwMaleCustom,
																												kExtraItemDB.m_ExtraItem[i].m_dwFemaleCustom );
			}

			rvIndexVec.push_back( kExtraItemDB.m_ExtraItem[i].m_iIndex );

			(*iter).m_ExtraItem[i].Init();
			(*iter).m_bChange = true;

			DecreasePossessionCount(1);
		}
	}
}

bool ioUserExtraItem::GetExtraItemIndex( IN int iSlotIndex, OUT DWORD &rdwIndex, OUT int &iFieldCnt )
{
	// 초기화
	rdwIndex  = 0;
	iFieldCnt = 0;

	for(vEXTRAITEMDB::iterator iter = m_vExtraItemList.begin(); iter != m_vExtraItemList.end(); ++iter)
	{
		EXTRAITEMDB &kExtraItemDB = *iter;
		for (int i = 0; i < MAX_SLOT ; i++)
		{
			if( kExtraItemDB.m_ExtraItem[i].m_iIndex == iSlotIndex )
			{
				rdwIndex = kExtraItemDB.m_dwIndex;
				iFieldCnt= i+1; // +1 : 필드는 1부터 시작 하므로 
				return true;
			}
		}
	}
	return false;
}

void ioUserExtraItem::DBtoData( CQueryResultData *query_data, int& iLastIndex )
{
	if( !m_pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioUserExtraItem::DBtoData() User NULL!!"); 
		return;
	}

	LOOP_GUARD();
	while( query_data->IsExist() )
	{		
		EXTRAITEMDB kExtraItemDB;
		PACKET_GUARD_BREAK( query_data->GetValue( kExtraItemDB.m_dwIndex, sizeof(int) ) );
		for(int i = 0;i < MAX_SLOT;i++)
		{
			DBTIMESTAMP dts;

			PACKET_GUARD_BREAK( query_data->GetValue( kExtraItemDB.m_ExtraItem[i].m_iItemCode, sizeof(int) ) );
			PACKET_GUARD_BREAK( query_data->GetValue( kExtraItemDB.m_ExtraItem[i].m_iReinforce, sizeof(int) ) );
			PACKET_GUARD_BREAK( query_data->GetValue( kExtraItemDB.m_ExtraItem[i].m_iIndex, sizeof(int) ) );
			PACKET_GUARD_BREAK( query_data->GetValue( kExtraItemDB.m_ExtraItem[i].m_iTradeState, sizeof(int) ) );
			PACKET_GUARD_BREAK( query_data->GetValue( kExtraItemDB.m_ExtraItem[i].m_PeriodType, sizeof(int) ) );
			PACKET_GUARD_BREAK( query_data->GetValue( kExtraItemDB.m_ExtraItem[i].m_dwMaleCustom, sizeof(DWORD) ) );
			PACKET_GUARD_BREAK( query_data->GetValue( kExtraItemDB.m_ExtraItem[i].m_dwFemaleCustom, sizeof(DWORD) ) );
			PACKET_GUARD_BREAK( query_data->GetValue( kExtraItemDB.m_ExtraItem[i].m_dwFailExp, sizeof(short) ) );
			PACKET_GUARD_BREAK( query_data->GetValue( (char*)&dts, sizeof(DBTIMESTAMP) ) );
			CTime kLimitTime(Help::GetSafeValueForCTimeConstructor(dts.year,dts.month,dts.day, dts.hour,dts.minute,dts.second));
			kExtraItemDB.m_ExtraItem[i].SetDate( kLimitTime.GetYear(), kLimitTime.GetMonth(), kLimitTime.GetDay(), kLimitTime.GetHour(), kLimitTime.GetMinute() );
		
			m_iCurMaxIndex = max( m_iCurMaxIndex, kExtraItemDB.m_ExtraItem[i].m_iIndex );
		}

		m_vExtraItemList.push_back( kExtraItemDB );
		iLastIndex = kExtraItemDB.m_dwIndex;

		LOOP_GUARD_CLEAR();

		if( m_vExtraItemList.empty() ) return;
	}
}

void ioUserExtraItem::SendAllExtraItemInfo()
{
	if( !m_pUser )
		return;

	int iSendCount = 0;
	int iItemSize = 0;
	int iLimitCount = g_ExtraItemInfoMgr.GetDefaultExtraItemCount();

	SetMaxPossessionCount();

	static std::vector<EXTRAITEMSLOT> vSendItemInfo;
	
	vSendItemInfo.reserve(iLimitCount);
	vSendItemInfo.clear();

	vEXTRAITEMDB::iterator iter, iEnd;
	iEnd = m_vExtraItemList.end();
	for(iter = m_vExtraItemList.begin();iter != iEnd;iter++)
	{
		EXTRAITEMDB &kExtraItemDB = *iter;
		for(int i = 0;i < MAX_SLOT;i++)
		{
			if( kExtraItemDB.m_ExtraItem[i].m_iItemCode > 0 )
			{
				vSendItemInfo.push_back(kExtraItemDB.m_ExtraItem[i]);
			}
		}
	} 

	SetCurPossessionCount(vSendItemInfo.size());

	iSendCount = iItemSize = vSendItemInfo.size();

	if( iItemSize >= iLimitCount )
		iSendCount = iLimitCount;
	
	int iIndex = 0;
	bool bEnd	= false;
	
	while( 0 != iItemSize )
	{
		SP2Packet kPacket( STPK_USER_EXTRAITEM_DATA );
		PACKET_GUARD_VOID ( kPacket.Write(iSendCount) );

		int iPrevIndex = iIndex;

		for( ; iIndex < iSendCount+iPrevIndex; iIndex++ )
		{
			if( iIndex >= (int)vSendItemInfo.size() )
			{
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[warning][extraitem]item index is over" );
				return;
			}

			if( vSendItemInfo[iIndex].m_iItemCode <= 0 )
				continue;

			PACKET_GUARD_VOID( kPacket.Write(vSendItemInfo[iIndex].m_iItemCode ));
			PACKET_GUARD_VOID( kPacket.Write(vSendItemInfo[iIndex].m_iReinforce ));
			PACKET_GUARD_VOID( kPacket.Write(vSendItemInfo[iIndex].m_iIndex ));
			PACKET_GUARD_VOID( kPacket.Write(vSendItemInfo[iIndex].m_iTradeState ));
			PACKET_GUARD_VOID( kPacket.Write(vSendItemInfo[iIndex].m_PeriodType ));
			PACKET_GUARD_VOID( kPacket.Write(vSendItemInfo[iIndex].m_iValue1 ));
			PACKET_GUARD_VOID( kPacket.Write(vSendItemInfo[iIndex].m_iValue2 ));
			PACKET_GUARD_VOID( kPacket.Write(vSendItemInfo[iIndex].m_dwMaleCustom ));
			PACKET_GUARD_VOID( kPacket.Write(vSendItemInfo[iIndex].m_dwFemaleCustom ));
			PACKET_GUARD_VOID( kPacket.Write(vSendItemInfo[iIndex].m_dwFailExp ));
		}

		iItemSize -= iSendCount;

		if( iItemSize < 0 )
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[warning][extraitem]item size check error : [%d]", vSendItemInfo.size() ); 
			return;
		}

		if( 0 == iItemSize )
			bEnd = true;

		PACKET_GUARD_VOID( kPacket.Write(bEnd));

		m_pUser->SendMessage( kPacket );

		if( iLimitCount > iItemSize )
			iSendCount = iItemSize;
	}
}

void ioUserExtraItem::FillMoveDataWithStartRow( SP2Packet &rkPacket, int iStartRow )
{
	int iEndRow = iStartRow + DB_EXTRAITEM_SELECT_COUNT;

	if( iEndRow > GetRowCount() )
		iEndRow = GetRowCount();

	PACKET_GUARD_VOID( rkPacket.Write( iStartRow ) );
	PACKET_GUARD_VOID( rkPacket.Write( iEndRow ) );

	if( 0 == iStartRow )
	{
		PACKET_GUARD_VOID( rkPacket.Write(m_iCurPossessionCount) );
		PACKET_GUARD_VOID( rkPacket.Write(m_iMaxPossessionCount) );
	}
	for( int i =  iStartRow; i < iEndRow; i++ )
	{

		PACKET_GUARD_VOID( rkPacket.Write( m_vExtraItemList[i].m_dwIndex )  );
		PACKET_GUARD_VOID( rkPacket.Write( m_vExtraItemList[i].m_bChange ) );

		for( int j = 0; j < MAX_SLOT; j++ )
		{
			PACKET_GUARD_VOID( rkPacket.Write( m_vExtraItemList[i].m_ExtraItem[j].m_iItemCode ) );
			PACKET_GUARD_VOID( rkPacket.Write( m_vExtraItemList[i].m_ExtraItem[j].m_iReinforce ) );
			PACKET_GUARD_VOID( rkPacket.Write( m_vExtraItemList[i].m_ExtraItem[j].m_iIndex ) );
			PACKET_GUARD_VOID( rkPacket.Write( m_vExtraItemList[i].m_ExtraItem[j].m_iTradeState ) );
			PACKET_GUARD_VOID( rkPacket.Write( m_vExtraItemList[i].m_ExtraItem[j].m_PeriodType ) );
			PACKET_GUARD_VOID( rkPacket.Write( m_vExtraItemList[i].m_ExtraItem[j].m_iValue1 ) );
			PACKET_GUARD_VOID( rkPacket.Write( m_vExtraItemList[i].m_ExtraItem[j].m_iValue2 ) );
			PACKET_GUARD_VOID( rkPacket.Write( m_vExtraItemList[i].m_ExtraItem[j].m_dwMaleCustom ) );
			PACKET_GUARD_VOID( rkPacket.Write( m_vExtraItemList[i].m_ExtraItem[j].m_dwFemaleCustom ) );
			PACKET_GUARD_VOID( rkPacket.Write( m_vExtraItemList[i].m_ExtraItem[j].m_dwFailExp ) );
		}
	}
}

void ioUserExtraItem::ApplyMoveDataWithRow( SP2Packet &rkPacket, bool bDummyNode )
{
	int iStartRow	= 0;
	int iEndRow		= 0;

	PACKET_GUARD_VOID( rkPacket.Read( iStartRow ) );
	PACKET_GUARD_VOID( rkPacket.Read( iEndRow ) );

	if( 0 == iStartRow )
	{
		PACKET_GUARD_VOID( rkPacket.Read( m_iCurPossessionCount ) );
		PACKET_GUARD_VOID( rkPacket.Read( m_iMaxPossessionCount ) );
	}

	for( int i = iStartRow; i < iEndRow; i++ )
	{
		EXTRAITEMDB kExtraItemDB;
		PACKET_GUARD_BREAK( rkPacket.Read( kExtraItemDB.m_dwIndex ) );
		PACKET_GUARD_BREAK( rkPacket.Read( kExtraItemDB.m_bChange ) );

		for( int j = 0;j < MAX_SLOT;j++ )
		{
			PACKET_GUARD_BREAK( rkPacket.Read( kExtraItemDB.m_ExtraItem[j].m_iItemCode ) );
			PACKET_GUARD_BREAK( rkPacket.Read( kExtraItemDB.m_ExtraItem[j].m_iReinforce ) );
			PACKET_GUARD_BREAK( rkPacket.Read( kExtraItemDB.m_ExtraItem[j].m_iIndex ) );
			PACKET_GUARD_BREAK( rkPacket.Read( kExtraItemDB.m_ExtraItem[j].m_iTradeState ) );
			PACKET_GUARD_BREAK( rkPacket.Read( kExtraItemDB.m_ExtraItem[j].m_PeriodType ) );
			PACKET_GUARD_BREAK( rkPacket.Read( kExtraItemDB.m_ExtraItem[j].m_iValue1 ) );
			PACKET_GUARD_BREAK( rkPacket.Read( kExtraItemDB.m_ExtraItem[j].m_iValue2 ) );
			PACKET_GUARD_BREAK( rkPacket.Read( kExtraItemDB.m_ExtraItem[j].m_dwMaleCustom ) );
			PACKET_GUARD_BREAK( rkPacket.Read( kExtraItemDB.m_ExtraItem[j].m_dwFemaleCustom ) );
			PACKET_GUARD_BREAK( rkPacket.Read( kExtraItemDB.m_ExtraItem[j].m_dwFailExp ) );

			m_iCurMaxIndex = max( m_iCurMaxIndex, kExtraItemDB.m_ExtraItem[j].m_iIndex );
		}

		if( kExtraItemDB.m_dwIndex == NEW_INDEX && !bDummyNode )
		{
			g_DBClient.OnSelectExtraItemIndex( m_pUser->GetUserDBAgentID(), m_pUser->GetAgentThreadID(), m_pUser->GetGUID(), m_pUser->GetPublicID(), m_pUser->GetUserIndex(), false, 0, 0, 0, 0 );
		}
		m_vExtraItemList.push_back( kExtraItemDB );
	}

	if( m_pUser )
	{
		g_CriticalError.CheckExtraItemTableCount( m_pUser->GetPublicID(), m_vExtraItemList.size() );
	}
}

void ioUserExtraItem::DecreasePossessionCount( int iVal )
{
	m_iCurPossessionCount -= iVal;

	if( m_iCurPossessionCount < 0 ) 
		m_iCurPossessionCount = 0;
}

void ioUserExtraItem::SetMaxPossessionCount()
{
	if( !m_pUser )
		return;
	
	m_iMaxPossessionCount = g_ExtraItemInfoMgr.GetDefaultExtraItemCount();

	ioUserEtcItem* pUserEtcItem = m_pUser->GetUserEtcItem();
	ioEtcItem* pEtcItem =  g_EtcItemMgr.FindEtcItem(ioEtcItem::EIT_ETC_EXTRAITEM_SLOT_EXTEND);

	if( !pUserEtcItem || !pEtcItem )
		return;

	ioUserEtcItem::ETCITEMSLOT kSlot;
	if( !pUserEtcItem->GetEtcItem( ioEtcItem::EIT_ETC_EXTRAITEM_SLOT_EXTEND, kSlot ) )
		return;

	m_iMaxPossessionCount += kSlot.GetUse();

	int iMaximumItemCount = g_ExtraItemInfoMgr.GetDefaultExtraItemCount() + pEtcItem->GetMaxUse();

	m_iMaxPossessionCount = min( m_iMaxPossessionCount, iMaximumItemCount );
}

bool ioUserExtraItem::IsSlotFull()
{
	//소유 갯수 체크
	if( m_iCurPossessionCount >= m_iMaxPossessionCount )
		return true;

	return false;
}
