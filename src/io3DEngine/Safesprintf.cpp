#include "StdAfx.h"
#include "iolocalmanagerparent.h"
#include ".\safesprintf.h"
#include <strsafe.h>


enum 
{
	ASCII_CODE_NUM0       = 48,
	ASCII_CODE_NUM1       = 49,
	ASCII_CODE_NUM9_PLUS1 = 58,

	MAX_STRING            = 30,
	MAX_FORMAT_CHECK      = 10,
};

bool IsRightArgs_format( const char *szSource, int iArgSize )
{
	if( !COMPARE( iArgSize, 1, 100 ) ) // 인자 갯수는 1~99깨까지만
		return false;

	bool bFormat10Pos = false; // true : 10자리숫자 , false : 1자리숫자
	if( iArgSize > 9 )
		bFormat10Pos = true;

	int iAsciiCodeNumEnd10Pos = 0; // 10자리
	int iAsciiCodeNumEnd      = 0;
	if( !bFormat10Pos )
	{
		iAsciiCodeNumEnd = ASCII_CODE_NUM0 + iArgSize;
		iAsciiCodeNumEnd += 1; // +1 compare 범위 체크를 위해서
	}
	else
	{
		iAsciiCodeNumEnd      = ASCII_CODE_NUM0 + ( iArgSize%10);
		iAsciiCodeNumEnd10Pos = ASCII_CODE_NUM0 + ( iArgSize/10);

		iAsciiCodeNumEnd += 1;          // +1 compare 범위 체크를 위해서
		iAsciiCodeNumEnd10Pos += 1; // +1 compare 범위 체크를 위해서
	}

	int iFormatSize  = 0;
	int iSourceSize = strlen( szSource )-1; // -1:%N을 체크해야 하므로 N을 제외한다.
	for (int i = 0; i < iSourceSize ; i++)
	{
		if( szSource[i] != '%' )
			continue;
		if( szSource[i+1] == '%' )
		{
			i++;
			continue;
		}
		if( iFormatSize == iArgSize )
			return false;

		if( !bFormat10Pos )
		{
			if( COMPARE( szSource[i+1] , ASCII_CODE_NUM1, iAsciiCodeNumEnd ) )
				iFormatSize++;
			else
				return false;
		}
		else
		{
			if( COMPARE( szSource[i+2], ASCII_CODE_NUM0,  ASCII_CODE_NUM9_PLUS1 ) ) // %XY : Y가 숫자이면 %10~99 2자리 포맷
			{
				if(   COMPARE( szSource[i+1], ASCII_CODE_NUM1, iAsciiCodeNumEnd10Pos )     // 10자리
					&&COMPARE( szSource[i+2], ASCII_CODE_NUM0, iAsciiCodeNumEnd )       )  // 1자리
					iFormatSize++;
				else
					return false;
			}
			else // %1~9 1자리 포맷
			{
				if( COMPARE( szSource[i+1] , ASCII_CODE_NUM1, ASCII_CODE_NUM9_PLUS1 ) )
					iFormatSize++;
				else
					return false;
			}
		}

	}

	if( iFormatSize != iArgSize )
		return false;

	return true;
}

