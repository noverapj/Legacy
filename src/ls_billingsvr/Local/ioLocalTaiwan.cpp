#include "../stdafx.h"
#include "./iolocaltaiwan.h"
#include "../DataBase/LogDBClient.h"
#include "../MainProcess.h"
#include "../NodeInfo/ServerNode.h"
#include "../Channeling/ioChannelingNodeParent.h"
#include "../ThreadPool/ioThreadPool.h"
#include "../NodeInfo/ServerNodeManager.h"
#include "../EtcHelpFunc.h"
#include "../xml/ioxmldocument.h"
#include "../xml/ioxmlelement.h"


extern CLog LOG;

ioLocalTaiwan::ioLocalTaiwan(void)
{
}

ioLocalTaiwan::~ioLocalTaiwan(void)
{
}

ioLocalManager::LocalType ioLocalTaiwan::GetType()
{
	return ioLocalManager::LCT_TAIWAN;
}

void ioLocalTaiwan::Init()
{
	char szTemp[MAX_PATH]="";
	ioINILoader kLoader( "ls_config_billingsvr.ini" );
	kLoader.SetTitle( "Local" );

	kLoader.LoadString( "GamonKey", "", szTemp, sizeof( szTemp ) );
	m_sKey = szTemp;

	kLoader.LoadString( "GamonLoginURL", "", szTemp, sizeof( szTemp ) );
	m_sLoginURL = szTemp;

	kLoader.LoadString( "GamonGetCashURL", "", szTemp, sizeof( szTemp ) );
	m_sBillingGetURL = szTemp;

	kLoader.LoadString( "GamonPaymentURL", "", szTemp, sizeof( szTemp ) );
	m_sBillingOutPutURL = szTemp;

	kLoader.LoadString( "GamonCancelPaymentURL", "", szTemp, sizeof( szTemp ) );
	m_sBillingCancelURL = szTemp;

	if( m_sLoginURL.IsEmpty() || m_sBillingGetURL.IsEmpty() || m_sBillingOutPutURL.IsEmpty() || m_sBillingCancelURL.IsEmpty() || m_sKey.IsEmpty() )
		LOG.PrintTimeAndLog(0,"Error Gamon URL Error ");
}

void ioLocalTaiwan::OnLoginData( ServerNode *pServerNode, SP2Packet &rkPacket )
{
	ioHashString szBillingGUID;
	ioHashString szPrivateID;
	ioHashString sEncodePW;
	ioHashString szPublicIP;
	DWORD        dwReturnMsgType;
	ioHashString szMacAddress;

	rkPacket >> szBillingGUID >> szPrivateID >> sEncodePW >> szPublicIP >> dwReturnMsgType; // 공통
	rkPacket >> szMacAddress; // 대만

	if( !pServerNode )
	{
		LOG.PrintTimeAndLog(0, "%s pServerNode == NULL:%s:%s:%s:%s:%s", __FUNCTION__, szBillingGUID.c_str(), szPrivateID.c_str(), sEncodePW.c_str(), szPublicIP.c_str(), szMacAddress.c_str() );
		return;
	}

	ioData kData;
	kData.SetBillingGUID( szBillingGUID );
	kData.SetPrivateID( szPrivateID );
	kData.SetEncodePW( sEncodePW );
	kData.SetUserIP( szPublicIP );
	kData.SetReturnMsgType( dwReturnMsgType );
	kData.SetUserNo( szMacAddress );
	if( pServerNode )
	{
		kData.SetServerIP( pServerNode->GetIP() );
		kData.SetServerPort( pServerNode->GetClientPort() );
	}
	kData.SetEmpty( false );
	kData.SetProcessType( ioData::PT_LOGIN );

	g_ThreadPool.SetData( kData );
}

