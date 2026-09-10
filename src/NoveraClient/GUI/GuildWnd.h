#pragma once

#include "../io3DEngine/ioEdit.h"
#include "../io3DEngine/ioMovingWnd.h"

class SP2Packet;
class ioPlayStage;
class ioUIRenderImage;
class ioUIRenderFrame;
class ioXMLElement;
class ioGuildData;
typedef struct tagGuildRankInfo
{
	DWORD m_dwGuildIndex;
	DWORD m_dwGuildRank;
	DWORD m_dwGuildLevel;
	DWORD m_dwGuildMark;
	ioHashString m_szGuildName;
	DWORD m_dwGuildJoinUser;
	DWORD m_dwGuildMaxUser;
	float m_fGuildBonus;
	int   m_iGuildPoint;

	tagGuildRankInfo()
	{
		m_dwGuildIndex = 0;
		m_dwGuildRank  = 0;
		m_dwGuildLevel = 0;
		m_dwGuildMark  = 0;
		m_dwGuildJoinUser = 0;
		m_dwGuildMaxUser  = 0;
		m_fGuildBonus  = FLOAT1;
		m_iGuildPoint  = 0;
	}
}GuildRankInfo;
class GuildRankingListBtn : public ioButton
{
protected:
	ioUIRenderImage *m_pBlueNormal;
	ioUIRenderImage *m_pRedNormal;
	ioUIRenderImage *m_pBlueOver;
	ioUIRenderImage *m_pRedOver;
	ioUIRenderImage *m_pBlueGradeText;
	ioUIRenderImage *m_pRedGradeText;

protected:
	GuildRankInfo    m_GuildInfo;
	bool             m_bMyGuild;

public:
	void SetGuildInfo( GuildRankInfo &rkInfo );

public:
	DWORD GetGuildIndex(){ return m_GuildInfo.m_dwGuildIndex; }

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();
	void OnRenderBack( int iXPos, int iYPos );

public:
	GuildRankingListBtn();
	virtual ~GuildRankingListBtn();
};
//////////////////////////////////////////////////////////////////////////
class GuildRankingListWnd : public ioWnd
{
public:
	enum
	{
		ID_LEFT_PAGE		 = 1,
		ID_RIGHT_PAGE		 = 2,
		ID_GUILD_CREATE_BTN  = 3,		

		ID_LIST_1       = 100,
		ID_LIST_2       = 101,
		ID_LIST_3       = 102,
		ID_LIST_4       = 103,
		ID_LIST_5       = 104,
		ID_LIST_6       = 105,
		ID_LIST_7       = 106,
		ID_LIST_8       = 107,
		ID_LIST_9       = 108,
		ID_LIST_10      = 109,
	};

	enum
	{
		MAX_PAGE_LIST		= 10,
		GUILD_SEARCH_TIME	= 10000,		
	};

protected:
	DWORD        m_dwTabID;
	int			 m_iCurPage;	
	int          m_iMaxPage;
	DWORD        m_dwSearchTime;

	typedef std::vector< GuildRankInfo > vGuildRankInfo;
	vGuildRankInfo m_vGuildList;

public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	void UpdateSetPage( int iCurPage );
	void ProcessSearch();

public:
	void SetCampRadioBtn( DWORD dwTabID );
	void StartSend( DWORD dwSearchIndex, bool bProcess = false );
	void ApplyGuildList( SP2Packet &rkPacket );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	GuildRankingListWnd();
	virtual ~GuildRankingListWnd();
};

//////////////////////////////////////////////////////////////////////////
class GuildMarkBtn : public ioRadioButton
{
protected:
	ioUIRenderFrame *m_pSlotSelectBtn;
	ioUIRenderFrame *m_pGuildMarkLine;

protected:
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );
	virtual void OnRender();

