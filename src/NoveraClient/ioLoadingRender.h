#pragma once

class ioUIRenderImage;
class ioUIRenderFrame;

class ioLoadingRender
{
public:
	enum
	{
		DEF_SCREEN_W  = 1024,
		DEF_SCREEN_H  = 768,
		FIRST_BACK_W  = 1448,
		FIRST_BACK_H  = 678,
		CHNAGE_HELP_Y_OFFSET = 270,
	};

	enum
	{
		STATE_LOGO		 = 1,
		STATE_REGULATION = 2,
		STATE_LOADING	 = 3,
	};

protected:
	ioUIRenderImage *m_pGameLimit;
	ioUIRenderImage *m_pLoadGaugeBack;
	ioUIRenderImage *m_pLoadGauge;

	ioUIRenderImage *m_pHelpBack;
	ioUIRenderImage *m_pTitle;
	ioUIRenderImage *m_pTitleSub;
	ioUIRenderImage *m_pTitleLine;
	ioUIRenderImage *m_pModeIcon;
	ioUIRenderImage *m_pMiniMap;
	ioUIRenderImage *m_pHeroMark;
	ioUIRenderImage *m_pShuffleTitle;
	
	ioHashString     m_szHelpDesc;
	ioHashString     m_CurMapFileName;
	ioHashString     m_szChangeServerHelp;

	//camp
	ioUIRenderImage *m_pCampVersus;
	ioUIRenderImage *m_pCampBlueTeam;
	ioUIRenderImage *m_pCampRedTeam;
	
	int m_iHelpXPos;
	int m_iHelpYPos;
	int m_iTitleXPos;
	int m_iTitleYPos;
	int m_iTitleSubXPos;
	int m_iTitleSubYPos;
	int m_iTitleLineXPos;
	int m_iTitleLineYPos;
	int m_iModeIconXPos;
	int m_iModeIconYPos;

	DWORD m_dwFirstState;
	DWORD m_dwCurLogoTime;
	DWORD m_dwLoadingTime;
	int   m_iLogoCurRate;
	int   m_iCurrentRate;
	int   m_iFullRate;
	bool  m_bCampBattle;
	bool  m_bCampHeroBattle;

	int m_iModeType;
	int m_iMyRoomType;

	// 팁관련 멤버 - megamegy
	IORandom m_TipRandom;
	int m_nTipRandomSeed;
	ioUIRenderImage *m_pTipBack;
	bool m_isAppearTip;
	ioComplexStringPrinter m_TipPrinter;

protected:
	typedef std::map< int, ioHashString > ModePathMap;
	ModePathMap m_ModePathMap;

protected:
	//1vs1
	ioUIRenderImage *m_pSuccessionRevenge;
	int m_iRevengeTitleXPos;
	int m_iRevengeTitleYPos;
public:
	void RenderFirstLoading( ioRenderSystem *pRenderSystem, ioUIRenderer *pUIRenderer, int iCurRate, int iFullRate, DWORD dwLogoTime, DWORD dwRegulationTime );
	bool IsCloseFirstLoading();
		
	void RenderModeLoading( int iModeType, int iSubNum, int iModeMapNum, bool bCampBattle, bool bCampHeroBattle, PlazaType ePlazaType, MyRoomType eMyRoomType, bool bExitRoomBySearchMode = false );

	//로딩 배경이미지 설정
	void LoadLoadingMapImage( ioINILoader& rkLoader, const ioHashString& szTitle, int iModeType, int iSubNum, bool bExitRoomBySearchMode );

	//테투리, 모드 이름 등 이미지 설정
	void LoadLoadingModePart( ioINILoader& rkLoader, const ioHashString& szTitle, int iModeType, int iSubNum );

	//모드 아이콘
	void LoadLoadingModeIcon( ioINILoader& rkLoader, const ioHashString& szTitle, int iModeType, PlazaType ePlazaType, MyRoomType eMyRoomType, bool bCampHeroBattle );

	//모드 설명
	void LoadLoadingModeDesc( ioINILoader& rkLoader, const ioHashString& szTitle, int iModeType, int iSub );

	//모드 공통 사용정보
	void LoadLoadingModeCommon( ioINILoader& rkLoader, const ioHashString& szTitle, int iModeType );

	//오늘의 모드 로딩
	void LoadLoadingShuffle( ioINILoader& rkLoader, const ioHashString& szTitle, int eMyRoomType );

	//진영전
	void LoadLoadingCamp( ioINILoader& rkLoader, const ioHashString& szTitle, int iModeType );

	//미니맵
	void LoadModeToolTip( const ioHashString& szTitle );

	//로딩 툴팁
	void LoadMiniMapLoading( int iModeType, int iSubNum, int iModeMapNum );

	void ClearResource();
	DWORD GetLoadingBackColor();
	void SetChangeServerHelp( const ioHashString &rsHelp ) { m_szChangeServerHelp = rsHelp; }

	void LoadModeINIPATH();

	// 팁관련 초기화 - megamegy
	void InitTipInfo();

protected:
	const char* GetModeINIPATH( ModeType eType );
	const char* GetModeMapINIFileName( int iModeType, int iSubNum, int iModeMapNum );

public:
	void RenderFirstBackGround( int iBackAlpha = MAX_ALPHA_RATE );
	void RenderLoading( bool bExitRoomBySearchMode );
	void RenderGameLimit( int iBackAlpha = MAX_ALPHA_RATE );
	void RenderLoadBar( float fRate );
	void RenderShuffleProcess( int iXPos, int iYPos );

protected:
	bool IsLoadedBar() const;

public:
	ioLoadingRender();
	virtual ~ioLoadingRender();
};

