

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

SP2Packet::SP2Packet( DWORD ID )
: CPacket( ID )
{
}

SP2Packet::SP2Packet( char *buffer, int size )
: CPacket( buffer, size )
{
}

SP2Packet::SP2Packet( char *szIP, DWORD dwPort, DWORD dwUserIndex, int iIndex, const SP2Packet &rhs )
{
	Clear();

	*m_packet_header.m_ID = rhs.GetPacketID();
	m_currentPos = sizeof(PACKETHEADER);

	*this << StrToDwordIP(szIP) << dwPort << dwUserIndex << iIndex;

	SetDataAdd( (char*)rhs.GetData(), rhs.GetDataSize() );
}

SP2Packet::SP2Packet( char *szIP, DWORD dwPort, DWORD dwUserIndex, int iIndex, DWORD dwNpcIndex, const SP2Packet &rhs )
{
	Clear();

	*m_packet_header.m_ID = rhs.GetPacketID();
	m_currentPos = sizeof(PACKETHEADER);

	*this << StrToDwordIP(szIP) << dwPort << dwUserIndex << iIndex;
	*this << dwNpcIndex;


	SetDataAdd( (char*)rhs.GetData(), rhs.GetDataSize() );
}

SP2Packet::~SP2Packet()
{
}

void SP2Packet::MovePointer( DWORD dwMoveBytes )
{
	m_currentPos += dwMoveBytes;
	m_currentPos = min( m_currentPos, GetBufferSize() );
}

DWORD SP2Packet::StrToDwordIP( char *iip )
{
	int  count       = 0;
	int  cut_ip		 = 0;
	char szCut_ip[4][4];
	memset(szCut_ip,0,sizeof(szCut_ip));
	int  len	     = strlen(iip);
	for(int i = 0;i < len;i++)
	{
		if(iip[i] == '.')
		{
			count = 0;
			cut_ip++;
		}
		else
			szCut_ip[cut_ip][count++] = iip[i];
	}
	return (DWORD)(atoi(szCut_ip[0])<<24) | (DWORD)(atoi(szCut_ip[1])<<16) | (DWORD)(atoi(szCut_ip[2])<<8) | atoi(szCut_ip[3]);	
}

void SP2Packet::DwordToStrIP( DWORD dwIP, char *iip )
{
	sprintf(iip,"%d.%d.%d.%d",(dwIP & 0xff000000)>>24,(dwIP & 0x00ff0000)>>16,(dwIP & 0x0000ff00)>>8,(dwIP & 0xff));
}

void SP2Packet::InitCurPos()
{
	m_currentPos = sizeof(PACKETHEADER);
}

bool SP2Packet::Write( WORD nLength, BYTE* arg )
{
	if( !CheckLeftPacketSize( nLength ) ) return false;

	memcpy(&m_pBuffer[m_currentPos],arg,nLength);
	m_currentPos += nLength;
	*m_packet_header.m_Size = m_currentPos;

	return true;
}

bool SP2Packet::Read( bool &arg )
{
	int nLength = sizeof(bool);
	if( !CheckRightPacketSize( nLength ) )
	{
		LOG.PrintTimeAndLog( 0, "[ERROR] Packet Read bool[%x]", GetPacketID() );
		arg = 0;
		return false;
	}

	memcpy(&arg,&m_pBuffer[m_currentPos],nLength );
	m_currentPos += nLength;

	return true;
}

bool SP2Packet::Read( BYTE &arg )
{
	int nLength = sizeof(BYTE);
	if( !CheckRightPacketSize( nLength ) )
	{
		LOG.PrintTimeAndLog( 0, "[ERROR] Packet Read BYTE[%x]", GetPacketID() );
		arg = 0;
		return false;
	}

	memcpy(&arg,&m_pBuffer[m_currentPos],nLength );
	m_currentPos += nLength;

	return true;
}

bool SP2Packet::Read( INT64 &arg )
{
	int nLength = sizeof(INT64);
	if( !CheckRightPacketSize( nLength ) )
	{
		LOG.PrintTimeAndLog( 0, "[ERROR] Packet Read INT64[%x]", GetPacketID() );
		arg = 0;
		return false;
	}

	memcpy(&arg,&m_pBuffer[m_currentPos],nLength );
	m_currentPos += nLength;

	return true;
}