public:
	GuildMarkBtn();
	virtual ~GuildMarkBtn();
};
//////////////////////////////////////////////////////////////////////////
class GuildCreateWnd : public ioWnd
{
public:
	enum
	{
		ID_EXIT         = 1,
		ID_MARK_SELECT1 = 2,
		ID_MARK_SELECT2 = 3,
		ID_MARK_SELECT3 = 4,
		ID_MARK_SELECT4 = 5,
		ID_MARK_SELECT5 = 6,
		ID_MARK_SELECT6 = 7,
		ID_MARK_SELECT7 = 8,
		ID_MARK_SELECT8 = 9,
		ID_MARK_SELECT9 = 10,
		ID_MARK_SELECT10= 11,
		
		ID_NAME_EDIT    = 100,
		ID_CREATE_BTN   = 101,
		ID_CANCEL_BTN   = 102,
	};

protected:
	ioUIRenderFrame *m_pSlotWhite;

protected:
	ioWnd *m_pPreEdit;
	ioWnd *m_pNameEdit;
	DWORD  m_dwMarkID;


	bool IsOnlyEngHanNumText( const char *szText );     //영문, 한글, 숫자만 가능
	void CreateGuild();

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void iwm_show();	
	virtual void iwm_hide();
	virtual bool iwm_esc();

protected:
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	GuildCreateWnd();
	virtual ~GuildCreateWnd();
};
//////////////////////////////////////////////////////////////////////////
class GuildEventPopup : public ioWnd
{
public:
	enum
	{
		ID_EXIT         = 1,
		ID_GUILD_VIEW   = 2,
		ID_OK           = 3,
	};

protected:
	ioUIRenderFrame *m_pGuildMarkUpper;

	static DWORD sg_dwGuildIndex;
	static DWORD sg_dwGuildMark;
	static ioHashString sg_szGuildName;
	static int sg_iEventType;

public:
	static void SetEventType( int iEventType, DWORD dwGuildIndex, const ioHashString &szGuildName, DWORD dwGuildMark );

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual bool iwm_esc();
		
protected:
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();
	void OnOkProcess();

public:
	GuildEventPopup();
	virtual ~GuildEventPopup();
};
//////////////////////////////////////////////////////////////////////////
class GuildInfoUserOptionWnd : public ioWnd
{
public:
	enum
	{
		OPTION_INFO			= 1000,
		OPTION_MASTER		= 1001,
		OPTION_SECOND_MASTER_DISMISSAL = 1002,
		OPTION_SECOND_MASTER = 1003,
		OPTION_KICK_OUT      = 1004,
		OPTION_OPEN_SLOT     = 1005,
		OPTION_CLOSE_SLOT    = 1006,
	};

protected:
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

protected:
	ioGuildData *m_pGuildData;
	ioHashString m_szUserID;
	ioHashString m_szUserPos;
	int          m_iUserLevel;
	DWORD        m_dwSlotArray;
	DWORD		 m_dwMsgCmd;


	typedef std::map< DWORD, ioHashString > OptionMap;
	OptionMap m_OptionMap;

public:
	virtual void ParseExtraInfo( ioXMLElement &xElement );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

public:
	void SetOptionInfo( ioGuildData *pGuildData, const ioHashString &szUserID, const ioHashString &szUserPos, int iUserLevel, DWORD dwSlotArray );

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
	GuildInfoUserOptionWnd();
	virtual ~GuildInfoUserOptionWnd();
};
//////////////////////////////////////////////////////////////////////////
class GuildInfoUserBtn : public ioButton
{
protected:
	ioUIRenderImage *m_pMasterMark;
	ioUIRenderImage *m_pSecondMasterMark;
	ioUIRenderImage *m_pPeopleOrange;
	ioUIRenderImage *m_pPeopleGray;

