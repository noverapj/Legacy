#ifndef __NMFUNCTIONOBJECT_H_2D622A4E_137B_4150_95A1_A97D3E9CC46C__
#define __NMFUNCTIONOBJECT_H_2D622A4E_137B_4150_95A1_A97D3E9CC46C__

/////////////////////////////////////////////////////////////////
// DO NOT CHANGE AN ORDER OF THE CODE BELOW
/////////////////////////////////////////////////////////////////

enum	NMFUNCCODE
{
	// control
	kNMFuncCode_ControlStart_				= 0x1000,
	kNMFuncCode_Init,						// REMOVED
	kNMFuncCode_RegisterCallback,
	kNMFuncCode_ResetCallback,
	kNMFuncCode_AttachToNMCOServer,
	kNMFuncCode_DetachFromNMCOServer,

	kNMFuncCode_BringForwardStandAloneMsg,
	kNMFuncCode_StartStandAloneMsg,

	kNMFuncCode_InitClientObject,

	// session control
	kNMFuncCode_SessionControlStart_		= 0x1100,
	kNMFuncCode_Login,						//	REMOVED
	kNMFuncCode_Logout,						//	REMOVED
	kNMFuncCode_LoginVirtual,
	kNMFuncCode_LogoutVirtual,
	kNMFuncCode_GetMyVirtualUserList,
	kNMFuncCode_ChangeMyPosition,
	kNMFuncCode_SendRefreshMessage,
	kNMFuncCode_ChangeMyLevel,
	kNMFuncCode_RemoveMyVirtualUser,
	kNMFuncCode_LoginPassport,				//	NOT IN KOREA
	kNMFuncCode_RetryLogin,					//	REMOVED
	kNMFuncCode_LoginNexonPassport,			//	INTERNAL (between ClientDLL & ClientCore )
	kNMFuncCode_LoginMessenger,
	kNMFuncCode_LogoutMessenger,
	kNMFuncCode_LoginAuth,
	kNMFuncCode_LogoutAuth,
	kNMFuncCode_Initialize,
	kNMFuncCode_SecurityAuth,
	kNMFuncCode_GetGameToken,
	kNMFuncCode_Finalize,

	// user info
	kNMFuncCode_UserInfoStart_				= 0x2000,
	kNMFuncCode_GetMyInfo,
	kNMFuncCode_GetUserDataList,
	kNMFuncCode_ChangeMyInfo,
	kNMFuncCode_GetConfig,
	kNMFuncCode_ChangeConfig,
	kNMFuncCode_GetLocalConfig,
	kNMFuncCode_ChangeLocalConfig,
	kNMFuncCode_GetCount,
	kNMFuncCode_SetCount,
	kNMFuncCode_ChangeNickname,
	kNMFuncCode_ChangeMetaData,

	// 행사 등 각종 이벤트나 뉴스
	kNMFuncCode_RequestNews,
	kNMFuncCode_CheckNews,

	// client info
	kNMFuncCode_ClientInfoStart_			= 0x2100,
	kNMFuncCode_GetDomain,
	kNMFuncCode_GetVersion,
	kNMFuncCode_SetStatInfo,
	kNMFuncCode_SetLocale,
	kNMFuncCode_GetNexonPassport,
	kNMFuncCode_GetSecurityQuestion,
	kNMFuncCode_GetNxHWID,
	kNMFuncCode_GetSecurityCodeCN,
	kNMFuncCode_SetCodePage,

	// P2P
	kNMFuncCode_P2PStart_					= 0x3000,
	kNMFuncCode_P2PLogin,
	kNMFuncCode_P2PSendData,
	kNMFuncCode_P2PLogout,
	kNMFuncCode_P2PMultiSendData,

	// friend
	kNMFuncCode_FriendStart_				= 0x4000,
	kNMFuncCode_GetFriendList,
	kNMFuncCode_GetFriendInfo,
	kNMFuncCode_RequestNewFriend,
	kNMFuncCode_ConfirmNewFriend,
	kNMFuncCode_BlockFriend,
	kNMFuncCode_ChangeFriendMemo,
	kNMFuncCode_AddFriendToCategory,
	kNMFuncCode_DeleteFriendFromCategory,
	kNMFuncCode_MoveFriendCategory,
	kNMFuncCode_ChangeFriendMemoEx,

	// category
	kNMFuncCode_CategoryStart_				= 0x4100,
	kNMFuncCode_AddCategory,
	kNMFuncCode_DeleteCategory,
	kNMFuncCode_ChangeCategoryName,
	kNMFuncCode_ChangeCategoryProperty,
	kNMFuncCode_ChangeCategoryAllowType,

	// black-list (obsolete)
	kNMFuncCode_BlackListStart_				= 0x5000,
	kNMFuncCode_GetBlackList,
	kNMFuncCode_AddToBlackList,
	kNMFuncCode_DeleteFromBlackList,

	// reject-list (obsolete)
	kNMFuncCode_RejectListStart_			= 0x5100,
	kNMFuncCode_GetRejectList,
	kNMFuncCode_AddToRejectList,
	kNMFuncCode_DeleteFromRejectList,

	// new rejected user list
	kNMFuncCode_RejectedUserListStart_		= 0x5200,
	kNMFuncCode_GetRejectedUserList,
	kNMFuncCode_AppendRejectedUser,
	kNMFuncCode_RemoveRejectedUser,

	// note
	kNMFuncCode_NoteStart_					= 0x6000,
	kNMFuncCode_GetNoteBox,
	kNMFuncCode_SendNoteMsg,
	kNMFuncCode_ProcessNote,
	kNMFuncCode_SendNoteInfo,
	kNMFuncCode_GetNoteBox2,
	kNMFuncCode_SendAccuseNoteMsg,

	// guild
	kNMFuncCode_GuildStart_					= 0x7000,
	kNMFuncCode_GetMyGuildList,							// REMOVED //
	kNMFuncCode_MonitorGuildOnlineInfo,					// REMOVED //
	kNMFuncCode_GetMyGuildListEx,
	kNMFuncCode_MonitorGuildOnlineInfoEx,
	kNMFuncCode_SendGuildChatMessage,
	kNMFuncCode_GetGuildList,							// OBSOLETE //

	// client util
	kNMFuncCode_ClientUtilStart_			= 0x8000,
	kNMFuncCode_ExecutePatcher,
	kNMFuncCode_ExecuteLauncher,
	kNMFuncCode_ExecuteCommon,
	kNMFuncCode_GetUrl,									// REMOVED //
	kNMFuncCode_GetPath,
	kNMFuncCode_SetSessionValue,
	kNMFuncCode_GetSessionValue,
	kNMFuncCode_GetGameList,
	kNMFuncCode_GetUrlEx,
	kNMFuncCode_DownloadFile,
	kNMFuncCode_UploadFile,
	kNMFuncCode_GetSupportGameList,
	kNMFuncCode_GetGameServerList,
	kNMFuncCode_GetGameFullName,
	kNMFuncCode_GetGameShortName,
	kNMFuncCode_GetGameFriendTitle,
	kNMFuncCode_GetGameServerName,

	kNMFuncCode_GetConnConfig,
	kNMFuncCode_SetConnConfig,
	kNMFuncCode_DownloadGameList,						//	REMOVED //

	kNMFuncCode_ExecuteNGMPatcher,
	kNMFuncCode_ExecuteNGMLauncher,
	kNMFuncCode_IsNGMInstalled,

	kNMFuncCode_RestrictedWords,
	kNMFuncCode_MinimizeAll,
	kNMFuncCode_LogReportSync,
	kNMFuncCode_IsGuestIDPassport,						// REMOVED
	kNMFuncCode_SetServerInfo,
	kNMFuncCode_DownloadGuildMarkFile,
	kNMFuncCode_GetServerInfo,
	kNMFuncCode_WriteToWiselog,
	kNMFuncCode_ExecuteNGMInstaller,
	kNMFuncCode_OpenGameSummary,

	kNMFuncCode_ClientUtilEnd_,

	// server util
	kNMFuncCode_ServerUtilStart_			= 0x9000,
	kNMFuncCode_GetUserInfo,
	kNMFuncCode_FindUser,
	kNMFuncCode_GetFindUserResult,
	kNMFuncCode_SendNoteExMsg,
	kNMFuncCode_SendReportMsg,
	kNMFuncCode_CustomNotify,

	// create session
	kNMFuncCode_CreateSessionStart_			= 0xA000,
	kNMFuncCode_RequestChatSession,
	kNMFuncCode_RequestMultiChatSession,
	kNMFuncCode_RequestFileUploadSession,
	kNMFuncCode_RequestWBSession,
	kNMFuncCode_ReplySessionRequest,
	kNMFuncCode_CreateChatRoom,							// REMOVED //
	kNMFuncCode_JoinToChatRoom,							// REMOVED //
	kNMFuncCode_RequestSessionToOther,
	kNMFuncCode_CreateChatRoomEx,
	kNMFuncCode_JoinToChatRoomEx,
	kNMFuncCode_RequestChatSessionEx,
	kNMFuncCode_RequestMultiChatSessionEx,
	kNMFuncCode_RequestFileUploadSessionEx,
	kNMFuncCode_RequestWBSessionEx,
	kNMFuncCode_JoinToDefaultGuildChatRoom,				// REMOVED //
	kNMFuncCode_CreateChannel,
	kNMFuncCode_JoinChannel,

	// chat room
	kNMFuncCode_ChatRoomStart_				= 0xB000,
	kNMFuncCode_CRRegisterCallback,
	kNMFuncCode_CRClose,
	kNMFuncCode_CRGetRoomInfo,							// REMOVED //
	kNMFuncCode_CRChangeRoomInfo,
	kNMFuncCode_CRChangeMyInfo,
	kNMFuncCode_CRGetMemberList,
	kNMFuncCode_CRInviteUser,
	kNMFuncCode_CRBanUser,
	kNMFuncCode_CRSendChatMessage,
	kNMFuncCode_CRGetMemberInfo,
	kNMFuncCode_CRGetRoomInfoEx,

	// general session
	kNMFuncCode_GenralSessionStart_			= 0xC000,
	kNMFuncCode_GSRegisterCallback,
	kNMFuncCode_GSWantClose,
	kNMFuncCode_GSGetSessionInfo,
	kNMFuncCode_GSSetServingProcessID,
	kNMFuncCode_GSGetMemberList,
	kNMFuncCode_GSInviteUser,
	kNMFuncCode_GSGetInviteCandidateList,
	kNMFuncCode_GSInviteUserEx,

	// chat session
	kNMFuncCode_ChatSessionStart_			= 0xC100,
	kNMFuncCode_CSSendChatMessage,

	// file upload session
	kNMFuncCode_FileUploadSessionStart_		= 0xC200,
	kNMFuncCode_FUDSGetFileEvent,
	kNMFuncCode_FUSSendFile,
	kNMFuncCode_FUSControl,

	// file download session
	kNMFuncCode_FileDownloadSessionStart_	= 0xC300,
	kNMFuncCode_FDSControl,
	kNMFuncCode_FDSGetDownloadDir,
	kNMFuncCode_FDSSetDownloadDir,

	// white-board session
	kNMFuncCode_WhiteboardSessionStart_		= 0xC400,
	kNMFuncCode_WSSendWBMessage,
	kNMFuncCode_WSGetAssocSerialKey,

	// channel
	kNMFuncCode_ChannelStart_				= 0xD000,
	kNMFuncCode_CHClose,
	kNMFuncCode_CHGetChannelInfo,
	kNMFuncCode_CHChangeChannelInfo,
	kNMFuncCode_CHGetMemberInfoList,
	kNMFuncCode_CHChangeMyInfo,
	kNMFuncCode_CHInviteUser,
	kNMFuncCode_CHBanUser,
	kNMFuncCode_CHSendChannelMessage,

	// for event
	kNMFuncCode_EventStart_					= 0xE000,
	kNMFuncCode_KartComeback,							//	REMOVED //

	// for character
	kNMFuncCode_CharacterLogin				= 0xF000,
	kNMFuncCode_CharacterLogout,
	kNMFuncCode_CharacterRemove,
	kNMFuncCode_CharacterChangeName,
	kNMFuncCode_CharacterLogin64,
	kNMFuncCode_CharacterLogout64,
	kNMFuncCode_CharacterRemove64,
	kNMFuncCode_CharacterChangeName64,
	kNMFuncCode_CharacterSync,
	kNMFuncCode_CharacterSync64,

	// for game log
	kNMFuncCode_GameLogInitialize			= 0xF010,
	kNMFuncCode_GameLogFinalize,
	kNMFuncCode_GameLogWriteStageLog,
	kNMFuncCode_GameLogWriteErrorLog,
	kNMFuncCode_GameLogGetSessionID,

	// for play feed
	kNMFuncCode_PlayFeedWritePlayLog		= 0xF0A0,
	kNMFuncCode_PlayFeedPublishPlayFeed,

	// for p2 linker
	kNMFuncCode_P2LinkerWriteChat			= 0xF100,
};

//=========================================
// base class
//=========================================

class CNMFunc : public CNMSerializable
{
private:
	_UInt32_	m_uCode;
	BOOL		m_bCalling;
	BOOL		m_bHandled;
	BOOL		m_bPassThrough;
	BOOL		m_bUseEncrypt;
	_UInt32_	m_uProcID;

public:
	_UInt32_	GetCode( void )						{ return this->m_uCode; }
	void		SetCalling( void )					{ this->m_bCalling = TRUE; }
	BOOL		IsCalling( void )					{ return this->m_bCalling; }
	void		SetReturn( void )					{ this->m_bCalling = FALSE; }
	BOOL		IsReturn( void )					{ return !this->m_bCalling; }
	void		SetHandled( BOOL bSet = TRUE )		{ this->m_bHandled = bSet; }
	BOOL		IsHandled( void )					{ return this->m_bHandled; }
	void		SetPassThrough( BOOL bSet = TRUE )	{ this->m_bPassThrough = bSet; }
	BOOL		IsPassThrough( void )				{ return this->m_bPassThrough; }
	BOOL		UseEncrypt( void )					{ return this->m_bUseEncrypt; }
	void		SetProcID( _UInt32_ uProcID )		{ this->m_uProcID = uProcID; }
	_UInt32_	GetProcID( void )					{ return this->m_uProcID; }
	void		SetSerializerVersion( _UInt8_ uSerializerVersion )
													{ this->m_uSerializerVersion = uSerializerVersion; }
	_UInt8_		GetSerializerVersion( void )		{ return this->m_uSerializerVersion; }
	virtual		LPCTSTR GetName( void )				{ return _T( "CNMFunc" ); }

public:
	// default calling data
	_UInt32_	uSerialKey;

