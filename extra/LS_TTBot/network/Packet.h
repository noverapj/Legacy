#pragma once


const int MAX_BUFFER = 1024 * 32;

enum 
{
	PK_ID_ADDR		= 0,
	PK_SIZE_ADDR    = 4,
	PK_CKSUM_ADDR   = 8,
	PK_FSM_ADDR     = 12,
	PK_HEADER_SIZE  = 16,
};

class CPacket
{
public:
	struct PACKETHEADER
	{
		DWORD *m_ID;
		DWORD *m_Size;
		DWORD *m_CheckSum;
		int   *m_iState;
		PACKETHEADER()
		{
			m_ID = m_Size = m_CheckSum = NULL;
			m_iState = NULL;
		}
	};

protected:
	PACKETHEADER m_packet_header;
	uint32 m_currentPos;
	uint8 m_pBuffer[MAX_BUFFER];

	bool CheckLeftPacketSize( int iAddSize );
	bool CheckRightPacketSize( int iAddSize );
public:
	void Clear();
	void SetBufferCopy(char *pBuf,int size);
	void SetDataAdd(char *pBuf,int size);
	void SetCheckSum( DWORD dwSum );
	void SetState( int iState );
	
public:
	const uint8* GetBuffer() const;            //패킷 버퍼의 모든 내용(헤더 포함).
	const uint8* GetData() const;              //패킷 버퍼의 데이터 내용(헤더 미포함).
	
	uint32 GetBufferSize() const;
	uint32 GetDataSize() const;

	DWORD GetPacketID() const;
	DWORD GetCheckSum() const;
	int GetState() const;

	bool IsValidHeader() const;
	bool IsValidPacket() const;

public:
	CPacket();
	CPacket( DWORD ID );
	CPacket( const uint8 *buffer, uint32 size );
	virtual ~CPacket();
};