    ioGuildData  *m_pGuildData;
	DWORD        m_dwBtnArray;
	DWORD		 m_dwTableIndex;
	DWORD		 m_dwUserIndex;
	int			 m_iUserLevel;
	ioHashString m_szUserID;
	ioHashString m_szUserPos;

public:
	void MemberInfo( ioGuildData *pGuildData, int iListArray, DWORD dwTableIndex, DWORD dwUserIndex, int iUserLevel, const ioHashString &szUserID, const ioHashString &szUserPos );

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();	

public:
	const ioHashString &GetUserID(){ return m_szUserID; }
	const ioHashString &GetUserPos(){ return m_szUserPos; }
	int GetUserLevel(){ return m_iUserLevel; }
	DWORD GetSlotArray(){ return m_dwBtnArray; }

public:
	GuildInfoUserBtn();
	virtual ~GuildInfoUserBtn();
};
//////////////////////////////////////////////////////////////////////////
class GuildInfoUserListWnd : public ioWnd
{
public:
	enum
	{
		ID_USER_LIST_1   = 0,              //32개의 유저 버튼이 있다.
		ID_USER_LIST_32  = 31,

		ID_OPTION_WND = 1000,
	};

protected:
	ioGuildData *m_pGuildData;
	
public:
	void UpdateUserList( ioGuildData *pGuildData );

public:
	virtual void iwm_show();
	virtual bool iwm_esc();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnProcess( float fTimePerSec );

public:
	GuildInfoUserListWnd();
	virtual ~GuildInfoUserListWnd();
};
//////////////////////////////////////////////////////////////////////////
class GuildInfoWnd : public ioWnd
{
public:
	enum
	{
		ID_GUILD_HOMEPAGE	= 1,
		ID_MARK_CHANGE		= 2,
		ID_TITLE_CHANGE		= 3,
		ID_ENTRY_APP		= 4,
		ID_ENTRY_CANCEL		= 5,
		ID_ENTRY_INVITATION = 6,
		ID_ENTRY_DELAY_USER = 7,
		//ID_GUILD_HELP   	= 8,
		ID_GUILD_LEAVE		= 9,		

		//Guild attendance reward And Rank Reward Info
		ID_RANK_REWARD_INFO	  = 10,
		ID_ATTEND_REWARD_INFO = 11,
		ID_ATTEND_CHECK		  = 12,
		ID_ACCEPT_REWARD	  = 13,
		ID_GUILD_HEADQUATER   = 14,
	};

	enum
	{
		RANK_REWARD		= 1,
		ATTEND_REWARD	= 2,
	};

protected:
	ioUIRenderFrame *m_pGuildMarkFrame;

protected:
	DWORD m_dwGuildHelpID;

protected:
	ioGuildData *m_pGuildData;

public:
	void UpdateGuildInfo( ioGuildData *pGuildData );

public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );
	virtual void ParseExtraInfo( ioXMLElement &xElement );
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();
	virtual void OnRenderTitle( int iXPos, int iYPos );
	
public:
	GuildInfoWnd();
	virtual ~GuildInfoWnd();
};
//////////////////////////////////////////////////////////////////////////
class GuildMainWnd : public ioMovingWnd
{
public:
	enum
	{
		ID_EXIT							= 1,
		//other guild	
		ID_GUILD_INFO_BTN				= 2,
		ID_GUILD_USER_BTN				= 3,

		//myguild
		ID_GUILD_INFO_BTN2				= 4,
		ID_GUILD_USER_BTN2				= 5,
		ID_GUILD_ATTEND_LIST_BTN		= 6,
		ID_GUILD_HOME_ADMIN_LIST_BTN	= 7,

		ID_GUILD_INFO_WND				= 10,
		ID_GUILD_USER_WND				= 11,
		ID_GUILD_ATTEND_WND				= 12,
		ID_GUILD_ADMIN_WND				= 13,

		ID_GUILD_HOUSING_INVEN			= 100,
	};

	enum
	{
		GUILD_LEAVE = 1000,
	};

protected:
	DWORD			m_dwTabID;
	bool			m_bIsMyGuild;
	ioUIRenderImage *m_pTabLine;

protected:
	ioGuildData		*m_pGuildData;
	DWORD			m_dwGuildIndex;

protected:
	void SetRadioButton( DWORD dwTabID );

public:
	virtual void iwm_create();
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual bool iwm_esc();

