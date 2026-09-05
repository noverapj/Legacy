#include "../stdafx.h"
#include "../NodeInfo/ServerNode.h"
#include "../xml/ioxmldocument.h"
#include "../xml/ioxmlelement.h"
#include "./ioChannelingNodeNaver.h"
#include "../database/logdbclient.h"
#include "../MainProcess.h"
#include <strsafe.h>
#include "../Util/ioHashString.h"
#include "../ThreadPool/ioThreadPool.h"
#include "../NodeInfo/ServerNodeManager.h"
#include "../../../extra/LS_HTTP/LS_HTTP/ioHTTP.h"


extern CLog LOG;

ioChannelingNodeNaver::ioChannelingNodeNaver(void)
{
	char szNaverURL[MAX_PATH]="";
	ioINILoader kLoader( "ls_config_billingsvr.ini" );
	kLoader.SetTitle( "Channeling" );
	kLoader.LoadString( "NaverURL", "", szNaverURL, sizeof( szNaverURL ) );
	m_sURL = szNaverURL;
}

ioChannelingNodeNaver::~ioChannelingNodeNaver(void)
{
}

ChannelingType ioChannelingNodeNaver::GetType()
{
	return CNT_NAVER;
}

void ioChannelingNodeNaver::_OnGetCash( ServerNode *pServerNode, SP2Packet &rkPacket )
{
	int          iChannelingType   = 0;
	ioHashString szBillingGUID;
	DWORD        dwUserIndex       = 0;
	ioHashString szPrivateID;
	ioHashString szPublicID;
	bool         bSetUserMouse     = false;
	ioHashString szUserNo;

	rkPacket >> iChannelingType >> szBillingGUID >> dwUserIndex >> szPrivateID >> szPublicID >> bSetUserMouse; // 공통사항
	rkPacket >> szUserNo; // naver용

	ioData kData;
	kData.SetChannelingType( iChannelingType );
	kData.SetBillingGUID( szBillingGUID );
	kData.SetUserIndex( dwUserIndex );
	kData.SetPrivateID( szPrivateID );
	kData.SetPublicID( szPublicID );
	kData.SetSetUserMouse( bSetUserMouse );
	kData.SetUserNo( szUserNo );
	if( pServerNode )
	{
		kData.SetServerIP( pServerNode->GetIP() );
		kData.SetServerPort( pServerNode->GetClientPort() );
	}
	kData.SetEmpty( false );
	kData.SetProcessType( ioData::PT_GET_CASH );

	g_ThreadPool.SetData( kData );
}

void ioChannelingNodeNaver::_OnOutputCash( ServerNode *pServerNode, SP2Packet &rkPacket, DWORD dwGoodsNo, const ioHashString &rszGoodsName )
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

	ioHashString szUserNo;
	rkPacket >> szUserNo; // naver 용 

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
	kData.SetUserNo( szUserNo );
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

