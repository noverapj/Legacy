// cCommandHandler.cpp

#include "stdafx.h"
#include "GameCommand.h"
#include "InternalCommand.h"
#include "cCommandHandler.h"
#include "../common/Protocol.h"
#include "../common/Internal.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

cCommandHandler::cCommandHandler()
{
	Init();
}

cCommandHandler::~cCommandHandler()
{
	Destroy();
}

void	cCommandHandler::Init()
{
	m_commandTable.resize(0xFFFF, NULL);
	LoadCommand();
}

void	cCommandHandler::Destroy()
{
	COMMMANDTABLE::iterator it;
	for(it = m_commandTable.begin() ; it != m_commandTable.end() ; ++it)
	{
		delete *it;
	}
	m_commandTable.clear();
}

//////////////////////////////////////////////////////////////////////
// operation
//////////////////////////////////////////////////////////////////////

void	cCommandHandler::LoadCommand()
{
	InsertCommand(I_CONNECT, new IConnect);
	InsertCommand(I_CLOSE, new IClose);
	InsertCommand(I_PING, new IPing);
	InsertCommand(I_FLUSH, new IFlush);
	
	InsertCommand(C_PING, new CPing);
	InsertCommand(C_SERVICES, new CServices);
	InsertCommand(C_TRANSFER, new CTransfer);
	InsertCommand(C_REMOVE, new CRemove);
	InsertCommand(C_INSTALL, new CInstall);
	InsertCommand(C_PLAY, new CPlay);
	InsertCommand(C_BACKUP, new CBackup);
	InsertCommand(C_REBOOT, new CReboot);
	InsertCommand(C_RESOURCE, new CResource);
}

void	cCommandHandler::InsertCommand(const uint32 commandId, cCommand* command)
{
	COMMMANDTABLE::pointer ptr = &m_commandTable[commandId];
	*ptr = command;
}

cCommand* cCommandHandler::Get(const uint32 commandId)
{
	return (commandId < 0xFFFF) ? m_commandTable[commandId] : NULL;
}
