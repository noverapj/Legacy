#include "../stdafx.h"
#include "./ioChannelingNodeNexonBuy_v2.h"
#include "../NodeInfo/ServerNode.h"
#include "../Util/ioHashString.h"
#include "../ThreadPool/ioThreadPool.h"
#include "../NodeInfo/ServerNodeManager.h"
#include "../MainProcess.h"
#include "../database/logdbclient.h"
#include "../Local/ioLocalParent.h"
#include "../../../extra/LS_RestAPI/ioRestAPI.h"
#include "../ThreadPool/NexonThreadPool.h"
#include "../EtcHelpFunc.h"
#include "../Util/cJSON.h"
#define NEXON_REUTRN_SUCCESS 200

extern CLog LOG;



ioChannelingNodeNexonBuy_v2::ioChannelingNodeNexonBuy_v2(void)
{
	char szTemp[MAX_PATH]="";
	ioINILoader kLoader( "ls_config_billingsvr.ini" );
	kLoader.SetTitle("Channeling");

	kLoader.LoadString( "NexonGetURL", "", szTemp, sizeof( szTemp ) );
	m_sGetURL = szTemp;

	kLoader.LoadString( "NexonOutputURL", "", szTemp, sizeof( szTemp ) );
	m_sOutputURL = szTemp;

	kLoader.LoadString( "NexonSubsRetractURL", "", szTemp, sizeof( szTemp ) );
	m_sSubscriptionRetractURL = szTemp;

	kLoader.LoadString( "NexonClientSecret", "", szTemp, sizeof( szTemp ) );
	m_sClientSecret = szTemp;
}

ioChannelingNodeNexonBuy_v2::~ioChannelingNodeNexonBuy_v2(void)
{
}

ChannelingType ioChannelingNodeNexonBuy_v2::GetType()
{
	return CNT_NEXON;
}

void ioChannelingNodeNexonBuy_v2::_OnGetCash( ServerNode *pServerNode, SP2Packet &rkPacket )
{
	int          iChannelingType   = 0;
	ioHashString szBillingGUID;
	DWORD        dwUserIndex       = 0;
	ioHashString szPrivateID;
	ioHashString szPublicID;
	bool         bSetUserMouse     = false;
	ioHashString szNexonID;
	ioHashString szNexonNo;
	ioData kData;

	rkPacket >> iChannelingType >> szBillingGUID >> dwUserIndex >> szPrivateID >> szPublicID >> bSetUserMouse; // 공통사항
	rkPacket >> szNexonID; // For Nexon
	rkPacket >> szNexonNo; // For Nexon


	kData.SetChannelingType( iChannelingType );
	kData.SetBillingGUID( szBillingGUID );
	kData.SetUserIndex( dwUserIndex );
	kData.SetPrivateID( szPrivateID );
	kData.SetPublicID( szPublicID );
	kData.SetSetUserMouse( bSetUserMouse );
	kData.SetUserNo( szNexonID );
	kData.SetNexonUserNo( szNexonNo );

	if( pServerNode )
	{
		kData.SetServerIP( pServerNode->GetIP() );
		kData.SetServerPort( pServerNode->GetClientPort() );
	}
	kData.SetEmpty( false );
	kData.SetProcessType( ioData::PT_GET_CASH );

	g_ThreadPool.SetData( kData );
}

void ioChannelingNodeNexonBuy_v2::_OnOutputCash( ServerNode *pServerNode, SP2Packet &rkPacket, DWORD dwGoodsNo, const ioHashString &rszGoodsName )
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
	ioHashString szNexonID;
	ioHashString szNexonNo;
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

	
	rkPacket >> szNexonID; // For Nexon 
	rkPacket >> szNexonNo; // For Nexon No

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
	kData.SetUserNo( szNexonID );
	kData.SetGoodsNo( dwGoodsNo );
	kData.SetGoodsName( rszGoodsName );
	kData.SetNexonUserNo( szNexonNo );

	if( iType == OUTPUT_CASH_PRESENT)
		kData.SetRecvUserIndex(iItemValueList[3]);

	if( pServerNode )
	{
		kData.SetServerIP( pServerNode->GetIP() );
		kData.SetServerPort( pServerNode->GetClientPort() );
	}
	kData.SetEmpty( false );
	kData.SetProcessType( ioData::PT_OUTPUT_CASH );

	g_ThreadPool.SetData( kData );
}

