#pragma once

#include "../ioPlayMode.h"
#include "../io3DEngine/ioMovingWnd.h"
#include "ResultInfo.h"
#include "GlobalWnd.h"

class SP2Packet;
class ioXMLElement;
class ioPlayStage;
class ioUIRenderImage;
class ioUIRenderFrame;
//////////////////////////////////////////////////////////////////////////
class RoundStateUserWnd : public ioWnd
{
protected:
	enum
	{
		TOWERDEF_EASY = 0,
		TOWERDEF_NORMAL,
		TOWERDEF_HARD,
		TOWERDEF_MASTER,
		MAX_TOWERDEF,
	};

protected:
	ioUIRenderFrame *m_pMyTeamFrm;
	ioUIRenderFrame *m_pGuildMarkFrm;

protected:
	//
	ioUIRenderImage *m_pBlueDark;
	ioUIRenderImage *m_pBlueLight;
	ioUIRenderImage *m_pBlueOver;
	ioUIRenderImage *m_pRedDark;
	ioUIRenderImage *m_pRedLight;
	ioUIRenderImage *m_pRedOver;
	
	//
	ioUIRenderImage *m_pPCRoomBlue;
	ioUIRenderImage *m_pPCRoomRed;
	ioUIRenderImage *m_pGuildBlue;
	ioUIRenderImage *m_pGuildRed;
	ioUIRenderImage *m_pFriendBlue;
	ioUIRenderImage *m_pFriendRed;
	ioUIRenderImage *m_pVictoriesBlue1;
	ioUIRenderImage *m_pVictoriesRed1;
	ioUIRenderImage *m_pVictoriesBlue2;
	ioUIRenderImage *m_pVictoriesRed2;
	ioUIRenderImage *m_pVictoriesNumBlue;
	ioUIRenderImage *m_pVictoriesNumRed;
	ioUIRenderImage *m_pModeIconBack;

	//
	ioUIRenderImage *m_pOrangeMan;
	ioUIRenderImage *m_pGrayMan;

	//
	ioUIRenderImage *m_pBlueVert;
	ioUIRenderImage *m_pRedVert;
	ioUIRenderImage *m_pMonsterModeTitle;
	ioUIRenderImage *m_pSurvivalModeTitle;
	ioUIRenderImage *m_pBossModeTitle;
	ioUIRenderImage *m_pGangsiModeTitle1;
	ioUIRenderImage *m_pGangsiModeTitle2;
	ioUIRenderImage *m_pFightModeTitle;
	ioUIRenderImage *m_pShuffleBonusModeTitle;
	ioUIRenderImage *m_pRaidModeTitle;
	ioUIRenderImage *m_pBlueNumber;
	ioUIRenderImage *m_pRedNumber;
	ioUIRenderImage *m_pBlueFloor;
	ioUIRenderImage *m_pRedRank;
	ioUIRenderImage *m_pBlueRank;
	ioUIRenderImage *m_pBluePer;
	ioUIRenderImage *m_pRedPer;
	ioUIRenderImage *m_pBluePoint;
	ioUIRenderImage *m_pRedPoint;

	ioUIRenderImage *m_pTowerDefTitle;
	ioUIRenderImage *m_pSnowTownTitle;
	ioUIRenderImage *m_pFireTempleTitle;
	ioUIRenderImage *m_pFactoryTitle;
	ioUIRenderImage *m_pTowerDefDiff[MAX_TOWERDEF];

	//
	ioUIRenderImage *m_pGoldMonsterCoin;
	ioUIRenderImage *m_pMonsterCoin;
	ioUIRenderImage *m_pRaidCoin;
	ioUIRenderImage *m_pCoinNumber;
	ioUIRenderImage *m_pCoinUnlimited;

	//
	ioUIRenderImage *m_pCurrentDark;
	ioUIRenderImage *m_pCurrentLight;
	ioUIRenderImage *m_pCurrentOver;
	ioUIRenderImage *m_pCurrentNumber;
	ioUIRenderImage *m_pCurrentNumberText;

	//
	ioUIRenderImage *m_pBlueTeam;
	ioUIRenderImage *m_pRedTeam;

