#pragma once

class CardMatching : public ioDBDataController
{
private:
	BYTE	m_byCardMissionType;					// 카드미션 타입 
	BYTE	m_byCardMissionMark1;					// 카드미션 레드
	BYTE	m_byCardMissionMark2;					// 카드미션 블루
	BYTE	m_byCardMark1;						// 진행 중 카드미션 레드
	BYTE	m_byCardMark2;						// 진행 중 카드미션 블루

	CardInfo m_stSourceCard[15];				// 소스카드 15장 
	CardInfo m_stMixedCardList[30];				// 확인 카드 30장
		
	int		m_iPairSuccessCount;				// pair 매칭된 카드 카운드 (쌍 ) 		
	DWORD	m_dwCardMatching_Time;				// 매칭 게임 시작 시간 틱. 
	bool	m_bTimeCheckStart;					// 타임 체크 플래그 

public:
	void SetSourceCard();											// 15개의 소스 카드 선택 SourceCard
	bool GetMatchedSpecialCardsCount(int& nBCard, int& nRcard); 

	void DetermineTheMissionData();									// 달성해야 할 미션 결정 
	void PreDoneMissionFinish();									// 이전 완료 미션 정리(?) 새로운 미션 정보 

	void InsertSpecialCard();								// 15개의 소스 카드 선택 SourceCard			/SPECIAL_CARD_NONE
	bool SetMixedCardList();										// 소스카드 * 2 >> 30 삽입	MixedCardList
	bool ReOrderCard(int nCount);				// 랜덤 재배열 ( 횟수 ) 

	void SetTimeCheckFlag(bool bFlag){m_bTimeCheckStart = bFlag;} ;	// 타임 체크 flag 설정 SetTimeCheckFlag();	// 타임 체크 flag 설정 
	bool GetTimeCheckFlag(){ return m_bTimeCheckStart; }
	
	int ConfirmCard_Single(BYTE byConfirmCard1);
	CardMatchingCommandResult ConfirmCard(BYTE byComfirmCard1, BYTE byComfirmCard2 );	// 카드 확인( 1개씩 / 클라는 한번에 1 개 또는 2개씩 줌 / 2번 호출하면서 예외처리 )
	
	bool MatchEndProcess(CardMatchingCommandResult iEndType );
	
	void SetCardMatchingMissionData(BYTE CardMissionType, BYTE MissionMark1, BYTE MissionMark2);		// 유저가 달성해야 하는 데이터 
	void SetCardMatchingMarkData( BYTE Mark1, BYTE Mark2);			// 현재 유저가 진행중인 데이터 
	void CardMatchingDataFill( SP2Packet &rkPacket );

	DWORD GetCardMatchingTimeStamp(){ return m_dwCardMatching_Time; }
	void SetCardMatchingTimeStamp( DWORD dwTime ){ m_dwCardMatching_Time = dwTime; }

	int GetCardCode(BYTE byCardSlotNum){ return m_stMixedCardList[byCardSlotNum].nCardCode; }
	int GetCardState(BYTE byCardSlotNum){ return m_stMixedCardList[byCardSlotNum].nCardState; }
	int GetMatchingCount(){ return m_iPairSuccessCount; }

	void SendMatchingDoneResult( CardMatchingCommandResult ResultID );	
	void SaveData(CardMatchingUpdateType iType );

	//////////////////////////////////////////////////////////////////////////////////////////

public:
	CardMatching();
	virtual ~CardMatching();

	void Init();
	void Destroy();

public:
	virtual void Initialize( User *pUser );
	void Reset();
	virtual bool DBtoNewIndex( DWORD dwIndex );
	virtual void DBtoData( CQueryResultData *query_data );
	virtual void SaveData();	
	virtual void FillMoveData( SP2Packet &rkPacket );
	virtual void ApplyMoveData( SP2Packet &rkPacket, bool bDummyNode = false  );
	
public:
	User* GetUser(){ return m_pUser; }
	void SetUser( User* pUser ){ m_pUser = pUser; }

	// S->C 요청 된 보상 전송
	void SendCardMatchingReward( User *pUser,int nMatchingCount, bool bIsSpecialReward = false, BYTE byMissionType = 0, BYTE byBlueMark = 0, BYTE byRedMark = 0);
};
