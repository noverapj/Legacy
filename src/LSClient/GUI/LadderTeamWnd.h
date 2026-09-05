#pragma once

#include "../ioLadderTeamMgr.h"

#include "GlobalWnd.h"

class LadderTeamListBtn : public ioButton
{
protected:
	ioUIRenderImage *m_pBlueTeamMark;
	ioUIRenderImage *m_pRedTeamMark;
	ioUIRenderImage *m_pLockKey;
	ioUIRenderImage *m_pLockKeyGray;
	ioUIRenderImage *m_pIconEmpty;

	LadderTeamInfo m_LadderTeamInfo;
	ioHashString   m_szState;

public:
	void SetLadderTeamInfo( LadderTeamInfo &rkInfo );
	void SetTeamIcon( ioUIRenderImage* pBlueIcon, ioUIRenderImage* pRedIcon );

public:
	DWORD GetLadderTeamIndex(){ return m_LadderTeamInfo.m_dwTeamIndex; }
	LadderTeamInfo &GetLadderTeamInfo(){ return m_LadderTeamInfo; }

protected:
	virtual void OnRender();
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	

public:
	LadderTeamListBtn();
	virtual ~LadderTeamListBtn();
};
//////////////////////////////////////////////////////////////////////////
class LadderTeamCreateBtn : public ioButton
{
protected:
	ioUIRenderImage *m_pBtnIcon;

	ioHashString m_szTitle;
	CampType m_eCampType;
	
protected:
	virtual void OnRender();
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	

public:
	void SetText( ioHashString szTitle, CampType eCampType );

public:
	LadderTeamCreateBtn();
	~LadderTeamCreateBtn();
};
//////////////////////////////////////////////////////////////////////////
class LadderTeamListWnd : public ioWnd
{
public:
	enum
	{
		ID_BLUE_TEAM_CREATE     = 1,
		ID_RED_TEAM_CREATE      = 2,
		ID_LEFT_PAGE            = 3,
		ID_RIGHT_PAGE           = 4,
		ID_BLUE_LADDER_RANKING	= 5,
		ID_RED_LADDER_RANKING	= 6,

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
		LOBBY_MAX_TEAM_LIST = 5,
		PLAZA_MAX_TEAM_LIST = 10,
		TEAM_SEARCH_TIME	= 10000,		
	};

protected:
	int	  m_iCurPage;	
	int   m_iMaxPage;
	int   m_iPageYPos;
	DWORD m_dwSearchTime;
	DWORD m_dwSelectIndex;
	DWORD m_dwMaxTeamList;

	typedef std::vector< LadderTeamInfo > vLadderTeamInfo;
	vLadderTeamInfo m_vLadderTeamList;

public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	void UpdateSetPage( int iCurPage );
	void ProcessSearch();
	
public:
	void StartSend( bool bProcess = false );
	void ApplyLadderTeamList( SP2Packet &rkPacket );
	void OnUpdateProcess();	

public:
	void PlazaUIMove();
	inline void SetMaxTeamList( DWORD dwMax ){ m_dwMaxTeamList = dwMax; }

protected:
	void UpdateMainUI();

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	LadderTeamListWnd();
	virtual ~LadderTeamListWnd();
};
//////////////////////////////////////////////////////////////////////////
class LadderTeamInfoBtn : public ioButton
{
protected:
	ioUIRenderImage *m_pBlueNormal;
	ioUIRenderImage *m_pRedNormal;
	ioUIRenderImage *m_pBlueOver;
	ioUIRenderImage *m_pRedOver;
	ioUIRenderImage *m_pPeopleOrange;
	ioUIRenderImage *m_pPeopleGray;

protected:
	TeamType m_eTeam;
	ioHashString m_szName;
	int   m_iGradeLevel;
	int   m_iLadderPoint;
	int   m_iPingStep;
	int   m_iSlotArray;
	int   m_iMaxSlot;

protected:
	PingStepWnd m_PingStep;

public:
	virtual void iwm_create();

public:
	void SetMemberInfo( int iMaxSlot, int iSlotArray, const ioHashString &rkName, int iGradeLevel, int iPingStep, int iLadderPoint, TeamType eTeam );
	const ioHashString &GetMemberName(){ return m_szName; }

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void OnRender();
	virtual void OnProcess( float fTimePerSec );
	void OnRenderBase( int iXPos, int iYPos );
	void OnRenderPing( int iXPos, int iYPos );

public:
	LadderTeamInfoBtn();
	virtual ~LadderTeamInfoBtn();
};
//////////////////////////////////////////////////////////////////////////
class LadderTeamInfoWnd : public ioMovingWnd
{
public:
	enum
	{
		ID_EXIT    = 1,
		ID_ENTER   = 2,
		ID_CANCEL  = 3,
	
