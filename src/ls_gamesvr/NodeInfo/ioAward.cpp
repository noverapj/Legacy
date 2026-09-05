
#include "stdafx.h"

//#include "../Window.h"
#include "../MainProcess.h"
#include "../EtcHelpFunc.h"
#include "../QueryData/QueryResultData.h"
#include "../DataBase/DBClient.h"
#include "User.h"
#include "ioMyLevelMgr.h"
#include "ioAward.h"


ioAward::ioAward()
{
	Initialize( NULL );
	m_iAwardLevel = 0;
	m_iAwardExp   = 0;
}

ioAward::~ioAward()
{
	m_vAwardList.clear();
}

void ioAward::Initialize( User *pUser )
{
	m_pUser = pUser;
	m_bAwardChange = false;
	m_vAwardList.clear();
}

void ioAward::InsertDBAward( AWARDDB &kAwardDB )
{
	if( !m_pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioAward::InsertDBAward() User NULL!!"); 
		return;
	}

	std::vector<int> contents;
	contents.reserve(MAX_SLOT * 3);
	for(int i = 0;i < MAX_SLOT;i++)
	{
		contents.push_back( kAwardDB.m_Award[i].m_iCategory );
		contents.push_back( kAwardDB.m_Award[i].m_iCount );
		contents.push_back( kAwardDB.m_Award[i].m_iPoint );
	}

	// 새로 추가된 시상내역 DB Insert
	g_DBClient.OnInsertAwardData( m_pUser->GetUserDBAgentID(), m_pUser->GetAgentThreadID(), m_pUser->GetGUID(), m_pUser->GetPublicID(), m_pUser->GetUserIndex(), contents );
}

bool ioAward::DBtoNewIndex( DWORD dwIndex )
{
	if( !m_pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioAward::DBtoNewIndex() User NULL!!"); 
		return false;
	}

	{	// 빈 인덱스 없으면 패스
		bool bEmptyIndex = false;
		vAWARDDB::iterator iter, iEnd;
		iEnd = m_vAwardList.end();
		for(iter = m_vAwardList.begin();iter != iEnd;iter++)
		{
			AWARDDB &kAwardDB = *iter;
			if( kAwardDB.m_dwIndex == NEW_INDEX )
			{
				bEmptyIndex = true;
				break;
			}
		}

		if( !bEmptyIndex )
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioAward::DBtoNewIndex() None Empty Index : %s - %d", m_pUser->GetPublicID().c_str(), dwIndex  ); 
			return false;
		}
	}

	{	// 이미 보유하고 있는 인덱스라면 다시 가져온다.
		vAWARDDB::iterator iter, iEnd;
		iEnd = m_vAwardList.end();
		for(iter = m_vAwardList.begin();iter != iEnd;iter++)
		{
			AWARDDB &kAwardDB = *iter;
			if( kAwardDB.m_dwIndex == dwIndex )
			{
				g_DBClient.OnSelectAwardIndex( m_pUser->GetUserDBAgentID(), m_pUser->GetAgentThreadID(), m_pUser->GetGUID(), m_pUser->GetPublicID(), m_pUser->GetUserIndex() );
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioAward::DBtoNewIndex() Already Index : %s - %d", m_pUser->GetPublicID().c_str(), dwIndex  );
				return false;
			}
		}
	}

	{   // 빈 인덱스에 받은 인덱스 적용
		vAWARDDB::iterator iter, iEnd;
		iEnd = m_vAwardList.end();
		for(iter = m_vAwardList.begin();iter != iEnd;iter++)
		{
			AWARDDB &kAwardDB = *iter;
			if( kAwardDB.m_dwIndex == NEW_INDEX )
			{
				kAwardDB.m_dwIndex = dwIndex;
				LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "ioAward::DBtoNewIndex() Add : %s - %d", m_pUser->GetPublicID().c_str(), dwIndex  );
				return true;
			}
		}
	}
	return false;
}

