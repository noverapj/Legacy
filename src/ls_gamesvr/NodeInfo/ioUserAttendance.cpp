
#include "stdafx.h"

#include "../QueryData/QueryResultData.h"
#include "../DataBase/DBClient.h"
#include "../MainProcess.h"
#include "../EtcHelpFunc.h"

#include "User.h"
#include "ioUserAttendance.h"
#include "UserNodeManager.h"

#include "ioAttendanceRewardManager.h"
#include <time.h>

ioUserAttendance::ioUserAttendance()
{
	Initialize( NULL );
}

ioUserAttendance::~ioUserAttendance()
{
}

void ioUserAttendance::Initialize( User *pUser )
{
	m_pUser = pUser;
	m_bSelectedAttendanceDB = false;
	m_dwConnectTime = 0;
}

void ioUserAttendance::DBtoData( CQueryResultData *query_data )
{
	if( !m_pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s User NULL!!", __FUNCTION__ ); 
		return;
	}

	m_bSelectedAttendanceDB = true;
	m_AttendanceRecord.clear();

	LOOP_GUARD();	
	while( query_data->IsExist() )
	{
		DWORD dwTableIdx = 0;
		DWORD dwUserIdx  = 0;
		DBTIMESTAMP dts;

		PACKET_GUARD_BREAK( query_data->GetValue( dwTableIdx, sizeof(DWORD) ) );
		PACKET_GUARD_BREAK( query_data->GetValue( dwUserIdx, sizeof(DWORD) ) );
		PACKET_GUARD_BREAK( query_data->GetValue( (char*)&dts, sizeof(DBTIMESTAMP) ) );

		m_AttendanceRecord.push_back( Help::ConvertYYMMDDToDate( dts.year, dts.month, dts.day ) );
	}
	LOOP_GUARD_CLEAR();

	//쿼리에서 날짜 삽입
	if( IsThisMonthFirstConnect() )
	{
		//이달의 첫번째 접속이면 Delete쿼리를 날림
		g_DBClient.OnDeleteAttendanceRecord( m_pUser->GetUserDBAgentID(), m_pUser->GetAgentThreadID(), m_pUser->GetUserIndex() );
		m_AttendanceRecord.clear();
		SendAttendanceRecord();
	}
	else
	{
		//오늘 접속을 하지 않았다면
		if( !IsTodayAttendanceCheck() )
		{
			SendAttendanceRecord();
		}
	}
}

void ioUserAttendance::FillMoveData( SP2Packet &rkPacket )
{
	//사용하지 않음
}

void ioUserAttendance::ApplyMoveData( SP2Packet &rkPacket, bool bDummyNode )
{
	//사용하지 않음

// 	if( !m_pUser )
// 	{
// 		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s User NULL!!", __FUNCTION__ ); 
// 		return;
// 	}
// 
// 	if( !m_pUser->IsDeveloper() )
// 		return;	
// 
// 	OnSelectAttendanceRecord();	
}

void ioUserAttendance::OnSelectAttendanceRecord()
{
	if( !m_pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s User NULL!!", __FUNCTION__ ); 
		return;
	}

	if( m_pUser->GetGradeLevel() == 0 )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s:%s is not available grade level", __FUNCTION__, m_pUser->GetPublicID().c_str() ); 
		return;	
	}

	CTime CurrTime = CTime::GetCurrentTime();
	m_dwConnectTime = Help::ConvertYYMMDDToDate( CurrTime.GetYear(), CurrTime.GetMonth(), CurrTime.GetDay() );
	g_DBClient.OnSelectAttendanceRecord( m_pUser->GetUserDBAgentID(), m_pUser->GetAgentThreadID(), m_pUser->GetUserIndex() );	
}

