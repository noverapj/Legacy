#include "stdafx.h"
//#include "../Window.h"
#include "../MainProcess.h"
#include "iocpHandler.h"

const uint32 WORKER_COUNT = 16;


iocpHandler *iocpHandler::sg_Instance = NULL;
iocpHandler::iocpHandler(){}
iocpHandler::~iocpHandler(){}

iocpHandler &iocpHandler::GetInstance()
{
	if( sg_Instance == NULL )
		sg_Instance = new iocpHandler;
	return *sg_Instance;
}

void iocpHandler::ReleaseInstance()
{
	SAFEDELETE( sg_Instance );
}

bool iocpHandler::Initialize()                           //IOCP INITIALIZE
{
	if( !CCompletionHandler::CreateIOCP() ) return false;
	
	CreateWorkers( WORKER_COUNT );
	return true;
}