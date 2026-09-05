#include "stdafx.h"
#include "cSystem.h"
#include "../include/ISystem.h"
#include "../include/IProcessor.h"
#include "../include/cSingleton.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ISystem::ISystem(void)
{
	Init();
}

ISystem::~ISystem(void)
{
	Destroy();
}

void ISystem::Init()
{
}

void ISystem::Destroy()
{
	Stop();
	Cleanup();
}

//////////////////////////////////////////////////////////////////////
// Operation
//////////////////////////////////////////////////////////////////////

BOOL ISystem::Startup(const uint32 maxSession, const uint32 maxPacket, const uint32 maxQueue, const uint32 encoding) const
{
	cSystem* system = cSingleton<cSystem>::GetInstance();
	return system->Startup(maxSession, maxPacket, maxQueue, encoding);
}

void ISystem::Cleanup() const
{
	cSystem* system = cSingleton<cSystem>::GetInstance();
	system->Cleanup();
}

BOOL ISystem::Start(const int32 workerCount, const int32 commandCount, const int32 connectorCount) const
{
	cSystem* system = cSingleton<cSystem>::GetInstance();
	
	if(!system->StartModule(E_THREADID_WORKER, workerCount))		return FALSE;
	if(!system->StartModule(E_THREADID_COMMAND, commandCount))		return FALSE;
	if(!system->StartModule(E_THREADID_CONNECTOR, connectorCount))	return FALSE;
	return TRUE;
}

void ISystem::Stop() const
{
	cSystem* system = cSingleton<cSystem>::GetInstance();
	system->EndModule();
}

void ISystem::SetProcessor(IProcessor* processor) const
{
	cSystem* system = cSingleton<cSystem>::GetInstance();
	system->SetProcessor(processor);
}

void ISystem::SetEvent(const uint32 connectId, const uint32 disconnectId) const
{
	cSystem* system = cSingleton<cSystem>::GetInstance();
	system->SetEvent(connectId, disconnectId);
}

BOOL ISystem::Listen(const uint16 port, const uint32 backlog) const
{
	cSystem* system = cSingleton<cSystem>::GetInstance();
	return system->Listen(port, backlog);
}

BOOL ISystem::Connect(const uint32 serverId, const TCHAR* IP, const uint16 port) const
{
	cSystem* system = cSingleton<cSystem>::GetInstance();
	return system->Connect(serverId, IP, port);
}

//////////////////////////////////////////////////////////////////////
// Operation::queue
//////////////////////////////////////////////////////////////////////

BOOL ISystem::Call(const uint32 command, const uint8* buffer, const uint32 length) const
{
	cCommandQueue* pCommandQueue = cSingleton<cSystem>::GetInstance()->GetCommandQueue();
	if(pCommandQueue)
	{
		pCommandQueue->PushCommand(
			static_cast<uint32>(0), 
			command, 
			buffer, 
			length);
		return TRUE;
	}
	return FALSE;
}
