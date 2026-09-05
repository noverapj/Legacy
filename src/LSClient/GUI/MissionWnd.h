#pragma once

#include "../ioMission.h"

class ioUIRenderFrame;
class ioUIRenderImage;

class MissionQuestMainWnd :	public ioMovingWnd
{
public:
	enum
	{
		ID_EXIT = 1,		
		ID_MISSION_BTN = 2,
		ID_QUEST_BTN = 3,

		ID_MISSIONLIST_WND = 9,
		ID_QUESTLIST_WND = 16,
	};

protected:
	DWORD m_dwTabID;

protected:
	void			ChangeRadioWnd( DWORD dwID );
	void			ChangeTitle( DWORD dwID );
	void			UpdateRadioBtn();
protected:
	virtual void OnProcess( float fTimePerSec );

public:
	virtual void	iwm_create();
	virtual void	iwm_show();
	virtual void	iwm_hide();
	virtual void	iwm_command( ioWnd *pWnd, int cmd, DWORD param );

public:
	MissionQuestMainWnd();
	virtual ~MissionQuestMainWnd();
};
//////////////////////////////////////////////////////////////////////////
class MainMissionWnd : public ioWnd
{
protected:
	enum
	{	
		ID_PRE_BTN		= 1,
		ID_NEXT_BTN		= 2,

		ID_DAILY_MISSION_BTN = 3,
		ID_WEEKLY_MISSION_BTN = 4,
		ID_MONTHLY_MISSION_BTN = 5,

		ID_MISSION_INFO_BTN1 = 10,
		ID_MISSION_INFO_BTN2 = 11,
		ID_MISSION_INFO_BTN3 = 12,
		ID_MISSION_INFO_BTN4 = 13,
		ID_MISSION_INFO_BTN5 = 14,
		ID_MISSION_INFO_BTN6 = 15,

		ID_MISSION_TOOLTIP	 = 20,
	};

	enum
	{
		MAX_MISSION_CNT = 5,
	};

	DWORD					m_dwTabID;
	int						m_iMissionDateType;

	int						m_iCurPage;
	int						m_iMaxPage;
	ioComplexStringPrinter	m_PagePrinter;

protected:
	void			ChangeRadioBtn( DWORD dwID );
	void			InitMissionInfoBtn();
	void			UpdateMissionInfo();
	void			UpdateRadioBtn();
public:
	virtual void	iwm_show();
	virtual void	iwm_hide();
	virtual void	iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void	OnProcess( float fTimePerSec );
	virtual void	OnRender();
public:
	MainMissionWnd();
	virtual ~MainMissionWnd();
};
//////////////////////////////////////////////////////////////////////////
class MissionInfoBtn : public ioButton
{
protected:
	enum
	{
		ID_ATTAIN_BTN		= 1,
	};
	enum
	{
		TITLE_CNT = 2,
	};

	ioUIRenderImage			*m_pIconBack;
	ioUIRenderImage			*m_pMissionIcon;
	ioUIRenderImage			*m_pIconEmpty;
	ioUIRenderImage			*m_pCompleteImg;
	ioUIRenderImage			*m_pGauegeBack;
	ioUIRenderImage			*m_pGauegeBar;
	ioUIRenderImage			*m_pTextNum;
	ioUIRenderImage			*m_pTextPercent;
	ioUIRenderImage			*m_pToolTip;
	ioUIRenderImage			*m_pCostumeMark;

	ioUIRenderFrame			*m_pCurFrm;
	ioUIRenderFrame			*m_pYellowBackFrm;
	ioUIRenderFrame			*m_pNormalBackFrm;

	MissionAttainShowStyle	m_AttainShowStyle;

	DWORD					m_dwMissionMagicCode;
	ioComplexStringPrinter	m_TitlePrinter[ TITLE_CNT ];

	int						m_iMaxValue;
	int						m_iCurValue;
	BYTE					m_byClearType;
	float					m_fGaugeBarRate;
	float					m_fLineCut;

	typedef std::map< ioHashString, ioUIRenderImage * > MissionIconMap; 
	MissionIconMap			m_MissionIconMap;

	bool					m_bIsCostume;

protected:
	void				ClearData();
	void				ClearIconMap();
	ioUIRenderImage*	GetMissionIcon(  const ioHashString &szIconName );

	void				OnRenderAttain( int iXPos, int iYPos );
public:
	void				SetMission( const ioMission* const rkMissionData );
protected:
	void				SetMissionTitle( const ioMission* const rkMissionData );
public:
	const DWORD&		GetMissionCode() const { return m_dwMissionMagicCode; }

//Virtual Func
protected:
	virtual void		AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void		AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );
	virtual void		OnRender();
	virtual void		OnDrawNormal( int iXPos, int iYPos );
	virtual void		OnDrawOvered( int iXPos, int iYPos );
	virtual void		OnDrawPushed( int iXPos, int iYPos );
