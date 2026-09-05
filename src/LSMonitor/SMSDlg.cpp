// SMSDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "LSMonitor.h"
#include "LSMonitorDlg.h"
#include "SMSDlg.h"
#include ".\smsdlg.h"
#include <strsafe.h>

// CSMSDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CSMSDlg, CDialog)
CSMSDlg::CSMSDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSMSDlg::IDD, pParent)
{
	m_iZoneArray = 0;
}

CSMSDlg::~CSMSDlg()
{
}

void CSMSDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_SERVER, m_ComboBoxSelectServer);
	DDX_Control(pDX, IDC_EDIT_CUSTOM_STOP, m_EditCustomStopMin);
}


BEGIN_MESSAGE_MAP(CSMSDlg, CDialog)
	ON_BN_CLICKED(IDSTART, OnBnClickedStart)
	ON_BN_CLICKED(IDSTOP1H, OnBnClickedOnehourstop)
	ON_BN_CLICKED(IDC_CUSTOMSTOP, OnBnClickedCustomstop)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedDefaultCancel) // esc 종료 하지 못하게
	ON_MESSAGE(WM_TO_PARENT_NORMAL, OnSMSNormal)
	ON_MESSAGE(WM_TO_PARENT_STOP, OnSMSStop)
	ON_WM_SHOWWINDOW()
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CSMSDlg 메시지 처리기입니다.
void CSMSDlg::OnBnClickedStart()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CLSMonitorDlg * pDlg = (CLSMonitorDlg*) AfxGetApp()->m_pMainWnd;
	if( !pDlg ) return;

	int iSMSArray = pDlg->GetSMSArray( m_iZoneArray, m_ComboBoxSelectServer.GetCurSel() );

	if( iSMSArray == -1 )
	{
		int iSize = pDlg->GetCurSMSInfoSize(m_iZoneArray);
		for (int i = 0; i < iSize ; i++)
		{
			int iCurSMSArray = pDlg->GetSMSArray( m_iZoneArray, i );
			if( iCurSMSArray != -1)
				m_SMSRuleManager.ClearStates(iCurSMSArray);
		}
	}
	else
		m_SMSRuleManager.ClearStates(iSMSArray);

	pDlg->UpdateListItem( iSMSArray, STATE_NORMAL);	
	LOG.PrintTimeAndLog(0, "Manual Start:%d",iSMSArray);
}

void CSMSDlg::OnBnClickedOnehourstop()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CLSMonitorDlg * pDlg = (CLSMonitorDlg*) AfxGetApp()->m_pMainWnd;
	if( !pDlg ) return;

	int iSMSArray = pDlg->GetSMSArray( m_iZoneArray, m_ComboBoxSelectServer.GetCurSel() );

	if( iSMSArray == -1 )
	{
		int iSize = pDlg->GetCurSMSInfoSize(m_iZoneArray);
		for (int i = 0; i < iSize ; i++)
		{
			int iCurSMSArray = pDlg->GetSMSArray( m_iZoneArray, i );
			if( iCurSMSArray != -1)
				m_SMSRuleManager.SetStopMonitering( 3600000, iCurSMSArray);
		}
	}
	else
		m_SMSRuleManager.SetStopMonitering(3600000,iSMSArray);

	CString szFinalText;
	szFinalText.Format("%s:60min", STATE_STOP_MONITOR);
	pDlg->UpdateListItem( iSMSArray , (LPCTSTR)szFinalText);
	LOG.PrintTimeAndLog(0, "Manual STOP-1:%d:60",iSMSArray);
}


void CSMSDlg::OnBnClickedCustomstop()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	char szText[MAX_PATH]="";
	m_EditCustomStopMin.GetWindowText(szText, sizeof(szText));
	if(strcmp(szText, "") == 0 || strcmp(szText, "0") == 0)
		return;

	int iMinute = atoi(szText);
	DWORD dwStopTime = (iMinute * 60) * 1000; 

	CLSMonitorDlg * pDlg = (CLSMonitorDlg*) AfxGetApp()->m_pMainWnd;
	if( !pDlg ) return;

	int iSMSArray = pDlg->GetSMSArray( m_iZoneArray, m_ComboBoxSelectServer.GetCurSel() );

	if( iSMSArray == -1 )
	{
		int iSize = pDlg->GetCurSMSInfoSize(m_iZoneArray);
		for (int i = 0; i < iSize ; i++)
		{
			int iCurSMSArray = pDlg->GetSMSArray( m_iZoneArray, i );
			if( iCurSMSArray != -1)
				m_SMSRuleManager.SetStopMonitering(dwStopTime, iCurSMSArray);
		}
	}
	else
		m_SMSRuleManager.SetStopMonitering(dwStopTime, iSMSArray);

	CString szFinalText;
	szFinalText.Format("%s:%dmin", STATE_STOP_MONITOR,iMinute);
	pDlg->UpdateListItem( iSMSArray , (LPCTSTR)szFinalText);
	LOG.PrintTimeAndLog(0, "Manual STOP-2:%d:%d", iSMSArray, iMinute);
}

BOOL CSMSDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_EditCustomStopMin.SetLimitText(3);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}



SMSRULESTATE CSMSDlg::GetState( int iSMSArray )
{
	return m_SMSRuleManager.GetState(iSMSArray);
}

