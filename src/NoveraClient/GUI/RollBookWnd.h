#pragma once

class CRollBookRewardInfo
{
protected:
	enum
	{
		MAX_ROW				 = 7,
		MAX_COLUMN			 = 4,
		ICON_ROW_X			 = 69,
		ICON_COLUMN_Y        = 97,
		ICON_ROW_X_OFFSET	 = 82,
		ICON_COLUMN_Y_OFFSET = 77,
	};

	//Data
	int					m_iPresentType;	//PresentType
	int					m_iValue1;		//PresentValue1
	int					m_iValue2;		//PresentValue2
	bool				m_bHidden;
	bool				m_bReceive;
	bool				m_bEmphasize;

	//UI
	int					m_iIconPosX;
	int					m_iIconPosY;
	ioUIRenderImage*	m_pPresentIcon;
	ioUIRenderImage*	m_pSubIcon;
	ioHashString		m_szPresentName;

public:
	bool					Init( ioINILoader& rkLoader, const int& iRewardNum, const int& iLastDay );
	bool					IsEmpty() const;
protected:
	ioUIRenderImage*		GetRewardIconByValues( const int& iPresentType, const int& iPresentValue1, const int& iPresentValue2 );
	ioUIRenderImage*		GetRewardSubIconByValues( const int& iPresentType, const int& iPresentValue1, const int& iPresentValue2 );

public://GetFunc
	const int&				GetPresentType()	const { return m_iPresentType; }
	const int&				GetPresentValue1()	const { return m_iValue1; }
	const int&				GetPresentValue2()	const { return m_iValue2; }
	const int&				GetRewardIconPosX() const { return m_iIconPosX; }
	const int&				GetRewardIconPosY() const { return m_iIconPosY; }
	ioUIRenderImage*		GetPresentIcon()	const { return m_pPresentIcon; }
	ioUIRenderImage*		GetPresentSubIcon()	const { return m_pSubIcon; }
	const bool&				GetReceive()		const { return m_bReceive; }
	const bool&				GetHidden()			const { return m_bHidden; }
	const bool&				GetEmphasize()		const { return m_bEmphasize; }
	const ioHashString&		GetRewardName()		const { return m_szPresentName; }
public:
	CRollBookRewardInfo();
	~CRollBookRewardInfo();

};
typedef std::vector<CRollBookRewardInfo*> RollBookRewardVec;
///////////////////////////////////////////////////////////////////////

class RollBookWnd :	public ioWnd
{
public:
	enum RollBookType
	{
		RBT_NONE		= 0,
		RBT_NEWBIE		= 1,	//신규 유저
		RBT_RETURN		= 2,	//복귀 유저
		RBT_DEFAULT		= 3,	//기존 유저
	};

protected:
	enum
	{
		ID_CLOSE_BTN	= 1,
		ID_TOOL_TIP		= 10,
	};
	enum
	{
		MAX_REWARD_NUM	= 1,
		MAX_SUB_TITLE	= 3,
		MAX_DAY			= 28,
	};

	enum
	{
		ICON_SIZE_X		= 50,
		ICON_SIZE_Y		= 55,
	};

protected:
	//CTPK_ROLLBOOK_RENEWAL Request Time
	static CTime			m_RequestDateTime;
	static ModeType			m_CurModeType;
	static bool				m_bBattleRoomCheck;
	static bool				m_bLadderRoomCheck;

	//Rollbook Table Index and Cur Position
	static int				m_TableIndex;
	static int				m_TablePosition;

	static bool				m_bDirectlyShow;
public:
	static const bool&	IsDirectShow()	{ return m_bDirectlyShow; }
protected:
	//Data
	RollBookRewardVec		m_CurRollBookRewardList;
	ioPlayStage*			m_pPlayStage;
	int						m_iOverTooltipIndex;		//오버시 보상 출력
	int						m_iTodayRewardIndex;		//당일보상 Effect 관련
	bool					m_bRequestTime;				//서버에 요청 가능한 시간인지 체크
	DWORD					m_dwProcessCheckTime;
	DWORD					m_dwProcessCheckTic;

	//UI
	int						m_iTableType;				//RollBookType 신규,복귀,기존
	ioComplexStringPrinter	m_Title;
	ioComplexStringPrinter	m_SubText[MAX_SUB_TITLE];
	ioUIRenderImage*		m_pRewardIconBG;
	ioUIRenderImage*		m_pGainIcon;
	ioUIRenderImage*		m_pEmphasizeEffect;
	ioUIRenderImage*		m_pHidden;
	ioUIRenderImage*		m_pCostumeMark;
	ioHashString			m_szRewardSnd;
	