void ioLocalTaiwan::_OnGetCash( ServerNode *pServerNode, SP2Packet &rkPacket )
{
	int          iChannelingType   = 0;
	ioHashString szBillingGUID;
	DWORD        dwUserIndex       = 0;
	ioHashString szPrivateID;
	ioHashString szPublicID;
	bool         bSetUserMouse     = false;

	rkPacket >> iChannelingType >> szBillingGUID >> dwUserIndex >> szPrivateID >> szPublicID >> bSetUserMouse; // 공통사항

	if( !pServerNode )
	{
		LOG.PrintTimeAndLog(0, "%s pServerNode == NULL:%s:%d:%s:%s", __FUNCTION__, szBillingGUID.c_str(), dwUserIndex, szPrivateID.c_str(), szPublicID.c_str() );
		return;
	}

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

void ioLocalTaiwan::_OnOutputCash( ServerNode *pServerNode, SP2Packet &rkPacket, DWORD dwGoodsNo, const ioHashString &rszGoodsName )
{
	int          iChannelingType = 0;
	ioHashString szBillingGUID;
	DWORD        dwUserIndex = 0;
	ioHashString szPublicID;
	ioHashString szPrivateID;
	ioHashString szUserIP;
	int          iPayAmt  = 0;
	int          iType    = 0;
	ioHashString szBillingUserKey;
	int          iUserGradeLevel = 0;

	rkPacket >> iChannelingType >> szBillingGUID >> dwUserIndex >> szPublicID >> szPrivateID >> szUserIP >> iPayAmt >> iType; // 공통사항

	int iItemValueList[MAX_ITEM_VALUE];
	for (int i = 0; i < MAX_ITEM_VALUE ; i++)
		iItemValueList[i] = 0;
	ioChannelingNodeParent::GetItemValueList( rkPacket, iType, iItemValueList );

	ioData kData;
	kData.SetChannelingType( iChannelingType );
	kData.SetBillingGUID( szBillingGUID );
	kData.SetUserIndex( dwUserIndex );
	kData.SetPublicID( szPublicID );
	kData.SetPrivateID( szPrivateID );
	kData.SetUserIP( szUserIP );
	kData.SetItemPayAmt( iPayAmt );
	kData.SetItemType( iType );
	kData.SetItemValueList( iItemValueList );
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


void ioLocalTaiwan::OnCancelCash( ServerNode *pServerNode, SP2Packet &rkPacket )
{
	int          iChannelingType = 0;
	ioHashString szBillingGUID;
	DWORD        dwUserIndex = 0;
	ioHashString szPrivateID;
	ioHashString szTransaction;
	
	rkPacket >> iChannelingType >> szBillingGUID >> dwUserIndex ; // 공통사항
	rkPacket >> szPrivateID >> szTransaction; // Taiwan

	ioData kData;
	kData.SetChannelingType( iChannelingType );
	kData.SetBillingGUID( szBillingGUID );
	kData.SetUserIndex( dwUserIndex );
	kData.SetPrivateID( szPrivateID );
	kData.SetUserNo( szTransaction );
	if( pServerNode )
	{
		kData.SetServerIP( pServerNode->GetIP() );
		kData.SetServerPort( pServerNode->GetClientPort() );
	}
	kData.SetEmpty( false );
	kData.SetProcessType( ioData::PT_CANCEL_CASH );

	g_ThreadPool.SetData( kData );
}

void ioLocalTaiwan::ThreadLogin( const ioData &rData, LoginManager &rLoginMgr )
{
	if( rData.IsEmpty() )
	{
		LOG.PrintTimeAndLog( 0, "%s Data is Empty.", __FUNCTION__ );
		return;
	}

	if( g_App.IsReserveLogOut() )
	{
		LOG.PrintTimeAndLog(0, "%s LogOut:%s:%s:%s", __FUNCTION__, rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetEncodePW().c_str() );
		SP2Packet kPacket( rData.GetReturnMsgType() );
		kPacket << rData.GetPrivateID();
		kPacket << rData.GetBillingGUID();
		kPacket << BILLING_LOGIN_RESULT_FAIL;
		kPacket << false;
		g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket );
		return;
	}

	/*if( rLoginMgr.CheckLogin( rData ) )
		return;
*/
	enum { MAX_RANDOM_KEY = 20, };
	char szRandomKey[MAX_PATH]="";
	if( rData.GetEncodePW().Length() > MAX_RANDOM_KEY )
		StringCbCopyN( szRandomKey, sizeof( szRandomKey ), rData.GetEncodePW().c_str(), MAX_RANDOM_KEY );
	char szEncode[MAX_PATH]="";
	int iEncodeCnt = 0;
	int iEncodPwLength = rData.GetEncodePW().Length();
	for (int i = 0; i < iEncodPwLength; i++)
	{
		if( i >= MAX_RANDOM_KEY )
		{
			szEncode[iEncodeCnt] = rData.GetEncodePW().At(i);
			iEncodeCnt++;
			if( iEncodeCnt >= MAX_PATH )
				break;
		}
	}
	char szPW[MAX_PATH]="";
	char szUserKey[MAX_PATH]="";
	StringCbPrintf( szUserKey, sizeof( szUserKey ), "%s%s", rData.GetPrivateID().c_str(), szRandomKey );
	Help::Decode( szEncode, strlen( szEncode ), szPW, sizeof( szPW ), szUserKey, strlen( szUserKey ) );

	char szReturnValue[WEB_BUFF_SIZE]="";
	ZeroMemory( szReturnValue, WEB_BUFF_SIZE );

	char szHexMD5PW[MAX_PATH]="";
	ZeroMemory( szHexMD5PW, sizeof( szHexMD5PW ) );
	GetHexMD5( szHexMD5PW, sizeof( szHexMD5PW ), szPW );

	char szFullURL[MAX_PATH*4]="";
	StringCbPrintf( szFullURL, sizeof( szFullURL ), "%s?user_id=%s&user_pwd=%s&ip_addr=%s&mac_addr=%s&as_code=LOSTSAGA&as_key=%s" , m_sLoginURL.c_str(), rData.GetPrivateID().c_str(), szHexMD5PW, rData.GetUserIP().c_str(), rData.GetUserNo().c_str(), m_sKey.c_str() );

	if( g_App.IsTestMode() )
	{
		/*
		int iArray = -1;
		enum { MAX_LIST = 11, };
		char szUserNoList[MAX_LIST][MAX_PATH]=
		{
			"08071500000000000034",
			"08071500000000000035",
			"08071500000000000036",
			"08071500000000000037",
			"08071500000000000038",
			"08071500000000000039",
			"08071500000000000040",
			"08071500000000000041",
			"08071500000000000042",
			"08071500000000000043",
			"10082200000000012488",
		};
		if( strcmp( rData.GetPrivateID().c_str() ,"derks2007@ioenter.com" ) == 0 )
			iArray = 0;
		else if( strcmp( rData.GetPrivateID().c_str() ,"derks2008@ioenter.com") == 0 )
			iArray = 1;
		else if( strcmp( rData.GetPrivateID().c_str() ,"derks2009@ioenter.com" ) == 0 )
			iArray = 2;
		else if( strcmp( rData.GetPrivateID().c_str() ,"derks2010@ioenter.com" ) == 0 )
			iArray = 3;
		else if( strcmp( rData.GetPrivateID().c_str(),"derks2011@ioenter.com" ) == 0 )
			iArray = 4;						 
		else if( strcmp( rData.GetPrivateID().c_str(),"derks2012@ioenter.com" ) == 0 )
			iArray = 5;						 
		else if( strcmp( rData.GetPrivateID().c_str(),"derks2013@ioenter.com" ) == 0 )
			iArray = 6;						 
		else if( strcmp( rData.GetPrivateID().c_str(),"derks2014@ioenter.com" ) == 0 )
			iArray = 7;						 
		else if( strcmp( rData.GetPrivateID().c_str(),"derks2015@ioenter.com" ) == 0 )
			iArray = 8;						 
		else if( strcmp( rData.GetPrivateID().c_str(),"derks2016@ioenter.com" ) == 0 )
			iArray = 9;						 
		else if( strcmp( rData.GetPrivateID().c_str(),"dog730521@yahoo.com.tw" ) == 0 )
			iArray = 10;

		if( strcmp( rData.GetPrivateID().c_str(), "twlogin01" ) == 0 )
		{
			if( strcmp( szPW, "123456") == 0 )
			{
				iArray = 0;
			}
		}
		else if( strcmp( rData.GetPrivateID().c_str(), "twlogin02" ) == 0 )
		{
			if( strcmp( szPW, "123456") == 0 )
			{
				iArray = 1;
			}
		}

		if( COMPARE( iArray, 0, 10+1 ) ) //성공
			StringCbPrintf( szReturnValue, WEB_BUFF_SIZE,  "<?xml version=\"1.0\" encoding=\"utf-8\" ?><ReturnInfo xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\"xmlns=\"http://www.gamon.net/xmlns/GamonAuthSvr\"><result_str> user_no=%s|user_nick=|conn_no=0000|user_state=N|ip_addr=111.111.111.111|code=GAME </result_str><result>0</result></ReturnInfo>", szUserNoList[iArray]);
		else //실패
			StringCbPrintf( szReturnValue, WEB_BUFF_SIZE,  "<?xml version=\"1.0\" encoding=\"utf-8\" ?><ReturnInfo xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\"xmlns=\"http://www.gamon.net/xmlns/GamonAuthSvr\"><result_str>Error</result_str><result>-15</result></ReturnInfo>" );
			*/

		SP2Packet kPacket( rData.GetReturnMsgType() );
		kPacket << rData.GetPrivateID();
		kPacket << rData.GetBillingGUID();
		kPacket << BILLING_LOGIN_RESULT_SUCCESS;
		kPacket << rData.GetPrivateID();
		g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket );
		LOG.PrintTimeAndLog(0, "[TESTMODE]%s Complete.MsgType:%x:%s:%s:%s", __FUNCTION__, rData.GetReturnMsgType(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), szReturnValue );
	}
	else
	{

		ioHTTP Winhttp; 
		if( !Winhttp.GetResultData( szFullURL, "GET", szReturnValue, WEB_BUFF_SIZE ) )
		{
			LOG.PrintTimeAndLog( 0, "%s Http Error: %s:%s:%d", __FUNCTION__, rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), GetLastError() );
			SP2Packet kPacket( rData.GetReturnMsgType()  );
			kPacket << rData.GetPrivateID();
			kPacket << rData.GetBillingGUID();
			kPacket << BILLING_LOGIN_RESULT_FAIL;
			kPacket << false;
			g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket );
			g_LogDBClient.OnInsertBillingServerError( CNT_WEMADEBUY, BILLING_ERROR_LOG_EXCEPTION,"Login exception HTTP." );
			return;
		}
	

	ioXMLDocument xmlDoc;
	if( !xmlDoc.LoadFromMemory( szReturnValue ) )
	{
		SP2Packet kPacket( rData.GetReturnMsgType() );
		kPacket << rData.GetPrivateID();
		kPacket << rData.GetBillingGUID();
		kPacket << BILLING_LOGIN_RESULT_FAIL;
		g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket );
		LOG.PrintTimeAndLog(0, "%s Fail ioXMLDocument::LoadFromMemory :%d:%s:%s:%s:%s:%d", __FUNCTION__, rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetPublicID().c_str(), rData.GetUserNo().c_str(), GetLastError() );
		LOG.PrintTimeAndLog(0, "%s Fail ReturnValue :%s", __FUNCTION__, szReturnValue );
		g_LogDBClient.OnInsertBillingServerError( CNT_WEMADEBUY, BILLING_ERROR_LOG_EXCEPTION,"Billing exception taiwan." );
		return;
	}

	ioXMLElement xRootElement = xmlDoc.GetRootElement();
	if( xRootElement.IsEmpty() )	
	{
		SP2Packet kPacket( rData.GetReturnMsgType() );
		kPacket << rData.GetPrivateID();
		kPacket << rData.GetBillingGUID();
		kPacket << BILLING_LOGIN_RESULT_FAIL;
		g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket );
		LOG.PrintTimeAndLog(0, "%s ioXMLElement is Empty. :%d:%s:%s:%s:%s:%d", __FUNCTION__, rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetPublicID().c_str(), rData.GetUserNo().c_str(), GetLastError() );
		LOG.PrintTimeAndLog(0, "%s Fail ReturnValue :%s", __FUNCTION__, szReturnValue );
		g_LogDBClient.OnInsertBillingServerError( CNT_WEMADEBUY, BILLING_ERROR_LOG_EXCEPTION,"Billing exception taiwan." );
		return;
	}

