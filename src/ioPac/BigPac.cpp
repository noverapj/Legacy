// BigPac.cpp: implementation of the CBigPac class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BigPac.h"
#include "ZipArchive/ZipArchive.h"
#include "PacErrorReport.h"

#include <strsafe.h>

#ifdef _DEBUG
#include <algorithm>
#include <sstream>
#include <fstream>
#include <map>
#include <atlstr.h> //cstring 용
#endif
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBigPac::CBigPac()
{
#ifdef STATIC_PAC_API
	//m_SetIter = NULL;
	m_GarbageSize = 0;
#endif

	m_pZip = NULL;
	m_iMaxIndex = 0;

	m_pMemFile = NULL;
	m_pMemFileBuf = NULL;

	m_hDivideFile = INVALID_HANDLE_VALUE;
	m_dwTotalLoadBytes   = 0;
	m_dwOneTimeLoadBytes = 0;
	m_dwCurLoadedBytes   = 0;
}

CBigPac::~CBigPac()
{
	CloseBigPac();
	ClearDivideLoadResource();
}

bool CBigPac::OpenBigPac( IN const char* szPath, IN bool bReadOnly /*= true */, IN bool bBuildSetFileInfo /*= true*/, IN const char *szPassword /*= "" */ )
{
	CloseBigPac();

	try
	{
		m_pZip = new CZipArchive;

		if( bReadOnly )
			m_pZip->Open(_T(szPath), CZipArchive::zipOpenReadOnly );
		else
			m_pZip->Open(_T(szPath), CZipArchive::zipOpen );

		if( strcmp( szPassword , "") != 0)
		{
			if( !m_pZip->SetPassword(_T(szPassword) ) )
			{
				m_pZip->Close();
				DestroyZip();
				return false;
			}
		}

		if( bBuildSetFileInfo )
		{
			if( !BuildSetFileInfo() )
			{
				m_pZip->Close();
				ClearSetFileInfo();
				DestroyZip();
				return false;
			}
		}
		else
		{
			m_iMaxIndex = m_pZip->GetCount();
		}
	}
	catch( CZipException e )
	{
		PacErrorReport::SetPacException( __FILE__, __LINE__, e.m_iCause );

		m_pZip->Close( CZipArchive::afAfterException );
		ClearSetFileInfo();
		DestroyZip();
		return false;
	}
	catch(...)
	{
		m_pZip->Close( CZipArchive::afAfterException );
		ClearSetFileInfo();
		DestroyZip();
		return false;
	}
	
	return true;
}

bool CBigPac::BeginDividePackFile( const char *szPackFileName, int iDivideCount )
{
	CloseBigPac();
	ClearDivideLoadResource();

	m_hDivideFile = CreateFile( szPackFileName,
								GENERIC_READ,
								0,
								NULL,
								OPEN_EXISTING,
								FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,
								NULL );

	if( m_hDivideFile == INVALID_HANDLE_VALUE )
		return false;

	iDivideCount = max( iDivideCount, 1 );

	DWORD dwFileSize = ::GetFileSize( m_hDivideFile, NULL );
	m_pMemFileBuf        = (BYTE*)malloc( dwFileSize );
	if( m_pMemFileBuf == NULL )
		return false;

	m_dwTotalLoadBytes   = dwFileSize;
	m_dwOneTimeLoadBytes = dwFileSize / iDivideCount;
	m_dwCurLoadedBytes   = 0;

	return true;
}

