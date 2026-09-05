#include "../stdafx.h"
#include "../NodeInfo/ServerNode.h"
#include "../xml/ioxmldocument.h"
#include "../xml/ioxmlelement.h"
#include "../database/logdbclient.h"
#include "../MainProcess.h"
#include <strsafe.h>
#include "../Util/ioHashString.h"
#include "../ThreadPool/ioThreadPool.h"
#include "../NodeInfo/ServerNodeManager.h"
#include "../../../extra/LS_HTTP/LS_HTTP/ioHTTP.h"
#include "ioChannelingNodeHangame.h"

extern CLog LOG;

ioChannelingNodeHangame::ioChannelingNodeHangame()
{
	Init();
}

ioChannelingNodeHangame::~ioChannelingNodeHangame()
{
	Destroy();
}

void ioChannelingNodeHangame::Init()
{
	char szURL[MAX_PATH]="";

	ioINILoader kLoader( "ls_config_billingsvr.ini" );
	kLoader.SetTitle( "Channeling" );

	kLoader.LoadString( "HangameGetURL", "", szURL, sizeof( szURL ) );
	m_szGetURL = szURL;

	kLoader.LoadString( "HangameOutPutURL", "", szURL, sizeof( szURL ) );
	m_szBuyURL = szURL;
}

void ioChannelingNodeHangame::Destroy()
{
}

void ioChannelingNodeHangame::_OnGetCash( ServerNode *pServerNode, SP2Packet &rkPacket )
{
	int          iChannelingType   = 0;
	DWORD        dwUserIndex       = 0;
	bool         bSetUserMouse     = false;

	ioHashString szPrivateID;
	ioHashString szPublicID;
	ioHashString szBillingGUID;
	ioHashString szUserKey;

	//LOG.PrintTimeAndLog( 0, "[test][hangame]getcash Start" ); 
	rkPacket >> iChannelingType >> szBillingGUID >> dwUserIndex >> szPrivateID >> szPublicID >> bSetUserMouse; // 공통사항
	rkPacket >> szUserKey; // 한게임.

	ioData kData;
	kData.SetChannelingType( iChannelingType );
	kData.SetBillingGUID( szBillingGUID );
	kData.SetUserIndex( dwUserIndex );
	kData.SetPrivateID( szPrivateID );
	kData.SetPublicID( szPublicID );
	kData.SetSetUserMouse( bSetUserMouse );
	kData.SetUserNo( szUserKey );
	if( pServerNode )
	{
		kData.SetServerIP( pServerNode->GetIP() );
		kData.SetServerPort( pServerNode->GetClientPort() );
	}
	kData.SetEmpty( false );
	kData.SetProcessType( ioData::PT_GET_CASH );

	g_ThreadPool.SetData( kData );

	//LOG.PrintTimeAndLog( 0, "[test][hangame]getcash Push END" );
}

void ioChannelingNodeHangame::TestGetURL()
{
	ioData kData;
	kData.SetChannelingType( CNT_HANGAME );

	kData.SetUserIndex( 1 );
	kData.SetPrivateID( "kk" );
	kData.SetPublicID( "kk" );
	kData.SetSetUserMouse( 1 );
	kData.SetUserNo( "bta_1512_012" );
	/*if( pServerNode )
	{
		kData.SetServerIP( pServerNode->GetIP() );
		kData.SetServerPort( pServerNode->GetClientPort() );
	}*/
	kData.SetEmpty( false );
	kData.SetProcessType( ioData::PT_GET_CASH );

	g_ThreadPool.SetData( kData );
}

