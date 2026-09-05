#pragma once

#pragma warning ( disable : 4995 )

#include "../iocpSocketDLL/iocpSocketDLL.h"
#include "../include/Log.h"
#include "Util/ioHashString.h"
#include "Protocol.h"
#include <stdio.h>
#include <windows.h>
#include <Direct.h>
#include <mmsystem.h>

#include "QueryData/QueryData.h"
#include "QueryData/QueryResultData.h"
#include "../Log/ioLogQueue.h"

extern CLog LOG;
extern CLog ReportLOG;
extern LONG WINAPI UnHandledExceptionFilter(struct _EXCEPTION_POINTERS *exceptionInfo);

#define COMPARE(x,min,max)		(((x)>=(min))&&((x)<(max)))
#define SAFEDELETE(x)			if(x != NULL) { delete x; x = NULL; }
#define SAFEDELETEARRAY(x)		if(x != NULL) { delete[] x; x = NULL; }
