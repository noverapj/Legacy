// GraphDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "LS_Monitor.h"
#include "GraphDlg.h"
#include "afxdialogex.h"
#include "WinMessage.h"
#include "cFile.h"
#include <iostream>
#include <fstream>
using namespace std;

// CGraphDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CGraphDlg, CDialogEx)

CGraphDlg::CGraphDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CGraphDlg::IDD, pParent), m_PCCU(0), m_terms(GRAPH_TERM_3SEC), m_bCountryCode(false)
{
	m_bChangeGraph = true;
}

CGraphDlg::~CGraphDlg()
{
}

void CGraphDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_YEAR, m_CbbYear);
	DDX_Control(pDX, IDC_DAY, m_CBBDAY);
	DDX_Control(pDX, IDC_MONTH, m_CBBMONTH);
}


BEGIN_MESSAGE_MAP(CGraphDlg, CDialogEx)
	ON_BN_CLICKED(IDC_TODAY, &CGraphDlg::OnBnClickedToday)
	ON_BN_CLICKED(IDC_YESTERDAY, &CGraphDlg::OnBnClickedYesterday)
	ON_BN_CLICKED(IDC_SERACH, &CGraphDlg::OnBnClickedSerach)
	ON_BN_CLICKED(IDC_1WEEK_AGO, &CGraphDlg::OnBnClicked1weekAgo)
END_MESSAGE_MAP()


// CGraphDlg 메시지 처리기입니다.
void CGraphDlg::CreateGraph()
{
	CRect rc;
	CDC * pDC = GetDC();

	GetClientRect(rc);

	HDC m_hMemDC = CreateCompatibleDC(pDC->m_hDC);
	GetDlgItem(IDC_OSC_CCU)->GetWindowRect(rc) ;
	ScreenToClient(rc) ;
	m_graphCCU.DestroyWindow();
	m_graphCCU.Create(WS_VISIBLE | WS_CHILD, rc, this) ; 
	m_graphCCU.SetRange( true, 0, 100, 1) ;
	m_graphCCU.SetYUnits(_T("")) ;
	GetDlgItem(IDC_OSC_CCU)->ShowWindow(SW_HIDE);
}

BOOL CGraphDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	CWnd* pWnd = GetDesktopWindow();
	CenterWindow(pWnd);

	CreateGraph();
	CreateMenu();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CGraphDlg::Startup(CString title)
{
	m_title = title;
	
	RefreshTitle();
	ShowWindow(SW_HIDE);
	m_CbbYear.InsertString(0, ("2018"));
	m_CbbYear.InsertString(1, ("2019"));
	m_CbbYear.InsertString(2, ("2020"));
	m_CbbYear.InsertString(3, ("2021"));
	m_CbbYear.InsertString(4, ("2022"));
	m_CbbYear.InsertString(5, ("2023"));

	m_CBBMONTH.InsertString(0, ("1"));
	m_CBBMONTH.InsertString(1, ("2"));
	m_CBBMONTH.InsertString(2, ("3"));
	m_CBBMONTH.InsertString(3, ("4"));
	m_CBBMONTH.InsertString(4, ("5"));
	m_CBBMONTH.InsertString(5, ("6"));
	m_CBBMONTH.InsertString(6, ("7"));
	m_CBBMONTH.InsertString(7, ("8"));
	m_CBBMONTH.InsertString(8, ("9"));
	m_CBBMONTH.InsertString(9, ("10"));
	m_CBBMONTH.InsertString(10, ("11"));
	m_CBBMONTH.InsertString(11, ("12"));

	m_CBBDAY.InsertString(0, ("1"));
	m_CBBDAY.InsertString(1, ("2"));
	m_CBBDAY.InsertString(2, ("3"));
	m_CBBDAY.InsertString(3, ("4"));
	m_CBBDAY.InsertString(4, ("5"));
	m_CBBDAY.InsertString(5, ("6"));
	m_CBBDAY.InsertString(6, ("7"));
	m_CBBDAY.InsertString(7, ("8"));
	m_CBBDAY.InsertString(8, ("9"));
	m_CBBDAY.InsertString(9, ("10"));
	m_CBBDAY.InsertString(10, ("11"));
	m_CBBDAY.InsertString(11, ("12"));
	m_CBBDAY.InsertString(12, ("13"));
	m_CBBDAY.InsertString(13, ("14"));
	m_CBBDAY.InsertString(14, ("15"));
	m_CBBDAY.InsertString(15, ("16"));
	m_CBBDAY.InsertString(16, ("17"));
	m_CBBDAY.InsertString(17, ("18"));
	m_CBBDAY.InsertString(18, ("19"));
	m_CBBDAY.InsertString(19, ("20"));
	m_CBBDAY.InsertString(20, ("21"));
	m_CBBDAY.InsertString(21, ("22"));
	m_CBBDAY.InsertString(22, ("23"));
	m_CBBDAY.InsertString(23, ("24"));
	m_CBBDAY.InsertString(24, ("25"));
	m_CBBDAY.InsertString(25, ("26"));
	m_CBBDAY.InsertString(26, ("27"));
	m_CBBDAY.InsertString(27, ("28"));
	m_CBBDAY.InsertString(28, ("29"));
	m_CBBDAY.InsertString(29, ("30"));
	m_CBBDAY.InsertString(30, ("31"));
}

