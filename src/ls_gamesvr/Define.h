#ifndef ___DEFINE_H__
#define ___DEFINE_H__
#include "../include/cSingleton.h"
#include "Network/GameSvrUDPModule.h"
#include "Network/GameSvrUDPNode.h"
#include "../../src/iocpSocketDLL/SocketModules/PacketQueue.h"
#include <iterator>
#include "IPBlocker/IPBlockerManager.h"
#include "Filter/WordFilterManager.h"
#include "Languages/ioLanguages.h"

#define EXCAVATION_EX_BYBCKIM				// 2018-01-15 by bckim, 탐사 확장	
#define EXCAVATION_EX_BYBCKIM_DEBUG			// 2018-01-15 by bckim, 탐사 확장	

#define FLAG_MODE_BY_BCKIM			// 2018-03-15 by bckim, 깃발모드 추가
#define FLAG_MODE_BY_BCKIM_DEBUG	// 2018-03-15 by bckim, 깃발모드 추가
 
#define CARD_MATCHING_BY_BCKIM			// 2018-06-18 by bckim, 카드 짝 마추기 이벤트 추가
#define CARD_MATCHING_BY_BCKIM_DEBUG	// 2018-06-18 by bckim, 카드 짝 마추기 이벤트 추가 _DEBUG

#define __OHTG_POWERDOWNMMANAGER__
#define __OHTG_LOGIN_IP_CHECK__
#define __OHTG_PRACTICE_MACRO_ADD__
#define NEXON_NEW_CASH_URL
#define __OHTG_BATTLEROOM_MONSTER_COIN_CHECK__
#define __OHTG_BATTLEROOM_MODE_CHECK__
#define ARENA_MODE_BY_BCKIM			// 2018-07-25 by bckim, 아레나 모드 추가
#define ARENA_MODE_BY_BCKIM_DEBUG	// 2018-07-25 by bckim, 아레나 모드 추가

#define DICE_GAME_BY_BCKIM			// 2018-08-30 by bckim, 주사위 이벤트 추가
#define DICE_GAME_BY_BCKIM_DEBUG	// 2018-08-30 by bckim, 주사위 이벤트 추가

#define USER_RECONFIRM_IN_SERVER_BY_BCKIM		// 2018-11-08 by bckim, 개발자 ID 검증 추가
#define USER_RECONFIRM_IN_SERVER_BY_BCKIM_DEBUG	// 2018-11-08 by bckim, 개발자 ID 검증 추가 _DEBUG

#define BATTLE_MODE_BY_BCKIM		// 2019-02-14 by bckim, 배틀 모드 추가
#define BATTLE_MODE_BY_BCKIM_DEBUG	// 2019-02-14 by bckim, 배틀 모드 추가

#define FARMING_MODE_BY_BCKIM		// 2019-07-03 by bckim, 파밍모드 추가
//#define FARMING_MODE_BY_BCKIM_DEBUG	// 2019-07-03 by bckim, 파밍모드 추가


// 2020-02-17 by bckim, 낚시 시스템 확장
#define FISHING_SYSTEM_EX_BY_BCKIM		// 2020-02-17 by bckim, 낚시 시스템 확장
#define FISHING_SYSTEM_EX_BY_BCKIM_DEBUG	// 2020-02-17 by bckim, 낚시 시스템 확장
// End. 2020-02-17 by bckim, 낚시 시스템 확장

// 2020-03-26 by bckim, 가차 선택 제한
#define GASHAPON_PRESENT_LIMITED_QUANTITY_BY_BCKIM			// 2020-03-26 by bckim, 가차 선택 제한
#define GASHAPON_PRESENT_LIMITED_QUANTITY_BY_BCKIM_DEBUG	// 2020-03-26 by bckim, 가차 선택 제한
// End. 2020-03-26 by bckim, 가차 선택 제한

// 2020-05-07 by bckim, 넥슨 채널링 구매 불가 처리
#define NEXON_CHANNELING_GET_CASH_ZERO_BY_BCKIM				// 2020-05-07 by bckim, 넥슨 채널링 구매 불가 처리
#define NEXON_CHANNELING_GET_CASH_ZERO_BY_BCKIM_DEBUG		// 2020-05-07 by bckim, 넥슨 채널링 구매 불가 처리
// End. 2020-05-07 by bckim, 넥슨 채널링 구매 불가 처리

// 2020-06-05 by bckim, 하드시리얼 유저 제재
//#define HDD_SERIALNUM_RESTRICTION_BY_BCKIM				// 2020-06-05 by bckim, 하드시리얼 유저 제재
//#define HDD_SERIALNUM_RESTRICTION_BY_BCKIM_DEBUG		// 2020-06-05 by bckim, 하드시리얼 유저 제재
// End. // 2020-06-05 by bckim, 하드시리얼 유저 제재

// 2020-12-01 by yhSeo, 위메이드 채널링 구매 불가 처리
//#define WEMADE_CHANNELING_GET_CASH_ZERO_BY_YHSEO				// 2020-12-01 by yhSeo, 위메이드 채널링 구매 불가 처리
//#define WEMADE_CHANNELING_BLOCK_USER_BY_YHSEO					// 2020-12-01 by yhSeo, 위메이드 채널링 유저 접속 불가 처리

#define OHTG_PRESENT_200_CHANGE
#define OHTG_USER_BLOCK_DISCONNECT
#define __OHTG_PRESENT_SENDID_INI__
//#define __OHTG_BILLING_TAIWAN_HAPPYTUK__
//#define __OHTG_LOCAL_PHILIPPINE__
//#define __OHTG_LOCAL_BRAZIL__
#define OHTG_NEW_RANROM_BOX_20201109

class ioBroadCastRelayModule;
#define STR_IP_MAX 64
#define ENC_LOGIN_KEY_NUM       30
#define LOGIN_KEY_PLUS_ONE      16
#define ENC_ID_NUM_PLUS_ONE     25

#ifdef THAILAND_LONG_ID
#define ID_NUMBER       40
#define ID_NUM_PLUS_ONE 41
#else
#define ID_NUMBER       20
#define ID_NUM_PLUS_ONE 21
#endif

#define PW_NUMBER       12
#define PW_NUM_PLUS_ONE 13
#define PW_ENCRYPT_NUMBER	24
#define PW_ENCRYPT_PLUS_ONE 25

#define MAX_RIGHT_SLOT_SIZE 20	//아이템 도감 별 최대 인덱스 사이즈

#define MAX_BATTLE_OBSERVER 4
#define MAX_PLAYER			16
#define MAX_PLAZA_PLAYER	32
#define SET_ITEM_CODE   700000  // 세트 아이템 인식 코드. 

#define MAX_JOIN_CHANNEL 2

#define KINDRED_HUMAN	1
#define KINDRED_ELF		2
#define KINDRED_DWARF	3

#define	EQUIP_UNKNOWN	1000
#define EQUIP_WEAPON    0
#define EQUIP_ARMOR     1
#define EQUIP_HELM      2
#define EQUIP_CLOAK     3
#define EQUIP_OBJECT	4
#define EQUIP_WEAR      5
#define EQUIP_RING      6
#define EQUIP_NECKLACE  7
#define EQUIP_BRACELET  8
#define MAX_EQUIP_SLOT	9		//Character EquipSlot의 최대사이즈

#define GUILD_NAME_NUMBER			20
#define GUILD_NAME_NUM_PLUS_ONE		21
#define GUILD_POS_NUMBER			20
#define GUILD_POS_NUM_PLUS_ONE		21
#define GUILD_TITLE_NUMBER			110
#define GUILD_TITLE_NUMBER_PLUS_ONE 111
#define GUILD_CREATE_ENTRY_USER     8
#define GUILD_MAX_ENTRY_DELAY_USER  16
#define GUILD_MAX_ENTRY_USER		32

#define TOURNAMENT_TITLE_NUM_PLUS_ONE              21
#define TOURNAMENT_CAMP_NAME_NUM_PLUS_ONE          21
#define TOURNAMENT_TEAM_NAME_NUM_PLUS_ONE          21
#define TOURNAMENT_TEAM_MAX_LOAD                   10
#define TOURNAMENT_CHEER_MAX_LOAD                  10

#define CREATE_RESERVE_DELAY_TIME          10000       //전투 길드 파티 생성 예약 대기 시간

#define US_TUTORIAL_CLEAR      -1

#define MAX_CHAR_DBITEM_SLOT   4	//InventorySlot의 최대사이즈
#define MAX_CHAR_COSTUME_SLOT	4	//costume max 사이즈
#define MAX_CHAR_ACCESSORY_SLOT	3	//costume max 사이즈

#define COSTUME_SUBTYPE_DELIMITER	100000
#define ACCESSORY_SUBTYPE_DELIMITER	1000000
#define IP_NUM_PLUS_ONE 16

#define USER_GUID_NUM_PLUS_ONE 32

#define CHANNELING_USER_ID_NUM_PLUS_ONE 200
#define CHANNELING_USER_NO_NUM_PLUS_ONE 21

#ifdef BATTLE_MODE_BY_BCKIM			
#define MAX_CONTROL_KEYS_PLUS_ONE       256		// 2019-06-28 by bckim, 배틀 모드 유저 단축키 추가 // DB table & proc 수정됨.  
#else
#define MAX_CONTROL_KEYS_PLUS_ONE       201
#endif

