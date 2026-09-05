
#include "../stdafx.h"
#include "./ioLocalLatin.h"
#include "../MainProcess.h"
#include "../NodeInfo/ServerNode.h"
#include "../DataBase/LogDBClient.h"
#include "../Channeling/ioChannelingNodeParent.h"
#include "../ThreadPool/ioThreadPool.h"
#include "../NodeInfo/ServerNodeManager.h"
#include "../EtcHelpFunc.h"

extern CLog LOG;

ioLocalLatin::ioLocalLatin(void)
{
}


ioLocalLatin::~ioLocalLatin(void)
{
}

ioLocalManager::LocalType ioLocalLatin::GetType()
{
	return ioLocalManager::LCT_LATIN;
}

void ioLocalLatin::Init()
{
	
	//ip, port, servicevalue -> 아직 모름 
	ioINILoader kLoader( "ls_config_billingsvr.ini" );
	kLoader.SetTitle( "Latin" );

	char szSoftnyxLoginURL[MAX_PATH]="";
	kLoader.LoadString( "SoftnyxLoginURL", "", szSoftnyxLoginURL, sizeof( szSoftnyxLoginURL ) );
	m_sLoginURL = szSoftnyxLoginURL;

	char szSoftnyxBillingGetURL[MAX_PATH]="";
	kLoader.LoadString( "SoftnyxBillingURL", "", szSoftnyxBillingGetURL, sizeof( szSoftnyxBillingGetURL ) );
	m_sBillingGetURL = szSoftnyxBillingGetURL;

	char szSoftnyxBillingOutPutURL[MAX_PATH]="";
	kLoader.LoadString( "SoftnyxBillingURL", "", szSoftnyxBillingOutPutURL, sizeof( szSoftnyxBillingOutPutURL ) );
	m_sBillingOutPutURL = szSoftnyxBillingOutPutURL;

	char szSoftnyxBillingLogURL[MAX_PATH]="";
	kLoader.LoadString( "SoftnyxLogURL", "", szSoftnyxBillingLogURL, sizeof( szSoftnyxBillingLogURL ) );
	m_sBillingLogURL = szSoftnyxBillingLogURL;

	char szSoftnyxBillingCcuURL[MAX_PATH]="";
	kLoader.LoadString( "SoftnyxCCuURL", "", szSoftnyxBillingCcuURL, sizeof( szSoftnyxBillingCcuURL ) );
	m_sBillingCcuURL = szSoftnyxBillingCcuURL;

	m_serviceValue =  kLoader.LoadInt( "SoftnyxServiceValue", 0 );
	m_local =  kLoader.LoadInt( "SoftnyxLocal", 0 );
	
	LOG.PrintTimeAndLog(0,"LoginURL:%s, BillingGetURL:%s,BillingOutputURL:%s,LogURL:%s,ccuURL:%s,ServiceValue:%d,ServiceLocal:%d", 
		m_sLoginURL.c_str(), m_sBillingGetURL.c_str(), m_sBillingOutPutURL.c_str(), m_sBillingLogURL.c_str(), m_sBillingCcuURL.c_str(), m_serviceValue, m_local);

	if( m_sLoginURL.IsEmpty() || m_sBillingGetURL.IsEmpty() || m_sBillingOutPutURL.IsEmpty() )
		LOG.PrintTimeAndLog(0,"Error Softnyx URL");
}
void ioLocalLatin::OnLoginData( ServerNode *pServerNode, SP2Packet &rkPacket )
{
	ioHashString szBillingGUID;
	ioHashString szPrivateID;
	ioHashString sEncodePW;
	ioHashString szPublicIP;
	DWORD        dwReturnMsgType = 0;
	ioHashString szMacAddress;
	ioHashString szKey;
	DWORD iChannelingType = 0;
	DWORD		 dwIndex = 0;	//accountIDX
	int		     dwVCode = 0;
	int			 intID   = 0;
	int			 reqNum = 0;
	reqNum = TIMEGETTIME();
	
  
	
	rkPacket >> szBillingGUID >> szPrivateID >> sEncodePW >> szPublicIP >> dwReturnMsgType ; // 공통

	
	  if( !pServerNode )
	{
		LOG.PrintTimeAndLog(0, "%s pServerNode == NULL:%s:%s:%s", __FUNCTION__, szBillingGUID.c_str(), szPrivateID.c_str(),  szPublicIP.c_str() );
		return;
	}
	
	//intID = 1200;
    //dwVCode = 1000;
	
	
	ioData kData;
	kData.SetBillingGUID( szBillingGUID );
	kData.SetPrivateID( szPrivateID );
	kData.SetUserIndex( dwIndex );
	kData.SetReturnMsgType( dwReturnMsgType );
	kData.SetUserIP( szPublicIP );
	
	kData.SetEncodePW( sEncodePW );
	//kData.SetUserVCode( atoi(sEncodePW.c_str()) );
	kData.SetUserReqNum( reqNum );	//시간값 넣어주는 걸루

	if( pServerNode )
	{
		kData.SetServerIP( pServerNode->GetIP() );
		kData.SetServerPort( pServerNode->GetClientPort() );
	}
	kData.SetEmpty( false );
	kData.SetProcessType( ioData::PT_LOGIN );

	g_ThreadPool.SetData( kData );
	
	//LOG.PrintTimeAndLog( 0, "%s %s %s %s %d %d %s", __FUNCTION__, szPrivateID.c_str(), szPublicIP.c_str(), szBillingGUID.c_str(), intID, dwVCode, sEncodePW.c_str() );
}

