#pragma once

#include "GlobalWnd.h"

typedef struct tagPlazaInfo
{
	enum
	{
		PRS_ACTIVE				= 1,
		PRS_FULL_USER			= 2,
		PRS_NOT_MIN_LEVEL_MATCH = 3,
		PRS_NOT_MAX_LEVEL_MATCH = 4,
	};

	enum
	{
		PRS_SUB_NONE = 0,
		PRS_SUB_NPC_EVENT = 1,
	};

	ioHashString m_szRoomName;
	ioHashString m_szMasterName;
	ioHashString m_szSubTitle;
	ioHashString m_szRoomPW;
	
	int m_iRoomIndex;
	int m_iRoomNum;
	int m_iRoomLevel;	
	int m_iJoinUserCount;
	int m_iPlayUserCount;
	int m_iMaxUserCount;
	int m_iMasterLevel;
	
	bool m_bRoomPW;
	bool m_bMyRoom;
	int  m_iState;
	int  m_iSubState;
	PlazaType m_ePlazaType;

	tagPlazaInfo()
	{
		m_iRoomIndex    = -1;
		m_iRoomNum		= -1;
		m_iRoomLevel	= 0;	
		m_iJoinUserCount= 0;
		m_iPlayUserCount= 0;
		m_iMaxUserCount	= 0;
		m_iMasterLevel	= 0;
        
		m_bRoomPW	 = false;
		m_bMyRoom    = false;
		m_iState     = PRS_ACTIVE;
		m_iSubState	 = PRS_SUB_NONE;

		m_ePlazaType = PT_NONE;
	}
}PlazaInfo;

typedef struct tagPlazaUserInfo
{
	ioHashString m_szUserID;
	
	int m_iGradeLevel;
	int m_iServerPingStep;
	bool m_bMyFriend;
	
	DWORD m_dwUserIndex;
	DWORD m_dwGuildIndex;
	DWORD m_dwGuildMark;

	tagPlazaUserInfo()
	{
		m_iGradeLevel = 0;
		m_iServerPingStep = 0;
		m_bMyFriend = false;
		
		m_dwUserIndex = 0;
		m_dwGuildIndex = 0;
		m_dwGuildMark = 0;
	}

}PlazaUserInfo;

class PlazaListBtn : public ioButton
{
protected:
	ioUIRenderImage *m_pRoomNumBack;
	ioUIRenderImage *m_pLockKey;
	ioUIRenderImage *m_pLockKeyGray;
	ioUIRenderImage *m_pIconEmpty;
	ioUIRenderImage *m_pIconNpc;

protected:
	PlazaInfo m_PlazaInfo;
	ioHashString m_szPlazaType;

	float m_fWinkTime;
	bool m_bNpc;
	bool m_bTwinkle;

public:
	int GetRoomIndex(){ return m_PlazaInfo.m_iRoomIndex; }
	int GetRoomNum(){ return m_PlazaInfo.m_iRoomNum; }
	int GetRoomLevel(){ return m_PlazaInfo.m_iRoomLevel; }
	ioHashString GetRoomName(){ return m_PlazaInfo.m_szRoomName; }
	int GetCurJoiner(){ return m_PlazaInfo.m_iJoinUserCount; }
	int GetCurPlayer(){ return m_PlazaInfo.m_iPlayUserCount; }
	int GetMaxPlayer(){ return m_PlazaInfo.m_iMaxUserCount; }
	bool IsRoomPW(){ return m_PlazaInfo.m_bRoomPW; }
	int GetRoomState() { return m_PlazaInfo.m_iState; }
	int GetRoomSubState() { return m_PlazaInfo.m_iSubState; }
	PlazaType GetPlazaModeType(){ return m_PlazaInfo.m_ePlazaType; }
	bool IsMyRoom(){ return m_PlazaInfo.m_bMyRoom; }
	
	void SetPlazaInfo( PlazaInfo &rkPlazaInfo );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	

public:
	PlazaListBtn();
	virtual ~PlazaListBtn();
};

