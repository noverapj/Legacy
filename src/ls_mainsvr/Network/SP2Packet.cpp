#include "../stdafx.h"
#include "SP2Packet.h"
#include "../QueryData/QueryData.h"
#include "../QueryData/QueryResultData.h"

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

SP2Packet& SP2Packet::operator << ( const GAMESERVERINFO& arg )
{
	if( ! CheckLeftPacketSize( sizeof( GAMESERVERINFO ) ) )
		return *this;

	memcpy( &m_pBuffer[ m_currentPos ], &arg, sizeof( GAMESERVERINFO ) );
	m_currentPos += sizeof( GAMESERVERINFO );
	*m_packet_header.m_Size	= m_currentPos;

	return *this;
}

SP2Packet& SP2Packet::operator << ( const MAINSERVERINFO& arg )
{
	if( ! CheckLeftPacketSize( sizeof( MAINSERVERINFO ) ) )
		return *this;

	memcpy( &m_pBuffer[ m_currentPos ], &arg, sizeof( MAINSERVERINFO ) );
	m_currentPos += sizeof( MAINSERVERINFO );
	*m_packet_header.m_Size	= m_currentPos;

	return *this;
}

SP2Packet&  SP2Packet::operator >> (BYTE &arg)
{
	if( !CheckRightPacketSize( sizeof(BYTE) ) )
	{
		arg = 0;
		return *this;
	}

	memcpy(&arg,&m_pBuffer[m_currentPos],sizeof(BYTE));
	m_currentPos += sizeof(BYTE);
	
	return *this;
}

SP2Packet&  SP2Packet::operator >> (bool &arg)
{
	if( !CheckRightPacketSize( sizeof(bool) ) )
	{
		arg = false;
		return *this;
	}

	memcpy(&arg,&m_pBuffer[m_currentPos],sizeof(bool));
	m_currentPos += sizeof(bool);

	return *this;
}

SP2Packet&  SP2Packet::operator >> (int &arg)
{
	if( !CheckRightPacketSize( sizeof(int) ) )
	{
		arg = 0;
		return *this;
	}

	memcpy(&arg,&m_pBuffer[m_currentPos],sizeof(int));
	m_currentPos += sizeof(int);
	
	return *this;
}

SP2Packet&  SP2Packet::operator >> (LONG &arg)
{
	if( !CheckRightPacketSize( sizeof(LONG) ) )
	{
		arg = 0;
		return *this;
	}

	memcpy(&arg,&m_pBuffer[m_currentPos],sizeof(LONG));
	m_currentPos += sizeof(LONG);
	
	return *this;
}

SP2Packet&  SP2Packet::operator >> (WORD &arg)
{
	if( !CheckRightPacketSize( sizeof(WORD) ) )
	{
		arg = 0;
		return *this;
	}

	memcpy(&arg,&m_pBuffer[m_currentPos],sizeof(WORD));
	m_currentPos += sizeof(WORD);
	
	return *this;
}

SP2Packet&  SP2Packet::operator >> (DWORD &arg)
{
	if( !CheckRightPacketSize( sizeof(DWORD) ) )
	{
		arg = 0;
		return *this;
	}

	memcpy(&arg,&m_pBuffer[m_currentPos],sizeof(DWORD));
	m_currentPos += sizeof(DWORD);
	
	return *this;
}

SP2Packet&  SP2Packet::operator >> (__int64 &arg)
{	
	if( !CheckRightPacketSize( sizeof(__int64) ) )
	{
		arg = 0;
		return *this;
	}
	
	memcpy(&arg,&m_pBuffer[m_currentPos],sizeof(__int64));
	m_currentPos += sizeof(__int64);
	
	return *this;
}

SP2Packet&  SP2Packet::operator >> (LPTSTR arg)
{
	int nlen = lstrlen((LPTSTR)&m_pBuffer[m_currentPos]) * sizeof( TCHAR ) + sizeof( TCHAR );

	if( !CheckRightPacketSize( nlen ) )
	{
		arg = NULL;
		return *this;
	}

	memcpy(arg,&m_pBuffer[m_currentPos],nlen);
	m_currentPos += nlen;
	
	return *this;
}