void ioChannelingNodeNexonBuy_v2::_OnSubscriptionRetract( ServerNode* pServerNode, SP2Packet& rkPacket ) //kyg 이루틴 타는걸로도 테스트 해봐야함 
{
	int          iChannelingType = 0;
	ioHashString szBillingGUID;
	DWORD        dwUserIndex = 0;
	ioHashString szPublicID;
	ioHashString szPrivateID;
	DWORD		 dwIndex = 0;
	ioHashString szChargeNo;
	ioHashString szNexonID;
	ioHashString szNexonNo;
	int cancelGold = 0;

	rkPacket >> iChannelingType;
	rkPacket >> szBillingGUID;
	rkPacket >> dwUserIndex;
	rkPacket >> szPublicID;
	rkPacket >> szPrivateID;
	rkPacket >> dwIndex; 
	//위에까지 공통인자 
	rkPacket >> szChargeNo;
	rkPacket >> szNexonID;
	rkPacket >> szNexonNo;
	rkPacket >> cancelGold;

	LOG.PrintTimeAndLog(0,"ioChannelingNodeNexonBuy_v2::_OnSubscriptionRetract cancelGold : %s %d",szNexonNo.c_str(), cancelGold);

	ioData kData;

	kData.SetChannelingType( iChannelingType );
	kData.SetBillingGUID( szBillingGUID );
	kData.SetUserIndex( dwUserIndex );
	kData.SetPublicID( szPublicID );
	kData.SetPrivateID( szPrivateID );
	kData.SetUserNo( szNexonID );
	kData.SetChargeNo( szChargeNo );
	kData.SetIndex( dwIndex );
	kData.SetItemPayAmt(cancelGold);
	kData.SetNexonUserNo( szNexonNo );

	if( pServerNode )
	{
		kData.SetServerIP( pServerNode->GetIP() );
		kData.SetServerPort( pServerNode->GetClientPort() );
	}
	kData.SetEmpty( false );
	kData.SetProcessType( ioData::PT_SUBSCRIPTION_RETRACT );

	g_ThreadPool.SetData( kData );
}


void ioChannelingNodeNexonBuy_v2::ThreadGetCash( const ioData &rData )
{
	const ioHashString& szNexonNo = rData.GetNexonUserNo();

	if( rData.IsEmpty() )
	{
		LOG.PrintTimeAndLog( 0, "[error][nexon] getCash Data is Empty." );
		return;
	}

	if( g_App.IsReserveLogOut() )
	{
		LOG.PrintTimeAndLog( 0, "[error][nexon] getCash LogOut: %d:%s:%s:%s:%s", rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetPublicID().c_str(), rData.GetUserNo().c_str() );
		SP2Packet kPacket( BSTPK_GET_CASH_RESULT );
		kPacket << rData.GetUserIndex();
		kPacket << rData.GetBillingGUID();
		kPacket << rData.GetSetUserMouse();
		kPacket << CASH_RESULT_EXCEPT;
			  
		if ( !g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket ) )
		{
			LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:%s", "ioChannelingNodeNexonBuy_v2::ThreadGetCash", rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str() );
		}
		
		return;
	}

	int iReturnCash    = 0;
	int iPurchasedCash = 0; // 현금을 주고 구매한 캐쉬
	int iResult = 0;

	ioHashString szErrString;

#ifdef _TEST
	iReturnCash = 500;

	SP2Packet testkPacket( BSTPK_GET_CASH_RESULT );
	testkPacket << rData.GetUserIndex();
	testkPacket << rData.GetBillingGUID();
	testkPacket << rData.GetSetUserMouse();
	testkPacket << CASH_RESULT_SUCCESS;
	testkPacket << iReturnCash;
	testkPacket << iPurchasedCash;

	if ( !g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), testkPacket ) )
	{
		LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:%s", "ioChannelingNodeNaver::ThreadGetCash", rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str() );
		return;
	}
	return;
