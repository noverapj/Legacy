#pragma once


// CLauncherListCtrl

class CLauncherListCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(CLauncherListCtrl)

public:
	CLauncherListCtrl();
	virtual ~CLauncherListCtrl();

public:
	void CreatePopup();

	int GetSelectedIndex();

	void Save();
	void Restore();

protected:
	CMenu m_menu;

	int m_topIndex;
	int m_currentIndex;

protected:
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnNMRClick(NMHDR *pNMHDR, LRESULT *pResult);

protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
};


