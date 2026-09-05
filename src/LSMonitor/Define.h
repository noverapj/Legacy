#ifndef ___DEFINE_H__
#define ___DEFINE_H__

#include "Local\ioLocalManager.h"

#define COMPARE(x,min,max) (((x)>=(min))&&((x)<(max)))
#define SAFEDELETE(x)		if(x != NULL) { delete x; x = NULL; }
#define SAFEDELETEARRAY(x)	if(x != NULL) { delete [] x; x = NULL; }

struct Vector3
{
	float x,y,z;

	Vector3(){}
	Vector3( float _x, float _y, float _z )
	{
		x = _x;
		y = _y;
		z = _z;
	}
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define MAIN_TIMER 1
#define LS_SERVER 2
#define REQ_SVRINFO 3

#define NO_REACTION_TIME        60000
#define NO_REACTION_TIME_LONG   180000

#define STATE_STARTING         "STARTING"
#define STATE_NORMAL           "NORMAL"
#define STATE_NO_REACTION      "NO REACTION"
#define STATE_STOP_MONITOR     "STOP MONITOR"
#define STATE_HIGH_CONNECT     "HIGH CONNECT"
#define STATE_TOO_HIGH_CONNECT "TOO HIGH CONNECT"
#define STATE_MAXIMUM_CONNECT  "MAXIMUM CONNECT"
#define STATE_LOW_MEMORY	   "LOW MEMORY"
#define STATE_LOW_CDRIVE       "LOW C DRIVE"
#define STATE_LOW_DDRIVE       "LOW D DRIVE"
#define STATE_HIGH_CPU         "HIGH CPU"
#define STATE_TOO_HIGH_CPU     "TOO HIGH CPU"
#define STATE_MAXIMUM_CPU      "MAXIMUM CPU"

// server list
#define MAXIMUM_CONNECT_PER    0.95f
#define TOO_HIGH_CONNECT_PER   0.7f
#define HIGH_CONNECT           0.5f

#define LIST_ITEM_NAME			0
#define LIST_ITEM_IP			1
#define LIST_ITEM_PORT			2
#define LIST_ITEM_CONNECT		3
#define LIST_ITEM_ROOMCNT		4
#define LIST_ITEM_PLAZACNT      5
#define LIST_ITEM_BATTLEROOMCNT 6
#define LIST_ITEM_PINGMS        7 
#define LIST_ITEM_DBMS          8
#define LIST_ITEM_STATE			9

#define RGB_BLACK           RGB(0,0,0)
#define RGB_GRAY            RGB(238, 236, 222)
#define RGB_WHITE           RGB(255,255,255)
#define RGB_RED             RGB(255, 0, 0)
#define RGB_LIGHT_RED       RGB(255, 100, 100)
#define RGB_MORE_LIGHT_RED  RGB(255, 222, 227)
#define RGB_LIGHT_GREEN     RGB(0, 155, 0)

// system list
#define LIST_SYSTEM_ITEM_IP       0
#define LIST_SYSTEM_ITEM_NAME     1
#define LIST_SYSTEM_ITEM_CPU      2
#define LIST_SYSTEM_ITEM_MEMORY   3
#define LIST_SYSTEM_ITEM_C_MEMORY 4
#define LIST_SYSTEM_ITEM_CDRIVE   5
#define LIST_SYSTEM_ITEM_DDRIVE   6
#define LIST_SYSTEM_ITEM_STATE    7

#define DONT_HAVE_DDRIVE   -1
#define DONT_HAVE_DDRIVE_STRING "NONE"
namespace ESOCKET
{
	enum Socket
	{
		CONNECTED = 0,
		DISCONNECTED,
		ACCEPT = 0x1001,
		CLOSE,
		READ,
		SEND,
		LS_REQSERVERSTATE,
		LS_RESPSERVERSTATE,
		LS_ZOMBIESERVERSTATE,

	};
}


enum ServerType
{
	eServerType_Default		= 0,
	eServerType_MainServer	= 1,
	eServerType_GameServer	= 2,
	eServerType_LoginServer = 3,
	eServerType_LoginPanel  = 4,
	eServerType_LoginServerInfo = 5,
};

typedef struct tagSERVERINFO
{
	WORD   wID;
	CString szName;
	CString szIP;
	int		iPort;
	WORD	wConnect;
	WORD	wRoomCount;
	WORD    wPlazaCount;
	WORD	wBattleRoomCount;
	WORD 	wPingMS;
	WORD    wDBQueryMs;
	DWORD	dwLastRcvTime;
	WORD    wListCtrlArray;
	WORD    wSMSArray;
	bool    bReaction;
	int		serverType;
	int     nzoneid;
	int     nstate;
	int     nwmid;

	tagSERVERINFO()
	{
		Init();
	}

	void	Init()
	{
		nwmid = 0;
		nstate = -1;
		nzoneid = 0;
		wID              = 0,
		szName	         = "";
		szIP	         = "";
		iPort	         = 0;
		wConnect         = 0;
		wRoomCount	     = 0;
		wPlazaCount      = 0;
		wBattleRoomCount = 0;
		wPingMS          = 0;
		wDBQueryMs       = 0;
		dwLastRcvTime    = timeGetTime();
		wListCtrlArray   = 0;
		wSMSArray        = 0;
		bReaction        = true;
		serverType		= eServerType_Default;
	}
}SERVERINFO;
typedef std::vector<SERVERINFO> vSERVERINFO;

typedef struct tagSYSTEMINFO
{
	CString  szIP;
	CString  szName;
	CString  szURL;
	int      iCPUUsed;
	int      iMemoryRemain;
	double   dbMemoryUsed;
	int      iCDriveRemain;
	int      iDDreiveRemain;
	DWORD    dwLastRcvTime;
	WORD     wListCtrlArray;
	WORD     wSMSArray;
	bool     bReaction;
	bool     bControl;

	tagSYSTEMINFO()
	{
		szIP           = "";
		szName         = "";
		szURL          = "";
		iCPUUsed       = 0;
		iMemoryRemain  = 0;
		dbMemoryUsed   = 0;
		iCDriveRemain  = 0;
		iDDreiveRemain = 0;
		dwLastRcvTime  = timeGetTime();
		wListCtrlArray = 0;
		wSMSArray      = 0;
		bReaction      = true;
	}

}SYSTEMINFO;
typedef std::vector<SYSTEMINFO> vSYSTEMINFO;

typedef struct tagZONEINFO
{
	int			gradeType;
	CString		szName;
	CString		szIP;
	int			iPort;
	ioLocalManager::LocalType eLocalType;
	vSERVERINFO vServerInfo;
	vSYSTEMINFO vSystemInfo;
	int         iCurrentTotalCount;
	int			maxTotalCount;

	tagZONEINFO()
	{
		gradeType = 0;
		szName = "";
		szIP   = "";
		iPort  = 0;
		eLocalType = ioLocalManager::LCT_KOREA;
		iCurrentTotalCount = 0;
		maxTotalCount = 0;
	}
	~tagZONEINFO()
	{
		vServerInfo.clear();
	}
}ZONEINFO;
typedef std::vector<ZONEINFO> vZONEINFO;



struct MAINSERVERINFO
{
	DWORD	dwGlobalTime;			//GLOBAL TIME
	char	szPublicIP[ 32 ];		//Network Info
	char	szPrivateIP[ 32 ];
	int		iPort;
	int		ThreadCount;			//Thread Info
	int		JoinServerCount;		//Connect Client Info
	int		RemainderMemPoolCount;	//Remainder MemPool Info
	int		RecvQueuePacketCount[4];//RECV QUEUE
	int		RecvQueueRemainderCount[4];//Remainder MemPool Info
	char	szDBAgentIP[ 32 ];		//DB AGENT SERVER INFO
	int		DBAgentPort;
	int		MaxGuildCount;			//GUILD INFO
	int		MaxUpdateGuild;
	char	szCampStringHelp[ 128 ];//CAMP INFO
	int		MaxTradeItemCount;		//Trade Info
	char	szEventShopState[ 32 ];	//Event Shop Info
	int		EventGoodsSaveDataCount;
	int		MaxToolConnectCount;	//MANAGER TOOL
	bool	bUseClientVersion;		// Client Version
	int		iClientVersion;
	char	szMainServerVersion[ 8 ];// Main Server Version
	char	szMainServerName[ 32 ];