//사용안함
void ioLocalLatin::OnLogoutData( ServerNode *pServerNode, SP2Packet &rkPacket )
{
	/*
	게임서버에서 패킷을어떻게 보내주는지에 따라서 달라짐
	*/
	DWORD		 dwIndex = 0;	//accountIDX
	ioHashString szConnectTime;
	ioHashString szDisconnectTime;
	DWORD		 dwServerNo = 0; 
	DWORD		 dwCountry = 0;
	DWORD		 dwCountryByIP = 0;
	ioHashString szPublicIP;
	ioHashString szPrivateIP;
	DWORD		 dwWin = 0;
	DWORD		 dwDraw = 0;
	DWORD		 dwLose = 0;
	DWORD		 dwGiveup = 0;
	DWORD		 dwGold = 0;
	DWORD        dwExp = 0;
	DWORD		 dwGender = 0;
	DWORD		 dwExitCode = 0;
	
	rkPacket >> dwIndex >> szConnectTime >> szDisconnectTime >> dwServerNo >> dwCountry >> dwCountryByIP >> szPublicIP >> szPrivateIP;
	rkPacket >> dwWin >> dwDraw >> dwLose >> dwGiveup >> dwGold >> dwExp >> dwGender >> dwExitCode; // 공통
	
	ioData kData;
	kData.SetUserIndex( dwIndex );
	kData.SetUserIP( szPublicIP );
	kData.SetConnectTime( szConnectTime );
	kData.SetDisConnectTime( szDisconnectTime );
	kData.SetServerNo( dwServerNo );
	//kData.SetCountry( dwCountry );
	kData.SetCountryByIP( dwCountryByIP );
	kData.SetUserIP( szPublicIP );
	kData.SetPrivateIP( szPrivateIP );
	kData.SetWin( dwWin );
	kData.SetDraw( dwDraw );
	kData.SetLose( dwLose );
	kData.SetGiveup( dwGiveup );
	kData.SetGold( dwGold );
	kData.SetExp( dwExp );
	kData.SetGender( dwGender );
	kData.SetExitCode( dwExitCode );

	if( pServerNode )
	{
		kData.SetServerIP( pServerNode->GetIP() );
		kData.SetServerPort( pServerNode->GetClientPort() );
	}
	kData.SetEmpty( false );
	kData.SetProcessType( ioData::PT_LOGIN );

	g_ThreadPool.SetData( kData );
}

void ioLocalLatin::_OnGetCash( ServerNode *pServerNode, SP2Packet &rkPacket )
{
	int          iChannelingType   = 0;
	ioHashString szBillingGUID;
	DWORD        dwUserIndex       = 0;
	ioHashString szPublicID;
	ioHashString szPrivateID;
	bool         bSetUserMouse     = false;
	ioHashString szUserIP;	
	int			 intID   = 0;
	ioHashString sVCode;

	rkPacket >> iChannelingType >> szBillingGUID >> dwUserIndex >> szPrivateID >> szPublicID >> bSetUserMouse; // 공통사항
	rkPacket >> sVCode; 
	//rkPacket >> iChannelingType >> szBillingGUID >> dwUserIndex >> szPrivateID >> szPublicID >> intID >> szUserIP >> bSetUserMouse; 
	
	if( !pServerNode )
	{
		LOG.PrintTimeAndLog(0, "%s pServerNode == NULL:%s:%d:%s", __FUNCTION__, szBillingGUID.c_str(), dwUserIndex, szPublicID.c_str() );
		return;
	}
	
	ioData kData;
	kData.SetBillingGUID( szBillingGUID );
	kData.SetPrivateID( szPrivateID );		//이걸 intid 로 쓰면됨
	kData.SetPublicID( szPublicID );
	kData.SetUserIndex( dwUserIndex );
	kData.SetUserIP( szUserIP );
	kData.SetSetUserMouse( bSetUserMouse );
	
	if( pServerNode )
	{
		kData.SetServerIP( pServerNode->GetIP() );
		kData.SetServerPort( pServerNode->GetClientPort() );
	}
	kData.SetEmpty( false );
	kData.SetProcessType( ioData::PT_GET_CASH );

	g_ThreadPool.SetData( kData );
	LOG.PrintTimeAndLog(0, "%s UserIndex : %d BillingGUID : %s publicID :%s privateID :%s", "ioLocalLatin::_OnGetCash", dwUserIndex, szBillingGUID.c_str(), szPublicID.c_str(), szPrivateID.c_str() );

}

