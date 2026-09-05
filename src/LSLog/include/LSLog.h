

#ifndef _LSLog_h_
#define _LSLog_h_

#ifdef STATIC_LS_LOG
	#define LOG_API
#else
	#ifdef EXPORT_LS_LOG
		#define LOG_API __declspec(dllexport)
	#else
		#define LOG_API __declspec(dllimport)
	#endif
#endif

#include <stdio.h>
#include <stdlib.h>
#include <direct.h>
#include <assert.h>
#include <conio.h>

#include "CLog.h"

#endif