void CGraphDlg::CreateMenu()
{
	m_menu.DestroyMenu();
	m_menu.CreatePopupMenu();

	CString menu = _T("3 sec");
	m_menu.AppendMenu(MF_STRING, IDC_SERVICE_GRAPH3SEC, (LPCTSTR)menu);
	menu = _T("1 min");
	m_menu.AppendMenu(MF_STRING, IDC_SERVICE_GRAPH1MIN, (LPCTSTR)menu);
	menu = _T("10 min");
	m_menu.AppendMenu(MF_STRING, IDC_SERVICE_GRAPH10MIN, (LPCTSTR)menu);
}

void CGraphDlg::RefreshTitle()
{
	CString temp;
	temp.Format(_T("%s - CCU Graph(PCCU : %d)"), GetTitle(), (int)GetPCCU());
	SetWindowText(temp);
}

void CGraphDlg::InitDate()
{
	m_graphCCU.InitDate();
}

void CGraphDlg::SetRange(bool b, double dLower, double dUpper, int nDecimalPlaces)
{
	m_graphCCU.SetRange(b, dLower, dUpper, nDecimalPlaces);
}

void CGraphDlg::AppendPoint(bool bCCU, int nPoint)
{
	if(m_bChangeGraph == false)
	{
		return ;
	}

	m_bCountryCode = bCCU;
	m_points.push_back(nPoint); // 11860(200 * 593) max

	if(nPoint > m_PCCU)
	{
		m_PCCU = nPoint;
		RefreshTitle();
	}

	int term = GetTerm(m_terms);
	int divide =  m_points.size() % term;

	if(divide == 0)
	{
		m_graphCCU.AppendPoint(m_bCountryCode, nPoint);
	}
}

void CGraphDlg::SetXUnits(CString string)
{
	if(m_bChangeGraph == false)
	{
		return ;
	}

	m_graphCCU.SetXUnits(string);
}

void CGraphDlg::ShowGraph(MSG* pMsg)
{
	if(m_graphCCU.GetSafeHwnd() == pMsg->hwnd)
	{
		//TRACE("Message : %08x\n", pMsg->message);
		 if(pMsg->message == WM_RBUTTONUP)
		{
			CPoint pos;
			GetCursorPos(&pos);

			int term = GetTerm();
			for(int i = 1 ; i <= 3 ; i++)
			{
				if(i == term)
				{
					m_menu.CheckMenuItem(IDC_SERVICE_GRAPH3SEC+i-1, MF_CHECKED);
				}
				else
				{
					m_menu.CheckMenuItem(IDC_SERVICE_GRAPH3SEC+i-1, MF_UNCHECKED);
				}
			}
			m_menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pos.x, pos.y, this);
			
		}
	}
}

void CGraphDlg::HideGraph(MSG* pMsg)
{
	if(m_graphCCU.GetSafeHwnd() == pMsg->hwnd)
	{
		//TRACE("Message : %08x\n", pMsg->message);
		if(pMsg->message == WM_LBUTTONUP)
		{
			ShowWindow(SW_HIDE);
		}
	}
}

