#pragma once

#include "../ThreadModules/SuperParent.h"
#include "IOCP.h"
#include "Packet.h"
#include "SendIO.h"
#include "ReceiveIO.h"
#include "BufferPool.h"

/* 사용자가 꼭 정의 해야 할 함수 들 
	SessionClose
	PacketParsing
	ReceivePacket*/

class IOCP_SOCKET_API UDPNode : public SuperParent, public CCommandNode
{
public:
	UDPNode(void);
	virtual ~UDPNode(void);

public:
	virtual void InitMemory( int contextCount, int recvSize , int packetCount);
	virtual void InitMemory( int contextCount, int recvSize , int seed_64, int seed_128,int seed_256, int seed_1024, int seed_2048 );
	virtual void SetAddr(std::string ip,std::vector<int>& ports);
	virtual int GetConnectType() = 0;
	virtual void ReleaseMemoryPool();

public:
	virtual void ReceivePacket( CPacket &packet , int index)=0;
	virtual BOOL SendMessage( const char* ip, int port, CPacket& rkPacket );
	virtual BOOL SendMessageByDWORDIP( DWORD dwIp, int port, CPacket& rkPacket);
	virtual BOOL SendCurrentPortMessage( const char* ip, int port, int index, CPacket& rkPacket );
	virtual	BOOL SendMessage(const char*  ip,int port, const char* buffer, const int size, int index = -1);
	virtual	BOOL SendMessageByAddr(sockaddr_in& clientaddr,const char* buffer, const int size, int index = -1);
	SOCKET GetSendSocket( int index );
	IOBufferedContext* ReadyToSend( const int size, const char* buffer );
	void SetIptoSockAddr( sockaddr_in &clientaddr, const char* ip, int port );
	void CloseConnection(int index);
	void ExceptionClose(int lastError,int index);
	void SessionClose(int index){};
	BOOL WaitForPacketReceive(int index);

public:
	void SetNS( NetworkSecurity *pNS );
	void SetNS( NetworkSecurity *pNS, int index);
	void SetSockInfo(std::vector<SOCKINFOPAIR>& socketPairs,SOCKET& sendSocket);
	virtual BOOL CheckNS( CPacket &rkPacket, int index );
	virtual BOOL Dispatch( DWORD bytesTransferred, OVERLAPPED *ov, CPacket& packet );
	virtual BOOL SetNetworkSecurity(int i) = 0;
	virtual void DispatchReceive(CPacket& packet, DWORD bytesTransferred,int i);
	
public:
	const std::vector<int>* GetUDP_port();
	const char* GetPublicIP();
	int FindIndextoOv(WSAOVERLAPPED* ov);
 	void BufferFlush( IOBufferedContext* lpIOBufferedContext );
	char* GetBuffer(int size) { return m_bufferPool.Get(size); }
	void PushBuffer(char* buf,int size)	{ return m_bufferPool.Push(buf,size); }

public:
	int GetBufferPoolTotalCount()   { return m_bufferPool.GetBufferPoolTotalCount(); }
	int GetBuffer64PoolCount()		{ return m_bufferPool.GetBuffer64PoolCount(); }
	int GetBuffer128PoolCount()		{ return m_bufferPool.GetBuffer128PoolCount(); }
	int GetBuffer256PoolCount()		{ return m_bufferPool.GetBuffer256PoolCount(); }
	int GetBuffer1024PoolCount()	{ return m_bufferPool.GetBuffer1024PoolCount(); }
	int GetBuffer2048PoolCount()	{ return m_bufferPool.GetBuffer2048PoolCount(); }

	long GetMax64PopCount() const		{ return m_bufferPool.GetMax64PopCount(); }
	long GetMax128PopCount() const		{ return m_bufferPool.GetMax128PopCount(); }
	long GetMax256PopCount() const		{ return m_bufferPool.GetMax256PopCount(); }
	long GetMax1024PopCount() const		{ return m_bufferPool.GetMax1024PopCount(); }
	long GetMax2048PopCount() const		{ return m_bufferPool.GetMax2048PopCount(); }

	long Get64DropCount()			{ return m_bufferPool.Get64DropCount(); }
	long Get128DropCount()  		{ return m_bufferPool.Get128DropCount(); }
	long Get256DropCount()  		{ return m_bufferPool.Get256DropCount(); }
	long Get1024DropCount()  		{ return m_bufferPool.Get1024DropCount(); }
	long Get2048DropCount()			{ return m_bufferPool.Get2048DropCount(); }

	long Get64RemainderCount()		{ return m_bufferPool.Get64Remainder(); }
	long Get128RemainderCount()		{ return m_bufferPool.Get128Remainder(); }
	long Get256RemainderCount()		{ return m_bufferPool.Get256Remainder(); }
	long Get1024RemainderCount()	{ return m_bufferPool.Get1024Remainder(); }
	long Get2048RemainderCount()	{ return m_bufferPool.Get2048Remainder(); }

	void IncremnetContextpoolCount() { InterlockedIncrement(&m_contextpoolCount); }
	unsigned long GetContextpoolCount() { 
										  unsigned long val = m_contextpoolCount; 
										  InterlockedExchange(&m_contextpoolCount,0);
										  return val;	
										}

protected:
	typedef std::vector<UDPIoInfo*> UDPIoInfos;
	UDPIoInfos m_recvInfos;

	std::vector<int> m_iPorts;
	std::string	m_strIP;
	int m_iRecvSize;

protected:
	MemPooler <IOBufferedContext> m_contextPool;
	BufferPool m_bufferPool;

protected:
	NetworkSecurity *m_pNS;

protected:
	unsigned long m_contextpoolCount;
};


