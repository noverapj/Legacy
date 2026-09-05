#include "StdAfx.h"
#include "LSLogic.h"
#include <boost/date_time/posix_time/posix_time_duration.hpp>

LSLogic::LSLogic(void)
{
	InitData();
}

LSLogic::~LSLogic(void)
{
}

void LSLogic::InitData()
{
	m_logCheckTime = 0;
	m_logic = NULL;

	TCHAR temp[MAX_PATH];
	GetModuleFileName(NULL, temp, MAX_PATH);

	TCHAR* token = _tcsrchr(temp, _T('\\'));
	*(token+1) = _T('\0');
	SetCurrentDirectory(temp);
	std::string strtemp = temp;
	strtemp = strtemp + g_Config()->GetLogFolder();
	boost::filesystem::create_directory(strtemp);
}
 
void LSLogic::Init()
{

	m_logic = new LogicThread;
	m_logic->SetProcessor(this);
	m_logic->Begin();
	StartShcedule(); //kyg 주석 풀것

}
void LSLogic::Destory()
{
	m_logic->Destroy();

}
void LSLogic::InitScheduler()
{
	m_scheduler = new SchedulerNode;
	m_scheduler->OnCreate();
	m_scheduler->AddSchedule(ITPK_SENDBUFFER_FLUSH_PROCESS, g_Config()->NagleTime() );
}

void LSLogic::StartShcedule()
{
	InitScheduler();

	//여기서 오퍼레이션 실행 
	{
		SchedulerOperation_ *data = (SchedulerOperation_*)g_OPMemPool()->Pop(sizeof(SchedulerOperation_));
		data->SetOperation(EOPID::SCHEDULER, ESCHEDULE::SCHEDULEOP, m_scheduler);
		g_TimerMgr()->AddTimer(true, 1, data);
	}
	{
		SchedulerOperation_ *data = (SchedulerOperation_*)g_OPMemPool()->Pop(sizeof(SchedulerOperation_));
		data->SetOperation(EOPID::SCHEDULER, ESCHEDULE::GHOSTCLIENT, m_scheduler);
		g_TimerMgr()->AddTimer(true, g_Config()->ScanClientGhostTime(), data);
	}
	{
		SchedulerOperation_ *data = (SchedulerOperation_*)g_OPMemPool()->Pop(sizeof(SchedulerOperation_));
		data->SetOperation(EOPID::SCHEDULER, ESCHEDULE::TIMEOUTCLIENT, m_scheduler);
		g_TimerMgr()->AddTimer(true, g_Config()->CheckUserTimeout(), data);
	}
	//여기서 서버 정보 요청하는 패킷 타이머도 만듬 
	{
		SchedulerOperation_ *data = (SchedulerOperation_*)g_OPMemPool()->Pop(sizeof(SchedulerOperation_));
		data->SetOperation(EOPID::SCHEDULER, ESCHEDULE::REQUESTSERVERINFO, m_scheduler);
		g_TimerMgr()->AddTimer(true, g_Config()->RequestServerInfoTime(), data);
	}
	{
		SchedulerOperation_ *data = (SchedulerOperation_*)g_OPMemPool()->Pop(sizeof(SchedulerOperation_));
		data->SetOperation(EOPID::SCHEDULER, ESCHEDULE::PING, m_scheduler);
		g_TimerMgr()->AddTimer(true, g_Config()->CheckPingTime(), data);
	}

	{
		if(g_Config()->GetPartition() != 0)
		{
			SchedulerOperation_ *data = (SchedulerOperation_*)g_OPMemPool()->Pop(sizeof(SchedulerOperation_));
			data->SetOperation(EOPID::SCHEDULER, ESCHEDULE::SERVERPARTITION, m_scheduler);
			g_TimerMgr()->AddTimer(true, g_Config()->GetPartitionTimer(), data);
		}
	}
}

void LSLogic::Process(uint32& idleTime)
{
	__try
	{

		int iPacketParsingSize = g_Queue()->PacketParsing();
	 
		if(iPacketParsingSize == 0)
			idleTime = 1;
		else			
			idleTime = 0;		

		SetNewdata();

	}
	__except(UnHandledExceptionFilter(GetExceptionInformation()))
	{

	}
}



void LSLogic::SetNewdata(DWORD dwProcessTime)
{
	TCHAR temp[MAX_PATH];
	GetModuleFileName(NULL, temp, MAX_PATH);

	TCHAR* token = _tcsrchr(temp, _T('\\'));
	*(token+1) = _T('\0');
	SetCurrentDirectory(temp);

	if( dwProcessTime > 0 && dwProcessTime - m_logCheckTime < 600000 )
		return;

	m_logCheckTime = dwProcessTime;
	TCHAR szCurTime[1024] = _T("");
	static TCHAR szPrevTime[1024] = _T("");
	SYSTEMTIME st;
	GetLocalTime(&st);
	wsprintf(szCurTime,_T("%02d%02d"),st.wMonth,st.wDay);

	if(strcmp(szCurTime,szPrevTime) != 0)
	{
		strcpy_s(szPrevTime,szCurTime);
		TCHAR LogName[1024] = _T("");
		TCHAR ReportName[1024] = "";
		wsprintf(LogName,_T("%s%s%s%s%s"),temp, g_Config()->GetLogFolder(), _T("\\LOG"),szCurTime,_T(".log"));
		wsprintf(ReportName,_T("%s%s%s%s%s"),temp, g_Config()->GetLogFolder(), _T("\\REPORT"),szCurTime,_T(".log"));
		{
			LOG.PrintTimeAndLog(0, "<<< --------------------  End File -------------------- >>>");
			LOG.CloseLog();
			LOG.OpenLog(0,LogName,true);	
			LOG.PrintTimeAndLog(0, "<<< --------------------  Create File -------------------- >>>");
		}	
		{
			ReportLOG.PrintTimeAndLog(0, "<<< --------------------  End File -------------------- >>>");
			ReportLOG.CloseLog();
			ReportLOG.OpenLog(0,ReportName,true);
			ReportLOG.PrintTimeAndLog(0, "<<< --------------------  Create File -------------------- >>>");
		}		
	}	

}

void LSLogic::PrintTimeAndLog(int debuglv, LPSTR fmt )
{
	LOG.PrintTimeAndLog( debuglv, fmt );
}

void LSLogic::DebugLog(int debuglv, LPSTR filename, int linenum, LPSTR fmt )
{
	LOG.DebugLog( debuglv, filename, linenum, fmt );
}

void LSLogic::ShutDown(BOOL state)
{
	LOG.PrintTimeAndLog(0, "<<< --------------------  End File -------------------- >>>");
	LOG.CloseLog();
	ReportLOG.PrintTimeAndLog(0, "<<< --------------------  End File -------------------- >>>");
	ReportLOG.CloseLog();
	Sleep(30);
}
