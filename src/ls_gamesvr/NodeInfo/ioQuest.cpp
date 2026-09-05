
#include "stdafx.h"

//#include "../Window.h"
#include "../MainProcess.h"
#include "../EtcHelpFunc.h"
#include "../QueryData/QueryResultData.h"
#include "../DataBase/DBClient.h"

#include "User.h"
#include "ioQuest.h"
#include <strsafe.h>

QuestDataParent::QuestDataParent()
{
	m_dwIndexData = m_dwDateData = 0;
	m_pLinkQuest = NULL;
}

QuestDataParent::~QuestDataParent()
{

}

DWORD QuestDataParent::GetMainIndex()
{
	return m_dwIndexData / INDEX_HALF_VALUE;
}

DWORD QuestDataParent::GetSubIndex()
{
	return m_dwIndexData % INDEX_HALF_VALUE;
}

DWORD QuestDataParent::GetYear()
{
	return DEFAULT_YEAR + ( m_dwDateData / DATE_YEAR_VALUE );
}

DWORD QuestDataParent::GetMonth()
{
	return ( m_dwDateData % DATE_YEAR_VALUE ) / DATE_MONTH_VALUE;
}

DWORD QuestDataParent::GetDay()
{
	return ( m_dwDateData % DATE_MONTH_VALUE ) / DATE_DAY_VALUE;
}

DWORD QuestDataParent::GetHour()
{
	return ( m_dwDateData % DATE_DAY_VALUE ) / DATE_HOUR_VALUE;
}

DWORD QuestDataParent::GetMinute()
{
	return ( m_dwDateData % DATE_HOUR_VALUE );
}

void QuestDataParent::SetIndexData( DWORD dwIndexData )
{
	m_dwIndexData = dwIndexData;
}

void QuestDataParent::SetDateData( DWORD dwDateData )
{
	m_dwDateData = dwDateData;
}

void QuestDataParent::SetDate( WORD wYear, WORD wMonth, WORD wDay, WORD wHour, WORD wMinute )
{
	if( wYear == 0 )  //초기화
		m_dwDateData = 0;
	else
	{
		m_dwDateData = ((wYear - DEFAULT_YEAR) * DATE_YEAR_VALUE) +
						(wMonth * DATE_MONTH_VALUE) + (wDay * DATE_DAY_VALUE) +
						(wHour * DATE_HOUR_VALUE) + wMinute;
	}
}

QuestParent *QuestDataParent::GetLinkQuest()
{ 
	if( m_pLinkQuest == NULL )
	{
		SetLinkQuest( g_QuestMgr.GetQuest( GetMainIndex(), GetSubIndex() ) );
	}
	return m_pLinkQuest; 
}

void QuestDataParent::SetLinkQuest( QuestParent *pQuest )
{
	m_pLinkQuest = pQuest;
}

DWORD QuestDataParent::GetCompleteValue()
{
	QuestParent *pQuestParent = GetLinkQuest();
	if( !pQuestParent )
		return 0;

	return pQuestParent->GetCompleteValue();
}

bool QuestDataParent::CheckDelete()
{
	if( GetMainIndex() == 0 ) return false;

	QuestParent *pQuestParent = GetLinkQuest();
	if( !pQuestParent ) return true;    // 없는 퀘스트이므로 삭제
	if( pQuestParent->GetPerformType() != QP_EVENT ) return false;

	// 퀘스트 받은 시간이 이벤트 시작전 시간이면 다른 퀘스트이므로 삭제
	if( GetDateData() < pQuestParent->GetStartDateData() )
		return true;

	// 이벤트 종료된 퀘스트면 삭제
	if( !pQuestParent->IsAlive() )
		return true;
	return false;
}

bool QuestDataParent::CheckOneDayDelete( DWORD dwCheckTime )
{
	if( GetMainIndex() == 0 ) return false;

	QuestParent *pQuestParent = GetLinkQuest();
	if( !pQuestParent ) return true;    // 없는 퀘스트이므로 삭제
	if( !pQuestParent->IsOneDayStyle() ) return false;   //일일 퀘스트만 체크

	// 퀘스트 완료한 시점이 일일 퀘스트 초기화 이전이면 삭제
	if( GetDateData() < dwCheckTime )
		return true;
	return false;
}

bool QuestDataParent::CheckGuildQuestDelete( bool bGuild )
{
	if( GetMainIndex() == 0 ) return false;
	
	QuestParent *pQuestParent = GetLinkQuest();
	if( !pQuestParent ) return true;    // 없는 퀘스트이므로 삭제
	if( !pQuestParent->IsGuildStyle() ) return false;
	
	// 길드에 가입되어있지 않으면 삭제
	if( !bGuild ) 
		return true;

	return false;
}
//////////////////////////////////////////////////////////////////////////
QuestData::QuestData()
{
	m_dwIndexData = m_dwValueData = m_dwMagicData = m_dwDateData = 0;
	m_pLinkQuest = NULL;
}

QuestData::~QuestData()
{

}

void QuestData::ClearData()
{
	m_dwIndexData = m_dwValueData = m_dwMagicData = m_dwDateData = 0;
	m_pLinkQuest = NULL;
}

DWORD QuestData::GetCurrentData()
{
	return m_dwValueData;
}

DWORD QuestData::GetCurrentMagicData()
{
	return m_dwMagicData / MAGIC_VALUE;
}

DWORD QuestData::GetState()
{
	return ( m_dwMagicData % ALARM_VALUE );
}

bool QuestData::IsAlarm()
{
	if( ( ( m_dwMagicData % MAGIC_VALUE ) / ALARM_VALUE ) == 0 )
		return false;
	return true;
}

void QuestData::SetCurrentData( DWORD dwCurrentData )
{
	m_dwValueData = dwCurrentData;
}

void QuestData::SetCurrentMagicData( DWORD dwCurrentMagicData )
{
	m_dwMagicData = (dwCurrentMagicData * MAGIC_VALUE) + ((int)IsAlarm() * ALARM_VALUE) + GetState();
}

void QuestData::SetAlarm( bool bAlarm )
{
	m_dwMagicData = (GetCurrentMagicData() * MAGIC_VALUE) + ((int)bAlarm * ALARM_VALUE) + GetState();
}

void QuestData::SetState( DWORD dwState )
{
	m_dwMagicData = (GetCurrentMagicData() * MAGIC_VALUE) + ((int)IsAlarm() * ALARM_VALUE) + dwState;

	// 진행중이 아니면 알람 해제
	if( dwState != QS_PROGRESS ) 
		SetAlarm( false );
}

void QuestData::PresetData( QuestParent *pQuest )
{
	if( pQuest == NULL )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "QuestData::PresetData : NULL" );
		return;
	}

	m_pLinkQuest  = pQuest;
	m_dwIndexData = (pQuest->GetMainIndex() * INDEX_HALF_VALUE) + pQuest->GetSubIndex();
	m_dwValueData = 0;
	SetCurrentMagicData( pQuest->GetPresetMagicData() );
	SetAlarm( true );

	CTime cTime = CTime::GetCurrentTime();
	SetDate( cTime.GetYear(), cTime.GetMonth(), cTime.GetDay(), cTime.GetHour(), cTime.GetMinute() );
}

