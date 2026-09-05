
#include "stdafx.h"
#include "CardMatching.h"

#include "../EtcHelpFunc.h"
#include "../QueryData/QueryResultData.h"
#include "../DataBase/DBClient.h"
#include "CardMatchingManager.h"
#include "../DataBase/LogDBClient.h"
#include "../MainProcess.h"
#include "../Util/IORandom.h"
#include "UserNodeManager.h"
#include "../MainServerNode/MainServerNode.h"

CardMatching::CardMatching()
{
	Init();
}

CardMatching::~CardMatching()
{
	Destroy();
}

void CardMatching::Init()
{
	m_byCardMissionType = 0;	// 카드미션 타입 
	m_byCardMissionMark1 = 0;	// 카드미션 레드
	m_byCardMissionMark2 = 0;	// 카드미션 블루
	m_byCardMark1 = 0;			// 진행 중 카드미션 레드
	m_byCardMark2 = 0;			// 진행 중 카드미션 블루

	m_iPairSuccessCount = 0;
	m_bTimeCheckStart	= false;
}

void CardMatching::Destroy()
{
	m_iPairSuccessCount = 0;	
	m_bTimeCheckStart			= false;
}

void CardMatching::Initialize( User *pUser )
{
	SetUser( pUser );
	Init();
}

void CardMatching::Reset()
{
	m_iPairSuccessCount = 0;
	SetTimeCheckFlag(false);
	SetCardMatchingTimeStamp(0);
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void CardMatching::SetSourceCard()			// 15개의 소스 카드 선택 SourceCard
{
	for(int i=0; i<15; i++)
	{
		m_stSourceCard[i].CardType = NORMAL_CARD_TYPE;
		m_stSourceCard[i].nCardState = CARD_NOT_CHECKED;
		m_stSourceCard[i].nCardCode = i;
	}	
	return;
}

bool CardMatching::GetMatchedSpecialCardsCount(int& nBlueCard, int& nRedCard) 
{
	nBlueCard = 0;
	nRedCard = 0;

	for( int i= 0;i<30 ;i++)
	{
		if( m_stMixedCardList[i].CardType == SPECIAL_CARD_TYPE_BLUE && m_stMixedCardList[i].nCardState ==CARD_MATCHED_DONE )
			nBlueCard++;
		if ( m_stMixedCardList[i].CardType == SPECIAL_CARD_TYPE_RED && m_stMixedCardList[i].nCardState ==CARD_MATCHED_DONE )
			nRedCard++;
	}	

	nBlueCard = nBlueCard / 2; 
	nRedCard = nRedCard / 2; 

	if(  (nBlueCard+nRedCard) > 0	 )
		return true;
	
	return false;
}


void CardMatching::InsertSpecialCard()			// 15개의 소스 카드 선택 SourceCard			/SPECIAL_CARD_TYPE_BLUE
{
	int iCount = rand()%2 + 1;		// 1개 또는 2개 
	for( int i=0; i<iCount; i++)
	{
		int nCardType = rand()%2;
		Card_Type _CardType = SPECIAL_CARD_TYPE_BLUE;
		if( nCardType == 0)
		{
			_CardType = SPECIAL_CARD_TYPE_RED;
		}

		int nCardSlot = rand()%15;
		m_stSourceCard[nCardSlot].CardType = _CardType;
		m_stSourceCard[nCardSlot].nCardCode = _CardType;
	}

#ifdef CARD_MATCHING_BY_BCKIM_DEBUG	
	User* pTEmpUser = GetUser();
	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"[CARD_MATCHING]InsertSpecialCard IDX[%s][%d]", pTEmpUser->GetPublicID().c_str(),iCount);
#endif	// CARD_MATCHING_BY_BCKIM_DEBUG

	return;
}

