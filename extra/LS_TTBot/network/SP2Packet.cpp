#include "../stdafx.h"
#include "SP2Packet.h"

SP2Packet::SP2Packet()
{
}

SP2Packet::SP2Packet( const SP2Packet &rhs )
{
	Clear();
	memcpy( m_pBuffer, rhs.GetBuffer(), rhs.GetBufferSize() );
	m_currentPos = rhs.m_currentPos;
}

SP2Packet::SP2Packet(DWORD ID) : CPacket( ID )
{
}

SP2Packet::SP2Packet(const uint8* buffer,int size) : CPacket( buffer, size )
{
}

SP2Packet::SP2Packet( DWORD dwUserIndex, SP2Packet &rhs )
{
	Clear();

	*m_packet_header.m_ID = rhs.GetPacketID();
	m_currentPos = sizeof(PACKETHEADER);
	*this << dwUserIndex;
	SetDataAdd( (char*)rhs.GetData(), rhs.GetDataSize() );
}

SP2Packet::~SP2Packet()
{
}

const uint8* SP2Packet::GetData() const
{
	return &m_pBuffer[0] + sizeof(PACKETHEADER);
}

int SP2Packet::GetDataSize() const
{
	return *m_packet_header.m_Size - sizeof(PACKETHEADER);
}

const uint8* SP2Packet::GetBuffer() const
{
	return &m_pBuffer[0];
}

int SP2Packet::GetBufferSize() const
{
	return *m_packet_header.m_Size;
}

void SP2Packet::SetDataAdd( char *buffer, int size, bool bCurPosReSet )
{
	memcpy( &m_pBuffer[m_currentPos], buffer, size );
	m_currentPos += size;
	*m_packet_header.m_Size = m_currentPos;
	if( bCurPosReSet )
		m_currentPos = sizeof(PACKETHEADER);
}

void SP2Packet::SetDataAddCreateUDP( DWORD dwIP, DWORD dwPort, char *buffer, int size, bool bCurPosReSet /* = false  */ )
{
	*this << dwIP << dwPort;
	memcpy( &m_pBuffer[m_currentPos], buffer, size );
	m_currentPos += size;
	*m_packet_header.m_Size = m_currentPos;
	if( bCurPosReSet )
		m_currentPos = sizeof(PACKETHEADER);
}

void SP2Packet::MovePointer( DWORD dwMoveBytes )
{
	m_currentPos += dwMoveBytes;
	m_currentPos = min( m_currentPos, GetBufferSize() );
}

void SP2Packet::SetPosBegin()
{
	m_currentPos = sizeof(PACKETHEADER);
}


//operator
SP2Packet& SP2Packet::operator = (const SP2Packet &packet)
{
	Clear();
	memcpy(&m_pBuffer[0],packet.GetBuffer(),packet.GetBufferSize());
	m_currentPos = packet.m_currentPos;

	return *this;
}

SP2Packet&  SP2Packet::operator << (BYTE arg)
{
	if( !CheckLeftPacketSize( sizeof(BYTE) ) ) return *this;

	memcpy(&m_pBuffer[m_currentPos],&arg,sizeof(BYTE));
	m_currentPos += sizeof(BYTE);
	*m_packet_header.m_Size = m_currentPos;
	return *this;
}

SP2Packet&  SP2Packet::operator << (bool arg)
{
	if( !CheckLeftPacketSize( sizeof(bool) ) ) return *this;

	memcpy(&m_pBuffer[m_currentPos],&arg,sizeof(bool));
	m_currentPos += sizeof(bool);
	*m_packet_header.m_Size = m_currentPos;
	return *this;
}

SP2Packet&  SP2Packet::operator << (int arg)
{
	if( !CheckLeftPacketSize( sizeof(int) ) ) return *this;

	memcpy(&m_pBuffer[m_currentPos],&arg,sizeof(int));
	m_currentPos += sizeof(int);
	*m_packet_header.m_Size = m_currentPos;
	return *this;
}

SP2Packet&  SP2Packet::operator << (LONG arg)
{
	if( !CheckLeftPacketSize( sizeof(LONG) ) ) return *this;

	memcpy(&m_pBuffer[m_currentPos],&arg,sizeof(LONG));
	m_currentPos += sizeof(LONG);
	*m_packet_header.m_Size = m_currentPos;
	return *this;
}

