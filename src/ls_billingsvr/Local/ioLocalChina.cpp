#include "../stdafx.h"
#include "./ioLocalChina.h"
#include "../Util/md5.h"
#include "../Util/cJSON.h"
#include "../MainProcess.h"
#include "../NodeInfo/ServerNode.h"
#include "../DataBase/LogDBClient.h"
#include "../Channeling/ioChannelingNodeParent.h"

extern CLog LOG;

ioLocalChina::ioLocalChina(void)
{
}

ioLocalChina::~ioLocalChina(void)
{
}

ioLocalManager::LocalType ioLocalChina::GetType()
{
	return ioLocalManager::LCT_CHINA;
}


void ioLocalChina::Init()
{
	/*
	ioINILoader kLoader( "BillingRelayServerInfo.ini" );
	kLoader.SetTitle( "Local" );

	char szKreonLoginURL[MAX_PATH]="";
	kLoader.LoadString( "KreonLoginURL", "", szKreonLoginURL, sizeof( szKreonLoginURL ) );
	m_sLoginURL = szKreonLoginURL;

	char szKreonBillingGetURL[MAX_PATH]="";
	kLoader.LoadString( "KreonBillingGetURL", "", szKreonBillingGetURL, sizeof( szKreonBillingGetURL ) );
	m_sBillingGetURL = szKreonBillingGetURL;

	char szKreonBillingOutPutURL[MAX_PATH]="";
	kLoader.LoadString( "KreonBillingOutPutURL", "", szKreonBillingOutPutURL, sizeof( szKreonBillingOutPutURL ) );
	m_sBillingOutPutURL = szKreonBillingOutPutURL;

	if( m_sLoginURL.IsEmpty() || m_sBillingGetURL.IsEmpty() || m_sBillingOutPutURL.IsEmpty() )
		MessageBox( NULL, "Error Kreon URL", "Error" , MB_OK );
	*/
}

