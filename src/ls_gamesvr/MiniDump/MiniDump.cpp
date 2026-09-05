
#include "stdafx.h"
#include <malloc.h>
#include "File.h"
#include "MiniDump.h"

extern CLog CriticalLOG;


TCHAR MiniDump::m_filename[MAX_PATH];
TCHAR MiniDump::m_folername[MAX_PATH];

CONTEXT MiniDump::m_stackContext;
IMAGEHLP_LINE64 MiniDump::m_stackLine;
STACKFRAME64 MiniDump::m_stackFrame;
BYTE MiniDump::m_stackSymbol[SYM_BUFF_SIZE];
TCHAR MiniDump::m_dumpText[BUFF_SIZE];
BOOL MiniDump::m_symEngInit = FALSE;


typedef BOOL (WINAPI *MINIDUMPWRITEDUMP)(
    HANDLE hProcess, 
    DWORD dwPid, 
    HANDLE hFile, 
    MINIDUMP_TYPE DumpType,
    CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
    CONST PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
    CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam);


LPTOP_LEVEL_EXCEPTION_FILTER PreviousExceptionFilter = NULL;
_invalid_parameter_handler PreviousInvalidParameterHandler = NULL;
_purecall_handler PreviousHandlerPureCallFilter =  NULL;

void InvalidParameterHandler(const wchar_t* expression, const wchar_t* function, const wchar_t* file, unsigned int line, uintptr_t pReserved)
{
	Information( "InvalidParameterHandler\n" );
	CriticalLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "MINIDUMP - InvalidParameterHandler" );
}

void PurecallHandler(void)
{
	Information( "PurecallHandler\n" );
	CriticalLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "MINIDUMP - PurecallHandler" );
}

