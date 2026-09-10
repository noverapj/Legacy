#pragma once

#include "../io3DEngine/ioMovingWnd.h"

#include "../CharacterInfo.h"
#include "FSMStateMgr.h"
#include "ioUserInfoMgr.h"
#include "MannerTrialChat.h"

class ioUIRenderImage;
class ioUIRenderFrame;
class ioPlayStage;
class ioEdit;
class ioUI3DRender;
class ioCreateChar;

class MannerTrialWnd : public ioMovingWnd
{
public:
	enum
	{
		ID_EXIT			 = 1,
		ID_TRIAL		 = 2,
		ID_CANCEL		 = 3, 
		ID_EDIT_REASON_1 = 4,
	};

protected:
	ioHashString m_szTargetID;
	ioEdit      *m_pPreEdit;
	ioHashString m_szChatList;
	bool         m_bChatHaveID;
	DWORD        m_dwChannelIndex;
	bool         m_bVoiceChat;
	ioMannerTrialChatManager::TrialType    m_eTrialType;

protected:
	bool OnMinusMannerAndTrial();
	void GetReasonString(OUT ioHashString &rszReason );
	const ioHashString GetTypeString();

public:
	void SetMannerTrialInfo( const ioHashString &szID , ioMannerTrialChatManager::TrialType eType, DWORD dwChannelIndex );

protected:
	virtual void OnRender();
	virtual void OnProcess( float fTimePerSec );

public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void ParseExtraInfo( ioXMLElement &xElement );

public:
	MannerTrialWnd();
	virtual ~MannerTrialWnd();
};
//////////////////////////////////////////////////////////////////////////
class MannerValuationParentWnd : public ioMovingWnd
{
public:
	virtual void SetMannerValuationInfo( const ioHashString &szID, bool bOnline, ioMannerTrialChatManager::TrialType eType, DWORD dwChannelIndex = 0) = 0;
	virtual bool IsOpenState( const ioHashString &szID ){ return true; }

public:
	MannerValuationParentWnd(){};
	virtual ~MannerValuationParentWnd(){};
};
//////////////////////////////////////////////////////////////////////////
class MannerValuationControlWnd : public ioWnd
{
public:
	enum
	{
		ID_LEFT_ROTATE_BTN  = 1,
		ID_RIGHT_ROTATE_BTN = 2,
		ID_DEFAULT_POS_BTN  = 3,
		ID_LEADER_CHANGE_BTN= 4,
		MAX_BTN = 4,
	};

	enum AniType
	{
		AT_NONE      = 0,
		AT_ALPHA_IN  = 1,
		AT_NORMAL    = 2,
		AT_ALPHA_OUT = 3,
		AT_HIDE      = 4,
	};

protected:
	FSMStateMgr m_AniState;

	bool             m_bOnline;
	UserInfoNode    *m_pUserInfo;	

	ioPlayStage		*m_PlayStage;

protected:
	void InitAniState();
	void SetChildAlpha( int iAlphaRate );

public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

	void SetDefaultPosTitle( const char* szTitle ); 
	void HideWndAnimation();
	void SetMannerValuationUserInfo( UserInfoNode *pUserInfo, bool bOnline );

	void SetPlayStage( ioPlayStage* pStage );

protected:
	virtual void OnProcess( float fTimePerSec );

public:
	MannerValuationControlWnd();
	virtual ~MannerValuationControlWnd();
};
//////////////////////////////////////////////////////////////////////////
class MannerValuationSlotBtn : public ioButton
{
public:
	enum
	{
		//
		ID_RENTAL	  = 1,
		ID_EXPERIENCE = 2,

		ID_SET_RENTAL = 3,
		ID_DEL_RENTAL = 4,
	};

	enum    // 
	{
		MAX_CHAR_LODING = 8,
	};

