

#ifndef _ErrorReport_h_
#define _ErrorReport_h_

namespace ErrorReport
{
	__EX void SetPosition( float fMajor, float fMinor );
	__EX void SetAndSoOnInfo( int iInfo );
	__EX void SetEnginePos( int iMajor, int iMinor );
	__EX void SetString( const char *szString );
	__EX void SetFormat( LPSTR fmt, ... );

	__EX float GetMajorPosition();
	__EX float GetMinorPosition();
	__EX int GetEngineMajorPos();
	__EX int GetEngineMinorPos();
	__EX int GetAndSoOnInfo();

	__EX const char* GetString();

	__EX int DynamicBufferFilter( DWORD dwErrorCode );

	__EX bool IsPacException();
	__EX void SetPacExeptionInfo( const char *szFileName );
	__EX const char* GetPacExceptionLog();
}

#endif