#include "../stdafx.h"
#include "./iolocalkorea.h"
#include "../Channeling\ioChannelingNodeManager.h"
#include "../Channeling\ioChannelingNodeParent.h"

ioLocalKorea::ioLocalKorea(void)
{
}

ioLocalKorea::~ioLocalKorea(void)
{
}

ioLocalManager::LocalType ioLocalKorea::GetType()
{
	return ioLocalManager::LCT_KOREA;
}

void ioLocalKorea::OnLoginData( ServerNode *pServerNode, SP2Packet &rkPacket )
{

}

void ioLocalKorea::_OnGetCash( ServerNode *pServerNode, SP2Packet &rkPacket )
{

}

void ioLocalKorea::_OnOutputCash( ServerNode *pServerNode, SP2Packet &rkPacket, DWORD dwGoodsNo, const ioHashString &rszGoodsName )
{

}

void ioLocalKorea::OnRefundCash( ServerNode *pServerNode, SP2Packet &rkPacket )
{

}

void ioLocalKorea::OnUserInfo( ServerNode *pServerNode, SP2Packet &rkPacket )
{

}

void ioLocalKorea::OnCancelCash( ServerNode *pServerNode, SP2Packet &rkPacket )
{
	int iChannelingType = 0;
	rkPacket >> iChannelingType;
	ioChannelingNodeParent *pNode = g_ChannelingMgr.GetNode( (ChannelingType) iChannelingType );
	if( pNode )
	{
		rkPacket.SetPosBegin();
		pNode->OnCancelCash( pServerNode, rkPacket );
	}
}

void ioLocalKorea::OnAddCash( ServerNode *pServerNode, SP2Packet &rkPacket )
{
	int iChannelingType = 0;
	rkPacket >> iChannelingType;
	ioChannelingNodeParent *pNode = g_ChannelingMgr.GetNode( (ChannelingType) iChannelingType );
	if( pNode )
	{
		rkPacket.SetPosBegin();
		pNode->OnAddCash( pServerNode, rkPacket );
	}
}

void ioLocalKorea::OnFillCashUrl( ServerNode *pServerNode, SP2Packet &rkPacket )
{
	int iChannelingType = 0;
	rkPacket >> iChannelingType;
	ioChannelingNodeParent *pNode = g_ChannelingMgr.GetNode( (ChannelingType) iChannelingType );
	if( pNode )
	{
		rkPacket.SetPosBegin();
		pNode->OnFillCashUrl( pServerNode, rkPacket );
	}
}