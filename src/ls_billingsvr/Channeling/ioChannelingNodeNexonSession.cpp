#include "../stdafx.h"
#include "../NodeInfo/ServerNode.h"
#include "../NodeInfo/ServerNodeManager.h"
#include "../WemadeBillingServer/WemadeBillingServer.h"
#include "../database/logdbclient.h"
#include "./ioChannelingNodeNexonSession.h"
#include "../MainProcess.h"

extern CLog LOG;


ioChannelingNodeNexonSession::ioChannelingNodeNexonSession(void)
{
}

ioChannelingNodeNexonSession::~ioChannelingNodeNexonSession(void)
{
}

ChannelingType ioChannelingNodeNexonSession::GetType()
{
	return CNT_NEXONSESSION;
}

void ioChannelingNodeNexonSession::_OnPCRoom( ServerNode *pServerNode, SP2Packet &rkPacket )
{
	int          iChannelingType   = 0;
	ioHashString szBillingGUID;
	DWORD        dwUserIndex       = 0;
	ioHashString szPrivateID;
	ioHashString szPublicID;
	ioHashString szPublicIP;
	int          iPort             = 0;

	rkPacket >> iChannelingType >> szBillingGUID >> dwUserIndex >> szPrivateID >> szPublicID >> szPublicIP >> iPort;  // 공통 사항

	if( g_App.IsReserveLogOut() )
	{
		LOG.PrintTimeAndLog( 0, "%s LogOut: %d:%s:%s:%s",__FUNCTION__, dwUserIndex, szBillingGUID.c_str(), szPrivateID.c_str(), szPublicID.c_str() );
		SP2Packet kPacket( BSTPK_PCROOM_RESULT );
		kPacket << dwUserIndex;
		kPacket << szBillingGUID;
		kPacket << CASH_RESULT_EXCEPT;
		pServerNode->SendMessage( kPacket );
		return;
	}

	BillInfoMgr::BillingInfo *pInfo = new BillInfoMgr::BillingInfo;
	if( !pInfo )
	{
		LOG.PrintTimeAndLog( 0, "%s pInfo == NULL: %d:%s:%s:%s:%d",__FUNCTION__, dwUserIndex, szBillingGUID.c_str(), szPrivateID.c_str(), szPublicID.c_str(), GetLastError()  );
		SP2Packet kPacket( BSTPK_PCROOM_RESULT );
		kPacket << dwUserIndex;
		kPacket << szBillingGUID;
		kPacket << CASH_RESULT_EXCEPT;
		pServerNode->SendMessage( kPacket );
		g_LogDBClient.OnInsertBillingServerError( CNT_WEMADE, BILLING_ERROR_LOG_FAIL_ADD_BUYINFO,"fail add buy info." );
		return;
	}

	pInfo->m_szKey          = szPrivateID;
	pInfo->m_dwCreateTime   = TIMEGETTIME();
	pInfo->m_kPacket << dwUserIndex << szBillingGUID << pServerNode->GetIP() << pServerNode->GetClientPort();
	pInfo->m_kPacket.SetPosBegin();
	if( !m_BillInfoMgr.Add( pInfo ) )
	{
		LOG.PrintTimeAndLog( 0, "%s BillingInfo is full.: %d:%s:%s:%s:%d",__FUNCTION__, dwUserIndex, szBillingGUID.c_str(), szPrivateID.c_str(), szPublicID.c_str(), GetLastError()  );
		SP2Packet kPacket( BSTPK_PCROOM_RESULT );
		kPacket << dwUserIndex;
		kPacket << szBillingGUID;
		kPacket << CASH_RESULT_EXCEPT;
		pServerNode->SendMessage( kPacket );
		g_LogDBClient.OnInsertBillingServerError( CNT_WEMADE, BILLING_ERROR_LOG_FAIL_ADD_BUYINFO,"fail add buy info." );
		return;
	}

	if( g_App.IsTestMode() )
	{
		SP2Packet kPacket( BSTPK_PCROOM_RESULT );
		kPacket << dwUserIndex;
		kPacket << szBillingGUID;
		kPacket << CASH_RESULT_SUCCESS;
		kPacket << 1;
		pServerNode->SendMessage( kPacket );
		return;
	}

	BOQPTS_CHECKPREMIUM2 kInfo;
	kInfo.SetInfo( szPrivateID.c_str(), szPublicIP.c_str(), iPort  );
	kInfo.Htonl();

	SP2Packet kPacket( BWTPK_CHECKPREMIUM2_REQUEST );
	kPacket << kInfo;

	if( !g_WemadeBillingServer.SendMessage( kPacket ) )
	{
		LOG.PrintTimeAndLog( 0, "%s Fail Send: %d:%s:%s:%s:%d",__FUNCTION__, dwUserIndex, szBillingGUID.c_str(), szPrivateID.c_str(), szPublicID.c_str(), GetLastError()  );
		SP2Packet kPacket( BSTPK_PCROOM_RESULT );
		kPacket << dwUserIndex;
		kPacket << szBillingGUID;
		kPacket << CASH_RESULT_EXCEPT;
		pServerNode->SendMessage( kPacket );

		m_BillInfoMgr.Delete( szPrivateID );
		g_LogDBClient.OnInsertBillingServerError( CNT_WEMADE, BILLING_ERROR_LOG_FAIL_SEND_WEMADE_BILLINGSERVER,"fail send Wemade billingserver." );
		return;
	}

	//LOG.PrintTimeAndLog( 0, "%s Send Success: %d:%s:%s:%s:%s",__FUNCTION__, dwUserIndex, szBillingGUID.c_str(), szPrivateID.c_str(), szPublicID.c_str(), szPublicIP.c_str() );
}