void ioChannelingNodeNaver::ThreadGetCash( const ioData &rData )
{
	if( rData.IsEmpty() )
	{
		LOG.PrintTimeAndLog( 0, "[error][naver]%s Data is Empty.", __FUNCTION__ );
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
			LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:%s", "ioChannelingNodeNaver::ThreadGetCash", rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str() );
		}
		return;
	}

	long iReturnCash    = 0;
	long iPurchasedCash = 0; // 현금을 주고 구매한 캐쉬
	char szReturnValue[WEB_BUFF_SIZE]="";
	ZeroMemory( szReturnValue, WEB_BUFF_SIZE );

	char szFullURL[MAX_PATH*2]="";
	StringCbPrintf( szFullURL, sizeof( szFullURL ), "%s/queryCoin.xml?chnl=%s&userkey=%s&cointype=%s" , m_sURL.c_str(),NAVER_CHANNEL_INFO,rData.GetUserNo().c_str(), NAVER_COIN_TYPE );

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
	

	ioHTTP Winhttp; //kyg 확인 필요 ;
	if( !Winhttp.GetResultData( szFullURL, "GET", szReturnValue, WEB_BUFF_SIZE ) )
	{
		LOG.PrintTimeAndLog( 0, "%s Http Error: %d:%s:%s:%s:%s:%d", __FUNCTION__, rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetPublicID().c_str(), rData.GetUserNo().c_str(), GetLastError() );
		SP2Packet kPacket( BSTPK_GET_CASH_RESULT );
		kPacket << rData.GetUserIndex();
		kPacket << rData.GetBillingGUID();
		kPacket << rData.GetSetUserMouse();
		kPacket << CASH_RESULT_EXCEPT;
		
		if ( !g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket ) )
		{
			LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:%s", "ioChannelingNodeNaver::ThreadGetCash", rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str() );
		}
		g_LogDBClient.OnInsertBillingServerError( CNT_NAVER, BILLING_ERROR_LOG_EXCEPTION,"Billing exception naver." );
		return;
	}

	ioXMLDocument xmlDoc;
	if( !xmlDoc.LoadFromMemory( szReturnValue ) )
	{
		SP2Packet kPacket( BSTPK_GET_CASH_RESULT );
		kPacket << rData.GetUserIndex();
		kPacket << rData.GetBillingGUID();
		kPacket << rData.GetSetUserMouse();
		kPacket << CASH_RESULT_EXCEPT;
			  
		if ( !g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket ) )
		{
			LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:%s", "ioChannelingNodeNaver::ThreadGetCash", rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str() );
		}

		LOG.PrintTimeAndLog(0, "%s Fail ioXMLDocument::LoadFromMemory :%d:%s:%s:%s:%s:%d", __FUNCTION__, rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetPublicID().c_str(), rData.GetUserNo().c_str(), GetLastError() );
		LOG.PrintTimeAndLog(0, "%s Fail ReturnValue :%s", __FUNCTION__, szReturnValue );
		g_LogDBClient.OnInsertBillingServerError( CNT_NAVER, BILLING_ERROR_LOG_EXCEPTION,"Billing exception naver." );
		return;
	}

	ioXMLElement xRootElement = xmlDoc.GetRootElement();
	if( xRootElement.IsEmpty() )	
	{
		SP2Packet kPacket( BSTPK_GET_CASH_RESULT );
		kPacket << rData.GetUserIndex();
		kPacket << rData.GetBillingGUID();
		kPacket << rData.GetSetUserMouse();
		kPacket << CASH_RESULT_EXCEPT;
			  
		if ( !g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket ) )
		{
			LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:%s", "ioChannelingNodeNaver::ThreadGetCash", rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str() );
		}
		LOG.PrintTimeAndLog(0, "[error][naver]%s ioXMLElement is Empty. :%d:%s:%s:%s:%s:%d", __FUNCTION__, rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetPublicID().c_str(), rData.GetUserNo().c_str(), GetLastError() );
		LOG.PrintTimeAndLog(0, "%s Fail ReturnValue :%s", __FUNCTION__, szReturnValue );
		return;
	}

