#ifndef __MonitoringNode_h__
#define __MonitoringNode_h__

// WGMS_연동_프로토콜.doc 참고
#define MONITORING_STATUS_CMD 9002
#define MONITORING_CHANGE_CMD 9003

#pragma pack(1) 

typedef struct tagMonitorHeader
{
	unsigned short m_usCommand;
	unsigned short m_usSize;

	tagMonitorHeader()
	{
		m_usCommand = 0;
		m_usSize    = 0;
	}

} MonitorHeader;

// Manage Server -> Game Server
typedef struct tagMonitorStatusRequest
{
	MonitorHeader m_Header;

	tagMonitorStatusRequest() 
	{
		m_Header.m_usCommand = MONITORING_STATUS_CMD;
		m_Header.m_usSize    = sizeof(MonitorStatusRequest);
	}
} MonitorStatusRequest;

// Game Server -> Manage Server
typedef struct tagMonitorStatusResult
{	
	enum 
	{ 
		STATUS_READY   = 2, 
		STATUS_RUN     = 3, 
		STATUS_BLOCK   = 4, 
		STATUS_EXITING = 5, 
		STATUS_EXIT    = 6,  
	};

	MonitorHeader m_Header;

	char m_cStatus;	    // 2:동작준비, 3:동작중, 4:추가접속제한상태, 5:종료준비, 6:종료완료
	int  m_iCuCount;	// 동접

	tagMonitorStatusResult() 
	{
		m_Header.m_usCommand = MONITORING_STATUS_CMD;
		m_Header.m_usSize    = sizeof(MonitorStatusResult);
		m_cStatus            = NULL;
		m_iCuCount           = 0;
	}
} MonitorStatusResult;

// Manage Server -> Game Server
typedef struct tagMonitorChangeRequest
{
	enum 
	{ 
		CHANGE_OPEN   = 1, 
		CHANGE_BLOCK  = 2, 
		CHANGE_EXIT   = 3,  
	};

	MonitorHeader m_Header;
	int m_iReqStatus; // 1:오픈 요청, 2:추가 접속 제한 요청, 3:셧다운

	tagMonitorChangeRequest() 
	{
		m_Header.m_usCommand = MONITORING_CHANGE_CMD;
		m_Header.m_usSize    = sizeof(MonitorChangeRequest);
		m_iReqStatus         = CHANGE_OPEN;
	}
} MonitorChangeRequest;

// Game Server -> Manage Server
typedef struct tagMonitorChangeResult
{
	enum 
	{ 
		CHANGE_SUCCESS   = 1, 
		CHANGE_FAIL      = 2, 
	};

	MonitorHeader m_Header;
	int m_iResult;	       // 1 : 성공, 0 : 실패

	tagMonitorChangeResult()
	{
		m_Header.m_usCommand = MONITORING_CHANGE_CMD;
		m_Header.m_usSize    = sizeof(MonitorChangeResult);
		m_iResult            = CHANGE_SUCCESS;
	}
} MonitorChangeResult;

#pragma pack()
//

class CConnectNode;
class SP2Packet;

class MonitoringNode : public CConnectNode
{
	friend class MonitoringNodeManager;

public:
	enum SessionState
	{
		SS_DISCONNECT	= 0,
		SS_CONNECT		= 1,
	};

protected:
	SessionState m_eSessionState;
	int m_iLogCnt;

public:
	virtual void SessionClose( BOOL safely=TRUE );
	virtual bool SendMessage( CPacket &rkPacket );
	virtual void ReceivePacket( CPacket &packet );
	virtual void PacketParsing( CPacket &packet );

public:
	void SetSessionState( MonitoringNode::SessionState eState ){ m_eSessionState = eState; }
	bool IsDisconnectState(){ return ( m_eSessionState == SS_DISCONNECT ); }
	bool IsConnectState(){ return ( m_eSessionState == SS_CONNECT ); }

public:
	virtual void OnCreate();       //초기화
	virtual void OnDestroy();
	void OnSessionDestroy();
	virtual bool CheckNS( CPacket &rkPacket );	
	virtual int  GetConnectType();

protected:
	void InitData();
	virtual void DispatchReceive( CPacket& packet, DWORD bytesTransferred );

public:
	void OnClose( SP2Packet &rkPacket );
	void OnStatus( SP2Packet &rkPacket );
	void OnChange( SP2Packet &rkPacket );

public:
	MonitoringNode();
	MonitoringNode( SOCKET s, DWORD dwSendBufSize, DWORD dwRecvBufSize );
	virtual ~MonitoringNode(void);
};

#endif // __MonitoringNode_h__