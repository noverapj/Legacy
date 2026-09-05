
#include "../StdAfx.h"
#include "Scheduler.h"
#include "SchedulerNode.h"
#include "../Network/ioPacketQueue.h"

#include "../DataBase/DBClient.h"
#include "../DataBase/LogDBClient.h"
#include "../NodeInfo/ServerNodeManager.h"
#include "../NodeInfo/MgrToolNodeManager.h"
#include "../NodeInfo/TradeNodeManager.h"
#include "../NodeInfo/SpecialShopManager.h"
#include "../NodeInfo/MatchNodeManager.h"

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
	case ITPK_FLUSH_PROCESS:
		OnFlushProcess( kPacket );
		break;

	case ITPK_TIMEOUT_TRADE:
		OnTradeProcess( kPacket );
		break;

	case ITPK_QUERY:
		OnQuery( kPacket );
		break;

	case ITPK_SPECIAL_SHOP_CHECK:
		OnSpecialShopCheck( kPacket );
		break;
		
	case ITPK_MATCH_PROCESS:
		OnMatchProcess( kPacket );
		break;

	case ITPK_MATCH_RANKSEASON:
		OnMatchRankSeason( kPacket );
		break;

	default:
		LOG.PrintTimeAndLog( 0, "0x%x Unknown CPacket",  kPacket.GetPacketID() );
		break;
	}
}

void SchedulerNode::Call(const int MSG)
{
	CPacket packet(MSG);
	ReceivePacket(packet);
}

void SchedulerNode::OnFlushProcess( SP2Packet &packet )
{
	g_LogDBClient.LogDBNode_SendBufferFlush();
	g_ServerNodeManager.ServerNode_SendBufferFlush();
	//g_MgrTool.MgrToolNode_SendBufferFlush();
}

void SchedulerNode::OnTradeProcess( SP2Packet &packet )
{
	g_TradeNodeManager.ProcessUpdateTrade();
}

void SchedulerNode::OnQuery( SP2Packet &packet )
{
	for(int i = 0 ; i < 100 ; i++)
	{
		g_DBClient.OnSelectPrevTournamentChampInfo( 0 );
	}

}

void SchedulerNode::OnSpecialShopCheck( SP2Packet &packet )
{
	//½ºÆÐ¼È»ð ¿ÀÇÂ, Å¬·ÎÁî Ã¼Å© ÈÄ ¼¥ Á¤º¸ Àü¼Û.
	g_SpecialShopManager.CheckShopState();
	if( g_SpecialShopManager.IsOpen() )
	{
		g_SpecialShopManager.SendGoodsUpdateInfo();
	}
}

void SchedulerNode::OnMatchProcess( SP2Packet &packet )
{
	g_MatchNodeManager.MatchProcess();
}

void SchedulerNode::OnMatchRankSeason( SP2Packet &packet )
{
	g_MatchNodeManager.RankSeasonProcess();
}