		ID_LIST_1  = 10,
		ID_LIST_2  = 11,
		ID_LIST_3  = 12,
		ID_LIST_4  = 13,
		ID_LIST_5  = 14,
		ID_LIST_6  = 15,
		ID_LIST_7  = 16,
		ID_LIST_8  = 17,

		ID_DEVELOPER_ENTER = 1000,
	};

	enum
	{
		MAX_LIST   = 8,
	};

protected:
	ioUIRenderImage *m_pModeIconBack;
	ioUIRenderImage *m_pBlueCampIcon;
	ioUIRenderImage *m_pRedCampIcon;

	LadderTeamInfo  m_LadderTeamInfo;
	typedef std::vector< LadderTeamUserInfo > vUserInfo;
	vUserInfo	    m_vUserList;
	bool            m_bStateBackup;    
	bool            m_bLadderOrBattleTeam;

public:
	void SetInfo( SP2Packet &rkPacket );
	void SetInfo( LadderTeamInfo &rkInfo, bool bStateBackup );
	void UpdateTeamState( DWORD dwTeamIndex, int iState );
	void SetBtn();

protected:
	void JoinLadderTeam();
	void UpdateUserList();
	const LadderTeamUserInfo &GetUserInfo( int iArray );

public:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	

public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:	
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	LadderTeamInfoWnd();
	virtual ~LadderTeamInfoWnd();
};
//////////////////////////////////////////////////////////////////////////
class MyLadderTeamInfoWnd : public ioMovingWnd
{
public:
	enum
	{
		ID_EXIT    = 1,
		ID_CLOSE   = 2,
		ID_CHANGE_INFO = 3,
	};

	enum
	{
		MAX_LIST   = 8,
	};

protected:
	ioUIRenderImage *m_pModeIconBack;
	ioUIRenderImage *m_pBlueCampIcon;
	ioUIRenderImage *m_pRedCampIcon;

	ioUIRenderImage *m_pBlueDark;
	ioUIRenderImage *m_pBlueLight;
	ioUIRenderImage *m_pBlueOver;
	ioUIRenderImage *m_pRedDark;
	ioUIRenderImage *m_pRedLight;
	ioUIRenderImage *m_pRedOver;

	ioUIRenderImage *m_pCurrentDark;
	ioUIRenderImage *m_pCurrentLight;
	ioUIRenderImage *m_pCurrentOver;

protected:
	ioHashString m_MouseOverName;
	bool         m_bMouseLBtnDown;	

protected:
	PingStepWnd m_PingStep;

public:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	

public:
	virtual void iwm_create();
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:	
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

protected:
	void ProcessSiteClick();
	void OnRenderUser( int iXPos, int iYPos, int iSlotArray, const ioHashString szName, int iGradeLevel, int iLadderPoint );
	void OnRenderPing( const ioHashString &rkName, int iXPos, int iYPos );

public:
	MyLadderTeamInfoWnd();
	virtual ~MyLadderTeamInfoWnd();
}; 
//////////////////////////////////////////////////////////////////////////
class LadderPasswordWnd : public ioWnd
{
public:
	enum
	{
		ID_EXIT    = 1,
		ID_OK      = 2,
		ID_CANCEL  = 3,

		ID_PASS_EDIT = 100,
	};

protected:
	ioWnd *m_pPreEdit;
	
	LadderTeamInfo m_LadderTeamInfo;

