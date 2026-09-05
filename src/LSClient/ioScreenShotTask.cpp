
#include "stdafx.h"

#include "ioScreenShotTask.h"

#include <strsafe.h>
#include "Version.h"

ioScreenShotTask::ioScreenShotTask( const ioHashString &szSavePath,
									bool bSaveJPG,
									bool bSource16Bits )
 :m_SavePath( szSavePath )
{
	m_bSaveJPG = bSaveJPG;
	m_bSource16Bits = bSource16Bits;

	m_pSystem = NULL;
	m_iWidth  = 0;
	m_iHeight = 0;

	memset( m_szSaveFullPath, 0, sizeof(m_szSaveFullPath) );
	m_bDone = false;
}

ioScreenShotTask::~ioScreenShotTask()
{
}

void ioScreenShotTask::MakeFullPathName()
{
	char szFileExt[12];

	if( m_bSaveJPG )
		strcpy( szFileExt, "jpg" );
	else
		strcpy(szFileExt, "bmp");

	char szFullSavePath[MAX_PATH*2]="";
	_fullpath( szFullSavePath, m_SavePath.c_str(), sizeof( szFullSavePath ) );

	char szTestPath[MAX_PATH*2];
	StringCbPrintf( szTestPath, sizeof(szTestPath), "%s\\*.%s", szFullSavePath, szFileExt );	

	/*
	WIN32_FIND_DATA wfd;
	ZeroMemory( &wfd, sizeof(WIN32_FIND_DATA) );

	int iNameCount = 1;
	char szFileName[MAX_PATH];

	BOOL bResult = TRUE;
	HANDLE hSrch = FindFirstFile( szTestPath, &wfd );

	while( bResult )
	{
		if( !( wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) )
		{
			const char *pStr = strstr( wfd.cFileName, "LostSagaShot" );
			if( pStr )
			{
				_splitpath( pStr, NULL, NULL, szFileName, NULL );

				int iNumber = atoi( szFileName + 20 );	// 20 == strlen( "LostSagaShot_000000_" )
				iNameCount = max( iNameCount, iNumber + 1 );
			}
		}

		bResult = FindNextFile( hSrch, &wfd );
	}

	FindClose( hSrch );
	*/

	SYSTEMTIME st;
	GetLocalTime(&st);

	StringCbPrintf( m_szSaveFullPath,
					sizeof(m_szSaveFullPath),
					"%s\\LostSagaShot_%02d%02d%02d_%02d%02d%02d.%s",
					szFullSavePath,
					st.wYear - 2000,
					st.wMonth,
					st.wDay,
					st.wHour,
					st.wMinute,
					st.wSecond,
					szFileExt );
}

bool ioScreenShotTask::Initialize( ioRenderSystem *pSystem,
								   D3DTEXTUREFILTERTYPE eFilter,
								   float fSizeRate )
{
	m_pSystem = pSystem;

	if( m_pSystem->PrePareThreadCapture( eFilter, fSizeRate ) )
	{
		m_iWidth  = m_pSystem->GetWindowWidth() * fSizeRate;
		m_iHeight = m_pSystem->GetWindowHeight() * fSizeRate;

		return true;
	}

	return false;
}

void ioScreenShotTask::Process()
{
	MakeFullPathName();

	HRESULT hr;
	IDirect3DSurface9 *pSrc = m_pSystem->BeginThreadCapture();
	if( !pSrc || !m_pSystem->IsCaptureShotSafe() )
	{
		m_pSystem->EndThreadCapture();
		m_bDone = false;
		return;
	}

	hr = SaveSurfaceToFile( pSrc,
							m_iWidth,
							m_iHeight,
							m_bSource16Bits,
							m_szSaveFullPath );

	m_pSystem->EndThreadCapture();

	// 스크린샷에 정보 추가
	FILE *pFile = fopen( m_szSaveFullPath, "ab" );
	if( pFile )
	{
		char szText[MAX_PATH]="";
		StringCbPrintf( szText, sizeof( szText ), " [%d|%d]", g_MyInfo.GetUserIndex(), FILEVER );
		fwrite( szText, strlen( szText ), 1, pFile );
		fclose( pFile );
	}
	//

	if( SUCCEEDED(hr) )
		m_bDone = true;
	else
		m_bDone = false;
}

void ioScreenShotTask::Done()	
{
	g_App.ScreenShotDone( m_szSaveFullPath, m_bDone );
}