#define USER_BIRTH_DATE_PLUS_ONE        7

#define CHANNELING_KEY_VALUE_PLUS_ONE	101

#define MAX_INT_VALUE					2147483647

// 로그 레벨 - 
#define LOG_TEST_LEVEL                  -1

#ifdef _DEBUG
#define LOG_SHUFFLE						0
#else
#define LOG_SHUFFLE						-1
#endif

#define LOG_DEBUG_LEVEL                 0
#define LOG_RELEASE_LEVEL               0

#define COMPARE(x,min,max) (((x)>=(min))&&((x)<(max)))
#define SAFEDELETE(x)		if(x != NULL) { delete x; x = NULL; }
#define SAFEDELETEARRAY(x)	if(x != NULL) { delete [] x; x = NULL; }

const int G_MAXDELAY_REQUESTUSERDATA		=	300;
const int G_MAXDELAY_CHECK_TITLE_PREMIUM    =	3000;
const int PACKET_QUERY_DELAY_TIME		    =	30000;

const int DB_COSTUME_SELECT_COUNT			=	500;
const int DB_EXTRAITEM_SELECT_COUNT			=	50;
const int DB_DECO_SELECT_COUNT				=	100;
const int DB_ACCESSORY_SELECT_COUNT			=	500;

const int MAX_SELECT_EXTRA_ITEM_CODE		=	10;

const int R_SOLDIER_START_NUM				=	501;
const int R_SOLDIER_END_NUM					=	600;

const int GUILD_MAP_ARRAY					=	64;
const int GUILD_MAP_XZ_ARRAY				=	GUILD_MAP_ARRAY * GUILD_MAP_ARRAY;
const int GUILD_MAP_Y_ARRAY					=	20;

const int DB_BUILT_BLOCK_ITEM_SELECT_COUNT	= 500;
const int DB_GUILD_IN_SELCET_COUNT			= 7000;

const int GUILD_ROOM_ITEM_DELIMITER			= 1000000;

const int HOME_MAP_ARRAY					=	32;
const int HOME_MAP_XZ_ARRAY					=	HOME_MAP_ARRAY * HOME_MAP_ARRAY;
const int HOME_MAP_Y_ARRAY					=	8;

const int HOME_MODE_ITEM_DELIMITER			= 1000000;

const int GFRIEND_SOLDIER_START_NUM			= 807;
const int GFRIEND_SOLDIER_END_NUM			= 938;

const int DB_SPIRIT_SELECT_COUNT	= 200;
const int G_MAXDELAY_BATTLE = 300;
//수련장

const int PRACTICE_INIT_HOURTIME = 4;
const int PRACTICE_DIFF_TIME = 5000;

const int TIMESTRING = 12;

#define FREEDAY_EVENT_CODE			(0x02000000)  // jal : 집에서도 피씨방처럼

enum GuildRoomItemType
{
	GRT_NONE	= 0,
	GRT_BLOCK	= 1,
	GRT_TILE	= 2,
	GRT_FISHERY	= 3,
	GRT_END		= 4,
};

enum HomeModeItemType
{
	HMT_NONE	= 0,
	HMT_BLOCK	= 1,
	HMT_TILE	= 2,
	HMT_END		= 3,
};

enum BlockPropertyType
{
	BPT_NONE	= 0,
	BPT_GUILD	= 1,
	BPT_HOME	= 2,
};

struct Vector3
{
	float x,y,z;

	Vector3(){}
	Vector3( float _x, float _y, float _z )
	{
		x = _x;
		y = _y;
		z = _z;
	}
};

struct Quaternion
{
	float x, y, z, w;
	
	Quaternion()
	{
		x = 0;
		y = 0;
		z = 0;
		w = 1;
	}

	Quaternion( float _x, float _y, float _z, float _w )
	{
		x = _x;
		y = _y;
		z = _z;
		w = _w;
	}
};

typedef std::vector< Vector3 > Vector3Vec;
typedef std::deque< Vector3 > Vector3Deq;

enum TeamType
{
	TEAM_NONE,
	TEAM_RED,
	TEAM_BLUE,
	TEAM_PRIVATE_1,
	TEAM_PRIVATE_2,
	TEAM_PRIVATE_3,
	TEAM_PRIVATE_4,
	TEAM_PRIVATE_5,
	TEAM_PRIVATE_6,
	TEAM_PRIVATE_7,
	TEAM_PRIVATE_8,
	TEAM_PRIVATE_9,
	TEAM_PRIVATE_10,
	TEAM_PRIVATE_11,
	TEAM_PRIVATE_12,
	TEAM_PRIVATE_13,
	TEAM_PRIVATE_14,
	TEAM_PRIVATE_15,
	TEAM_PRIVATE_16,
	TEAM_PRIVATE_17,
	TEAM_PRIVATE_18,
	TEAM_PRIVATE_19,
	TEAM_PRIVATE_20,
	TEAM_PRIVATE_21,
	TEAM_PRIVATE_22,
	TEAM_PRIVATE_23,
	TEAM_PRIVATE_24,
	TEAM_PRIVATE_25,
	TEAM_PRIVATE_26,
	TEAM_PRIVATE_27,
	TEAM_PRIVATE_28,
	TEAM_PRIVATE_29,
	TEAM_PRIVATE_30,
	TEAM_PRIVATE_31,
	TEAM_PRIVATE_32,
	TEAM_PRIVATE_33,
	TEAM_PRIVATE_34,
	TEAM_PRIVATE_35,
	TEAM_PRIVATE_36,
	TEAM_PRIVATE_37,
	TEAM_PRIVATE_38,
	TEAM_PRIVATE_39,
	TEAM_PRIVATE_40,
	TEAM_PRIVATE_41,
	TEAM_PRIVATE_42,
	TEAM_PRIVATE_43,
	TEAM_PRIVATE_44,
	TEAM_PRIVATE_45,
	TEAM_PRIVATE_46,
	TEAM_PRIVATE_47,
	TEAM_PRIVATE_48,
	TEAM_PRIVATE_49,
	TEAM_PRIVATE_50,
	TEAM_PRIVATE_51,
	TEAM_PRIVATE_52,
	TEAM_PRIVATE_53,
	TEAM_PRIVATE_54,
	TEAM_PRIVATE_55,
	TEAM_PRIVATE_56,
	TEAM_PRIVATE_57,
	TEAM_PRIVATE_58,
	TEAM_PRIVATE_59,
	TEAM_PRIVATE_60,
	TEAM_PRIVATE_61,
	TEAM_PRIVATE_62,
	TEAM_PRIVATE_63,
	TEAM_PRIVATE_64,
	TEAM_PRIVATE_65,
	TEAM_PRIVATE_66,
	TEAM_PRIVATE_67,
	TEAM_PRIVATE_68,
	TEAM_PRIVATE_69,
	TEAM_PRIVATE_70,
	TEAM_PRIVATE_71,
	TEAM_PRIVATE_72,
	TEAM_PRIVATE_73,
	TEAM_PRIVATE_74,
	TEAM_PRIVATE_75,
	TEAM_PRIVATE_76,
	TEAM_PRIVATE_77,
	TEAM_PRIVATE_78,
	TEAM_PRIVATE_79,
	TEAM_PRIVATE_80,
	TEAM_PRIVATE_81,
	TEAM_PRIVATE_82,
	TEAM_PRIVATE_83,
	TEAM_PRIVATE_84,
	TEAM_PRIVATE_85,
	TEAM_PRIVATE_86,
	TEAM_PRIVATE_87,
	TEAM_PRIVATE_88,
	TEAM_PRIVATE_89,
	TEAM_PRIVATE_90,
	TEAM_PRIVATE_91,
	TEAM_PRIVATE_92,
	TEAM_PRIVATE_93,
	TEAM_PRIVATE_94,
	TEAM_PRIVATE_95,
	TEAM_PRIVATE_96,
	TEAM_PRIVATE_97,
	TEAM_PRIVATE_98,
	TEAM_PRIVATE_99,
	TEAM_PRIVATE_100,
	TEAM_PRIVATE_101,
	TEAM_PRIVATE_102,
	TEAM_PRIVATE_103,
	TEAM_PRIVATE_104,
	TEAM_PRIVATE_105,
	TEAM_PRIVATE_106,
	TEAM_PRIVATE_107,
	TEAM_PRIVATE_108,
	TEAM_PRIVATE_109,
	TEAM_PRIVATE_110,
	TEAM_PRIVATE_111,
	TEAM_PRIVATE_112,
	TEAM_PRIVATE_113,
	TEAM_PRIVATE_114,
	TEAM_PRIVATE_115,
	TEAM_PRIVATE_116,
	TEAM_PRIVATE_117,
	TEAM_PRIVATE_118,
	TEAM_PRIVATE_119,
	TEAM_PRIVATE_120,
	TEAM_PRIVATE_121,
	TEAM_PRIVATE_122,
	TEAM_PRIVATE_123,
	TEAM_PRIVATE_124,
	TEAM_PRIVATE_125,
	TEAM_PRIVATE_126,
	TEAM_PRIVATE_127,
	TEAM_PRIVATE_128,
	TEAM_PRIVATE_129,
	TEAM_PRIVATE_130,
	TEAM_PRIVATE_131,
	TEAM_PRIVATE_132,
	TEAM_PRIVATE_133,
	TEAM_PRIVATE_134,
	TEAM_PRIVATE_135,
	TEAM_PRIVATE_136,
	TEAM_PRIVATE_137,
	TEAM_PRIVATE_138,
	TEAM_PRIVATE_139,
	TEAM_PRIVATE_140,
	TEAM_PRIVATE_141,
	TEAM_PRIVATE_142,
	TEAM_PRIVATE_143,
	TEAM_PRIVATE_144,
	TEAM_PRIVATE_145,
	TEAM_PRIVATE_146,
	TEAM_PRIVATE_147,
	TEAM_PRIVATE_148,
	TEAM_PRIVATE_149,
	TEAM_PRIVATE_150,
	TEAM_PRIVATE_151,
	TEAM_PRIVATE_152,
	TEAM_PRIVATE_153,
	TEAM_PRIVATE_154,
	TEAM_PRIVATE_155,
	TEAM_PRIVATE_156,
	TEAM_PRIVATE_157,
	TEAM_PRIVATE_158,
	TEAM_PRIVATE_159,
	TEAM_PRIVATE_160,
	TEAM_PRIVATE_161,
	TEAM_PRIVATE_162,
	TEAM_PRIVATE_163,
	TEAM_PRIVATE_164,
	TEAM_PRIVATE_165,
	TEAM_PRIVATE_166,
	TEAM_PRIVATE_167,
	TEAM_PRIVATE_168,
	TEAM_PRIVATE_169,
	TEAM_PRIVATE_170,
	TEAM_PRIVATE_171,
	TEAM_PRIVATE_172,
	TEAM_PRIVATE_173,
	TEAM_PRIVATE_174,
	TEAM_PRIVATE_175,
	TEAM_PRIVATE_176,
	TEAM_PRIVATE_177,
	TEAM_PRIVATE_178,
	TEAM_PRIVATE_179,
	TEAM_PRIVATE_180,
	TEAM_PRIVATE_181,
	TEAM_PRIVATE_182,
	TEAM_PRIVATE_183,
	TEAM_PRIVATE_184,
	TEAM_PRIVATE_185,
	TEAM_PRIVATE_186,
	TEAM_PRIVATE_187,
	TEAM_PRIVATE_188,
	TEAM_PRIVATE_189,
	TEAM_PRIVATE_190,
};

