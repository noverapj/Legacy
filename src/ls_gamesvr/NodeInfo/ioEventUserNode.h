#ifndef __ioEventUserNode_h__
#define __ioEventUserNode_h__

#include "ioEventManager.h"

class User;
class SP2Packet;
class ioPlayStage;

// user가 가지고 있는 event
class EventUserNode
{
protected:
	DWORD     m_dwIndex;
	IntVec    m_ValueVec;
	IntVec    m_BackupVec;
	bool      m_bMustSave;
	EventType m_eEventType;
	ModeCategory m_eModeCategory;


protected:
	bool IsChange();
	void Clear();

public:
	int  GetSize() const;
	void SetSize(int iSize );
	int  GetValue( int iArray );
	void SetValue( int iArray, int iValue );

	virtual EventType GetType() const;
	virtual void Init();
	virtual void Save( const DWORD dwAgentID, const DWORD dwThreadID );
	virtual void UpdateFirst( User *pUser );
	virtual void InsertDB( const DWORD dwAgentID, const DWORD dwThreadID, const char *szUserGUID, DWORD dwUserIndex );
	virtual void Process( User *pUser );      // 게임플레이 체크
	virtual void ProcessTime( User *pUser );  // 접속시간 체크
	virtual void SendData( User *pUser );

	virtual void FillMoveData( SP2Packet &rkPacket, bool bLog );
	virtual void ApplyMoveData( SP2Packet &rkPacket );
	virtual int GetFillMoveDataSize();
	virtual int GetAddSize();
	virtual bool IsEmptyValue();

	virtual void OnRecievePacket( User *pUser, SP2Packet &rkPacket );

	void Backup();
	void SetIndex( DWORD dwIndex );
	DWORD GetIndex() const { return m_dwIndex; }

	void SetType( EventType eEventType );

	inline int GetModeCategory() { return m_eModeCategory; }
	inline void SetModeCategory( ModeCategory eModeCategory ) { m_eModeCategory = eModeCategory; }

public:
	EventUserNode();
	virtual ~EventUserNode();
};

//----------------------------------------------------------------------------------------------------------------------------------
// class 추가시에 CreatEventUserNode() 에도 추가할 것
enum
{
	// EVT_PROPOSAL 
	  // vec array
	VA_PROPOSAL_CNT       = 0, // from db
	VA_GAVE_CHAR          = 1, // from db
	VA_ADD_PROPOSAL_CNT   = 2, // from memory
	VA_ADD_GAVE_CHAR_CNT  = 3, // from memory
	  // add size
	ADD_PROPOSAL_SIZE     = 2, // from memory 변수가 2개 추가됨
	
	// EVT_COIN
	  // vec array
	VA_COIN_CNT           = 0, // from db
	VA_PLAY_SEC           = 1, // from db
	VA_ADD_COIN_CNT       = 2, // from memory
	  // add size
	ADD_COIN_SIZE         = 1, // from memory 변수가 1개 추가됨

	// EVT_PLAYTIME
	  // vec array
	VA_PLAYTIME_PLAY_SEC  = 0, // from db
	VA_PLAYTIME_GET_GIFT  = 1, // from db    11111 : 0이면 미지급 1이면 지급 10000이면 5번째 선물만 지급하고 나머지는 미지급

	// EVT_CHANCE_MORTMAIN_CHAR
	  // vec array
    VA_CMC_MAGIC_CODE     = 0, // from db   >= 0이면 플레이한 시간이고 -1은 일반 복권 -2는 피씨방 복권 -3은 일반/피씨방 복권 2장 보유
	VA_CMC_MORTMAIN_CNT   = 1, // from db   획득한 영구용병 카운트

	//EVT_ONE_DAY_GOLD_ITEM
	  // vec array
	 VA_GOLD_ITEM_RECV_DATE = 0, // from db   >= 마지막으로 받은 월:일 저장

    //EVT_PLAYTIME_PRESENT
	  // vec array
    VA_PLAYTIME_PRESENT_TIME_CNT = 0, // from db   >= 0이면 플레이한 시간이고 -1은 선물을 받을 수 있는 1번의 기회

