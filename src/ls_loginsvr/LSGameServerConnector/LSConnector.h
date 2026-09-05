#pragma once

class LSConnector : public CConnectNode
{
public:

	LSConnector( SOCKET s, DWORD dwSendBufSize, DWORD dwRecvBufSize );
	LSConnector();
	virtual ~LSConnector(void);

	void Init();
	void Destroy();
public:
	virtual void SessionClose( BOOL safely=TRUE );
	virtual bool SendMessage( CPacket &rkPacket );
	virtual void ReceivePacket( CPacket &packet );
	virtual void PacketParsing( CPacket &packet );
	void OnResponse( CPacket & packet );
	void OnClose( SP2Packet &packet );

public:
	virtual void OnCreate();       //√ ±‚»≠
	virtual void OnDestroy();
	virtual bool CheckNS( CPacket &rkPacket );	
	virtual int  GetConnectType();
	
public:
	bool ConnectTo(std::string serverIP, int& serverPort);

private:
	void OnPing();

public:
	unsigned int GetReferenceCount() const	{ return m_referenceCount; }
	void IncreaseReferenceCount()			{ m_referenceCount++; }
	void ResetReferenceCount( int value )	{ m_referenceCount = value; }

	void UpdateState( int value );
	void UpdateSockState( int value );

public:
	int	m_mineId;
	std::string m_ipAddr;
	int	m_port;
	int	m_sendServerId;

protected:
	unsigned int m_referenceCount;
};

