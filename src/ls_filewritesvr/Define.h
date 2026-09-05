#ifndef ___DEFINE_H__
#define ___DEFINE_H__
#include "../include/cSingleton.h"
//#include "Network/GameSvrUDPModule.h"
//#include "Network/GameSvrUDPNode.h"
#include "../../src/iocpSocketDLL/SocketModules/PacketQueue.h"
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
#define MAX_EQUIP_SLOT	6		//Character EquipSlot의 최대사이즈

#define GUILD_NAME_NUMBER			20
#define GUILD_NAME_NUM_PLUS_ONE		21
#define GUILD_POS_NUMBER			20
#define GUILD_POS_NUM_PLUS_ONE		21
#define GUILD_TITLE_NUMBER			110
#define GUILD_TITLE_NUMBER_PLUS_ONE 111
#define GUILD_CREATE_ENTRY_USER     8
#define GUILD_MAX_ENTRY_DELAY_USER  16

#define TOURNAMENT_TITLE_NUM_PLUS_ONE              21
#define TOURNAMENT_CAMP_NAME_NUM_PLUS_ONE          21
#define TOURNAMENT_TEAM_NAME_NUM_PLUS_ONE          21
#define TOURNAMENT_TEAM_MAX_LOAD                   10

#define CREATE_RESERVE_DELAY_TIME          10000       //전투 길드 파티 생성 예약 대기 시간

#define US_TUTORIAL_CLEAR      -1

#define MAX_CHAR_DBITEM_SLOT   4	//InventorySlot의 최대사이즈

#define IP_NUM_PLUS_ONE 16

#define USER_GUID_NUM_PLUS_ONE 32

#define CHANNELING_USER_ID_NUM_PLUS_ONE 33
#define CHANNELING_USER_NO_NUM_PLUS_ONE 21

#define MAX_CONTROL_KEYS_PLUS_ONE       201
#define USER_BIRTH_DATE_PLUS_ONE        7

#define MAX_INT_VALUE					2147483647

// 로그 레벨 - 
#define LOG_TEST_LEVEL                  -1
#define LOG_DEBUG_LEVEL                 0
#define LOG_RELEASE_LEVEL               0

#define COMPARE(x,min,max) (((x)>=(min))&&((x)<(max)))
#define SAFEDELETE(x)		if(x != NULL) { delete x; x = NULL; }
#define SAFEDELETEARRAY(x)	if(x != NULL) { delete [] x; x = NULL; }

struct Vector3
{
	float x,y,z;

	Vector3()
	{
		x = 0;
		y = 0;
		z = 0;
	}

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
};

enum CampType
{
	CAMP_NONE,
	CAMP_BLUE,
	CAMP_RED,
};

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
	MAX_MODE_TYPE
};

enum RoomStyle
{
	RSTYLE_NONE	= 0,
	RSTYLE_BATTLEROOM,	//전투방
	RSTYLE_PLAZA,		//광장
	RSTYLE_LADDERBATTLE,//길드전
	RSTYLE_HEADQUARTERS,//본부
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
	CNT_MGAME       =   300,
	CNT_DAUM        =   400,
	CNT_NAVER       =   600,
	CNT_TOONILAND   =   700,
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

typedef std::vector< DamageTable > DamageTableList;

#define MAX_MODE ( MAX_MODE_TYPE - 1 )

typedef std::vector<int> IntVec;
typedef std::vector<DWORD> DWORDVec;
typedef std::vector<float> FloatVec;
typedef std::vector<UserRankInfo> UserRankInfoList;

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
 
struct Relayheader : public NodeData
{
	int m_packetId;
};
struct UDPPacekt :Relayheader
{
	sockaddr_in m_client_addr;
	int m_size;
	char* m_buffer;

};
struct InsertData : Relayheader
{
	DWORD		 m_dwRoomIndex;
	DWORD		 m_dwUserIndex;
	char         m_szPublicIP[STR_IP_MAX];
	int          m_iClientPort;
};
struct SendRelayInsertData : Relayheader // 릴레이 서버에게 보낼때 쓰는 전용  
{
	int          m_modeType;
	DWORD		 m_dwRoomIndex;
	DWORD		 m_dwUserIndex;
	char         m_szPublicIP[STR_IP_MAX];
	int          m_iClientPort;
	char         m_szPublicID[ID_NUM_PLUS_ONE];
};
struct RemoveData :Relayheader
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
	DWORD   m_dwRoomIndex;
	std::vector<UserData> m_RelayUserList;

};
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
		m_serverCount = 0;
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
//typedef cSingleton<GameSvrUDPModule> S_UDPModule;
//#define g_UDPModule (*S_UDPModule::GetInstance())
//
//typedef cSingleton<GameSvrUDPNode> S_UDPNode;
//#define g_UDPNode (*S_UDPNode::GetInstance())
//
//typedef cSingleton<ioBroadCastRelayModule> S_RELAY;
//#define g_Relay (*S_RELAY::GetInstance())

#define MAX_PACKET_BUF 512
typedef struct tagFilePacket
{
	char m_FilePacket[MAX_PACKET_BUF];

	tagFilePacket()
	{
		ZeroMemory( m_FilePacket, sizeof( m_FilePacket ) );
	}
}FilePacket;

#endif