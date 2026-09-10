#pragma once

#include "../io3DEngine/ioMovingWnd.h"

class SP2Packet;
class ioPlayStage;
class ioUIRenderImage;
class ioUIRenderFrame;
class TournamentNode;

class CampMyInfoBtn : public ioButton
{
protected:
	ioHashString m_szCampName;

protected:
	virtual void OnRender();

protected:
	void OnCampRender( int iXPos, int iYPos );
	void OnGuildRender( int iXPos, int iYPos );
	void OnTournamentPointRender( int iXPos, int iYPos );
	void OnCampRankRender( int iXPos, int iYPos );

public:
	void SetCampName( ioHashString szCampName );

public:
	CampMyInfoBtn();
	virtual ~CampMyInfoBtn();
};

class LobbyCampInfoWnd : public ioWnd
{
	// 로비의 진영전 탭에서 표시할 정보 UI
public:
	enum
	{
		ID_LEAVE_CAMP_BTN	= 1,
		ID_CAMP_TOOLTIP_BTN	= 2,
		ID_CREATE_GUILD_BTN	= 3,

		ID_MY_CAMP_BTN			= 10,
		ID_MY_GUILD_BTN			= 11,
		ID_TOURNAMENT_POINT_BTN = 12,
		ID_MY_CAMP_RANK_BTN		= 13,
	};

	enum
	{
		MB_OPEN_GUILD_INFO   = 1,
		MB_LEAVE_CAMP        = 2,
	};

protected:
	ioUIRenderImage *m_pNoneCamp;
	ioUIRenderImage *m_pCampIcon;
	ioUIRenderImage *m_pCampBack;

	ioUIRenderImage *m_pCloseBox;
	ioUIRenderImage *m_pOpenBox;
	ioUIRenderImage *m_pTodayBox;

	DWORD m_dwMsgBoxID;
	DWORD m_dwSelectIndex;

	ioHashString m_szTournamentTitle;
	ioHashString m_szCampName;

	DWORD m_StateStartYear;	
	DWORD m_StateStartMonth;
	DWORD m_StateStartDay;
	ioHashString m_szStartWeekDayOfWeek;

	DWORD m_StateEndYear;
	DWORD m_StateEndMonth;
	DWORD m_StateEndDay;
	DWORD m_StateEndHour;
	ioHashString m_szEndWeekDayOfWeek;

protected:
	bool  m_bScreen;
	DWORD m_dwScreenTime;

public:
	virtual void iwm_show();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );


protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	

protected:
	void OnRenderNoneCamp( int iXPos, int iYPos );
	void OnRenderCampInfo( int iXPos, int iYPos );	

protected:
	void UpdateMainUI();
	void UpdateGauge();
	void UpdateGuildButton();

public:
	LobbyCampInfoWnd();
	virtual ~LobbyCampInfoWnd();
};
//////////////////////////////////////////////////////////////////////////
class CampMainWnd : public ioWnd
{
public:
	enum
	{
		ID_CAMP_INFO_BTN   = 1,
		ID_CAMP_TEAM_BTN   = 2,		
		ID_HERO_MATCH_BTN  = 3,

		ID_CAMP_TEAM_WND   = 10,
		ID_CAMP_INFO_WND   = 11,
		ID_HERO_MATCH_WND  = 12,
		ID_CAMP_ENTER_WND  = 13,
	};

protected:
	DWORD m_dwTabID;

	int m_iMyCampPos;

public:
	void CheckCampButton( DWORD dwID );

public:
	virtual void iwm_create();
	virtual void iwm_show();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnProcess( float fTimePerSec );

public:
	void SetLadderTeamList( SP2Packet &rkPacket );

public:
	CampMainWnd();
	virtual ~CampMainWnd();
};
//////////////////////////////////////////////////////////////////////////
class InfluenceBtn : public ioButton
{
protected:
	DWORD m_dwRank;

public:
	void SetRank( DWORD dwRank );

protected:
	virtual void OnRender();

public:
	InfluenceBtn();
	~InfluenceBtn();
};

//////////////////////////////////////////////////////////////////////////
class InfluenceWnd : public ioWnd
{
public:
	enum
	{
		ID_GUILD_RANK_BTN	= 100,
		ID_CAMP_RANK_BTN	= 101,
		ID_LADDER_RANK_BTN	= 102,
	};

protected:

	DWORD m_dwSelectIndex;

