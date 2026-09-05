#pragma once

#include "../iocpSocketDLL/iocpSocketDLL.h"
#include <atlcoll.h>
#include "UserDefine.h"
#include "BaseClass/HackCheck.h"

typedef unsigned int		uint;  
typedef unsigned char		uchar;
typedef unsigned short		ushort;
typedef unsigned long		ulong;
typedef unsigned long		uint32;
typedef unsigned short		uint16;
typedef unsigned char		uint8;
typedef unsigned __int64	uint64;
typedef char				int8;
typedef short				int16;
typedef long				int32;
typedef	__int64				int64;
typedef	uint64				uniq;
typedef	uint32				unit;
/************************************************************************/
/* Relay 관련                                                                      */
/************************************************************************/ 
struct Relayheader : public NodeData
{
	int m_packetId;
};

struct UDPPacekt :Relayheader
{
	sockaddr_in m_client_addr;
	int m_size;
	char* m_buffer;

};

struct InsertData : Relayheader
{
	DWORD		 m_dwRoomIndex;
	DWORD		 m_dwUserIndex;
	char         m_szPublicIP[48];
	int          m_iClientPort;
};

struct SendRelayInsertData : Relayheader // 릴레이 서버에게 보낼때 쓰는 전용  
{
	int          m_modeType;
	DWORD		 m_dwRoomIndex;
	DWORD		 m_dwUserIndex;
	char         m_szPublicIP[STR_IP_MAX];
	int          m_iClientPort;
	char         m_szPublicID[PUBLICID_MAX];
};

struct RemoveData :Relayheader
{
	DWORD m_dwRoomIndex;
	DWORD m_dwUserIndex;
};

struct UserData
{
	DWORD		 m_dwUserIndex;
	char         m_szPublicIP[STR_IP_MAX];
	int          m_iClientPort;
	char         m_szPublicID[PUBLICID_MAX];
	int          m_modeType;
	UserData()
	{
		m_dwUserIndex = 0;
		ZeroMemory(m_szPublicIP,STR_IP_MAX);
		m_iClientPort = 0;
		ZeroMemory(m_szPublicID,PUBLICID_MAX);
		m_modeType = 0;
	}
};

struct UserInfo
{
	DWORD		 m_dwUserIndex;
	char         m_szPublicIP[STR_IP_MAX];
	int          m_iClientPort;
	int          m_serverID;
	char         m_szPublicID[PUBLICID_MAX];
	int			 m_ping_total_send_index;		//지금까지 보내온 핑의 인덱스
	int			 m_ping_less_error_count;		//동기화 시간보다 빠른 에러
	int			 m_ping_over_error_count;		//동기화 시간보다 늦은 에러
	int			 m_total_ping_error_count;		//총 동기화 에러
	DWORD		 m_prev_over_ping_time;			//이전메세지가 늦게 온경우의 시간차
	bool		 m_first_heart_beat;			//첫번째 Ping 동기화
	DWORD		 m_dwPingStep;                  //Ping 단계. 0(좋음) ~~~ N(나쁨)
	DWORD        m_dwSpeedHackQuizLimitTime;
	DWORD        m_sync_time;
	short        m_iRoomState;
	short          m_firstState;
	HackCheck::CheckProblem m_SpeedHackQuiz;
	int     m_iCurSpeedHackAnswerChance;
	int     m_modeType;
	int     m_iKingPing;
	int m_iCurCheckKingIndex;
	int m_iCurRecvKingPingCnt;

	UserInfo()
	{
		m_firstState = 0;
		m_dwUserIndex = 0;
		ZeroMemory(m_szPublicIP,STR_IP_MAX);
		m_iClientPort = 0;
		m_serverID = 0;
		ZeroMemory(m_szPublicID,PUBLICID_MAX);
		m_ping_total_send_index = 0;
		m_ping_less_error_count = 0;
		m_ping_over_error_count = 0;
		m_total_ping_error_count = 0;	
		m_prev_over_ping_time = 0;	
		m_first_heart_beat = 0;
		m_dwPingStep = 0;
		m_dwSpeedHackQuizLimitTime = 0;
		m_sync_time = 0;
		m_iRoomState = 0;
		m_modeType = 0;
		m_iKingPing = 0;
		m_iCurCheckKingIndex = 0;
		m_iCurRecvKingPingCnt = 0;
	};
};

struct RelayGroup			
{
	DWORD   m_dwRoomIndex;
	typedef std::vector<UserData> RelayGroups;
	RelayGroups m_RelayUserList;
	RelayGroups* GetUserLists() { return &m_RelayUserList;}
	void Init()
	{
		m_dwRoomIndex = -1;
		m_RelayUserList.reserve(16);
	}