	void JoinLadderTeam();

public:
	void SetLadderTeamInfo( LadderTeamInfo &rkLadderTeam );

public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnRender();	

public:
	LadderPasswordWnd();
	virtual ~LadderPasswordWnd();
};
//////////////////////////////////////////////////////////////////////////
class LadderCreateWnd : public ioWnd
{
public:
	enum
	{
		ID_EXIT   = 1,
		ID_CREATE = 2,
		ID_CANCEL = 3,
		ID_GUILD_TEAM = 4,
		ID_CHANGE_MODE = 5,

		ID_EDIT_NAME = 100,	
		ID_EDIT_PW   = 101,

		ID_MODE_LIST = 300,
	};

protected:
	ioWnd *m_pPreEdit;
	ioWnd *m_pNameEdit;
	ioWnd *m_pPWEdit;
	int   m_iModeIndex;
	DWORD m_dwJoinGuildIndex;
	
	void CreateLadderTeam();
	void SetGuildTeam( bool bGuildTeam );
public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void iwm_show();	
	virtual void iwm_hide();

protected:
	virtual void OnRender();

public:
	bool QuestGuide( int iModeIndex, float &rfXPos, float &rfYPos );

public:
	LadderCreateWnd();
	virtual ~LadderCreateWnd();
};
//////////////////////////////////////////////////////////////////////////
class LadderOwnerWnd : public ioWnd
{
public:
	enum
	{
		ID_EXIT   = 1,
		ID_CHANGE = 2,
		ID_CANCEL = 3,
		ID_GUILD_TEAM = 4,

		ID_EDIT_NAME = 100,	
		ID_EDIT_PW   = 101,
	};

protected:
	ioWnd *m_pPreEdit;
	ioWnd *m_pNameEdit;
	ioWnd *m_pPWEdit;

	ioHashString m_szLadderName;
	ioHashString m_szLadderPW;
	DWORD        m_dwJoinGuildIndex;


	void ChangeLadderTeamInfo();
	void SetGuildTeam( bool bGuildTeam );
public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void iwm_show();	
	virtual void iwm_hide();

protected:
	virtual void OnRender();

public:
	LadderOwnerWnd();
	virtual ~LadderOwnerWnd();
};
//////////////////////////////////////////////////////////////////////////
class LadderBattleMemberList : public ioButton
{
public:
	enum
	{
		ID_CHAT_BUBBLE = 2000,
	};

	enum    // 
	{
		MAX_CHAR_LODING = 8,
	};

	enum
	{
		VIEW_WIDTH = 82,
		VIEW_HEIGHT= 240,
	};

protected:
	ioUI3DRender *m_pUI3DRender;
	ioCreateChar *m_aMyChar;
	CHARACTER     m_CharInfo;
	DWORD         m_dwCharMotionItem;

protected:
	LadderTeamInfo     m_TeamInfo;
	LadderTeamUserInfo m_LadderUserInfo;
	int                m_iListArray;
	TeamType           m_eTeamType;
	
protected:
	ioUIRenderFrame *m_pBlueNor;
	ioUIRenderFrame *m_pBlueOver;
	ioUIRenderFrame *m_pRedNor;
	ioUIRenderFrame *m_pRedOver;
	ioUIRenderFrame *m_pCaptainBack;

	ioUIRenderImage *m_pBlueNorGra;
	ioUIRenderImage *m_pBlueOverGra;
	ioUIRenderImage *m_pRedNorGra;
	ioUIRenderImage *m_pRedOverGra;
	ioUIRenderImage *m_pGuildMarkLine;
	ioUIRenderImage *m_pBlueEmpty;
	ioUIRenderImage *m_pRedEmpty;

	ioUIRenderFrame *m_pCurrentNor;
	ioUIRenderFrame *m_pCurrentOver;
	ioUIRenderImage *m_pCurrentNorGra;
	ioUIRenderImage *m_pCurrentOverGra;
	ioUIRenderImage *m_pCurrentEmptyX;

