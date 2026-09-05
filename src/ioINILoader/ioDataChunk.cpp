
#include "stdafx.h"
#include "ioDataChunk.h"

const int32 INIT_FILEBUFFER_SIZE = 1024 * 1024 * 2;	// 2 MB

ioDataChunk::ioDataChunk()
{
	Init();
}

ioDataChunk::~ioDataChunk()
{
	Destroy();
}

void ioDataChunk::Init()
{
	m_dataFile = new TCHAR[ INIT_FILEBUFFER_SIZE ];
	SetDataFileLength( INIT_FILEBUFFER_SIZE );
	m_position  = NULL;
	m_endPos  = NULL;
}

void ioDataChunk::Destroy()
{
	// alloc
	if( m_dataFile != NULL )
	{
		delete [] m_dataFile;
		m_dataFile = NULL;
	}

	m_position  = NULL;
	m_endPos  = NULL;
}

void ioDataChunk::ReAllocateDataFile( const int32 size )
{
	if( GetFileBuffer() != NULL )
	{
		delete [] m_dataFile;
		m_dataFile = NULL;
	}

	m_dataFile = new TCHAR[ size ];
	if( m_dataFile )
		SetDataFileLength( size );
}

bool ioDataChunk::AllocateFromFile( const TCHAR* fileName, bool bCreate /*=false*/ )
{
	HANDLE hFile = CreateFile( fileName,
							   GENERIC_READ,
							   FILE_SHARE_READ | FILE_SHARE_WRITE,
							   NULL,
							   OPEN_EXISTING,
							   FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,
							   NULL );

	if( INVALID_HANDLE_VALUE == hFile )
	{
		if ( bCreate == true )
		{
			hFile = CreateFile( fileName,
				GENERIC_READ,
				FILE_SHARE_READ | FILE_SHARE_WRITE,
				NULL,
				CREATE_NEW,
				FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,
				NULL );
		}
		else
		{
			return false;
		}
	}

	DWORD fileSize = GetFileSize( hFile, NULL );
	if( INVALID_FILE_SIZE == fileSize )
	{
		CloseHandle( hFile );
		return false;
	}

	// check size
	if( fileSize + 1 > static_cast< DWORD >( GetDataFileLength() ) )
	{
		ReAllocateDataFile( fileSize + 1 );
	}

	TCHAR* chunkBuffer = GetFileBuffer();
	if( chunkBuffer )
	{
		ZeroMemory( GetFileBuffer(), GetDataFileLength() );

		chunkBuffer[ fileSize ] = _T( '\0' );

		DWORD readSize = 0;
		BOOL ret = ReadFile( hFile,
							  chunkBuffer,
							  fileSize,
							  &readSize,
							  NULL );

		if( FALSE == ret )
		{
			CloseHandle( hFile );
			return false;
		}

		m_position  = chunkBuffer;
		m_endPos  = chunkBuffer + fileSize;
	
		CloseHandle( hFile );
	}
	else
	{
		return false;
	}

	return true;	
}

bool ioDataChunk::IsEOF() const
{
	if( m_position >= m_endPos )
		return true;

	return false;
}

void ioDataChunk::GetLine( tstring& line, bool trim /*= true*/ )
{
	TCHAR buff[ 512 ] = {0,};

	int32 count = ReadUpTo( buff, sizeof( buff ) - 1 );
	buff[ count ] = _T( '\0' );

	line = buff;

	if( trim )
	{
		int32 len = line.length();
		int32 lSpace = 0;
		int32 rSpace = 0;

		//int i=0;
		for( int32 i = 0 ; i < len && ( line.at(i) == ' ' || line.at(i) == _T( '\t' ) || line.at(i) == _T( '\r' ) ) ; ++lSpace, i++ );

		if( lSpace < len )
		{
			for( int32 i = len - 1 ; i >= 0 && ( line.at(i) == ' ' || line.at(i) == _T( '\t' ) || line.at(i) == _T( '\r' ) ) ; rSpace++, i-- );
		}

		line = line.substr( lSpace, len - lSpace - rSpace );
	}
}

int32 ioDataChunk::ReadUpTo( TCHAR* buff, const int32 size, const TCHAR* delim /*= "\n"*/ )
{
	int32 pos = _tcscspn( (const TCHAR*)GetPos(), delim );
	if( pos > size )
		pos = size;

	if( GetPos() + pos > GetEnd() )
		pos = GetEnd() - GetPos();

	if( pos > 0 )
	{
		memcpy( buff, (const void*)GetPos(), pos );
	}

	m_position += pos + 1;

	return pos;
}
