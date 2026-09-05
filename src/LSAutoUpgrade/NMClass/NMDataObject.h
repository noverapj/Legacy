#ifndef __NMDATAOBJECT_H_51C2349E_4DF4_4898_AEBD_E86B4E28CA3F__
#define __NMDATAOBJECT_H_51C2349E_4DF4_4898_AEBD_E86B4E28CA3F__

#include "NMSerializable.h"
#include "NMDataTypes.h"

using namespace Nexon::Platform;

// String Length Define
#define DOMAIN_SIZE			(1024)
#define	PASSWORD_SIZE		(128)
#if defined (_NMSERVICE) || defined (_NMSERVER)
	#define NICKNAME_SIZE	(32)
	#define LOGINID_SIZE	(32)
#else
	#define NICKNAME_SIZE	(64)
	#define LOGINID_SIZE	(64)
#endif
#define	LARGE_LOGINID_SIZE	(256)
#define	PASSPORT_SIZE		(4096)
#define	NXPASSPORT_SIZE		(1024)
#define SECURITYCN_SIZE		(1024)
#define	TODAYWORD_SIZE		(64)
#define	MEMO_SIZE			(32)
#define	NXNAME_SIZE			(32)
#define	NXAREA_SIZE			(32)
#define	NXSCHOOL_SIZE		(128)
#define	NXTEL_SIZE			(32)
#define	CATEGORY_SIZE		(32)
#define	POSITION_SIZE		(256)
#ifdef _NMSERVER
	#define METADATA_SIZE		(64)
#else
	#define	METADATA_SIZE		(1000)
#endif
#define LAYER_SIZE			(40)
#define	MSG_SIZE			(256)
#define	CHATSENDER_SIZE		(64)
#define CHATHDR_SIZE		(256)
#define CHATMSG_SIZE		(1024)
#define	SERVER_MSG_SIZE		(4096)
#define	TARGETID_SIZE		(1024)
#define	NOTEMSG_SIZE		(2048)
#define	NOTEARG_SIZE		(256)
#define	NOTECUSTOM_SIZE		(256)
#define	NOTEEXMSG_SIZE		(32768)
#define MYGAME_COUNT		(10)
#define	FULL_LAYER_SIZE		(38)
#define	FILENAME_SIZE		(256)
#define	GUILDNAME_SIZE		(64)
#define	GUILDID_SIZE		(32)
#define	GUILDHOME_SIZE		(256)
#define GUILDLEVEL_SIZE		(64)
#define	ROOMTITLE_SIZE		(1024)
#define	ROOMPWD_SIZE		(64)
#define IPADDR_SIZE			(32)
#define P2PDATA_SIZE		(4096)
#define EXTRADATA_SIZE		(256)
#define P2PRECEIVERS_SIZE	(32)
#define NXHWID_SIZE			(64)
#define SECUREDATA_SIZE		(32)

// Locale ID
#ifndef _NMLOCALEID_DEFINED_
#define _NMLOCALEID_DEFINED_
enum NMLOCALEID
{
	kLocaleID_Null			= 0x00000000,

	kLocaleID_KR			= 0x00000001,	//	Korea, Republic of
	kLocaleID_KR_Test		= 0x10000001,	//	Test

	kLocaleID_JP			= 0x00000100,	//	Japan
	kLocaleID_CN			= 0x00000101,	//	China(Tiancity)
	kLocaleID_TW			= 0x00000102,	//	Taiwan
	kLocaleID_TH			= 0x00000103,	//	Thailand
	kLocaleID_VN			= 0x00000104,	//	Vietnam
	kLocaleID_SG			= 0x00000105,	//	Singapore
	kLocaleID_ID			= 0x00000106,	//	Indonesia
	kLocaleID_CN2			= 0x00000107,	//	China BF(Shanda)
	kLocaleID_ID2			= 0x00000108,	//	Indonesia(Kreon)
	kLocaleID_ID_Kreon		= kLocaleID_ID2,
	kLocaleID_TH2			= 0x00000109,	//	Thailand(AsiaSoft)
	kLocaleID_ID3			= 0x0000010A,	//	Indonesia(AsiaSoft)

	kLocaleID_US			= 0x00000200,	//	United States
	kLocaleID_EU			= 0x00000300,	//	Europe
	kLocaleID_RU			= 0x00000301,	//	Russia
	kLocaleID_RU2			= 0x00000302,	//	Russia(Syncopate)
	kLocaleID_BR			= 0x00000400,	//	Brazil
	//
	//	DO NOT USE THIS CODE! (Only for backward compatibility)
	//
	kLocaleID_CN_CNC		= 0x00000111,	//	China / CNC
	kLocaleID_CN_CT			= 0x00000112,	//	China / CT
};
#endif

//
//	Region Code
//
#ifndef _NMREGIONCODE_DEFINED_
#define _NMREGIONCODE_DEFINED_
enum NMREGIONCODE
{
	kRegionCode_Default		= 0,
	kRegionCode_CT1			= 1,			//	China public CT1
	kRegionCode_CT2			= 2,			//	China public CT2
	kRegionCode_CT3			= 3,			//	China kart CT1
	kRegionCode_CT4			= 4,			//	China kart CT2
	kRegionCode_CT5			= 5,			//	China cso CT1
	kRegionCode_CT6			= 6,			//	China cso CT2
	kRegionCode_CT7			= 7,			//	China cso CT3
	kRegionCode_CNC1		= 11,			//	China public CNC1
	kRegionCode_CNC2		= 12,			//	China public CNC2
	kRegionCode_CNC3		= 13,			//	China kart CNC1
	kRegionCode_CNC4		= 14,			//	China kart CNC2
	kRegionCode_CNC5		= 15,			//	China cso CNC1
	kRegionCode_CNC6		= 16,			//	China cso CNC2

	kRegionCode_CGZ1		= 30,			//  China Mabinogi Heroes CT1 // 광주 6.3
	kRegionCode_CGZ2		= 31,			//  China Mabinogi Heroes CT2 // 광주 -?
	kRegionCode_CQD1		= 32,			//  China Mabinogi Heroes CNC1 // 청도 6.3
	kRegionCode_CQD2		= 33,			//  China Mabinogi Heroes CNC2 // 청도 6.3
	kRegionCode_CSH1		= 34,			//  China Mabinogi Heroes CSH1 // 상해1 6.3
	kRegionCode_CSH2		= 35,			//  China Mabinogi Heroes CSH2 // 상해2 6.3 -> 카단 (ServerCode:21) 로 변경
	kRegionCode_CTJ1		= 36,			//  China Mabinogi Heroes CNC3 // 천진 6.17
	kRegionCode_CTJ2		= 37,			//  China Mabinogi Heroes CNC4 // 천진 -?
	kRegionCode_CCD1		= 38,			//  China Mabinogi Heroes CT5 // 성도1 6.24
	kRegionCode_CCD2		= 39,			//  China Mabinogi Heroes CT6 // 성도2 6.24
	kRegionCode_CXA1		= 40,			//  China Mabinogi Heroes ? // 시안
	kRegionCode_CSJZ1		= 41,			//  China Mabinogi Heroes ? // 석가장
	kRegionCode_CSX1		= 42,			//  China Mabinogi Heroes ? // 섬서
	kRegionCode_COT1		= 43,			//	China Mabinogi Heroes OT1 // 오픈테스트서버.

	kRegionCode_NPNA1		= 100,			//	NPNA
	kRegionCode_NXA1		= 200,			//	NXA	 (CombatArms)
	kRegionCode_NXA2		= 201,			//	NXA2 (DF, etc...)
	kRegionCode_NXA3		= 202,			//	NXA3 (Vindictus West)
	kRegionCode_NXA4		= 203,			//	NXA4 (Vindictus East)
	kRegionCode_NXA5		= 204,			//	NXA5 (Vindictus AU)
	kRegionCode_NXA6		= 205,			//	NXA6 (Vindictus Staging)
	kRegionCode_TW2			= 300,			//	Taiwan CSO
	kRegionCode_TW3			= 301,			//	Taiwan MH Zone #1 
	kRegionCode_TW4			= 302,			//	Taiwan MH Zone #2
	kRegionCode_TW5			= 303,			//	Taiwan BubbleFighter
	kRegionCode_TW6			= 304,			//	Taiwan CSO2
	kRegionCode_TH1			= 400,			//	Thailand
	kRegionCode_TH2			= 401,			//	Thailand (THCSO)
	kRegionCode_TH3			= 402,			//	Thailand (Everplanet)
	kRegionCode_JP2			= 500,			//	Japan 2nd (1st: lunia, 2nd: Tenvi & Atlantica & ... )
	kRegionCode_JP3			= 501,			//	Japan 3rd (Arad)
	kRegionCode_JP4			= 502,			//	Japan 4th (TalesWeaver)
	kRegionCode_JP5			= 503,			//	Japan 5th (CSO)
	kRegionCode_JP6			= 504,			//	Japan 6th - new combined SSO
	kRegionCode_JP7			= 505,			//	Japan 7th (EverPlanet)
	kRegionCode_JP8			= 506,			//	Japan 8th (Vindictus/영웅전)
	kRegionCode_JP9			= 507,			//	Japan 9th (jpKart)
	kRegionCode_JP10		= 508,			//	Japan 10th (new expanded SSO - 2013/05/02)
	kRegionCode_EU1			= 600,			//	Europe 
	kRegionCode_EU2			= 601,			//	Europe (Vindictus/영웅전)
	kRegionCode_EU3			= 602,			//	Europe (Dekaron, ATRLANTICA, WARROCK, NAVYFIELD2)
	kRegionCode_EU			= kRegionCode_EU3,	//	Europe Common
	kRegionCode_EU4			= 603,			//	Europe (TURCSO)
	kRegionCode_VN1			= 700,			//	Vietnam (Kart)
	kRegionCode_RU1			= 800,			//	Russia (Kart)
	kRegionCode_RU2			= 801,			//	Russia (Combat)
	kRegionCode_BR1			= 900,			//	Brazil (Combat)
	kRegionCode_SG1			= 1000,			//	Singapore (CSO)
	kRegionCode_SNDA1		= 1100,			//	China BF (Shanda/남경(전신) IDC)
	kRegionCode_SNDA2		= 1101,			//	China BF (Shanda/광주IDC) // 폐쇄
	kRegionCode_SNDA3		= 1102,			//	China BF (Shanda/성도IDC) // 폐쇄
	kRegionCode_SNDA4		= 1103,			//	China BF (Shanda/북경(왕통) IDC)
	kRegionCode_SNDA5		= 1104,			//	China BF (Shanda/북경2 IDC) // 폐쇄
	kRegionCode_SNDA6		= 1105,			//	China BF (Shanda/무한 IDC)
	kRegionCode_ID1			= 1200,			//	Indonesia (CSO)
	kRegionCode_ID2			= 1201,			//	Indonesia (Kart) - (KREON)
	kRegionCode_ID3			= 1202,			//	Indonesia (EverPlanet) - (AsiaSoft)
	kRegionCode_IDBF		= 1203,			//	Indonesia Kreon BubbleFighter
};
#endif

//
//	Channel Code
//
#ifndef _NMCHANNELCODE_DEFINED_
#define _NMCHANNELCODE_DEFINED_
enum	NMCHANNELCODE
{
	kChannelCode_Default	= 0,
	kChannelCode_THAS		= 1001,
	kChannelCode_THTC		= 1002,
	kChannelCode_THPP		= 1003,
};
#endif

//
//	Guild Type
//
#ifndef _NMGUILDTYPE_DEFINED_
#define _NMGUILDTYPE_DEFINED_
enum	NMGUILDTYPE
{
	kGuildType_None			= 0,
	kGuildType_NexonDefault	= 1,
	kGuildType_NexonNew		= 2,
	kGuildType_CSO			= 3,
};
#endif

//
//	AuthCheck module type
//
#ifndef _NMAUTHCHECKMODULETYPE_DEFINED_
#define _NMAUTHCHECKMODULETYPE_DEFINED_
enum	NMAUTHCHECKMODULETYPE
{
	kAuthCheckModuleType_Unknown		= 0,	//	Unknown
	kAuthCheckModuleType_NexonPassport	= 1,	//	Nexon passport checker
	kAuthCheckModuleType_SpServer		= 2,	//	SpServerDll.dll (Chinese Kartrider, etc.)
	kAuthCheckModuleType_AppDll			= 3,	//	Alpha Passport (Chinese BubbleFighter, etc.)
};
#endif

//
//	Character Converter Type
//
#ifndef _NMCONVERTERTYPE_DEFINED_
#define _NMCONVERTERTYPE_DEFINED_
enum	NMCONVERTERTYPE
{
	kCharacterConverter_Direct			= 0,
	kCharacterConverter_UserWeb			= 1,
	kCharacterConverter_CharacterWeb	= 2,
	kCharacterConverter_GameUserLib		= 3,
};
#endif

// Enumeration Types
enum	NMUSERSTATUS
{
	kStatus_Unknown				= 0,

	kStatus_Online				= 10,		//	인증 OK, 메신저 OK
	kStatus_Busy				= 11,
	kStatus_Away				= 12,
	kStatus_AppearOffline		= 13,
	kStatus_Offline				= 14,		//	인증 --, 메신저 --
	kStatus_GameOnline			= 15,
	kStatus_Error				= 16,
	kStatus_Authenticated		= 17,		//	인증 OK, 메신저 --

	kStatus_NexonPlay_Online	= 18,		//	넥슨플레이 온라인
	kStatus_NexonPlay_Offline	= 19,		//	넥슨플레이 오프라인
	kStatus_DualOnline			= 20		//	PC,넥슨플레이 온라인
};

enum	NMSEXTYPE
{
	kSex_Unknown		= 0,

	kSex_Male			= 1,
	kSex_Female			= 2,
};

enum	NMURLTYPE
{
	kURLType_NULL		= 0,

	kURLType_Page		= 1,
	kURLType_Game		= 2,
	kURLType_Guild		= 3,
	kURLType_DirectURL	= 4,
};

enum	NMURLPAGE
{
	kURL_NULL						= 0,

	kURL_MainHome					= 1,
	kURL_WebGameHome				= 2,
	kURL_RPGGameHome				= 3,
	kURL_CasualGameHome				= 4,
	kURL_ClubHome					= 5,
	kURL_PlazaHome					= 6,
	kURL_ItemMallHome				= 7,
	kURL_MyPage						= 8,
	kURL_Support					= 9,
	kURL_Help						= 10,
	kURL_Register					= 11,
	kURL_MyGiftBox					= 12,
	kURL_NXPaybill					= 13,
	kURL_MemberPopup_OID			= 14,
	kURL_AgreementPopup				= 15,
	kURL_PrivacyPopup				= 16,
	kURL_SearchIDPopup				= 17,
	kURL_SearchPass					= 18,
	kURL_MoneyRefillForWebGame		= 19,	// uParam = game code
	kURL_MyRoomForWegGame			= 20,
	kURL_ChangeClothes				= 21,
	kURL_ChangeMyInfo				= 22,
	kURL_MailHome					= 23,
	kURL_WriteMail_General			= 24,
	kURL_WriteMail_Target			= 25,
	kURL_MemberPopup_LoginID		= 26,
	kURL_Penalty					= 27,
	kURL_KartBox_OID				= 28,
	kURL_KartBox_RiderID			= 29,
	kURL_KartBox_RiderName			= 30,
	kURL_NGMInstallGuide			= 31,
	kURL_P2Home						= 32,
	kURL_P2Profile					= 33,
};

enum	NMSESSIONTYPE
{
	kSessionType_General			= 0,
	kSessionType_Chat				= 1,
	kSessionType_FileUpload			= 2,
	kSessionType_FileDownload		= 3,
	kSessionType_WhiteBoard			= 4,

	kSessionType_NexonPlay			= 5
};

enum	NMSYSTEMMSGCODE
{
	kSystemMsgCode_NULL					= 0,

	kSystemMsgCode_AdminNotice			= 1,
	kSystemMsgCode_Advertisement		= 2,
	kSystemMsgCode_ErrorMessage			= 3,
	kSystemMsgCode_GeneralMessage		= 4,
	kSystemMsgCode_WebNotice			= 5,
	kSystemMsgCode_Notify				= 6,
	kSystemMsgCode_ConnectionClose		= 7,

	kCustomMsgCode_RequestFriendOK		= 1,
	kCustomMsgCode_RequestFriendFailed	= 2,
	kCustomMsgCode_NoteReply			= 3,
};

enum	NMREJECTCODE
{
	kRejectCode_Unknown				= 0,

	kRejectCode_Reject				= 1,
	kRejectCode_NotOnline			= 2,
	kRejectCode_NoResponse			= 3,
	kRejectCode_NoSession			= 4,
	kRejectCode_NotSupported		= 5,
};

enum	NMCONFIRMCODE
{
	kConfirmUnknown					= 0,

	kConfirmOK						= 1,
	kConfirmDenied					= 2,
	kConfirmLater					= 3,

	kConfirmNext					= 4,
};

enum	NMFILESTATUS
{
	kFileStatus_Unknown				= 0,

	kFileStatus_WaitSend			= 1,
	kFileStatus_SendFile			= 2,
	kFileStatus_SendComplete		= 3,
	kFileStatus_WaitConfirm			= 4,
	kFileStatus_GetFile				= 5,
	kFileStatus_GetComplete			= 6,

	kFileStatus_FileOpenError		= 100,
	kFileStatus_FileReadError		= 101,
	kFileStatus_FileWriteError		= 102,
	kFileStatus_NetworkError		= 103,
	kFileStatus_SessionError		= 104,
	kFileStatus_CantSupport			= 105,

	kFileStatus_UserAbort			= 200,
};

enum	NMNOTEBOXTYPE
{
	kNoteBoxType_NULL				= 0,

	kNoteBoxType_Volatile			= 1,
	kNoteBoxType_TempBox			= 2,
	kNoteBoxType_PermBox			= 3,

	kNoteBoxType_RecvBox			= 10,
	kNoteBoxType_SendBox			= 11,
	kNoteBoxType_KeepBox			= 12,
};

