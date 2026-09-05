#include "../stdafx.h"
#include "SP2Packet.h"
#include "../QueryData/QueryData.h"
#include "../QueryData/QueryResultData.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SP2Packet::SP2Packet()
{
}

SP2Packet::SP2Packet(DWORD ID) : CPacket( ID )
{
}

SP2Packet::SP2Packet(char *buffer,int size) : CPacket( buffer, size )
{
}
SP2Packet::~SP2Packet()
{
}

//////////////////////////////////////////////////////////////////////
// Operator
//////////////////////////////////////////////////////////////////////

SP2Packet& SP2Packet::operator = (SP2Packet &packet)
{
	Clear();
	memcpy(&m_pBuffer[0],packet.GetBuffer(),packet.GetBufferSize());
	m_currentPos = packet.GetCurPos();

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

bool SP2Packet::Write(LOGMessage& arg)
{
	if( !CheckLeftPacketSize( sizeof(arg) ) ) 
	{
		//PrintTimeAndLog( 0,"[ID:%d - TYPE:LOGMessage, SIZE:%d] << packet overflow", GetPacketID(), m_currentPos );
		return false;
	}

	memcpy(&m_pBuffer[m_currentPos], &arg, sizeof(arg));
	m_currentPos			+= sizeof(arg);
	*m_packet_header.m_Size = m_currentPos;
	return true;
}

bool SP2Packet::Write(LOGMessageHeader& arg)
{
	if( !CheckLeftPacketSize( sizeof(arg) ) )
	{
		//PrintTimeAndLog( 0,"[ID:%d - TYPE:LOGMessageHeader, SIZE:%d] << packet overflow", GetPacketID(), m_currentPos );
		return false;
	}

	memcpy(&m_pBuffer[m_currentPos], &arg, sizeof(arg));
	m_currentPos			+= sizeof(arg);
	*m_packet_header.m_Size = m_currentPos;
	return true;
}

bool SP2Packet::Write(SYSTEMTIME& arg)
{
	if( !CheckLeftPacketSize( sizeof(arg) ) )
	{
		//PrintTimeAndLog( 0,"[ID:%d - TYPE:SYSTEMTIME, SIZE:%d] << packet overflow", GetPacketID(), m_currentPos );
		return false;
	}

	memcpy(&m_pBuffer[m_currentPos], &arg, sizeof(arg));
	m_currentPos			+= sizeof(arg);
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

bool SP2Packet::Read(LOGMessage& arg)
{
	if( !CheckRightPacketSize( sizeof(arg) ) )
	{
		ZeroMemory( &arg, sizeof( LOGMessage ) );
		return false;
	}

	memcpy(&arg, &m_pBuffer[m_currentPos], sizeof(arg));
	m_currentPos += sizeof(arg);
	return true;
}

bool SP2Packet::Read(LOGMessageHeader& arg)
{
	if( !CheckRightPacketSize( sizeof(arg) ) )
	{
		ZeroMemory( &arg, sizeof( LOGMessageHeader ) );
		return false;
	}

	memcpy(&arg, &m_pBuffer[m_currentPos], sizeof(arg));
	m_currentPos += sizeof(arg);
	return true;
}


bool SP2Packet::Read(SYSTEMTIME& arg)
{
	if( !CheckRightPacketSize( sizeof(arg) ) )
	{
		ZeroMemory( &arg, sizeof( SYSTEMTIME ) );
		return false;
	}
	
	memcpy(&arg, &m_pBuffer[m_currentPos], sizeof(arg));
	m_currentPos += sizeof(arg);
	return true;
}

bool SP2Packet::Ref(QueryHeader& arg)
{
	if( !CheckRightPacketSize( sizeof(QueryHeader) ) )
	{
		ZeroMemory( &arg, sizeof( QueryHeader ) );
		return false;
	}

	memcpy(&arg, &m_pBuffer[m_currentPos], sizeof(arg));
	return true;
}
