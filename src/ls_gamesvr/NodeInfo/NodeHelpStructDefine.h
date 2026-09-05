#include "..\Network\SP2Packet.h"


#ifndef _NodeHelpStructDefine_h_
#define _NodeHelpStructDefine_h_

extern CLog LOG;

enum EntryType 
{
	ET_TEMPORARY      =  1, 
	ET_FORMALITY      =  5, 
	ET_TERMINATION    = -1,  // 게임서버에서만 존재하는 타입, DB에는 존재하지 않음.
	ET_FORMALITY_CASH =  10, // 정식가입 유저로서 캐쉬 충전도 가능한 유저
};

enum
{
	USER_TYPE_NORMAL       = 0,    // 일반 유저
	USER_TYPE_BROADCAST_AFRICA = 1,    // 2009.02.19 아프리카 방송 이벤트 주최 유저
	USER_TYPE_LIMIT_IGNORE = 2,    // 용병과 치장의 제한으로 부터 자유로운 타입
	USER_TYPE_BROADCAST_MBC= 3,    // 2009.03.04 MBC 게임 방송 이벤트 주최 유저
};

enum
{
	HERO_SEASON_RANK_1 = 0,    //영웅전 시즌 랭킹 최근 첫 시즌
	HERO_SEASON_RANK_2,		   //영웅전 시즌 랭킹 최근 두번째 시즌
	HERO_SEASON_RANK_3,		   //영웅전 시즌 랭킹 최근 세번째 시즌
	HERO_SEASON_RANK_4,		   //영웅전 시즌 랭킹 최근 네번째 시즌
	HERO_SEASON_RANK_5,		   //영웅전 시즌 랭킹 최근 다섯번째 시즌
	HERO_SEASON_RANK_6,		   //영웅전 시즌 랭킹 최근 여섯번째 시즌
	HERO_SEASON_RANK_MAX,
};

// 성별. 구조체 CHARACTER::m_sex 에서 사용.
enum GenderType
{
	GT_MAN		= 1,
	GT_WOMAN	= 2,
	GT_MAX		= 3,
};

struct RecordData
{
	int m_iWin;
	int m_iLose;
	int m_iKill;
	int m_iDeath;
	int m_iContinuousKill;
	RecordData()
	{
		Init();
	}
	void Init()
	{
		m_iWin = m_iLose = 0;
		m_iKill = m_iDeath = 0;
		m_iContinuousKill = 0;
	}
};

struct USERDATA          //2007.05.29 LJH
{
	DWORD	m_user_idx;						//유저 고유키
	ioHashString	m_private_id;			//로그인 아이디
	ioHashString    m_public_id;            //게임 아이디
	int		m_cash;							//유저 캐쉬
	__int64	m_money;						//유저 머니  EX.서프의 코드 정도.
	int     m_connect_count;                //접속 횟수.
	CTime   m_connect_time;                 //최근 접속 시간 - 접속중인 상태에서 변경하지 말것!!!!!!!!
	int     m_user_state;                   //훈련하기 정보.
	int     m_grade_level;                  //계급
	int     m_grade_exp;                    //계급 구간 경험치
	int		m_fishing_level;				//낚시 레벨
	int		m_fishing_exp;					//낚시 구간 경험치
	int     m_user_event_type;              //특수한 유저 이벤트 타입
	int     m_user_rank;                    //전체 랭킹    
	EntryType m_eEntryType;                 //임시가입, 정식가입(실명확인), 임시가입 만료 ( UserMemberDB.joinType , -1값은 DB에 존재하지 않음-게임서버가설정-)
	int     m_camp_position;                //소속 진영 : (0:무소속)(1:연합군)(2:제국군)
	int     m_ladder_point;                 //래더 포인트
	int     m_accumulation_ladder_point;    //누적 래더 포인트
	int     m_camp_rank;                    //진영 랭킹
	ChannelingType m_eChannelingType;       //채널링 : 일반, mgame, 등
	ioHashString   m_szChannelingUserID;    //채널링사의 ID
	ioHashString   m_szChannelingUserNo;    //채널링사의 유저식별no : naver에서 사용
	int            m_iChannelingCash;       //채널링사에 제공하는 Cash나 머니
	BlockType      m_eBlockType;            //차단타입
	CTime          m_kBlockTime;            //차단날짜
	int            m_refill_data;           //리필받기까지 남은 초
	bool           m_bSavePossible;         //처음 로그인시 모든 용병 정보까지 받으면 true고 그 뒤로는 서버 이동해도 항상 true다.
	int            m_purchased_cash;        //유저가 현금을 주고 구매한 캐쉬 m_cash중 일부 금액
	int            m_iExcavationLevel;      //발굴레벨
	int            m_iExcavationExp;        //발굴 구간 경험치
	int            m_iAccrueHeroExpert;     //영웅전 누적 경험치
	int		       m_iHeroExpert;           //영웅전 경험치
	short		   m_sPractice;           //영웅전 경험치
	CTime		   m_login_time;			//로그인 한 시간.
	__int64		   m_iTimeGateOpenTime;		//타임 게이트 연 시간

