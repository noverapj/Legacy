#include "../stdafx.h"
#include "./ioLocalIndonesia.h"
#include "../MainProcess.h"
#include "../NodeInfo/ServerNode.h"
#include "../DataBase/LogDBClient.h"
#include "../Channeling/ioChannelingNodeParent.h"
#include "../ThreadPool/ioThreadPool.h"
#include "../NodeInfo/ServerNodeManager.h"
#include "../EtcHelpFunc.h"

extern CLog LOG;

ioLocalIndonesia::ioLocalIndonesia(void)
{
}

ioLocalIndonesia::~ioLocalIndonesia(void)
{
}

ioLocalManager::LocalType ioLocalIndonesia::GetType()
{
	return ioLocalManager::LCT_INDONESIA;
}

void ioLocalIndonesia::Init()
{
	ioINILoader kLoader( "ls_config_billingsvr.ini" );
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

	char szKreonPCRoomURL[MAX_PATH]="";
	kLoader.LoadString( "KreonPCRoomURL", "", szKreonPCRoomURL, sizeof( szKreonPCRoomURL ) );
	m_sPCRoomURL = szKreonPCRoomURL;

	if( m_sLoginURL.IsEmpty() || m_sBillingGetURL.IsEmpty() || m_sBillingOutPutURL.IsEmpty() || m_sPCRoomURL.IsEmpty() )
		LOG.PrintTimeAndLog(0,"Error Kreon URL");
		
}

void ioLocalIndonesia::OnLoginData( ServerNode *pServerNode, SP2Packet &rkPacket )
{
	ioHashString szBillingGUID;
	ioHashString szPrivateID;
	ioHashString sEncodePW;
	ioHashString szPublicIP;
	DWORD        dwReturnMsgType;
	ioHashString szMacAddress;
	ioHashString szKey;

	ioHashString szAuthInfo;
	DWORD			dwSessionID;

	szAuthInfo.Clear();
	dwSessionID = 0;

	rkPacket >> szBillingGUID >> szPrivateID >> sEncodePW >> szPublicIP >> dwReturnMsgType; // 공통

	rkPacket >> szMacAddress >> szKey; // id
	
	if(dwReturnMsgType == BSTPK_LOGIN_RESULT)
	{
		rkPacket >> szAuthInfo >> dwSessionID;
	}

	if( !pServerNode )
	{
		LOG.PrintTimeAndLog(0, "%s pServerNode == NULL:%s:%s:%s:%s", __FUNCTION__, szBillingGUID.c_str(), szPrivateID.c_str(), sEncodePW.c_str(), szPublicIP.c_str() );
		return;
	}

	ioData kData;
	kData.SetBillingGUID( szBillingGUID );
	kData.SetPrivateID( szPrivateID );
	kData.SetEncodePW( sEncodePW );
	kData.SetUserIP( szPublicIP );
	kData.SetReturnMsgType( dwReturnMsgType );
	kData.SetUserMacAddress( szMacAddress );		
	kData.SetUserKey( szKey );
	kData.SetAuthInfo(szAuthInfo);
	kData.SetSessionID(dwSessionID);

	if( pServerNode )
	{
		kData.SetServerIP( pServerNode->GetIP() );
		kData.SetServerPort( pServerNode->GetClientPort() );
	}
	kData.SetEmpty( false );
	kData.SetProcessType( ioData::PT_LOGIN );

	g_ThreadPool.SetData( kData );
}

void ioLocalIndonesia::_OnGetCash( ServerNode *pServerNode, SP2Packet &rkPacket )
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
	
	ioData kData;
	kData.SetChannelingType( iChannelingType );
	kData.SetBillingGUID( szBillingGUID );
	kData.SetUserIndex( dwUserIndex );
	kData.SetPrivateID( szPrivateID );
	kData.SetPublicID( szPublicID );
	kData.SetSetUserMouse( bSetUserMouse );
	kData.SetUserNo( szBillingUserKey );
	if( pServerNode )
	{
		kData.SetServerIP( pServerNode->GetIP() );
		kData.SetServerPort( pServerNode->GetClientPort() );
	}
	kData.SetEmpty( false );
	kData.SetProcessType( ioData::PT_GET_CASH );

	g_ThreadPool.SetData( kData );
}

