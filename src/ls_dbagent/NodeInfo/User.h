// User.h: interface for the User class.
#pragma once

class CConnectNode;
class SP2Packet;

class User : public CConnectNode
{
	friend class UserNodeManager;

public:
	User( SOCKET s=INVALID_SOCKET, DWORD dwSendBufSize=0, DWORD dwRecvBufSize=0 );
	virtual ~User();

public:
	virtual void OnCreate();
	virtual void OnDestroy();	
	virtual int  GetConnectType(){ return 0; }

public:
	virtual void SessionClose( BOOL safely=TRUE );
	virtual void ReceivePacket( CPacket &packet );	
	virtual void PacketParsing( CPacket &packet );

private:
	void OnQuery( SP2Packet &packet );
	void OnClose( SP2Packet &packet );

public:
	BOOL SendPacket( SP2Packet &packet );
	void GetPeerIP();

	const char* GetIP()	{ return m_IP; }
	const int GetPort()	{ return m_port; }

protected:
	char m_IP[64];
	int m_port;
};