	RelayGroup()
	{
		Init();
	}
};

/************************************************************************/
/* for Connector                                                                     */
/************************************************************************/
 struct SVRCONNECTINFO_
{
	int		serverIndex;
	int     port;
	TCHAR	serverName[STR_IP_MAX];
	TCHAR	ipAddr[STR_IP_MAX];
};

 typedef std::vector<SVRCONNECTINFO_> ConnectIPAddrs;

 
/************************************************************************/
/* For Operation Memorypool                                                                     */
/************************************************************************/
struct MEM32{ };
struct MEM64{ };
struct MEM128{ };
struct MEM256{ };
struct MEM512{ };
struct MEM1024{ };
struct MEM2048{ };
struct MEM4096{ };

/************************************************************************/
/* LogAssist struct                                                                     */
/************************************************************************/
 struct LOG_
{
	int debugid;
	TCHAR message[2000];
};

/************************************************************************/
/* ConnectAssist struct                                                                     */
/************************************************************************/
class GameServerNode;
class SchedulerNode;

struct Connect_
{
	int   opid;
	int   serverIndex;
	TCHAR ipAddr[STR_IP_MAX];
	int	  port;
	GameServerNode* node;

	Connect_()
	{
		Init();
	}

	void Init()
	{
		opid			= 0;
		serverIndex		= 0;
		port			= 0;
		node			= NULL;
	}
};

 struct ServerInfo_
{
	int		serverId;
	int		serverState;
	int		gameIndex;
	int	    userCount;
	int		roomCount;
	TCHAR	ipAddr[STR_IP_MAX];
	int		port;
	int     sendCount;
	int     serverIndex;
	TCHAR   publicAddr[STR_IP_MAX];
	int		csport;

	ServerInfo_()
	{
		Init();
	};

	void Init()
	{
		serverState = SocketTypes::LS_DISCONNECTED;
		ZeroMemory(ipAddr, sizeof(ipAddr));
		serverId	= -1;
		gameIndex	= 0;
		userCount	= 0;
		roomCount	= 0;
		port		= 0;
		csport		= 0;
		sendCount	= 0;
		serverIndex	= 0;
	}
};
 
/************************************************************************/
/* PacketStruct                                                                     */
/************************************************************************/
 struct ResponseServerInfo_
{
	TCHAR ipAddr[STR_IP_MAX];
	int  port;

	ResponseServerInfo_()
	{
		Init();
	}

	void Init()
	{
		memset(ipAddr,0,STR_IP_MAX);
		port =0;
	}
};

 struct _OnError_ //kyg 고민중 
{
	int ErrorCode;

}OnError_;

/************************************************************************/
/* Operation Structure                                                                     */
/************************************************************************/
struct stOp_ 
{
	int opid;
};

struct Tedata_ : public stOp_
{
	TCHAR m_IpAddr[STR_IP_MAX];
	int serverId;
	int freeCount;
};

struct OnAccept_ : public stOp_
{
	GameServerNode* node;
};


struct ReConnect_ : public stOp_
{
	int   serverId;
	TCHAR ipAddr[STR_IP_MAX];
	int serverPort;
	int serverIndex;
};

struct SchedulerOperation_ : public stOp_
{
	SchedulerNode* node;
	int eoperation;
};

struct ChangeTickTime_ : public stOp_
{
	SchedulerNode* node;
	int nagleTime;
	int code;
};

struct WaitClient_ : public stOp_
{
	int token;
	int mineId;
	unsigned int time;
};

struct SendToken_ : public stOp_
{
	CConnectNode* node;
	int serverId;
	TCHAR strserverip[STR_IP_MAX];
	int port;

};

struct OnClient_ 
{
	TCHAR userid[STR_USERID_MAX];
	int  port;
};

struct TrySendServerInfo_ : public stOp_
{

};

#pragma pack(push,1)
struct SendRelayInfo_
{
	TCHAR m_ipAddr[STR_IP_MAX];
	int m_port;
	int m_userCount;
	int m_roomCount;
	int m_serverCount;
	int m_64DropCount;
	int m_256DropCount;
	int m_1024DropCount;
	int m_64UsingCount;
	int m_256UsingCount;
	int m_1024UsingCount;

	SendRelayInfo_()
	{
		Init();
	}

	void Init()
	{
		ZeroMemory(m_ipAddr,STR_IP_MAX);
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