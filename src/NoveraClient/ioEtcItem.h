#pragma once

class SP2Packet;
class ioINILoader;
class ioUserEtcItem;
class ioPlayStage;
class ioWnd;

typedef struct tagBlockItemInfo
{
	ioHashString m_szGuidEffect;
	int m_iBlockNum;
} BlockItemInfo;

typedef std::vector<BlockItemInfo> vBlockItemInfoList;

typedef struct tagPrizeData
{
	int	m_iPresentType;
	int	m_iPresentMent;
	int	m_iPresentPeriod;

	int m_iPresentValue1;
	int m_iPresentValue2;
	int m_iPresentValue3;
	int m_iPresentValue4;

	tagPrizeData()
	{
		Init();
	}

	void Init()
	{
		m_iPresentType = 0;		// 선물 타입
		m_iPresentMent = 0;		// 선물 받았을때 맨트
		m_iPresentPeriod = 0;	// 선물 기간

		m_iPresentValue1 = 0;
		m_iPresentValue2 = 0;
		m_iPresentValue3 = 0;
		m_iPresentValue4 = 0;
	}

} PrizeData;

typedef std::vector< PrizeData > PrizeDataVec;

class ioEtcItem
{
public:
	enum 
	{
		// AABBBBBB // AA: 1:횟수용, 2:시간용, 3:즉시사용, 4:영구사용, 5:날자용 // BBBBBB: 종류 1~99999 : 권한 , 100000 ~ 199999 : 클래스별 권한 아이템
		// BBBBBB ( 십만 자리는 겹치면 안됨 : 1000001(기존) , 1000002(추가정상) , 2000001(추가불가) )
		EIT_NONE							= 0,

		// 권한 아이템 
		EIT_ETC_WHOLE_CHAT					= 1000001,  
		EIT_ETC_FRIEND_SLOT_EXTEND			= 4000002,
		EIT_ETC_PESO_EXP_BONUS				= 2000003,
		EIT_ETC_CHANGE_ID					= 3000004,
		EIT_ETC_CUSTOM_SOUND				= 1000005,
		EIT_ETC_BUY_MORTMAIN_CHAR			= 1000006,
		EIT_ETC_GUILD_CREATE				= 3000007,
		EIT_ETC_GUILD_MARK_CHANGE			= 3000008,
		EIT_ETC_GROWTH_DOWN					= 1000009,
		EIT_ETC_CHAR_SLOT_EXTEND			= 4000010,
		EIT_ETC_PESO_BONUS					= 5000011,
		EIT_ETC_EXP_BONUS					= 5000012,
		EIT_ETC_FISHING_BAIT				= 1000013,
		EIT_ETC_FISHING_ROD					= 5000014,
		EIT_ETC_FISHING_SLOT_EXTEND			= 4000015,
		EIT_ETC_FISHING_MOON_BAIT			= 1000016,
		EIT_ETC_FISHING_MOON_ROD			= 5000017,
		EIT_ETC_GUILD_NAME_CHANGE			= 3000018,
		EIT_ETC_SOLDIER_PACKAGE				= 3000019,
		EIT_ETC_DECORATION_PACKAGE			= 3000020,
		EIT_ETC_GOLDMONSTER_COIN			= 1000021,
		EIT_ETC_GASHAPON					= 1000022,
		EIT_ETC_ITEM_COMPOUND				= 1000023,
		EIT_ETC_FISHING_PACKAGE				= 3000024,
		EIT_ETC_PESO_EXP_PACKAGE			= 3000025,
		EIT_ETC_MONSTER_COIN				= 1000026,
		EIT_ETC_DECO_UNDERWEAR_PACKAGE		= 3000027,
		EIT_ETC_RANDOM_DECO_M				= 1000028,						// 치장 랜덤(남)
		EIT_ETC_RANDOM_DECO_W				= 1000029,						// 치장 랜덤(여)
		EIT_ETC_MULTIPLE_COMPOUND			= 1000030,
		EIT_ETC_MULTIPLE_EQUAL_COMPOUND		= 1000031,
		EIT_ETC_EXCAVATING_KIT				= 2000032,
		EIT_ETC_NEW_EXCAVATING_KIT			= 4000001,
		EIT_ETC_NEW_EXCAVATING_SHOVEL		= 1002831,
		EIT_ETC_GASHAPON2					= 1000034,
		EIT_ETC_GASHAPON13					= 1000045,
		EIT_ETC_TRADE_STATE_CHANGE			= 1000046,
		EIT_ETC_SPECIAL_PACKAGE1			= 3000047,
		EIT_ETC_QUEST_EVENT					= 1000048,
		EIT_ETC_SILVER_COIN					= 1000049,  
		EIT_ETC_BATTLE_RECORD_INIT			= 3000050,
		EIT_ETC_LADDER_RECORD_INIT			= 3000051,
		EIT_ETC_HERO_RECORD_INIT			= 3000052,
		EIT_ETC_GASHAPON14					= 1000053,
		EIT_ETC_GASHAPON53					= 1000092,
		EIT_ETC_SKELETON_BIG				= 2000093,
		EIT_ETC_SKELETON_BIGHEAD			= 2000094,
		EIT_ETC_SKELETON_SMALL				= 2000095,
		EIT_ETC_ITEM_COMPOUND2				= 1000096,	//최고급 장비 강화 도구
		EIT_ETC_ITEM_COMPOUND3				= 1000097,	//장비강화 도구(예비)
		EIT_ETC_MULTIPLE_COMPOUND2			= 1000098,
		EIT_ETC_MULTIPLE_COMPOUND3			= 1000099,
		EIT_ETC_MULTIPLE_EQUAL_COMPOUND2	= 1000100,
		EIT_ETC_MULTIPLE_EQUAL_COMPOUND3	= 1000101, //상급 부위별 영구장비 합성기
		EIT_ETC_MOTION1						= 4000102,
		EIT_ETC_MOTION100					= 4000201,
		EIT_ETC_CUSTOM_ITEM_SKIN			= 1000202,
		EIT_ETC_PACKAGE1					= 3000203,
		EIT_ETC_PACKAGE100					= 3000302,
		EIT_ETC_CUSTOM_ITEM_SKIN_TEST		= 4000303,
		EIT_ETC_BLOCK1						= 5000304,
		EIT_ETC_BLOCK10						= 5000313,
		EIT_ETC_PREMIUM_SOLDIER_PACKAGE		= 3000314,
		EIT_ETC_EVENT_CHECK1				= 1000315,
		EIT_ETC_EVENT_CHECK100				= 1000414,
		EIT_ETC_ITEM_GROWTH_CATALYST		= 1000415,
		EIT_ETC_ITEM_LUCKY_COIN_1			= 1000416,
		EIT_ETC_ITEM_LUCKY_COIN_2			= 1000417,
		EIT_ETC_ITEM_LUCKY_COIN_3			= 1000418,
		EIT_ETC_ITEM_LUCKY_COIN_4			= 1000419,
		EIT_ETC_ITEM_COMPOUNDEX_1			= 1000420,
		EIT_ETC_ITEM_COMPOUNDEX_2			= 1000421,
		EIT_ETC_ITEM_COMPOUNDEX_3			= 1000422,
		EIT_ETC_MULTIPLE_COMPOUND4			= 1000423, //유니크 장비합성기
		EIT_ETC_MULTIPLE_COMPOUND5			= 1000424, 
		EIT_ETC_ITEM_RAINBOW_MIXER			= 4000423,
		EIT_ETC_ITEM_LOSTSAGA_MIXER			= 4000424,

		EIT_ETC_ITEM_TIME_GASHAPON1			= 5000425,						// 5000425 ~ 5000524 총 100개
		EIT_ETC_ITEM_TIME_GASHAPON100		= 5000524,

		EIT_ETC_GASHAPON54					= 1000525,						// 1000525 ~ 1000724 총 200개
		EIT_ETC_GASHAPON253					= 1000724,

		EIT_ETC_PACKAGE101					= 3000725,						// 3000725 ~ 3000924 총 200개
		EIT_ETC_PACKAGE300					= 3000924,
		
		EIT_ETC_GOLD_BOX					= 1000925,

		EIT_ETC_ITEM_COMPOUNDEX_4			= 1000926,						// 1000926 ~ 1000932 총 7개
		EIT_ETC_ITEM_COMPOUNDEX_10			= 1000932,

		EIT_ETC_SOLDIER_PACKAGE2			= 3000933,

		EIT_ETC_SOLDIER_SELECTOR1			= 3000934,
		EIT_ETC_SOLDIER_SELECTOR101			= 3001034,

		EIT_ETC_FOUR_EXTRA_COMPOUND1		= 1001035,
		EIT_ETC_FOUR_EXTRA_COMPOUND51		= 1001085,

		EIT_ETC_MOTION101					= 4001086,						// 모션 아이템 300개 더 추가 1186 ~ 1485
		EIT_ETC_MOTION400					= 4001385,

		EIT_ETC_EXPAND_MEDAL_SLOT01			= 1001386,						// 메달슬롯 오픈 아이템. 기간제 5개, 영구 5개
		EIT_ETC_EXPAND_MEDAL_SLOT20			= 1001405,

		EIT_ETC_SOLDIER_EXP_BONUS			 = 5001406,

		EIT_ETC_CONSUMPTION_BUFF01			= 1001407,						// 64 개 예약		
		EIT_ETC_CONSUMPTION_BUFF33			= 1001439,						// 38번부터 스킬형임
		EIT_ETC_CONSUMPTION_SKILL01			= EIT_ETC_CONSUMPTION_BUFF33,
		EIT_ETC_CONSUMPTION_BUFF64			= 1001470,		
		EIT_ETC_CONSUMPTION_SKILL32			= EIT_ETC_CONSUMPTION_BUFF64, 	
		
		EIT_ETC_CONSUMPTION_REVIVE			= 1001471,						// 즉시 부활	
		EIT_ETC_CONSUMPTION_COIN			= 1001472,						// 던전 코인 JSM 150518, 던전 코인 아님, 사용하지 않음

        EIT_ETC_SELECT_EXTRA_GASHAPON		= 3001473,

