#include "../stdafx.h"
#include "./ioLocalThailand.h"
#include "../Util/md5.h"
#include "../Util/cJSON.h"
#include "../MainProcess.h"
#include "../NodeInfo/ServerNode.h"
#include "../DataBase/LogDBClient.h"
#include "../Channeling/ioChannelingNodeParent.h"
#include "../NodeInfo/ServerNodeManager.h"
#include "../ThailandOTPServer/ThailandOTPNodeManager.h"
#include "../EtcHelpFunc.h"
#include "../ThailandLoginServer/ThailandLoginServer.h"
#include "../ThailandBillingServer/ThailandBillingGetServer.h"
#include "../ThailandBillingServer/ThailandBillingSetServer.h"
#include "../ThailandIPBonusServer/ThailandIPBonusServer.h"
#include "../ThailandIPBonusServer/ThailandIPBonusOutServer.h"
#include "../ThreadPool/ioThreadPool.h"


extern CLog LOG;

ioLocalThailand::ioLocalThailand(void)
{
	m_bLoginSendAlive   = false;
	m_bIPBonusSendAlive = false;
	m_bIPBonusOutSendAlive = false;
	m_bBillingGetSendAlive = false;
	m_bBillingSetSendAlive = false;
}

ioLocalThailand::~ioLocalThailand(void)
{
	ReleaseMemoryPool();
}

ioLocalManager::LocalType ioLocalThailand::GetType()
{
	return ioLocalManager::LCT_THAILAND;
}


void ioLocalThailand::Init()
{
	InitMemoryPool();
}

void ioLocalThailand::OnLoginData( ServerNode *pServerNode, SP2Packet &rkPacket )
{
	ioHashString szBillingGUID;
	ioHashString szPrivateID;
	ioHashString sEncodePW;
	ioHashString szPublicIP;
	DWORD        dwReturnMsgType;

	rkPacket >> szBillingGUID >> szPrivateID >> sEncodePW >> szPublicIP >> dwReturnMsgType;

	if( !pServerNode )
	{
		LOG.PrintTimeAndLog(0, "%s pServerNode == NULL:%s:%s:%s:%s:%d", __FUNCTION__, szBillingGUID.c_str(), szPrivateID.c_str(), sEncodePW.c_str(), szPublicIP.c_str(), dwReturnMsgType );
		return;
	}

	if( g_App.IsReserveLogOut() )
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

	ioData kData;
	kData.SetBillingGUID( szBillingGUID );
	kData.SetPrivateID( szPrivateID );
	kData.SetEncodePW( sEncodePW );
	kData.SetUserIP( szPublicIP );
	kData.SetReturnMsgType( dwReturnMsgType );
	kData.SetServerIP( pServerNode->GetIP() );
	kData.SetServerPort( pServerNode->GetClientPort() );

	if( m_LoginMgr.CheckLogin( kData ) )
		return;

	if( g_App.IsTestMode() )
	{
		// 성공
		SP2Packet kPacket( dwReturnMsgType );
		kPacket << szPrivateID;
		kPacket << szBillingGUID;
		kPacket << BILLING_LOGIN_RESULT_SUCCESS;

		// THPP.tester001에서 tester001 분리
		enum { MAX_DOMAIN = 4 };
		char szThaillandID[MAX_PATH]="";
		int iCnt = 0;
		int iSize = szPrivateID.Length();
		iSize--; // 로그인 ID 구분자 | 제거
		for (int i = 0; i < iSize; i++)
		{
			if( i >= MAX_DOMAIN )
			{
				szThaillandID[iCnt] = szPrivateID.At(i);
				iCnt++;
				if( iCnt >= MAX_PATH )
					break;
			}
		}
		char szRealThaillandID[MAX_PATH]="";
		StringCbPrintf( szRealThaillandID, sizeof( szThaillandID ), "_%s_", szThaillandID );
		kPacket << szRealThaillandID; // 테스트시에는 유저가 전송ID가 real private id
		pServerNode->SendMessage( kPacket );
		return;
	}

	ThailandUser *pUser = InsertUser();
	if( !pUser )
	{
		LOG.PrintTimeAndLog(0, "%s Fail InsertUser:%s:%s:%s", __FUNCTION__, szBillingGUID.c_str(), szPrivateID.c_str(), sEncodePW.c_str() );
		SP2Packet kPacket( dwReturnMsgType );
		kPacket << szPrivateID;
		kPacket << szBillingGUID;
		kPacket << BILLING_LOGIN_RESULT_FAIL;
		kPacket << false;
		pServerNode->SendMessage( kPacket );
		return;
	}

	ioHashString sPID;
	pUser->SetData( TIMEGETTIME(), szBillingGUID, szPrivateID, sEncodePW, szPublicIP, dwReturnMsgType, pServerNode->GetIP(), pServerNode->GetClientPort() );
	SendLogin( pUser->GetThailandID().c_str(), pUser->GetPW().c_str(), pUser->GetDomain().c_str(), pUser->GetIP().c_str(), false, sPID );
	pUser->SetPID( sPID );
	LOG.PrintTimeAndLog(0, "%s Send:%s:%s:%s[%s]", __FUNCTION__, pUser->GetBillingGUID().c_str(), pUser->GetPrivateID().c_str(), pUser->GetPW().c_str(), sPID.c_str() );
}

void ioLocalThailand::SendLogin( IN const char *szLoginID, IN const char *szPW, IN const char *szDomain, IN const char *szIP, IN bool bCheckAlive, OUT ioHashString &rsPID )
{
	if( !szLoginID || !szPW || !szDomain || !szIP )
	{
		LOG.PrintTimeAndLog( 0, "%s NULL", __FUNCTION__ );
		return;
	}

	// id|pw|domain|ip|sock_id;
	// playid001|4bb0bfdc6b98dbf3813597ec6ed65d25|THPP|203.144.226.26|8599AE3124094875;

	char szHexMD5PW[MAX_PATH]="";
	ZeroMemory( szHexMD5PW, sizeof( szHexMD5PW ) );
	GetHexMD5( szHexMD5PW, sizeof( szHexMD5PW ), szPW );
	_strlwr_s( szHexMD5PW );

	char szAgencyNo[ioLocalParent::MAX_AGENCY_NO_PLUS_ONE]="";
	GetAgencyNo( szAgencyNo, sizeof( szAgencyNo ), false );

	char szData[MAX_PATH*2]="";
	StringCbPrintf( szData, sizeof( szData ), "%s|%s|%s|%s|%s;", szLoginID, szHexMD5PW, szDomain, szIP, szAgencyNo );

	SP2Packet kPacket( BTTPK_LOGIN_REQUEST );
	kPacket << szData;
	kPacket.SetBufferSizeMinusOne(); // 마지막 NULL 문자 제거
	g_ThailandLoginServer.SendMessage( kPacket );

	rsPID = szAgencyNo;

	if( bCheckAlive )
	{
		if( m_bLoginSendAlive )
		{
			LOG.PrintTimeAndLog( 0, "%s Server No Answer.", __FUNCTION__  );
			g_LogDBClient.OnInsertBillingServerError( CNT_WEMADEBUY, BILLING_ERROR_LOG_NO_ANSWER, "Login server no answer." );
		}

		m_sLoginCheckAliveID = szAgencyNo;
		m_bLoginSendAlive    = true;
	}
}

