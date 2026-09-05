#pragma once

#define MAX_BUFFER		32768 * 2         //8192 * 4
#define MAX_SENDBUFFER	4096

enum PacketDefinition
{
	PK_ID_ADDR		= 0,
	PK_SIZE_ADDR    = 4,
	PK_CKSUM_ADDR   = 8,
	PK_FSM_ADDR     = 12,
	PK_HEADER_SIZE  = 16,
};

enum PacketFlowTypes
{
	PK_TCP,
	PK_UDP,
};

class IOCP_SOCKET_API CPacket
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
	bool m_bClear;

    bool CheckLeftPacketSize( int iAddSize );
	bool CheckRightPacketSize( int iAddSize );
public:
	void Clear();
	
public:
	void SetBufferCopy(const char *pBuf,int size);   // currentPos가 초기화된다.
	void SetBufferCopy(const char *pBuf,int size,int pos);
	void SetCheckSum( DWORD dwSum );
	void SetState( int iState );
	
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
	CPacket();
	CPacket(DWORD ID);
	CPacket(char *buffer,int size);
	CPacket(DWORD ID, bool bClear, PacketFlowTypes pkflowType, int iSize);
	CPacket(bool bClear, PacketFlowTypes pkflowType, int iSize);
	virtual ~CPacket();

	//operator
public:
	CPacket&  operator =  (CPacket& packet); // currentPos이 초기화되지 않는다.
	CPacket&  operator << (bool arg);
	CPacket&  operator << (int arg);
	CPacket&  operator << (LONG arg);
	CPacket&  operator << (DWORD arg);
	CPacket&  operator << (__int64 arg);
	CPacket&  operator << (LPTSTR arg);
	CPacket&  operator << (double arg);	
	CPacket&  operator << (float arg);
	
	CPacket&  operator >> (bool &arg);
	CPacket&  operator >> (int &arg);
	CPacket&  operator >> (LONG &arg);
	CPacket&  operator >> (DWORD &arg);
	CPacket&  operator >> (__int64 &arg);
	CPacket&  operator >> (LPTSTR arg);
	CPacket&  operator >> (double &arg);	
	CPacket&  operator >> (float &arg);	

public:
	bool Write(bool arg);
	bool Write(int arg);
	bool Write(LONG arg);
	bool Write(DWORD arg);
	bool Write(__int64 arg);
	bool Write(LPTSTR arg);
	bool Write(double arg);
	bool Write(float arg);

	bool Read(bool& arg);
	bool Read(int& arg);
	bool Read(LONG& arg);
	bool Read(DWORD& arg);
	bool Read(__int64& arg);
	bool Read(const int nLength, LPTSTR arg);
	bool Read(double& arg);
	bool Read(float& arg);
};