/*
// Result
0  유효 회원
-1 INPUT 값 오류
-2 ID존재하지 않고나 같은ID 존재하는 경우
-3 ID혹은 비번 오류 경우
-4 ID존재 하지 않은경우
-5 비번 잘 못 되는 경우
-10 user_id 값 잘 못 되는경우
-11 user_pwd 값 오류
-12 MD5과정 오류
-13 Gamon회원이지만 아직 인증 하지 않은경우 예:메일인증
-15 해당 게임을 아직 개통 하지 않은경우
-16 Server인증 실패
-999 DB실행 오류

// 성공
<?xml version="1.0" encoding="utf-8" ?>
<ReturnInfo xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns="http://www.gamon.net/xmlns/GamonAuthSvr ">
<result_str> user_no=08071500000000000034|user_nick=|conn_no=0000|user_state=N|ip_addr=111.111.111.111|code=GAME </result_str>
<result>0</result>
</ReturnInfo>

// 실패
<?xml version="1.0" encoding="utf-8" ?>
<ReturnInfo xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:xsd="http://www.w3.org/2001/XMLSchema"
xmlns="http://www.gamon.net/xmlns/GamonAuthSvr ">
<result_str>Error</result_str>
<result>-15</result>
</ReturnInfo>
*/
	// xml parsing
	int iReturnCode = 0;
	ioXMLElement xChildElement = xRootElement.FirstChild( "result" );
	if( !xChildElement.IsEmpty() && xChildElement.GetText() != NULL )
		iReturnCode = atoi( xChildElement.GetText() );

	ioHashString sReturnText;
	xChildElement = xRootElement.FirstChild( "result_str" );
	if( !xChildElement.IsEmpty() && xChildElement.GetText() != NULL )
		sReturnText = xChildElement.GetText();

	if( iReturnCode != 0 )
	{
		SP2Packet kPacket( rData.GetReturnMsgType() );
		kPacket << rData.GetPrivateID();
		kPacket << rData.GetBillingGUID();
		kPacket << BILLING_LOGIN_RESULT_FAIL;
		kPacket << true;

		char szError[MAX_PATH]="";
		StringCbPrintf( szError, sizeof( szError ), "%s(%d)", sReturnText.c_str(), iReturnCode );
		kPacket << szError;

		g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket );
		g_LogDBClient.OnInsertBillingServerError( CNT_WEMADEBUY, BILLING_ERROR_LOG_EXCEPTION,"Login Return Error." );
		LOG.PrintTimeAndLog(0, "%s Return Error.:%s:%s:%s[%s:%s:%s]", __FUNCTION__, rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), szReturnValue, szPW, szUserKey, rData.GetEncodePW().c_str() );
		return;
	}

	ioHashString szUserNo;
	ioHashStringVec vParseStr;
	Help::GetParseData( sReturnText, vParseStr, TAIWAN_TOKEN );
	int iSize = vParseStr.size();
	for (int i = 0; i < iSize ; i++)
	{
		if( strstr( vParseStr[i].c_str(), "user_no") != NULL )
		{
			ioHashStringVec vParseExtendStr;
			Help::GetParseData( vParseStr[i], vParseExtendStr, TAIWAN_EXTEND_TOKEN );

			if( vParseStr.size() < MAX_LOGIN_ARRAY )
			{
				LOG.PrintTimeAndLog( 0, "%s Size Error [%s:%s:%s].", __FUNCTION__ , rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), sReturnText.c_str() );
				return;
			}
			szUserNo = vParseExtendStr[LOGIN_ARRAY_USERNO];
			break;
		}
	}

	/*if( !rLoginMgr.InsertLogin( rData, szUserNo.c_str(), 0, false, LoginInfo::PT_ENCODE_LOGIN_PW ) ) 
		return;*/

	SP2Packet kPacket( rData.GetReturnMsgType() );
	kPacket << rData.GetPrivateID();
	kPacket << rData.GetBillingGUID();
	kPacket << BILLING_LOGIN_RESULT_SUCCESS;
	kPacket << szUserNo;
	g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket );
	LOG.PrintTimeAndLog(0, "%s Complete.MsgType:%x:%s:%s:%s", __FUNCTION__, rData.GetReturnMsgType(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), szReturnValue );
	
	
	}
}

