#include "../stdafx.h"
#include "./ioLocalJapan.h"
#include "../Util/md5.h"
#include "../Util/cJSON.h"
#include "../MainProcess.h"
#include "../NodeInfo/ServerNode.h"
#include "../DataBase/LogDBClient.h"
#include "../Channeling/ioChannelingNodeParent.h"
#include "../NodeInfo/ServerNodeManager.h"
#include "../ThailandOTPServer/ThailandOTPNodeManager.h"
#include "../EtcHelpFunc.h"
#include "../xml/ioxmldocument.h"
#include "../xml/ioxmlelement.h"
#include "../NodeInfo/GoodsManager.h"

// adbill Billing
#import "../JapanDLL/bxIPGClient.dll" no_namespace
#define BUF_SIZE 512
#define WTOM( wstr, mstr ) WideCharToMultiByte( CP_ACP, 0, (wstr), -1, (mstr), BUF_SIZE, NULL, NULL )
//

extern CLog LOG;

ioLocalJapan::ioLocalJapan(void)
{
	m_szCompanyCD = "BX00";
}

ioLocalJapan::~ioLocalJapan(void)
{
}

ioLocalManager::LocalType ioLocalJapan::GetType()
{
	return ioLocalManager::LCT_JAPAN;
}


void ioLocalJapan::Init()
{

	ioINILoader kLoader( "ls_config_billingsvr.ini" );
	kLoader.SetTitle( "Local" );

	char szMileageGetURL[MAX_PATH]="";
	kLoader.LoadString( "CJIJMileageGetURL", "", szMileageGetURL, sizeof( szMileageGetURL ) );
	m_sMileageGetURL = szMileageGetURL;

	char szMileageAddURL[MAX_PATH]="";
	kLoader.LoadString( "CJIJMileageAddURL", "", szMileageAddURL, sizeof( szMileageAddURL ) );
	m_sMileageAddURL = szMileageAddURL;

	if( m_sMileageGetURL.IsEmpty() || m_sMileageAddURL.IsEmpty() )
		LOG.PrintTimeAndLog(0,"Error CJIJ URL Error");
}

void ioLocalJapan::_OnGetCash( ServerNode *pServerNode, SP2Packet &rkPacket )
{
	int          iChannelingType   = 0;
	ioHashString szBillingGUID;
	DWORD        dwUserIndex       = 0;
	ioHashString szPrivateID;
	ioHashString szPublicID;
	bool         bSetUserMouse     = false;

	rkPacket >> iChannelingType >> szBillingGUID >> dwUserIndex >> szPrivateID >> szPublicID >> bSetUserMouse; // 공통사항

	if( g_App.IsReserveLogOut() )
	{
		LOG.PrintTimeAndLog( 0, "%s LogOut: %d:%s:%s:%s",__FUNCTION__, dwUserIndex, szBillingGUID.c_str(), szPrivateID.c_str(), szPublicID.c_str() );
		SP2Packet kPacket( BSTPK_GET_CASH_RESULT );
		kPacket << dwUserIndex;
		kPacket << szBillingGUID;
		kPacket << bSetUserMouse;
		kPacket << CASH_RESULT_EXCEPT;
		kPacket << false;
		pServerNode->SendMessage( kPacket );
		return;
	}

	long iReturnValue = 0;
	long iReturnCash  = 0;
	long iPurchasedCash = 0; // 현금을 주고 구매한 캐쉬
	try 
	{
		IIPGClientPtr ptrClient;
		ptrClient.CreateInstance(__uuidof(IPGClient));

		_variant_t vsUserNo, vsUserID, vsNickName, vsCompanyCD; // input
		_variant_t vsRemainCashShop, vsRemainCashContents, vsRemainCashBonus,
			vsRemainCashEtc, vsRemainMileageShop, vaRemainMileageContents;// output

		vsUserNo    = (LPSTR) szPrivateID.c_str(); 
		vsUserID    = (LPSTR) szPrivateID.c_str();
		vsNickName  = (LPSTR) szPublicID.c_str();
		vsCompanyCD = (LPSTR) m_szCompanyCD.c_str(); 

		if( g_App.IsTestMode() )
		{
			iReturnValue = CASH_RESULT_SUCCESS;
			TestCashManager::TestCashInfo *pInfo = m_TestCashManager.GetInfo( szPrivateID );
			if( pInfo )
			{
				m_TestCashManager.CheckNChargeCash( pInfo );
				iReturnCash  = pInfo->m_iCash;
				iPurchasedCash = pInfo->m_iCash;
			}
			else
			{
				m_TestCashManager.AddInfo( szPrivateID );
				TestCashManager::TestCashInfo *pInfo = m_TestCashManager.GetInfo( szPrivateID );
				if( pInfo )
				{
					iReturnCash    = pInfo->m_iCash;
					iPurchasedCash = pInfo->m_iCash;
				}
			}
		}
		else
		{
			// 값을 받을때까지 대기
			iReturnValue = ptrClient->GetCash( &vsUserNo, &vsUserID, &vsCompanyCD, &vsNickName
				                              ,&vsRemainCashShop,&vsRemainCashContents ,&vsRemainCashBonus
				                              ,&vsRemainCashEtc,&vsRemainMileageShop, &vaRemainMileageContents); 

			iReturnCash = 0;
			iReturnCash += vsRemainCashShop.lVal;
			iReturnCash += vsRemainCashContents.lVal;
			iReturnCash += vsRemainCashBonus.lVal;
			iReturnCash += vsRemainCashEtc.lVal;
			iReturnCash += vsRemainMileageShop.lVal;
			iReturnCash += vaRemainMileageContents.lVal;

			iPurchasedCash = iReturnCash;
		}
	} 
	catch (const _com_error &e )
	{
		LOG.PrintTimeAndLog( 0, "%s Come Exception: %d:%s:%s:%s:%s:%d", __FUNCTION__, dwUserIndex, szBillingGUID.c_str(), szPrivateID.c_str(), szPublicID.c_str(), (char*) e.ErrorMessage(), GetLastError()  );
		SP2Packet kPacket( BSTPK_GET_CASH_RESULT );
		kPacket << dwUserIndex;
		kPacket << szBillingGUID;
		kPacket << bSetUserMouse;
		kPacket << CASH_RESULT_EXCEPT;
		kPacket << false;
		pServerNode->SendMessage( kPacket );
		g_LogDBClient.OnInsertBillingServerError( CNT_WEMADEBUY, BILLING_ERROR_LOG_EXCEPTION,"Billing exception." );
		return;
	}
	catch ( ... )
	{
		LOG.PrintTimeAndLog( 0, "%s Come Exception: %d:%s:%s:%s:%d",__FUNCTION__, dwUserIndex, szBillingGUID.c_str(), szPrivateID.c_str(), szPublicID.c_str(), GetLastError()  );
		SP2Packet kPacket( BSTPK_GET_CASH_RESULT );
		kPacket << dwUserIndex;
		kPacket << szBillingGUID;
		kPacket << bSetUserMouse;
		kPacket << CASH_RESULT_EXCEPT;
		kPacket << false;
		pServerNode->SendMessage( kPacket );
		g_LogDBClient.OnInsertBillingServerError( CNT_WEMADEBUY, BILLING_ERROR_LOG_EXCEPTION,"Billing exception." );
		return;
	}

	SP2Packet kPacket( BSTPK_GET_CASH_RESULT );
	kPacket << dwUserIndex;
	kPacket << szBillingGUID;
	kPacket << bSetUserMouse;
	if( iReturnValue == CASH_RESULT_SUCCESS )
	{
		kPacket << CASH_RESULT_SUCCESS;
		kPacket << iReturnCash;
		kPacket << iPurchasedCash;
	}
	else
	{
		kPacket << CASH_RESULT_EXCEPT;
		kPacket << true;
		char szError[MAX_PATH]="";
		StringCbPrintf( szError, sizeof( szError ), "%d", iReturnValue );
		kPacket << szError;
	}
	pServerNode->SendMessage( kPacket );

	LOG.PrintTimeAndLog( 0, "%s Success: %d:%s:PrivateID %s:%s:%d:[%d/%d]", __FUNCTION__, dwUserIndex, szBillingGUID.c_str(), szPrivateID.c_str(), szPublicID.c_str(), iReturnValue, iReturnCash, iPurchasedCash  );
}