	// default return data
	BOOL		bSuccess;

public:
	CNMFunc( _UInt32_ uCode, _UInt8_ uObjectVersion, _UInt32_ uSerialKey = 0, BOOL bUseEncrypt = FALSE );

	DECLARE_SERIALIZABLE
};

#define	DECLARE_NMFUNC( funcName )					\
	enum { kCode = kNMFuncCode_##funcName };		\
	virtual LPCTSTR GetName( void ) { return _T( "CNM" ) _T( #funcName ) _T( "Func" ); };

//=========================================
// classes
//=========================================

class CNMRegisterCallbackFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( RegisterCallback )

	// call data
	HWND				hWnd;
	_UInt32_			uMessage;

	// return data
	// NONE

public:
	CNMRegisterCallbackFunc( void );
	CNMRegisterCallbackFunc( HWND hWnd, _UInt32_ uMessage );

	DECLARE_SERIALIZABLE
};

class CNMResetCallbackFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( ResetCallback )

	// call data
	HWND				hWnd;

	// return data
	// NONE

public:
	CNMResetCallbackFunc( void );
	CNMResetCallbackFunc( HWND hWnd );

	DECLARE_SERIALIZABLE
};

class CNMAttachToNMCOServerFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( AttachToNMCOServer )

	// call data
	_UInt32_			uProcessId;
	_UInt32_			uGameCode;
	_UInt32_			uSupportFlag;
	NMLOCALEID			uLocaleID;
	INT32				nRegionCode;

	// return data
	// NONE

public:
	CNMAttachToNMCOServerFunc( void );
	CNMAttachToNMCOServerFunc( _UInt32_ uProcessId, _UInt32_ uGameCode, _UInt32_ uSupportFlag, NMLOCALEID uLocaleID, INT32 nRegionCode );

	DECLARE_SERIALIZABLE
};

class CNMDetachFromNMCOServerFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( DetachFromNMCOServer )

	// call data
	_UInt32_			uProcessId;

	// return data
	// NONE

public:
	CNMDetachFromNMCOServerFunc( void );
	CNMDetachFromNMCOServerFunc( _UInt32_ uProcessId );

	DECLARE_SERIALIZABLE
};

class CNMBringForwardStandAloneMsgFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( BringForwardStandAloneMsg )

	// call data
	TCHAR				szDomain[ DOMAIN_SIZE ];

	// return data
	// NONE

public:
	CNMBringForwardStandAloneMsgFunc( void );
	CNMBringForwardStandAloneMsgFunc( LPCTSTR szDomain );

	DECLARE_SERIALIZABLE
};

class CNMStartStandAloneMsgFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( StartStandAloneMsg )

	// call data
	TCHAR				szDomain[ DOMAIN_SIZE ];

	// return data
	// NONE

public:
	CNMStartStandAloneMsgFunc( void );
	CNMStartStandAloneMsgFunc( LPCTSTR szDomain );

	DECLARE_SERIALIZABLE
};

class CNMGetConnConfigFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( GetConnConfig )

	// call data
	// NONE

	// return data
	CNMConnConfig		m_ConnConfig;

public:
	CNMGetConnConfigFunc( void );

	DECLARE_SERIALIZABLE
};

class CNMSetConnConfigFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( SetConnConfig )

	// call data
	CNMConnConfig		m_ConnConfig;

	// return data
	// NONE

public:
	CNMSetConnConfigFunc( void );
	CNMSetConnConfigFunc( const CNMConnConfig & ConnConfig );

	DECLARE_SERIALIZABLE
};

class CNMLoginVirtualFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( LoginVirtual )

	// call data
	TCHAR				szUserId[ NICKNAME_SIZE ];
	NMVirtualKey		keyUser;
	TCHAR				szPosition[ POSITION_SIZE ];

	// return data
	// NONE

public:
	CNMLoginVirtualFunc( void );
	CNMLoginVirtualFunc( LPCTSTR szUserId, _UInt32_ uVirtualIdCode, LPCTSTR szPosition );

	DECLARE_SERIALIZABLE
};

class CNMLogoutVirtualFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( LogoutVirtual )

	// call data
	NMVirtualKey		keyUser;

	// return data
	// NONE

public:
	CNMLogoutVirtualFunc( void );
	CNMLogoutVirtualFunc( NMVirtualKey keyUser );

	DECLARE_SERIALIZABLE
};

class CNMGetMyVirtualUserListFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( GetMyVirtualUserList )

	// call data
	_UInt32_			uGameCode;

	// return data
	NMVirtualUserList	aVirtualUser;

public:
	CNMGetMyVirtualUserListFunc( void );
	CNMGetMyVirtualUserListFunc( _UInt32_ uGameCode );

	DECLARE_SERIALIZABLE
};

class CNMChangeMyPositionFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( ChangeMyPosition )

	// call data
	NMVirtualKey		keyUser;
	TCHAR				szPosition[ POSITION_SIZE ];

	// return data
	// NONE

public:
	CNMChangeMyPositionFunc( void );
	CNMChangeMyPositionFunc( LPCTSTR szPosition );
	CNMChangeMyPositionFunc( NMVirtualKey keyUser, LPCTSTR szPosition );

	DECLARE_SERIALIZABLE
};

class CNMSendRefreshMessageFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( SendRefreshMessage )

	// call data
	NMREFRESHTYPE		uRefreshType;

	// return data
	// NONE

public:
	CNMSendRefreshMessageFunc( void );
	CNMSendRefreshMessageFunc( NMREFRESHTYPE uRefreshType );

	DECLARE_SERIALIZABLE
};

class CNMGetMyInfoFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( GetMyInfo )

	// call data
	// NONE

	// return data
	CNMRealUserInfo	ruiUserInfo;
	_UInt64_			uSessionKeyHigh;	//	NOT USED
	_UInt64_			uSessionKeyLow;		//	NOT USED
	_UInt32_			uUnreadNoteCount;	//	NOT USED
	_UInt32_			uUnreadMailCount;	//	NOT USED
	TCHAR				szPassport[ PASSPORT_SIZE ];	//	NOT USED

public:
	CNMGetMyInfoFunc( void );

	DECLARE_SERIALIZABLE
};

class CNMGetUserDataListFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( GetUserDataList )
	enum
	{
		kMaxBufferSize	= 512 * 1024,
	};

	// call data
	_UInt32_			uStartPtr;

	// return data
	_UInt32_			uTotalSize;
	_UInt32_			uCurrentSize;
	BYTE				aBuffer[ kMaxBufferSize ];

public:
	CNMGetUserDataListFunc( void );

	DECLARE_SERIALIZABLE
};

class CNMChangeMyInfoFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( ChangeMyInfo )

	enum
	{
		kFlag_Status		= ( 0x1 << 0 ),
		kFlag_Word			= ( 0x1 << 1 ),
		kFlag_NickName		= ( 0x1 << 2 ),
		kFlag_MaskInfo		= ( 0x1 << 4 ),
	};

	// call data
	UINT32				uFlag;
	NMUSERSTATUS		uStatus;
	TCHAR				szWord[ TODAYWORD_SIZE ];
	TCHAR				szNickName[ NICKNAME_SIZE ];
	UINT32				uMaskInfo;
	NMGameCode			uGameCode;

	// return data
	// NONE

public:
	CNMChangeMyInfoFunc( void );
	CNMChangeMyInfoFunc( NMUSERSTATUS uStatus, LPCTSTR szWord, LPCTSTR szNickName, UINT32 uMaskInfo, NMGameCode uGameCode );

	DECLARE_SERIALIZABLE
};

class CNMGetConfigFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( GetConfig )

	enum
	{
		kFlag_AllowChatType				= ( 0x1 << 0 ),
		kFlag_AllowNoteType				= ( 0x1 << 1 ),
		kFlag_AllowGameInviteType		= ( 0x1 << 2 ),
		kFlag_AllowRequestFriendType	= ( 0x1 << 3 ),
		kFlag_MyGame					= ( 0x1 << 4 ),
		kFlag_All						= 0xFFFFFFFF,
	};

	// common data
	_UInt32_			uFlag;

	// call data
	// NONE

	// return data
	NMALLOWTYPE			uAllowChatType;
	NMALLOWTYPE			uAllowNoteType;
	NMALLOWTYPE			uAllowGameInviteType;
	NMALLOWTYPE			uAllowRequestFriendType;
	CNMMyGameInfo		mgiGameInfo;

public:
	CNMGetConfigFunc( void );
	CNMGetConfigFunc( _UInt32_ uFlag );

	DECLARE_SERIALIZABLE
};

class CNMChangeConfigFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( ChangeConfig )

	enum
	{
		kFlag_AllowChatType				= ( 0x1 << 0 ),
		kFlag_AllowNoteType				= ( 0x1 << 1 ),
		kFlag_AllowGameInviteType		= ( 0x1 << 2 ),
		kFlag_AllowRequestFriendType	= ( 0x1 << 3 ),
		kFlag_MyGame					= ( 0x1 << 4 ),
		kFlag_All						= 0xFFFFFFFF,
	};

	// call data
	_UInt32_			uFlag;
	NMALLOWTYPE			uAllowChatType;
	NMALLOWTYPE			uAllowNoteType;
	NMALLOWTYPE			uAllowGameInviteType;
	NMALLOWTYPE			uAllowRequestFriendType;
	CNMMyGameInfo		mgiGameInfo;

	// return data
	// NONE

public:
	CNMChangeConfigFunc( void );
	CNMChangeConfigFunc( NMALLOWTYPE uAllowChatType, NMALLOWTYPE uAllowNoteType, NMALLOWTYPE uAllowGameInviteType, NMALLOWTYPE uAllowRequestFriendType, CNMMyGameInfo* pmgiGameInfo );

	DECLARE_SERIALIZABLE
};

class CNMGetLocalConfigFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( GetLocalConfig )

	// call data
	NMLOCALCONFIGTYPE	uConfigType;

	// return data
	TCHAR				szValueOut[ 1024 ];

public:
	CNMGetLocalConfigFunc( void );
	CNMGetLocalConfigFunc( NMLOCALCONFIGTYPE uConfigType );

	DECLARE_SERIALIZABLE
};

class CNMChangeLocalConfigFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( ChangeLocalConfig )

	// call data
	NMLOCALCONFIGTYPE	uConfigType;
	TCHAR				szValue[ 1024 ];

	// return data
	// NONE

public:
	CNMChangeLocalConfigFunc( void );
	CNMChangeLocalConfigFunc( NMLOCALCONFIGTYPE uConfigType, LPCTSTR szValue );

	DECLARE_SERIALIZABLE
};

class CNMGetCountFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( GetCount )

	enum
	{
		kType_NULL			= 0,
		kType_UnreadMail	= 1,
		kType_UnreadNote	= 2,
	};

	// call data
	_UInt32_	uType;

	// return data
	_UInt32_	uCount;

public:
	CNMGetCountFunc( void );
	CNMGetCountFunc( _UInt32_ uType );

	DECLARE_SERIALIZABLE
};

class CNMSetCountFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( SetCount )

	enum
	{
		kType_NULL			= 0,
		kType_UnreadMail	= 1,
	};

	// call data
	_UInt32_			uCount;
	_UInt32_			uType;

	// return data
	// NONE

public:
	CNMSetCountFunc( void );
	CNMSetCountFunc( _UInt32_ uType, _UInt32_ uCount );

	DECLARE_SERIALIZABLE
};

class CNMChangeNicknameFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( ChangeNickname )

	//	call data
	NMVirtualKey	keyCharacter;
	TCHAR			szNickname[ NICKNAME_SIZE ];

	//	return data
	//	nothing...

public:
	CNMChangeNicknameFunc( void );
	CNMChangeNicknameFunc( NMVirtualKey keyCharacter, LPCTSTR szNickname );

	DECLARE_SERIALIZABLE
};

class CNMChangeMetaDataFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( ChangeMetaData )

	//	call data
	NMVirtualKey	keyCharacter;
	TCHAR			szMetaData[ METADATA_SIZE ];

	//	return data
	//	nothing...

public:
	CNMChangeMetaDataFunc( void );
	CNMChangeMetaDataFunc( NMVirtualKey keyCharacter, LPCTSTR szMetaData );

	DECLARE_SERIALIZABLE
};

class CNMRequestNewsFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( RequestNews )

public:
	CNMRequestNewsFunc( void );

	DECLARE_SERIALIZABLE
};

class CNMCheckNewsFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( CheckNews )

	NMGameCode			uEventGameCode;	//	참가할 게임 코드, 예를 들어 플러그 이벤트라면 kGameCode_plug
	NMVirtualKey		keyVirtual;		//	참가하는 캐릭터. 만약 캐릭터를 지정할 수 없는 상황이라면 게임 코드 부분만 유효하게 된다

public:
	CNMCheckNewsFunc( void );
	CNMCheckNewsFunc( NMGameCode uEventGameCode, NMVirtualKey keyVirtual = keyVirtualAll );

	DECLARE_SERIALIZABLE
};

class CNMGetDomainFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( GetDomain )

	// call data
	// NONE

	// return data
	TCHAR				szDomain[ DOMAIN_SIZE ];

public:
	CNMGetDomainFunc( void );

	DECLARE_SERIALIZABLE
};

class CNMGetVersionFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( GetVersion )

	enum
	{
		kCore	= 0,
		kDll	= 1,
	};

	// call data
	_UInt32_			uModuleIndex;

	// return data
	_UInt64_			uVersion;

public:
	CNMGetVersionFunc( void );
	CNMGetVersionFunc( _UInt32_ uModuleIndex );

	DECLARE_SERIALIZABLE
};

class CNMSetStatInfoFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( SetStatInfo )

	//
	//	NOTE: 이 타입은 CStatisticsPacket::NMSTATTYPE 이뉴머레이터와 값을 맞춰야 합니다
	//
	enum NMSTATTYPE
	{
		// Messenger Section
		kStatType_NULL = 0,
		kStatType_Login,			// 메신저 - 메신저 로그인(서버 판정)
		kStatType_Logout,			// 메신저 - 메신저 로그아웃(서버 판정)
		kStatType_Launch,			// 메신저 - 게임 시작
		kStatType_Note,				// 메신저 - 쪽지 보내기
		kStatType_Chat,				// 메신저 - 채팅 보내기(라인별)
		kStatType_GuildChat,		// 메신저 - 길드챗 보내기(라인별)
		kStatType_Soap,				// <사용안함>
		kStatType_OSVersion,		// 메신저 - OS 버전
		kStatType_ULogin,			// <사용안함>
		kStatType_UUpdateSession,	// <사용안함>
		kStatType_ULogout,			// <사용안함>

		// Plug - Click, Key		// 이벤트 발생 시 측정하며 카운트 및 순서를 수집
		kStatType_HomeClick = 20,	// 메인 창 - 홈 버튼 클릭
		kStatType_MyInfoClick,		// 메인 창 - 내 정보 버튼 클릭
		kStatType_NoteClick,		// 메인 창 - 쪽지 버튼 클릭
		kStatType_InfoPanelLClick,	// 메인 창 - 전광판 좌클릭
		kStatType_InfoPanelRClick,	// 메인 창 - 전광판 우클릭
		kStatType_ConfigClick,		// 메인 창 - 설정 버튼 클릭
		kStatType_GameStartClick,	// 메인 창 - 게임 시작 버튼 클릭
		kStatType_EnterKey,			// 메인 창 - 엔터 키 누름 (게임 시작)
		kStatType_GameSelectClick,	// 메인 창 - 게임 선택 버튼 클릭 (좌우 버튼)
		kStatType_LeftRightKey,		// 메인 창 - 게임 선택 키 클릭 (화살표 좌우키)
		kStatType_SpaceKey,			// 메인 창 - Space 키 누름 (친구/길드 리스트)
		kStatType_FriendListClick,	// 메인 창 - 친구/길드 리스트 클릭
		kStatType_AltF4,			// 메인 창 - Alt + F4 키 누름 (닫기)
		kStatType_Esc,				// 메인 창 - Esc 키 누름 (닫기)
		kStatType_CloseClick,		// 메인 창 - 닫기 버튼 클릭
		kStatType_MinimizeClick,	// 메인 창 - 최소화 버튼 클릭
		kStatType_MainRClick,		// 메인 창 - 우클릭 (팝업메뉴 사용)

		kStatType_FriendTabClick = 40,	// 친구/길드 리스트 - 친구 탭 클릭
		kStatType_GuildTabClick,		// 친구/길드 리스트 - 길드 탭 클릭
		kStatType_FriendDClick,			// 친구/길드 리스트 - 친구 더블클릭 (채팅)
		kStatType_GuildMemberDClick,	// 친구/길드 리스트 - 길드원 더블클릭 (채팅)
		kStatType_GuildChatClick,		// 친구/길드 리스트 - 길드챗 버튼 클릭 (길드챗)
		kStatType_GuildHomeClick,		// 친구/길드 리스트 - 길드홈 버튼 클릭
		kStatType_GuildMainClick,		// 친구/길드 리스트 - 길드메인 버튼 클릭
		kStatType_StateClick,			// 친구/길드 리스트 - 현재 상태 버튼 클릭
		kStatType_FriendGroupRClick,	// 친구/길드 리스트 - 그룹명 우클릭 (팝업메뉴 사용)
		kStatType_FriendRClick,			// 친구/길드 리스트 - 친구 우클릭 (팝업메뉴 사용)
		kStatType_GuildRClick,			// 친구/길드 리스트 - 길드원 우클릭 (팝업메뉴 사용)

		kStatType_TrayLClick = 60,	// 트레이 - 좌클릭 (팝업메뉴 사용)
		kStatType_TrayRClick,		// 트레이 - 우클릭 (팝업메뉴 사용)
		kStatType_TrayDClick,		// 트레이 - 더블클릭 (메인 창 열기)

		//////////////////////////////////
		kStatType_NoCountSection = 70,	// 횟수 누적시키지 않는 영역. Data1을 사용할 수 있음
		// 토스트는 gamecode 가 모두 kGameCode_plug 로 저장됨
		kStatType_ToastPopup = 70,	// 토스트 - 팝업 (Data1: 토스트 종류)
		kStatType_ToastClick,		// 토스트 - 클릭 (Data1: 토스트 종류)
		// Data1: 토스트 종류
		//kGameToastPaneType_NULL		= 0
		//kGameToastPaneType_Online		= 1
		//kGameToastPaneType_Chat		= 2
		//kGameToastPaneType_LevelUp	= 3
		//kGameToastPaneType_License	= 4
		//kGameToastPaneType_Play		= 5
		//kGameToastPaneType_PVPLevelUp	= 6
		//kGameToastPaneType_StageClear	= 7
		//kGameToastPaneType_ObtainItem	= 8
		//kGameToastPaneType_GradeUp	= 9

		//
		// 팝업 메뉴
		// 일반 서브메뉴
		//kNMCommandId_UserCommon_OpenProfile = 136
		//kNMCommandId_UserCommon_SendNote	= 137
		//kNMCommandId_UserCommon_OpenChat	= 138
		//kNMCommandId_UserCommon_AddFriend	= 139

		kStatType_PopupMain = 80,	// 팝업메뉴 - 메인/트레이 (Data1: 서브 메뉴)
		// Data1: 서브 메뉴
		//kNMCommandId_Login		= 100
		//kNMCommandId_Logout		= 101
		//kNMCommandId_Exit			= 102
		//kNMCommandId_OpenMain		= 103
		//kNMCommandId_OpenOptions	= 104
		//kNMCommandId_OpenHelp		= 105
		//kNMCommandId_OpenAbout	= 106			


		kStatType_PopupGroup,		// 팝업메뉴 - 친구그룹 (Data1: 서브 메뉴)
		// Data1: 서브 메뉴
		//kNMCommandId_GroupItem_ChangeName	= 113
		//kNMCommandId_GroupItem_Delete		= 114
		//kNMCommandId_GroupItem_GroupChat	= 115
		//kNMCommandId_GroupItem_GroupNote	= 116
		//kNMCommandId_GroupItem_AddGroup	= 117
		//kNMCommandId_GroupItem_SortStatus	= 118
		//kNMCommandId_GroupItem_SortGroup	= 119
		//kNMCommandId_GroupItem_Refresh	= 120

		kStatType_PopupFriend,		// 팝업메뉴 - 친구 (Data1: 서브 메뉴)
		// Data1: 서브 메뉴
		//kNMCommandId_FriendItem_OpenChat	= 121
		//kNMCommandId_FriendItem_SendFile	= 122
		//kNMCommandId_FriendItem_SendNote	= 123
		//kNMCommandId_FriendItem_OpenProfile = 124
		//kNMCommandId_FriendItem_Block		= 125
		//kNMCommandId_FriendItem_Delete	= 126
		//kNMCommandId_FriendItem_MoveGroup	= 127
		//kNMCommandId_FriendItem_CopyGroup	= 128

		kStatType_PopupGuild,		// 팝업메뉴 - 길드원 (Data1: 서브 메뉴)
		// Data1: 서브 메뉴
		//kNMCommandId_GuildMemberItem_OpenChat	= 129	
		//kNMCommandId_GuildMemberItem_SendNote	= 130
		//kNMCommandId_GuildMemberItem_OpenProfile	= 131
		//kNMCommandId_GuildMemberItem_AddFriend	= 132

		kStatType_PopupStatus,		// 팝업메뉴 - 현재 상태 (Data1: 서브 메뉴)
		// Data1: 서브 메뉴
		//kNMCommandId_ChangeStatusOnline	= 109
		//kNMCommandId_ChangeStatusAway		= 110
		//kNMCommandId_ChangeStatusBusy		= 111
		//kNMCommandId_ChangeStatusAppearOffline = 112

		// Plug - Status					// 메신저 로긴 시점에 측정하며 로그인별 1회 수집
		kStatType_Settings = 90,			// 환경설정의 모든 저장 값 (Data1: 설정 타입, Data2: 값)
		kStatType_MainPosition,				// 환경설정의 메인 창 위치 (로그인별 1회 수집)
		kStatType_FriendListSize,			// 환경설정의 친구 리스트 크기/위치 (Data1: 크기, Data2: 아래(0), 위(1))
		kStatType_FriendGroupingMethod,		// 환경설정의 친구리스트 정렬 방식 (Data1: 상태별(1), 그룹별(2))

		// P2P Test report					// P2P Test 결과 보고용
		kStatType_P2PTest_Start = 100,
		kStatType_P2PTest_TestInfo,			// 테스트 정보 (Data1: 데이터 크기, Data2: 전송 주기)
		kStatType_P2PTest_LoginError,
		kStatType_P2PTest_SendDataError,
		kStatType_P2PTest_LoginReply,
		kStatType_P2PTest_SendDataReply,
		kStatType_P2PTest_SendCount,		// 전송 횟수
		kStatType_P2PTest_ResponseCount,	// 응답 횟수
		kStatType_P2PTest_MinResponseTime,	// 응답 시간 최소
		kStatType_P2PTest_MaxResponseTime,	// 응답 시간 최대
		kStatType_P2PTest_AvgResponseTime,	// 응답 시간 평균 (ms)
		kStatType_P2PTest_End,

		// End mark
		kStatType_End,
	};

	// call data
	NMSTATTYPE			uType;
	UINT32				uClientDllGameCode;
	UINT32				uGameCode;
	UINT32				uData1;
	UINT32				uData2;

	// return data
	// NONE

public:
	CNMSetStatInfoFunc( void );
	CNMSetStatInfoFunc( NMSTATTYPE uType, UINT32 uClientDllGameCode, UINT32 uGameCode, UINT32 uData1, UINT32 uData2 );

	DECLARE_SERIALIZABLE
};

class CNMSetLocaleFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( SetLocale )

public:
	// call data
	NMLOCALEID			uLocaleID;

	//	DO NOT USE THIS!
	UINT32				uRegionCode;

	// return data
	// NONE

public:
	CNMSetLocaleFunc( void );
	CNMSetLocaleFunc( NMLOCALEID uLocaleID, UINT32 uRegionCode = 0 );

	DECLARE_SERIALIZABLE
};

class CNMSetCodePageFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( SetCodePage )

public:
	UINT32				uCodePage;

	// return data
	// NONE~ng

public:
	CNMSetCodePageFunc( void );
	CNMSetCodePageFunc( UINT32 uCodePage );

	DECLARE_SERIALIZABLE
};

class CNMGetNexonPassportFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( GetNexonPassport )

	// call data
	BOOL				bSecondary;

	// return data
	TCHAR				szNexonPassport[ PASSPORT_SIZE ];

public:
	CNMGetNexonPassportFunc();
	CNMGetNexonPassportFunc( BOOL bSecondary );

	DECLARE_SERIALIZABLE
};

class CNMGetSecurityCodeCNFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( GetSecurityCodeCN )

	// return data
	UINT32				uSecurityCodeCN;

public:
	CNMGetSecurityCodeCNFunc();

	DECLARE_SERIALIZABLE
};

class CNMGetSecurityQuestionFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( GetSecurityQuestion )

	// return data
	StringZip< SECURITYCN_SIZE >	szSecurityQuestion;

public:
	CNMGetSecurityQuestionFunc();

	DECLARE_SERIALIZABLE
};

class CNMGetNxHWIDFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( GetNxHWID )

	// return data
	StringZip< NXHWID_SIZE >	szNxHWID;

public:
	CNMGetNxHWIDFunc();

	DECLARE_SERIALIZABLE
};

class CNMGetServerInfoFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( GetServerInfo )

	//
	// NOTE: 이 타입은 ServerInfoLayer::LayerType 이뉴머레이터와 값을 맞춰야 합니다.
	//
	enum LayerType
	{
		DEFAULT_LAYER		= 0,
		SYSTEM_LAYER		= 1,
		DLLCONFIG_LAYER		= 2,
		EXECONFIG_LAYER		= 3,
		CONFIG_LAYER		= 4,
		CUSTOM_LAYER		= 5,
		LAYER_COUNT			= 6
	};

	// call data
	ServerInfo::ServerType		uServerType;
	LayerType					uLayerType;

	// return data
	ServerInfoList				aServerInfo;

public:
	CNMGetServerInfoFunc();
	CNMGetServerInfoFunc( ServerInfo::ServerType uServerType, LayerType uLayerType = DEFAULT_LAYER );

	DECLARE_SERIALIZABLE
};

class CNMGetFriendListFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( GetFriendList )

	// call data
	NMVirtualKey		keyVirtuality;

	// return data
	NMCategoryUserList	aCateUserList;
	NMFriendList		aUnrelated;

public:
	CNMGetFriendListFunc( void );
	CNMGetFriendListFunc( NMVirtualKey keyVirtuality );

	DECLARE_SERIALIZABLE
};

class CNMGetFriendInfoFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( GetFriendInfo )

	// call data
	NMFriendKey			keyFriend;

	// return data
	CNMFriendInfo		fiFriendInfo;

public:
	CNMGetFriendInfoFunc( void );
	CNMGetFriendInfoFunc( NMFriendKey keyFriend );

	DECLARE_SERIALIZABLE
};

class CNMRequestNewFriendFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( RequestNewFriend )

	// call data
	CNMRequestNewFriendData	
						rnfData;

	// return data
	// NONE

public:
	CNMRequestNewFriendFunc( void );
	CNMRequestNewFriendFunc( CNMRequestNewFriendData& rnfData );

	DECLARE_SERIALIZABLE
};

class CNMConfirmNewFriendFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( ConfirmNewFriend )

	// call data
	_UInt32_			uSerialNo;
	NMCONFIRMCODE		uConfirmCode;
	_UInt32_			uInsertCategoryCode;

	// return data
	// NONE

public:
	CNMConfirmNewFriendFunc( void );
	CNMConfirmNewFriendFunc( _UInt32_ uSerialNo, NMCONFIRMCODE uConfirmCode, _UInt32_ uInsertCategoryCode );

	DECLARE_SERIALIZABLE
};

class CNMBlockFriendFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( BlockFriend )

	// call data
	NMFriendKey			keyFriend;
	BOOL				bBlock;

	// return data
	// NONE
	
public:
	CNMBlockFriendFunc( void );
	CNMBlockFriendFunc( NMFriendKey keyFriend, BOOL bBlock );

	DECLARE_SERIALIZABLE
};

class CNMChangeFriendMemoFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( ChangeFriendMemo )

	// call data
	_UInt64_			uIDCode;
	TCHAR				szMemo[ MEMO_SIZE ];

	// return data
	// NONE
	
public:
	CNMChangeFriendMemoFunc( void );
	CNMChangeFriendMemoFunc( _UInt64_ uIDCode, LPCTSTR szMemo );

	DECLARE_SERIALIZABLE
};

class CNMChangeFriendMemoExFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( ChangeFriendMemoEx )

	// call data
	NMFriendKey			keyFriend;
	TCHAR				szMemo[ MEMO_SIZE ];

	// return data
	// NONE
	
public:
	CNMChangeFriendMemoExFunc( void );
	CNMChangeFriendMemoExFunc( NMFriendKey keyFriend, LPCTSTR szMemo );

	DECLARE_SERIALIZABLE
};

class CNMAddFriendToCategoryFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( AddFriendToCategory )

	// call data
	NMFriendKey			keyFriend;
	UINT32				uCategoryCode;
	BOOL				bSystemCall;

	// return data
	// NONE
	
public:
	CNMAddFriendToCategoryFunc( void );
	CNMAddFriendToCategoryFunc( NMFriendKey keyFriend, UINT32 uCategoryCode, BOOL bSystemCall = FALSE );

	DECLARE_SERIALIZABLE
};

class CNMDeleteFriendFromCategoryFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( DeleteFriendFromCategory )

	// call data
	NMFriendKey			keyFriend;
	UINT32				uCategoryCode;
	BOOL				bSystemCall;

	// return data
	// NONE
	
public:
	CNMDeleteFriendFromCategoryFunc( void );
	CNMDeleteFriendFromCategoryFunc( NMFriendKey keyFriend, UINT32 uCategoryCode, BOOL bSystemCall = FALSE );

	DECLARE_SERIALIZABLE
};

class CNMMoveFriendCategoryFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( MoveFriendCategory )

	// call data
	NMFriendKey			keyFriend;
	UINT32				uFromCateCode;
	UINT32				uToCateCode;
	BOOL				bSystemCall;

	// return data
	// NONE
	
public:
	CNMMoveFriendCategoryFunc( void );
	CNMMoveFriendCategoryFunc( NMFriendKey keyFriend, UINT32 uFromCateCode, UINT32 uToCateCode, BOOL bSystemCall );

	DECLARE_SERIALIZABLE
};

class CNMAddCategoryFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( AddCategory )

	// call data
	NMVirtualKey		keyOwner;
	TCHAR				szCategoryName[ CATEGORY_SIZE ];
	NMCATEGORYPROPERTY	uProperty;
	NMALLOWTYPE			uAllowType;
	
	// return data
	// NONE

public:
	CNMAddCategoryFunc( void );
	CNMAddCategoryFunc( NMVirtualKey keyOwner, LPCTSTR szCategoryName, NMCATEGORYPROPERTY uProperty = kCateProp_None, NMALLOWTYPE uAllowType = kConfigAllowValue_FromAll );

	DECLARE_SERIALIZABLE
};

class CNMDeleteCategoryFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( DeleteCategory )

	// call data
	UINT32				uCategoryCode;
	BOOL				bSystemCall;

	// return data
	// NONE

public:
	CNMDeleteCategoryFunc( void );
	CNMDeleteCategoryFunc( UINT32 uCategoryCode, BOOL bSystemCall = FALSE );

	DECLARE_SERIALIZABLE
};

class CNMChangeCategoryNameFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( ChangeCategoryName )

	// call data
	UINT32				uCategoryCode;
	TCHAR				szCategoryName[ CATEGORY_SIZE ];
	BOOL				bSystemCall;

	// return data
	// NONE

public:
	CNMChangeCategoryNameFunc( void );
	CNMChangeCategoryNameFunc( UINT32 uCategoryCode, LPCTSTR szCategoryName, BOOL bSystemCall = FALSE );

	DECLARE_SERIALIZABLE
};

class CNMChangeCategoryPropertyFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( ChangeCategoryProperty )

	// call data
	UINT32				uCategoryCode;
	NMCATEGORYPROPERTY	uProperty;
	BOOL				bSystemCall;

	// return data
	// NONE

public:
	CNMChangeCategoryPropertyFunc( void );
	CNMChangeCategoryPropertyFunc( UINT32 uCategoryCode, NMCATEGORYPROPERTY uProperty, BOOL bSystemCall = FALSE );

	DECLARE_SERIALIZABLE
};

class CNMChangeCategoryAllowTypeFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( ChangeCategoryAllowType )

	// call data
	UINT32				uCategoryCode;
	NMALLOWTYPE			uAllowType;
	BOOL				bSystemCall;

	// return data
	// NONE

public:
	CNMChangeCategoryAllowTypeFunc( void );
	CNMChangeCategoryAllowTypeFunc( UINT32 uCategoryCode, NMALLOWTYPE uAllowType, BOOL bSystemCall = FALSE );

	DECLARE_SERIALIZABLE
};

class CNMGetNoteBoxFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( GetNoteBox )

	// call data
	NMNOTEBOXTYPE		uBoxType;
	_UInt32_			uNoteSerial;
	_UInt32_			uStartPtr;

	// return data
	NMNoteList			aNotes;
	BOOL				bMore;

public:
	CNMGetNoteBoxFunc( void );
	CNMGetNoteBoxFunc( NMNOTEBOXTYPE uBoxType, _UInt32_ uNoteSerial );
	CNMGetNoteBoxFunc( NMNOTEBOXTYPE uBoxType, _UInt32_ uNoteSerial, _UInt32_ uStartPtr );

	DECLARE_SERIALIZABLE
};

class CNMSendNoteMsgFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( SendNoteMsg )

	// call data
	BOOL				bSaveToPermBox;
	CNMNoteInfo			noteInfo;

	// return data
	// NONE

public:
	CNMSendNoteMsgFunc( void );
	CNMSendNoteMsgFunc( CNMNoteInfo::NMNOTETYPE uNoteType, CNMNoteInfo::NMNOTESUBTYPE uSubType, LPCTSTR szTargetId, LPCTSTR szMessage, LPCTSTR szArgument, LPCTSTR szCustomData, BOOL bSaveToPermBox );
	CNMSendNoteMsgFunc( CNMNoteInfo::NMNOTETYPE uNoteType, CNMNoteInfo::NMNOTESUBTYPE uSubType, NMVirtualKey keyFromVirtual, LPCTSTR szTargetId, LPCTSTR szMessage, LPCTSTR szArgument, LPCTSTR szCustomData, BOOL bSaveToPermBox );

	DECLARE_SERIALIZABLE
};

class CNMSendAccuseNoteMsgFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( SendAccuseNoteMsg )

	// call data
	CNMReportInfo		riReportInfo;

	// return data
	// NONE

public:
	CNMSendAccuseNoteMsgFunc( void );
	CNMSendAccuseNoteMsgFunc( CNMReportInfo::NMREPORTSUBTYPE uSubType, LPCTSTR szReason, _UInt32_ uNoteSerial = 0 );

	DECLARE_SERIALIZABLE
};

class CNMProcessNoteFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( ProcessNote )

	enum
	{
		kType_SetReadFlag	= 0,
		kType_SaveToPerm	= 1,
		kType_DeleteNote	= 2,
	};

	// call data
	_UInt32_			uType;
	_UInt32_			uNoteSerial;

	// return data
	// NONE

public:
	CNMProcessNoteFunc( void );
	CNMProcessNoteFunc( _UInt32_ uType, _UInt32_ uNoteSerial );

	DECLARE_SERIALIZABLE
};

//////////////////////////////////////////////////////////////////////////
//	CNMSendNoteInfoFunc
//////////////////////////////////////////////////////////////////////////

class CNMSendNoteInfoFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( SendNoteInfo )

	//	call data
	BOOL				bSaveToPermBox;
	CNMNoteInfo2		noteInfo;

	//	return data
	//	NONE

public:
	CNMSendNoteInfoFunc( void );
	CNMSendNoteInfoFunc( CNMNoteInfo & noteInfo, BOOL bSaveToPermBox );

	DECLARE_SERIALIZABLE
};

//////////////////////////////////////////////////////////////////////////
//	CNMGetNoteBox2Func : public CNMFunc
//////////////////////////////////////////////////////////////////////////

class CNMGetNoteBox2Func : public CNMFunc
{
public:
	DECLARE_NMFUNC( GetNoteBox2 )

	//	call data
	NMNOTEBOXTYPE		uBoxType;
	UINT32				uNoteSerial;
	UINT32				uStartPtr;

	//	return data
	NMNoteInfoList		aNotes;
	BOOL				bMore;

public:
	CNMGetNoteBox2Func();
	CNMGetNoteBox2Func( NMNOTEBOXTYPE uBoxType, UINT32 uNoteSerial, UINT32 uStartPtr = 0 );

	DECLARE_SERIALIZABLE
};

//////////////////////////////////////////////////////////////////////////
//	CNMGetRejectedUserListFunc
//////////////////////////////////////////////////////////////////////////

class CNMGetRejectedUserListFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( GetRejectedUserList )

	//	call data
	//	NONE

	//	return data
	NMRejectedUserList	rejectedUserList;

public:
	CNMGetRejectedUserListFunc();

	DECLARE_SERIALIZABLE
};

//////////////////////////////////////////////////////////////////////////
//	CNMAppendRejectedUserFunc
//////////////////////////////////////////////////////////////////////////

class CNMAppendRejectedUserFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( AppendRejectedUser )

	//	call data
	CNMRejectedUser	rejectedUser;

	//	return data
	//	NONE

public:
	CNMAppendRejectedUserFunc();
	CNMAppendRejectedUserFunc(CNMRejectedUser & rejectedUser);

	DECLARE_SERIALIZABLE
};

//////////////////////////////////////////////////////////////////////////
//	CNMRemoveRejectedUserFunc
//////////////////////////////////////////////////////////////////////////

class CNMRemoveRejectedUserFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( RemoveRejectedUser )

	//	call data
	CNMRejectedUser	rejectedUser;

	//	return data
	//	NONE

public:
	CNMRemoveRejectedUserFunc();
	CNMRemoveRejectedUserFunc(CNMRejectedUser const & rejectedUser);
	CNMRemoveRejectedUserFunc(CNMCharacter const & rejectedCharacter, LPCTSTR pszMemo = NULL);

	DECLARE_SERIALIZABLE
};

class CNMGetMyGuildListExFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( GetMyGuildListEx )

	// call data
	// NONE

	// return data
	CNMGuildListExInfo	gliGuildList;

public:
	CNMGetMyGuildListExFunc( void );

	DECLARE_SERIALIZABLE
};

class CNMMonitorGuildOnlineInfoFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( MonitorGuildOnlineInfo )

	// call data
	UINT32				uGuildSerialNo;
	BOOL				bMonitorAttach;
	UINT32				uServingProcessID;
	// NONE

	// return data

public:
	CNMMonitorGuildOnlineInfoFunc( void );
	CNMMonitorGuildOnlineInfoFunc( UINT32 uGuildSerialNo, BOOL bMonitorAttach );
	CNMMonitorGuildOnlineInfoFunc( UINT32 uGuildSerialNo, BOOL bMonitorAttach, UINT32 uServingProcessID );

	DECLARE_SERIALIZABLE
};

class CNMMonitorGuildOnlineInfoExFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( MonitorGuildOnlineInfoEx )

	// call data
	NMGuildKey			uGuildKey;
	BOOL				bMonitorAttach;
	UINT32				uServingProcessID;
	UINT32				uGameCode;
	UINT32				uVirtualIDCode;

	// return data
	// NONE

public:
	CNMMonitorGuildOnlineInfoExFunc();
	CNMMonitorGuildOnlineInfoExFunc( NMGuildKey uGuildKey, BOOL bMonitorAttach );
	CNMMonitorGuildOnlineInfoExFunc( NMGuildKey uGuildKey, BOOL bMonitorAttach, UINT32 uServingProcessID );
	CNMMonitorGuildOnlineInfoExFunc( NMGuildKey uGuildKey, BOOL bMonitorAttach, UINT32 uServingProcessID, UINT32 uGameCode, UINT32 uVirtualIDCode );

	DECLARE_SERIALIZABLE
};

class CNMExecutePatcherFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( ExecutePatcher )

	// call data
	TCHAR				szUrl[ 256 ];
	TCHAR				szPatchDir[ 256 ];
	TCHAR				szExecCmdLine[ 2000 ];
	BOOL				bWaitParentExit;
	BOOL				bWaitPatchEnd;

	// return data
	// NONE

public:
	CNMExecutePatcherFunc( void );
	CNMExecutePatcherFunc( LPCTSTR szUrl, LPCTSTR szPatchDir, LPCTSTR szExecCmdLine, BOOL bWaitParentExit, BOOL bWaitPatchEnd );

	DECLARE_SERIALIZABLE
};

class CNMExecuteLauncherFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( ExecuteLauncher )

	// call data
	_UInt32_			uGameCode;
	_UInt32_			uLaunchType;
	TCHAR				szArgument[ 2000 ];
	BOOL				bWaitParentExit;

	// return data
	// NONE

public:
	CNMExecuteLauncherFunc( void );
	CNMExecuteLauncherFunc( _UInt32_ uGameCode, _UInt32_ uLaunchType, LPCTSTR szArgument, BOOL bWaitParentExit );

	DECLARE_SERIALIZABLE
};

class CNMExecuteNGMPatcherFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( ExecuteNGMPatcher )

	// call data
	_UInt32_			uGameCode;
	TCHAR				szUrl[ SHRT_MAX ];
	TCHAR				szPatchDir[ MAX_PATH ];
	TCHAR				szExecCmdLine[ 1024 ];
	BOOL				bWaitParentExit;
	BOOL				bSmallDialog;
	BOOL				bUseLocalNGM;
	TCHAR				szSessionID[ 32 ];
	_UInt32_			uPatchType;
	TCHAR				szPosition[ 128 ];

	// return data
	// NONE

public:
	CNMExecuteNGMPatcherFunc( void );
	CNMExecuteNGMPatcherFunc( _UInt32_ uGameCode, LPCTSTR szUrl, LPCTSTR szPatchDir, LPCTSTR szExecCmdLine, BOOL bWaitParentExit, BOOL bSmallDialog, BOOL bUseLocalNGM, LPCTSTR szSessionID, _UInt32_ uPatchType, LPCTSTR szPosition );

	DECLARE_SERIALIZABLE
};

class CNMExecuteNGMLauncherFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( ExecuteNGMLauncher )

	// call data
	_UInt32_			uGameCode;
	_UInt32_			uLaunchType;
	TCHAR				szArgument[ 1024 ];
	BOOL				bWaitParentExit;
	TCHAR				szPassport[ 1024 ];
	BOOL				bUseLocalNGM;
	TCHAR				szStickPath[ 8 ];
	TCHAR				szPosition[ 128 ];

	// return data
	// NONE

public:
	CNMExecuteNGMLauncherFunc( void );
	CNMExecuteNGMLauncherFunc( _UInt32_ uGameCode, _UInt32_ uLaunchType, LPCTSTR szArgument, BOOL bWaitParentExit, LPCTSTR szPassport, BOOL bUseLocalNGM, LPCTSTR szStickPath, LPCTSTR szPosition );

	DECLARE_SERIALIZABLE
};

class CNMExecuteNGMInstallerFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( ExecuteNGMInstaller )

	// call data
	_UInt32_			uGameCode;
	BOOL				bWaitParentExit;
	BOOL				bLaunchAfterInstall;
	TCHAR				szArgument[ 1024 ];
	TCHAR				szPassport[ 1024 ];
	TCHAR				szSessionID[ 32 ];
	TCHAR				szPosition[ 128 ];

	// return data
	// NONE

public:
	CNMExecuteNGMInstallerFunc( void );
	CNMExecuteNGMInstallerFunc( _UInt32_ uGameCode, BOOL bWaitParentExit, BOOL bLaunchAfterInstall, LPCTSTR szArgument, LPCTSTR szPassport, LPCTSTR szSessionID, LPCTSTR szPosition );

	DECLARE_SERIALIZABLE
};

class CNMIsNGMInstalledFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( IsNGMInstalled )

	// call data
	// NONE

	// return data
	BOOL				bInstalled;
	
public:
	CNMIsNGMInstalledFunc( void );
	
	DECLARE_SERIALIZABLE
};

class CNMRestrictedWordsFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( RestrictedWords )

	enum
	{
		kOpType_HasRestrictedWords		= 1,
		kOpType_ReplaceRestrictedWords	= 2,

		kMaxTextSize					= 1024,
	};

	// call data
	UINT32						uOpType;
	NMGameCode					uGameCode;
	StringZip< kMaxTextSize >	szText;

	// return data
	BOOL						bHasRestrictedWords;

public:
	CNMRestrictedWordsFunc();
	CNMRestrictedWordsFunc( UINT32 uOpType, NMGameCode uGameCode, LPCTSTR pszText );

	DECLARE_SERIALIZABLE
};

class CNMMinimizeAllFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( MinimizeAll )

	// call data
	BOOL				bMinimize;

	// return data
	// NONE

public:
	CNMMinimizeAllFunc( void );
	CNMMinimizeAllFunc( BOOL bMinimize );

	DECLARE_SERIALIZABLE
};

class CNMLogReportSyncFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( LogReportSync )

	// call data
	UINT16				uLogType;

	// return data
	// NONE

public:
	CNMLogReportSyncFunc( void );
	CNMLogReportSyncFunc( UINT16 uLogType );

	DECLARE_SERIALIZABLE
};

class CNMSetServerInfoFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( SetServerInfo )

	// call data
	ServerInfoList aServerInfo;

	// return data
	// NONE

public:
	CNMSetServerInfoFunc();
	CNMSetServerInfoFunc( ServerInfoList const & aServerInfo );

	DECLARE_SERIALIZABLE
};

class CNMWriteToWiselogFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( WriteToWiselog )

	// call data
	TCHAR				szUrl[ 1024 ];

	// return data
	// NONE

public:
	CNMWriteToWiselogFunc( void );
	CNMWriteToWiselogFunc( LPCTSTR szUrl );

	DECLARE_SERIALIZABLE
};

class CNMExecuteCommonFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( ExecuteCommon )

	enum
	{
		kType_StartMessenger		= 0,
		kType_ReinstallMessenger,
		kType_PatchNMService,				//	obsolete
		kType_DeletePatcher,				//	obsolete
		kType_PatchMessenger,				//	obsolete
		kType_StartPlug,
		kType_PatchPlug,					//	obsolete
		kType_ReinstallPlug,
	};

	// call data
	_UInt32_			uType;
	TCHAR				szArgument[ DOMAIN_SIZE ];

	// return data
	// NONE

public:
	CNMExecuteCommonFunc( void );
	CNMExecuteCommonFunc( _UInt32_ uType, LPCTSTR szArgument );

	DECLARE_SERIALIZABLE
};

class CNMGetUrlFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( GetUrl )

	// call data
	NMURLTYPE			uType;
	NMURLPAGE			uPage;			// for kURLType_Page
	_UInt32_			uGameCode;		// for kURLType_Game
	_UInt32_			uGuildSerial;	// for kURLType_Guild
	TCHAR				szUrl[ 256 ];	// for kURLType_DirectURL
	_UInt32_			uParam;			// additional number param
	TCHAR				szParam[ 256 ];	// additional string param
	BOOL				bLoginDirect;
	BOOL				bIsPopup;
	BOOL				bNoActiveX;

	// return data
	TCHAR				szUrlOut[ 1024 ];

public:
	CNMGetUrlFunc( void );

	DECLARE_SERIALIZABLE
};

class CNMGetUrlExFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( GetUrlEx )

	// call data
	NMURLTYPE			uType;
	NMURLPAGE			uPage;			// for kURLType_Page
	_UInt32_			uGameCode;		// for kURLType_Game
	NMGuildKey			uGuildKey;		// for kURLType_Guild
	TCHAR				szUrl[ 512 ];	// for kURLType_DirectURL
	_UInt32_			uParam;			// additional number param
	TCHAR				szParam[ 256 ];	// additional string param
	BOOL				bLoginDirect;
	BOOL				bIsPopup;
	BOOL				bNoActiveX;
	UINT32				uMaskGameCode;	// 게임에서 호출하면 로그인 처리 화면이 달라집니다

	// return data
	TCHAR				szUrlOut[ 1024 ];

public:
	CNMGetUrlExFunc( void );

	DECLARE_SERIALIZABLE
};

class CNMDownloadGuildMarkFileFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( DownloadGuildMarkFile )

	// call data
	NMGameCode			uGameCode;
	NMGuildKey			uGuildKey;
	TCHAR				szDownloadDir[ 256 ];

	// return data
	// NONE

public:
	CNMDownloadGuildMarkFileFunc( void );
	CNMDownloadGuildMarkFileFunc( NMGameCode uGameCode, NMGuildKey uGuildKey, LPCTSTR szDownloadDir );

	DECLARE_SERIALIZABLE
};

class CNMDownloadFileFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( DownloadFile )

	// call data
	TCHAR				szUrl[ 256 ];
	TCHAR				szFileName[ 256 ];

	// return data
	// NONE

public:
	CNMDownloadFileFunc( void );
	CNMDownloadFileFunc( LPCTSTR szUrl, LPCTSTR szFileName );

	DECLARE_SERIALIZABLE
};

class CNMUploadFileFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( UploadFile )

	// call data
	TCHAR				szUrl[ 1024 ];
	TCHAR				szFileName[ 256 ];
	TCHAR				szPostField[ 256 ];
	BOOL				bAppendPassport;

	// return data
	TCHAR				szResponse[ 1024 ];

public:
	CNMUploadFileFunc( void );
	CNMUploadFileFunc( LPCTSTR szUrl, LPCTSTR szFileName, LPCTSTR szPostField, BOOL bAppendPassport );

	DECLARE_SERIALIZABLE
};

class CNMGetSupportGameListFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( GetSupportGameList )

	// call data
	BOOL				bAll;

	// return data
	NMGameCodeList		aGameCodeList;

public:
	CNMGetSupportGameListFunc( void );
	CNMGetSupportGameListFunc( BOOL bAll );

	DECLARE_SERIALIZABLE
};

class CNMGetGameServerListFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( GetGameServerList )

	// call data
	NMGameCode				uGameCode;

	// return data
	NMGameServerCodeList	aGameServerCodeList;

public:
	CNMGetGameServerListFunc( void );
	CNMGetGameServerListFunc( NMGameCode uGameCode );

	DECLARE_SERIALIZABLE
};

class CNMGetGameFullNameFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( GetGameFullName  )

	// call data
	NMGameCode				uGameCode;

	// return data
	TCHAR					szGameFullName[ 128 ];

public:
	CNMGetGameFullNameFunc( void );
	CNMGetGameFullNameFunc( NMGameCode uGameCode );

	DECLARE_SERIALIZABLE
};

class CNMGetGameShortNameFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( GetGameShortName  )

	// call data
	NMGameCode				uGameCode;

	// return data
	TCHAR					szGameShortName[ 128 ];

public:
	CNMGetGameShortNameFunc( void );
	CNMGetGameShortNameFunc( NMGameCode uGameCode );

	DECLARE_SERIALIZABLE
};

class CNMGetGameFriendTitleFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( GetGameFriendTitle  )

	// call data
	NMGameCode				uGameCode;

	// return data
	TCHAR					szGameFriendTitle[ 128 ];

public:
	CNMGetGameFriendTitleFunc( void );
	CNMGetGameFriendTitleFunc( NMGameCode uGameCode );

	DECLARE_SERIALIZABLE
};

class CNMGetGameServerNameFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( GetGameServerName  )

	// call data
	NMGameCode				uGameCode;
	NMGameServerCode		uGameServerCode;

	// return data
	TCHAR					szGameServerName[ 128 ];

public:
	CNMGetGameServerNameFunc( void );
	CNMGetGameServerNameFunc( NMGameCode uGameCode, NMGameServerCode uGameServerCode );

	DECLARE_SERIALIZABLE
};

class CNMGetPathFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( GetPath )

	enum
	{
		kCommon				= 0,
		kAvatarCache		= 1,
		kMessengerClient	= 2,
		kUserData			= 3,
	};

	// call data
	_UInt32_			uPathType;

	// return data
	TCHAR				szPath[ MAX_PATH ];

public:
	CNMGetPathFunc( void );
	CNMGetPathFunc( _UInt32_ uPathType );

	DECLARE_SERIALIZABLE
};

class CNMSetSessionValueFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( SetSessionValue )

	// call data
	TCHAR				szName[ 32 ];
	TCHAR				szValue[ 256 ];
	BOOL				bAttachToProcess;
	_UInt32_			uProcessId;

	// return data
	// NONE

public:
	CNMSetSessionValueFunc( void );
	CNMSetSessionValueFunc( LPCTSTR szName, LPCTSTR szValue, BOOL bAttachToProcess );

	DECLARE_SERIALIZABLE
};

class CNMGetSessionValueFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( GetSessionValue )

	// call data
	TCHAR				szName[ 32 ];

	// return data
	TCHAR				szValue[ 256 ];

public:
	CNMGetSessionValueFunc( void );
	CNMGetSessionValueFunc( LPCTSTR szName );

	DECLARE_SERIALIZABLE
};

class CNMGetGameListFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( GetGameList )

	// call data
	// NONE

	// return data
	CNMGameListData		glGameList;

public:
	CNMGetGameListFunc( void );

	DECLARE_SERIALIZABLE
};

class CNMOpenGameSummaryFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( OpenGameSummary )

	// call data
	// NONE

	// return data
	// NONE

public:
	CNMOpenGameSummaryFunc( void );

	DECLARE_SERIALIZABLE
};

class CNMGetUserInfoFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( GetUserInfo )

	// call data
	TCHAR				szTargetID[ LOGINID_SIZE ];
	_UInt32_			uGameCode;

	// return data
	// NONE

public:
	CNMGetUserInfoFunc( void );
	CNMGetUserInfoFunc( LPCTSTR szTargetID, _UInt32_ uGameCode );

	DECLARE_SERIALIZABLE
};

class CNMFindUserFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( FindUser )

	// call data
	CNMFindUserRequestInfo	
						furiRequestInfo;

	// return data
	_UInt32_			uToken;

public:
	CNMFindUserFunc( void );
	CNMFindUserFunc( CNMFindUserRequestInfo& furiRequestInfo );

	DECLARE_SERIALIZABLE
};

class CNMGetFindUserResultFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( GetFindUserResult )

	// call data
	_UInt32_			uToken;

	// return data
	NMFindUserInfoList	aFoundUser;

public:
	CNMGetFindUserResultFunc( void );
	CNMGetFindUserResultFunc( _UInt32_ uToken );

	DECLARE_SERIALIZABLE
};

class CNMSendNoteExMsgFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( SendNoteExMsg )

	// call data
	CNMNoteInfoEx		nieNoteEx;

	// return data
	// NONE

public:
	CNMSendNoteExMsgFunc( void );
	CNMSendNoteExMsgFunc( CNMNoteInfoEx::NMNOTEEXTYPE uNoteExType, LPCTSTR szMessage );

	DECLARE_SERIALIZABLE
};

class CNMSendReportMsgFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( SendReportMsg )

	// call data
	CNMReportInfo		riReportInfo;

	// return data
	// NONE

public:
	CNMSendReportMsgFunc( void );
	CNMSendReportMsgFunc( CNMReportInfo::NMREPORTTYPE uType, CNMReportInfo::NMREPORTSUBTYPE uSubType, LPCTSTR szTargetNickname, LPCTSTR szReason, LPCTSTR szEvidence, _UInt32_ uGameCode, _UInt32_ uAppliedGameCode, LPCTSTR szReporterNickname );

	DECLARE_SERIALIZABLE
};

class CNMRequestChatSessionFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( RequestChatSession )

	// call data
	NMFriendCode		fcFriendCode;
	_UInt32_			uServingProcessID;

	// return data
	_UInt32_			uExistingSerialKey;
	_UInt32_			uCreatedSerialKey;

public:
	CNMRequestChatSessionFunc( void );
	CNMRequestChatSessionFunc( NMFriendCode fcFriendCode, _UInt32_ uServingProcessID );

	DECLARE_SERIALIZABLE
};

class CNMRequestMultiChatSessionFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( RequestMultiChatSession )

	// call data
	NMVirtualKey		keyVirtuality;
	NMFriendCodeList	aFriendCodes;
	_UInt32_			uServingProcessID;

	// return data
	_UInt32_			uCreatedSerialKey;

public:
	CNMRequestMultiChatSessionFunc( void );
	CNMRequestMultiChatSessionFunc( NMVirtualKey keyVirtuality, NMFriendCodeList& aFriendCodeList, _UInt32_ uServingProcessID );

	DECLARE_SERIALIZABLE
};

class CNMRequestFileUploadSessionFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( RequestFileUploadSession )

	// call data
	NMFriendCode		fcFriendCode;
	_UInt32_			uServingProcessID;
	NMFilePathList		aFilePath;

	// return data
	_UInt32_			uExistingSerialKey;
	_UInt32_			uCreatedSerialKey;

public:
	CNMRequestFileUploadSessionFunc( void );
	CNMRequestFileUploadSessionFunc( NMFriendCode fcFriendCode, _UInt32_ uServingProcessID, NMFilePathList aFilePath );

	DECLARE_SERIALIZABLE
};

class CNMRequestWBSessionFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( RequestWBSession )

	// call data
	NMFriendCode		fcFriendCode;
	_UInt32_			uAssocSerialKey;
	_UInt32_			uServingProcessID;

	// return data
	_UInt32_			uExistingSerialKey;
	_UInt32_			uCreatedSerialKey;

public:
	CNMRequestWBSessionFunc( void );
	CNMRequestWBSessionFunc( NMFriendCode fcFriendCode, _UInt32_ uAssocSerialKey, _UInt32_ uServingProcessID );

	DECLARE_SERIALIZABLE
};

class CNMRequestChatSessionExFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( RequestChatSessionEx )

	// call data
	NMFriendKey			keyFriend;
	_UInt32_			uServingProcessID;

	// return data
	_UInt32_			uExistingSerialKey;
	_UInt32_			uCreatedSerialKey;

public:
	CNMRequestChatSessionExFunc( void );
	CNMRequestChatSessionExFunc( NMFriendKey keyFriend, _UInt32_ uServingProcessID );

	DECLARE_SERIALIZABLE
};

class CNMRequestMultiChatSessionExFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( RequestMultiChatSessionEx )

	// call data
	NMVirtualKey		keyVirtuality;
	NMFriendKeyList		aFriendKey;
	_UInt32_			uServingProcessID;

	// return data
	_UInt32_			uCreatedSerialKey;

public:
	CNMRequestMultiChatSessionExFunc( void );
	CNMRequestMultiChatSessionExFunc( NMVirtualKey keyVirtuality, NMFriendKeyList& aFriendKey, _UInt32_ uServingProcessID );

	DECLARE_SERIALIZABLE
};

class CNMRequestFileUploadSessionExFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( RequestFileUploadSessionEx )

	// call data
	NMFriendKey			keyFriend;
	_UInt32_			uServingProcessID;
	NMFilePathList		aFilePath;

	// return data
	_UInt32_			uExistingSerialKey;
	_UInt32_			uCreatedSerialKey;

public:
	CNMRequestFileUploadSessionExFunc( void );
	CNMRequestFileUploadSessionExFunc( NMFriendKey keyFriend, _UInt32_ uServingProcessID, NMFilePathList aFilePath );

	DECLARE_SERIALIZABLE
};

class CNMRequestWBSessionExFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( RequestWBSessionEx )

	// call data
	NMFriendKey			keyFriend;
	_UInt32_			uAssocSerialKey;
	_UInt32_			uServingProcessID;

	// return data
	_UInt32_			uExistingSerialKey;
	_UInt32_			uCreatedSerialKey;

public:
	CNMRequestWBSessionExFunc( void );
	CNMRequestWBSessionExFunc( NMFriendKey keyFriend, _UInt32_ uAssocSerialKey, _UInt32_ uServingProcessID );

	DECLARE_SERIALIZABLE
};

class CNMReplySessionRequestFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( ReplySessionRequest )

	// call data
	_UInt64_			uSessionID;
	_UInt32_			uServingProcessID;
	BOOL				bAccept;

	// return data
	// NONE

public:
	CNMReplySessionRequestFunc( void );
	CNMReplySessionRequestFunc( _UInt64_ uSessionID, BOOL bAccept );

	DECLARE_SERIALIZABLE
};

class CNMCreateChatRoomFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( CreateChatRoom )

	// call data
	CNMChatRoomInfo		criRoomInfo;
	CNMChatRoomMemberInfo	
						crmiMyInfo;
	TCHAR				szServerAddr[ 32 ];
	_UInt16_			uServerPort;

	// return data
	// NONE

public:
	CNMCreateChatRoomFunc( void );
	CNMCreateChatRoomFunc( _UInt32_ uRoomType, _UInt32_ uClubSerialNo, LPCTSTR szClubTitle, LPCTSTR szRoomTitle, _UInt32_ uLimitLevel, _UInt32_ uMaxSize, _UInt32_ uFlag, LPCTSTR szPassword, LPCTSTR szServerAddr, _UInt16_ uServerPort );

	DECLARE_SERIALIZABLE
};

class CNMCreateChatRoomExFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( CreateChatRoomEx )

	// call data
	CNMChatRoomInfoEx	criRoomInfo;
	CNMChatRoomMemberInfo	
						crmiMyInfo;
	TCHAR				szServerAddr[ 32 ];
	_UInt16_			uServerPort;
	UINT32				uVirtualIdCode;

	// return data
	// NONE

public:
	CNMCreateChatRoomExFunc( void );
	CNMCreateChatRoomExFunc( _UInt32_ uRoomType, NMGuildKey uGuildKey, LPCTSTR szGuildTitle, LPCTSTR szRoomTitle, _UInt32_ uLimitLevel, _UInt32_ uMaxSize, _UInt32_ uFlag, LPCTSTR szPassword, LPCTSTR szServerAddr, _UInt16_ uServerPort, UINT32 uVirtualIdCode = 1 );

	DECLARE_SERIALIZABLE
};

class CNMJoinToChatRoomFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( JoinToChatRoom )

	// call data
	CNMChatRoomInfo		criRoomInfo;
	CNMChatRoomMemberInfo	
						crmiMyInfo;
	TCHAR				szServerAddr[ 32 ];
	_UInt16_			uServerPort;

	// return data
	_UInt32_			uExistingSerialKey;
	_UInt32_			uCreatedSerialKey;

public:
	CNMJoinToChatRoomFunc( void );
	CNMJoinToChatRoomFunc( _UInt32_ uClubSerialNo, _UInt32_ uRoomSerialNo, LPCTSTR szPassword, LPCTSTR szServerAddr, _UInt16_ uServerPort );

	DECLARE_SERIALIZABLE
};

class CNMJoinToChatRoomExFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( JoinToChatRoomEx )

	// call data
	CNMChatRoomInfoEx	criRoomInfo;
	CNMChatRoomMemberInfo	
						crmiMyInfo;
	TCHAR				szServerAddr[ 32 ];
	_UInt16_			uServerPort;
	UINT32				uVirtualIdCode;

	// return data
	_UInt32_			uExistingSerialKey;
	_UInt32_			uCreatedSerialKey;

public:
	CNMJoinToChatRoomExFunc( void );
	CNMJoinToChatRoomExFunc( NMGuildKey uGuildKey, _UInt32_ uRoomSerialNo, LPCTSTR szPassword, LPCTSTR szServerAddr, _UInt16_ uServerPort, _UInt32_ uFlag, UINT32 uVirtualIdCode );

	DECLARE_SERIALIZABLE
};

class CNMRequestSessionToOtherFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( RequestSessionToOther )

	// call data
	NMSESSIONTYPE		uSessionType;
	_UInt32_			uInviteMode;
	_UInt32_			uServingProcessID;
	NMVirtualKey		keyVirtuality;

	TCHAR				szRequestID[ LOGINID_SIZE ];
	_UInt32_			uRequesteeGameCode;

	NMFilePathList		aFilePath;

	_UInt32_			uAssocSerialKey;

	// return data
	_UInt32_			uExistingSerialKey;
	_UInt32_			uCreatedSerialKey;

public:
	CNMRequestSessionToOtherFunc( void );
	CNMRequestSessionToOtherFunc( NMSESSIONTYPE uSessionType, _UInt32_ uInviteMode, NMVirtualKey keyVirtuality, LPCTSTR szRequestID, _UInt32_ uRequesteeGameCode, NMFilePathList& aFileList );
	CNMRequestSessionToOtherFunc( NMSESSIONTYPE uSessionType, _UInt32_ uInviteMode, NMVirtualKey keyVirtuality, LPCTSTR szRequestID, _UInt32_ uRequesteeGameCode, _UInt32_ uAssocSerialKey = 0 );

	DECLARE_SERIALIZABLE
};

//-----------------------------------------------------------------------------
//	CreateChannel
//-----------------------------------------------------------------------------

class CNMCreateChannelFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( CreateChannel )

public:
	// call data

	// return data

public:
	CNMCreateChannelFunc();

	DECLARE_SERIALIZABLE
};

//-----------------------------------------------------------------------------
//	JoinChannel
//-----------------------------------------------------------------------------

class CNMJoinChannelFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( JoinChannel )

public:
	// call data
	UINT32							uRequestKey1;
	UINT32							uRequestKey2;
	StringZip< NXPASSPORT_SIZE >	szPassport;
	StringZip< 256 >				szServerAddr;
	UINT16							uServerPort;
	CNMClientChannelInfo			clientChannelInfo;
	CNMChannelMemberInfo			channelMemberInfo;
	BOOL							bMonitorGuildOnlineInfo;

	// return data
	UINT32							uExistingSerialKey;
	UINT32							uCreatedSerialKey;

public:
	CNMJoinChannelFunc();

	DECLARE_SERIALIZABLE
};

class CNMCRRegisterCallbackFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( CRRegisterCallback )

	// call data
	HWND				hWnd;
	_UInt32_			uMsg;

	// return data
	// NONE

public:
	CNMCRRegisterCallbackFunc( void );
	CNMCRRegisterCallbackFunc( _UInt32_ uSerialKey, HWND hWnd, _UInt32_ uMsg );

	DECLARE_SERIALIZABLE
};

class CNMCRCloseFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( CRClose )

	// call data
	// NONE

	// return data
	// NONE

public:
	CNMCRCloseFunc( void );
	CNMCRCloseFunc( _UInt32_ uSerialKey );

	DECLARE_SERIALIZABLE
};

class CNMCRGetRoomInfoFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( CRGetRoomInfo )

	// call data
	// NONE

	// return data
	CNMChatRoomInfo		criInfo;

public:
	CNMCRGetRoomInfoFunc( void );
	CNMCRGetRoomInfoFunc( _UInt32_ uSerialKey );

	DECLARE_SERIALIZABLE
};

class CNMCRGetRoomInfoExFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( CRGetRoomInfoEx )

	// call data
	// NONE

	// return data
	CNMChatRoomInfoEx		criInfo;

public:
	CNMCRGetRoomInfoExFunc( void );
	CNMCRGetRoomInfoExFunc( _UInt32_ uSerialKey );

	DECLARE_SERIALIZABLE
};

class CNMCRChangeRoomInfoFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( CRChangeRoomInfo )

	enum
	{
		kFlag_RoomTitle		= ( 0x1 << 0 ),
		kFlag_LimitLevel	= ( 0x1 << 1 ),
		kFlag_MaxSize		= ( 0x1 << 2 ),
		kFlag_Flag			= ( 0x1 << 3 ),
		kFlag_OwnerIdCode	= ( 0x1 << 4 ),
		kFlag_Password		= ( 0x1 << 5 ),
		kFlag_All			= 0xFFFFFFFF,
	};

	// call data
	_UInt32_			uChangeFlag;
	TCHAR				szRoomTitle[ ROOMTITLE_SIZE ];
	_UInt32_			uLimitLevel;
	_UInt32_			uMaxSize;
	_UInt32_			uFlag;
	_UInt64_			uOwnerIdCode;
	TCHAR				szPassword[ ROOMPWD_SIZE ];

	// return data
	// NONE

public:
	CNMCRChangeRoomInfoFunc( void );
	CNMCRChangeRoomInfoFunc( _UInt32_ uSerialKey, LPCTSTR szRoomTitle, _UInt32_* pLimitLevel, _UInt32_* pMaxSize, _UInt32_* pFlag, LPCTSTR szPassword, _UInt64_* pOwnerIdCode );

	DECLARE_SERIALIZABLE
};

class CNMCRChangeMyInfoFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( CRChangeMyInfo )

	// call data
	CNMChatRoomMemberInfo	
						crmiInfo;

	// return data
	// NONE

public:
	CNMCRChangeMyInfoFunc( void );
	CNMCRChangeMyInfoFunc( _UInt32_ uSerialKey, CNMChatRoomMemberInfo& crmiInfo );

	DECLARE_SERIALIZABLE
};

class CNMCRGetMemberListFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( CRGetMemberList )

	// call data
	// NONE

	// return data
	NMChatRoomMemberList	
						aMembers;

public:
	CNMCRGetMemberListFunc( void );
	CNMCRGetMemberListFunc( _UInt32_ uSerialKey );

	DECLARE_SERIALIZABLE
};

class CNMCRInviteUserFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( CRInviteUser )

	// call data
	TCHAR				szToId[ 1024 ];
	TCHAR				szMessage[ 1024 ];

	// return data
	// NONE

public:
	CNMCRInviteUserFunc( void );
	CNMCRInviteUserFunc( _UInt32_ uSerialKey, LPCTSTR szToId, LPCTSTR szMessage );

	DECLARE_SERIALIZABLE
};

class CNMCRBanUserFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( CRBanUser )

	// call data
	_UInt64_			uIdCode;

	// return data
	// NONE

public:
	CNMCRBanUserFunc( void );
	CNMCRBanUserFunc( _UInt32_ uSerialKey, _UInt64_ uIdCode );

	DECLARE_SERIALIZABLE
};


class CNMCRSendChatMessageFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( CRSendChatMessage )

	// call data
	_UInt64_			aToIdCode[ 10 ];
	CNMChatMessage		chatMsg;

	// return data
	// NONE

public:
	CNMCRSendChatMessageFunc( void );
	CNMCRSendChatMessageFunc( _UInt32_ uSerialKey, _UInt64_ aToIdCode[ 10 ], CNMChatMessage& chatMsg );

	DECLARE_SERIALIZABLE
};

class CNMCRGetMemberInfoFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( CRGetMemberInfo )

	enum
	{
		kType_NULL			= 0,
		kType_FirstMember	= 1,
		kType_MySelf		= 2,
		kType_Joined		= 3,
		kType_Banned		= 4,
		kType_Leaved		= 5,
		kType_InfoChanged	= 6,
	};

	// call data
	_UInt32_			uType;
	// NONE

	// return data
	CNMChatRoomMemberInfo	
						crmiInfo;

public:
	CNMCRGetMemberInfoFunc( void );
	CNMCRGetMemberInfoFunc( _UInt32_ uSerialKey, _UInt32_ uType );

	DECLARE_SERIALIZABLE
};

class CNMGSRegisterCallbackFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( GSRegisterCallback )

	// call data
	HWND				hWnd;
	_UInt32_			uMsg;

	// return data
	// NONE

public:
	CNMGSRegisterCallbackFunc( void );
	CNMGSRegisterCallbackFunc( _UInt32_ uSerialKey, HWND hWnd, _UInt32_ uMsg );

	DECLARE_SERIALIZABLE
};

class CNMGSWantCloseFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( GSWantClose )

	// call data
	BOOL				bForceClose;
	// NONE

	// return data
	// NONE

public:
	CNMGSWantCloseFunc( void );
	CNMGSWantCloseFunc( _UInt32_ uSerialKey, BOOL bForceClose );

	DECLARE_SERIALIZABLE
};

class CNMGSGetSessionInfoFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( GSGetSessionInfo )

	// call data
	// NONE

	// return data
	CNMSessionInfo		siInfo;
	NMSessionMemberList	aMembers;

public:
	CNMGSGetSessionInfoFunc( void );
	CNMGSGetSessionInfoFunc( _UInt32_ uSerialKey );

	DECLARE_SERIALIZABLE
};

class CNMGSSetServingProcessIDFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( GSSetServingProcessID )

	// call data
	_UInt32_			uServingProcessID;

	// return data
	// NONE

public:
	CNMGSSetServingProcessIDFunc( void );
	CNMGSSetServingProcessIDFunc( _UInt32_ uSerialKey, _UInt32_ uServingProcessID );

	DECLARE_SERIALIZABLE
};

class CNMGSGetMemberListFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( GSGetMemberList )

	// call data
	// NONE

	// return data
	NMSessionMemberList	aMembers;

public:
	CNMGSGetMemberListFunc( void );
	CNMGSGetMemberListFunc( _UInt32_ uSerialKey );

	DECLARE_SERIALIZABLE
};

class CNMGSGetInviteCandidateListFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( GSGetInviteCandidateList )

	// call data
	// NONE

	// return data
	NMFriendList		aCandidates;

public:
	CNMGSGetInviteCandidateListFunc( void );
	CNMGSGetInviteCandidateListFunc( _UInt32_ uSerialKey );

	DECLARE_SERIALIZABLE
};

class CNMGSInviteUserFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( GSInviteUser )

	// call data
	NMFriendCode		fcFriendCode;

	// return data
	// NONE

public:
	CNMGSInviteUserFunc( void );
	CNMGSInviteUserFunc( _UInt32_ uSerialKey, NMFriendCode fcFriendCode );

	DECLARE_SERIALIZABLE
};

class CNMGSInviteUserExFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( GSInviteUserEx )

	// call data
	NMFriendKey			keyFriend;

	// return data
	// NONE

public:
	CNMGSInviteUserExFunc( void );
	CNMGSInviteUserExFunc( _UInt32_ uSerialKey, NMFriendKey keyFriend );

	DECLARE_SERIALIZABLE
};

class CNMCSSendChatMessageFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( CSSendChatMessage )

	// call data
	CNMChatMessage		chatMsg;

	// return data
	// NONE

public:
	CNMCSSendChatMessageFunc( void );
	CNMCSSendChatMessageFunc( _UInt32_ uSerialKey, CNMChatMessage& chatMsg );

	DECLARE_SERIALIZABLE
};

class CNMFUDSGetFileEventFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( FUDSGetFileEvent )

	// call data
	// NONE

	// return data
	NMFileEventList		aEventList;

public:
	CNMFUDSGetFileEventFunc( void );
	CNMFUDSGetFileEventFunc( _UInt32_ uSerialKey );

	DECLARE_SERIALIZABLE
};


class CNMFUSSendFileFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( FUSSendFile )

	// call data
	TCHAR				szFilePath[ MAX_PATH ];

	// return data
	_UInt32_			uEventId;

public:
	CNMFUSSendFileFunc( void );
	CNMFUSSendFileFunc( _UInt32_ uSerialKey, LPCTSTR szFilePath );

	DECLARE_SERIALIZABLE
};

class CNMFUSControlFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( FUSControl )

	enum
	{
		kType_NULL			= 0,
		kType_SendFileAgain	= 1,
		kType_Cancel		= 2,
		kType_Remove		= 3,
	};

	// call data
	_UInt32_			uType;
	_UInt32_			uEventId;

	// return data
	// NONE

public:
	CNMFUSControlFunc( void );
	CNMFUSControlFunc( _UInt32_ uSerialKey, _UInt32_ uType, _UInt32_ uEventId );

	DECLARE_SERIALIZABLE
};

class CNMFDSControlFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( FDSControl )

	enum
	{
		kType_NULL		= 0,
		kType_GetFile	= 1,
		kType_Cancel	= 2,
		kType_Remove	= 3,
	};

	// call data
	_UInt32_			uType;
	_UInt32_			uEventId;

	// return data
	// NONE

public:
	CNMFDSControlFunc( void );
	CNMFDSControlFunc( _UInt32_ uSerialKey, _UInt32_ uType, _UInt32_ uEventId );

	DECLARE_SERIALIZABLE
};

class CNMFDSGetDownloadDirFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( FDSGetDownloadDir )

	// call data
	// NONE

	// return data
	TCHAR				szDir[ MAX_PATH ];

public:
	CNMFDSGetDownloadDirFunc( void );
	CNMFDSGetDownloadDirFunc( _UInt32_ uSerialKey );

	DECLARE_SERIALIZABLE
};

class CNMFDSSetDownloadDirFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( FDSSetDownloadDir )

	// call data
	TCHAR				szDir[ MAX_PATH ];

	// return data
	// NONE

public:
	CNMFDSSetDownloadDirFunc( void );
	CNMFDSSetDownloadDirFunc( _UInt32_ uSerialKey, LPCTSTR szDir );

	DECLARE_SERIALIZABLE
};

class CNMWSSendWBMessageFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( WSSendWBMessage )

	enum
	{
		kMaxDataLen	 = 32768,
	};

	// call data
	_UInt32_			uContentId;
	_UInt32_			uDataLen;
	BYTE				aData[ kMaxDataLen ];

	// return data
	// NONE

public:
	CNMWSSendWBMessageFunc( void );
	CNMWSSendWBMessageFunc( _UInt32_ uSerialKey, _UInt32_ uContentId, const BYTE* aData, _UInt32_ uDataLen );

	DECLARE_SERIALIZABLE
};

class CNMWSGetAssocSerialKeyFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( WSGetAssocSerialKey )

	// call data

	// return data
	_UInt32_			uAssocSerialKey;

public:
	CNMWSGetAssocSerialKeyFunc( void );
	CNMWSGetAssocSerialKeyFunc( _UInt32_ uSerialKey );

	DECLARE_SERIALIZABLE
};

class CNMCustomNotifyFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( CustomNotify )

	// call data
	CNMCustomNotifyData	cnData;

	// return data
	// NONE

public:
	CNMCustomNotifyFunc( void );
	CNMCustomNotifyFunc( UINT32 uType, LPCTSTR szArgument, UINT32 uPolicyFlag = CNMCustomNotifyData::kCustomNotifyPolicy_Default);
	CNMCustomNotifyFunc( CNMCustomNotifyData& cnData );

	DECLARE_SERIALIZABLE
};

class CNMChangeMyLevelFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( ChangeMyLevel )

	// call data
	NMVirtualKey		keyVirtual;
	_UInt32_			uLevel;
	UINT32				uLevelType;

	// return data
	// NONE

public:
	CNMChangeMyLevelFunc( void );
	CNMChangeMyLevelFunc( _UInt32_ uLevel, UINT32 uLevelType );

	DECLARE_SERIALIZABLE
};

class CNMRemoveMyVirtualUserFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( RemoveMyVirtualUser )

	//	call data
	NMVirtualKey		keyVirtual;

	//	return data
	//	NONE

public:
	CNMRemoveMyVirtualUserFunc( void );
	CNMRemoveMyVirtualUserFunc( NMVirtualKey & keyVirtual );

	DECLARE_SERIALIZABLE
};

class CNMLoginPassportFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( LoginPassport )

	// call data
	TCHAR				szLoginId[ LOGINID_SIZE ];
	TCHAR				szPassword[ LOGINID_SIZE ];
	_UInt32_			uGameCode;
	_UInt64_			uVersion;

	PBYTE				pbyPassport[ PASSPORT_SIZE ];
	INT32				cbyPassportLength;

	NMGameCode			uLoginGameCode;

	//
	//	메신저서버로부터 지원받을 기능 플래그 설정
	//
	UINT32				uSupportFlag;
	INT32				nRegionCode;

	// return data
	// NONE

public:
	CNMLoginPassportFunc( void );
	CNMLoginPassportFunc( LPCTSTR szLoginId, LPCTSTR szPassword, _UInt64_ uVersion, PBYTE pbyPassport, INT32 cbyPassportLength, NMGameCode uLoginGameCode, INT32 nRegionCode );
	CNMLoginPassportFunc( LPCTSTR szLoginId, LPCTSTR szPassword, PBYTE pbyPassport, INT32 cbyPassportLength, UINT32 uSupportFlag, INT32 nRegionCode );

	DECLARE_SERIALIZABLE
};

class CNMLoginNexonPassportFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( LoginNexonPassport )

	// call data
	TCHAR				szPassport[ PASSPORT_SIZE ];
	_UInt32_			uGameCode;
	_UInt64_			uVersion;

	NMGameCode			uLoginGameCode;

public:
	CNMLoginNexonPassportFunc( void );
	CNMLoginNexonPassportFunc( LPCTSTR szPassport, _UInt64_ uVersion, NMGameCode uLoginGameCode = kGameCode_nexoncom );

	DECLARE_SERIALIZABLE
};

class CNMLoginMessengerFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( LoginMessenger )

	// call data
	UINT32				uSupportFlag;					//	= kSupportFlag_FromGameCode

	// return data
	// NONE

public:
	CNMLoginMessengerFunc();
	CNMLoginMessengerFunc( UINT32 uSupportFlag );

	DECLARE_SERIALIZABLE
};

class CNMLogoutMessengerFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( LogoutMessenger )

	// call data
	// NONE

	// return data
	// NONE

public:
	CNMLogoutMessengerFunc();

	DECLARE_SERIALIZABLE
};

class CNMLoginAuthFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( LoginAuth )

	enum LoginType
	{
		kLoginType_None,
		kLoginType_Passport,
		kLoginType_IDPassword,
	};

public:
	// call data
	UINT32		uLoginType;
    TCHAR		szPassport[ PASSPORT_SIZE ];
	TCHAR		szNexonID[ LARGE_LOGINID_SIZE ];
	TCHAR		szPassword[ PASSWORD_SIZE ];
	BOOL		bSecondary;
	INT32		nRegionCode;
	TCHAR		szExtraData[ EXTRADATA_SIZE ];
	UINT32		uChannelCode;
	INT32		nSubErrCode;

	// return data
	INT32		nErrorCode;

public:
	CNMLoginAuthFunc();
	CNMLoginAuthFunc( LPCTSTR pszPassport, INT32 nRegionCode );
	CNMLoginAuthFunc( LPCTSTR pszNexonID, LPCTSTR pszPassword, INT32 nRegionCode );
	CNMLoginAuthFunc( LPCTSTR pszNexonID, LPCTSTR pszPassword, INT32 nRegionCode, LPCTSTR pszExtraData, UINT32 uChannelCode );

	DECLARE_SERIALIZABLE
};

class CNMLoginAuthSecurityCNFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( SecurityAuth )

public:
	// call data
	StringZip< PASSPORT_SIZE >		szPassport;
	StringZip< SECURITYCN_SIZE >	szSecurityData;

	// return data
	INT32		nErrorCode;

public:
	CNMLoginAuthSecurityCNFunc();
	CNMLoginAuthSecurityCNFunc( LPCTSTR pszPassport, LPCTSTR pszSecurityData );

	DECLARE_SERIALIZABLE
};

class CNMGetGameTokenFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( GetGameToken )

public:
	// call data
	StringZip< SECUREDATA_SIZE >		szSecureData;

	// return data
	INT32		nReplyCode;
	INT32		nSecureCode;

public:
	CNMGetGameTokenFunc();
	CNMGetGameTokenFunc( LPCTSTR pszSecureData );

	DECLARE_SERIALIZABLE
};

class CNMLogoutAuthFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( LogoutAuth )

public:
	// call data
	BOOL		bApplyToAllSession;
	BOOL		bSecondary;

	// return data
	// NONE

public:
	CNMLogoutAuthFunc();
	CNMLogoutAuthFunc( BOOL bApplyToAllSession, BOOL bSecondary );
	
	DECLARE_SERIALIZABLE
};

class CNMInitializeFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( Initialize )

public:
	// call data
	NMGameCode	uGameCode;
	UINT32		uSupportFlag;

	// return data
	// NONE

public:
	CNMInitializeFunc();
	CNMInitializeFunc( NMGameCode uGameCode, UINT32 uSupportFlag );

	DECLARE_SERIALIZABLE
};

class CNMFinalizeFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( Finalize )

public:
	// NONE

public:
	CNMFinalizeFunc();

	DECLARE_SERIALIZABLE
};

///////////////////////////////////////////////////////////////////////////////
//
//	Character functions
//
///////////////////////////////////////////////////////////////////////////////

class CNMCharacterLoginFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( CharacterLogin )

	// call data
	TCHAR				szNickname[ NICKNAME_SIZE ];
	NMVirtualKey		keyUser;
	INT32				nServerCode;

	// return data
	// NONE

public:
	CNMCharacterLoginFunc( void );
	CNMCharacterLoginFunc( LPCTSTR szNickname, _UInt32_ uVirtualIdCode, INT32 nServerCode );

	DECLARE_SERIALIZABLE
};

class CNMCharacterLogoutFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( CharacterLogout )

	// call data
	TCHAR				szNickname[ NICKNAME_SIZE ];
	NMVirtualKey		keyUser;
	INT32				nServerCode;

	// return data
	// NONE

public:
	CNMCharacterLogoutFunc( void );
	CNMCharacterLogoutFunc( LPCTSTR szNickname, _UInt32_ uVirtualIdCode, INT32 nServerCode );

	DECLARE_SERIALIZABLE
};

class CNMCharacterRemoveFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( CharacterRemove )

	//	call data
	NMVirtualKey		keyVirtual;
	TCHAR				szNickname[ NICKNAME_SIZE ];
	INT32				nServerCode;

	//	return data
	//	NONE

public:
	CNMCharacterRemoveFunc( void );
	CNMCharacterRemoveFunc( NMVirtualKey keyVirtual, LPCTSTR szNickname, INT32 nServerCode );

	DECLARE_SERIALIZABLE
};

class CNMCharacterChangeNameFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( CharacterChangeName )

	//	call data
	NMVirtualKey	keyVirtual;
	TCHAR			szNickname[ NICKNAME_SIZE ];
	INT32			nServerCode;

	//	return data
	//	nothing...

public:
	CNMCharacterChangeNameFunc( void );
	CNMCharacterChangeNameFunc( NMVirtualKey keyVirtual, LPCTSTR szNickname, INT32 nServerCode );

	DECLARE_SERIALIZABLE
};

class CNMCharacterSyncFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( CharacterSync )

	// call data
	TCHAR				szNickname[ NICKNAME_SIZE ];
	NMVirtualKey		keyUser;
	INT32				nServerCode;
	INT32				nSyncType;

	// return data
	// NONE

public:
	CNMCharacterSyncFunc( void );
	CNMCharacterSyncFunc( LPCTSTR szNickname, _UInt32_ uVirtualIdCode, INT32 nServerCode, INT32 nSyncType = 0);

	DECLARE_SERIALIZABLE
};

class CNMCharacterLogin64Func : public CNMFunc
{
public:
	DECLARE_NMFUNC( CharacterLogin64 )

