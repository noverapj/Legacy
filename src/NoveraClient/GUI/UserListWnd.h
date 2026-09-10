#pragma once

class CloverButton : public ioButton
{
protected:
	bool m_bActive;
	ioUIRenderImage *m_pInActiveNormal;
	ioUIRenderImage *m_pInActiveOvered;
	ioUIRenderImage *m_pInActiveOveredAdd;
	ioUIRenderImage *m_pInActivePushed;

public:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

public:
	virtual void iwm_create();
	virtual void iwm_show();
	virtual void iwm_mouseover( const ioMouse& mouse );
	virtual void iwm_mouseleave( const ioMouse& mouse );

protected:
	virtual void OnDrawNormal( int iXPos, int iYPos );
	virtual void OnDrawOvered( int iXPos, int iYPos );
	virtual void OnDrawOveredAdd( int iXPos, int iYPos );
	virtual void OnDrawPushed( int iXPos, int iYPos );

public:
	virtual void SetActive();
	virtual void SetInActive();
	bool IsActivate() { return m_bActive; }

public:
	CloverButton();
	~CloverButton();
};

class UserInfoListWnd : public ioButton
{
public:
	enum
	{
		ID_RECV_CLOVER = 1,
		ID_SEND_CLOVER = 2,
	};

	enum
	{
		TYPE_FRIENDLIST    = 1,
		TYPE_BLACKLIST     = 2,
		TYPE_GUILDUSERLIST = 3,
	};

protected:
	ioUIRenderImage *m_pBlackMark;
	ioUIRenderImage *m_pMyBestFriendBlack;
	ioUIRenderImage *m_pMyBestFriendGray;
	ioUIRenderImage *m_pOtherBestFriendBlack;
	ioUIRenderImage *m_pOtherBestFriendGray;
	ioUIRenderImage *m_pCoupleBestFriendBlack;
	ioUIRenderImage *m_pCoupleBestFriendGray;

	ioHashString m_szUserName;
	bool         m_bOnline;
	int          m_iUserPos;
	int          m_iUserLevel;
	bool		 m_bMyBestFriend;
	bool         m_bOtherBestFriend;
	ioPlayStage *m_pPlayStage;
	int          m_iListType;

protected:
	float GetNameCut();

public:
	void SetFriendUserInfo( ioHashString szUserName, bool bOnline, int iUserPos, int iUserLevel, bool bMyBestFriend, bool bOtherBestFriend, ioPlayStage *pPlayStage );
	void SetGuildUserInfo( ioHashString szUserName, bool bOnline, int iUserPos, int iUserLevel, ioPlayStage *pPlayStage );
	void SetBlackListUserInfo( ioHashString szUserName, bool bOnline );
	const ioHashString &GetUserName(){ return m_szUserName; }
	bool IsOnline(){ return m_bOnline; }

public:
	virtual void iwm_create();
	virtual void iwm_show();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	
public:
	bool CheckInvite( bool bAlarm );
	bool CheckMove( bool bAlarm );

public:
	const ioHashString &GetUserID() { return m_szUserName; }
	const int  GetUserPosition()    { return m_iUserPos; }
	const bool IsBestFriend()       { return m_bMyBestFriend; }
	const int  GetListType()        { return m_iListType; }
	const int  GetUserLevel()       { return m_iUserLevel; }

protected:
	bool SendClover();
	void RecvClover();

public:
	UserInfoListWnd();
	virtual ~UserInfoListWnd();
};

// 친구 + 길드원의 리스트 합
typedef struct tagUserListInfo
{
	ioHashString m_szUserID;
	bool         m_bOnline;
	int          m_iUserPos;
	int          m_iGradeLevel;
	bool         m_bMyBestFriend;
	bool         m_bOtherBestFriend;
	int          m_iRecvWaitClover;
	int          m_iRecvClover;
	int          m_iSendClover;
	tagUserListInfo()
	{
		m_bOnline	 = false;
		m_bMyBestFriend    = false;
		m_bOtherBestFriend = false;
		m_iUserPos	 = 1;
		m_iGradeLevel= 0;
		m_iRecvWaitClover = 0;
		m_iRecvClover = 0;
		m_iSendClover = 0;
	}
}UserListInfo;

