// BigPac.h: interface for the CBigPac class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BIGPAC_H__28A1AF78_F4C7_402D_BB14_797D73A6A549__INCLUDED_)
#define AFX_BIGPAC_H__28A1AF78_F4C7_402D_BB14_797D73A6A549__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MAX_TEMP_STRING 1024

#include <set>
#include <vector>
#include <string>
#include <algorithm> 
#include "PacUtil.h"
#include "ZipArchive/ZipArchive.h"

#include "atlstr.h"//for cstring
using namespace std;

typedef struct tagFileInfo 
{
	DWORD  HashCode;
	string FileName;
	unsigned short Index;

#ifdef STATIC_PAC_API
	DWORD ZipFileSize;
#endif

	tagFileInfo()
	{
		HashCode = 0;
		Index = 0;

#ifdef STATIC_PAC_API
		ZipFileSize   = 0;
#endif
	}

	DWORD CalcHashCode( const char *str ) const
	{
		DWORD dwLen, dwCh, dwResult;

		dwLen    = strlen( str );
		dwResult = 5381;

		for( DWORD i=0; i<dwLen ; i++ )
		{
			dwCh     = (DWORD)str[i];
			dwResult = ((dwResult<< 5) + dwResult) + dwCh; // hash * 33 + ch
		}

		return dwResult;
	}
}FileInfo;

//--------------------------------------------------------------------------------------------------
class FileInfoCompare : public binary_function< const FileInfo*, const FileInfo*, bool > // setºñ±³ class
{
public:
	bool operator()(const FileInfo *lhs , const FileInfo *rhs) const
	{
		if(lhs->HashCode < rhs->HashCode) 
			return true; 
		else if( lhs->HashCode == rhs->HashCode) 
		{ 
			if( lhs->FileName < rhs->FileName )  
				return true; 
			else 
				return false; 
		} 
		else 
			return false; 
	}
};

//-------------------------------------------------------------------------------------------------
typedef set<FileInfo* ,FileInfoCompare> FileInfoSet;
typedef FileInfoSet::iterator FileInfoIter;

typedef vector<unsigned short> vIndex;
typedef vIndex::iterator vIndexIter;

typedef std::vector<FileInfo*> vFileInfo;
struct DeleteFileList
{
	string    m_szToken;
	vFileInfo m_vDeleteFileInfo;
};
typedef std::vector<DeleteFileList> vDeleteFileList;


#define BIGPAC_ERROR_INDEX -1
#define BIGPAC_ERROR_SIZE  -1
#define BIGPAC_ERROR_CRC    0
#define BIGPAC_ERROR_TIME   0

class CZipArchive;
class CZipMemFile;

class PAC_API CBigPac : public CPacUtil 
{
private:
	friend class ioPac;

protected:
	CBigPac();
	virtual ~CBigPac();

private:
	FileInfoSet	 m_setFileInfo;

	CZipArchive *m_pZip;
	int			 m_iMaxIndex;

#ifdef STATIC_PAC_API
	ULONGLONG    m_GarbageSize;
	FileInfoIter m_SetIter;
#endif

public:
	enum DivideLoadResult
	{
		DLR_ERROR,
		DLR_LOADING,
		DLR_DONE,
	};

private:
	CZipMemFile *m_pMemFile;
	BYTE *m_pMemFileBuf;

	HANDLE m_hDivideFile;
	DWORD  m_dwTotalLoadBytes;
	DWORD  m_dwOneTimeLoadBytes;
	DWORD  m_dwCurLoadedBytes;

public:
	bool OpenBigPac(IN const char* szPath, IN bool bReadOnly = true , IN bool bBuildSetFileInfo = true, IN const char *szPassword = "" );
	void CloseBigPac();

public:
	bool BeginDividePackFile( const char *szPackFileName, int iDivideCount );
	DivideLoadResult LoadDividePackFile( char *szErrorLog , IN const char *szPassword = "" );

private:
	bool CheckOverlapPath( CString const strPath );
	bool OpenBigPacInMemory( CZipMemFile *pMemFile , IN const char *szPassword = "");
	void ClearDivideLoadResource();

public:
	bool IsAllocBigPac() const;
	bool IsMemoryLoading() const;

private:
	bool BuildSetFileInfo();
	void ClearSetFileInfo();
	void DestroyZip();

public:
	int GetFileIndex(IN const char* szFileName);
//	int GetFileIndexInZip(IN const char* szFileName);
//	DWORD GetFileCRCInZip(int iIndex);
	int GetFileSize(IN const int iIndex);
	DWORD GetFileCRC( IN const int iIndex );
	time_t GetFileTime(IN const int iIndex);
	void   GetFileComment( OUT char *szComment, IN int iCommentSize, IN const int iIndex );

	bool GetBuffer( IN const int iIndex, OUT BYTE* pData, IN const DWORD dwDataSize );
	void DeleteFileInfo( DeleteFileList *pDeleteList );
	void DeleteFileInZip( IN string strPath, IN DWORD iCrc);
public:
	DWORD GetTotalLoadBytes(){ return m_dwTotalLoadBytes; }

#ifdef STATIC_PAC_API
public:
	bool InitSetIter();
	bool GetCurFileInfo(OUT FileInfo &kFileInfo);
	void NextSetIter();
	int  GetSetSize();

	bool Extract(IN const char* szExtractPath);
	bool Extract(IN const int iIndex, IN const char* szExtractPath);
	bool TestFile( IN const int iIndex );

public:
	ULONGLONG GetGarbageSize() const { return m_GarbageSize; }
	int  GetMaxIndex() const;
	bool GetFileName(IN const int iIndex, OUT char* szName, IN const int iNameSize);
	int  GetFileComprSize( IN const char* szFileName );

	bool AddBigPac(IN const char* szPiecePacName, IN const char* szInPacName, IN const time_t lasWriteTime, IN const char *szPassword, IN const char *szComment );
	bool AddBigPacData( IN const char* szInPacName, IN const BYTE * pData, IN const DWORD uSize, IN const char *szComment );
	void RemoveBigPacInFile(IN int iIndex);
	void Flush();
#endif // STATIC_PAC_API
};


#endif // !defined(AFX_BIGPAC_H__28A1AF78_F4C7_402D_BB14_797D73A6A549__INCLUDED_)
