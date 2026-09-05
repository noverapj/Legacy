#include "../stdafx.h"
#include "./ioLocalEU.h"
#include "../MainProcess.h"
#include "../NodeInfo/ServerNode.h"
#include "../NodeInfo/ServerNodeManager.h"
#include "../DataBase/LogDBClient.h"
#include "../Channeling/ioChannelingNodeParent.h"
#include "../ThreadPool/ioThreadPool.h"
#include "../EtcHelpFunc.h"
#include "../NexonEUSessionServer/NexonEUSessionServer.h"
#include "../NexonNISMSServer/NexonNISMSServer.h"
#include "../NodeInfo/MemInfoManager.h"
#include "../NexonEUDefine.h"
#include "../Channeling/ioChannelingNodeParent.h"
#include "../xml/ioxmldocument.h"
#include "../xml/ioxmlelement.h"

extern CLog LOG;
ioLocalEU::ioLocalEU(void)
{
	m_dwReqKey		= 0;
	m_dwSSOReqKey	= 0;
	m_ProductListMap.clear();
	m_sBillingPixelURL.Clear();
}


ioLocalEU::~ioLocalEU(void)
{
}

ioLocalManager::LocalType ioLocalEU::GetType()
{
	return ioLocalManager::LCT_EU;
}

void ioLocalEU::Init()
{
	m_ProductListMap.clear();

	//ip, port, servicevalue -> 아직 모름 
	ioINILoader kLoader( "ls_config_billingsvr.ini" );
	kLoader.SetTitle( "EU" );

	char szEUPixelURL[MAX_PATH]="";
	kLoader.LoadString( "EUPixelURL", "", szEUPixelURL, sizeof( szEUPixelURL ) );
	m_sBillingPixelURL = szEUPixelURL;
}

//로그인 , 인증 작업은 추후에 다시 작업
void ioLocalEU::OnLoginData( ServerNode *pServerNode, SP2Packet &rkPacket )
{
	DWORD        dwReturnMsgType = 0;
	DWORD		 dwKey = 0;
	DWORD		 dwGameCode = 0;
	dwGameCode = 50360359;


	ioHashString szBillingGUID;
	ioHashString szPrivateID;
	ioHashString szPublicIP;
	ioHashString szUserType;
	ioHashString szNexonPassPort;
	ioHashString szUserHWID;
	
	szUserType.Clear();
	szBillingGUID.Clear();
	szPrivateID.Clear();
	szPublicIP.Clear();
	szUserType.Clear();
	szNexonPassPort.Clear();
	szUserHWID.Clear();
	
	dwKey = InterlockedIncrement((LONG *)&m_dwSSOReqKey);
	//rkPacket >> dwUserIndex >> szBillingGUID >> szPrivateID >> sEncodePW >> szPublicIP >> dwReturnMsgType; // 공통
    rkPacket >> szBillingGUID >> szPrivateID >> szNexonPassPort >> szPublicIP >> dwReturnMsgType; // 공통
	
	
	if( g_App.IsReserveLogOut() )
	{
		LOG.PrintTimeAndLog( 0, "%s LogOut: %s:%s", "ioLocalEU::OnLoginData", szBillingGUID.c_str(), szPrivateID.c_str() );
		SP2Packet kPacket( BSTPK_LOGIN_RESULT );
		kPacket << szPrivateID;
		kPacket << szBillingGUID;
		kPacket << BILLING_LOGIN_RESULT_FAIL;
		kPacket << false;
		pServerNode->SendMessage( kPacket );
		return;
	}
		
	BillInfoManager::BillingInfo *pInfo = g_BillInfoManager->PopBillInfo();
	if( !pInfo )
	{
		LOG.PrintTimeAndLog( 0, "%s LogOut: %s:%s", "ioLocalEU::OnLoginData", szBillingGUID.c_str(), szPrivateID.c_str() );
		SP2Packet kPacket( BSTPK_LOGIN_RESULT );
		kPacket << szPrivateID;
		kPacket << szBillingGUID;
		kPacket << BILLING_LOGIN_RESULT_FAIL;
		kPacket << false;
		pServerNode->SendMessage( kPacket );
		return;
	}
	pInfo->m_dwKey				= dwKey;			//id가 아닌 랜덤하게 사용할 수 있도록 시간값
	pInfo->m_dwCreateTime		= TIMEGETTIME();
	pInfo->m_eType				= BillInfoManager::AT_GET;
	
	pInfo->m_kPacket << dwKey << dwReturnMsgType << szBillingGUID << szPrivateID << szNexonPassPort << szUserHWID << szPublicIP << dwGameCode << pServerNode->GetIP() << pServerNode->GetClientPort();
	pInfo->m_kPacket.SetPosBegin();

	if( !g_BillInfoManager->Add( pInfo ) )
	{
		LOG.PrintTimeAndLog( 0, "%s BillingInfo is full.: %d:%s:%s:%d", "ioLocalEU::OnLoginData", dwKey, szBillingGUID.c_str(), szPrivateID.c_str(), GetLastError()  );
		SP2Packet kPacket( BSTPK_LOGIN_RESULT );
		kPacket << szPrivateID;
		kPacket << szBillingGUID;
		kPacket << BILLING_LOGIN_RESULT_FAIL;
		kPacket << false;
		pServerNode->SendMessage( kPacket );
		return;
	}

	EU_SESSION4_REQUEST sessionRequest;
	//szPublicIP = "222.110.172.248";
	if( g_App.IsEuTestMode() )
	{
		//SSO 
		szPublicIP =  g_App.GeEUTestPublicIP();
		LOG.PrintTimeAndLog( 0, "ioLocalEU::OnLoginData() EU Test MODE public ip : %s", szPublicIP.c_str() );
	}
	
	sessionRequest.SetInfo( dwKey, szNexonPassPort.c_str(), szPublicIP.c_str(), szUserHWID.c_str(), dwGameCode ); 
	
	SP2Packet kPacket;
	kPacket.SetPosBegin();
	kPacket.Write( sessionRequest );
	
	if( !g_NexonEUSessionServer.SendMessage( kPacket ) )
	{
		LOG.PrintTimeAndLog( 0, "%s Fail Send: %d:%s:%s:%d", "ioLocalEU::OnLoginData", dwKey, szBillingGUID.c_str(), szPrivateID.c_str(), GetLastError()  );
		SP2Packet kPacket( BSTPK_LOGIN_RESULT );
		kPacket << szPrivateID;
		kPacket << szBillingGUID;
		kPacket << BILLING_LOGIN_RESULT_FAIL;
		kPacket << false;
		pServerNode->SendMessage( kPacket );
		g_BillInfoManager->DeleteEUInfo( dwKey );
		return;
	}

	//LOG.PrintTimeAndLog( 0, "%s Success: %s, %s, %d, privateID %s ", "ioLocalEU::OnLoginData", szNexonPassPort.c_str(), szPublicIP.c_str(), dwGameCode, szPrivateID.c_str() );
}

