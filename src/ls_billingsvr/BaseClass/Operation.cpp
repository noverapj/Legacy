#include "../stdAfx.h"
#include "Operation.h"


Operation::Operation(MPSCRecvQueue* queue)
{
	m_queue = queue;
}

Operation::~Operation(void)
{
}

 

 