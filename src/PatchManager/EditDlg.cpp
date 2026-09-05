// EditDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "PatchManager.h"
#include "EditDlg.h"
#include "GetTextDlg.h"
#include "afxdialogex.h"


// CEditDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CEditDlg, CDialogEx)

CEditDlg::CEditDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CEditDlg::IDD, pParent), m_strPath(_T(""))
	, m_strTxtData(_T("")), m_cNowPushedKey(NULL), m_iStartSel(0), m_iEndSel(0), m_uFileSize(0), m_bFirstSet(FALSE)
{
	for(int i = 0 ; i < FK_END ; ++i)
		m_bFuncKey[i] = FALSE;
	vecRollstr.reserve(5);
	InitializeCriticalSection(&m_cs);
	m_KeyWord.iIndex = -1;
	m_KeyWord.StrKey = "";
}

CEditDlg::~CEditDlg()
{
	DeleteCriticalSection(&m_cs);
}

void CEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_strTxtData);
	DDX_Control(pDX, IDC_EDIT1, m_EditCtr);
}


BEGIN_MESSAGE_MAP(CEditDlg, CDialogEx)
	ON_WM_DESTROY()
	ON_WM_CREATE()
	ON_WM_ACTIVATE()
END_MESSAGE_MAP()


// CEditDlg 메시지 처리기입니다.

void CEditDlg::OnDestroy()
{
	CDialogEx::OnDestroy();
	UpdateData(TRUE);
	//if(m_strTxtData.Find("\r\n") != -1)
		m_strTxtData.Replace(_T("\r\n"),_T("\n")); //edit control에 줄바꿈하기 위해 넣은 문자 재 변경

	UpdateData(FALSE);
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}


int CEditDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;
	// TODO:  여기에 특수화된 작성 코드를 추가합니다.
	return 0;
}

void CEditDlg::SetLoadFilePath( const CString& strPath )
{
	m_strPath = strPath;
	CStdioFile	Txt_File;
	CString strTxt;
	if(Txt_File.Open(m_strPath, CFile::modeRead | CFile::typeText))
	{
		CString		strToken;
		while(Txt_File.ReadString(strToken))
		{
			//strTxt.Format(_T("%d"),i); //확인용
			m_strTxtData += strTxt;
			m_strTxtData += strToken;
			m_strTxtData += "\r\n"; //줄바꿈하기 위해~ 나중에 지워줌
		}
	}
	m_uFileSize = Txt_File.SeekToEnd(); //한계 텍스트 사이즈 변경 용
	if(strPath.Find(DELETE_FILE_LIST_NAME) != -1 && m_uFileSize < 1048576)
	{
		m_uFileSize = 1048576;//최소 1메가로 잡아줌
	}
	Txt_File.Close();
}


BOOL CEditDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if(pMsg->message == WM_KEYDOWN)
	{
		if(::GetKeyState(VK_CONTROL) < 0 && ::GetKeyState('A') < 0) Funckey(FK_ALL_SELECT);
		if(::GetKeyState(VK_CONTROL) < 0 && ::GetKeyState('F') < 0) Funckey(FK_FIND);
		if(_KEYDOWN(VK_F3)) Funckey(FK_FIND_NEXT);
	}
	

	return CDialogEx::PreTranslateMessage(pMsg);

	////////////////////////////////////////////////
	//사이즈를 수정하니 정상작동되어 필요가 없는 부분     //
	////////////////////////////////////////////////
 	if(pMsg->message == WM_KEYDOWN)
	{
		(_KEYDOWN(VK_SHIFT))?m_bFuncKey[FK_SHIFT] = TRUE : m_bFuncKey[FK_SHIFT] = FALSE;

		//func key
		if(::GetKeyState(VK_CONTROL) < 0 && ::GetKeyState('C') < 0) Funckey(FK_COPY);
		if(::GetKeyState(VK_CONTROL) < 0 && ::GetKeyState('X') < 0) Funckey(FK_CUT);
		if(::GetKeyState(VK_CONTROL) < 0 && ::GetKeyState('V') < 0) Funckey(FK_PASTE);
		if(::GetKeyState(VK_CONTROL) < 0 && ::GetKeyState('Z') < 0) Funckey(FK_BACK);

		//글입력
		InsertText();
	}
	if(pMsg->message == WM_LBUTTONUP)
	{
		m_EditCtr.GetSel(m_iStartSel, m_iEndSel);
	}
}

