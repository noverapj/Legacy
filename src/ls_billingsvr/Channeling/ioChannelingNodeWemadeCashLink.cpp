#include "../stdafx.h"
#include "./ioChannelingNodeWemadeCashLink.h"
#include "../NodeInfo/ServerNode.h"
#include "../xml/ioxmldocument.h"
#include "../xml/ioxmlelement.h"
#include "../Util/ioHashString.h"
#include "../ThreadPool/ioThreadPool.h"
#include "../NodeInfo/ServerNodeManager.h"
#include "../MainProcess.h"
#include "../database/logdbclient.h"
#include "../Local/ioLocalParent.h"
#include "../Util/cJSON.h"
#include <atltime.h>

extern CLog LOG;

ioChannelingNodeWemadeCashLink::ioChannelingNodeWemadeCashLink(void)
{
	char szTemp[MAX_PATH]="";
	ioINILoader kLoader( "ls_config_billingsvr.ini" );
	kLoader.SetTitle( "Channeling" );

	kLoader.LoadString( "WemadeGetURL", "", szTemp, sizeof( szTemp ) );
	m_sGetURL = szTemp;

	kLoader.LoadString( "WemadeOutputURL", "", szTemp, sizeof( szTemp ) );
	m_sOutputURL = szTemp;

	kLoader.LoadString( "WemadeSubsRetractURL", "", szTemp, sizeof( szTemp ) );
	m_sSubscriptionRetractURL = szTemp;

	kLoader.LoadString( "WemadePresentURL", "", szTemp, sizeof( szTemp ) );
	m_sPresentURL = szTemp;

	kLoader.LoadString( "WemadeSubsCheckURL", "", szTemp, sizeof( szTemp ) );
	m_sSubscriptionRetractCheckURL = szTemp;

	kLoader.SetTitle( "Channeling" );
	char WemadeFillURL[MAX_PATH]="";

	kLoader.LoadString( "WemadeFillURL", "", WemadeFillURL, sizeof( WemadeFillURL ) );
	m_sURL = WemadeFillURL;

	kLoader.LoadString( "WemadePresentCashURL", "", szTemp, sizeof( szTemp ) );
	m_sPresentCashURL = szTemp;

	m_dwMakeCode = kLoader.LoadInt( "WemadeMakeCode ",100103);
}

ioChannelingNodeWemadeCashLink::~ioChannelingNodeWemadeCashLink(void)
{
}

ChannelingType ioChannelingNodeWemadeCashLink::GetType()
{
	return CNT_WEMADEBUY;
}

void ioChannelingNodeWemadeCashLink::_OnGetCash( ServerNode *pServerNode, SP2Packet &rkPacket )
{
	int          iChannelingType   = 0;
	ioHashString szBillingGUID;
	DWORD        dwUserIndex       = 0;
	ioHashString szPrivateID;
	ioHashString szPublicID;
	bool         bSetUserMouse     = false;

	rkPacket >> iChannelingType >> szBillingGUID >> dwUserIndex >> szPrivateID >> szPublicID >> bSetUserMouse; // 공통사항

	ioData kData;
	kData.SetChannelingType( iChannelingType );
	kData.SetBillingGUID( szBillingGUID );
	kData.SetUserIndex( dwUserIndex );
	kData.SetPrivateID( szPrivateID );
	kData.SetPublicID( szPublicID );
	kData.SetSetUserMouse( bSetUserMouse );

	if( pServerNode )
	{
		kData.SetServerIP( pServerNode->GetIP() );
		kData.SetServerPort( pServerNode->GetClientPort() );
	}
	kData.SetEmpty( false );
	kData.SetProcessType( ioData::PT_GET_CASH );

	g_ThreadPool.SetData( kData );
}

void ioChannelingNodeWemadeCashLink::_OnPresentCash(ServerNode *pServerNode, SP2Packet &rkPacket )
{
	DWORD dwUserIndex	= 0;
	ioHashString szBillingGUID;
	int iEventID		= 0;
	int iCash			= 0;
	int iExpireDay		= 0;
	char szExpire[20] = {0,};
	char szMonth[5] = {0,};
	char szDay[5] = {0,};

	//ioHashString szExpire;
	ioHashString szUserIP;

	rkPacket >> dwUserIndex >> szBillingGUID >> iEventID >> iCash >> iExpireDay >> szUserIP;

	CTime cCurTime	= CTime::GetCurrentTime();
	CTime cExpireDate;
	CTimeSpan cGap(iExpireDay,0,0,0);

	cExpireDate = cCurTime + cGap;

	if( cExpireDate.GetMonth() < 10 )
		StringCbPrintf( szMonth, sizeof( szMonth ), "0%d", cExpireDate.GetMonth() );
	else if( cExpireDate.GetMonth() >= 10 )
		StringCbPrintf( szMonth, sizeof( szMonth ), "%d", cExpireDate.GetMonth() );

	if( cExpireDate.GetDay() < 10 )
		StringCbPrintf( szDay, sizeof( szDay ), "0%d", cExpireDate.GetDay() );
	else if( cExpireDate.GetDay() >= 10 )
		StringCbPrintf( szDay, sizeof( szDay ), "%d", cExpireDate.GetDay() );

	StringCbPrintf( szExpire, sizeof( szExpire ), "%d%s%s", cExpireDate.GetYear(), szMonth, szDay);

	ioData kData;
	kData.SetChannelingType( CNT_WEMADEBUY );
	kData.SetBillingGUID( szBillingGUID );
	kData.SetUserIndex( dwUserIndex );
	kData.SetUserIP( szUserIP );
	kData.SetUserKey(szExpire);
	kData.SetGold( iCash );
	kData.SetUserReqNum( iEventID );

	if( pServerNode )
	{
		kData.SetServerIP( pServerNode->GetIP() );
		kData.SetServerPort( pServerNode->GetClientPort() );
	}

	kData.SetEmpty( false );
	kData.SetProcessType( ioData::PT_PRESENT_CASH );

	g_ThreadPool.SetData( kData );
}

void ioChannelingNodeWemadeCashLink::_OnOutputCash( ServerNode *pServerNode, SP2Packet &rkPacket, DWORD dwGoodsNo, const ioHashString &rszGoodsName )
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

	ioData kData;

	rkPacket >> iChannelingType >> szBillingGUID >> iReturnItemPrice >> dwUserIndex >> szPublicID >> szPrivateID >> szUserIP >> iPayAmt >> iBonusCashSize;

	for( int i = 0; i < iBonusCashSize; i++ )
	{
		int iIndex		= 0;
		int iBonusCash	= 0;

		rkPacket >> iIndex >> iBonusCash;

		kData.AddBonusCashInfoForConsume(iIndex, iBonusCash);
	}

	rkPacket >> iType; // 공통사항	

	int iItemValueList[MAX_ITEM_VALUE];
	for (int i = 0; i < MAX_ITEM_VALUE ; i++)
		iItemValueList[i] = 0;
	GetItemValueList( rkPacket, iType, iItemValueList );

	int          iGameServerPort = 0;
	ioHashString szReceivePrivateID; // 선물인 경우만 셋팅됨
	ioHashString szReceivePublicID;  // 선물인 경우만 셋팅됨
	DWORD dwRecvUserIndex = 0;

	rkPacket >> iGameServerPort >> szReceivePrivateID >> szReceivePublicID >> dwRecvUserIndex; // wemadebuy

	kData.SetChannelingType( iChannelingType );
	kData.SetBillingGUID( szBillingGUID );
	kData.SetExp( iReturnItemPrice );
	kData.SetUserIndex( dwUserIndex );
	kData.SetPublicID( szPublicID );
	kData.SetPrivateID( szPrivateID );
	kData.SetUserIP( szUserIP );
	kData.SetItemPayAmt( iPayAmt );
	kData.SetItemType( iType );
	kData.SetItemValueList( iItemValueList );
	kData.SetGoodsNo( dwGoodsNo );
	kData.SetGoodsName( rszGoodsName );
	kData.SetGameServerPort(iGameServerPort);
	kData.SetReceivePrivateID(szReceivePrivateID);
	kData.SetReceivePublicID(szReceivePublicID);

	if( pServerNode )
	{
		kData.SetServerIP( pServerNode->GetIP() );
		kData.SetServerPort( pServerNode->GetClientPort() );
	}
	kData.SetEmpty( false );
	kData.SetProcessType( ioData::PT_OUTPUT_CASH );

	g_ThreadPool.SetData( kData );
}