void ioLocalLatin::_OnOutputCash( ServerNode *pServerNode, SP2Packet &rkPacket, DWORD dwGoodsNo, const ioHashString &rszGoodsName )
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
	int			 intID   = 0;
	ioHashString szCountry;
//#if(_TEST)
//	//테스트 일경우 임의로 아이템 패킷을 보내므로 아래와 같이 받는다. 
//	int temp1, temp2, temp3, temp4, temp5, temp6 ;
//	rkPacket >> iChannelingType >> szBillingGUID >> dwUserIndex >> szPublicID >> szPrivateID >> szUserIP >> iPayAmt >> iType >> temp1 >> temp2 >> temp3 >> temp4 >> temp5 >> temp6 >> intID ; 
//#else if
	rkPacket >> iChannelingType >> szBillingGUID >> dwUserIndex >> szPublicID >> szPrivateID >> szUserIP >> szCountry >> iPayAmt >> iType; 
//#endif
	int iItemValueList[MAX_ITEM_VALUE];
	for (int i = 0; i < MAX_ITEM_VALUE ; i++)
		iItemValueList[i] = 0;
	ioChannelingNodeParent::GetItemValueList( rkPacket, iType, iItemValueList );

	ioData kData;
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
	kData.SetCountry( szCountry );
	
	if( pServerNode )
	{
		kData.SetServerIP( pServerNode->GetIP() );
		kData.SetServerPort( pServerNode->GetClientPort() );
	}
	kData.SetEmpty( false );
	kData.SetProcessType( ioData::PT_OUTPUT_CASH );

	g_ThreadPool.SetData( kData );
	LOG.PrintTimeAndLog(0, "ioLocalLatin::_OnOutputCash() %d, %s, %s, %s, Country : %s, payamt: %d, type:%d %d, %s", dwUserIndex, szBillingGUID.c_str(), szPublicID.c_str(), szPrivateID.c_str(),
		szCountry.c_str(), iPayAmt, iType, dwGoodsNo, rszGoodsName.c_str() );

}
//VCODE 인증 받고 난 후 초기화 해주는 부분 필요함
void ioLocalLatin::ThreadLogin( const ioData &rData, LoginManager &rLoginMgr )
{
	if( rData.IsEmpty() )
	{
		LOG.PrintTimeAndLog( 0, "%s Data is Empty.", __FUNCTION__ );
		return;
	}
	if( g_App.IsReserveLogOut() )
	{
		LOG.PrintTimeAndLog(0, "%s LogOut:%s:%s:%s", __FUNCTION__, rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str() );
		SP2Packet kPacket( rData.GetReturnMsgType() );
		kPacket << rData.GetPrivateID();
		kPacket << rData.GetBillingGUID();
		kPacket << BILLING_LOGIN_RESULT_FAIL;
		kPacket << false;
		if(!g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket ))
		{
			LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:%s Ret %d:%s:%d", "ioLocalLatin::ThreadLogin", rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetGoodsNo(), rData.GetGoodsName().c_str(), rData.GetItemPayAmt());
		}
		return;
	}
	
	if( g_App.IsTestMode() )
	{
		SP2Packet kPacket( rData.GetReturnMsgType() );
		kPacket << rData.GetPrivateID();
		kPacket << rData.GetBillingGUID();
		kPacket << BILLING_LOGIN_RESULT_SUCCESS;
		kPacket << rData.GetPrivateID();
		g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket );
		LOG.PrintTimeAndLog( 0, "%s Login Success: reqNum:%d %s:%s:%d:%d:%s", "ioLocalLatin::ThreadLogin", rData.GetUserReqNum(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetUserIndex(), rData.GetUserIntID() , rData.GetEncodePW().c_str());
	}
	else
	{
		//정상인증시(국가번호 105, 권한레벨 10, 남성, 카페등급30, 나이20세)
		//1234567890;01378;aabbddc;1;105;;10;;;;;0;30;20
		//reqNum;intid;userid;returnValue(1:정상);국가번호;사용안함;일반유저;카페등급;나이
		ioHTTP Winhttp; 
		
		char szReturnValue[WEB_BUFF_SIZE]="";
		
		char szFullURL[MAX_PATH*2]="";
		StringCbPrintf( szFullURL, sizeof( szFullURL ), "%s?REQNUM=%d&GAMETYPE=%c&LOCAL=%d&INTID=%s&IP=%s&VCODE=%s" , m_sLoginURL.c_str(), rData.GetUserReqNum(), SA_GAMETYPE, m_local, rData.GetPrivateID().c_str(), rData.GetUserIP().c_str(), rData.GetEncodePW().c_str() );
		
		LOG.PrintTimeAndLog( 0, "ioLocalLatin::ThreadLogin() URL : %s", szFullURL );

		if( !Winhttp.GetResultData( szFullURL, "GET", szReturnValue, WEB_BUFF_SIZE ) )
		{
			LOG.PrintTimeAndLog( 0, "%s Http Error: %s:%s:%d", "ioLocalLatin::ThreadLogin", rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), GetLastError() );
			SP2Packet kPacket( rData.GetReturnMsgType() );
			kPacket << rData.GetPrivateID();
			kPacket << rData.GetBillingGUID();
			kPacket << BILLING_LOGIN_RESULT_FAIL;
			kPacket << false;
			if(!g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket ))
			{
				LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:%s Ret %d:%s:%d", "ioLocalLatin::ThreadLogin", rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetGoodsNo(), rData.GetGoodsName().c_str(), rData.GetItemPayAmt());
			}
			return;
		}

		ioHashStringVec vParseStr;
		Help::GetParseData( szReturnValue, vParseStr, SA_TOKEN );
		if( vParseStr.size() > MAX_LOGIN_ARRAY )
		{
			LOG.PrintTimeAndLog( 0, "%s Size Error. %s", "ioLocalLatin::ThreadLogin", szReturnValue );
			return;
		}
		
		if(strcmp( vParseStr[3].c_str(), "1") != 0)
		{
			SP2Packet kPacket( rData.GetReturnMsgType() );
			kPacket << rData.GetPrivateID();
			kPacket << rData.GetBillingGUID();
			kPacket << BILLING_LOGIN_RESULT_FAIL;
			kPacket << rData.GetPrivateID();
			//kPacket << false;
		
			if(!g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket ))
			{
				LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:%s Ret %d:%s:%d", "ioLocalLatin::ThreadLogin", rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetGoodsNo(), rData.GetGoodsName().c_str(), rData.GetItemPayAmt());
			}
			LOG.PrintTimeAndLog( 0, "%sResult%s", "ioLocalLatin::ThreadLogin", szReturnValue );
			LOG.PrintTimeAndLog( 0, "%s Login Error: reqNum:%d Error:%d %s:%s:%d:%d:%s", "ioLocalLatin::ThreadLogin", rData.GetUserReqNum(), vParseStr[3].c_str(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetUserIndex(), rData.GetUserIntID(), rData.GetEncodePW().c_str() );
			return;
		}
		SP2Packet kPacket( rData.GetReturnMsgType() );
		kPacket << rData.GetPrivateID();
		kPacket << rData.GetBillingGUID();
		kPacket << BILLING_LOGIN_RESULT_SUCCESS;
		kPacket << rData.GetPrivateID();
		//Latin 만 추가로 더 보냄
		kPacket << vParseStr[4];	//국가 번호
		kPacket << vParseStr[5];	//권한 레벨
		kPacket << vParseStr[11];	//성별
		kPacket << vParseStr[12];	//카페등급
		kPacket << vParseStr[13];	//나이

		g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket );

		LOG.PrintTimeAndLog( 0, "%s Login Success: Result%s", "ioLocalLatin::ThreadLogin", szReturnValue );
		LOG.PrintTimeAndLog( 0, "%s Login Success: reqNum:%d %s:%s:%d:%d:%s", "ioLocalLatin::ThreadLogin", rData.GetUserReqNum(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetUserIndex(), rData.GetUserIntID(), rData.GetEncodePW().c_str() );
	}
}

