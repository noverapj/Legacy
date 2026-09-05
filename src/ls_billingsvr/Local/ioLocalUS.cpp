#include "../stdafx.h"
#include "./iolocalus.h"
#include "../NodeInfo/ServerNode.h"
#include "../ThreadPool/ioThreadPool.h"
#include "../DataBase/LogDBClient.h"
#include "../Channeling/ioChannelingNodeParent.h"
#include "../NodeInfo/ServerNodeManager.h"
#include "../MainProcess.h"
#include "../EtcHelpFunc.h"
#include "../USBillingServer/USBillingServer.h"
#include "../NodeInfo/MemInfoManager.h"
#include "../xml/ioxmldocument.h"
#include "../xml/ioxmlelement.h"


extern CLog LOG;

ioLocalUS::ioLocalUS(void)
{
}

ioLocalUS::~ioLocalUS(void)
{
}

ioLocalManager::LocalType ioLocalUS::GetType()
{
	return ioLocalManager::LCT_US;
}

void ioLocalUS::Init()
{
	ioINILoader kLoader( "ls_config_billingsvr.ini" );
	kLoader.SetTitle( "Local" );

	char szLoginURL[MAX_PATH]="";
	kLoader.LoadString( "USALoginURL", "", szLoginURL, sizeof( szLoginURL ) );
	m_sLoginURL = szLoginURL;

	if( m_sLoginURL.IsEmpty() )
		LOG.PrintTimeAndLog(0,"Error USA URL Error");
}

void ioLocalUS::OnLoginData( ServerNode *pServerNode, SP2Packet &rkPacket )
{

	ioHashString szBillingGUID;
	ioHashString szPrivateID;
	ioHashString sEncodePW;
	ioHashString szPublicIP;
	DWORD        dwReturnMsgType = 0;
	ioHashString szUserType;

	rkPacket >> szBillingGUID >> szPrivateID >> sEncodePW >> szPublicIP >> dwReturnMsgType; // 공통
	rkPacket >> szUserType; // us

	if( !pServerNode )
	{
		LOG.PrintTimeAndLog(0, "%s pServerNode == NULL:%s:%s:%s", __FUNCTION__, szBillingGUID.c_str(), szPrivateID.c_str(), sEncodePW.c_str() );
		return;
	}

	ioData kData;
	kData.SetBillingGUID( szBillingGUID );
	kData.SetPrivateID( szPrivateID );
	kData.SetEncodePW( sEncodePW );
	kData.SetReturnMsgType( dwReturnMsgType );
	kData.SetUserKey( szUserType );
	kData.SetUserIP(szPublicIP);
	if( pServerNode )
	{
		kData.SetServerIP( pServerNode->GetIP() );
		kData.SetServerPort( pServerNode->GetClientPort() );
	}
	kData.SetEmpty( false );
	kData.SetProcessType( ioData::PT_LOGIN );

	g_ThreadPool.SetData( kData );
} 

