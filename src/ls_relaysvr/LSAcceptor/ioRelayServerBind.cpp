#include "StdAfx.h"
#include "ioRelayServerBind.h"


ioRelayServerBind::ioRelayServerBind(void)
{
	SetAcceptor(new AcceptorMgr, Protocols::ITPK_ACCEPT_SESSION);
}

ioRelayServerBind::~ioRelayServerBind(void)
{
}
