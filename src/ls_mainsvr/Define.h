#ifndef ___DEFINE_H__
#define ___DEFINE_H__

#include "../include/cSingleton.h"
#include "Languages/ioLanguages.h"

#define ENC_LOGIN_KEY_NUM       30
#define LOGIN_KEY_PLUS_ONE      16
#define ENC_ID_NUM_PLUS_ONE     25
#define ID_NUMBER       20
#define ID_NUM_PLUS_ONE 21
#define PW_NUMBER       12
#define PW_NUM_PLUS_ONE 13
#define PW_ENCRYPT_NUMBER	24
#define PW_ENCRYPT_PLUS_ONE 25

#define MAX_CHARACTER   12      //유저가 소유할수 있는 최대 캐릭터 수  
#define MAX_SLOTBAG     20      //한개의 슬롯에 들어갈수있는 최대값.
#define MAX_USE_SLOTBAG 16		//한개 슬롯에서 현재 사용가능 최대값

#define MAX_RIGHT_SLOT_SIZE 20	//아이템 도감 별 최대 인덱스 사이즈

#define MAX_PLAYER		16

#define MAX_JOIN_CHANNEL 3

#define KINDRED_HUMAN	1
#define KINDRED_ELF		2
#define KINDRED_DWARF	3

#define	EQUIP_UNKNOWN	1000
#define EQUIP_WEAPON    0
#define EQUIP_ARMOR     1
#define EQUIP_HELM      2
#define EQUIP_CLOAK     3
#define EQUIP_OBJECT	4
#define EQUIP_WEAR      5
#define MAX_EQUIP_SLOT	6		//Character EquipSlot의 최대사이즈

#define GUILD_NAME_NUMBER			20
#define GUILD_NAME_NUM_PLUS_ONE		21
#define GUILD_POS_NUMBER			12
#define GUILD_POS_NUM_PLUS_ONE		13
#define GUILD_TITLE_NUMBER			110
#define GUILD_TITLE_NUMBER_PLUS_ONE 111
#define GUILD_MAX_ENTRY_DELAY_USER  16
#define GUILD_MAX_LOAD_LIST         30 //kyg 맥스 
#define GUILD_LOAD_START_INDEX      2147483647

#define TOURNAMENT_TEAM_NAME_NUM_PLUS_ONE			21
#define TOURNAMENT_NAME_NUM_PLUS_ONE				21
#define TOURNAMENT_CAMP_NAME_NUM_PLUS_ONE           21
#define TOURNAMENT_ANNOUNCE_NUM_PLUS_ONE			513
#define TOURNAMENT_CUSTOM_MAX_ROUND                 11
#define TOURNAMENT_CUSTOM_CONFIRM_USER_LIST_COUNT   30

#define TOURNAMENT_LOAD_START_INDEX		2147483647
#define TOURNAMENT_MAX_LOAD_LIST		10
#define TOURNAMENT_TEAM_MAX_LOAD_LIST   30

#define TRADE_START_LAST_INDEX      2147483647
#define TRADE_MAX_LOAD_LIST         30

#define PRACTICE_MAX_LOAD_LIST         100 //kyg 맥스 
#define PRACTICE_LOAD_START_INDEX      2147483647


#define US_TUTORIAL_CLEAR      -1

#define MAX_CHAR_DBITEM_SLOT   4	//InventorySlot의 최대사이즈

#define IP_NUM_PLUS_ONE 16

#define USER_GUID_NUM_PLUS_ONE 32

#define BLOCK_NOTE_NUM_PLUS_ONE 1001
#define BLOCK_REASON_NUM_PLUS_ONE BLOCK_NOTE_NUM_PLUS_ONE

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

struct Quaternion
{
	float x, y, z, w;
	
	Quaternion()
	{
		x = 0;
		y = 0;
		z = 0;
		w = 1;
	}

	Quaternion( float _x, float _y, float _z, float _w )
	{
		x = _x;
		y = _y;
		z = _z;
		w = _w;
	}
};

typedef std::vector< Vector3 > Vector3Vec;
typedef std::deque< Vector3 > Vector3Deq;

enum TeamType
{
	TEAM_NONE,
	TEAM_RED,
	TEAM_BLUE,
	TEAM_PRIVATE_1,
	TEAM_PRIVATE_2,
	TEAM_PRIVATE_3,
	TEAM_PRIVATE_4,
	TEAM_PRIVATE_5,
	TEAM_PRIVATE_6,
	TEAM_PRIVATE_7,
	TEAM_PRIVATE_8,
	TEAM_PRIVATE_9,
	TEAM_PRIVATE_10,
	TEAM_PRIVATE_11,
	TEAM_PRIVATE_12,
	TEAM_PRIVATE_13,
	TEAM_PRIVATE_14,
	TEAM_PRIVATE_15,
	TEAM_PRIVATE_16
};

enum CampType
{
	CAMP_NONE,
	CAMP_BLUE,
	CAMP_RED,
};

enum WinTeamType
{
	WTT_NONE,
	WTT_RED_TEAM,
	WTT_BLUE_TEAM,
	WTT_DRAW,
	WTT_VICTORY_RED_TEAM,
	WTT_VICTORY_BLUE_TEAM
};

// DB에 저장되는 값으로 순차적으로 증가하지 않는다.
enum ChannelingType
{
	CNT_NONE        =   -1,
	CNT_WEMADEBUY   =   0,
	CNT_STEAM		=   1,
	CNT_MGAME       =   300,
	CNT_DAUM        =   400,
	CNT_BUDDY       =   500,
	CNT_NAVER       =   600,
	CNT_TOONILAND   =   700,
	CNT_NEXON		=   800,
	CNT_HANGAME		=	900,
	CNT_VALOFE		=  1000,
	CNT_HAPPYTUK	=  1100,
};