void CardMatching::DetermineTheMissionData()	// 달성해야 할 미션 결정 
{	
	int rnd_type = rand()%2;				// 50% ( 3 or 4)
	m_byCardMissionType = rnd_type + 3;		// 미션 타입이 3 또는 4 
	m_byCardMissionMark1 = (BYTE)(rand()%m_byCardMissionType);	
	m_byCardMissionMark2 = max(m_byCardMissionType - m_byCardMissionMark1, 0);	

#ifdef CARD_MATCHING_BY_BCKIM_DEBUG
	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"[CARD_MATCHING] DetermineTheMissionData[Type:%d][Mark1:%d][Mark2:%d]", m_byCardMissionType,m_byCardMissionMark1,m_byCardMissionMark2);
#endif	// CARD_MATCHING_BY_BCKIM_DEBUG

	User* tempUser = GetUser();
	if( tempUser )
	{
		g_LogDBClient.OnInsertCatdMatchingInfo( tempUser, CARD_MATCHING_NEW_MISSION, m_byCardMissionType,m_byCardMissionMark1,m_byCardMissionMark2,m_byCardMark1,m_byCardMark2 , 0, 0, 0, 0 );	
	}
	return;
}

void CardMatching::PreDoneMissionFinish()
{
	User* pTempUser = GetUser();
	if( pTempUser == NULL )	return;

#ifdef CARD_MATCHING_BY_BCKIM_DEBUG	
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"[CARD_MATCHING] BEFORE >> PreDoneMissionFinish IDX[%s][%d][%d][%d][%d][%d]", pTempUser->GetPublicID().c_str(),\
		m_byCardMissionType,m_byCardMissionMark1,m_byCardMissionMark2,m_byCardMark1,m_byCardMark2);
#endif	// CARD_MATCHING_BY_BCKIM_DEBUG


	// 미션 완료 		// 특별 보상 지급 // 조건은 m_byCardMissionType, m_byCardMissionMark1, m_byCardMissionMark2 
	if( m_byCardMissionType == (m_byCardMark1+m_byCardMark2) && m_byCardMissionMark1 == m_byCardMark1 && m_byCardMissionMark2 == m_byCardMark2 )
	{ 
#ifdef CARD_MATCHING_BY_BCKIM_DEBUG
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"[CARD_MATCHING] ================PreDoneMissionFinish===============");
#endif	// CARD_MATCHING_BY_BCKIM_DEBUG

		DetermineTheMissionData();
		SetCardMatchingMarkData( 0,0);			// 현재 유저가 진행중인 데이터 
		SaveData(CARD_MATCHING_NEW_MISSION);
	}
}

bool CardMatching::SetMixedCardList()		// 소스카드 * 2 >> 30 삽입	MixedCardList
{
	for(int i=0; i<15;i++)
	{
		m_stMixedCardList[i] = m_stSourceCard[i];
		m_stMixedCardList[i+15] = m_stSourceCard[i];
	}
	ReOrderCard(40);
	return true;
}
bool CardMatching::ReOrderCard(int nCount)				// 랜덤 재배열 ( 횟수 ) 
{
	srand( timeGetTime() );	
	nCount = max(nCount,0);
	for(int i=0;i<nCount;i++)
	{
		DWORD rnd = rand()%30;
		DWORD rnd_2 = rand()%30;
		// DWORD rnd_2 = (4+rnd)%30;

		CardInfo stTemp;
		stTemp = m_stMixedCardList[rnd_2];
		m_stMixedCardList[rnd_2] = m_stMixedCardList[rnd];
		m_stMixedCardList[rnd] = stTemp;				
	}

/*
#ifdef CARD_MATCHING_BY_BCKIM_DEBUG
	User* tempUser = GetUser();
	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"[CARD_MATCHING]ReOrderCard>>Clent[%s]", tempUser->GetPublicID().c_str());
	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"[CARD_MATCHING]======================================================");

	for(int i=0; i<30; i+=6)
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"[CARD_MATCHING]== [%2d][%2d][%2d][%2d][%2d][%2d] ==",m_stMixedCardList[i].nCardCode,m_stMixedCardList[i+1].nCardCode,\
			m_stMixedCardList[i+2].nCardCode,m_stMixedCardList[i+3].nCardCode,m_stMixedCardList[i+4].nCardCode,m_stMixedCardList[i+5].nCardCode);
	}
	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"[CARD_MATCHING]======================================================");

	for(int i=0; i<30; i++)
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"[CARD_MATCHING][%2d][%2d][%2d][%2d]",i,m_stMixedCardList[i].CardType,m_stMixedCardList[i].nCardState,m_stMixedCardList[i].nCardCode);
	}
#endif	// CARD_MATCHING_BY_BCKIM_DEBUG
*/

	return true;
}