void ioLocalTaiwan::ThreadGetCash( const ioData &rData )
{
	if( rData.IsEmpty() )
	{
		LOG.PrintTimeAndLog( 0, "%s Data is Empty.", __FUNCTION__ );
		return;
	}

	if( g_App.IsReserveLogOut() )
	{
		LOG.PrintTimeAndLog( 0, "%s LogOut: %d:%s:%s:%s",__FUNCTION__, rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetPublicID().c_str() );
		SP2Packet kPacket( BSTPK_GET_CASH_RESULT );
		kPacket << rData.GetUserIndex();
		kPacket << rData.GetBillingGUID();
		kPacket << rData.GetSetUserMouse();
		kPacket << CASH_RESULT_EXCEPT;
		kPacket << false;
		g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket );
		return;
	}

	char szReturnValue[WEB_BUFF_SIZE]="";
	ZeroMemory( szReturnValue, WEB_BUFF_SIZE );

	char szFullURL[MAX_PATH*4]="";
	StringCbPrintf( szFullURL, sizeof( szFullURL ), "%s?memberNo=%s&serviceCode=%s&as_key=%s" , m_sBillingGetURL.c_str(), rData.GetPrivateID().c_str(), TAIWAN_CODE,  m_sKey.c_str() );

	if( g_App.IsTestMode() )
	{
		int iCash = 0;
		TestCashManager::TestCashInfo *pInfo = m_TestCashManager.GetInfo( rData.GetPrivateID() );
		if( pInfo )
		{
			m_TestCashManager.CheckNChargeCash( pInfo );
			iCash = pInfo->m_iCash;
		}
		else
		{
			m_TestCashManager.AddInfo( rData.GetPrivateID() );
			TestCashManager::TestCashInfo *pInfo = m_TestCashManager.GetInfo( rData.GetPrivateID() );
			if( pInfo )
			{
				iCash = pInfo->m_iCash;
			}
		}

		int iRand = rand()%100;
		if( iRand < 90 )
			StringCbPrintf( szReturnValue, WEB_BUFF_SIZE,  "<?xml version=\"1.0\" encoding=\"utf-8\" ?><ResultOfOperation xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns=\"http://billingws.gamon.net/\"><ApprovalNo /><EtcInfo /><RemainedBalanceAmount>%d</RemainedBalanceAmount><SpReturnNo>0</SpReturnNo><TransactionID /></ResultOfOperation>", iCash );
		else
			StringCbPrintf( szReturnValue, WEB_BUFF_SIZE,  "<?xml version=\"1.0\" encoding=\"utf-8\" ?><ResultOfOperation xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns=\"http://billingws.gamon.net/\"><ApprovalNo /><EtcInfo>1</EtcInfo><RemainedBalanceAmount>0</RemainedBalanceAmount><SpReturnNo>-101</SpReturnNo><TransactionID /></ResultOfOperation>" );
	}
	else
	{	
		ioHTTP Winhttp; //kyg 확인 필요 
		if( !Winhttp.GetResultData( szFullURL, "GET", szReturnValue, WEB_BUFF_SIZE ) )
		{
			LOG.PrintTimeAndLog( 0, "%s Http Error: %d:%s:%s:%s:%d", __FUNCTION__, rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetPublicID().c_str(), GetLastError() );
			SP2Packet kPacket( BSTPK_GET_CASH_RESULT );
			kPacket << rData.GetUserIndex();
			kPacket << rData.GetBillingGUID();
			kPacket << rData.GetSetUserMouse();
			kPacket << CASH_RESULT_EXCEPT;
			kPacket << false;
			g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket );
			g_LogDBClient.OnInsertBillingServerError( CNT_WEMADEBUY, BILLING_ERROR_LOG_EXCEPTION,"Billing exception GetCash(HTTP)." );
			return;
		}
	}
	ioXMLDocument xmlDoc;
	if( !xmlDoc.LoadFromMemory( szReturnValue ) )
	{
		SP2Packet kPacket( BSTPK_GET_CASH_RESULT );
		kPacket << rData.GetUserIndex();
		kPacket << rData.GetBillingGUID();
		kPacket << rData.GetSetUserMouse();
		kPacket << CASH_RESULT_EXCEPT;
		kPacket << false;
		g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket );
		LOG.PrintTimeAndLog(0, "%s Fail ioXMLDocument::LoadFromMemory :%d:%s:%s:%s:%s:%d", __FUNCTION__, rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetPublicID().c_str(), rData.GetUserNo().c_str(), GetLastError() );
		LOG.PrintTimeAndLog(0, "%s Fail ReturnValue :%s", __FUNCTION__, szReturnValue );
		g_LogDBClient.OnInsertBillingServerError( CNT_WEMADEBUY, BILLING_ERROR_LOG_EXCEPTION,"Billing exception taiwan." );
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
		kPacket << false;
		g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket );
		LOG.PrintTimeAndLog(0, "%s ioXMLElement is Empty. :%d:%s:%s:%s:%s:%d", __FUNCTION__, rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetPublicID().c_str(), rData.GetUserNo().c_str(), GetLastError() );
		LOG.PrintTimeAndLog(0, "%s Fail ReturnValue :%s", __FUNCTION__, szReturnValue );
		g_LogDBClient.OnInsertBillingServerError( CNT_WEMADEBUY, BILLING_ERROR_LOG_EXCEPTION,"Billing exception taiwan." );
		return;
	}

	/*
	* Error code
	0 성공
	-100 서비스 오픈 하지 않습니다.
	-101 Billing Key오류
	-101001 참수오류
	-101002 입력한 Data 오류
	-101999	Fatal Error
	-103001	coupon코드 생성 오류
	-103999	Fatal Error
	-104001	참수오류
	-104002	coupon정보 등록 오류
	-104999	Fatal Error
	-111006	구매 대상 아이템 정보 및table 간에 Data 일치 하지 않음.
	-111009	구매 대상 아이템 정보 및 입력하신 아이템 가격과 일치 하지 않음
	-111011	구매 대상 아이템 정보중에 판대 대상 아이템에 틀린 정보 포함함
	
	* 성공
	<?xml version="1.0" encoding="utf-8" ?>
	<ResultOfOperation xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns="http://billingws.gamon.net/">
	<ApprovalNo />
	<EtcInfo />
	<RemainedBalanceAmount>79275</RemainedBalanceAmount>
	<SpReturnNo>0</SpReturnNo>
	<TransactionID />
	</ResultOfOperation>
	
	* 실패
	<?xml version="1.0" encoding="utf-8" ?>
	<ResultOfOperation xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns="http://billingws.gamon.net/">
	<ApprovalNo />
	<EtcInfo>1</EtcInfo>
	<RemainedBalanceAmount>0</RemainedBalanceAmount>
	<SpReturnNo>-101</SpReturnNo>
	<TransactionID />
	</ResultOfOperation>
	*/	

	// xml parsing
	int iReturnCode = 0;
	ioXMLElement xChildElement = xRootElement.FirstChild( "SpReturnNo" );
	if( !xChildElement.IsEmpty() && xChildElement.GetText() != NULL )
		iReturnCode = atoi( xChildElement.GetText() );

	int iTotalCash = 0;
	xChildElement = xRootElement.FirstChild( "RemainedBalanceAmount" );
	if( !xChildElement.IsEmpty() && xChildElement.GetText() != NULL )
		iTotalCash = atoi( xChildElement.GetText() );

	if( iReturnCode != 0 )
	{
		LOG.PrintTimeAndLog( 0, "%s Return Error: %d:%s:%s:%s:%d[%s]", __FUNCTION__, rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetPublicID().c_str(), GetLastError(), szReturnValue );
		SP2Packet kPacket( BSTPK_GET_CASH_RESULT );
		kPacket << rData.GetUserIndex();
		kPacket << rData.GetBillingGUID();
		kPacket << rData.GetSetUserMouse();
		kPacket << CASH_RESULT_EXCEPT;
		kPacket << true;

		char szError[MAX_PATH]="";
		StringCbPrintf( szError, sizeof( szError ), "%d", iReturnCode );
		kPacket << szError;

		g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket );
		g_LogDBClient.OnInsertBillingServerError( CNT_WEMADEBUY, BILLING_ERROR_LOG_EXCEPTION,"Billing exception Gatcash(ReturnError)." );
		return;
	}

	SP2Packet kPacket( BSTPK_GET_CASH_RESULT );
	kPacket << rData.GetUserIndex();
	kPacket << rData.GetBillingGUID();
	kPacket << rData.GetSetUserMouse();
	kPacket << CASH_RESULT_SUCCESS;
	kPacket << iTotalCash;
	kPacket << iTotalCash;  // total cash와 purchase cash가 동일함.
	g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket );
	LOG.PrintTimeAndLog( 0, "%s Success: %d:%s:PrivateID %s:%s:%d[%s]", __FUNCTION__, rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetPublicID().c_str(), iTotalCash, szReturnValue  );
}

