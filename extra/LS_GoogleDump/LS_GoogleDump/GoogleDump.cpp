#include <tchar.h>
#include <iostream>
#include <string>
#include "ExceptionHandler.h"
#include "GoogleDump.h"

google_breakpad::ExceptionHandler *g_handler;
TCHAR GoogleDump::m_foldername[MAXPATH];
TCHAR GoogleDump::m_filename[MAXPATH];

GoogleDump::GoogleDump()
{
	g_handler = NULL;
}

GoogleDump::~GoogleDump()
{
	End();
}

void GoogleDump::Begin(const TCHAR* fileName)
{
	SetErrorMode(SEM_FAILCRITICALERRORS);

	TCHAR temp[256];
	GetCurrentDirectory(_countof(temp), temp);
	_sntprintf_s(m_foldername,_countof(m_foldername), _T("%s\\dump\\"), temp);
	_tcscpy_s(m_filename,_countof(m_filename), fileName);
	CreateDirectory(m_foldername, NULL);

	g_handler = new google_breakpad::ExceptionHandler(
		std::string(m_foldername,m_foldername+_tcslen(m_foldername)),
		NULL,
		NULL,
		NULL,
		google_breakpad::ExceptionHandler::HANDLER_ALL,
		MiniDumpNormal);
}

void GoogleDump::End()
{
	if(g_handler != NULL)
	{
		delete g_handler;
		g_handler = NULL;
	}
}