SP2Packet&  SP2Packet::operator << (WORD arg)
{
	if( !CheckLeftPacketSize( sizeof(WORD) ) ) return *this;

	memcpy(&m_pBuffer[m_currentPos],&arg,sizeof(WORD));
	m_currentPos += sizeof(WORD);
	*m_packet_header.m_Size = m_currentPos;
	return *this;
}

SP2Packet&  SP2Packet::operator << (DWORD arg)
{
	if( !CheckLeftPacketSize( sizeof(DWORD) ) ) return *this;

	memcpy(&m_pBuffer[m_currentPos],&arg,sizeof(DWORD));
	m_currentPos += sizeof(DWORD);
	*m_packet_header.m_Size = m_currentPos;
	return *this;
}

SP2Packet&  SP2Packet::operator << (__int64 arg)
{
	if( !CheckLeftPacketSize( sizeof(__int64) ) ) return *this;

	memcpy(&m_pBuffer[m_currentPos],&arg,sizeof(__int64));
	m_currentPos += sizeof(__int64);
	*m_packet_header.m_Size = m_currentPos;
	return *this;
}

SP2Packet&  SP2Packet::operator << (LPTSTR arg)
{
	int nlen = lstrlen(arg) * sizeof( TCHAR ) + sizeof( TCHAR );

	if( !CheckLeftPacketSize( nlen ) ) return *this;

	memcpy(&m_pBuffer[m_currentPos],arg,nlen);
	m_currentPos += nlen;
	*m_packet_header.m_Size = m_currentPos;
	return *this;
}

SP2Packet&  SP2Packet::operator << (double arg)
{
	if( !CheckLeftPacketSize( sizeof(double) ) ) return *this;

	memcpy(&m_pBuffer[m_currentPos],&arg,sizeof(double));
	m_currentPos += sizeof(double);
	*m_packet_header.m_Size = m_currentPos;
	return *this;
}

SP2Packet&  SP2Packet::operator << (float arg)
{
	if( !CheckLeftPacketSize( sizeof(float) ) ) return *this;

	memcpy(&m_pBuffer[m_currentPos],&arg,sizeof(float));
	m_currentPos += sizeof(float);
	*m_packet_header.m_Size = m_currentPos;
	return *this;
}

SP2Packet&  SP2Packet::operator << (short arg)
{
	if( !CheckLeftPacketSize( sizeof(short) ) ) return *this;

	memcpy(&m_pBuffer[m_currentPos],&arg,sizeof(short));
	m_currentPos += sizeof(short);
	*m_packet_header.m_Size = m_currentPos;
	return *this;
}

//SP2Packet& SP2Packet::operator << ( CHARACTER &arg )
//{
//	arg.FillData( *this );
//	return *this;
//}

SP2Packet& SP2Packet::operator << ( const ioHashString &arg )
{
	int nlen = lstrlen( arg.c_str() ) + 1;

	if( !CheckLeftPacketSize( nlen ) ) return *this;

	memcpy( &m_pBuffer[m_currentPos], arg.c_str(), nlen );
	m_currentPos += nlen;
	*m_packet_header.m_Size = m_currentPos;

	return *this;
}

SP2Packet&  SP2Packet::operator << (CQueryData &arg)
{
	memcpy(&m_pBuffer[m_currentPos],arg.GetHeader(),sizeof(QueryHeader));
	m_currentPos += sizeof(QueryHeader);
	memcpy(&m_pBuffer[m_currentPos],arg.GetBuffer(),arg.GetBufferSize());
	m_currentPos += arg.GetBufferSize();
	*m_packet_header.m_Size = m_currentPos;
	return *this;
}

SP2Packet&  SP2Packet::operator << (CQueryResultData &arg)
{
	memcpy(&m_pBuffer[m_currentPos],arg.GetHeader(),sizeof(QueryResultHeader));
	m_currentPos += sizeof(QueryResultHeader);
	memcpy(&m_pBuffer[m_currentPos],arg.GetBuffer(),arg.GetResultBufferSize());
	m_currentPos += arg.GetResultBufferSize();
	*m_packet_header.m_Size = m_currentPos;
	return *this;
}


SP2Packet&  SP2Packet::operator >> (BYTE &arg)
{
	if( !CheckRightPacketSize( sizeof(BYTE) ) ) return *this;

	memcpy(&arg,&m_pBuffer[m_currentPos],sizeof(BYTE));
	m_currentPos += sizeof(BYTE);
	
	return *this;
}

