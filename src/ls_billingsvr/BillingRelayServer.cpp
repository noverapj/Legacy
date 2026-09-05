// BillingRelayServer.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "Window.h"
#include <crtdbg.h>
#include "CrashFind/BugslayerUtil.h"
#include "Minidump/MiniDump.h"

extern CLog LOG;

LONG __stdcall ExceptCallBack ( EXCEPTION_POINTERS * pExPtrs );

LONG __stdcall ExceptCallBack ( EXCEPTION_POINTERS * pExPtrs )
{
	static bool bHappenCrash = false;

	// 여기서 남겨야 함.
	//UnHandledExceptionFilter( pExPtrs );

	if(bHappenCrash)
		return EXCEPTION_EXECUTE_HANDLER;

	char szLog[2048]="";
	strcpy_s(szLog, g_App.GetPublicIP().c_str());

	char szTemp[2048]="";
	LOG.PrintLog(0, "---- Crash Help Data ----");
	wsprintf(szTemp, "%s", GetFaultReason(pExPtrs));
	LOG.PrintLog(0, "%s", szTemp);
	strcat_s(szLog, "\n");
	strcat_s(szLog, szTemp);
	memset(szTemp, 0, sizeof(szTemp));

	wsprintf(szTemp, "%s", GetRegisterString(pExPtrs));
	LOG.PrintLog(0, "%s", szTemp);
	strcat_s(szLog, "\n");
	strcat_s(szLog, szTemp);

	const char * szBuff = MiniDump::GetFirstStackTraceString( GSTSO_SYMBOL | GSTSO_SRCLINE,pExPtrs  );
	do
	{
		LOG.PrintLog(0,"%s" , szBuff );	
		if(strlen(szLog)+strlen(szBuff) < 1500)
		{
			strcat_s(szLog, "\n");
			strcat_s(szLog, szBuff);
		}
		szBuff = MiniDump::GetNextStackTraceString( GSTSO_SYMBOL | GSTSO_SRCLINE , pExPtrs );
	}
	while ( NULL != szBuff );

	 
	LOG.PrintLog(0, "---- Crash Help End ----");

	bHappenCrash = true;
	
	//g_App.CrashDown();	
	return EXCEPTION_EXECUTE_HANDLER;
}


CLog LOG;
CLog ProcessLOG;
int APIENTRY WinMain(HINSTANCE hInstance,
					 HINSTANCE hPrevInstance,
					 LPSTR     lpCmdLine,
					 int       nCmdShow)
{
	
	MiniDump::Begin("billingrelaysvr");
	timeBeginPeriod(1);

	g_App.CheckCreateNewLog( true );

	//NetLOG.PrintTimeAndLog( 0, "{DB PACKET}:<SESSION PACKET>" );

	if(!g_App.Initialize(hInstance))
	{
		CWindow::ReleaseInstance();
//		LOG.CloseAndRelease(); //kyg 뭔지 확인 
		return 0;
	}

// 	if(TRUE != SetCrashHandlerFilter(ExceptCallBack))
// 	{
// 		LOG.PrintTimeAndLog(0,"SetCrashHandler Failed");
// 		CWindow::ReleaseInstance();
// //		LOG.CloseAndRelease();
// 		return 0;
// 	}

	g_App.Run();

	CWindow::ReleaseInstance();	
//	LOG.CloseAndRelease();
	//NetLOG.CloseAndRelease();
	//ProcessLOG.CloseAndRelease();
	timeEndPeriod(1);
	MiniDump::End();
	return 0;
}