void ioChannelingNodeWemadeCashLink::_OnSubscriptionRetract( ServerNode* pServerNode, SP2Packet& rkPacket )
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
	rkPacket >> szChargeNo;
	rkPacket >> szUserIP;

	ioData kData;

	kData.SetChannelingType( iChannelingType );
	kData.SetBillingGUID( szBillingGUID );
	kData.SetUserIndex( dwUserIndex );
	kData.SetPublicID( szPublicID );
	kData.SetPrivateID( szPrivateID );
	kData.SetIndex( dwIndex );
	kData.SetChargeNo(szChargeNo);
	kData.SetUserIP(szUserIP);

	if( pServerNode )
	{
		kData.SetServerIP( pServerNode->GetIP() );
		kData.SetServerPort( pServerNode->GetClientPort() );
	}
	kData.SetEmpty( false );
	kData.SetProcessType( ioData::PT_SUBSCRIPTION_RETRACT );

	g_ThreadPool.SetData( kData );
}

void ioChannelingNodeWemadeCashLink::_OnSubscriptionRetractCheck( ServerNode* pServerNode, SP2Packet &rkPacket )
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

	//for wemade
	rkPacket >> szChargeNo;
	rkPacket >> szUserIP;


	ioData kData;

	kData.SetChannelingType( iChannelingType );
	kData.SetBillingGUID( szBillingGUID );
	kData.SetUserIndex( dwUserIndex );
	kData.SetPublicID( szPublicID );
	kData.SetPrivateID( szPrivateID );
	kData.SetIndex( dwIndex );
	kData.SetChargeNo(szChargeNo);
	kData.SetUserIP(szUserIP);

	if( pServerNode )
	{
		kData.SetServerIP( pServerNode->GetIP() );
		kData.SetServerPort( pServerNode->GetClientPort() );
	}
	kData.SetEmpty( false );
	kData.SetProcessType( ioData::PT_SUBSCRIPTION_RETRACT_CHECK );

	g_ThreadPool.SetData( kData );
}

void ioChannelingNodeWemadeCashLink::ThreadGetCash( const ioData &rData )
{
	if( rData.IsEmpty() )
	{
		LOG.PrintTimeAndLog( 0, "[error][wemade]%s Data is Empty.", __FUNCTION__ );
		return;
	}

	if( g_App.IsReserveLogOut() )
	{
		LOG.PrintTimeAndLog( 0, "%s LogOut: %d:%s:%s:%s:%s",__FUNCTION__, 
			rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetPublicID().c_str(), rData.GetUserNo().c_str() );

		SendExecptMessage(BSTPK_GET_CASH_RESULT, CASH_RESULT_EXCEPT, rData);

		return;
	}

	int iBonusCash    = 0;//보너스캐쉬
	int iRealCash   = 0; //현질캐쉬
	int iCash = 0; //전체캐쉬

	char szReturnValue[WEB_BUFF_SIZE]="";
	ZeroMemory( szReturnValue, WEB_BUFF_SIZE );

	char szFullURL[MAX_PATH*2]="";
	char szParam[MAX_PATH]="";
	char szErrString[MAX_PATH]="";

	GetCashURL(szFullURL, sizeof(szFullURL), szParam, sizeof(szParam), rData.GetUserIndex(), rData.GetPrivateID());

	DWORD dwBeforeTime = TIMEGETTIME();
	ioHTTP Winhttp; //kyg 확인 필요 
	if( !Winhttp.GetResultData( szFullURL, szParam, szReturnValue, WEB_BUFF_SIZE ) )
	{
		LOG.PrintTimeAndLog( 0, "%s Http Error: %d:%s:%s:%s:%s:%d", __FUNCTION__, 
			rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetPublicID().c_str(), rData.GetUserNo().c_str(), GetLastError() );
		
		SendExecptMessage(BSTPK_GET_CASH_RESULT, CASH_RESULT_EXCEPT, rData);

		g_LogDBClient.OnInsertBillingServerError( CNT_WEMADEBUY, BILLING_ERROR_LOG_EXCEPTION,"Billing exception wemadeCashLink.(GetCash)" );
		return;
	}
	DWORD dwAfterTime = TIMEGETTIME();

	if( ParseGetCashReturnData(szReturnValue, sizeof(szReturnValue), iRealCash, iBonusCash, rData, szErrString, sizeof(szErrString)) == FALSE )
	{
		LOG.PrintTimeAndLog( 0, "%s Http Error: %d:%s:%s:%s:%s:%d errString:%s", __FUNCTION__, 
			rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetPublicID().c_str(), rData.GetUserNo().c_str(), GetLastError(),szErrString );

		SendExecptMessage(BSTPK_GET_CASH_RESULT, CASH_RESULT_EXCEPT, rData);

		g_LogDBClient.OnInsertBillingServerError( CNT_WEMADEBUY, BILLING_ERROR_LOG_EXCEPTION,"Billing exception wemadeCashLink.(GetCash)" );
	}
	else
	{

		iCash = iRealCash + iBonusCash;

		SP2Packet kPacket( BSTPK_GET_CASH_RESULT );

		kPacket << rData.GetUserIndex();
		kPacket << rData.GetBillingGUID();
		kPacket << rData.GetSetUserMouse();
		kPacket << CASH_RESULT_SUCCESS;
		kPacket << iCash;
		kPacket << iRealCash;

#if(_TEST)
//		g_App.DecrementCashProcessCount();

	//	if(g_App.GetCashProcessCount() == 0)
	//		LOG.PrintTimeAndLog(0,"Comple GetCash Test! ");
#endif

		if(!g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket ))
		{
			LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:%s", "ioChannelingNodeWemadeCashLink::ThreadGetCash", rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str() );
			return;		
		}
	
		LOG.PrintTimeAndLog( 0, "%s Success: %d:%s:PrivateID %s:%s:%s[%d:%d]", __FUNCTION__,
			rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetPublicID().c_str(), rData.GetUserNo().c_str(), iRealCash, iBonusCash  );
	}
}