void ioLocalChina::OnLoginData( ServerNode *pServerNode, SP2Packet &rkPacket )
{
	ioHashString szBillingGUID;
	ioHashString szPrivateID;
	ioHashString sEncodePW;
	ioHashString szPublicIP;
	DWORD        dwReturnMsgType;

	rkPacket >> szBillingGUID >> szPrivateID >> sEncodePW >> szPublicIP >> dwReturnMsgType;

	if( !pServerNode )
	{
		LOG.PrintTimeAndLog(0, "%s pServerNode == NULL:%s:%s:%s:%s", __FUNCTION__, szBillingGUID.c_str(), szPrivateID.c_str(), sEncodePW.c_str(), szPublicIP.c_str() );
		return;
	}

	//if( g_App.IsReserveLogOut() )
	{
		LOG.PrintTimeAndLog(0, "%s LogOut:%s:%s:%s", __FUNCTION__, szBillingGUID.c_str(), szPrivateID.c_str(), sEncodePW.c_str() );
		SP2Packet kPacket( dwReturnMsgType );
		kPacket << szPrivateID;
		kPacket << szBillingGUID;
		kPacket << BILLING_LOGIN_RESULT_FAIL;
		kPacket << false;
		pServerNode->SendMessage( kPacket );
		return;
	}

/*	enum { MAX_RANDOM_KEY = 20, };
	char szRandomKey[MAX_PATH]="";
	if( sEncodePW.Length() > MAX_RANDOM_KEY )
		StringCbCopyN( szRandomKey, sizeof( szRandomKey ), sEncodePW.c_str(), MAX_RANDOM_KEY );
	char szEncode[MAX_PATH]="";
	int iEncodeCnt = 0;
	int iEncodPwLength = sEncodePW.Length();
	for (int i = 0; i < iEncodPwLength; i++)
	{
		if( i >= MAX_RANDOM_KEY )
		{
			szEncode[iEncodeCnt] = sEncodePW.At(i);
			iEncodeCnt++;
			if( iEncodeCnt >= MAX_PATH )
				break;
		}
	}
	char szPW[MAX_PATH]="";
	char szUserKey[MAX_PATH]="";
	StringCbPrintf( szUserKey, sizeof( szUserKey ), "%s%s", szPrivateID.c_str(), szRandomKey );
	Decode( szEncode, strlen( szEncode ), szPW, sizeof( szPW ), szUserKey, strlen( szUserKey ) );

	char szReturnValue[WEB_BUFF_SIZE]="";
	ZeroMemory( szReturnValue, WEB_BUFF_SIZE );

	char szHexMD5PW[MAX_PATH]="";
	ZeroMemory( szHexMD5PW, sizeof( szHexMD5PW ) );
	GetHexMD5( szHexMD5PW, sizeof( szHexMD5PW ), szPW );

	SYSTEMTIME st;
	GetLocalTime( &st );
	char szDate[MAX_PATH]="";
	StringCbPrintf( szDate, sizeof( szDate ), "%04d%02d%02d%02d%02d%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond );

	char szUniteValue[MAX_PATH*2]="";
	StringCbPrintf( szUniteValue, sizeof( szUniteValue ), "%s%s%s%s%s", szPublicIP.c_str(), szPrivateID.c_str(), szDate, SHOP_ID, szPW  );

	char szHexMD5Key[MAX_PATH]="";
	ZeroMemory( szHexMD5Key, sizeof( szHexMD5Key ) );
	GetHexMD5( szHexMD5Key, sizeof( szHexMD5Key ), szUniteValue );

	char szPostData[MAX_PATH*4]="";
	StringCbPrintf( szPostData, sizeof( szPostData ), "id=%s&pw=%s&ip=%s&dt=%s&svc_code=%s&key=%s", szPrivateID.c_str(), szHexMD5PW, szPublicIP.c_str(), szDate, SHOP_ID, szHexMD5Key );

	ioHTTP Winhttp; //kyg 확인 필요 
	if( !Winhttp.GetResultData( m_sLoginURL.c_str(), szPostData, szReturnValue, sizeof( szReturnValue ) , false ) )
	{
		LOG.PrintTimeAndLog( 0, "%s Http Error: %s:%s:%d", __FUNCTION__, szBillingGUID.c_str(), szPrivateID.c_str(), GetLastError() );
		SP2Packet kPacket( dwReturnMsgType  );
		kPacket << szPrivateID;;
		kPacket << szBillingGUID;
		kPacket << BILLING_LOGIN_RESULT_FAIL;
		kPacket << false;
		pServerNode->SendMessage( kPacket );
		g_LogDBClient.OnInsertBillingServerError( CNT_WEMADEBUY, BILLING_ERROR_LOG_EXCEPTION,"Login exception HTTP." );
		return;
	}

	if( strncmp( szReturnValue, "00", 2 )  != 0 )
	{
		LOG.PrintTimeAndLog(0, "%s Return Error.:%s:%s:%s[%s:%s:%s]", __FUNCTION__, szBillingGUID.c_str(), szPrivateID.c_str(), szReturnValue, szPW, szUserKey, sEncodePW.c_str() );

		char szToken[MAX_PATH]="";
		szToken[0] = 9; // 구분자
		// E01HTUser password is not correctHT 에서 User password is not correct을 추출
		char *pPos = strtok( szReturnValue, szToken );
		if( pPos != NULL )
			pPos = strtok( NULL, szToken );
		
		SP2Packet kPacket( dwReturnMsgType );
		kPacket << szPrivateID;
		kPacket << szBillingGUID;
		kPacket << BILLING_LOGIN_RESULT_FAIL;
		if( pPos != NULL )
		{
			kPacket << true;
			kPacket << pPos;
		}
		else
		{
			kPacket << false;
		}
		pServerNode->SendMessage( kPacket );
		g_LogDBClient.OnInsertBillingServerError( CNT_WEMADEBUY, BILLING_ERROR_LOG_EXCEPTION,"Login Return Error." );
		return;
	}

	LOG.PrintTimeAndLog(0, "%s Complete.:%s:%s:%s", __FUNCTION__, szBillingGUID.c_str(), szPrivateID.c_str(), szReturnValue );

	char szToken[MAX_PATH]="";
	szToken[0] = 9; // 구분자
	// 00HT234513HTHotGamerHTgemscool01HTgemcool test userHTMHT2007-10-13HT33300HT1HT 에서 234513빌링에서 사용하는 유저넘버
	char *pPos = strtok( szReturnValue, szToken );
	if( pPos != NULL )
		pPos = strtok( NULL, szToken );
		
	SP2Packet kPacket( dwReturnMsgType );
	kPacket << szPrivateID;
	kPacket << szBillingGUID;
	kPacket << BILLING_LOGIN_RESULT_SUCCESS;
	if( pPos != NULL )
		kPacket << pPos;
	pServerNode->SendMessage( kPacket );
	*/
}

