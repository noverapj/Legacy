#include "stdafx.h"
#include "ioLocalLatin.h"
#include "../MainProcess.h"
#include "../NodeInfo/User.h"
#include "../BillingRelayServer/BillingRelayServer.h"
#include "../EtcHelpFunc.h"
#include "../MainProcess.h"
#include "../Channeling/ioChannelingNodeParent.h"
#include "../NodeInfo/UserNodeManager.h"
#include "../MainServerNode/MainServerNode.h"

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

const char * ioLocalLatin::GetTextListFileName()
{
	return "text_latin.txt";
}

bool ioLocalLatin::ParseLoginData( IN ioHashString &rsEncLoginKeyAndID, OUT char *szEncLoginKey, IN int iEncLoginKeySize, OUT char *szPrivateID, IN int iPrivaiteIDSize )
{
	//hr 파싱하는거 추가
	enum 
	{ 
		MAX_TOKEN   = 3, 
		ID_ARRAY    = 0,
		TOKEN_ARRAY = 1,
		NAME_ARRAY  = 2,
	};

	char szTemp[MAX_PATH*2]="";
	StringCbCopy( szTemp, sizeof( szTemp ), rsEncLoginKeyAndID.c_str() );

	ioHashString sUserName;

	char* next_token = NULL;
	for (int i = 0; i < MAX_TOKEN ; i++)
	{
		char *pPos = NULL;
		if( i == 0 )
			pPos = strtok_s( szTemp, EU_TOKEN, &next_token );
		else
			pPos = strtok_s( NULL, EU_TOKEN, &next_token );

		if( pPos == NULL )
			break;

		if( i == ID_ARRAY )
		{
			size_t decryptIDLength = strlen( pPos );
			if ( decryptIDLength <= iPrivaiteIDSize )
				memcpy( szPrivateID, pPos, decryptIDLength );
				//szPrivateID  = pPos;
		}
		else if( i == TOKEN_ARRAY )
			szEncLoginKey = pPos;
		else if( i == NAME_ARRAY )
			sUserName   = pPos;

		StringCbCopy( szPrivateID, iPrivaiteIDSize, pPos );
		
	}
	
	return true;
}

// 미국에서는 빌링서버를 통해서 로그인 인증을 하므로 무조건 true로 셋팅 
bool ioLocalLatin::DecryptLoginKey( IN const char *szEncLoginKey, IN const char *szUserKey, OUT char *szDecLoginKey, IN int iDecLoginKeySize )
{
	return true;
}

bool ioLocalLatin::IsRightLoginKey( const char *szDBKey, const char *szDecryptKey )
{
	return true;
}

bool ioLocalLatin::IsRightTimeLoginKey( DWORD dwTotalMins )
{
	return true;
}

void ioLocalLatin::ApplyConnect( IN User *pUser, IN SP2Packet &rkPacket )
{
	if( !pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pUser == NULL.", __FUNCTION__ );
		return;
	}
	ioHashString sEndPW;
	rkPacket >> sEndPW;

	// encode pw 값 
	pUser->SetBillingUserKey( sEndPW );
}

