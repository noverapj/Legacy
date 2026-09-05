#include "stdafx.h"
#include "ioUserRollBook.h"
#include "RollBookManager.h"
#include "../QueryData/QueryResultData.h"
#include "User.h"
#include "../EtcHelpFunc.h"
#include "../DataBase/DBClient.h"
#include "../DataBase/LogDBClient.h"

ioUserRollBook::ioUserRollBook()
{
	Init(NULL);
}

ioUserRollBook::~ioUserRollBook()
{}

void ioUserRollBook::Init(User* pUser)
{
	m_pUser					= pUser;
	m_iCurPage				= 0;
	m_iCurLine				= 0;
	m_dwCheckDate			= 0;
	m_eLoginRollBookType	= RBT_NONE;
	m_bUpdate				= FALSE;
}

void ioUserRollBook::Destroy()
{}

void ioUserRollBook::DBToData(CQueryResultData *query_data)
{
	if( !m_pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s User NULL!!", __FUNCTION__ ); 
		return;
	}

	int iTableIndex		= 0;
	int iNextStamp		= 0;

	//처음 접속 유저.
	if( !query_data->IsExist() )
	{
		if( GetRollBookType() != RBT_NONE )
		{
			iTableIndex = g_RollBookMgr.GetFirstStartTable(GetRollBookType());
			if( 0 == iTableIndex )
				return;

			iNextStamp = 1;
		}
		else
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[warning][rollbook] user type is invalid : %s", m_pUser->GetPublicID().c_str() );
			return;
		}
	}
	else
	{
		//DB data Get
		DBTIMESTAMP dts;
		int iPrevTableIndex = 0;
		int iPrevStamp		= 0;

		PACKET_GUARD_VOID( query_data->GetValue( iPrevStamp, sizeof(DWORD) ) );
		PACKET_GUARD_VOID( query_data->GetValue( iPrevTableIndex, sizeof(DWORD) ) );
		PACKET_GUARD_VOID( query_data->GetValue( (char*)&dts, sizeof(DBTIMESTAMP) ) );

		if( !Help::IsAvailableDate(dts.year - 2000, dts.month, dts.day) )
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[warning][rollbook] recent reward date is wrong : %s (%d:%d:%d)", m_pUser->GetPublicID().c_str(), dts.year, dts.month, dts.day );
			return;
		}

		CTime cRecentRewardDate(dts.year, dts.month, dts.day, dts.hour, dts.minute, 0);
		SetCheckDate(cRecentRewardDate.GetTime());
		SetCurTable(iPrevTableIndex);
		SetAccumulationCount(iPrevStamp);

		if( IsRenewalDate() )
		{
			int iType = RBR_PROGRESS;
			
			//복귀 유저, 리셋 유저 체크
			if( g_RollBookMgr.IsReturnUser(cRecentRewardDate) )
				iType = RPR_CHANGE_RETURN;
			else if( g_RollBookMgr.IsReset(cRecentRewardDate, iPrevTableIndex) )
				iType = RPR_CHANGE_RESET;

			GetNextStep(iType, iTableIndex, iNextStamp);
			if( 0 == iTableIndex )
				return;
		}
		else
		{
			//이미 오늘 접속 했었음.
			SP2Packet kPacket( STPK_ROLLBOOK_ATTEND );
			PACKET_GUARD_VOID( kPacket.Write(ROLLBOOK_ATTEND_DONE) );
			PACKET_GUARD_VOID( kPacket.Write(GetCheckDate()) );
			PACKET_GUARD_VOID( kPacket.Write(GetCurTable()) );
			PACKET_GUARD_VOID( kPacket.Write(GetAccumulationCount()) );
			m_pUser->SendMessage(kPacket);
			return;
		}
	}

	SetSQLUpdateFlag(TRUE);
	g_DBClient.OnUpdateRollBookData(m_pUser->GetUserDBAgentID(), m_pUser->GetAgentThreadID(), m_pUser->GetUserIndex(), iTableIndex, iNextStamp );
	return;
}

