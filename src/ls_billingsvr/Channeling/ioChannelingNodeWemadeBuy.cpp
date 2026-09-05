#include "../stdafx.h"
#include "../NodeInfo/ServerNode.h"
#include "../NodeInfo/ServerNodeManager.h"
#include "../WemadeBillingServer/WemadeBuyServer.h"
#include "../database/logdbclient.h"
#include "./ioChannelingNodeWemadeBuy.h"
#include "../MainProcess.h"
#include "../ThreadPool\ioThreadPool.h"
#include "../xml/ioxmldocument.h"
#include "../xml/ioxmlelement.h"

extern CLog LOG;
#define MAXPOOL 8000
ioChannelingNodeWemadeBuy::ioChannelingNodeWemadeBuy(void)
{
	char WemadeFillURL[MAX_PATH]="";
	ioINILoader kLoader( "ls_config_billingsvr.ini" );
	kLoader.SetTitle( "Channeling" );
	kLoader.LoadString( "WemadeFillURL", "", WemadeFillURL, sizeof( WemadeFillURL ) );
	m_sURL = WemadeFillURL;
	m_iReqKey = 0;
	//m_BillInfoPool.CreatePool(3000,MAXPOOL,TRUE);
}

ioChannelingNodeWemadeBuy::~ioChannelingNodeWemadeBuy(void)
{

}

ChannelingType ioChannelingNodeWemadeBuy::GetType()
{
	return CNT_WEMADEBUY;
}

void ioChannelingNodeWemadeBuy::_OnGetCash( ServerNode *pServerNode, SP2Packet &rkPacket )
{
	int          iChannelingType   = 0;
	ioHashString szBillingGUID;
	DWORD        dwUserIndex       = 0;
	ioHashString szPrivateID;
	ioHashString szPublicID;
	ioHashString szPublicIP;
	bool         bSetUserMouse     = false;

	rkPacket >> iChannelingType >> szBillingGUID >> dwUserIndex >> szPrivateID >> szPublicID >> bSetUserMouse // 공통 사항
		     >> szPublicIP; // Wemadebuy용

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
		g_LogDBClient.OnInsertBillingServerError( CNT_WEMADEBUY, BILLING_ERROR_LOG_FAIL_ADD_BUYINFO,"fail add buy info." );
		return;
	}

	//hr//
	DWORD reqKey = InterlockedIncrement((LONG *)&m_iReqKey);


	pInfo->m_szKey			= szPrivateID;
	pInfo->m_dwCreateTime	= TIMEGETTIME();
	pInfo->m_eType			= BillInfoManager::AT_GET;
	pInfo->m_dwReqKey		= reqKey;

	pInfo->m_iChannelingType = CNT_WEMADEBUY;	//BillInfo 삭제시 로그용 
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
		g_LogDBClient.OnInsertBillingServerError( CNT_WEMADEBUY, BILLING_ERROR_LOG_FAIL_ADD_BUYINFO,"fail add buy info." );
		return;
	}
	
	CS_BALANCE kInfo;
	kInfo.SetInfo( reqKey, dwUserIndex, szPrivateID.c_str(), szPublicIP.c_str() );

	SP2Packet kPacket( BTWBTPK_BALANCE_REQUEST );
	kPacket << kInfo;
	if( !g_WemadeBuyServer.SendMessage( kPacket ) )
	{
		LOG.PrintTimeAndLog( 0, "%s Fail Send: %d:%s:%s:%s:%d",__FUNCTION__, dwUserIndex, szBillingGUID.c_str(), szPrivateID.c_str(), szPublicID.c_str(), GetLastError()  );
		SP2Packet kPacket( BSTPK_GET_CASH_RESULT );
		kPacket << dwUserIndex;
		kPacket << szBillingGUID;
		kPacket << bSetUserMouse;
		kPacket << CASH_RESULT_EXCEPT;
		pServerNode->SendMessage( kPacket );

		g_BillInfoManager->Delete( szPrivateID ); 
		g_LogDBClient.OnInsertBillingServerError( CNT_WEMADEBUY, BILLING_ERROR_LOG_FAIL_SEND_WEMADE_BILLINGSERVER,"fail send billingserver." );
		return;
	}
	LOG.PrintTimeAndLog( 0, "%s Send Success: %d:%s:%s:%s:%s",__FUNCTION__, dwUserIndex, szBillingGUID.c_str(), szPrivateID.c_str(), szPublicID.c_str(), szPublicIP.c_str() );

}

