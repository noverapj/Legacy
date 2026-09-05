// PiecePac.cpp: implementation of the CPiecePac class.
//
//////////////////////////////////////////////////////////////////////

#include <string>

#include "stdafx.h"

#include "ZipArchive/ZipArchive.h"

#include "PiecePac.h"
#include "BigPac.h"
#include "PacErrorReport.h"
#include <strsafe.h>

using namespace std;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPiecePac::CPiecePac()
{
#ifndef STATIC_PAC_API
	m_pMemFile = NULL;
#endif
}

CPiecePac::~CPiecePac()
{
#ifndef STATIC_PAC_API
	SAFEDELETE( m_pMemFile );
#endif
}

#ifdef PATCH_PAC_API
bool CPiecePac::PacPieceFile(  IN const char* szFileName, IN const char* szPassword /*= ""*/, IN const char* szComment /*= ""*/  )
{
	CZipArchive zip;
	try
	{
		string lastZipFileName;
		lastZipFileName = szFileName;
		lastZipFileName += ".iop";

		zip.Open(_T(lastZipFileName.c_str()), CZipArchive::zipCreate);   

		if( strcmp( szPassword , "") != 0)
		{
			if( !zip.SetPassword(_T(szPassword) ) )
			{
				zip.Close();
				return false;
			}
		}

		if(!zip.AddNewFile(_T(szFileName), -1, false))
		{
			zip.Close();  
			return false;
		}

		if( strcmp( szComment , "" ) != 0 )
		{
			if( !zip.SetFileComment( 0, _T(szComment) ) )
			{
				zip.Close();
				return false;
			}
		}
		
		zip.Close();  
	}
	catch( CZipException e )
	{
		PacErrorReport::SetPacException( __FILE__, __LINE__, e.m_iCause );

		zip.Close(CZipArchive::afAfterException);
		return false;
	}
	catch (...)
	{
		zip.Close(CZipArchive::afAfterException);
		return false;
	}
	return true;
}

int CPiecePac::GetPieceFileSize( IN const char *szFileName, IN const char* szPassword /*= ""*/ )
{
	CZipArchive zip;
	CZipFileHeader info;
	try
	{
		zip.Open(_T(szFileName));

		if( strcmp( szPassword , "") != 0)
		{
			if( !zip.SetPassword(_T(szPassword) ) )
			{
				zip.Close();
				return BIGPAC_ERROR_SIZE;
			}
		}

		if(!zip.GetFileInfo(info, 0))
		{
			zip.Close();
			return BIGPAC_ERROR_SIZE;
		}

		zip.Close();    
	}
	catch( CZipException e )
	{
		PacErrorReport::SetPacException( __FILE__, __LINE__, e.m_iCause );

		zip.Close(CZipArchive::afAfterException);
		return BIGPAC_ERROR_SIZE;
	}
	catch (...)
	{
		zip.Close(CZipArchive::afAfterException);
		return BIGPAC_ERROR_SIZE;
	}

	return info.m_uUncomprSize;	
}

DWORD CPiecePac::GetPieceFileCRC( IN const char *szFileName, IN const char *szPassword /*= "" */ )
{
	CZipArchive zip;
	CZipFileHeader info;
	try
	{
		zip.Open(_T(szFileName));
		if( strcmp( szPassword , "") != 0)
		{
			if( !zip.SetPassword(_T(szPassword) ) )
			{
				zip.Close();
				return BIGPAC_ERROR_CRC;
			}
		}

		if(!zip.GetFileInfo(info, 0))
		{
			zip.Close();
			return BIGPAC_ERROR_CRC;
		}

		zip.Close();    
	}
	catch( CZipException e )
	{
		PacErrorReport::SetPacException( __FILE__, __LINE__, e.m_iCause );

		zip.Close(CZipArchive::afAfterException);
		return BIGPAC_ERROR_CRC;
	}
	catch (...)
	{
		zip.Close(CZipArchive::afAfterException);
		return BIGPAC_ERROR_CRC;
	}

	return info.m_uCrc32;	
}

CPacUtil::ReturnValue CPiecePac::UnPacPieceFileWithPassword( IN const char* szFileName, IN const char* szPassword /*= ""*/  )
{
	CZipArchive zip;
	try
	{
		char szDir[MAX_PATH]="";
		char szPath[MAX_PATH]="";
		_splitpath(szFileName, szDir, szPath, NULL, NULL);
		char szFullPath[MAX_PATH]="";
		StringCbPrintf(szFullPath, sizeof(szFullPath),  "%s%s",szDir, szPath);

		if(!zip.Open(_T(szFileName)))
			return CPacUtil::RV_ERROR_1;

		if( strcmp( szPassword , "") != 0) 
			zip.SetPassword( _T(szPassword) ); // 비밀번호가 없는 경우도 있으므로 에러 체크를 하지 않음.

		if(!zip.ExtractFile(0, _T(szFullPath)))
		{
			zip.Close();    
			return CPacUtil::RV_ERROR_3;
		}

		zip.Close();    
	}
	catch( CZipException e )
	{
		PacErrorReport::SetPacException( __FILE__, __LINE__, e.m_iCause );

		zip.Close(CZipArchive::afAfterException);
		return CPacUtil::RV_ERROR_4;
	}
	catch (...)
	{
		zip.Close(CZipArchive::afAfterException);
		return CPacUtil::RV_ERROR_5;
	}
	return CPacUtil::RV_OK;	
}

