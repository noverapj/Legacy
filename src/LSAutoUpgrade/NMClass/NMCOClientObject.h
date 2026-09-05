#ifndef __NMCOCLIENTOBJECT_H_ED4FBADF_AB83_4be8_BCD6_4D7D9E06D9B5__
#define __NMCOCLIENTOBJECT_H_ED4FBADF_AB83_4be8_BCD6_4D7D9E06D9B5__
#ifndef _NMCOCLIENTOBJECT
#define _NMCOCLIENTOBJECT
#endif

#include "NMGeneral.h"

class CNMCOClientObject 
	: public CSingleton< CNMCOClientObject >
{
	friend class CNMEventHandler;
	friend class CSingleton< CNMCOClientObject >;

public:
	BOOL		SetLocale
	(
				IN	NMLOCALEID			uLocaleID
	);

	BOOL		SetLocaleAndRegion
	(
				IN	NMLOCALEID			uLocaleID
	,			IN	NMREGIONCODE		uRegionCode
	);

	BOOL		SetCodePage
	(
				IN	UINT32				uCodePage
	);

	UINT32		GetCodePage
	(
	);

	BOOL		SetConnConfig
	(
				IN	CNMConnConfig const & ConnConfig
	);

	BOOL		SetCustomServerInfo
	(
				IN	ServerInfoList const & aServerInfo
	);

	BOOL		SetPatchOption
	(
				IN	BOOL				bPatch
	);

	BOOL		SetUseFriendModuleOption
	(
				IN	BOOL				bUseFriendModule
	);

	BOOL		Initialize
	(
				IN	NMGameCode			uGameCode
	);

	BOOL		Finalize
	(
	);

	BOOL		RegisterCallbackMessage
	(	
				IN	HWND				hWnd
	,			IN	UINT32				uMessage
	);

	BOOL		ResetCallbackMessage
	(
				IN	HWND				hWnd				= NULL
	);

	//////////////////////////////////////////////////////////////////////////
	//
	//	Authentication Functions
	//
	//////////////////////////////////////////////////////////////////////////
	
	NMLoginAuthReplyCode		
				AttachAuth
	(
				IN	LPCTSTR				pszPassport,								//	NXPASSPORT_SIZE
				IN	INT32				nRegionCode			= 0
	);

	BOOL		DetachAuth
	(
				void
	);

	NMLoginAuthReplyCode
				LoginAuth
	(
				IN	LPCTSTR				pszNexonID,								//	LOGINID_SIZE
				IN	LPCTSTR				pszPassword,							//	PASSWORD_SIZE
				IN	INT32				nRegionCode			= 0,
				IN	LPCTSTR				szExtraData			= NULL,				//	EXTRADATA_SIZE
				IN	UINT32				uChannelCode		= 0,
				OUT	INT32*				pnSubErrCode		= NULL
	);

	BOOL		LogoutAuth
	(
				void
	);

	LPCTSTR		GetNexonPassport
	(
				IN	LPTSTR				szPassportOut							//	NXPASSPORT_SIZE
	);

	NMLoginAuthReplyCode
				LoginAuthSecondary
	(
				IN	LPCTSTR				pszNexonID,								//	LOGINID_SIZE
				IN	LPCTSTR				pszPassword								//	PASSWORD_SIZE
	);

	BOOL		LogoutAuthSecondary
	(
				void
	);

	LPCTSTR		GetNexonPassportSecondary
	(
				IN	LPTSTR				szPassportOut		= NULL				//	NXPASSPORT_SIZE
	);

	NMLoginAuthReplyCode
				SecurityAuth
	(
				IN	LPCTSTR				pszPassport,							//	NXPASSPORT_SIZE
				IN	LPCTSTR				pszSecurityData							//	SECURITYCN_SIZE	
	);

	SecurityCode
				GetSecurityCodeCN
	(
				void
	);

	LPCTSTR		GetSecurityQuestion
	(
				IN	LPTSTR				szSecurityQuestion	= NULL				//	SECURITYCN_SIZE
	);

	LPCTSTR		GetNxHWID
	(
				IN	LPTSTR				szNxHWID								//	NXHWID_SIZE
	);

	NMGameTokenReplyCode
				GetGameToken
	(
				IN	LPCTSTR				pszSecureData		//	SECUREDATA_SIZE
	,			OUT	INT32 &				nSecureCode
	);

	//////////////////////////////////////////////////////////////////////////
	//
	//	Messenger functions
	//
	//////////////////////////////////////////////////////////////////////////
	
	BOOL		LoginMessenger
	(
				IN	UINT32				uSupportFlag		= kSupportFlag_FromGameCode
	);

	BOOL		LoginMessengerWithPassport
	(
				IN	LPCTSTR				pszNexonID,			//	LOGINID_SIZE
				IN	LPCTSTR				pszPassword,		//	PASSWORD_SIZE
				IN	PBYTE				pbyPassportData,	//	PASSPORT_SIZE
				IN	INT32				cbyPassportLength,
				IN	UINT32				uSupportFlag		= kSupportFlag_FromGameCode,
				IN	INT32				nRegionCode			= 0
	);

	BOOL		LogoutMessenger
	(
				void
	);

	//////////////////////////////////////////////////////////////////////////
	//
	//	Virtual User Session Control Functions
	//
	//////////////////////////////////////////////////////////////////////////
	
	//
	//	NOTE: LoginVirtual() and LogoutVirtual() must be called once in one game session
	// 

	BOOL		LoginVirtual
	(
				IN	LPCTSTR				szUserId						//	NICKNAME_SIZE
	,			IN	UINT32				uVirtualIdCode		= 1			//	support for multiple character
	,			IN	LPCTSTR				szPosition			= NULL		//	POSITION_SIZE
	);

	BOOL		LogoutVirtual( void );

	BOOL		GetMyVirtualUserList
	(	
				OUT NMVirtualUserList&	aVirtualUser
	,			IN	NMGameCode			uGameCode			= kGameCode_NULL 
	);
	
	BOOL		ChangeMyPosition
	( 
				IN	LPCTSTR				szPosition			//	POSITION_SIZE
	);

	BOOL		SendRefreshMessage
	( 
				IN	NMREFRESHTYPE		uRefreshType
	);

	BOOL		ChangeMyLevel
	(
				IN UINT32				uLevel,
				IN UINT32				uLevelType = 0
	);

	BOOL		RemoveMyVirtualUser( NMVirtualKey & keyVirtual );

	UINT64		GetDllVersion( void );
	UINT64		GetNMCOSrvVersion( void );

	/* Get My Info Functions*/
	NMIDCode		GetMyIdCode( void );
	UINT32			GetMyOid( void );
	LPCTSTR			GetMyLoginId( void );					//	LOGINID_SIZE
	LPCTSTR			GetMyWord( void );						//	TODAYWORD_SIZE
	NMUSERSTATUS	GetMyStatus( void );
	UINT32			GetMyLocalIP( void );
	UINT32			GetMyVirtualIP( void );
	LPCTSTR			GetMyName( void );						//	NXNAME_SIZE
	LPCTSTR			GetMyNickName( void );					//	NICKNAME_SIZE
	UINT32			GetMyMaskInfo( void );
	NMSEXTYPE		GetMySex( void );
	UINT32			GetMyAge( void );
	UINT32			GetMyAreaCode( void );
	LPCTSTR			GetMyAreaName( void );					//	NXAREA_SIZE
	UINT32			GetMySchoolCode( void );
	LPCTSTR			GetMySchoolName( void );				//	NXSCHOOL_SIZE
	SYSTEMTIME		GetMyBirthday( void );
	CNMRealUserInfo	GetMyInfo( void );

	UINT32			GetUnreadNoteCount( void );

	//	행사 등 이벤트
	BOOL			RequestNews();
	BOOL			CheckNews( NMGameCode uEventGameCode, NMVirtualKey keyVirtual = keyVirtualAll );

	/* Change My Info Functions */
	BOOL		ChangeMyStatus
	( 
				IN	NMUSERSTATUS		uStatus
	);

	BOOL		ChangeMyWord
	( 
				IN	LPCTSTR				szWord				//	TODAYWORD_SIZE
	);

	BOOL		ChangeMyNickName
	( 
				IN	LPCTSTR				szNickName			//	NICKNAME_SIZE
	);

	BOOL		ChangeMyMaskInfo
	( 
				IN	UINT32				uMaskInfo
	);

	BOOL		ChangeNickname
	(
				IN NMVirtualKey			keyCharacter
	,			IN LPCTSTR				szNickname			//	NICKNAME_SIZE
	);

	BOOL		ChangeMetaData
	(
				IN NMVirtualKey			keyCharacter
	,			IN LPCTSTR				szMetaData			//	METADATA_SIZE
	);

	BOOL		ChangeMyInfo
	(	
				IN	NMUSERSTATUS		uStatus			= kStatus_Unknown
	,			IN	LPCTSTR				szWord			= NULL					//	TODAYWORD_SIZE
	,			IN	LPCTSTR				szNickName		= NULL					//	NICKNAME_SIZE
	,			IN	UINT32				uMaskInfo		= 0xFFFFFFFF
	);

	/* Get User Config */
	NMALLOWTYPE		GetAllowChatType( void );
	NMALLOWTYPE		GetAllowNoteType( void );
	NMALLOWTYPE		GetAllowGameInviteType( void );
	NMALLOWTYPE		GetAllowRequestFriendType( void );

	/* Set User Config */
	BOOL		ChangeAllowChatType
	( 
				IN	NMALLOWTYPE			uType
	);

	BOOL		ChangeAllowNoteType
	( 
				IN	NMALLOWTYPE			uType
	);

	BOOL		ChangeAllowGameInviteType
	( 
				IN	NMALLOWTYPE			uType
	);
	
	BOOL		ChangeAllowRequestFriendType
	( 
				IN	NMALLOWTYPE			uType
	);

	/* User Local Config */
	BOOL		GetLocalConfig
	(
				IN	NMLOCALCONFIGTYPE	uConfigType
	,			OUT	LPTSTR				szValueOut			//	1024
	);

	BOOL		ChangeLocalConfig
	(
				IN	NMLOCALCONFIGTYPE	uConfigType
	,			IN	LPCTSTR				szValue				//	1024
	);

	/* Friend Request Related Functions */
	BOOL		RequestNewFriend
	(	
				IN	LPCTSTR				szRequestId				//	LOGINID_SIZE
	,			IN	LPCTSTR				szMessage				//	MSG_SIZE
	,			IN	BOOL				bAddToNxFriend			= FALSE
	,			IN	NMGameCode			uGameCode				= kGameCode_NULL
	,			IN	NMGameCode			uRequesteeGameCode		= kGameCode_NULL
	,			IN	UINT32				uVirtualIdCode			= 0
	,			IN	UINT32				uRequesteeVirtualIdCode	= 0
	);

	BOOL		ConfirmNewFriend
	(	
				IN	UINT32				uSerialNo
	,			IN	NMCONFIRMCODE		uConfirmCode
	);

	/* Category Related Functions */
	BOOL		AddCategory
	(	
				IN	NMVirtualKey		keyOwner
	,			IN	LPCTSTR				szCategoryName		//	CATEGORY_SIZE
	,			IN	NMCATEGORYPROPERTY	uProperty			= kCateProp_None
	,			IN	NMALLOWTYPE			uAllowType			= kConfigAllowValue_FromAll
	);

	BOOL		DeleteCategory
	( 
				IN	UINT32				uCategoryCode
	,			IN	BOOL				bSystemCall			= FALSE
	);

	BOOL		ChangeCategoryName
	(	
				IN	UINT32				uCategoryCode
	,			IN	LPCTSTR				szCategoryName		//	CATEGORY_SIZE
	,			IN	BOOL				bSystemCall			= FALSE
	);

	BOOL		ChangeCategoryProperty
	(
				IN	UINT32				uCategoryCode
	,			IN	NMCATEGORYPROPERTY	uProperty
	,			IN	BOOL				bSystemCall			= FALSE
	);

	BOOL		ChangeCategoryAllowType
	(
				IN	UINT32				uCategoryCode
	,			IN	NMALLOWTYPE			uAllowType
	,			IN	BOOL				bSystemCall			= FALSE
	);

	/* Friends List Functions */
	BOOL		GetFriendList
	(
				OUT	NMCategoryUserList&		
										aCateFriend
	,			OUT	NMFriendList&		aUnrelated
	,			IN	BOOL				bAll				= FALSE
	);

	BOOL		GetFriendListEx
	(
				OUT	NMCategoryUserList&		
										aCateFriend
	,			OUT	NMFriendList&		aUnrelated
	,			IN	NMVirtualKey		keyVirtual			= keyVirtualAll
	);

	BOOL		GetFriendInfo
	(
				IN	NMFriendKey			keyFriend
	,			OUT	CNMFriendInfo&		fiFriendInfo
	);

	BOOL		BlockFriend
	(	
				IN	NMFriendKey			keyFriend
	,			IN	BOOL				bBlock
	);

	BOOL		AddFriendToCategory
	(
				IN	NMFriendKey			keyFriend
	,			IN	UINT32				uCategoryCode
	,			IN	BOOL				bSystemCall			= FALSE
	);

	BOOL		DeleteFriendFromCategory
	(			
				IN	NMFriendKey			keyFriend
	,			IN	UINT32				uCategoryCode
	,			IN	BOOL				bSystemCall			= FALSE
	);

	BOOL		MoveFriendCategory
	(	
				IN	NMFriendKey			keyFriend
	,			IN	UINT32				uFromCategoryCode
	,			IN	UINT32				uToCategoryCode
	,			IN	BOOL				bSystemCall			= FALSE
	);

	BOOL		ChangeFriendMemo
	(
				IN	NMFriendKey			keyFriend
	,			IN	LPCTSTR				szMemo				//	MEMO_SIZE
	);

	//
	//	rejected user list
	//

	BOOL		GetRejectedUserList
	(
				OUT	NMRejectedUserList &	aRejectedUserList
	);

	BOOL		AppendRejectedUser
	(
				IN	CNMRejectedUser &		rejectedUser
	);

	BOOL		RemoveRejectedUser
	(
				IN	CNMRejectedUser	&		rejectedUser
	);

	/* Note Functions */
	BOOL		SendNoteMsg
	(	
				IN	CNMNoteInfo::NMNOTETYPE		uNoteType
	,			IN	UINT32				uSubType
	,			IN	BOOL				bSaveToPermBox
	,			IN	LPCTSTR				szTargetId								//	TARGETID_SIZE
	,			IN	LPCTSTR				szMessage								//	NOTEMSG_SIZE
	,			IN	LPCTSTR				szArgument		= NULL					//	NOTEARG_SIZE
	,			IN	LPCTSTR				szCustomData	= NULL					//	NOTECUSTOM_SIZE
	,			IN	NMGameCode			uGameCode		= kGameCode_NULL
	,			IN	UINT32				uVirtualIdCode	= 1
	);

	BOOL		SendNoteExMsg
	(	
				IN	CNMNoteInfoEx::NMNOTEEXTYPE	
										uNoteExType
	,			IN	LPCTSTR				szMessage								//	NOTEEXMSG_SIZE
	);

	BOOL		SendReportMsg
	(	
				IN	CNMReportInfo::NMREPORTTYPE		
										uType
	,			IN	CNMReportInfo::NMREPORTSUBTYPE	
										uSubType
	,			IN	LPCTSTR				szTargetNickname						//	NICKNAME_SIZE
	,			IN	LPCTSTR				szReason								//	4000
	,			IN	LPCTSTR				szEvidence								//	20000
	,			IN	NMGameCode			uGameCode			= kGameCode_NULL
	,			IN	NMGameCode			uAppliedGameCode	= kGameCode_NULL
	,			IN	LPCTSTR				szReporterNickname	= NULL				//	NICKNAME_SIZE
	);

	BOOL		SendAccuseNote
	(	
				IN	CNMReportInfo::NMREPORTSUBTYPE	
										uSubType
	,			IN	LPCTSTR				szReason								//	4000
	,			IN	UINT32				uNoteSerial		= 0
	);

	BOOL		LogReportSync
	(	
				IN	UINT16				uLogType
	);

	NMNoteList	GetNoteBox
	(
				IN	NMNOTEBOXTYPE		uBoxType
	,			IN	UINT32				uNoteSerial
	);

	BOOL		GetNoteBox
	(
				IN	NMNOTEBOXTYPE		uBoxType
	,			IN	UINT32				uNoteSerial
	,			OUT NMNoteInfoList &	noteInfoList
	);

	BOOL		SetNoteReadFlag
	(
				IN	UINT32			uNoteSerial
	);

	BOOL		SaveNoteToPermBox
	(
				IN	UINT32			uNoteSerial
	);

	BOOL		DeleteNote
	(
				IN	UINT32			uNoteSerial
	);

	CNMGuildListExInfo
				GetMyGuildList
	(
				void
	);

	BOOL		MonitorGuildOnlineInfoEx
	(		
				IN	NMGuildKey				uGuildKey,
				IN	BOOL					bAttach
	);

	BOOL		MonitorGuildOnlineInfoEx
	(
				IN	NMGuildID const &		guildID,
				IN	UINT32					uVirtualIDCode,
				IN	BOOL					bAttach
	);

	BOOL		GetGuildOnlineInfoEx
	(
				IN	NMGuildKey				uGuildKey,
				OUT	CNMGuildOnlineInfoEx &	goi
	);

	BOOL		GetGuildOnlineInfoEx
	(
				IN	NMGuildID const &		guildID,
				OUT	CNMGuildOnlineInfoEx &	goi
	);

	/* Client Util */
	BOOL		GetGameList
	(
				OUT	CNMGameListData*	pglGameList
	);

	BOOL		GetUserInfo
	(
				IN	LPCTSTR				szTargetID						//	LOGINID_SIZE
	,			IN	NMGameCode			uGameCode	= kGameCode_NULL
	);

	BOOL		SendCustomNotify
	(
				IN UINT32				uType
	,			IN LPCTSTR				szArgument						//	MSG_SIZE
	,			IN UINT32				uPolicyFlag = CNMCustomNotifyData::kCustomNotifyPolicy_Default
	);

	BOOL		SendCustomNotify
	(
				IN CNMCustomNotifyData&	cnData
	);

	/* Session Request Functions */
	BOOL		RequestChatSession
	(
				IN	NMFriendCode		fcFriendCode
	);

	BOOL		RequestMultiChatSession
	( 
				IN	NMFriendCodeList&	aFriendCodeList
	,			IN	NMVirtualKey		keyVirtuality	= keyVirtualAll
	);

	BOOL		RequestFileUploadSession
	( 
				IN	NMFriendCode		fcFriendCode
	,			IN	NMFilePathList&		aFilePath
	);

	BOOL		RequestChatSession
	(
				IN	NMFriendKey			keyFriend
	,			OUT	_UInt32_			*pSessionKey		= NULL
	);

	BOOL		RequestMultiChatSession
	( 
				IN	NMFriendKeyList&	aFriendKey
	,			IN	NMVirtualKey		keyVirtuality	= keyVirtualAll
	);

	BOOL		RequestFileUploadSession
	( 
				IN	NMFriendKey			keyFriend
	,			IN	NMFilePathList&		aFilePath
	);

	BOOL		RequestChatSessionToGuildMember
	(
				IN	LPCTSTR				szRequestID
	,			IN	NMVirtualKey		keyVirtuality	= keyVirtualAll
	);

	BOOL		RequestFileUploadSessionToGuildMember
	(
				IN	LPCTSTR				szRequestID
	,			IN	NMFilePathList&		aFilePath
	,			IN	NMVirtualKey		keyVirtuality	= keyVirtualAll
	);

	///////////////////////////////////////////////////////////////////////////
	//
	//	Chat Room Request Functions
	//
	///////////////////////////////////////////////////////////////////////////

	BOOL		CreateChatRoomEx
	(	
				IN	UINT32				uRoomType
	,			IN	NMGuildKey			uGuildKey
	,			IN	LPCTSTR				szGuildTitle				//	GUILDNAME_SIZE
	,			IN	LPCTSTR				szRoomTitle					//	ROOMTITLE_SIZE
	,			IN	UINT32				uLimitLevel
	,			IN	UINT32				uMaxSize
	,			IN	UINT32				uFlag
	,			IN	LPCTSTR				szPassword					//	ROOMPWD_SIZE
	,			IN	LPCTSTR				szServerAddr				//	32
	,			IN	UINT16				uServerPort
	,			IN	UINT32				uVirtualIdCode = 1
	);

	BOOL		JoinToChatRoomEx
	(	
				IN	NMGuildKey			uGuildKey
	,			IN	UINT32				uRoomSerialNo
	,			IN	LPCTSTR				szPassword					//	ROOMPWD_SIZE
	,			IN	LPCTSTR				szServerAddr				//	32
	,			IN	UINT16				uServerPort
	,			IN	UINT32				uFlag			= 0
	,			IN	UINT32				uVirtualIdCode	= 1
	);

	///////////////////////////////////////////////////////////////////////////
	//
	//	Game Character Functions
	//
	///////////////////////////////////////////////////////////////////////////

	INT32		CharacterLogin
	(
				IN	LPCTSTR				szCharacterName
	,			IN	UINT32				uCharacterSN		= 1
	,			IN	INT32				nServerCode			= 1
	);

	INT32		CharacterLogout
	(	
				IN	LPCTSTR				szCharacterName
	,			IN	UINT32				uCharacterSN		= 1
	,			IN	INT32				nServerCode			= 1
	);

	INT32		CharacterRemove
	(
				IN	LPCTSTR				szCharacterName
	,			IN	UINT32				uCharacterSN		= 1
	,			IN	INT32				nServerCode			= 1
	);

	INT32		CharacterChangeName
	(	
				IN	LPCTSTR				szCharacterName
	,			IN	UINT32				uCharacterSN		= 1
	,			IN	INT32				nServerCode			= 1
	);
	
	// OBSOLETE : 캐릭터 미러링 호출 인터페이스 통일화로 인해 NMVirtualKey 사용하는 인터페이스는 더 이상 사용하지 않도록 함. //
	INT32		CharacterRemove
	( 
				IN	NMVirtualKey &		keyVirtual
	,			IN	LPCTSTR				szCharacterName
	,			IN	INT32				nServerCode			= 1
	);

	// OBSOLETE : 캐릭터 미러링 호출 인터페이스 통일화로 인해 NMVirtualKey 사용하는 인터페이스는 더 이상 사용하지 않도록 함. //
	INT32		CharacterChangeName
	(
				IN	NMVirtualKey &		keyVirtual
	,			IN	LPCTSTR				szCharacterName
	,			IN	INT32				nServerCode			= 1
	);

	INT32		CharacterSync
	(
				IN	LPCTSTR				szCharacterName
	,			IN	UINT32				uCharacterSN		= 1
	,			IN	INT32				nServerCode			= 1
	);

	INT32		CharacterLogin64
	(
				IN	LPCTSTR				szCharacterName
	,			IN	UINT64				uCharacterSN		= 1
	,			IN	INT32				nServerCode			= 1
	);

	INT32		CharacterLogout64
	(	
				IN	LPCTSTR				szCharacterName
	,			IN	UINT64				uCharacterSN		= 1
	,			IN	INT32				nServerCode			= 1
	);
	
	INT32		CharacterRemove64
	( 
				IN	LPCTSTR				szCharacterName
	,			IN	UINT64				uCharacterSN		= 1
	,			IN	INT32				nServerCode			= 1
	);

	INT32		CharacterChangeName64
	(
				IN	LPCTSTR				szCharacterName
	,			IN	UINT64				uCharacterSN		= 1
	,			IN	INT32				nServerCode			= 1
	);

	INT32		CharacterSync64
	(
				IN	LPCTSTR				szCharacterName
	,			IN	UINT64				uCharacterSN		= 1
	,			IN	INT32				nServerCode			= 1
	);

	///////////////////////////////////////////////////////////////////////////
	//
	//	Restricted Words Functions
	//
	///////////////////////////////////////////////////////////////////////////
	
	BOOL		HasRestrictedWords
	(
				IN	LPCTSTR				szText						//	1024 (max)
	);

	BOOL		ReplaceRestrictedWords
	(
				OUT	LPTSTR				szText,						//	1024 (max)
				IN	UINT32				uTextSize
	);

	///////////////////////////////////////////////////////////////////////////
	//
	//	P2P Functions
	//
	///////////////////////////////////////////////////////////////////////////

	NMP2PErrorCode	P2PLogin
	(
				IN	LPCTSTR				szNexonPassport				//	NXPASSPORT_SIZE
	);

	NMP2PErrorCode	P2PLogout
	(
	);

	NMP2PErrorCode	P2PSendData
	(
		IN	UINT32				uSerialKey,					//	serial number of data
		IN	NMP2PKey const &	sender,						//	sender
		IN	NMP2PKey const &	receiver,					//	receiver
		IN	BYTE const *		pbyData,					//	P2PDATA_SIZE (max)
		IN	UINT32				cbyDataSize,
		IN	UINT32				uTTL = 0
	);

	NMP2PErrorCode	P2PSendData
	(
		IN	NMP2PKey const &	sender,						//	sender
		IN	NMP2PKey const &	receiver,					//	receiver
		IN	BYTE const *		pbyData,					//	P2PDATA_SIZE (max)
		IN	UINT32				cbyDataSize,
		OUT	UINT32 &			uSerialKey,					//	serial number of data
		IN	UINT32				uTTL = 0					//	Time to live. The data will be dropped after uTTL ms elapsed.
															//	If uTTL is zero, the data won't be dropped and will have priority.
	);

	NMP2PErrorCode	P2PSendData
	(
		IN	NMP2PKey const &	sender,
		IN	NMP2PKey const *	receivers,					//	P2PRECEIVERS_SIZE (max)
		IN	UINT32				receiversSize,
		IN	BYTE const *		pbyData,					//	P2PDATA_SIZE (max)
		IN	UINT32				cbyDataSize,
		OUT	UINT32 &			uSerialKey,					//	serial number of data
		IN	UINT32				uTTL = 0					//	Time to live. The data will be dropped after uTTL ms elapsed.
															//	If uTTL is zero, the data won't be dropped and will have priority.
	);

	///////////////////////////////////////////////////////////////////////////////
	//
	//	GameSummary functions
	//
	///////////////////////////////////////////////////////////////////////////////
	
	BOOL	OpenGameSummary
	(
		void
	);

	///////////////////////////////////////////////////////////////////////////
	//
	//	P2Linker Functions
	//
	///////////////////////////////////////////////////////////////////////////

	BOOL	P2LinkerGetHomeURL
	(
		OUT	LPTSTR			szUrlOut		// 1024
	);

	BOOL	P2LinkerGetProfileURL
	(
		IN	UINT32			uServerCode,
		IN	LPCTSTR			szCharacterName,
		OUT	LPTSTR			szUrlOut		// 1024
	);

	NMP2ChatError	P2LinkerWriteChat
	(
		IN	NMP2ChatType	chatType,
		IN	UINT32			uServerCode,
		IN	INT64			nCharacterSN,
		IN	LPCTSTR			szChat,
		IN	LPCTSTR			szParam
	);

protected:
	typedef std::vector< CNMGuildOnlineInfoEx >	NMGOIList;

protected:
	CRITICAL_SECTION	m_cs;

	CNMRealUserInfo		m_ruiRealUserInfo;

	NMGOIList			m_aGOIs;

	CNMCOClientObject( void );
public:
	~CNMCOClientObject( void );

protected:
	void		ResetMyInfo( void );
	void		ResetGuildOnlineInfo( void );
};

#endif	//	#ifndef __NMCOCLIENTOBJECT_H_ED4FBADF_AB83_4be8_BCD6_4D7D9E06D9B5__