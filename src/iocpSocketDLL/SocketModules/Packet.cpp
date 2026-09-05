#include "../iocpSocketDLL.h"
#include "Packet.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPacket::CPacket()
{
	Clear();
}

CPacket::CPacket(DWORD ID)
{
	Clear();
	*m_packet_header.m_ID = ID;
	m_currentPos = sizeof(PACKETHEADER);
}

CPacket::CPacket(char *buffer,int size) 
{
	Clear();
	memcpy( &m_pBuffer[0], buffer, min( MAX_BUFFER, size ) );
	m_currentPos = sizeof(PACKETHEADER);
}

CPacket::CPacket( bool bClear, PacketFlowTypes pkflowType, int iSize )
{
	UNREFERENCED_PARAMETER(iSize);
	UNREFERENCED_PARAMETER(pkflowType);

	m_bClear = bClear;
	Clear();
	m_currentPos = PK_HEADER_SIZE;
}

CPacket::CPacket( DWORD ID, bool bClear, PacketFlowTypes pkflowType, int iSize )
{
	UNREFERENCED_PARAMETER(iSize);
	UNREFERENCED_PARAMETER(pkflowType);

	m_bClear = bClear;
	Clear();
	*m_packet_header.m_ID = ID;
	m_currentPos = PK_HEADER_SIZE;
}

CPacket::~CPacket()
{

}

void CPacket::Clear()
{
	/*if( m_bClear )
		ZeroMemory(m_pBuffer, MAX_BUFFER);
	else
		ZeroMemory(m_pBuffer, PK_HEADER_SIZE);*/

	//HRYOON 국내와 동일하도록 수정
	memset(m_pBuffer,0,MAX_BUFFER);

	m_currentPos = 0;
	m_packet_header.m_ID		= (DWORD*)&m_pBuffer[PK_ID_ADDR];
	m_packet_header.m_Size		= (DWORD*)&m_pBuffer[PK_SIZE_ADDR];
	m_packet_header.m_CheckSum  = (DWORD*)&m_pBuffer[PK_CKSUM_ADDR];
	m_packet_header.m_iState    = (int*)&m_pBuffer[PK_FSM_ADDR];
	*m_packet_header.m_Size		= sizeof(PACKETHEADER);
}

void CPacket::SetBufferCopy(const char *pBuf,int size)
{
	Clear();
	memcpy( &m_pBuffer[0], pBuf, min( MAX_BUFFER, size ) );
	m_currentPos = sizeof(PACKETHEADER);
}

void CPacket::SetBufferCopy(const char *pBuf,int size,int pos)
{
	Clear();
	memcpy( &m_pBuffer[0], pBuf, min( MAX_BUFFER, size ) );
	m_currentPos = pos;
}

void CPacket::SetCheckSum( DWORD dwSum )
{
	*m_packet_header.m_CheckSum = dwSum;
}

void CPacket::SetState( int iState )
{
	*m_packet_header.m_iState = iState;
}

bool CPacket::IsValidHeader()
{
	return (GetBufferSize() >= sizeof(PACKETHEADER));
}

bool CPacket::IsValidPacket()
{
	if(!IsValidHeader()) return false;

	return (GetBufferSize() >= m_currentPos);
}

bool CPacket::CheckLeftPacketSize( int iAddSize )
{	
	if( m_currentPos + iAddSize >= MAX_BUFFER )
	{
		PrintTimeAndLog( 0,"[ID:%d - SIZE:%d] << Packat Size Overflow!", GetPacketID(), m_currentPos + iAddSize );
		return false;
	}

	return true;
}

bool CPacket::CheckRightPacketSize( int iAddSize )
{	
	if( m_currentPos + iAddSize >= MAX_BUFFER )
	{
		PrintTimeAndLog( 0,"[ID:%d - SIZE:%d] >> Packat Size Overflow!", GetPacketID(), m_currentPos + iAddSize );
		return false;
	}

	return true;
}

//operator
CPacket& CPacket::operator = (CPacket &packet)
{
	Clear();
	memcpy(&m_pBuffer[0],packet.GetBuffer(),packet.GetBufferSize());
	m_currentPos = packet.GetCurPos();

	return *this;
}

CPacket&  CPacket::operator << (bool arg)
{
	if( !CheckLeftPacketSize( sizeof(bool) ) ) return *this;

	memcpy(&m_pBuffer[m_currentPos],&arg,sizeof(bool));
	m_currentPos += sizeof(bool);
	*m_packet_header.m_Size = m_currentPos;
	return *this;
}