CBigPac::DivideLoadResult CBigPac::LoadDividePackFile( char *szErrorLog , IN const char *szPassword /*= "" */ )
{
	if( m_hDivideFile == INVALID_HANDLE_VALUE)
	{
		wsprintf( szErrorLog, "File Not Opened(Handle Err)" );
		return DLR_ERROR;
	}

	if( !m_pMemFileBuf )
	{
		wsprintf( szErrorLog, "File Not Opened(Mem Err)" );
		return DLR_ERROR;
	}

	if( m_dwTotalLoadBytes > m_dwCurLoadedBytes )
	{
		DWORD dwCurrentLoadBytes = 0;
		DWORD dwRemainLoadBytes = m_dwTotalLoadBytes - m_dwCurLoadedBytes;

		if( dwRemainLoadBytes >= m_dwOneTimeLoadBytes * 2 )
		{
			dwCurrentLoadBytes = m_dwOneTimeLoadBytes;
		}
		else
		{
			dwCurrentLoadBytes = dwRemainLoadBytes;	// 나누기에 의한 나머지까지 한꺼번에 로딩한다.
		}

		DWORD dwReadBytes = 0;
		BOOL bRet = ReadFile( m_hDivideFile,
							  m_pMemFileBuf + m_dwCurLoadedBytes,
							  dwCurrentLoadBytes,
							  &dwReadBytes,
							  NULL );

		if( !bRet )
		{
			wsprintf( szErrorLog, "ReadFile Failed(%d), Total:%d, Loaded:%d, Try:%d, Read:%d",
								  GetLastError(),
								  m_dwTotalLoadBytes,
								  m_dwCurLoadedBytes,
								  dwCurrentLoadBytes,
								  dwReadBytes );

			return DLR_ERROR;
		}

		if( dwReadBytes != 0 && dwReadBytes != dwCurrentLoadBytes )
		{
			wsprintf( szErrorLog, "Try(%d) and Read(%d) Not Matched", dwCurrentLoadBytes, dwReadBytes );
			return DLR_ERROR;
		}

		m_dwCurLoadedBytes += dwCurrentLoadBytes;
	}

	if( m_dwTotalLoadBytes == m_dwCurLoadedBytes )
	{
		if( !m_pMemFile )
		{
			CloseHandle( m_hDivideFile );
			m_hDivideFile = INVALID_HANDLE_VALUE;

			m_pMemFile = new CZipMemFile( m_pMemFileBuf, m_dwTotalLoadBytes );

			if( !OpenBigPacInMemory( m_pMemFile, szPassword ) )
			{
				wsprintf( szErrorLog, "OpenBigPacInMemory Failed" );
				return DLR_ERROR;
			}

			return DLR_DONE;
		}
	}

	return DLR_LOADING;
}

bool CBigPac::OpenBigPacInMemory( CZipMemFile *pMemFile , IN const char *szPassword /*= ""*/ )
{
	CloseBigPac();

	try
	{
		m_pZip = new CZipArchive;
		m_pZip->Open( *pMemFile, CZipArchive::zipOpenReadOnly );

		if( strcmp( szPassword, "") != 0 )
		{
			if( !m_pZip->SetPassword( _T(szPassword) ) )
			{
				m_pZip->Close();
				DestroyZip();
				return false;
			}
		}

        if( !BuildSetFileInfo() )
		{
			m_pZip->Close();
			ClearSetFileInfo();
			DestroyZip();
			return false;
		}
	}
	catch( CZipException e )
	{
		PacErrorReport::SetPacException( __FILE__, __LINE__, e.m_iCause );

		m_pZip->Close( CZipArchive::afAfterException );
		ClearSetFileInfo();
		DestroyZip();
		return false;
	}
	catch(...)
	{
		m_pZip->Close( CZipArchive::afAfterException );
		ClearSetFileInfo();
		DestroyZip();
		return false;
	}
	
	return true;
}

void CBigPac::ClearDivideLoadResource()
{
	if( m_hDivideFile != INVALID_HANDLE_VALUE )
	{
		CloseHandle( m_hDivideFile );
		m_hDivideFile = INVALID_HANDLE_VALUE;
	}

	if( m_pMemFile )
	{
		m_pMemFile->Detach();
		m_pMemFile->Close();
		
		delete m_pMemFile;
		m_pMemFile = NULL;
	}

	if( m_pMemFileBuf )
	{
		free( m_pMemFileBuf );
		m_pMemFileBuf = NULL;
	}
}

bool CBigPac::BuildSetFileInfo()
{
	if( !m_pZip )	return false;

	ClearSetFileInfo();

	FileInfo *pFileInfo = NULL;
	CZipFileHeader *pHeader = NULL;
	char szTemp[MAX_PATH]="";

	CZipIndexesArray* pIdxArray = new CZipIndexesArray;
	m_iMaxIndex = m_pZip->GetCount();
	for( int iIndex=0; iIndex<m_iMaxIndex; iIndex++ )
	{
		pHeader = m_pZip->GetFileInfo( iIndex );
		if( !pHeader )
			return false;

		pFileInfo = new FileInfo;
		strcpy( szTemp, pHeader->GetFileName().c_str() );
		strlwr( szTemp );

		pFileInfo->FileName = szTemp;
		pFileInfo->HashCode = pFileInfo->CalcHashCode( szTemp );
		pFileInfo->Index		= iIndex;

#ifdef STATIC_PAC_API
		pFileInfo->ZipFileSize   = pHeader->m_uComprSize;
#endif
		FileInfoIter iter = m_setFileInfo.find(pFileInfo);	
		//if( CheckOverlapPath(pHeader->GetFileName().c_str()) ); //중복 경로 제거용
		if( iter == m_setFileInfo.end() )
		{
			m_setFileInfo.insert(pFileInfo);
		}
		else
		{
// 			pIdxArray->Add( (*iter)->Index ); //최초에 지울려면 여기서
// 			pIdxArray->Add( iIndex );
#ifdef STATIC_PAC_API
			// 사용하지 않은 중복 파일에 사이즈를 구한다.
			//m_GarbageSize += (*iter)->ZipFileSize;
			(*iter)->ZipFileSize   = pFileInfo->ZipFileSize;
#endif
			(*iter)->Index	= pFileInfo->Index;
			//m_setFileInfo.erase(iter);
			delete pFileInfo;
		}

		pFileInfo = NULL;
	}
	if(pIdxArray->GetSize() != 0)
	{
		pIdxArray->Sort(true);		//정렬 
		pIdxArray->RestoreIndex();  //중복 삭제
		m_pZip->RemoveFiles(*pIdxArray);
	}
	return true;
}

