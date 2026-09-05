#pragma once

#include "NetWork/ioSP2TCPSocket.h"

#define MAX_TCPSOCKET_MAP 100

class TCPSocketTable
{
protected:
	static TCPSocketTable *sg_Instance;
public:
	static TCPSocketTable &GetInstance();
	static void ReleaseInstance();

protected:
	typedef std::map<UINT, ioSP2TCPSocket*> TCPSocketMap;
	TCPSocketMap m_TCPSocketMap;

public:
	bool Init();
	void Close();

	bool AddAndInit(HWND hWnd, vZONEINFO &rvConnectInfo);
	bool AddLoginServer(HWND hWnd,vSERVERINFO &vConnectInfo);
	bool CheckAllClose();
	void Reconnect(HWND hWnd);
	void CloseAndRelease();
	bool Send(UINT wmdMsg, CPacket &rkPacket );
	int GetSize();

	ioSP2TCPSocket* GetTCPSocket(UINT wmdMsg);

private: /* Singleton Class */
	TCPSocketTable(void);
	virtual ~TCPSocketTable(void);
	HWND m_hWnd;
};

#define g_TCPSocketTable TCPSocketTable::GetInstance()