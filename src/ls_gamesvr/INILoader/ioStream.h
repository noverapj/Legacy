

#ifndef _ioStream_h_
#define _ioStream_h_

#include "ioDataChunk.h"

class ioStream
{
public:
	virtual bool  OpenFile( const char *szFileName ) = 0;
	virtual BYTE* OpenMemory( int iSize, BYTE *pBuf = NULL ) = 0;
	virtual void  Clear() = 0;

public:
	virtual const BYTE* GetPtr() const = 0;
	virtual BYTE* GetPtr() = 0;
	virtual int GetSize() const = 0;

public:
	ioStream(){}
	virtual ~ioStream(){}
};


class ioTextStream : public ioStream
{
protected:
	ioDataChunk m_DataChunk;

public:
	virtual bool  OpenFile( const char *szFileName );
	virtual BYTE* OpenMemory( int iSize, BYTE *pBuf = NULL );
	virtual void  Clear();

public:
	virtual const BYTE* GetPtr() const;
	virtual BYTE* GetPtr();
	virtual int GetSize() const;

public:
	int ReadUpTo( void *pBuf, int iSize, const char* pDelim = "\n" );
	int SkipUpTo( const char* pDelim = "\n" );

	void SkipToNextOpenBrace();
	void SkipToNextCloseBrace();

public:
	void GetLine( std::string &line, bool bTrim = true );
	bool IsEOF() const;


public:
	ioTextStream();
	virtual ~ioTextStream();
};

#endif



















