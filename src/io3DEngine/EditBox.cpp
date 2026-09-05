#include "stdafx.h"
#include ".\editbox.h"
#include <strsafe.h>
#include "InputBox.h"
#include "ioWnd.h"
#include "ioEdit.h"
#include "ioRenderSystem.h"

WNDPROC EditBox::m_EditProc = NULL;

EditBox::EditBox(void)
{
	ZeroMemory( m_szInput, sizeof( m_szInput ) );
	m_hEdit      = NULL;
	m_hParentWnd = NULL;

	m_iCurCaretPos = 0;
    m_dwSavedwConv = 0;
	m_dwSavedwSent = 0;

	m_hSavedIMC = NULL;
}

EditBox::~EditBox(void)
{
	if( m_hEdit )
	{
		DestroyWindow( m_hEdit );
		m_hEdit = NULL;
	}
}

LRESULT CALLBACK EditBox::EditProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	if( g_InputBox.InputBoxProc( hWnd, msg, wParam, lParam ) )
		return 0;

	// 채팅에 쓰레기값 입력 방지 
	switch( msg )
	{
	case WM_CHAR: 
		switch(wParam)
		{
		case VK_RETURN:
			return TRUE;
		case VK_TAB:
			return TRUE;
		}
		break;
	}
	//

	return CallWindowProc( m_EditProc, hWnd, msg, wParam, lParam );
	// return DefWindowProc( hWnd, msg, wParam, lParam );
}

BOOL EditBox::IMEWinProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	if( message == WM_IME_STARTCOMPOSITION )
	{
		if( ioEdit::m_pKeyFocusEdit )
			SetCompositionWindow( ioEdit::m_pKeyFocusEdit->GetDerivedPosX(), ioEdit::m_pKeyFocusEdit->GetDerivedPosY() );
	}
	
	return FALSE;
}


void EditBox::Create( HWND hWnd, HINSTANCE hInstance )
{
	if( m_hEdit )
		return;

	m_hParentWnd = hWnd;

	DWORD dwS;
	dwS = WS_CHILD|ES_LEFT|ES_MULTILINE|WS_VISIBLE;

#if defined( SRC_OVERSEAS )
	// edit창이 커야 CompositionWindow 위치 설정이 가능하다, CompositionWindow 위치가 edit창을 벗어 나면 독립 윈도우로 출력됨
	m_hEdit = CreateWindowW(L"edit", L"ls-edit", dwS, 0, 0, 1280, 1027, hWnd, NULL, hInstance, NULL); 	
	SendMessage(m_hEdit, EM_LIMITTEXT, MAX_IME_STRING, 0);	
	m_EditProc = (WNDPROC)SetWindowLongW(m_hEdit, GWL_WNDPROC, (LONG)EditProc);
#else
	// edit창이 커야 CompositionWindow 위치 설정이 가능하다, CompositionWindow 위치가 edit창을 벗어 나면 독립 윈도우로 출력됨
	m_hEdit = CreateWindow("edit", "ls-edit", dwS, 0, 0, 1280, 1027, hWnd, NULL, hInstance, NULL);
	SendMessage(m_hEdit, EM_LIMITTEXT, MAX_IME_STRING, 0);	
	m_EditProc = (WNDPROC)SetWindowLong(m_hEdit, GWL_WNDPROC, (LONG)EditProc);
#endif

	ShowWindow( m_hEdit , SW_HIDE );
}


void EditBox::SetString( const char *szText )
{

#if defined( SRC_OVERSEAS )
	StringCbCopy( m_szInput, sizeof( m_szInput ), szText );

	// determine required length of new string
	size_t reqLength = ::MultiByteToWideChar( COUNTRY_CODE_PAGE, 0, szText, strlen(szText), 0, 0 );

	// construct new string of required length
	std::wstring ret( reqLength, L'\0' );

	// convert old string to new string
	::MultiByteToWideChar( COUNTRY_CODE_PAGE, 0, szText, strlen(szText), &ret[0], (int)ret.length() );

	SetWindowTextW( m_hEdit, ret.c_str() );
#else
	StringCbCopy( m_szInput, sizeof( m_szInput ), szText );
	SetWindowText( m_hEdit, m_szInput );
#endif

	SendMessage(m_hEdit, EM_SETSEL , MAX_IME_STRING, -1 ); // 커서를 끝으로
	m_iCurCaretPos = strlen( m_szInput );
}


void EditBox::SetStringNotDeleteComposing( const char *szText )
{
	SetString( szText );
}


void EditBox::SetMaxInputLen( int iLen )
{
	SendMessage( m_hEdit, EM_LIMITTEXT, iLen, 0 );
}


void EditBox::ClearString()
{
	ZeroMemory( m_szInput, sizeof( m_szInput ) );
	SetWindowText( m_hEdit, m_szInput );
}