void ioLocalLatin::ThreadLogout( const ioData &rData, LoginManager &rLoginMgr )
{
	if( rData.IsEmpty() )
	{
		LOG.PrintTimeAndLog( 0, "%s Data is Empty.", __FUNCTION__ );
		return;
	}
	if( g_App.IsReserveLogOut() )
	{
		LOG.PrintTimeAndLog(0, "%s LogOut:%s:%s:%s", __FUNCTION__, rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str() );
		SP2Packet kPacket( BSTPK_LOGIN_RESULT );
		kPacket << rData.GetPrivateID();
		kPacket << rData.GetBillingGUID();
		kPacket << BILLING_LOGIN_RESULT_FAIL;
		kPacket << false;
		if(!g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket ))
		{
			LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:%s Ret %d:%s:%d", "ioLocalLatin::ThreadLogin", rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetGoodsNo(), rData.GetGoodsName().c_str(), rData.GetItemPayAmt());
		}
		return;
	}

	if( g_App.IsTestMode() )
	{
		
	}
	else
	{
		
	}
}

void ioLocalLatin::ThreadGetCash( const ioData &rData )
{
	if( rData.IsEmpty() )
	{
		LOG.PrintTimeAndLog( 0, "%s Data is Empty.", __FUNCTION__ );
		return;
	}
	if( g_App.IsReserveLogOut() )
	{
		LOG.PrintTimeAndLog(0, "%s LogOut:%s:%s:%s", __FUNCTION__, rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str() );
		SP2Packet kPacket( BSTPK_GET_CASH_RESULT );
		kPacket << rData.GetUserIndex();
		kPacket << rData.GetBillingGUID();
		kPacket << rData.GetSetUserMouse();
		kPacket << CASH_RESULT_EXCEPT;
		if(!g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket ))
		{
			LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:%s Ret %d:%s:%d", "ioLocalLatin::ThreadGetCash", rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetGoodsNo(), rData.GetGoodsName().c_str(), rData.GetItemPayAmt());
		}
		return;
	}
	char szReturnValue[WEB_BUFF_SIZE]="";
	ioHTTP Winhttp; 
	/*
	//CashConsumptionAPI.aspx?USERID=&INTID=유저고유숫자형아이디&PROCESSTYPE=2&RETURNTYPE=1&SERVICEVALUE=Service Value*&GAMETYPE=Game Type*
	USERID			Varchar(20)	User.id 값이고  INTID 파라미터 값이 있으면 무시
	INTID			INT(11) user.IdSerial (숫자형 유저 아이디)
	PROCESSTYPE		Char(1)	2 (Fixed)
	RETURNTYPE		Char(1)	1 (Fixed)
	SERVICEVALUE	Char(1)	1 Billing test IP 에 테스트 할 경우
	GAMETYPE		Char(3)	O Lostsaga
	*/
	char szFullURL[MAX_PATH*2]="";
	int reqNum = 1;
		
	StringCbPrintf( szFullURL, sizeof( szFullURL ), "%s?USERID=&INTID=%s&PROCESSTYPE=2&RETURNTYPE=1&SERVICEVALUE=%d&GAMETYPE=%c" , m_sBillingGetURL.c_str(), rData.GetPrivateID().c_str(), m_serviceValue, SA_GAMETYPE );
	
	LOG.PrintTimeAndLog( 0, "ioLocalLatin::ThreadGetCashurl %s", szFullURL );

	if( !Winhttp.GetResultData( szFullURL, "GET", szReturnValue, WEB_BUFF_SIZE ) )
	{
		LOG.PrintTimeAndLog( 0, "%s Http Error: %s:%s:%d", "ioLocalLatin::ThreadGetCash", rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), GetLastError() );
			
		SP2Packet kPacket( BSTPK_GET_CASH_RESULT );
		kPacket << rData.GetUserIndex();
		kPacket << rData.GetBillingGUID();
		kPacket << rData.GetSetUserMouse();
		kPacket << CASH_RESULT_EXCEPT;
			
		if(!g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket ))
		{
			LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:%s Ret %d:%s:%d", "ioLocalLatin::ThreadGetCash", rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetGoodsNo(), rData.GetGoodsName().c_str(), rData.GetItemPayAmt());
		}
		return;
	}
		
	ioHashStringVec vParseStr;
	Help::GetParseData( szReturnValue, vParseStr, SA_TOKEN );
	if( vParseStr.size() < MAX_GETCASH_ARRAY )
	{

		LOG.PrintTimeAndLog( 0, "%s Size Error. %s", "ioLocalLatin::ThreadGetCash", szReturnValue );
		return;
	}
	/*
	1번째 인자	
	-1	Error
	0	Cash is 0
	1	Normal
	10	Not found ID
	30	Not enough Cash (Affected 체크 후 정상적이지 않으면 Cash 부족)
	52	Not found request parameters value

	2번째 인자	Cash
	*/
	if( strcmp( vParseStr[0].c_str(), "-1" ) == 0 )
	{
		SP2Packet kPacket( BSTPK_GET_CASH_RESULT );
		kPacket << rData.GetUserIndex();
		kPacket << rData.GetBillingGUID();
		kPacket << rData.GetSetUserMouse();
		kPacket << CASH_RESULT_EXCEPT;
			
		if(!g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket ))
		{
			LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:%s Ret %d:%s:%d", "ioLocalLatin::ThreadGetCash", rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetGoodsNo(), rData.GetGoodsName().c_str(), rData.GetItemPayAmt());
		}

		LOG.PrintTimeAndLog( 0, "%s GetCash Error: %s:%s:%d:%d:%s", "ioLocalLatin::ThreadGetCash", rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetUserIndex(), rData.GetUserIntID(), vParseStr[0].c_str() );
		return;
	}
	else
	{
		printf( "%s GetCash Success: %s:%s:%d:%d:%s,result:%s", "ioLocalLatin::ThreadGetCash\n", rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetUserIndex(), rData.GetUserIntID(), vParseStr[1].c_str(), vParseStr[0].c_str() );
		SP2Packet kPacket( BSTPK_GET_CASH_RESULT );
		kPacket << rData.GetUserIndex();
		kPacket << rData.GetBillingGUID();
		kPacket << rData.GetSetUserMouse();
		kPacket << CASH_RESULT_SUCCESS;
		kPacket << atoi(vParseStr[1].c_str() );
		kPacket << atoi(vParseStr[1].c_str() );
		if(!g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket ))
		{
			LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:%s", "ioLocalLatin::ThreadGetCash", rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str() );
			return;		
		}
	}
	LOG.PrintTimeAndLog( 0, "%s GetCash Success: %s:%s:%d:%d:%s", "ioLocalLatin::ThreadGetCash\n", rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetUserIndex(), rData.GetUserIntID(), vParseStr[1].c_str() );
}

