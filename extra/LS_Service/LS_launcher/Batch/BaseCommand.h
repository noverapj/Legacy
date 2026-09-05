#pragma once
#include <vector>
#include "BatchCompileDefine.h"

class CLS_launcherDlg;
class CServerDialog;

class BaseCommand
{
public:
	BaseCommand(DWORD type);
	virtual ~BaseCommand(void);

private:
	void Init();
	void Destroy();

public:
	BOOL Execute();
	BOOL CheckContinue();
	
	BOOL IsValid();

	void SetOption( DWORD optionBit );

	void SetCommandText(const char* commandLine);

protected:
	virtual BOOL ExecuteServiceAll(CServerDialog* dialog) = 0;
	virtual BOOL ExecuteServiceOne(CServerDialog* dialog) = 0;
	
	virtual BOOL CheckContinueServiceAll(CServerDialog* dialog) = 0;
	virtual BOOL CheckContinueServiceOne(CServerDialog* dialog) = 0;

	virtual BOOL IsValidDetail() = 0;

public:
	DWORD GetType()		{ return m_commandType; }

	void SetLauncher( CLS_launcherDlg* launcher )	{ m_launcher = launcher; }
	void AddIndex( int index )						{ m_tabIndexes.push_back( index ); }
	void AddString( const char* string, int len );

	CServerDialog* GetDialog(int index);
	int GetDialogCount();

	DWORD GetState()		{ return m_state; }
	DWORD GetError()		{ return m_error; }
	CString GetCommand()	{ return m_commandText; }

protected:
	const DWORD	m_commandType;
	CString		m_commandText;

	CLS_launcherDlg* m_launcher;

	VECINT		m_tabIndexes;
	VECCSTRING	m_strings;

	BOOL m_isWait;
	BOOL m_isServiceAll;
	BOOL m_isApplyAll;

	DWORD m_state;
	DWORD m_error;
};