#endif	

	if(NexonGetCash(szNexonNo, iReturnCash, iPurchasedCash, iResult, szErrString) == FALSE)
	{
		SendExecptMessage(BSTPK_GET_CASH_RESULT, CASH_RESULT_EXCEPT, rData, szErrString);

		LOG.PrintTimeAndLog( 0, "[error][nexon] getCash fail: threadid:%d userIndex: %d "
			"BillGUID:%s "
			"PrivateID %s "
			"PublicID  %s "
			"UserNo : %s "
			"NexonUserNo : %s"
			"(%d/%d)"
			"errString :%s",
			GetCurrentThreadId(),rData.GetUserIndex(),
			rData.GetBillingGUID().c_str(),
			rData.GetPrivateID().c_str(),
			rData.GetPublicID().c_str(),
			rData.GetUserNo().c_str(),
			rData.GetNexonUserNo().c_str(),
			iReturnCash, iPurchasedCash,
			szErrString.c_str());
		return;
	}

	SP2Packet kPacket( BSTPK_GET_CASH_RESULT );

	kPacket << rData.GetUserIndex();
	kPacket << rData.GetBillingGUID();
	kPacket << rData.GetSetUserMouse();
	kPacket << CASH_RESULT_SUCCESS;
	kPacket << iReturnCash;

	if ( !g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket ) )
	{
		LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:%s:%s", "ioChannelingNodeNexonBuy_v2::ThreadGetCash", rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetNexonUserNo().c_str() );
	}

	LOG.PrintTimeAndLog( 0, "[info][nexon] getCash Success: threadId:%d userIndex: %d "
		"BillGUID:%s "
		"PrivateID %s "
		"PublicID  %s "
		"UserNo : %s "
		"NexonNo : %s "
		"(%d/%d)",
		GetCurrentThreadId(),rData.GetUserIndex(),
		rData.GetBillingGUID().c_str(),
		rData.GetPrivateID().c_str(),
		rData.GetPublicID().c_str(),
		rData.GetUserNo().c_str(),
		rData.GetNexonUserNo().c_str(),
		iReturnCash, iPurchasedCash  );
}

void ioChannelingNodeNexonBuy_v2::ThreadOutputCash( const ioData &rData )
{
	const ioHashString& szNexonID = rData.GetNexonUserNo();
	ioHashString szErrString;
	ioHashString szBuyNo;
	int iResult = 0;

	if( rData.IsEmpty() )
	{
		LOG.PrintTimeAndLog( 0, "[error][nexon] outputCash Data is Empty.");
		return;
	}

	if( g_App.IsReserveLogOut() )
	{
		LOG.PrintTimeAndLog( 0, "[error][nexon] outputCash %s LogOut: %d:%s:%s:%s:%s", rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetPublicID().c_str(), rData.GetNexonUserNo().c_str() );

		SendExecptMessage(BSTPK_OUTPUT_CASH_RESULT, CASH_RESULT_EXCEPT, rData, szErrString);
		
		return;
	}

#ifdef _TEST
	SP2Packet testkPacket( BSTPK_OUTPUT_CASH_RESULT );
	testkPacket << rData.GetUserIndex();
	testkPacket << rData.GetBillingGUID();
	testkPacket << CASH_RESULT_SUCCESS;
	testkPacket << rData.GetExp();
	testkPacket << rData.GetItemType();
	testkPacket << rData.GetItemPayAmt();
	testkPacket << 0; // TransactionID ( FOR US ) //kyg 여기에 구매 유니크값 szReturnBuyNO
	testkPacket << "";

	int iiItemValueList[MAX_ITEM_VALUE];
	for (int i = 0; i <MAX_ITEM_VALUE; i++)
		iiItemValueList[i] = rData.GetItemValue( i );
	SetItemValueList( testkPacket, rData.GetItemType(), iiItemValueList );
	testkPacket << rData.GetChannelingType(); // 공통
	testkPacket << 0;
	testkPacket << 0;

	static TwoOfINTVec vvInfo;
	vvInfo.clear();

	rData.GetBonusCashInfo(vvInfo);
	int iiBonusCashSize	= vvInfo.size();

	testkPacket << iiBonusCashSize	;

	for( int i = 0; i < iiBonusCashSize; i++ )
	{
		testkPacket << vvInfo[i].value1 << vvInfo[i].value2;
	}

	if ( !g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), testkPacket ) )
	{
		LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:%s  Ret %d:%s:%d", "ioChannelingNodeNaver::ThreadOutputCash", rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetGoodsNo(), rData.GetGoodsName().c_str(), rData.GetItemPayAmt() );
		return;
	}
	return;