void QuestData::AttainIntegrity( User *pUser )
{
	if( !m_pLinkQuest )
	{
		SetLinkQuest( g_QuestMgr.GetQuest( GetMainIndex(), GetSubIndex() ) );
	}

	if( m_pLinkQuest )
	{
		m_pLinkQuest->IsCheckCompleteTerm( pUser, this );
	}
}

void QuestData::ApplyData( CQueryResultData *query_data )
{
	if( !query_data ) return;

	query_data->GetValue( m_dwIndexData, sizeof(int) );
	query_data->GetValue( m_dwValueData, sizeof(int) );
	query_data->GetValue( m_dwMagicData, sizeof(int) );
	query_data->GetValue( m_dwDateData, sizeof(int) );

	// 퀘스트 포인터 링크
	SetLinkQuest( g_QuestMgr.GetQuest( GetMainIndex(), GetSubIndex() ) );
}

void QuestData::ApplyData( SP2Packet &rkPacket )
{
	rkPacket >> m_dwIndexData >> m_dwValueData >> m_dwMagicData >> m_dwDateData;

	// 퀘스트 포인터 링크
	SetLinkQuest( g_QuestMgr.GetQuest( GetMainIndex(), GetSubIndex() ) );
}
//////////////////////////////////////////////////////////////////////////
QuestCompleteData::QuestCompleteData()
{
	m_dwIndexData = m_dwDateData = 0;
	m_pLinkQuest = NULL;
}

QuestCompleteData::~QuestCompleteData()
{
}

void QuestCompleteData::ClearData()
{
	m_dwIndexData = m_dwDateData = 0;
	m_pLinkQuest = NULL;
}

void QuestCompleteData::PresetData( QuestParent *pQuest )
{
	if( pQuest == NULL )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "QuestCompleteData::PresetData : NULL" );
		return;
	}
	m_dwIndexData = (pQuest->GetMainIndex() * INDEX_HALF_VALUE) + pQuest->GetSubIndex();

	CTime cTime = CTime::GetCurrentTime();
	SetDate( cTime.GetYear(), cTime.GetMonth(), cTime.GetDay(), cTime.GetHour(), cTime.GetMinute() );
}

void QuestCompleteData::ApplyData( CQueryResultData *query_data )
{
	if( !query_data ) return;

	query_data->GetValue( m_dwIndexData, sizeof(int) );
	query_data->GetValue( m_dwDateData, sizeof(int) );

	// 퀘스트 포인터 링크
	SetLinkQuest( g_QuestMgr.GetQuest( GetMainIndex(), GetSubIndex() ) );
}

void QuestCompleteData::ApplyData( SP2Packet &rkPacket )
{
	rkPacket >> m_dwIndexData >> m_dwDateData;

	// 퀘스트 포인터 링크
	SetLinkQuest( g_QuestMgr.GetQuest( GetMainIndex(), GetSubIndex() ) );
}
//////////////////////////////////////////////////////////////////////////
ioQuest::ioQuest()
{
}

ioQuest::~ioQuest()
{
	
}

void ioQuest::InitQuestData()
{
	{
		for(vQuestInfo::iterator iter = m_vItemList.begin();iter != m_vItemList.end();iter++)
		{
			QuestInfo &kItemDB = *iter;
			kItemDB.m_bChange = true;
			for(int i = 0;i < MAX_SLOT;i++)
			{
				kItemDB.m_Data[i].ClearData();
			}
		}
	}

	{
		for(vQuestCompleteInfo::iterator iter = m_vCompleteList.begin();iter != m_vCompleteList.end();iter++)
		{
			QuestCompleteInfo &kItemDB = *iter;
			kItemDB.m_bChange = true;
			for(int i = 0;i < MAX_SLOT;i++)
			{
				kItemDB.m_Data[i].ClearData();
			}
		}
	}
	m_vDeleteList.clear();

	g_DBClient.OnDeleteQuestData( m_pUser->GetUserDBAgentID(), m_pUser->GetAgentThreadID(), m_pUser->GetUserIndex());
}

void ioQuest::Initialize( User *pUser )
{
	m_pUser = pUser;
	m_vItemList.clear();
	m_vCompleteList.clear();
	m_vDeleteList.clear();
	m_AbuseList.clear();
}

void ioQuest::InsertDB( QuestInfo &kQuestDB )
{
	if( !m_pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioQuest::InsertDB() User NULL!!"); 
		return;
	}

	std::vector<int> contents;
	contents.reserve(MAX_SLOT * 4);
	for(int i = 0;i < MAX_SLOT;i++)
	{
		contents.push_back( kQuestDB.m_Data[i].GetIndexData() );
		contents.push_back( kQuestDB.m_Data[i].GetValueData() );
		contents.push_back( kQuestDB.m_Data[i].GetMagicData() );
		contents.push_back( kQuestDB.m_Data[i].GetDateData() );
	}

	// 새로 추가된 인벤토리 DB Insert
	g_DBClient.OnInsertQuestData( m_pUser->GetUserDBAgentID(), m_pUser->GetAgentThreadID(), m_pUser->GetGUID(), m_pUser->GetPublicID(), m_pUser->GetUserIndex(), contents );
}

bool ioQuest::DBtoNewIndex( DWORD dwIndex )
{
	if( !m_pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioQuest::DBtoNewIndex() User NULL!!"); 
		return false;
	}

	{	// 빈 인덱스 없으면 패스
		bool bEmptyIndex = false;
		vQuestInfo::iterator iter, iEnd;
		iEnd = m_vItemList.end();
		for(iter = m_vItemList.begin();iter != iEnd;iter++)
		{
			QuestInfo &kItemDB = *iter;
			if( kItemDB.m_dwIndex == NEW_INDEX )
			{
				bEmptyIndex = true;
				break;
			}
		}

		if( !bEmptyIndex )
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioQuest::DBtoNewIndex() None Empty Index : %s - %d", m_pUser->GetPublicID().c_str(), dwIndex  ); 
			return false;
		}
	}

	{	// 이미 보유하고 있는 인덱스라면 다시 가져온다.
		vQuestInfo::iterator iter, iEnd;
		iEnd = m_vItemList.end();
		for(iter = m_vItemList.begin();iter != iEnd;iter++)
		{
			QuestInfo &kItemDB = *iter;
			if( kItemDB.m_dwIndex == dwIndex )
			{
				g_DBClient.OnSelectQuestIndex( m_pUser->GetUserDBAgentID(), m_pUser->GetAgentThreadID(), m_pUser->GetGUID(), m_pUser->GetPublicID(), m_pUser->GetUserIndex() );
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioQuest::DBtoNewIndex() Already Index : %s - %d", m_pUser->GetPublicID().c_str(), dwIndex  );
				return false;
			}
		}
	}

	{   // 빈 인덱스에 받은 인덱스 적용
		vQuestInfo::iterator iter, iEnd;
		iEnd = m_vItemList.end();
		for(iter = m_vItemList.begin();iter != iEnd;iter++)
		{
			QuestInfo &kItemDB = *iter;
			if( kItemDB.m_dwIndex == NEW_INDEX )
			{
				kItemDB.m_dwIndex = dwIndex;
				LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "ioQuest::DBtoNewIndex() Add : %s - %d", m_pUser->GetPublicID().c_str(), dwIndex  );
				return true;
			}
		}
	}
	return false;
}