bool ioLocalLatin::ApplyLogin( IN User *pUser, IN SP2Packet &rkPacket )
{
	if( !pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pUser == NULL.", __FUNCTION__ );
		return true;
	}

	enum 
	{ 
		MAX_TOKEN   = 3, 
		ID_ARRAY    = 0,
		TOKEN_ARRAY = 1,
		NAME_ARRAY  = 2,
	};

	ioHashString sReturnValue;
	ioHashString sCountry;
	ioHashString sAuth;
	ioHashString sGender;
	ioHashString sCafe;
	ioHashString sAge;

	rkPacket >> sReturnValue >> sCountry >> sAuth >> sGender >> sCafe >> sAge;

	char szTemp[MAX_PATH*2]="";
	StringCbCopy( szTemp, sizeof( szTemp ), sReturnValue.c_str() );

	ioHashString sPrivateID;
	ioHashString sLoginToken;
	ioHashString sUserName;

	
	char* next_token = NULL;
	for (int i = 0; i < MAX_TOKEN ; i++)
	{
		char *pPos = NULL;
		if( i == 0 )
			pPos = strtok_s( szTemp, EU_TOKEN, &next_token );
		else
			pPos = strtok_s( NULL, EU_TOKEN, &next_token );

		if( pPos == NULL )
			break;

		if( i == ID_ARRAY )
			sPrivateID  = pPos;
		else if( i == TOKEN_ARRAY )
			sLoginToken = pPos;
		else if( i == NAME_ARRAY )
			sUserName   = pPos;
	}
	
	/*
	if( g_UserNodeManager.IsConnectUser( sPrivateID ) )   // hr 
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Duplication: %s:%s", __FUNCTION__, sPrivateID.c_str(), pUser->GetBillingGUID().c_str() );
		SP2Packet kPacket( STPK_CONNECT );
		kPacket << CONNECT_ID_ALREADY << sPrivateID;
		pUser->SendMessage( kPacket );
		return false;
	}
	

	if( !sLoginToken.IsEmpty() )
	{
		SP2Packet kPacket( STPK_LOGIN_TOKEN );
		kPacket << sLoginToken;
		pUser->SendMessage( kPacket );
	}
	
	if( !sUserName.IsEmpty() )
	{
		pUser->SetBillingUserKey( sUserName ); // 새로운 Wemade USA의 UserName을 셋팅 한다.
	}
	*/
	// real private id로 교체
	
	pUser->SetPrivateID( sPrivateID );
	pUser->SetDBAgentID( Help::GetUserDBAgentID( pUser->GetPrivateID() ) );

	pUser->SetCountry( sCountry );
	pUser->SetGender( sGender );
	pUser->SetCafeLevel( sCafe );
	pUser->SetAge( sAge );

	return true;
}

void ioLocalLatin::ApplyRequestLogin( IN SP2Packet &rkPacket, OUT ioHashString &rsValue1, OUT ioHashString &rsValue2 )
{
	rkPacket >> rsValue1; // usertype
}