	//hr 북미추가
	DWORD		   m_dwUSMemberIndex;
	ioHashString   m_USMemberID;
	DWORD		   m_USMemberType;

	USERDATA()
	{
		Initialize();
	}
	
	void Initialize()
	{
		m_user_idx		  = 0;				
		m_private_id.Clear();
		m_public_id.Clear();
		m_cash			  = 0;
		m_money			  = 0;				
		m_connect_count	  = 0;           
		m_user_state      = 0;
		m_grade_level     = 0;
		m_grade_exp		  = 0;      
		m_fishing_level	  = 0;
		m_fishing_exp	  = 0;
		m_user_event_type = USER_TYPE_NORMAL;
		m_user_rank       = 0;
		m_eEntryType      = ET_TEMPORARY;
		m_camp_position   = 0;
		m_ladder_point    = 0;
		m_accumulation_ladder_point = 0;
		m_camp_rank       = 0;
		m_eChannelingType = CNT_NONE;
		m_szChannelingUserID.Clear();
		m_szChannelingUserNo.Clear();
		m_iChannelingCash = 0;
		m_eBlockType      = BKT_NONE;
		m_refill_data     = 0;
		m_bSavePossible   = false;
		m_purchased_cash  = 0;
		m_iExcavationLevel= 0;
		m_iExcavationExp  = 0;
		m_iAccrueHeroExpert = 0;
		m_iHeroExpert     = 0;
		m_sPractice			= 0;

		m_dwUSMemberIndex	= 0;
		m_USMemberID.Clear();
		m_USMemberType = 0;
	}

	bool IsChange()
	{		
		if( m_user_idx == 0 )
			return false;
		return m_bSavePossible;
	}
};

struct UserRelativeGradeData
{
	int  m_init_code;     //상대적 계급 경험치 갱신 코드
	bool m_enable_reward; //상대적 계급 보상이 있는가?
	int  m_iBackupExp;

	UserRelativeGradeData()
	{
		Initialize();
	}

	void Initialize()
	{
		m_init_code = 0;
		m_enable_reward = 0;
		m_iBackupExp = 0;
	}

	void FillData( SP2Packet &rkPacket )
	{
		rkPacket << m_init_code;
		rkPacket << m_enable_reward;
		rkPacket << m_iBackupExp;
	}

	void ApplyData( SP2Packet &rkPacket )
	{
		rkPacket >> m_init_code;
		rkPacket >> m_enable_reward;
		rkPacket >> m_iBackupExp;
	}
};

struct UserHeroData
{
	int            m_iHeroTitle;            //영웅전 칭호
	int            m_iHeroTodayRank;        //영웅전 일일 랭킹
	int            m_iHeroSeasonRank[HERO_SEASON_RANK_MAX];       //영웅전 시즌 랭킹

	UserHeroData()
	{
		Initialize();
	}

	void Initialize()
	{
		m_iHeroTitle      = 0;
		m_iHeroTodayRank  = 0;
		for (int i = 0; i < HERO_SEASON_RANK_MAX ; i++)
			m_iHeroSeasonRank[i] = 0;
	}
};

enum
{
	MAX_DISPLAY_CNT = 10,
};
struct UserHeadquartersOption
{
	// 상위 옵션
	short m_sLock;

	// 진열 옵션
	DWORD m_dwCharacterIndex[MAX_DISPLAY_CNT];
	int   m_iCharacterXPos[MAX_DISPLAY_CNT];
	int   m_iCharacterZPos[MAX_DISPLAY_CNT];

	bool  m_bChangeOption;

	UserHeadquartersOption()
	{
		Initialize();		
	}

