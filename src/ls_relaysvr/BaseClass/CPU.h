#pragma once


class SmartLock
{
public:
	SmartLock( LPCRITICAL_SECTION sect ){  EnterCriticalSection( pLock=sect ); };
	~SmartLock(){ LeaveCriticalSection( pLock ); };
private:
	LPCRITICAL_SECTION      pLock;
};


typedef __int64             TKLong;
typedef __int64             TKLongID;
typedef __int64             TKRefCount;
typedef unsigned __int64    TKULong;

typedef DWORD               TKDATE;

// undef const
const int   UNDEF_INT     = 0x80000000;
const DWORD UNDEF_DWORD   = 0xFFFFFFFF;


typedef struct TKuuid
{
	__int64     m_lo;
	__int64     m_hi;
}
TKuuid;

#define /*BOOL*/ TKuuidEQ(u1,u2) (memcmp( u1, u2, sizeof(TKuuid) )==0)

struct TKTIME
{
	int     m_hour;
	int     m_minute;
	int     m_second;
	int     m_milliseconds;
};

// The TKTime structure holds an unsigned 64-bit date and time value.
// This value could represents the number of 100-nanosecond units since the beginning of January 1, 1601.
class TKTime
{
public:
	TKTime( void );
	TKTime( LPCTSTR szGetSystemTimeDummy );  // szGetSystemTimeDummy is not used it's just to Get the system time in the constructor
	TKTime( const TKTime& time );
	TKTime( const FILETIME& fileTime );
	TKTime( const SYSTEMTIME& sysTime );
	TKTime( DWORD days, DWORD hours, DWORD minutes, DWORD seconds, DWORD millisseconds );
	TKTime( WORD year, WORD month, WORD day, WORD hour, WORD minute, WORD second );
	TKTime( TKLong time );

	~TKTime( void ) {}

	TKTime& operator =( TKLong time );
	TKTime& operator +=( TKLong time );
	TKTime& operator -=( TKLong time );
	TKTime& operator /=( TKLong time );
	TKTime& operator *=( TKLong time );

	operator FILETIME( void );
	operator SYSTEMTIME( void );
	operator TKTIME( void );
	operator TKLong( void );

	TKTime& Set( DWORD days, DWORD hours, DWORD minutes, DWORD seconds, DWORD milliseconds );
	TKTime& Set( WORD year, WORD month, WORD day, WORD hour, WORD minute, WORD second );
	TKTime& Set( DWORD milliseconds );  // becarefull you set some milliseconds
	TKTime& SetSystemTime( void );

	void LocalToUTC( void );
	void UTCToLocal( void );

private:
	TKLong m_time;
};



// sound frame duration in 100-nanosecond units (25 frames per second)
const TKLong TKTimeDay         =  864000000000 ;
const TKLong TKTimeHour        =  36000000000;
const TKLong TKTimeMinute      =  600000000;
const TKLong TKTimeSecond      =  10000000;
const TKLong TKTimeDecisecond  =  1000000;
const TKLong TKTimeCentisecond =  100000;
const TKLong TKTimeMillisecond =  10000;
const TKLong TKTimeMicrosecond =  10;
const TKLong TKTimeFrame       =  10000000 / 25;
const TKLong TKTimeInfinite    =  0xFFFFFFFFFFFFFFFF;
const TKLong TKTimeNULL        =  0;
const TKLong TKTimeMax         =  0x7FFFFFFFFFFFFFFF;
const TKLong TKTimeMin         =  (-TKTimeMax);
const TKLong TKTimeUNDEF       =  0x8000000000000000;

// WARNING
// alignement PB
// http://blogs.msdn.com/oldnewthing/archive/2004/08/25/220195.aspx

inline TKTime::TKTime( void )
{
	m_time = 0;
}

// GetSystemTime is not used it's just to Get the sysytem time in the constructor
inline TKTime::TKTime( LPCTSTR /*GetSystemTime*/ )
{
	FILETIME ft;
	::GetSystemTimeAsFileTime( &ft );
	(*(LARGE_INTEGER*)&m_time).HighPart = ft.dwHighDateTime;
	(*(LARGE_INTEGER*)&m_time).LowPart  = ft.dwLowDateTime;
}

inline TKTime::TKTime( const TKTime &time )
{
	m_time = time.m_time;
}