void ioLocalJapan::_OnOutputCash( ServerNode *pServerNode, SP2Packet &rkPacket, DWORD dwGoodsNo, const ioHashString &rszGoodsName )
{
	int          iChannelingType = 0;
	ioHashString szBillingGUID;
	DWORD        dwUserIndex = 0;
	ioHashString szPublicID;
	ioHashString szPrivateID;
	ioHashString szUserIP;
	int          iPayAmt  = 0;
	int          iType    = 0;

	rkPacket >> iChannelingType >> szBillingGUID >> dwUserIndex >> szPublicID >> szPrivateID >> szUserIP >> iPayAmt >> iType; // 공통사항

	if( g_App.IsReserveLogOut() )
	{
		LOG.PrintTimeAndLog( 0, "%s LogOut: %d:%s:%s:%s",__FUNCTION__, dwUserIndex, szBillingGUID.c_str(), szPrivateID.c_str(), szPublicID.c_str() );
		SP2Packet kPacket( BSTPK_OUTPUT_CASH_RESULT  );
		kPacket << dwUserIndex;
		kPacket << szBillingGUID;
		kPacket << CASH_RESULT_EXCEPT;
		kPacket << iType;
		kPacket << false;
		pServerNode->SendMessage( kPacket );
		return;
	}

	ioChannelingNodeParent::ItemInfo kBuyInfo;
	ioChannelingNodeParent::GetItemInfo( rkPacket, iType, kBuyInfo );
	
	char szOrderNo[BUF_SIZE]="";
	ZeroMemory( szOrderNo, sizeof( szOrderNo ) );
	long iReturnValue = 0;
	try 
	{
		IIPGClientPtr ptrClient;
		ptrClient.CreateInstance(__uuidof(IPGClient));

		_variant_t vsUserNo, vsUserID, vsCompanyCD, vsNickName, vsGoodsNo, vsPayAmt, vsAgencyNo, vsUserIP, vsEtc1;// input
		_variant_t vsBxaid ;    // output

		int iJapanBillingcode = GetJapanBillingCode( dwGoodsNo );
		enum { MAX_ETC_PLUS_ONE = 101, };
		char szEtc1[MAX_ETC_PLUS_ONE]="";
		StringCbPrintf( szEtc1, sizeof( szEtc1 ), "%d_%s", dwGoodsNo, rszGoodsName.c_str() );

		char szAgencyNo[MAX_AGENCY_NO_PLUS_ONE]="";
		GetAgencyNo( szAgencyNo, sizeof( szAgencyNo ), true );

		vsUserNo    = (LPSTR) szPrivateID.c_str(); 
		vsUserID    = (LPSTR) szPrivateID.c_str();
		vsCompanyCD = (LPSTR) m_szCompanyCD.c_str();  	
		vsGoodsNo   = (LONG)  iJapanBillingcode;
		vsPayAmt    = (LONG)  iPayAmt;
		vsUserIP    = (LPSTR) szUserIP.c_str();
		vsEtc1      = (LPSTR) szEtc1;
		vsNickName  = (LPSTR) szPublicID.c_str();
		vsAgencyNo  = (LPSTR) szAgencyNo;

		if( g_App.IsTestMode() )
		{
			iReturnValue = CASH_RESULT_SUCCESS;

			TestCashManager::TestCashInfo *pInfo = m_TestCashManager.GetInfo( szPrivateID );
			if( pInfo )
			{
				if( ( pInfo->m_iCash - iPayAmt ) > 0 )
					pInfo->m_iCash -= iPayAmt;
				else
					iReturnValue = CASH_RESULT_EXCEPT;
			}
			else
			{
				iReturnValue = CASH_RESULT_EXCEPT;
			}
		}
		else
		{
			// 값을 받을때까지 대기 
			iReturnValue = ptrClient->Output(&vsUserNo, &vsUserID, &vsCompanyCD, &vsNickName, &vsUserIP, &vsGoodsNo, &vsPayAmt ,&vsAgencyNo, &vsEtc1, &vsBxaid ); 
		}

		if( iReturnValue == CASH_RESULT_SUCCESS )
		{
			if( !g_App.IsTestMode() )
				WTOM( vsBxaid.bstrVal, szOrderNo );
			LOG.PrintTimeAndLog(0, "%s Success:%d:%s:PrivateID %s:%s:%s:%u:%s:%d", __FUNCTION__, dwUserIndex, szPublicID.c_str(), szPrivateID.c_str(), szBillingGUID.c_str(), szOrderNo, dwGoodsNo, rszGoodsName.c_str(), iPayAmt );
		}
		else
			LOG.PrintTimeAndLog(0, "%s ServerNode::OnOutputCash ReturnValue is Error:%d:%s:PrivateID %s:%s:%d:%u:%s", __FUNCTION__, dwUserIndex, szPublicID.c_str(), szPrivateID.c_str(), szBillingGUID.c_str(), iReturnValue, dwGoodsNo, rszGoodsName.c_str() );
	} 
	catch (const _com_error &e )
	{
		LOG.PrintTimeAndLog(0, "%s Exception:%s:%s:%u:%s:%s:%d", __FUNCTION__, szPublicID.c_str(), szBillingGUID.c_str(), dwGoodsNo, rszGoodsName.c_str(), (char*) e.ErrorMessage(), GetLastError() );
		SP2Packet kPacket( BSTPK_OUTPUT_CASH_RESULT );
		kPacket << dwUserIndex;
		kPacket << szBillingGUID;
		kPacket << CASH_RESULT_EXCEPT;
		kPacket << iType;
		kPacket << false;
		pServerNode->SendMessage( kPacket );
		g_LogDBClient.OnInsertBillingServerError( CNT_WEMADEBUY, BILLING_ERROR_LOG_EXCEPTION,"Billing exception." );
		return;
	}
	catch ( ... )	
	{
		LOG.PrintTimeAndLog(0, "%s Exception:%s:%s:%u:%s:%d", __FUNCTION__, szPublicID.c_str(), szBillingGUID.c_str(), dwGoodsNo, rszGoodsName.c_str(), GetLastError() );
		SP2Packet kPacket( BSTPK_OUTPUT_CASH_RESULT );
		kPacket << dwUserIndex;
		kPacket << szBillingGUID;
		kPacket << CASH_RESULT_EXCEPT;
		kPacket << iType;
     	kPacket << false;
		pServerNode->SendMessage( kPacket );
		g_LogDBClient.OnInsertBillingServerError( CNT_WEMADEBUY, BILLING_ERROR_LOG_EXCEPTION,"Billing exception." );
		return;
	}

	SP2Packet kPacket( BSTPK_OUTPUT_CASH_RESULT );
	kPacket << dwUserIndex;
	kPacket << szBillingGUID;

	// return value
	if( iReturnValue == CASH_RESULT_SUCCESS )
	{
		kPacket << CASH_RESULT_SUCCESS;
		kPacket << iType;
		kPacket << iPayAmt;
		kPacket << 0; // TransactionID ( FOR US )
		ioChannelingNodeParent::SetItemInfo( kPacket, iType, kBuyInfo );

		// Cancel Step 1
		kPacket << iChannelingType;  // 공통
		kPacket << szPrivateID;
		kPacket << szOrderNo;
		kPacket << szUserIP;
		kPacket << dwGoodsNo;
		kPacket << rszGoodsName;
		kPacket << szPublicID;
	}
	else
	{
		kPacket << CASH_RESULT_EXCEPT;
		kPacket << iType;
		kPacket << true;
		char szError[MAX_PATH]="";
		StringCbPrintf( szError, sizeof( szError ), "%d", iReturnValue );
		kPacket << szError;
		g_LogDBClient.OnInsertBillingServerError( CNT_WEMADEBUY, iReturnValue,"Error return value." );
	}

	pServerNode->SendMessage( kPacket );
	LOG.PrintTimeAndLog( 0, "%s Send : %d %s %d",__FUNCTION__, dwUserIndex, szPrivateID.c_str(), iReturnValue );
}