	void Initialize()
	{
		m_sLock = 0;
		m_bChangeOption = false;

		for(int i = 0;i < MAX_DISPLAY_CNT;i++)
		{
			m_dwCharacterIndex[i] = 0;
			m_iCharacterXPos[i] = m_iCharacterZPos[i] = 0;
		}
	}

	void FillData( SP2Packet &rkPacket )
	{
		rkPacket << m_sLock;
		rkPacket << m_bChangeOption;

		for (int i = 0; i < MAX_DISPLAY_CNT ; i++)
		{
			rkPacket << m_dwCharacterIndex[i] << m_iCharacterXPos[i] << m_iCharacterZPos[i]; 
		}
	}

	void ApplyData( SP2Packet &rkPacket )
	{
		rkPacket >> m_sLock;
		rkPacket >> m_bChangeOption;

		for (int i = 0; i < MAX_DISPLAY_CNT ; i++)
		{
			rkPacket >> m_dwCharacterIndex[i] >> m_iCharacterXPos[i] >> m_iCharacterZPos[i]; 
		}
	}
};

struct ITEM_DATA          //2007.05.29 LJH
{ 
	int		m_item_code;             //아이템 종류
	int  	m_item_reinforce;	     //강화 값
	DWORD   m_item_male_custom;      //남성 커스텀
	DWORD   m_item_female_custom;    //여성 커스텀
	
	ITEM_DATA()
	{
		Initialize();
	}

	void Initialize()
	{
		m_item_code = 0;
		m_item_reinforce = 0;
		m_item_male_custom = m_item_female_custom = 0;
	}	

	ITEM_DATA& operator = ( const ITEM_DATA &rhs )
	{
		m_item_code			= rhs.m_item_code;
		m_item_reinforce	= rhs.m_item_reinforce;
		m_item_male_custom	= rhs.m_item_male_custom;
		m_item_female_custom= rhs.m_item_female_custom;
		return *this;
	}
};
typedef std::vector< ITEM_DATA > ITEM_DATAVec;

struct CharCostume
{
	int m_iCostumeIndex;
	int m_iCostumeCode;

	CharCostume()
	{
		m_iCostumeIndex	= 0;
		m_iCostumeCode	= 0;
	}
};

struct CharAccessory
{
	int m_iAccessoryIndex;
	int m_iAccessoryCode;
	int m_iAccessoryValue;

	int m_iComposeCode;
	int m_iComposeValue;

	CharAccessory()
	{
		m_iAccessoryIndex	= 0;
		m_iAccessoryCode	= 0;
		m_iAccessoryValue	= 0;

		m_iComposeCode		= 0;
		m_iComposeValue		= 0;
	}
};

struct ITEMSLOT          //2007.08.25 LJH 
{
	int		m_item_type;     //고유 특성 1111 22 333 ( 세트, 종족(성별), 치장타입 ) (UserItemDB : itemX_type)
	int  	m_item_code;	 //아이템 치장 타입      ( 치장 고유 번호 0 ~ 9999) (UserItemDB: itemX_code 의 천자리까지)
	int     m_item_equip;    //아이템 장착 여부      ( 0:미장착, 1:장착 ) (UserItemDB: itemX_code  의 만자리)           
                             //ex) UserItemDb item1_code 10005 ( 5번 치장 타입을 장착함 )
	ITEMSLOT()
	{
		Initialize();
	}

	void Initialize()
	{
		m_item_type		= 0;
		m_item_code     = 0;
		m_item_equip    = 0;
	}
};
typedef std::vector< ITEMSLOT > ITEMSLOTVec;

// 기간제 & 영구
enum  CharPeriodType
{
	CPT_TIME     = 0,
	CPT_MORTMAIN = 1,
	CPT_DATE	= 2, // 라틴의 기간제 용병을 위해 추가
	CPT_MAX,
};

// 대표 용병 설정
enum
{
	CLT_GENERAL= 0,
	CLT_LEADER = 1,
};

// 대여 용병
enum
{
	CRT_GENERAL= 0,
	CRT_RENTAL = 1,
};

// 각성 
enum
{
	AWAKE_NONE      = 0,
	AWAKE_NORMAL	= 1,
	AWAKE_RARE      = 2
};

