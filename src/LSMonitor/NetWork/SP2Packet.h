#ifndef _SP2Packet_h_
#define _SP2Packet_h_

class CPacket;
class CQueryData;
class CQueryResultData;
struct GAMESERVERINFO;

class SP2Packet : public CPacket
{
	public:
	SP2Packet();
	SP2Packet(DWORD ID);
	SP2Packet(char *buffer,int size);
	virtual ~SP2Packet();

	const char* GetData() const;
	int GetDataSize() const;
	void SetDataAdd( char *buffer, int size );
	void  SetPosBegin();

	//operator
	SP2Packet&  operator =  (SP2Packet& packet);
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
	SP2Packet&  operator << ( const ioHashString &arg );
	SP2Packet&  operator << ( const Vector3 &arg );
	SP2Packet&  operator << (CQueryData &arg);
	SP2Packet&  operator << (CQueryResultData &arg);
	SP2Packet&	operator << ( const GAMESERVERINFO& arg );
	SP2Packet&	operator << ( const MAINSERVERINFO& arg );
		
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
	SP2Packet&  operator >> ( ioHashString &arg );
	SP2Packet&  operator >> ( Vector3 &arg );
	SP2Packet&  operator >> (CQueryData &arg);
	SP2Packet&  operator >> (CQueryResultData &arg);
	SP2Packet&	operator >> ( GAMESERVERINFO& arg );
	SP2Packet&	operator >> ( MAINSERVERINFO& arg );
 
	SP2Packet&	operator << (LoginServerInfo_ &data)
	{
		if( !CheckLeftPacketSize( sizeof(data) ) ) return *this;
		memcpy(&m_pBuffer[m_currentPos],&data,sizeof(data));
		m_currentPos += sizeof(data);
		*m_packet_header.m_Size = m_currentPos;
		return *this;
	}
 
	SP2Packet&  operator >> (LoginServerInfo_ &data)
	{ 
		if( !CheckRightPacketSize( sizeof(data) ) ) return *this;
		memcpy(&data,&m_pBuffer[m_currentPos],sizeof(data));
		m_currentPos += sizeof(data);
		return *this;
	}

};
#endif