#include "NMManager.h"
#include "NMCOClientObject.h"

CNMCOClientObject::CNMCOClientObject()
{
	::ZeroMemory( &this->m_cs, sizeof( this->m_cs ) );
	::InitializeCriticalSection( &this->m_cs );
}

CNMCOClientObject::~CNMCOClientObject()
{
	::DeleteCriticalSection( &this->m_cs );
}

void CNMCOClientObject::ResetMyInfo( void )
{
	NMCSLOCK( this->m_cs )
	{
		CNMRealUserInfo		ruiRealUserInfo;

		this->m_ruiRealUserInfo		= ruiRealUserInfo;

		this->m_aGOIs.clear();
	}
}

void CNMCOClientObject::ResetGuildOnlineInfo( void )
{
	NMCSLOCK( this->m_cs )
	{
		this->m_aGOIs.clear();
	}
}

BOOL CNMCOClientObject::SetLocale
(
	NMLOCALEID	uLocaleID
)
{
	if ( CNMManager::GetInstance().SetLocale( uLocaleID ) )
	{
		CNMSetLocaleFunc func( uLocaleID );
		return CNMManager::GetInstance().CallNMFunc( &func );
	}
	return FALSE;
}

BOOL CNMCOClientObject::SetLocaleAndRegion
(
			IN	NMLOCALEID			uLocaleID
,			IN	NMREGIONCODE		uRegionCode
)
{
	if( CNMManager::GetInstance().SetLocaleAndRegion( uLocaleID, uRegionCode ) )
	{
		CNMSetLocaleFunc func( uLocaleID, uRegionCode );
		return CNMManager::GetInstance().CallNMFunc( &func );		
	}

	return FALSE;
}

BOOL CNMCOClientObject::SetCodePage
( 
			IN	UINT32				uCodePage 
)
{
	if ( CNMManager::GetInstance().SetCodePage( uCodePage ) )
	{
		CNMSetCodePageFunc func( uCodePage );
		return CNMManager::GetInstance().CallNMFunc( &func );
	}
	return FALSE;
}

UINT32 CNMCOClientObject::GetCodePage()
{
	return CNMManager::GetInstance().GetCodePage();
}

BOOL CNMCOClientObject::SetConnConfig
(
	const CNMConnConfig &	ConnConfig
)
{
	CNMSetConnConfigFunc func( ConnConfig );

	return CNMManager::GetInstance().CallNMFunc( &func );
}

BOOL CNMCOClientObject::SetCustomServerInfo
(
	ServerInfoList const & aServerInfo
)
{
	CNMSetServerInfoFunc func( aServerInfo );
	return CNMManager::GetInstance().CallNMFunc( &func );
}

BOOL CNMCOClientObject::SetPatchOption
(
	BOOL		bPatch
)
{
	return CNMManager::GetInstance().SetPatchOption( bPatch );
}

BOOL CNMCOClientObject::SetUseFriendModuleOption
(
	BOOL		bUseFriendModule
)
{
	return CNMManager::GetInstance().SetUseFriendModuleOption( bUseFriendModule );
}

BOOL CNMCOClientObject::Initialize
(
	NMGameCode	uGameCode
)
{
	CNMInitializeFunc func( uGameCode, static_cast<UINT32>( kSupportFlag_FromGameCode ) );
	return CNMManager::GetInstance().CallNMFunc( &func );
}

BOOL CNMCOClientObject::Finalize
(
)
{
	CNMFinalizeFunc func;
	return CNMManager::GetInstance().CallNMFunc( &func );
};

BOOL CNMCOClientObject::RegisterCallbackMessage
( 
	HWND		hWnd,
	UINT32		uMessage
)
{
	CNMRegisterCallbackFunc func( hWnd, uMessage );
	return CNMManager::GetInstance().CallNMFunc( &func );
}

BOOL CNMCOClientObject::ResetCallbackMessage
(
	HWND		hWnd
)
{
	CNMResetCallbackFunc func( hWnd );
	return CNMManager::GetInstance().CallNMFunc( &func );
}


NMLoginAuthReplyCode CNMCOClientObject::AttachAuth( LPCTSTR pszPassport, INT32 nRegionCode )
{
	CNMLoginAuthFunc func( pszPassport, nRegionCode );
	CNMManager::GetInstance().CallNMFunc( &func );
	return (NMLoginAuthReplyCode) func.nErrorCode;
}

BOOL CNMCOClientObject::DetachAuth()
{
	CNMLogoutAuthFunc func;
	return CNMManager::GetInstance().CallNMFunc( &func );
}

NMLoginAuthReplyCode CNMCOClientObject::LoginAuth( LPCTSTR pszNexonID, LPCTSTR pszPassword, INT32 nRegionCode, LPCTSTR szExtraData, UINT32 uChannelCode, INT32* pnSubErrCode )
{
	CNMLoginAuthFunc func( pszNexonID, pszPassword, nRegionCode, szExtraData, uChannelCode );
	CNMManager::GetInstance().CallNMFunc( &func );

	if ( pnSubErrCode != NULL )
	{
		*pnSubErrCode = func.nSubErrCode;
	}

	return (NMLoginAuthReplyCode) func.nErrorCode;
}

BOOL CNMCOClientObject::LogoutAuth()
{
	CNMLogoutAuthFunc func;
	return CNMManager::GetInstance().CallNMFunc( &func );
}

LPCTSTR CNMCOClientObject::GetNexonPassport( LPTSTR szPassportOut )
{
	CNMGetNexonPassportFunc func;
	if ( CNMManager::GetInstance().CallNMFunc( &func ) )
	{
		NMSAFE_STRCOPY( szPassportOut, func.szNexonPassport, NXPASSPORT_SIZE );
		return szPassportOut;
	}

	if ( szPassportOut )
	{
		szPassportOut[0] = 0;
	}

	return NULL;
}

NMLoginAuthReplyCode CNMCOClientObject::LoginAuthSecondary( LPCTSTR pszNexonID, LPCTSTR pszPassword )
{
	CNMLoginAuthFunc func( pszNexonID, pszPassword, 0 );
	func.bSecondary = TRUE;
	CNMManager::GetInstance().CallNMFunc( &func );
	return (NMLoginAuthReplyCode) func.nErrorCode;
}

BOOL CNMCOClientObject::LogoutAuthSecondary()
{
	CNMLogoutAuthFunc func;
	func.bSecondary = TRUE;
	return CNMManager::GetInstance().CallNMFunc( &func );
}

LPCTSTR CNMCOClientObject::GetNexonPassportSecondary( LPTSTR szPassportOut )
{
	CNMGetNexonPassportFunc func;
	func.bSecondary = TRUE;
	if ( CNMManager::GetInstance().CallNMFunc( &func ) )
	{
		NMSAFE_STRCOPY( szPassportOut, func.szNexonPassport, NXPASSPORT_SIZE );
		return szPassportOut;
	}

	if ( szPassportOut )
	{
		szPassportOut[0] = 0;
	}

	return NULL;
}

