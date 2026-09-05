#include "../stdafx.h"
//#include "../MainProcess.h"
#include "../Network/ioPacketQueue.h"
#include "../NexonServer/NexonSessionServer.h"
#include "Scheduler.h"
#include "SchedulerNode.h"
#include "../Channeling/iochannelingnodemanager.h"

extern CLog LOG;

SchedulerNode::SchedulerNode(void)
{
	Init();
}

SchedulerNode::~SchedulerNode(void)
{
	Destroy();
}

void SchedulerNode::Init()
{
	//Default : 5분
	char szTemp[MAX_PATH]="";
	ioINILoader kLoader( "ls_config_billingsvr.ini" );
	kLoader.SetTitle( "BillInfoTimeout" );

	m_BillMgrTimeout = kLoader.LoadInt( "MaxAliveTime", 300000 );
	
}

void SchedulerNode::Destroy()
{
}

void SchedulerNode::ReceivePacket( CPacket &packet )
{
	 
	g_RecvQueue.InsertQueue( (DWORD)this, packet, PK_QUEUE_INTERNAL );
}

void SchedulerNode::PacketParsing( CPacket &packet )
{

	SP2Packet &kPacket = (SP2Packet&)packet;

	switch( packet.GetPacketID() )
	{
	case ITPK_TEST_TIMER:
		OnTestProcess(kPacket);
		break;

	case ITPK_NEXONALIVE:
		OnNexonAlive(kPacket);
		break;
	case ITPK_NEXON_SESSION_CHECK:
		OnNexonSessionCheck( kPacket);
		break;
	case ITPK_WRITEPROCESSLOG:
		OnWriteProcessLog( kPacket );
		break;
	case ITPK_TIMEOUT_BILLINFO:
		OnDeleteMaxAliveTimeBillInfo( kPacket );
		break;

	default:
		LOG.PrintTimeAndLog( 0, "[%s]0x%x Unknown CPacket",__FUNCTION__, kPacket.GetPacketID() );
		break;
	}
}

void SchedulerNode::Call(const int MSG)
{
	CPacket packet(MSG);
	ReceivePacket(packet);
}

void SchedulerNode::OnTestProcess( SP2Packet& packet )
{
	Tedata_ test_;
	test_.serverId = 11;
//	OPERATIONRUN(test_);
}

void SchedulerNode::OnRoomProcess( SP2Packet &packet )
{
	//모든 룸의 일괄 처리.
	//g_RoomNodeManager.RoomProcess();
}

void SchedulerNode::OnUserGhostProcess( SP2Packet &packet )
{
	//g_UserNodeManager.UserNode_GhostCheck();
}

void SchedulerNode::OnUserUpdateProcess( SP2Packet &packet )
{
	//g_UserNodeManager.UserNode_SaveCheck();
}

void SchedulerNode::OnMonsterCoinProcess( SP2Packet &packet )
{
	//유저 몬스터 코인 충전
	 
}

void SchedulerNode::OnSendBufferFlushProcess( SP2Packet &packet )
{
// 	g_ServerNodeManager.ProcessFlush();
// 	g_BillingRelayServer.ProcessFlush();
// 	g_MonitoringNodeManager.ProcessFlush();
// 	g_UserNodeManager.ProcessFlush();
// 	g_LogDBClient.ProcessFlush();
}

void SchedulerNode::OnTimerProcess( SP2Packet &packet )
{

}

void SchedulerNode::OnPingProcess( SP2Packet &packet )
{
	//g_DBClient.ProcessPing();
//	g_LogDBClient.ProcessPing();
//	g_ServerNodeManager.ProcessPing();
}

void SchedulerNode::OnNexonAlive( SP2Packet &packet )
{
	g_NexonSessionServer.SendAlivePacket();
}

void SchedulerNode::OnNexonSessionCheck( SP2Packet &packet )
{
	g_NexonSessionServer.CheckSessionServer();
} 

void SchedulerNode::OnWriteProcessLog( SP2Packet &packet )
{
	WriteProcessLog_ writeLog;
	OPERATIONRUN(writeLog);
}

void SchedulerNode::OnDeleteMaxAliveTimeBillInfo( SP2Packet &packet )
{
	g_BillInfoManager->DeleteMaxAliveTimeoutInfo( m_BillMgrTimeout );
	
}