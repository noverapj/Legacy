#include "StdAfx.h"
#include "FileReader.h"


FileReader::FileReader(void)
{
	Init();
}


FileReader::~FileReader(void)
{
	Destroy();
}

void FileReader::Init()
{
	m_readFile.clear();
}

void FileReader::Destroy()
{
	if( m_readFile.is_open() )
	{
		m_readFile.close();
	}
}

BOOL FileReader::OpenFile( TCHAR* fileName )
{
	if( _tcslen( fileName ) >= _MAX_FNAME )
	{
		return FALSE;
	}

#ifdef UNICODE
	char _fileName[_MAX_FNAME];
	int ret = WideCharToMultiByte( CP_ACP, 0, fileName, _tcslen( fileName ), _fileName, sizeof(_fileName), NULL, NULL );
	_fileName[ret] = 0;
	
	m_readFile.open( _fileName, ios::in );
#else
	m_readFile.open( fileName, ios::in );
#endif
	if( !m_readFile.is_open() )
	{

		return FALSE;
	}

	return TRUE;	
}

BOOL FileReader::GetLine( char* line, int len )
{
	/*if( !m_readFile.is_open() || m_readFile.eof() )
	{
		return FALSE;
	}*/

	m_readFile.getline( line, len );
	if( 0 == strlen(line ) )
	{
		if( m_readFile.eof() )
		{
			return FALSE;
		}
		else
		{
			return GetLine( line, len );
		}
	}

	return TRUE;	
}