void ioLocalJapan::OnCancelCash( ServerNode *pServerNode, SP2Packet &rkPacket )
{
	int          iChannelingType = 0;
	ioHashString szBillingGUID;
	DWORD        dwUserIndex = 0;
	ioHashString szPublicID;
	ioHashString szPrivateID;
	ioHashString szOrderNo;
	ioHashString szUserIP;
	int          iPayAmt  = 0;
	DWORD        dwGoodsNo = 0;
	ioHashString szGoodsName;

	// 공통
	rkPacket >> iChannelingType; 
	rkPacket >> szBillingGUID; 
	rkPacket >> dwUserIndex; 

	// Cancel Step 4
	rkPacket >> iPayAmt;
	rkPacket >> szPrivateID;
	rkPacket >> szOrderNo;
	rkPacket >> szUserIP;
	rkPacket >> dwGoodsNo;
	rkPacket >> szGoodsName;
	rkPacket >> szPublicID;


	long iReturnValue = 0;
	try 
	{
		IIPGClientPtr ptrClient;
		ptrClient.CreateInstance(__uuidof(IPGClient));

		_variant_t vsUserNo, vsUserID, vsCompanyCD, vsNickName, vOrderNo, vsGoodsNo, vsPayAmt, vsAgencyNo, vsUserIP, vsEtc1;// input
		_variant_t vsBxaid ;    // output

		int iJapanBillingcode = GetJapanBillingCode( dwGoodsNo );
		enum { MAX_ETC_PLUS_ONE = 101, };
		char szEtc1[MAX_ETC_PLUS_ONE]="";
		StringCbPrintf( szEtc1, sizeof( szEtc1 ), "%d_%s", dwGoodsNo, szGoodsName.c_str() );

		char szAgencyNo[MAX_AGENCY_NO_PLUS_ONE]="";
		GetAgencyNo( szAgencyNo, sizeof( szAgencyNo ), true );

		vsUserNo    = (LPSTR) szPrivateID.c_str(); 
		vsUserID    = (LPSTR) szPrivateID.c_str();
		vsCompanyCD = (LPSTR) m_szCompanyCD.c_str();  	
		vsGoodsNo   = (LONG)  iJapanBillingcode;
		vsPayAmt    = (LONG)  iPayAmt;
		vsUserIP    = (LPSTR) szUserIP.c_str();
		vsEtc1      = (LPSTR) szEtc1;
		vsNickName  = (LPSTR) szPublicID.c_str();
		vsAgencyNo  = (LPSTR) szAgencyNo;
		vOrderNo    = (LPSTR) szOrderNo.c_str();

		if( g_App.IsTestMode() )
		{
			iReturnValue = CASH_RESULT_SUCCESS;
		}
		else
		{
			// 값을 받을때까지 대기 
			iReturnValue = ptrClient->OutputCancel(&vsUserNo, &vsUserID, &vsCompanyCD, &vsNickName, &vsUserIP, &vOrderNo, &vsGoodsNo, &vsPayAmt ,&vsAgencyNo, &vsEtc1, &vsBxaid ); 
		}

		if( iReturnValue == CASH_RESULT_SUCCESS )
		{
			char szReOrderNo[BUF_SIZE]="";
			ZeroMemory( szReOrderNo, sizeof( szReOrderNo ) );
			if( !g_App.IsTestMode() )
				WTOM( vsBxaid.bstrVal, szReOrderNo );
			LOG.PrintTimeAndLog(0, "%s Success:%d:%s:PrivateID %s:%s:%s:%u:%s:%d", __FUNCTION__, dwUserIndex, szPublicID.c_str(), szPrivateID.c_str(), szBillingGUID.c_str(), szReOrderNo, dwGoodsNo, szGoodsName.c_str(), iPayAmt );
		}
		else
			LOG.PrintTimeAndLog(0, "%s ReturnValue is Error:%d:%s:PrivateID %s:%s:%d:%u:%s", __FUNCTION__, dwUserIndex, szPublicID.c_str(), szPrivateID.c_str(), szBillingGUID.c_str(), iReturnValue, dwGoodsNo, szGoodsName.c_str() );
	} 
	catch (const _com_error &e )
	{
		LOG.PrintTimeAndLog(0, "%s Exception:%s:%s:%u:%s:%s:%d", __FUNCTION__, szPublicID.c_str(), szBillingGUID.c_str(), dwGoodsNo, szGoodsName.c_str(), (char*) e.ErrorMessage(), GetLastError() );
		return;
	}
	catch ( ... )	
	{
		LOG.PrintTimeAndLog(0, "%s Exception:%s:%s:%u:%s:%d", __FUNCTION__, szPublicID.c_str(), szBillingGUID.c_str(), dwGoodsNo, szGoodsName.c_str(), GetLastError() );
		return;
	}
}

