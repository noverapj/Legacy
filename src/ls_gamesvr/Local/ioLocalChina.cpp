#include "stdafx.h"
#include ".\iolocalChina.h"
//#include "../Window.h"
#include "../MainProcess.h"
#include "../Util/ioEncrypted.h"
#include <strsafe.h>
#include "../EtcHelpFunc.h"
#include "ioLocalChinaLanguage.h"

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

const char * ioLocalChina::GetTextListFileName()
{
	return "text.txt";
}

bool ioLocalChina::ParseLoginData( IN ioHashString &rsEncLoginKeyAndID, OUT char *szEncLoginKey, IN int iEncLoginKeySize, OUT char *szPrivateID, IN int iPrivaiteIDSize )
{
	return true;
}

bool ioLocalChina::DecryptLoginKey( IN const char *szEncLoginKey, IN const char *szUserKey, OUT char *szDecLoginKey, IN int iDecLoginKeySize )
{
	return true;
}

bool ioLocalChina::IsRightTimeLoginKey( DWORD dwTotalMins )
{
	return true;
}

void ioLocalChina::ApplyConnect( IN User *pUser, IN SP2Packet &rkPacket )
{

}

bool ioLocalChina::SendLoginData( User *pUser )
{
	return false;
}

bool ioLocalChina::IsRightLoginKey( const char *szDBKey, const char *szDecryptKey )
{
	return true;
}

void ioLocalChina::FillRequestGetCash( IN User *pUser, IN SP2Packet &rkPacket )
{

}

void ioLocalChina::SendRefundCash( User *pUser, int iTransactionID, bool bRefund )
{

}

void ioLocalChina::SendUserInfo( User *pUser )
{

}

int ioLocalChina::GetFirstIDMaxSize()
{
	return 12;
}

const char * ioLocalChina::GetGuildMasterPostion()
{
	return GUILD_MASTER_POSTION_CH;
}

const char * ioLocalChina::GetGuildSecondMasterPosition()
{
	return GUILD_SECOND_MASTER_POSTION_CH;
}

const char * ioLocalChina::GetGuildGeneralPosition()
{
	return GUILD_GENERAL_POSTION_CH;
}

bool ioLocalChina::IsCheckKorean()
{
	return true;
}

bool ioLocalChina::IsChannelingID()
{
	return false;
}

bool ioLocalChina::IsBillingTestID( const ioHashString &rsPublicID )
{
	return false;
}

void ioLocalChina::GetGuildTitle( OUT char *szTitle, IN int iTitleSize, IN const char *szName )
{
	StringCbPrintf( szTitle, iTitleSize, "%s.", szName );
}

bool ioLocalChina::IsSamePCRoomUser()
{
	return true;
}

int ioLocalChina::GetLimitGradeLevel()
{
	return -1;
}

bool ioLocalChina::IsBadPingKick( bool bLadder )
{
	return true;
}


int ioLocalChina::GetLicenseDate()
{
	return 20150806;
}
