#include "../stdAfx.h"
#include "Handler.h"

extern CLog LOG;

Handler::Handler( MPSCRecvQueue* queue )
{
	m_queue = queue;
	m_operations.reserve(100);
}

Handler::~Handler(void)
{
}

Operation* Handler::FindOperation (unsigned int operationID )
{
	if( m_operations.size() < operationID )
	{
		LOG.PrintTimeAndLog(0,"Unknown Operation[%d]",operationID);
		return NULL;
	}

	return m_operations[operationID];
}