#endif

CPacUtil::ReturnValue CPiecePac::UnPacPieceFile( IN const char* szFileName )
{
	CZipArchive zip;
	try
	{
		char szDir[MAX_PATH]="";
		char szPath[MAX_PATH]="";
		_splitpath(szFileName, szDir, szPath, NULL, NULL);
		char szFullPath[MAX_PATH]="";
		StringCbPrintf(szFullPath, sizeof(szFullPath),  "%s%s",szDir, szPath);

		if(!zip.Open(_T(szFileName)))
			return CPacUtil::RV_ERROR_1;

		if(!zip.ExtractFile(0, _T(szFullPath)))
		{
			zip.Close();    
			return CPacUtil::RV_ERROR_3;
		}
		
		zip.Close();    
	}
	catch( CZipException e )
	{
		PacErrorReport::SetPacException( __FILE__, __LINE__, e.m_iCause );

		zip.Close(CZipArchive::afAfterException);
		return CPacUtil::RV_ERROR_4;
	}
	catch (...)
	{
		zip.Close(CZipArchive::afAfterException);
		return CPacUtil::RV_ERROR_5;
	}
	return CPacUtil::RV_OK;	
}

void CPiecePac::GetPieceFileComment( OUT char *szComment, IN int iCommentSize, IN const char *szFileName )
{
	CZipArchive zip;
	try
	{
		if(!zip.Open(_T(szFileName)))
			return;

		StringCbCopy( szComment, iCommentSize, zip.GetFileComment( 0 ).c_str() );

		zip.Close();    
	}
	catch( CZipException e )
	{
		PacErrorReport::SetPacException( __FILE__, __LINE__, e.m_iCause );

		zip.Close(CZipArchive::afAfterException);
	}
	catch (...)
	{
		zip.Close(CZipArchive::afAfterException);
	}
}

#ifndef STATIC_PAC_API

void CPiecePac::NewMemFile()
{
	SAFEDELETE( m_pMemFile );
	m_pMemFile = new CZipMemFile;
}

void CPiecePac::DeleteMemFile()
{
	SAFEDELETE( m_pMemFile );
}

bool CPiecePac::PacPieceFileInMem( IN const char* szFileName )
{
	if( !m_pMemFile )
		return false;

	CZipArchive zip;
	try
	{
		zip.Open(*m_pMemFile, CZipArchive::zipCreate);   

		// 0(최저) - 9(최고) 디폴트 6(-1)
		if(!zip.AddNewFile(_T(szFileName), 9, false))
		{
			zip.Close();  
			return false;
		}

		zip.Close();  
	}
	catch( CZipException e )
	{
		PacErrorReport::SetPacException( __FILE__, __LINE__, e.m_iCause );
		zip.Close(CZipArchive::afAfterException);
		return false;
	}
	catch (...)
	{
		zip.Close(CZipArchive::afAfterException);
		return false;
	}

	return true;
}

DWORD CPiecePac::GetMemFileSize()
{
	if( !m_pMemFile )
		return 0;

	return (DWORD) m_pMemFile->GetLength();
}

bool CPiecePac::GetPacPieceFileData( OUT BYTE* pPacData, IN DWORD dwPacSize )
{
	if( !m_pMemFile )
		return false;

	CZipArchive zip;
	try
	{
		m_pMemFile->SeekToBegin();
		if( m_pMemFile->Read( pPacData, dwPacSize ) != dwPacSize )
			return false;
	}
	catch( CZipException e )
	{
		PacErrorReport::SetPacException( __FILE__, __LINE__, e.m_iCause );
		return false;
	}
	catch (...)
	{
		return false;
	}

	return true;
}


bool CPiecePac::UnPacPieceFileByMem( IN const char* szFileName, IN BYTE* pPacData , IN DWORD dwDataSize )
{
	CZipArchive zip;
	CZipMemFile mf;
	
	try
	{
		mf.Attach( pPacData, dwDataSize );
	
		char szDir[MAX_PATH]="";
		char szPath[MAX_PATH]="";
		char szName[MAX_PATH]="";
		char szExt[MAX_PATH]="";
		_splitpath(szFileName, szDir, szPath, szName, szExt);
		char szFullPath[MAX_PATH]="";
		StringCbPrintf(szFullPath, sizeof(szFullPath), "%s%s",szDir, szPath);
		char szNameExt[MAX_PATH]="";
		StringCbPrintf( szNameExt, sizeof(szNameExt), "%s%s", szName, szExt );

		if(!zip.Open(mf))
			return false;

		if(!zip.ExtractFile(0, _T(szFullPath), false, _T(szNameExt) ) )
		{
			zip.Close();  
			return false;
		}

		zip.Close();    
	}
	catch( CZipException e )
	{
		PacErrorReport::SetPacException( __FILE__, __LINE__, e.m_iCause );
		zip.Close(CZipArchive::afAfterException);
		return false;
	}
	catch (...)
	{
		zip.Close(CZipArchive::afAfterException);
		return false;
	}

	return true;
}

#endif