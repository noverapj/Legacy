#pragma once


#include <WTypes.h>
#define MAX_BUFFER		32768         //8192 * 4
#define MAX_SENDBUFFER	4096

enum PacketDefinition
{
	PK_ID_ADDR		= 0,
	PK_SIZE_ADDR    = 4,
	PK_CKSUM_ADDR   = 8,
	PK_FSM_ADDR     = 12,
	PK_HEADER_SIZE  = 16,
};

class LOGPacket
{
protected:   //예약되어있는 패킷데이터 8 byte
	typedef struct tagHeader
	{
		DWORD *m_ID;
		DWORD *m_Size;
		DWORD *m_CheckSum;
		int   *m_iState;
		tagHeader()
		{
			m_ID = m_Size = m_CheckSum = NULL;
			m_iState = NULL;
		}
	}PACKETHEADER;
	PACKETHEADER m_packet_header;                //PACKET ID

protected:
	int  m_currentPos;
	char m_pBuffer[MAX_BUFFER];
	bool CheckLeftPacketSize( int iAddSize );
	bool CheckRightPacketSize( int iAddSize );

public:
	void SetBufferCopy(const char *pBuf,int size);   // currentPos가 초기화된다.
	void SetBufferCopy(const char *pBuf,int size,int pos);
	void SetCheckSum( DWORD dwSum );
	void SetState( int iState );
	void Clear();

public:
	char *GetBuffer()		{ return &m_pBuffer[0]; }
	int   GetBufferSize()	{ return *m_packet_header.m_Size; }
	DWORD GetPacketID()		{ return *m_packet_header.m_ID; }
	DWORD GetCheckSum()		{ return *m_packet_header.m_CheckSum; }
	int   GetState()		{ return *m_packet_header.m_iState; }
	int   GetCurPos()		{ return m_currentPos; }

public:
	bool  IsValidHeader();
	bool  IsValidPacket();

public:
	LOGPacket();
	LOGPacket(DWORD ID);
	LOGPacket(char *buffer,int size);
	virtual ~LOGPacket();

	//operator
public:
	LOGPacket&  operator =  (LOGPacket& packet); // currentPos이 초기화되지 않는다.
	LOGPacket&  operator << (bool arg);
	LOGPacket&  operator << (int arg);
	LOGPacket&  operator << (LONG arg);
	LOGPacket&  operator << (DWORD arg);
	LOGPacket&  operator << (__int64 arg);
	LOGPacket&  operator << (LPTSTR arg);
	LOGPacket&  operator << (double arg);	
	LOGPacket&  operator << (float arg);
	template <typename T>
	LOGPacket&	operator << (T &data)
	{
		if( !CheckLeftPacketSize( sizeof(data) ) ) return *this;
		memcpy(&m_pBuffer[m_currentPos],&data,sizeof(data));
		m_currentPos += sizeof(data);
		*m_packet_header.m_Size = m_currentPos;
		return *this;
	}

	LOGPacket&  operator >> (bool &arg);
	LOGPacket&  operator >> (int &arg);
	LOGPacket&  operator >> (LONG &arg);
	LOGPacket&  operator >> (DWORD &arg);
	LOGPacket&  operator >> (__int64 &arg);
	LOGPacket&  operator >> (LPTSTR arg);
	LOGPacket&  operator >> (double &arg);	
	LOGPacket&  operator >> (float &arg);	
	template <typename T>
	LOGPacket&  operator >> (T &data)
	{ 
		if( !CheckRightPacketSize( sizeof(data) ) ) return *this;
		memcpy(&data,&m_pBuffer[m_currentPos],sizeof(data));
		m_currentPos += sizeof(data);
		return *this;
	}
};