void ioUserRollBook::ResultSQLUpdateRollBook(CQueryResultData *query_data)
{
	if( !m_pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[warning][rollbook] user is not exist" ); 
		return;
	}

	int iRollBookIndex	= 0;
	int iCount			= 0;
	int iResult			= 0;
	DBTIMESTAMP dts;

	PACKET_GUARD_VOID( query_data->GetValue( iRollBookIndex, sizeof(int) ) );
	PACKET_GUARD_VOID( query_data->GetValue( iCount, sizeof(int) ) );
	PACKET_GUARD_VOID( query_data->GetValue( iResult, sizeof(int) ) );

	if( iResult != 0 )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"[warning][rollbook] user rollbook update failed : [%d:%d]",  m_pUser->GetUserIndex(), iResult);
		return;
	}

	PACKET_GUARD_VOID( query_data->GetValue( (char*)&dts, sizeof(DBTIMESTAMP) ) );

	if( !Help::IsAvailableDate(dts.year - 2000, dts.month, dts.day) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[warning][rollbook] recent reward date is wrong : [%d, %d:%d:%d]", m_pUser->GetUserIndex(), dts.year, dts.month, dts.day );
		return;
	}

	CTime cRewardDate(dts.year, dts.month, dts.day, dts.hour, dts.minute, 0);
	DWORD dwRewardDate = cRewardDate.GetTime();

	SetCurTable(iRollBookIndex);
	SetAccumulationCount(iCount);
	SetCheckDate(dwRewardDate);

	if( !g_RollBookMgr.SendReward(m_pUser, iRollBookIndex, iCount) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[warning][rollbook] send reward fail : [%d,%d,%d]", iRollBookIndex, iCount, m_pUser->GetUserIndex() );
		return;
	}

	SP2Packet kPacket( STPK_ROLLBOOK_ATTEND );
	PACKET_GUARD_VOID( kPacket.Write(ROLLBOOK_ATTEND_OK) );
	PACKET_GUARD_VOID( kPacket.Write(dwRewardDate) );
	PACKET_GUARD_VOID( kPacket.Write(iRollBookIndex) );
	PACKET_GUARD_VOID( kPacket.Write(iCount) );
	m_pUser->SendMessage(kPacket);

	SQLUpdateLogInfo();
	SetSQLUpdateFlag(FALSE);
}

void ioUserRollBook::FillMoveData(SP2Packet& kPacket)
{
	PACKET_GUARD_VOID(kPacket.Write(GetCurTable()));
	PACKET_GUARD_VOID(kPacket.Write(GetAccumulationCount()));
	PACKET_GUARD_VOID(kPacket.Write(GetCheckDate()));
	PACKET_GUARD_VOID(kPacket.Write(GetRollBookType()));
}

void ioUserRollBook::ApplyMoveData(SP2Packet& kPacket)
{
	int iRollBookType = 0;

	PACKET_GUARD_VOID(kPacket.Read(m_iCurPage));
	PACKET_GUARD_VOID(kPacket.Read(m_iCurLine));
	PACKET_GUARD_VOID(kPacket.Read(m_dwCheckDate));
	PACKET_GUARD_VOID(kPacket.Read(iRollBookType));

	SetRollBookType(iRollBookType);
}

void ioUserRollBook::SQLUpdateRollBook()
{
	if( !m_pUser )
		return;

	g_DBClient.OnUpdateRollBookData(m_pUser->GetUserDBAgentID(), m_pUser->GetAgentThreadID(), m_pUser->GetUserIndex(), GetCurTable(), GetAccumulationCount() );
}

DWORD ioUserRollBook::GetStandardDate(DWORD dwDate)
{
	CTime cDate(dwDate);
	int iRenewalHour = g_RollBookMgr.GetRenewalHour();

	CTime cStandardDate(cDate.GetYear(), cDate.GetMonth(), cDate.GetDay(), iRenewalHour, 0, 0);
	CTimeSpan cGap(1,0,0,0);

	if( cDate.GetHour() < iRenewalHour )
		cStandardDate = cStandardDate - cGap;

	return (DWORD)cStandardDate.GetTime();
}