void ioLocalEU::_OnGetCash( ServerNode *pServerNode, SP2Packet &rkPacket )
{
	int          iChannelingType   = 0;
	bool         bSetUserMouse     = false;
	DWORD		 dwKey			   = 0;
	DWORD        dwUserIndex       = 0;
	
	ioHashString szPrivateID;
	ioHashString szPublicID;
	ioHashString szBillingGUID;
	ioHashString szPublicIP;
	ioHashString szNexonID;

	szPrivateID.Clear();
	szPublicID.Clear();
	szBillingGUID.Clear();
	szPublicIP.Clear();
	szNexonID.Clear();
	
	//GetCash 시 nexonID 값 필요
	//rkPacket >> iChannelingType >> szBillingGUID >> dwUserIndex >> szPrivateID >> szPublicID >> bSetUserMouse // 공통 사항
	//		 >> szNexonID;

	

	rkPacket >> iChannelingType >> szBillingGUID >> dwUserIndex >> szPrivateID >> szPublicID >> bSetUserMouse; // 공통 사항
	//Test 용임
	szNexonID = szPrivateID;
	szPublicID = szPrivateID;
	
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
		return;
	}
	dwKey = InterlockedIncrement((LONG *)&m_dwReqKey);

	pInfo->m_dwKey				= dwKey;		//id가 아닌 시간값을 인덱스로 사용함
	pInfo->m_dwCreateTime		= TIMEGETTIME();
	pInfo->m_eType				= BillInfoManager::AT_GET;
	
	pInfo->m_kPacket << dwUserIndex << szBillingGUID << bSetUserMouse << pServerNode->GetIP() << pServerNode->GetClientPort();
	pInfo->m_kPacket.SetPosBegin();

	if( !g_BillInfoManager->Add( pInfo ) )
	{
		LOG.PrintTimeAndLog( 0, "%s BillingInfo is full.: %d:%s:%s:%s:%d",__FUNCTION__, dwUserIndex, szBillingGUID.c_str(), szPrivateID.c_str(), szPublicID.c_str(), GetLastError()  );
		SP2Packet kPacket( BSTPK_GET_CASH_RESULT );
		kPacket << dwUserIndex;
		kPacket << szBillingGUID;
		kPacket << bSetUserMouse;
		kPacket << CASH_RESULT_EXCEPT;
		pServerNode->SendMessage( kPacket );
		return;
	}


	EU_GETCASH cashInfo;
	cashInfo.SetInfo( NEXON_EU_BALANCE, szNexonID.c_str() );
	cashInfo.packetNo = dwKey;
	cashInfo.Htonl();
	
	SP2Packet kPacket;
	kPacket.SetPosBegin();
	kPacket.Write( cashInfo );

	if( !g_NexonNISMSServer.SendMessage( kPacket ) )
	{
		LOG.PrintTimeAndLog( 0, "%s Fail Send: %d:%s:%s:%s:%d",__FUNCTION__, dwUserIndex, szBillingGUID.c_str(), szPrivateID.c_str(), szPublicID.c_str(), GetLastError()  );
		SP2Packet kPacket( BSTPK_GET_CASH_RESULT );
		kPacket << dwUserIndex;
		kPacket << szBillingGUID;
		kPacket << bSetUserMouse;
		kPacket << CASH_RESULT_EXCEPT;
		pServerNode->SendMessage( kPacket );
		//g_BillInfoManager->Delete( szPrivateID );
		g_BillInfoManager->DeleteEUInfo( dwKey );
		return;
	}
}
	
