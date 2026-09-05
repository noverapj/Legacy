#pragma once

class ioFile
{
public:
	ioFile(void);
	ioFile(LPCTSTR fileName, DWORD dwMode, DWORD dwShareMode, DWORD dwCreationDisposition);
	virtual ~ioFile(void);

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

// ioFileReader
class ioFileReader : public ioFile
{
public:
	ioFileReader(const TCHAR* fileName=NULL);

public:
	BOOL Open(const TCHAR* fileName);

	BOOL Read(BYTE* buffer, const DWORD dwLength);
	BOOL Convert(const BYTE* bufferA, const int lengthA, BYTE* bufferB, const int lengthB);

	int GetBOM();

	virtual DWORD GetFileSize();

private:
	int m_nReadType;
};

// ioFileWriter
class ioFileWriter : public ioFile
{
public:
	ioFileWriter(const TCHAR* fileName=NULL);

public:
	BOOL Open(const TCHAR* fileName, DWORD diposition=0);

	BOOL Write(const BYTE* buffer, const DWORD dwLength);
	BOOL WriteFormat(const TCHAR* format, ...);

	void WriteHeader();
};