void ioLocalThailand::OnRecieveLoginData( const ioHashString &rsResult )
{
	/*
	"error_code|error_desc|master_id|akey_flag|akey_user_id|sock_id;"
	"0|Success|master_001|0|master_akey_001|8599AE3124094875;"
	"0|Success|master_001|1|master_akey_001|8599AE3124094875;" 
	"-1001|Invalid request parameters||0||8599AE3124094875;" 
	"-1002|Invalid user id||0||8599AE3124094875;"
	"-1003|User id not found||0||8599AE3124094875;"
	"-1004|Wrong password||0||8599AE3124094875;"
	"-9009|DB error||0||8599AE3124094875;"
	"-9010|Unexpected error||0||8599AE3124094875;"
	*/

	ioHashStringVec vParseStr;
	Help::GetParseData( rsResult, vParseStr, THAILAND_TOKEN );

	if( vParseStr.size() < MAX_LOGIN_ARRAY )
	{
		LOG.PrintTimeAndLog( 0, "%s Size Error. %s", __FUNCTION__, rsResult.c_str() );
		return;
	}

	// 끝에 ; 제거 
	Help::DelEndCh( vParseStr[LOGIN_ARRAY_PID] );

	if( m_sLoginCheckAliveID == vParseStr[LOGIN_ARRAY_PID] )
	{
		m_bLoginSendAlive = false; // 초기화
		return;
	}


	ThailandUser *pUser = GetUser( vParseStr[LOGIN_ARRAY_PID] );
	if( !pUser )
	{
		LOG.PrintTimeAndLog( 0, "%s pUser == NULL. %s", __FUNCTION__, rsResult.c_str() );
		return;
	}

	// error
	if( vParseStr[LOGIN_ARRAY_CODE] != "0" )
	{
		LOG.PrintTimeAndLog(0, "%s Return Error.:%s:%s[%s]", __FUNCTION__, pUser->GetPrivateID().c_str(), pUser->GetBillingGUID().c_str(), rsResult.c_str() );

		ioHashStringVec vParseStr;
		Help::GetParseData( rsResult, vParseStr, THAILAND_TOKEN );

		SP2Packet kPacket( pUser->GetReturnMsgType() );
		kPacket << pUser->GetPrivateID();
		kPacket << pUser->GetBillingGUID();
		kPacket << BILLING_LOGIN_RESULT_FAIL;
		if( !vParseStr[LOGIN_ARRAY_DESC].IsEmpty() )
		{
			kPacket << true;
			kPacket << vParseStr[LOGIN_ARRAY_DESC];
		}
		else
		{
			kPacket << false;
		}

		if( !g_ServerNodeManager.SendMessageIP( (ioHashString)pUser->GetServerIP(), pUser->GetServerPort(), kPacket ) )
		{	
			LOG.PrintTimeAndLog( 0, "%s Send Fail", __FUNCTION__ );
		}
		RemoveUser( vParseStr[LOGIN_ARRAY_PID] );
		return;
	}
	
	char szRealPrivateID[MAX_PATH]="";
	bool bRealPrivateID = false;
	bool bRealOTPID     = false;
	if( !vParseStr[LOGIN_ARRAY_ID].IsEmpty() )
	{
		StringCbPrintf( szRealPrivateID, sizeof( szRealPrivateID ), "%s.%s", pUser->GetDomain().c_str(), vParseStr[LOGIN_ARRAY_ID].c_str() );
		bRealPrivateID = true;
	}
	

	if( vParseStr[LOGIN_ARRAY_OTPUSE] == "1" && !vParseStr[LOGIN_ARRAY_OTPID].IsEmpty() )
	{
		bRealOTPID = true;
	}

	ioData kData;
	kData.SetPrivateID( pUser->GetPrivateID() );
	kData.SetEncodePW( pUser->GetPW() );
	kData.SetReturnMsgType( pUser->GetReturnMsgType() );
	kData.SetServerIP( (ioHashString)pUser->GetServerIP() );
	kData.SetServerPort( pUser->GetServerPort() );

	if( !m_LoginMgr.InsertLogin( kData, szRealPrivateID, 0, bRealOTPID, LoginInfo::PT_DECODE_LOGIN_PW ) )
	{
		RemoveUser( vParseStr[LOGIN_ARRAY_PID] );
		return;
	}

	// success
	SP2Packet kPacket( pUser->GetReturnMsgType() );
	kPacket << pUser->GetPrivateID();
	kPacket << pUser->GetBillingGUID();
	kPacket << BILLING_LOGIN_RESULT_SUCCESS;
	if( bRealPrivateID )
		kPacket << szRealPrivateID;
	if( bRealOTPID )
		kPacket << vParseStr[LOGIN_ARRAY_OTPID];

	if( !g_ServerNodeManager.SendMessageIP( (ioHashString)pUser->GetServerIP(), pUser->GetServerPort(), kPacket ) )
	{	
		LOG.PrintTimeAndLog( 0, "%s Send Fail", __FUNCTION__ );
	}

	RemoveUser( vParseStr[LOGIN_ARRAY_PID] );
	LOG.PrintTimeAndLog(0, "%s Success.:[%s]%s:%s[%s]", __FUNCTION__, szRealPrivateID, pUser->GetPrivateID().c_str(), pUser->GetBillingGUID().c_str(), rsResult.c_str() );
}


