#include "../stdafx.h"
#include "DBServer.h"
#include "../NodeInfo/AcceptorUserNode.h"
#include "../NodeInfo/User.h"
#include "../NodeInfo/UserNodeManager.h"
#include "../ThreadPool/ioThreadPool.h"

//////////////////////////////////////////////////////////////////////////
ioDBServer::ioDBServer()
{
	SetAcceptor(new AcceptorUserNode, ITPK_ACCEPT_SESSION);
}

