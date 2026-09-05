// ListCtrlEx.cpp : implementation file
//

#include "stdafx.h"
#include "ListCtrlEx.h"
#include <tchar.h>


// CListCtrlEx

IMPLEMENT_DYNAMIC(CListCtrlEx, CListCtrl)
CListCtrlEx::CListCtrlEx() : m_ProgressColumn(0)
{
	m_progress = NULL;
}

CListCtrlEx::~CListCtrlEx()
{
}


BEGIN_MESSAGE_MAP(CListCtrlEx, CListCtrl)
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
END_MESSAGE_MAP()



// CListCtrlEx message handlers


void CListCtrlEx::OnPaint()
{
	// TODO: Add your message handler code here
	// Do not call CListCtrl::OnPaint() for painting messages
	int Top=GetTopIndex();
	int Total=GetItemCount();
	int PerPage=GetCountPerPage();
	int LastItem=((Top+PerPage)>Total)?Total:Top+PerPage;

	CHeaderCtrl* pHeader=GetHeaderCtrl();
	POSITION pos = m_ProgressList.GetHeadPosition();

	for(int i=Top;i<LastItem;i++)
	{
		if(pos != NULL)
		{
			CTextProgressCtrl* progress = m_ProgressList.GetAt(pos);
			CRect ColRt;
			pHeader->GetItemRect(m_ProgressColumn,&ColRt);
			// get the rect
			CString Data=GetItemText(i,m_ProgressColumn);
			//SetItemText(i,m_ProgressColumn,_T(""));
			CRect rt;
			GetItemRect(i,&rt,LVIR_BOUNDS);
			rt.top+=1;
			rt.bottom-=1;
			rt.right -= 10;
			rt.left+=ColRt.left;
			int Width=ColRt.Width() ;
			rt.right=rt.left+Width-4;
	 
			int Percent=_ttoi(Data.GetBuffer());

			// set the position on the control
			progress->MoveWindow(rt);
			progress->SetPos(Percent);
			progress->ShowWindow(SW_SHOWNORMAL);
 
			m_ProgressList.GetNext(pos);

		}
	}
	CListCtrl::OnPaint();
}

void CListCtrlEx::InitProgressColumn(int ColNum/*=0*/)
{
	m_ProgressColumn=ColNum;
	// create the progress control and set their position
}

int CListCtrlEx::InsertItem( _In_ const LVITEM* pItem )
{
	m_ProgressList.AddTail(CreateProgress());

	return CListCtrl::InsertItem(pItem);
}

int CListCtrlEx::InsertItem( _In_ int nItem, _In_z_ LPCTSTR lpszItem )
{
	m_ProgressList.AddTail(CreateProgress());

	return CListCtrl::InsertItem(nItem,lpszItem);
}

int CListCtrlEx::InsertItem( _In_ int nItem, _In_z_ LPCTSTR lpszItem, _In_ int nImage )
{
	m_ProgressList.AddTail(CreateProgress());

	return CListCtrl::InsertItem(nItem,lpszItem,nImage);
}

BOOL CListCtrlEx::SetItemText( _In_ int nItem, _In_ int nSubItem, _In_z_ LPCTSTR lpszText )
{
	return CListCtrl::SetItemText(nItem,nSubItem,lpszText);
}

BOOL CListCtrlEx::DeleteItem( _In_ int nItem )
{
	CTextProgressCtrl* progress = m_ProgressList.RemoveTail();
	delete progress;

	return CListCtrl::DeleteItem(nItem);
}

BOOL CListCtrlEx::DeleteAllItems()
{
	POSITION pos = m_ProgressList.GetHeadPosition();
	while(pos)
	{
		CTextProgressCtrl* progress = m_ProgressList.GetAt(pos);
		delete progress;
		m_ProgressList.GetNext(pos);
	}
	m_ProgressList.RemoveAll();

	return CListCtrl::DeleteAllItems();
}

CTextProgressCtrl* CListCtrlEx::CreateProgress()
{
	CTextProgressCtrl* pControl=new CTextProgressCtrl();
	CRect rt;
	pControl->Create(NULL,rt,this,IDC_PROGRESS_LIST+ m_ProgressList.GetCount());
	pControl->ShowWindow(SW_HIDE);
	return pControl;
}

#if 0 

void CListCtrlEx::SetProgressRange( int progressIndex, int min, int max )
{

	if(m_ProgressList.GetSize() < progressIndex)
	{
		CProgressCtrl* progress =  m_ProgressList.GetAt(progressIndex);

	}
	
}





#endif

void CListCtrlEx::OnSetFocus(CWnd* pOldWnd)
{
	CListCtrl::OnSetFocus(pOldWnd);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}


void CListCtrlEx::OnKillFocus(CWnd* pNewWnd)
{
	CListCtrl::OnKillFocus(pNewWnd);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}