bool ioLocalLatin::SendLoginData( User *pUser )
{
	if( !pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pUser == NULL.", __FUNCTION__ );
		return true;
	}

	char szTempGUID[USER_GUID_NUM_PLUS_ONE]="";
	Help::GetGUID( szTempGUID, sizeof(szTempGUID) );
	pUser->SetBillingGUID( szTempGUID );

	SP2Packet kBillingPacket( BSTPK_LOGIN );
	kBillingPacket << pUser->GetBillingGUID();
	kBillingPacket << pUser->GetPrivateID();
	kBillingPacket << pUser->GetBillingUserKey();
	kBillingPacket << pUser->GetPublicIP();
	kBillingPacket << BSTPK_LOGIN_RESULT; // return msg type

	if( !g_BillingRelayServer.SendMessage( kBillingPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Send Fail : %s:%s(%s)", __FUNCTION__, pUser->GetPrivateID().c_str(),  pUser->GetBillingGUID().c_str(), pUser->GetBillingUserKey().c_str() );
		SP2Packet kPacket( STPK_CONNECT );
		kPacket << CONNECT_EXCEPT << "";
		pUser->SendMessage( kPacket );
		pUser->ClearBillingGUID();
	}
	else
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Send : %s:%s(%s)", __FUNCTION__, pUser->GetPrivateID().c_str(), pUser->GetBillingGUID().c_str(), pUser->GetBillingUserKey().c_str() );

	ioHashString szUserKey;
	pUser->SetBillingUserKey( szUserKey ); // Encode PW은 여기서 삭제 한다.

	//접속 시간
	SYSTEMTIME st;
	GetLocalTime( &st );
	char szDisconnectDate[MAX_PATH]="";
	StringCbPrintf( szDisconnectDate, sizeof( szDisconnectDate ), "%04d-%02d-%02d_%02d:%02d:%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond );
	pUser->SetLatinConnTime( szDisconnectDate );



	return true;
}

bool ioLocalLatin::SendLogoutData( User *pUser )
{
	if( !pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pUser == NULL.", __FUNCTION__ );
		return true;
	}

	ioHashString publicIP;
	ioHashString privateIP;
	ioHashString szCountry;

	int iWin = 0;
	int iLose = 0;
	int iKill = 0;
	int iDeath = 0;
	int userExp = 0;
	__int64 userPeso = 0;
	
	publicIP.Clear();
	privateIP.Clear();
	szCountry.Clear();

	publicIP = pUser->GetPublicIP();
	if( publicIP.IsEmpty() )
		publicIP.Clear();
	
	privateIP = pUser->GetPrivateIP();
	if( privateIP.IsEmpty() )
		privateIP.Clear();

	//경험치와 얻은 페소 계산
	userExp = pUser->GetGradeExpert() - pUser->GetFirstExp();
	userPeso = pUser->GetMoney() - pUser->GetFirstMoney();
		
	//승패
	iWin  = pUser->GetWinCount();
	iLose = pUser->GetLoseCount();
	
	SP2Packet kBillingPacket( BSTPK_LOGOUT );
	kBillingPacket.Write( g_App.GetServerNo() );
	kBillingPacket.Write( pUser->GetPrivateID() );
	kBillingPacket.Write( pUser->GetCountry() );
	kBillingPacket.Write( pUser->GetGender() );
	kBillingPacket.Write( pUser->GetLatinConnTime() );
	kBillingPacket.Write( publicIP );
	kBillingPacket.Write( privateIP );
	kBillingPacket.Write( userPeso );		//게임 중 획득페소
	kBillingPacket.Write( iWin );											//게임 이긴 횟수
	kBillingPacket.Write( iLose );											//게임 진 횟수
	kBillingPacket.Write( 0 );											//게임 무승부 횟수 -> 0으로 보내줌
	kBillingPacket.Write( pUser->GetGiveupCount() );						//게임 포기 횟수
	kBillingPacket.Write( userExp );	//게임 획득 경험치
	kBillingPacket.Write( pUser->GetLogoutType() );

	if( !g_BillingRelayServer.SendMessage( kBillingPacket ) )			
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Send Fail : %s:%s(%s)", __FUNCTION__, pUser->GetPrivateID().c_str(),  pUser->GetBillingGUID().c_str(), pUser->GetBillingUserKey().c_str() );
	}
	else


	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[GAMELOG]%s ID:%s, Country:%s, Gender:%s, ConnTime:%s, IP:%s, IP:%s, Peso:%I64d, Win:%d, Lose:%d, GiveUp:%d, Exp:%d, ExitCode:%d", "ioLocalLatin::SendLogoutData", 
	pUser->GetPrivateID().c_str(), pUser->GetCountry().c_str(), pUser->GetGender().c_str(), pUser->GetLatinConnTime().c_str(), pUser->GetPublicIP(), pUser->GetPrivateIP(), userPeso, iWin, iLose, pUser->GetGiveupCount(), userExp, pUser->GetLogoutType() );

	return true;
}


void ioLocalLatin::FillRequestGetCash( IN User *pUser, IN SP2Packet &rkPacket )
{
	

	if( !pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pUser == NULL.", __FUNCTION__ );
		return;
	}

	rkPacket << pUser->GetBillingUserKey();
}

void ioLocalLatin::FillRequestOutputCash( IN User *pUser, IN SP2Packet &rkPacket, const char *szRecvPrivateID )
{
	if( !pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pUser == NULL.", __FUNCTION__ );
		return;
	}

	rkPacket << g_App.GetCSPort();
	rkPacket << pUser->GetBillingUserKey();
}

void ioLocalLatin::FillRequestLogin( OUT SP2Packet &rkPacket, IN ioHashString &rsValue1, IN ioHashString &rsValue2 )
{
	rkPacket << rsValue1; // usertype
}

void ioLocalLatin::SendRefundCash( User *pUser, int iTransactionID, bool bRefund )
{
}

void ioLocalLatin::SendUserInfo( User *pUser )
{
}