SP2Packet&  SP2Packet::operator >> (double &arg)
{
	if( !CheckRightPacketSize( sizeof(double) ) )
	{
		arg = 0;
		return *this;
	}

	memcpy(&arg,&m_pBuffer[m_currentPos],sizeof(double));
	m_currentPos += sizeof(double);
	
	return *this;
}

SP2Packet&  SP2Packet::operator >> (float &arg)
{
	if( !CheckRightPacketSize( sizeof(float) ) )
	{
		arg = 0;
		return *this;
	}

	memcpy(&arg,&m_pBuffer[m_currentPos],sizeof(float));
	m_currentPos += sizeof(float);
	
	return *this;
}

SP2Packet&  SP2Packet::operator >> (short &arg)
{
	if( !CheckRightPacketSize( sizeof(short) ) )
	{
		arg = 0;
		return *this;
	}

	memcpy(&arg,&m_pBuffer[m_currentPos],sizeof(short));
	m_currentPos += sizeof(short);

	return *this;
}

SP2Packet& SP2Packet::operator >> ( ioHashString &arg )
{
	int nlen = lstrlen((LPTSTR)&m_pBuffer[m_currentPos]) * sizeof( TCHAR ) + sizeof( TCHAR );

	if( !CheckRightPacketSize( nlen ) )
	{
		arg.Clear();
		return *this;
	}

	arg = &m_pBuffer[m_currentPos];
	m_currentPos += nlen;
	
	return *this;
}

SP2Packet& SP2Packet::operator >> ( Vector3 &arg )
{
	if( !CheckRightPacketSize( sizeof(Vector3) ) )
	{
		ZeroMemory( &arg, sizeof( Vector3 ) );
		return *this;
	}

	memcpy(&arg,&m_pBuffer[m_currentPos],sizeof(Vector3));
	m_currentPos += sizeof(Vector3);

	return *this;
}

SP2Packet& SP2Packet::operator >> ( Quaternion &arg )
{
	if( !CheckRightPacketSize( sizeof(Quaternion) ) )
	{
		ZeroMemory( &arg, sizeof( Quaternion ) );
		return *this;
	}

	memcpy(&arg,&m_pBuffer[m_currentPos],sizeof(Quaternion));
	m_currentPos += sizeof(Quaternion);

	return *this;
}

SP2Packet&  SP2Packet::operator >> (CQueryData &arg)
{
	if( !CheckRightPacketSize( sizeof(QueryHeader) + arg.GetBufferSize() ) )
	{
		ZeroMemory( &arg, sizeof( CQueryData ) );
		return *this;
	}

	arg.Deserialize(&m_pBuffer[m_currentPos]);
	m_currentPos += sizeof(QueryHeader);
	m_currentPos += arg.GetBufferSize();
	*m_packet_header.m_Size = m_currentPos;
	return *this;
}

SP2Packet&  SP2Packet::operator >> (CQueryResultData &arg)
{
	if( !CheckRightPacketSize( sizeof(QueryResultHeader) + arg.GetResultBufferSize() ) )
	{
		ZeroMemory( &arg, sizeof( CQueryResultData ) );
		return *this;
	}

	arg.Deserialize(&m_pBuffer[m_currentPos]);
	m_currentPos += sizeof(QueryResultHeader);
	m_currentPos += arg.GetResultBufferSize();
	*m_packet_header.m_Size = m_currentPos;
	return *this;
}

SP2Packet& SP2Packet::operator >> ( GAMESERVERINFO& arg )
{
	if( ! CheckRightPacketSize( sizeof( GAMESERVERINFO ) ) )
	{
		ZeroMemory( &arg, sizeof( GAMESERVERINFO ) );
		return *this;
	}

	memcpy( &arg, &m_pBuffer[ m_currentPos ], sizeof( GAMESERVERINFO ) );
	m_currentPos += sizeof( GAMESERVERINFO );

	return *this;
}

