#include "../stdafx.h"
#include "./ioChannelingNodeDaum_v2.h"
#include "./ioChannelingNodeDaum.h"
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

extern CLog LOG;

ioChannelingNodeDaum_v2::ioChannelingNodeDaum_v2(void)
{
	char szTemp[MAX_PATH]="";
	ioINILoader kLoader( "ls_config_billingsvr.ini" );
	kLoader.SetTitle( "Channeling" );

	kLoader.LoadString( "DaumGetURL", "", szTemp, sizeof( szTemp ) );
	m_sGetURL = szTemp;

	kLoader.LoadString( "DaumOutputURL", "", szTemp, sizeof( szTemp ) );
	m_sOutputURL = szTemp;

	kLoader.LoadString( "DaumCancelURL", "", szTemp, sizeof( szTemp ) );
	m_sSubscriptionRetractURL = szTemp;

	kLoader.LoadString( "DaumApikey", "", szTemp, sizeof( szTemp ) );
	m_sApikey = szTemp;

	kLoader.LoadString( "DaumFillURL", "", szTemp, sizeof( szTemp ) );
	m_sFillURL = szTemp;

	kLoader.LoadString( "DaumSendFillURL", "", szTemp, sizeof( szTemp ) );
	m_sSendFillURL = szTemp;

	kLoader.LoadString( "DaumShutDownCheckURL", "", szTemp, sizeof( szTemp ) );
	m_sShutDownCheckURL = szTemp;
}


ioChannelingNodeDaum_v2::~ioChannelingNodeDaum_v2(void)
{
}

ChannelingType ioChannelingNodeDaum_v2::GetType()
{
	return CNT_DAUM;
}

void ioChannelingNodeDaum_v2::_OnGetCash( ServerNode *pServerNode, SP2Packet &rkPacket )
{
	int          iChannelingType   = 0;
	ioHashString szBillingGUID;
	DWORD        dwUserIndex       = 0;
	ioHashString szPrivateID;
	ioHashString szPublicID;
	bool         bSetUserMouse     = false;
	ioHashString szDaumNo;

	rkPacket >> iChannelingType >> szBillingGUID >> dwUserIndex >> szPrivateID >> szPublicID >> bSetUserMouse; // 공통사항
	rkPacket >> szDaumNo; // daum용

	ioData kData;
	kData.SetChannelingType( iChannelingType );
	kData.SetBillingGUID( szBillingGUID );
	kData.SetUserIndex( dwUserIndex );
	kData.SetPrivateID( szPrivateID );
	kData.SetPublicID( szPublicID );
	kData.SetSetUserMouse( bSetUserMouse );
	kData.SetUserNo( szDaumNo );
	if( pServerNode )
	{
		kData.SetServerIP( pServerNode->GetIP() );
		kData.SetServerPort( pServerNode->GetClientPort() );
	}
	kData.SetEmpty( false );
	kData.SetProcessType( ioData::PT_GET_CASH );

	g_ThreadPool.SetData( kData );
}

void ioChannelingNodeDaum_v2::_OnOutputCash( ServerNode *pServerNode, SP2Packet &rkPacket, DWORD dwGoodsNo, const ioHashString &rszGoodsName )
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

	ioHashString szDaumNo;
	rkPacket >> szDaumNo; // daum 용 

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
	kData.SetUserNo( szDaumNo );
	kData.SetGoodsNo( dwGoodsNo );
	kData.SetGoodsName( rszGoodsName );
	if( pServerNode )
	{
		kData.SetServerIP( pServerNode->GetIP() );
		kData.SetServerPort( pServerNode->GetClientPort() );
	}
	kData.SetEmpty( false );
	kData.SetProcessType( ioData::PT_OUTPUT_CASH );

	g_ThreadPool.SetData( kData );
}

void ioChannelingNodeDaum_v2::ThreadGetCash( const ioData &rData )
{
	if( rData.IsEmpty() )
	{
		LOG.PrintTimeAndLog( 0, "[error][daum]%s Data is Empty.", __FUNCTION__ );
		return;
	}

	if( g_App.IsReserveLogOut() )
	{
		LOG.PrintTimeAndLog( 0, "%s LogOut: %d:%s:%s:%s:%s",__FUNCTION__, rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetPublicID().c_str(), rData.GetUserNo().c_str() );
		SP2Packet kPacket( BSTPK_GET_CASH_RESULT );
		kPacket << rData.GetUserIndex();
		kPacket << rData.GetBillingGUID();
		kPacket << rData.GetSetUserMouse();
		kPacket << CASH_RESULT_EXCEPT;
		if(!g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket ))
		{
			LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:%s", "ioChannelingNodeDaum_v2::ThreadGetCash", rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str() );
		}
		return;
	}

	int iReturnCash    = 0;
	int iPurchasedCash = 0; // 현금을 주고 구매한 캐쉬
	char szErrString[MAX_PATH]="";
	ioHashString szErrMessage;


	if( DaumGetCash(rData, iPurchasedCash, iReturnCash, szErrString, sizeof(szErrString)) == FALSE )
	{
		LOG.PrintTimeAndLog( 0, "%s Http Error: %d:%s:%s:%s:%s:%d szErrString:%s", __FUNCTION__, 
			rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetPublicID().c_str(), rData.GetUserNo().c_str(), GetLastError(),szErrString );

		szErrMessage = szErrString;
		SendExecptMessage(BSTPK_GET_CASH_RESULT, CASH_RESULT_EXCEPT, rData, szErrMessage);

		g_LogDBClient.OnInsertBillingServerError( CNT_WEMADEBUY, BILLING_ERROR_LOG_EXCEPTION,"Billing exception wemadeCashLink.(GetCash)" );
		return;
	}
	
	iReturnCash = iPurchasedCash;
#ifdef _TEST
	iReturnCash = 500;
