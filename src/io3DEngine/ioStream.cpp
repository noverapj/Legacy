

#include "stdafx.h"

#include "ioStream.h"

ioBinaryStream::ioBinaryStream()
{
}

ioBinaryStream::~ioBinaryStream()
{
	Clear();
}

bool ioBinaryStream::OpenFile( const char *szFileName )
{
	return m_MemFile.OpenFile( szFileName );
}

BYTE* ioBinaryStream::OpenMemory( int iSize, BYTE *pBuf )
{
	return m_MemFile.AllocateMemory( iSize, pBuf );
}

void ioBinaryStream::Clear()
{
	m_MemFile.Clear();
}

const BYTE* ioBinaryStream::GetPtr() const
{
	return m_MemFile.GetPtr();
}

BYTE* ioBinaryStream::GetPtr()
{
	return m_MemFile.GetPtr();
}

int ioBinaryStream::GetSize() const
{
	return m_MemFile.GetMemFileSize();
}

int ioBinaryStream::Read( void *pBuf, int iReadSize )
{
	return m_MemFile.Read( pBuf, iReadSize );
}

int ioBinaryStream::ReadString( char *pBuf )
{
	int iLen;
	m_MemFile.Read( &iLen, sizeof(int) );

	if( iLen > 0 )
	{
		int iReadBytes = Read( pBuf, iLen );
		pBuf[iLen] = '\0';
	}

	return iLen;
}

void ioBinaryStream::SetCurPos( int iPos )
{
	m_MemFile.SetCurPos( iPos );
}

int ioBinaryStream::GetCurPos() const
{
	return m_MemFile.GetCurPos();
}

void ioBinaryStream::DecompressLSCFile()
{
	m_MemFile.DecompressLSCFile();
}

ioTextStream::ioTextStream()
{
}

ioTextStream::~ioTextStream()
{
	Clear();
}

bool ioTextStream::OpenFile( const char *szFileName )
{
	return m_DataChunk.AllocateFromFile( szFileName );
}

BYTE* ioTextStream::OpenMemory( int iSize, BYTE *pBuf )
{
	return m_DataChunk.Allocate( iSize, pBuf );
}

void ioTextStream::Clear()
{
	m_DataChunk.Clear();
}

const BYTE* ioTextStream::GetPtr() const
{
	return m_DataChunk.GetPtr();
}

BYTE* ioTextStream::GetPtr()
{
	return m_DataChunk.GetPtr();
}

int ioTextStream::GetSize() const
{
	return m_DataChunk.GetSize();
}

int ioTextStream::ReadUpTo( void *pBuf, int iSize, const char* pDelim )
{
	return m_DataChunk.ReadUpTo( pBuf, iSize, pDelim );
}

int ioTextStream::SkipUpTo( const char* pDelim )
{
	return m_DataChunk.SkipUpTo( pDelim );
}

void ioTextStream::SkipToNextOpenBrace()
{
	m_DataChunk.SkipToNextOpenBrace();
}

void ioTextStream::SkipToNextCloseBrace()
{
	m_DataChunk.SkipToNextCloseBrace();
}

void ioTextStream::GetLine( std::string &line, bool bTrim )
{
	m_DataChunk.GetLine( line, bTrim );
}

bool ioTextStream::IsEOF() const
{
	return m_DataChunk.IsEOF();
}