void ioQuest::DBtoData( CQueryResultData *query_data )
{
	if( !m_pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioQuest::DBtoData() User NULL!!"); 
		return;
	}

	LOOP_GUARD();
	while( query_data->IsExist() )
	{		
		QuestInfo kItemDB;
		PACKET_GUARD_BREAK( query_data->GetValue( kItemDB.m_dwIndex, sizeof(int) ) );
		for(int i = 0;i < MAX_SLOT;i++)
		{
			kItemDB.m_Data[i].ApplyData( query_data );
			if( kItemDB.m_Data[i].CheckDelete() )				// 이벤트 퀘스트 삭제
			{
				QuestDeleteInfo kDelete;
				kDelete.m_dwMainIndex = kItemDB.m_Data[i].GetMainIndex();
				kDelete.m_dwSubIndex  = kItemDB.m_Data[i].GetSubIndex();
				m_vDeleteList.push_back( kDelete );

				kItemDB.m_Data[i].ClearData();
				kItemDB.m_bChange = true;
			}
			else if( kItemDB.m_Data[i].CheckGuildQuestDelete( m_pUser->IsGuild() ) )      // 길드 퀘스트 삭제
			{
				QuestDeleteInfo kDelete;
				kDelete.m_dwMainIndex = kItemDB.m_Data[i].GetMainIndex();
				kDelete.m_dwSubIndex  = kItemDB.m_Data[i].GetSubIndex();
				m_vDeleteList.push_back( kDelete );

				kItemDB.m_Data[i].ClearData();
				kItemDB.m_bChange = true;
			}
			AddAbuseCheckQuest( kItemDB.m_Data[i].GetMainIndex(), kItemDB.m_Data[i].GetSubIndex() );
		} 
		m_vItemList.push_back( kItemDB );
	}
	LOOP_GUARD_CLEAR();
	g_CriticalError.CheckProgressQuestTableCount( m_pUser->GetPublicID(), m_vItemList.size() );

	if( !m_vItemList.empty() )
	{
		// 유저 전송
		SP2Packet kPacket( STPK_QUEST_DATA );
		PACKET_GUARD_VOID (kPacket.Write((int)( m_vItemList.size() * MAX_SLOT )) );      //전체 전송
		{
			vQuestInfo::iterator iter, iEnd;
			iEnd = m_vItemList.end();
			for(iter = m_vItemList.begin();iter != iEnd;iter++)
			{
				QuestInfo &kItemDB = *iter;
				for(int i = 0;i < MAX_SLOT;i++)
				{	
					PACKET_GUARD_VOID( kPacket.Write( kItemDB.m_Data[i].GetIndexData()) );
					PACKET_GUARD_VOID( kPacket.Write( kItemDB.m_Data[i].GetValueData()) );
					PACKET_GUARD_VOID( kPacket.Write( kItemDB.m_Data[i].GetMagicData()) );
					PACKET_GUARD_VOID( kPacket.Write( kItemDB.m_Data[i].GetDateData()) );
				}
			}
		}		
		m_pUser->SendMessage( kPacket );
	}

	// 삭제해야할 퀘스트 목록 전송
	if( !m_vDeleteList.empty() )
	{
		// 유저 전송
		SP2Packet kPacket( STPK_QUEST_DELETE );
		PACKET_GUARD_VOID( kPacket.Write((int)m_vDeleteList.size()) );      //전체 전송
		{
			vQuestDeleteInfo::iterator iter, iEnd;
			iEnd = m_vDeleteList.end();
			for(iter = m_vDeleteList.begin();iter != iEnd;iter++)
			{
				QuestDeleteInfo &kItemDB = *iter;
				PACKET_GUARD_VOID( kPacket.Write(kItemDB.m_dwMainIndex) );
				PACKET_GUARD_VOID( kPacket.Write(kItemDB.m_dwSubIndex) );
			}
		}		
		m_pUser->SendMessage( kPacket );
		m_vDeleteList.clear();
	}
}

void ioQuest::InsertCompleteDB( QuestCompleteInfo &kQuestDB )
{
	if( !m_pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioQuest::InsertCompleteDB() User NULL!!"); 
		return;
	}

	std::vector<int> contents;
	contents.reserve(MAX_SLOT * 2);
	for(int i = 0;i < MAX_SLOT;i++)
	{
		contents.push_back( kQuestDB.m_Data[i].GetIndexData() );
		contents.push_back( kQuestDB.m_Data[i].GetDateData() );
	}

	g_DBClient.OnInsertQuestCompleteData( m_pUser->GetUserDBAgentID(), m_pUser->GetAgentThreadID(), m_pUser->GetGUID(), m_pUser->GetPublicID(), m_pUser->GetUserIndex(), contents );
}

bool ioQuest::DBtoNewCompleteIndex( DWORD dwIndex )
{
	if( !m_pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioQuest::DBtoNewCompleteIndex() User NULL!!"); 
		return false;
	}

	{	// 빈 인덱스 없으면 패스
		bool bEmptyIndex = false;
		for(vQuestCompleteInfo::iterator iter = m_vCompleteList.begin();iter != m_vCompleteList.end();iter++)
		{
			QuestCompleteInfo &kItemDB = *iter;
			if( kItemDB.m_dwIndex == NEW_INDEX )
			{
				bEmptyIndex = true;
				break;
			}
		}

		if( !bEmptyIndex )
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioQuest::DBtoNewCompleteIndex() None Empty Index : %s - %d", m_pUser->GetPublicID().c_str(), dwIndex  ); 
			return false;
		}
	}

	{	// 이미 보유하고 있는 인덱스라면 다시 가져온다.
		for(vQuestCompleteInfo::iterator iter = m_vCompleteList.begin();iter != m_vCompleteList.end();iter++)
		{
			QuestCompleteInfo &kItemDB = *iter;
			if( kItemDB.m_dwIndex == dwIndex )
			{
				g_DBClient.OnSelectQuestCompleteIndex( m_pUser->GetUserDBAgentID(), m_pUser->GetAgentThreadID(), m_pUser->GetGUID(), m_pUser->GetPublicID(), m_pUser->GetUserIndex() );
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioQuest::DBtoNewCompleteIndex() Already Index : %s - %d", m_pUser->GetPublicID().c_str(), dwIndex  );
				return false;
			}
		}
	}

	{   // 빈 인덱스에 받은 인덱스 적용
		for(vQuestCompleteInfo::iterator iter = m_vCompleteList.begin();iter != m_vCompleteList.end();iter++)
		{
			QuestCompleteInfo &kItemDB = *iter;
			if( kItemDB.m_dwIndex == NEW_INDEX )
			{
				kItemDB.m_dwIndex = dwIndex;
				LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "ioQuest::DBtoNewCompleteIndex() Add : %s - %d", m_pUser->GetPublicID().c_str(), dwIndex  );
				return true;
			}
		}
	}
	return false;
}

