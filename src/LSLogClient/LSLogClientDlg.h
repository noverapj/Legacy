// LSLogClientDlg.h : 헤더 파일
//

#pragma once

#include "resource.h"
#include "NetWork/ioSP2TCPSocket.h"
#include "NetWork/SP2Packet.h"
#include "NetWork/DBClientSocket.h"

#define TIMER_10SECOND 1
#define TIMER_EXIT     2
#define TIMER_PING    3


// CLSLogClientDlg 대화 상자
class CLSLogClientDlg : public CDialog
{
// 생성
public:
	CLSLogClientDlg(CWnd* pParent = NULL);	// 표준 생성자

// 대화 상자 데이터
	enum { IDD = IDD_LSLOGCLIENT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원


// 구현
protected:
	HICON m_hIcon;

protected:
	ioSP2TCPSocket			*m_pTCPSocket;
	ioHashString             m_szIP;
	int                      m_iPort;

	DBClientSocket          *m_pDBSocket;
	ioHashString             m_szDBIP;
	int                      m_iDBPort;

	ioHashString             m_szZoneName;

protected:
	void LoadINI();

	void SetPos(int iX, int iY);

	void CheckLogNewOpen();
	void Write5MinCheck();

	void AutoReconnect();
	void ProcessPing();
	void _AutoReconnect(ioClientTCPSocket *pClientSocket, UINT uMsg, ioHashString &szIP, int iPort);

	void ReleaseAll();

	bool CheckSocketClose(ioClientTCPSocket *pClientSocket);

public:
	const ioHashString& GetIP() const {return m_szIP;}
	int GetPort() const {return m_iPort;}

public:
	void ProcessPacket( SP2Packet &rkPacket );
	void OnConnect( SP2Packet & rkPacket );
	void OnLog( SP2Packet & rkPacket );

public:

	// 메시지 맵 함수를 생성했습니다.
	virtual BOOL OnInitDialog();

	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnClose();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg LONG OnTCPSocket (WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnDBSocket (WPARAM wParam, LPARAM lParam);
	afx_msg void OnDestroy();
	DECLARE_MESSAGE_MAP()
};
