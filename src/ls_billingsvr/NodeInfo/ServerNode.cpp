#include "../stdafx.h"

#include "../MainProcess.h"

#include "../Network/GameServer.h"
#include "../Network/ioPacketQueue.h"
#include "../channeling/iochannelingnodemanager.h"
#include "../channeling/iochannelingnodeparent.h"

#include "ServerNodeManager.h"
#include "ServerNode.h"
#include "GoodsManager.h"
#include <strsafe.h>
#include "../local/iolocalparent.h"
#include "../local/iolocalmanager.h"
#include "../ioProcessChecker.h"


extern CLog LOG;

ServerNode::ServerNode() : CConnectNode(NULL,0,0)
{
	InitData();
}

ServerNode::ServerNode( SOCKET s, DWORD dwSendBufSize, DWORD dwRecvBufSize ) : CConnectNode( s, dwSendBufSize, dwRecvBufSize )
{
	InitData();
}

ServerNode::~ServerNode()
{	
}

void ServerNode::InitData()
{
	m_iClientPort = 0;
	m_eSessionState = SS_DISCONNECT;
	m_serverIndex = 0;
	SetSendLoginInfo(FALSE);
}

void ServerNode::OnCreate()
{
	CConnectNode::OnCreate();
	m_eSessionState = SS_CONNECT;
	m_serverIndex = 0;
	SetSendLoginInfo(FALSE);
}

void ServerNode::OnDestroy()
{
	CConnectNode::OnDestroy();
	m_eSessionState = SS_DISCONNECT;
	LOG.PrintTimeAndLog(0, "[warning][servernode]Disconnect : %s:%d", m_szServerIP.c_str(), m_iClientPort );
}

bool ServerNode::CheckNS( CPacket &rkPacket )
{
	return true;             //네트웍 감시 필요없다.
}

int ServerNode::GetConnectType()
{
	return CONNECT_TYPE_GAMESERVER;
}

void ServerNode::ReceivePacket( CPacket &packet )
{
	g_RecvQueue.InsertQueue( (DWORD)this, packet, PK_QUEUE_SESSION );
}