int CardMatching::ConfirmCard_Single(BYTE byConfirmCard1)
{
	if( m_stMixedCardList[byConfirmCard1].nCardState == CARD_NOT_CHECKED )
	{
		m_stMixedCardList[byConfirmCard1].nCardState = CARD_CONFIRMED;			// 의미 있음 ? 
		return m_stMixedCardList[byConfirmCard1].nCardCode;				
	}
	return m_stMixedCardList[byConfirmCard1].nCardCode;
}

CardMatchingCommandResult CardMatching::ConfirmCard(BYTE byConfirmCard1, BYTE byConfirmCard2 )	// 카드 확인( 1개씩 / 클라는 한번에 1 개 또는 2개씩 줌 / 2번 호출하면서 예외처리 )
{
	if( byConfirmCard1 != byConfirmCard2 )
	{
		if( m_stMixedCardList[byConfirmCard1].nCardCode == m_stMixedCardList[byConfirmCard2].nCardCode )
		{
			m_stMixedCardList[byConfirmCard1].nCardState = CARD_MATCHED_DONE;
			m_stMixedCardList[byConfirmCard2].nCardState = CARD_MATCHED_DONE;
			m_iPairSuccessCount++;
			return CARD_MATCHING_CARD_MATCHED_DONE; 
		}
		else		
		{
			m_stMixedCardList[byConfirmCard1].nCardState = CARD_CONFIRMED;
			m_stMixedCardList[byConfirmCard2].nCardState = CARD_CONFIRMED;			
			return CARD_MATCHING_CONFIRM_CARD; 
		}
	}
	return CARD_MATCHING_CONFIRM_NONE;
}

void CardMatching::SendMatchingDoneResult( CardMatchingCommandResult ResultID)	
{
	User* TempUser = GetUser();
	if( TempUser != NULL )
	{
		SP2Packet kPacket( STPK_CARD_MATCHING_END_RESULT );
		PACKET_GUARD_VOID( kPacket.Write( ResultID ));	
		CardMatchingDataFill( kPacket);		
		PACKET_GUARD_VOID( kPacket.Write( GetMatchingCount()));
		TempUser->SendMessage( kPacket );
	}
}