void ioLocalThailand::OnOTP( ServerNode *pServerNode, SP2Packet &rkPacket )
{
	ioHashString szBillingGUID;
	ioHashString szPrivateID;
	ioHashString sEncodePW;
	rkPacket >> szBillingGUID >> szPrivateID >> sEncodePW;

	if( !pServerNode )
	{
		LOG.PrintTimeAndLog(0, "%s pServerNode == NULL:%s:%s:%s", __FUNCTION__, szBillingGUID.c_str(), szPrivateID.c_str(), sEncodePW.c_str() );
		return;
	}

	if( g_App.IsReserveLogOut() )
	{
		LOG.PrintTimeAndLog(0, "%s LogOut:%s:%s:%s", __FUNCTION__, szBillingGUID.c_str(), szPrivateID.c_str(), sEncodePW.c_str() );
		SP2Packet kPacket( BSTPK_OTP_RESULT );
		kPacket << szPrivateID;
		kPacket << szBillingGUID;
		kPacket << BILLING_OTP_RESULT_FAIL;
		kPacket << false;
		pServerNode->SendMessage( kPacket );
		return;
	}

	if( g_App.IsTestMode() )
	{
		// 성공
		SP2Packet kPacket( BSTPK_OTP_RESULT );
		kPacket << szPrivateID;
		kPacket << szBillingGUID;
		kPacket << BILLING_OTP_RESULT_SUCCESS;
		return;
	}

	ThailandOTPNode *pNode = g_ThailandOTPNodeManager.ConnectTo();
	if( !pNode )
	{
		LOG.PrintTimeAndLog(0, "%s Fail ConnectTo:%s:%s:%s", __FUNCTION__, szBillingGUID.c_str(), szPrivateID.c_str(), sEncodePW.c_str() );
		SP2Packet kPacket( BSTPK_OTP_RESULT );
		kPacket << szPrivateID;
		kPacket << szBillingGUID;
		kPacket << BILLING_OTP_RESULT_FAIL;
		kPacket << false;
		pServerNode->SendMessage( kPacket );
		return;
	}

	ThailandUser &rUser = pNode->GetUser();
	rUser.SetData( 0, szBillingGUID, szPrivateID, sEncodePW, pServerNode->GetIP(), 0, pServerNode->GetIP(), pServerNode->GetClientPort() );

	char szLowerDomain[MAX_PATH]="";
	StringCbCopy( szLowerDomain, sizeof( szLowerDomain ), rUser.GetDomain().c_str() );
	_strlwr_s( szLowerDomain );
	int iPacketSize = rUser.GetDomain().Length() + rUser.GetOTPID().Length() + rUser.GetOTPPW().Length() + 18;

	// |1001|42|thas|AP00000001||123456|9999|
	char szData[MAX_PATH*2]="";
	StringCbPrintf( szData, sizeof( szData ), "|1001|%d|%s|%s||%s|9999|", iPacketSize, szLowerDomain, rUser.GetOTPID().c_str(), rUser.GetOTPPW().c_str() );
	SP2Packet kPacket( BTTPK_OTP_REQUEST );
	kPacket << szData;
	kPacket.SetBufferSizeMinusOne(); // 마지막 NULL 문자 제거
	pNode->SendMessage( kPacket );
	LOG.PrintTimeAndLog(0, "%s Send:%s:%s:%s[%s]", __FUNCTION__, rUser.GetBillingGUID().c_str(), rUser.GetPrivateID().c_str(), rUser.GetPW().c_str(), szData );
}

void ioLocalThailand::OnRecieveOTP( const ioHashString &rsResult, const ThailandUser &rUser )
{
/*
	"|1002|28|true|Success|9999|"
	"|1002|31|false|Invalid OTP|9999|"
*/
	ioHashStringVec vParseStr;
	Help::GetParseData( rsResult, vParseStr, THAILAND_TOKEN );

	if( vParseStr.size() < MAX_OTP_ARRAY )
	{
		LOG.PrintTimeAndLog( 0, "%s Size Error. %s", __FUNCTION__, rsResult.c_str() );
		return;
	}

	vParseStr[OTP_ARRAY_STATUS].MakeLower();

	// error
	if( vParseStr[OTP_ARRAY_STATUS] == "false" )
	{
		LOG.PrintTimeAndLog(0, "%s Return Error.:%s:%s[%s]", __FUNCTION__, rUser.GetPrivateID().c_str(), rUser.GetBillingGUID().c_str(), rsResult.c_str() );

		SP2Packet kPacket( BSTPK_OTP_RESULT );
		kPacket << rUser.GetPrivateID();
		kPacket << rUser.GetBillingGUID();
		kPacket << BILLING_OTP_RESULT_FAIL;
		if( !vParseStr[OTP_ARRAY_DESC].IsEmpty() )
		{
			kPacket << true;
			kPacket << vParseStr[OTP_ARRAY_DESC];
		}
		else
		{
			kPacket << false;
		}

		if( !g_ServerNodeManager.SendMessageIP( (ioHashString)rUser.GetServerIP(), rUser.GetServerPort(), kPacket ) )
		{	
			LOG.PrintTimeAndLog( 0, "%s Send Fail", __FUNCTION__ );
		}
		return;
	}

	if( !m_LoginMgr.InsertOTP( rUser ) )
	{
		LOG.PrintTimeAndLog( 0, "%s Fail InsertOTP %s", __FUNCTION__, rsResult.c_str() );
		return;
	}

	// success
	SP2Packet kPacket( BSTPK_OTP_RESULT );
	kPacket << rUser.GetPrivateID();
	kPacket << rUser.GetBillingGUID();
	kPacket << BILLING_OTP_RESULT_SUCCESS;
	
	if( !g_ServerNodeManager.SendMessageIP( (ioHashString)rUser.GetServerIP(), rUser.GetServerPort(), kPacket ) )
	{	
		LOG.PrintTimeAndLog( 0, "%s Send Fail", __FUNCTION__ );
	}


	LOG.PrintTimeAndLog(0, "%s Success.:[%s]%s:%s[%s]", __FUNCTION__, rUser.GetOTPID().c_str(), rUser.GetPrivateID().c_str(), rUser.GetBillingGUID().c_str(), rsResult.c_str() );
}