void ioLocalUS::ThreadLogin( const ioData &rData, LoginManager &rLoginMgr )
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

	if( rLoginMgr.CheckLogin( rData ) )
		return;

	enum { MAX_RANDOM_KEY = 20, };
	char szRandomKey[MAX_PATH]="";
	if( rData.GetEncodePW().Length() > MAX_RANDOM_KEY )
		StringCbCopyN( szRandomKey, sizeof( szRandomKey ), rData.GetEncodePW().c_str(), MAX_RANDOM_KEY );
	char szEncode[MAX_PATH*2]="";
	int iEncodeCnt = 0;
	int iEncodPwLength = rData.GetEncodePW().Length();
	for (int i = 0; i < iEncodPwLength; i++)
	{
		if( i >= MAX_RANDOM_KEY )
		{
			szEncode[iEncodeCnt] = rData.GetEncodePW().At(i);
			iEncodeCnt++;
			if( iEncodeCnt >= MAX_PATH*2 )
				break;
		}
	}

	char szPW[MAX_PATH*2]="";
	char szUserKey[MAX_PATH]="";
	StringCbPrintf( szUserKey, sizeof( szUserKey ), "%s%s", rData.GetPrivateID().c_str(), szRandomKey );
	Help::Decode( szEncode, strlen( szEncode ), szPW, sizeof( szPW ), szUserKey, strlen( szUserKey ) );

	char szRealLoginID[MAX_PATH]="";
	StringCbCopy( szRealLoginID, sizeof( szRealLoginID ), rData.GetPrivateID().c_str() );
	if( rData.GetPrivateID().Length() > 0 && rData.GetPrivateID().Length() < MAX_PATH )
		szRealLoginID[rData.GetPrivateID().Length()-1] = NULL; // 페이스북 숫자 아이디가 실제 privateID와 중복 될 수 있으므로 |구분자을 붙였던 것을 제거

	char szUserIP[MAX_PATH]="";
	StringCbCopy( szUserIP, sizeof( szUserIP ), rData.GetUserIP().c_str() );


	char szReturnValue[WEB_BUFF_SIZE]="";
	ZeroMemory( szReturnValue, WEB_BUFF_SIZE );

	char szFullURL[MAX_PATH*4]="";
	if( rData.GetUserKey() == USER_TYPE_NORMAL )
	{
//		StringCbPrintf( szFullURL, sizeof( szFullURL ), "%s?email=%s&password=%s&usertype=%s", m_sLoginURL.c_str(), szRealLoginID, szPW, rData.GetUserKey().c_str() );
//		StringCbPrintf(szFullURL, sizeof( szFullURL ), "%s?appid=dfg6hdfAds&userid=%s&pass=%s", m_sLoginURL.c_str(), szRealLoginID, szPW);

		StringCbPrintf(szFullURL, sizeof( szFullURL ), "%sloginLostsaga.aspx", m_sLoginURL.c_str());//, szRealLoginID, szPW);

//		https://app.z8games.com/WS/loginLostsaga.aspx?appid=dfg6hdfAds&userid=jalnan&pass=jal1092
	}
	else if( rData.GetUserKey() == USER_TYPE_FB )
	{
		StringCbPrintf( szFullURL, sizeof( szFullURL ), "%s?facebook_id=%s&access_token=%s&usertype=%s", m_sLoginURL.c_str(), szRealLoginID, szPW, rData.GetUserKey().c_str() );
	}
	else
	{
		LOG.PrintTimeAndLog( 0 , "%s Error Type %s %s", __FUNCTION__, rData.GetPrivateID().c_str(), rData.GetUserKey().c_str() );
	}
	
	if( g_App.IsTestMode() )
	{
		int iUserID = 0;
		if( strcmp( szRealLoginID, "derks2007@ioenter.com" ) == 0 )
			iUserID = 1;
		if( strcmp( szRealLoginID, "derks2008@ioenter.com" ) == 0 )
			iUserID = 2;
		if( strcmp( szRealLoginID, "derks2009@ioenter.com" ) == 0 )
			iUserID = 3;
		if( strcmp( szRealLoginID, "derks2010@ioenter.com" ) == 0 )
			iUserID = 4;
		if( strcmp( szRealLoginID, "derks2011@ioenter.com" ) == 0 )
			iUserID = 5;
		if( strcmp( szRealLoginID, "derks2012@ioenter.com" ) == 0 )
			iUserID = 6;
		if( strcmp( szRealLoginID, "derks2013@ioenter.com" ) == 0 )
			iUserID = 7;
		if( strcmp( szRealLoginID, "derks2014@ioenter.com" ) == 0 )
			iUserID = 8;
		if( strcmp( szRealLoginID, "derks2015@ioenter.com" ) == 0 )
			iUserID = 9;
		if( strcmp( szRealLoginID, "jalnan" ) == 0 )
			iUserID = 10;

		StringCbPrintf( szReturnValue, WEB_BUFF_SIZE,  "{\"result\" : 0, \"msg\" : \"Login successful\" , \"userid\":%d, \"username\":\"teset@test.com\", \"loginToken\":\"ABCDE\" }", iUserID );
	}
	else
	{
		ioHTTP Winhttp; //kyg 확인 필요 
		char szPost[MAX_PATH*2]="";
		StringCbPrintf(szPost, sizeof( szPost ), "appid=dfg6hdfAds&userid=%s&pass=%s&ipaddress=%s", 
					szRealLoginID, szPW, szUserIP);

		if( !Winhttp.GetResultData( szFullURL, szPost, szReturnValue, WEB_BUFF_SIZE ) )
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
	
	}

	if( g_App.IsTestMode())
	{
		char szInsertValue[MAX_PATH*2]="";

		enum { MAX_INSERT_VALUE = 3, };

		ioHashStringVec vValueVec;
		vValueVec.reserve(MAX_INSERT_VALUE);
		vValueVec.push_back( szRealLoginID );
		vValueVec.push_back( szRealLoginID );
		vValueVec.push_back( szRealLoginID );

		for (int i = 0; i < MAX_INSERT_VALUE ; i++)
		{
			StringCbCat( szInsertValue, sizeof( szInsertValue ), vValueVec[i].c_str() ); // userid, loginToken, username
			StringCbCat( szInsertValue, sizeof( szInsertValue ), "|" );
		}

		if( !rLoginMgr.InsertLogin( rData, szInsertValue, 0, false, LoginInfo::PT_ENCODE_LOGIN_PW ) ) 
			return;

		SP2Packet kPacket( rData.GetReturnMsgType() );
		kPacket << rData.GetPrivateID();
		kPacket << rData.GetBillingGUID();
		kPacket << BILLING_LOGIN_RESULT_SUCCESS;
		kPacket << szInsertValue;
		g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket );

		LOG.PrintTimeAndLog( 0, "%s %s [%s %s %s %s ]", __FUNCTION__, rData.GetPrivateID().c_str(), vValueVec[0].c_str(), vValueVec[1].c_str(), vValueVec[2].c_str(), szInsertValue );
	}
	else
	{
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
				<Z8RetMsg xmlns="http://schemas.datacontract.org/2004/07/WS" 
				xmlns:i="http://www.w3.org/2001/XMLSchema-instance">
				<data>412f4ef1-3042-4169-b5fd-c70b22a99796</data>
				<errorcode>0</errorcode>
				<status>true</status>
				</Z8RetMsg>

				-110    Internal IP Only
						We are currently under maintenance. Please try again at a later time.

				-120	Invalid Auth Infomation		
						Please check your login credentials and try again.

				-150	Auth Token is expired   
						Please check your login credentials and try again.

				-200	CBT User Only Access
						You have not signed up for Closed Beta Testing

						¿¡AO ½a RestAPI for Lostsaga v1.1.pptx Au°iCI¼A ¤¶¤²
				-300
				-400
				-500
				-1001   Your ID/Password is wrong   
						Please check your login credentials and try again.

			*/

		// xml parsing
		int iReturnCode = -10000;
		ioXMLElement xChildElement = xRootElement.FirstChild( "errorcode" );
		if( !xChildElement.IsEmpty() && xChildElement.GetText() != NULL )
			iReturnCode = atoi( xChildElement.GetText() );

		ioHashString sReturnText;
		xChildElement = xRootElement.FirstChild( "data" );
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
			LOG.PrintTimeAndLog(0, "%s Return Error.:%s:%s:%s[%s:%s:%s]", __FUNCTION__, rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), szError, szPW, szUserKey, rData.GetEncodePW().c_str() );
			return;
		}


		char szFullURLGet[MAX_PATH*2]="";
		StringCbPrintf( szFullURLGet, sizeof(szFullURLGet), "http://api.z8games.com/ws/lostsaga.svc/getuserinfo/dfg6hdfAds/%s" , sReturnText.c_str() );

		ioHTTP Winhttp;
		strcpy_s(szReturnValue, "");

		if( !Winhttp.GetResultData( szFullURLGet, "GET", szReturnValue, WEB_BUFF_SIZE ) )
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

		ioXMLDocument xmlDocGetUserInfo;
		if( !xmlDocGetUserInfo.LoadFromMemory( szReturnValue ) )
		{
			SP2Packet kPacket( rData.GetReturnMsgType() );
			kPacket << rData.GetPrivateID();
			kPacket << rData.GetBillingGUID();
			kPacket << BILLING_LOGIN_RESULT_FAIL;
			g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket );
			LOG.PrintTimeAndLog(0, "%s Fail GetUserInfo :%d:%s:%s:%s:%s:%d", __FUNCTION__, rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetPublicID().c_str(), rData.GetUserNo().c_str(), GetLastError() );
			LOG.PrintTimeAndLog(0, "%s Fail ReturnValue :%s", __FUNCTION__, szReturnValue );
			g_LogDBClient.OnInsertBillingServerError( CNT_WEMADEBUY, BILLING_ERROR_LOG_EXCEPTION,"Billing exception taiwan." );
			return;
		}

		ioXMLElement xRootElementUser = xmlDocGetUserInfo.GetRootElement();
		if( xRootElementUser.IsEmpty() )	
		{
			SP2Packet kPacket( rData.GetReturnMsgType() );
			kPacket << rData.GetPrivateID();
			kPacket << rData.GetBillingGUID();
			kPacket << BILLING_LOGIN_RESULT_FAIL;
			g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket );
			LOG.PrintTimeAndLog(0, "%s GetUserInfo is Empty. :%d:%s:%s:%s:%s:%d", __FUNCTION__, rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetPublicID().c_str(), rData.GetUserNo().c_str(), GetLastError() );
			LOG.PrintTimeAndLog(0, "%s Fail ReturnValue :%s", __FUNCTION__, szReturnValue );
			g_LogDBClient.OnInsertBillingServerError( CNT_WEMADEBUY, BILLING_ERROR_LOG_EXCEPTION,"Billing exception taiwan." );
			return;
		}


		// xml parsing
		iReturnCode = -10000;
		xChildElement = xRootElementUser.FirstChild( "ErrCode" );
		if( !xChildElement.IsEmpty() && xChildElement.GetText() != NULL )
			iReturnCode = atoi( xChildElement.GetText() );

		ioHashString sRtnUserNo;
		xChildElement = xRootElementUser.FirstChild( "UserNo" );
		if( !xChildElement.IsEmpty() && xChildElement.GetText() != NULL )
			sRtnUserNo = xChildElement.GetText();

		ioHashString sRtnUserID;
		xChildElement = xRootElementUser.FirstChild( "UserID" );
		if( !xChildElement.IsEmpty() && xChildElement.GetText() != NULL )
			sRtnUserID = xChildElement.GetText();

		if( iReturnCode != 0 )
		{
			SP2Packet kPacket( rData.GetReturnMsgType() );
			kPacket << rData.GetPrivateID();
			kPacket << rData.GetBillingGUID();
			kPacket << BILLING_LOGIN_RESULT_FAIL;
			kPacket << true;

			char szError[MAX_PATH]="";
			StringCbPrintf( szError, sizeof( szError ), "GetUserInfo %s(%d)", sReturnText.c_str(), iReturnCode );
			kPacket << szError;

			g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket );
			g_LogDBClient.OnInsertBillingServerError( CNT_WEMADEBUY, BILLING_ERROR_LOG_EXCEPTION,"Login Return Error." );
			LOG.PrintTimeAndLog(0, "%s Return Error.:%s:%s:%s[%s:%s:%s]", __FUNCTION__, rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), szError, szPW, szUserKey, rData.GetEncodePW().c_str() );
			return;
		}

		char szInsertValue[MAX_PATH*2]="";

		enum { MAX_INSERT_VALUE = 3, };

		ioHashStringVec vValueVec;
		vValueVec.reserve(MAX_INSERT_VALUE);
		vValueVec.push_back( sRtnUserNo );
		vValueVec.push_back( sReturnText );
		vValueVec.push_back( sRtnUserID );

		for (int i = 0; i < MAX_INSERT_VALUE ; i++)
		{
			StringCbCat( szInsertValue, sizeof( szInsertValue ), vValueVec[i].c_str() ); // userid, loginToken, username
			StringCbCat( szInsertValue, sizeof( szInsertValue ), "|" );
		}

		if( !rLoginMgr.InsertLogin( rData, szInsertValue, 0, false, LoginInfo::PT_ENCODE_LOGIN_PW ) ) 
			return;

		SP2Packet kPacket( rData.GetReturnMsgType() );
		kPacket << rData.GetPrivateID();
		kPacket << rData.GetBillingGUID();
		kPacket << BILLING_LOGIN_RESULT_SUCCESS;
		kPacket << szInsertValue;
		g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket );

		LOG.PrintTimeAndLog( 0, "RESULT_SUCCESS %s %s [%s %s %s %s ]", __FUNCTION__, rData.GetPrivateID().c_str(), vValueVec[0].c_str(), vValueVec[1].c_str(), vValueVec[2].c_str(), szInsertValue );
	}

	
}

