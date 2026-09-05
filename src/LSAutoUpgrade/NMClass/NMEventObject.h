#ifndef __NMEVENTOBJECT_H_40D058E4_937B_4f2d_8BA1_5981279D97CE__
#define __NMEVENTOBJECT_H_40D058E4_937B_4f2d_8BA1_5981279D97CE__

/////////////////////////////////////////////////////////////////
// DO NOT CHANGE AN ORDER OF THE CODE BELOW
/////////////////////////////////////////////////////////////////

enum	NMEVENTCODE
{
	// control
	kNMEventCode_ControlStart_					= 0x1000,
	kNMEventCode_MessengerReply					= 0x1001,
	kNMEventCode_MsgConnectionClosed			= 0x1002,
	kNMEventCode_Refresh						= 0x1003,
	kNMEventCode_Special						= 0x1004,
	kNMEventCode_RequestNewFriend				= 0x1005,
	kNMEventCode_ServerMessage					= 0x1006,
	kNMEventCode_CustomMessage					= 0x1007,
	kNMEventCode_NoteInstantMsg					= 0x1008,	// original note version
	kNMEventCode_RefreshMessage					= 0x1009,
	kNMEventCode_FindUserReply					= 0x100A,
	kNMEventCode_InviteVirtualUser				= 0x100B,
	kNMEventCode_UserInfo						= 0x100C,
	kNMEventCode_GuildOnlineInfo				= 0x100D,	// REMOVED

	kNMEventCode_RealFriendInfoChanged			= 0x100E,
	kNMEventCode_VirtualFriendInfoChanged		= 0x100F,
	kNMEventCode_FriendInfoChanged				= 0x1010,

	kNMEventCode_Notify							= 0x1011,

	kNMEventCode_GuildOnlineInfoEx				= 0x1012,
	kNMEventCode_GuildChatMessage				= 0x1013,

	kNMEventCode_CustomNotify					= 0x1014,

	kNMEventCode_RejectedUserListChanged		= 0x1015,
	kNMEventCode_NoteInfo						= 0x1016,	// new note version
	kNMEventCode_AuthConnectionClosed			= 0x1017,
	kNMEventCode_AuthSecondaryConnectionClosed	= 0x1018,

	// chat room
	kNMEventCode_ChatRoomStart_					= 0x9000,
	kNMEventCode_CRChatRoomCreated				= 0x9001,	// REMOVED //
	kNMEventCode_CRChatRoomEstablished			= 0x9002,	// REMOVED //
	kNMEventCode_CRChatRoomError				= 0x9003,
	kNMEventCode_CRChatRoomMember				= 0x9004,
	kNMEventCode_CRChatRoomInfo					= 0x9005,	// REMOVED //
	kNMEventCode_CRChatRoomMessageReceived		= 0x9006,
	kNMEventCode_CRChatRoomCreatedEx			= 0x9007,
	kNMEventCode_CRChatRoomEstablishedEx		= 0x9008,
	kNMEventCode_CRChatRoomInfoEx				= 0x9009,

	// general session
	kNMEventCode_GeneralSessionStart_			= 0xA000,
	kNMEventCode_GSSessionCreated				= 0xA001,
	kNMEventCode_GSSessionEstablished			= 0xA002,
	kNMEventCode_GSSessionFailed				= 0xA003,
	kNMEventCode_GSSessionClosed				= 0xA004,
	kNMEventCode_GSSessionInfoChanged			= 0xA005,
	kNMEventCode_GSNewMemberJoined				= 0xA006,
	kNMEventCode_GSMemberLeaved					= 0xA007,
	kNMEventCode_GSMemberInfoChanged			= 0xA008,
	kNMEventCode_GSSessionInfo					= 0xA009,
	kNMEventCode_GSInviteRejected				= 0xA00A,

	// chat session
	kNMEventCode_ChatSessionStart_				= 0xA100,
	kNMEventCode_CSChatMessageReceived			= 0xA101,
	kNMEventCode_CSMultiChatCreated				= 0xA102,

	// file upload session
	kNMEventCode_FileUploadSessionStart_		= 0xA200,
	kNMEventCode_FUSFileUploadEventReceived 	= 0xA201,

	// file download session
	kNMEventCode_FileDownloadSessionStart_		= 0xA300,
	kNMEventCode_FDSFileDownloadEventReceived	= 0xA301,

	// white-board session
	kNMEventCode_WhiteboardSessionStart_		= 0xA400,
	kNMEventCode_WSWhiteBoardMsgReceived		= 0xA401,
	kNMEventCode_WSWhiteBoardAckReceived		= 0xA402,
	kNMEventCode_WSAssocSerialKey				= 0xA403,

	//	이벤트 (출석체크 이벤트 등)
	kNMEventCode_EventStart_					= 0xB000,
	kNMEventCode_AttendanceEvent				= 0xB001,
	kNMEventCode_KartDormant					= 0xB002,	// REMOVED //

	// channel
	kNMEventCode_ChannelStart_					= 0xC000,
	kNMEventCode_ChannelError					= 0xC001,
	kNMEventCode_ChannelCreated					= 0xC002,
	kNMEventCode_ChannelEstablished				= 0xC003,
	kNMEventCode_ChannelInfo					= 0xC004,
	kNMEventCode_ChannelMemberInfo				= 0xC005,
	kNMEventCode_ChannelMemberInfoList			= 0xC006,
	kNMEventCode_ChannelMessage					= 0xC007,

	// client util
	kNMEventCode_ClientUtilStart_				= 0xD000,
	kNMEventCode_LogReportSync					= 0xD001,

	// P2P
	kNMEventCode_P2PStart_						= 0xE000,
	kNMEventCode_P2PLoginReply					= 0xE001,
	kNMEventCode_P2PSendDataReply				= 0xE002,
	kNMEventCode_P2PMessage						= 0xE003,
	kNMEventCode_P2PConnectionClosed			= 0xE004,
};

