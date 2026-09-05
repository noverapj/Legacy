// BillingRelayServer.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "MainProcess.h"
#include <crtdbg.h>
#include "../Service/Service.h"
#include "../Service/ServiceLS.h"
#include "CrashFind/BugslayerUtil.h"
#include "Minidump/MiniDump.h"
#include <conio.h>
extern CLog LOG;

LONG __stdcall ExceptCallBack ( EXCEPTION_POINTERS * pExPtrs );

LONG __stdcall ExceptCallBack ( EXCEPTION_POINTERS * pExPtrs )
{
	static bool bHappenCrash = false;
 
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


BOOL ConsoleHandler(DWORD fdwCtrlType) 
{ 
	LOG.PrintTimeAndLog(0,"[TID]I'm ConsoleHandler:%d\n",GetCurrentThreadId());
	switch (fdwCtrlType) 
	{ 
		// Handle the CTRL+C signal. 
	case CTRL_C_EVENT: 
	case CTRL_CLOSE_EVENT: // CTRL+CLOSE: confirm! that the user wants to exit. 
	case CTRL_BREAK_EVENT: 
	case CTRL_LOGOFF_EVENT: 
	case CTRL_SHUTDOWN_EVENT:						
	default: 
		g_App.SetWantExit(); 
		return FALSE;
	} 
	return TRUE;
}	



CLog LOG;
CLog ProcessLOG;
int _tmain(int argc, _TCHAR* argv[])
{
	if( !SetConsoleCtrlHandler( (PHANDLER_ROUTINE)ConsoleHandler, TRUE) ) 
		return FALSE; 

	ServiceLS *service = new ServiceLS( argc, argv );
	service->ServiceMainProc();

	delete service;
	 
	return 0;
}