/*
	// 계좌가 없는 유저 이므로 캐쉬 0으로 셋팅 : 네이버 요청 사항
	<?xml version="1.0" encoding="UTF-8"?>
	<error version="1.0">
	<code>404</code>
	<message>not exist</message>
	</error>
*/
	ioHashString szTagName = xRootElement.GetTagName();
	szTagName.MakeLower();
	if( szTagName == "error" )
	{
		int iErrorCode = 0;
		ioXMLElement xChildElement = xRootElement.FirstChild( "code" );
		if( !xChildElement.IsEmpty() && xChildElement.GetText() != NULL )
			iErrorCode = atoi( xChildElement.GetText() );
		if( iErrorCode == 404 )
		{
			SP2Packet kPacket( BSTPK_GET_CASH_RESULT );
			kPacket << rData.GetUserIndex();
			kPacket << rData.GetBillingGUID();
			kPacket << rData.GetSetUserMouse();
			kPacket << CASH_RESULT_SUCCESS;
			kPacket << 0;
			kPacket << 0;
			if ( !g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket ) )
			{
				LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:%s", "ioChannelingNodeNaver::ThreadGetCash", rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str() );
			}
			return;
		}

		SP2Packet kPacket( BSTPK_GET_CASH_RESULT );
		kPacket << rData.GetUserIndex();
		kPacket << rData.GetBillingGUID();
		kPacket << rData.GetSetUserMouse();
		kPacket << CASH_RESULT_EXCEPT;
		    
		if ( !g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket ) )
		{
			LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:%s", "ioChannelingNodeNaver::ThreadGetCash", rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str() );
		}
		LOG.PrintTimeAndLog(0, "%s xml error return. :%d:%s:%s:%s:%s:%d", __FUNCTION__, rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetPublicID().c_str(), rData.GetUserNo().c_str(), GetLastError() );
		LOG.PrintTimeAndLog(0, "%s Fail SendValue :%s", __FUNCTION__, szFullURL);
		LOG.PrintTimeAndLog(0, "%s Fail ReturnValue :%s", __FUNCTION__, szReturnValue );
		return;
	}
/*
// 성공
<?xml version="1.0" encoding="UTF-8"?>
<response version="1.0"> 
	<entries> 
		<entry key="CHRG1AMT" value="38067"/> 
		<entry key="CHRG2AMT" value="0"/>
		<entry key="CHRG3AMT" value="0"/>
		<entry key="CHRG4AMT" value="0"/>
	</entries> 
</response>

// 실패
<?xml version="1.0" encoding="UTF-8"?>
<error version="1.0">
	<code>403</code> 
	<message>Forbidden</message>
</error>
*/
	// xml parsing
	enum { MAX_ENTRY = 4,};
	bool bKeyCheckArray[MAX_ENTRY];
	for (int i = 0; i < MAX_ENTRY ; i++)
		bKeyCheckArray[i] = false;

	ioXMLElement xChildElement = xRootElement.FirstChild( "entries" );
	ioXMLElement xGrandChildElement;
	if( !xChildElement.IsEmpty() )
		xGrandChildElement = xChildElement.FirstChild( "entry" );
	for (int i = 0; i < MAX_ENTRY ; i++)
	{
		if( xGrandChildElement.IsEmpty() )
			break;
		szTagName = xGrandChildElement.GetTagName();
		if( szTagName == "entry" )
		{
			ioHashString szKey = xGrandChildElement.GetStringAttribute( "key" );
			szKey.MakeLower();

			bool bOk = false;
			if( szKey == "chrg1amt" )
			{
				bOk = true;
				bKeyCheckArray[0] = true;
			}
			else if( szKey == "chrg2amt" )
			{
				bOk = true;
				bKeyCheckArray[1] = true;
			}
			else if( szKey == "chrg3amt" )
			{
				bOk = true;
				bKeyCheckArray[2] = true;
			}
			else if( szKey == "chrg4amt" )
			{
				bOk = true;
				bKeyCheckArray[3] = true;
			}

			if( bOk )
			{
				ioHashString szValue = xGrandChildElement.GetStringAttribute( "value" );
				iReturnCash += atoi( szValue.c_str() );
			}
		}
		xGrandChildElement = xGrandChildElement.NextSibling();
	}


	bool bError = false;
	for (int i = 0; i < MAX_ENTRY ; i++)
	{
		if( !bKeyCheckArray[i] )
		{
			bError = true;
			break;
		}
	}

	if( bError )
	{
		SP2Packet kPacket( BSTPK_GET_CASH_RESULT );
		kPacket << rData.GetUserIndex();
		kPacket << rData.GetBillingGUID();
		kPacket << rData.GetSetUserMouse();
		kPacket << CASH_RESULT_EXCEPT;
			  
		if ( !g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket ) )
		{
			LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:%s", "ioChannelingNodeNaver::ThreadGetCash", rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str() );
		}
		LOG.PrintTimeAndLog(0, "%s xml data error. :%d:%s:%s:%s:%s:%d", __FUNCTION__, rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetPublicID().c_str(), rData.GetUserNo().c_str(), GetLastError() );
		LOG.PrintTimeAndLog(0, "%s Fail ReturnValue :%s", __FUNCTION__, szReturnValue );
		return;
	}

	// 캐쉬와 실재 구매한 캐쉬가 동일하다.
	iPurchasedCash = iReturnCash;

	SP2Packet kPacket( BSTPK_GET_CASH_RESULT );
	kPacket << rData.GetUserIndex();
	kPacket << rData.GetBillingGUID();
	kPacket << rData.GetSetUserMouse();
	kPacket << CASH_RESULT_SUCCESS;
	kPacket << iReturnCash;
	kPacket << iPurchasedCash;
	      
	if ( !g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket ) )
	{
		LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:%s", "ioChannelingNodeNaver::ThreadGetCash", rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str() );
		return;
	}
	LOG.PrintTimeAndLog( 0, "%s Success: %d:%s:PrivateID %s:%s:%s[%d:%d]", __FUNCTION__, rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetPublicID().c_str(), rData.GetUserNo().c_str(), iReturnCash, iPurchasedCash  );
	//LOG.PrintTimeAndLog(0, "%s Success ReturnValue :%s", __FUNCTION__, szReturnValue );
}