NMLoginAuthReplyCode CNMCOClientObject::SecurityAuth( LPCTSTR pszPassport, LPCTSTR pszSecurityData )
{
	CNMLoginAuthSecurityCNFunc func( pszPassport, pszSecurityData );
	CNMManager::GetInstance().CallNMFunc( &func );
	return (NMLoginAuthReplyCode) func.nErrorCode;
}

SecurityCode CNMCOClientObject::GetSecurityCodeCN()
{
	CNMGetSecurityCodeCNFunc func;
	CNMManager::GetInstance().CallNMFunc( &func );
	return (SecurityCode) func.uSecurityCodeCN;
}

LPCTSTR CNMCOClientObject::GetSecurityQuestion( LPTSTR szSecurityQuestion )
{
	CNMGetSecurityQuestionFunc func;
	if ( CNMManager::GetInstance().CallNMFunc( &func ) )
	{
		NMSAFE_STRCOPY( szSecurityQuestion, func.szSecurityQuestion, SECURITYCN_SIZE );
		return szSecurityQuestion;
	}

	if ( szSecurityQuestion )
	{
		szSecurityQuestion[0] = 0;
	}

	return NULL;
}

LPCTSTR CNMCOClientObject::GetNxHWID( LPTSTR szNxHWID )
{
	CNMGetNxHWIDFunc func;

	if ( szNxHWID )
	{
		if ( CNMManager::GetInstance().CallNMFunc( &func ) )
		{
			NMSAFE_STRCOPY( szNxHWID, func.szNxHWID, NXHWID_SIZE );
			return szNxHWID;
		}

		szNxHWID[0] = 0;
	}

	return szNxHWID;
}

NMGameTokenReplyCode CNMCOClientObject::GetGameToken( LPCTSTR pszSecureData, INT32 & nSecureCode )
{
	CNMGetGameTokenFunc func( pszSecureData );
	
	if ( CNMManager::GetInstance().CallNMFunc( &func ) )
	{
		nSecureCode = func.nSecureCode;
	}
	return (NMGameTokenReplyCode) func.nReplyCode;
}

BOOL CNMCOClientObject::LoginMessenger
(
	UINT32	uSupportFlag
)
{
	CNMLoginMessengerFunc func( uSupportFlag );
	return CNMManager::GetInstance().CallNMFunc( &func );
}

BOOL CNMCOClientObject::LoginMessengerWithPassport
(
	LPCTSTR	pszNexonID,
	LPCTSTR	pszPassword,
	PBYTE	pbyPassportData,
	INT32	cbyPassportLength,
	UINT32	uSupportFlag,
	INT32	nRegionCode
)
{
	CNMLoginPassportFunc func( pszNexonID, pszPassword, pbyPassportData, cbyPassportLength, uSupportFlag, nRegionCode );
	return CNMManager::GetInstance().CallNMFunc( &func );
}

BOOL CNMCOClientObject::LogoutMessenger
(
	void
)
{
	CNMLogoutMessengerFunc func;
	return CNMManager::GetInstance().CallNMFunc(&func);
}

BOOL CNMCOClientObject::LoginVirtual
(	
	LPCTSTR		szUserId,
	UINT32		uVirtualIdCode,
	LPCTSTR		szPosition
)
{
	CNMLoginVirtualFunc	func( szUserId, uVirtualIdCode, szPosition );
	return CNMManager::GetInstance().CallNMFunc( &func );
}

BOOL CNMCOClientObject::LogoutVirtual
( 
	void 
)
{
	CNMLogoutVirtualFunc func;
	return CNMManager::GetInstance().CallNMFunc( &func );
}

BOOL CNMCOClientObject::GetMyVirtualUserList
(	
	NMVirtualUserList&	
					aVirtualUser
,	_UInt32_		uGameCode				// = kGameCode_NULL
)
{
	CNMGetMyVirtualUserListFunc		
				func( uGameCode );

	if ( CNMManager::GetInstance().CallNMFunc( &func ) )
	{
		aVirtualUser	= func.aVirtualUser;
		return TRUE;
	}

	return FALSE;
}

BOOL CNMCOClientObject::ChangeMyPosition
( 
	LPCTSTR			szPosition				// POSITION_SIZE
)	
{
	CNMChangeMyPositionFunc		
				func( szPosition );
	return		CNMManager::GetInstance().CallNMFunc( &func );
}

BOOL CNMCOClientObject::SendRefreshMessage
( 
	NMREFRESHTYPE	uRefreshType
)
{
	CNMSendRefreshMessageFunc
					func( uRefreshType );
	return			CNMManager::GetInstance().CallNMFunc( &func );
}

BOOL CNMCOClientObject::ChangeMyLevel
(
	UINT32			uLevel,
	UINT32			uLevelType /* = 0 */
)
{
	CNMChangeMyLevelFunc	func( uLevel, uLevelType );
	return					CNMManager::GetInstance().CallNMFunc( &func );
}

BOOL CNMCOClientObject::RemoveMyVirtualUser
(
	NMVirtualKey & keyVirtual
)
{
	CNMRemoveMyVirtualUserFunc		func( keyVirtual );
	return CNMManager::GetInstance().CallNMFunc( &func );
}

_UInt64_ CNMCOClientObject::GetDllVersion
( 
	void 
)
{
	CNMGetVersionFunc	
				func( CNMGetVersionFunc::kDll );
	return		( CNMManager::GetInstance().CallNMFunc( &func ) ? func.uVersion : 0 );
}

_UInt64_ CNMCOClientObject::GetNMCOSrvVersion
( 
	void
)
{
	CNMGetVersionFunc	
				func( CNMGetVersionFunc::kCore );
	return		( CNMManager::GetInstance().CallNMFunc( &func ) ? func.uVersion : 0 );
}

_UInt64_ CNMCOClientObject::GetMyIdCode
( 
	void 
)
{
	return		this->m_ruiRealUserInfo.uIDCode;
}

_UInt32_ CNMCOClientObject::GetMyOid
( 
	void
)
{
	return		GET_OID( this->m_ruiRealUserInfo.uIDCode );
}

LPCTSTR CNMCOClientObject::GetMyLoginId
(
	void
)		
{
	return		this->m_ruiRealUserInfo.szLoginID;
}

LPCTSTR CNMCOClientObject::GetMyWord
(
	void
)
{
	return		this->m_ruiRealUserInfo.szWord;
}

NMUSERSTATUS CNMCOClientObject::GetMyStatus
( 
	void
)
{
	return		this->m_ruiRealUserInfo.uStatus;
}

_UInt32_ CNMCOClientObject::GetMyLocalIP
( 
	void
)
{
	return		this->m_ruiRealUserInfo.uLocalIP;
}