class UserListInfoSort : public std::binary_function< UserListInfo &, UserListInfo &, bool >
{
public:
	bool operator()( UserListInfo &lhs , UserListInfo &rhs ) const
	{
		if( (int)lhs.m_bOnline > (int)rhs.m_bOnline )
		{
			return true;
		}
		else if( lhs.m_bOnline == rhs.m_bOnline )
		{
			if( (int)lhs.m_iRecvWaitClover > (int)rhs.m_iRecvWaitClover )
			{
				return true;
			}
			else if( (int)lhs.m_iRecvWaitClover == (int)rhs.m_iRecvWaitClover )
			{
				if( (int)lhs.m_iRecvClover > (int)rhs.m_iRecvClover )
				{
					return true;
				}
				else if( (int)lhs.m_iRecvClover == (int)rhs.m_iRecvClover )
				{
					if( (int)lhs.m_iSendClover > (int)rhs.m_iSendClover )
					{
						return true;
					}
					else if( (int)lhs.m_iSendClover == (int)rhs.m_iSendClover )
					{
						if( (int)lhs.m_bMyBestFriend + (int)lhs.m_bOtherBestFriend > (int)rhs.m_bMyBestFriend + (int)rhs.m_bOtherBestFriend )
						{
							return true;
						}
						else if( (int)lhs.m_bMyBestFriend + (int)lhs.m_bOtherBestFriend == (int)rhs.m_bMyBestFriend + (int)rhs.m_bOtherBestFriend )
						{
							if( (int)lhs.m_bMyBestFriend > (int)rhs.m_bMyBestFriend )
							{
								return true;
							}
							else if( (int)lhs.m_bMyBestFriend == (int)rhs.m_bMyBestFriend )
							{
								if( (int)lhs.m_bOtherBestFriend > (int)rhs.m_bOtherBestFriend )
								{
									return true;
								}
								else if( (int)lhs.m_bOtherBestFriend == (int)rhs.m_bOtherBestFriend )
								{
									if( lhs.m_iGradeLevel > rhs.m_iGradeLevel )
									{
										return true;
									}
									else if( lhs.m_iGradeLevel == rhs.m_iGradeLevel )
									{
										int iCmpValue = stricmp( lhs.m_szUserID.c_str(), rhs.m_szUserID.c_str() );     
										if( iCmpValue < 0 )
											return true;
									}
								}					
							}				
						}
					}
				}
			}
		}
		return false;
	}
};

class UserListWnd : public ioWnd
{
public:
	enum
	{
		//BTN
		ID_FRIEND_LIST = 1,
		ID_GUILD_LIST  = 2,
		ID_BLACK_LIST  = 3,

		ID_FRIEND_INSERT	= 10,
		ID_BESTFRIEND_INSERT= 11,
		ID_GUILD_INFO		= 12,
		ID_GUILD_HOME_PAGE	= 13,
		ID_BLACK_INSERT     = 14,
		ID_LEFT_PAGE		= 15,
		ID_RIGHT_PAGE       = 16,

		//USER LIST
		ID_USER_LIST_1 = 100,
		ID_USER_LIST_2 = 101,
		ID_USER_LIST_3 = 102,
		ID_USER_LIST_4 = 103,
		ID_USER_LIST_5 = 104,
		ID_USER_LIST_6 = 105,
		ID_USER_LIST_7 = 106,
		ID_USER_LIST_8 = 107,
		ID_USER_LIST_9 = 108,

		ID_OPTION_WND = 1000,
		ID_TOOLTIP    = 1001,
	};

	enum
	{
		MAX_LIST_SIZE = 9,
	};

protected:
	int m_iTabID;
	int m_iCurPage;
	int m_iMaxPage;
	DWORD m_dwChangePageTimer;
	DWORD m_dwUserPosRefreshTimer;
	ioPlayStage *m_pPlayStage;

	typedef std::vector< UserListInfo > vUserListInfo;
	vUserListInfo m_UserList;