void ioLocalThailand::_OnGetCash( ServerNode *pServerNode, SP2Packet &rkPacket )
{
	
	int          iChannelingType   = 0;
	ioHashString szBillingGUID;
	DWORD        dwUserIndex       = 0;
	ioHashString szPrivateID;
	ioHashString szPublicID;
	bool         bSetUserMouse     = false;
	ioHashString szPublicIP;

	rkPacket >> iChannelingType >> szBillingGUID >> dwUserIndex >> szPrivateID >> szPublicID >> bSetUserMouse; // 공통사항
	rkPacket >> szPublicIP;

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
		int iCash = 0;
		TestCashManager::TestCashInfo *pInfo = m_TestCashManager.GetInfo( szPrivateID );
		if( pInfo )
		{
			m_TestCashManager.CheckNChargeCash( pInfo );
			iCash = pInfo->m_iCash;
		}
		else
		{
			m_TestCashManager.AddInfo( szPrivateID );
			TestCashManager::TestCashInfo *pInfo = m_TestCashManager.GetInfo( szPrivateID );
			if( pInfo )
			{
				iCash = pInfo->m_iCash;
			}
		}

		SP2Packet kPacket( BSTPK_GET_CASH_RESULT );
		kPacket << dwUserIndex;
		kPacket << szBillingGUID;
		kPacket << bSetUserMouse;
		kPacket << CASH_RESULT_SUCCESS;
		kPacket << iCash;
		kPacket << iCash;  // total cash와 purchase cash가 동일함.
		pServerNode->SendMessage( kPacket );
		LOG.PrintTimeAndLog( 0, "%s Test Success: %d:%s:PrivateID %s:%s:%d", __FUNCTION__, dwUserIndex, szBillingGUID.c_str(), szPrivateID.c_str(), szPublicID.c_str(), iCash  );
		return;
	}

	ThailandUser *pUser = InsertUser();
	if( !pUser )
	{
		LOG.PrintTimeAndLog(0, "%s Fail InsertUser:%s:%s:%s", __FUNCTION__, szBillingGUID.c_str(), szPrivateID.c_str(), szPublicIP.c_str() );
		SP2Packet kPacket( BSTPK_GET_CASH_RESULT );
		kPacket << dwUserIndex;
		kPacket << szBillingGUID;
		kPacket << bSetUserMouse;
		kPacket << CASH_RESULT_EXCEPT;
		kPacket << false;
		pServerNode->SendMessage( kPacket );
		return;
	}

	ioHashString sPID;
	ioHashString sEncodePW     = "";
	pUser->SetData( TIMEGETTIME(), szBillingGUID, szPrivateID, sEncodePW, szPublicIP, 0, pServerNode->GetIP(), pServerNode->GetClientPort() );

	int iItemValueList[MAX_ITEM_VALUE];
	for (int i = 0; i < MAX_ITEM_VALUE; i++)
		iItemValueList[i] = 0;
	pUser->SetExtendData( 0, dwUserIndex, bSetUserMouse, 0, iItemValueList, 0 );
	SendGetCash( pUser->GetPrivateID().c_str(), pUser->GetIP().c_str(), false, sPID );
	pUser->SetPID( sPID );
	LOG.PrintTimeAndLog(0, "%s Send:%s:%s:%s:[%s]", __FUNCTION__, szBillingGUID.c_str(), szPrivateID.c_str(), szPublicIP.c_str(), sPID.c_str() );
}

void ioLocalThailand::SendGetCash( IN const char *szPrivateID, IN const char *szPublicIP, IN bool bCheckAlive, OUT ioHashString &rsPID )
{
	if( !szPrivateID || !szPublicIP )
	{
		LOG.PrintTimeAndLog( 0, "%s NULL", __FUNCTION__ );
		return;
	}

	// id|serviceid|ip|sock_id;
	// THPP.playid001|105|203.144.226.26|NC56M4E2F94NFNEN;
	// THAS.asid001|105|203.144.226.26|C8B7M68T882P9X5T;

	char szAgencyNo[ioLocalParent::MAX_AGENCY_NO_PLUS_ONE]="";
	GetAgencyNo( szAgencyNo, sizeof( szAgencyNo ), false );

	char szData[MAX_PATH*2]="";
	StringCbPrintf( szData, sizeof( szData ), "%s|105|%s|%s;", szPrivateID, szPublicIP, szAgencyNo );

	SP2Packet kPacket( BTTPK_BILLING_GET_REQUEST );
	kPacket << szData;
	kPacket.SetBufferSizeMinusOne(); // 마지막 NULL 문자 제거
	g_ThailandBillingGetServer.SendMessage( kPacket );

	rsPID = szAgencyNo;

	if( bCheckAlive )
	{
		if( m_bBillingGetSendAlive )
		{
			LOG.PrintTimeAndLog( 0, "%s Server No Answer.", __FUNCTION__  );
			g_LogDBClient.OnInsertBillingServerError( CNT_WEMADEBUY, BILLING_ERROR_LOG_NO_ANSWER, "Billing Get server no answer." );
		}

		m_sBillingGetCheckAliveID = szAgencyNo;
		m_bBillingGetSendAlive    = true;
	}
}

void ioLocalThailand::OnRecieveGetCash( const ioHashString &rsResult )
{
/*
	error_code|error_desc|Balancecash|BalanceBonus|sock_id;
	0|Success|0|0|NC56M4E2F94NFNEN;
	0|Success|100|0|NC56M4E2F94NFNEN;
	0|Success|0|200|NC56M4E2F94NFNEN;
	-1001|Invalid request parameters|0|0|NC56M4E2F94NFNEN;
	-8001|Invalid Userid|0|0|NC56M4E2F94NFNEN;
    -8002|Record not found|0|0|NC56M4E2F94NFNEN;
	-8003|Invalid ServiceID|0|0|NC56M4E2F94NFNEN;
	-9009|DB error|0|0|NC56M4E2F94NFNEN;
	-9010|Unexpected error|0|0|NC56M4E2F94NFNEN;
*/
	ioHashStringVec vParseStr;
	Help::GetParseData( rsResult, vParseStr, THAILAND_TOKEN );

	if( vParseStr.size() < MAX_BILLING_GET_ARRAY )
	{
		LOG.PrintTimeAndLog( 0, "%s Size Error. %s", __FUNCTION__, rsResult.c_str() );
		return;
	}

	// 끝에 ; 제거 
	Help::DelEndCh( vParseStr[BILLING_GET_ARRAY_PID] );

	if( m_sBillingGetCheckAliveID == vParseStr[BILLING_GET_ARRAY_PID] )
	{
		m_bBillingGetSendAlive = false; // 초기화
		return;
	}

	ThailandUser *pUser = GetUser( vParseStr[BILLING_GET_ARRAY_PID] );
	if( !pUser )
	{
		LOG.PrintTimeAndLog( 0, "%s pUser == NULL. %s", __FUNCTION__, rsResult.c_str() );
		return;
	}

	if( vParseStr[BILLING_GET_ARRAY_CODE] != "0" )
	{
		SP2Packet kPacket( BSTPK_GET_CASH_RESULT );
		kPacket << pUser->GetUserIndex();
		kPacket << pUser->GetBillingGUID();
		kPacket << pUser->IsMouseBusy();
		kPacket << CASH_RESULT_EXCEPT;
		if( !vParseStr[BILLING_GET_ARRAY_DESC].IsEmpty() )
		{
			kPacket << true;
			kPacket << vParseStr[BILLING_GET_ARRAY_DESC];
		}
		else
		{
			kPacket << false;
		}
		if( !g_ServerNodeManager.SendMessageIP( (ioHashString)pUser->GetServerIP(), pUser->GetServerPort(), kPacket ) )
		{	
			LOG.PrintTimeAndLog( 0, "%s Send Fail", __FUNCTION__ );
		}
		LOG.PrintTimeAndLog(0, "%s Return Error.:%s:%s[%s]", __FUNCTION__, pUser->GetBillingGUID().c_str(), pUser->GetPrivateID().c_str(), rsResult.c_str() );
		RemoveUser( vParseStr[BILLING_GET_ARRAY_PID] );
		return;
	}

	// success
	int iCash  = atoi( vParseStr[BILLING_GET_ARRAY_CASH].c_str() );
	int iBonus = atoi( vParseStr[BILLING_GET_ARRAY_BONUS].c_str() );
	int iTotalCash = iCash + iBonus;
	SP2Packet kPacket( BSTPK_GET_CASH_RESULT );
	kPacket << pUser->GetUserIndex();
	kPacket << pUser->GetBillingGUID();
	kPacket << pUser->IsMouseBusy();
	kPacket << CASH_RESULT_SUCCESS;
	kPacket << iTotalCash;
	kPacket << iTotalCash;  // total cash와 purchase cash가 동일함.
	if( !g_ServerNodeManager.SendMessageIP( (ioHashString)pUser->GetServerIP(), pUser->GetServerPort(), kPacket ) )
	{	
		LOG.PrintTimeAndLog( 0, "%s Send Fail", __FUNCTION__ );
	}
	LOG.PrintTimeAndLog(0, "%s Success.:%s:%s:[%s]", __FUNCTION__, pUser->GetBillingGUID().c_str(), pUser->GetPrivateID().c_str(), rsResult.c_str() );

	RemoveUser( vParseStr[BILLING_GET_ARRAY_PID] );
}

