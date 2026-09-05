// stdafx.cpp : 표준 포함 파일을 포함하는 소스 파일입니다.
// LSLogClient.pch는 미리 컴파일된 헤더가 됩니다.
// stdafx.obj는 미리 컴파일된 형식 정보를 포함합니다.

#include "stdafx.h"
#include <crtdbg.h>
#include "./CrashFind/BugslayerUtil.h"
#include "LSLogClientDlg.h"

LONG __stdcall ExceptCallBack ( EXCEPTION_POINTERS * pExPtrs )
{
	static bool bHappenCrash = false;

	if(bHappenCrash)
		return EXCEPTION_EXECUTE_HANDLER;

	char szLog[2048]="";

	char szTemp[MAX_PATH]="";
	LOG.PrintLog(0, "---- Crash Help Data ----");
	wsprintf(szTemp, "%s", GetFaultReason(pExPtrs));
	LOG.PrintLog(0, "%s", szTemp);
	strcat(szLog, "\n");
	strcat(szLog, szTemp);
	memset(szTemp, 0, sizeof(szTemp));

	wsprintf(szTemp, "%s", GetRegisterString(pExPtrs));
	LOG.PrintLog(0, "%s", szTemp);
	strcat(szLog, "\n");
	strcat(szLog, szTemp);

	const char * szBuff = GetFirstStackTraceString( GSTSO_SYMBOL | GSTSO_SRCLINE,pExPtrs  );
	do
	{
		LOG.PrintLog(0,"%s" , szBuff );	
		if(strlen(szLog) < 2000)
		{
			strcat(szLog, "\n");
			strcat(szLog, szBuff);
		}
		szBuff = GetNextStackTraceString( GSTSO_SYMBOL | GSTSO_SRCLINE , pExPtrs );
	}
	while ( NULL != szBuff );

	LOG.PrintLog(0, "---- Crash Help End ----");

	bHappenCrash = true;

	return EXCEPTION_EXECUTE_HANDLER;
}