enum
{
	DEFAULT_YEAR    = 2010,			// 2010년은 DB에 저장하지 않는다. 즉 DateData의 년도가 0이면 2010이란 뜻이다. 1이면 2011년
	DATE_YEAR_VALUE = 100000000,    // 년까지 나눈다.
	DATE_MONTH_VALUE= 1000000,      // 월까지 나눈다.
	DATE_DAY_VALUE =  10000,        // 일까지 나눈다.
	DATE_HOUR_VALUE = 100,          // 시까지 나눈다.
};

struct CHARACTER                        //2007.05.29 LJH 
{
	// 변수 추가시에 FillData(), ApplayData(), Init() 함수도 수정해야함.
	int m_class_type;       //클래스 타입 : 세트 아이템 타입
	int	m_kindred;          //종족 1(휴먼)2(엘프)3(드워프)
	int	m_sex;              //성별 1(남자)2(여자)
	int	m_beard;            //수염 1(드워프수염) 2,3,4(남자 공통)
	int	m_face;             //얼굴 1(종족별남여기본)
	int	m_hair;		        //머리 1(종족별남여기본)
	int	m_skin_color;       //피부색 1,2,3,4
	int	m_hair_color;       //머리색 1,2,3
	int	m_accessories;      //장신구 1
	int m_underwear;        //속옷   1,2,.....

	int m_extra_item[MAX_CHAR_DBITEM_SLOT]; // 아이템 부위별 정보

	int m_iSlotIndex;       //용병 슬롯에서의 위치
	int m_iLimitSecond;     //남은 기간

    short m_sLeaderType;          // 대표 용병
	short m_sRentalType;          // 대여 상태
	DWORD m_dwRentalMinute;       // 대여중 시간
	CharPeriodType m_ePeriodType; // 무제한용병, 기간제용병

	bool m_bActive;         //사용 가능한 용병.
	BYTE m_chExerciseStyle; //체험 스타일 

	BYTE m_chAwakeType;		//각성 여부, 각성 종류 ( 0:NONE, 1:NORMAL, 2:RARE )
    int m_iAwakeLimitTime;	//각성 종료 일

	BYTE m_byReinforceGrade;

	CharCostume m_costume_item[MAX_CHAR_COSTUME_SLOT];
	CharAccessory m_accessory_item[MAX_CHAR_ACCESSORY_SLOT];

	CHARACTER()
	{
		Init();
	}

	void Init()
	{
		m_class_type    = 0;
		m_kindred		= 1;
		m_sex			= 1;
		m_face			= 1;
		m_hair			= 1;
		m_skin_color	= 1;
		m_hair_color	= 1;
		m_underwear     = 1;

		m_beard			= 0;     //현재 사용하는 곳 없음
		m_accessories	= 0;     //현재 사용하는 곳 없음

		for (int i = 0; i < MAX_CHAR_DBITEM_SLOT ; i++)
			m_extra_item[i] = 0;

		m_iSlotIndex    = -1;
		m_iLimitSecond  = 0;

		m_sLeaderType  = CLT_GENERAL;
		m_sRentalType  = CRT_GENERAL;
		m_dwRentalMinute = 0;
		m_ePeriodType  = CPT_TIME;

		m_bActive       = true;
		m_chExerciseStyle = 0x00;
		m_chAwakeType	  = AWAKE_NONE;		
		m_iAwakeLimitTime = 0;
		m_byReinforceGrade = 0;

		for( int i = 0; i < MAX_CHAR_COSTUME_SLOT; i++ )
		{
			m_costume_item[i].m_iCostumeIndex = 0;
			m_costume_item[i].m_iCostumeCode  = 0;
		}

		for( int i = 0; i < MAX_CHAR_ACCESSORY_SLOT; i++ )
		{
			m_accessory_item[i].m_iAccessoryIndex = 0;
			m_accessory_item[i].m_iAccessoryCode  = 0;
			m_accessory_item[i].m_iAccessoryValue  = 0;
			m_accessory_item[i].m_iComposeCode  = 0;
			m_accessory_item[i].m_iComposeValue  = 0;
		}
	}

// 	void FillData( SP2Packet &rkPacket )
// 	{
// 		rkPacket << m_class_type;
// 		rkPacket << m_kindred;
// 		rkPacket << m_sex;			
// 		rkPacket << m_beard;		
// 		rkPacket << m_face;			
// 		rkPacket << m_hair;			
// 		rkPacket << m_skin_color;	
// 		rkPacket << m_hair_color;	
// 		rkPacket << m_accessories;	
// 		rkPacket << m_underwear;    
// 
// 		for (int i = 0; i < MAX_CHAR_DBITEM_SLOT ; i++)
// 			rkPacket << m_extra_item[i]; 
// 
// 		rkPacket << m_iSlotIndex;    
// 		rkPacket << m_iLimitSecond;  
// 
// 		rkPacket << m_sLeaderType;
// 		rkPacket << m_sRentalType;
// 		rkPacket << m_dwRentalMinute;
// 		rkPacket << (int)m_ePeriodType;   
// 
// 		rkPacket << m_bActive;       
// 		rkPacket << m_chExerciseStyle;   
// 	}

