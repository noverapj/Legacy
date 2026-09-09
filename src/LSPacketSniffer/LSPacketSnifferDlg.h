
#pragma once

#include "CaptureEngine.h"
#include "PacketStore.h"
#include "PacketDecoder.h"

#include <vector>

#define LSSNIFF_TIMER_DRAIN 1

class CLSPacketSnifferDlg : public CDialogEx
{
public:
	CLSPacketSnifferDlg(CWnd* pParent = nullptr);

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LSPACKETSNIFFER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);

protected:
	HICON m_hIcon;

	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnBnClickedStart();
	afx_msg void OnBnClickedClear();
	afx_msg void OnBnClickedExport();
	afx_msg void OnBnClickedSelftest();
	afx_msg void OnBnClickedFind();
	afx_msg void OnBnClickedFamily(UINT nID);
	afx_msg void OnBnClickedStrictCS();
	afx_msg void OnLvnGetdispinfoList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLvnItemchangedList(NMHDR* pNMHDR, LRESULT* pResult);
	DECLARE_MESSAGE_MAP()

	virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
	CComboBox       m_comboDevice;
	CStatic         m_stFilter;
	CEdit           m_editFilter;
	CStatic         m_stPort;
	CEdit           m_editServerPort;
	CButton         m_btnStart;
	CButton         m_btnClear;
	CButton         m_btnExport;
	CButton         m_btnSelfTest;
	CButton         m_chkFamily[5];
	CButton         m_chkStrictCS;
	CStatic         m_stSearch;
	CEdit           m_editSearch;
	CButton         m_btnFind;
	CListCtrl       m_list;
	CEdit           m_editDetail;
	CStatusBarCtrl  m_status;
	CFont           m_fontMono;

	static const wchar_t* FAMILY_LABELS[5];

protected:
	std::vector<CaptureDeviceInfo> m_devices;
	std::vector<DecodedPacket> m_packets;
	std::vector<int> m_visIdx;
	bool m_bFamily[5];
	PacketStore m_store;
	CaptureEngine m_engine;
	bool m_bStarted;

protected:
	void LayoutControls(int cx, int cy);
	void RefreshDeviceList();
	void UpdateDetail(int iItem);
	void AppendLogLine(const std::wstring& s);
	void UpdateStatusBar();
	bool IsFamilyVisible(const DecodedPacket& dp) const;
	void RebuildVisibleIndex();
	bool PacketMatchesSearch(const DecodedPacket& dp, const CString& csNeedle) const;
	static int FamilyIndexOf(const DecodedPacket& dp);
	static std::vector<WORD> ParsePortList(LPCTSTR sz);

	static std::wstring FormatTime(const SYSTEMTIME& st);
	static std::wstring FormatAddr(DWORD dwIP, WORD wPort);
	static std::wstring FormatDir(const DecodedPacket& dp);
};
