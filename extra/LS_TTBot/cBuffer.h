#pragma once

class cBuffer  
{
public:
	cBuffer();
	cBuffer( uint32 length );
	virtual ~cBuffer();

	void Init();
	void Destroy();

public:
	BOOL Create( uint32 length );
	BOOL Erase();

public:
	void SetLength( uint32 length )	{ m_length = length; }

	uint32 GetMaxLength()		{ return m_max; }
	uint32 GetLength()			{ return m_length; }
	uint8* GetBuffer() const	{ return m_pDT;	}
	
	uint8* GetBuffer( uint32 length ) const { return (m_pDT+length); }

	/*BOOL Copy( const uint8* pDT, uint32 length );
	BOOL Copy( const uint32 segment, const uint8* pDT, uint32 length );
	BOOL Append( const uint8* pDT, uint32 length );*/

	void Shift( uint32 length );

	bool Copy( const TCHAR* pDT );
	bool Copy( uint32 unDst, const TCHAR* pDT );
	bool Copy( const uint8* pDT, uint32 length );
	bool Copy( uint32 unDst, const uint8* pDT, uint32 length );

	bool Append( const char pDT );
	bool Append( const char* pDT );
	bool Append( const uint8* pDT, uint32 length );
	bool Append( const char* pDT, uint32 length );

protected:
	uint8* m_pDT;
	uint32 m_length;
	uint32 m_max;
};