void ioLocalUS::_OnGetCash( ServerNode *pServerNode, SP2Packet &rkPacket )
{
	int          iChannelingType   = 0;
	ioHashString szBillingGUID;
	DWORD        dwUserIndex       = 0;
	ioHashString szPrivateID;
	ioHashString szPublicID;
	bool         bSetUserMouse     = false;
	ioHashString szBillingUserKey;

	rkPacket >> iChannelingType >> szBillingGUID >> dwUserIndex >> szPrivateID >> szPublicID >> bSetUserMouse; // 공통사항
	rkPacket >> szBillingUserKey; // Wemade USA

	if( !pServerNode )
	{
		LOG.PrintTimeAndLog(0, "%s pServerNode == NULL:%s:%d:%s:%s", __FUNCTION__, szBillingGUID.c_str(), dwUserIndex, szPrivateID.c_str(), szPublicID.c_str() );
		return;
	}

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

	if( g_App.IsTestMode() )
	{
		int iReturnCash    = 0;
		int iPurchasedCash = 0;
		TestCashManager::TestCashInfo *pInfo = m_TestCashManager.GetInfo( szPrivateID );
		if( pInfo )
		{
			m_TestCashManager.CheckNChargeCash( pInfo );
			iReturnCash    = pInfo->m_iCash;
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

		SP2Packet kPacket( BSTPK_GET_CASH_RESULT );
		kPacket << dwUserIndex;
		kPacket << szBillingGUID;
		kPacket << bSetUserMouse;
		kPacket << CASH_RESULT_SUCCESS;
		kPacket << iReturnCash;
		kPacket << iPurchasedCash;
		pServerNode->SendMessage( kPacket );
		return;
	}

/*
	0       성공
	450	    Stored Procedure 실행 오류입니다.
	452	    데이터베이스에 연결할 수 없습니다.
	1001	파라메터 오류입니다.
	1003	정의되지 않은 전문 명령어 요청입니다.
	1004	Socket Timeout 이 발생하였습니다.
	1006	GTX서버가 일시 중지 되었습니다.
	1007	허용되지 않은 IP 요청입니다.
	1010	GTX서버가 중지 중입니다. 클라이언트의 요청을 수락할 수 없습니다.
	1011	요청된 클라이언트 수가 GTX서버의 처리 용량을 초과하였습니다.
	1012	GTX 서버 내부 Exception 오류입니다.
	1013	요청된 전문이 정의 규약에 벗어납니다.
 */

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

	pInfo->m_szKey        = szPrivateID;
	pInfo->m_dwCreateTime = TIMEGETTIME();
	pInfo->m_eType	= BillInfoManager::AT_GET;

	pInfo->m_kPacket << dwUserIndex << szBillingGUID << bSetUserMouse << pServerNode->GetIP() << pServerNode->GetClientPort();
	pInfo->m_kPacket.SetPosBegin();
	if(!g_BillInfoManager->Add(pInfo))
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

	BILL_PACK_GETBALANCE kInfo;
	kInfo.SetInfo( szPrivateID.c_str(), szBillingUserKey.c_str(), szPublicID.c_str() );
	kInfo.Htonl();

	SP2Packet kPacket( BTUBTPK_BALANCE_REQUEST );
	kPacket << kInfo;
	if( !g_USBillingServer.SendMessage( kPacket ) )
	{
		LOG.PrintTimeAndLog( 0, "%s Fail Send: %d:%s:%s:%s:%d",__FUNCTION__, dwUserIndex, szBillingGUID.c_str(), szPrivateID.c_str(), szPublicID.c_str(), GetLastError()  );
		SP2Packet kPacket( BSTPK_GET_CASH_RESULT );
		kPacket << dwUserIndex;
		kPacket << szBillingGUID;
		kPacket << bSetUserMouse;
		kPacket << CASH_RESULT_EXCEPT;
		pServerNode->SendMessage( kPacket );

		g_BillInfoManager->Delete(szPrivateID);

		g_LogDBClient.OnInsertBillingServerError( CNT_WEMADEBUY, BILLING_ERROR_LOG_FAIL_SEND_WEMADE_BILLINGSERVER,"fail send billingserver." );
		return;
	}

//	LOG.PrintTimeAndLog( 0, "%s Send Success: %d:%s:%s:%s",__FUNCTION__, dwUserIndex, szBillingGUID.c_str(), szPrivateID.c_str(), szPublicID.c_str());
}

void ioLocalUS::OnRecieveGetCash( const BILL_PACK_GETBALANCE &rkResult )
{
	ioHashString szPrivateID;
	char szTemp[MAX_PATH]="";
	StringCbPrintf( szTemp, sizeof( szTemp ), "%u", rkResult.UserNo );
	szPrivateID = szTemp;

	BillInfoManager::BillingInfo *pInfo = g_BillInfoManager->Get(szPrivateID);

	if( !pInfo )
	{
		LOG.PrintTimeAndLog( 0, "%s pInfo == NULL. %d:%s:%d:%d:%d:%s", __FUNCTION__, rkResult.UserNo, rkResult.UserID, rkResult.RealCash, rkResult.BonusCash, rkResult.RetCode, rkResult.RetMsg );
		return;
	}

	DWORD        dwUserIndex   = 0;
	ioHashString szBillingGUID;
	bool         bSetUserMouse = false;
	ioHashString szServerIP;
	int          iClientPort   = 0;
	pInfo->m_kPacket >> dwUserIndex >> szBillingGUID >> bSetUserMouse >> szServerIP >> iClientPort;

	if( rkResult.RetCode != RESULT_SUCCESS )
	{
		ioHashString sResultMessage = rkResult.RetMsg;

		SP2Packet kPacket( BSTPK_GET_CASH_RESULT );
		kPacket << dwUserIndex;
		kPacket << szBillingGUID;
		kPacket << bSetUserMouse;
		kPacket << CASH_RESULT_EXCEPT;
		kPacket << true;
		kPacket << sResultMessage;
		if( !g_ServerNodeManager.SendMessageIP( szServerIP, iClientPort, kPacket ) )
		{
			LOG.PrintTimeAndLog( 0, "%s Send Fail(1): %d:%s:%s:Ret %d:%d:%d", __FUNCTION__, dwUserIndex, szBillingGUID.c_str(), szPrivateID.c_str(),  rkResult.RetCode, rkResult.RealCash, rkResult.BonusCash );
			g_LogDBClient.OnInsertBillingServerError( CNT_WEMADEBUY, BILLING_ERROR_LOG_FAIL_SEND_WEMADE_BILLINGSERVER,"fail send billingserver." );
			g_BillInfoManager->Delete(szPrivateID);
			return;
		}
		LOG.PrintTimeAndLog( 0, "%s Fail: %d:%s:%s:Ret %d:%s:%d:%d", __FUNCTION__, dwUserIndex, szBillingGUID.c_str(), szPrivateID.c_str(),  rkResult.RetCode, rkResult.RetMsg, rkResult.RealCash, rkResult.BonusCash );
		g_LogDBClient.OnInsertBillingServerError( CNT_WEMADEBUY, rkResult.RetCode, "Error return value." );
		g_BillInfoManager->Delete(szPrivateID);
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
		LOG.PrintTimeAndLog( 0, "%s Send Fail(2): %d:%s:%s:Ret %d:%d:%d", __FUNCTION__, dwUserIndex, szBillingGUID.c_str(), szPrivateID.c_str(),  rkResult.RetCode, rkResult.RealCash, rkResult.BonusCash );
		g_LogDBClient.OnInsertBillingServerError( CNT_WEMADEBUY, BILLING_ERROR_LOG_FAIL_SEND_WEMADE_BILLINGSERVER,"fail send billingserver." );
		g_BillInfoManager->Delete(szPrivateID);
		return;
	}
	LOG.PrintTimeAndLog( 0, "%s Success: %d:%s:%s:Ret %d:%d:%d", __FUNCTION__, dwUserIndex, szBillingGUID.c_str(), szPrivateID.c_str(), rkResult.RetCode, rkResult.RealCash, rkResult.BonusCash );
	g_BillInfoManager->Delete(szPrivateID);
}

void ioLocalUS::_OnOutputCash( ServerNode *pServerNode, SP2Packet &rkPacket, DWORD dwGoodsNo, const ioHashString &rszGoodsName )
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

	int         iGameServerPort = 0;
	ioHashString szBillingUserKey;
	rkPacket >> iGameServerPort;  // us
	rkPacket >> szBillingUserKey; // us

	if( g_App.IsTestMode() )
	{
		int iReturnCash    = 0;
		int iPurchasedCash = 0;
		TestCashManager::TestCashInfo *pInfo = m_TestCashManager.GetInfo( szPrivateID );
		if( pInfo )
		{
			if( ( pInfo->m_iCash - iPayAmt ) > 0 )
				pInfo->m_iCash -= iPayAmt;

			iReturnCash    = pInfo->m_iCash;
			iPurchasedCash = iReturnCash;
		}

		SP2Packet kPacket( BSTPK_OUTPUT_CASH_RESULT );
		kPacket << dwUserIndex;
		kPacket << szBillingGUID;
		kPacket << CASH_RESULT_SUCCESS;
		kPacket << iType;
		kPacket << iPayAmt;
		kPacket << 0; // TransactionID ( FOR US )

		ioChannelingNodeParent::SetItemInfo( kPacket, iType, kBuyInfo );

		// Cancel Step 1
		kPacket << iChannelingType;  // 공통
		kPacket << iReturnCash;
		kPacket << iPurchasedCash;
		kPacket << szPrivateID; 
		kPacket << "TEST@TEST.COM#WMU"; // UserID
		kPacket << "TEST001";           // chargeNo
		pServerNode->SendMessage( kPacket );
		return;
	}

	BillInfoManager::BillingInfo *pInfo = g_BillInfoManager->PopBillInfo();
	if( !pInfo )
	{
		LOG.PrintTimeAndLog( 0, "%s pInfo == NULL: %d:%s:%s:%s:%d",__FUNCTION__, dwUserIndex, szBillingGUID.c_str(), szPrivateID.c_str(), szPublicID.c_str(), GetLastError()  );
		SP2Packet kPacket( BSTPK_OUTPUT_CASH_RESULT  );
		kPacket << dwUserIndex;
		kPacket << szBillingGUID;
		kPacket << CASH_RESULT_EXCEPT;
		kPacket << iType;
		pServerNode->SendMessage( kPacket );
		g_LogDBClient.OnInsertBillingServerError( CNT_WEMADEBUY, BILLING_ERROR_LOG_FAIL_ADD_BUYINFO,"fail add buy info." );
		return;
	}

	pInfo->m_szKey        = szPrivateID;
	pInfo->m_dwCreateTime = TIMEGETTIME();
	pInfo->m_eType				= BillInfoManager::AT_OUTPUT;
	pInfo->m_kPacket << dwUserIndex << szBillingGUID << iType << iChannelingType << szUserIP;
	pInfo->m_kPacket << pServerNode->GetIP() << pServerNode->GetClientPort();
	ioChannelingNodeParent::SetItemInfo( pInfo->m_kPacket, iType, kBuyInfo );
	pInfo->m_kPacket.SetPosBegin();

	if( !g_BillInfoManager->Add(pInfo))
	{
		LOG.PrintTimeAndLog( 0, "%s BillingInfo is full.: %d:%s:%s:%s:%d",__FUNCTION__, dwUserIndex, szBillingGUID.c_str(), szPrivateID.c_str(), szPublicID.c_str(), GetLastError()  );
		SP2Packet kPacket( BSTPK_OUTPUT_CASH_RESULT );
		kPacket << dwUserIndex;
		kPacket << szBillingGUID;
		kPacket << CASH_RESULT_EXCEPT;
		kPacket << iType;
		pServerNode->SendMessage( kPacket );
		g_LogDBClient.OnInsertBillingServerError( CNT_WEMADEBUY, BILLING_ERROR_LOG_FAIL_ADD_BUYINFO,"fail add buy info." );
		return;
	}

	BILL_PACK_PURCHASEITEM kInfo;
	kInfo.SetInfo( szPrivateID.c_str(), szBillingUserKey.c_str(), szPublicID.c_str(), Help::GetStringIPToDWORDIP( szUserIP.c_str() ), (WORD)iGameServerPort, dwGoodsNo, iPayAmt, rszGoodsName.c_str() );
	kInfo.Htonl();

	SP2Packet kPacket( BTUBTPK_BUY_REQUEST );
	kPacket << kInfo;
	if( !g_USBillingServer.SendMessage( kPacket ) )
	{
		LOG.PrintTimeAndLog( 0, "%s Fail Send: %d:%s:%s:%s:%d",__FUNCTION__, dwUserIndex, szBillingGUID.c_str(), szPrivateID.c_str(), szPublicID.c_str(), GetLastError()  );
		SP2Packet kPacket( BSTPK_OUTPUT_CASH_RESULT );
		kPacket << dwUserIndex;
		kPacket << szBillingGUID;
		kPacket << CASH_RESULT_EXCEPT;
		kPacket << iType;
		pServerNode->SendMessage( kPacket );

		g_BillInfoManager->Delete(szPrivateID);
		g_LogDBClient.OnInsertBillingServerError( CNT_WEMADEBUY, BILLING_ERROR_LOG_FAIL_SEND_WEMADE_BILLINGSERVER,"fail send billingserver." );
		return;
	}

	LOG.PrintTimeAndLog( 0, "%s Send Success: %d:%s:%s:%s:%s:Price %d:%d:%s",__FUNCTION__, dwUserIndex, szBillingGUID.c_str(), szPrivateID.c_str(), szPublicID.c_str()
		                                                                                 , szUserIP.c_str(), iPayAmt, dwGoodsNo, rszGoodsName.c_str() );
}