void ioLocalEU::_OnOutputCash( ServerNode *pServerNode, SP2Packet &rkPacket, DWORD dwGoodsNo, const ioHashString &rszGoodsName )
{
	int          iChannelingType = 0;
	DWORD		 userAge		= 0;
	DWORD        dwUserIndex	= 0;
	DWORD		 dwNexonOID		= 0;
	DWORD		 dwPaymentType	= 0;
	DWORD		 dwKey			= 0;
	int          iPayAmt		= 0;
	int          iType			= 0;
	int			 iBalance		= 0;
	int          iGameServerPort = 0;
	
	ioHashString szBillingUserKey;
	ioHashString szBillingGUID;
	ioHashString szPublicID;
	ioHashString szPrivateID;
	ioHashString szUserIP;
	ioHashString szNexonID;
	ioHashString szUserName;
	
	szBillingGUID.Clear();
	szPublicID.Clear();
	szPrivateID.Clear();
	szUserIP.Clear();
	szNexonID.Clear();
	szUserName.Clear();

	rkPacket >> iChannelingType >> szBillingGUID >> dwUserIndex >> szPublicID >> szPrivateID >> szUserIP >> iPayAmt >> iType; // 공통사항

	if( g_App.IsReserveLogOut() )
	{
		LOG.PrintTimeAndLog( 0, "%s LogOut: %d:%s:%s:%s",__FUNCTION__, dwUserIndex, szBillingGUID.c_str(), szPrivateID.c_str(), szPublicID.c_str() );
		SP2Packet kPacket( BSTPK_OUTPUT_CASH_RESULT  );
		kPacket << dwUserIndex;
		kPacket << szBillingGUID;
		kPacket << CASH_RESULT_EXCEPT;
		kPacket << iType;
		pServerNode->SendMessage( kPacket );
		return;
	}
	ioChannelingNodeParent::ItemInfo kBuyInfo;
	ioChannelingNodeParent::GetItemInfo( rkPacket, iType, kBuyInfo );

	rkPacket >> iGameServerPort;  // us
	rkPacket >> szBillingUserKey; // us
	rkPacket >> szNexonID;
	rkPacket >> userAge;

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
		return;
	}
	//여기서는 Key 값을 BillingGUID 값으로 잡아야 할듯..
	
	dwKey = InterlockedIncrement((LONG *)&m_dwReqKey);

	pInfo->m_dwKey				= dwKey;		//id가 아닌 시간값을 키로 사용함
	//pInfo->m_szKey			= szBillingGUID;
	pInfo->m_eType				= BillInfoManager::AT_OUTPUT;
	pInfo->m_dwCreateTime		= TIMEGETTIME();
	pInfo->m_kPacket << dwUserIndex << szBillingGUID << iType << iPayAmt << iChannelingType;
	pInfo->m_kPacket << pServerNode->GetIP() << pServerNode->GetClientPort();
	ioChannelingNodeParent::SetItemInfo( pInfo->m_kPacket, iType, kBuyInfo );
	pInfo->m_kPacket.SetPosBegin();


	if( !g_BillInfoManager->Add( pInfo ))
	{
		LOG.PrintTimeAndLog( 0, "%s BillingInfo is full.: %d:%s:%s:%s:%d",__FUNCTION__, dwUserIndex, szBillingGUID.c_str(), szPrivateID.c_str(), szPublicID.c_str(), GetLastError()  );
		SP2Packet kPacket( BSTPK_OUTPUT_CASH_RESULT );
		kPacket << dwUserIndex;
		kPacket << szBillingGUID;
		kPacket << CASH_RESULT_EXCEPT;
		kPacket << iType;
		pServerNode->SendMessage( kPacket );
		return;
	}
	
	
	DWORD dwNismsGoodsNo = 0;
	DWORD dwNismsGoodsPrice = 0;

	dwNismsGoodsNo		= GetGoodsNo( dwGoodsNo );
	dwNismsGoodsPrice	= GetGoodsPrice( dwGoodsNo );
	
	if( dwNismsGoodsPrice != iPayAmt )
	{
		LOG.PrintTimeAndLog( 0, "%s item Price Error: NISMSGoodsNo : %d,userIndex :%d, BillingGUID :%s, privateID :%s, publicID :%s, NISMS price : %d, losa price : %d",__FUNCTION__, 
															dwNismsGoodsNo, dwUserIndex, szBillingGUID.c_str(), szPrivateID.c_str(), szPublicID.c_str(), dwNismsGoodsPrice, iPayAmt );
		SP2Packet kPacket( BSTPK_OUTPUT_CASH_RESULT );
		kPacket << dwUserIndex;
		kPacket << szBillingGUID;
		kPacket << CASH_RESULT_EXCEPT;
		kPacket << iType;
		pServerNode->SendMessage( kPacket );
		g_BillInfoManager->DeleteEUInfo( dwKey );
		return;
	}

	EU_PURCHASEITEM purchaseItem;
	purchaseItem.SetInfo( inet_addr( szUserIP.c_str() ), 1, szPrivateID.c_str(), szPrivateID.c_str(), dwKey, szPrivateID.c_str(), userAge, szBillingGUID.c_str(), 13001, iPayAmt, 1, dwNismsGoodsNo, 1 ); 
	purchaseItem.packetNo = dwKey;
	purchaseItem.Htonl();

	SP2Packet kPacket;
	kPacket.SetPosBegin();
	kPacket.Write( purchaseItem );
	
	if( !g_NexonNISMSServer.SendMessage( kPacket ) )
	{
		LOG.PrintTimeAndLog( 0, "%s Fail Send: %d:%s:%s:%s:%d",__FUNCTION__, dwUserIndex, szBillingGUID.c_str(), szPrivateID.c_str(), szPublicID.c_str(), GetLastError()  );
		SP2Packet kPacket( BSTPK_OUTPUT_CASH_RESULT );
		kPacket << dwUserIndex;
		kPacket << szBillingGUID;
		kPacket << CASH_RESULT_EXCEPT;
		kPacket << iType;
		pServerNode->SendMessage( kPacket );
		g_BillInfoManager->DeleteEUInfo( dwKey );
		return;
	}
	LOG.PrintTimeAndLog( 0, "%s Send: %d, orderID:%s:%s:%s:packetNo:%d,Age:%d,Amount:%d, Type(GoodsNo): %d, Type(NismsNo): %d, NISMS Price : %d, Price : %d, GameID : %s, NexonID : %s",
		__FUNCTION__, dwUserIndex, szBillingGUID.c_str(), szPrivateID.c_str(), szPublicID.c_str(), dwKey, userAge, iPayAmt, dwGoodsNo, dwNismsGoodsNo, dwNismsGoodsPrice, iPayAmt, szPrivateID.c_str(), szPrivateID.c_str() );
}