void ioLocalJapan::OnGetMileage( ServerNode *pServerNode, SP2Packet &rkPacket )
{
	DWORD        dwUserIndex       = 0;
	ioHashString szPrivateID;
	
	rkPacket >> dwUserIndex >> szPrivateID; 

	if( !pServerNode )
	{
		LOG.PrintTimeAndLog(0, "%s pServerNode == NULL:%d:%s", __FUNCTION__, dwUserIndex, szPrivateID.c_str() );
		return;
	}

	if( g_App.IsReserveLogOut() )
	{
		LOG.PrintTimeAndLog( 0, "%s LogOut: %d:%s",__FUNCTION__, dwUserIndex, szPrivateID.c_str() );
		SP2Packet kPacket( BSTPK_GET_MILEAGE_RESULT );
		kPacket << dwUserIndex;
		kPacket << "Dummy";
		kPacket << BILLING_GET_MILEAGE_RESULT_FAIL;
		pServerNode->SendMessage( kPacket );
		return;
	}

	char szReturnValue[WEB_BUFF_SIZE]="";
	ZeroMemory( szReturnValue, WEB_BUFF_SIZE );

	char szTimeStamp[MAX_PATH]="";
	GetTimeStamp( szTimeStamp, sizeof( szTimeStamp ) );

	char szUniteValue[MAX_PATH*2]="";
	StringCbPrintf( szUniteValue, sizeof( szUniteValue ), "%s%s%s", szPrivateID.c_str() , szTimeStamp, JAPAN_MILEAGE_KEY );

	char szHexMD5Key[MAX_PATH]="";
	ZeroMemory( szHexMD5Key, sizeof( szHexMD5Key ) );
	GetHexMD5( szHexMD5Key, sizeof( szHexMD5Key ), szUniteValue );

	char szFullURL[MAX_PATH*4]="";
	StringCbPrintf( szFullURL, sizeof( szFullURL ), "%s?iCN=%s&TT=%s&MD=%s" , m_sMileageGetURL.c_str(), szPrivateID.c_str(), szTimeStamp, szHexMD5Key );

	if( g_App.IsTestMode() )
	{
		ioHashString szMileagePrivateID;
		char szTempID[MAX_PATH]="";
		StringCbPrintf( szTempID, sizeof( szTempID ), "%s#mileage#", szPrivateID.c_str() );
		szMileagePrivateID = szTempID;

		int iMileage = 0;
		TestCashManager::TestCashInfo *pInfo = m_TestCashManager.GetInfo( szMileagePrivateID );
		if( pInfo )
		{
			m_TestCashManager.CheckNChargeCash( pInfo );
			iMileage = pInfo->m_iCash;
		}
		else
		{
			m_TestCashManager.AddInfo( szMileagePrivateID );
			TestCashManager::TestCashInfo *pInfo = m_TestCashManager.GetInfo( szMileagePrivateID );
			if( pInfo )
			{
				iMileage = pInfo->m_iCash;
			}
		}

		StringCbPrintf( szReturnValue, WEB_BUFF_SIZE,  "<?xml version=\"1.0\" encoding=\"utf-8\" ?> <mileage> <oMileage>%d</oMileage> </mileage>", iMileage );
	}
	else
	{	
		ioHTTP Winhttp; //kyg 확인 필요 
		if( !Winhttp.GetResultData( szFullURL, "GET", szReturnValue, WEB_BUFF_SIZE ) )
		{
			LOG.PrintTimeAndLog( 0, "%s Http Error: %d:%s:%d", __FUNCTION__, dwUserIndex, szPrivateID.c_str(), GetLastError() );
			SP2Packet kPacket( BSTPK_GET_MILEAGE_RESULT );
			kPacket << dwUserIndex;
			kPacket << "Dummy";
			kPacket << BILLING_GET_MILEAGE_RESULT_FAIL;
			pServerNode->SendMessage( kPacket );
			g_LogDBClient.OnInsertBillingServerError( CNT_WEMADEBUY, BILLING_ERROR_LOG_EXCEPTION,"Billing exception GetCash(HTTP)." );
			return;
		}
	}
	
	if( strcmp( szReturnValue , "" ) == 0 )
	{
		LOG.PrintTimeAndLog( 0, "%s Return Empty: %d:%s:%d", __FUNCTION__, dwUserIndex, szPrivateID.c_str(), GetLastError() );
		SP2Packet kPacket( BSTPK_GET_MILEAGE_RESULT );
		kPacket << dwUserIndex;
		kPacket << "Dummy";
		kPacket << BILLING_GET_MILEAGE_RESULT_FAIL;
		pServerNode->SendMessage( kPacket );
		g_LogDBClient.OnInsertBillingServerError( CNT_WEMADEBUY, BILLING_ERROR_LOG_EXCEPTION,"Billing exception Gatcash(ReturnEmpty)." );
		return;
	}

	ioXMLDocument xmlDoc;
	if( !xmlDoc.LoadFromMemory( szReturnValue ) )
	{
		SP2Packet kPacket( BSTPK_GET_MILEAGE_RESULT );
		kPacket << dwUserIndex;
		kPacket << "Dummy";
		kPacket << BILLING_GET_MILEAGE_RESULT_FAIL;
		pServerNode->SendMessage( kPacket );
		LOG.PrintTimeAndLog(0, "%s Fail ioXMLDocument::LoadFromMemory :%d:%s", __FUNCTION__, dwUserIndex, szPrivateID.c_str() );
		return;
	}

	ioXMLElement xRootElement = xmlDoc.GetRootElement();
	if( xRootElement.IsEmpty() )	
	{
		SP2Packet kPacket( BSTPK_GET_MILEAGE_RESULT );
		kPacket << dwUserIndex;
		kPacket << "Dummy";
		kPacket << BILLING_GET_MILEAGE_RESULT_FAIL;
		pServerNode->SendMessage( kPacket );
		LOG.PrintTimeAndLog(0, "%s ioXMLElement is Empty.:%d:%s", __FUNCTION__, dwUserIndex, szPrivateID.c_str() );
		return;
	}	

/*
	<?xml version="1.0" encoding="utf-8" ?>
	<mileage>
			<oMileage>1000</oMileage>
	</mileage>
*/	

	// parsing
	int iReturnMileage  = 0;
	ioXMLElement xChildElement = xRootElement.FirstChild( "oMileage" );
	if( !xChildElement.IsEmpty() && xChildElement.GetText() != NULL )
		iReturnMileage = atoi( xChildElement.GetText() );

	SP2Packet kPacket( BSTPK_GET_MILEAGE_RESULT );
	kPacket << dwUserIndex;
	kPacket << "Dummy";
	kPacket << BILLING_GET_MILEAGE_RESULT_SUCCESS;
	kPacket << iReturnMileage;
	pServerNode->SendMessage( kPacket );

	LOG.PrintTimeAndLog( 0, "%s Success %s:%d:%d", __FUNCTION__, szPrivateID.c_str(), dwUserIndex, iReturnMileage );
}

