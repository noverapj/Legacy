#pragma once

#include "../CharacterInfo.h"

#include "PartyMenuWnd.h"
#include "GlobalWnd.h"

class ioUI3DRender;
class ioCreateChar;
class ioUIRenderImage;
class ioUIRenderFrame;

class ActiveScreenBtn : public ioButton
{
protected:
	bool m_bScreen;
	DWORD m_dwScreenTimer;

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnDrawNormal( int iXPos, int iYPos );

public:
	ActiveScreenBtn();
	virtual ~ActiveScreenBtn();
};

class PartyMemberList : public ioButton
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
		VIEW_HEIGHT= 125,
		VIEW_BATTLE_MODE_HEIGHT= 240,
	};

protected:
	ioUI3DRender *m_pUI3DRender;
	ioUI3DRender *m_pUI3DRender_BattleMode;
	ioCreateChar *m_aMyChar;
	CHARACTER     m_CharInfo;

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
	ioUIRenderImage *m_pGoldMonsterCoin;
	ioUIRenderImage *m_pMonsterCoin;
	ioUIRenderImage *m_pRaidCoin;
	ioUIRenderImage *m_pCoinNumber;
	ioUIRenderImage *m_pCoinUnlimited;
	ioUIRenderImage *m_pBlueEmpty;
	ioUIRenderImage *m_pRedEmpty;

	ioUIRenderFrame *m_pCurrentNor;
	ioUIRenderFrame *m_pCurrentOver;
	ioUIRenderImage *m_pCurrentNorGra;
	ioUIRenderImage *m_pCurrentOverGra;
	ioUIRenderImage *m_pCurrentEmptyX;

	ioUIRenderImage *m_pLoadCharBack;
	ioUIRenderImage *m_pCharLoading[MAX_CHAR_LODING];
	ioUIRenderImage *m_pAIBG;
	bool		 m_bAI;

protected:
	int			 m_iGradeLevel;
	int          m_iMonsterCoin;
	int          m_iGoldMonsterCoin;
	int          m_iRaidCoin;
	ioHashString m_szName;
	DWORD        m_dwCharIndex;
	DWORD        m_dwCharMotionItem;
	int          m_iListArray;
	TeamType     m_eUITeam;             // UI 컬러를 위한 팀
	TeamType     m_ePrivateTeam;        // 전투방 좌우 팀을 위한 팀
	bool         m_bRenderCoin;

	DWORD        m_dwCharLoadTime;
	int          m_iCharLoadArray;

protected:
	PingStepWnd m_PingStep;

protected:
	void CreateCharacter();

public:
	void MemberInfo( TeamType eUITeam, int iListArray, int iGradeLevel, ioHashString szName, int iMonsterCoin, int iGoldMonsterCoin, int iRaidCoin, bool bAI = false );
	int GetListArray(){ return m_iListArray; }
	const ioHashString &GetListName(){ return m_szName; }
	TeamType GetTeam(){ return m_ePrivateTeam; }

public:
	virtual void iwm_create();

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
	ModeType ModifyUIYPosByMode(int& iYPos, bool bFrame);

public:
	void StartChatBubble( const char *szID, const char *szChat, bool bWholeChat );
	bool IsChangeChatBubble();
	void ChangeChatBubble( PartyMemberList *pMemberList );
	void ClearChatBubble();

public:
	PartyMemberList();
	virtual ~PartyMemberList();
};

class PartyModeNMapBtn : public ioButton
{
	// 배틀모드일때만 사용
	// UserIndex저장해서 일치한 버튼만 업데이트 가능
	DWORD m_dwUserIndex;
protected:
	virtual void OnRender();
	void OnModeRender( int iXPos, int iYPos );
	void OnMapRender( int iXPos, int iYPos );
	void OnAiRender( int iXPos, int iYPos );
	void OnBattleOrderRender( int iXPos, int iYPos );		// 배틀모드 출전 순서 표시

public:
	enum eBtnImg { eNormal, eOver, eOverAdd, ePush, eMax };
	virtual void CheckFrameReSize( int iWidth, int iHeight, int iPrevWidth, int iPrevHeight );