void ioLocalTaiwan::ThreadOutputCash( const ioData &rData )
{
	if( rData.IsEmpty() )
	{
		LOG.PrintTimeAndLog( 0, "%s Data is Empty.", __FUNCTION__ );
		return;
	}

	if( g_App.IsReserveLogOut() )
	{
		LOG.PrintTimeAndLog( 0, "%s LogOut: %d:%s:%s:%s",__FUNCTION__, rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetPublicID().c_str() );
		SP2Packet kPacket( BSTPK_OUTPUT_CASH_RESULT  );
		kPacket << rData.GetUserIndex();
		kPacket << rData.GetBillingGUID();
		kPacket << CASH_RESULT_EXCEPT;
		kPacket << rData.GetItemType();
		kPacket << false;
		g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket );
		return;
	}

	char szReturnValue[WEB_BUFF_SIZE]="";
	ZeroMemory( szReturnValue, WEB_BUFF_SIZE );

	char szAgencyNo[ioLocalParent::MAX_AGENCY_NO_PLUS_ONE]="";
	GetAgencyNo( szAgencyNo, sizeof( szAgencyNo ), true );

	char szFullURL[MAX_PATH*4]="";
	StringCbPrintf( szFullURL, sizeof( szFullURL ), "%s?serviceCode=%s&useSort=GAM&targetMemberNo=%s&targetMemberID=&targetGameServerID=&targetCharName=%s&order_id=%s&item_id=%d&item_price=%d&item_qty=1&targetUserIP=%s&as_key=%s", m_sBillingOutPutURL.c_str(), TAIWAN_CODE, rData.GetPrivateID().c_str(), rData.GetPublicID().c_str(), szAgencyNo, rData.GetGoodsNo(), rData.GetItemPayAmt(), rData.GetUserIP().c_str(), m_sKey.c_str() );

	if( g_App.IsTestMode() )
	{
		int iCash = 0;
		TestCashManager::TestCashInfo *pInfo = m_TestCashManager.GetInfo( rData.GetPrivateID() );
		if( pInfo )
		{
			m_TestCashManager.CheckNChargeCash( pInfo );
			pInfo->m_iCash -= rData.GetItemPayAmt();
			iCash = pInfo->m_iCash;
		}
		else
		{
			m_TestCashManager.AddInfo( rData.GetPrivateID() );
			TestCashManager::TestCashInfo *pInfo = m_TestCashManager.GetInfo( rData.GetPrivateID() );
			if( pInfo )
			{
				pInfo->m_iCash -= rData.GetItemPayAmt();
				iCash = pInfo->m_iCash;
			}
		}

		int iRand = rand()%100;
		if( iRand < 90 )
			StringCbPrintf( szReturnValue, WEB_BUFF_SIZE,   "<?xml version=\"1.0\" encoding=\"utf-8\" ?><ResultOfOperation xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns=\"http://billingws.gamon.net/\"><ApprovalNo>PAYMENT20111220183352503018989</ApprovalNo><EtcInfo>itemIndex=|itemSerialNo=|remainTime=0|regBindate=00000000|expBindate=00000000|</EtcInfo><RemainedBalanceAmount>%d</RemainedBalanceAmount><SpReturnNo>0</SpReturnNo><TransactionID>111220A0000000000002</TransactionID></ResultOfOperation>", iCash );
		else
			StringCbPrintf( szReturnValue, WEB_BUFF_SIZE,   "<?xml version=\"1.0\" encoding=\"utf-8\" ?><ResultOfOperation xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns=\"http://billingws.gamon.net/\"><ApprovalNo /><EtcInfo>itemIndex=|itemSerialNo=|remainTime=0|regBindate=00000000|expBindate=00000000|</EtcInfo><RemainedBalanceAmount>0</RemainedBalanceAmount><SpReturnNo>-111009</SpReturnNo><TransactionID /></ResultOfOperation>" );
	}
	else
	{
		ioHTTP Winhttp; //kyg 확인 필요 
		if( !Winhttp.GetResultData( szFullURL, "GET", szReturnValue, WEB_BUFF_SIZE ) )
		{
			LOG.PrintTimeAndLog( 0, "%s Http Error: %d:%s:%s:%s:%s:%d", __FUNCTION__, rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetPublicID().c_str(), rData.GetUserNo().c_str(), GetLastError() );
			SP2Packet kPacket( BSTPK_OUTPUT_CASH_RESULT );
			kPacket << rData.GetUserIndex();
			kPacket << rData.GetBillingGUID();
			kPacket << CASH_RESULT_EXCEPT;
			kPacket << rData.GetItemType();
			kPacket << false;
			g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket );
			g_LogDBClient.OnInsertBillingServerError( CNT_WEMADEBUY, BILLING_ERROR_LOG_EXCEPTION,"Billing exception Http." );
			return;
		}
	}
	ioXMLDocument xmlDoc;
	if( !xmlDoc.LoadFromMemory( szReturnValue ) )
	{
		SP2Packet kPacket( BSTPK_OUTPUT_CASH_RESULT );
		kPacket << rData.GetUserIndex();
		kPacket << rData.GetBillingGUID();
		kPacket << CASH_RESULT_EXCEPT;
		kPacket << rData.GetItemType();
		kPacket << false;
		g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket );
		LOG.PrintTimeAndLog(0, "%s Fail ioXMLDocument::LoadFromMemory :%d:%s:%s:%s:%s:%d", __FUNCTION__, rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetPublicID().c_str(), rData.GetUserNo().c_str(), GetLastError() );
		LOG.PrintTimeAndLog(0, "%s Fail ReturnValue :%s", __FUNCTION__, szReturnValue );
		g_LogDBClient.OnInsertBillingServerError( CNT_WEMADEBUY, BILLING_ERROR_LOG_EXCEPTION,"Billing exception taiwan." );
		return;
	}

	ioXMLElement xRootElement = xmlDoc.GetRootElement();
	if( xRootElement.IsEmpty() )	
	{
		SP2Packet kPacket( BSTPK_OUTPUT_CASH_RESULT );
		kPacket << rData.GetUserIndex();
		kPacket << rData.GetBillingGUID();
		kPacket << CASH_RESULT_EXCEPT;
		kPacket << rData.GetItemType();
		kPacket << false;
		g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket );
		LOG.PrintTimeAndLog(0, "%s ioXMLElement is Empty. :%d:%s:%s:%s:%s:%d", __FUNCTION__, rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetPublicID().c_str(), rData.GetUserNo().c_str(), GetLastError() );
		LOG.PrintTimeAndLog(0, "%s Fail ReturnValue :%s", __FUNCTION__, szReturnValue );
		g_LogDBClient.OnInsertBillingServerError( CNT_WEMADEBUY, BILLING_ERROR_LOG_EXCEPTION,"Billing exception taiwan." );
		return;
	}

	/*
	* Error code
	0 성공
	-100 서비스 오픈 하지 않습니다.
	-101 Billing Key오류
	-101001 참수오류
	-101002 입력한 Data 오류
	-101999	Fatal Error
	-103001	coupon코드 생성 오류
	-103999	Fatal Error
	-104001	참수오류
	-104002	coupon정보 등록 오류
	-104999	Fatal Error
	-111006	구매 대상 아이템 정보 및table 간에 Data 일치 하지 않음.
	-111009	구매 대상 아이템 정보 및 입력하신 아이템 가격과 일치 하지 않음
	-111011	구매 대상 아이템 정보중에 판대 대상 아이템에 틀린 정보 포함함
	
	* 성공
	<?xml version="1.0" encoding="utf-8" ?>
	<ResultOfOperation xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns="http://billingws.gamon.net/">
	<ApprovalNo>PAYMENT20111220183352503018989</ApprovalNo>
	<EtcInfo>itemIndex=|itemSerialNo=|remainTime=0|regBindate=00000000|expBindate=00000000|</EtcInfo>
	<RemainedBalanceAmount>28888</RemainedBalanceAmount>
	<SpReturnNo>0</SpReturnNo>
	<TransactionID>111220A0000000000002</TransactionID>
	</ResultOfOperation>
	
	* 실패 
	<?xml version="1.0" encoding="utf-8" ?>
	<ResultOfOperation xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns="http://billingws.gamon.net/">
	<ApprovalNo />
	<EtcInfo>itemIndex=|itemSerialNo=|remainTime=0|regBindate=00000000|expBindate=00000000|</EtcInfo>
	<RemainedBalanceAmount>0</RemainedBalanceAmount>
	<SpReturnNo>-111009</SpReturnNo>
	<TransactionID />
	</ResultOfOperation>
	
	*/

	// xml parsing
	int iReturnCode = 0;
	ioXMLElement xChildElement = xRootElement.FirstChild( "SpReturnNo" );
	if( !xChildElement.IsEmpty() && xChildElement.GetText() != NULL )
		iReturnCode = atoi( xChildElement.GetText() );

	int iTotalCash = 0;
	xChildElement = xRootElement.FirstChild( "RemainedBalanceAmount" );
	if( !xChildElement.IsEmpty() && xChildElement.GetText() != NULL )
		iTotalCash = atoi( xChildElement.GetText() );

	ioHashString sTransactionID;
	xChildElement = xRootElement.FirstChild( "TransactionID" );
	if( !xChildElement.IsEmpty() && xChildElement.GetText() != NULL )
		sTransactionID = xChildElement.GetText();

	if( iReturnCode != 0 )
	{
		LOG.PrintTimeAndLog( 0, "%s Return Error: %d:%s:%s:%s:%s:%d[%s]", __FUNCTION__, rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetPublicID().c_str(), rData.GetUserNo().c_str(), GetLastError(), szReturnValue );
		SP2Packet kPacket( BSTPK_OUTPUT_CASH_RESULT );
		kPacket << rData.GetUserIndex();
		kPacket << rData.GetBillingGUID();
		kPacket << CASH_RESULT_EXCEPT;
		kPacket << rData.GetItemType();
		kPacket << true;

		char szError[MAX_PATH]="";
		StringCbPrintf( szError, sizeof( szError ), "%d", iReturnCode );
		kPacket << szError;

		g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket );
		g_LogDBClient.OnInsertBillingServerError( CNT_WEMADEBUY, BILLING_ERROR_LOG_EXCEPTION,"Billing exception Return Error." );
		return;
	}

	SP2Packet kPacket( BSTPK_OUTPUT_CASH_RESULT );
	kPacket << rData.GetUserIndex();
	kPacket << rData.GetBillingGUID();
	kPacket << CASH_RESULT_SUCCESS;
	kPacket << rData.GetItemType();
	kPacket << rData.GetItemPayAmt();
	kPacket << 0; // TransactionID ( FOR US )
	int iItemValueList[MAX_ITEM_VALUE];
	for (int i = 0; i <MAX_ITEM_VALUE; i++)
		iItemValueList[i] = rData.GetItemValue( i );
	ioChannelingNodeParent::SetItemValueList( kPacket, rData.GetItemType(), iItemValueList );
	// Cancel Step 1
	kPacket << rData.GetChannelingType();  // 공통
	kPacket << iTotalCash;
	kPacket << iTotalCash; // 캐쉬와 실재 구매한 캐쉬가 동일하다.
	kPacket << rData.GetPrivateID();
	kPacket << sTransactionID;
	g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket );

	LOG.PrintTimeAndLog(0, "%s Success. :%d:%s:PrivateID %s:%s:%s[%d:%s:%d:%d][%s]", __FUNCTION__, rData.GetUserIndex(), rData.GetBillingGUID().c_str(),rData.GetPrivateID().c_str(), rData.GetPublicID().c_str(), rData.GetUserNo().c_str(), rData.GetGoodsNo(), rData.GetGoodsName().c_str(), rData.GetItemPayAmt(), iTotalCash, szReturnValue );
}