enum	NMNOTEOPERATION
{
	kNoteOperation_NULL				= 0,

	kNoteOperation_SetReadFlag		= 1,
	kNoteOperation_DeleteTempNote	= 2,
	kNoteOperation_DeletePermNote	= 3,
	kNoteOperation_SaveToPermBox	= 4,
};

enum	NMCONFIGTYPE
{
	kConfigType_NULL				= 0,

	// Save to Server
	kConfigType_AllowChat			= 1,
	kConfigType_AllowNote			= 2,
	kConfigType_AllowGameInvite		= 3,
	kConfigType_AllowFileShare		= 4,
	kConfigType_AllowFriendRequest	= 5,
};

enum	NMREFRESHTYPE
{
	kRefresh_NULL					= 0,

	kRefresh_Avatar					= 1,
	kRefresh_GameMoney				= 2,
	kRefresh_NickName				= 3,
	kRefresh_Config					= 4,

	kRefresh_GuildInfo				= 100,
	kRefresh_GuildList				= 101,
};

enum	NMALLOWTYPE
{
	kConfigAllowValue_Unknown				= 0,

	kConfigAllowValue_FromAll				= 1,
	kConfigAllowValue_FromFriend			= 2,
	kConfigAllowValue_FromNoBody			= 3,
	kConfigAllowValue_FromFriendOrMatched	= 4,
};

enum	NMLOCALCONFIGTYPE
{
	kLocalConfig_NULL				= 0,

	kLocalConfig_FileDownloadDir,
	kLocalConfig_LogChatMsg,
	kLocalConfig_ChatLogDir,
	kLocalConfig_ShowGameStatus,

	kLocalConfig_FilterMyMessage,
	kLocalConfig_FilterOtherMessage,

	kLocalConfig_AdToastRemindDate,

	kLocalConfig_HotClipRemindDate,
	kLocalConfig_ShowHotClip,

	kLocalConfig_Start				= kLocalConfig_FileDownloadDir,
	kLocalConfig_End				= kLocalConfig_ShowHotClip,
	kLocalConfig_Count				= ( kLocalConfig_End - kLocalConfig_Start + 1 ),
};

enum	NMCATEGORYPROPERTY
{
	kCateProp_None					= 0,
	kCateProp_DefaultCategory		= 1 << 0,
	kCateProp_CateNotDelete			= 1 << 1,
	kCateProp_CateNotRename			= 1 << 2,
	kCateProp_UserNotAdd			= 1 << 3,
	kCateProp_UserNotDelete			= 1 << 4,
};

#define	IS_DEFAULTCATEGORY( x )		( ( x & kCateProp_DefaultCategory ) ? TRUE : FALSE )
#define	SET_DEFAULTCATEGORY( x )	{ x |= ( kCateProp_DefaultCategory | kCateProp_CateNotDelete );\
									  x &= ~( kCateProp_UserNotAdd | kCateProp_UserNotDelete ); }
#define	RESET_DEFAULTCATEGORY( x )	{ x &= ~( kCateProp_DefaultCategory | kCateProp_CateNotDelete ); }


enum
{
	kUserFlag_NULL				= 0x0000000,

	kUserFlag_Blocked			= 0x00000001,
	kUserFlag_WaitingConfirm	= 0x00000002,
	kUserFlag_AddNxFriend		= 0x00000004,

	// For Virtual Friend Only. Do not save to DB
	kUserFlag_LevelUp			= 0x00000010,
	kUserFlag_LicenseUp			= 0x00000020,
	
	kUserFlag_GeneralLevelUp	= 0x00000100,
	kUserFlag_PVPLevelUp		= 0x00000200,
	kUserFlag_ClassChanged		= 0x00000400,
	kUserFlag_GeneralGradeUp	= 0x00000800,
};

// Macros
#define		IS_ONLINE( userStatus )					( userStatus != kStatus_Unknown && userStatus != kStatus_AppearOffline && userStatus != kStatus_Offline )
#define		IS_BLOCKED( userFlag )					( ( userFlag & kUserFlag_Blocked ) != 0 )
#define		IS_WAITCONFIRM( userFlag )				( ( userFlag & kUserFlag_WaitingConfirm ) != 0 )
#define		SET_BLOCKED( userFlag )					userFlag = ( userFlag | kUserFlag_Blocked )
#define		SET_WAITCONFIRM( userFlag )				userFlag = ( userFlag | kUserFlag_WaitingConfirm )
#define		RESET_BLOCKED( userFlag )				userFlag = ( userFlag & ( ~kUserFlag_Blocked ) )
#define		RESET_WAITCONFIRM( userFlag )			userFlag = ( userFlag & ( ~kUserFlag_WaitingConfirm ) )

#define		USER_OPEN_NAME_TYPE( userMask )			( ( userMask >> 14 ) & 0x00000003 )
#define		USER_OPEN_SEX_TYPE( userMask )			( kOpenType_All )
#define		USER_OPEN_AGE_TYPE( userMask )			( ( userMask >> 18 ) & 0x00000003 )
#define		USER_OPEN_BIRTHDAY_TYPE( userMask )		( ( userMask >> 20 ) & 0x00000003 )
#define		USER_OPEN_SCHOOL_TYPE( userMask )		( ( userMask >> 22 ) & 0x00000003 )
#define		USER_OPEN_AREA_TYPE( userMask )			( ( userMask >> 24 ) & 0x00000003 )
#define		USER_OPEN_TEL_TYPE( userMask )			( ( userMask >> 26 ) & 0x00000003 )
#define		USER_OPEN_TYPE_ALL( userMask )			( ( userMask >> 14 ) & 0x0000FFFF )
#define		USER_TYPE( userMask )					( ( userMask >> 29 ) & 0x00000003 )

#define		SET_USER_OPEN_TYPE( userMask, type, offset )	userMask = ( ( type & 0x00000003 ) << offset ) | ( userMask & ( ~( 0x00000003 << offset ) ) )
#define		SET_USER_OPEN_NAME_TYPE( userMask, type )		SET_USER_OPEN_TYPE( userMask, type, 14 )
#define		SET_USER_OPEN_SEX_TYPE( userMask, type )		( FALSE )
#define		SET_USER_OPEN_AGE_TYPE( userMask, type )		SET_USER_OPEN_TYPE( userMask, type, 18 )
#define		SET_USER_OPEN_BIRTHDAY_TYPE( userMask, type )	SET_USER_OPEN_TYPE( userMask, type, 20 )
#define		SET_USER_OPEN_SCHOOL_TYPE( userMask, type )		SET_USER_OPEN_TYPE( userMask, type, 22 )
#define		SET_USER_OPEN_AREA_TYPE( userMask, type )		SET_USER_OPEN_TYPE( userMask, type, 24 )
#define		SET_USER_OPEN_TEL_TYPE( userMask, type )		SET_USER_OPEN_TYPE( userMask, type, 26 )
#define		SET_USER_OPEN_ALL( userMask, newMask )			( userMask = ( newMask & 0x3FFFC000 ) | ( userMask & ~( 0x3FFFC000 ) ) )

#define		GET_HASHCODE( idcode )					_UInt8_( idcode >> 56 )
#define		GET_OID( idcode )						_UInt32_( _UInt32_( idcode >> 32 ) & 0xFFFFFFFF )
#define		GET_GAMECODE( idcode )					_UInt32_( idcode )
#define		GET_GAMEMAINCODE( idcode )				_UInt16_( _UInt32_( idcode ) >> 16 )
#define		GET_GAMESUBCODE( idcode )				_UInt16_( idcode )

#define		MAKE_GAMECODE( maincode, subcode )		( ( maincode << 16 ) | subcode )
#define		MAKE_IDCODE( gameCode, oid )			( ( ( ( UINT64 )oid ) << 32 ) | gameCode )
#define		MAKE_NEXONIDCODE( oid )					MAKE_IDCODE( kGameCode_nexoncom, oid )

#define		SET_RESTRICT_WORD( flag )			flag = ( flag | 0x00000001 )
#define		RESET_RESTRICT_WORD( flag )			flag = ( flag & ~( 0x00000001 ) )
#define		HAS_RESTRICT_WORD( flag )			( ( flag & 0x00000001 ) != 0 )

// Global Constants
enum
{
	kMaxSessionMemberSize		= 30,
	kMaxHashCodeValue			= 128,
};

enum
{	// Open Type
	kOpenType_NULL				= 0,

	kOpenType_All				= 1,
	kOpenType_FriendsOnly		= 2,
	kOpenType_Closed			= 3,
};

enum
{	// Main Game Codes
	kMainGameCode_NULL					= 0,

	kMainGameCode_Start					= 0x0001,

	kMainGameCode_nexoncom				= 0x0001,
	kMainGameCode_baram					= 0x0002,
	kMainGameCode_lod					= 0x0003,
	kMainGameCode_elan					= 0x0004,
	kMainGameCode_asgard				= 0x0006,
	kMainGameCode_mabi					= 0x0007,
	kMainGameCode_tales					= 0x0008,
	kMainGameCode_maplestory			= 0x0009,
	kMainGameCode_ca					= 0x000B,
	kMainGameCode_qplay					= 0x000C,
	kMainGameCode_zera					= 0x000D,
	kMainGameCode_zeratest				= 0x000E,
	kMainGameCode_zeratest_internal		= 0x000F,
	kMainGameCode_lunia					= 0x0010,
	kMainGameCode_planb					= 0x0011,
	kMainGameCode_ndoors				= 0x0012,

	// new style (commended)
	kMainGameCode_nexon_jp				= 0x0100,
	kMainGameCode_nexon_cn				= 0x0101,
	kMainGameCode_nexon_tw				= 0x0102,
	kMainGameCode_nexon_th				= 0x0103,
	kMainGameCode_nexon_vn				= 0x0104,
	kMainGameCode_nexon_sg				= 0x0105,
	kMainGameCode_nexon_id				= 0x0106,
	kMainGameCode_nexon_cn2				= 0x0107,
	kMainGameCode_nexon_id2				= 0x0108,
	kMainGameCode_nexon_th2				= 0x0109,
	kMainGameCode_nexon_id3				= 0x010A,
	kMainGameCode_nexon_us				= 0x0200,
	kMainGameCode_nexon_eu				= 0x0300,
	kMainGameCode_nexon_ru				= 0x0301,
	kMainGameCode_nexon_ru2				= 0x0302,
	kMainGameCode_nexon_br				= 0x0400,

	// old style (not commended)
	kMainGameCode_nexonjp				= kMainGameCode_nexon_jp,
	kMainGameCode_nexoncn				= kMainGameCode_nexon_cn,
	kMainGameCode_nexontw				= kMainGameCode_nexon_tw,
	kMainGameCode_nexonus				= kMainGameCode_nexon_us,
	kMainGameCode_nexoneu				= kMainGameCode_nexon_eu,

	kMainGameCode_guild					= 0x1000,
	kMainGameCode_gworld				= 0x1001,
	kMainGameCode_reservedid			= 0x1002,
	kMainGameCode_guildzera				= 0x1003,
	kMainGameCode_navertoolbar			= 0x1004,
};

enum
{
	// Sub Game Codes
	kSubGameCode_NULL					= 0,

	// for Application Login
	kSubGameCode_messenger				= 0x0001,
	kSubGameCode_plug					= 0x0002,
	kSubGameCode_gamesummary			= 0x0003,
	kSubGameCode_nexonsticklauncher		= 0x0004,
	kSubGameCode_nexonstickmanager		= 0x0005,
	kSubGameCode_ca_launcher			= 0x0006,
	kSubGameCode_usbmanager				= 0x0007,

	// for Web Login
	kSubGameCode_web_main				= 0x1001,

	// for Nx Game Login
	//kSubGameCode_nxgame_start			= 0x2001,
	//kSubGameCode_gostop				= 0x2001,
	//kSubGameCode_matgo				= 0x2002,
	//kSubGameCode_starmatgo			= 0x2003,
	//kSubGameCode_poker				= 0x2004,
	//kSubGameCode_thiefnyang			= 0x2005,
	//kSubGameCode_webbz				= 0x2006,
	//kSubGameCode_onecard				= 0x2007,
	//kSubGameCode_sachun				= 0x2008,
	//kSubGameCode_omok					= 0x2009,
	//kSubGameCode_jangki				= 0x200A,
	//kSubGameCode_manmani				= 0x200B,

	// for billing
	kSubGameCode_point					= 0x200C,

	//kSubGameCode_nxgame_common		= 0x20FF,
	//kSubGameCode_nxgame_end			= 0x20FF,

	// for Kart Related Game
	kSubGameCode_kartrider				= 0x2101,
	kSubGameCode_karttest				= 0x2102,

	// for Nexon Game
	//kSubGameCode_pucca				= 0x2201,
	kSubGameCode_zera					= 0x2202,
	kSubGameCode_bigshot				= 0x2203,
	//kSubGameCode_mabikiz				= 0x2204,
	kSubGameCode_mabinogi				= 0x2205,
	kSubGameCode_nexonbyul				= 0x2206,
	kSubGameCode_combatarms				= 0x2207,
	kSubGameCode_talesweaver			= 0x2208,
	kSubGameCode_cso					= 0x2009,
	kSubGameCode_ca						= 0x200A,
	kSubGameCode_heroes					= 0x200B,
	kSubGameCode_husky					= 0x200C,
	kSubGameCode_air					= 0x200D,
	kSubGameCode_trashbuster			= 0x220E,
	kSubGameCode_bubblefighter			= 0x220F,
	kSubGameCode_kavatina				= 0x2210,
	kSubGameCode_df						= 0x2211,
	kSubGameCode_projectb				= 0x2212,
	kSubGameCode_maple					= 0x2213,
	kSubGameCode_lob					= 0x2214,
	kSubGameCode_apex					= 0x2215,
	kSubGameCode_projectearth			= 0x2216,
	kSubGameCode_quizquiz				= 0x2217,
	kSubGameCode_cyphers				= 0x2218,
	kSubGameCode_desktopheroes			= 0x2219,
	kSubGameCode_mlb2k11				= 0x221A,
	kSubGameCode_mabinogi2				= 0x221B,
	kSubGameCode_cso2					= 0x221C,
	kSubGameCode_baram					= 0x2220,
	kSubGameCode_reload					= 0x2221,
	kSubGameCode_asgard					= 0x2222,
	kSubGameCode_lod					= 0x2223,
	kSubGameCode_elan					= 0x2224,
	kSubGameCode_shaiya					= 0x2225,

	// for Mobile Game
	kSubGameCode_baseballmaster			= 0x5001,
	kSubGameCode_yg						= 0x5002,	// 영웅의군단

	// for Channeling Game
	kSubGameCode_maple_hangame			= 0x6001,
	kSubGameCode_goonzu_gamania			= 0x6002,
	kSubGameCode_talesweaver_hangame	= 0x6003,

	// for Publishing Game (Korea)
	//kSubGameCode_fruty				= 0x7001,
	//kSubGameCode_cuberun				= 0x7002,
	//kSubGameCode_bumperking			= 0x7003,
	kSubGameCode_warrock				= 0x7004,
	kSubGameCode_lunia					= 0x7005,
	kSubGameCode_ninedragons			= 0x7006,
	kSubGameCode_nanaimo				= 0x7007,
	kSubGameCode_kickoff				= 0x7008,	//	94216
	kSubGameCode_koongpa				= 0x7009,
	kSubGameCode_zone4					= 0x700A,
	kSubGameCode_slapshot				= 0x700B,	//	94219
	kSubGameCode_everplanet				= 0x700C,
	kSubGameCode_dragonnest				= 0x700D,
	kSubGameCode_istar					= 0x700E,
	kSubGameCode_warface				= 0x700F,
	kSubGameCode_elsword				= 0x7010,
	kSubGameCode_sp1					= 0x7011,
	kSubGameCode_rxsg					= 0x7012,	// 열혈삼국
	kSubGameCode_suddenattack			= 0x7013,
	kSubGameCode_dekaron				= 0x7014,
	kSubGameCode_metalrage				= 0x7015,
	kSubGameCode_immortal				= 0x7016,
	kSubGameCode_freestylefootball		= 0x7017,
	kSubGameCode_sd3					= 0x7018,
	kSubGameCode_ageofocean				= 0x7019,
	kSubGameCode_kingdoms				= 0x701A,
	kSubGameCode_dho					= 0x701B,
	kSubGameCode_sampoom				= 0x701C,	// 삼국지를 품다
	kSubGameCode_ageofhero				= 0x701D,	// 영웅시대
	kSubGameCode_chaos					= 0x701F,
	kSubGameCode_luckyspace				= 0x7020,
	kSubGameCode_heroesx				= 0x7021,	// 능력자X
	kSubGameCode_fo3					= 0x7022,	// Fifa online 3
	kSubGameCode_fc						= 0x7023,	// 파이터스클럽
	kSubGameCode_fs2					= 0x7024,	// 프리스타일2
	kSubGameCode_atlantica_lite			= 0x7025,	// 아틀란티카 라이트 클라이언트 설치용
	kSubGameCode_dota2					= 0x7026,
	kSubGameCode_lostsaga				= 0x7027,
	kSubGameCode_taleslunner			= 0x7028,

	// for NPNA Games : 0x8101 ~ 0x8199
	kSubGameCode_npna_start_			= 0x8101,
	kSubGameCode_motoloco				= 0x8101,
	kSubGameCode_sugarrush				= 0x8102,
	kSubGameCode_nxopp					= 0x8103,
	kSubGameCode_golf					= 0x8104,
	kSubGameCode_npna_end_				= 0x8199,

	// for Nexon Japan
	kSubGameCode_tenvi					= 0x8201,
	kSubGameCode_atlantica				= 0x8202,
	kSubGameCode_tenvitest				= 0x8203,
	kSubGameCode_goonzu					= 0x8204,
	kSubGameCode_druaga					= 0x8205,
	kSubGameCode_dragonica				= 0x8206,
	kSubGameCode_pachimachi				= 0x8207,
	kSubGameCode_corum					= 0x8208,
	kSubGameCode_7souls					= 0x8209,
	kSubGameCode_edda					= 0x820A,
	kSubGameCode_nexon777				= 0x820B,
	kSubGameCode_dragona				= 0x820C,
	kSubGameCode_navyfield2				= 0x820D,
	kSubGameCode_seiken					= 0x820E,
	kSubGameCode_shadowcompany			= 0x820F,
	kSubGameCode_kaiketsu				= 0x8210,
	kSubGameCode_scarletblade			= 0x8211,
	kSubGameCode_divacoliseum			= 0x8212,