void ioLocalJapan::OnAddMileage( ServerNode *pServerNode, SP2Packet &rkPacket )
{
	DWORD        dwUserIndex = 0;
	ioHashString szPrivateID;
	ioHashString szPublicID;
	ioHashString szPublicIP;
	int          iPresentType = 0;
	int          iValue1      = 0;
	int          iValue2      = 0;
	int          iSellPeso    = 0;
	bool         bPresent     = false;


	rkPacket >> dwUserIndex >> szPrivateID >> szPublicID >> szPublicIP >> iPresentType >> iValue1 >> iValue2 >> iSellPeso >> bPresent;

	if( !pServerNode )
	{
		LOG.PrintTimeAndLog(0, "%s pServerNode == NULL:%d:%s:%s:%s:%d:%d:%d:%d:%d", __FUNCTION__, dwUserIndex, szPrivateID.c_str(), szPublicID.c_str(), szPublicIP.c_str(), iPresentType, iValue1, iValue2, iSellPeso, (int)bPresent );
		return;
	}


	if( g_App.IsReserveLogOut() )
	{
		LOG.PrintTimeAndLog(0, "%s LogOut:%d:%s:%s:%s:%d:%d:%d:%d:%d", __FUNCTION__, dwUserIndex, szPrivateID.c_str(), szPublicID.c_str(), szPublicIP.c_str(), iPresentType, iValue1, iValue2, iSellPeso, (int)bPresent );
		SP2Packet kPacket( BSTPK_ADD_MILEAGE_RESULT );
		kPacket << dwUserIndex;
		kPacket << "Dummy";
		kPacket << BILLING_ADD_MILEAGE_RESULT_FAIL;
		kPacket << false;
		pServerNode->SendMessage( kPacket );
		return;
	}

	DWORD dwGoodsNo = 0;
	ioHashString szGoodsName;
	g_GoodsMgr.GetGoodsInfoPresentMileage( iPresentType, iValue1, iValue2, bPresent, dwGoodsNo, szGoodsName );

	char szReturnValue[WEB_BUFF_SIZE]="";
	ZeroMemory( szReturnValue, WEB_BUFF_SIZE );

	char szTimeStamp[MAX_PATH]="";
	GetTimeStamp( szTimeStamp, sizeof( szTimeStamp ) );

	char szUniteValue[MAX_PATH*2]="";
	StringCbPrintf( szUniteValue, sizeof( szUniteValue ), "%s%s%d%d%d%s%s", szPrivateID.c_str() , szPublicIP.c_str(), 0, dwGoodsNo, iSellPeso, szTimeStamp, JAPAN_MILEAGE_KEY );

	char szHexMD5Key[MAX_PATH]="";
	ZeroMemory( szHexMD5Key, sizeof( szHexMD5Key ) );
	GetHexMD5( szHexMD5Key, sizeof( szHexMD5Key ), szUniteValue );

	char szFullURL[MAX_PATH*4]="";
	StringCbPrintf( szFullURL, sizeof( szFullURL ), "%s?iCN=%s&iUserIP=%s&iItemClass=%d&iItemCode=%d&iMileage=%d&iETC=%s&TT=%s&MD=%s" , m_sMileageAddURL.c_str(), szPrivateID.c_str(), szPublicIP.c_str(), 0, dwGoodsNo, iSellPeso, szGoodsName.c_str(), szTimeStamp, szHexMD5Key );

	if( g_App.IsTestMode() )
	{
		ioHashString szMileagePrivateID;
		char szTempID[MAX_PATH]="";
		StringCbPrintf( szTempID, sizeof( szTempID ), "%s#mileage#", szPrivateID.c_str() );
		szMileagePrivateID = szTempID;

		int iMileage = 0;
		TestCashManager::TestCashInfo *pInfo = m_TestCashManager.GetInfo( szMileagePrivateID );
		if( pInfo )
		{
			m_TestCashManager.CheckNChargeCash( pInfo );
			pInfo->m_iCash = pInfo->m_iCash + iSellPeso;
			iMileage = pInfo->m_iCash;
		}
		else
		{
			m_TestCashManager.AddInfo( szMileagePrivateID );
			TestCashManager::TestCashInfo *pInfo = m_TestCashManager.GetInfo( szMileagePrivateID );
			if( pInfo )
			{
				pInfo->m_iCash = pInfo->m_iCash + iSellPeso;
				iMileage = pInfo->m_iCash;
			}
		}

		StringCbPrintf( szReturnValue, WEB_BUFF_SIZE,  "<?xml version=\"1.0\" encoding=\"utf-8\" ?>	<mileage> <p_MID>12345678901234567890</p_MID> <p_message>1</p_message> <p_mileage>%d</p_mileage> <p_addMileage>%d</p_addMileage> </mileage>", iMileage, iSellPeso );
	}
	else
	{	
		ioHTTP Winhttp; //kyg 확인 필요 
		if( !Winhttp.GetResultData( szFullURL, "GET", szReturnValue, WEB_BUFF_SIZE ) )
		{
			LOG.PrintTimeAndLog( 0, "%s Http Error: %d:%s:%d", __FUNCTION__, dwUserIndex, szPrivateID.c_str(), GetLastError() );
			SP2Packet kPacket( BSTPK_ADD_MILEAGE_RESULT );
			kPacket << dwUserIndex;
			kPacket << "Dummy";
			kPacket << BILLING_ADD_MILEAGE_RESULT_FAIL;
			kPacket << false;
			pServerNode->SendMessage( kPacket );
			g_LogDBClient.OnInsertBillingServerError( CNT_WEMADEBUY, BILLING_ERROR_LOG_EXCEPTION,"Billing exception OnAddMileage(HTTP)." );
			return;
		}
	}
	
	if( strcmp( szReturnValue , "" ) == 0 )
	{
		LOG.PrintTimeAndLog( 0, "%s Return Empty: %d:%s:%d", __FUNCTION__, dwUserIndex, szPrivateID.c_str(), GetLastError() );
		SP2Packet kPacket( BSTPK_ADD_MILEAGE_RESULT );
		kPacket << dwUserIndex;
		kPacket << "Dummy";
		kPacket << BILLING_ADD_MILEAGE_RESULT_FAIL;
		kPacket << false;
		pServerNode->SendMessage( kPacket );
		g_LogDBClient.OnInsertBillingServerError( CNT_WEMADEBUY, BILLING_ERROR_LOG_EXCEPTION,"Billing exception OnAddMileage(ReturnEmpty)." );
		return;
	}

	ioXMLDocument xmlDoc;
	if( !xmlDoc.LoadFromMemory( szReturnValue ) )
	{
		SP2Packet kPacket( BSTPK_ADD_MILEAGE_RESULT );
		kPacket << dwUserIndex;
		kPacket << "Dummy";
		kPacket << BILLING_ADD_MILEAGE_RESULT_FAIL;
		kPacket << false;
		pServerNode->SendMessage( kPacket );
		LOG.PrintTimeAndLog(0, "%s Fail ioXMLDocument::LoadFromMemory :%d:%s", __FUNCTION__, dwUserIndex, szPrivateID.c_str() );
		return;
	}

	ioXMLElement xRootElement = xmlDoc.GetRootElement();
	if( xRootElement.IsEmpty() )	
	{
		SP2Packet kPacket( BSTPK_ADD_MILEAGE_RESULT );
		kPacket << dwUserIndex;
		kPacket << "Dummy";
		kPacket << BILLING_ADD_MILEAGE_RESULT_FAIL;
		kPacket << false;
		pServerNode->SendMessage( kPacket );
		LOG.PrintTimeAndLog(0, "%s ioXMLElement is Empty.:%d:%s", __FUNCTION__, dwUserIndex, szPrivateID.c_str() );
		return;
	}	

/*
게임에서 아이템을 매각했을때 받은 마일리지를 삭제한다.
p_MID(varchar(20)) : 등록로그번호
p_message (int) : 처리결과
1  : 등록성공
-2 : 유저정보 등록에러
-3 : 로그번호 생성에러
-7 : 로그 등록에러
-99 : 파라메터 에러

p_mileage(int) :등록후 마일리지
p_addMileage (int) :더해진 마일리지

? <?xml version="1.0" encoding="utf-8" ?> 
  <mileage>
  <p_MID>12345678901234567890</p_MID>
  <p_message >1</ p_message >
  <p_mileage>25000</p_mileage>
  < p_addMileage >300</p_addMileage >
  </mileage>
*/	

	// parsing
	int iTotalMileage  = 0;
	int iAddMileage    = 0;
	int iReturnValue   = 0;
	char szLogNum[MAX_PATH]="";
	ioXMLElement xChildElement = xRootElement.FirstChild( "p_MID" );
	if( !xChildElement.IsEmpty() && xChildElement.GetText() != NULL )
		StringCbCopy( szLogNum, sizeof( szLogNum ), xChildElement.GetText() );

	xChildElement = xRootElement.FirstChild( "p_message" );
	if( !xChildElement.IsEmpty() && xChildElement.GetText() != NULL )
		iReturnValue = atoi( xChildElement.GetText() );

	xChildElement = xRootElement.FirstChild( "p_mileage" );
	if( !xChildElement.IsEmpty() && xChildElement.GetText() != NULL )
		iTotalMileage = atoi( xChildElement.GetText() );

	xChildElement = xRootElement.FirstChild( "p_addMileage" );
	if( !xChildElement.IsEmpty() && xChildElement.GetText() != NULL )
		iAddMileage = atoi( xChildElement.GetText() );

	if( iReturnValue != 1 )
	{
		SP2Packet kPacket( BSTPK_ADD_MILEAGE_RESULT );
		kPacket << dwUserIndex;
		kPacket << "Dummy";
		kPacket << BILLING_ADD_MILEAGE_RESULT_FAIL;
		kPacket << true;
		char szError[MAX_PATH]="";
		StringCbPrintf( szError, sizeof( szError ), "CJIJ Error Code : %d", iReturnValue );
		kPacket << szError;
		pServerNode->SendMessage( kPacket );
		g_LogDBClient.OnInsertJapanMileageLog( dwUserIndex, szPrivateID, szPublicID, szPublicIP, iPresentType, iValue1, iValue2, bPresent, szLogNum, iAddMileage, iReturnValue );
		LOG.PrintTimeAndLog( 0, "%s ReturnError %s:%d:%d", __FUNCTION__, szPrivateID.c_str(), dwUserIndex, iReturnValue );
		return;
	}

	SP2Packet kPacket( BSTPK_ADD_MILEAGE_RESULT );
	kPacket << dwUserIndex;
	kPacket << "Dummy";
	kPacket << BILLING_ADD_MILEAGE_RESULT_SUCCESS;
	kPacket << iTotalMileage;
	kPacket << iAddMileage;
	pServerNode->SendMessage( kPacket );

	g_LogDBClient.OnInsertJapanMileageLog( dwUserIndex, szPrivateID, szPublicID, szPublicIP, iPresentType, iValue1, iValue2, bPresent, szLogNum, iAddMileage, iReturnValue );
	LOG.PrintTimeAndLog(0, "%s Success:%d:%s:%s:%s:%d:%d:%d:%d:%d:[%d:%d]", __FUNCTION__, dwUserIndex, szPrivateID.c_str(), szPublicID.c_str(), szPublicIP.c_str(), iPresentType, iValue1, iValue2, iSellPeso, (int)bPresent, iTotalMileage, iAddMileage );
}


