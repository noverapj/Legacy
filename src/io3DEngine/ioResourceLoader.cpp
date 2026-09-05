

#include "stdafx.h"

#include "ioStream.h"
#include "ioDataProcessor.h"
#include "ioResourceLoader.h"

#include "ErrorReport.h"
#include "HelpFunc.h"

#include "../ioPac/ioPac.h"
#include <strsafe.h>
#include "ioLocalManagerParent.h"

ioResourceLoader* ioResourceLoader::m_pLoader = NULL;

DWORD WINAPI _FileLoadThreadProc( LPVOID lpParameter )
{
	return ( (ioResourceLoader*)lpParameter )->LoadingThreadProc();
}

ioResourceLoader& ioResourceLoader::GetInstance()
{
	if( m_pLoader )
		return *m_pLoader;

	m_pLoader = new ioResourceLoader;
	return *m_pLoader;
}

void ioResourceLoader::ReleaseInstance()
{
	SAFEDELETE( m_pLoader );
}

ioResourceLoader::ioResourceLoader()
{
	m_hQueueSemaphore = INVALID_HANDLE_VALUE;
	m_hLoadThread = INVALID_HANDLE_VALUE;

	m_bNowRunningThread = false;
	m_bLoadThreadDone = false;
	m_bDone = false;

	m_bLoadPackFile = false;
	m_bBeforeThreadLoading = false;
}

ioResourceLoader::~ioResourceLoader()
{
	if( !m_bDone )
	{
		DestroyThreadObjects();
		g_Pac.ReleaseInstance();
	}
}

void ioResourceLoader::Initialize( bool bLoadInPack )
{
	m_bLoadPackFile = bLoadInPack;
	InitializeThreadObjects();
	g_Pac.AddDeleteList("_map"); // map.iop
	g_Pac.AddDeleteList("");     // rs.iop

	// parsing
	char szRootDir[MAX_PATH];
	::GetCurrentDirectory(MAX_PATH,szRootDir);

	char szPath[MAX_PATH*2]="";
	StringCbPrintf(szPath, sizeof(szPath), "%s\\info\\delete_file_list.cfg", szRootDir);

	SetFileAttributes(szPath,FILE_ATTRIBUTE_NORMAL);
	::DeleteFile(szPath);

	char szPacPath[MAX_PATH*2]="";
	StringCbPrintf(szPacPath, sizeof(szPacPath), "%s\\info\\delete_file_list.cfg.iop", szRootDir);

	FILE *pFile = fopen( szPacPath, "rb");
	if( pFile == NULL ) return;
	fclose(pFile);

	// 파일이 없으면 예외 발생.
	if( g_Pac.UnPacPieceFile(szPacPath) != CPacUtil::RV_OK )
		ErrorReport::SetPacExeptionInfo( "delete_file_list.cfg.iop" );
	else
		g_Pac.LoadDeleteList(szPath);
	SetFileAttributes(szPath,FILE_ATTRIBUTE_NORMAL);
	::DeleteFile(szPath);
}

bool ioResourceLoader::AddPackFile( const char *szFileName , const char *szDeleteListToken )
{
	if( g_Pac.Add( szFileName ) )
	{
		CBigPac *pBigPac = g_Pac.GetPac( szFileName );
		if( pBigPac )
		{
			char szEncPW[MAX_PASSWORD+1]="";
			ioLocalManagerParent::GetPacPassword(szEncPW, MAX_PASSWORD, 0, ioLocalManagerParent::GetLocalType() );
			pBigPac->OpenBigPac( szFileName, true, true, szEncPW );
			pBigPac->DeleteFileInfo( g_Pac.GetDeleteList( szDeleteListToken ) );
			return true;
		}
		else
		{
			g_Pac.Remove( szFileName );
		}
	}
	return false;
}

bool ioResourceLoader::RemovePackFile( const char *szFileName )
{
	if( g_Pac.Remove( szFileName ) )
		return true;

	return false;
}