void ioQuest::DBtoCompleteData( CQueryResultData *query_data )
{
	if( !m_pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioQuest::DBtoCompleteData() User NULL!!"); 
		return;
	}

	Initialize( m_pUser );

	LOOP_GUARD();
	DWORD dwPrevOneDayCheckTime = g_QuestMgr.GetPrevOneDayQuestDate();
	while( query_data->IsExist() )
	{		
		QuestCompleteInfo kItemDB;
		PACKET_GUARD_BREAK ( query_data->GetValue( kItemDB.m_dwIndex, sizeof(int) ) );
		for(int i = 0;i < MAX_SLOT;i++)
		{
			kItemDB.m_Data[i].ApplyData( query_data );
			if( kItemDB.m_Data[i].CheckOneDayDelete( dwPrevOneDayCheckTime ) )       // 일일 퀘스트 삭제
			{
				// 일일 퀘스트는 바로 삭제하고 유저에게 알리지 않는다.
				kItemDB.m_Data[i].ClearData();
				kItemDB.m_bChange = true;
			}
			else if( kItemDB.m_Data[i].CheckDelete() )								  // 이벤트 퀘스트 삭제	
			{
				QuestDeleteInfo kDelete;
				kDelete.m_dwMainIndex = kItemDB.m_Data[i].GetMainIndex();
				kDelete.m_dwSubIndex  = kItemDB.m_Data[i].GetSubIndex();
				m_vDeleteList.push_back( kDelete );

				kItemDB.m_Data[i].ClearData();
				kItemDB.m_bChange = true;
			}
		} 
		m_vCompleteList.push_back( kItemDB );
	}
	LOOP_GUARD_CLEAR();
	g_CriticalError.CheckCompleteQuestTableCount( m_pUser->GetPublicID(), m_vCompleteList.size() );

	if( !m_vCompleteList.empty() )
	{
		// 유저 전송
		SP2Packet kPacket( STPK_QUEST_COMPLETE_DATA );        
		PACKET_GUARD_VOID( kPacket.Write( (int)( m_vCompleteList.size() * MAX_SLOT) ) );      //전체 전송
		{
			vQuestCompleteInfo::iterator iter, iEnd;
			iEnd = m_vCompleteList.end();
			for(iter = m_vCompleteList.begin();iter != iEnd;iter++)
			{
				QuestCompleteInfo &kItemDB = *iter;
				for(int i = 0;i < MAX_SLOT;i++)
				{	
					PACKET_GUARD_VOID( kPacket.Write( kItemDB.m_Data[i].GetIndexData()) );
					PACKET_GUARD_VOID( kPacket.Write(kItemDB.m_Data[i].GetDateData()) );
				}
			}
		}		
		m_pUser->SendMessage( kPacket );
	}
}

void ioQuest::SaveData()
{
	if( m_vItemList.empty() ) return;
	if( !m_pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioQuest::SaveData() User NULL!!"); 
		return;
	}

	const int iTestCount = 500;
	{   //진행 & 달성 목록
		int iLoopCnt = 0;
		DWORD dwLastIndex = 0;
		for(vQuestInfo::iterator iter = m_vItemList.begin();iter != m_vItemList.end();iter++)
		{
			if( iLoopCnt++ > iTestCount )
			{
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "SaveQuest Loop Error - (%s - %d)", m_pUser->GetPublicID().c_str(), dwLastIndex );
				break;
			}

			QuestInfo &kItemDB = *iter;
			if( kItemDB.m_bChange )
			{
				std::vector<int> contents;
				contents.reserve(MAX_SLOT * 4);
				for(int i = 0;i < MAX_SLOT;i++)
				{
					contents.push_back( kItemDB.m_Data[i].GetIndexData() );
					contents.push_back( kItemDB.m_Data[i].GetValueData() );
					contents.push_back( kItemDB.m_Data[i].GetMagicData() );
					contents.push_back( kItemDB.m_Data[i].GetDateData() );
				}
				if( kItemDB.m_dwIndex == NEW_INDEX )
				{
					LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[warning][quest]None quest Index : [%lu] [%d]", m_pUser->GetUserIndex(), kItemDB.m_dwIndex );
					//LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s", szContent );
				}
				else
				{
					g_DBClient.OnUpdateQuestData( m_pUser->GetUserDBAgentID(), m_pUser->GetAgentThreadID(), m_pUser->GetPublicID(), kItemDB.m_dwIndex, contents );
					kItemDB.m_bChange = false;
					LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "SaveQuest(%s:%d)", m_pUser->GetPublicID().c_str(), kItemDB.m_dwIndex );
					dwLastIndex = kItemDB.m_dwIndex;
				}
			}		
		}
	}

	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[info][quest]Save quest complete : [%lu]", m_pUser->GetUserIndex() );
	{   //완료 목록
		int iLoopCnt = 0;
		DWORD dwLastIndex = 0;
		for(vQuestCompleteInfo::iterator iter = m_vCompleteList.begin();iter != m_vCompleteList.end();iter++)
		{
			if( iLoopCnt++ > iTestCount )
			{
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[warning][quest]Save quest loop error : [%lu] [%d]", m_pUser->GetUserIndex(), dwLastIndex );
				break;
			}

			QuestCompleteInfo &kItemDB = *iter;
			if( kItemDB.m_bChange )
			{
				std::vector<int> contents;
				contents.reserve(MAX_SLOT * 2);
				for(int i = 0;i < MAX_SLOT;i++)
				{
					contents.push_back( kItemDB.m_Data[i].GetIndexData() );
					contents.push_back( kItemDB.m_Data[i].GetDateData() );
				}

				if( kItemDB.m_dwIndex == NEW_INDEX )
				{
					LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[warning][quest]None quest Index : [%lu] [%d]", m_pUser->GetUserIndex(), kItemDB.m_dwIndex );
					//LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s", szContent );

				}
				else
				{
					g_DBClient.OnUpdateQuestCompleteData( m_pUser->GetUserDBAgentID(), m_pUser->GetAgentThreadID(), m_pUser->GetPublicID(), kItemDB.m_dwIndex, contents );
					kItemDB.m_bChange = false;
					LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "SaveQuestComplete(%s:%d)", m_pUser->GetPublicID().c_str(), kItemDB.m_dwIndex );
					dwLastIndex = kItemDB.m_dwIndex;
				}
			}		
		}
	}
}

void ioQuest::FillMoveData( SP2Packet &rkPacket )
{
	{	// 진행 목록
		rkPacket << (int)m_vItemList.size();

		vQuestInfo::iterator iter, iEnd;
		iEnd = m_vItemList.end();
		for(iter = m_vItemList.begin();iter != iEnd;iter++)
		{
			QuestInfo &kItemDB = *iter;
			rkPacket << kItemDB.m_dwIndex << kItemDB.m_bChange;
			for(int i = 0;i < MAX_SLOT;i++)
			{
				rkPacket << kItemDB.m_Data[i].GetIndexData() << kItemDB.m_Data[i].GetValueData() << kItemDB.m_Data[i].GetMagicData() << kItemDB.m_Data[i].GetDateData();
			}
		}
	}

	{	// 완료 목록
		rkPacket << (int)m_vCompleteList.size();

		vQuestCompleteInfo::iterator iter, iEnd;
		iEnd = m_vCompleteList.end();
		for(iter = m_vCompleteList.begin();iter != iEnd;iter++)
		{
			QuestCompleteInfo &kItemDB = *iter;
			rkPacket << kItemDB.m_dwIndex << kItemDB.m_bChange;
			for(int i = 0;i < MAX_SLOT;i++)
			{
				rkPacket << kItemDB.m_Data[i].GetIndexData() << kItemDB.m_Data[i].GetDateData();
			}
		}
	}

	{	// 어뷰즈 체크 목록
		rkPacket << (int)m_AbuseList.size();

		vQuestAbuseInfo::iterator iter, iEnd;
		iEnd = m_AbuseList.end();
		for(iter = m_AbuseList.begin();iter != iEnd;iter++)
		{
			QuestAbuseInfo &rkItem = *iter;
			rkPacket << rkItem.m_dwMainIndex << rkItem.m_dwSubIndex;
			if( rkItem.m_dwCurrentTime == 0 )
			{
				rkPacket << 0;	// 아직 퀘를 받지 못했다.
			}
			else
			{
				rkPacket << (DWORD)( TIMEGETTIME() - rkItem.m_dwCurrentTime );
			}
		}
	}
}