void ioLocalIndonesia::_OnOutputCash( ServerNode *pServerNode, SP2Packet &rkPacket, DWORD dwGoodsNo, const ioHashString &rszGoodsName )
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

	// local 별 add 값은 GetItemInfo 후에 한다.
	rkPacket >> szBillingUserKey;
	rkPacket >> iUserGradeLevel;

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
	kData.SetUserNo( szBillingUserKey );
	kData.SetUserLevel( iUserGradeLevel );
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

//g_ThreadPool.SetData( kData ); 여기서 데이터 가지고옴 
void ioLocalIndonesia::ThreadLogin( const ioData &rData, LoginManager &rLoginMgr )
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

	DWORD dwRCRoomNum = 0;

	//클라이언트에서 로그인 패킷 날라왔을 때 
	if( rData.GetReturnMsgType() == BSTPK_LOGIN_RESULT )
	{
		ioHashString szReturn;

		rLoginMgr.GetReturnValue(rData.GetPrivateID(), szReturn);

		if(ThreadPCRoom( rData, szReturn, dwRCRoomNum ))
		{
			rLoginMgr.SetPCroomNum(rData.GetPrivateID(), dwRCRoomNum);
			
		}
		else
		{
			LOG.PrintTimeAndLog( 0, "%s HWID %s:%s", __FUNCTION__ , rData.GetPrivateID().c_str(), rData.GetBillingGUID().c_str() );
			SP2Packet kPacket( rData.GetReturnMsgType() );
			kPacket << rData.GetPrivateID();
			kPacket << rData.GetBillingGUID();
			kPacket << BILLING_LOGIN_RESULT_FAIL;
			kPacket << false;
			g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket );
			LOG.PrintTimeAndLog(0, "%s:%s:%s:pcroom:%d", "ioLocalIndonesia::ThreadLogin()-lv0 fail", rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), dwRCRoomNum);
			return;
		}

		if( rLoginMgr.CheckLogin( rData ) )
			return;
	}

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

	SYSTEMTIME st;
	GetLocalTime( &st );
	char szDate[MAX_PATH]="";
	StringCbPrintf( szDate, sizeof( szDate ), "%04d%02d%02d%02d%02d%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond );

	char szUniteValue[MAX_PATH*2]="";
//	StringCbPrintf( szUniteValue, sizeof( szUniteValue ), "%s%s%s%s%s", rData.GetUserIP().c_str(), rData.GetPrivateID().c_str(), szDate, SHOP_ID, szPW  );
	StringCbPrintf(szUniteValue, sizeof( szUniteValue ), "%s%s%s%s%s", 
		SHOP_ID, rData.GetPrivateID().c_str(), rData.GetUserIP().c_str(), szHexMD5PW, szDate);

	char szHexMD5Key[MAX_PATH]="";
	ZeroMemory( szHexMD5Key, sizeof( szHexMD5Key ) );
	GetHexMD5( szHexMD5Key, sizeof( szHexMD5Key ), szUniteValue );

	char szPostData[MAX_PATH*4]="";

	StringCbPrintf( szPostData, sizeof( szPostData ), "id=%s&password=%s&ip=%s&svcCd=%s&reqDate=%s&key=%s", 
		rData.GetPrivateID().c_str(), szHexMD5PW, rData.GetUserIP().c_str(), SHOP_ID, szDate, szHexMD5Key );

	if( g_App.IsTestMode() )
	{
		// web test
// 		ioHTTP Winhttp; //kyg 확인 필요 
// 		Winhttp.GetResultData( "http://210.118.58.227/LostSagaPatchID/test.xml", "", szReturnValue, WEB_BUFF_SIZE );
// 		static bool bOK = true;
// 		if( bOK )
// 		{
// 			LOG.PrintTimeAndLog( 0, szReturnValue );
// 			bOK = false;
// 		}
		
		StringCbPrintf( szReturnValue, WEB_BUFF_SIZE,  "S000	10000");
	}
	else
	{

		ioHTTP Winhttp; //kyg 확인 필요 
		if( !Winhttp.GetResultData( m_sLoginURL.c_str(), szPostData, szReturnValue, WEB_BUFF_SIZE ) )
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

	if( strncmp( szReturnValue, "S000", 4 )  != 0 )
	{
		//LOG.PrintTimeAndLog(0, "%s Return Error.:%s:%s:%s[%s:%s:%s]", __FUNCTION__, rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), szReturnValue, szPW, szUserKey, rData.GetEncodePW().c_str() );

		char szToken[MAX_PATH]="";
		char *pContext = NULL;
		szToken[0] = 9; // 구분자
		// E01HTUser password is not correctHT 에서 User password is not correct을 추출
		char *pPos = strtok_s( szReturnValue, szToken, &pContext );
		if( pPos != NULL )
			pPos = strtok_s( NULL, szToken, &pContext );

		SP2Packet kPacket( rData.GetReturnMsgType() );
		kPacket << rData.GetPrivateID();
		kPacket << rData.GetBillingGUID();
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
		g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket );
		g_LogDBClient.OnInsertBillingServerError( CNT_WEMADEBUY, BILLING_ERROR_LOG_EXCEPTION,"Login Return Error." );
		return;
	}

	//LOG.PrintTimeAndLog(0, "%s Complete.:%s:%s:%s", __FUNCTION__, rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), szReturnValue );

	char szToken[MAX_PATH]="";
	char *pContext = NULL;
	szToken[0] = 9; // 구분자
	// S000HT2132HTtestHTtestHTtest@test.comHTNHTNHT0HTID 에서 2132빌링에서 사용하는 유저넘버
	char *pPos = strtok_s( szReturnValue, szToken, &pContext );
	if( pPos != NULL )
	{
		pPos = strtok_s( NULL, szToken, &pContext );

		if( pPos != NULL)
			pPos = strtok_s( NULL, szToken, &pContext );
	}

//	DWORD dwRCRoomNum = 0;
// 	if( !ThreadPCRoom( rData, dwRCRoomNum ) )
// 		return;
	if( !rLoginMgr.InsertLogin( rData, pPos, dwRCRoomNum, false, LoginInfo::PT_ENCODE_LOGIN_PW ) ) 
		return;

	SP2Packet kPacket( rData.GetReturnMsgType() );
	kPacket << rData.GetPrivateID();
	kPacket << rData.GetBillingGUID();
	kPacket << BILLING_LOGIN_RESULT_SUCCESS;
	if( pPos != NULL )
		kPacket << pPos;
	g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket );

	LOG.PrintTimeAndLog( 0, "ioLocalIndonesia::ThreadLogin() Send OK Type :0x%x:%s:%s:pcroom:%d", rData.GetReturnMsgType(), rData.GetPrivateID().c_str(), rData.GetBillingGUID().c_str(), dwRCRoomNum );
}