	ioUIRenderImage *m_pLoadCharBack;
	ioUIRenderImage *m_pCharLoading[MAX_CHAR_LODING];

protected:
	DWORD        m_dwCharLoadTime;
	int          m_iCharLoadArray;

protected:
	PingStepWnd m_PingStep;

protected:
	void CreateCharacter();

public:
	virtual void iwm_create();

public:
	void MemberInfo( int iListArray, TeamType eTeam, const LadderTeamInfo &rkTeamInfo, const LadderTeamUserInfo &rkUserInfo );
	int  GetListArray(){ return m_iListArray; }
	TeamType GetTeam(){ return m_eTeamType; }
	const ioHashString &GetListName(){ return m_LadderUserInfo.m_szUserID; }

public:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();	
	void OnRenderSoldier( int iXPos, int iYPos );
	void OnRenderFrame( int iXPos, int iYPos );
	void OnRenderLoadingChar( int iXPos, int iYPos );
	void OnRenderPing( int iXPos, int iYPos );

public:
	void StartChatBubble( const char *szID, const char *szChat, bool bWholeChat );
	bool IsChangeChatBubble();
	void ChangeChatBubble( LadderBattleMemberList *pMemberList );
	void ClearChatBubble();

public:
	LadderBattleMemberList();
	virtual ~LadderBattleMemberList();
};

class LadderModeNMapBtn : public ioButton
{
protected:
	virtual void OnRender();
	void OnModeRender( int iXPos, int iYPos );
	void OnMapRender( int iXPos, int iYPos );

public:
	LadderModeNMapBtn();
	virtual ~LadderModeNMapBtn();
};

class LadderChatWnd : public ioWnd
{
public:
	enum
	{
		ID_CHAT_MSG_WND   = 1,
		ID_CHAT_INPUT_WND = 2,
	};

protected:
	ioHashString m_szLoadInfo;
	ChatType m_eChatType;

public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual bool iwm_esc();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

public:
	virtual void ParseExtraInfo( ioXMLElement &xElement );

public:
	void SetPlayStage( ioPlayStage *pPlayStage, ChatType eType );

public:
	LadderChatWnd();
	virtual ~LadderChatWnd();
};

class LadderUserOptionWnd : public ioWnd
{
public:
	enum
	{
		OPTION_INFO		   = 1000,
		OPTION_KICK		   = 1001,
		OPTION_CLOSE	   = 1002,
		OPTION_OPEN        = 1003,
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
	ioHashString m_szName;
	TeamType     m_eTeamType;
	int          m_iSlotArray;

	typedef std::map< DWORD, ioHashString > OptionMap;
	OptionMap m_OptionMap;

public:
	virtual void ParseExtraInfo( ioXMLElement &xElement );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

public:
	void SetOptionInfo( const ioHashString &rkName, TeamType eTeam, int iSlotArray );

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
	LadderUserOptionWnd();
	virtual ~LadderUserOptionWnd();
};

class LadderBattleWnd : public ioMovingWnd
{
public:
	enum
	{
		ID_CHANGE_INFO			= 1,
		ID_CHANGE_MODE			= 2,
		ID_CHANGE_MAP			= 3,
		ID_LEVEL_MATCH_SEARCH	= 4,
		ID_SAME_USER_SEARCH		= 5,
		ID_OTHER_CHAT_HIDE      = 6,
		ID_BAD_PING_KICK        = 7,

		ID_LADDER_RANKING		= 8,
		ID_LADDER_INVITE		= 9,
		ID_SEARCH_START			= 10,
		ID_SEARCH_END			= 11,

		ID_BATTLE_EXIT			= 12,

		ID_USER_1				= 100,
		ID_USER_2				= 101,
		ID_USER_3				= 102,
		ID_USER_4				= 103,
		ID_USER_5				= 104,
		ID_USER_6				= 105,
		ID_USER_7				= 106,
		ID_USER_8				= 107,

		ID_HERO_WND             = 200,

		ID_MODE_MAP_LIST		= 1000,
		ID_LADDER_CHAT_WND      = 1001,
		ID_USER_SLOT_OPTION		= 1002,
		ID_MODE_HELP_TOOLTIP	= 1003,
	};

	enum
	{
		RANKING_UPDATE_TIME = 60000,
		MAX_MID_MAP			= 7,
	};

protected:
	enum 
	{
		POS_SAVE_VERSION = 4,
	};