void CBigPac::ClearSetFileInfo()
{
	FileInfoIter iter = m_setFileInfo.begin();
	for( ; iter != m_setFileInfo.end() ; ++iter )
	{
		if( *iter != NULL )	
			delete *iter;	
	}
	m_setFileInfo.clear();
}

void CBigPac::DestroyZip()
{
	SAFEDELETE( m_pZip );
	m_iMaxIndex = 0;
#ifdef STATIC_PAC_API
	m_GarbageSize = 0;
	//m_SetIter = NULL;
#endif
}

void CBigPac::CloseBigPac()
{
	if(!m_pZip)	return;
	
	try
	{
		m_pZip->Close();
	}
	catch( CZipException e )
	{
		PacErrorReport::SetPacException( __FILE__, __LINE__, e.m_iCause );
		m_pZip->Close(CZipArchive::afAfterException);
	}
	catch(...)
	{
		m_pZip->Close(CZipArchive::afAfterException);
	}

	ClearSetFileInfo();
	ClearFileInfo();
	DestroyZip();
}


int CBigPac::GetFileIndex( IN const char* szFileName )
{
	if(!m_pZip) 
		return BIGPAC_ERROR_INDEX;

	char szTemp[MAX_PATH]="";

	strcpy(szTemp, szFileName);
	strlwr(szTemp);
	ReplaceChar(szTemp, MAX_PATH, '/', '\\');

	FileInfo findFileInfo;
	findFileInfo.FileName = szTemp;
	findFileInfo.HashCode = findFileInfo.CalcHashCode( szTemp );

 	FileInfoIter iter = m_setFileInfo.find(&findFileInfo);
 	if( iter == m_setFileInfo.end() )
 		return BIGPAC_ERROR_INDEX;

	return (*iter)->Index;
}

int CBigPac::GetFileSize( IN const int iIndex )
{
	if(!m_pZip) return BIGPAC_ERROR_SIZE;

	//UJ 131227, 최적화: INI 로딩에 CZipArchive::GetFileInfo가 2.5초 정도를 소모하고 있다(@i5-3770)
	// uninitialized data copy 때문이므로 const를 붙여서 data copy를 제거하였음
	//CZipFileHeader *pInfo = NULL;
	const CZipFileHeader *pInfo = NULL;
	try
	{
		pInfo = m_pZip->GetFileInfo( iIndex );
		if( !pInfo )
			return BIGPAC_ERROR_SIZE;
	}
	catch( CZipException e )
	{
		PacErrorReport::SetPacException( __FILE__, __LINE__, e.m_iCause );
		m_pZip->Close(CZipArchive::afAfterException);
		return BIGPAC_ERROR_SIZE;
	}
	catch (...)
	{
		m_pZip->Close(CZipArchive::afAfterException);
		return BIGPAC_ERROR_SIZE;
	}

	if( pInfo )
		return pInfo->m_uUncomprSize;

	return BIGPAC_ERROR_SIZE;
}

DWORD CBigPac::GetFileCRC( IN const int iIndex )
{
	if(!m_pZip) 
		return BIGPAC_ERROR_CRC;

	//UJ 131227, 최적화: INI 로딩에 CZipArchive::GetFileInfo가 2.5초 정도를 소모하고 있다(@i5-3770)
	// uninitialized data copy 때문이므로 const를 붙여서 data copy를 제거하였음
	//CZipFileHeader *pInfo = NULL;
	const CZipFileHeader *pInfo = NULL;
	try
	{
		pInfo = m_pZip->GetFileInfo( iIndex );
		if( !pInfo )
			return BIGPAC_ERROR_CRC;
	}
	catch( CZipException e )
	{
		PacErrorReport::SetPacException( __FILE__, __LINE__, e.m_iCause );
		m_pZip->Close(CZipArchive::afAfterException);
		return BIGPAC_ERROR_CRC;
	}
	catch (...)
	{
		m_pZip->Close(CZipArchive::afAfterException);
		return BIGPAC_ERROR_CRC;
	}

	if( pInfo )
		return pInfo->m_uCrc32;

	return BIGPAC_ERROR_CRC;
}