	ioUIRenderFrame *m_pMainBannerFrm;

	ioUIRenderImage *m_pBanner;
	ioUIRenderImage *m_pHorzLine;
	ioUIRenderImage *m_pTextBack;

	ioUIRenderImage *m_pGagueBack;
	ioUIRenderImage *m_pGagueBlue;
	ioUIRenderImage *m_pGagueBlueAlpha;
	ioUIRenderImage *m_pGagueRed;
	ioUIRenderImage *m_pGagueRedAlpha;
	
	ioHashString m_szBlueCampName;
	ioHashString m_szRedCampName;
	
	DWORD m_dwBlueUserCnt;
	DWORD m_dwRedUserCnt;

	bool m_bScreen;
	DWORD m_dwScreenTime;
		
public:
	virtual void iwm_show();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );

public:
	void OnUpdateProcess();	
	void OnUpdateGauge();

	void OnMainBannerRender( int iXPos, int iYPos );
	void OnExplainRender( int iXPos, int iYPos );
	void OnScheduleRender( int iXPos, int iYPos );

protected:
	void UpdateMainUI();

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();
		
public:
	InfluenceWnd();
	virtual ~InfluenceWnd();
};
//////////////////////////////////////////////////////////////////////////
class CampEnterResultWnd : public ioWnd 
{
public:
	enum
	{
		ID_EXIT  = 1,
		ID_CLOSE = 2,
		ID_CAMP	 = 3,
	};

protected:
	ioUIRenderImage *m_pCampMarkBack;
	ioUIRenderImage *m_pBlueCampMark;
	ioUIRenderImage *m_pRedCampMark;

	ioHashString m_szBlueCampName;
	ioHashString m_szRedCampName;

public:
	virtual void iwm_show();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void OnRender();

public:
	CampEnterResultWnd();
	virtual ~CampEnterResultWnd();
};
//////////////////////////////////////////////////////////////////////////
class CampEnterAlarmWnd : public ioWnd
{
public:
	enum
	{
		ID_EXIT = 1,
		ID_ENTER_DECISION = 2,
		ID_CANCEL = 3,
	};

protected:
	ioUIRenderImage *m_pBlueCampMark;
	ioUIRenderImage *m_pRedCampMark;
	ioUIRenderImage *m_pCampBack;

	ioHashString m_szBlueCampName;
	ioHashString m_szRedCampName;

protected:
	CampType m_eCampType;

public:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	
public:
	void SetCampType( ioUIRenderImage *pBlueCampMark,
					  ioUIRenderImage *pRedCampMark,
					  ioHashString szBlueCampName,
					  ioHashString szRedCampName,
					  CampType eCampType );

protected:	
	virtual void OnRender();

public:
	CampEnterAlarmWnd();
	virtual ~CampEnterAlarmWnd();
};
//////////////////////////////////////////////////////////////////////////
class CampEnterWnd : public ioWnd
{
public:
	enum
	{
		ID_BLUE_ENTER  = 1,
		ID_RED_ENTER   = 2,
		ID_CAMP_HELP   = 3,
		ID_SHOW_GUILD  = 4,
		ID_AGREE_CHECK = 5,
		ID_REGULATIONS = 6,
	};

protected:
	DWORD m_dwSelectIndex;

	ioUIRenderFrame *m_pMainBannerFrm;
	ioUIRenderFrame *m_pScheduleBack;

	ioUIRenderImage *m_pBanner;
	ioUIRenderImage *m_pHorzLine;
	ioUIRenderImage *m_pTextBack;

	ioHashString m_szTournamentTitile;

	ioHashString m_szBlueCampName;
	ioHashString m_szRedCampName;
	
	DWORD m_StartMonth;
	DWORD m_StartDay;
	ioHashString m_szStartWeekDayOfWeek;
	
	DWORD m_EndMonth;
	DWORD m_EndDay;	
	DWORD m_EndHour;

	ioHashString m_szEndWeekDayOfWeek;

public:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );

	virtual void iwm_show();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

public:
	void OnUpdateProcess();	

	void OnMainBannerRender( int iXPos, int iYPos );
	void OnExplainRender( int iXPos, int iYPos );
	void OnScheduleRender( int iXPos, int iYPos );

protected:
	void UpdateMainUI();

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	CampEnterWnd();
	virtual ~CampEnterWnd();
};
//////////////////////////////////////////////////////////////////////////