	//EVT_CHRISTMAS
	  // vec array
    VA_CHRISTMAS_GET_GIFT_DATE   = 0, // from db   선물은 받은 날짜

	//EVT_CONNECTION_TIME
	  // vec array
    VA_CT_GET_CHANCE_DATE = 0, // from db 찬스를 받을 날짜
	VA_CT_IS_CHANCE       = 1, // form db 1이면 찬스가 있고 0이면 찬스가 없다.

	//EVT_ONE_DAY_GIFT
	  // vec array
    VA_OG_GET_GIFT_DATE   = 0, // from db 선물을 받은 날짜를 저장 예) 20100830

	//EVT_GRADEUP_PRESENT
	  // vec array
    VA_GP_CAN_RECEIVE_GIFT = 0, // from memory 1이면 선물을 받을 수 있고 0이면 선물을 안 받는다.

	//EVT_CONNECTION_TIME_SELECT_GIFT
	  // vec array
    VA_CTSG_GET_CHANCE_DATE = 0, // from db 찬스를 받을 날짜
	VA_CTSG_IS_CHANCE       = 1, // form db 1이면 찬스가 있고 0이면 찬스가 없다.

	//EVT_ENTRY
	 // vec array
	VA_E_GET_GIFT           = 0, // from db 1:이면 선물을 받았다, 0:이면 선물을 받지 않았다.
	                             // 1번 array DB에 값이므로 메모리값은 2번으로 설정
	VA_E_CAN_RECEIVE_GIFT   = 2, // from memory 1:이면 선물을 받을 조건이다. 0:이면 선물을 받을 조건이 안된다.
	  // add size
	ADD_ENTRY_SIZE          = 1, // from memory 변수가 1개 추가됨

	//EVT_ENTRY_AFTER
	  // vec array
	VA_EA_GET_GIFT          = 0, // from db 1:이면 선물을 받았다, 0:이면 선물을 받지 않았다.
	// 1번 array DB에 값이므로 메모리값은 2번으로 설정
	VA_EA_CAN_RECEIVE_GIFT  = 2, // from memory 1:이면 선물을 받을 조건이다. 0:이면 선물을 받을 조건이 안된다.
	  // add size
	ADD_ENTRY_AFTER_SIZE    = 1, // from memory 변수가 1개 추가됨

	//EVT_CONNECT_AND_PLAYTIME
		// vec array
	VA_CAP_CONNECT_RESET_DATE = 0, // from db [0000]0000 : 접속 포인트를 받은 월일,    0000[0000] : 포인트가 리셋된 월일.
	VA_CAP_POINT_AND_SEC      = 1, // from db [0000]0000 : 게임접속시간으로 받은포인트, 0000[0000] : 게임 접속 초
};

class ProposalEventUserNode : public EventUserNode
{
public:
	virtual void Init();
	virtual void Save( const DWORD dwAgentID, const DWORD dwThreadID );
	virtual void InsertDB( const DWORD dwAgentID, const DWORD dwThreadID, const char *szUserGUID, DWORD dwUserIndex );
	virtual int GetAddSize();

public:
	bool IsGiveChar( const User *pUser );
	void SetValueGiveChar();

public:
	ProposalEventUserNode();
	virtual ~ProposalEventUserNode();
};

//-------------------------------------------------------------------------------------------------------------------------------
class CoinEventUserNode : public EventUserNode
{
public:
	virtual void Init();
	virtual void Save( const DWORD dwAgentID, const DWORD dwThreadID );
	virtual void InsertDB( const DWORD dwAgentID, const DWORD dwThreadID, const char *szUserGUID, DWORD dwUserIndex );
	virtual void Process( User *pUser );
	virtual int GetAddSize();

public:
	void SetGradeUpCoin( User *pUser );

public:
	CoinEventUserNode();
	virtual ~CoinEventUserNode();
};

//---------------------------------------------------------------------------------------------------------------------------------
class ExpEventUserNode : public EventUserNode
{
public:
	virtual void Save( const DWORD dwAgentID, const DWORD dwThreadID );
	virtual void InsertDB( const DWORD dwAgentID, const DWORD dwThreadID, const char *szUserGUID, DWORD dwUserIndex );
	virtual bool IsEmptyValue();

protected:
	bool IsEventWeek( WORD wDayOfWeek, int iValueArray, ModeCategory eModeCategory );

public:
	bool IsEventTime( const User *pUser, ModeCategory eModeCategory );
	float GetEventPer( float fPCRoomBonus, const User *pUser, ModeCategory eModeCategory = MC_DEFAULT );

public:
	ExpEventUserNode();
	virtual ~ExpEventUserNode();
};