void ioChannelingNodeWemadeBuy::OnRecieveGetCash( const SC_BALANCE& rkResult )
{
	BillInfoManager::BillingInfo *pInfo = g_BillInfoManager->Get( rkResult.UserID );
		
	if( !pInfo )
	{
		LOG.PrintTimeAndLog( 0, "[warning][timeout]ioChannelingNodeWemadeBuy::OnRecieveGetCash Not UserInfo. %d:%s:%d:%d:%d:%s", 
								rkResult.UserNo, rkResult.UserID, rkResult.RealCash, rkResult.BonusCash, rkResult.ResultCode, rkResult.ResultMessage );
		return;
	}
  
	DWORD        dwUserIndex   = 0;
	ioHashString szBillingGUID;
	bool         bSetUserMouse = false;
	ioHashString szServerIP;
	int          iClientPort   = 0;
	pInfo->m_kPacket >> dwUserIndex >> szBillingGUID >> bSetUserMouse >> szServerIP >> iClientPort;

	if( rkResult.ResultCode != RESULT_CODE_SUCCESS )
	{
		ioHashString sResultMessage = rkResult.ResultMessage;

		SP2Packet kPacket( BSTPK_GET_CASH_RESULT );
		kPacket << dwUserIndex;
		kPacket << szBillingGUID;
		kPacket << bSetUserMouse;
		kPacket << CASH_RESULT_EXCEPT;
		kPacket << true;
		kPacket << sResultMessage;
		if( !g_ServerNodeManager.SendMessageIP( szServerIP, iClientPort, kPacket ) )
		{
			LOG.PrintTimeAndLog( 0, "%s Send Fail(1): %d:%s:%s:Ret %d:%d:%d", __FUNCTION__, dwUserIndex, szBillingGUID.c_str(), rkResult.UserID,  rkResult.ResultCode, rkResult.RealCash, rkResult.BonusCash );
			g_LogDBClient.OnInsertBillingServerError( CNT_WEMADEBUY, BILLING_ERROR_LOG_FAIL_SEND_WEMADE_BILLINGSERVER,"fail send billingserver." );
			g_BillInfoManager->Delete( rkResult.UserID);
			return;
		}
		LOG.PrintTimeAndLog( 0, "%s Fail: %d:%s:%s:Ret %d:%s:%d:%d", __FUNCTION__, dwUserIndex, szBillingGUID.c_str(), rkResult.UserID,  rkResult.ResultCode, rkResult.ResultMessage, rkResult.RealCash, rkResult.BonusCash );
		g_LogDBClient.OnInsertBillingServerError( CNT_WEMADEBUY, rkResult.ResultCode, "Error return value." );
		g_BillInfoManager->Delete( rkResult.UserID );
		return;
	}

	int iReturnCash    = ( rkResult.RealCash + rkResult.BonusCash );
	int iPurchasedCash = rkResult.RealCash;

	SP2Packet kPacket( BSTPK_GET_CASH_RESULT );
	kPacket << dwUserIndex;
	kPacket << szBillingGUID;
	kPacket << bSetUserMouse;
	kPacket << CASH_RESULT_SUCCESS;
	kPacket << iReturnCash;
	kPacket << iPurchasedCash;
	if( !g_ServerNodeManager.SendMessageIP( szServerIP, iClientPort, kPacket ) )
	{
		LOG.PrintTimeAndLog( 0, "%s Send Fail(2): %d:%s:%s:Ret %d:%d:%d", __FUNCTION__, dwUserIndex, szBillingGUID.c_str(), rkResult.UserID,  rkResult.ResultCode, rkResult.RealCash, rkResult.BonusCash );
		g_LogDBClient.OnInsertBillingServerError( CNT_WEMADEBUY, BILLING_ERROR_LOG_FAIL_SEND_WEMADE_BILLINGSERVER,"fail send billingserver." );
		g_BillInfoManager->Delete( rkResult.UserID );
		return;
	}
	LOG.PrintTimeAndLog( 0, "%s Success: %d:%s:%s:Ret %d:%d:%d", __FUNCTION__, dwUserIndex, szBillingGUID.c_str(), rkResult.UserID, rkResult.ResultCode, rkResult.RealCash, rkResult.BonusCash );

	g_BillInfoManager->Delete( rkResult.UserID );

}

void ioChannelingNodeWemadeBuy::_OnOutputCash( ServerNode *pServerNode, SP2Packet &rkPacket, DWORD dwGoodsNo, const ioHashString &rszGoodsName )
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
	int			 iPresentType = 0;
	int			 iBuyValue1 = 0;
	int			 iBuyValue2 = 0;
	rkPacket >> iChannelingType >> szBillingGUID >> iReturnItemPrice >> dwUserIndex >> szPublicID >> szPrivateID >> szUserIP >> iPayAmt >> iType; // 공통사항

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

	int          iGameServerPort = 0;
	ioHashString szReceivePrivateID; // 선물인 경우만 셋팅됨
	ioHashString szReceivePublicID;  // 선물인 경우만 셋팅됨

	rkPacket >> iGameServerPort >> szReceivePrivateID >> szReceivePublicID; // wemadebuy

	bool bPresent = false;
	if( iType == OUTPUT_CASH_PRESENT || 
		iType == OUTPUT_CASH_POPUP )
		bPresent  = true;
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
		g_LogDBClient.OnInsertBillingServerError( CNT_WEMADEBUY, BILLING_ERROR_LOG_FAIL_ADD_BUYINFO,"fail add buy info." );
		return;
	}

	//hr//
	DWORD reqKey = InterlockedIncrement((LONG *)&m_iReqKey);

	pInfo->m_szKey				= szPrivateID;
	pInfo->m_dwCreateTime		= TIMEGETTIME();
	pInfo->m_eType				= BillInfoManager::AT_OUTPUT;
	pInfo->m_iChannelingType	= CNT_WEMADEBUY;
	pInfo->m_dwReqKey			= reqKey;

	pInfo->m_kPacket << dwUserIndex << szBillingGUID << iReturnItemPrice << iType << iPayAmt << iChannelingType << szUserIP;
	pInfo->m_kPacket << pServerNode->GetIP() << pServerNode->GetClientPort();

	SetItemInfo( pInfo->m_kPacket, iType, kBuyInfo );
	pInfo->m_kPacket.SetPosBegin();

	if( !g_BillInfoManager->Add( pInfo ) )
	{
		LOG.PrintTimeAndLog( 0, "%s BillingInfo is full.: %d:%s:%s:%s:%d",__FUNCTION__, dwUserIndex, szBillingGUID.c_str(), szPrivateID.c_str(), szPublicID.c_str(), GetLastError()  );
		SP2Packet kPacket( BSTPK_OUTPUT_CASH_RESULT );
		kPacket << dwUserIndex;
		kPacket << szBillingGUID;
		kPacket << CASH_RESULT_EXCEPT;
		kPacket << iReturnItemPrice;
		kPacket << iType;
		pServerNode->SendMessage( kPacket );
		g_LogDBClient.OnInsertBillingServerError( CNT_WEMADEBUY, BILLING_ERROR_LOG_FAIL_ADD_BUYINFO,"fail add buy info." );
		return;
	}

	
	char szGoodsNo[MAX_PATH]="";
	StringCbPrintf( szGoodsNo, sizeof( szGoodsNo ), "%u", dwGoodsNo );
	bool bSend = false;
	if( bPresent )
	{
		CS_GIFTITEM kInfo;
		kInfo.SetInfo( reqKey, dwUserIndex, szPrivateID.c_str(), szPublicID.c_str(), szUserIP.c_str(), (WORD)iGameServerPort, iPayAmt, szGoodsNo, rszGoodsName.c_str(), szReceivePrivateID.c_str(), szReceivePublicID.c_str() );

		SP2Packet kPacket( BTWBTPK_PRESENT_REQUEST );
		kPacket << kInfo;
		bSend = g_WemadeBuyServer.SendMessage( kPacket );
	}
	else
	{
		CS_PURCHASEITEM kInfo;
		kInfo.SetInfo( reqKey, dwUserIndex, szPrivateID.c_str(), szPublicID.c_str(), szUserIP.c_str(), (WORD)iGameServerPort, iPayAmt, szGoodsNo, rszGoodsName.c_str() );

		SP2Packet kPacket( BTWBTPK_BUY_REQUEST );
		kPacket << kInfo;
		bSend = g_WemadeBuyServer.SendMessage( kPacket );
	}

	if( !bSend )
	{
		LOG.PrintTimeAndLog( 0, "%s Fail Send: %d:%s:%s:%s:%d",__FUNCTION__, dwUserIndex, szBillingGUID.c_str(), szPrivateID.c_str(), szPublicID.c_str(), GetLastError()  );
		SP2Packet kPacket( BSTPK_OUTPUT_CASH_RESULT );
		kPacket << dwUserIndex;
		kPacket << szBillingGUID;
		kPacket << CASH_RESULT_EXCEPT;
		kPacket << iReturnItemPrice;
		kPacket << iType;
		pServerNode->SendMessage( kPacket );
		g_BillInfoManager->Delete( szPrivateID );
		g_LogDBClient.OnInsertBillingServerError( CNT_WEMADEBUY, BILLING_ERROR_LOG_FAIL_SEND_WEMADE_BILLINGSERVER,"fail send billingserver." );
		return;
	}
 
	LOG.PrintTimeAndLog( 0, "%s Send Success: ReqKey:%d:%d:%s:%s:%s:%s:Price %d:%d:%s[%s:%s]",__FUNCTION__, pInfo->m_dwReqKey, dwUserIndex, szBillingGUID.c_str(), szPrivateID.c_str(), szPublicID.c_str()
		, szUserIP.c_str(), iPayAmt, dwGoodsNo, rszGoodsName.c_str(), szReceivePrivateID.c_str(), szReceivePublicID.c_str() );

}

