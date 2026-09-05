#pragma once
#include "BaseCommand.h"

class PlayCommand : public BaseCommand
{
public:
	PlayCommand();
	~PlayCommand();

public:
	BOOL ExecuteServiceAll(CServerDialog* dialog);
	BOOL ExecuteServiceOne(CServerDialog* dialog);
	
	BOOL CheckContinueServiceAll(CServerDialog* dialog);
	BOOL CheckContinueServiceOne(CServerDialog* dialog);

	BOOL IsValidDetail();
};