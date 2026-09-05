#include "stdafx.h"
#include "ioLocalEU.h"

#include "../NodeInfo/User.h"
#include "../BillingRelayServer/BillingRelayServer.h"
#include "../EtcHelpFunc.h"
#include "../MainProcess.h"
#include "../Channeling/ioChannelingNodeParent.h"
#include "../NodeInfo/UserNodeManager.h"

ioLocalEU::ioLocalEU(void)
{
	m_EUCountryCCUMap.clear();
}

ioLocalEU::~ioLocalEU(void)
{
}

ioLocalManager::LocalType ioLocalEU::GetType()
{
	return ioLocalManager::LCT_EU;
}

const char * ioLocalEU::GetTextListFileName()
{
	return "text_eu.txt";
}

bool ioLocalEU::ParseLoginData( IN ioHashString &rsEncLoginKeyAndID, OUT char *szEncLoginKey, IN int iEncLoginKeySize, OUT char *szPrivateID, IN int iPrivaiteIDSize )
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
bool ioLocalEU::DecryptLoginKey( IN const char *szEncLoginKey, IN const char *szUserKey, OUT char *szDecLoginKey, IN int iDecLoginKeySize )
{
	return true;
}

bool ioLocalEU::IsRightLoginKey( const char *szDBKey, const char *szDecryptKey )
{
	return true;
}

bool ioLocalEU::IsRightTimeLoginKey( DWORD dwTotalMins )
{
	return true;
}

void ioLocalEU::ApplyConnect( IN User *pUser, IN SP2Packet &rkPacket )
{
	if( !pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pUser == NULL.", __FUNCTION__ );
		return;
	}
	ioHashString szNexonPassPort;
	szNexonPassPort.Clear();


	rkPacket >> szNexonPassPort;

	
    // nexonPassPort 값 
	pUser->SetBillingUserKey( szNexonPassPort );
}

//유럽일경우 빌링에서 인증 후 추가로 데이터 보냄
bool ioLocalEU::ApplyLogin( IN User *pUser, IN SP2Packet &rkPacket )
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
	ioHashString sNexonID;
	__int64		 nexonSN;
	int          gender;
	int          age;
	ioHashString nationCode;
	

	rkPacket >> sReturnValue >> sNexonID >> nexonSN >> gender >> age >> nationCode;
	

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
	*/
	// real private id로 교체
	pUser->SetPrivateID( sPrivateID );		//HR 빌링에서 받음
	pUser->SetDBAgentID( Help::GetUserDBAgentID( pUser->GetPrivateID() ) );
	
	pUser->SetNexonEUID( sNexonID );
	pUser->SetNexonSN( nexonSN );
	pUser->SetEUGender( gender );
	pUser->SetEUAge( age );
	//pUser->SetCountry( nationCode );
	
	//hr EU sql 로 유저 국가 알아내야함
	//SetEUCountryType()
	return true;
}

void ioLocalEU::ApplyRequestLogin( IN SP2Packet &rkPacket, OUT ioHashString &rsValue1, OUT ioHashString &rsValue2 )
{
	rkPacket >> rsValue1; //  passport
	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "%s %s", __FUNCTION__ , rsValue1.c_str() );
}

bool ioLocalEU::SendLoginData( User *pUser )
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

	return true;
}

void ioLocalEU::FillRequestGetCash( IN User *pUser, IN SP2Packet &rkPacket )
{                                                   
	if( !pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pUser == NULL.", __FUNCTION__ );
		return;
	}

	rkPacket << pUser->GetBillingUserKey();
	
}

void ioLocalEU::FillRequestOutputCash( IN User *pUser, IN SP2Packet &rkPacket, const char *szRecvPrivateID )
{
	//구매시 필요한 데이터 추가로 넣어줌
	if( !pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s pUser == NULL.", __FUNCTION__ );
		return;
	}

	rkPacket << pUser->GetNexonEUID();
	rkPacket << pUser->GetEUAge();

}