bool SP2Packet::Read( int &arg )
{
	int nLength = sizeof(int);
	if( !CheckRightPacketSize( nLength ) )
	{
		LOG.PrintTimeAndLog( 0, "[ERROR] Packet Read int[%x]", GetPacketID() );
		arg = 0;
		return false;
	}

	memcpy(&arg,&m_pBuffer[m_currentPos],nLength );
	m_currentPos += nLength;

	return true;
}

bool SP2Packet::Read( long &arg )
{
	int nLength = sizeof(long);
	if( !CheckRightPacketSize( nLength ) )
	{
		LOG.PrintTimeAndLog( 0, "[ERROR] Packet Read long[%x]", GetPacketID() );
		arg = 0;
		return false;
	}

	memcpy(&arg,&m_pBuffer[m_currentPos],nLength );
	m_currentPos += nLength;

	return true;
}

bool SP2Packet::Read( float &arg )
{
	int nLength = sizeof(float);
	if( !CheckRightPacketSize( nLength ) )
	{
		LOG.PrintTimeAndLog( 0, "[ERROR] Packet Read float[%x]", GetPacketID() );
		arg = 0;
		return false;
	}

	memcpy(&arg,&m_pBuffer[m_currentPos],nLength );
	m_currentPos += nLength;

	return true;
}

bool SP2Packet::Read( WORD &arg )
{
	int nLength = sizeof(WORD);
	if( !CheckRightPacketSize( nLength ) )
	{
		LOG.PrintTimeAndLog( 0, "[ERROR] Packet Read WORD[%x]", GetPacketID() );
		arg = 0;
		return false;
	}

	memcpy(&arg,&m_pBuffer[m_currentPos],nLength );
	m_currentPos += nLength;

	return true;
}

bool SP2Packet::Read( DWORD &arg )
{
	int nLength = sizeof(DWORD);
	if( !CheckRightPacketSize( nLength ) )
	{
		LOG.PrintTimeAndLog( 0, "[ERROR] Packet Read DWORD[%x]", GetPacketID() );
		arg = 0;
		return false;
	}

	memcpy(&arg,&m_pBuffer[m_currentPos],nLength );
	m_currentPos += nLength;

	return true;
}

bool SP2Packet::Read( short &arg )
{
	int nLength = sizeof(short);
	if( !CheckRightPacketSize( nLength ) )
	{
		LOG.PrintTimeAndLog( 0, "[ERROR] Packet Read short[%x]", GetPacketID() );
		arg = 0;
		return false;
	}

	memcpy(&arg,&m_pBuffer[m_currentPos],nLength );
	m_currentPos += nLength;

	return true;
}

bool SP2Packet::Read( ioHashString &arg )
{
	int nlen = lstrlen((LPTSTR)&m_pBuffer[m_currentPos]) * sizeof( TCHAR ) + sizeof( TCHAR );

	if( !CheckRightPacketSize( nlen ) )
	{
		LOG.PrintTimeAndLog( 0, "[ERROR] Packet Read HashString[%x]", GetPacketID() );
		return false;
	}

	arg = &m_pBuffer[m_currentPos];
	m_currentPos += nlen;

	return true;
}

bool SP2Packet::Read( LPTSTR arg )
{
	int nlen = lstrlen((LPTSTR)&m_pBuffer[m_currentPos]) * sizeof( TCHAR ) + sizeof( TCHAR );

	if( !CheckRightPacketSize( nlen ) )
	{
		LOG.PrintTimeAndLog( 0, "[ERROR] Packet Read LPTSTR[%x]", GetPacketID() );
		return false;
	}

	memcpy(arg,&m_pBuffer[m_currentPos],nlen);
	m_currentPos += nlen;

	return true;
}

