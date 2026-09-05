#include "stdafx.h"
#include "cService.h"

using namespace std;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
cService* cService::_serviceMainClass = NULL;

cService::cService(int argc, TCHAR **argv)
{
	_serviceMainClass	= this;

	m_serviceName = _T("");
	m_displayName = _T("");

	m_arguments.clear();
	m_arguments.reserve(argc + 1);
	for(int i = 1 ; i < argc ; i++)
	{
		m_arguments.push_back(argv[i]);
	}
}

cService::~cService()
{
}

void cService::ServiceMainProc()
{
	if(m_arguments.size() > 0)
	{
		if(m_arguments[0] == _T("-i"))
		{
			if(m_arguments.size() < 3) return;

			// -i serviceName displayName
			Debug(_T("service : install\n"));

			m_serviceName = m_arguments[1];
			m_displayName = m_arguments[2];

			if(Install(m_serviceName.c_str(), m_displayName.c_str()))
			{
				Debug(_T("service installed\n"));
			}
			else
			{
				Debug(_T("service install failed\n"));
			}
		}
		else if(m_arguments[0] == _T("-u"))
		{
			if(m_arguments.size() < 2) return;

			// -u serviceName
			Debug(_T("service : uninstall\n"));

			m_serviceName = m_arguments[1];

			if(Uninstall(m_serviceName.c_str()))
			{
				Debug(_T("service uninstalled\n"));
			}
			else
			{
				Debug(_T("service uninstall failed\n"));
			}
		}
		else if(m_arguments[0] == _T("-c"))
		{
			if(m_arguments.size() < 1) return;

			// -c 
			Debug(_T("service : console\n"));

			ServiceStart();
		}
		else
		{
			Debug(_T("USAGE : \n"));
			Debug(_T("	    -i [service name] [display name]\n"));
			Debug(_T("	    -u [service name]\n"));
			Debug(_T("	    -c [lua file]\n"));
			Debug(_T("	    [lua file] [log file]\n\n"));
			Debug(_T("	    i------------Creates a service\n"));
			Debug(_T("	    u------------Deletes a service\n"));
			Debug(_T("	    c------------Run as console\n"));
			Debug(_T("	     ------------Run as service\n"));
			Debug(_T("EXAMPLE : \n"));
			Debug(_T("	    -i rod-service \"R.O.D game service\"\n"));
			Debug(_T("	    -c lua-file.lua\n"));
		}
	}
	else
	{
		_ExecuteProcess();
	}
}

void WINAPI cService::_ServiceMain(int argc, TCHAR **argv)
{
	if (_serviceMainClass != NULL)
	{
		_serviceMainClass->ServiceMain(argc, argv);
	}
}

void cService::ServiceMain(int argc, TCHAR **argv)
{
	m_hServiceStatusHandle = RegisterServiceCtrlHandler(m_serviceName.c_str(), _ServiceHandler); 
	if(0 == m_hServiceStatusHandle)
	{
		return; 
	} 
 
	m_arguments.clear();
	m_arguments.reserve(argc + 1);
	m_arguments.push_back(_T(""));
	for(int i = 1 ; i < argc ; i++)
	{
		m_arguments.push_back(argv[i]);
	}

	// Initialization complete - report running status 
	m_serviceStatus.dwServiceType		= SERVICE_WIN32; 
	m_serviceStatus.dwCurrentState		= SERVICE_START_PENDING; 
	m_serviceStatus.dwControlsAccepted	= SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN; 
	m_serviceStatus.dwWin32ExitCode		= NO_ERROR; 
	m_serviceStatus.dwServiceSpecificExitCode = 0; 
	m_serviceStatus.dwCheckPoint		= 0; 
	m_serviceStatus.dwWaitHint			= 0; 
	m_serviceStatus.dwCheckPoint		= 0; 
	m_serviceStatus.dwWaitHint			= 0;  
	m_serviceStatus.dwCurrentState		= SERVICE_RUNNING; 

	SetServiceStatus(m_hServiceStatusHandle, &m_serviceStatus);

	if(!ServiceStart())
	{
		m_serviceStatus.dwCurrentState	= SERVICE_STOPPED;
		SetServiceStatus(m_hServiceStatusHandle, &m_serviceStatus);	
	}
}