	enum
	{
		VIEW_WIDTH = 79,
		VIEW_HEIGHT= 189,
	};

protected:
	ioUI3DRender  *m_pUI3DRender;
	ioCreateChar  *m_aMyChar;
	CHARACTER      m_CharInfo;
	
protected:
	ioUIRenderFrame *m_pBlueNor;
	ioUIRenderFrame *m_pBlueOver;
	ioUIRenderImage *m_pBlueNorGra;
	ioUIRenderImage *m_pBlueOverGra;
	ioUIRenderImage *m_pLoadCharBack;
	ioUIRenderImage *m_pCharLoading[MAX_CHAR_LODING];
	ioUIRenderImage *m_pRentalMark;

protected:
	ioButton *m_pRentalSetBtn;
	float     m_fAlphaRate;

protected:
	bool             m_bOnline;
	UserInfoNode    *m_pUserInfo;
	ioPlayStage		*m_pPlayStage;
	int              m_iListArray;

protected:
	DWORD			m_dwCharLoadTime;
	int				m_iCharLoadArray;
	DWORD			m_dwCharAniDelayTime;
	ioHashString	m_szAwakeEffect;

protected:
	void CreateCharacter( CharInfoData &rkInfo );
	
protected:
	void SendRentalRequest();
	void SendExperience();
	void SendRentalSet( short sRentalType );
	
public:
	virtual void iwm_create();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

public:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );

public:
	void SetCharSlot( int iListArray, UserInfoNode *pUserInfo, bool bOnline, ioPlayStage * pPlayStage );
	void SetSlotButton();
	int  GetListArray(){ return m_iListArray; }

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();	
	void OnRenderSoldier( int iXPos, int iYPos );
	void OnRenderFrame( int iXPos, int iYPos );
	void OnRenderLoadingChar( int iXPos, int iYPos );

public:
	MannerValuationSlotBtn();
	virtual ~MannerValuationSlotBtn();
};
//////////////////////////////////////////////////////////////////////////
class MannerValuationLeaderSlotWnd : public ioWnd
{
public:
	enum 
	{
		ID_CHAR_CONTROL_WND     = 1,
	};

protected:
	ioUIRenderImage *m_pBackBar;
	ioUIRenderImage *m_pShadowChar;

protected:
	ioUI3DRender    *m_pUI3DRender;
	ioCreateChar    *m_aMyChar;
	CHARACTER        m_CharInfo;
	bool             m_bOnline;
	UserInfoNode    *m_pUserInfo;

	float            m_fRotateYaw;
	float            m_fPreYaw;
	float            m_fXMLRotateYaw;
	float		     m_fZPosition;

	bool             m_bLeftRotateOn;
	bool             m_bRightRotateOn;

protected:
	void CreateCharacter( CharInfoData &rkInfo );
	void EquipItem( DWORD dwItemCode, DWORD dwItemMaleCustom, DWORD dwItemFemaleCustom, int nCostumeCode=0 );
	void ReleaseItem( int iSlot );
	void RotateY( float fYaw );
	void SetCharAnimation( bool bShopMotion );

public:
	void SetLeader( UserInfoNode *pUserInfo, bool bOnline );

	void InitRotate();
	void SetCharScale( float fScale );
	void SetZPosition( float fZPos );

public:
	virtual void iwm_create();
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void ParseExtraInfo( ioXMLElement &xElement );

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	MannerValuationLeaderSlotWnd();
	virtual ~MannerValuationLeaderSlotWnd();
};
//////////////////////////////////////////////////////////////////////////
class MannerValuationChatWnd : public ioWnd
{
public:
	enum
	{
		ID_EXIT = 1,
		ID_CHAT = 2,
		ID_MEMO = 3,
	};

protected:
	bool          m_bOnline;
	UserInfoNode *m_pUserInfo;

protected:
	void OnChannelChat();
	void OnOpenMemo();

protected:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

public:
	void SetChatUser( UserInfoNode *pUserInfo, bool bOnline );

protected:
	virtual void OnRender();

public:
	MannerValuationChatWnd();
	virtual ~MannerValuationChatWnd();
};
//////////////////////////////////////////////////////////////////////////
class MannerValuationExtensionWnd : public MannerValuationParentWnd
{
public:
	enum
	{
		ID_EXIT					= 1,
		ID_HEADQUARTER          = 2,
		ID_HERO_RANKING         = 3,

		ID_HEADQUARTER_MOVE     = 4,
		ID_MEMO_CHAT			= 5,
		ID_INSERT_FRIEND		= 6,
		ID_DELETE_FRIEND        = 7,
		ID_INSERT_BESTFRIEND	= 8,
		ID_DELETE_BESTFRIEND    = 9,
		ID_INSERT_BLACK			= 10,
		ID_DELETE_BLACK         = 11,
		ID_MINUS_MANNER_TRIAL	= 12,
		ID_KICK_VOTE            = 13,