	// for Nexon Europe
	kSubGameCode_arcanechronicle		= 0x8301,

	// for Not Game
	kSubGameCode_navertoolbar			= 0xE001,
	kSubGameCode_scrapper				= 0xE002,
	kSubGameCode_p2app					= 0xE003,
	kSubGameCode_gcube					= 0xE004,
	kSubGameCode_nexonmusic				= 0xE005,

	// for TEST
	kSubGameCode_testgame				= 0xF001,
	kSubGameCode_zeratest				= 0xF002,
	kSubGameCode_luniatest				= 0xF003,
	kSubGameCode_zeratest_internal		= 0xF004,
	kSubGameCode_kickofftest			= 0xF005,
	kSubGameCode_nanaimotest			= 0xF006,
	kSubGameCode_csotest				= 0xF007,
	kSubGameCode_csointernal			= 0xF008,
	kSubGameCode_csodevelopment			= 0xF009,
	kSubGameCode_elswordtest			= 0xF00A,
	kSubGameCode_elswordtest_internal	= 0xF00B,
	kSubGameCode_slapshotinternal		= 0xF00C,
	kSubGameCode_zone4internal			= 0xF00D,
	kSubGameCode_heroestest				= 0xF00E,
	kSubGameCode_heroesdevelopment		= 0xF00F,
	kSubGameCode_airtest				= 0xF010,
	kSubGameCode_everplanet_test		= 0xF011,
	kSubGameCode_everplanet_dev			= 0xF012,
	kSubGameCode_husky_dev				= 0xF013,
	kSubGameCode_dragonnest_test		= 0xF014,
	kSubGameCode_dragonnest_dev			= 0xF015,
	kSubGameCode_kavatina_light			= 0xF016,
	kSubGameCode_mabitest				= 0xF017,
	kSubGameCode_mabilocaltest			= 0xF018,
	kSubGameCode_dftest					= 0xF019,
	kSubGameCode_catest					= 0xF01A,
	kSubGameCode_mapletest				= 0xF01B,
	kSubGameCode_heroesgm				= 0xF01C,
	kSubGameCode_talesweavertest		= 0xF01D,
	kSubGameCode_mapletespia			= 0xF01E,
	kSubGameCode_dragonnest_opentest	= 0xF01F,
	kSubGameCode_suddenattack_test		= 0xF020,
	kSubGameCode_dekaron_test			= 0xF021,
	kSubGameCode_metalrage_test			= 0xF022,
	kSubGameCode_suddenattack_dev		= 0xF023,
	kSubGameCode_dekaron_dev			= 0xF024,
	kSubGameCode_metalrage_dev			= 0xF025,
	kSubGameCode_p2_test				= 0xF026,
	kSubGameCode_7souls_test			= 0xF027,
	kSubGameCode_dragonica_test			= 0xF028,
	kSubGameCode_edda_test				= 0xF029,
	kSubGameCode_baram_test				= 0xF02A,
	kSubGameCode_nexon777_test			= 0xF02B,
	kSubGameCode_7souls_dev				= 0xF02C,
	kSubGameCode_nexon777_dev			= 0xF02D,
	kSubGameCode_edda_test2				= 0xF02E,
	kSubGameCode_mabinogi2_dev			= 0xF02F,
	kSubGameCode_dragona_test			= 0xF030,
	kSubGameCode_kartriderb				= 0xF031,
	kSubGameCode_asgard_test			= 0xF032,
	kSubGameCode_lod_test				= 0xF033,
	kSubGameCode_elan_test				= 0xF034,
	kSubGameCode_navyfield2_test		= 0xF035,
	kSubGameCode_seiken_test			= 0xF036,
	kSubGameCode_shadowcompany_test		= 0xF037,
	kSubGameCode_warrock_test			= 0xF038,
	kSubGameCode_cso2_test				= 0xF039,
	kSubGameCode_mlb2k11_test			= 0xF03A,
	kSubGameCode_mlb2k11_dev			= 0xF03B,
	kSubGameCode_warface_test			= 0xF03C,
	kSubGameCode_warface_stage			= 0xF03D,
	kSubGameCode_elswordtest2			= 0xF03E,
	kSubGameCode_fo3_test				= 0xF03F,
	kSubGameCode_elswordtest3			= 0xF040,
	kSubGameCode_fo3_stage				= 0xF041,
	kSubGameCode_scarletblade_test		= 0xF042,
	kSubGameCode_mabinogi_gm			= 0xF043,

	// for MapleStory
	kSubGameCode_maple_install			= 0x0001,
	kSubGameCode_maplet_install			= 0x0002,

	// for Qplay
	kSubGameCode_qplay_install			= 0x0001,

	// for Crazy Arcade
	kSubGameCode_ca_install				= 0x0001,

	// for Legend Of Darkness
	kSubGameCode_lod_install			= 0x0001,

	// for PlanB
	kSubGameCode_planb					= 0x0001,
};

enum
{
	// Sub Guild Codes
	kSubGuildCode_NULL					= 0,

	// for game guild
	kSubGuildCode_kart					= 2,
	kSubGuildCode_warrock				= 3,
	kSubGuildCode_zera					= 5,
	kSubGuildCode_zeratest				= 0x03E9,
	kSubGuildCode_zeratest_internal		= 0x03EA,
	kSubGuildCode_lunia					= 6,
	kSubGuildCode_manmani				= 7,
	kSubGuildCode_luniatest				= 8,
	kSubGuildCode_koongpa				= 9,
	kSubGuildCode_combatarms			= 10,
	kSubGuildCode_cso					= 11,
	kSubGuildCode_slapshot				= 12,
	kSubGuildCode_bubblefighter			= 14,
	kSubGuildCode_heroes				= 15,
	kSubGuildCode_heroes_xe				= 16,

	// for friendship guild
	kSubGuildCode_friendship			= 511,

	// fo all guild
	kSubGuildCode_all					= 0xF000,
};

enum
{
	// Game Code List
	kGameCode_NULL						= ( ( ( kMainGameCode_NULL ) << 16 ) | kSubGameCode_NULL ),
	kCategoryCode_NULL					= 0,

