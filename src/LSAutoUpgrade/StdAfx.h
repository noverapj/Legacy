// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__71C463A1_22B4_4766_AF89_624917AD702D__INCLUDED_)
#define AFX_STDAFX_H__71C463A1_22B4_4766_AF89_624917AD702D__INCLUDED_

#define _WIN32_WINNT 0x501
//#define _WIN32_WINNT 0x500

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

#ifdef SHIPPING
	#define STATIC_PAC_API
#else
	#define STATIC_PAC_API
	#define PATCH_PAC_API
#endif 
#define STATIC_LS_LOG
#define STATIC_TOWN_PORTAL_API
#define __EX

#include "../ioPac/ioPac.h"
#include "FireWall/FirewallWrapper.h"
#include "../LSLog/include/LSLog.h"
#include "../TownPortal/include/TownPortal.h"
#include "resource.h"

#pragma warning(disable:4786)
// for <strsafe.h>
#pragma warning(disable:4995)
// for string
#pragma warning(disable:4996)

#ifdef _DEBUG
	#ifdef SHIPPING
		#pragma comment(lib, "../../lib/ioPacStaticd.lib" )
	#else
	 	#pragma comment(lib, "../../lib/ioPacStaticPatchd.lib" )
	#endif
	//#pragma comment(lib, "../../lib/ioPacd.lib" )
	#pragma comment(lib, "FireWall/bin/FireWalld.lib")
	#pragma comment(lib, "../../lib/LSLogStaticd.lib" )
	#pragma comment(lib, "../../lib/TownPortalStaticd.lib" )
#else
	#ifdef SHIPPING
		#pragma comment(lib, "../../lib/ioPacStatic.lib" )
	#else
		#pragma comment(lib, "../../lib/ioPacStaticPatch.lib" )
	#endif
	#pragma comment(lib, "FireWall/bin/FireWall.lib")
	#pragma comment(lib, "../../lib/LSLogStatic.lib" )
	#pragma comment(lib, "../../lib/TownPortalStatic.lib" )
#endif

#define SAFEDELETE(x) if(x != NULL) { delete x; x = NULL; }
#define SAFEDELETEARRAY(x) if(x != NULL) { delete [] x; x = NULL; }
#define COMPARE(x,min,max) (((x)>=(min))&&((x)<(max)))

#pragma comment(lib,"Windowscodecs.lib")

// DB에 저장되는 값으로 순차적으로 증가하지 않는다.
enum ChannelingType
{
	CNT_NONE        =   -1,
	CNT_NORMAL      =   0,
	CNT_MGAME       =   300,
	CNT_DAUM        =   400,
	CNT_BUDDY       =   500,
	CNT_NAVER       =   600,
};

enum NetworkState
{
	NS_START     = 0,
	NS_CONNECTED = 1,
	NS_SENT      = 2,
};



// 국가 코드 페이지. 기본은 Korean 949
#if defined( SRC_ID )
	#define COUNTRY_CODE_PAGE	1252
	#define IDR_ERROR IDR_TXT_ERROR_ID
#elif defined( SRC_SEA )
	#define COUNTRY_CODE_PAGE	1252
	#define IDR_ERROR IDR_TXT_ERROR_NA
#elif defined( SRC_NA )
	#define COUNTRY_CODE_PAGE	1252
	#define IDR_ERROR IDR_TXT_ERROR_NA
#elif defined( SRC_BR )
	#define COUNTRY_CODE_PAGE	1252
	#define IDR_ERROR IDR_TXT_ERROR_NA
#elif defined( SRC_PH )
	#define COUNTRY_CODE_PAGE	1252
	#define IDR_ERROR IDR_TXT_ERROR_NA
#elif defined( SRC_TW )
	#define COUNTRY_CODE_PAGE	950
	#define MULTI_BYTE_CHECK
	#define IDR_ERROR IDR_TXT_ERROR_TW
#elif defined( SRC_TH )
	#define COUNTRY_CODE_PAGE	874
	#define MULTI_BYTE_CHECK
	#define IDR_ERROR IDR_TXT_ERROR_TH
#elif defined( SRC_EU )
	#define COUNTRY_CODE_PAGE	1252
	#define IDR_ERROR IDR_TXT_ERROR_EU
#elif defined( SRC_SA )
	#define COUNTRY_CODE_PAGE	1250
	#define IDR_ERROR IDR_TXT_ERROR_SA
#else
	#define COUNTRY_CODE_PAGE	949
	#define IDR_ERROR IDR_TXT_ERROR
#endif

#define TEXT_RESOURCE "txt"


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
#endif // !defined(AFX_STDAFX_H__71C463A1_22B4_4766_AF89_624917AD702D__INCLUDED_)


