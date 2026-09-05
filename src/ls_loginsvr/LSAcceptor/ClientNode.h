#pragma once


#include "../iocpsocketdll\socketmodules\connectnode.h"
#include <boost/timer.hpp>

class ClientNode : public CConnectNode
{
public:
	ClientNode( SOCKET s, DWORD dwSendBufSize, DWORD dwRecvBufSize );
	ClientNode();
	virtual ~ClientNode(void);

public:
	virtual void OnCreate();       //√ ±‚»≠
	virtual void OnDestroy();
	virtual bool CheckNS( CPacket &rkPacket );	
	virtual void SetConnectType(int nodetype) { m_nodeType = nodetype; }
	virtual int  GetConnectType() { return m_nodeType; }

public:
	virtual void SessionClose( BOOL safely=TRUE );
	virtual bool SendMessage( CPacket &rkPacket );
	virtual void ReceivePacket( CPacket &packet );
	virtual void PacketParsing( CPacket &packet );

public:
	void OnClient(SP2Packet & kPacket);
	void OnClose();
	void OnMonitor(SP2Packet& kPacket);
	void OnControlServer(SP2Packet& kPacket); 
	void OnResponseTicketInfo();
	void OnFillDraw( int type );
	void OnSetServerBlock( SP2Packet& kPacket, int type ) ;
	void OnSetAllServerBlock( SP2Packet& kPacket, int type );
	void OnGetServerInfo();
	void OnFillServerInfo();
	void OnWhiteList(SP2Packet& kPacket);
	void SendErrorCode(int errcode);

protected:
	BOOL RestrictIP();



public: //get set
	int IsGhost() const			{ return m_isGhost; }
	void IsGhost(int val)		{ m_isGhost = val; }
	int Currenttime() const		{ return m_currentTime; }
	void Currenttime(int val)	{ m_currentTime = val; }
	int NWaitNumber() const { return m_waitNumber; }
	void NWaitNumber(int val) { m_waitNumber = val; }
	void AVGPacket();

private:
	int m_currentTime;
	int m_waitNumber;
	int m_isGhost;
	int m_nodeType;
};