void ioLocalIndonesia::ThreadGetCash( const ioData &rData )
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

	char szUniteValue[MAX_PATH*2]="";
	StringCbPrintf( szUniteValue, sizeof( szUniteValue ), "%s%s%s", SHOP_KEY, rData.GetUserNo().c_str(), SHOP_ID );

	char szHexMD5Key[MAX_PATH]="";
	ZeroMemory( szHexMD5Key, sizeof( szHexMD5Key ) );
	GetHexMD5( szHexMD5Key, sizeof( szHexMD5Key ), szUniteValue );

	char szFullURL[MAX_PATH*4]="";
	StringCbPrintf( szFullURL, sizeof( szFullURL ), "%s?SID=%s&CN=%s&A-KEY=%s" , m_sBillingGetURL.c_str(), SHOP_ID, rData.GetUserNo().c_str(), szHexMD5Key );

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

		StringCbPrintf( szReturnValue, WEB_BUFF_SIZE,  "{\"RESULT-CODE\" : \"S000\", \"CASH-BALANCE\" : \"%d\"}", iCash );
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
	if( strcmp( szReturnValue , "" ) == 0 )
	{
		LOG.PrintTimeAndLog( 0, "%s Return Empty: %d:%s:%s:%s:%d[%s]", __FUNCTION__, rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetPublicID().c_str(), GetLastError(), szReturnValue );
		SP2Packet kPacket( BSTPK_GET_CASH_RESULT );
		kPacket << rData.GetUserIndex();
		kPacket << rData.GetBillingGUID();
		kPacket << rData.GetSetUserMouse();
		kPacket << CASH_RESULT_EXCEPT;
		kPacket << false;
		g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket );
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

	if( !Help::GetParseJSon( szReturnValue, vKeyVec, vValueVec ) )
	{
		LOG.PrintTimeAndLog( 0, "%s Json Error: %d:%s:%s:%s:%d[%s]", __FUNCTION__, rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetPublicID().c_str(), GetLastError(), szReturnValue );
		SP2Packet kPacket( BSTPK_GET_CASH_RESULT );
		kPacket << rData.GetUserIndex();
		kPacket << rData.GetBillingGUID();
		kPacket << rData.GetSetUserMouse();
		kPacket << CASH_RESULT_EXCEPT;
		kPacket << false;
		g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket );
		g_LogDBClient.OnInsertBillingServerError( CNT_WEMADEBUY, BILLING_ERROR_LOG_EXCEPTION,"Billing exception Gatcash(JsonCrash)." );
		return;
	}

	if( strcmp( vValueVec[0].c_str(), "S000" ) != 0 )
	{
		LOG.PrintTimeAndLog( 0, "%s KreonError: %d:%s:%s:%s:%d[%s:%s]", __FUNCTION__, rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetPublicID().c_str(), GetLastError(), vValueVec[0].c_str(), szReturnValue );
		SP2Packet kPacket( BSTPK_GET_CASH_RESULT );
		kPacket << rData.GetUserIndex();
		kPacket << rData.GetBillingGUID();
		kPacket << rData.GetSetUserMouse();
		kPacket << CASH_RESULT_EXCEPT;
		kPacket << true;
		kPacket << vValueVec[0];
		g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket );
		g_LogDBClient.OnInsertBillingServerError( CNT_WEMADEBUY, BILLING_ERROR_LOG_EXCEPTION,"Billing exception Gatcash(KreonError)." );
		return;
	}

	int iTotalCash = atoi( vValueVec[1].c_str() );

	SP2Packet kPacket( BSTPK_GET_CASH_RESULT );
	kPacket << rData.GetUserIndex();
	kPacket << rData.GetBillingGUID();
	kPacket << rData.GetSetUserMouse();
	kPacket << CASH_RESULT_SUCCESS;
	kPacket << iTotalCash;
	kPacket << iTotalCash;  // total cash와 purchase cash가 동일함.
	g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket );

	//LOG.PrintTimeAndLog( 0, "%s Success: %d:%s:PrivateID %s:%s:%d[%s]", __FUNCTION__, rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetPublicID().c_str(), iTotalCash, szReturnValue  );
}