BOOL cService::Install(const TCHAR* serviceName, const TCHAR* displayName)
{  
	// 실행 파일 경로
	TCHAR serviceDemon[MAX_PATH+1];			
	DWORD size = GetModuleFileName(NULL, serviceDemon, MAX_PATH);
	serviceDemon[size] = 0;

	SC_HANDLE schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE); 
	if(0 == schSCManager) return FALSE;

	SC_HANDLE schService = CreateService( 
		schSCManager,
		serviceName,
		displayName,
		SERVICE_ALL_ACCESS,
		SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS,
		SERVICE_AUTO_START,
		SERVICE_ERROR_NORMAL,
		serviceDemon,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL);
	if(0 == schService) return FALSE;

	CloseServiceHandle(schService); 
	CloseServiceHandle(schSCManager);
	return TRUE;
}

BOOL cService::Uninstall(const TCHAR* serviceName)
{
	// 서비스 관리자 얻기
	SC_HANDLE schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS); 
	if(0 == schSCManager) return FALSE;

	// 서비스 핸들 얻기
	SC_HANDLE schService = OpenService(schSCManager, serviceName, DELETE);
	if(0 == schService) return FALSE;
	
	// 서비스 삭제
	if(!DeleteService(schService)) 
	{
		CloseServiceHandle(schService); 
		CloseServiceHandle(schSCManager);	
		return FALSE;
	}

	CloseServiceHandle(schService); 
	CloseServiceHandle(schSCManager);	
	return TRUE;
}

BOOL cService::KillService(const TCHAR* serviceName) 
{ 
	// 서비스 관리자 얻기
	SC_HANDLE schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if(0 == schSCManager) return FALSE;

	// 서비스 핸들 얻기
	SC_HANDLE schService = OpenService(schSCManager, serviceName, SERVICE_STOP);
	if(0 == schService) return FALSE;

	// 서비스 중지
	SERVICE_STATUS status;
	if(!ControlService(schService, SERVICE_CONTROL_STOP, &status))
	{
		CloseServiceHandle(schService); 
		CloseServiceHandle(schSCManager); 
		return FALSE;
	}

	CloseServiceHandle(schService); 
	CloseServiceHandle(schSCManager); 
	return TRUE;
}

BOOL cService::RunService(const TCHAR* serviceName) 
{ 
	// 서비스 관리자 얻기
	SC_HANDLE schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if(0 == schSCManager) return FALSE;

	// 서비스 핸들 얻기
	SC_HANDLE schService = OpenService(schSCManager, serviceName, SERVICE_START);
	if(0 == schService) return FALSE;

	// 서비스 시작
	if(!StartService(schService, 0, 0))
	{
		CloseServiceHandle(schService); 
		CloseServiceHandle(schSCManager); 
		return FALSE;
	}

	CloseServiceHandle(schService); 
	CloseServiceHandle(schSCManager); 
	return TRUE;
}

void WINAPI cService::_ServiceHandler(DWORD fdwControl)
{
	if(_serviceMainClass != NULL)
	{
		_serviceMainClass->ServiceHandler(fdwControl);
	}
}

void cService::ServiceHandler(DWORD fdwControl)
{
	switch(fdwControl) 
	{
	case SERVICE_CONTROL_STOP:
	case SERVICE_CONTROL_SHUTDOWN:
		m_serviceStatus.dwWin32ExitCode = 0; 
		m_serviceStatus.dwCurrentState  = SERVICE_STOPPED;
		m_serviceStatus.dwCheckPoint    = 0; 
		m_serviceStatus.dwWaitHint      = 0;

		ServiceStop();
		break; 

	case SERVICE_CONTROL_PAUSE:
		m_serviceStatus.dwCurrentState = SERVICE_PAUSED;
		break;

	default:
		m_serviceStatus.dwCurrentState = SERVICE_RUNNING;
	};

    SetServiceStatus(m_hServiceStatusHandle, &m_serviceStatus);
}

BOOL cService::_ExecuteProcess()
{
	SERVICE_TABLE_ENTRY dipatchTable[] = 
	{
		{const_cast<LPTSTR>(m_serviceName.c_str()), reinterpret_cast<LPSERVICE_MAIN_FUNCTION>(_ServiceMain)},
		{NULL , NULL}
	}; 

	StartServiceCtrlDispatcher(dipatchTable);
	return TRUE;
}