void ioChannelingNodeWemadeCashLink::ThreadOutputCash( const ioData &rData )
{
	if( rData.IsEmpty() )
	{
		LOG.PrintTimeAndLog( 0, "[error][wemade]%s Data is Empty.", __FUNCTION__ );
		return;
	}

	if( g_App.IsReserveLogOut() )
	{
		LOG.PrintTimeAndLog( 0, "%s LogOut: %d:%s:%s:%s",__FUNCTION__, rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetPublicID().c_str() );

		SendExecptMessage(BSTPK_OUTPUT_CASH_RESULT, CASH_RESULT_EXCEPT, rData);

		return;
	}

	int iRealCash    = 0;// 현금을 주고 구매한 캐쉬
	int iBonusCash   = 0; 
	int iPayAmt      = 0;

	char szReturnValue[WEB_BUFF_SIZE]="";
 	char szFullURL[MAX_PATH*2]="";
	char szParam[MAX_PATH*2]="";
	char szErrString[MAX_PATH]="";

	bool bPresent = false;
	if( OUTPUT_CASH_PRESENT == rData.GetItemType() || 
		OUTPUT_CASH_POPUP == rData.GetItemType() )
		bPresent  = true;

	if(bPresent ==false)
		GetOutputCashURL(szFullURL, sizeof(szFullURL), szParam, sizeof(szParam), rData);
	else
		GetPresentURL(szFullURL, sizeof(szFullURL), szParam, sizeof(szParam), rData);


	//{
	//	SP2Packet kPacket( BSTPK_OUTPUT_CASH_RESULT );
	//	kPacket << rData.GetUserIndex();
	//	kPacket << rData.GetBillingGUID();
	//	kPacket << CASH_RESULT_SUCCESS;
	//	kPacket << rData.GetItemType();
	//	kPacket << rData.GetItemPayAmt();
	//	kPacket << 0; // TransactionID ( FOR US )
	//	kPacket << rData.GetChargeNo();

	//	int iItemValueList[MAX_ITEM_VALUE];
	//	for (int i = 0; i <MAX_ITEM_VALUE; i++)
	//		iItemValueList[i] = rData.GetItemValue( i );
	//	SetItemValueList( kPacket, rData.GetItemType(), iItemValueList );

	//	// Cancel Step 1
	//	kPacket << rData.GetChannelingType();  // 공통
	//	kPacket << 0;
	//	kPacket << 0;
	//	g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket );
	//	return;
	//}
	ioHTTP Winhttp; //kyg 확인 필요 
	if( !Winhttp.GetResultData( szFullURL, szParam, szReturnValue, WEB_BUFF_SIZE ) )
	{
		LOG.PrintTimeAndLog( 0, "%s Http Error: %d:%s:%s:%s:%s:%d", __FUNCTION__, 
			rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetPublicID().c_str(), rData.GetUserNo().c_str(), GetLastError() );

		SendExecptMessage(BSTPK_OUTPUT_CASH_RESULT, CASH_RESULT_EXCEPT, rData);

		g_LogDBClient.OnInsertBillingServerError( CNT_WEMADEBUY, BILLING_ERROR_LOG_EXCEPTION,"Billing exception wemadeCashLink.(OutPutCash)" );
		return;
	}

	if(bPresent)
	{
		if( ParsePresentReturnData(szReturnValue, sizeof(szReturnValue), iRealCash, iBonusCash, iPayAmt, (ioData&)rData, szErrString, sizeof(szErrString)) == FALSE )
		{
			LOG.PrintTimeAndLog( 0, "%s Http Error: %d:%s:%s:%s:%s:%d errString:%s", __FUNCTION__, rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetPublicID().c_str(), rData.GetUserNo().c_str(), GetLastError(),szErrString );

			SendExecptMessage(BSTPK_OUTPUT_CASH_RESULT, CASH_RESULT_EXCEPT, rData);

			g_LogDBClient.OnInsertBillingServerError( CNT_WEMADEBUY, BILLING_ERROR_LOG_EXCEPTION,"Billing exception wemadeCashLink." );
			return;
		}
	}
	else
	{
		if( ParseOutPutCashReturnData(szReturnValue, sizeof(szReturnValue), iRealCash, iBonusCash, iPayAmt, (ioData&)rData, szErrString, sizeof(szErrString)) == FALSE )
		{
			LOG.PrintTimeAndLog( 0, "%s Http Error: %d:%s:%s:%s:%s:%d errString:%s", __FUNCTION__, rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetPublicID().c_str(), rData.GetUserNo().c_str(), GetLastError(),szErrString );

			SendExecptMessage(BSTPK_OUTPUT_CASH_RESULT, CASH_RESULT_EXCEPT, rData);

			g_LogDBClient.OnInsertBillingServerError( CNT_WEMADEBUY, BILLING_ERROR_LOG_EXCEPTION,"Billing exception wemadeCashLink." );
			return;
		}
	}

	SP2Packet kPacket( BSTPK_OUTPUT_CASH_RESULT );

	kPacket << rData.GetUserIndex();
	kPacket << rData.GetBillingGUID();
	kPacket << CASH_RESULT_SUCCESS;
	kPacket << rData.GetExp();
	kPacket << rData.GetItemType();
	kPacket << rData.GetItemPayAmt();
	kPacket << 0; // TransactionID ( FOR US )
	kPacket << rData.GetChargeNo();
	
	int iiItemValueList[MAX_ITEM_VALUE];
	for (int i = 0; i <MAX_ITEM_VALUE; i++)
		iiItemValueList[i] = rData.GetItemValue( i );
	SetItemValueList( kPacket, rData.GetItemType(), iiItemValueList );

	// Cancel Step 1
	kPacket << rData.GetChannelingType();  // 공통
	kPacket << iBonusCash + iRealCash;
	kPacket << iRealCash;

	static TwoOfINTVec vInfo;
	vInfo.clear();

	rData.GetBonusCashInfo(vInfo);
	int iiBonusCashSize	= vInfo.size();

	kPacket << iiBonusCashSize	;

	for( int i = 0; i < iiBonusCashSize	; i++ )
	{
		kPacket << vInfo[i].value1 << vInfo[i].value2;
	}

	kPacket << rData.GetPrivateID();
	kPacket << rData.GetUserIP();
	kPacket << rData.GetChargeNo();

	if(!g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket ))
	{
		LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:%s Ret %d:%s:%d", "ioChannelingNodeWemadeCashLink::ThreadOutputCash", rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetGoodsNo(), rData.GetGoodsName().c_str(), rData.GetItemPayAmt());
		return;	
	}

	LOG.PrintTimeAndLog(0, "%s Success. :%d:%s:PrivateID %s:%s:%s[%d:%s:%d:%d:%d:%s]", __FUNCTION__, rData.GetUserIndex(), rData.GetBillingGUID().c_str(),
		rData.GetPrivateID().c_str(), rData.GetPublicID().c_str(), rData.GetUserNo().c_str(), //PrivateID %s:%s:%s
		rData.GetGoodsNo(), rData.GetGoodsName().c_str(), rData.GetItemPayAmt(), iRealCash, iBonusCash, rData.GetChargeNo().c_str());
}

