#include "stdafx.h"
#include ".\ioLocalBrazil.h"
#include <strsafe.h>
#include "../NodeInfo/User.h"
#include "../BillingRelayServer/BillingRelayServer.h"
#include "../EtcHelpFunc.h"
#include "../MainProcess.h"
#include "../Channeling/ioChannelingNodeParent.h"
#include "../NodeInfo/UserNodeManager.h"

ioLocalBrazil::ioLocalBrazil(void)
{
}

ioLocalBrazil::~ioLocalBrazil(void)
{
}

ioLocalManager::LocalType ioLocalBrazil::GetType()
{
	return ioLocalManager::LCT_BRAZIL;
}

const char * ioLocalBrazil::GetTextListFileName()
{
	return "text_us.txt";
}

bool ioLocalBrazil::ParseLoginData( IN ioHashString &rsEncLoginKeyAndID, OUT char *szEncLoginKey, IN int iEncLoginKeySize, OUT char *szPrivateID, IN int iPrivaiteIDSize )
{
	return true;
}
// 미국에서는 빌링서버를 통해서 로그인 인증을 하므로 무조건 true로 셋팅 
bool ioLocalBrazil::DecryptLoginKey( IN const char *szEncLoginKey, IN const char *szUserKey, OUT char *szDecLoginKey, IN int iDecLoginKeySize )
{
	return true;
}

bool ioLocalBrazil::IsRightLoginKey( const char *szDBKey, const char *szDecryptKey )
{
	return true;
}

bool ioLocalBrazil::IsRightTimeLoginKey( DWORD dwTotalMins )
{
	return true;
}

//인증토큰
void ioLocalBrazil::ApplyConnect( IN User *pUser, IN SP2Packet &rkPacket )
{
	if( !pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pUser == NULL.", __FUNCTION__ );
		return;
	}
	ioHashString sEndPW;
	DWORD		 memberType = 0;
	rkPacket >> sEndPW;		//인증토큰
	rkPacket >> memberType;	//스팀유저 구분타입

    // encode pw 값 
	pUser->SetBillingUserKey( sEndPW );
	pUser->SetUSMemberType( memberType );

}

bool ioLocalBrazil::ApplyLogin( IN User *pUser, IN SP2Packet &rkPacket )
{
	if( !pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pUser == NULL.", __FUNCTION__ );
		return true;
	}
	
	ioHashString	szUserId;
	ioHashString    szUserIndex;	
	
	rkPacket >> szUserIndex;		//int 형의 북미아이디 실제userID
	rkPacket >> szUserId;			// 북미에서 인증 후 주는 id

	// real private id로 교체
	//pUser->SetUSMemberIndex( dwMemberID );

	pUser->SetPrivateID(szUserIndex);		//GAuthClient 에서 인증결과값으로 받은 userID를 저장한다. //이거
	pUser->SetUSMemberID( szUserId );
	
	pUser->SetDBAgentID( Help::GetUserDBAgentID( pUser->GetPrivateID() ) );
	
	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Success %d:%s:%s, USMemberType:%d, USMemberIndex:%s, USMemberID:%s", 
		__FUNCTION__, szUserIndex.c_str(), szUserId.c_str(), pUser->GetPrivateID().c_str(), pUser->GetUSMemberType(), pUser->GetUSMemberIndex(), pUser->GetUSMemberID().c_str());
	return true;
	/*
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
	rkPacket >> sReturnValue;
	
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
			pPos = strtok_s( szTemp, US_TOKEN, &next_token );
		else
			pPos = strtok_s( NULL, US_TOKEN, &next_token );

		if( pPos == NULL )
			break;

		if( i == ID_ARRAY )
			sPrivateID  = pPos;
		else if( i == TOKEN_ARRAY )
			sLoginToken = pPos;
		else if( i == NAME_ARRAY )
			sUserName   = pPos;
	}

	LOG.PrintTimeAndLog( 0, "%s %s %s %s", __FUNCTION__, sPrivateID.c_str(), sLoginToken.c_str(), sUserName.c_str() );

	if( g_UserNodeManager.IsConnectUser( sPrivateID ) )   //접속중..
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

	// real private id로 교체
	pUser->SetPrivateID( sPrivateID );
	pUser->SetDBAgentID( Help::GetUserDBAgentID( pUser->GetPrivateID() ) );
	return true;
	*/
}
void ioLocalBrazil::ApplyRequestLogin( IN SP2Packet &rkPacket, OUT ioHashString &rsValue1, OUT ioHashString &rsValue2 )
{
	rkPacket >> rsValue1; // usertype
}