enum CampType
{
	CAMP_NONE,
	CAMP_BLUE,
	CAMP_RED,
};

// 2019-02-14 by bckim, 배틀 모드 추가
#define BATTLE_ORDER_RANDOM  0
#define BATTLE_ORDER_FIRST   1
#define BATTLE_ORDER_SECOND  2
#define BATTLE_ORDER_THIRD   3
// End. 2019-02-14 by bckim, 배틀 모드 추가


enum WinTeamType
{
	WTT_NONE,
	WTT_RED_TEAM,
	WTT_BLUE_TEAM,
	WTT_DRAW,
	WTT_VICTORY_RED_TEAM,
	WTT_VICTORY_BLUE_TEAM
};

enum ModeType
{
	MT_NONE				= 0,
	MT_SYMBOL			= 1,
	MT_CATCH			= 2,
	MT_KING				= 3,
	MT_TRAINING			= 4,
	MT_SURVIVAL			= 5,
	MT_TEAM_SURVIVAL	= 6,
	MT_BOSS				= 7,
	MT_MONSTER_SURVIVAL = 8,
	MT_FOOTBALL			= 9,
	MT_HEROMATCH		= 10,
	MT_GANGSI			= 11,
	MT_DUNGEON_A    	= 12,
	MT_HEADQUARTERS     = 13,
	MT_CATCH_RUNNINGMAN = 14,
	MT_FIGHT_CLUB		= 15,
	MT_TOWER_DEFENSE	= 16,
	MT_DARK_XMAS		= 17,
	MT_FIRE_TEMPLE		= 18,
	MT_DOBULE_CROWN		= 19,
	MT_SHUFFLE_BONUS	= 20,
	MT_FACTORY			= 21,
	MT_TEAM_SURVIVAL_AI = 22,
	MT_HOUSE			= 23,
	MT_MYROOM			= 24,		
	MT_UNDERWEAR		= 25,
	MT_CBT				= 26,
	MT_RAID				= 27,
	MT_SUCCESSION		= 28,
	MT_PRACTICE			= 29,
	MT_FLAG				= 30,		// 2018-03-15 by bckim, 깃발모드 추가
	MT_ARENA			= 31,		// 2018-07-25 by bckim, 아레나 모드 추가
	MT_BATTLE			= 32,		// 2019-02-14 by bckim, 배틀 모드 추가
	MT_FARMING			= 33,		// 2019-07-03 by bckim, 파밍모드 추가 
	MAX_MODE_TYPE
};

enum RoomStyle
{
	RSTYLE_NONE	= 0,
	RSTYLE_BATTLEROOM,	//전투방
	RSTYLE_PLAZA,		//광장
	RSTYLE_LADDERBATTLE,//길드전
	RSTYLE_HEADQUARTERS,//본부
	RSTYLE_SHUFFLEROOM,	//셔플전투방
	RSTYLE_MATCHROOM,	//일대일모드
	RSTYLE_PRACTICE,
};

enum PlazaType
{
	PT_NONE      = 0,
	PT_BATTLE	 = 1,
	PT_COMMUNITY = 2,
	PT_GUILD     = 3,
};

// User::OnTrail / DBClient::OnInsertTrail 함수에서 사용
// Client ioMannerTrialChatManager::TrialType 존재
enum TrialType
{
	TT_NONE         = 0,
	TT_NORMAL_CHAT  = 1, // 방
	TT_BATTLE_CHAT  = 2, // 전투방 
	TT_CHANNEL_CHAT = 3,
	TT_MEMO         = 4,
	TT_GUILD_CHAT   = 5,
};

// DB에 저장되는 값으로 순차적으로 증가하지 않는다.
enum ChannelingType
{
	CNT_NONE        =   -1,
	CNT_WEMADEBUY   =    0,
	CNT_STEAM       =    1,
	CNT_MGAME       =   300,
	CNT_DAUM        =   400,
	CNT_NAVER       =   600,
	CNT_TOONILAND   =   700,
	CNT_NEXON		=	800,
	CNT_HANGAME		=	900,
	CNT_VALOFE		=	1000,
	CNT_HAPPYTUK	=	1100,
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

// 청약철회
enum
{
	SUBSCRIPTION_NONE	= 0,	// 청약철회 불가
	SUBSCRIPTION_TYPE1	= 1,	// 청약철회 가능
};

//유저 전투방 입장 경로에 따른 처리
enum RoomEntryType
{
	RET_PLAZA_TO_ROOM						= 0,
	RET_MOVE_PLAZA_TO_ROOM					= 1,
	RET_ROOM_TO_ROOM						= 2,
	RET_MOVE_ROOM_TO_ROOM					= 3,
};

//용병 , 아이템 파워업
enum PowerUpTargetType
{
	PUTT_NONE = 0,
	PUTT_CHAR = 1,
	PUTT_ITEM = 2,
	PDTT_CHAR = 3,
	PDTT_ITEM = 4,
	PUTT_MAX  = 5,
};

enum PowerUpGradeType
{
	PUGT_NONE = 0,

	PUGT_CHAR_GRADE1 = 1,
	PUGT_CHAR_GRADE2 = 2,
	PUGT_CHAR_GRADE3 = 3,
	PUGT_CHAR_GRADE4 = 4,
	PUGT_CHAR_GRADE5 = 5,

	PUGT_ITEM_GRADE_BASE = 10000,

	PUGT_ITEM_GRADE1 = 50000,
	PUGT_ITEM_GRADE2 = 60000,
	PUGT_ITEM_GRADE3 = 70000,
	PUGT_ITEM_GRADE4 = 80000,
	PUGT_ITEM_GRADE5 = 90000,
};

enum PowerUpRareItemGradeType
{
	PURIGT_NONE = 0,

	PURIGT_RARE_ITEM_GRADE_BASE = 1000,

