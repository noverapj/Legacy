#include "../stdafx.h"
#include "./ioChannelingNodeNexonBuy.h"
#include "../NodeInfo/ServerNode.h"
#include "../xml/ioxmldocument.h"
#include "../xml/ioxmlelement.h"
#include "../Util/ioHashString.h"
#include "../ThreadPool/ioThreadPool.h"
#include "../NodeInfo/ServerNodeManager.h"
#include "../MainProcess.h"
#include "../database/logdbclient.h"
#include "../Local/ioLocalParent.h"
#include "../../../extra/LS_NXSoap/LS_NXSoap/cNEXON.h"
#include "../ThreadPool/NexonThreadPool.h"
#include "../EtcHelpFunc.h"

extern CLog LOG;

cNEXON nxSoap;

ioChannelingNodeNexonBuy::ioChannelingNodeNexonBuy(void)
{
	char szTemp[MAX_PATH]="";
	ioINILoader kLoader( "ls_config_billingsvr.ini" );
	kLoader.SetTitle("NETWORK");

	kLoader.LoadString( "NexonSoqpIP", "", szTemp, sizeof( szTemp ) );
	m_sSoapIP = szTemp;

	kLoader.LoadString( "NexonHash", "", szTemp, sizeof( szTemp ) );
	m_sHashCode = szTemp;

	kLoader.LoadString( "NexonClientSecret", "", m_szNexonClientSecret, sizeof( m_szNexonClientSecret) );

	kLoader.LoadString( "NexonPlayRock", "", szTemp, sizeof( szTemp ) );
	m_sNexonPlayRock = szTemp;


	m_iNexonReqKey = 0;	//플레이락 용 ReqKey 초기화
}

ioChannelingNodeNexonBuy::~ioChannelingNodeNexonBuy(void)
{
}

ChannelingType ioChannelingNodeNexonBuy::GetType()
{
	return CNT_NEXON;
}

void ioChannelingNodeNexonBuy::_OnGetCash( ServerNode *pServerNode, SP2Packet &rkPacket )
{


	int          iChannelingType   = 0;
	ioHashString szBillingGUID;
	DWORD        dwUserIndex       = 0;
	ioHashString szPrivateID;
	ioHashString szPublicID;
	bool         bSetUserMouse     = false;
	ioHashString szNexonID;
	ioData kData;

	rkPacket >> iChannelingType >> szBillingGUID >> dwUserIndex >> szPrivateID >> szPublicID >> bSetUserMouse; // 공통사항
	rkPacket >> szNexonID; // For Nexon


	kData.SetChannelingType( iChannelingType );
	kData.SetBillingGUID( szBillingGUID );
	kData.SetUserIndex( dwUserIndex );
	kData.SetPrivateID( szPrivateID );
	kData.SetPublicID( szPublicID );
	kData.SetSetUserMouse( bSetUserMouse );
	kData.SetUserNo( szNexonID );
	//kData.SetSoap(GetNexonSoap());

	if( pServerNode )
	{
		kData.SetServerIP( pServerNode->GetIP() );
		kData.SetServerPort( pServerNode->GetClientPort() );
	}
	kData.SetEmpty( false );
	kData.SetProcessType( ioData::PT_GET_CASH );

	g_NexonThreadPool.SetData( kData );


}

void ioChannelingNodeNexonBuy::_OnOutputCash( ServerNode *pServerNode, SP2Packet &rkPacket, DWORD dwGoodsNo, const ioHashString &rszGoodsName )
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

	rkPacket >> iChannelingType >> szBillingGUID >> iReturnItemPrice >> dwUserIndex >> szPublicID >> szPrivateID >> szUserIP >> iPayAmt >> iType; // 공통사항

	int iItemValueList[MAX_ITEM_VALUE];
	for (int i = 0; i < MAX_ITEM_VALUE ; i++)
		iItemValueList[i] = 0;
	GetItemValueList( rkPacket, iType, iItemValueList );

	ioHashString szNexonID;
	rkPacket >> szNexonID; // For Nexon 

	ioData kData;
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

	if( iType == OUTPUT_CASH_PRESENT)
		kData.SetRecvUserIndex(iItemValueList[3]);

	//	kData.SetSoap(GetNexonSoap());

	if( pServerNode )
	{
		kData.SetServerIP( pServerNode->GetIP() );
		kData.SetServerPort( pServerNode->GetClientPort() );
	}
	kData.SetEmpty( false );
	kData.SetProcessType( ioData::PT_OUTPUT_CASH );

	g_NexonThreadPool.SetData( kData );
}