void ioLocalLatin::ThreadOutputCash( const ioData &rData )
{
	ioHashString szChargeNo = "";
	if( rData.IsEmpty() )
	{
		LOG.PrintTimeAndLog( 0, "%s Data is Empty.", __FUNCTION__ );
		return;
	}
	if( g_App.IsReserveLogOut() )
	{
		LOG.PrintTimeAndLog(0, "%s LogOut:%s:%s:%s", __FUNCTION__, rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str() );
		
		SP2Packet kPacket( BSTPK_OUTPUT_CASH_RESULT  );
		kPacket << rData.GetUserIndex();
		kPacket << rData.GetBillingGUID();
		kPacket << CASH_RESULT_EXCEPT;
		kPacket << rData.GetItemType();
		if(!g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket ))
		{
			LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:%s Ret %d:%s:%d", "ioLocalLatin::ThreadOutputCash", rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetGoodsNo(), rData.GetGoodsName().c_str(), rData.GetItemPayAmt());
		}
		return;
	}
	if( g_App.IsTestMode() )
	{
		char szReturnValue[WEB_BUFF_SIZE]="";
		
		ioHTTP Winhttp; 
		
		/*
		//CashConsumptionAPI.aspx?USERID=&INTID=유저고유숫자형아이디&PROCESSTYPE=3&ITEMCODE=111&ITEMPRICE=100&RETURNTYPE=1&SERVICEVALUE= Service Value*&GAMETYPE= Game Type*&CC=222

		USERID			Varchar(20)	User.id 값이고  INTID 파라미터 값이 있으면 무시
		INTID			INT(11) user.IdSerial (숫자형 유저 아이디)
		PROCESSTYPE		Char(1)	2 (Fixed)
		RETURNTYPE		Char(1)	1 (Fixed)
		SERVICEVALUE	Char(1)	1 Billing test IP 에 테스트 할 경우
		GAMETYPE		Char(3)	O Lostsaga
		*/
		char szFullURL[MAX_PATH*2] = "http://112.175.228.40:8000/CashConsumptionAPI.aspx?USERID=testUser&INTID=1200&PROCESSTYPE=3&ITEMCODE=100030055&ITEMPRICE=9800&RETURNTYPE=1&SERVICEVALUE=1&GAMETYPE=O&CC=";
		LOG.PrintTimeAndLog( 0, "ioLocalLatin::ThreadOutputCash() Output url %s", szFullURL );
		if( !Winhttp.GetResultData( szFullURL, "GET", szReturnValue, WEB_BUFF_SIZE ) )
		{
			LOG.PrintTimeAndLog( 0, "%s Http Error: %s:%s:%d", "ioLocalLatin::ThreadOutputCash", rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), GetLastError() );
			SP2Packet kPacket( BSTPK_OUTPUT_CASH_RESULT  );
			kPacket << rData.GetUserIndex();
			kPacket << rData.GetBillingGUID();
			kPacket << CASH_RESULT_EXCEPT;
			kPacket << rData.GetItemType();
			if(!g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket ))
			{
				LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:%s Ret %d:%s:%d", "ioLocalLatin::ThreadOutputCash", rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetGoodsNo(), rData.GetGoodsName().c_str(), rData.GetItemPayAmt());
			}
			return;
		}
		
		ioHashStringVec vParseStr;
		Help::GetParseData( szReturnValue, vParseStr, SA_TOKEN );
		if( vParseStr.size() < MAX_GETCASH_ARRAY )
		{
			LOG.PrintTimeAndLog( 0, "%s Size Error. %s", "ioLocalLatin::ThreadOutputCash", szReturnValue );
			return;
		}
	}
	else
	{
		ioHTTP Winhttp; 
		char szReturnValue[WEB_BUFF_SIZE]="";
		/*
		//CashConsumptionAPI.aspx?USERID=&INTID=유저고유숫자형아이디&PROCESSTYPE=3&ITEMCODE=111&ITEMPRICE=100&RETURNTYPE=1&SERVICEVALUE= Service Value*&GAMETYPE= Game Type*&CC=222

		USERID			Varchar(20)	User.id 값이고  INTID 파라미터 값이 있으면 무시
		INTID			INT(11) user.IdSerial (숫자형 유저 아이디)
		PROCESSTYPE		Char(1)	2 (Fixed)
		RETURNTYPE		Char(1)	1 (Fixed)
		SERVICEVALUE	Char(1)	1 Billing test IP 에 테스트 할 경우
		GAMETYPE		Char(3)	O Lostsaga
		*/
		char szFullURL[MAX_PATH*2]="";
		StringCbPrintf( szFullURL, sizeof( szFullURL ), "%s?USERID=&INTID=%s&PROCESSTYPE=3&ITEMCODE=%d&ITEMPRICE=%d&RETURNTYPE=1&SERVICEVALUE=%d&GAMETYPE=%c&CC=%s" , 
			m_sBillingOutPutURL.c_str(), rData.GetPrivateID().c_str(), rData.GetGoodsNo(), rData.GetItemPayAmt(), m_serviceValue, SA_GAMETYPE, rData.GetCountry().c_str() );
		LOG.PrintTimeAndLog( 0, "ioLocalLatin::ThreadOutputCash() Output url %s", szFullURL );
		if( !Winhttp.GetResultData( szFullURL, "GET", szReturnValue, WEB_BUFF_SIZE ) )
		{
			SP2Packet kPacket( BSTPK_OUTPUT_CASH_RESULT  );
			kPacket << rData.GetUserIndex();
			kPacket << rData.GetBillingGUID();
			kPacket << CASH_RESULT_EXCEPT;
			kPacket << rData.GetItemType();
			if(!g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket ))
			{
				LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:%s Ret %d:%s:%d", "ioLocalLatin::ThreadOutputCash", rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetGoodsNo(), rData.GetGoodsName().c_str(), rData.GetItemPayAmt());
			}
			return;
		}
		
		ioHashStringVec vParseStr;
		Help::GetParseData( szReturnValue, vParseStr, SA_TOKEN );
		if( vParseStr.size() < MAX_GETCASH_ARRAY )
		{
			LOG.PrintTimeAndLog( 0, "%s Size Error. %s", "ioLocalLatin::ThreadOutputCash", szReturnValue );
			return;
		}
		/*
		-1	Error
		0	Cash is 0
		1	Normal
		10	Not found ID
		30	Not enough Cash (Affected 체크 후 정상적이지 않으면 Cash 부족)
		52	Not found request parameters value
		*/
		if(  strcmp(vParseStr[0].c_str(), "-1" ) == 0 )
		{
			SP2Packet kPacket( BSTPK_OUTPUT_CASH_RESULT  );
			kPacket << rData.GetUserIndex();
			kPacket << rData.GetBillingGUID();
			kPacket << CASH_RESULT_EXCEPT;
			kPacket << rData.GetItemType();
			if(!g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket ))
			{
				LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:%s Ret %d:%s:%d", "ioLocalLatin::ThreadOutputCash", rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetGoodsNo(), rData.GetGoodsName().c_str(), rData.GetItemPayAmt());
			}
			
			LOG.PrintTimeAndLog( 0, "%s OutputCash Error: %s:%s:%d:%s:%s", "ioLocalLatin::ThreadOutputCash", rData.GetBillingGUID().c_str(), rData.GetPublicID().c_str(), rData.GetUserIndex(), rData.GetPrivateID().c_str(), vParseStr[0].c_str() );
			return;
		}
		else
		{
			SP2Packet kPacket( BSTPK_OUTPUT_CASH_RESULT );
			kPacket << rData.GetUserIndex();
			kPacket << rData.GetBillingGUID();
			kPacket << CASH_RESULT_SUCCESS;
			kPacket << rData.GetItemType();
			kPacket << rData.GetItemPayAmt();
			kPacket << 0; // TransactionID ( FOR US )
			//kPacket << szChargeNo; // same szChargeNo

			int iItemValueList[MAX_ITEM_VALUE];
			for (int i = 0; i <MAX_ITEM_VALUE; i++)
				iItemValueList[i] = rData.GetItemValue( i );
			ioChannelingNodeParent::SetItemValueList( kPacket, rData.GetItemType(), iItemValueList );


			kPacket << 0; // 공통
			kPacket << atoi(vParseStr[1].c_str() );
			kPacket << atoi(vParseStr[1].c_str() );
	 

			if(!g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket ))
			{
				LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:%s Ret %d:%s:%d", "ioLocalLatin::ThreadOutputCash", rData.GetUserIndex(), rData.GetBillingGUID().c_str(), rData.GetPrivateID().c_str(), rData.GetGoodsNo(), rData.GetGoodsName().c_str(), rData.GetItemPayAmt());
			}
			LOG.PrintTimeAndLog( 0, "%s OutputCash Success: %s:%s:%d:%s:%s", "ioLocalLatin::ThreadOutputCash", rData.GetBillingGUID().c_str(), rData.GetPublicID().c_str(), rData.GetUserIndex(), rData.GetPrivateID().c_str(), vParseStr[1].c_str() );
		}
		
	}
}

