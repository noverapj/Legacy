#pragma once


#include "CharacterInfo.h"

// server가 가지고 있는 event
enum EventType
{
	EVT_NONE     = 0,
	EVT_PROPOSAL = 1,
	EVT_COIN     = 2,
	EVT_EXP      = 3, 
	EVT_PESO     = 4,
	EVT_PLAYTIME = 5,
	EVT_CHILDRENDAY = 6,
	EVT_PESOBONUS   = 7,
	EVT_BUY_CHAR_NO_LEVEL_LIMIT = 8,
	EVT_GRADEUP     = 9,
	EVT_PCROOM_BONUS=10,
	EVT_CHANCE_MORTMAIN_CHAR = 11,
	EVT_ONE_DAY_GOLD_ITEM = 12,
	EVT_DORMANCY_USER     = 13,
	EVT_PLAYTIME_PRESENT  = 14,
	EVT_CHRISTMAS         = 15,
	EVT_BUY_ITEM          = 16,
	EVT_FISHING			  = 17,
	EVT_EXERCISESOLDIER   = 18,
	EVT_CONNECTION_TIME   = 19,
	EVT_ONE_DAY_GIFT      = 20,
	EVT_GRADEUP_PRESENT   = 21,
	EVT_CONNECTION_TIME_SELECT_GIFT = 22,
	EVT_ENTRY             = 23,
	EVT_ONE_DAY_GIFT_2    = 24,
	EVT_BUY_ITEM_2        = 25,
	EVT_LADDER_POINT      = 26,
	EVT_BUY_ITEM_3        = 27,
	EVT_ANNOUNCE          = 28,
	EVT_ENTRY_AFTER       = 29,
	EVT_CONNECT_AND_PLAYTIME  = 30,
	EVT_ROULETTE          = 31,

	EVT_PLAZA_MONSTER		= 32,	// 광장 몬스터
	EVT_EXP2				= 33,	// 경험치2
	EVT_PES02				= 34,	// 페소2
	EVT_MODE_BONUS			= 35,	// 모드 보너스
	EVT_MODE_BONUS2			= 36,	// 모드 보너스2
	EVT_MONSTER_DUNGEON		= 37,
	EVT_PCROOM_MONSTER_DUNGEON	= 38,
	EVT_PRESENT_BUY			= 39,	

	EVT_FREEDAY_HERO = 40,
	EVT_PRESENT_BUY_2	= 41,	// 선물 시 선물 한 유저에게도 해당 아이템 지급.
};

// event vector array
enum 
{
	// ALL
	MAX_EVENT_CHECK_MS  = 10000, // millisecond

	// 아래 값들은 sp2_event.ini 의 value 값과 매칭됨.

	// EVT_PROPOSAL 
	EA_MAX_PROPOSAL_CHAR = 0,
	EA_MAX_CHAR_DAY      = 1,
	EA_MAX_CHAR_USE_SEC  = 2,

	// EVT_COIN
	EA_MAX_PLAY_SEC     = 0,
	EA_NORMAL_GIVE_CNT  = 1,
	EA_BURNING_GIVE_CNT = 2,
	EA_GRADEUP_GIVE_CNT = 3,
	EA_BURNING_HOUR_1   = 4,
	EA_BURNING_HOUR_2   = 5,

	// EVT_EXP
	EA_EXP_PER          = 0,

	// EVT_PESO 
	EA_PESO_PER         = 0,
	EA_START_WEEK       = 1,
	EA_END_WEEK         = 2,
	EA_START_TIME       = 3,
	EA_END_TIME         = 4,

	// EVT_PLAYTIME
	EA_PASS_PLAYTIME_1  = 0,
	EA_PASS_PLAYTIME_2  = 1,
	EA_PASS_PLAYTIME_3  = 2,
	EA_PASS_PLAYTIME_4  = 3,
	EA_PASS_PLAYTIME_5  = 4, 
	EA_GIFT_1           = 5,
	EA_GIFT_2           = 6,
	EA_GIFT_3           = 7,
	EA_GIFT_4           = 8,
	EA_GIFT_5           = 9,

	// EVT_PESOBONUS
	EA_BONUS_PESO       = 0,

	// EVT_CHILDRENDAY
	EA_CHILDRENDAY_IS_LIMIT = 0,

	// EVT_BUY_CHAR_NO_LEVEL_LIMIT
	EA_CHAR_LIMIT_SECOND= 0,

	// EVT_GRADEUP
	EA_GRADEUP_1_GRADE   = 0,
	EA_GRADEUP_1_PESO    = 1,
	EA_GRADEUP_2_GRADE   = 2,
	EA_GRADEUP_2_PESO    = 3,
	EA_GRADEUP_3_GRADE   = 4,
	EA_GRADEUP_3_PESO    = 5,
	EA_GRADEUP_4_GRADE   = 6,
	EA_GRADEUP_4_PESO    = 7,