//테스트 용으로 사용하는 거니까 삭제해도 될듯
void ioLocalEU::ThreadLogin( const ioData &rData, LoginManager &rLoginMgr )
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


	
	
	if( g_App.IsTestMode())
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
		char szReturnValue[MAX_PATH*2]="";
		StringCbPrintf( szReturnValue, WEB_BUFF_SIZE,  "{\"result\" : 0, \"msg\" : \"Login successful\" , \"userid\":%d, \"username\":\"teset@test.com\", \"loginToken\":\"ABCDE\" }", iUserID );
	

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

		
		SP2Packet kPacket( rData.GetReturnMsgType() );
		kPacket << rData.GetPrivateID();
		kPacket << rData.GetBillingGUID();
		kPacket << BILLING_LOGIN_RESULT_SUCCESS;
		kPacket << szInsertValue;
		g_ServerNodeManager.SendMessageIP( (ioHashString) rData.GetServerIP(),  rData.GetServerPort(), kPacket );

		LOG.PrintTimeAndLog( 0, "%s %s [%s %s %s %s ]", __FUNCTION__, rData.GetPrivateID().c_str(), vValueVec[0].c_str(), vValueVec[1].c_str(), vValueVec[2].c_str(), szInsertValue );
	}
}

void ioLocalEU::OnReceiveGetCash( const EU_GETCASH_RESPONSE& rkResult ) 
{
	BillInfoManager::BillingInfo *pInfo = g_BillInfoManager->GetEUInfo( rkResult.packetNo );
	if( !pInfo )
	{
		LOG.PrintTimeAndLog( 0, "%s pInfo == NULL. %d:%d:%d:%d", __FUNCTION__, rkResult.packetNo, rkResult.result, rkResult.balance, rkResult.notRefundableBalance );
		return;
	}

	DWORD        dwUserIndex   = 0;
	ioHashString szBillingGUID;
	bool         bSetUserMouse = false;
	ioHashString szServerIP;
	int          iClientPort   = 0;
	pInfo->m_kPacket >> dwUserIndex >> szBillingGUID >> bSetUserMouse >> szServerIP >> iClientPort;

	if( rkResult.result != CASH_RESULT_SUCCESS )
	{
		SP2Packet kPacket( BSTPK_GET_CASH_RESULT );
		kPacket << dwUserIndex;
		kPacket << szBillingGUID;
		kPacket << bSetUserMouse;
		kPacket << CASH_RESULT_EXCEPT;
		
		if( !g_ServerNodeManager.SendMessageIP( szServerIP, iClientPort, kPacket ) )
		{
			LOG.PrintTimeAndLog( 0, "%s Send Fail(1): %d:%s: Ret %d:%d:%d", __FUNCTION__, dwUserIndex, szBillingGUID.c_str(), rkResult.result, rkResult.balance, rkResult.notRefundableBalance );
			g_BillInfoManager->DeleteEUInfo( rkResult.packetNo );
			return;
		}
		LOG.PrintTimeAndLog( 0, "%s Fail: %d:%s: Ret %d:%d:%d", __FUNCTION__, dwUserIndex, szBillingGUID.c_str(), rkResult.result, rkResult.balance, rkResult.notRefundableBalance );
		g_BillInfoManager->DeleteEUInfo( rkResult.packetNo  );
		return;
	}

	int iReturnCash    =  rkResult.balance + rkResult.notRefundableBalance ;
	
	SP2Packet kPacket( BSTPK_GET_CASH_RESULT );
	kPacket << dwUserIndex;
	kPacket << szBillingGUID;
	kPacket << bSetUserMouse;
	kPacket << CASH_RESULT_SUCCESS;
	kPacket << rkResult.balance;
	kPacket << rkResult.balance;
	if( !g_ServerNodeManager.SendMessageIP( szServerIP, iClientPort, kPacket ) )
	{
		LOG.PrintTimeAndLog( 0, "%s Send Fail(2): %d:%s: Ret %d:%d:%d", __FUNCTION__, dwUserIndex, szBillingGUID.c_str(), rkResult.result, rkResult.balance, rkResult.notRefundableBalance );
		g_BillInfoManager->DeleteEUInfo( rkResult.packetNo );
		return;
	}
	
	g_BillInfoManager->DeleteEUInfo( rkResult.packetNo );
	LOG.PrintTimeAndLog( 0, "%s Success: packetNo :%d, %d:%s: Ret %d:%d:%d", __FUNCTION__, rkResult.packetNo, dwUserIndex, szBillingGUID.c_str(), rkResult.result, rkResult.balance, rkResult.notRefundableBalance );

}

