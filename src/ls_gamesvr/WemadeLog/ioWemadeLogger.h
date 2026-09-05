#pragma once

#include <vector>
#include "../Util/cBuffer.h"

class ioWemadeLogger
{
public:
	ioWemadeLogger();
	~ioWemadeLogger();

	void Init();
	void Destroy();

public:
	void Register(const TCHAR* szServerIP);
	BOOL Create();
	void Close();

public:
	BOOL Begin(const int iLogType);
	BOOL Write(const TCHAR* szData);
	BOOL Write(const int32 nData);
	BOOL Write(const uint32 nData);
	BOOL Write(const int64 nData);
	BOOL Write(const uint64 nData);
	BOOL Write(const double nData);

public:
	void End();

protected:
	void Prepare();
	int GetPort();

	BOOL IsEnable();

protected:
	SOCKET m_hSocket;
	SOCKADDR_IN m_LogServer;

protected:
	typedef std::vector<std::string> SERVERS;
	
	SERVERS m_vServers;

	TCHAR m_szDelimiter[64];
	int32 m_iDelimiter;
	cBuffer m_Buffer;

	int m_iLogType;
	BOOL m_bEnable;
};