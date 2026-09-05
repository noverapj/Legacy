
#include "stdafx.h"

#include "InputBox.h"
#include "ioIME.h"
#include "ErrorReport.h"
#include "ioWnd.h"
#include "ioEdit.h"

ioIME::ioIME()
{
	m_iInputLen		= 0;
	m_iPreCompLen	= 0;
	m_iCurCaretPos  = 0;
	m_dwProperty	= 0;

	m_lpCandList	= NULL;
	m_iShiftDownPos = -1;

	m_iSelectStart	= -1;
	m_iSelectEnd	= -1;
	m_hParent		= NULL;

	m_bComposition	= false;

	memset( m_szInput, 0, MAX_IME_STRING );
	memset( m_szComp, 0, MAX_COMP_LEN );
	m_iMaxInputLen = MAX_PATH;


	m_bCtrl_V = false;
	m_bCtrl = false;
	m_bV = false;
}

ioIME::~ioIME()
{
	if(m_lpCandList)
	{
		GlobalFree((HANDLE)m_lpCandList);
		m_lpCandList = NULL;
	}
}

void ioIME::SetString( const char *szText )
{
	memset(m_szInput, 0, MAX_IME_STRING);
	strcpy(m_szInput, szText);
	m_iInputLen = strlen(m_szInput);
	m_iCurCaretPos = m_iInputLen;

	DeleteComposingText();
	InitSelectArea();
}

void ioIME::SetStringNotDeleteComposing( const char *szText )
{
	memset(m_szInput, 0, MAX_IME_STRING);
	strcpy(m_szInput, szText);

	if( m_iPreCompLen > 0 )
	{
		m_iInputLen = 0;
		m_iCurCaretPos = 0;
	}
	else
	{
		m_iInputLen = strlen( szText );
		m_iCurCaretPos = m_iInputLen;
	}

	InitSelectArea();
}

void ioIME::SetCaretPos( int iPos )
{
	m_iCurCaretPos = iPos;
	m_iCurCaretPos = max(0, iPos );
	m_iCurCaretPos = min( iPos, (int)strlen(m_szInput) );
}

void ioIME::ClearString()
{
	memset( m_szInput, 0, MAX_IME_STRING );
	m_iInputLen = 0;
	m_iCurCaretPos = 0;	

	DeleteComposingText();
	InitSelectArea();
}

void ioIME::DeleteComposingText()
{
	HIMC hIMC = ImmGetContext(m_hParent);
	if(!hIMC) return;

	memset(m_szComp, 0, MAX_COMP_LEN);
	m_iPreCompLen = 0;

	ImmNotifyIME(hIMC, NI_COMPOSITIONSTR, CPS_CANCEL, 0);
	ImmReleaseContext(m_hParent, hIMC);
}

LRESULT ioIME::OnCreate(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	HIMC	hIMC;
	hIMC = ImmCreateContext();
	m_hOldIMC = ImmAssociateContext(hWnd, hIMC);
	m_dwProperty = ImmGetProperty(GetKeyboardLayout(0), IGP_PROPERTY);
	m_hParent = hWnd;

	CheckCurIMEDesc();	

	if( !m_lpCandList )
		m_lpCandList = (LPCANDIDATELIST)GlobalAlloc(GPTR, sizeof(CANDIDATELIST));

	return 1;
}

LRESULT ioIME::OnDestroy(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	HIMC	hIMC;
	hIMC = ImmAssociateContext(hWnd, m_hOldIMC);
	ImmDestroyContext(hIMC);

	return 1;
}

LRESULT ioIME::OnStartComposition(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if( IsThereSelectArea() )
	{
		OnKeyDown(hWnd, VK_DELETE, 0);
	}

	return 1;
}

LRESULT ioIME::OnComposition(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if( m_iInputLen + 1 >= m_iMaxInputLen )		// HARDCODE: 2byte so +1
	{
		DeleteComposingText();
		return 1;
	}

	HIMC hIMC = ImmGetContext(hWnd);
	if( hIMC == NULL)	return 0;

	ProcessCompStr(hIMC, lParam);
	ProcessResultStr(hIMC, lParam);

	ImmReleaseContext(hWnd, hIMC);

	return 1;
}

