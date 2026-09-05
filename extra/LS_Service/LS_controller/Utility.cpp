#include "StdAfx.h"
#include "cService.h"
#include "Utility.h"
#include <iostream>
#include <wtsapi32.h>
#include <userenv.h>
#include <tlhelp32.h>


using namespace std;



BOOL FindUserWindow(const TCHAR* title, DWORD& PID);


// trace
void Trace( const TCHAR *format, ... ) 
{ 
#ifdef _DEBUG
	TCHAR buffer[2048]; 
	va_list marker; 

	va_start( marker, format ); 
	_vstprintf_s( buffer, _countof(buffer), format, marker ); 
	va_end( marker );

	OutputDebugString( buffer ); 
#endif
}

// console
void Debug( const TCHAR *format, ... )
{
#ifdef _DEBUG
	static TCHAR buffer[2048];

	va_list marker; 
	va_start(marker, format); 
	_vstprintf_s(buffer, _countof(buffer), format, marker); 
	va_end(marker);
	
	cService::_serviceMainClass->Debug(buffer);
	//wcout << buffer;
#endif
}

// console
void Information( const TCHAR *format, ... )
{
	static TCHAR buffer[2048]; 

	va_list marker; 
	va_start( marker, format ); 
	_vstprintf_s( buffer, _countof(buffer), format, marker ); 
	va_end( marker );

	cService::_serviceMainClass->Debug(buffer);
}

// last error
void Debug()
{
	DWORD dwError = WSAGetLastError();
	LPTSTR lpMsgBuf = NULL;
	FormatMessage( 	FORMAT_MESSAGE_ALLOCATE_BUFFER | 
					FORMAT_MESSAGE_FROM_SYSTEM | 
					FORMAT_MESSAGE_IGNORE_INSERTS,
					NULL, 
					dwError, 
					0, // Default language 
					(LPTSTR)&lpMsgBuf,	
					0,	
					NULL );


#ifdef _UNICODE
	wcout << _T("WSAGetLastError[") << dwError << _T("] : ") << lpMsgBuf << endl << flush;
#else
	cout << _T("WSAGetLastError[") << dwError << _T("] : ") << lpMsgBuf << endl << flush;
#endif
	

	// Free the buffer.
	LocalFree( lpMsgBuf );
}

void Tokenize(const std::tstring& str, TOKENS& tokens, const std::tstring& delimiters)
{
	tokens.clear();

	// 맨 첫 글자가 구분자인 경우 무시
	string::size_type lastPos = str.find_first_not_of(delimiters, 0);

	// 구분자가 아닌 첫 글자를 찾는다
	string::size_type pos     = str.find_first_of(delimiters, lastPos);

	while (string::npos != pos || string::npos != lastPos)
	{
		// token을 찾았으니 vector에 추가한다
		tokens.push_back(str.substr(lastPos, pos - lastPos));

		// 구분자를 뛰어넘는다.  "not_of"에 주의하라
		lastPos = str.find_first_not_of(delimiters, pos);

		// 다음 구분자가 아닌 글자를 찾는다
		pos = str.find_first_of(delimiters, lastPos);
	}
}

BOOL GetService(std::tstring& service, std::tstring& file, std::tstring& script)
{
	HKEY key;
	LONG ret = RegOpenKeyEx(HKEY_CURRENT_USER, _T("software\\LostSaga"), 0, KEY_ALL_ACCESS, &key);
	if(ERROR_SUCCESS != ret) return FALSE;

	DWORD size;
	FILETIME fileTime;
	TCHAR group[256], regFile[512], regScript[512];
	for(DWORD i = 0 ; i < 1000 ; i++)
	{
		size = _countof(group);

		ret = RegEnumKeyEx(key, i, group, &size, NULL, NULL, NULL, &fileTime);
		if(ERROR_SUCCESS != ret) break;
	
		if(service == group)
		{
			HKEY subkey;
			ret = RegOpenKeyEx(key, group, 0, KEY_ALL_ACCESS, &subkey);
			if(ERROR_SUCCESS != ret) break;

			size = sizeof(regFile);
			if(RegQueryValueEx(subkey, _T("file"), NULL, NULL, (LPBYTE)regFile, &size) != ERROR_SUCCESS)
			{
				RegCloseKey(subkey);
				RegCloseKey(key);
				return FALSE;
			}
			size = sizeof(regScript);
			if(RegQueryValueEx(subkey, _T("script"), NULL, NULL, (LPBYTE)regScript, &size) != ERROR_SUCCESS)
			{
				RegCloseKey(subkey);
				RegCloseKey(key);
				return FALSE;
			}

			file	= regFile;
			script	= regScript;
			RegCloseKey(subkey);
			return TRUE;
		}
	}
	RegCloseKey(key);
	return FALSE;
}