void ioLocalIndonesia::ThreadOutputCash( const ioData &rData )
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

	char szUniteValue[MAX_PATH*2]="";
	StringCbPrintf( szUniteValue, sizeof( szUniteValue ), "%s1%s%d%s", SHOP_KEY, rData.GetUserNo().c_str(), rData.GetItemPayAmt(), SHOP_ID  );

	char szHexMD5Key[MAX_PATH]="";
	ZeroMemory( szHexMD5Key, sizeof( szHexMD5Key ) );
	GetHexMD5( szHexMD5Key, sizeof( szHexMD5Key ), szUniteValue );

	char szPostData[MAX_PATH*4]="";
	StringCbPrintf( szPostData, sizeof( szPostData ), "SHOP-ID=%s&BUYER-CN=%s&BUYER-LEVEL=%d&PG-CODE=P001&TOTAL-COUNT=1&TOTAL-PRICE=%d&P1-CODE=%d&P1-TYPE=C033&P1-NAME=%s&P1-COUNT=1&P1-PRICESUM=%d&A-KEY=%s", SHOP_ID, rData.GetUserNo().c_str(), rData.GetUserLevel(), rData.GetItemPayAmt(), rData.GetGoodsNo(), rData.GetGoodsName().c_str(), rData.GetItemPayAmt(), szHexMD5Key );

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

		StringCbPrintf( szReturnValue, WEB_BUFF_SIZE,   "{\"RESULT-CODE\" : \"S000\", \"CASH-BALANCE\" : \"%d\"}", iCash );
	}
	else
	{
		ioHTTP Winhttp; //kyg 확인 필요 
		if( !Winhttp.GetResultData( m_sBillingOutPutURL.c_str(), szPostData, szReturnValue, WEB_BUFF_SIZE ) )
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

	if( strcmp( szReturnValue , "" ) == 0 )
	{
		LOG.PrintTimeAndLog( 0, "%s Return empty: %d:%s:%s:%s:%s:%d", __FUNCTION__, rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetPublicID().c_str(), rData.GetUserNo().c_str(), GetLastError() );
		SP2Packet kPacket( BSTPK_OUTPUT_CASH_RESULT );
		kPacket << rData.GetUserIndex();
		kPacket << rData.GetBillingGUID();
		kPacket << CASH_RESULT_EXCEPT;
		kPacket << rData.GetItemType();
		kPacket << false;
		g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket );
		g_LogDBClient.OnInsertBillingServerError( CNT_WEMADEBUY, BILLING_ERROR_LOG_EXCEPTION,"Billing exception Return empty." );
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

	if( !Help::GetParseJSon( szReturnValue, vKeyVec, vValueVec ) )
	{
		LOG.PrintTimeAndLog( 0, "%s Json Error: %d:%s:%s:%s:%s:%d[%s]", __FUNCTION__, rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetPublicID().c_str(), rData.GetUserNo().c_str(), GetLastError(), szReturnValue );
		SP2Packet kPacket( BSTPK_OUTPUT_CASH_RESULT );
		kPacket << rData.GetUserIndex();
		kPacket << rData.GetBillingGUID();
		kPacket << CASH_RESULT_EXCEPT;
		kPacket << rData.GetItemType();
		kPacket << false;
		g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket );
		g_LogDBClient.OnInsertBillingServerError( CNT_WEMADEBUY, BILLING_ERROR_LOG_EXCEPTION,"Billing exception Json Error." );
		return;
	}

	if( strcmp( vValueVec[0].c_str(), "S000" ) != 0 )
	{
		LOG.PrintTimeAndLog( 0, "%s KreonError: %d:%s:%s:%s:%s:%d[%s:%s]<%s>", __FUNCTION__, rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetPublicID().c_str(), rData.GetUserNo().c_str(), GetLastError(), vValueVec[0].c_str(), szReturnValue, szPostData );
		SP2Packet kPacket( BSTPK_OUTPUT_CASH_RESULT );
		kPacket << rData.GetUserIndex();
		kPacket << rData.GetBillingGUID();
		kPacket << CASH_RESULT_EXCEPT;
		kPacket << rData.GetItemType();
		kPacket << true;
		kPacket << vValueVec[0];
		g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket );
		g_LogDBClient.OnInsertBillingServerError( CNT_WEMADEBUY, BILLING_ERROR_LOG_EXCEPTION,"Billing exception OutputCash(KreonError)." );
		return;
	}

	int iTotalCash = atoi( vValueVec[1].c_str() );

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
	kPacket << rData.GetChannelingType();  // 공통
	kPacket << iTotalCash;
	kPacket << iTotalCash; // 캐쉬와 실재 구매한 캐쉬가 동일하다.
	g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket );

	LOG.PrintTimeAndLog(0, "%s Success. :%d:%s:PrivateID %s:%s:%s[%d:%s:%d:%d][%s]", __FUNCTION__, rData.GetUserIndex(), rData.GetBillingGUID().c_str(),rData.GetPrivateID().c_str(), rData.GetPublicID().c_str(), rData.GetUserNo().c_str(), rData.GetGoodsNo(), rData.GetGoodsName().c_str(), rData.GetItemPayAmt(), iTotalCash, szReturnValue );
}

