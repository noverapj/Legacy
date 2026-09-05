
#ifndef __GSS_WIN_H__
#define __GSS_WIN_H__

#if defined(WIN32)  || defined(_WIN64) 

//*****************************************************************************
#ifndef _VISUAL_C
#define _VISUAL_C
#endif

#pragma warning(disable:4793)
#pragma warning(disable:4996)

// includes specifics for Win32 platform
#include <winsock2.h>
#include <mswsock.h>
#include <process.h>
#include <stdio.h>
#include <time.h>
#include <queue>
#include <list>
#include <string>
#include <map>
#include <hash_map>
#include <dbghelp.h>
#include <tchar.h>
#include <sql.h>
#include <sqlext.h>
#include <winsvc.h>

namespace nsGSS
{


	//*****************************************************************************
	typedef unsigned char byte;
	typedef int socklen_t;	// to standardise with BSD sockets for accept and recvfrom

	//*****************************************************************************
	#ifndef TRUE
	#define TRUE	1
	#endif
	#ifndef true
	#define true	TRUE
	#endif
	#ifndef FALSE
	#define FALSE	0
	#endif
	#ifndef false
	#define false	FALSE
	#endif
	#ifndef ERROR
	#define ERROR	-1
	#endif
	/*
	#ifndef error
	#define error	ERROR
	#endif
	*/
	#ifdef _DEBUG 
	#define UNUSED(x) 
	#else 
	#define UNUSED(x) x 
	#endif 
	#define UNUSED_ALWAYS(x) x 


	#ifndef _UNICODE
	#define tstring			std::string
	#define tstringstream	std::stringstream
	#else
	#define tstring			std::wstring
	#define tstringstream	std::wstringstream
	#endif


	//*****************************************************************************
	/**
		@brief about pthread in windows visual studio compiler;
		
		pthread_win32_process_attach_np() & pthread_win32_process_detach_np() are non-portable related\n
		functions that must be called to use this POSIX pthread implementation when running as a \n
		statically linked library. If using pthread as a dll, these are not required.\n
	*/

	#undef INIT_SOCKET
	#define INIT_SOCKET()																		\
	{																							\
		WSADATA wsaData;																		\
		if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)											\
		{																						\
			return -1;																			\
		}																						\
		return 0;																				\
	}

	#undef DEINIT_SOCKET
	#define DEINIT_SOCKET()																		\
	{																							\
		WSACleanup();																			\
	}

	#undef INVALID_SOCKET_CHECK
	#define INVALID_SOCKET_CHECK(hSocket) {	if( hSocket == INVALID_SOCKET ) return -1; }

	#ifdef FD_SETSIZE
	#undef FD_SETSIZE
	#endif
	#define FD_SETSIZE		2048			///< FD_SETSIZE는 윈도우에서 select() 함수를 사용할 때에만 필요하다.

	// Backup
	//#undef INIT_SOCKET
	//#define INIT_SOCKET()																		\
	//{																							\
	//	WSADATA wsaData;																		\
	//	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)											\
	//	{																						\
	//		char szTemp[100];																	\
	//			wsprintf((LPWSTR)szTemp, (LPWSTR)"WSAStartup Failed: %d\n", WSAGetLastError());			\
	//			::MessageBox(NULL, (LPCTSTR)szTemp, (LPCTSTR)"DNetLIB Socket", MB_ICONERROR | MB_OK);		\
	//		return -1;																			\
	//	}																						\
	//	/*	pthread_win32_process_attach_np();														\ */
	//	return 0;																				\
	//}
	//
	//#undef DEINIT_SOCKET
	//#define DEINIT_SOCKET()																		\
	//{																							\
	//	WSACleanup();																			\
	///	*	pthread_win32_process_detach_np();														\ */
	//}
	//
	//#undef INVALID_SOCKET_CHECK
	//#define INVALID_SOCKET_CHECK(hSocket) {	if( hSocket == INVALID_SOCKET ) return -1; }

	#ifndef writeChar
	#define writeChar(x, y, z)                                                     \
	{                                                                              \
		puts(y);                                                                   \
	}
	#endif

	//*****************************************************************************
	/**
		@brief about pthread in windows visual studio compiler;

		gettimeofday implementation from Visual C(win32)\n
	*/
	// epoch time으로 변환할 상수
	#if defined(_MSC_VER) || defined(_MSC_EXTENSIONS)
	#define DELTA_EPOCH_IN_MICROSECS  11644473600000000Ui64
	#else
	#define DELTA_EPOCH_IN_MICROSECS  11644473600000000ULL
	#endif

	// for timezone
	struct timezone
	{
		int  tz_minuteswest; /* minutes W of Greenwich */
		int  tz_dsttime;     /* type of dst correction */
	};

	// gettimeofday in windows
	inline int gettimeofday(struct timeval *tv, struct timezone *tz)
	{
		SYSTEMTIME  st;
		FILETIME    ft;
		unsigned __int64 tmpres = 0;
		static int tzflag;

		if (NULL != tv)
		{
			// system time을 구하기
			//GetSystemTimeAsFileTime(&ft);
			GetLocalTime(&st);
			SystemTimeToFileTime(&st, &ft);

			// unsigned 64 bit로 만들기
			tmpres |= ft.dwHighDateTime;
			tmpres <<= 32;
			tmpres |= ft.dwLowDateTime;

			// 100nano를 1micro로 변환하기
			tmpres /= 10;

			// epoch time으로 변환하기
			tmpres -= DELTA_EPOCH_IN_MICROSECS;    

			// sec와 micorsec으로 맞추기
			tv->tv_sec = (long)(tmpres / 1000000UL);
			tv->tv_usec = (long)((tmpres % 1000000UL)/1000);
		}

		// timezone 처리
		if (NULL != tz)
		{
			long	_Timezone;
			int		_Daylight;

			if (!tzflag)
			{
				_tzset();
				tzflag++;
			}
			_get_timezone(&_Timezone);
			_get_daylight(&_Daylight);

			tz->tz_minuteswest = _Timezone / 60;
			tz->tz_dsttime = _Daylight;
		}

		return 0;
	}
} // namespace nsGSS