		//
		ID_GUILD_INVITE			= 14,		
		ID_GUILD_INFO			= 15,

		//
		ID_HOME_MOVE			= 16,

		//
		ID_NAMED_TITLE			= 17,

		//
		ID_TOTAL_INFO			= 100,
		ID_DETAIL_INFO			= 101,

		//
		ID_SOLDIER_SLOT_1       = 200,
		ID_SOLDIER_SLOT_2       = 201,
		ID_SOLDIER_SLOT_3       = 202,
		ID_SOLDIER_SLOT_4       = 203,
		ID_SOLDIER_SLOT_5       = 204,
		ID_SOLDIER_SLOT_6       = 205,

		//
		ID_LEADER_SLOT          = 300,

		//
		ID_VERT_SCROLL			= 1001,
		ID_SLOT_TOOLTIP         = 1002,
	};

	enum
	{
		MAX_DOT = 79,
		MAX_SLOT= 6,
	};

protected:
	ioUIRenderImage *m_pGradeIcon;
	ioUIRenderImage *m_pIconBack;
	ioUIRenderImage *m_pNoneIcon;
	ioUIRenderImage *m_pInfoOver;
	ioUIRenderImage *m_pDot;
	ioUIRenderImage *m_pMyBestFriend;
	ioUIRenderImage *m_pOtherBestFriend;
	ioUIRenderImage *m_pCoupleBestFriend;

	ioWnd *m_pGuildBtn;

	int m_iPrevGradeLevel;
	ioHashString m_szTargetID;
	UserInfoNode *m_pUserInfo;
	ioPlayStage  *m_pPlayStage;
	ioMannerTrialChatManager::TrialType     m_eTrialType;
	DWORD         m_dwChannelIndex;
	bool          m_bOnline;
	DWORD         m_dwTabID;
	int           m_iCurPage;

	DWORD         m_dwSysPopupMsg;
	ioHashString  m_szCampName;

	ioHashString m_szNamedTitle;
	BYTE m_byNamedTitlePremium;
	ioUIRenderImage *m_pNamedTitleUiMark;

	void OnHeadquartersMove();
	void OnHouseMove();
	void OnApplicationFriend();
	void _OnDeleteFriend();
	void OnDeleteFriend();
	void OnInsertBestFriend();
	void _OnDeleteBestFriend();
	void OnDeleteBestFriend();
	void OnInsertBlack();
	void OnDeleteBlack();
	void OnKickVote();
	void OnGuildInfo();
	void OnGuildInvite();
	void OnLeaderCharSelect( int iSelectArray );

	void OnRenderDot( int iXPos, int iYPos );
	void OnRenderGrade( int iXPos, int iYPos );
	void OnRenderHeroTitle( int iXPos, int iYPos );
	void OnRenderGradeInfo( int iXPos, int iYPos );
	void OnRenderKillDeathLevel( int iXPos, int iYPos );
	void OnRenderLadderInfo( int iXPos, int iYPos );
	void OnRenderUserManner( int iXPos, int iYPos );
	void OnRenderUserCamp( int iXPos, int iYPos );
	void OnRenderGuild( int iXPos, int iYPos );

	void OnRenderUserBattle( int iXPos, int iYPos );
	void OnRenderUserAward( int iXPos, int iYPos );
	void OnRenderUserLadder( int iXPos, int iYPos );
	void OnRenderUserFishing( int iXPos, int iYPos );
	void OnRenderUserExcavation( int iXPos, int iYPos );
	void OnRenderSuccession( int iXPos, int iYPos );

	void ProcessGuildButton( bool bProcess = false );
	void ProcessScroll();
	void ProcessToolTip();

public:
	static bool sg_bUpdateInfo;

public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void iwm_vscroll(DWORD dwID, int curPos );
	virtual void iwm_wheel( int zDelta );
	virtual void SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate = false );
	void SetPlayStage( ioPlayStage * pPlayStage );
	void SetButton();

protected:
	void UpdatePage( int iCurPos );

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	virtual void SetMannerValuationInfo( const ioHashString &szID, bool bOnline, ioMannerTrialChatManager::TrialType eType, DWORD dwChannelIndex = 0);
    virtual bool IsOpenState( const ioHashString &szID );
	
