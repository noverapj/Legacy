#include "../stdafx.h"
#include "../NodeInfo/ServerNode.h"
#include "../NodeInfo/ServerNodeManager.h"
#include "../database/logdbclient.h"
#include "./ioChannelingNodeTooniland.h"
#include "../MainProcess.h"
#include "../Local\ioLocalParent.h"
#include "../ToonilandBillingServer\ToonilandBillingServer.h"
#include "../EtcHelpFunc.h"

extern CLog LOG;

ioChannelingNodeTooniland::ioChannelingNodeTooniland(void)
{
}

ioChannelingNodeTooniland::~ioChannelingNodeTooniland(void)
{
}

ChannelingType ioChannelingNodeTooniland::GetType()
{
	return CNT_TOONILAND;
}

void ioChannelingNodeTooniland::_OnGetCash( ServerNode *pServerNode, SP2Packet &rkPacket )
{
	int          iChannelingType   = 0;
	ioHashString szBillingGUID;
	DWORD        dwUserIndex       = 0;
	ioHashString szPrivateID;
	ioHashString szPublicID;
	bool         bSetUserMouse     = false;
	ioHashString szToonilandID;

	rkPacket >> iChannelingType >> szBillingGUID >> dwUserIndex >> szPrivateID >> szPublicID >> bSetUserMouse; // 공통 사항
	rkPacket >> szToonilandID; // 투니랜드용

	if( g_App.IsReserveLogOut() )
	{
		LOG.PrintTimeAndLog( 0, "%s LogOut: %d:%s:%s:%s",__FUNCTION__, dwUserIndex, szBillingGUID.c_str(), szPrivateID.c_str(), szToonilandID.c_str() );
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
		LOG.PrintTimeAndLog( 0, "%s pInfo == NULL: %d:%s:%s:%s:%d",__FUNCTION__, dwUserIndex, szBillingGUID.c_str(), szPrivateID.c_str(), szToonilandID.c_str(), GetLastError()  );
		SP2Packet kPacket( BSTPK_GET_CASH_RESULT );
		kPacket << dwUserIndex;
		kPacket << szBillingGUID;
		kPacket << bSetUserMouse;
		kPacket << CASH_RESULT_EXCEPT;
		pServerNode->SendMessage( kPacket );
		g_LogDBClient.OnInsertBillingServerError( CNT_TOONILAND, BILLING_ERROR_LOG_FAIL_ADD_BUYINFO,"fail add buy info." );
		return;
	}


	char szAgencyNo[ioLocalParent::MAX_AGENCY_NO_PLUS_ONE]="";
	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( pLocal )
	{
		pLocal->GetAgencyNo( szAgencyNo, sizeof( szAgencyNo ), true );
	}

	pInfo->m_szKey        = szAgencyNo;
	pInfo->m_eType		  = BillInfoManager::AT_GET;
	pInfo->m_dwCreateTime = TIMEGETTIME();
	pInfo->m_iChannelingType = CNT_TOONILAND;
	pInfo->m_kPacket << dwUserIndex << szBillingGUID << bSetUserMouse << pServerNode->GetIP() << pServerNode->GetClientPort();
	pInfo->m_kPacket.SetPosBegin();
	if( !g_BillInfoManager->Add( pInfo))
	{
		LOG.PrintTimeAndLog( 0, "%s BillingInfo is full.: %d:%s:%s:%s:%d",__FUNCTION__, dwUserIndex, szBillingGUID.c_str(), szPrivateID.c_str(), szToonilandID.c_str(), GetLastError()  );
		SP2Packet kPacket( BSTPK_GET_CASH_RESULT );
		kPacket << dwUserIndex;
		kPacket << szBillingGUID;
		kPacket << bSetUserMouse;
		kPacket << CASH_RESULT_EXCEPT;
		pServerNode->SendMessage( kPacket );
		g_LogDBClient.OnInsertBillingServerError( CNT_TOONILAND, BILLING_ERROR_LOG_FAIL_ADD_BUYINFO,"fail add buy info." );
		return;
	}
#ifdef _TEST

	SP2Packet testkPacket( BSTPK_GET_CASH_RESULT );
	testkPacket<< dwUserIndex;
	testkPacket<< szBillingGUID;
	testkPacket<< bSetUserMouse;
	testkPacket<< CASH_RESULT_SUCCESS;
	testkPacket<< 500;
	testkPacket<< 500;

	pServerNode->SendMessage ( testkPacket );
	g_BillInfoManager->Delete( szAgencyNo );
	return;
#endif
	char szPacket[MAX_PATH]=""; //kyg ?? 
	if( g_App.IsTestMode() )
		StringCbPrintf( szPacket, sizeof( szPacket ), "toon|onmedia|auth|%s|%s|WEMADE|LOSTSAGA-DEV|\n", szAgencyNo, szToonilandID.c_str() );
	else
		StringCbPrintf( szPacket, sizeof( szPacket ), "toon|onmedia|auth|%s|%s|WEMADE|LOSTSAGA|\n", szAgencyNo, szToonilandID.c_str() );
 

	char szUTF8[MAX_PATH*2]="";
	ZeroMemory( szUTF8, sizeof( szUTF8 ) );
	int iReturnUTF8Size = 0;
	AnsiToUTF8( szPacket, szUTF8, iReturnUTF8Size, sizeof( szUTF8 ) );

	if( iReturnUTF8Size <= 0 )
	{
		LOG.PrintTimeAndLog( 0, "%s UTF8 Size Error.: %d:%s:%s:%s:%d",__FUNCTION__, dwUserIndex, szBillingGUID.c_str(), szPrivateID.c_str(), szToonilandID.c_str(), GetLastError()  );
		SP2Packet kPacket( BSTPK_GET_CASH_RESULT );
		kPacket << dwUserIndex;
		kPacket << szBillingGUID;
		kPacket << bSetUserMouse;
		kPacket << CASH_RESULT_EXCEPT;
		pServerNode->SendMessage( kPacket );

		ioHashString sAgencyNo = szAgencyNo;

		g_BillInfoManager->Delete( sAgencyNo );
		g_LogDBClient.OnInsertBillingServerError( CNT_TOONILAND, BILLING_ERROR_LOG_FAIL_ADD_BUYINFO,"UTF8 Size Error." );
		return;
	}

	SP2Packet kPacket( BTNTPK_FIND_CASH_REQUEST );
	kPacket.SetDataAdd( szUTF8, iReturnUTF8Size );
	kPacket.SetBufferSizeMinusOne();

	if( !g_ToonilandBillingServer.SendMessage( kPacket ) )
	{
		LOG.PrintTimeAndLog( 0, "%s Fail Send: %d:%s:%s:%s:%d",__FUNCTION__, dwUserIndex, szBillingGUID.c_str(), szPrivateID.c_str(), szToonilandID.c_str(), GetLastError()  );
		SP2Packet kPacket( BSTPK_GET_CASH_RESULT );
		kPacket << dwUserIndex;
		kPacket << szBillingGUID;
		kPacket << bSetUserMouse;
		kPacket << CASH_RESULT_EXCEPT;
		pServerNode->SendMessage( kPacket );

		ioHashString sAgencyNo = szAgencyNo;

		g_BillInfoManager->Delete( sAgencyNo );

		g_LogDBClient.OnInsertBillingServerError( CNT_TOONILAND, BILLING_ERROR_LOG_FAIL_SEND_TOONILAND_BILLINGSERVER,"fail send tooniland billingserver." );
		return;
	}
 
	//LOG.PrintTimeAndLog( 0, "%s Send Success: %d:%s:%s:%s:[%s:%s]",__FUNCTION__, dwUserIndex, szBillingGUID.c_str(), szPrivateID.c_str(), szPublicID.c_str(), szAgencyNo, szToonilandID.c_str() );
}

