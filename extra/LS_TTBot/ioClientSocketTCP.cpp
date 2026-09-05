#include "StdAfx.h"
#include "ioTestBot.h"
#include "network/SecurityTCP.h"
#include "ioClientSocketTCP.h"
#include "protocol.h"
#include "LS_TTBotDlg.h"


extern void PacketProcess(ioTestBot* unit, SP2Packet& packet);


ioClientSocketTCP::ioClientSocketTCP(void) : m_NS(NULL), m_storage(NULL)
{
	Init();
}

ioClientSocketTCP::~ioClientSocketTCP(void)
{
	Destroy();
}

void ioClientSocketTCP::Init()
{
	//m_storage = new cBuffer(1024 * 4);
	m_storage = new cBuffer(1024 * 64);// 16384
}

void ioClientSocketTCP::Destroy()
{
	if(m_NS)
	{
		delete m_NS;
		m_NS = NULL;
	}
}

void ioClientSocketTCP::OnConnect()
{
	Startup();
	//SetNS(new SecurityTCP);
	SecurityTCP *pCS = reinterpret_cast<SecurityTCP*>(m_NS);
	 
	if( pCS ) 
	{
		pCS->InitState();		
	}
}

void ioClientSocketTCP::OnDisconnect()
{
}

void ioClientSocketTCP::OnSend()
{
}

void ioClientSocketTCP::OnReceive(char* buffer, int& length)
{
	if( length > 0 )
	{
		if(!m_storage->Append(reinterpret_cast<uint8*>(buffer), length))
		{
			// 버퍼가 모자르다
			TRACE("buffer error(%lu)", m_storage->GetLength());
			MessageBox( NULL, "buffer error", "error", MB_OK );
			exit(1);
			return;
		}

		int index = 0;
		while( m_storage->GetLength() > 0 )
		{
			if( m_storage->GetLength() < sizeof(CPacket::PACKETHEADER) )
			{
				break;
			}

			CPacket kPacket( m_storage->GetBuffer(), m_storage->GetLength() );
			if( !kPacket.IsValidPacket() )
			{
				//받으려는 사이즈보다 적게 들어왔다
				//((CLS_TTBotDlg*)AfxGetMainWnd())->LOG("header error(%lu)", m_storage->GetLength());
				TRACE( "length : %lu, require : %lu\n", m_storage->GetLength(), kPacket.GetBufferSize() );
				break;
			}

			if( m_storage->GetLength() > kPacket.GetBufferSize() )
			{
				//받으려는 사이즈보다 크게 들어왔다
				if(kPacket.GetPacketID() != DTPK_QUERY)
				{
					MessageBox( NULL, "packet error", "error", MB_OK );
				}

 				SP2Packet packet = (SP2Packet&)kPacket;
				int bufferSize = kPacket.GetBufferSize();
 				PacketProcess( (ioTestBot*)this, packet );
 
				int length = m_storage->GetLength() - bufferSize;
				char *temp = new char[bufferSize];
				CopyMemory(temp, m_storage->GetBuffer(bufferSize), length);
				if(length >= sizeof(CPacket::PACKETHEADER))
				{
					CPacket packet( (uint8*)temp, length );
					if( packet.GetPacketID() != 1 )
					{
						int b;
						b = 0;
					}

				}
				if(m_storage->GetLength() > 35000)
				{
					int a;
					a = 0;
				}
				m_storage->Erase();
				m_storage->Copy((uint8*)temp, length);

 				//m_storage->Shift( bufferSize );
			}
			else if( m_storage->GetLength() == kPacket.GetBufferSize() )
			{
				if(kPacket.GetPacketID() != DTPK_QUERY)
				{
					MessageBox( NULL, "packet error", "error", MB_OK );
				}

 				SP2Packet packet = (SP2Packet&)kPacket;
				int bufferSize = kPacket.GetBufferSize();
				PacketProcess( (ioTestBot*)this, packet );
				m_storage->Erase();
				break;
			}
			else
			{
				break;
			}
		}
	}
	else
	{
		m_storage->Erase();
		// 소켓에러
	}
}

void ioClientSocketTCP::BeforeSend(CPacket &rkPacket)
{
	if( m_NS )
		m_NS->PrepareMsg( rkPacket );
}

void ioClientSocketTCP::AfterSend(CPacket &rkPacket)
{
	if( m_NS )
		m_NS->CompletionMsg( rkPacket );
}


BOOL ioClientSocketTCP::SendPacket(CPacket &rkPacket)
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

BOOL ioClientSocketTCP::CheckNS( CPacket &rkPacket )
{
	if( m_NS && m_NS->IsCheckSum( rkPacket ) && m_NS->CheckState( rkPacket ) )
	{
		return TRUE;
	}
	return FALSE;
}
