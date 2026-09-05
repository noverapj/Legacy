// DBClient.h: interface for the DBClient class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOGDBCLIENT_H__8BBCE6AF_B8A6_4D7C_A9FA_B2B4B32491CA__INCLUDED_)
#define AFX_LOGDBCLIENT_H__8BBCE6AF_B8A6_4D7C_A9FA_B2B4B32491CA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "../QueryData/QueryData.h"
#include "../Nodeinfo/NodeHelpStructDefine.h"

//DB AGENT MSG TYPE
// GET : Select , SET : Insert , DEL : Delete , UPD : Update

#define LOGDBAGENT_SET          	0x0001 // return 값이 없으니 한개만 존재함
#define DBAGENT_LOG_PINGPONG		0x1000

//작업 방식
#define _INSERTDB       0
#define _DELETEDB       1
#define _SELECTDB       2
#define _UPDATEDB       3   
#define _SELECTEX1DB    4 

//결과 행동
#define _RESULT_CHECK   0
#define _RESULT_NAUGHT  1
#define _RESULT_DESTROY 2

struct ModeRecord;
class User;
class Room;
class CConnectNode;

class LogDBClient : public CConnectNode 
{
private:
	static LogDBClient *sg_Instance;
	DWORD m_dwCurrentTime;
	int   m_iClassPriceTime;

protected:
	std::vector<std::string> m_vServerIP;
	std::vector<int> m_vServerPort;

	ioHashString m_DBAgentIP;
	int          m_iDBAgentPort;

public:
	static LogDBClient &GetInstance();
	static void ReleaseInstance();

private:
	ValueType GetValueType(VariableType nType,int len);

public:
	inline ioHashString &GetDBAgentIP(){ return m_DBAgentIP; }
	inline int GetDBAgentPort(){ return m_iDBAgentPort; }

public:
	virtual void SessionClose( BOOL safely=TRUE );
	virtual bool SendMessage( CPacket &rkPacket );
	virtual void ReceivePacket( CPacket &packet );
	virtual void PacketParsing( CPacket &packet );

public:
	virtual void OnCreate();       //초기화
	virtual void OnDestroy();
	virtual bool CheckNS( CPacket &rkPacket );	
	virtual int  GetConnectType();

public:
	bool ConnectTo();

	void GenerateLOGDBAgentInfo();
	void SetLOGDBAgentInfo(std::vector<std::string>& vServerIP, std::vector<int>& vServerPort );

public:
	void ProcessTime();
	void ProcessPing();
	void ProcessFlush();

	void OnPing();

// insert
public:
	// OnInsert 함수에서 *인자로 받은 값을 변경하지말것. 알수없는 값에 영향을 줄 수 있음.
	enum PlayResultType 
	{
		PRT_END_SET   = 1,
		PRT_EXIT_ROOM = 2,
	};
	enum GameType
	{
		GT_NONE              = 0, // 이전에 사용했던 값이 있어 순차적으로 증가하지 않음.
		GT_SQUARE            = 3, 
		GT_BATTLE            = 4, 
		GT_LADDER            = 5,
		GT_SHUFFLE           = 6,
		GT_MATCH	         = 7,
	};
	void OnInsertPlayResult( ModeRecord *pRecord, PlayResultType ePlayResultType );
	void OnInsertCharacterResult( ModeRecord *pRecord, int iArray );
	void OnInsertPlayRoomResult( Room* pRoom, PlayResultType ePlayResultType );
	
