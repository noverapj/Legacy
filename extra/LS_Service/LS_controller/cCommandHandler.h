#pragma once

#include <vector>

class cCommand;

class cCommandHandler
{
public:
	cCommandHandler();
	virtual ~cCommandHandler();

	void	Init();
	void	Destroy();

protected:
	void	LoadCommand();
	void	InsertCommand(const uint32 commandId, cCommand* command);

public:
	cCommand* Get(const uint32 commandId);

protected:
	typedef std::vector<cCommand*> COMMMANDTABLE;
	COMMMANDTABLE m_commandTable;
};