bool SP2Packet::Read(double &arg)
{
	int nLength = sizeof(double);
	if( !CheckRightPacketSize( nLength ) )
	{
		LOG.PrintTimeAndLog( 0, "[ERROR] Packet Read double[%x]", GetPacketID() );
		arg = 0;
		return false;
	}

	memcpy(&arg,&m_pBuffer[m_currentPos],nLength );
	m_currentPos += nLength;

	return true;
}
bool SP2Packet::Read( D3DXVECTOR2 &arg )
{
	int nLength = sizeof(D3DXVECTOR2);
	if( !CheckRightPacketSize( nLength ) )
	{
		LOG.PrintTimeAndLog( 0, "[ERROR] Packet Read D3DXVECTOR2[%x]", GetPacketID() );
		arg = D3DXVECTOR2(0.0f, 0.0f);
		return false;
	}

	memcpy(&arg,&m_pBuffer[m_currentPos],nLength );
	m_currentPos += nLength;

	return true;
}
bool SP2Packet::Read( D3DXVECTOR3 &arg )
{
	int nLength = sizeof(D3DXVECTOR3);
	if( !CheckRightPacketSize( nLength ) )
	{
		LOG.PrintTimeAndLog( 0, "[ERROR] Packet Read D3DXVECTOR3[%x]", GetPacketID() );
		arg = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		return false;
	}

	memcpy(&arg,&m_pBuffer[m_currentPos],nLength );
	m_currentPos += nLength;

	return true;
}
bool SP2Packet::Read( D3DXQUATERNION &arg )
{
	int nLength = sizeof(D3DXQUATERNION);
	if( !CheckRightPacketSize( nLength ) )
	{
		LOG.PrintTimeAndLog( 0, "[ERROR] Packet Read D3DXQUATERNION[%x]", GetPacketID() );
		arg = D3DXQUATERNION(0.0f, 0.0f, 0.0f, 0.0f);
		return false;
	}

	memcpy(&arg,&m_pBuffer[m_currentPos],nLength );
	m_currentPos += nLength;

	return true;
}
bool SP2Packet::Read( CHARACTER &arg )
{
	arg.ApplyData( *this );

	return true;
}
bool SP2Packet::Read(CEncrypt<BYTE> &arg)
{
	int nLength = sizeof(BYTE);
	if( !CheckRightPacketSize( nLength ) )
	{
		LOG.PrintTimeAndLog( 0, "[ERROR] Packet Read E_BYTE[%x]", GetPacketID() );
		arg = 0;
		return false;
	}

	BYTE decryptValue = 0;
	memcpy(&decryptValue,&m_pBuffer[m_currentPos],nLength );
	m_currentPos += nLength;
	arg = decryptValue;

	return true;
}
bool SP2Packet::Read(CEncrypt<bool> &arg)
{
	int nLength = sizeof(bool);
	if( !CheckRightPacketSize( nLength ) )
	{
		LOG.PrintTimeAndLog( 0, "[ERROR] Packet Read E_bool[%x]", GetPacketID() );
		arg = 0;
		return false;
	}

	bool decryptValue = false;
	memcpy(&decryptValue,&m_pBuffer[m_currentPos],nLength );
	m_currentPos += nLength;
	arg = decryptValue;

	return true;
}
bool SP2Packet::Read(CEncrypt<int> &arg)
{
	int nLength = sizeof(int);
	if( !CheckRightPacketSize( nLength ) )
	{
		LOG.PrintTimeAndLog( 0, "[ERROR] Packet Read E_int[%x]", GetPacketID() );
		arg = 0;
		return false;
	}

	int decryptValue = 0;
	memcpy(&decryptValue,&m_pBuffer[m_currentPos],nLength );
	m_currentPos += nLength;
	arg = decryptValue;

	return true;
}
bool SP2Packet::Read(CEncrypt<long> &arg)
{
	int nLength = sizeof(long);
	if( !CheckRightPacketSize( nLength ) )
	{
		LOG.PrintTimeAndLog( 0, "[ERROR] Packet Read E_long[%x]", GetPacketID() );
		arg = 0;
		return false;
	}

	long decryptValue = 0;
	memcpy(&decryptValue,&m_pBuffer[m_currentPos],nLength );
	m_currentPos += nLength;
	arg = decryptValue;

	return true;
}
bool SP2Packet::Read(CEncrypt<WORD> &arg)
{
	int nLength = sizeof(WORD);
	if( !CheckRightPacketSize( nLength ) )
	{
		LOG.PrintTimeAndLog( 0, "[ERROR] Packet Read E_WORD[%x]", GetPacketID() );
		arg = 0;
		return false;
	}

	WORD decryptValue = 0;
	memcpy(&decryptValue,&m_pBuffer[m_currentPos],nLength );
	m_currentPos += nLength;
	arg = decryptValue;

	return true;
}
bool SP2Packet::Read(CEncrypt<DWORD> &arg)
{
	int nLength = sizeof(DWORD);
	if( !CheckRightPacketSize( nLength ) )
	{
		LOG.PrintTimeAndLog( 0, "[ERROR] Packet Read E_DWORD[%x]", GetPacketID() );
		arg = 0;
		return false;
	}

	DWORD decryptValue = 0;
	memcpy(&decryptValue,&m_pBuffer[m_currentPos],nLength );
	m_currentPos += nLength;
	arg = decryptValue;

	return true;
}
bool SP2Packet::Read(CEncrypt<__int64> &arg)
{
	int nLength = sizeof(__int64);
	if( !CheckRightPacketSize( nLength ) )
	{
		LOG.PrintTimeAndLog( 0, "[ERROR] Packet Read E_int64[%x]", GetPacketID() );
		arg = 0;
		return false;
	}

	__int64 decryptValue = 0;
	memcpy(&decryptValue,&m_pBuffer[m_currentPos],nLength );
	m_currentPos += nLength;
	arg = decryptValue;

	return true;
}
bool SP2Packet::Read(CEncrypt<double> &arg)
{
	int nLength = sizeof(double);
	if( !CheckRightPacketSize( nLength ) )
	{
		LOG.PrintTimeAndLog( 0, "[ERROR] Packet Read E_double[%x]", GetPacketID() );
		arg = 0;
		return false;
	}

	double decryptValue = 0;
	memcpy(&decryptValue,&m_pBuffer[m_currentPos],nLength );
	m_currentPos += nLength;
	arg = decryptValue;

	return true;
}
bool SP2Packet::Read(CEncrypt<float> &arg)
{
	int nLength = sizeof(float);
	if( !CheckRightPacketSize( nLength ) )
	{
		LOG.PrintTimeAndLog( 0, "[ERROR] Packet Read E_float[%x]", GetPacketID() );
		arg = 0;
		return false;
	}

	float decryptValue = 0.f;
	memcpy(&decryptValue,&m_pBuffer[m_currentPos],nLength );
	m_currentPos += nLength;
	arg = decryptValue;

	return true;
}
bool SP2Packet::Read( FilePacket &arg )
{
	int nLength = sizeof(FilePacket);
	if( !CheckRightPacketSize( nLength ) )
	{
		LOG.PrintTimeAndLog( 0, "[ERROR] Packet Read FilePacket[%x]", GetPacketID() );
		ZeroMemory( arg.m_FilePacket, sizeof( arg.m_FilePacket ) );
		return false;
	}

	memcpy(&arg,&m_pBuffer[m_currentPos],nLength );
	m_currentPos += nLength;

	return true;
}
bool SP2Packet::Read( ControlKeys &arg )
{
	int nLength = sizeof(ControlKeys);
	if( !CheckRightPacketSize( nLength ) )
	{
		LOG.PrintTimeAndLog( 0, "[ERROR] Packet Read ControlKeys[%x]", GetPacketID() );
		arg.Clear();
		return false;
	}

	memcpy(&arg,&m_pBuffer[m_currentPos],nLength );
	m_currentPos += nLength;

	return true;
}

