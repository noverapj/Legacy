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

struct GAMESERVERINFO
{
	DWORD	dwGlobalTime;			//GLOBAL TIME
	char szPublicIP[ 32 ];		//Network Info
	int	csPort;
	int SSPort;
	int MSPort;
	int ThreadCount;				//Thread Info
	int NodeSize;					//Connect Client Info
	int CopyNodeSize;
	int RemainderNode;				//Remainder MemPool Info
	int RoomNodeSize;				//CREATE ROOM 
	int PlazaNodeSize;
	int HeapQuartersNodeSize;
	int CopyRoomNodeSize;
	int CopyPlazaNodeSize;
	int CopyHeapQuartersNodeSize;
	int RommRemainderNode;			//Remainder MemPool Info
	int BattleRoomNodeSize;			//CREATE BATTLEROOM
	int BattleRoomCopyNodeSize;
	int PartyLevelCheckMinRoom;
	int BattleRoomRemainderNode;	//Remainder MemPool Info
	int LadderTeamNodeSize;			//CREATE LADDERTEAM
	int LadderTeamCopyNodeSize;
	int LadderTeamCampBattlePlay;
	int LadderTeamRemainderNode;	//Remainder MemPool Info
	int RecvQueueNodeSize[ 4 ];			//RECV QUEUE
	int BroadCastUDPnRelayNodeSize;
	int RecvQueueRemainderNodeSize[4];	//Remainder MemPool Info
	int BroadCastUDPnRelayRemainderNodeSize;
	int DBClientNodeSize;				//DB AGENT SERVER INFO
	int DBClientTotalNodeSize;				//DB AGENT SERVER INFO
	char szSTRFILEVER[ 8 ];				// Game Server Version
	char szSTRINTERNALNAME[ 32 ];
	char szGameServerID[ 32 ];			// Game Server ID
	char szGameServerName[ 32 ];		// Game Server Name
	char szLogDBIP[ 32 ];				// LogDB Agent IP / port
	int LogDBPort;
	bool IsLogDBAActive;
	DWORD HackCheckMin;					// HackCheck
	DWORD HackCheckMax;
	int HackCheckLess;
	int HackCheckOver;
	int HackCheckLessOver;
	int HackCheckTotal;
	int IsClientVersion;				// Client Version
	int GetClientVersion;
	int ChannelNodeSize;				//CREATE Channel
	int ChannelRemainderSize;
	int ChannelCopyNodeSize;
	int MemoNodeSize;					//MEMO
	DWORD GetServerIndex;				//GAME SERVER INFO
	int ServerNodeSize;
	int ServerRemainderNodeSize;
	bool IsMainServerActive;			// Main Server Info
	char MainServerIP[ 32 ];
	int MainServerPort;
	__int64 UDPTransferCount;			//UDP Transfer Count
	__int64 UDPTransferTCPCount;
	__int64 UDPTransferTCPSendCount;
	bool IsBillingRelayServerActive;	//BILLING RELAY SERVER INFO
	char BillingIP[ 32 ];
	int BillingPort;
	char XtrapVersion[ 32 ];
	int LicenseDate;					// Expiration dates
	bool m_bReserveLogout;				// Exit
	int remainSecond;
	int sendBufferUsingCnt;				// SendBuffer
	int sendBufferRemainCnt;
	int sendBufferMaxCnt;

	// Log
	int usingLogCount;					// 현재
	int maxUsingLogCount;				// 가장 많이 쓸때
	int remainLogCount;					// remain
	int dropLogCount;					// 드랍된 갯수.

	GAMESERVERINFO() : dwGlobalTime(0), csPort(0), SSPort(0), MSPort(0), ThreadCount(0), NodeSize(0), CopyNodeSize(0)
		, RemainderNode(0),	RoomNodeSize(0), PlazaNodeSize(0), HeapQuartersNodeSize(0),	CopyRoomNodeSize(0), CopyPlazaNodeSize(0)
		, CopyHeapQuartersNodeSize(0), RommRemainderNode(0), BattleRoomNodeSize(0),	BattleRoomCopyNodeSize(0), PartyLevelCheckMinRoom(0)
		, BattleRoomRemainderNode(0), LadderTeamNodeSize(0), LadderTeamCopyNodeSize(0),	LadderTeamCampBattlePlay(0), LadderTeamRemainderNode(0)
		, BroadCastUDPnRelayNodeSize(0), BroadCastUDPnRelayRemainderNodeSize(0), DBClientNodeSize(0), DBClientTotalNodeSize(0)
		, LogDBPort(0), IsLogDBAActive(false), HackCheckMin(0), HackCheckMax(0), HackCheckLess(0), HackCheckOver(0), HackCheckLessOver(0), HackCheckTotal(0), IsClientVersion(0)
		, GetClientVersion(0), ChannelNodeSize(0), ChannelRemainderSize(0), ChannelCopyNodeSize(0), MemoNodeSize(0)
		, GetServerIndex(0), ServerNodeSize(0), ServerRemainderNodeSize(0), IsMainServerActive(false), MainServerPort(0)
		, UDPTransferCount(0), UDPTransferTCPCount(0), UDPTransferTCPSendCount(0), IsBillingRelayServerActive(false), BillingPort(0)
		, LicenseDate(0), m_bReserveLogout(false), remainSecond(0), sendBufferUsingCnt(0), sendBufferRemainCnt(0), sendBufferMaxCnt(0)
		, usingLogCount(0), maxUsingLogCount(0), remainLogCount(0), dropLogCount(0)
	{
		ZeroMemory( szPublicIP, sizeof( szPublicIP ) );
		ZeroMemory( RecvQueueNodeSize, sizeof( RecvQueueNodeSize ) );
		ZeroMemory( RecvQueueRemainderNodeSize, sizeof( RecvQueueRemainderNodeSize ) );
		ZeroMemory( szSTRFILEVER, sizeof( szSTRFILEVER ) );
		ZeroMemory( szSTRINTERNALNAME, sizeof( szSTRINTERNALNAME ) );
		ZeroMemory( szGameServerID, sizeof( szGameServerID ) );
		ZeroMemory( szGameServerName, sizeof( szGameServerName ) );
		ZeroMemory( szLogDBIP, sizeof( szLogDBIP ) );
		ZeroMemory( MainServerIP, sizeof( MainServerIP ) );
		ZeroMemory( BillingIP, sizeof( BillingIP ) );
		ZeroMemory( XtrapVersion, sizeof( XtrapVersion ) );
	}
};


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
	MonitoringNode( SOCKET s = INVALID_SOCKET, DWORD dwSendBufSize = 0, DWORD dwRecvBufSize = 0 );
	virtual ~MonitoringNode(void);
};

#endif // __MonitoringNode_h__