public:
	virtual void		iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void		iwm_hide();

public:
	MissionInfoBtn();
	virtual ~MissionInfoBtn();
};
//////////////////////////////////////////////////////////////////////////
class MissionInfoRewardBtn : public ioButton
{
protected:
	enum
	{
		LOBBY_OUTLINE_TYPE = 0,	//LobbyMainMissionWnd 의 MissionInfoRewardBtn
		POPUP_OUTLINE_TYPE = 1, //MainMissionWnd 의 MissionInfoRewardBtn
	};
	DWORD					m_dwAlphaLoopStartTime;
	DWORD					m_dwAlphaLoopTicTime;
	BYTE					m_byOutLineScaleType;
	float					m_fAlphaSpeed;
	float					m_fCurAlpha;
	ioUIRenderFrame			*m_pRewardOutLine;

protected:
	virtual void	AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );
	virtual void	ParseExtraInfo( ioXMLElement &xElement );
	virtual void	OnProcess( float fTimePerSec );
	virtual void	OnRender();
	virtual void	OnDrawDisabled( int iXPos, int iYPos );
public:
	virtual void	iwm_create();
	virtual void	iwm_show();
public:
	MissionInfoRewardBtn();
	virtual ~MissionInfoRewardBtn();
};
//////////////////////////////////////////////////////////////////////////
class LobbyMissionInfoBtn : public ioButton
{
protected:
	enum
	{
		ID_ATTAIN_BTN		= 1,
	};
	enum
	{
		TITLE_CNT = 2,
	};
	ioUIRenderImage			*m_pIconBack;
	ioUIRenderImage			*m_pMissionIcon;
	ioUIRenderImage			*m_pIconEmpty;
	ioUIRenderImage			*m_pCompleteImg;

	ioUIRenderImage			*m_pGauegeBack;
	ioUIRenderImage			*m_pGauegeBar;

	ioUIRenderImage			*m_pTextNum;
	ioUIRenderImage			*m_pTextPercent;

	ioUIRenderImage			*m_pToolTip;
	ioUIRenderImage			*m_pCostumeMark;

	ioUIRenderFrame			*m_pCurFrm;
	ioUIRenderFrame			*m_pYellowBackFrm;
	ioUIRenderFrame			*m_pNormalBackFrm;

	MissionAttainShowStyle	m_AttainShowStyle;

	DWORD					m_dwMissionMagicCode;
	ioComplexStringPrinter	m_TitlePrinter[ TITLE_CNT ];

	int						m_iMaxValue;
	int						m_iCurValue;
	BYTE					m_byClearType;
	float					m_fGaugeBarRate;
	float					m_fLineCut;

	typedef std::map< ioHashString, ioUIRenderImage * > MissionIconMap; 
	MissionIconMap			m_MissionIconMap;

	bool					m_bIsCostume;

protected:
	void				ClearData();
	void				ClearIconMap();
	ioUIRenderImage*	GetMissionIcon(  const ioHashString &szIconName );

	void				OnRenderAttain( int iXPos, int iYPos );
public:
	void				SetMission( const ioMission* const rkMissionData );
	void				SetMissionTitle( const ioMission* const rkMissionData );
public:
	const DWORD&		GetMissionCode() const { return m_dwMissionMagicCode; }

	//Virtual Func
protected:
	virtual void		AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void		AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );
	virtual void		OnRender();
	virtual void		OnDrawNormal( int iXPos, int iYPos );
	virtual void		OnDrawOvered( int iXPos, int iYPos );
	virtual void		OnDrawPushed( int iXPos, int iYPos );
public:
	virtual void		iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void		iwm_hide();

public:
	LobbyMissionInfoBtn();
	virtual ~LobbyMissionInfoBtn();
};
//////////////////////////////////////////////////////////////////////////////////
class LobbyMainMissionWnd : public ioWnd
{
protected:
	enum
	{	
		ID_PRE_BTN		= 1,
		ID_NEXT_BTN		= 2,

		ID_DAILY_MISSION_BTN = 3,
		ID_WEEKLY_MISSION_BTN = 4,
		ID_MONTHLY_MISSION_BTN = 5,

		ID_MISSION_INFO_BTN1 = 10,
		ID_MISSION_INFO_BTN2 = 11,
		ID_MISSION_INFO_BTN3 = 12,
		ID_MISSION_INFO_BTN4 = 13,
		ID_MISSION_INFO_BTN5 = 14,

		ID_MISSION_TOOLTIP	 = 20,
	};
	enum
	{
		LOBBY_MAX_MISSION_CNT = 5,
	};

	DWORD					m_dwTabID;
	int						m_iMissionDateType;

