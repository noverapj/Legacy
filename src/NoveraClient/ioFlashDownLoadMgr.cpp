#include "stdafx.h"

#include "ioFlashDownLoadMgr.h"

#include "./GUI/ShopItemMovieWnd.h"

ioFlashDownLoadTask::ioFlashDownLoadTask( const char *szFullName, const char *szFullURL, FlashDownLoadData &kData ) 
	: m_FullName( szFullName ), 
	  m_FullURL( szFullURL )
{
	m_DownData = kData;
	m_bDone    = false;
}

ioFlashDownLoadTask::~ioFlashDownLoadTask()
{
}

void ioFlashDownLoadTask::Process()
{
	HRESULT hr = ::URLDownloadToFile( 0, m_FullURL.c_str(), m_FullName.c_str(),	0, NULL );

	if( FAILED(hr) )
		m_bDone = false;
	else
		m_bDone = true;
}

void ioFlashDownLoadTask::Done()
{
	g_FlashDownLoadMgr.FileDownLoadingDone( m_DownData, m_bDone );
}

template<> ioFlashDownLoadMgr* Singleton< ioFlashDownLoadMgr >::ms_Singleton = 0;

ioFlashDownLoadMgr::ioFlashDownLoadMgr()
{
}

ioFlashDownLoadMgr::~ioFlashDownLoadMgr()
{	
	m_DownDataList.clear();
	m_BaseDownDataList.clear();
}

ioFlashDownLoadMgr& ioFlashDownLoadMgr::GetSingleton()
{
	return Singleton< ioFlashDownLoadMgr >::GetSingleton();
}

void ioFlashDownLoadMgr::Initialize()
{
	//경로 저장
	char szBuf[MAX_PATH], szAppDir[MAX_PATH] = "";

	ioINILoader_e kLoader( "config/sp2_flash_config.ini" );
	kLoader.SetTitle_e( "common" );

	kLoader.LoadString_e( "default_flash_resource_dir", "", szBuf, MAX_PATH );
	m_szDefaultDir = szBuf;

	::GetCurrentDirectory( MAX_PATH, szAppDir );

	sprintf( szBuf, "%s/%s", szAppDir, m_szDefaultDir.c_str() );
	::CreateDirectory( szBuf, NULL );

	m_szResourceDir = szBuf;

	bool bSetDelete = kLoader.LoadBool_e( "set_delete_file", false );
	if ( bSetDelete )
		DeleteProcess();

	BaseDownLoad();
}

void ioFlashDownLoadMgr::BaseDownLoad()
{
	LoadBaseDownLoadList();

	for ( int i=0; i<(int)m_BaseDownDataList.size(); ++i )
	{
		const FlashBaseDownLoadData &sFlashBaseDownLoadData = m_BaseDownDataList[i];
		IsExistenceFile( sFlashBaseDownLoadData.m_szFileDir, sFlashBaseDownLoadData.m_szFileName, sFlashBaseDownLoadData.m_szUrlAddress, ioFlashDownLoadMgr::FDEP_ShopItemMovieWnd, false );
	}
}

bool ioFlashDownLoadMgr::IsFileCheck( const ioHashString &rkFileName )
{
	FILE *pFile = fopen( rkFileName.c_str(), "r" );
	if( pFile ) 
	{
		fclose( pFile );		
		return true;
	}
	return false;
}

bool ioFlashDownLoadMgr::IsDownLoading( FlashDownLoadData &rkData )
{
	FlashDownLoadDataList::iterator iter = std::find( m_DownDataList.begin(), m_DownDataList.end(), rkData );

	if( iter != m_DownDataList.end() )
		return true;

	return false;
}

bool ioFlashDownLoadMgr::IsExistenceFile( const ioHashString &szFileDir, const ioHashString &szFileName, const ioHashString &szUrlAddress, int nType, bool bAfterProcess )
{
	if( szFileName.IsEmpty() || szUrlAddress.IsEmpty() ) 
		return false;

	//폴더 생성
	CreateDownLoadDir( szFileDir );

	// 파일이 존재하는지 확인
	char szFlashPath[MAX_PATH] = "";
	sprintf( szFlashPath, "%s/%s%s", m_szResourceDir.c_str(), szFileDir.c_str(), szFileName.c_str() );
	if( bAfterProcess && IsFileCheck( szFlashPath ) ) 
		return true;
	
	FlashDownLoadData kDownLoad = { szFlashPath, szUrlAddress, nType, bAfterProcess };
	if( IsDownLoading( kDownLoad ) ) 
		return false;

	// 저장소 URL 생성
	char szStorageURL[MAX_PATH] = "";
	sprintf( szStorageURL, "%s", szUrlAddress.c_str() );

	ioFlashDownLoadTask *pTask = new ioFlashDownLoadTask( szFlashPath, szStorageURL, kDownLoad );
	if( pTask )
	{
		m_DownDataList.push_back( kDownLoad );
		g_ThreadTaskMgr.AddTask( pTask );
	}
	return false;
}

void ioFlashDownLoadMgr::FileDownLoadingDone( FlashDownLoadData &rkData, bool bDone )
{
	if ( rkData.m_bAfterProcess )
		DownLoadEndProcess( rkData.m_nType, rkData.m_szFileName, bDone );

	//다운로드 리스트 삭제
	m_DownDataList.remove( rkData );
}