class CampEnterModalWnd : public ioWnd
{
public:
	enum
	{
		ID_BLUE_ENTER = 1,
		ID_RED_ENTER  = 2,
		ID_POSTPONE	  = 3,		
		ID_EXIT		  = 10,
	};

protected:
	DWORD m_dwSelectIndex;

	ioUIRenderFrame *m_pMainBannerFrm;
	ioUIRenderFrame *m_pScheduleBack;

	ioUIRenderImage *m_pBanner;
	ioUIRenderImage *m_pHorzLine;
	ioUIRenderImage *m_pTextBack;

	ioHashString m_szTournamentTitile;

	ioHashString m_szBlueCampName;
	ioHashString m_szRedCampName;

	DWORD m_StartMonth;
	DWORD m_StartDay;
	ioHashString m_szStartWeekDayOfWeek;

	DWORD m_EndMonth;
	DWORD m_EndDay;	
	DWORD m_EndHour;
	ioHashString m_szEndWeekDayOfWeek;

public:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );

	virtual void iwm_show();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

public:
	void OnUpdateProcess();	

	void OnMainBannerRender( int iXPos, int iYPos );
	void OnExplainRender( int iXPos, int iYPos );
	void OnScheduleRender( int iXPos, int iYPos );

protected:
	void UpdateMainUI();

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	CampEnterModalWnd();
	virtual ~CampEnterModalWnd();
};

//////////////////////////////////////////////////////////////////////////
class CampSeasonBonus : public ioWnd
{
public:
	enum
	{
		ID_EXIT				= 1,
		ID_OK				= 2,
	};
protected:
	ioUIRenderImage *m_pBlueCampMark;
	ioUIRenderImage *m_pRedCampMark;
	ioUIRenderImage *m_pMapLeft;
	ioUIRenderImage *m_pMapCenter1;
	ioUIRenderImage *m_pMapCenter2;
	ioUIRenderImage *m_pMapCenter3;
	ioUIRenderImage *m_pMapCenter4;
	ioUIRenderImage *m_pMapCenter5;
	ioUIRenderImage *m_pMapCenter6;
	ioUIRenderImage *m_pMapCenter7;
	ioUIRenderImage *m_pMapRight;
	ioUIRenderImage *m_pIconBack;
	ioUIRenderImage *m_pCampPointIcon;
	ioUIRenderImage *m_pCampRankIcon;
	ioUIRenderImage *m_pCampBonusIcon;
	ioUIRenderImage *m_pBlueCampText;
	ioUIRenderImage *m_pRedCampText;
	ioUIRenderImage *m_pWinText;
	ioUIRenderImage *m_pLoseText;
	ioUIRenderImage *m_pDrawText;

protected:
	int m_iSeasonBonus;
	int m_iBlueCampPoint;
	int m_iRedCampPoint;
	int m_iMyCampType;
	int m_iMyCampPoint;
	int m_iMyCampRank;

	ioHashString m_szBlueCampName;
	ioHashString m_szRedCampName;

protected:
	float GetCampInfluencePoint( CampType eCampType );

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void OnRender();
	
protected:
	void OnRenderMent( int iXPos, int iYPos );
	void OnRenderMap( int iXPos, int iYPos );
	void OnRenderIcon( int iXPos, int iYPos );

public:
	void SetSeasonBonus( int iSeasonBonus, int iBlueCampPoint, int iRedCampPoint, int iMyCampType, int iMyCampPoint, int iMyCampRank );
	
public:
	inline const int &GetSeasonBonus(){ return m_iSeasonBonus; }
	inline const int &GetBlueCampPoint(){ return m_iBlueCampPoint; }
	inline const int &GetRedCampPoint(){ return m_iRedCampPoint; }
	inline const int &GetMyCampType(){ return m_iMyCampType; }
	inline const int &GetMyCampPoint(){ return m_iMyCampPoint; }
	inline const int &GetMyCampRank(){ return m_iMyCampRank; }

public:
	CampSeasonBonus();
	virtual ~CampSeasonBonus();
};
//////////////////////////////////////////////////////////////////////////
class CampJoinNeedPopup : public ioWnd
{
public:
	enum
	{
		ID_EXIT  = 1,
		ID_LINK  = 2,
		ID_CANCEL= 3,
	};

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnRender();

public:
	CampJoinNeedPopup();
	virtual ~CampJoinNeedPopup();
};

