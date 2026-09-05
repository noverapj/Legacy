#ifndef ___DEFINE_H__
#define ___DEFINE_H__

#include "Protocol.h"
#include <stdint.h>

class SP2Packet;

#define ENC_LOGIN_KEY_NUM       30
#define LOGIN_KEY_PLUS_ONE      16
#define ENC_ID_NUM_PLUS_ONE     25
#define ID_NUMBER       20
#define ID_NUM_PLUS_ONE 21
#define PW_NUMBER       12
#define PW_NUM_PLUS_ONE 13
#define PW_ENCRYPT_NUMBER	24
#define PW_ENCRYPT_PLUS_ONE 25

#define MAX_CHARACTER   12      //유저가 소유할수 있는 최대 캐릭터 수  
#define MAX_SLOTBAG     20      //한개의 슬롯에 들어갈수있는 최대값.
#define MAX_USE_SLOTBAG 16		//한개 슬롯에서 현재 사용가능 최대값

#define MAX_RIGHT_SLOT_SIZE 20	//아이템 도감 별 최대 인덱스 사이즈

#define MAX_PLAYER		16

#define MAX_JOIN_CHANNEL 3

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

#define GUILD_NAME_NUMBER			12
#define GUILD_NAME_NUM_PLUS_ONE		13
#define GUILD_POS_NUMBER			12
#define GUILD_POS_NUM_PLUS_ONE		13
#define GUILD_TITLE_NUMBER			110
#define GUILD_TITLE_NUMBER_PLUS_ONE 111
#define GUILD_MAX_ENTRY_USER		16
#define GUILD_MAX_ENTRY_DELAY_USER  16
#define GUILD_MAX_LOAD_LIST         30
#define GUILD_LOAD_START_INDEX      2147483647

#define US_TUTORIAL_CLEAR      -1

#define MAX_CHAR_DBITEM_SLOT   4	//InventorySlot의 최대사이즈

#define IP_NUM_PLUS_ONE 16

#define USER_GUID_NUM_PLUS_ONE   32

#define USER_ID_NUM				 100

#define CONNECT_WAIT_SECONDS     2

#define WEB_BUFF_SIZE            4096

#define DAUM_USER_ADULT_VALUE	101

#define HANGAME_CHNL			"HG"
#define HANGAME_COIN_TYPE		"05"
#define HANGAME_CPID			"K_LOSA"

enum 
{
	MAX_ITEM_VALUE = 5,
};

#define COMPARE(x,min,max) (((x)>=(min))&&((x)<(max)))
#define SAFEDELETE(x)		if(x != NULL) { delete x; x = NULL; }
#define SAFEDELETEARRAY(x)	if(x != NULL) { delete [] x; x = NULL; }

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

struct TwoOfINT
{
	int value1;
	int value2;

	TwoOfINT()
	{
		value1	= 0;
		value2	= 0;
	}
};

typedef std::vector< Vector3 > Vector3Vec;
typedef std::deque< Vector3 > Vector3Deq;
typedef std::vector<TwoOfINT> TwoOfINTVec;

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
	TEAM_PRIVATE_16
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
	MT_NONE			= 0,
	MT_SYMBOL		= 1,
	MT_CATCH		= 2,
	MT_KING			= 3,
	MT_TRAINING		= 4,
	MT_SURVIVAL		= 5,
//	MT_TEAM_SURVIVAL = 6,
	MAX_MODE_TYPE
};