CPacket&  CPacket::operator << (int arg)
{
	if( !CheckLeftPacketSize( sizeof(int) ) ) return *this;

	memcpy(&m_pBuffer[m_currentPos],&arg,sizeof(int));
	m_currentPos += sizeof(int);
	*m_packet_header.m_Size = m_currentPos;
	return *this;
}

CPacket&  CPacket::operator << (LONG arg)
{
	if( !CheckLeftPacketSize( sizeof(LONG) ) ) return *this;

	memcpy(&m_pBuffer[m_currentPos],&arg,sizeof(LONG));
	m_currentPos += sizeof(LONG);
	*m_packet_header.m_Size = m_currentPos;
	return *this;
}

CPacket&  CPacket::operator << (DWORD arg)
{
	if( !CheckLeftPacketSize( sizeof(DWORD) ) ) return *this;

	memcpy(&m_pBuffer[m_currentPos],&arg,sizeof(DWORD));
	m_currentPos += sizeof(DWORD);
	*m_packet_header.m_Size = m_currentPos;
	return *this;
}

CPacket&  CPacket::operator << (__int64 arg)
{
	if( !CheckLeftPacketSize( sizeof(__int64) ) ) return *this;

	memcpy(&m_pBuffer[m_currentPos],&arg,sizeof(__int64));
	m_currentPos += sizeof(__int64);
	*m_packet_header.m_Size = m_currentPos;
	return *this;
}

CPacket&  CPacket::operator << (LPTSTR arg)
{
	int nlen = lstrlen(arg) * sizeof( TCHAR ) + sizeof( TCHAR );
	
	if( !CheckLeftPacketSize( nlen ) ) return *this;

	memcpy(&m_pBuffer[m_currentPos],arg,nlen);
	m_currentPos += nlen;
	*m_packet_header.m_Size = m_currentPos;
	return *this;
}

CPacket&  CPacket::operator << (double arg)
{
	if( !CheckLeftPacketSize( sizeof(double) ) ) return *this;

	memcpy(&m_pBuffer[m_currentPos],&arg,sizeof(double));
	m_currentPos += sizeof(double);
	*m_packet_header.m_Size = m_currentPos;
	return *this;
}

CPacket&  CPacket::operator << (float arg)
{
	if( !CheckLeftPacketSize( sizeof(float) ) ) return *this;

	memcpy(&m_pBuffer[m_currentPos],&arg,sizeof(float));
	m_currentPos += sizeof(float);
	*m_packet_header.m_Size = m_currentPos;
	return *this;
}

CPacket&  CPacket::operator >> (bool &arg)
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

CPacket&  CPacket::operator >> (int &arg)
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

CPacket&  CPacket::operator >> (LONG &arg)
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

CPacket&  CPacket::operator >> (DWORD &arg)
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

CPacket&  CPacket::operator >> (__int64 &arg)
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

CPacket&  CPacket::operator >> (LPTSTR arg)
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

CPacket&  CPacket::operator >> (double &arg)
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

CPacket&  CPacket::operator >> (float &arg)
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

//////////////////////////////////////////////////////////////////////
// Safety operator
//////////////////////////////////////////////////////////////////////

bool CPacket::Write(bool arg)
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

bool CPacket::Write(int arg)
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

bool CPacket::Write(LONG arg)
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

bool CPacket::Write(DWORD arg)
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

bool CPacket::Write(__int64 arg)
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

bool CPacket::Write(LPTSTR arg)
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

bool CPacket::Write(double arg)
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

bool CPacket::Write(float arg)
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

bool CPacket::Read(bool& arg)
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

bool CPacket::Read(int& arg)
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

bool CPacket::Read(LONG& arg)
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

bool CPacket::Read(DWORD& arg)
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

bool CPacket::Read(__int64& arg)
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

bool CPacket::Read(const int nLength, LPTSTR arg)
{
	int nLen = lstrlen((LPTSTR)&m_pBuffer[m_currentPos]) * sizeof( TCHAR ) + sizeof( TCHAR );

	if( !CheckRightPacketSize( nLen ) )
	{
		arg = NULL;
		//PrintTimeAndLog( 0,"[ID:%d - TYPE:STR, SIZE:%d] >> packet overflow", GetPacketID(), m_currentPos );
		return false;
	}

	if( nLen < nLength )
	{
		memcpy(arg, &m_pBuffer[m_currentPos], nLen);
		arg[nLen] = NULL;
		m_currentPos += nLen;
		return true;
	}
	return false;
}

bool CPacket::Read(double& arg)
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

bool CPacket::Read(float& arg)
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