time_t CBigPac::GetFileTime( IN const int iIndex )
{
	if(!m_pZip) return BIGPAC_ERROR_TIME;

	//UJ 131227, 최적화: INI 로딩에 CZipArchive::GetFileInfo가 2.5초 정도를 소모하고 있다(@i5-3770)
	// uninitialized data copy 때문이므로 const를 붙여서 data copy를 제거하였음
	//CZipFileHeader *pInfo = NULL;
	const CZipFileHeader *pInfo = NULL;
	try
	{
		pInfo = m_pZip->GetFileInfo( iIndex );
		if( !pInfo )
			return BIGPAC_ERROR_TIME;
	}
	catch( CZipException e )
	{
		PacErrorReport::SetPacException( __FILE__, __LINE__, e.m_iCause );
		m_pZip->Close(CZipArchive::afAfterException);
		return BIGPAC_ERROR_TIME;
	}
	catch (...)
	{
		m_pZip->Close(CZipArchive::afAfterException);
		return BIGPAC_ERROR_TIME;
	}

	if( pInfo )
		return pInfo->GetTime();

	return BIGPAC_ERROR_TIME;
}

void CBigPac::GetFileComment( OUT char *szComment, IN int iCommentSize, IN const int iIndex )
{
	if(!m_pZip) return;

	//UJ 131227, 최적화: INI 로딩에 CZipArchive::GetFileInfo가 2.5초 정도를 소모하고 있다(@i5-3770)
	// uninitialized data copy 때문이므로 const를 붙여서 data copy를 제거하였음
	//CZipFileHeader *pInfo = NULL;
	const CZipFileHeader *pInfo = NULL;
	try
	{
		pInfo = m_pZip->GetFileInfo( iIndex );
		if( !pInfo )
			return;
	}
	catch( CZipException e )
	{
		PacErrorReport::SetPacException( __FILE__, __LINE__, e.m_iCause );
		m_pZip->Close(CZipArchive::afAfterException);
		return;
	}
	catch (...)
	{
		m_pZip->Close(CZipArchive::afAfterException);
		return;
	}

	if( pInfo )
	{
		StringCbCopy( szComment, iCommentSize, pInfo->GetComment().c_str() );
	}
}

bool CBigPac::GetBuffer(  IN const int iIndex, OUT BYTE* pData, IN const DWORD dwDataSize  )
{
	if( !m_pZip )
		return false;

	try
	{
		if( !m_pZip->OpenFile(iIndex) )
			return false;
	}
	catch( CZipException e )
	{
		PacErrorReport::SetPacException( __FILE__, __LINE__, e.m_iCause );
		return false;
	}
	catch (...)
	{
		m_pZip->Close( CZipArchive::afAfterException );
		return false;
	}

	try
	{
		if ( m_pZip->ReadFile(pData, dwDataSize) != dwDataSize )
		{
			m_pZip->CloseFile();
			return false;
		}
	}
	catch( CZipException e )
	{
		PacErrorReport::SetPacException( __FILE__, __LINE__, e.m_iCause );
		m_pZip->CloseFile( NULL, true );
		return false;
	}
	catch (...)
	{
		m_pZip->Close( CZipArchive::afAfterException );
		return false;
	}

	try
	{
		if( m_pZip->CloseFile() != 1)
			return false;
	}
	catch( CZipException e )
	{
		PacErrorReport::SetPacException( __FILE__, __LINE__, e.m_iCause );
		m_pZip->Close( CZipArchive::afAfterException );
		return false;
	}
	catch (...)
	{
		m_pZip->Close( CZipArchive::afAfterException );
		return false;
	}

	return true;	
}

bool CBigPac::IsAllocBigPac() const
{
	if( m_pZip )
		return true;

	return false;
}

bool CBigPac::IsMemoryLoading() const
{
	if( m_pMemFile )
		return true;

	return false;
}

void CBigPac::DeleteFileInfo( DeleteFileList *pDeleteList )
{
	if( !pDeleteList ) return;
	if( pDeleteList->m_vDeleteFileInfo.empty() ) return;

	for(vFileInfo::const_iterator it = pDeleteList->m_vDeleteFileInfo.begin(); it != pDeleteList->m_vDeleteFileInfo.end(); ++it)
	{
		FileInfoIter iter = m_setFileInfo.find(*it);
		if( iter != m_setFileInfo.end() )
		{
			delete *iter;
			m_setFileInfo.erase( iter );
		}
	}
}