	enum PesoType 
	{
		PT_TUTORIAL			= 1,
		PT_LEVELUP			= 2,
		PT_BANKRUPT			= 3,
		PT_EXIT_ROOM		= 4,
		PT_BUY_CASH			= 5,
		PT_DEL_CHAR			= 6,
		PT_GROWTH			= 7,
		PT_MGAME_OPEN		= 8,
		PT_LADDER_BONUS		= 9,	// 시즌종료시 DB에서 넣어주는 값 ( 게임서버에서는 사용하지 않음 DB에서 넣어줌 )
		PT_EVENT_PESO		= 10,	// 이벤트로 페소 획득
		PT_FISH_ITEM		= 11,
		PT_SELL_PRESENT		= 12,
		PT_RECV_PRESENT		= 13,
		PT_SELL_EXTRAITEM	= 14,
		PT_PACKAGE_DECO		= 15,	// 패키지 아이템으로 구매시 보유중인 치장 보상
		PT_SELL_ETCITEM		= 16,	// 권한 아이템 판매
		PT_GROWTH_DOWN		= 17,	// 성장 복구시 회수 페소
		PT_EXCAVATION_ITEM	= 18,  
		PT_ALCHEMIC_PESO	= 19,	// 조합에서 페소로 전환
		PT_GROWTH_ALL_DOWN  = 20,   // 모든 성장 복구시 회수 페소
		PT_SELL_MEDALITEM   = 21,	// 메달 되팔기
		PT_SELL_PET			= 22,	// 펫 팔기
		PT_SELL_COSTUME		= 23,	// 코스튬 팔기
		PT_BATTLE			= 24,	// 전투 보상( 포탈, 몬던등 )
		PT_TRADE			= 25,	// 거래소 구매
		PT_TRADE_TEX		= 26,	// 거래소 수수료
		PT_GUILD_LEAVE		= 27,	// 길드 탈퇴
		PT_GUILD_KICKOUT	= 28,	// 길드 추방
		PT_BUY_CHAR			= 29,	
		PT_BUY_DECO			= 30,	
		PT_BUY_ETCITEM		= 31,	
		PT_BUY_EXTRAITEM	= 32,
		PT_BUY_COSTUME		= 33,	
		PT_SELL_ACCESSORY	= 34,	// 악세사리 팔기
		PT_PRACTICE			= 35,	// 수련장
		PT_SELL_DECO		= 36,
	};
	void OnInsertPeso( User *pUser, int iPeso, PesoType ePesoType );

	enum TimeType
	{
		TT_ENTER_ROOM   = 1, // 본부에서 룸으로 이동시 본부시간
		TT_EXIT_PROGRAM = 2, // 본부에서 게임 종료시 본부시간
		TT_MOVE_SERVER  = 3, // 본부에서 서버 이동시 본부시간 및 룸에서 룸으로 이동시 지연 시간
		// 4~10번까지는 예비 번호로
		TT_VIEW         = 11,
		TT_WAIT_NEXT    = 12,
		TT_WAIT_LADDER_NEXT = 13,
		TT_VIEW_LADDER      = 14,
		TT_FISHING		= 15,
		TT_EXCAVATING   = 16,
		TT_FIGHTMODE_VIEW = 17,
		TT_LADDER_MATCH		= 18,
	};
	void OnInsertTime( User *pUser, TimeType eTimeType );

	enum CharType
	{
		CT_BUY     = 1,
		CT_LEVELUP = 2,
		CT_PCROOM  = 3,
		CT_DEL     = 4,
		CT_TUTORIAL= 5,
		CT_BANKRUPT= 6,
		CT_EVENT_PROPOSAL = 7,
		CT_EVENT_PLAYTIME = 8,
		CT_EVENT_CHAR = 9,      // 이벤트로 용병 획득
		CT_PRESENT = 10,
		CT_PACKAGE = 11,
		CT_RENTAL  = 12,
		CT_DISASSEMBLE	= 13,
		CT_ALCHEMIC_SOLDIER	= 14,
		CT_EVENT_PCBANG_PLAYTIME = 15,
		CT_CASH    = 1000000, // DB에는 저장되지 않는 타입으로 구분용으로 사용
	};
	// 영구용병은 iLimitDate가 0
	void OnInsertChar( User *pUser, int iClassType, int iLimitDate, int iPesoPrice, const char *szItemIndex, CharType eCharType );

	enum DecoType
	{
		DT_BUY     = 1,
		DT_DEL     = 2,
		DT_DEFAULT = 3,
		DT_PRESENT = 4,
		DT_PACKAGE = 5,
	};
	void OnInsertDeco( User *pUser, int iItemType, int iItemCode, int iPesoPrice, const char *szItemIndex, DecoType eDecoType );

	enum EtcType
	{
		ET_BUY = 1,
		ET_DEL = 2,
		ET_PRESENT = 3,
		ET_SELL = 4,
		ET_REFILL = 5,
		ET_DATE_DEL = 6,
		ET_APPLY    = 7,
		ET_USE		= 8,	// 중첩 아이템 사용시
	};
	void OnInsertEtc( User *pUser, int iItemType, int iItemValue, int iPesoPrice, const char *szItemIndex, EtcType eEtcType );