SP2Packet&  SP2Packet::operator >> (bool &arg)
{
	if( !CheckRightPacketSize( sizeof(bool) ) ) return *this;

	memcpy(&arg,&m_pBuffer[m_currentPos],sizeof(bool));
	m_currentPos += sizeof(bool);

	return *this;
}

SP2Packet&  SP2Packet::operator >> (int &arg)
{
	if( !CheckRightPacketSize( sizeof(int) ) ) return *this;

	memcpy(&arg,&m_pBuffer[m_currentPos],sizeof(int));
	m_currentPos += sizeof(int);
	
	return *this;
}

SP2Packet&  SP2Packet::operator >> (LONG &arg)
{
	if( !CheckRightPacketSize( sizeof(LONG) ) ) return *this;

	memcpy(&arg,&m_pBuffer[m_currentPos],sizeof(LONG));
	m_currentPos += sizeof(LONG);
	
	return *this;
}

SP2Packet&  SP2Packet::operator >> (WORD &arg)
{
	if( !CheckRightPacketSize( sizeof(WORD) ) ) return *this;

	memcpy(&arg,&m_pBuffer[m_currentPos],sizeof(WORD));
	m_currentPos += sizeof(WORD);
	
	return *this;
}

SP2Packet&  SP2Packet::operator >> (DWORD &arg)
{
	if( !CheckRightPacketSize( sizeof(DWORD) ) ) return *this;

	memcpy(&arg,&m_pBuffer[m_currentPos],sizeof(DWORD));
	m_currentPos += sizeof(DWORD);
	
	return *this;
}

SP2Packet&  SP2Packet::operator >> (__int64 &arg)
{	
	if( !CheckRightPacketSize( sizeof(__int64) ) ) return *this;
	
	memcpy(&arg,&m_pBuffer[m_currentPos],sizeof(__int64));
	m_currentPos += sizeof(__int64);
	
	return *this;
}

SP2Packet&  SP2Packet::operator >> (LPTSTR arg)
{
	int nlen = lstrlen((LPTSTR)&m_pBuffer[m_currentPos]) * sizeof( TCHAR ) + sizeof( TCHAR );

	if( !CheckRightPacketSize( nlen ) ) return *this;

	memcpy(arg,&m_pBuffer[m_currentPos],nlen);
	m_currentPos += nlen;
	
	return *this;
}

SP2Packet&  SP2Packet::operator >> (double &arg)
{
	if( !CheckRightPacketSize( sizeof(double) ) ) return *this;

	memcpy(&arg,&m_pBuffer[m_currentPos],sizeof(double));
	m_currentPos += sizeof(double);
	
	return *this;
}

SP2Packet&  SP2Packet::operator >> (float &arg)
{
	if( !CheckRightPacketSize( sizeof(float) ) ) return *this;

	memcpy(&arg,&m_pBuffer[m_currentPos],sizeof(float));
	m_currentPos += sizeof(float);
	
	return *this;
}

SP2Packet&  SP2Packet::operator >> (short &arg)
{
	if( !CheckRightPacketSize( sizeof(short) ) ) return *this;

	memcpy(&arg,&m_pBuffer[m_currentPos],sizeof(short));
	m_currentPos += sizeof(short);

	return *this;
}

//SP2Packet&  SP2Packet::operator >> (CHARACTER &arg)
//{
//	arg.ApplyData( *this );
//
//	return *this;
//}

SP2Packet& SP2Packet::operator >> ( ioHashString &arg )
{
	int nlen = lstrlen((LPTSTR)&m_pBuffer[m_currentPos]) * sizeof( TCHAR ) + sizeof( TCHAR );

	if( !CheckRightPacketSize( nlen ) ) return *this;

	arg = reinterpret_cast<const char*>(&m_pBuffer[m_currentPos]);
	m_currentPos += nlen;
	
	return *this;
}

SP2Packet&  SP2Packet::operator >> (CQueryData &arg)
{
	arg.SetBuffer(&m_pBuffer[m_currentPos]);
	m_currentPos += sizeof(QueryHeader);
	m_currentPos += arg.GetBufferSize();
	*m_packet_header.m_Size = m_currentPos;
	return *this;
}

SP2Packet&  SP2Packet::operator >> (CQueryResultData &arg)
{
	arg.SetBuffer(&m_pBuffer[m_currentPos]);
	m_currentPos += sizeof(QueryResultHeader);
	m_currentPos += arg.GetResultBufferSize();
	*m_packet_header.m_Size = m_currentPos;
	return *this;
}
