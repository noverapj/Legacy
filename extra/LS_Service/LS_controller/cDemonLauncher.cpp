#include "StdAfx.h"
#include "cDemonLauncher.h"
#include <wtsapi32.h>
#include <userenv.h>


  


extern BOOL GetService(std::tstring& service, std::tstring& file, std::tstring& script);



cDemonLauncher g_demonLauncher;

cDemonLauncher::cDemonLauncher(void)
{
}


cDemonLauncher::~cDemonLauncher(void)
{
}

BOOL cDemonLauncher::Play(const TCHAR* service, const BOOL console)
{
	// 실행중인지 확인
	if(IsRunningService(service)) return FALSE;
	if(IsRunningConsole(service)) return FALSE;
	
	std::tstring key = service;
	std::tstring m_file;
	std::tstring m_script;
	if(GetService(key, m_file, m_script))
	{
		if(console == FALSE)
		{
			TCHAR command[512];

			_stprintf_s(command, _countof(command), _T("/c \"sc start %s %s %s.txt\""), service, m_script.c_str(), service);
			ShellExecute(NULL , NULL, _T("cmd.exe") , command, NULL, SW_HIDE); 

			Debug(_T("@Play : %s\r\n"), command);
			return TRUE;
		}
		else
		{	
			//TCHAR command[512];
			//TCHAR path[512]={0};
			//TCHAR tempPath[512]={0};
			//TCHAR drive[512]={0};
			//TCHAR exeFile[512]={0};
			//TCHAR tempExeFile[512]={0};
			//TCHAR luaFile[512]={0}; 
			//TCHAR tempLuaFile[512]={0};
			//TCHAR ext1[512]={0};
			//TCHAR ext2[512]={0};
			//TCHAR arguments[512]={0};

			//_tsplitpath_s(m_file.c_str(), drive, tempPath, tempExeFile, ext1);
			////_tsplitpath_s(m_script.c_str(), drive, tempPath, tempLuaFile, ext2);

			//_stprintf_s(path, _countof(path), _T("%s%s"), drive, tempPath);
			//_stprintf_s(exeFile, _countof(exeFile), _T("%s%s"), tempExeFile, ext1);
			////_stprintf_s(luaFile, _countof(luaFile), _T("%s%s"), tempLuaFile, ext2);

			////_stprintf_s(command, _countof(command), _T("/c \"start \"%s\" /D%s %s -c %s\""), service, path, exeFile, luaFile);
			////ShellExecute(NULL , NULL, _T("cmd.exe") , command, path, SW_SHOW);


			//_stprintf_s(command, _countof(command), _T("%s\\%s"), path, exeFile);
			//_stprintf_s(arguments, _countof(arguments), _T(" -c"));
			////_stprintf_s(arguments, _countof(arguments), _T(" -c %s"), luaFile);

			//CreateConsole(service, command, arguments);
			return TRUE;
		}
	}
	
	return FALSE;
}

BOOL cDemonLauncher::Stop(const TCHAR* service, const BOOL console)
{
	TCHAR command[512];
	//uint32 PID = GetPID(service);
	HANDLE hProcess = GetHandle(service);
	if( hProcess != 0 )
	{
		if(TerminateProcess(hProcess, 0))
		{
			RemovePI(service);
		}
		//_stprintf_s(command, _countof(command), _T("/c \"taskkill /PID %d\""), PID);
		//ShellExecute(NULL , NULL, _T("cmd.exe") , command, NULL, SW_HIDE); 
		return TRUE;
	}
	else
	{
		_stprintf_s(command, _countof(command), _T("/c \"sc stop %s\""), service);
		ShellExecute(NULL , NULL, _T("cmd.exe") , command, NULL, SW_HIDE); 
		return TRUE;
	}
	return FALSE;
}