_UInt32_ CNMCOClientObject::GetMyVirtualIP
( 
	void
)
{
	return		this->m_ruiRealUserInfo.uVirtualIP;
}

LPCTSTR CNMCOClientObject::GetMyName
(
	void
)
{
	return		this->m_ruiRealUserInfo.szName;
}

LPCTSTR CNMCOClientObject::GetMyNickName
(
	void
)
{
	return		this->m_ruiRealUserInfo.szNickName;
}

_UInt32_ CNMCOClientObject::GetMyMaskInfo
(
	void
)
{
	return		this->m_ruiRealUserInfo.uMaskInfo;
}

NMSEXTYPE CNMCOClientObject::GetMySex
( 
	void
)
{
	return		this->m_ruiRealUserInfo.uSex;
}

_UInt32_ CNMCOClientObject::GetMyAge
(
	void
)
{
	return		this->m_ruiRealUserInfo.uAge;
}

_UInt32_ CNMCOClientObject::GetMyAreaCode
(
	void
)
{
	return		this->m_ruiRealUserInfo.uAreaCode;
}

LPCTSTR CNMCOClientObject::GetMyAreaName
( 
	void
)
{
	return		this->m_ruiRealUserInfo.szAreaName;
}

_UInt32_ CNMCOClientObject::GetMySchoolCode
(
	void
)
{
	return		this->m_ruiRealUserInfo.uSchoolCode;
}

LPCTSTR CNMCOClientObject::GetMySchoolName
(
	void
)
{
	return		this->m_ruiRealUserInfo.szSchoolName;
}

SYSTEMTIME CNMCOClientObject::GetMyBirthday
(
	void
)
{
	return		this->m_ruiRealUserInfo.stBirthday;
}

CNMRealUserInfo CNMCOClientObject::GetMyInfo
(
	void
)
{
	CNMGetMyInfoFunc	
					func;

	if ( CNMManager::GetInstance().CallNMFunc( &func ) )
	{
		NMCSLOCK( this->m_cs )
		{
			this->m_ruiRealUserInfo		= func.ruiUserInfo;
		}
	}

	return this->m_ruiRealUserInfo;
}

BOOL CNMCOClientObject::ChangeMyStatus
( 
	NMUSERSTATUS	uStatus 
)
{
	return		this->ChangeMyInfo( uStatus, NULL, NULL, 0xFFFFFFFF );
}

BOOL CNMCOClientObject::ChangeMyWord
( 
	LPCTSTR			szWord					// TODAYWORD_SIZE
)			
{
	return		this->ChangeMyInfo( kStatus_Unknown, szWord, NULL, 0xFFFFFFFF );
}

BOOL CNMCOClientObject::ChangeMyNickName
( 
	LPCTSTR			szNickName				// NICKNAME_SIZE
)	
{
	return		this->ChangeMyInfo( kStatus_Unknown, NULL, szNickName, 0xFFFFFFFF );
}

BOOL CNMCOClientObject::ChangeMyMaskInfo
( 
	_UInt32_		uMaskInfo 
)
{
	return		FALSE;
}

BOOL CNMCOClientObject::ChangeNickname
(
	NMVirtualKey	keyCharacter,
    LPCTSTR			szNickname
)
{
	if (szNickname)
	{
		CNMChangeNicknameFunc func( keyCharacter, szNickname );
		return CNMManager::GetInstance().CallNMFunc( &func );
	}
	return FALSE;
}

BOOL CNMCOClientObject::ChangeMetaData
(
	NMVirtualKey	keyCharacter,
    LPCTSTR			szMetaData
)
{
	if ( szMetaData )
	{
		CNMChangeMetaDataFunc func( keyCharacter, szMetaData );
		return CNMManager::GetInstance().CallNMFunc( &func );
	}
	return FALSE;
}

BOOL CNMCOClientObject::ChangeMyInfo
(	
	NMUSERSTATUS	uStatus					// = kStatus_Unknown
,	LPCTSTR			szWord					// TODAYWORD_SIZE
,	LPCTSTR			szNickName				// NICKNAME_SIZE
,	_UInt32_		uMaskInfo				// = 0xFFFFFFFF
)						
{
	CNMChangeMyInfoFunc	
					func( uStatus, szWord, szNickName, uMaskInfo, kGameCode_NULL );
	return			CNMManager::GetInstance().CallNMFunc( &func );
}

NMALLOWTYPE CNMCOClientObject::GetAllowChatType
( 
	void
)
{
	CNMGetConfigFunc	func( CNMGetConfigFunc::kFlag_AllowChatType );
	return				( CNMManager::GetInstance().CallNMFunc( &func )
						? func.uAllowChatType
						: kConfigAllowValue_Unknown );
}

NMALLOWTYPE CNMCOClientObject::GetAllowNoteType
( 
	void 
)
{
	CNMGetConfigFunc	func( CNMGetConfigFunc::kFlag_AllowNoteType );
	return				( CNMManager::GetInstance().CallNMFunc( &func )
						? func.uAllowNoteType
						: kConfigAllowValue_Unknown );
}

NMALLOWTYPE CNMCOClientObject::GetAllowGameInviteType
( 
	void
)
{
	CNMGetConfigFunc	func( CNMGetConfigFunc::kFlag_AllowGameInviteType );
	return				( CNMManager::GetInstance().CallNMFunc( &func )
						? func.uAllowGameInviteType
						: kConfigAllowValue_Unknown );
}

NMALLOWTYPE CNMCOClientObject::GetAllowRequestFriendType
( 
	void 
)
{
	CNMGetConfigFunc	func( CNMGetConfigFunc::kFlag_AllowRequestFriendType );
	return				( CNMManager::GetInstance().CallNMFunc( &func )
						? func.uAllowRequestFriendType
						: kConfigAllowValue_Unknown );
}

BOOL CNMCOClientObject::ChangeAllowChatType
( 
	NMALLOWTYPE			uType 
)
{
	CNMChangeConfigFunc	func( uType, kConfigAllowValue_Unknown, kConfigAllowValue_Unknown, kConfigAllowValue_Unknown, NULL );
	return				CNMManager::GetInstance().CallNMFunc( &func );
}

BOOL CNMCOClientObject::ChangeAllowNoteType
( 
	NMALLOWTYPE			uType 
)
{
	CNMChangeConfigFunc	func( kConfigAllowValue_Unknown, uType, kConfigAllowValue_Unknown, kConfigAllowValue_Unknown, NULL );
	return				CNMManager::GetInstance().CallNMFunc( &func );
}

BOOL CNMCOClientObject::ChangeAllowGameInviteType
( 
	NMALLOWTYPE			uType 
)
{
	CNMChangeConfigFunc	func( kConfigAllowValue_Unknown, kConfigAllowValue_Unknown, uType, kConfigAllowValue_Unknown, NULL );
	return				CNMManager::GetInstance().CallNMFunc( &func );
}