		EIT_ETC_PRESET_PACKAGE1				= 3001601,						// preset package
		EIT_ETC_PRESET_PACKAGE100			= 3001700,

		EIT_ETC_SOLDIER_PACKAGE3			= 3001701,						// 영구 용병 패키지
		EIT_ETC_SOLDIER_PACKAGE10			= 3001708,      

		EIT_ETC_GROWTH_ALL_DOWN				= 1001709,

		EIT_ETC_PRIZE_ITEM1					= 1001711,						// 대회 모드 상품 200개
		EIT_ETC_PRIZE_ITEM200				= 1001910,

		EIT_ETC_TOURNAMENT_CREATE			= 3001911,						// 대회 생성권
		EIT_ETC_TOURNAMENT_PREMIUM_CREATE	= 3001912,						// 프리미엄 대회 생성권 - (배너 변경권)

		EIT_ETC_CLOVER						= 1001913,

		EIT_ETC_GOLD_BOX01					= 1002001,
		EIT_ETC_GOLD_BOX32					= 1002032,

		EIT_ETC_ADD_CASH001					= 5002033,         
		EIT_ETC_ADD_CASH100					= 5002132,    

		EIT_ETC_TOURNAMENT_COIN				= 1002133,

		EIT_ETC_ROULETTE_COIN				= 1002134,

		EIT_ETC_BINGO_ITEM					= 1002135,
		EIT_ETC_BINGO_NUMBER_GASHAPON		= 1002136,
		EIT_ETC_BINGO_SHUFFLE_NUMBER		= 1002137,
		EIT_ETC_BINGO_SHUFFLE_REWARD_ITEM	= 1002138,
		EIT_ETC_BINGO_RANDOM_NUMBER_CLEAR	= 1002139,

		EIT_ETC_ITEM_COMPOUNDEX_11			= 1002140,						// 강화카드 1002140 ~ 1002229 90개 추가
		EIT_ETC_ITEM_COMPOUNDEX_100			= 1002229, 

		EIT_ETC_SUPER_GASHAPON1				= 1002230,
		EIT_ETC_SUPER_GASHAPON200			= 1002429,

		EIT_ETC_ITEM_LUCKY_COIN_5			= 1002430,
		EIT_ETC_ITEM_LUCKY_COIN_205			= 1002529,

		EIT_ETC_ITEM_SEND_PRESENT_1			= 1002530,
		EIT_ETC_ITEM_SEND_PRESENT_100		= 1002629,

		EIT_ETC_ITEM_RECHARGE_1				= 1002630,
		EIT_ETC_ITEM_RECHARGE_100			= 1002729,

		EIT_ETC_ACCESSORY_RECHARGE_1		= 1002830,     // 악세 충전기.
		EIT_ETC_ACCESSORY_RECHARGE_100		= 1002929,

		EIT_ETC_PACKAGE301					= 3002730,
		EIT_ETC_PACKAGE800					= 3003329,

		EIT_ETC_SOLDIER_EXP_ADD_001			= 3003330,
		EIT_ETC_SOLDIER_EXP_ADD_200			= 3003529,

		EIT_ETC_GASHAPON254					= 1003530,						// 1003530 ~ 1003829 총 300개
		EIT_ETC_GASHAPON553					= 1003829,

		EIT_ETC_ITEM_TIME_GASHAPON101		= 5003830,						// 5003830 ~ 5004129 총 200개
		EIT_ETC_ITEM_TIME_GASHAPON300		= 5004029,

		EIT_ETC_EVENT_CHECK101				= 1004030,						// 1004030 ~ 1004129 총 100개 추가
		EIT_ETC_EVENT_CHECK200				= 1004129,

		EIT_ETC_SELECT_EXTRA_GASHAPON02		= 3004130,						// 3004130 ~ 3004178 총 50개 추가
		EIT_ETC_SELECT_EXTRA_GASHAPON51		= 3004179,

		EIT_ETC_SELECT_GASHAPON001			= 3004180,						// 3004180 ~ 3004479 총 300개 추가
		EIT_ETC_SELECT_GASHAPON300			= 3004479,

		EIT_ETC_FIXED_BINGO_NUMBER001		= 1004480,						// 1004480 ~ 1004879 총 400개 추가(빙고 더미용)
		//EIT_ETC_FIXED_BINGO_NUMBER400		= 1004879,
		EIT_ETC_FIXED_BINGO_NUMBER400		= 1004859,

		EIT_ETC_MATERIAL_COMPOUND_001		= 1004880,						// 1004880 ~ 1004929 총 50개 추가(아이템강화)
		EIT_ETC_MATERIAL_COMPOUND_050		= 1004929,

		EIT_ETC_PET_EGG_001					= 1004930,						// 1004930 ~ 1005029 총 100개 추가(펫알)
		EIT_ETC_PET_EGG_100					= 1005029,

		EIT_ETC_RAINBOW_WHOLE_CHAT			= 1005030,

		EIT_ETC_SOUL_STONE					= 1005031,

		EIT_ETC_CUSTOM_COSTUME_SKIN			= 1005032,

		EIT_ETC_ITEM_LUCKY_COIN_206			= 1005033,						// 1005033 ~ 1005332 총 300개 추가
		EIT_ETC_ITEM_LUCKY_COIN_506			= 1005332,

		EIT_ETC_SUPER_GASHAPON201			= 1005333,						// 1005333 ~ 1005632 총 300개 추가
		EIT_ETC_SUPER_GASHAPON500			= 1005632,

		EIT_ETC_MILEAGE_COIN				= 1005633,
		EIT_ETC_EXTRAITEM_SLOT_EXTEND		= 4005634,
		
		EIT_ETC_SUPER_GASHAPON501			= 1005635,						// 1005635 ~ 1005934 총 300개 추가
		EIT_ETC_SUPER_GASHAPON800			= 1005934,

		EIT_ETC_GASHAPON554					= 1005935,						// 1005935 ~ 1006234 총 300개 추가
		EIT_ETC_GASHAPON853					= 1006234,

		EIT_ETC_ITEM_TIME_GASHAPON301		= 5006235,						// 5006235 ~ 5006534 총 300개 추가
		EIT_ETC_ITEM_TIME_GASHAPON600		= 5006534,
		
		EIT_ETC_NEW_MULTIPLE_COMPOUND001	= 1006535,
		EIT_ETC_NEW_MULTIPLE_COMPOUND010	= 1006544,

		EIT_ETC_GUILD_HOUSING_BLOCK_0001	= 1006545,						// 1006545 ~ 1007544 길드 본부 하우징 시스템 아이템 1000개
		EIT_ETC_GUILD_HOUSING_BLOCK_1000	= 1007544,
		EIT_ETC_CREATE_GUILD_HQ				= 3007545,

		EIT_ETC_HOME_HOUSING_BLOCK_0001		= 1007546,						// 1006545 ~ 1007544 개인 본부 하우징 시스템 아이템 1000개
		EIT_ETC_HOME_HOUSING_BLOCK_1000		= 1008545,
		EIT_ETC_CREATE_HOME					= 5008546,

		EIT_ETC_SUPER_GASHAPON801			= 1008547,						// 1008547 ~ 1008746 총 200개 추가
		EIT_ETC_SUPER_GASHAPON1000			= 1008746,
		
		EIT_ETC_NAMEDTITLE_0001				= 1008747,						// 1008747 ~ 1009746 칭호 선물 아이템 1000개 추가
		EIT_ETC_NAMEDTITLE_1000				= 1009746,						
		EIT_ETC_NAMEDTITLE_PREMIUM			= 1009747,
		EIT_ETC_TIME_CASH					= 5009748,
		EIT_ETC_TIME_CASH2					= 5009749,

		EIT_ETC_OAK_DRUM_ITEM				= 1009749,						// 공포의 오크통
		EIT_ETC_OAK_WOOD_SWORD				= 1009750,						// 공포의 오크통 꼽기 나무 칼
		EIT_ETC_OAK_SILVER_SWORD			= 1009751,						// 공포의 오크통 꼽기 실버 칼
		EIT_ETC_OAK_GOLD_SWORD				= 1009752,						// 공포의 오크통 꼽기 골드 칼

		EIT_ETC_PACKAGE801					= 3009753,
		EIT_ETC_PACKAGE1100					= 3010752,

		EIT_ETC_PCROOM_FISHING_ROD			= 4010753,		//pc방 전용 낚싯대
		EIT_ETC_PCROOM_FISHING_BAIT			= 4010754,

		EIT_ETC_NEW_MULTIPLE_COMPOUND011	= 1010755,		//메달 합성 확장
		EIT_ETC_NEW_MULTIPLE_COMPOUND060	= 1010804,
		EIT_ETC_RAID_COIN					= 1010805,
		EIT_ETC_ACCESSORY_COMPOUND			= 1010806,

		EIT_ETC_SUPER_GASHAPON1001			= 1011001,
		EIT_ETC_SUPER_GASHAPON2000			= 1012000,

		EIT_ETC_CARD_MATCH_ITEM				= 1004870,
		EIT_ETC_CARD_MATCH_TICKET			= 1004871,
		EIT_ETC_CARD_MATCH_PREMIUM_TICKET	= 1004872,

		EIT_ETC_DICE_GAME_ITEM				= 1004860,
		EIT_ETC_DICE_GAME_DICE				= 1004861,
		EIT_ETC_DICE_GAME_REWARD_CHANGE		= 1004862,
		EIT_ETC_DICE_GAME_BOARD_CHANGE		= 1004863,

		EIT_ETC_PET_FEED_01 = 1012001,   // 펫사료 1~10
		EIT_ETC_PET_FEED_10 = 1012010,
		EIT_ETC_ADDICTIVE_PIECE = 1012011,   // 만능조각

		EIT_ETC_SOLDIER_EXP_ADD_201 = 3012012,
		EIT_ETC_SOLDIER_EXP_ADD_2000 = 3013811,

		EIT_ETC_FISHING_NEW_BAIT_01 = 3004300,