	virtual void OnProcess( float fTimePerSec );
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

protected:
	void UpdateTitle();

protected:
	virtual void OnRender();

public:
	void  SetGuildIndex( DWORD dwGuildIndex );
	void  UpdateGuildInfo();
	DWORD GetGuildIndex(); 
	void  GuildLeaveProcess();

public:
	GuildMainWnd();
	virtual ~GuildMainWnd();
};
//////////////////////////////////////////////////////////////////////////
class GuildEntryDelayUserBtn : public ioButton
{
public:
	enum
	{
		ID_AGREE   = 1,
		ID_REFUSE  = 2,
	};	

protected:
	DWORD m_dwGuildIndex;
	DWORD m_dwTableIndex;
	DWORD m_dwUserIndex;
	int	  m_iUserLevel;
	ioHashString m_szUserID;
    int   m_iMaxList;
	DWORD m_dwMsgBoxID;
public:  
	void MemberInfo( int iMaxList, DWORD dwGuildIndex, DWORD dwTableIndex, DWORD dwUserIndex, int iUserLevel, const ioHashString &szUserID );

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnRender();	
	virtual void OnProcess( float fTimePerSec );

public:
	ioHashString &GetMemberName(){ return m_szUserID; }

public:
	GuildEntryDelayUserBtn();
	virtual ~GuildEntryDelayUserBtn();
};

class GuildEntryDelayUserWnd : public ioMovingWnd
{
public:
	enum
	{
		ID_EXIT				= 1,
		ID_LEFT_BTN			= 2,
		ID_RIGHT_BTN		= 3,
		ID_AUTO_SHOW_IGNORE = 4,

		ID_LIST_1	= 10,
		ID_LIST_2	= 11,
		ID_LIST_3	= 12,
		ID_LIST_4	= 13,
		ID_LIST_5	= 14,
	};	

	enum
	{
		MAX_LIST = 5,
		AUTO_SHOW_HEIGHT = 249,
		CUSTOM_SHOW_HEIGHT = 201,
	};

protected:
	ioUIRenderFrame *m_pBottomFrm;

protected:
	static bool sg_bAutoShowIgnore;

protected:
	int  m_iCurPage;
	int  m_iMaxPage;
	bool m_bAutoShow;

	DWORD m_dwGuildIndex;
	struct EntryUser
	{
		DWORD m_dwTableIndex;
		DWORD m_dwUserIndex;
		int   m_iUserLevel;
		ioHashString m_szUserID;

		EntryUser()
		{
			m_dwTableIndex = 0;
			m_dwUserIndex  = 0;
			m_iUserLevel   = 0;
		}
	};
	typedef std::vector< EntryUser> vEntryUser;
	vEntryUser m_vUserList;

	void UpdateSetPage( int iCurPage );
	void ReSizeWnd();

public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();	

public:
	void SetAutoShow( bool bAutoShow ){ m_bAutoShow = bAutoShow; }
	void SetEntryDelayUser( SP2Packet &rkPacket );
	void DeleteUserList( DWORD dwTableIndex );

public:
	virtual void SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate = false );

public:
	GuildEntryDelayUserWnd();
	virtual ~GuildEntryDelayUserWnd();
};
//////////////////////////////////////////////////////////////////////////
class GuildInvitationFriendWnd : public ioWnd
{
public:
	enum
	{
		ID_VERT_SCROLL = 1000,		
	};

	enum
	{
		MAX_PAGE_LIST = 5,
	};

protected:
	ioUIRenderImage *m_pDarkLine;
	ioUIRenderImage *m_pLightLine;
	ioUIRenderImage *m_pOverLine;