BOOL CNMCOClientObject::ChangeAllowRequestFriendType
( 
	NMALLOWTYPE			uType 
)
{
	CNMChangeConfigFunc	func( kConfigAllowValue_Unknown, kConfigAllowValue_Unknown, kConfigAllowValue_Unknown, uType, NULL );
	return				CNMManager::GetInstance().CallNMFunc( &func );
}

BOOL CNMCOClientObject::GetLocalConfig
(
	NMLOCALCONFIGTYPE	uConfigType
,	LPTSTR				szValueOut
)
{
	CNMGetLocalConfigFunc	func( uConfigType );
	if ( CNMManager::GetInstance().CallNMFunc( &func ) )
	{
		NMSAFE_STRCOPY( szValueOut, func.szValueOut, 1024 );
		return TRUE;
	}

	return FALSE;
}

BOOL CNMCOClientObject::ChangeLocalConfig
(
	NMLOCALCONFIGTYPE	uConfigType
,	LPCTSTR				szValue
)
{
	CNMChangeLocalConfigFunc
						func( uConfigType, szValue );
	return				( CNMManager::GetInstance().CallNMFunc( &func ) );
}

BOOL CNMCOClientObject::RequestNewFriend
(
	LPCTSTR		szRequestId				// LOGINID_SIZE
,	LPCTSTR		szMessage				// MSG_SIZE
,	BOOL		bAddToNxFriend			// = FALSE
,	NMGameCode	uGameCode				// = kGameCode_NULL
,	NMGameCode	uRequesteeGameCode		// = kGameCode_NULL
,	_UInt32_	uVirtualIdCode			// = 0
,	_UInt32_	uRequesteeVirtualIdCode	// = 0
)
{
	UNREFERENCED_PARAMETER(uVirtualIdCode);
	UNREFERENCED_PARAMETER(bAddToNxFriend);

	CNMRequestNewFriendFunc func;

	::memcpy( func.rnfData.szRequestID, szRequestId, LOGINID_SIZE * sizeof( szRequestId[0] ) );
	::memcpy( func.rnfData.szMessage, szMessage, MSG_SIZE * sizeof( szMessage[0] ) );
	func.rnfData.uRequesteeGameCode				= uRequesteeGameCode;
	func.rnfData.keyToVirtual.uGameCode			= uGameCode;
	func.rnfData.keyToVirtual.uVirtualIDCode	= uRequesteeVirtualIdCode;
	func.rnfData.keyFromVirtual.uGameCode		= uGameCode;
	func.rnfData.keyFromVirtual.uVirtualIDCode	= uVirtualIdCode;
	func.rnfData.uInsertCategoryCode			= 0;
	func.rnfData.bAddToNxFriend					= FALSE;

	return		CNMManager::GetInstance().CallNMFunc( &func );
}

BOOL CNMCOClientObject::ConfirmNewFriend
(
	_UInt32_		uSerialNo
,	NMCONFIRMCODE	uConfirmCode
)
{
	CNMConfirmNewFriendFunc
					func( uSerialNo, uConfirmCode, 0 );
	return			CNMManager::GetInstance().CallNMFunc( &func );
}

BOOL CNMCOClientObject::AddCategory
(
	NMVirtualKey		keyOwner
,	LPCTSTR				szCategoryName			// CATEGORY_SIZE
,	NMCATEGORYPROPERTY	uProperty
,	NMALLOWTYPE			uAllowType
)
{
	CNMAddCategoryFunc	
					func( keyOwner, szCategoryName, uProperty, uAllowType );
	return			CNMManager::GetInstance().CallNMFunc( &func );
}

BOOL CNMCOClientObject::DeleteCategory
(
	UINT32				uCategoryCode
,	BOOL				bSystemCall
)
{
	CNMDeleteCategoryFunc
					func( uCategoryCode, bSystemCall );
	return			CNMManager::GetInstance().CallNMFunc( &func );
}

BOOL CNMCOClientObject::ChangeCategoryName
(
	UINT32				uCategoryCode
,	LPCTSTR				szCategoryName			// CATEGORY_SIZE
,	BOOL				bSystemCall
)
{
	CNMChangeCategoryNameFunc
					func( uCategoryCode, szCategoryName, bSystemCall );
	return			CNMManager::GetInstance().CallNMFunc( &func );
}

BOOL CNMCOClientObject::ChangeCategoryProperty
(
	UINT32				uCategoryCode
 ,	NMCATEGORYPROPERTY	uProperty
 ,	BOOL				bSystemCall
)
{
	CNMChangeCategoryPropertyFunc
					func( uCategoryCode, uProperty, bSystemCall );
	return			CNMManager::GetInstance().CallNMFunc( &func );
}

BOOL CNMCOClientObject::ChangeCategoryAllowType
(
	UINT32				uCategoryCode
 ,	NMALLOWTYPE			uAllowType
 ,	BOOL				bSystemCall
)
{
	CNMChangeCategoryAllowTypeFunc
					func( uCategoryCode, uAllowType, bSystemCall );
	return			CNMManager::GetInstance().CallNMFunc( &func );
}

BOOL CNMCOClientObject::GetFriendList
(
	NMCategoryUserList&		
					aCateFriend
,	NMFriendList&
					aUnrelated
,	BOOL			bAll
)
{
	NMVirtualKey	keyMine = { 0xFFFFFFFF, 0 };
	CNMGetFriendListFunc
					func( bAll ? keyVirtualAll : keyMine );
	
	if (  CNMManager::GetInstance().CallNMFunc( &func ) )
	{
		aCateFriend		= func.aCateUserList;
		aUnrelated		= func.aUnrelated;
		return TRUE;
	}

	return FALSE;
}

BOOL CNMCOClientObject::GetFriendListEx
(
	NMCategoryUserList&		
					aCateFriend
,	NMFriendList&
					aUnrelated
,	NMVirtualKey	keyVirtual			// = keyVirtualAll
)
{
//	NMVirtualKey	keyMine = { 0xFFFFFFFF, 0 };
	CNMGetFriendListFunc
					func( keyVirtual );
	
	if (  CNMManager::GetInstance().CallNMFunc( &func ) )
	{
		aCateFriend		= func.aCateUserList;
		aUnrelated		= func.aUnrelated;
		return TRUE;
	}

	return FALSE;
}

BOOL CNMCOClientObject::GetFriendInfo
(
	NMFriendKey		keyFriend
,	CNMFriendInfo&	fiFriendInfo
)
{
	CNMGetFriendInfoFunc
					func( keyFriend );
	if ( CNMManager::GetInstance().CallNMFunc( &func ) )
	{
		fiFriendInfo = func.fiFriendInfo;
		return TRUE;
	}
	return FALSE;
}

BOOL CNMCOClientObject::BlockFriend
(
	NMFriendKey		keyFriend
,	BOOL			bBlock
)
{
	CNMBlockFriendFunc
					func( keyFriend, bBlock );
	return			CNMManager::GetInstance().CallNMFunc( &func );
}

