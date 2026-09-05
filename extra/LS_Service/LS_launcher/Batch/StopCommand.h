#pragma once

#include "BaseCommand.h"

class StopCommand : public BaseCommand
{
public:
	StopCommand();
	~StopCommand();

public:
	BOOL ExecuteServiceAll(CServerDialog* dialog);
	BOOL ExecuteServiceOne(CServerDialog* dialog);
	
	BOOL CheckContinueServiceAll(CServerDialog* dialog);
	BOOL CheckContinueServiceOne(CServerDialog* dialog);

	BOOL IsValidDetail();
};