void ioQuest::ApplyMoveData( SP2Packet &rkPacket, bool bDummyNode )
{
	{	// 진행 목록
		int iSize;
		rkPacket >> iSize;
		for(int i = 0;i < iSize;i++)
		{
			QuestInfo kItemDB;
			rkPacket >> kItemDB.m_dwIndex >> kItemDB.m_bChange;
			for(int j = 0;j < MAX_SLOT;j++)
			{
				kItemDB.m_Data[j].ApplyData( rkPacket );
			}
			if( kItemDB.m_dwIndex == NEW_INDEX && !bDummyNode )
				g_DBClient.OnSelectQuestIndex( m_pUser->GetUserDBAgentID(), m_pUser->GetAgentThreadID(), m_pUser->GetGUID(), m_pUser->GetPublicID(), m_pUser->GetUserIndex() );
			m_vItemList.push_back( kItemDB );
		}	

		if( m_pUser )
		{
			g_CriticalError.CheckProgressQuestTableCount( m_pUser->GetPublicID(), m_vItemList.size() );
		}
	}

	{	// 완료 목록
		int iSize;
		rkPacket >> iSize;
		for(int i = 0;i < iSize;i++)
		{
			QuestCompleteInfo kItemDB;
			rkPacket >> kItemDB.m_dwIndex >> kItemDB.m_bChange;
			for(int j = 0;j < MAX_SLOT;j++)
			{
				kItemDB.m_Data[j].ApplyData( rkPacket );
			}
			if( kItemDB.m_dwIndex == NEW_INDEX && !bDummyNode )
				g_DBClient.OnSelectQuestCompleteIndex( m_pUser->GetUserDBAgentID(), m_pUser->GetAgentThreadID(), m_pUser->GetGUID(), m_pUser->GetPublicID(), m_pUser->GetUserIndex() );
			m_vCompleteList.push_back( kItemDB );
		}	

		if( m_pUser )
		{
			g_CriticalError.CheckCompleteQuestTableCount( m_pUser->GetPublicID(), m_vCompleteList.size() );
		}
	}

	{	// 어뷰즈 체크 목록
		int iSize;
		rkPacket >> iSize;
		for(int i = 0;i < iSize;i++)
		{
			QuestAbuseInfo kItem;
			rkPacket >> kItem.m_dwMainIndex >> kItem.m_dwSubIndex >> kItem.m_dwCurrentTime;
			if( kItem.m_dwCurrentTime != 0 )
				kItem.m_dwCurrentTime = TIMEGETTIME() - kItem.m_dwCurrentTime;    
			kItem.m_bServerMoving = true;       // 서버 이동 후 다음 틱은 여유있게 처리한다.
			m_AbuseList.push_back( kItem );
		}
	}
}
//
bool ioQuest::IsQuestComplete( DWORD dwMainIndex, DWORD dwSubIndex )
{
	vQuestCompleteInfo::iterator iter, iEnd;
	iEnd = m_vCompleteList.end();
	for(iter = m_vCompleteList.begin();iter != iEnd;iter++)
	{
		QuestCompleteInfo &kItemDB = *iter;
		for(int i = 0;i < MAX_SLOT;i++)
		{
			if( kItemDB.m_Data[i].GetMainIndex() != dwMainIndex ) continue;
			
			// 서브 인덱스는 완료된 퀘스트로 판단
			if( dwSubIndex <= kItemDB.m_Data[i].GetSubIndex() )   
			{
				return true;
			}
			return false;
		}
	}
	return false;
}

bool ioQuest::IsQuestIndexCheck( DWORD dwMainIndex, DWORD dwSubIndex )
{
	{
		vQuestInfo::iterator iter, iEnd;
		iEnd = m_vItemList.end();
		for(iter = m_vItemList.begin();iter != iEnd;iter++)
		{
			QuestInfo &kItemDB = *iter;
			for(int i = 0;i < MAX_SLOT;i++)
			{
				if( kItemDB.m_Data[i].GetMainIndex() != dwMainIndex ) continue;

				// 서브 인덱스는 보유중으로 판단.
				if( dwSubIndex <= kItemDB.m_Data[i].GetSubIndex() )   
					return true;
			}
		}
	}

	{
		vQuestCompleteInfo::iterator iter, iEnd;
		iEnd = m_vCompleteList.end();
		for(iter = m_vCompleteList.begin();iter != iEnd;iter++)
		{
			QuestCompleteInfo &kItemDB = *iter;
			for(int i = 0;i < MAX_SLOT;i++)
			{
				if( kItemDB.m_Data[i].GetMainIndex() != dwMainIndex ) continue;

				// 서브 인덱스는 보유중으로 판단.
				if( dwSubIndex <= kItemDB.m_Data[i].GetSubIndex() )   
					return true;
			}
		}
	}
	return false;
}

bool ioQuest::IsQuestAttainCheck( DWORD dwMainIndex, DWORD dwSubIndex )
{	
	vQuestCompleteInfo::iterator iter, iEnd;
	iEnd = m_vCompleteList.end();
	for(iter = m_vCompleteList.begin();iter != iEnd;iter++)
	{
		QuestCompleteInfo &kItemDB = *iter;
		for(int i = 0;i < MAX_SLOT;i++)
		{
			if( kItemDB.m_Data[i].GetMainIndex() != dwMainIndex ) continue;
			
			// 서브 인덱스는 보유중으로 판단.
			if( dwSubIndex == kItemDB.m_Data[i].GetSubIndex() )   
			{
				// 퀘스트 달성 여부
				int iCompleteDate =  kItemDB.m_Data[i].GetDateData();
				if( iCompleteDate > 0 ) 
					return true;
			}
		}
	}
	return false;
}


bool ioQuest::IsCheckAvailableQuest(  DWORD dwMainIndex, DWORD dwSubIndex, int iStartDate, int iEndDate )
{
	
	vQuestCompleteInfo::iterator iter, iEnd;
	iEnd = m_vCompleteList.end();
	for(iter = m_vCompleteList.begin();iter != iEnd;iter++)
	{
		QuestCompleteInfo &kItemDB = *iter;
		for(int i = 0;i < MAX_SLOT;i++)
		{
			if( kItemDB.m_Data[i].GetMainIndex() != dwMainIndex ) continue;

			// 서브 인덱스는 보유중으로 판단.
			if( dwSubIndex == kItemDB.m_Data[i].GetSubIndex() )   
			{
				// 퀘스트 달성 여부
				int iCompleteDate =  kItemDB.m_Data[i].GetDateData();
				
				// ini 세팅 날짜보다 이전에 획득한 퀘스트 인지 
				if( COMPARE( iCompleteDate, iStartDate, iEndDate ) )
				{
					LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioQuest::IsCheckAvailableQuest false %d:%d:%d", iCompleteDate, iStartDate, iEndDate); 
					return false;
				}
				else
				{
					LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioQuest::IsCheckAvailableQuest true %d:%d:%d", iCompleteDate, iStartDate, iEndDate); 
					return true;
				}
			}
		}
	}
	
	return false;
}