inline TKTime::TKTime( const FILETIME &ft )
{
	(*(LARGE_INTEGER*)&m_time).HighPart = ft.dwHighDateTime;
	(*(LARGE_INTEGER*)&m_time).LowPart  = ft.dwLowDateTime;
}

inline TKTime::TKTime( const SYSTEMTIME &sysTime )
{
	FILETIME ft;
	if ( ! SystemTimeToFileTime( &sysTime, &ft ) )
	{
		m_time = 0;
	}
	else
	{
		(*(LARGE_INTEGER*)&m_time).HighPart = ft.dwHighDateTime;
		(*(LARGE_INTEGER*)&m_time).LowPart  = ft.dwLowDateTime;
	}
}

inline TKTime::TKTime( DWORD days, DWORD hours, DWORD minutes, DWORD seconds, DWORD milliseconds )
{
	Set( days, hours, minutes, seconds, milliseconds );
}

inline TKTime::TKTime( WORD year, WORD month, WORD day, WORD hour, WORD minute, WORD second )
{
	Set( year, month, day, hour, minute, second );
}

inline TKTime::TKTime( TKLong time )
{
	m_time = time;
}

inline TKTime& TKTime::operator =( TKLong time )
{
	m_time = time;
	return *this;
}

inline TKTime& TKTime::operator +=( TKLong time )
{
	m_time = m_time + time;
	return *this;
}

inline TKTime& TKTime::operator -=( TKLong time )
{
	m_time = m_time - time;
	return *this;
}

inline TKTime& TKTime::operator /=( TKLong time )
{
	m_time = m_time / time;
	return *this;
}

inline TKTime& TKTime::operator *=( TKLong time )
{
	m_time = m_time * time;
	return *this;
}

inline TKTime::operator FILETIME( void )
{
	TKLong time = (m_time < 0) ? -m_time : m_time;
	FILETIME ft;
	ft.dwHighDateTime = (*(LARGE_INTEGER*)&time).HighPart;
	ft.dwLowDateTime  = (*(LARGE_INTEGER*)&time).LowPart;
	return ft;
}

inline TKTime::operator TKLong( void )
{
	return m_time;
}

inline TKTime::operator SYSTEMTIME( void )
{
	TKLong time = (m_time < 0) ? -m_time : m_time;
	SYSTEMTIME sysTime;
	FILETIME ft;
	ft.dwHighDateTime = (*(LARGE_INTEGER*)&time).HighPart;
	ft.dwLowDateTime  = (*(LARGE_INTEGER*)&time).LowPart;
	if ( ! FileTimeToSystemTime( &ft, &sysTime))
		ZeroMemory (&sysTime, sizeof(sysTime));

	return sysTime;
}

inline TKTime::operator TKTIME( void )
{
	TKLong time = (m_time < 0) ? -m_time : m_time;

	time /= TKTimeMillisecond; // Forget everything after millisecond

	TKTIME tkTime;
	tkTime.m_hour           = int(time / 3600000);
	tkTime.m_minute         = int((time - (TKLong(tkTime.m_hour)*3600000)) / 60000);
	tkTime.m_second         = int((time - (TKLong(tkTime.m_hour)*3600000) - (TKLong(tkTime.m_minute)*60000)) / 1000);
	tkTime.m_milliseconds   = int(time - (TKLong(tkTime.m_hour)*3600000) - (TKLong(tkTime.m_minute)*60000) - (TKLong(tkTime.m_second)*1000));

	return tkTime;
}

inline TKTime& TKTime::Set( DWORD days, DWORD hours, DWORD minutes, DWORD seconds, DWORD milliseconds )
{
	m_time = UInt32x32To64( days, 24 *60 *60 ) *10000000 +
		UInt32x32To64( hours, 60 *60 ) *10000000 +
		UInt32x32To64( minutes, 60 ) *10000000 +
		UInt32x32To64( seconds, 10000000 ) +
		UInt32x32To64( milliseconds, 10000 );
	return *this;
}

