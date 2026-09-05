#ifndef _GameServer_h_
#define _GameServer_h_

class ServerSocket;
class AcceptorUserNode;
class AcceptorServerNode;
class AcceptorMonitorNode;

#define CONNECT_TYPE_USER                 1
#define CONNECT_TYPE_SERVER               2
#define CONNECT_TYPE_MAIN_SERVER          3
#define CONNECT_TYPE_GAMEDB_SERVER        4
#define CONNECT_TYPE_BILLING_RELAY_SERVER 5
#define CONNECT_TYPE_MONITORING           6
#define CONNECT_TYPE_LOGDB_SERVER         7
#define CONNECT_TYPE_UDP				  8

// ioClientBind
class ioClientBind : public ServerSocket
{
public:
	ioClientBind();
};

// ioServerBind
class ioServerBind : public ServerSocket
{
public:
	ioServerBind();
};

// ioMonitoringBind
class ioMonitoringBind : public ServerSocket
{
public:
	ioMonitoringBind();
};





#endif