#endif
	SP2Packet kPacket( BSTPK_GET_CASH_RESULT );
	kPacket << rData.GetUserIndex();
	kPacket << rData.GetBillingGUID();
	kPacket << rData.GetSetUserMouse();
	kPacket << CASH_RESULT_SUCCESS;
	kPacket << iReturnCash;
	kPacket << iPurchasedCash;
	if(!g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket ))
	{
		LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:%s", "ioChannelingNodeDaum_v2::ThreadGetCash", rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str() );
		return;		
	}
	//[%d:%d]
	LOG.PrintTimeAndLog( 0, "%s Success: %d:%s:PrivateID %s:%s:%s[%d:%d]", __FUNCTION__, rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetPublicID().c_str(), rData.GetUserNo().c_str(), iReturnCash, iPurchasedCash  );
	//LOG.PrintTimeAndLog(0, "%s Success ReturnValue :%s", __FUNCTION__, szReturnValue );
}

void ioChannelingNodeDaum_v2::ThreadOutputCash( const ioData &rData )
{
	if( rData.IsEmpty() )
	{
		LOG.PrintTimeAndLog( 0, "[error][daum]%s Data is Empty.", __FUNCTION__ );
		return;
	}

	
	static TwoOfINTVec vInfo;
	vInfo.clear();

	rData.GetBonusCashInfo(vInfo);
	int iBonusCashSize	= vInfo.size();

	if( g_App.IsReserveLogOut() )
	{
		LOG.PrintTimeAndLog( 0, "%s LogOut: %d:%s:%s:%s",__FUNCTION__, rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetPublicID().c_str() );
		SP2Packet kPacket( BSTPK_OUTPUT_CASH_RESULT  );
		kPacket << rData.GetUserIndex();
		kPacket << rData.GetBillingGUID();
		kPacket << CASH_RESULT_EXCEPT;
		kPacket << rData.GetExp();
		kPacket << rData.GetItemType();

		kPacket << iBonusCashSize;

		for( int i = 0; i < iBonusCashSize; i++ )
		{
			kPacket << vInfo[i].value1;
		}

		if(!g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket ))
		{
			LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:%s Ret %d:%s:%d", "ioChannelingNodeDaum_v2::ThreadOutputCash", rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetGoodsNo(), rData.GetGoodsName().c_str(), rData.GetItemPayAmt());
		}
		return;
	}
	int iReturnCash = 0;
	int iBonusCash = 0;
	int iChargeAmount = 0;
	char szErrString[MAX_PATH] ="";
	ioHashString szReturnBuyNO;
	ioHashString szErrMessage;

	char szReturnValue[WEB_BUFF_SIZE]="";
	ZeroMemory( szReturnValue, WEB_BUFF_SIZE );

	char szAgencyNo[ioLocalParent::MAX_AGENCY_NO_PLUS_ONE]="";
	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( pLocal )
	{
		pLocal->GetAgencyNo( szAgencyNo, sizeof( szAgencyNo ), true );
	}

	char ts[16]="";
	char nonce[41]="";
	char *sign = NULL;
	//Settlement+PayCash+{cpId}+{clientSeq}+{userId}+{sumAmt}
	char szParameters[MAX_PATH*4]="";
	StringCbPrintf( szParameters, sizeof( szParameters ), "SettlementPayCashlostsaga%s%s%d",szAgencyNo, rData.GetUserNo().c_str(), rData.GetItemPayAmt() );

	try 
	{
		getNewTimeStamp(ts); //kyg 
		getNewNonce(nonce);
		sign = getSig( (char*)m_sApikey.c_str(), szParameters, ts, nonce);
	}
	catch ( ... )
	{
		LOG.PrintTimeAndLog( 0, "%s Daum Error: %d:%s:%s:%s:%s:%d", __FUNCTION__, rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetPublicID().c_str(), rData.GetUserNo().c_str(), GetLastError() );
		SP2Packet kPacket( BSTPK_OUTPUT_CASH_RESULT );
		kPacket << rData.GetUserIndex();
		kPacket << rData.GetBillingGUID();
		kPacket << CASH_RESULT_EXCEPT;
		kPacket << rData.GetExp();
		kPacket << rData.GetItemType();
		kPacket << iBonusCashSize;

		for( int i = 0; i < iBonusCashSize; i++ )
		{
			kPacket << vInfo[i].value1;
		}

		if(!g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket ))
		{
			LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:%s Ret %d:%s:%d", "ioChannelingNodeDaum_v2::ThreadOutputCash", rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), 100020001, rData.GetGoodsName().c_str(), rData.GetItemPayAmt());
		}
		g_LogDBClient.OnInsertBillingServerError( CNT_DAUM, BILLING_ERROR_LOG_EXCEPTION,"Billing exception daum." );
		return;
	}

	char szOutputURL[MAX_PATH*2]="";
	StringCbPrintf( szOutputURL, sizeof( szOutputURL ), "%s%s/%d.json",m_sOutputURL.c_str(), rData.GetUserNo().c_str(), rData.GetItemPayAmt() );

	char szPostData[MAX_PATH*2]=""; //userid/sumAmt/itemId|amt/sign/ts/nonce/clientSeq
	StringCbPrintf( szPostData, sizeof( szPostData ), "itemIdAndAmts=%d|%s|%d&sign=%s&ts=%s&nonce=%s&clientSeq=%s",
		100020001,//하드코딩으로 박아놈 
		rData.GetGoodsName().c_str(),
		rData.GetItemPayAmt(),
		sign,
		ts,
		nonce,
		szAgencyNo);

	free(sign); ////////////////////// 반드시 free 필요 DAUM 소스