inline TKTime& TKTime::Set( WORD year, WORD month, WORD day, WORD hour, WORD minute, WORD second )
{
	SYSTEMTIME sys;
	memset( &sys, 0, sizeof(sys) );
	sys.wYear   = year;
	sys.wMonth  = month;
	sys.wDay    = day;
	sys.wHour   = hour;
	sys.wMinute = minute;
	sys.wSecond = second;

	FILETIME ft;
	if ( ! SystemTimeToFileTime( &sys, &ft ) )
	{
		m_time = 0;
	}
	else
	{
		(*(LARGE_INTEGER*)&m_time).HighPart = ft.dwHighDateTime;
		(*(LARGE_INTEGER*)&m_time).LowPart  = ft.dwLowDateTime;
	}

	return *this;
}

inline TKTime& TKTime::Set( DWORD milliseconds )
{
	m_time = milliseconds * TKTimeMillisecond;
	return *this;
}

inline TKTime& TKTime::SetSystemTime( void )
{
	FILETIME ft;
	GetSystemTimeAsFileTime( &ft );
	(*(LARGE_INTEGER*)&m_time).HighPart = ft.dwHighDateTime;
	(*(LARGE_INTEGER*)&m_time).LowPart  = ft.dwLowDateTime;
	return *this;
}

inline
	void TKTime::LocalToUTC( void )
{
	FILETIME ft;
	ft.dwHighDateTime = (*(LARGE_INTEGER*)&m_time).HighPart;
	ft.dwLowDateTime  = (*(LARGE_INTEGER*)&m_time).LowPart;
	LocalFileTimeToFileTime( &ft, &ft );
	(*(LARGE_INTEGER*)&m_time).HighPart = ft.dwHighDateTime;
	(*(LARGE_INTEGER*)&m_time).LowPart  = ft.dwLowDateTime;
}

inline
	void TKTime::UTCToLocal( void )
{
	FILETIME ft;
	ft.dwHighDateTime = (*(LARGE_INTEGER*)&m_time).HighPart;
	ft.dwLowDateTime  = (*(LARGE_INTEGER*)&m_time).LowPart;
	FileTimeToLocalFileTime( &ft, &ft );
	(*(LARGE_INTEGER*)&m_time).HighPart = ft.dwHighDateTime;
	(*(LARGE_INTEGER*)&m_time).LowPart  = ft.dwLowDateTime;
}


// Little helper to work with GetTickCount(), since this is an 32bit
// free running counter which pass from 0xFFFFFFFF to 0, it would be
// a good idea to use TKDelay, which counts delays in milliseconds up
// to (about) 24 days (2147483647 msec).
class TKDelay
{
public:
	void    Mark(){ m_mark = ::GetTickCount(); };
	int     MSec(){ return( (::GetTickCount()-m_mark)&0x7FFFFFFF); };
private:
	DWORD   m_mark;
};

// helper for get CPU cycle for very very short delay
// use Pentium Cycle Counter
#define RDTSC __asm __emit 0fh __asm __emit 031h /* hack for VC++ 5.0 */
class TKCPUDelay
{
public:
	void Mark(){ m_mark = _GetTicks(); };
	int  Diff(){ return int(_GetTicks().QuadPart - m_mark.QuadPart); };
private:
	static __inline LARGE_INTEGER _GetTicks( void )
	{
		LARGE_INTEGER res;
		__asm {
			RDTSC
				mov res.HighPart, edx
				mov res.LowPart, eax
		}
		return res;
	}

	LARGE_INTEGER m_mark;
};




typedef BOOL ( __stdcall * pfnGetSystemTimes)( LPFILETIME lpIdleTime, LPFILETIME lpKernelTime, LPFILETIME lpUserTime );

class CPU
{
public:
	CPU( void );
	~CPU( void );

	// return :
	// % of cpu usage for this process 
	// % cpu systemUsage 
	// uptime
	int GetUsage( int* pSystemUsage, TKTime* pUpTime );
private:
	static TKDelay s_delay;

	static TKLong s_time;

	static TKLong s_idleTime;
	static TKLong s_kernelTime;
	static TKLong s_userTime;
	static int    s_lastCpu;
	static int    s_cpu[5];

	static TKLong s_kernelTimeProcess;
	static TKLong s_userTimeProcess;
	static int    s_lastCpuProcess;
	static int    s_cpuProcess[5];

	static int    s_count;
	static int    s_index;

	static TKLong s_lastUpTime;

	static HINSTANCE s_hKernel;
	static pfnGetSystemTimes s_pfnGetSystemTimes;

	CRITICAL_SECTION m_lock;
};