void ioChannelingNodeWemadeCashLink::ThreadSubscriptionRetract( const ioData& rData )
{
	if( rData.IsEmpty() )
	{
		LOG.PrintTimeAndLog( 0, "[error][wemade]%s Data is Empty.", __FUNCTION__ );
		return;
	}

	int iRealCash    = 0;// 현금을 주고 구매한 캐쉬
	int iBonusCash   = 0; 
	int iCancelCash      = 0;
	int iRealCancelCash = 0;
	int iBonusCancelCash = 0;

	char szReturnValue[WEB_BUFF_SIZE]="";
	char szFullURL[MAX_PATH*2]="";
	char szParam[MAX_PATH]="";
	char szErrString[MAX_PATH]="";

	GetSubsRetract(szFullURL, sizeof(szFullURL), szParam, sizeof(szParam), rData);

	ioHTTP Winhttp; //kyg 확인 필요 

	if( !Winhttp.GetResultData( szFullURL, szParam, szReturnValue, WEB_BUFF_SIZE ) )
	{
		SendExecptMessage(BSTPK_SUBSCRIPTION_RETRACT_CASH, BILLING_SUBSCRIPTION_RETRACT_RESULT_FAIL, rData);

		g_LogDBClient.OnInsertBillingServerError( CNT_WEMADEBUY, BILLING_SUBSCRIPTION_RETRACT_RESULT_FAIL,"RetractCheckCash::Error return value." );
		return;
	}

	if( ParseSubsRetract(szReturnValue, sizeof(szReturnValue), iRealCash, iBonusCash, iCancelCash, iRealCancelCash, iBonusCancelCash, (ioData&)rData, szErrString, sizeof(szErrString)) == FALSE )
	{
		LOG.PrintTimeAndLog( 0, "%s Http Error: %d:%s:%s:%s:%s:%d errString:%s", __FUNCTION__,
			rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetPublicID().c_str(), rData.GetUserNo().c_str(), GetLastError(), szErrString );

		SendExecptMessage(BSTPK_SUBSCRIPTION_RETRACT_CASH, BILLING_SUBSCRIPTION_RETRACT_RESULT_FAIL, rData);

		g_LogDBClient.OnInsertBillingServerError( CNT_WEMADEBUY, BILLING_SUBSCRIPTION_RETRACT_RESULT_FAIL,"RetractCheckCash::Error return value." );
		return;
	}

	SP2Packet kPacket( BSTPK_SUBSCRIPTION_RETRACT_CASH_RESULT);

	kPacket << rData.GetUserIndex();
	kPacket << rData.GetBillingGUID();
	kPacket << rData.GetIndex();
	kPacket << rData.GetChargeNo();
	kPacket << BILLING_SUBSCRIPTION_RETRACT_RESULT_SUCCESS;
	kPacket << iCancelCash;
	kPacket << iRealCash;
	kPacket << iBonusCash;
	
	if( !g_ServerNodeManager.SendMessageIP( (ioHashString)rData.GetServerIP(), rData.GetServerPort(), kPacket ) )
	{
		LOG.PrintTimeAndLog( 0, "%s Send fail: %d:%s:%s:Ret %d:%d:%d:%d[%s]", __FUNCTION__,
			rData.GetUserIndex(), 
			rData.GetPublicID().c_str(),
			rData.GetPrivateID().c_str(), 
			0,
			iRealCash, 
			iBonusCancelCash,
			iCancelCash,
			rData.GetChargeNo().c_str() );
		return;
	}
	else
	{
		LOG.PrintTimeAndLog( 0, "%s Success: %d:%s:%s:Ret %d:%d:%d:%d[%s]", __FUNCTION__,
			rData.GetUserIndex(), 
			rData.GetPublicID().c_str(),
			rData.GetPrivateID().c_str(),
			0, 
			iRealCash, 
			iBonusCancelCash,
			iCancelCash,
			rData.GetChargeNo().c_str() );
	}
}

void ioChannelingNodeWemadeCashLink::ThreadSubscriptionRetractCheck( const ioData &rData )
{
	if( rData.IsEmpty() )
	{
		LOG.PrintTimeAndLog( 0, "[error][wemade]%s Data is Empty.", __FUNCTION__ );
		return;
	}

	int iRealCash    = 0;// 현금을 주고 구매한 캐쉬
	int iBonusCash   = 0; 
	int iCancelCash      = 0;
	int iRealCancelCash = 0;
	int iBonusCancelCash = 0;

	char szReturnValue[WEB_BUFF_SIZE]="";
	char szFullURL[MAX_PATH*2]="";
	char szParam[MAX_PATH]="";
	char szErrString[MAX_PATH]="";

	GetSubsCheck(szFullURL, sizeof(szFullURL), szParam, sizeof(szParam), rData);

	ioHTTP Winhttp; //kyg 확인 필요 

	if( !Winhttp.GetResultData( szFullURL, szParam, szReturnValue, WEB_BUFF_SIZE ) )
	{
		SendExecptMessage(BSTPK_SUBSCRIPTION_RETRACT_CASH_CHECK_RESULT, BILLING_SUBSCRIPTION_RETRACT_RESULT_FAIL, rData);

		g_LogDBClient.OnInsertBillingServerError( CNT_WEMADEBUY, BILLING_SUBSCRIPTION_RETRACT_RESULT_FAIL,"RetractCheckCash::Error return value." );
		return;
	}

	if( ParseSubsCheckReturnData(szReturnValue, sizeof(szReturnValue), iRealCash, iBonusCash, iCancelCash, iRealCancelCash, iBonusCancelCash, (ioData&)rData, szErrString, sizeof(szErrString)) == FALSE )
	{
		LOG.PrintTimeAndLog( 0, "%s Http Error: %d:%s:%s:%s:%s:%d errString:%s", __FUNCTION__, rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetPublicID().c_str(), rData.GetUserNo().c_str(), GetLastError(),szErrString );

		SendExecptMessage(BSTPK_SUBSCRIPTION_RETRACT_CASH_CHECK_RESULT, BILLING_SUBSCRIPTION_RETRACT_RESULT_FAIL, rData);

		g_LogDBClient.OnInsertBillingServerError( CNT_WEMADEBUY, BILLING_SUBSCRIPTION_RETRACT_RESULT_FAIL,"RetractCheckCash::Error return value." );
		return;
	}
	SP2Packet kPacket( BSTPK_SUBSCRIPTION_RETRACT_CASH_CHECK_RESULT );

	kPacket << rData.GetUserIndex();
	kPacket << rData.GetBillingGUID();
	kPacket << rData.GetIndex();
	kPacket << rData.GetChargeNo();
	kPacket << BILLING_SUBSCRIPTION_RETRACT_RESULT_SUCCESS;
	kPacket << (iBonusCancelCash + iRealCancelCash);
	kPacket << iRealCancelCash;
	kPacket << iBonusCancelCash;
	kPacket << iCancelCash;
	kPacket << iRealCash;
	kPacket << iBonusCash;

	if( !g_ServerNodeManager.SendMessageIP( (ioHashString)rData.GetServerIP(), rData.GetServerPort(), kPacket ) )
	{
		LOG.PrintTimeAndLog( 0, "%s fail: %d:%s:%s:Ret %d:%d:%d:%d[%s]", __FUNCTION__,
			rData.GetUserIndex(), 
			rData.GetPublicID().c_str(),
			rData.GetPrivateID().c_str(), 
			0,
			iRealCash, 
			iBonusCancelCash,
			iCancelCash,
			rData.GetChargeNo().c_str() );
		return;
	}

	LOG.PrintTimeAndLog( 0, "%s Success: %d:%s:%s:Ret %d:%d:%d:%d[%s]", __FUNCTION__,
																					rData.GetUserIndex(), 
																					rData.GetPublicID().c_str(),
																					rData.GetPrivateID().c_str(),
																					0, 
																					iRealCash, 
																					iBonusCancelCash,
																					iCancelCash,
																					rData.GetChargeNo().c_str() );

	LOG.PrintTimeAndLog(0, "%s Cancled Amt : %d",__FUNCTION__, iCancelCash);
}

