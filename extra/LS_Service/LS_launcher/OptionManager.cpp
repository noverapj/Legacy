#include "StdAfx.h"
#include "OptionManager.h"


OptionManager::OptionManager(void)
{
	TCHAR temp[512];
	GetCurrentDirectory(_countof(temp), temp);
	m_folder = temp;

	Init();
}

OptionManager::~OptionManager(void)
{
	Destroy();
}

void OptionManager::Init()
{

	m_options.reserve(OPTION_END);
	for(int i = 0 ; i <= OPTION_END ; i++)
	{
		m_options.push_back(FALSE);
	}
}

void OptionManager::Destroy()
{
	m_lock.lock();
	m_logFile.Close();
	m_lock.unlock();
}

void OptionManager::WriteLog( const TCHAR* fmt,... )
{
	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);

	TCHAR tFilePath[MAX_PATH];
	_sntprintf_s(tFilePath, _countof(tFilePath),_T("%s\\log%04d%02d%02d.txt"), m_folder, sysTime.wYear,  sysTime.wMonth, sysTime.wDay); 

	TCHAR message[1024];
	va_list marker; 
	va_start(marker, fmt); 
	_vstprintf_s(message, _countof(message), fmt, marker); 
	va_end(marker);

	m_lock.lock();

	if(m_logFile.Open(tFilePath, CFile::modeCreate | CFile::modeNoTruncate | CFile::shareDenyNone | CFile::modeReadWrite))
	{
		if(0 == m_logFile.GetLength())
		{
			USHORT BOM = 0xfeff;
			m_logFile.Write(&BOM,2);
		}
		m_logFile.SeekToEnd(); 

		CString text;
		text.Format(L"[%04d-%02d-%02d %02d:%02d:%02d] %s \r\n", 
			sysTime.wYear , 
			sysTime.wMonth, 
			sysTime.wDay, 
			sysTime.wHour,
			sysTime.wMinute,
			sysTime.wSecond,
			message);

		m_logFile.Write(text.GetBuffer(), text.GetLength()*sizeof(TCHAR));
		m_logFile.Close();
	}

	m_lock.unlock();
}

void OptionManager::LoadOptions()
{
	CString optName;

	if(GetOptionName(OPTION_MONITOR, optName))
	{
		BOOL b = GetRegistry(optName);
		m_options[OPTION_MONITOR] = b;
	}

	if(GetOptionName(OPTION_RECURSIVE, optName))
	{
		BOOL b = GetRegistry(optName);
		m_options[OPTION_RECURSIVE] = b;
	}

	if(GetOptionName(OPTION_CONSOLE, optName))
	{
		BOOL b = GetRegistry(optName);
		m_options[OPTION_CONSOLE] = b;
	}
}

void OptionManager::SetOption(const OptionTypes opt, BOOL b)
{
	m_options[opt] = b;

	CString optName;
	if(GetOptionName(opt, optName))
	{
		SetRegistry(optName, b ? 1 :0);
	}
}
BOOL OptionManager::GetOption(const OptionTypes opt)
{
	return m_options[opt];
}

void OptionManager::SetRegistry(const TCHAR* option, int value)
{
	TCHAR regKey[512];
	_stprintf_s(regKey, _countof(regKey), _T("software\\lostsaga\\ls_launcher"));
	
	HKEY key;
	DWORD disp = 0;
	LONG ret = RegCreateKeyEx(HKEY_CURRENT_USER, regKey, 
		0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &key, &disp);
	if(ret != ERROR_SUCCESS) return;

	uint32 length = sizeof(value);
	RegSetValueEx(key, option, 0, REG_DWORD, (BYTE*)&value, length);

	RegCloseKey(key);
}

int OptionManager::GetRegistry(const TCHAR* option)
{
	HKEY key;
	LONG ret = RegOpenKeyEx(HKEY_CURRENT_USER, _T("software\\lostsaga\\ls_launcher"), 0, KEY_ALL_ACCESS, &key);
	if(ERROR_SUCCESS != ret) return 0;

	DWORD value = 0;
	DWORD size = sizeof(value);
	if(RegQueryValueEx(key, option, NULL, NULL, (LPBYTE)&value, &size) != ERROR_SUCCESS)
	{
		RegCloseKey(key);
		return value;
	}
	return value;
}

BOOL OptionManager::GetOptionName(const OptionTypes opt, CString& name)
{
	switch(opt)
	{
	case OPTION_MONITOR :
		name = _T("monitor");
		return TRUE;

	case OPTION_RECURSIVE :
		name = _T("recursive");
		return TRUE;
	
	case OPTION_CONSOLE :
		name = _T("console");
		return TRUE;
	}

	name = "";
	return FALSE;
}
