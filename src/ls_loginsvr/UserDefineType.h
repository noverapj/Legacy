#pragma once
#pragma pack(push,1)
#include "../iocpSocketDLL/iocpSocketDLL.h"
#include <atlcoll.h>
#include "UserDefine.h"

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
/* for Connector                                                                     */
/************************************************************************/
typedef struct _SVRCONNECTINFO_
{
	int		serverIndex;
	int     port;
	TCHAR	serverName[STR_IP_MAX];
	TCHAR	ipAddr[STR_IP_MAX];
}SVRCONNECTINFO_;

typedef std::vector<SVRCONNECTINFO_> VIPADDR;
typedef SVRCONNECTINFO_ SVRCONNECTINFO_;
 
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
typedef struct _LOG_
{
	int debugid;
	TCHAR message[2000];
}LOG_;
/************************************************************************/
/* ConnectAssist struct                                                                     */
/************************************************************************/
class LSConnector;
class SchedulerNode;
typedef struct _Connect_
{
	_Connect_()
	{
		opid			= 0;
		serverIndex		= 0;
		port			= 0;
		node			= NULL;
	}
	int   opid;
	int   serverIndex;
	TCHAR ipAddr[STR_IP_MAX];
	int	  port;
	LSConnector* node;
}Connect_;

typedef struct _ServerInfo_
{
	_ServerInfo_()
	{
		Clear();
	};

	void Clear()
	{
		serverId	= -1;
		socketState = ESOCKET::LS_DISCONNECTED;
		gameIndex	= 0;
		userCount	= 0;
		roomCount	= 0;
		port		= 0;
		csport		= 0;
		sendCount	= 0;
		serverIndex	= 0;
		serverState = ESVRSTATE::LS_RESPSERVERSTATE;
		ZeroMemory(ipAddr, sizeof(ipAddr));
	}

	int		serverId;
	int		serverState;
	int     socketState;
	int		gameIndex;
	int	    userCount;
	int		roomCount;
	TCHAR	ipAddr[STR_IP_MAX];
	int		port;
	int     sendCount;
	int     serverIndex;
	TCHAR   publicAddr[STR_IP_MAX];
	int		csport;
}ServerInfo_;
 
/************************************************************************/
/* PacketStruct                                                                     */
/************************************************************************/
typedef struct _ResponseServerInfo_
{
	TCHAR ipAddr[STR_IP_MAX];
	int  port;
	_ResponseServerInfo_()
	{
		memset(ipAddr,0,STR_IP_MAX);
		port =0;
	}
}ResponseServerInfo_;

typedef struct _OnError_ //kyg ∞ÌπŒ¡ﬂ 
{
	int ErrorCode;

}OnError_;
/************************************************************************/
/* Operation Structure                                                                     */
/************************************************************************/
typedef struct _stOp_ 
{
	int opid;
}stOp_;

typedef struct _Tedata_ : public stOp_
{
	TCHAR m_IpAddr[STR_IP_MAX];
	int serverId;
	int freeCount;
}Tedata_;

typedef struct _OnAccept_ : public stOp_
{
	LSConnector* node;
}OnAccept_;


typedef struct _ReConnect_ : public stOp_
{
	int   serverId;
	TCHAR ipAddr[STR_IP_MAX];
	int serverPort;
	int serverIndex;
}ReConnect_;

typedef struct _SchedulerOperation_ : public stOp_
{
	void SetOperation(const EOPID::OPID Id, const ESCHEDULE::ESCHE operation, SchedulerNode* data)
	{
		opid		= Id;
		eoperation	= operation;
		node		= data;
	}

	SchedulerNode* node;
	int eoperation;
}SchedulerOperation_;

typedef struct _ChangeTickTime_ : public stOp_
{
	SchedulerNode* node;
	int nagleTime;
	int code;
}ChangeTickTime_;

typedef struct _UserInfo_ : public stOp_
{
	int  mineId;
	int  token;
	int  timeNow;
	bool sendState;
	bool timeOutCloeState;
	_UserInfo_()
	{
		mineId = -1;
		timeNow = 0;
		token = 0;
		sendState = false;
		timeOutCloeState =false;
	}
}UserInfo_;

typedef struct _WaitClient_ : public stOp_
{
	int token;
	int mineId;
	unsigned int time;
}WaitClient_;

typedef struct _SendToken_ : public stOp_
{
	CConnectNode* node;
	int serverId;
	TCHAR strserverip[STR_IP_MAX];
	int port;

}SendToken_;

typedef struct _OnClient_ 
{
	TCHAR userid[STR_USERID_MAX];
	int  port;
}OnClient_;

typedef struct _TrySendServerInfo_ : public stOp_
{

}TrySendServerInfo_;

typedef struct _LoginServerInfo_
{
	TCHAR ipAddr[STR_IP_MAX];
	int   port;
	int acceptCountpersec;
	int acceptCount;
	int closeCount;
	int opPoolCount;
	int clientPoolCount;
	int userinfoCount;
	int serverConnectorPoolCount;
	int packetQueueCount;
	int serverConnectCount;
}LoginServerInfo_;

#pragma pack(pop)