bool ioResourceLoader::BeginDividePackFile( const char *szPackFileName, int iDivideCount , const char *szDeleteListToken )
{
	CBigPac *pBigPac = g_Pac.GetPac( szPackFileName );
	if( !pBigPac )
	{
		LOG.PrintTimeAndLog( 0, "ioResourceLoader::BeginDividePackFile - %s File Not Exist", szPackFileName );
		return false;
	}

	m_bBeforeThreadLoading = m_bNowRunningThread;
	if( m_bNowRunningThread )
	{
		StopLoadingThread();
	}

	if( !pBigPac->BeginDividePackFile( szPackFileName, iDivideCount ) )
	{
		LOG.PrintTimeAndLog( 0, "ioResourceLoader::BeginDividePackFile - %s Open Failed", szPackFileName );
		return false;
	}

	m_DivideLoadingName = szPackFileName;
	m_DivideDeleteListToken = szDeleteListToken;
	return true;
}

bool ioResourceLoader::LoadDividePackFile()
{
	CBigPac *pBigPac = g_Pac.GetPac( m_DivideLoadingName.c_str() );
	if( !pBigPac )
	{
		LOG.PrintTimeAndLog( 0, "ioResourceLoader::LoadDividePackFile - %s Not Exist", m_DivideLoadingName.c_str() );
		return false;
	}

	char szErrorLog[MAX_PATH * 2];
	CBigPac::DivideLoadResult eResult;

	char szEncPW[MAX_PASSWORD+1]="";
	ioLocalManagerParent::GetPacPassword(szEncPW, MAX_PASSWORD, 0, ioLocalManagerParent::GetLocalType() );
	eResult = pBigPac->LoadDividePackFile( szErrorLog, szEncPW );

	switch( eResult )
	{
	case CBigPac::DLR_ERROR:
		LOG.PrintTimeAndLog( 0, "ioResourceLoader::LoadDividePackFile - %s", szErrorLog );
		return false;
	case CBigPac::DLR_LOADING:
		// loading...
		return true;
	case CBigPac::DLR_DONE:
		pBigPac->DeleteFileInfo( g_Pac.GetDeleteList( m_DivideDeleteListToken.c_str() ) );
		if( m_bBeforeThreadLoading )
		{
			ResumeLoadingThread();
		}
		return true;
	}

	LOG.PrintTimeAndLog( 0, "ioResourceLoader::LoadDividePackFile - CRITICAL ERROR Unknown Code(%d)", eResult );
	return false;
}

void ioResourceLoader::EndDividePackFile()
{
	m_DivideLoadingName.Clear();
	m_DivideDeleteListToken.Clear();
}

void ioResourceLoader::ResumeLoadingThread()
{
	m_bNowRunningThread = true;
}

void ioResourceLoader::StopLoadingThread()
{
	if( !m_bNowRunningThread )
		return;

	RequestQueue kTempQueue;
	EnterCriticalSection( &m_RequestSection );
	kTempQueue = m_RequestQueue;
	m_RequestQueue.clear();
	LeaveCriticalSection( &m_RequestSection );

	EnterCriticalSection( &m_LoadInternalLock );

	RequestQueue::iterator iter;
	for( iter=kTempQueue.begin() ; iter!=kTempQueue.end() ; ++iter )
	{
		LoadRequest &rkRequest = *iter;

		bool bLoaded = LoadInternal( rkRequest.m_szFileName.c_str(),
			rkRequest.m_pStream,
			rkRequest.m_bLoadPiece );

		if ( !bLoaded )
		{
			LOG.PrintTimeAndLog( 0, "ioResourceLoader::StopLoadingThread - LoadInternal Fail" );
		}

		rkRequest.m_pProcessor->NotifyLoadDone( rkRequest.m_pStream,
			rkRequest.m_pParam,
			bLoaded );

	}

	LeaveCriticalSection( &m_LoadInternalLock );

	m_bNowRunningThread = false;
}

void ioResourceLoader::InitializeThreadObjects()
{
	m_hQueueSemaphore = CreateSemaphore( NULL, 0, LONG_MAX, NULL );

	InitializeCriticalSection( &m_RequestSection );
	InitializeCriticalSection( &m_LoadInternalLock );

	m_hLoadThread = CreateThread( NULL, 0, _FileLoadThreadProc, (LPVOID)this, CREATE_SUSPENDED, NULL );
	ResumeThread( m_hLoadThread );

	m_bNowRunningThread = true;
}