void ServerNode::PacketParsing( CPacket &packet )
{
	__try
	{
		SP2Packet &kPacket = (SP2Packet&)packet;

		switch( kPacket.GetPacketID() )
		{
		case SSTPK_CLOSE:
			{
				OnClose(kPacket);
			}
			break;
		case BSTPK_GET_CASH:
			g_ProcessChecker.BroadcastThreadCheckTimeStart();
			g_ProcessChecker.CallUDPThread();		
			OnGetCash( kPacket );
			g_ProcessChecker.BroadcastThreadCheckTimeEnd();
			break;
		case BSTPK_OUTPUT_CASH:
			g_ProcessChecker.ClientAThreadCheckTimeStart();
			g_ProcessChecker.CallClientAccept();
			OnOutputCash( kPacket );
			g_ProcessChecker.ClientAThreadCheckTimeEnd();
			break;
		case BSTPK_SERVER_IPPORT:
			g_ProcessChecker.ServerAThreadCheckTimeStart();
			g_ProcessChecker.CallServerAccept();
			OnServerIPPort( kPacket );
			g_ProcessChecker.ServerAThreadCheckTimeEnd();
			break;
		case BSTPK_LOGIN:
			g_ProcessChecker.MonitoringAThreadCheckTimeStart();
			g_ProcessChecker.CallMonitoringAccept();
			OnLogin( kPacket );
			g_ProcessChecker.MonitoringAThreadCheckTimeEnd();
			break;
		case BSTPK_REFUND_CASH:
			OnRefundCash( kPacket );
			g_ProcessChecker.UDPRecvMessage( kPacket.GetBufferSize() );
			break;
		case BSTPK_USER_INFO:
			OnUserInfo( kPacket );
			g_ProcessChecker.UserRecvMessage( kPacket.GetBufferSize() );
			break;
		case BSTPK_PCROOM:
			OnPCRoom( kPacket );
			break;
		case BSTPK_OTP:
			OnOTP( kPacket );
			break;
		case BSTPK_GET_MILEAGE:
			OnGetMileage( kPacket );
			break;
		case BSTPK_ADD_MILEAGE:
			OnAddMileage( kPacket );
			break;
		case BSTPK_IPBONUS:
			OnIPBonus( kPacket );
			break;
		case BSTPK_CANCEL_CASH:
			OnCancelCash( kPacket );
			break;
		case BSTPK_IPBONUS_OUT:
			OnIPBonusOut( kPacket );
			break;
		case BSTPK_ADD_CASH:
			OnAddCash( kPacket );
			break;
		case BSTPK_FILL_CASH_URL:
			OnFillCashUrl( kPacket );
			break;
		case BSTPK_SUBSCRIPTION_RETRACT_CASH_CHECK:
			{
				OnSubscriptionRetractCheck( kPacket );
			}
			break;
		case BSTPK_SUBSCRIPTION_RETRACT_CASH:
			{
				OnSubscriptionRetract( kPacket );
			}
			break;
		case BSTPK_SESSION_CONTROL:
			{
				OnSessionControl( kPacket );
			}
			break;

		case BSTPK_FIRST_LOGIN:
			{
				OnSessionFirstLogin( kPacket );
			}
			break;
		case BSTPK_LOGOUT : 
			{
				//hr 라틴추가
				OnSessionLogoutLog( kPacket );
			}
			break;

		case BSTPK_CCU_COUNT :
			{
				OnGetCCUCount( kPacket );
			}
			break;

		case BSTPK_DAUM_SHUTDOWN_CHECK:
			{
				OnDaumShutDownCheck( kPacket );
			}
			break;

		case BSTPK_REQUEST_TIME_CASH:
			{
				OnRequestTimeCash( kPacket );
			}
			break;

		default:
			LOG.PrintTimeAndLog( 0, "ServerNode::PacketParsing 알수없는 패킷 : 0x%x", kPacket.GetPacketID() );
			break;
		}
	}
	__except(1)
	{
		LOG.PrintTimeAndLog(0,"crash ServerNode PacketID :%x ",packet.GetPacketID());
	}
}

void ServerNode::OnGetCash( SP2Packet &rkPacket )
{
	int         iChannelingType   = 0;
	
	__try
	{
		rkPacket >> iChannelingType;

	ioChannelingNodeParent *pNode = g_ChannelingMgr.GetNode( (ChannelingType) iChannelingType );
	if( !pNode )
	{
		LOG.PrintTimeAndLog( 0, "%s Channeling Type is wrong.%d", __FUNCTION__, iChannelingType );
		return;
	}

	rkPacket.SetPosBegin(); // Pos to Begin
	pNode->OnGetCash( this, rkPacket );		
	}
	__except(1)
	{	
		LOG.PrintTimeAndLog(0,"Crash NodeType :%d ",iChannelingType);
	}
}

void ServerNode::OnOutputCash( SP2Packet &rkPacket )
{
 	int         iChannelingType   = 0;
	rkPacket >> iChannelingType;
	ioChannelingNodeParent *pNode = g_ChannelingMgr.GetNode( (ChannelingType) iChannelingType );
	if( !pNode )
	{
		LOG.PrintTimeAndLog( 0, "%s Channeling Type is wrong.%d", __FUNCTION__, iChannelingType );
		return;
	}

	rkPacket.SetPosBegin(); // Pos to Begin
	pNode->OnOutputCash( this, rkPacket );
}

void ServerNode::OnServerIPPort( SP2Packet &rkPacket )
{
	rkPacket >> m_szServerIP >> m_iClientPort >> m_serverIndex;
	LOG.PrintTimeAndLog(0, "[info][servernode]Server Connect : %s:%d::%d", m_szServerIP.c_str(), m_iClientPort, m_serverIndex );
	
}