#endif // WIN32

#endif //__GSS_WIN_H__



#ifndef _GSS_W_LOCK_H
#define _GSS_W_LOCK_H

#include <windows.h>

namespace nsGSS
{
#define MAX_NORMAL_LOCK 10

	//---------------------------------------------------------------------------
	// 쓰레드 세이프한 상황을 만들기위해서 WIN32의 가장 기본적인 방법인
	// CRITICAL_SECTION을 사용한 기본 클래스이다..
	//---------------------------------------------------------------------------
	class Lock
	{
	public:
		Lock(void)
		{
			InitializeCriticalSection(&m_CS);
		}
		~Lock(void)
		{
			DeleteCriticalSection(&m_CS);
		};

		void Leave(void)
		{
			LeaveCriticalSection(&m_CS);
		}
		void Enter(void)
		{
			EnterCriticalSection(&m_CS);
		}

	private:
		CRITICAL_SECTION m_CS;
	};

	//---------------------------------------------------------------------------
	// CRITICAL_SECTION에 오버헤드를 피하기 위해서 
	// Atomic 함수를 이용한 스핀락 구현
	//---------------------------------------------------------------------------
	class SpinLock
	{
	public:
		SpinLock()
		{
			InterlockedExchange( &m_bResInUse, FALSE );
		}
		~SpinLock()
		{
		}
		void Enter(void)
		{
			while( InterlockedExchange( &m_bResInUse, TRUE ) == TRUE )
			{
				// Sleep도 있고 쓰레드간 전환 방식도 있다..
				Sleep(0);
			}
		}
		void Leave(void)
		{
			InterlockedExchange( &m_bResInUse, FALSE );
		}
	private:
		volatile LONG           m_bResInUse;
	};

}

#endif // _GSS_W_LOCK_H
#ifndef __GSS_SHARED_H__
#define __GSS_SHARED_H__

//#include "GOS.h"

namespace nsGSS
{

/////////////////////////////////////////////////////////////////////////////
// Nothing
/////////////////////////////////////////////////////////////////////////////

};

#endif //__SHARED_H__
#ifndef __GSS_SHAREDDEF_H__
#define __GSS_SHAREDDEF_H__

namespace nsGSS
{
	//-------------------------------------------------------------------------------------
	// Global Definition
	//-------------------------------------------------------------------------------------
	#define INVALID_THREAD_INDEX (-1)

	//-------------------------------------------------------------------------------------
	// Max Size Def.
	//-------------------------------------------------------------------------------------
	#define MAX_BUFFER_CONTROL_DATA     10          // 버퍼 컨트롤 데이타 갯수
	#define MAX_BUFFER_CONTROL_SIZE     5           // 5바이트의 길이
	#define MAX_GSS_LOADSTRING				128			// 일반적인 스트링 길이...
	#define MALLOC_INTERVAL				100			// malloc으로 동적할당시 인터벌을 둔다.
	#define MIN_COMM_BUF                256         // 작은 버퍼가 필요할때 사용..
	#define MAX_COMM_BUF				8192		// 단일 소켓 버퍼의 최대 크기 정의.
	#define MAX_SINGLE_PACKET_SIZE		4096		// 단일 패킷의 최대 크기
	#define MAX_BUF_SIZE				16384		// Maximum Buffer Size.
	#define MAX_IOBUFFER_COUNT          100         // IOCP에서 사용할 IOBuffer 최대 갯수..
	#define MAX_NORMAL_LOCK             10          // 노말 - 락으로 만들어진 최대 갯수..