QuestData ioQuest::GetQuestData( ioHashString szClass )
{
	vQuestInfo::iterator iter, iEnd;
	iEnd = m_vItemList.end();
	for(iter = m_vItemList.begin();iter != iEnd;iter++)
	{
		QuestInfo &kItemDB = *iter;
		for(int i = 0;i < MAX_SLOT;i++)
		{
			if(kItemDB.m_Data[i].GetLinkQuest() == NULL) continue;
			if( strcmp( kItemDB.m_Data[i].GetLinkQuest()->GetClass().c_str(), szClass.c_str() ) != 0 ) continue;

			return kItemDB.m_Data[i];
		}
	}

	static QuestData kNoneItem;
	return kNoneItem;
}

QuestData ioQuest::GetQuestData( DWORD dwMainIndex, DWORD dwSubIndex )
{
	vQuestInfo::iterator iter, iEnd;
	iEnd = m_vItemList.end();
	for(iter = m_vItemList.begin();iter != iEnd;iter++)
	{
		QuestInfo &kItemDB = *iter;
		for(int i = 0;i < MAX_SLOT;i++)
		{
			if( kItemDB.m_Data[i].GetMainIndex() != dwMainIndex ) continue;
			if( kItemDB.m_Data[i].GetSubIndex() != dwSubIndex ) continue;

			return kItemDB.m_Data[i];
		}
	}

	static QuestData kNoneItem;
	return kNoneItem;
}

QuestData ioQuest::AddQuestData( DWORD dwMainIndex, DWORD dwSubIndex )
{
	static QuestData kErrorItem;

	if( !m_pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioQuest::AddQuestData User NULL!!"); 
		return kErrorItem;
	}
	
	if( IsQuestComplete( dwMainIndex, dwSubIndex ) )
	{
		// 이미 완료한 퀘스트 인가?
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioQuest::AddQuestData 완료한 퀘스트를 받으려고함(%s) - %d:%d", m_pUser->GetPublicID().c_str(), dwMainIndex, dwSubIndex ); 
		return kErrorItem;
	}
	
	if( IsQuestIndexCheck( dwMainIndex, dwSubIndex ) )
	{
		// 이미 보유중인 퀘스트 인가?
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioQuest::AddQuestData  보유중인 퀘스트를 받으려고함(%s) - %d:%d", m_pUser->GetPublicID().c_str(), dwMainIndex, dwSubIndex ); 
		return kErrorItem;
	}
	
	QuestParent *pQuestParent = g_QuestMgr.GetQuest( dwMainIndex, dwSubIndex );
	if( !pQuestParent )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioQuest::AddQuestData 없는 퀘스트를 받으려고함(%s) - %d:%d", m_pUser->GetPublicID().c_str(), dwMainIndex, dwSubIndex ); 
		return kErrorItem;
	}
	
	{
		/************************************************************************/
		/* 서브 퀘스트는 덮어 쓴다.                                             */
		/************************************************************************/
		vQuestInfo::iterator iter, iEnd;
		iEnd = m_vItemList.end();
		for(iter = m_vItemList.begin();iter != iEnd;iter++)
		{
			QuestInfo &kItemDB = *iter;
			for(int i = 0;i < MAX_SLOT;i++)
			{
				if( kItemDB.m_Data[i].GetMainIndex() == dwMainIndex )
				{
					kItemDB.m_Data[i].PresetData( pQuestParent );
					AddAbuseCheckQuest( kItemDB.m_Data[i].GetMainIndex(), kItemDB.m_Data[i].GetSubIndex() );
					kItemDB.m_bChange = true;
					return kItemDB.m_Data[i];
				}
			}		
		}
	}

	{
		/************************************************************************/
		/*빈칸에 넣는다.														*/
		/************************************************************************/
		vQuestInfo::iterator iter, iEnd;
		iEnd = m_vItemList.end();
		for(iter = m_vItemList.begin();iter != iEnd;iter++)
		{
			QuestInfo &kItemDB = *iter;
			for(int i = 0;i < MAX_SLOT;i++)
			{
				if( kItemDB.m_Data[i].GetMainIndex() == 0 )
				{
					kItemDB.m_Data[i].PresetData( pQuestParent );	
					AddAbuseCheckQuest( kItemDB.m_Data[i].GetMainIndex(), kItemDB.m_Data[i].GetSubIndex() );	
					kItemDB.m_bChange = true;
					return kItemDB.m_Data[i];
				}
			}		
		}
	}

	/************************************************************************/
	/*새로운 테이블 														*/
	/************************************************************************/
	QuestInfo kItemDB;
	kItemDB.m_dwIndex = NEW_INDEX;
	kItemDB.m_Data[0].PresetData( pQuestParent );
	AddAbuseCheckQuest( kItemDB.m_Data[0].GetMainIndex(), kItemDB.m_Data[0].GetSubIndex() );
	m_vItemList.push_back( kItemDB );
	InsertDB( kItemDB );

	if( m_pUser )
	{
		g_CriticalError.CheckProgressQuestTableCount( m_pUser->GetPublicID(), m_vItemList.size() );
	}
  	return kItemDB.m_Data[0];
}

QuestCompleteData ioQuest::AddQuestCompleteData( DWORD dwMainIndex, DWORD dwSubIndex )
{
	static QuestCompleteData kErrorItem;

	if( !m_pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioQuest::AddQuestCompleteData User NULL!!"); 
		return kErrorItem;
	}

	QuestParent *pQuestParent = g_QuestMgr.GetQuest( dwMainIndex, dwSubIndex );
	if( !pQuestParent )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioQuest::AddQuestCompleteData 없는 퀘스트를 완료하려고함(%s) - %d:%d", m_pUser->GetPublicID().c_str(), dwMainIndex, dwSubIndex ); 
		return kErrorItem;
	}

	// 반복 퀘스트는 완료 목록에 넣지 않느다.
	if( pQuestParent->IsRepeatStyle() )
		return kErrorItem;

	{
		/************************************************************************/
		/* 서브 퀘스트는 덮어 쓴다.                                             */
		/************************************************************************/
		vQuestCompleteInfo::iterator iter, iEnd;
		iEnd = m_vCompleteList.end();
		for(iter = m_vCompleteList.begin();iter != iEnd;iter++)
		{
			QuestCompleteInfo &kItemDB = *iter;
			for(int i = 0;i < MAX_SLOT;i++)
			{
				if( kItemDB.m_Data[i].GetMainIndex() == dwMainIndex )
				{
					kItemDB.m_Data[i].PresetData( pQuestParent );
					kItemDB.m_bChange = true;
					return kItemDB.m_Data[i];
				}
			}		
		}
	}

	{
		/************************************************************************/
		/*빈칸에 넣는다.														*/
		/************************************************************************/
		vQuestCompleteInfo::iterator iter, iEnd;
		iEnd = m_vCompleteList.end();
		for(iter = m_vCompleteList.begin();iter != iEnd;iter++)
		{
			QuestCompleteInfo &kItemDB = *iter;
			for(int i = 0;i < MAX_SLOT;i++)
			{
				if( kItemDB.m_Data[i].GetMainIndex() == 0 )
				{
					kItemDB.m_Data[i].PresetData( pQuestParent );		
					kItemDB.m_bChange = true;
					return kItemDB.m_Data[i];
				}
			}		
		}
	}

	/************************************************************************/
	/*새로운 테이블 														*/
	/************************************************************************/
	QuestCompleteInfo kItemDB;
	kItemDB.m_dwIndex = NEW_INDEX;
	kItemDB.m_Data[0].PresetData( pQuestParent );
	m_vCompleteList.push_back( kItemDB );
	InsertCompleteDB( kItemDB );

	if( m_pUser )
	{
		g_CriticalError.CheckCompleteQuestTableCount( m_pUser->GetPublicID(), m_vCompleteList.size() );
	}
	return kItemDB.m_Data[0];
}