BOOL CNMCOClientObject::AddFriendToCategory
(
	NMFriendKey		keyFriend
,	UINT32			uCategoryCode
,	BOOL			bSystemCall
)
{
	CNMAddFriendToCategoryFunc
					func( keyFriend, uCategoryCode, bSystemCall );
	return			CNMManager::GetInstance().CallNMFunc( &func );
}

BOOL CNMCOClientObject::DeleteFriendFromCategory
(
	NMFriendKey		keyFriend
,	UINT32			uCategoryCode
,	BOOL			bSystemCall
)
{
	CNMDeleteFriendFromCategoryFunc
					func( keyFriend, uCategoryCode, bSystemCall );
	return			CNMManager::GetInstance().CallNMFunc( &func );
}

BOOL CNMCOClientObject::MoveFriendCategory
(
	NMFriendKey		keyFriend
,	UINT32			uFromCategoryCode
,	UINT32			uToCategoryCode
,	BOOL			bSystemCall
)
{
	CNMMoveFriendCategoryFunc
					func( keyFriend, uFromCategoryCode, uToCategoryCode, bSystemCall );
	return			CNMManager::GetInstance().CallNMFunc( &func );
}

BOOL CNMCOClientObject::ChangeFriendMemo
(
	NMFriendKey		keyFriend
,	LPCTSTR			szMemo					// MEMO_SIZE
)
{
	CNMChangeFriendMemoExFunc
					func( keyFriend, szMemo );
	return			CNMManager::GetInstance().CallNMFunc( &func );
}

BOOL CNMCOClientObject::GetRejectedUserList
( 
	NMRejectedUserList &	rejectedUserList
)
{
	CNMGetRejectedUserListFunc func;
	if ( CNMManager::GetInstance().CallNMFunc( &func ) )
	{
		rejectedUserList = func.rejectedUserList;
		return TRUE;
	}
	return FALSE;
}

BOOL CNMCOClientObject::AppendRejectedUser
(
	CNMRejectedUser & rejectedUser
)
{
	CNMAppendRejectedUserFunc func(rejectedUser);
	return CNMManager::GetInstance().CallNMFunc( &func );
}

BOOL CNMCOClientObject::RemoveRejectedUser
(
	CNMRejectedUser & rejectedUser
)
{
	CNMRemoveRejectedUserFunc func(rejectedUser);
	return CNMManager::GetInstance().CallNMFunc( &func );
}

BOOL CNMCOClientObject::SendNoteMsg
(	
	CNMNoteInfo::NMNOTETYPE	uNoteType
,	UINT32 uSubType
,	BOOL			bSaveToPermBox
,	LPCTSTR			szTargetId				// TARGETID_SIZE
,	LPCTSTR			szMessage				// NOTEMSG_SIZE
,	LPCTSTR			szArgument				// NOTEARG_SIZE, = NULL
,	LPCTSTR			szCustomData			// NOTECUSTOM_SIZE, = NULL
,	NMGameCode		uGameCode				// = kGameCode_NULL
,	_UInt32_		uVirtualIdCode			// = 1
)
{
	if ( uGameCode != kGameCode_NULL )
	{
		NMVirtualKey	keyVirtual	= { uGameCode, uVirtualIdCode };
		CNMSendNoteMsgFunc	func( uNoteType, static_cast<CNMNoteInfo::NMNOTESUBTYPE>(uSubType), keyVirtual, szTargetId, szMessage, szArgument, szCustomData, bSaveToPermBox );
		return		CNMManager::GetInstance().CallNMFunc( &func );
	}

	CNMSendNoteMsgFunc	func( uNoteType, static_cast<CNMNoteInfo::NMNOTESUBTYPE>(uSubType), szTargetId, szMessage, szArgument, szCustomData, bSaveToPermBox );
	return			CNMManager::GetInstance().CallNMFunc( &func );
}

BOOL CNMCOClientObject::SendNoteExMsg
(	CNMNoteInfoEx::NMNOTEEXTYPE	
					uNoteExType
,	LPCTSTR			szMessage				// NOTEEXMSG_SIZE
)		
{
	CNMSendNoteExMsgFunc	
					func( uNoteExType, szMessage );
	return			CNMManager::GetInstance().CallNMFunc( &func );
}

BOOL CNMCOClientObject::SendReportMsg
(	
	CNMReportInfo::NMREPORTTYPE		
					uType
,	CNMReportInfo::NMREPORTSUBTYPE	
					uSubType
,	LPCTSTR			szTargetNickname		// NICKNAME_SIZE
,	LPCTSTR			szReason				// 4000
,	LPCTSTR			szEvidence				// 20000
,	_UInt32_		uGameCode				// = kGameCode_NULL
,	_UInt32_		uAppliedGameCode		// = kGameCode_NULL
,	LPCTSTR			szReporterNickname		// NICKNAME_SIZE
)
{
	if ( _tcslen( szReporterNickname ) == 0 )
		return FALSE;

	CNMSendReportMsgFunc
					func( uType, uSubType, szTargetNickname, szReason, szEvidence, uGameCode, uAppliedGameCode, szReporterNickname );
	return			CNMManager::GetInstance().CallNMFunc( &func );
}

BOOL CNMCOClientObject::SendAccuseNote
(	
	CNMReportInfo::NMREPORTSUBTYPE	
					uSubType
,	LPCTSTR			szReason				// 4000
,	_UInt32_		uNoteSerial
)
{
	CNMSendAccuseNoteMsgFunc
					func( uSubType, szReason, uNoteSerial );
	return			CNMManager::GetInstance().CallNMFunc( &func );
}

BOOL CNMCOClientObject::LogReportSync
(	UINT16			uLogType
)		
{
	CNMLogReportSyncFunc	
		func( uLogType );
	return			CNMManager::GetInstance().CallNMFunc( &func );
}

NMNoteList CNMCOClientObject::GetNoteBox
(
	NMNOTEBOXTYPE	uBoxType
,	_UInt32_		uNoteSerial
)
{
	CNMGetNoteBoxFunc
					func( uBoxType, uNoteSerial );
	BOOL			bReturn		= CNMManager::GetInstance().CallNMFunc( &func );
	(void)bReturn;
	return			func.aNotes;
}

BOOL CNMCOClientObject::GetNoteBox
(
	NMNOTEBOXTYPE		uBoxType
,	UINT32				uNoteSerial
,	NMNoteInfoList &	noteInfoList
)
{
	CNMGetNoteBox2Func func( uBoxType, uNoteSerial );
	BOOL bResult = CNMManager::GetInstance().CallNMFunc( &func );
	if (bResult)
	{
		noteInfoList.assign(func.aNotes.begin(), func.aNotes.end());
	}
	return bResult;
}

