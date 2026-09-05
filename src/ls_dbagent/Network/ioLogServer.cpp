#include "../stdafx.h"
#include "ioLogServer.h"
#include "../NodeInfo/AcceptorLogNode.h"
#include "../NodeInfo/logNode.h"
#include "../NodeInfo/LogNodeManager.h"
#include "../ThreadPool/ioThreadPool.h"


ioLogServer::ioLogServer(void)
{
	SetAcceptor((AcceptorNode*)new AcceptorLogNode, ITPK_ACCEPT_SESSION);
}