class PlazaPasswordWnd : public ioWnd
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

	int    m_iRoomIndex;
	int    m_iRoomLevel;
	ioHashString m_szRoomName;
	int    m_iCurJoiner;
	int    m_iCurPlayer;
	int    m_iMaxPlayer;
	PlazaType m_ePlazaType;

	bool  m_bHidePrevWndShow;

	void JoinPlaza();

public:
	void SetPlazaInfo( int iRoomIndex, int iRoomLevel, ioHashString szName, int iCurJoiner, int iCurPlayer, int iMaxPlayer, PlazaType ePlazaType );

public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnRender();	

public:
	PlazaPasswordWnd();
	virtual ~PlazaPasswordWnd();
};

class PlazaListWnd : public ioWnd
{
public:
	enum
	{
		ID_PLAZA_CREATE = 1,
		ID_PAGE_LEFT    = 2,
		ID_PAGE_RIGHT   = 3,

		ID_LIST_1 = 100,
		ID_LIST_2 = 101,
		ID_LIST_3 = 102,
		ID_LIST_4 = 103,
		ID_LIST_5 = 104,
		ID_LIST_6 = 105,
		ID_LIST_7 = 106,
		ID_LIST_8 = 107,
	};

	enum
	{
		LOBBY_MAX_PLAZA_LIST		= 6,
		PLAZA_MAX_PLAZA_LIST		= 8,
		PLAZA_SEARCH_TIME	= 10000,		
	};

protected:
	DWORD      m_dwSearchTime;
	DWORD	   m_dwMaxPlazaList;
	int        m_iCurPage;	
	int        m_iMaxPage;
	int		   m_iPageYPos;

	typedef std::vector< PlazaInfo > vPlazaInfo;
	vPlazaInfo m_vPlazaList;

public:
	virtual void iwm_show();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

public:
	void PlazaUIMove();
	inline void SetMaxPlazaList( DWORD dwMax ){ m_dwMaxPlazaList = dwMax; }

protected:
	void UpdateSetPage( int iCurPage );
	void ProcessSearch();

	void OnJoinPlaza( int iRoomIndex );
public:
	void SetPlazaList( SP2Packet &rkPacket );
	void StartSend( bool bProcess = false );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	PlazaListWnd();
	virtual ~PlazaListWnd();
};

class PlazaOwnerWnd : public ioWnd
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

	ioHashString m_szPlazaName;
	ioHashString m_szPlazaPW;

	void ChangePlazaInfo();
public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void iwm_show();	
	virtual void iwm_hide();

protected:
	virtual void OnRender();

public:
	void SetPlazaInfo( ioHashString szName, ioHashString szPW );

public:
	PlazaOwnerWnd();
	virtual ~PlazaOwnerWnd();
};

class PlazaInviteList : public ioButton
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
	DWORD        m_dwGuildIndex;
	DWORD        m_dwGuildMark;
	bool         m_bInviteActive;

public:
	void InviteInfo( int iGradeLevel, ioHashString szName, DWORD dwGuildIndex, DWORD dwGuildMark, bool bActive );

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
	PlazaInviteList();
	virtual ~PlazaInviteList();
};

class PlazaInviteListWnd : public ioMovingWnd
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
			m_iLevel = 0;
		}
	};
	typedef std::vector< UserInviteInfo > vUserInviteInfo;
	vUserInviteInfo m_vUserList;

	bool  m_bInviteActive;
	int   m_iRoomLevel;
	
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
	void SetUserList( SP2Packet &rkPacket );
	void SetPlazaInfo( int iCurUser, int iMaxUser, int iRoomLevel );
	void StartInivteWait( int iValue, const ioHashString &szName );

public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	PlazaInviteListWnd();
	virtual ~PlazaInviteListWnd();
};