void ioAward::DBtoData( CQueryResultData *query_data )
{
	if( !m_pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioAward::DBtoData() User NULL!!"); 
		return;
	}

	Initialize( m_pUser );

	LOOP_GUARD();
	while( query_data->IsExist() )
	{		
		AWARDDB kAwardDB;
		PACKET_GUARD_BREAK( query_data->GetValue( kAwardDB.m_dwIndex, sizeof(int) ) );
		for(int i = 0;i < MAX_SLOT;i++)
		{
			PACKET_GUARD_BREAK( query_data->GetValue( kAwardDB.m_Award[i].m_iCategory, sizeof(int) ) );
			PACKET_GUARD_BREAK( query_data->GetValue( kAwardDB.m_Award[i].m_iCount, sizeof(int) ) );
			PACKET_GUARD_BREAK( query_data->GetValue( kAwardDB.m_Award[i].m_iPoint, sizeof(int) ) );
		} 
		m_vAwardList.push_back( kAwardDB );
	}	
	LOOP_GUARD_CLEAR();
}

void ioAward::SaveData()
{	
	if( !m_pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioAward::SaveData() User NULL!!"); 
		return;
	}

	// 경험치 & 레벨 
	if( m_bAwardChange )
	{
		g_DBClient.OnUpdateAwardExpert( m_pUser->GetUserDBAgentID(), m_pUser->GetAgentThreadID(), m_pUser->GetUserIndex(), GetAwardLevel(), GetAwardExp() );
		LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "SaveAwardExpert(%s:%d(%d))", m_pUser->GetPublicID().c_str(), m_iAwardLevel, m_iAwardExp );
		m_bAwardChange = false;
	}
	
	if( m_vAwardList.empty() ) return;

	vAWARDDB::iterator iter, iEnd;
	iEnd = m_vAwardList.end();
	for(iter = m_vAwardList.begin();iter != iEnd;iter++)
	{
		AWARDDB &kAwardDB = *iter;
		if( kAwardDB.m_bChange )
		{
			std::vector<int> contents;
			contents.reserve(MAX_SLOT * 3);
			for(int i = 0;i < MAX_SLOT;i++)
			{
				contents.push_back( kAwardDB.m_Award[i].m_iCategory );
				contents.push_back( kAwardDB.m_Award[i].m_iCount );
				contents.push_back( kAwardDB.m_Award[i].m_iPoint );
			}

			if( kAwardDB.m_dwIndex == NEW_INDEX )
			{
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "SaveAward(%s:%d) None Index", m_pUser->GetPublicID().c_str(), kAwardDB.m_dwIndex );
				//LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s", szContent );
			}
			else
			{
				g_DBClient.OnUpdateAwardData( m_pUser->GetUserDBAgentID(), m_pUser->GetAgentThreadID(), m_pUser->GetPublicID(), kAwardDB.m_dwIndex, contents );
				kAwardDB.m_bChange = false;
				LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "SaveAward(%s:%d)", m_pUser->GetPublicID().c_str(), kAwardDB.m_dwIndex );
			}
		}		
	}
}

void ioAward::SetAwardExpert( int iLevel, int iExp )
{
	if( !m_pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioAward::SetAwardExpert() User NULL!!"); 
		return;
	}
	m_iAwardLevel = iLevel;
	m_iAwardExp   = iExp;

	SP2Packet kPacket( STPK_AWARD_DATA );  

	PACKET_GUARD_VOID( kPacket.Write(m_iAwardLevel) );
	PACKET_GUARD_VOID( kPacket.Write(m_iAwardExp) );

	m_pUser->SendMessage( kPacket );
}

