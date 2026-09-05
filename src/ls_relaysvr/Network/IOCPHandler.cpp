#include "StdAfx.h"
#include "IOCPHandler.h"


IOCPHandler::IOCPHandler(void)
{
}

IOCPHandler::~IOCPHandler(void)
{
}

bool IOCPHandler::Init(int nworkercount)
{
	if(!CreateIOCP())
		return false;
	CreateWorkers(nworkercount);
	return true;
}