	void OnAiMode( bool bSet );

	void SetUserIndex(DWORD dwUserindex) { m_dwUserIndex = dwUserindex; }
	DWORD GetUserIndex() { return m_dwUserIndex; }

	PartyModeNMapBtn();
	virtual ~PartyModeNMapBtn();
};

class BattleChatWnd : public ioWnd
{
public:
	enum
	{
		ID_CHAT_MSG_WND   = 1,
		ID_CHAT_INPUT_WND = 2,
	};

protected:
	ioHashString m_szLoadInfo;

public:
	virtual void iwm_create();
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual bool iwm_esc();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

public:
	virtual void ParseExtraInfo( ioXMLElement &xElement );

public:
	void SetPlayStage( ioPlayStage *pPlayStage );

public:
	BattleChatWnd();
	virtual ~BattleChatWnd();
};

class BattleUserOptionWnd : public ioWnd
{
public:
	enum
	{
		OPTION_INFO		   = 1000,
		OPTION_KICK		   = 1001,
		OPTION_CLOSE	   = 1002,
		OPTION_OPEN        = 1003,
		OPTION_CHANGE_TEAM = 1004,
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
	BattleUserOptionWnd();
	virtual ~BattleUserOptionWnd();
};

class PartyShamBattleWnd : public ioMovingWnd
{
public:
	enum
	{
		ID_CHANGE_INFO    = 1,
		ID_CHANGE_MODE    = 2,
		ID_CHANGE_MAP     = 3,
		ID_CHANGE_AI_LEVEL	= 16,

		ID_EXTRA_OPTION_BTN = 4,
		ID_RANDOM_TEAM		= 5,
		ID_START_ENTER_X	= 6,
		ID_AUTO_START		= 7,
		ID_BAD_PING_KICK	= 8,

		ID_CHANGE_TEAM      = 9,
		ID_OBSERVER_TEAM	= 10,
		ID_BATTLE_INVITE	= 11,
		ID_BATTLE_START		= 12,
		ID_BATTLE_END		= 13,

		ID_BATTLE_EXIT		= 14,

		ID_NO_CHALLENGER	= 15,

		// 순서 정하는 리스트 박스
		ID_EXHIBITION_BLUE_ORDER_1	= 17,
		ID_EXHIBITION_BLUE_ORDER_2	= 18,
		ID_EXHIBITION_BLUE_ORDER_3	= 19,

		ID_EXHIBITION_RED_ORDER_1	= 20,
		ID_EXHIBITION_RED_ORDER_2	= 21,
		ID_EXHIBITION_RED_ORDER_3	= 22,

		ID_BLUE_USER_1    = 100,
		ID_BLUE_USER_2    = 101,
		ID_BLUE_USER_3    = 102,
		ID_BLUE_USER_4    = 103,
		ID_BLUE_USER_5    = 104,
		ID_BLUE_USER_6    = 105,
		ID_BLUE_USER_7    = 106,
		ID_BLUE_USER_8    = 107,

		ID_BLUE_BATTLE_USER_1    = 108,
		ID_BLUE_BATTLE_USER_2    = 109,
		ID_BLUE_BATTLE_USER_3    = 110,
		ID_BLUE_BATTLE_USER_4    = 111,

		ID_RED_USER_1     = 200,
		ID_RED_USER_2     = 201,
		ID_RED_USER_3     = 202,
		ID_RED_USER_4     = 203,
		ID_RED_USER_5     = 204,
		ID_RED_USER_6     = 205,
		ID_RED_USER_7     = 206,
		ID_RED_USER_8     = 207,