bool ioUserAttendance::IsThisMonthFirstConnect()
{
	int iYear  = m_dwConnectTime / 10000;
	int iMonth = (m_dwConnectTime % 10000) / 100;
	int iDay   = m_dwConnectTime % 100;
	if( !Help::IsAvailableDate( iYear, iMonth, iDay ) )
		return false;

	for( AttendanceRecord::iterator iter = m_AttendanceRecord.begin(); iter != m_AttendanceRecord.end(); ++iter )
	{
		DWORD dwRecordTime = *iter;

		int iRecordYear  = dwRecordTime / 10000;
		int iRecordMonth = (dwRecordTime % 10000) / 100;
		int iRecordDay   = dwRecordTime % 100;
		if( !Help::IsAvailableDate( iRecordYear, iRecordMonth, iRecordDay ) )
			continue;

		if( iRecordYear == iYear && iRecordMonth == iMonth )
			return false;
	}

	return true;
}

bool ioUserAttendance::IsTodayAttendanceCheck()
{	
	for( AttendanceRecord::iterator iter = m_AttendanceRecord.begin(); iter != m_AttendanceRecord.end(); ++iter )
	{
		if( *iter == m_dwConnectTime )
			return true;
	}

	return false;
}

bool ioUserAttendance::HasDay( CTime Time )
{
	for( AttendanceRecord::iterator iter = m_AttendanceRecord.begin(); iter != m_AttendanceRecord.end(); ++iter )
	{
		if( *iter == m_dwConnectTime )
			return true;
	}

	return false;
}

void ioUserAttendance::CheckTodayAttendance( SP2Packet& rkPacket )
{
	if( !m_pUser )
	{
		SP2Packet kReturn( STPK_ATTENDANCE_CHECK );
		PACKET_GUARD_VOID( kReturn.Write( ATTENDANCE_CHECK_FAIL ) );
		m_pUser->SendMessage( kReturn );	

		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s User NULL!!", __FUNCTION__ ); 
		return;
	}

	if( !m_bSelectedAttendanceDB )
	{
		SP2Packet kReturn( STPK_ATTENDANCE_CHECK );
		PACKET_GUARD_VOID( kReturn.Write( ATTENDANCE_RECORD_SELECT_DB_WAIT ) );
		m_pUser->SendMessage( kReturn );

		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s-%s DB select waitng", __FUNCTION__, m_pUser->GetPublicID().c_str() ); 
		return;
	}

	if( m_dwConnectTime == 0 )
	{
		SP2Packet kReturn( STPK_ATTENDANCE_CHECK );
		PACKET_GUARD_VOID( kReturn.Write( ATTENDANCE_RECORD_SELECT_DB_WAIT ) );
		m_pUser->SendMessage( kReturn );

		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s-%s ConnectTime = 0", __FUNCTION__, m_pUser->GetPublicID().c_str() );
		return;
	}

	if( IsTodayAttendanceCheck() )
	{
		SP2Packet kReturn( STPK_ATTENDANCE_CHECK );
		PACKET_GUARD_VOID( kReturn.Write( ATTENDANCE_RECORD_EXIST ) );
		m_pUser->SendMessage( kReturn );

		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s-%s DB select waitng", __FUNCTION__, m_pUser->GetPublicID().c_str() );
		return;
	}

	int iYear  = m_dwConnectTime / 10000;
	int iMonth = (m_dwConnectTime % 10000) / 100;
	int iDay   = m_dwConnectTime % 100;

	CTime CurrTime( 2000 + iYear, iMonth, iDay, 0, 0, 0 );
	if( !Help::IsAvailableDate( iYear, iMonth, iDay ) )
	{
		SP2Packet kReturn( STPK_ATTENDANCE_CHECK );
		PACKET_GUARD_VOID( kReturn.Write( ATTENDANCE_CHECK_FAIL ) );
		m_pUser->SendMessage( kReturn );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s:%s not available date : %d/%d/%d", __FUNCTION__, m_pUser->GetPublicID().c_str(), iYear, iMonth, iDay ); 
		return;
	}

	g_DBClient.OnInsertAttendanceRecord( m_pUser->GetUserDBAgentID(), m_pUser->GetAgentThreadID(), m_pUser->GetUserIndex(), CurrTime );

	m_AttendanceRecord.push_back( m_dwConnectTime );

	int iTodayPresentType = 0;
	int iTodayValue1 = 0;
	int iTodayValue2 = 0;
	g_AttendanceRewardMgr.SendTodayAttanceReward( m_pUser, m_dwConnectTime, iTodayPresentType, iTodayValue1, iTodayValue2 );

	int iAccruePresentType = 0;
	int iAccrueValue1 = 0;
	int iAccrueValue2 = 0;
	g_AttendanceRewardMgr.SendAccurePeriodReward( m_pUser, iYear, iMonth, (int)m_AttendanceRecord.size(), iAccruePresentType, iAccrueValue1, iAccrueValue2 );

	SP2Packet kReturn( STPK_ATTENDANCE_CHECK );
	PACKET_GUARD_VOID( kReturn.Write( ATTENDANCE_CHECK_OK ) );
	PACKET_GUARD_VOID( kReturn.Write( m_dwConnectTime ) );
	PACKET_GUARD_VOID( kReturn.Write( iTodayPresentType ) );
	PACKET_GUARD_VOID( kReturn.Write( iTodayValue1 ) );
	PACKET_GUARD_VOID( kReturn.Write( iTodayValue2 ) );
	PACKET_GUARD_VOID( kReturn.Write( iAccruePresentType ) );
	PACKET_GUARD_VOID( kReturn.Write( iAccrueValue1 ) );
	PACKET_GUARD_VOID( kReturn.Write( iAccrueValue2 ) );
	m_pUser->SendMessage( kReturn );
}