void ioLocalUS::OnRecieveOutputCash( const BILL_PACK_PURCHASEITEM& rkResult )
{
	ioHashString szPrivateID;
	char szTemp[MAX_PATH]="";
	StringCbPrintf( szTemp, sizeof( szTemp ), "%u", rkResult.UserNo );
	szPrivateID = szTemp;

	BillInfoManager::BillingInfo *pInfo = g_BillInfoManager->Get(szPrivateID);
	if( !pInfo )
	{
		LOG.PrintTimeAndLog( 0, "%s pInfo == NULL. %d:%s:%d:%d:%d:%s[%s]", __FUNCTION__, rkResult.UserNo, rkResult.UserID, rkResult.RealCash, rkResult.BonusCash, rkResult.RetCode, rkResult.RetMsg, rkResult.ChargeNo );
		return;
	}

	DWORD        dwUserIndex     = 0;
	ioHashString szBillingGUID;
	int          iType           = 0;
	int          iChannelingType = 0;
	ioHashString szUserIP;
	ioHashString szServerIP;
	int          iClientPort     = 0;
	pInfo->m_kPacket >> dwUserIndex >> szBillingGUID >> iType >> iChannelingType >> szUserIP >> szServerIP >> iClientPort;

	if( rkResult.RetCode != RESULT_SUCCESS )
	{
		ioHashString sResultMessage = rkResult.RetMsg;

		SP2Packet kPacket( BSTPK_OUTPUT_CASH_RESULT );
		kPacket << dwUserIndex;
		kPacket << szBillingGUID;
		kPacket << CASH_RESULT_EXCEPT;
		kPacket << iType;
		kPacket << true;
		kPacket << sResultMessage;

		if( !g_ServerNodeManager.SendMessageIP( szServerIP, iClientPort, kPacket ) )
		{
			g_BillInfoManager->Delete(szPrivateID);
			LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:%s:Ret %d:%d:%d[%s]", __FUNCTION__, rkResult.UserNo, szBillingGUID.c_str(), rkResult.UserID, rkResult.RetCode
				                                                                           , rkResult.RealCash, rkResult.BonusCash, rkResult.ChargeNo );
			g_LogDBClient.OnInsertBillingServerError( CNT_WEMADEBUY, BILLING_ERROR_LOG_DISCONNECT_GAMESERVER,"disconnect game server." );
			return;
		}

		g_BillInfoManager->Delete(szPrivateID);
		LOG.PrintTimeAndLog( 0, "%s Fail: %d:%s:%s:Ret %d:%s:%d:%d[%s]", __FUNCTION__, rkResult.UserNo, szBillingGUID.c_str(), rkResult.UserID,  rkResult.RetCode
			                                                                         , rkResult.RetMsg, rkResult.RealCash, rkResult.BonusCash, rkResult.ChargeNo );
		g_LogDBClient.OnInsertBillingServerError( CNT_WEMADEBUY, rkResult.RetCode,"Error return value." );
		return;
	}

	int iReturnCash    = ( rkResult.RealCash + rkResult.BonusCash );
	int iPurchasedCash = rkResult.RealCash;

	SP2Packet kPacket( BSTPK_OUTPUT_CASH_RESULT );
	kPacket << dwUserIndex;
	kPacket << szBillingGUID;
	kPacket << CASH_RESULT_SUCCESS;
	kPacket << iType;
	kPacket << rkResult.ChargedCashAmt;
	kPacket << 0; // TransactionID ( FOR US )

	ioChannelingNodeParent::ItemInfo kItemInfo;
	ioChannelingNodeParent::GetItemInfo( pInfo->m_kPacket, iType, kItemInfo );
	ioChannelingNodeParent::SetItemInfo( kPacket, iType, kItemInfo );

	ioHashString sBillingUserKey = rkResult.UserID;
	ioHashString sChargeNo       = rkResult.ChargeNo;

	// Cancel Step 1
	kPacket << iChannelingType;  // 공통
	kPacket << iReturnCash;
	kPacket << iPurchasedCash;
	kPacket << szPrivateID;
	kPacket << sBillingUserKey;
	kPacket << sChargeNo;

	if( !g_ServerNodeManager.SendMessageIP( szServerIP, iClientPort, kPacket ) )
	{
		LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:%s:Ret %d:%d:%d[%s]", __FUNCTION__, rkResult.UserNo, szBillingGUID.c_str(), rkResult.UserID,  rkResult.RetCode, rkResult.RealCash, rkResult.BonusCash, rkResult.ChargeNo );
		g_LogDBClient.OnInsertBillingServerError( CNT_WEMADEBUY, BILLING_ERROR_LOG_DISCONNECT_GAMESERVER,"disconnect game server." );
		g_BillInfoManager->Delete(szPrivateID);
		return;
	}

	g_BillInfoManager->Delete(szPrivateID);
	LOG.PrintTimeAndLog( 0, "%s Success: %d:%s:%s:Ret %d:%d:%d:%d[%s]", __FUNCTION__, rkResult.UserNo, szBillingGUID.c_str(), rkResult.UserID,  rkResult.RetCode
		                                                                            , rkResult.RealCash, rkResult.BonusCash, rkResult.ChargedCashAmt, rkResult.ChargeNo );
}