#endif
	if(NexonOutputCash(szNexonID, szBuyNo, (ioData&)rData, szErrString, iResult) == FALSE)
	{
		LOG.PrintTimeAndLog( 0, "[error][nexon] outputCash RestAPI Error: %d:%d:%s:%s:%s:%s:%s:%d:%s",GetCurrentThreadId(), rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetPublicID().c_str(), rData.GetUserNo().c_str(), rData.GetNexonUserNo().c_str(), GetLastError(), szErrString.c_str());

		SendExecptMessage(BSTPK_OUTPUT_CASH_RESULT, CASH_RESULT_EXCEPT, rData, szErrString);
		return;
	}

	SP2Packet kPacket( BSTPK_OUTPUT_CASH_RESULT );

	kPacket << rData.GetUserIndex();
	kPacket << rData.GetBillingGUID();
	kPacket << CASH_RESULT_SUCCESS;
	kPacket << rData.GetExp();
	kPacket << rData.GetItemType();
	kPacket << rData.GetItemPayAmt();
	kPacket << 0; // TransactionID ( FOR US )
	kPacket << szBuyNo; // same szChargeNo

	int iItemValueList[MAX_ITEM_VALUE];
	for (int i = 0; i <MAX_ITEM_VALUE; i++)
		iItemValueList[i] = rData.GetItemValue( i );
	SetItemValueList( kPacket, rData.GetItemType(), iItemValueList );

	int iReturnCash = 0;//kyg 채울것
	int iPurchaseCash = 0;

	if( NexonGetCash(szNexonID, iReturnCash, iPurchaseCash, iResult, szErrString) == FALSE )
		iReturnCash = 0;

	kPacket << rData.GetChannelingType(); // 공통
	kPacket << iReturnCash;
	kPacket << iReturnCash;

	static TwoOfINTVec vInfo;
	vInfo.clear();

	rData.GetBonusCashInfo(vInfo);
	int iBonusCashSize	= vInfo.size();

	kPacket << iBonusCashSize;

	for( int i = 0; i < iBonusCashSize; i++ )
	{
		kPacket << vInfo[i].value1 << vInfo[i].value2;
	}

	if ( !g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket ) )
	{
		LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:%s  Ret %d:%s:%d", "ioChannelingNodeNexonBuy_v2::ThreadOutputCash", rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetGoodsNo(), rData.GetGoodsName().c_str(), rData.GetItemPayAmt() );
		return;
	}

	LOG.PrintTimeAndLog(0,"[info][nexon] outputCash Success %d:%s:%d", rData.GetItemPayAmt(), szNexonID.c_str(), szErrString.c_str());
}