void SafeSprintfFunc_format( const char * szSource, char * szDest, int iDestSize, int iArgSize, va_list *pArgs )
{
	__try
	{
		if( !pArgs )
		{
			StringCbCopy( szDest, iDestSize, szSource );
			return;
		}

		if( !IsRightArgs_format( szSource, iArgSize ) )
		{
			StringCbCopy( szDest, iDestSize, szSource );
			return;
		}

		if ( !FormatMessage( FORMAT_MESSAGE_FROM_STRING | FORMAT_MESSAGE_ARGUMENT_ARRAY, szSource, 0,0, szDest, iDestSize, pArgs ) )
		{
			StringCbCopy( szDest, iDestSize, szSource );
			return;
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		StringCbCopy( szDest, iDestSize, szSource );
		return;
	}
}

bool IsRightArgs_printf( const char *szSource, const vArgType &rvArgTypeList )
{
	int iArgSize = rvArgTypeList.size();

	if( !COMPARE( iArgSize, 1, 100 ) ) // 인자 갯수는 1~99깨까지만
		return false;

	int iFormatSize  = 0;
	int iSourceSize = strlen( szSource )-1; // -1:%N을 체크해야 하므로 N을 제외한다.
	for (int i = 0; i < iSourceSize ; i++)
	{
		if( szSource[i] != '%' )
			continue;
		if( szSource[i+1] == '%' )
		{
			i++;
			continue;
		}

		if( iFormatSize == iArgSize )
			return false;

		if( rvArgTypeList[iFormatSize] == AT_INT )
		{
			for (int j = 1; j < MAX_FORMAT_CHECK ; j++)
			{
				if( i + j >= iSourceSize+1 )
					return false;

				if( szSource[i+j] == 'd' || szSource[i+j] == 'u' )
				{
					iFormatSize++;
					i+=j;
					break;
				}
				else if ( COMPARE( szSource[i+j], ASCII_CODE_NUM0, ASCII_CODE_NUM9_PLUS1 ) ) // %02d 허용
					continue;
				else
					return false;
			}
			continue;
		}
		else if( rvArgTypeList[iFormatSize] == AT_STRING )
		{
			if( szSource[i+1] == 's' )
				iFormatSize++;
			else
				return false;
		}
		else if( rvArgTypeList[iFormatSize] == AT_FLOAT )
		{
			for (int j = 1; j < MAX_FORMAT_CHECK ; j++)
			{
				if( i + j >= iSourceSize+1 )
					return false;

				if( szSource[i+j] == 'f' )
				{
					iFormatSize++;
					i+=j;
					break;
				}
				else if ( COMPARE( szSource[i+j], ASCII_CODE_NUM0, ASCII_CODE_NUM9_PLUS1 ) ) // %.1f 허용
					continue;
				else if( szSource[i+j] == '.' ) // %.1f 허용
					continue;
				else
					return false;
			}
			continue;
		}
		else if( rvArgTypeList[iFormatSize] == AT_INT64 )
		{
			enum { MAX_FORMAT_NAME = 4, }; // I64d
			if( i + MAX_FORMAT_NAME >= iSourceSize+1 )
				return false;

			if( strncmp( &szSource[i+1], "I64d", MAX_FORMAT_NAME ) == 0 )
			{
				iFormatSize++;
				i+=MAX_FORMAT_NAME;
			}
			else
				return false;
		}
		else if( rvArgTypeList[iFormatSize] == AT_DWORD )
		{
			for (int j = 1; j < MAX_FORMAT_CHECK ; j++)
			{
				if( i + j >= iSourceSize+1 )
					return false;

				if( szSource[i+j] == 'd' || szSource[i+j] == 'u' )
				{
					iFormatSize++;
					i+=j;
					break;
				}
				else if ( COMPARE( szSource[i+j], ASCII_CODE_NUM0, ASCII_CODE_NUM9_PLUS1 ) ) // %02u 허용
					continue;
				else
					return false;
			}
			continue;
		}
	}

	if( iFormatSize != iArgSize )
		return false;

	return true;
}

#ifdef USE_LIST_ARQUMENTS_TEXT_FUNCTION
void SafeSprintf( char *szDest, int iDestSize, const char *szSource, ... )
{
	if( ioLocalManagerParent::IsVPrintf() )
	{
		va_list args;
		va_start( args, szSource );
		StringCbVPrintf( szDest, iDestSize, szSource, args );
		va_end( args );
	}
	else
	{
		StringCbCopy( szDest, iDestSize, szSource );
	}
}
#else
void SafeSprintf( char *szDest, int iDestSize, const char *szSource )
{	
	StringCbCopy( szDest, iDestSize, szSource );
}
#endif
// 1
void SafeSprintf( char *szDest, int iDestSize, const char *szSource, int iArg1 )
{	
	if( ioLocalManagerParent::IsVPrintf() )
	{
		vArgType vArgTypeList;
		vArgTypeList.reserve(1);
		vArgTypeList.push_back( AT_INT );
		if( IsRightArgs_printf( szSource, vArgTypeList ) )
			StringCbPrintf( szDest, iDestSize, szSource, iArg1 );
		else
			StringCbCopy( szDest, iDestSize, szSource );
		vArgTypeList.clear();
	}
	else
	{
		// FormatMessage int의 포맷인자(%!d!)를 사용할 수 있으나 인터페이스 편의를 위해서 string 변환
		char szArg1[MAX_STRING]="";
		StringCbPrintf( szArg1, sizeof( szArg1 ), "%d", iArg1 );

		va_list pArgs[] = {(va_list) szArg1 };
		SafeSprintfFunc_format(szSource, szDest, iDestSize, 1, pArgs);
	}
}

void SafeSprintf( char *szDest, int iDestSize, const char *szSource, const char *szArg1 )
{
	if( ioLocalManagerParent::IsVPrintf() )
	{
		vArgType vArgTypeList;
		vArgTypeList.reserve(1);
		vArgTypeList.push_back( AT_STRING );
		if( IsRightArgs_printf( szSource, vArgTypeList ) )
			StringCbPrintf( szDest, iDestSize, szSource, szArg1 );
		else
			StringCbCopy( szDest, iDestSize, szSource );
		vArgTypeList.clear();
	}
	else
	{
		va_list pArgs[] = {(va_list) szArg1 };
		SafeSprintfFunc_format(szSource, szDest, iDestSize, 1, pArgs);
	}
}

void SafeSprintf( char *szDest, int iDestSize, const char *szSource, DWORD dwArg1 )
{
	if( ioLocalManagerParent::IsVPrintf() )
	{
		vArgType vArgTypeList;
		vArgTypeList.reserve(1);
		vArgTypeList.push_back( AT_DWORD );
		if( IsRightArgs_printf( szSource, vArgTypeList ) )
			StringCbPrintf( szDest, iDestSize, szSource, dwArg1 );
		else
			StringCbCopy( szDest, iDestSize, szSource );
		vArgTypeList.clear();
	}
	else
	{
		char szArg1[MAX_STRING]="";
		StringCbPrintf( szArg1, sizeof( szArg1 ), "%d", dwArg1 );

		va_list pArgs[] = { (va_list) szArg1 };
		SafeSprintfFunc_format(szSource, szDest, iDestSize, 1, pArgs);
	}
}

void SafeSprintf( char *szDest, int iDestSize, const char *szSource, unsigned int uArg1 )
{
	if( ioLocalManagerParent::IsVPrintf() )
	{
		vArgType vArgTypeList;
		vArgTypeList.reserve(1);
		vArgTypeList.push_back( AT_DWORD );
		if( IsRightArgs_printf( szSource, vArgTypeList ) )
			StringCbPrintf( szDest, iDestSize, szSource, uArg1 );
		else
			StringCbCopy( szDest, iDestSize, szSource );
		vArgTypeList.clear();
	}
	else
	{
		char szArg1[MAX_STRING]="";
		StringCbPrintf( szArg1, sizeof( szArg1 ), "%d", uArg1 );

		va_list pArgs[] = { (va_list) szArg1 };
		SafeSprintfFunc_format(szSource, szDest, iDestSize, 1, pArgs);
	}
}

void SafeSprintf( char *szDest, int iDestSize, const char *szSource, float fArg1 )
{
	if( ioLocalManagerParent::IsVPrintf() )
	{
		vArgType vArgTypeList;
		vArgTypeList.reserve(1);
		vArgTypeList.push_back( AT_FLOAT );
		if( IsRightArgs_printf( szSource, vArgTypeList ) )
			StringCbPrintf( szDest, iDestSize, szSource, fArg1 );
		else
			StringCbCopy( szDest, iDestSize, szSource );
		vArgTypeList.clear();
	}
	else
	{
		char szArg1[MAX_STRING]="";
		StringCbPrintf( szArg1, sizeof( szArg1 ), "%.2f", fArg1 ); // float은  소수점 2자리까지 표시

		va_list pArgs[] = { (va_list) szArg1 };
		SafeSprintfFunc_format(szSource, szDest, iDestSize, 1, pArgs);
	}
}

void SafeSprintf( char *szDest, int iDestSize, const char *szSource, __int64 i64Arg1 )
{
	if( ioLocalManagerParent::IsVPrintf() )
	{
		vArgType vArgTypeList;
		vArgTypeList.reserve(1);
		vArgTypeList.push_back( AT_INT64 );
		if( IsRightArgs_printf( szSource, vArgTypeList ) )
			StringCbPrintf( szDest, iDestSize, szSource, i64Arg1 );
		else
			StringCbCopy( szDest, iDestSize, szSource );
		vArgTypeList.clear();
	}
	else
	{
		// FormatMessage __int64의 포맷인자(%I64d)사용할 수 없어 string 변환
		char szArg1[MAX_STRING]="";
		StringCbPrintf( szArg1, sizeof( szArg1 ), "%I64d", i64Arg1 );

		va_list pArgs[] = {(va_list) szArg1 };
		SafeSprintfFunc_format(szSource, szDest, iDestSize, 1, pArgs);
	}
}

// 2
void SafeSprintf( char *szDest, int iDestSize, const char *szSource, int iArg1, int iArg2 )
{
	if( ioLocalManagerParent::IsVPrintf() )
	{
		vArgType vArgTypeList;
		vArgTypeList.reserve(2);
		vArgTypeList.push_back( AT_INT );
		vArgTypeList.push_back( AT_INT );
		if( IsRightArgs_printf( szSource, vArgTypeList ) )
			StringCbPrintf( szDest, iDestSize, szSource, iArg1, iArg2 );
		else
			StringCbCopy( szDest, iDestSize, szSource );
		vArgTypeList.clear();
	}
	else
	{
		char szArg1[MAX_STRING]="";
		StringCbPrintf( szArg1, sizeof( szArg1 ), "%d", iArg1 );
		char szArg2[MAX_STRING]="";
		StringCbPrintf( szArg2, sizeof( szArg2 ), "%d", iArg2 );

		va_list pArgs[] = {(va_list) szArg1, (va_list) szArg2 };
		SafeSprintfFunc_format(szSource, szDest, iDestSize, 2, pArgs);
	}
}

void SafeSprintf( char *szDest, int iDestSize, const char *szSource, int iArg1, float fArg2 )
{
	if( ioLocalManagerParent::IsVPrintf() )
	{
		vArgType vArgTypeList;
		vArgTypeList.reserve(2);
		vArgTypeList.push_back( AT_INT );
		vArgTypeList.push_back( AT_FLOAT );
		if( IsRightArgs_printf( szSource, vArgTypeList ) )
			StringCbPrintf( szDest, iDestSize, szSource, iArg1, fArg2 );
		else
			StringCbCopy( szDest, iDestSize, szSource );
		vArgTypeList.clear();
	}
	else
	{
		char szArg1[MAX_STRING]="";
		StringCbPrintf( szArg1, sizeof( szArg1 ), "%d", iArg1 );
		char szArg2[MAX_STRING]="";
		StringCbPrintf( szArg2, sizeof( szArg2 ), "%.2f", fArg2 );

		va_list pArgs[] = {(va_list) szArg1, (va_list) szArg2 };
		SafeSprintfFunc_format(szSource, szDest, iDestSize, 2, pArgs);
	}
}

void SafeSprintf( char *szDest, int iDestSize, const char *szSource, float fArg1, int iArg2 )
{
	if( ioLocalManagerParent::IsVPrintf() )
	{
		vArgType vArgTypeList;
		vArgTypeList.reserve(2);
		vArgTypeList.push_back( AT_FLOAT );
		vArgTypeList.push_back( AT_INT );
		if( IsRightArgs_printf( szSource, vArgTypeList ) )
			StringCbPrintf( szDest, iDestSize, szSource, fArg1, iArg2 );
		else
			StringCbCopy( szDest, iDestSize, szSource );
		vArgTypeList.clear();
	}
	else
	{
		char szArg1[MAX_STRING]="";
		StringCbPrintf( szArg1, sizeof( szArg1 ), "%.2f", fArg1 );
		char szArg2[MAX_STRING]="";
		StringCbPrintf( szArg2, sizeof( szArg2 ), "%d", iArg2 );

		va_list pArgs[] = {(va_list) szArg1, (va_list) szArg2 };
		SafeSprintfFunc_format(szSource, szDest, iDestSize, 2, pArgs);
	}
}

void SafeSprintf( char *szDest, int iDestSize, const char *szSource, float fArg1, float fArg2 )
{
	if( ioLocalManagerParent::IsVPrintf() )
	{
		vArgType vArgTypeList;
		vArgTypeList.reserve(2);
		vArgTypeList.push_back( AT_FLOAT );
		vArgTypeList.push_back( AT_FLOAT );
		if( IsRightArgs_printf( szSource, vArgTypeList ) )
			StringCbPrintf( szDest, iDestSize, szSource, fArg1, fArg2 );
		else
			StringCbCopy( szDest, iDestSize, szSource );
		vArgTypeList.clear();
	}
	else
	{
		char szArg1[MAX_STRING]="";
		StringCbPrintf( szArg1, sizeof( szArg1 ), "%.2f", fArg1 );
		char szArg2[MAX_STRING]="";
		StringCbPrintf( szArg2, sizeof( szArg2 ), "%.2f", fArg2 );

		va_list pArgs[] = {(va_list) szArg1, (va_list) szArg2 };
		SafeSprintfFunc_format(szSource, szDest, iDestSize, 2, pArgs);
	}
}

void SafeSprintf( char *szDest, int iDestSize, const char *szSource, const char *szArg1, int iArg2 )
{
	if( ioLocalManagerParent::IsVPrintf() )
	{
		vArgType vArgTypeList;
		vArgTypeList.reserve(2);
		vArgTypeList.push_back( AT_STRING );
		vArgTypeList.push_back( AT_INT );
		if( IsRightArgs_printf( szSource, vArgTypeList ) )
			StringCbPrintf( szDest, iDestSize, szSource, szArg1, iArg2 );
		else
			StringCbCopy( szDest, iDestSize, szSource );
		vArgTypeList.clear();
	}
	else
	{
		char szArg2[MAX_STRING]="";
		StringCbPrintf( szArg2, sizeof( szArg2 ), "%d", iArg2 );

		va_list pArgs[] = {(va_list) szArg1, (va_list) szArg2 };
		SafeSprintfFunc_format(szSource, szDest, iDestSize, 2, pArgs);
	}
}

void SafeSprintf( char *szDest, int iDestSize, const char *szSource, const char *szArg1, DWORD dwArg2 )
{
	if( ioLocalManagerParent::IsVPrintf() )
	{
		vArgType vArgTypeList;
		vArgTypeList.reserve(2);
		vArgTypeList.push_back( AT_STRING );
		vArgTypeList.push_back( AT_DWORD );
		if( IsRightArgs_printf( szSource, vArgTypeList ) )
			StringCbPrintf( szDest, iDestSize, szSource, szArg1, dwArg2 );
		else
			StringCbCopy( szDest, iDestSize, szSource );
		vArgTypeList.clear();
	}
	else
	{
		char szArg2[MAX_STRING]="";
		StringCbPrintf( szArg2, sizeof( szArg2 ), "%d", dwArg2 );

		va_list pArgs[] = {(va_list) szArg1, (va_list) szArg2 };
		SafeSprintfFunc_format(szSource, szDest, iDestSize, 2, pArgs);
	}
}

void SafeSprintf( char *szDest, int iDestSize, const char *szSource, const char *szArg1, __int64 i64Arg2 )
{
	if( ioLocalManagerParent::IsVPrintf() )
	{
		vArgType vArgTypeList;
		vArgTypeList.reserve(2);
		vArgTypeList.push_back( AT_STRING );
		vArgTypeList.push_back( AT_INT64 );
		if( IsRightArgs_printf( szSource, vArgTypeList ) )
			StringCbPrintf( szDest, iDestSize, szSource, szArg1, i64Arg2 );
		else
			StringCbCopy( szDest, iDestSize, szSource );
		vArgTypeList.clear();
	}
	else
	{
		char szArg2[MAX_STRING]="";
		StringCbPrintf( szArg2, sizeof( szArg2 ), "%I64d", i64Arg2 );

		va_list pArgs[] = {(va_list) szArg1, (va_list) szArg2 };
		SafeSprintfFunc_format(szSource, szDest, iDestSize, 2, pArgs);
	}
}

void SafeSprintf( char *szDest, int iDestSize, const char *szSource, int iArg1, const char *szArg2 )
{
	if( ioLocalManagerParent::IsVPrintf() )
	{
		vArgType vArgTypeList;
		vArgTypeList.reserve(2);
		vArgTypeList.push_back( AT_INT );
		vArgTypeList.push_back( AT_STRING );
		if( IsRightArgs_printf( szSource, vArgTypeList ) )
			StringCbPrintf( szDest, iDestSize, szSource, iArg1, szArg2 );
		else
			StringCbCopy( szDest, iDestSize, szSource );
		vArgTypeList.clear();
	}
	else
	{
		char szArg1[MAX_STRING]="";
		StringCbPrintf( szArg1, sizeof( szArg1 ), "%d", iArg1 );

		va_list pArgs[] = {(va_list) szArg1, (va_list) szArg2 };
		SafeSprintfFunc_format(szSource, szDest, iDestSize, 2, pArgs);
	}
}

void SafeSprintf( char *szDest, int iDestSize, const char *szSource, const char *szArg1, const char *szArg2 )
{
	if( ioLocalManagerParent::IsVPrintf() )
	{
		vArgType vArgTypeList;
		vArgTypeList.reserve(2);
		vArgTypeList.push_back( AT_STRING );
		vArgTypeList.push_back( AT_STRING );
		if( IsRightArgs_printf( szSource, vArgTypeList ) )
			StringCbPrintf( szDest, iDestSize, szSource, szArg1, szArg2 );
		else
			StringCbCopy( szDest, iDestSize, szSource );
		vArgTypeList.clear();
	}
	else
	{
		va_list pArgs[] = {(va_list) szArg1, (va_list) szArg2 };
		SafeSprintfFunc_format(szSource, szDest, iDestSize, 2, pArgs);
	}
}

// 3
void SafeSprintf( char *szDest, int iDestSize, const char *szSource, int iArg1, int iArg2, int iArg3 )
{
	if( ioLocalManagerParent::IsVPrintf() )
	{
		vArgType vArgTypeList;
		vArgTypeList.reserve(3);
		vArgTypeList.push_back( AT_INT );
		vArgTypeList.push_back( AT_INT );
		vArgTypeList.push_back( AT_INT );
		if( IsRightArgs_printf( szSource, vArgTypeList ) )
			StringCbPrintf( szDest, iDestSize, szSource, iArg1, iArg2, iArg3 );
		else
			StringCbCopy( szDest, iDestSize, szSource );
		vArgTypeList.clear();
	}
	else
	{
		char szArg1[MAX_STRING]="";
		StringCbPrintf( szArg1, sizeof( szArg1 ), "%d", iArg1 );
		char szArg2[MAX_STRING]="";
		StringCbPrintf( szArg2, sizeof( szArg2 ), "%d", iArg2 );
		char szArg3[MAX_STRING]="";
		StringCbPrintf( szArg3, sizeof( szArg3 ), "%d", iArg3 );

		va_list pArgs[] = {(va_list) szArg1, (va_list) szArg2, (va_list)szArg3 };
		SafeSprintfFunc_format(szSource, szDest, iDestSize, 3, pArgs);
	}
}

void SafeSprintf( char *szDest, int iDestSize, const char *szSource, int iArg1, float fArg2, float fArg3 )
{
	if( ioLocalManagerParent::IsVPrintf() )
	{
		vArgType vArgTypeList;
		vArgTypeList.reserve(3);
		vArgTypeList.push_back( AT_INT );
		vArgTypeList.push_back( AT_FLOAT );
		vArgTypeList.push_back( AT_FLOAT );
		if( IsRightArgs_printf( szSource, vArgTypeList ) )
			StringCbPrintf( szDest, iDestSize, szSource, iArg1, fArg2, fArg3 );
		else
			StringCbCopy( szDest, iDestSize, szSource );
		vArgTypeList.clear();
	}
	else
	{
		char szArg1[MAX_STRING]="";
		StringCbPrintf( szArg1, sizeof( szArg1 ), "%d", iArg1 );
		char szArg2[MAX_STRING]="";
		StringCbPrintf( szArg2, sizeof( szArg2 ), "%.2f", fArg2 );
		char szArg3[MAX_STRING]="";
		StringCbPrintf( szArg3, sizeof( szArg3 ), "%.2f", fArg3 );

		va_list pArgs[] = {(va_list) szArg1, (va_list) szArg2, (va_list)szArg3 };
		SafeSprintfFunc_format(szSource, szDest, iDestSize, 3, pArgs);
	}
}

void SafeSprintf( char *szDest, int iDestSize, const char *szSource, float fArg1, float fArg2, float fArg3 )
{
	if( ioLocalManagerParent::IsVPrintf() )
	{
		vArgType vArgTypeList;
		vArgTypeList.reserve(3);
		vArgTypeList.push_back( AT_FLOAT );
		vArgTypeList.push_back( AT_FLOAT );
		vArgTypeList.push_back( AT_FLOAT );
		if( IsRightArgs_printf( szSource, vArgTypeList ) )
			StringCbPrintf( szDest, iDestSize, szSource, fArg1, fArg2, fArg3 );
		else
			StringCbCopy( szDest, iDestSize, szSource );
		vArgTypeList.clear();
	}
	else
	{
		char szArg1[MAX_STRING]="";
		StringCbPrintf( szArg1, sizeof( szArg1 ), "%.2f", fArg1 );
		char szArg2[MAX_STRING]="";
		StringCbPrintf( szArg2, sizeof( szArg2 ), "%.2f", fArg2 );
		char szArg3[MAX_STRING]="";
		StringCbPrintf( szArg3, sizeof( szArg3 ), "%.2f", fArg3 );

		va_list pArgs[] = {(va_list) szArg1, (va_list) szArg2, (va_list)szArg3 };
		SafeSprintfFunc_format(szSource, szDest, iDestSize, 3, pArgs);
	}
}

void SafeSprintf( char *szDest, int iDestSize, const char *szSource, int iArg1, int iArg2, const char *szArg3 )
{
	if( ioLocalManagerParent::IsVPrintf() )
	{
		vArgType vArgTypeList;
		vArgTypeList.reserve(3);
		vArgTypeList.push_back( AT_INT );
		vArgTypeList.push_back( AT_INT );
		vArgTypeList.push_back( AT_STRING );
		if( IsRightArgs_printf( szSource, vArgTypeList ) )
			StringCbPrintf( szDest, iDestSize, szSource, iArg1, iArg2, szArg3 );
		else
			StringCbCopy( szDest, iDestSize, szSource );
		vArgTypeList.clear();
	}
	else
	{
		char szArg1[MAX_STRING]="";
		StringCbPrintf( szArg1, sizeof( szArg1 ), "%d", iArg1 );
		char szArg2[MAX_STRING]="";
		StringCbPrintf( szArg2, sizeof( szArg2 ), "%d", iArg2 );

		va_list pArgs[] = {(va_list) szArg1, (va_list) szArg2, (va_list)szArg3 };
		SafeSprintfFunc_format(szSource, szDest, iDestSize, 3, pArgs);
	}
}

void SafeSprintf( char *szDest, int iDestSize, const char *szSource, int iArg1, const char *szArg2, int iArg3 )
{
	if( ioLocalManagerParent::IsVPrintf() )
	{
		vArgType vArgTypeList;
		vArgTypeList.reserve(3);
		vArgTypeList.push_back( AT_INT );
		vArgTypeList.push_back( AT_STRING );
		vArgTypeList.push_back( AT_INT );
		if( IsRightArgs_printf( szSource, vArgTypeList ) )
			StringCbPrintf( szDest, iDestSize, szSource, iArg1, szArg2, iArg3 );
		else
			StringCbCopy( szDest, iDestSize, szSource );
		vArgTypeList.clear();
	}
	else
	{
		char szArg1[MAX_STRING]="";
		StringCbPrintf( szArg1, sizeof( szArg1 ), "%d", iArg1 );
		char szArg3[MAX_STRING]="";
		StringCbPrintf( szArg3, sizeof( szArg3 ), "%d", iArg3 );

		va_list pArgs[] = {(va_list) szArg1, (va_list) szArg2, (va_list) szArg3 };
		SafeSprintfFunc_format(szSource, szDest, iDestSize, 3, pArgs);
	}
}

void SafeSprintf( char *szDest, int iDestSize, const char *szSource, int iArg1, const char* szArg2, const char *szArg3 )
{
	if( ioLocalManagerParent::IsVPrintf() )
	{
		vArgType vArgTypeList;
		vArgTypeList.reserve(3);
		vArgTypeList.push_back( AT_INT );
		vArgTypeList.push_back( AT_STRING );
		vArgTypeList.push_back( AT_STRING );
		if( IsRightArgs_printf( szSource, vArgTypeList ) )
			StringCbPrintf( szDest, iDestSize, szSource, iArg1, szArg2, szArg3 );
		else
			StringCbCopy( szDest, iDestSize, szSource );
		vArgTypeList.clear();
	}
	else
	{
		char szArg1[MAX_STRING]="";
		StringCbPrintf( szArg1, sizeof( szArg1 ), "%d", iArg1 );

		va_list pArgs[] = {(va_list) szArg1, (va_list) szArg2, (va_list)szArg3 };
		SafeSprintfFunc_format(szSource, szDest, iDestSize, 3, pArgs);
	}
}

void SafeSprintf( char *szDest, int iDestSize, const char *szSource, const char *szArg1, int iArg2, int iArg3 )
{
	if( ioLocalManagerParent::IsVPrintf() )
	{
		vArgType vArgTypeList;
		vArgTypeList.reserve(3);
		vArgTypeList.push_back( AT_STRING );
		vArgTypeList.push_back( AT_INT );
		vArgTypeList.push_back( AT_INT );
		if( IsRightArgs_printf( szSource, vArgTypeList ) )
			StringCbPrintf( szDest, iDestSize, szSource, szArg1, iArg2, iArg3 );
		else
			StringCbCopy( szDest, iDestSize, szSource );
		vArgTypeList.clear();
	}
	else
	{
		char szArg2[MAX_STRING]="";
		StringCbPrintf( szArg2, sizeof( szArg2 ), "%d", iArg2 );
		char szArg3[MAX_STRING]="";
		StringCbPrintf( szArg3, sizeof( szArg3 ), "%d", iArg3 );

		va_list pArgs[] = {(va_list) szArg1, (va_list) szArg2, (va_list) szArg3 };
		SafeSprintfFunc_format(szSource, szDest, iDestSize, 3, pArgs);
	}
}

void SafeSprintf( char *szDest, int iDestSize, const char *szSource, const char *szArg1, int iArg2, const char *szArg3 )
{
	if( ioLocalManagerParent::IsVPrintf() )
	{
		vArgType vArgTypeList;
		vArgTypeList.reserve(3);
		vArgTypeList.push_back( AT_STRING );
		vArgTypeList.push_back( AT_INT );
		vArgTypeList.push_back( AT_STRING );
		if( IsRightArgs_printf( szSource, vArgTypeList ) )
			StringCbPrintf( szDest, iDestSize, szSource, szArg1, iArg2, szArg3 );
		else
			StringCbCopy( szDest, iDestSize, szSource );
		vArgTypeList.clear();
	}
	else
	{
		char szArg2[MAX_STRING]="";
		StringCbPrintf( szArg2, sizeof( szArg2 ), "%d", iArg2 );

		va_list pArgs[] = {(va_list) szArg1, (va_list) szArg2, (va_list)szArg3 };
		SafeSprintfFunc_format(szSource, szDest, iDestSize, 3, pArgs);
	}
}

void SafeSprintf( char *szDest, int iDestSize, const char *szSource, const char *szArg1, const char *szArg2, int iArg3 )
{
	if( ioLocalManagerParent::IsVPrintf() )
	{
		vArgType vArgTypeList;
		vArgTypeList.reserve(3);
		vArgTypeList.push_back( AT_STRING );
		vArgTypeList.push_back( AT_STRING );
		vArgTypeList.push_back( AT_INT );
		if( IsRightArgs_printf( szSource, vArgTypeList ) )
			StringCbPrintf( szDest, iDestSize, szSource, szArg1, szArg2, iArg3 );
		else
			StringCbCopy( szDest, iDestSize, szSource );
		vArgTypeList.clear();
	}
	else
	{
		char szArg3[MAX_STRING]="";
		StringCbPrintf( szArg3, sizeof( szArg3 ), "%d", iArg3 );

		va_list pArgs[] = {(va_list) szArg1, (va_list) szArg2, (va_list)szArg3 };
		SafeSprintfFunc_format(szSource, szDest, iDestSize, 3, pArgs);
	}
}

void SafeSprintf( char *szDest, int iDestSize, const char *szSource, const char *szArg1, const char *szArg2, const char *szArg3 )
{
	if( ioLocalManagerParent::IsVPrintf() )
	{
		vArgType vArgTypeList;
		vArgTypeList.reserve(3);
		vArgTypeList.push_back( AT_STRING );
		vArgTypeList.push_back( AT_STRING );
		vArgTypeList.push_back( AT_STRING );
		if( IsRightArgs_printf( szSource, vArgTypeList ) )
			StringCbPrintf( szDest, iDestSize, szSource, szArg1, szArg2, szArg3 );
		else
			StringCbCopy( szDest, iDestSize, szSource );
		vArgTypeList.clear();
	}
	else
	{
		va_list pArgs[] = {(va_list) szArg1, (va_list) szArg2, (va_list)szArg3 };
		SafeSprintfFunc_format(szSource, szDest, iDestSize, 3, pArgs);
	}
}

// 4
void SafeSprintf( char *szDest, int iDestSize, const char *szSource, int iArg1, int iArg2, int iArg3, int iArg4 )
{
	if( ioLocalManagerParent::IsVPrintf() )
	{
		vArgType vArgTypeList;
		vArgTypeList.reserve(4);
		vArgTypeList.push_back( AT_INT );
		vArgTypeList.push_back( AT_INT );
		vArgTypeList.push_back( AT_INT );
		vArgTypeList.push_back( AT_INT );
		if( IsRightArgs_printf( szSource, vArgTypeList ) )
			StringCbPrintf( szDest, iDestSize, szSource, iArg1, iArg2, iArg3, iArg4 );
		else
			StringCbCopy( szDest, iDestSize, szSource );
		vArgTypeList.clear();
	}
	else
	{
		char szArg1[MAX_STRING]="";
		StringCbPrintf( szArg1, sizeof( szArg1 ), "%d", iArg1 );
		char szArg2[MAX_STRING]="";
		StringCbPrintf( szArg2, sizeof( szArg2 ), "%d", iArg2 );
		char szArg3[MAX_STRING]="";
		StringCbPrintf( szArg3, sizeof( szArg3 ), "%d", iArg3 );
		char szArg4[MAX_STRING]="";
		StringCbPrintf( szArg4, sizeof( szArg4 ), "%d", iArg4 );

		va_list pArgs[] = {(va_list) szArg1, (va_list) szArg2, (va_list) szArg3, (va_list) szArg4 };
		SafeSprintfFunc_format(szSource, szDest, iDestSize, 4, pArgs);
	}
}

void SafeSprintf( char *szDest, int iDestSize, const char *szSource, int iArg1, float fArg2, float fArg3, float fArg4 )
{
	if( ioLocalManagerParent::IsVPrintf() )
	{
		vArgType vArgTypeList;
		vArgTypeList.reserve(4);
		vArgTypeList.push_back( AT_INT );
		vArgTypeList.push_back( AT_FLOAT );
		vArgTypeList.push_back( AT_FLOAT );
		vArgTypeList.push_back( AT_FLOAT );
		if( IsRightArgs_printf( szSource, vArgTypeList ) )
			StringCbPrintf( szDest, iDestSize, szSource, iArg1, fArg2, fArg3, fArg4 );
		else
			StringCbCopy( szDest, iDestSize, szSource );
		vArgTypeList.clear();
	}
	else
	{
		char szArg1[MAX_STRING]="";
		StringCbPrintf( szArg1, sizeof( szArg1 ), "%d", iArg1 );
		char szArg2[MAX_STRING]="";
		StringCbPrintf( szArg2, sizeof( szArg2 ), "%.2f", fArg2 );
		char szArg3[MAX_STRING]="";
		StringCbPrintf( szArg3, sizeof( szArg3 ), "%.2f", fArg3 );
		char szArg4[MAX_STRING]="";
		StringCbPrintf( szArg4, sizeof( szArg4 ), "%.2f", fArg4 );

		va_list pArgs[] = {(va_list) szArg1, (va_list) szArg2, (va_list) szArg3, (va_list) szArg4 };
		SafeSprintfFunc_format(szSource, szDest, iDestSize, 4, pArgs);
	}
}

void SafeSprintf( char *szDest, int iDestSize, const char *szSource, float fArg1, float fArg2, float fArg3, float fArg4 )
{
	if( ioLocalManagerParent::IsVPrintf() )
	{
		vArgType vArgTypeList;
		vArgTypeList.reserve(4);
		vArgTypeList.push_back( AT_FLOAT );
		vArgTypeList.push_back( AT_FLOAT );
		vArgTypeList.push_back( AT_FLOAT );
		vArgTypeList.push_back( AT_FLOAT );
		if( IsRightArgs_printf( szSource, vArgTypeList ) )
			StringCbPrintf( szDest, iDestSize, szSource, fArg1, fArg2, fArg3, fArg4 );
		else
			StringCbCopy( szDest, iDestSize, szSource );
		vArgTypeList.clear();
	}
	else
	{
		char szArg1[MAX_STRING]="";
		StringCbPrintf( szArg1, sizeof( szArg1 ), "%.2f", fArg1 );
		char szArg2[MAX_STRING]="";
		StringCbPrintf( szArg2, sizeof( szArg2 ), "%.2f", fArg2 );
		char szArg3[MAX_STRING]="";
		StringCbPrintf( szArg3, sizeof( szArg3 ), "%.2f", fArg3 );
		char szArg4[MAX_STRING]="";
		StringCbPrintf( szArg4, sizeof( szArg4 ), "%.2f", fArg4 );

		va_list pArgs[] = {(va_list) szArg1, (va_list) szArg2, (va_list) szArg3, (va_list) szArg4 };
		SafeSprintfFunc_format(szSource, szDest, iDestSize, 4, pArgs);
	}
}


void SafeSprintf( char *szDest, int iDestSize, const char *szSource, int iArg1, int iArg2, const char *szArg3, int iArg4 )
{
	if( ioLocalManagerParent::IsVPrintf() )
	{
		vArgType vArgTypeList;
		vArgTypeList.reserve(4);
		vArgTypeList.push_back( AT_INT );
		vArgTypeList.push_back( AT_INT );
		vArgTypeList.push_back( AT_STRING );
		vArgTypeList.push_back( AT_INT );
		if( IsRightArgs_printf( szSource, vArgTypeList ) )
			StringCbPrintf( szDest, iDestSize, szSource, iArg1, iArg2, szArg3, iArg4 );
		else
			StringCbCopy( szDest, iDestSize, szSource );
		vArgTypeList.clear();
	}
	else
	{
		char szArg1[MAX_STRING]="";
		StringCbPrintf( szArg1, sizeof( szArg1 ), "%d", iArg1 );
		char szArg2[MAX_STRING]="";
		StringCbPrintf( szArg2, sizeof( szArg2 ), "%d", iArg2 );
		char szArg4[MAX_STRING]="";
		StringCbPrintf( szArg4, sizeof( szArg4 ), "%d", iArg4 );

		va_list pArgs[] = {(va_list) szArg1, (va_list) szArg2, (va_list) szArg3, (va_list) szArg4 };
		SafeSprintfFunc_format(szSource, szDest, iDestSize, 4, pArgs);
	}
}

void SafeSprintf( char *szDest, int iDestSize, const char *szSource, int iArg1, const char *szArg2, int iArg3, int iArg4 )
{
	if( ioLocalManagerParent::IsVPrintf() )
	{
		vArgType vArgTypeList;
		vArgTypeList.reserve(4);
		vArgTypeList.push_back( AT_INT );
		vArgTypeList.push_back( AT_STRING );
		vArgTypeList.push_back( AT_INT );
		vArgTypeList.push_back( AT_INT );
		if( IsRightArgs_printf( szSource, vArgTypeList ) )
			StringCbPrintf( szDest, iDestSize, szSource, iArg1, szArg2, iArg3, iArg4 );
		else
			StringCbCopy( szDest, iDestSize, szSource );
		vArgTypeList.clear();
	}
	else
	{
		char szArg1[MAX_STRING]="";
		StringCbPrintf( szArg1, sizeof( szArg1 ), "%d", iArg1 );
		char szArg3[MAX_STRING]="";
		StringCbPrintf( szArg3, sizeof( szArg3 ), "%d", iArg3 );
		char szArg4[MAX_STRING]="";
		StringCbPrintf( szArg4, sizeof( szArg4 ), "%d", iArg4 );

		va_list pArgs[] = {(va_list) szArg1, (va_list) szArg2, (va_list) szArg3, (va_list) szArg4 };
		SafeSprintfFunc_format(szSource, szDest, iDestSize, 4, pArgs);
	}
}

void SafeSprintf( char *szDest, int iDestSize, const char *szSource, const char *szArg1, const char *szArg2, int iArg3, const char *szArg4 )
{
	if( ioLocalManagerParent::IsVPrintf() )
	{
		vArgType vArgTypeList;
		vArgTypeList.reserve(4);
		vArgTypeList.push_back( AT_STRING );
		vArgTypeList.push_back( AT_STRING );
		vArgTypeList.push_back( AT_INT );
		vArgTypeList.push_back( AT_STRING );
		if( IsRightArgs_printf( szSource, vArgTypeList ) )
			StringCbPrintf( szDest, iDestSize, szSource, szArg1, szArg2, iArg3, szArg4 );
		else
			StringCbCopy( szDest, iDestSize, szSource );
		vArgTypeList.clear();
	}
	else
	{
		char szArg3[MAX_STRING]="";
		StringCbPrintf( szArg3, sizeof( szArg3 ), "%d", iArg3 );

		va_list pArgs[] = {(va_list) szArg1, (va_list) szArg2, (va_list) szArg3, (va_list) szArg4 };
		SafeSprintfFunc_format(szSource, szDest, iDestSize, 4, pArgs);
	}
}

void SafeSprintf( char *szDest, int iDestSize, const char *szSource, const char *szArg1, int iArg2, int iArg3, int iArg4 )
{
	if( ioLocalManagerParent::IsVPrintf() )
	{
		vArgType vArgTypeList;
		vArgTypeList.reserve(4);
		vArgTypeList.push_back( AT_STRING );
		vArgTypeList.push_back( AT_INT );
		vArgTypeList.push_back( AT_INT );
		vArgTypeList.push_back( AT_INT );
		if( IsRightArgs_printf( szSource, vArgTypeList ) )
			StringCbPrintf( szDest, iDestSize, szSource, szArg1, iArg2, iArg3, iArg4 );
		else
			StringCbCopy( szDest, iDestSize, szSource );
		vArgTypeList.clear();
	}
	else
	{
		char szArg2[MAX_STRING]="";
		StringCbPrintf( szArg2, sizeof( szArg2 ), "%d", iArg2 );
		char szArg3[MAX_STRING]="";
		StringCbPrintf( szArg3, sizeof( szArg3 ), "%d", iArg3 );
		char szArg4[MAX_STRING]="";
		StringCbPrintf( szArg4, sizeof( szArg4 ), "%d", iArg4 );

		va_list pArgs[] = {(va_list) szArg1, (va_list) szArg2, (va_list) szArg3, (va_list) szArg4 };
		SafeSprintfFunc_format(szSource, szDest, iDestSize, 4, pArgs);
	}
}

void SafeSprintf( char *szDest, int iDestSize, const char *szSource, const char *szArg1, int iArg2, int iArg3, const char *szArg4 )
{
	if( ioLocalManagerParent::IsVPrintf() )
	{
		vArgType vArgTypeList;
		vArgTypeList.reserve(4);
		vArgTypeList.push_back( AT_STRING );
		vArgTypeList.push_back( AT_INT );
		vArgTypeList.push_back( AT_INT );
		vArgTypeList.push_back( AT_STRING );
		if( IsRightArgs_printf( szSource, vArgTypeList ) )
			StringCbPrintf( szDest, iDestSize, szSource, szArg1, iArg2, iArg3, szArg4 );
		else
			StringCbCopy( szDest, iDestSize, szSource );
		vArgTypeList.clear();
	}
	else
	{
		char szArg2[MAX_STRING]="";
		StringCbPrintf( szArg2, sizeof( szArg2 ), "%d", iArg2 );
		char szArg3[MAX_STRING]="";
		StringCbPrintf( szArg3, sizeof( szArg3 ), "%d", iArg3 );

		va_list pArgs[] = {(va_list) szArg1, (va_list) szArg2, (va_list) szArg3, (va_list) szArg4 };
		SafeSprintfFunc_format(szSource, szDest, iDestSize, 4, pArgs);
	}
}

void SafeSprintf( char *szDest, int iDestSize, const char *szSource, const char *szArg1, const char *szArg2, const char *szArg3, const char *szArg4 )
{
	if( ioLocalManagerParent::IsVPrintf() )
	{
		vArgType vArgTypeList;
		vArgTypeList.reserve(4);
		vArgTypeList.push_back( AT_STRING );
		vArgTypeList.push_back( AT_STRING );
		vArgTypeList.push_back( AT_STRING );
		vArgTypeList.push_back( AT_STRING );
		if( IsRightArgs_printf( szSource, vArgTypeList ) )
			StringCbPrintf( szDest, iDestSize, szSource, szArg1, szArg2, szArg3, szArg4 );
		else
			StringCbCopy( szDest, iDestSize, szSource );
		vArgTypeList.clear();
	}
	else
	{
		va_list pArgs[] = {(va_list) szArg1, (va_list) szArg2, (va_list)szArg3, (va_list)szArg4 };
		SafeSprintfFunc_format(szSource, szDest, iDestSize, 4, pArgs);
	}
}

void SafeSprintf( char *szDest, int iDestSize, const char *szSource, const char *szArg1, int iArg2, const char *szArg3, int iArg4 )
{
	if( ioLocalManagerParent::IsVPrintf() )
	{
		vArgType vArgTypeList;
		vArgTypeList.reserve(4);
		vArgTypeList.push_back( AT_STRING );
		vArgTypeList.push_back( AT_INT );
		vArgTypeList.push_back( AT_STRING );
		vArgTypeList.push_back( AT_INT );
		if( IsRightArgs_printf( szSource, vArgTypeList ) )
			StringCbPrintf( szDest, iDestSize, szSource, szArg1, iArg2, szArg3, iArg4 );
		else
			StringCbCopy( szDest, iDestSize, szSource );
		vArgTypeList.clear();
	}
	else
	{
		char szArg2[MAX_STRING]="";
		StringCbPrintf( szArg2, sizeof( szArg2 ), "%d", iArg2 );
		char szArg4[MAX_STRING]="";
		StringCbPrintf( szArg4, sizeof( szArg4 ), "%d", iArg4 );

		va_list pArgs[] = {(va_list) szArg1, (va_list) szArg2, (va_list) szArg3, (va_list) szArg4 };
		SafeSprintfFunc_format(szSource, szDest, iDestSize, 4, pArgs);
	}
}

// 5
void SafeSprintf( char *szDest, int iDestSize, const char *szSource, int iArg1, int iArg2, int iArg3, int iArg4, int iArg5 )
{
	if( ioLocalManagerParent::IsVPrintf() )
	{
		vArgType vArgTypeList;
		vArgTypeList.reserve(5);
		vArgTypeList.push_back( AT_INT );
		vArgTypeList.push_back( AT_INT );
		vArgTypeList.push_back( AT_INT );
		vArgTypeList.push_back( AT_INT );
		vArgTypeList.push_back( AT_INT );
		if( IsRightArgs_printf( szSource, vArgTypeList ) )
			StringCbPrintf( szDest, iDestSize, szSource, iArg1, iArg2, iArg3, iArg4, iArg5 );
		else
			StringCbCopy( szDest, iDestSize, szSource );
		vArgTypeList.clear();
	}
	else
	{
		char szArg1[MAX_STRING]="";
		StringCbPrintf( szArg1, sizeof( szArg1 ), "%d", iArg1 );
		char szArg2[MAX_STRING]="";
		StringCbPrintf( szArg2, sizeof( szArg2 ), "%d", iArg2 );
		char szArg3[MAX_STRING]="";
		StringCbPrintf( szArg3, sizeof( szArg3 ), "%d", iArg3 );
		char szArg4[MAX_STRING]="";
		StringCbPrintf( szArg4, sizeof( szArg4 ), "%d", iArg4 );
		char szArg5[MAX_STRING]="";
		StringCbPrintf( szArg5, sizeof( szArg5 ), "%d", iArg5 );

		va_list pArgs[] = {(va_list) szArg1, (va_list) szArg2, (va_list) szArg3, (va_list) szArg4, (va_list) szArg5 };
		SafeSprintfFunc_format(szSource, szDest, iDestSize, 5, pArgs);
	}
}

void SafeSprintf( char *szDest, int iDestSize, const char *szSource, int iArg1, float fArg2, float fArg3, float fArg4, float fArg5 )
{
	if( ioLocalManagerParent::IsVPrintf() )
	{
		vArgType vArgTypeList;
		vArgTypeList.reserve(5);
		vArgTypeList.push_back( AT_INT );
		vArgTypeList.push_back( AT_FLOAT );
		vArgTypeList.push_back( AT_FLOAT );
		vArgTypeList.push_back( AT_FLOAT );
		vArgTypeList.push_back( AT_FLOAT );
		if( IsRightArgs_printf( szSource, vArgTypeList ) )
			StringCbPrintf( szDest, iDestSize, szSource, iArg1, fArg2, fArg3, fArg4, fArg5 );
		else
			StringCbCopy( szDest, iDestSize, szSource );
		vArgTypeList.clear();
	}
	else
	{
		char szArg1[MAX_STRING]="";
		StringCbPrintf( szArg1, sizeof( szArg1 ), "%d", iArg1 );
		char szArg2[MAX_STRING]="";
		StringCbPrintf( szArg2, sizeof( szArg2 ), "%.2f", fArg2 );
		char szArg3[MAX_STRING]="";
		StringCbPrintf( szArg3, sizeof( szArg3 ), "%.2f", fArg3 );
		char szArg4[MAX_STRING]="";
		StringCbPrintf( szArg4, sizeof( szArg4 ), "%.2f", fArg4 );
		char szArg5[MAX_STRING]="";
		StringCbPrintf( szArg5, sizeof( szArg5 ), "%.2f", fArg5 );

		va_list pArgs[] = {(va_list) szArg1, (va_list) szArg2, (va_list) szArg3, (va_list) szArg4, (va_list) szArg5 };
		SafeSprintfFunc_format(szSource, szDest, iDestSize, 5, pArgs);
	}
}

void SafeSprintf( char *szDest, int iDestSize, const char *szSource, const char *szArg1, int iArg2, int iArg3, int iArg4, int iArg5 )
{
	if( ioLocalManagerParent::IsVPrintf() )
	{
		vArgType vArgTypeList;
		vArgTypeList.reserve(5);
		vArgTypeList.push_back( AT_STRING );
		vArgTypeList.push_back( AT_INT );
		vArgTypeList.push_back( AT_INT );
		vArgTypeList.push_back( AT_INT );
		vArgTypeList.push_back( AT_INT );
		if( IsRightArgs_printf( szSource, vArgTypeList ) )
			StringCbPrintf( szDest, iDestSize, szSource, szArg1, iArg2, iArg3, iArg4, iArg5 );
		else
			StringCbCopy( szDest, iDestSize, szSource );
		vArgTypeList.clear();
	}
	else
	{
		char szArg2[MAX_STRING]="";
		StringCbPrintf( szArg2, sizeof( szArg2 ), "%d", iArg2 );
		char szArg3[MAX_STRING]="";
		StringCbPrintf( szArg3, sizeof( szArg3 ), "%d", iArg3 );
		char szArg4[MAX_STRING]="";
		StringCbPrintf( szArg4, sizeof( szArg4 ), "%d", iArg4 );
		char szArg5[MAX_STRING]="";
		StringCbPrintf( szArg5, sizeof( szArg5 ), "%d", iArg5 );

		va_list pArgs[] = {(va_list) szArg1, (va_list) szArg2, (va_list) szArg3, (va_list) szArg4, (va_list) szArg5 };
		SafeSprintfFunc_format(szSource, szDest, iDestSize, 5, pArgs);
	}
}

void SafeSprintf( char *szDest, int iDestSize, const char *szSource, const char *szArg1, int iArg2, int iArg3, int iArg4, const char *szArg5 )
{
	if( ioLocalManagerParent::IsVPrintf() )
	{
		vArgType vArgTypeList;
		vArgTypeList.reserve(5);
		vArgTypeList.push_back( AT_STRING );
		vArgTypeList.push_back( AT_INT );
		vArgTypeList.push_back( AT_INT );
		vArgTypeList.push_back( AT_INT );
		vArgTypeList.push_back( AT_STRING );
		if( IsRightArgs_printf( szSource, vArgTypeList ) )
			StringCbPrintf( szDest, iDestSize, szSource, szArg1, iArg2, iArg3, iArg4, szArg5 );
		else
			StringCbCopy( szDest, iDestSize, szSource );
		vArgTypeList.clear();
	}
	else
	{
		char szArg2[MAX_STRING]="";
		StringCbPrintf( szArg2, sizeof( szArg2 ), "%d", iArg2 );
		char szArg3[MAX_STRING]="";
		StringCbPrintf( szArg3, sizeof( szArg3 ), "%d", iArg3 );
		char szArg4[MAX_STRING]="";
		StringCbPrintf( szArg4, sizeof( szArg4 ), "%d", iArg4 );

		va_list pArgs[] = {(va_list) szArg1, (va_list) szArg2, (va_list) szArg3, (va_list) szArg4, (va_list) szArg5 };
		SafeSprintfFunc_format(szSource, szDest, iDestSize, 5, pArgs);
	}
}

void SafeSprintf( char *szDest, int iDestSize, const char *szSource, int iArg1, const char *szArg2, int iArg3, int iArg4, const char *szArg5 )
{
	if( ioLocalManagerParent::IsVPrintf() )
	{
		vArgType vArgTypeList;
		vArgTypeList.reserve(5);
		vArgTypeList.push_back( AT_INT );
		vArgTypeList.push_back( AT_STRING );
		vArgTypeList.push_back( AT_INT );
		vArgTypeList.push_back( AT_INT );
		vArgTypeList.push_back( AT_STRING );
		if( IsRightArgs_printf( szSource, vArgTypeList ) )
			StringCbPrintf( szDest, iDestSize, szSource, iArg1, szArg2, iArg3, iArg4, szArg5 );
		else
			StringCbCopy( szDest, iDestSize, szSource );
		vArgTypeList.clear();
	}
	else
	{
		char szArg1[MAX_STRING]="";
		StringCbPrintf( szArg1, sizeof( szArg1 ), "%d", iArg1 );
		char szArg3[MAX_STRING]="";
		StringCbPrintf( szArg3, sizeof( szArg3 ), "%d", iArg3 );
		char szArg4[MAX_STRING]="";
		StringCbPrintf( szArg4, sizeof( szArg4 ), "%d", iArg4 );

		va_list pArgs[] = {(va_list) szArg1, (va_list) szArg2, (va_list) szArg3, (va_list) szArg4, (va_list) szArg5 };
		SafeSprintfFunc_format(szSource, szDest, iDestSize, 5, pArgs);
	}
}

void SafeSprintf( char *szDest, int iDestSize, const char *szSource, int iArg1, const char *szArg2, const char *szArg3, const char *szArg4, const char *szArg5 )
{
	if( ioLocalManagerParent::IsVPrintf() )
	{
		vArgType vArgTypeList;
		vArgTypeList.reserve(5);
		vArgTypeList.push_back( AT_INT );
		vArgTypeList.push_back( AT_STRING );
		vArgTypeList.push_back( AT_STRING );
		vArgTypeList.push_back( AT_STRING );
		vArgTypeList.push_back( AT_STRING );
		if( IsRightArgs_printf( szSource, vArgTypeList ) )
			StringCbPrintf( szDest, iDestSize, szSource, iArg1, szArg2, szArg3, szArg4, szArg5 );
		else
			StringCbCopy( szDest, iDestSize, szSource );
		vArgTypeList.clear();
	}
	else
	{
		char szArg1[MAX_STRING]="";
		StringCbPrintf( szArg1, sizeof( szArg1 ), "%d", iArg1 );

		va_list pArgs[] = {(va_list) szArg1, (va_list) szArg2, (va_list) szArg3, (va_list) szArg4, (va_list) szArg5 };
		SafeSprintfFunc_format(szSource, szDest, iDestSize, 5, pArgs);
	}
}


__EX void SafeSprintf( char *szDest, int iDestSize, const char *szSource, float fArg1, float fArg2, float fArg3, float fArg4, float fArg5 )
{
	if( ioLocalManagerParent::IsVPrintf() )
	{
		vArgType vArgTypeList;
		vArgTypeList.reserve(5);
		vArgTypeList.push_back( AT_FLOAT );
		vArgTypeList.push_back( AT_FLOAT );
		vArgTypeList.push_back( AT_FLOAT );
		vArgTypeList.push_back( AT_FLOAT );
		vArgTypeList.push_back( AT_FLOAT );
		if( IsRightArgs_printf( szSource, vArgTypeList ) )
			StringCbPrintf( szDest, iDestSize, szSource, fArg1, fArg2, fArg3, fArg4, fArg5 );
		else
			StringCbCopy( szDest, iDestSize, szSource );
		vArgTypeList.clear();
	}
	else
	{
		char szArg1[MAX_STRING]="";
		StringCbPrintf( szArg1, sizeof( szArg1 ), "%.2f", fArg1 );
		char szArg2[MAX_STRING]="";
		StringCbPrintf( szArg2, sizeof( szArg2 ), "%.2f", fArg2 );
		char szArg3[MAX_STRING]="";
		StringCbPrintf( szArg3, sizeof( szArg3 ), "%.2f", fArg3 );
		char szArg4[MAX_STRING]="";
		StringCbPrintf( szArg4, sizeof( szArg4 ), "%.2f", fArg4 );
		char szArg5[MAX_STRING]="";
		StringCbPrintf( szArg5, sizeof( szArg5 ), "%.2f", fArg5 );

		va_list pArgs[] = {(va_list) szArg1, (va_list) szArg2, (va_list) szArg3, (va_list) szArg4, (va_list) szArg5 };
		SafeSprintfFunc_format(szSource, szDest, iDestSize, 5, pArgs);
	}
}

void SafeSprintf( char *szDest, int iDestSize, const char *szSource, const char *szArg1, const char *szArg2, int iArg3, int iArg4, int iArg5 )
{
	if( ioLocalManagerParent::IsVPrintf() )
	{
		vArgType vArgTypeList;
		vArgTypeList.reserve(5);
		vArgTypeList.push_back( AT_STRING );
		vArgTypeList.push_back( AT_STRING );
		vArgTypeList.push_back( AT_INT );
		vArgTypeList.push_back( AT_INT );
		vArgTypeList.push_back( AT_INT );
		if( IsRightArgs_printf( szSource, vArgTypeList ) )
			StringCbPrintf( szDest, iDestSize, szSource, szArg1, szArg2, iArg3, iArg4, iArg5 );
		else
			StringCbCopy( szDest, iDestSize, szSource );
		vArgTypeList.clear();
	}
	else
	{
		char szArg3[MAX_STRING]="";
		StringCbPrintf( szArg3, sizeof( szArg3 ), "%d", iArg3 );
		char szArg4[MAX_STRING]="";
		StringCbPrintf( szArg4, sizeof( szArg4 ), "%d", iArg4 );
		char szArg5[MAX_STRING]="";
		StringCbPrintf( szArg5, sizeof( szArg5 ), "%d", iArg5 );

		va_list pArgs[] = {(va_list) szArg1, (va_list) szArg2, (va_list) szArg3, (va_list) szArg4, (va_list) szArg5 };
		SafeSprintfFunc_format(szSource, szDest, iDestSize, 5, pArgs);
	}
}

void SafeSprintf( char *szDest, int iDestSize, const char *szSource, const char *szArg1, const char *szArg2, const char *szArg3, const char *szArg4, const char *szArg5 )
{
	if( ioLocalManagerParent::IsVPrintf() )
	{
		vArgType vArgTypeList;
		vArgTypeList.reserve(5);
		vArgTypeList.push_back( AT_STRING );
		vArgTypeList.push_back( AT_STRING );
		vArgTypeList.push_back( AT_STRING );
		vArgTypeList.push_back( AT_STRING );
		vArgTypeList.push_back( AT_STRING );
		if( IsRightArgs_printf( szSource, vArgTypeList ) )
			StringCbPrintf( szDest, iDestSize, szSource, szArg1, szArg2, szArg3, szArg4, szArg5 );
		else
			StringCbCopy( szDest, iDestSize, szSource );
		vArgTypeList.clear();
	}
	else
	{
		va_list pArgs[] = {(va_list) szArg1, (va_list) szArg2, (va_list)szArg3, (va_list)szArg4, (va_list)szArg5 };
		SafeSprintfFunc_format(szSource, szDest, iDestSize, 5, pArgs);
	}
}

// 6
void SafeSprintf( char *szDest, int iDestSize, const char *szSource, float fArg1, float fArg2, float fArg3, float fArg4, float fArg5, float fArg6 )
{
	if( ioLocalManagerParent::IsVPrintf() )
	{
		vArgType vArgTypeList;
		vArgTypeList.reserve(6);
		vArgTypeList.push_back( AT_FLOAT );
		vArgTypeList.push_back( AT_FLOAT );
		vArgTypeList.push_back( AT_FLOAT );
		vArgTypeList.push_back( AT_FLOAT );
		vArgTypeList.push_back( AT_FLOAT );
		vArgTypeList.push_back( AT_FLOAT );
		if( IsRightArgs_printf( szSource, vArgTypeList ) )
			StringCbPrintf( szDest, iDestSize, szSource, fArg1, fArg2, fArg3, fArg4, fArg5, fArg6 );
		else
			StringCbCopy( szDest, iDestSize, szSource );
		vArgTypeList.clear();
	}
	else
	{
		char szArg1[MAX_STRING]="";
		StringCbPrintf( szArg1, sizeof( szArg1 ), "%.2f", fArg1 );
		char szArg2[MAX_STRING]="";
		StringCbPrintf( szArg2, sizeof( szArg2 ), "%.2f", fArg2 );
		char szArg3[MAX_STRING]="";
		StringCbPrintf( szArg3, sizeof( szArg3 ), "%.2f", fArg3 );
		char szArg4[MAX_STRING]="";
		StringCbPrintf( szArg4, sizeof( szArg4 ), "%.2f", fArg4 );
		char szArg5[MAX_STRING]="";
		StringCbPrintf( szArg5, sizeof( szArg5 ), "%.2f", fArg5 );
		char szArg6[MAX_STRING]="";
		StringCbPrintf( szArg6, sizeof( szArg6 ), "%.2f", fArg6 );

		va_list pArgs[] = {(va_list) szArg1, (va_list) szArg2, (va_list) szArg3, (va_list) szArg4, (va_list) szArg5, (va_list) szArg6 };
		SafeSprintfFunc_format(szSource, szDest, iDestSize, 6, pArgs);
	}
}

void SafeSprintf( char *szDest, int iDestSize, const char *szSource, const char *szArg1, const char *szArg2, int iArg3, int iArg4, int iArg5, const char *szArg6 )
{
	if( ioLocalManagerParent::IsVPrintf() )
	{
		vArgType vArgTypeList;
		vArgTypeList.reserve(6);
		vArgTypeList.push_back( AT_STRING );
		vArgTypeList.push_back( AT_STRING );
		vArgTypeList.push_back( AT_INT );
		vArgTypeList.push_back( AT_INT );
		vArgTypeList.push_back( AT_INT );
		vArgTypeList.push_back( AT_STRING );
		if( IsRightArgs_printf( szSource, vArgTypeList ) )
			StringCbPrintf( szDest, iDestSize, szSource, szArg1, szArg2, iArg3, iArg4, iArg5, szArg6 );
		else
			StringCbCopy( szDest, iDestSize, szSource );
		vArgTypeList.clear();
	}
	else
	{
		char szArg3[MAX_STRING]="";
		StringCbPrintf( szArg3, sizeof( szArg3 ), "%d", iArg3 );
		char szArg4[MAX_STRING]="";
		StringCbPrintf( szArg4, sizeof( szArg4 ), "%d", iArg4 );
		char szArg5[MAX_STRING]="";
		StringCbPrintf( szArg5, sizeof( szArg5 ), "%d", iArg5 );

		va_list pArgs[] = {(va_list) szArg1, (va_list) szArg2, (va_list) szArg3, (va_list) szArg4, (va_list) szArg5, (va_list) szArg6 };
		SafeSprintfFunc_format(szSource, szDest, iDestSize, 6, pArgs);
	}
}

void SafeSprintf( char *szDest, int iDestSize, const char *szSource, const int iArg1, int iArg2, const char *szArg3, int iArg4, int iArg5, const char *szArg6 )
{
	if( ioLocalManagerParent::IsVPrintf() )
	{
		vArgType vArgTypeList;
		vArgTypeList.reserve(6);
		vArgTypeList.push_back( AT_INT );
		vArgTypeList.push_back( AT_INT );
		vArgTypeList.push_back( AT_STRING );
		vArgTypeList.push_back( AT_INT );
		vArgTypeList.push_back( AT_INT );
		vArgTypeList.push_back( AT_STRING );
		if( IsRightArgs_printf( szSource, vArgTypeList ) )
			StringCbPrintf( szDest, iDestSize, szSource, iArg1, iArg2, szArg3, iArg4, iArg5, szArg6 );
		else
			StringCbCopy( szDest, iDestSize, szSource );
		vArgTypeList.clear();
	}
	else
	{
		char szArg1[MAX_STRING]="";
		StringCbPrintf( szArg1, sizeof( szArg1 ), "%d", iArg1 );
		char szArg2[MAX_STRING]="";
		StringCbPrintf( szArg2, sizeof( szArg2 ), "%d", iArg2 );
		char szArg4[MAX_STRING]="";
		StringCbPrintf( szArg4, sizeof( szArg4 ), "%d", iArg4 );
		char szArg5[MAX_STRING]="";
		StringCbPrintf( szArg5, sizeof( szArg5 ), "%d", iArg5 );

		va_list pArgs[] = {(va_list) szArg1, (va_list) szArg2, (va_list) szArg3, (va_list) szArg4, (va_list) szArg5, (va_list) szArg6 };
		SafeSprintfFunc_format(szSource, szDest, iDestSize, 6, pArgs);
	}
}

// 7
void SafeSprintf( char *szDest, int iDestSize, const char *szSource, const char *szArg1, int iArg2, int iArg3, int iArg4, int iArg5, int iArg6, const char *szArg7 )
{
	if( ioLocalManagerParent::IsVPrintf() )
	{
		vArgType vArgTypeList;
		vArgTypeList.reserve(7);
		vArgTypeList.push_back( AT_STRING );
		vArgTypeList.push_back( AT_INT );
		vArgTypeList.push_back( AT_INT );
		vArgTypeList.push_back( AT_INT );
		vArgTypeList.push_back( AT_INT );
		vArgTypeList.push_back( AT_INT );
		vArgTypeList.push_back( AT_STRING );
		if( IsRightArgs_printf( szSource, vArgTypeList ) )
			StringCbPrintf( szDest, iDestSize, szSource, szArg1, iArg2, iArg3, iArg4, iArg5, iArg6, szArg7 );
		else
			StringCbCopy( szDest, iDestSize, szSource );
		vArgTypeList.clear();
	}
	else
	{
		char szArg2[MAX_STRING]="";
		StringCbPrintf( szArg2, sizeof( szArg2 ), "%d", iArg2 );
		char szArg3[MAX_STRING]="";
		StringCbPrintf( szArg3, sizeof( szArg3 ), "%d", iArg3 );
		char szArg4[MAX_STRING]="";
		StringCbPrintf( szArg4, sizeof( szArg4 ), "%d", iArg4 );
		char szArg5[MAX_STRING]="";
		StringCbPrintf( szArg5, sizeof( szArg5 ), "%d", iArg5 );
		char szArg6[MAX_STRING]="";
		StringCbPrintf( szArg6, sizeof( szArg6 ), "%d", iArg6 );


		va_list pArgs[] = {(va_list) szArg1, (va_list) szArg2, (va_list) szArg3, (va_list) szArg4, (va_list) szArg5, (va_list) szArg6, (va_list) szArg7 };
		SafeSprintfFunc_format(szSource, szDest, iDestSize, 7, pArgs);
	}
}

// 8
void SafeSprintf( char *szDest, int iDestSize, const char *szSource, const char *szArg1, int iArg2, int iArg3, int iArg4, int iArg5, int iArg6, int iArg7, const char *szArg8 )
{
	if( ioLocalManagerParent::IsVPrintf() )
	{
		vArgType vArgTypeList;
		vArgTypeList.reserve(8);
		vArgTypeList.push_back( AT_STRING );
		vArgTypeList.push_back( AT_INT );
		vArgTypeList.push_back( AT_INT );
		vArgTypeList.push_back( AT_INT );
		vArgTypeList.push_back( AT_INT );
		vArgTypeList.push_back( AT_INT );
		vArgTypeList.push_back( AT_INT );
		vArgTypeList.push_back( AT_STRING );
		if( IsRightArgs_printf( szSource, vArgTypeList ) )
			StringCbPrintf( szDest, iDestSize, szSource, szArg1, iArg2, iArg3, iArg4, iArg5, iArg6, iArg7, szArg8 );
		else
			StringCbCopy( szDest, iDestSize, szSource );
		vArgTypeList.clear();
	}
	else
	{
		char szArg2[MAX_STRING]="";
		StringCbPrintf( szArg2, sizeof( szArg2 ), "%d", iArg2 );
		char szArg3[MAX_STRING]="";
		StringCbPrintf( szArg3, sizeof( szArg3 ), "%d", iArg3 );
		char szArg4[MAX_STRING]="";
		StringCbPrintf( szArg4, sizeof( szArg4 ), "%d", iArg4 );
		char szArg5[MAX_STRING]="";
		StringCbPrintf( szArg5, sizeof( szArg5 ), "%d", iArg5 );
		char szArg6[MAX_STRING]="";
		StringCbPrintf( szArg6, sizeof( szArg6 ), "%d", iArg6 );
		char szArg7[MAX_STRING]="";
		StringCbPrintf( szArg7, sizeof( szArg7 ), "%d", iArg7 );


		va_list pArgs[] = {(va_list) szArg1, (va_list) szArg2, (va_list) szArg3, (va_list) szArg4, (va_list) szArg5, (va_list) szArg6, (va_list) szArg7, (va_list) szArg8 };
		SafeSprintfFunc_format(szSource, szDest, iDestSize, 8, pArgs);
	}
}


void SafeSprintf( char *szDest, int iDestSize, const char *szSource, const char *szArg1, const char *szArg2, const char *szArg3, int iArg4, int iArg5, int iArg6, int iArg7, const char *szArg8 )
{
	if( ioLocalManagerParent::IsVPrintf() )
	{
		vArgType vArgTypeList;
		vArgTypeList.reserve(8);
		vArgTypeList.push_back( AT_STRING );
		vArgTypeList.push_back( AT_STRING );
		vArgTypeList.push_back( AT_STRING );
		vArgTypeList.push_back( AT_INT );
		vArgTypeList.push_back( AT_INT );
		vArgTypeList.push_back( AT_INT );
		vArgTypeList.push_back( AT_INT );
		vArgTypeList.push_back( AT_STRING );
		if( IsRightArgs_printf( szSource, vArgTypeList ) )
			StringCbPrintf( szDest, iDestSize, szSource, szArg1, szArg2, szArg3, iArg4, iArg5, iArg6, iArg7, szArg8 );
		else
			StringCbCopy( szDest, iDestSize, szSource );
		vArgTypeList.clear();
	}
	else
	{
		char szArg4[MAX_STRING]="";
		StringCbPrintf( szArg4, sizeof( szArg4 ), "%d", iArg4 );
		char szArg5[MAX_STRING]="";
		StringCbPrintf( szArg5, sizeof( szArg5 ), "%d", iArg5 );
		char szArg6[MAX_STRING]="";
		StringCbPrintf( szArg6, sizeof( szArg6 ), "%d", iArg6 );
		char szArg7[MAX_STRING]="";
		StringCbPrintf( szArg7, sizeof( szArg7 ), "%d", iArg7 );

		va_list pArgs[] = {(va_list) szArg1, (va_list) szArg2, (va_list) szArg3, (va_list) szArg4, (va_list) szArg5, (va_list) szArg6, (va_list) szArg7,(va_list) szArg8 };
		SafeSprintfFunc_format(szSource, szDest, iDestSize, 8, pArgs);
	}
}