void ioLocalEU::OnRecieveLoginData( const EU_SESSION4_REPLY &rkResult )
{
	BillInfoManager::BillingInfo *pInfo = g_BillInfoManager->GetEUInfo( rkResult.serial );	//여기에 인덱스 번호 넣어줘서 보냈음
	if( !pInfo )
	{
		LOG.PrintTimeAndLog( 0, "%s pInfo == NULL. %d:%d:%s", "ioLocalEU::OnRecieveLoginData", rkResult.serial, rkResult.resultCode, rkResult.nexonID.c_str() );
		return;
	}
	
	int          iClientPort     = 0;
	DWORD		 dwGameCode		 = 0;
	DWORD		 dwUserIndex	 = 0;
	DWORD        dwReturnMsgType = 0;
	ioHashString szUserType;
	ioHashString szBillingGUID;
	ioHashString szPrivateID;
	ioHashString szServerIP;
	ioHashString szEncodePW;
	ioHashString szPublicIP;
	ioHashString szNexonPassPort;
	ioHashString szUserHWID;
	
	szUserType.Clear();
	szBillingGUID.Clear();
	szPrivateID.Clear();
	szServerIP.Clear();
	szEncodePW.Clear();
	szPublicIP.Clear();
	szNexonPassPort.Clear();
	szUserHWID.Clear();
	

	pInfo->m_kPacket >> dwUserIndex >> dwReturnMsgType >> szBillingGUID >> szPrivateID  >> szNexonPassPort >> szUserHWID >> szPublicIP >> dwGameCode >> szServerIP >> iClientPort;
	pInfo->m_kPacket.SetPosBegin();

	int iReturn = 0;
	
	if( rkResult.resultCode == 0 )
	{
		iReturn = BILLING_LOGIN_RESULT_SUCCESS;
	}
	else
	{
		iReturn = rkResult.resultCode;
	}
	
	SP2Packet kPacket( dwReturnMsgType );
	kPacket << szPrivateID;
	kPacket << szBillingGUID;
	kPacket << iReturn;
	kPacket << szPrivateID;
	//EU 만 추가로 더보냄
	kPacket << rkResult.nexonID; 
	kPacket << rkResult.nexonSN;
	kPacket << rkResult.gender;
	kPacket << rkResult.age;
	kPacket << rkResult.nationCode;
	
	
	if( !g_ServerNodeManager.SendMessageIP( szServerIP, iClientPort, kPacket ) )
	{
		LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s: Ret %d:%s", "ioLocalEU::OnRecieveLoginData", dwUserIndex, szBillingGUID.c_str(), rkResult.resultCode, rkResult.nexonID.c_str() );
		g_BillInfoManager->DeleteEUInfo( rkResult.serial );
		return;
	}
	
	g_BillInfoManager->DeleteEUInfo( rkResult.serial );
	
	LOG.PrintTimeAndLog( 0, "%s Success: %d:%s: packetNo : %d, Ret %d, nexonID : %s nexonSN : %I64d, gender: %d, age:%d, nationCode:%s, clientIP:%s ResultCode : %d, %d", "ioLocalEU::OnRecieveLoginData"
		, dwUserIndex, szBillingGUID.c_str(), rkResult.serial, rkResult.resultCode, rkResult.nexonID.c_str()
		,rkResult.nexonSN, rkResult.gender, rkResult.age, rkResult.nationCode.c_str(), rkResult.clientIP.c_str(), iReturn, rkResult.resultCode );


}

