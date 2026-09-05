#include "../stdafx.h"
#include "../NodeInfo/ServerNodeManager.h"
#include "ioPacketQueue.h"

ioPacketQueue *ioPacketQueue::sg_Instance = NULL;

ioPacketQueue::ioPacketQueue()	{}
ioPacketQueue::~ioPacketQueue()	{}

ioPacketQueue &ioPacketQueue::GetInstance()
{
	if( sg_Instance == NULL )
		sg_Instance = new ioPacketQueue;
	return *sg_Instance;
}

void ioPacketQueue::ReleaseInstance()
{
	SAFEDELETE( sg_Instance );
}

void ioPacketQueue::Initialize()
{
	ioINILoader kLoader( "ls_config_main.ini" );
	kLoader.SetTitle( "RecvQueue" );
	DWORD dwFirstRecvQueue = kLoader.LoadInt( "FirstQueue", 100 );
	SetMemoryPool( dwFirstRecvQueue );
}

void ioPacketQueue::ParseSession( PacketQueue *pq )
{
	__try
	{
		//서버 패킷 처리
		CConnectNode *pSessionNode = (CConnectNode*)pq->GetNode();
		if( pSessionNode && pSessionNode->IsActive() )
		{
			m_SessionPacket.SetBufferCopy( pq->GetBuffer(), pq->GetSize(), pq->GetPosition() );
			g_PacketChecker.SessionPacket( m_SessionPacket.GetPacketID() );	
			pSessionNode->PacketParsing( m_SessionPacket );
		}
	}
	__except (ExceptCallBack (GetExceptionInformation()))
	{
		LOG.PrintTimeAndLog( 0, "ParseSession Crash!!" );
	}
}

void ioPacketQueue::ParseQuery( PacketQueue *pq )
{
	__try
	{
		m_QueryPacket.SetBufferCopy( pq->GetBuffer(), pq->GetSize(), pq->GetPosition() );
		g_ServerNodeManager.GlobalQueryParse( (SP2Packet&)m_QueryPacket );
	}
	__except (ExceptCallBack (GetExceptionInformation()))
	{
		//
		LOG.PrintTimeAndLog( 0, "ParseQuery Crash!!" );
	}
}

void ioPacketQueue::ParseInternal( PacketQueue *pq )
{
	DWORD dwPacketID = 0;
	__try
	{
		CCommandNode *pCommandNode = (CCommandNode*)pq->GetNode();
		if( pCommandNode )
		{
			m_SessionPacket.SetBufferCopy( pq->GetBuffer(), pq->GetSize(), pq->GetPosition() );
			dwPacketID = m_SessionPacket.GetPacketID();

			pCommandNode->PacketParsing( m_SessionPacket );
		}
		dwPacketID = 0;
	}
	__except (ExceptCallBack (GetExceptionInformation()))
	{
		//
		LOG.PrintTimeAndLog( 0, "ParseSession Crash!! - Packet : 0x%x", dwPacketID );
	}
}

void ioPacketQueue::ParseAccept( PacketQueue *pq )
{
	DWORD dwPacketID = 0;
	__try
	{
		CCommandNode *pCommandNode = (CCommandNode*)pq->GetNode();
		if( pCommandNode )
		{
			PacketQueueTypeAccept* acceptor = (PacketQueueTypeAccept*)pq;

			m_SessionPacket.SetBufferCopy( pq->GetBuffer(), pq->GetSize(), pq->GetPosition() );
			dwPacketID = m_SessionPacket.GetPacketID();

			pCommandNode->PacketParsing( m_SessionPacket, acceptor->GetSocketHandle() );
		}
		dwPacketID = 0;
	}
	__except (ExceptCallBack (GetExceptionInformation()))
	{
		LOG.PrintTimeAndLog( 0, "ParseAccept Crash!! - Packet : 0x%x", dwPacketID );
	}
}