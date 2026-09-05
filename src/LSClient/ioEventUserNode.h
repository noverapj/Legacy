#pragma once

#include "ioEventManager.h"

class SP2Packet;
class ioPlayStage;

// user가 가지고 있는 event
class EventUserNode
{
protected:
	EventType m_eEventType;
	IntVec    m_ValueVec;

public:
	int  GetSize() const;
	void SetSize(int iSize );
	int  GetValue( int iArray );
	void SetValue( int iArray, int iValue );

public:
	virtual EventType GetType() const;
	virtual void Init();
	virtual int  GetAddSize();
	virtual bool IsEmptyValue();
	virtual void OnRecievePacket( SP2Packet &rkPacket, ioPlayStage *pPlayStage );
	void SetType( EventType eEventType );

public:
	EventUserNode();
	EventUserNode( EventType eEventType );
	virtual ~EventUserNode();
};

//-------------------------------------------------------------------------------------------------------------------------------
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

    //EV_PLAYTIME_PRESENT_TIME
	  // vec array
    VA_PLAYTIME_PRESENT_TIME_CNT = 0, // from db   >= 0이면 플레이한 시간이고 -1은 선물을 받을 수 있는 1번의 기회

	//EVT_CHRISTMAS
		// vec array
	VA_CHRISTMAS_GET_GIFT_DATE   = 0, // from db   선물은 받은 날짜

	//EVT_CONNECTION_TIME
		// vec array
	VA_CT_GET_CHANCE_DATE = 0, // from db 찬스를 받을 날짜
	VA_CT_IS_CHANCE       = 1, // form db 1이면 찬스가 있고 0이면 찬스가 없다

	//EVT_ONE_DAY_GIFT
	   // vec array
	VA_OG_GET_GIFT        = 0, // from db 0:이면 선물을 받았다, 1:이면 선물을 받지 않았다.

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

	//EVT_CONNECT_AND_PLAYTIME
		// vec array
	VA_CAP_CONNECT_RESET_DATE = 0, // from db [0000]0000 : 접속 포인트를 받은 월일,    0000[0000] : 포인트가 리셋된 월일.
	VA_CAP_POINT_AND_SEC      = 1, // from db [0000]0000 : 게임접속시간으로 받은포인트, 0000[0000] : 게임 접속 초
};

class ProposalEventUserNode : public EventUserNode
{
public:
	virtual void Init();
	virtual int GetAddSize();

public:
	bool IsGiveChar();
	void SetValueGiveChar();

public:
	ProposalEventUserNode();
	virtual ~ProposalEventUserNode();
};

//---------------------------------------------------------------------------------------------------------------------------------
class CoinEventUserNode : public EventUserNode
{
public:
	virtual void Init();
	virtual int  GetAddSize();
	virtual void OnRecievePacket( SP2Packet &rkPacket, ioPlayStage *pPlayStage );

public:
	CoinEventUserNode();
	virtual ~CoinEventUserNode();
};

//---------------------------------------------------------------------------------------------------------------------------------
class ExpEventUserNode : public EventUserNode
{
public:
	virtual bool IsEmptyValue();
public:
	ExpEventUserNode();
	virtual ~ExpEventUserNode();
};

//--------------------------------------------------------------------------------------------------------------------------------
class PesoEventUserNode : public EventUserNode
{
public:
	virtual bool IsEmptyValue();
public:
	PesoEventUserNode();
	virtual ~PesoEventUserNode();
};

//--------------------------------------------------------------------------------------------------------------------------------
class FishingEventUserNode : public EventUserNode
{
public:
	virtual bool IsEmptyValue();
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
	virtual void OnRecievePacket( SP2Packet &rkPacket, ioPlayStage *pPlayStage );

public:
	void SetGift( GiftType eType );
	bool IsGift( GiftType eType );
	int GetLimitSecond( GiftType eType );
	bool IsGiftData( GiftType eType, GiftState eGiftState );
	
protected:
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
	virtual bool IsEmptyValue();
	virtual void OnRecievePacket( SP2Packet &rkPacket, ioPlayStage *pPlayStage );

public:
	PesoBonusEventUserNode();
	virtual ~PesoBonusEventUserNode();
};

//---------------------------------------------------------------------------------------------------------------------------------
// 2시간 용병 레벨제한 없이 구매하는 이벤트
class BuyCharNoLevelLimitEventUserNode : public EventUserNode
{
public:
	virtual bool IsEmptyValue();

public:
	bool IsNoLevelLimit( bool bCharBuyPeso, int iCharLimitSecond, int iCharPeriodType );
	bool IsAliveUserNode();

public:
	BuyCharNoLevelLimitEventUserNode();
	virtual ~BuyCharNoLevelLimitEventUserNode();
};