	// new style (commended)
	kGameCode_nexoncom					= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_NULL ),
	kGameCode_nexon_jp					= MAKE_GAMECODE( kMainGameCode_nexon_jp		, kSubGameCode_NULL ),
	kGameCode_nexon_cn					= MAKE_GAMECODE( kMainGameCode_nexon_cn		, kSubGameCode_NULL ),
	kGameCode_nexon_tw					= MAKE_GAMECODE( kMainGameCode_nexon_tw		, kSubGameCode_NULL ),
	kGameCode_nexon_th					= MAKE_GAMECODE( kMainGameCode_nexon_th		, kSubGameCode_NULL ),
	kGameCode_nexon_vn					= MAKE_GAMECODE( kMainGameCode_nexon_vn		, kSubGameCode_NULL ),
	kGameCode_nexon_sg					= MAKE_GAMECODE( kMainGameCode_nexon_sg		, kSubGameCode_NULL ),
	kGameCode_nexon_cn2					= MAKE_GAMECODE( kMainGameCode_nexon_cn2	, kSubGameCode_NULL ),
	kGameCode_nexon_us					= MAKE_GAMECODE( kMainGameCode_nexon_us		, kSubGameCode_NULL ),
	kGameCode_nexon_eu					= MAKE_GAMECODE( kMainGameCode_nexon_eu		, kSubGameCode_NULL ),
	kGameCode_nexon_ru					= MAKE_GAMECODE( kMainGameCode_nexon_ru		, kSubGameCode_NULL ),
	kGameCode_nexon_br					= MAKE_GAMECODE( kMainGameCode_nexon_br		, kSubGameCode_NULL ),

	// old style (not commended)
	kGameCode_nexonjp					= kGameCode_nexon_jp,
	kGameCode_nexoncn					= kGameCode_nexon_cn,
	kGameCode_nexontw					= kGameCode_nexon_tw,
	kGameCode_nexonus					= kGameCode_nexon_us,
	kGameCode_nexoneu					= kGameCode_nexon_eu,

	kGameCode_messenger					= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_messenger ),
	kGameCode_messenger_cn				= MAKE_GAMECODE( kMainGameCode_nexon_cn		, kSubGameCode_messenger ),
	kGameCode_messenger_cn2				= MAKE_GAMECODE( kMainGameCode_nexon_cn2	, kSubGameCode_messenger ),
	kGameCode_plug						= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_plug ),
	kGameCode_gamesummary				= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_gamesummary ),
	kGameCode_nexonsticklauncher		= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_nexonsticklauncher ),
	kGameCode_nexonstickmanager			= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_nexonstickmanager ),
	kGameCode_usbmanager				= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_usbmanager ),
	kGameCode_web						= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_web_main ),

	kGameCode_baram_old					= MAKE_GAMECODE( kMainGameCode_baram		, kSubGameCode_NULL ),
	kGameCode_lod_old					= MAKE_GAMECODE( kMainGameCode_lod			, kSubGameCode_NULL ),
	kGameCode_lod_install				= MAKE_GAMECODE( kMainGameCode_lod			, kSubGameCode_lod_install ),
	kGameCode_elan_old					= MAKE_GAMECODE( kMainGameCode_elan			, kSubGameCode_NULL ),
	kGameCode_asgard_old				= MAKE_GAMECODE( kMainGameCode_asgard		, kSubGameCode_NULL ),
	kGameCode_mabi						= MAKE_GAMECODE( kMainGameCode_mabi			, kSubGameCode_NULL ),
	kGameCode_tales						= MAKE_GAMECODE( kMainGameCode_tales		, kSubGameCode_NULL ),
	kGameCode_maplestory				= MAKE_GAMECODE( kMainGameCode_maplestory	, kSubGameCode_NULL ),
	kGameCode_maple_install				= MAKE_GAMECODE( kMainGameCode_maplestory	, kSubGameCode_maple_install ),
	kGameCode_maplet_install			= MAKE_GAMECODE( kMainGameCode_maplestory	, kSubGameCode_maplet_install ),
	kGameCode_ca						= MAKE_GAMECODE( kMainGameCode_ca			, kSubGameCode_NULL ),
	kGameCode_ca_jp						= MAKE_GAMECODE( kMainGameCode_nexon_jp		, kSubGameCode_ca ),
	kGameCode_ca_us						= MAKE_GAMECODE( kMainGameCode_nexon_us		, kSubGameCode_ca ),
	kGameCode_ca_install				= MAKE_GAMECODE( kMainGameCode_ca			, kSubGameCode_ca_install ),
	kGameCode_ca_launcher				= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_ca_launcher ),
	kGameCode_qplay						= MAKE_GAMECODE( kMainGameCode_qplay		, kSubGameCode_NULL ),
	kGameCode_qplay_install				= MAKE_GAMECODE( kMainGameCode_qplay		, kSubGameCode_qplay_install ),
	kGameCode_planb						= MAKE_GAMECODE( kMainGameCode_planb		, kSubGameCode_planb ),

	kGameCode_point_jp					= MAKE_GAMECODE( kMainGameCode_nexon_jp		, kSubGameCode_point ),

	kGameCode_kartrider					= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_kartrider ),
	kGameCode_karttest					= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_karttest ),
	kGameCode_kartriderb				= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_kartriderb ),
	kGameCode_kartrider_jp				= MAKE_GAMECODE( kMainGameCode_nexon_jp		, kSubGameCode_kartrider ),
	kGameCode_karttest_jp				= MAKE_GAMECODE( kMainGameCode_nexon_jp		, kSubGameCode_karttest ),
	kGameCode_kartrider_cn				= MAKE_GAMECODE( kMainGameCode_nexon_cn		, kSubGameCode_kartrider ),
	kGameCode_karttest_cn				= MAKE_GAMECODE( kMainGameCode_nexon_cn		, kSubGameCode_karttest ),
	kGameCode_kartrider_tw				= MAKE_GAMECODE( kMainGameCode_nexon_tw		, kSubGameCode_kartrider ),
	kGameCode_karttest_tw				= MAKE_GAMECODE( kMainGameCode_nexon_tw		, kSubGameCode_karttest ),
	kGameCode_kartrider_th				= MAKE_GAMECODE( kMainGameCode_nexon_th		, kSubGameCode_kartrider ),
	kGameCode_karttest_th				= MAKE_GAMECODE( kMainGameCode_nexon_th		, kSubGameCode_karttest ),
	kGameCode_kartrider_vn				= MAKE_GAMECODE( kMainGameCode_nexon_vn		, kSubGameCode_kartrider ),
	kGameCode_karttest_vn				= MAKE_GAMECODE( kMainGameCode_nexon_vn		, kSubGameCode_karttest ),
	kGameCode_kartrider_us				= MAKE_GAMECODE( kMainGameCode_nexon_us		, kSubGameCode_kartrider ),
	kGameCode_karttest_us				= MAKE_GAMECODE( kMainGameCode_nexon_us		, kSubGameCode_karttest ),	
	kGameCode_kartrider_ru				= MAKE_GAMECODE( kMainGameCode_nexon_ru		, kSubGameCode_kartrider ),
	kGameCode_karttest_ru				= MAKE_GAMECODE( kMainGameCode_nexon_ru		, kSubGameCode_karttest ),
	kGameCode_kartrider_id2				= MAKE_GAMECODE( kMainGameCode_nexon_id2	, kSubGameCode_kartrider ),
	kGameCode_karttest_id2				= MAKE_GAMECODE( kMainGameCode_nexon_id2	, kSubGameCode_karttest ),

	kGameCode_zera						= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_zera ),
	kGameCode_bigshot					= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_bigshot ),
	kGameCode_bigshot_cn				= MAKE_GAMECODE( kMainGameCode_nexon_cn		, kSubGameCode_bigshot ),
	kGameCode_nexonbyul					= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_nexonbyul ),
	kGameCode_nexonbyul_cn				= MAKE_GAMECODE( kMainGameCode_nexon_cn		, kSubGameCode_nexonbyul ),
	kGameCode_combatarms				= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_combatarms ),
	kGameCode_combatarms_us				= MAKE_GAMECODE( kMainGameCode_nexon_us		, kSubGameCode_combatarms ),
	kGameCode_combatarms_eu				= MAKE_GAMECODE( kMainGameCode_nexon_eu		, kSubGameCode_combatarms ),
	kGameCode_combatarms_br				= MAKE_GAMECODE( kMainGameCode_nexon_br		, kSubGameCode_combatarms ),
	kGameCode_combatarms_ru2			= MAKE_GAMECODE( kMainGameCode_nexon_ru2	, kSubGameCode_combatarms ),
	kGameCode_talesweaver				= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_talesweaver ),
	kGameCode_talesweaver_cn			= MAKE_GAMECODE( kMainGameCode_nexon_cn		, kSubGameCode_talesweaver ),
	kGameCode_talesweaver_jp			= MAKE_GAMECODE( kMainGameCode_nexon_jp		, kSubGameCode_talesweaver ),
	kGameCode_talesweaver_hangame_jp	= MAKE_GAMECODE( kMainGameCode_nexon_jp		, kSubGameCode_talesweaver_hangame ),
	kGameCode_cso						= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_cso ),
	kGameCode_cso_tw					= MAKE_GAMECODE( kMainGameCode_nexon_tw		, kSubGameCode_cso ),
	kGameCode_cso_cn					= MAKE_GAMECODE( kMainGameCode_nexon_cn		, kSubGameCode_cso ),
	kGameCode_cso_jp					= MAKE_GAMECODE( kMainGameCode_nexon_jp		, kSubGameCode_cso ),
	kGameCode_cso_sg					= MAKE_GAMECODE( kMainGameCode_nexon_sg		, kSubGameCode_cso ),
	kGameCode_cso_id					= MAKE_GAMECODE( kMainGameCode_nexon_id		, kSubGameCode_cso ),
	kGameCode_cso_th2					= MAKE_GAMECODE( kMainGameCode_nexon_th2	, kSubGameCode_cso ),
	kGameCode_cso_tur					= MAKE_GAMECODE( kMainGameCode_nexon_eu		, kSubGameCode_cso ),
	kGameCode_heroes					= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_heroes ),
	kGameCode_heroes_us					= MAKE_GAMECODE( kMainGameCode_nexon_us		, kSubGameCode_heroes ),
	kGameCode_heroes_cn					= MAKE_GAMECODE( kMainGameCode_nexon_cn		, kSubGameCode_heroes ),
	kGameCode_heroes_jp					= MAKE_GAMECODE( kMainGameCode_nexon_jp		, kSubGameCode_heroes ),
	kGameCode_heroes_eu					= MAKE_GAMECODE( kMainGameCode_nexon_eu		, kSubGameCode_heroes ),
	kGameCode_heroes_tw					= MAKE_GAMECODE( kMainGameCode_nexon_tw		, kSubGameCode_heroes ),
	kGameCode_husky						= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_husky ),
	kGameCode_petcity					= kGameCode_husky,
	kGameCode_air						= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_air ),
	kGameCode_neo						= kGameCode_air,
	kGameCode_trashbuster				= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_trashbuster ),
	kGameCode_bubblefighter				= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_bubblefighter ),
	kGameCode_bubblefighter_cn2			= MAKE_GAMECODE( kMainGameCode_nexon_cn2	, kSubGameCode_bubblefighter ),
	kGameCode_bubblefighter_tw			= MAKE_GAMECODE( kMainGameCode_nexon_tw		, kSubGameCode_bubblefighter ),
	kGameCode_bubblefighter_us			= MAKE_GAMECODE( kMainGameCode_nexon_us		, kSubGameCode_bubblefighter ),
	kGameCode_bubblefighter_id2			= MAKE_GAMECODE( kMainGameCode_nexon_id2	, kSubGameCode_bubblefighter ),
	kGameCode_kavatina					= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_kavatina ),
	kGameCode_projects					= kGameCode_kavatina,
	kGameCode_df						= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_df ),
	kGameCode_df_jp						= MAKE_GAMECODE( kMainGameCode_nexon_jp		, kSubGameCode_df ),
	kGameCode_df_us						= MAKE_GAMECODE( kMainGameCode_nexon_us		, kSubGameCode_df ),
	kGameCode_projectb					= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_projectb ),
	kGameCode_lob						= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_lob ),
	kGameCode_mabinogi					= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_mabinogi ),
	kGameCode_mabinogi_us				= MAKE_GAMECODE( kMainGameCode_nexon_us		, kSubGameCode_mabinogi ),
	kGameCode_mabinogi_jp				= MAKE_GAMECODE( kMainGameCode_nexon_jp		, kSubGameCode_mabinogi ),
	kGameCode_maple						= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_maple ),
	kGameCode_maple_us					= MAKE_GAMECODE( kMainGameCode_nexon_us		, kSubGameCode_maple ),
	kGameCode_maple_jp					= MAKE_GAMECODE( kMainGameCode_nexon_jp		, kSubGameCode_maple ),
	kGameCode_maple_eu					= MAKE_GAMECODE( kMainGameCode_nexon_eu		, kSubGameCode_maple ),
	kGameCode_mapletespia_jp			= MAKE_GAMECODE( kMainGameCode_nexon_jp		, kSubGameCode_mapletespia ),
	kGameCode_maple_hangame_jp			= MAKE_GAMECODE( kMainGameCode_nexon_jp		, kSubGameCode_maple_hangame ),
	kGameCode_apex						= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_apex ),
	kGameCode_projectearth				= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_projectearth ),
	kGameCode_quizquiz					= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_quizquiz ),
	kGameCode_cyphers					= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_cyphers ),
	kGameCode_cyphers_cn				= MAKE_GAMECODE( kMainGameCode_nexon_cn		, kSubGameCode_cyphers ),
	kGameCode_mlb2k11					= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_mlb2k11 ),
	kGameCode_mabinogi2					= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_mabinogi2 ),
	kGameCode_cso2						= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_cso2 ),
	kGameCode_cso2_tw					= MAKE_GAMECODE( kMainGameCode_nexon_tw		, kSubGameCode_cso2 ),
	kGameCode_cso2_cn					= MAKE_GAMECODE( kMainGameCode_nexon_cn		, kSubGameCode_cso2 ),
	kGameCode_desktopheroes				= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_desktopheroes ),
	kGameCode_baram						= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_baram ),
	kGameCode_reload					= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_reload ),
	kGameCode_asgard					= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_asgard ),
	kGameCode_lod						= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_lod ),
	kGameCode_elan						= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_elan ),
	kGameCode_shaiya					= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_shaiya ),

	kGameCode_baseballmaster			= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_baseballmaster ),
	kGameCode_yg						= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_yg ),

	kGameCode_warrock					= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_warrock ),
	kGameCode_warrock_eu				= MAKE_GAMECODE( kMainGameCode_nexon_eu		, kSubGameCode_warrock ),
	kGameCode_lunia						= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_lunia ),
	kGameCode_lunia_jp					= MAKE_GAMECODE( kMainGameCode_nexon_jp		, kSubGameCode_lunia ),
	kGameCode_ninedragons				= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_ninedragons ),
	kGameCode_nanaimo					= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_nanaimo ),
	kGameCode_kickoff					= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_kickoff ),
	kGameCode_koongpa					= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_koongpa ),
	kGameCode_koongpa_cn				= MAKE_GAMECODE( kMainGameCode_nexon_cn		, kSubGameCode_koongpa ),
	kGameCode_zone4						= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_zone4 ),
	kGameCode_slapshot					= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_slapshot ),
	kGameCode_everplanet				= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_everplanet ),
	kGameCode_everplanet_jp				= MAKE_GAMECODE( kMainGameCode_nexon_jp		, kSubGameCode_everplanet ),
	kGameCode_everplanet_th2			= MAKE_GAMECODE( kMainGameCode_nexon_th2	, kSubGameCode_everplanet ),
	kGameCode_everplanet_id3			= MAKE_GAMECODE( kMainGameCode_nexon_id3	, kSubGameCode_everplanet ),
	kGameCode_projectmv					= kGameCode_everplanet,
	kGameCode_dragonnest				= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_dragonnest ),
	kGameCode_dragonnest_us				= MAKE_GAMECODE( kMainGameCode_nexon_us		, kSubGameCode_dragonnest ),
	kGameCode_istar						= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_istar ),
	kGameCode_warface					= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_warface ),
	kGameCode_warface_jp				= MAKE_GAMECODE( kMainGameCode_nexon_jp		, kSubGameCode_warface ),
	kGameCode_elsword					= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_elsword ),
	kGameCode_sp1						= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_sp1 ),
	kGameCode_rxsg						= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_rxsg ),
	kGameCode_suddenattack				= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_suddenattack ),
	kGameCode_suddenattack_jp			= MAKE_GAMECODE( kMainGameCode_nexon_jp		, kSubGameCode_suddenattack ),
	kGameCode_suddenattack_us			= MAKE_GAMECODE( kMainGameCode_nexon_us		, kSubGameCode_suddenattack ),
	kGameCode_dekaron_jp				= MAKE_GAMECODE( kMainGameCode_nexon_jp		, kSubGameCode_dekaron ),
	kGameCode_dekaron_eu				= MAKE_GAMECODE( kMainGameCode_nexon_eu		, kSubGameCode_dekaron ),
	kGameCode_metalrage_jp				= MAKE_GAMECODE( kMainGameCode_nexon_jp		, kSubGameCode_metalrage ),
	kGameCode_immortal					= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_immortal ),
	kGameCode_freestylefootball			= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_freestylefootball ),
	kGameCode_sd3						= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_sd3 ),
	kGameCode_ageofocean				= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_ageofocean ),
	kGameCode_kingdoms					= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_kingdoms ),
	kGameCode_dho						= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_dho ),
	kGameCode_sampoom					= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_sampoom ),
	kGameCode_sampoom_jp				= MAKE_GAMECODE( kMainGameCode_nexon_jp		, kSubGameCode_sampoom ),
	kGameCode_ageofhero					= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_ageofhero ),
	kGameCode_chaos						= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_chaos ),
	kGameCode_luckyspace				= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_luckyspace ),
	kGameCode_heroesx					= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_heroesx ),
	kGameCode_fo3						= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_fo3 ),
	kGameCode_fc						= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_fc ),
	kGameCode_fs2						= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_fs2 ),
	kGameCode_fs2_cn					= MAKE_GAMECODE( kMainGameCode_nexon_cn		, kSubGameCode_fs2 ),
	kGameCode_fs2_vn					= MAKE_GAMECODE( kMainGameCode_nexon_vn		, kSubGameCode_fs2 ),
	kGameCode_dota2						= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_dota2 ),
	kGameCode_lostsaga					= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_lostsaga ),
	kGameCode_lostsaga_eu				= MAKE_GAMECODE( kMainGameCode_nexon_eu		, kSubGameCode_lostsaga ),
	kGameCode_taleslunner				= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_taleslunner ),

	kGameCode_motoloco					= MAKE_GAMECODE( kMainGameCode_nexon_us		, kSubGameCode_motoloco ),
	kGameCode_sugarrush					= MAKE_GAMECODE( kMainGameCode_nexon_us		, kSubGameCode_sugarrush ),
	kGameCode_nxopp						= MAKE_GAMECODE( kMainGameCode_nexon_us		, kSubGameCode_nxopp ),
	kGameCode_golf						= MAKE_GAMECODE( kMainGameCode_nexon_us		, kSubGameCode_golf ),

	kGameCode_tenvi						= MAKE_GAMECODE( kMainGameCode_nexon_jp		, kSubGameCode_tenvi ),
	kGameCode_tenvi_cn					= MAKE_GAMECODE( kMainGameCode_nexon_cn		, kSubGameCode_tenvi ),
	kGameCode_atlantica_kr				= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_atlantica ),
	kGameCode_atlantica_kr_lite			= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_atlantica_lite ),
	kGameCode_atlantica_kr_ndoors		= MAKE_GAMECODE( kMainGameCode_ndoors		, kSubGameCode_atlantica ),
	kGameCode_atlantica					= MAKE_GAMECODE( kMainGameCode_nexon_jp		, kSubGameCode_atlantica ),
	kGameCode_atlantica_us				= MAKE_GAMECODE( kMainGameCode_nexon_us		, kSubGameCode_atlantica ),
	kGameCode_tenvitest					= MAKE_GAMECODE( kMainGameCode_nexon_jp		, kSubGameCode_tenvitest ),
	kGameCode_goonzu					= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_goonzu ),
	kGameCode_goonzu_jp					= MAKE_GAMECODE( kMainGameCode_nexon_jp		, kSubGameCode_goonzu ),
	kGameCode_goonzu_gamania			= MAKE_GAMECODE( kMainGameCode_nexon_jp		, kSubGameCode_goonzu_gamania ),
	kGameCode_druaga					= MAKE_GAMECODE( kMainGameCode_nexon_jp		, kSubGameCode_druaga ),
	kGameCode_dragonica_jp				= MAKE_GAMECODE( kMainGameCode_nexon_jp		, kSubGameCode_dragonica ),
	kGameCode_pachimachi_jp				= MAKE_GAMECODE( kMainGameCode_nexon_jp		, kSubGameCode_pachimachi ),
	kGameCode_corum_jp					= MAKE_GAMECODE( kMainGameCode_nexon_jp		, kSubGameCode_corum ),
	kGameCode_7souls_jp					= MAKE_GAMECODE( kMainGameCode_nexon_jp		, kSubGameCode_7souls ),
	kGameCode_edda_jp					= MAKE_GAMECODE( kMainGameCode_nexon_jp		, kSubGameCode_edda ),
	kGameCode_nexon777_jp				= MAKE_GAMECODE( kMainGameCode_nexon_jp		, kSubGameCode_nexon777 ),
	kGameCode_dragona_jp				= MAKE_GAMECODE( kMainGameCode_nexon_jp		, kSubGameCode_dragona ),
	kGameCode_navyfield2_jp				= MAKE_GAMECODE( kMainGameCode_nexon_jp		, kSubGameCode_navyfield2 ),
	kGameCode_navyfield2_us				= MAKE_GAMECODE( kMainGameCode_nexon_us		, kSubGameCode_navyfield2 ),
	kGameCode_navyfield2_eu				= MAKE_GAMECODE( kMainGameCode_nexon_eu		, kSubGameCode_navyfield2 ),
	kGameCode_seiken_jp					= MAKE_GAMECODE( kMainGameCode_nexon_jp		, kSubGameCode_seiken ),
	kGameCode_shadowcompany_jp			= MAKE_GAMECODE( kMainGameCode_nexon_jp		, kSubGameCode_shadowcompany ),
	kGameCode_kaiketsu_jp				= MAKE_GAMECODE( kMainGameCode_nexon_jp		, kSubGameCode_kaiketsu ),
	kGameCode_scarletblade_jp			= MAKE_GAMECODE( kMainGameCode_nexon_jp		, kSubGameCode_scarletblade ),
	kGameCode_divacoliseum_jp			= MAKE_GAMECODE( kMainGameCode_nexon_jp		, kSubGameCode_divacoliseum ),
	kGameCode_arcanechronicle_eu		= MAKE_GAMECODE( kMainGameCode_nexon_eu		, kSubGameCode_arcanechronicle ),

	kGameCode_navertoolbar				= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_navertoolbar ),
	kGameCode_scrapper					= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_scrapper ),
	kGameCode_p2app						= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_p2app ),
	kGameCode_gcube						= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_gcube ),
	kGameCode_nexonmusic				= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_nexonmusic ),

	kGameCode_testgame					= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_testgame ),
	kGameCode_testgame_cn				= MAKE_GAMECODE( kMainGameCode_nexon_cn		, kSubGameCode_testgame ),
	kGameCode_zeratest					= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_zeratest ),
	kGameCode_luniatest					= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_luniatest ),
	kGameCode_luniatest_jp				= MAKE_GAMECODE( kMainGameCode_nexon_jp		, kSubGameCode_luniatest ),
	kGameCode_zeratest_internal			= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_zeratest_internal ),
	kGameCode_kickofftest				= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_kickofftest ),
	kGameCode_nanaimotest				= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_nanaimotest ),
	kGameCode_csotest					= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_csotest ),
	kGameCode_csointernal				= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_csointernal ),
	kGameCode_csodevelopment			= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_csodevelopment ),
	kGameCode_elswordtest				= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_elswordtest ),
	kGameCode_elswordtest_internal		= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_elswordtest_internal ),
	kGameCode_slapshotinternal			= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_slapshotinternal ),
	kGameCode_zone4internal				= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_zone4internal ),
	kGameCode_heroestest				= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_heroestest ),
	kGameCode_heroestest_cn				= MAKE_GAMECODE( kMainGameCode_nexon_cn		, kSubGameCode_heroestest ),
	kGameCode_heroestest_jp				= MAKE_GAMECODE( kMainGameCode_nexon_jp		, kSubGameCode_heroestest ),
	kGameCode_heroesdevelopment			= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_heroesdevelopment ),
	kGameCode_airtest					= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_airtest ),
	kGameCode_everplanet_test			= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_everplanet_test ),
	kGameCode_everplanet_dev			= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_everplanet_dev ),
	kGameCode_everplanet_test_jp		= MAKE_GAMECODE( kMainGameCode_nexon_jp		, kSubGameCode_everplanet_test ),
	kGameCode_everplanet_dev_jp			= MAKE_GAMECODE( kMainGameCode_nexon_jp		, kSubGameCode_everplanet_dev ),
	kGameCode_everplanet_test_th2		= MAKE_GAMECODE( kMainGameCode_nexon_th2	, kSubGameCode_everplanet_test ),
	kGameCode_everplanet_dev_th2		= MAKE_GAMECODE( kMainGameCode_nexon_th2	, kSubGameCode_everplanet_dev ),
	kGameCode_everplanet_test_id3		= MAKE_GAMECODE( kMainGameCode_nexon_id3	, kSubGameCode_everplanet_test ),
	kGameCode_everplanet_dev_id3		= MAKE_GAMECODE( kMainGameCode_nexon_id3	, kSubGameCode_everplanet_dev ),
	kGameCode_husky_dev					= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_husky_dev ),
	kGameCode_dragonnest_test			= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_dragonnest_test ),
	kGameCode_dragonnest_dev			= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_dragonnest_dev ),
	kGameCode_dragonnest_opentest		= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_dragonnest_opentest ),
	kGameCode_kavatina_light			= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_kavatina_light ),
	kGameCode_mabitest_us				= MAKE_GAMECODE( kMainGameCode_nexon_us		, kSubGameCode_mabitest ),
	kGameCode_mabilocaltest_us			= MAKE_GAMECODE( kMainGameCode_nexon_us		, kSubGameCode_mabilocaltest ),
	kGameCode_mabitest_jp				= MAKE_GAMECODE( kMainGameCode_nexon_jp		, kSubGameCode_mabitest ),
	kGameCode_mabinogi_gm_jp			= MAKE_GAMECODE( kMainGameCode_nexon_jp		, kSubGameCode_mabinogi_gm ),
	kGameCode_dftest_us					= MAKE_GAMECODE( kMainGameCode_nexon_us		, kSubGameCode_dftest ),
	kGameCode_catest_us					= MAKE_GAMECODE( kMainGameCode_nexon_us		, kSubGameCode_catest ),
	kGameCode_mapletest_us				= MAKE_GAMECODE( kMainGameCode_nexon_us		, kSubGameCode_mapletest ),
	kGameCode_mapletespia_us			= MAKE_GAMECODE( kMainGameCode_nexon_us		, kSubGameCode_mapletespia ),
	kGameCode_heroesgm					= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_heroesgm ),
	kGameCode_heroesgm_us				= MAKE_GAMECODE( kMainGameCode_nexon_us		, kSubGameCode_heroesgm ),
	kGameCode_heroesgm_cn				= MAKE_GAMECODE( kMainGameCode_nexon_cn		, kSubGameCode_heroesgm ),
	kGameCode_heroesgm_jp				= MAKE_GAMECODE( kMainGameCode_nexon_jp		, kSubGameCode_heroesgm ),
	kGameCode_heroesgm_eu				= MAKE_GAMECODE( kMainGameCode_nexon_eu		, kSubGameCode_heroesgm ),
	kGameCode_heroesgm_tw				= MAKE_GAMECODE( kMainGameCode_nexon_tw		, kSubGameCode_heroesgm ),
	kGameCode_talesweavertest_cn		= MAKE_GAMECODE( kMainGameCode_nexon_cn		, kSubGameCode_talesweavertest ),
	kGameCode_talesweavertest			= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_talesweavertest ),
	kGameCode_suddenattack_test_jp		= MAKE_GAMECODE( kMainGameCode_nexon_jp		, kSubGameCode_suddenattack_test ),
	kGameCode_dekaron_test_jp			= MAKE_GAMECODE( kMainGameCode_nexon_jp		, kSubGameCode_dekaron_test ),
	kGameCode_dekaron_test_eu			= MAKE_GAMECODE( kMainGameCode_nexon_eu		, kSubGameCode_dekaron_test ),
	kGameCode_metalrage_test_jp			= MAKE_GAMECODE( kMainGameCode_nexon_jp		, kSubGameCode_metalrage_test ),
	kGameCode_suddenattack_dev_jp		= MAKE_GAMECODE( kMainGameCode_nexon_jp		, kSubGameCode_suddenattack_dev ),
	kGameCode_dekaron_dev_jp			= MAKE_GAMECODE( kMainGameCode_nexon_jp		, kSubGameCode_dekaron_dev ),
	kGameCode_metalrage_dev_jp			= MAKE_GAMECODE( kMainGameCode_nexon_jp		, kSubGameCode_metalrage_dev ),
	kGameCode_p2_test					= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_p2_test ),
	kGameCode_7souls_test_jp			= MAKE_GAMECODE( kMainGameCode_nexon_jp		, kSubGameCode_7souls_test ),
	kGameCode_7souls_dev_jp				= MAKE_GAMECODE( kMainGameCode_nexon_jp		, kSubGameCode_7souls_dev ),
	kGameCode_dragonica_test_jp			= MAKE_GAMECODE( kMainGameCode_nexon_jp		, kSubGameCode_dragonica_test ),
	kGameCode_edda_test_jp				= MAKE_GAMECODE( kMainGameCode_nexon_jp		, kSubGameCode_edda_test ),
	kGameCode_baram_test				= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_baram_test ),
	kGameCode_nexon777_test_jp			= MAKE_GAMECODE( kMainGameCode_nexon_jp		, kSubGameCode_nexon777_test ),
	kGameCode_nexon777_dev_jp			= MAKE_GAMECODE( kMainGameCode_nexon_jp		, kSubGameCode_nexon777_dev ),
	kGameCode_edda_test2_jp				= MAKE_GAMECODE( kMainGameCode_nexon_jp		, kSubGameCode_edda_test2 ),
	kGameCode_mabinogi2_dev				= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_mabinogi2_dev ),
	kGameCode_dragona_test_jp			= MAKE_GAMECODE( kMainGameCode_nexon_jp		, kSubGameCode_dragona_test ),
	kGameCode_asgard_test				= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_asgard_test ),
	kGameCode_lod_test					= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_lod_test ),
	kGameCode_elan_test					= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_elan_test ),
	kGameCode_navyfield2_test_jp		= MAKE_GAMECODE( kMainGameCode_nexon_jp		, kSubGameCode_navyfield2_test ),
	kGameCode_navyfield2_test_eu		= MAKE_GAMECODE( kMainGameCode_nexon_eu		, kSubGameCode_navyfield2_test ),
	kGameCode_seiken_test_jp			= MAKE_GAMECODE( kMainGameCode_nexon_jp		, kSubGameCode_seiken_test ),
	kGameCode_shadowcompany_test_jp		= MAKE_GAMECODE( kMainGameCode_nexon_jp		, kSubGameCode_shadowcompany_test ),
	kGameCode_warrock_test_eu			= MAKE_GAMECODE( kMainGameCode_nexon_eu		, kSubGameCode_warrock_test ),
	kGameCode_cso2_test					= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_cso2_test ),
	kGameCode_cso2_test_tw				= MAKE_GAMECODE( kMainGameCode_nexon_tw		, kSubGameCode_cso2_test ),
	kGameCode_cso2_test_cn				= MAKE_GAMECODE( kMainGameCode_nexon_cn		, kSubGameCode_cso2_test ),
	kGameCode_mlb2k11_test				= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_mlb2k11_test ),
	kGameCode_mlb2k11_dev				= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_mlb2k11_dev ),
	kGameCode_warface_test				= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_warface_test ),
	kGameCode_warface_test_jp			= MAKE_GAMECODE( kMainGameCode_nexon_jp		, kSubGameCode_warface_test ),
	kGameCode_warface_stage				= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_warface_stage ),
	kGameCode_elswordtest2				= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_elswordtest2 ),
	kGameCode_fo3_test					= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_fo3_test ),
	kGameCode_suddenattack_test			= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_suddenattack_test ),
	kGameCode_elswordtest3				= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_elswordtest3 ),
	kGameCode_fo3_stage					= MAKE_GAMECODE( kMainGameCode_nexoncom		, kSubGameCode_fo3_stage ),
	kGameCode_scarletblade_test_jp		= MAKE_GAMECODE( kMainGameCode_nexon_jp		, kSubGameCode_scarletblade_test ),

	kGameCode_guild						= MAKE_GAMECODE( kMainGameCode_guild		, kSubGameCode_NULL ),
	kGameCode_gworld					= MAKE_GAMECODE( kMainGameCode_gworld		, kSubGameCode_NULL ),
	kGameCode_reservedid				= MAKE_GAMECODE( kMainGameCode_reservedid	, kSubGameCode_NULL ),
};

