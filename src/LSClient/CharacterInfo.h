#pragma once

#include "EncryptValue.h"
#include "SP2Packet.h"


enum EntryType 
{
	ET_TEMPORARY      =  1,
	ET_FORMALITY      =  5,
	ET_TERMINATION    = -1, // 게임서버에서만 존재하는 타입, DB에는 존재하지 않음.
	ET_FORMALITY_CASH = 10, // 정식가입 유저로서 캐쉬 충전도 가능한 유저
};

enum
{
	USER_TYPE_NORMAL       = 0,    // 일반 유저
	USER_TYPE_BROADCAST_AFRICA = 1,    // 2009.02.19 아프리카 방송 이벤트 주최 유저
	USER_TYPE_LIMIT_IGNORE = 2,    // 용병과 치장의 제한으로 부터 자유로운 타입
	USER_TYPE_BROADCAST_MBC= 3,    // 2009.03.04 MBC 게임 방송 이벤트 주최 유저
};

// DB에 저장되는 값으로 순차적으로 증가하지 않는다.
enum ChannelingType
{
	CNT_NONE		= -1,
	CNT_WEMADEBUY	= 0,
	CNT_MGAME		= 300,
	CNT_DAUM		= 400,
	CNT_NAVER		= 600,
	CNT_TOONILAND	= 700,
	CNT_NEXON		= 800,
	CNT_HANGAME		= 900,
	CNT_VALOFE		= 1000,
	CNT_HAPPYTALK	= 1100,
	CNT_MAX,
};

// DB에 저장되는 값으로 순차적으로 증가하지 않는다.
enum BlockType
{
	BKT_NONE        = -1,
	BKT_NORMAL      =  0,   // 차단안됨 (정상)
	BKT_WARNNING    = 10,   // 경고 ( 약 )
	BKT_LIMITATION  = 20,   // 제한 ( 중 )
	BKT_BLOCK       = 100,  // 차단 ( 강 )
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

struct RecordData
{
	int m_iWin;
	int m_iLose;
	int m_iKill;
	int m_iDeath;
	RecordData()
	{
		Init();
	}
	void Init()
	{
		m_iWin = m_iLose = 0;
		m_iKill = m_iDeath = 0;
	}
};

struct USERDATA           //2007.05.29 LJH
{
	DWORD			m_user_idx;				//유저 고유키
	ioHashString	m_private_id;		    //유저 닉네임
	ioHashString    m_public_id;            //닉네임
	int		m_cash;							//유저 캐쉬
	__int64	m_money;						//유저 머니  EX.서프의 코드 정도.
	int     m_connect_count;                //접속 횟수.
	DWORD   m_connect_date;                 //최종 접속일
	int     m_user_state;                   //훈련 정보
	int     m_grade_level;                  //계급
	int     m_grade_exp;                    //계급 구간 경험치
	int		m_fishing_level;				//낚시 레벨
	int		m_fishing_exp;					//낚시 구간 경험치
	int     m_user_event_type;              //특수한 유저 이벤트 타입
	int     m_user_rank;                    //전체 랭킹
	EntryType m_eEntryType;					//임시가입, 정식가입(실명확인), 임시가입 만료 ( UserMemberDB.joinType , -1값은 DB에 존재하지 않음-게임서버가설정-)
	int     m_camp_position;                //소속 진영 : (0:무소속)(1:정부군)(2:반란군)
	int     m_ladder_point;                 //대회 포인트
	int     m_camp_rank;                    //진영 랭킹
	ChannelingType m_eChannelingType;       //채널링 : 일반, mgame, 등
	ioHashString   m_szChannelingUserNo;    //채널링사의 유저식별no : naver에서 사용
	ioHashString   m_szChannelingUserID;    //채널링사의 ID
	int            m_iChannelingCash;       //채널링사에 제공하는 Cash나 머니
	BlockType      m_eBlockType;            //차단타입
	DWORD          m_dwBlockYearMonthDay;   //차단날짜
	DWORD          m_dwBlockHourMinute;     //차단시간
	int            m_refill_data;           //리필받기까지 남은 초
	int            m_purchased_cash;        //유저가 현금을 주고 구매한 캐쉬 m_cash중 일부 금액
	int            m_iExcavationLevel;      //발굴레벨
	int            m_iExcavationExp;        //발굴 구간 경험치
	int            m_iAccrueHeroExpert;     //영웅전 누적 경험치
	int		       m_iHeroExpert;           //영웅전 경험치
	int			   m_iBonusCash;			//보너스 캐쉬

