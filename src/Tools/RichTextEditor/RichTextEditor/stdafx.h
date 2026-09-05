// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once

#pragma warning( disable : 4995 )
#pragma warning( disable : 4996 )
#pragma warning( disable : 4819 )

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
// Windows 헤더 파일:
#include <windows.h>

// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

//io3DEngine 관련 흠..
//#ifdef _DEBUG
//#define __EX
//#else
#define __EX __declspec(dllimport)//export)
//#endif

// io3DEngine 관련
#include "mmsystem.h"
#include <d3d9.h>
#include <d3dx9.h>
#include <d3dx9math.h>
#include <dsound.h>
#include <tchar.h>
//#include <strsafe.h>
//#include <atlbase.h>	
//#include <atlcom.h>		
//#include <atlhost.h>
//#include <GdiPlus.h>
//#include <Wininet.h>

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "Dsound.lib")

#ifdef _DEBUG
#pragma comment(lib, "io3DEngined.lib")
#else
#pragma comment(lib, "io3DEngine.lib")
#endif

#define DEL(x) if(x){delete x;x=0;}
#define SAFEDELETE(x) if(x != NULL){delete x;x=NULL;}

#include "wx/msgdlg.h"

// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.