class PlazaMainOptionWnd : public ioWnd
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
	ioHashString m_szMasterName;
	int          m_iSlotArray;
	int          m_iMaxSlot;
	bool         m_bMonsterBugBear;

	typedef std::map< DWORD, ioHashString > OptionMap;
	OptionMap m_OptionMap;

public:
	virtual void ParseExtraInfo( ioXMLElement &xElement );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

public:
	void SetOptionInfo( const ioHashString &rkName, const ioHashString &rkMasterName, int iSlotArray, int iMaxSlot, bool bMonsterBugBear );

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
	PlazaMainOptionWnd();
	virtual ~PlazaMainOptionWnd();
};

class PlazaMainWnd : public ioMovingWnd
{
public:
	enum 
	{
		ID_EXIT			= 1,
		ID_INVITE_BTN   = 2,
		ID_CHANGE_INFO  = 3,

		ID_OPTION_WND   = 1000,
	};	

	enum
	{
		MAX_HORZ = 4,
		MAX_VERT = 8,
	};

protected:
	ioUIRenderImage *m_pBarNormal;
	ioUIRenderImage *m_pBarNormalLight;
	ioUIRenderImage *m_pBarOver;
	ioUIRenderImage *m_pPeopleOrange;
	ioUIRenderImage *m_pPeopleGray;
	ioUIRenderImage *m_pModeIconBack;

	bool		 m_bUserInfoUpadte;
	PlazaInfo	 m_PlazaInfo;
	ioHashString m_szPlazaType;	
	ioPlayStage *m_pPlayStage;
	ioHashString m_MouseOverName;
	int          m_MouseOverIndex;
	bool         m_bMouseLBtnDown;	
	int          m_iModeSubNum;

	typedef std::vector< RoundInfo > RoundInfoList;
	RoundInfoList m_UserInfoList;

protected:
	void Update();


public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual bool iwm_esc();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

public:
	virtual void SetPositionToDefault( int iParentWidth,
									   int iParentHeight,
									   bool bChildMove,
									   bool bCreate = false );

public:
	void UpdatePlazaInfo( PlazaInfo &rkPlazaInfo );
	bool IsUserUpdate();

	void InitUserInfo();
	void SetUserInfo( RoundInfo &rkInfo );
	void UserInfoUpdate();

public:
	void SetPlayStage( ioPlayStage *pPlayStage ){ m_pPlayStage = pPlayStage; }
	void SetPlazaName( ioHashString szName );
	void SetPlazaMasterPW( ioHashString szPW );
	void SetPlazaLevel( int iLevel );
	void SetPlazaMaster( ioHashString szMaster );
	void SetPlazaPW( bool bPW );
	void SetJoinUserCount( int iJoinUserCnt );
	void SetMaxUserCount( int iMaxUserCnt );

public:
	int GetMaxUserCount(){ return m_PlazaInfo.m_iMaxUserCount; }
	int GetPlazaLevel(){ return m_PlazaInfo.m_iRoomLevel; }
	int GetPlazaNum(){ return m_PlazaInfo.m_iRoomNum; }
	const ioHashString &GetPlazaName(){ return m_PlazaInfo.m_szRoomName; }
	const ioHashString &GetPlazaMasterName(){ return m_PlazaInfo.m_szMasterName; }
	PlazaType GetPlazaModeType(){ return m_PlazaInfo.m_ePlazaType; }
	bool IsRoomPW(){ return m_PlazaInfo.m_bRoomPW; }

public:
	bool IsBugBearEmerge();

protected:
	void ProcessSiteClick();
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();	
	void OnRenderBar( int iXPos, int iYPos, int iMaxSlot, int iCurSlot, const ioHashString szName, int iGradeLevel );

public:
	PlazaMainWnd();
	virtual ~PlazaMainWnd();
};
//////////////////////////////////////////////////////////////////////////
class PlazaInvitedBtn : public ioButton
{
protected:
	ioUIRenderImage *m_pPeopleOrange;
	ioUIRenderImage *m_pPeopleGray;

protected:
	ioHashString m_szName;
	int m_iGradeLevel;
	int m_iPingStep;
	int m_iSlotArray;
	int m_iMaxSlot;

protected:
	PingStepWnd m_PingStep;

public:
	virtual void iwm_create();

public:
	void SetMemberInfo( int iMaxSlot, int iSlotArray, const ioHashString &rkName, int iGradeLevel, int iPingStep );
	const ioHashString &GetMemberName(){ return m_szName; }

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void OnRender();
	virtual void OnProcess( float fTimePerSec );

public:
	PlazaInvitedBtn();
	virtual ~PlazaInvitedBtn();
};
//////////////////////////////////////////////////////////////////////////
class PlazaInvitedWnd : public ioMovingWnd
{
public:
	enum 
	{
		ID_EXIT		   = 1,
		ID_AGREE	   = 2,
		ID_REFUSE	   = 3,
		ID_AUTO_REFUSE = 4,
		ID_LEFT_PAGE   = 5,
		ID_RIGHT_PAGE  = 6,

