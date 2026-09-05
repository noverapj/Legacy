#ifndef __Safesprintf_h__
#define __Safesprintf_h__

// for %1, %2, %3
__EX bool IsRightArgs_format( const char *szSource, int iArgSize );
__EX void SafeSprintfFunc_format( const char * szSource, char * szDest, int iDestSize, int iArgSize, va_list *pArgs ) ;

// for %s, %d, %f
enum ArgType
{
	AT_INT    = 1,
	AT_STRING = 2,
	AT_FLOAT  = 3,
	AT_INT64  = 4,
	AT_DWORD  = 5,
};
typedef std::vector<ArgType> vArgType;
__EX bool IsRightArgs_printf( const char *szSource, const vArgType &rvArgTypeList );

// 인자에 따른 다양한 SafeSprintf 추가
#ifdef USE_LIST_ARQUMENTS_TEXT_FUNCTION
__EX void SafeSprintf( char *szDest, int iDestSize, const char *szSource, ... );
#else
__EX void SafeSprintf( char *szDest, int iDestSize, const char *szSource );
#endif
// 1
__EX void SafeSprintf( char *szDest, int iDestSize, const char *szSource, int iArg1 );
__EX void SafeSprintf( char *szDest, int iDestSize, const char *szSource, DWORD dwArg1 );
__EX void SafeSprintf( char *szDest, int iDestSize, const char *szSource, unsigned int uArg1 );
__EX void SafeSprintf( char *szDest, int iDestSize, const char *szSource, float fArg1 );
__EX void SafeSprintf( char *szDest, int iDestSize, const char *szSource, __int64 i64Arg1 );
__EX void SafeSprintf( char *szDest, int iDestSize, const char *szSource, const char *szArg1 );

// 2
__EX void SafeSprintf( char *szDest, int iDestSize, const char *szSource, int iArg1, int iArg2 );
__EX void SafeSprintf( char *szDest, int iDestSize, const char *szSource, int iArg1, float fArg2 );
__EX void SafeSprintf( char *szDest, int iDestSize, const char *szSource, float fArg1, int iArg2 );
__EX void SafeSprintf( char *szDest, int iDestSize, const char *szSource, float fArg1, float fArg2 );
__EX void SafeSprintf( char *szDest, int iDestSize, const char *szSource, int iArg1, const char *szArg2 );
__EX void SafeSprintf( char *szDest, int iDestSize, const char *szSource, const char *szArg1, int iArg2 );
__EX void SafeSprintf( char *szDest, int iDestSize, const char *szSource, const char *szArg1, DWORD dwArg2 );
__EX void SafeSprintf( char *szDest, int iDestSize, const char *szSource, const char *szArg1, __int64 i64Arg2 );
__EX void SafeSprintf( char *szDest, int iDestSize, const char *szSource, const char *szArg1, const char *szArg2 );

// 3
__EX void SafeSprintf( char *szDest, int iDestSize, const char *szSource, int iArg1, int iArg2, int iArg3 );
__EX void SafeSprintf( char *szDest, int iDestSize, const char *szSource, int iArg1, float fArg2, float fArg3 );
__EX void SafeSprintf( char *szDest, int iDestSize, const char *szSource, float fArg1, float fArg2, float fArg3 );
__EX void SafeSprintf( char *szDest, int iDestSize, const char *szSource, int iArg1, int iArg2, const char *szArg3 );
__EX void SafeSprintf( char *szDest, int iDestSize, const char *szSource, int iArg1, const char *szArg2, int iArg3 );
__EX void SafeSprintf( char *szDest, int iDestSize, const char *szSource, int iArg1, const char *szArg2, const char *szArg3 );
__EX void SafeSprintf( char *szDest, int iDestSize, const char *szSource, const char *szArg1, int iArg2, int iArg3 );
__EX void SafeSprintf( char *szDest, int iDestSize, const char *szSource, const char *szArg1, int iArg2, const char *szArg3 );
__EX void SafeSprintf( char *szDest, int iDestSize, const char *szSource, const char *szArg1, const char *szArg2, const char *szArg3 );
__EX void SafeSprintf( char *szDest, int iDestSize, const char *szSource, const char *szArg1, const char *szArg2, int iArg3 );