#ifdef _TEST
	SP2Packet testkPacket( BSTPK_OUTPUT_CASH_RESULT );
	testkPacket << rData.GetUserIndex();
	testkPacket << rData.GetBillingGUID();
	testkPacket << CASH_RESULT_SUCCESS;
	testkPacket << rData.GetExp();
	testkPacket << rData.GetItemType();
	testkPacket << rData.GetItemPayAmt();
	testkPacket << 0; // TransactionID ( FOR US )
	testkPacket << szReturnBuyNO; // same szChargeNo

	int iItemValueList[MAX_ITEM_VALUE];
	for (int i = 0; i <MAX_ITEM_VALUE; i++)
		iItemValueList[i] = rData.GetItemValue( i );
	SetItemValueList( testkPacket, rData.GetItemType(), iItemValueList );
	testkPacket << rData.GetChannelingType(); // 공통
	testkPacket << iReturnCash;
	testkPacket << iReturnCash;
	testkPacket << iBonusCashSize;

	for( int i = 0; i < iBonusCashSize; i++ )
	{
		testkPacket << vInfo[i].value1 << vInfo[i].value2;
	}

	//kyg 일단.. 넣어주질 않음 szReturnBuyNO를 
	if(!g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), testkPacket ))
	{
		LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:%s Ret %d:%s:%d", "ioChannelingNodeDaum_v2::ThreadOutputCash", rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetGoodsNo(), rData.GetGoodsName().c_str(), rData.GetItemPayAmt());
		return;	
	}
	return;
#endif
	ioHTTP Winhttp; 
	if( !Winhttp.GetResultData( szOutputURL, szPostData, szReturnValue, WEB_BUFF_SIZE ) )
	{
		LOG.PrintTimeAndLog( 0, "%s GetResultData Http Error: %d:%s:%s:%s:%s:%d:%s:%s", __FUNCTION__, rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetPublicID().c_str(), rData.GetUserNo().c_str(), GetLastError(), szOutputURL, szPostData);
		SP2Packet kPacket( BSTPK_OUTPUT_CASH_RESULT );
		kPacket << rData.GetUserIndex();
		kPacket << rData.GetBillingGUID();
		kPacket << CASH_RESULT_EXCEPT;
		kPacket << rData.GetExp();
		kPacket << rData.GetItemType();
		kPacket << iBonusCashSize;

		for( int i = 0; i < iBonusCashSize; i++ )
		{
			kPacket << vInfo[i].value1;
		}

		if(!g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket ))
		{
			LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:%s Ret %d:%s:%d", "ioChannelingNodeDaum_v2::ThreadOutputCash", rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetGoodsNo(), rData.GetGoodsName().c_str(), rData.GetItemPayAmt());
		}
		g_LogDBClient.OnInsertBillingServerError( CNT_DAUM, BILLING_ERROR_LOG_EXCEPTION,"Billing exception daum." );
		return;
	}

	
	if( ParseOutPutCashReturnData(szReturnValue, WEB_BUFF_SIZE, iChargeAmount, szErrString, sizeof(szErrString), szReturnBuyNO) == FALSE )
	{
		szErrMessage = szErrString;
		LOG.PrintTimeAndLog( 0, "%s Http Error: %d:%s:%s:%s:%s:%d:%s:%d", __FUNCTION__, 
			rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetPublicID().c_str(), rData.GetUserNo().c_str(), GetLastError(), szErrMessage.c_str(), rData.GetGoodsNo());

		SendExecptMessage(BSTPK_OUTPUT_CASH_RESULT, CASH_RESULT_EXCEPT, rData, szErrMessage);

		g_LogDBClient.OnInsertBillingServerError( CNT_DAUM, BILLING_ERROR_LOG_EXCEPTION,"Billing exception daum.(OutPutCash)" );
		return;
	}

	DaumGetCash(rData, iReturnCash, iReturnCash, szErrString, sizeof(szErrString));
	
	SP2Packet kPacket( BSTPK_OUTPUT_CASH_RESULT );
	kPacket << rData.GetUserIndex();
	kPacket << rData.GetBillingGUID();
	kPacket << CASH_RESULT_SUCCESS;
	kPacket << rData.GetExp();
	kPacket << rData.GetItemType();
	kPacket << rData.GetItemPayAmt();
	kPacket << 0; // TransactionID ( FOR US )
	kPacket << szReturnBuyNO; // same szChargeNo
	
	int iiItemValueList[MAX_ITEM_VALUE];
	for (int i = 0; i <MAX_ITEM_VALUE; i++)
		iiItemValueList[i] = rData.GetItemValue( i );
	SetItemValueList( kPacket, rData.GetItemType(), iiItemValueList );
	kPacket << rData.GetChannelingType(); // 공통
	kPacket << iReturnCash;
	kPacket << iReturnCash;
	kPacket << iBonusCashSize;

	for( int i = 0; i < iBonusCashSize; i++ )
	{
		kPacket << vInfo[i].value1 << vInfo[i].value2;
	}

    //kyg 일단.. 넣어주질 않음 szReturnBuyNO를 
	if(!g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket ))
	{
		LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:%s Ret %d:%s:%d", "ioChannelingNodeDaum_v2::ThreadOutputCash", rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetGoodsNo(), rData.GetGoodsName().c_str(), rData.GetItemPayAmt());
		return;	
	}
	//LOG.PrintTimeAndLog(0, "%s Success ReturnValue :%s", __FUNCTION__, szReturnValue );
	LOG.PrintTimeAndLog(0, "%s Success. :%d:%s:PrivateID %s:%s:%s[%d:%s:%d:%d:%d:%s]", __FUNCTION__, rData.GetUserIndex(), rData.GetBillingGUID().c_str(),
		rData.GetPrivateID().c_str(), rData.GetPublicID().c_str(), rData.GetUserNo().c_str(), //PrivateID %s:%s:%s
		rData.GetGoodsNo(), rData.GetGoodsName().c_str(), rData.GetItemPayAmt(), iReturnCash, iReturnCash, szReturnBuyNO.c_str() );
}