void ioLocalLatin::SendCancelCash( IN SP2Packet &rkPacket )
{
	if( rkPacket.GetPacketID() != BSTPK_OUTPUT_CASH_RESULT )
		return;

	int          iChannelingType = 0;
	ioHashString szBillingGUID;
	DWORD        dwUserIndex = 0;
	ioHashString szPrivateID;
	int          iPayAmt  = 0;
	int          iType    = 0;
	int          iReturnValue = 0;
	int          iTransactionID = 0;
	int          iCash = 0;
	int          iPurchasedCash = 0;
	ioHashString szBillingUserKey;
	ioHashString szChargeNo;

	rkPacket >> dwUserIndex;
	rkPacket >> szBillingGUID;
	rkPacket >> iReturnValue;

	if( iReturnValue != CASH_RESULT_SUCCESS )
		return;

	rkPacket >> iType;
	rkPacket >> iPayAmt;
	rkPacket >> iTransactionID;
	int iItemValueList[ioChannelingNodeParent::MAX_ITEM_VALUE];
	for (int i = 0; i < ioChannelingNodeParent::MAX_ITEM_VALUE ; i++)
		iItemValueList[i] = 0;
	ioChannelingNodeParent::GetItemValueList( rkPacket, iType, iItemValueList );

	// Cancel Step 2
	rkPacket >> iChannelingType;
	rkPacket >> iCash;
	rkPacket >> iPurchasedCash; 
	rkPacket >> szPrivateID;
	rkPacket >> szBillingUserKey;
	rkPacket >> szChargeNo;

	//--------------------------------SEND------------------------------------//
	char szTempGUID[USER_GUID_NUM_PLUS_ONE]="";
	Help::GetGUID( szTempGUID, sizeof(szTempGUID) );
	SP2Packet kBillingPacket( BSTPK_CANCEL_CASH );
	// 공통
	kBillingPacket << iChannelingType;
	kBillingPacket << szTempGUID;
	kBillingPacket << dwUserIndex;

	// Cancel Step 3
	kBillingPacket << szPrivateID;
	kBillingPacket << szBillingUserKey;
	kBillingPacket << szChargeNo;

	if( !g_BillingRelayServer.SendMessage( kBillingPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[GOLD_INFO]%s Send Fail : %d:%s:%s:%s", __FUNCTION__, dwUserIndex, szPrivateID.c_str(), szBillingGUID.c_str(), szTempGUID );
	}
	else
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[GOLD_INFO]%s Send : %d:%s:%s:%s", __FUNCTION__, dwUserIndex, szPrivateID.c_str(), szBillingGUID.c_str(), szTempGUID );
	}
}

int ioLocalLatin::GetFirstIDMaxSize()
{
	return 12;
}

const char * ioLocalLatin::GetGuildMasterPostion()
{
	return "Leader";
}

const char * ioLocalLatin::GetGuildSecondMasterPosition()
{
	return "Officer";
}

const char * ioLocalLatin::GetGuildGeneralPosition()
{
	return "Member";
}

bool ioLocalLatin::IsCheckKorean()
{
	return false;
}

bool ioLocalLatin::IsChannelingID()
{
	return false;
}

bool ioLocalLatin::IsBillingTestID( const ioHashString &rsPublicID )
{
	return false;
}

void ioLocalLatin::GetGuildTitle( OUT char *szTitle, IN int iTitleSize, IN const char *szName )
{
	StringCbPrintf( szTitle, iTitleSize, "%s.", szName );
}

bool ioLocalLatin::IsSamePCRoomUser()
{
	return false;
}

int ioLocalLatin::GetLimitGradeLevel()
{
	return -1;
}

bool ioLocalLatin::IsBadPingKick( bool bLadder )
{
	return false;
}

int ioLocalLatin::GetLicenseDate()
{
	return 20150806;

}

bool ioLocalLatin::GetCCUCountSend()
{
	/*
	int ccuCount = 0;
	ccuCount = g_MainServer.GetTotalUserRegCount();


	SP2Packet kBillingPacket( BSTPK_CCU_COUNT );
	kBillingPacket << g_MainServer.GetTotalUserRegCount();
	
	if( !g_BillingRelayServer.SendMessage( kBillingPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Send Fail : %s", "ioLocalLatin::GetCCUCountSend" , ccuCount);
	}
	else
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Send : %s", "ioLocalLatin::GetCCUCountSend" , ccuCount);
		*/
	return true;
}