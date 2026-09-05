//////////////////////////////////////////////////////////////////////////
// 2006.06.19 LJH
// IOCP & THREAD & MEMPOOL & EVENT SELECT SOCKET 
//////////////////////////////////////////////////////////////////////////

#ifndef _IOCPSOCKET_H_
#define _IOCPSOCKET_H_

#ifdef EXPORT_IOCP_SOCKET
#define IOCP_SOCKET_API __declspec(dllexport)
#else
#define IOCP_SOCKET_API __declspec(dllimport)
#endif

#define WIN32_LEAN_AND_MEAN

#include <stdio.h>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>

#pragma comment(lib, "ws2_32.lib")


#include "../include/common.h"



#ifndef IOCP_SOCKET_API
#define IOCP_SOCKET_API __declspec(dllexport)
#endif

#pragma warning(disable:4251)

#include "UserTypeDefine.h"
#include "ThreadModules/SuperParent.h"
#include "ThreadModules/Thread.h"
#include "ThreadModules/ThreadManager.h"
#include "ThreadModules/ThreadSync.h"

#include "IOCP/CompletionHandler.h"
#include "IOCP/WorkerThread.h"
#include "IOCP/LogicThread.h"

#include "SocketModules/SendIO.h"
#include "SocketModules/ConnectNode.h"
#include "SocketModules/Packet.h"
#include "SocketModules/RecvQueue.h"
#include "SocketModules/MPSCRecvQueue.h"
#include "SocketModules/ServerSocket.h"
#include "SocketModules/AcceptorNode.h"
#include "SocketModules/SendBuffer.h"			// SendBuffer
#include "SocketModules/SendBufferManager.h"	// SendBufferManager
#include "SocketModules/UDPNode.h"
#include "SocketModules/ioUDPModule.h"
#include "SocketModules/BufferPool.h"
#include "SocketModules/ioUDPSecurity.h"
#include "../include/MPMCQueue.h"
#include "../include/MPMCMemPool.h"


#include "Processor/Processor.h"
#include "Util/NetworkSecurity.h"

#include "Encrypt/FSM.h"
#include "Encrypt/cryption.h"

extern CProcessor* g_processor;

IOCP_SOCKET_API bool BeginSocket();
IOCP_SOCKET_API void EndSocket();
IOCP_SOCKET_API void PrintTimeAndLog(int debuglv, LPSTR fmt, ... );
IOCP_SOCKET_API void DebugLog(int debuglv, LPSTR filename, int linenum, LPSTR fmt, ... );



#endif	// _IOCPSOCKET_H_