SP2Packet& SP2Packet::operator >> ( MAINSERVERINFO& arg )
{
	if( ! CheckRightPacketSize( sizeof( MAINSERVERINFO ) ) )
	{
		ZeroMemory( &arg, sizeof( MAINSERVERINFO ) );
		return *this;
	}

	memcpy( &arg, &m_pBuffer[ m_currentPos ], sizeof( MAINSERVERINFO ) );
	m_currentPos += sizeof( MAINSERVERINFO );

	return *this;
}
SP2Packet& SP2Packet::operator>>( SendRelayInfo_& arg )
{
	if( !CheckRightPacketSize( sizeof(SendRelayInfo_) ) )
	{
		ZeroMemory( &arg, sizeof( SendRelayInfo_ ) );
		return *this;
	}

	memcpy(&arg,&m_pBuffer[m_currentPos],sizeof(SendRelayInfo_));
	m_currentPos += sizeof(SendRelayInfo_);
	return *this;
}
SP2Packet& SP2Packet::operator<<( SendRelayInfo_& arg )
{
	if( !CheckLeftPacketSize( sizeof(SendRelayInfo_) ) ) return *this;

	memcpy(&m_pBuffer[m_currentPos],&arg,sizeof(SendRelayInfo_));
	m_currentPos += sizeof(SendRelayInfo_);
	*m_packet_header.m_Size = m_currentPos;
	return *this;
}

bool SP2Packet::Write(bool arg)
{
	if( !CheckLeftPacketSize( sizeof(bool) ) ) 
	{
		//PrintTimeAndLog( 0,"[ID:%d - TYPE:bool, SIZE:%d] << packet overflow", GetPacketID(), m_currentPos );
		return false;
	}

	memcpy(&m_pBuffer[m_currentPos], &arg, sizeof(bool));
	m_currentPos			+= sizeof(bool);
	*m_packet_header.m_Size = m_currentPos;
	return true;
}

bool SP2Packet::Write(int arg)
{
	if( !CheckLeftPacketSize( sizeof(int) ) )
	{
		//PrintTimeAndLog( 0,"[ID:%d - TYPE:int, SIZE:%d] << packet overflow", GetPacketID(), m_currentPos );
		return false;
	}

	memcpy(&m_pBuffer[m_currentPos], &arg, sizeof(int));
	m_currentPos			+= sizeof(int);
	*m_packet_header.m_Size = m_currentPos;
	return true;
}

bool SP2Packet::Write(LONG arg)
{
	if( !CheckLeftPacketSize( sizeof(LONG) ) )
	{
		//PrintTimeAndLog( 0,"[ID:%d - TYPE:LONG, SIZE:%d] << packet overflow", GetPacketID(), m_currentPos );
		return false;
	}

	memcpy(&m_pBuffer[m_currentPos], &arg, sizeof(LONG));
	m_currentPos			+= sizeof(LONG);
	*m_packet_header.m_Size = m_currentPos;
	return true;
}

bool SP2Packet::Write(DWORD arg)
{
	if( !CheckLeftPacketSize( sizeof(DWORD) ) )
	{
		//PrintTimeAndLog( 0,"[ID:%d - TYPE:DWORD, SIZE:%d] << packet overflow", GetPacketID(), m_currentPos );
		return false;
	}

	memcpy(&m_pBuffer[m_currentPos], &arg, sizeof(DWORD));
	m_currentPos			+= sizeof(DWORD);
	*m_packet_header.m_Size = m_currentPos;
	return true;
}

bool SP2Packet::Write(__int64 arg)
{
	if( !CheckLeftPacketSize( sizeof(__int64) ) )
	{
		//PrintTimeAndLog( 0,"[ID:%d - TYPE:int64, SIZE:%d] << packet overflow", GetPacketID(), m_currentPos );
		return false;
	}

	memcpy(&m_pBuffer[m_currentPos], &arg, sizeof(__int64));
	m_currentPos			+= sizeof(__int64);
	*m_packet_header.m_Size = m_currentPos;
	return true;
}

bool SP2Packet::Write(LPTSTR arg)
{
	int nLen = lstrlen(arg) * sizeof( TCHAR ) + sizeof( TCHAR );
	if( !CheckLeftPacketSize( nLen ) )
	{
		//PrintTimeAndLog( 0,"[ID:%d - TYPE:STR, SIZE:%d] << packet overflow", GetPacketID(), m_currentPos );
		return false;
	}

	memcpy(&m_pBuffer[m_currentPos], arg, nLen);
	m_currentPos			+= nLen;
	*m_packet_header.m_Size = m_currentPos;
	return true;
}

