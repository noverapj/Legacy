#pragma once
#include "BaseCommand.h"

class UninstallCommand : public BaseCommand
{
public:
	UninstallCommand();
	~UninstallCommand();

public:
	BOOL ExecuteServiceAll(CServerDialog* dialog);
	BOOL ExecuteServiceOne(CServerDialog* dialog);
	
	BOOL CheckContinueServiceAll(CServerDialog* dialog);
	BOOL CheckContinueServiceOne(CServerDialog* dialog);

	BOOL IsValidDetail();
};