bool SP2Packet::Read( WORD nLength, BYTE* arg )
{
	if( !CheckRightPacketSize( nLength ) )
	{
		LOG.PrintTimeAndLog( 0, "[ERROR] Packet Read nByte[%x]", GetPacketID() );
		arg = 0;
		return false;
	}

	memcpy(arg,&m_pBuffer[m_currentPos],nLength );
	m_currentPos += nLength;

	return true;
}

//operator
SP2Packet& SP2Packet::operator = ( const SP2Packet &packet )
{
	Clear();
	memcpy( m_pBuffer, packet.GetBuffer(), packet.GetBufferSize() );
	m_currentPos = packet.m_currentPos;

	return *this;
}

SP2Packet&  SP2Packet::operator << (BYTE arg)
{
	if( !CheckLeftPacketSize( sizeof(BYTE) ) )
		return *this;

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

SP2Packet& SP2Packet::operator << ( const char *arg )
{
	int nlen = lstrlen(arg) + 1;

	if( !CheckLeftPacketSize( nlen ) ) return *this;

	memcpy(&m_pBuffer[m_currentPos],arg,nlen);
	m_currentPos += nlen;
	*m_packet_header.m_Size = m_currentPos;

	return *this;
}

SP2Packet& SP2Packet::operator << ( const ioHashString &arg )
{
	int nlen = arg.Length() + 1;

	if( !CheckLeftPacketSize( nlen ) )
		return *this;

	memcpy( &m_pBuffer[m_currentPos], arg.c_str(), nlen - 1 );
	m_pBuffer[m_currentPos + nlen - 1 ] = '\0';

	m_currentPos += nlen;
	*m_packet_header.m_Size = m_currentPos;

	return *this;
}

SP2Packet& SP2Packet::operator << ( const D3DXVECTOR2 &arg )
{
	if( !CheckLeftPacketSize( sizeof(D3DXVECTOR2) ) ) return *this;

	memcpy( &m_pBuffer[m_currentPos], &arg, sizeof(D3DXVECTOR2) );
	m_currentPos += sizeof(D3DXVECTOR2);
	*m_packet_header.m_Size = m_currentPos;

	return *this;
}

SP2Packet& SP2Packet::operator << ( const D3DXVECTOR3 &arg )
{
	if( !CheckLeftPacketSize( sizeof(D3DXVECTOR3) ) ) return *this;

	memcpy( &m_pBuffer[m_currentPos], &arg, sizeof(D3DXVECTOR3) );
	m_currentPos += sizeof(D3DXVECTOR3);
	*m_packet_header.m_Size = m_currentPos;

	return *this;
}

SP2Packet& SP2Packet::operator << ( const D3DXQUATERNION &arg )
{
	if( !CheckLeftPacketSize( sizeof(D3DXQUATERNION) ) ) return *this;

	memcpy( &m_pBuffer[m_currentPos], &arg, sizeof(D3DXQUATERNION) );
	m_currentPos += sizeof(D3DXQUATERNION);
	*m_packet_header.m_Size = m_currentPos;

	return *this;
}

SP2Packet& SP2Packet::operator << ( CHARACTER &arg )
{
	arg.FillData( *this );
	return *this;
}

SP2Packet& SP2Packet::operator << ( const VOICEDATA &arg )
{
	if( !CheckLeftPacketSize( sizeof(VOICEDATA) ) ) return *this;

	memcpy(&m_pBuffer[m_currentPos],&arg,sizeof(VOICEDATA));
	m_currentPos += sizeof(VOICEDATA);
	*m_packet_header.m_Size = m_currentPos;

	return *this;
}

SP2Packet& SP2Packet::operator << ( const CustomSoundPacket &arg )
{
	int iSize  = sizeof( arg.m_dwWrittenSize );
	int iTotal = arg.m_dwWrittenSize + iSize;
	if( !CheckLeftPacketSize( iTotal ) ) return *this;

	memcpy(&m_pBuffer[m_currentPos],&arg.m_dwWrittenSize, iSize );
	m_currentPos += iSize;
	memcpy(&m_pBuffer[m_currentPos],arg.m_Data,arg.m_dwWrittenSize);
	m_currentPos += arg.m_dwWrittenSize;
	*m_packet_header.m_Size = m_currentPos;
	return *this;
}

SP2Packet& SP2Packet::operator << (CEncrypt<BYTE> &arg)
{
	if( !CheckLeftPacketSize( sizeof(BYTE) ) ) return *this;

	BYTE decryptValue = arg;
	memcpy(&m_pBuffer[m_currentPos],&decryptValue,sizeof(BYTE));
	m_currentPos += sizeof(BYTE);
	*m_packet_header.m_Size = m_currentPos;
	return *this;
}

SP2Packet& SP2Packet::operator << (CEncrypt<bool> &arg)
{
	if( !CheckLeftPacketSize( sizeof(bool) ) ) return *this;

	bool decryptValue = arg;
	memcpy(&m_pBuffer[m_currentPos],&decryptValue,sizeof(bool));
	m_currentPos += sizeof(bool);
	*m_packet_header.m_Size = m_currentPos;
	return *this;
}

SP2Packet& SP2Packet::operator << (CEncrypt<int> &arg)
{
	if( !CheckLeftPacketSize( sizeof(int) ) ) return *this;

	int decryptValue = arg;
	memcpy(&m_pBuffer[m_currentPos],&decryptValue,sizeof(int));
	m_currentPos += sizeof(int);
	*m_packet_header.m_Size = m_currentPos;
	return *this;
}

SP2Packet& SP2Packet::operator << (CEncrypt<long> &arg)
{
	if( !CheckLeftPacketSize( sizeof(long) ) ) return *this;

	long decryptValue = arg;
	memcpy(&m_pBuffer[m_currentPos],&decryptValue,sizeof(long));
	m_currentPos += sizeof(long);
	*m_packet_header.m_Size = m_currentPos;
	return *this;
}

SP2Packet& SP2Packet::operator << (CEncrypt<WORD> &arg)
{
	if( !CheckLeftPacketSize( sizeof(WORD) ) ) return *this;

	WORD decryptValue = arg;
	memcpy(&m_pBuffer[m_currentPos],&decryptValue,sizeof(WORD));
	m_currentPos += sizeof(WORD);
	*m_packet_header.m_Size = m_currentPos;
	return *this;
}

SP2Packet& SP2Packet::operator << (CEncrypt<DWORD> &arg)
{
	if( !CheckLeftPacketSize( sizeof(DWORD) ) ) return *this;

	DWORD decryptValue = arg;
	memcpy(&m_pBuffer[m_currentPos],&decryptValue,sizeof(DWORD));
	m_currentPos += sizeof(DWORD);
	*m_packet_header.m_Size = m_currentPos;
	return *this;
}

SP2Packet& SP2Packet::operator << (CEncrypt<__int64> &arg)
{
	if( !CheckLeftPacketSize( sizeof(__int64) ) ) return *this;

	__int64 decryptValue = arg;
	memcpy(&m_pBuffer[m_currentPos],&decryptValue,sizeof(__int64));
	m_currentPos += sizeof(__int64);
	*m_packet_header.m_Size = m_currentPos;
	return *this;
}

SP2Packet& SP2Packet::operator << (CEncrypt<double> &arg)
{
	if( !CheckLeftPacketSize( sizeof(double) ) ) return *this;

	double decryptValue = arg;
	memcpy(&m_pBuffer[m_currentPos],&decryptValue,sizeof(double));
	m_currentPos += sizeof(double);
	*m_packet_header.m_Size = m_currentPos;
	return *this;
}

SP2Packet&  SP2Packet::operator << (CEncrypt<float> &arg)
{
	if( !CheckLeftPacketSize( sizeof(float) ) ) return *this;

	float decryptValue = arg;
	memcpy(&m_pBuffer[m_currentPos],&decryptValue,sizeof(float));
	m_currentPos += sizeof(float);
	*m_packet_header.m_Size = m_currentPos;
	return *this;
}

#ifdef XTRAP
SP2Packet& SP2Packet::operator << ( const XtrapPacket &arg )
{
	if( !CheckLeftPacketSize( sizeof(XtrapPacket) ) ) return *this;

	memcpy(&m_pBuffer[m_currentPos],&arg,sizeof(XtrapPacket));
	m_currentPos += sizeof(XtrapPacket);
	*m_packet_header.m_Size = m_currentPos;

	return *this;
}
#endif 
SP2Packet& SP2Packet::operator << ( const FilePacket &arg )
{
	if( !CheckLeftPacketSize( sizeof(FilePacket) ) ) return *this;

	memcpy(&m_pBuffer[m_currentPos],&arg,sizeof(FilePacket));
	m_currentPos += sizeof(FilePacket);
	*m_packet_header.m_Size = m_currentPos;

	return *this;
}

SP2Packet& SP2Packet::operator << ( const ControlKeys &arg )
{
	if( !CheckLeftPacketSize( sizeof(ControlKeys) ) ) return *this;

	memcpy(&m_pBuffer[m_currentPos],&arg,sizeof(ControlKeys));
	m_currentPos += sizeof(ControlKeys);
	*m_packet_header.m_Size = m_currentPos;

	return *this;
}

#ifdef NPROTECT
#ifdef NPROTECT_CSAUTH3
SP2Packet& SP2Packet::operator << ( const NProtectPacket &arg )
{
	if( !CheckLeftPacketSize( arg.m_dwStructSize ) ) return *this;

	memcpy(&m_pBuffer[m_currentPos],&arg,arg.m_dwStructSize);
	m_currentPos += arg.m_dwStructSize;
	*m_packet_header.m_Size = m_currentPos;

	return *this;
}
#else
SP2Packet& SP2Packet::operator << ( const GG_AUTH_DATA &arg )
{
	if( !CheckLeftPacketSize( sizeof(GG_AUTH_DATA) ) ) return *this;

	memcpy(&m_pBuffer[m_currentPos],&arg,sizeof(GG_AUTH_DATA));
	m_currentPos += sizeof(GG_AUTH_DATA);
	*m_packet_header.m_Size = m_currentPos;

	return *this;
}
#endif 
#endif // NPROTECT

#ifdef XIGNCODE
SP2Packet& SP2Packet::operator << ( const XignCodePacket &arg )
{
	if( !CheckLeftPacketSize( sizeof(XignCodePacket) ) ) return *this;

	memcpy(&m_pBuffer[m_currentPos],&arg,sizeof(XignCodePacket));
	m_currentPos += sizeof(XignCodePacket);
	*m_packet_header.m_Size = m_currentPos;

	return *this;
}
#endif
#ifdef HACKSHIELD
SP2Packet& SP2Packet::operator << ( const HackShieldPacket &arg )
{
	if( !CheckLeftPacketSize( sizeof(HackShieldPacket) ) ) return *this;

	memcpy(&m_pBuffer[m_currentPos],&arg,sizeof(HackShieldPacket));
	m_currentPos += sizeof(HackShieldPacket);
	*m_packet_header.m_Size = m_currentPos;

	return *this;
}
#endif
//-----------------------------------------------------------------
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

SP2Packet& SP2Packet::operator >> ( D3DXVECTOR2 &arg )
{
	if( !CheckRightPacketSize( sizeof(D3DXVECTOR2) ) ) return *this;

	memcpy(&arg,&m_pBuffer[m_currentPos],sizeof(D3DXVECTOR2));
	m_currentPos += sizeof(D3DXVECTOR2);

	return *this;
}

SP2Packet& SP2Packet::operator >> ( D3DXVECTOR3 &arg )
{
	if( !CheckRightPacketSize( sizeof(D3DXVECTOR3) ) ) return *this;

	memcpy(&arg,&m_pBuffer[m_currentPos],sizeof(D3DXVECTOR3));
	m_currentPos += sizeof(D3DXVECTOR3);

	return *this;
}

SP2Packet& SP2Packet::operator >> ( D3DXQUATERNION &arg )
{
	if( !CheckRightPacketSize( sizeof(D3DXQUATERNION) ) ) return *this;

	memcpy(&arg,&m_pBuffer[m_currentPos],sizeof(D3DXQUATERNION));
	m_currentPos += sizeof(D3DXQUATERNION);

	return *this;
}

SP2Packet& SP2Packet::operator >> ( CHARACTER &arg )
{
	arg.ApplyData( *this );
	return *this;
}

SP2Packet& SP2Packet::operator >> ( VOICEDATA &arg )
{
	if( !CheckRightPacketSize( sizeof(VOICEDATA) ) ) return *this;

	memcpy(&arg,&m_pBuffer[m_currentPos],sizeof(VOICEDATA));
	m_currentPos += sizeof(VOICEDATA);

	return *this;
}

SP2Packet& SP2Packet::operator >> ( CustomSoundPacket &arg )
{
	int iSize  = sizeof( arg.m_dwWrittenSize );
	if( !CheckRightPacketSize( iSize ) ) return *this;
	memcpy(&arg.m_dwWrittenSize,&m_pBuffer[m_currentPos], iSize);
	m_currentPos += iSize;

	if( !CheckRightPacketSize( arg.m_dwWrittenSize ) ) return *this;
	memcpy(arg.m_Data,&m_pBuffer[m_currentPos],arg.m_dwWrittenSize);
	m_currentPos += arg.m_dwWrittenSize;
	return *this;
}

SP2Packet& SP2Packet::operator >> (CEncrypt<BYTE> &arg)
{
	if( !CheckRightPacketSize( sizeof(BYTE) ) ) return *this;

	BYTE decryptValue = 0;
	memcpy(&decryptValue,&m_pBuffer[m_currentPos],sizeof(BYTE));
	m_currentPos += sizeof(BYTE);
	arg = decryptValue;

	return *this;
}

SP2Packet& SP2Packet::operator >> (CEncrypt<bool> &arg)
{
	if( !CheckRightPacketSize( sizeof(bool) ) ) return *this;

	bool decryptValue = false;
	memcpy(&decryptValue,&m_pBuffer[m_currentPos],sizeof(bool));
	m_currentPos += sizeof(bool);
	arg = decryptValue;

	return *this;
}

SP2Packet& SP2Packet::operator >> (CEncrypt<int> &arg)
{
	if( !CheckRightPacketSize( sizeof(int) ) ) return *this;

	int decryptValue = 0;
	memcpy(&decryptValue,&m_pBuffer[m_currentPos],sizeof(int));
	m_currentPos += sizeof(int);
	arg = decryptValue;

	return *this;
}

SP2Packet& SP2Packet::operator >> (CEncrypt<long> &arg)
{
	if( !CheckRightPacketSize( sizeof(long) ) ) return *this;

	long decryptValue = 0;
	memcpy(&decryptValue,&m_pBuffer[m_currentPos],sizeof(long));
	m_currentPos += sizeof(long);
	arg = decryptValue;

	return *this;
}

SP2Packet& SP2Packet::operator >> (CEncrypt<WORD> &arg)
{
	if( !CheckRightPacketSize( sizeof(WORD) ) ) return *this;

	WORD decryptValue = 0;
	memcpy(&decryptValue,&m_pBuffer[m_currentPos],sizeof(WORD));
	m_currentPos += sizeof(WORD);
	arg = decryptValue;

	return *this;
}

SP2Packet& SP2Packet::operator >> (CEncrypt<DWORD> &arg)
{
	if( !CheckRightPacketSize( sizeof(DWORD) ) ) return *this;

	DWORD decryptValue = 0;
	memcpy(&decryptValue,&m_pBuffer[m_currentPos],sizeof(DWORD));
	m_currentPos += sizeof(DWORD);
	arg = decryptValue;

	return *this;
}

SP2Packet& SP2Packet::operator >> (CEncrypt<__int64> &arg)
{
	if( !CheckRightPacketSize( sizeof(__int64) ) ) return *this;

	__int64 decryptValue = 0;
	memcpy(&decryptValue,&m_pBuffer[m_currentPos],sizeof(__int64));
	m_currentPos += sizeof(__int64);
	arg = decryptValue;

	return *this;
}

SP2Packet& SP2Packet::operator >> (CEncrypt<double> &arg)
{
	if( !CheckRightPacketSize( sizeof(double) ) ) return *this;

	double decryptValue = 0.0f;
	memcpy(&decryptValue,&m_pBuffer[m_currentPos],sizeof(double));
	m_currentPos += sizeof(double);
	arg = decryptValue;

	return *this;
}

SP2Packet&  SP2Packet::operator >> (CEncrypt<float> &arg)
{
	if( !CheckRightPacketSize( sizeof(float) ) ) return *this;

	float decryptValue = 0.0f;
	memcpy(&decryptValue,&m_pBuffer[m_currentPos],sizeof(float));
	m_currentPos += sizeof(float);
	arg = decryptValue;

	return *this;
}

#ifdef XTRAP
SP2Packet& SP2Packet::operator >> ( XtrapPacket &arg )
{
	if( !CheckRightPacketSize( sizeof(XtrapPacket) ) ) return *this;

	memcpy(&arg,&m_pBuffer[m_currentPos],sizeof(XtrapPacket));
	m_currentPos += sizeof(XtrapPacket);

	return *this;
}
#endif 
SP2Packet& SP2Packet::operator >> ( FilePacket &arg )
{
	if( !CheckRightPacketSize( sizeof(FilePacket) ) ) return *this;

	memcpy(&arg,&m_pBuffer[m_currentPos],sizeof(FilePacket));
	m_currentPos += sizeof(FilePacket);

	return *this;
}

SP2Packet& SP2Packet::operator >> ( ControlKeys &arg )
{
	if( !CheckRightPacketSize( sizeof(ControlKeys) ) ) return *this;

	memcpy(&arg,&m_pBuffer[m_currentPos],sizeof(ControlKeys));
	m_currentPos += sizeof(ControlKeys);

	return *this;
}

#ifdef NPROTECT
#ifdef NPROTECT_CSAUTH3
SP2Packet& SP2Packet::operator >> ( NProtectPacket &arg )
{
	DWORD dwStructSize = 0;
	memcpy(&dwStructSize,&m_pBuffer[m_currentPos],sizeof(DWORD));

	if( !CheckRightPacketSize( dwStructSize ) ) return *this; 
	arg.m_dwStructSize = dwStructSize;
	memcpy(&arg, &m_pBuffer[m_currentPos], dwStructSize );
	m_currentPos += dwStructSize;

	return *this;
}
#else
SP2Packet& SP2Packet::operator >> ( GG_AUTH_DATA &arg )
{
	if( !CheckRightPacketSize( sizeof(GG_AUTH_DATA) ) ) return *this;

	memcpy(&arg,&m_pBuffer[m_currentPos],sizeof(GG_AUTH_DATA));
	m_currentPos += sizeof(GG_AUTH_DATA);

	return *this;
}
#endif // NPROTECT
#endif

#ifdef XIGNCODE
SP2Packet& SP2Packet::operator >> ( XignCodePacket &arg )
{
	if( !CheckRightPacketSize( sizeof(XignCodePacket) ) ) return *this;

	memcpy(&arg,&m_pBuffer[m_currentPos],sizeof(XignCodePacket));
	m_currentPos += sizeof(XignCodePacket);

	return *this;
}
#endif 

#ifdef HACKSHIELD
SP2Packet& SP2Packet::operator >> ( HackShieldPacket &arg )
{
	if( !CheckRightPacketSize( sizeof(HackShieldPacket) ) ) return *this;

	memcpy(&arg,&m_pBuffer[m_currentPos],sizeof(HackShieldPacket));
	m_currentPos += sizeof(HackShieldPacket);

	return *this;
}
#endif 