
#include "stdafx.h"
#include "BlockKeyBoard.h"

HHOOK CBlockKeyBoard::m_hHook = NULL;
bool CBlockKeyBoard::m_bAltTab = false;

CBlockKeyBoard::CBlockKeyBoard( bool bWin98Series )
{
	m_bWin98Series = bWin98Series;
	m_bBlock = false;
}

CBlockKeyBoard::~CBlockKeyBoard()
{

}

LRESULT CALLBACK CBlockKeyBoard::LowLevelKeyboardProc (int nCode, WPARAM wParam, LPARAM lParam)
{
	// By returning a non-zero value from the hook procedure, the
    // message does not get passed to the target window
    KBDLLHOOKSTRUCT *pkbhs = (KBDLLHOOKSTRUCT *) lParam;
    BOOL bControlKeyDown = 0;

	if(nCode >= 0)
    {    
		// Check to see if the CTRL key is pressed
		//bControlKeyDown = GetAsyncKeyState (VK_CONTROL) < 0 ;

		// Disable CTRL+ESC
		//if (pkbhs->vkCode == VK_ESCAPE && bControlKeyDown)		
		//	return 1;		
		
		// Disable ALT+TAB
		//if (pkbhs->vkCode == VK_TAB && pkbhs->flags & LLKHF_ALTDOWN)
		//{
		//	m_bAltTab = true;
		//	return 1;
		//}
		
		// Disable ALT+ESC
		//if (pkbhs->vkCode == VK_ESCAPE && pkbhs->flags & LLKHF_ALTDOWN)
		//	return 1;

		// Disable ALT+F6
		//if(pkbhs->vkCode == VK_F6 && pkbhs->flags & LLKHF_ALTDOWN )
		//	return 1;

		// Disable ALT+F4
		//if(pkbhs->vkCode == VK_F4 && pkbhs->flags & LLKHF_ALTDOWN )
		//	return 1;
		
		// Disable Application Key ( Right Mouse Button Key )			
		//if(pkbhs->vkCode == VK_APPS)
		//	return 1;		

		// Disable ALT, CTRL ( CTRL 키가 정상 동작하지 않음 )
		//if(wParam == WM_SYSKEYDOWN || wParam == WM_SYSKEYUP)
		//	return 1;		

		// Ctrl down
		if( GetAsyncKeyState (VK_CONTROL) < 0 )
			return 1;

		// Alt down
		if( pkbhs->flags & LLKHF_ALTDOWN )
			return 1;
    }

    return CallNextHookEx(m_hHook, nCode, wParam, lParam);
} 

void CBlockKeyBoard::DisableSysKey( HINSTANCE hInstance )
{
	if( m_bBlock )	return;

	if( m_bWin98Series )
	{			
		int	nOldVal;
		SystemParametersInfo(SPI_SETSCREENSAVERRUNNING, TRUE, &nOldVal, 0); 
	}
	else
	{	
		m_hHook = SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)LowLevelKeyboardProc, hInstance, 0);	// 키보드 후킹
		if(m_hHook == NULL)
		{			
			LOG.PrintTimeAndLog( 0,"EXCEPT:HOOK Failed, Error Code:%d", GetLastError() );
		}
	}

	LOG.PrintTimeAndLog( 0,"Disable Sys Key");
	m_bBlock = true;
}

void CBlockKeyBoard::EnableSysKey()
{
	if( !m_bBlock )		return;	

	if( m_bWin98Series )
	{			
		int	nOldVal;
		SystemParametersInfo(SPI_SETSCREENSAVERRUNNING, FALSE, &nOldVal, 0); 
	}
	else
	{								
		UnhookWindowsHookEx(m_hHook);
	}

	LOG.PrintTimeAndLog( 0,"Enable Sys Key");	
	m_bBlock = false;
}

bool CBlockKeyBoard::CheckAltTabKey()
{
	if( !m_bAltTab )
		return false;

	m_bAltTab = false;
	return true;
}