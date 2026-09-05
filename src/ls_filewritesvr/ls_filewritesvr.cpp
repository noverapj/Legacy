// ls_filewritesvr.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "service\ServiceLS.h"
#include "ioMainProcess.h"
#include "CrashFind\BugslayerUtil.h"
#include "Shutdown.h"

CLog LOG;
CLog NetLOG;
CLog CriticalLOG;

LONG __stdcall ExceptCallBack ( EXCEPTION_POINTERS * pExPtrs );

int _tmain(int argc, _TCHAR* argv[])
{
	ServiceLS *service = new ServiceLS( argc, argv );
	service->ServiceMainProc();
	delete service;

	return 0;
}

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
	CriticalLOG.PrintLog(0, "---- Crash Help Data ----");
	wsprintf(szTemp, "%s", GetFaultReason(pExPtrs));
	CriticalLOG.PrintLog(0, "%s", szTemp);
	strcat_s(szLog, "\n");
	strcat_s(szLog, szTemp);
	memset(szTemp, 0, sizeof(szTemp));

	wsprintf(szTemp, "%s", GetRegisterString(pExPtrs));
	CriticalLOG.PrintLog(0, "%s", szTemp);
	strcat_s(szLog, "\n");
	strcat_s(szLog, szTemp);
	
	const char * szBuff = GetFirstStackTraceString( GSTSO_SYMBOL | GSTSO_SRCLINE,pExPtrs  );
	do
	{
		CriticalLOG.PrintLog(0,"%s" , szBuff );	
		if(strlen(szLog)+strlen(szBuff) < 1500)
		{
			strcat_s(szLog, "\n");
			strcat_s(szLog, szBuff);
		}
		szBuff = GetNextStackTraceString( GSTSO_SYMBOL | GSTSO_SRCLINE , pExPtrs );
	}
	while ( NULL != szBuff );
	
	CriticalLOG.PrintLog(0, "---- Crash Help End ----");

	bHappenCrash = true;

	g_App.Shutdown(SHUTDOWN_CRASH);	
	return EXCEPTION_EXECUTE_HANDLER;
}
