#include "stdafx.h"
#include "Batch.h"
#include "BatchManager.h"
#include "BatchScanner.h"


Batch::Batch(void)
{
	Init();
}

Batch::~Batch(void)
{
	Destroy();
}

void Batch::Init()
{
	m_commands.reserve( 32 );
	m_currentIndex = 0;
}

void Batch::Destroy()
{
	for( unsigned i = 0 ; i < m_commands.size() ; i++ )
	{
		if( m_commands[i] )
		{
			delete m_commands[i];
		}
	}
	m_commands.clear();
}

BOOL Batch::OpenFile( TCHAR* fileName )
{
	return m_file.OpenFile( fileName );
}

BOOL Batch::ScanProcess( BatchScanner* scanner, CLS_launcherDlg* launcher )
{
	if( NULL == scanner )
		return FALSE;

	char line[_MAX_FNAME];
	while( true )
	{
		if( !m_file.GetLine( line, _MAX_FNAME ) )
		{
			break;
		}

		BaseCommand* command = scanner->Scanner(line);

		if( !command )
		{
			Destroy();
			return FALSE;
		}

		command->SetLauncher( launcher );
		if( !command->IsValid() )
		{
			delete command;
			command = NULL;
			Destroy();
			return FALSE;
		}

		m_commands.push_back( command );
	}

	if( m_commands.empty() )
		return FALSE;

	return TRUE;
}

BOOL Batch::Execute(CBatchManager* batchManager)
{
	if( m_commands.empty() )
		return FALSE;

	for( m_currentIndex ; m_currentIndex < m_commands.size() ; m_currentIndex++ )
	{
		BaseCommand* command = m_commands[m_currentIndex];
		if( !command )	continue;

		int index = 0;
		batchManager->BeginProgress(command->GetCommand(), 0, _T("..."));
		if(batchManager->IsConsoleMode())
			_tprintf(command->GetCommand());

		if( !command->Execute() )
		{
			return FALSE;
		}
		batchManager->EndProgress( 100, _T("completed"),FALSE);
	}

	return TRUE;
}

BOOL Batch::CheckContinue()
{
	if( m_commands.empty() )
		return FALSE;

	if( m_currentIndex >= m_commands.size() )
		return FALSE;
	
	return m_commands[m_currentIndex]->CheckContinue();
}

BOOL Batch::Next()
{
	m_currentIndex++;
	if( m_currentIndex >= m_commands.size() )
		return FALSE;

	return TRUE;
}

DWORD Batch::GetCurrentState()
{
	if( m_currentIndex >= m_commands.size() )
		return CST_UNKNOWN_COMMAND;

	return m_commands[m_currentIndex]->GetState();
}