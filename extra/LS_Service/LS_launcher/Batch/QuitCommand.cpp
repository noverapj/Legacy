#include "StdAfx.h"
#include "QuitCommand.h"
#include "../resource.h"
#include "../ServerDialog.h"


QuitCommand::QuitCommand(): BaseCommand(BCT_QUIT)
{
}

QuitCommand::~QuitCommand()
{
}

BOOL QuitCommand::ExecuteServiceAll(CServerDialog* dialog)
{
	AfxGetMainWnd()->SendMessage(WM_CLOSE);
	return TRUE;
}

BOOL QuitCommand::ExecuteServiceOne(CServerDialog* dialog)
{
	AfxGetMainWnd()->SendMessage(WM_CLOSE);
	return TRUE;
}

BOOL QuitCommand::CheckContinueServiceAll(CServerDialog* dialog)
{
	return TRUE;
}

BOOL QuitCommand::CheckContinueServiceOne(CServerDialog* dialog)
{
	return TRUE;
}

BOOL QuitCommand::IsValidDetail()
{
	return TRUE;
}