void ioLocalThailand::_OnOutputCash( ServerNode *pServerNode, SP2Packet &rkPacket, DWORD dwGoodsNo, const ioHashString &rszGoodsName )
{
	int          iChannelingType = 0;
	ioHashString szBillingGUID;
	DWORD        dwUserIndex = 0;
	ioHashString szPublicID;
	ioHashString szPrivateID;
	ioHashString szUserIP;
	int          iPayAmt  = 0;
	int          iType    = 0;
	ioHashString szPresentPrivateID;

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

	int iItemValueList[MAX_ITEM_VALUE];
	for (int i = 0; i < MAX_ITEM_VALUE; i++)
	{
		iItemValueList[i] = 0;
	}
	ioChannelingNodeParent::GetItemValueList( rkPacket, iType, iItemValueList );

	// Add
	rkPacket >> szPresentPrivateID;

	if( szPresentPrivateID.IsEmpty() )
		szPresentPrivateID = szPrivateID;


	if( g_App.IsTestMode() )
	{
		int iCash = 0;
		TestCashManager::TestCashInfo *pInfo = m_TestCashManager.GetInfo( szPrivateID );
		if( pInfo )
		{
			m_TestCashManager.CheckNChargeCash( pInfo );
			pInfo->m_iCash -= iPayAmt;
			iCash = pInfo->m_iCash;
		}
		else
		{
			m_TestCashManager.AddInfo( szPrivateID );
			TestCashManager::TestCashInfo *pInfo = m_TestCashManager.GetInfo( szPrivateID );
			if( pInfo )
			{
				pInfo->m_iCash -= iPayAmt;
				iCash = pInfo->m_iCash;
			}
		}

		SP2Packet kPacket( BSTPK_OUTPUT_CASH_RESULT );
		kPacket << dwUserIndex;
		kPacket << szBillingGUID;
		kPacket << CASH_RESULT_SUCCESS;
		kPacket << iType;
		kPacket << iPayAmt;
		kPacket << 0; // TransactionID ( FOR US )
		ioChannelingNodeParent::SetItemValueList( kPacket, iType, iItemValueList );
		kPacket << iChannelingType; // 공통
		kPacket << iCash;
		kPacket << iCash; // 캐쉬와 실재 구매한 캐쉬가 동일하다.
		pServerNode->SendMessage( kPacket ); 
		LOG.PrintTimeAndLog( 0, "%s Test Success: %d:%s:PrivateID %s:%s:%d", __FUNCTION__, dwUserIndex, szBillingGUID.c_str(), szPrivateID.c_str(), szPublicID.c_str(), iCash  );
		return;
	}

	ThailandUser *pUser = InsertUser();
	if( !pUser )
	{
		LOG.PrintTimeAndLog(0, "%s Fail InsertUser:%s:%s:%s", __FUNCTION__,szBillingGUID.c_str(),  szPrivateID.c_str(), szUserIP.c_str() );
		SP2Packet kPacket( BSTPK_OUTPUT_CASH_RESULT );
		kPacket << dwUserIndex;
		kPacket << szBillingGUID;
		kPacket << CASH_RESULT_EXCEPT;
		kPacket << iType;
		kPacket << false;
		pServerNode->SendMessage( kPacket );
		return;
	}

	ioHashString sPID;
	ioHashString sEncodePW     = "";
	pUser->SetData( TIMEGETTIME(), szBillingGUID, szPrivateID, sEncodePW, szUserIP, 0, pServerNode->GetIP(), pServerNode->GetClientPort() );
	pUser->SetExtendData( iChannelingType, dwUserIndex, false, iType, iItemValueList, iPayAmt );
	SendOutputCash( pUser->GetPrivateID().c_str(), pUser->GetIP().c_str(), szPresentPrivateID.c_str(), iPayAmt, dwGoodsNo, false, sPID );
	pUser->SetPID( sPID );
	LOG.PrintTimeAndLog(0, "%s Send:%s:%s:%s:[%s]", __FUNCTION__, szBillingGUID.c_str(), szPrivateID.c_str(), szUserIP.c_str(), sPID.c_str() );
}

void ioLocalThailand::SendOutputCash( IN const char *szPrivateID, IN const char *szPublicIP, IN const char *szPresentPrivateID, int iPayAmt, DWORD dwGoodsNo, IN bool bCheckAlive, OUT ioHashString &rsPID )
{
	if( !szPrivateID || !szPublicIP || !szPresentPrivateID )
	{
		LOG.PrintTimeAndLog( 0, "%s NULL", __FUNCTION__ );
		return;
	}

	// Userid|GifeUserID|Itemprice|ItemID|QTY|serviceid|Source|Txno|ip|sock_id;
	// THPP.playid001|THPP.playid001|1000|354|1|105|1|201110300000000000000000000001|203.144.226.26|8J245G6XBH9F82HN;
	// THAS.aid001|THPP.aid001|1000|356|1|105|1|201110300000000000000000000002|203.144.226.26|S656H8NHR6SPZ757;
	// THTC.tcgid001|THPP.tcgid001|1000|354|1|105|1|201110300000000000000000000003|203.144.226.26|C468E7S7YEN8N9MN;

	char szGoodGUID[ioLocalParent::MAX_AGENCY_NO_PLUS_ONE]="";
	GetAgencyNo( szGoodGUID, sizeof( szGoodGUID ), true );

	char szAgencyNo[ioLocalParent::MAX_AGENCY_NO_PLUS_ONE]="";
	GetAgencyNo( szAgencyNo, sizeof( szAgencyNo ), false );

	char szData[MAX_PATH*2]="";
	StringCbPrintf( szData, sizeof( szData ), "%s|%s|%d|%d|1|105|1|%s|%s|%s;", szPrivateID, szPresentPrivateID, iPayAmt, dwGoodsNo, szGoodGUID, szPublicIP, szAgencyNo );

	SP2Packet kPacket( BTTPK_BILLING_SET_REQUEST );
	kPacket << szData;
	kPacket.SetBufferSizeMinusOne(); // 마지막 NULL 문자 제거
	g_ThailandBillingSetServer.SendMessage( kPacket );

	rsPID = szAgencyNo;

	if( bCheckAlive )
	{
		if( m_bBillingSetSendAlive )
		{
			LOG.PrintTimeAndLog( 0, "%s Server No Answer.", __FUNCTION__  );
			g_LogDBClient.OnInsertBillingServerError( CNT_WEMADEBUY, BILLING_ERROR_LOG_NO_ANSWER, "Billing Set server no answer." );
		}

		m_sBillingSetCheckAliveID = szAgencyNo;
		m_bBillingSetSendAlive    = true;
	}
}

