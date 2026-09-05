
#pragma once

class User : public CConnectNode
{
	friend class UserNodeManager;

	// 유저노드를 여러개 두지 않고 아래의 스테이트로 유저노드의 삭제를 처리한다. 즉 크리티컬 세션이 필요없어진다.
	enum SessionState
	{
		SS_DISCONNECT	= 0,
		SS_CONNECT		= 1,
	};

private:
	SessionState m_eSessionState;

private:
	DWORD	 m_sync_time;                   //동기화 시간
	DWORD    m_dwSyncCheckTime;             //유령소켓 해제 시간 120000 ~ 300000.

	DWORD        m_dwUserIndex;
	HANDLE       m_hFile;
	DWORD        m_dwFileCRC;
	ioHashString m_szFileName;
	DWORD        m_dwFileWidth;
	DWORD        m_dwFileHeight;
	int			 m_iUserState;
public:
	static bool m_bUseSecurity;
	static int  m_iSecurityOneSecRecv;
	
protected:
	void InitData();

public:
	virtual bool SendMessage( CPacket &rkPacket );
	virtual void ReceivePacket( CPacket &packet );
	virtual void PacketParsing(CPacket &packet);
	virtual void SessionClose( BOOL safely=TRUE );

public:
	virtual void OnCreate();                //초기화
	virtual void OnDestroy();
	void         OnSessionDestroy();       //정보 저장 & 초기화
	virtual bool CheckNS( CPacket &rkPacket );
	virtual int  GetConnectType();
			void OnClose();
	
public:
	void SetSessionState( User::SessionState eState ){ m_eSessionState = eState; }
	bool IsDisconnectState(){ return ( m_eSessionState == SS_DISCONNECT ); }
	bool IsConnectState(){ return ( m_eSessionState == SS_CONNECT ); }

	inline DWORD GetSyncCheckTime() const { return m_dwSyncCheckTime; }
	inline DWORD GetSyncTime() const { return m_sync_time; }

	DWORD GetUserIndex() const { return m_dwUserIndex; }

public:
	static void LoadHackCheckValue();

	// TCP
public:
	void OnFileWrite( SP2Packet &rkPacket );
	void OnAllFileWrite( SP2Packet &rkPacket );

private:
	bool IsExistFile( const char* fileName); /* 파일 존재 확인 */
	bool IsRightFile( DWORD dwFileWidth, DWORD dwFileHeight, const char *szFileName );

public:
	User( SOCKET s = INVALID_SOCKET, DWORD dwSendBufSize = 0, DWORD dwRecvBufSize = 0 );
	virtual ~User();
};