void ServerNode::OnLogin( SP2Packet &rkPacket )
{
	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( !pLocal )
	{
		ioHashString szBillingGUID;
		ioHashString szPrivateID;
		ioHashString szUserKey;

		rkPacket >> szBillingGUID >> szPrivateID >> szUserKey;
		LOG.PrintTimeAndLog(0, "%s pLocal == NULL:%s:%s:%s", __FUNCTION__, szBillingGUID.c_str(), szPrivateID.c_str(), szUserKey.c_str() );
		return;
	}
	
	pLocal->OnLoginData( this, rkPacket );
}

void ServerNode::OnRefundCash( SP2Packet &rkPacket )
{
	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( !pLocal )
	{
		DWORD        dwUserIndex = 0;
		ioHashString szBillingGUID;
		ioHashString szPrivateID;
		ioHashString szPublicID;
		ioHashString szUserKey;
		int          iTransactionID = 0;
		bool         bRefund        = false;

		rkPacket >> szBillingGUID >> dwUserIndex >> szPrivateID >> szPublicID >> szUserKey >> iTransactionID >> bRefund;
		LOG.PrintTimeAndLog(0, "%s pLocal == NULL:%s:%d:%s:%s:%d:%d", __FUNCTION__, szBillingGUID.c_str(), dwUserIndex, szPrivateID.c_str(), szPublicID.c_str(), iTransactionID , bRefund );
		return;
	}

	pLocal->OnRefundCash( this, rkPacket );
}

void ServerNode::OnUserInfo( SP2Packet &rkPacket )
{
	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( !pLocal )
	{
		ioHashString szBillingGUID;
		ioHashString szPrivateID;
		ioHashString szUserKey;
		
		rkPacket >> szBillingGUID >>  szPrivateID >> szUserKey;
		LOG.PrintTimeAndLog(0, "%s pLocal == NULL:%s:%s", __FUNCTION__, szBillingGUID.c_str(), szPrivateID.c_str() );
		return;
	}

	pLocal->OnUserInfo( this, rkPacket );
}

void ServerNode::OnPCRoom( SP2Packet &rkPacket )
{
	ioChannelingNodeParent *pNode = g_ChannelingMgr.GetNode( CNT_WEMADE ); // kyg 이부분 수정하면 될듯 
	if( !pNode )
	{
		LOG.PrintTimeAndLog( 0, "%s Channeling Type is wrong.%d", __FUNCTION__, CNT_WEMADE );
		return;
	}

	pNode->OnPCRoom( this, rkPacket );		
}


void ServerNode::OnOTP( SP2Packet &rkPacket )
{
	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( !pLocal )
	{
		ioHashString szBillingGUID;
		ioHashString szPrivateID;
		ioHashString szEncodePW;

		rkPacket >> szBillingGUID >> szPrivateID >> szEncodePW;
		LOG.PrintTimeAndLog(0, "%s pLocal == NULL:%s:%s:%s", __FUNCTION__, szBillingGUID.c_str(), szPrivateID.c_str(), szEncodePW.c_str() );
		return;
	}

	pLocal->OnOTP( this, rkPacket );
}

void ServerNode::OnGetMileage( SP2Packet &rkPacket )
{
	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( !pLocal )
	{
		DWORD        dwUserIndex       = 0;
		ioHashString szPrivateID;

		rkPacket >> dwUserIndex >> szPrivateID;
		LOG.PrintTimeAndLog(0, "%s pLocal == NULL:%d:%s", __FUNCTION__, dwUserIndex, szPrivateID.c_str() );
		return;
	}

	pLocal->OnGetMileage( this, rkPacket );
}


