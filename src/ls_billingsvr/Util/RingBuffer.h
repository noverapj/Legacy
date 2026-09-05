#ifndef _RingBuffer_h_
#define _RingBuffer_h_

#define LIMIT_SEND_BUFFER           52428800              //50M
class CRingBuffer
{
protected:
	DWORD m_dwBufferSize;
	DWORD m_dwPlusBufferSize;
	DWORD m_dwPacketSize;
	bool  m_bRotateBuffer;

protected:
	char *m_aBuffer;
	char *m_aInstantBuffer;

protected:
	char *m_pStart;       
	char *m_pEnd;         
	char *m_pRealEnd;     

protected:
	bool IsNewMemory( int iAddBuffer );
	void Realloc();

public:
	void InitBuffer();

public:
	void WriteBuffer( char *pBuffer, int iBufferSize );
	void SendComplete( int iBytesTransfer );

public:
	inline char *GetBuffer(){ return m_pStart; }
	inline int   GetBufferSize(){ return m_pEnd - m_pStart; }

public:
	CRingBuffer( const DWORD dwBufferSize, const DWORD dwPacketSize );
	virtual ~CRingBuffer();
};

#endif