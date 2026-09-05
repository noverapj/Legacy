#ifndef __ThailandOTPNode_h__
#define __ThailandOTPNode_h__

#include "../ThailandLoginServer/ThailandUser.h"

class ThailandUser;
class CConnectNode;
class ThailandOTPNode : public CConnectNode 
{
public:
	enum SessionState
	{
		SS_DISCONNECT	= 0,
		SS_CONNECT		= 1,
	};

protected:
	SessionState m_eSessionState;
	DWORD        m_dwConnectTime;
	ThailandUser m_User;

public:
	virtual void SessionClose( BOOL safely=TRUE );
	virtual void DispatchReceive(CPacket& packet, DWORD bytesTransferred);
	virtual bool SendMessage( CPacket &rkPacket );
	virtual void ReceivePacket( CPacket &packet );
	virtual void PacketParsing( CPacket &packet );
	void InitData();

public:
	virtual void OnCreate();
	virtual void OnDestroy();
	virtual bool CheckNS( CPacket &rkPacket );	
	virtual int  GetConnectType();
	void SendUserMessage();

	bool IsDisconnectState(){ return ( m_eSessionState == SS_DISCONNECT ); }
	bool IsConnectState(){ return ( m_eSessionState == SS_CONNECT ); }

	DWORD GetConnectTime() const { return m_dwConnectTime; }
	ThailandUser &GetUser() { return m_User; }

protected:
	void OnOTP( SP2Packet &rkPacket );
	void OnClose( SP2Packet &rkPacket );

public:
	ThailandOTPNode();
	ThailandOTPNode( SOCKET s, DWORD dwSendBufSize, DWORD dwRecvBufSize );
	virtual ~ThailandOTPNode(void);
};

#endif // __ThailandOTPNode_h__