void ioChannelingNodeWemadeCashLink::TestPresentCash()
{
	
	int iBonusCash    = 0;//보너스캐쉬
	int iRealCash   = 0; //현질캐쉬
	int iCash		= 0;

	char szReturnValue[WEB_BUFF_SIZE]="";
	char szFullURL[MAX_PATH*2]="";
	char szParam[MAX_PATH]="";
	char szErrString[MAX_PATH]="";

	ioData rData;
	rData.SetChannelingType( CNT_WEMADEBUY );
	rData.SetUserIndex( 42839 );
	rData.SetUserIP( "172.20.20.174" );
	rData.SetUserKey("20151130");
	rData.SetGold( 1000 );
	rData.SetUserReqNum( 7 );

	StringCbPrintf( szFullURL, sizeof( szFullURL ), "%s?accountidx=%d&eventid=%d&bonuscash=%d&enddate=%s&ip=%s" ,m_sPresentCashURL.c_str(), rData.GetUserIndex(),
		rData.GetUserReqNum(), rData.GetGold(), rData.GetUserKey().c_str(), rData.GetUserIP().c_str());

	ioHTTP Winhttp; //kyg 확인 필요 

	if( !Winhttp.GetResultData( szFullURL, "GET", szReturnValue, WEB_BUFF_SIZE ) )
	{
		return;
	}

	printf("%s \n", szFullURL);
	printf("%s \n", szReturnValue);
	ParseGetCashReturnData(szReturnValue, sizeof(szReturnValue), iRealCash, iBonusCash, rData, szErrString, sizeof(szErrString));
}

void ioChannelingNodeWemadeCashLink::ThreadPresentCash(const ioData &rData)
{
	if( rData.IsEmpty() )
	{
		LOG.PrintTimeAndLog( 0, "[error][wemade]%s Data is Empty.", __FUNCTION__ );
		return;
	}

	int iBonusCash    = 0;//보너스캐쉬
	int iRealCash   = 0; //현질캐쉬
	int iCash		= 0;

	char szReturnValue[WEB_BUFF_SIZE]="";
	char szFullURL[MAX_PATH*2]="";
	char szParam[MAX_PATH]="";
	char szErrString[MAX_PATH]="";

	StringCbPrintf( szFullURL, sizeof( szFullURL ), "%s?accountidx=%d&eventid=%d&bonuscash=%d&enddate=%s&ip=%s" ,m_sPresentCashURL.c_str(), rData.GetUserIndex(),
		rData.GetUserReqNum(), rData.GetGold(), rData.GetUserKey().c_str(), rData.GetUserIP().c_str());
	
	ioHTTP Winhttp; //kyg 확인 필요 

	if( !Winhttp.GetResultData( szFullURL, "GET", szReturnValue, WEB_BUFF_SIZE ) )
	{
		LOG.PrintTimeAndLog( 0, "%s Http Error: %d:%s:%s:%s:%s:%d", __FUNCTION__, rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetPublicID().c_str(), rData.GetUserNo().c_str(), GetLastError() );
		
		SP2Packet kPacket(BSTPK_REQUEST_TIME_CASH_RESULT);
		kPacket << rData.GetUserIndex();
		kPacket << rData.GetBillingGUID();
		kPacket<<BILLING_TIME_CASH_RESULT_FAIL;
		
		if(!g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket ))
		{
			LOG.PrintTimeAndLog( 0, "ProcessType:%d Send Fail: %d:%s:%s", "ioChannelingNodeWemadeCashLink::SendExecptMessage Fail", 
				rData.GetProcessType(),
				rData.GetUserIndex(),
				rData.GetBillingGUID().c_str(),
				rData.GetPrivateID().c_str() );
		}
		return;
	}

	if( ParsePresentCashReturnData(szReturnValue, sizeof(szReturnValue), iRealCash, iBonusCash, rData, szErrString, sizeof(szErrString)) == FALSE )
	{
		LOG.PrintTimeAndLog( 0, "%s Http Error: %d:%s:%s:%s:%s:%d errString:%s", __FUNCTION__, 
			rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetPublicID().c_str(), rData.GetUserNo().c_str(), GetLastError(),szErrString );

		SP2Packet kPacket(BSTPK_REQUEST_TIME_CASH_RESULT);
		kPacket << rData.GetUserIndex();
		kPacket << rData.GetBillingGUID();
		kPacket<<BILLING_TIME_CASH_RESULT_FAIL;
		
		if(!g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket ))
		{
			LOG.PrintTimeAndLog( 0, "ProcessType:%d Send Fail: %d:%s:%s", "ioChannelingNodeWemadeCashLink::SendExecptMessage Fail", 
				rData.GetProcessType(),
				rData.GetUserIndex(),
				rData.GetBillingGUID().c_str(),
				rData.GetPrivateID().c_str() );
		}
		return;
	}
	else
	{
		iCash = iRealCash + iBonusCash;

		SP2Packet kPacket( BSTPK_REQUEST_TIME_CASH_RESULT );
		kPacket << rData.GetUserIndex();
		kPacket << rData.GetBillingGUID();
		kPacket << BILLING_TIME_CASH_RESULT_SUCCESS;
		kPacket << rData.GetGold();
		kPacket << iCash;
		kPacket << iRealCash;

		if(!g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket ))
		{
			LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:%s", "ioChannelingNodeWemadeCashLink::ThreadGetCash", rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str() );
			return;		
		}
	}
}

BOOL ioChannelingNodeWemadeCashLink::ParsePresentCashReturnData(char* szReturnData, const int iSize, int& iRealCash, int& iBonusCash, const ioData& rData, char* errString, int iErrSize)
{
	try 
	{
		cJSON *pJson = NULL;
		pJson=cJSON_Parse(szReturnData);

		if( pJson == NULL )
			return FALSE;

		BOOL bState =FALSE;

		for (BYTE i = 0; i < cJSON_GetArraySize(pJson); i++)
		{
			cJSON *pObject = cJSON_GetArrayItem( pJson, i);

			if( pObject && pObject->string )
			{
				if( strcmp("result", pObject->string) == 0 )
				{
					if(pObject->valuestring)
					{
						if(strcmp(pObject->valuestring, "0") != 0)
						{
							bState = FALSE;
						}
						else
							bState = TRUE;
					}
				}
				else if( strcmp("real", pObject->string) == 0 )
				{
					 iRealCash = pObject->valueint;
				}
				else if( strcmp("bonus", pObject->string) == 0 )
				{
					iBonusCash = pObject->valueint;
				}
			}
		}

		cJSON_Delete(pJson);
		return bState;
	}
	catch ( ... )
	{
		LOG.PrintTimeAndLog( 0, "%s Json Crash %s", __FUNCTION__, szReturnData );
		return FALSE;
	}

	return FALSE;
}

void ioChannelingNodeWemadeCashLink::UTF8ToAnsi( IN const char *szUTF8, OUT char *szAnsi, OUT int &riReturnAnsiSize, IN int iAnsiBufferSize )
{
	if( szUTF8 == NULL )
		return;

	WCHAR szUni[MAX_PATH*2];
	ZeroMemory( szUni, sizeof(szUni) );

	int iUnisize = MultiByteToWideChar(CP_UTF8, 0, szUTF8, -1 , szUni, MAX_PATH*2 );
	riReturnAnsiSize = WideCharToMultiByte(CP_ACP, 0 , szUni, iUnisize, szAnsi, iAnsiBufferSize ,NULL ,NULL );
}