void ioChannelingNodeDaum_v2::ThreadSubscriptionRetract( const ioData& rData )
{
	ioHashString sError = "UnKnownError";

	if( rData.IsEmpty() )
	{
		LOG.PrintTimeAndLog( 0, "[error][daum]%s Data is Empty.", __FUNCTION__ );
		return;
	}

	if( g_App.IsReserveLogOut() )
	{
		LOG.PrintTimeAndLog( 0, "%s LogOut: %d:%s:%s:%s",__FUNCTION__, rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetPublicID().c_str() );

		SP2Packet kPacket( BSTPK_SUBSCRIPTION_RETRACT_CASH_RESULT );

		kPacket << rData.GetUserIndex();
		kPacket << rData.GetBillingGUID();
		kPacket << (DWORD) 0;
		kPacket << rData.GetChargeNo();
		kPacket << BILLING_SUBSCRIPTION_RETRACT_RESULT_FAIL;
		kPacket << sError;		
		
		if(!g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket ))
		{
			LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:%s Ret %d", "ioChannelingNodeDaum_v2::ThreadSubscriptionRetract", rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetChargeNo());
		}
		return;
	}

	char szReturnValue[WEB_BUFF_SIZE]="";
	ZeroMemory( szReturnValue, WEB_BUFF_SIZE );

	char szAgencyNo[ioLocalParent::MAX_AGENCY_NO_PLUS_ONE]="";
	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( pLocal )
	{
		pLocal->GetAgencyNo( szAgencyNo, sizeof( szAgencyNo ), true );
	}

	char ts[16]="";
	char nonce[41]="";
	char *sign = NULL;

	char szParameters[MAX_PATH*4]="";
	//Settlement+Cancel+{cpId}+{clientSeq}+{userId}

	StringCbPrintf( szParameters, sizeof( szParameters ), "SettlementCancellostsaga%s%s", szAgencyNo, rData.GetUserNo().c_str());

	try 
	{
		getNewTimeStamp(ts); //kyg 
		getNewNonce(nonce);
		sign = getSig( (char*)m_sApikey.c_str(), szParameters, ts, nonce);
	}
	catch ( ... )
	{
		LOG.PrintTimeAndLog( 0, "%s Daum Error: %d:%s:%s:%s:%s:%d", __FUNCTION__, rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetPublicID().c_str(), rData.GetUserNo().c_str(), GetLastError() );

		SP2Packet kPacket( BSTPK_SUBSCRIPTION_RETRACT_CASH_RESULT );

		kPacket << rData.GetUserIndex();
		kPacket << rData.GetBillingGUID();
		kPacket << (DWORD) 0;
		kPacket << rData.GetChargeNo();
		kPacket << BILLING_SUBSCRIPTION_RETRACT_RESULT_FAIL;
		kPacket << sError;

		if(!g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket ))
		{
			LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:%s Ret %d", "ioChannelingNodeDaum_v2::ThreadSubscriptionRetract", rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetChargeNo());
		}
		g_LogDBClient.OnInsertBillingServerError( CNT_DAUM, BILLING_ERROR_LOG_EXCEPTION,"ThreadSubscriptionRetract::Billing exception daum." );

		return;
	}


	char szPostData[MAX_PATH*4]="";
	StringCbPrintf( szPostData, sizeof( szPostData ), "logSeqGroup=%s&cancelMemo=userCancel&sign=%s&ts=%s&nonce=%s&clientSeq=%s",
		rData.GetChargeNo().c_str(),
		sign,
		ts,
		nonce,
		szAgencyNo);


	
	free(sign); ////////////////////// 반드시 free 필요 DAUM 소스

	char szFullURL[MAX_PATH*2]="";
	StringCbPrintf( szFullURL, sizeof( szFullURL ), "%s%s.json",m_sSubscriptionRetractURL.c_str(), rData.GetUserNo().c_str());
	ioHTTP Winhttp; //kyg 확인 필요  Get방식 되는지 확인 해야함 

	if( !Winhttp.GetResultData( szFullURL, szPostData, szReturnValue, WEB_BUFF_SIZE ) )
	{
		LOG.PrintTimeAndLog( 0, "%s http fail %d:%s:%s:%s:%s:%s",__FUNCTION__, rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetPublicID().c_str(), szFullURL, szPostData);
		SP2Packet kPacket( BSTPK_SUBSCRIPTION_RETRACT_CASH_RESULT );
		kPacket << rData.GetUserIndex();
		kPacket << rData.GetBillingGUID();
		kPacket << (DWORD) 0;
		kPacket << rData.GetChargeNo();
		kPacket << BILLING_SUBSCRIPTION_RETRACT_RESULT_FAIL;
		kPacket << sError;

		if(!g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket ))
		{
			LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:%s Ret %d", "ioChannelingNodeDaum_v2::ThreadSubscriptionRetract", rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetChargeNo());
		}
		g_LogDBClient.OnInsertBillingServerError( CNT_DAUM, BILLING_ERROR_LOG_EXCEPTION,"ThreadSubscriptionRetract::Billing exception daum." );
		return;
	}
	//BSTPK_SUBSCRIPTION_RETRACT_CASH_RESULT
	int iReturnCash = 0;
	int iCancelAmt = 0;

	ioHashString szErrMessage;
	char szErrString[MAX_PATH] = "";

	if( ParseSubsRetract(szReturnValue, WEB_BUFF_SIZE,  iCancelAmt, szErrString, sizeof(szErrString)) ==FALSE )
	{
		szErrMessage = szErrString;

		LOG.PrintTimeAndLog( 0, "%s Http Error: %d:%s:%s:%s:%s:%d", __FUNCTION__, 
			rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetPublicID().c_str(), rData.GetUserNo().c_str(), GetLastError() );

		SendExecptMessage(BSTPK_SUBSCRIPTION_RETRACT_CASH_RESULT, CASH_RESULT_EXCEPT, rData, szErrMessage);

		g_LogDBClient.OnInsertBillingServerError( CNT_DAUM, BILLING_ERROR_LOG_EXCEPTION,"Billing exception daum.(OutPutCash)" );
		return;
	}
	
	DaumGetCash( rData, iReturnCash, iReturnCash, szErrString, sizeof(szErrString) );

	// 캐쉬와 실재 구매한 캐쉬가 동일하다.
	SP2Packet kPacket( BSTPK_SUBSCRIPTION_RETRACT_CASH_RESULT );
	
	kPacket << rData.GetUserIndex();
	kPacket << rData.GetBillingGUID();
	kPacket << rData.GetIndex();
	kPacket << rData.GetChargeNo();
	kPacket << BILLING_SUBSCRIPTION_RETRACT_RESULT_SUCCESS;
	kPacket << iCancelAmt;
	kPacket << iReturnCash; 
	kPacket << iReturnCash; 
	 
	//kyg 일단.. 넣어주질 않음 szReturnBuyNO를 
	if(!g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket ))
	{
		LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:%s Ret %d", "ioChannelingNodeDaum_v2::ThreadSubscriptionRetract", rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetChargeNo());
		return;
	}
	LOG.PrintTimeAndLog(0, "%s Success. :%d:%s:PrivateID %s:%s:%s[%s]", __FUNCTION__, rData.GetUserIndex(), rData.GetBillingGUID().c_str(),
		rData.GetPrivateID().c_str(), rData.GetPublicID().c_str(), rData.GetUserNo().c_str(),rData.GetChargeNo().c_str() );
}

