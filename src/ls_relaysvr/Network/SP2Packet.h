#ifndef _SP2Packet_h_
#define _SP2Packet_h_

class CPacket;
 
 

class SP2Packet : public CPacket
{
	public:
	SP2Packet();
	SP2Packet( const SP2Packet &rhs );
	SP2Packet(DWORD ID);
	SP2Packet(TCHAR *buffer,int size);
	SP2Packet( DWORD dwUserIndex, SP2Packet &rhs );
	virtual ~SP2Packet();

	const TCHAR* GetData() const;
	int GetDataSize() const;
	const TCHAR* GetBuffer() const;	
	int   GetBufferSize() const;
	void SetDataAdd( TCHAR *buffer, int size, bool bCurPosReSet = false );
	void  SetDataAddCreateUDP( DWORD dwIP, DWORD dwPort, char *buffer, int size, bool bCurPosReSet = false );
	// >> 쪽으로 current_pos 이동
	void  MovePointer( DWORD dwMoveBytes );
	void  MoveBufferPointer(int size); //For UDP Packet
	void  GetSockAddress(sockaddr_in& arg);//For UDP Packet
	void  SetPosBegin();
	//operator
public:
	SP2Packet&  operator =  ( const SP2Packet& packet );
	SP2Packet&  operator << (BYTE arg);
	SP2Packet&  operator << (bool arg);
	SP2Packet&  operator << (int arg);
	SP2Packet&  operator << (LONG arg);
	SP2Packet&  operator << (WORD arg);
	SP2Packet&  operator << (DWORD arg);
	SP2Packet&  operator << (__int64 arg);
	SP2Packet&  operator << (LPTSTR arg);
	SP2Packet&  operator << (double arg);	
	SP2Packet&  operator << (float arg);
	SP2Packet&  operator << (short arg);
	SP2Packet& operator << ( const sockaddr_in& arg);//For UDP Packet
	template <typename T>
	SP2Packet&	operator << (T &data)
	{
		if( !CheckLeftPacketSize( sizeof(data) ) ) return *this;
		memcpy(&m_pBuffer[m_currentPos],&data,sizeof(data));
		m_currentPos += sizeof(data);
		*m_packet_header.m_Size = m_currentPos;
		return *this;
	}
 
	
	SP2Packet&  operator >> (BYTE &arg);
	SP2Packet&  operator >> (bool &arg);
	SP2Packet&  operator >> (int &arg);
	SP2Packet&  operator >> (LONG &arg);
	SP2Packet&  operator >> (WORD &arg);
	SP2Packet&  operator >> (DWORD &arg);
	SP2Packet&  operator >> (__int64 &arg);
	SP2Packet&  operator >> (LPTSTR arg);
	SP2Packet&  operator >> (double &arg);	
	SP2Packet&  operator >> (float &arg);	
	SP2Packet&  operator >> (short &arg);
	SP2Packet& operator  >> ( sockaddr_in& arg);//For UDP Packet
	template <typename T>
	SP2Packet&  operator >> (T &data)
	{ 
		if( !CheckRightPacketSize( sizeof(data) ) ) return *this;
		memcpy(&data,&m_pBuffer[m_currentPos],sizeof(data));
		m_currentPos += sizeof(data);
		return *this;
	}

 
};
#endif