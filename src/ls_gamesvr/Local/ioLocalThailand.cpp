#include "stdafx.h"
#include ".\ioLocalThailand.h"
#include <strsafe.h>
#include "..\NodeInfo\User.h"
#include "../BillingRelayServer/BillingRelayServer.h"
#include "../EtcHelpFunc.h"
#include "ioLocalThailandLanguage.h"
#include "../NodeInfo/UserNodeManager.h"

ioLocalThailand::ioLocalThailand(void)
{
}

ioLocalThailand::~ioLocalThailand(void)
{
}

ioLocalManager::LocalType ioLocalThailand::GetType()
{
	return ioLocalManager::LCT_THAILAND;
}

const char * ioLocalThailand::GetTextListFileName()
{
	return "text.txt";
}

bool ioLocalThailand::ParseLoginData( IN ioHashString &rsEncLoginKeyAndID, OUT char *szEncLoginKey, IN int iEncLoginKeySize, OUT char *szPrivateID, IN int iPrivaiteIDSize )
{
	return true;
}
// 빌링서버를 통해서 로그인 인증을 하므로 무조건 true로 셋팅 
bool ioLocalThailand::DecryptLoginKey( IN const char *szEncLoginKey, IN const char *szUserKey, OUT char *szDecLoginKey, IN int iDecLoginKeySize )
{
	return true;
}

bool ioLocalThailand::IsRightTimeLoginKey( DWORD dwTotalMins )
{
	return true;
}

bool ioLocalThailand::IsRightLoginKey( const char *szDBKey, const char *szDecryptKey )
{
	return true;
}

void ioLocalThailand::ApplyConnect( IN User *pUser, IN SP2Packet &rkPacket )
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

bool ioLocalThailand::ApplyLogin( IN User *pUser, IN SP2Packet &rkPacket )
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

bool ioLocalThailand::SendLoginData( User *pUser )
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
		kPacket << CONNECT_EXCEPT << "ioLocalThailand";
		pUser->SendMessage( kPacket );
		pUser->ClearBillingGUID();
	}
	else
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Send : %s:%s", __FUNCTION__, pUser->GetPrivateID().c_str(), pUser->GetBillingGUID().c_str() );

	ioHashString szUserKey;
	pUser->SetBillingUserKey( szUserKey ); // 로그인에서만 사용하므로 바로 삭제

	return true;
}

void ioLocalThailand::FillRequestGetCash( IN User *pUser, IN SP2Packet &rkPacket )
{
	if( !pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pUser == NULL.", __FUNCTION__ );
		return;
	}

	rkPacket << pUser->GetPublicIP();
}

void ioLocalThailand::FillRequestOutputCash( IN User *pUser, IN SP2Packet &rkPacket, const char *szRecvPrivateID )
{
	if( !pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pUser == NULL.", __FUNCTION__ );
		return;
	}

	if( !szRecvPrivateID )
		return;

	ioHashString sRecvPrivateID = szRecvPrivateID; // 패킷에 const char*을 바로 넣을 수 없어서
	rkPacket << sRecvPrivateID;
}

void ioLocalThailand::SendRefundCash( User *pUser, int iTransactionID, bool bRefund )
{

}


bool ioLocalThailand::CheckDuplication( User *pUser, const ioHashString &rsPrivateID )
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

void ioLocalThailand::SendUserInfo( User *pUser )
{

}

int ioLocalThailand::GetFirstIDMaxSize()
{
	return 12;
}

const char * ioLocalThailand::GetGuildMasterPostion()
{
	return GUILD_MASTER_POSTION_TH;
}

const char * ioLocalThailand::GetGuildSecondMasterPosition()
{
	return GUILD_SECOND_MASTER_POSTION_TH;
}

const char * ioLocalThailand::GetGuildGeneralPosition()
{
	return GUILD_GENERAL_POSTION_TH;
}

bool ioLocalThailand::IsCheckKorean()
{
	return false;
}

bool ioLocalThailand::IsChannelingID()
{
	return false;
}

bool ioLocalThailand::IsBillingTestID( const ioHashString &rsPublicID )
{
	return false;
}

void ioLocalThailand::GetGuildTitle( OUT char *szTitle, IN int iTitleSize, IN const char *szName )
{
	StringCbPrintf( szTitle, iTitleSize, "%s.", szName );
}

bool ioLocalThailand::IsSamePCRoomUser()
{
	return false;
}

int ioLocalThailand::GetLimitGradeLevel()
{
	return -1;
}

bool ioLocalThailand::IsBadPingKick( bool bLadder )
{
	return false;
}

bool ioLocalThailand::IsRightID( const char *szID )
{
	enum { MIN_LENGTH = 6, MAX_LENGTH = 40, }; // MIN_LENGTH 1+5(domain) / 32(max id) + 8( domain + 구분자 )

	int iSize = strlen( szID );
	if ( iSize < MIN_LENGTH || iSize > MAX_LENGTH )
		return false;

	for (int i=0; i<iSize; i++)
	{
		if ((!COMPARE(szID[i], 'A', 'Z'+1)) &&
			(!COMPARE(szID[i], 'a', 'z'+1)) &&
			(!COMPARE(szID[i], '0', '9'+1)) &&
			(szID[i]!='!') &&
			(szID[i]!='@') &&
			(szID[i]!='#') &&
			(szID[i]!='$') &&
			(szID[i]!='%') &&
			(szID[i]!='^') &&
			(szID[i]!='&') &&
			(szID[i]!='*') &&
			(szID[i]!='(') &&
			(szID[i]!=')') &&
			(szID[i]!='_') &&
			(szID[i]!='-') &&
			(szID[i]!='[') &&
			(szID[i]!=']') &&
			(szID[i]!=' ') &&	
			(szID[i]!='.') &&   // THPP.tester001 도메인을 구분하는 구분자로 사용
			(szID[i]!='|') )    // 로그인 아이디와 private 아이디를 구분하기 위해서 로그인 아이디 끝에 붙임
		{
			return false;
		}
	}
	return true;
}

const char * ioLocalThailand::GetDuplicationMent()
{
	return DUPLICATION_MENT_TH;
}

const char * ioLocalThailand::GetExitingServerMent()
{
	return EXITING_MENT_TH;
}

const char * ioLocalThailand::GetOtherComanyErrorMent()
{
	return "Asiasoft Error : ";
}

bool ioLocalThailand::IsPrivateLowerID()
{
	return false;
}

int ioLocalThailand::GetLicenseDate()
{
	return 20180806;

}