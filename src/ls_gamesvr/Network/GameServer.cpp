#include "stdafx.h"
//#include "../Window.h"
#include "../MainProcess.h"
#include "../EtcHelpFunc.h"

#include "../DataBase/DBClient.h"
#include "../DataBase/LogDBClient.h"
#include "../NodeInfo/User.h"
#include "../NodeInfo/UserNodeManager.h"
#include "../NodeInfo/ServerNode.h"
#include "../NodeInfo/ServerNodeManager.h"
#include "../NodeInfo/AcceptorUserNode.h"
#include "../NodeInfo/AcceptorServerNode.h"
#include "../NodeInfo/AcceptorMonitorNode.h"
#include "../MainServerNode/MainServerNode.h"
#include "../BillingRelayServer/BillingRelayServer.h"
#include "../MonitoringServerNode/monitoringnodemanager.h"
#include "GameServer.h"


extern CLog CriticalLOG;


// ioClientBind
ioClientBind::ioClientBind()
{
	SetAcceptor(new AcceptorUserNode, ITPK_ACCEPT_SESSION);
}

// ioServerBind
ioServerBind::ioServerBind()
{
	SetAcceptor(new AcceptorServerNode, ITPK_ACCEPT_SESSION);
}

// ioMonitoringBind
ioMonitoringBind::ioMonitoringBind()
{
	SetAcceptor(new AcceptorMonitorNode, ITPK_ACCEPT_SESSION);
}