		ID_LIST_1 = 10,
		ID_LIST_2 = 11,
		ID_LIST_3 = 12,
		ID_LIST_4 = 13,
		ID_LIST_5 = 14,
		ID_LIST_6 = 15,
		ID_LIST_7 = 16,
		ID_LIST_8 = 17,
	};

	enum
	{
		INVITE_DELAY_SEC = 20,
		MAX_PAGE_LIST = 8,
	};

protected:
	static bool sg_bAutoRefuse;

protected:
	ioUIRenderImage *m_pModeIconBack;

protected:
	int m_iRoomNum;
	int m_iRoomIndex;
	int m_iPlazaType;
	int m_iModeSubNum;
	int m_iMaxPlayer;
    ioHashString m_szRoomName;
	ioHashString m_szPassword;
	ioHashString m_szPlazaType;
	typedef std::vector< PlazaUserInfo > vUserInfo;
	vUserInfo	  m_vUserList;

	DWORD m_dwStartTime;
	int   m_iCurPage;
	int   m_iMaxPage;

public:
	void SetInfo( int iRoomNum, SP2Packet &rkPacket );

protected:
	void UpdatePage( int iCurPage );
	const PlazaUserInfo &GetUserInfo( int iArray );

protected:
	void JoinPlaza();

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void iwm_show();	
	virtual void iwm_hide();

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	PlazaInvitedWnd();
	virtual ~PlazaInvitedWnd();
};
//////////////////////////////////////////////////////////////////////////
class PlazaModeNMapListWnd : public ioWnd
{
public:
	enum
	{
		ID_VERT_SCROLL = 1000,		
	};

	enum
	{
		MAX_PAGE_LIST  = 10,
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
	struct ListData
	{	
		int m_iMagicCode;
		ioHashString m_szTitle;
		ioUIRenderImage *m_pMarkRes;
		ListData()
		{
			m_iMagicCode = 0;
			m_pMarkRes = NULL;
		}
	};
	typedef std::vector< ListData > vListData;
	vListData m_ListData;
	ListData  m_OverListItem;
	int		  m_iCurPos;	
	ioWnd    *m_pOpenBtn;

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
	void SetList( ioWnd *pOpenBtn, int iStartXPos, int iStartYPos );
	void AddList( int iIndex, const ioHashString &szTitle, ioUIRenderImage *pMarkRes );
	void ShowListWnd();

public:
	ioWnd *GetOpenBtn(){ return m_pOpenBtn; }
	bool GetIndexToXYPos( int iSelectIndex, float &rfXPos, float &rfYPos );

public:
	PlazaModeNMapListWnd();
	virtual ~PlazaModeNMapListWnd();
};
////////////////////////////////////////////////////////////////////////////////////////////////
class PlazaCreateWnd : public ioWnd
{
public:
	enum
	{
		ID_EXIT   = 1,
		ID_CREATE = 2,
		ID_CANCEL = 3,
		ID_LEFT   = 4,
		ID_RIGHT  = 5,