	bool FillData( SP2Packet &rkPacket )
	{
		PACKET_GUARD_bool( rkPacket.Write(m_class_type) );
		PACKET_GUARD_bool( rkPacket.Write(m_kindred) );
		PACKET_GUARD_bool( rkPacket.Write(m_sex) );		
		PACKET_GUARD_bool( rkPacket.Write(m_beard) );	
		PACKET_GUARD_bool( rkPacket.Write(m_face) );		
		PACKET_GUARD_bool( rkPacket.Write(m_hair) );		
		PACKET_GUARD_bool( rkPacket.Write(m_skin_color) );
		PACKET_GUARD_bool( rkPacket.Write(m_hair_color) );
		PACKET_GUARD_bool( rkPacket.Write(m_accessories) );
		PACKET_GUARD_bool( rkPacket.Write(m_underwear) );

		for (int i = 0; i < MAX_CHAR_DBITEM_SLOT ; i++)
		{
			PACKET_GUARD_bool( rkPacket.Write(m_extra_item[i]) ); 
		}

		PACKET_GUARD_bool( rkPacket.Write(m_iSlotIndex) );
		PACKET_GUARD_bool( rkPacket.Write(m_iLimitSecond) );  

		PACKET_GUARD_bool( rkPacket.Write(m_sLeaderType) );
		PACKET_GUARD_bool( rkPacket.Write(m_sRentalType) );
		PACKET_GUARD_bool( rkPacket.Write(m_dwRentalMinute) );
		PACKET_GUARD_bool( rkPacket.Write((int)m_ePeriodType) );

		PACKET_GUARD_bool( rkPacket.Write(m_bActive) );
		PACKET_GUARD_bool( rkPacket.Write(m_chExerciseStyle) );

		PACKET_GUARD_bool( rkPacket.Write( m_chAwakeType ) );
		PACKET_GUARD_bool( rkPacket.Write( m_iAwakeLimitTime ) );

		PACKET_GUARD_bool( rkPacket.Write( m_byReinforceGrade ) );

		for( int i = 0; i < MAX_CHAR_COSTUME_SLOT; i++ )
		{
			PACKET_GUARD_bool( rkPacket.Write(m_costume_item[i].m_iCostumeIndex) );
			PACKET_GUARD_bool( rkPacket.Write(m_costume_item[i].m_iCostumeCode) );
		}

		for( int i = 0; i < MAX_CHAR_ACCESSORY_SLOT; i++ )
		{
			PACKET_GUARD_bool( rkPacket.Write(m_accessory_item[i].m_iAccessoryIndex) );
			PACKET_GUARD_bool( rkPacket.Write(m_accessory_item[i].m_iAccessoryCode) );
			PACKET_GUARD_bool( rkPacket.Write(m_accessory_item[i].m_iAccessoryValue) );
			PACKET_GUARD_bool( rkPacket.Write(m_accessory_item[i].m_iComposeCode) );
			PACKET_GUARD_bool( rkPacket.Write(m_accessory_item[i].m_iComposeValue) );
		}
		return true;
	}