void ioIME::ProcessCompStr(HIMC	hIMC, LPARAM lParam)	// default Korea
{
	if( !( lParam & GCS_COMPSTR ) )	return;

	LONG iStrLen = 0;

	iStrLen = ImmGetCompositionString(hIMC, GCS_COMPSTR, m_szComp, MAX_COMP_LEN);
	m_szComp[iStrLen] = NULL;
	
	DeleteTokenToString(&iStrLen); 	
	
	MakeInputString(m_szComp, iStrLen);
}

void ioIME::ProcessResultStr(HIMC hIMC, LPARAM lParam)
{
	if( !(lParam & GCS_RESULTSTR) )	return;

	LONG	iStrLen = 0;

	iStrLen = ImmGetCompositionString(hIMC, GCS_RESULTSTR, NULL, 0);
	ImmGetCompositionString(hIMC, GCS_RESULTSTR, m_szComp, MAX_COMP_LEN);
	
	if( m_iInputLen + iStrLen > m_iMaxInputLen)	
		iStrLen = m_iMaxInputLen - m_iInputLen;

	m_szComp[iStrLen] = NULL;
	
	DeleteTokenToString(&iStrLen); 	

	MakeInputString(m_szComp, iStrLen);

	m_bComposition = false;
	m_iCurCaretPos += iStrLen;
	m_iInputLen += iStrLen;
	m_iPreCompLen = 0;
	m_szComp[0] = NULL;
}

int ioIME::FilterCompString(char *szComp, int strLen)
{
	for( int i=0, j=0 ; i<strLen ; i++)
	{
		if(szComp[i] != -95 && szComp[i] != 32 )
		{
			szComp[j] = szComp[i];
			j++;
		}
	}
	szComp[j] = NULL;

	return j;
}

LRESULT ioIME::OnEndComposition(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	return 1;
}

LRESULT ioIME::OnChar(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	m_bComposition = false;

	switch( wParam )
	{
	case VK_ESCAPE:
	case VK_TAB:
	case VK_RETURN:
	case VK_BACK:
	case '%':
		return 0;
	case VK_SPACE:
		if( ioWnd::m_pTopModalWnd && ioEdit::m_pKeyFocusEdit == NULL ) // ioEdit::m_pKeyFocusEdit == NULL : 모달 edit 창에 스페이스키를 입력하기 위해서
			return 0;
		break;
	}

	if( IsThereSelectArea() )
	{
		OnKeyDown(hWnd, VK_DELETE, 0);
	}

	if( m_iInputLen >= m_iMaxInputLen )
		return 1;

	if( m_iCurCaretPos != m_iInputLen )	// Middle Insert, so make 1 blank
	{		
		int iTempLen = strlen(&m_szInput[m_iCurCaretPos]);
		memmove(&m_szInput[m_iCurCaretPos + 1], &m_szInput[m_iCurCaretPos], iTempLen);
	}
	
	m_szInput[m_iCurCaretPos] = (BYTE)wParam;
	m_iCurCaretPos++;
	m_iInputLen++;

	m_szInput[m_iInputLen] = NULL;
	return 1;
}

LRESULT ioIME::OnIMESetContext(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
//	if( lParam & ISC_SHOWUICANDIDATEWINDOW)
//		lParam &= ~ISC_SHOWUICANDIDATEWINDOW;		// Hide Candidate Window

//	if( m_dwProperty & IME_PROP_AT_CARET)
		lParam &= ~ISC_SHOWUICOMPOSITIONWINDOW;		// Hide Composition Window

	return DefWindowProc(hWnd, WM_IME_SETCONTEXT, wParam, lParam);
}

