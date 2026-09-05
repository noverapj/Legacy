

#include "stdafx.h"
#include "ioStream.h"

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