		ID_RED_BATTLE_USER_1    = 208,
		ID_RED_BATTLE_USER_2    = 209,
		ID_RED_BATTLE_USER_3    = 210,
		ID_RED_BATTLE_USER_4    = 211,

		ID_MODE_MAP_LIST     = 1000,
		ID_BATTLE_CHAT_WND   = 1001,
		ID_USER_SLOT_OPTION  = 1002,
		ID_MODE_HELP_TOOLTIP = 1003,
	};

	enum
	{
		MAX_WAR_COUNT	= 6,
	};

	enum
	{
		BATTLE_ORDER_NONE = -1,
		BATTLE_ORDER_RANDOM = 0,
		BATTLE_ORDER_FIRST = 1,
		BATTLE_ORDER_SECOND = 2,
		BATTLE_ORDER_THIRD = 3,
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
	ioUIRenderFrame *m_pTournamentStartGrayFrm;
	ioUIRenderFrame *m_pTournamentEntryArmFrm;

protected:
	ioUIRenderImage *m_pWarCount[MAX_WAR_COUNT];
	ioUIRenderImage *m_pWarCountCircle;
	ioUIRenderImage *m_pManualMark;

	DWORD m_dwChangeCountTime;

protected:
	bool    m_bOwner;
	bool    m_bFreeForAllMode;
	bool    m_bStartEnterXMode;
	DWORD   m_mode_start_time;
	int     m_mode_cur_sec;

	ioPlayStage *m_pPlayStage;

protected:
	ioHashString m_szBackOgg;

protected:
	DWORD m_dwAutoStartCheckTime;
	DWORD m_dwAutoStartCurrentTime;
	int   m_iAutoStartSecond;

protected:
	ioWnd  *m_pModeHelpTooltip;
	bool    m_bModeTooltipOver;

protected:
	ioHashString m_szTournamentTitle;
	ioHashString m_szTournamentRound;
	ioHashString m_szTournamentBlueTeamName;
	ioHashString m_szTournamentRedTeamName;

protected:
	bool m_bSearchEntry;
	TeamType m_SearchEntryTeamType;

protected:
	bool m_bTournamentStart;
	bool m_bTournamentStartLog;

	bool m_bCheckSlotByMode;

	// 배틀모드 순서 변경 가능한 상태인지 체크하는 플래그
	bool m_bBattleOrderControl;

	// 팀섞어 옵션 true로 변경 시 출전 순서 패킷 보내지 않도록 하는 플래그
	bool m_bDoNotSendPacket;
public:
	virtual void iwm_create();
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual bool iwm_esc();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate = false );

public:
	virtual void ParseExtraInfo( ioXMLElement &xElement );

public:
	void ChangeModeType( bool bOwnerChange = false );		// 방장이 모드 변경 시 체크하는 bool형 파라메터 추가
	void SetModeStart( bool isStart, bool isAuto );

public:
	bool IsChangeMaxPlayerLimit( int iPrevModeIndex, int iPrevMapIndex );
	bool IsMaxPlayerLimit( int iChangeMaxPlayer );
	void SetMaxPlayerLimit();
	void SendMaxPlayer( int iMaxBluePlayer, int iMaxRedPlayer, int iMaxObserver );
	void SendAILevel( int iLevel );
	void SwitchUserSlot();

public:
	bool IsUserCountFailed(int iMinUser = -1);
	void OnBattleReadyGO();
	void OnChangeTeam();
	void OnChangeAILevel();
	void ApplyAILevel( int iLevel );

public:
	void UpdateShamBattle();
	void UpdateShamBattleTournament();
	void SetPlayStage( ioPlayStage *pPlayStage );

	bool IsModeStart(){ return ( m_mode_start_time != 0 ); }
	void InitAutoStart();
	
	TeamType GetSearchEntryTeamType(){ return m_SearchEntryTeamType; }
	void SetEntrySerchShow( bool bSearchShow ){ m_bSearchEntry = bSearchShow; }

protected:
	void ProcessTournament();
	void ProcessModeStart();
	void ProcessOwnerAutoStart();
	void UpdateUserList();