void ioChannelingNodeTooniland::OnRecieveGetCash( const ioHashString &rsPacket )
{
	/*
	 auth|2011102814361200001|0-10000\n
	 auth|2011102814361200002|1-002-User does not exist./n
	 */
	
	if( rsPacket.IsEmpty() )
	{
		LOG.PrintTimeAndLog( 0, "[error][tooniland]%s Empty.", __FUNCTION__ );
		return;
	}
 

	ioHashStringVec vParseStr;
	Help::GetParseData( rsPacket, vParseStr, TOONILAND_TOKEN );

	if( vParseStr.size() < MAX_GET_CASH_ARRAY )
	{
		LOG.PrintTimeAndLog( 0, "%s Size Error [%s].", __FUNCTION__ , rsPacket.c_str() );
		return;
	}

	BillInfoManager::BillingInfo *pInfo = g_BillInfoManager->Get( vParseStr[GET_CASH_ARRAY_CPID] );
	if( !pInfo )
	{
		if( g_ToonilandBillingServer.GetAliveCheckCPID() == vParseStr[GET_CASH_ARRAY_CPID] )
		{
			g_ToonilandBillingServer.SetSendAlive( false );
		}
		else
			LOG.PrintTimeAndLog( 0, "%s pInfo == NULL. %s", __FUNCTION__, rsPacket.c_str() );
		return;
	}

	DWORD        dwUserIndex   = 0;
	ioHashString szBillingGUID;
	bool         bSetUserMouse = false;
	ioHashString szServerIP;
	int          iClientPort   = 0;
	pInfo->m_kPacket >> dwUserIndex >> szBillingGUID >> bSetUserMouse >> szServerIP >> iClientPort;

	ioHashStringVec vParseExtendStr;
	Help::GetParseData( vParseStr[GET_CASH_ARRAY_RESULT], vParseExtendStr, TOONILAND_EXTEND_TOKEN );

	if( vParseExtendStr.size() < MAX_GET_CASH_EXTEND_ARRAY )
	{
		LOG.PrintTimeAndLog( 0, "%s Size Error 2 [%d:%s][%s].", __FUNCTION__ , dwUserIndex, szBillingGUID.c_str(), rsPacket.c_str() );

		g_BillInfoManager->Delete( vParseStr[GET_CASH_ARRAY_CPID] );
		return;
	}

	int iToonilandResult = atoi( vParseExtendStr[GET_CASH_EXTEND_ARRAY_RESULT].c_str() );
	int iCash            = atoi( vParseExtendStr[GET_CASH_EXTEND_ARRAY_CASH].c_str() );

	int iReturn = CASH_RESULT_SUCCESS;
	if( iToonilandResult != 0 )
		iReturn = CASH_RESULT_EXCEPT;

	SP2Packet kPacket( BSTPK_GET_CASH_RESULT );
	kPacket << dwUserIndex;
	kPacket << szBillingGUID;
	kPacket << bSetUserMouse;
	kPacket << iReturn;
	kPacket << iCash;
	kPacket << iCash; // 구매한 캐쉬도 동일.
	if( !g_ServerNodeManager.SendMessageIP( szServerIP, iClientPort, kPacket ) )
	{
		LOG.PrintTimeAndLog( 0, "[warning][tooniland]%s Send Fail: %d:%s:%d:%d[%s]", __FUNCTION__, dwUserIndex, szBillingGUID.c_str(), iToonilandResult, iCash, vParseStr[GET_CASH_ARRAY_CPID].c_str() );
		g_LogDBClient.OnInsertBillingServerError( CNT_TOONILAND, BILLING_ERROR_LOG_DISCONNECT_GAMESERVER,"disconnect game server." );

		g_BillInfoManager->Delete( vParseStr[GET_CASH_ARRAY_CPID] );

		return;
	}

	if( iReturn == CASH_RESULT_SUCCESS )
	{
		LOG.PrintTimeAndLog( 0, "%s Success: %d:%s:%d:%d[%s]", __FUNCTION__, dwUserIndex, szBillingGUID.c_str(), iToonilandResult, iCash, vParseStr[GET_CASH_ARRAY_CPID].c_str()  );
	}
	else
	{
		LOG.PrintTimeAndLog( 0, "%s Fail: %d:%s:%d:%d[%s]", __FUNCTION__, dwUserIndex, szBillingGUID.c_str(), iToonilandResult, iCash, rsPacket.c_str() );
		g_LogDBClient.OnInsertBillingServerError( CNT_TOONILAND, iToonilandResult, "Error return value." );
	}

	g_BillInfoManager->Delete( vParseStr[GET_CASH_ARRAY_CPID] );
}