		ID_CHANGE_MODE = 6,
		ID_CHANGE_MAP = 7,

		ID_EDIT_NAME = 100,	
		ID_EDIT_PW   = 101,

		ID_MODE_MAP_LIST = 300,
	};

protected:
	ioWnd *m_pPreEdit;
	ioWnd *m_pNameEdit;
	ioWnd *m_pPWEdit;
	
    int m_iMaxPlayer;
	int	m_iMapIndex;

	PlazaType m_ePlazaType;

	void CreatePlaza();

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void iwm_show();	
	virtual void iwm_hide();
	virtual bool iwm_esc();

protected:
	virtual void OnRender();

public:
	bool QuestGuide( int iModeIndex, int iMapIndex, float &rfXPos, float &rfYPos );

public:
	PlazaCreateWnd();
	virtual ~PlazaCreateWnd();
};
//////////////////////////////////////////////////////////////////////////
class PlazaInfoWnd : public ioMovingWnd
{
public:
	enum 
	{
		ID_EXIT			= 1,
		ID_ENTER_PLAZA  = 2,
		ID_CANCEL       = 3,
	};	

	enum
	{
		MAX_HORZ = 4,
		MAX_VERT = 8,
	};

protected:
	ioUIRenderImage *m_pBarNormal;
	ioUIRenderImage *m_pBarNormalLight;
	ioUIRenderImage *m_pBarOver;
	ioUIRenderImage *m_pPeopleOrange;
	ioUIRenderImage *m_pPeopleGray;
	ioUIRenderImage *m_pModeIconBack;

	PlazaInfo m_PlazaInfo;
	typedef std::vector< PlazaUserInfo > vUserInfo;
	ioHashString m_szPlazaType;
	vUserInfo	 m_vUserList;
	int          m_iModeSubNum;
	bool         m_bStateBackup;
	ioHashString m_MouseOverName;
	bool         m_bMouseLBtnDown;

public:
	virtual void iwm_create();

public:
	void SetInfo( SP2Packet &rkPacket );
	void SetPassword( bool bPassword );
	void SetState( int iState );
	void UpdatePlazaState( int iPlazaIndex, int iState );

protected:
	void ProcessSiteClick();
	void JoinPlazaRoom();

public:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	

public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:	
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();
	void OnRenderBar( int iXPos, int iYPos, int iMaxSlot, int iCurSlot, const ioHashString szName, int iGradeLevel );

public:
	PlazaInfoWnd();
	virtual ~PlazaInfoWnd();
};
//////////////////////////////////////////////////////////////////////////
class LobbyPlazaInfoWnd : public ioWnd
{
	// 로비의 광장 탭에서 표시할 정보 UI
public:
	enum
	{
		ID_MY_INFO_BTN = 1,
		ID_MISSION_BTN = 2,
		ID_QUEST_BTN   = 3,
		ID_NOVICE_GRADE_BTN   = 4,
		ID_NOVICE_GRADE_TOOTIP_BTN  = 5,
		ID_NOVICE_GRADE_TOOTIP		= 6,
		ID_TIME_GATE_BTN = 7,

		ID_MISSION_LIST_WND = 9,
		ID_QUEST_ALARM_WND     = 10,

		//-------------------------------------------------------------
		// 날짜 : 
		// 일일 퀘스트 추가 및 퀘스트 변경으로 
		// 로비에서 User정보들을 따로 보여주지 않는다.
		//-------------------------------------------------------------
		//ID_LOBBY_USER_LIST_WND = 11,
	};
protected:
	DWORD m_dwTabID;
	bool m_bIsNoviceGrade;

public:
	virtual void iwm_create();
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

protected:
	void		  ChangeTab( DWORD dwTabID );
	void		  UpdateRadioBtn();

protected:
	void SetNamedTitleSparkle();

public:
	LobbyPlazaInfoWnd();
	virtual ~LobbyPlazaInfoWnd();
};

