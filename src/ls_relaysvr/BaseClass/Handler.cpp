#include "stdAfx.h"
#include "Handler.h"


Handler::Handler( MPSCRecvQueue* queue )
{
	m_queue = queue;
	m_voperations.reserve(100);
}

Handler::~Handler(void)
{
}

Operation* Handler::FindOperation (unsigned int operationId )
{
	if( m_voperations.size() < operationId )
	{
		LOG.PrintTimeAndLog(0,"Unknown Operation[%d]",operationId);
		return NULL;
	}

	return m_voperations[operationId];
}