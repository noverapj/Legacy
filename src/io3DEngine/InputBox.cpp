

#include "stdafx.h"

#include "ioIME.h"
#include "InputBox.h"
#include "EditBox.h"
#include "ioLocalManagerParent.h"

template<> ioInputBox* Singleton< ioInputBox >::ms_Singleton = 0;

ioInputBox::ioInputBox( HWND hWnd,  HINSTANCE hInstance )
{
	m_pIME     = NULL;
	if( ioLocalManagerParent::IsIME() )
		m_pIME = new ioIME;
	else
	{
		m_pIME = new EditBox;
		if( m_pIME )
			m_pIME->Create( hWnd, hInstance );
	}
	
	m_iWheelMoveAmt = 0;
	
	m_bEnter = false;
	m_bTab   = false;
	m_bLeft  = false;
	m_bRight = false;
	m_bDel   = false;

	m_bUp		 = false;
	m_bDown		 = false;
	m_bBackSpace = false;
	m_bKeyDown   = false;

	m_bUseClipboard = false;
}

ioInputBox::~ioInputBox()
{
	SAFEDELETE( m_pIME );
}
 
BOOL ioInputBox::InputBoxProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	if( m_pIME && m_pIME->IMEWinProc(hWnd, message, wParam, lParam) )	
		return TRUE;

	switch( message )
	{
	case WM_MOUSEWHEEL:
		m_iWheelMoveAmt = (short)HIWORD(wParam);
		break;
	case WM_KEYDOWN:
		switch( wParam )
		{
		case VK_TAB:
			m_bTab = true;
			return TRUE;
		case VK_RETURN:
			if( !(lParam & 0x40000000 )	)// Key Press ??
			{
				m_bEnter = !( m_pIME && m_pIME->IsNowComposing() );
			}
			else	// Key Repeat
			{
				m_bEnter = false;
			}
			break;
		case VK_BACK:
			m_bBackSpace = true;
			break;
		case VK_LEFT:
			m_bLeft = true;
			break;
		case VK_RIGHT:
			m_bRight = true;
			break;
		case VK_UP:			
			m_bUp = true;			
			break;
		case VK_DOWN:
			m_bDown = true;
			break;
		case VK_DELETE:
			m_bDel = true;
			break;
		default:
			m_bKeyDown = true;
			break;
		}
	case WM_CONTEXTMENU:
		return TRUE;
	}

	return FALSE;
}

void ioInputBox::SetString( const char *szText )
{
	if( m_pIME )
		m_pIME->SetString(szText);
}

void ioInputBox::SetStringNotDeleteComposing( const char *szText )
{
	if( m_pIME )
		m_pIME->SetStringNotDeleteComposing( szText );
}

void ioInputBox::SetCaretPos( int iPos )
{
	if( m_pIME )
		m_pIME->SetCaretPos( iPos );
}

const char* ioInputBox::GetString() const
{
	if( m_pIME )
		return m_pIME->GetString();

	return NULL;
}

const int ioInputBox::GetCaretPos() const
{
	if( m_pIME )
		return m_pIME->GetCaretPos();

	return 0;
}

void ioInputBox::Clear()
{
	if( m_pIME )
		m_pIME->ClearString();
}

void ioInputBox::SetLimitText( int iLen )
{
	if( m_pIME )
		m_pIME->SetMaxInputLen( iLen );
}

void ioInputBox::SetNativeInput()
{
	if( m_pIME )
		m_pIME->ChangeConversionStatus( ioLocalManagerParent::GetNativeIMEConvMode( ioLocalManagerParent::GetLocalType() ) );
}

void ioInputBox::SetEnglishInput()
{
	if( m_pIME )
		m_pIME->ChangeConversionStatus( IME_CMODE_ALPHANUMERIC );
}

void ioInputBox::GetFocusToMe( HWND hWnd )
{
	if( hWnd != GetFocus() )
		SetFocus(hWnd);
}

bool ioInputBox::IsEnter()
{
	if( m_bEnter )
	{
		m_bEnter = false;
		return true;
	}
	else
		return false;
}

bool ioInputBox::IsBackSpace()
{
	if( m_bBackSpace )
	{
		m_bBackSpace = false;
		return true;
	}
	else
		return false;
}

bool ioInputBox::IsTab()
{
	if( m_bTab )
	{
		m_bTab = false;
		return true;
	}
	else
		return false;
}

bool ioInputBox::IsLeft()
{
	if( m_bLeft )
	{
		m_bLeft = false;
		return true;
	}
	else
		return false;
}

bool ioInputBox::IsRight()
{
	if( m_bRight )
	{
		m_bRight = false;
		return true;
	}
	else
		return false;
}

bool ioInputBox::IsUp()
{
	if( m_bUp )
	{
		m_bUp = false;
		return true;
	}
	else
		return false;
}

bool ioInputBox::IsDown()
{
	if( m_bDown )
	{
		m_bDown = false;
		return true;
	}
	else
		return false;
}

bool ioInputBox::IsDel()
{
	if( m_bDel )
	{
		m_bDel = false;
		return true;
	}
	else
		return false;
}

bool ioInputBox::IsKeyDown()
{
	if( m_bKeyDown )
	{
		m_bKeyDown = false;
		return true;
	}
	else
		return false;
}

void ioInputBox::ClearKeyState()
{
	m_iWheelMoveAmt = 0;
	m_bEnter = false;
	m_bTab   = false;
	m_bLeft  = false;
	m_bRight = false;
	m_bDel   = false;

	m_bKeyDown   = false;
	m_bUp		 = false;
	m_bDown		 = false;
	m_bBackSpace = false;
	m_bKeyDown   = false;
}

ioInputBox& ioInputBox::GetSingleton()
{
	return Singleton< ioInputBox >::GetSingleton();
}

void ioInputBox::Process()
{
	if( m_pIME )
		m_pIME->Process();
}

bool ioInputBox::HasFocus( HWND hFocus )
{
	if( m_pIME )
		return m_pIME->HasFocus( hFocus );

	return false;
}