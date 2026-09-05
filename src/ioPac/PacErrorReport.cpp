

#include "stdafx.h"
#include "PacErrorReport.h"

namespace PacErrorReport
{
	static int s_iError = 0;
	static char s_ErrorLog[MAX_PATH * 2]="";

	PAC_API bool IsException()
	{
		if( s_iError == 0 )
			return false;

		return true;
	}

	PAC_API void SetPacException( const char *szFile, int iLine, int iCode )
	{
		// 최초 일어난 익셉션을 다음 일어난 익셉션이 덮어 씌우지 못하게 하기 위함
		if( s_iError == 0 )
		{
			wsprintf( s_ErrorLog, "%s(%d) : %d", szFile, iLine, iCode );
			s_iError = iCode;
		}
	}

	PAC_API const char* GetPacExceptionCode()
	{
		return s_ErrorLog;
	}
}