#define		MAKE_GUILDCODE( subcode )				MAKE_GAMECODE( kMainGameCode_guild, subcode )
#define		MAKE_GUILDKEY( guildcode, guildserial )	( ( ( _UInt64_ )guildcode << 32 ) | guildserial )

#define		GET_GUILDCODE( guildkey )				_UInt32_( _UInt32_( guildkey >> 32 ) & 0xFFFFFFFF )
#define		GET_GUILDSERIAL( guildkey )				_UInt32_( guildkey )

enum
{
	// Guild Code List
	kGuildCode_NULL						= MAKE_GUILDCODE( kSubGuildCode_NULL ),

	kGuildCode_kart						= MAKE_GUILDCODE( kSubGuildCode_kart ),
	kGuildCode_warrock					= MAKE_GUILDCODE( kSubGuildCode_warrock ),
	kGuildCode_zera						= MAKE_GUILDCODE( kSubGuildCode_zera ),
	kGuildCode_zeratest					= MAKE_GUILDCODE( kSubGuildCode_zeratest ),
	kGuildCode_zeratest_internal		= MAKE_GUILDCODE( kSubGuildCode_zeratest_internal ),
	kGuildCode_lunia					= MAKE_GUILDCODE( kSubGuildCode_lunia ),
	kGuildCode_manmani					= MAKE_GUILDCODE( kSubGuildCode_manmani ),
	kGuildCode_luniatest				= MAKE_GUILDCODE( kSubGuildCode_luniatest ),
	kGuildCode_koongpa					= MAKE_GUILDCODE( kSubGuildCode_koongpa ),
	kGuildCode_cso						= MAKE_GUILDCODE( kSubGuildCode_cso ),
	kGuildCode_combatarms				= MAKE_GUILDCODE( kSubGuildCode_combatarms ),
	kGuildCode_slapshot					= MAKE_GUILDCODE( kSubGuildCode_slapshot ),
	kGuildCode_bubblefighter			= MAKE_GUILDCODE( kSubGuildCode_bubblefighter ),
	kGuildCode_heroes					= MAKE_GUILDCODE( kSubGuildCode_heroes ),
	kGuildCode_heroes_xe				= MAKE_GUILDCODE( kSubGuildCode_heroes_xe ),

	kGuildCode_friendship				= MAKE_GUILDCODE( kSubGuildCode_friendship ),

	kGuildCode_all						= MAKE_GUILDCODE( kSubGuildCode_all ),
};

enum	NMLoginAuthReplyCode
{	//	Reply code for login result to authentication server
	kLoginAuth_OK,									//	성공

	kLoginAuth_ServerFailed				= 20000,	//	인증서버 실패
	kLoginAuth_ServiceShutdown			= 20002,	//	서비스 종료
	kLoginAuth_NotAllowedLocale			= 20003,	//	시스템에 맞지 않는 로케일
	kLoginAuth_WrongID					= 20006,	//	해당 아이디가 없거나 오류가 있음
	kLoginAuth_BlockedIP				= 20007,	//	제한된 아이피
	kLoginAuth_TempBlockedByLoginFail	= 20008,	//	여러 차례 로그인 실패로 인해 임시로 사용 제한
	kLoginAuth_TempBlockedByWarning		= 20009,	//	운영자에 의해 임시로 사용 제한
	kLoginAuth_BlockedByAdmin			= 20010,	//	운영자에 의해 사용 제한
	kLoginAuth_SessionDataNotExist		= 20013,	//	세션 데이터가 존재하지 않음
	kLoginAuth_InvalidUserIP			= 20014,	//	IP 정보가 일치하지 않음
	kLoginAuth_InvalidPassportKey		= 20015,	//	패스포트 key 정보가 일치하지 않음
	kLoginAuth_Disconnected				= 20018,	//	다른 IP에서 접속하여 연결 끊김
	kLoginAuth_NotAllowedServer			= 20021,	//	인증서버에서 받아들이지 않음
	kLoginAuth_InvalidArgument			= 20024,	//	잘못된 파라미터 입력 / AuthCheck
	kLoginAuth_UserNotExists			= 20025,	//	존재하지 않는 아이디
	kLoginAuth_WrongPassword			= 20026,	//	암호가 맞지 않음
	kLoginAuth_WithdrawnUser			= 20027,	//	탈퇴한 회원
	kLoginAuth_WrongOwner				= 20028,	//	본인 아님 인증된 유저
	kLoginAuth_GameServerInspection		= 20030,	//	게임 서버가 점검중입니다.
	kLoginAuth_TempUserLoginBlocked		= 20031,	//	임시회원은 실명인증 후에 로그인 가능
	kLoginAuth_NeedMatrixLogin			= 20032,	//	2차인증을 필요로 합니다.
	kLoginAuth_WrongSecurityData		= 20033,	//	2차 인증 정보가 맞지 않음
	kLoginAuth_BlockedByEvent			= 20035,	//	이벤트로 인한 사용 제한
	kLoginAuth_DeactivatedAccount		= 20036,	//	계정 활성화 후에 로그인 가능
	kLoginAuth_AlreadyLoggedIn			= 20037,	//	이미 로그인 되어 있습니다.
	kLoginAuth_SecureDataNotExist		= 20038,	//	2차 인증 데이터가 존재하지 않습니다
	kLoginAuth_InvalidOtpData			= 20039,	//	OTP 데이터가 일치하지 않습니다
	kLoginAuth_NotRegisteredPC			= 20040,	//	지정PC가 아닙니다.
	kLoginAuth_NeedSecureData			= 20041,	//	2차 인증 데이터가 필요합니다.
	kLoginAuth_RegisteredPC				= 20042,	//	지정PC입니다.
	kLoginAuth_Timeout					= 20043,	//	제한시간동안 응답이 없습니다.
	kLoginAuth_PasswordExpired			= 20044,	//	비밀번호 만료
	kLoginAuth_NotVerified				= 20045,	//	E-mail Verification 필요
	kLoginAuth_BlockedBySuspiciousIP	= 20046,	//	IP Blocking Rule 에 의해 블록된 IP에서 로그인한 기록이 있는 계정. verification 필요.
	kLoginAuth_ForcePasswordExpired		= 20047,	//	IP Blocking Rule 에 의해 Password 를 강제로 변경해야 합니다.
	kLoginAuth_LowLevelPassport			= 20048,	//	Passport Level is low. Secured Game must have level 2 passport.
	kLoginAuth_GameCodeMismatch			= 20049,	//	Mismatch between the caller's gamecode and the passport's gamecode.
	kLoginAuth_InvalidHWID				= 20050,	//	Invalid HWID.
	kLoginAuth_UserNotExistsBlockparty	= 20051,	//	Entered ID is not exists in Blockparty.
	kLoginAuth_BlockedByServiceArea		= 20052,	//	Blocked by not service area
	kLoginAuth_ExpiredByServiceArea		= 20053,	//	Password expired by not service area
	kLoginAuth_BlockedByLongInactivated	= 20054,	//	Blocked by long-time inactivated user; 북미에서 15개월동안 미접속 유저에 대해 이메일 재인증 요청 상태
	kLoginAuth_ExpiredByLongInactivated	= 20055,	//	Password expired by long-time inactivated user; 북미에서 15개월동안 미접속 유저에 대해 패스워드 강제변경 요청상태
	kLoginAuth_InvalidChannelCode		= 20056,	//	지원하지 않는 채널 코드 사용
	kLoginAuth_ChargeBackUser			= 20057,	//	Blocked by malicious charge back.
	kLoginAuth_BlockedBySafeLogin		= 20058,	//	Blocked by safe login
	kLoginAuth_ErrorBySafeLogin			= 20059,	//	Error by safe login.(time out or thinkpool internal error)
	kLoginAuth_TransMember				= 20061,	//	Membership user cannot use the Nexon ID to login. (use a e-mail)
	kLoginAuth_GuestServiceClosed		= 20063,	//	체험 서비스 기간 종료
	kLoginAuth_NeedEMailVerify			= 20064,	//	E메일 인증 필요(EU).
	kLoginAuth_NeedCaptcha				= 20065,	//	CAPTCHA 인증 필요.
	kLoginAuth_WrongCaptcha				= 20066,	//	CAPTCHA 인증 실패.
	kLoginAuth_ModuleNotInitialized		= 30002,	//	메신저 모듈 초기화가 안되어 있음
	kLoginAuth_ModuleInitializeFailed	= 30003,	//	메신저 모듈 초기화에 실패
};

enum	SecurityCode
{
	kSecurityCode_None			= 0,
	kSecurityCode_OTP			= 1,
	kSecurityCode_RegisteredPC	= 2,
	kSecurityCode_NexonStick	= 3,
	kSecurityCode_Matrix		= 4,	// for china
	kSecurityCode_Passpod		= 5,	// for china
};

enum	NMGameTokenReplyCode
{
	kGameToken_OK					= 0,
	kGameToken_RegisteredPC			= 20042,	// This is a registered PC.

	kGameToken_ServerFailed			= 20000,	// Cannot connect to SSO server, or SSO system error.
	kGameToken_InvalidNexonSN		= 20012,	// User serial number should not be zero.
	kGameToken_SessionDataNotExist	= 20013,	// User session data not exists. ( Maybe timeout or something. )
	kGameToken_InvalidUserIP		= 20014,	// Session validation failed. Passport information missmatched.
	kGameToken_InvalidPassportKey	= 20015,	// Session validation failed.
	kGameToken_LockFailed			= 20016,	// Temporal error occurred. Try again.
	kGameToken_OtpDataNotExist		= 20038,	// OTP data not exits.
	kGameToken_InvalidOtpData		= 20039,	// OTP data validation failed.
	kGameToken_NotRegisteredPC		= 20040,	// This is not a registered PC. Enter OTP code.
	kGameToken_NeedOtpData			= 20041,	// You don't use registered PC. Enter OTP code.
};

enum SecureLoginType
{
	kSecureLoginType_None				= 0,
	kSecureLoginType_OTP				= 1,
	kSecureLoginType_RegisteredPC		= 2,
	kSecureLoginType_NexonStick			= 3,
};

enum SecureLoginFlag
{
	kFlag_NotUsed				= 0,
	kFlag_OTP					= 0x1 << 0,
	kFlag_AllGameUsedOTP		= 0x1 << 1,
	kFlag_NexonStick			= 0x1 << 2,
	kFlag_AllGameUsedNexonStick
								= 0x1 << 3,
};

enum	NMMessengerReplyCode
{	// Result Code for Messenger Reply
	kMessengerReplyOK										= 0,
	kMessengerReplyNewUser									= 1,

	kMessengerReplyNotMine									= -1,
	kMessengerReplyWrongId									= -2,
	kMessengerReplyWrongPwd									= -3,
	kMessengerReplyWrongClientVersion						= -4,
	kMessengerReplyWrongMsgVersion							= -5,
	kMessengerReplyServiceShutdown							= -6,
	kMessengerReplyLockedByAnotherProcess					= -7,
	kMessengerReplySwitchSA									= -8,
	kMessengerReplyWrongOwner								= -9,
	kMessengerReplyBlockByAdmin								= -10,
	kMessengerReplyTempBlockByWarning						= -11,
	kMessengerReplyTempBlockByLoginFail						= -12,
	kMessengerReplyMabiInfoSoapFail							= -13,
	kMessengerReplyWrongPassport							= -14,
	kMessengerReplyBlockedIp								= -15,
	kMessengerReplyTempUser									= -16,
	kMessengerReplyNotAuthenticated							= -17,

	kMessengerReplyInvalidExternalPassportType				= -18,
	kMessengerReplyExternalPassportDecoderNotImplemented	= -19,
	kMessengerReplyExternalPassportDecodingFailed			= -20,
	kMessengerReplyWrongExternalPassport					= -21,
	kMessengerReplyExternalPassportTimeout					= -22,
	kMessengerReplyInvalidArgument							= -23,

	kMessengerReplyServerFailed								= -99,
};

enum	NMP2PReplyCode
{
	kP2PReply_OK											= 0,

	kP2PReply_ServerFailed									= -1,
	kP2PReply_CreateRelayObjectFail							= -2,
	kP2PReply_NotAuthenticated								= -3,
	kP2PReply_NotMine										= -4,
};

enum
{
	kErrorNo_Success			= 0,

	kErrorNo_Unknown			= 0xFFFFFFFF,
};

enum
{
	// Messenger Service Support Flag
	kSupportFlag_ChatSession				= 1 << 0,
	kSupportFlag_FileUpDnSession			= 1 << 1,
	kSupportFlag_WBSession					= 1 << 2,
	kSupportFlag_AddFriendConfirm			= 1 << 3,
	kSupportFlag_ServerMessage				= 1 << 4,
	kSupportFlag_GeneralNoteMsg				= 1 << 5,
	kSupportFlag_GameInviteNoteMsg			= 1 << 6,
	kSupportFlag_ItemGiftNoteMsg			= 1 << 7,
	kSupportFlag_RequestSession				= 1 << 8,
	kSupportFlag_GuildNoticeNoteMsg			= 1 << 9,
	kSupportFlag_GuildChatRoomInviteNoteMsg	= 1 << 10,
	kSupportFlag_WarningNoteMsg				= 1 << 11,
	kSupportFlag_GuildChatRoom				= 1 << 12,
	kSupportFlag_GuildNote					= 1 << 13,
	kSupportFlag_GuildWarInviteNote			= 1 << 14,
	kSupportFlag_CustomNotify				= 1 << 15,
	kSupportFlag_NexonEvent					= 1 << 16,		//	경품행사 등 각종 이벤트

	kSupportFlag_All						= 0x7FFFFFFF,
	kSupportFlag_FromGameCode				= 0x80000000,
};

enum
{
	kChatRoomFlag_IsDefaultRoom				= 1 << 0,
	kChatRoomFlag_IsOpen					= 1 << 1,
};

enum
{
	kPatchType_Normal = 0,
	kPatchType_FullPatch,
};

enum
{
	//
	//	204
	//		- 쪽지 차단 리스트 제외
	//
	kVersion_UserDataList		= 204,