		EIT_ETC_PRIVATE_FISHING_BAIT_ITEM_1		= 1013812,                
		EIT_ETC_PRIVATE_FISHING_BAIT_ITEM_100	= 1013911,                
		EIT_ETC_PRIVATE_FISHING_ROD_ITEM_1		= 5013912,                
		EIT_ETC_PRIVATE_FISHING_ROD_ITEM_100	= 5014011,                
		EIT_ETC_PRIVATE_FISHING_PLACE_ITEM_1	= 5014012,                
		EIT_ETC_PRIVATE_FISHING_PLACE_ITEM_100	= 5014111,        

		EIT_ETC_NEW_GASHAPON5000                = 1015000,
		EIT_ETC_NEW_GASHAPON7000                = 1017000,
	};

	enum
	{
		MAX_COUNT          = 99999,
		MAX_TIME           = 31536000, // SEC, 365일
		MAX_DAY            = 999,      // 999일 
		USE_TYPE_CUT_VALUE = 1000000,
		MAX_CLASS_ITEM_ADD = 99900,    // 999개의 클래스타입이 존재
	};

	enum UseType
	{
		UT_NONE      = 0,
		UT_COUNT     = 1,
		UT_TIME      = 2,
		UT_ONCE      = 3,
		UT_ETERNITY  = 4,
		UT_DATE      = 5,
	};

	enum BuyPopupType
	{
		BPT_SINGLE = 0,
		BPT_LINK   = 1,
		BPT_NONE   = 2,
	};

	enum 
	{
		GROUP_NONE    = 0,
		GROUP_FUNCTION= 1,
		GROUP_PREMIUM = 2,
		GROUP_PLAZA   = 3,
	};
private:
	friend class ioEtcItemManager;

protected:
	ioHashString	m_Name;
	ioHashString	m_ExtraName;
	DWORD			m_dwType;					// AABBBBB -> AA:Use Type , BBBBB:item type
	UseType			m_eUseType;
	int				m_iMaxUse;
	int				m_iSellPeso;
	int				m_iMortmainSellPeso;

	int				m_iExtraType;
	int				m_iBuyPopupType;
	int				m_iGroup;					// GROUP_NONE ....
	int				m_iSecretShopGroup;
	int				m_iInventoryManual;
	int				m_iInventorySubManual;
	int				m_iInventoryOrder;
	int				m_iLimitChannelingType;

	bool			m_bDecorationMaxCheck;
	bool			m_bExtraItemMaxCheck;
	bool			m_bMedalItemMaxCheck;
	bool			m_bCanExtendUse;
	bool			m_bCanPresentUse;
	bool			m_bCanSell;
	bool			m_bBlockBuying;
	bool			m_bCanOnOff;

	IntVec			m_vValueList;
	IntVec			m_vPesoList;
	IntVec			m_vCashList;
	IntVec			m_vBonusPesoList;
	IntVec			m_vMainManualList;
	IntVec			m_vSubManualList;
	IntVec			m_vShopOrderList;				// 상점에서 정렬 순서
	IntVec			m_vRecommandedShopOrderList;	// 상점의 추천상품에서 정렬 순서
	IntVec			m_vShowPesoList;					// 1면 상점에서 페소가격 표시 , 0이면 상점에서 캐쉬가격 표시 	
	IntVec			m_vActiveList;
	IntVec			m_vShopMarkTypeList;			// SHOP_MARK_TYPE_NONE = 0, SHOP_MARK_TYPE_SALE = 1, SHOP_MARK_TYPE_NEW = 2, SHOP_MARK_TYPE_HIT = 3, SHOP_MARK_TYPE_EVENT = 4,
	IntVec			m_vShopLimitDateList;
	IntVec			m_vShopTabTypeList;			// SHOP_TAB_TYPE_NONE = 0...
	IntVec			m_vCanMortmainList;
	IntVec			m_vSubscriptionList;

	ioHashStringVec	m_vShopNameList;
	ioHashString	m_IconName;
	ioHashString	m_SubIconName;	
	 
	ioHashString m_szAlarmMent1;
	ioHashString m_szAlarmMent2;
	ioHashString m_szAlarmMent3;

	bool m_bOnlyBuyBtn;
	bool m_bOnlyGiftBtn;

	int m_nGradeType;

public:
	virtual void   LoadProperty( ioINILoader &rkLoader );

public:
	virtual void   SetUse(){}
	virtual void   Execute() const{}
	virtual void   SendBuy(){}
public:
	inline DWORD   GetType() const { return m_dwType; }
	inline UseType GetUseType() const { return m_eUseType; }

	inline int     GetMaxUse() const { return m_iMaxUse; }
	inline int     GetSellPeso() const { return m_iSellPeso; }
	inline int     GetMortmainSellPeso() const { return m_iMortmainSellPeso; }
	inline int	   GetExtraType() const { return m_iExtraType; }
	inline int     GetBuyPopupType() const { return m_iBuyPopupType; }
	inline int     GetGroup() const { return m_iGroup; }
	inline int     GetSecretGroup() const { return m_iSecretShopGroup; }
	inline bool	   IsCanExtendUse() const { return m_bCanExtendUse; }
	inline bool    IsCanPresentUse() const { return m_bCanPresentUse; }
	inline bool    IsCanSell() const { return m_bCanSell; }
	inline bool    IsBlockBuying() const { return m_bBlockBuying; }
	inline bool    IsCanOnOff() const { return m_bCanOnOff; }
	inline int     GetInventoryManual() const { return m_iInventoryManual; }
	inline int     GetInventorySubManual() const { return m_iInventorySubManual; }
	inline int     GetInventoryOrder() const { return m_iInventoryOrder; }
	inline int     GetLimitChannelingType() const { return m_iLimitChannelingType; }
	inline bool    IsOnlyBuyBtn() const { return m_bOnlyBuyBtn; }
	inline bool    IsOnlyGiftBtn() const { return m_bOnlyGiftBtn; }
	inline int GetGradeType() const { return m_nGradeType; }

	int		       GetValue( int iArray ) const;
	int            GetPeso( int iArray ) const;
	int            GetCash( int iArray ) const;
	int            GetBonusPeso( int iArray ) const;
	int			   GetMainManual( int iArray ) const;
	int            GetSubManual( int iArray ) const;
	int            GetShopOrder( int iArray ) const;
	int			   GetRecommandedShopOrder( int iArray ) const;
	int			   GetRecommandedShopOrderSize() const;
	bool           IsShowPeso( int iArray ) const;
	ioHashString   GetShopName( int iArray );
	bool           IsActive( int iArray ) const;
	int            GetShopMarkType( int iArray ) const;
	int            GetLimitDate( int iArray ) const;
	int            GetShopTabType( int iArray ) const;
	int			   GetShopTabTypeSize();

	int            GetSubscriptionType( int iArray ) const;

	bool		   IsDecorationMaxCheck() const { return m_bDecorationMaxCheck; }
	bool		   IsExtraItemMaxCheck() const { return m_bExtraItemMaxCheck; }
	bool		   IsMedalItemMaxCheck() const { return m_bMedalItemMaxCheck; }

	inline const ioHashString& GetName() const { return m_Name; }
	inline const ioHashString& GetExtraName() const { return m_ExtraName; }
	inline const ioHashString& GetIconName() const { return m_IconName; }
	inline const ioHashString& GetSubIconName() const { return m_SubIconName; }

	inline int     GetValueSize() { return m_vValueList.size(); }

	bool           IsValid() const;

	void           SetCash( int iArray , int iCash );
	void           SetPeso( int iArray , int iPeso );
	void           SetActive( int iArray , bool bActive );
	void           SetShopOrder( int iArray, int iShopOrder );
	void		   SetRecommandedShopOrder( int iArray, int iRecommandedShopOrder );
	void           SetShopMarkType( int iArray, int iShopMarkType );
	void           SetShopLimitDate( int iArray, int iLimitDate );

	void SetCanMortmain( int iArray, bool bCanMortMain );
	bool IsCanMortMain( int iArray ) const;

	const ioHashString &GetAlarmMent1(){ return m_szAlarmMent1; }
	const ioHashString &GetAlarmMent2(){ return m_szAlarmMent2; }
	const ioHashString &GetAlarmMent3(){ return m_szAlarmMent3; }

public:
	virtual void OnInit( ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage ){}
	virtual void OnUse( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual void OnSell( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem );
	virtual void OnBuy(){}
	virtual bool OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck = true ){ return false; }
	virtual bool OnSellBtnPush( int iType, ioUserEtcItem *pUserEtcItem, ioWnd *pReturnWnd );
	virtual void OnAction( SP2Packet &rkPacket, ioPlayStage *pPlayStage ){}
	virtual bool OnUseSwitchBtnPush( ioUserEtcItem *pUserEtcItem, bool bUseOn, ioPlayStage *pPlayStage ){ return false; }
	virtual void OnUseSwitch( SP2Packet &rkPacket, ioPlayStage *pPlayStage ){}
	virtual bool IsBuyCondition( int iUse ){ return true; }
	virtual bool IsShop3DChar(){ return false; }

public:
	ioEtcItem();
	virtual ~ioEtcItem();
};

typedef std::vector< const ioEtcItem* > EtcItemList;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemWholeChat : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_WHOLE_CHAT; }
	virtual void  OnInit( ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual void  OnUse(  int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual void  OnBuy();
	virtual bool  OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck = true );
	virtual void  OnSell( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem );

public:
	ioEtcItemWholeChat();
	virtual ~ioEtcItemWholeChat();
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemGrowthDown : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_GROWTH_DOWN; }
	virtual void  OnUse(  int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual bool  OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck = true );

public:
	ioEtcItemGrowthDown();
	virtual ~ioEtcItemGrowthDown();
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemExtraItemCompound : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_ITEM_COMPOUND; }
	virtual void  OnUse(  int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual bool  OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck = true );

public:
	ioEtcItemExtraItemCompound();
	virtual ~ioEtcItemExtraItemCompound();
};

/////////////
class ioEtcItemExtraItemCompound2 : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_ITEM_COMPOUND2; }
	virtual void  OnUse(  int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual bool  OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck = true );