void ioChannelingNodeDaum_v2::ThreadFillCashUrl( const ioData &rData )
{
	if( rData.IsEmpty() )
	{
		LOG.PrintTimeAndLog( 0, "[error][daum]%s Data is Empty.", __FUNCTION__ );
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
	ioHashString szFillCashURL;

	char ts[16]="";
	char nonce[41]="";
	char *sign = NULL;
	char szAgencyNo[ioLocalParent::MAX_AGENCY_NO_PLUS_ONE]="";
	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( pLocal )
	{
		pLocal->GetAgencyNo( szAgencyNo, sizeof( szAgencyNo ), true );
	}
	ioHashString szItemList;
		//Settlement+Reserve+{cpId}+{clientSeq}+{userId}+{platform}+{paymentForm}
	char szParameters[MAX_PATH*4]="";
	StringCbPrintf( szParameters, sizeof( szParameters ), "SettlementReservelostsaga%s%sComputerCharge", szAgencyNo, rData.GetUserNo().c_str() );

	try 
	{
		getNewTimeStamp(ts); //kyg 
		getNewNonce(nonce);
		sign = getSig( (char*)m_sApikey.c_str(), szParameters, ts, nonce);
	}
	catch ( ... )
	{
		LOG.PrintTimeAndLog( 0, "%s Daum Error: %d:%s:%s:%s:%s:%d", __FUNCTION__, rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetPublicID().c_str(), rData.GetUserNo().c_str(), GetLastError() );
		SP2Packet kPacket( BSTPK_FILL_CASH_URL_RESULT );
		kPacket << rData.GetUserIndex();
		kPacket << rData.GetBillingGUID();
		kPacket << CASH_RESULT_EXCEPT;
		kPacket << rData.GetItemType();
		if(!g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket ))
		{
			LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:%s Ret %d:%s:%d", "ioChannelingNodeDaum_v2::ThreadOutputCash", rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetGoodsNo(), rData.GetGoodsName().c_str(), rData.GetItemPayAmt());
		}
		g_LogDBClient.OnInsertBillingServerError( CNT_DAUM, BILLING_ERROR_LOG_EXCEPTION,"Billing exception daum." );
		return;
	}


	char szReturnValue[WEB_BUFF_SIZE]="";

	char szErrorString[WEB_BUFF_SIZE] = "";
	ioHashString szErrMessage;

	char szFullURL[MAX_PATH*3]="";
	StringCbPrintf( szFullURL, sizeof( szFullURL ), "%s%s.json" , m_sFillURL.c_str(), rData.GetUserNo().c_str() );
	int iRsvSeq = 0;
	//
	char szPostData[MAX_PATH*2]="";//?paymentForm=Charge?platform=Computer?itemIdAndAmts=lostsaga_charge?successForward=N?appYn=N?sign=%d?ts=%d?nonce=%d?clientSeq=%s"
	StringCbPrintf( szPostData, sizeof( szPostData ), "paymentForm=Charge&platform=Computer&itemIdAndAmts=lostsaga_charge|충전|10000&successForward=N&appYn=N&sign=%s&ts=%s&nonce=%s&clientSeq=%s",
		sign,
		ts,
		nonce,
		szAgencyNo );

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

	if( ParseFillCashURL(szReturnValue, sizeof(szReturnValue),iRsvSeq, szErrorString, sizeof(szErrorString)) == FALSE )
	{
		LOG.PrintTimeAndLog( 0, "%s Http Error: %d:%s:%s:%s:%s:%d szErrString:%s", __FUNCTION__, 
			rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetPublicID().c_str(), rData.GetUserNo().c_str(), GetLastError(),szErrorString );

		SendExecptMessage(BSTPK_FILL_CASH_URL_RESULT, CASH_RESULT_EXCEPT, rData,szErrMessage);

		g_LogDBClient.OnInsertBillingServerError( CNT_DAUM, BILLING_ERROR_LOG_EXCEPTION,"Billing exception daum.(FillCAsh)" );
		return;
	}
	
	ZeroMemory(szFullURL, sizeof(szFullURL));

	StringCbPrintf( szFullURL, sizeof( szFullURL ), "%s%d" , m_sSendFillURL.c_str(), iRsvSeq );
	szFillCashURL = szFullURL;

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
}