void ioChannelingNodeHangame::ThreadGetCash( const ioData &rData )
{
	//LOG.PrintTimeAndLog( 0, "[test][hangame]Call get cash thread" );
	if( rData.IsEmpty() )
	{
		LOG.PrintTimeAndLog( 0, "[error][hangame]%s Data is Empty.", __FUNCTION__ );
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
			LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:%s", "ioChannelingNodeHangame::ThreadGetCash", rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str() );
		}
		return;
	}

	long iReturnCash    = 0;
	long iPurchasedCash = 0; // 현금을 주고 구매한 캐쉬
	char szReturnValue[WEB_BUFF_SIZE]="";
	ZeroMemory( szReturnValue, WEB_BUFF_SIZE );

	char szFullURL[MAX_PATH*2]="";
	StringCbPrintf( szFullURL, sizeof( szFullURL ), "%s&userkey=%s&cointype=%s" , m_szGetURL.c_str(), rData.GetUserNo().c_str(), HANGAME_COIN_TYPE );

	//LOG.PrintTimeAndLog( 0, "[test][hangame]%s", szFullURL );

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

	ioHTTP Winhttp;
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
			LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:%s", "ioChannelingNodeHangame::ThreadGetCash", rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str() );
		}

		g_LogDBClient.OnInsertBillingServerError( CNT_HANGAME, BILLING_ERROR_LOG_EXCEPTION,"Billing exception hangame." );
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
			LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:%s", "ioChannelingNodeHangame::ThreadGetCash", rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str() );
		}

		LOG.PrintTimeAndLog(0, "%s Fail ioXMLDocument::LoadFromMemory :%d:%s:%s:%s:%s:%d", __FUNCTION__, rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetPublicID().c_str(), rData.GetUserNo().c_str(), GetLastError() );
		LOG.PrintTimeAndLog(0, "%s Fail ReturnValue :%s", __FUNCTION__, szReturnValue );
		g_LogDBClient.OnInsertBillingServerError( CNT_HANGAME, BILLING_ERROR_LOG_EXCEPTION,"Billing exception hangame." );
		return;
	}

	//LOG.PrintTimeAndLog( 0, "[test][hangame]%s", szReturnValue );

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
			LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:%s", "ioChannelingNodeHangame::ThreadGetCash", rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str() );
		}

		LOG.PrintTimeAndLog(0, "%s Fail ReturnValue :%s", __FUNCTION__, szReturnValue );
		return;
	}

	//LOG.PrintTimeAndLog( 0, "[test][hangame]Get Result" );

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
				LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:%s", "ioChannelingNodeHangame::ThreadGetCash", rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str() );
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
			LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:%s", "ioChannelingNodeHangame::ThreadGetCash", rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str() );
		}

		LOG.PrintTimeAndLog(0, "[warning][hangame]Get cash fail : [%d][%d]", rData.GetUserIndex(), GetLastError() );
		LOG.PrintTimeAndLog(0, "[warning][hangame]Get cash send value : [%s]", szFullURL);
		LOG.PrintTimeAndLog(0, "[warning][hangame]Get cash return value : [%s]", szReturnValue );
		return;
	}

	// xml parsing
	enum { MAX_ENTRY = 4,};
	bool bKeyCheckArray[MAX_ENTRY];
	for (int i = 0; i < MAX_ENTRY ; i++)
		bKeyCheckArray[i] = false;

	ioXMLElement xElement	= xRootElement.FirstChild();
	if( xElement.IsEmpty() )
	{
		SP2Packet kPacket( BSTPK_GET_CASH_RESULT );
		kPacket << rData.GetUserIndex();
		kPacket << rData.GetBillingGUID();
		kPacket << rData.GetSetUserMouse();
		kPacket << CASH_RESULT_EXCEPT;
		    
		if ( !g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket ) )
		{
			LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:%s", "ioChannelingNodeHangame::ThreadGetCash", rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str() );
		}

		LOG.PrintTimeAndLog(0, "%s xml element is empty :%d:%s:%s:%s:%s:%d", __FUNCTION__, rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetPublicID().c_str(), rData.GetUserNo().c_str(), GetLastError() );
		return;
	}

	xElement	 = xElement.FirstChild();

	for (int i = 0; i < MAX_ENTRY ; i++)
	{
		szTagName = xElement.GetTagName();

		if( szTagName == "entry" )
		{
			ioHashString szKey = xElement.GetStringAttribute( "key" );

			bool bOk = false;
			if( szKey == "CHRG1AMT" )
			{
				bOk = true;
				bKeyCheckArray[0] = true;
			}
			else if( szKey == "CHRG2AMT" )
			{
				bOk = true;
				bKeyCheckArray[1] = true;
			}
			else if( szKey == "CHRG3AMT" )
			{
				bOk = true;
				bKeyCheckArray[2] = true;
			}
			else if( szKey == "CHRG4AMT" )
			{
				bOk = true;
				bKeyCheckArray[3] = true;
			}

			if( bOk )
			{
				ioHashString szValue = xElement.GetStringAttribute( "value" );
				iReturnCash += atoi( szValue.c_str() );
			}
		}

		xElement = xElement.NextSibling();
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
			LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:%s", "ioChannelingNodeHangame::ThreadGetCash", rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str() );
		}

		LOG.PrintTimeAndLog(0, "%s xml data error. :%d:%s:%s:%s:%s:%d", __FUNCTION__, rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetPublicID().c_str(), rData.GetUserNo().c_str(), GetLastError() );
		LOG.PrintTimeAndLog(0, "%s Fail ReturnValue :%s", __FUNCTION__, szReturnValue );
		return;
	}

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
		LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:%s", "ioChannelingNodeHangame::ThreadGetCash", rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str() );
		return;
	}
	LOG.PrintTimeAndLog( 0, "%s Success: %d:%s:PrivateID %s:%s:%s[%d:%d]", __FUNCTION__, rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetPublicID().c_str(), rData.GetUserNo().c_str(), iReturnCash, iPurchasedCash  );
	//LOG.PrintTimeAndLog(0, "%s Success ReturnValue :%s", __FUNCTION__, szReturnValue );
}