public:
	ioEtcItemExtraItemCompound2();
	virtual ~ioEtcItemExtraItemCompound2();
};

//////////////
class ioEtcItemExtraItemCompound3 : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_ITEM_COMPOUND3; }
	virtual void  OnUse(  int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual bool  OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck = true );

public:
	ioEtcItemExtraItemCompound3();
	virtual ~ioEtcItemExtraItemCompound3();
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemMultipleCompound : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_MULTIPLE_COMPOUND; }
	virtual void  OnUse(  int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual bool  OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck = true );

public:
	ioEtcItemMultipleCompound();
	virtual ~ioEtcItemMultipleCompound();
};

//////////
class ioEtcItemMultipleCompound2 : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_MULTIPLE_COMPOUND2; }
	virtual void  OnUse(  int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual bool  OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck = true );

public:
	ioEtcItemMultipleCompound2();
	virtual ~ioEtcItemMultipleCompound2();
};

/////////
class ioEtcItemMultipleCompound3 : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_MULTIPLE_COMPOUND3; }
	virtual void  OnUse(  int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual bool  OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck = true );

public:
	ioEtcItemMultipleCompound3();
	virtual ~ioEtcItemMultipleCompound3();
};

class ioEtcItemMultipleCompound4 : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_MULTIPLE_COMPOUND4; }
	virtual void  OnUse(  int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual bool  OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck = true );

public:
	ioEtcItemMultipleCompound4();
	virtual ~ioEtcItemMultipleCompound4();
};

class ioEtcItemMultipleCompound5 : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_MULTIPLE_COMPOUND5; }
	virtual void  OnUse(  int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual bool  OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck = true );

public:
	ioEtcItemMultipleCompound5();
	virtual ~ioEtcItemMultipleCompound5();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemMultipleEqualCompound : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_MULTIPLE_EQUAL_COMPOUND; }
	virtual void  OnUse(  int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual bool  OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck = true );

public:
	ioEtcItemMultipleEqualCompound();
	virtual ~ioEtcItemMultipleEqualCompound();
};

/////////
class ioEtcItemMultipleEqualCompound2 : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_MULTIPLE_EQUAL_COMPOUND2; }
	virtual void  OnUse(  int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual bool  OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck = true );

public:
	ioEtcItemMultipleEqualCompound2();
	virtual ~ioEtcItemMultipleEqualCompound2();
};

//////////
class ioEtcItemMultipleEqualCompound3 : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_MULTIPLE_EQUAL_COMPOUND3; }
	virtual void  OnUse(  int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual bool  OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck = true );

public:
	ioEtcItemMultipleEqualCompound3();
	virtual ~ioEtcItemMultipleEqualCompound3();
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemFishingBait : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_FISHING_BAIT; }

	virtual void  OnUse(  int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual void  OnBuy();
	virtual bool  OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck = true );

public:
	ioEtcItemFishingBait();
	virtual ~ioEtcItemFishingBait();
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemFishingMoonBait : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_FISHING_MOON_BAIT; }

	virtual void  OnUse(  int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual void  OnBuy();
	virtual bool  OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck = true );

public:
	ioEtcItemFishingMoonBait();
	virtual ~ioEtcItemFishingMoonBait();
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemFishingRod : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_FISHING_ROD; }
	virtual bool  OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck = true );

public:
	ioEtcItemFishingRod();
	virtual ~ioEtcItemFishingRod();
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemFishingMoonRod : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_FISHING_MOON_ROD; }
	virtual bool  OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck = true );

public:
	ioEtcItemFishingMoonRod();
	virtual ~ioEtcItemFishingMoonRod();
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemFriendSlotExtend : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_FRIEND_SLOT_EXTEND; }

public:
	virtual bool IsBuyCondition( int iUse );
	virtual bool OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck = true );

public:
	ioEtcItemFriendSlotExtend();
	virtual ~ioEtcItemFriendSlotExtend();
};
//////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemCharSlotExtend : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_CHAR_SLOT_EXTEND; }

public:
	virtual bool IsBuyCondition( int iUse );
	virtual bool OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck = true );

public:
	ioEtcItemCharSlotExtend();
	virtual ~ioEtcItemCharSlotExtend();
};
//////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemFishingSlotExtend : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_FISHING_SLOT_EXTEND; }

public:
	virtual bool IsBuyCondition( int iUse );
	virtual bool OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck = true );

public:
	ioEtcItemFishingSlotExtend();
	virtual ~ioEtcItemFishingSlotExtend();
};
//////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemChangeID : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_CHANGE_ID; }
	virtual void  OnUse(  int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual bool  OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck = true );

public:
	ioEtcItemChangeID();
	virtual ~ioEtcItemChangeID();
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemCustomSound : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_CUSTOM_SOUND; }
	virtual void  OnUse(  int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual bool  OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck = true );

public:
	ioEtcItemCustomSound();
	virtual ~ioEtcItemCustomSound();
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemBuyMortmainChar : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_BUY_MORTMAIN_CHAR; }

public:
	void SetUsed( ioUserEtcItem *pUserEtcItem );
	bool AddEtcItem( ioUserEtcItem *pUserEtcItem, int iGradeLevel, int iBonusLevel );
	virtual bool OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck = true );

public:
	ioEtcItemBuyMortmainChar();
	virtual ~ioEtcItemBuyMortmainChar();
};
//////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemGuildCreate : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_GUILD_CREATE; }
	virtual void  OnUse(  int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual bool  OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck = true );

public:
	ioEtcItemGuildCreate();
	virtual ~ioEtcItemGuildCreate();
};
//////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemGuildMarkChange : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_GUILD_MARK_CHANGE; }
	virtual void  OnUse(  int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual bool  OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck = true );

public:
	ioEtcItemGuildMarkChange();
	virtual ~ioEtcItemGuildMarkChange();
};
//////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemGuildNameChange : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_GUILD_NAME_CHANGE; }
	virtual void  OnUse(  int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual bool  OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck = true );

public:
	ioEtcItemGuildNameChange();
	virtual ~ioEtcItemGuildNameChange();
};
//////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemPesoBonus : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_PESO_BONUS; }
	virtual bool  OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck = true );

public:
	ioEtcItemPesoBonus();
	virtual ~ioEtcItemPesoBonus();
};
//////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemExpBonus : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_EXP_BONUS; }
	virtual bool  OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck = true );

public:
	ioEtcItemExpBonus();
	virtual ~ioEtcItemExpBonus();
};
//////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemSoldierPackage : public ioEtcItem
{
public:	
	enum ActiveFilter
	{
		AF_ALL				= 0, //Active값이 0과 1인 용병
		AF_ACTIVE			= 1, //Active값이 1인 용병
	};

protected:
	int m_iLimitClassTypeNum;
	int m_iActiveFilter;
	IntVec m_vSoldierCode;

public:
	virtual DWORD GetType() const { return m_dwType; }
	virtual void  OnUse(  int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual bool  OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck = true );

public:
	virtual void LoadProperty( ioINILoader &rkLoader );

public:
	inline int GetLimitClassTypeNum() { return m_iLimitClassTypeNum; }
	inline int GetActiveFilter(){ return m_iActiveFilter; }

	bool IsRightSoldierCode( int iClassType );

public:
	ioEtcItemSoldierPackage();
	virtual ~ioEtcItemSoldierPackage();
};
//////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemDecorationPackage : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_DECORATION_PACKAGE; }
	virtual void  OnUse(  int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual bool  OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck = true );

public:
	ioEtcItemDecorationPackage();
	virtual ~ioEtcItemDecorationPackage();
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemGoldMonsterCoin : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_GOLDMONSTER_COIN; }
	virtual void  OnInit( ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual void  OnUse(  int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual void  OnBuy();
	virtual bool  OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck = true );

public:
	ioEtcItemGoldMonsterCoin();
	virtual ~ioEtcItemGoldMonsterCoin();
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemMonsterCoin : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_MONSTER_COIN; }
	virtual void  OnInit( ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual void  OnUse(  int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual void  OnBuy();
	virtual bool  OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck = true );

public:
	ioEtcItemMonsterCoin();
	virtual ~ioEtcItemMonsterCoin();
};
//////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemGashapon : public ioEtcItem
{
public:
	virtual void LoadProperty( ioINILoader &rkLoader );

public:
	virtual DWORD GetType() const { return m_dwType; }
	virtual void  OnUse(  int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual void  OnBuy();
	virtual bool  OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck = true );

public:
	ioEtcItemGashapon();
	virtual ~ioEtcItemGashapon();
};
//////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemRandomDecoM : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_RANDOM_DECO_M; }
	virtual void  OnUse(  int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual bool  OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck = true );

public:
	ioEtcItemRandomDecoM();
	virtual ~ioEtcItemRandomDecoM();
};
//////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemRandomDecoW : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_RANDOM_DECO_W; }
	virtual void  OnUse(  int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual bool  OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck = true );

public:
	ioEtcItemRandomDecoW();
	virtual ~ioEtcItemRandomDecoW();
};
//////////////////////////////////////////////////////////////////////////
class ioEtcItemPackage : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return m_dwType; }
	virtual void  OnUse(  int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual bool  OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck = true );

public:
	ioEtcItemPackage();
	virtual ~ioEtcItemPackage();
};
///////////////////////////////////////////////////////////////////////////
class ioEtcItemDecoUnderwearPackage : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_DECO_UNDERWEAR_PACKAGE; }
	virtual void  OnUse(  int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual bool  OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck = true );

public:
	ioEtcItemDecoUnderwearPackage();
	virtual ~ioEtcItemDecoUnderwearPackage();
};
///////////////////////////////////////////////////////////////////////////
class ioEtcItemExcavatingKit : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return m_dwType; }
	virtual void  OnUse(  int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual bool  OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck = true );
	virtual bool IsBuyCondition( int iUse );

