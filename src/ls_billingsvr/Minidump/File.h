#pragma once


// File
class File  
{
public:
	File();
	File(LPCTSTR fileName, DWORD dwMode, DWORD dwShareMode, DWORD dwCreationDisposition);
	virtual ~File();

public:
	BOOL Open(const TCHAR* fileName, DWORD dwMode, DWORD dwShareMode, DWORD dwCreationDisposition);
	void Close();

	BOOL Move(DWORD dwOrigin, LONG lOffset);
	virtual DWORD GetFileSize();

protected:
	enum SaveFileFormat
	{
		FILEFORMAT_ANSI = 1,
		FILEFORMAT_UNICODE_UTF8,
		FILEFORMAT_UNICODE_LE,
		FILEFORMAT_UNICODE_BE,
	};

	HANDLE	m_hFile;
	DWORD	m_dwSizeLow, m_dwSizeHigh;
};

// FileReader
class FileReader : public File
{
public:
	FileReader(const TCHAR* fileName=NULL);

public:
	BOOL Open(const TCHAR* fileName);

	BOOL Read(BYTE* buffer, const DWORD dwLength);
	BOOL Convert(const BYTE* bufferA, const int lengthA, BYTE* bufferB, const int lengthB);

	int GetBOM();

	virtual DWORD GetFileSize();

private:
	int m_nReadType;
};

// FileWriter
class FileWriter : public File
{
public:
	FileWriter(const TCHAR* fileName=NULL);

public:
	BOOL Open(const TCHAR* fileName, DWORD diposition=0);

	BOOL Write(const BYTE* buffer, const DWORD dwLength);
	BOOL WriteFormat(const TCHAR* format, ...);

	void WriteHeader();
};

