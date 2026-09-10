#pragma once

class MatchingGameCardButton : public ioButton
{
public:
	enum
	{
		MGCBS_CLOSED,
		MGCBS_OPENING,
		MGCBS_OPENED,
		MGCBS_MATCHED,
		MGCBS_CLOSING
	};

protected:
	ioUIRenderImage*	m_pCardBack;
	ioUIRenderImage*	m_pCardImage;
	ioUIRenderImage*	m_pOpenEffect;
	ioUIRenderImage*	m_pMatchEffect;

	ioHashStringVec		m_vCardOpenEffect;
	ioHashStringVec		m_vCardMatchEffect;

	DWORD	m_dwCardOpenEffectTime;
	DWORD	m_dwCardOpenEffectCheckTime;
	DWORD	m_dwCardMatchEffectTime;
	DWORD	m_dwCardMatchEffectCheckTime;

	DWORD	m_iCardOpeningTime;
	DWORD	m_iCardOpenTime;

	int		m_iCurEffectArray;

	int		m_Status;
	DWORD	m_dwStatusChangeTime;

public:
	virtual void ParseExtraInfo( ioXMLElement &xElement );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void OnProcess( float fTimePerSec );

public:
	void	SetCardImage( ioUIRenderImage* pImage );
	void	ResetCardImage();

	void	Reset();

	void	CardOpenEffect( float fTimePerSec );
	void	CardMatchEffect( float fTimePerSec );

	void	SetStatus( int iStatus );
	int		GetStatus();

	inline	DWORD	GetStatusChangeTime()	{	return	m_dwStatusChangeTime;	}

	inline	bool	IsCardImage()			{	return	m_pCardImage ? true:false;			}

protected:
	virtual void OnRender();
	virtual void OnRenderAfterChild();

public:
	MatchingGameCardButton();
	virtual ~MatchingGameCardButton();
};

class MatchingGameWnd : public ioWnd
{
public:
	enum
	{
		MAX_NUMOF_ICON	= 17,
		MAX_NUMOF_BUTTON	= 30
	};

	enum
	{
		TEXT_MISSION_SUCCESS = 0,
		TEXT_MISSION_FAILED,
		TEXT_MISSION_DESCRIPTION,
		TEXT_MISSION_REWARD,
		TEXT_LIMIT_TIME,
		TEXT_NORMAL_TICKET,
		TEXT_PREMIUM_TICKET,
		TEXT_MATCHING_COUNT,
		TEXT_GAME_EXIT_WARNING,
		TEXT_MAX
	};

	enum
	{
		PRINTER_SUCCESS_FAIL = 0,
		PRINTER_DESCRIPTION,
		PRINTER_NORMAL_TICKET,
		PRINTER_PREMIUM_TICKET,
		PRINTER_MAX
	};

	enum
	{
		CARD_MATCH_COUNT_CHECK_1	= 3,
		CARD_MATCH_COUNT_CHECK_2	= 7,
		CARD_MATCH_COUNT_CHECK_3	= 15
	};

	enum
	{
		Gauge_Max_Count	= 15,
		Gauge_Max_Length	= 288
	};

	enum
	{
		COUNT_DOWN_MAX	= 4,
		COUNT_DOWN_TIME	= 4000
	};

	enum
	{
		MAX_REWARD_ICON	= 3
	};

	enum
	{
		ID_EXIT = 1,

		ID_CARD_SELECT1 = 101,
		ID_CARD_SELECT2,
		ID_CARD_SELECT3,
		ID_CARD_SELECT4,
		ID_CARD_SELECT5,
		ID_CARD_SELECT6,
		ID_CARD_SELECT7,
		ID_CARD_SELECT8,
		ID_CARD_SELECT9,
		ID_CARD_SELECT10,
		ID_CARD_SELECT11,
		ID_CARD_SELECT12,
		ID_CARD_SELECT13,
		ID_CARD_SELECT14,
		ID_CARD_SELECT15,
		ID_CARD_SELECT16,
		ID_CARD_SELECT17,
		ID_CARD_SELECT18,
		ID_CARD_SELECT19,
		ID_CARD_SELECT20,
		ID_CARD_SELECT21,
		ID_CARD_SELECT22,
		ID_CARD_SELECT23,
		ID_CARD_SELECT24,
		ID_CARD_SELECT25,
		ID_CARD_SELECT26,
		ID_CARD_SELECT27,
		ID_CARD_SELECT28,
		ID_CARD_SELECT29,
		ID_CARD_SELECT30,