public:
	ioEtcItemExcavatingKit();
	virtual ~ioEtcItemExcavatingKit();
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemTradeStateChange : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_TRADE_STATE_CHANGE; }
	virtual void  OnUse(  int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual bool  OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck = true );

public:
	ioEtcItemTradeStateChange();
	virtual ~ioEtcItemTradeStateChange();
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemQuestEvent : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_QUEST_EVENT; }

public:
	ioEtcItemQuestEvent();
	virtual ~ioEtcItemQuestEvent();
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemSilverCoin : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_SILVER_COIN; }
	virtual void  OnInit( ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual void  OnUse(  int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual void  OnBuy();
	virtual bool  OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck = true );

public:
	ioEtcItemSilverCoin();
	virtual ~ioEtcItemSilverCoin();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemMileageCoin : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_MILEAGE_COIN; }
	virtual void  OnInit( ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual void  OnUse(  int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual void  OnBuy();
	virtual bool  OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck = true );

public:
	ioEtcItemMileageCoin();
	virtual ~ioEtcItemMileageCoin();
};

//////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemBattleRecordInit : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_BATTLE_RECORD_INIT; }
	virtual void  OnUse(  int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual bool  OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck = true );

public:
	ioEtcItemBattleRecordInit();
	virtual ~ioEtcItemBattleRecordInit();
};

//////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemLadderRecordInit : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_LADDER_RECORD_INIT; }
	virtual void  OnUse(  int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual bool  OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck = true );

public:
	ioEtcItemLadderRecordInit();
	virtual ~ioEtcItemLadderRecordInit();
};

//////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemHeroRecordInit : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_HERO_RECORD_INIT; }
	virtual void  OnUse(  int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual bool  OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck = true );

public:
	ioEtcItemHeroRecordInit();
	virtual ~ioEtcItemHeroRecordInit();
};
//////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemSkeleton : public ioEtcItem
{
protected:
	ioHashString m_szHumanManSkl;
	ioHashString m_szHumanWomanSkl;

public:
	virtual void   LoadProperty( ioINILoader &rkLoader );

public:
	virtual DWORD GetType() const { return m_dwType; }
	virtual bool  IsBuyCondition( int iUse );
	virtual bool  OnUseSwitchBtnPush( ioUserEtcItem *pUserEtcItem, bool bUseOn, ioPlayStage *pPlayStage );
	virtual bool  OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck = true );
	virtual void  OnUseSwitch( SP2Packet &rkPacket, ioPlayStage *pPlayStage );
	virtual bool  IsShop3DChar(){ return true; }

public:
	virtual const ioHashString &GetHumanManSkl(){ return m_szHumanManSkl; }
	virtual const ioHashString &GetHumanWomanSkl(){ return m_szHumanWomanSkl; }

public:
	ioEtcItemSkeleton();
	virtual ~ioEtcItemSkeleton();
};
class ioEtcItemSkeletonBig : public ioEtcItemSkeleton
{
public:
	virtual DWORD GetType() const { return EIT_ETC_SKELETON_BIG; }

public:
	ioEtcItemSkeletonBig(){}
	virtual ~ioEtcItemSkeletonBig(){}
};
class ioEtcItemSkeletonBigHead : public ioEtcItemSkeleton
{
public:
	virtual DWORD GetType() const { return EIT_ETC_SKELETON_BIGHEAD; }

public:
	ioEtcItemSkeletonBigHead(){}
	virtual ~ioEtcItemSkeletonBigHead(){}
};
class ioEtcItemSkeletonSmall : public ioEtcItemSkeleton
{
public:
	virtual DWORD GetType() const { return EIT_ETC_SKELETON_SMALL; }

public:
	ioEtcItemSkeletonSmall(){}
	virtual ~ioEtcItemSkeletonSmall(){}
};
//////////////////////////////////////////////////////////////////////////
class ioEtcItemMotion : public ioEtcItem
{
public:
	enum
	{   // 1은 설정 안함이다.                                   
		MOTION_OPTION_1 = 2,            // 게임 승리 포즈
		MOTION_OPTION_2 = 3,			// 게임 패배 포즈
		MOTION_OPTION_3 = 4,			// 좋은 수상 포즈
		MOTION_OPTION_4 = 5,			// 나쁜 수상 포즈
		MOTION_OPTION_5 = 6,			// 전투대기방 포즈
		MOTION_OPTION_6 = 7,			// 진영전 대기방 포즈
		MOTION_OPTION_7 = 8,			// 래더 페이지 포즈
		MOTION_OPTION_8 = 9,			// 래더전 대기방 포즈
		MOTION_OPTION_9 = 10,           // 특수 동작 포즈
		MOTION_OPTION_10= 11,           // 대표 용병 포즈
		MOTION_OPTION_100 = 101,        // 진열 용병 포즈 예약 - 용병 200개
		MOTION_OPTION_300 = 300,        // 진열 용병 포즈 예약 - 용병 200개

		MAX_OPTION,
	};

protected:
	ioHashString	m_szHumanManMotion;
	ioHashString	m_szHumanWomanMotion;
	ioHashStringVec m_vMotionMacro;
	bool			m_bJoinUserSync;
	bool            m_bDisplayMotion;

protected:
	struct MotionOption
	{
		DWORD m_dwType;
		ioHashString m_szTitle;
		ioHashString m_szUITitle;
		bool  m_bOnlyOneMotion;
		MotionOption()
		{
			m_dwType = 0;
			m_bOnlyOneMotion = false;
		}
	};
	typedef std::vector< MotionOption > vMotionOption;
	vMotionOption m_MotionOption;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual bool IsBuyCondition( int iUse );
	virtual void OnBuy();
	virtual bool IsShop3DChar(){ return true; }

public:
	virtual DWORD GetType() const { return m_dwType; }
	virtual void  OnUse(  int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );

public:
	const ioHashString &GetHumanManMotion(){ return m_szHumanManMotion; }
	const ioHashString &GetHumanWomanMotion(){ return m_szHumanWomanMotion; }

public:
	bool IsMotionMacro( const ioHashString &rkMacro );

public:
	void GetDisplayMotion( DWORDVec &rkClassList );
	int GetMaxMotionOption();
	DWORD GetMotionOptionArrayToID( int iArray );
	ioHashString GetMotionOptionTitle( DWORD dwType );
	ioHashString GetMotionOptionUITitle( DWORD dwType );
	bool IsMotionOptionOnlyOne( DWORD dwType );
	
public:
	inline bool IsJoinUserSync(){ return m_bJoinUserSync; }

public:
	ioEtcItemMotion();
	virtual ~ioEtcItemMotion();
};
//////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemCustomItemSkin : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_CUSTOM_ITEM_SKIN; }
	virtual void  OnUse(  int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual bool  OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck = true );

public:
	ioEtcItemCustomItemSkin();
	virtual ~ioEtcItemCustomItemSkin();
};
//////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemCustomItemSkinTest : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_CUSTOM_ITEM_SKIN_TEST; }
	virtual void  OnUse(  int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual bool  OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck = true );

public:
	ioEtcItemCustomItemSkinTest();
	virtual ~ioEtcItemCustomItemSkinTest();
};
//////////////////////////////////////////////////////////////////////////

class ioEtcItemBlock : public ioEtcItem
{
protected:
	ioHashString m_szCommond;
	vBlockItemInfoList m_vBlockItemInfoList;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual void OnBuy();

public:
	virtual DWORD GetType() const { return m_dwType; }

public:
	bool IsBlockMacro( const ioHashString &rkMacro );

	bool GetBlockGuidEffect( IN int iIndex, OUT ioHashString &szEffect );
	bool GetBlockStructNum( IN int iIndex, OUT int &iStructNum );

	int GetPreBlockIndex( int iIndex );
	int GetNextBlockIndex( int iIndex );

public:
	ioEtcItemBlock();
	virtual ~ioEtcItemBlock();
};
///////////////////////////////////////////////////////////////////////////
class ioEtcItemEventCheck : public ioEtcItem
{
public:
	ioEtcItemEventCheck(){}
	virtual ~ioEtcItemEventCheck(){}
};
//////////////////////////////////////////////////////////////////////////
class ioEtcItemExtraItemGrowthCatalyst : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_ITEM_GROWTH_CATALYST; }
	virtual void  OnUse(  int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual bool  OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck = true );

public:
	ioEtcItemExtraItemGrowthCatalyst();
	virtual ~ioEtcItemExtraItemGrowthCatalyst();
};
//////////////////////////////////////////////////////////////////////////
class ioEtcItemLuckyCoin : public ioEtcItem
{
protected:
	struct ExchangeData
	{
		int m_iExchangeIndex;
		int m_iExchangeUseManual;
		int m_iExchangeResultManual;
		ioHashString m_szExchangeName;
		ioHashString m_szExchangeResultName;
		ioHashString m_szExchangeIcon;
		bool m_bExchangeCostume;
		int m_nGradeType;
		
		int	 m_iExchangeTabType;
		ioHashString m_szExchangeSubText;
		ioHashString m_szExchangeSubText2;
		int  m_iNeedCoin;

		ExchangeData()
		{
			m_iExchangeIndex = m_iExchangeUseManual = m_iExchangeResultManual = m_nGradeType = 0;
			m_iNeedCoin = m_iExchangeTabType = -1;
		}
	};
	typedef std::vector< ExchangeData > vExchangeData;
	vExchangeData m_vExchangeList;

	ioHashStringVec m_szRadioNameList;
public:
	virtual void LoadProperty( ioINILoader &rkLoader );

public:
	virtual DWORD GetType() const { return m_dwType; }
	virtual void  OnUse(  int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual bool  OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck = true );

public:
	inline const int GetExchangeCount() const { return m_vExchangeList.size(); }
	void GetExchangeUseData( int iArray, int &rExchangeIndex, int &rExchangeManual, ioHashString &rExchangeName, ioHashString &rExchangeIcon, bool &rExchangeCostume, int &rGradeType );
	void GetExchangeUseTabData( int iArray, ioHashString &rExchangeSubText, ioHashString &rExchangeSubText2, int &rExchangeTabType, int& rExchangeNeedCoin );
	void GetExchangeResultData( int iExchangeIndex, int &rExchangeManual, ioHashString &rExchangeName, ioHashString &rExchangeIcon, bool &rExchangeCostume, int &rGradeType );
	inline const ioHashStringVec& GetRadionNameList() { return m_szRadioNameList; }
public:
	ioEtcItemLuckyCoin();
	virtual ~ioEtcItemLuckyCoin();
};
//////////////////////////////////////////////////////////////////////////
class ioEtcItemCompoundEx : public ioEtcItem
{
protected:
	int m_iCompoundSuccessRand;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );

public:
	virtual DWORD GetType() const { return m_dwType; }
	virtual void  OnUse(  int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual bool  OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck = true );

public:
	ioEtcItemCompoundEx();
	virtual ~ioEtcItemCompoundEx();
};
//////////////////////////////////////////////////////////////////////////
class ioEtcItemRainbowMixer : public ioEtcItem 
{
protected:
	/*************************/
	/*           1           */
	/*     6           2     */
	/*           0           */
	/*     5           3     */
	/*           4           */
	/*************************/
	
	struct MixerData
	{
		DWORD m_dwEtcItemCode;
		ioHashString m_szEquipIcon;
		ioHashString m_szReleaseIcon;
		MixerData()
		{
			m_dwEtcItemCode = 0;
		}
	};
	typedef std::vector< MixerData > vMixerData;
	vMixerData m_MixerList;

	struct PatternData
	{
		DWORDVec m_vCodeList;		
		DWORD m_dwPresentCode;
		ioHashString m_szPresentIcon;
		ioHashString m_szPresentName;
		PatternData()
		{
			m_dwPresentCode = 0;
		}
	};
	typedef std::vector< PatternData > vPatternData;
	vPatternData m_PatternList;

	ioHashStringVec m_BackgroundList;

	int m_iSubResultManual;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );

public:
	virtual DWORD GetType() const { return m_dwType; }
	virtual void  OnUse(  int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual bool  OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck = true );

public:
	void GetRandomBackGround( ioHashString &rReturnImage );

public:
	int GetMaxMixerList(){ return m_MixerList.size(); }
	int GetMixerData( int iArray, ioHashString &rReturnIcon, ioHashString &rReleaseIcon );
	void GetPatternResultData( int iPresentIdx, ioHashString &rReturnIcon, ioHashString &rReturnName );

public:
	int  GetPatternPresent( DWORDVec &rEquipList );
	int  GetSubResultManual(){ return m_iSubResultManual; }

public:
	ioEtcItemRainbowMixer();
	virtual ~ioEtcItemRainbowMixer();
};
//////////////////////////////////////////////////////////////////////////
class ioEtcItemLostSagaMixer : public ioEtcItemRainbowMixer 
{
public:
	virtual DWORD GetType() const { return m_dwType; }
	virtual void  OnUse(  int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual bool  OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck = true );

public:
	ioEtcItemLostSagaMixer();
	virtual ~ioEtcItemLostSagaMixer();
};
//////////////////////////////////////////////////////////////////////////
class ioEtcItemTimeGashapon : public ioEtcItem 
{
public:

	//순차획득 가챠 전용
	enum
	{
		SEQUENCE_STATE_VALUE = 10000,	
	};

protected:
	/************************************************************************/
	/* RepeatMinute마다 선물 획득 가능한 아이템								*/
	/************************************************************************/
	bool	m_bRealTimeCheck;
	int  	m_iRepeatMinute;

protected:
	/************************************************************************/
	/* ExtendData에 값이 세팅되면 유지한 시간에 따라 선물이 바뀌고 사용시      */
	/* 특별아이템에서 삭제된다                                               */
	/************************************************************************/
	struct ExtendData
	{
		int m_iUseMinute;
		int m_iMagicCode;

		ExtendData()
		{
			m_iUseMinute = m_iMagicCode = -1;
		}
	};
	typedef std::vector< ExtendData > ExtendDataVec;
	ExtendDataVec m_ExtendDataList;

protected:
	bool m_bSequenceOrder;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );

public:
	virtual DWORD GetType() const { return m_dwType; }
	virtual void  OnUse( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual bool  OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck = true );

public:
	int  GetRepeatDate(){ return m_iRepeatMinute; }
	bool IsExtendData(){ return !m_ExtendDataList.empty(); }
	bool IsSequenceOrder(){ return m_bSequenceOrder; }

public:
	int GetSequenceDate( int iIndex );

public:
	DWORD GetExtendGashaponType( ioUserEtcItem *pUserEtcItem );
	bool  IsBtnActive( ioUserEtcItem *pUserEtcItem );

public:
	ioEtcItemTimeGashapon();
	virtual ~ioEtcItemTimeGashapon();
};
//////////////////////////////////////////////////////////////////////////
class ioEtcItemGoldBox : public ioEtcItemRainbowMixer 
{
public:
	virtual DWORD GetType() const { return m_dwType; }
	virtual void  OnUse(  int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual bool  OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck = true );

public:
	ioEtcItemGoldBox();
	virtual ~ioEtcItemGoldBox();
};

//////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemSoldierSelector : public ioEtcItem
{
public:
	enum ActiveFilter
	{
		AF_ALL				= 0, //Active값이 0과 1인 용병
		AF_ACTIVE			= 1, //Active값이 1인 용병
	};

protected:
	IntVec       m_vSoldierCode;
	int          m_iInventorySubManual2;
	int			 m_iActiveFilter;

public:
	bool		IsRightSoldierCode( int iClassType );
	inline int	GetInventorySubManual2() const { return m_iInventorySubManual2; }
	inline int	GetActiveFilter(){ return m_iActiveFilter; }


public:
	virtual void LoadProperty( ioINILoader &rkLoader );

public:
	virtual DWORD GetType() const { return m_dwType; }
	virtual void  OnUse(  int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual bool  OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck = true );

public:
	ioEtcItemSoldierSelector();
	virtual ~ioEtcItemSoldierSelector();
};

//////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemFourExtraCompound : public ioEtcItem
{
public:
	enum 
	{
		MAX_EXTRA_ITEM = 4,
	};
protected:
	IntVec       m_vSoldierCode;

public:
	bool         IsRightSoldierCode( int iClassType );

public:
	virtual void LoadProperty( ioINILoader &rkLoader );

public:
	virtual DWORD GetType() const { return m_dwType; }
	virtual void  OnUse(  int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual bool  OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck = true );

public:
	ioEtcItemFourExtraCompound();
	virtual ~ioEtcItemFourExtraCompound();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemExpandMedalSlot : public ioEtcItem
{
protected:
	enum
	{
		SLOT_NONE = 0,	// ExMedalSlot에서 1번은 쓰지 않는다.
		SLOT_NUM2 = 1,
		SLOT_NUM3 = 2,
		SLOT_NUM4 = 3,
		SLOT_NUM5 = 4,
		SLOT_NUM6 = 5,
	};
	CEncrypt<int>	m_iUseLevel;
	CEncrypt<DWORD>	m_dwLimitTime;
	BYTE			m_iSlotNumber;

public:
	virtual void   LoadProperty( ioINILoader &rkLoader );

public:
	virtual DWORD GetType() const { return m_dwType; }
	virtual void  OnUse(  int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual bool  OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck = true );

public:
	inline const int GetLimitLevel() const { return m_iUseLevel; }
	inline const DWORD GetLimitTime() const { return m_dwLimitTime; }
	inline const int GetUseSlotNumber() const { return m_iSlotNumber; }

public:
	ioEtcItemExpandMedalSlot();
	virtual ~ioEtcItemExpandMedalSlot();
};

//////////////////////////////////////////////////////////////////////////
class ioEtcItemSoldierExpBonus : public ioEtcItem 
{
	// 아이템 사용하기 - > 용병 선택 - > 해당 용병 경험치 보너스
protected:
	int m_iLimitLevel;
	int m_iUseValue;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );

public:
	virtual DWORD GetType() const { return m_dwType; }
	virtual void  OnUse(  int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual bool  OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck = true );

public:
	inline const int GetLimitLevel() const { return m_iLimitLevel; }
	inline const int GetUseValue() const { return m_iUseValue; }

public:
	ioEtcItemSoldierExpBonus();
	virtual ~ioEtcItemSoldierExpBonus();
};

//////////////////////////////////////////////////////////////////////////
//버프효과가 있는 소모품
//////////////////////////////////////////////////////////////////////////
class ioEtcItemConsumption : public ioEtcItem
{
protected:
	ioHashString	m_strIcon;
	ioHashString	m_strBackImg;
	ioHashString	m_strSlotBuff;