void ioChannelingNodeNexonBuy::ThreadGetCash( const ioData &rData )
{
	ioHashString szNexonNo = rData.GetUserNo();

	if( rData.IsEmpty() )
	{
		LOG.PrintTimeAndLog( 0, "[error][nexon]%s Data is Empty.", __FUNCTION__ );
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

		if ( !g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket ) )
		{
			LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:%s", "ioChannelingNodeNexonBuy::ThreadGetCash", rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str() );
		}

		return;
	}

	long iReturnCash    = 0;
	long iPurchasedCash = 0; // 현금을 주고 구매한 캐쉬

	//cNEXON nxSoap;

	// 	if(nxSoap == NULL)
	// 	{
	// 		LOG.PrintTimeAndLog(0,"Error ioChannelingNodeNexonBuy::ThreadGetCash NxSoap IS NULL ");
	// 		return;
	// 	}

	char TS[256] = {};
	SetTS(TS);

	BOOL ret = NexonInitializeSoap(nxSoap,TS,m_sSoapIP.c_str(),m_sHashCode.c_str());

	INT64 balance = 0;
	int error = 0;

	if(ret)
	{
		ret = NexonGetPurse(nxSoap,szNexonNo.c_str(), 1, m_sSoapIP.c_str(), error, balance);
	}
	else
	{
		LOG.PrintTimeAndLog( 0, "ioChannelingNodeNexonBuy::ThreadGetCash Soap Error: "
			"userIndex: %d "
			"BillginGUID:%s "
			"PrivateID:%s "
			"publicID:%s "
			"userNo:%s "
			"LasstError:%d",
			rData.GetUserIndex(),
			rData.GetBillingGUID().c_str(), 
			rData.GetPrivateID().c_str(), 
			rData.GetPublicID().c_str(), 
			rData.GetUserNo().c_str(),
			GetLastError() );

		LOG.PrintTimeAndLog(0, "(tid:%d)Soap Parameter : "
			"TS: %s ",
			GetCurrentThreadId(),
			TS);

		SP2Packet kPacket( BSTPK_GET_CASH_RESULT );
		kPacket << rData.GetUserIndex();
		kPacket << rData.GetBillingGUID();
		kPacket << rData.GetSetUserMouse();
		kPacket << CASH_RESULT_EXCEPT;
		kPacket << error;

		if ( !g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket ) )
		{
			LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:%s", "ioChannelingNodeNexonBuy::ThreadGetCash", rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str() );
		}
		return;
	}

	if(ret)
	{
		iReturnCash = balance;

		SP2Packet kPacket( BSTPK_GET_CASH_RESULT );

		kPacket << rData.GetUserIndex();
		kPacket << rData.GetBillingGUID();
		kPacket << rData.GetSetUserMouse();
		kPacket << CASH_RESULT_SUCCESS;
		kPacket << iReturnCash;

		if ( !g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket ) )
		{
			LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:%s", "ioChannelingNodeNexonBuy::ThreadGetCash", rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str() );
		}
		rData.GetUserNo().c_str();
		LOG.PrintTimeAndLog( 0, "(%d)ioChannelingNodeNexonBuy::ThreadGetCash Success: userIndex: %d "
			"BillGUID:%s "
			"PrivateID %s "
			"PublicID  %s "
			"UserNo : %s "
			"(%d/%d)",
			GetCurrentThreadId(),rData.GetUserIndex(),
			rData.GetBillingGUID().c_str(),
			rData.GetPrivateID().c_str(),
			rData.GetPublicID().c_str(),
			rData.GetUserNo().c_str(),
			iReturnCash, iPurchasedCash  );

	}
	else//false
	{
		SP2Packet kPacket( BSTPK_GET_CASH_RESULT );

		kPacket << rData.GetUserIndex();
		kPacket << rData.GetBillingGUID();
		kPacket << rData.GetSetUserMouse();
		kPacket << CASH_RESULT_EXCEPT;
		kPacket << error;


		if ( !g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket ) )
		{
			LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:%s", "ioChannelingNodeNexonBuy::ThreadGetCash", rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str() );
		}

		LOG.PrintTimeAndLog( 0, "(%d)ioChannelingNodeNexonBuy::ThreadGetCash Fail: userIndex: %d "
			"BillGUID:%s "
			"PrivateID %s "
			"PublicID  %s "
			"UserNo : %s "
			"(%d/%d)",
			GetCurrentThreadId(),rData.GetUserIndex(),
			rData.GetBillingGUID().c_str(),
			rData.GetPrivateID().c_str(),
			rData.GetPublicID().c_str(),
			rData.GetUserNo().c_str(),
			iReturnCash, iPurchasedCash  );

		return;
	}
}

