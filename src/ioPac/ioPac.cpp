// ioPac.cpp: implementation of the ioPac class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ioPac.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////



//-------------------------------------------------------------------------------------------------

ioPac *ioPac::sg_Instance = NULL;

ioPac::ioPac()
{	
	m_vBigPacInfo.reserve(10);
	m_iMaxSize = 0;
	m_vDeleteFileList.reserve(10);
}

ioPac::~ioPac()
{
	Clear();
	RemoveAllDeleteList();
}

ioPac &ioPac::GetInstance()
{
	if(sg_Instance == NULL)
		sg_Instance = new ioPac;
	return *sg_Instance;
}

void ioPac::ReleaseInstance()
{
	if(sg_Instance)
		delete sg_Instance;
	sg_Instance = NULL;
}

void ioPac::Clear()
{
	vBigPacInfo::iterator iter = m_vBigPacInfo.begin();
	for( ; iter!=m_vBigPacInfo.end() ; ++iter )
	{
		CBigPac *pBigPac = iter->m_pBigPac;
		if( pBigPac )
		{
			pBigPac->CloseBigPac();
			delete pBigPac;
		}
	}
	m_vBigPacInfo.clear();
	m_iMaxSize = m_vBigPacInfo.size();
}

bool ioPac::Add( const char* szFilePath )
{
	CBigPac *pBigPac = GetPac( szFilePath );
	if( pBigPac )	
		return true;

	pBigPac = new CBigPac;
	BigPacInfo kInfo;

	kInfo.m_FileInfo.FileName = szFilePath;
	kInfo.m_FileInfo.HashCode = kInfo.m_FileInfo.CalcHashCode( szFilePath );
	kInfo.m_pBigPac = pBigPac;
	m_vBigPacInfo.push_back( kInfo );

	m_iMaxSize = m_vBigPacInfo.size();
	return true;
}

bool ioPac::Remove( const char* szFilePath )
{
	vBigPacInfo::iterator iter = m_vBigPacInfo.begin();
	for( ; iter!=m_vBigPacInfo.end() ; ++iter )
	{
		if( iter->m_FileInfo.HashCode == iter->m_FileInfo.CalcHashCode( szFilePath ) )
		{
			if( strcmp( iter->m_FileInfo.FileName.c_str(), szFilePath ) == 0 )
			{
				CBigPac *pBigPac = iter->m_pBigPac;
				m_vBigPacInfo.erase( iter );

				if( pBigPac )
				{
					pBigPac->CloseBigPac();
					delete pBigPac;
				}
				m_iMaxSize = m_vBigPacInfo.size();
				return true;
			}
		}
	}

	return false;
}

CBigPac * ioPac::GetPac( const char* szFilePath ) const
{
	vBigPacInfo::const_iterator iter = m_vBigPacInfo.begin();
	for( ; iter!=m_vBigPacInfo.end() ; ++iter )
	{
		if( iter->m_FileInfo.HashCode == iter->m_FileInfo.CalcHashCode( szFilePath ) )
		{
			if( strcmp( iter->m_FileInfo.FileName.c_str(), szFilePath ) == 0 )
			{
				return iter->m_pBigPac;
			}
		}
	}

	return NULL;
}

CBigPac * ioPac::GetPacArray( int iArray ) const
{
	if(!COMPARE( iArray, 0, m_iMaxSize)) return NULL;

	return m_vBigPacInfo[iArray].m_pBigPac;
}

CBigPac * ioPac::GetPacHasFile( const char *szFileName, int &iFileIndex ) const
{
	vBigPacInfo::const_iterator iter = m_vBigPacInfo.begin();
	for( ; iter!=m_vBigPacInfo.end() ; ++iter )
	{
		if( iter->m_pBigPac )
		{
			iFileIndex = iter->m_pBigPac->GetFileIndex( szFileName );
			if( iFileIndex != BIGPAC_ERROR_INDEX )
				return iter->m_pBigPac;
		}
	}

	return NULL;
}

void ioPac::AddDeleteList( const char *szToken )
{
	RemoveDeleteList( szToken );

	char szLwr[MAX_PATH]="";
	strcpy( szLwr, szToken );
	strlwr( szLwr );

	DeleteFileList kList;
	kList.m_szToken = szLwr;
	kList.m_vDeleteFileInfo.clear();

	m_vDeleteFileList.push_back( kList );
}

