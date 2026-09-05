// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
// Windows 헤더 파일:
#include <windows.h>

// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <atlstr.h>

#include <d3d9.h>
#include <d3dx9.h>
#include <d3dx9math.h>
#include "Version.h"


#pragma warning(disable:4786)
// for <strsafe.h>
#pragma warning(disable:4995)
// for string
#pragma warning(disable:4996)


#define __EX
#define SAFEDELETE(x) if(x != NULL) { delete x; x = NULL; }
#define SAFERELEASE(x) if(x != NULL) { x->Release(); x = NULL; }
#define SAFEDELETEARRAY(x) if(x != NULL) { delete [] x; x = NULL; }
#define COMPARE(x,min,max) (((x)>=(min))&&((x)<(max)))

// error dlg skin
#define ERROR_DLG_WIDTH       410
#define ERROR_DLG_HEIGHT      298
#define ERROR_EDIT_WIDTH      376
#define ERROR_EDIT_HEIGHT     191
#define ERROR_X_WIDTH         25
#define ERROR_X_HEIGHT        26
#define ERROR_SOLUTION_WIDTH  286
#define ERROR_SOLUTION_HEIGHT 31
#define ERROR_EXIT_WIDTH      88
#define ERROR_EXIT_HEIGHT     31
#define BTN_BITMAP_SIZE		  3

#define ERROR_EDIT_BK_RGB   RGB(235, 235, 235)
#define ERROR_EDIT_TEXT_RGB RGB(58,58,58)

#include "io3DEngine/ioSingleton.h"
#include "StringManager/ioHashString.h"
#include "Local/ioLocalParent.h"
#include "skin.h"

// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.
struct ioVtxNrm4
{
	D3DXVECTOR4 vPos;
	D3DXVECTOR4 vNrm;
};



//----------------------

enum AntiAliasingQuality
{
	AQ_NONE,
	AQ_LOW,
	AQ_MIDDLE,
	AQ_HIGH,
};

enum LightQuality
{
	LQ_LOW,
	LQ_MIDDLE,
	LQ_HIGH,
};

enum EffectQuality
{
	EQ_LOW,
	EQ_MIDDLE,
	EQ_HIGH,
};

enum MapObjectQuality
{
	MQ_LOW,
	MQ_MIDDLE,
	MQ_HIGH,
};

enum FilterType
{
	FILTER_NONE,
	FILTER_GRAY40,
	FILTER_GRAY80,
	FILTER_GRAY100,
	FILTER_SEPIA40,
	FILTER_SEPIA80,
	FILTER_SEPIA100,
	FILTER_MAX
};


enum TextureQuality
{
	TQ_DEFAULT,	// 매니저의 설정대로
	TQ_LOW,		// 16비트, 1/4 사이즈
	TQ_MIDDLE,	// 16비트
	TQ_HIGH,	// 원본그대로, 32비트
};

enum OverayType
{
	OVERAY_NONE,
	OVERAY_BLACK,
	OVERAY_WHITE,
	OVERAY_MAX
};