void ioChannelingNodeNaver::ThreadOutputCash( const ioData &rData )
{
	if( rData.IsEmpty() )
	{
		LOG.PrintTimeAndLog( 0, "[error][naver]%s Data is Empty.", __FUNCTION__ );
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
			LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:%s  Ret %d:%s:%d", "ioChannelingNodeNaver::ThreadOutputCash", rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetGoodsNo(), rData.GetGoodsName().c_str(), rData.GetItemPayAmt() );
		}
		return;
	}

	enum { ITEM_NAME_SIZE = 25, };
	char szItemName[ITEM_NAME_SIZE+1]="";
	StringCbCopyN( szItemName, sizeof( szItemName ), rData.GetGoodsName().c_str(), ITEM_NAME_SIZE );

	// 깨진 한글 삭제
	for (int i = 0; i < ITEM_NAME_SIZE ; i++)
	{
		if( IsDBCSLeadByte( szItemName[i]) )
		{
			i++;
			if( i == ITEM_NAME_SIZE )
				szItemName[i-1]=NULL;
		}
	}

	char szReturnValue[WEB_BUFF_SIZE]="";
	ZeroMemory( szReturnValue, WEB_BUFF_SIZE );

	char szFullURL[MAX_PATH*2]="";
	StringCbPrintf( szFullURL, sizeof( szFullURL ), "%s/useCoin.xml?chnl=%s&cointype=%s&userkey=%s&payamt=%d&cpid=%s&itemid=%d&title=%s" , m_sURL.c_str(), NAVER_CHANNEL_INFO, NAVER_COIN_TYPE, rData.GetUserNo().c_str(), rData.GetItemPayAmt(), NAVER_CP_ID, rData.GetGoodsNo(), szItemName );

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

	ioHTTP Winhttp;
	if( !Winhttp.GetResultData( szFullURL, "GET", szReturnValue, WEB_BUFF_SIZE ) )
	{
		LOG.PrintTimeAndLog( 0, "%s Http Error: %d:%s:%s:%s:%s:%d", __FUNCTION__, rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetPublicID().c_str(), rData.GetUserNo().c_str(), GetLastError() );
		SP2Packet kPacket( BSTPK_OUTPUT_CASH_RESULT );
		kPacket << rData.GetUserIndex();
		kPacket << rData.GetBillingGUID();
		kPacket << CASH_RESULT_EXCEPT;
		kPacket << rData.GetExp();
		kPacket << rData.GetItemType();
			  
		if ( !g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket ) )
		{
			LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:%s  Ret %d:%s:%d", "ioChannelingNodeNaver::ThreadOutputCash", rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetGoodsNo(), rData.GetGoodsName().c_str(), rData.GetItemPayAmt() );
		}
		g_LogDBClient.OnInsertBillingServerError( CNT_NAVER, BILLING_ERROR_LOG_EXCEPTION,"Billing exception naver." );
		return;
	}

	ioXMLDocument xmlDoc;
	if( !xmlDoc.LoadFromMemory( szReturnValue ) )
	{
		SP2Packet kPacket( BSTPK_OUTPUT_CASH_RESULT );
		kPacket << rData.GetUserIndex();
		kPacket << rData.GetBillingGUID();
		kPacket << CASH_RESULT_EXCEPT;
		kPacket << rData.GetExp();
		kPacket << rData.GetItemType();
			  
		if ( !g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket ) )
		{
			LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:%s  Ret %d:%s:%d", "ioChannelingNodeNaver::ThreadOutputCash", rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetGoodsNo(), rData.GetGoodsName().c_str(), rData.GetItemPayAmt() );
		}
		LOG.PrintTimeAndLog(0, "%s Fail ioXMLDocument::LoadFromMemory :%d:%s:%s:%s:%s:%d", __FUNCTION__, rData.GetUserIndex(), rData.GetBillingGUID().c_str(),rData.GetPrivateID().c_str(), rData.GetPublicID().c_str(), rData.GetUserNo().c_str(), GetLastError() );
		LOG.PrintTimeAndLog(0, "%s Fail ReturnValue :%s", __FUNCTION__, szReturnValue );
		g_LogDBClient.OnInsertBillingServerError( CNT_NAVER, BILLING_ERROR_LOG_EXCEPTION,"Billing exception naver." );
		return;
	}

	ioXMLElement xRootElement = xmlDoc.GetRootElement();
	if( xRootElement.IsEmpty() )	
	{
		SP2Packet kPacket( BSTPK_OUTPUT_CASH_RESULT );
		kPacket << rData.GetUserIndex();
		kPacket << rData.GetBillingGUID();
		kPacket << CASH_RESULT_EXCEPT;
		kPacket << rData.GetExp();
		kPacket << rData.GetItemType();
			  
		if ( !g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket ) )
		{
			LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:%s  Ret %d:%s:%d", "ioChannelingNodeNaver::ThreadOutputCash", rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetGoodsNo(), rData.GetGoodsName().c_str(), rData.GetItemPayAmt() );
		}
		LOG.PrintTimeAndLog(0, "[error][naver]%s ioXMLElement is Empty. :%d:%s:%s:%s:%s:%d", __FUNCTION__, rData.GetUserIndex(), rData.GetBillingGUID().c_str(),rData.GetPrivateID().c_str(), rData.GetPublicID().c_str(), rData.GetUserNo().c_str(), GetLastError() );
		LOG.PrintTimeAndLog(0, "%s Fail ReturnValue :%s", __FUNCTION__, szReturnValue );
		return;
	}

	/*
	error code
	1:유효하지 않은 파라미터
	2:유료하지 않은 사용자 정보
	3:유효하지 않은 PG
	4:유효하지 않은 아이템
	5:결제 금액은 "0" 보다 커야함
	6:존재하지 않은 계좌
	7:잔액부족
	8:잔액수정실패
	21:결제 금지 ID( 블랙 리스트 )
	98:기타 에러
	99:DB 에러
	*/
	ioHashString szTagName = xRootElement.GetTagName();
	szTagName.MakeLower();
	if( szTagName == "error" )
	{
		SP2Packet kPacket( BSTPK_OUTPUT_CASH_RESULT );
		kPacket << rData.GetUserIndex();
		kPacket << rData.GetBillingGUID();
		kPacket << CASH_RESULT_EXCEPT;
		kPacket << rData.GetExp();
		kPacket << rData.GetItemType();
			  
		if ( !g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket ) )
		{
			LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:%s  Ret %d:%s:%d", "ioChannelingNodeNaver::ThreadOutputCash", rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetGoodsNo(), rData.GetGoodsName().c_str(), rData.GetItemPayAmt() );
		}
		LOG.PrintTimeAndLog(0, "%s xml error return. :%d:%s:%s:%s:%s:%d[%d:%s:%d]", __FUNCTION__, rData.GetUserIndex(), rData.GetBillingGUID().c_str(),rData.GetPrivateID().c_str(), rData.GetPublicID().c_str(), rData.GetUserNo().c_str(), GetLastError(), rData.GetGoodsNo(), szItemName, rData.GetItemPayAmt() );
		LOG.PrintTimeAndLog(0, "%s Fail SendValue :%s", __FUNCTION__, szFullURL);
		LOG.PrintTimeAndLog(0, "%s Fail ReturnValue :%s", __FUNCTION__, szReturnValue );
		return;
	}