void ioLocalEU::FillRequestLogin( OUT SP2Packet &rkPacket, IN ioHashString &rsValue1, IN ioHashString &rsValue2 )
{
	rkPacket << rsValue1; // usertype passport
	
	LOG.PrintTimeAndLog( LOG_TEST_LEVEL, "%s %s", __FUNCTION__ , rsValue1.c_str() );
}

void ioLocalEU::SendRefundCash( User *pUser, int iTransactionID, bool bRefund )
{
}

//유저가 가지고 있는 캐쉬 계산
bool ioLocalEU::UpdateOutputCash( User *pUser, SP2Packet &rkRecievePacket, int iPayAmt, DWORD dwErrorPacketID, DWORD dwErrorPacketType )
{
	
	int iBalance = 0;
	iBalance = pUser->GetCash() - iPayAmt;
	pUser->SetCash( iBalance );

	return true;
}

void ioLocalEU::SendUserInfo( User *pUser )
{
}

void ioLocalEU::SendCancelCash( IN SP2Packet &rkPacket )
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

int ioLocalEU::GetFirstIDMaxSize()
{
	return 12;
}

const char * ioLocalEU::GetGuildMasterPostion()
{
	return "Leader";
}

const char * ioLocalEU::GetGuildSecondMasterPosition()
{
	return "Officer";
}

const char * ioLocalEU::GetGuildGeneralPosition()
{
	return "Member";
}

bool ioLocalEU::IsCheckKorean()
{
	return false;
}

bool ioLocalEU::IsChannelingID()
{
	return false;
}

bool ioLocalEU::IsBillingTestID( const ioHashString &rsPublicID )
{
	return false;
}

void ioLocalEU::GetGuildTitle( OUT char *szTitle, IN int iTitleSize, IN const char *szName )
{
	StringCbPrintf( szTitle, iTitleSize, "%s.", szName );
}

bool ioLocalEU::IsSamePCRoomUser()
{
	return false;
}

int ioLocalEU::GetLimitGradeLevel()
{
	return -1;
}

bool ioLocalEU::IsPrivateLowerID()
{	
	return false;
}

bool ioLocalEU::IsBadPingKick( bool bLadder )
{
	return false;
}

int ioLocalEU::GetLicenseDate()
{
	return 20150806;

}

//유저 국가별 ccu 증가
void ioLocalEU::IncreaseCountryCCU( const ioHashString &rsCountryCode )
{
	//m_DBAgentMap.insert( DBAgentNodeMap::value_type( index, pAgentNode ) );
	EUCountryCCUMap::iterator iter = m_EUCountryCCUMap.find( rsCountryCode );

	//데이터가 있는경우 
	if( iter != m_EUCountryCCUMap.end() )
	{
		iter->second++;
	}

	int nCount = 1;
	m_EUCountryCCUMap.insert( EUCountryCCUMap::value_type( rsCountryCode, nCount ) );
}

//hr 유저 로그아웃 & 유저 채널 변경시에 카운트 감소 될 수 있도록 함
void ioLocalEU::DecreaseCountryCCU( const ioHashString &rsCountryCode )
{
	EUCountryCCUMap::iterator iter = m_EUCountryCCUMap.find( rsCountryCode );

	//국가코드가 있는경우 
	if( iter != m_EUCountryCCUMap.end() )
	{
		if( iter->second <= 1 )	//국가 1명이거나 0인경우
		{	
			
			iter->second = 0;
		}
		else
		{
			iter->second--;
		}
	}
}

//hr 국가코드별  ccu 메인서버에 보내주는 값
void ioLocalEU::FillCountryCCU( OUT SP2Packet &rkPacket )
{
	int count = m_EUCountryCCUMap.size();
	
	rkPacket << count;

	EUCountryCCUMap::iterator iter;
	for(iter = m_EUCountryCCUMap.begin();iter != m_EUCountryCCUMap.end() ; ++iter )
	{
		rkPacket << iter->first << iter->second;
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[LSEU_CCU]%s %s, %d", __FUNCTION__, iter->first.c_str(), iter->second );
	}
}