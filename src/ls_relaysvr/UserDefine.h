#pragma once


#define LOG_DEBUG_LEVEL 0
#define  CLOCK_PER_MILSEC 100
#define FREE_SERVERCOUNT 5
#define DEFAULT_BUFFER 16384 

#define CONNECT_TYPE_USER                 1
#define CONNECT_TYPE_SERVER               2
#define CONNECT_TYPE_MAIN_SERVER          3
#define CONNECT_TYPE_GAMEDB_SERVER        4
#define CONNECT_TYPE_BILLING_RELAY_SERVER 5
#define CONNECT_TYPE_MONITORING           6
#define CONNECT_TYPE_LOGDB_SERVER         7
#define ITPK_SENDBUFFER_FLUSH_PROCESS		0x1007

// Monitoring Server <-> Server TCP PACKET 0x10601 ~ 0x10700
#define LS_PING_TIME 1000 * 180
#define RS_SERVERINFO_TIME 5000
#define RS_GHOST_TIME_CHECK 4000
#define RS_KING_CHECK 5000
#define RS_REPORT_TIME (10000*6)*10

/************************************************************************/
/* Packet deinfe max                                                                     */
/************************************************************************/
#define STR_IP_MAX 64
#define STR_USERID_MAX 128
#define PUBLICID_MAX 21

/************************************************************************/
/* Enum                                                                     */
/************************************************************************/
namespace SocketTypes
{
	enum Socket
	{
		LS_CONNECTED = 0,
		LS_DISCONNECTED,
		LS_REQSERVERSTATE,
		LS_RESPSERVERSTATE,
		LS_ZOMBIESERVERSTATE,
		LS_BLOCKED,
	};
}

namespace NodeTypes
{
	enum NODETYPE
	{
		ACCEPTOR = 0x2001,
		ACCEPTCLIENT,
		CONNECTOR,
		MONITOR,
		USER,
	};

}

namespace OperationIndex
{
	enum OPID
	{
		TEST_O = 0,
		ONACCEPT,
		RECONNECT,
		SCHEDULER,
		CHANGETICKTIME,
		TRYSENDSERVERINFO,
		OPERATIONINDEX_END,
	
	};
}

namespace ScheduleTypes
{
	enum ESCHE
	{
		SCHEDULEOP,
		SENDSERVERINFO,
		ONPING,
		ONSENDSERVERINFO,
		USERGHOSTCHECK,
		USERKINGCHECK,
		REPORT,
		SENDBUFFERFLUSH,
		TEST,
	};
}

namespace ConnectAssistTypes
{
	enum OPID
	{
		CONNECT,
		CLOSE,
	};
}

namespace ErrorCodes
{
	enum ERRORS
	{
		LS_SERVERMAX,
		LS_WAITLISTMAX,
		LS_TIMEOUT,
		LS_SERVERALLBLOCK,
		LS_UNKNOWNTYPE,
		LS_NOSERVER,	
	};
}

namespace ConnectStates
{
	enum ECETATE
	{
		LS_GHOST,
		LS_ALLBLOCK,
		LS_USERCONNECTED,
	};
}

namespace ModeTypes
{
	enum ModeType
	{
		MT_NONE				= 0,
		MT_SYMBOL			= 1,
		MT_CATCH			= 2,
		MT_KING				= 3,
		MT_TRAINING			= 4,
		MT_SURVIVAL			= 5,
		MT_TEAM_SURVIVAL	= 6,
		MT_BOSS				= 7,
		MT_MONSTER_SURVIVAL = 8,
		MT_FOOTBALL			= 9,
		MT_HEROMATCH		= 10,
		MT_GANGSI			= 11,
		MT_DUNGEON_A    	= 12,
		MT_HEADQUARTERS     = 13,
		MT_CATCH_RUNNINGMAN = 14,
		MT_FIGHT_CLUB		= 15,
		MT_TOWER_DEFENSE	= 16,
		MT_DARK_XMAS		= 17,
		MT_FIRE_TEMPLE		= 18,
		MAX_MODE_TYPE
	};
}