void ioChannelingNodeWemadeCashLink::SendExecptMessage( DWORD dwPacketID, int iErrCode, const ioData& rData )
{
	SP2Packet kPacket( dwPacketID );

	switch(dwPacketID)
	{
	case BSTPK_GET_CASH_RESULT:
		{
			kPacket << rData.GetUserIndex();
			kPacket << rData.GetBillingGUID();
			kPacket << rData.GetSetUserMouse();
			kPacket << iErrCode;
		}
		break;
	case BSTPK_OUTPUT_CASH_RESULT:
		{
			kPacket << rData.GetUserIndex();
			kPacket << rData.GetBillingGUID();
			kPacket << iErrCode;
			kPacket << rData.GetExp();
			kPacket << rData.GetItemType();
			kPacket << false;

			static TwoOfINTVec vInfo;
			vInfo.clear();

			rData.GetBonusCashInfo(vInfo);
			int iBonusCashSize	= vInfo.size();

			kPacket << iBonusCashSize;

			for( int i = 0; i < iBonusCashSize; i++ )
			{
				kPacket << vInfo[i].value1 ;
			}

			kPacket << true;
		}
		break;
	case BSTPK_SUBSCRIPTION_RETRACT_CASH_CHECK_RESULT:
		{
			kPacket << rData.GetUserIndex();
			kPacket << rData.GetBillingGUID();
			kPacket << (DWORD) 0;
			kPacket << rData.GetChargeNo();
			kPacket << iErrCode;
		}
		break;
	}

	

	if(!g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket ))
	{
		LOG.PrintTimeAndLog( 0, "ProcessType:%d Send Fail: %d:%s:%s", "ioChannelingNodeWemadeCashLink::SendExecptMessage Fail", 
			rData.GetProcessType(),
			rData.GetUserIndex(),
			rData.GetBillingGUID().c_str(),
			rData.GetPrivateID().c_str() );
	}
	return;
}

void ioChannelingNodeWemadeCashLink::GetCashURL( char* szFullURL, const int iUrlSize, char* szParam, const int iParamSize, const DWORD dwUserIndex, const ioHashString& szPrivateID )
{
	StringCbPrintf( szFullURL, iUrlSize, "%s", m_sGetURL.c_str() );

	//string multibyte -> UTF8 변경
	char szUTF8PrivateID[USER_ID_NUM] = "";
	MultiToUTF8(szPrivateID.c_str(), szUTF8PrivateID);

	StringCbPrintf( szParam, iParamSize, "makeCodeNo=%d&userNo=%lu&userId=%s", GetMakeCode(), dwUserIndex, szUTF8PrivateID);
}

BOOL ioChannelingNodeWemadeCashLink::ParseGetCashReturnData( char* szReturnData, const int iSize, int& iRealCash, int& iBonusCash, const ioData& rData, char* errString, int iErrSize)
{
	try 
	{
		cJSON *pJson = NULL;
		pJson=cJSON_Parse(szReturnData);

		if( pJson == NULL )
			return FALSE;

		BOOL bState =FALSE;

		for (BYTE i = 0; i < cJSON_GetArraySize(pJson); i++)
		{
			cJSON *pObject = cJSON_GetArrayItem( pJson, i);

			if( pObject && pObject->string )
			{
				if( strcmp("result", pObject->string) == 0 )
				{
					if(pObject->valuestring)
					{
						if(strcmp(pObject->valuestring, WEMADE_SUCCESS_STRING) != 0)
						{
							bState = FALSE;
						}
						else
							bState = TRUE;
					}
				}
				else if( strcmp("message", pObject->string) == 0 )
				{
					if(pObject->valuestring)
					{
						strcpy_s(errString, iErrSize, pObject->valuestring);
						
					}
				}
				else if( strcmp("userNo", pObject->string) == 0 )
				{
					if(pObject->type == cJSON_Number)
					{
						if(rData.GetUserIndex() != pObject->valueint)
						{
							bState = FALSE;
						}
					}
				}
// 				else if( strcmp("userId", pObject->string) == 0 )
// 				{
// 					if(pObject->valuestring)
// 					{
// 						
// 					}
// 				}
				else if( strcmp("realCash", pObject->string) == 0 )
				{
					 iRealCash = pObject->valueint;
				}
				else if( strcmp("bonusCash", pObject->string) == 0 )
				{
					iBonusCash = pObject->valueint;
				}
			}
		}

		cJSON_Delete(pJson);
		return bState;
	}
	catch ( ... )
	{
		LOG.PrintTimeAndLog( 0, "%s Json Crash %s", __FUNCTION__, szReturnData );
		return FALSE;
	}

	return FALSE;
}

void ioChannelingNodeWemadeCashLink::GetOutputCashURL( char* szFullURL, const int iUrlSize, char* szParam, const int iParamSize, const ioData& rData)
{
	StringCbPrintf( szFullURL, iUrlSize, "%s", m_sOutputURL.c_str() );

	//string multibyte -> UTF8 변경
	char szPrivateID[USER_ID_NUM] = "";
	char szPublicID[USER_ID_NUM] = "";
	MultiToUTF8(rData.GetPrivateID().c_str(), szPrivateID);
	MultiToUTF8(rData.GetPublicID().c_str(), szPublicID);
	/*
	StringCbPrintf( szParam, iParamSize,  "makeCodeNo=%d&userNo=%lu&userId=%s&charId=%s&"
										  "clientIp=%s&clientPort=%d&"
										  "itemInfos[0].itemId=%d&itemInfos[0].itemCnt=%d&itemInfos[0].itemUnitPrice=%d&"
										   "gameServerNo=%d&worldNo=%d&statProperty1=&statProperty2=&statProperty3=&location=GAME",
										   GetMakeCode(), rData.GetUserIndex(), szPrivateID, szPublicID,
										   rData.GetUserIP().c_str(), rData.GetGameServerPort(),
										   rData.GetGoodsNo(), 1, rData.GetItemPayAmt(),
										   rData.GetGameServerPort(), 0);
										   */
	StringCbPrintf( szParam, iParamSize,  "makeCodeNo=%d&userNo=%lu&userId=%s&charId=%s&"
										  "clientIp=%s&clientPort=%d&"
										  "itemId=%d&itemCnt=%d&itemUnitPrice=%d&"
										   "gameServerNo=%d&worldNo=%d&statProperty1=&statProperty2=&statProperty3=&location=GAME",
										   GetMakeCode(), rData.GetUserIndex(), szPrivateID, szPublicID,
										   rData.GetUserIP().c_str(), rData.GetGameServerPort(),
										   rData.GetGoodsNo(), 1, rData.GetItemPayAmt(),
										   rData.GetGameServerPort(), 0);
}