void ioChannelingNodeNexonSession::OnRecievePCRoom( const BOQPTS_CHECKPREMIUM2& rkResult )
{
	BillInfoMgr::BillingInfo *pInfo = m_BillInfoMgr.Get( rkResult.m_szUserID );
	if( !pInfo )
	{
		LOG.PrintTimeAndLog( 0, "%s pInfo == NULL. %d:%s:%d", __FUNCTION__, rkResult.m_wRetCode, rkResult.m_szUserID, rkResult.m_dwPCBangNo );
		return;
	}

	DWORD        dwUserIndex   = 0;
	ioHashString szBillingGUID;
	ioHashString szServerIP;
	int          iClientPort   = 0;
	pInfo->m_kPacket >> dwUserIndex >> szBillingGUID >> szServerIP >> iClientPort;

	SP2Packet kPacket( BSTPK_PCROOM_RESULT );
	kPacket << dwUserIndex;
	kPacket << szBillingGUID;
	kPacket << CASH_RESULT_SUCCESS;
	kPacket << rkResult.m_dwPCBangNo;
	if( !g_ServerNodeManager.SendMessageIP( szServerIP, iClientPort, kPacket ) )
	{
		LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:ID %s:Ret %d:%d", __FUNCTION__, dwUserIndex, szBillingGUID.c_str(), rkResult.m_szUserID,  rkResult.m_wRetCode, rkResult.m_dwPCBangNo );
		g_LogDBClient.OnInsertBillingServerError( CNT_WEMADE, BILLING_ERROR_LOG_DISCONNECT_GAMESERVER,"disconnect game server." );
		m_BillInfoMgr.Delete( rkResult.m_szUserID );
		return;
	}

	if( rkResult.m_wRetCode == 0 )
	{
		//LOG.PrintTimeAndLog( 0, "%s Success: %d:%s:ID %s:Ret %d:%d", __FUNCTION__, dwUserIndex, szBillingGUID.c_str(), rkResult.m_szUserID, rkResult.m_wRetCode, rkResult.m_dwPCBangNo );
	}
	else
	{
		LOG.PrintTimeAndLog( 0, "%s Fail: %d:%s:ID %s:Ret %d:%d", __FUNCTION__, dwUserIndex, szBillingGUID.c_str(), rkResult.m_szUserID,  rkResult.m_wRetCode, rkResult.m_dwPCBangNo );
		g_LogDBClient.OnInsertBillingServerError( CNT_WEMADE, rkResult.m_wRetCode, "Error return value." );
	}

	m_BillInfoMgr.Delete( rkResult.m_szUserID );
}