void ioChannelingNodeDaum_v2::UTF8ToAnsi( IN const char *szUTF8, OUT char *szAnsi, OUT int &riReturnAnsiSize, IN int iAnsiBufferSize )
{
	if( szUTF8 == NULL )
		return;

	WCHAR szUni[MAX_PATH*2];
	ZeroMemory( szUni, sizeof(szUni) );

	int iUnisize = MultiByteToWideChar(CP_UTF8, 0, szUTF8, -1 , szUni, MAX_PATH*2 );
	riReturnAnsiSize = WideCharToMultiByte(CP_ACP, 0 , szUni, iUnisize, szAnsi, iAnsiBufferSize ,NULL ,NULL );
}

//------------------------------------------------------------------------------------------------DAUM
#include "openssl/sha.h"
#include "openssl/hmac.h"

#pragma warning (disable : 4996)
#pragma warning (disable : 4554)
#pragma warning (disable : 4018)

void ioChannelingNodeDaum_v2::decode(unsigned char* s, unsigned char* ret)
{
	int i=0;
	for(; i < strlen((const char*)s); i += 2)
	{
		unsigned char c = s[i];
		unsigned char c1 = s[i + 1];
		int j = i / 2;
		if(c < 'a')
			ret[j] = (unsigned char)(c - 48 << 4);
		else
			ret[j] = (unsigned char)((c - 97) + 10 << 4);
		if(c1 < 'a')
			ret[j] += (unsigned char)(c1 - 48);
		else
			ret[j] += (unsigned char)((c1 - 97) + 10);
	}
	ret[strlen((const char*)s)/2] = '\0';
}

char *ioChannelingNodeDaum_v2::pt(unsigned char *md, char* buf)
{
	int i;

	for (i=0; i<SHA_DIGEST_LENGTH; i++)
		sprintf(&(buf[i*2]),"%02x",md[i]);
	return(buf);
}

char *ioChannelingNodeDaum_v2::getSig(char *apiKey, char* data, char* ts, char* nonce) 
{
	int orgLen = strlen(data) + strlen(ts) + strlen(nonce) + 1;
	char *orgStr = (char*)malloc(orgLen);
	char *digestStr = (char*)malloc(SHA_DIGEST_LENGTH*2 + 1);
	char decodeApiKey[80];
	char *md;
	char tmp[80];
	char c[80];

	strncpy_s(orgStr,orgLen, data, strlen(data)+1);
	strncat_s(orgStr,orgLen, ts, strlen(ts)+1);
	strncat_s(orgStr,orgLen, nonce, strlen(nonce)+1);	

	decode((unsigned char*)apiKey, (unsigned char*)decodeApiKey);

	md =(char*) HMAC(EVP_sha1(),
		decodeApiKey, strlen(decodeApiKey),
		(const unsigned char*)orgStr, strlen(orgStr),
		(unsigned char*)c, NULL);

	strncpy_s(digestStr,SHA_DIGEST_LENGTH*2 + 1, pt((unsigned char *)md, tmp), SHA_DIGEST_LENGTH*2);
	digestStr[SHA_DIGEST_LENGTH*2] = '\0';

	free(orgStr);

	return digestStr;

}

void ioChannelingNodeDaum_v2::getNewNonce(char *nonce) 
{
	int i, tmp;

	memset(nonce, '0', SHA_DIGEST_LENGTH*2);
	srand(time(NULL));
	for(i=0; i<40; i+=6) {
		tmp = rand();
		sprintf(&(nonce[i]),"%02x", tmp);
	}
	nonce[40] = '\0';
}

void ioChannelingNodeDaum_v2::getNewTimeStamp(char *ts) 
{
	time_t          cal;
	struct tm       *lctm;

	if ((cal = time(NULL)) < 0)  {
		return;
	}

	lctm = localtime(&cal);

	strftime(ts, 15, "%Y%m%d%H%M%S", lctm);
	ts[15] = '\0'; //kyg 변수가 15짜린데 15에 널값어줌.. 
}

void ioChannelingNodeDaum_v2::_OnSubscriptionRetract( ServerNode* pServerNode, SP2Packet& rkPacket ) //kyg 이루틴 타는걸로도 테스트 해봐야함 
{
	int          iChannelingType = 0;
	ioHashString szBillingGUID;
	DWORD        dwUserIndex = 0;
	ioHashString szPublicID;
	ioHashString szPrivateID;
	DWORD		 dwIndex = 0;
	ioHashString szChargeNo;
	ioHashString szDaumNo;

	rkPacket >> iChannelingType;
	rkPacket >> szBillingGUID;
	rkPacket >> dwUserIndex;
	rkPacket >> szPublicID;
	rkPacket >> szPrivateID;
	rkPacket >> dwIndex; 
	//위에까지 공통인자 
	rkPacket >> szChargeNo;
	rkPacket >> szDaumNo;
 


	ioData kData;

	kData.SetChannelingType( iChannelingType );
	kData.SetBillingGUID( szBillingGUID );
	kData.SetUserIndex( dwUserIndex );
	kData.SetPublicID( szPublicID );
	kData.SetPrivateID( szPrivateID );
	kData.SetUserNo( szDaumNo );
	kData.SetChargeNo( szChargeNo );
	kData.SetIndex( dwIndex );
 
	if( pServerNode )
	{
		kData.SetServerIP( pServerNode->GetIP() );
		kData.SetServerPort( pServerNode->GetClientPort() );
	}
	kData.SetEmpty( false );
	kData.SetProcessType( ioData::PT_SUBSCRIPTION_RETRACT );

	g_ThreadPool.SetData( kData );
}