void ioChannelingNodeTooniland::_OnOutputCash( ServerNode *pServerNode, SP2Packet &rkPacket, DWORD dwGoodsNo, const ioHashString &rszGoodsName )
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
		g_LogDBClient.OnInsertBillingServerError( CNT_TOONILAND, BILLING_ERROR_LOG_FAIL_ADD_BUYINFO,"fail add buy info." );
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

	ioHashString szToonilandID;
	rkPacket >> szToonilandID; // 투니랜드용
	

	char szAgencyNo[ioLocalParent::MAX_AGENCY_NO_PLUS_ONE]="";
	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( pLocal )
	{
		pLocal->GetAgencyNo( szAgencyNo, sizeof( szAgencyNo ), true );
	}

	pInfo->m_szKey				= szAgencyNo;
	pInfo->m_dwCreateTime		= TIMEGETTIME();
	pInfo->m_eType				= BillInfoManager::AT_OUTPUT;
	pInfo->m_iChannelingType	= CNT_TOONILAND;
	pInfo->m_kPacket << dwUserIndex << szBillingGUID << iReturnItemPrice << iType << iPayAmt << iChannelingType << szPrivateID;
	pInfo->m_kPacket << pServerNode->GetIP() << pServerNode->GetClientPort();
	SetItemInfo( pInfo->m_kPacket, iType, kBuyInfo );
	pInfo->m_kPacket.SetPosBegin();


	if( !g_BillInfoManager->Add( pInfo ))
	{
		LOG.PrintTimeAndLog( 0, "%s BillingInfo is full.: %d:%s:%s:%s:%d",__FUNCTION__, dwUserIndex, szBillingGUID.c_str(), szPrivateID.c_str(), szToonilandID.c_str(), GetLastError()  );
		SP2Packet kPacket( BSTPK_OUTPUT_CASH_RESULT );
		kPacket << dwUserIndex;
		kPacket << szBillingGUID;
		kPacket << CASH_RESULT_EXCEPT;
		kPacket << iReturnItemPrice;
		kPacket << iType;
		pServerNode->SendMessage( kPacket );
		g_LogDBClient.OnInsertBillingServerError( CNT_TOONILAND, BILLING_ERROR_LOG_FAIL_ADD_BUYINFO,"fail add buy info." );
		return;
	}