	//-------------------------------------------------------------------------------------
	// Library Socket Error Message List.
	//-------------------------------------------------------------------------------------
	#define DNETLIB_OK						0		// 소켓 이상 없음.
	#define DNETLIB_ERORR					-1		// 시스템 에러.
	#define DNETLIB_ERROR_UNDEFINED			-9		// 정의되지 않은 에러.
	#define DNETLIB_ERROR_DUPLICATED		1		// 이미 접속하고 있을 경우.
	#define DNETLIB_ERROR_PROTOCOLTYPE		11		// Protocol Type 지정 에러.
	#define DNETLIB_ERROR_SOCKETTYPE		12		// 지원하지 않는 Socket Type.
	#define DNETLIB_ERROR_PORTNUMBER		13		// Port 수치 범위 에러.
	#define DNETLIB_ERROR_IPADDRESS			14		// IP Address 지정 에러.
	#define DNETLIB_ERROR_SOCKETDEACTIVE	100		// Socket has not be activated.
	#define DNETLIB_ERROR_CONNECTERROR		201		// connect() returned error.
	#define DNETLIB_ERROR_BINDERROR			202		// bind() returned error.
	#define DNETLIB_ERROR_RECVERROR			251		// recv() returned error.
	#define DNETLIB_ERROR_SENDERROR			261		// send() reutrned error.
	#define DNETLIB_ERROR_BUFFERBROKEN		300		// Any problem in buffer. such as broken buffer or not be activated.
	#define DNETLIB_ERROR_BUFFERWRITE		3001	// 버퍼에 쓰기 에러.
	#define DNETLIB_ERROR_MEMALLOCFAIL		500		// Memory allocation fail.
	#define DNETLIB_ERROR_PARAMETERVALUE	1000	// Error in parameter value for function.


	//-------------------------------------------------------------------------------------
	// DNetLIB Type Def
	//-------------------------------------------------------------------------------------
	#define SOCKET_TYPE_NONE				0		// Socket type has not defined.
	#define SOCKET_TYPE_SOCKET				1		// Traditional Socket.
	#define SOCKET_TYPE_RAW					2		// Raw socket model. raw socket only can use PROTOCOL_TYPE_RAW.
	#define SOCKET_TYPE_MFCCSOCKET			3		// Windows MFC CSocket model.
	#define SOCKET_TYPE_MFCASYNC			4		// Windows MFC AsyncSocket model.
	#define SOCKET_TYPE_WSAASYNC			5		// Windows WSAAsyncSocket model.
	#define SOCKET_TYPE_WSAEVENT			6		// Windows Event Socket model.
	#define SOCKET_TYPE_OVERLAPPED			7		// Windows overlapped IO socket model.
	#define SOCKET_TYPE_IOCP				8		// Windows IOCP model.
	#define SOCKET_TYPE_RTS					9		// Linux RTS socket model.
	#define SOCKET_TYPE_EPOLL				10		// Linux epoll socket model.
	#define SOCKET_TYPE_KQUEUE				11		// FreeBSD kqueue model.
	#define SOCKET_TYPE_SELECT				12		// Select Model

	#define PROTOCOL_TYPE_NONE				0		// Protocol type has not defined.
	#define PROTOCOL_TYPE_TCP				1		// Network class is using protocol type TCP/IP.
	#define PROTOCOL_TYPE_UDP				2		// Network class is using protocol type UDP/IP.
	#define PROTOCOL_TYPE_RAW				3		// Network class is using protocol type RAW.

	//-------------------------------------------------------------------------------------
	// For IOCP - IO 타입 구분
	//-------------------------------------------------------------------------------------
	#define OVERLAPPED_IO_TYPE_READ			1
	#define OVERLAPPED_IO_TYPE_WRITE		2
	#define OVERLAPPED_IO_TYPE_ACCEPT		3
	#define OVERLAPPED_IO_TYPE_DB           4