LRESULT ioIME::OnNotify(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	HIMC	hIMC = NULL;
	DWORD	dwSize, dwCount;

	dwSize = dwCount = 0;

	switch(wParam)
	{
	case IMN_OPENSTATUSWINDOW:
	case IMN_CLOSESTATUSWINDOW:
	case IMN_PRIVATE:
	case IMN_SETCONVERSIONMODE:
	case IMN_SETSENTENCEMODE:
		break;
/*	
	case IMN_GUIDELINE:
	case IMN_SETOPENSTATUS:
	case IMN_SETCOMPOSITIONFONT:
	case IMN_SETCOMPOSITIONWINDOW:
	case IMN_SETSTATUSWINDOWPOS:
	case IMN_SETCANDIDATEPOS:
		DefWindowProc(hWnd, WM_IME_NOTIFY, wParam, lParam);
		break;
*/
// 	case IMN_OPENCANDIDATE:
// 	case IMN_CHANGECANDIDATE:
// 		hIMC = ImmGetContext(hWnd);
// 
// 		if( dwSize = BASE_GetCandidateList(hIMC, 0, NULL, 0, &dwCount) )
// 		{
// 			GlobalFree((HANDLE)m_lpCandList);
// 			m_lpCandList = (LPCANDIDATELIST)GlobalAlloc(GPTR, dwSize);
// 			BASE_GetCandidateList(hIMC, 0, m_lpCandList, dwSize, &dwCount);
// 		}
// 		else
// 		{
// 			memset(m_lpCandList, 0, sizeof(CANDIDATELIST));
// 		}
// 
// 		ImmReleaseContext(hWnd,hIMC);
// 		break;
// 	case IMN_CLOSECANDIDATE:
// 		memset(m_lpCandList, 0, sizeof(CANDIDATELIST));
// 		break;
	default:
		DefWindowProc(hWnd, WM_IME_NOTIFY, wParam, lParam);
		break;
    }

	return 0;
}

LRESULT ioIME::OnInputLangChange(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	m_dwProperty = ImmGetProperty(GetKeyboardLayout(0), IGP_PROPERTY);

	HIMC hIMC = ImmGetContext(hWnd);
	if( hIMC )
	{
		CANDIDATEFORM	cand_form;

		if( !( m_dwProperty & IME_PROP_AT_CARET ) )
		{
			for( DWORD i=0 ; i<4 ; i++)
			{
				if( !ImmGetCandidateWindow(hIMC, i, &cand_form) )	continue;
				if( cand_form.dwStyle == CFS_DEFAULT )	continue;

				cand_form.dwStyle = CFS_DEFAULT;
				ImmSetCandidateWindow(hIMC, &cand_form);
			}
		}

		ImmReleaseContext(hWnd, hIMC);
	}

	CheckCurIMEDesc();

	return DefWindowProc(hWnd, WM_INPUTLANGCHANGE, wParam, lParam);
}

void ioIME::CheckCurIMEDesc()
{
	memset(m_szIMEDesc, 0, MAX_IMEDESC_LEN);

	ImmGetDescription(GetKeyboardLayout(0), m_szIMEDesc, MAX_IMEDESC_LEN);
}

void ioIME::ChangeConversionStatus(DWORD dwConvMode, DWORD dwSentMode)
{
	HIMC hIMC = ImmGetContext(m_hParent);
	if( hIMC )
	{
		ImmSetConversionStatus(hIMC, dwConvMode, dwSentMode);
		ImmReleaseContext(m_hParent, hIMC);	
	}
}

