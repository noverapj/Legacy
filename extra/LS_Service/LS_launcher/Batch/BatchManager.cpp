#include "StdAfx.h"
#include "Batch.h"
#include "../resource.h"
#include "../BatchDialog.h"
#include "BatchManager.h"


CBatchManager::CBatchManager(void) : m_currentIndex(0), m_batchDlg(NULL), m_current(0), m_elapse(0), m_beginTime(0)
{
	Init();
}

CBatchManager::~CBatchManager(void)
{
	Destroy();
}

void CBatchManager::Init()
{
	m_batchs.reserve( 32 );

	if(!m_batchDlg)
	{
		m_batchDlg = new CBatchDialog();
		m_batchDlg->Create(IDD_BATCH_DIALOG, NULL);
		m_batchDlg->SetWindowPos(&m_batchDlg->wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

		m_consoleMode = FALSE;
	}
}

void CBatchManager::Destroy()
{
	for( unsigned i = 0 ; i < m_batchs.size() ; i++ )
	{
		if( m_batchs[i] )
		{
			delete m_batchs[i];
		}
	}
	m_batchs.clear();
}

void CBatchManager::ShowDialog()
{
	if(m_batchDlg)
	{
		CString log;
		log.Format(_T("Elapsed : %lu s"), 0);
		m_batchDlg->UpdateBatchResult(log);

		m_batchDlg->CenterWindow();
		m_batchDlg->ShowWindow(SW_SHOW);
	}
}

void CBatchManager::End()
{
}

BOOL CBatchManager::AddBatchFile( TCHAR* batchFileName, CLS_launcherDlg* launcher )
{
	CString checkNull = batchFileName;

	if(checkNull.IsEmpty())
		return FALSE;

	Batch* batch = new Batch;

	if( !batch->OpenFile( batchFileName ) )
	{
		delete batch;
		Error(_T("File open"), _T("failed"), TRUE);
		return FALSE;
	}

	if( !batch->ScanProcess( &m_Scanner, launcher ) )
	{
		delete batch;
		Error(_T("Command parsing"), _T("failed"));
		return FALSE;
	}

	m_batchs.push_back( batch );
	return TRUE;
}

BOOL CBatchManager::Execute()
{
	if( m_batchs.empty() )
		return FALSE;

	if( m_currentIndex >= m_batchs.size() )
		return TRUE;

	for( m_currentIndex ; m_currentIndex < m_batchs.size() ; m_currentIndex++ )
	{
		Batch *batch = m_batchs[m_currentIndex];
		if( !batch ) continue;

		if( !batch->Execute(this) )
		{
			return FALSE;
		}
	}
	return TRUE;
}

BOOL CBatchManager::CheckContinue()
{
	if( m_batchs.empty() )
		return FALSE;

	if( m_currentIndex >= m_batchs.size() )
		return FALSE;

	return m_batchs[m_currentIndex]->CheckContinue();
}

BOOL CBatchManager::Next()
{
	if( m_currentIndex >= m_batchs.size() )
		return FALSE;

	if( !m_batchs[m_currentIndex]->Next() )
	{
		m_currentIndex++;
		if( m_currentIndex >= m_batchs.size() )
			return FALSE;
	}

	return TRUE;
}

BOOL CBatchManager::HaveBaches()
{
	if(m_batchs.size() > 0) return TRUE;
	return FALSE;
}

int CBatchManager::BeginProgress(const TCHAR* title, const int progress, const TCHAR* result)
{
	m_beginTime = GetTickCount();
	m_batchDlg->BeginProgress(title, progress, result, m_current);
	return m_current;
}

void CBatchManager::DoingProgress(const int progress)
{
	m_batchDlg->DoingProgress(m_current, progress);
}

void CBatchManager::ShowExecuteResult(const TCHAR* result, BOOL quit)
{
	if(IsConsoleMode())
	{
		_tprintf(_T("\t\t %s \n"), result); 
	}
}

void CBatchManager::EndProgress(const int progress, const TCHAR* result, BOOL quit)
{
	DWORD elapse = GetTickCount() - m_beginTime;
	m_elapse += elapse;

	CString log;
	log.Format(_T("Elapsed : %0.2lf s"), (double)m_elapse / 1000.0);
	m_batchDlg->EndProgress(m_current, progress, result);
	m_batchDlg->UpdateBatchResult(log);

	ShowExecuteResult(result, quit);
}

void CBatchManager::EndProgress(const int progress, const TCHAR* result)
{
	DWORD elapse = GetTickCount() - m_beginTime;
	m_elapse += elapse;

	CString log;
	log.Format(_T("Elapsed : %0.2lf s"), (double)m_elapse / 1000.0);
	m_batchDlg->EndProgress(m_current, progress, result);
	m_batchDlg->UpdateBatchResult(log);
}

void CBatchManager::Error(const TCHAR* title, const TCHAR* result, BOOL quit)
{
	BeginProgress(title, 0, result);
	EndProgress(0, result);

	

	if(IsConsoleMode())
	{
		_tprintf(_T("%s %s \n"), title, result); 
	}
	else
		ShowDialog();

}


void CBatchManager::ResetExecute()
{
	for( UINT i = 0 ; i < m_batchs.size() ; i ++ )
	{
		Batch* batch = m_batchs[i];
		if( !batch ) continue;

		batch->ResetExecute();
	}

	m_currentIndex = 0;
}

DWORD CBatchManager::GetCurrentState()
{
	if( m_currentIndex >= m_batchs.size() )
		return CST_UNKNOWN_COMMAND;

	return m_batchs[m_currentIndex]->GetCurrentState();
}

void CBatchManager::SetConsoleMode(BOOL mode)
{
	m_consoleMode = mode;
}

BOOL CBatchManager::IsConsoleMode()
{
	return m_consoleMode;
}