#ifdef _TEST
	ioHashString szServerIP;
	int          iClientPort   = 0;
		
	BillInfoManager::BillingInfo *pInfo1 = g_BillInfoManager->Get( szAgencyNo );
	
	pInfo1->m_kPacket >> dwUserIndex >> szBillingGUID >> iReturnItemPrice >> iType >> iPayAmt >> iChannelingType >> szPrivateID >> szServerIP >> iClientPort;
	int iReturn = CASH_RESULT_SUCCESS;
	SP2Packet testkPacket( BSTPK_OUTPUT_CASH_RESULT );
	testkPacket<< dwUserIndex;
	testkPacket<< szBillingGUID;
	testkPacket<< iReturn;
	testkPacket<< iReturnItemPrice;
	testkPacket<< iType;
	
	// return value
	if( iReturn == CASH_RESULT_SUCCESS )
	{
		ioHashString szChargeNo = "";
		testkPacket<< iPayAmt;
		testkPacket<< 0; // TransactionID ( FOR US )
		testkPacket<< szChargeNo;
	
		ItemInfo kItemInfo;
		GetItemInfo( pInfo1->m_kPacket, iType, kItemInfo );
		SetItemInfo( testkPacket, iType, kItemInfo );
	
		testkPacket<< iChannelingType;  // 공통
		testkPacket<< 500;
		testkPacket<< 500;
	
		static TwoOfINTVec vInfo;
		vInfo.clear();
	
		pInfo1->GetBonusCashInfo(vInfo);
		int iiBonusCashSize	= vInfo.size();
	
		testkPacket<< iiBonusCashSize	;
	
		for( int i = 0; i < iiBonusCashSize	; i++ )
		{
			testkPacket<< vInfo[i].value1 << vInfo[i].value2;
		}
	}
	
	if( !g_ServerNodeManager.SendMessageIP( szServerIP, iClientPort, testkPacket ) )
	{
		return;
	}
	g_BillInfoManager->Delete( szAgencyNo );
	return;
#endif
	char szPacket[MAX_PATH*2]="";
	ZeroMemory( szPacket, sizeof( szPacket ) );//kyg ?? 
	if( g_App.IsTestMode() )
		StringCbPrintf( szPacket, sizeof( szPacket ), "toon|onmedia|bill|%s|%s|%d|WEMADE|LOSTSAGA-DEV|%d|%s|\n", szAgencyNo, szToonilandID.c_str(), iPayAmt, dwGoodsNo, rszGoodsName.c_str() );
	else
		StringCbPrintf( szPacket, sizeof( szPacket ), "toon|onmedia|bill|%s|%s|%d|WEMADE|LOSTSAGA|%d|%s|\n", szAgencyNo, szToonilandID.c_str(), iPayAmt, dwGoodsNo, rszGoodsName.c_str() );
 
	char szUTF8[MAX_PATH*2]="";
	ZeroMemory( szUTF8, sizeof( szUTF8 ) );
	int iReturnUTF8Size = 0;
	AnsiToUTF8( szPacket, szUTF8, iReturnUTF8Size, sizeof( szUTF8 ) );

	if( iReturnUTF8Size <= 0 )
	{
		LOG.PrintTimeAndLog( 0, "%s Fail UTF8: %d:%s:%s:%s:%d",__FUNCTION__, dwUserIndex, szBillingGUID.c_str(), szPrivateID.c_str(), szToonilandID.c_str(), GetLastError()  );
		SP2Packet kPacket( BSTPK_OUTPUT_CASH_RESULT );
		kPacket << dwUserIndex;
		kPacket << szBillingGUID;
		kPacket << CASH_RESULT_EXCEPT;
		kPacket << iReturnItemPrice;
		kPacket << iType;
		pServerNode->SendMessage( kPacket );

		ioHashString sAgencyNo = szAgencyNo;
		
		g_BillInfoManager->Delete( sAgencyNo );
		g_LogDBClient.OnInsertBillingServerError( CNT_TOONILAND, BILLING_ERROR_LOG_EXCEPTION,"Error UTF8." );
		return;
	}

	SP2Packet kPacket( BTNTPK_BUY_CASH_REQUEST );
	kPacket.SetDataAdd( szUTF8, iReturnUTF8Size );
	kPacket.SetBufferSizeMinusOne();
	if( !g_ToonilandBillingServer.SendMessage( kPacket ) )
	{
		LOG.PrintTimeAndLog( 0, "%s Fail Send: %d:%s:%s:%s:%d",__FUNCTION__, dwUserIndex, szBillingGUID.c_str(), szPrivateID.c_str(), szToonilandID.c_str(), GetLastError()  );
		SP2Packet kPacket( BSTPK_OUTPUT_CASH_RESULT );
		kPacket << dwUserIndex;
		kPacket << szBillingGUID;
		kPacket << CASH_RESULT_EXCEPT;
		kPacket << iReturnItemPrice;
		kPacket << iType;
		pServerNode->SendMessage( kPacket );

		ioHashString sAgencyNo = szAgencyNo;
		g_BillInfoManager->Delete( sAgencyNo );

		g_LogDBClient.OnInsertBillingServerError( CNT_TOONILAND, BILLING_ERROR_LOG_FAIL_SEND_TOONILAND_BILLINGSERVER,"fail send tooniland billingserver." );
		return;
	}

	//LOG.PrintTimeAndLog( 0, "%s Send Success: %d:%s:%s:%s:%s:Price%d:%d[%s:%s]",__FUNCTION__, dwUserIndex, szBillingGUID.c_str(), szPrivateID.c_str(), szPublicID.c_str(), szUserIP.c_str(), iPayAmt, dwGoodsNo, szAgencyNo, szToonilandID.c_str() );
}

