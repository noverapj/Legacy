#include "StdAfx.h"
#include "PlayCommand.h"
#include "../resource.h"
#include "../LS_launcherDlg.h"
#include "../ServerDialog.h"


PlayCommand::PlayCommand() : BaseCommand(BCT_PLAY)
{
}

PlayCommand::~PlayCommand()
{
}

BOOL PlayCommand::ExecuteServiceAll(CServerDialog* dialog)
{
	if( !dialog )
		return FALSE;

	if( CheckContinueServiceAll( dialog ) )
	{
		m_state = CST_ALREADY_EXCUTE;
		return TRUE;
	}
	m_state = CST_BEFORE_EXCUTE;

	if( !dialog->CommandPlayAll() )
	{
		return FALSE;
	}

	return TRUE;
}

BOOL PlayCommand::ExecuteServiceOne(CServerDialog* dialog)
{
	if( !dialog )
		return FALSE;

	if( CheckContinueServiceOne( dialog ) )
	{
		m_state = CST_ALREADY_EXCUTE;
		return TRUE;
	}
	m_state = CST_BEFORE_EXCUTE;

	if( !dialog->CommandPlay( m_strings[0] ) )
	{
		return FALSE;
	}

	return TRUE;
}

BOOL PlayCommand::CheckContinueServiceAll(CServerDialog* dialog)
{
	if( !dialog )
		return FALSE;

	int serviceCount = dialog->GetServiceCount();
	for( int index = 0; index < serviceCount ; index ++ )
	{
		if( !dialog->IsServiceOn(index) )
		{
			return FALSE;
		}
	}

	return TRUE;
}

BOOL PlayCommand::CheckContinueServiceOne(CServerDialog* dialog)
{
	if( !dialog )
		return FALSE;
	
	if( !dialog->IsServiceOn(m_strings[0]) )
	{
		return FALSE;
	}

	return TRUE;
}

BOOL PlayCommand::IsValidDetail()
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