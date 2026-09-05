
// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이 
// 들어 있는 포함 파일입니다.

#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 일부 CString 생성자는 명시적으로 선언됩니다.

// MFC의 공통 부분과 무시 가능한 경고 메시지에 대한 숨기기를 해제합니다.
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC 핵심 및 표준 구성 요소입니다.
#include <afxext.h>         // MFC 확장입니다.





#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // Internet Explorer 4 공용 컨트롤에 대한 MFC 지원입니다.
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // Windows 공용 컨트롤에 대한 MFC 지원입니다.
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // MFC의 리본 및 컨트롤 막대 지원


//추가한 내용입니다.
#define MAX_PATH  260
#define SAFEDELETE(x) if(x != NULL) { delete x; x = NULL; }
#define COMPARE(x,min,max) (((x)>=(min))&&((x)<(max)))
#define SAFEDELETE(x) if(x != NULL) { delete x; x = NULL; }

//글로벌 핸들
extern	HWND	g_hWnd;

//검사에 사용할 파일입니다.
#define CRC_EXE_FILE_NAME	"CrcReader.exe"
#define CRC_TXT_FILE_NAME	"Crc.txt"
#define MGAME_TXT_FILE_NAME "start_mgame.txt"
#define START_TXT_FILE_NAME "start.txt"
#define AUTO_UP_EXE			"autoupgrade.exe"
#define AUTO_UP_INI			"autoupgrade_info.ini"
#define AUTO_UP_MGAME		"autoupgrade_info_mgame.ini"
#define COUNTRY_INFO_INI	"Country_Info.ini"

//인터넷 체크용
#include <WinInet.h>
#pragma comment(lib, "WinInet.lib")

//HTML오류
#include <mshtml.h>
#include <Windows.h>

//ENUM
enum	DATE_ID		{ DI_AUTOEXE, DI_MGAMEINI, DI_AUTOINFO}; //date 목록
enum	OBJ_ID		{ LIST_BOX };
enum	WORK_STEP	{ WS_CHECKSERVER, WS_LOADINIINFO, WS_GETINIDATA, WS_GETURLDATA
					,WS_COPYFILEPATH, WS_RUNCRC, WS_CREATE_FILEnFOLDER, WS_PROCESS_COMPLETE
					, WS_OTHERSTART, WS_OTHERSVRINFO, WS_OTHERWORKSTART};