//=========================================
// base class
//=========================================
class CNMFuncCaller;
class CNMEvent : public CNMSerializable
{
public:
	enum
	{
		kFlag_PreferPlug	= ( 0x1 << 0 ),
	};

private:
	_UInt32_			m_uCode;
	BOOL				m_bHandled;
	BOOL				m_bPassThrough;
	BOOL				m_bHasResult;
	_UInt32_			m_uRemainingCount;
	_UInt32_			m_uFlag;

public:
	_UInt32_			GetCode( void )							{ return this->m_uCode; }
	void				SetHandled( BOOL bSet = TRUE )			{ this->m_bHandled = bSet; }
	BOOL				IsHandled( void )						{ return this->m_bHandled; }
	void				SetPassThrough( BOOL bSet = TRUE )		{ this->m_bPassThrough = bSet; }
	BOOL				IsPassThrough( void )					{ return this->m_bPassThrough; }
	BOOL				HasResult( void )						{ return this->m_bHasResult; }
	_UInt32_			GetSerialKey( void )					{ return this->GetSerialKeyImpl(); }
	_UInt32_			GetResult( void )						{ return this->GetResultImpl(); }
	void				SetResult( _UInt32_ uResult )			{ this->SetResultImpl( uResult ); }
	_UInt32_			GetRemainingCount( void )				{ return this->m_uRemainingCount; }
	void				SetRemainingCount( _UInt32_ uCount )	{ this->m_uRemainingCount = uCount; }
	_UInt32_			GetFlag( void )							{ return this->m_uFlag; }
	void				SetFlag( _UInt32_ uFlag )				{ this->m_uFlag = uFlag; }

	virtual LPCTSTR		GetName( void )							{ return _T( "CNMEvent" ); }
	virtual _UInt32_	GetSerialKeyImpl( void )				{ return 0; };
	virtual _UInt32_	GetResultImpl( void )					{ return 0; };
	virtual void		SetResultImpl( _UInt32_ uResult )		{ UNREFERENCED_PARAMETER(uResult); };

public:
	// event data

public:
	CNMEvent( _UInt32_ uCode, _UInt8_ uObjectVersion, BOOL bHasResult = FALSE, _UInt32_ uFlag = 0 );

	DECLARE_SERIALIZABLE
};