bool ioLocalIndonesia::ThreadPCRoom( IN const ioData &rData, IN const ioHashString &szReturn, OUT DWORD &rdwPCRoom )
{
	if( rData.IsEmpty() )
	{
		LOG.PrintTimeAndLog( 0, "%s Data is Empty.", __FUNCTION__ );
		return false;
	}

	if( g_App.IsReserveLogOut() )
	{
		LOG.PrintTimeAndLog(0, "%s LogOut:%s:%s:%s", __FUNCTION__, rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetEncodePW().c_str() );
		SP2Packet kPacket( rData.GetReturnMsgType() );
		kPacket << rData.GetPrivateID();
		kPacket << rData.GetBillingGUID();
		kPacket << BILLING_LOGIN_RESULT_FAIL;
		kPacket << false;
		g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  
											rData.GetServerPort(), kPacket );
		return false;
	}

	char szReturnValue[WEB_BUFF_SIZE]="";
	ZeroMemory( szReturnValue, WEB_BUFF_SIZE );

	char szPostData[WEB_BUFF_SIZE]="";
	StringCbPrintf( szPostData, sizeof( szPostData ), "mac=%s&ip=%s&svcCd=%s&key=%s&cn=%s&hwId=%s&dwSessionId=%d", 
		rData.GetUserMacAddress().c_str(), rData.GetUserIP().c_str(), SHOP_ID, rData.GetUserKey().c_str(), szReturn.c_str(), 
		rData.GetAuthInfo().c_str(), rData.GetSessionID() );

	if( g_App.IsTestMode() )
	{
		// web test
// 		ioHTTP Winhttp; //kyg 확인 필요 
// 		Winhttp.GetResultData( "http://210.118.58.227/LostSagaPatchID/test.xml", "", szReturnValue, WEB_BUFF_SIZE );
// 		static bool bOK = true;
// 		if( bOK )
// 		{
// 			LOG.PrintTimeAndLog( 0, szReturnValue );
// 			bOK = false;
// 		}
		//
		
		bool bPcroom = true;

		if( bPcroom )
			StringCbPrintf( szReturnValue, WEB_BUFF_SIZE,  "Silver	Silver");
		else
			StringCbPrintf( szReturnValue, WEB_BUFF_SIZE,  "Normal");

		bPcroom = !bPcroom;

		/*
		int iRand = rand()%5;
		if( iRand == 0 )
			StringCbPrintf( szReturnValue, WEB_BUFF_SIZE,  "Normal");
		else if( iRand == 1 )
			StringCbPrintf( szReturnValue, WEB_BUFF_SIZE,  "Silver	Silver");
		else if( iRand == 2 )
			StringCbPrintf( szReturnValue, WEB_BUFF_SIZE,  "Gold	Gold");
		else if( iRand == 3 )
			StringCbPrintf( szReturnValue, WEB_BUFF_SIZE,  "Blocked	Warnet ini tidak dapat mengakses layanan gemscool karena ip address telah diblokir. Silahkan hubungi layanan warnet biz gemscool.");
		else if( iRand == 4 )
			StringCbPrintf( szReturnValue, WEB_BUFF_SIZE,  "E901	Kesalahan pada sistem yang tidak diketahui.");
		*/
	}
	else
	{

		ioHTTP Winhttp; //kyg 확인 필요 
		if( !Winhttp.GetResultData( m_sPCRoomURL.c_str(), szPostData, szReturnValue, WEB_BUFF_SIZE ) )
		{
			LOG.PrintTimeAndLog( 0, "%s Http Error: %s:%s:%d", __FUNCTION__, rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), GetLastError() );
			SP2Packet kPacket( rData.GetReturnMsgType()  );
			kPacket << rData.GetPrivateID();
			kPacket << rData.GetBillingGUID();
			kPacket << BILLING_LOGIN_RESULT_FAIL;
			kPacket << false;
			g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket );
			g_LogDBClient.OnInsertBillingServerError( CNT_WEMADEBUY, BILLING_ERROR_LOG_EXCEPTION,"PCROOM exception HTTP." );
			return false;
		}
	}

	if( strncmp( szReturnValue, "Normal", 6 )  != 0 && 
		strncmp( szReturnValue, "Silver", 6 )  != 0 &&
		strncmp( szReturnValue, "Gold", 4 )  != 0 )
	{
		LOG.PrintTimeAndLog(0, "%s Return Error.:%s:%s:%s[%s]", __FUNCTION__, rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), szReturnValue, rData.GetEncodePW().c_str() );

		char *pContext = NULL;
		char szToken[MAX_PATH]="";
		szToken[0] = 9; // 구분자
		// E01HTUser password is not correctHT 에서 User password is not correct을 추출
		char *pPos = strtok_s( szReturnValue, szToken, &pContext );
		char *pPos2 = NULL;
		if( pPos != NULL )
			pPos2 = strtok_s( NULL, szToken, &pContext );

		char szError[MAX_PATH*2]="";
		StringCbPrintf( szError, sizeof( szError ), "%s(%s)", pPos2, pPos );

		SP2Packet kPacket( rData.GetReturnMsgType() );
		kPacket << rData.GetPrivateID();
		kPacket << rData.GetBillingGUID();
		kPacket << BILLING_LOGIN_RESULT_FAIL;
		if( pPos != NULL )
		{
			kPacket << true;
			kPacket << szError;
		}
		else
		{
			kPacket << false;
		}
		g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket );
		g_LogDBClient.OnInsertBillingServerError( CNT_WEMADEBUY, BILLING_ERROR_LOG_EXCEPTION,"PCRoom Return Error." );
		return false;
	}

	

	// return value;
	if( strncmp( szReturnValue, "Normal", 6 ) == 0 )
	{
		rdwPCRoom = 0;
	}
	else if( strncmp( szReturnValue, "Silver", 6 ) == 0 )
	{
		rdwPCRoom = 1;
	}
	else if( strncmp( szReturnValue, "Gold", 4 ) == 0 )
	{
		rdwPCRoom = 2;
	}
	LOG.PrintTimeAndLog(0, "%s Complete.:%s:%s:%s:%s:%s:pcroom:%d", __FUNCTION__, rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), szReturnValue, szPostData, m_sPCRoomURL.c_str(), rdwPCRoom);
	
	return true;
}