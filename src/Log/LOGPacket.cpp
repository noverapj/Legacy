#include "stdafx.h"
#include "LOGPacket.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

LOGPacket::LOGPacket()
{
	Clear();
}

LOGPacket::LOGPacket(DWORD ID)
{
	Clear();
	*m_packet_header.m_ID = ID;
	m_currentPos = sizeof(PACKETHEADER);
}

LOGPacket::LOGPacket(char *buffer,int size) 
{
	Clear();
	memcpy( &m_pBuffer[0], buffer, min( MAX_BUFFER, size ) );
	m_currentPos = sizeof(PACKETHEADER);
}

LOGPacket::~LOGPacket()
{

}

void LOGPacket::Clear()
{
	memset(m_pBuffer,0,MAX_BUFFER);
	m_currentPos = 0;
	m_packet_header.m_ID		= (DWORD*)&m_pBuffer[PK_ID_ADDR];
	m_packet_header.m_Size		= (DWORD*)&m_pBuffer[PK_SIZE_ADDR];
	m_packet_header.m_CheckSum  = (DWORD*)&m_pBuffer[PK_CKSUM_ADDR];
	m_packet_header.m_iState    = (int*)&m_pBuffer[PK_FSM_ADDR];
	*m_packet_header.m_Size = sizeof(PACKETHEADER);
}

void LOGPacket::SetBufferCopy(const char *pBuf,int size)
{
	Clear();
	memcpy( &m_pBuffer[0], pBuf, min( MAX_BUFFER, size ) );
	m_currentPos = sizeof(PACKETHEADER);
}

void LOGPacket::SetBufferCopy(const char *pBuf,int size,int pos)
{
	Clear();
	memcpy( &m_pBuffer[0], pBuf, min( MAX_BUFFER, size ) );
	m_currentPos = pos;
}

void LOGPacket::SetCheckSum( DWORD dwSum )
{
	*m_packet_header.m_CheckSum = dwSum;
}

void LOGPacket::SetState( int iState )
{
	*m_packet_header.m_iState = iState;
}

bool LOGPacket::IsValidHeader()
{
	return (GetBufferSize() >= sizeof(PACKETHEADER));
}

bool LOGPacket::IsValidPacket()
{
	if(!IsValidHeader()) return false;

	return (GetBufferSize() >= m_currentPos);
}

bool LOGPacket::CheckLeftPacketSize( int iAddSize )
{	
	if( m_currentPos + iAddSize >= MAX_BUFFER )
	{
		//		PrintTimeAndLog( 0,"[ID:%d - SIZE:%d] << Packat Size Overflow!", GetPacketID(), m_currentPos + iAddSize );
		return false;
	}

	return true;
}

bool LOGPacket::CheckRightPacketSize( int iAddSize )
{	
	if( m_currentPos + iAddSize >= MAX_BUFFER )
	{
		//	PrintTimeAndLog( 0,"[ID:%d - SIZE:%d] >> Packat Size Overflow!", GetPacketID(), m_currentPos + iAddSize );
		return false;
	}

	return true;
}

//operator
LOGPacket& LOGPacket::operator = (LOGPacket &packet)
{
	Clear();
	memcpy(&m_pBuffer[0],packet.GetBuffer(),packet.GetBufferSize());
	m_currentPos = packet.GetCurPos();

	return *this;
}

LOGPacket&  LOGPacket::operator << (bool arg)
{
	if( !CheckLeftPacketSize( sizeof(bool) ) ) return *this;

	memcpy(&m_pBuffer[m_currentPos],&arg,sizeof(bool));
	m_currentPos += sizeof(bool);
	*m_packet_header.m_Size = m_currentPos;
	return *this;
}

LOGPacket&  LOGPacket::operator << (int arg)
{
	if( !CheckLeftPacketSize( sizeof(int) ) ) return *this;

	memcpy(&m_pBuffer[m_currentPos],&arg,sizeof(int));
	m_currentPos += sizeof(int);
	*m_packet_header.m_Size = m_currentPos;
	return *this;
}

LOGPacket&  LOGPacket::operator << (LONG arg)
{
	if( !CheckLeftPacketSize( sizeof(LONG) ) ) return *this;

	memcpy(&m_pBuffer[m_currentPos],&arg,sizeof(LONG));
	m_currentPos += sizeof(LONG);
	*m_packet_header.m_Size = m_currentPos;
	return *this;
}

LOGPacket&  LOGPacket::operator << (DWORD arg)
{
	if( !CheckLeftPacketSize( sizeof(DWORD) ) ) return *this;

	memcpy(&m_pBuffer[m_currentPos],&arg,sizeof(DWORD));
	m_currentPos += sizeof(DWORD);
	*m_packet_header.m_Size = m_currentPos;
	return *this;
}

LOGPacket&  LOGPacket::operator << (__int64 arg)
{
	if( !CheckLeftPacketSize( sizeof(__int64) ) ) return *this;

	memcpy(&m_pBuffer[m_currentPos],&arg,sizeof(__int64));
	m_currentPos += sizeof(__int64);
	*m_packet_header.m_Size = m_currentPos;
	return *this;
}

LOGPacket&  LOGPacket::operator << (LPTSTR arg)
{
	int nlen = lstrlen(arg) * sizeof( TCHAR ) + sizeof( TCHAR );

	if( !CheckLeftPacketSize( nlen ) ) return *this;

	memcpy(&m_pBuffer[m_currentPos],arg,nlen);
	m_currentPos += nlen;
	*m_packet_header.m_Size = m_currentPos;
	return *this;
}

LOGPacket&  LOGPacket::operator << (double arg)
{
	if( !CheckLeftPacketSize( sizeof(double) ) ) return *this;

	memcpy(&m_pBuffer[m_currentPos],&arg,sizeof(double));
	m_currentPos += sizeof(double);
	*m_packet_header.m_Size = m_currentPos;
	return *this;
}

LOGPacket&  LOGPacket::operator << (float arg)
{
	if( !CheckLeftPacketSize( sizeof(float) ) ) return *this;

	memcpy(&m_pBuffer[m_currentPos],&arg,sizeof(float));
	m_currentPos += sizeof(float);
	*m_packet_header.m_Size = m_currentPos;
	return *this;
}

LOGPacket&  LOGPacket::operator >> (bool &arg)
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

LOGPacket&  LOGPacket::operator >> (int &arg)
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

LOGPacket&  LOGPacket::operator >> (LONG &arg)
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

LOGPacket&  LOGPacket::operator >> (DWORD &arg)
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

LOGPacket&  LOGPacket::operator >> (__int64 &arg)
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

LOGPacket&  LOGPacket::operator >> (LPTSTR arg)
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

LOGPacket&  LOGPacket::operator >> (double &arg)
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

LOGPacket&  LOGPacket::operator >> (float &arg)
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