	// Log
	int usingLogCount;					// 현재
	int maxUsingLogCount;				// 가장 많이 쓸때
	int remainLogCount;					// remain
	int32 dropLogCount;					// 드랍된 갯수.

	MAINSERVERINFO() : dwGlobalTime(0), iPort(0), ThreadCount(0), JoinServerCount(0), RemainderMemPoolCount(0)
		, DBAgentPort(0), MaxGuildCount(0)
		, MaxUpdateGuild(0), MaxTradeItemCount(0), EventGoodsSaveDataCount(0), MaxToolConnectCount(0)
		, bUseClientVersion(false), iClientVersion(0), usingLogCount(0), maxUsingLogCount(0), remainLogCount(0), dropLogCount(0)
	{
		   ZeroMemory( szPublicIP, sizeof( szPublicIP ) );
		   ZeroMemory( szPrivateIP, sizeof( szPrivateIP ) );
		   ZeroMemory( RecvQueuePacketCount, sizeof( RecvQueuePacketCount ) );
		   ZeroMemory( RecvQueueRemainderCount, sizeof( RecvQueueRemainderCount ) );
		   ZeroMemory( szDBAgentIP, sizeof( szDBAgentIP ) );
		   ZeroMemory( szCampStringHelp, sizeof( szCampStringHelp ) );
		   ZeroMemory( szEventShopState, sizeof( szEventShopState ) );
		   ZeroMemory( szMainServerVersion, sizeof( szMainServerVersion ) );
		   ZeroMemory( szMainServerName, sizeof( szMainServerName ) );
	}
};

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
	char szSTRFILEVER[ 8 ];				// Game Server Version
	char szSTRINTERNALNAME[ 32 ];
	char szGameServerID[ 32 ];			// Game Server ID
	char szGameServerName[ 32 ];		// Game Server Name
	char szLogDBIP[ 32 ];				// LogDB Agent IP / port
	int LogDBPort;
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
	int32 dropLogCount;					// 드랍된 갯수.