void ioChannelingNodeTooniland::OnRecieveOutputCash( const ioHashString &rsPacket )
{
	/*
	 bill|2011102814361200001|0-2611594-10000\n
	 auth|2011102814361200002|1-002-User does not exist./n
	 */
	if( rsPacket.IsEmpty() )
	{
		LOG.PrintTimeAndLog( 0, "[error][tooniland]%s Empty.", __FUNCTION__ );
		return;
	}

	ioHashStringVec vParseStr;
	Help::GetParseData( rsPacket, vParseStr, TOONILAND_TOKEN );

	if( vParseStr.size() < MAX_BUY_CASH_ARRAY )
	{
		LOG.PrintTimeAndLog( 0, "%s Size Error [%s].", __FUNCTION__ , rsPacket.c_str() );
		return;
	}
	BillInfoManager::BillingInfo *pInfo = g_BillInfoManager->Get( vParseStr[BUY_CASH_ARRAY_CPID] );
	if( !pInfo )
	{
		LOG.PrintTimeAndLog( 0, "%s pInfo == NULL. %s", __FUNCTION__, rsPacket.c_str() );
		return;
	}

	DWORD        dwUserIndex   = 0;
	ioHashString szBillingGUID;
	int			 iReturnItemPrice = 0;
	int          iType         = 0;
	int          iPayAmt       = 0;
	int          iChannelingType = 0;
	ioHashString szPrivateID;
	ioHashString szServerIP;
	int          iClientPort   = 0;
	pInfo->m_kPacket >> dwUserIndex >> szBillingGUID >> iReturnItemPrice >> iType >> iPayAmt >> iChannelingType >> szPrivateID >> szServerIP >> iClientPort;

	ioHashStringVec vParseExtendStr;
	Help::GetParseData( vParseStr[BUY_CASH_ARRAY_RESULT], vParseExtendStr, TOONILAND_EXTEND_TOKEN );

	if( vParseExtendStr.size() < MAX_BUY_CASH_EXTEND_ARRAY )
	{
		LOG.PrintTimeAndLog( 0, "%s Size Error 2 [%d:%s:%s][%s].", __FUNCTION__ , dwUserIndex, szBillingGUID.c_str(), szPrivateID.c_str(), rsPacket.c_str() );

		g_BillInfoManager->Delete( vParseStr[BUY_CASH_ARRAY_CPID] );
		return;
	}

	int iToonilandResult = atoi( vParseExtendStr[BUY_CASH_EXTEND_ARRAY_RESULT].c_str() );
	int iCash            = atoi( vParseExtendStr[BUY_CASH_EXTEND_ARRAY_CASH].c_str() );

	int iReturn = CASH_RESULT_SUCCESS;
	if( iToonilandResult != 0 )
		iReturn = CASH_RESULT_EXCEPT;

	SP2Packet kPacket( BSTPK_OUTPUT_CASH_RESULT );
	kPacket << dwUserIndex;
	kPacket << szBillingGUID;
	kPacket << iReturn;
	kPacket << iReturnItemPrice ;
	kPacket << iType;

	// return value
	if( iReturn == CASH_RESULT_SUCCESS )
	{
		ioHashString szChargeNo = vParseExtendStr[BUY_CASH_EXTEND_ARRAY_BOUGHTID];
		kPacket << iPayAmt;
		kPacket << 0; // TransactionID ( FOR US )
		kPacket << szChargeNo;

		ItemInfo kItemInfo;
		GetItemInfo( pInfo->m_kPacket, iType, kItemInfo );
		SetItemInfo( kPacket, iType, kItemInfo );

		// Cancel Step 1
		kPacket << iChannelingType;  // 공통
		kPacket << iCash;
		kPacket << iCash; // 구매한 캐쉬도 동일.

		static TwoOfINTVec vInfo;
		vInfo.clear();

		pInfo->GetBonusCashInfo(vInfo);
		int iiBonusCashSize	= vInfo.size();

		kPacket << iiBonusCashSize	;

		for( int i = 0; i < iiBonusCashSize	; i++ )
		{
			kPacket << vInfo[i].value1 << vInfo[i].value2;
		}

		kPacket << szPrivateID;
		kPacket << vParseExtendStr[BUY_CASH_EXTEND_ARRAY_BOUGHTID];
	}
	else
	{
		kPacket << true;
		kPacket << vParseExtendStr[BUY_CASH_EXTEND_ARRAY_ERROR_DESC];
	}

	if( !g_ServerNodeManager.SendMessageIP( szServerIP, iClientPort, kPacket ) )
	{
		LOG.PrintTimeAndLog( 0, "[warning][tooniland]%s Send Fail: %d:%s:%s:[%s]", __FUNCTION__, dwUserIndex, szBillingGUID.c_str(), szPrivateID.c_str(), rsPacket.c_str() );
		g_LogDBClient.OnInsertBillingServerError( CNT_TOONILAND, BILLING_ERROR_LOG_DISCONNECT_GAMESERVER,"disconnect game server." );

		g_BillInfoManager->Delete( vParseStr[BUY_CASH_ARRAY_CPID] );
		return;
	}

	if( iReturn == CASH_RESULT_SUCCESS )
		LOG.PrintTimeAndLog( 0, "%s Success: %d:%s:%s:%s:%s", __FUNCTION__, dwUserIndex, szBillingGUID.c_str(), szPrivateID.c_str(), rsPacket.c_str(), vParseStr[BUY_CASH_ARRAY_CPID].c_str() );
	else
	{
		LOG.PrintTimeAndLog( 0, "%s Fail: %d:%s:%s:%s:%s", __FUNCTION__, dwUserIndex, szBillingGUID.c_str(), szPrivateID.c_str(), rsPacket.c_str(), vParseStr[BUY_CASH_ARRAY_CPID].c_str() );
		g_LogDBClient.OnInsertBillingServerError( CNT_TOONILAND, iToonilandResult,"Error return value." );
	}
	g_BillInfoManager->Delete( vParseStr[BUY_CASH_ARRAY_CPID] );

#if(0) //kyg test영역
	ioHashString toonID = "cash12";
	SP2Packet pk(1);
	WORD dd = 3;
	pk << iChannelingType;
	pk << szBillingGUID;
	pk << dwUserIndex;
	pk << szPrivateID;
	pk << szPrivateID;
	pk << szPrivateID;
	pk << false;
	pk <<  vParseExtendStr[BUY_CASH_EXTEND_ARRAY_BOUGHTID];
	pk << dd;
	pk << toonID;
	pk.SetPosBegin();
	_OnSubscriptionRetract(NULL,pk);
#endif
}