void ioLocalChina::_OnGetCash( ServerNode *pServerNode, SP2Packet &rkPacket )
{
	int          iChannelingType   = 0;
	ioHashString szBillingGUID;
	DWORD        dwUserIndex       = 0;
	ioHashString szPrivateID;
	ioHashString szPublicID;
	bool         bSetUserMouse     = false;
	ioHashString szBillingUserKey;

	rkPacket >> iChannelingType >> szBillingGUID >> dwUserIndex >> szPrivateID >> szPublicID >> bSetUserMouse; // 공통사항
	rkPacket >> szBillingUserKey;

	if( !pServerNode )
	{
		LOG.PrintTimeAndLog(0, "%s pServerNode == NULL:%s:%d:%s:%s", __FUNCTION__, szBillingGUID.c_str(), dwUserIndex, szPrivateID.c_str(), szPublicID.c_str() );
		return;
	}
	
	//if( g_App.IsReserveLogOut() )
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
	/*

	char szReturnValue[WEB_BUFF_SIZE]="";
	ZeroMemory( szReturnValue, WEB_BUFF_SIZE );
	
	char szUniteValue[MAX_PATH*2]="";
	StringCbPrintf( szUniteValue, sizeof( szUniteValue ), "%s%s%s", SHOP_KEY, szBillingUserKey.c_str(), SHOP_ID );
	
	char szHexMD5Key[MAX_PATH]="";
	ZeroMemory( szHexMD5Key, sizeof( szHexMD5Key ) );
	GetHexMD5( szHexMD5Key, sizeof( szHexMD5Key ), szUniteValue );
	
	char szFullURL[MAX_PATH*4]="";
	StringCbPrintf( szFullURL, sizeof( szFullURL ), "%s?SID=%s&CN=%s&A-KEY=%s" , m_sBillingGetURL.c_str(), SHOP_ID, szBillingUserKey.c_str(), szHexMD5Key );
	
	ioHTTP Winhttp; //kyg 확인 필요 
	if( !Winhttp.GetResultData( szFullURL, szReturnValue, sizeof( szReturnValue ) ) )
	{
		LOG.PrintTimeAndLog( 0, "%s Http Error: %d:%s:%s:%s:%d", __FUNCTION__, dwUserIndex, szBillingGUID.c_str(), szPrivateID.c_str(), szPublicID.c_str(), GetLastError() );
		SP2Packet kPacket( BSTPK_GET_CASH_RESULT );
		kPacket << dwUserIndex;
		kPacket << szBillingGUID;
		kPacket << bSetUserMouse;
		kPacket << CASH_RESULT_EXCEPT;
		kPacket << false;
		pServerNode->SendMessage( kPacket );
		g_LogDBClient.OnInsertBillingServerError( CNT_WEMADEBUY, BILLING_ERROR_LOG_EXCEPTION,"Billing exception GetCash(HTTP)." );
		return;
	}*/
/*
{
"RESULT-CODE" : "S000",
“RESULT-MESSAGE” : “Success”
"SID" : "SHOP01",
“CN” : “32239342”,
“UID” : “testid”
"CASH-BALANCE" : “35000” 
}

S000 Success
E001 Can not find shop id
E002 CN or UID not found
E003 Auth key not found
E004 Do not match the auth key
E005 System error
E200 Can't find user's billing account	
*/	
	/*
	if( strcmp( szReturnValue , "" ) == 0 )
	{
		LOG.PrintTimeAndLog( 0, "%s Return Empty: %d:%s:%s:%s:%d[%s]", __FUNCTION__, dwUserIndex, szBillingGUID.c_str(), szPrivateID.c_str(), szPublicID.c_str(), GetLastError(), szReturnValue );
		SP2Packet kPacket( BSTPK_GET_CASH_RESULT );
		kPacket << dwUserIndex;
		kPacket << szBillingGUID;
		kPacket << bSetUserMouse;
		kPacket << CASH_RESULT_EXCEPT;
		kPacket << false;
		pServerNode->SendMessage( kPacket );
		g_LogDBClient.OnInsertBillingServerError( CNT_WEMADEBUY, BILLING_ERROR_LOG_EXCEPTION,"Billing exception Gatcash(ReturnEmpty)." );
		return;
	}
	
	ioHashStringVec vKeyVec;
	vKeyVec.reserve(2);
	vKeyVec.push_back( ioHashString( "RESULT-CODE" ) );
	vKeyVec.push_back( ioHashString( "CASH-BALANCE" ) );
	ioHashStringVec vValueVec;
	vValueVec.reserve(2);
	vValueVec.push_back( ioHashString( "" ) );
	vValueVec.push_back( ioHashString( "" ) );

	if( !GetParseJSon( szReturnValue, vKeyVec, vValueVec ) )
	{
		LOG.PrintTimeAndLog( 0, "%s Json Error: %d:%s:%s:%s:%d[%s]", __FUNCTION__, dwUserIndex, szBillingGUID.c_str(), szPrivateID.c_str(), szPublicID.c_str(), GetLastError(), szReturnValue );
		SP2Packet kPacket( BSTPK_GET_CASH_RESULT );
		kPacket << dwUserIndex;
		kPacket << szBillingGUID;
		kPacket << bSetUserMouse;
		kPacket << CASH_RESULT_EXCEPT;
		kPacket << false;
		pServerNode->SendMessage( kPacket );
		g_LogDBClient.OnInsertBillingServerError( CNT_WEMADEBUY, BILLING_ERROR_LOG_EXCEPTION,"Billing exception Gatcash(JsonCrash)." );
		return;
	}
	
	if( strcmp( vValueVec[0].c_str(), "S000" ) != 0 )
	{
		LOG.PrintTimeAndLog( 0, "%s KreonError: %d:%s:%s:%s:%d[%s:%s]", __FUNCTION__, dwUserIndex, szBillingGUID.c_str(), szPrivateID.c_str(), szPublicID.c_str(), GetLastError(), vValueVec[0].c_str(), szReturnValue );
		SP2Packet kPacket( BSTPK_GET_CASH_RESULT );
		kPacket << dwUserIndex;
		kPacket << szBillingGUID;
		kPacket << bSetUserMouse;
		kPacket << CASH_RESULT_EXCEPT;
		kPacket << true;
		kPacket << vValueVec[0];
		pServerNode->SendMessage( kPacket );
		g_LogDBClient.OnInsertBillingServerError( CNT_WEMADEBUY, BILLING_ERROR_LOG_EXCEPTION,"Billing exception Gatcash(KreonError)." );
		return;
	}
	
	int iTotalCash = atoi( vValueVec[1].c_str() );
	
	SP2Packet kPacket( BSTPK_GET_CASH_RESULT );
	kPacket << dwUserIndex;
	kPacket << szBillingGUID;
	kPacket << bSetUserMouse;
	kPacket << CASH_RESULT_SUCCESS;
	kPacket << iTotalCash;
	kPacket << iTotalCash;  // total cash와 purchase cash가 동일함.
	pServerNode->SendMessage( kPacket );
	
	LOG.PrintTimeAndLog( 0, "%s Success: %d:%s:PrivateID %s:%s:%d[%s]", __FUNCTION__, dwUserIndex, szBillingGUID.c_str(), szPrivateID.c_str(), szPublicID.c_str(), iTotalCash, szReturnValue  );
	*/
}