void CSMSDlg::_OnTimerSMS()
{
	m_SMSRuleManager._OnTimer();

	// 1분체크
	static int i10secondsCnt = 0;
	if(i10secondsCnt >= 6)
	{
		i10secondsCnt = 0;
		//
		for(int i = 0; i < m_SMSRuleManager.GetSize(); i++)
		{
			if(m_SMSRuleManager.GetState(i) == STOPING)
			{
				int iMinute = ( (m_SMSRuleManager.GetRemainWaitTime(i)/1000) / 60 );
				CString szText;
				szText.Format("%s:%dmin", STATE_STOP_MONITOR, iMinute);
				CLSMonitorDlg * pDlg = (CLSMonitorDlg*) AfxGetApp()->m_pMainWnd;
				if( pDlg ) 
					pDlg->UpdateListItem( i , (LPCTSTR)szText);
			}
		}
		//
	}
	i10secondsCnt++;
	//
}

LONG CSMSDlg::OnSMSNormal( WPARAM wParam, LPARAM lParam )
{
	CLSMonitorDlg * pDlg = (CLSMonitorDlg*) AfxGetApp()->m_pMainWnd;
	if( pDlg)
		pDlg->UpdateListItem( (int) wParam , STATE_NORMAL);
	return 1;
}

LONG CSMSDlg::OnSMSStop( WPARAM wParam, LPARAM lParam )
{
	CLSMonitorDlg * pDlg = (CLSMonitorDlg*) AfxGetApp()->m_pMainWnd;
	if( pDlg)
		pDlg->UpdateListItem( (int) wParam , STATE_STOP_MONITOR);
	return 1;
}

void CSMSDlg::SetFirstError( int iSMSArray )
{
	m_SMSRuleManager.SetFirstError(iSMSArray);
}

void CSMSDlg::SetErrorToNormal( int iSMSArray )
{
	m_SMSRuleManager.SetErrorToNormal(iSMSArray);
}

void CSMSDlg::SetErrorInfo( CString &szErrorInfo )
{
	m_SMSRuleManager.SetErrorInfo(szErrorInfo);
}

void CSMSDlg::InitServerInfo( vZONEINFO &rvZoneInfo )
{
	if(rvZoneInfo.empty()) return;

	int iSMSArray = 0;
	const int iMax = (int)rvZoneInfo.size();
	for(int i = 0; i < iMax; i++)
	{
		int iSize = (int)rvZoneInfo[i].vServerInfo.size();
 		for(int j = 0;j < iSize;j++)
 		{
 			SERVERINFO &kServerInfo = rvZoneInfo[i].vServerInfo[j];
			kServerInfo.wSMSArray = iSMSArray;
			iSMSArray++;
 		}		
	}
	m_SMSRuleManager.SetInfo(iSMSArray, GetSafeHwnd());
}

void CSMSDlg::InitSystemInfo( vZONEINFO &rvZoneInfo )
{
	if(rvZoneInfo.empty()) return;

	int iSMSArray = 0;
	const int iMax = (int)rvZoneInfo.size();
	for(int i = 0; i < iMax; i++)
	{
		int iSize = (int)rvZoneInfo[i].vSystemInfo.size();
		for(int j = 0;j < iSize;j++)
		{
			SYSTEMINFO &kSystemInfo = rvZoneInfo[i].vSystemInfo[j];
			kSystemInfo.wSMSArray = iSMSArray;
			iSMSArray++;
		}		
	}
	m_SMSRuleManager.SetInfo(iSMSArray, GetSafeHwnd());
}


void CSMSDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if(nStatus != 0) return;
	//----------창 중간에 띄우기 ---------------------
	RECT winrect, workrect;
	// Find how large the desktop work area is
	SystemParametersInfo(SPI_GETWORKAREA, 0, &workrect, 0);
	int workwidth = workrect.right -  workrect.left;
	int workheight = workrect.bottom - workrect.top;

	// And how big the window is
	GetWindowRect(&winrect);
	int winwidth = winrect.right - winrect.left;
	int winheight = winrect.bottom - winrect.top;
	// Make sure it"s not bigger than the work area
	winwidth = min(winwidth, workwidth);
	winheight = min(winheight, workheight);

	// Now center it
	SetWindowPos( 
		NULL,
		workrect.left + (workwidth-winwidth) / 2,
		workrect.top + (workheight-winheight) / 2,
		winwidth, winheight, 
		SWP_SHOWWINDOW);
	//----------------------------------------------------------
	m_ComboBoxSelectServer.SetFocus();
	// 공지를 보낼 서버를 선택하고 타이틀바에 기록 한다. 
	CLSMonitorDlg * pDlg = (CLSMonitorDlg*) AfxGetApp()->m_pMainWnd;
	if( !pDlg ) return;
	char szTitle[MAX_PATH]="";
	StringCbPrintf(szTitle, sizeof(szTitle), "[%s] SMS", (LPCTSTR)pDlg->GetCurZoneName());
	SetWindowText(szTitle);
	pDlg->SetSMSComboList( m_ComboBoxSelectServer );

	m_iZoneArray = pDlg->GetCurZoneArray();
	pDlg->SetActiveTab( false );
}

DWORD CSMSDlg::GetRemainWaitTime( int iSMSArray )
{
	return m_SMSRuleManager.GetRemainWaitTime( iSMSArray );
}


void CSMSDlg::OnClose()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	CLSMonitorDlg * pDlg = (CLSMonitorDlg*) AfxGetApp()->m_pMainWnd;
	if( pDlg ) 
		pDlg->SetActiveTab( true );
	EndDialog(0);
}

void CSMSDlg::OnBnClickedDefaultCancel()
{

}