void ioChannelingNodeNexonBuy_v2::ThreadSubscriptionRetract( const ioData& rData )
{
	ioHashString sError = "UnKnownError";

	const ioHashString& szNexonID = rData.GetNexonUserNo();

	int iResult = 0;

	if( rData.IsEmpty() )
	{
		LOG.PrintTimeAndLog( 0, "%s Data is Empty.", __FUNCTION__ );
		return;
	}

	if( g_App.IsReserveLogOut() )
	{
		LOG.PrintTimeAndLog( 0, "%s LogOut: %d:%s:%s:%s:%s",__FUNCTION__, rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetPublicID().c_str(), szNexonID.c_str() );

		SendExecptMessage(BSTPK_SUBSCRIPTION_RETRACT_CASH_RESULT, BILLING_SUBSCRIPTION_RETRACT_RESULT_FAIL, rData, sError);
	}

	if(NexonSubscription(szNexonID, (ioData&)rData, sError, iResult) == TRUE)
	{
		int iReturnCash = 0;
		int iPurchaseCash = 0;
		int iCancelCash = rData.GetItemPayAmt();

		if( NexonGetCash(szNexonID, iReturnCash, iPurchaseCash, iResult, sError) == FALSE )
			iReturnCash = 0;

		SP2Packet kPacket( BSTPK_SUBSCRIPTION_RETRACT_CASH_RESULT );

		kPacket << rData.GetUserIndex();
		kPacket << rData.GetBillingGUID();
		kPacket << rData.GetIndex();
		kPacket << rData.GetChargeNo();
		kPacket << BILLING_SUBSCRIPTION_RETRACT_RESULT_SUCCESS;
		kPacket << iCancelCash;
		kPacket << iReturnCash; 
		kPacket << iReturnCash; 

		if ( !g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket ) )
		{
			LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:%s  Ret %d:%s:%d", "ioChannelingNodeNexonBuy_v2::ThreadSubscriptionRetract", rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetGoodsNo(), rData.GetGoodsName().c_str(), rData.GetItemPayAmt() );
			return;
		}
	}
	else //fail처리 추가 
	{
	 
		SendExecptMessage(BSTPK_SUBSCRIPTION_RETRACT_CASH_RESULT, BILLING_SUBSCRIPTION_RETRACT_RESULT_FAIL, rData, sError);

		LOG.PrintTimeAndLog(0, "[error][nexon] subscriptionRetract Fail thriadID:%d userIndex %d "
			"BillGUID :%s "
			"PrivateID: %s "
			"PublicID : %s "
			"UserNo : %s "
			"NexonUserNo : %s"
			"Charge No : %s ",
			GetCurrentThreadId(),rData.GetUserIndex(),
			rData.GetBillingGUID().c_str(),
			rData.GetPrivateID().c_str(),
			rData.GetPublicID().c_str(),
			rData.GetUserNo().c_str(),
			rData.GetNexonUserNo().c_str(),
			rData.GetChargeNo().c_str() );
		return;
	}
	// 캐쉬와 실재 구매한 캐쉬가 동일하다.

	LOG.PrintTimeAndLog(0, "[info][nexon] subscriptionRetract Success threadID:%d userIndex %d "
									"BillGUID :%s "
									"PrivateID: %s "
									"PublicID : %s "
									"UserNo : %s "
									"NexonUserNo : %s"
									"Charge No : %s ",
									GetCurrentThreadId(),rData.GetUserIndex(),
									rData.GetBillingGUID().c_str(),
									rData.GetPrivateID().c_str(),
									rData.GetPublicID().c_str(),
									rData.GetUserNo().c_str(),
									rData.GetNexonUserNo().c_str(),
									rData.GetChargeNo().c_str() );


}
  
BOOL ioChannelingNodeNexonBuy_v2::NexonGetCash( const ioHashString& szUserID, int& iRealCash, int& iPurchase, int& iResult, ioHashString& szErrString )
{
	ioRestAPI ioRest;
	char szFullURL[MAX_PATH*2] = {0,};
	char szHeader[MAX_PATH] = {0,};
	std::string szReturnData;
	std::string szTempString;

	StringCbPrintf(szFullURL, sizeof(szFullURL), "%s?billingID=%s@nx", m_sGetURL.c_str(), szUserID.c_str());
	StringCbPrintf(szHeader, sizeof(szFullURL), "clientsecret:%s", m_sClientSecret.c_str());

	ioRest.SetURL(szFullURL);
	ioRest.AddCustomHeader(szHeader);

	if(ioRest.Perfrom(szReturnData, szTempString) >= 0)
	{
		szErrString = szTempString.c_str();

		if(!szReturnData.empty())
		{
			if(NexGetCashParse(szReturnData.c_str(), iRealCash, iPurchase, iResult, szErrString) == TRUE)
			{
				iRealCash = iRealCash + iPurchase;
				return TRUE;
			}
			else
				LOG.PrintTimeAndLog(0,"[error][nexon] getcash Json Return Error :%s %s %s", szReturnData.c_str(), szErrString.c_str(), szUserID.c_str());
		}
	}
	else
	{
		LOG.PrintTimeAndLog(0,"[error][nexon] getcash ioRestAPI Error :%s %s %s %s", szReturnData.c_str(), szErrString.c_str(), szUserID.c_str(), szTempString.c_str());
	}

	return FALSE;
}

