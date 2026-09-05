#pragma once

class ioDBServer;
class ioLogServer;

class Manager
{
public:
	Manager(void);
	~Manager(void);

	void Init();
	void Destroy();

public:
	BOOL Run(const char* scriptName);

	int GetErrorCode() { return m_error; }

protected:
	void Startup(const char* scriptName);
	BOOL LoadINI();
	BOOL CreateDB();
	BOOL CreateUserPool();
	BOOL ListenNetwork();
	BOOL StartModules();
	BOOL Prepare();
	void Timer();

protected:
	ioHashString m_szPrivateIP;
	ioHashString m_szPublicIP;

	ioDBServer *m_pDBServer;
	ioLogServer* m_pLogServer;
	LogicThread *m_logicThread;

	char m_currentPath[512];
	USHORT m_port;

	char m_dbIP[512];
	USHORT m_dbPort;
	char m_dbName[512];
	char m_dbID[512];
	char m_dbPwd[512];

	char m_logSvrIPAddr[16];
	int m_logSvrPort;
	
protected:
	int m_error;
};