void ioChannelingNodeDaum_v2::OnFillCashUrl( ServerNode *pServerNode, SP2Packet &rkPacket )
{
	int          iChannelingType   = 0;
	ioHashString szBillingGUID;
	DWORD        dwUserIndex       = 0;
	ioHashString szUserNo;



	rkPacket >> iChannelingType >> szBillingGUID >> dwUserIndex; // 공통사항
	rkPacket >> szUserNo; // daum buy

	ioData kData;
	kData.SetChannelingType( iChannelingType );
	kData.SetBillingGUID( szBillingGUID );
	kData.SetUserIndex( dwUserIndex );
	kData.SetUserNo( szUserNo );

	if( pServerNode )
	{
		kData.SetServerIP( pServerNode->GetIP() );
		kData.SetServerPort( pServerNode->GetClientPort() );
	}
	kData.SetEmpty( false );
	kData.SetProcessType( ioData::PT_FILL_CASH_URL );

	g_ThreadPool.SetData( kData );
}

BOOL ioChannelingNodeDaum_v2::ParseGetCashReturnData( char* szReturnData, const int iSize, int& iRealCash, int& iBonusCash, char* szErrString, int iErrSize )
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
				if( strcmp("apiResult", pObject->string) == 0 )
				{
					for(int j =0; j< cJSON_GetArraySize(pObject); j++)
					{
						cJSON* pChild =  cJSON_GetArrayItem(pObject,j);

						if(pChild)
						{
							if(pChild && pChild->string)
							{
								if(strcmp("code", pChild->string) == 0)
								{
									if (strcmp(pChild->valuestring,"0000") == 0)
										bState = TRUE;
								}
								else if(strcmp("message", pChild->string) == 0)
								{
									int iReturnAnsiSize = 0;

									UTF8ToAnsi( pChild->valuestring, szErrString, iReturnAnsiSize, iErrSize );
								}
								else if( strcmp("responseData", pChild->string) == 0 )
								{ 
									cJSON* pArrayParrent = pChild->child;
									if(pArrayParrent && pArrayParrent->string)
									{
										if(strcmp("itemGroupSize", pArrayParrent->string) == 0)
										{
											int itemGroupSize = pArrayParrent->valueint;
											for(int x=0; x<itemGroupSize; x++)
											{
												cJSON* pArrayChild = cJSON_GetObjectItem(pChild, "itemGroup");

												if(pArrayChild)
												{
													cJSON* pArray = cJSON_GetObjectItem(pArrayChild->child,"daumCashBalance");

													if(strcmp("daumCashBalance", pArray->string) == 0)
													{
														iRealCash = pArray->valueint;
													}
												}
											}											
										}
									}
								}
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

BOOL ioChannelingNodeDaum_v2::ParseOutPutCashReturnData( char* szReturnData, const int iSize, int& iChargedAmt, char* szErrString, int iErrSize, ioHashString& szCharNo )
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
				if( strcmp("apiResult", pObject->string) == 0 )
				{
					for(int j =0; j< cJSON_GetArraySize(pObject); j++)
					{
						cJSON* pChild =  cJSON_GetArrayItem(pObject,j);

						if(pChild)
						{
							if(pChild && pChild->string)
							{
								if(strcmp("code", pChild->string) == 0)
								{
									if (strcmp(pChild->valuestring,"0000") == 0)
										bState = TRUE;
								}
								else if(strcmp("message", pChild->string) == 0)
								{
									int iReturnAnsiSize = 0;

									UTF8ToAnsi( pChild->valuestring, szErrString, iReturnAnsiSize, iErrSize );
								}
								else if( strcmp("responseData", pChild->string) == 0 )
								{ 
									cJSON* pArrayParrent = pChild->child;
									if(pArrayParrent && pArrayParrent->string)
									{
										if(strcmp("itemGroupSize", pArrayParrent->string) == 0)
										{
											int itemGroupSize = pArrayParrent->valueint;
											for(int x=0; x<itemGroupSize; x++)
											{
												cJSON* pArrayChild = cJSON_GetObjectItem(pChild, "itemGroup");

												if(pArrayChild)
												{
													cJSON* pArray = cJSON_GetObjectItem(pArrayChild->child,"logSeq");
													if(pArray)
													{
														char szTemp[MAX_PATH] = "";
														sprintf_s(szTemp,"%d",pArray->valueint);
														szCharNo = szTemp;
													}
													pArray = cJSON_GetObjectItem(pArrayChild->child,"daumCashAmt");
													if(pArray)
													{ 
														iChargedAmt = pArray->valueint;
													}
												}
											}											
										}
									}
								}
								
							}
						}
					}
				}
			}

		}//endfor
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

BOOL ioChannelingNodeDaum_v2::ParseSubsRetract( char* szReturnData, const int iSize, int& iCancelAmt, char* szErrString, int iErrSize )
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
				if( strcmp("apiResult", pObject->string) == 0 )
				{
					for(int j =0; j< cJSON_GetArraySize(pObject); j++)
					{
						cJSON* pChild =  cJSON_GetArrayItem(pObject,j);

						if(pChild)
						{
							if(pChild && pChild->string)
							{
								if(strcmp("code", pChild->string) == 0)
								{
									if (strcmp(pChild->valuestring,"0000") == 0)
										bState = TRUE;
								}
								else if(strcmp("message", pChild->string) == 0)
								{
									int iReturnAnsiSize = 0;

									UTF8ToAnsi( pChild->valuestring, szErrString, iReturnAnsiSize, iErrSize );
								}
								else if( strcmp("responseData", pChild->string) == 0 )
								{ 
									cJSON* pArrayParrent = pChild->child;
									if(pArrayParrent && pArrayParrent->string)
									{
										if(strcmp("itemGroupSize", pArrayParrent->string) == 0)
										{
											int itemGroupSize = pArrayParrent->valueint;
											for(int x=0; x<itemGroupSize; x++)
											{
												cJSON* pArrayChild = cJSON_GetObjectItem(pChild, "itemGroup");

												if(pArrayChild)
												{
													cJSON* pArray = cJSON_GetObjectItem(pArrayChild->child,"canceledAmt");
													
													if(pArray)
													{ 
														iCancelAmt = pArray->valueint;
													}
												}
											}											
										}
									}
								}

							}
						}
					}
				}
			}
		}//endfor
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