void ioChannelingNodeTooniland::AnsiToUTF8( IN const char *szAnsi, OUT char *szUTF8, OUT int &riReturnUTF8Size, IN int iUTF8BufferSize )
{
	if( szAnsi == NULL )
		return;

	WCHAR szUni[MAX_PATH*2];
	ZeroMemory( szUni, sizeof(szUni) );

	int iUnisize = MultiByteToWideChar(CP_ACP, 0, szAnsi, -1 , szUni, MAX_PATH*2 );
	riReturnUTF8Size = WideCharToMultiByte(CP_UTF8, 0 , szUni, iUnisize, szUTF8, iUTF8BufferSize ,NULL ,NULL );
}

void ioChannelingNodeTooniland::OnCancelCash( ServerNode *pServerNode, SP2Packet &rkPacket )
{
#if 0 
	int          iChannelingType = 0;
	ioHashString szBillingGUID;
	DWORD        dwUserIndex = 0;
	ioHashString szPrivateID;
	ioHashString szOrderNo;

	// 공통
	rkPacket >> iChannelingType; 
	rkPacket >> szBillingGUID; 
	rkPacket >> dwUserIndex; 

	// Cancel Step 4
	rkPacket >> szPrivateID;
	rkPacket >> szOrderNo;

	char szAgencyNo[ioLocalParent::MAX_AGENCY_NO_PLUS_ONE]="";
	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( pLocal )
	{
		pLocal->GetAgencyNo( szAgencyNo, sizeof( szAgencyNo ), true );
	}

	char szPacket[MAX_PATH*2]="";
	ZeroMemory( szPacket, sizeof( szPacket ) );
	StringCbPrintf( szPacket, sizeof( szPacket ), "toon|onmedia|cancel|%s|%s|%s|WEMADE|LOSTSAGA|\n", szAgencyNo, szOrderNo.c_str(), szPrivateID.c_str() );

	char szUTF8[MAX_PATH*2]="";
	ZeroMemory( szUTF8, sizeof( szUTF8 ) );
	int iReturnUTF8Size = 0;
	AnsiToUTF8( szPacket, szUTF8, iReturnUTF8Size, sizeof( szUTF8 ) );

	if( iReturnUTF8Size <= 0 )
	{
		LOG.PrintTimeAndLog( 0, "%s Fail UTF8: %d:%s:%s:%s:%d",__FUNCTION__, dwUserIndex, szBillingGUID.c_str(), szPrivateID.c_str(), szOrderNo.c_str(), GetLastError()  );
		g_LogDBClient.OnInsertBillingServerError( CNT_TOONILAND, BILLING_ERROR_LOG_EXCEPTION,"Cancel Error UTF8." );
		return;
	}

	SP2Packet kPacket( BTNTPK_SUBSCRIPTION_RETRACT_CASH_REQUEST );
	kPacket.SetDataAdd( szUTF8, iReturnUTF8Size );
	kPacket.SetBufferSizeMinusOne();
	if( !g_ToonilandBillingServer.SendMessage( kPacket ) )
	{
		LOG.PrintTimeAndLog( 0, "%s Fail Send: %d:%s:%s:%s:%d",__FUNCTION__, dwUserIndex, szBillingGUID.c_str(), szToonilandID.c_str(), szOrderNo.c_str(), GetLastError()  );
		g_LogDBClient.OnInsertBillingServerError( CNT_TOONILAND, BILLING_ERROR_LOG_FAIL_SEND_TOONILAND_BILLINGSERVER,"fail send tooniland billingserver." );
		return;
	}

	LOG.PrintTimeAndLog( 0, "%s Send: %d:%s:%s:%s:[%s]",__FUNCTION__, dwUserIndex, szBillingGUID.c_str(), szToonilandID.c_str(), szOrderNo.c_str(), szAgencyNo );
#endif
}



