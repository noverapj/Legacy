#include "stdAfx.h"
#include "Handler.h"


Handler::Handler(MPSCRecvQueue* queue)
{
	m_queue = queue;
	m_operations.reserve(100);
}

Handler::~Handler(void)
{
}

Operation* Handler::FindOperation(unsigned int opid)
{
	if(m_operations.size() < opid)
	{
		LOG.PrintTimeAndLog(0,"Unknown Operation[%d]",opid);
		return NULL;
	}
	return m_operations[opid];
}