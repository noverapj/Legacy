#include "stdafx.h"
#include ".\iolocaltaiwan.h"
//#include "../Window.h"
#include "../MainProcess.h"
#include "../Util/ioEncrypted.h"
#include <strsafe.h>
#include "ioLocalTaiwanLanguage.h"
#include "..\NodeInfo\User.h"
#include "../BillingRelayServer/BillingRelayServer.h"
#include "../EtcHelpFunc.h"
#include "../NodeInfo/UserNodeManager.h"
#include "../Channeling/ioChannelingNodeParent.h"

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

const char * ioLocalTaiwan::GetTextListFileName()
{
	return "text_tw.txt";
}

bool ioLocalTaiwan::ParseLoginData( IN ioHashString &rsEncLoginKeyAndID, OUT char *szEncLoginKey, IN int iEncLoginKeySize, OUT char *szPrivateID, IN int iPrivaiteIDSize )
{	
	return true;
}
// 빌링서버를 통해서 로그인 인증을 하므로 무조건 true로 셋팅 

bool ioLocalTaiwan::DecryptLoginKey( IN const char *szEncLoginKey, IN const char *szUserKey, OUT char *szDecLoginKey, IN int iDecLoginKeySize )
{
	return true;
}

bool ioLocalTaiwan::IsRightTimeLoginKey( DWORD dwTotalMins )
{
	return true;
}

bool ioLocalTaiwan::IsRightLoginKey( const char *szDBKey, const char *szDecryptKey )
{
	return true;
}

void ioLocalTaiwan::ApplyConnect( IN User *pUser, IN SP2Packet &rkPacket )
{
	if( !pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pUser == NULL.", __FUNCTION__ );
		return;
	}
	ioHashString sEndPW;
	rkPacket >> sEndPW;

    // encode pw 값 (임시저장)
	pUser->SetBillingUserKey( sEndPW );

}

bool ioLocalTaiwan::ApplyLogin( IN User *pUser, IN SP2Packet &rkPacket )
{
	if( !pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pUser == NULL.", __FUNCTION__ );
		return true;
	}

	ioHashString sPrivateID;
	rkPacket >> sPrivateID;

	if( g_UserNodeManager.IsConnectUser( sPrivateID ) )   //접속중..
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Duplication: %s:%s", __FUNCTION__, sPrivateID.c_str(), pUser->GetBillingGUID().c_str() );
		SP2Packet kPacket( STPK_CONNECT );
		kPacket << CONNECT_ID_ALREADY << sPrivateID;
		pUser->SendMessage( kPacket );
		return false;
	}

	// real private id로 교체
	pUser->SetPrivateID( sPrivateID );
	pUser->SetDBAgentID( Help::GetUserDBAgentID( pUser->GetPrivateID() ) );
	return true;
}

bool ioLocalTaiwan::SendLoginData( User *pUser )
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
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Send Fail : %s:%s", __FUNCTION__, pUser->GetPrivateID().c_str(),  pUser->GetBillingGUID().c_str() );
		SP2Packet kPacket( STPK_CONNECT );
		kPacket << CONNECT_EXCEPT << "ioLocalTaiwan";
		pUser->SendMessage( kPacket );
		pUser->ClearBillingGUID();
	}
	else
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Send : %s:%s", __FUNCTION__, pUser->GetPrivateID().c_str(), pUser->GetBillingGUID().c_str() );

	ioHashString szUserKey;
	pUser->SetBillingUserKey( szUserKey ); // 로그인에서만 사용하므로 바로 삭제

	return true;
}

void ioLocalTaiwan::FillRequestGetCash( IN User *pUser, IN SP2Packet &rkPacket )
{
}

void ioLocalTaiwan::SendRefundCash( User *pUser, int iTransactionID, bool bRefund )
{

}