	int   m_iAddWidth;
	int   m_iAddHeight;
	int   m_iListXPos;
	int   m_iListYPos;
	int   m_iHeightGap;
	int   m_iCutWidth;
	int   m_iStartXPos;
	int   m_iStartYPos;

protected:
	ioHashStringVec m_OnlineUserList;
	ioHashString    m_szOverListUser;
	int				m_iCurPos;	

public:
	virtual void ParseExtraInfo( ioXMLElement &xElement );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_wheel( int zDelta );
	virtual void iwm_vscroll( DWORD dwID, int curPos );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	const ioHashString &GetOverUser(){ return m_szOverListUser; }

public:
	GuildInvitationFriendWnd();
	virtual ~GuildInvitationFriendWnd();
};
//////////////////////////////////////////////////////////////////////////
class GuildInvitationWnd : public ioWnd
{
public:
	enum
	{
		ID_EXIT         = 1,
		ID_SEND_BTN		= 2,
		ID_CANCEL_BTN   = 3,
		ID_FRIEND_BTN   = 4,

		ID_NAME_EDIT    = 100,
		ID_FRIEND_LIST  = 1000,
	};

protected:
	ioWnd *m_pPreEdit;
	ioWnd *m_pNameEdit;

	void SendInvitation();
public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void iwm_show();	
	virtual void iwm_hide();
	virtual bool iwm_esc();

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	GuildInvitationWnd();
	virtual ~GuildInvitationWnd();
};
//////////////////////////////////////////////////////////////////////////
class GuildInvitedWnd : public ioMovingWnd
{
public:
	enum
	{
		ID_EXIT           = 1,
		ID_INFO_BTN		  = 2,
		ID_REFUSE_BTN	  = 3,
		ID_INVITED_IGNORE = 4,
	};

protected:
	ioUIRenderFrame *m_pGuildMarkUpper;

protected:
	static DWORD m_dwGuildIndex;
	static DWORD m_dwGuildMark;
	static ioHashString m_szGuildName;
	static ioHashString m_szUserName;
	static int   m_iUserLevel;
	static bool  sg_bInvitedIgnore;

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual bool iwm_esc();
	virtual void iwm_show();

protected:
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	void SetInvitedInfo( SP2Packet &rkPacket );

public:
	GuildInvitedWnd();
	virtual ~GuildInvitedWnd();
};
//////////////////////////////////////////////////////////////////////////
class GuildTitleChangeWnd : public ioWnd
{
public:
	enum
	{
		ID_EXIT		= 1,
		ID_CHANGE	= 2,
		ID_CANCEL   = 3,
		
		ID_EDIT_LINE = 100,
	};

protected:
	ioWnd *m_pPreEdit;

protected:
	void ChangeTitle();

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void iwm_show();	
	virtual void iwm_hide();
	virtual bool iwm_esc();

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	GuildTitleChangeWnd();
	virtual ~GuildTitleChangeWnd();
};
//////////////////////////////////////////////////////////////////////////
class GuildEtcItemPopup : public ioWnd
{
public:
	enum
	{
		ID_EXIT			   = 1,
		ID_CREATE_ITEM_BUY = 2,
		ID_CREATE_ITEM_USE = 3,
		ID_MARK_CHANGE_ITEM_BUY = 4,
		ID_MARK_CHANGE_ITEM_USE = 5,
		ID_CANCEL = 6,
	};

protected:
	ioUIRenderImage *m_pNeedIconBack;
	ioUIRenderImage *m_pUseIconBack;
	ioUIRenderImage *m_pGuildCreateIcon;
	ioUIRenderImage *m_pGuildMarkChangeIcon;

protected:
	DWORD m_dwPopupBtnID;

public:
	void ShowGuildCreate();
	void ShowGuildMarkChange();

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void OnRender();

public:
	GuildEtcItemPopup();
	virtual ~GuildEtcItemPopup();
};
//////////////////////////////////////////////////////////////////////////
class GuildNameChangeWnd : public ioWnd
{
public:
	enum
	{
		ID_OK = 1,
	};

protected:
	ioUIRenderImage *m_pIcon;
	ioUIRenderImage *m_pSubIcon;
	ioUIRenderImage *m_pBackEffect;

	ioHashString m_szPreGuildName;
	ioHashString m_szNewGuildName;

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void OnRender();

public:
	void SetInfoAndShow( const ioHashString &szPreName, const ioHashString &szNewName );

public:
	GuildNameChangeWnd();
	virtual ~GuildNameChangeWnd();
};