protected:
	void SetNamedTitleSparkle();
	void OnRenderNamedTitle( int nXPos, int nYPos, int nFriendOffsetX );

public:
	void ApplyTitle();

public:
	MannerValuationExtensionWnd();
	virtual ~MannerValuationExtensionWnd();
};
//////////////////////////////////////////////////////////////////////////
class MannerValuationSlotTooltip : public ioWnd
{
public:
	enum    // 
	{
		MAX_INFO_LODING = 8,
		SUB_INFO_SYNC_TIME = 1000,
		MAX_MEDAL = 6,

		HEIGHT_SMALL = 371,
		HEIGHT_BIG   = 422,	

		//초월각성, 진화용병시 추가되는 높이
		EXTRA_ADD_HEIGHT = 16,
	};

protected:
    struct IconData
	{	
		ioUIRenderImage *m_pIcon;
		ioHashString m_szFullName;
		IconData()
		{
			m_pIcon = NULL;
		}
	};
	IconData m_ItemGrowth[MAX_ITEM_GROWTH];
	IconData m_CharGrowth[MAX_CHAR_GROWTH];
	
	typedef std::map< ioHashString, ioUIRenderImage *> ioUIRenderImageMap;
	ioUIRenderImageMap m_MedalIconList;
	ioUIRenderImageMap m_CostumeIconList;
	ioUIRenderImageMap m_AccessoryIconList;

protected:
	ioUIRenderImage *m_pInfoLine;
	ioUIRenderImage *m_pInfoLineHroz;
	ioUIRenderImage *m_pNoneMedal;
	ioUIRenderImage *m_pIconEffect;
	ioUIRenderImage *m_pMedalSlot;
	ioUIRenderImage *m_pNumText;
	ioUIRenderImage *m_pPlusText;
	ioUIRenderImage *m_pInfoLoading[MAX_INFO_LODING];
	ioUIRenderImage *m_pCostumeSlot[MAX_INVENTORY];
	ioUIRenderImage *m_pAccessorySlot[MAX_ACC_INVENTORY];
	ioUIRenderImage *m_pCostumeMark;

	ioUIRenderFrame *m_pNoneTitleFrm;
	ioUIRenderFrame *m_pPowerUpNAwakeTitleFrm;

	ioUIRenderImage *m_pCostumeInfoLine;
	ioUIRenderImage *m_pCostumeInfoLineHroz;

	ioUIRenderImage *m_pInfoBlackLine;
	ioUIRenderImage *m_pInfoBlackLineHroz;

	DWORD            m_dwInfoLoadTime;
	int              m_iInfoLoadArray;
	DWORD            m_dwSubInfoSyncTime;

	bool			m_bExtraInfo; //초월각성, 진화용병시 true

protected:
	UserInfoNode *m_pUserInfo;
	int           m_iListArray;

protected:
	int		m_nPetCode;
	int		m_nPetLevel;
	
public:
	void SetTooltipInfo( UserInfoNode *pUserInfo, int iListArray );

protected:
	void ProcessPosition();
	void ProcessLoading();
	void ProcessSubInfoSync();

protected:
	ioUIRenderImage *GetMedalIcon( int iType );
	ioUIRenderImage *GetMedalSubIcon( int iType );

public:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();
	void OnRenderTooltip( int iXPos, int iYPos );
	void OnRenderItemGrowth( CharInfoData &rkChar, int iXPos, int iYPos );
	void OnRenderCharGrowth( CharInfoData &rkChar, int iXPos, int iYPos );
	void OnRenderMedal( CharInfoData &rkChar, int iXPos, int iYPos );
	void OnRenderGrowthPoint( int iXPos, int iYPos, int iGrowthPoint );

protected:
	ioUIRenderImage *GetCostumeIcon( int nCostumeCode );
	void OnRenderCostume( CharInfoData &rkChar, int iXPos, int iYPos );

protected:
	ioUIRenderImage *GetAccessoryIcon( int iAccessoryCode );
	void OnRenderAccessory( CharInfoData &rkChar, int iXPos, int iYPos );

public:
	MannerValuationSlotTooltip();
	virtual ~MannerValuationSlotTooltip();
};
