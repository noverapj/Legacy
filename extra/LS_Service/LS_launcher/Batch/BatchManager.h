#pragma once
#include "BatchScanner.h"

class Batch;
class CLS_launcherDlg;
class CBatchDialog;

using namespace std;

class CBatchManager
{
public:
	typedef vector<Batch*>	BATCHES;

public:
	CBatchManager(void);
	~CBatchManager(void);

private:
	void Init();
	void Destroy();

public:
	void ShowDialog();
	void End();

public:
	BOOL AddBatchFile( TCHAR* batchFileName, CLS_launcherDlg* launcher );
	BOOL Execute();
	BOOL CheckContinue();
	BOOL Next();

	BOOL HaveBaches();

public:
	int BeginProgress(const TCHAR* title, const int progress, const TCHAR* result);
	void DoingProgress(const int progress);
	void EndProgress(const int progress, const TCHAR* result);
	void EndProgress(const int progress, const TCHAR* result, BOOL quit);
	void Error(const TCHAR* title, const TCHAR* result, BOOL quit=NULL);

public:
	void ResetExecute();
	DWORD GetCurrentState();

	void SetConsoleMode(BOOL mode);
	BOOL IsConsoleMode();

	void ShowExecuteResult(const TCHAR* result, BOOL quit);

protected:
	BatchScanner	m_Scanner;
	BATCHES			m_batchs;
	UINT			m_currentIndex;
	CBatchDialog*	m_batchDlg;

	int				m_current;
	int				m_elapse;
	DWORD			m_beginTime;
	BOOL			m_consoleMode;
};