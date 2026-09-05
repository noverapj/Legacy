#include "stdafx.h"
#include "Log.h"
#include "ioLogTestThread.h"

CLog LOG;
CLog HackLOG;
CLog ProcessLOG;

const int TEST_COUNT = 20000000;



ioLogTestThread::ioLogTestThread(void)
{
}

ioLogTestThread::~ioLogTestThread(void)
{
}

void ioLogTestThread::Run()
{
	printf("[%lu]Thread Begin\n", GetCurrentThreadId() );

	for(int i = 0 ; i <= TEST_COUNT ; i++)
	{
		int choice = (rand() % 10);
		switch(choice)
		{
		case 0 :
			TEST_Open();
			break;
		case 1 :
			TEST_Close();
			break;
		default :
			TEST_Write();
			break;
		}
	}

	printf("[%lu]-> Thread End\n", GetCurrentThreadId() );
}

void ioLogTestThread::TEST_Open()
{
	//printf("[%lu]TEST_Open\n", GetCurrentThreadId() );

	SYSTEMTIME st;
	GetLocalTime(&st);
	char szCurTime[MAX_PATH] = "";
	sprintf_s(szCurTime, sizeof(szCurTime), "%04d%02d%02d", st.wYear, st.wMonth, st.wDay );

	char TimeLogName[MAX_PATH]="";
	memset(TimeLogName, 0, sizeof(TimeLogName));
	sprintf_s(TimeLogName, sizeof(TimeLogName), "%d\\BUG%s-%s-%d.log", rand() % 5, szCurTime, "127.0.0.1", 111111 );

	LOG.CloseLog();
	LOG.OpenLog( 0, TimeLogName, true);	
	LOG.PrintTimeAndLog(0, "<<< --------------------  Create File -------------------- >>>\r\n");
	LOG.PrintTimeAndLog( 0, "Version : 버젼 | 시간" );

	memset(TimeLogName, 0, sizeof(TimeLogName));
	sprintf_s(TimeLogName, sizeof(TimeLogName), "%d\\HACK%s-%s-%d.log", rand() % 5, szCurTime, "127.0.0.1", 111111);
	HackLOG.CloseLog();
	HackLOG.OpenLog( 0, TimeLogName, true);
	HackLOG.PrintTimeAndLog(0, "<<< --------------------  Create File -------------------- >>>\r\n");
	HackLOG.PrintTimeAndLog( 0, "Version : 버젼 | 시간" );

	memset(TimeLogName, 0, sizeof(TimeLogName));
	sprintf_s(TimeLogName, sizeof(TimeLogName), "%d\\PROCESS%s-%s-%d.log", rand() % 5, szCurTime, "127.0.0.1", 111111);
	ProcessLOG.CloseLog();
	ProcessLOG.OpenLog( 0, TimeLogName, true);
	ProcessLOG.PrintTimeAndLog(0, "<<< --------------------  Create File -------------------- >>>\r\n");
	ProcessLOG.PrintTimeAndLog( 0, "Version : 버젼 | 시간" );
	ProcessLOG.PrintTimeAndLog( 0, "[Main][LogDB][UDP][ClientA][ServerA][Work]" );
}

void ioLogTestThread::TEST_Close()
{
	//printf("[%lu]TEST_Close\n", GetCurrentThreadId() );

	LOG.CloseLog();
	LOG.PrintTimeAndLog(0, "<<< --------------------  End File -------------------- >>>\r\n");
	HackLOG.CloseLog();
	HackLOG.PrintTimeAndLog(0, "<<< --------------------  End File -------------------- >>>\r\n");
	ProcessLOG.CloseLog();
	ProcessLOG.PrintTimeAndLog(0, "<<< --------------------  End File -------------------- >>>\r\n");
}

void ioLogTestThread::TEST_Write()
{
	//printf("[%lu]TEST_Write\n", GetCurrentThreadId() );

	LOG.PrintTimeAndLog( 0, "%s pUser == NULL.", __FUNCTION__ );
	HackLOG.PrintTimeAndLog( 0, "%s PenaltyPeso Hack :->10000", __FUNCTION__ );
	ProcessLOG.PrintTimeAndLog( 0, "MSTPK_SERVER_UPDATE Packet Delay : %dms", 100 );
}