	void ClearCountImage();

public:
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

protected:
	void OnTournamentRender( int iXPos, int iYPos );
	void OnTournamentEntrySearchRender( int iXPos, int iYPos );
	
protected:
	void ProcessModeHelpTooltip();	
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();	
	virtual void OnRenderAfterChild();

	void OnModeNMapRender( int iXPos, int iYPos );
	void OnCountRender();	

public:	
	void StartChatBubble( const char *szID, const char *szChat, bool bWholeChat );

public:
	bool QuestGuide( int iModeSelectIndex, int iMapSelectIndex, float &rfXPos, float &rfYPos );

	void SetBattleModeOrderListItem( ioWnd* pWnd );

	// 배틀모드 방에서 팀 섞기 체크하면 모든 유저의 출전 순서 버튼 비활성화 후 순서가 '무작위'로 변경됨
	void FixBattleModeOrderByRandomTeamOpt();
public:
	void SetTournamentDrawBye();

public:
	PartyShamBattleWnd();
	virtual ~PartyShamBattleWnd();
};
//////////////////////////////////////////////////////////////////////////
class PartyBattleSimpleBtn : public ioButton
{
protected:
	ioUIRenderImage *m_pBlueNormal;
	ioUIRenderImage *m_pRedNormal;
	ioUIRenderImage *m_pNoneNormal;
	ioUIRenderImage *m_pBlueOver;
	ioUIRenderImage *m_pRedOver;
	ioUIRenderImage *m_pNoneOver;
	ioUIRenderImage *m_pPeopleOrange;
	ioUIRenderImage *m_pPeopleGray;

protected:
	TeamType m_eTeam;
	ioHashString m_szName;
	int m_iGradeLevel;
	int m_iSlotArray;
	int m_iMaxSlot;

protected:
	PingStepWnd m_PingStep;

public:
	virtual void iwm_create();

public:
	void SetMemberInfo( int iMaxSlot, int iSlotArray, const ioHashString &rkName, int iGradeLevel, TeamType eTeam );
	const ioHashString &GetMemberName(){ return m_szName; }
	int GetSlotArray(){ return m_iSlotArray; }
	int GetMaxSlot(){ return m_iMaxSlot; }

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void OnRender();
	virtual void OnProcess( float fTimePerSec );
	void OnRenderBase( int iXPos, int iYPos );
	void OnRenderPing( int iXPos, int iYPos );

public:
	PartyBattleSimpleBtn();
	virtual ~PartyBattleSimpleBtn();
};
//////////////////////////////////////////////////////////////////////////
class PartyBattleSimpleWnd : public ioMovingWnd
{
public:
	enum 
	{
		ID_EXIT				= 1,
		ID_INVITE_LIST		= 2,
		ID_OBSERVER     	= 3,

		ID_EXTRA_OPTION     = 4,
		ID_RANDOM_TEAM		= 5,
		ID_START_ENTER_X	= 6,
		ID_AUTO_START		= 7,
		ID_BAD_PING_KICK	= 8,

		ID_CHANGE_INFO      = 9,

		ID_NO_CHALLENGER	= 31,

		ID_BLUE_TEAM_1 = 10,
		ID_BLUE_TEAM_2 = 11,
		ID_BLUE_TEAM_3 = 12,
		ID_BLUE_TEAM_4 = 13,
		ID_BLUE_TEAM_5 = 14,
		ID_BLUE_TEAM_6 = 15,
		ID_BLUE_TEAM_7 = 16,
		ID_BLUE_TEAM_8 = 17,

		ID_RED_TEAM_1  = 20,
		ID_RED_TEAM_2  = 21,
		ID_RED_TEAM_3  = 22,
		ID_RED_TEAM_4  = 23,
		ID_RED_TEAM_5  = 24,
		ID_RED_TEAM_6  = 25,
		ID_RED_TEAM_7  = 26,
		ID_RED_TEAM_8  = 27,