void ioLocalUS::OnCancelCash( ServerNode *pServerNode, SP2Packet &rkPacket )
{
	int          iChannelingType = 0;
	ioHashString szBillingGUID;
	DWORD        dwUserIndex = 0;
	ioHashString szPrivateID;
	ioHashString szBillingUserKey;
	ioHashString szChargeNo;

	// 공통
	rkPacket >> iChannelingType; 
	rkPacket >> szBillingGUID; 
	rkPacket >> dwUserIndex; 

	// Cancel Step 4
	rkPacket >> szPrivateID;
	rkPacket >> szBillingUserKey;
	rkPacket >> szChargeNo;

	BILL_PACK_CNLPURCHASE kInfo;
	kInfo.SetInfo( szPrivateID.c_str(), szBillingUserKey.c_str(), szChargeNo.c_str() );

	SP2Packet kPacket( BTUBTPK_CANCEL_REQUEST );
	kPacket << kInfo;
	kInfo.Htonl();

	if( !g_USBillingServer.SendMessage( kPacket ) )
	{
		LOG.PrintTimeAndLog( 0, "%s Fail Send: %d:%s:%s:%s:%d",__FUNCTION__, dwUserIndex, szBillingGUID.c_str(), szPrivateID.c_str(), szChargeNo.c_str(), GetLastError()  );
		g_LogDBClient.OnInsertBillingServerError( CNT_WEMADEBUY, BILLING_ERROR_LOG_FAIL_SEND_WEMADE_BILLINGSERVER,"fail send billingserver." );
		return;
	}

	//LOG.PrintTimeAndLog( 0, "%s Send: %d:%s:%s:%s:[%s]",__FUNCTION__, dwUserIndex, szBillingGUID.c_str(), szPrivateID.c_str(), szChargeNo.c_str(), szUserIP.c_str() );
}