void ioFlashDownLoadMgr::DownLoadEndProcess( int nType, const ioHashString &szFileName, bool bDone )
{
	switch( nType )
	{
	case FDEP_ShopItemMovieWnd:
		{
			// 파일 다운로드가 완료되면 여기서 처리한다.
			ShopItemMovieWnd *pShopItemMovieWnd = dynamic_cast<ShopItemMovieWnd*>( g_GUIMgr.FindWnd( SHOP_ITEM_MOVIE_WND ) );
			if ( pShopItemMovieWnd && pShopItemMovieWnd->IsShow() )
			{
				if ( bDone )
				{
					pShopItemMovieWnd->SetResetFlash();
					pShopItemMovieWnd->SetLoadFlash( szFileName );
				}
				else
				{
					pShopItemMovieWnd->SetDownFail();
				}
			}
		}
		break;
	}
}

void ioFlashDownLoadMgr::CreateDownLoadDir( const ioHashString &szFileDir )
{
	char szBuf[MAX_PATH];
	sprintf( szBuf, "%s/", m_szResourceDir.c_str() );

	ioHashString szCurDir = szBuf;
	StringVector kString = ioStringConverter::Split( szFileDir.c_str(), "/" );
	for ( int i=0; i<(int)kString.size(); ++i )
	{
		sprintf( szBuf, "%s/%s/", szCurDir.c_str(), kString[i].c_str() );
		::CreateDirectory( szBuf, NULL );
		szCurDir = szBuf;
	}
}

void ioFlashDownLoadMgr::NoneDeleteList( ioHashStringVec &szFileList )
{
	szFileList.clear();

	ioINILoader_e rkLoader( "config/sp2_flash_config.ini" );

	rkLoader.SetTitle_e( "undeletefile" );

	char szBuf[MAX_PATH], szKey[MAX_PATH];
	int nMaxInfo = rkLoader.LoadInt_e( "un_delete_file", 0 );
	for ( int i=0; i<nMaxInfo; ++i )
	{
		ioHashString szFilePath;
				
		wsprintf_e( szKey, "un_delete_file%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		szFilePath = szBuf;
		
		wsprintf_e( szBuf, "%s/%s", m_szResourceDir.c_str(), szFilePath.c_str() );
		szFileList.push_back( szBuf );
	}
}

void ioFlashDownLoadMgr::GetFilesFromDirectory( const char *Path, ioHashStringVec &szFileList )
{
	char szBuf[MAX_PATH];
	ioHashString szFilePath;
	WIN32_FIND_DATA file_data;

	wsprintf_e( szBuf, "%s\\*.*", Path );

	HANDLE search_handle = FindFirstFile( szBuf, &file_data );
	if( search_handle != INVALID_HANDLE_VALUE )
	{
		do 
		{
			if( file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
			{
				if( file_data.cFileName[0] != '.' )
				{
					wsprintf_e( szBuf, "%s/%s", Path, file_data.cFileName );
					szFilePath = szBuf;

					szFileList.push_back( szBuf );
					GetFilesFromDirectory( szFilePath.c_str(), szFileList );
				}
			} 
			else 
			{
				wsprintf_e( szBuf, "%s/%s", Path, file_data.cFileName );
				szFilePath = szBuf;

				szFileList.push_back( szBuf );
			}
		}while( FindNextFile( search_handle, &file_data ) );

		FindClose(search_handle);
	}
}

void ioFlashDownLoadMgr::DeleteFileList( ioHashStringVec &szFileList )
{
	if ( szFileList.empty() )
		return;

	int nCnt = (int)szFileList.size();
	for ( int i=0; i<nCnt; ++i )
		DeleteFile( szFileList[i].c_str() );

	for ( int i=0; i<nCnt; ++i )
		RemoveDirectory( szFileList[i].c_str() );

	szFileList.clear();
}

bool ioFlashDownLoadMgr::SearchFileList( ioHashStringVec &szFileList, const char* szFileName )
{
	int nCnt = (int)szFileList.size();
	for ( int i=0; i<nCnt; ++i )
	{
		if ( strcmp( szFileList[i].c_str(), szFileName ) == 0 )
			return true;
	}
	return false;
}

void ioFlashDownLoadMgr::DeleteProcess()
{
	ioHashStringVec szFileList;
	ioHashStringVec szNoneDeleteList;
	ioHashStringVec szDeleteList;

	//삭제 안하는 파일 목록
	NoneDeleteList( szNoneDeleteList );

	//전체 파일 목록 얻기
	GetFilesFromDirectory( m_szResourceDir.c_str(), szFileList );

	//파일 최종 삭제 목록 작성
	int nCnt = (int)szFileList.size();
	if ( nCnt > 0 )
	{
		for ( int i=0; i<nCnt; ++i )
		{
			if ( !SearchFileList( szNoneDeleteList, szFileList[i].c_str()) )
				szDeleteList.push_back( szFileList[i] );
		}

		//파일 삭제
		DeleteFileList( szDeleteList );
	}
	else
	{
		DeleteFileList( szFileList );
	}

	szFileList.clear();
	szNoneDeleteList.clear();
	szDeleteList.clear();
}

void ioFlashDownLoadMgr::LoadBaseDownLoadList()
{
	ioINILoader_e rkLoader( "config/sp2_flash_config.ini" );

	rkLoader.SetTitle_e( "base_down" );

	char szBuf[MAX_PATH], szKey[MAX_PATH];
	int nMaxInfo = rkLoader.LoadInt_e( "base_down_cnt", 0 );
	for ( int i=0; i<nMaxInfo; ++i )
	{
		FlashBaseDownLoadData sFlashBaseDownLoadData;

		wsprintf_e( szKey, "base_down%d_dir", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		sFlashBaseDownLoadData.m_szFileDir = szBuf;

		wsprintf_e( szKey, "base_down%d_file", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		sFlashBaseDownLoadData.m_szFileName = szBuf;

		wsprintf_e( szKey, "base_down%d_url", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		sFlashBaseDownLoadData.m_szUrlAddress = szBuf;

		m_BaseDownDataList.push_back( sFlashBaseDownLoadData );
	}
}