bool ioLocalTaiwan::CheckDuplication( User *pUser, const ioHashString &rsPrivateID )
{
	if( !pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pUser == NULL.", __FUNCTION__ );
		return true;
	}

	if( rsPrivateID.IsEmpty() )
		return true;

	if( g_UserNodeManager.IsConnectUser( rsPrivateID ) )  //접속중..
	{
		SP2Packet kReturn( ASTPK_OTHER_COMPANY_LOGIN_RESULT );
		kReturn << false;
		kReturn << true;
		char szMent[MAX_PATH]="";
		StringCbCopy( szMent , sizeof( szMent ), GetDuplicationMent() );
		kReturn << szMent;
		pUser->SendMessage( kReturn );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Exist User: [%s]:%s", __FUNCTION__, rsPrivateID.c_str() , pUser->GetBillingGUID().c_str() );
		ioHashString szEmpty;
		pUser->SetPrivateID( szEmpty ); // 초기화
		pUser->ClearBillingGUID();
		return false;
	}

	return true;
}

void ioLocalTaiwan::SendUserInfo( User *pUser )
{

}

int ioLocalTaiwan::GetFirstIDMaxSize()
{
	return 12;
}

const char * ioLocalTaiwan::GetGuildMasterPostion()
{
	return GUILD_MASTER_POSTION_TW;
}

const char * ioLocalTaiwan::GetGuildSecondMasterPosition()
{
	return GUILD_SECOND_MASTER_POSTION_TW;
}

const char * ioLocalTaiwan::GetGuildGeneralPosition()
{
	return GUILD_GENERAL_POSTION_TW;
}

bool ioLocalTaiwan::IsCheckKorean()
{
	return false;
}

bool ioLocalTaiwan::IsChannelingID()
{
	return false;
}

bool ioLocalTaiwan::IsBillingTestID( const ioHashString &rsPublicID )
{
	return false;
}

void ioLocalTaiwan::GetGuildTitle( OUT char *szTitle, IN int iTitleSize, IN const char *szName )
{
	StringCbPrintf( szTitle, iTitleSize, "%s.", szName );
}

bool ioLocalTaiwan::IsSamePCRoomUser()
{
	return false;
}

int ioLocalTaiwan::GetLimitGradeLevel()
{
	return -1;
}

bool ioLocalTaiwan::IsBadPingKick( bool bLadder )
{
	return true;
}

bool ioLocalTaiwan::IsRightID( const char *szID )
{
	enum { MIN_LENGTH = 1, MAX_LENGTH = 60, };

	int iSize = strlen( szID );
	if ( iSize < MIN_LENGTH || iSize > MAX_LENGTH )
		return false;

	return true;
}

const char * ioLocalTaiwan::GetDuplicationMent()
{
	return DUPLICATION_MENT_TW;
}

const char * ioLocalTaiwan::GetExitingServerMent()
{
	return EXITING_MENT_TW;
}

const char * ioLocalTaiwan::GetOtherComanyErrorMent()
{
	return "GaMonster Error : ";
}

bool ioLocalTaiwan::IsPrivateLowerID()
{
	return true;
}

int ioLocalTaiwan::GetLicenseDate()
{
	return 20181231;
}

void ioLocalTaiwan::SendCancelCash( IN SP2Packet &rkPacket )
{
	if( rkPacket.GetPacketID() != BSTPK_OUTPUT_CASH_RESULT )
		return;

	int          iChannelingType = 0;
	ioHashString szBillingGUID;
	DWORD        dwUserIndex = 0;
	ioHashString szPrivateID;
	ioHashString szOrderNo;
	ioHashString szUserIP;
	int          iPayAmt  = 0;
	int          iType    = 0;
	int          iReturnValue = 0;
	int          iTransactionID = 0;
	int          iTotalCash     = 0;
	int          iPurchasedCash = 0;

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
	rkPacket >> iTotalCash;
	rkPacket >> iPurchasedCash;
	rkPacket >> szPrivateID;
	rkPacket >> szOrderNo;

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
	kBillingPacket << szOrderNo;

	if( !g_BillingRelayServer.SendMessage( kBillingPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Send Fail : %d:%s:%s:%s", __FUNCTION__, dwUserIndex, szPrivateID.c_str(), szBillingGUID.c_str(), szTempGUID );
	}
	else
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Send : %d:%s:%s:%s", __FUNCTION__, dwUserIndex, szPrivateID.c_str(), szBillingGUID.c_str(), szTempGUID );
	}
}