QuestData ioQuest::SetQuestCurrentData( QuestData &kData )
{
	vQuestInfo::iterator iter, iEnd;
	iEnd = m_vItemList.end();
	for(iter = m_vItemList.begin();iter != iEnd;iter++)
	{
		QuestInfo &kItemDB = *iter;
		for(int i = 0;i < MAX_SLOT;i++)
		{
			if( kItemDB.m_Data[i].GetMainIndex() == kData.GetMainIndex() &&
				kItemDB.m_Data[i].GetSubIndex() == kData.GetSubIndex() )
			{
				if( IsQuestAbuse( kItemDB.m_Data[i], kData ) )
					return kItemDB.m_Data[i];

				kItemDB.m_Data[i].SetCurrentData( kData.GetCurrentData() );
				kItemDB.m_Data[i].SetCurrentMagicData( kData.GetCurrentMagicData() );
				kItemDB.m_Data[i].AttainIntegrity( m_pUser );		
				kItemDB.m_bChange = true;
				return kItemDB.m_Data[i];
			}
		}		
	}

	static QuestData kNoneItem;
	return kNoneItem;
}

void ioQuest::SetQuestReward( DWORD dwMainIndex, DWORD dwSubIndex, bool isRewardSelectStyle, std::vector<BYTE>& vSelIndexes, SP2Packet &rkPacket )
{
	vQuestInfo::iterator iter, iEnd;
	iEnd = m_vItemList.end();
	for(iter = m_vItemList.begin();iter != iEnd;iter++)
	{
		QuestInfo &kItemDB = *iter;
		for(int i = 0;i < MAX_SLOT;i++)
		{
			if( kItemDB.m_Data[i].GetMainIndex() != dwMainIndex ) continue;
			if( kItemDB.m_Data[i].GetSubIndex() != dwSubIndex ) continue;
			if( kItemDB.m_Data[i].GetState() != QS_ATTAIN ) continue;

			QuestParent *pQuestParent = kItemDB.m_Data[i].GetLinkQuest();
			if( !pQuestParent ) continue;

			// 완료 처리
			bool bPresent = false;		
			rkPacket << kItemDB.m_Data[i].GetIndexData();

			// 완료 목록에 추가하고 날짜를 보내준다.
			QuestCompleteData kCompleteData = AddQuestCompleteData( pQuestParent->GetMainIndex(), pQuestParent->GetSubIndex() );						
			rkPacket << kCompleteData.GetDateData();

			// 보상 지급
			BYTE indexes[ 5 ] = { 0, 1, 2, 3, 4 };
			bool bServerRewardSelectStyle = pQuestParent->IsRewardSelectStyle();
			if ( !bServerRewardSelectStyle ) // 써버가 선택보상이 아니면 지급
			{				
				bPresent = PackQuestReward( rkPacket, pQuestParent, indexes, pQuestParent->GetMaxRewardPresent() );
			}
			else if ( !isRewardSelectStyle ) // 써버가 선택 보상인데 client는 선택보상이 아니라고 오면 앞에서부터 순서대로 선택지급
			{
				bPresent = PackQuestReward( rkPacket, pQuestParent, indexes, pQuestParent->GetRewardSelectNum() );
			}
			else // 선택보상이라면 선택된것들 지급
			{
				int numReward = pQuestParent->GetRewardSelectNum();
				if ( vSelIndexes.size() != numReward ) // Server 정보와 다르므로 앞에서부터 순서대로 선택지급
				{
					bPresent = PackQuestReward( rkPacket, pQuestParent, indexes, std::min<int>( numReward, vSelIndexes.size() ) );
				}
				else // 선택보상 지급
				{
					bPresent = PackQuestReward( rkPacket, pQuestParent, &(vSelIndexes[ 0 ]), numReward );
				}
			}

			if( bPresent )
			{
				 m_pUser->SendPresentMemory();
				//m_pUser->_OnSelectPresent( 30 );     // 메모리에만 저장하니까 요청하지 않는다. 2011.04.12 LJH
			}
			
			DeleteAbuseCheckQuest( pQuestParent->GetMainIndex(), pQuestParent->GetSubIndex() );

			// 진행중인 퀘스트 삭제
			kItemDB.m_Data[i].ClearData();
			kItemDB.m_bChange = true;
			return;
		}		
	}
	return;
}

bool	ioQuest::PackQuestReward( SP2Packet& rpacket, QuestParent* quest, BYTE* pindexes, int numindex )
{
	bool pay = false;
	if ( !quest || !pindexes || numindex <= 0 ) 
	{
		rpacket << (int)0;		
	}
	else
	{
		rpacket << numindex;
		for ( int i = 0; i < numindex; ++i )
		{
			bool direct = g_QuestMgr.SendRewardPresent( m_pUser, quest->GetRewardPresentIndex( pindexes[ i ] ) );
			rpacket << direct;
			if ( !direct ) {
				pay = true;
			}
		}
	}

	rpacket << m_pUser->GetMoney() << m_pUser->GetGradeLevel() << m_pUser->GetGradeExpert();
	return pay;
}

QuestData ioQuest::SetQuestAlarm( DWORD dwMainIndex, DWORD dwSubIndex, bool bAlarm )
{
	vQuestInfo::iterator iter, iEnd;
	iEnd = m_vItemList.end();
	for(iter = m_vItemList.begin();iter != iEnd;iter++)
	{
		QuestInfo &kItemDB = *iter;
		for(int i = 0;i < MAX_SLOT;i++)
		{
			if( kItemDB.m_Data[i].GetMainIndex() == dwMainIndex &&
				kItemDB.m_Data[i].GetSubIndex() == dwSubIndex )
			{
				kItemDB.m_Data[i].SetAlarm( bAlarm );
				kItemDB.m_bChange = true;
				return kItemDB.m_Data[i];
			}
		}		
	}

	static QuestData kNoneItem;
	return kNoneItem;
}

void ioQuest::CheckQuestReConnectSeconds( int iTotalSeconds )
{
	vQuestInfo::iterator iter, iEnd;
	iEnd = m_vItemList.end();
	for(iter = m_vItemList.begin();iter != iEnd;iter++)
	{
		QuestInfo &kItemDB = *iter;
		for(int i = 0;i < MAX_SLOT;i++)
		{
			QuestParent *pQuestParent = kItemDB.m_Data[i].GetLinkQuest();
			if( pQuestParent )
			{
				if( pQuestParent->IsCheckQuestReConnectSeconds() )
				{
					kItemDB.m_Data[i].SetCurrentData( kItemDB.m_Data[i].GetCurrentData() + iTotalSeconds );
					kItemDB.m_bChange = true;
				}
			}
		}		
	}
}

