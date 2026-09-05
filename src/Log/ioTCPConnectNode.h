#pragma once


#include <WinSock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <string>
#include <vector>
#include "LOGPacket.h"
#include "ioDefine.h"
#include "../include/MemPooler.h"
#include "ioLogReceiveIO.h"

class CLogData;
class CLog;

class ioTCPConnectNode
{
public:
	ioTCPConnectNode(void);
	virtual ~ioTCPConnectNode(void);

public:
	void InitData();
	void InitMemoryPool();
	int InitSocket( int port );

public:
	void SetServerIP( const char* ipAddr, int port );
	void SetReserveData( CLogData* logData );

public:
	int ReadyToSocket( int port, int makeNonBlocking );
	int CreateSocket();
	int BindSocket( int port );
	int MakeSocketNonBlocking( SOCKET sock );
	void SetSocketOption( const char* ipAddress, const int port ,int timeOut, fd_set& set, timeval& tvout);


public:
	BOOL Connect( const char* ipAddr,const int port) ;
	BOOL AfterCreate();
	int WaitForReceive();
	void SessionClose( int errCode = 0 );
	void ReadyToReceive();
	int SendMessage( const char* buf, const int size );
	int SendMessage( LOGPacket& pk);
	int SendMessage( CLogData* logData );

public:
	int DispatchReceive( DWORD bytesTransferred );
	void PacketParsing( LOGBufferedContext* operationContext );

	void ParseReceivePacket();

	void OnClose();

public:
	int ExceptionSend( CLogData* logData );
	void SendLog( LOGBufferedContext* operationContext );
	void RequestConnect( LOGBufferedContext* operationContext );
	void SendOnConnect();

public:
	void ProcessReserveData();
	void SetLoggerPacket( CLogData* logData, LOGPacket& kPacket );
	
public:
	LOGBufferedContext* GetOveralpped() { LOGBufferedContext* ov = m_contextPool.Pop(); ov->Init(); return ov;}
	void PushQueue( LOGBufferedContext* operationContext, int flag );
	void PushOperation( int flag );
	void PushOperation( int flag, char* daemoneName, int port, int svrPort, char* ipAddr, ioLogger* log=NULL );
	void PushOperation( CLogData* logData );

protected:
	void PrintTimeAndLog(LPSTR fmt,...);

public: //get/set
	const int GetSockState()				{ return m_sockState; }
	void SetSockState(int val)				{ m_sockState = val; }
	int GetPort() const						{ return m_port; }
	void SetPort(int val)					{ m_port = val; }
	int SetSvrPort() const					{ return m_svrPort; }
	void SetSvrPort(int val)				{ m_svrPort = val; }
	const char* SvrIpAddress() const		{ return m_ServerIP; }
	void SetSvrIpAddress(const char* val)			{ strcpy_s(m_ServerIP,val); }
	void SetLogInstance(ioLogger* log)		{ m_logger = log; }
	int GetCategoryIndex()					{ return ++m_categoryIndex; }
	SOCKET GetSocketHandle() const			{ return m_sock; }
	BOOL GetPrintState() const				{ return m_printState; }
	void SetPrintState(BOOL val)			{ m_printState = val; }

protected:
	MemPooler<LOGBufferedContext> m_contextPool;
	//MemPooler<LOGMessage> m_logMessagePool;

protected: //내 정보 
	sockaddr_in m_addr;
	char m_daemonName[MAX_DAMONE_NAME_SIZE];
	int m_port;
	BOOL m_printState;
	SOCKET m_sock; 
	int m_sockState;
	ioLogger* m_logger;
	LOGPacket m_packet;

protected:
	sockaddr_in m_svrAddr;
	ioLogReceiveIO m_recvIO;
	char m_ServerIP[16];
	int m_svrPort;
	//LOGBufferedContext m_receiveOV;
	std::vector<ReserveLogData> m_reserveFiles;
	int m_categoryIndex;

public:
	LOGMessage m_logMessage;// 전성용

};

#define g_TCPNode cSingleton<ioTCPConnectNode>::GetInstance()