//--------------------------------------------------------------------------------------------------------------------------------
class PesoEventUserNode : public EventUserNode
{
public:
	virtual void Save( const DWORD dwAgentID, const DWORD dwThreadID );
	virtual void InsertDB( const DWORD dwAgentID, const DWORD dwThreadID, const char *szUserGUID, DWORD dwUserIndex );
	virtual bool IsEmptyValue();

protected:
	bool IsEventWeek( WORD wDayOfWeek, int iValueArray, ModeCategory eModeCategory );

public:
	bool IsPesoTime( const User *pUser, ModeCategory eModeCategory );
	float GetPesoPer( float fPCRoomBonus, const User *pUser, ModeCategory eModeCategory = MC_DEFAULT );

public:
	PesoEventUserNode();
	virtual ~PesoEventUserNode();
};

//--------------------------------------------------------------------------------------------------------------------------------
class FishingEventUserNode : public EventUserNode
{
public:
	virtual void Save( const DWORD dwAgentID, const DWORD dwThreadID );
	virtual void InsertDB( const DWORD dwAgentID, const DWORD dwThreadID, const char *szUserGUID, DWORD dwUserIndex );
	virtual bool IsEmptyValue();

protected:
	bool IsEventWeek( WORD wDayOfWeek, int iValueArray );

public:
	bool IsEventTime( const User *pUser );

public:
	FishingEventUserNode();
	virtual ~FishingEventUserNode();
};

//-------------------------------------------------------------------------------------------------------------------------------
// VA_PLAYTIME_GET_GIFT : XXXXX 자리를 사용하면 1자리 부터 gift 1에 대응됨(10->2, 100->3, 1000->4, 10000->5) , 11111은 
// 안내 메세지를 보낸 것을 나타냄, 22222는 상품을 지급 받은 것을 나타냄, 00112은 gift 1은 사용했고 2~3번은 안내 메세지를 보냈다는 표시
class PlayTimeEventUserNode : public EventUserNode
{
public:
	enum GiftType 
	{
		GT_1_PESO    = 10001,
		GT_2_PESO    = 10002,
		GT_3_SOLDIER = 10003,
		GT_4_SOLDIER = 10004,
		GT_5_SOLDIER = 10005, 
	};

	enum GiftState
	{
		GS_NONE   = 0,
		GS_NOTICE = 1,
		GS_USED   = 2,
	};
public:
	virtual void Save( const DWORD dwAgentID, const DWORD dwThreadID );
	virtual void InsertDB( const DWORD dwAgentID, const DWORD dwThreadID, const char *szUserGUID, DWORD dwUserIndex );
	virtual void Process( User *pUser );
	virtual void OnRecievePacket( User *pUser, SP2Packet &rkPacket );

public:
	void SetGift( GiftType eType, User *pUser, bool bPeso );
	bool IsGift( GiftType eType, User *pUser, bool bPeso );
	int  GetLimitSecond( GiftType eType );
	virtual void SendData( User *pUser );
	
protected:
	bool IsGiftData( GiftType eType, GiftState eGiftState );
	void SetGiftData( GiftType eGiftType , GiftState eGiftState );

public:
	PlayTimeEventUserNode();
	virtual ~PlayTimeEventUserNode();
};

//-------------------------------------------------------------------------------------------------------------------------------
// 어린이날 이벤트 노드
class ChildrenDayEventUserNode : public EventUserNode
{
public:
	virtual void Save( const DWORD dwAgentID, const DWORD dwThreadID );
	virtual void InsertDB( const DWORD dwAgentID, const DWORD dwThreadID, const char *szUserGUID, DWORD dwUserIndex );
	virtual bool IsEmptyValue();

public:
	ChildrenDayEventUserNode();
	virtual ~ChildrenDayEventUserNode();
};