void ioPac::LoadDeleteList( const char *szFilePath )
{
	if( m_vDeleteFileList.empty() )
		return;

	FILE *fp=NULL;
	char buffer[MAX_PATH]="";
	char szDeleteFileName[MAX_PATH]="";
	fp = fopen(szFilePath,"rt");
	if(fp == NULL)
		return ;
	
	do
	{
		ZeroMemory(buffer, MAX_PATH);
		fgets(buffer, MAX_PATH, fp);
		int bufferSize = strlen(buffer);
		if( bufferSize != 0)
		{
			if(buffer[bufferSize-1] == '\n')
				buffer[bufferSize-1] = 0;
		}

		int     len = ::strlen(buffer);
		int     iCnt = 0;
		memset(szDeleteFileName, 0, sizeof(szDeleteFileName) );

		for(int i = 0;i < len;i++)
		{
			if(buffer[i] =='[' || buffer[i] == ']') continue;
			if(iCnt >= MAX_PATH) break;
			szDeleteFileName[iCnt++] = buffer[i];
		}

		if(  strcmp( szDeleteFileName, "" ) == 0 )
			continue;

		char szTemp[MAX_PATH]="";

		strcpy(szTemp, szDeleteFileName);
		strlwr(szTemp);
		ReplaceChar(szTemp, MAX_PATH, '/', '\\');

		bool bSetList = false;
		// map.iop 팩 파일 검사 현재는 _map만 존재
		for(vDeleteFileList::iterator it = m_vDeleteFileList.begin(); it != m_vDeleteFileList.end(); ++it)
		{
			if( it->m_szToken.length() <= 0 )
				continue;

			if( strncmp( szTemp, it->m_szToken.c_str(), it->m_szToken.length() ) == 0 )
			{
				FileInfo *pFileInfo = new FileInfo;
				if( !pFileInfo )
					continue;
				pFileInfo->FileName = szTemp;
				pFileInfo->FileName.erase( 0, it->m_szToken.length() + 1); // _map\ 삭제
				pFileInfo->HashCode = pFileInfo->CalcHashCode( pFileInfo->FileName.c_str() );
				it->m_vDeleteFileInfo.push_back( pFileInfo );
				bSetList = true;
				break;
			}
		}

		if( bSetList )
			continue;

		// rs.iop 검사 구분자는 ""
		for(vDeleteFileList::iterator it = m_vDeleteFileList.begin(); it != m_vDeleteFileList.end(); ++it)
		{
			if( it->m_szToken.length() > 0 )
				continue;

			if( it->m_szToken == "" )
			{
				FileInfo *pFileInfo = new FileInfo;
				if( !pFileInfo )
					continue;
				pFileInfo->FileName = szTemp;
				pFileInfo->HashCode = pFileInfo->CalcHashCode( pFileInfo->FileName.c_str() );
				it->m_vDeleteFileInfo.push_back( pFileInfo );
				break;
			}
		}

	}while(!feof(fp));

	fclose(fp);
}

void ioPac::RemoveDeleteList( const char *szToken )
{
	char szLwr[MAX_PATH]="";
	strcpy( szLwr, szToken );
	strlwr( szLwr );

	for(vDeleteFileList::iterator it = m_vDeleteFileList.begin(); it != m_vDeleteFileList.end(); ++it)
	{
	    if( it->m_szToken == szLwr )
		{
			for(vFileInfo::iterator itChild = it->m_vDeleteFileInfo.begin(); itChild != it->m_vDeleteFileInfo.end(); ++itChild)
				delete *itChild;   

			it->m_vDeleteFileInfo.clear();
			m_vDeleteFileList.erase(it);
			return;
		}
	}
}

DeleteFileList *ioPac::GetDeleteList( const char *szToken )
{
	char szLwr[MAX_PATH]="";
	strcpy( szLwr, szToken );
	strlwr( szLwr );

	for(vDeleteFileList::iterator it = m_vDeleteFileList.begin(); it != m_vDeleteFileList.end(); ++it)
	{
		if( it->m_szToken == szLwr )
		{
			return &(*it);
		}
	}

	return NULL;
}

void ioPac::RemoveAllDeleteList()
{
	for(vDeleteFileList::iterator it = m_vDeleteFileList.begin(); it != m_vDeleteFileList.end(); ++it)
	{
		for(vFileInfo::iterator itChild = it->m_vDeleteFileInfo.begin(); itChild != it->m_vDeleteFileInfo.end(); ++itChild)
			delete *itChild;   
			
		it->m_vDeleteFileInfo.clear();
	}

	m_vDeleteFileList.clear();
}

bool ioPac::PacPieceFile( IN const char *szFileName, IN const char* szPassword /*=""*/, IN const char* szComment /*= "" */ )
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