void ioChannelingNodeNexonBuy::ThreadOutputCash( const ioData &rData )
{
	char szTmp[64];
	ZeroMemory(szTmp,sizeof(szTmp));

	ioHashString szNexonID = rData.GetUserNo();

	if( rData.IsEmpty() )
	{
		LOG.PrintTimeAndLog( 0, "[error][nexon]%s Data is Empty.", __FUNCTION__ );
		return;
	}

	if( g_App.IsReserveLogOut() )
	{
		LOG.PrintTimeAndLog( 0, "%s LogOut: %d:%s:%s:%s",__FUNCTION__, rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetPublicID().c_str() );
		SP2Packet kPacket( BSTPK_OUTPUT_CASH_RESULT  );
		kPacket << rData.GetUserIndex();
		kPacket << rData.GetBillingGUID();
		kPacket << CASH_RESULT_EXCEPT;
		kPacket << rData.GetExp();
		kPacket << rData.GetItemType();

		if ( !g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket ) )
		{
			LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:%s  Ret %d:%s:%d", "ioChannelingNodeNexonBuy::ThreadOutputCash", rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetGoodsNo(), rData.GetGoodsName().c_str(), rData.GetItemPayAmt() );
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

	char szTransactionID[64]="";
	StringCbPrintf( szTransactionID, sizeof( szTransactionID ), "NEXONBUY%s%s10000", szAgencyNo, rData.GetUserNo().c_str() );

	//cNEXON nxSoap;

	char TS[256] = {};
	SetTS(TS);

	BOOL ret = NexonInitializeSoap(nxSoap,TS,m_sSoapIP.c_str(),m_sHashCode.c_str());
	INT64 balance = 0;
	int error = 0;
	BOOL result;

	if(ret == FALSE)//fail
	{
		LOG.PrintTimeAndLog( 0, "(tid:%d) %s Soap Error: %d:%s:%s:%s:%s:%d",GetCurrentThreadId(), __FUNCTION__, rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetPublicID().c_str(), rData.GetUserNo().c_str(), GetLastError() );
		SP2Packet kPacket( BSTPK_OUTPUT_CASH_RESULT );
		kPacket << rData.GetUserIndex();
		kPacket << rData.GetBillingGUID();
		kPacket << CASH_RESULT_EXCEPT;
		kPacket << rData.GetExp();
		kPacket << rData.GetItemType();

		if ( !g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket ) )
		{
			LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:%s  Ret %d:%s:%d", "ioChannelingNodeNexonBuy::ThreadOutputCash", rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetGoodsNo(), rData.GetGoodsName().c_str(), rData.GetItemPayAmt() );
		}
		return;
	}

	char szGoodsNo[64];
	sprintf_s(szGoodsNo,sizeof(szGoodsNo),"%d",rData.GetGoodsNo());
	char szReceveIndex[64];
	sprintf_s(szReceveIndex,"%d",rData.GetItemValue(3));


	if(rData.GetItemType() == OUTPUT_CASH_PRESENT ||
		rData.GetItemType() == OUTPUT_CASH_POPUP )
	{
		ZeroMemory(szTmp,sizeof(szTmp));
		sprintf_s(szTmp,64,"%d",rData.GetItemPayAmt());
		LOG.PrintTimeAndLog(0,"Present Price : %s",szTmp);


		ret = NexonPresent(nxSoap,szNexonID.c_str(),
			szTransactionID,
			NEXON_GAME_CODE,
			szGoodsNo,
			szTmp,
			szNexonID.c_str(),
			szReceveIndex,
			30,
			rData.GetServerIP().c_str(),
			szTransactionID,
			1,
			rData.GetServerIP().c_str(),
			error,
			result);
	}
	else
	{	
		ZeroMemory(szTmp,sizeof(szTmp));
		sprintf_s(szTmp,sizeof(szTmp),"%d",rData.GetItemPayAmt()); //kygtestcod 꼭 바꿔놀것 
		ret = NexonPurchase(nxSoap,szNexonID.c_str(),
			szTransactionID,
			NEXON_GAME_CODE,
			szGoodsNo,
			szTmp,
			szNexonID.c_str(),
			rData.GetServerIP().c_str(),
			szTransactionID,
			1,
			rData.GetServerIP().c_str(),
			error,
			result);
	}
	if(ret)
	{
		if(result && error >= 0)//성공
		{

			char szTemp[64];
			sprintf_s(szTemp,sizeof(szTemp),"%d",error);

			ioHashString szChargeNo = szTemp;

			SP2Packet kPacket( BSTPK_OUTPUT_CASH_RESULT );
			kPacket << rData.GetUserIndex();
			kPacket << rData.GetBillingGUID();
			kPacket << CASH_RESULT_SUCCESS;
			kPacket << rData.GetExp();
			kPacket << rData.GetItemType();
			kPacket << rData.GetItemPayAmt();
			kPacket << 0; // TransactionID ( FOR US )
			kPacket << szChargeNo; // same szChargeNo

			int iItemValueList[MAX_ITEM_VALUE];
			for (int i = 0; i <MAX_ITEM_VALUE; i++)
				iItemValueList[i] = rData.GetItemValue( i );
			SetItemValueList( kPacket, rData.GetItemType(), iItemValueList );

			int iReturnCash = NexonGetCash(nxSoap,szNexonID);

			kPacket << rData.GetChannelingType(); // 공통
			kPacket << iReturnCash;
			kPacket << iReturnCash;

			//kyg 일단.. 넣어주질 않음 szReturnBuyNO를 
			if ( !g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket ) )
			{
				LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:%s  Ret %d:%s:%d", "ioChannelingNodeNexonBuy::ThreadOutputCash", rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetGoodsNo(), rData.GetGoodsName().c_str(), rData.GetItemPayAmt() );
				return;
			}
			LOG.PrintTimeAndLog(0,"ioChannelingNodeNexonBuy::ThreadOutputCash Success %d:%s:%d",rData.GetItemPayAmt(),szNexonID.c_str(),error);
		}
		else
		{
			//실패
			ZeroMemory(szTmp,sizeof(szTmp));
			sprintf_s(szTmp,"%d",error);

			SP2Packet kPacket( BSTPK_OUTPUT_CASH_RESULT );

			kPacket << rData.GetUserIndex();
			kPacket << rData.GetBillingGUID();
			kPacket << CASH_RESULT_EXCEPT;
			kPacket << rData.GetExp();
			kPacket << rData.GetItemType();
			kPacket << true;

			kPacket << szTmp;

			if ( !g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket ) )
			{
				LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:%s  Ret %d:%s:%d", "ioChannelingNodeNexonBuy::ThreadOutputCash", rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetGoodsNo(), rData.GetGoodsName().c_str(), rData.GetItemPayAmt() );
			}
			LOG.PrintTimeAndLog(0,"ioChannelingNodeNexonBuy::ThreadOutputCash Fail %d:%s:%d",rData.GetItemPayAmt(),szNexonID.c_str(),error);

			return;

		}
	}
	else
	{
		//실패
		SP2Packet kPacket( BSTPK_OUTPUT_CASH_RESULT );

		kPacket << rData.GetUserIndex();
		kPacket << rData.GetBillingGUID();
		kPacket << CASH_RESULT_EXCEPT;
		kPacket << rData.GetExp();
		kPacket << rData.GetItemType();
		kPacket << true;
		kPacket << szTmp;

		if ( !g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket ) )
		{
			LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:%s  Ret %d:%s:%d", "ioChannelingNodeNexonBuy::ThreadOutputCash", rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetGoodsNo(), rData.GetGoodsName().c_str(), rData.GetItemPayAmt() );
		}
		LOG.PrintTimeAndLog(0,"ioChannelingNodeNexonBuy::ThreadOutputCash ErrorFail %d:%s:%d",rData.GetItemPayAmt(),szNexonID.c_str(),error);

		return;
	}
}