	int	  m_iDefaultX;
	int   m_iDefaultY;

protected:
	typedef std::vector< ioWnd * > vWndList;

protected:
	ioUIRenderFrame	*m_pModeNMapGrayFrm;

protected:
	ioPlayStage		*m_pPlayStage;	
	bool             m_bOwner;
	DWORD            m_dwRankingUpdateTime;

protected:
	DWORD            m_dwCurSearchTime;
	DWORD            m_dwFullSearchTime;
    int              m_iCurSearchSec;

protected:
	ioUIRenderImage *m_pWarCountWait;
	ioUIRenderImage *m_pWarCountCircle;
	ioUIRenderImage *m_pManualMark;

	ioUIRenderImage *m_pTextBack;

	DWORD m_dwChangeCountTime;

protected:
	ioHashString m_szBackOgg;

protected:
	ioWnd  *m_pModeHelpTooltip;
	bool    m_bModeTooltipOver;

protected:
	ioHashString m_szCampName;

public:
	void SetPlayStage( ioPlayStage *pPlayStage );

public:
	void UpdateLadderBattle();
	void StartSearch( DWORD dwFullTime );

protected:
	void ClearMapMidImage();

public:
	virtual void iwm_create();
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual bool iwm_esc();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate = false );

public:
	virtual void ParseExtraInfo( ioXMLElement &xElement );

protected:	
	void ProcessModeHelpTooltip();	
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();
	virtual void OnRenderAfterChild();
	void OnTeamRender( int iXPos, int iYPos );
	void OnHeroRender( int iXPos, int iYPos );
	void OnModeNMapRender( int iXPos, int iYPos );

	void OnSearchRender();

public:	
	void StartChatBubble( const char *szID, const char *szChat, bool bWholeChat );

public:
	LadderBattleWnd();
	virtual ~LadderBattleWnd();
};
//////////////////////////////////////////////////////////////////////////
class LadderBattleHeroRankBtn : public ioButton
{
public:
	enum
	{
		VIEW_WIDTH = 121,
		VIEW_HEIGHT= 240,
	};

protected:
	ioUI3DRender   *m_pUI3DRender;
	ioCreateChar   *m_aMyChar;
	CHARACTER       m_CharInfo;

protected:
	ioUIRenderFrame  *m_pBlueBar;
	ioUIRenderFrame  *m_pRedBar;
	ioUIRenderFrame  *m_pGrayBar;

	ioUIRenderFrame  *m_pBlueOverBar;
	ioUIRenderFrame  *m_pRedOverBar;
	ioUIRenderFrame  *m_pGrayOverBar;

	ioUIRenderImage *m_pBlueGra;
	ioUIRenderImage *m_pRedGra;
	ioUIRenderImage *m_pGrayGra;

	ioUIRenderImage *m_pBlueOverGra;
	ioUIRenderImage *m_pRedOverGra;
	ioUIRenderImage *m_pGrayOverGra;

	ioUIRenderImage *m_pDarkBar;
	ioUIRenderImage *m_pLightBar;

	ioUIRenderImage *m_pHeroRankTitle;
	ioUIRenderImage *m_pIconBack;
	ioUIRenderImage *m_pNumber;
	ioUIRenderImage *m_pRankText;
	ioUIRenderImage *m_pNoneText;
	ioUIRenderImage *m_pNoneIcon;

	ioUIRenderFrame *m_pCurrentBar;
	ioUIRenderFrame *m_pCurrentOverBar;
	ioUIRenderImage *m_pCurrentGra;
	ioUIRenderImage *m_pCurrentOverGra;

public:
	void CreateButton();

public:
	virtual void iwm_create();
	
protected:
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();
	void OnRenderSoldier( int iXPos, int iYPos );
	
public:
	LadderBattleHeroRankBtn();
	virtual ~LadderBattleHeroRankBtn();
};
//////////////////////////////////////////////////////////////////////////
class LadderBattleHeroWnd : public ioWnd
{
public:
	enum
	{
		ID_TAB_RANK_GRAPH	 = 1,
		ID_TAB_RANK_TOP100	 = 2,
		ID_TAB_BATTLE_RECORD = 3,
		ID_MY_RANK_BTN		 = 4,

		ID_RANK_GRAPH_WND    = 100,
		ID_RANK_TOP100_WND   = 101,
		ID_BATTLE_RECORD_WND = 102,
	};

protected:
	ioUIRenderFrame *m_pBlueBar;
	ioUIRenderFrame *m_pRedBar;

