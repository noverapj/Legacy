#include "StdAfx.h"
#include "UninstallCommand.h"
#include "../resource.h"
#include "../LS_launcherDlg.h"
#include "../ServerDialog.h"


UninstallCommand::UninstallCommand() : BaseCommand(BCT_UNINSTALL)
{
}

UninstallCommand::~UninstallCommand()
{
}

BOOL UninstallCommand::ExecuteServiceAll(CServerDialog* dialog)
{
	if( !dialog )
		return FALSE;

	if( CheckContinueServiceAll( dialog ) )
	{
		m_state = CST_ALREADY_EXCUTE;
		return TRUE;
	}
	m_state = CST_BEFORE_EXCUTE;

	if( !dialog->CommandUninstallAll() )
	{
		return FALSE;
	}

	return TRUE;
}

BOOL UninstallCommand::ExecuteServiceOne(CServerDialog* dialog)
{
	if( !dialog )
		return FALSE;
	
	if( CheckContinueServiceOne( dialog ) )
	{
		m_state = CST_ALREADY_EXCUTE;
		return TRUE;
	}
	m_state = CST_BEFORE_EXCUTE;

	if( !dialog->CommandUninstall( m_strings[0] ) )
	{
		return FALSE;
	}

	return TRUE;
}

BOOL UninstallCommand::CheckContinueServiceAll(CServerDialog* dialog)
{
	if( !dialog )
		return FALSE;

	int serviceCount = dialog->GetServiceCount();
	if( serviceCount != 0 )
		return FALSE;

	return TRUE;
}

BOOL UninstallCommand::CheckContinueServiceOne(CServerDialog* dialog)
{
	if( !dialog )
		return FALSE;
	
	int index = dialog->FindServiceIndex( m_strings[0] );
	if( index != -1 )
	{
		return FALSE;
	}

	return TRUE;
}

BOOL UninstallCommand::IsValidDetail()
{
	if( m_isServiceAll )
	{
		if( !m_strings.empty() )
			return FALSE;
	}
	else
	{
		if( m_strings.size() != 1 )
			return FALSE;
	}
	return TRUE;
}