void ioChannelingNodeNexonBuy::ThreadSubscriptionRetract( const ioData& rData )
{
	ioHashString sError = "UnKnownError";
	ioHashString szNexonID = rData.GetUserNo();

	if( rData.IsEmpty() )
	{
		LOG.PrintTimeAndLog( 0, "[error][nexon]%s Data is Empty.", __FUNCTION__ );
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

		if ( !g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket ) )
		{
			LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:%s  Ret %d:%s:%d", "ioChannelingNodeNexonBuy::ThreadSubscriptionRetract", rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetGoodsNo(), rData.GetGoodsName().c_str(), rData.GetItemPayAmt() );
		} 
		return;
	}

	char szAgencyNo[ioLocalParent::MAX_AGENCY_NO_PLUS_ONE]="";
	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( pLocal )
	{
		pLocal->GetAgencyNo( szAgencyNo, sizeof( szAgencyNo ), true );
	}

	//cNEXON nxSoap;

	char TS[256] = {};
	SetTS(TS);

	BOOL ret = NexonInitializeSoap(nxSoap,TS,m_sSoapIP.c_str(),m_sHashCode.c_str());
	INT64 balance = 0;
	int error = 0;


	char szTranSactionID[64]="";
	StringCbPrintf( szTranSactionID, sizeof( szTranSactionID ), "NEXONLODSTSAGASUBS%s%s", szAgencyNo, rData.GetChargeNo().c_str() );


	if(ret == FALSE)//fail
	{
		SP2Packet kPacket( BSTPK_SUBSCRIPTION_RETRACT_CASH_RESULT );
		kPacket << rData.GetUserIndex();
		kPacket << rData.GetBillingGUID();
		kPacket << (DWORD) 0;
		kPacket << rData.GetChargeNo();
		kPacket << BILLING_SUBSCRIPTION_RETRACT_RESULT_FAIL;
		kPacket << sError;

		if ( !g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket ) )
		{
			LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:%s  Ret %d:%s:%d", "ioChannelingNodeNexonBuy::ThreadSubscriptionRetract", rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetGoodsNo(), rData.GetGoodsName().c_str(), rData.GetItemPayAmt() );
		}
		LOG.PrintTimeAndLog(0, "(%d)ioChannelingNodeNexonBuy::ThreadSubscriptionRetract SoapFail . :userIndex %d "
			"BillGUID :%s "
			"PrivateID: %s "
			"PublicID : %s "
			"UserNo : %s "
			"Charge No : %s ",
			GetCurrentThreadId(),rData.GetUserIndex(),
			rData.GetBillingGUID().c_str(),
			rData.GetPrivateID().c_str(),
			rData.GetPublicID().c_str(),
			rData.GetUserNo().c_str(),
			rData.GetChargeNo().c_str() );
		return;
	}

	if(ret)
	{
		DWORD userSN = atoi(rData.GetChargeNo().c_str());
		ret = NexonUsageCancelByUsageSN(nxSoap,szNexonID.c_str(),userSN,rData.GetItemPayAmt(),error);
	}

	if(ret == TRUE &&error == 0)
	{
		SP2Packet kPacket( BSTPK_SUBSCRIPTION_RETRACT_CASH_RESULT );

		kPacket << rData.GetUserIndex();
		kPacket << rData.GetBillingGUID();
		kPacket << rData.GetIndex();
		kPacket << rData.GetChargeNo();
		kPacket << BILLING_SUBSCRIPTION_RETRACT_RESULT_SUCCESS;
		kPacket << 0;
		kPacket << 0; 

		if ( !g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket ) )
		{
			LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:%s  Ret %d:%s:%d", "ioChannelingNodeNexonBuy::ThreadSubscriptionRetract", rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetGoodsNo(), rData.GetGoodsName().c_str(), rData.GetItemPayAmt() );
			return;
		}
	}
	else //fail처리 추가 
	{
		SP2Packet kPacket( BSTPK_SUBSCRIPTION_RETRACT_CASH_RESULT );

		kPacket << rData.GetUserIndex();
		kPacket << rData.GetBillingGUID();
		kPacket << rData.GetIndex();
		kPacket << rData.GetChargeNo();
		kPacket << BILLING_SUBSCRIPTION_RETRACT_RESULT_FAIL;
		kPacket << 0;
		kPacket << 0; 

		if ( !g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket ) )
		{
			LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:%s  Ret %d:%s:%d", "ioChannelingNodeNexonBuy::ThreadSubscriptionRetract", rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetGoodsNo(), rData.GetGoodsName().c_str(), rData.GetItemPayAmt() );
		}

		LOG.PrintTimeAndLog(0, "(%d)ioChannelingNodeNexonBuy::ThreadSubscriptionRetract Return Fail. :userIndex %d "
			"BillGUID :%s "
			"PrivateID: %s "
			"PublicID : %s "
			"UserNo : %s "
			"Charge No : %s ",
			GetCurrentThreadId(),rData.GetUserIndex(),
			rData.GetBillingGUID().c_str(),
			rData.GetPrivateID().c_str(),
			rData.GetPublicID().c_str(),
			rData.GetUserNo().c_str(),
			rData.GetChargeNo().c_str() );
		return;
	}
	// 캐쉬와 실재 구매한 캐쉬가 동일하다.

	LOG.PrintTimeAndLog(0, "(%d)ioChannelingNodeNexonBuy::ThreadSubscriptionRetract Success. :userIndex %d "
		"BillGUID :%s "
		"PrivateID: %s "
		"PublicID : %s "
		"UserNo : %s "
		"Charge No : %s ",
		GetCurrentThreadId(),rData.GetUserIndex(),
		rData.GetBillingGUID().c_str(),
		rData.GetPrivateID().c_str(),
		rData.GetPublicID().c_str(),
		rData.GetUserNo().c_str(),
		rData.GetChargeNo().c_str() );
}