	//
	//	201
	//		- AppServer 컨피규레이션 추가
	//
	//	204
	//		- RelayServer 추가
	//
	kVersion_ServerConfigData	= 204,
};

enum
{
	kPlayFeedType_Achievement	= 1,
	kPlayFeedType_Playfeed		= 2,
	kPlayFeedType_PlayResult	= 3,
	kPlayFeedType_PlayLog		= 4,
};

enum	NMP2ChatType
{
	kP2ChatType_Unknown			= 0,

	kP2ChatType_General			= 1,
	kP2ChatType_Photo			= 2,
	kP2ChatType_Achievement		= 3,
};

enum	NMP2ChatError
{
	kP2ChatError_No				= 0,
	kP2ChatError_NotSupported	= 1,
	kP2ChatError_ServerError	= 2,
	kP2ChatError_UploadPhotoFail	
								= 3,
	kP2ChatError_AuthFail		= 4,
};

#pragma pack( push, 4 )

struct NMVirtualKey
{
	NMGameCode	uGameCode;
	UINT32		uVirtualIDCode;
};

bool operator==( const NMVirtualKey& keyLeft, const NMVirtualKey& keyRight );

#define	INITIALIZE_VIRTUALKEY( x )	{ x.uGameCode = 0; x.uVirtualIDCode = 0; }
#define	IS_VIRTUALKEY_NULL( x )		( x.uGameCode == 0 && x.uVirtualIDCode == 0 )
#define	ENCODE_VIRTUALKEY( x )		{ encoder.Encode4( x.uGameCode ); encoder.Encode4( x.uVirtualIDCode ); }
#define	DECODE_VIRTUALKEY( x )		{ x.uGameCode = decoder.Decode4(); x.uVirtualIDCode = decoder.Decode4(); }

extern	NMVirtualKey const	keyVirtualAll;
extern	NMVirtualKey const	keyNexoncom;
extern	NMVirtualKey const	keyKart;

struct NMFriendCode
{
	NMIDCode				uIDCode;
	NMVirtualKey			keyVirtual;
};

bool operator==( const NMFriendCode& fcLeft, const NMFriendCode& fcRight );

#define	INITIALIZE_FRIENDCODE( x )	{ x.uIDCode = 0; INITIALIZE_VIRTUALKEY( x.keyVirtual ); }
#define ENCODE_FRIENDCODE( x )		{ encoder.Encode8( x.uIDCode ); ENCODE_VIRTUALKEY( x.keyVirtual ); }
#define DECODE_FRIENDCODE( x )		{ x.uIDCode = decoder.Decode8(); DECODE_VIRTUALKEY( x.keyVirtual ); }

struct NMFriendKey
{
	NMIDCode				uIDCode;
	NMVirtualKey			keyOwner;
	NMVirtualKey			keyVirtual;

	operator NMFriendCode() const
	{
		NMFriendCode	fcFriendCode;
		fcFriendCode.uIDCode	= uIDCode;
		fcFriendCode.keyVirtual	= keyVirtual;
		return fcFriendCode;
	}
};

bool operator==( const NMFriendKey& fkLeft, const NMFriendKey& fkRight );

#define	INITIALIZE_FRIENDKEY( x )	{ INITIALIZE_VIRTUALKEY( x.keyOwner ); INITIALIZE_VIRTUALKEY( x.keyVirtual ); }
#define	ENCODE_FRIENDKEY( x )		{ encoder.Encode8( x.uIDCode ); ENCODE_VIRTUALKEY( x.keyOwner ); ENCODE_VIRTUALKEY( x.keyVirtual ); }
#define	DECODE_FRIENDKEY( x )		{ x.uIDCode = decoder.Decode8(); DECODE_VIRTUALKEY( x.keyOwner ); DECODE_VIRTUALKEY( x.keyVirtual ); }

#pragma pack( pop )

class	CNMAvatarItemInfo : public CNMSerializable
{
public:

public:
	CNMAvatarItemInfo( void );

	DECLARE_SERIALIZABLE
};

class	CNMAvatarFullInfo : public CNMAvatarItemInfo
{
public:

public:
	CNMAvatarFullInfo( void );
	bool operator==( CNMAvatarFullInfo& rInfo );

	DECLARE_SERIALIZABLE
};

class	CNMDefaultUserInfo : public CNMSerializable
{
public:
	// Permanent Data
	TCHAR					szLoginID[ LOGINID_SIZE ];
	NMIDCode				uIDCode;

	// Changeable Data
	_UInt32_				uFlag;
	TCHAR					szWord[ TODAYWORD_SIZE ];

	// Volatile Data
	NMUSERSTATUS			uStatus;
	_UInt32_				uLocalIP;
	_UInt32_				uVirtualIP;
	_UInt16_				uPort;

public:
	CNMDefaultUserInfo( void );

	DECLARE_SERIALIZABLE
};

class CNMRealFriendInfo : public CNMDefaultUserInfo
{
public:
	enum
	{
		kChangeFlag_All					= 0xFFFFFFFF,

		kChangeFlag_FlagChanged			= 1 << 0,
		kChangeFlag_WordChanged			= 1 << 1,
		kChangeFlag_StatusChanged		= 1 << 2,
		kChangeFlag_NetworkInfoChanged	= 1 << 3,
		kChangeFlag_BirthdayChanged		= 1 << 7,
	};

public:
	CNMAvatarItemInfo		aiAvatarItemInfo;

	SYSTEMTIME				stBirthday;
	_UInt8_					uBirthdayType;

public:
	CNMRealFriendInfo( void );

	DECLARE_SERIALIZABLE
};

class CNMVirtualFriendInfo : public CNMSerializable
{
public:
	enum
	{
		kChangeFlag_All					= 0xFFFFFFFF,

		kChangeFlag_FlagChanged			= 1 << 0,
		kChangeFlag_StatusChanged		= 1 << 1,
		kChangeFlag_NickNameChanged		= 1 << 2,
		kChangeFlag_PositionChanged		= 1 << 3,
		kChangeFlag_LevelChanged		= 1 << 4,
		kChangeFlag_LicenseChanged		= 1 << 5,
		kChangeFlag_MetaDataChanged		= 1 << 6,
		kChangeFlag_MemoChanged			= 1 << 7,

		kVirtualStatus_Offline			= 0,
		kVirtualStatus_SemiOnline		= 1,
		kVirtualStatus_Online			= 2,
	};

public:
	NMFriendKey				keyFriend;

	TCHAR					szNickName[ NICKNAME_SIZE ];
	TCHAR					szPosition[ POSITION_SIZE ];

	TCHAR					szMetaData[ METADATA_SIZE ];
	TCHAR					szMemo[ MEMO_SIZE ];

	_UInt32_				uVirtualStatus;
	_UInt32_				uFlag;					// kUserFlag_XXX 값이 설정됨.
	_UInt32_				uLevel;

public:
	CNMVirtualFriendInfo( void );

	DECLARE_SERIALIZABLE
};

class	CNMFriendInfo : public CNMRealFriendInfo
{
public:
	NMFriendKey				keyFriend;

	TCHAR					szNickName[ NICKNAME_SIZE ];
	TCHAR					szPosition[ POSITION_SIZE ];
	TCHAR					szMetaData[ METADATA_SIZE ];
	TCHAR					szMemo[ MEMO_SIZE ];

	BOOL					bIsVirtualOnline;
	_UInt32_				uLevel;

public:
	CNMFriendInfo( void );

	DECLARE_SERIALIZABLE

	NMFriendCode	GetFriendCode( void );
};

class	CNMRealUserInfo : public CNMDefaultUserInfo
{
	/* Mask Info Bit
	32		: Reserved
	31		: Reserved
	// Open Type	1 : All, 2 : Friends Only, 3 : Closed
	//30, 29	: Mobile Phone(Deprecated) - 플러그에서 사용하지 않고, 신 멤버쉽에서도 사용하지 않으므로 해당 비트는 삭제 처리. 데이터 받아 오지 않음.
	28, 27	: Phone
	26, 25	: Region
	24, 23	: School
	22, 21	: Birth
	20, 19	: Age
	18, 17	: Sex
	16, 15	: Name
	// Info
	14		: AlertState
	13		: Foreigner Confirmed
	12		: Modify Info
	11		: Temp Password
	10		: Blocked By Admin
	9		: Not Owner Confirmed
	8		: Owner Confirmed
	7		: Parent Auth
	6		: Real Name Confirmed
	5		: Main ID
	4		: Foreigner Temp User 
	3		: Child Temp User
	2		: Blocked By Warning 
	1		: Blocked By Login Fail
	*/

public:
	TCHAR					szName[ NXNAME_SIZE ];
	TCHAR					szNickName[ NICKNAME_SIZE ];
	_UInt32_				uMaskInfo;
	NMSEXTYPE				uSex;
	_UInt32_				uAge;
	_UInt32_				uAreaCode;
	TCHAR					szAreaName[ NXAREA_SIZE ];
	_UInt32_				uSchoolCode;
	TCHAR					szSchoolName[ NXSCHOOL_SIZE ];
	SYSTEMTIME				stBirthday;
	CNMAvatarFullInfo		aiAvatarInfo;

	NMALLOWTYPE				uAllowChatType;
	NMALLOWTYPE				uAllowNoteType;
	NMALLOWTYPE				uAllowGameInviteType;
	NMALLOWTYPE				uAllowRequestFriendType;

	_UInt8_					uBirthdayType;
	TCHAR					szMabiID[ LOGINID_SIZE ];

public:
	CNMRealUserInfo( void );

	DECLARE_SERIALIZABLE
};

class	CNMCategoryInfo;
class	CNMSelfUserInfo : public CNMRealUserInfo
{
public:
	CNMSelfUserInfo( void );

	virtual void	OnHashValueUpdated( _UInt32_ uNewHashValue ) = 0;

	virtual void	OnCateFriendOperationError( LPCTSTR szMsg ) = 0;
	virtual void	OnNoteBoxOperationError( LPCTSTR szMsg ) = 0;
	virtual void	OnRejectedUserListOperationError( LPCTSTR szMsg ) = 0;

	virtual void	OnRealFriendListChanged( void ) = 0;
	virtual void	OnRealFriendInfoDeleted( NMIDCode uIDCode ) = 0;
	virtual void	OnRealFriendInfoChanged( _UInt32_ uChangeFlag, CNMRealFriendInfo* pRealFriendInfo, BOOL bNeedNotify ) = 0;

	virtual void	OnVirtualFriendListChanged( void ) = 0;
	virtual void	OnVirtualFriendInfoDeleted( NMFriendKey	keyFriend ) = 0;
	virtual void	OnVirtualFriendInfoChanged( _UInt32_ uChangeFlag, CNMVirtualFriendInfo* pVirtualFriendInfo ) = 0;

	virtual void	OnCategoryListChanged( void ) = 0;
	virtual void	OnCategoryInfoChanged( void ) = 0;

	virtual void	OnCateFriendMapChanged( void ) = 0;

	virtual void	OnRejectedUserListChanged( void ) = 0;
};

class	CNMVirtualUserInfo : public CNMSerializable
{
public:
	NMVirtualKey			keyVirtual;
	TCHAR					szNickName[ NICKNAME_SIZE ];
	TCHAR					szPosition[ POSITION_SIZE ];
	TCHAR					szMetaData[ METADATA_SIZE ];
	BOOL					bIsVirtualOnline;
	BOOL					bIsStealth;
	_UInt32_				uLoginTick;
	_UInt32_				uLevel;

public:
	CNMVirtualUserInfo( void );

	DECLARE_SERIALIZABLE
};

typedef std::vector< CNMVirtualUserInfo >		NMVirtualUserList;

typedef std::vector< CNMRealFriendInfo >		NMRealFriendList;
typedef	std::vector< CNMVirtualFriendInfo >		NMVirtualFriendList;
typedef std::vector< CNMFriendInfo >			NMFriendList;
typedef std::vector< NMFriendCode >				NMFriendCodeList;
typedef std::vector< NMFriendKey >				NMFriendKeyList;

class	CNMFriendList : public CNMSerializable
{
public:
	NMFriendList	aMembers;

public:
	CNMFriendList( void );

	DECLARE_SERIALIZABLE
};

class	CNMCategoryInfo : public CNMSerializable
{
public:
	_UInt32_				uCategoryCode;
	NMVirtualKey			keyOwner;
	TCHAR					szCategoryName[ CATEGORY_SIZE ];
	_UInt8_					uProperty;
	_UInt8_					uAllowType;

public:
	CNMCategoryInfo( void );

	DECLARE_SERIALIZABLE
};

class	CNMCateFriendInfo : public CNMCategoryInfo
{
public:
	NMFriendList			aFriends;

public:
	CNMCateFriendInfo( void );

	DECLARE_SERIALIZABLE
};

typedef std::vector< CNMCateFriendInfo >	NMCategoryUserList;

//////////////////////////////////////////////////////////////////////////
//	CNMGameCharacter
//////////////////////////////////////////////////////////////////////////

class CNMCharacter : public CNMSerializable
{
public:
	TCHAR			szLoginID[ LOGINID_SIZE ];
	NMVirtualKey	vkey;
	TCHAR			szNickname[ NICKNAME_SIZE ];

public:
	CNMCharacter();
	CNMCharacter(NMGameCode gameCode, LPCTSTR pszNickname, LPCTSTR pszLoginID = NULL, UINT32 uVirtualIDCode = 0);
	CNMCharacter(NMVirtualKey vkey, LPCTSTR pszNickname, LPCTSTR pszLoginID = NULL);

	DECLARE_SERIALIZABLE

public:
	NMGameCode		GetGameCode() const { return this->vkey.uGameCode; }
	void			SetGameCode(NMGameCode gameCode) { this->vkey.uGameCode = gameCode; }
	NMVirtualKey	GetVKey() const { return this->vkey; }

	void			NormalizeVirtualKey()
	{
		if (this->vkey.uGameCode == kGameCode_NULL) this->vkey.uGameCode = kGameCode_nexoncom;
		if (this->vkey.uVirtualIDCode == 0) this->vkey.uVirtualIDCode = 1;
	}

	LPCTSTR			GetName() const { return this->szNickname; }
};

typedef std::vector< CNMCharacter >	NMCharacterList;

//////////////////////////////////////////////////////////////////////////
//	CNMRejectedUser
//////////////////////////////////////////////////////////////////////////

class CNMRejectedUser : public CNMSerializable
{
public:
	CNMCharacter	rejectedCharacter;
	TCHAR			szMemo[ MEMO_SIZE ];

public:
	CNMRejectedUser();
	CNMRejectedUser(CNMCharacter const & rejectedCharacter, LPCTSTR pszMemo = NULL);

	DECLARE_SERIALIZABLE

public:
	const CNMCharacter & GetCharacter() const { return rejectedCharacter; }
	CNMCharacter & GetCharacter() { return rejectedCharacter; }
};

typedef std::vector<CNMRejectedUser> NMRejectedUserList;

//////////////////////////////////////////////////////////////////////////
//	CNMNoteInfo
//////////////////////////////////////////////////////////////////////////

class CNMNoteInfo2;		//	forward declaration

class CNMNoteInfo : public CNMSerializable
{
public:
	enum	NMNOTETYPE
	{
		kNoteType_NULL					= 0,

		kNoteType_GeneralMsg			= 1,
		kNoteType_GameInvite			= 2,
		kNoteType_ItemGift				= 3,
		kNoteType_GuildNotice			= 4,
		kNoteType_GuildChatRoomInvite	= 5,
		kNoteType_Warning				= 6,
		kNoteType_Guild					= 7,
		kNoteType_AdminNotice			= 8,

		kNoteType_NexonPlay				= 10,
	};

	enum	NMNOTESUBTYPE
	{
		kNoteSubType_NULL				= 0,

		kNoteSubType_GuildNotice		= 1,
		kNoteSubType_GuildNotify		= 2,
		kNoteSubType_GuildChallenge		= 3,
		kNoteSubType_GuildAlly			= 4,
		kNoteSubType_GuildWarInvite		= 5,
	};

	// Note Info
	_UInt32_				uNoteSerial;
	NMNOTETYPE				uNoteType;
	_UInt32_				uSubType;
	NMVirtualKey			keyVirtual;
	union
	{
		BOOL				bReadFlag;
		BOOL				bSendFlag;
	};
	SYSTEMTIME				stTime;

	// User Info
	TCHAR					szTargetID[ TARGETID_SIZE ];

	TCHAR					szFromLoginID[ LOGINID_SIZE ];
	TCHAR					szFromNickName[ NICKNAME_SIZE ];

	// Contents
	TCHAR					szMessage[ NOTEMSG_SIZE ];
	TCHAR					szArgument[ NOTEARG_SIZE ];
	TCHAR					szCustomData[ NOTECUSTOM_SIZE ];
	CNMAvatarFullInfo		aiAvatarInfo;

public:
	CNMNoteInfo( void );
	CNMNoteInfo( CNMNoteInfo2 & rhs );

	DECLARE_SERIALIZABLE
};

typedef std::vector< CNMNoteInfo >	NMNoteList;

class	CNMNoteInfoEx : public CNMSerializable
{
public:
	enum	NMNOTEEXTYPE
	{
		kNoteExType_NULL				= 0,

		kNoteExType_GameErrorReport		= 1,
	};

	NMNOTEEXTYPE			uNoteExType;
	TCHAR					szMessage[ NOTEEXMSG_SIZE ];

public:
	CNMNoteInfoEx( void );

	DECLARE_SERIALIZABLE
};

//////////////////////////////////////////////////////////////////////////
//	CNMNoteInfo2
//////////////////////////////////////////////////////////////////////////

class CNMNoteInfo2 : public CNMSerializable
{
	//--------------------------------------------------------------------
	//	enumerators
	//--------------------------------------------------------------------
public:

	enum	NMNOTETYPE
	{
		kNoteType_NULL					= 0,
		kNoteType_GeneralMsg			= 1,
		kNoteType_GameInvite			= 2,
		kNoteType_ItemGift				= 3,
		kNoteType_GuildNotice			= 4,
		kNoteType_GuildChatRoomInvite	= 5,
		kNoteType_Warning				= 6,
		kNoteType_Guild					= 7,
		kNoteType_AdminNotice			= 8,
		
