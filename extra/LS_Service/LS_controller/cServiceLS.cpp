#include "stdafx.h"
#include "cFile.h"
#include "cManager.h"
#include "cMiniDump.h"
#include "cServiceLS.h"
#include "../include/cSingleton.h"
#include <iostream>

using namespace std;

void DisplayHeapInfo(HANDLE heap) 
{ 
	ULONG heapInfo; 
	SIZE_T size; 

	if(HeapQueryInformation(
		heap,
		HeapCompatibilityInformation,
		&heapInfo, 
		sizeof(heapInfo), 
		&size) == FALSE) 
	{ 
		Debug(); 
	} 
	else 
	{
		//Information(_T("Handle: %X, HeapInfo: %d\n", heap, heapInfo); 
	}
} 

void SetHeapInformation()
{
	HANDLE heaps[1025];
	BOOL results[1025];

	ZeroMemory(heaps, sizeof(heaps));
	ZeroMemory(results, sizeof(results));

	ULONG HeapFragValue = 2;

	DWORD count = GetProcessHeaps(1024, heaps);
	for(DWORD i = 0; i < count; i++)
	{
		results[i] = HeapSetInformation(
			heaps[i],
			HeapCompatibilityInformation,
			&HeapFragValue,
			sizeof(HeapFragValue));
		if(!results[i])
		{
			DisplayHeapInfo(heaps[i]);
		}
	}
}

void SetCurrentModulePath()
{
	TCHAR temp[MAX_PATH+1];
	GetModuleFileName(NULL, temp, MAX_PATH);

	TCHAR* token = _tcsrchr(temp, _T('\\'));
	*(token+1) = _T('\0');
	SetCurrentDirectory(temp);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

cServiceLS::cServiceLS(int argc, TCHAR **argv) : cService(argc, argv)
{
}

cServiceLS::~cServiceLS()
{

}

BOOL cServiceLS::ExecuteProcess()
{
	return TRUE;
}

void cServiceLS::ServiceStop()
{
	Debug(_T("service stopped\r\n"));
	cMiniDump::End();	
}

BOOL cServiceLS::ServiceStart()
{
	ConfigureSystem();

	cMiniDump::Begin(_T("lostsaga-controller"));

	m_logFile = _T("lostsaga-controller.txt");

	Debug(_T("----------------------------------------------------\r\n"));
	Debug(_T("-- start lostsaga-controller\r\n"));
	Debug(_T("----------------------------------------------------\r\n"));

	if(!cSingleton<cManager>::GetInstance()->Init(10, 80))
	{
		TCHAR error[512];
		wsprintf( error, _T("ls_controller Init failed(%lu)\r\n"), GetLastError());
		Debug(error);
		return FALSE;
	}

	if(!cSingleton<cManager>::GetInstance()->Listen(24444, 10))
	{
		TCHAR error[512];
		wsprintf( error, _T("ls_controller Listen failed(%lu)\r\n"), GetLastError());
		Debug(error);
		return FALSE;
	}

	if(!cSingleton<cManager>::GetInstance()->Start(2, 1))
	{
		TCHAR error[512];
		wsprintf( error, _T("ls_controller Start failed(%lu)\r\n"), GetLastError());
		Debug(error);
		return FALSE;
	}

	Debug(_T("service start - success\r\n"));
	cSingleton<cManager>::GetInstance()->Timer();
	return TRUE;
}

void cServiceLS::Debug(const TCHAR *message)
{
	if(0 == m_logFile.size())
	{
#ifdef _UNICODE
		wcout <<  message;
#else
		cout <<  message;
#endif
	}
	else
	{
		/*
		SYSTEMTIME systime;
		GetLocalTime(&systime);

		TCHAR currentTime[256];
		_stprintf_s(
			currentTime, 
			sizeof(currentTime),
			"[%04d-%02d-%02d %02d:%02d:%02d]", 
			systime.wYear,
			systime.wMonth,
			systime.wDay,
			systime.wHour,
			systime.wMinute,
			systime.wSecond);
		*/
		cFileWriter file; 
		if(file.Open(GetLogFile(), OPEN_ALWAYS))
		{
			uint32 length = (_tcslen(message))*sizeof(TCHAR);
			file.Move(FILE_BEGIN, file.GetFileSize());
			file.Write(reinterpret_cast<const BYTE*>(message), length);
			file.Close();
		}
	}
}

void cServiceLS::ConfigureSystem()
{
	SetHeapInformation();
	SetCurrentModulePath();
}
