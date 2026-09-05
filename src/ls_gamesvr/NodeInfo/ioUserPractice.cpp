
#include <stdafx.h>


#include "../QueryData/QueryResultData.h"
#include "../DataBase/DBClient.h"

#include "ioUserPractice.h"
#include "User.h"
#include "PracticeManager.h"

extern CLog TradeLOG;
ioUserPractice::ioUserPractice()
{
	m_dwUserIndex = 0;
	Initialize( NULL );
}

ioUserPractice::~ioUserPractice()
{
}

void ioUserPractice::Initialize( User* pUser )
{
	m_pUser				= pUser;
	if(NULL != m_pUser)
	{
		m_dwUserIndex = m_pUser->GetUserIndex();
		TradeLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Practice Initialize - [UserIndex %d]", m_dwUserIndex);
 	}
	InitData();
}

void ioUserPractice::InitData()
{
	m_bApplyMove = false;
	m_mapPracticeList.clear();
	m_vPracticeRankList.clear();

	m_PracticeEndTime = boost::posix_time::microsec_clock::local_time();
	m_iAbusingCount = 0;

	if(NULL != m_pUser)
	{
		TradeLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Practice InitData - [UserIndex %d]", m_dwUserIndex);
 	}
}

void ioUserPractice::InitCount(DWORD dwAgentID, DWORD dwThreadID, DWORD dwUserIndex)
{
	MAPPRACTICE_iter iter,iEnd;
	iEnd = m_mapPracticeList.end();
	for(iter = m_mapPracticeList.begin() ; iter != iEnd ; iter++)
	{
		SPractice &kPractice = (*iter).second;

		LSC_Practice* pkPractice = g_PracticeMgr.GetLSCPractice(kPractice.m_dwID);
		if(NULL == pkPractice)
		{
			continue;
		}

		kPractice.m_dwCount = pkPractice->FreeAdmission;
		TradeLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Practice InitCount - [UserIndex %d] [Index:%d] [m_dwGrade:%d] [m_dwCount:%d] [m_dwTime:%d] [m_dwRank:%d]",
								 dwUserIndex, kPractice.m_dwID, kPractice.m_dwGrade, kPractice.m_dwCount, kPractice.m_dwTime, kPractice.m_dwRank);

		g_DBClient.OnInitPracticeData( dwAgentID, dwThreadID, dwUserIndex, kPractice.m_dwID, kPractice.m_dwCount );
	}
}

DWORD ioUserPractice::GetPracticeCount()
{
	return m_mapPracticeList.size();
}

DWORD ioUserPractice::GetPracticeAdmissionCount()
{
	int iCnt = 0;

	MAPPRACTICE_iter iter,iEnd;
	iEnd = m_mapPracticeList.end();
	for(iter = m_mapPracticeList.begin() ; iter != iEnd ; iter++)
	{
		SPractice &kPractice = (*iter).second;
		iCnt += kPractice.m_dwCount;
	}
	return iCnt;
}

