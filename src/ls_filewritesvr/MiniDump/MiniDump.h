#pragma once

#include <DBGHELP.h>
#include <PSAPI.h>

#define DUMP_PARAMS    0x01
#define DUMP_MODULE    0x02
#define DUMP_SYMBOL    0x04
#define DUMP_SRCLINE   0x08

#define BUFF_SIZE 2048
#define SYM_BUFF_SIZE 1024
#define MAX_SYM_SIZE  512


LONG WINAPI UnHandledExceptionFilter(struct _EXCEPTION_POINTERS *exceptionInfo);

// ¹Ì´Ï´ýÇÁ
class MiniDump
{
public:
    static BOOL Begin(const TCHAR* filename);
    static BOOL End(VOID);

public:
	static void InitSymEng(void);
	static void FillInStackFrame(PCONTEXT contextRecord);
	static LPCTSTR InternalGetStackTraceString(DWORD options);
	static LPCTSTR GetFirstStackTraceString(DWORD options, EXCEPTION_POINTERS * exceptionInfo);
	static LPCTSTR GetNextStackTraceString(DWORD options, EXCEPTION_POINTERS* exceptionInfo);

	static void PrintCallStack(struct _EXCEPTION_POINTERS *exceptionInfo);
	
public:
	static TCHAR m_filename[MAX_PATH];
	static TCHAR m_folername[MAX_PATH];

	static TCHAR m_dumpText[BUFF_SIZE];
	static BYTE m_stackSymbol[SYM_BUFF_SIZE];
	static IMAGEHLP_LINE64 m_stackLine;
	static CONTEXT m_stackContext;
	static STACKFRAME64 m_stackFrame;
	static BOOL m_symEngInit;
};
