#pragma once


class GameServerNode : public CConnectNode
{
public:
	GameServerNode();
	GameServerNode( SOCKET s, DWORD dwSendBufSize, DWORD dwRecvBufSize );
	virtual ~GameServerNode(void);

public:
	void InitData();
	void ReleaseData();

public:
	bool ConnectTo(std::string serverIP, int& serverPort);

	virtual void OnCreate();       //초기화
	virtual bool AfterCreate();
	virtual void OnDestroy();
	virtual void SessionClose( BOOL safely=TRUE );
	virtual bool SendMessage( CPacket &rkPacket );
	virtual void ReceivePacket( CPacket &packet );
	virtual void PacketParsing( CPacket &packet );

	virtual bool CheckNS( CPacket &rkPacket );	
	virtual int  GetConnectType();

protected:
	void OnConnect( SP2Packet & kPacket );
	void OnClose( SP2Packet &packet );
	void OnRelayControl( SP2Packet & kPacket );
	void AddUser( SP2Packet & kPacket );
	void OnDelUser( SP2Packet & kPacket );
	void OnChangeNickName( SP2Packet & kPacket );

public:
	void SendOnAddUser( char * publicID );
	void ChangeUserAddr(const char* ipAddr, int port, DWORD userIndex);
	bool SetServerAddress( std::string &serverIP, int& serverPort );

public:

	RelayGroup* InitRelayGroup();
	//BOOL SendRelayPacket(DWORD dwUserIndex,SP2Packet& sppacket);
	RelayGroup* GetRelayGroupByRoom( DWORD dwRoomIndex ); //여기부터 옮길것
	RelayGroup*  GetRelayGroupByUser( DWORD dwUserIndex );
	RelayGroup* CreateRelayGroup();


protected:
	void InsertRoom( SendRelayInsertData& inData );  
	void RemoveRoom( RemoveData& rmData );
	void RemoveRelayGroup( DWORD dwRoomIndex );

public://get/set 
	bool WholeChatState() const { return m_wholeChatState; }
	void WholeChatState(bool val) { m_wholeChatState = val; }
	int ServerIndex() const { return m_serverIndex; }
	void ServerIndex(int val) { m_serverIndex = val; }
	int ServerPort() const { return m_port; }
	void SetServerPort(int val) { m_port = val; }
	std::string ServerAddress() const { return m_ipAddr; }
	void SetAddress(std::string val) { m_ipAddr = val; }
	int SendServerId() const { return m_sendServerId; }
	void SetSendServerId(int val) { m_sendServerId = val; }
	int GetRoomCount() { return m_relayGroups.GetCount(); }

protected:
	std::string m_ipAddr;
	int		m_port;
	int		m_sendServerId;
	sockaddr_in m_serverAddress;

protected:
	bool	m_wholeChatState;
	int		m_mineId;
	int		m_maxuserCount;
	int		m_currentUserCount;
	DWORD	m_currentTime;	
	int		m_serverIndex;// Relayserver에서 바바론 서버 인덱스 
	int		m_relayServerIndex; // Gameserver에서 바라본 릴레이서버 인덱스 
	int		m_roomMaxCount;

protected:
	MemPooler<RelayGroup>		m_relayGroupPool; //메모리풀 확인 완료 
	ATL::CAtlList<RelayGroup*>	m_relayGroups;
};

