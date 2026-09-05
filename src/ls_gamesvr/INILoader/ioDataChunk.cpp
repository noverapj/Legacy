

#include "stdafx.h"

#include "ioDataChunk.h"

ioDataChunk::ioDataChunk()
{
	m_pData = NULL;
	m_pPos  = NULL;
	m_pEnd  = NULL;
	m_iSize = 0;
}

ioDataChunk::~ioDataChunk()
{
	SAFEDELETEARRAY(m_pData);
}

BYTE* ioDataChunk::Allocate( int iSize, const BYTE *pPtr )
{
	assert( iSize > 0 );

	SAFEDELETEARRAY( m_pData );
	m_pPos  = NULL;
	m_pEnd  = NULL;
	m_iSize = 0;

	m_pData = new BYTE[ iSize + 1 ];	// +1 is for null
	if( m_pData )
	{
		m_pData[iSize] = '\0';

		m_pPos  = m_pData;
		m_pEnd  = m_pData + iSize;
		m_iSize = iSize;

		if( pPtr )
		{
			memcpy( m_pData, pPtr, iSize );
		}
	}

	return m_pData;
}

bool ioDataChunk::AllocateFromFile(  const char *szName  )
{
	Clear();

	HANDLE hFile = CreateFile( szName,
							   GENERIC_READ,
							   0,
							   NULL,
							   OPEN_EXISTING,
							   FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,
							   NULL );

	if( INVALID_HANDLE_VALUE == hFile )
		return false;

	DWORD dwFileSize = GetFileSize(hFile, NULL);
	if( INVALID_FILE_SIZE == dwFileSize )
	{
		CloseHandle( hFile );
		return false;
	}

	m_pData = new BYTE[ dwFileSize + 1 ];	// +1 is for null
	m_pData[dwFileSize] = '\0';

	DWORD dwReadedSize = 0;
	BOOL bRet = ReadFile( hFile,
						  m_pData,
						  dwFileSize,
						  &dwReadedSize,
						  NULL );

	if( FALSE == bRet )
	{
		SAFEDELETEARRAY(m_pData);
		CloseHandle( hFile );
		return false;
	}

	m_iSize = dwFileSize;
	m_pPos  = m_pData;
	m_pEnd  = m_pData + dwFileSize;

	CloseHandle( hFile );

	return true;	
}

void ioDataChunk::Clear()
{
	SAFEDELETEARRAY( m_pData );
	m_iSize = 0;
}

int ioDataChunk::Read( void *pBuf, int iSize )
{
	int iCnt = iSize;
	if( m_pPos + iSize > m_pEnd )
		iCnt = m_pEnd - m_pPos;
	
	if( iCnt > 0 )
	{
		memcpy( pBuf, (const void*)m_pPos, iCnt );
		m_pPos += iCnt;
	}

	return iCnt;
}

void ioDataChunk::Seek( int iPos )
{
	if( iPos <= m_iSize )
	{
		m_pPos = m_pData + iPos;
	}
}

bool ioDataChunk::IsEOF() const
{
	if( m_pPos >= m_pEnd )
		return true;

	return false;
}

int ioDataChunk::ReadUpTo( void *pBuf, int iSize, const char* pDelim )
{
	int iPos = strcspn( (const char*)m_pPos, pDelim );
	if( iPos > iSize )
		iPos = iSize;

	if( m_pPos + iPos > m_pEnd )
		iPos = m_pEnd - m_pPos;

	if( iPos > 0 )
	{
		memcpy( pBuf, (const void*)m_pPos, iPos );
	}

	m_pPos += iPos + 1;

	return iPos;
}

int ioDataChunk::SkipUpTo( const char *pDelim )
{
	int iPos = strcspn( (const char*)m_pPos, pDelim );

	if( m_pPos + iPos > m_pEnd )
		iPos = m_pEnd - m_pPos;

	m_pPos += iPos + 1;

	return iPos;
}

void ioDataChunk::GetLine( std::string &line, bool bTrim )
{
	char pBuf[512];

	int iCount = ReadUpTo( pBuf, 511 );
	pBuf[iCount] = '\0';

	line = pBuf;

	if( bTrim )
	{
		int iLen = line.length();

		int lSpace, rSpace;
		lSpace = rSpace = 0;

		int i=0;
		for(int i=0 ; 
			 i<iLen && ( line.at(i) == ' ' || line.at(i) == '\t' || line.at(i) == '\r' ) ;
			 ++lSpace, i++ );

		if( lSpace < iLen )
		{
			for(int i= iLen-1 ; 
				 i>=0 && ( line.at(i) == ' ' || line.at(i) == '\t' || line.at(i) == '\r' ) ;
				 rSpace++, i-- );
		}

		line = line.substr( lSpace, iLen - lSpace - rSpace );
	}
}

void ioDataChunk::SkipToNextOpenBrace()
{
	std::string line = "";

	while( !IsEOF() && line !="{" )
	{
		GetLine( line );		
	}
}

void ioDataChunk::SkipToNextCloseBrace()
{
	std::string line = "";

	while( !IsEOF() && line.substr(0,1) != "}" )
	{
		GetLine( line );
	}
}

