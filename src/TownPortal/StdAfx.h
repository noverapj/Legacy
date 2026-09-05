// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__7E5C6BAF_57ED_4B18_921F_4518951C5AA1__INCLUDED_)
#define AFX_STDAFX_H__7E5C6BAF_57ED_4B18_921F_4518951C5AA1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// Insert your headers here
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

#include "../LSLog/include/LSLog.h"

#ifdef STATIC_TOWN_PORTAL_API
	#ifdef _DEBUG
		#pragma comment(lib, "../../lib/LSLogStaticd.lib" )
	#else
		#pragma comment(lib, "../../lib/LSLogStatic.lib" )
	#endif
#else
	#ifdef _DEBUG
		#pragma comment(lib, "../../lib/LSLogd.lib" )
	#else
		#pragma comment(lib, "../../lib/LSLog.lib" )
	#endif
#endif

#ifdef STATIC_TOWN_PORTAL_API
	#define TOWN_PORTAL_API
#else
	#ifdef EXPORT_TOWN_PORTAL
		#define TOWN_PORTAL_API __declspec(dllexport)
	#else
		#define TOWN_PORTAL_API __declspec(dllimport)
	#endif
#endif

// disable: "<type> needs to have dll-interface to be used by clients'
// Happens on STL member variables which are not public therefore is ok
#pragma warning (disable : 4251)

#define COMPARE(x,min,max) (((x)>=(min))&&((x)<(max)))
#define SAFEDELETE(x)		if(x != NULL) { delete x; x = NULL; }
#define SAFEDELETEARRAY(x)	if(x != NULL) { delete [] x; x = NULL; }
#define SAFERELEASE(x)		if(x != NULL) { x->Release(); x = NULL; }

// TODO: reference additional headers your program requires here

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__7E5C6BAF_57ED_4B18_921F_4518951C5AA1__INCLUDED_)