	ioUIRenderImage *m_pBlueGra;
	ioUIRenderImage *m_pRedGra;
	
	ioUIRenderImage *m_pDarkBar;
	ioUIRenderImage *m_pLightBar;

	ioUIRenderImage *m_pHeroRankTitle;
	ioUIRenderImage *m_pSeasonHeroTitle;
	ioUIRenderImage *m_pSeasonRecordTitle;

	ioUIRenderImage *m_pIconBack;
	ioUIRenderImage *m_pRecordIcon;
	ioUIRenderImage *m_pNumber;
	ioUIRenderImage *m_pRankText;
	ioUIRenderImage *m_pPercentText;
	ioUIRenderImage *m_pNoneText;
	ioUIRenderImage *m_pNoneIcon;

	ioUIRenderFrame *m_pCurrentBar;
	ioUIRenderImage *m_pCurrentGra;

protected:
	DWORD m_dwTabID;

protected:
	void CheckTabWnd( DWORD dwID );

public:
	virtual void iwm_create();
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

protected:
	void OnRenderHeroTitle( int iXPos, int iYPos );
	void OnRenderSeasonRecord( int iXPos, int iYPos );

public:
	LadderBattleHeroWnd();
	virtual ~LadderBattleHeroWnd();
};

class LadderTeamInviteList : public ioButton
{
public:
	enum
	{
		ID_INVITE_BTN = 1,
	};

protected:
	ioUIRenderImage *m_pGuildMark;
	ioUIRenderImage *m_pFriendMark;

protected:
	int			 m_iGradeLevel;
	ioHashString m_szName;

public:
	void InviteInfo( int iGradeLevel, ioHashString szName );

public:
	int  GetLevel() const { return m_iGradeLevel; }
	const ioHashString &GetName() const { return m_szName; }

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();	

public:
	LadderTeamInviteList();
	virtual ~LadderTeamInviteList();
};

class LadderTeamInviteListWnd : public ioMovingWnd
{
public:
	enum
	{
		ID_EXIT		   = 1,
		ID_LEFT_PAGE   = 2,
		ID_RIGHT_PAGE  = 3,
		ID_ALL_INVITE  = 4,
		ID_NAME_INVITE = 5,
		ID_NAME_EDIT   = 6,

		//USER LIST
		ID_USER_LIST_1  = 100,
		ID_USER_LIST_2  = 101,
		ID_USER_LIST_3  = 102,
		ID_USER_LIST_4  = 103,
		ID_USER_LIST_5  = 104,
		ID_USER_LIST_6  = 105,
		ID_USER_LIST_7  = 106,
		ID_USER_LIST_8  = 107,
		ID_USER_LIST_9  = 108,
		ID_USER_LIST_10 = 109,
	};

	enum
	{
		MAX_PAGE_LIST      = 10,
		INVITE_SEARCH_TIME = 15000,
	};
protected:
	struct UserInviteInfo
	{
		ioHashString m_szName;
		int			 m_iLevel;

		UserInviteInfo()
		{
			m_iLevel		= 0;
		}
	};
	typedef std::vector< UserInviteInfo > vUserInviteInfo;
	vUserInviteInfo m_vUserList;

	void AllInvite();
	void NameInvite();

	// °Ë»ö
	DWORD      m_dwSearchTime;
	int        m_iCurPage;
	int        m_iPrevCurPage;
	int        m_iMaxPage;

	//
	ioWnd *m_pPreEdit;

	void UpdateSetPage( int iCurPage );