//HRYOON 140925
bool ioLocalBrazil::SendLoginData( User *pUser )
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
	kBillingPacket << pUser->GetBillingUserKey();	//유저인증키
	kBillingPacket << pUser->GetPublicIP();
	kBillingPacket << BSTPK_LOGIN_RESULT; // return msg type
	//추가로 스팀인지 북미 채널링 유저인지 구별하는 값도 함께 보내야함
	kBillingPacket << "WMU";
	if( !g_BillingRelayServer.SendMessage( kBillingPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Send Fail : %s:%s(%s)", __FUNCTION__, pUser->GetPrivateID().c_str(),  pUser->GetBillingGUID().c_str(), pUser->GetBillingUserKey().c_str() );
		SP2Packet kPacket( STPK_CONNECT );
		kPacket << CONNECT_EXCEPT << "ioLocalBrazil";
		pUser->SendMessage( kPacket );
		pUser->ClearBillingGUID();
	}
	else
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Send : %s:%s(%s)", __FUNCTION__, pUser->GetPrivateID().c_str(), pUser->GetBillingGUID().c_str(), pUser->GetBillingUserKey().c_str() );

	ioHashString szUserKey;
	pUser->SetBillingUserKey( szUserKey ); // Encode PW은 여기서 삭제 한다.

	return true;
}

void ioLocalBrazil::FillRequestGetCash( IN User *pUser, IN SP2Packet &rkPacket )
{
	if( !pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pUser == NULL.", __FUNCTION__ );
		return;
	}

	rkPacket << pUser->GetUSMemberID() << pUser->GetUSMemberType();
	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioLocalBrazil::FillRequestGetCash USMemberID : %s, MemberType : %d", 
												pUser->GetUSMemberID().c_str(), pUser->GetUSMemberType());
}

void ioLocalBrazil::FillRequestOutputCash( IN User *pUser, IN SP2Packet &rkPacket, const char *szRecvPrivateID )
{
	if( !pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pUser == NULL.", __FUNCTION__ );
		return;
	}

	rkPacket << g_App.GetCSPort();
	rkPacket << pUser->GetUSMemberType();	//스팀 구별용 타입 1: 스팀
	rkPacket << pUser->GetUSMemberID();		//US 멤버아이디

	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s MemberType : %d,%s ", __FUNCTION__, pUser->GetUSMemberType(), pUser->GetUSMemberID().c_str() );
}

//HRYOON 오토업그레이드에서 보내주는 이 값이 어떤값인지 알아야함 140926
void ioLocalBrazil::FillRequestLogin( OUT SP2Packet &rkPacket, IN ioHashString &rsValue1, IN ioHashString &rsValue2 )
{
	rkPacket << rsValue1; // usertype ( USER_TYPE_NORMAL / USER_TYPE_FB )
}

void ioLocalBrazil::SendRefundCash( User *pUser, int iTransactionID, bool bRefund )
{
}

void ioLocalBrazil::SendUserInfo( User *pUser )
{
}

void ioLocalBrazil::SendCancelCash( IN SP2Packet &rkPacket )
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

int ioLocalBrazil::GetFirstIDMaxSize()
{
	return 12;
}

const char * ioLocalBrazil::GetGuildMasterPostion()
{
	return "Leader";
}

const char * ioLocalBrazil::GetGuildSecondMasterPosition()
{
	return "Officer";
}

const char * ioLocalBrazil::GetGuildGeneralPosition()
{
	return "Member";
}

const char * ioLocalBrazil::GetGuildBuilderPosition()
{
	return "Admin";
}

bool ioLocalBrazil::IsCheckKorean()
{
	return false;
}

bool ioLocalBrazil::IsChannelingID()
{
	return false;
}

bool ioLocalBrazil::IsBillingTestID( const ioHashString &rsPublicID )
{
	return false;
}

void ioLocalBrazil::GetGuildTitle( OUT char *szTitle, IN int iTitleSize, IN const char *szName )
{
	StringCbPrintf( szTitle, iTitleSize, "%s.", szName );
}

bool ioLocalBrazil::IsSamePCRoomUser()
{
	return false;
}

int ioLocalBrazil::GetLimitGradeLevel()
{
	return -1;
}

bool ioLocalBrazil::IsBadPingKick( bool bLadder )
{
	return false;
}

int ioLocalBrazil::GetLicenseDate()
{
	return 20190806;

}

#if _TESTMODE
bool ioLocalBrazil::SendLogoutData( User *pUser )
{
	if( !pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pUser == NULL.", __FUNCTION__ );
		return true;
	}

	SP2Packet kBillingPacket( BSTPK_LOGOUT );
	kBillingPacket.Write( pUser->GetPrivateID() );
	if( !g_BillingRelayServer.SendMessage( kBillingPacket ) )			
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Send Fail : %s:%s(%s)", __FUNCTION__, pUser->GetPrivateID().c_str(),  pUser->GetBillingGUID().c_str(), pUser->GetBillingUserKey().c_str() );
	}
	
	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[GAMELOG] ioLocalLatin::SendLogoutData ID:%s", pUser->GetPrivateID().c_str() );

	return true;
}
#endif