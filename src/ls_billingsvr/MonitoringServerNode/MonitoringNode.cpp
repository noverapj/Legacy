#include "../stdafx.h"
#include "../MainProcess.h"
#include "../Network/GameServer.h"
#include "../Network/ioPacketQueue.h"
#include "../Util/RingBuffer.h"
#include "MonitoringNodeManager.h"
#include "./monitoringnode.h"


extern CLog LOG;

MonitoringNode::MonitoringNode() : CConnectNode( NULL, 0, 0 )
{
	InitData();
}

MonitoringNode::MonitoringNode( SOCKET s, DWORD dwSendBufSize, DWORD dwRecvBufSize ) : CConnectNode( s, dwSendBufSize, dwRecvBufSize ) 
{
	InitData();
}

MonitoringNode::~MonitoringNode(void)
{
}


void MonitoringNode::InitData()
{
	m_eSessionState = SS_DISCONNECT;
	m_iLogCnt = 0;
}

void MonitoringNode::OnCreate()
{
	CConnectNode::OnCreate();
	InitData();
	m_eSessionState = SS_CONNECT;
}

void MonitoringNode::OnDestroy()
{
	LOG.PrintTimeAndLog( 0, "MonitoringNode::OnDestroy()" );
	CConnectNode::OnDestroy();
	m_eSessionState = SS_DISCONNECT;
}

void MonitoringNode::OnSessionDestroy()
{
}

void MonitoringNode::SessionClose(BOOL safely)
{
	CPacket packet(MNSTPK_CLOSE);
	ReceivePacket( packet );
}

bool MonitoringNode::SendMessage( CPacket &rkPacket )
{
	if( m_socket == INVALID_SOCKET ) return false;

	CPacket SendPacket( rkPacket.GetBuffer(), rkPacket.GetBufferSize() );
	return CConnectNode::SendMessage( SendPacket.GetBuffer() + SendPacket.GetCurPos(), SendPacket.GetBufferSize() - SendPacket.GetCurPos(), TRUE );
}

bool MonitoringNode::CheckNS( CPacket &rkPacket )
{
	return true;             //네트웍 감시 필요없다.
}

int MonitoringNode::GetConnectType()
{
	return CONNECT_TYPE_MONITORING;
}

void MonitoringNode::ReceivePacket( CPacket &packet )
{
	g_RecvQueue.InsertQueue( (DWORD)this, packet, PK_QUEUE_SESSION );
}

void MonitoringNode::PacketParsing( CPacket &packet )
{
	SP2Packet &kPacket = (SP2Packet&)packet;

	//FUNCTION_TIME_CHECKER( 500000.0f, kPacket.GetPacketID() );          // 0.5 초 이상 걸리면로그 남김

	switch( kPacket.GetPacketID() )
	{
	case MNSTPK_CLOSE:
		OnClose( kPacket );
		break;
	case MNSTPK_STATUS_REQUEST:
		OnStatus( kPacket );
		break;
	case MNSTPK_CHANGE_REQUEST:
		OnChange( kPacket );
		break;
	default:
		LOG.PrintTimeAndLog( 0, "MonitoringServer::PacketParsing 알수없는 패킷 : 0x%x", kPacket.GetPacketID() );
		break;
	}
}