	bool ApplyData( SP2Packet &rkPacket )
	{
		PACKET_GUARD_bool( rkPacket.Read(m_class_type) );
		PACKET_GUARD_bool( rkPacket.Read(m_kindred) );
		PACKET_GUARD_bool( rkPacket.Read(m_sex) );
		PACKET_GUARD_bool( rkPacket.Read(m_beard) );
		PACKET_GUARD_bool( rkPacket.Read(m_face) );
		PACKET_GUARD_bool( rkPacket.Read(m_hair) );
		PACKET_GUARD_bool( rkPacket.Read(m_skin_color) );
		PACKET_GUARD_bool( rkPacket.Read(m_hair_color) );
		PACKET_GUARD_bool( rkPacket.Read(m_accessories) );
		PACKET_GUARD_bool( rkPacket.Read(m_underwear) );

		for (int i = 0; i < MAX_CHAR_DBITEM_SLOT ; i++)
		{
			PACKET_GUARD_bool( rkPacket.Read(m_extra_item[i]) ); 	
			
		}
		PACKET_GUARD_bool( rkPacket.Read(m_iSlotIndex) );
		PACKET_GUARD_bool( rkPacket.Read(m_iLimitSecond) );  

		PACKET_GUARD_bool( rkPacket.Read(m_sLeaderType) );
		PACKET_GUARD_bool( rkPacket.Read(m_sRentalType) );
		PACKET_GUARD_bool( rkPacket.Read(m_dwRentalMinute) );

		int iPeriodType = 0;
		PACKET_GUARD_bool( rkPacket.Read(iPeriodType) );
		m_ePeriodType = (CharPeriodType) iPeriodType;   

		PACKET_GUARD_bool( rkPacket.Read(m_bActive) );
		PACKET_GUARD_bool( rkPacket.Read(m_chExerciseStyle) );

		PACKET_GUARD_bool( rkPacket.Read( m_chAwakeType ) );
		PACKET_GUARD_bool( rkPacket.Read( m_iAwakeLimitTime ) );

		PACKET_GUARD_bool( rkPacket.Read( m_byReinforceGrade ) );

		for( int i = 0; i < MAX_CHAR_COSTUME_SLOT; i++ )
		{
			PACKET_GUARD_bool( rkPacket.Read(m_costume_item[i].m_iCostumeIndex) );
			PACKET_GUARD_bool( rkPacket.Read(m_costume_item[i].m_iCostumeCode) );
		}

		for( int i = 0; i < MAX_CHAR_ACCESSORY_SLOT; i++ )
		{
			PACKET_GUARD_bool( rkPacket.Read(m_accessory_item[i].m_iAccessoryIndex) );
			PACKET_GUARD_bool( rkPacket.Read(m_accessory_item[i].m_iAccessoryCode) );
			PACKET_GUARD_bool( rkPacket.Read(m_accessory_item[i].m_iAccessoryValue) );
			PACKET_GUARD_bool( rkPacket.Read(m_accessory_item[i].m_iComposeCode) );
			PACKET_GUARD_bool( rkPacket.Read(m_accessory_item[i].m_iComposeValue) );
		}
		return true;
	}
};

enum StatType
{
	STAT_NONE,
	STAT_STR,
	STAT_DEX,
	STAT_INT,
	STAT_VIT,
};

struct Stat
{
	union
	{
		struct
		{
			float	m_fStrength;
			float	m_fDexterity;
			float	m_fIntelligence;
			float	m_fVitality;
		};

		float m_fStat[4];
	};

	Stat()
	{
		Initialize();
	}

	void Initialize()
	{
		m_fStrength = 0.0f;
		m_fDexterity = 0.0f;
		m_fIntelligence = 0.0f;
		m_fVitality = 0.0f;
	}

	void ZeroBound()
	{
		m_fStrength     = max( m_fStrength, 0.0f );
		m_fDexterity    = max( m_fDexterity, 0.0f );
		m_fIntelligence = max( m_fIntelligence, 0.0f );
		m_fVitality     = max( m_fVitality, 0.0f );
	}

	Stat& operator += ( const Stat &rhs )
	{
		m_fStrength += rhs.m_fStrength;
		m_fDexterity += rhs.m_fDexterity;
		m_fIntelligence += rhs.m_fIntelligence;
		m_fVitality += rhs.m_fVitality;

		return *this;
	}
	
	Stat& operator -= ( const Stat &rhs )
	{
		m_fStrength -= rhs.m_fStrength;
		m_fDexterity -= rhs.m_fDexterity;
		m_fIntelligence -= rhs.m_fIntelligence;
		m_fVitality -= rhs.m_fVitality;

		return *this;
	}
};

enum RaceDetailType
{
	RDT_HUMAN_MAN,
	RDT_HUMAN_WOMAN,
	RDT_ELF_MAN,
	RDT_ELF_WOMAN,
	RDT_DWARF_MAN,
	RDT_DWARF_WOMAN,
	MAX_RACE_DETAIL
};

enum ObejctCreatorType
{
	OCT_NONE,
	OCT_SOILDER,
	OCT_EQUIP_SKILL,
	OCT_EQUIP_BUFF1,
	OCT_EQUIP_BUFF2,
};


#endif