BOOL ioChannelingNodeWemadeCashLink::ParseOutPutCashReturnData( char* szReturnData, const int iSize, int& iReturnCash, int& iBonusCash, int& iChargedAmt, ioData& rData, char* errString, int iErrSize )
{
	try 
	{
		cJSON *pJson = NULL;
		pJson=cJSON_Parse(szReturnData);

		if( pJson == NULL )
			return FALSE;

		BOOL bState =FALSE;

		for (BYTE i = 0; i < cJSON_GetArraySize(pJson); i++)
		{
			cJSON *pObject = cJSON_GetArrayItem( pJson, i);

			if( pObject && pObject->string )
			{
				if( strcmp("result", pObject->string) == 0 )
				{
					if(pObject->valuestring)
					{
						if(strcmp(pObject->valuestring, WEMADE_SUCCESS_STRING) != 0)
						{
							bState = FALSE;
						}
						else
							bState = TRUE;
					}
				}
				else if( strcmp("message", pObject->string) == 0 )
				{
					if(pObject->valuestring)
					{
						strcpy_s(errString, iErrSize, pObject->valuestring);

					}
				}
				else if( strcmp("userNo", pObject->string) == 0 )
				{
					if(pObject->type == cJSON_Number)
					{
						if(rData.GetUserIndex() != pObject->valueint)
						{
							bState = FALSE;
						}
					}
				}
				else if( strcmp("realCash", pObject->string) == 0 )
				{
					iReturnCash = pObject->valueint;
				}
				else if( strcmp("bonusCash", pObject->string) == 0 )
				{
					iBonusCash = pObject->valueint;
				}
				else if( strcmp("chargedCashAmt", pObject->string) == 0 )
				{
					iChargedAmt = pObject->valueint;
				}
				else if( strcmp("itemInfos", pObject->string) == 0 )
				{
					for(int j =0; j< cJSON_GetArraySize(pObject); j++)
					{
						cJSON* pChild =  cJSON_GetArrayItem(pObject,j);

						if(pChild)
						{
							cJSON* pjChild = cJSON_GetObjectItem(pChild,"chargeNo");
							if(pjChild && pjChild->type == cJSON_String)
							{
								rData.SetChargeNo(pjChild->valuestring);

								break;
							}
						}
					}
				}
			}
		}

		cJSON_Delete(pJson);
		return bState;
	}
	catch ( ... )
	{
		LOG.PrintTimeAndLog( 0, "%s Json Crash %s", __FUNCTION__, szReturnData );
		return FALSE;
	}

	return FALSE;
}

void ioChannelingNodeWemadeCashLink::GetPresentURL( char* szFullURL, const int iUrlSize, char* szParam, const int iParamSize, const ioData& rData )
{
	StringCbPrintf( szFullURL, iUrlSize, "%s", m_sPresentURL.c_str() );

	//string multibyte -> UTF8 변경
	char szPrivateID[USER_ID_NUM] = "";
	char szPublicID[USER_ID_NUM] = "";
	MultiToUTF8(rData.GetPrivateID().c_str(), szPrivateID);
	MultiToUTF8(rData.GetPublicID().c_str(), szPublicID);

	char szRecvPrivateID[USER_ID_NUM] = "";
	char szRecvPublicID[USER_ID_NUM] = "";
	MultiToUTF8(rData.GetReceivePrivateID().c_str(), szRecvPrivateID);
	MultiToUTF8(rData.GetReceivePublicID().c_str(), szRecvPublicID);
	/*
	StringCbPrintf( szParam, iParamSize,  
		"makeCodeNo=%d&userNo=%lu&userId=%s&charId=%s&"
		"receiveUserNo=%d&receiveUserId=%s&receiveCharId=%s&"
		"clientIp=%s&clientPort=%d&"
		"itemInfos[0].itemId=%d&itemInfos[0].itemCnt=%d&itemInfos[0].itemUnitPrice=%d&"
		"gameServerNo=%d&worldNo=%d&statProperty1=&statProperty2=&statProperty3=&location=GAME",
		GetMakeCode(), rData.GetUserIndex(), szPrivateID, szPublicID,
		rData.GetRecvUserIndex(), szRecvPrivateID, szRecvPublicID,
		rData.GetUserIP().c_str(), rData.GetGameServerPort(),
		rData.GetGoodsNo(), 1, rData.GetItemPayAmt(),
		rData.GetGameServerPort(), 0);*/
	
	StringCbPrintf( szParam, iParamSize,  
		"makeCodeNo=%d&userNo=%lu&userId=%s&charId=%s&"
		"receiveUserNo=%d&receiveUserId=%s&receiveCharId=%s&"
		"clientIp=%s&clientPort=%d&"
		"itemId=%d&itemCnt=%d&itemUnitPrice=%d&"
		"gameServerNo=%d&worldNo=%d&statProperty1=&statProperty2=&statProperty3=&location=GAME",
		GetMakeCode(), rData.GetUserIndex(), szPrivateID, szPublicID,
		rData.GetRecvUserIndex(), szRecvPrivateID, szRecvPublicID,
		rData.GetUserIP().c_str(), rData.GetGameServerPort(),
		rData.GetGoodsNo(), 1, rData.GetItemPayAmt(),
		rData.GetGameServerPort(), 0);
}

BOOL ioChannelingNodeWemadeCashLink::ParsePresentReturnData( char* szReturnData, const int iSize, int& iReturnCash, int& iBonusCash, int& iChargedAmt, ioData& rData, char* errString, int iErrSize )
{
	try 
	{
		cJSON *pJson = NULL;
		pJson=cJSON_Parse(szReturnData);

		if( pJson == NULL )
			return FALSE;

		BOOL bState =FALSE;

		for (BYTE i = 0; i < cJSON_GetArraySize(pJson); i++)
		{
			cJSON *pObject = cJSON_GetArrayItem( pJson, i);

			if( pObject && pObject->string )
			{
				if( strcmp("result", pObject->string) == 0 )
				{
					if(pObject->valuestring)
					{
						if(strcmp(pObject->valuestring, WEMADE_SUCCESS_STRING) != 0)
						{
							bState = FALSE;
						}
						else
							bState = TRUE;
					}
				}
				else if( strcmp("message", pObject->string) == 0 )
				{
					if(pObject->valuestring)
					{
						strcpy_s(errString, iErrSize, pObject->valuestring);

					}
				}
				else if( strcmp("userNo", pObject->string) == 0 )
				{
					if(pObject->type == cJSON_Number)
					{
						if(rData.GetUserIndex() != pObject->valueint)
						{
							bState = FALSE;
						}
					}
				}
				else if( strcmp("realCash", pObject->string) == 0 )
				{
					iReturnCash = pObject->valueint;
				}
				else if( strcmp("bonusCash", pObject->string) == 0 )
				{
					iBonusCash = pObject->valueint;
				}
				else if( strcmp("chargedCashAmt", pObject->string) == 0 )
				{
					iChargedAmt = pObject->valueint;
				}
				else if( strcmp("itemInfos", pObject->string) == 0 )
				{
					for(int j =0; j< cJSON_GetArraySize(pObject); j++)
					{
						cJSON* pChild =  cJSON_GetArrayItem(pObject,j);

						if(pChild)
						{
							cJSON* pjChild = cJSON_GetObjectItem(pChild,"chargeNo");
							if(pjChild && pjChild->type == cJSON_String)
							{
								rData.SetChargeNo(pjChild->valuestring);

								break;
							}
						}
					}
				}
			}
		}

		cJSON_Delete(pJson);
		return bState;
	}
	catch ( ... )
	{
		LOG.PrintTimeAndLog( 0, "%s Json Crash %s", __FUNCTION__, szReturnData );
		return FALSE;
	}

	return FALSE;
}

