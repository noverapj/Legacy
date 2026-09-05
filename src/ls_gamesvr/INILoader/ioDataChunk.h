

#ifndef _ioDataChunk_h_
#define _ioDataChunk_h_

class ioDataChunk
{
private:
	BYTE *m_pData;
	BYTE *m_pPos;
	BYTE *m_pEnd;

	int	  m_iSize;

public:
	BYTE* Allocate( int iSize, const BYTE *pPtr = NULL );
	bool AllocateFromFile( const char *szName );
	int  Read( void *pBuf, int iSize );
	void Seek( int iPos );
	void Clear();

public:
	int ReadUpTo( void *pBuf, int iSize, const char* pDelim = "\n" );
	int SkipUpTo( const char* pDelim = "\n" );

	void SkipToNextOpenBrace();
	void SkipToNextCloseBrace();

public:
	void GetLine( std::string &line, bool bTrim = true );
	bool IsEOF() const;

public:
	inline const BYTE* GetPtr() const { return m_pData; }
	inline BYTE* GetPtr() { return m_pData; }

	inline int GetSize() const { return m_iSize; }

public:
	ioDataChunk();
	~ioDataChunk();
};

#endif