void ioLocalThailand::OnRecieveOutputCash( const ioHashString &rsResult )
{
/*
    error_code|error_desc|Balancecash|BalanceBonus|CashTransactionconfirm|sock_id;
	0|Success|0|0|000000000001;
	0|Success|0|0|000000000001|8J245G6XBH9F82HN;
	-1001|Invalid request parameters|0|0||8J245G6XBH9F82HN;
	-8001|Invalid Userid|0|0||8J245G6XBH9F82HN;
	-8002|Record not found|0|0||8J245G6XBH9F82HN;
	-8003|Invalid Input Item Detail|100|0||8J245G6XBH9F82HN;
	-8004|Not Enough Cash to buy Item|100|0||8J245G6XBH9F82HN;
*/
	ioHashStringVec vParseStr;
	Help::GetParseData( rsResult, vParseStr, THAILAND_TOKEN );

	if( vParseStr.size() < MAX_BILLING_SET_ARRAY )
	{
		LOG.PrintTimeAndLog( 0, "%s Size Error. %s", __FUNCTION__, rsResult.c_str() );
		return;
	}

	// 끝에 ; 제거 
	Help::DelEndCh( vParseStr[BILLING_SET_ARRAY_PID] );

	if( m_sBillingSetCheckAliveID == vParseStr[BILLING_SET_ARRAY_PID] )
	{
		m_bBillingSetSendAlive = false; // 초기화
		return;
	}

	ThailandUser *pUser = GetUser( vParseStr[BILLING_SET_ARRAY_PID] );
	if( !pUser )
	{
		LOG.PrintTimeAndLog( 0, "%s pUser == NULL. %s", __FUNCTION__, rsResult.c_str() );
		return;
	}

	if( vParseStr[BILLING_SET_ARRAY_CODE] != "0" )
	{
		SP2Packet kPacket( BSTPK_OUTPUT_CASH_RESULT );
		kPacket << pUser->GetUserIndex();
		kPacket << pUser->GetBillingGUID();
		kPacket << CASH_RESULT_EXCEPT;
		kPacket << pUser->GetItemType();
		if( !vParseStr[BILLING_SET_ARRAY_DESC].IsEmpty() )
		{
			kPacket << true;
			kPacket << vParseStr[BILLING_SET_ARRAY_DESC];
		}
		else
		{
			kPacket << false;
		}
		if( !g_ServerNodeManager.SendMessageIP( (ioHashString)pUser->GetServerIP(), pUser->GetServerPort(), kPacket ) )
		{	
			LOG.PrintTimeAndLog( 0, "%s Send Fail", __FUNCTION__ );
		}
		LOG.PrintTimeAndLog(0, "%s Return Error.:%s:%s[%s]", __FUNCTION__, pUser->GetBillingGUID().c_str(), pUser->GetPrivateID().c_str(), rsResult.c_str() );
		RemoveUser( vParseStr[BILLING_SET_ARRAY_PID] );
		return;
	}

	// success
	int iCash  = atoi( vParseStr[BILLING_SET_ARRAY_CASH].c_str() );
	int iBonus = atoi( vParseStr[BILLING_SET_ARRAY_BONUS].c_str() );
	int iTotalCash = iCash + iBonus;

	int iItemValueList[MAX_ITEM_VALUE];
	for (int i = 0; i < MAX_ITEM_VALUE; i++)
	{
		iItemValueList[i] = pUser->GetItemValue( i );
	}

	SP2Packet kPacket( BSTPK_OUTPUT_CASH_RESULT );
	kPacket << pUser->GetUserIndex();
	kPacket << pUser->GetBillingGUID();
	kPacket << CASH_RESULT_SUCCESS;
	kPacket << pUser->GetItemType();
	kPacket << pUser->GetPayAmt();
	kPacket << 0; // TransactionID ( FOR US )
	ioChannelingNodeParent::SetItemValueList( kPacket, pUser->GetItemType(), iItemValueList );

	kPacket << pUser->GetChannelingType(); // 공통
	kPacket << iTotalCash;
	kPacket << iTotalCash;  // total cash와 purchase cash가 동일함.
	if( !g_ServerNodeManager.SendMessageIP( (ioHashString)pUser->GetServerIP(), pUser->GetServerPort(), kPacket ) )
	{	
		LOG.PrintTimeAndLog( 0, "%s Send Fail", __FUNCTION__ );
	}
	LOG.PrintTimeAndLog(0, "%s Success.:%s:%s:[%d:%d:%d:%d][%s]", __FUNCTION__, pUser->GetBillingGUID().c_str(), pUser->GetPrivateID().c_str(), iCash, iBonus, pUser->GetItemType(), pUser->GetPayAmt(), rsResult.c_str() );
	RemoveUser( vParseStr[BILLING_SET_ARRAY_PID]  );
}