const char* EditBox::GetString()
{

#if defined( SRC_OVERSEAS )
	std::wstring ret( MAX_IME_STRING, L'\0' );

	GetWindowTextW( m_hEdit, &ret[0], MAX_IME_STRING );

	::WideCharToMultiByte( COUNTRY_CODE_PAGE, 0, &ret[0], ret.length(), m_szInput, ret.length(), "*", NULL );
#else
	GetWindowText(m_hEdit, m_szInput, sizeof( m_szInput ) );
#endif

	m_iCurCaretPos = strlen( m_szInput );
	return m_szInput;
}

void EditBox::ChangeConversionStatus(DWORD dwConvMode, DWORD dwSentMode)
{
	if( dwConvMode == IME_CMODE_ALPHANUMERIC || ( m_dwSavedwConv == 0 && m_dwSavedwSent == 0 ) )
	{
		HIMC hIMC = ImmGetContext(m_hEdit);
		if( hIMC )
		{
			ImmGetConversionStatus(hIMC, &m_dwSavedwConv, &m_dwSavedwSent); 
			ImmReleaseContext( m_hEdit, hIMC );
		}
	}
	else if( dwConvMode == IME_CMODE_NATIVE )
	{
		dwConvMode = m_dwSavedwConv;
		dwSentMode = m_dwSavedwSent;
	}
	
	ioIME::ChangeConversionStatus( dwConvMode, dwSentMode );
}
 
void EditBox::Process()
{
	if( ioEdit::m_pKeyFocusEdit == NULL ) 
	{
		// 불필요한 후보,조합윈도우 제거
		if( m_hSavedIMC == NULL )
			m_hSavedIMC = ImmAssociateContext( m_hParentWnd, NULL );
		return;
	}
	
	if ( GetFocus()!= m_hEdit )	
		SetFocus(m_hEdit);

	if( m_hSavedIMC != NULL )
	{
		ImmAssociateContext( m_hParentWnd, m_hSavedIMC );
		m_hSavedIMC = NULL;
		
	}

	SetCompositionWindow( ioEdit::m_pKeyFocusEdit->GetCaretXPos(), ioEdit::m_pKeyFocusEdit->GetDerivedPosY()-3 ); // -3: 위치 조정

	/*
	RECT rcWindow={0,0,0,0};
	
		if( RenderSystem().IsWindowed() )
			GetWindowRect( RenderSystem().GetHWND(), &rcWindow );
	
		int iCurScreenPosX = ioEdit::m_pKeyFocusEdit->GetDerivedPosX() + (m_iCurCaretPos*4);
		int iCurScreenPosY = ioEdit::m_pKeyFocusEdit->GetDerivedPosY();
	
		if( m_iPreEditScreenPosX == iCurScreenPosX && m_iPreEditScreenPosY == iCurScreenPosY )
			return;
	
		m_iPreEditScreenPosX = iCurScreenPosX;
		m_iPreEditScreenPosY = iCurScreenPosY;
	
		MoveWindow( m_hEdit, m_iPreEditScreenPosX, m_iPreEditScreenPosY, 100,100, FALSE );
		LOG.PrintConsoleLog( 0, "%d:%d", m_iPreEditScreenPosX, m_iPreEditScreenPosY );
	
	SetCompositionWindow( ioEdit::m_pKeyFocusEdit->GetDerivedPosX()  + (m_iCurCaretPos*4), ioEdit::m_pKeyFocusEdit->GetDerivedPosY() );
	SetCandidateWindow( ioEdit::m_pKeyFocusEdit->GetDerivedPosX() + 100  + (m_iCurCaretPos*4), ioEdit::m_pKeyFocusEdit->GetDerivedPosY()  ); */
}

bool EditBox::IsNowComposing()
{
	return false;
}

void  EditBox::SetCompositionWindow( int iX, int iY )
{
	
	HIMC hImc = ImmGetContext(m_hEdit);
	if( !hImc )
		return;

	COMPOSITIONFORM cf;
	cf.dwStyle = CFS_POINT;
	cf.ptCurrentPos.x = iX;          
	cf.ptCurrentPos.y = iY;          

	// 조합 윈도우의 위치를 설정한다.
	ImmSetCompositionWindow(hImc, &cf);
	ImmReleaseContext(m_hEdit, hImc);

/*
		hImc = ImmGetContext(m_hEdit);
		if( !hImc )
			return;
	
		POINT ptPos;
		ptPos.x = iX;          
		ptPos.y = iY;          
	
		ImmSetStatusWindowPos(hImc, &ptPos);
		ImmReleaseContext(m_hEdit, hImc);*/

	
}

void EditBox::SetCandidateWindow( int iX, int iY )
{
	HIMC hImc = ImmGetContext(m_hEdit);
	if( !hImc )
		return;

	CANDIDATEFORM cf;
	cf.dwStyle = CFS_CANDIDATEPOS;
	cf.ptCurrentPos.x = iX;
	cf.ptCurrentPos.y = iY;

	ImmSetCandidateWindow(hImc, &cf);
	ImmReleaseContext(m_hEdit, hImc);
}

bool EditBox::HasFocus( HWND hFocus )
{
	if( ioEdit::m_pKeyFocusEdit == NULL ) 
		return false;

	if( m_hEdit == hFocus )
		return true;

	return false;
}