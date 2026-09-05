#pragma once

#include <map>
#include <string>


class cDemonLauncher
{
public:
	cDemonLauncher(void);
	~cDemonLauncher(void);

public:
	BOOL Play(const TCHAR* service, const BOOL console);
	BOOL Stop(const TCHAR* service, const BOOL console);

	BOOL CreateConsole(const TCHAR* title, TCHAR* command, TCHAR* arguments);

public:
	void SetPI(const TCHAR* service, PROCESS_INFORMATION& pi);
	void RemovePI(const TCHAR* service);

	uint32 GetPID(const TCHAR* service);
	HANDLE GetHandle(const TCHAR* service);

	BOOL IsRunningConsole(const TCHAR* service);
	BOOL IsRunningService(const TCHAR* service);

protected:
	typedef std::map<std::tstring,PROCESS_INFORMATION> PROCESS_TABLE;
	PROCESS_TABLE m_demons;
};

extern cDemonLauncher g_demonLauncher;