	//-------------------------------------------------------------------------------------
	// ASyncSelect에서 사용하는 메세지....유저 및 
	//-------------------------------------------------------------------------------------
	#define WM_USER_ACCEPT			(WM_USER+1)
	#define WM_USER_MESSAGE			(WM_USER+2)

	#define	MAKE_USERMSG(def_nID)	(WM_USER_MESSAGE) + (def_nID)
	#define	NID_IN_USERMSG(def_Msg)	(def_Msg) - (WM_USER_MESSAGE)
}// namespace nsGSS

#endif //__GSS_SHAREDDEF_H__


#ifndef __GSS_SHAREDDS_H__
#define __GSS_SHAREDDS_H__

/////////////////////////////////////////////////////////////////////////////
// Global Data Structure
/////////////////////////////////////////////////////////////////////////////
using namespace std;

namespace nsGSS
{
	//*****************************************************************************
	// IOCP용 기본 자료구조..
	typedef struct _IOContext
	{
		WSAOVERLAPPED	overlapped;
		int				iKey;
		int				iIOType;
		WSABUF			wsaBuf;
		BYTE			buffer[MAX_SINGLE_PACKET_SIZE*2];
	}IOContext, *LPIOContext;

	//*****************************************************************************
	// IO Buffer 기본 내용.
	typedef struct _IOBuffer
	{
		byte			buffer[MAX_SINGLE_PACKET_SIZE*2];
		int				iBufferLen;

		int				iThreadId;
		int				iSocketContextKey;

		struct _IOBuffer *pNext;
		struct _IOBuffer *pPrev;

	}IOBuffer, *LPIOBuffer;

	typedef std::list<IOBuffer*>	IOBUFFER_LIST;
	typedef IOBUFFER_LIST::iterator	IOBUFFER_LIST_ITOR;


	//*****************************************************************************
	// 싱글턴 패턴에서 사용할 기본 템플릿 클래스
	// namespace DNetUtil {

	template <class Obj>
	class Singleton
	{
	public:
		static Obj*    getInstance()          // 스레드 안정성 문제 있음
		{
			if( m_pInstance == NULL )
			{
				m_pInstance = new Obj();
			}

			return m_pInstance;
		}

		static void releaseInstance()
		{
			if( m_pInstance )
			{
				delete m_pInstance;
				m_pInstance = NULL;
			}
		}

	private:
		static Obj*    m_pInstance;
	};

	//}

	//using namespace DNetUtil;

	//*****************************************************************************
	// IOCP를 위한 구조체 및 자료
	struct OVERLAPPED_EX {
		OVERLAPPED	Overlapped;		// 원래의 구조체.
		int			IOType;			// 이 구조체가 하는 일, 'OVERLAPPED_IO_TYPE_XXXX'
		VOID		*object;		// 부모 개체 주소.
	};

} //namespace nsGSS

#endif //__GSS_SHAREDDS_H__

#ifndef __GSS_SHAREDINLINE_H__
#define __GSS_SHAREDINLINE_H__


namespace nsGSS
{


	/////////////////////////////////////////////////////////////////////////////
	// Global Inline function
	/////////////////////////////////////////////////////////////////////////////
	inline int GetDefaultIOCPThreadNum()
	{
		SYSTEM_INFO si;
		GetSystemInfo(&si);

		return ((2 * si.dwNumberOfProcessors) + 2);
	}