	enum ExtraType
	{
		ERT_BUY        = 1,
		ERT_DEL        = 2,
		ERT_PRESENT    = 3,
		ERT_COMPOUND   = 4,
		ERT_EXCAVATION = 5,
		ERT_CUSTOM_ADD = 6,
		ERT_CUSTOM_DEL = 7,
		ERT_ALCHEMIC_ITEM	= 8,
		ERT_RECHARGE_ITEM   = 9,
	};
	void OnInsertExtraItem( User *pUser, int iItemCode, int iReinforce, int iMachineCode, int iLimitDays, int iPesoPrice, int iPeriodType, DWORD dwMaleCustom, DWORD dwFemaleCustom, const char *szItemIndex, ExtraType eExtraType );

	void OnInsertTutorialTime( User *pUser );

	enum CashItemType
	{
		CIT_CHAR    = 1,
		CIT_DECO    = 2,
		CIT_ETC     = 3,
		CIT_PESO    = 4,
		CIT_EXTRA   = 5,
		CIT_PRESENT = 6,
		CIT_POPUP	= 7,
	};
	// itemtype -> classType, decoType, etcType / itemvlaue -> limitDate, itemcode, item 횟수나 분
	void OnInsertCashItem( User *pUser, int iItemType, int iItemValue, int iCashPrice, const char *szItemIndex, CashItemType eCashItemType, ioHashString szSubscriptionID = "" );

	enum PresentType
	{
		PST_RECIEVE = 1,
		PST_DEL     = 2,
		PST_SELL    = 3,
		PST_TRADE   = 4,
		PST_EVENTSHOP = 5,
		PST_WEB_INSERT = 6,			//웹에서 선물 지급
		PST_MASTER_INSERT = 7,		//관리페이지에서 선물지급
		PST_ALCHEMIC	= 8,		//조합에의해 선물 지급
		PST_CLOVERSHOP	= 9,		// 클로버샵
		PST_MILEAGESHOP = 10,		// 마일리지샵
		PST_TIMEGATE = 11,			// 타임게이트
		PST_PRACTICE = 12,			// 수련장 보상
	};
	void OnInsertPresent( DWORD dwSendUserIndex, const ioHashString &rszSendPublicID, const char *szSendPublicIP, DWORD dwRecievUserIndex, short iPresentType, int iPresentValue1, int iPresentValue2, int iPresentValue3, int iPresentValue4, PresentType ePresentType, const char *szNote );

	// Pet
	enum PetDataType
	{
		PDT_CREATE			= 1,
		PDT_DELETE			= 2,
		PDT_EQUIP			= 3,
		PDT_RANKUP			= 4,
		PDT_LEVELUP			= 5,
		PDT_NURTURE			= 6,
		PDT_COMPOUND		= 7,
	};
	void OnInsertPetLog( User *pUser, const int& iPetIndex, const int& iPetCode, const int& iPetRank, const int& iPetLevel, const int& iPetExp, const int& iCode, PetDataType ePetDataType, int iSubInfo = 0 );

	// subscription
	enum SubscriptionState
	{
		SST_RECIVE	= 1,
		SST_DELETE	= 2,
	};

	void OnInsertSubscription( User *pUser, int iIndex, short iPresentType, int iValue1, int iValue2, int iSubscriptionGold, ioHashString szSubscriptionID, DWORD dwLimitDate, SubscriptionState eStateType );

	enum MedalType
	{
		MT_PRESENT      = 1,
		MT_DEL          = 2,
	};
	void OnInsertMedalItem( User *pUser, int iItemType, int iPeriodType, const char *szItemIndex, MedalType eMedalType );

	enum ExMedalType
	{
		EMT_USE             = 1,
		EMT_DELETE_DATE     = 2,
		EMT_DELETE_GRADE_UP = 3,
	};
	void OnInsertExMedalSlot( User *pUser, int iClassType, BYTE iSlotNumber, int iLimitTime, ExMedalType eMedalType );

