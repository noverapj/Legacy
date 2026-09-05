#pragma once
#include <vector>
#include "BatchCompileDefine.h"
#include "FileReader.h"

class BatchScanner;
class CLS_launcherDlg;
class BaseCommand;
class CBatchManager;

using namespace std;

class Batch
{
public:
	typedef	vector<BaseCommand*>	VECCOMMANDS;
public:
	Batch(void);
	~Batch(void);

private:
	void Init();
	void Destroy();

public:
	BOOL OpenFile( TCHAR* fileName );
	BOOL ScanProcess( BatchScanner* scanner, CLS_launcherDlg* launcher  );

	BOOL Execute(CBatchManager* batchManager);
	BOOL CheckContinue();
	
	BOOL Next();

public:
	void ResetExecute()	{ m_currentIndex = 0; }

	DWORD GetCurrentState();

private:
	FileReader m_file;
	VECCOMMANDS	m_commands;

	UINT m_currentIndex;
};