//-------------------------------------------------------------------------------------------------------------------------------
// 영구 용병 이벤트 노드
class PesoBonusEventUserNode : public EventUserNode
{
public:
	virtual void Save( const DWORD dwAgentID, const DWORD dwThreadID );
	virtual void InsertDB( const DWORD dwAgentID, const DWORD dwThreadID, const char *szUserGUID, DWORD dwUserIndex );
	virtual bool IsEmptyValue();

public:
	void SetPesoBonus( User *pUser );

public:
	PesoBonusEventUserNode();
	virtual ~PesoBonusEventUserNode();
};

//---------------------------------------------------------------------------------------------------------------------------------
// 2시간 용병 레벨제한 없이 구매하는 이벤트
class BuyCharNoLevelLimitEventUserNode : public EventUserNode
{
public:
	virtual void Save( const DWORD dwAgentID, const DWORD dwThreadID );
	virtual void InsertDB( const DWORD dwAgentID, const DWORD dwThreadID, const char *szUserGUID, DWORD dwUserIndex );
	virtual bool IsEmptyValue();

public:
	bool IsNoLevelLimit( const User *pUser, bool bCharCreate, int iMsgType, int iCharLimitSecond, int iCharPeriodType );

public:
	BuyCharNoLevelLimitEventUserNode();
	virtual ~BuyCharNoLevelLimitEventUserNode();
};

//----------------------------------------------------------------------------------------------------------------------------------
// 용병단 레벨업시 페소 지급하는 이벤트
class GradeUpEventUserNode : public EventUserNode
{
public:
	virtual void Save( const DWORD dwAgentID, const DWORD dwThreadID );
	virtual void InsertDB( const DWORD dwAgentID, const DWORD dwThreadID, const char *szUserGUID, DWORD dwUserIndex );
	virtual bool IsEmptyValue();

public:
	void SetGift( User *pUser, int iGradeLevel );

public:
	GradeUpEventUserNode();
	virtual ~GradeUpEventUserNode();
};

//-------------------------------------------------------------------------------------------------------------------------------
// 피씨방 보너스 이벤트 노드
class PCRoomEventUserNode : public EventUserNode
{
public:
	virtual void Save( const DWORD dwAgentID, const DWORD dwThreadID );
	virtual void InsertDB( const DWORD dwAgentID, const DWORD dwThreadID, const char *szUserGUID, DWORD dwUserIndex );
	virtual bool IsEmptyValue();

protected:
	bool IsEventWeek( WORD wDayOfWeek, int iValueArray, ModeCategory eModeType );

public:
	bool IsEventTime( const User *pUser, ModeCategory eModeType );
	void SetPesoAndExpBonus( const User *pUser, float& fPesoBonus, float& fExpBonus, ModeCategory eModeType );

public:
	PCRoomEventUserNode();
	virtual ~PCRoomEventUserNode();
};

//-------------------------------------------------------------------------------------------------------------------------------
// 피씨방 플레이타임 이벤트
class PCRoomEventPlayTime : public EventUserNode
{
private:
	DWORD m_dwCurrentTime;
public:
	virtual void Init();
	virtual void Save( const DWORD dwAgentID, const DWORD dwThreadID );
	virtual void InsertDB( const DWORD dwAgentID, const DWORD dwThreadID, const char *szUserGUID, DWORD dwUserIndex );
	virtual bool IsEmptyValue();
	virtual void ProcessTime( User *pUser );

protected:
	bool IsEventWeek( WORD wDayOfWeek, int iValueArray, ModeCategory eModeType );

public:
	bool IsEventTime( const User *pUser, ModeCategory eModeType );

public:
	PCRoomEventPlayTime();
	virtual ~PCRoomEventPlayTime();
};

