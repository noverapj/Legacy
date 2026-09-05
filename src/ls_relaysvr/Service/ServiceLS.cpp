#include "stdafx.h"
 
 
#include "ServiceLS.h"
#include <iostream>
#include "../Manager.h"


using namespace std;

void ShutDown()
{
	LOG.PrintTimeAndLog(0, "<<< --------------------  End File -------------------- >>>\r\n");
	LOG.CloseLog();
	ReportLOG.PrintTimeAndLog(0, "<<< --------------------  End File -------------------- >>>\r\n");
	ReportLOG.CloseLog();
	HackLOG.PrintTimeAndLog(0, "<<< --------------------  End File -------------------- >>>\r\n");
	HackLOG.CloseLog();
	MiniDump::End();
	Sleep(300);
	g_IoServices()->stop();
	g_UDPNode()->Destroy();
	
	
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

ServiceLS::ServiceLS(int argc, TCHAR **argv) : Service(argc, argv)
{
	m_iniFile = _T("ls_relaysvr.ini");
}

ServiceLS::~ServiceLS()
{
}

BOOL ServiceLS::ExecuteProcess()
{
	return TRUE;
}

void ServiceLS::ServiceStop()
{
	ConfigureTime();
	
	Debug(_T("service stopped\r\n"));
	Debug(GetCurrentTime());
	LOG.PrintTimeAndLog( 0, "- EXIT SERVER" );
	
 	
	ShutDown();


	//exit(1);
}

BOOL ServiceLS::ServiceStart()
{
	ConfigureSystem();
	ConfigureTime();

	MiniDump::Begin(_T("ls_relaysvr"));

	// option iniFile logFile
	switch(m_arguments.size())
	{
	case 2 :
		{
			m_iniFile = m_arguments[1];
		}
		break;

	case 3 :
		{
			m_iniFile = m_arguments[1];
			m_logFile = m_arguments[2];
		}
		break;

	default :
		return FALSE;

	}

	Debug(_T("----------------------------------------------------\r\n"));
	Debug(_T("-- start service ls_relaysvr\r\n"));
	Debug(GetCurrentTime());
	Debug(_T("----------------------------------------------------\r\n"));

	Manager manager;
	if(manager.Run((TCHAR*)m_iniFile.c_str()))
	{
		Debug(_T("service start - success\r\n"));

		boost::asio::io_service::work work(*g_IoServices());
		g_IoServices()->run();
		ShutDown();
		return TRUE;
	}
	else 
	{
		Debug(_T("service start - failed\r\n"));		
		return FALSE;
	}
	
}

void ServiceLS::Debug(const TCHAR *message)
{
	if(0 == m_logFile.size())
	{
#ifdef _UNICODE
		wcout << message << flush;
#else		
		cout << message << flush;
#endif
	}
	else
	{
		FileWriter file;
		if(file.Open(GetLogFile(), OPEN_ALWAYS))
		{
			uint32 length = _tcslen(message)*sizeof(TCHAR);
			file.Move(FILE_BEGIN, file.GetFileSize());
			file.Write(reinterpret_cast<const BYTE*>(message), length);
			file.Close();
		}
	}
}

void ServiceLS::ConfigureSystem()
{
	SetCurrentModulePath();
}

void ServiceLS::ConfigureTime()
{
	// 현재시간
	SYSTEMTIME systime;
	GetLocalTime(&systime);

	TCHAR time[512];
    _sntprintf_s(
		time, 
		sizeof(time),
		_T("-- [%04d-%02d-%02d %02d:%02d:%02d]\r\n"),
		systime.wYear,
		systime.wMonth,
		systime.wDay,
		systime.wHour,
		systime.wMinute,
		systime.wSecond);
	
	m_time = time;
}
