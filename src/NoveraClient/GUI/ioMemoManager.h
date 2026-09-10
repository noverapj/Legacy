#pragma once

class SP2Packet;

class MemoNode
{
protected:
	ioHashString m_UserID;
	ioHashString m_Memo;
	DWORD        m_dwMemoTime;
	bool         m_bViewMemo;
	bool         m_bOnline;
	bool         m_bFromMemo;

public:
	const ioHashString &GetUserID(){ return m_UserID; }
	const ioHashString &GetMemo(){ return m_Memo; }
	DWORD GetTime(){ return m_dwMemoTime; }
	
	bool  IsOnline(){ return m_bOnline; }
	void  SetOnline( bool bOnline ){ m_bOnline = bOnline; }

	bool  IsViewMemo(){ return m_bViewMemo; }
	void  SetViewMemo( bool bView ){ m_bViewMemo = bView; }

	bool  IsFromMemo(){ return m_bFromMemo; }

public:
	MemoNode( ioHashString UserID, ioHashString Memo, DWORD dwTime, bool bOnline, bool bFromMemo );
	virtual ~MemoNode();
};

class MemoNodeSort : public std::binary_function< MemoNode*, MemoNode*, bool >
{
public:
	bool operator()( MemoNode *lhs , MemoNode *rhs ) const
	{
		if( lhs->GetTime() > rhs->GetTime() )
		{
			return true;
		}
		else if( lhs->GetTime() == rhs->GetTime() )
		{
			int iCmpValue = stricmp( lhs->GetUserID().c_str(), rhs->GetUserID().c_str() );     
			if( iCmpValue < 0 )
				return true;
		}
		return false;
	}
};
typedef std::vector< MemoNode* > vMemoNode;
typedef vMemoNode::iterator vMemoNode_iter;

class UserLoginCheck
{
protected:
	struct UserLogin
	{
		ioHashString m_szUserID;
		DWORD        m_dwSendTimer;
		UserLogin()
		{
			m_dwSendTimer = 0;
		}
	};
	typedef std::vector< UserLogin > vUserLogin;
	typedef vUserLogin::iterator vUserLogin_iter;
	vUserLogin m_vList;	
    
public:
	void SendUserLogin( ioHashString szUserID );
	void RecvUserLogin( ioHashString szUserID );

	void ChangeName( const ioHashString &rszUserID, const ioHashString &rszNewUserID );

public:
	UserLoginCheck();
	virtual ~UserLoginCheck();
};

class ioMemoManager : public Singleton< ioMemoManager >
{
	public:
		enum
		{
			MAX_RECV_MEMO_SIZE = 50,
			OFFLINE_MEMO_SEND_TIME = 500,
		};
	protected:
	vMemoNode            m_vMemoNode;
	UserLoginCheck       m_UserLoginList;
	bool                 m_bUpdate;
	int                  m_iServerArray;
	DWORD                m_dwProcessOfflineSendTime;
	typedef std::vector< SP2Packet > SP2PacketList;
	SP2PacketList        m_SendOfflineMemoList;
		
	public:
	void RemoveNode( MemoNode *node );
	
	public:
	int GetNodeSize(){ return m_vMemoNode.size(); }
	int GetOnlineMemoCount();
	int GetNotViewMemoCount();
	MemoNode *GetNode( int iArray );

	public:
	void ShowMemoPopup( MemoNode *node );

	public:
	bool IsLogIn( ioHashString szName );
	bool IsUpdate(){ return m_bUpdate; }
	void ClearUpdate(){ m_bUpdate = false; }

	public:
	void OnLogInUser( ioHashString szName );
	void OnLogOutUser( ioHashString szName );

	public:
	void SendOfflineMemo();
	void SendUserLogin( ioHashString szName );
	void SendMemo( ioHashString szFromID, ioHashString szMemo );

	public:
	void OnlineMemo( SP2Packet &rkPacket );
	void OfflineMemo( SP2Packet &rkPacket );

	public:
	void ChangeName( const ioHashString &rszUserID, const ioHashString &rszNewUserID );

	public:
	void ProcessMemo();

	public:
	static ioMemoManager &GetSingleton();
		
	public:
	ioMemoManager();
	virtual ~ioMemoManager();	
};
#define g_MemoManager ioMemoManager::GetSingleton()