void ioChannelingNodeWemadeBuy::OnRecieveOutputCash( const SC_PURCHASEITEM& rkResult )
{
	
	BillInfoManager::BillingInfo *pInfo = g_BillInfoManager->Get( rkResult.UserID );
    if( !pInfo )
	{
		LOG.PrintTimeAndLog( 0, "[warning][timeout]ioChannelingNodeWemadeBuy::OnRecieveOutputCash Not UserInfo. ReqKey:%d:%d:%s:%d:%d:%d:%s[%s]"
								,rkResult.ReqKey, rkResult.UserNo, rkResult.UserID, rkResult.RealCash, rkResult.BonusCash, rkResult.ResultCode, rkResult.ResultMessage, rkResult.ChargeNo );
		return;
	}
	//hr
	if( rkResult.ReqKey != pInfo->m_dwReqKey )
	{
		LOG.PrintTimeAndLog( 0, "[warning][timeout]ioChannelingNodeWemadeBuy::OnRecieveOutputCash ReqKey Different. ReqKey:%d:%d:%s:%d:%d:%d:%s[%s]", rkResult.ReqKey, rkResult.UserNo, rkResult.UserID, rkResult.RealCash, rkResult.BonusCash, rkResult.ResultCode, rkResult.ResultMessage, rkResult.ChargeNo );
		return;
	}
	DWORD        dwUserIndex   = 0;
	ioHashString szBillingGUID;
	int			 iReturnItemPrice = 0; 
	int          iType         = 0;
	int			 iPayAmt	   = 0;
	int          iChannelingType = 0;
	ioHashString szServerIP;
	int          iClientPort   = 0;
	ioHashString szUserIP;
	pInfo->m_kPacket >> dwUserIndex>> szBillingGUID >> iReturnItemPrice >> iType >> iPayAmt >> iChannelingType >> szUserIP >> szServerIP >> iClientPort;

	if( rkResult.ResultCode != RESULT_CODE_SUCCESS )
	{
		ioHashString sResultMessage = rkResult.ResultMessage;

		SP2Packet kPacket( BSTPK_OUTPUT_CASH_RESULT );
		kPacket << rkResult.UserNo;
		kPacket << szBillingGUID;
		kPacket << CASH_RESULT_EXCEPT;
		kPacket << iReturnItemPrice;
		kPacket << iType;
		kPacket << true;
		kPacket << sResultMessage;

		if( !g_ServerNodeManager.SendMessageIP( szServerIP, iClientPort, kPacket ) )
		{
			g_BillInfoManager->Delete( rkResult.UserID );
			LOG.PrintTimeAndLog( 0, "[warning][wemade]%s Send Fail: %d:%s:%s:Ret %d:%d:%d[%s]", __FUNCTION__, rkResult.UserNo, szBillingGUID.c_str(), rkResult.UserID,  rkResult.ResultCode, rkResult.RealCash, rkResult.BonusCash, rkResult.ChargeNo );
			g_LogDBClient.OnInsertBillingServerError( CNT_WEMADEBUY, BILLING_ERROR_LOG_DISCONNECT_GAMESERVER,"disconnect game server." );
			return;
		}
		g_BillInfoManager->Delete( rkResult.UserID );
		LOG.PrintTimeAndLog( 0, "%s Fail: %d:%s:%s:Ret %d:%s:%d:%d[%s]", __FUNCTION__, rkResult.UserNo, szBillingGUID.c_str(), rkResult.UserID,  rkResult.ResultCode, rkResult.ResultMessage, rkResult.RealCash, rkResult.BonusCash, rkResult.ChargeNo );
		g_LogDBClient.OnInsertBillingServerError( CNT_WEMADEBUY, rkResult.ResultCode,"Error return value." );
		return;
	}

	int iReturnCash    = ( rkResult.RealCash + rkResult.BonusCash );
	int iPurchasedCash = rkResult.RealCash;

	ioHashString sUserID   = rkResult.UserID;
	ioHashString sChargeNo = rkResult.ChargeNo;

	SP2Packet kPacket( BSTPK_OUTPUT_CASH_RESULT );

	kPacket << rkResult.UserNo;
	kPacket << szBillingGUID;
	kPacket << CASH_RESULT_SUCCESS;
	kPacket << iReturnItemPrice;
	kPacket << iType;
	kPacket << rkResult.ChargedAmt;
	kPacket << 0; // TransactionID ( FOR US )
	kPacket << sChargeNo;

	ItemInfo kItemInfo;
	GetItemInfo( pInfo->m_kPacket, iType, kItemInfo );
	SetItemInfo( kPacket, iType, kItemInfo );

	// Cancel Step 1
	kPacket << iChannelingType;  // 공통
	kPacket << iReturnCash;
	kPacket << iPurchasedCash;
	kPacket << sUserID;
	kPacket << szUserIP;
	kPacket << sChargeNo;

	if( !g_ServerNodeManager.SendMessageIP( szServerIP, iClientPort, kPacket ) )
	{
		LOG.PrintTimeAndLog( 0, "[warning][wemade]%s Send Fail: %d:%s:%s:Ret %d:%d:%d[%s]", __FUNCTION__, rkResult.UserNo, szBillingGUID.c_str(), rkResult.UserID,  rkResult.ResultCode, rkResult.RealCash, rkResult.BonusCash, rkResult.ChargeNo );
		g_LogDBClient.OnInsertBillingServerError( CNT_WEMADEBUY, BILLING_ERROR_LOG_DISCONNECT_GAMESERVER,"disconnect game server." );
		g_BillInfoManager->Delete( rkResult.UserID  );
		return;
	}
	g_BillInfoManager->Delete( rkResult.UserID );
	LOG.PrintTimeAndLog( 0, "%s Success: %d:%s:%s:Ret %d:%d:%d:%d[%s]", __FUNCTION__, rkResult.UserNo, szBillingGUID.c_str(), rkResult.UserID,  rkResult.ResultCode, rkResult.RealCash, rkResult.BonusCash, rkResult.ChargedAmt, rkResult.ChargeNo );
 
}