void ioAward::AddAward( int iCategory, int iPoint )
{
	vAWARDDB::iterator iter, iEnd;
	iEnd = m_vAwardList.end();

	// 이미 수상 받은 내역이있는지 확인
	for(iter = m_vAwardList.begin();iter != iEnd;iter++)
	{
		AWARDDB &kAwardDB = *iter;
		for(int i = 0;i < MAX_SLOT;i++)
		{
			if( kAwardDB.m_Award[i].m_iCategory == iCategory )
			{
				kAwardDB.m_Award[i].m_iCount++;
				kAwardDB.m_Award[i].m_iPoint += iPoint;
				kAwardDB.m_bChange = true;
				return;
			}
		}		
	}

	// 빈 수상 슬롯이 있는지 확인
	for(iter = m_vAwardList.begin();iter != iEnd;iter++)
	{
		AWARDDB &kAwardDB = *iter;
		for(int i = 0;i < MAX_SLOT;i++)
		{
			if( kAwardDB.m_Award[i].m_iCategory == 0 )
			{
				kAwardDB.m_Award[i].m_iCategory = iCategory;
				kAwardDB.m_Award[i].m_iCount    = 1;
				kAwardDB.m_Award[i].m_iPoint    = iPoint;
				kAwardDB.m_bChange = true;
				return;
			}
		}		
	}

	// 슬롯 추가
	AWARDDB kAwardDB;
	kAwardDB.m_dwIndex = NEW_INDEX;
	kAwardDB.m_Award[0].m_iCategory = iCategory;
	kAwardDB.m_Award[0].m_iCount    = 1;
	kAwardDB.m_Award[0].m_iPoint    = iPoint;
	m_vAwardList.push_back( kAwardDB );
	InsertDBAward( kAwardDB );
}

bool ioAward::AddAwardExp( int iExp )
{
	if( m_pUser == NULL ) return false;
	if( iExp == 0 ) return false;
	
	m_bAwardChange = true;
	int iNextExp = g_LevelMgr.GetNextAwardupExp( GetAwardLevel() );
	m_iAwardExp += iExp;
	if( iNextExp <= m_iAwardExp )
	{
		m_iAwardLevel++;
		//
		int iRemainExp = m_iAwardExp - iNextExp;
		m_iAwardExp = 0;
		if( iRemainExp > 0 )
			AddAwardExp( iRemainExp );			
		return true;
	}
	return false;
}

int ioAward::GetAwardLevel()
{
	return m_iAwardLevel;
}

int ioAward::GetAwardExp()
{
	return m_iAwardExp;
}

void ioAward::FillMoveData( SP2Packet &rkPacket )
{
	rkPacket << m_iAwardLevel << m_iAwardExp << m_bAwardChange;

	rkPacket << (int)m_vAwardList.size();
	vAWARDDB::iterator iter, iEnd;
	iEnd = m_vAwardList.end();
	for(iter = m_vAwardList.begin();iter != iEnd;iter++)
	{
		AWARDDB &kAwardDB = *iter;
		rkPacket << kAwardDB.m_dwIndex << kAwardDB.m_bChange;
		for(int i = 0;i < MAX_SLOT;i++)
			rkPacket << kAwardDB.m_Award[i].m_iCategory << kAwardDB.m_Award[i].m_iCount << kAwardDB.m_Award[i].m_iPoint;
	}
}

void ioAward::ApplyMoveData( SP2Packet &rkPacket, bool bDummyNode )
{
	rkPacket >> m_iAwardLevel >> m_iAwardExp >> m_bAwardChange;

	int iSize;
	rkPacket >> iSize;
	for(int i = 0;i < iSize;i++)
	{
		AWARDDB kAwardDB;
		rkPacket >> kAwardDB.m_dwIndex >> kAwardDB.m_bChange;
		for(int j = 0;j < MAX_SLOT;j++)
			rkPacket >> kAwardDB.m_Award[j].m_iCategory >> kAwardDB.m_Award[j].m_iCount >> kAwardDB.m_Award[j].m_iPoint;
		if( kAwardDB.m_dwIndex == NEW_INDEX && !bDummyNode )
			g_DBClient.OnSelectAwardIndex( m_pUser->GetUserDBAgentID(), m_pUser->GetAgentThreadID(), m_pUser->GetGUID(), m_pUser->GetPublicID(), m_pUser->GetUserIndex() );
		m_vAwardList.push_back( kAwardDB );
	}	
}