	PURIGT_RARE_ITEM_GRADE1 = 5000,
	PURIGT_RARE_ITEM_GRADE2 = 6000,
	PURIGT_RARE_ITEM_GRADE3 = 7000,
	PURIGT_RARE_ITEM_GRADE4 = 8000,
	PURIGT_RARE_ITEM_GRADE5 = 9000,
	PURIGT_RARE_ITEM_GRADE_MAX = 10000,
};

//아이템별 구분 타입
enum ExtraItemExtendType
{
	EIET_DEFAULT, // 기본 디폴트 아이템
	EIET_EXTRA, // 고급 
	EIET_RARE, // 레어
	EIET_RARE_POWERUP,	// 강화 레어 아이템
	EIET_SPECIAL_EXTRA, // 특별 고급
	EIET_DEFAULT_POWERUP, // 강화 디폴트 아이템
	EIET_EXTREA_POWERUP,  // 강화 고급 아이템
};

//모든 아이템 타입
enum AllItemType
{
	AIT_SOLDIER			= 1,
	AIT_DECORATION,
	AIT_ETC_ITEM,
	AIT_PESO,
	AIT_EXTRAITEM,
	AIT_EXTRAITEM_BOX,
	AIT_RANDOM_DECO,
	AIT_EXP,
	AIT_MEDALITEM,
	AIT_ALCHEMIC_ITEM,
	AIT_PET,
	AIT_COSTUME,
};

//스킨 삭제 구분 타입
enum SkinDeleteType
{
	SDT_ALL		= 0,
	SDT_MALE	= 1,
	SDT_FEMALE	= 2,
	SDT_POWERUP	= 3,
};

enum MaterialCode
{
	ADDITIVE_CODE_NUM = 100001,
};

enum MaterialCompoundType
{
	MATERIAL_SPIRIT = 1,
	MATERIAL_PESO = 2,
};

enum AdditiveMissionTypes
{
	AMT_AWAKE		= 1,
	AMT_PET			= 2,
	AMT_COMPOUND	= 3,
	AMT_REINFORCE	= 4,
};

//길드 랭크
enum GuildRank
{
	GUILD_RANK_F	= 0,
	GUILD_RANK_E	= 1,
	GUILD_RANK_D	= 2,
	GUILD_RANK_C	= 3,
	GUILD_RANK_B	= 4,
	GUILD_RANK_A	= 5,
	GUILD_RANK_S	= 6,
};

enum SpecialShopBuyType
{
	SBT_GOODS_BUY		= 0,
	SBT_GOODS_PRESENT	= 1,
};

enum MoniterInertItemTypes
{
	MONITER_INSERT_ITEM_PUBLIC_ID = 1,
	MONITER_INSERT_ITEM_PRIVATE_ID = 2,
};

enum EtcBuyResultType
{
    EBRT_BUY,
    EBRT_PRESENT,
};

enum ItemSearchingType
{
	IST_ALL		= 0,
	IST_EQUIP	= 1,
	IST_RELEASE	= 2,
};

enum GuildBlockRetrieveORDeleteType
{
	GBT_RETRIEVE	= 0,
	GBT_DELETE		= 1,
	GBT_EXCEPTION	= 2,
};

enum GuildBlockConstructORMoveType
{
	GBT_CONSTRUCT			= 1,
	GBT_MOVE				= 2,
	GBT_DEFAULT_CONSTRUCT	= 3,
};

enum GuildBlockAddType
{
	GBA_ITEM_BUY		= 1,
	GBA_ITEM_PRESENT	= 2,
};

enum GuilInvenRequestType
{
	INVEN_OPEN		= 0,
	CONSTRUCT_MODE	= 1,
};

enum BlockModeType
{
	BMT_GUILD		= 0,
	BMT_PERSONAL		= 1,
};

enum CompensationType
{
	CT_MAINTENANCE	= 0,	//점검 보상.
};

enum UpdateTimeCashType
{
	UTCT_SUCCESS	= 0,
	UTCT_EXPIRE		= 1,
	UTCT_FAIL		= 2,
};

enum TitleUpdateType
{
	TUT_INSERT		= 0,
	TUT_UPDATE		= 1,
	TUT_PREMIUM		= 2,
	TUT_LEVELUP		= 3,
	TUT_INSERT_ETC	= 4,
	TUT_EQUIP		= 5,
	TUT_ALL_RELEASE	= 6,
	TUT_RELEASE		= 7,
};

enum SpecialSoldierType
{
	SST_RSOLDIER	= 0,
	SST_GFRIEND		= 1,
	SST_END			= 2,
};

enum BonusCashUpdateType
{
	BUT_CHANGE	= 1,
	BUT_END		= 0,
};

enum TradeType
{
	TRADE_NONE	= 0,
	TRADE_ALL	= 1,	// 메인서버가 처음 보내는 거래소 아이템 전체 리스트 패킷
	TRADE_ADD	= 2,	// 메인서버로 보내는 거래소 아이템 추가 패킷
	TRADE_DEL	= 3,	// 메인서버로 보내는 거래소 아이템 삭제 패킷
};
// 오크통 개선
enum OakBarrelType
{
	OAK_BARREL_STEP = 11,	// 오크통 진행 단계
	OAK_BARREL_HOLE = 12,	// 오크통 구멍 개수
};

enum OakBarrelHole
{
	OAK_HOLE_UNUSED = 0,
	OAK_HOLE_USED	= 1,
};

enum OakBarrelUpdateType
{
	OAK_BARREL_FAIL		= 0,	// 실패 시에 대한 업데이트
	OAK_BARREL_SUCCESS	= 1,	// 성공 시에 대한 업데이트
	OAK_BARREL_SWORD	= 2,	// 칼 수량 업데이트
	OAK_BARREL_SAVE		= 3,	// 서버 이동 등에 대한 현재 상황 저장 시에 대한 업데이트
	OAK_BARREL_REWARD	= 4,	// 오크통 보상 시에 대한 업데이트
};

// 오크통 예외 처리 사항
enum OakBarrelException
{
	OAK_BARREL_OK						= 0,
	OAK_BARREL_ERR_HOLE_INDEX			= 1,	// 구멍 인덱스 범위 초과된 값
	OAK_BARREL_ERR_HOLE_DUPLICATED		= 2,	// 구멍 인덱스 중복
	OAK_BARREL_ERR_REWARD_STEP			= 3,	// 보상 단계가 맞지 않음
	OAK_BARREL_ERR_LIMIT_SWORD			= 4,	// 일일 한도 수량을 넘어섬
};

// 2018-06-18 by bckim, 카드 짝 마추기 이벤트 추가
enum CardMatchingUpdateType			// DB 로그 저장 타입 
{
	CARD_MATCHING_NEW_MISSION			= 0,		// 새로운 미션 시작 
	CARD_MATCHING_IN_PROGRESS_UPDATE	= 1,		// 진행 중 업데이트 
	CARD_MATCHING_MISSION_REWARD		= 2,		// 미션 완료 보상
	CARD_MATCHING_NORMAL_REWARD			= 3,		// 일반 보상 	
};

enum Card_State		
{
	CARD_NONE				= 0,
	CARD_NOT_CHECKED		= 2,
	CARD_CONFIRMED			= 3,
	CARD_MATCHED_DONE		= 4,
};

enum Card_Type
{
	CARD_TYPE_ERROR			= 0,		// 실제 카드 타입에 저장되지 않음. 에러처리를 위해서만 사용 
	NORMAL_CARD_TYPE		= 1,		// 일반 카드  코드값 1~15 까지 
	SPECIAL_CARD_TYPE_BLUE	= 15,		// 블루 카드  코드값 15 
	SPECIAL_CARD_TYPE_RED	= 16,		// 레드 카드  코드값 16 
};

struct CardInfo
{
	Card_Type	CardType;		// 카드 타입 
	Card_State	nCardState;		// 카드 상태 
	int			nCardCode;		// 카드 내용(코드) 

	CardInfo()
	{
		CardType	= NORMAL_CARD_TYPE;
		nCardState	= CARD_NOT_CHECKED;
		nCardCode	= 0;
	}
};
// End. 2018-06-18 by bckim, 카드 짝 마추기 이벤트 추가

// 2018-08-30 by bckim, 주사위 이벤트 추가		DiceGameType

enum DiceGameTraceStepType
{
	NON_STEP = 0,
	ON_STEPED = 1,	
};

enum DiceGameType
{
	DICE_GAME_TRACE_DB = 6,
	DICE_GAME_REWARD_DB = 10,
	DICE_GAME_SLOT_COUNT = 96,
	DICE_GAME_SLOT_DATA_COUNT = 96,		// 90개가 넘으면 안됨. 16 * DICE_GAME_TRACE_DB = 90
	DICE_GAME_SLOT_REAL_COUNT = 87,		// 90개가 넘으면 안됨. 16 * DICE_GAME_TRACE_DB = 90
};
// End. 2018-08-30 by bckim, 주사위 이벤트 추가

struct UserRankInfo
{
	ioHashString szName;
	int iRank;

	UserRankInfo()
	{
		iRank = 0;
	}
};

struct DamageTable
{
	ioHashString szName;
	int iDamage;

	DamageTable()
	{
		iDamage = 0;
	}
};

struct ControlKeys
{
	char m_szControlKeys[MAX_CONTROL_KEYS_PLUS_ONE];

	bool IsRight()
	{
		if( strcmp( m_szControlKeys, "" ) == 0 )
			return false;

		for (int i = 0; i < MAX_CONTROL_KEYS_PLUS_ONE ; i++)
		{
			if( m_szControlKeys[i] == NULL )
				break;

			if ((!COMPARE(m_szControlKeys[i], 'A', 'Z'+1)) &&
				(!COMPARE(m_szControlKeys[i], 'a', 'z'+1)) &&
				(!COMPARE(m_szControlKeys[i], '0', '9'+1)) )
			{
				return false;
			}

		}

		return true;
	}

	void Clear()
	{
		ZeroMemory( m_szControlKeys, sizeof( m_szControlKeys ) );
	}

	ControlKeys()
	{
		Clear();
	}

};

struct IntOfTwo
{
	int value1;
	int value2;

	IntOfTwo()
	{
		value1	= 0;
		value2	= 0;
	}
};

struct IntOfStat
{
	int index;
	int itemCode;
	int stat1;
	int stat2;
	int stat3;
	int stat4;
	int stat5;
	int stat6;
	int stat7;
	int stat8;