void ioUserAttendance::SendAttendanceRecord()
{
	if( !m_pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s User NULL!!", __FUNCTION__ ); 
		return;
	}

	if( !m_bSelectedAttendanceDB )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s-%s DB select waitng", __FUNCTION__, m_pUser->GetPublicID().c_str() ); 
		return;
	}

	if( m_dwConnectTime == 0 )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s-%s ConnectTime = 0", __FUNCTION__, m_pUser->GetPublicID().c_str() );
		return;
	}

	int iSize = m_AttendanceRecord.size();


	SP2Packet kReturn( STPK_ATTENDANCE_RECORD );
	PACKET_GUARD_VOID( kReturn.Write( m_dwConnectTime ) );
	PACKET_GUARD_VOID( kReturn.Write( iSize ) );
	for( int i = 0; i < iSize; i++ )
	{
		PACKET_GUARD_VOID( kReturn.Write( m_AttendanceRecord[i] ) );
	}

	m_pUser->SendMessage( kReturn );
}

void ioUserAttendance::OnMacroAttendancePrevMonth( int iDayCount )
{
	if( !m_pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s User NULL!!", __FUNCTION__ ); 
		return;
	}

	if( !m_pUser->IsDeveloper() )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s:%s is Not Developer", __FUNCTION__, m_pUser->GetPublicID().c_str() ); 
		return;
	}

	int iYear  = m_dwConnectTime % 10000;
	int iMonth = (m_dwConnectTime % 10000) / 100;

	CTime NewTime( 2000 + iYear, iMonth, 1, 0, 0, 0 );
	for( int i = 1; i <= iDayCount; i++ )
	{
		CTimeSpan AddTime( i, 0, 0, 0 );
		CTime NewAddTime = NewTime - AddTime;

		if( NewTime.GetMonth() == NewAddTime.GetMonth() )
			continue;

		if( HasDay( NewAddTime ) )
			continue;

		g_DBClient.OnInsertAttendanceRecord( m_pUser->GetUserDBAgentID(), m_pUser->GetAgentThreadID(), m_pUser->GetUserIndex(), NewAddTime );
		m_AttendanceRecord.push_back( Help::ConvertYYMMDDToDate( NewAddTime.GetYear(), NewAddTime.GetMonth(), NewAddTime.GetDay() ) );
	}

	SendAttendanceRecord();
	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s:%s", __FUNCTION__, m_pUser->GetPublicID().c_str() ); 
}

