#include "stdafx.h"
#include ".\iolocalJapan.h"
//#include "../Window.h"
#include "../MainProcess.h"
#include "../Util/ioEncrypted.h"
#include <strsafe.h>
#include "../Netmarble/NMCrypt.h"
#include "../EtcHelpFunc.h"
#include "../Util/md5.h"
#include "ioLocalJapanLanguage.h"
#include "../NodeInfo/User.h"
#include "../Channeling/ioChannelingNodeParent.h"
#include "../BillingRelayServer/BillingRelayServer.h"

ioLocalJapan::ioLocalJapan(void)
{
	m_bRightTimeLoginKey = false;
	m_bDecryptLoginKey   = false;
}

ioLocalJapan::~ioLocalJapan(void)
{
}

ioLocalManager::LocalType ioLocalJapan::GetType()
{
	return ioLocalManager::LCT_JAPAN;
}

const char * ioLocalJapan::GetTextListFileName()
{
	return "text.txt";
}

bool ioLocalJapan::ParseLoginData( IN ioHashString &rsEncLoginKeyAndID, OUT char *szEncLoginKey, IN int iEncLoginKeySize, OUT char *szPrivateID, IN int iPrivaiteIDSize )
{
	enum 
	{ 
		MAX_KEY_DATA         = 1024, 
		TOTAL_MINUTES        = 60,

		MAX_TOKEN            = 3, 
		TOKEN_TIME_ARRAY     = 0, 
		TOKEN_MD5_ARRAY      = 1, 
		TOKEN_ID_ARRAY       = 2, 
	};

	char szEncDataList[MAX_TOKEN][MAX_KEY_DATA];
	ZeroMemory( szEncDataList, sizeof( szEncDataList ) );

	char* next_token = NULL;

	char szEncData[MAX_KEY_DATA];
	ZeroMemory( szEncData, MAX_KEY_DATA );
	StringCbCopy( szEncData, MAX_KEY_DATA, rsEncLoginKeyAndID.c_str() );

	for (int i = 0; i < MAX_TOKEN ; i++)
	{
		char *pPos = NULL;
		if( i == 0 )
			pPos = strtok_s( szEncData, JAPAN_TOKEN, &next_token );
		else
			pPos = strtok_s( NULL, JAPAN_TOKEN, &next_token );

		if( pPos == NULL )
			break;

		StringCbCopy( szEncDataList[i], sizeof( szEncDataList[i] ), pPos  );
	}

	char szDate[MAX_PATH]="";
	int  iCheckArrayList[MAX_TOKEN]={ TOKEN_ID_ARRAY, TOKEN_TIME_ARRAY, TOKEN_MD5_ARRAY };
	for (int i = 0; i < MAX_TOKEN ; i++)
	{
		int iCheckArray = iCheckArrayList[i];
		if( strcmp( szEncDataList[iCheckArray], "" ) == 0 )
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Error data is empty.(%d)", __FUNCTION__, iCheckArray );
			return false;
		}

		char szDecryptData[MAX_KEY_DATA];
		ZeroMemory( szDecryptData, MAX_KEY_DATA );
		int iReturn = DecryptString( szEncDataList[iCheckArray], JAPAN_CRYPT_KEY, szDecryptData );
		if( CRYPT_SUCCESS != iReturn )
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s Error Decrypt.(%d:%d)", __FUNCTION__, iCheckArray, iReturn );
			return false;
		}

		if( iCheckArray == TOKEN_ID_ARRAY )
		{
			StringCbCopy( szPrivateID, iPrivaiteIDSize, szDecryptData );
		}
		else if( iCheckArray == TOKEN_TIME_ARRAY )
		{
			// parse date
			enum { MAX_DATE = 6, };
			SHORT iDate[MAX_DATE];
			int iPos = 0;
			for (int j = 0; j < MAX_DATE ; j++)
			{
				int iSize = 2;
				if( j == 0 )
					iSize = 4;
				char szDate[MAX_PATH]="";
				StringCbCopyN( szDate, sizeof( szDate ), &szDecryptData[iPos], iSize ); // [2011]0221052348
				iPos += iSize;
				iDate[j] = atoi( szDate );
			}

			// calculate time
			CTime keyCreateTime(Help::GetSafeValueForCTimeConstructor( iDate[0], iDate[1], iDate[2], iDate[3], iDate[4], iDate[5] ) );
			CTime current_time = CTime::GetCurrentTime();
			CTimeSpan gaptime;
			gaptime = current_time - keyCreateTime;

			if( g_App.IsTestZone() )
			{
				m_bRightTimeLoginKey = true;
			}
			else
			{
				if( gaptime.GetTotalMinutes() < TOTAL_MINUTES )
					m_bRightTimeLoginKey = true;
				else
					m_bRightTimeLoginKey = false;
			}

			StringCbCopy( szDate, sizeof( szDate ), szDecryptData );

			if( !m_bRightTimeLoginKey )
				break;
		}
		else if( iCheckArray == TOKEN_MD5_ARRAY )
		{
			char szIDTime[MAX_PATH];
			ZeroMemory( szIDTime, sizeof( szIDTime ) );
			StringCbCat( szIDTime, sizeof( szIDTime ), szPrivateID );
			StringCbCat( szIDTime, sizeof( szIDTime ), szDate );

			char szHexMD5[MAX_PATH*4];
			ZeroMemory( szHexMD5, sizeof( szHexMD5 ) );
			GetHexMD5( szHexMD5, sizeof( szHexMD5 ), szIDTime );
			_strupr_s( szHexMD5, sizeof(szHexMD5) ); // CJIJ값이 대문자이다.

			if( strcmp( szHexMD5, szDecryptData ) == 0 )
				m_bDecryptLoginKey = true;
			else
				m_bDecryptLoginKey = false;
		}
	}

	return true;
}