bool CBigPac::OpenBigPacAll( IN const char* szPath, IN bool bReadOnly /*= true */, IN const char *szPassword /*= "" */ )
{
	CloseBigPac();

	try
	{
		m_pZip = new CZipArchive;

		if( bReadOnly )
			m_pZip->Open(_T(szPath), CZipArchive::zipOpenReadOnly );
		else
			m_pZip->Open(_T(szPath), CZipArchive::zipOpen );

		if( strcmp( szPassword , "") != 0)
		{
			if( !m_pZip->SetPassword(_T(szPassword) ) )
			{
				m_pZip->Close();
				DestroyZip();
				return false;
			}		
		}


		if( !BuildFileInfo() )
		{
			m_pZip->Close();
			ClearFileInfo();
			DestroyZip();
			return false;
		}

		else
		{
			m_iMaxIndex = m_pZip->GetCount();
		}
	}
	catch( CZipException e )
	{
		PacErrorReport::SetPacException( __FILE__, __LINE__, e.m_iCause );

		m_pZip->Close( CZipArchive::afAfterException );
		ClearFileInfo();
		DestroyZip();
		return false;
	}
	catch(...)
	{
		m_pZip->Close( CZipArchive::afAfterException );
		ClearFileInfo();
		DestroyZip();
		return false;
	}

	return true;
}

void CBigPac::ClearFileInfo()
{
	for(size_t i = 0; i < m_vecFileInfo.size(); ++i)
	{
		SAFEDELETE(m_vecFileInfo[i]);
	}
	m_vecFileInfo.clear();
}

int CBigPac::GetFileIndexVec( IN const char* szFileName )
{
	if(!m_pZip) 
		return BIGPAC_ERROR_INDEX;

	char szTemp[MAX_PATH]="";

	strcpy(szTemp, szFileName);
	strlwr(szTemp);
	ReplaceChar(szTemp, MAX_PATH, '/', '\\');

	FileInfo findFileInfo;
	findFileInfo.FileName = szTemp;
	findFileInfo.HashCode = findFileInfo.CalcHashCode( szTemp );

	for(auto it = m_vecFileInfo.begin(); it != m_vecFileInfo.end(); ++it)
	{
		if((*it)->FileName == findFileInfo.FileName && (*it)->HashCode == findFileInfo.HashCode)
			return (*it)->Index;
	}

	return BIGPAC_ERROR_INDEX;
}

CZipFileHeader* CBigPac::AddFile(IN const char* szFilePath ,IN const char* szFileName)
{//iopmanager
	if(!m_pZip) return false;

	WORD aindex = -1;

	try
	{
		if(!m_pZip->AddNewFile(szFilePath, szFileName))
			return NULL;

		aindex = m_pZip->FindFile(szFileName);
	}
	catch( CZipException e )
	{
		PacErrorReport::SetPacException( __FILE__, __LINE__, e.m_iCause );

		m_pZip->Close(CZipArchive::afAfterException);
		return NULL;
	}
	catch (...)
	{
		m_pZip->Close(CZipArchive::afAfterException);
		return NULL;
	}

	return m_pZip->GetFileInfo(aindex);
}

void CBigPac::DeleteFileInZip( IN string strPath, IN DWORD iCrc )
{
#ifdef _DEBUG
	std::ofstream OverlapText;
	OverlapText.open( "Overlap.txt", std::ios_base::out | std::ios_base::app );
	OverlapText<< "\n [CHECK FILE]NAME : " << strPath << " Crc : "<< iCrc << "\n";
#endif
	if(!m_pZip) 
		return;
	CZipFileHeader	*pHeader	= NULL;
	char szName[MAX_PATH] ="";
	char szPath[MAX_PATH] ="";
	bool bLatestPatch = false;
	CZipIndexesArray* pIdxArray = new CZipIndexesArray; //지울애들
	for( int iIndex=0; iIndex<m_pZip->GetCount(); iIndex++ ) //끝까지 검사
	{
		pHeader = m_pZip->GetFileInfo( iIndex );
		if( !pHeader )	
			continue;;
		strcpy( szName, pHeader->GetFileName().c_str() );
		strcpy( szPath, strPath.c_str() );
		strlwr( szPath );
		ReplaceChar(szPath, MAX_PATH, '/', '\\');
		if( stricmp( szPath, szName) == 0 ) //대소문자 구분 X
		{
			DWORD dwCRCInPac = GetFileCRC(iIndex);
			if( dwCRCInPac != iCrc ) //old file
			{
#ifdef _DEBUG
				OverlapText << "Add Different Crc : " << dwCRCInPac << "\n";
#endif
				pIdxArray->Add(iIndex);
			}
			if( dwCRCInPac == iCrc )
			{
				if(!bLatestPatch)
				{
					bLatestPatch = true;
					continue;
				}
				if( bLatestPatch )
					pIdxArray->Add(iIndex);
#ifdef _DEBUG
				OverlapText << "Delete Lastest Patch : " << dwCRCInPac << "\n";
#endif
			}
		}
	}
#ifdef _DEBUG
	OverlapText << "========================================================\n";
	OverlapText.close();
#endif
	if(pIdxArray->GetSize() != 0) //삭제
	{
		pIdxArray->Sort(true);		//정렬 
		pIdxArray->RestoreIndex();  //중복 삭제
		m_pZip->RemoveFiles(*pIdxArray);
	}
	return;
}


