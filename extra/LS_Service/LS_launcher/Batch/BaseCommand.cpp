#include "StdAfx.h"
#include "BaseCommand.h"
#include "../resource.h"
#include "../LS_launcherDlg.h"
#include "../LauncherTabCtrl.h"


BaseCommand::BaseCommand(DWORD type) : m_commandType(type)
{
	Init();
}

BaseCommand::~BaseCommand(void)
{
	Destroy();
}


void BaseCommand::Init()
{
	m_launcher		= NULL;
	m_isServiceAll	= FALSE;
	m_isApplyAll	= FALSE;
	m_state			= CST_BEFORE_EXCUTE;
	m_error			= CET_NOERROR;
}

void BaseCommand::Destroy()
{
}

BOOL BaseCommand::Execute()
{
	if( NULL == m_launcher )
	{
		m_error = CET_WRONG_NO_LAUNCHER;
		return FALSE;
	}

	if( m_isApplyAll )
	{
		if( m_isServiceAll )
		{
			for( int i = 0 ; i < GetDialogCount() ; i++ )
			{
				CServerDialog* dialog = GetDialog(i);
				if( !ExecuteServiceAll( dialog ) )
					return FALSE;
			}
		}
		else
		{
			for( int i = 0 ; i < GetDialogCount() ; i++ )
			{
				CServerDialog* dialog = GetDialog(i);
				if( !ExecuteServiceOne( dialog ) )
					return FALSE;
			}
		}
	}
	else
	{
		if( m_isServiceAll )
		{
			for( UINT i = 0 ; i < m_tabIndexes.size() ; i++ )
			{
				CServerDialog* dialog = GetDialog(m_tabIndexes[i]);
				if( !ExecuteServiceAll(dialog) )
					return FALSE;
			}
		}
		else
		{
			for( UINT i = 0 ; i < m_tabIndexes.size() ; i++ )
			{
				CServerDialog* dialog = GetDialog(m_tabIndexes[i]);
				if( !ExecuteServiceOne(dialog) )
					return FALSE;
			}
		}
	}
	if( m_isWait && m_state != CST_ALREADY_EXCUTE )
	{
		m_state = CST_WAITING;
		m_launcher->BatchWait();
		return FALSE;
	}

	m_state = CST_EXCUTE_DONE;
	return TRUE;
}

BOOL BaseCommand::CheckContinue()
{
	if( NULL == m_launcher )
	{
		m_error = CET_WRONG_NO_LAUNCHER;
		return FALSE;
	}

	if( m_state != CST_WAITING )
		return FALSE;
	
	if( m_isApplyAll )
	{
		if( m_isServiceAll )
		{
			for( int i = 0 ; i < GetDialogCount() ; i++ )
			{
				CServerDialog* dialog = GetDialog(i);
				if( !CheckContinueServiceAll(dialog)  )
				{
					return FALSE;
				}
			}
		}
		else
		{
			for( int i = 0 ; i < GetDialogCount() ; i++ )
			{
				CServerDialog* dialog = GetDialog(i);
				if( !CheckContinueServiceOne(dialog)  )
				{
					return FALSE;
				}
			}
		}
	}
	else
	{
		if( m_isServiceAll )
		{
			for( UINT i = 0 ; i < m_tabIndexes.size() ; i++ )
			{
				CServerDialog* dialog = GetDialog(m_tabIndexes[i]);
				if( !CheckContinueServiceAll(dialog) )
					return FALSE;
			}
		}
		else
		{
			for( UINT i = 0 ; i < m_tabIndexes.size() ; i++ )
			{
				CServerDialog* dialog = GetDialog(m_tabIndexes[i]);
				if( !CheckContinueServiceOne(dialog) )
					return FALSE;
			}
		}
	}

	m_state = CST_EXCUTE_DONE;
	return TRUE;
}

BOOL BaseCommand::IsValid()
{
	// 전체적으로 체크할 오류?..
	if( !m_launcher )
		return FALSE;

	// Apply All 처리 - TabIndex가 있으면 ApplyAll이 아니다
	if( m_tabIndexes.empty() )
	{
		m_isApplyAll = TRUE;
	}
	else
	{
		m_isApplyAll = FALSE;
	}
	
	return IsValidDetail();
}

void BaseCommand::SetOption( DWORD optionBit )
{
	m_isWait = FALSE;
	if( BITCHK(optionBit, BOT_SERVICE_WAIT) )
	{
		m_isWait = TRUE;
	}

	m_isServiceAll = FALSE;
	if( BITCHK(optionBit, BOT_ALL_SERVICE) )
	{
		m_isServiceAll = TRUE;
	}
}

void BaseCommand::SetCommandText(const char* commandLine)
{
	TCHAR tcommandLine[_MAX_FNAME];
#ifdef UNICODE
	MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, commandLine, strlen(commandLine)+1, tcommandLine, _MAX_FNAME-1 );
#else
	strcpy_s( tstring, string );
#endif
	
	m_commandText.Format( _T("%s"), tcommandLine );
}

void BaseCommand::AddString( const char* string, int len )
{
	TCHAR	tstring[_MAX_FNAME];
	CString temp;

#ifdef UNICODE
	MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, string, len, tstring, _MAX_FNAME-1 );
	tstring[len] = 0;
#else
	strcpy_s( tstring, string );
#endif

	temp.Format( _T("%s"), tstring );
	m_strings.push_back( temp );
}

CServerDialog* BaseCommand::GetDialog(int index)
{
	CServerDialog* dlg = m_launcher->GetTabDialog(index);
	if(dlg)
	{
		m_launcher->m_serverTabs.UserFocus(index);
	}
	return dlg;
}

int BaseCommand::GetDialogCount()
{
	return m_launcher->GetTabDialogCount();
}
