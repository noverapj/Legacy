#include "StdAfx.h"
#include "ioTestBot.h"
#include "network/SP2Packet.h"
#include "ioClientSocketUDP.h"


extern void PacketProcess(ioTestBot* unit, SP2Packet& packet);


ioClientSocketUDP::ioClientSocketUDP(void)
{
	Init();
}

ioClientSocketUDP::~ioClientSocketUDP(void)
{
	Destroy();
}

void ioClientSocketUDP::Init()
{
}

void ioClientSocketUDP::Destroy()
{
}

void ioClientSocketUDP::OnConnect()
{
}

void ioClientSocketUDP::OnDisconnect()
{
}

void ioClientSocketUDP::OnSend()
{
}

void ioClientSocketUDP::OnReceive(char* buffer, int& length)
{
	CPacket kPacket( reinterpret_cast<uint8*>(buffer), length );
	if( kPacket.IsValidPacket() && (length >= kPacket.GetBufferSize()) )
	{
		if( CheckNS( kPacket ) )
		{
			SP2Packet &packet = (SP2Packet&)kPacket;
			PacketProcess( (ioTestBot*)this, packet );
		}
	}
	else
	{
		
	}
}

void ioClientSocketUDP::BeforeSend(CPacket &rkPacket)
{
	if( m_NS )
		m_NS->PrepareMsg( rkPacket );
}

void ioClientSocketUDP::AfterSend(CPacket &rkPacket)
{
	if( m_NS )
		m_NS->CompletionMsg( rkPacket );
}

BOOL ioClientSocketUDP::SendPacket(CPacket &rkPacket)
{
	if(IsConnected())
	{
		BeforeSend( rkPacket );

		BOOL result = Send( reinterpret_cast<const char*>(rkPacket.GetBuffer()), rkPacket.GetBufferSize() );
			
		AfterSend( rkPacket );
		return result;
	}
	return FALSE;
}

BOOL ioClientSocketUDP::CheckNS( CPacket &rkPacket )
{
	if( m_NS && m_NS->IsCheckSum( rkPacket ) && m_NS->CheckState( rkPacket ) )
	{
		return TRUE;
	}
	return FALSE;
}