	USERDATA()
	{
		m_user_idx      = 0;
		m_private_id.Clear();
		m_public_id.Clear();
		m_cash			= 0;
		m_money			= 0;
		m_connect_count	= 0;
		m_connect_date	= 0;
		m_user_state    = 0;
		m_grade_level   = 0;
		m_grade_exp		= 0;      
		m_fishing_level	  = 0;
		m_fishing_exp	  = 0;
		m_user_event_type = USER_TYPE_NORMAL;
		m_user_rank     = 0;
		m_eEntryType    = ET_TEMPORARY;
		m_camp_position = 0;
		m_ladder_point  = 0;
		m_camp_rank     = 0;
		m_eChannelingType   = CNT_NONE;
		m_szChannelingUserID.Clear();
		m_szChannelingUserNo.Clear();
		m_iChannelingCash   = 0;
		m_eBlockType        = BKT_NONE;
		m_dwBlockYearMonthDay = 0;
		m_dwBlockHourMinute   = 0;
		m_refill_data = 0;
		m_purchased_cash  = 0;
		m_iExcavationLevel= 0;
		m_iExcavationExp  = 0;
		m_iAccrueHeroExpert = 0;
		m_iHeroExpert     = 0;
		m_iBonusCash = 0;
	}
};

struct UserRelativeGradeData
{
	int   m_init_code;     //상대적 계급 경험치 갱신 코드
	bool  m_enable_reward; //상대적 계급 보상이 있는가?

	UserRelativeGradeData()
	{
		m_init_code = 0;
		m_enable_reward = 0;
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

	UserHeadquartersOption()
	{
		Initialize();		
	}

	void Initialize()
	{
		m_sLock = 0;

		for(int i = 0;i < MAX_DISPLAY_CNT;i++)
		{
			m_dwCharacterIndex[i] = 0;
			m_iCharacterXPos[i] = m_iCharacterZPos[i] = 0;
		}
	}

	void ApplyData( SP2Packet &rkPacket )
	{
		rkPacket >> m_sLock;
		for (int i = 0; i < MAX_DISPLAY_CNT ; i++)
		{
			rkPacket >> m_dwCharacterIndex[i] >> m_iCharacterXPos[i] >> m_iCharacterZPos[i]; 
		}
	}
};

struct ITEM_DATA          //2007.05.29 LJH
{ 
	CEncrypt<int>		m_item_code;             //아이템 종류
	CEncrypt<int>	  	m_item_reinforce;        //강화
	int		m_item_bullet;			 //Bullet
	DWORD   m_item_male_custom;      //남성 커스텀
	DWORD   m_item_female_custom;    //여성 커스텀

	ITEM_DATA()
	{
		Initialize();
	}

	void Initialize()
	{
		m_item_code		 = 0;
		m_item_reinforce = 0;
		m_item_bullet	 = -1;
		m_item_male_custom = m_item_female_custom = 0;
	}	

	ITEM_DATA& operator = ( const ITEM_DATA &rhs )
	{
		m_item_code			= rhs.m_item_code;
		m_item_reinforce	= rhs.m_item_reinforce;
		m_item_bullet		= rhs.m_item_bullet;
		m_item_male_custom	= rhs.m_item_male_custom;
		m_item_female_custom= rhs.m_item_female_custom;
		return *this;
	}
};

struct ITEMSLOT          //2007.08.25 LJH 
{
	int  	m_item_type;             //고유 특성 1111 22 333 ( 세트, 종족(성별), 치장타입 )
	int		m_item_code;	         //아이템 치장 타입      ( 치장 고유 번호 )
	int     m_item_equip;    //아이템 장착 여부      ( 0:미장착, 1:장착 ) (UserItemDB: itemX_code  의 만자리)     

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

//각성 여부
enum
{
	AwakeTypeCount = 2
};

enum AwakeType
{
	AWAKE_NONE		= 0,
	AWAKE_NORMAL	= 1,
	AWAKE_RARE		= 2,
	AWAKE_MAX		= 3,
};

struct CHARACTER                        //2007.05.29 LJH 
{
	// 변수 추가시에 FillData(), ApplayData(), operator=, operator==, Init() 함수도 수정해야함. 
	// 암호변수 추가로 CHARACTER구조체의 memset, memcpy, memcmp 사용 금지