	//연승전
	ioUIRenderImage *m_pSuccessionTitle;
	ioUIRenderFrame *m_pSuccessionFrm;
	//
protected:
	TeamType m_eWindowTeam;
	int      m_MouseOverIndex;
	int      m_iMaxSlot;
	int      m_iCurrentFloor;
	int      m_iCurrentRank;
	int      m_iCurrentScore;
	int      m_iCurrentLiveUser;

protected:
	int      m_iMaxLine;
	int      m_iLinePlayer;
	int      m_iLinePlayerGap;
	int      m_iLineStartX;
	int      m_iLineStartY;
	int      m_iLineWidth;
	int      m_iLineHeight;
	int      m_iNameLimit;

protected:
	ioPlayStage	  *m_pPlayStage;

	typedef std::vector< RoundInfo > RoundInfoList;
	RoundInfoList m_RoundInfoList;

protected:
	PingStepWnd m_PingStep;

//tournament data
protected:
	ioHashString m_szTournamentTeamName;

protected:
	bool IsMyTeam();
	bool IsDieUser( const ioHashString &szName );
	TeamType GetUserTeam( int iIndex );

public:
	void SetWindowTeam( TeamType eTeam );
	void SetPlazaScore( int iMaxSlot );
	void SetHeadequartersScore( int iMaxSlot );
	void SetSingleScore( int iMaxSlot );
	void SetGangsiScore( int iMaxSlot );
	void SetDungeonsScore( int iMaxSlot, int iCurrentFloor );
	void SetTeamScore( int iMaxSlot, int iScore );
	void SetPlayStage( ioPlayStage *pPlayStage );
	void AddRoundInfo( const RoundInfo &rkInfo );
	void ClearRoundInfo();

//tournament data
public:
	void SetTournamentTeamName( ioHashString szName );

public:
	virtual void iwm_create();
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void ParseExtraInfo( ioXMLElement &xElement );

protected:
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

protected:
	void ProcessSiteClick();

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

protected:
	void OnRenderBack( int iXPos, int iYPos );
	void OnRenderGangsiBack( int iXPos, int iYPos );
	void OnRenderTowerDefLine( int iXPos, int iYPos );
	int  OnRenderGradeAndNick( const RoundInfo &rkInfo, int iXPos, int iYPos, bool bOver );
	int  OnRenderGuildMark( const RoundInfo &rkInfo, int iXPos, int iYPos );
	void OnRenderBonus( const RoundInfo &rkInfo, int iXPos, int iYPos );
	void OnRenderKillDeath( const RoundInfo &rkInfo, int iXPos, int iYPos, bool bOver );
	void OnRenderContribute( const RoundInfo &rkInfo, int iXPos, int iYPos, bool bOver );
	void OnRenderTowerDefRank(const RoundInfo &rkInfo, int iXPos, int iYPos, bool bOver);
	void OnRenderFightWin( const RoundInfo &rkInfo, int iXPos, int iYPos, bool bOver );
	void OnRenderFightLose( const RoundInfo &rkInfo, int iXPos, int iYPos, bool bOver );
	void OnRenderGoal( const RoundInfo &rkInfo, int iXPos, int iYPos, bool bOver );
	void OnRenderMonsterCoin( const RoundInfo &rkInfo, int iXPos, int iYPos );
	void OnRenderLadderPoint( const RoundInfo &rkInfo, int iXPos, int iYPos, bool bOver );
	void OnRenderPing( const RoundInfo &rkInfo, int iXPos, int iYPos, bool bOver );
	void OnRenderStarCount( const RoundInfo &rkInfo, int iXPos, int iYPos, bool bOver );
	void OnRenderRaidCoinCount( const RoundInfo &rkInfo, int iXPos, int iYPos );