void ioLocalTaiwan::ThreadCancelCash( const ioData &rData )
{
	// 유저가 접속해 있지 않은 경우 처리하므로 Return 값이 없다.

	if( rData.IsEmpty() )
	{
		LOG.PrintTimeAndLog( 0, "%s Data is Empty.", __FUNCTION__ );
		return;
	}

	char szReturnValue[WEB_BUFF_SIZE]="";
	ZeroMemory( szReturnValue, WEB_BUFF_SIZE );

	char szFullURL[MAX_PATH*4]="";
	StringCbPrintf( szFullURL, sizeof( szFullURL ), "%s?payNo=%s&serviceCode=%s&memberNo=%s&useSort=GAM&as_key=%s" , m_sBillingCancelURL.c_str(), rData.GetUserNo().c_str(), TAIWAN_CODE, rData.GetPrivateID().c_str(), m_sKey.c_str() );

	if( g_App.IsTestMode() )
	{
		int iRand = rand()%100;
		if( iRand < 90 )
			StringCbPrintf( szReturnValue, WEB_BUFF_SIZE,  "<?xml version=\"1.0\" encoding=\"utf-8\" ?><ResultOfOperation xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns=\"http://billingws.gamon.net/\"><ApprovalNo>CCLPAYMENT20111220194121900757957</ApprovalNo><EtcInfo /><RemainedBalanceAmount>28890</RemainedBalanceAmount><SpReturnNo>0</SpReturnNo><TransactionID /></ResultOfOperation>" );
		else
			StringCbPrintf( szReturnValue, WEB_BUFF_SIZE,  "<?xml version=\"1.0\" encoding=\"utf-8\" ?><ResultOfOperation xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns=\"http://billingws.gamon.net/\"><ApprovalNo /><EtcInfo /><RemainedBalanceAmount>0</RemainedBalanceAmount><SpReturnNo>-327012</SpReturnNo><TransactionID /></ResultOfOperation>" );
	}
	else
	{	
		ioHTTP Winhttp; //kyg 확인 필요 
		if( !Winhttp.GetResultData( szFullURL, "GET", szReturnValue, WEB_BUFF_SIZE ) )
		{
			LOG.PrintTimeAndLog( 0, "%s Http Error: %d:%s:%s:%s:%d", __FUNCTION__, rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetPublicID().c_str(), GetLastError() );
			g_LogDBClient.OnInsertBillingServerError( CNT_WEMADEBUY, BILLING_ERROR_LOG_EXCEPTION,"Billing exception GetCash(HTTP)." );
			return;
		}
	}

	/*
	* Error code
	0 성공
	-100 서비스 오픈 하지 않습니다.
	-101 Billing Key오류
	-101001 참수오류
	-101002 입력한 Data 오류
	-101999	Fatal Error
	-103001	coupon코드 생성 오류
	-103999	Fatal Error
	-104001	참수오류
	-104002	coupon정보 등록 오류
	-104999	Fatal Error
	-111006	구매 대상 아이템 정보 및table 간에 Data 일치 하지 않음.
	-111009	구매 대상 아이템 정보 및 입력하신 아이템 가격과 일치 하지 않음
	-111011	구매 대상 아이템 정보중에 판대 대상 아이템에 틀린 정보 포함함
	
	* 성공
	<?xml version="1.0" encoding="utf-8" ?>
	<ResultOfOperation xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns="http://billingws.gamon.net/">
	<ApprovalNo>CCLPAYMENT20111220194121900757957</ApprovalNo>
	<EtcInfo />
	<RemainedBalanceAmount>28890</RemainedBalanceAmount>
	<SpReturnNo>0</SpReturnNo>
	<TransactionID />
	</ResultOfOperation>
	
	* 실패 
	<?xml version="1.0" encoding="utf-8" ?>
	<ResultOfOperation xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:xsd="http://www.w3.org/2001/XMLSchema" xmlns="http://billingws.gamon.net/">
	<ApprovalNo />
	<EtcInfo />
	<RemainedBalanceAmount>0</RemainedBalanceAmount>
	<SpReturnNo>-327012</SpReturnNo>
	<TransactionID />
	</ResultOfOperation>
	*/	
	LOG.PrintTimeAndLog( 0, "%s Success: %d:%s:PrivateID %s:%s:%s[%s]", __FUNCTION__, rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetPublicID().c_str(), rData.GetUserNo().c_str(), szReturnValue  );
}