	CEncrypt<int>	m_class_type;       //클래스 타입 : 세트 아이템 타입
	CEncrypt<int>	m_kindred;          //종족 1(휴먼)2(엘프)3(드워프)
	CEncrypt<int>	m_sex;              //성별 1(남자)2(여자)
	CEncrypt<int>	m_beard;            //수염 1(드워프수염) 2,3,4(남자 공통)
	CEncrypt<int>	m_face;             //얼굴 1(종족별남여기본)
	CEncrypt<int>	m_hair;		        //머리 1(종족별남여기본)
	CEncrypt<int>	m_skin_color;       //피부색 1,2,3,4
	CEncrypt<int>	m_hair_color;       //머리색 1,2,3
	CEncrypt<int>	m_accessories;      //장신구 1
	CEncrypt<int>   m_underwear;        //속옷   1,2,.....
	
	CEncrypt<int>   m_extra_item[MAX_INVENTORY];       // 아이템 부위별 추가 장비

	int m_iSlotIndex;       //용병 슬롯에서의 위치
	int m_iLimitSecond;     //남은 기간

	short m_sLeaderType;          // 대표 용병
	short m_sRentalType;          // 대여 상태 
	DWORD m_dwRentalMinute;       // 대여중 시간
	CharPeriodType m_ePeriodType; // 무제한용병, 기간제용병

	bool m_bActive;         //사용 가능한 용병.
	byte m_chExerciseStyle; //체험 스타일 

	int m_iAwakeType;		//각성 종류
	int m_iAwakeLimitTime;	//각성 만료 시간
	
	byte m_eCharPowerUpType;
	
	CostumeEquipInfo m_EquipCostume[MAX_INVENTORY];
	AccessoryEquipInfo m_EquipAccessory[MAX_ACC_INVENTORY];

	CHARACTER()
	{
		Init();
	}

	void Init()
	{
		m_class_type	    = 0;
		m_kindred			= 1;
		m_sex				= 1;
		m_beard				= 1;
		m_face				= 1;
		m_hair				= 1;
		m_skin_color		= 1;
		m_hair_color		= 1;
		m_accessories		= 1;
		m_underwear			= 1;

		for(int i = 0; i < MAX_INVENTORY ; i++ )
			m_extra_item[i]	= 0;

		m_iSlotIndex		= -1;
		m_iLimitSecond		= 0;

		m_sLeaderType		= CLT_GENERAL;
		m_sRentalType		= CRT_GENERAL;
		m_dwRentalMinute	= 0;
		m_ePeriodType		= CPT_TIME;

		m_bActive			= true;
		m_chExerciseStyle	= 0x00;

		m_iAwakeType		= AWAKE_NONE;
		m_iAwakeLimitTime	= 0;

		m_eCharPowerUpType = PUGT_NONE;

		for(int i = 0; i < MAX_INVENTORY ; i++ )
			m_EquipCostume[i].Init();
		for(int i = 0; i < MAX_ACC_INVENTORY ; i++ )
			m_EquipAccessory[i].Init();
	}

	CHARACTER& operator = ( const CHARACTER &rhs )
	{
		m_class_type		= rhs.m_class_type;
		m_kindred			= rhs.m_kindred;
		m_sex				= rhs.m_sex;
		m_beard				= rhs.m_beard;
		m_face				= rhs.m_face;
		m_hair				= rhs.m_hair;
		m_skin_color		= rhs.m_skin_color;
		m_hair_color		= rhs.m_hair_color;
		m_accessories		= rhs.m_accessories;
		m_underwear			= rhs.m_underwear;

		for( int i = 0; i < MAX_INVENTORY ; i++ )
			m_extra_item[i]	= rhs.m_extra_item[i];

		m_iSlotIndex		= rhs.m_iSlotIndex;
		m_iLimitSecond		= rhs.m_iLimitSecond;

		m_sLeaderType		= rhs.m_sLeaderType;
		m_sRentalType		= rhs.m_sRentalType;
		m_dwRentalMinute	= rhs.m_dwRentalMinute;
		m_ePeriodType		= rhs.m_ePeriodType;

		m_bActive			= rhs.m_bActive;
		m_chExerciseStyle	= rhs.m_chExerciseStyle;

		m_iAwakeType		= rhs.m_iAwakeType;
		m_iAwakeLimitTime	= rhs.m_iAwakeLimitTime;

		m_eCharPowerUpType = rhs.m_eCharPowerUpType;

		for(int i = 0; i < MAX_INVENTORY ; i++ )
			m_EquipCostume[i] = rhs.m_EquipCostume[i];
		for(int i = 0; i < MAX_ACC_INVENTORY ; i++ )
			m_EquipAccessory[i] = rhs.m_EquipAccessory[i];

		return *this;
	}