void ioLocalChina::_OnOutputCash( ServerNode *pServerNode, SP2Packet &rkPacket, DWORD dwGoodsNo, const ioHashString &rszGoodsName )
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

	//if( g_App.IsReserveLogOut() )
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
/*
	ioChannelingNodeParent::ItemInfo kBuyInfo;
	ioChannelingNodeParent::GetItemInfo( rkPacket, iType, kBuyInfo );

	// local 별 add 값은 GetItemInfo 후에 한다.
	rkPacket >> szBillingUserKey;
	rkPacket >> iUserGradeLevel;

	char szReturnValue[WEB_BUFF_SIZE]="";
	ZeroMemory( szReturnValue, WEB_BUFF_SIZE );

	char szUniteValue[MAX_PATH*2]="";
	StringCbPrintf( szUniteValue, sizeof( szUniteValue ), "%s1%s%d%s", SHOP_KEY, szBillingUserKey.c_str(), iPayAmt, SHOP_ID  );

	char szHexMD5Key[MAX_PATH]="";
	ZeroMemory( szHexMD5Key, sizeof( szHexMD5Key ) );
	GetHexMD5( szHexMD5Key, sizeof( szHexMD5Key ), szUniteValue );

	char szPostData[MAX_PATH*4]="";
	StringCbPrintf( szPostData, sizeof( szPostData ), "SHOP-ID=%s&BUYER-CN=%s&BUYER-LEVEL=%d&PG-CODE=P001&TOTAL-COUNT=1&TOTAL-PRICE=%d&P1-CODE=%d&P1-TYPE=C033&P1-NAME=%s&P1-COUNT=1&P1-PRICESUM=%d&A-KEY=%s", SHOP_ID, szBillingUserKey.c_str(), iUserGradeLevel, iPayAmt, dwGoodsNo, rszGoodsName.c_str(), iPayAmt, szHexMD5Key );

	ioHTTP Winhttp; //kyg 확인 필요 
	if( !Winhttp.GetResultData( m_sBillingOutPutURL.c_str(), szPostData, szReturnValue, sizeof( szReturnValue ) , false ) )
	{
		LOG.PrintTimeAndLog( 0, "%s Http Error: %d:%s:%s:%s:%s:%d", __FUNCTION__, dwUserIndex, szBillingGUID.c_str(), szPrivateID.c_str(), szPublicID.c_str(), szBillingUserKey.c_str(), GetLastError() );
		SP2Packet kPacket( BSTPK_OUTPUT_CASH_RESULT );
		kPacket << dwUserIndex;
		kPacket << szBillingGUID;
		kPacket << CASH_RESULT_EXCEPT;
		kPacket << iType;
		kPacket << false;
		pServerNode->SendMessage( kPacket );
		g_LogDBClient.OnInsertBillingServerError( CNT_NAVER, BILLING_ERROR_LOG_EXCEPTION,"Billing exception Http." );
		return;
	}*/