void ioLocalEU::OnRecieveOutputCash( const EU_PURCAHSEITEM_RESPONSE &rkResult ) 
{
	BillInfoManager::BillingInfo *pInfo = g_BillInfoManager->GetEUInfo( rkResult.packetNo );
	if( !pInfo )
	{
		LOG.PrintTimeAndLog( 0, "%s pInfo == NULL. %d:%d:%d:%s", __FUNCTION__, rkResult.packetNo, rkResult.result, rkResult.orderNo, rkResult.orderID.c_str() );
		return;
	}
  
	DWORD		 dwNexonOID	   = 0;
	DWORD        dwUserIndex   = 0;
	int          iType         = 0;
	int          iPayAmt       = 0;
	int          iChannelingType = 0;
	int          iClientPort   = 0;
	ioHashString szBillingGUID;
	ioHashString szServerIP;
	ioHashString szNexonID;
	ioHashString szUserName;
	ioHashString szChargeNo;
	
	pInfo->m_kPacket >> dwUserIndex >> szBillingGUID >> iType >> iPayAmt >> iChannelingType >> szServerIP >> iClientPort;
	
	
	int iReturn = CASH_RESULT_SUCCESS;
	if( rkResult.result != CASH_ACTION_RESULT_SUCCESS )
		iReturn = CASH_RESULT_EXCEPT;

	/*if( g_App.IsEuTestMode() )
	{
		iReturn = CASH_RESULT_SUCCESS;
		LOG.PrintTimeAndLog( 0, "ioLocalEU::OnRecieveOutputCash() EU Test MODE ret : %d", iReturn);
	}*/

	// 성공 일 경우 구매 시 차감액을 조회함
	// return value
	if( iReturn == CASH_RESULT_SUCCESS )
	{	
		int tempBalance = 0;
		
		SP2Packet kPacket( BSTPK_OUTPUT_CASH_RESULT );
		kPacket << dwUserIndex;
		kPacket << szBillingGUID;
		kPacket << iReturn;
		kPacket << iType;

		// return value
		if( iReturn == CASH_RESULT_SUCCESS )
		{
			kPacket << iPayAmt;
			kPacket << 0; // TransactionID ( FOR US )
			
			ioChannelingNodeParent::ItemInfo kItemInfo;
			ioChannelingNodeParent::GetItemInfo( pInfo->m_kPacket, iType, kItemInfo );
			ioChannelingNodeParent::SetItemInfo( kPacket, iType, kItemInfo );
			
			
			kPacket << 0;  // 공통
			kPacket << tempBalance;
			kPacket << tempBalance;
		}
		if( !g_ServerNodeManager.SendMessageIP( szServerIP, iClientPort, kPacket ) )
		{
			LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:Ret %d", __FUNCTION__, dwUserIndex, szBillingGUID.c_str(), rkResult.result );
			g_BillInfoManager->DeleteEUInfo( rkResult.packetNo  );
			return;
		}
	}
	else
	{
		SP2Packet kPacket( BSTPK_OUTPUT_CASH_RESULT );
		kPacket << dwUserIndex;
		kPacket << szBillingGUID;
		kPacket << iReturn;
		kPacket << iType;
		if( !g_ServerNodeManager.SendMessageIP( szServerIP, iClientPort, kPacket ) )
		{
			LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s:Ret %d", __FUNCTION__, dwUserIndex, szBillingGUID.c_str(), rkResult.result );
			g_BillInfoManager->DeleteEUInfo( rkResult.packetNo  );
			return;
		}
	}
	
	if( iReturn == CASH_RESULT_SUCCESS )
		LOG.PrintTimeAndLog( 0, "%s Success: %d:%s Ret %d", __FUNCTION__, dwUserIndex, szBillingGUID.c_str(), rkResult.result );
	
	else
	{
		LOG.PrintTimeAndLog( 0, "%s CASH RESULT Fail: packetNO : %d, %d, orderID:%s:Ret:%d,Type:%d,orderNO:%d", 
			__FUNCTION__, rkResult.packetNo, dwUserIndex, rkResult.orderID.c_str(), rkResult.result, rkResult.productNo, rkResult.orderNo );
	}

	g_BillInfoManager->DeleteEUInfo( rkResult.packetNo  );
	
}