	DWORD			m_dwCoolTime;

public:
	virtual DWORD GetType() const { return m_dwType; }
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual void  OnInit( ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual void  OnUse(  int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual void  OnBuy();
	virtual bool  OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck = true );
	virtual void  OnSell( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem );

public:
	const ioHashString& GetIconImg() {return m_strIcon;}
	const ioHashString& GetIconBackImg() {return m_strBackImg;}
	const ioHashString& GetSlotBuff() {return m_strSlotBuff;}
	

	const DWORD	GetCoolTime() {return m_dwCoolTime;}

public:
	ioEtcItemConsumption();
	virtual ~ioEtcItemConsumption();
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//즉시 부활
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemRevive : public ioEtcItemConsumption
{
protected:

public:
	virtual DWORD GetType() const { return m_dwType; }
	virtual void  OnUse(  int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual void  OnBuy();

public:

public:
	ioEtcItemRevive();
	virtual ~ioEtcItemRevive();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class ioEtcItemSelectExtraGashapon : public ioEtcItem
{
protected:
	int m_iExtraItemMachineCode;

	ioHashString m_szTtitle;
	ioHashString m_szMent1;
	ioHashString m_szMent2;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual bool OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck = true );
	virtual void OnUse(  int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
public:
	inline const int GetExtraItemMachineCode() const { return m_iExtraItemMachineCode; }

	inline const ioHashString& GetTtitle() const { return m_szTtitle; }
	inline const ioHashString& GetMent1() const { return m_szMent1; }
	inline const ioHashString& GetMent2() const { return m_szMent2; }

public:
	ioEtcItemSelectExtraGashapon();
	virtual ~ioEtcItemSelectExtraGashapon();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class ioEtcItemPreSetPackage : public ioEtcItem
{
protected:
	int m_iClassType;
	int m_iLimitTime;
	ITEMSLOTVec m_vItemSlot;

	ITEMSLOT m_SlotKindred;
	ITEMSLOT m_SlotHair;
	ITEMSLOT m_SlotHairColor;
	ITEMSLOT m_SlotFace;
	ITEMSLOT m_SlotSkin;
	ITEMSLOT m_SlotUnder;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );

public:
	virtual DWORD GetType() const { return m_dwType; }
	virtual void  OnUse(  int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual bool  OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck = true );

public:
	void SetPreSetPackageInfo( CHARACTER &rkCharInfo );

public:
	ioEtcItemPreSetPackage();
	virtual ~ioEtcItemPreSetPackage();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class ioEtcItemGrowthAllDown : public ioEtcItem
{
public:
	virtual void  OnUse(  int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual bool  OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck = true );

public:
	ioEtcItemGrowthAllDown();
	virtual ~ioEtcItemGrowthAllDown();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class ioEtcItemPrizeItem : public ioEtcItem
{
protected:
	PrizeDataVec m_PrizeDataVec;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );

public:
	virtual DWORD GetType() const { return m_dwType; }
	virtual void  OnUse(  int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual bool  OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck = true );

public:
	ioEtcItemPrizeItem();
	virtual ~ioEtcItemPrizeItem();
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class ioEtcItemTournamentCreate : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_TOURNAMENT_CREATE; }
	virtual void  OnUse(  int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual bool  OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck = true );

public:
	ioEtcItemTournamentCreate();
	virtual ~ioEtcItemTournamentCreate();
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class ioEtcItemTournamentPremiumCreate : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_TOURNAMENT_PREMIUM_CREATE; }
	virtual void  OnUse(  int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual bool  OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck = true );

public:
	ioEtcItemTournamentPremiumCreate();
	virtual ~ioEtcItemTournamentPremiumCreate();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemClover : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_CLOVER; }
	virtual void  OnInit( ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual void  OnUse(  int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual bool  OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck = true );

public:
	ioEtcItemClover();
	virtual ~ioEtcItemClover();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class ioEtcItemAddCash : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return m_dwType; }
	virtual void  OnUse(  int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual bool  OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck = true );

public:
	ioEtcItemAddCash();
	virtual ~ioEtcItemAddCash();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class ioEtcItemTournamentCoin : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_TOURNAMENT_COIN; }
	virtual void  OnInit( ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual void  OnUse(  int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual void  OnBuy();
	virtual bool  OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck = true );

public:
	ioEtcItemTournamentCoin();
	virtual ~ioEtcItemTournamentCoin();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class ioEtcItemRouletteCoin : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_ROULETTE_COIN; }
	virtual void  OnInit( ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual void  OnUse(  int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual bool  OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck = true );

public:
	ioEtcItemRouletteCoin();
	virtual ~ioEtcItemRouletteCoin();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class ioEtcItemBingoItem : public ioEtcItem
{
protected:
	ioHashString m_szTtitle;

public:
	virtual DWORD GetType() const { return EIT_ETC_BINGO_ITEM; }
	virtual void  OnInit( ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual void  LoadProperty( ioINILoader &rkLoader );
	virtual void  OnUse(  int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual bool  OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck = true );

public:
	ioEtcItemBingoItem();
	virtual ~ioEtcItemBingoItem();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class ioEtcItemBingoNumberGashapon : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_BINGO_NUMBER_GASHAPON; }
	virtual void  OnInit( ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual void  OnUse(  int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual bool  OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck = true );

public:
	ioEtcItemBingoNumberGashapon();
	virtual ~ioEtcItemBingoNumberGashapon();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class ioEtcItemBingoShuffleNumber : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_BINGO_SHUFFLE_NUMBER; }
	virtual void  OnInit( ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual void  OnUse(  int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual bool  OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck = true );

public:
	ioEtcItemBingoShuffleNumber();
	virtual ~ioEtcItemBingoShuffleNumber();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class ioEtcItemBingoShuffleRewardItem : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_BINGO_SHUFFLE_REWARD_ITEM; }
	virtual void  OnInit( ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual void  OnUse(  int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual bool  OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck = true );

public:
	ioEtcItemBingoShuffleRewardItem();
	virtual ~ioEtcItemBingoShuffleRewardItem();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class ioEtcItemBingoRandomNumberClear : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_BINGO_RANDOM_NUMBER_CLEAR; }
	virtual void  OnInit( ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual void  OnUse(  int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual bool  OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck = true );

public:
	ioEtcItemBingoRandomNumberClear();
	virtual ~ioEtcItemBingoRandomNumberClear();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

enum SuperGashponUseType;
class ioEtcItemSuperGashapon : public ioEtcItem
{
protected:
	SuperGashponUseType m_SuperGashponUseType;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );

public:
	virtual DWORD GetType() const { return m_dwType; }
	virtual void  OnUse(  int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual void  OnBuy();
	virtual bool  OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck = true );

public:
	inline void SetUseType( SuperGashponUseType Type ){ m_SuperGashponUseType = Type; }

public:
	ioEtcItemSuperGashapon();
	virtual ~ioEtcItemSuperGashapon();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemSendPresent : public ioEtcItem
{
public:
	typedef struct PresentData
	{
		ioHashString m_szSendID;

		int m_iPresentState;
		int	m_iPresentType;
		int	m_iPresentMent;
		int	m_iPresentPeriod;

		int m_iPresentValue1;
		int m_iPresentValue2;
		int m_iPresentValue3;
		int m_iPresentValue4;

		PresentData()
		{
			Init();
		}

		void Init()
		{
			m_szSendID.Clear();

			m_iPresentState = 0;
			m_iPresentType = 0;		// 선물 타입
			m_iPresentMent = 0;		// 선물 받았을때 맨트
			m_iPresentPeriod = 0;	// 선물 기간

			m_iPresentValue1 = 0;
			m_iPresentValue2 = 0;
			m_iPresentValue3 = 0;
			m_iPresentValue4 = 0;
		}

	} PresentData;
	typedef std::vector<PresentData> vPresentData;

protected:
	vPresentData m_vPresentInfo;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );

public:
	virtual DWORD GetType() const { return m_dwType; }
	virtual void  OnUse(  int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual bool  OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck = true );

public:
	ioEtcItemSendPresent();
	virtual ~ioEtcItemSendPresent();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class ioEtcItemSoldierExpAdd : public ioEtcItem 
{
public:
	virtual DWORD GetType() const { return m_dwType; }
	virtual void  OnUse( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual bool  OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck = true );

public:
	ioEtcItemSoldierExpAdd();
	virtual ~ioEtcItemSoldierExpAdd();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemRecharge : public ioEtcItem
{
public:
	struct RechargeInfo
	{
		int   m_iPeriod;
		float m_fMinRate;
		float m_fMaxRate;
		RechargeInfo()
		{
			m_iPeriod  = -1;
			m_fMinRate = 0.0f;
			m_fMaxRate = 0.0f;
		}
		RechargeInfo( int iPeriod, float fMinRate, float fMaxRate )
		{
			m_iPeriod  = iPeriod;
			m_fMinRate = fMinRate;
			m_fMaxRate = fMaxRate;
		}
	};
	typedef std::vector<RechargeInfo> vRechargeInfo;

protected:
	CEncrypt<int> m_iMaxInfoCnt;
	vRechargeInfo m_RechargeInfoList;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );

public:
	virtual DWORD GetType() const { return m_dwType; }
	virtual void  OnUse(  int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual bool  OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck = true );

public:
	inline int GetMaxRechargeInfoCount() { return m_iMaxInfoCnt; }
	int GetRechargeTime( int iArray );

public:
	ioEtcItemRecharge();
	virtual ~ioEtcItemRecharge();
};



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcAccessoryRecharge : public ioEtcItem
{
public:
	struct RechargeInfo
	{
		int   m_iPeriod;
		float m_fMinRate;
		float m_fMaxRate;
		RechargeInfo()
		{
			m_iPeriod  = -1;
			m_fMinRate = 0.0f;
			m_fMaxRate = 0.0f;
		}
		RechargeInfo( int iPeriod, float fMinRate, float fMaxRate )
		{
			m_iPeriod  = iPeriod;
			m_fMinRate = fMinRate;
			m_fMaxRate = fMaxRate;
		}
	};
	typedef std::vector<RechargeInfo> vRechargeInfo;

protected:
	CEncrypt<int> m_iMaxInfoCnt;
	vRechargeInfo m_RechargeInfoList;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );

public:
	virtual DWORD GetType() const { return m_dwType; }
	virtual void  OnUse(  int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual bool  OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck = true );

public:
	inline int GetMaxRechargeInfoCount() { return m_iMaxInfoCnt; }
	int GetRechargeTime( int iArray );

public:
	ioEtcAccessoryRecharge();
	virtual ~ioEtcAccessoryRecharge();
};

///////////////////////////////////////////////////////////////////////////
struct TabFiter
{
	int m_iTypeFilter;

	TabFiter()
	{
		m_iTypeFilter	  = 0;
	}
};

typedef std::vector<TabFiter> TabFiterVec;

struct TabUISetting
{
	enum
	{
		ORDER_BY_CODE     = 1,
		ORDER_BY_SETTING  = 2,
	};

	ioHashString m_szTtitle;
	TabFiterVec m_FilterVec;

	int m_iOrder;
	int m_iSpecialAddType;

public:

	TabUISetting()
	{
		Clear();
	}

	void Clear()
	{
		m_szTtitle.Clear();
		m_FilterVec.clear();

		m_iOrder          = 0;
		m_iSpecialAddType = 0;
	}
};
typedef std::vector<TabUISetting> TabUISettingVec;

class ioSelectGashapon : public ioEtcItem
{
public:
	enum 
	{
		TAB_MAX = 5,
	};

protected:
	TabUISettingVec m_TabUISettingVec;

protected:
	ioHashString m_szTtitle;
	ioHashString m_szMent1;
	ioHashString m_szMent2;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual bool OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck = true );
	virtual void OnUse(  int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );

public:
	inline const TabUISettingVec& GetTabSettingVec(){ return m_TabUISettingVec; }

	inline const ioHashString& GetTtitle() const { return m_szTtitle; }
	inline const ioHashString& GetMent1() const { return m_szMent1; }
	inline const ioHashString& GetMent2() const { return m_szMent2; }

public:
	ioSelectGashapon();
	virtual ~ioSelectGashapon();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemFixedBingoNumber : public ioEtcItem
{
public:
	virtual void  OnUse(  int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );

public:
	ioEtcItemFixedBingoNumber();
	virtual ~ioEtcItemFixedBingoNumber();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemMaterialCompound : public ioEtcItem 
{
public:
	virtual void  LoadProperty( ioINILoader &rkLoader );
	virtual DWORD GetType() const { return m_dwType; }
	virtual void  OnUse( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual bool  OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck = true );

public:
	ioEtcItemMaterialCompound();
	virtual ~ioEtcItemMaterialCompound();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemPetEgg : public ioEtcItem 
{
public:
	virtual void  LoadProperty( ioINILoader &rkLoader );
	virtual DWORD GetType() const { return m_dwType; }
	virtual void  OnUse( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual bool  OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck = true );

public:
	ioEtcItemPetEgg();
	virtual ~ioEtcItemPetEgg();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemSoulStone : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_SOUL_STONE; }
	virtual void  OnInit( ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual void  OnUse(  int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual void  OnBuy();
	virtual bool  OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck = true );

public:
	void OnUseGradeAdjustment( int iCharIndex, PowerUpTargetType eTargetType );
	//void OnUseItemPowerUp( int iItemIndex, PowerUpTargetType eTargetType );

public:
	ioEtcItemSoulStone();
	virtual ~ioEtcItemSoulStone();
};

//////////////////////////////////////////////////////////////////////////////////////////////

class ioEtcCostumeCustomItemSkin : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_CUSTOM_COSTUME_SKIN; }
	virtual void  OnUse(  int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual bool  OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck = true );

public:
	ioEtcCostumeCustomItemSkin();
	virtual ~ioEtcCostumeCustomItemSkin();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemExtraItemSlotExtend : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_EXTRAITEM_SLOT_EXTEND; }

public:
	virtual bool IsBuyCondition( int iUse );
	virtual bool OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck = true );

public:
	ioEtcItemExtraItemSlotExtend();
	virtual ~ioEtcItemExtraItemSlotExtend();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemNewMultipleCompound : public ioEtcItem
{
protected:
	ioHashStringVec m_szRadioNameList;
	DWORD			m_dwMaterialType;
	DWORD			m_dwResultGashaponIndex;
public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual DWORD GetType() const { return m_dwType; }
	virtual void  OnUse(  int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual bool  OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck = true );
	inline const ioHashStringVec& GetRadionNameList() { return m_szRadioNameList; }
	inline const DWORD& GetMaterialType() { return m_dwMaterialType; }
	inline const DWORD& GetResultGashaponIndex() { return m_dwResultGashaponIndex; }
public:
	ioEtcItemNewMultipleCompound();
	virtual ~ioEtcItemNewMultipleCompound();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class ioEtcItemHousingBlockItem : public ioEtcItem
{
protected:
	DWORD			m_dwBlockItemCode;
	ioHashString	m_BlockPresentDesc;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual DWORD GetType() const { return m_dwType; }
	virtual DWORD GetBlockCode() const{ return m_dwBlockItemCode; }

public:
	const ioHashString& GetBlockPresentDesc() const{ return m_BlockPresentDesc; }

public:
	virtual void  OnUse( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual bool  OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck = true );
	
public:
	ioEtcItemHousingBlockItem();
	virtual ~ioEtcItemHousingBlockItem();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class ioEtcItemCreateGuildHQCreate : public ioEtcItem
{
public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual DWORD GetType() const { return m_dwType; }	

	virtual void  OnUse( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual bool  OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck = true );

public:
	ioEtcItemCreateGuildHQCreate();
	virtual ~ioEtcItemCreateGuildHQCreate();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class ioEtcItemCreateMyHomeCreate : public ioEtcItem
{
public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual DWORD GetType() const { return m_dwType; }	

	virtual void  OnUse( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual bool  OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck = true );

public:
	ioEtcItemCreateMyHomeCreate();
	virtual ~ioEtcItemCreateMyHomeCreate();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class ioEtcItemNamedTitle : public ioEtcItem
{
public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual DWORD GetType() const { return m_dwType; }
	
public:
	virtual void  OnUse( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual bool  OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck = true );

public:
	ioEtcItemNamedTitle();
	virtual ~ioEtcItemNamedTitle();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class ioEtcItemNamedTitlePremium : public ioEtcItem
{
public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual DWORD GetType() const { return m_dwType; }

public:
	virtual void  OnUse( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual bool  OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck = true );

public:
	ioEtcItemNamedTitlePremium();
	virtual ~ioEtcItemNamedTitlePremium();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class ioEtcItemTimeCash : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return m_dwType; }
	virtual bool  OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck = true );

public:
	ioEtcItemTimeCash();
	virtual ~ioEtcItemTimeCash();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class ioEtcItemOakDrumItem : public ioEtcItem
{
public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual DWORD GetType() const { return m_dwType; }	

	virtual void  OnUse( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual bool  OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck = true );

public:
	ioEtcItemOakDrumItem();
	virtual ~ioEtcItemOakDrumItem();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class ioEtcItemOakSwordItem : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return m_dwType; }

public:
	ioEtcItemOakSwordItem();
	virtual ~ioEtcItemOakSwordItem();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class ioEtcItemPCRoomFishingRod : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_PCROOM_FISHING_ROD; }

public:
	ioEtcItemPCRoomFishingRod();
	virtual ~ioEtcItemPCRoomFishingRod();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class ioEtcItemPCRoomFishingBait : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_PCROOM_FISHING_BAIT; }

public:
	ioEtcItemPCRoomFishingBait();
	virtual ~ioEtcItemPCRoomFishingBait();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemRaidCoin : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_RAID_COIN; }
	virtual void  OnInit( ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual void  OnUse(  int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual void  OnBuy();
	virtual bool  OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck = true );

public:
	ioEtcItemRaidCoin();
	virtual ~ioEtcItemRaidCoin();
};
/////////////////////////////////////////////////////////////////////////////
class ioEtcItemAccessoryCompound : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_ITEM_COMPOUND; }
	virtual void  OnUse(  int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual bool  OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck = true );

public:
	ioEtcItemAccessoryCompound();
	virtual ~ioEtcItemAccessoryCompound();
};
/////////////////////////////////////////////////////////////////////////////
class ioEtcItemExcavatingShovel : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_NEW_EXCAVATING_SHOVEL; }
	virtual void  OnUse(  int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual bool  OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck = true );

public:
	ioEtcItemExcavatingShovel();
	virtual ~ioEtcItemExcavatingShovel();
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemCardMatching : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_CARD_MATCH_ITEM; }
	virtual void  OnUse(  int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual bool  OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck = true );