bool CardMatching::MatchEndProcess(CardMatchingCommandResult iEndType)	// 미션 진행 상황 처리 , 일반 보상 지급 , DB 저장 및 로그  
{
	User* TempUser = GetUser();
	if( TempUser == NULL || GetTimeCheckFlag() == false )
		return false;

	SetTimeCheckFlag(false);
	int nBlueCard = 0;
	int nRedCard = 0;	
	bool bExsit = GetMatchedSpecialCardsCount(nBlueCard,nRedCard);
	if( bExsit )
	{ 
		m_byCardMark1 = min( m_byCardMissionMark1, m_byCardMark1 + nBlueCard );
		m_byCardMark2 = min( m_byCardMissionMark2, m_byCardMark2 + nRedCard );
		g_LogDBClient.OnInsertCatdMatchingInfo( TempUser, CARD_MATCHING_IN_PROGRESS_UPDATE,m_byCardMissionType,m_byCardMissionMark1,m_byCardMissionMark2,m_byCardMark1,m_byCardMark2,0, 0, 0, 0 );			
			
		if( m_byCardMissionType == (m_byCardMark1+m_byCardMark2) && m_byCardMissionMark1 == m_byCardMark1 && m_byCardMissionMark2 == m_byCardMark2 )
		{// 목표 미션   ==  // 진행중인 미션 
			SendCardMatchingReward(TempUser,GetMatchingCount(), true, m_byCardMissionType,m_byCardMissionMark1,m_byCardMissionMark2);		
		}
	}

	if( GetMatchingCount() > 3 )		// 일반 보상 지급  // 보상 로그는 안에 있음. 
		SendCardMatchingReward(TempUser,GetMatchingCount(), false ,0,0,0);			

#ifdef CARD_MATCHING_BY_BCKIM_DEBUG
	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"[CARD_MATCHING] MatchEndProcess >> ClentID[%s] [%d][%d][%d] : [%d][%d]",TempUser->GetPublicID().c_str(),m_byCardMissionType,m_byCardMissionMark1,m_byCardMissionMark2, m_byCardMark1, m_byCardMark2 );
#endif	// CARD_MATCHING_BY_BCKIM_DEBUG

	SaveData(CARD_MATCHING_IN_PROGRESS_UPDATE);		// DB 저장 update 
	SendMatchingDoneResult( iEndType );				// 최종 결과 전송 
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

bool CardMatching::DBtoNewIndex( DWORD dwIndex )
{	
	return true;
}

void CardMatching::DBtoData( CQueryResultData *query_data )
{
}

void CardMatching::SaveData()
{
}
void CardMatching::SaveData(CardMatchingUpdateType iType)
{
	User* pUser = GetUser();
	if( pUser == NULL )
		return;

	g_DBClient.OnUpdateCardMatchingData( pUser->GetUserDBAgentID()
		, pUser->GetAgentThreadID()
		, pUser->GetUserIndex()
		, iType
		, m_byCardMissionType 
		, m_byCardMissionMark1
		, m_byCardMissionMark2
		, m_byCardMark1
		, m_byCardMark2 );
}


void CardMatching::FillMoveData( SP2Packet &rkPacket )
{
}


void CardMatching::ApplyMoveData( SP2Packet &rkPacket, bool bDummyNode )
{
}

// S->C 로그인 시 카드 매칭 정보 전송
void CardMatching::CardMatchingDataFill( SP2Packet &rkPacket )
{
	PACKET_GUARD_VOID( rkPacket.Write( m_byCardMissionType ) );
	PACKET_GUARD_VOID( rkPacket.Write( m_byCardMissionMark1 ) );
	PACKET_GUARD_VOID( rkPacket.Write( m_byCardMissionMark2 ) );
	PACKET_GUARD_VOID( rkPacket.Write( m_byCardMark1 ) );
	PACKET_GUARD_VOID( rkPacket.Write( m_byCardMark2) );

#ifdef CARD_MATCHING_BY_BCKIM_DEBUG	
	User* pTEmpUser = GetUser();
	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"[CARD_MATCHING]SendCardMatchingData IDX[%s][%d][%d][%d][%d][%d]", pTEmpUser->GetPublicID().c_str(),\
		m_byCardMissionType,m_byCardMissionMark1,m_byCardMissionMark2,m_byCardMark1,m_byCardMark2);
#endif	// CARD_MATCHING_BY_BCKIM_DEBUG
}