void CBigPac::DeleteFiles( const vector<int>& vecIndex )
{
	if(!m_pZip) 
		return;
	CZipIndexesArray* pIdxArray = new CZipIndexesArray;
	CZipFileHeader	*pHeader	= NULL;

	for(size_t i = 0 ; i < vecIndex.size(); ++i)
	{
		pHeader = m_pZip->GetFileInfo(vecIndex[i]);
		if( !pHeader )
			continue;
		pIdxArray->Add(vecIndex[i]);
	}
	m_pZip->RemoveFiles(*pIdxArray);
}


#ifdef STATIC_PAC_API
bool CBigPac::InitSetIter()
{
	if( m_setFileInfo.empty() )
		return false;

	m_SetIter = m_setFileInfo.begin();

	return true;
}

bool CBigPac::GetCurFileInfo( OUT FileInfo &kFileInfo )
{
// 	if(m_SetIter == NULL)
// 		return false;

	if( m_SetIter == m_setFileInfo.end() )
		return false;

	FileInfo *pInfo = (*m_SetIter);
	if(pInfo == NULL) return false;

	kFileInfo = *pInfo;

	return true;
}

bool CBigPac::Extract( IN const char* szExtractPath )
{
	if(!m_pZip) return false;
	
// 	if(m_SetIter == NULL)
// 		return false;

	if( m_SetIter == m_setFileInfo.end() )
		return false;

	FileInfo *pInfo = (*m_SetIter);
	if(pInfo == NULL) return false;

	try
	{
		if(!m_pZip->ExtractFile(pInfo->Index, _T(szExtractPath)))
			return false;
	}
	catch( CZipException e )
	{
		PacErrorReport::SetPacException( __FILE__, __LINE__, e.m_iCause );

		m_pZip->Close(CZipArchive::afAfterException);
		return false;
	}
	catch (...)
	{
		m_pZip->Close(CZipArchive::afAfterException);
		return false;
	}

	return true;
}

void CBigPac::NextSetIter()
{
	m_SetIter++;
}

int CBigPac::GetSetSize()
{
	return m_setFileInfo.size();
}

bool CBigPac::Extract( IN const int iIndex, IN const char* szExtractPath )
{
	if(!m_pZip) return false;
	if( iIndex >= m_iMaxIndex ) return false;
	if( iIndex < 0 ) return false;

	try
	{
		if(!m_pZip->ExtractFile(iIndex, _T(szExtractPath)))
			return false;
	}
	catch( CZipException e )
	{
		PacErrorReport::SetPacException( __FILE__, __LINE__, e.m_iCause );

		m_pZip->Close(CZipArchive::afAfterException);
		return false;
	}
	catch (...)
	{
		m_pZip->Close(CZipArchive::afAfterException);
		return false;
	}

	return true;
}

bool CBigPac::TestFile( IN const int iIndex )
{
	if(!m_pZip) return false;
	if( iIndex >= m_iMaxIndex ) return false;
	if( iIndex < 0 ) return false;

	try
	{
		if( !m_pZip->TestFile( iIndex ) )
			return false;
	}
	catch( CZipException e )
	{
		PacErrorReport::SetPacException( __FILE__, __LINE__, e.m_iCause );

		m_pZip->Close(CZipArchive::afAfterException);
		return false;
	}
	catch (...)
	{
		m_pZip->Close(CZipArchive::afAfterException);
		return false;
	}

	return true;
}

int CBigPac::GetMaxIndex() const
{
	return m_iMaxIndex;
}

bool CBigPac::GetFileName( IN const int iIndex, OUT char* szName, IN const int iNameSize )
{
	if(!m_pZip) return false;
	if( iIndex >= m_iMaxIndex ) return false;
	if( iIndex < 0 ) return false;

	try
	{
		//UJ 131227, 최적화: INI 로딩에 CZipArchive::GetFileInfo가 2.5초 정도를 소모하고 있다(@i5-3770)
		// uninitialized data copy 때문이므로 const를 붙여서 data copy를 제거하였음
		CZipFileHeader *pInfo = m_pZip->GetFileInfo( iIndex );
		//const CZipFileHeader *pInfo = m_pZip->GetFileInfo( iIndex );
		if( !pInfo )	return false;

		if( pInfo->IsDirectory() )
		{
			strcpy(szName, "This is a Directory!!!!");
			return true;
		}

		if(iNameSize > (int)pInfo->GetFileName().length() )
			strcpy(szName, pInfo->GetFileName().c_str());
		else
			return false;
	}
	catch( CZipException e )
	{
		PacErrorReport::SetPacException( __FILE__, __LINE__, e.m_iCause );

		m_pZip->Close(CZipArchive::afAfterException);
		return false;
	}
	catch (...)
	{
		m_pZip->Close(CZipArchive::afAfterException);
		return false;		
	}

	return true;
}

