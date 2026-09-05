#include "stdafx.h"
#include ".\iolocalkorea.h"
//#include "../Window.h"
#include "../MainProcess.h"
#include "../Util/ioEncrypted.h"
#include <strsafe.h>
#include "../Channeling/ioChannelingNodeParent.h"
#include "../NodeInfo/User.h"
#include "../Channeling/ioChannelingNodeManager.h"
#include "../BillingRelayServer/BillingRelayServer.h"

ioLocalKorea::ioLocalKorea(void)
{
}

ioLocalKorea::~ioLocalKorea(void)
{
}

ioLocalManager::LocalType ioLocalKorea::GetType()
{
	return ioLocalManager::LCT_KOREA;
}

const char * ioLocalKorea::GetTextListFileName()
{
	return "text.txt";
}

bool ioLocalKorea::ParseLoginData( IN ioHashString &rsEncLoginKeyAndID, OUT char *szEncLoginKey, IN int iEncLoginKeySize, OUT char *szPrivateID, IN int iPrivaiteIDSize )
{
	// extract ID
#ifdef __OHTG_LOGIN_IP_CHECK__
	char szCipherID[ENC_LOGIN_KEY_NUM+1]="";
	int iLength = rsEncLoginKeyAndID.Length();
	int iCipherIDCnt = 0;
	for (int i = ENC_LOGIN_KEY_NUM; i <= iLength ; i++)
	{
		szCipherID[iCipherIDCnt] = rsEncLoginKeyAndID.At(i);
		iCipherIDCnt++;
		if(iCipherIDCnt >= ENC_LOGIN_KEY_NUM+1)
			break;
	}
#else //__OHTG_LOGIN_IP_CHECK__
	char szCipherID[ENC_ID_NUM_PLUS_ONE]="";
	int iLength = rsEncLoginKeyAndID.Length();
	int iCipherIDCnt = 0;
	for (int i = ENC_LOGIN_KEY_NUM; i < iLength ; i++)
	{
		szCipherID[iCipherIDCnt] = rsEncLoginKeyAndID.At(i);
		iCipherIDCnt++;
		if(iCipherIDCnt >= ENC_ID_NUM_PLUS_ONE)
			break;
	}
#endif //__OHTG_LOGIN_IP_CHECK__

	// save Enc login key
	StringCbCopyN(szEncLoginKey, iEncLoginKeySize, rsEncLoginKeyAndID.c_str(), ENC_LOGIN_KEY_NUM );

	if( iPrivaiteIDSize != DATA_LEN )
		return false;

	if(!ioEncrypted::Decode15(szCipherID, (char*)g_App.GetSecondKey().c_str(), szPrivateID))
		return false;

	return true;
}

bool ioLocalKorea::DecryptLoginKey( IN const char *szEncLoginKey, IN const char *szUserKey, OUT char *szDecLoginKey, IN int iDecLoginKeySize )
{
	return ioEncrypted::Decode15((char*)szEncLoginKey, (char*)szUserKey, szDecLoginKey );
}

bool ioLocalKorea::IsRightTimeLoginKey( DWORD dwTotalMins )
{
#ifdef _DEBUG
	return true;
#endif

	enum { MAX_LIVE_MINUTES = 30 };

	if( dwTotalMins >= MAX_LIVE_MINUTES )
		return false;

	return true;
}

void ioLocalKorea::ApplyConnect( IN User *pUser, IN SP2Packet &rkPacket )
{

}

bool ioLocalKorea::SendLoginData( User *pUser )
{

	SP2Packet kPacket( BSTPK_FIRST_LOGIN );
	kPacket.Write( pUser->GetPrivateID() );
	kPacket.SetPosBegin();

	g_BillingRelayServer.SendMessage( kPacket );

	return false;
}

bool ioLocalKorea::IsRightLoginKey( const char *szDBKey, const char *szDecryptKey )
{
	if( strcmp( szDBKey, szDecryptKey ) == 0 )
		return true;

	return false;
}

void ioLocalKorea::FillRequestGetCash( IN User *pUser, IN SP2Packet &rkPacket )
{

}

void ioLocalKorea::SendRefundCash( User *pUser, int iTransactionID, bool bRefund )
{

}