	enum QuestType
	{
		QT_PROGRESS = 1,		// 진행중 - 받은 상태
		QT_ATTAIN   = 2,		// 달성 - 퀘스트 달성했지만 보상은 받지 않았음
		QT_COMPLETE = 3,		// 완료 - 보상 받았음.
		QT_REPEAT_REWARD = 4,   // 진행중 - 보상 받음 - 진행중일 때 반복해서 보상 지급함.
	};
	void OnInsertQuest( User *pUser, DWORD dwMainIndex, DWORD dwSubIndex, QuestType eQuestType );
	void OnCheatingQuest( User *pUser, DWORD dwMainIndex, DWORD dwSubIndex );
	enum TradeSysType
	{
		TST_REG		= 1,
		TST_BUY		= 2,
		TST_CANCEL	= 3,
		TST_TIMEOUT	= 4,
	};
	void OnInsertTrade( DWORD dwUserIndex,
						const ioHashString &rszPublicID,
						DWORD dwTradeIndex,
						DWORD dwItemType,
						DWORD dwMagicCode,
						DWORD dwValue,
						__int64 iItemPrice,
						TradeSysType eType,
						const char *szPublicIP,
						const char *szNote );

	enum RecordTypes
	{
		RCT_LOGIN = 0,
		RCT_PCROOM  = 1,
		RCT_LOGOUT = 2,
	};
	enum PCRoomSubType
	{
		PCST_FREE_PCROOM   = 0,
		PCST_CHARGE_PCROOM = 1,
	};
	//void OnInsertPCRoom( User *pUser, int iPlayTime,  PCRoomType ePCRoomType , PCRoomSubType ePCRoomSubType );
	void OnInsertRecordInfo( User *pUser, int iPlayTime,  RecordTypes eRecordType );


	void OnInsertPCInfo( User *pUser, const ioHashString &rsOS, const ioHashString &rsWebBrowser, const ioHashString &rsDXVersion, const ioHashString &rsCPU, const ioHashString &rsGPU
		                , int iMemory, const ioHashString &rsDesktopWH, const ioHashString &rsGameWH, bool bFullScreen , 	const ioHashString &rsHddSerialString);		// 2019-03-05 by bckim, 유저 UAC rsHddSerialString 추가

	enum MatchTypes
	{
		MT_CANCEL = 0,
		MT_TIMEOVER  = 1,
		MT_SUCCESS = 2,
	};

	enum MatchLogTypes
	{
		MT_MATCHING = 0,
		MT_REVENGE  = 1,
	};

	void OnInsertMatchInfo( User *pUser, int iStartTier, int iEndTier, int iDelayTime, int iMaxWinCount, int iMatchPoint, MatchTypes eType, int iRoomIndex, int iRevenge );

	// alchemic
	// 조합형식
	enum AlchemicFuncType
	{
		AFT_SOLDIER		= 1,	// 용병조합
		AFT_EXTRAITEM	= 2,	// 장비조합
		AFT_CHANGE		= 3,	// 랜덤변환
		AFT_EXCHANGE	= 4,	// 선택변환
		AFT_PESO		= 5,	// 페소변환
	};

	// 결과타입
	enum AlchemicFuncResultType
	{
		AFRT_RECIPE_SUCC	= 1,
		AFRT_RANDOM_SUCC	= 2,
		AFRT_CHANGE_SUCC	= 3,
		AFRT_FAIL			= 4,
	};

	// 분해타입
	enum DisassembleType
	{
		DST_SOLDIER		= 1,
		DST_EXTRAITEM	= 2,
	};

	// 코스튬 타입
	enum CostumeType
	{
		COT_PRESENT = 1,
		COT_DEL     = 2,
	};

	// 로그 개편 타입
	enum GameLogType
	{
		//로그인, 로그 아웃
		GLT_LOGIN					= 10003,
		GLT_LOGOUT					= 10004,
		GLT_PCROOM					= 10005,

		//미션
		GLT_UPDATE_MISSION			= 20001,
		GLT_COMPLETE_MISSION		= 20002,
		GLT_COMPLETE_COMPENSATION	= 20003,
		GLT_INIT					= 20004,

		//출석부
		GLT_ATTENDANCE				= 20005,

		//길드보상
		GLT_GUILD_ATTENDANCE		= 20006,
		GLT_GUILD_ATTENDANCE_REWARD	= 20007,
		GLT_GUILD_RANK_REWARD		= 20008,

		// 확률 상승가챠
		GLT_RISING_GASHA			= 20009,

		GLT_ITEM_BUY				= 30001,
		GLT_POPUPSTORE				= 30002,

		//페소 흐름
		GLT_PESO_GAIN				= 30003,
		GLT_PESO_CONSUME			= 30004,