bool CBigPac::AddBigPac( IN const char* szPiecePacName, IN const char* szInPacName, IN const time_t lasWriteTime, IN const char *szPassword, IN const char *szComment )
{
	if(!m_pZip) return false;

	CZipArchive zip;
	char *pData = NULL;
	try
	{
		// piece zip open
		if( !zip.Open(_T(szPiecePacName), CZipArchive::zipOpenReadOnly) )
			return false;

		if( strcmp( szPassword, "" ) != 0 )
		{
			if(!zip.SetPassword( _T(szPassword) ))
				return false;
		}

		int iMaxIndex = zip.GetCount();
		if(!iMaxIndex) 
			return false;

		if(!zip.OpenFile(0))
			return false;

		//UJ 131227, 최적화: INI 로딩에 CZipArchive::GetFileInfo가 2.5초 정도를 소모하고 있다(@i5-3770)
		// uninitialized data copy 때문이므로 const를 붙여서 data copy를 제거하였음
		//CZipFileHeader *pInfo = zip.GetFileInfo( 0 );
		const CZipFileHeader *pInfo = zip.GetFileInfo( 0 );
		if( !pInfo )	return false;

		DWORD uSize = pInfo->m_uUncomprSize;

		if(!uSize)
			return false;

		pData = new char[uSize];

		if (zip.ReadFile(pData, uSize) != uSize)
		{
			SAFEDELETEARRAY( pData );
			return false;
		}

		if( zip.CloseFile() != 1)
		{
			SAFEDELETEARRAY( pData );
			return false;
		}

		zip.Close();

		// new file add
		CZipFileHeader NewFileH;
		NewFileH.SetFileName(_T(szInPacName));
		NewFileH.SetTime(lasWriteTime);

		if( strcmp( szComment, "" ) != 0 )
			NewFileH.SetComment( _T(szComment) );

		// 0(최저) - 9(최고) 디폴트 6(-1)
		if(!m_pZip->OpenNewFile(NewFileH,9))
		{
			SAFEDELETEARRAY( pData );
			return false;
		}

		if(!m_pZip->WriteNewFile(pData, uSize))
		{
			SAFEDELETEARRAY( pData );
			return false;
		}

		if(!m_pZip->CloseNewFile())
		{
			SAFEDELETEARRAY( pData );
			return false;
		}

		SAFEDELETEARRAY( pData );
	}
	catch( CZipException e )
	{
		PacErrorReport::SetPacException( __FILE__, __LINE__, e.m_iCause );

		SAFEDELETEARRAY( pData );

		zip.Close(CZipArchive::afAfterException);
		m_pZip->Close(CZipArchive::afAfterException);
		return false;
	}
	catch (...)
	{
		SAFEDELETEARRAY( pData );

		zip.Close(CZipArchive::afAfterException);
		m_pZip->Close(CZipArchive::afAfterException);
		return false;
	}

	return true;
}

bool CBigPac::AddBigPacData( IN const char* szInPacName, IN const BYTE * pData, IN const DWORD uSize, IN const char *szComment )
{
	if(!m_pZip) return false;

	try
	{
		// new file add
		CZipFileHeader NewFileH;
		NewFileH.SetFileName(_T(szInPacName));
		if( strcmp( szComment, "" ) != 0 )
			NewFileH.SetComment( _T( szComment ));

		// 0(최저) - 9(최고) 디폴트 6(-1)
		if(!m_pZip->OpenNewFile(NewFileH,9))
		{
			return false;
		}

		if(!m_pZip->WriteNewFile(pData, uSize))
		{
			return false;
		}

		if(!m_pZip->CloseNewFile())
		{
			return false;
		}
	}
	catch( CZipException e )
	{
		PacErrorReport::SetPacException( __FILE__, __LINE__, e.m_iCause );

		m_pZip->Close(CZipArchive::afAfterException);
		return false;
	}
	catch (...)
	{
		m_pZip->Close(CZipArchive::afAfterException);
		return false;
	}

	return true;
}

