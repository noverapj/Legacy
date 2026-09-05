// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>



// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.
#include <WinSock2.h>

#include <Gdiplus.h>
using namespace Gdiplus;

#include <cassert>
#include <vector>
#include <map>
#include <algorithm>

#include <strsafe.h>
#pragma warning(disable:4995)	// for <strsafe.h>

#include "../include/common.h"

#include <iostream>
using namespace std;

#include "../iocpSocketDLL/iocpSocketDLL.h"
#include "../include/Log.h"
#include "../FrameTimerDLL/FrameTimerDll.h"
#include "../ioINILoader/ioINILoader.h"

#include "Util\ioHashString.h"
#include "Define.h"
#include "Network/SP2Packet.h"
#include "network\iocpHandler.h"

#include "network\Protocol.h"
#include "NodeInfo\AcceptorUserNode.h"



// ioClientBind
class ioClientBind : public ServerSocket
{
public:
	ioClientBind()
	{
		SetAcceptor( new AcceptorUserNode, ITPK_ACCEPT_SESSION );
	}
};


#define SAFEDELETE(x)		if(x != NULL) { delete x; x = NULL; }
#define LOG_DEBUG_LEVEL                 0

extern CLog LOG;
extern CLog NetLOG;
extern CLog CriticalLOG;

extern void Trace( const char *format, ... );
extern void Debug( const char *format, ... );
extern void Information( const char *format, ... ) ;
extern void Debug();