namespace ControlTypes
{
	enum ECONTYPE
	{
		RS_INFO = 0xab,
		RS_ADD_USER,//
		RS_DEL_USER,
		RS_ON_ADD_USER,
		RS_INSERT_GROUP,
		RS_REMOVE_GROUP,
		RS_SEND_PACKET,
		RS_CHANGE_ADDR,
		RS_HACK_ANNOUNCE,
		RS_USER_GHOST,
		RS_USER_MODE_PING_ROW,
		RS_RESERVER_ROOM_JOIN,
		RS_WHOLECHAT_STATE,
		RS_CHANGE_NICKNAME,
		RS_END,
		RC_USE_RELAYSVR = 0xf0,
		RC_NOTUSE_RELAYSVR,
	};
}

namespace TimeTypes
{
	enum etimeval
	{
		MIN_5 = 300000,
		SEC_30 = 30000,
		SEC_60 = 60000,
		SEC_90 = 90000,
	};
}

namespace Protocols
{
	enum PROTOCOL
	{
	    STPK_HACK_ANNOUNCE	=  0x2117,
     	STPK_CHAT_MODE		=  0x2118,
        STPK_HACK_QUIZ		=  0x2119,
		ITPK_OPERATIONTYPE = 99,
		ITPK_ACCEPT_SESSION = 0x1001,
		MNSTPK_STATUS_REQUEST = 0x10601, // 안쓰는 프로토콜
		MNSTPK_STATUS_RESULT  =	0x10602,
		MNSTPK_CHANGE_REQUEST = 0x10603,
		MNSTPK_CHANGE_RESULT  = 0x10604,
		MNSTPK_CLOSE		  = 0x10605, // 여기까지
		ITPK_CLOSE_SESSION = 0x10679,
		LSTPK_STATUS_REQUEST = 0x10680,
		LSTPK_CONNECT_CLIENT = 0x10681,
		LSTPK_STATUS_RESPONSE = 0x10682,
		LSPTK_CONNECT_REQUEST = 0x10683,
		LSPTK_TICKET_REQUEST = 0x10684,
		LSPTK_TICKET_RESPONSE = 0x10685,
		LSPTK_TIMEOUT_CLOSE_REQUEST = 0x10686,
		LSPTK_ERROR = 0x10687,
		LSPTK_CONNECT_MONITOR = 0x10688,
		LSPTK_CONTROL_SERVER = 0x10689,
		LSPTK_SERVER_RESPONSE = 0x10690,
		LSPTK_PING = 0x10691,
		LSPTK_SERVER_FILLINFO = 0x10692,
		LSPTK_BLOCK_REQUEST = 0x10693,
		LSPTK_BLOCK_RESPONSE = 0x10694,
		LSPTK_CONNECT_RESPONSE = 0x10695,

		LSPTK_END = 0x10699,

		////////////////////////////////////////////////////////////////////////
		//CLIENT SEND UCP PACKET
		CUPK_CONNECT = 			0x4001,
		CUPK_SYNCTIME = 			0x4002,
		CUPK_CHAT = 				0x4003,
		CUPK_RESERVE_ROOM_JOIN = 	0x4006,
		CUPK_CHECK_KING_PING = 	0x4007,

		//SERVER SEND UCP PACKET
		SUPK_CONNECT =               0x6001,
		SUPK_SYNCTIME =              0x6002,
		SUPK_MODE_PING = 			  0x6003,
		SUPK_CAMP_INFLUENCE_ALARM =  0x6004,
		SUPK_SERVER_ALARM_MENT =     0x6005,
		SUPK_PROTECT =               0x6006,
		SUPK_WHOLE_CHAT =            0x6007,

		///////////////////////////////////////////////////////////////
		/// REALY SEND GAME SERVER PACKET
		RSPTK_ON_CONNECT = 0x2ab00,
		RSPTK_ON_CONTROL = 0x2ab01,
		///////////////////////////////////////////////////////////////
		/////LOG Library Packet
		LTPK_LOG =			0x9C01,
	};

}

#define USER_TIME_OUT	2500
#define PING_MS_VALUE	60
#define SYNC_TIME_GAP	9000

/************************************************************************/
/* extern function                                                                     */
/************************************************************************/
extern LONG WINAPI UnHandledExceptionFilter(struct _EXCEPTION_POINTERS *exceptionInfo);
 
#define PUTQFUNNC(x,q)\
	SP2Packet st(Protocols::ITPK_OPERATIONTYPE);\
	st << x;\
	q->InsertQueue(NULL,st,PK_QUEUE_INTERNAL);

#define SAFEDELETE(x)		if(x != NULL) { delete x; x = NULL; }

#define COMPARE(x,min,max) (((x)>=(min))&&((x)<(max)))