	bool operator==( const CHARACTER &rhs ) const
	{
		if( m_class_type != rhs.m_class_type )
			return false;
		if( m_kindred != rhs.m_kindred )
			return false;
		if( m_sex != rhs.m_sex )
			return false;
		if( m_beard	!= rhs.m_beard )
			return false;
		if( m_face != rhs.m_face )
			return false;
		if( m_hair != rhs.m_hair )
			return false;
		if( m_skin_color != rhs.m_skin_color )
			return false;
		if( m_hair_color != rhs.m_hair_color )
			return false;
		if( m_accessories != rhs.m_accessories )
			return false;
		if( m_underwear != rhs.m_underwear )
			return false;

		for (int i = 0; i < MAX_INVENTORY ; i++)
		{
			if( m_extra_item[i] != rhs.m_extra_item[i] )
				return false;
		}

		if( m_iSlotIndex != rhs.m_iSlotIndex )
			return false;
		if( m_iLimitSecond != rhs.m_iLimitSecond )
			return false;

		if( m_sLeaderType != rhs.m_sLeaderType )
			return false;
		if( m_sRentalType != rhs.m_sRentalType )
			return false;
		if( m_dwRentalMinute != rhs.m_dwRentalMinute )
			return false;
		if( m_ePeriodType != rhs.m_ePeriodType )
			return false;

		if( m_bActive != rhs.m_bActive )
			return false;
		if( m_chExerciseStyle != rhs.m_chExerciseStyle )
			return false;
		if( m_iAwakeType != rhs.m_iAwakeType )
			return false;
		if( m_iAwakeLimitTime != rhs.m_iAwakeLimitTime )
			return false;
		
		if( m_eCharPowerUpType != rhs.m_eCharPowerUpType )
			return false;

		for (int i=0; i<MAX_INVENTORY; i++)
		{
			if( m_EquipCostume[i] != rhs.m_EquipCostume[i] )
				return false;
		}

		for (int i=0; i<MAX_ACC_INVENTORY; i++)
		{
			if( m_EquipAccessory[i] != rhs.m_EquipAccessory[i] )
				return false;
		}

		return true;
	}

	bool operator!=( const CHARACTER &rhs ) const
	{
		if( *this == rhs )
			return false;

		return true;
	}

	void FillData( SP2Packet &rkPacket )
	{
		rkPacket << m_class_type;
		rkPacket << m_kindred;
		rkPacket << m_sex;			
		rkPacket << m_beard;		
		rkPacket << m_face;			
		rkPacket << m_hair;			
		rkPacket << m_skin_color;	
		rkPacket << m_hair_color;	
		rkPacket << m_accessories;	
		rkPacket << m_underwear;    

		for (int i = 0; i < MAX_INVENTORY ; i++)
			rkPacket << m_extra_item[i]; 

		rkPacket << m_iSlotIndex;    
		rkPacket << m_iLimitSecond;  

		rkPacket << m_sLeaderType;
		rkPacket << m_sRentalType;
		rkPacket << m_dwRentalMinute;
		rkPacket << (int)m_ePeriodType;   

		rkPacket << m_bActive;
		rkPacket << m_chExerciseStyle;

		rkPacket << static_cast<byte>( m_iAwakeType );
		rkPacket << m_iAwakeLimitTime;

		rkPacket << m_eCharPowerUpType;
		
		for( int i=0; i<MAX_INVENTORY; i++ )
		{
			rkPacket << m_EquipCostume[i].m_CostumeIdx;
			rkPacket << m_EquipCostume[i].m_CostumeCode;
		}

		for( int i=0; i<MAX_ACC_INVENTORY; i++ )
		{
			rkPacket << m_EquipAccessory[i].m_iAccessoryIdx;
			rkPacket << m_EquipAccessory[i].m_iAccessoryCode;
			rkPacket << m_EquipAccessory[i].m_iRandomNumber;
			rkPacket << m_EquipAccessory[i].m_iCompoundCode;
			rkPacket << m_EquipAccessory[i].m_iCompoundValue;
		}
	}

