
#include "stdafx.h"

#include "ioCPU.h"
#include "HelpFunc.h"
#include "ioMemFile.h"

ioMemFile::ioMemFile()
{
	memset(m_szFileName, 0, MAX_PATH);
	m_iFileSize = 0;
	m_iCurPos = 0;

	m_pFileBuf = NULL;
}

ioMemFile::~ioMemFile()
{
	Clear();
}

bool ioMemFile::OpenFile( const char *szName )
{
	Clear();
	HANDLE hFile = CreateFile( szName,
							   GENERIC_READ,
							   FILE_SHARE_READ,
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

	m_pFileBuf = new unsigned char[dwFileSize];

	BOOL  bRet;
	DWORD dwReadedSize = 0;
	bRet = ReadFile( hFile,
				     m_pFileBuf,
					 dwFileSize,
					 &dwReadedSize,
					 NULL );

	if( FALSE == bRet )
	{
		SAFEDELETEARRAY(m_pFileBuf);
		return false;
	}

	m_iFileSize = dwFileSize;
	m_iCurPos = 0;

	strcpy( m_szFileName, szName );

	CloseHandle( hFile );

	return true;
}


BYTE* ioMemFile::AllocateMemory(  int iSize, const BYTE *pPtr )
{
	assert( iSize > 0 );

	m_iFileSize = 0;
	m_iCurPos = 0;
	SAFEDELETEARRAY( m_pFileBuf );

	m_pFileBuf = new unsigned char[iSize];
	if( m_pFileBuf )
	{
		m_iFileSize = iSize;

		if( pPtr )
		{
			ioCPU::FastMemcpy( m_pFileBuf, pPtr, iSize );
		}
	}

	return m_pFileBuf;
}

int ioMemFile::Read( void *pBuf, size_t iReadSize )
{
	size_t iRealSize = 0;
	size_t iRemainSize = 0;

	iRemainSize = m_iFileSize - m_iCurPos;

	if( 0 == iRemainSize )		// end of file
		return 0;
	else if( iReadSize < iRemainSize )
		iRealSize = iReadSize;
	else						// 남은 부분이 부족함
		iRealSize = iRemainSize;

	ioCPU::FastMemcpy( pBuf, m_pFileBuf + m_iCurPos, iRealSize );

	m_iCurPos += iRealSize;

	return iRealSize;
}

bool ioMemFile::SetCurPos( int iPos )
{
	if( m_iFileSize < iPos || iPos < 0 )
		return false;

	m_iCurPos = iPos;

	return true;
}

void ioMemFile::Clear()
{
	memset(m_szFileName, 0, MAX_PATH);
	m_iFileSize = 0;
	m_iCurPos = 0;

	SAFEDELETEARRAY(m_pFileBuf);
}

const BYTE* ioMemFile::GetPtr() const
{
	return m_pFileBuf;
}

BYTE* ioMemFile::GetPtr()
{
	return m_pFileBuf;	
}

void ioMemFile::DecompressLSCFile()
{
	if( !m_pFileBuf ) return;

	Decrypt( m_pFileBuf, m_iFileSize );

/*	int iCutSize = (float)m_iFileSize * LSC_FILE_CUT_RATE;
	unsigned char *pTempBuf = new unsigned char[m_iFileSize];
	ioCPU::FastMemcpy( &pTempBuf[0], &m_pFileBuf[iCutSize], m_iFileSize - iCutSize );
	ioCPU::FastMemcpy( &pTempBuf[m_iFileSize - iCutSize], &m_pFileBuf[0], iCutSize );
	ioCPU::FastMemcpy( m_pFileBuf, pTempBuf, m_iFileSize );

	SAFEDELETEARRAY( pTempBuf );
*/
}

void ioMemFile::Decrypt( BYTE *szSrc, int len )
{
	BYTE ch;
	BYTE temp;
	for(int i = 0;i < len;i++)
	{
		ch = szSrc[i];
		temp = (ch & 0x07) << 5;
		ch>>=3;
		ch |= temp;
		ch ^= 0xff;
		szSrc[i] = ch;
	}	
}