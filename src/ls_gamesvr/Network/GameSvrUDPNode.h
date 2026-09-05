#pragma once
 #include "../iocpSocketDLL/SocketModules/UDPNode.h"

class SP2Packet;
class GameSvrUDPNode : public UDPNode
{
public:
	GameSvrUDPNode(void);
	virtual ~GameSvrUDPNode(void);
	virtual int  GetConnectType();
	virtual void SessionClose( int index)  ;
	virtual void PacketParsing( CPacket &packet ) ;
	virtual void ReceivePacket( CPacket &packet, int index ) ;
	virtual BOOL SetNetworkSecurity(int i);

public:
	void SendPacket( UDPIoInfo* recvinfo, CPacket &packet );
	void MakeLogicPacket( SP2Packet& packet, sockaddr_in& addr);
	void SendLog(CPacket& packet);
	int MakeIpAddres( char* rcv_ip, UDPIoInfo* recvInfo, int& port );

 	long ProcessCount() {long rtval = m_processCount; InterlockedExchange(&m_processCount,0); return rtval;}
 	void IncrementProcessCount() {InterlockedIncrement(&m_processCount); }
	unsigned long Maxsize() const { return m_maxSize; }
	void SetMaxSize(unsigned long val)
	{
		if(m_maxSize < val)
			m_maxSize = val;
	}
	
public:
	SP2Packet* m_packet;
	unsigned long m_inputCount;
	unsigned long m_processCount;
	unsigned long m_sendPacketCount;
	unsigned long m_maxSize;
	
 
};