public:
	ioEtcItemCardMatching();
	virtual ~ioEtcItemCardMatching();
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemCardMatchingTicket : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_CARD_MATCH_TICKET; }
	virtual void  OnUse(  int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual bool  OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck = true );

public:
	ioEtcItemCardMatchingTicket();
	virtual ~ioEtcItemCardMatchingTicket();
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemCardMatchingPremiumTicket : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_CARD_MATCH_PREMIUM_TICKET; }
	virtual void  OnUse(  int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual bool  OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck = true );

public:
	ioEtcItemCardMatchingPremiumTicket();
	virtual ~ioEtcItemCardMatchingPremiumTicket();
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemDiceGame : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_DICE_GAME_ITEM; }
	virtual void  OnUse(  int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual bool  OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck = true );

public:
	ioEtcItemDiceGame();
	virtual ~ioEtcItemDiceGame();
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemDiceGameDice : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_DICE_GAME_DICE; }
	virtual void  OnUse(  int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual bool  OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck = true );

public:
	ioEtcItemDiceGameDice();
	virtual ~ioEtcItemDiceGameDice();
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemDiceGameRewardChange : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_DICE_GAME_REWARD_CHANGE; }
	virtual void  OnUse(  int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual bool  OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck = true );

public:
	ioEtcItemDiceGameRewardChange();
	virtual ~ioEtcItemDiceGameRewardChange();
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemDiceGameBoardChange : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_DICE_GAME_BOARD_CHANGE; }
	virtual void  OnUse(  int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual bool  OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck = true );

public:
	ioEtcItemDiceGameBoardChange();
	virtual ~ioEtcItemDiceGameBoardChange();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

enum MultiSuperGashponUseType;
class ioEtcItemMultiSuperGashapon : public ioEtcItem
{
protected:
	MultiSuperGashponUseType m_MultiSuperGashponUseType;

	//NewSuperGashaponCategoryMap m_NewSuperGashaponMap;

/*public:
	void LoadData( DWORD dwEtcItemCode, DWORD dwCategoryIdx, DWORD dwPackageIdx, NewSuperGashaponElement& element );*/

public:
	virtual DWORD GetType() const { return m_dwType; }
	virtual void  OnUse(  int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage );
	virtual void  OnBuy();
	virtual bool  OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck = true );

public:
	inline void SetUseType( MultiSuperGashponUseType Type ){ m_MultiSuperGashponUseType = Type; }

public:
	ioEtcItemMultiSuperGashapon();
	virtual ~ioEtcItemMultiSuperGashapon();
};