void ServerNode::OnAddMileage( SP2Packet &rkPacket )
{
	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( !pLocal )
	{
		DWORD        dwUserIndex = 0;
		ioHashString szPrivateID;
		ioHashString szPublicID;
		ioHashString szPublicIP;
		int          iPresentType = 0;
		int          iValue1      = 0;
		int          iVlaue2      = 0;
		int          iSellPeso    = 0;
		bool         bPresent     = false;


		rkPacket >> dwUserIndex >> szPrivateID >> szPublicID >> szPublicIP >> iPresentType >> iValue1 >> iVlaue2 >> iSellPeso >> bPresent;
		LOG.PrintTimeAndLog(0, "%s pLocal == NULL:%d:%s:%s:%s:%d:%d:%d:%d:%d", __FUNCTION__, dwUserIndex, szPrivateID.c_str(), szPublicID.c_str(), szPublicIP.c_str(), iPresentType, iValue1, iVlaue2, iSellPeso, (int)bPresent );
		return;
	}

	pLocal->OnAddMileage( this, rkPacket );
}


void ServerNode::OnIPBonus( SP2Packet &rkPacket )
{
	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( !pLocal )
	{
		ioHashString szPrivateID;
		ioHashString szPublicIP;
		rkPacket >> szPrivateID >> szPublicIP;

		LOG.PrintTimeAndLog(0, "%s pLocal == NULL:%s:%s", __FUNCTION__, szPrivateID.c_str(), szPublicIP.c_str() );
		return;
	}

	pLocal->OnIPBonus( this, rkPacket );
}

void ServerNode::OnCancelCash( SP2Packet &rkPacket )
{
	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( !pLocal )
	{
		int          iChannelingType = 0;
		ioHashString szBillingGUID;
		DWORD        dwUserIndex = 0;
		rkPacket >> iChannelingType;
		rkPacket >> szBillingGUID;
		rkPacket >> dwUserIndex;

		LOG.PrintTimeAndLog(0, "%s pLocal == NULL:%d:%s:%d", __FUNCTION__, iChannelingType, szBillingGUID.c_str(), dwUserIndex );
		return;
	}

	pLocal->OnCancelCash( this, rkPacket );
}

bool ServerNode::SendMessage( CPacket &rkPacket )
{
	ThreadSync ts(this);
	g_ProcessChecker.ServerSendMessage( rkPacket.GetBufferSize() );
	return CConnectNode::SendMessage( rkPacket, TRUE);
}

void ServerNode::OnIPBonusOut( SP2Packet &rkPacket )
{
	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( !pLocal )
	{
		ioHashString szPrivateID;
		ioHashString szPublicIP;
		rkPacket >> szPrivateID >> szPublicIP;

		LOG.PrintTimeAndLog(0, "%s pLocal == NULL:%s:%s", __FUNCTION__, szPrivateID.c_str(), szPublicIP.c_str() );
		return;
	}

	pLocal->OnIPBonusOut( this, rkPacket );
}

void ServerNode::OnAddCash( SP2Packet &rkPacket )
{
	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( !pLocal )
	{
		int          iChannelingType = 0;
		ioHashString szBillingGUID;
		DWORD        dwUserIndex = 0;
		rkPacket >> iChannelingType;
		rkPacket >> szBillingGUID;
		rkPacket >> dwUserIndex;

		LOG.PrintTimeAndLog(0, "%s pLocal == NULL:%d:%s:%d", __FUNCTION__, iChannelingType, szBillingGUID.c_str(), dwUserIndex );
		return;
	}

	pLocal->OnAddCash( this, rkPacket );
}

void ServerNode::OnFillCashUrl( SP2Packet &rkPacket )
{
	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( !pLocal )
	{
		int          iChannelingType = 0;
		ioHashString szBillingGUID;
		DWORD        dwUserIndex = 0;
		rkPacket >> iChannelingType;
		rkPacket >> szBillingGUID;
		rkPacket >> dwUserIndex;

		LOG.PrintTimeAndLog(0, "%s pLocal == NULL:%d:%s:%d", __FUNCTION__, iChannelingType, szBillingGUID.c_str(), dwUserIndex );
		return;
	}

	pLocal->OnFillCashUrl( this, rkPacket );
}

void ServerNode::SessionClose( BOOL safely/*=TRUE */ )
{
	CPacket packet(SSTPK_CLOSE);
	ReceivePacket( packet );
}