		//골드 소비
		GLT_SPEND_CASH				= 30006,

		GLT_ITEM_REINFORCE			= 30007,
		GLT_PRACTICE_ENTER          = 30008,
		GLT_PRACTICE_RESULT         = 30009,
		//아이템 강화
	};

	enum LogNewExcavationType		// 2018-01-15 by bckim, 탐사 확장 로그 타입 정의
	{
		LNET_START_DIGGING			= 1,
		LNET_DIGGING_SUCCESS		= 2,
		LNET_DIGGING_REAPPRAISAL	= 3,
		LNET_DIGGING_FAIL			= 4,
		LNET_EXCAVATION_RESULT		= 5,
		LNET_EXCAVATION_END			= 6,
		LNET_RECHARGE_SHOVEL		= 7,		
		LNET_LEVELUP_RECHARGE_SHOVEL= 8,
	};  // End. 2018-01-15 by bckim, 탐사 확장 로그 타입 정의


	enum LogDiceGameType	// 2018-08-30 by bckim, 주사위 이벤트 추가  로그 타입 정의
	{
		LNET_DICEGAME_PLAY_GAME			= 0,
		LNET_DICEGAME_COMPELETE			= 1,
		LNET_DICEGAME_USE_ITEM_DICE		= 2,
		LNET_DICEGAME_USE_ITEM_BOARD	= 3,
		LNET_DICEGAME_USE_ITEM_REWARD	= 4,				
	};  // End. 2018-08-30 by bckim, 주사위 이벤트 추가  로그 타입 정의

	enum LogEventType
	{
		LET_MODE	= 1,
		LET_ITEM	= 2,
		LET_ETC		= 3,
	};

	enum SpendCashType
	{
		SCT_REAL_CASH	= 1,
		SCT_BONUS_CASH	= 2,
	};

	// 확률 상승 가챠 타입
	enum RisingGashaType
	{
		RGT_BUY_PESO = 0,
		RGT_BUY_CASH,
	};
	// 조합
	void OnInsertAlchemicFunc( User *pUser, short iFuncType, int iFuncCode, BYTE iResultType,
							   int iUseCnt1, int iUseCnt2, int iResult1, int iResult2 );
	// 분해
	void OnInsertDisassemble( User *pUser, int iType, int iCode, int iResultCode, int iResultCnt );
	// 조각획득
	void OnInsertAddAlchemicPiece( int iUserIndex, const ioHashString &rszPublicID, int iLevel, int iPlayTime, BYTE iDifficulty, int iCnt );

	// 클로버
	void OnInsertCloverInfo( const int iUserIndex, const int iFriendIndex, const BYTE byCloverType, const int iCount );

	// 빙고
	void OnInsertBingoChoiceNumber( const int iUserIndex, const ioHashString& szPublicID, const BYTE byChoiceType, const int iSelectNumber, const BYTE byState );

	enum EventCashType
	{
		ECT_USE = 1, 
	};

	void OnInsertEventCash( User *pUser, DWORD dwEtcItemType, int iAddCash, EventCashType eEventCashType );

	// 강화
	enum MaterialCompoundResult
	{
		MCR_COMPOUND_SUCCESS	= 1,
		MCR_COMPOUND_FAIL	    = 2,
	};

	void OnInsertMaterialCompound( User *pUser, const int& iExtraItemCode, const int& iBeforeReinforce, const int& iNowReinforce, const int& iMaterialCode, MaterialCompoundResult eCompoundResult );

	// 각성
	void OnInsertCharAwakeInfo( User *pUser, const int& iSoldierCode, const int& iMaterialCode, const short& iUseMaterialCount, BYTE chAwakeType );

	// 코스튬
	void OnInsertCostumeInfo( User *pUser, const int& iCostumeCode, int iCount, int iEventType );

	// 게임 로그 개편
	void OnInsertGameLogInfo( int iLogType, User *pUser, int iTableIndex, int iCode, BYTE byEventType, int iParam1, int iParam2, int iParam3, int iParam4, char* szParam5 );

	// 2018-01-15 by bckim, 탐사 확장 - 로그 
	void OnInsertExcavatingLogInfo( int iLogType, User *pUser, const CoordinateInfo* istInfo, const UserExcavationRewardInfo* istRewardInfo );	// End. 2018-01-15 by bckim, 탐사 확장 - 로그 