void ioQuest::ClearQuestInfo( DWORD dwMainIndex, DWORD dwSubIndex )
{
	{
		for(vQuestInfo::iterator iter = m_vItemList.begin();iter != m_vItemList.end();iter++)
		{
			QuestInfo &kItemDB = *iter;
			for(int i = 0;i < MAX_SLOT;i++)
			{
				if( kItemDB.m_Data[i].GetMainIndex() != dwMainIndex ) continue;
				if( kItemDB.m_Data[i].GetSubIndex() != dwSubIndex ) continue;

				kItemDB.m_bChange = true;
				kItemDB.m_Data[i].ClearData();
				return;
			}
		}
	}

	{
		for(vQuestCompleteInfo::iterator iter = m_vCompleteList.begin();iter != m_vCompleteList.end();iter++)
		{
			QuestCompleteInfo &kItemDB = *iter;
			kItemDB.m_bChange = true;
			for(int i = 0;i < MAX_SLOT;i++)
			{
				if( kItemDB.m_Data[i].GetMainIndex() != dwMainIndex ) continue;
				if( kItemDB.m_Data[i].GetSubIndex() != dwSubIndex ) continue;

				kItemDB.m_bChange = true;
				kItemDB.m_Data[i].ClearData();
				return;
			}
		}
	}
}

void ioQuest::ClearOneDayQuestCompleteAll()
{
	{
		for(vQuestCompleteInfo::iterator iter = m_vCompleteList.begin();iter != m_vCompleteList.end();iter++)
		{
			QuestCompleteInfo &kItemDB = *iter;
			kItemDB.m_bChange = true;
			for(int i = 0;i < MAX_SLOT;i++)
			{
				QuestParent *pQuestParent = kItemDB.m_Data[i].GetLinkQuest();
				if( !pQuestParent ) continue;
				if( !pQuestParent->IsOneDayStyle() ) continue;

				kItemDB.m_bChange = true;
				kItemDB.m_Data[i].ClearData();
			}
		}
	}
}

void ioQuest::ClearGuildQuestProgressAll()
{
	if( !m_pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioQuest::ClearGuildQuestProgressAll() User NULL!!"); 
		return;	
	}

	vQuestDeleteInfo vDeleteList;
	{
		// 진행중인 목록만 삭제
		for(vQuestInfo::iterator iter = m_vItemList.begin();iter != m_vItemList.end();iter++)
		{
			QuestInfo &kItemDB = *iter;
			for(int i = 0;i < MAX_SLOT;i++)
			{
				if( kItemDB.m_Data[i].CheckGuildQuestDelete( m_pUser->IsGuild() ) )      // 길드 퀘스트 삭제
				{
					QuestDeleteInfo kDelete;
					kDelete.m_dwMainIndex = kItemDB.m_Data[i].GetMainIndex();
					kDelete.m_dwSubIndex  = kItemDB.m_Data[i].GetSubIndex();
					vDeleteList.push_back( kDelete );

					kItemDB.m_Data[i].ClearData();
					kItemDB.m_bChange = true;
				}				
			}
		}


		// 삭제해야할 퀘스트 목록 전송
		if( !vDeleteList.empty() )
		{
			LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "ClearGuildQuestProgressAll : %s - %d", m_pUser->GetPublicID().c_str(), (int)vDeleteList.size() );
			// 유저 전송
			SP2Packet kPacket( STPK_QUEST_DELETE );
			kPacket << (int)vDeleteList.size();      //전체 전송
			{
				vQuestDeleteInfo::iterator iter, iEnd;
				iEnd = vDeleteList.end();
				for(iter = vDeleteList.begin();iter != iEnd;iter++)
				{
					QuestDeleteInfo &kItemDB = *iter;
					kPacket << kItemDB.m_dwMainIndex << kItemDB.m_dwSubIndex;
				}
			}		
			m_pUser->SendMessage( kPacket );
			vDeleteList.clear();
		}
	}
}

// 다음 퀘스트 어뷰즈 체크할 것인지
void ioQuest::AddAbuseCheckQuest( DWORD dwMainIndex, DWORD dwSubIndex )
{
	QuestParent *pQuestParent = g_QuestMgr.GetQuest( dwMainIndex, dwSubIndex );
	if( pQuestParent )
	{
		if( pQuestParent->IsAbuseCheckQuest() )
		{
			QuestAbuseInfo kItem;
			kItem.m_dwMainIndex		= dwMainIndex;
			kItem.m_dwSubIndex		= dwSubIndex;
			kItem.m_dwCurrentTime	= TIMEGETTIME();
			m_AbuseList.push_back( kItem );
		}
	}
}

// 어뷰즈 체크하는 퀘스트 삭제
void ioQuest::DeleteAbuseCheckQuest( DWORD dwMainIndex, DWORD dwSubIndex )
{
	vQuestAbuseInfo::iterator iter, iEnd;
	iEnd = m_AbuseList.end();
	for(iter = m_AbuseList.begin();iter != iEnd;iter++)
	{
		QuestAbuseInfo &rkItem = *iter;
		if( rkItem.m_dwMainIndex != dwMainIndex ) continue;
		if( rkItem.m_dwSubIndex != dwSubIndex ) continue;

		m_AbuseList.erase( iter );
		return;
	}
}

// 퀘스트 어뷰즈
bool ioQuest::IsQuestAbuse( QuestData &rkServerQuest, QuestData &rkClientData )
{
	if( m_pUser == NULL )
		return true;

	if( Help::IsQuestAbuseDeveloperPass() && m_pUser->IsDeveloper() )
		return false;      // 개발자B는 제외

	QuestParent *pQuestParent = rkServerQuest.GetLinkQuest();
	if( pQuestParent == NULL ) 
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "존재하지 않는 퀘스트로 어뷰즈 처리" );
		return true;
	}

	for(vQuestAbuseInfo::iterator iter = m_AbuseList.begin();iter != m_AbuseList.end();iter++)
	{
		QuestAbuseInfo &rkItem = *iter;
		if( rkItem.m_dwMainIndex != rkServerQuest.GetMainIndex() ) continue;
		if( rkItem.m_dwSubIndex  != rkServerQuest.GetSubIndex() ) continue;

		DWORD dwGapTime = ( ( TIMEGETTIME() - rkItem.m_dwCurrentTime ) / 1000 ) + 1;
		int   iGapValue = rkClientData.GetCurrentData() - rkServerQuest.GetCurrentData();
		if( rkItem.m_bServerMoving )
		{
			dwGapTime = dwGapTime + 60;
			rkItem.m_bServerMoving = false;
		}

		if( pQuestParent->IsAbuseCheck( dwGapTime, iGapValue ) )
		{
			// 어뷰즈 알림
			SP2Packet kPacket( STPK_QUEST_ABUSE_ALARM );
			kPacket << Help::IsQuestAbuseLogOut() << rkServerQuest.GetMainIndex() << rkServerQuest.GetSubIndex() << dwGapTime << iGapValue;
			m_pUser->SendMessage( kPacket );

			g_CriticalError.CheckQuestAbuse( m_pUser->GetPublicID(), rkServerQuest.GetMainIndex(), rkServerQuest.GetSubIndex(), dwGapTime, iGapValue );
			return true;
		}
		else
		{
			// 어뷰즈 아님
			rkItem.m_dwCurrentTime = TIMEGETTIME();
		}
	}
	return false;
}