BOOL CNMCOClientObject::SetNoteReadFlag
(
	_UInt32_		uNoteSerial
)
{
	CNMProcessNoteFunc
					func( CNMProcessNoteFunc::kType_SetReadFlag, uNoteSerial );
	return			CNMManager::GetInstance().CallNMFunc( &func );
}

BOOL CNMCOClientObject::SaveNoteToPermBox
(
	_UInt32_		uNoteSerial
)
{
	CNMProcessNoteFunc
					func( CNMProcessNoteFunc::kType_SaveToPerm, uNoteSerial );
	return			CNMManager::GetInstance().CallNMFunc( &func );
}

BOOL CNMCOClientObject::DeleteNote
(
	_UInt32_		uNoteSerial
)
{
	CNMProcessNoteFunc
					func( CNMProcessNoteFunc::kType_DeleteNote, uNoteSerial );
	return			CNMManager::GetInstance().CallNMFunc( &func );
}

CNMGuildListExInfo CNMCOClientObject::GetMyGuildList
(
	void
)
{
	CNMGetMyGuildListExFunc func;
	CNMManager::GetInstance().CallNMFunc( &func );
	return func.gliGuildList;
}

BOOL CNMCOClientObject::MonitorGuildOnlineInfoEx( NMGuildKey uGuildKey, BOOL bAttach )
{
	CNMMonitorGuildOnlineInfoExFunc func( uGuildKey, bAttach );
	return CNMManager::GetInstance().CallNMFunc( &func );
}

BOOL CNMCOClientObject::MonitorGuildOnlineInfoEx( NMGuildID const & guildID, UINT32 uVirtualIDCode, BOOL bAttach )
{
	CNMMonitorGuildOnlineInfoExFunc func( guildID.GetGuildKey(), bAttach, 0, guildID.nGameCode, uVirtualIDCode );
	return CNMManager::GetInstance().CallNMFunc( &func );
}

BOOL CNMCOClientObject::GetGuildOnlineInfoEx( NMGuildKey uGuildKey, CNMGuildOnlineInfoEx& goiInfo )
{
	NMCSLOCK( this->m_cs )
	{
		NMGOIList::iterator		iterBegin, iterEnd, iterCur;

		iterBegin		= this->m_aGOIs.begin();
		iterEnd			= this->m_aGOIs.end();

		for ( iterCur = iterBegin; iterCur != iterEnd; ++iterCur )
		{
			if ( (*iterCur).uGuildKey == uGuildKey )
			{
				goiInfo = (*iterCur);
				return TRUE;
			}
		}
	}

	return FALSE;
}