	void StartSend( bool bProcess = false );
	void ProcessSearch();

public:
	void StartInivteWait( int iValue, const ioHashString &szName );
	void SetUserList( SP2Packet &rkPacket );

public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	LadderTeamInviteListWnd();
	virtual ~LadderTeamInviteListWnd();
};
//////////////////////////////////////////////////////////////////////////
class LadderTeamInvitedBtn : public ioButton
{
protected:
	ioUIRenderImage *m_pBlueNormal;
	ioUIRenderImage *m_pRedNormal;
	ioUIRenderImage *m_pBlueOver;
	ioUIRenderImage *m_pRedOver;
	ioUIRenderImage *m_pPeopleOrange;
	ioUIRenderImage *m_pPeopleGray;

protected:
	TeamType m_eTeam;
	ioHashString m_szName;
	int   m_iGradeLevel;
	int   m_iSlotArray;
	int   m_iPingStep;
	int   m_iMaxSlot;

protected:
	PingStepWnd m_PingStep;

public:
	virtual void iwm_create();

public:
	void SetMemberInfo( int iMaxSlot, int iSlotArray, const ioHashString &rkName, int iGradeLevel, int iPingStep, TeamType eTeam );
	const ioHashString &GetMemberName(){ return m_szName; }

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void OnRender();
	void OnRenderBase( int iXPos, int iYPos );

public:
	LadderTeamInvitedBtn();
	virtual ~LadderTeamInvitedBtn();
};
//////////////////////////////////////////////////////////////////////////
class LadderTeamInvitedWnd : public ioMovingWnd
{
public:
	enum
	{
		ID_EXIT			= 1,
		ID_ENTER		= 2,
		ID_CANCEL		= 3,
		ID_AUTO_REFUSE	= 4,
		ID_LEFT_PAGE	= 5,
		ID_RIGHT_PAGE	= 6,

		ID_LIST_1		= 10,
		ID_LIST_2		= 11,
		ID_LIST_3		= 12,
		ID_LIST_4		= 13,
	};
	enum
	{
		INVITE_DELAY_SEC = 20,
		MAX_PAGE_LIST    = 4,
	};

protected:
	static bool sg_bAutoRefuse;

protected:
	ioUIRenderImage *m_pModeIconBack;
	ioUIRenderImage *m_pBlueCampIcon;
	ioUIRenderImage *m_pRedCampIcon;

	LadderTeamInfo    m_LadderTeamInfo;
	typedef std::vector< LadderTeamUserInfo > vUserInfo;
	vUserInfo	     m_vUserList;

	int			  m_iCurPage;	
	int           m_iMaxPage;
	DWORD         m_dwStartTime;

public:
	void SetInfo( SP2Packet &rkPacket );

protected:
	void UpdatePage( int iCurPage );
	const LadderTeamUserInfo &GetUserInfo( int iArray );

protected:
	void JoinLadderTeam();

public:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	

public:
	virtual void iwm_show();
	virtual bool iwm_esc();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:	
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	LadderTeamInvitedWnd();
	virtual ~LadderTeamInvitedWnd();
};
//////////////////////////////////////////////////////////////////////////
class LadderRankingListWnd : public ioMovingWnd
{
public:
	enum
	{
		ID_EXIT			 = 1,
		ID_TEAM_RANKING  = 2,
		ID_HERO_RANKING  = 3,

		ID_TEAM_RANK_WND = 10,
		ID_HERO_RANK_WND = 11,
	};

protected:
	DWORD m_dwTabID;

protected:
	void CheckRankingButton( DWORD dwID );

public:
	virtual void iwm_create();
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	void ApplyRankingList( SP2Packet &rkPacket );

public:
	void ShowTabWnd( DWORD dwTabID );

public:
	LadderRankingListWnd();
	virtual ~LadderRankingListWnd();
};

class LadderRankListBtn : public ioButton
{
protected:
	ioUIRenderFrame *m_pInActiveFrm;
	ioUIRenderImage *m_pBlueNormal;
	ioUIRenderImage *m_pRedNormal;
	ioUIRenderImage *m_pBluePush;
	ioUIRenderImage *m_pRedPush;
	ioUIRenderImage *m_pBlueTeamIcon;
	ioUIRenderImage *m_pRedTeamIcon;
	ioUIRenderImage *m_pKey;
	ioUIRenderImage *m_pArrow;


protected:
	LadderTeamInfo   m_LadderTeamInfo;	
	bool m_bHeroMatch;

