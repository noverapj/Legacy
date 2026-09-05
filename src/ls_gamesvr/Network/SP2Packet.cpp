#include "stdafx.h"
#include "SP2Packet.h"

#include "../Util/ioHashString.h"
#include "../QueryData/QueryData.h"
#include "../QueryData/QueryResultData.h"
#include "../NodeInfo/NodeHelpStructDefine.h"
#include "../../iocpSocketDLL/SocketModules/Packet.h"


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

#ifdef ANTIHACK
SP2Packet::SP2Packet( bool bClear, PacketFlowTypes pkflowType, int iSize ) : CPacket(bClear, pkflowType, iSize)
{

}
SP2Packet::SP2Packet( DWORD ID, bool bClear, PacketFlowTypes pkflowType, int iSize ) : CPacket(ID, bClear, pkflowType, iSize)
{

}
#endif //ANTIHACK

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

SP2Packet& SP2Packet::operator << ( CHARACTER &arg )
{
	arg.FillData( *this );
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
SP2Packet& SP2Packet::operator << ( const AHNHS_TRANS_BUFFER &arg )
{
	if( !CheckLeftPacketSize( sizeof(AHNHS_TRANS_BUFFER) ) ) return *this;

	memcpy(&m_pBuffer[m_currentPos],&arg,sizeof(AHNHS_TRANS_BUFFER));
	m_currentPos += sizeof(AHNHS_TRANS_BUFFER);
	*m_packet_header.m_Size = m_currentPos;

	return *this;
}
#endif

#ifdef SRC_LATIN
SP2Packet& SP2Packet::operator << ( const ApexPacket &arg )
{
	if( !CheckLeftPacketSize( sizeof(ApexPacket) ) ) return *this;

	memcpy(&m_pBuffer[m_currentPos],&arg,sizeof(ApexPacket));
	m_currentPos += sizeof(ApexPacket);
	*m_packet_header.m_Size = m_currentPos;

	return *this;
}
#endif

SP2Packet& SP2Packet::operator << ( const ControlKeys &arg )
{
	if( !CheckLeftPacketSize( sizeof(ControlKeys) ) ) return *this;

	memcpy(&m_pBuffer[m_currentPos],&arg,sizeof(ControlKeys));
	m_currentPos += sizeof(ControlKeys);
	*m_packet_header.m_Size = m_currentPos;

	return *this;
}



SP2Packet& SP2Packet::operator << ( const MonitorStatusRequest &arg )
{
	if( !CheckLeftPacketSize( sizeof(MonitorStatusRequest) ) ) return *this;

	memcpy(&m_pBuffer[m_currentPos],&arg,sizeof(MonitorStatusRequest));
	m_currentPos += sizeof(MonitorStatusRequest);
	*m_packet_header.m_Size = m_currentPos;

	return *this;
}

SP2Packet& SP2Packet::operator << ( const MonitorStatusResult &arg )
{
	if( !CheckLeftPacketSize( sizeof(MonitorStatusResult) ) ) return *this;

	memcpy(&m_pBuffer[m_currentPos],&arg,sizeof(MonitorStatusResult));
	m_currentPos += sizeof(MonitorStatusResult);
	*m_packet_header.m_Size = m_currentPos;

	return *this;
}

SP2Packet& SP2Packet::operator << ( const MonitorChangeRequest &arg )
{
	if( !CheckLeftPacketSize( sizeof(MonitorChangeRequest) ) ) return *this;

	memcpy(&m_pBuffer[m_currentPos],&arg,sizeof(MonitorChangeRequest));
	m_currentPos += sizeof(MonitorChangeRequest);
	*m_packet_header.m_Size = m_currentPos;

	return *this;
}

SP2Packet& SP2Packet::operator << ( const MonitorChangeResult &arg )
{
	if( !CheckLeftPacketSize( sizeof(MonitorChangeResult) ) ) return *this;

	memcpy(&m_pBuffer[m_currentPos],&arg,sizeof(MonitorChangeResult));
	m_currentPos += sizeof(MonitorChangeResult);
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



SP2Packet& SP2Packet::operator<<( const sockaddr_in& arg )
{
	if( !CheckLeftPacketSize( sizeof(sockaddr_in) ) ) return *this;
	int size =sizeof(sockaddr_in);
	memcpy(&m_pBuffer[GetBufferSize()],&arg,sizeof(sockaddr_in));
	 
	*m_packet_header.m_Size += size;
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

SP2Packet&  SP2Packet::operator >> (CHARACTER &arg)
{
	if( !CheckRightPacketSize( sizeof( CHARACTER ) ) )
	{
		arg.Init();
		return *this;
	}

	arg.ApplyData( *this );

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

	arg.SetBuffer(&m_pBuffer[m_currentPos]);
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

	arg.SetBuffer(&m_pBuffer[m_currentPos]);
	m_currentPos += sizeof(QueryResultHeader);
	m_currentPos += arg.GetResultBufferSize();
	*m_packet_header.m_Size = m_currentPos;
	return *this;
}

#ifdef XTRAP
SP2Packet& SP2Packet::operator >> ( XtrapPacket &arg )
{
	if( !CheckRightPacketSize( sizeof(XtrapPacket) ) )
	{
		ZeroMemory( &arg, sizeof( XtrapPacket ) );
		return *this;
	}

	memcpy(&arg,&m_pBuffer[m_currentPos],sizeof(XtrapPacket));
	m_currentPos += sizeof(XtrapPacket);

	return *this;
}
#endif
#ifdef NPROTECT
#ifdef NPROTECT_CSAUTH3
SP2Packet& SP2Packet::operator >> ( NProtectPacket &arg )
{
	DWORD dwStructSize = 0;
	memcpy(&dwStructSize,&m_pBuffer[m_currentPos],sizeof(DWORD));

	if( !CheckRightPacketSize( dwStructSize ) )
	{
		ZeroMemory( &arg, sizeof( NProtectPacket ) );
		return *this;
	}
	arg.m_dwStructSize = dwStructSize;
	memcpy(&arg, &m_pBuffer[m_currentPos], dwStructSize );
	m_currentPos += dwStructSize;

	return *this;
}
#else
SP2Packet& SP2Packet::operator >> ( GG_AUTH_DATA &arg )
{
	if( !CheckRightPacketSize( sizeof(GG_AUTH_DATA) ) )
	{
		ZeroMemory( &arg, sizeof( GG_AUTH_DATA ) );
		return *this;
	}

	memcpy(&arg,&m_pBuffer[m_currentPos],sizeof(GG_AUTH_DATA));
	m_currentPos += sizeof(GG_AUTH_DATA);

	return *this;
}
#endif
#endif // NPROTECT
#ifdef XIGNCODE
SP2Packet& SP2Packet::operator >> ( XignCodePacket &arg )
{
	if( !CheckRightPacketSize( sizeof(XignCodePacket) ) )
	{
		ZeroMemory( &arg, sizeof( XignCodePacket ) );
		return *this;
	}

	memcpy(&arg,&m_pBuffer[m_currentPos],sizeof(XignCodePacket));
	m_currentPos += sizeof(XignCodePacket);

	return *this;
}
#endif 
#ifdef HACKSHIELD
SP2Packet& SP2Packet::operator >> ( AHNHS_TRANS_BUFFER &arg )
{
	if( !CheckRightPacketSize( sizeof(AHNHS_TRANS_BUFFER) ) )
	{
		ZeroMemory( &arg, sizeof( AHNHS_TRANS_BUFFER ) );
		return *this;
	}

	memcpy(&arg,&m_pBuffer[m_currentPos],sizeof(AHNHS_TRANS_BUFFER));
	m_currentPos += sizeof(AHNHS_TRANS_BUFFER);

	return *this;
}
#endif 
#ifdef SRC_LATIN
SP2Packet& SP2Packet::operator >> ( ApexPacket &arg )
{
	if( !CheckRightPacketSize( sizeof(ApexPacket) ) ) 
	{
		ZeroMemory( &arg, sizeof( ApexPacket ) );
		return *this;
	}

	memcpy(&arg,&m_pBuffer[m_currentPos],sizeof(ApexPacket));
	m_currentPos += sizeof(ApexPacket);

	return *this;
}
#endif 


SP2Packet& SP2Packet::operator >> ( ControlKeys &arg )
{
	if( !CheckRightPacketSize( sizeof(ControlKeys) ) )
	{
		arg.Clear();
		return *this;
	}

	memcpy(&arg,&m_pBuffer[m_currentPos],sizeof(ControlKeys));
	m_currentPos += sizeof(ControlKeys);

	return *this;
}

SP2Packet& SP2Packet::operator >> ( MonitorStatusRequest &arg )
{
	if( !CheckRightPacketSize( sizeof(MonitorStatusRequest) ) )
	{
		ZeroMemory( &arg, sizeof( MonitorStatusRequest ) );
		return *this;
	}

	memcpy(&arg,&m_pBuffer[m_currentPos],sizeof(MonitorStatusRequest));
	m_currentPos += sizeof(MonitorStatusRequest);

	return *this;
}

SP2Packet& SP2Packet::operator >> ( MonitorStatusResult &arg )
{
	if( !CheckRightPacketSize( sizeof(MonitorStatusResult) ) )
	{
		ZeroMemory( &arg, sizeof( MonitorStatusResult ) );
		return *this;
	}

	memcpy(&arg,&m_pBuffer[m_currentPos],sizeof(MonitorStatusResult));
	m_currentPos += sizeof(MonitorStatusResult);

	return *this;
}

SP2Packet& SP2Packet::operator >> ( MonitorChangeRequest &arg )
{
	if( !CheckRightPacketSize( sizeof(MonitorChangeRequest) ) )
	{
		ZeroMemory( &arg, sizeof( MonitorChangeRequest ) );
		return *this;
	}

	memcpy(&arg,&m_pBuffer[m_currentPos],sizeof(MonitorChangeRequest));
	m_currentPos += sizeof(MonitorChangeRequest);

	return *this;
}

SP2Packet& SP2Packet::operator >> ( MonitorChangeResult &arg )
{
	if( !CheckRightPacketSize( sizeof(MonitorChangeResult) ) )
	{
		ZeroMemory( &arg, sizeof( MonitorChangeResult ) );
		return *this;
	}

	memcpy(&arg,&m_pBuffer[m_currentPos],sizeof(MonitorChangeResult));
	m_currentPos += sizeof(MonitorChangeResult);

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

SP2Packet& SP2Packet::operator>>( sockaddr_in& arg )
{
	if( !CheckRightPacketSize( sizeof(sockaddr_in) ) )
		return *this;
	int npos = GetBufferSize();
	int nsize = sizeof(sockaddr_in);
	memcpy(&arg,&m_pBuffer[npos-nsize],nsize);
	MoveBufferPointer(-nsize);
	return *this;
}



void SP2Packet::MoveBufferPointer( int size )  
{
	 
	*m_packet_header.m_Size += size;

}

void SP2Packet::GetSockAddress( sockaddr_in& arg )
{
	if( !CheckRightPacketSize( sizeof(sockaddr_in) ) )
		return;
	int npos = GetBufferSize();
	int nsize = sizeof(sockaddr_in);
	memcpy(&arg,&m_pBuffer[npos-nsize],nsize);
	MoveBufferPointer(-nsize);


}

SP2Packet& SP2Packet::operator>>( InsertData& arg )
{
	if( !CheckRightPacketSize( sizeof(InsertData) ) )
	{
		ZeroMemory( &arg, sizeof( InsertData ) );
		return *this;
	}

	memcpy(&arg,&m_pBuffer[m_currentPos],sizeof(InsertData));
	m_currentPos += sizeof(InsertData);

	return *this;
}

SP2Packet& SP2Packet::operator>>( RemoveData& arg )
{
	if( !CheckRightPacketSize( sizeof(RemoveData) ) )
	{
		ZeroMemory( &arg, sizeof( RemoveData ) );
		return *this;
	}

	memcpy(&arg,&m_pBuffer[m_currentPos],sizeof(RemoveData));
	m_currentPos += sizeof(RemoveData);

	return *this;
}

SP2Packet& SP2Packet::operator<<( SendRelayInsertData& arg )
{
	if( ! CheckLeftPacketSize( sizeof( SendRelayInsertData ) ) )
		return *this;

	memcpy( &m_pBuffer[ m_currentPos ], &arg, sizeof( SendRelayInsertData ) );
	m_currentPos += sizeof( SendRelayInsertData );
	*m_packet_header.m_Size	= m_currentPos;
	return *this;
}

SP2Packet& SP2Packet::operator<<( RemoveData& arg )
{
	if( ! CheckLeftPacketSize( sizeof( RemoveData ) ) )
		return *this;

	memcpy( &m_pBuffer[ m_currentPos ], &arg, sizeof( RemoveData ) );
	m_currentPos += sizeof( RemoveData );
	*m_packet_header.m_Size	= m_currentPos;

	return *this;
}

SP2Packet& SP2Packet::operator<<( InsertData& arg )
{
	if( ! CheckLeftPacketSize( sizeof( InsertData ) ) )
		return *this;

	memcpy( &m_pBuffer[ m_currentPos ], &arg, sizeof( InsertData ) );
	m_currentPos += sizeof( InsertData );
	*m_packet_header.m_Size	= m_currentPos;

	return *this;
}


SP2Packet& SP2Packet::operator>>( SendRelayInsertData& arg )
{
	if( !CheckRightPacketSize( sizeof(SendRelayInsertData) ) )
	{
		ZeroMemory( &arg, sizeof( SendRelayInsertData ) );
		return *this;
	}

	memcpy(&arg,&m_pBuffer[m_currentPos],sizeof(SendRelayInsertData));
	m_currentPos += sizeof(SendRelayInsertData);
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

bool SP2Packet::Write( const ioHashString &arg )
{
	int nLen = lstrlen(arg.c_str() ) + 1;

	if( !CheckLeftPacketSize( nLen ) )
	{
		return false;
	}

	memcpy(&m_pBuffer[m_currentPos], arg.c_str(), nLen);
	m_currentPos			+= nLen;
	*m_packet_header.m_Size = m_currentPos;

	return true;
}

bool SP2Packet::Write( const Vector3 &arg )
{
	if( !CheckLeftPacketSize( sizeof(Vector3) ) ) return false;

	memcpy(&m_pBuffer[m_currentPos],&arg,sizeof(Vector3));
	m_currentPos += sizeof(Vector3);
	*m_packet_header.m_Size = m_currentPos;

	return true;
}

bool SP2Packet::Write( const ControlKeys &arg )
{
	if( !CheckLeftPacketSize( sizeof(ControlKeys) ) ) return false;

	memcpy(&m_pBuffer[m_currentPos],&arg,sizeof(ControlKeys));
	m_currentPos += sizeof(ControlKeys);
	*m_packet_header.m_Size = m_currentPos;

	return true;
}

bool SP2Packet::Write(CQueryData &arg)
{
	if( !CheckLeftPacketSize( sizeof(QueryHeader) + arg.GetBufferSize() ) ) return false; //기존에는 이걸 왜 안해줬는지..?

	memcpy(&m_pBuffer[m_currentPos],arg.GetHeader(),sizeof(QueryHeader));
	m_currentPos += sizeof(QueryHeader);
	memcpy(&m_pBuffer[m_currentPos],arg.GetBuffer(),arg.GetBufferSize());
	m_currentPos += arg.GetBufferSize();
	*m_packet_header.m_Size = m_currentPos;

	return true;
}

bool SP2Packet::Write( BYTE arg )
{
	if( !CheckLeftPacketSize( sizeof(BYTE) ) ) return false;

	memcpy(&m_pBuffer[m_currentPos],&arg,sizeof(BYTE));
	m_currentPos += sizeof(BYTE);
	*m_packet_header.m_Size = m_currentPos;
	
	return true;
}

bool SP2Packet::Write( short arg )
{
	if( !CheckLeftPacketSize( sizeof(short) ) ) return false;

	memcpy(&m_pBuffer[m_currentPos],&arg,sizeof(short));
	m_currentPos += sizeof(short);
	*m_packet_header.m_Size = m_currentPos;

	return true;
}

bool SP2Packet::Write( WORD arg )
{
	if( !CheckLeftPacketSize( sizeof(WORD) ) ) return false;

	memcpy(&m_pBuffer[m_currentPos],&arg,sizeof(WORD));
	m_currentPos += sizeof(WORD);
	*m_packet_header.m_Size = m_currentPos;

	return true;
}

bool SP2Packet::Write( CHARACTER& arg )
{
	PACKET_GUARD_bool( arg.FillData( *this ) );

	return true;
}

bool SP2Packet::Write( CQueryResultData &arg )
{
	if( !CheckLeftPacketSize( sizeof(QueryResultHeader) + arg.GetResultBufferSize() ) ) return false;
	
	memcpy(&m_pBuffer[m_currentPos],arg.GetHeader(),sizeof(QueryResultHeader));
	m_currentPos += sizeof(QueryResultHeader);
	memcpy(&m_pBuffer[m_currentPos],arg.GetBuffer(),arg.GetResultBufferSize());
	m_currentPos += arg.GetResultBufferSize();
	*m_packet_header.m_Size = m_currentPos;

	return true;
}

bool SP2Packet::Write( const Quaternion &arg )
{
	if( !CheckLeftPacketSize( sizeof(Quaternion) ) ) return false;

	memcpy(&m_pBuffer[m_currentPos],&arg,sizeof(Quaternion));
	m_currentPos += sizeof(Quaternion);
	*m_packet_header.m_Size = m_currentPos;

	return true;
}

bool SP2Packet::Read( ioHashString &arg )
{
	int nLen = lstrlen((LPTSTR)&m_pBuffer[m_currentPos]) * sizeof( TCHAR ) + sizeof( TCHAR );

	if( !CheckRightPacketSize( nLen ) )
	{
		arg.Clear();
		return false;
	}

	arg = &m_pBuffer[m_currentPos];
	m_currentPos += nLen;

	return true;
}

bool SP2Packet::Read( ControlKeys& arg )
{
	if( !CheckRightPacketSize( sizeof(ControlKeys) ) )
	{
		arg.Clear();
		return false;
	}

	memcpy(&arg,&m_pBuffer[m_currentPos],sizeof(ControlKeys));
	m_currentPos += sizeof(ControlKeys);

	return true;
}

bool SP2Packet::Read( BYTE& arg )
{
	if( !CheckRightPacketSize( sizeof(BYTE) ) )
	{
		arg = 0;
		return false;
	}

	memcpy(&arg,&m_pBuffer[m_currentPos],sizeof(BYTE));
	m_currentPos += sizeof(BYTE);

	return true;
}

bool SP2Packet::Read( short& arg )
{
	if( !CheckRightPacketSize( sizeof(short) ) )
	{
		arg = 0;
		return false;
	}

	memcpy(&arg,&m_pBuffer[m_currentPos],sizeof(short));
	m_currentPos += sizeof(short);

	return true;
}

bool SP2Packet::Read( WORD& arg )
{
	if( !CheckRightPacketSize( sizeof(WORD) ) )
	{
		arg = 0;
		return false;
	}

	memcpy(&arg,&m_pBuffer[m_currentPos],sizeof(WORD));
	m_currentPos += sizeof(WORD);

	return true;
}

bool SP2Packet::Read( CHARACTER& arg )
{
	if( !CheckRightPacketSize( sizeof( CHARACTER ) ) )
	{
		arg.Init();
		return false;
	}

	arg.ApplyData( *this );

	return true;
}

bool SP2Packet::Read( CQueryResultData &arg )
{
	if( !CheckRightPacketSize( sizeof(QueryResultHeader) + arg.GetResultBufferSize() ) )
	{
		ZeroMemory( &arg, sizeof( CQueryResultData ) );
		return false;
	}

	arg.SetBuffer(&m_pBuffer[m_currentPos]);
	m_currentPos += sizeof(QueryResultHeader);
	m_currentPos += arg.GetResultBufferSize();
	*m_packet_header.m_Size = m_currentPos;
	return true;
}

bool SP2Packet::Read( Vector3 &arg )
{
	if( !CheckRightPacketSize( sizeof(Vector3) ) )
	{
		arg.x = arg.y = arg.z = 0;
	}

	memcpy(&arg,&m_pBuffer[m_currentPos],sizeof(Vector3));
	m_currentPos += sizeof(Vector3);

	return true;
}

bool SP2Packet::Read( Quaternion &arg )
{
	if( !CheckRightPacketSize( sizeof(Quaternion) ) )
	{
		ZeroMemory( &arg, sizeof( Quaternion ) );
		return false;
	}

	memcpy(&arg,&m_pBuffer[m_currentPos],sizeof(Quaternion));
	m_currentPos += sizeof(Quaternion);

	return true;
}

void SP2Packet::GetStringOfStream(std::string & str)
{
	char const hex[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A',   'B','C','D','E','F'};

	char buf[100]; 
	sprintf(buf, "0x%x", this->GetPacketID());

	str.append("ID: ");
	str.append(buf);

	str.append(", Header: ");

	// header
	for (int i = 0; i < sizeof(PACKETHEADER); ++i) 
	{
		const char ch = m_pBuffer[i];
		str.append(&hex[(ch  & 0xF0) >> 4], 1);
		str.append(&hex[ch & 0xF], 1);
	}

	str.append(", Body: ");
	str.append("(Size:");
	sprintf(buf, "%d", this->GetBufferSize() - sizeof(PACKETHEADER));
	str.append(buf);
	str.append("), ");

	// body
	for (int i = sizeof(PACKETHEADER); i < this->GetBufferSize(); ++i) 
	{
		const char ch = m_pBuffer[i];
		str.append(&hex[(ch & 0xF0) >> 4], 1);
		str.append(&hex[ch & 0xF], 1);
	}
}

void SP2Packet::GetStringOfStreamBody(std::string & str)
{
	char const hex[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A',   'B','C','D','E','F'};

	// body
	for (int i = sizeof(PACKETHEADER); i < this->GetBufferSize(); ++i) 
	{
		const char ch = m_pBuffer[i];
		str.append(&hex[(ch & 0xF0) >> 4], 1);
		str.append(&hex[ch & 0xF], 1);
	}
}