void ioChannelingNodeNexonBuy::_OnSubscriptionRetract( ServerNode* pServerNode, SP2Packet& rkPacket ) //kyg 이루틴 타는걸로도 테스트 해봐야함 
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

	LOG.PrintTimeAndLog(0,"ioChannelingNodeNexonBuy::_OnSubscriptionRetract cancelGold : %d",cancelGold);

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
	//	kData.SetSoap(GetNexonSoap());

	if( pServerNode )
	{
		kData.SetServerIP( pServerNode->GetIP() );
		kData.SetServerPort( pServerNode->GetClientPort() );
	}
	kData.SetEmpty( false );
	kData.SetProcessType( ioData::PT_SUBSCRIPTION_RETRACT );

	g_NexonThreadPool.SetData( kData );
}


void ioChannelingNodeNexonBuy::SetTS( char* TS )
{
	SYSTEMTIME systime;
	GetLocalTime(&systime);
	sprintf_s(TS, 255, "%04d%02d%02d%02d%02d%02d", systime.wYear, systime.wMonth, systime.wDay, systime.wHour, systime.wMinute, systime.wSecond);
}

int ioChannelingNodeNexonBuy::NexonGetCash( cNEXON& nxSoap, ioHashString& szNexonNo )
{
	char TS[256] = {};
	SetTS(TS);

	int error = 0;
	BOOL ret = FALSE;
	INT64 balance = 0;

	try
	{
		ret = NexonInitializeSoap(nxSoap,TS, m_sSoapIP.c_str(), m_sHashCode.c_str());

		if(!ret) 
		{
			LOG.PrintTimeAndLog(0,"[error][soap]InitializeSoap Error (%d)",error);
			return 0;
		}


		int error = 0;

		ret = NexonGetPurse(nxSoap,szNexonNo.c_str(), 1, m_sSoapIP.c_str(), error, balance);
	}
	catch( ... )
	{
		LOG.PrintTimeAndLog(0,"[error][soap]InitializeSoap");
	}
	return ret ? balance : 0;
}