void ioResourceLoader::DestroyThreadObjects()
{
	m_bDone = true;

	// Waiting Thread
	int iTryCount = 0;
	if( m_bNowRunningThread )
	{
		while( !m_bLoadThreadDone && iTryCount < 10 )
		{
			ReleaseSemaphore( m_hQueueSemaphore, 1, NULL );
			iTryCount++;

			::SleepEx( 500, FALSE );
		}
	}

	if( iTryCount < 10 )
	{
		CloseHandle( m_hLoadThread );
		LOG.PrintTimeAndLog( 0, "Close LoadThread" );
	}
	else
	{
		TerminateThread( m_hLoadThread, 0 );
		LOG.PrintTimeAndLog( 0, "Terminate LoadThread" );
	}

	CloseHandle( m_hQueueSemaphore );
	DeleteCriticalSection( &m_RequestSection );
	DeleteCriticalSection( &m_LoadInternalLock );

	// 종료시점에 로드되지 못한 요청사항들
	RequestQueue::iterator iter;
	for( iter=m_RequestQueue.begin() ; iter!=m_RequestQueue.end() ; ++iter )
	{
		SAFEDELETE( iter->m_pStream );
	}
	m_RequestQueue.clear();
}

bool ioResourceLoader::IsFileExist( const char *szFileName, bool bInPackOnly )
{
	if( IsLoadInPack() || bInPackOnly )
	{
		int iIndex = BIGPAC_ERROR_INDEX;
		if( g_Pac.GetPacHasFile( szFileName, iIndex ) )
			return true;
	}
	else
	{
		HANDLE hFile = CreateFile( szFileName, 
			GENERIC_READ,
			FILE_SHARE_READ,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL );

		if( hFile != INVALID_HANDLE_VALUE )
		{
			CloseHandle( hFile );
			return true;
		}
	}

	return false;
}


bool ioResourceLoader::LoadStream( const char *szFileName,
	ioStream *pStream,
	ioDataProcessor *pProcessor,
	void *pParam,
	bool bLoadPiece )
{
	bool bLoaded = true;

	if( m_bNowRunningThread )
	{
		if( !pProcessor )
		{
			EnterCriticalSection( &m_LoadInternalLock );
			bLoaded = LoadInternal( szFileName, pStream, bLoadPiece );
			if ( !bLoaded )
			{
				LOG.PrintTimeAndLog( 0, "ioResourceLoader::LoadStream1 - LoadInternal Fail" );
			}
			LeaveCriticalSection( &m_LoadInternalLock );
		}
		else
		{
			LoadRequest kRequest;
			kRequest.m_szFileName = szFileName;
			kRequest.m_pStream = pStream;
			kRequest.m_pProcessor = pProcessor;
			kRequest.m_pParam = pParam;
			kRequest.m_bLoadPiece = bLoadPiece;

			EnterCriticalSection( &m_RequestSection );
			m_RequestQueue.push_back( kRequest );
			LeaveCriticalSection( &m_RequestSection );

			ReleaseSemaphore( m_hQueueSemaphore, 1, NULL );
		}
	}
	else
	{
		bLoaded = LoadInternal( szFileName, pStream, bLoadPiece );
		if ( !bLoaded )
		{
			LOG.PrintTimeAndLog( 0, "ioResourceLoader::LoadStream2 - LoadInternal Fail" );
		}
		if( pProcessor )
		{
			pProcessor->NotifyLoadDone( pStream, pParam, bLoaded );
		}
	}

	return bLoaded;
}