/*
{
"RESULT-CODE" : "S000",
"RESULT-MESSAGE" : "Payment Success",
“CASH-BALANCE” : 25700,
"SHOP-ID" : "S0001", 
"SHOP-TRNO" : "TR0032401", 
"USER-IP" : "123.123.123.123", 
"BUYER-CN", "CN001",
"BUYER-CHARATER", "Plutos_Abel",
"BUYER-SERVER", "Pluto",
"BUYER-LEVEL", "29",
"PG-CODE", "P001",
"TOTAL-COUNT", "2",
“TOTAL-PRICE”, “7700”,

"PRODUCT-LIST" : 
[
{
"CODE" : "P032301",
"TYPE" : "1", 
"NAME" : "AK-47자동화기(20일이용권)", 
"COUNT" : "2", 
"CATEGORY" : "총기류", 
"DEFAULTPRICE" : "2500", 
"PRICESUM" : "5000", 
},
{
"CODE" : "P001024",
"TYPE" : "1", 
"NAME" : "즉시 리스폰 물약", 
"COUNT" : "1", 
"CATEGORY" : "물약", 
"DEFAULTPRICE" : "3000", 
"EVENTCODE" : "E001",
"EVENTNAME" : "10% 할인 이벤트",
"PRICESUM" : "2700", 
}
]

}
*/
/*
	if( strcmp( szReturnValue , "" ) == 0 )
	{
		LOG.PrintTimeAndLog( 0, "%s Return empty: %d:%s:%s:%s:%s:%d", __FUNCTION__, dwUserIndex, szBillingGUID.c_str(), szPrivateID.c_str(), szPublicID.c_str(), szBillingUserKey.c_str(), GetLastError() );
		SP2Packet kPacket( BSTPK_OUTPUT_CASH_RESULT );
		kPacket << dwUserIndex;
		kPacket << szBillingGUID;
		kPacket << CASH_RESULT_EXCEPT;
		kPacket << iType;
		kPacket << false;
		pServerNode->SendMessage( kPacket );
		g_LogDBClient.OnInsertBillingServerError( CNT_NAVER, BILLING_ERROR_LOG_EXCEPTION,"Billing exception Return empty." );
		return;
	}

	ioHashStringVec vKeyVec;
	vKeyVec.reserve(2);
	vKeyVec.push_back( ioHashString( "RESULT-CODE" ) );
	vKeyVec.push_back( ioHashString( "CASH-BALANCE" ) );
	ioHashStringVec vValueVec;
	vValueVec.reserve(2);
	vValueVec.push_back( ioHashString( "" ) );
	vValueVec.push_back( ioHashString( "" ) );

	if( !GetParseJSon( szReturnValue, vKeyVec, vValueVec ) )
	{
		LOG.PrintTimeAndLog( 0, "%s Json Error: %d:%s:%s:%s:%s:%d[%s]", __FUNCTION__, dwUserIndex, szBillingGUID.c_str(), szPrivateID.c_str(), szPublicID.c_str(), szBillingUserKey.c_str(), GetLastError(), szReturnValue );
		SP2Packet kPacket( BSTPK_OUTPUT_CASH_RESULT );
		kPacket << dwUserIndex;
		kPacket << szBillingGUID;
		kPacket << CASH_RESULT_EXCEPT;
		kPacket << iType;
		kPacket << false;
		pServerNode->SendMessage( kPacket );
		g_LogDBClient.OnInsertBillingServerError( CNT_NAVER, BILLING_ERROR_LOG_EXCEPTION,"Billing exception Json Error." );
		return;
	}

	if( strcmp( vValueVec[0].c_str(), "S000" ) != 0 )
	{
		LOG.PrintTimeAndLog( 0, "%s KreonError: %d:%s:%s:%s:%s:%d[%s:%s]<%s>", __FUNCTION__, dwUserIndex, szBillingGUID.c_str(), szPrivateID.c_str(), szPublicID.c_str(), szBillingUserKey.c_str(), GetLastError(), vValueVec[0].c_str(), szReturnValue, szPostData );
		SP2Packet kPacket( BSTPK_OUTPUT_CASH_RESULT );
		kPacket << dwUserIndex;
		kPacket << szBillingGUID;
		kPacket << CASH_RESULT_EXCEPT;
		kPacket << iType;
		kPacket << true;
		kPacket << vValueVec[0];
		pServerNode->SendMessage( kPacket );
		g_LogDBClient.OnInsertBillingServerError( CNT_WEMADEBUY, BILLING_ERROR_LOG_EXCEPTION,"Billing exception OutputCash(KreonError)." );
		return;
	}

	int iTotalCash = atoi( vValueVec[1].c_str() );

	SP2Packet kPacket( BSTPK_OUTPUT_CASH_RESULT );
	kPacket << dwUserIndex;
	kPacket << szBillingGUID;
	kPacket << CASH_RESULT_SUCCESS;
	kPacket << iType;
	kPacket << iPayAmt;
	kPacket << 0; // TransactionID ( FOR US )
	ioChannelingNodeParent::SetItemInfo( kPacket, iType, kBuyInfo );
	kPacket << iChannelingType;  // 공통
	kPacket << iTotalCash;
	kPacket << iTotalCash; // 캐쉬와 실재 구매한 캐쉬가 동일하다.
	pServerNode->SendMessage( kPacket ); 

	LOG.PrintTimeAndLog(0, "%s Success. :%d:%s:PrivateID %s:%s:%s[%d:%s:%d:%d][%s]", __FUNCTION__, dwUserIndex, szBillingGUID.c_str(),szPrivateID.c_str(), szPublicID.c_str(), szBillingUserKey.c_str(), dwGoodsNo, rszGoodsName.c_str(), iPayAmt, iTotalCash, szReturnValue );
	*/
}