LRESULT ioIME::OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch(wParam)
	{
/*
	case VK_SHIFT:
		if( m_iInputLen == 0 )
			return 0;

		if( IsThereSelectArea() )
		{
			if(m_iCurCaretPos <= m_iSelectStart)
				m_iShiftDownPos = m_iSelectEnd;
			else
				m_iShiftDownPos = m_iSelectStart;
		}
		else
		{
			m_iShiftDownPos = m_iCurCaretPos;
			m_iSelectStart = m_iCurCaretPos;
			m_iSelectEnd = m_iCurCaretPos;
		}
		break;
	case VK_END:
		if( IsKeyDownWithShift() )
		{
			if( m_iCurCaretPos <= m_iSelectStart )
				m_iSelectStart = m_iSelectEnd;

			m_iSelectEnd = m_iInputLen;
		}
		else
		{
			InitSelectArea();
		}

		m_iCurCaretPos = m_iInputLen;
		break;
	case VK_HOME:
		if( IsKeyDownWithShift() )
		{
			if(m_iCurCaretPos >= m_iSelectEnd)
				m_iSelectEnd = m_iSelectStart;

			m_iSelectStart = 0;
		}
		else
		{
			InitSelectArea();
		}

		m_iCurCaretPos = 0;
		break;
*/
	case VK_LEFT:
	case VK_RIGHT:
		if( ioEdit::m_pKeyFocusEdit && ioEdit::m_pKeyFocusEdit->HasExWndStyle( IWS_EX_MULTI_EDIT ) )
			break;

		if(wParam == VK_LEFT)
		{
			if( IsEnglish(m_iCurCaretPos-1) )
				m_iCurCaretPos--;
			else
				m_iCurCaretPos -= 2;
		}
		else		// wParam == VK_RIGHT
		{
			if( IsEnglish(m_iCurCaretPos) )
				m_iCurCaretPos++;
			else
				m_iCurCaretPos += 2;
		}

		m_iCurCaretPos = max(0, min( m_iCurCaretPos, (int)strlen(m_szInput) ) );

		if( IsKeyDownWithShift() )
		{
			if(m_iCurCaretPos < m_iShiftDownPos)
				m_iSelectStart = m_iCurCaretPos;
			else if(m_iCurCaretPos > m_iShiftDownPos)
				m_iSelectEnd = m_iCurCaretPos;
			else 
			{
				m_iSelectStart = m_iCurCaretPos;
				m_iSelectEnd = m_iCurCaretPos;
			}
		}
		else
		{
			InitSelectArea();
		}
		break;

	case VK_DELETE:
	case VK_BACK:
		if( DeleteSelectArea() )
			break;

		if( wParam == VK_DELETE)
		{
			if( m_iInputLen == 0 || m_iCurCaretPos == m_iInputLen )
				return 0;

			if( !IsEnglish(m_iCurCaretPos) )
			{
				m_iInputLen -= 2;
			}
			else
			{
				m_iInputLen--;
			}

			m_iInputLen = max( 0, m_iInputLen );
		}
		else		// wParam == VK_BACK
		{
			if(m_iInputLen == 0 || m_iCurCaretPos == 0 )
				return 0;

			if(!IsEnglish(m_iCurCaretPos-1))
			{
				m_iInputLen -= 2;
				m_iCurCaretPos -= 2;
			}
			else
			{
				m_iInputLen--;
				m_iCurCaretPos--;
			}

			m_iInputLen = max( 0, m_iInputLen );
			m_iCurCaretPos = max( 0, m_iCurCaretPos );
		}

		if(!IsEnglish(m_iCurCaretPos))
		{
			memmove(&m_szInput[m_iCurCaretPos], &m_szInput[m_iCurCaretPos+2], strlen(&m_szInput[m_iCurCaretPos+2]) );
			m_szInput[m_iInputLen]   = 0;
			m_szInput[m_iInputLen+1] = 0;
		}
		else
		{
			memmove(&m_szInput[m_iCurCaretPos], &m_szInput[m_iCurCaretPos+1], strlen(&m_szInput[m_iCurCaretPos+1]) );
			m_szInput[m_iInputLen] = 0;
		}
		break;
	}

	return 1;
}

LRESULT ioIME::OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if(wParam == VK_SHIFT)
		m_iShiftDownPos = -1;

	return 1;
}

