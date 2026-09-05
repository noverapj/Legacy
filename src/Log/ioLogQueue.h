#pragma once


#include "ioQueue.h"

class CLogData;
class ioTCPConnectNode;

class ioLogQueue : public ioQueue
{
public:
	ioLogQueue(void)			{ Init(); }
	virtual ~ioLogQueue(void)	{ Destroy(); }

private:
	void Init();
	void Destroy();

public:
	void Startup();
	void Parse();
	void ParseTcpLogData( LPOVERLAPPED overlapped, DWORD bytes, int& retVal, ioTCPConnectNode* tcpNode );
	void ParseLogData( CLogData* logData, DWORD bytes);
};

#define g_logQueue cSingleton<ioLogQueue>::GetInstance()