void CGraphDlg::ProgressTime(MSG* pMsg)
{
	if(m_graphCCU.GetSafeHwnd() == pMsg->hwnd)
	{
		CString szTime;
		int x = GET_X_LPARAM(pMsg->lParam);
		int y = GET_Y_LPARAM(pMsg->lParam);
		int rightPixel = m_graphCCU.GetRightPixel();
		int leftPixel = m_graphCCU.GetLeftPixel();

		if( (x < leftPixel) || (x > rightPixel) )
			return;

		int elapseTime = (rightPixel - x) / 4;
		int term = GetTerm(m_terms) * 3;
		elapseTime *= term;

		CTime nowTime = CTime::GetCurrentTime();
		CTime pastTime = nowTime;
		pastTime -= elapseTime;

		float fPosTerm = (rightPixel - leftPixel)/24.0;
		int Diff = x - leftPixel;

		int iHour = Diff/fPosTerm;
		int iTest = Diff%(int)fPosTerm;
		int iMinute = iTest /fPosTerm  * 60;
		szTime.Format("Time:%02d:%02d", iHour, iMinute);
		m_graphCCU.SetToolTip( szTime );;
	}
}


int CGraphDlg::GetTerm(const GraphTerms terms)
{
	int term = 1;
	switch(terms)
	{
	case GRAPH_TERM_3SEC :
		term = 1;
		break;
	case GRAPH_TERM_1MIN :
		//term = 5;
		term = 20;
		break;
	case GRAPH_TERM_10MIN :
		//term = 10;
		term = 200;
		break;
	}
	return term;
}

BOOL CGraphDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	ProgressTime(pMsg);
	HideGraph(pMsg);
	ShowGraph(pMsg);
	return CDialogEx::PreTranslateMessage(pMsg);
}

void CGraphDlg::Rearrange(const GraphTerms terms)
{
	ShowWindow(SW_HIDE);

	m_graphCCU.Reset();

	m_terms = terms;
	int term = GetTerm(terms);
	int width = m_graphCCU.GetWidth();
	int offset = (width+1) * term;

	int start = 0;
	if((int)m_points.size() > offset)
	{
		start = m_points.size() - offset;
	}
	
	TRACE("[Rearrange - %d]\n", term);
	for(int i = start ; i < (int)m_points.size() ; i += term)
	{
		m_graphCCU.AppendPoint(m_bCountryCode, m_points[i]);
		TRACE("graph : %d\n", m_points[i]);
	}

	ShowWindow(SW_SHOW);
}

BOOL CGraphDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	int itemIndex = lParam;
	switch(wParam)
	{
	case IDC_SERVICE_GRAPH3SEC :
		{
			m_terms = GRAPH_TERM_3SEC;
			Rearrange(GRAPH_TERM_3SEC);
		}
		return TRUE;

	case IDC_SERVICE_GRAPH1MIN :
		{
			m_terms = GRAPH_TERM_1MIN;
			Rearrange(GRAPH_TERM_1MIN);
		}
		return TRUE;

	case IDC_SERVICE_GRAPH10MIN :
		{
			m_terms = GRAPH_TERM_10MIN;
			Rearrange(GRAPH_TERM_10MIN);
		}
		return TRUE;
	}

	return CDialog::OnCommand(wParam, lParam);
}

void CGraphDlg::AppendCCUPoint( double dNewGB,double dNewDE,double dNewFR,double dNewIT,double dNewPL,double dNewTR, double dNewETC )
{
	m_graphCCU.AppendCCUPoint(dNewGB, dNewDE, dNewFR, dNewIT, dNewPL, dNewTR, dNewETC);
}


void CGraphDlg::OnBnClickedToday()
{
	// 오늘 내역을 출력한다.
	__int64 IN_WEEK = (__int64)10000000*60*60*24*7;
	__int64 IN_DAY  = (__int64)10000000*60*60*24;
	__int64 IN_HOUR = (__int64)10000000*60*60;
    __int64 IN_MIN  = (__int64)10000000*60;
    __int64 IN_SEC  = (__int64)10000000; 

	// 어제 내역을 출력한다.
	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);

	GraphSetting(sysTime);
	m_bChangeGraph = true;
	ComboBoxSetting(sysTime.wYear, sysTime.wMonth, sysTime.wDay);
}


