// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__B8078AF7_7B36_44F0_A0FA_9C440CD78637__INCLUDED_)
#define AFX_STDAFX_H__B8078AF7_7B36_44F0_A0FA_9C440CD78637__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// Insert your headers here
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <direct.h>
#include <assert.h>
#include <conio.h>

#include "include/ConsoleTextColorDefine.h"

#ifndef LOG_API
	#define LOG_API __declspec(dllexport)
#endif

// TODO: reference additional headers your program requires here

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__B8078AF7_7B36_44F0_A0FA_9C440CD78637__INCLUDED_)
