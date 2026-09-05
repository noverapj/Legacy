// cConnector.cpp: implementation of the cConnector class.
//
//////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "cPacket.h"
#include "cBuffer.h"
#include "cSystem.h"
#include "cConnector.h"
#include "../include/cSingleton.h"




//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

cConnector::cConnector() : m_ID(-1), m_sequence(0), m_packet(NULL)
{
	Init();
}

cConnector::~cConnector()
{
	Destroy();
}

void cConnector::Init()
{
	uint32 maxPacket = cSingleton<cSystem>::GetInstance()->GetPacket();
	uint32 encoding = cSingleton<cSystem>::GetInstance()->GetEncoding();

	m_packet = new cPacket( encoding );
	m_storage.Create( maxPacket * 4 );
	m_temporary.Create( maxPacket * 4 );
}

void cConnector::Destroy()
{
	if(m_packet)
	{
		delete m_packet;
		m_packet = NULL;
	}
}

//////////////////////////////////////////////////////////////////////
// operation
//////////////////////////////////////////////////////////////////////

BOOL cConnector::SendPacket(	const uint32 command,
								const uint8* buffer,
								const uint32 length )
{
	m_packet->Make(	static_cast<uint16>(command), 
					buffer, 
					static_cast<uint16>(length),
					static_cast<uint8>(GetSequence()) );

	return Send( m_packet->GetBuffer(), m_packet->GetLength() );
}

void cConnector::OnConnected()
{
	cSystem* system = cSingleton<cSystem>::GetInstance();
		
	PushCommand( 
		system->GetConnectEvent(),
		NULL,
		0 );

	m_sequence = 0;
}


void cConnector::OnDisconnected()
{
	cSystem* system = cSingleton<cSystem>::GetInstance();
	
	PushCommand( 
		system->GetDisconnectEvent(),
		NULL,
		0 );
}

void cConnector::OnReceived( uint8* buffer, const uint32 length )
{
	// 임시버퍼에 보관된 데이타와 현재받은 데이타를 복사한다
	if( !m_temporary.Copy( m_storage.GetBuffer(), m_storage.GetLength() ) ) 
	{
		return;
	}
	if( !m_temporary.Append( buffer, length ) ) 
	{
		return;
	}
		 
	m_storage.Erase();

	// 임시버퍼에 저장된 패킷을 검사하고 프로토콜 별로 나누어서 command queue에 push
	CheckPacket( m_temporary.GetBuffer(), m_temporary.GetLength() );
}

void cConnector::CheckPacket( uint8* buffer, uint32 length )
{
	// 패킷의 유효성 검사
	if( length < sizeof(cHeader) )	
	{
		// 헤더보다 작은 패킷이 들어왔을 경우 처리하지 않는다
		m_storage.Copy( buffer, length );
		return;
	}
	
	// 헤더정보 검사
	cHeader* header = reinterpret_cast<cHeader*>(buffer);
	//if( !header->IsPerfect() ) // 2011-02-25 youngdie, 유효성 검사 삭제
	//{
	//	// 잘못된 헤더가 들어왔을때 패킷을 drop한다
	//	return;
	//}

	// 패킷 헤더에 따른 길이 검사
	uint32 packetLength = sizeof(cHeader) + header->GetPayload();
	if( length < packetLength ) // 패킷길이가 잘려서 들어왔다
	{
		// 저장하고 리턴
		m_storage.Copy( buffer, length );
	}
	else if( length == packetLength ) // 패킷길이가 정확하게 들어왔다
	{
		PushCommand(	header->GetCommand(),
						buffer + sizeof(cHeader), 
						header->GetPayload() );
	}
	else if( length > packetLength )	// 패킷길이가 붙어서 들어왔다
	{
		PushCommand(	header->GetCommand(),
						buffer + sizeof(cHeader), 
						header->GetPayload() );

		// 다음 버퍼 포인터로 이동하여 재검사
		CheckPacket( buffer + packetLength, length - packetLength );
	}
}

void cConnector::PushCommand( uint32 command, uint8* buffer, uint32 length )
{
	// 받은 패킷을 Command큐에 입력한다
	cCommandQueue* commandQueue = cSingleton<cSystem>::GetInstance()->GetCommandQueue();
	if( commandQueue )
	{
		commandQueue->PushCommand(		GetIdentity(),
										command,
										buffer,
										static_cast<uint16>(length) );
	}
}