bool SP2Packet::Write(double arg)
{
	if( !CheckLeftPacketSize( sizeof(double) ) )
	{
		//PrintTimeAndLog( 0,"[ID:%d - TYPE:double, SIZE:%d] << packet overflow", GetPacketID(), m_currentPos );
		return false;
	}

	memcpy(&m_pBuffer[m_currentPos], &arg, sizeof(double));
	m_currentPos			+= sizeof(double);
	*m_packet_header.m_Size = m_currentPos;
	return true;
}

bool SP2Packet::Write(float arg)
{
	if( !CheckLeftPacketSize( sizeof(float) ) )
	{
		//PrintTimeAndLog( 0,"[ID:%d - TYPE:float, SIZE:%d] << packet overflow", GetPacketID(), m_currentPos );
		return false;
	}

	memcpy(&m_pBuffer[m_currentPos], &arg, sizeof(float));
	m_currentPos			+= sizeof(float);
	*m_packet_header.m_Size = m_currentPos;
	return true;
}
bool SP2Packet::Write(const ioHashString &arg)
{
	int nLen = lstrlen( arg.c_str() ) + 1;
	if( !CheckLeftPacketSize( nLen ) )
	{
		//PrintTimeAndLog( 0,"[ID:%d - TYPE:ioHashString, SIZE:%d] << packet overflow", GetPacketID(), m_currentPos );
		return false;
	}

	memcpy( &m_pBuffer[m_currentPos], arg.c_str(), nLen );
	m_currentPos			+= nLen;
	*m_packet_header.m_Size = m_currentPos;
	return true;
}


bool SP2Packet::Write(CQueryData &arg)
{
	if( !CheckLeftPacketSize( sizeof(QueryHeader)+arg.GetBufferSize() ) )
	{
		//PrintTimeAndLog( 0,"[ID:%d - TYPE:CQueryData, SIZE:%d] << packet overflow", GetPacketID(), m_currentPos );
		return false;
	}

	memcpy(&m_pBuffer[m_currentPos],arg.GetHeader(),sizeof(QueryHeader));
	m_currentPos += sizeof(QueryHeader);
	memcpy(&m_pBuffer[m_currentPos],arg.GetBuffer(),arg.GetBufferSize());
	m_currentPos += arg.GetBufferSize();
	*m_packet_header.m_Size = m_currentPos;
	return true;
}

bool SP2Packet::Write(CQueryResultData &arg)
{
	if( !CheckLeftPacketSize( sizeof(QueryResultHeader)+arg.GetResultBufferSize() ) )
	{
		//PrintTimeAndLog( 0,"[ID:%d - TYPE:CQueryResultData, SIZE:%d] << packet overflow", GetPacketID(), m_currentPos );
		return false;
	}

	memcpy(&m_pBuffer[m_currentPos],arg.GetHeader(),sizeof(QueryResultHeader));
	m_currentPos += sizeof(QueryResultHeader);
	memcpy(&m_pBuffer[m_currentPos],arg.GetBuffer(),arg.GetResultBufferSize());
	m_currentPos += arg.GetResultBufferSize();
	*m_packet_header.m_Size = m_currentPos;
	return true;
}

bool SP2Packet::Read(bool& arg)
{
	if( !CheckRightPacketSize( sizeof(bool) ) )
	{
		arg = false;
		//PrintTimeAndLog( 0,"[ID:%d - TYPE:bool, SIZE:%d] >> packet overflow", GetPacketID(), m_currentPos );
		return false;
	}

	memcpy(&arg, &m_pBuffer[m_currentPos], sizeof(bool));
	m_currentPos += sizeof(bool);
	return true;
}

bool SP2Packet::Read(int& arg)
{
	if( !CheckRightPacketSize( sizeof(int) ) )
	{
		arg = 0;
		//PrintTimeAndLog( 0,"[ID:%d - TYPE:int, SIZE:%d] >> packet overflow", GetPacketID(), m_currentPos );
		return false;
	}

	memcpy(&arg, &m_pBuffer[m_currentPos], sizeof(int));
	m_currentPos += sizeof(int);
	return true;
}

bool SP2Packet::Read(LONG& arg)
{
	if( !CheckRightPacketSize( sizeof(LONG) ) )
	{
		arg = 0;
		//PrintTimeAndLog( 0,"[ID:%d - TYPE:LONG, SIZE:%d] >> packet overflow", GetPacketID(), m_currentPos );
		return false;
	}

	memcpy(&arg, &m_pBuffer[m_currentPos], sizeof(LONG));
	m_currentPos += sizeof(LONG);
	return true;
}