	// EVT_PCROOM_BONUS
	EA_PCROOM_BONUS = 0,

	// EVT_CHANCE_MORTMAIN_CHAR
	EA_CHANCE_MORTMAIN_CHAR_TIME = 0,

	// EVT_ONE_DAY_GOLD_ITEM
	EA_GOLD_ITEM_PRESENT_LIMIT = 0,
	EA_GOLD_ITEM_PRESENT_ALARM = 1,
	EA_GOLD_ITEM_INIT_DATE_HOUR= 2,
	EA_GOLD_ITEM_PRESENT_MENT  = 3,

	// EVT_DORMANCY_USER
	EA_DORMANCY_USER_LIMIT_DATE		= 0,
	EA_DORMANCY_USER_PRESENT_LIMIT	= 1,
	EA_DORMANCY_USER_PRESENT_ALARM	= 2,
	EA_DORMANCY_USER_PRESENT_MENT	= 3,

	// EVT_PLAYTIME_PRESENT
	EA_PLAYTIME_PRESENT_TIME        = 0,

	// EVT_CHRISTMAS
	EA_CHRISTMAS_IS_ONLY_ONE_GIFT   = 0, // 1:이면 이벤트 동안 한번만 지급, 0:이면 하루에 1번 지급

	// EVT_FISHING
	EA_FS_PESO_PER         = 0,
	EA_FS_START_WEEK       = 1,
	EA_FS_END_WEEK         = 2,
	EA_FS_START_TIME       = 3,
	EA_FS_END_TIME         = 4,

	// EVT_CONNECTION_TIME
	EA_CT_CHANCE_TIME              = 0, // 선물 받을 대기 시간 ( 분 )
	EA_CT_PRESENT_LOW_USER_LEVEL   = 1, // 낮은 레벨 유저의 등급

	// EVT_ONE_DAY_GIFT , EVT_ONE_DAY_GIFT_2
	EA_OD_IS_EVERYDAY_GIFT         = 0, // 1이면 매일 아이템 지급 , 0이면 1번만 아이템 지급

	// EVT_GRADEUP_PRESENT
	EA_GP_GRADE_OF_PRESENT         = 0, 
	EA_GP_START_ENTRY_DATE         = 1,
	EA_GP_END_ENTRY_DATE           = 2,

	// EVT_CONNECTION_TIME_SELECT_GIFT
	EA_CTSG_CHANCE_TIME            = 0, // 선물 받을 대기 시간 ( 분 )

	// EVT_ENTRY
	EA_E_START_ENTRY_DATE          = 0,
	EA_E_END_ENTRY_DATE            = 1,

	// EVT_ENTRY_AFTER
	EA_EA_GIFT_ENTRY_DATE_AFTER    = 0,

	// EVT_CONNECT_AND_PLAYTIME
	EA_CAP_MAX_POINT   = 0,
	EA_CAP_MAX_SEC     = 1,
};

class EventNode
{
protected:
	bool      m_bAlive;
	EventType m_eEventType;
	IntVec    m_ValueVec;
	typedef std::vector< ChannelingType > vChannelingTypeVec;
	vChannelingTypeVec m_vUseChannelingTypeVec;

protected:
	enum AnnounceType 
	{
		AT_START = 0,
		AT_END   = 1,
	};

	struct AnnounceTimeInfo
	{
		AnnounceType m_eAnnounceType;
		DWORD m_dwWeekOnOff; // 1111122 | 일월화수목금토 | 1은 이벤트 없다 2는 이벤트 있다 | 예시는 금,토요일이벤트 있음
		WORD  m_wStartHourMin;
		WORD  m_wEndHourMin;

		AnnounceTimeInfo()
		{
			m_eAnnounceType  = AT_START;
			m_dwWeekOnOff    = 0;
			m_wStartHourMin  = 0;
			m_wEndHourMin    = 0;
		}

		bool IsEventWeek( WORD wDayOfWeek )
		{
			// wDayOfWeek : ( 0:일, 1:월, 2:화, 3:수, 4:목, 5:금, 6:토 )
			// 1211111    : 일월화수목금토 / 월요일에 이벤트 있음
			enum { EVENT_WEEK_OFF = 1, EVENT_WEEK_ON = 2, };

			int iOnOff = 0;
			if( wDayOfWeek == 0 )      // [1]211111 
				iOnOff = ( m_dwWeekOnOff / 1000000 );
			else if( wDayOfWeek == 1 ) // 1[2]11111 
				iOnOff = ( ( m_dwWeekOnOff / 100000 ) % 10 );
			else if( wDayOfWeek == 2 ) // 12[1]1111 
				iOnOff = ( ( m_dwWeekOnOff / 10000 ) % 10 );
			else if( wDayOfWeek == 3 ) // 121[1]111 
				iOnOff = ( ( m_dwWeekOnOff / 1000 ) % 10 );
			else if( wDayOfWeek == 4 ) // 1211[1]11 
				iOnOff = ( ( m_dwWeekOnOff / 100 ) % 10 );
			else if( wDayOfWeek == 5 ) // 12111[1]1 
				iOnOff = ( ( m_dwWeekOnOff / 10 ) % 10 );
			else if( wDayOfWeek == 6 ) // 121111[1] 
				iOnOff = ( m_dwWeekOnOff % 10 );
			else
				return false;

			if( EVENT_WEEK_ON == iOnOff )
				return true;

			return false;
		}
	};
	typedef std::vector< AnnounceTimeInfo > vAnnounceTimeInfoVec;