	IntOfStat()
	{

		index	= 0;
		itemCode	= 0;
		stat1	= 0;
		stat2	= 0;
		stat3	= 0;
		stat4	= 0;
		stat5	= 0;
		stat6	= 0;
		stat7	= 0;
		stat8	= 0;
		
	}
};

enum ItemType
{
	IT_SOLDIER	= 1,
	IT_PESO		= 4,
};

typedef std::vector< DamageTable > DamageTableList;

#define MAX_MODE ( MAX_MODE_TYPE - 1 )

typedef std::vector<int> IntVec;
typedef std::vector<DWORD> DWORDVec;
typedef std::vector<float> FloatVec;
typedef std::vector<bool> BoolVec;
typedef std::vector<UserRankInfo> UserRankInfoList;
typedef std::vector<IntOfTwo> IntOfTwoVec;
typedef std::vector<IntOfStat> IntOfStatVec;

extern LONG __stdcall ExceptCallBack ( EXCEPTION_POINTERS * pExPtrs );
extern char *_i64toStr(__int64 i64Num);
/************************************************************************/
/* packetid(UDP 전용)                                                                     */
/************************************************************************/
enum UDPPACKETID
{
	UDP_INSERTDATAPACKET = 0x8001,
	UDP_REMOVEPACKET,
	UDP_SENDPACKET,
};

/************************************************************************/
/* Struct                                                                     */
/************************************************************************/
struct RelayHeader : public NodeData
{
	int m_packetId;
};

struct UDPPacket :RelayHeader
{
	sockaddr_in m_client_addr;
	int m_size;
	char* m_buffer;

};

//////////////////////////////////////////////////////////////////////////
#ifndef ANTIHACK
struct InsertData : RelayHeader
{
	DWORD		 m_dwRoomIndex;
	DWORD		 m_dwUserIndex;
	char         m_szPublicIP[STR_IP_MAX];
	int          m_iClientPort;
};

struct SendRelayInsertData : RelayHeader // 릴레이 서버에게 보낼때 쓰는 전용  
{
	int          m_modeType;
	DWORD		 m_dwRoomIndex;
	DWORD		 m_dwUserIndex;
	char         m_szPublicIP[STR_IP_MAX];
	int          m_iClientPort;
	char         m_szPublicID[ID_NUM_PLUS_ONE];
};
struct RemoveData :RelayHeader
{
	DWORD m_dwRoomIndex;
	DWORD m_dwUserIndex;
};

struct UserData
{
	DWORD		 m_dwUserIndex;
	char         m_szPublicIP[STR_IP_MAX];
	int          m_iClientPort;

};

struct RelayGroup			
{
	typedef std::vector<UserData> RelayGroups;
	DWORD   m_dwRoomIndex;
	RelayGroups m_RelayUserList;

public:
	void GetUserLists(RelayGroups& relayGroup) 
	{
		std::copy(m_RelayUserList.begin(),m_RelayUserList.end(),std::back_inserter(relayGroup));
	}
	RelayGroups* GetUsers() { return &m_RelayUserList; }

	void AddUser(const DWORD dwUserIndex, const int iPort, const char* szIP)
	{
		for(unsigned int i=0; i< m_RelayUserList.size() ; i++)
		{
			UserData& rkUser = m_RelayUserList.at(i);
			if( rkUser.m_dwUserIndex == dwUserIndex )
			{
				// 기존 유저 갱신
				rkUser.m_iClientPort= iPort;
				strcpy_s(rkUser.m_szPublicIP, szIP);
				return;
			}
		}

		UserData kUserData;
		kUserData.m_dwUserIndex = dwUserIndex;
		kUserData.m_iClientPort = iPort;
		strcpy_s(kUserData.m_szPublicIP, szIP);

		m_RelayUserList.push_back(kUserData);
	}
	void RemoveUser(const DWORD dwUserIndex)
	{
		for(unsigned int i=0; i< m_RelayUserList.size() ; i++)
		{
			UserData& rkUser = m_RelayUserList.at(i);
			if( rkUser.m_dwUserIndex == dwUserIndex )
			{
				m_RelayUserList.erase(m_RelayUserList.begin() + i);
				break;
			}
		}
	}
};
#else
//////////////////////////////////////////////////////////////////////////
struct InsertData : RelayHeader
{
	DWORD		 m_dwRoomIndex;
	DWORD		 m_dwUserIndex;
	char         m_szPublicIP[STR_IP_MAX];
	int          m_iClientPort;
	DWORD		 m_dwUserSeed;
	DWORD		 m_dwNPCSeed;
	int			 m_iCoolType;
	int			 m_iModeType;
	int			 m_iRoomStyle;
	int			 m_iTeamType;
};

struct UpdateRelayGroupWinCntData : RelayHeader
{
	DWORD	m_dwRoomIndex;
	int		m_iRedTeamWinCnt;
	int		m_iBueTeamWinCnt;
};

struct UpdateRelayGroupScoreData : RelayHeader
{
	DWORD	m_dwRoomIndex;
	int		m_iTeamType;
};

struct UpdateAntihackInfo : RelayHeader
{
	float fAntiErrorRate;
	int iAntiWaitTime;

	int iPenguinCount;
	int iKickCount;
	int iTimeDecrease;	

	int iSkillHackCount;
	int iSkillKickCount;
	int iSkillTimeDecrease;
	int iExceptSkillID[10];
};

struct UpdateRelayGroupSpData : RelayHeader
{
	DWORD	m_dwRoomIndex;
	DWORD	m_dwUserIndex;
};

struct UpdateDieStateData : RelayHeader
{
	DWORD m_dwRoomIndex;
	DWORD m_dwUserIndex;
	BOOL  m_bDieState;
};

struct UserData
{
	DWORD		 m_dwUserIndex;
	char         m_szPublicIP[STR_IP_MAX];
	int          m_iClientPort;
	DWORD		 m_dwUserSeed;
	DWORD		 m_dwNPCSeed;

	DWORD		 m_dwUserSeedOri;
	DWORD		 m_dwNPCSeedOri;

	int			 m_iTeamType;		// 레드 - 1 , 블루 - 2 

	std::vector<DWORD> m_vecRUDP;
	std::vector<DWORD> m_vecRUDPErase;
	std::vector<DWORD> m_vecRUDPTest;

	std::vector<DWORD> m_vecRUDPNPC;
	std::vector<DWORD> m_vecRUDPEraseNPC;
	std::vector<DWORD> m_vecRUDPTestNPC;

	//카운팅용
	DWORD		m_dwReqCount;			//요청갯수
	DWORD		m_dwAckCount;			//응답받은갯수
	DWORD		m_dwAckCountDupl;		//ack 두번이상 올경우 
	DWORD		m_dwErrCount;			//요청 하지 않았는데 올 경우
	DWORD		m_dwLossCount;			//실패 카운트
	DWORD		m_dwEraseCount;			//시드 지운 후에 들어온 경우

	//유저에게 직접 스킬 정보 해줌
	DWORD		m_dwSkillUseTime[4];	//장비4개
	DWORD		m_dwSkillUseTime2[4];	//장비4개
	std::vector<DWORD> m_dwVecMultiCountSkill[4];	// 스킬중에 갯수 모아서 조금씩 쓰는 스킬들.....

	//스킬쪽에서
	bool		m_bRecoverSkill;		// 회복스킬일 경우 가능한지 여부.
	DWORD		m_dwRecoverTime;
	//엑스트라스킬에서
	bool		m_bRecoverExtraSkill;
	DWORD		m_dwRecoverExtraTime;
	
	int			m_iRecoverSkillSize;
	int			m_iSlot[4];
	DWORD		m_dwLatedTime[4];