	void ApplyData( SP2Packet &rkPacket )
	{
		rkPacket >> m_class_type;
		rkPacket >> m_kindred;
		rkPacket >> m_sex;			
		rkPacket >> m_beard;		
		rkPacket >> m_face;			
		rkPacket >> m_hair;			
		rkPacket >> m_skin_color;	
		rkPacket >> m_hair_color;	
		rkPacket >> m_accessories;	
		rkPacket >> m_underwear;    

		for (int i = 0; i < MAX_INVENTORY ; i++)
			rkPacket >> m_extra_item[i]; 

		rkPacket >> m_iSlotIndex;    
		rkPacket >> m_iLimitSecond;  

		rkPacket >> m_sLeaderType;
		rkPacket >> m_sRentalType;
		rkPacket >> m_dwRentalMinute;
		int iPeriodType = 0;
		rkPacket >> iPeriodType; 
		m_ePeriodType = (CharPeriodType) iPeriodType;   

		rkPacket >> m_bActive;       
		rkPacket >> m_chExerciseStyle;

		byte eAwakeType;
		rkPacket >> eAwakeType;
		m_iAwakeType = eAwakeType;
		rkPacket >> m_iAwakeLimitTime;

		rkPacket >> m_eCharPowerUpType;
		
		for( int i=0; i<MAX_INVENTORY; i++ )
		{
			rkPacket >> m_EquipCostume[i].m_CostumeIdx;
			rkPacket >> m_EquipCostume[i].m_CostumeCode;
			//rkPacket >> m_EquipCostume[i].m_Costume_Male_Custom;
			//rkPacket >> m_EquipCostume[i].m_Costume_Female_Custom;
		}

		for( int i=0; i<MAX_ACC_INVENTORY; i++ )
		{
			rkPacket >> m_EquipAccessory[i].m_iAccessoryIdx;
			rkPacket >> m_EquipAccessory[i].m_iAccessoryCode;
			rkPacket >> m_EquipAccessory[i].m_iRandomNumber;
			rkPacket >> m_EquipAccessory[i].m_iCompoundCode;
			rkPacket >> m_EquipAccessory[i].m_iCompoundValue;
		}
	}
};

struct CHARACTERDATA         //2007.05.29 LJH 
{
	DWORD           m_dwIndex;
	CHARACTER       m_data;
	ITEM_DATA       m_equip_item[MAX_INVENTORY];       //장착 아이템.

	CHARACTERDATA()
	{
		m_dwIndex = 0;

		for (int i = 0; i < MAX_INVENTORY ; i++)
			m_equip_item[i].Initialize();
	}
};

struct ITEM_GAUGE_DATA
{
	float m_fMaxGauge;
	float m_fCurGauge;
	float m_fRecoveryGauge;
	DWORD m_dwRecoveryTic;
	DWORD m_dwCheckTime;

	bool m_bRecoverEnable;

	ITEM_GAUGE_DATA()
	{
		m_fMaxGauge = 0.0f;
		m_fCurGauge = 0.0f;
		m_fRecoveryGauge = 0.0f;
		m_dwRecoveryTic = 0;
		m_dwCheckTime = 0;

		m_bRecoverEnable = true;
	}
};

struct PlayCharInfo
{
	float m_fRevivalPenalty;
	bool m_bCharDied;
	bool m_bCharDiedPenalty;
	bool m_bCharJoined;

	ITEM_GAUGE_DATA m_EquipItem[MAX_INVENTORY];

	PlayCharInfo()
	{
		m_fRevivalPenalty = 0.0f;
		m_bCharDied = false;
		m_bCharDiedPenalty = false;
		m_bCharJoined = false;
	}
};

typedef std::vector< PlayCharInfo* > PlayCharInfoList;

struct CustomTexture
{
	DWORD m_dwCustomSeq;
	ioHashString m_szTextureName;
	CustomTexture()
	{
		m_dwCustomSeq = 0;
	}
};
typedef std::vector< CustomTexture > CustomTextureList;
