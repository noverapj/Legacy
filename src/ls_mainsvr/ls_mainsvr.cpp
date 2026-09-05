// ls_main.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "ServiceLS.h"
#include "MainProcess.h"
#include "Shutdown.h"

#include <crtdbg.h>
#include "./CrashFind/BugslayerUtil.h"

CLog LOG;
CLog TradeLOG;
CLog MonitorLOG;
CLog ProcessLOG;
CLog OperatorLOG;

LONG __stdcall ExceptCallBack ( EXCEPTION_POINTERS * pExPtrs );

int _tmain(int argc, _TCHAR* argv[])
{
	ServiceLS* pService = new ServiceLS( argc, argv );
	pService->ServiceMainProc();
	delete pService;

	/*LOG.CloseAndRelease();
	TradeLOG.CloseAndRelease();
	MonitorLOG.CloseAndRelease();
	ProcessLOG.CloseAndRelease();
	timeEndPeriod(1);*/

	return 0;
}

LONG __stdcall ExceptCallBack ( EXCEPTION_POINTERS * pExPtrs )
{
	static bool bHappenCrash = false;

	if(bHappenCrash)
		return EXCEPTION_EXECUTE_HANDLER;

	char szLog[2048]="";
	strcpy_s(szLog, sizeof(szLog), g_MainProc.GetPublicIP().c_str());

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
	
	const char * szBuff = GetFirstStackTraceString( GSTSO_SYMBOL | GSTSO_SRCLINE,pExPtrs  );
	do
	{
		LOG.PrintLog(0,"%s" , szBuff );	
		if(strlen(szLog)+strlen(szBuff) < 1500)
		{
			strcat_s(szLog, "\n");
			strcat_s(szLog, szBuff);
		}
		szBuff = GetNextStackTraceString( GSTSO_SYMBOL | GSTSO_SRCLINE , pExPtrs );
	}
	while ( NULL != szBuff );
	
	LOG.PrintLog(0, "---- Crash Help End ----");

	bHappenCrash = true;

	g_MainProc.Shutdown( SHUTDOWN_CRASH );
	return EXCEPTION_EXECUTE_HANDLER;
}
