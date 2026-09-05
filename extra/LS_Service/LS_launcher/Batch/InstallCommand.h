#pragma once
#include "BaseCommand.h"

class InstallCommand
	: public BaseCommand
{
public:
	InstallCommand();
	~InstallCommand();

public:
	BOOL ExecuteServiceAll(CServerDialog* dialog);
	BOOL ExecuteServiceOne(CServerDialog* dialog);
	
	BOOL CheckContinueServiceAll(CServerDialog* dialog);
	BOOL CheckContinueServiceOne(CServerDialog* dialog);

	BOOL IsValidDetail();
};