bool ioResourceLoader::LoadInternal( const char *szFileName,
	ioStream *pStream,
	bool bLoadPiece )
{
	if( !IsLoadInPack() || bLoadPiece )
	{
		if( !pStream->OpenFile( szFileName ) )
		{
			LOG.PrintTimeAndLog( 0, "ioResourceLoader::LoadInternal - %s Not Exist", szFileName );
			return false;
		}
	}
	else
	{
		int iIndex = BIGPAC_ERROR_INDEX;

		CBigPac *pBigPac = g_Pac.GetPacHasFile( szFileName, iIndex );
		if( !pBigPac )
		{
			LOG.PrintTimeAndLog( 0, "ioResourceLoader::LoadInternal - %s (%d) Not Exist In Pack", szFileName , iIndex);

			if( m_szLastFailedFileName.IsEmpty() )
				m_szLastFailedFileName = szFileName;
			return false;
		}

		int iSize = pBigPac->GetFileSize( iIndex );
		if( iSize > 0 )
		{
			BYTE *pBuf = pStream->OpenMemory( iSize );
			if( !pBuf )
			{
				LOG.PrintTimeAndLog( 0, "ioResourceLoader::LoadInternal - %s (%d) (%d) (%x) Buffer NULL", szFileName , iIndex , iSize, GetLastError());
				return false;
			}

			if( !pBigPac->GetBuffer( iIndex, pBuf, iSize ) )
			{
				if( ErrorReport::IsPacException() )
				{
					char szErrorInfo[MAX_PATH*2]="";
					StringCbPrintf( szErrorInfo, sizeof(szErrorInfo), "%d:%d:%d:%s", iIndex, iSize, pStream->GetSize(), szFileName );
					ErrorReport::SetPacExeptionInfo( szErrorInfo );
				}

				LOG.PrintTimeAndLog( 0, "ioResourceLoader::LoadInternal - %s (%d) (%d) GetBuffer Error", szFileName , iIndex , iSize );
				return false;
			}

			if( strstr( szFileName, ".ini" ) != NULL )
			{
				char szComment[MAX_PATH];//="";
				szComment[0] = 0;
				pBigPac->GetFileComment( szComment, sizeof( szComment ), iIndex );

				if( atoi( szComment ) == 1 )
					ioLocalManagerParent::EncryptDecryptData( (char*)pBuf, iSize, (const char*) pBuf, iSize, false );
			}

		}
		else
		{
			LOG.PrintTimeAndLog( 0, "ioResourceLoader::LoadInternal - %s (%d) Size(%d) Error", szFileName , iIndex, iSize );
			return false;
		}
	}

	return true;
}

int ioResourceLoader::LoadingThreadProc()
{
	m_bLoadThreadDone = false;

	LoadRequest kRequest;
	while( !m_bDone )
	{
		WaitForSingleObject( m_hQueueSemaphore, INFINITE );

		if( m_bDone )
			break;

		EnterCriticalSection( &m_RequestSection );
		if( !m_RequestQueue.empty() )
		{
			kRequest = m_RequestQueue.front();
			m_RequestQueue.pop_front();
			LeaveCriticalSection( &m_RequestSection );
		}
		else
		{
			LeaveCriticalSection( &m_RequestSection );
			continue;
		}

		EnterCriticalSection( &m_LoadInternalLock );

		bool bLoaded = LoadInternal( kRequest.m_szFileName.c_str(),
			kRequest.m_pStream,
			kRequest.m_bLoadPiece );
		if ( !bLoaded )
		{
			LOG.PrintTimeAndLog( 0, "ioResourceLoader::LoadingThreadProc - LoadInternal Fail" );
		}
		kRequest.m_pProcessor->NotifyLoadDone( kRequest.m_pStream,
			kRequest.m_pParam,
			bLoaded );

		LeaveCriticalSection( &m_LoadInternalLock );
	}

	m_bLoadThreadDone = true;
	return 0;
}

void ioResourceLoader::ClearDeleteFileList()
{
	g_Pac.RemoveAllDeleteList();
}

bool ioResourceLoader::PacPieceFileDataInMem( IN const char* szFileName , OUT DWORD& rdwDataSize )
{
	g_Pac.NewMemFile();
	if( !g_Pac.PacPieceFileInMem( szFileName ) )
	{
		g_Pac.DeleteMemFile();
		return false;
	}

	rdwDataSize = g_Pac.GetMemFileSize();
	if( rdwDataSize <= 0 )
	{
		g_Pac.DeleteMemFile();
		return false;
	}
	return true;
}

bool ioResourceLoader::GetPacPieceFileData( OUT BYTE *pPacData, IN DWORD dwPacSize )
{
	if( !g_Pac.GetPacPieceFileData( pPacData, dwPacSize ) )
	{
		g_Pac.DeleteMemFile();
		return false;
	}

	g_Pac.DeleteMemFile();
	return true;
}

bool ioResourceLoader::UnPacPieceFileByMem( IN const char* szFileName, IN BYTE* pPacData , IN DWORD dwDataSize )
{
	return g_Pac.UnPacPieceFileByMem( szFileName, pPacData, dwDataSize );
}

void  ioResourceLoader::ClearLastFailedFileName()
{
	m_szLastFailedFileName.Clear();
}

int ioResourceLoader::GetPackFileMemSize( IN const char* szFileName )
{	
	CBigPac *pBigPac = g_Pac.GetPac( szFileName );
	if( pBigPac )
	{
		return pBigPac->GetTotalLoadBytes();
	}

	return 0;
}