void ioChannelingNodeWemadeBuy::OnRecievePresentCash( const SC_GIFTITEM& rkResult )
{
	// 함수가 동일하여 구조체만 복사해서 넘긴다.
	
	SC_PURCHASEITEM kConvertResult;
	kConvertResult.Code    = rkResult.Code;
	kConvertResult.Length  = rkResult.Length;
	kConvertResult.Version = rkResult.Version;
	kConvertResult.ReqKey  = rkResult.ReqKey; 
	kConvertResult.GameID  = rkResult.GameID;
	kConvertResult.PCode   = rkResult.PCode;
	kConvertResult.Result  = rkResult.Result;
	kConvertResult.UserNo  = rkResult.UserNo;
	StringCbCopy( kConvertResult.UserID, sizeof( kConvertResult.UserID ), rkResult.UserID );  
	StringCbCopy( kConvertResult.ChargeNo, sizeof( kConvertResult.ChargeNo ), rkResult.ChargeNo );
	kConvertResult.ChargedAmt     = rkResult.ChargedCashAmt;
	StringCbCopy( kConvertResult.EventItemID, sizeof( kConvertResult.EventItemID ), rkResult.EventItemID );
	StringCbCopy( kConvertResult.EventChargeNo, sizeof( kConvertResult.EventChargeNo ), rkResult.EventChargeNo );
	kConvertResult.RealCash       = rkResult.RealCash;
	kConvertResult.BonusCash      = rkResult.BonusCash;
	kConvertResult.ResultCode     = rkResult.ResultCode;
	StringCbCopy( kConvertResult.ResultMessage, sizeof( kConvertResult.ResultMessage ), rkResult.ResultMessage );

	OnRecieveOutputCash( kConvertResult );
	//LOG.PrintTimeAndLog( 0, "PRESENT:%d:%s", rkResult.RUserNo, rkResult.RUserID );
}

void ioChannelingNodeWemadeBuy::OnCancelCash( ServerNode *pServerNode, SP2Packet &rkPacket )
{
	int          iChannelingType = 0;
	ioHashString szBillingGUID;
	DWORD        dwUserIndex = 0;
	ioHashString szPrivateID;
	ioHashString szUserIP;
	ioHashString szChargeNo;

	// 공통
	rkPacket >> iChannelingType; 
	rkPacket >> szBillingGUID; 
	rkPacket >> dwUserIndex; 

	// Cancel Step 4
	rkPacket >> szPrivateID;
	rkPacket >> szUserIP;
	rkPacket >> szChargeNo;

	//hr//
	DWORD reqKey = InterlockedIncrement((LONG *)&m_iReqKey);

	CS_CNLPURCHASE kInfo;
	kInfo.SetInfo( reqKey, dwUserIndex, szPrivateID.c_str(), szUserIP.c_str(), szChargeNo.c_str() );

	SP2Packet kPacket( BTWBTPK_CANCEL_REQUEST );
	kPacket << kInfo;
	if( !g_WemadeBuyServer.SendMessage( kPacket ) )
	{
		LOG.PrintTimeAndLog( 0, "%s Fail Send: %d:%s:%s:%s:%d",__FUNCTION__, dwUserIndex, szBillingGUID.c_str(), szPrivateID.c_str(), szChargeNo.c_str(), GetLastError()  );
		g_LogDBClient.OnInsertBillingServerError( CNT_WEMADEBUY, BILLING_ERROR_LOG_FAIL_SEND_WEMADE_BILLINGSERVER,"fail send billingserver." );
		return;
	}

	//LOG.PrintTimeAndLog( 0, "%s Send: %d:%s:%s:%s:[%s]",__FUNCTION__, dwUserIndex, szBillingGUID.c_str(), szPrivateID.c_str(), szChargeNo.c_str(), szUserIP.c_str() );

}

void ioChannelingNodeWemadeBuy::OnFillCashUrl( ServerNode *pServerNode, SP2Packet &rkPacket )
{
	int          iChannelingType   = 0;
	ioHashString szBillingGUID;
	DWORD        dwUserIndex       = 0;
	ioHashString szPrivateID;
	ioHashString szUserIP;


	rkPacket >> iChannelingType >> szBillingGUID >> dwUserIndex; // 공통사항
	rkPacket >> szPrivateID >> szUserIP; // wemade buy

	ioData kData;
	kData.SetChannelingType( iChannelingType );
	kData.SetBillingGUID( szBillingGUID );
	kData.SetUserIndex( dwUserIndex );
	kData.SetPrivateID( szPrivateID );
	kData.SetUserIP( szUserIP );
	if( pServerNode )
	{
		kData.SetServerIP( pServerNode->GetIP() );
		kData.SetServerPort( pServerNode->GetClientPort() );
	}
	kData.SetEmpty( false );
	kData.SetProcessType( ioData::PT_FILL_CASH_URL );

	g_ThreadPool.SetData( kData );

	//LOG.PrintTimeAndLog( 0, "%s Set :%s:%s:%d", __FUNCTION__, szBillingGUID.c_str(), szPrivateID.c_str(), dwUserIndex );
}