//이부분은 좀 더 고려 해봐야 할듯..
void ioLocalEU::OnRecieveOutputCashAmount( const  EU_GETCASH_AMOUNT_RESPONSE &rkResult )
{
	BillInfoManager::BillingInfo *pInfo = g_BillInfoManager->GetEUInfo( rkResult.packetNo );
	if( !pInfo )
	{
		LOG.PrintTimeAndLog( 0, "%s pInfo == NULL. %d:%d", __FUNCTION__, rkResult.packetNo, rkResult.result );
		return;
	}
  
	DWORD        dwUserIndex   = 0;
	DWORD        dwNexonOID = 0;
	int          iType         = 0;
	int          iPayAmt       = 0;
	int          iChannelingType = 0;
	int          iClientPort   = 0;
	ioHashString szBillingGUID;
	ioHashString szServerIP;
	ioHashString szNexonID;
	ioHashString szUserName;
	ioHashString szChargeNo;
	
	//pInfo->m_kPacket >> dwUserIndex >> szBillingGUID >> iType >> iPayAmt >> iChannelingType >> szServerIP >> iClientPort;
	pInfo->m_kPacket >> dwUserIndex >> szBillingGUID >> iType >> iPayAmt >> iChannelingType >> szServerIP >> iClientPort >> szNexonID >> szUserName >> dwNexonOID;

	int iReturn = CASH_RESULT_SUCCESS;
	if( rkResult.result != CASH_ACTION_RESULT_SUCCESS )
		iReturn = CASH_RESULT_EXCEPT;

	SP2Packet kPacket( BSTPK_OUTPUT_CASH_RESULT );
	kPacket << dwUserIndex;
	kPacket << szBillingGUID;
	kPacket << iReturn;
	kPacket << iType;

	kPacket << iPayAmt;
	kPacket << 0; // TransactionID ( FOR US )
	kPacket << szChargeNo;
 
	if( iReturn == CASH_RESULT_SUCCESS )
	{
	
		ioChannelingNodeParent::ItemInfo kItemInfo;
		ioChannelingNodeParent::GetItemInfo( pInfo->m_kPacket, iType, kItemInfo );
		ioChannelingNodeParent::SetItemInfo( kPacket, iType, kItemInfo );
		
		DWORD balance = 0;
		balance = rkResult.balance;


		kPacket << iChannelingType;  
		kPacket << balance;
		kPacket << balance;
	}


	if( !g_ServerNodeManager.SendMessageIP( szServerIP, iClientPort, kPacket ) )
	{
		LOG.PrintTimeAndLog( 0, "%s Send Fail: %d:%s Ret %d:%d", __FUNCTION__, dwUserIndex, szBillingGUID.c_str(), rkResult.result, rkResult.result );
		g_BillInfoManager->DeleteEUInfo( rkResult.packetNo );
		return;
	}
	g_BillInfoManager->DeleteEUInfo( rkResult.packetNo );
	LOG.PrintTimeAndLog( 0, "%s Success: %d:%s Ret %d:%d", __FUNCTION__, dwUserIndex, szBillingGUID.c_str(), rkResult.result, rkResult.result );


}

DWORD ioLocalEU::GetGoodsNo( DWORD dwGoodsNo )
{
	DWORD dwNismsGoodsNo = 0;
	ProductListMap::const_iterator iter = m_ProductListMap.find( dwGoodsNo );

	if( iter != m_ProductListMap.end() )
	{
		
		dwNismsGoodsNo = iter->second.goodsNo;
		
	}
	else
	{
		LOG.PrintTimeAndLog(0, "ioLocalEU::GetGoodsNo Not Exist : %d", dwGoodsNo );
	}

	return dwNismsGoodsNo;
}

DWORD ioLocalEU::GetGoodsPrice( DWORD dwGoodsNo )
{
	DWORD dwNismsGoodsPrice = 0;
	ProductListMap::const_iterator iter = m_ProductListMap.find( dwGoodsNo );

	if( iter != m_ProductListMap.end() )
	{

		dwNismsGoodsPrice = iter->second.salesPrice;
	}
	else
	{
		LOG.PrintTimeAndLog(0, "ioLocalEU::GetGoodsPrice Not Exist : %d", dwGoodsNo );
	}
	return dwNismsGoodsPrice;
}