void ioUserPractice::FillMoveData( SP2Packet &rkPacket )
{
	int iCnt = m_mapPracticeList.size();
	PACKET_GUARD_VOID( rkPacket.Write( iCnt ) );

	MAPPRACTICE_iter iter,iEnd;
	iEnd = m_mapPracticeList.end();
	for(iter = m_mapPracticeList.begin() ; iter != iEnd ; iter++)
	{
		SPractice kPractice = (*iter).second;
		PACKET_GUARD_VOID( rkPacket.Write( kPractice.m_dwID ) );
		PACKET_GUARD_VOID( rkPacket.Write( kPractice.m_dwGrade ) );
		PACKET_GUARD_VOID( rkPacket.Write( kPractice.m_dwCount ) );
		PACKET_GUARD_VOID( rkPacket.Write( kPractice.m_dwTime ) );
		PACKET_GUARD_VOID( rkPacket.Write( kPractice.m_dwRank ) );

		if(NULL != m_pUser)
		{
			TradeLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Practice FillMoveData - [UserIndex %d] [Index:%d] [m_dwGrade:%d] [m_dwCount:%d] [m_dwTime:%d] [m_dwRank:%d]",
									 m_dwUserIndex, kPractice.m_dwID, kPractice.m_dwGrade, kPractice.m_dwCount, kPractice.m_dwTime, kPractice.m_dwRank);
		}
	}

	int iCount = m_vPracticeRankList.size();
	PACKET_GUARD_VOID( rkPacket.Write( iCount ) );

	std::vector< SPracticeRank >::iterator	viter = m_vPracticeRankList.begin();
	std::vector< SPracticeRank >::iterator	viterEnd = m_vPracticeRankList.end();

	while( viter != viterEnd )
	{
		SPracticeRank kPracticeRank = (*viter);
		PACKET_GUARD_VOID( rkPacket.Write( kPracticeRank.m_strStartDate ) );
		PACKET_GUARD_VOID( rkPacket.Write( kPracticeRank.m_strEndDate ) );
		PACKET_GUARD_VOID( rkPacket.Write( kPracticeRank.m_dwID ) );	
		PACKET_GUARD_VOID( rkPacket.Write( kPracticeRank.m_dwRank ) );
		PACKET_GUARD_VOID( rkPacket.Write( (int)kPracticeRank.m_vSPracticePresent.size() ) );

		std::vector< SPracticePresent >::iterator	PP_iter = kPracticeRank.m_vSPracticePresent.begin();
		std::vector< SPracticePresent >::iterator	PP_iterEnd = kPracticeRank.m_vSPracticePresent.end();

		while( PP_iter != PP_iterEnd )
		{
			SPracticePresent kPracticePresent = (*PP_iter);
			PACKET_GUARD_VOID( rkPacket.Write( kPracticePresent.m_dwPresentType ) );
			PACKET_GUARD_VOID( rkPacket.Write( kPracticePresent.m_dwCode ) );
			PACKET_GUARD_VOID( rkPacket.Write( kPracticePresent.m_dwValue ) );
			++PP_iter;
		}
		++viter;
	}
}

void ioUserPractice::ApplyMoveData( SP2Packet &rkPacket, bool bDummyNode )
{
	int iCnt = 0;
	m_mapPracticeList.clear();
	PACKET_GUARD_VOID( rkPacket.Read( iCnt ) );
	for(int i = 0;i < iCnt;i++)
	{
		SPractice kPractice;
		PACKET_GUARD_VOID( rkPacket.Read( kPractice.m_dwID ) );
		PACKET_GUARD_VOID( rkPacket.Read( kPractice.m_dwGrade ) );
		PACKET_GUARD_VOID( rkPacket.Read( kPractice.m_dwCount ) );
		PACKET_GUARD_VOID( rkPacket.Read( kPractice.m_dwTime ) );
		PACKET_GUARD_VOID( rkPacket.Read( kPractice.m_dwRank ) );

		if(NULL != m_pUser)
		{
			TradeLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Practice ApplyMoveData - [UserIndex %d] [Index:%d] [m_dwGrade:%d] [m_dwCount:%d] [m_dwTime:%d] [m_dwRank:%d]",
									 m_dwUserIndex, kPractice.m_dwID, kPractice.m_dwGrade, kPractice.m_dwCount, kPractice.m_dwTime, kPractice.m_dwRank);
		}

		m_mapPracticeList.insert(MAPPRACTICE::value_type( kPractice.m_dwID, kPractice ));
	}

	int iCount = 0;
	m_vPracticeRankList.clear();
	PACKET_GUARD_VOID( rkPacket.Read( iCount ) );
	for(int i = 0;i < iCount;i++)
	{
		int iPresentCount = 0;
		SPracticeRank kPracticeRank;
		PACKET_GUARD_VOID( rkPacket.Read( kPracticeRank.m_strStartDate ) );
		PACKET_GUARD_VOID( rkPacket.Read( kPracticeRank.m_strEndDate ) );
		PACKET_GUARD_VOID( rkPacket.Read( kPracticeRank.m_dwID ) );	
		PACKET_GUARD_VOID( rkPacket.Read( kPracticeRank.m_dwRank ) );
		PACKET_GUARD_VOID( rkPacket.Read( iPresentCount ) );

		for(int i = 0; i < iPresentCount; i++)
		{
			SPracticePresent kPracticePresent;
			PACKET_GUARD_VOID( rkPacket.Read( kPracticePresent.m_dwPresentType ) );
			PACKET_GUARD_VOID( rkPacket.Read( kPracticePresent.m_dwCode ) );
			PACKET_GUARD_VOID( rkPacket.Read( kPracticePresent.m_dwValue ) );
			kPracticeRank.m_vSPracticePresent.push_back(kPracticePresent);
		}
		m_vPracticeRankList.push_back(kPracticeRank);
	}
	SetApplyMove(true);
}