void ServerNode::OnClose( SP2Packet &rkPacket )
{
	g_UserInfoManager->DelUserInfoByServerIndex(m_serverIndex);
	if( !IsDisconnectState() )
	{
		OnDestroy();
	 
		g_ServerNodeManager.RemoveNode( this );
	}
}

void ServerNode::OnSubscriptionRetractCheck( SP2Packet& rkPacket )
{
	int         iChannelingType   = 0;
	rkPacket >> iChannelingType;

	ioChannelingNodeParent *pNode = g_ChannelingMgr.GetNode( (ChannelingType) iChannelingType );
	if( !pNode )
	{
		LOG.PrintTimeAndLog( 0, "%s Channeling Type is wrong.%d", __FUNCTION__, iChannelingType );
		return;
	}

	rkPacket.SetPosBegin(); // Pos to Begin
	pNode->OnSubscriptionRetractCheck( this, rkPacket );

}

void ServerNode::OnSubscriptionRetract( SP2Packet& rkPacket )
{
	int         iChannelingType   = 0;
	rkPacket >> iChannelingType;
	ioChannelingNodeParent *pNode = g_ChannelingMgr.GetNode( (ChannelingType) iChannelingType );
	if( !pNode )
	{
		LOG.PrintTimeAndLog( 0, "%s Channeling Type is wrong.%d", __FUNCTION__, iChannelingType );
		return;
	}

	rkPacket.SetPosBegin(); // Pos to Begin
	pNode->OnSubscriptionRetract( this, rkPacket );
}

void ServerNode::OnSessionControl( SP2Packet& rkPakcet )
{
	 int controlType = 0;
	 rkPakcet >> controlType;

	 OnSessionControlPacketParse(rkPakcet,controlType);
}

void ServerNode::OnSessionControlPacketParse( SP2Packet& rkPacket, int controlType )
{
	switch(controlType)
	{
	case NexonSessionLogin:
		{
			OnSessionLogin(rkPacket);
		}
		break;
	case NexonSessionLogout:
		{
			OnSessionLogout(rkPacket);
		}
		break;
	case NexonReconnect:
		{
			OnSessionReConnect(rkPacket);
		}
		break;
	}
}

void ServerNode::OnSessionLogin( SP2Packet& rkPacket )
{
	if( ioLocalManager::GetLocalType() != ioLocalManager::LCT_KOREA )
		return;

	int			 chType = 0;
	DWORD		 userIndex;
	ioHashString chanID;
	ioHashString publicIP;
	ioHashString privateIP;
	ioHashString privateID;
	DWORD		 serverIndex;

	rkPacket >> chType;
	rkPacket >> userIndex;
	rkPacket >> chanID;
	rkPacket >> publicIP;
	rkPacket >> privateIP;
	rkPacket >> privateID;
	rkPacket >> serverIndex;

	LOG.PrintTimeAndLog(0,"OnSessionLogin (%d:%d:%s:%s:%s:%s:%d)",chType,userIndex,chanID.c_str(),publicIP.c_str(),privateIP.c_str(),privateID.c_str(),serverIndex);
	
	if( ioLocalManager::GetLocalType() == ioLocalManager::LCT_KOREA )
	{
		g_UserInfoManager->AddUserInfo(chType,privateID,chanID,userIndex,serverIndex,GetIP(),GetClientPort(),publicIP,privateIP);
		g_NexonSessionServer.SendLoginPacket(chanID.c_str(),privateID.c_str(),publicIP.c_str(),privateIP.c_str(),chType);
	}

	if(g_App.IsTestMode())
	{
		NexonUserInfo* userInfo = g_UserInfoManager->GetUserInfoByUserIndex(userIndex);
		OnNexonLogin login;
		login.UserId = privateIP;
		
		if( ioLocalManager::GetLocalType() == ioLocalManager::LCT_KOREA )
			g_NexonSessionServer.SendPCRoomPacket(userInfo,login,11);
	}
}

