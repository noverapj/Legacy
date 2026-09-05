// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>



// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include <Windows.h>
#include <assert.h>
#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <map>
using namespace std;

typedef std::vector< std::string > StringVector;

#include "Util\ioHashString.h"

namespace ioStringConverter
{
	StringVector Split( const std::string &param,
								const std::string &delims,
								int iMaxSplit = 0 );
	void toLowerCase( std::string &str );
}

#define COMPARE(x,min,max) (((x)>=(min))&&((x)<(max)))
#define SAFEDELETE(x)		if(x != NULL) { delete x; x = NULL; }
#define SAFEDELETEARRAY(x)	if(x != NULL) { delete [] x; x = NULL; }

typedef unsigned char       BYTE;

struct Vector3
{
	float x,y,z;

	Vector3(){}
	Vector3( float _x, float _y, float _z )
	{
		x = _x;
		y = _y;
		z = _z;
	}
};



#define use_lib

#ifdef use_lib

	// typedef
	typedef std::basic_string<TCHAR> tstring;

	#include "../../../src/include/common.h"
	#include "../../../src/ioINILoader/ioINILoader.h"

	#ifdef _DEBUG
		#pragma comment( lib, "../../../lib/INID.lib" )
	#else
		#pragma comment( lib, "../../../lib/INI.lib" )
	#endif
#else
	
	#include "INILoader\ioINILoader.h"

#endif