/*
<?xml version="1.0" encoding="UTF-8"?>
<response version=”1.0”>
	<entries> 
		<entry key="rtn" value="0"/> 
		<entry key="status" value="1186654898"/>
		<entry key="amt1" value="1"/> 
		<entry key="amt2" value="0"/>
		<entry key="amt3" value="0"/>
		<entry key="amt4" value="0"/>
		<entry key="minusamt" value="0"/> 
		<entry key="bal1" value="38068"/> 
		<entry key="bal2" value="0"/>
		<entry key="bal3" value="0"/> 
		<entry key="bal4" value="0"/>
		<entry key="minusbal" value="0"/>
	</entries> 
</response>

// 실패
<?xml version="1.0" encoding="UTF-8"?>
<error version="1.0">
	<code>403</code> 
	<message>Forbidden</message>
</error>
*/

	// xml parsing
	enum { MAX_ENTRY = 12,};
	bool bKeyCheckArray[MAX_ENTRY];
	for (int i = 0; i < MAX_ENTRY ; i++)
		bKeyCheckArray[i] = false;

	ioHashString szReturnBuyNO ="";  
	int  iReturnCash = 0;
	bool bError = false;
	ioXMLElement xChildElement = xRootElement.FirstChild( "entries" );
	ioXMLElement xGrandChildElement;
	if( !xChildElement.IsEmpty() )
		xGrandChildElement = xChildElement.FirstChild( "entry" );
	for (int i = 0; i < MAX_ENTRY ; i++)
	{
		if( xGrandChildElement.IsEmpty() )
			break;
		szTagName = xGrandChildElement.GetTagName();
		if( szTagName == "entry" )
		{
			ioHashString szKey = xGrandChildElement.GetStringAttribute( "key" );
			szKey.MakeLower();

			bool bOk = false;
			if( szKey == "rtn" )
			{
				ioHashString szValue = xGrandChildElement.GetStringAttribute( "value" );
				int iRtn = atoi( szValue.c_str() );
				if( iRtn != 0 ) // 리턴값이 0이 아니면 에러
				{
					bError = true;
					break;
				}
				bKeyCheckArray[0] = true;
			}
			else if( szKey == "status" )
			{
				szReturnBuyNO = xGrandChildElement.GetStringAttribute( "value" );
				bKeyCheckArray[1] = true;
			}
			else if( szKey == "amt1" )
				bKeyCheckArray[2] = true;
			else if( szKey == "amt2" )
				bKeyCheckArray[3] = true;
			else if( szKey == "amt3" )
				bKeyCheckArray[4] = true;
			else if( szKey == "amt4" )
				bKeyCheckArray[5] = true;
			else if( szKey == "minusamt" )
				bKeyCheckArray[6] = true;
			else if( szKey == "bal1" )
			{
				bOk = true;
				bKeyCheckArray[7] = true;
			}
			else if( szKey == "bal2" )
			{
				bOk = true;
				bKeyCheckArray[8] = true;
			}
			else if( szKey == "bal3" )
			{
				bOk = true;
				bKeyCheckArray[9] = true;
			}
			else if( szKey == "bal4" )
			{
				bOk = true;
				bKeyCheckArray[10] = true;
			}
			else if( szKey == "minusbal" )
				bKeyCheckArray[11] = true;

			if( bOk )
			{
				ioHashString szValue = xGrandChildElement.GetStringAttribute( "value" );
				iReturnCash += atoi( szValue.c_str() );
			}
		}
		xGrandChildElement = xGrandChildElement.NextSibling();
	}



	for (int i = 0; i < MAX_ENTRY ; i++)
	{
		if( !bKeyCheckArray[i] )
		{
			bError = true;
			break;
		}
	}

	if( bError )
	{
		SP2Packet kPacket( BSTPK_OUTPUT_CASH_RESULT );
		kPacket << rData.GetUserIndex();
		kPacket << rData.GetBillingGUID();
		kPacket << CASH_RESULT_EXCEPT;
		kPacket << rData.GetExp();
		kPacket << rData.GetItemType();
			 
		if ( !g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket ) )
		{
			LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:%s  Ret %d:%s:%d", "ioChannelingNodeNaver::ThreadOutputCash", rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetGoodsNo(), rData.GetGoodsName().c_str(), rData.GetItemPayAmt() );
		}
		LOG.PrintTimeAndLog(0, "%s xml data error. :%d:%s:%s:%s:%d[%d:%s:%d]", __FUNCTION__, rData.GetUserIndex(), rData.GetBillingGUID().c_str(),rData.GetPrivateID().c_str(), rData.GetPublicID().c_str(), GetLastError(), rData.GetGoodsNo(), szItemName, rData.GetItemPayAmt() );
		LOG.PrintTimeAndLog(0, "%s Fail ReturnValue :%s", __FUNCTION__, szReturnValue );
		return;
	}

	// 캐쉬와 실재 구매한 캐쉬가 동일하다.
	int iPurchasedCash = iReturnCash;

	SP2Packet kPacket( BSTPK_OUTPUT_CASH_RESULT );
	kPacket << rData.GetUserIndex();
	kPacket << rData.GetBillingGUID();
	kPacket << CASH_RESULT_SUCCESS;
	kPacket << rData.GetExp();
	kPacket << rData.GetItemType();
	kPacket << rData.GetItemPayAmt();
	kPacket << 0; // TransactionID ( FOR US ) //kyg 여기에 구매 유니크값 szReturnBuyNO
	kPacket << szReturnBuyNO;

	int iItemValueList[MAX_ITEM_VALUE];
	for (int i = 0; i <MAX_ITEM_VALUE; i++)
		iItemValueList[i] = rData.GetItemValue( i );
	SetItemValueList( kPacket, rData.GetItemType(), iItemValueList );
	kPacket << rData.GetChannelingType(); // 공통
	kPacket << iReturnCash;
	kPacket << iPurchasedCash;
	
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
		LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:%s  Ret %d:%s:%d", "ioChannelingNodeNaver::ThreadOutputCash", rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetGoodsNo(), rData.GetGoodsName().c_str(), rData.GetItemPayAmt() );
		return;
	}
	//LOG.PrintTimeAndLog(0, "%s Success ReturnValue :%s", __FUNCTION__, szReturnValue );
	LOG.PrintTimeAndLog(0, "%s Success. :%d:%s:PrivateID %s:%s:%s[%d:%s:%d:%d:%d:%s]", __FUNCTION__, rData.GetUserIndex(), rData.GetBillingGUID().c_str(),rData.GetPrivateID().c_str(), rData.GetPublicID().c_str(), rData.GetUserNo().c_str(), rData.GetGoodsNo(), szItemName, rData.GetItemPayAmt(), iReturnCash, iPurchasedCash, szReturnBuyNO.c_str() );
}