		ID_MODE_HELP_TOOLTIP = 500,
	};	

	enum
	{
		MAX_TEAM_LIST = 8,

		NORMAL_MODE_HEIGHT   = 399,
		USER_MODE_HEIGHT     = 539,
		BOTTOM_BAR_GAP       = 65,
		BOTTOM_BTN_GAP       = 48,
		BOTTOM_CHECK_BTN_GAP = 96,
	};

	enum
	{
		// 유저 모드 옵션 인덱스
		UMO_GROWTH_USE = 0,
		UMO_CHANGE_CHAR,
		UMO_KO,
		UMO_DROP_DAMAGE,
		UMO_TEAM_ATTACK,
		UMO_ROUND_TYPE,
		UMO_EXTRAITEM_USE,
		UMO_COOL_TIME,
		UMO_KO_EFFECT,
		UMO_GET_UP_DELAY,
		UMO_GRAVITY,
		UMO_ROUND_TIME,


		// 유저 모드 팀 옵션
		UMO_BLUE_HP,
		UMO_BLUE_MOVE_SPD,
		UMO_BLUE_EQUIP,
		UMO_BLUE_BLOW,
		UMO_RED_HP,
		UMO_RED_MOVE_SPD,
		UMO_RED_EQUIP,
		UMO_RED_BLOW,
	};

protected:
	ioUIRenderFrame *m_pBottomFrm;
	ioUIRenderImage *m_pModeIconBack;
	ioUIRenderImage *m_pUserModeLine;
	ioUIRenderImage *m_pUserModeBoxDark;
	ioUIRenderImage *m_pUserModeBoxLight;

protected:
	ioWnd        *m_pModeHelpTooltip;
	bool          m_bIconOver;

protected:
	void UpdateUserList();
	void BottomButtonPos( DWORD dwID, int iBottomGap );
	bool IsChangeMaxPlayerLimit( int iPrevModeIndex, int iPrevMapIndex );
	bool IsMaxPlayerLimit( int iChangeMaxPlayer );
	void SendMaxPlayer( int iMaxBluePlayer, int iMaxRedPlayer, int iMaxObserver );

protected:
	ioHashString GetUserModeDesc( int iType );
	bool IsDefatulUserModeType( int iType );

public:
	void UpdateBattleInfo();

public:
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	

public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate = false );

protected:	
	void ProcessModeHelpTooltip();	
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();
	void OnRenderUserMode( int iXPos, int iYPos );

public:
	PartyBattleSimpleWnd();
	virtual ~PartyBattleSimpleWnd();
};
//////////////////////////////////////////////////////////////////////////
class PartyObserverBtn : public ioButton
{
protected:
	ioUIRenderImage *m_pPeopleOrange;
	ioUIRenderImage *m_pPeopleGray;

protected:
	ioHashString m_szName;
	int m_iGradeLevel;
	int m_iSlotArray;
	int m_iMaxSlot;

public:
	void SetMemberInfo( int iMaxSlot, int iSlotArray, const ioHashString &rkName, int iGradeLevel );
	const ioHashString &GetMemberName(){ return m_szName; }
	int GetSlotArray(){ return m_iSlotArray; }
	int GetMaxSlot(){ return m_iMaxSlot; }

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void OnRender();
	virtual void OnProcess( float fTimePerSec );

public:
	PartyObserverBtn();
	virtual ~PartyObserverBtn();
};
//////////////////////////////////////////////////////////////////////////
class PartyObserverWnd : public ioMovingWnd
{
public:
	enum 
	{
		ID_EXIT		  = 1,
		ID_OBSERVER   = 2,
		ID_CANCEL     = 3,

		ID_OBSERVER_1 = 10,
		ID_OBSERVER_2 = 11,
		ID_OBSERVER_3 = 12,
		ID_OBSERVER_4 = 13,

