

#ifndef _ioFreeType_h_
#define _ioFreeType_h_

#ifdef IOFREETYPE_EXPORTS
	#define FREETYPE_API __declspec(dllexport)
#else
	#define FREETYPE_API __declspec(dllimport)
#endif

#include "include/ioFTDefine.h"
#include "include/ioFTFace.h"
#include "include/ioFTManager.h"

#endif