BOOL ioChannelingNodeNexonBuy_v2::NexGetCashParse( const char* szReturnData, int& iRealCash, int& iBonus, int& iResult, ioHashString& szErrString )
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
				if(strcmp("ResultCode", pObject->string) == 0)
				{
					if(pObject->valueint == NEXON_REUTRN_SUCCESS)
						bState = TRUE;
				}
				else if(strcmp("ResultMessage", pObject->string) == 0)
				{
					int iReturnAnsiSize = 0;
					char szTemp[512] = {0,};

					UTF8ToAnsi( pObject->valuestring, szTemp, iReturnAnsiSize, sizeof(szTemp) );

					szErrString = szTemp;
				}
				else if( strcmp("ResultValue", pObject->string) == 0 )
				{ 
					{
						cJSON* pArrayChild = cJSON_GetObjectItem(pObject, "FreeAmount");

						if(pArrayChild && pArrayChild->string)
						{
							if(strcmp("FreeAmount", pArrayChild->string) == 0)
							{
								iBonus = pArrayChild->valueint;
							}
						}
					}
					{
						cJSON* pArrayChild = cJSON_GetObjectItem(pObject, "PaidAmount");

						if(pArrayChild && pArrayChild->string)
						{
							if(strcmp("PaidAmount", pArrayChild->string) == 0)
							{
								iRealCash = pArrayChild->valueint;
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
		LOG.PrintTimeAndLog( 0, "[error][restapi] nexongetcash Json parse Crash %s ", szReturnData);
		return FALSE;
	}

	return FALSE;
}

BOOL ioChannelingNodeNexonBuy_v2::NexonOutputCash(const ioHashString& szUserID, ioHashString& szBuyNo, ioData& rData, ioHashString& szErrString, int& iResult)
{
	ioRestAPI ioRest;
	char szFullURL[MAX_PATH*2] = {0,};
	char szHeader[MAX_PATH] = {0,};
	std::string szReturnData;
	std::string szTempString;
	ioHashString szJsonData;

	StringCbPrintf(szFullURL, sizeof(szFullURL), "%s", m_sOutputURL.c_str());
	StringCbPrintf(szHeader, sizeof(szFullURL), "clientsecret:%s", m_sClientSecret.c_str());

	if(NexonOutputJsonParam(szUserID, rData, szErrString, szJsonData) == FALSE)
	{
		szErrString = "Json Error";
		return FALSE;
	}

	ioRest.SetURL(szFullURL);
	ioRest.AddCustomHeader(szHeader);
	ioRest.SetPostParam((char*)szJsonData.c_str());

	if(ioRest.Perfrom(szReturnData, szTempString) >= 0)
	{
		szErrString = szTempString.c_str();

		szErrString = "Unknown Error";

		if(!szReturnData.empty())
		{
			if(NexonOutputCashParse(szReturnData.c_str(), szBuyNo, rData, szErrString))
			{
				return TRUE;
			}
			else
				LOG.PrintTimeAndLog(0,"[error][restapi] outputcash json Return Error :%s %s %s %s", szReturnData.c_str(), szErrString.c_str(), szUserID.c_str(), rData.GetNexonUserNo().c_str() );
		}
	}
	else
		LOG.PrintTimeAndLog(0,"[error][restapi] outputcash ioRestAPI Error :%s %s %s %s %s", szReturnData.c_str(), szErrString.c_str(), szUserID.c_str(), szTempString.c_str(), rData.GetNexonUserNo().c_str());

	return FALSE;
}

BOOL ioChannelingNodeNexonBuy_v2::NexonOutputCashParse( const char* szReturnData, ioHashString& szBuyNo, ioData& rData, ioHashString& szErrString )
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
				if(strcmp("ResultCode", pObject->string) == 0)
				{
					if(pObject->valueint == NEXON_REUTRN_SUCCESS)
						bState = TRUE;
				}
				else if(strcmp("ResultMessage", pObject->string) == 0)
				{
					int iReturnAnsiSize = 0;
					char szTemp[512] = {0,};

					UTF8ToAnsi( pObject->valuestring, szTemp, iReturnAnsiSize, sizeof(szTemp) );

					szErrString = szTemp;
				}
				else if( strcmp("ResultValue", pObject->string) == 0 )
				{ 
					{
						cJSON* pArrayChild = cJSON_GetObjectItem(pObject, "usageSn");

						if(pArrayChild && pArrayChild->string)
						{
							if(strcmp("usageSn", pArrayChild->string) == 0)
							{
								char szSnTemp[MAX_PATH] = {0,};
								StringCbPrintf(szSnTemp, sizeof(szSnTemp), "%d", pArrayChild->valueint);
								szBuyNo = szSnTemp;
							
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
		LOG.PrintTimeAndLog( 0, "[error][nexon] outputCash Json ParseError %s", szReturnData );
		return FALSE;
	}

	return FALSE;
}

void ioChannelingNodeNexonBuy_v2::SendExecptMessage( DWORD dwPacketID, int iErrCode, const ioData& rData, ioHashString& szErrString )
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
	case BSTPK_SUBSCRIPTION_RETRACT_CASH_RESULT: 
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
		LOG.PrintTimeAndLog( 0, "[error][nexon] ProcessType:%d Send Fail: %d:%s:%s", "ioChannelingNodeWemadeCashLink::SendExecptMessage Fail", 
			rData.GetProcessType(),
			rData.GetUserIndex(),
			rData.GetBillingGUID().c_str(),
			rData.GetPrivateID().c_str() );
	}
	return;
}

void ioChannelingNodeNexonBuy_v2::UTF8ToAnsi( IN const char *szUTF8, OUT char *szAnsi, OUT int &riReturnAnsiSize, IN int iAnsiBufferSize )
{
	if( szUTF8 == NULL )
		return;

	WCHAR szUni[MAX_PATH*2];
	ZeroMemory( szUni, sizeof(szUni) );

	int iUnisize = MultiByteToWideChar(CP_UTF8, 0, szUTF8, -1 , szUni, MAX_PATH*2 );
	riReturnAnsiSize = WideCharToMultiByte(CP_ACP, 0 , szUni, iUnisize, szAnsi, iAnsiBufferSize ,NULL ,NULL );
}

BOOL ioChannelingNodeNexonBuy_v2::NexonOutputJsonParam( const ioHashString& szUserID, ioData& rData, ioHashString& szErrString, ioHashString& szJsonData )
{
	cJSON* root = NULL;

	__try
	{
		root = cJSON_CreateObject();
		 
		if(root)
		{
			char szTemp[MAX_PATH] = {0,};

			cJSON_AddStringToObject(root, "clientSecret", m_sClientSecret.c_str());

			cJSON_AddNumberToObject(root, "serverType", 1);

			StringCbPrintf(szTemp, sizeof(szTemp), "%s@nx", szUserID.c_str());
			cJSON_AddStringToObject(root, "billingID", szTemp);

			if(rData.GetRecvUserIndex())
				StringCbPrintf(szTemp, sizeof(szTemp), "%d", rData.GetRecvUserIndex());
			else
				StringCbPrintf(szTemp, sizeof(szTemp), "%d", rData.GetUserIndex());
			cJSON_AddStringToObject(root, "serviceID", szTemp);

			StringCbPrintf(szTemp, sizeof(szTemp), "%d", rData.GetGoodsNo());
			cJSON_AddStringToObject(root, "productCode", szTemp);

			cJSON_AddNumberToObject(root, "amount", rData.GetItemPayAmt());

			char szTransactionID[64]="";
			MakeTransactionID(szTransactionID, rData);
			cJSON_AddStringToObject(root, "transactionID", szTransactionID);

			if( OUTPUT_CASH_PRESENT == rData.GetItemType() )
			{
				char szRecvUser[128] = {0,};
				StringCbPrintf(szRecvUser, sizeof(szRecvUser), "%d", rData.GetRecvUserIndex());

				cJSON_AddNumberToObject(root, "useType", 2); // 선물 
				cJSON_AddStringToObject(root, "receiveBillingID", szRecvUser); // 선물 
				cJSON_AddStringToObject(root, "receiveServiceID", szRecvUser); // 선물 
			}
			else
				cJSON_AddNumberToObject(root, "useType", 1); // 구매

			char* szReutrnTemp = cJSON_PrintUnformatted(root);
			szJsonData = szReutrnTemp;

			free(szReutrnTemp);
			cJSON_Delete(root);	

			return TRUE;
		}
	}
	__except(1)
	{
		LOG.PrintTimeAndLog(0,"[error][nexon] outPutJasonParam Error %s", szUserID.c_str());
	}

	return FALSE;
}

void ioChannelingNodeNexonBuy_v2::MakeTransactionID( char* szTransactionID, ioData &rData )
{
	if(szTransactionID)
	{
		char szAgencyNo[ioLocalParent::MAX_AGENCY_NO_PLUS_ONE]="";

		ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
		if( pLocal )
		{
			pLocal->GetAgencyNo( szAgencyNo, sizeof( szAgencyNo ), true );
		}
		StringCbPrintf( szTransactionID, 64, "NEXONBILL%s%s10000", szAgencyNo, rData.GetUserNo().c_str() );
	}
}

BOOL ioChannelingNodeNexonBuy_v2::NexonSubscription( const ioHashString& szUserID, ioData& rData, ioHashString& szErrString, int& iResult )
{
	ioRestAPI ioRest;
	char szFullURL[MAX_PATH*2] = {0,};
	char szHeader[MAX_PATH] = {0,};
	std::string szReturnData;
	std::string szTempString;
	ioHashString szJsonData;
	ioHashString szCharNo = rData.GetChargeNo();

	StringCbPrintf(szFullURL, sizeof(szFullURL), "%s", m_sSubscriptionRetractURL.c_str());
	StringCbPrintf(szHeader, sizeof(szFullURL), "clientsecret:%s", m_sClientSecret.c_str());

	if(NexonSubsJsonParam(szUserID, rData, szErrString, szJsonData, szCharNo) == FALSE)
	{
		szErrString = "Error JasonParam";

		return FALSE;
	}

	ioRest.SetURL(szFullURL);
	ioRest.AddCustomHeader(szHeader);
	ioRest.SetPostParam((char*)szJsonData.c_str());

	if(ioRest.Perfrom(szReturnData, szTempString) >= 0)
	{
		szErrString = szTempString.c_str();

		szErrString = "Unknown Error";

		if(!szReturnData.empty())
		{
			if(NexonSubscriptionParse(szReturnData.c_str(), rData, szErrString))
			{
				return TRUE;
			}
			else
				LOG.PrintTimeAndLog(0,"[error][nexon] subcription json return Error %s %s %s %s", szUserID.c_str(), rData.GetNexonUserNo().c_str(), szReturnData.c_str(), szErrString.c_str());
		}
	}
	else
		LOG.PrintTimeAndLog(0,"[error][nexon] subscription ioRestAPI Error %s %s %s %s %s", szUserID.c_str(), rData.GetNexonUserNo().c_str(), szReturnData.c_str(), szErrString.c_str(), szTempString.c_str());

	return FALSE;
}

BOOL ioChannelingNodeNexonBuy_v2::NexonSubscriptionParse( const char* szReturnData, ioData& rData, ioHashString& szErrString )
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
				if(strcmp("ResultCode", pObject->string) == 0)
				{
					if(pObject->valueint == NEXON_REUTRN_SUCCESS)
						bState = TRUE;
				}
				else if(strcmp("ResultMessage", pObject->string) == 0)
				{
					int iReturnAnsiSize = 0;
					char szTemp[512] = {0,};

					UTF8ToAnsi( pObject->valuestring, szTemp, iReturnAnsiSize, sizeof(szTemp) );

					szErrString = szTemp;
				}
				else if( strcmp("ResultValue", pObject->string) == 0 )
				{ 
					
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

BOOL ioChannelingNodeNexonBuy_v2::NexonSubsJsonParam( const ioHashString& szUserID, ioData& rData, ioHashString& szErrString, ioHashString& szJsonData, ioHashString& szCharNo)
{
	cJSON* root = NULL;

	root = cJSON_CreateObject();

	DWORD dwBuyNo = atoi(szCharNo.c_str());

	__try
	{
		if(root)
		{
			char szTemp[MAX_PATH] = {0,};

			cJSON_AddStringToObject(root, "clientSecret", m_sClientSecret.c_str());

			cJSON_AddNumberToObject(root, "serverType", 1);

			StringCbPrintf(szTemp, sizeof(szTemp), "%s@nx", szUserID.c_str());
			cJSON_AddStringToObject(root, "billingID", szTemp);

			cJSON_AddNumberToObject(root, "usageSn", dwBuyNo);

			cJSON_AddNumberToObject(root, "refundAmount", rData.GetItemPayAmt());

			char* szReutrnTemp = cJSON_PrintUnformatted(root);
			szJsonData = szReutrnTemp;

			free(szReutrnTemp);   
			cJSON_Delete(root);	

			return TRUE;
		}
	}
	__except(1)
	{
		LOG.PrintTimeAndLog(0,"[error][nexon] SubsJsonParam Error %s", szUserID.c_str());
	}

	return FALSE;
}