enum SpecialShopBuyType
{
	SBT_GOODS_BUY		= 0,
	SBT_GOODS_PRESENT	= 1,
};

struct UserRankInfo
{
	ioHashString szName;
	int iRank;

	UserRankInfo()
	{
		iRank = 0;
	}
};

struct DamageTable
{
	ioHashString szName;
	int iDamage;

	DamageTable()
	{
		iDamage = 0;
	}
};

typedef std::vector< DamageTable > DamageTableList;

#define MAX_MODE ( MAX_MODE_TYPE - 1 )

typedef std::vector<int> IntVec;
typedef std::vector<DWORD> DWORDVec;
typedef std::vector<float> FloatVec;
typedef std::vector<UserRankInfo> UserRankInfoList;

extern LONG __stdcall ExceptCallBack ( EXCEPTION_POINTERS * pExPtrs );
extern char *_i64toStr(__int64 i64Num);

//#pragma pack( 1 )

enum ServerType
{
	eServerType_Default		= 0,
	eServerType_MainServer	= 1,
	eServerType_GameServer	= 2,
};

enum TradeType
{
	TRADE_NONE	= 0,
	TRADE_ALL	= 1,	// 메인서버가 처음 보내는 거래소 아이템 전체 리스트 패킷
	TRADE_ADD	= 2,	// 메인서버로 보내는 거래소 아이템 추가 패킷
	TRADE_DEL	= 3,	// 메인서버로 보내는 거래소 아이템 삭제 패킷
};

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
	char	szLogDBAgentIP[ 32 ];		//DB AGENT SERVER INFO
	int		DBAgentPort;
	bool	bDBAConnected;
	int		LogDBAgentPort;
	bool	bLogDBAConnected;
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
	int		usingLogCount;					// 현재
	int		maxUsingLogCount;				// 가장 많이 쓸때
	int		remainLogCount;					// remain
	int		dropLogCount;					// 드랍된 갯수.

	MAINSERVERINFO() : dwGlobalTime(0), iPort(0), ThreadCount(0), JoinServerCount(0), RemainderMemPoolCount(0)
		, DBAgentPort(0), bDBAConnected(false), LogDBAgentPort(0), bLogDBAConnected(false), MaxGuildCount(0)
		, MaxUpdateGuild(0), MaxTradeItemCount(0), EventGoodsSaveDataCount(0), MaxToolConnectCount(0)
		, bUseClientVersion(false), iClientVersion(0), usingLogCount(0), maxUsingLogCount(0), remainLogCount(0), dropLogCount(0)
	{
		   ZeroMemory( szPublicIP, sizeof( szPublicIP ) );
		   ZeroMemory( szPrivateIP, sizeof( szPrivateIP ) );
		   ZeroMemory( RecvQueuePacketCount, sizeof( RecvQueuePacketCount ) );
		   ZeroMemory( RecvQueueRemainderCount, sizeof( RecvQueueRemainderCount ) );
		   ZeroMemory( szDBAgentIP, sizeof( szDBAgentIP ) );
		   ZeroMemory( szLogDBAgentIP, sizeof( szLogDBAgentIP ) );
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

#pragma pack(push,1)
struct SendRelayInfo_
{
	TCHAR m_ipAddr[64];
	int m_port;
	int  m_userCount;
	int  m_roomCount;
	int  m_serverCount;
	int m_64DropCount;
	int m_256DropCount;
	int m_1024DropCount;
	int m_64UsingCount;
	int m_256UsingCount;
	int m_1024UsingCount;
	SendRelayInfo_()
	{
		ZeroMemory(m_ipAddr,64);
		m_port = 0; 
		m_userCount = 0;
		m_roomCount= 0;
		m_64DropCount= 0;
		m_256DropCount= 0;
		m_1024DropCount= 0;
		m_64UsingCount= 0;
		m_256UsingCount= 0;
		m_1024UsingCount= 0;
	}
};
#pragma pack(pop)

//SIngleton
typedef cSingleton<ioLanguages> S_Languages;
#define g_Languages (*S_Languages::GetInstance())


//#pragma pack()

#define PACKET_GUARD_VOID(x)	{ BOOL rtval = x; if( (rtval)==FALSE ) { LOG.PrintTimeAndLog(0,"[PACKET_GUARRD_ERROR] %s::%s",__FUNCTION__,#x); return; } }
#define PACKET_GUARD_INT(x)		{ BOOL rtval = x; if( (rtval)==FALSE ) { LOG.PrintTimeAndLog(0,"[PACKET_GUARRD_ERROR] %s::%s",__FUNCTION__,#x); return -1; } }
#define PACKET_GUARD_BOOL(x)	{ BOOL rtval = x; if( (rtval)==FALSE ) { LOG.PrintTimeAndLog(0,"[PACKET_GUARRD_ERROR] %s::%s",__FUNCTION__,#x); return FALSE; } }
#define PACKET_GUARD_bool(x)	{ BOOL rtval = x; if( (rtval)==FALSE ) { LOG.PrintTimeAndLog(0,"[PACKET_GUARRD_ERROR] %s::%s",__FUNCTION__,#x); return false; } }
#define PACKET_GUARD_BREAK(x)	{ BOOL rtval = x; if( (rtval)==FALSE ) { LOG.PrintTimeAndLog(0,"[PACKET_GUARRD_ERROR] %s::%s",__FUNCTION__,#x); break; } }

#endif