		ID_GAME_START = 201,
		ID_PREMIUM_GAME_START,
		ID_CONFIRM
	};

protected:
	typedef std::vector<ioUIRenderImage*> UIRenderImageList;

	ioUIRenderImage*	m_pLucky1Icon;
	ioUIRenderImage*	m_pLucky2Icon;
	UIRenderImageList	m_CardIconList;

	ioUIRenderImage*	m_pNormalTicket;
	ioUIRenderImage*	m_pPremiumTicket;
	ioUIRenderImage*	m_pMissionEnd;
	ioUIRenderImage*	m_pMissionCheck;
	ioUIRenderImage*	m_pMissionEnable;
	ioUIRenderImage*	m_pGaugeBarLeft;
	ioUIRenderImage*	m_pGaugeBarCenter;
	ioUIRenderImage*	m_pGaugeBarRight;
	ioUIRenderImage*	m_pRewardIcon[MAX_REWARD_ICON];
	ioUIRenderImage*	m_pMissionRewardIcon;
	ioUIRenderImage*	m_pButtonImage;
	ioUIRenderImage*	m_pComplete;
	ioUIRenderImage*	m_pCountDown[COUNT_DOWN_MAX];
	ioUIRenderImage*	m_pCountCircle;

	ioUIRenderImage*	m_pEffect;

	ioUIRenderFrame*	m_pMainTitle;
	ioUIRenderFrame*	m_pMissionTitle;
	ioUIRenderFrame*	m_pRewardTitle;
	ioUIRenderFrame*	m_pGameDisbled;

	ioHashString		m_szSoundMatchingSuccess;
	ioHashString		m_szSoundMatchingAllSuccess;
	ioHashString		m_szSoundGetReward;
	ioHashString		m_szSoundCountDown[COUNT_DOWN_MAX];

	MatchingGameCardButton*	m_pFirstButton;
	MatchingGameCardButton*	m_pSecondButton;

	ioHashStringVec		m_vGameEndEffect;

	DWORD	m_dwRewardEffectTime;
	DWORD	m_dwRewardEffectCheckTime;

	int		m_iCurEffectArray;

	ioComplexStringPrinter	m_Printer[PRINTER_MAX];

	ioHashString		m_Text[TEXT_MAX];

	DWORD	m_dwRemainedTime;
	DWORD	m_dwStartTime;

	bool	m_bIsPremium;
	bool	m_bCountDown;
	DWORD	m_dwCountDownTime;
	int		m_iCurCountArray;
	bool	m_bStartSend;

	bool	m_bPlayAllSuccessSound;
	bool	m_bPlayMissionComplete;
	UniqueObjID	m_uidAllSuccessSoundID;

public:
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void OnProcess( float fTimePerSec );

public:
	virtual void ParseExtraInfo( ioXMLElement &xElement );
	virtual void iwm_show();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual bool iwm_esc();

protected:
	virtual void OnRender();
	virtual void OnRenderAfterChild();

public:
	void	Reset();

	void	RewardEffect( float fTimePerSec );

	ioUIRenderImage*	GetCardImage( int iCardNum );

	MatchingGameCardButton*	GetButton( int iButton );

	void	SetButtonActivate( MatchingGameCardButton* pButton );
	void	ResetButtonActivate( bool bIsFirst = true );

	MatchingGameCardButton*	GetActivatedButton( bool bIsFirst = true );

	ioHashString	GetMatchingSuccessSound()	{	return	m_szSoundMatchingSuccess;	}
	ioHashString	GetMatchingAllSuccessSound()	{	return	m_szSoundMatchingAllSuccess;	}

public:
	MatchingGameWnd();
	virtual ~MatchingGameWnd();
};