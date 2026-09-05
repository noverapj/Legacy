#include "StdAfx.h"
#include "StopCommand.h"
#include "../resource.h"
#include "../ServerDialog.h"


StopCommand::StopCommand() : BaseCommand(BCT_STOP)
{
}

StopCommand::~StopCommand()
{
}

BOOL StopCommand::ExecuteServiceAll(CServerDialog* dialog)
{
	if( !dialog )
		return FALSE;

	if( CheckContinueServiceAll( dialog ) )
	{
		m_state = CST_ALREADY_EXCUTE;
		return TRUE;
	}
	m_state = CST_BEFORE_EXCUTE;

	if( !dialog->CommandStopAll() )
	{
		return FALSE;
	}

	return TRUE;
}

BOOL StopCommand::ExecuteServiceOne(CServerDialog* dialog)
{
	if( !dialog )
		return FALSE;

	if( CheckContinueServiceOne( dialog ) )
	{
		m_state = CST_ALREADY_EXCUTE;
		return TRUE;
	}
	m_state = CST_BEFORE_EXCUTE;

	if( !dialog->CommandStop( m_strings[0] ) )
	{
		return FALSE;
	}

	return TRUE;
}

BOOL StopCommand::CheckContinueServiceAll(CServerDialog* dialog)
{
	if( !dialog )
		return FALSE;

	int serviceCount = dialog->GetServiceCount();
	for( int index = 0; index < serviceCount ; index ++ )
	{
		if( dialog->IsServiceOn(index) )
		{
			return FALSE;
		}
	}

	return TRUE;
}

BOOL StopCommand::CheckContinueServiceOne(CServerDialog* dialog)
{
	if( !dialog )
		return FALSE;
	
	if( dialog->IsServiceOn(m_strings[0]) )
	{
		return FALSE;
	}

	return TRUE;
}

BOOL StopCommand::IsValidDetail()
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