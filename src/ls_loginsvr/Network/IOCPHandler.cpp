#include "StdAfx.h"
#include "IOCPHandler.h"


IOCPHandler::IOCPHandler(void)
{
}


IOCPHandler::~IOCPHandler(void)
{
}

bool IOCPHandler::Init(int workerCount)
{
	if(!CreateIOCP()) 
		return false;

	CreateWorkers(workerCount);
	return true;
}