void ioLocalEU::OnRecieveProductList( const EU_PRODUCT_INQUIRY_RESPONSE &rkResult )
{
	if( g_App.IsReserveLogOut() )
	{
		LOG.PrintTimeAndLog( 0, "%s LogOut",__FUNCTION__ );
		return;
	}

	LOG.PrintTimeAndLog( 0, "ioLocalEU::OnRecieveProductList TotalProduct Count : %d, Remain Product Count : %d", rkResult.totalProductCount, rkResult.remainProductCount );

	
	ioXMLDocument xmlDoc;
	if( !xmlDoc.LoadFromMemory( rkResult.resultXML.c_str() ) )
	{
		LOG.PrintTimeAndLog( 0, "%s Load Fail", "ioLocalEU::OnRecieveProductList");
		return;
	}

	ioXMLElement xRootElement = xmlDoc.GetRootElement();
	if( xRootElement.IsEmpty() )	
	{
		LOG.PrintTimeAndLog( 0, "%s RootElement Fail", "ioLocalEU::OnRecieveProductList" );
		return;
	}
	
	
	enum { MAX_ENTRY = 40, };
	
	ioXMLElement xGrandChildElement;
	ioXMLElement xGreatGrandChildElement;
	
	DWORD			dwGoodsNo = 0, dwNismsGoodsNo = 0, dwSalesPrice;
	ioHashString	szTagName, szGoodsName;

	ioXMLElement xChildElement = xRootElement.FirstChild( "Table" );
	for (DWORD i = 0; i < rkResult.totalProductCount ; i++)
	{
		if( xChildElement.IsEmpty() )
			break;
		
		szTagName = xChildElement.GetTagName();
		if( szTagName == "Table" )

		if( !xChildElement.IsEmpty() )
			xGrandChildElement = xChildElement.FirstChild( "product_no" );
			dwNismsGoodsNo = atoi( xGrandChildElement.GetText() );

		if( !xChildElement.IsEmpty() )
			xGrandChildElement = xChildElement.FirstChild( "item_no" );
			dwGoodsNo = atoi( xGrandChildElement.GetText() );

		if( !xChildElement.IsEmpty() )
			xGrandChildElement = xChildElement.FirstChild( "product_name" );
			 szGoodsName = xGrandChildElement.GetText();

		if( !xChildElement.IsEmpty() )
			xGrandChildElement = xChildElement.FirstChild( "sale_price" );
		dwSalesPrice = atoi( xGrandChildElement.GetText() );

		LOG.PrintTimeAndLog( 0, "ioLocalEU::OnRecieveProductList NAME : %s, NISMS No : %d, GOODS No: %d, price : %d",szGoodsName.c_str(),  dwNismsGoodsNo, dwGoodsNo, dwSalesPrice );

		NISMS_LIST nismsList;
		nismsList.SetData( dwNismsGoodsNo, dwSalesPrice );
		
		m_ProductListMap.insert( ProductListMap::value_type( dwGoodsNo, nismsList) );	

		xChildElement = xChildElement.NextSibling();
	}
}

void ioLocalEU::OnEUNexonPixel( ServerNode *pServerNode, SP2Packet &rkPacket )
{
	if( g_App.IsReserveLogOut() )
	{
		LOG.PrintTimeAndLog( 0, "%s LogOut",__FUNCTION__ );
		return;
	}
	DWORD		 userIndex;
	INT64		 nexonSN;
	
	ioHashString szPrivateID;
	
	userIndex	= 0;
	nexonSN		= 0;
	szPrivateID.Clear();
	
	rkPacket >> userIndex >> nexonSN; // 공통

	char szReturnValue[WEB_BUFF_SIZE]="";

	ioHTTP Winhttp; 

	char szFullURL[MAX_PATH*2]="";
	//http://network.adsmarket.com/cpx?script=1&programid=133215&action=lead&p1=[YOUR_UNIQUE_VALUE_HERE]
	StringCbPrintf( szFullURL, sizeof( szFullURL ), "%s=%I64d" , m_sBillingPixelURL.c_str(), nexonSN );

	LOG.PrintTimeAndLog( 0, "ioLocalEU::OnEUNexonPixel url : %s", szFullURL );
	if( !Winhttp.GetResultData( szFullURL, "GET", szReturnValue, WEB_BUFF_SIZE ) )
	{
		LOG.PrintTimeAndLog( 0, "%s Http Error", "ioLocalEU::OnEUNexonPixel" );
		return;
	}
	
	LOG.PrintTimeAndLog( 0, "ioLocalEU::OnEUNexonPixel Send : userIndex : %d, nexonSN : %I64d", userIndex, nexonSN );

}