void ioLocalJapan::GetTimeStamp( OUT char *szTimeStamp, IN int iTimeStampSize )
{
	SYSTEMTIME st;
	GetLocalTime( &st );

	char szTemp[MAX_PATH]="";
	StringCbPrintf( szTemp, sizeof( szTemp ), "%04d", st.wYear );
	StringCbCat( szTimeStamp, iTimeStampSize, szTemp );
	StringCbPrintf( szTemp, sizeof( szTemp ), "%02d", st.wMonth );
	StringCbCat( szTimeStamp, iTimeStampSize, szTemp );
	StringCbPrintf( szTemp, sizeof( szTemp ), "%02d", st.wDay );
	StringCbCat( szTimeStamp, iTimeStampSize, szTemp );
	StringCbPrintf( szTemp, sizeof( szTemp ), "%02d", st.wHour );
	StringCbCat( szTimeStamp, iTimeStampSize, szTemp );
	StringCbPrintf( szTemp, sizeof( szTemp ), "%02d", st.wMinute );
	StringCbCat( szTimeStamp, iTimeStampSize, szTemp );
	StringCbPrintf( szTemp, sizeof( szTemp ), "%02d", st.wSecond );
	StringCbCat( szTimeStamp, iTimeStampSize, szTemp );
}

int ioLocalJapan::GetJapanBillingCode( DWORD dwGoodsNo )
{
	int iValue  = dwGoodsNo / 100000000;
	int iValue2 = ( dwGoodsNo % 10000 ) / 1000;
	int iValue3 = ( dwGoodsNo % 10000000 ) / 1000000;

	if( iValue == 1 )
		return 464;
	else if( iValue == 2 )
	{
		if( iValue3 == 1 )
			return 467;
		else if( iValue2 == 1 || iValue2 == 2 || iValue2 == 3 || iValue2 == 4 || iValue2 == 7 )
			return 465;
		else if( iValue2 == 5 )
			return 466;
	}
	else if( iValue == 3  )
		return 468;
	else if( iValue == 11 )
		return 469;
	else if( iValue == 12 )
	{
		if( iValue3 == 1 )
			return 472;
		else if( iValue2 == 1 || iValue2 == 2 || iValue2 == 3 || iValue2 == 4 || iValue2 == 7 )
			return 470;
		else if( iValue2 == 5 )
			return 471;
	}
	else if( iValue == 13  )
		return 473;

	return 0;
}
