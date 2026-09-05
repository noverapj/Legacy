#include "../stdafx.h"
#include "../NodeInfo/ServerNode.h"
#include "../NodeInfo/ServerNodeManager.h"
#include "../MgameBillingServer/MgameBillingServer.h"
#include "../database/logdbclient.h"
#include "./iochannelingnodemgame.h"
#include "../MainProcess.h"

extern CLog LOG;

ioChannelingNodeMgame::ioChannelingNodeMgame(void)
{
}

ioChannelingNodeMgame::~ioChannelingNodeMgame(void)
{
}

ChannelingType ioChannelingNodeMgame::GetType()
{
	return CNT_MGAME;
}

void ioChannelingNodeMgame::_OnGetCash( ServerNode *pServerNode, SP2Packet &rkPacket )
{
	int          iChannelingType   = 0;
	ioHashString szBillingGUID;
	DWORD        dwUserIndex       = 0;
	ioHashString szPrivateID;
	ioHashString szPublicID;
	ioHashString szMgameID;
	ioHashString szPublicIP;
	bool         bSetUserMouse     = false;

	rkPacket >> iChannelingType >> szBillingGUID >> dwUserIndex >> szPrivateID >> szPublicID >> bSetUserMouse // 공통 사항
		     >> szMgameID >> szPublicIP; // mgame용

	if( g_App.IsReserveLogOut() )
	{
		LOG.PrintTimeAndLog( 0, "%s LogOut: %d:%s:%s:%s",__FUNCTION__, dwUserIndex, szBillingGUID.c_str(), szPrivateID.c_str(), szPublicID.c_str() );
		SP2Packet kPacket( BSTPK_GET_CASH_RESULT );
		kPacket << dwUserIndex;
		kPacket << szBillingGUID;
		kPacket << bSetUserMouse;
		kPacket << CASH_RESULT_EXCEPT;
		pServerNode->SendMessage( kPacket );
		return;
	}
	BillInfoManager::BillingInfo *pInfo = g_BillInfoManager->PopBillInfo();
	if( !pInfo )
	{
		LOG.PrintTimeAndLog( 0, "%s pInfo == NULL: %d:%s:%s:%s:%d",__FUNCTION__, dwUserIndex, szBillingGUID.c_str(), szPrivateID.c_str(), szPublicID.c_str(), GetLastError()  );
		SP2Packet kPacket( BSTPK_GET_CASH_RESULT );
		kPacket << dwUserIndex;
		kPacket << szBillingGUID;
		kPacket << bSetUserMouse;
		kPacket << CASH_RESULT_EXCEPT;
		pServerNode->SendMessage( kPacket );
		g_LogDBClient.OnInsertBillingServerError( CNT_MGAME, BILLING_ERROR_LOG_FAIL_ADD_BUYINFO,"fail add buy info." );
		return;
	}

	pInfo->m_szKey				= szMgameID;
	pInfo->m_eType				= BillInfoManager::AT_GET;
	pInfo->m_iChannelingType		= CNT_MGAME;	//고스트 로그용
	pInfo->m_dwCreateTime = TIMEGETTIME();
	pInfo->m_kPacket << dwUserIndex << szBillingGUID << bSetUserMouse << pServerNode->GetIP() << pServerNode->GetClientPort();
	pInfo->m_kPacket.SetPosBegin();
	if( !g_BillInfoManager->Add( pInfo ) )
	{
		LOG.PrintTimeAndLog( 0, "%s BillingInfo is full.: %d:%s:%s:%s:%d",__FUNCTION__, dwUserIndex, szBillingGUID.c_str(), szPrivateID.c_str(), szPublicID.c_str(), GetLastError()  );
		SP2Packet kPacket( BSTPK_GET_CASH_RESULT );
		kPacket << dwUserIndex;
		kPacket << szBillingGUID;
		kPacket << bSetUserMouse;
		kPacket << CASH_RESULT_EXCEPT;
		pServerNode->SendMessage( kPacket );
		g_LogDBClient.OnInsertBillingServerError( CNT_MGAME, BILLING_ERROR_LOG_FAIL_ADD_BUYINFO,"fail add buy info." );
		return;
	}
//#ifdef _TEST
//	
//	SP2Packet testkPacket( BSTPK_GET_CASH_RESULT );
//	testkPacket<< dwUserIndex;
//	testkPacket<< szBillingGUID;
//	testkPacket<< bSetUserMouse;
//	testkPacket<< CASH_RESULT_SUCCESS;
//	testkPacket<< 500;
//	testkPacket<< 500;
//
//	pServerNode->SendMessage ( testkPacket );
//	g_BillInfoManager->Delete( szMgameID );
//	//g_BillInfoManager->Get( szMgameID );
//	return;
//#endif
	MgameCashRequest kInfo;
	kInfo.SetInfo( CASH_ACTION_REQUEST_GET_CASH, szMgameID.c_str(), 0, "0", 0, szPublicIP.c_str(), szPublicID.c_str(), false );

	SP2Packet kPacket( BMTPK_CASH_ACTION_REQUEST );
	kPacket << kInfo;
	if( !g_MgameBillingServer.SendMessage( kPacket ) )
	{
		LOG.PrintTimeAndLog( 0, "%s Fail Send: %d:%s:%s:%s:%d",__FUNCTION__, dwUserIndex, szBillingGUID.c_str(), szPrivateID.c_str(), szPublicID.c_str(), GetLastError()  );
		SP2Packet kPacket( BSTPK_GET_CASH_RESULT );
		kPacket << dwUserIndex;
		kPacket << szBillingGUID;
		kPacket << bSetUserMouse;
		kPacket << CASH_RESULT_EXCEPT;
		pServerNode->SendMessage( kPacket );

		g_BillInfoManager->Delete( szMgameID );
		g_LogDBClient.OnInsertBillingServerError( CNT_MGAME, BILLING_ERROR_LOG_FAIL_SEND_MGAME_BILLINGSERVER,"fail send mgame billingserver." );
		return;
	}
 
	//LOG.PrintTimeAndLog( 0, "%s Send Success: %d:%s:%s:%s:MgameID %s:%s",__FUNCTION__, dwUserIndex, szBillingGUID.c_str(), szPrivateID.c_str(), szPublicID.c_str(), szMgameID.c_str(), szPublicIP.c_str() );
}

