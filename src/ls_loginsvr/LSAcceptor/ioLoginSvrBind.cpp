#include "StdAfx.h"
#include "ioLoginSvrBind.h"


ioLoginSvrBind::ioLoginSvrBind(void)
{
	SetAcceptor(new AcceptorMgr, EPROTOCOL::ITPK_ACCEPT_SESSION);
}

ioLoginSvrBind::~ioLoginSvrBind(void)
{
}