BOOL ioIME::IMEWinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_CREATE:
		OnCreate(hWnd, wParam, lParam);
		return FALSE;
	case WM_IME_STARTCOMPOSITION:
		OnStartComposition(hWnd, wParam, lParam);
		return TRUE;
	case WM_IME_COMPOSITION:
		OnComposition(hWnd, wParam, lParam);
		return TRUE;
	case WM_IME_ENDCOMPOSITION:
		OnEndComposition(hWnd, wParam, lParam);
		return TRUE;
	case WM_IME_SETCONTEXT:
		OnIMESetContext(hWnd, wParam, lParam);
		return TRUE;
	case WM_INPUTLANGCHANGE:
		OnInputLangChange(hWnd, wParam, lParam);
		return TRUE;
	case WM_CHAR:
		OnChar(hWnd, wParam, lParam);
		return FALSE;
	case WM_KEYDOWN:
		OnKeyDown(hWnd, wParam, lParam);
		return FALSE;
	case WM_KEYUP:
		OnKeyUp(hWnd, wParam, lParam);
		return FALSE;
	case WM_IME_NOTIFY:
		OnNotify(hWnd, wParam, lParam);
		return TRUE;
	case WM_DESTROY:
		OnDestroy(hWnd, wParam, lParam);
		return FALSE;
	}
	return FALSE;
}

bool ioIME::IsThereSelectArea()
{
	if( m_iSelectStart == m_iSelectEnd )
		return false;

	return true;
}

bool ioIME::IsEnglish(int iCheckPos)
{
	if( (int)strlen(m_szInput) < iCheckPos )
		return false;

	enum TCodeType { ctEng, ctFirst, ctLast };

	TCodeType	type = ctEng;

	for( int i=0 ; m_szInput[i] != NULL ; i++ )
	{
		if( type == ctFirst)
		{
			type = ctLast;
		}
		else
		{

#if defined( SRC_OVERSEAS )

#if defined( MULTI_BYTE_CHECK )
			if( IsDBCSLeadByteEx( COUNTRY_CODE_PAGE, m_szInput[i] ) )
#else
			if( false && IsDBCSLeadByteEx( COUNTRY_CODE_PAGE, m_szInput[i] ) )
#endif

#else
			if( IsDBCSLeadByte( m_szInput[i] ) )
#endif

				type = ctFirst;
			else
				type = ctEng;
		}

		if( i == iCheckPos)
			break;
	}

	if( type == ctEng )
		return true;

	return false;
}

void ioIME::InitSelectArea()
{
	m_iShiftDownPos = m_iSelectStart = m_iSelectEnd = -1;
}

bool ioIME::IsKeyDownWithShift()
{
	if( m_iShiftDownPos == -1)
		return false;

	return true;
}

bool ioIME::DeleteSelectArea()
{
	if( IsThereSelectArea() == false )	return false;

	int iSelectLen = abs(m_iSelectEnd - m_iSelectStart);

	if( iSelectLen == 0 )
	{
		InitSelectArea();
		return false;
	}

	m_iInputLen -= iSelectLen;

	if( m_iCurCaretPos > m_iSelectStart )
		m_iCurCaretPos -= iSelectLen;

	if( m_iInputLen == 0)
	{
		ClearString();
		return true;
	}

	memmove( &m_szInput[m_iSelectStart], &m_szInput[m_iSelectEnd], strlen(&m_szInput[m_iSelectEnd]) );
	memset( &m_szInput[m_iInputLen], 0, iSelectLen );

	InitSelectArea();

	return true;
}

void ioIME::MakeInputString(char *szComp, int iCompStrLen)
{
	char *pCurStart = m_szInput + m_iCurCaretPos;

	if( iCompStrLen == 0 )	// delete Composing char all while Composing
	{
		if( m_iPreCompLen > 0 )
			memmove(pCurStart, pCurStart + m_iPreCompLen, strlen(pCurStart + m_iPreCompLen) );

		m_iPreCompLen	= 0;
		szComp[0]		= NULL;
		m_bComposition	= false;
		m_szInput[m_iInputLen] = NULL;
		return;
	}

	if( iCompStrLen == m_iPreCompLen )
	{
		memcpy(pCurStart, szComp, iCompStrLen);
	}
	else
	{
		if( m_iCurCaretPos == m_iInputLen)	// tail Insert
		{
			memcpy(pCurStart, szComp, iCompStrLen);
			pCurStart[iCompStrLen] = NULL;
		}
		else	// middle insert
		{
			int TempEndLen = strlen(pCurStart + m_iPreCompLen);

			memmove( pCurStart + iCompStrLen, pCurStart + m_iPreCompLen, TempEndLen);
			memcpy( pCurStart, szComp, iCompStrLen);

			pCurStart[TempEndLen + iCompStrLen] = NULL;
		}
		m_iPreCompLen = iCompStrLen;
	}
	m_bComposition = true;
}

