

#include "stdafx.h"

#include "ErrorReport.h"

#include "../ioPac/ioPac.h"
#include "../ioPac/PacErrorReport.h"

#define MAX_ERROR_STRING	1024

namespace ErrorReport
{
	static float s_fErrorMajor = 0;
	static float s_fErrorMinor = 0;

	static int s_iErrorAndSoOnInt = 0;

	static int s_iEngineMajor = 0;
	static int s_iEngineMinor = 0;

	static char s_szErrorString[MAX_ERROR_STRING]="";

	static char s_szLoadFailName[MAX_PATH]="";
	static char s_szPacExceptionFull[MAX_ERROR_STRING]="";
	static bool s_bPacException = false;

	__EX void SetPosition( float fMajor, float fMinor )
	{
		s_fErrorMajor = fMajor;
		s_fErrorMinor = fMinor;
	}

	__EX void SetEnginePos( int iMajor, int iMinor )
	{
		s_iEngineMajor = iMajor;
		s_iEngineMinor = iMinor;
	}

	__EX void SetString( const char *szString )
	{
		strncpy( s_szErrorString, szString, MAX_ERROR_STRING - 1 );
	}

	__EX void SetFormat( LPSTR fmt, ... )
	{
		va_list args;
		va_start( args, fmt );
		vsprintf( s_szErrorString, fmt, args );
		va_end( args );
	}

	__EX void SetAndSoOnInfo( int iInfo )
	{
		s_iErrorAndSoOnInt = iInfo;
	}

	__EX int GetAndSoOnInfo()
	{
		return s_iErrorAndSoOnInt;
	}

	__EX float GetMajorPosition()
	{
		return s_fErrorMajor;
	}

	__EX float GetMinorPosition()
	{
		return s_fErrorMinor;
	}

	__EX int GetEngineMajorPos()
	{
		return s_iEngineMajor;
	}

	__EX int GetEngineMinorPos()
	{
		return s_iEngineMinor;
	}

	__EX const char* GetString()
	{
		return s_szErrorString;
	}

	__EX int DynamicBufferFilter( DWORD dwErrorCode )
	{
		if( dwErrorCode == EXCEPTION_ACCESS_VIOLATION )
			return EXCEPTION_EXECUTE_HANDLER;

		return EXCEPTION_CONTINUE_SEARCH;
	}

	__EX bool IsPacException()
	{
		return PacErrorReport::IsException();
	}

	__EX void SetPacExeptionInfo( const char *szFileName )
	{
		if( !s_bPacException )
		{
			wsprintf( s_szPacExceptionFull, "%s : %s", szFileName, PacErrorReport::GetPacExceptionCode() );
			s_bPacException = true;
		}
	}

	__EX const char* GetPacExceptionLog()
	{
		return s_szPacExceptionFull;
	}
}
