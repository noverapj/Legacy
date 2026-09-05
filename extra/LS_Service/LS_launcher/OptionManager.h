#pragma once

#include <vector>
#include <boost/thread/mutex.hpp>

enum OptionTypes
{
	OPTION_NONE = 0,
	OPTION_APPLYALL,
	OPTION_WATCHDOG,
	OPTION_MONITOR,
	OPTION_RECURSIVE,
	OPTION_CONSOLE,

	OPTION_END
};

class OptionManager
{
public:
	OptionManager(void);
	virtual ~OptionManager(void);
	
	void Init();
	void Destroy();

public:
	void WriteLog(const TCHAR* fmt,...);

public:
	void LoadOptions();

	void SetOption(const OptionTypes opt, BOOL b);
	BOOL GetOption(const OptionTypes opt);

protected:
	void SetRegistry(const TCHAR* option, int value);
	int GetRegistry(const TCHAR* option);

	BOOL GetOptionName(const OptionTypes opt, CString& name);

private:
	typedef std::vector<BOOL> OPTIONS;

	OPTIONS m_options;
	
	CString m_folder;
	CFile m_logFile;
	boost::mutex m_lock;
};