void ioLocalChina::OnRefundCash( ServerNode *pServerNode, SP2Packet &rkPacket )
{

}

void ioLocalChina::OnUserInfo( ServerNode *pServerNode, SP2Packet &rkPacket )
{

}
/*
void ioLocalChina::GetHexMD5( OUT char *szHexMD5, IN int iHexSize, IN const char *szSource )
{
	enum { MAX_DIGEST = 16, };
	MD5Context md5_ctx;
	BYTE byDigest[MAX_DIGEST];

	MD5Init( &md5_ctx );
	MD5Update( &md5_ctx, (unsigned char const *)szSource, strlen( szSource ) );
	MD5Final( byDigest, &md5_ctx );

	for (int i = 0; i < MAX_DIGEST ; i++)
	{
		char szTempHex[MAX_PATH]="";
		StringCbPrintf(szTempHex, sizeof( szTempHex ), "%02x", byDigest[i]); // BYTE 캐스팅해서 FFFF붙지 않는다.
		StringCbCat( szHexMD5, iHexSize, szTempHex );	
	}
}

void ioLocalChina::EncryptDecryptData2( OUT char *szResultData, IN const int iResultSize, IN const char *szSourceData, IN const int iSourceSize, IN const char *szUserKey, IN int iUserKeySize )
{
	enum { MAX_KEY = 30, };
	BYTE byKey[MAX_KEY]={ 1,56,211,49,67,190,123,231,34,6,8,9,56,23,90,8,124,126,137,59,34,23,90,200,201,202,39,98,96,21};

	for(int i =0; i < iSourceSize; i++)
	{
		if( i >= iResultSize ) break;
		szResultData[i] = szSourceData[i] ^ byKey[i%MAX_KEY];
		szResultData[i] = szResultData[i] ^ szUserKey[i%iUserKeySize];
		szResultData[i] = szResultData[i] ^ byKey[( (MAX_KEY-1)-i )%MAX_KEY];
		szResultData[i] = szResultData[i] ^ szUserKey[( (iUserKeySize-1)-i )%iUserKeySize];
	}
}

void ioLocalChina::Encode( IN const char* szPlain, IN int iPlainSize, OUT char *szCipher, IN int iCipherSize, IN const char *szUserKey, IN int iUserKey )
{
	EncryptDecryptData2( szCipher, iCipherSize, szPlain, iPlainSize, szUserKey, iUserKey );

	// char를 16진수로 변경
	char szTemp[MAX_PATH]="";
	for(int i = 0; i < iPlainSize; i++)
	{
		char szTempHex[MAX_PATH]="";
		StringCbPrintf(szTempHex, sizeof( szTempHex ), "%02x", (BYTE)szCipher[i]); // BYTE 캐스팅해서 FFFF붙지 않는다.
		StringCbCat(szTemp, sizeof( szTemp ),  szTempHex);
	}
	StringCbCopy(szCipher, iCipherSize, szTemp);
}

void ioLocalChina::Decode( IN const char *szCipher, IN int iCipherSize, OUT char* szPlain, IN int iPlainSize, IN const char *szUserKey, IN int iUserKey  )
{
	char szCharCipher[MAX_PATH]="";
	int  iCharChipherSize = iCipherSize/2;
	// 16진수 -> char
	int pos = 0;
	for(int i = 0; i < iCharChipherSize; i++)
	{
		char szTempOneHex[MAX_PATH]="";
		char *stopstring;
		if( pos >= iCipherSize )
			break;
		memcpy(szTempOneHex, &szCipher[pos], 2);
		pos += 2;
		if( i >= MAX_PATH )
			break;
		szCharCipher[i] = (BYTE)strtol(szTempOneHex, &stopstring, 16);
	}

	EncryptDecryptData2( szPlain, iPlainSize, szCharCipher, iCharChipherSize, szUserKey, iUserKey );
}

bool ioLocalChina::GetParseJSon( IN const char * szJsonSource, IN const ioHashStringVec &rvKeyVec, OUT ioHashStringVec &rvValueVec )
{
	try 
	{
		cJSON *pJson = NULL;
		pJson=cJSON_Parse(szJsonSource);
		if( pJson == NULL )
			return false;

		int iSize      = rvKeyVec.size();
		int iValueSize = rvValueVec.size();
		if( iValueSize < iSize )
			iSize = iValueSize;

		for (int i = 0; i < iSize ; i++)
		{
			const ioHashString &rKey = rvKeyVec[i];
			if( rKey.IsEmpty() )
				continue;

			cJSON *pObject = cJSON_GetObjectItem( pJson, rKey.c_str() );
			if( pObject )
			{
				char szValue[MAX_PATH]="";
				if( pObject->type == cJSON_Number )
				{
					StringCbPrintf( szValue, MAX_PATH, "%d", pObject->valueint );
					rvValueVec[i] = szValue;

				}
				else if( pObject->type == cJSON_String )
				{
					StringCbCopy( szValue, MAX_PATH, pObject->valuestring );
					rvValueVec[i] = szValue;
				}
			}
		}

		cJSON_Delete(pJson);
		return true;
	}
	catch ( ... )
	{
		LOG.PrintTimeAndLog( 0, "%s Json Crash %s", __FUNCTION__, szJsonSource );
		return false;
	}

	return false;
}
*/