		ID_USER_SLOT_OPTION = 1001,
	};

protected:
	int  m_iObserverUserCount;
	int  m_iMaxObserver;
	bool m_bModeStart;

protected:
	bool IsObserverLimit();
	bool IsCancelLimit();
	void UpdateObserver();
	void SendMaxPlayer( int iMaxBluePlayer, int iMaxRedPlayer, int iMaxObserver );

public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:	
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	PartyObserverWnd();
	virtual ~PartyObserverWnd();
};
//////////////////////////////////////////////////////////////////////////
class PartyOwnerWnd : public ioWnd
{
public:
	enum
	{
		ID_EXIT   = 1,
		ID_CHANGE = 2,
		ID_CANCEL = 3,

		ID_EDIT_NAME = 100,	
		ID_EDIT_PW   = 101,
	};

protected:
	ioWnd *m_pPreEdit;
	ioWnd *m_pNameEdit;
	ioWnd *m_pPWEdit;

	ioHashString m_szPartyName;
	ioHashString m_szPartyPW;

	void ChangeBattleRoomInfo();
public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void iwm_show();	
	virtual void iwm_hide();

protected:
	virtual void OnRender();

public:
	PartyOwnerWnd();
	virtual ~PartyOwnerWnd();
};
// 초대 리스트 윈도우
class PartyInviteList : public ioButton
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
	int			    m_iGradeLevel;
	ioHashString    m_szName;
	DWORD           m_dwGuildIndex;
	DWORD           m_dwGuildMark;

public:
	void InviteInfo( int iGradeLevel, ioHashString szName, DWORD dwGuildIndex, DWORD dwGuildMark );

public:
	int  GetLevel() const { return m_iGradeLevel; }
	const ioHashString &GetName() const { return m_szName; }
	DWORD GetGuildIndex() const { return m_dwGuildIndex; }
	DWORD GetGuildMark() const { return m_dwGuildMark; }

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();	

public:
	PartyInviteList();
	virtual ~PartyInviteList();
};

class PartyInviteListWnd : public ioMovingWnd
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

	// 검색
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
	PartyInviteListWnd();
	virtual ~PartyInviteListWnd();
};
//////////////////////////////////////////////////////////////////////////
// 파티 연습 중간 참여 후 최종 결과나 엠블렘시 대기 UI
//////////////////////////////////////////////////////////////////////////
class MiddleJoinDelayWnd : public ioMovingWnd
{
protected:
	struct ScaleImage
	{
		ioUIRenderImage *m_pImage;
		DWORD m_dwScaleTime;
		float m_fCurScale;
	};

	typedef std::vector< ScaleImage* > ScaleImageList;
	ScaleImageList m_ScaleImageList;
	ioUIRenderImage *m_pScaleImageBack;

protected:
	DWORD m_dwChangeDuration;
	float m_fBigScale;
	float m_fSmallScale;
	DWORD m_dwChangeTime;
	int   m_iCurBigIdx;	
	bool  m_bReverse;
	
protected:
	void ScaleIconImage();

public:
	virtual void ParseExtraInfo( ioXMLElement &xElement );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	virtual void iwm_show();
	virtual void iwm_hide();

public:
	MiddleJoinDelayWnd();
	virtual ~MiddleJoinDelayWnd();
};


//
class PartyOptionBtn : public ioButton
{
protected:
	DWORD m_dwDefaultColor;
	DWORD m_dwChangeColor;

public:
	virtual void ParseExtraInfo( ioXMLElement &xElement );

public:
	void SetOptionInfo( const char *szTitle, int iCurrentType, int iDefaultType );

public:
	PartyOptionBtn();
	virtual ~PartyOptionBtn();
};

class PartyOptionWnd : public ioMovingWnd
{
public:
	enum
	{	
		ID_PRESET_BTN			= 1,