	// 각 모드들 타이틀
	void OnRenderMonsterTitle( int iXPos, int iYPos );
	void OnRenderTowerDefTitle( int iXPos, int iYPos );
	void OnRenderFireTempleTitle( int iXPos, int iYPos );
	void OnRenderSnowTownTitle( int iXPos, int iYPos );
	void OnRenderSurvivalTitle( int iXPos, int iYPos );
	void OnRenderBossTitle( int iXPos, int iYPos );
	void OnRenderGangsiTitle( int iXPos, int iYPos );
	void OnRenderTeamTitle( int iXPos, int iYPos, bool bPointRender=true, bool bScroreRender=true );
	void OnRenderFightClubTitle( int iXPos, int iYPos );
	void OnRenderShuffleBonusTitle( int iXPos, int iYPos );
	void OnRenderFactoryTitle( int iXPos, int iYPos );
	void OnRenderRaidTitle( int iXPos, int iYPos );
	void OnRenderSuccessionTitle( int iXPos, int iYPos );

	// 모드별로 구분
	void OnRenderPlazaUser( int iXPos, int iYPos );
	void OnRenderTeamUser( int iXPos, int iYPos );
	void OnRenderSingleUser( int iXPos, int iYPos );
	void OnRenderSoccerUser( int iXPos, int iYPos );
	void OnRenderMonsterUser( int iXPos, int iYPos );
	void OnRenderTowerDefUser( int iXPos, int iYPos );
	void OnRenderFightClubUser( int iXPos, int iYPos );
	void OnRenderShuffleBonusUser( int iXPos, int iYPos );
	void OnRenderRaidUser( int iXPos, int iYPos );
	void OnRenderSuccessionUser( int iXPos, int iYPos );

public:
	RoundStateUserWnd();
	virtual ~RoundStateUserWnd();
};
//////////////////////////////////////////////////////////////////////////
class RoundStateMainWnd : public ioWnd
{
public:
	enum
	{
		ID_ROOM_INFO  = 1,  // 방정보
		ID_PLAZA_WND  = 2,  // 광장
		ID_SINGLE_WND = 3,  // 해골영웅,보스레이드,데스매치,강시서바이벌
		ID_BLUE_WND	  = 4,  
		ID_RED_WND    = 5,		
	};

	enum
	{
		PLAZA_WIDTH = 672,
		PLAZA_HEIGHT= 275,
		MODE_WIDTH  = 670,
		MODE_HEIGHT = 326,
		TOWER_MODE_WIDTH = 562,
		TOWER_MODE_HEIGHT = 326,

		ROOM_INFO_BTN_RIGHT_GAP = 54,
	};

	enum
	{
		ID_EXIT_BTN = 1,
	};

protected:
	ioPlayStage	  *m_pPlayStage;
	
	typedef std::vector< RoundInfo > RoundInfoList;
	RoundInfoList m_RoundInfoList;

	ioComplexStringPrinter m_Title;

	//대회모드 데이터
	int m_iTournamentCurrentRound;

public:
	void SetPlayStage( ioPlayStage *pPlayStage );
	
public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnRender();

	/************************************************************************/
	/*  아래에 함수를 추가해서 모드별로 특별한 정보들을 수집한다. 예전처럼  */
	/*  XML을 계속 늘려나가면 이미지 수정하나만해도 XML을 너무 바꿔줘야해서 */
	/*  불편함..															*/
	/************************************************************************/	
public:
	virtual void ClearRoundInfo();
	virtual void AddRoundInfo( const RoundInfo &rkInfo );
	virtual void SetPlazaScore();
	virtual void SetHeadequartersScore();
	virtual void SetSingleScore();
	virtual void SetGangsiScore();
	virtual void SetTowerDefScore();
	virtual void SetDungeonsScore( int iCurrentFloor );
	virtual void SetTeamScore( int iBlueWin, int iRedWin );
	virtual void SetStarCountScore();
	virtual void SetRaidCoinCountScore();

public:
	void SetTournamentData();
	bool IsSearchShow();	// 2019-07-03

public:
	RoundStateMainWnd();
	virtual ~RoundStateMainWnd();
};
//////////////////////////////////////////////////////////////////////////
class RoundResultMainWnd : public RoundStateMainWnd
{
public:
	virtual void iwm_show();
	virtual void iwm_hide();

protected:
	virtual void OnProcess( float fTimePerSec );

public:
	virtual void SetTeamScore( int iBlueWin, int iRedWin );

public:
	RoundResultMainWnd();
	virtual ~RoundResultMainWnd();
};