BOOL CNMCOClientObject::GetGuildOnlineInfoEx( NMGuildID const & guildID, CNMGuildOnlineInfoEx & goi )
{
	NMCSLOCK( this->m_cs )
	{
		NMGOIList::iterator i = std::find_if( this->m_aGOIs.begin(), this->m_aGOIs.end(), IsSameGuildID< NMGOIList::value_type >( guildID ) );
		if ( i != this->m_aGOIs.end() )
		{
			goi = *i;
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CNMCOClientObject::GetGameList
(
	CNMGameListData*	pglGameList
)
{
	if ( pglGameList != NULL )
	{
		CNMGetGameListFunc	func;
		if ( CNMManager::GetInstance().CallNMFunc( &func ) )
		{
			*pglGameList = func.glGameList;
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CNMCOClientObject::GetUserInfo
(
	LPCTSTR				szTargetID						// LOGINID_SIZE
,	NMGameCode			uGameCode						// = kGameCode_NULL
)
{
	CNMGetUserInfoFunc	func( szTargetID, uGameCode );
	return CNMManager::GetInstance().CallNMFunc( &func );
}

BOOL CNMCOClientObject::SendCustomNotify
(
	UINT32				uType
,	LPCTSTR				szArgument			// MSG_SIZE (256)
,	UINT32				uPolicyFlag/* = CNMCustomNotifyData::kCustomNotifyPolicy_Default*/
)
{
	CNMCustomNotifyFunc	func( uType, szArgument, uPolicyFlag );
	return				CNMManager::GetInstance().CallNMFunc( &func );
}

BOOL CNMCOClientObject::SendCustomNotify
(
	CNMCustomNotifyData& cnData
)
{
	CNMCustomNotifyFunc	func( cnData );
	return				CNMManager::GetInstance().CallNMFunc( &func );
}

BOOL CNMCOClientObject::RequestChatSession
( 
	NMFriendCode		fcFriendCode
)
{
	CNMRequestChatSessionFunc
						func( fcFriendCode, 0 );
	return				CNMManager::GetInstance().CallNMFunc( &func );
}

BOOL CNMCOClientObject::RequestMultiChatSession
( 
	NMFriendCodeList&	aFriendCodeList
,	NMVirtualKey		keyVirtuality
)
{
	CNMRequestMultiChatSessionFunc
						func( keyVirtuality, aFriendCodeList, 0 );
	return				CNMManager::GetInstance().CallNMFunc( &func );
}

BOOL CNMCOClientObject::RequestFileUploadSession
( 
	NMFriendCode		fcFriendCode
,	NMFilePathList&		aFilePath
)
{
	CNMRequestFileUploadSessionFunc
						func( fcFriendCode, 0, aFilePath );
	return				CNMManager::GetInstance().CallNMFunc( &func );
}

BOOL CNMCOClientObject::RequestChatSession
( 
	NMFriendKey			keyFriend
,	_UInt32_			*pSessionKey
)
{
	CNMRequestChatSessionExFunc
						func( keyFriend, 0 );
	BOOL bResult		= CNMManager::GetInstance().CallNMFunc( &func );

	if ( bResult != FALSE && pSessionKey != NULL )
	{
		if ( func.uCreatedSerialKey != 0 )
		{
			*pSessionKey	= func.uCreatedSerialKey;
		}
		else
		{
			*pSessionKey	= func.uExistingSerialKey;
		}
	}

	return bResult;
}

BOOL CNMCOClientObject::RequestMultiChatSession
( 
	NMFriendKeyList&	aFriendKey
,	NMVirtualKey		keyVirtuality
)
{
	CNMRequestMultiChatSessionExFunc
						func( keyVirtuality, aFriendKey, 0 );
	return				CNMManager::GetInstance().CallNMFunc( &func );
}

BOOL CNMCOClientObject::RequestFileUploadSession
( 
	NMFriendKey			keyFriend
,	NMFilePathList&		aFilePath
)
{
	CNMRequestFileUploadSessionExFunc
						func( keyFriend, 0, aFilePath );
	return				CNMManager::GetInstance().CallNMFunc( &func );
}

BOOL CNMCOClientObject::RequestChatSessionToGuildMember
(
	LPCTSTR				szRequestID
,	NMVirtualKey		keyVirtuality
)
{
	CNMRequestSessionToOtherFunc
						func( kSessionType_Chat, CNMSessionInfo::kInviteMode_GuildMember, keyVirtuality, szRequestID, kGameCode_NULL );
	return				CNMManager::GetInstance().CallNMFunc( &func );
}

BOOL CNMCOClientObject::RequestFileUploadSessionToGuildMember
(
	LPCTSTR				szRequestID
,	NMFilePathList&		aFilePath
,	NMVirtualKey		keyVirtuality
)
{
	CNMRequestSessionToOtherFunc
						func( kSessionType_FileUpload, CNMSessionInfo::kInviteMode_GuildMember, keyVirtuality, szRequestID, kGameCode_NULL, aFilePath );
	return				CNMManager::GetInstance().CallNMFunc( &func );
}

BOOL CNMCOClientObject::CreateChatRoomEx
(
	_UInt32_			uRoomType
,	NMGuildKey			uGuildKey
,	LPCTSTR				szGuildTitle			// GUILDNAME_SIZE
,	LPCTSTR				szRoomTitle				// ROOMTITLE_SIZE
,	_UInt32_			uLimitLevel
,	_UInt32_			uMaxSize
,	_UInt32_			uFlag
,	LPCTSTR				szPassword				// ROOMPWD_SIZE
,	LPCTSTR				szServerAddr			// 32
,	_UInt16_			uServerPort
,	UINT32				uVirtualIdCode			// = 1
)
{
	CNMCreateChatRoomExFunc	
						func( uRoomType, uGuildKey, szGuildTitle, szRoomTitle, uLimitLevel,uMaxSize, uFlag, szPassword, szServerAddr, uServerPort, uVirtualIdCode );
	return				CNMManager::GetInstance().CallNMFunc( &func );
}

BOOL CNMCOClientObject::JoinToChatRoomEx
(	
	NMGuildKey			uGuildKey
,	_UInt32_			uRoomSerialNo
,	LPCTSTR				szPassword				// ROOMPWD_SIZE
,	LPCTSTR				szServerAddr			// 32
,	_UInt16_			uServerPort 
,	_UInt32_			uFlag					// = 0
,	UINT32				uVirtualIdCode			// = 1
)
{
	CNMJoinToChatRoomExFunc	
						func( uGuildKey, uRoomSerialNo, szPassword, szServerAddr, uServerPort, uFlag, uVirtualIdCode );
	return				CNMManager::GetInstance().CallNMFunc( &func );
}

UINT32 CNMCOClientObject::GetUnreadNoteCount
(
	void
)
{
	CNMGetCountFunc func( CNMGetCountFunc::kType_UnreadNote );
	if ( CNMManager::GetInstance().CallNMFunc( &func ) )
	{
		return func.uCount;
	}
	return 0;
}

BOOL CNMCOClientObject::RequestNews()
{
	CNMRequestNewsFunc	func;
	return CNMManager::GetInstance().CallNMFunc( &func );
}

BOOL CNMCOClientObject::CheckNews
( 
	NMGameCode		uEventGameCode,
	NMVirtualKey	keyVirtual/* = keyVirtualAll */
)
{
	CNMCheckNewsFunc	func( uEventGameCode, keyVirtual );
	return				CNMManager::GetInstance().CallNMFunc( &func );
}

INT32 CNMCOClientObject::CharacterLogin
(
	LPCTSTR				szCharacterName
,	UINT32				uCharacterSN	/*	= 1  */
,	INT32				nServerCode		/*	= 1  */
)
{
	CNMCharacterLoginFunc	func( szCharacterName, uCharacterSN, nServerCode );
	return CNMManager::GetInstance().CallNMFunc( &func );
}

INT32 CNMCOClientObject::CharacterLogout
( 
	LPCTSTR				szCharacterName
,	UINT32				uCharacterSN	/*	= 1  */
,	INT32				nServerCode		/*	= 1  */
)
{
	CNMCharacterLogoutFunc func( szCharacterName, uCharacterSN, nServerCode );
	return CNMManager::GetInstance().CallNMFunc( &func );
}

INT32 CNMCOClientObject::CharacterRemove
(
	LPCTSTR				szCharacterName
,	UINT32				uCharacterSN	/*	= 1  */
,	INT32				nServerCode		/*	= 1  */
)
{
	NMVirtualKey	keyVirtual = { 0, 0 };
	keyVirtual.uVirtualIDCode = uCharacterSN;

	CNMCharacterRemoveFunc	func( keyVirtual, szCharacterName, nServerCode );
	return CNMManager::GetInstance().CallNMFunc( &func );
}

INT32 CNMCOClientObject::CharacterChangeName
(
	LPCTSTR				szCharacterName
,	UINT32				uCharacterSN	/*	= 1  */
,	INT32				nServerCode		/*	= 1  */
 )
{
	if ( szCharacterName )
	{
		NMVirtualKey	keyVirtual = { 0, 0 };
		keyVirtual.uVirtualIDCode = uCharacterSN;

		CNMCharacterChangeNameFunc func( keyVirtual, szCharacterName, nServerCode );
		return CNMManager::GetInstance().CallNMFunc( &func );
	}
	return FALSE;
}

INT32 CNMCOClientObject::CharacterRemove
( 
	NMVirtualKey &		keyVirtual 
,	LPCTSTR				szNickname
,	INT32				nServerCode		/*	= 1	*/
)
{
	CNMCharacterRemoveFunc	func( keyVirtual, szNickname, nServerCode );
	return CNMManager::GetInstance().CallNMFunc( &func );
}

INT32 CNMCOClientObject::CharacterChangeName
(
	NMVirtualKey &		keyVirtual 
,	LPCTSTR				szNickname
,	INT32				nServerCode		/*	= 1	*/
 )
{
	if (szNickname)
	{
		CNMCharacterChangeNameFunc func( keyVirtual, szNickname, nServerCode );
		return CNMManager::GetInstance().CallNMFunc( &func );
	}
	return FALSE;
}

INT32 CNMCOClientObject::CharacterSync
(
	LPCTSTR				szCharacterName
,	UINT32				uCharacterSN	/*	= 1  */
,	INT32				nServerCode		/*	= 1  */
)
{
	CNMCharacterSyncFunc	func( szCharacterName, uCharacterSN, nServerCode );
	return CNMManager::GetInstance().CallNMFunc( &func );
}

INT32 CNMCOClientObject::CharacterLogin64
(
	LPCTSTR				szCharacterName
,	UINT64				uCharacterSN	/*	= 1  */
,	INT32				nServerCode		/*	= 1  */
)
{
	CNMCharacterLogin64Func	func( szCharacterName, uCharacterSN, nServerCode );
	return CNMManager::GetInstance().CallNMFunc( &func );
}

INT32 CNMCOClientObject::CharacterLogout64
( 
	LPCTSTR				szCharacterName
,	UINT64				uCharacterSN	/*	= 1  */
,	INT32				nServerCode		/*	= 1  */
)
{
	CNMCharacterLogout64Func func( szCharacterName, uCharacterSN, nServerCode );
	return CNMManager::GetInstance().CallNMFunc( &func );
}

INT32 CNMCOClientObject::CharacterRemove64
( 
	LPCTSTR				szCharacterName
,	UINT64				uCharacterSN	/*	= 1  */
,	INT32				nServerCode		/*	= 1	*/
)
{
	CNMCharacterRemove64Func	func( szCharacterName, uCharacterSN, nServerCode );
	return CNMManager::GetInstance().CallNMFunc( &func );
}

INT32 CNMCOClientObject::CharacterChangeName64
(
	LPCTSTR				szCharacterName
,	UINT64				uCharacterSN	/*	= 1  */
,	INT32				nServerCode		/*	= 1	*/
)
{
	if ( szCharacterName )
	{
		CNMCharacterChangeName64Func func( szCharacterName, uCharacterSN, nServerCode );
		return CNMManager::GetInstance().CallNMFunc( &func );
	}
	return FALSE;
}

INT32 CNMCOClientObject::CharacterSync64
(
	LPCTSTR				szCharacterName
,	UINT64				uCharacterSN	/*	= 1  */
,	INT32				nServerCode		/*	= 1  */
)
{
	CNMCharacterSync64Func	func( szCharacterName, uCharacterSN, nServerCode );
	return CNMManager::GetInstance().CallNMFunc( &func );
}

BOOL CNMCOClientObject::HasRestrictedWords
(
	LPCTSTR				szText
)
{
	if ( szText )
	{
		CNMRestrictedWordsFunc func( CNMRestrictedWordsFunc::kOpType_HasRestrictedWords, kGameCode_NULL, szText );
		if ( CNMManager::GetInstance().CallNMFunc( &func ) )
		{
			return func.bHasRestrictedWords;
		}
	}
	return FALSE;
}

BOOL CNMCOClientObject::ReplaceRestrictedWords
(
	LPTSTR				szText,
	UINT				uTextSize
)
{
	if ( szText )
	{
		CNMRestrictedWordsFunc func( CNMRestrictedWordsFunc::kOpType_ReplaceRestrictedWords, kGameCode_NULL, szText );
		if ( CNMManager::GetInstance().CallNMFunc( &func ) )
		{
			if ( func.bHasRestrictedWords )
			{
				::StringCchCopyEx( szText, uTextSize, func.szText, NULL, NULL, STRSAFE_IGNORE_NULLS );
				return TRUE;
			}
		}
	}
	return FALSE;
}

NMP2PErrorCode CNMCOClientObject::P2PLogin
(
	LPCTSTR				szNexonPassport
)
{
	CNMP2PLoginFunc func( szNexonPassport );
	CNMManager::GetInstance().CallNMFunc( &func );
	return func.nErrorCode;
}

NMP2PErrorCode CNMCOClientObject::P2PLogout
(
)
{
	CNMP2PLogoutFunc func;
	CNMManager::GetInstance().CallNMFunc( &func );
	return func.nErrorCode;
}

NMP2PErrorCode CNMCOClientObject::P2PSendData
(
	UINT32				uP2PSerialKey,
	NMP2PKey const &	sender,
	NMP2PKey const &	receiver,
	BYTE const *		pbyData,
	UINT32				cbyDataSize,
	UINT32				uTTL
)
{
	CNMP2PSendDataFunc func( uP2PSerialKey, CNMP2PMessage( sender, receiver, pbyData, cbyDataSize ), uTTL );
	CNMManager::GetInstance().CallNMFunc( &func );
	return func.nErrorCode;
}

NMP2PErrorCode CNMCOClientObject::P2PSendData
(
	NMP2PKey const &	sender,
	NMP2PKey const &	receiver,
	BYTE const *		pbyData,
	UINT32				cbyDataSize,
	UINT32 &			uP2PSerialKey,	// OUT
	UINT32				uTTL
)   
{
	CNMP2PMultiSendDataFunc func( sender, &receiver, 1, pbyData, cbyDataSize, uTTL );
	CNMManager::GetInstance().CallNMFunc( &func );
	uP2PSerialKey = func.uP2PSerialKey;
	return func.nErrorCode;
}

NMP2PErrorCode CNMCOClientObject::P2PSendData
(
	NMP2PKey const &	sender,
	NMP2PKey const *	receivers,
	UINT32				receiversSize,
	BYTE const *		pbyData,
	UINT32				cbyDataSize,
	UINT32 &			uP2PSerialKey,	// OUT
	UINT32				uTTL
)   
{
	CNMP2PMultiSendDataFunc func( sender, receivers, receiversSize, pbyData, cbyDataSize, uTTL );
	CNMManager::GetInstance().CallNMFunc( &func );
	uP2PSerialKey = func.uP2PSerialKey;
	return func.nErrorCode;
}


BOOL CNMCOClientObject::OpenGameSummary
(
	void
)
{
	CNMOpenGameSummaryFunc func;
	return CNMManager::GetInstance().CallNMFunc( &func );
}


BOOL CNMCOClientObject::P2LinkerGetHomeURL
(
	LPTSTR			szUrlOut		// 1024
)
{
	if ( szUrlOut != NULL )
	{
		CNMGetUrlExFunc		func;
		func.uType			= kURLType_Page;
		func.uPage			= kURL_P2Home;
		func.bLoginDirect	= TRUE;

		if ( CNMManager::GetInstance().CallNMFunc( &func ) )
		{
			NMSAFE_STRCOPY( szUrlOut, func.szUrlOut, 1024 );
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CNMCOClientObject::P2LinkerGetProfileURL
(
	UINT32			uServerCode,
	LPCTSTR			szCharacterName,
	LPTSTR			szUrlOut		// 1024
)
{
	if ( szUrlOut != NULL && szCharacterName != NULL )
	{
		CNMGetUrlExFunc		func;
		func.uType			= kURLType_Page;
		func.uPage			= kURL_P2Profile;
		func.uParam			= uServerCode;
		NMSAFE_STRCOPY( func.szParam, szCharacterName, 256 );
		func.bLoginDirect	= TRUE;

		if ( CNMManager::GetInstance().CallNMFunc( &func ) )
		{
			NMSAFE_STRCOPY( szUrlOut, func.szUrlOut, 1024 );
			return TRUE;
		}
	}
	return FALSE;
}

NMP2ChatError CNMCOClientObject::P2LinkerWriteChat
(
	NMP2ChatType	chatType,
	UINT32			uServerCode,
	INT64			nCharacterSN,
	LPCTSTR			szChat,
	LPCTSTR			szParam
)
{
	CNMP2LinkerWriteChatFunc func( chatType, uServerCode, nCharacterSN, szChat, szParam );
	CNMManager::GetInstance().CallNMFunc( &func );
	return func.nErrorCode;
}