//----------------------------------------------------------------------------------------------------------------------------------
// 용병단 레벨업시 페소 지급하는 이벤트
class GradeUpEventUserNode : public EventUserNode
{
public:
	virtual bool IsEmptyValue();

public:
	bool IsGift();
	int  GetBounsPeso();

public:
	GradeUpEventUserNode();
	virtual ~GradeUpEventUserNode();
};

//-------------------------------------------------------------------------------------------------------------------------------
// 피씨방 이벤트 노드
class PCRoomEventUserNode : public EventUserNode
{
public:
	virtual bool IsEmptyValue();

public:
	PCRoomEventUserNode();
	virtual ~PCRoomEventUserNode();
};

//-------------------------------------------------------------------------------------------------------------------------------
// 찬스! 영구용병 이벤트 노드
class ChanceMortmainCharEventUserNode : public EventUserNode
{
public:
	virtual void Init();
	virtual void OnRecievePacket( SP2Packet &rkPacket, ioPlayStage *pPlayStage );

public:
	ChanceMortmainCharEventUserNode();
	virtual ~ChanceMortmainCharEventUserNode();
};

//-------------------------------------------------------------------------------------------------------------------------------
// 매일매일 골드 아이템~♬ 이벤트 노드
class OneDayGoldItemEvent : public EventUserNode
{
public:
	virtual void Init();
	virtual void OnRecievePacket( SP2Packet &rkPacket, ioPlayStage *pPlayStage );

public:
	OneDayGoldItemEvent();
	virtual ~OneDayGoldItemEvent();
};
//-------------------------------------------------------------------------------------------------------------------------------
// 휴면유저 ComeBack 보너스
class DormancyUserEvent : public EventUserNode
{
public:
	virtual bool IsEmptyValue();

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
	virtual void OnRecievePacket( SP2Packet &rkPacket, ioPlayStage *pPlayStage );

public:
	PlayTimePresentEventUserNode();
	virtual ~PlayTimePresentEventUserNode();
};
//-----------------------------------------------------------------------------------------------------------------------------------
class ChristmasEventUserNode : public EventUserNode
{
protected:
	DWORD m_dwSendTimer;

public:
	virtual void Init();
	virtual void OnRecievePacket( SP2Packet &rkPacket, ioPlayStage *pPlayStage );

	void SendRequestGift();

public:
	ChristmasEventUserNode();
	virtual ~ChristmasEventUserNode();
};
//---------------------------------------------------------------------------------------------------------------------------------
class BuyItemEventUserNode : public EventUserNode
{
public:
	virtual bool IsEmptyValue();

public:
	BuyItemEventUserNode();
	virtual ~BuyItemEventUserNode();
};
//-----------------------------------------------------------------------------------------------------------------------------------
// 본부에서 체험용병을 사용할 수 있는 이벤트
class ExerciseSoldierEventUserNode : public EventUserNode
{
public:
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
	virtual void OnRecievePacket( SP2Packet &rkPacket, ioPlayStage *pPlayStage );

public:
	ConnectionTimeEventUserNode();
	virtual ~ConnectionTimeEventUserNode();
};
//----------------------------------------------------------------------------------------------------------------------------------
class OneDayGiftEventUserNode : public EventUserNode
{
public:
	virtual void Init();

public:
	OneDayGiftEventUserNode();
	virtual ~OneDayGiftEventUserNode();
};
//----------------------------------------------------------------------------------------------------------------------------------
class GradePresentEventUserNode : public EventUserNode
{
public:
	virtual void Init();

public:
	GradePresentEventUserNode();
	virtual ~GradePresentEventUserNode();
};
//----------------------------------------------------------------------------------------------------------------------------------
class ConnectionTimeSelectGiftEventUserNode : public EventUserNode
{
public:
	virtual void Init();
	virtual void OnRecievePacket( SP2Packet &rkPacket, ioPlayStage *pPlayStage );

public:
	ConnectionTimeSelectGiftEventUserNode();
	virtual ~ConnectionTimeSelectGiftEventUserNode();
};

//-----------------------------------------------------------------------------------------------------------------------------------
class EntryEventUserNode : public EventUserNode
{
public:
	virtual void Init();
	virtual int  GetAddSize();

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
class ConnectAndPlayTimeEventUserNode : public EventUserNode
{
public:
	virtual void Init();
	virtual void OnRecievePacket( SP2Packet &rkPacket, ioPlayStage *pPlayStage );

public:
	ConnectAndPlayTimeEventUserNode();
	virtual ~ConnectAndPlayTimeEventUserNode();
};


//----------------------------------------------------------------------------------------------------------------------------------

class FreeDayEventUserNode : public EventUserNode
{
public:
	FreeDayEventUserNode();
	virtual ~FreeDayEventUserNode();
	virtual void OnRecievePacket( SP2Packet &rkPacket, ioPlayStage *pPlayStage );
};
