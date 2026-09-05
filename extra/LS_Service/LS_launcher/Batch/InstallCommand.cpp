#include "StdAfx.h"
#include "InstallCommand.h"
#include "../resource.h"
#include "../LS_launcherDlg.h"
#include "../ServerDialog.h"
#include "../cFile.h"


InstallCommand::InstallCommand(): BaseCommand(BCT_INSTALL)
{
}

InstallCommand::~InstallCommand()
{
}

BOOL InstallCommand::ExecuteServiceAll(CServerDialog* dialog)
{
	return ExecuteServiceOne(dialog);
}

BOOL InstallCommand::ExecuteServiceOne(CServerDialog* dialog)
{
	if( !dialog )
		return FALSE;

	// 이미 설치됐다면?
	if( CheckContinueServiceOne( dialog ) )
	{
		m_state = CST_ALREADY_EXCUTE;
		return TRUE;
	}
	m_state = CST_BEFORE_EXCUTE;

	if( !dialog->CommandInstall( m_strings[0], m_strings[1], m_strings[2] ) )
	{
		return FALSE;
	}

	return TRUE;
}

BOOL InstallCommand::CheckContinueServiceAll(CServerDialog* dialog)
{
	return CheckContinueServiceOne(dialog);
}

BOOL InstallCommand::CheckContinueServiceOne(CServerDialog* dialog)
{
	if( !dialog )
		return FALSE;
	
	int index = dialog->FindServiceIndex( m_strings[0] );
	if( index == -1 )
	{
		return FALSE;
	}

	return TRUE;
}

BOOL InstallCommand::IsValidDetail()
{
	if( m_strings.size() != 3 )
		return FALSE;
	
	cFileReader exeFile;
	if( !exeFile.Open( m_strings[1].GetBuffer() ) )
	{
		return FALSE;
	}
	exeFile.Close();

	cFileReader iniFile;
	if( !iniFile.Open( m_strings[2].GetBuffer() ) )
	{
		return FALSE;
	}
	iniFile.Close();
	return TRUE;
}