	// call data
	TCHAR				szNickname[ NICKNAME_SIZE ];
	UINT64				uVirtualIDCode;
	INT32				nServerCode;

	// return data
	// NONE

public:
	CNMCharacterLogin64Func( void );
	CNMCharacterLogin64Func( LPCTSTR szNickname, _UInt64_ uVirtualIdCode, INT32 nServerCode );

	DECLARE_SERIALIZABLE
};

class CNMCharacterLogout64Func : public CNMFunc
{
public:
	DECLARE_NMFUNC( CharacterLogout64 )

	// call data
	TCHAR				szNickname[ NICKNAME_SIZE ];
	UINT64				uVirtualIDCode;
	INT32				nServerCode;

	// return data
	// NONE

public:
	CNMCharacterLogout64Func( void );
	CNMCharacterLogout64Func( LPCTSTR szNickname, _UInt64_ uVirtualIdCode, INT32 nServerCode );

	DECLARE_SERIALIZABLE
};

class CNMCharacterRemove64Func : public CNMFunc
{
public:
	DECLARE_NMFUNC( CharacterRemove64 )

	//	call data
	//NMVirtualKey		keyVirtual;
	TCHAR				szNickname[ NICKNAME_SIZE ];
	UINT64				uVirtualIDCode;
	INT32				nServerCode;

	//	return data
	//	NONE

public:
	CNMCharacterRemove64Func( void );
	CNMCharacterRemove64Func( LPCTSTR szNickname, _UInt64_ uVirtualIdCode, INT32 nServerCode );

	DECLARE_SERIALIZABLE
};

class CNMCharacterChangeName64Func : public CNMFunc
{
public:
	DECLARE_NMFUNC( CharacterChangeName64 )

	//	call data
	TCHAR			szNickname[ NICKNAME_SIZE ];
	UINT64			uVirtualIDCode;
	INT32			nServerCode;

	//	return data
	//	nothing...

public:
	CNMCharacterChangeName64Func( void );
	CNMCharacterChangeName64Func( LPCTSTR szNickname, _UInt64_ uVirtualIdCode, INT32 nServerCode );

	DECLARE_SERIALIZABLE
};

class CNMCharacterSync64Func : public CNMFunc
{
public:
	DECLARE_NMFUNC( CharacterSync64 )

	// call data
	TCHAR				szNickname[ NICKNAME_SIZE ];
	UINT64				uVirtualIDCode;
	INT32				nServerCode;
	INT32				nSyncType;

	// return data
	// NONE

public:
	CNMCharacterSync64Func( void );
	CNMCharacterSync64Func( LPCTSTR szNickname, _UInt64_ uVirtualIdCode, INT32 nServerCode, INT32 nSyncType = 0);

	DECLARE_SERIALIZABLE
};

///////////////////////////////////////////////////////////////////////////////
//
//	Channel functions
//
///////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
//	CHClose
//-----------------------------------------------------------------------------

class CNMCHCloseFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( CHClose )

public:
	// call data
	// NONE

	// return data
	// NONE

public:
	CNMCHCloseFunc();
	CNMCHCloseFunc( UINT32 uSerialKey );

	DECLARE_SERIALIZABLE
};

//-----------------------------------------------------------------------------
//	CHGetChannelInfo
//-----------------------------------------------------------------------------

class CNMCHGetChannelInfoFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( CHGetChannelInfo )

public:
	// call data
	// NONE

	// return data
	CNMClientChannelInfo clientChannelInfo;

public:
	CNMCHGetChannelInfoFunc();
	CNMCHGetChannelInfoFunc( UINT32 uSerialKey );

	DECLARE_SERIALIZABLE
};

//-----------------------------------------------------------------------------
//	CHChangeChannelInfo
//-----------------------------------------------------------------------------

class CNMCHChangeChannelInfoFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( CHChangeChannelInfo )

public:
	// call data
	CNMClientChannelInfo clientChannelInfo;

	// return data
	// NONE

public:
	CNMCHChangeChannelInfoFunc();
	CNMCHChangeChannelInfoFunc( UINT32 uSerialKey, CNMClientChannelInfo const & clientChannelInfo );

	DECLARE_SERIALIZABLE
};

//-----------------------------------------------------------------------------
//	CHGetMemberInfoList
//-----------------------------------------------------------------------------

class CNMCHGetMemberInfoListFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( CHGetMemberInfoList )

public:
	// call data
	// NONE

	// return data
	NMChannelMemberInfoList aMember;

public:
	CNMCHGetMemberInfoListFunc();
	CNMCHGetMemberInfoListFunc( UINT32 uSerialKey );

	DECLARE_SERIALIZABLE
};

//-----------------------------------------------------------------------------
//	CHChangeMyInfo
//-----------------------------------------------------------------------------

class CNMCHChangeMyInfoFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( CHChangeMyInfo )

public:
	// call data
	CNMChannelMemberInfo	channelMemberInfo;

	// return data
	// NONE

public:
	CNMCHChangeMyInfoFunc();
	CNMCHChangeMyInfoFunc( UINT32 uSerialKey );
	CNMCHChangeMyInfoFunc( UINT32 uSerialKey, CNMChannelMemberInfo const & channelMemberInfo );

	DECLARE_SERIALIZABLE
};

//-----------------------------------------------------------------------------
//	CHInviteUser
//-----------------------------------------------------------------------------

class CNMCHInviteUserFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( CHInviteUser )

public:
	// call data
	StringZip< TARGETID_SIZE >	szTargetIds;
	StringZip< NOTEMSG_SIZE >	szMessage;

	// return data
	// NONE

public:
	CNMCHInviteUserFunc();
	CNMCHInviteUserFunc( UINT32 uSerialKey, LPCTSTR pszTargetIds, LPCTSTR pszMessage );

	DECLARE_SERIALIZABLE
};

//-----------------------------------------------------------------------------
//	CHBanUser
//-----------------------------------------------------------------------------

class CNMCHBanUserFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( CHBanUser )

public:
	// call data
	INT32 nNexonSN;

	// return data
	// NONE

public:
	CNMCHBanUserFunc();
	CNMCHBanUserFunc( UINT32 uSerialKey, INT32 nNexonSN );

	DECLARE_SERIALIZABLE
};

//-----------------------------------------------------------------------------
//	CHSendChannelMessage
//-----------------------------------------------------------------------------

class CNMCHSendChannelMessageFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( CHSendChannelMessage )

public:
	// call data
	CNMChannelMessage channelMessage;

	// return data
	// NONE

public:
	CNMCHSendChannelMessageFunc();
	CNMCHSendChannelMessageFunc( UINT32 uSerialKey );
	CNMCHSendChannelMessageFunc( UINT32 uSerialKey, CNMChannelMessage const & channelMessage );

	DECLARE_SERIALIZABLE
};

//-----------------------------------------------------------------------------
//	CNMP2PLoginFunc
//-----------------------------------------------------------------------------

class CNMP2PLoginFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( P2PLogin )

public:
	// call data
	StringZip< NXPASSPORT_SIZE >	szNexonPassport;

	// return data
	NMP2PErrorCode					nErrorCode;

public:
	CNMP2PLoginFunc();
	CNMP2PLoginFunc( LPCTSTR pszNexonPassport );
	CNMP2PLoginFunc( NMP2PErrorCode nErrorCode );

	DECLARE_SERIALIZABLE
};

//-----------------------------------------------------------------------------
//	CNMP2PSendDataFunc
//-----------------------------------------------------------------------------

class CNMP2PSendDataFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( P2PSendData )

public:
	// call data
	CNMP2PMessage					message;
	BOOL							bCustomSerialKey;
	UINT32							ttl;

	// return data
	NMP2PErrorCode					nErrorCode;
	UINT32							uP2PSerialKey;

public:
	CNMP2PSendDataFunc();
	CNMP2PSendDataFunc( UINT32 uSerialKey, CNMP2PMessage const & message, UINT32 ttl );
	CNMP2PSendDataFunc( CNMP2PMessage const & message, UINT32 ttl );
	CNMP2PSendDataFunc( NMP2PErrorCode nErrorCode );

	DECLARE_SERIALIZABLE
};

//-----------------------------------------------------------------------------
//	CNMP2PLogoutFunc
//-----------------------------------------------------------------------------

class CNMP2PLogoutFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( P2PLogout )

public:
	// call data
	// NONE

	// return data
	NMP2PErrorCode					nErrorCode;

public:
	CNMP2PLogoutFunc();

	DECLARE_SERIALIZABLE
};

//-----------------------------------------------------------------------------
//	CNMP2PMultiSendDataFunc
//-----------------------------------------------------------------------------

class CNMP2PMultiSendDataFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( P2PMultiSendData )

public:
	// call data
	NMP2PKey						sender;
	NMP2PKey						receivers[ P2PRECEIVERS_SIZE ];
	UINT32							receiversSize;
	BYTE							data[ P2PDATA_SIZE ];
	UINT32							dataSize;
	UINT32							ttl;

	// return data
	NMP2PErrorCode					nErrorCode;
	UINT32							uP2PSerialKey;

public:
	CNMP2PMultiSendDataFunc();
	CNMP2PMultiSendDataFunc( NMP2PKey const & sender, NMP2PKey const * receivers, UINT32 receiversSize, BYTE const * data, UINT32 dataSize, UINT32 ttl );

	DECLARE_SERIALIZABLE
};


///////////////////////////////////////////////////////////////////////////////
//
//	GameLog functions
//
///////////////////////////////////////////////////////////////////////////////

class CNMGameLogInitializeFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( GameLogInitialize )

	// call data
	TCHAR				szGameName[ 32 ];
	TCHAR				szBaseURL[ 256 ];
	TCHAR				szSessionID[ 32 ];

	// return data
	// NONE

public:
	CNMGameLogInitializeFunc( void );
	CNMGameLogInitializeFunc( LPCTSTR szGameName, LPCTSTR szBaseURL, LPCTSTR szSessionID );

	DECLARE_SERIALIZABLE
};

class CNMGameLogFinalizeFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( GameLogFinalize )

	// call data
	// NONE

	// return data
	// NONE

public:
	CNMGameLogFinalizeFunc( void );

	DECLARE_SERIALIZABLE
};

class CNMGameLogWriteStageLogFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( GameLogWriteStageLog )

	// call data
	_SInt32_			nStage;
	TCHAR				szComment[ 256 ];

	// return data
	// NONE

public:
	CNMGameLogWriteStageLogFunc( void );
	CNMGameLogWriteStageLogFunc( _SInt32_ nStage, LPCTSTR szComment );

	DECLARE_SERIALIZABLE
};

class CNMGameLogWriteErrorLogFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( GameLogWriteErrorLog )

	// call data
	_SInt32_			nErrorType;
	TCHAR				szComment[ 256 ];

	// return data
	// NONE

public:
	CNMGameLogWriteErrorLogFunc( void );
	CNMGameLogWriteErrorLogFunc( _SInt32_ nErrorType, LPCTSTR szComment );

	DECLARE_SERIALIZABLE
};

class CNMGameLogGetSessionIDFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( GameLogGetSessionID )

	// call data
	// NONE

	// return data
	TCHAR				szSessionID[ 32 ];

public:
	CNMGameLogGetSessionIDFunc( void );

	DECLARE_SERIALIZABLE
};


///////////////////////////////////////////////////////////////////////////////
//
//	PlayFeed functions
//
///////////////////////////////////////////////////////////////////////////////

class CNMPlayFeedWritePlayLogFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( PlayFeedWritePlayLog )

	// call data
	UINT32	uFeedCategory;
	UINT32	uFeedTypeNo;
	TCHAR	szPlayLog[ 4096 ];

	// return data
	// NONE

public:
	CNMPlayFeedWritePlayLogFunc( void );
	CNMPlayFeedWritePlayLogFunc( UINT32 uFeedCategory, UINT32 uFeedTypeNo, LPCTSTR szPlayLog );

	DECLARE_SERIALIZABLE
};

class CNMPlayFeedPublishPlayFeedFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( PlayFeedPublishPlayFeed )

	// call data
	UINT32	uLogType;
	UINT32	uFeedCategory;
	UINT32	uFeedTypeNo;
	TCHAR	szPlayLog[ 4096 ];

	// return data
	// NONE

public:
	CNMPlayFeedPublishPlayFeedFunc( void );
	CNMPlayFeedPublishPlayFeedFunc( UINT32 uLogType, UINT32 uFeedCategory, UINT32 uFeedTypeNo, LPCTSTR szPlayLog );

	DECLARE_SERIALIZABLE
};

///////////////////////////////////////////////////////////////////////////////
//
//	P2Linker functions
//
///////////////////////////////////////////////////////////////////////////////

class CNMP2LinkerWriteChatFunc : public CNMFunc
{
public:
	DECLARE_NMFUNC( P2LinkerWriteChat )

	// call data
	NMP2ChatType	chatType;
	UINT32			uServerCode;
	INT64			nCharacterSN;
	TCHAR			szChat[ 512 ];
	TCHAR			szParam[ 512 ];

	// return data
	NMP2ChatError	nErrorCode;
	// NONE

public:
	CNMP2LinkerWriteChatFunc( void );
	CNMP2LinkerWriteChatFunc( NMP2ChatType chatType, UINT32 uServerCode, INT64 nCharacterSN, LPCTSTR szChat, LPCTSTR szParam );

	DECLARE_SERIALIZABLE
};

//=========================================
// Etc.
//=========================================

#define BEGIN_NMFUNC_MAP	switch ( pFunc->GetCode() ) {
#define END_NMFUNC_MAP		default:break; }
#define ON_NMFUNC( funcClass, funcHandler )		\
							case funcClass::kCode: pFunc->bSuccess = funcHandler( ( funcClass* )pFunc ); pFunc->SetHandled(); break;

class CNMFuncReceiver
{
public:
	CNMFuncReceiver()			{};
	virtual ~CNMFuncReceiver()	{};

	virtual BOOL	OnNMFuncCall( CNMFunc* pFunc )	{ UNREFERENCED_PARAMETER(pFunc); return FALSE; }
	inline BOOL		CallByNMFunc( CNMFunc* pFunc, BOOL bReference = FALSE )
	{
		if ( pFunc != NULL )
		{
			BOOL	bResult = this->OnNMFuncCall( pFunc );
			if ( !bReference )
				delete pFunc;
			return bResult;
		}
		return FALSE;
	}
};

namespace CNMUtil
{
	CNMFunc*			CreateFuncFromCode( _UInt32_ uCode );
};

#endif	//	#ifndef __NMFUNCTIONOBJECT_H_2D622A4E_137B_4150_95A1_A97D3E9CC46C__