void ioChannelingNodeWemadeBuy::ThreadFillCashUrl( const ioData &rData )
{
	if( rData.IsEmpty() )
	{
		LOG.PrintTimeAndLog( 0, "[error][wemade]%s Data is Empty.", __FUNCTION__ );
		return;
	}

	if( g_App.IsReserveLogOut() )
	{
		LOG.PrintTimeAndLog( 0, "%s LogOut: %d:%s:%s",__FUNCTION__, rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str() );
		SP2Packet kPacket( BSTPK_FILL_CASH_URL_RESULT );
		kPacket << rData.GetUserIndex();
		kPacket << rData.GetBillingGUID();
		kPacket << CASH_RESULT_EXCEPT;
		
		if ( !g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket ) )
		{
			LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:%s", "ioChannelingNodeWemadeBuy::ThreadFillCashUrl", rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str() );
		}
		
		return;
	}

	char szReturnValue[WEB_BUFF_SIZE]="";
	ZeroMemory( szReturnValue, WEB_BUFF_SIZE );

	char szFullURL[MAX_PATH*2]="";
	StringCbPrintf( szFullURL, sizeof( szFullURL ), "%s" , m_sURL.c_str() );

	enum { MAKE_CODE_NO = 100103, GMT_CODE_NO = 201125 };

	char szPostData[MAX_PATH]="";
	StringCbPrintf( szPostData, sizeof( szPostData ), "u=%s&r=%s&m=%d&g=%d", rData.GetPrivateID().c_str(), rData.GetUserIP().c_str(), MAKE_CODE_NO, GMT_CODE_NO );

	ioHTTP Winhttp; //kyg 확인필요 
	if( !Winhttp.GetResultData( szFullURL, szPostData, szReturnValue, WEB_BUFF_SIZE ) )
	{
		LOG.PrintTimeAndLog( 0, "%s Http Error: %d:%s:%s:%d", __FUNCTION__, rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), GetLastError() );
		SP2Packet kPacket( BSTPK_FILL_CASH_URL_RESULT );
		kPacket << rData.GetUserIndex();
		kPacket << rData.GetBillingGUID();
		kPacket << CASH_RESULT_EXCEPT;
		
		if ( !g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket ) )
		{
			LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:%s", "ioChannelingNodeWemadeBuy::ThreadFillCashUrl", rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str() );
		}
		g_LogDBClient.OnInsertBillingServerError( CNT_WEMADEBUY, BILLING_ERROR_LOG_EXCEPTION,"Billing exception." );
		return;
	}

	ioXMLDocument xmlDoc;
	if( !xmlDoc.LoadFromMemory( szReturnValue ) )
	{
		SP2Packet kPacket( BSTPK_FILL_CASH_URL_RESULT );
		kPacket << rData.GetUserIndex();
		kPacket << rData.GetBillingGUID();
		kPacket << CASH_RESULT_EXCEPT;
		
		if ( !g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket ) )
		{
			LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:%s", "ioChannelingNodeWemadeBuy::ThreadFillCashUrl", rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str() );
		}
		LOG.PrintTimeAndLog(0, "%s Fail ioXMLDocument::LoadFromMemory :%d:%s:%s:%d", __FUNCTION__, rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), GetLastError() );
		LOG.PrintTimeAndLog(0, "%s Fail ReturnValue :%s", __FUNCTION__, szReturnValue );
		g_LogDBClient.OnInsertBillingServerError( CNT_WEMADEBUY, BILLING_ERROR_LOG_EXCEPTION,"Billing exception." );
		return;
	}

	ioXMLElement xRootElement = xmlDoc.GetRootElement();
	if( xRootElement.IsEmpty() )	
	{
		SP2Packet kPacket( BSTPK_FILL_CASH_URL_RESULT );
		kPacket << rData.GetUserIndex();
		kPacket << rData.GetBillingGUID();
		kPacket << CASH_RESULT_EXCEPT;
		
		if ( !g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket ) )
		{
			LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:%s", "ioChannelingNodeWemadeBuy::ThreadFillCashUrl", rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str() );
		}
		LOG.PrintTimeAndLog(0, "[error][wemade]%s ioXMLElement is Empty. :%d:%s:%s:%d", __FUNCTION__, rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), GetLastError() );
		LOG.PrintTimeAndLog(0, "%s Fail ReturnValue :%s", __FUNCTION__, szReturnValue );
		return;
	}

/*
<?xml version="1.0" encoding="euc-kr" ?>
<billingURLInfo>
	<getParamInfo>
		<u value="테스터1"/>
		<r value="111.111.111.111"/>
		<m value="100103"/>
		<g value="201125"/>
		<t value=""/>
	</getParamInfo>
	<getResultInfo>
		<billingReturn value="1"/>
		<fullURL value="http://billlosatest.wemade.com/login/mem_paylogin.asp?  u=Bmf5X73ZO297VA3Lpf7Z3Upf&amp;r=FhoEWYRvF2XeVRnAuIEOX4Ze&amp;t=9vsCWsNpfJIOQx6DIqHIIS3E&amp;f=G&amp;m=LWyLWh9glBQpRvlKZC11vHH8&amp;g=2ByMfgIRQ8mA3v49xwGg_UPN"/>
		<baseURL value="http://billlosatest.wemade.com/login/mem_paylogin.asp"/>
		<u value="Bmf5X73ZO297VA3Lpf7Z3Upf"/>
		<r value="FhoEWYRvF2XeVRnAuIEOX4Ze|||tkdwja2dkaghzl"/>
		<t value="9vsCWsNpfJIOQx6DIqHIIS3E"/>
		<f value="G"/>
		<m value="LWyLWh9glBQpRvlKZC11vHH8"/>
		<g value="2ByMfgIRQ8mA3v49xwGg_UPN"/>
		<MakeCodeNo value="100103"/>
	</getResultInfo>
<hostName value="219.240.38.244"/>
</billingURLInfo>
*/

	// xml parsing
	ioXMLElement xChildElement = xRootElement.FirstChild( "getResultInfo" );
	ioXMLElement xGrandChildElement;
	if( !xChildElement.IsEmpty() )
		xGrandChildElement = xChildElement.FirstChild( "fullURL" );

	ioHashString szFillCashURL;
	if( !xGrandChildElement.IsEmpty() )
		szFillCashURL = xGrandChildElement.GetStringAttribute( "value" );

	SP2Packet kPacket( BSTPK_FILL_CASH_URL_RESULT );
	kPacket << rData.GetUserIndex();
	kPacket << rData.GetBillingGUID();
	kPacket << CASH_RESULT_SUCCESS;
	kPacket << szFillCashURL;
	if ( !g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket ) )
	{
		LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:%s", "ioChannelingNodeWemadeBuy::ThreadFillCashUrl", rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str() );
		return;
	}

	//LOG.PrintTimeAndLog( 0, "%s Success: %d:%s:PrivateID %s:%s", __FUNCTION__, rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), szFillCashURL.c_str() );
	//LOG.PrintTimeAndLog(0, "%s Success ReturnValue :%s", __FUNCTION__, szReturnValue );
}

