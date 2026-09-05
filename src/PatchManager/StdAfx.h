// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__71C463A1_22B4_4766_AF89_624917AD702D__INCLUDED_)
#define AFX_STDAFX_H__71C463A1_22B4_4766_AF89_624917AD702D__INCLUDED_

#define _WIN32_WINNT 0x500

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT
#include <afxinet.h>

#define STATIC_PAC_API
#define PATCH_PAC_API
#define STATIC_LS_LOG

#pragma comment(lib,"Windowscodecs.lib")

#include "../ioPac/ioPac.h"
#include "../LSLog/include/LSLog.h"
#include "ioINILoaderAU.h"
#include <afxcontrolbars.h>
#pragma warning(disable:4786)
// for <strsafe.h>
#pragma warning(disable:4995)

#ifdef _DEBUG
    #pragma comment(lib, "../../lib/LSLogStaticd.lib" )
	#pragma comment(lib, "../../lib/ioPacStaticPatchd.lib" )
#else
	#pragma comment(lib, "../../lib/LSLogStatic.lib" )
	#pragma comment(lib, "../../lib/ioPacStaticPatch.lib" )
#endif

#define TIME_ID_EXIT    2 
#define TIME_ID_DISPLAY 3
#define COMPARE(x,min,max) (((x)>=(min))&&((x)<(max)))

// zone type
enum ZoneType
{
	ZONE_TYPE_SHIPPING     = 1,
	ZONE_TYPE_OFFICE       = 2,
	ZONE_TYPE_OFFICE_ADMIN = 3,
};

#define SAFEDELETE(x) if(x != NULL) { delete x; x = NULL; }

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
#endif // !defined(AFX_STDAFX_H__71C463A1_22B4_4766_AF89_624917AD702D__INCLUDED_)