void ioLocalKorea::SendUserInfo( User *pUser )
{

}

int ioLocalKorea::GetFirstIDMaxSize()
{
	return 12;
}

const char * ioLocalKorea::GetGuildMasterPostion()
{
	return "길드장";
}

const char * ioLocalKorea::GetGuildSecondMasterPosition()
{
	return "부길드장";
}

const char * ioLocalKorea::GetGuildBuilderPosition()
{
	return "Admin";
}

const char * ioLocalKorea::GetGuildGeneralPosition()
{
	return "길드원";
}

bool ioLocalKorea::IsCheckKorean()
{
	return true;
}

bool ioLocalKorea::IsChannelingID()
{
	return true;
}

bool ioLocalKorea::IsBillingTestID( const ioHashString &rsPublicID )
{
	// 아래 "금벌", "금벌인", "금벌진" 아이디 수정시 반드시 대윤씨에게 문의해주세요.
	// 금벌 : 일반, 금벌인 : 엠게임, galura17 : 네이버 ( byosa190 / 로사테스트1@ ), 금벌진 : 버디버디, 금벌금 : 투니랜드 ( cash6 /cash6 )
	if( rsPublicID == "금벌" || rsPublicID == "금벌인" || rsPublicID == "galura17" || rsPublicID == "금벌진" || rsPublicID == "금벌금" )
		return true;

	return false;
}

void ioLocalKorea::GetGuildTitle( OUT char *szTitle, IN int iTitleSize, IN const char *szName )
{
	StringCbPrintf( szTitle, iTitleSize, "%s길드입니다.", szName );
}

bool ioLocalKorea::IsSamePCRoomUser()
{
	return true;
}

int ioLocalKorea::GetLimitGradeLevel()
{
	return -1;
}

bool ioLocalKorea::IsBadPingKick( bool bLadder )
{
	return true;
}

int ioLocalKorea::GetLicenseDate()
{
	return 30111131;
}


void ioLocalKorea::SendCancelCash( IN SP2Packet &rkPacket )
{
	if( rkPacket.GetPacketID() != BSTPK_OUTPUT_CASH_RESULT )
		return;

	int          iChannelingType = 0;
	ioHashString szBillingGUID;
	DWORD        dwUserIndex = 0;
	int          iPayAmt  = 0;
	int          iType    = 0;
	int          iReturnValue = 0;
	int          TransactionID = 0;

	rkPacket >> dwUserIndex;
	rkPacket >> szBillingGUID;
	rkPacket >> iReturnValue;
	
	if( iReturnValue != CASH_RESULT_SUCCESS )
		return;

	rkPacket >> iType;
	rkPacket >> iPayAmt;
	rkPacket >> TransactionID;
	int iItemValueList[ioChannelingNodeParent::MAX_ITEM_VALUE];
	for (int i = 0; i < ioChannelingNodeParent::MAX_ITEM_VALUE ; i++)
		iItemValueList[i] = 0;
	ioChannelingNodeParent::GetItemValueList( rkPacket, iType, iItemValueList );
	rkPacket >> iChannelingType;

	ioChannelingNodeParent *pNode = g_ChannelingMgr.GetNode( (ChannelingType)iChannelingType );
	if( pNode )
	{
		rkPacket.SetPosBegin();
		pNode->SendCancelCash( rkPacket );
	}
}

bool ioLocalKorea::UpdateOutputCash( User *pUser, SP2Packet &rkRecievePacket, int iPayAmt, DWORD dwErrorPacketID, DWORD dwErrorPacketType )
{
	if( !pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pUser == NULL.", __FUNCTION__ );
		return false;
	}

	ioChannelingNodeParent *pNode = g_ChannelingMgr.GetNode( pUser->GetChannelingType() );
	if( !pNode )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pNode == NULL.", __FUNCTION__ );
		return false;
	}
	
	if( !pNode->UpdateOutputCash( pUser, rkRecievePacket, iPayAmt, dwErrorPacketID, dwErrorPacketType ) )
		return ioLocalParent::UpdateOutputCash( pUser, rkRecievePacket, iPayAmt, dwErrorPacketID, dwErrorPacketType ); // Default

	return true;
}