void ioChannelingNodeMgame::_OnOutputCash( ServerNode *pServerNode, SP2Packet &rkPacket, DWORD dwGoodsNo, const ioHashString &rszGoodsName )
{
	int          iChannelingType = 0;
	ioHashString szBillingGUID;
	int          iReturnItemPrice = 0;
	DWORD        dwUserIndex = 0;
	ioHashString szPublicID;
	ioHashString szPrivateID;
	ioHashString szUserIP;
	int          iPayAmt  = 0;
	int          iType    = 0;
	int			 iBonusCashSize	= 0;

	rkPacket >> iChannelingType >> szBillingGUID >> iReturnItemPrice >> dwUserIndex >> szPublicID >> szPrivateID >> szUserIP >> iPayAmt >> iBonusCashSize;
	
	BillInfoManager::BillingInfo *pInfo = g_BillInfoManager->PopBillInfo();
	if( !pInfo )
	{
		LOG.PrintTimeAndLog( 0, "%s pInfo == NULL: %d:%s:%s:%s:%d",__FUNCTION__, dwUserIndex, szBillingGUID.c_str(), szPrivateID.c_str(), szPublicID.c_str(), GetLastError()  );
		SP2Packet kPacket( BSTPK_OUTPUT_CASH_RESULT  );
		kPacket << dwUserIndex;
		kPacket << szBillingGUID;
		kPacket << CASH_RESULT_EXCEPT;
		kPacket << iReturnItemPrice;
		kPacket << iType;
		pServerNode->SendMessage( kPacket );
		g_LogDBClient.OnInsertBillingServerError( CNT_MGAME, BILLING_ERROR_LOG_FAIL_ADD_BUYINFO,"fail add buy info." );
		return;
	}

	for( int i = 0; i < iBonusCashSize; i++ )
	{
		int iIndex		= 0;
		int iBonusCash	= 0;

		rkPacket >> iIndex >> iBonusCash;
		
		pInfo->AddBonusCashInfoForConsume(iIndex, iBonusCash);
	}
	rkPacket >> iType; // 공통사항

	if( g_App.IsReserveLogOut() )
	{
		LOG.PrintTimeAndLog( 0, "%s LogOut: %d:%s:%s:%s",__FUNCTION__, dwUserIndex, szBillingGUID.c_str(), szPrivateID.c_str(), szPublicID.c_str() );
		SP2Packet kPacket( BSTPK_OUTPUT_CASH_RESULT  );
		kPacket << dwUserIndex;
		kPacket << szBillingGUID;
		kPacket << CASH_RESULT_EXCEPT;
		kPacket << iReturnItemPrice;
		kPacket << iType;
		pServerNode->SendMessage( kPacket );
		return;
	}

	ItemInfo kBuyInfo;
	GetItemInfo( rkPacket, iType, kBuyInfo );

	ioHashString szMgameID;
	rkPacket >> szMgameID; // Mgame용

	bool bPresent = false;
	if( iType == OUTPUT_CASH_PRESENT || 
		iType == OUTPUT_CASH_POPUP )
		bPresent  = true;

	pInfo->m_szKey				= szMgameID;
	pInfo->m_eType				= BillInfoManager::AT_OUTPUT;
	pInfo->m_dwCreateTime		= TIMEGETTIME();
	pInfo->m_iChannelingType	= CNT_MGAME;
	pInfo->m_kPacket << dwUserIndex << szBillingGUID << iReturnItemPrice << iType << iPayAmt << iChannelingType;
	pInfo->m_kPacket << pServerNode->GetIP() << pServerNode->GetClientPort();
	SetItemInfo( pInfo->m_kPacket, iType, kBuyInfo );
	pInfo->m_kPacket.SetPosBegin();

	if( !g_BillInfoManager->Add( pInfo ))
	{
		LOG.PrintTimeAndLog( 0, "%s BillingInfo is full.: %d:%s:%s:%s:%d",__FUNCTION__, dwUserIndex, szBillingGUID.c_str(), szPrivateID.c_str(), szPublicID.c_str(), GetLastError()  );
		SP2Packet kPacket( BSTPK_OUTPUT_CASH_RESULT );
		kPacket << dwUserIndex;
		kPacket << szBillingGUID;
		kPacket << CASH_RESULT_EXCEPT;
		kPacket << iReturnItemPrice;
		kPacket << iType;
		pServerNode->SendMessage( kPacket );
		g_LogDBClient.OnInsertBillingServerError( CNT_MGAME, BILLING_ERROR_LOG_FAIL_ADD_BUYINFO,"fail add buy info." );
		return;
	}
//#ifdef _TEST
//	ioHashString szServerIP;
//	int          iClientPort   = 0;
//	
//	BillInfoManager::BillingInfo *pInfo1 = g_BillInfoManager->Get( szMgameID );
//
//	if( pInfo1->m_eType == BillInfoManager::AT_OUTPUT)
//		pInfo1->m_kPacket >> dwUserIndex >> szBillingGUID >> iReturnItemPrice >> iType >> iPayAmt >> iChannelingType >> szServerIP >> iClientPort;
//	else
//		BillInfoManager::BillingInfo *pInfo1 = g_BillInfoManager->Get( szMgameID );
//
//	int iReturn = CASH_RESULT_SUCCESS;
//	SP2Packet testkPacket( BSTPK_OUTPUT_CASH_RESULT );
//	testkPacket<< dwUserIndex;
//	testkPacket<< szBillingGUID;
//	testkPacket<< iReturn;
//	testkPacket<< iReturnItemPrice;
//	testkPacket<< iType;
//
//	// return value
//	if( iReturn == CASH_RESULT_SUCCESS )
//	{
//		ioHashString szChargeNo = "";
//		testkPacket<< iPayAmt;
//		testkPacket<< 0; // TransactionID ( FOR US )
//		testkPacket<< szChargeNo;
//
//		ItemInfo kItemInfo;
//		GetItemInfo( pInfo1->m_kPacket, iType, kItemInfo );
//		SetItemInfo( testkPacket, iType, kItemInfo );
//
//		testkPacket<< iChannelingType;  // 공통
//		testkPacket<< 500;
//		testkPacket<< 500;
//
//		static TwoOfINTVec vInfo;
//		vInfo.clear();
//
//		pInfo1->GetBonusCashInfo(vInfo);
//		int iiBonusCashSize	= vInfo.size();
//
//		testkPacket<< iiBonusCashSize	;
//
//		for( int i = 0; i < iiBonusCashSize	; i++ )
//		{
//			testkPacket<< vInfo[i].value1 << vInfo[i].value2;
//		}
//	}
//
//	if( !g_ServerNodeManager.SendMessageIP( szServerIP, iClientPort, testkPacket ) )
//	{
//		return;
//	}
//	g_BillInfoManager->Delete( szMgameID );
//	return;
//#endif
	MgameCashRequest kInfo;
	kInfo.SetInfo( CASH_ACTION_REQUEST_BUY, szMgameID.c_str(), dwGoodsNo, rszGoodsName.c_str(), iPayAmt, szUserIP.c_str(), szPublicID.c_str(), bPresent );

	SP2Packet kPacket( BMTPK_CASH_ACTION_REQUEST );
	kPacket << kInfo;
	if( !g_MgameBillingServer.SendMessage( kPacket ) )
	{
		LOG.PrintTimeAndLog( 0, "%s Fail Send: %d:%s:%s:%s:%d",__FUNCTION__, dwUserIndex, szBillingGUID.c_str(), szPrivateID.c_str(), szPublicID.c_str(), GetLastError()  );
		SP2Packet kPacket( BSTPK_OUTPUT_CASH_RESULT );
		kPacket << dwUserIndex;
		kPacket << szBillingGUID;
		kPacket << CASH_RESULT_EXCEPT;
		kPacket << iReturnItemPrice;
		kPacket << iType;
		pServerNode->SendMessage( kPacket );
		g_BillInfoManager->Delete( szMgameID );
		g_LogDBClient.OnInsertBillingServerError( CNT_MGAME, BILLING_ERROR_LOG_FAIL_SEND_MGAME_BILLINGSERVER,"fail send mgame billingserver." );
		return;
	}
	 
	LOG.PrintTimeAndLog( 0, "%s Send Success: %d:%s:%s:%s:MgameID %s:%s:Price %d:%d",__FUNCTION__, dwUserIndex, szBillingGUID.c_str(), szPrivateID.c_str(), szPublicID.c_str(), szMgameID.c_str(), szUserIP.c_str(), iPayAmt, dwGoodsNo );
}