void MonitoringNode::DispatchReceive( CPacket& packet, DWORD bytesTransferred )
{
	m_recvIO.AddBytesTransferred( bytesTransferred );
	int loopCount = 0;
	while( m_recvIO.GetBytesTransferred() > 0 )
	{
		loopCount ++;
		// 헤더가 없는 패킷에 헤더를 더한다. CConnectNode::DispatchReceive() 이부분만 다름.
		if( m_recvIO.GetBytesTransferred() < 4 ) // 4: m_usCommand + m_usSize
			break;

		unsigned short usCommand = 0;
		unsigned short m_usSize  = 0;
		m_recvIO.GetBuffer( &usCommand, 2, 0 );
		m_recvIO.GetBuffer( &m_usSize, 2, 4 );

		DWORD dwPacketSize = 0;
		DWORD dwPacketID   = 0;
		if( usCommand == MONITORING_STATUS_CMD )
		{
			dwPacketID   = MNSTPK_STATUS_REQUEST; // 우리 패킷 ID로 변환
			dwPacketSize = m_usSize; 
			if( !COMPARE( dwPacketSize, 0, sizeof( MonitorStatusRequest ) + 1 ) )
			{
				LOG.PrintTimeAndLog( 0, "%s MNSTPK_STATUS_REQUEST Error Size : %d", __FUNCTION__, dwPacketSize );
				break;
			}
		}
		else if( usCommand == MONITORING_CHANGE_CMD )
		{
			dwPacketID = MNSTPK_CHANGE_REQUEST; // 우리 패킷 ID로 변환
			dwPacketSize = m_usSize; 
			if( !COMPARE( dwPacketSize, 0, sizeof( tagMonitorChangeRequest ) + 1 ) )
			{
				LOG.PrintTimeAndLog( 0, "%s MNSTPK_CHANGE_REQUEST Error Size : %d", __FUNCTION__, dwPacketSize );
				break;
			}
		}
		else
		{
			LOG.PrintTimeAndLog( 0, "%s Error ID : %d", __FUNCTION__, usCommand );
			break;
		}

		SP2Packet RecvPacket( dwPacketID );
		RecvPacket.SetDataAdd( (char*)m_recvIO.GetBuffer(), min( dwPacketSize, m_recvIO.GetBytesTransferred() ), true );

		if( (RecvPacket.IsValidPacket() == true) && (m_recvIO.GetBytesTransferred() >= dwPacketSize) )
		{
			if( !CheckNS( RecvPacket ) ) return;

			ReceivePacket( RecvPacket );

			m_recvIO.AfterReceive( dwPacketSize );
		}
		else 
			break;

		if(loopCount > 200)
		{
			LOG.PrintTimeAndLog(0,"Error %s LoopCountOver(%d:%d)",dwPacketSize,bytesTransferred);
			m_recvIO.AfterReceive(bytesTransferred);
			break;
		}
	}

	WaitForPacketReceive();
}

void MonitoringNode::OnClose( SP2Packet &rkPacket )
{
	if(!IsDisconnectState())
	{
		OnDestroy();
		OnSessionDestroy();
		
		g_MonitoringNodeManager.RemoveNode(this);
	}
}

void MonitoringNode::OnStatus( SP2Packet &rkPacket )
{
	MonitorStatusResult kStatus;
	// 게임서버 종료 처리 중
	if( g_App.IsReserveLogOut() )
		kStatus.m_cStatus = MonitorStatusResult::STATUS_EXITING;
	else
		kStatus.m_cStatus = MonitorStatusResult::STATUS_RUN;

	kStatus.m_iCuCount = 1;//g_UserNodeManager.GetNodeSize();

	m_iLogCnt++;
	if( (m_iLogCnt%100) == 0 )
	{
		LOG.PrintTimeAndLog( 0, "%s %d", __FUNCTION__, kStatus.m_iCuCount );
		m_iLogCnt = 0;
	}
	SP2Packet kResult( MNSTPK_STATUS_RESULT );
	kResult << kStatus;
	SendMessage( kResult );
}

void MonitoringNode::OnChange( SP2Packet &rkPacket )
{
	MonitorChangeRequest kChange;
	rkPacket >> kChange;

	MonitorChangeResult kRChagne;

	if( kChange.m_iReqStatus == MonitorChangeRequest::CHANGE_OPEN ) 
	{
		// 기능 없음
		kRChagne.m_iResult = MonitorChangeResult::CHANGE_FAIL;
	}
	else if( kChange.m_iReqStatus == MonitorChangeRequest::CHANGE_BLOCK ) 
	{
		// 기능 없음
		kRChagne.m_iResult = MonitorChangeResult::CHANGE_FAIL;
	}
	else if( kChange.m_iReqStatus == MonitorChangeRequest::CHANGE_EXIT ) 
	{
		g_App.CrashDown();
		kRChagne.m_iResult = MonitorChangeResult::CHANGE_SUCCESS;
	}

	LOG.PrintTimeAndLog( 0, "%s %d", __FUNCTION__, kChange.m_iReqStatus );
	SP2Packet kResult( MNSTPK_CHANGE_RESULT );
	kResult << kRChagne;
	SendMessage( kResult );
}