void CGraphDlg::OnBnClickedYesterday()
{
	__int64 IN_WEEK = (__int64)10000000*60*60*24*7;
	__int64 IN_DAY  = (__int64)10000000*60*60*24;
	__int64 IN_HOUR = (__int64)10000000*60*60;
    __int64 IN_MIN  = (__int64)10000000*60;
    __int64 IN_SEC  = (__int64)10000000; 

	// 어제 내역을 출력한다.
	SYSTEMTIME sysTime;
	FILETIME ftm;
	LARGE_INTEGER largeInt; 
	GetLocalTime(&sysTime);

	SystemTimeToFileTime(&sysTime, &ftm); 
	memcpy(&largeInt, &ftm, sizeof(FILETIME));
	largeInt.QuadPart += IN_DAY * (-1); 
	memcpy(&ftm, &largeInt, sizeof(FILETIME));  
	FileTimeToSystemTime(&ftm, &sysTime); 
	GraphSetting(sysTime);
	m_bChangeGraph = false;
	ComboBoxSetting(sysTime.wYear, sysTime.wMonth, sysTime.wDay);
}


void CGraphDlg::OnBnClicked1weekAgo()
{
	__int64 IN_WEEK = (__int64)10000000*60*60*24*7;
	__int64 IN_DAY  = (__int64)10000000*60*60*24;
	__int64 IN_HOUR = (__int64)10000000*60*60;
    __int64 IN_MIN  = (__int64)10000000*60;
    __int64 IN_SEC  = (__int64)10000000; 

	// 어제 내역을 출력한다.
	SYSTEMTIME sysTime;
	FILETIME ftm;
	LARGE_INTEGER largeInt; 
	GetLocalTime(&sysTime);

	SystemTimeToFileTime(&sysTime, &ftm); 
	memcpy(&largeInt, &ftm, sizeof(FILETIME));
	largeInt.QuadPart += IN_DAY * (-7); 
	memcpy(&ftm, &largeInt, sizeof(FILETIME));  
	FileTimeToSystemTime(&ftm, &sysTime); 
	GraphSetting(sysTime);
	m_bChangeGraph = false;
	ComboBoxSetting(sysTime.wYear, sysTime.wMonth, sysTime.wDay);
}


void CGraphDlg::OnBnClickedSerach()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strYear, strMonth, strDay;
	m_CbbYear.GetLBText(m_CbbYear.GetCurSel(), strYear);
	m_CBBMONTH.GetLBText(m_CBBMONTH.GetCurSel(), strMonth);
	m_CBBDAY.GetLBText(m_CBBDAY.GetCurSel(), strDay);

	int iYear = atoi(strYear);
	int iMonth = atoi(strMonth);
	int iDay = atoi(strDay);


	// 어제 내역을 출력한다.
	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);
	sysTime.wYear = iYear;
	sysTime.wMonth = iMonth;
	sysTime.wDay = iDay;

	GraphSetting(sysTime);
	m_bChangeGraph = false;
	ComboBoxSetting(iYear, iMonth, iDay);
}