void ioChannelingNodeMgame::OnRecieveCashAction( const MgameCashResult& rkResult )
{
	BillInfoManager::BillingInfo *pInfo = g_BillInfoManager->Get( rkResult.m_szUserID );

	if( !pInfo )
	{
		LOG.PrintTimeAndLog( 0, "%s pInfo == NULL. %d:%s:%d:%d", __FUNCTION__, rkResult.m_iRet, rkResult.m_szUserID, rkResult.m_iMCash, rkResult.m_iPCash );
		return;
	}

	if( pInfo->m_eType == BillInfoManager::AT_GET )
	{
		OnRecieveGetCash( rkResult, pInfo );
	}
	else if( pInfo->m_eType == BillInfoManager::AT_OUTPUT )
	{
		OnRecieveOutputCash( rkResult, pInfo );
	}
	else
	{
		LOG.PrintTimeAndLog( 0, "%s Type is wrong. %d:%s:%d:%d:%d", __FUNCTION__, rkResult.m_iRet, rkResult.m_szUserID, rkResult.m_iMCash, rkResult.m_iPCash, (int) pInfo->m_eType );
	}

	g_BillInfoManager->Delete( rkResult.m_szUserID );
}

void ioChannelingNodeMgame::OnRecieveGetCash( const MgameCashResult& rkResult, BillInfoManager::BillingInfo *pInfo )
{
	if( !pInfo )
	{
		LOG.PrintTimeAndLog( 0, "%s pInfo == NULL. %d:%s:%d:%d", __FUNCTION__, rkResult.m_iRet, rkResult.m_szUserID, rkResult.m_iMCash, rkResult.m_iPCash );
		return;
	}

	DWORD        dwUserIndex   = 0;
	ioHashString szBillingGUID;
	bool         bSetUserMouse = false;
	ioHashString szServerIP;
	int          iClientPort   = 0;
	pInfo->m_kPacket >> dwUserIndex >> szBillingGUID >> bSetUserMouse >> szServerIP >> iClientPort;

	int iReturn = CASH_RESULT_SUCCESS;
	if( rkResult.m_iRet != CASH_ACTION_RESULT_SUCCESS )
		iReturn = CASH_RESULT_EXCEPT;

	SP2Packet kPacket( BSTPK_GET_CASH_RESULT );
	kPacket << dwUserIndex;
	kPacket << szBillingGUID;
	kPacket << bSetUserMouse;
	kPacket << iReturn;
	kPacket << rkResult.m_iMCash;
	kPacket << rkResult.m_iPCash;

	if( !g_ServerNodeManager.SendMessageIP( szServerIP, iClientPort, kPacket ) )
	{
		LOG.PrintTimeAndLog( 0, "[warning][mgame]%s Send Fail: %d:%s:MgameID %s:Ret %d:%d:%d", __FUNCTION__, dwUserIndex, szBillingGUID.c_str(), rkResult.m_szUserID,  rkResult.m_iRet, rkResult.m_iMCash, rkResult.m_iPCash );
		g_LogDBClient.OnInsertBillingServerError( CNT_MGAME, BILLING_ERROR_LOG_DISCONNECT_GAMESERVER,"disconnect game server." );
		return;
	}

	if( iReturn == CASH_RESULT_SUCCESS )
	{
		LOG.PrintTimeAndLog( 0, "%s Success: %d:%s:MgameID %s:Ret %d:%d:%d", __FUNCTION__, dwUserIndex, szBillingGUID.c_str(), rkResult.m_szUserID, rkResult.m_iRet, rkResult.m_iMCash, rkResult.m_iPCash );
	}
	else
	{
		LOG.PrintTimeAndLog( 0, "%s Fail: %d:%s:MgameID %s:Ret %d:%d:%d", __FUNCTION__, dwUserIndex, szBillingGUID.c_str(), rkResult.m_szUserID,  rkResult.m_iRet, rkResult.m_iMCash, rkResult.m_iPCash );
		g_LogDBClient.OnInsertBillingServerError( CNT_MGAME, rkResult.m_iRet, "Error return value." );
	}
}

