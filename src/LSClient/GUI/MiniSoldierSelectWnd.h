#pragma once

///////////////////////////////////////////////////////////////////////////
class MiniSoldierBtn : public ioButton
{	
protected:
	int	m_iSoldierNumber;
	int	m_iSoldierSlot;
	int m_nGradeType;

	ioUIRenderImage* m_pEmptySlot;
	ioUIRenderImage* m_pCharIcon;
	ioUIRenderImage* m_pCharSubIcon;
	ioUIRenderImage* m_pCharIconBack;
	ioUIRenderFrame* m_pKeyBack;

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );

	virtual void OnRender();

public:
	void SetCharacterCount( int iNumber, int iSlot );

public:
	MiniSoldierBtn();
	virtual ~MiniSoldierBtn();
};
///////////////////////////////////////////////////////////////////////////
class CharChangeGaugeHelp : public ioButton
{
protected:
	ioUIRenderImage*	m_HelpIcon;

public:
	virtual	void	AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

protected:
	virtual	void	OnRender();

public:
	CharChangeGaugeHelp(void);
	virtual ~CharChangeGaugeHelp(void);
};
///////////////////////////////////////////////////////////////////////////
class CharChangeGauge : public ioMovingWnd
{
protected:
	enum
	{
		GAUGE_SCALE_TIME	= 450
	};

protected:
	ioUIRenderImage*	m_pGaugeBack;
	ioUIRenderImage*	m_pGauge;
	ioUIRenderImage*	m_pGaugeBig;

protected:
	DWORD	m_dwPassedTime;
	float	m_fGaugeRate;

	CharState	m_eOwnerState;

	DWORD	m_dwScaleStartTime;
	float	m_fScaleRate;

public:
	virtual	void	AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

protected:
	virtual	void	OnRender();
	virtual	void	OnRenderAfterChild();
	virtual	void	OnProcess( float fTimePerSec );

public:
	void	UpdateByOwner( ioBaseChar *pOwner, bool bObserver );

protected:
	bool	SkipRender();

public:
	CharChangeGauge(void);
	virtual ~CharChangeGauge(void);
};
///////////////////////////////////////////////////////////////////////////
class MiniSoldierSelectWnd : public ioMovingWnd
{
public:
	enum SlotInfo
	{
		ID_SOLDIER_BTN1	= 0,
		ID_SOLDIER_BTN2 = 1,
		ID_SOLDIER_BTN3 = 2,
		ID_SOLDIER_BTN4 = 3,
		ID_SOLDIER_BTN5 = 4,
		ID_SOLDIER_BTN6 = 5,
		ID_SOLDIER_MAX	= 6,

		ID_BUTTON_PAGE_UP		= 7,
		ID_BUTTON_PAGE_DN		= 8,
		ID_BUTTON_MAX			= 9,
	};

	enum CharChangeGaugeInfo
	{
		ID_CHAR_CHANGE_GAUGE	= 100,
		ID_CHAR_CHANGE_HELP		= 101,
		ID_CHAR_CHANGE_TOOLTIP	= 102
	};

	enum
	{
		ID_BTNUP_DONT_SETTING = 999999999,
	};

	enum ShowAniType 
	{
		SAT_NONE         = 0,
		SAT_START_ROUND  = 1,
		SAT_DELAY        = 2,
		SAT_UP           = 3,  // ¡Æ¢®¨ùO
		SAT_DOWN         = 4,  // ¡Æ¡§¨ùO
		SAT_UP_STOP      = 5,  // ¡Æ¢®¨ùO
		SAT_STOP         = 6,
		SAT_END          = 7,
	};

protected:
	enum 
	{
		POS_SAVE_VERSION = 4,
	};

	CharState	m_eOwnerState;
	ioPlayStage *m_pPlayStage;

	int m_iDefaultX;
	int m_iDefaultY;

	int m_iCurPage;
	bool m_bChangeResolution;

	bool m_bSetMouseOver;

	FSMStateMgr m_ShowAniState;
	ModeType m_eModeType;

public:
	virtual void iwm_create();
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_mouseover( const ioMouse& mouse );
	virtual void iwm_mouseleave( const ioMouse& mouse );
	virtual void iwm_lbuttonup( const ioMouse& mouse );
	virtual void iwm_wheel( int zDelta );
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	void  SendCharChange(int iArray );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();
	void CheckApplyToolTip();

public:
	void UpdateByOwner( ioBaseChar *pOwner );
	void ChangeSoldierSlot( int iLeftArray, int iRightArray );
	void AddMySoldier( DWORD dwIndex );
	void SettingMiniSoldierBtn();
	void ReSetWindow();
	virtual void SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate = false );

	int GetCurState() const;
	void ChangeShowAniState( ShowAniType eShowAniType );
	void SetCurModeType( ModeType eModeType );
	void InitShowAniState();

	bool IsCharacterChangeImmediately();

protected:
	void UpdateDefaultPos();
	void ChangeWnd();

public:
	bool SkipRender( bool bApplyOption = true );
	void SetPlayStage(ioPlayStage * pStage);

public:
	MiniSoldierSelectWnd();
	virtual ~MiniSoldierSelectWnd();
};