void ioLocalThailand::OnIPBonus( ServerNode *pServerNode, SP2Packet &rkPacket )
{
	ioHashString szPrivateID;
	ioHashString szPublicIP;
	rkPacket >> szPrivateID >> szPublicIP;

	if( !pServerNode )
	{
		LOG.PrintTimeAndLog(0, "%s pServerNode == NULL:%s:%s", __FUNCTION__, szPrivateID.c_str(), szPublicIP.c_str() );
		return;
	}

	if( g_App.IsReserveLogOut() )
	{
		LOG.PrintTimeAndLog(0, "%s LogOut:%s:%s", __FUNCTION__,  szPrivateID.c_str(), szPublicIP.c_str() );
		SP2Packet kPacket( BSTPK_IPBONUS_RESULT );
		kPacket << szPrivateID;
		kPacket << "Dummy";
		kPacket << BILLING_IPBONUS_RESULT_FAIL;
		kPacket << false;
		pServerNode->SendMessage( kPacket );
		return;
	}

	if( g_App.IsTestMode() )
	{
		// 성공
		SP2Packet kPacket( BSTPK_IPBONUS_RESULT );
		kPacket << szPrivateID;
		kPacket << "Dummy";
		kPacket << BILLING_IPBONUS_RESULT_SUCCESS;
		kPacket << 1; // 0~5 : 0은 보너스 없다, 
		return;
	}

	ThailandUser *pUser = InsertUser();
	if( !pUser )
	{
		LOG.PrintTimeAndLog(0, "%s Fail InsertUse:%s:%s", __FUNCTION__,  szPrivateID.c_str(), szPublicIP.c_str() );
		SP2Packet kPacket( BSTPK_IPBONUS_RESULT );
		kPacket << szPrivateID;
		kPacket << "Dummy";
		kPacket << BILLING_IPBONUS_RESULT_FAIL;
		kPacket << false;
		pServerNode->SendMessage( kPacket );
		return;
	}

	ioHashString sPID;
	ioHashString szBillingGUID = "Dummy";
	ioHashString sEncodePW     = "";
	pUser->SetData( TIMEGETTIME(), szBillingGUID, szPrivateID, sEncodePW, szPublicIP, 0, pServerNode->GetIP(), pServerNode->GetClientPort() );
	SendIPBonus( pUser->GetPrivateID().c_str(), pUser->GetIP().c_str(), false, sPID );
	pUser->SetPID( sPID );

	LOG.PrintTimeAndLog(0, "%s Send:%s:%s:[%s]", __FUNCTION__, szPrivateID.c_str(), szPublicIP.c_str(), sPID.c_str() );
}

void ioLocalThailand::SendIPBonus( IN const char *szPrivateID, IN const char *szPublicIP, IN bool bCheckAlive, OUT ioHashString &rsPID )
{
	if( !szPrivateID || !szPublicIP )
	{
		LOG.PrintTimeAndLog( 0, "%s NULL", __FUNCTION__ );
		return;
	}

	// id|ip|sock_id;
	// THPP.playid001|203.144.226.26|G2A72P872GGPP7Z4;
	// THAS.asid001|203.144.226.26|5K89CA9PB574Z9X6;

	char szAgencyNo[ioLocalParent::MAX_AGENCY_NO_PLUS_ONE]="";
	GetAgencyNo( szAgencyNo, sizeof( szAgencyNo ), false );

	char szData[MAX_PATH*2]="";
	StringCbPrintf( szData, sizeof( szData ), "%s|%s|%s;", szPrivateID, szPublicIP, szAgencyNo );

	SP2Packet kPacket( BTTPK_IPBONUS_REQUEST );
	kPacket << szData;
	kPacket.SetBufferSizeMinusOne(); // 마지막 NULL 문자 제거
	g_ThailandIPBonusServer.SendMessage( kPacket );

	rsPID = szAgencyNo;

	if( bCheckAlive )
	{
		if( m_bIPBonusSendAlive )
		{
			LOG.PrintTimeAndLog( 0, "%s Server No Answer.", __FUNCTION__  );
			g_LogDBClient.OnInsertBillingServerError( CNT_WEMADEBUY, BILLING_ERROR_LOG_NO_ANSWER, "IPBonus server no answer." );
		}

		m_sIPBonusCheckAliveID = szAgencyNo;
		m_bIPBonusSendAlive    = true;
	}
}


void ioLocalThailand::OnRecieveIPBonus( const ioHashString &rsResult )
{
/*
    error_code|error_desc|Level|sock_id;

	0|Success|0|G2A72P872GGPP7Z4;
	0|Success|1|G2A72P872GGPP7Z4;
	0|Success|2|G2A72P872GGPP7Z4;
	0|Success|3|G2A72P872GGPP7Z4;
	0|Success|4|G2A72P872GGPP7Z4;
	0|Success|5|G2A72P872GGPP7Z4;
	-1001|Invalid request parameters|0|G2A72P872GGPP7Z4;
	-9010|Unexpected error|0|G2A72P872GGPP7Z4;
*/
	ioHashStringVec vParseStr;
	Help::GetParseData( rsResult, vParseStr, THAILAND_TOKEN );

	if( vParseStr.size() < MAX_IPBONUS_ARRAY )
	{
		LOG.PrintTimeAndLog( 0, "%s Size Error. %s", __FUNCTION__, rsResult.c_str() );
		return;
	}

	// 끝에 ; 제거 
	Help::DelEndCh( vParseStr[IPBONUS_ARRAY_PID] );

	if( m_sIPBonusCheckAliveID == vParseStr[IPBONUS_ARRAY_PID] )
	{
		m_bIPBonusSendAlive = false; // 초기화
		return;
	}

	ThailandUser *pUser = GetUser( vParseStr[IPBONUS_ARRAY_PID] );
	if( !pUser )
	{
		LOG.PrintTimeAndLog( 0, "%s pUser == NULL. %s", __FUNCTION__, rsResult.c_str() );
		return;
	}

	if( vParseStr[IPBONUS_ARRAY_CODE] != "0" )
	{
		SP2Packet kPacket( BSTPK_IPBONUS_RESULT );
		kPacket << pUser->GetPrivateID();
		kPacket << "Dummy";
		kPacket << BILLING_IPBONUS_RESULT_FAIL;
		if( !vParseStr[IPBONUS_ARRAY_DESC].IsEmpty() )
		{
			kPacket << true;
			kPacket << vParseStr[IPBONUS_ARRAY_DESC];
		}
		else
		{
			kPacket << false;
		}
		if( !g_ServerNodeManager.SendMessageIP( (ioHashString)pUser->GetServerIP(), pUser->GetServerPort(), kPacket ) )
		{	
			LOG.PrintTimeAndLog( 0, "%s Send Fail", __FUNCTION__ );
		}
		LOG.PrintTimeAndLog(0, "%s Return Error.:%s[%s]", __FUNCTION__, pUser->GetPrivateID().c_str(), rsResult.c_str() );
		RemoveUser( vParseStr[IPBONUS_ARRAY_PID]  );
		return;
	}

	// success
	SP2Packet kPacket( BSTPK_IPBONUS_RESULT );
	kPacket << pUser->GetPrivateID();
	kPacket << "Dummy";
	kPacket << BILLING_IPBONUS_RESULT_SUCCESS;
	kPacket << atoi( vParseStr[IPBONUS_ARRAY_LEVEL].c_str() );
	
	if( !g_ServerNodeManager.SendMessageIP( (ioHashString)pUser->GetServerIP(), pUser->GetServerPort(), kPacket ) )
	{	
		LOG.PrintTimeAndLog( 0, "%s Send Fail", __FUNCTION__ );
	}

	LOG.PrintTimeAndLog(0, "%s Success.:%s:[%s]", __FUNCTION__, pUser->GetPrivateID().c_str(), rsResult.c_str() );
	RemoveUser( vParseStr[IPBONUS_ARRAY_PID]  );
}

