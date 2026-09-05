#pragma once
#include <set>
#include "BaseCommand.h"

class SyncCommand : public BaseCommand
{
public:
	SyncCommand();
	~SyncCommand();

public:
	BOOL ExecuteServiceAll(CServerDialog* dialog);
	BOOL ExecuteServiceOne(CServerDialog* dialog);
	
	BOOL CheckContinueServiceAll(CServerDialog* dialog);
	BOOL CheckContinueServiceOne(CServerDialog* dialog);

	BOOL IsValidDetail();

private:
	typedef std::set<CServerDialog*>	SETDIALOGS;
	typedef SETDIALOGS::iterator		DIALOGSITER;

	void PushCompleteDialogs( SETDIALOGS& dialogs, CServerDialog* dialog );
	BOOL CheckCompleteDialogs( SETDIALOGS& dialogs, CServerDialog* dialog );

private:
	UINT m_totalSyncTabCount;

	SETDIALOGS	m_syncCommandDialogs;
	SETDIALOGS	m_syncComplteDialogs;
};