// S->C 요청 된 보상 전송
void CardMatching::SendCardMatchingReward( User *pUser, int nMatchingCount, bool bIsSpecialReward, BYTE byMissionType, BYTE byBlueMark, BYTE byRedMark)
{
	if( pUser == NULL )
		return;

	int iType = 0;
	DWORD dwValue1 = 0;
	DWORD dwValue2 = 0;
	int RewardStep = 0;

	// 보상 상품 선물함 전송
	CardMatchingManager::mapAllReward::iterator iter; 
	CardMatchingManager::mapAllReward &LuckyReward = g_CardMatchingMgr.GetMapCardMatchingLuckyReward();
	CardMatchingManager::mapAllReward &mapSectionReward = g_CardMatchingMgr.GetMapCardMatchingSectionReward();

	if( bIsSpecialReward )		// 특별 보상 
	{
		if( byMissionType == 4 )
			RewardStep = byMissionType + byBlueMark;			// 4,5,6,7,8
		else
			RewardStep = byBlueMark;							// 0,1,2,3
		
		iter = LuckyReward.find( RewardStep );
	}
	else	// 일반보상 
	{	// nMatchingCount			3,7,15  //  #define COMPARE(x,min,max) (((x)>=(min))&&((x)<(max)))
		if( COMPARE(nMatchingCount,3,6) )
			RewardStep = 0;
		else if ( COMPARE(nMatchingCount,7,14) )
			RewardStep = 1;	
		else if( nMatchingCount == 15)
			RewardStep = 2;	
	
		iter = mapSectionReward.find( RewardStep );
	}

	iType = iter->second.m_iType;
	dwValue1 = iter->second.m_dwValue1;
	dwValue2 = iter->second.m_dwValue2;

	CTimeSpan cPresentGapTime( g_CardMatchingMgr.GetPeriod(), 0, 0, 0 );		// 보관 기간 ini 
	CTime kPresentTime = CTime::GetCurrentTime() + cPresentGapTime;				// 보관 기간 설정 

	pUser->AddPresentMemory( g_MainServer.GetSendID(), iType, dwValue1, dwValue2, 0, 0, g_CardMatchingMgr.GetMent(), kPresentTime, g_CardMatchingMgr.GetState() );
	pUser->SendPresentMemory();

	int iLogType = CARD_MATCHING_NORMAL_REWARD;
	if( bIsSpecialReward )
		iLogType = CARD_MATCHING_MISSION_REWARD;

	g_LogDBClient.OnInsertCatdMatchingInfo( pUser, iLogType, m_byCardMissionType,m_byCardMissionMark1,m_byCardMissionMark2,\
		m_byCardMark1,m_byCardMark2,RewardStep, iType, dwValue1, dwValue2 );	
}

void CardMatching::SetCardMatchingMarkData( BYTE Mark1, BYTE Mark2)
{
	m_byCardMark1 = Mark1;					// 진행중인 카드미션 레드
	m_byCardMark2 = Mark2;					// 진행중인 카드미션 블루

#ifdef CARD_MATCHING_BY_BCKIM_DEBUG
	User* pTempUser = GetUser();
	if(pTempUser != NULL)
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"[CARD_MATCHING] SetCardMatchingMarkData ID[%s]>>>MARK1[%d], MARK2[%d]",pTempUser->GetPublicID().c_str(),m_byCardMark1,m_byCardMark2);
#endif	// CARD_MATCHING_BY_BCKIM_DEBUG
}

void CardMatching::SetCardMatchingMissionData(BYTE CardMissionType, BYTE MissionMark1, BYTE MissionMark2)
{
	m_byCardMissionType = CardMissionType;	// 카드미션 타입 
	m_byCardMissionMark1 = MissionMark1;	// 카드미션 레드
	m_byCardMissionMark2 = MissionMark2;	// 카드미션 블루

#ifdef CARD_MATCHING_BY_BCKIM_DEBUG
	User* pTempUser = GetUser();
	if(pTempUser != NULL)
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"[CARD_MATCHING] SetCardMatchingMissionData ID[%s]>>>TYPE[%d]1>>[%d]2>>[%d]", pTempUser->GetPublicID().c_str(),m_byCardMissionType,m_byCardMissionMark1,m_byCardMissionMark2 );
#endif	// CARD_MATCHING_BY_BCKIM_DEBUG
}