#define	DECLARE_NMEVENT( eventName )						\
	enum { kCode = kNMEventCode_##eventName };				\
	virtual LPCTSTR GetName( void )							\
		{ return _T( "CNM" ) _T( #eventName ) _T( "Event" ); }

#define	DECLARE_NMSERIALKEY( serialKeyVal )					\
	virtual _UInt32_	GetSerialKeyImpl( void )			\
		{ return ( _UInt32_ )this->serialKeyVal; }

#define	DECLARE_NMRESULT( type, resultVal )					\
	virtual _UInt32_	GetResultImpl( void )				\
		{ return ( _UInt32_ )this->resultVal; }				\
	virtual void		SetResultImpl( _UInt32_ uResult )	\
		{ this->resultVal = ( type )uResult; }

//=========================================
// classes
//=========================================

class CNMMessengerReplyEvent : public CNMEvent
{
public:
	DECLARE_NMEVENT( MessengerReply )

public:
	// event data
	_SInt32_				nResultCode;

	DECLARE_NMSERIALKEY( nResultCode )

public:
	CNMMessengerReplyEvent( void );
	CNMMessengerReplyEvent( _SInt32_ nResultCode );

	DECLARE_SERIALIZABLE
};

class CNMMsgConnectionClosedEvent : public CNMEvent
{
public:
	DECLARE_NMEVENT( MsgConnectionClosed )

	enum
	{
		kType_ByServer		= 0,
		kType_ByError		= 1,
		kType_Gracefully	= 2,
	};

public:
	// event data
	_UInt32_				uType;

public:
	CNMMsgConnectionClosedEvent( void );
	CNMMsgConnectionClosedEvent( _UInt32_ uType );

	DECLARE_SERIALIZABLE
};

class CNMRefreshEvent : public CNMEvent
{
public:
	DECLARE_NMEVENT( Refresh )

	enum
	{
		kType_NULL		= 0,
		kType_MyInfo,
		kType_UserDataList,
		kType_MyGuildList,
		kType_TempNoteBox,
		kType_PermNoteBox,
		kType_GameList,
	};

public:
	// event data
	_UInt32_				uType;

public:
	CNMRefreshEvent( void );
	CNMRefreshEvent( _UInt32_ uType );

	DECLARE_SERIALIZABLE
};

class CNMSpecialEvent : public CNMEvent
{
public:
	DECLARE_NMEVENT( Special )

	enum
	{
		kType_NULL		= 0,
		kType_KillMessenger,
		kType_SwitchDomain,
		kType_BringForward,
	};

public:
	// event data
	_UInt32_	uType;
	TCHAR		szArgument[ 256 ];

public:
	CNMSpecialEvent( void );
	CNMSpecialEvent( _UInt32_ uType, LPCTSTR szArgument );

	DECLARE_SERIALIZABLE
};

class CNMRequestNewFriendEvent : public CNMEvent
{
public:
	DECLARE_NMEVENT( RequestNewFriend )

public:
	// event data
	CNMRequestNewFriendData	rnfData;

public:
	CNMRequestNewFriendEvent( void );
	CNMRequestNewFriendEvent( CNMRequestNewFriendData& rnfData );

	DECLARE_SERIALIZABLE
};

class CNMServerMessageEvent : public CNMEvent
{
public:
	DECLARE_NMEVENT( ServerMessage )

public:
	// event data
	CNMMessageData			msgData;

public:
	CNMServerMessageEvent( void );
	CNMServerMessageEvent( CNMMessageData const & msgData );

	DECLARE_SERIALIZABLE
};

class CNMCustomMessageEvent : public CNMEvent
{
public:
	DECLARE_NMEVENT( CustomMessage )

public:
	// event data
	CNMMessageData			msgData;

public:
	CNMCustomMessageEvent( void );
	CNMCustomMessageEvent( CNMMessageData const & msgData );

	DECLARE_SERIALIZABLE
};

class CNMNoteInstantMsgEvent : public CNMEvent
{
public:
	DECLARE_NMEVENT( NoteInstantMsg )

public:
	// event data
	CNMNoteInfo				niInfo;

public:
	CNMNoteInstantMsgEvent( void );
	CNMNoteInstantMsgEvent( CNMNoteInfo & niInfo );
	CNMNoteInstantMsgEvent( CNMNoteInfo2 & noteInfo );

	DECLARE_SERIALIZABLE
};

class CNMRefreshMessageEvent : public CNMEvent
{
public:
	DECLARE_NMEVENT( RefreshMessage )

public:
	// event data
	NMREFRESHTYPE			uType;

public:
	CNMRefreshMessageEvent( void );
	CNMRefreshMessageEvent( NMREFRESHTYPE uType );

	DECLARE_SERIALIZABLE
};

class CNMFindUserReplyEvent : public CNMEvent
{
public:
	DECLARE_NMEVENT( FindUserReply )

public:
	// event data
	_UInt32_				uToken;
	
public:
	CNMFindUserReplyEvent( void );
	CNMFindUserReplyEvent( _UInt32_ uToken );

	DECLARE_SERIALIZABLE
};

class CNMInviteVirtualUserEvent : public CNMEvent
{
public:
	DECLARE_NMEVENT( InviteVirtualUser )

public:
	// event data
	_UInt64_				uSessionID;
	_UInt32_				uSessionType;
	_UInt32_				uInviteMode;

	NMVirtualKey			keyToVirtual;
	NMVirtualKey			keyFromVirtual;
	TCHAR					szFromLoginID[ LOGINID_SIZE ];
	TCHAR					szFromNickName[ NICKNAME_SIZE ];
	TCHAR					szMessage[ MSG_SIZE ];

	BOOL					bIsHandled;

	DECLARE_NMRESULT( BOOL, bIsHandled )

public:
	CNMInviteVirtualUserEvent( void );
	CNMInviteVirtualUserEvent( _UInt64_ uSessionID, _UInt32_ uSessionType, _UInt32_ uInviteMode, NMVirtualKey keyToVirtual, NMVirtualKey keyFromVirtual, LPCTSTR szFromLoginID, LPCTSTR szFromNickName );

	DECLARE_SERIALIZABLE
};

class CNMUserInfoEvent : public CNMEvent
{
public:
	DECLARE_NMEVENT( UserInfo )

public:
	// event data
	CNMRealUserInfo			ruiInfo;

public:
	CNMUserInfoEvent( void );
	CNMUserInfoEvent( CNMRealUserInfo& ruiInfo );

	DECLARE_SERIALIZABLE
};

class CNMGuildOnlineInfoEvent : public CNMEvent
{
public:
	DECLARE_NMEVENT( GuildOnlineInfo )

public:
	// event data
	CNMGuildOnlineInfo		goiInfo;

public:
	CNMGuildOnlineInfoEvent( void );
	CNMGuildOnlineInfoEvent( CNMGuildOnlineInfo& goiInfo );
	CNMGuildOnlineInfoEvent( CNMGuildOnlineInfoEx& goiInfo );

	DECLARE_SERIALIZABLE
};

class CNMGuildChatMessageEvent : public CNMEvent
{
public:
	DECLARE_NMEVENT( GuildChatMessage )

public:
	// event data
	NMGuildKey			uGuildKey;
	NMIDCode			uFromIDCode;
	BOOL				bIsWhisper;
	CNMChatMessage		cmChatMsg;

public:
	CNMGuildChatMessageEvent( void );
	CNMGuildChatMessageEvent( NMGuildKey uGuildKey, NMIDCode uFromIDCode, BOOL bIsWhisper, CNMChatMessage cmChatMsg );

	DECLARE_SERIALIZABLE
};

class CNMCustomNotifyEvent : public CNMEvent
{
public:
	DECLARE_NMEVENT( CustomNotify )

public:
	// event data
	CNMCustomNotifyData		cnData;

public:
	CNMCustomNotifyEvent( void );
	CNMCustomNotifyEvent( CNMCustomNotifyData& cnData );

	DECLARE_SERIALIZABLE
};

//////////////////////////////////////////////////////////////////////////
//	CNMRejectedUserListChangedEvent
//////////////////////////////////////////////////////////////////////////

class CNMRejectedUserListChangedEvent : public CNMEvent
{
public:
	DECLARE_NMEVENT( RejectedUserListChanged )

public:
	// event data
	// NOTHING!

public:
	CNMRejectedUserListChangedEvent( void );

	DECLARE_SERIALIZABLE
};

//////////////////////////////////////////////////////////////////////////
//	CNMNoteInfoEvent
//////////////////////////////////////////////////////////////////////////

class CNMNoteInfoEvent : public CNMEvent
{
public:
	DECLARE_NMEVENT( NoteInfo )

public:
	//	event data
	CNMNoteInfo2	noteInfo;

public:
	CNMNoteInfoEvent( void );
	CNMNoteInfoEvent( CNMNoteInfo2 & noteInfo );

	DECLARE_SERIALIZABLE
};

//////////////////////////////////////////////////////////////////////////
//	CNMAuthConnectionClosedEvent
//////////////////////////////////////////////////////////////////////////

class CNMAuthConnectionClosedEvent : public CNMEvent
{
public:
	DECLARE_NMEVENT( AuthConnectionClosed )

public:
	enum
	{
		kType_SystemError				= 1,			//	서비스가 중지되었음
		kType_NetworkError				= 2,			//	네트워크 연결에 실패
		kType_InvalidUserIP				= 20014,		//	IP가 패스포트 정보와 맞지 않음
		kType_InvalidPassport			= 20015,		//	패스포트가 맞지 않음
		kType_Disconnected				= 20018,		//	이미 다른 세션에 의해 로그아웃 되었음
	};

public:
	UINT32 uType;

public:
	CNMAuthConnectionClosedEvent( void );
	CNMAuthConnectionClosedEvent( UINT32 uType );

	DECLARE_SERIALIZABLE
};

//////////////////////////////////////////////////////////////////////////
//	CNMAuthSecondaryConnectionClosedEvent
//////////////////////////////////////////////////////////////////////////

class CNMAuthSecondaryConnectionClosedEvent : public CNMEvent
{
public:
	DECLARE_NMEVENT( AuthSecondaryConnectionClosed )

public:
	enum
	{
		kType_SystemError				= 1,			//	서비스가 중지되었음
		kType_NetworkError				= 2,			//	네트워크 연결에 실패
		kType_InvalidUserIP				= 20014,		//	IP가 패스포트 정보와 맞지 않음
		kType_InvalidPassport			= 20015,		//	패스포트가 맞지 않음
		kType_Disconnected				= 20018,		//	이미 다른 세션에 의해 로그아웃 되었음
	};

public:
	UINT32 uType;

public:
	CNMAuthSecondaryConnectionClosedEvent( void );
	CNMAuthSecondaryConnectionClosedEvent( UINT32 uType );

	DECLARE_SERIALIZABLE
};

class CNMGuildOnlineInfoExEvent : public CNMEvent
{
public:
	DECLARE_NMEVENT( GuildOnlineInfoEx )

public:
	// event data
	CNMGuildOnlineInfoEx		goiInfo;

public:
	CNMGuildOnlineInfoExEvent( void );
	CNMGuildOnlineInfoExEvent( CNMGuildOnlineInfoEx& goiInfo );

	DECLARE_SERIALIZABLE
};

class CNMRealFriendInfoChangedEvent : public CNMEvent
{
public:
	DECLARE_NMEVENT( RealFriendInfoChanged )

public:
	// event data
	_UInt32_			uChangeFlag;
	CNMRealFriendInfo	rfiInfo;
	BOOL				bAvatarInfoValid;
	CNMAvatarFullInfo	aiAvatarInfo;

public:
	CNMRealFriendInfoChangedEvent( void );
	CNMRealFriendInfoChangedEvent( _UInt32_ uChangeFlag, CNMRealFriendInfo& rfiInfo );

	DECLARE_SERIALIZABLE
};

class CNMVirtualFriendInfoChangedEvent : public CNMEvent
{
public:
	DECLARE_NMEVENT( VirtualFriendInfoChanged )

public:
	// event data
	_UInt32_				uChangeFlag;
	CNMVirtualFriendInfo	vfiInfo;

public:
	CNMVirtualFriendInfoChangedEvent( void );
	CNMVirtualFriendInfoChangedEvent( _UInt32_ uChangeFlag, CNMVirtualFriendInfo& vfiInfo );

	DECLARE_SERIALIZABLE
};

class CNMFriendInfoChangedEvent : public CNMEvent
{
public:
	DECLARE_NMEVENT( FriendInfoChanged )

public:
	// event data
	CNMFriendInfo			fiFriendInfo;
	BOOL					bAvatarInfoValid;
	CNMAvatarFullInfo		aiAvatarInfo;

public:
	CNMFriendInfoChangedEvent( void );
	CNMFriendInfoChangedEvent( CNMFriendInfo& fiFriendInfo, BOOL bAvatarInfoValid, CNMAvatarFullInfo* paiAvatarInfo );

	DECLARE_SERIALIZABLE
};

class CNMNotifyEvent : public CNMEvent
{
public:
	DECLARE_NMEVENT( Notify )

	enum
	{
		kType_NULL			= 0,
		kType_LogoutVirtual,
		kType_Detach,
	};

public:
	// event data
	_UInt32_	uType;
	_UInt32_	uParam1;
	_UInt32_	uParam2;
	_UInt64_	uParam3;
	_UInt64_	uParam4;
	TCHAR		szParam1[ 1024 ];
	TCHAR		szParam2[ 1024 ];

public:
	CNMNotifyEvent( void );

	DECLARE_SERIALIZABLE
};



class CNMCRChatRoomCreatedEvent : public CNMEvent
{
public:
	DECLARE_NMEVENT( CRChatRoomCreated )

public:
	// event data
	_UInt32_				uSerialKey;
	CNMChatRoomInfo			criInfo;
	CNMChatRoomMemberInfo	crmiMyInfo;

	DECLARE_NMSERIALKEY( uSerialKey )

public:
	CNMCRChatRoomCreatedEvent( void );
	CNMCRChatRoomCreatedEvent( _UInt32_ uSerialKey );
	CNMCRChatRoomCreatedEvent( _UInt32_ uSerialKey, CNMChatRoomInfo& criInfo, CNMChatRoomMemberInfo& crmiMyInfo );

	DECLARE_SERIALIZABLE
};

class CNMCRChatRoomCreatedExEvent : public CNMEvent
{
public:
	DECLARE_NMEVENT( CRChatRoomCreatedEx )

public:
	// event data
	_UInt32_				uSerialKey;
	CNMChatRoomInfoEx		criInfo;
	CNMChatRoomMemberInfo	crmiMyInfo;

	DECLARE_NMSERIALKEY( uSerialKey )

public:
	CNMCRChatRoomCreatedExEvent( void );
	CNMCRChatRoomCreatedExEvent( _UInt32_ uSerialKey );
	CNMCRChatRoomCreatedExEvent( _UInt32_ uSerialKey, CNMChatRoomInfoEx& criInfo, CNMChatRoomMemberInfo& crmiMyInfo );

	DECLARE_SERIALIZABLE
};

class CNMCRChatRoomEstablishedEvent : public CNMEvent
{
public:
	DECLARE_NMEVENT( CRChatRoomEstablished )

public:
	// event data
	_UInt32_				uSerialKey;
	CNMChatRoomInfo			criInfo;
	CNMChatRoomMemberInfo	crmiMyInfo;

	DECLARE_NMSERIALKEY( uSerialKey )

public:
	CNMCRChatRoomEstablishedEvent( void );
	CNMCRChatRoomEstablishedEvent( _UInt32_ uSerialKey );
	CNMCRChatRoomEstablishedEvent( _UInt32_ uSerialKey, CNMChatRoomInfo& criInfo, CNMChatRoomMemberInfo& crmiMyInfo );

	DECLARE_SERIALIZABLE
};

class CNMCRChatRoomEstablishedExEvent : public CNMEvent
{
public:
	DECLARE_NMEVENT( CRChatRoomEstablishedEx )

public:
	// event data
	_UInt32_				uSerialKey;
	CNMChatRoomInfoEx		criInfo;
	CNMChatRoomMemberInfo	crmiMyInfo;

	DECLARE_NMSERIALKEY( uSerialKey )

public:
	CNMCRChatRoomEstablishedExEvent( void );
	CNMCRChatRoomEstablishedExEvent( _UInt32_ uSerialKey );
	CNMCRChatRoomEstablishedExEvent( _UInt32_ uSerialKey, CNMChatRoomInfoEx& criInfo, CNMChatRoomMemberInfo& crmiMyInfo );

	DECLARE_SERIALIZABLE
};

class CNMCRChatRoomErrorEvent : public CNMEvent
{
public:
	DECLARE_NMEVENT( CRChatRoomError )

	enum
	{
		kType_NULL					= 0,
		kType_Failed,
		kType_Closed,
		kType_CreateFailed,
		kType_DeniedByLevel,
		kType_DeniedByMaxSize,
		kType_DeniedByWrongPassword,
		kType_UnidentifiedUser,
		kType_AlreadyOpened,
	};

public:
	// event data
	_UInt32_				uSerialKey;
	_UInt32_				uType;

	DECLARE_NMSERIALKEY( uSerialKey )

public:
	CNMCRChatRoomErrorEvent( void );
	CNMCRChatRoomErrorEvent( _UInt32_ uSerialKey, _UInt32_ uType );

	DECLARE_SERIALIZABLE
};

class CNMCRChatRoomMemberEvent : public CNMEvent
{
public:
	DECLARE_NMEVENT( CRChatRoomMember )

	enum
	{
		kType_NULL			= 0,
		kType_MemberList,
		kType_Joined,
		kType_Leaved,
		kType_Banned,
		kType_InfoChanged,
		kType_RefreshMember,
	};

public:
	// event data
	_UInt32_				uSerialKey;
	_UInt32_				uType;
	NMChatRoomMemberList	aMembers;
	CNMChatRoomMemberInfo	crmiInfo;
	CNMChatRoomMemberInfo	crmiOldInfo;

	DECLARE_NMSERIALKEY( uSerialKey )

public:
	CNMCRChatRoomMemberEvent( void );
	CNMCRChatRoomMemberEvent( _UInt32_ uSerialKey );
	CNMCRChatRoomMemberEvent( _UInt32_ uSerialKey, NMChatRoomMemberList const & aMembers );
	CNMCRChatRoomMemberEvent( _UInt32_ uSerialKey, _UInt32_ uType, CNMChatRoomMemberInfo const & crmiInfo );
	CNMCRChatRoomMemberEvent( _UInt32_ uSerialKey, CNMChatRoomMemberInfo const & crmiInfo, CNMChatRoomMemberInfo const & crmiOldInfo );

	DECLARE_SERIALIZABLE
};

class CNMCRChatRoomInfoEvent : public CNMEvent
{
public:
	DECLARE_NMEVENT( CRChatRoomInfo )

public:
	// event data
	_UInt32_				uSerialKey;
	CNMChatRoomInfo			criInfo;
	CNMChatRoomMemberInfo	crmiMyInfo;

	DECLARE_NMSERIALKEY( uSerialKey )

public:
	CNMCRChatRoomInfoEvent( void );
	CNMCRChatRoomInfoEvent( _UInt32_ uSerialKey );
	CNMCRChatRoomInfoEvent( _UInt32_ uSerialKey, CNMChatRoomInfo& criInfo, CNMChatRoomMemberInfo& crmiMyInfo );
	CNMCRChatRoomInfoEvent( _UInt32_ uSerialKey, CNMChatRoomInfoEx& criInfo, CNMChatRoomMemberInfo& crmiMyInfo );

	DECLARE_SERIALIZABLE
};

class CNMCRChatRoomInfoExEvent : public CNMEvent
{
public:
	DECLARE_NMEVENT( CRChatRoomInfoEx )

public:
	// event data
	_UInt32_				uSerialKey;
	CNMChatRoomInfoEx		criInfo;
	CNMChatRoomMemberInfo	crmiMyInfo;

	DECLARE_NMSERIALKEY( uSerialKey )

public:
	CNMCRChatRoomInfoExEvent( void );
	CNMCRChatRoomInfoExEvent( _UInt32_ uSerialKey );
	CNMCRChatRoomInfoExEvent( _UInt32_ uSerialKey, CNMChatRoomInfoEx& criInfo, CNMChatRoomMemberInfo& crmiMyInfo );

	DECLARE_SERIALIZABLE
};

class CNMCRChatRoomMessageReceivedEvent : public CNMEvent
{
public:
	DECLARE_NMEVENT( CRChatRoomMessageReceived )

public:
	// event data
	_UInt32_				uSerialKey;
	CNMChatMessage			cmMessage;
	BOOL					bIsWhisper;
	BOOL					bIsHandled;

	DECLARE_NMSERIALKEY( uSerialKey )
	DECLARE_NMRESULT( BOOL, bIsHandled )

public:
	CNMCRChatRoomMessageReceivedEvent( void );
	CNMCRChatRoomMessageReceivedEvent( _UInt32_ uSerialKey, CNMChatMessage& cmMessage, BOOL bIsWhisper, BOOL bIsHandled );

	DECLARE_SERIALIZABLE
};

class CNMGSSessionCreatedEvent : public CNMEvent
{
public:
	DECLARE_NMEVENT( GSSessionCreated )

public:
	// event data
	_UInt32_			uSerialKey;
	CNMSessionInfo		siInfo;
	NMSessionMemberList	aMembers;
	BOOL				bWndCreated;
	BOOL				bByMsgRecv;

	DECLARE_NMSERIALKEY( uSerialKey )
	DECLARE_NMRESULT( BOOL, bWndCreated );

public:
	CNMGSSessionCreatedEvent( void );
	CNMGSSessionCreatedEvent( _UInt32_ uSerialKey, CNMSessionInfo& siInfo, NMSessionMemberList& aMembers, BOOL bWndCreated );
	CNMGSSessionCreatedEvent( _UInt32_ uSerialKey, CNMSessionInfo& siInfo, NMSessionMemberList& aMembers, BOOL bWndCreated, BOOL bByMsgRecv );

	DECLARE_SERIALIZABLE
};

class CNMGSSessionEstablishedEvent : public CNMEvent
{
public:
	DECLARE_NMEVENT( GSSessionEstablished )

public:
	// event data
	_UInt32_				uSerialKey;
	CNMSessionInfo			siInfo;
	NMSessionMemberList		aMembers;
	BOOL					bWndCreated;

	DECLARE_NMSERIALKEY( uSerialKey )
	DECLARE_NMRESULT( BOOL, bWndCreated );

public:
	CNMGSSessionEstablishedEvent( void );
	CNMGSSessionEstablishedEvent( _UInt32_ uSerialKey, CNMSessionInfo& siInfo, NMSessionMemberList& aMembers, BOOL bWndCreated );

	DECLARE_SERIALIZABLE
};

class CNMGSSessionFailedEvent : public CNMEvent
{
public:
	DECLARE_NMEVENT( GSSessionFailed )

public:
	// event data
	_UInt32_			uSerialKey;
	CNMSessionInfo		siInfo;

	DECLARE_NMSERIALKEY( uSerialKey )

public:
	CNMGSSessionFailedEvent( void );
	CNMGSSessionFailedEvent( _UInt32_ uSerialKey, CNMSessionInfo& siInfo );

	DECLARE_SERIALIZABLE
};

class CNMGSSessionClosedEvent : public CNMEvent
{
public:
	DECLARE_NMEVENT( GSSessionClosed )

public:
	// event data
	_UInt32_			uSerialKey;
	CNMSessionInfo		siInfo;

	DECLARE_NMSERIALKEY( uSerialKey )

public:
	CNMGSSessionClosedEvent( void );
	CNMGSSessionClosedEvent( _UInt32_ uSerialKey, CNMSessionInfo& siInfo );

	DECLARE_SERIALIZABLE
};

class CNMGSSessionInfoChangedEvent : public CNMEvent
{
public:
	DECLARE_NMEVENT( GSSessionInfoChanged )

public:
	// event data
	_UInt32_			uSerialKey;
	CNMSessionInfo		siInfo;

	DECLARE_NMSERIALKEY( uSerialKey )

public:
	CNMGSSessionInfoChangedEvent( void );
	CNMGSSessionInfoChangedEvent( _UInt32_ uSerialKey, CNMSessionInfo& siInfo );

	DECLARE_SERIALIZABLE
};

class CNMGSNewMemberJoinedEvent : public CNMEvent
{
public:
	DECLARE_NMEVENT( GSNewMemberJoined )

public:
	// event data
	_UInt32_				uSerialKey;
	CNMSessionMemberInfo	smiInfo;

	DECLARE_NMSERIALKEY( uSerialKey )

public:
	CNMGSNewMemberJoinedEvent( void );
	CNMGSNewMemberJoinedEvent( _UInt32_ uSerialKey, CNMSessionMemberInfo& smiInfo );

	DECLARE_SERIALIZABLE
};

class CNMGSMemberLeavedEvent : public CNMEvent
{
public:
	DECLARE_NMEVENT( GSMemberLeaved )

public:
	// event data
	_UInt32_				uSerialKey;
	CNMSessionMemberInfo	smiInfo;

	DECLARE_NMSERIALKEY( uSerialKey )

public:
	CNMGSMemberLeavedEvent( void );
	CNMGSMemberLeavedEvent( _UInt32_ uSerialKey, CNMSessionMemberInfo& smiInfo );

	DECLARE_SERIALIZABLE
};

class CNMGSMemberInfoChangedEvent : public CNMEvent
{
public:
	DECLARE_NMEVENT( GSMemberInfoChanged )

public:
	// event data
	_UInt32_				uSerialKey;
	CNMSessionMemberInfo	smiBefore;
	CNMSessionMemberInfo	smiAfter;

	DECLARE_NMSERIALKEY( uSerialKey )

public:
	CNMGSMemberInfoChangedEvent( void );
	CNMGSMemberInfoChangedEvent( _UInt32_ uSerialKey, CNMSessionMemberInfo& smiBefore, CNMSessionMemberInfo& smiAfter );

	DECLARE_SERIALIZABLE
};

class CNMGSSessionInfoEvent : public CNMEvent
{
public:
	DECLARE_NMEVENT( GSSessionInfo )

public:
	// event data
	_UInt32_				uSerialKey;
	CNMSessionInfo			siInfo;
	NMSessionMemberList		aMembers;
	BOOL					bWndCreated;

	DECLARE_NMSERIALKEY( uSerialKey )
	DECLARE_NMRESULT( BOOL, bWndCreated );

public:
	CNMGSSessionInfoEvent( void );
	CNMGSSessionInfoEvent( _UInt32_ uSerialKey, CNMSessionInfo& siInfo, NMSessionMemberList& aMembers, BOOL bWndCreated );

	DECLARE_SERIALIZABLE
};

class CNMGSInviteRejectedEvent : public CNMEvent
{
public:
	DECLARE_NMEVENT( GSInviteRejected )

public:
	// event data
	_UInt32_				uSerialKey;
	CNMSessionInfo			siInfo;
	TCHAR					szFromLoginID[ LOGINID_SIZE ];
	TCHAR					szFromNickName[ NICKNAME_SIZE ];
	NMREJECTCODE			uRejectCode;

	DECLARE_NMSERIALKEY( uSerialKey )

public:
	CNMGSInviteRejectedEvent( void );
	CNMGSInviteRejectedEvent( _UInt32_ uSerialKey, CNMSessionInfo& siInfo, LPCTSTR szFromLoginID, LPCTSTR szFromNickName, NMREJECTCODE uRejectCode );

	DECLARE_SERIALIZABLE
};


class CNMCSChatMessageReceivedEvent : public CNMEvent
{
public:
	DECLARE_NMEVENT( CSChatMessageReceived )

public:
	// event data
	_UInt32_			uSerialKey;
	CNMChatMessage		cmMessage;
	BOOL				bIsHandled;

	DECLARE_NMSERIALKEY( uSerialKey )
	DECLARE_NMRESULT( BOOL, bIsHandled )

public:
	CNMCSChatMessageReceivedEvent( void );
	CNMCSChatMessageReceivedEvent( _UInt32_ uSerialKey, CNMChatMessage& cmMessage, BOOL bIsHandled );

	DECLARE_SERIALIZABLE
};

class CNMCSMultiChatCreatedEvent : public CNMEvent
{
public:
	DECLARE_NMEVENT( CSMultiChatCreated )

public:
	// event data
	_UInt32_				uSerialKey;
	_UInt32_				uServingProcessID;

	DECLARE_NMSERIALKEY( uSerialKey )

public:
	CNMCSMultiChatCreatedEvent( void );
	CNMCSMultiChatCreatedEvent( _UInt32_ uSerialKey, _UInt32_ uServingProcessID );

	DECLARE_SERIALIZABLE
};

class CNMFUSFileUploadEventReceivedEvent : public CNMEvent
{
public:
	DECLARE_NMEVENT( FUSFileUploadEventReceived )

public:
	// event data
	_UInt32_			uSerialKey;
	NMFileEventList		aEvent;

	DECLARE_NMSERIALKEY( uSerialKey )

public:
	CNMFUSFileUploadEventReceivedEvent( void );
	CNMFUSFileUploadEventReceivedEvent( _UInt32_ uSerialKey, NMFileEventList& aEvent );

	DECLARE_SERIALIZABLE
};

class CNMFDSFileDownloadEventReceivedEvent : public CNMEvent
{
public:
	DECLARE_NMEVENT( FDSFileDownloadEventReceived )

public:
	// event data
	_UInt32_			uSerialKey;
	NMFileEventList		aEvent;

	DECLARE_NMSERIALKEY( uSerialKey )

public:
	CNMFDSFileDownloadEventReceivedEvent( void );
	CNMFDSFileDownloadEventReceivedEvent( _UInt32_ uSerialKey, NMFileEventList& aEvent );

	DECLARE_SERIALIZABLE
};

class CNMWSWhiteBoardMsgReceivedEvent : public CNMEvent
{
public:
	DECLARE_NMEVENT( WSWhiteBoardMsgReceived )

	enum
	{
		kMaxDataLen		= 32768,
	};

public:
	// event data
	_UInt32_			uSerialKey;
	_UInt32_			uBufLen;
	BYTE				aBuffer[ kMaxDataLen ];

	DECLARE_NMSERIALKEY( uSerialKey )

public:
	CNMWSWhiteBoardMsgReceivedEvent( void );
	CNMWSWhiteBoardMsgReceivedEvent( _UInt32_ uSerialKey, BYTE* pBuffer, _UInt32_ uBufLen );

	DECLARE_SERIALIZABLE
};

class CNMWSWhiteBoardAckReceivedEvent : public CNMEvent
{
public:
	DECLARE_NMEVENT( WSWhiteBoardAckReceived )

public:
	// event data
	_UInt32_				uSerialKey;
	_UInt32_				uContentId;

	DECLARE_NMSERIALKEY( uSerialKey )

public:
	CNMWSWhiteBoardAckReceivedEvent( void );
	CNMWSWhiteBoardAckReceivedEvent( _UInt32_ uSerialKey, _UInt32_ uContentId );

	DECLARE_SERIALIZABLE
};

class CNMWSAssocSerialKeyEvent : public CNMEvent
{
public:
	DECLARE_NMEVENT( WSAssocSerialKey )

public:
	// event data
	_UInt32_				uSerialKey;
	_UInt32_				uAssocSerialKey;

	DECLARE_NMSERIALKEY( uSerialKey )

public:
	CNMWSAssocSerialKeyEvent( void );
	CNMWSAssocSerialKeyEvent( _UInt32_ uSerialKey, _UInt32_ uAssocSerialKey );

	DECLARE_SERIALIZABLE
};

//
//	출석체크 이벤트
//
class CNMAttendanceEventEvent : public CNMEvent
{
public:
	DECLARE_NMEVENT( AttendanceEvent )

public:
	NMGameCode	m_uGameCode;
	UINT32		m_uAttendanceDays;

public:
	CNMAttendanceEventEvent();
	CNMAttendanceEventEvent( NMGameCode uGameCode, UINT32 uAttendanceDays );

	DECLARE_SERIALIZABLE
};

///////////////////////////////////////////////////////////////////////////////
//
//	Channel
//
///////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
//	ChannelError
//-----------------------------------------------------------------------------

class CNMChannelErrorEvent : public CNMEvent
{
public:
	DECLARE_NMEVENT( ChannelError )

	enum
	{
		kType_None						= 0,
		kType_ChannelClosed				= 1,
		kType_AuthenticationFailed		= 2,
		kType_AuthorizationFailed		= 3,
		kType_ChannelCreationFailed		= 4,
		kType_ChannelConnectionFailed	= 5,
		kType_NotExistingChannelSN		= 6,
	};

public:
	UINT32						uSerialKey;
	UINT32						uType;

	DECLARE_NMSERIALKEY( uSerialKey )

public:
	CNMChannelErrorEvent();
	CNMChannelErrorEvent( UINT32 uSerialKey, UINT32 uType );

	DECLARE_SERIALIZABLE
};

//-----------------------------------------------------------------------------
//	ChannelCreated
//-----------------------------------------------------------------------------

class CNMChannelCreatedEvent : public CNMEvent
{
public:
	DECLARE_NMEVENT( ChannelCreated )

public:
	UINT32						uSerialKey;
	CNMClientChannelInfo		clientChannelInfo;
	CNMChannelMemberInfo		channelMemberInfo;

	DECLARE_NMSERIALKEY( uSerialKey )

public:
	CNMChannelCreatedEvent();
	CNMChannelCreatedEvent( UINT32 uSerialKey, CNMClientChannelInfo const & clientChannelInfo, CNMChannelMemberInfo const & channelMemberInfo );

	DECLARE_SERIALIZABLE
};

//-----------------------------------------------------------------------------
//	ChannelEstablished
//-----------------------------------------------------------------------------

class CNMChannelEstablishedEvent : public CNMEvent
{
public:
	DECLARE_NMEVENT( ChannelEstablished )

public:
	UINT32						uSerialKey;
	CNMClientChannelInfo		clientChannelInfo;
	CNMChannelMemberInfo		channelMemberInfo;

	DECLARE_NMSERIALKEY( uSerialKey )

public:
	CNMChannelEstablishedEvent();
	CNMChannelEstablishedEvent( UINT32 uSerialKey, CNMClientChannelInfo const & clientChannelInfo, CNMChannelMemberInfo const & channelMemberInfo );

	DECLARE_SERIALIZABLE
};

//-----------------------------------------------------------------------------
//	ChannelInfo
//-----------------------------------------------------------------------------

class CNMChannelInfoEvent : public CNMEvent
{
public:
	DECLARE_NMEVENT( ChannelInfo )

public:
	UINT32						uSerialKey;
	CNMClientChannelInfo		clientChannelInfo;
	CNMChannelMemberInfo		channelMemberInfo;

	DECLARE_NMSERIALKEY( uSerialKey )

public:
	CNMChannelInfoEvent();
	CNMChannelInfoEvent( UINT32 uSerialKey, CNMClientChannelInfo const & clientChannelInfo, CNMChannelMemberInfo const & channelMemberInfo );

	DECLARE_SERIALIZABLE
};

//-----------------------------------------------------------------------------
//	ChannelMemberInfo
//-----------------------------------------------------------------------------

class CNMChannelMemberInfoEvent : public CNMEvent
{
public:
	DECLARE_NMEVENT( ChannelMemberInfo )

	enum Type
	{
		kType_None		= 0,
		kType_Changed	= 1,
		kType_Joined	= 2,
		kType_Leaved	= 3,
		kType_Banned	= 4,
	};

public:
	UINT32						uSerialKey;
	UINT32						uType;
	CNMChannelMemberInfo		channelMemberInfo;

	DECLARE_NMSERIALKEY( uSerialKey )

public:
	CNMChannelMemberInfoEvent();
	CNMChannelMemberInfoEvent( UINT32 uSerialKey, UINT32 uType, CNMChannelMemberInfo const & channelMemberInfo );

	DECLARE_SERIALIZABLE
};

//-----------------------------------------------------------------------------
//	ChannelMemberInfoList
//-----------------------------------------------------------------------------

class CNMChannelMemberInfoListEvent : public CNMEvent
{
public:
	DECLARE_NMEVENT( ChannelMemberInfoList )

public:
	UINT32						uSerialKey;
	NMChannelMemberInfoList		aMember;

	DECLARE_NMSERIALKEY( uSerialKey )

public:
	CNMChannelMemberInfoListEvent();
	CNMChannelMemberInfoListEvent( UINT32 uSerialKey, NMChannelMemberInfoList const & aMember );

	DECLARE_SERIALIZABLE
};

//-----------------------------------------------------------------------------
//	ChannelMessage
//-----------------------------------------------------------------------------

class CNMChannelMessageEvent : public CNMEvent
{
public:
	DECLARE_NMEVENT( ChannelMessage )

public:
	UINT32						uSerialKey;
	CNMChannelMessage			message;

	DECLARE_NMSERIALKEY( uSerialKey )

public:
	CNMChannelMessageEvent();
	CNMChannelMessageEvent( UINT32 uSerialKey, CNMChannelMessage const & channelMessage );
	
	DECLARE_SERIALIZABLE
};

///////////////////////////////////////////////////////////////////////////////
//
//	Client Util
//
///////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
//	LogReportSync
//-----------------------------------------------------------------------------
class CNMLogReportSyncEvent : public CNMEvent
{
public:
	DECLARE_NMEVENT( LogReportSync )

public:
	UINT16		m_uLogType;

public:
	CNMLogReportSyncEvent();
	CNMLogReportSyncEvent( UINT16 uLogReport );

	DECLARE_SERIALIZABLE
};

//-----------------------------------------------------------------------------
//	P2PLoginReply
//-----------------------------------------------------------------------------

class CNMP2PLoginReplyEvent : public CNMEvent
{
public:
	DECLARE_NMEVENT( P2PLoginReply )

public:
	NMP2PReplyCode				nReplyCode;
	NMP2PKey					nP2PKey;

public:
	CNMP2PLoginReplyEvent();
	CNMP2PLoginReplyEvent( NMP2PReplyCode nReplyCode );
	CNMP2PLoginReplyEvent( NMP2PReplyCode nReplyCode, NMP2PKey const & nP2PKey );

	DECLARE_SERIALIZABLE
};

//-----------------------------------------------------------------------------
//	P2PSendDataReply
//-----------------------------------------------------------------------------

class CNMP2PSendDataReplyEvent : public CNMEvent
{
public:
	DECLARE_NMEVENT( P2PSendDataReply )

public:
	UINT32						uSerialKey;
	NMP2PErrorCode				nReplyCode;

public:
	CNMP2PSendDataReplyEvent();
	CNMP2PSendDataReplyEvent( UINT32 uSerialKey, NMP2PErrorCode nReplyCode );

	DECLARE_SERIALIZABLE
};

//-----------------------------------------------------------------------------
//	P2PMessage
//-----------------------------------------------------------------------------

class CNMP2PMessageEvent : public CNMEvent
{
public:
	DECLARE_NMEVENT( P2PMessage )

public:
	CNMP2PMessage message;

public:
	CNMP2PMessageEvent();
	CNMP2PMessageEvent( CNMP2PMessage const & message );
	CNMP2PMessageEvent( NMP2PKey const & sender, NMP2PKey const & receiver, BYTE const * pbyData, UINT32 cbyDataSize );

	DECLARE_SERIALIZABLE
};

//-----------------------------------------------------------------------------
//	P2PConnectionClosed
//-----------------------------------------------------------------------------

class CNMP2PConnectionClosedEvent : public CNMEvent
{
public:
	DECLARE_NMEVENT( P2PConnectionClosed )

public:
	enum Type
	{
		kType_Unknown	= 0,
		kType_ByUser	= 1,
		kType_ByServer	= 2,
		kType_ByError	= 3,
	};

	INT32						nType;

public:
	CNMP2PConnectionClosedEvent();
	CNMP2PConnectionClosedEvent( INT32 nType );

	DECLARE_SERIALIZABLE
};

//=========================================
// Etc.
//=========================================

#define BEGIN_NMEVENT_MAP	switch ( pEvent->GetCode() ) {
#define END_NMEVENT_MAP		default:break; }
#define ON_NMEVENT( eventClass, eventHandler )		\
							case eventClass::kCode: eventHandler( ( eventClass* )pEvent ); pEvent->SetHandled(); break;

class CNMEventReceiver
{
public:
	CNMEventReceiver()			{};
	virtual ~CNMEventReceiver()	{};

	virtual void	OnNMEventNotify( CNMEvent* pEvent )	{ UNREFERENCED_PARAMETER(pEvent); }
	inline void		CallByNMEvent( CNMEvent* pEvent, BOOL bReference = FALSE )
	{
		if ( pEvent != NULL )
		{
			this->OnNMEventNotify( pEvent );
			if ( !bReference )
				delete pEvent;
		}
	}
};

namespace CNMUtil
{
	CNMEvent*			CreateEventFromCode( _UInt32_ uCode );
	CNMEvent*			CreateEventFromCodeAndBuffer( _UInt32_ uCode, BYTE* pBuffer );
	BOOL				ProcessEvent( CNMEventReceiver* pReceiver, _UInt32_ uCode, BYTE* pBuffer, _UInt32_& uResult, _UInt32_ uRemainingCount );
};

#endif	//	#ifndef __NMEVENTOBJECT_H_40D058E4_937B_4f2d_8BA1_5981279D97CE__