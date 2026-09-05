#pragma once


// CMonitorCtrl

class CMonitorCtrl : public CProgressCtrl
{
	DECLARE_DYNAMIC(CMonitorCtrl)

public:
	CMonitorCtrl();
	virtual ~CMonitorCtrl();

public:
	void InitToolTip();
	void SetToolTip(CString text);

protected:
	CToolTipCtrl m_toolTip;

protected:
	DECLARE_MESSAGE_MAP()

	BOOL PreTranslateMessage(MSG* pMsg);
public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};