	// m_iTableType 마다 Frame이 다르다.
	ioUIRenderFrame*		m_pCurFrm;
	ioUIRenderFrame*		m_pNormalUserTitleFrm;
	ioUIRenderFrame*		m_pReturnUserTitleFrm;
	ioUIRenderFrame*		m_pNewUserTitleFrm;

	//Today Reward Effect
	float					m_fTodayRewardStartScale;
	float					m_fCurTodayRewardScale;
	float					m_fTodayRewardScaleSpeed;

	//Emphasize Effect
	DWORD					m_dwEmphasizeEffectStartTime;
	DWORD					m_dwEmphasizeEffectDuration;
	float					m_fCurEmphasizeEffectAlpha;
	float					m_fEmphasizeEffectAlphaSpeed;
	float					m_fCurEmphasizeEffectScale;
	float					m_fEmphasizeEffectScaleSpeed;
protected:
	void				ClearList();
	//LoadINI
	void				LoadRenewalTimeToINI( DWORD dwRecieveTime, ioINILoader& rkLoader );
	void				LoadTableDataToINI( ioINILoader& rkLoader, const int& iCurTableIndex, const int& iLastTablePosision );
	void				LoadReward( ioINILoader& rkLoader, const int& iLastTablePosision );
	void				LoadTableData( ioINILoader& rkLoader );
	void				LoadTitleByType( ioINILoader& rkLoader, const int& iType );
	void				LoadSubTitle( ioINILoader& rkLoader );

	//Process
	void				OnToolTipProcess();
	void				OnEmphasizeEffectProcess( float fTimePerSec );
	void				OnTodayRewardEffectProcess( float fTimePerSec );
	void				OnRequestProcess();

	//Render
	void				OnRewardInfoRender( int iXPos, int iYPos );
public:
	void				SetPlayStage( ioPlayStage *pPlayStage ) { m_pPlayStage = pPlayStage; }
	void				SettingRollBookByPacket( DWORD dwRecieveTime, int iState, int iTableIndex, int iCurPosition );
	void				ShowRollBookWnd();
	inline const bool&	IsRequestTime() { return m_bRequestTime; }
public:
	virtual void		AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void		AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );
	virtual void		ParseExtraInfo( ioXMLElement &xElement );
	virtual void		iwm_show();
	virtual void		iwm_hide();
	virtual void		iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual bool		iwm_spacebar();

protected:
	virtual void	OnProcess( float fTimePerSec );
	virtual void	OnRender();

public:
	RollBookWnd(void);
	virtual ~RollBookWnd(void);
};

/////////////////////////////////////////////////////////////////////////////////////////////////////
class RollBookCloseBtn: public ioButton
{
protected:
	ioUIRenderFrame*	m_pCurBackFrm;
	ioUIRenderFrame*	m_pNormalUserBackFrm;
	ioUIRenderFrame*	m_pReturnUserBackFrm;	//복귀유저
	ioUIRenderFrame*	m_pNewUserBackFrm;		//신규유저

	ioUIRenderFrame*	m_pCurDownFrm;
	ioUIRenderFrame*	m_pNormalUserDownFrm;
	ioUIRenderFrame*	m_pReturnUserDownFrm;	//복귀유저
	ioUIRenderFrame*	m_pNewUserDownFrm;		//신규유저

public:
	void		 SetType( const int& iType );

protected:
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );
	virtual void OnDrawNormal( int iXPos, int iYPos );
	virtual void OnDrawOvered( int iXPos, int iYPos );
	virtual void OnDrawPushed( int iXPos, int iYPos );

public:
	RollBookCloseBtn();
	virtual ~RollBookCloseBtn();
};

/////////////////////////////////////////////////////////////////////////////////////////////////////
class RollBookToolTip : public ioWnd
{
protected:
	enum
	{
		MIN_WIDTH		= 0,
		MIN_HEIGHT		= 26,
		TEXT_LINE_GAP   = 18,
		SIDE_TEXT_GAP	= 14,
	};
	ioComplexStringPrinter m_szPresentName;
	ioHashString		   m_szRewardInfoOverSnd;
public:
	void		 SetInfo( const CRollBookRewardInfo* const ConstRewardInfo );
public:
	virtual void ParseExtraInfo( ioXMLElement &xElement );
protected:
	virtual void OnRender();
	virtual void OnProcess( float fTimePerSec );
public:
	RollBookToolTip();
	virtual ~RollBookToolTip();
};
///////////////////////////////////////////////////////////////////