void ioChannelingNodeHangame::_OnOutputCash( ServerNode *pServerNode, SP2Packet &rkPacket, DWORD dwGoodsNo, const ioHashString &rszGoodsName )
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

	ioHashString szUserKey;
	rkPacket >> szUserKey; // 한게임 용 

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
	kData.SetUserNo( szUserKey );
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

void ioChannelingNodeHangame::ThreadOutputCash( const ioData &rData )
{
	if( rData.IsEmpty() )
	{
		LOG.PrintTimeAndLog( 0, "[error][hangame]%s Data is Empty.", __FUNCTION__ );
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
			LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:%s  Ret %d:%s:%d", "ioChannelingNodeHangame::ThreadOutputCash", rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetGoodsNo(), rData.GetGoodsName().c_str(), rData.GetItemPayAmt() );
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
	StringCbPrintf( szFullURL, sizeof( szFullURL ), "%s?cointype=%s&userkey=%s&chnl=%s&payamt=%d&cpid=%s&itemid=%d", m_szBuyURL.c_str(), HANGAME_COIN_TYPE, rData.GetUserNo().c_str(), HANGAME_CHNL, rData.GetItemPayAmt(), HANGAME_CPID, rData.GetGoodsNo());
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
			LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:%s  Ret %d:%s:%d", "ioChannelingNodeHangame::ThreadOutputCash", rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetGoodsNo(), rData.GetGoodsName().c_str(), rData.GetItemPayAmt() );
		}
		g_LogDBClient.OnInsertBillingServerError( CNT_HANGAME, BILLING_ERROR_LOG_EXCEPTION,"Billing exception hangame." );
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
			LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:%s  Ret %d:%s:%d", "ioChannelingNodeHangame::ThreadOutputCash", rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetGoodsNo(), rData.GetGoodsName().c_str(), rData.GetItemPayAmt() );
		}
		LOG.PrintTimeAndLog(0, "%s Fail ioXMLDocument::LoadFromMemory :%d:%s:%s:%s:%s:%d", __FUNCTION__, rData.GetUserIndex(), rData.GetBillingGUID().c_str(),rData.GetPrivateID().c_str(), rData.GetPublicID().c_str(), rData.GetUserNo().c_str(), GetLastError() );
		LOG.PrintTimeAndLog(0, "%s Fail ReturnValue :%s", __FUNCTION__, szReturnValue );
		g_LogDBClient.OnInsertBillingServerError( CNT_HANGAME, BILLING_ERROR_LOG_EXCEPTION,"Billing exception hangame." );
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
			LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:%s  Ret %d:%s:%d", "ioChannelingNodeHangame::ThreadOutputCash", rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetGoodsNo(), rData.GetGoodsName().c_str(), rData.GetItemPayAmt() );
		}
		LOG.PrintTimeAndLog(0, "[error][hangame]%s ioXMLElement is Empty. :%d:%s:%s:%s:%s:%d", __FUNCTION__, rData.GetUserIndex(), rData.GetBillingGUID().c_str(),rData.GetPrivateID().c_str(), rData.GetPublicID().c_str(), rData.GetUserNo().c_str(), GetLastError() );
		LOG.PrintTimeAndLog(0, "%s Fail ReturnValue :%s", __FUNCTION__, szReturnValue );
		return;
	}

	ioHashString szTagName = xRootElement.GetTagName();
	szTagName.MakeLower();
	if( szTagName == "error" )
	{
		int iErrorCode	= 0;
		xRootElement = xRootElement.FirstChild("code");

		if( !xRootElement.IsEmpty() && xRootElement.GetText() != NULL )
		{
			iErrorCode	= atoi(xRootElement.GetText());
		}

		SP2Packet kPacket( BSTPK_OUTPUT_CASH_RESULT );
		kPacket << rData.GetUserIndex();
		kPacket << rData.GetBillingGUID();
		kPacket << CASH_RESULT_EXCEPT;
		kPacket << rData.GetExp();
		kPacket << rData.GetItemType();
			  
		if ( !g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket ) )
		{
			LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:%s  Ret %d:%s:%d", "ioChannelingNodeHangame::ThreadOutputCash", rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetGoodsNo(), rData.GetGoodsName().c_str(), rData.GetItemPayAmt() );
		}

		LOG.PrintTimeAndLog(0, "%s xml error return. :%d:%s:%s:%s:%s:%d[%d:%s:%d]", __FUNCTION__, rData.GetUserIndex(), rData.GetBillingGUID().c_str(),rData.GetPrivateID().c_str(), rData.GetPublicID().c_str(), rData.GetUserNo().c_str(), GetLastError(), rData.GetGoodsNo(), szItemName, rData.GetItemPayAmt() );
		LOG.PrintTimeAndLog(0, "%s Fail SendValue :%s", __FUNCTION__, szFullURL);
		LOG.PrintTimeAndLog(0, "%s Fail ReturnValue :%s errorCode : %d", __FUNCTION__, szReturnValue, iErrorCode);
		return;
	}

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
			LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:%s  Ret %d:%s:%d", "ioChannelingHangame::ThreadOutputCash", rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetGoodsNo(), rData.GetGoodsName().c_str(), rData.GetItemPayAmt() );
		}
		LOG.PrintTimeAndLog(0, "[warning][hangame]goods buy fail :%d:%s:%s:%s:%d[%d:%s:%d]", rData.GetUserIndex(), rData.GetBillingGUID().c_str(),rData.GetPrivateID().c_str(), rData.GetPublicID().c_str(), GetLastError(), rData.GetGoodsNo(), szItemName, rData.GetItemPayAmt() );
		LOG.PrintTimeAndLog(0, "[warning][hangame]return value :%s", szReturnValue );
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
		LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:%s  Ret %d:%s:%d", "ioChannelingNodeHangame::ThreadOutputCash", rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetGoodsNo(), rData.GetGoodsName().c_str(), rData.GetItemPayAmt() );
		return;
	}

	LOG.PrintTimeAndLog(0, "[info][hangame]goods buy success. :%d:%s:PrivateID %s:%s:%s[%d:%s:%d:%d:%d:%s]", rData.GetUserIndex(), rData.GetBillingGUID().c_str(),rData.GetPrivateID().c_str(), rData.GetPublicID().c_str(), rData.GetUserNo().c_str(), rData.GetGoodsNo(), szItemName, rData.GetItemPayAmt(), iReturnCash, iPurchasedCash, szReturnBuyNO.c_str() );
}