void ioChannelingNodeTooniland::_OnSubscriptionRetract( ServerNode* pServerNode, SP2Packet& rkPacket )
{//kyg pServerNode가 널이면 걍 리턴 
	if(pServerNode == NULL)
		return;
	int          iChannelingType = 0;
	ioHashString szBillingGUID;
	DWORD        dwUserIndex = 0;
	ioHashString szPublicID;
	ioHashString szPrivateID;
	DWORD		 dwIndex = 0;
	ioHashString szChargeNo;
	ioHashString szToonilandID;

	rkPacket >> iChannelingType;
	rkPacket >> szBillingGUID;
	rkPacket >> dwUserIndex;
	rkPacket >> szPublicID;
	rkPacket >> szPrivateID;
	rkPacket >> dwIndex;
	//위에까지 공통인자 
	rkPacket >> szChargeNo;
	rkPacket >> szToonilandID;
	
 
	char szAgencyNo[ioLocalParent::MAX_AGENCY_NO_PLUS_ONE]="";
	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( pLocal )
	{
		pLocal->GetAgencyNo( szAgencyNo, sizeof( szAgencyNo ), true );
	}

	BillInfoManager::BillingInfo *pInfo = g_BillInfoManager->PopBillInfo();

	if( !pInfo )
	{
		LOG.PrintTimeAndLog( 0, "%s pInfo == NULL: %d:%s:%s:%s:%d",__FUNCTION__, dwUserIndex, szBillingGUID.c_str(), szPrivateID.c_str(), szPublicID.c_str(), GetLastError()  );

		SendSubscriptionRetractErrorPacket( pServerNode,BSTPK_SUBSCRIPTION_RETRACT_CASH_CHECK_RESULT,dwUserIndex,szBillingGUID );

		g_LogDBClient.OnInsertBillingServerError( CNT_TOONILAND, BILLING_ERROR_LOG_FAIL_SEND_WEMADE_BILLINGSERVER,"fail SubscriptionRetract." );
		return;
	}
	//test 
	//ioHashString serverIP;
	//int serverPort;
  
	pInfo->m_szKey        = szAgencyNo; //key를 azAgentNo으로
	pInfo->m_eType		  = BillInfoManager::AT_SUBSCRIPTIONRETRACT;
	pInfo->m_dwCreateTime = TIMEGETTIME();
	pInfo->m_kPacket << dwUserIndex;
	pInfo->m_kPacket << szBillingGUID;
	pInfo->m_kPacket << pServerNode->GetIP();
	pInfo->m_kPacket << pServerNode->GetClientPort();;
	pInfo->m_kPacket << dwIndex;
	pInfo->m_kPacket << szChargeNo; //kyg chargeno으로 인증할건지 안할건지 정해야함 

	pInfo->m_kPacket.SetPosBegin();
	
	if( !g_BillInfoManager->Add( pInfo ) )
	{
		LOG.PrintTimeAndLog( 0, "%s BillingInfo is full.: %d:%s:%s:%s:%d",__FUNCTION__, dwUserIndex, szBillingGUID.c_str(), szPrivateID.c_str(), szPublicID.c_str(), GetLastError()  );

		SendSubscriptionRetractErrorPacket( pServerNode,BSTPK_SUBSCRIPTION_RETRACT_CASH_CHECK_RESULT,dwUserIndex,szBillingGUID );

		g_LogDBClient.OnInsertBillingServerError( CNT_TOONILAND, BILLING_ERROR_LOG_FAIL_SEND_WEMADE_BILLINGSERVER,"fail SubscriptionRetract." );
		return;
	}


	char szPacket[MAX_PATH*2]="";
	ZeroMemory( szPacket, sizeof( szPacket ) );
	if( g_App.IsTestMode() )
		StringCbPrintf( szPacket, sizeof( szPacket ), "toon|onmedia|cancel|%s|%s|%s|WEMADE|LOSTSAGA-DEV|\n", szAgencyNo, szChargeNo.c_str(), szToonilandID.c_str() );
	else 
		StringCbPrintf( szPacket, sizeof( szPacket ), "toon|onmedia|cancel|%s|%s|%s|WEMADE|LOSTSAGA|\n", szAgencyNo, szChargeNo.c_str(), szToonilandID.c_str() );
 
	char szUTF8[MAX_PATH*2]="";
	ZeroMemory( szUTF8, sizeof( szUTF8 ) );
	int iReturnUTF8Size = 0;
	AnsiToUTF8( szPacket, szUTF8, iReturnUTF8Size, sizeof( szUTF8 ) );

	if( iReturnUTF8Size <= 0 )
	{
		LOG.PrintTimeAndLog( 0, "%s Fail UTF8: %d:%s:%s:%s:%d",__FUNCTION__, dwUserIndex, szBillingGUID.c_str(), szToonilandID.c_str(), szChargeNo.c_str(), GetLastError()  );
		g_LogDBClient.OnInsertBillingServerError( CNT_TOONILAND, BILLING_ERROR_LOG_EXCEPTION,"Cancel Error UTF8." );
		return;
	}

	SP2Packet kPacket( BTNTPK_SUBSCRIPTION_RETRACT_CASH_REQUEST );
	kPacket.SetDataAdd( szUTF8, iReturnUTF8Size );
	kPacket.SetBufferSizeMinusOne();
	if( !g_ToonilandBillingServer.SendMessage( kPacket ) )
	{
		LOG.PrintTimeAndLog( 0, "%s Fail Send: %d:%s:%s:%s:%d",__FUNCTION__, dwUserIndex, szBillingGUID.c_str(), szToonilandID.c_str(), szChargeNo.c_str(), GetLastError()  );
		g_LogDBClient.OnInsertBillingServerError( CNT_TOONILAND, BILLING_ERROR_LOG_FAIL_SEND_TOONILAND_BILLINGSERVER,"fail send tooniland billingserver." );
		return;
	}

	LOG.PrintTimeAndLog( 0, "%s Send: %d:%s:%s:%s:[%s]",__FUNCTION__, dwUserIndex, szBillingGUID.c_str(), szToonilandID.c_str(), szChargeNo.c_str(), szAgencyNo );
}