void CEditDlg::InsertText()
{
	int nSelStart, nSelEnd, nLineNum, nColNum;
	
	m_EditCtr.GetSel(nSelStart, nSelEnd);					//현재 위치 정보 찾자!
	nLineNum	= m_EditCtr.LineFromChar(nSelStart);			//행 row
	nColNum		= nSelStart - m_EditCtr.LineIndex(nLineNum);		//열 col
	
	UpdateData(TRUE);
	
	if(_KEYDOWN(VK_RETURN))
	{
		if(nSelStart != nSelEnd) m_strTxtData.Delete(nSelStart, nSelEnd - nSelStart); //선택된영역 삭제하기
		m_strTxtData.Insert(nSelStart,CString("\r\n"));
	}
	//영문자
	if(_KEYDOWN('A'))	SetChangeText('a',nSelStart, nSelEnd);
	if(_KEYDOWN('B'))	SetChangeText('b',nSelStart, nSelEnd);
	if(_KEYDOWN('C'))	SetChangeText('c',nSelStart, nSelEnd);
	if(_KEYDOWN('D'))	SetChangeText('d',nSelStart, nSelEnd);
	if(_KEYDOWN('E'))	SetChangeText('e',nSelStart, nSelEnd);
	if(_KEYDOWN('F'))	SetChangeText('f',nSelStart, nSelEnd);
	if(_KEYDOWN('G'))	SetChangeText('g',nSelStart, nSelEnd);
	if(_KEYDOWN('H'))	SetChangeText('h',nSelStart, nSelEnd);
	if(_KEYDOWN('I'))	SetChangeText('i',nSelStart, nSelEnd);
	if(_KEYDOWN('J'))	SetChangeText('j',nSelStart, nSelEnd);
	if(_KEYDOWN('K'))	SetChangeText('k',nSelStart, nSelEnd);
	if(_KEYDOWN('L'))	SetChangeText('l',nSelStart, nSelEnd);
	if(_KEYDOWN('M'))	SetChangeText('m',nSelStart, nSelEnd);
	if(_KEYDOWN('N'))	SetChangeText('n',nSelStart, nSelEnd);
	if(_KEYDOWN('O'))	SetChangeText('o',nSelStart, nSelEnd);
	if(_KEYDOWN('P'))	SetChangeText('p',nSelStart, nSelEnd);
	if(_KEYDOWN('Q'))	SetChangeText('q',nSelStart, nSelEnd);
	if(_KEYDOWN('R'))	SetChangeText('r',nSelStart, nSelEnd);
	if(_KEYDOWN('S'))	SetChangeText('s',nSelStart, nSelEnd);
	if(_KEYDOWN('T'))	SetChangeText('t',nSelStart, nSelEnd);
	if(_KEYDOWN('U'))	SetChangeText('u',nSelStart, nSelEnd);
	if(_KEYDOWN('V'))	SetChangeText('v',nSelStart, nSelEnd);
	if(_KEYDOWN('W'))	SetChangeText('w',nSelStart, nSelEnd);
	if(_KEYDOWN('X'))	SetChangeText('x',nSelStart, nSelEnd);
	if(_KEYDOWN('Y'))	SetChangeText('y',nSelStart, nSelEnd);
	if(_KEYDOWN('Z'))	SetChangeText('z',nSelStart, nSelEnd);

	//숫자
	if(_KEYDOWN('0'))	SetChangeText('0',nSelStart, nSelEnd);
	if(_KEYDOWN('1'))	SetChangeText('1',nSelStart, nSelEnd);
	if(_KEYDOWN('2'))	SetChangeText('2',nSelStart, nSelEnd);
	if(_KEYDOWN('3'))	SetChangeText('3',nSelStart, nSelEnd);
	if(_KEYDOWN('4'))	SetChangeText('4',nSelStart, nSelEnd);
	if(_KEYDOWN('5'))	SetChangeText('5',nSelStart, nSelEnd);
	if(_KEYDOWN('6'))	SetChangeText('6',nSelStart, nSelEnd);
	if(_KEYDOWN('7'))	SetChangeText('7',nSelStart, nSelEnd);
	if(_KEYDOWN('8'))	SetChangeText('8',nSelStart, nSelEnd);
	
	//특수문자
	if(_KEYDOWN(VK_OEM_1))	SetChangeText(';',nSelStart, nSelEnd);
	if(_KEYDOWN(VK_OEM_2))	SetChangeText('/',nSelStart, nSelEnd);
	if(_KEYDOWN(VK_OEM_4))	SetChangeText('[',nSelStart, nSelEnd);
	if(_KEYDOWN(VK_OEM_5))	SetChangeText('\\',nSelStart, nSelEnd);
	if(_KEYDOWN(VK_OEM_6))	SetChangeText(']',nSelStart, nSelEnd);
	if(_KEYDOWN(VK_OEM_PERIOD))	SetChangeText('.',nSelStart, nSelEnd);
	
	static int bTest = 0;
	static int sSel = 0;
	static int sSPos = 0;

	SCROLLINFO info;
	m_EditCtr.GetScrollInfo(SB_VERT,&info);
	int iGetScrollPos = info.nPos;
	UpdateData(FALSE); //다시 그려주자
	if( bTest == 1 )
	{
		SCROLLINFO info;
		m_EditCtr.GetScrollInfo(SB_VERT,&info);
		int iGetScrollPos = info.nPos;
		
	}
	else if( bTest == 2 )
	{
		m_EditCtr.SetScrollPos(SB_VERT, iGetScrollPos+23); //뭐가 문제임?
		m_EditCtr.SetSel(nSelStart,nSelStart); //셀위치로 이동
	}	
}