bool ioLocalJapan::DecryptLoginKey( IN const char *szEncLoginKey, IN const char *szUserKey, OUT char *szDecLoginKey, IN int iDecLoginKeySize )
{
	return true;
}

bool ioLocalJapan::IsRightTimeLoginKey( DWORD dwTotalMins )
{
	return m_bRightTimeLoginKey;
}

void ioLocalJapan::ApplyConnect( IN User *pUser, IN SP2Packet &rkPacket )
{

}

bool ioLocalJapan::SendLoginData( User *pUser )
{
	return false;
}

bool ioLocalJapan::IsRightLoginKey( const char *szDBKey, const char *szDecryptKey )
{
	return m_bDecryptLoginKey;
}

void ioLocalJapan::FillRequestGetCash( IN User *pUser, IN SP2Packet &rkPacket )
{
}


void ioLocalJapan::SendRefundCash( User *pUser, int iTransactionID, bool bRefund )
{

}

void ioLocalJapan::SendUserInfo( User *pUser )
{

}

int ioLocalJapan::GetFirstIDMaxSize()
{
	return 12;
}

const char * ioLocalJapan::GetGuildMasterPostion()
{
	return GUILD_MASTER_POSTION_JP;
}

const char * ioLocalJapan::GetGuildSecondMasterPosition()
{
	return GUILD_SECOND_MASTER_POSTION_JP;
}

const char * ioLocalJapan::GetGuildGeneralPosition()
{
	return GUILD_GENERAL_POSTION_JP;
}

bool ioLocalJapan::IsCheckKorean()
{
	return true;
}

bool ioLocalJapan::IsChannelingID()
{
	return false;
}

bool ioLocalJapan::IsBillingTestID( const ioHashString &rsPublicID )
{
	return false;
}

void ioLocalJapan::GetGuildTitle( OUT char *szTitle, IN int iTitleSize, IN const char *szName )
{
	StringCbPrintf( szTitle, iTitleSize, "%s.", szName );
}

bool ioLocalJapan::IsSamePCRoomUser()
{
	return true;
}

int ioLocalJapan::GetLimitGradeLevel()
{
	return -1;
}

bool ioLocalJapan::IsBadPingKick( bool bLadder )
{
	return true;
}

void ioLocalJapan::GetHexMD5( OUT char *szHexMD5, IN int iHexSize, IN const char *szSource )
{
	enum { MAX_DIGEST = 16, };
	MD5Context md5_ctx;
	BYTE byDigest[MAX_DIGEST];

	MD5Init( &md5_ctx );
	MD5Update( &md5_ctx, (unsigned char const *)szSource, strlen( szSource ) );
	MD5Final( byDigest, &md5_ctx );

	for (int i = 0; i < MAX_DIGEST ; i++)
	{
		char szTempHex[MAX_PATH]="";
		StringCbPrintf(szTempHex, sizeof( szTempHex ), "%02x", byDigest[i]); // BYTE 캐스팅해서 FFFF붙지 않는다.
		StringCbCat( szHexMD5, iHexSize, szTempHex );	
	}
}

int ioLocalJapan::GetLicenseDate()
{
	return 20150806;
}

void ioLocalJapan::SendCancelCash( IN SP2Packet &rkPacket )
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
	DWORD        dwGoodsNo;
	ioHashString szGoodsName;
	ioHashString szPublicID;

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
	rkPacket >> szPrivateID;
	rkPacket >> szOrderNo;
	rkPacket >> szUserIP;
	rkPacket >> dwGoodsNo;
	rkPacket >> szGoodsName;
	rkPacket >> szPublicID;


	//--------------------------------SEND------------------------------------//
	char szTempGUID[USER_GUID_NUM_PLUS_ONE]="";
	Help::GetGUID( szTempGUID, sizeof(szTempGUID) );
	SP2Packet kBillingPacket( BSTPK_CANCEL_CASH );
	// 공통
	kBillingPacket << iChannelingType;
	kBillingPacket << szTempGUID;
	kBillingPacket << dwUserIndex;

	// Cancel Step 3
	kBillingPacket << iPayAmt;
	kBillingPacket << szPrivateID;
	kBillingPacket << szOrderNo;
	kBillingPacket << szUserIP;
	kBillingPacket << dwGoodsNo;
	kBillingPacket << szGoodsName;
	kBillingPacket << szPublicID;

	if( !g_BillingRelayServer.SendMessage( kBillingPacket ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[GOLD_INFO]%s Send Fail : %d:%s:%s:%s", __FUNCTION__, dwUserIndex, szPrivateID.c_str(), szBillingGUID.c_str(), szTempGUID );
	}
	else
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[GOLD_INFO]%s Send : %d:%s:%s:%s", __FUNCTION__, dwUserIndex, szPrivateID.c_str(), szBillingGUID.c_str(), szTempGUID );
	}
}

bool ioLocalJapan::UpdateOutputCash( User *pUser, SP2Packet &rkRecievePacket, int iPayAmt, DWORD dwErrorPacketID, DWORD dwErrorPacketType )
{
	if( !pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pUser == NULL.", __FUNCTION__ );
		return false;
	}

	pUser->AddCash( -iPayAmt );
	pUser->AddPurchasedCash( -iPayAmt );

	return true;
}