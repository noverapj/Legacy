#include "stdafx.h"
#include "Service.h"

using namespace std;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
Service* Service::_serviceMainClass = NULL;

Service::Service(int argc, TCHAR **argv)
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

Service::~Service()
{
}

void Service::ServiceMainProc()
{
	if(m_arguments.size() > 0)
	{
		if(m_arguments[0] == _T("-i"))
		{
			if(m_arguments.size() < 3) return;

			// -i serviceName displayName
			Debug(_T("service : install\r\n"));

			m_serviceName	= m_arguments[1];
			m_displayName	= m_arguments[2];

			if(Install(m_serviceName.c_str(), m_displayName.c_str()))
			{
				Debug(_T("service installed\r\n"));
			}
			else
			{
				DWORD error = GetLastError();
				Debug(_T("service install failed\r\n"));
			}
		}
		else if(m_arguments[0] == _T("-u"))
		{
			if(m_arguments.size() < 2) return;

			// -u serviceName
			Debug(_T("service : uninstall\r\n"));

			m_serviceName = m_arguments[1];

			if(Uninstall(m_serviceName.c_str()))
			{
				Debug(_T("service uninstalled\r\n"));
			}
			else
			{
				DWORD error = GetLastError();
				Debug(_T("service uninstall failed\r\n"));
			}
		}
		else if(m_arguments[0] == _T("-c"))
		{
			if(m_arguments.size() < 2) return;

			// -c scriptName
			Debug(_T("service : console\r\n"));

			TCHAR fullPath[MAX_PATH];
			GetCurrentDirectory(MAX_PATH, fullPath);
			sprintf_s(fullPath, sizeof(fullPath), _T("%s\\%s"), fullPath, m_arguments[1].c_str());
			m_arguments[1] = fullPath;

			ServiceStart();
		}
		else
		{
			Debug(_T("USAGE : \r\n"));
			Debug(_T("	    -i [service name] [display name]\r\n"));
			Debug(_T("	    -u [service name]\r\n"));
			Debug(_T("	    i------------Creates a service\r\n"));
			Debug(_T("	    u------------Deletes a service\r\n"));
			Debug(_T("	     ------------Run as service\r\n"));
			Debug(_T("EXAMPLE : \r\n"));
			Debug(_T("	    -i ls-service \"LostSaga Service\"\r\n"));
		}
	}
	else
	{
		_ExecuteProcess();
	}
}

void WINAPI Service::_ServiceMain(int argc, TCHAR **argv)
{
	if (_serviceMainClass != NULL)
	{
		_serviceMainClass->ServiceMain(argc, argv);
	}
}

void Service::ServiceMain(int argc, TCHAR **argv)
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

BOOL Service::Install(const TCHAR* serviceName, const TCHAR* displayName)
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
		SERVICE_WIN32_OWN_PROCESS,
		SERVICE_DEMAND_START,
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

BOOL Service::Uninstall(const TCHAR* serviceName)
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

BOOL Service::KillService(const TCHAR* serviceName) 
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

BOOL Service::RunService(const TCHAR* serviceName) 
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

void WINAPI Service::_ServiceHandler(DWORD fdwControl)
{
	if(_serviceMainClass != NULL)
	{
		_serviceMainClass->ServiceHandler(fdwControl);
	}
}

void Service::ServiceHandler(DWORD fdwControl)
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

BOOL Service::_ExecuteProcess()
{
	SERVICE_TABLE_ENTRY dipatchTable[] = 
	{
		{const_cast<LPTSTR>(m_serviceName.c_str()), reinterpret_cast<LPSERVICE_MAIN_FUNCTION>(_ServiceMain)},
		{NULL , NULL}
	}; 

	StartServiceCtrlDispatcher(dipatchTable);
	return TRUE;
}