//-------------------------------------------------------------------------------------------------------------------------------
class ChanceMortmainCharEventUserNode : public EventUserNode
{
public:
	virtual void Init();
	virtual void Save( const DWORD dwAgentID, const DWORD dwThreadID );
	virtual void InsertDB( const DWORD dwAgentID, const DWORD dwThreadID, const char *szUserGUID, DWORD dwUserIndex );
	virtual void OnRecievePacket( User *pUser, SP2Packet &rkPacket );

public:
	void UpdatePlayTime( User *pUser, DWORD dwPlayTime );

public:
	ChanceMortmainCharEventUserNode();
	virtual ~ChanceMortmainCharEventUserNode();
};
//-------------------------------------------------------------------------------------------------------------------------------
class OneDayGoldItemEvent : public EventUserNode
{

public:
	virtual void Init();
	virtual void Save( const DWORD dwAgentID, const DWORD dwThreadID );
	virtual void InsertDB( const DWORD dwAgentID, const DWORD dwThreadID, const char *szUserGUID, DWORD dwUserIndex );
	virtual void OnRecievePacket( User *pUser, SP2Packet &rkPacket );

public:
	int GetEventCurrentDate();
	void CheckGoldItemDate( User *pUser );

public:
	OneDayGoldItemEvent();
	virtual ~OneDayGoldItemEvent();
};
//-------------------------------------------------------------------------------------------------------------------------------
class DormancyUserEvent : public EventUserNode
{
public:
	virtual void Save( const DWORD dwAgentID, const DWORD dwThreadID );
	virtual void InsertDB( const DWORD dwAgentID, const DWORD dwThreadID, const char *szUserGUID, DWORD dwUserIndex );
	virtual bool IsEmptyValue();

public:
	bool CheckDormancyDateToPresent( User *pUser, CTime &rkEntryTime, CTime &rkConnectTime );

public:
	DormancyUserEvent();
	virtual ~DormancyUserEvent();
};
//----------------------------------------------------------------------------------------------------------------------------------
class PlayTimePresentEventUserNode : public EventUserNode
{
public:
	enum 
	{
		CHANCE_GET_PRESENT = -1,
	};
public:
	virtual void Init();
	virtual void Save( const DWORD dwAgentID, const DWORD dwThreadID );
	virtual void InsertDB( const DWORD dwAgentID, const DWORD dwThreadID, const char *szUserGUID, DWORD dwUserIndex );
	virtual void OnRecievePacket( User *pUser, SP2Packet &rkPacket );

public:
	void UpdatePlayTime( User *pUser, DWORD dwPlayTime );

public:
	PlayTimePresentEventUserNode();
	virtual ~PlayTimePresentEventUserNode();
};
//-----------------------------------------------------------------------------------------------------------------------------------
class ChristmasEventUserNode : public EventUserNode
{
public:
	virtual void Init();
	virtual void Save( const DWORD dwAgentID, const DWORD dwThreadID );
	virtual void InsertDB( const DWORD dwAgentID, const DWORD dwThreadID, const char *szUserGUID, DWORD dwUserIndex );
	virtual void OnRecievePacket( User *pUser, SP2Packet &rkPacket );

public:
	ChristmasEventUserNode();
	virtual ~ChristmasEventUserNode();
};
//---------------------------------------------------------------------------------------------------------------------------------
class BuyItemEventUserNode : public EventUserNode
{
public:
	virtual void Save( const DWORD dwAgentID, const DWORD dwThreadID );
	virtual void InsertDB( const DWORD dwAgentID, const DWORD dwThreadID, const char *szUserGUID, DWORD dwUserIndex );
	virtual bool IsEmptyValue();

public:
	void SendBuyPresent( User *pUser, bool bPeso, short iBuyType, int iBuyValue1, int iBuyValue2, bool bCash, int iItemPrice, int iRealCash );
	bool InsertUserPresentByBuyPresent( User *pUser, bool bPeso, DWORD dwRecvUserIndex, short iPresentType, int iBuyValue1, int iBuyValue2, bool bPresentEvent = false );

public:
	BuyItemEventUserNode();
	virtual ~BuyItemEventUserNode();
};
//----------------------------------------------------------------------------------------------------------------------------------
class ExerciseSoldierEventUserNode : public EventUserNode
{
public:
	virtual void Save( const DWORD dwAgentID, const DWORD dwThreadID );
	virtual void InsertDB( const DWORD dwAgentID, const DWORD dwThreadID, const char *szUserGUID, DWORD dwUserIndex );
	virtual bool IsEmptyValue();

public:
	ExerciseSoldierEventUserNode();
	virtual ~ExerciseSoldierEventUserNode();
};
//----------------------------------------------------------------------------------------------------------------------------------
class ConnectionTimeEventUserNode : public EventUserNode
{
public:
	virtual void Init();
	virtual void Save( const DWORD dwAgentID, const DWORD dwThreadID );
	virtual void UpdateFirst( User *pUser );
	virtual void InsertDB( const DWORD dwAgentID, const DWORD dwThreadID, const char *szUserGUID, DWORD dwUserIndex );
	virtual void OnRecievePacket( User *pUser, SP2Packet &rkPacket );

protected:
	int GetNextCheckTime();
	int GetCheckTime( int iMonth, int iDays, int iHours, int iMins );

public:
	ConnectionTimeEventUserNode();
	virtual ~ConnectionTimeEventUserNode();
};
//----------------------------------------------------------------------------------------------------------------------------------
class OneDayGiftEventUserNode : public EventUserNode
{
public:
	virtual void Init();
	virtual void Save( const DWORD dwAgentID, const DWORD dwThreadID );
	virtual void InsertDB( const DWORD dwAgentID, const DWORD dwThreadID, const char *szUserGUID, DWORD dwUserIndex );
	virtual void OnRecievePacket( User *pUser, SP2Packet &rkPacket );

public:
	OneDayGiftEventUserNode();
	virtual ~OneDayGiftEventUserNode();
};
//----------------------------------------------------------------------------------------------------------------------------------
class GradePresentEventUserNode : public EventUserNode
{
public:
	virtual void Init();
	virtual void Save( const DWORD dwAgentID, const DWORD dwThreadID );
	virtual void InsertDB( const DWORD dwAgentID, const DWORD dwThreadID, const char *szUserGUID, DWORD dwUserIndex );

public:
	void SetCanReceiveGift( CTime &rkEntryTime );
	void SetGift( User *pUser, int iGradeLevel );

public:
	GradePresentEventUserNode();
	virtual ~GradePresentEventUserNode();
};
//----------------------------------------------------------------------------------------------------------------------------------
class ConnectionTimeSelectGiftEventUserNode : public EventUserNode
{
public:
	virtual void Init();
	virtual void Save( const DWORD dwAgentID, const DWORD dwThreadID );
	virtual void UpdateFirst( User *pUser );
	virtual void InsertDB( const DWORD dwAgentID, const DWORD dwThreadID, const char *szUserGUID, DWORD dwUserIndex );
	virtual void OnRecievePacket( User *pUser, SP2Packet &rkPacket );

protected:
	int GetNextCheckTime();
	int GetCheckTime( int iMonth, int iDays, int iHours, int iMins );

public:
	ConnectionTimeSelectGiftEventUserNode();
	virtual ~ConnectionTimeSelectGiftEventUserNode();
};
//-----------------------------------------------------------------------------------------------------------------------------------
class EntryEventUserNode : public EventUserNode
{
public:
	virtual void Init();
	virtual void Save( const DWORD dwAgentID, const DWORD dwThreadID );
	virtual void InsertDB( const DWORD dwAgentID, const DWORD dwThreadID, const char *szUserGUID, DWORD dwUserIndex );
	virtual int GetAddSize();

public:
	void SetCanReceiveGift( CTime &rkEntryTime );
	void SetGift( User *pUser );

public:
	EntryEventUserNode();
	virtual ~EntryEventUserNode();
};
//---------------------------------------------------------------------------------------------------------------------------------------
class LadderPointEventUserNode : public ExpEventUserNode 
{
public:
	LadderPointEventUserNode();
	virtual ~LadderPointEventUserNode();
};
//-----------------------------------------------------------------------------------------------------------------------------------
class EntryAfterEventUserNode : public EventUserNode
{
public:
	virtual void Init();
	virtual void Save( const DWORD dwAgentID, const DWORD dwThreadID );
	virtual void InsertDB( const DWORD dwAgentID, const DWORD dwThreadID, const char *szUserGUID, DWORD dwUserIndex );
	virtual int GetAddSize();

public:
	void SetCanReceiveGift( CTime &rkEntryTime, int iUserState );
	void SetGift( User *pUser );

public:
	EntryAfterEventUserNode();
	virtual ~EntryAfterEventUserNode();
};
//----------------------------------------------------------------------------------------------------------------------------------
class ConnectAndPlayTimeEventUserNode : public EventUserNode
{
public:
	virtual void Init();
	virtual void Save( const DWORD dwAgentID, const DWORD dwThreadID );
	virtual void InsertDB( const DWORD dwAgentID, const DWORD dwThreadID, const char *szUserGUID, DWORD dwUserIndex );
	virtual void ProcessTime( User *pUser );
	virtual void OnRecievePacket( User *pUser, SP2Packet &rkPacket );

protected:
	int GetPoint();
	int GetSec();
	int GetPointAndSec( int iPoint, int iSec );