BOOL ioChannelingNodeNexonBuy::NexonInitializeSoap( cNEXON& nxSoap, const char* TS, const char* IP, const char* KEY )
{
	try
	{
		BOOL rtVal = FALSE;
		int error = 0;

		rtVal = nxSoap.InitializeSoap(TS,IP,KEY,error);

		if(rtVal == false)
		{
			LOG.PrintTimeAndLog(0,"[error][soap]InitializeSoap (%d)",error);
		}	

		return rtVal;
	}
	catch( ... )
	{
		LOG.PrintTimeAndLog(0,"[error][soap]ioChannelingNodeNexonBuy::InitializeSoap");
	}
	return FALSE;
}

BOOL ioChannelingNodeNexonBuy::NexonGetPurse( cNEXON& nxSoap, const char* ID, const BYTE reason, const char* IP, int& error, INT64& balance )
{
	try
	{
		BOOL rtVal = FALSE;

		rtVal = nxSoap.GetPurse(ID,reason,IP,error,balance);

		return rtVal;
	}
	catch( ... )
	{
		LOG.PrintTimeAndLog(0,"[error][soap]ioChannelingNodeNexonBuy::NexonGetPurse::Getpurse");
	}
	return FALSE;
}

BOOL ioChannelingNodeNexonBuy::NexonPurchase( cNEXON& nxSoap, const char* ID, const char* transactionID, short productType, const char* productCode, const char* amount, const char* gameID, const char* serverID, const char* orderNO, BYTE reason, const char* IP, int& error, BOOL& result )
{
	try
	{
		BOOL rtVal = FALSE;

		rtVal = nxSoap.Purchase(ID,transactionID,productType,productCode,amount,gameID,serverID,orderNO,reason,serverID,error,result);

		return rtVal;
	}
	catch( ... )
	{
		LOG.PrintTimeAndLog(0,"[error][soap]ioChannelingNodeNexonBuy::Purchase Error (%s:%s:%d:%s:%d:%s:%s)",ID, transactionID, productType, productCode, amount, gameID, serverID);
	}
	return FALSE;
}

BOOL ioChannelingNodeNexonBuy::NexonPresent( cNEXON& nxSoap, const char* ID, const char* transactionID, short productType, const char* productCode, const char* amount, const char *gameID, const char *friendID, BYTE age, const char* serverID, const char* orderNO, BYTE reason, const char* IP, int& error, BOOL& result )
{
	try
	{
		BOOL rtVal = FALSE;

		rtVal = nxSoap.Present(ID,transactionID,productType,productCode,amount,gameID,friendID,age,serverID,orderNO,reason,IP,error,result);

		return rtVal;
	}
	catch( ... )
	{
		LOG.PrintTimeAndLog(0,"[error][soap]NexonPresent Error");
	}
	return FALSE;
}

BOOL ioChannelingNodeNexonBuy::NexonUsageCancelByUsageSN( cNEXON& nxSoap, const char* ID, int usageSN, int refundAmount, int& result )
{
	try
	{
		BOOL rtVal = FALSE;

		rtVal = nxSoap.UsageCancelByUsageSN(ID,usageSN,refundAmount,result);

		return rtVal;
	}
	catch( ... )
	{
		LOG.PrintTimeAndLog(0,"[error][soap]NexonUsageCancelByUsageSN Error");
	}
	return FALSE;
}