	// 보너스 캐쉬
	void OnInsertUsageOfBonusCash( User *pUser, int iBonusCashIndex, int iAmount, int iType, int iCode, int iValue, const ioHashString &szBillingGUID ,int iStatus);

	// 악세사리
	void OnInsertAccessoryInfo( User *pUser, const int& iAccessoryCode, int iCount, int iEventType );

	// 오크통
	void OnInsertOakbarrelInfo( User *pUser, int iFlag, BYTE byStep, int iType, DWORD dwValue1, DWORD dwValue2 );

	// 2018-06-18 by bckim, 카드 짝 마추기 이벤트 추가
	void OnInsertCatdMatchingInfo( User *pUser,int iLogType,BYTE M_Type,BYTE M_Mark1,BYTE M_Mark2,BYTE Mark1,BYTE Mark2, int iRewardStep, int iType, DWORD dwValue1, DWORD dwValue2 );	// 카드 매칭 이벤트 // 로그 남겨라
	// End. 2018-06-18 by bckim, 카드 짝 마추기 이벤트 추가

	// 2018-08-30 by bckim, 주사위 이벤트 추가 - 로그 
	void OnInsertDiceGameLogInfo( int iLogType, User *pUser,const int iGameState);

	// 포탈 방 생성 로그
	void OnInsertPrisonerRoomLog( const uint64 iServerID, int iEnterRoom, int iMakeRoom, int iChoiceRoom );

	// 게임방 입장 로그
	void OnInsertEnterBattleRoomLog( int iRoomNumber, int iTeamBlue, int TeamRed, int TeamMaxBlue, int TeamMaxRed, int iRandomTeamMode, int iclosedRoomOption, int iUserModeOption );

	// 정기
	void OnInsertPieceToPeso( User *pUser, int iPieceCount, int iAddictivePieceCount, int iGetPeso );
	enum SpiritObtainType
	{
		SPIRIT_MODE_REWARD = 1,
		SPIRIT_SELL = 2,
	};
	void OnInsertObtainSpirit( User *pUser, int iSpiritCode, int iSpiritQuantity, int iValue, BYTE byLogType );
	void OnInsertComposeSpirit( User *pUser, int iClassType, int iSpiritCode, int iSpiritQuantity, int iSpecialSpiritCode, int iSpecialSpiritQuantity );
	void OnInsertDecomposeSpirit( User *pUser, int iClassType, int iSpiritCode, int iSpiritCount, int iSoulStoneCount, BYTE byCritical );
	void OnInsertConversionSpirit( User *pUser, int iUseSpiritCode, int iUseSpiritCount, int iGetSpiritCode, int iGetSpiritCount, BYTE byCritical );
	
	// 커스텀 메달
	void OnInsertCustomMedalInfo( User *pUser, int iMedalCode, int iMedalIndex, int iStat1, int iStat2, int iStat3, int iStat4,
		int iStat5, int iStat6, int iStat7, int iStat8, int iLimitType, SYSTEMTIME sysTime, MedalType iLogType );

	// 캐릭터 교체 해킹 로그 관련
	void OnInsertHackLogInfo( User *pUser, int playType, int modeType, int subType1, int subType2, int itemType, int itemCode, int itemValue);

	// 채팅 메시지 해킹 로그 관련
	void OnInsertHackLogMessage( User *pUser, const char * szItemName, int iItemReinforce);

	// 악세서리 강화
	void OnInsertAccessoryCompound( User *pUser, const int& iExtraItemCode, const int& iBeforeReinforce, const int& iNowReinforce, const int& iMaterialCode, MaterialCompoundResult eCompoundResult );

private:
	void OnConConnect( const uint64 gameServerId, const ioHashString &szIP, const int iPort , const ioHashString &szServerName , const int iConConnect, ChannelingType eChannelingType );
	
// 기능
private: 
	const ioHashString GetClassName(const int iClassType);
public:
	GameType GetGameType( ModeType eModeType, Room *pRoom );

private:			/* Singleton Class */
	LogDBClient( SOCKET s, DWORD dwSendBufSize, DWORD dwRecvBufSize );
	virtual ~LogDBClient();
};
#define g_LogDBClient LogDBClient::GetInstance()
#endif // !defined(AFX_LOGDBCLIENT_H__8BBCE6AF_B8A6_4D7C_A9FA_B2B4B32491CA__INCLUDED_)