	struct ArrowAni
	{
		DWORD m_dwCheckTime;
		int   m_iOffsetX;
		ArrowAni()
		{
			m_dwCheckTime = 0;
			m_iOffsetX = 0;
		}
	};
	typedef std::vector< ArrowAni > vArrowAni;
	vArrowAni m_ArrowAni;
	int       m_AniArray;
	DWORD     m_dwAniTime;

public:
	void SetLadderTeamInfo( LadderTeamInfo &rkInfo, bool bHeroMatch );
	void SetTeamIcon( ioUIRenderImage* pBlueIcon, ioUIRenderImage* pRedIcon );

public:
	DWORD GetLadderTeamIndex(){ return m_LadderTeamInfo.m_dwTeamIndex; }
	const ioHashString &GetTeamName(){ return m_LadderTeamInfo.m_szTeamName; }
	LadderTeamInfo &GetLadderTeamInfo(){ return m_LadderTeamInfo; }

protected:
	virtual void OnDrawNormal( int iXPos, int iYPos );
	virtual void OnDrawOveredAdd( int iXPos, int iYPos );
	virtual void OnDrawPushed( int iXPos, int iYPos );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );

public:
	LadderRankListBtn();
	virtual ~LadderRankListBtn();
};

class LadderTeamRankListWnd : public ioWnd
{
public:
	enum
	{
		ID_LEFT_PAGE	= 1,
		ID_RIGHT_PAGE	= 2,

		ID_LIST_1	= 100,
		ID_LIST_2	= 101,
		ID_LIST_3	= 102,
		ID_LIST_4	= 103,
		ID_LIST_5	= 104,
		ID_LIST_6	= 105,
		ID_LIST_7	= 106,
		ID_LIST_8	= 107,
	};

	enum
	{
		MAX_RANK_LIST		= 8,
		RANK_SEARCH_TIME	= 10000,		
	};

protected:
	DWORD m_dwMainWndTabID;
	bool  m_bHeroMatch;
	DWORD m_dwSearchTime;
	int	  m_iCurPage;
	int   m_iMaxPage;

	typedef std::vector< LadderTeamInfo > vLadderTeamInfo;
	vLadderTeamInfo m_vLadderTeamList;

protected:
	ioUIRenderImage* m_pBlueCampMark;
	ioUIRenderImage* m_pRedCampMark;
	
public:
	virtual void iwm_show();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	void UpdateSetPage( int iCurPage );
	void ProcessSearch();

public:
	void ApplyLadderTeamRank( SP2Packet &rkPacket );
	void StartSend( DWORD dwTeamIndex );
	void SetMainWndTabID( DWORD dwMainWndTabID ){ m_dwMainWndTabID = dwMainWndTabID; }
	DWORD GetMainWndTabID(){ return m_dwMainWndTabID; }

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	LadderTeamRankListWnd();
	virtual ~LadderTeamRankListWnd();
};
//////////////////////////////////////////////////////////////////////////
class LadderHeroRankListWnd : public ioWnd
{
public:
	enum
	{
		ID_LEFT_PAGE	= 1,
		ID_RIGHT_PAGE	= 2,

		ID_LIST_1	= 100,
		ID_LIST_2	= 101,
		ID_LIST_3	= 102,
		ID_LIST_4	= 103,
		ID_LIST_5	= 104,
		ID_LIST_6	= 105,
		ID_LIST_7	= 106,
		ID_LIST_8	= 107,
	};

	enum
	{
		MAX_RANK_LIST		= 8,
		RANK_SEARCH_TIME	= 10000,		
	};

protected:
	DWORD      m_dwMainWndTabID;
	bool       m_bHeroMatch;
	DWORD      m_dwSearchTime;
	int		   m_iCurPage;	
	int        m_iMaxPage;

	typedef std::vector< LadderTeamInfo > vLadderTeamInfo;
	vLadderTeamInfo m_vLadderHeroList;

protected:
	ioUIRenderImage* m_pBlueCampMark;
	ioUIRenderImage* m_pRedCampMark;

public:
	virtual void iwm_show();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	void UpdateSetPage( int iCurPage );
	void ProcessSearch();

public:
	void ApplyLadderHeroRank( SP2Packet &rkPacket );
	void StartSend( DWORD dwTeamIndex );
	void SetMainWndTabID( DWORD dwMainWndTabID ){ m_dwMainWndTabID = dwMainWndTabID; }
	DWORD GetMainWndTabID(){ return m_dwMainWndTabID; }

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	LadderHeroRankListWnd();
	virtual ~LadderHeroRankListWnd();
};
