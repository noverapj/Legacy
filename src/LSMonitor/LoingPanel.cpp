// LoingPanel.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "LSMonitor.h"
#include "LoingPanel.h"
#include "LSMonitorDlg.h"
#include "afxdialogex.h"
#include "LS_ServerInfoDlg.h"
#include "ScrollPanel.h"

// CLoingPanel 대화 상자입니다.

IMPLEMENT_DYNAMIC(CLoingPanel, CDialogEx)

CLoingPanel::CLoingPanel(CWnd* pParent /*=NULL*/)
	: CDialogEx(CLoingPanel::IDD, pParent)
{
	
}

CLoingPanel::~CLoingPanel()
{
}

void CLoingPanel::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CLoingPanel, CDialogEx)
	ON_BN_CLICKED(IDOK, &CLoingPanel::OnBnClickedOk)
	ON_WM_SHOWWINDOW()
	ON_WM_MOUSEWHEEL()
	ON_WM_SIZE()
	ON_WM_VSCROLL()
	ON_WM_SIZING()
	ON_MESSAGE(WM_LSLOGINSERVER,SendPacket)
	ON_COMMAND(ID_ENABLE_ALLSERVER, &CLoingPanel::EnableAllServer)
	ON_COMMAND(ID_DISALBE_ALLSERVER,&CLoingPanel::DisableAllServerBlock)
END_MESSAGE_MAP()


// CLoingPanel 메시지 처리기입니다.


void CLoingPanel::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	ShowWindow(SW_HIDE);
	//CDialogEx::OnOK();
}


void CLoingPanel::PostNcDestroy()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	//ShowWindow(SW_HIDE);
	delete this;

 
}


BOOL CLoingPanel::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_bInit = false;
	GetWindowRect(m_rect);
	m_nScrollPos = 0;
	 
	m_parrentmenu.LoadMenu( IDR_MENU );
	m_menu = m_parrentmenu.GetSubMenu(eServerType_LoginPanel -1);
//	Init();
 
	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}
#define PMAN 10

void CLoingPanel::Init()
{
 
	if(m_blockstate == 0)
		EnableServerState();
	else
		DisableServerState();
	int nbottom=1;
	int nright=0;
	for(int i=0; i<m_vsvrInfo.size();++i)
	{
		CRect rctmp;
		ServerInfo_& svrinfo = m_vsvrInfo[i];
		LS_ServerInfoDlg* dlg = new LS_ServerInfoDlg(this,svrinfo);
		dlg->Create(LS_ServerInfoDlg::IDD,this);
		dlg->ShowWindow(SW_SHOW);
		dlg->GetClientRect(rctmp);
		nright = 0;
// 		if(i ==0 )
// 			nright += rctmp.right;
		if((i) % PMAN ==0 && i != 0 )
		{
			nbottom += rctmp.bottom;
			 
		}
		rctmp.left += (i%PMAN*rctmp.right+1) +nright;
		rctmp.right +=(i%PMAN*rctmp.right+1) +nright;
		rctmp.top +=  nbottom;
		rctmp.bottom += nbottom;
		dlg->MoveWindow(rctmp);
		dlg->SetWindowPos(&wndTopMost,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
		dlg->ShowWindow(SW_HIDE);
		m_vinfodlg.push_back(dlg);
	}
	 GetWindowRect(m_rect);
	 m_rect.bottom +=( m_vsvrInfo.size()*20 - m_rect.bottom);
	 


 
}


void CLoingPanel::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
 
	for(int i=0; i<m_vinfodlg.size(); ++i)
	{
		
		m_vinfodlg[i]->ShowWindow(SW_SHOW);
		m_vinfodlg[i]->SetWindowPos(&wndTopMost,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
	}
	CRect tmp;
	GetClientRect(&tmp);
	OnSizing(2,tmp);
	ShowScrollBar(SB_VERT,TRUE);

	
	 
}



void CLoingPanel::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here.
	m_nCurHeight = cy;
	int nScrollMax;
	if (cy < m_rect.Height())
	{
		nScrollMax = m_rect.Height() - cy;
	}
	else
		nScrollMax = 0;
 //	nScrollMax += PMAN * 30;
	SCROLLINFO si;
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_ALL; // SIF_ALL = SIF_PAGE | SIF_RANGE | SIF_POS;
	si.nMin = 0;
	si.nMax = nScrollMax;
	si.nPage = si.nMax/10;
	si.nPos = 0;
	SetScrollInfo(SB_VERT, &si, TRUE); 
}