void ServerNode::OnSessionLogout( SP2Packet& rkPacket )
{
	if( ioLocalManager::GetLocalType() != ioLocalManager::LCT_KOREA )
		return;

	int chType = 0;
	DWORD userIndex;
	ioHashString chanID;
	ioHashString privateID;
	
	rkPacket >> chType;
	rkPacket >> userIndex;
	rkPacket >> chanID;
	rkPacket >> privateID;

	LOG.PrintTimeAndLog(0,"OnSessionLogout (%d:%d:%s:%s)",chType,userIndex,chanID.c_str(),privateID.c_str());
	if( ioLocalManager::GetLocalType() == ioLocalManager::LCT_KOREA )
		g_NexonSessionServer.SendLogoutPacket(userIndex);

	g_UserInfoManager->DelUserInfoByUserIndex(userIndex);
}

void ServerNode::OnSessionReConnect( SP2Packet& rkPacket )
{
	if( ioLocalManager::GetLocalType() != ioLocalManager::LCT_KOREA )
		return;

	int			 ChanType= 0;
	DWORD		 userIndex;
	ioHashString publicIP;
	ioHashString privateIP;
	ioHashString privateID;
	ioHashString chanID;
	DWORD		 serverIndex;
	 
	int size;
	rkPacket >> size;

	for(int i=0; i< size; ++i)
	{
		rkPacket >> ChanType;
		rkPacket >> userIndex;
		rkPacket >> chanID;
		rkPacket >> publicIP;
		rkPacket >> privateIP;
		rkPacket >> privateID;
		rkPacket >> serverIndex;

		g_UserInfoManager->AddUserInfo(ChanType,privateID,chanID,userIndex,serverIndex,GetIP(),GetClientPort(),publicIP,privateIP);
		LOG.PrintTimeAndLog(0,"SendReConnet :%s :%s ",chanID.c_str(),privateID.c_str());
		g_NexonSessionServer.SendLoginPacket(chanID.c_str(),privateID.c_str(),publicIP.c_str(),privateIP.c_str(),ChanType);
	}

	LOG.PrintTimeAndLog(0,"%s Size :%d",__FUNCTION__,size);

	//g_NexonSessionServer.SendReLoginPacket(true);

}

void ServerNode::SendRequestLoginInfo()
{
	SP2Packet pk(BSTPK_REQUEST_USERINFO);
	SendMessage(pk);
}

void ServerNode::OnSessionFirstLogin(SP2Packet& rkPacket)
{
	//첫 로그인 유저이니까 그전에 로그인했었던 정보가 있었는지 확인한다. 
	//게임서버에서 담아서줄 정보는 유저 szKey 값
	ioHashString szKey;
	rkPacket >> szKey;
	BillInfoManager::BillingInfo* pInfo = NULL;
	pInfo = g_BillInfoManager->Get( szKey );

	//로그, READ WRITE 로 변경 패킷, 초기화
	if( pInfo != NULL )
	{
		DWORD        dwUserIndex   = 0;
		ioHashString szBillingGUID;
		int          iType         = 0;
		int          iPayAmt       = 0;
		int          iChannelingType = 0;
		ioHashString szServerIP;
		ioHashString szUserIP = "";
		int          iClientPort   = 0;
		ioHashString szChargeNo = "";
		ioHashString szPrivateID = "";

		if( pInfo->m_eType == BillInfoManager::AT_GET )
		{
			pInfo->m_kPacket >> dwUserIndex >> szBillingGUID;

			LOG.PrintTimeAndLog(0,"[warning][timeout]ServerNode::OnSessionFirstLogin Duplicate UserInfo:AT_GET:%d:%d:%d:%s:%s",
								pInfo->m_eType, 
								pInfo->m_iChannelingType,
								dwUserIndex,
								pInfo->m_szKey.c_str(),
								szBillingGUID.c_str()
								);
		}
		else if( pInfo->m_eType == BillInfoManager::AT_OUTPUT )
		{
			if( pInfo->m_iChannelingType == CNT_MGAME )
			{
				pInfo->m_kPacket >> dwUserIndex >> szBillingGUID >> iType >> iPayAmt >> iChannelingType >> szServerIP >> iClientPort;
			}
			else if ( pInfo->m_iChannelingType == CNT_WEMADEBUY )
			{
				pInfo->m_kPacket >> dwUserIndex >> szBillingGUID >> iType >> iPayAmt >> iChannelingType >> szUserIP >> szServerIP >> iClientPort;
			}
			else if ( pInfo->m_iChannelingType == CNT_TOONILAND )
			{
				pInfo->m_kPacket >> dwUserIndex >> szBillingGUID >> iType >> iPayAmt >> iChannelingType >> szPrivateID >> szServerIP >> iClientPort;
			}
				

			LOG.PrintTimeAndLog( 0, "[warning][timeout]ServerNode::OnSessionFirstLogin Duplicate UserInfo:AT_OUTPUT:%d:%s:%s:%d:%d:ServerIP:%s:%d", 
									pInfo->m_iChannelingType, 
									pInfo->m_szKey.c_str(),
									szBillingGUID.c_str(),
									iType, 
									iPayAmt,
									szServerIP.c_str(),
									iClientPort);
		}
		g_BillInfoManager->Delete( szKey );
	}
}

