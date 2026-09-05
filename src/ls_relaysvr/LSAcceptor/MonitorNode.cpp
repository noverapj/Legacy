#include "StdAfx.h"
#include "MonitorNode.h"


MonitorNode::MonitorNode( SOCKET s, DWORD dwSendBufSize, DWORD dwRecvBufSize ) : CConnectNode(s, dwSendBufSize,dwRecvBufSize)
{
	if(g_Config()->GetUseSecurity() == 1) //kyg 다시 살릴것 
 		SetNS(new ioServerSecurity);
}
 
MonitorNode::MonitorNode() : CConnectNode(INVALID_SOCKET,DEFAULT_BUFFER,DEFAULT_BUFFER*2+1) 
{
	if(g_Config()->GetUseSecurity() == 1)
		SetNS(new ioServerSecurity);
}

MonitorNode::~MonitorNode(void)
{
}

void MonitorNode::OnCreate()
{
	CConnectNode::OnCreate();
	m_nodeType = 0;
	m_currentTime = std::clock();
 

	if( g_Config()->GetUseSecurity() )
	{		
		ioServerSecurity *pSS = (ioServerSecurity *)m_pNS;
		if( pSS ) 
		{
			pSS->InitDoSAttack( 30 );
			pSS->InitState( m_socket );		
		}
	}
}

void MonitorNode::OnDestroy()
{ 
	CConnectNode::OnDestroy();
}

bool MonitorNode::CheckNS( CPacket &rkPacket )
{
	if( m_pNS == NULL ) return true;

	ioServerSecurity *pSS = reinterpret_cast<ioServerSecurity*>(m_pNS);
	if( !pSS->IsCheckSum( rkPacket ) )
	{
 		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "User::CheckNS Check Sum Fail!! [%s : 0x%x]",
  			"EE", rkPacket.GetPacketID() );
		ExceptionClose( 0 );
		return false;
	}

	if( !pSS->CheckState( rkPacket ) )
	{
  		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "User::CheckNS State Not Same Client:%d, Server:%d [%s : 0x%x]", 
  			rkPacket.GetState(), pSS->GetRcvState(), "EE", rkPacket.GetPacketID() );
		ExceptionClose( 0 );
		return false;
	}

	if( !pSS->UpdateReceiveCount() )
 	{
  		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "User::CheckNS ONE SEC MANY PACKET(%d)!! [%s : 0x%x]", 
  			pSS->GetRcvCount(), "EE", rkPacket.GetPacketID() );

		ExceptionClose( 0 );
		return false;
	}

	return true;
}

int MonitorNode::GetConnectType()
{
	return m_nodeType;
}


bool MonitorNode::SendMessage( CPacket &rkPacket )
{
	return CConnectNode::SendMessage(rkPacket,TRUE);
}

void MonitorNode::SessionClose( BOOL safely )
{
	if(IsActive())
	{
		CPacket packet( Protocols::ITPK_CLOSE_SESSION );
		ReceivePacket( packet );
	}
 
}

void MonitorNode::ReceivePacket( CPacket &packet )
{
	if(g_Queue()->InsertQueue( (DWORD)this, packet, (PacketQueueTypes)PK_QUEUE_SESSION ) == false)
	{
		g_State()->PrintLowTime();
	}
}

void MonitorNode::PacketParsing( CPacket &packet )
{
 	SP2Packet &kPacket = (SP2Packet&)packet;
	FUNCTION_TIME_CHECKER( 100000.0f, kPacket.GetPacketID() );  
	switch( kPacket.GetPacketID() )
	{//OnAccept했을때 처리해줘야됨 
	case Protocols::ITPK_ACCEPT_SESSION:
		{
			OnMonitor(kPacket);
		}
		break;
	case Protocols::ITPK_CLOSE_SESSION:
		{
			OnClose();
		}
		break;
 
	}
}

void MonitorNode::OnClose()
{
	Debug(_T("OnClose\r\n"));
	g_MonitorMgr()->DelClient(this);
	OnDestroy();
}
 
void MonitorNode::SetConnectType(int nodetype)
{
	m_nodeType = nodetype;

}

void MonitorNode::OnMonitor( SP2Packet& kPacket )
{

}

 