void ioChannelingNodeTooniland::OnReceiveRetractCash( const ioHashString &rsPacket )
{
 	if( rsPacket.IsEmpty() )
	{
		LOG.PrintTimeAndLog( 0, "[error][tooniland]%s Empty.", __FUNCTION__ );
		return;
	}

	ioHashStringVec vParseStr;
	Help::GetParseData( rsPacket, vParseStr, TOONILAND_TOKEN );

	if( vParseStr.size() < MAX_BUY_CASH_ARRAY )
	{
		LOG.PrintTimeAndLog( 0, "%s Size Error [%s].", __FUNCTION__ , rsPacket.c_str() );
		return;
	}
	BillInfoManager::BillingInfo *pInfo = g_BillInfoManager->Get( vParseStr[SUBSCRIPTION_RETRACT_ARRAY_BOUGHTID] );
	if( !pInfo )
	{
		LOG.PrintTimeAndLog( 0, "%s pInfo == NULL. %s", __FUNCTION__, rsPacket.c_str() );
		return;
	}

	DWORD		 dwUserIndex = 0;
	ioHashString szBillingGUID;
	int          iChannelingType = 0;
	ioHashString szServerIP;
	int          iClientPort   = 0;
	DWORD		 dwIndex = 0;
	ioHashString szChargeNo;

	pInfo->m_kPacket >> dwUserIndex >>  szBillingGUID >> szServerIP >> iClientPort >> dwIndex >> szChargeNo;

	ioHashStringVec vParseExtendStr;
	Help::GetParseData( vParseStr[BUY_CASH_ARRAY_RESULT], vParseExtendStr, TOONILAND_EXTEND_TOKEN );

	if( vParseExtendStr.size() < MAX_BUY_CASH_EXTEND_ARRAY )
	{
		LOG.PrintTimeAndLog( 0, "%s Size Error 2 [%d:%s:%s][%s].", __FUNCTION__ , dwUserIndex, szBillingGUID.c_str(), szChargeNo.c_str(), rsPacket.c_str() );
		g_BillInfoManager->Delete( vParseStr[SUBSCRIPTION_RETRACT_ARRAY_BOUGHTID] );
		return;
	}

	int iToonilandResult = atoi( vParseExtendStr[SUBSCRIPTION_RETRACT_ARRAY_RESULT].c_str() );
 

	int iReturn = BILLING_SUBSCRIPTION_RETRACT_RESULT_SUCCESS;
	if( iToonilandResult != 0 )
		iReturn = BILLING_SUBSCRIPTION_RETRACT_RESULT_FAIL;
 
	SP2Packet kPacket( BSTPK_SUBSCRIPTION_RETRACT_CASH_RESULT );

	kPacket << dwUserIndex;
	kPacket << szBillingGUID;
	kPacket << dwIndex;
	kPacket << szChargeNo;
	kPacket << iReturn;
 
	if( !g_ServerNodeManager.SendMessageIP( szServerIP, iClientPort, kPacket ) )
	{
		LOG.PrintTimeAndLog( 0, "[warning][tooniland]%s Fail: %d:%s:%s:%s", __FUNCTION__, dwUserIndex, szBillingGUID.c_str(),rsPacket.c_str(), vParseStr[SUBSCRIPTION_RETRACT_ARRAY_BOUGHTID].c_str() );
		g_LogDBClient.OnInsertBillingServerError( CNT_TOONILAND, BILLING_ERROR_LOG_DISCONNECT_GAMESERVER,"disconnect game server." );
		g_BillInfoManager->Delete( vParseStr[BUY_CASH_ARRAY_CPID] );
		return;
	}

 	if( iReturn == CASH_RESULT_SUCCESS )
 		LOG.PrintTimeAndLog( 0, "%s Success: %d:%s:%s:%s", __FUNCTION__, dwUserIndex, szBillingGUID.c_str(),rsPacket.c_str(), vParseStr[SUBSCRIPTION_RETRACT_ARRAY_BOUGHTID].c_str() );
 	else
 	{
 		LOG.PrintTimeAndLog( 0, "%s Fail: %d:%s:%s:%s", __FUNCTION__, dwUserIndex, szBillingGUID.c_str(),rsPacket.c_str(), vParseStr[SUBSCRIPTION_RETRACT_ARRAY_BOUGHTID].c_str() );
 		g_LogDBClient.OnInsertBillingServerError( CNT_TOONILAND, iToonilandResult,"Error return value." );
 	}
	g_BillInfoManager->Delete( vParseStr[SUBSCRIPTION_RETRACT_ARRAY_BOUGHTID] );
}