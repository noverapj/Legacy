

#ifndef _PacErrorReport_h_
#define _PacErrorReport_h_

namespace PacErrorReport
{
	PAC_API bool IsException();
	PAC_API void SetPacException( const char *szFile, int iLine, int iCode );
	PAC_API const char* GetPacExceptionCode();
}

#endif