	void CheckButton( DWORD dwID );
	void UpdateSetPage( int iNewPage );
	void SendUserPosRefresh();

public:
	void InviteResult( int iResult, ioHashString szInvitedName );
	void ChannelCreate( int iResult, ioHashString szManToManName );
	bool IsChatBlock();

	int GetTabID() const { return m_iTabID; }
	void SetPlayStage( ioPlayStage *pPlayStage );

public:
	void UpdateUserList();

public:
	virtual void iwm_create();
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual bool iwm_esc();

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	void OnInsertBestFriend( const ioHashString & szUserName );
	bool CheckDeleteBestFriend( const ioHashString & szUserName );
	void _OnDeleteBestFriend( const ioHashString & szUserName );

public:
	static bool sg_bUserListUpdate;

public:
	UserListWnd();
	virtual ~UserListWnd();
};
//////////////////////////////////////////////////////////////////////////
class LobbyFriendListWnd : public ioWnd
{
public:
	enum
	{
		ID_LEFT_BTN  = 1,
		ID_RIGHT_BTN = 2,
		ID_HELP_BTN  = 3,

		ID_LIST_1    = 10,
		ID_LIST_2    = 11,
		ID_LIST_3    = 12,
		ID_LIST_4    = 13,
		ID_LIST_5    = 14,

		ID_OPTION_WND= 1000,
	};

	enum
	{
		MAX_PAGE_LIST = 5,
	};

protected:
	typedef std::vector< UserListInfo > vUserListInfo;
	vUserListInfo m_UserList;


protected:
	int m_iCurPage;
	int m_iMaxPage;
	DWORD m_dwChangePageTimer;
	DWORD m_dwUserPosRefreshTimer;
	DWORD m_dwFriendManual;

public:
	static bool sg_bFriendListUpdate;
	static void ChangeFriendList(){ sg_bFriendListUpdate = true; }

public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );	

public:
	virtual void ParseExtraInfo( ioXMLElement &xElement );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	void UpdateFriendList( int iCurPage );
	void SendUserPosRefresh();

public:
	LobbyFriendListWnd();
	virtual ~LobbyFriendListWnd();
};
//////////////////////////////////////////////////////////////////////////
class UserInfoListOptionWnd : public ioWnd
{
public:
	enum
	{
		OPTION_INFO					= 1000,
		OPTION_ADD_BEST_FRIEND		= 1001,
		OPTION_RELEASE_BEST_FRIEND	= 1002,
		OPTION_BLACK_DELETE			= 1003,
		OPTION_FOLLOW				= 1004,
		OPTION_INVITE				= 1005,
	};

protected:
	ioPlayStage     *m_pPlayStage;
	ioUIRenderImage *m_pDarkLine;
	ioUIRenderImage *m_pLightLine;
	ioUIRenderImage *m_pOverLine;

	int   m_iAddWidth;
	int   m_iAddHeight;
	int   m_iListXPos;
	int   m_iListYPos;
	int   m_iHeightGap;
	int   m_iStartXPos;
	int   m_iStartYPos;
	int   m_iOverListIndex;

	DWORD m_dwMsgCmd;

protected:
	ioHashString m_szUserID;
	int          m_iUserLevel;
	bool         m_bBestFriend;
	int          m_iUserPos;
	int          m_iListType;
	bool         m_bOnLine;

	bool         m_bEnableMove;
	bool         m_bEnableInvite;
	
	typedef std::map< DWORD, ioHashString > OptionMap;
	OptionMap m_OptionMap;

public:
	virtual void ParseExtraInfo( ioXMLElement &xElement );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

public:
	void SetOptionInfo( ioPlayStage *pStage, const ioHashString &szUserID, int iUserLevel, bool bBestFriend, int iUserPos, int iListType, bool bOnLine, bool bEnableMove, bool bEnableInvite );

protected:
	void CommandOption( DWORD dwCmd );

public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	UserInfoListOptionWnd();
	virtual ~UserInfoListOptionWnd();
};
//////////////////////////////////////////////////////////////////////////