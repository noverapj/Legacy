#include "stdafx.h"
#include "cMiniDump.h"
#include <TCHAR.h>
#include <DBGHELP.h>

TCHAR cMiniDump::m_filename[MAX_PATH];
TCHAR cMiniDump::m_folername[MAX_PATH];

typedef BOOL (WINAPI *MINIDUMPWRITEDUMP)(
    HANDLE hProcess, 
    DWORD dwPid, 
    HANDLE hFile, 
    MINIDUMP_TYPE DumpType,
    CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
    CONST PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
    CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam);


LPTOP_LEVEL_EXCEPTION_FILTER PreviousExceptionFilter = NULL;


LONG WINAPI UnHandledExceptionFilter(struct _EXCEPTION_POINTERS *exceptionInfo)
{
    HMODULE DllHandle = NULL;

    // Windows 2000 이전에는 따로 DBGHELP를 배포해서 설정해 주어야 한다.
    DllHandle = LoadLibrary(_T("DBGHELP.DLL"));
    if (DllHandle)
    {
        MINIDUMPWRITEDUMP MiniDumpWriteDump = reinterpret_cast<MINIDUMPWRITEDUMP>(GetProcAddress(DllHandle, "MiniDumpWriteDump"));
        if (MiniDumpWriteDump)
        {
            TCHAR DumpPath[MAX_PATH] = {0};

			SYSTEMTIME SystemTime;
            GetLocalTime(&SystemTime);

            _stprintf_s(
				DumpPath, 
				MAX_PATH-1,
				_T("%s\\%s(%04d%02d%02d-%02d%02d%02d).dmp"), 
				cMiniDump::m_folername,
				cMiniDump::m_filename,
                SystemTime.wYear,
                SystemTime.wMonth,
                SystemTime.wDay,
                SystemTime.wHour,
                SystemTime.wMinute,
                SystemTime.wSecond);
            
            HANDLE hDumpFile = CreateFile(
				DumpPath, 
                GENERIC_WRITE, 
                FILE_SHARE_WRITE, 
                NULL, 
				CREATE_ALWAYS, 
                FILE_ATTRIBUTE_NORMAL, 
                NULL);

            if (hDumpFile != INVALID_HANDLE_VALUE)
            {
                _MINIDUMP_EXCEPTION_INFORMATION ExceptionInfo;
                
                ExceptionInfo.ThreadId			= GetCurrentThreadId();
                ExceptionInfo.ExceptionPointers	= exceptionInfo;
                ExceptionInfo.ClientPointers	= NULL;

                BOOL bSuccess = MiniDumpWriteDump(
                    GetCurrentProcess(), 
                    GetCurrentProcessId(), 
                    hDumpFile, 
                    MiniDumpNormal, 
                    &ExceptionInfo, 
                    NULL, 
                    NULL);

                if (bSuccess)
                {
                    CloseHandle(hDumpFile);
                    return EXCEPTION_EXECUTE_HANDLER;
                }

				CloseHandle(hDumpFile);
            }
        }
    }
    return EXCEPTION_CONTINUE_SEARCH;
}

BOOL cMiniDump::Begin(const TCHAR* filename)
{
    SetErrorMode(SEM_FAILCRITICALERRORS);

	TCHAR temp[512];
	GetCurrentDirectory(_countof(temp), temp);
    _sntprintf_s(m_folername, sizeof(m_folername), _T("%s\\dump"), temp); 
	CreateDirectory(m_folername, NULL);

	_tcscpy_s(m_filename, 260, filename);

    PreviousExceptionFilter = SetUnhandledExceptionFilter(UnHandledExceptionFilter);
    return TRUE;
}

BOOL cMiniDump::End(VOID)
{
    SetUnhandledExceptionFilter(PreviousExceptionFilter);
    return TRUE;
}

