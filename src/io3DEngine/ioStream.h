

#ifndef _ioStream_h_
#define _ioStream_h_

#include "ioMemFile.h"
#include "ioDataChunk.h"

class __EX ioStream
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
	virtual void DecompressLSCFile(){};

public:
	ioStream(){}
	virtual ~ioStream(){}
};

class __EX ioBinaryStream : public ioStream
{
protected:
	ioMemFile m_MemFile;

public:
	virtual bool  OpenFile( const char *szFileName );
	virtual BYTE* OpenMemory( int iSize, BYTE *pBuf = NULL );
	virtual void  Clear();

public:
	virtual const BYTE* GetPtr() const;
	virtual BYTE* GetPtr();
	virtual int GetSize() const;

public:
	int Read( void *pBuf, int iReadSize );
	int ReadString( char *pBuf );

public:
	void SetCurPos( int iPos );
	int GetCurPos() const;

public:
	virtual void DecompressLSCFile();

public:
	inline const char* GetMemFileName(){ return m_MemFile.GetFileName(); }

public:
	ioBinaryStream();
	virtual ~ioBinaryStream();
};

class __EX ioTextStream : public ioStream
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



