		ID_CHANGE_CHAR_TYPE_TLE	= 11,
		ID_CHANGE_CHAR_TYPE		= 12,

		ID_COOLTIME_TYPE_TLE	= 13,
		ID_COOLTIME_TYPE		= 14,

		ID_DROP_DAMAGE_TYPE_TLE	= 15,
		ID_DROP_DAMAGE_TYPE		= 16,

		ID_TEAM_ATTACK_TYPE_TLE	= 17,
		ID_TEAM_ATTACK_TYPE		= 18,

		ID_GRAVITY_TYPE_TLE		= 19,
		ID_GRAVITY_TYPE			= 20,

		ID_GETUP_TYPE_TLE		= 21,
		ID_GETUP_TYPE			= 22,

		ID_KO_TYPE_TLE			= 23,
		ID_KO_TYPE				= 24,

		ID_KO_EFFECT_TYPE_TLE	= 25,
		ID_KO_EFFECT_TYPE		= 26,

		ID_ROUND_TYPE_TLE		= 27,
		ID_ROUND_TYPE			= 28,
		ID_ROUND_TIME_TYPE_TLE	= 29,
		ID_ROUND_TIME_TYPE		= 30,

		ID_GROWTH_USE_TYPE_TLE	= 31,
		ID_GROWTH_USE_TYPE		= 32,

		ID_EXTRAITEM_USE_TYPE_TLE	= 33,
		ID_EXTRAITEM_USE_TYPE		= 34,

		ID_HP_TYPE_TLE			= 101,
		ID_BLUE_HP_TYPE			= 102,
		ID_RED_HP_TYPE			= 103,

		ID_BLOW_TYPE_TLE		= 104,
		ID_BLUE_BLOW_TYPE		= 105,
		ID_RED_BLOW_TYPE		= 106,

		ID_MOVE_SPEED_TYPE_TLE	= 107,
		ID_BLUE_MOVE_SPEED_TYPE	= 108,
		ID_RED_MOVE_SPEED_TYPE	= 109,

		ID_EQUIP_TYPE_TLE		= 110,
		ID_BLUE_EQUIP_TYPE		= 111,
		ID_RED_EQUIP_TYPE		= 112,

		ID_DEFAULT_SET			= 501,

		ID_TYPE_LIST_WND		= 1000,
		ID_TYPE_LIST2_WND		= 1001,
	};

public:
	void UpdateOption();
	void SetButtonTitle( DWORD dwID, const char *szTitle, int iCurrentType, int iDefaultType );
	ioHashString GetOptionTitle( int iType );

public:
	virtual void iwm_show();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

protected:
	void CheckShowChildListWnd( ioWnd *pWnd, DWORD dwID );
	bool SelectCheildListWnd( DWORD dwID, DWORD dwParam );

	bool CheckChangeCharType( int iIndex );
	bool CheckCoolTimeType( int iIndex );
	bool CheckRedHPType( int iIndex );
	bool CheckBlueHPType( int iIndex );
	bool CheckDropDamageType( int iIndex );
	bool CheckGravityType( int iIndex );
	bool CheckTeamAttackType( int iIndex );
	bool CheckGetUpType( int iIndex );
	bool CheckKOType( int iIndex );
	bool CheckKOEffectType( int iIndex );
	bool CheckRedBlowType( int iIndex );
	bool CheckBlueBlowType( int iIndex );
	bool CheckRedMoveSpeedType( int iIndex );
	bool CheckBlueMoveSpeedType( int iIndex );
	bool CheckRedEquipType( int iIndex );
	bool CheckBlueEquipType( int iIndex );

	bool CheckRoundType( int iIndex );
	bool CheckRoundTimeType( int iIndex );

	bool CheckGrowthUseType( int iIndex );
	bool CheckExtraItemUseType( int iIndex );

	bool CheckPreSetModeType( int iIndex );

public:
	PartyOptionWnd();
	virtual ~PartyOptionWnd();
};