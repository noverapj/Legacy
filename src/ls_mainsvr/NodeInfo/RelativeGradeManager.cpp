#include "../stdafx.h"

#include "../ioProcessChecker.h"
#include "../Network/GameServer.h"
#include "../QueryData/QueryResultData.h"
#include "../DataBase/DBClient.h"

#include "../EtcHelpFunc.h"
#include "ServerNodeManager.h"

#include "RelativeGradeManager.h"


RelativeGradeManager *RelativeGradeManager::sg_Instance = NULL;

RelativeGradeManager::RelativeGradeManager()
{
	m_dwUniqueCode = 0;
	m_cTestLastUpdateTime = 0;
	m_bEnableUpdate = true;
}

RelativeGradeManager::~RelativeGradeManager()
{
}

RelativeGradeManager &RelativeGradeManager::GetInstance()
{
	if( !sg_Instance )
		sg_Instance = new RelativeGradeManager;

	return *sg_Instance;
}

void RelativeGradeManager::ReleaseInstance()
{
	SAFEDELETE( sg_Instance );
}

void RelativeGradeManager::LoadINIData()
{
	ioINILoader kLoader( "config/sp2_relative_grade.ini" );

	kLoader.SetTitle( "common" );
	m_ReduceRate =  kLoader.LoadInt( "reduce_rate", 0 );

	kLoader.SetTitle( "update" );
	m_UpdateWeek = kLoader.LoadInt( "week", MONDAY );
	m_UpdateHour = kLoader.LoadInt( "hour", 4 );
	m_UpdateMinute = kLoader.LoadInt( "minute", 0 );

	kLoader.SetTitle( "update_test" );
	m_bTestProcess = kLoader.LoadBool( "test_process", false );
	m_iTestUpdateMin = kLoader.LoadInt( "test_process_min", 60 );

	LOG.PrintTimeAndLog( 0, "Relative Grade Load INI : w:%d - h:%d - m:%d", m_UpdateWeek, m_UpdateHour, m_UpdateMinute );
}

void RelativeGradeManager::Process()
{
	if( m_bTestProcess )
	{
		if( !EnableTestProcess() )
			return;
	}
	else
	{
		if( !EnableProcess() )
			return;
	}

#ifndef SRC_ID
	g_DBClient.OnUpdateRelativeGrade( m_dwUniqueCode, m_ReduceRate );
	UpdateRealtiveGradeToAllServer();
#endif
	LOG.PrintTimeAndLog( 0, "RelativeGradeManager::Process() : %d", m_dwUniqueCode );
}

bool RelativeGradeManager::EnableProcess()
{
	CTime curTime = CTime::GetCurrentTime();
	int year  = curTime.GetYear();
	int month = curTime.GetMonth();
	int day   = curTime.GetDay();
	int hour  = curTime.GetHour();
	int minute= curTime.GetMinute();
	int week  = curTime.GetDayOfWeek();
	if( m_UpdateWeek != week || m_UpdateHour != hour || m_UpdateMinute != minute )
	{
		m_bEnableUpdate = true;
		return false;
	}

	if( !m_bEnableUpdate )
		return false;

	m_dwUniqueCode = 0;
	m_dwUniqueCode += (year%100) * 100000000;
	m_dwUniqueCode += month * 1000000;
	m_dwUniqueCode += day * 10000;
	m_dwUniqueCode += hour * 100;
	m_dwUniqueCode += minute;

	m_bEnableUpdate = false;

	return true;
}

bool RelativeGradeManager::EnableTestProcess()
{
	CTime curTime = CTime::GetCurrentTime();
	
	if( m_cTestLastUpdateTime == 0 )
	{
		m_cTestLastUpdateTime = CTime::GetCurrentTime();
		return false;
	}

	CTimeSpan cGapTime = curTime - m_cTestLastUpdateTime;
	if( cGapTime.GetTotalMinutes() >= m_iTestUpdateMin )
	{
		int year  = curTime.GetYear();
		int month = curTime.GetMonth();
		int day   = curTime.GetDay();
		int hour  = curTime.GetHour();
		int minute= curTime.GetMinute();

		m_dwUniqueCode = 0;
		m_dwUniqueCode += (year%100) * 100000000;
		m_dwUniqueCode += month * 1000000;
		m_dwUniqueCode += day * 10000;
		m_dwUniqueCode += hour * 100;
		m_dwUniqueCode += minute;

		m_cTestLastUpdateTime = CTime::GetCurrentTime();
		return true;
	}

	return false;
}

void RelativeGradeManager::UpdateRealtiveGradeToAllServer()
{
	SP2Packet kPacket( MSTPK_UPDATE_RELATIVE_GRADE );
	kPacket << m_dwUniqueCode;
	g_ServerNodeManager.SendMessageAllNode( kPacket );
}