#include "stdafx.h"
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

SP2Packet::SP2Packet(char *buffer,int size) : CPacket( buffer, size )
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

const char* SP2Packet::GetData() const
{
	return &m_pBuffer[0] + sizeof(PACKETHEADER);
}

int SP2Packet::GetDataSize() const
{
	return *m_packet_header.m_Size - sizeof(PACKETHEADER);
}

const char* SP2Packet::GetBuffer() const
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

SP2Packet& SP2Packet::operator << ( const ioHashString &arg )
{
	int nlen = lstrlen( arg.c_str() ) + 1;

	if( !CheckLeftPacketSize( nlen ) ) return *this;

	memcpy( &m_pBuffer[m_currentPos], arg.c_str(), nlen );
	m_currentPos += nlen;
	*m_packet_header.m_Size = m_currentPos;

	return *this;
}

SP2Packet& SP2Packet::operator << ( const Vector3 &arg )
{
	if( !CheckLeftPacketSize( sizeof(Vector3) ) ) return *this;

	memcpy(&m_pBuffer[m_currentPos],&arg,sizeof(Vector3));
	m_currentPos += sizeof(Vector3);
	*m_packet_header.m_Size = m_currentPos;

	return *this;
}

SP2Packet& SP2Packet::operator << ( const Quaternion &arg )
{
	if( !CheckLeftPacketSize( sizeof(Quaternion) ) ) return *this;

	memcpy(&m_pBuffer[m_currentPos],&arg,sizeof(Quaternion));
	m_currentPos += sizeof(Quaternion);
	*m_packet_header.m_Size = m_currentPos;

	return *this;
}

SP2Packet& SP2Packet::operator << ( const FilePacket &arg )
{
	if( !CheckLeftPacketSize( sizeof(FilePacket) ) ) return *this;

	memcpy(&m_pBuffer[m_currentPos],&arg,sizeof(FilePacket));
	m_currentPos += sizeof(FilePacket);
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

SP2Packet& SP2Packet::operator >> ( ioHashString &arg )
{
	int nlen = lstrlen((LPTSTR)&m_pBuffer[m_currentPos]) * sizeof( TCHAR ) + sizeof( TCHAR );

	if( !CheckRightPacketSize( nlen ) ) return *this;

	arg = &m_pBuffer[m_currentPos];
	m_currentPos += nlen;
	
	return *this;
}

SP2Packet& SP2Packet::operator >> ( Vector3 &arg )
{
	if( !CheckRightPacketSize( sizeof(Vector3) ) ) return *this;

	memcpy(&arg,&m_pBuffer[m_currentPos],sizeof(Vector3));
	m_currentPos += sizeof(Vector3);

	return *this;
}

SP2Packet& SP2Packet::operator >> ( Quaternion &arg )
{
	if( !CheckRightPacketSize( sizeof(Quaternion) ) ) return *this;

	memcpy(&arg,&m_pBuffer[m_currentPos],sizeof(Quaternion));
	m_currentPos += sizeof(Quaternion);

	return *this;
}

SP2Packet& SP2Packet::operator >> ( FilePacket &arg )
{
	if( !CheckRightPacketSize( sizeof(FilePacket) ) ) return *this;

	memcpy(&arg,&m_pBuffer[m_currentPos],sizeof(FilePacket));
	m_currentPos += sizeof(FilePacket);

	return *this;
}
