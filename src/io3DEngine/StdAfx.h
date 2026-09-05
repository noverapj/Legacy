// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__511353F2_E8C1_4AD3_B012_8E2B3C54FFD6__INCLUDED_)
#define AFX_STDAFX_H__511353F2_E8C1_4AD3_B012_8E2B3C54FFD6__INCLUDED_

#define _WIN32_WINNT 0x500

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// Insert your headers here
//#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_NONSTDC_NO_DEPRECATE

#define __EX __declspec(dllexport)

#include "ioPrerequisites.h"

#include <d3d9.h>
#include <d3dx9.h>
#include <dsound.h>
#include <iterator>
 
#if DIRECTSOUND_VERSION < 0x0800
	#define LPDIRECTSOUND8 LPDIRECTSOUND
	#define IDirectSound8 IDirectSound
	#define DirectSoundCreate8 DirectSoundCreate
#endif

#include "ioVertexFormat.h"
#include "../LSLog/include/LSLog.h"

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "Dsound.lib")

#pragma comment(lib, "ogg.lib") 
#pragma comment(lib, "vorbis.lib") 
#pragma comment(lib, "vorbisfile.lib") 

#include "Squish/squish.h"
#include "DevIL/il.h"
#pragma comment(lib, "DevIL/IL.lib") 

#ifdef _DEBUG
	#pragma comment(lib, "Opcode/Opcoded.lib")
	#pragma comment(lib, "TinyXML/tinyxmld.lib")

	//#pragma comment(lib, "Bullet/libbulletmath_d.lib")
	//#pragma comment(lib, "Bullet/libbulletcollision_d.lib")
	//#pragma comment(lib, "Bullet/libbulletdynamics_d.lib")
	
	#pragma comment(lib, "Bullet/LinearMath_vs2010.lib")
	#pragma comment(lib, "Bullet/BulletCollision_vs2010.lib")
	#pragma comment(lib, "Bullet/BulletDynamics_vs2010.lib")
	#pragma comment(lib, "Squish/squishd.lib")
#else
	#pragma comment(lib, "Opcode/Opcode.lib")
	#pragma comment(lib, "TinyXML/tinyxml.lib")

	//UJ 131022, 최적화: bullet 2.81 rev.2613으로 업그레이드 + 내장 프로파일러 제거
	//#pragma comment(lib, "Bullet/libbulletmath.lib")
	//#pragma comment(lib, "Bullet/libbulletcollision.lib")
	//#pragma comment(lib, "Bullet/libbulletdynamics.lib")
	#pragma comment(lib, "Bullet/LinearMath_vs2010.lib")
	#pragma comment(lib, "Bullet/BulletCollision_vs2010.lib")
	#pragma comment(lib, "Bullet/BulletDynamics_vs2010.lib")
	#pragma comment(lib, "Squish/squish.lib")
#endif

#define COMPARE(x,min,max) (((x)>=(min))&&((x)<(max)))
#define SAFEDELETE(x)		if(x != NULL) { delete x; x = NULL; }
#define SAFEDELETEARRAY(x)	if(x != NULL) { delete [] x; x = NULL; }
#define SAFERELEASE(x)		if(x != NULL) { x->Release(); x = NULL; }
#define USE_LIST_ARQUMENTS_TEXT_FUNCTION // 해외버전은 가변인자 사용하면 안되므로 주석 처리해서 테스트

// 국가 코드 페이지. 기본은 Korean 949
#if defined( SRC_INDONESIA )
#define COUNTRY_CODE_PAGE	1252
#elif defined( SRC_NA )
#define COUNTRY_CODE_PAGE	1252
#elif defined( SRC_TAIWAN )
#define COUNTRY_CODE_PAGE	950
#elif defined( SRC_THAILAND )
#define COUNTRY_CODE_PAGE	874
#elif defined( SRC_EU )
#define COUNTRY_CODE_PAGE	1252
#elif defined( SRC_LATIN )
#define COUNTRY_CODE_PAGE	1250
#else
#define COUNTRY_CODE_PAGE	949
#endif

template <typename type>					// RADtoDEG
type  RADtoDEG (type val) {return (type) (val * 57.2957795132);}
template <typename type>					// DEGtoRAD
type  DEGtoRAD (type val) {return (type) (val * 0.0174532925199);}

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__511353F2_E8C1_4AD3_B012_8E2B3C54FFD6__INCLUDED_)

#ifdef SHIPPING
#define _ENCRYPT_STRING

__forceinline unsigned int return5()		{ __asm mov eax, 5		};
__forceinline unsigned int return10()		{ __asm mov eax, 10		};
__forceinline unsigned int return1000()		{ __asm mov eax, 1000	};

#define FLOAT5		((float)return5())
#define FLOAT10		((float)return10())
#define FLOAT1000	((float)return1000())
#define FLOAT05		((float)(FLOAT5/FLOAT10))
#else
#define FLOAT05		0.5f
#define FLOAT1000	1000.f
#endif