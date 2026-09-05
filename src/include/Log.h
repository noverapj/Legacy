// common

class ioLogger;

class CLog
{
public:
	CLog(void);
	virtual ~CLog(void);
	void Init();
	void Destroy();
	void InitData();

public:
	void OpenLog(int logLevel, char* fileName, bool append = false);
	void CloseLog();
	void PrintNoEnterLog(int logLevel, LPSTR fmt,...);
	void PrintLog(int logLevel, LPSTR fmt,...);
	void PrintTimeAndLog(int logLevel, LPSTR fmt,...);
	void DebugLog(int logLevel, LPSTR filename, int linenum, LPSTR fmt,...);
	void DebugLog(int logLevel, LPSTR fileLine, LPSTR fmt,...); 

public:
	void SetTcpMode(char* daemonName, int tcpPort, int svrPort, char* ipAddr);
	void SetCategory(char* categoryName, BOOL tcpState = TRUE);

public:
	void GetMemoryInfo(int& remainCount, int& usingCount, int& maxUsing, int& dropCount);

protected:
	ioLogger* m_logger;

};