		kNoteType_NexonPlayMsg			= 9,
	};

	enum	NMNOTESUBTYPE
	{
		kNoteSubType_NULL				= 0,
		kNoteSubType_GuildNotice		= 1,
		kNoteSubType_GuildNotify		= 2,
		kNoteSubType_GuildChallenge		= 3,
		kNoteSubType_GuildAlly			= 4,
		kNoteSubType_GuildWarInvite		= 5,
	};

	//--------------------------------------------------------------------
	//	constructor
	//--------------------------------------------------------------------
public:
	CNMNoteInfo2();
	CNMNoteInfo2( CNMNoteInfo & rhs );
	CNMNoteInfo2 & operator = ( CNMNoteInfo & rhs );

	DECLARE_SERIALIZABLE

	//--------------------------------------------------------------------
	//	service functions
	//--------------------------------------------------------------------
public:
	CNMCharacter const & GetSender() const { return this->sender; }
	CNMCharacter const & GetReceiver() const { return (!receiverList.empty()) ? receiverList[0] : defaultReceiver; }
	void SetReceiverVKey( NMVirtualKey keyVirtual )
	{	
		if ( !receiverList.empty() )		
			receiverList[ 0 ].vkey	= keyVirtual;	
	};
	void SetSender( CNMCharacter const & sender ) { this->sender = sender; }
	void SetReceiver( CNMCharacter const & receiver );

	//--------------------------------------------------------------------
	//	data members
	//--------------------------------------------------------------------
public:

	//	note info
	UINT32				uNoteSerial;
	NMNOTETYPE			uNoteType;
	NMNOTESUBTYPE		uNoteSubType;
	SYSTEMTIME			stTime;
	BOOL				bReadFlag;		//	read or not read
	BOOL				bSendFlag;		//	send or received

	//	contents
	TCHAR				szMessage[ NOTEMSG_SIZE ];
	TCHAR				szArgument[ NOTEARG_SIZE ];
	TCHAR				szCustomData[ NOTECUSTOM_SIZE ];

	//	sender
	CNMCharacter		sender;

	//	receivers
	NMCharacterList		receiverList;

private:
	//	used when there is no receiver in the receiver list
	static const CNMCharacter	defaultReceiver;
};

typedef std::vector< CNMNoteInfo2 > NMNoteInfoList;
typedef std::vector< CNMNoteInfo2 >	NMNoteList2;

//////////////////////////////////////////////////////////////////////////
//	ID list <-> Character list
//////////////////////////////////////////////////////////////////////////

namespace CNMNoteInfoHelper
{
	BOOL CharacterListToString( NMCharacterList const & characterList, LPTSTR pszBuffer, size_t nBufferSize );
	BOOL StringToCharacterList( LPCTSTR pszBuffer, NMCharacterList & characterList, NMGameCode gameCode );
};

//////////////////////////////////////////////////////////////////////////
//	CNMReportInfo
//////////////////////////////////////////////////////////////////////////

class	CNMReportInfo : public CNMSerializable
{
public:
	enum	NMREPORTTYPE
	{
		kReportType_NULL	= 0,
		kReportType_Board,
		kReportType_Guild,
		kReportType_Game,
		kReportType_Note,
		kReportType_Chat,
	};

	enum	NMREPORTSUBTYPE
	{
		kReportSubType_NULL							= 0,

		kReportSubType_AbusiveOrLewdWords			= 1,		//	욕설/음란
		kReportSubType_IllegalData					= 2,		//	불법자료
		kReportSubType_CommercialArticle			= 3,		//	상업적인 글
		kReportSubType_CopyrightViolation			= 9,		//	저작권 위반
		kReportSubType_PretendedToAdmin				= 11,		//	운영자 사칭
		kReportSubType_SpamOrAdvertisement			= 16,		//	스팸/광고
		kReportSubType_Disturbance					= 17,		//	상대방 비방
		kReportSubType_MoneyTrade					= 18,		//	현금거래
		kReportSubType_RequestedPrivateInfo			= 19,		//	개인 정보 요구
		
		kReportSubType_Etc							= 255,		//	기타
	};

	NMREPORTTYPE			uType;
	NMREPORTSUBTYPE			uSubType;
	NMGameCode				uGameCode;
	NMGameCode				uAppliedGameCode;
	TCHAR					szTargetNickname[ NICKNAME_SIZE ];
	TCHAR					szReason[ 4000 ];
	TCHAR					szEvidence[ 20000 ];
	UINT32					uNoteSerial;
	TCHAR					szReporterNickname[ NICKNAME_SIZE ];

public:
	CNMReportInfo( void );

	DECLARE_SERIALIZABLE
};

class CNMChannelMessage;
class CNMChatMessage : public CNMSerializable
{
public:
	NMVirtualKey			keyVirtual;
	CNMAvatarItemInfo		aiAvatarItemInfo;

	TCHAR					szSenderName[ CHATSENDER_SIZE ];
	TCHAR					szChatHeader[ CHATHDR_SIZE ];
	TCHAR					szFontHeader[ CHATHDR_SIZE ];
	TCHAR					szChatMessage[ CHATMSG_SIZE ];

	UINT32					uFlag;

public:
	CNMChatMessage();
	CNMChatMessage( CNMChannelMessage const & rhs, UINT32 uGameCode );

	DECLARE_SERIALIZABLE
};

typedef std::vector< CNMChatMessage >	NMChatMessageList;

class	CNMFileEvent : public CNMSerializable
{
public:
	enum	NMFILESTATUS
	{
		kFileStatus_Unknown			= 0,

		kFileStatus_WaitSend		= 1,
		kFileStatus_SendFile		= 2,
		kFileStatus_SendComplete	= 3,
		kFileStatus_WaitConfirm		= 4,
		kFileStatus_GetFile			= 5,
		kFileStatus_GetComplete		= 6,

		kFileStatus_FileOpenError	= 100,
		kFileStatus_FileReadError	= 101,
		kFileStatus_FileWriteError	= 102,
		kFileStatus_NetworkError	= 103,
		kFileStatus_SessionError	= 104,
		kFileStatus_CantSupport		= 105,

		kFileStatus_UserAbort		= 200,
	};

	_UInt32_				uEventId;
	TCHAR					szFileName[ FILENAME_SIZE ];
	NMFILESTATUS			uStatus;
	_UInt32_				uParam1;
	_UInt32_				uParam2;
	BOOL					bSendFlag;

public:
	CNMFileEvent( void );

	DECLARE_SERIALIZABLE
};

typedef std::vector< CNMFileEvent >		NMFileEventList;

class	CNMFilePath : public CNMSerializable
{
public:
	TCHAR	szValue[ MAX_PATH ];

public:
	CNMFilePath( void );

	DECLARE_SERIALIZABLE
};

typedef std::vector< CNMFilePath >		NMFilePathList;

class	CNMMyGameInfo : public CNMSerializable
{
public:
	NMGameCode				aGameCodes[ MYGAME_COUNT ];

public:
	CNMMyGameInfo( void );

	DECLARE_SERIALIZABLE
};

class CNMGuildInfoEx : public CNMSerializable
{
public:
	NMGuildKey	uGuildKey;

	TCHAR		szName[ GUILDNAME_SIZE ];
	TCHAR		szId[ GUILDID_SIZE ];
	NMGameCode	uGameCode;

	TCHAR		szHomeUrl[ GUILDHOME_SIZE ];
	UINT32		uLevel;
	UINT32		uMemberCount;
	BOOL		bHasNewContent;

	BOOL		bIsMaster;

public:
	CNMGuildInfoEx();

public:
	NMGuildID GetGuildID() const;

	DECLARE_SERIALIZABLE
};

typedef std::vector< CNMGuildInfoEx >		NMGuildInfoExList;

class	CNMGuildListExInfo : public CNMSerializable
{
public:
	NMGuildInfoExList			aGuilds;

public:
	CNMGuildListExInfo( void );

	DECLARE_SERIALIZABLE
};

class	CNMFindUserRequestInfo : public CNMSerializable
{
public:
	_UInt32_				uToken;

	TCHAR					szName[ NXNAME_SIZE ];
	_UInt32_				uAgeMin;
	_UInt32_				uAgeMax;
	NMSEXTYPE				uSex;
	_UInt32_				uRegion;
	_UInt32_				uSchoolCode;
	BOOL					bOnlineUserOnly;

public:
	CNMFindUserRequestInfo( void );

	DECLARE_SERIALIZABLE
};

class	CNMFindUserInfo : public CNMSerializable
{
public:
	NMIDCode				uIdCode;
	TCHAR					szId[ LOGINID_SIZE ];
	TCHAR					szName[ NXNAME_SIZE ];	// NULL String : closed
	TCHAR					szNickName[ NICKNAME_SIZE ];
	_UInt32_				uAge;					// 0 : closed
	NMSEXTYPE				uSex;					// 0 : closed
	_UInt32_				uAreaCode;				// 0 : closed
	_UInt32_				uSchoolCode;			// 0 : closed
	CNMAvatarItemInfo		aiAvatarInfo;
	NMUSERSTATUS			uStatus;		

public:
	CNMFindUserInfo( void );

	DECLARE_SERIALIZABLE
};

typedef std::vector< CNMFindUserInfo >	NMFindUserInfoList;

class CNMChannelMemberInfo;
class CNMGuildMemberInfo : public CNMSerializable
{
public:
	NMIDCode				uIdCode;
	TCHAR					szLoginId[ LOGINID_SIZE ];
	TCHAR					szNickName[ NICKNAME_SIZE ];
	CNMAvatarItemInfo		aiAvatarInfo;
	BOOL					bIsGameOnline;
	UINT8					uLevel;
	UINT32					uGameLevel;
	UINT32					uVirtualIdCode;

public:
	CNMGuildMemberInfo();
	CNMGuildMemberInfo( CNMChannelMemberInfo const & channelMemberInfo );

	DECLARE_SERIALIZABLE
};

#ifdef _NMSERVER
typedef std::multimap< _UInt64_, CNMGuildMemberInfo >			NMGuildMemberList;
typedef NMGuildMemberList::iterator								guildmemberlist_iter;
typedef std::pair< guildmemberlist_iter, guildmemberlist_iter >	guildmemberlist_pair;
#else
typedef std::vector< CNMGuildMemberInfo >						NMGuildMemberList;
#endif

class CNMClientChannelInfo;
class CNMChannelMemberInfoList;
class CNMGuildOnlineInfoEx : public CNMSerializable
{
public:
	NMGuildKey				uGuildKey;
	UINT32					uChatRoomSerialNo;
	UINT32					uChatRoomMemberCount;
	TCHAR					szChatServerIpAddr[ 32 ];
	UINT16					uChatServerPort;
	NMGuildMemberList		aOnlineGuildMembers;
	UINT32					uGameCode;

public:
	CNMGuildOnlineInfoEx();
	CNMGuildOnlineInfoEx( CNMClientChannelInfo const & clientChannelInfo, CNMChannelMemberInfoList const & channelMemberInfoList );

public:
	NMGuildID GetGuildID() const;

	DECLARE_SERIALIZABLE
};

typedef std::map< NMGuildID, CNMGuildOnlineInfoEx, NMGuildIDLess >	NMGuildOnlineInfoMap;

class	CNMGuildOnlineInfo : public CNMSerializable
{
public:
	_UInt32_				uGuildSerialNo;
	_UInt32_				uChatRoomSerialNo;
	_UInt32_				uChatRoomMemberCount;
	TCHAR					szChatServerIpAddr[ 32 ];
	_UInt16_				uChatServerPort;
	NMGuildMemberList		aOnlineGuildMembers;

public:
	CNMGuildOnlineInfo( void );
	CNMGuildOnlineInfo&		operator=( CNMGuildOnlineInfoEx& infoEx );
	operator CNMGuildOnlineInfoEx( void );

	DECLARE_SERIALIZABLE
};

class	CNMSessionInfo : public CNMSerializable
{
public:
	enum
	{
		// session status
		kStatus_ConnectingToServer		= 0,
		kStatus_ConnectionEstablished,
		kStatus_WaitingForInviteUser,
		kStatus_SessionEstablished,
		kStatus_SwitchingServer,
		kStatus_WaitingForOtherUser,
		kStatus_ConnectionClosed,
		kStatus_ConnectionFailed,

		// session mode
		kMode_BiConnMode				= 0,
		kMode_MultiConnMode,

		// invite mode
		kInviteMode_Friend				= 0,
		kInviteMode_GuildMember,
		kInviteMode_Stranger,
	};

public:
	// Global
	NMSESSIONTYPE			uType;				// Never Change
	NMVirtualKey			keyVirtuality;		// Never Change
	BOOL					bPassive;			// Never Change

	_UInt32_				uInviteMode;		// Change One Time
	_UInt32_				uMode;				// Change One Time
	_UInt64_				uId;				// Change One Time
	NMFriendCode			fcAssocFriend;		// Change One Time

	_UInt32_				uStatus;			// Frequently Change
	_UInt32_				uServingProcessID;

public:
	CNMSessionInfo( void );

	DECLARE_SERIALIZABLE
};

class	CNMSessionMemberInfo : public CNMSerializable
{
public:
	NMIDCode				uIDCode;
	TCHAR					szLoginID[ LOGINID_SIZE ];
	TCHAR					szWord[ TODAYWORD_SIZE ];

	NMVirtualKey			keyVirtual;
	TCHAR					szNickName[ NICKNAME_SIZE ];
	_UInt32_				uStatus;
	CNMAvatarFullInfo		aiAvatarInfo;

public:
	CNMSessionMemberInfo( void );

	DECLARE_SERIALIZABLE
};

typedef std::vector< CNMSessionMemberInfo >	NMSessionMemberList;

class	CNMSessionMemberList : public CNMSerializable
{
public:
	NMSessionMemberList		aMembers;

public:
	CNMSessionMemberList( void );

	DECLARE_SERIALIZABLE
};

class CNMChannelMemberInfo;
class CNMChatRoomMemberInfo : public CNMRealUserInfo
{
public:
	TCHAR					szTel[ NXTEL_SIZE ];
	NMVirtualKey			keyVirtual;
	UINT32					uLevel;
	TCHAR					szLevelName[ GUILDLEVEL_SIZE ];
	UINT32					uFlag;
	UINT32					uIpAddr;

public:
	CNMChatRoomMemberInfo();
	CNMChatRoomMemberInfo( CNMChannelMemberInfo const & rhs, UINT32 uGameCode );

	DECLARE_SERIALIZABLE
};

typedef std::vector< CNMChatRoomMemberInfo >	NMChatRoomMemberList;

class	CNMChatRoomMemberList : public CNMSerializable
{
public:
	NMChatRoomMemberList	aMemberList;

public:
	CNMChatRoomMemberList( void );

	DECLARE_SERIALIZABLE
};

class CNMGuildMemberLevelInfo : public CNMSerializable
{
public:
	_UInt8_	uLevel;
	TCHAR	szName[ GUILDLEVEL_SIZE ];

public:
	CNMGuildMemberLevelInfo( void );

	DECLARE_SERIALIZABLE
};

typedef std::vector< CNMGuildMemberLevelInfo >	NMGuildMemberLevelList;

class	CNMClientChannelInfo;
class	CNMChatRoomInfoEx : public CNMSerializable
{
public:
	enum
	{
		// chat room status
		kStatus_ConnectingToServer		= 0,
		kStatus_ConnectionEstablished,
		kStatus_SessionEstablished,
		kStatus_ConnectionClosed,
		kStatus_ConnectionFailed,
	};

public:
	// Room Info

	_UInt32_				uRoomType;
	NMGuildKey				uGuildKey;
	_UInt32_				uRoomSerialNo;

	NMGameCode				uGameCode;
	TCHAR					szGuildTitle[ GUILDNAME_SIZE ];
	TCHAR					szRoomTitle[ ROOMTITLE_SIZE ];
	_UInt32_				uLimitLevel;
	_UInt32_				uMaxSize;
	_UInt32_				uFlag;
	TCHAR					szPassword[ ROOMPWD_SIZE ];

	NMIDCode				uOwnerIdCode;

	NMGuildMemberLevelList	aLevelInfo;

	_UInt32_				uStatus;
	_UInt32_				uRequestProcId;

public:
	CNMChatRoomInfoEx( void );
	CNMChatRoomInfoEx( CNMClientChannelInfo const & rhs );
	CNMChatRoomInfoEx & operator = ( CNMClientChannelInfo const & rhs );

	DECLARE_SERIALIZABLE
};

#ifndef	_NMSERVER
class	CNMChatRoomInfo : public CNMSerializable
{
public:
	enum
	{
		// chat room status
		kStatus_ConnectingToServer		= 0,
		kStatus_ConnectionEstablished,
		kStatus_SessionEstablished,
		kStatus_ConnectionClosed,
		kStatus_ConnectionFailed,
	};

public:
	// Room Info
	_UInt32_				uRoomType;
	_UInt32_				uClubSerialNo;
	_UInt32_				uRoomSerialNo;

	NMGameCode				uGameCode;
	TCHAR					szClubTitle[ GUILDNAME_SIZE ];
	TCHAR					szRoomTitle[ ROOMTITLE_SIZE ];
	_UInt32_				uLimitLevel;
	_UInt32_				uMaxSize;
	_UInt32_				uFlag;
	TCHAR					szPassword[ ROOMPWD_SIZE ];

	NMIDCode				uOwnerIdCode;

	TCHAR					szLevel1Name[ GUILDLEVEL_SIZE ];
	TCHAR					szLevel2Name[ GUILDLEVEL_SIZE ];
	TCHAR					szLevel3Name[ GUILDLEVEL_SIZE ];
	TCHAR					szLevel4Name[ GUILDLEVEL_SIZE ];
	TCHAR					szLevel5Name[ GUILDLEVEL_SIZE ];