void ioUserPractice::AddPractice( SPractice& kPractice )
{
	MAPPRACTICE_iter iter,iEnd;
	iEnd = m_mapPracticeList.end();
	iter = m_mapPracticeList.find( kPractice.m_dwID );

	if( iter != iEnd )
		return;

	if(NULL != m_pUser)
	{
		TradeLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Practice AddPractice - [UserIndex %d] [Index:%d] [m_dwGrade:%d] [m_dwCount:%d] [m_dwTime:%d] [m_dwRank:%d]",
									m_dwUserIndex, kPractice.m_dwID, kPractice.m_dwGrade, kPractice.m_dwCount, kPractice.m_dwTime, kPractice.m_dwRank);
 	}

	m_mapPracticeList.insert(MAPPRACTICE::value_type( kPractice.m_dwID, kPractice ));
}

void ioUserPractice::GetPracticeList( MAPPRACTICE& kPracticeList )
{
	kPracticeList.clear();
	kPracticeList = m_mapPracticeList;
}

void ioUserPractice::SetPractice( DWORD dwID, DWORD dwGrade, DWORD dwCount, DWORD dwTime, DWORD dwRank )
{
	MAPPRACTICE_iter iter,iEnd;
	iEnd = m_mapPracticeList.end();
	iter = m_mapPracticeList.find( dwID );

	if(NULL != m_pUser)
	{
		TradeLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Practice SetPractice - [UserIndex %d] [Index:%d] [m_dwGrade:%d] [m_dwCount:%d] [m_dwTime:%d] [m_dwRank:%d]",
									m_dwUserIndex, dwID, dwGrade, dwCount, dwTime, dwRank);
 	}

	if( iter == iEnd )
	{
		SPractice kPractice;
		kPractice.m_dwID = dwID;
		kPractice.m_dwGrade = dwGrade;
		kPractice.m_dwCount = dwCount;
		kPractice.m_dwTime = dwTime;
		kPractice.m_dwRank = dwRank;
		AddPractice( kPractice );
	}
	else
	{
		SPractice& rPractice = (*iter).second;
		rPractice.m_dwGrade = dwGrade;
		rPractice.m_dwCount = dwCount;
		rPractice.m_dwTime = dwTime;
		rPractice.m_dwRank = dwRank;
	}
}


void ioUserPractice::SetPracticeCount( DWORD dwID, DWORD dwCount )
{
	MAPPRACTICE_iter iter,iEnd;
	iEnd = m_mapPracticeList.end();
	iter = m_mapPracticeList.find( dwID );

	if(NULL != m_pUser)
	{
		TradeLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Practice SetPracticeCount - [UserIndex %d] [Index:%d][m_dwCount:%d]",
									m_dwUserIndex, dwID, dwCount);
 	}

	if( iter == iEnd )
	{
		SPractice kPractice;
		kPractice.m_dwID = dwID;
		kPractice.m_dwGrade = 0;
		kPractice.m_dwCount = dwCount;
		kPractice.m_dwTime = PRACTICETIME;
		kPractice.m_dwRank = PRACTICERANK;
		AddPractice( kPractice );
	}
	else
	{
		SPractice& rPractice = (*iter).second;
		rPractice.m_dwCount = dwCount;
	}
}