	void OnRecvoerSkill( DWORD dwRecoverTime )
	{
		//CheatLOG.PrintTimeAndLog( 0, "[skill] Test - 1 OnRecvoerSkill , Time(%u)", dwRecoverTime );

		m_bRecoverSkill = true;
		m_dwRecoverTime = dwRecoverTime;
		if( m_bRecoverExtraSkill )
		{
			UpdateRecoverSkill();
		}
	}
	void OnRecoverSkillExtraInfo( DWORD dwRecoverExtraTime, int nSize, int* iSlots, DWORD* dwLatedTime )
	{
		m_bRecoverExtraSkill = true;
		m_dwRecoverExtraTime = dwRecoverExtraTime;

		//CheatLOG.PrintTimeAndLog( 0, "[skill] Test - 2 OnRecoverSkillExtraInfo, Time(%u)", dwRecoverExtraTime );

		m_iRecoverSkillSize = nSize;
		if( m_iRecoverSkillSize > 4 )
		{
			//CheatLOG.PrintTimeAndLog( 0, "[skill] Skill Extra Info Size(%d) Error", m_iRecoverSkillSize );
			m_iRecoverSkillSize = 0;
		}

		// 스킬회복에 관련된 스킬을 연속으로 사용할 경우.. a,b 페어중에 a 만 손실될경우에 대한 체크도 추가 해야 할까?
		for( int i = 0; i < m_iRecoverSkillSize; ++i )
		{
			m_iSlot[i] = iSlots[i];
			m_dwLatedTime[i] = dwLatedTime[i];
		}
		
		if( m_bRecoverSkill )
		{
			UpdateRecoverSkill();
		}
	}
	void UpdateRecoverSkill()
	{
		// 현재 시간을 호출하지 않고 두 패킷의 차이 시간만으로 해줌
		DWORD dwDiffTime = m_dwRecoverTime > m_dwRecoverExtraTime ? m_dwRecoverTime - m_dwRecoverExtraTime : m_dwRecoverExtraTime - m_dwRecoverTime;
		//차이가 너무 많이 나면 이전 정보인데..		
		// 테스트 해보니 5초 뒤에도 들어오는 애들이 있음
		if( dwDiffTime < 10000 ) 
		{
			//CheatLOG.PrintTimeAndLog( 0, "[skill] Test - 3 UpdateRecoverSkill OK, Time(%u)", dwDiffTime );
			for( int i = 0; i < m_iRecoverSkillSize; ++i )
			{
				int iSlot = m_iSlot[i];
				if( m_dwSkillUseTime[i] > m_dwLatedTime[i] )
					m_dwSkillUseTime[i] -= m_dwLatedTime[i];
				else
					m_dwSkillUseTime[i] = 0;

				if( m_dwSkillUseTime2[i] > m_dwLatedTime[i] )
					m_dwSkillUseTime2[i] -= m_dwLatedTime[i];
				else
					m_dwSkillUseTime2[i] = 0;

				//CheatLOG.PrintTimeAndLog( 0, "[skill] Test - 4 UpdateRecoverSkill OK, slot(%d), time(%u)", iSlot, m_dwLatedTime[i] );
			}
			m_bRecoverSkill = m_bRecoverExtraSkill = false;
		}
		else
		{
			// 이 경우 둘중 하나가 이전 정보이다. 냅둬도 될듯..
		}
	}

	DWORD		m_dwPenaltySkill;
	DWORD		m_dwPenaltyCount;
	DWORD		m_dwLastPenaltyTime;
	DWORD		m_dwLastSkillPenaltyTime;

	// 페널티 로그 정보 추가
	DWORD		m_dwAntiHitCount;
	DWORD		m_dwAntiHitCount2;
	DWORD		m_dwAntiSkillCount;
	DWORD		m_dwAntiSkillCount2;

	struct sAntiHackHitData
	{
		DWORD dwAttackerIndex;
		DWORD dwWeaponIndex;

		DWORD dwHitCount;
		DWORD dwWDCount;	// wounded, defense

		DWORD dwLastTime;
		sAntiHackHitData() : dwAttackerIndex(0), dwWeaponIndex(0), dwHitCount(0), dwWDCount(0){}
	};

	std::vector<sAntiHackHitData> m_vecAntiData;

	sAntiHackHitData& GetHitData( DWORD dwAttackerIndex, DWORD dwWeaponIndex )
	{
		int nSize = m_vecAntiData.size();
		for( int i = 0; i < nSize; ++i )
		{
			if( m_vecAntiData[i].dwAttackerIndex == dwAttackerIndex &&
				m_vecAntiData[i].dwWeaponIndex == dwWeaponIndex	)
				return m_vecAntiData[i];
		}
		//없으면

		sAntiHackHitData antiData;
		antiData.dwAttackerIndex = dwAttackerIndex;
		antiData.dwWeaponIndex = dwWeaponIndex;

		m_vecAntiData.push_back( antiData );
		nSize = m_vecAntiData.size();
		return m_vecAntiData[nSize-1];
	}

	void AddHitData( DWORD dwAttackerIndex, DWORD dwWeaponIndex/*, bool bLog */)
	{
		sAntiHackHitData& antiData = GetHitData( dwAttackerIndex, dwWeaponIndex );
		antiData.dwHitCount++;
		antiData.dwLastTime = TIMEGETTIME();
		
		/*if( bLog )
		{
			CheatLOG.PrintTimeAndLog( 0, "[ANTILOG] AddHitData Attacker hit me(%u >> %u), Weapon(%u), HitCount(%u), Time(%u)", dwAttackerIndex, m_dwUserIndex ,dwWeaponIndex, antiData.dwHitCount, antiData.dwLastTime );
		}*/
		}

	void AddWoundedData( DWORD dwAttackerIndex, DWORD dwWeaponIndex/*, bool bLog*/ )
	{
		sAntiHackHitData& antiData = GetHitData( dwAttackerIndex, dwWeaponIndex );
		antiData.dwWDCount++;
		antiData.dwLastTime = TIMEGETTIME();

		/*if( bLog )
		{
			CheatLOG.PrintTimeAndLog( 0, "[ANTILOG] AddWoundedData  me wounded Attacker(%u << %u), Weapon(%u), HitCount(%u), Time(%u)", m_dwUserIndex, dwAttackerIndex, dwWeaponIndex, antiData.dwHitCount, antiData.dwLastTime );
		}*/
	}

	//bullet작업
	struct sBulletData
	{
		DWORD dwItemCode;
		DWORD dwCurCount;
		DWORD dwMaxCount;
		DWORD dwReloadTime;
		std::vector<DWORD> vecWasteBullet;
		std::vector<DWORD> vecWasteTime;
		std::vector<DWORD> vecRecvTime;
		sBulletData() : dwItemCode(0), dwCurCount(0), dwMaxCount(0), dwReloadTime(0){ vecWasteBullet.reserve(7); vecWasteTime.reserve(7); vecRecvTime.reserve(7); };
		void WasteBullet( DWORD dwCount, DWORD dwWasteTime, DWORD dwRecvTime )
		{
			dwCurCount += dwCount;
			vecWasteBullet.push_back( dwCount );
			vecWasteTime.push_back( dwWasteTime );
			vecRecvTime.push_back( dwRecvTime );
		};
		void Clear()
		{
			dwCurCount = 0;
			vecWasteBullet.clear();
			vecWasteTime.clear();
			vecRecvTime.clear();
		};
	};

	std::map<DWORD,sBulletData> m_mapBulletDatas;
	sBulletData* GetBulletData( DWORD dwItemCode )
	{
		auto it = m_mapBulletDatas.find( dwItemCode );
		if( it != m_mapBulletDatas.end() )
		{
			sBulletData* pData = &(it->second);
			return pData;
		}
		return NULL;
	}

	sBulletData* InsertBulletData( DWORD dwItemCode, DWORD dwCount )
	{
		sBulletData sData;
		sData.dwItemCode = dwItemCode;
		sData.dwCurCount = 0;
		sData.dwMaxCount = dwCount;
		m_mapBulletDatas.insert( std::map<DWORD,sBulletData>::value_type(dwItemCode,sData) );
		return GetBulletData(dwItemCode);
	}

	void ClearBulletData() { m_mapBulletDatas.clear(); }

	//////////////////////////////////////////////////////////////////////////
	

	void		 Init()
	{
		m_vecRUDP.clear();
		m_vecRUDPErase.clear();
		m_vecRUDPTest.clear();
		m_vecRUDPNPC.clear();
		m_vecRUDPEraseNPC.clear();
		m_vecRUDPTestNPC.clear();

		m_vecAntiData.clear();

		m_dwReqCount = m_dwAckCount = m_dwAckCountDupl = m_dwErrCount = m_dwLossCount = m_dwEraseCount = 0;
		for( int i = 0; i < 4; ++i )
		{
			m_dwSkillUseTime[i] = 0;
			m_dwSkillUseTime2[i] = 0;
			m_dwLatedTime[i] = 0;
		}

		m_iRecoverSkillSize = 0;
		m_iTeamType = 0;
		m_dwPenaltySkill = m_dwPenaltyCount = m_dwLastPenaltyTime = m_dwLastSkillPenaltyTime = 0;
		m_bRecoverSkill = m_bRecoverExtraSkill = false;
		m_dwRecoverTime = m_dwRecoverExtraTime = 0;
		m_dwAntiHitCount = m_dwAntiSkillCount = m_dwAntiHitCount2 = m_dwAntiSkillCount2 = 0;

		m_mapBulletDatas.clear();
	}
	UserData()
	{
		m_vecAntiData.reserve( 100 );
	};

};
struct RelayGroup			
{
	typedef std::vector<UserData> RelayGroups;
	int		m_iCoolType;	//스킬 쿨타임용( 옵션에 따른 고정값 )
	int		m_iModeType;
	int		m_iRoomStyle;
	DWORD   m_dwRoomIndex;
	RelayGroups m_RelayUserList;

	int		GetModeType() { return m_iModeType; }
	int		GetModeStyle() { return m_iRoomStyle; }

	int		m_iWinCntRed;
	int		m_iWinCntBlue;

	int		m_iScoreRed;
	int		m_iScoreBlue;
	void	IncTeamScoreRed(){ m_iScoreRed++; }
	void	IncTeamScoreBlue(){ m_iScoreBlue++; }

	float	m_fTeamBalanceRed;		//팀인원
	float	m_fTeamBalanceBlue;

	float	m_fWinBalanceRed;		//팀스코어
	float	m_fWinBalanceBlue;

	float	m_fModeBalanceRed;		//모드별로..
	float	m_fModeBalanceBlue;

	float GetTeamBalanceRed(){ return m_fTeamBalanceRed; }
	float GetTeamBalanceBlue(){ return m_fTeamBalanceBlue; }