BOOL cDemonLauncher::CreateConsole(const TCHAR* title, TCHAR* command, TCHAR* arguments)
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	ZeroMemory(&pi, sizeof(pi));

	//
	HANDLE hTokenNew = NULL, hTokenDup = NULL;
	WTSQueryUserToken(WTSGetActiveConsoleSessionId(), &hTokenNew);
	DuplicateTokenEx(hTokenNew,MAXIMUM_ALLOWED,NULL,SecurityIdentification,TokenPrimary,&hTokenDup);

	LPVOID  pEnv = NULL;
	if(!CreateEnvironmentBlock(&pEnv, hTokenDup, FALSE))
	{
		return FALSE;
	}
	// 

	ZeroMemory( &si, sizeof( STARTUPINFO ) );
	si.cb			= sizeof( STARTUPINFO );
	si.lpDesktop	= TEXT("winsta0\\default");
	si.dwFlags		= STARTF_USESHOWWINDOW;	
	si.wShowWindow	= SW_SHOW ;
	si.lpTitle		= const_cast<TCHAR*>(title); // (LPWSTR)

	if(CreateProcessAsUser(hTokenDup, command, arguments, NULL, NULL, FALSE, CREATE_UNICODE_ENVIRONMENT |NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE, pEnv, NULL, &si, &pi))
	{
		SetPI(title, pi);
	}
	else
	{
		DWORD error = GetLastError();
	}

	RevertToSelf();
	return TRUE;
}

void cDemonLauncher::SetPI(const TCHAR* service, PROCESS_INFORMATION& pi)
{
	if(!IsRunningConsole(service))
	{
		m_demons[service] = pi;
	}
}

void cDemonLauncher::RemovePI(const TCHAR* service)
{
	PROCESS_TABLE::iterator it = m_demons.find(service);
	if(it != m_demons.end())
	{
		m_demons.erase(it);
	}
}

uint32 cDemonLauncher::GetPID(const TCHAR* service)
{
	PROCESS_TABLE::iterator it = m_demons.find(service);
	if(it != m_demons.end())
	{
		PROCESS_INFORMATION pi = it->second;
		return pi.dwProcessId;
	}
	return 0;
}

HANDLE cDemonLauncher::GetHandle(const TCHAR* service)
{
	PROCESS_TABLE::iterator it = m_demons.find(service);
	if(it != m_demons.end())
	{
		PROCESS_INFORMATION pi = it->second;
		return pi.hProcess;
	}
	return 0;
}

BOOL cDemonLauncher::IsRunningConsole(const TCHAR* service)
{
	PROCESS_TABLE::const_iterator it = m_demons.find(service);
	if(it != m_demons.end())
	{
		return TRUE;
	}
	return FALSE;
}

BOOL cDemonLauncher::IsRunningService(const TCHAR* service)
{
	// 서비스 관리자 얻기
	SC_HANDLE schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS); 
	if(0 == schSCManager)
	{
		Debug(_T("IsRunning :: failed(%s)\n"), service);
		return FALSE;
	}

	// 서비스 핸들 얻기
	SC_HANDLE schService = OpenService(schSCManager, service, GENERIC_ALL);
	if(0 == schService) 
	{
		Debug(_T("IsRunning :: failed(%s)\n"), service);
		return FALSE;
	}
	
	// 서비스 상태얻기
	SERVICE_STATUS_PROCESS status;
	status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;

	DWORD bytesNeeded;
	BOOL result = QueryServiceStatusEx(
		schService,
		SC_STATUS_PROCESS_INFO,
		(LPBYTE)(&status),
		sizeof(status),
		&bytesNeeded);

	CloseServiceHandle(schService); 
	CloseServiceHandle(schSCManager);
	
	if(result)
	{
		switch(status.dwCurrentState)
		{
		case SERVICE_CONTINUE_PENDING :
			return FALSE;

		case SERVICE_PAUSE_PENDING :
			return FALSE;

		case SERVICE_PAUSED :
			return FALSE;

		case SERVICE_RUNNING :
			return TRUE;

		case SERVICE_START_PENDING :
			return TRUE;

		case SERVICE_STOP_PENDING :
			return FALSE;

		case SERVICE_STOPPED :
			return FALSE;
		}
	}
	return FALSE;
}