BOOL ioChannelingNodeDaum_v2::ParseFillCashURL( char* szReturnData, const int iSize, int& iRsvSeq, char* szErrString, int iErrSize )
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
				if( strcmp("apiResult", pObject->string) == 0 )
				{
					for(int j =0; j< cJSON_GetArraySize(pObject); j++)
					{
						cJSON* pChild =  cJSON_GetArrayItem(pObject,j);

						if(pChild)
						{
							if(pChild && pChild->string)
							{
								if(strcmp("code", pChild->string) == 0)
								{
									if (strcmp(pChild->valuestring,"0000") == 0)
										bState = TRUE;
								}
								else if(strcmp("message", pChild->string) == 0)
								{
									int iReturnAnsiSize = 0;

									UTF8ToAnsi( pChild->valuestring, szErrString, iReturnAnsiSize, iErrSize );
								}
								else if( strcmp("responseData", pChild->string) == 0 )
								{ 
									cJSON* pArrayParrent = pChild->child;
									if(pArrayParrent && pArrayParrent->string)
									{
										if(strcmp("itemGroupSize", pArrayParrent->string) == 0)
										{
											int itemGroupSize = pArrayParrent->valueint;
											for(int x=0; x<itemGroupSize; x++)
											{
												cJSON* pArrayChild = cJSON_GetObjectItem(pChild, "itemGroup");

												if(pArrayChild)
												{
													cJSON* pArray = cJSON_GetObjectItem(pArrayChild->child,"rsvSeq");

													if(pArray)
													{ 
														iRsvSeq = pArray->valueint;
													}
												}
											}											
										}
									}
								}

							}
						}
					}
				}
			}
		}//endfor
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

void ioChannelingNodeDaum_v2::SendExecptMessage( DWORD dwPacketID, int iErrCode, const ioData& rData, ioHashString& szErrString )
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
			kPacket << (!szErrString.IsEmpty());
			kPacket << szErrString;
		}
		break;
	case BSTPK_OUTPUT_CASH_RESULT:
		{
			bool bError	= !szErrString.IsEmpty();

			kPacket << rData.GetUserIndex();
			kPacket << rData.GetBillingGUID();
			kPacket << iErrCode;
			kPacket << rData.GetExp();
			kPacket << rData.GetItemType();
			kPacket << bError;

			if( bError )
				kPacket << szErrString;

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

BOOL ioChannelingNodeDaum_v2::DaumGetCash( const ioData &rData, int& iPurchasedCash, int& iReturnCash, char* szErrString, int iErrSize )
{
	char szReturnValue[WEB_BUFF_SIZE]="";
	ZeroMemory( szReturnValue, WEB_BUFF_SIZE );

	char szFullURL[MAX_PATH*2]="";
	char szParam[MAX_PATH]="";

	StringCbPrintf( szFullURL, sizeof( szFullURL ), "%s/%s.json?apiKey=%s" , m_sGetURL.c_str(), rData.GetUserNo().c_str(), m_sApikey.c_str() );

	ioHTTP Winhttp; //kyg 확인 필요 
	if( !Winhttp.GetResultData( szFullURL, "GET", szReturnValue, WEB_BUFF_SIZE ) )
	{
		return FALSE;
	}

	if( ParseGetCashReturnData(szReturnValue, sizeof(szReturnValue), iPurchasedCash, iReturnCash, szErrString, iErrSize) == FALSE )
	{
		return FALSE;
	}

	return TRUE;
}


BOOL ioChannelingNodeDaum_v2::ParseShutDownCheck(char* szReturnData, int& iResult)
{
	try 
	{
		cJSON *pJson = NULL;
		pJson=cJSON_Parse(szReturnData);

		if( pJson == NULL )
			return FALSE;
		
		for( BYTE i = 0; i < cJSON_GetArraySize(pJson); i++ )
		{
			cJSON *pObject = cJSON_GetArrayItem( pJson, i);

			if( pObject )
			{
				if( strcmp("status_code", pObject->string) == 0 )
				{
					ioHashString szReturnValue = pObject->valuestring;
					iResult = atoi(szReturnValue.c_str());
					return TRUE;
				}
			}
		}
	}
	catch ( ... )
	{
		LOG.PrintTimeAndLog( 0, "%s Json Crash %s", __FUNCTION__, szReturnData );
		return FALSE;
	}

	return FALSE;
}

BOOL ioChannelingNodeDaum_v2::CheckShutDownCheck(ioHashString& szDaumUserID)
{
	char szFullURL[MAX_PATH*2]="";
	char szReturnValue[WEB_BUFF_SIZE]="";
	ZeroMemory( szReturnValue, WEB_BUFF_SIZE );

	StringCbPrintf( szFullURL, sizeof(szFullURL), "%s%s", m_sShutDownCheckURL.c_str(), szDaumUserID.c_str() );
	
	ioHTTP Winhttp;
	if( !Winhttp.GetResultData( szFullURL, "GET", szReturnValue, WEB_BUFF_SIZE ) )
		return FALSE;

	int iResult = 0;
	if( !ParseShutDownCheck(szReturnValue, iResult) )
	{
		return FALSE;
	}

	if( DAUM_USER_ADULT_VALUE == iResult )
		return TRUE;

	return FALSE;
}