BOOL ioUserRollBook::IsRenewalDate()
{
	CTime cCurTime = CTime::GetCurrentTime();
	if( 0 == m_dwCheckDate )
		return FALSE;

	DWORD dwTodayStandardDate = GetStandardDate(cCurTime.GetTime());
	DWORD dwPrevStandardDate = GetStandardDate(m_dwCheckDate);

	if( dwTodayStandardDate > dwPrevStandardDate )
		return TRUE;

	return FALSE;
	/*int iRenewalHour = g_RollBookMgr.GetRenewalHour();

	CTime cCurTime = CTime::GetCurrentTime();
	CTime cCheckTime(cCurTime.GetYear(), cCurTime.GetMonth(), cCurTime.GetDay(), iRenewalHour, 0, 0);
	CTime cAgreeDate( m_dwCheckDate );

	if( cAgreeDate >= cCheckTime )
		return FALSE;

	CTimeSpan cGap = cCheckTime - cAgreeDate;
	if( cGap.GetDays() >= 2 )
		return TRUE;
	else if( cGap.GetDays() >= 1 )
	{
		if( cCurTime.GetHour() >= iRenewalHour )
			return TRUE;
	}

	if( cCurTime.GetHour() < iRenewalHour )
		return FALSE;
	
	return TRUE;*/
}

void ioUserRollBook::GetNextStep(int iRollBookType, int& iTableIndex, int& iAccumulationCount)
{
 	int iNextRollBook	  = 0;
 	int iNextStamp		  = 0;

	switch( iRollBookType )
	{
	case RPR_CHANGE_RESET:
		{
			//리셋
			iNextRollBook = g_RollBookMgr.GetNextStartTable(GetCurTable(), TRUE);
			iNextStamp = 1;
		}
		break;
	case RPR_CHANGE_RETURN:
		{
			//복귀
			iNextRollBook = g_RollBookMgr.GetFirstStartTable(RBT_RETURN);
			iNextStamp = 1;
		}
		break;
	case RBR_PROGRESS:
		{
			//정상진행
			iNextStamp = g_RollBookMgr.GetNextRollBookLine(GetCurTable(), GetAccumulationCount());

			if( -1 == iNextStamp )
				return;
			else if( 0 == iNextStamp )
			{
				//다음 테이블로 변경
				iNextRollBook = g_RollBookMgr.GetNextStartTable(GetCurTable(), FALSE);
				iNextStamp = 1;
			}
			else
				iNextRollBook = GetCurTable();
		}
		break;
	}
	
	if( 0 == iNextRollBook )
		return;

	iTableIndex			= iNextRollBook;
	iAccumulationCount	= iNextStamp;
}

void ioUserRollBook::ProgressRollBook(SP2Packet& kPacket)
{
	if( !m_pUser )
		return;

	int iProgressCnt = 0;
	PACKET_GUARD_VOID( kPacket.Read(iProgressCnt) );

	if( !COMPARE(iProgressCnt, 1, 8) )
		return;

	int iTableIndex = 0;
	int iNextStamp	= 0;

	for( int i = 0; i < iProgressCnt; i++ )
	{
		//int iType = g_RollBookMgr.GetRollBookType(GetCurTable());
		//if( 0 == iType )
			//return;
		
		GetNextStep(RBR_PROGRESS, iTableIndex, iNextStamp);
 		SetCurTable(iTableIndex);
 		SetAccumulationCount(iNextStamp);
		g_DBClient.OnUpdateRollBookData(m_pUser->GetUserDBAgentID(), m_pUser->GetAgentThreadID(), m_pUser->GetUserIndex(), iTableIndex, iNextStamp );
// 		g_RollBookMgr.SendReward(m_pUser, iTableIndex, iNextStamp);
	}
}

void ioUserRollBook::SQLUpdateLogInfo()
{
	RollBookManager::RewardInfo stReward;
	g_RollBookMgr.GetRewardInfo(GetCurTable(), GetAccumulationCount(), stReward);
	g_LogDBClient.OnInsertGameLogInfo(LogDBClient::GLT_ATTENDANCE, m_pUser, 0, GetCurTable(), 1, stReward.iType, stReward.iValue1, stReward.iValue2, GetAccumulationCount(), NULL);
}