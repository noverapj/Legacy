#pragma once

#include <vector>
#include <map>
#include "ServerDialog.h"


// CLauncherTabCtrl

class CLauncherTabCtrl : public CTabCtrl
{
	DECLARE_DYNAMIC(CLauncherTabCtrl)

public:
	CLauncherTabCtrl();
	virtual ~CLauncherTabCtrl();

public:
	void Init();
	void AddTab(CString& title, CString& IP, UINT port);
	void DeleteTab(const UINT focus);
	void SetFocus(const UINT index);
	void UserFocus(const UINT index);
	void CreatePopup();
	UINT GetTabFocus(LONG x, LONG y);

public:
	BOOL IsAllReady();

public:
	const UINT GetTabIndex()	{ return m_tabIndex; }
	const UINT GetCount()		{ return m_dialogs.size(); }

	void ShowTab(const UINT index, const BOOL show = TRUE);
	
public:
	CServerDialog* GetDialog(const int index);

private:
	void TapRenewal(const UINT focus);
	void DeleteServerInfo(CString name);

public:
	typedef std::map<UINT, CServerDialog*> SERVER_DIALOGS;		
	SERVER_DIALOGS m_dialogs;

protected:
	UINT m_tabIndex;
	CMenu m_menu;
	int m_x, m_y, m_width, m_height;

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);

protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
};


