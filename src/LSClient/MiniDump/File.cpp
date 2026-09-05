// File.cpp: implementation of the File class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "File.h"
#include <stdio.h>
 


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

File::File() : m_hFile(INVALID_HANDLE_VALUE), m_dwSizeLow(0), m_dwSizeHigh(0)
{
}

File::File(LPCTSTR fileName, DWORD dwMode, DWORD dwShareMode, DWORD dwCreationDisposition) : m_hFile(INVALID_HANDLE_VALUE), m_dwSizeLow(0), m_dwSizeHigh(0)
{
	Open(fileName, dwMode, dwShareMode, dwCreationDisposition);
}

File::~File()
{
	Close();
}

//////////////////////////////////////////////////////////////////////
// operation
//////////////////////////////////////////////////////////////////////

BOOL File::Open(const TCHAR* fileName, DWORD dwMode, DWORD dwShareMode, DWORD dwCreationDisposition)
{
	Close();

	m_hFile	= CreateFile(	
		reinterpret_cast<LPCTSTR>(fileName), 
		dwMode,
		dwShareMode,
		NULL,
		dwCreationDisposition,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if(m_hFile == INVALID_HANDLE_VALUE) return FALSE;

	return (m_hFile == INVALID_HANDLE_VALUE) ? FALSE : TRUE;
}

void File::Close()
{
	if(m_hFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hFile);
		m_hFile = NULL;
	}

	m_dwSizeLow = 0;
	m_dwSizeHigh = 0;
}

BOOL File::Move(DWORD dwOrigin, LONG lOffset)
{
	if(m_hFile != INVALID_HANDLE_VALUE)
	{
		DWORD dwResult = SetFilePointer(	m_hFile,
			lOffset,
			NULL, 
			dwOrigin);
		return (dwResult == 0xFFFFFFFF) ? FALSE : TRUE;
	}
	return FALSE;
}

DWORD  File::GetFileSize()
{
	if(m_dwSizeLow != 0)
		return m_dwSizeLow;

	DWORD dwError = 0;
	if(m_hFile != INVALID_HANDLE_VALUE)
	{
		m_dwSizeLow = ::GetFileSize(m_hFile, &m_dwSizeHigh);
		if((INVALID_FILE_SIZE == m_dwSizeLow) && 
			((dwError = GetLastError()) != NO_ERROR))
			return 0;
	}
	return m_dwSizeLow;
}

//////////////////////////////////////////////////////////////////////
// FileReader class
//////////////////////////////////////////////////////////////////////

FileReader::FileReader(const TCHAR* fileName) : m_nReadType(-1)
{
	if(fileName != NULL)
	{
		Open(fileName);
	}
}

BOOL FileReader::Open(const TCHAR* fileName)
{
	if(File::Open(fileName, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, OPEN_EXISTING))
	{
		return (GetBOM() > 0) ? TRUE : FALSE;
	}
	return FALSE;
}

BOOL FileReader::Read(BYTE* buffer, const DWORD dwLength)
{
	DWORD dwWritten = 0;
	if(m_hFile == INVALID_HANDLE_VALUE) return FALSE;

	if(ReadFile(	m_hFile, 
		buffer,
		dwLength, 
		&dwWritten, 
		NULL))
	{
		if(dwLength == dwWritten) return TRUE;
	}
	return FALSE;
}

BOOL FileReader::Convert(const BYTE* bufferA, const int lengthA, BYTE* bufferB, const int lengthB)
{
	switch(m_nReadType)
	{
	case FILEFORMAT_ANSI :
		CopyMemory(bufferB, bufferA, (lengthA > lengthB) ? lengthB : lengthA);
		return TRUE;

	case FILEFORMAT_UNICODE_UTF8 :
		CopyMemory(bufferB, bufferA, (lengthA > lengthB) ? lengthB : lengthA);
		return TRUE;

	case FILEFORMAT_UNICODE_LE :
		WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)bufferA, lengthA, (LPSTR)bufferB, lengthB, NULL, NULL);
		return TRUE;

	case FILEFORMAT_UNICODE_BE :
		return TRUE;
	}
	return FALSE;
}

int FileReader::GetBOM()
{
	if((-1) == m_nReadType)
	{
		Move(FILE_BEGIN, 0);

		WORD BOM = 0;
		if(Read(reinterpret_cast<BYTE*>(&BOM), sizeof(WORD)))
		{
			switch(BOM)
			{
			case 0xFEFF :
				m_nReadType = FILEFORMAT_UNICODE_LE;
				break;

			case 0xFFFE :
				m_nReadType = 0; //FILEFORMAT_UNICODE_BE; not supported
				break;

			case 0xBBEF :
				m_nReadType = FILEFORMAT_UNICODE_UTF8;
				Move(FILE_BEGIN, 3);
				break;

			default:
				m_nReadType = FILEFORMAT_ANSI;
				Move(FILE_BEGIN, 0);
				break;
			}
			return m_nReadType;
		}
		return 0;
	}
	return m_nReadType;
}

DWORD FileReader::GetFileSize()
{
	if(0 == m_dwSizeLow)
	{
		File::GetFileSize();

		switch(m_nReadType)
		{
		case FILEFORMAT_UNICODE_LE :
		case FILEFORMAT_UNICODE_BE :
			m_dwSizeLow -= (2);
			break;

		case FILEFORMAT_UNICODE_UTF8 :
			m_dwSizeLow -= (3);
			break;

		default:
			break;
		}
	}
	return m_dwSizeLow;
}

//////////////////////////////////////////////////////////////////////
// FileWriter class
//////////////////////////////////////////////////////////////////////

FileWriter::FileWriter(const TCHAR* fileName)
{
	if(fileName != NULL)
	{
		Open(fileName);
	}
}

BOOL FileWriter::Open(const TCHAR* fileName, DWORD diposition)
{
	if(0 == diposition)
	{
		diposition = CREATE_NEW;
	}

	if(File::Open(fileName, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, diposition))
	{
		WriteHeader();
		return TRUE;
	}
	return FALSE;
}

BOOL FileWriter::Write(const BYTE* buffer, const DWORD dwLength)
{
	if(dwLength == 0)
		return TRUE;

	DWORD dwBytes = 0;
	if(WriteFile(	m_hFile, 
		buffer, 
		dwLength, 
		&dwBytes, 
		NULL))
	{
		return (dwBytes == dwLength) ? TRUE : FALSE;
	}
	return FALSE;
}

BOOL FileWriter::WriteFormat(const TCHAR* format, ...)
{
	TCHAR buffer[4096]; 

	va_list marker; 
	va_start(marker, format); 
	_vstprintf_s(buffer, _countof(buffer), format, marker); 
	va_end(marker);

	return Write(reinterpret_cast<BYTE*>(buffer), (DWORD)(_tcslen(buffer)*sizeof(TCHAR)));
}

void FileWriter::WriteHeader()
{
#ifdef _UNICODE
	Move(FILE_BEGIN, 0);

	WORD BOM = 0xFEFF;
	Write(reinterpret_cast<BYTE*>(&BOM), sizeof(BOM));
#endif
}