	inline TCHAR *GetDateTimeString(TCHAR *buf)
	{
		SYSTEMTIME st;

		if( buf != NULL )
		{
			GetLocalTime(&st);
			_stprintf(buf, _T("%04d%02d%02d%02d%02d%02d"),
						st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
		}

		return buf;
	}



	inline void DebugStackTrace(int ndepth)	//STACK TRACE
	#ifdef _GCC
	{
		/*
		void  *array[3];
		int    size = backtrace( array, 3);
		char **symbols = backtrace_symbols( array, size);

		while ((--size) >= 0)
		fprintf( stderr, " %2.i: %s\n", size + 1, symbols[size]);
		*/
	}
	#else
	{

	}
	#endif

	// 스트링이 비어 있는지 검사한다.
	// 비어있음의 기준은 isspace()와 동일하다.
	inline bool IsBlankString(const TCHAR* szString)
	{
		static TCHAR szSourceString[MAX_GSS_LOADSTRING * 8];
		static TCHAR delim[6] = {0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x20};

		if (szString != NULL)
		{
			//strncpy(szSourceString, szString, MAX_LOADSTRING * 8);
			_tcsncpy(szSourceString, szString, MAX_GSS_LOADSTRING * 8);
			return (_tcstok(szSourceString, delim) == NULL);
		}
		return true;
	}


	inline void *MALLOC(size_t size)
	{
		void	*ret;
		for(ret = malloc(size); ret == NULL; ret = malloc(size))
		{
			Sleep(MALLOC_INTERVAL);
		}
		return ret;
	}

	inline int TIMER_MINUS( struct timeval s1, struct timeval s2 )
	{
		int iRet;

		/*
		if( s1.tv_usec < s2.tv_usec )
		{
			s1.tv_usec	= s1.tv_usec + 1000;
			s1.tv_sec	= s1.tv_sec - 1;
		}
		*/

		iRet = ( ( s1.tv_sec - s2.tv_sec ) * 1000 ) + ( s1.tv_usec - s2.tv_usec );

		return iRet;
	}


	inline void TIMER_PLUS( struct timeval *s1, struct timeval s2 )
	{
		s1->tv_sec	= s1->tv_sec  + s2.tv_sec;
		s1->tv_usec	= s1->tv_usec + s2.tv_usec;
	}

} //namespace nsGSS

#endif //__GSS_SHAREDINLINE_H__


#ifndef __GSS_SHARED_LOCK_H__
#define __GSS_SHARED_LOCK_H__


namespace nsGSS
{
	// AutoLock implementation
	// 1. Class Lock
	template <class Obj>
	class MultiAutoLock
	{
		friend class ClassAutoLock;
	public:
		class ClassAutoLock
		{
		public:
			ClassAutoLock(void)
			{
				Obj::m_cLock.Enter();
			}
			~ClassAutoLock(void)
			{
				Obj::m_cLock.Leave();
			}
		};
	private:
		static class Lock m_cLock;
	};

	template <class Obj>
	Lock MultiAutoLock<Obj>::m_cLock;

	// 2. Normal Lock
	template <class NObj>
	class NormalAutoLock
	{
		friend class ClassAutoLock;

	public:
		class ClassAutoLock
		{
		public:
			ClassAutoLock(NObj *pThis)
			{
				m_pThis = pThis;
				m_pThis->GetLock()->Enter();
			}
			~ClassAutoLock()
			{
				m_pThis->GetLock()->Leave();
			}

			NObj *m_pThis;
		};
	public:
		Lock *GetLock() { return &m_cLock; }
	private:
		class Lock m_cLock;
	};

	template<class NObj>
	class SpinAutoLock
	{
		friend class ClassAutoLock;

	public:
		class ClassAutoLock
		{
		public:
			ClassAutoLock(NObj* _pThis)
			{
				m_pThis = _pThis;
				m_pThis->GetSpinLock()->Enter();
			}
			~ClassAutoLock()
			{
				m_pThis->GetSpinLock()->Leave();
			}

			NObj* m_pThis;
		};
	public:
		SpinLock*	GetSpinLock() { return &m_cSpinLock;}

	private:
		class SpinLock	m_cSpinLock;
	};


	// 사용하고자 하는 곳에서 만들어서 사용?
	//class Lock NormalAutoLock::m_cLock[MAX_NORMAL_LOCK];

} //namespace nsGSS
#endif // __GSS_SHARED_LOCK_H__

#ifndef __SHAREDLOG_H__
#define __SHAREDLOG_H__


namespace nsGSS
{


/////////////////////////////////////////////////////////////////////////////
// 에러 및 로그 관련 함수..
/////////////////////////////////////////////////////////////////////////////

//*****************************************************************************
// 라이브러리 에러내용을 관리할 수 있음.
// Thread-Safe 할까?
class DNetError
{
public:
	static int			m_iDNetErrorNo;							// Library Super Class Global Error Number.
	static TCHAR	    m_szDNetErrorMsg[1024];					// Error message for DNetLIB.

	static __inline void SetLastError(int iErrorNo, const TCHAR *szErrorMsg = _T(""), ...)
	{
		va_list args;

		va_start(args, szErrorMsg);

		if( NULL == szErrorMsg ) 
			return;

		_vstprintf(m_szDNetErrorMsg, szErrorMsg, args);

		va_end(args);

		m_iDNetErrorNo = iErrorNo;
	}

    static __inline TCHAR *GetLastErrorMsg()
    {
        return m_szDNetErrorMsg;
    }

    static __inline int GetLastErrorNo()
    {
        return m_iDNetErrorNo;
    }
};

static DNetError g_sDNetError;

//*****************************************************************************
// 라이브러리 에러내용을 관리할 수 있음.
//class LOG
//{
	// Nothing
//};
}// namespace nsGSS

#endif //__SHAREDLOG_H__