BOOL GetServices(STRINGS& services, STRINGS& files, STRINGS& scripts)
{
	services.clear();
	files.clear();
	scripts.clear();

	HKEY key;
	LONG ret = RegOpenKeyEx(HKEY_CURRENT_USER, _T("software\\LostSaga"), 0, KEY_ALL_ACCESS, &key);
	if(ERROR_SUCCESS != ret)
	{
		DWORD error = GetLastError();
		return FALSE;
	}

	DWORD size;
	FILETIME fileTime;
	TCHAR group[256], service[256], file[512], script[512];
	for(DWORD i = 0 ; i < 1000 ; i++)
	{
		size = _countof(group);

		ret = RegEnumKeyEx(key, i, group, &size, NULL, NULL, NULL, &fileTime);
		if(ERROR_SUCCESS != ret) break;
	
		//_stprintf_s(folder, sizeof(folder), "software\\LostSaga\\%s", group);

		HKEY subkey;
		ret = RegOpenKeyEx(key, group, 0, KEY_ALL_ACCESS, &subkey);
		if(ERROR_SUCCESS != ret) break;

		size = _countof(service);
		if(RegQueryValueEx(subkey, _T("service"), NULL, NULL, (LPBYTE)service, &size) != ERROR_SUCCESS)
		{
			RegCloseKey(subkey);
			RegCloseKey(key);
			return TRUE;
		}
		size = _countof(file);
		if(RegQueryValueEx(subkey, _T("file"), NULL, NULL, (LPBYTE)file, &size) != ERROR_SUCCESS)
		{
			RegCloseKey(subkey);
			RegCloseKey(key);
			return FALSE;
		}
		size = _countof(script);
		if(RegQueryValueEx(subkey, _T("script"), NULL, NULL, (LPBYTE)script, &size) != ERROR_SUCCESS)
		{
			RegCloseKey(subkey);
			RegCloseKey(key);
			return FALSE;
		}
		
		services.push_back(service);
		files.push_back(file);
		scripts.push_back(script);
		RegCloseKey(subkey);
	}
	RegCloseKey(key);
	return TRUE;
}


BOOL IsService(const TCHAR* serviceName)
{
	// 서비스 관리자 얻기
	SC_HANDLE schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS); 
	if(0 == schSCManager) return FALSE;

	// 서비스 핸들 얻기
	SC_HANDLE schService = OpenService(schSCManager, serviceName, GENERIC_ALL);
	if(0 == schService) return FALSE;
	
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
	return result;
}

BOOL CreateFolder(const TCHAR* path)
{
	std::tstring folder = path;

	TOKENS folders;
	Tokenize(folder, folders, _T("\\"));

	if(folders.size() > 1 )
	{
		std::tstring temp = folders[0];
		for(uint32 i = 1; i < folders.size(); i++)
		{
			temp = temp + _T("\\") + folders[i];
			::CreateDirectory(temp.c_str(), NULL);
		}
	}
	return TRUE;
}






BOOL FindUserWindow(const TCHAR* title, DWORD& PID)
{
	TCHAR text[512] = {0};

	HANDLE hProcessSnap; 
	PROCESSENTRY32 pe32; //프로세스정보를 저장할 구조체
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);

	// Set the size of the structure before using it.
	pe32.dwSize = sizeof( PROCESSENTRY32 );

	Process32First( hProcessSnap, &pe32 );
		printf( "\n  Process ID        = 0x%08X", pe32.th32ProcessID );
		printf( "\n  Thread count      = %d",   pe32.cntThreads );
		printf( "\n  Parent process ID = 0x%08X", pe32.th32ParentProcessID );
		printf( "\n  Priority base     = %d", pe32.pcPriClassBase );
	while(Process32Next( hProcessSnap, &pe32 ))
	{
		HANDLE hProcess = OpenProcess(PROCESS_VM_READ, FALSE, pe32.th32ProcessID);
		if(hProcess != NULL)
		{
			GetWindowText((HWND)hProcess, text, _countof(text));

			wcout << "title : " << text << endl;;

			if(0 == _tcscmp(title, text))
			{
				PID = pe32.th32ProcessID;
				return TRUE;
			}
		}
	}
	return FALSE;

	//HWINSTA hWinsta = OpenWindowStation(_T("winsta0"), FALSE, WINSTA_ALL_ACCESS);

	//// 데몬 서버 상태 얻기
	//HWND hWnd = FindWindow(NULL, title);
	//if(hWnd)
	//{
	//	DWORD ret = GetWindowThreadProcessId(hWnd, &PID);

	//	CloseWindowStation(hWinsta);
	//	return (ret != 0) ? TRUE : FALSE;
	//}

	//CloseWindowStation(hWinsta);
	//return FALSE;
}