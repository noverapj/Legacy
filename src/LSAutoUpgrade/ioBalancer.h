#pragma once

#include "ThreadModules/Thread.h"
#include "NetWork/SP2Packet.h"

class ioClientSocket;

enum BalancerStates
{
	BALANCER_STATE_NONE = 0,
	BALANCER_STATE_CONNECTING,
	BALANCER_STATE_CONNECTFAILED,
	BALANCER_STATE_CONNECTED,
	BALANCER_STATE_SUCCESS,
	BALANCER_STATE_CLOSE,
	BALANCER_STATE_ERROR,
	BALANCER_STATE_TRYING,
	BALANCER_STATE_END
};

class ioBalancer : public Thread 
{
public:
	ioBalancer(void);
	~ioBalancer(void);

	void Init();
	void Destroy();

protected:
	BOOL LoadINI();

	void SetState(const BalancerStates state)	{ m_state = state; }

protected:
	void Connect();
	void Disconnect();

	void GenerateID(TCHAR* IP, const int port);

	void SendLOG(const char* log, int number);
public:
	void Begin(TCHAR* INI);

	virtual void Run();
	virtual HWND GetHandle() = 0;

	LONG Evaluate(WPARAM wParam, LPARAM lParam);
	void RecvPacket( SP2Packet &rkPacket );

	BOOL IsFine()	{ return (BALANCER_STATE_SUCCESS == m_state) ? TRUE : FALSE; }
	BOOL IsError()  { return (BALANCER_STATE_ERROR == m_state) ? TRUE : FALSE; }
	void GetADDR(std::string& serverId);

	__int64 GetServerID() { return m_serverId; }

protected:
	TCHAR m_INI[512], m_serverIP[512];

	BalancerStates m_state;
	int m_serverPort;
	__int64 m_serverId;

	ioClientSocket* m_socket;
};