void ioChannelingNodeMgame::OnRecieveOutputCash( const MgameCashResult& rkResult, BillInfoManager::BillingInfo *pInfo )
{
	if( !pInfo )
	{
		LOG.PrintTimeAndLog( 0, "%s pInfo == NULL. %d:%s:%d:%d", __FUNCTION__, rkResult.m_iRet, rkResult.m_szUserID, rkResult.m_iMCash, rkResult.m_iPCash );
		return;
	}

	DWORD        dwUserIndex   = 0;
	ioHashString szBillingGUID;
	int          iReturnItemPrice = 0;
	int          iType         = 0;
	int          iPayAmt       = 0;
	int          iChannelingType = 0;
	ioHashString szServerIP;
	int          iClientPort   = 0;
	ioHashString szChargeNo = "";
	pInfo->m_kPacket >> dwUserIndex >> szBillingGUID >> iReturnItemPrice >> iType >> iPayAmt >> iChannelingType >> szServerIP >> iClientPort;

	int iReturn = CASH_RESULT_SUCCESS;
	if( rkResult.m_iRet != CASH_ACTION_RESULT_SUCCESS )
		iReturn = CASH_RESULT_EXCEPT;

	SP2Packet kPacket( BSTPK_OUTPUT_CASH_RESULT );
	kPacket << dwUserIndex;
	kPacket << szBillingGUID;
	kPacket << iReturn;
	kPacket << iReturnItemPrice;
	kPacket << iType;

	// return value
	if( iReturn == CASH_RESULT_SUCCESS )
	{
		kPacket << iPayAmt;
		kPacket << 0; // TransactionID ( FOR US )
		kPacket << szChargeNo;
 
		ItemInfo kItemInfo;
		GetItemInfo( pInfo->m_kPacket, iType, kItemInfo );
		SetItemInfo( kPacket, iType, kItemInfo );

		kPacket << iChannelingType;  // 공통
		kPacket << rkResult.m_iMCash;
		kPacket << rkResult.m_iPCash;

		static TwoOfINTVec vInfo;
		vInfo.clear();

		pInfo->GetBonusCashInfo(vInfo);
		int iiBonusCashSize	= vInfo.size();

		kPacket << iiBonusCashSize	;

		for( int i = 0; i < iiBonusCashSize	; i++ )
		{
			kPacket << vInfo[i].value1 << vInfo[i].value2;
		}
	}

	if( !g_ServerNodeManager.SendMessageIP( szServerIP, iClientPort, kPacket ) )
	{
		LOG.PrintTimeAndLog( 0, "[warning][mgame]%s Send Fail: %d:%s:MgameID %s:Ret %d:%d:%d", __FUNCTION__, dwUserIndex, szBillingGUID.c_str(), rkResult.m_szUserID,  rkResult.m_iRet, rkResult.m_iMCash, rkResult.m_iPCash );
		g_LogDBClient.OnInsertBillingServerError( CNT_MGAME, BILLING_ERROR_LOG_DISCONNECT_GAMESERVER,"disconnect game server." );
		return;
	}

	if( iReturn == CASH_RESULT_SUCCESS )
		LOG.PrintTimeAndLog( 0, "%s Success: %d:%s:MgameID %s:Ret %d:%d:%d", __FUNCTION__, dwUserIndex, szBillingGUID.c_str(), rkResult.m_szUserID, rkResult.m_iRet, rkResult.m_iMCash, rkResult.m_iPCash );
	else
	{
		LOG.PrintTimeAndLog( 0, "%s Fail: %d:%s:MgameID %s:Ret %d:%d:%d", __FUNCTION__, dwUserIndex, szBillingGUID.c_str(), rkResult.m_szUserID,  rkResult.m_iRet, rkResult.m_iMCash, rkResult.m_iPCash );
		g_LogDBClient.OnInsertBillingServerError( CNT_MGAME, rkResult.m_iRet,"Error return value." );
	}
}