LONG WINAPI UnHandledExceptionFilter(struct _EXCEPTION_POINTERS *exceptionInfo)
{
	// 콜스택저장
	MiniDump::PrintCallStack(exceptionInfo);

    // Windows 2000 이전에는 따로 DBGHELP를 배포해서 설정해 주어야 한다.
    HMODULE DllHandle = LoadLibrary(_T("DBGHELP.DLL"));
    if (DllHandle)
    {
        MINIDUMPWRITEDUMP MiniDumpWriteDump = reinterpret_cast<MINIDUMPWRITEDUMP>(GetProcAddress(DllHandle, "MiniDumpWriteDump"));
        if (MiniDumpWriteDump)
        {
            TCHAR dumpPath[MAX_PATH] = {0};

			SYSTEMTIME SystemTime;
            GetLocalTime(&SystemTime);

            _sntprintf_s(
				dumpPath, 
				_countof(dumpPath),
				MAX_PATH, 
				_T("%s\\%s(%04d%02d%02d-%02d%02d%02d).dmp"), 
				MiniDump::m_folername,
				MiniDump::m_filename,
                SystemTime.wYear,
                SystemTime.wMonth,
                SystemTime.wDay,
                SystemTime.wHour,
                SystemTime.wMinute,
                SystemTime.wSecond);
            
            HANDLE hDumpFile = CreateFile(
				dumpPath, 
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
                    (MINIDUMP_TYPE)(MiniDumpWithFullMemory | 
									MiniDumpWithThreadInfo |
									MiniDumpWithHandleData |
									MiniDumpWithProcessThreadData |
									MiniDumpWithFullMemoryInfo |
									MiniDumpWithUnloadedModules |
									MiniDumpIgnoreInaccessibleMemory |
									MiniDumpWithTokenInformation ),
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

BOOL MiniDump::Begin(const TCHAR* filename)
{
    SetErrorMode(SEM_FAILCRITICALERRORS);

	TCHAR temp[512];
	GetCurrentDirectory(_countof(temp), temp);
    _sntprintf_s(m_folername, _countof(m_folername),_T("%s\\dump"), temp); 
	CreateDirectory(m_folername, NULL);

	_tcscpy_s(m_filename, _countof(m_filename), filename);	// 디버깅 해볼것

	_CrtSetReportMode(_CRT_ASSERT, 0);
	//_set_new_handler();
	//_set_abort_behavior();
	PreviousInvalidParameterHandler = _set_invalid_parameter_handler(InvalidParameterHandler);
	PreviousHandlerPureCallFilter	= _set_purecall_handler(PurecallHandler);
    PreviousExceptionFilter			= SetUnhandledExceptionFilter(UnHandledExceptionFilter);
    return TRUE;
}

BOOL MiniDump::End(VOID)
{
    SetUnhandledExceptionFilter(PreviousExceptionFilter);
	_set_invalid_parameter_handler(PreviousInvalidParameterHandler);
	_set_purecall_handler(PreviousHandlerPureCallFilter);  
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////

// Initializes the symbol engine if needed
void MiniDump::InitSymEng(void)
{
    if(FALSE == m_symEngInit)
    {
        // Set up the symbol engine.
        DWORD options = SymGetOptions();

        // Turn on line loading.
        SymSetOptions(options | SYMOPT_LOAD_LINES);

        // Force the invade process flag on.
        BOOL bRet = SymInitialize(GetCurrentProcess(), NULL , TRUE);
        m_symEngInit = bRet;
    }
}

// Helper function to isolate filling out the stack frame, which is CPU specific.
void MiniDump::FillInStackFrame(PCONTEXT contextRecord)
{
    // Initialize the STACKFRAME structure.
    ZeroMemory(&m_stackFrame, sizeof (STACKFRAME64));

    m_stackFrame.AddrPC.Offset       = contextRecord->Eip;
    m_stackFrame.AddrPC.Mode         = AddrModeFlat;
    m_stackFrame.AddrStack.Offset    = contextRecord->Esp;
    m_stackFrame.AddrStack.Mode      = AddrModeFlat;
    m_stackFrame.AddrFrame.Offset    = contextRecord->Ebp;
    m_stackFrame.AddrFrame.Mode      = AddrModeFlat;
}

//////////////////////////////////////////////////////////////////////
// Operations
//////////////////////////////////////////////////////////////////////

BOOL __stdcall CH_ReadProcessMemory(	HANDLE	hHandle,
										DWORD64	qwBaseAddress,
										PVOID	lpBuffer,
										DWORD	nSize,
										LPDWORD	lpNumberOfBytesRead)
{
    return (ReadProcessMemory(	GetCurrentProcess(),
                                (LPCVOID)qwBaseAddress,
                                lpBuffer,
                                nSize,
                                lpNumberOfBytesRead));
}

DWORD Ansi2Wide(const char * szANSI, wchar_t* szWide, int iWideLen)
{
   int iRet = MultiByteToWideChar(	GetACP(),
                                    0,
									szANSI,
                                    -1,
                                    szWide,
                                    iWideLen);
    return (iRet);
}


DWORD Wide2Ansi(const wchar_t* szWide, char * szANSI, int iAnsiLen)
{
	ZeroMemory(szANSI, iAnsiLen);
	int iRet = WideCharToMultiByte(CP_ACP, 0, szWide, -1, szANSI, iAnsiLen, NULL, NULL);
	return iRet;
}

// The internal function that does all the stack walking
LPCTSTR MiniDump::InternalGetStackTraceString(DWORD options)
{
	// The value that is returned
    LPCTSTR szRet;
    // The module base address. I look this up right after the stack
    // walk to ensure that the module is valid.
    DWORD64 modBase;

    __try
    {
        // Initialize the symbol engine in case it isn't initialized.
        MiniDump::InitSymEng();

        // Note:  If the source file and line number functions are used,
        //        StackWalk can cause an access violation.
        BOOL ret = StackWalk64 ( IMAGE_FILE_MACHINE_I386,
                                    GetCurrentProcess(),
                                    GetCurrentThread(),
                                    &m_stackFrame,
                                    &m_stackContext,
                                    CH_ReadProcessMemory,
                                    SymFunctionTableAccess64,
                                    SymGetModuleBase64,
                                    NULL);
        if((FALSE == ret) || (0 == m_stackFrame.AddrFrame.Offset))
        {
            szRet = NULL;
            __leave;
        }

        // Before I get too carried away and start calculating
        // everything, I need to double-check that the address returned
        // by StackWalk really exists. I've seen cases in which
        // StackWalk returns TRUE but the address doesn't belong to
        // a module in the process.
        modBase = SymGetModuleBase64(GetCurrentProcess(), m_stackFrame.AddrPC.Offset);
        if(0 == modBase)
        {
            szRet = NULL;
            __leave;
        }

        int iCurr = 0;

        // At a minimum, put in the address.
#ifdef _WIN64
        iCurr += wsprintf(	m_dumpText + iCurr,
                            _T( "0x%016X" ),
                            m_stackFrame.AddrPC.Offset);
#else
        iCurr += wsprintf ( m_dumpText + iCurr,
                            _T("%04X:%08X"),
                            m_stackContext.SegCs,
                            m_stackFrame.AddrPC.Offset);
#endif

        // Output the parameters?
        if(DUMP_PARAMS == (options & DUMP_PARAMS))
        {
            iCurr += wsprintf(	m_dumpText + iCurr, 
                                _T("0x%08X 0x%08X 0x%08X 0x%08X"),
                                m_stackFrame.Params[ 0 ],
                                m_stackFrame.Params[ 1 ],
                                m_stackFrame.Params[ 2 ],
                                m_stackFrame.Params[ 3 ]);
        }

        // Output the module name.
        if(DUMP_MODULE == (options & DUMP_MODULE))
        {
            iCurr += wsprintf(m_dumpText + iCurr, _T (" "));

            iCurr += GetModuleBaseName(	GetCurrentProcess(),
                                        (HINSTANCE)modBase,
                                        m_dumpText + iCurr,
										BUFF_SIZE - iCurr);
        }

        // Output the symbol name?
        if(DUMP_SYMBOL == (options & DUMP_SYMBOL))
        {
			DWORD64 displacement;

            // Start looking up the exception address.
            PIMAGEHLP_SYMBOL64 symbol = (PIMAGEHLP_SYMBOL64)&m_stackSymbol;
            ZeroMemory(symbol , SYM_BUFF_SIZE);
            symbol->SizeOfStruct	= sizeof(IMAGEHLP_SYMBOL64);
            symbol->MaxNameLength	= SYM_BUFF_SIZE - sizeof(IMAGEHLP_SYMBOL64);
            symbol->Address			= m_stackFrame.AddrPC.Offset;

            if(TRUE == SymGetSymFromAddr64(	GetCurrentProcess(),
											m_stackFrame.AddrPC.Offset,
											&displacement,
											symbol))
            {
                if(options & ~DUMP_SYMBOL)
                {
                    iCurr += wsprintf(m_dumpText + iCurr , _T ( "," ));
                }

                // Copy no more symbol information than there's room
                // for.  Symbols are ANSI
                int iLen = lstrlenA( symbol->Name );
                if(iLen > (BUFF_SIZE - iCurr - (MAX_SYM_SIZE + 50)))
                {
                    // Get some room on the stack to convert the string.
                    lstrcpyn(	m_dumpText + iCurr,
								symbol->Name,
								BUFF_SIZE - iCurr - 1);

					// Gotta leave now
                    szRet = m_dumpText;
                    __leave;
                }
                else
                {
                    if(displacement > 0)
                    {
                        iCurr += wsprintf ( m_dumpText + iCurr,
                                            //_T("%S()+%04d byte(s)"),  // for unicode
											_T("%s()+%04d byte(s)"),
                                            symbol->Name,
                                            displacement);
                    }
                    else
                    {
                        iCurr += wsprintf ( m_dumpText + iCurr,
                                            //_T("%S"), // for unicode
											_T("%s"),
                                            symbol->Name);
                    }
                }
            }
            else
            {
                // If the symbol wasn't found, the source file and line
                // number won't be found either, so leave now.
                szRet = m_dumpText;
                __leave;
            }
		}

        // Output the source file and line number information?
        if(DUMP_SRCLINE == (options & DUMP_SRCLINE))
        {
            ZeroMemory(&m_stackLine , sizeof(IMAGEHLP_LINE64));
            m_stackLine.SizeOfStruct = sizeof(IMAGEHLP_LINE64);

            DWORD dwLineDisp;
            if(TRUE == SymGetLineFromAddr64(GetCurrentProcess(),
											m_stackFrame.AddrPC.Offset,
											&dwLineDisp,
											&m_stackLine))
            {
                if(options & ~DUMP_SRCLINE)
                {
                    iCurr += wsprintf(m_dumpText + iCurr , _T ( "," ));
                }

                // Copy no more of the source file and line number
                // information than there's room for.
                int iLen = lstrlenA(m_stackLine.FileName);
                if(iLen > (BUFF_SIZE - iCurr - (MAX_PATH + 50)))
                {
					// Get some room on the stack to convert the string.
                    lstrcpyn(m_dumpText + iCurr, m_stackLine.FileName, BUFF_SIZE - iCurr - 1);

					// Gotta leave now
                    szRet = m_dumpText;
                    __leave;
                }
                else
                {
                    if(dwLineDisp > 0)
                    {
                        iCurr += wsprintf( m_dumpText + iCurr,
                                           //_T(" %S, line %04d+%04d byte(s)"),
										   _T(" %s, line %04d+%04d byte(s)"),
                                           m_stackLine.FileName,
                                           m_stackLine.LineNumber,
                                           dwLineDisp);
                    }
                    else
                    {
                        iCurr += wsprintf(	m_dumpText + iCurr,
                                            //_T(" %S, line %04d"),
											_T(" %s, line %04d"),
                                            m_stackLine.FileName,
                                            m_stackLine.LineNumber);
                    }
                }
            }
        }

        szRet = m_dumpText;
    }
    __except(EXCEPTION_EXECUTE_HANDLER)
    {
        szRet = NULL;
    }
    return (szRet);
}

LPCTSTR MiniDump::GetFirstStackTraceString(DWORD options, EXCEPTION_POINTERS * exceptionInfo)
{
	if(TRUE == IsBadReadPtr(exceptionInfo, sizeof(EXCEPTION_POINTERS*)))
    {
        Debug(_T("GetFirstStackTraceString - invalid exceptionInfo\r\n"));
        return (NULL);
	}

    // Get the stack frame filled in.
   MiniDump:: FillInStackFrame(exceptionInfo->ContextRecord);

    // Copy over the exception pointers fields so I don't corrupt the
    // real one.
    m_stackContext = *(exceptionInfo->ContextRecord);

    return (MiniDump::InternalGetStackTraceString(options));
}

LPCTSTR MiniDump::GetNextStackTraceString(DWORD options, EXCEPTION_POINTERS* exceptionInfo)
{
    // All error checking is in InternalGetStackTraceString.
    // Assume that GetFirstStackTraceString has already initialized the
    // stack frame information.
	return (InternalGetStackTraceString(options));
}

void MiniDump::PrintCallStack(struct _EXCEPTION_POINTERS *exceptionInfo)
{
    TCHAR dumpPath[MAX_PATH] = {0};

	SYSTEMTIME SystemTime;
    GetLocalTime(&SystemTime);

    _sntprintf_s(
		dumpPath, 
		_countof(dumpPath),
		MAX_PATH, 
		_T("%s\\%s(%04d%02d%02d-%02d%02d%02d).txt"), 
		MiniDump::m_folername,
		MiniDump::m_filename,
        SystemTime.wYear,
        SystemTime.wMonth,
        SystemTime.wDay,
        SystemTime.wHour,
        SystemTime.wMinute,
        SystemTime.wSecond);

	FileWriter file;
	if(file.Open(dumpPath))
	{
		// 콜스택 정보
		DWORD options = DUMP_MODULE | DUMP_SYMBOL | DUMP_SRCLINE; //DUMP_PARAMS
		const TCHAR* dumpText = GetFirstStackTraceString(options, exceptionInfo);
		do
		{
			file.WriteFormat(_T("%s\r\n"), dumpText);
			dumpText = GetNextStackTraceString(options, exceptionInfo);
		}
		while(NULL != dumpText);

		// 파일저장
		file.Close();
	}
}