void ioLocalThailand::OnIPBonusOut( ServerNode *pServerNode, SP2Packet &rkPacket )
{
	ioHashString szPrivateID;
	ioHashString szPublicIP;
	rkPacket >> szPrivateID >> szPublicIP;

	if( !pServerNode )
	{
		LOG.PrintTimeAndLog(0, "%s pServerNode == NULL:%s:%s", __FUNCTION__, szPrivateID.c_str(), szPublicIP.c_str() );
		return;
	}

	ioHashString sPID;
	SendIPBonusOut( szPrivateID.c_str(), szPublicIP.c_str(), false, sPID );
	LOG.PrintTimeAndLog(0, "%s Send:%s:%s:[%s]", __FUNCTION__, szPrivateID.c_str(), szPublicIP.c_str(), sPID.c_str() );
}

void ioLocalThailand::SendIPBonusOut( IN const char *szPrivateID, IN const char *szPublicIP, IN bool bCheckAlive, OUT ioHashString &rsPID )
{
	if( !szPrivateID || !szPublicIP )
	{
		LOG.PrintTimeAndLog( 0, "%s NULL", __FUNCTION__ );
		return;
	}

	// id|ip|sock_id;
	// THPP.playid001|203.144.226.26|G2A72P872GGPP7Z4;
	// THAS.asid001|203.144.226.26|5K89CA9PB574Z9X6;

	char szAgencyNo[ioLocalParent::MAX_AGENCY_NO_PLUS_ONE]="";
	GetAgencyNo( szAgencyNo, sizeof( szAgencyNo ), false );

	char szData[MAX_PATH*2]="";
	StringCbPrintf( szData, sizeof( szData ), "%s|%s|%s;", szPrivateID, szPublicIP, szAgencyNo );

	SP2Packet kPacket( BTTPK_IPBONUS_OUT_REQUEST );
	kPacket << szData;
	kPacket.SetBufferSizeMinusOne(); // 마지막 NULL 문자 제거
	g_ThailandIPBonusOutServer.SendMessage( kPacket );

	rsPID = szAgencyNo;

	if( bCheckAlive )
	{
		if( m_bIPBonusOutSendAlive )
		{
			LOG.PrintTimeAndLog( 0, "%s Server No Answer.", __FUNCTION__  );
			g_LogDBClient.OnInsertBillingServerError( CNT_WEMADEBUY, BILLING_ERROR_LOG_NO_ANSWER, "IPBonus out server no answer." );
		}

		m_sIPBonusOutCheckAliveID = szAgencyNo;
		m_bIPBonusOutSendAlive    = true;
	}
}

void ioLocalThailand::OnRecieveIPBonusOut( const ioHashString &rsResult )
{
/*
    error_code|error_desc|Level|sock_id;

	0|Success|0|8ZAXE85HJMR9F938;
	-1001|Invalid request parameters|0|8ZAXE85HJMR9F938;
	-9009|DB error|0|8ZAXE85HJMR9F938;
	-9010|Unexpected error|0|8ZAXE85HJMR9F938;
*/
	ioHashStringVec vParseStr;
	Help::GetParseData( rsResult, vParseStr, THAILAND_TOKEN );

	if( vParseStr.size() < MAX_IPBONUS_OUT_ARRAY )
	{
		LOG.PrintTimeAndLog( 0, "%s Size Error. %s", __FUNCTION__, rsResult.c_str() );
		return;
	}

	// 끝에 ; 제거 
	Help::DelEndCh( vParseStr[IPBONUS_OUT_ARRAY_PID] );

	if( m_sIPBonusOutCheckAliveID == vParseStr[IPBONUS_OUT_ARRAY_PID] )
	{
		m_bIPBonusOutSendAlive = false; // 초기화
		return;
	}

	LOG.PrintTimeAndLog(0, "%s Success.[%s]", __FUNCTION__, rsResult.c_str() );
}


void ioLocalThailand::InitMemoryPool()
{
	// login
	m_LoginMgr.InitMemoryPool();

	// total
	ioINILoader kLoader( "ls_config_billingsvr.ini" );
	kLoader.SetTitle( "MemoryPool" );
	int iMax = kLoader.LoadInt( "ThailandUserPool", 100 );
	for(int i = 0;i < iMax * 2 ;i++) // kyg 맥스값 조정 
	{
		ThailandUser *pUser = new ThailandUser();
		if( !pUser )
			continue;
		m_UserMemNode.Push( pUser );
	}
}


void ioLocalThailand::ReleaseMemoryPool()
{
	// Total
	for(vThailandUser::iterator iter = m_vUser.begin(); iter != m_vUser.end(); ++iter)
	{
		ThailandUser *pUser = *iter;
		if( !pUser )
			continue;
		m_UserMemNode.Push( pUser );
	}
	m_vUser.clear();
	m_UserMemNode.DestroyPool();
}

ThailandUser *ioLocalThailand::GetUser( const ioHashString &rsPID )
{
	for(vThailandUser::iterator iter = m_vUser.begin(); iter != m_vUser.end(); ++iter)
	{
		ThailandUser *pUser = *iter;
		if( !pUser )
			continue;
		if( pUser->GetPID() != rsPID )
			continue;
		return pUser;
	}

	return NULL;
}

void ioLocalThailand::RemoveUser( const ioHashString &rsPID )
{
	vThailandUser::iterator iter = m_vUser.begin();
	while( iter != m_vUser.end() )
	{
		ThailandUser *pUser = *iter;
		if( !pUser )
		{
			iter++;
			continue;
		}
		if( pUser->GetPID() == rsPID )
		{
			pUser->Clear();
			iter = m_vUser.erase( iter );
			m_UserMemNode.Push( pUser );			
		}
		else
			iter++;
	}
}

ThailandUser *ioLocalThailand::InsertUser()
{
	ThailandUser *pUser = (ThailandUser*) m_UserMemNode.Pop();
	if( !pUser )
	{
		for(vThailandUser::iterator iter = m_vUser.begin(); iter != m_vUser.end(); ++iter)
		{
			ThailandUser *pUser = *iter;
			if( !pUser )
				continue;
			if( TIMEGETTIME() - pUser->GetCreateTime() < MAX_PACKET_ALIVE_TIME )
				continue;
			LOG.PrintTimeAndLog( 0, "%s Delete User %s:%d:%s", __FUNCTION__, pUser->GetPID().c_str(), pUser->GetCreateTime(), pUser->GetPrivateID().c_str() );
			m_vUser.erase( iter );
			m_UserMemNode.Push( pUser );
			break;
		}

		pUser = (ThailandUser*) m_UserMemNode.Pop();
	}

	if( !pUser )
	{
		return NULL;
	}

	m_vUser.push_back( pUser );
	return pUser;
}