void ioChannelingNodeWemadeCashLink::GetSubsCheck( char* szFullURL, const int iUrlSize, char* szParam, const int iParamSize, const ioData& rData )
{
	StringCbPrintf( szFullURL, iUrlSize, "%s", m_sSubscriptionRetractCheckURL.c_str() );

	//string multibyte -> UTF8 변경
	char szPrivateID[USER_ID_NUM] = "";
	MultiToUTF8(rData.GetPrivateID().c_str(), szPrivateID);

	StringCbPrintf( szParam, iParamSize,  
		"makeCodeNo=%d&userNo=%lu&userId=%s&"
		"itemInfos[0].chargeNo=%s",
		GetMakeCode(), rData.GetUserIndex(), szPrivateID,
		rData.GetChargeNo().c_str());
}

BOOL ioChannelingNodeWemadeCashLink::ParseSubsCheckReturnData( char* szReturnData, 
															   const int iSize, 
															   int& iReturnCash, 
															   int& iBonusCash, 
															   int& iCancelAmt, 
															   int& iRealCancelCash, 
															   int& iBonusCancelCash, 
															   ioData& rData, 
															   char* errString, 
															   int iErrSize)
{
	try 
	{
		cJSON *pJson = NULL;
		pJson=cJSON_Parse(szReturnData);

		if( pJson == NULL )
			return FALSE;

		BOOL bState =FALSE;

		for (BYTE i = 0; i < cJSON_GetArraySize(pJson); i++)
		{
			cJSON *pObject = cJSON_GetArrayItem( pJson, i);

			if( pObject && pObject->string )
			{
				if( strcmp("result", pObject->string) == 0 )
				{
					if(pObject->valuestring)
					{
						if(strcmp(pObject->valuestring, WEMADE_SUCCESS_STRING) != 0)
						{
							bState = FALSE;
						}
						else
							bState = TRUE;
					}
				}
				else if( strcmp("message", pObject->string) == 0 )
				{
					if(pObject->valuestring)
					{
						strcpy_s(errString, iErrSize, pObject->valuestring);

					}
				}
				else if( strcmp("userNo", pObject->string) == 0 )
				{
					if(pObject->type == cJSON_Number)
					{
						if(rData.GetUserIndex() != pObject->valueint)
						{
							bState = FALSE;
						}
					}
				}
				else if( strcmp("realChargedAmt", pObject->string) == 0 )
				{
					iReturnCash = pObject->valueint;
				}
				else if( strcmp("bonusChargedAmt", pObject->string) == 0 )
				{
					iBonusCash = pObject->valueint;
				}
				else if( strcmp("canceledAmt", pObject->string) == 0 )
				{
					iCancelAmt = pObject->valueint;
				}
				else if( strcmp("realCanceledAmt", pObject->string) == 0 )
				{
					iRealCancelCash = pObject->valueint;
				}
				else if( strcmp("bonusCanceledAmt", pObject->string) == 0 )
				{
					iBonusCancelCash = pObject->valueint;
				}
				else if( strcmp("itemInfos", pObject->string) == 0 )
				{
					for(int j =0; j< cJSON_GetArraySize(pObject); j++)
					{
						cJSON* pChild =  cJSON_GetArrayItem(pObject,j);

						if(pChild)
						{
							cJSON* pjChild = cJSON_GetObjectItem(pChild,"chargeNo");
							if(pjChild && pjChild->type == cJSON_String)
							{
								rData.SetChargeNo(pjChild->valuestring);

								break;
							}
						}
					}
				}
			}
		}

		cJSON_Delete(pJson);
		return bState;
	}
	catch ( ... )
	{
		LOG.PrintTimeAndLog( 0, "%s Json Crash %s", __FUNCTION__, szReturnData );
		return FALSE;
	}

	return FALSE;
}

void ioChannelingNodeWemadeCashLink::GetSubsRetract( char* szFullURL, const int iUrlSize, char* szParam, const int iParamSize, const ioData& rData )
{
	StringCbPrintf( szFullURL, iUrlSize, "%s", m_sSubscriptionRetractURL.c_str() );
	
	//string multibyte -> UTF8 변경
	char szPrivateID[USER_ID_NUM] = "";
	MultiToUTF8(rData.GetPrivateID().c_str(), szPrivateID);

	StringCbPrintf( szParam, iParamSize,  
		"makeCodeNo=%d&userNo=%lu&userId=%s&"
		"itemInfos[0].chargeNo=%s",
		GetMakeCode(), rData.GetUserIndex(), szPrivateID,
		rData.GetChargeNo().c_str());
}

BOOL ioChannelingNodeWemadeCashLink::ParseSubsRetract( char* szReturnData, 
													   const int iSize, 
													   int& iReturnCash, 
													   int& iBonusCash, 
													   int& iCancelAmt, 
													   int& iRealCancelCash, 
													   int& iBonusCancelCash, 
													   ioData& rData, 
													   char* errString, 
													   int iErrSize)
{
	try 
	{
		cJSON *pJson = NULL;
		pJson=cJSON_Parse(szReturnData);

		if( pJson == NULL )
			return FALSE;

		BOOL bState =FALSE;

		for (BYTE i = 0; i < cJSON_GetArraySize(pJson); i++)
		{
			cJSON *pObject = cJSON_GetArrayItem( pJson, i);

			if( pObject && pObject->string )
			{
				if( strcmp("result", pObject->string) == 0 )
				{
					if(pObject->valuestring)
					{
						if(strcmp(pObject->valuestring, WEMADE_SUCCESS_STRING) != 0)
						{
							bState = FALSE;
						}
						else
							bState = TRUE;
					}
				}
				else if( strcmp("message", pObject->string) == 0 )
				{
					if(pObject->valuestring)
					{
						strcpy_s(errString, iErrSize, pObject->valuestring);

					}
				}
				else if( strcmp("userNo", pObject->string) == 0 )
				{
					if(pObject->type == cJSON_Number)
					{
						if(rData.GetUserIndex() != pObject->valueint)
						{
							bState = FALSE;
						}
					}
				}
				else if( strcmp("realCash", pObject->string) == 0 )
				{
					iReturnCash = pObject->valueint;
				}
				else if( strcmp("bonusCash", pObject->string) == 0 )
				{
					iBonusCash = pObject->valueint;
				}
				else if( strcmp("canceledCashAmt", pObject->string) == 0 )
				{
					iCancelAmt = pObject->valueint;
				}
				else if( strcmp("itemInfos", pObject->string) == 0 )
				{
					for(int j =0; j< cJSON_GetArraySize(pObject); j++)
					{
						cJSON* pChild =  cJSON_GetArrayItem(pObject,j);

						if(pChild)
						{
							cJSON* pjChild = cJSON_GetObjectItem(pChild,"chargeNo");
							if(pjChild && pjChild->type == cJSON_String)
							{
								rData.SetChargeNo(pjChild->valuestring);

								break;
							}
						}
					}
				}
			}
		}

		cJSON_Delete(pJson);
		return bState;
	}
	catch ( ... )
	{
		LOG.PrintTimeAndLog( 0, "%s Json Crash %s", __FUNCTION__, szReturnData );
		return FALSE;
	}

	return FALSE;
}


void ioChannelingNodeWemadeCashLink::OnFillCashUrl( ServerNode *pServerNode, SP2Packet &rkPacket )
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

void ioChannelingNodeWemadeCashLink::ThreadFillCashUrl( const ioData &rData )
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
