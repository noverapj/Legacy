#include "stdafx.h"
#include ".\ioLocalIndonesia.h"
#include <strsafe.h>
#include "..\NodeInfo\User.h"
#include "../BillingRelayServer/BillingRelayServer.h"
#include "../EtcHelpFunc.h"

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

const char * ioLocalIndonesia::GetTextListFileName()
{
	return "text_id.txt";
}

bool ioLocalIndonesia::ParseLoginData( IN ioHashString &rsEncLoginKeyAndID, OUT char *szEncLoginKey, IN int iEncLoginKeySize, OUT char *szPrivateID, IN int iPrivaiteIDSize )
{	
	return true;
}
// 빌링서버를 통해서 로그인 인증을 하므로 무조건 true로 셋팅 
bool ioLocalIndonesia::DecryptLoginKey( IN const char *szEncLoginKey, IN const char *szUserKey, OUT char *szDecLoginKey, IN int iDecLoginKeySize )
{
	return true;
}

bool ioLocalIndonesia::IsRightTimeLoginKey( DWORD dwTotalMins )
{
	return true;
}

bool ioLocalIndonesia::IsRightLoginKey( const char *szDBKey, const char *szDecryptKey )
{
	return true;
}

void ioLocalIndonesia::ApplyConnect( IN User *pUser, IN SP2Packet &rkPacket )
{
	if( !pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pUser == NULL.", __FUNCTION__ );
		return;
	}
	ioHashString sEndPW;
	rkPacket >> sEndPW;

    // 인도네시아는 encode pw 값 ( 임시 저장 ) 
	pUser->SetBillingUserKey( sEndPW );

}

bool ioLocalIndonesia::ApplyLogin( IN User *pUser, IN SP2Packet &rkPacket )
{
	if( !pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pUser == NULL.", __FUNCTION__ );
		return false;
	}

	// indonesia는 billingKey을 받는다.
	ioHashString sBillingKey;
	DWORD dwPCRoomNum = 0;
	rkPacket >> sBillingKey;
	rkPacket >> dwPCRoomNum;

	pUser->SetBillingUserKey( sBillingKey );
	pUser->SetPCRoomNumber( dwPCRoomNum );

	return true;
}

bool ioLocalIndonesia::SendLoginData( User *pUser )
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
		kPacket << CONNECT_EXCEPT << "ioLocalIndonesia";
		pUser->SendMessage( kPacket );
		pUser->ClearBillingGUID();
	}
	else
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Send : %s:%s(%s)", __FUNCTION__, pUser->GetPrivateID().c_str(), pUser->GetBillingGUID().c_str(), pUser->GetBillingUserKey().c_str() );

	ioHashString szUserKey;
	pUser->SetBillingUserKey( szUserKey ); // 로그인에서만 사용하므로 바로 삭제

	return true;
}

void ioLocalIndonesia::FillRequestGetCash( IN User *pUser, IN SP2Packet &rkPacket )
{
	if( !pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pUser == NULL.", __FUNCTION__ );
		return;
	}

	rkPacket << pUser->GetBillingUserKey();
}

void ioLocalIndonesia::FillRequestOutputCash( IN User *pUser, IN SP2Packet &rkPacket, const char *szRecvPrivateID )
{
	if( !pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pUser == NULL.", __FUNCTION__ );
		return;
	}

	rkPacket << pUser->GetBillingUserKey();
	rkPacket << pUser->GetGradeLevel();
}

void ioLocalIndonesia::SendRefundCash( User *pUser, int iTransactionID, bool bRefund )
{

}

void ioLocalIndonesia::SendUserInfo( User *pUser )
{

}

int ioLocalIndonesia::GetFirstIDMaxSize()
{
	return 12;
}

const char * ioLocalIndonesia::GetGuildMasterPostion()
{
	return "Leader";
}

const char * ioLocalIndonesia::GetGuildSecondMasterPosition()
{
	return "Officer";
}

const char * ioLocalIndonesia::GetGuildGeneralPosition()
{
	return "Member";
}

bool ioLocalIndonesia::IsCheckKorean()
{
	return false;
}

bool ioLocalIndonesia::IsChannelingID()
{
	return false;
}

bool ioLocalIndonesia::IsBillingTestID( const ioHashString &rsPublicID )
{
	return false;
}

void ioLocalIndonesia::GetGuildTitle( OUT char *szTitle, IN int iTitleSize, IN const char *szName )
{
	StringCbPrintf( szTitle, iTitleSize, "%s.", szName );
}

bool ioLocalIndonesia::IsSamePCRoomUser()
{
	return false;
}

int ioLocalIndonesia::GetLimitGradeLevel()
{
	return -1;
}

bool ioLocalIndonesia::IsBadPingKick( bool bLadder )
{
	if( bLadder )
		return true;
	else
		return false;
}

bool ioLocalIndonesia::IsRightID( const char *szID )
{
	enum { MIN_LENGTH = 6, MAX_LENGTH = 12, };

	int iSize = strlen( szID );
	if ( iSize < MIN_LENGTH || iSize > MAX_LENGTH )
		return false;

	for (int i=0; i<iSize; i++)
	{
		if ((!COMPARE(szID[i], 'A', 'Z'+1)) &&
			(!COMPARE(szID[i], 'a', 'z'+1)) &&
			(!COMPARE(szID[i], '0', '9'+1)) &&
			(szID[i]!='-') &&
			(szID[i]!='_') )
		{
			return false;
		}
	}
	return true;
}

const char * ioLocalIndonesia::GetDuplicationMent()
{
	return "Telah mencoba login berulang kali.\r\n\r\n Silahkan coba login kembali 5 menit lagi.\r\n\r\n Jika masih mengalami masalah \r\n\r\n hubungi Tim GM.";
}

const char * ioLocalIndonesia::GetExitingServerMent()
{
	return "Server ditutup.\r\n\r\n Cek Pengumuman\r\n\r\n dan pelayanan akan kembali beberapa saat lagi.";
}

const char * ioLocalIndonesia::GetOtherComanyErrorMent()
{
	return "KREON Error : ";
}

bool ioLocalIndonesia::IsPrivateLowerID()
{
	return true;
}

int ioLocalIndonesia::GetLicenseDate()
{
	return 20131231;
}

bool ioLocalIndonesia::IsRightNewID( const char *szID )
{
	int size = strlen(szID);
	for (int i=0; i<size; i++)
	{
		if ((!COMPARE(szID[i], 'A', 'Z'+1)) &&
			(!COMPARE(szID[i], 'a', 'z'+1)) &&
			(!COMPARE(szID[i], '0', '9'+1)) )
		{
			return false;
		}
	}
	return true;
}