// DB에 저장되는 값으로 순차적으로 증가하지 않는다.
enum ChannelingType
{
	CNT_NONE        =   -1,
	CNT_WEMADEBUY   =   0,
	CNT_MGAME       =   300,
	CNT_DAUM        =   400,
	CNT_NAVER       =   600,
	CNT_TOONILAND   =   700,
	CNT_NEXON		=	800,
	CNT_HANGAME		=	900,
	CNT_WEMADE      =   21234, // DB에 저장되지 않는 임의값, 전 유저 PC방만 확인용.
	CNT_NEXONSESSION	=   21235,
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

typedef std::vector< DamageTable > DamageTableList;

#define MAX_MODE ( MAX_MODE_TYPE - 1 )

typedef std::vector<int> IntVec;
typedef std::vector<DWORD> DWORDVec;
typedef std::vector<float> FloatVec;
typedef std::vector<UserRankInfo> UserRankInfoList;

extern LONG __stdcall ExceptCallBack ( EXCEPTION_POINTERS * pExPtrs );
extern char *_i64toStr(__int64 i64Num);

#pragma pack(1) 

// mgame
typedef struct tagMgameCashRequest
{
	BYTE  m_byStart[2];
	short m_sSize;
	BYTE  m_byCommand;
	char  m_szGameID[11];
	char  m_szUserID[20];
	char  m_szGoodsNo[10];
	char  m_szItemName[50];
	int   m_iPrice;
	char  m_szUserIP[16];
	int   m_iServerIdx;
	char  m_szCharName[50];
	int   m_iPresent; // 1:선물, 0:선물아님
	BYTE  m_byEnd[2];

	tagMgameCashRequest()
	{
		m_byStart[0] = 0xAA;
		m_byStart[1] = 0x44;
		m_sSize      = 170; // byStart + sSize + byEnd을 제외한 나머지 사이즈
		m_byCommand  = 0;
		StringCbCopy( m_szGameID, sizeof( m_szGameID), "KMLS1233" );
		ZeroMemory( m_szUserID, sizeof( m_szUserID ) );
		ZeroMemory( m_szGoodsNo, sizeof( m_szGoodsNo ) );
		ZeroMemory( m_szItemName, sizeof( m_szItemName ) );
		m_iPrice     = 0;
		ZeroMemory( m_szUserIP, sizeof( m_szUserIP ) );
		m_iServerIdx = 0;
		ZeroMemory( m_szCharName, sizeof( m_szCharName ) );
		m_iPresent   = 0;
		m_byEnd[0] = 0x44;
		m_byEnd[1] = 0xAA;
	}

	void SetInfo( BYTE byCommand, const char *szUserID, int iGoodsNo, const char *szItemName, int iPrice, const char *szUserIP, const char *szPublicID, bool bPresent )
	{
		m_byCommand = byCommand;
		StringCbCopy( m_szUserID, sizeof( m_szUserID ), szUserID );
		if( iGoodsNo != 0 )
			StringCbPrintf( m_szGoodsNo, sizeof( m_szGoodsNo ), "%d", iGoodsNo );
		else
			StringCbCopy( m_szGoodsNo, sizeof( m_szGoodsNo ), "0000" ); // 조회시 설정하는 값
		StringCbCopy( m_szItemName, sizeof( m_szItemName ), szItemName );
		m_iPrice = iPrice;
		StringCbCopy( m_szUserIP, sizeof( m_szUserIP ), szUserIP );
		StringCbCopy( m_szCharName, sizeof( m_szCharName ), szPublicID );

		if( bPresent )
			m_iPresent = 1;
		else
			m_iPresent = 0;
	}

}MgameCashRequest;

typedef struct tagMgameCashResult
{
	BYTE  m_byStart[2];
	short m_sSize;
	int   m_iRet;
	char  m_szUserID[20];
	int   m_iMCash;  // 엠캐쉬
	int   m_iPCash;  // 선물캐쉬
	BYTE  m_byEnd[2];

	tagMgameCashResult()
	{
		m_byStart[0] = 0xAA;
		m_byStart[1] = 0x44;
		m_sSize      = 32; // byStart + sSize + byEnd을 제외한 나머지 사이즈
		m_iRet       = 0;
		ZeroMemory( m_szUserID, sizeof( m_szUserID ) );
		m_iMCash     = 0;
		m_iPCash     = 0;
		m_byEnd[0] = 0x44;
		m_byEnd[1] = 0xAA;
	}

	bool IsValidPacket()
	{
		if( m_byStart[0] != 0xAA )
			return false;

		if( m_byStart[1] != 0x44 )
			return false;

		if( m_sSize != 32 )
			return false;

		if( m_byEnd[0] != 0x44 )
			return false;

		if( m_byEnd[1] != 0xAA )
			return false;

		return true;
	}
}MgameCashResult;

// Wemade
#define WEMADE_LOSTSAGA_GAME_NO   125
typedef struct _BOQPTS_GS_CONNECT
{
	WORD  m_wReqLen;
	WORD  m_wReqType;
	DWORD m_dwReqKey;

	WORD  m_wGameServerNo;
	DWORD m_dwWorldNo;
	WORD  m_wGameNo;
	BYTE  m_byConnectFlag;

	WORD	m_wRetCode;
	
	_BOQPTS_GS_CONNECT()
	{
		m_wReqLen   = sizeof(BOQPTS_GS_CONNECT);
		m_wReqType  = 100;
		m_dwReqKey  = 0;

		m_wGameServerNo = 0;
		m_dwWorldNo     = 0;
		m_wGameNo       = WEMADE_LOSTSAGA_GAME_NO;
		m_byConnectFlag = 0;

		m_wRetCode      = 0;
	}

	void SetInfo( bool bBillingRelayServerStart )
	{
		if( bBillingRelayServerStart )
			m_byConnectFlag = 1;
		else
			m_byConnectFlag = 2;
	}

	// 바이트오더 갱신
	void Htonl()
	{
		m_wReqLen        = htons( m_wReqLen );       
		m_wReqType       = htons( m_wReqType );             
		m_dwReqKey       = htonl( m_dwReqKey );             
		m_wGameServerNo  = htons( m_wGameServerNo );             
		m_dwWorldNo      = htonl( m_dwWorldNo );             
		m_wGameNo        = htons( m_wGameNo );             
		m_wRetCode       = htons( m_wRetCode );           	
	}

	void Ntohl()
	{
		m_wReqLen        = ntohs( m_wReqLen );       
		m_wReqType       = ntohs( m_wReqType );             
		m_dwReqKey       = ntohl( m_dwReqKey );             
		m_wGameServerNo  = ntohs( m_wGameServerNo );             
		m_dwWorldNo      = ntohl( m_dwWorldNo );             
		m_wGameNo        = ntohs( m_wGameNo );             
		m_wRetCode       = ntohs( m_wRetCode );             	
	}
	
}BOQPTS_GS_CONNECT, PBOQPTS_GS_CONNECT;

typedef struct _BOQPTS_HEALTH_CHECK
{
	WORD  m_wReqLen;       // 전문길이 (sizeof(BOQPTS_HEALTH_CHECK))
	WORD  m_wReqType;      // 요청 구분(110: PTS 정상 동작 체크 요청)
	DWORD m_dwReqKey;      // 요청 패킷 고유번호, Async 통신을 위해서 각 요청 패킷에 유니크하게 부여하는 번호, 필요하지 않은 경우 0으로 설정

	WORD  m_wGameServerNo; // 게임서버 식별자 번호
	DWORD m_dwWorldNo;     // 게임월드 식별자 번호(월드 구분이 없는 경우 0 으로 설정)
	WORD  m_wGameNo;       // 게임별 식별자 (위메이드가 최초에 부여함.)

	// OUT
	WORD  m_wRetCode;      // 응답코드(0:정상 동작), 즉 PTS에서 응답이 가면 정상으로 판단하고 응답이 가지 않는 경우 또는 요청을 보낼 수 없는 경우 PTS가 문제가 있다고 판단한다.

	_BOQPTS_HEALTH_CHECK()
	{
		m_wReqLen       = sizeof( BOQPTS_HEALTH_CHECK );
		m_wReqType      = 110; 
		m_dwReqKey      = 0;   
        m_wGameServerNo = 0; 
		m_dwWorldNo     = 0; 
		m_wGameNo       = WEMADE_LOSTSAGA_GAME_NO;
		m_wRetCode      = 0;
	}

	void Htonl()
	{
		m_wReqLen        = htons( m_wReqLen );       
		m_wReqType       = htons( m_wReqType );             
		m_dwReqKey       = htonl( m_dwReqKey );             
		m_wGameServerNo  = htons( m_wGameServerNo );             
		m_dwWorldNo      = htonl( m_dwWorldNo );             
		m_wGameNo        = htons( m_wGameNo );             
		m_wRetCode       = htons( m_wRetCode );           
	}

	void Ntohl()
	{
		m_wReqLen        = ntohs( m_wReqLen );       
		m_wReqType       = ntohs( m_wReqType );             
		m_dwReqKey       = ntohl( m_dwReqKey );             
		m_wGameServerNo  = ntohs( m_wGameServerNo );             
		m_dwWorldNo      = ntohl( m_dwWorldNo );             
		m_wGameNo        = ntohs( m_wGameNo );             
		m_wRetCode       = ntohs( m_wRetCode );             
	}

} BOQPTS_HEALTH_CHECK, *PBOQPTS_HEALTH_CHECK;

typedef struct _BOQPTS_CHECKPREMIUM2
{
	WORD  m_wReqLen;        // 전문길이 (sizeof(BOQPTS_CHECKPREMIUM))
	WORD  m_wReqType;       // 요청 구분(121: PC방 결제정보 확인 요청2)
	DWORD m_dwReqKey;       // 요청 패킷 고유번호, Async 통신을 위해서 각 요청 패킷에 유니크하게 부여하는 번호, 필요하지 않은 경우 0으로 설정

	DWORD m_dwUserNo;       // 사용자의 고유번호 (아이디별 SeqNo) , 없는 경우 0 설정
	DWORD m_dwClientIP;     // 사용자의 게임 클라이언트가 동작중인 PC의 IP주소
	WORD  m_wClientPort;    // 사용자의 게임 클라이언트가 Binding된 포트
	WORD  m_wGameServerNo;  // 게임서버 식별자 번호
	DWORD m_dwWorldNo;      // 게임월드 식별자 번호(월드 구분이 없는 경우 0 으로 설정)
	WORD  m_wGameNo;        // 게임별 식별자 (위메이드가 최초에 부여함.)
	char  m_szUserID[50+1]; // 이용자ID(로그인 ID)

	// OUT
	BYTE  m_byPremiumType;  // 프리미엄등급(0:No 프리미엄, 1:1등급, 2:2등급…)
	BYTE  m_byBillMethod;   // 티켓 유형(1:정액제, 2:정량제,시간 정량제)
	DWORD m_dwRemainTime;   // 잔여시간(잔여시간 분 단위)
	BYTE  m_bySameGameUser; // 현재 PC방에서 같은 게임을 하는 사용자수	(0~255 사이)
	DWORD m_dwPCBangNo;     // 현재 PC방의 유니크번호
	WORD  m_wRetCode;       // 응답코드(0:성공, 9:중복로그인,  >50:Failed)	*50초과의 에러코드는 에러코드집 참조.

	_BOQPTS_CHECKPREMIUM2()
	{
		m_wReqLen  = sizeof( BOQPTS_CHECKPREMIUM2 );
		m_wReqType = 121;    
		m_dwReqKey = 0;    

		m_dwUserNo      = 0;    
		m_dwClientIP    = 0;  
		m_wClientPort   = 0; 
		m_wGameServerNo = 0; 
		m_dwWorldNo     = 0;     
		m_wGameNo       = WEMADE_LOSTSAGA_GAME_NO;       
		ZeroMemory( m_szUserID, sizeof( m_szUserID ) ); 

		// OUT
		m_byPremiumType  = 0;  
		m_byBillMethod   = 0;   
		m_dwRemainTime   = 0;   
		m_bySameGameUser = 0; 
		m_dwPCBangNo     = 0;     
		m_wRetCode       = 0;       
	}

	void SetInfo( const char *szUserID, const char *szUserIP, const int iUserPort )
	{
		if( !szUserID )
			return;
		if( !szUserIP )
			return;
		StringCbCopy( m_szUserID, sizeof( m_szUserID ), szUserID );
		inet_pton(AF_INET, szUserIP, &m_dwClientIP);
		m_wClientPort = iUserPort;
	}

	void Htonl()
	{
		m_wReqLen        = htons( m_wReqLen );       
		m_wReqType       = htons( m_wReqType );             
		m_dwReqKey       = htonl( m_dwReqKey );  
		m_dwUserNo       = htonl( m_dwUserNo );  
		m_dwClientIP     = htonl( m_dwClientIP );  
		m_wClientPort    = htons( m_wClientPort );  
		m_wGameServerNo  = htons( m_wGameServerNo );             
		m_dwWorldNo      = htonl( m_dwWorldNo );             
		m_wGameNo        = htons( m_wGameNo );   
		m_dwRemainTime   = htonl( m_dwRemainTime );     
		m_dwPCBangNo     = htonl( m_dwPCBangNo );     
		m_wRetCode       = htons( m_wRetCode );           
	}

	void Ntohl()
	{
		m_wReqLen        = ntohs( m_wReqLen );       
		m_wReqType       = ntohs( m_wReqType );             
		m_dwReqKey       = ntohl( m_dwReqKey );  
		m_dwUserNo       = ntohl( m_dwUserNo );  
		m_dwClientIP     = ntohl( m_dwClientIP );  
		m_wClientPort    = ntohs( m_wClientPort );  
		m_wGameServerNo  = ntohs( m_wGameServerNo );             
		m_dwWorldNo      = ntohl( m_dwWorldNo );             
		m_wGameNo        = ntohs( m_wGameNo );   
		m_dwRemainTime   = ntohl( m_dwRemainTime );     
		m_dwPCBangNo     = ntohl( m_dwPCBangNo );     
		m_wRetCode       = ntohs( m_wRetCode );                     
	}

} BOQPTS_CHECKPREMIUM2, *PBOQPTS_CHECKPREMIUM2;

// Wemade buy /////////////////////////////////////////////////////////////
#define WEMADE_BUY_LOSTSAGA_GAME_NO   201125
#define WEMADE_BUY_LOSTSAGA_PCODE     100103
// 통신상태 체크때 (다른 패킷 헤더도 겸함)
// Client -> Server
typedef struct _tagPHEADER  {
	WORD	Code;
	WORD	Version;
	WORD	Length;
	DWORD	ReqKey;
	DWORD	GameID;
	DWORD	PCode;
	DWORD	Result;

	_tagPHEADER()
	{
		Code    = 0;
		Length  = sizeof( PHEADER );
		Version = 0;
		ReqKey  = 0;
		GameID  = WEMADE_BUY_LOSTSAGA_GAME_NO;
		PCode   = WEMADE_BUY_LOSTSAGA_PCODE;
		Result  = 0;
	}

	void Htonl()
	{
		Code    = htons( Code );       
		Length  = htons( Length );             
		Version = htons( Version );  
		ReqKey  = htonl( ReqKey );  
		GameID  = htonl( GameID );  
		PCode   = htonl( PCode );  
		Result  = htonl( Result );  
	}

	void Ntohl()
	{
		Code    = ntohs( Code );       
		Length  = ntohs( Length );             
		Version = ntohs( Version );  
		ReqKey  = ntohl( ReqKey );  
		GameID  = ntohl( GameID );  
		PCode   = ntohl( PCode );  
		Result  = ntohl( Result );
	}

} PHEADER;

// Server  ->  Client
typedef struct _tagSC_ALIVE : public PHEADER  {

	DWORD	ResultCode;
	char	ResultMessage[256];

	_tagSC_ALIVE()
	{
		Code    = 0;
		Length  = sizeof( PHEADER );
		Version = 0;
		ReqKey  = 0;
		GameID  = WEMADE_BUY_LOSTSAGA_GAME_NO;
		PCode   = WEMADE_BUY_LOSTSAGA_PCODE;
		Result  = 0;
		ResultCode    = 0;
		ZeroMemory( ResultMessage, sizeof( ResultMessage ) ); 
	}

	void Htonl()
	{       
		Code    = htons( Code );       
		Length  = htons( Length );             
		Version = htons( Version );  
		ReqKey  = htonl( ReqKey );  
		GameID  = htonl( GameID );  
		PCode   = htonl( PCode );  
		Result  = htonl( Result );  
		ResultCode  = htonl( ResultCode );        
	}

	void Ntohl()
	{
		Code    = ntohs( Code );       
		Length  = ntohs( Length );             
		Version = ntohs( Version );  
		ReqKey  = ntohl( ReqKey );  
		GameID  = ntohl( GameID );  
		PCode   = ntohl( PCode );  
		Result  = ntohl( Result );
		ResultCode = ntohl( ResultCode );
	}
} SC_ALIVE;


// 잔액조회
// Client  ->  Server
typedef struct _tagCS_BALANCE : public PHEADER  {

	DWORD	UserNo;
	char	UserID[64];
	char	ClientIP[16];

	_tagCS_BALANCE()
	{
		Code    = 10;
		Length  = sizeof( CS_BALANCE );
		Version = 0;
		ReqKey  = 0;
		GameID  = WEMADE_BUY_LOSTSAGA_GAME_NO;
		PCode   = WEMADE_BUY_LOSTSAGA_PCODE;
		Result  = 0;
		UserNo  = 0;
		ZeroMemory( UserID, sizeof( UserID ) ); 
		ZeroMemory( ClientIP, sizeof( ClientIP ) ); 
	}

	void SetInfo( DWORD dwReqKey, DWORD dwUserNo, const char *szUserID, const char *szUserIP )
	{
		if( !szUserID )
			return;
		if( !szUserIP )
			return;
		ReqKey = dwReqKey;
		UserNo = dwUserNo;
		StringCbCopy( UserID, sizeof( UserID ), szUserID );
		StringCbCopy( ClientIP, sizeof( ClientIP ), szUserIP );
	}

	void Htonl()
	{
		Code    = htons( Code );       
		Length  = htons( Length );             
		Version = htons( Version );  
		ReqKey  = htonl( ReqKey );  
		GameID  = htonl( GameID );  
		PCode   = htonl( PCode );  
		Result  = htonl( Result );  
		UserNo  = htonl( UserNo );             
	}

	void Ntohl()
	{
		Code    = ntohs( Code );       
		Length  = ntohs( Length );             
		Version = ntohs( Version );  
		ReqKey  = ntohl( ReqKey );  
		GameID  = ntohl( GameID );  
		PCode   = ntohl( PCode );  
		Result  = ntohl( Result );
		UserNo  = ntohl( UserNo );                   
	}

} CS_BALANCE;


// 잔액조회
// Server ->  Client
typedef struct _tagSC_BALANCE : public PHEADER  {
	DWORD	UserNo;
	char	UserID[64];
	DWORD	RealCash;
	DWORD	BonusCash;
	DWORD	ResultCode;
	char	ResultMessage[256];

	_tagSC_BALANCE()
	{
		Code    = 10;
		Length  = sizeof( SC_BALANCE );
		Version = 0;
		ReqKey  = 0;
		GameID  = WEMADE_BUY_LOSTSAGA_GAME_NO;
		PCode   = WEMADE_BUY_LOSTSAGA_PCODE;
		Result  = 0;
		UserNo  = 0;
		ZeroMemory( UserID, sizeof( UserID ) ); 
		RealCash= 0;
		BonusCash = 0;
		ResultCode= 0;
		ZeroMemory( ResultMessage, sizeof( ResultMessage ) ); 
	}

	void Htonl()
	{
		Code    = htons( Code );       
		Length  = htons( Length );             
		Version = htons( Version );  
		ReqKey  = htonl( ReqKey );  
		GameID  = htonl( GameID );  
		PCode   = htonl( PCode );  
		Result  = htonl( Result );  
		UserNo  = htonl( UserNo );             
		RealCash    = htonl( RealCash );        
		BonusCash   = htonl( BonusCash );        
		ResultCode  = htonl( ResultCode );        
	}

	void Ntohl()
	{
		Code    = ntohs( Code );       
		Length  = ntohs( Length );             
		Version = ntohs( Version );  
		ReqKey  = ntohl( ReqKey );  
		GameID  = ntohl( GameID );  
		PCode   = ntohl( PCode );  
		Result  = ntohl( Result );
		UserNo  = ntohl( UserNo );                   
		RealCash   = ntohl( RealCash );
		BonusCash  = ntohl( BonusCash );
		ResultCode = ntohl( ResultCode );
	}

} SC_BALANCE;

// 아이템구매
// Client  ->  Server
typedef struct _tagCS_PURCHASEITEM : public PHEADER  {
	DWORD	UserNo;
	char	UserID[64];
	char	ClientIP[16];
	WORD	ClientPort;
	char	CharacterName[64];
	DWORD	WorldNo;
	DWORD	ChannelNo;
	char	Location[2];
	DWORD	ChargeAmt;
	DWORD	ItemCount;
	DWORD	ItemCategoryCode;
	char	ItemCategoryName[64];
	DWORD	ItemType; 
	char	ItemCode[256];
	char	ItemName[64];
	DWORD	ItemUnitPrice;
	DWORD	ItemQuantity;

	_tagCS_PURCHASEITEM()
	{
		Code    = 20;
		Length  = sizeof( CS_PURCHASEITEM );
		Version = 0;
		ReqKey  = 0;
		GameID  = WEMADE_BUY_LOSTSAGA_GAME_NO;
		PCode   = WEMADE_BUY_LOSTSAGA_PCODE;
		Result  = 0;
		UserNo  = 0;
		ZeroMemory( UserID, sizeof( UserID ) ); 
		ZeroMemory( ClientIP, sizeof( ClientIP ) ); 
		ClientPort = 0;
		ZeroMemory( CharacterName, sizeof( CharacterName ) );
		WorldNo   = 0;
		ChannelNo = 0 ;
		StringCbCopy( Location, sizeof( Location ), "G" );
		ChargeAmt = 0;
		ItemCount = 1;
		ItemCategoryCode = 0;
		ZeroMemory( ItemCategoryName, sizeof( ItemCategoryName ) );
		ItemType = 0; 
		ZeroMemory( ItemCode, sizeof( ItemCode ) );
		ZeroMemory( ItemName, sizeof( ItemName ) );
		ItemUnitPrice = 0;
		ItemQuantity  = 1;
	}

	void SetInfo( DWORD dwReqkey, DWORD dwUserNo, const char *szUserID, const char *szCharName, const char *szUserIP, WORD wGameServerPort, DWORD dwChargeAmt, const char * szItemCode, const char *szItemName )
	{
		if( !szUserID )
			return;
		if( !szUserIP )
			return;
		ReqKey = dwReqkey;
		UserNo = dwUserNo;
		StringCbCopy( UserID, sizeof( UserID ), szUserID );
		StringCbCopy( CharacterName, sizeof( CharacterName ), szCharName );
		StringCbCopy( ClientIP, sizeof( ClientIP ), szUserIP );
		ClientPort = wGameServerPort;
		ChargeAmt = dwChargeAmt;
		ItemUnitPrice = dwChargeAmt;
		StringCbCopy( ItemCode, sizeof( ItemCode ), szItemCode );
		StringCbCopy( ItemName, sizeof( ItemName ), szItemName );
	}

	void Htonl()
	{
		Code    = htons( Code );       
		Length  = htons( Length );             
		Version = htons( Version );  
		ReqKey  = htonl( ReqKey );  
		GameID  = htonl( GameID );  
		PCode   = htonl( PCode );  
		Result  = htonl( Result );  
		UserNo  = htonl( UserNo );             
		ClientPort  = htons( ClientPort );    
		WorldNo   = htonl( WorldNo );
		ChannelNo = htonl( ChannelNo );
		ChargeAmt = htonl( ChargeAmt );
		ItemCount = htonl( ItemCount );
		ItemCategoryCode = htonl( ItemCategoryCode );
		ItemType = htonl( ItemType ); 
		ItemUnitPrice = htonl( ItemUnitPrice );
		ItemQuantity  = htonl( ItemQuantity );
	}

	void Ntohl()
	{
		Code    = ntohs( Code );       
		Length  = ntohs( Length );             
		Version = ntohs( Version );  
		ReqKey  = ntohl( ReqKey );  
		GameID  = ntohl( GameID );  
		PCode   = ntohl( PCode );  
		Result  = ntohl( Result );
		UserNo  = ntohl( UserNo );    
		ClientPort= ntohs( ClientPort );    
		WorldNo   = ntohl( WorldNo );
		ChannelNo = ntohl( ChannelNo );
		ChargeAmt = ntohl( ChargeAmt );
		ItemCount = ntohl( ItemCount );
		ItemCategoryCode = ntohl( ItemCategoryCode );
		ItemType = ntohl( ItemType ); 
		ItemUnitPrice = ntohl( ItemUnitPrice );
		ItemQuantity  = ntohl( ItemQuantity );
	}

} CS_PURCHASEITEM;

// Server  ->  Client
typedef struct _tagSC_PURCHASEITEM : public PHEADER  {
	DWORD	UserNo;
	char	UserID[64];
	char	ChargeNo[512];
	DWORD	ChargedAmt;
	char	EventItemID[256];
	char	EventChargeNo[512];
	DWORD	RealCash;
	DWORD	BonusCash;
	DWORD	ResultCode;
	char	ResultMessage[256];

	_tagSC_PURCHASEITEM()
	{
		Code    = 20;
		Length  = sizeof( SC_PURCHASEITEM );
		Version = 0;
		ReqKey  = 0;
		GameID  = WEMADE_BUY_LOSTSAGA_GAME_NO;
		PCode   = WEMADE_BUY_LOSTSAGA_PCODE;
		Result  = 0;
		UserNo  = 0;
		ZeroMemory( UserID, sizeof( UserID ) );  
		ZeroMemory( ChargeNo, sizeof( ChargeNo ) );
		ChargedAmt     = 0;
		ZeroMemory( EventItemID, sizeof( EventItemID ) );
		ZeroMemory( EventChargeNo, sizeof( EventChargeNo ) );
		RealCash       = 0;
		BonusCash      = 0;
		ResultCode     = 0;
		ZeroMemory( ResultMessage, sizeof( ResultMessage ) );
	}

	void Htonl()
	{
		Code    = htons( Code );       
		Length  = htons( Length );             
		Version = htons( Version );  
		ReqKey  = htonl( ReqKey );  
		GameID  = htonl( GameID );  
		PCode   = htonl( PCode );  
		Result  = htonl( Result );  
		UserNo  = htonl( UserNo );             
		ChargedAmt     = htonl( ChargedAmt );
		RealCash       = htonl( RealCash );
		BonusCash      = htonl( BonusCash );
		ResultCode     = htonl( ResultCode );
	}

	void Ntohl()
	{
		Code    = ntohs( Code );       
		Length  = ntohs( Length );             
		Version = ntohs( Version );  
		ReqKey  = ntohl( ReqKey );  
		GameID  = ntohl( GameID );  
		PCode   = ntohl( PCode );  
		Result  = ntohl( Result );
		UserNo  = ntohl( UserNo );    
		ChargedAmt     = ntohl( ChargedAmt );
		RealCash       = ntohl( RealCash );
		BonusCash      = ntohl( BonusCash );
		ResultCode     = ntohl( ResultCode );
	}

} SC_PURCHASEITEM;

// 아이템 구매 선물하기
// Client ->  Server
typedef struct _tagCS_GIFTITEM : public PHEADER  {
	DWORD	UserNo;
	char	UserID[64];
	char	ClientIP[16];
	WORD	ClientPort;
	char	CharacterName[64];
	DWORD	WorldNo;
	DWORD	ChannelNo;
	char	Location[2];
	DWORD	ChargeAmt;
	DWORD	ItemCount;
	DWORD	ItemCategoryCode;
	char	ItemCategoryName[64];
	DWORD	ItemType;
	char	ItemCode[256];
	char	ItemName[64];
	DWORD	ItemUnitPrice;
	DWORD	ItemQuantity;
	char	RUserID[64];
	char	RCharacterName[64];

	_tagCS_GIFTITEM()
	{
		Code    = 21;
		Length  = sizeof( CS_GIFTITEM );
		Version = 0;
		ReqKey  = 0;
		GameID  = WEMADE_BUY_LOSTSAGA_GAME_NO;
		PCode   = WEMADE_BUY_LOSTSAGA_PCODE;
		Result  = 0;
		UserNo  = 0;
		ZeroMemory( UserID, sizeof( UserID ) ); 
		ZeroMemory( ClientIP, sizeof( ClientIP ) ); 
		ClientPort = 0;
		ZeroMemory( CharacterName, sizeof( CharacterName ) );
		WorldNo   = 0;
		ChannelNo = 0 ;
		StringCbCopy( Location, sizeof( Location ), "G" );
		ChargeAmt = 0;
		ItemCount = 1;
		ItemCategoryCode = 0;
		ZeroMemory( ItemCategoryName, sizeof( ItemCategoryName ) );
		ItemType = 0; 
		ZeroMemory( ItemCode, sizeof( ItemCode ) );
		ZeroMemory( ItemName, sizeof( ItemName ) );
		ItemUnitPrice = 0;
		ItemQuantity  = 1;
		ZeroMemory( RUserID, sizeof( RUserID ) );
		ZeroMemory( RCharacterName, sizeof( RCharacterName ) );
	}

	void SetInfo( DWORD dwReqkey, DWORD dwUserNo, const char *szUserID, const char *szCharName, const char *szUserIP, WORD wGameServerPort, DWORD dwChargeAmt, const char *szItemCode, const char *szItemName, const char *szReceiveUserID, const char *szReceiveCharName )
	{
		if( !szUserID )
			return;
		if( !szUserIP )
			return;
		ReqKey = dwReqkey;
		UserNo = dwUserNo;
		StringCbCopy( UserID, sizeof( UserID ), szUserID );
		StringCbCopy( CharacterName, sizeof( CharacterName ), szCharName );
		StringCbCopy( ClientIP, sizeof( ClientIP ), szUserIP );
		ClientPort = wGameServerPort;
		ChargeAmt = dwChargeAmt;
		ItemUnitPrice = dwChargeAmt;
		StringCbCopy( ItemCode, sizeof( ItemCode ), szItemCode );
		StringCbCopy( ItemName, sizeof( ItemName ), szItemName );
		StringCbCopy( RUserID, sizeof( RUserID ), szReceiveUserID );
		StringCbCopy( RCharacterName, sizeof( RCharacterName ), szReceiveCharName );
	}

	void Htonl()
	{
		Code    = htons( Code );       
		Length  = htons( Length );             
		Version = htons( Version );  
		ReqKey  = htonl( ReqKey );  
		GameID  = htonl( GameID );  
		PCode   = htonl( PCode );  
		Result  = htonl( Result );  
		UserNo  = htonl( UserNo );             
		ClientPort  = htons( ClientPort );    
		WorldNo   = htonl( WorldNo );
		ChannelNo = htonl( ChannelNo );
		ChargeAmt = htonl( ChargeAmt );
		ItemCount = htonl( ItemCount );
		ItemCategoryCode = htonl( ItemCategoryCode );
		ItemType = htonl( ItemType ); 
		ItemUnitPrice = htonl( ItemUnitPrice );
		ItemQuantity  = htonl( ItemQuantity );
	}

	void Ntohl()
	{
		Code    = ntohs( Code );       
		Length  = ntohs( Length );             
		Version = ntohs( Version );  
		ReqKey  = ntohl( ReqKey );  
		GameID  = ntohl( GameID );  
		PCode   = ntohl( PCode );  
		Result  = ntohl( Result );
		UserNo  = ntohl( UserNo );    
		ClientPort= ntohs( ClientPort );    
		WorldNo   = ntohl( WorldNo );
		ChannelNo = ntohl( ChannelNo );
		ChargeAmt = ntohl( ChargeAmt );
		ItemCount = ntohl( ItemCount );
		ItemCategoryCode = ntohl( ItemCategoryCode );
		ItemType = ntohl( ItemType ); 
		ItemUnitPrice = ntohl( ItemUnitPrice );
		ItemQuantity  = ntohl( ItemQuantity );
	}

} CS_GIFTITEM;

// Server  ->  Client
typedef struct _tagSC_GIFTITEM : public PHEADER  {
	DWORD	UserNo;
	char	UserID[64];
	char	ChargeNo[512];
	DWORD	ChargedCashAmt;
	char	EventItemID[256];
	char	EventChargeNo[512];
	DWORD	RealCash;
	DWORD	BonusCash;
	DWORD	RUserNo;
	char	RUserID[64];
	DWORD	ResultCode;
	char	ResultMessage[256];

	_tagSC_GIFTITEM()
	{
		Code    = 20;
		Length  = sizeof( SC_GIFTITEM );
		Version = 0;
		ReqKey  = 0;
		GameID  = WEMADE_BUY_LOSTSAGA_GAME_NO;
		PCode   = WEMADE_BUY_LOSTSAGA_PCODE;
		Result  = 0;
		UserNo  = 0;
		ZeroMemory( UserID, sizeof( UserID ) );  
		ZeroMemory( ChargeNo, sizeof( ChargeNo ) );
		ChargedCashAmt = 0;
		ZeroMemory( EventItemID, sizeof( EventItemID ) );
		ZeroMemory( EventChargeNo, sizeof( EventChargeNo ) );
		RealCash       = 0;
		BonusCash      = 0;
		RUserNo        = 0;
		ZeroMemory( RUserID, sizeof( RUserID ) );
		ResultCode     = 0;
		ZeroMemory( ResultMessage, sizeof( ResultMessage ) );
	}

	void Htonl()
	{
		Code    = htons( Code );       
		Length  = htons( Length );             
		Version = htons( Version );  
		ReqKey  = htonl( ReqKey );  
		GameID  = htonl( GameID );  
		PCode   = htonl( PCode );  
		Result  = htonl( Result );  
		UserNo  = htonl( UserNo );             
		ChargedCashAmt = htonl( ChargedCashAmt );
		RealCash       = htonl( RealCash );
		BonusCash      = htonl( BonusCash );
		RUserNo        = htonl( RUserNo );
		ResultCode     = htonl( ResultCode );
	}

	void Ntohl()
	{
		Code    = ntohs( Code );       
		Length  = ntohs( Length );             
		Version = ntohs( Version );  
		ReqKey  = ntohl( ReqKey );  
		GameID  = ntohl( GameID );  
		PCode   = ntohl( PCode );  
		Result  = ntohl( Result );
		UserNo  = ntohl( UserNo );    
		ChargedCashAmt = ntohl( ChargedCashAmt );
		RealCash       = ntohl( RealCash );
		BonusCash      = ntohl( BonusCash );
		RUserNo        = ntohl( RUserNo );
		ResultCode     = ntohl( ResultCode );
	}

} SC_GIFTITEM;

// 아이템구매취소
// Client  ->  Server
typedef struct _tagCS_CNLPURCHASE : public PHEADER  {
	DWORD	UserNo;
	char	UserID[64];
	char	ClientIP[16];
	char	ChargeNo[512];

	_tagCS_CNLPURCHASE()
	{
		Code    = 22;
		Length  = sizeof( CS_CNLPURCHASE );
		Version = 0;
		ReqKey  = 0;
		GameID  = WEMADE_BUY_LOSTSAGA_GAME_NO;
		PCode   = WEMADE_BUY_LOSTSAGA_PCODE;
		Result  = 0;
		UserNo  = 0;
		ZeroMemory( UserID, sizeof( UserID ) ); 
		ZeroMemory( ClientIP, sizeof( ClientIP ) ); 
		ZeroMemory( ChargeNo, sizeof( ChargeNo ) ); 
	}

	void SetInfo( DWORD dwReqKey, DWORD dwUserNo, const char *szUserID, const char *szUserIP, const char *szChargeNo )
	{
		if( !szUserID )
			return;
		if( !szUserIP )
			return;
		ReqKey = dwReqKey;
		UserNo = dwUserNo;
		StringCbCopy( UserID, sizeof( UserID ), szUserID );
		StringCbCopy( ClientIP, sizeof( ClientIP ), szUserIP );
		StringCbCopy( ChargeNo, sizeof( ChargeNo ), szChargeNo );
	}

	void Htonl()
	{
		Code    = htons( Code );       
		Length  = htons( Length );             
		Version = htons( Version );  
		ReqKey  = htonl( ReqKey );  
		GameID  = htonl( GameID );  
		PCode   = htonl( PCode );  
		Result  = htonl( Result );  
		UserNo  = htonl( UserNo );              
	}

	void Ntohl()
	{
		Code    = ntohs( Code );       
		Length  = ntohs( Length );             
		Version = ntohs( Version );  
		ReqKey  = ntohl( ReqKey );  
		GameID  = ntohl( GameID );  
		PCode   = ntohl( PCode );  
		Result  = ntohl( Result );
		UserNo  = ntohl( UserNo );      
	}

} CS_CNLPURCHASE;

// Server  ->  Client
typedef struct _tagSC_CNLPURCHASE : public PHEADER  {
	DWORD	UserNo;
	char	UserID[64];
	char	ChargeNo[512];
	DWORD	CanceledCashAmt;
	DWORD	RealCash;
	DWORD	BonusCash;
	DWORD	ResultCode;
	char	ResultMessage[256];

	_tagSC_CNLPURCHASE()
	{
		Code    = 22;
		Length  = sizeof( SC_CNLPURCHASE );
		Version = 0;
		ReqKey  = 0;
		GameID  = WEMADE_BUY_LOSTSAGA_GAME_NO;
		PCode   = WEMADE_BUY_LOSTSAGA_PCODE;
		Result  = 0;
		UserNo  = 0;
		ZeroMemory( UserID, sizeof( UserID ) );  
		ZeroMemory( ChargeNo, sizeof( ChargeNo ) );  
		CanceledCashAmt= 0;
		RealCash       = 0;
		BonusCash      = 0;
		ResultCode     = 0;
		ZeroMemory( ResultMessage, sizeof( ResultMessage ) );
	}

	void Htonl()
	{
		Code    = htons( Code );       
		Length  = htons( Length );             
		Version = htons( Version );  
		ReqKey  = htonl( ReqKey );  
		GameID  = htonl( GameID );  
		PCode   = htonl( PCode );  
		Result  = htonl( Result );  
		UserNo  = htonl( UserNo );             
		CanceledCashAmt= htonl( CanceledCashAmt );
		RealCash       = htonl( RealCash );
		BonusCash      = htonl( BonusCash );
		ResultCode     = htonl( ResultCode );
	}

	void Ntohl()
	{
		Code    = ntohs( Code );       
		Length  = ntohs( Length );             
		Version = ntohs( Version );  
		ReqKey  = ntohl( ReqKey );  
		GameID  = ntohl( GameID );  
		PCode   = ntohl( PCode );  
		Result  = ntohl( Result );
		UserNo  = ntohl( UserNo );    
		CanceledCashAmt= ntohl( CanceledCashAmt );
		RealCash       = ntohl( RealCash );
		BonusCash      = ntohl( BonusCash );
		ResultCode     = ntohl( ResultCode );
	}

} SC_CNLPURCHASE;

// US //////////////////////////////////////////////////////////////////////
typedef struct _BILL_PACK_HEALTH_CHECK
{
	WORD	ReqLen;
	WORD	ReqType;
	DWORD	ReqKey;

	WORD	RetCode;

	_BILL_PACK_HEALTH_CHECK()
	{
		ReqLen  = sizeof( BILL_PACK_HEALTH_CHECK );
		ReqType = 40;
		ReqKey  = 0;
		RetCode = 0;
	}

	void Htonl()
	{
		ReqLen  = htons( ReqLen );       
		ReqType = htons( ReqType );   
		ReqKey  = htonl( ReqKey );  
		RetCode = htons( RetCode );  
	}

	void Ntohl()
	{
		ReqLen  = ntohs( ReqLen );       
		ReqType = ntohs( ReqType );   
		ReqKey  = ntohl( ReqKey );  
		RetCode = ntohs( RetCode );  
	}
}BILL_PACK_HEALTH_CHECK, *PBILL_PACK_HEALTH_CHECK;


typedef struct _BILL_PACK_GETBALANCE
{
	WORD	ReqLen;
	WORD	ReqType;
	DWORD	ReqKey;

	DWORD   UserNo;
	char    UserID[100+1];
	char    CharacterID[50+1];

	DWORD   RealCash;
	DWORD   BonusCash;
	WORD	RetCode;
	char    RetMsg[256+1];

	_BILL_PACK_GETBALANCE()
	{
		ReqLen  = sizeof( BILL_PACK_GETBALANCE );
		ReqType = 10;
		ReqKey  = 0;

		UserNo  = 0;
		ZeroMemory( UserID, sizeof( UserID ) );
		ZeroMemory( CharacterID, sizeof( CharacterID ) );

		RealCash = 0;
		BonusCash= 0;
		RetCode  = 0;
		ZeroMemory( RetMsg, sizeof( RetMsg ) );
	}

	void Htonl()
	{
		ReqLen  = htons( ReqLen );       
		ReqType = htons( ReqType );   
		ReqKey  = htonl( ReqKey );  
		UserNo  = htonl( UserNo );  
		RealCash= htonl( RealCash );  
		BonusCash= htonl( BonusCash );    
		RetCode  = htons( RetCode );  
	}

	void Ntohl()
	{
		ReqLen  = ntohs( ReqLen );       
		ReqType = ntohs( ReqType );   
		ReqKey  = ntohl( ReqKey );  
		UserNo  = ntohl( UserNo );  
		RealCash= ntohl( RealCash );  
		BonusCash= ntohl( BonusCash );
		RetCode  = ntohs( RetCode );  
	}

	void SetInfo( const char *szUserNo, const char *szUserID, const char *szCharacterID )
	{
		if( !szUserNo )
			return;
		if( !szUserID )
			return;
		if( !szCharacterID )
			return;
		UserNo = atoi( szUserNo );
		StringCbCopy( UserID, sizeof( UserID ), szUserID );
		StringCbCopy( CharacterID, sizeof( CharacterID ), szCharacterID );
	}
}BILL_PACK_GETBALANCE, *PBILL_PACK_GETBALANCE;


typedef struct _BILL_PACK_PURCHASEITEM
{
	WORD	ReqLen; 
	WORD	ReqType;
	DWORD	ReqKey;

	DWORD	UserNo;
	DWORD	ClientIP;
	WORD	ClientPort;
	char	ItemID[256+1];
	char	ItemCnt[128+1];
	char    ItemUnitPrice[256+1];
	char    ItemName[1024+1];
	WORD	GameServerNo;
	DWORD	WorldNo;
	char	UserID[100+1];
	char    CharacterID[50+1];
	char    StatProperty1[50+1];
	char    StatProperty2[50+1];
	char    StatProperty3[50+1];
	char    Location[1+1];

	DWORD	RealCash;
	DWORD	BonusCash;
	DWORD	ChargedCashAmt;
	char	ChargeNo[512+1];
	WORD	RetCode;
	char    RetMsg[256+1];

	_BILL_PACK_PURCHASEITEM()
	{
		ReqLen  = sizeof( BILL_PACK_PURCHASEITEM );
		ReqType = 20;
		ReqKey  = 0;

		UserNo  = 0;
		ClientIP= 0;
		ClientPort = 0;
		ZeroMemory( ItemID, sizeof( ItemID ) );
		StringCbCopy( ItemCnt, sizeof( ItemCnt ), "1" );
		ZeroMemory( ItemUnitPrice, sizeof( ItemUnitPrice ) );
		ZeroMemory( ItemName, sizeof( ItemName ) );
		GameServerNo = 0;
		WorldNo      = 0;
		ZeroMemory( UserID, sizeof( UserID ) );
		ZeroMemory( CharacterID, sizeof( CharacterID ) );
		ZeroMemory( StatProperty1, sizeof( StatProperty1 ) );
		ZeroMemory( StatProperty2, sizeof( StatProperty2 ) );
		ZeroMemory( StatProperty3, sizeof( StatProperty3 ) );
		StringCbCopy( Location, sizeof( Location ), "G" );

		RealCash = 0;
		BonusCash= 0;
		ChargedCashAmt = 0;
		ZeroMemory( ChargeNo, sizeof( ChargeNo ) );
		RetCode  = 0;
		ZeroMemory( RetMsg, sizeof( RetMsg ) );
	}

	void Htonl()
	{
		ReqLen  = htons( ReqLen );       
		ReqType = htons( ReqType );   
		ReqKey  = htonl( ReqKey );  
		UserNo  = htonl( UserNo );  
		ClientIP= htonl( ClientIP );  
		ClientPort= htons( ClientPort );  
		GameServerNo= htons( GameServerNo );  
		WorldNo  = htonl( WorldNo );  
		RealCash = htonl( RealCash );  
		BonusCash= htonl( BonusCash );    
		ChargedCashAmt= htonl( ChargedCashAmt );    
		RetCode  = htons( RetCode );  
	}

	void Ntohl()
	{
		ReqLen  = ntohs( ReqLen );       
		ReqType = ntohs( ReqType );   
		ReqKey  = ntohl( ReqKey );  
		UserNo  = ntohl( UserNo );  
		ClientIP= ntohl( ClientIP );  
		ClientPort= ntohs( ClientPort );  
		GameServerNo= ntohs( GameServerNo );  
		WorldNo     = ntohl( WorldNo );  
		RealCash    = ntohl( RealCash );  
		BonusCash   = ntohl( BonusCash );
		ChargedCashAmt= htonl( ChargedCashAmt );    
		RetCode       = ntohs( RetCode );  
	}

	void SetInfo( const char *szUserNo, const char *szUserID, const char *szCharacterID, DWORD dwClientIP, WORD wClientPort, DWORD dwItemID, int iPrice, const char *szItemName )
	{
		if( !szUserNo )
			return;
		if( !szUserID )
			return;
		if( !szCharacterID )
			return;
		if( !szItemName )
			return;

		UserNo     = atoi( szUserNo );
		ClientIP   = dwClientIP;
		ClientPort = wClientPort;

		StringCbCopy( UserID, sizeof( UserID ), szUserID );
		StringCbCopy( CharacterID, sizeof( CharacterID ), szCharacterID );
		StringCbPrintf( ItemID, sizeof( ItemID ), "%u", dwItemID );
		StringCbPrintf( ItemUnitPrice, sizeof( ItemUnitPrice ), "%d", iPrice );
		StringCbCopy( ItemName, sizeof( ItemName ), szItemName );
	}
}BILL_PACK_PURCHASEITEM, *PBILL_PACK_PURCHASEITEM;


typedef struct _BILL_PACK_CNLPURCHASE
{
	WORD	ReqLen; 
	WORD	ReqType;
	DWORD	ReqKey;
	DWORD	UserNo;
	char    UserID[100+1];
	char	CharacterID[50+1];
	char	ChargeNo[512+1];

	DWORD	RealCash;
	DWORD	BonusCash;
	DWORD	CanceledCashAmt;
	WORD	RetCode;
	char    RetMsg[256+1];

	_BILL_PACK_CNLPURCHASE()
	{
		ReqLen  = sizeof( BILL_PACK_CNLPURCHASE );
		ReqType = 22;
		ReqKey  = 0;
		UserNo  = 0;
		ZeroMemory( UserID, sizeof( UserID ) );
		ZeroMemory( CharacterID, sizeof( CharacterID ) );
		ZeroMemory( ChargeNo, sizeof( ChargeNo ) );

		RealCash = 0;
		BonusCash= 0;
		CanceledCashAmt = 0;
		RetCode  = 0;
		ZeroMemory( RetMsg, sizeof( RetMsg ) );
	}

	void Htonl()
	{
		ReqLen  = htons( ReqLen );       
		ReqType = htons( ReqType );   
		ReqKey  = htonl( ReqKey );  
		UserNo  = htonl( UserNo );  
		RealCash = htonl( RealCash );  
		BonusCash= htonl( BonusCash );    
		CanceledCashAmt= htonl( CanceledCashAmt );    
		RetCode  = htons( RetCode );  
	}

	void Ntohl()
	{
		ReqLen  = ntohs( ReqLen );       
		ReqType = ntohs( ReqType );   
		ReqKey  = ntohl( ReqKey );  
		UserNo  = ntohl( UserNo );  
		RealCash    = ntohl( RealCash );  
		BonusCash   = ntohl( BonusCash );
		CanceledCashAmt= htonl( CanceledCashAmt );    
		RetCode       = ntohs( RetCode );  
	}

	void SetInfo( const char *szUserNo, const char *szUserID, const char *szChargeNo )
	{
		if( !szUserNo )
			return;
		if( !szUserID )
			return;
		if( !szChargeNo )
			return;

		UserNo     = atoi( szUserNo );

		StringCbCopy( UserID, sizeof( UserID ), szUserID );
		StringCbCopy( ChargeNo, sizeof( szChargeNo ), szChargeNo );
	}
}BILL_PACK_CNLPURCHASE, *PBILL_PACK_CNLPURCHASE;

// Client -> Server
//kyg 한번 봐볼것 
typedef struct tagCS_RETRACT_PAYBACK : public PHEADER 
{
	DWORD	UserNo;
	char	UserID[64];
	char	ClientIP[16];
	char	ChargeNo[512];

	tagCS_RETRACT_PAYBACK()
	{
		Code    = 25; //청역철회조회
		Length  = sizeof( tagCS_RETRACT_PAYBACK );
		Version = 0;
		ReqKey  = 0;
		GameID  = WEMADE_BUY_LOSTSAGA_GAME_NO;
		PCode   = WEMADE_BUY_LOSTSAGA_PCODE;
		Result  = 0;
		UserNo  = 0;
		ZeroMemory( UserID, sizeof( UserID ) ); 
		ZeroMemory( ClientIP, sizeof( ClientIP ) ); 
		ZeroMemory( ChargeNo, sizeof( ChargeNo ) ); 
	};

	void SetInfo( DWORD dwReqKey, DWORD dwUserNo, const char *szUserID, const char *szUserIP, const char* szChargeNo )
	{
		if( !szUserID )
			return;
		if( !szUserIP )
			return;
		if( !szChargeNo )
			return;

		ReqKey = dwReqKey;
		UserNo = dwUserNo;
		StringCbCopy( UserID, sizeof( UserID ), szUserID );
		StringCbCopy( ClientIP, sizeof( ClientIP ), szUserIP );
		StringCbCopy( ChargeNo, sizeof( ChargeNo ), szChargeNo );
	};
}CS_RETRACT_PAYBACK, *pCS_RETRACT_PAYBACK;

// Server -> Client
typedef struct tagSC_RETRACT_PAYBACK : public PHEADER  {
	DWORD	UserNo;
	char	UserID[64];
	char	ChargeNo[512];
	DWORD	ChargedAmt;
	DWORD	RealChargedAmt;
	DWORD	BonusChargedAmt;
	DWORD	CanceledAmt;
	DWORD	RealCash;
	DWORD	BonusCash;
	DWORD	ResultCode;
	char	ResultMessage[256];

	tagSC_RETRACT_PAYBACK()
	{
		Code    = 25; //청역철회조회
		Length  = sizeof( tagSC_RETRACT_PAYBACK );
		Version = 0;
		ReqKey  = 0;
		GameID  = WEMADE_BUY_LOSTSAGA_GAME_NO;
		PCode   = WEMADE_BUY_LOSTSAGA_PCODE;
		Result  = 0;

		UserNo  = 0;
		ZeroMemory( UserID, sizeof( UserID ) ); 
		ZeroMemory( ChargeNo, sizeof( ChargeNo ) ); 

		ChargedAmt = 0;
		RealChargedAmt = 0;
		BonusChargedAmt = 0;
		CanceledAmt = 0;
		RealCash = 0;
		BonusCash = 0;
		ResultCode = 0;

		ZeroMemory( ResultMessage, sizeof( ResultMessage) );
	}

}SC_RETRACT_PAYBACK, *pSC_RETRACT_PAYBACK;

typedef struct tagCS_RETRACT : public PHEADER 
{
	DWORD	UserNo;
	char	UserID[64];
	char	ClientIP[16];
	char	ChargeNo[512];
	WORD	CancelFlag;

	tagCS_RETRACT()
	{
		Code    = 24; //청역철회
		Length  = sizeof( tagCS_RETRACT );
		Version = 0;
		ReqKey  = 0;
		GameID  = WEMADE_BUY_LOSTSAGA_GAME_NO;
		PCode   = WEMADE_BUY_LOSTSAGA_PCODE;
		Result  = 0;

		UserNo  = 0;
		CancelFlag = 3; // 유저 취소 기본값

		ZeroMemory( UserID, sizeof( UserID ) ); 
		ZeroMemory( ClientIP, sizeof( ClientIP ) ); 
		ZeroMemory( ChargeNo, sizeof( ChargeNo ) ); 
	};

	void SetInfo( DWORD dwReqKey, DWORD dwUserNo, const char *szUserID, const char *szUserIP, const char*  szChargeNo, WORD wCancelFalg = 3)
	{
		if( !szUserID )
			return;
		if( !szUserIP )
			return;
		if( !szChargeNo )
			return; 

		ReqKey = dwReqKey;
		UserNo = dwUserNo;
		StringCbCopy( UserID, sizeof( UserID ), szUserID );
		StringCbCopy( ClientIP, sizeof( ClientIP ), szUserIP );
		StringCbCopy( ChargeNo, sizeof( ChargeNo ), szChargeNo );

		CancelFlag = wCancelFalg;
	};

}CS_RETRACT, *pCS_RETRACT;

typedef struct tagSC_RETRACT : public PHEADER  {
	DWORD	UserNo;
	char	UserID[64];
	char	ChargeNo[512];
	DWORD	CanceledCashAmt;
	DWORD	RealCash;
	DWORD	BonusCash;
	DWORD	ResultCode;
	char	ResultMessage[256];

	tagSC_RETRACT()
	{
		Code    = 24; //청역철회
		Length  = sizeof( tagSC_RETRACT );
		Version = 0;
		ReqKey  = 0;
		GameID  = WEMADE_BUY_LOSTSAGA_GAME_NO;
		PCode   = WEMADE_BUY_LOSTSAGA_PCODE;
		Result  = 0;

		UserNo = 0;
		ZeroMemory( UserID, sizeof(UserID) );
		ZeroMemory( ChargeNo, sizeof(ChargeNo) );
		CanceledCashAmt = 0;
		RealCash = 0;
		BonusCash = 0;
		ResultCode = 0;
		ZeroMemory (ResultMessage, sizeof(ResultMessage) );
	};

	void SetInfo( DWORD dwUserNo, const char *szUserID, const char* szChargeNo, DWORD dwCanceledCashAmt, DWORD dwRealCash, DWORD dwBonusCash, DWORD dwResultCode, const char* szResultMessage )
	{
		if( !szUserID )
			return;
		if( !szChargeNo )
			return;

		UserNo = dwUserNo;
		StringCbCopy( UserID, sizeof( UserID ), szUserID );
		StringCbCopy( ChargeNo, sizeof( ChargeNo ), szChargeNo );
		 
		CanceledCashAmt = dwCanceledCashAmt;
		RealCash = dwRealCash;
		BonusCash = dwBonusCash;
		ResultCode = dwResultCode;
		StringCbCopy( ResultMessage, sizeof( ResultMessage ), szResultMessage );
	};

}SC_RETRACT, *pSC_RECTRACT;

typedef struct tagCS_RETRACT_CANCEL : public PHEADER 
{
	DWORD	UserNo;
	char	UserID[64];
	char	ClientIP[16];
	char	ChargeNo[512];
	WORD	CancelFlag;

	tagCS_RETRACT_CANCEL()
	{
		Code    = 26; //청역철회취소
		Length  = sizeof( tagCS_RETRACT_CANCEL );
		Version = 0;
		ReqKey  = 0;
		GameID  = WEMADE_BUY_LOSTSAGA_GAME_NO;
		PCode   = WEMADE_BUY_LOSTSAGA_PCODE;
		Result  = 0;

		UserNo  = 0;
		CancelFlag = 0;

		ZeroMemory( UserID, sizeof( UserID ) ); 
		ZeroMemory( ClientIP, sizeof( ClientIP ) ); 
		ZeroMemory( ChargeNo, sizeof( ChargeNo ) ); 
	};

	void SetInfo( DWORD dwUserNo, const char *szUserID, const char *szUserIP, const char* szChargeNo, WORD wCancelFalg )
	{
		if( !szUserID )
			return;
		if( !szUserIP )
			return;

		UserNo = dwUserNo;
		StringCbCopy( UserID, sizeof( UserID ), szUserID );
		StringCbCopy( ClientIP, sizeof( ClientIP ), szUserIP );
		StringCbCopy( ChargeNo, sizeof( ChargeNo ), szChargeNo );

		CancelFlag = wCancelFalg;
	};

}CS_RETRACT_CANCEL, *pCS_RETRACT_CANCEL;

typedef struct tagSC_RETRACT_CANCEL : public PHEADER  {
	DWORD	UserNo;
	char	UserID[64];
	char	ChargeNo[512];
	DWORD	CanceledCashAmt;
	DWORD	RealCash;
	DWORD	BonusCash;
	DWORD	ResultCode;
	char	ResultMessage[256];

	tagSC_RETRACT_CANCEL()
	{
		Code    = 26; //청역철회취소
		Length  = sizeof( tagSC_RETRACT_CANCEL );
		Version = 0;
		ReqKey  = 0;
		GameID  = WEMADE_BUY_LOSTSAGA_GAME_NO;
		PCode   = WEMADE_BUY_LOSTSAGA_PCODE;
		Result  = 0;

		UserNo = 0;
		ZeroMemory( UserID, sizeof(UserID) );
		ZeroMemory( ChargeNo, sizeof(ChargeNo) );
		CanceledCashAmt = 0;
		RealCash = 0;
		BonusCash = 0;
		ResultCode = 0;
		ZeroMemory (ResultMessage, sizeof(ResultMessage) );
	};

	void SetInfo( DWORD dwUserNo, const char *szUserID, const char* szChargeNo, DWORD dwCanceledCashAmt, DWORD dwRealCash, DWORD dwBonusCash, DWORD dwResultCode, const char* szResultMessage )
	{
		if( !szUserID )
			return;
		if( !szChargeNo )
			return;

		UserNo = dwUserNo;
		StringCbCopy( UserID, sizeof( UserID ), szUserID );
		StringCbCopy( ChargeNo, sizeof( ChargeNo ), szChargeNo );

		CanceledCashAmt = dwCanceledCashAmt;
		RealCash = dwRealCash;
		BonusCash = dwBonusCash;
		ResultCode = dwResultCode;
		StringCbCopy( ResultMessage, sizeof( ResultMessage ), szResultMessage );
	};

}SC_RETRACT_CANCEL, *pSC_RETRACT_CANCEL;


#pragma pack()


/************************************************************************/
/* Operation Structure                                                                     */
/************************************************************************/
#define OPERATIONRUN(x)\
	SP2Packet st(ITPK_OPERATION);\
	st.Write(x);\
	g_RecvQueue.InsertQueue(NULL,st,PK_QUEUE_INTERNAL);

class CConnectNode;
class SchedulerNode;

struct stOp_ 
{
	int opid;
	