//hr 라틴추가
void ioLocalLatin::OnLogoutLog( ServerNode *pServerNode, SP2Packet &rkPacket )
{
	ioHashString szIntID;
	ioHashString szprivateIP;
	ioHashString szpublicIP;
	ioHashString szCountry;
	ioHashString szGender;
	ioHashString szCafe;
	ioHashString szAge;
	ioHashString szTime;
	int			 iServerNo = 0;
	__int64		 getPeso = 0;
	int			 winCount = 0;
	int			 loseCount = 0;
	int			 drawCount = 0;		//hr 무승부 처리 대신 kill 로 보냄
	int			 giveUpCount = 0;	//게임 포기 횟수
	int			 getExp = 0;
	int			 exitCode = 0;
	szIntID.Clear();
	szprivateIP.Clear();
	szpublicIP.Clear();
	szCountry.Clear();
	szGender.Clear();
	szCafe.Clear();
	szAge.Clear();
	szTime.Clear();

	/*
	*/
	rkPacket >> iServerNo >> szIntID >> szCountry >> szGender >> szTime >> szpublicIP >> szprivateIP >> getPeso >> winCount >> loseCount >> drawCount >> giveUpCount >> getExp >> exitCode;

	LOG.PrintTimeAndLog( 0, "%s ID:%s, Country:%s, Gender:%s, Time:%s, publicIP:%s, PrivateIP:%s, Peso:%I64d, win:%d, lose:%d, kill:%d, Giveup:%d, exp:%d, exitCode:%d" , "ioLocalLatin::OnLogoutLog", 
		szIntID.c_str(), szCountry.c_str(), szGender.c_str(), szTime.c_str(),szpublicIP.c_str(), szprivateIP.c_str(), getPeso, winCount, loseCount, drawCount, giveUpCount, getExp, exitCode );
	
	if( !pServerNode )
	{
		LOG.PrintTimeAndLog(0, "%s pServerNode == NULL", __FUNCTION__ );
		return;
	}

	if( szIntID.IsEmpty() || szCountry.IsEmpty() || szprivateIP.IsEmpty() || szTime.IsEmpty() )
	{
		LOG.PrintTimeAndLog( 0, "ioLocalLatin::OnLogoutLog : Value Error" );
		return;
	}
	//접속 끊긴 시간
	SYSTEMTIME st;
	GetLocalTime( &st );
	char szDisconnectDate[MAX_PATH]="";
	StringCbPrintf( szDisconnectDate, sizeof( szDisconnectDate ), "%04d-%02d-%02d_%02d:%02d:%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond );


	char szReturnValue[WEB_BUFF_SIZE]="";
		
	ioHTTP Winhttp; 
		
	char szFullURL[MAX_PATH*2]="";
	StringCbPrintf( szFullURL, sizeof( szFullURL ), "%s?IntID=%s&Connect=%s&Disconnect=%s&ServerNo=%d&Country=%s&CountryByIP=%s&PublicIP=%s&PrivateIP=%s&Win=%d&Draw=0&Lose=%d&Giveup=%d&Gold=%I64d&Exp=%d&Gender=%s&C01=0&C02=0&C03=0&ExitCode=%d"
		,m_sBillingLogURL.c_str(), szIntID.c_str(), szTime.c_str() , szDisconnectDate, iServerNo, szCountry.c_str(), szCountry.c_str(), szpublicIP.c_str(), szprivateIP.c_str()
		, winCount, loseCount, giveUpCount, getPeso, getExp, szGender.c_str(), exitCode );


	if( !Winhttp.GetResultData( szFullURL, "GET", szReturnValue, WEB_BUFF_SIZE ) )
	{
		LOG.PrintTimeAndLog( 0, "ioLocalLatin::OnLogoutLog:Http Error : %s", szFullURL );
		return;
	}
	
	LOG.PrintTimeAndLog( 0, "%s URL :%s, ret : %s", "ioLocalLatin::OnLogoutLog", szFullURL, szReturnValue );
}

void ioLocalLatin::OnCCUCount( ServerNode *pServerNode, SP2Packet &rkPacket )
{
	int dwCCuCount = 0;
	int dwServerNo = -1;

	rkPacket >> dwServerNo;
	rkPacket >> dwCCuCount;
	
	if( !pServerNode )
	{
		LOG.PrintTimeAndLog(0, "%s pServerNode == NULL:ServerNo : %d, ccuCount : %d", __FUNCTION__, dwServerNo, dwCCuCount );
		return;
	}

	if( g_App.IsReserveLogOut() )
	{
		return;
	}
		
	if( g_App.IsTestMode() )
	{	
		char szReturnValue[WEB_BUFF_SIZE]="";
		
		ioHTTP Winhttp; 
		
		char szFullURL[MAX_PATH*2] = "http://112.175.228.42:2515/log_ccu.aspx?ServerNo=2&CCU_Count=1";
		if( !Winhttp.GetResultData( szFullURL, "GET", szReturnValue, WEB_BUFF_SIZE ) )
		{
			return;
		}
	}
	else
	{
		char szReturnValue[WEB_BUFF_SIZE]="";
		
		ioHTTP Winhttp; 
		
		char szFullURL[MAX_PATH*2]="";
		StringCbPrintf( szFullURL, sizeof( szFullURL ), "%s?ServerNo=%d&CCU_Count=%d" , m_sBillingCcuURL.c_str(), dwServerNo, dwCCuCount );
		

		if( !Winhttp.GetResultData( szFullURL, "GET", szReturnValue, WEB_BUFF_SIZE ) )
		{
			LOG.PrintTimeAndLog( 0, "%s Http Error", "ioLocalLatin::OnCCUCount" );
			return;
		}
		LOG.PrintTimeAndLog( 0, "%s URL :%s, ret:%s", "ioLocalLatin::OnCCUCount", szFullURL, szReturnValue );
	}
}