	int GetConnectDate();
	int GetResetDate();
	int GetConnectResetDate( int iConnectDate, int iResetDate );

	int GetCurrentDate();

public:
	ConnectAndPlayTimeEventUserNode();
	virtual ~ConnectAndPlayTimeEventUserNode();
};
//----------------------------------------------------------------------------------------------------------------------------------
class PlazaMonsterEventUserNode : public EventUserNode
{
public:
	PlazaMonsterEventUserNode();
	virtual	~PlazaMonsterEventUserNode();

public:
	virtual void Save( const DWORD dwAgentID, const DWORD dwThreadID );
	virtual void InsertDB( const DWORD dwAgentID, const DWORD dwThreadID, const char *szUserGUID, DWORD dwUserIndex );
	virtual bool IsEmptyValue();

public:
	bool IsEventTime( User* pUser );
};
//----------------------------------------------------------------------------------------------------------------------------------
class ModeBonusEventUserNode : public EventUserNode
{
public:
	ModeBonusEventUserNode();
	virtual	~ModeBonusEventUserNode();

public:
	virtual void Save( const DWORD dwAgentID, const DWORD dwThreadID );
	virtual void InsertDB( const DWORD dwAgentID, const DWORD dwThreadID, const char *szUserGUID, DWORD dwUserIndex );
	virtual bool IsEmptyValue();

protected:
	bool IsEventWeek( WORD wDayOfWeek, int iValueArray, ModeCategory eModeCategory );

public:
	bool IsEventTime( const User *pUser, ModeCategory eModeCategory );
	float GetEventPer( float fPCRoomBonus, const User *pUser, ModeCategory eModeCategory = MC_DEFAULT );
	bool IsEventMode( const int iMode, ModeCategory eModeCategory = MC_DEFAULT );
};

