#include "../stdafx.h"
#include "SP2Packet.h"
#include "../QueryData/QueryData.h"
#include "../QueryData/QueryResultData.h"
#include "../NexonDefine.h"
#include "../NexonEUDefine.h"

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

void SP2Packet::SetBufferSizeMinusOne()
{
	if( (*m_packet_header.m_Size) <= sizeof( PACKETHEADER ) )
		return;
	(*m_packet_header.m_Size)--;
}

void SP2Packet::SetDataAdd( char *buffer, int size, bool bCurPosReSet )
{
	memcpy( &m_pBuffer[m_currentPos], buffer, size );
	m_currentPos += size;
	*m_packet_header.m_Size = m_currentPos;
	if( bCurPosReSet )
		m_currentPos = sizeof(PACKETHEADER);
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

SP2Packet&  SP2Packet::operator << (char arg)
{
	if( !CheckLeftPacketSize( sizeof(char) ) ) return *this;

	memcpy(&m_pBuffer[m_currentPos],&arg,sizeof(char));
	m_currentPos += sizeof(char);
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

SP2Packet& SP2Packet::operator<<( const MgameCashRequest &arg )
{
	if( !CheckLeftPacketSize( sizeof(MgameCashRequest) ) ) return *this;

	memcpy(&m_pBuffer[m_currentPos],&arg,sizeof(MgameCashRequest));
	m_currentPos += sizeof(MgameCashRequest);
	*m_packet_header.m_Size = m_currentPos;

	return *this;
}

SP2Packet& SP2Packet::operator<<( const MgameCashResult &arg )
{
	if( !CheckLeftPacketSize( sizeof(MgameCashResult) ) ) return *this;

	memcpy(&m_pBuffer[m_currentPos],&arg,sizeof(MgameCashResult));
	m_currentPos += sizeof(MgameCashResult);
	*m_packet_header.m_Size = m_currentPos;

	return *this;
}

SP2Packet& SP2Packet::operator<<( const BOQPTS_GS_CONNECT &arg )
{
	if( !CheckLeftPacketSize( sizeof(BOQPTS_GS_CONNECT) ) ) return *this;

	memcpy(&m_pBuffer[m_currentPos],&arg,sizeof(BOQPTS_GS_CONNECT));
	m_currentPos += sizeof(BOQPTS_GS_CONNECT);
	*m_packet_header.m_Size = m_currentPos;

	return *this;
}

SP2Packet& SP2Packet::operator<<( const BOQPTS_HEALTH_CHECK &arg )
{
	if( !CheckLeftPacketSize( sizeof(BOQPTS_HEALTH_CHECK) ) ) return *this;

	memcpy(&m_pBuffer[m_currentPos],&arg,sizeof(BOQPTS_HEALTH_CHECK));
	m_currentPos += sizeof(BOQPTS_HEALTH_CHECK);
	*m_packet_header.m_Size = m_currentPos;

	return *this;
}

SP2Packet& SP2Packet::operator<<( const BOQPTS_CHECKPREMIUM2 &arg )
{
	if( !CheckLeftPacketSize( sizeof(BOQPTS_CHECKPREMIUM2) ) ) return *this;

	memcpy(&m_pBuffer[m_currentPos],&arg,sizeof(BOQPTS_CHECKPREMIUM2));
	m_currentPos += sizeof(BOQPTS_CHECKPREMIUM2);
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

SP2Packet& SP2Packet::operator << ( const PHEADER &arg )
{
	if( !CheckLeftPacketSize( sizeof(PHEADER) ) ) return *this;

	memcpy(&m_pBuffer[m_currentPos],&arg,sizeof(PHEADER));
	m_currentPos += sizeof(PHEADER);
	*m_packet_header.m_Size = m_currentPos;

	return *this;
}

SP2Packet& SP2Packet::operator << ( const SC_ALIVE &arg )
{
	if( !CheckLeftPacketSize( sizeof(SC_ALIVE) ) ) return *this;

	memcpy(&m_pBuffer[m_currentPos],&arg,sizeof(SC_ALIVE));
	m_currentPos += sizeof(SC_ALIVE);
	*m_packet_header.m_Size = m_currentPos;

	return *this;
}

SP2Packet& SP2Packet::operator << ( const CS_BALANCE &arg )
{
	if( !CheckLeftPacketSize( sizeof(CS_BALANCE) ) ) return *this;

	memcpy(&m_pBuffer[m_currentPos],&arg,sizeof(CS_BALANCE));
	m_currentPos += sizeof(CS_BALANCE);
	*m_packet_header.m_Size = m_currentPos;

	return *this;
}

SP2Packet& SP2Packet::operator << ( const SC_BALANCE &arg )
{
	if( !CheckLeftPacketSize( sizeof(SC_BALANCE) ) ) return *this;

	memcpy(&m_pBuffer[m_currentPos],&arg,sizeof(SC_BALANCE));
	m_currentPos += sizeof(SC_BALANCE);
	*m_packet_header.m_Size = m_currentPos;

	return *this;
}

SP2Packet& SP2Packet::operator << ( const CS_PURCHASEITEM &arg )
{
	if( !CheckLeftPacketSize( sizeof(CS_PURCHASEITEM) ) ) return *this;

	memcpy(&m_pBuffer[m_currentPos],&arg,sizeof(CS_PURCHASEITEM));
	m_currentPos += sizeof(CS_PURCHASEITEM);
	*m_packet_header.m_Size = m_currentPos;

	return *this;
}

SP2Packet& SP2Packet::operator << ( const SC_PURCHASEITEM &arg )
{
	if( !CheckLeftPacketSize( sizeof(SC_PURCHASEITEM) ) ) return *this;

	memcpy(&m_pBuffer[m_currentPos],&arg,sizeof(SC_PURCHASEITEM));
	m_currentPos += sizeof(SC_PURCHASEITEM);
	*m_packet_header.m_Size = m_currentPos;

	return *this;
}

SP2Packet& SP2Packet::operator << ( const CS_GIFTITEM &arg )
{
	if( !CheckLeftPacketSize( sizeof(CS_GIFTITEM) ) ) return *this;

	memcpy(&m_pBuffer[m_currentPos],&arg,sizeof(CS_GIFTITEM));
	m_currentPos += sizeof(CS_GIFTITEM);
	*m_packet_header.m_Size = m_currentPos;

	return *this;
}

SP2Packet& SP2Packet::operator << ( const SC_GIFTITEM &arg )
{
	if( !CheckLeftPacketSize( sizeof(SC_GIFTITEM) ) ) return *this;

	memcpy(&m_pBuffer[m_currentPos],&arg,sizeof(SC_GIFTITEM));
	m_currentPos += sizeof(SC_GIFTITEM);
	*m_packet_header.m_Size = m_currentPos;

	return *this;
}

SP2Packet& SP2Packet::operator << ( const CS_CNLPURCHASE &arg )
{
	if( !CheckLeftPacketSize( sizeof(CS_CNLPURCHASE) ) ) return *this;

	memcpy(&m_pBuffer[m_currentPos],&arg,sizeof(CS_CNLPURCHASE));
	m_currentPos += sizeof(CS_CNLPURCHASE);
	*m_packet_header.m_Size = m_currentPos;

	return *this;
}

SP2Packet& SP2Packet::operator << ( const SC_CNLPURCHASE &arg )
{
	if( !CheckLeftPacketSize( sizeof(SC_CNLPURCHASE) ) ) return *this;

	memcpy(&m_pBuffer[m_currentPos],&arg,sizeof(SC_CNLPURCHASE));
	m_currentPos += sizeof(SC_CNLPURCHASE);
	*m_packet_header.m_Size = m_currentPos;

	return *this;
}
SP2Packet& SP2Packet::operator << ( const BILL_PACK_HEALTH_CHECK &arg )
{
	if( !CheckLeftPacketSize( sizeof(BILL_PACK_HEALTH_CHECK) ) ) return *this;

	memcpy(&m_pBuffer[m_currentPos],&arg,sizeof(BILL_PACK_HEALTH_CHECK));
	m_currentPos += sizeof(BILL_PACK_HEALTH_CHECK);
	*m_packet_header.m_Size = m_currentPos;

	return *this;
}

SP2Packet& SP2Packet::operator << ( const BILL_PACK_GETBALANCE &arg )
{
	if( !CheckLeftPacketSize( sizeof(BILL_PACK_GETBALANCE) ) ) return *this;

	memcpy(&m_pBuffer[m_currentPos],&arg,sizeof(BILL_PACK_GETBALANCE));
	m_currentPos += sizeof(BILL_PACK_GETBALANCE);
	*m_packet_header.m_Size = m_currentPos;

	return *this;
}

SP2Packet& SP2Packet::operator << ( const BILL_PACK_PURCHASEITEM &arg )
{
	if( !CheckLeftPacketSize( sizeof(BILL_PACK_PURCHASEITEM) ) ) return *this;

	memcpy(&m_pBuffer[m_currentPos],&arg,sizeof(BILL_PACK_PURCHASEITEM));
	m_currentPos += sizeof(BILL_PACK_PURCHASEITEM);
	*m_packet_header.m_Size = m_currentPos;

	return *this;
}

SP2Packet& SP2Packet::operator << ( const BILL_PACK_CNLPURCHASE &arg )
{
	if( !CheckLeftPacketSize( sizeof(BILL_PACK_CNLPURCHASE) ) ) return *this;

	memcpy(&m_pBuffer[m_currentPos],&arg,sizeof(BILL_PACK_CNLPURCHASE));
	m_currentPos += sizeof(BILL_PACK_CNLPURCHASE);
	*m_packet_header.m_Size = m_currentPos;

	return *this;
}

SP2Packet& SP2Packet::operator << ( const CS_RETRACT_PAYBACK &arg )
{
	if( !CheckLeftPacketSize( sizeof(CS_RETRACT_PAYBACK) ) ) return *this;

	memcpy(&m_pBuffer[m_currentPos],&arg,sizeof(CS_RETRACT_PAYBACK));
	m_currentPos += sizeof(CS_RETRACT_PAYBACK);
	*m_packet_header.m_Size = m_currentPos;

	return *this;
}

SP2Packet& SP2Packet::operator << ( const CS_RETRACT &arg )
{
	if( !CheckLeftPacketSize( sizeof(CS_RETRACT) ) ) return *this;

	memcpy(&m_pBuffer[m_currentPos],&arg,sizeof(CS_RETRACT));
	m_currentPos += sizeof(CS_RETRACT);
	*m_packet_header.m_Size = m_currentPos;

	return *this;
}

SP2Packet& SP2Packet::operator << ( const CS_RETRACT_CANCEL &arg )
{
	if( !CheckLeftPacketSize( sizeof(CS_RETRACT_CANCEL) ) ) return *this;

	memcpy(&m_pBuffer[m_currentPos],&arg,sizeof(CS_RETRACT_CANCEL));
	m_currentPos += sizeof(CS_RETRACT_CANCEL);
	*m_packet_header.m_Size = m_currentPos;

	return *this;
}

SP2Packet& SP2Packet::operator << ( const uint64_t arg )
{
	if( !CheckLeftPacketSize( sizeof(uint64_t) ) ) return *this;

	memcpy(&m_pBuffer[m_currentPos],&arg,sizeof(uint64_t) );
	m_currentPos += sizeof(uint64_t);
	*m_packet_header.m_Size = m_currentPos;

	return *this;
}


SP2Packet& SP2Packet::operator<<( const unsigned int arg )
{
	if( !CheckLeftPacketSize( sizeof(unsigned int) ) ) return *this;

	memcpy(&m_pBuffer[m_currentPos],&arg,sizeof(unsigned int));
	m_currentPos += sizeof(unsigned int);
	*m_packet_header.m_Size = m_currentPos;

	return *this;
}

//--------------------------------------------------------------------------------------------------------------------
SP2Packet&  SP2Packet::operator >> (BYTE &arg)
{
	if( !CheckRightPacketSize( sizeof(BYTE) ) ) return *this;

	memcpy(&arg,&m_pBuffer[m_currentPos],sizeof(BYTE));
	m_currentPos += sizeof(BYTE);
	
	return *this;
}

SP2Packet&  SP2Packet::operator >> (char &arg)
{
	if( !CheckRightPacketSize( sizeof(char) ) ) return *this;

	memcpy(&arg,&m_pBuffer[m_currentPos],sizeof(char));
	m_currentPos += sizeof(char);

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

SP2Packet& SP2Packet::operator>>( MgameCashRequest &arg )
{
	if( !CheckRightPacketSize( sizeof(MgameCashRequest) ) ) return *this;

	memcpy(&arg,&m_pBuffer[m_currentPos],sizeof(MgameCashRequest));
	m_currentPos += sizeof(MgameCashRequest);

	return *this;
}

SP2Packet& SP2Packet::operator>>( MgameCashResult &arg )
{
	if( !CheckRightPacketSize( sizeof(MgameCashResult) ) ) return *this;

	memcpy(&arg,&m_pBuffer[m_currentPos],sizeof(MgameCashResult));
	m_currentPos += sizeof(MgameCashResult);

	return *this;
}

SP2Packet& SP2Packet::operator>>( BOQPTS_GS_CONNECT &arg )
{
	if( !CheckRightPacketSize( sizeof(BOQPTS_GS_CONNECT) ) ) return *this;

	memcpy(&arg,&m_pBuffer[m_currentPos],sizeof(BOQPTS_GS_CONNECT));
	m_currentPos += sizeof(BOQPTS_GS_CONNECT);

	return *this;
}

SP2Packet& SP2Packet::operator>>( BOQPTS_HEALTH_CHECK &arg )
{
	if( !CheckRightPacketSize( sizeof(BOQPTS_HEALTH_CHECK) ) ) return *this;

	memcpy(&arg,&m_pBuffer[m_currentPos],sizeof(BOQPTS_HEALTH_CHECK));
	m_currentPos += sizeof(BOQPTS_HEALTH_CHECK);

	return *this;
}

SP2Packet& SP2Packet::operator>>( BOQPTS_CHECKPREMIUM2 &arg )
{
	if( !CheckRightPacketSize( sizeof(BOQPTS_CHECKPREMIUM2) ) ) return *this;

	memcpy(&arg,&m_pBuffer[m_currentPos],sizeof(BOQPTS_CHECKPREMIUM2));
	m_currentPos += sizeof(BOQPTS_CHECKPREMIUM2);

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
SP2Packet& SP2Packet::operator >> ( MonitorStatusRequest &arg )
{
	if( !CheckRightPacketSize( sizeof(MonitorStatusRequest) ) ) return *this;

	memcpy(&arg,&m_pBuffer[m_currentPos],sizeof(MonitorStatusRequest));
	m_currentPos += sizeof(MonitorStatusRequest);

	return *this;
}

SP2Packet& SP2Packet::operator >> ( MonitorStatusResult &arg )
{
	if( !CheckRightPacketSize( sizeof(MonitorStatusResult) ) ) return *this;

	memcpy(&arg,&m_pBuffer[m_currentPos],sizeof(MonitorStatusResult));
	m_currentPos += sizeof(MonitorStatusResult);

	return *this;
}

SP2Packet& SP2Packet::operator >> ( MonitorChangeRequest &arg )
{
	if( !CheckRightPacketSize( sizeof(MonitorChangeRequest) ) ) return *this;

	memcpy(&arg,&m_pBuffer[m_currentPos],sizeof(MonitorChangeRequest));
	m_currentPos += sizeof(MonitorChangeRequest);

	return *this;
}

SP2Packet& SP2Packet::operator >> ( MonitorChangeResult &arg )
{
	if( !CheckRightPacketSize( sizeof(MonitorChangeResult) ) ) return *this;

	memcpy(&arg,&m_pBuffer[m_currentPos],sizeof(MonitorChangeResult));
	m_currentPos += sizeof(MonitorChangeResult);

	return *this;
}

SP2Packet& SP2Packet::operator>>( PHEADER &arg )
{
	if( !CheckRightPacketSize( sizeof(PHEADER) ) ) return *this;

	memcpy(&arg,&m_pBuffer[m_currentPos],sizeof(PHEADER));
	m_currentPos += sizeof(PHEADER);

	return *this;
}

SP2Packet& SP2Packet::operator>>( SC_ALIVE &arg )
{
	if( !CheckRightPacketSize( sizeof(SC_ALIVE) ) ) return *this;

	memcpy(&arg,&m_pBuffer[m_currentPos],sizeof(SC_ALIVE));
	m_currentPos += sizeof(SC_ALIVE);

	return *this;
}

SP2Packet& SP2Packet::operator>>( CS_BALANCE &arg )
{
	if( !CheckRightPacketSize( sizeof(CS_BALANCE) ) ) return *this;

	memcpy(&arg,&m_pBuffer[m_currentPos],sizeof(CS_BALANCE));
	m_currentPos += sizeof(CS_BALANCE);

	return *this;
}

SP2Packet& SP2Packet::operator>>( SC_BALANCE &arg )
{
	if( !CheckRightPacketSize( sizeof(SC_BALANCE) ) ) return *this;

	memcpy(&arg,&m_pBuffer[m_currentPos],sizeof(SC_BALANCE));
	m_currentPos += sizeof(SC_BALANCE);

	return *this;
}

SP2Packet& SP2Packet::operator>>( CS_PURCHASEITEM &arg )
{
	if( !CheckRightPacketSize( sizeof(CS_PURCHASEITEM) ) ) return *this;

	memcpy(&arg,&m_pBuffer[m_currentPos],sizeof(CS_PURCHASEITEM));
	m_currentPos += sizeof(CS_PURCHASEITEM);

	return *this;
}

SP2Packet& SP2Packet::operator>>( SC_PURCHASEITEM &arg )
{
	if( !CheckRightPacketSize( sizeof(SC_PURCHASEITEM) ) ) return *this;

	memcpy(&arg,&m_pBuffer[m_currentPos],sizeof(SC_PURCHASEITEM));
	m_currentPos += sizeof(SC_PURCHASEITEM);

	return *this;
}

SP2Packet& SP2Packet::operator>>( CS_GIFTITEM &arg )
{
	if( !CheckRightPacketSize( sizeof(CS_GIFTITEM) ) ) return *this;

	memcpy(&arg,&m_pBuffer[m_currentPos],sizeof(CS_GIFTITEM));
	m_currentPos += sizeof(CS_GIFTITEM);

	return *this;
}

SP2Packet& SP2Packet::operator>>( SC_GIFTITEM &arg )
{
	if( !CheckRightPacketSize( sizeof(SC_GIFTITEM) ) ) return *this;

	memcpy(&arg,&m_pBuffer[m_currentPos],sizeof(SC_GIFTITEM));
	m_currentPos += sizeof(SC_GIFTITEM);

	return *this;
}

SP2Packet& SP2Packet::operator>>( CS_CNLPURCHASE &arg )
{
	if( !CheckRightPacketSize( sizeof(CS_CNLPURCHASE) ) ) return *this;

	memcpy(&arg,&m_pBuffer[m_currentPos],sizeof(CS_CNLPURCHASE));
	m_currentPos += sizeof(CS_CNLPURCHASE);

	return *this;
}

SP2Packet& SP2Packet::operator>>( SC_CNLPURCHASE &arg )
{
	if( !CheckRightPacketSize( sizeof(SC_CNLPURCHASE) ) ) return *this;

	memcpy(&arg,&m_pBuffer[m_currentPos],sizeof(SC_CNLPURCHASE));
	m_currentPos += sizeof(SC_CNLPURCHASE);

	return *this;
}

SP2Packet& SP2Packet::operator>>( BILL_PACK_HEALTH_CHECK &arg )
{
	if( !CheckRightPacketSize( sizeof(BILL_PACK_HEALTH_CHECK) ) ) return *this;

	memcpy(&arg,&m_pBuffer[m_currentPos],sizeof(BILL_PACK_HEALTH_CHECK));
	m_currentPos += sizeof(BILL_PACK_HEALTH_CHECK);

	return *this;
}

SP2Packet& SP2Packet::operator>>( BILL_PACK_GETBALANCE &arg )
{
	if( !CheckRightPacketSize( sizeof(BILL_PACK_GETBALANCE) ) ) return *this;

	memcpy(&arg,&m_pBuffer[m_currentPos],sizeof(BILL_PACK_GETBALANCE));
	m_currentPos += sizeof(BILL_PACK_GETBALANCE);

	return *this;
}

SP2Packet& SP2Packet::operator>>( BILL_PACK_PURCHASEITEM &arg )
{
	if( !CheckRightPacketSize( sizeof(BILL_PACK_PURCHASEITEM) ) ) return *this;

	memcpy(&arg,&m_pBuffer[m_currentPos],sizeof(BILL_PACK_PURCHASEITEM));
	m_currentPos += sizeof(BILL_PACK_PURCHASEITEM);

	return *this;
}

SP2Packet& SP2Packet::operator>>( BILL_PACK_CNLPURCHASE &arg )
{
	if( !CheckRightPacketSize( sizeof(BILL_PACK_CNLPURCHASE) ) ) return *this;

	memcpy(&arg,&m_pBuffer[m_currentPos],sizeof(BILL_PACK_CNLPURCHASE));
	m_currentPos += sizeof(BILL_PACK_CNLPURCHASE);

	return *this;
}

SP2Packet& SP2Packet::operator>>( CS_RETRACT_PAYBACK &arg )
{
	if( !CheckRightPacketSize( sizeof(CS_RETRACT_PAYBACK) ) ) return *this;

	memcpy(&arg,&m_pBuffer[m_currentPos],sizeof(CS_RETRACT_PAYBACK));
	m_currentPos += sizeof(CS_RETRACT_PAYBACK);

	return *this;
}

SP2Packet& SP2Packet::operator>>( SC_RETRACT_PAYBACK &arg )
{
	if( !CheckRightPacketSize( sizeof(SC_RETRACT_PAYBACK) ) ) return *this;

	memcpy(&arg,&m_pBuffer[m_currentPos],sizeof(SC_RETRACT_PAYBACK));
	m_currentPos += sizeof(SC_RETRACT_PAYBACK);

	return *this;
}

SP2Packet& SP2Packet::operator>>( CS_RETRACT &arg )
{
	if( !CheckRightPacketSize( sizeof(CS_RETRACT) ) ) return *this;

	memcpy(&arg,&m_pBuffer[m_currentPos],sizeof(CS_RETRACT));
	m_currentPos += sizeof(CS_RETRACT);

	return *this;
}

SP2Packet& SP2Packet::operator>>( SC_RETRACT &arg )
{
	if( !CheckRightPacketSize( sizeof(SC_RETRACT) ) ) return *this;

	memcpy(&arg,&m_pBuffer[m_currentPos],sizeof(SC_RETRACT));
	m_currentPos += sizeof(SC_RETRACT);

	return *this;
}

SP2Packet& SP2Packet::operator>>( CS_RETRACT_CANCEL &arg )
{
	if( !CheckRightPacketSize( sizeof(CS_RETRACT_CANCEL) ) ) return *this;

	memcpy(&arg,&m_pBuffer[m_currentPos],sizeof(CS_RETRACT_CANCEL));
	m_currentPos += sizeof(CS_RETRACT_CANCEL);

	return *this;
}

SP2Packet& SP2Packet::operator>>( SC_RETRACT_CANCEL &arg )
{
	if( !CheckRightPacketSize( sizeof(SC_RETRACT_CANCEL) ) ) return *this;

	memcpy(&arg,&m_pBuffer[m_currentPos],sizeof(SC_RETRACT_CANCEL));
	m_currentPos += sizeof(SC_RETRACT_CANCEL);

	return *this;
}

SP2Packet& SP2Packet::operator>>( unsigned int& arg )
{
	if( !CheckRightPacketSize( sizeof(unsigned int) ) ) return *this;

	memcpy(&arg,&m_pBuffer[m_currentPos],sizeof(unsigned int));
	m_currentPos += sizeof(unsigned int);

	return *this;
}

SP2Packet& SP2Packet::operator >> ( uint64_t& arg )
{
	if( !CheckRightPacketSize( sizeof(uint64_t) ) ) return *this;

	memcpy(&arg,&m_pBuffer[m_currentPos],sizeof(uint64_t));
	m_currentPos += sizeof(uint64_t);

	return *this;
}

bool SP2Packet::Write( NexonFirstInitialize& data )
{
	//kyg 추후 Write로 바꺼야댐..
	(*this) << data.header;
	(*this) << data.size;
	(*this) << data.PacketType;
	(*this) << data.InitializeType;
	(*this) << data.GameSn;
	(*this) << data.DomainSn;
	(*this) << data.DomainNameLength;
	Write(data.DomainName);
	(*this) << data.SynchronizeType;
	(*this) << data.SynchronizeCount;

	return true;

}

bool SP2Packet::Write( NexonReInitialize& data )
{
 	(*this) << data.header;
	(*this) << data.size;
	(*this) << data.PacketType;
	(*this) << data.InitializeType;
	(*this) << data.GameSn;
	(*this) << data.DomainSn;
	(*this) << data.DomainNameLength;
	Write(data.DomainName);
	(*this) << data.SynchronizeType;
	(*this) << data.UserSynchronize;
	(*this) << data.SynchronizeCount;


	return true;
}

bool SP2Packet::Write( NexonLogin& data )
{
	/*
	unsigned char LoginType;// 1
	unsigned char userIdLength;
	ioHashString UserId;//nexonID
	unsigned int ClientIP; // inet_addr 
	unsigned int PropertyCount; //우리가 쓰는건 3개임 
	//extend property
	unsigned short CharaterNameType;//1 
	unsigned char CharNameLength;
	ioHashString CharName;
	unsigned short LogcalIPType;//2
	unsigned int LocalIP;// PrivateIP inet_addr;.
	unsigned short AuthType; // 12
	unsigned char OnlyAuth;
	unsigned short IsPolicyReturnType;//17
	char PolicyReturnCount;//1
	unsigned char PolicyType; // 10
	*/
	(*this) << data.header;
	(*this) << data.size;
	(*this) << data.PacketType;

	(*this) << data.LoginType;
	(*this) << data.userIdLength;
	Write(data.UserId);
	(*this) << data.ClientIP;
	(*this) << data.PropertyCount;
	//케릭터이름
	(*this) << data.CharaterNameType;
	(*this) << data.CharNameLength;
	Write(data.CharName);
	//내부 아이피 
	(*this) << data.LogcalIPType;
	(*this) << data.LocalIP;

	//인증 
	(*this) << data.AuthType;
	(*this) << data.OnlyAuth;
	//피시방
	(*this) << data.PCBangNoType;
	(*this) << data.PCBangNo;
	//폴리시 
	(*this) << data.IsPolicyReturnType;
	(*this) << data.PolicyReturnCount;
	(*this) << data.PolicyType;

	return true;
}

bool SP2Packet::Write( NexonAlive& data )
{
	(*this) << data.header;
	(*this) << data.size;
	(*this) << data.PacketType;

	return true;
}

bool SP2Packet::Write( ioHashString& arg ) //kyg 넥슨일때엔 좀 특수한경우
{
	int nlen = lstrlen( arg.c_str() );

	if( !CheckLeftPacketSize( nlen ) ) return false;

	memcpy( &m_pBuffer[m_currentPos], arg.c_str(), nlen );
	m_currentPos += nlen;
	*m_packet_header.m_Size = m_currentPos;

	return true;
}

bool SP2Packet::Write( NexonSynchronize& arg )
{
	(*this) << arg.header;
	(*this) << arg.size;
	(*this) << arg.PacketType;

	(*this) << arg.IsMonitoring;
	(*this) << arg.Count;
 
	return true;
}

bool SP2Packet::Write( NexonLogOut& arg )
{
	/*
	BYTE LogoutType;
	BYTE userIdLength;
	ioHashString UserId;
	uint64_t SessionNo;
	*/
	(*this) << arg.header;
	(*this) << arg.size;
	(*this) << arg.PacketType;

	(*this) << arg.LogoutType;
	(*this) << arg.userIdLength;
	Write(arg.UserId);
	(*this) << arg.SessionNo;

	return 0;
}

bool SP2Packet::Write( OtherLogin& arg )
{
	/*
	unsigned char LoginType;// 1
	unsigned char userIdLength;
	ioHashString UserId;//nexonID
	unsigned int ClientIP; // inet_addr 
	unsigned int PropertyCount; //우리가 쓰는건 3개임 
	//extend property
	unsigned short CharaterNameType;//1 
	unsigned char CharNameLength;
	ioHashString CharName;
	unsigned short LogcalIPType;//2
	unsigned int LocalIP;// PrivateIP inet_addr;.
	unsigned short AuthType; // 12
	unsigned char OnlyAuth;
	unsigned short IsPolicyReturnType;//17
	char PolicyReturnCount;//1
	unsigned char PolicyType; // 10
	*/
	(*this) << arg.header;
	(*this) << arg.size;
	(*this) << arg.PacketType;

	(*this) << arg.LoginType;
	(*this) << arg.userIdLength;
	Write(arg.UserId);
	(*this) << arg.ClientIP;
	(*this) << arg.PropertyCount;
	//케릭터이름
	(*this) << arg.CharaterNameType;
	(*this) << arg.CharNameLength;
	Write(arg.CharName);
	//내부 아이피 
	(*this) << arg.LogcalIPType;
	(*this) << arg.LocalIP;

	//인증 
	(*this) << arg.AuthType;
	(*this) << arg.OnlyAuth;
	//피시방
	(*this) << arg.PCBangNoType;
	(*this) << arg.PCBangNo;
	return true;
}
bool SP2Packet::Write(NexonEUInitialize& arg)
{
	(*this) << arg.packetID;
	(*this) << arg.size;
	(*this) << arg.packetNo;
	(*this) << arg.packetType;
	(*this) << arg.serviceCodeLength;
	Write(arg.serviceCode);
	(*this) << arg.serverNo;
	return true;
}


bool SP2Packet::Write(NexonEUCheckCash& arg)
{
	(*this) << arg.packetID;
	(*this) << arg.size;
	(*this) << arg.packetNo;
	(*this) << arg.packetType;
	(*this) << arg.nexonIDLength;
	Write(arg.nexonID);
	return true;
}

bool SP2Packet::Write(NexonEUPurchaseItem& arg)
{
	(*this) << arg.packetID;
	(*this) << arg.size;
	(*this) << arg.packetNo;
	(*this) << arg.packetType;
	(*this) << arg.clientIP;
	(*this) << arg.reason;
	(*this) << arg.gameIDLength;
	Write(arg.gameID);
	(*this) << arg.nexonIDLengh;
	Write(arg.nexonID);
	(*this) << arg.nexonOID;
	(*this) << arg.useNameLength;
	Write(arg.userName);
	(*this) << arg.userAge;
	(*this) << arg.orderIDLength;
	Write(arg.orderID);
	(*this) << arg.paymentType;
	(*this) << arg.totalAmount;
	(*this) << arg.productArrayLength;
	(*this) << arg.productNo;
	(*this) << arg.orderQuantity;
	
	return true;
}

bool SP2Packet::Write( NexonEUSessionInitialize& arg )
{
	(*this) << arg.size;
	(*this) << arg.code;
	(*this) << arg.gameCode;
	(*this) << arg.serverCode;
	(*this) << arg.codePage;

	return true;
}


bool SP2Packet::Write(NexonEUSessionCheck& arg)
{
	(*this) << arg.size;
	(*this) << arg.code;
	(*this) << arg.serial;

	return true;
}

bool SP2Packet::Write(NexonEUSession4Request& arg)
{
	(*this) << arg.size;
	(*this) << arg.code;
	(*this) << arg.serial;
	(*this) << arg.passPortLength;
	Write(arg.passPort);
	(*this) << arg.userIPLength;
	Write(arg.userIP);
	(*this) << arg.userHwidLength;
	Write(arg.userHwid);
	(*this) << arg.gameCode;
	
	return true;
}

bool SP2Packet::Write(NexonEUGetPurchaseAmount& arg)
{
	
	(*this) << arg.packetID;
	(*this) << arg.size;
	(*this) << arg.packetNo;
	(*this) << arg.packetType;
	(*this) << arg.orderNo;
	(*this) << arg.productNo;

	return true;
}


bool SP2Packet::Write(NexonProductInquiry& arg)
{
	(*this) << arg.packetID;
	(*this) << arg.size;
	(*this) << arg.packetNo;
	(*this) << arg.packetType;
	(*this) << arg.pageIndex;
	(*this) << arg.rowPerPage;
	(*this) << arg.queryType;

	return true;
}

bool SP2Packet::Read(NexonEUSessionResponse& arg)
{
	(*this) >> arg.size;
	(*this) >> arg.code;
	(*this) >> arg.initializeResult;

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

bool SP2Packet::Read( OnNexonInitialize& data )
{
	(*this) >> data.header;
	(*this) >> data.size;
	(*this) >> data.PacketType;
	(*this) >> data.InitializeType;
	(*this) >> data.Result;
	(*this) >> data.DomainSn;
	(*this) >> data.MessageLength;
	data.Message.SetBuffer(&m_pBuffer[m_currentPos],data.MessageLength);
	m_currentPos += data.MessageLength;
	(*this) >> data.PropertyCount;
	
	return true;
}

bool SP2Packet::Read( OnNexonLogin& data )
{	
	/*
	unsigned char LoginType;
	int64_t SessionNo;
	unsigned char userIdLength;
	ioHashString UserId;
	char AuthorizeResult;
	char AuthorizeType;
	char ChargeType;
	unsigned char Option;
	int Argument;
	unsigned char IsCharged;
	unsigned char PropertyCount;
	char ProPertyData[2048]; // 순서가 바뀌어*/

	(*this) >> data.header;
	(*this) >> data.size;
	(*this) >> data.PacketType;

	(*this) >> data.LoginType;
	(*this) >> data.SessionNo;
	(*this) >> data.userIdLength;
	SetHashString(data.UserId,data.userIdLength);
	(*this) >> data.AuthorizeResult;
	(*this) >> data.AuthorizeType;
	(*this) >> data.ChargeType;
	(*this) >> data.Option;
	(*this) >> data.Argument;
	(*this) >> data.IsCharged;
	(*this) >> data.PropertyCount;

	if(data.PropertyCount > 0 )
	{
		
	}
	 //polyStart;

	return true;
}

bool SP2Packet::Read( OnNexonTerminate& data )
{
	/*
	BYTE TerminateType;
	uint64_t SessionNo;
	BYTE userIdLength;
	ioHashString UserId;
	BYTE CharNameLength;
	ioHashString CharName;
	unsigned int Option;
	BYTE PropertyCount;*/
	(*this) >> data.header;
	(*this) >> data.size;
	(*this) >> data.PacketType; //공통

	(*this) >> data.TerminateType; //공통
	(*this) >> data.SessionNo; //공통

	(*this) >> data.userIdLength; //공통
	SetHashString(data.UserId,data.userIdLength);

	(*this) >> data.CharNameLength;
	SetHashString(data.CharName,data.CharNameLength);

	(*this) >> data.Option;
	(*this) >> data.PropertyCount;

	return true;
}

bool SP2Packet::Read( OnNexonMessage& data )
{
	/*
	BYTE MessageType;
	uint64_t SessionNo;
	BYTE userIdLength;
	ioHashString UserId;
	BYTE charNameLegnth;
	ioHashString CharName;
	BYTE Option;
	unsigned int Argument;
	unsigned int SessionCount;*/

	(*this) >> data.header;
	(*this) >> data.size;
	(*this) >> data.PacketType; //공통

	(*this) >> data.MessageType; //공통
	(*this) >> data.SessionNo; //공통

	(*this) >> data.userIdLength; //공통
	SetHashString(data.UserId,data.userIdLength);

	(*this) >> data.charNameLegnth;
	SetHashString(data.CharName,data.charNameLegnth);

	(*this) >> data.Option;
	(*this) >> data.Argument;
	(*this) >> data.SessionCount;

	return true;
}

bool SP2Packet::Read( OnNexonSynchronize& data )
{
	/*BYTE IsMonitoring;
	unsigned int Count;
	uint64_t SessionNo;
	BYTE userIdLength;
	ioHashString userId;*/

	(*this) >> data.header;
	(*this) >> data.size;
	(*this) >> data.PacketType; //공통

	(*this) >> data.IsMonitoring;
	(*this) >> data.Count;

	return true;
}

void SP2Packet::SetHashString( ioHashString& arg, unsigned char length )
{
	arg.SetBuffer(&m_pBuffer[m_currentPos],length);
	m_currentPos += length;
}

bool SP2Packet::Read(NexonEUCheckCashResponse& data)
{
	(*this) >> data.packetID;
	(*this) >> data.size;
	(*this) >> data.packetNo;
	(*this) >> data.packetType;	//공통

	(*this) >> data.result;
	(*this) >> data.balance;
	(*this) >> data.notRefundableBalance;
	return true;
}

bool SP2Packet::Read(NexonEUCheckCashAmountResponse& data)
{
	(*this) >> data.packetID;
	(*this) >> data.size;
	(*this) >> data.packetNo;
	(*this) >> data.packetType;	//공통

	(*this) >> data.result;
	(*this) >> data.balance;
	return true;
}

bool SP2Packet::Read(NexonEUPurchaseItemResponse& data)
{
	(*this) >> data.packetID;
	(*this) >> data.size;
	(*this) >> data.packetNo;
	(*this) >> data.packetType;	//공통

	(*this) >> data.orderIDlength;	//orderidlength
	data.orderID.SetBuffer(&m_pBuffer[m_currentPos], htons( data.orderIDlength) );
	m_currentPos += htons( data.orderIDlength );
	(*this) >> data.result;
	(*this) >> data.orderNo;
	(*this) >> data.productArrayLength;	
	(*this) >> data.productNo;
	(*this) >> data.orderQuantity;
	(*this) >> data.extendValuelength;	
	data.extendValue.SetBuffer(&m_pBuffer[m_currentPos], htons( data.extendValuelength) );
	m_currentPos += htons( data.extendValuelength );
	
	return true;
}

bool SP2Packet::Read(NexonEUInitializeResponse& data)
{
	(*this) >> data.packetID;
	(*this) >> data.size;
	(*this) >> data.packetNo;
	(*this) >> data.packetType;	//공통

	(*this) >> data.serviceCodeLength;	//orderidlength
	data.serviceCode.SetBuffer(&m_pBuffer[m_currentPos], htons( data.serviceCodeLength) );
	m_currentPos += htons( data.serviceCodeLength );
	(*this) >> data.serverNo;
	(*this) >> data.result;
	return true;
}

bool SP2Packet::Read(NexonEUSessionCheck& data)
{
	(*this) >> data.size;
	(*this) >> data.code;
	(*this) >> data.serial;
	return true;
}

bool SP2Packet::Read(NexonEUSession4Reply& data)
{
	(*this) >> data.size;
	(*this) >> data.code;
	(*this) >> data.serial;
	(*this) >> data.resultCode;
	(*this) >> data.nexonIDLength;
	data.nexonID.SetBuffer(&m_pBuffer[m_currentPos], data.nexonIDLength);
	m_currentPos += data.nexonIDLength;
	(*this) >> data.clientIPLength;
	data.clientIP.SetBuffer(&m_pBuffer[m_currentPos], data.clientIPLength );
	m_currentPos += data.clientIPLength;
	(*this) >> data.nexonSN;
	(*this) >> data.gender;
	(*this) >> data.age;
	(*this) >> data.nationCodeLength;
	data.nationCode.SetBuffer(&m_pBuffer[m_currentPos], data.nationCodeLength );
	m_currentPos += data.nationCodeLength;
	(*this) >> data.metaDataLength;
	data.metaData.SetBuffer(&m_pBuffer[m_currentPos], data.metaDataLength );
	m_currentPos += data.metaDataLength;
	(*this) >> data.secureCode;
	(*this) >> data.channelCode;
	(*this) >> data.channelUIDLength;
	data.channelUID.SetBuffer(&m_pBuffer[m_currentPos], data.channelUIDLength );
	m_currentPos += data.channelUIDLength;
	(*this) >> data.memberShip;
	(*this) >> data.mainAuthLevel;
	(*this) >> data.subAuthLevel;

	return true;
}

bool SP2Packet::Read(NexonEUGetPurchaseAmountResponse& data)
{
	(*this) >> data.packetID;
	(*this) >> data.size;
	(*this) >> data.packetNo;
	(*this) >> data.packetType;	//공통
	(*this) >> data.result;
	(*this) >> data.arrayLength;
	(*this) >> data.type;
	(*this) >> data.value;
	return true;
}


bool SP2Packet::Read(NexonProductInquiryResponse& data)
{
	(*this) >> data.packetID;
	(*this) >> data.size;
	(*this) >> data.packetNo;
	(*this) >> data.packetType;	//공통

	(*this) >> data.result;
	(*this) >> data.releaseTicks;
	(*this) >> data.totalProductCount;
	(*this) >> data.remainProductCount;
	(*this) >> data.resultXMLLength;

	data.resultXML.SetBuffer(&m_pBuffer[m_currentPos], htonl(data.resultXMLLength) );
	m_currentPos += htonl(data.resultXMLLength);
	/*
	(*this) >> data.packetID;
	(*this) >> data.size;
	(*this) >> data.packetNo;
	(*this) >> data.packetType;	//공통

	(*this) >> data.result;
	(*this) >> data.releaseTicks;
	(*this) >> data.totalProductCount;
	(*this) >> data.productArrayLength;
	(*this) >> data.productNo;
	(*this) >> data.relationProductNo;
	(*this) >> data.productExpire;
	(*this) >> data.productPieces;

	(*this) >> data.productIDLength;
	data.productID.SetBuffer(&m_pBuffer[m_currentPos], htons(data.productIDLength) );
	m_currentPos += htons(data.productIDLength);

	(*this) >> data.productGUIDLength;
	data.productGUID.SetBuffer(&m_pBuffer[m_currentPos], htons(data.productGUIDLength) );
	m_currentPos += htons( data.productGUIDLength );

	(*this) >> data.paymentType;
	(*this) >> data.productType;
	(*this) >> data.salePrice;
	(*this) >> data.categoryNo;
	//(*this) >> data.productStatus;
	(*this) >> data.bonusProductCount;
	(*this) >> data.productNo;

	(*this) >> data.extendLength;
	data.extend.SetBuffer(&m_pBuffer[m_currentPos], htons(data.extendLength) );
	m_currentPos += htons( data.extendLength );
	*/
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