void ioChannelingNodeWemadeBuy::_OnSubscriptionRetractCheck( ServerNode* pServerNode, SP2Packet &rkPacket )
{
	int          iChannelingType = 0;
	ioHashString szBillingGUID;
	DWORD        dwUserIndex = 0;
	ioHashString szPublicID;
	ioHashString szPrivateID;
	ioHashString szChargeNo;
	DWORD		 dwIndex = 0;
	ioHashString szUserIP;

	rkPacket >> iChannelingType;
	rkPacket >> szBillingGUID;
	rkPacket >> dwUserIndex;
	rkPacket >> szPublicID;
	rkPacket >> szPrivateID;
 
	//위에까지 공통인자 
	rkPacket >> dwIndex;
	rkPacket >> szChargeNo;
	rkPacket >> szUserIP;

	BillInfoManager::BillingInfo *pInfo = g_BillInfoManager->PopBillInfo();
	
	if( !pInfo )
	{
		LOG.PrintTimeAndLog( 0, "%s pInfo == NULL: %d:%s:%s:%s:%d",__FUNCTION__, dwUserIndex, szBillingGUID.c_str(), szPrivateID.c_str(), szPublicID.c_str(), GetLastError()  );

		SendSubscriptionRetractErrorPacket( pServerNode,BSTPK_SUBSCRIPTION_RETRACT_CASH_CHECK_RESULT,dwUserIndex,szBillingGUID );

		g_LogDBClient.OnInsertBillingServerError( CNT_WEMADEBUY, BILLING_ERROR_LOG_FAIL_SEND_WEMADE_BILLINGSERVER,"fail SubscriptionRetract." );
		return;
	}

	//hr//
	DWORD reqKey = InterlockedIncrement((LONG *)&m_iReqKey);


	pInfo->m_szKey        = szPrivateID;
	pInfo->m_eType        = BillInfoManager::AT_SUBSCRIPTIONRETRACT;
	pInfo->m_dwCreateTime = TIMEGETTIME();
	pInfo->m_dwReqKey	  = reqKey;

	pInfo->m_kPacket << dwUserIndex << szBillingGUID << pServerNode->GetIP() << pServerNode->GetClientPort() << dwIndex << szChargeNo; //kyg ChargeNo으로 인증할건지 안할건지 정해야함
	pInfo->m_kPacket.SetPosBegin();
	if( !g_BillInfoManager->Add( pInfo ) )
	{
		LOG.PrintTimeAndLog( 0, "%s BillingInfo is full.: %d:%s:%s:%s:%d",__FUNCTION__, dwUserIndex, szBillingGUID.c_str(), szPrivateID.c_str(), szPublicID.c_str(), GetLastError()  );

		SendSubscriptionRetractErrorPacket (pServerNode,BSTPK_SUBSCRIPTION_RETRACT_CASH_CHECK_RESULT,dwUserIndex,szBillingGUID );

		g_LogDBClient.OnInsertBillingServerError( CNT_WEMADEBUY, BILLING_ERROR_LOG_FAIL_SEND_WEMADE_BILLINGSERVER,"fail SubscriptionRetract." );
		return;
	}
	
	CS_RETRACT_PAYBACK sendRetract;
	sendRetract.SetInfo( reqKey, dwUserIndex,szPrivateID.c_str(),szUserIP.c_str(),szChargeNo.c_str());

	SP2Packet kPacket( BTWBTPK_SUBSCRIPTION_RETRACT_CHECK );
	kPacket << sendRetract;
	if( !g_WemadeBuyServer.SendMessage( kPacket ) )
	{ 
		SendSubscriptionRetractErrorPacket(pServerNode,BSTPK_SUBSCRIPTION_RETRACT_CASH_CHECK_RESULT,dwUserIndex,szBillingGUID);
	}
}