	float GetWinBalanceRed(){ return m_fWinBalanceRed; }
	float GetWinBalanceBlue(){ return m_fWinBalanceBlue; }

	float GetModeBalanceRed(){ return m_fModeBalanceRed; }
	float GetModeBalanceBlue(){ return m_fModeBalanceBlue; }


	bool m_bWriteLog;
	void SetWriteLog( bool bWrite ) { m_bWriteLog = bWrite; }
	bool IsWriteLog(){ return m_bWriteLog; }

	//anti hack 검증 시간부분은 얘가..



	struct sAntiHackCheckTimeData
	{
		DWORD dwUser1;
		DWORD dwUser2;
		DWORD dwCheckTime;
		sAntiHackCheckTimeData() : dwUser1(0), dwUser2(0), dwCheckTime(0) {}
	};

	std::vector<sAntiHackCheckTimeData> m_vecAntiHackTime;

	bool IsAntiHackCheckTime( DWORD dwUser1, DWORD dwUser2, DWORD dwCurTime )
	{
		DWORD dwFinduser1 = dwUser1 < dwUser2 ? dwUser1 : dwUser2;
		DWORD dwFinduser2 = dwUser1 > dwUser2 ? dwUser1 : dwUser2;

		int nSize = m_vecAntiHackTime.size();
		for( int i = 0; i < nSize; ++i )
		{
			if( m_vecAntiHackTime[i].dwUser1 == dwFinduser1 &&
				m_vecAntiHackTime[i].dwUser2 == dwFinduser2 )
			{

				if( (dwCurTime-m_vecAntiHackTime[i].dwCheckTime) > 1000 )
				{
					m_vecAntiHackTime[i].dwCheckTime = dwCurTime;
					return true;
				}

			}
		}

		//없으면
		sAntiHackCheckTimeData antiHackCheckTimeData;
		antiHackCheckTimeData.dwUser1 = dwFinduser1;
		antiHackCheckTimeData.dwUser2 = dwFinduser2;
		antiHackCheckTimeData.dwCheckTime = dwCurTime;

		m_vecAntiHackTime.push_back( antiHackCheckTimeData );
		
		return false;
	}


public:
	void InitData()
	{
		m_iCoolType = 4;
		m_iModeType = m_iRoomStyle = 0;
		m_dwRoomIndex = 0;
		m_iWinCntRed = m_iWinCntBlue = m_iScoreRed = m_iScoreBlue = 0;
		m_fTeamBalanceRed = m_fTeamBalanceBlue = m_fWinBalanceRed = m_fWinBalanceBlue = 1.f;
		m_fModeBalanceRed = m_fModeBalanceBlue = 1.f;
		m_RelayUserList.clear();

		m_vecAntiHackTime.clear();
	}

	void CalculateTeamBalance()
	{
		int nSize = m_RelayUserList.size();
		int BlueCnt = 0, RedCnt = 0;
		for( int i = 0; i < nSize; ++i )
		{
			if( m_RelayUserList[i].m_iTeamType == TEAM_RED )
				RedCnt++;
			else if( m_RelayUserList[i].m_iTeamType == TEAM_BLUE )
				BlueCnt++;
		}
		if( RedCnt !=0 && BlueCnt != 0 )
		{
			m_fTeamBalanceRed = TeamBalanceCalcForGauge( RedCnt, BlueCnt, m_iModeType, m_iModeType );
			m_fTeamBalanceBlue = TeamBalanceCalcForGauge( BlueCnt, RedCnt, m_iModeType, m_iModeType );
		}
	}

	void CalculateTeamWinCnt()
	{
		//auto func = []( int iOwnerTeamCnt, int iEnemyTeamCnt, int iModeType ) -> float
		//{
		//	if( iModeType == MT_SYMBOL || iModeType == MT_KING )
		//	{
		//		if( iEnemyTeamCnt-iOwnerTeamCnt <= 0 )
		//			return 1.f;
		//		return 1.2f;
		//	}
		//	int iGap = iEnemyTeamCnt-iOwnerTeamCnt;
		//	if( iGap <= 0 )
		//		return 1.f;
		//	else if ( iGap == 1 )
		//		return 1.1f;
		//	return 1.2f;
		//};

		m_fWinBalanceRed = CalculateWinBalance( m_iWinCntRed, m_iWinCntBlue, m_iModeType );
		m_fWinBalanceBlue = CalculateWinBalance( m_iWinCntBlue, m_iWinCntRed, m_iModeType );
	}

	float CalculateWinBalance(int iOwnerTeamCnt, int iEnemyTeamCnt, int iModeType)
	{
		if( iModeType == MT_SYMBOL || iModeType == MT_KING )
		{
			if( iEnemyTeamCnt-iOwnerTeamCnt <= 0 )
				return 1.f;
			return 1.2f;
		}
		int iGap = iEnemyTeamCnt-iOwnerTeamCnt;
		if( iGap <= 0 )
			return 1.f;
		else if ( iGap == 1 )
			return 1.1f;
		return 1.2f;
	}

	void CalculateTeamScore()
	{
		//팀서바랑 더블크라운!
		if( m_iModeType == MT_TEAM_SURVIVAL || m_iModeType == MT_DOBULE_CROWN )
		{
			int iTeamCntRed = 0, iTeamCntBlue = 0;
			for( DWORD i = 0; i < m_RelayUserList.size(); ++i )
			{
				if( m_RelayUserList[i].m_iTeamType == TEAM_RED )
					iTeamCntRed++;
				else if( m_RelayUserList[i].m_iTeamType == TEAM_BLUE )
					iTeamCntBlue++;
			}

			// 하드코딩!! 2.f 는 win_score_constant 의 상수값이다. 특정 모드 ini에 정의 되어 있음.(평생 안 바뀔듯..)
			float fScoreKillPointRed = iTeamCntRed * 2.5f * 100.f;
			float fScoreKillPointBlue = iTeamCntBlue * 2.5f * 100.f;

			//auto funcKillRate = []( float fKillPoint, float fScoreKillPoint ) -> float
			//{
			//	if( fScoreKillPoint <= 0 )
			//		return 0.f;
			//	if( fKillPoint > 0 )
			//	{
			//		float fRate = fKillPoint / fScoreKillPoint;
			//		return min( fRate , 1.f );
			//	}

			//	return 0.f;
			//};

			float fKillRateRed = CalculateKillRate( float(m_iScoreRed*100), fScoreKillPointRed );
			float fKillRateBlue = CalculateKillRate( float(m_iScoreBlue*100), fScoreKillPointBlue );

			//auto funcScoreRate = []( float fOwnerTeamRate, float fEnemyTeamRate ) -> float
			//{
			//	float fScoreGap = fEnemyTeamRate - fOwnerTeamRate;
			//	fScoreGap *= 100.f;

			//	float fResult;
			//	if( fScoreGap <= 0 )
			//		fResult = 1.f;
			//	else if( fScoreGap < 10.f )
			//		fResult = 1.1f;
			//	else
			//		fResult = 1.2f;

			//	return fResult;
			//};

			m_fModeBalanceRed = CalculateScoreRate( fKillRateRed, fKillRateBlue );
			m_fModeBalanceBlue = CalculateScoreRate( fKillRateBlue, fKillRateRed );
		}
		// 파이트 클럽
		else if( m_iModeType == MT_FIGHT_CLUB )
		{
			// 이건 나중에...

		}
	}

	float CalculateKillRate( float fKillPoint, float fScoreKillPoint )
	{
		if( fScoreKillPoint <= 0 )
			return 0.f;
		if( fKillPoint > 0 )
		{
			float fRate = fKillPoint / fScoreKillPoint;
			return min( fRate , 1.f );
		}

		return 0.f;
	}

	float CalculateScoreRate( float fOwnerTeamRate, float fEnemyTeamRate )
	{
		float fScoreGap = fEnemyTeamRate - fOwnerTeamRate;
		fScoreGap *= 100.f;

		float fResult;
		if( fScoreGap <= 0 )
			fResult = 1.f;
		else if( fScoreGap < 10.f )
			fResult = 1.1f;
		else
			fResult = 1.2f;

		return fResult;
	}

	// 	void GetUserLists(RelayGroups& relayGroup) 
	// 	{
	// 		std::copy(m_RelayUserList.begin(),m_RelayUserList.end(),std::back_inserter(relayGroup));
	// 	}
	RelayGroups* GetUsers() { return &m_RelayUserList; }