	GAMESERVERINFO() : dwGlobalTime(0), csPort(0), SSPort(0), MSPort(0), ThreadCount(0), NodeSize(0), CopyNodeSize(0)
		, RemainderNode(0),	RoomNodeSize(0), PlazaNodeSize(0), HeapQuartersNodeSize(0),	CopyRoomNodeSize(0), CopyPlazaNodeSize(0)
		, CopyHeapQuartersNodeSize(0), RommRemainderNode(0), BattleRoomNodeSize(0),	BattleRoomCopyNodeSize(0), PartyLevelCheckMinRoom(0)
		, BattleRoomRemainderNode(0), LadderTeamNodeSize(0), LadderTeamCopyNodeSize(0),	LadderTeamCampBattlePlay(0), LadderTeamRemainderNode(0)
		, BroadCastUDPnRelayNodeSize(0), BroadCastUDPnRelayRemainderNodeSize(0), DBClientNodeSize(0)
		, LogDBPort(0), HackCheckMin(0), HackCheckMax(0), HackCheckLess(0), HackCheckOver(0), HackCheckLessOver(0), HackCheckTotal(0), IsClientVersion(0)
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

namespace EMCONTROLTYPE
{
	enum ECONTYPE
	{
		LS_GETSERVERINFO,
		LS_SETALLSERVERBLOCK,
		LS_SETSERVERBLOCK,
		LS_FILLSERVERINFO,
		LS_SETMAXUSER,
		LS_FILLINFODRAW,

	};
}
namespace EMSGTYPE
{
	enum ETYPE
	{
		LS_MSG_SVRINFO,
		LS_MSG_INIT,
	};
}
#define UPDATESVRINFO 990
#define  STR_IP_MAX 64
#pragma pack(push,1)
typedef struct _LoginServerInfo_
{
	TCHAR ipaddr[STR_IP_MAX];
	int   port;
	int acceptcountpersec;
	int acceptcount;
	int closecount;
	int oppoolcount;
	int clientpoolcount;
	int userinfocount;
	int serverconnectorpoolcount;
	int packetquecount;
	int serverconnectcount;
}LoginServerInfo_;

#pragma pack(pop)
#endif