void CGraphDlg::GraphSetting(const SYSTEMTIME systime)
{
	BOOL billingFlag = FALSE;
	
	TCHAR buffer[512];
	TCHAR path[512];
	GetCurrentDirectory(sizeof(path), path);

	CString fullPath;
	fullPath.Format(_T("%s\\%04lu%02lu%02lu.txt"), m_title, systime.wYear, systime.wMonth, systime.wDay);

	ifstream ifile;
	char line[200]; // 한 줄씩 읽어서 임시로 저장할 공간
	CString strTemp; 
	CString strCount; 
	int iLastMaxCount = 0;
	int imaxValue = 0;
	int iMaxCount =0;
        
    ifile.open(fullPath);  // 파일 열기
    if (ifile.is_open())
    {
        while (ifile.getline(line, sizeof(line))) // 한 줄씩 읽어 처리를 시작한다.
        {
            cout << line << endl; // 내용 출력
			strTemp = line;
//			strTime = strTemp.Right(strTemp.GetLength() - (1));
			CString strYear = strTemp.Mid(1, 4);
			int iYear = _ttoi(strYear);
			CString strMonth = strTemp.Mid(6, 2);
			int iMonth = _ttoi(strMonth);
			CString strDay = strTemp.Mid(9, 2);
			int iDay = _ttoi(strDay);
			CString strHour = strTemp.Mid(12, 2);
			int iHour = _ttoi(strHour);
			CString strMinute = strTemp.Mid(15, 2);
			int iMinute = _ttoi(strMinute);
			CString strSecond = strTemp.Mid(18, 2);
			int iSecond = _ttoi(strSecond);

			strCount = strTemp.Left(strTemp.GetLength() - (1));
			int iPoint2 = strCount.ReverseFind('[');
			strCount = strCount.Right(strCount.GetLength() - (iPoint2 + 1));
			int iCount = _ttoi(strCount);
			if(iMaxCount < iCount)
			{
				iMaxCount = iCount;
			}
        }

		if(iLastMaxCount < iMaxCount)
		{
			if(iMaxCount / 100 == 0) imaxValue = 100;
			else if(iMaxCount / 500 == 0) imaxValue = 500;
			else if(iMaxCount / 1000 == 0) imaxValue = 1000;
			else if(iMaxCount / 2500 == 0) imaxValue = 2500;
			else if(iMaxCount / 5000 == 0) imaxValue = 5000;
			else if(iMaxCount / 10000 == 0) imaxValue = 10000;
			else if(iMaxCount / 20000 == 0) imaxValue = 20000;
			else if(iMaxCount / 50000 == 0) imaxValue = 50000;
			else if(iMaxCount / 100000 == 0) imaxValue = 100000;
			else if(iMaxCount / 500000 == 0) imaxValue = 500000;
			else if(iMaxCount / 1000000 == 0) imaxValue = 1000000;
		}

		if(iLastMaxCount != imaxValue)
		{
			iLastMaxCount = imaxValue;
		}
		
		SetRange(true,	0, imaxValue, 1);

		ifile.clear();
		ifile.seekg(0, ios::beg);
		m_graphCCU.Init();
		m_graphCCU.InitDate();
        while (ifile.getline(line, sizeof(line))) // 한 줄씩 읽어 처리를 시작한다.
        {
            cout << line << endl; // 내용 출력
			strTemp = line;
//			strTime = strTemp.Right(strTemp.GetLength() - (1));
			CString strYear = strTemp.Mid(1, 4);
			int iYear = _ttoi(strYear);
			CString strMonth = strTemp.Mid(6, 2);
			int iMonth = _ttoi(strMonth);
			CString strDay = strTemp.Mid(9, 2);
			int iDay = _ttoi(strDay);
			CString strHour = strTemp.Mid(12, 2);
			int iHour = _ttoi(strHour);
			CString strMinute = strTemp.Mid(15, 2);
			int iMinute = _ttoi(strMinute);
			CString strSecond = strTemp.Mid(18, 2);
			int iSecond = _ttoi(strSecond);

			strCount = strTemp.Left(strTemp.GetLength() - (1));
			int iPoint2 = strCount.ReverseFind('[');
			strCount = strCount.Right(strCount.GetLength() - (iPoint2 + 1));
			int iCount = _ttoi(strCount);

			AppendDateTimePoint(iYear, iMonth, iDay, iHour, iMinute, iSecond, iCount);
        }
    }

	else
	{
		SetRange(true,	0, imaxValue, 1);
		CString xunits;
		m_graphCCU.Init();
		xunits.Format(_T("%04d-%02d-%02d"), systime.wYear, systime.wMonth, systime.wDay);

		MessageBox(_T("Not Found File"), xunits, MB_OK);
	}

	CString xunits;
	xunits.Format(_T("%04d-%02d-%02d User(%06d/%06d)"), systime.wYear, systime.wMonth, systime.wDay, iMaxCount, iMaxCount);

	m_graphCCU.SetXUnits(xunits);

    ifile.close(); // 파일 닫기
}

void CGraphDlg::AppendDateTimePoint(int iYear, int iMonth, int iDay, int iHour, int iMinute, int iSecond, int nPoint)
{
	m_points.push_back(nPoint); // 11860(200 * 593) max

	if(nPoint > m_PCCU)
	{
		m_PCCU = nPoint;
		RefreshTitle();
	}

	int term = GetTerm(m_terms);
	int divide =  m_points.size() % term;

	if(divide == 0)
	{
		m_graphCCU.AppendDateTimePoint(iYear, iMonth, iDay, iHour, iMinute, iSecond, m_bCountryCode, nPoint);
	}
}

void CGraphDlg::ComboBoxSetting(int iYear, int iMonth, int iDay)
{
	m_CbbYear.SendMessage(CB_SETCURSEL,iYear - 2018, 0);
	m_CBBMONTH.SendMessage(CB_SETCURSEL,iMonth - 1, 0);
	m_CBBDAY.SendMessage(CB_SETCURSEL,iDay - 1, 0);
}