//---------------------------------------------------------------------------------------------------------------------------------
class MonsterDungeonEventUserNode : public EventUserNode
{
protected:
	bool IsEventWeek( WORD wDayOfWeek, int iValueArray );

public:
	virtual void Save( const DWORD dwAgentID, const DWORD dwThreadID );
	virtual void InsertDB( const DWORD dwAgentID, const DWORD dwThreadID, const char *szUserGUID, DWORD dwUserIndex );
	virtual bool IsEmptyValue();


public:
	bool IsEventTime( const User *pUser );
	int GetAddCount( const User *pUser );

public:
	MonsterDungeonEventUserNode();
	virtual ~MonsterDungeonEventUserNode();
};

//----------------------------------------------------------------------------------------------------------------------------------


class FreeDayEventUserNode : public EventUserNode
{
public:
	virtual void Save( const DWORD dwAgentID, const DWORD dwThreadID );
	virtual void InsertDB( const DWORD dwAgentID, const DWORD dwThreadID, const char *szUserGUID, DWORD dwUserIndex );
	virtual void ProcessTime( User *pUser );

public:
	FreeDayEventUserNode();
	virtual ~FreeDayEventUserNode();
};

//----------------------------------------------------------------------------------------------------------------------------------


class HeroExpBoostEventUserNode : public EventUserNode
{
public:
	virtual void Save( const DWORD dwAgentID, const DWORD dwThreadID );
	virtual void InsertDB( const DWORD dwAgentID, const DWORD dwThreadID, const char *szUserGUID, DWORD dwUserIndex );
	virtual void ProcessTime( User *pUser );

public:
	HeroExpBoostEventUserNode();
	virtual ~HeroExpBoostEventUserNode();
};


#endif // __ioEventUserNode_h__