void ioChannelingNodeWemadeBuy::_OnSubscriptionRetract( ServerNode* pServerNode, SP2Packet& rkPacket )
{
	int          iChannelingType = 0;
	ioHashString szBillingGUID;
	DWORD        dwUserIndex = 0;
	ioHashString szPublicID;
	ioHashString szPrivateID;
	DWORD		 dwIndex = 0;
	ioHashString szChargeNo;
	ioHashString szUserIP;

	rkPacket >> iChannelingType;
	rkPacket >> szBillingGUID;
	rkPacket >> dwUserIndex;
	rkPacket >> szPublicID;
	rkPacket >> szPrivateID;
	rkPacket >> dwIndex;
	//위에까지 공통인자 
	rkPacket >> szChargeNo;
	rkPacket >> szUserIP;
	BillInfoManager::BillingInfo *pInfo = g_BillInfoManager->PopBillInfo();
	
	if( !pInfo )
	{
		LOG.PrintTimeAndLog( 0, "%s pInfo == NULL: %d:%s:%s:%s:%d",__FUNCTION__, dwUserIndex, szBillingGUID.c_str(), szPrivateID.c_str(), szPublicID.c_str(), GetLastError()  );

		SendSubscriptionRetractErrorPacket(pServerNode,BSTPK_SUBSCRIPTION_RETRACT_CASH_CHECK_RESULT,dwUserIndex,szBillingGUID);

		g_LogDBClient.OnInsertBillingServerError( CNT_WEMADEBUY, BILLING_ERROR_LOG_FAIL_SEND_WEMADE_BILLINGSERVER,"fail SubscriptionRetract." );
		return;
	}

	//hr//
	DWORD reqKey = InterlockedIncrement((LONG *)&m_iReqKey);

	pInfo->m_szKey        = szPrivateID;
	pInfo->m_eType        = BillInfoManager::AT_SUBSCRIPTIONRETRACT;
	pInfo->m_dwCreateTime = TIMEGETTIME();
	pInfo->m_dwReqKey	  = reqKey;

	pInfo->m_kPacket << dwUserIndex << szBillingGUID << pServerNode->GetIP() << pServerNode->GetClientPort() << dwIndex << szChargeNo; //kyg chargeno으로 인증할건지 안할건지 정해야함 
	pInfo->m_kPacket.SetPosBegin();

	if( !g_BillInfoManager->Add( pInfo ) )
	{
		LOG.PrintTimeAndLog( 0, "%s BillingInfo is full.: %d:%s:%s:%s:%d",__FUNCTION__, dwUserIndex, szBillingGUID.c_str(), szPrivateID.c_str(), szPublicID.c_str(), GetLastError()  );

		SendSubscriptionRetractErrorPacket(pServerNode,BSTPK_SUBSCRIPTION_RETRACT_CASH_CHECK_RESULT,dwUserIndex,szBillingGUID);

		g_LogDBClient.OnInsertBillingServerError( CNT_WEMADEBUY, BILLING_ERROR_LOG_FAIL_SEND_WEMADE_BILLINGSERVER,"fail SubscriptionRetract." );
		return;
	}

	

	CS_RETRACT sendRetract;
	sendRetract.SetInfo( reqKey, dwUserIndex,szPrivateID.c_str(),szUserIP.c_str(),szChargeNo.c_str() );

	SP2Packet kPacket( BTWBTPK_SUBSCRIPTION_RETRACT);

	kPacket << sendRetract;

	if( !g_WemadeBuyServer.SendMessage( kPacket ) )
	{
		SendSubscriptionRetractErrorPacket(pServerNode,BSTPK_SUBSCRIPTION_RETRACT_CASH_RESULT,dwUserIndex,szBillingGUID);
	}
}

void ioChannelingNodeWemadeBuy::_OnSubscriptionRetractFail( ServerNode* pServerNode, SP2Packet& rkPacket )
{

}

void ioChannelingNodeWemadeBuy::OnReceiveRetractCash( SC_RETRACT& rkResult )
{
	BillInfoManager::BillingInfo *pInfo = g_BillInfoManager->Get( rkResult.UserID );
	if( !pInfo )
	{
		LOG.PrintTimeAndLog( 0, "%s pInfo == NULL. %d:%s:%d:%d:%d:%s[%s]", __FUNCTION__, rkResult.UserNo, rkResult.UserID, rkResult.RealCash, rkResult.BonusCash, rkResult.ResultCode, rkResult.ResultMessage, rkResult.ChargeNo );
		return;
	}

	DWORD		 dwUserIndex = 0;
	ioHashString szBillingGUID;
	ioHashString szServerIP;
	int          iClientPort   = 0;
	DWORD		 dwIndex = 0;
	ioHashString szChargeNo;

	pInfo->m_kPacket >> dwUserIndex >>  szBillingGUID >> szServerIP >> iClientPort >> dwIndex >> szChargeNo;

	if(szChargeNo != rkResult.ChargeNo)
	{
		 //kyg 채워둘것
		rkResult.ResultCode = 1;
		sprintf_s( rkResult.ResultMessage, "Error ChargeNo is Not Same %s :: %s ", szChargeNo.c_str(), rkResult.ChargeNo );
	}
 
	if( rkResult.ResultCode != RESULT_CODE_SUCCESS )
	{
		ioHashString sResultMessage = rkResult.ResultMessage;

		SP2Packet kPacket( BSTPK_SUBSCRIPTION_RETRACT_CASH_RESULT );

		kPacket << rkResult.UserNo;
		kPacket << szBillingGUID;
		kPacket << dwIndex;
		kPacket << szChargeNo;
		kPacket << BILLING_SUBSCRIPTION_RETRACT_RESULT_FAIL;
		kPacket << sResultMessage;

		if( !g_ServerNodeManager.SendMessageIP( szServerIP, iClientPort, kPacket ) )
		{
			g_BillInfoManager->Delete( rkResult.UserID );

			LOG.PrintTimeAndLog( 0, "[warning][wemade]%s Send Fail: %d:%s:%s:Ret %d:%d:%d[%s]", __FUNCTION__, rkResult.UserNo, szBillingGUID.c_str(), rkResult.UserID,  rkResult.ResultCode, rkResult.RealCash, rkResult.BonusCash, rkResult.ChargeNo );
			g_LogDBClient.OnInsertBillingServerError( CNT_WEMADEBUY, BILLING_ERROR_LOG_DISCONNECT_GAMESERVER,"disconnect game server." );
			return;
		}

		g_BillInfoManager->Delete( rkResult.UserID );

		LOG.PrintTimeAndLog( 0, "%s Fail: %d:%s:%s:Ret %d:%s:%d:%d[%s]", __FUNCTION__, rkResult.UserNo, szBillingGUID.c_str(), rkResult.UserID,  rkResult.ResultCode, rkResult.ResultMessage, rkResult.RealCash, rkResult.BonusCash, rkResult.ChargeNo );
		g_LogDBClient.OnInsertBillingServerError( CNT_WEMADEBUY, rkResult.ResultCode,"RetractCheckCash::Error return value." );
		return;
	}

	SP2Packet kPacket( BSTPK_SUBSCRIPTION_RETRACT_CASH_RESULT );

	kPacket << rkResult.UserNo;
	kPacket << szBillingGUID;
	kPacket << dwIndex;
	kPacket << szChargeNo;
	kPacket << BILLING_SUBSCRIPTION_RETRACT_RESULT_SUCCESS;
	kPacket << rkResult.CanceledCashAmt;
	kPacket << rkResult.RealCash;
	kPacket << rkResult.BonusCash;
	
	if( !g_ServerNodeManager.SendMessageIP( szServerIP, iClientPort, kPacket ) )
	{
		LOG.PrintTimeAndLog( 0, "[warning][wemade]%s Send Fail: %d:%s:%s:Ret %d:%d:%d[%s]", __FUNCTION__, rkResult.UserNo, szBillingGUID.c_str(), rkResult.UserID,  rkResult.ResultCode, rkResult.RealCash, rkResult.BonusCash, rkResult.ChargeNo );
		g_LogDBClient.OnInsertBillingServerError( CNT_WEMADEBUY, BILLING_ERROR_LOG_DISCONNECT_GAMESERVER,"disconnect game server." );
		g_BillInfoManager->Delete( rkResult.UserID  );
		return;
	}
	g_BillInfoManager->Delete( rkResult.UserID );
	LOG.PrintTimeAndLog( 0, "%s Success: %d:%s:%s:Ret %d:%d:%d:%d[%s]", __FUNCTION__, rkResult.UserNo, szBillingGUID.c_str(), rkResult.UserID,  rkResult.ResultCode, rkResult.RealCash, rkResult.BonusCash, rkResult.CanceledCashAmt, rkResult.ChargeNo );
}

