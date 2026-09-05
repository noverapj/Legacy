// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__C1E9284D_7E49_4065_BC9E_7F9412036125__INCLUDED_)
#define AFX_STDAFX_H__C1E9284D_7E49_4065_BC9E_7F9412036125__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// Insert your headers here
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#ifdef STATIC_PAC_API
	#define PAC_API
#else
	#ifdef EXPORT_PAC_API
		#define PAC_API __declspec(dllexport)
	#else
		#define PAC_API __declspec(dllimport)
	#endif
#endif

// disable: "<type> needs to have dll-interface to be used by clients'
// Happens on STL member variables which are not public therefore is ok
#   pragma warning(disable:4251)
#   pragma warning(disable:4995)


#ifdef STATIC_PAC_API
	#ifdef _DEBUG
		#pragma comment(lib, "ZipArchive/ZipArchiveStaticd.lib")
	#else
		#pragma comment(lib, "ZipArchive/ZipArchiveStatic.lib")
	#endif
#else
	#ifdef _DEBUG
		#pragma comment(lib, "ZipArchive/ZipArchived.lib")
	#else
		#pragma comment(lib, "ZipArchive/ZipArchive.lib")
	#endif
#endif

#include <windows.h>

// TODO: reference additional headers your program requires here

#define COMPARE(x,min,max) (((x)>=(min))&&((x)<(max)))
#define SAFEDELETE(x)		if(x != NULL) { delete x; x = NULL; }
#define SAFEDELETEARRAY(x)	if(x != NULL) { delete [] x; x = NULL; }

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__C1E9284D_7E49_4065_BC9E_7F9412036125__INCLUDED_)