bool SP2Packet::Read(DWORD& arg)
{
	if( !CheckRightPacketSize( sizeof(DWORD) ) )
	{
		arg = 0;
		//PrintTimeAndLog( 0,"[ID:%d - TYPE:DWORD, SIZE:%d] >> packet overflow", GetPacketID(), m_currentPos );
		return false;
	}

	memcpy(&arg, &m_pBuffer[m_currentPos], sizeof(DWORD));
	m_currentPos += sizeof(DWORD);
	return true;
}

bool SP2Packet::Read(__int64& arg)
{
	if( !CheckRightPacketSize( sizeof(__int64) ) )
	{
		arg = 0;
		//PrintTimeAndLog( 0,"[ID:%d - TYPE:int64, SIZE:%d] >> packet overflow", GetPacketID(), m_currentPos );
		return false;
	}

	memcpy(&arg, &m_pBuffer[m_currentPos], sizeof(__int64));
	m_currentPos += sizeof(__int64);
	return true;
}

bool SP2Packet::Read(const int nLength, LPTSTR arg)
{
	int nLen = lstrlen((LPTSTR)&m_pBuffer[m_currentPos]) * sizeof( TCHAR ) + sizeof( TCHAR );

	if( !CheckRightPacketSize( nLen ) )
	{
		arg = NULL;
		//PrintTimeAndLog( 0,"[ID:%d - TYPE:STR, SIZE:%d] >> packet overflow", GetPacketID(), m_currentPos );
		return false;
	}

	if(nLen <= nLength)
	{
		memcpy(arg, &m_pBuffer[m_currentPos], nLen);
		arg[nLen] = NULL;
		m_currentPos += nLen;
		return true;
	}
	return false;
}

bool SP2Packet::Read(double& arg)
{
	if( !CheckRightPacketSize( sizeof(double) ) )
	{
		arg = 0;
		//PrintTimeAndLog( 0,"[ID:%d - TYPE:double, SIZE:%d] >> packet overflow", GetPacketID(), m_currentPos );
		return false;
	}

	memcpy(&arg, &m_pBuffer[m_currentPos], sizeof(double));
	m_currentPos += sizeof(double);
	return true;
}

bool SP2Packet::Read(float& arg)
{
	if( !CheckRightPacketSize( sizeof(float) ) )
	{
		arg = 0;
		//PrintTimeAndLog( 0,"[ID:%d - TYPE:float, SIZE:%d] >> packet overflow", GetPacketID(), m_currentPos );
		return false;
	}

	memcpy(&arg, &m_pBuffer[m_currentPos], sizeof(float));
	m_currentPos += sizeof(float);
	return true;
}

bool SP2Packet::Read(ioHashString& arg)
{
	int nLen = lstrlen((LPTSTR)&m_pBuffer[m_currentPos]) * sizeof(TCHAR) + sizeof(TCHAR);
	if( !CheckRightPacketSize( nLen ) )
	{
		arg.Clear();
		return false;
	}

	arg = &m_pBuffer[m_currentPos];
	m_currentPos += nLen;
	return true;
}

bool SP2Packet::Read(CQueryData& arg)
{
	if( !CheckRightPacketSize( sizeof(QueryHeader) + arg.GetBufferSize() ) )
	{
		ZeroMemory( &arg, sizeof( CQueryData ) );
		return false;
	}

	arg.Deserialize(&m_pBuffer[m_currentPos]);
	m_currentPos += sizeof(QueryHeader);
	m_currentPos += arg.GetBufferSize();
	*m_packet_header.m_Size = m_currentPos;
	return true;
}

bool SP2Packet::Read(CQueryResultData& arg)
{
	if( !CheckRightPacketSize( sizeof(QueryResultHeader) + arg.GetResultBufferSize() ) )
	{
		ZeroMemory( &arg, sizeof( CQueryResultData ) );
		return false;
	}

	arg.Deserialize(&m_pBuffer[m_currentPos]);
	m_currentPos += sizeof(QueryResultHeader);
	m_currentPos += arg.GetResultBufferSize();
	*m_packet_header.m_Size = m_currentPos;
	return true;
}