	struct AnnounceInfo
	{
		ioHashStringVec m_vStartAnnounceVec;
		DWORDVec        m_vStartColorVec;
		ioHashStringVec m_vLoopAnnounceVec;
		DWORDVec        m_vLoopColorVec;
		ioHashStringVec m_vEndAnnounceVec;
		DWORDVec        m_vEndColorVec;
		DWORD m_dwLoopAnnounceDelayTime;
		vAnnounceTimeInfoVec m_vTimeInfoVec;
		bool            m_bShutDownUser;
		
		AnnounceInfo()
		{
			m_dwLoopAnnounceDelayTime = 0;
			m_vTimeInfoVec.clear();
			m_bShutDownUser = false;
		}

		void Clear()
		{
			m_vStartAnnounceVec.clear();
			m_vStartColorVec.clear();
			m_vLoopAnnounceVec.clear();
			m_vLoopColorVec.clear();
			m_vEndAnnounceVec.clear();
			m_vEndColorVec.clear();
			m_vTimeInfoVec.clear();
		}
	};
	
	AnnounceInfo m_kAnnounceInfo;
	DWORD        m_dwAnnounceTime;
public:
	EventType GetType() const;

	int GetValue( int iArray );
	int GetValueSize() const;
	int GetUseChannelingTypeSize() const;
	ChannelingType GetUseChannelingType( int iArray );
	
	bool IsAlive( ChannelingType eUserChannelingType );

	void SetValue( int iArray , int iValue );
	void SetValueSize( int iSize );
	void SetUseChannelingType( int iArray, ChannelingType eType );
	void SetUseChannelingTypeSize( int iSize );
	void SetType( EventType eEventType );
	void SetAlive( bool bAlive );

	void Process( int iWeek, int iHour, int iMin );
	void LoadINI( ioINILoader &a_rkLoader );

public:
	EventNode();
	virtual ~EventNode();
};

//------------------------------------------------------------------------
class ioEventManager : public Singleton< ioEventManager >
{
protected:
	typedef std::vector< EventNode* > EventNodeVec;
	int          m_iMaxDBValue;
	EventNodeVec m_EventNodeVec;
	
	DWORD        m_dwProcessTime;
	bool m_bIsUnablePresentBonus;
	struct UnablePresentInfo
	{
		int event_type;
		bool is_unable_present_bonus;

		UnablePresentInfo( int _event_type, bool _is_unable_present_bonus )
		{
			event_type = _event_type;
			is_unable_present_bonus = _is_unable_present_bonus;
		}
	};
	typedef std::vector<UnablePresentInfo> vUnablePresentInfo;
	vUnablePresentInfo m_vUnablePresentInfo;

	void Clear();

public: 
	int GetSize() const;
	int GetMaxDBValue() const { return m_iMaxDBValue; }
	void SetSize( int iSize );
	void SetMaxDBValue(int iMaxDBValue) { m_iMaxDBValue = iMaxDBValue; }

	EventType GetType( int iArray );
	bool IsAlive( EventType eEventType, ChannelingType eChannelingType );
	int GetValue( EventType eEventType, int iArray );
	int GetValueSize( EventType eEventType );
	ChannelingType GetUseChannelingType( EventType eEventType, int iArray );
	int GetUseChannelingTypeSize( EventType eEventType );

	void SetValue( EventType eEventType, int iArray, int iValue );
	void SetValueSize( EventType eEventType, int iSize );
	void SetUseChannelingType( EventType eEventType, int iArray, ChannelingType eType );
	void SetUseChannelingTypeSize( EventType eEventType, int iSize );
	void SetEventType( int iArray , EventType eEventType );
	void SetAlive( EventType eEventType, bool bAlive );

	void Process();
	void LoadINI();

	void SetPresentBonus( int iEventType, bool bUnablePresentBonus );
	bool IsPresentBonus( int iEventType );

	static ioEventManager& GetSingleton();

public:
	ioEventManager(void);
	virtual ~ioEventManager(void);
};

#define g_EventMgr ioEventManager::GetSingleton()