	int						m_iCurPage;
	int						m_iMaxPage;
	ioComplexStringPrinter	m_PagePrinter;


protected:
	void			ChangeRadioBtn( DWORD dwID );
	void			InitMissionInfoBtn();
	void			UpdateMissionInfo();
	void			UpdateRadioBtn();

protected:
	void			ChangeData();
	void			ChangeTitle();

public:
	virtual void	iwm_show();
	virtual void	iwm_hide();
	virtual void	iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void iwm_wheel( int zDelta );

protected:
	virtual void	OnProcess( float fTimePerSec );
	virtual void	OnRender();
public:
	void			QuestUpdateData();
public:
	LobbyMainMissionWnd();
	virtual ~LobbyMainMissionWnd();
};
//////////////////////////////////////////////////////////////////////////
class MissionAttainStateWnd : public ioWnd
{
public:
	enum
	{
		ID_REWARD_BTN	= 1,

		ID_ALPHA_WND    = 100,
	};

	enum
	{
		WND_HEIGHT_A = 145,
		WND_HEIGHT_B = 23,
		WND_HEIGHT_C = 58,
		HELP_MENT_GAP= 19,
		REWARD_GAP   = 45,
	};

	enum
	{
		STATE_NONE  = 0,
		STATE_SCALE = 1,
		STATE_ALPHA = 2,
	};

protected:
	ioUIRenderFrame	*m_pBrownBackFrm;
	ioUIRenderFrame	*m_pBrownBottomFrm;

	ioUIRenderImage	*m_pMissionIconBack1;
	ioUIRenderImage	*m_pMissionIconBack2;
	ioUIRenderImage	*m_pCompleteText;
	
	ioUIRenderImage	*m_pRewardIcon;
	ioUIRenderImage	*m_pSubIcon;
	ioUIRenderImage	*m_pCostumeMark;

	int				m_iMissionPresentCode;
	bool			m_bIsCostume;

protected:
	DWORD			m_dwHideDelayTime;

	DWORD			m_dwState;
	DWORD			m_dwCompleteTime;
	int				m_iRewardIconWhiteAlpha;
	ioHashString	m_szRewardSnd;
	int				m_iHelpMentSize;
	int				m_iRewardSize;
	ioHashStringVec	m_RewardTextVec;

protected:
	void			SetWndReSize( const ioMission* const rkMissionData );
	ioUIRenderImage *GetRewardIcon( int iPresentType, int iPresentValue1, int iPresentValue2 );
	ioUIRenderImage *GetRewardSubIcon( int iPresentType, int iPresentValue1, int iPresentValue2 );
	void			ProcessComplete();
	void			OnRenderIcon( int iXPos, int iYPos );
	void			OnRenderRewardText( int iXPos, int iYPos );

public:
	void			ShowMissionAttainWnd( const ioMission* const rkMissionData );

	//Virtual Func
public:
	virtual void	iwm_show();
	virtual void	iwm_hide();
	virtual bool	iwm_spacebar();
	virtual void	iwm_command( ioWnd *pWnd, int cmd, DWORD param );
protected:
	virtual void	AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );
	virtual void	AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void	ParseExtraInfo( ioXMLElement &xElement );
	virtual void	OnProcess( float fTimePerSec );
	virtual void	OnRender();
	virtual bool	OnRenderShowAndHide();

public:
	MissionAttainStateWnd();
	virtual ~MissionAttainStateWnd();
};
//////////////////////////////////////////////////////////////////////////
class MissionToolTip : public ioWnd
{
public:
	enum
	{
		MAX_INFO				= 2,
	};

protected:
	float				   m_fMaxLimitRowSize;
	ioHashStringVec		   m_szTitleVec;	//타이틀
	ioHashStringVec		   m_szSubTitleVec;	//서브타이틀
	ioComplexStringPrinter m_szPresent;
	ioComplexStringPrinter m_szAttainProgression;	//달성 진행률
protected:
	void		 Clear();
	void		 ProcessPosition();
				 
public:			 
	bool		 SetInfo( const ioMission* const rkMissionData, const int& iXPos, const int& iYPos );

public:
	virtual void	iwm_hide();
protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	MissionToolTip();
	virtual ~MissionToolTip();
};
//////////////////////////////////////////////////////////////////////////
class MissionRadioBtn : public ioRadioButton
{
protected:
	bool		m_bUseSparkle;
	bool		m_bOver;
	DWORD		m_dwOverTimer;
	DWORD		m_dwOverTicTime;
	BYTE		m_byOverFrmPrintCount;

public:
	void			SetStartSparkle();
	void			SetEndSparkle();
	const bool&		IsSparkling() const { return m_bUseSparkle; }

public:
	virtual void	ParseExtraInfo( ioXMLElement &xElement );
protected:
	virtual void	OnProcess( float fTimePerSec );
	virtual void	OnRender();
public:
	MissionRadioBtn();
	virtual ~MissionRadioBtn();
};