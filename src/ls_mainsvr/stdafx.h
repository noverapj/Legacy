// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once

#include "targetver.h"

#include <atltime.h>

#include "winsock2.h"
#include <stdio.h>
#include <tchar.h>
#include <MMSystem.h>
#include <assert.h>
#include <vector>
#include <map>
#include <string>
#include <set>
#include <list>
#include <deque>
#include <algorithm>
#include <functional>
#include <utility>
#include <limits>
#include <iostream>
#include "common.h"
#include "../include/Log.h"
#include "../iocpSocketDLL/iocpSocketDLL.h"
#include "../FrameTimerDLL/FrameTimerDLL.h"
#include "Util\ioHashString.h"
#include "Define.h"
#include "Protocol.h"
#include "ioPacketChecker.h"
#include "Util\ioStringConverter.h"
#include "../ioINILoader/ioINILoader.h"
#include "Util\ioEncrypted.h"
#include "Util\Singleton.h"
#include "Network\SP2Packet.h"
#include "Version.h"

using namespace std;

extern LONG WINAPI UnHandledExceptionFilter( struct _EXCEPTION_POINTERS* exceptionInfo );

extern void Trace( const char *format, ... );
extern void Debug( const char *format, ... );
extern void Information( const char *format, ... ) ;
extern void Debug();

extern CLog LOG;

// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.