void CLoingPanel::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default.
	int nDelta;
	int nMaxPos = m_rect.Height() - m_nCurHeight;

	switch (nSBCode)
	{
	case SB_LINEDOWN:
		if (m_nScrollPos >= nMaxPos)
			return;
		nDelta = min(max(nMaxPos/20,5),nMaxPos-m_nScrollPos);
		break;

	case SB_LINEUP:
		if (m_nScrollPos <= 0)
			return;
		nDelta = -min(max(nMaxPos/20,5),m_nScrollPos);
		break;

	case SB_PAGEDOWN:
		if (m_nScrollPos >= nMaxPos)
			return;
		nDelta = min(max(nMaxPos/20,5),nMaxPos-m_nScrollPos);
		break;

	case SB_THUMBPOSITION:
		nDelta = (int)nPos - m_nScrollPos;
		break;
	case SB_THUMBTRACK:
	case SB_PAGEUP:
		if (m_nScrollPos <= 0)
			return;
		nDelta = -min(max(nMaxPos/20,5),m_nScrollPos);
		break;

	default:
		return;
	}
	m_nScrollPos += nDelta;
	SetScrollPos(SB_VERT,m_nScrollPos,TRUE);
	ScrollWindow(0,-nDelta);

	CDialogEx::OnVScroll(nSBCode, nPos, pScrollBar);
}


void CLoingPanel::OnSizing(UINT fwSide, LPRECT pRect)
{
	CDialogEx::OnSizing(fwSide, pRect);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}
BOOL CLoingPanel::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	int nMaxPos = m_rect.Height() - m_nCurHeight;

	if (zDelta<0)
	{
		if (m_nScrollPos < nMaxPos)
		{
			zDelta = min(max(nMaxPos/10,5),nMaxPos-m_nScrollPos);

			m_nScrollPos += zDelta;
			SetScrollPos(SB_VERT,m_nScrollPos,TRUE);
			ScrollWindow(0,-zDelta);
		}
	}
	else
	{
		if (m_nScrollPos > 0)
		{
			zDelta = -min(max(nMaxPos/10,5),m_nScrollPos);

			m_nScrollPos += zDelta;
			SetScrollPos(SB_VERT,m_nScrollPos,TRUE);
			ScrollWindow(0,-zDelta);
		}
	}

	return CDialog::OnMouseWheel(nFlags, zDelta, pt);
}

LRESULT CLoingPanel::SendPacket( WPARAM wParam,LPARAM lParam )
{
	if(wParam == UPDATESVRINFO && lParam == UPDATESVRINFO)
	{
		for(int i=0; i<m_vsvrInfo.size();++i)
		{
			ServerInfo_& ptmp = m_vsvrInfo[i];
			for(int j=0; j<m_vinfodlg.size(); ++j)
			{
				if(m_vinfodlg[j]->m_svrInfo.sendserverid == ptmp.sendserverid)
				{
					m_vinfodlg[j]->PostMessageA(WM_LSLOGINSERVER,(WPARAM)&ptmp,UPDATESVRINFO);

				}
			}
		}
	}
	else if(wParam == EMCONTROLTYPE::LS_SETALLSERVERBLOCK)
	{
		int blockstate = lParam;
		if(blockstate == 1)
		{
			DisableServerState();
		}
		else
		{
			EnableServerState();

		}
	}
	 
	return 0;
}

void CLoingPanel::EnableAllServer()
{
	if(IDYES== MessageBox("정말 진행 하시겠습니까? ","Enable All Server",MB_YESNO))
	{
		SendBlcokPacket(0);	 
	}
	else
	{

	}
}

void CLoingPanel::DisableAllServerBlock()
{ 
	if(IDYES== MessageBox("정말 진행 하시겠습니까? ","Disable All Server",MB_YESNO))
	{
		SendBlcokPacket(1);
	}
	else
	{

	}
	
}



BOOL CLoingPanel::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	switch(pMsg->message)
	{
	case WM_RBUTTONDOWN:
		CreatePopup();
		break;
 
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}

void CLoingPanel::CreatePopup()
{
	CPoint pos;
	GetCursorPos( &pos );
	m_menu->TrackPopupMenu( TPM_LEFTALIGN, pos.x, pos.y, this );
}

void CLoingPanel::DisableServerState()
{
	m_menu->CheckMenuItem(ID_ENABLE_ALLSERVER,MF_UNCHECKED);
	m_menu->CheckMenuItem(ID_DISALBE_ALLSERVER,MF_CHECKED);
}

void CLoingPanel::EnableServerState()
{
	m_menu->CheckMenuItem(ID_ENABLE_ALLSERVER,MF_CHECKED);
	m_menu->CheckMenuItem(ID_DISALBE_ALLSERVER,MF_UNCHECKED);
}

void CLoingPanel::SendBlcokPacket( int a )
{
	SP2Packet *pk = new SP2Packet(EPROTOCOL::LSPTK_CONTROL_SERVER);
	int type = EMCONTROLTYPE::LS_SETALLSERVERBLOCK;
	(*pk) << type;
	(*pk) << a;
	GetParent()->PostMessageA(WM_SENDLOGINSERVER,(WPARAM)pk);
}
