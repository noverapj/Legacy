#pragma once

#include "BaseCommand.h"

class QuitCommand : public BaseCommand
{
public:
	QuitCommand(void);
	~QuitCommand(void);

public:
	BOOL ExecuteServiceAll(CServerDialog* dialog);
	BOOL ExecuteServiceOne(CServerDialog* dialog);
	
	BOOL CheckContinueServiceAll(CServerDialog* dialog);
	BOOL CheckContinueServiceOne(CServerDialog* dialog);

	BOOL IsValidDetail();
};

