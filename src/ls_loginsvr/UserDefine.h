#pragma once


#define LOG_DEBUG_LEVEL		0
#define FREE_SERVERCOUNT	5
#define DEFAULT_BUFFER		16384 

#define CONNECT_TYPE_USER                 1
#define CONNECT_TYPE_SERVER               2
#define CONNECT_TYPE_MAIN_SERVER          3
#define CONNECT_TYPE_GAMEDB_SERVER        4
#define CONNECT_TYPE_BILLING_RELAY_SERVER 5
#define CONNECT_TYPE_MONITORING           6
#define CONNECT_TYPE_LOGDB_SERVER         7

#define ITPK_SENDBUFFER_FLUSH_PROCESS		0x1007
// Monitoring Server <-> Server TCP PACKET 0x10601 ~ 0x10700

/************************************************************************/
/* Packet deinfe max                                                    */
/************************************************************************/
#define STR_IP_MAX 64
#define STR_USERID_MAX 128

/************************************************************************/
/* Enum                                                                 */
/************************************************************************/

namespace ESOCKET
{
	enum Socket
	{
		LS_CONNECTED = 0,
		LS_DISCONNECTED,
	};
}

namespace ESVRSTATE
{
	enum
	{
		LS_REQSERVERSTATE = 2, //모니터링 툴과의 호환을 위하여 
		LS_RESPSERVERSTATE,
		LS_ZOMBIESERVERSTATE,
		LS_BLOCKED,
	};
}
namespace ENODETYPE
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
namespace EOPID
{
	enum OPID
	{
		TEST_O = 0,
		ONACCEPT,
		RECONNECT,
		SCHEDULER,
		CHANGETICKTIME,
		TRYSENDSERVERINFO,
	
	};
}
namespace ESCHEDULE
{
	enum ESCHE
	{
		SCHEDULEOP,
		GHOSTCLIENT,
		TIMEOUTCLIENT,
		TRYSENDSERVERINFO,
		REQUESTSERVERINFO,
		PING,
		SERVERPARTITION,
	};
}
namespace ECONNECTASSIST
{
	enum OPID
	{
		CONNECT,
		CLOSE,
	};
}
namespace EERRORCODE
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
namespace ECONNECTSTATE
{
	enum ECETATE
	{
		LS_GHOST,
		LS_ALLBLOCK,
		LS_USERCONNECTED,
		

	};
}
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
		LS_QUICKEXIT,

	};
}

namespace EPROTOCOL
{
	enum PROTOCOL
	{
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
		LSPTK_PARTITION_REQUEST = 0x10696,
		LSPTK_PARTITION_RESPONSE = 0x10697,
		LSPTK_WHITELIST_REQUEST = 0x10698,

		LSPTK_END = 0x10699
	};
}


/************************************************************************/
/* extern function                                                                     */
/************************************************************************/
extern LONG WINAPI UnHandledExceptionFilter(struct _EXCEPTION_POINTERS *exceptionInfo);

 
#define PUTQFUNNC(x,q)\
	SP2Packet st(EPROTOCOL::ITPK_OPERATIONTYPE);\
	st << x;\
	q->InsertQueue(NULL,st,PK_QUEUE_INTERNAL);

#define SAFEDELETE(x)		if(x != NULL) { delete x; x = NULL; }