void ServerNode::OnLogout(SP2Packet &rkPacket )
{
	//hr 로그아웃 할경우 ioLocalLatin 호출
	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( !pLocal )
	{
		LOG.PrintTimeAndLog(0, "%s pLocal == NULL:%s:%s:%s", "ServerNode::OnLogout" );
		return;
	}
		
	pLocal->OnLoginData( this, rkPacket );
}

//hr 라틴추가
void ServerNode::OnSessionLogoutLog(SP2Packet& rkPacket)
{
	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( !pLocal )
	{
		LOG.PrintTimeAndLog(0, "%s pLocal == NULL:%s:%s:%s", "ServerNode::OnGetCCUCount" );
		return;
	}
	rkPacket.SetPosBegin(); // Pos to Begin.
	pLocal->OnLogoutLog( this, rkPacket );
}

void ServerNode::OnGetCCUCount(SP2Packet& rkPacket)
{
	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( !pLocal )
	{
		LOG.PrintTimeAndLog(0, "%s pLocal == NULL:%s:%s:%s", "ServerNode::OnGetCCUCount" );
		return;
	}
		
	pLocal->OnCCUCount( this, rkPacket );
}

void ServerNode::OnDaumShutDownCheck( SP2Packet& rkPacket )
{
	ioChannelingNodeParent *pNode = g_ChannelingMgr.GetNode( CNT_DAUM );

	if( !pNode )
	{
		LOG.PrintTimeAndLog(0, "shut down check daum node == NULL" );
		return;
	}

	ioChannelingNodeDaum_v2 *pDaumNode = (ioChannelingNodeDaum_v2*)pNode;

	ioHashString szUserGUID;
	ioHashString szDaumUserID;

	//rkPacket >> szUserGUID;

	PACKET_GUARD_VOID( rkPacket.Read(szUserGUID) );
	PACKET_GUARD_VOID( rkPacket.Read(szDaumUserID) );

	BOOL bAdult = pDaumNode->CheckShutDownCheck(szDaumUserID);

	SP2Packet kPacket(BSTPK_DAUM_SHUTDOWN_CHECK);
	kPacket<<szUserGUID << bAdult;
	SendMessage(kPacket);
}

void ServerNode::OnRequestTimeCash( SP2Packet& rkPacket )
{
	DWORD dwUserIndex	= 0;
	int iEventID		= 0;
	int iCash			= 0;
	int iExpire			= 0;

	ioChannelingNodeParent *pNode = g_ChannelingMgr.GetNode( CNT_WEMADEBUY );
	if( !pNode )
	{
		LOG.PrintTimeAndLog(0, "OnRequestTimeCash node == NULL" );
		return;
	}

	ioChannelingNodeWemadeCashLink* pWemade	= static_cast<ioChannelingNodeWemadeCashLink*>(pNode);
	
	pWemade->_OnPresentCash(this, rkPacket);
}