void ioIME::DeleteTokenToString( long *strLen )
{
	if(( *strLen - 1) > 0 )
	{
		if(m_szComp[*strLen - 1] == '%')
		{
			m_szComp[*strLen - 1] = NULL;
			*strLen -= 1;
		}
	}
}

void ioIME::Create( HWND hWnd, HINSTANCE hInstance )
{

}


void ioIME::OnCheckPasteStr()
{
	if( GetAsyncKeyState( VK_CONTROL ) & 0x8001 )
		m_bCtrl = true;
	else
		m_bCtrl = false;

	if( GetAsyncKeyState( 'V' ) & 0x8001 )
		m_bV = true;
	else 
		m_bV = false;

	bool bPaste = false;

	if( m_bCtrl_V == false )
	{
		if( m_bCtrl && m_bV )
		{
			m_bCtrl_V = true;
			bPaste = true;
		}
	}
	else
	{
		if( !m_bCtrl || !m_bV )
			m_bCtrl_V = false;
	}

	if( bPaste )
	{
		ioHashString strTest;
		if(::OpenClipboard(NULL))
		{
			HANDLE hHandle = GetClipboardData( CF_TEXT );
			if( hHandle )
				strTest = (char*)hHandle;

			CloseClipboard();

			char szBuf[MAX_IME_STRING] = "";
			wsprintf( szBuf, "%s", strTest.c_str() );

			int nStrSize= strlen( szBuf );

			for( int i = 0; i < nStrSize; i++ )
				OnChar( m_hParent, szBuf[i], 0 );
		}
	}
}

void ioIME::Process()
{
	if( g_InputBox.GetUseClipboard() )
		OnCheckPasteStr();
}

const char* ioIME::GetString()
{
	return m_szInput;
}

DWORD BASE_GetCandidateList( HIMC hIMC, DWORD dwIndex, LPCANDIDATELIST lpCand, DWORD dwBufLen, LPDWORD lpdwListCount)
{
	// the normal call
	return ImmGetCandidateList(hIMC, dwIndex, lpCand, dwBufLen);
/*
	DWORD dwSize = 0;
	LPINPUTCONTEXT  lpIMC = NULL;
	LPCANDIDATEINFO lpCandInfo = NULL;
	LPCANDIDATELIST lpCandList = NULL;

	// 1) Go into the IMC
	lpIMC = (LPINPUTCONTEXT)ImmLockIMC(hIMC);
	if (!lpIMC)
		return 0;

	if (!lpIMC->hCandInfo)
	{
		ImmUnlockIMC(hIMC);
		return 0;
	}

	lpCandInfo = (CANDIDATEINFO*)ImmLockIMCC(lpIMC->hCandInfo);
	if (!lpCandInfo)
	{
		ImmUnlockIMC(hIMC);
		return 0;
	}

	//2) Get Candlist of the current IME
	lpCandList = (LPCANDIDATELIST)((LPBYTE)lpCandInfo + lpCandInfo->dwOffset[0]);
	dwSize = 0; //Init the return value
	
	if(lpCandList->dwCount)
	{
		if(!dwBufLen||!lpCand)	//3) When lpCand = NULL or dwBufLen = 0 , We just need sizeof candidatelist
		{
			dwSize = sizeof(CANDIDATELIST)  + lpCandList->dwOffset[lpCandList->dwCount] + 80;
		}
		else	//4) Copy the CandList out 
		{
			dwSize = dwBufLen;
			lpCandList->dwPageStart = lpCandList->dwSelection / lpCandList->dwPageSize * lpCandList->dwPageSize;
			CopyMemory((PVOID *)lpCand,(BYTE *)lpCandList, dwSize );
		}
		*lpdwListCount = lpCandList->dwCount;
	}
	//5) Go out The IMC 
	ImmUnlockIMCC(lpIMC->hCandInfo);
	ImmUnlockIMC(hIMC);
	return dwSize;
*/
}