// 4
__EX void SafeSprintf( char *szDest, int iDestSize, const char *szSource, int iArg1, int iArg2, int iArg3, int iArg4 );
__EX void SafeSprintf( char *szDest, int iDestSize, const char *szSource, int iArg1, float fArg2, float fArg3, float fArg4 );
__EX void SafeSprintf( char *szDest, int iDestSize, const char *szSource, float fArg1, float fArg2, float fArg3, float fArg4 );
__EX void SafeSprintf( char *szDest, int iDestSize, const char *szSource, int iArg1, int iArg2, const char *szArg3, int iArg4 );
__EX void SafeSprintf( char *szDest, int iDestSize, const char *szSource, int iArg1, const char *szArg2, int iArg3, int iArg4 );
__EX void SafeSprintf( char *szDest, int iDestSize, const char *szSource, const char *szArg1, const char *szArg2, int iArg3, const char *szArg4 );
__EX void SafeSprintf( char *szDest, int iDestSize, const char *szSource, const char *szArg1, int iArg2, int iArg3, int iArg4 );
__EX void SafeSprintf( char *szDest, int iDestSize, const char *szSource, const char *szArg1, int iArg2, int iArg3, const char *szArg4 );
__EX void SafeSprintf( char *szDest, int iDestSize, const char *szSource, const char *szArg1, const char *szArg2, const char *szArg3, const char *szArg4 );
__EX void SafeSprintf( char *szDest, int iDestSize, const char *szSource, const char *szArg1, int iArg2, const char *szArg3, int iArg4 );

// 5
__EX void SafeSprintf( char *szDest, int iDestSize, const char *szSource, int iArg1, int iArg2, int iArg3, int iArg4, int iArg5 );
__EX void SafeSprintf( char *szDest, int iDestSize, const char *szSource, int iArg1, float fArg2, float fArg3, float fArg4, float fArg5 );
__EX void SafeSprintf( char *szDest, int iDestSize, const char *szSource, const char *szArg1, int iArg2, int iArg3, int iArg4, int iArg5 );
__EX void SafeSprintf( char *szDest, int iDestSize, const char *szSource, const char *szArg1, int iArg2, int iArg3, int iArg4, const char *szArg5 );
__EX void SafeSprintf( char *szDest, int iDestSize, const char *szSource, int iArg1, const char *szArg2, int iArg3, int iArg4, const char *szArg5 );
__EX void SafeSprintf( char *szDest, int iDestSize, const char *szSource, int iArg1, const char *szArg2, const char *szArg3, const char *szArg4, const char *szArg5 );
__EX void SafeSprintf( char *szDest, int iDestSize, const char *szSource, float fArg1, float fArg2, float fArg3, float fArg4, float fArg5 );
__EX void SafeSprintf( char *szDest, int iDestSize, const char *szSource, const char *szArg1, const char *szArg2, int iArg3, int iArg4, int iArg5 );
__EX void SafeSprintf( char *szDest, int iDestSize, const char *szSource, const char *szArg1, const char *szArg2, const char *szArg3, const char *szArg4, const char *szArg5 );

// 6
__EX void SafeSprintf( char *szDest, int iDestSize, const char *szSource, float fArg1, float fArg2, float fArg3, float fArg4, float fArg5, float fArg6 );
__EX void SafeSprintf( char *szDest, int iDestSize, const char *szSource, const char *szArg1, const char *szArg2, int iArg3, int iArg4, int iArg5, const char *szArg6 );
__EX void SafeSprintf( char *szDest, int iDestSize, const char *szSource, const int iArg1, int iArg2, const char *szArg3, int iArg4, int iArg5, const char *szArg6 );

// 7
__EX void SafeSprintf( char *szDest, int iDestSize, const char *szSource, const char *szArg1, int iArg2, int iArg3, int iArg4, int iArg5, int iArg6, const char *szArg7 );

// 8
__EX void SafeSprintf( char *szDest, int iDestSize, const char *szSource, const char *szArg1, int iArg2, int iArg3, int iArg4, int iArg5, int iArg6, int iArg7, const char *szArg8 );
__EX void SafeSprintf( char *szDest, int iDestSize, const char *szSource, const char *szArg1, const char *szArg2, const char *szArg3, int iArg4, int iArg5, int iArg6, int iArg7, const char *szArg8 );

#endif // __Safesprintf_h__