#pragma once


#include "../iocpsocketdll\socketmodules\connectnode.h"
#include <boost/timer.hpp>

class MonitorNode :
	public CConnectNode
{
public:
	MonitorNode();
	MonitorNode( SOCKET s, DWORD dwSendBufSize, DWORD dwRecvBufSize );
	virtual ~MonitorNode(void);

public:
	virtual void SessionClose( BOOL safely=TRUE );
	void SendErrorCode(int errcode);
	virtual bool SendMessage( CPacket &rkPacket );
	virtual void ReceivePacket( CPacket &packet );
	virtual void PacketParsing( CPacket &packet );
	void OnFillDraw( int type );
	void OnSetServerBlock( SP2Packet& kPacket, int type ) ;
	void OnSetAllServerBlock( SP2Packet& kPacket, int type );
	void OnGetServerInfo();
	void OnFillServerInfo();
	void ResponseTiketInfo();
	void OnMonitor(SP2Packet& kPacket);
 
public: //get set
	int Currenttime() const { return m_currentTime; }
	void Currenttime(int val) { m_currentTime = val; }

public:
	void OnClose();
	virtual void OnCreate();       //√ ±‚»≠
	virtual void OnDestroy();
	virtual bool CheckNS( CPacket &rkPacket );	
	virtual void SetConnectType(int nodetype);
	virtual int  GetConnectType();
	void AVGPacket();

private:
	int m_currentTime;
	int m_nodeType;
};

