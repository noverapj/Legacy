#include "StdAfx.h"
#include "SyncCommand.h"
#include "../resource.h"
#include "../LS_launcherDlg.h"
#include "../ServerDialog.h"
#include <direct.h>


SyncCommand::SyncCommand() : BaseCommand(BCT_SYNC)
{
}

SyncCommand::~SyncCommand()
{
}

BOOL SyncCommand::ExecuteServiceAll(CServerDialog* dialog)
{
	return ExecuteServiceOne(dialog);
}

BOOL SyncCommand::ExecuteServiceOne(CServerDialog* dialog)
{
	if( !dialog )
		return FALSE;

	if( CheckCompleteDialogs( m_syncCommandDialogs, dialog ) )
	{
		m_state = CST_ALREADY_EXCUTE;
		return TRUE;
	}
	m_state = CST_BEFORE_EXCUTE;

	if( m_syncCommandDialogs.size() != m_syncComplteDialogs.size() )
	{
		return TRUE;
	}

	if( !dialog->CommandFileSync( m_strings[0] ) )
	{
		return FALSE;
	}

	PushCompleteDialogs( m_syncCommandDialogs, dialog );
	return TRUE;
}

BOOL SyncCommand::CheckContinueServiceAll(CServerDialog* dialog)
{
	return CheckContinueServiceOne(dialog);
}

BOOL SyncCommand::CheckContinueServiceOne(CServerDialog* dialog)
{
	if( !dialog )
		return FALSE;

	if( !dialog->IsSyncComplete() )
	{
		return FALSE;
	}
	
	if( CheckCompleteDialogs( m_syncComplteDialogs, dialog ) )
	{
		return TRUE;
	}

	PushCompleteDialogs( m_syncComplteDialogs, dialog );

	if( m_syncComplteDialogs.size() == m_totalSyncTabCount )
		return TRUE;

	return BaseCommand::Execute();
}

BOOL SyncCommand::IsValidDetail()
{
	if( m_strings.size() != 1 )
	{
		return FALSE;
	}

	TCHAR tPath[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, tPath);

	if( _tchdir(m_strings[0].GetBuffer()) == -1 )
	{
		return FALSE;
	}

	_tchdir( tPath );
	GetCurrentDirectory(MAX_PATH, tPath);

	// FileSync는 무조건 WaitMode로 한다
	m_isWait = TRUE;

	if( m_isApplyAll )
	{
		m_totalSyncTabCount = GetDialogCount();
	}
	else
	{
		m_totalSyncTabCount = m_tabIndexes.size();
	}
	m_syncComplteDialogs.clear();

	return TRUE;
}

void SyncCommand::PushCompleteDialogs( SETDIALOGS& dialogs, CServerDialog* dialog )
{
	DIALOGSITER	it = dialogs.find( dialog );

	if( it == dialogs.end() )
	{
		dialogs.insert( dialog );
	}
}

BOOL SyncCommand::CheckCompleteDialogs( SETDIALOGS& dialogs, CServerDialog* dialog )
{
	DIALOGSITER	it = dialogs.find( dialog );
	if( it != dialogs.end() )
	{
		return TRUE;
	}

	return FALSE;
}