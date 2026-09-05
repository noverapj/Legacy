// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__F77826AC_E455_4A39_87C3_D426BEC2023F__INCLUDED_)
#define AFX_STDAFX_H__F77826AC_E455_4A39_87C3_D426BEC2023F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../include/Common.h"
#include <winsock2.h>
#include <windows.h>
#include <mswsock.h>
#include <tchar.h>

extern void	Trace( const TCHAR *lpszFormat, ... );
extern void Debug( const TCHAR *lpszFormat, ... );
extern void Information( const TCHAR *lpszFormat, ... );
extern void	Debug();

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__F77826AC_E455_4A39_87C3_D426BEC2023F__INCLUDED_)
