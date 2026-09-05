#pragma once

#include "TextProgressCtrl.h"
#include <atlcoll.h>
// CListCtrlEx
#define IDC_PROGRESS_LIST WM_USER

class CListCtrlEx : public CListCtrl
{
	DECLARE_DYNAMIC(CListCtrlEx)

	// the array of the Progress Controls in the list control
	ATL::CAtlList<CTextProgressCtrl*> m_ProgressList;
	// the column which should contain the progress bars
	int m_ProgressColumn;
public:
	CListCtrlEx();
	virtual ~CListCtrlEx();

public:
	// initialize the column containing the bars
	void InitProgressColumn(int ColNum=0);
	//void SetProgressRange( int progressIndex, int min, int max ); NotWorking

public:
	int InsertItem(_In_ const LVITEM* pItem);
	int InsertItem(_In_ int nItem, _In_z_ LPCTSTR lpszItem);
	int InsertItem(_In_ int nItem, _In_z_ LPCTSTR lpszItem, _In_ int nImage);
	BOOL SetItemText(_In_ int nItem, _In_ int nSubItem, _In_z_ LPCTSTR lpszText);
	BOOL DeleteItem(_In_ int nItem);
	BOOL DeleteAllItems();

protected:
	CTextProgressCtrl* CreateProgress();

protected:
	CTextProgressCtrl* m_progress;

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
};


