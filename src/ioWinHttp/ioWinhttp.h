#ifndef __ioWinhttp_h__
#define __ioWinhttp_h__

#ifdef IOWINHTTP_EXPORTS
	#define WINHTTP_API __declspec(dllexport)
#else
	#define WINHTTP_API __declspec(dllimport)
#endif

#include "include/CWinhttp.h"

#endif // __ioWinhttp_h__