	_UInt32_				uStatus;
	_UInt32_				uRequestProcId;

public:
	CNMChatRoomInfo( void );
	CNMChatRoomInfo&		operator=( CNMChatRoomInfoEx& infoEx );
	operator CNMChatRoomInfoEx( void );
	DECLARE_SERIALIZABLE
};
#endif	// _NMSERVER

class CNMMessageData : public CNMSerializable
{
public:
	INT32					nCode;
	TCHAR					szMessage[ SERVER_MSG_SIZE ];

public:
	CNMMessageData( void );
	CNMMessageData( INT32 nCode, LPCTSTR pszMessage = NULL );

	DECLARE_SERIALIZABLE
};

// CallFunc	:	szRequestID, keyToVirtual, keyFromVirtual, uInsertCategoryCode(ignored), szMessage, bAddToNxFriend
// OnEvent	:	uSerialNo, keyToVirtual, keyFromVirtual, szFromLoginID, szFromNickName, szMessage

class CNMRequestNewFriendData : public CNMSerializable
{
public:
	_UInt32_				uSerialNo;

	TCHAR					szRequestID[ LOGINID_SIZE ];
	NMGameCode				uRequesteeGameCode;
	NMVirtualKey			keyToVirtual;

	TCHAR					szFromLoginID[ LOGINID_SIZE ];
	NMVirtualKey			keyFromVirtual;
	TCHAR					szFromNickName[ NICKNAME_SIZE ];

	_UInt32_				uInsertCategoryCode;
	TCHAR					szMessage[ MSG_SIZE ];

	BOOL					bAddToNxFriend;

public:
	CNMRequestNewFriendData( void );

	DECLARE_SERIALIZABLE
};

class CNMRequestSessionData : public CNMSerializable
{
public:
	_UInt32_				uDataKey;
	_UInt32_				uSessionType;
	TCHAR					szLoginId[ LOGINID_SIZE ];
	TCHAR					szNickName[ NICKNAME_SIZE ];

public:
	CNMRequestSessionData( void );

	DECLARE_SERIALIZABLE
};

class CNMGameInfo : public CNMSerializable
{
public:
	enum
	{
		kMask_CanExecute			= ( 0x1 << 0 ),
		kMask_Visible				= ( 0x1 << 1 ),
		kMask_MainID				= ( 0x1 << 2 ),
		kMask_New					= ( 0x1 << 3 ),
		kMask_Hot					= ( 0x1 << 4 ),
		kMask_RecommandedOver19		= ( 0x1 << 5 ),
		kMask_RecommandedUnder19	= ( 0x1 << 6 ),
		kMask_NotMainIDNotTemp		= ( 0x1 << 7 ),
		kMask_GetUserInstallPath	= ( 0x1 << 8 ),
	};

public:
	// id info
	NMGameCode				uGameCode;
	NMGameCode				uGameCodePrerequired;
	TCHAR					szID[ 32 ];
	TCHAR					szName[ 64 ];
	TCHAR					szCustomID[ 32 ];

	// executable info
	TCHAR					szRegistryRoot[ 1024 ];
	TCHAR					szRegistryExePath[ 1024 ];
	TCHAR					szSetupFileUrl[ 1024 ];

	// property info
	_UInt8_					uAgeLimit;
	_UInt32_				uMask;

	// aditional info
	TCHAR					szIconUrl[ 1024 ];
	TCHAR					szHomeUrl[ 1024 ];

	// for compatibility
	NMGameCode				uOldGameCode;

public:
	CNMGameInfo( void );

	// mask
	BOOL	CanExecute( void )				{ return ( this->uMask & kMask_CanExecute ) ? TRUE : FALSE; }
	BOOL	IsVisable( void )				{ return ( this->uMask & kMask_Visible ) ? TRUE : FALSE; }
	BOOL	NeedMainID( void )				{ return ( this->uMask & kMask_MainID ) ? TRUE : FALSE; }
	BOOL	IsNew( void )					{ return ( this->uMask & kMask_New ) ? TRUE : FALSE; }
	BOOL	IsHot( void )					{ return ( this->uMask & kMask_Hot ) ? TRUE : FALSE; }
	BOOL	IsRecommendedOver19( void )		{ return ( this->uMask & kMask_RecommandedOver19 ) ? TRUE : FALSE; }
	BOOL	IsRecommandedUnder19( void )	{ return ( this->uMask & kMask_RecommandedUnder19 ) ? TRUE : FALSE; }
	BOOL	IsNotMainIDNotTemp( void )		{ return ( this->uMask & kMask_NotMainIDNotTemp ) ? TRUE : FALSE; }
	BOOL	GetUserInstallPath( void )		{ return ( this->uMask & kMask_GetUserInstallPath ) ? TRUE : FALSE; }

	DECLARE_SERIALIZABLE
};

typedef std::vector< CNMGameInfo >			NMGameInfoList;

class CNMGameListData : public CNMSerializable
{
public:
	NMGameInfoList			aGameInfo;

public:
	CNMGameListData( void );

	DECLARE_SERIALIZABLE
};

class CNMCustomNotifyData : public CNMSerializable
{
public:
	enum
	{
		//	Custom Notify
		kCustomNotify_NULL			= 0,

		kCustomNotify_LevelChanged	= 4,
		kCustomNotify_ObtainLicence	= 5,
		kCustomNotify_ObtainItem	= 6,
		kCustomNotify_StageClear	= 7,
		kCustomNotify_NxOPP			= 8,
	};

	enum
	{
		//	Custom notify policy
		kCustomNotifyPolicy_Default		= 0,

		kCustomNotifyPolicy_NonVolatile	= 1 << 1,		//	로그오프 한 사람이 나중에 로그온 할 때 볼 수 있음
		kCustomNotifyPolicy_Multiple	= 1 << 2,		//	같은 플래그에 대해서 여러개의 알림이 올 수 있음
	};

	UINT32			uType;

	NMIDCode		uToIDCode;
	NMVirtualKey	keyToVirtual;

	NMIDCode		uFromIDCode;
	NMVirtualKey	keyFromVirtual;
	TCHAR			szFromLoginID[ LOGINID_SIZE ];
	TCHAR			szFromNickName[ NICKNAME_SIZE ];

	TCHAR			szArgument[ MSG_SIZE ];
	SYSTEMTIME		stDateTime;

	UINT32			uPolicyFlag;						//	알림 정책 플래그

public:
	CNMCustomNotifyData( void );

	DECLARE_SERIALIZABLE
};

class CNMGameCode : public CNMSerializable
{
public:
	NMGameCode	uGameCode;

public:
	CNMGameCode( void );
	CNMGameCode( UINT32 uGameCode );

	DECLARE_SERIALIZABLE
};

typedef std::vector< CNMGameCode >				NMGameCodeList;

class CNMGameServerCode : public CNMSerializable
{
public:
	NMGameServerCode	uGameServerCode;

public:
	CNMGameServerCode( void );
	CNMGameServerCode( UINT32 uGameServerCode );

	DECLARE_SERIALIZABLE
};

typedef std::vector< CNMGameServerCode >			NMGameServerCodeList;

class CNMConnConfig : public CNMSerializable
{
public:
	TCHAR						szLoginServerIp[ IPADDR_SIZE ];
	UINT16						uLoginServerPort;

	TCHAR						szStatServerIp[ IPADDR_SIZE ];
	UINT16						uStatServerPort;

	BOOL						bAutoPatch;

	StringZip< IPADDR_SIZE >	szAuthServerIp;
	UINT16						uAuthServerPort;

public:
	CNMConnConfig();
	CNMConnConfig( LPCTSTR szLoginServerIp, UINT16 uLoginServerPort, LPCTSTR szStatServerIp, UINT16 uStatServerPort, BOOL bAutoPatch = TRUE, LPCTSTR szAuthServerIp = NULL, UINT16 uAuthServerPort = 0 );

	CNMConnConfig( CNMConnConfig const & rhs );
	CNMConnConfig & operator = ( CNMConnConfig const & rhs );

	DECLARE_SERIALIZABLE
};

#ifdef _NMSERVER
#define IS_SERVER_LOCALE_KOR		( CNxDBManager::GetInstance().GetServerDBLocale() == 0 )
#define IS_SERVER_LOCALE_CHN		( CNxDBManager::GetInstance().GetServerDBLocale() == 1 )
#define IS_SERVER_LOCALE_TPE		( CNxDBManager::GetInstance().GetServerDBLocale() == 2 )
#define IS_SERVER_LOCALE_JPN		( CNxDBManager::GetInstance().GetServerDBLocale() == 3 )
#define IS_SERVER_LOCALE_USA		( CNxDBManager::GetInstance().GetServerDBLocale() == 4 )
#define IS_SERVER_LOCALE_THA		( CNxDBManager::GetInstance().GetServerDBLocale() == 5 )
#define IS_SERVER_LOCALE_EU			( CNxDBManager::GetInstance().GetServerDBLocale() == 6 )
#endif	// #ifdef _NMSERVER

//-----------------------------------------------------------------------------
//	CNMChannelInfo
//-----------------------------------------------------------------------------

class CNMChannelInfo : public CNMSerializable
{
public:
	enum
	{
		GuildNameLength				= GUILDNAME_SIZE,
		TitleLength					= ROOMTITLE_SIZE,
		PasswordLength				= ROOMPWD_SIZE,

		UserType_Nexon				= 0,
		UserType_Open				= 1,

		ChannelType_General			= 0,
		ChannelType_GuildDefault	= 1,	// only if userType is 'UserType_Nexon'
		ChannelType_Guild			= 2,	// only if userType is 'UserType_Nexon'
	};

	// key data
	INT32							channelSN;

	INT32							userType;
	INT32							channelType;
	INT32							gameCode;
	INT32							serverCode;		// 사실상 GuildCode 와 동일

	// common data
	StringZip< TitleLength >		title;
	StringZip< PasswordLength >		password;
	INT32							maxUserCount;
	UINT32							flag;

	INT32							masterUserSN;
	INT32							masterCharacterSN;

	// guild data
	INT32							guildSN;
	StringZip< GuildNameLength >	guildName;

public:
	CNMChannelInfo();
	CNMChannelInfo( CNMChannelInfo const & rhs );
	CNMChannelInfo & operator = ( CNMChannelInfo const & rhs );

public:
	NMGuildID GetGuildID() const
	{
		return NMGuildID( this->gameCode, this->serverCode, this->guildSN );
	}

	DECLARE_SERIALIZABLE
};

//-----------------------------------------------------------------------------
//	CNMClientChannelInfo
//-----------------------------------------------------------------------------

class CNMClientChannelInfo : public CNMChannelInfo
{
public:
	enum
	{
		kStatus_ConnectingToServer		= 0,
		kStatus_ConnectionEstablished,
		kStatus_SessionEstablished,
		kStatus_ConnectionClosed,
		kStatus_ConnectionFailed,
	};

public:
	INT32							nStatus;
	UINT32							uRequestProcessId;

public:
	CNMClientChannelInfo();
	CNMClientChannelInfo( CNMClientChannelInfo const & rhs );
	CNMClientChannelInfo & operator = ( CNMClientChannelInfo const & rhs );
	CNMClientChannelInfo( CNMChannelInfo const & channelInfo );
	CNMClientChannelInfo & operator = ( CNMChannelInfo const & channelInfo );
	CNMClientChannelInfo( CNMChatRoomInfoEx const & rhs );
	CNMClientChannelInfo & operator = ( CNMChatRoomInfoEx const & rhs );

	DECLARE_SERIALIZABLE
};

//-----------------------------------------------------------------------------
//	CNMChannelMemberInfo
//-----------------------------------------------------------------------------

class CNMChannelMemberInfo : public CNMSerializable
{
public:
	enum
	{
		NameLength					= NICKNAME_SIZE,
		MetaDataLength				= 1024,

		Flag_ReceiveMessage			= 0x1 << 0,
	};

public:
	// key data
	INT32							userSN;
	INT32							characterSN;

	// user data
	StringZip< NameLength >			name;
	StringZip< MetaDataLength >		metaData;
	UINT32							flag;

public:
	CNMChannelMemberInfo();
	CNMChannelMemberInfo( CNMChannelMemberInfo const & rhs );
	CNMChannelMemberInfo & operator = ( CNMChannelMemberInfo const & rhs );
	CNMChannelMemberInfo( CNMChatRoomMemberInfo const & rhs );
	CNMChannelMemberInfo & operator = ( CNMChatRoomMemberInfo const & rhs );

public:
	bool IsChatEnabled() const
	{
		return ( this->flag & Flag_ReceiveMessage );
	}
	void SetChatEnabled( bool bEnabled )
	{
		this->flag = ( this->flag & ~Flag_ReceiveMessage ) | ( bEnabled ? Flag_ReceiveMessage : 0 );
	}
	INT32 GetNexonSN() const
	{
		return ( this->userSN );
	}

	DECLARE_SERIALIZABLE
};

typedef std::vector< CNMChannelMemberInfo >	NMChannelMemberInfoList;

//-----------------------------------------------------------------------------
//	CNMChannelMemberInfoList
//-----------------------------------------------------------------------------

class CNMChannelMemberInfoList : public CNMSerializable
{
public:
	NMChannelMemberInfoList			members;

public:
	CNMChannelMemberInfoList();
	CNMChannelMemberInfoList( NMChannelMemberInfoList const & channelMemberInfoList );

	DECLARE_SERIALIZABLE
};

//-----------------------------------------------------------------------------
//	CNMChannelMessage
//-----------------------------------------------------------------------------

class CNMChannelMessage : public CNMSerializable
{
public:
	enum
	{
		NameLength					= NICKNAME_SIZE,
		MessageLength				= 4096,
		MetaDataLength				= 1024,

		Type_ChatMessage			= 0,
		Type_ErrorMessage			= 1,
		Type_SystemMessage			= 2,
	};

public:
	INT32							messageType;
	INT32							senderUserSN;
	INT32							senderCharacterSN;
	StringZip< NameLength >			senderName;
	INT32							targetUserSN;
	INT32							targetCharacterSN;
	StringZip< MessageLength >		message;
	StringZip< MetaDataLength >		metaData;
	UINT32							uFlag;

public:
	CNMChannelMessage();
	CNMChannelMessage( CNMChannelMessage const & rhs );
	CNMChannelMessage & operator = ( CNMChannelMessage const & rhs );
	CNMChannelMessage( CNMChatMessage const & rhs, INT32 nSenderUserSN );

	DECLARE_SERIALIZABLE
};

///////////////////////////////////////////////////////////////////////////////
//
//	P2P Service
//
///////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
//	NMP2PErrorCode
//-----------------------------------------------------------------------------

enum NMP2PErrorCode
{
	kP2PError_OK					= 0,
	kP2PError_AlreadyInitialized	= 1,
	
	kP2PError_Failed				= -1,		//	general error
	kP2PError_NotLoggedIn			= -2,		//	Not logged in SSO server
	kP2PError_NotInitialized		= -3,		//	P2P service is not initialized
	kP2PError_InvalidConnection		= -4,		//	Connection is invalid or connecting now...
	kP2PError_InvalidServerInfo		= -5,		//	Relay server info is invalid
	kP2PError_InternalError			= -6,		//	Internal error (maybe kernel object error)
	kP2PError_NotOnline				= -7,		//	Connection is not online (You should wait for initialization reply event)
	kP2PError_NotExists				= -8,		//	User is not exists
	kP2PError_SenderMismatch		= -9,		//	Sender of the data and owner of the connection are mismatch.
	kP2PError_ReceiverMismatch		= -10,		//	Receiver of the data and receiver of the server are mismatch.
};

//-----------------------------------------------------------------------------
//	NMP2PStatus
//-----------------------------------------------------------------------------

enum NMP2PStatus
{
	kP2PStatus_Unknown				= 0,

	kP2PStatus_Online				= 10,
	kP2PStatus_Offline				= 11,
};

//-----------------------------------------------------------------------------
//	CNMP2PKey
//-----------------------------------------------------------------------------

struct NMP2PKey
{
	INT32 nGameCode;
	INT32 nNexonSN;

	NMP2PKey( INT32 nNexonSN = 0, INT32 nGameCode = 0 )
		: nGameCode( nGameCode )
		, nNexonSN( nNexonSN )
	{
	}

	NMP2PKey( INT64 nP2PKey )
		: nGameCode( static_cast< INT32 >( nP2PKey ) )
		, nNexonSN( static_cast< INT32 >( nP2PKey >> 32 ) )
	{
	}

	operator INT64() const
	{
		return ( ( static_cast< INT64 >( nNexonSN ) << 32 ) | nGameCode );
	}

	bool operator == ( NMP2PKey const & rhs ) const
	{
		return ( this->nNexonSN == rhs.nNexonSN && this->nGameCode == rhs.nGameCode );
	}
};

typedef NMP2PKey*				PNMP2PKey;
typedef const NMP2PKey*			PCNMP2PKey;
typedef std::vector< NMP2PKey >	NMP2PKeyList;

#define	ENCODE_P2PKEY( x )	{ encoder.Encode4( x.nNexonSN ); encoder.Encode4( x.nGameCode ); }
#define	DECODE_P2PKEY( x )	{ x.nNexonSN = decoder.Decode4(); x.nGameCode = decoder.Decode4(); }
#define	IS_P2PKEY_NULL( x )	( x.nNexonSN == 0 && x.nGameCode == 0 )

//-----------------------------------------------------------------------------
//	CNMP2PMessage
//-----------------------------------------------------------------------------

class CNMP2PMessage : public CNMSerializable
{
public:
	NMP2PKey						sender;
	NMP2PKey						receiver;
	UINT32							cbyDataSize;
	BYTE							data[ P2PDATA_SIZE ];

public:
	CNMP2PMessage();
	CNMP2PMessage( CNMP2PMessage const & rhs );
	CNMP2PMessage( NMP2PKey const & sender, NMP2PKey const & receiver, BYTE const * pbyData, UINT32 cbyDataSize );

	DECLARE_SERIALIZABLE

public:
	CNMP2PMessage & operator = ( CNMP2PMessage const & rhs );
};

#endif	//	#ifndef __NMDATAOBJECT_H_51C2349E_4DF4_4898_AEBD_E86B4E28CA3F__
