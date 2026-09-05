#pragma once

class LobbyBtn : public ioRadioButton
{	
protected:
	ioUIRenderImage *m_pIconBack;
	ioUIRenderImage *m_pIcon;
	ioUIRenderImage *m_pOverImg;
	ioUIRenderImage *m_pOverAddImg;
	ioUIRenderFrame *m_pStateGreenFrm;
	ioUIRenderFrame *m_pStateRedFrm;
	ioUIRenderFrame *m_pStateOrangeFrm;
	ioUIRenderFrame *m_pStatePinkFrm;

	int m_iSelectPosY;

public:
	DWORD GetBtnNumber(DWORD dwID);
public:
	virtual void ReSetPosition(DWORD dwID);
protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );

public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_create();

protected:
	virtual void OnRender();

public:
	LobbyBtn();
	virtual ~LobbyBtn();
};
//////////////////////////////////////////////////////////////////////////
class IconBtn : public ioButton
{
protected:
	ioUIRenderImage *m_pIconBack;
	ioUIRenderImage *m_pIcon;
	float            m_fIconScale;
	float            m_fIconBackScale;

public:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void ParseExtraInfo( ioXMLElement &xElement );

protected:
	virtual void OnRender();

public:
	IconBtn();
	virtual ~IconBtn();
};
//////////////////////////////////////////////////////////////////////////
class LobbyWnd : public ioWnd
{
public:
	enum
	{
		ID_BATTLE_BTN			= 1,
		ID_CAMP_BTN				= 2,
		ID_HERO_MAIN_BTN		= 3,
		ID_PLAZA_BTN			= 4,
		ID_TRADE_BTN			= 5,
		ID_HEADQUARTER_BTN		= 6,
		ID_TOURNAMENT_BTN		= 7,
		ID_GUILD_HEADQUATER_BTN	= 8,
		ID_MY_HOME_BTN			= 9,

		ID_BATTLE_WND			= 10,
		ID_CAMP_MAIN_WND		= 11,
		ID_PLAZA_WND			= 12,
		ID_HERO_MAIN_WND		= 14,
		ID_TRADE_MAIN_WND		= 15,
		ID_TOURNAMENT_WND		= 16,
		ID_PRACTICE_BTN		= 17,
		ID_PRACTICE_WND		= 18,
		ID_BONUS_CASH_BTN		= 19,

	};

protected:
	ioUIRenderFrame *m_pCustomTitle;
	ioUIRenderImage *m_pTabBack1;
	ioUIRenderImage *m_pTabBack2;
	ioUIRenderImage *m_pTabBack3;
	ioUIRenderImage *m_pTabBack4;

	ioUIRenderImage *m_pMajorMark;
	ioUIRenderImage *m_pMinorGrade;
	ioUIRenderImage *m_pGaugeBack;
	ioUIRenderImage *m_pGauge;

	int              m_iPrevGradeLevel;

protected:
	DWORD m_dwTabID;

protected:
	int m_iMyCampPos;
	bool m_bMileage;

public:
	void RefreshTab();
	DWORD GetTabID(){ return m_dwTabID; }
	void SetTabID( DWORD dwID );

protected:
	void ChangeRadioWnd( DWORD dwID );
	void RenderGradeExp( int iXPos, int iYPos );

protected:
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	

public:
	virtual void iwm_create();
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnProcess( float fTimePerSec );

protected:
	virtual void OnRender();
	virtual void OnRenderAfterChild();

protected:
	void OnCreateHeadquarters();
	void OnCreateGuildHeadquarters();
	void OnCreateHouse();
	void UpdateGuildHQButton();
	void UpdateHomeButton();
	void CheckBonusCash();

public:
	void ApplyNormalTeamList( SP2Packet &rkPacket );
	void ApplyPlazaList( SP2Packet &rkPacket );
	void ApplyLadderTeamList( SP2Packet &rkPacket );
	
	void ApplyTournamentList( SP2Packet &rkPacket );
	void ApplyTournamentConfirmCheck( SP2Packet &rkPacket );
	void ApplyTournamentConfirmRequest( SP2Packet &rkPacket );

	void OnRefreshTradeItemList();

public:
	void RefreshBattleList( DWORD dwIndex );
	void SetCampLink();
	void SetTournamentLink( DWORD dwTourIndex );

public:
	bool QuestGuide( int iModeSelectIndex, float &rfXPos, float &rfYPos );

public:
	LobbyWnd();
	virtual ~LobbyWnd();
};
//////////////////////////////////////////////////////////////////////////
class LobbyChatWnd : public ioWnd
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
	LobbyChatWnd();
	virtual ~LobbyChatWnd();
};
//////////////////////////////////////////////////////////////////////////
class LobbyMainWnd : public ioMovingWnd
{
public:
	enum
	{
		ID_LOBBY_WND		= 1,
		ID_CHAT_WND			= 2,
		ID_CAMP_INFO_WND	= 3,
		ID_BATTLE_INFO_WND  = 4,
		ID_PLAZA_INFO_WND   = 5,
		ID_TRADE_INFO_WND   = 6,
		ID_TOURNAMENT_INFO_WND = 7,
		ID_PRACTICE_INFO_WND = 8,

		ID_MODE_HELP_TOOLTIP = 500,
		ID_QUICKJOIN_TOOLTIP = 501,
		ID_EFFECT_WND		= 504,
		ID_BONUS_CASH_TOOLTIP = 505,
	};

protected:
	enum 
	{
		POS_SAVE_VERSION = 4,
	};

	int	  m_iDefaultX;
	int   m_iDefaultY;

protected:
	ioPlayStage*	m_pPlayStage;	
	ioWnd*			m_pModeHelpTooltip;
	ioWnd*			m_pQuickJoinTooltip;

	ioHashString	m_szBackOgg;

	// 이벤트용 이펙트
	ioUI3DEffectRender * m_pEventEff;

protected:
	//보너스 캐쉬
	DWORD			m_dwBonusCashTooltipCheckDuration;
	DWORD			m_dwBonusCashTooltipCheckStartTime;

protected:
	void CmdMainTab( DWORD dwTabID );

public:	
	virtual void iwm_create();
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual bool iwm_esc();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate = false );

public:
	void ApplyNormalTeamList( SP2Packet &rkPacket );
	void ApplyPlazaList( SP2Packet &rkPacket );
	void ApplyLadderTeamList( SP2Packet &rkPacket );
	void ApplyTournamentList( SP2Packet &rkPacket );
	void ApplyTournamentConfirmCheck( SP2Packet &rkPacket );
	void ApplyTournamentConfirmRequest( SP2Packet &rkPacket );

	void OnRefreshTradeItemList();

public:
	void RefreshBattleList( DWORD dwIndex = 0 );

public:
	void SetModeHelpTooltip( ioWnd *pCallWnd, const ioHashString &szTitle, int iQuickstartModeHelpIndex );
	void SetQuickJoinTooltip(ioWnd *pCallWnd, const int nMode );

public:
	void ShowChatWnd( bool bShow );
	void PlayBackgroundOGG();
	void RefreshTab();
	void SetCampLink();
	void SetTournamentLink( DWORD dwTourIndex );

protected:
//보너스캐쉬
	void CheckBonusCashTooltip();


public:
	virtual void ParseExtraInfo( ioXMLElement &xElement );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	void SetPlayStage( ioPlayStage *pPlayStage );

public:
	bool QuestGuide( int iModeSelectIndex, float &rfXPos, float &rfYPos );
	ioWnd* GetPracticeInfoWnd();

public:
	LobbyMainWnd();
	virtual ~LobbyMainWnd();
};

