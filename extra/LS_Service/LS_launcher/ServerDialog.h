#pragma once

#include "afxwin.h"
#include "cBuffer.h"
#include "cPacket.h"
#include "ClientSocket.h"
#include "LauncherListCtrl.h"
#include "ProcessCtrl.h"
#include <string>
#include <vector>
#include "afxcmn.h"

class CLS_launcherDlg;
class CPerfMonDlg;

typedef std::vector<int> UNITS;

typedef struct _FOLDERSTRING_
{
	CString m_folder;
	CStringArray m_folders;
	CStringArray m_files;
}FOLDERSTRING;

// CServerDialog 대화 상자입니다.
enum StatusIcons
{
	STATUS_ON = 0,
	STATUS_OFF,
};

enum ResourceTypes
{
	RESOURCE_TYPE_CPU = 1,
	RESOURCE_TYPE_RAM,
	RESOURCE_TYPE_DISKC,
	RESOURCE_TYPE_DISKD,
	RESOURCE_TYPE_UPLOAD,
	RESOURCE_TYPE_DOWNLOAD,

	RESOURCE_TYPE_END
};

class CServerDialog : public CDialog
{
	DECLARE_DYNAMIC(CServerDialog)

public:
	CServerDialog(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CServerDialog();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_SERVERINFO_DIALOG };

public:
	void Init(CString& title, CString& IP, UINT port, RECT pos);
	void InitControls();

	void CreatePopup();
	BOOL CreateSocket(CString& IP, UINT port);
	BOOL CloseSocket();
	BOOL IsConnected();

	void SendPacket(const UINT command, const void* buffer, const UINT length);

	void Reset();
	void Next(BOOL success = TRUE);
	BOOL TransferFile();
	BOOL TransferBegin();
	BOOL TransferDo();
	BOOL TransferEnd();
	
	CString GetTitle()		{ return m_name; }
	//BOOL ReadFile();
	void SetAuto(BOOL b)	{ m_auto = b; }
	BOOL GetAuto()			{ return m_auto; }

	BOOL IsMouseOverTab(LONG x, LONG y);
	void SetTabRect(RECT rect) { m_tapRect = rect; }

	void SendMsgDlg( TCHAR * serviceName, int cmd );
	void SendMsgDlg( int cmd );
	void SendMsgDlg( FOLDERSTRING& sttmp);
	void ServicePlay(TCHAR* serviceName, BOOL console);
	void ServiceStop(TCHAR* serviceName, BOOL console);

public:
	void RequestServices();
	void RequestPeformances();
	void RequestReboot();
	void RequestInstall();
	void RequestPlayAll();
	void RequestStopAll();
	void RequestFileSync();
	void RequestFileRemove();
	void RequestFileBackup();

protected:
	void FindFiles(CString folder, CStringArray& files, CStringArray& folders);
	void UpdateRoot();
	void ResetApplyAll();

	void Log(const TCHAR *format, ...);
	int GetIndex(const TCHAR* service);

	void SetCPU(UNITS& units);
	void SetMemory(UNITS& units);
	void SetNetwork(UNITS& units);
	void SetDisk(UNITS& units);
	void SetSyncComplete(BOOL result);

public:
	void SyncCommandStart( CString& syncFolder, BOOL recursive );
	void SyncCommandEnd();
	void SyncFindFiles( BOOL recursive );

	BOOL CommandFileSync( CString& syncFolder, BOOL recursive = TRUE );
	BOOL CommandInstall( CString& service, CString& demon, CString& script );
	BOOL CommandUninstall( CString& service );
	BOOL CommandPlay( CString& service );
	BOOL CommandStop( CString& service );
	BOOL CommandPlayAll();
	BOOL CommandStopAll();
	BOOL CommandUninstallAll();

	int FindServiceIndex( CString& service );	
	BOOL IsServiceOn( int index );
	BOOL IsServiceOn( CString& service );
	BOOL IsReady()			{ return m_ready; }
	BOOL IsSyncComplete()	{ return m_syncComplete; }

	int GetServiceCount()	{ return m_services.GetItemCount(); }

protected:
	CClientSocket* m_socket;
	UINT m_sequence;
	CString m_name;
	CMenu m_menu;

	HBITMAP m_statusIcon[2];

	// 복사할 파일목록들
	CString m_folder;
	CStringArray m_folders;
	CStringArray m_files;
	UINT m_fileIndex;
	UINT m_fileOffset;
	cBuffer m_buffer;
	cBuffer m_send;
	DWORD m_begin;
	UINT m_maxCount;
	UINT m_fileCount;
	UINT m_fileLength;
	CString m_filePath;
	CString m_fileName;
	CString m_root;
	cPacket m_packet;
	BOOL m_auto;
	CMsgBox m_msgbox;

	CImageList m_imageList;
	RECT m_tapRect;		// 해당 탭의 위치정보
	CLS_launcherDlg* m_launcherDlg;
	CPerfMonDlg* m_perfMonDlg;

	CProgressCtrl m_progressBar;
	FOLDERSTRING m_stlocal;
	CLauncherListCtrl m_services;

	CProcessCtrl m_ctlCPU, m_ctlRAM, m_ctlUpload, m_ctlDownload, m_ctlDisk, m_ctlDisk2;
	UNITS m_CPUs, m_Memorys, m_Networks, m_Disks;
	int maxResource[RESOURCE_TYPE_END];

	// Batch Command 관련
	BOOL m_syncComplete;
	BOOL m_ready;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	CStatic m_nameStatic;
	CStatic m_status;
	CStatic m_log;
	CStatic m_barCPU;
	CStatic m_barRAM;
	CStatic m_barUpload;
	CStatic m_barDownload;
	CStatic m_barDisk;
	CStatic m_barDisk2;

	DECLARE_MESSAGE_MAP()

	afx_msg void OnBnClickedButtonUpdate();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnClose();
	afx_msg void OnBnClickedButtonRefresh();
	afx_msg void OnBnClickedButtonPlayAll();
	afx_msg void OnBnClickedButtonStopAll();
	afx_msg void OnBnClickedButtonRemove();
	afx_msg void OnBnClickedButtonBackup();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);

	afx_msg LRESULT OnConnect(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDisconnect(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnReceive(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnPeformanceMonitor(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnPlay(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnStop(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnPlayConsole(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnStopConsole(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUninstall(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnAutomatic(WPARAM wParam, LPARAM lParam);

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam); 
	
};
