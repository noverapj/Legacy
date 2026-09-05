#include "../stdafx.h"
#include "../Network/GameServer.h"
#include "../Network/ioPacketQueue.h"

#include "../Util/Ringbuffer.h"
#include "ThailandOTPNode.h"
#include "../NodeInfo/ServerNodeManager.h"
#include "../Local/ioLocalParent.h"
#include "../Local/ioLocalThailand.h"


extern CLog LOG;

ThailandOTPNode::ThailandOTPNode() : CConnectNode( NULL, 0, 0)
{
	InitData();
}

ThailandOTPNode::ThailandOTPNode( SOCKET s, DWORD dwSendBufSize, DWORD dwRecvBufSize )  : CConnectNode( s, dwSendBufSize, dwRecvBufSize )
{
	InitData();
}

ThailandOTPNode::~ThailandOTPNode(void)
{
}

void ThailandOTPNode::InitData()
{
	m_eSessionState = SS_DISCONNECT;
	m_dwConnectTime = 0;
	SetActive(false);
}

bool ThailandOTPNode::SendMessage( CPacket &rkPacket )
{
	if( m_socket == INVALID_SOCKET ) return false;

	CPacket SendPacket( rkPacket.GetBuffer(), rkPacket.GetBufferSize() );
	return CConnectNode::SendMessage( SendPacket.GetBuffer() + SendPacket.GetCurPos(), SendPacket.GetBufferSize() - SendPacket.GetCurPos(), TRUE );
}


void ThailandOTPNode::DispatchReceive(CPacket& packet, DWORD bytesTransferred)
{
	m_recvIO.AddBytesTransferred( bytesTransferred );

	while( m_recvIO.GetBytesTransferred() > 0 )
	{
		// 헤더가 없는 패킷에 헤더를 더한다. CConnectNode::DispatchReceive() 이부분만 다름.
		// |Command|Packet Length|Status|Message|EndPoint|
		// |1002|28|true|Success|9999|
		// |1002|31|false|Invalid OTP|9999|
		DWORD dwPacketSize = 0;
		int   iTokenCnt = 0;
		const char* buffer = m_recvIO.GetBuffer();
		for (int i = 0; i < (int)m_recvIO.GetBytesTransferred(); i++)
		{
			if( buffer[i] == THAILAND_TOKEN )
			{
				iTokenCnt++;
				if( iTokenCnt == 6 )
				{
					if( strncmp( &(buffer[i-4]), "9999", 4 ) == 0 ) // 9999 는 endpoint
					{
						dwPacketSize = i+1;
						break;
					}
				}
			}
		}

		if( dwPacketSize == 0 )
			break;

		SP2Packet RecvPacket( BTTPK_OTP_RESULT );
		RecvPacket.SetDataAdd( (char*)m_recvIO.GetBuffer(), min( dwPacketSize, m_recvIO.GetBytesTransferred() ), true );
		if( RecvPacket.IsValidPacket() == true && m_recvIO.GetBytesTransferred() >= dwPacketSize )
		{
			// TEST
			//char szLog[2000]="";
			//for (int i = 0; i < (int)m_RecvIO.m_dwBytesTransferred; i++)
			//{
			//	char szOneLog[MAX_PATH]="";
			//	StringCbPrintf( szOneLog, sizeof( szOneLog ), "%d|", m_RecvIO.m_aBuffer[i] );
			//	StringCbCat( szLog, sizeof( szLog ), szOneLog );
			//}
			//LOG.PrintTimeAndLog( 0, "%s [%d][%s]", __FUNCTION__, m_RecvIO.m_dwBytesTransferred, szLog );
			//
			if( !CheckNS( RecvPacket ) ) return;

			ReceivePacket( RecvPacket );

			m_recvIO.AfterReceive( dwPacketSize );
		}
		else 
			break;
	}

	WaitForPacketReceive();
}


void ThailandOTPNode::ReceivePacket( CPacket &packet )
{
	g_RecvQueue.InsertQueue( (DWORD)this, packet, PK_QUEUE_SESSION );
}

void ThailandOTPNode::PacketParsing( CPacket &packet )
{
	SP2Packet &kPacket = (SP2Packet&)packet;

	switch( kPacket.GetPacketID() )
	{
	case ITPK_CLOSE_SESSION:
		{
			OnClose(kPacket);
		}
		break;

	case BTTPK_OTP_RESULT:
		OnOTP( kPacket );
		break;
	default:
		LOG.PrintTimeAndLog( 0, "ThailandOTPNode::PacketParsing 알수없는 패킷 : 0x%x", kPacket.GetPacketID() );
		break;
	}
}

void ThailandOTPNode::OnCreate()
{
	CConnectNode::OnCreate();
	m_eSessionState = SS_CONNECT;
	m_dwConnectTime = TIMEGETTIME();
	m_User.SetAlive( true );
	m_User.SetSocket( GetSocket() );

	if( !AfterCreate() )
	{
		LOG.PrintTimeAndLog(0,"Error %s::OnCreate",__FUNCTION__);
	}
}

void ThailandOTPNode::OnDestroy()
{
	CConnectNode::OnDestroy();
	m_eSessionState = SS_DISCONNECT;
	m_dwConnectTime = 0;
	m_User.Clear();
}

bool ThailandOTPNode::CheckNS( CPacket &rkPacket )
{
	return true;             //네트웍 감시 필요없다.
}

int ThailandOTPNode::GetConnectType()
{
	return CONNECT_TYPE_THAILAND_OTP_SERVER;
}

void ThailandOTPNode::OnOTP( SP2Packet &rkPacket )
{
	if( GetSocket() != m_User.GetSocket() || !m_User.IsAlive() )
	{
		LOG.PrintTimeAndLog( 0, "%s Error: NodeSocket|%d : Socket|%d : ID|%s : GUID|%s :%d", __FUNCTION__ , GetSocket(), m_User.GetSocket() , m_User.GetPrivateID().c_str(), m_User.GetBillingGUID().c_str(), (int) m_User.IsAlive() );
		return;
	}

	ioHashString sResult;
	rkPacket >> sResult;
	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( pLocal )
		pLocal->OnRecieveOTP( sResult, m_User );

	OnDestroy();
}

void ThailandOTPNode::SendUserMessage()
{
	if( GetSocket() != m_User.GetSocket() || !m_User.IsAlive() )
	{
		LOG.PrintTimeAndLog( 0, "%s Error: NodeSocket|%d : Socket|%d : ID|%s : GUID|%s :%d", __FUNCTION__ , GetSocket(), m_User.GetSocket() , m_User.GetPrivateID().c_str(), m_User.GetBillingGUID().c_str(), (int) m_User.IsAlive() );
		return;
	}

	SP2Packet kPacket( BSTPK_OTP_RESULT );
	kPacket << m_User.GetPrivateID();
	kPacket << m_User.GetBillingGUID();
	kPacket << BILLING_OTP_RESULT_FAIL;
	if( !g_ServerNodeManager.SendMessageIP( (ioHashString) m_User.GetServerIP(),  m_User.GetServerPort(), kPacket ) )
	{
		LOG.PrintTimeAndLog( 0, "%s Send Fail %s : %d", __FUNCTION__, m_User.GetServerIP().c_str(), m_User.GetServerPort() );
	}
}

void ThailandOTPNode::SessionClose( BOOL safely/*=TRUE */ )
{
	CPacket packet(ITPK_CLOSE_SESSION);
	ReceivePacket( packet );
}

void ThailandOTPNode::OnClose( SP2Packet &rkPacket )
{ 
	if(IsActive())
		OnDestroy();
}