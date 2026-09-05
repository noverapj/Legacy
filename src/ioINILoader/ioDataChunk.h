
#pragma once

class ioDataChunk
{
public:
	ioDataChunk();
	~ioDataChunk();

private:
	void Init();
	void Destroy();

	const TCHAR* GetPos()	{ return m_position; }
	const TCHAR* GetEnd()	{ return m_endPos; }
	TCHAR* GetFileBuffer()	{ return m_dataFile; }

	void ReAllocateDataFile( const int32 size );
	void SetDataFileLength( const int32 fileSize )	{ m_dataFileSize = fileSize; }
	int32 GetDataFileLength() const					{ return m_dataFileSize; }

public:
	bool AllocateFromFile( const TCHAR* fileName, bool bCreate = false );
	bool IsEOF() const;
	void GetLine( tstring& line, bool trim = true );
	int32 ReadUpTo( TCHAR* buff, const int32 size, const TCHAR* delim = _T("\n") );

private:
	TCHAR* m_dataFile;
	TCHAR *m_position;
	TCHAR *m_endPos;

	int32 m_dataFileSize;
};