	void AddUser(const DWORD dwUserIndex, const int iPort, const char* szIP, DWORD dwUserSeed, DWORD dwNPCSeed, int iTeamType )
	{
		for(unsigned int i=0; i< m_RelayUserList.size() ; i++)
		{
			UserData& rkUser = m_RelayUserList.at(i);
			if( rkUser.m_dwUserIndex == dwUserIndex )
			{
					// 기존 유저 갱신 ( update 일 경우 rudp 정보 출력
					DWORD dwTotalCnt = rkUser.m_dwUserSeed - rkUser.m_dwUserSeedOri;
					dwTotalCnt += rkUser.m_dwNPCSeed - rkUser.m_dwNPCSeedOri;
					if( dwTotalCnt != 0 )
					{
// 					CheatLOG.PrintTimeAndLog( 0, "[ANTILOG] RudpInfo uid:%u, RoomIndex:%u, modeType:%d, total:%u, req:%u, ack:%u, dup:%u, err:%u, loss:%u, del:%u",
// 						dwUserIndex, m_dwRoomIndex, m_iModeType, dwTotalCnt,
// 						rkUser.m_dwReqCount, rkUser.m_dwAckCount, rkUser.m_dwAckCountDupl, rkUser.m_dwErrCount, rkUser.m_dwLossCount, rkUser.m_dwEraseCount );
					}

				/*if( IsWriteLog() )
					CheatLOG.PrintTimeAndLog( 0, "[ANTILOG] AddUser Update - Uidx(%u), Seed(%u/%u)", dwUserIndex, dwUserSeed, dwNPCSeed );*/

				rkUser.Init();
				rkUser.m_iClientPort = iPort;
				rkUser.m_dwUserSeed = dwUserSeed;
				rkUser.m_dwNPCSeed = dwNPCSeed;
				rkUser.m_dwUserSeedOri = dwUserSeed;
				rkUser.m_dwNPCSeedOri = dwNPCSeed;
				rkUser.m_iTeamType = iTeamType;

				strcpy_s(rkUser.m_szPublicIP, szIP);

				CalculateTeamBalance();
				return;
			}
		}

		UserData kUserData;
		kUserData.Init();

		kUserData.m_dwUserIndex = dwUserIndex;
		kUserData.m_iClientPort = iPort;
		kUserData.m_dwUserSeed = dwUserSeed;
		kUserData.m_dwNPCSeed = dwNPCSeed;
		kUserData.m_dwUserSeedOri = dwUserSeed;
		kUserData.m_dwNPCSeedOri = dwNPCSeed;
		kUserData.m_iTeamType = iTeamType;
		strcpy_s(kUserData.m_szPublicIP, szIP);		

		m_RelayUserList.push_back(kUserData);
		CalculateTeamBalance();

		/*if( IsWriteLog() )
		{
			CheatLOG.PrintTimeAndLog( 0, "[ANTILOG] AddUser Insert - Uidx(%u), Seed(%u/%u)", dwUserIndex, dwUserSeed, dwNPCSeed );
		}*/
		}
	void RemoveUser(const DWORD dwUserIndex)
	{
		/*if( IsWriteLog() )
		{
			CheatLOG.PrintTimeAndLog( 0, "[ANTILOG] RemoveUser - Uidx(%u)", dwUserIndex );
		}*/

		for(unsigned int i=0; i< m_RelayUserList.size() ; i++)
		{
			UserData& rkUser = m_RelayUserList.at(i);
			if( rkUser.m_dwUserIndex == dwUserIndex )
			{
				m_RelayUserList.erase(m_RelayUserList.begin() + i);
				CalculateTeamBalance();
				break;
			}
		}
	}
	int GetUserCount()
	{
		return m_RelayUserList.size();
	}

	UserData* GetUserData( DWORD dwUserIndex )
	{
		for( DWORD i = 0; i < m_RelayUserList.size(); i++ )
		{
			if( m_RelayUserList[i].m_dwUserIndex == dwUserIndex )
				return &m_RelayUserList[i];
		}
		return NULL;
	}
	float TeamBalanceCalcForGauge( int iTeamCnt, int iEnemyTeamCnt, int iModeType, int iModeStyle )
	{
		float fResult = iEnemyTeamCnt / iTeamCnt;

		if( fResult < 1.f )
			fResult = 1.f;

		if ( fResult == 1.f )
		{
			if( iModeStyle == 0 )
				fResult *= 10.f;
			else if( iModeStyle == 4 )
				fResult *= 2.f;

			return fResult;
		}

		if( iModeType == 1 )
			fResult *= 2.f;
		else if( iModeType == 2 )
			fResult *= 1.1f;
		else if( iModeType == 3 )
			fResult *= 2.f;
		else if( iModeType == 6 )
			fResult *= 1.1f;
		else if( iModeType == 9 )
			fResult *= 2.f;

		return fResult;
	}

	void InitMode()
	{
		//본부 및 광장
		if( m_iModeType == 4 || m_iModeType == 13)
		{
			m_fModeBalanceRed = m_fModeBalanceBlue = 2.0f;
		}

		//파이트 클럽일 때만
		else if( m_iModeType == 15 )
		{
			// 최대 1.3 * 1.3 이  가능함. 그러니 일단 맥시멈값으로 임시작업
			m_fModeBalanceRed = m_fModeBalanceBlue = 1.69f;
		}
		else
		{
			m_fTeamBalanceRed = m_fTeamBalanceBlue = 1.f;
			m_fWinBalanceRed = m_fWinBalanceBlue = 1.f;
			m_fModeBalanceRed = m_fModeBalanceBlue = 1.f;
		}
	}

	RelayGroup() : m_bWriteLog(false){}
};

struct SendRelayInsertData : RelayHeader // 릴레이 서버에게 보낼때 쓰는 전용  
{
	int          m_modeType;
	DWORD		 m_dwRoomIndex;
	DWORD		 m_dwUserIndex;
	char         m_szPublicIP[STR_IP_MAX];
	int          m_iClientPort;
	char         m_szPublicID[ID_NUM_PLUS_ONE];
};
struct RemoveData :RelayHeader
{
	DWORD m_dwRoomIndex;
	DWORD m_dwUserIndex;
};
#endif
//////////////////////////////////////////////////////////////////////////




#pragma pack(push,1)
struct SendRelayInfo_
{
	TCHAR m_ipAddr[STR_IP_MAX];
	int m_port;
	int  m_userCount;
	int  m_roomCount;
	int  m_serverCount;
	int m_64DropCount;
	int m_256DropCount;
	int m_1024DropCount;
	int m_64UsingCount;
	int m_256UsingCount;
	int m_1024UsingCount;
	SendRelayInfo_()
	{
		ZeroMemory(m_ipAddr,STR_IP_MAX);
		m_port = 0; 
		m_userCount = 0;
		m_roomCount= 0;
		m_64DropCount= 0;
		m_256DropCount= 0;
		m_1024DropCount= 0;
		m_64UsingCount= 0;
		m_256UsingCount= 0;
		m_1024UsingCount= 0;
	}
};
#pragma pack(pop)

/************************************************************************/
/* Singleton                                                                     */
/************************************************************************/
typedef cSingleton<GameSvrUDPModule> S_UDPModule;
#define g_UDPModule (*S_UDPModule::GetInstance())

typedef cSingleton<GameSvrUDPNode> S_UDPNode;
#define g_UDPNode (*S_UDPNode::GetInstance())

typedef cSingleton<ioBroadCastRelayModule> S_RELAY;
#define g_Relay (*S_RELAY::GetInstance())

typedef cSingleton<IPBlockerManager> S_BLOCK;
#define g_IPBlock (*S_BLOCK::GetInstance())

typedef cSingleton<WordFilterManager> S_WordFilter;
#define g_WordFilter (*S_WordFilter::GetInstance())

typedef cSingleton<ioLanguages> S_Languages;
#define g_Languages (*S_Languages::GetInstance())
#endif

#define PACKET_GUARD_VOID(x)	{ BOOL rtval = x; if( (rtval)==FALSE ) { LOG.PrintTimeAndLog(0,"[PACKET_GUARRD_ERROR] %s::%s",__FUNCTION__,#x); return; } }
#define PACKET_GUARD_INT(x)		{ BOOL rtval = x; if( (rtval)==FALSE ) { LOG.PrintTimeAndLog(0,"[PACKET_GUARRD_ERROR] %s::%s",__FUNCTION__,#x); return -1; } }
#define PACKET_GUARD_BOOL(x)	{ BOOL rtval = x; if( (rtval)==FALSE ) { LOG.PrintTimeAndLog(0,"[PACKET_GUARRD_ERROR] %s::%s",__FUNCTION__,#x); return FALSE; } }
#define PACKET_GUARD_bool(x)	{ BOOL rtval = x; if( (rtval)==FALSE ) { LOG.PrintTimeAndLog(0,"[PACKET_GUARRD_ERROR] %s::%s",__FUNCTION__,#x); return false; } }
#define PACKET_GUARD_BREAK(x)	{ BOOL rtval = x; if( (rtval)==FALSE ) { LOG.PrintTimeAndLog(0,"[PACKET_GUARRD_ERROR] %s::%s",__FUNCTION__,#x); break; } }
#define PACKET_GUARD_BOOL(x)	{ BOOL rtval = x; if( (rtval)==FALSE ) { LOG.PrintTimeAndLog(0,"[PACKET_GUARRD_ERROR] %s::%s",__FUNCTION__,#x); return FALSE; } }
#define PACKET_GUARD_NULL(x)	{ BOOL rtval = x; if( (rtval)==FALSE ) { LOG.PrintTimeAndLog(0,"[PACKET_GUARRD_ERROR] %s::%s",__FUNCTION__,#x); return NULL; } }

//x 인자값 mV maxValue 값 
#define MAX_GUARD(x, mV) if(x > mV || x < 0) { LOG.PrintTimeAndLog(0,"[error][limit][CHECK_MAX_VALUE_ERROR] %s::value:%d max:%d",__FUNCTION__,x, mV); x = 0; }

// template <typename T>
// typedef std::function<T(bool)> PACKET_FUNTION;

 