void ioUserAttendance::OnMacroAttendanceAddDay( int iDayCount )
{
	if( !m_pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s User NULL!!", __FUNCTION__ ); 
		return;
	}

	if( !m_pUser->IsDeveloper() )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s:%s is Not Developer", __FUNCTION__, m_pUser->GetPublicID().c_str() ); 
		return;
	}

	int iYear  = m_dwConnectTime / 10000;	
	int iMonth = m_dwConnectTime % 10000 / 100;
	CTime NewTime( 2000 + iYear, iMonth, 1, 0, 0, 0 );

	for( int i = 1; i <= iDayCount; i++ )
	{
		CTimeSpan AddTime( i, 0, 0, 0 );
		CTime NewAddTime = NewTime + AddTime;
		if( NewTime.GetMonth() != NewAddTime.GetMonth() )
			continue;

		if( HasDay( NewAddTime ) )
			continue;

		g_DBClient.OnInsertAttendanceRecord( m_pUser->GetUserDBAgentID(), m_pUser->GetAgentThreadID(), m_pUser->GetUserIndex(), NewAddTime );
		m_AttendanceRecord.push_back( Help::ConvertYYMMDDToDate( NewAddTime.GetYear(), NewAddTime.GetMonth(), NewAddTime.GetDay() ) );
	}

	SendAttendanceRecord();
	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s:%s", __FUNCTION__, m_pUser->GetPublicID().c_str() );
}

void ioUserAttendance::OnMacroShowAttendanceWnd()
{
	if( !m_pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s User NULL!!", __FUNCTION__ ); 
		return;
	}

	if( !m_pUser->IsDeveloper() )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s:%s is Not Developer", __FUNCTION__, m_pUser->GetPublicID().c_str() ); 
		return;
	}

	SendAttendanceRecord();
	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s:%s", __FUNCTION__, m_pUser->GetPublicID().c_str() ); 
}

void ioUserAttendance::OnMacroShowAttendanceReset()
{
	if( !m_pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s User NULL!!", __FUNCTION__ ); 
		return;
	}

	if( !m_pUser->IsDeveloper() )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s:%s is Not Developer", __FUNCTION__, m_pUser->GetPublicID().c_str() );
		return;
	}

	g_DBClient.OnDeleteAttendanceRecord( m_pUser->GetUserDBAgentID(), m_pUser->GetAgentThreadID(), m_pUser->GetUserIndex() );
	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s:%s", __FUNCTION__, m_pUser->GetPublicID().c_str() );
	m_AttendanceRecord.clear();

	SP2Packet kPacket( STPK_DEVELOPER_MACRO );
	PACKET_GUARD_VOID( kPacket.Write( DEVELOPER_ATTENDANCE_RESET ) );	
	m_pUser->SendMessage( kPacket );
}

void ioUserAttendance::OnMacroAttendanceDateModify( int iYear, int iMonth, int iDay )
{
	if( !m_pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s User NULL!!", __FUNCTION__ );
		return;
	}

	if( !m_pUser->IsDeveloper() )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s:%s is Not Developer", __FUNCTION__, m_pUser->GetPublicID().c_str() ); 
		return;
	}

	if( !Help::IsAvailableDate( iYear, iMonth, iDay ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s:%s not available date : %d/%d/%d", __FUNCTION__, m_pUser->GetPublicID().c_str(), iYear, iMonth, iDay ); 
		return;
	}

	m_dwConnectTime = Help::ConvertYYMMDDToDate( 2000 + iYear, iMonth, iDay );
	g_DBClient.OnSelectAttendanceRecord( m_pUser->GetUserDBAgentID(), m_pUser->GetAgentThreadID(), m_pUser->GetUserIndex() );
	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s:%s - set date(%d,%d,%d) - %d", __FUNCTION__, m_pUser->GetPublicID().c_str(), iYear, iMonth, iDay, m_dwConnectTime );	

	SP2Packet kPacket( STPK_DEVELOPER_MACRO );
	PACKET_GUARD_VOID( kPacket.Write( DEVELOPER_ATTENDANCE_SET_DATE ) );
	m_pUser->SendMessage( kPacket );
}