	stOp_() { opid = 0; };
	stOp_(int OpertationID)	{ opid = OpertationID; };
};

struct Tedata_ : public stOp_
{
	TCHAR m_IpAddr[256];
	int serverId;
	int freeCount;
	Tedata_() 
	{
		stOp_(OPID_TESTOPERATION);
		ZeroMemory(m_IpAddr,256);
		serverId =0;
		freeCount = 0;
	}
};

struct OnAccept_ : public stOp_
{
	CConnectNode* node;
};


struct OnConnect_ : public stOp_
{
	CConnectNode* node;
	OnConnect_()
	{
		opid = OPID_ONCONNECT;
		node = NULL;
	};
};

struct SchedulerOperation_ : public stOp_
{
	SchedulerNode* node;
	int eoperation;
};

struct WriteProcessLog_ : public stOp_
{
	SchedulerNode* node;
	int nagleTime;
	int code;
	WriteProcessLog_()
	{
		opid = OPID_WRITEPROCESSLOG;
	}
};

struct WaitClient_ : public stOp_
{
	int token;
	int mineId;
	unsigned int time;
};

struct SendToken_ : public stOp_
{
	CConnectNode* node;
	int serverId;
	TCHAR strserverip[256];
	int port;

};

struct OnClient_ 
{
	TCHAR userid[256];
	int  port;
};

struct TrySendServerInfo_ : public stOp_
{

};

struct TestClosedata_ : public stOp_
{
	TCHAR m_IpAddr[256];
	int serverId;
	int freeCount;
	TestClosedata_() 
	{
		opid = OPID_TESTCLOSEOPERATION;
		ZeroMemory(m_IpAddr,256);
		serverId =0;
		freeCount = 0;
	}
};


/************************************************************************/
/* UserInfomapdef                                                                     */
/************************************************************************/
struct NexonUserInfo
{
	uint64_t		 sessionNo;
	ioHashString	 privateID;
	ioHashString	 chanID;
	DWORD			 userIndex;
	DWORD			 serverIndex;
	ioHashString	 serverIP;
	int				 serverPort;
	ioHashString     publicIP;
	ioHashString     privateIP;
	int				 chType;
	int				 pcRoomState;		//PC 방 접속 여부

	NexonUserInfo()
	{
		InitData();
	}

	void InitData()
	{
		sessionNo = 0;
		privateID.Clear();
		chanID.Clear();
		userIndex = 0;
		serverIndex = 0;
		serverIP.Clear();
		serverPort = 0;
		chType = 0;
		publicIP.Clear();
		pcRoomState = 0;
	}	
};
#endif