void ioUserPractice::SetPracticeTimeRank( DWORD dwID, DWORD dwCount, DWORD dwRank)
{
	MAPPRACTICE_iter iter,iEnd;
	iEnd = m_mapPracticeList.end();
	iter = m_mapPracticeList.find( dwID );

	if(NULL != m_pUser)
	{
		TradeLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Practice SetPracticeTimeRank - [UserIndex %d] [Index:%d][m_dwRank:%d]",
									m_dwUserIndex, dwID, dwRank);
 	}

	if( iter == iEnd )
	{
		SPractice kPractice;
		kPractice.m_dwID = dwID;
		kPractice.m_dwGrade = 0;
		kPractice.m_dwCount = dwCount;
		kPractice.m_dwTime = PRACTICETIME;
		kPractice.m_dwRank = dwRank;
		AddPractice( kPractice );
	}
	else
	{
		SPractice& rPractice = (*iter).second;
		rPractice.m_dwRank = dwRank;
	}
}

void ioUserPractice::AddPracticeRankDate( SPracticeRank& kPracticeRank )
{
	m_vPracticeRankList.push_back(kPracticeRank);
}

void ioUserPractice::GetPracticeRankList( std::vector< SPracticeRank >& kPracticeList )
{
	kPracticeList.clear();
	kPracticeList = m_vPracticeRankList;
}

DWORD ioUserPractice::GetGrade( DWORD dwID )
{
	MAPPRACTICE_iter iter,iEnd;
	iEnd = m_mapPracticeList.end();
	iter = m_mapPracticeList.find( dwID );

	if( iter != iEnd )
	{
		SPractice& rPractice = (*iter).second;
		return rPractice.m_dwGrade;
	}

	return 0;
}

SPractice ioUserPractice::GetPractice( DWORD dwID )
{
	SPractice kPractice;
	MAPPRACTICE_iter iter,iEnd;
	iEnd = m_mapPracticeList.end();
	iter = m_mapPracticeList.find( dwID );
	if( iter != iEnd )
	{
		kPractice = (*iter).second;
		return kPractice;
	}

	LSC_Practice* pkPractice = g_PracticeMgr.GetLSCPractice(dwID);
	if(NULL == pkPractice)
	{
		return kPractice;
	}

	kPractice.m_dwID = dwID;
	kPractice.m_dwCount = pkPractice->FreeAdmission;

	if(NULL != m_pUser)
	{
		TradeLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Practice GetPractice - [UserIndex %d] [Index:%d] [m_dwGrade:%d] [m_dwCount:%d] [m_dwTime:%d] [m_dwRank:%d]",
			m_dwUserIndex, kPractice.m_dwID, kPractice.m_dwGrade, kPractice.m_dwCount, kPractice.m_dwTime, kPractice.m_dwRank);
 	}

	return kPractice;
}

void ioUserPractice::SetLastTime(CTime LastTime)
{
	m_LastTime = LastTime;
}

CTime ioUserPractice::GetLastTime()
{
	return m_LastTime;
}

void ioUserPractice::SetSendInfo(bool bSendInfo)
{
	m_bSendInfo = bSendInfo;
}

bool ioUserPractice::IsSendInfo()
{
	return m_bSendInfo;
}

void ioUserPractice::SetSendRank(bool bSendRank)
{
	m_bSendRank = bSendRank;
}

bool ioUserPractice::IsSendRank()
{
	return m_bSendRank;
}

void ioUserPractice::SetApplyMove(bool bApplyMove)
{
	m_bApplyMove = bApplyMove;
}

bool ioUserPractice::IsApplyMove()
{
	return m_bApplyMove;
}