void CEditDlg::SetChangeText( char cTxt, int iStart, int iEnd )
{
	m_cNowPushedKey	= cTxt;
	CString strToken(cTxt);

	if(iStart != iEnd) 
		m_strTxtData.Delete(iStart, iEnd - iStart); //선택된영역 삭제하기

	if(m_bFuncKey[FK_SHIFT])	
		strToken.MakeUpper();

	m_strTxtData.Insert(iStart,strToken);
}

void CEditDlg::Funckey(int ikey)
{
	CGetTextDlg GetTextDlg;
	CString strText;
	int iIndex = 0;
	UpdateData(TRUE);
	switch(ikey)
	{
	case FK_COPY :
		if(m_iStartSel == m_iEndSel)	return;
		m_strCopy = m_strTxtData.Mid(m_iStartSel, m_iEndSel - m_iStartSel);
		break;
	case FK_CUT :
		if(m_iStartSel == m_iEndSel)	return;
		m_strCopy = m_strTxtData.Mid(m_iStartSel, m_iEndSel - m_iStartSel);
		m_strTxtData.Delete(m_iStartSel, m_iEndSel - m_iStartSel);
		break;
	case FK_PASTE :
		if(m_strCopy.IsEmpty()) return;
		if(m_iStartSel != m_iEndSel) 
			m_strTxtData.Delete(m_iStartSel, m_iEndSel - m_iStartSel); //선택된영역 삭제하기
		m_strTxtData.Insert(m_iStartSel,m_strCopy);	
		break;
	case FK_ALL_SELECT :
		m_EditCtr.SetSel(0, m_strTxtData.GetLength()); //셀위치로 이동
		break;
	case FK_BACK:
		break;
	case FK_FIND_NEXT:
		if(m_KeyWord.StrKey.IsEmpty())			break;
		iIndex = m_strTxtData.Find(m_KeyWord.StrKey, m_KeyWord.iIndex);
		if( iIndex == -1 )
			m_KeyWord.iIndex = 0; //맨 처음부터 다시 검색
		else
		{
			m_KeyWord.iIndex = iIndex + m_KeyWord.StrKey.GetLength();
			m_EditCtr.SetSel(iIndex , iIndex + m_KeyWord.StrKey.GetLength()); //셀위치로 이동
		}
		break;
	case FK_FIND : 
		GetTextDlg.GetText(&strText);
		GetTextDlg.DoModal();
		iIndex = m_strTxtData.Find(strText);
		if( iIndex == -1 )
			MessageBox("TEXT FIND FAIL TT^TT", "FAIL", MB_OK);
		else
		{
			m_KeyWord.iIndex = iIndex + strText.GetLength();
			m_KeyWord.StrKey = strText;
			m_EditCtr.SetSel(iIndex , iIndex + strText.GetLength()); //셀위치로 이동
		}
		break;
	default:
		break;
	}
}


void CEditDlg::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CDialogEx::OnActivate(nState, pWndOther, bMinimized);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if(!m_bFirstSet)
	{
		m_bFirstSet = TRUE;
		m_EditCtr.SetSel(0,0);
		m_EditCtr.SetLimitText(m_uFileSize + (m_uFileSize / 2)); //사이즈 확장
		m_strTxtData.GetBufferSetLength(m_uFileSize);
	}	
}