int CBigPac::GetFileComprSize(IN const char* szFileName)
{
	if(!m_pZip) return BIGPAC_ERROR_SIZE;

	char szTemp[MAX_PATH]="";

	strcpy(szTemp, szFileName);
	strlwr(szTemp);
	ReplaceChar(szTemp, MAX_PATH, '/', '\\');

	FileInfo findFileInfo;
	findFileInfo.FileName = szTemp;
	findFileInfo.HashCode = findFileInfo.CalcHashCode( szTemp );

	FileInfoIter iter = m_setFileInfo.find(&findFileInfo);
	if( iter == m_setFileInfo.end() )
		return BIGPAC_ERROR_SIZE;

	return (int)(*iter)->ZipFileSize;
}

void CBigPac::Flush()
{
	if( !m_pZip ) return;

	try
	{
		m_pZip->Flush();
	}
	catch( CZipException e )
	{
		PacErrorReport::SetPacException( __FILE__, __LINE__, e.m_iCause );
		m_pZip->Close(CZipArchive::afAfterException);
		return;
	}
	catch (...)
	{
		m_pZip->Close(CZipArchive::afAfterException);
		return;
	}
}
void CBigPac::RemoveBigPacInFile( IN int iIndex )
{
	m_pZip->RemoveFile(iIndex);
}

// int CBigPac::GetFileIndexInZip( IN const char* szFileName )
// {
// 	if(!m_pZip) 
// 		return BIGPAC_ERROR_INDEX;
// 
// 	char szKey[MAX_PATH] = "";
// 	char szName[MAX_PATH] ="";
// 	strcpy(szKey, szFileName);
// 	strlwr(szKey);
// 	ReplaceChar(szKey, MAX_PATH, '/', '\\');
// 
// 	CZipFileHeader	*pHeader	= NULL;
// 	for( int iIndex=0; iIndex<m_pZip->GetCount(); iIndex++ )
// 	{
// 		pHeader = m_pZip->GetFileInfo( iIndex );
// 		if( !pHeader )	return false;
// 		strcpy( szName, pHeader->GetFileName().c_str() );
// 		//if(strcmp(szKey, szName) ==0)
// 		if(stricmp(szKey, szName) == 0) //대소문자 구분없이 비교
// 			return iIndex;
// 	}
// 	return BIGPAC_ERROR_INDEX;
// }
// 
// DWORD CBigPac::GetFileCRCInzZip( int iIndex )
// {
// 	CZipFileHeader	*pHeader	= NULL;
// 	pHeader = m_pZip->GetFileInfo( iIndex );
// 	return pHeader->m_uCrc32;
// }

bool CBigPac::CheckOverlapPath( CString const strPath )
{
	CString strToken[5];
	int iStartIndex = 0;
	int iValue = 0;
	for(int i = 0 ; i < strPath.GetLength() ; ++i)
	{
		if( '\\' == strPath.GetAt(i))
		{
			strToken[iValue] = strPath.Mid(iStartIndex , i-iStartIndex);
			iStartIndex = i+1;
			iValue += 1;
		}
		if(iValue >= 2) //경로를 2개이상 찾으면
		{
			if( strToken[iValue-2] == strToken[iValue-1] ) //바로 앞의 경로만 비교한다.
				return true;
		}
	}
	return false;
}

bool CBigPac::BuildFileInfo()
{
	if( !m_pZip )	return false;

	ClearFileInfo();

	FileInfo *pFileInfo = NULL;
	CZipFileHeader *pHeader = NULL;
	char szTemp[MAX_PATH]="";

	CZipIndexesArray* pIdxArray = new CZipIndexesArray;
	m_iMaxIndex = m_pZip->GetCount();
	m_vecFileInfo.reserve(m_iMaxIndex);
	for( int iIndex=0; iIndex<m_iMaxIndex; iIndex++ )
	{
		pHeader = m_pZip->GetFileInfo( iIndex );
		if( !pHeader )
			return false;

		pFileInfo = new FileInfo;
		strcpy( szTemp, pHeader->GetFileName().c_str() );
		strlwr( szTemp );

		pFileInfo->FileName = szTemp;
		pFileInfo->HashCode = pFileInfo->CalcHashCode( szTemp );
		pFileInfo->Index		= iIndex;

#ifdef STATIC_PAC_API
		pFileInfo->ZipFileSize   = pHeader->m_uComprSize;
#endif
		m_vecFileInfo.push_back(pFileInfo);

		pFileInfo = NULL;
	}
	if(pIdxArray->GetSize() != 0)
	{
		pIdxArray->Sort(true);		//정렬 
		pIdxArray->RestoreIndex();  //중복 삭제
		m_pZip->RemoveFiles(*pIdxArray);
	}
	return true;
}

#endif // STATIC_PAC_API