void ioChannelingNodeWemadeBuy::OnReceiveRetractCheckCash( SC_RETRACT_PAYBACK& rkResult )
{
	BillInfoManager::BillingInfo *pInfo = g_BillInfoManager->Get( rkResult.UserID );

	if( !pInfo )
	{
		LOG.PrintTimeAndLog( 0, "%s pInfo == NULL. %d:%s:%d:%d:%d:%s[%s]", __FUNCTION__, rkResult.UserNo, rkResult.UserID, rkResult.RealCash, rkResult.BonusCash, rkResult.ResultCode, rkResult.ResultMessage, rkResult.ChargeNo );
		return;
	}

	DWORD		 dwUserIndex = 0;
	ioHashString szBillingGUID;
	int          iChannelingType = 0;
	ioHashString szServerIP;
	int          iClientPort   = 0;
	ioHashString szUserIP;
	DWORD		 dwIndex = 0;
	ioHashString szChargeNo;
	
	pInfo->m_kPacket >> dwUserIndex >>  szBillingGUID >>  szServerIP >> iClientPort >> dwIndex >> szChargeNo;//kyg iChannelingType

	if(szChargeNo != rkResult.ChargeNo)
	{
		rkResult.ResultCode = 1;
		sprintf_s( rkResult.ResultMessage, "Error ChargeNo is Not Same %s :: %s ", szChargeNo.c_str(), rkResult.ChargeNo );
	}

	if( rkResult.ResultCode != RESULT_CODE_SUCCESS )
	{
		ioHashString sResultMessage = rkResult.ResultMessage;

		SP2Packet kPacket( BSTPK_SUBSCRIPTION_RETRACT_CASH_CHECK_RESULT );
		kPacket << rkResult.UserNo;
		kPacket << szBillingGUID;
		kPacket << dwIndex;
		kPacket << szChargeNo;
		kPacket << BILLING_SUBSCRIPTION_RETRACT_RESULT_FAIL;
		kPacket << sResultMessage;

		if( !g_ServerNodeManager.SendMessageIP( szServerIP, iClientPort, kPacket ) )
		{
			g_BillInfoManager->Delete( rkResult.UserID );
			LOG.PrintTimeAndLog( 0, "[warning][wemade]%s Send Fail: %d:%s:%s:Ret %d:%d:%d[%s]", __FUNCTION__, rkResult.UserNo, szBillingGUID.c_str(), rkResult.UserID,  rkResult.ResultCode, rkResult.RealCash, rkResult.BonusCash, rkResult.ChargeNo );
			g_LogDBClient.OnInsertBillingServerError( CNT_WEMADEBUY, BILLING_ERROR_LOG_DISCONNECT_GAMESERVER,"disconnect game server." );
			return;
		}
		
		g_BillInfoManager->Delete( rkResult.UserID );
		LOG.PrintTimeAndLog( 0, "%s Fail: %d:%s:%s:Ret %d:%s:%d:%d[%s]", __FUNCTION__, rkResult.UserNo, szBillingGUID.c_str(), rkResult.UserID,  rkResult.ResultCode, rkResult.ResultMessage, rkResult.RealCash, rkResult.BonusCash, rkResult.ChargeNo );
		g_LogDBClient.OnInsertBillingServerError( CNT_WEMADEBUY, rkResult.ResultCode,"RetractCheckCash::Error return value." );
		return;
	}
 
	SP2Packet kPacket( BSTPK_SUBSCRIPTION_RETRACT_CASH_CHECK_RESULT );

	kPacket << rkResult.UserNo;
	kPacket << szBillingGUID;
	kPacket << dwIndex;
	kPacket << szChargeNo;
	kPacket << BILLING_SUBSCRIPTION_RETRACT_RESULT_SUCCESS;
	kPacket << rkResult.ChargedAmt;
	kPacket << rkResult.RealChargedAmt;
	kPacket << rkResult.BonusChargedAmt;
	kPacket << rkResult.CanceledAmt;
	kPacket << rkResult.RealCash;
	kPacket << rkResult.BonusCash;
 
	if( !g_ServerNodeManager.SendMessageIP( szServerIP, iClientPort, kPacket ) )
	{
		LOG.PrintTimeAndLog( 0, "[warning][wemade]%s Send Fail: %d:%s:%s:Ret %d:%d:%d[%s]", __FUNCTION__, rkResult.UserNo, szBillingGUID.c_str(), rkResult.UserID,  rkResult.ResultCode, rkResult.RealCash, rkResult.BonusCash, rkResult.ChargeNo );
		g_LogDBClient.OnInsertBillingServerError( CNT_WEMADEBUY, BILLING_ERROR_LOG_DISCONNECT_GAMESERVER,"disconnect game server." );
		g_BillInfoManager->Delete( rkResult.UserID  );
		return;
	}
	
	g_BillInfoManager->Delete( rkResult.UserID );
	LOG.PrintTimeAndLog( 0, "%s Success: %d:%s:%s:Ret %d:%d:%d:%d[%s]", __FUNCTION__, rkResult.UserNo, szBillingGUID.c_str(), rkResult.UserID,  rkResult.ResultCode, rkResult.RealCash, rkResult.BonusCash, rkResult.ChargedAmt, rkResult.ChargeNo );
	LOG.PrintTimeAndLog(0, "%s Cancled Amt : %d",__FUNCTION__,rkResult.CanceledAmt);

}

void ioChannelingNodeWemadeBuy::OnReceiveRetractCancelCash( const SC_RETRACT_CANCEL& rkResult )
{

}
