#pragma once

class ioPetView;

class DiceToolTip : public ioWnd
{
public:
	enum ToolTipType
	{
		TTT_REWARD,
		TTT_ALL_BINGO_REWARD,
		TTT_DUMMY,
	};

protected:
	enum
	{
		ID_ICON = 1002,
	};

protected:
	ioUIRenderImage *m_pIconBg;
	ioUIRenderImage *m_pIcon;

	ioHashString m_szTtitle;

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

public:
	virtual void iwm_show();	
	virtual void iwm_hide();

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

protected:
	void PrintManual( int iXPos, int iYPos, float fScale );
	void UpdateWndSize();

public:
	void SetInfo( ioHashString szName, ioUIRenderImage* pIcon );

public:
	DiceToolTip();
	virtual ~DiceToolTip();
};

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class DiceRewardBtn : public ioButton
{
public:
	enum
	{
		RENDER_OFFSET_X		= 25,
		RENDER_OFFSET_Y		= 25
	};

protected:
	ioHashString	m_szName;

	ioUIRenderImage*	m_pRewardIcon;

	bool	m_bIsVisited;

public:
	virtual void	OnRender();
	virtual void	iwm_mouseover( const ioMouse& mouse );
	virtual void	iwm_mouseleave( const ioMouse& mouse );

public:
	void	SetRewardInfo( char* szName, char* szIconName );
	void	ResetRewardInfo();

	inline	void	SetVisited( bool bVisited )		{	m_bIsVisited	= bVisited;	}
	inline	bool	IsVisited()				{	return	m_bIsVisited;	}
	inline	ioUIRenderImage*	GetIcon()	{	return	m_pRewardIcon;	}

public:
	DiceRewardBtn();
	virtual	~DiceRewardBtn();
};

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class DiceItemEffect : public ioWnd
{
protected:
	ioUIRenderImage*	m_pEffectImage;

	float	m_fEffectScale;

	DWORD	m_dwEffectStartTime;
	DWORD	m_dwEffectPlayTime;

	float	m_fEffectScaleMin;
	float	m_fEffectScaleMax;

public:
	virtual void	OnProcess( float fTimePerSec );

public:
	virtual void	ParseExtraInfo( ioXMLElement &xElement );
	virtual void	iwm_show();

	virtual void	OnRender();

public:
	void	SetImage( ioUIRenderImage* pImage );

public:
	DiceItemEffect();
	virtual	~DiceItemEffect();
};

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class DiceDevK : public ioWnd
{
protected:
	typedef std::vector<ioUIRenderImage*>	UIEffectList;
	UIEffectList		m_DevKEffectList;
	ioUIRenderImage*	m_pDevKEffectImage;

	DWORD	m_dwDevKEffectTime;
	DWORD	m_dwDevKEffectCheckTime;

	int		m_iCurEffectArray;

public:
	virtual void	ParseExtraInfo( ioXMLElement &xElement );
	virtual	void	OnRender();

public:
	void	DevKEffect( float fTimePerSec );
	void	ResetDevKEffect();

	void	Reset();

public:
	DiceDevK();
	virtual	~DiceDevK();
};

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class DiceDisabledBack : public ioWnd
{
protected:
	enum
	{
		ID_RESTART_GAME	= 1
	};

	enum
	{
		ANIMATION_TIME_IMAGE_TEXT_SIZE		= 300,
		ANIMATION_TIME_DESCRIPT_TEXT_ALPHA	= 350,
		ANIMATION_TIME_BACK_IMAGE_SIZE_1	= 100,
		ANIMATION_TIME_BACK_IMAGE_SIZE_2	= 300,
		ANIMATION_TIME_BACK_IMAGE_ALPHA		= 200,
		ANIMATION_TIME_ICON_IMAGE_SIZE		= 200,
		ANIMATION_TIME_BUTTON_ALPHA_1		= 300,
		ANIMATION_TIME_BUTTON_ALPHA_2		= 500
	};

	enum
	{
		PROCESS_IMAGE_TEXT_SIZE_1		= 200,
		PROCESS_IMAGE_TEXT_SIZE_2		= 100,
		PROCESS_DESCRIPT_TEXT_ALPHA_1	= 0,
		PROCESS_DESCRIPT_TEXT_ALPHA_2	= 100,
		PROCESS_BACK_IMAGE_SIZE_1		= 60,
		PROCESS_BACK_IMAGE_SIZE_2		= 100,
		PROCESS_BACK_IMAGE_SIZE_3		= 150,
		PROCESS_BACK_IMAGE_ALPHA_1		= 50,
		PROCESS_BACK_IMAGE_ALPHA_2		= 100,
		PROCESS_ICON_IMAGE_SIZE_1		= 150,
		PROCESS_ICON_IMAGE_SIZE_2		= 100,
		PROCESS_BUTTON_ALPHA_1			= 0,
		PROCESS_BUTTON_ALPHA_2			= 0,
		PROCESS_BUTTON_ALPHA_3			= 100
	};

	ioUIRenderFrame*	m_pGameDisbled;
	ioUIRenderImage*	m_pSnakeIcon;
	ioUIRenderImage*	m_pIconBack;
	ioUIRenderImage*	m_pGameEnd;

	DWORD	m_dwOpenedTime;

	float	m_fImageTextSize;
	int		m_iDescriptTextAlpha;
	float	m_fBackImageSize;
	int		m_iBackImageAlpha;
	float	m_fIconImageSize;
	int		m_iButtonAlpha;

	float	m_fSizeScaleFactor;
	int		m_iAlphaScaleFactor;

protected:
	void	Reset();

public:
	virtual void	AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );
	virtual	void	AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void	OnProcess( float fTimePerSec );

public:
	virtual void	iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void	iwm_show();

	virtual void	OnRender();

public:
	DiceDisabledBack();
	virtual	~DiceDisabledBack();
};

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class DiceGameWnd :	public ioMovingWnd
{
public:
	enum
	{
		DICE_GAME_DISTANCE	= 40
	};

	enum
	{
		LINE_DIRECTION_RIGHT	= 0,
		LINE_DIRECTION_LEFT		= 1,
		LINE_DIRECTION_UP		= 2
	};

	enum
	{
		LADDER_SNAKE_DIRECTION_NONE		= 0,
		LADDER_SNAKE_DIRECTION_RIGHT,
		LADDER_SNAKE_DIRECTION_LEFT,
		LADDER_SNAKE_DIRECTION_UP,
		LADDER_SNAKE_DIRECTION_DOWN
	};

	enum
	{
		ID_EXIT				= 1,
		ID_DICE_ROLL		= 2,
		ID_BOARD_CHANGE		= 3,
		ID_REWARD_CHANGE	= 4,

		ID_REWARD_SLOT1		= 101,
		ID_REWARD_SLOT87	= 187,

		ID_DEV_K			= 200,

		ID_ITEM_EFFECT		= 300,

		ID_TOOLTIP			= 1001,
		ID_DISABLED_BACK	= 2001
	};

	enum	DiceGameStatus
	{
		DICE_GAME_STATUS_COUNTDOWN	= 1,
		DICE_GAME_STATUS_ROLLING,
		DICE_GAME_STATUS_PLAYING,
		DICE_GAME_STATUS_IDLE,
		DICE_GAME_STATUS_DICE_ROLL,
		DICE_GAME_STATUS_ITEM_EFFECT,
		DICE_GAME_STATUS_RESULT
	};

	enum
	{
		DICE_GAME_ERROR_DICE	= 1,
		DICE_GAME_ERROR_REWARD,
		DICE_GAME_ERROR_BOARD
	};

protected:

	enum
	{
		TIME_RANDOM_REWARD_SHOW		= 500
	};

	enum
	{
		DICE_EFFECT_IDLE	= 0,
		DICE_EFFECT_1,
		DICE_EFFECT_2,
		DICE_EFFECT_3,
		DICE_EFFECT_4,
		DICE_EFFECT_5,
		DICE_EFFECT_6,
		DICE_EFFECT_MAX
	};

	enum
	{
		TEXT_CHANGE_REWARD	= 0,
		TEXT_CHANGE_BOARD,
		TEXT_ERROR_DICE,
		TEXT_ERROR_REWARD,
		TEXT_ERROR_BOARD,
		TEXT_MAX
	};

	struct LineDirection
	{
		int		m_iLineStart;
		int		m_iLineEnd;
		int		m_iDirection;
		int		m_iLineCount;
	};

	struct JumpPoint
	{
		int		iPositionX;
		int		iPositionY;

		JumpPoint()
		{
			iPositionX	= iPositionY	= 0;
		}
	};

	typedef std::vector<ioUIRenderImage*> UIRenderImageList;
	typedef std::vector<LineDirection> LineDirectionList;

	ioUIRenderImage*	m_pDiceCountNum;
	ioUIRenderImage*	m_pDiceCount;
	UIRenderImageList	m_BoardBackList;

	ioUI3DEffectRender*	m_pDiceEffect[DICE_EFFECT_MAX];

	ioUIRenderFrame*	m_pGameDisbled;

	ioHashString		m_szText[TEXT_MAX];

	DiceGameStatus	m_iStatus;

	D3DXVECTOR3 m_vCharPos;
	D3DXVECTOR3 m_vCameraPos;

	int		m_iStartPosition;
	int		m_iEndPosition;
	int		m_iCurrentPosition;
	int		m_iJumpPosition;
	DWORD	m_dwPlayerSpeed;
	DWORD	m_dwPlayerSnakeLadderSpeed;
	DWORD	m_dwRandomRewardShowTime;
	DWORD	m_dwRandomRewardStartTime;
	DWORD	m_dwRandomRewardPrevTime;
	DWORD	m_dwDiceRollingTime;
	DWORD	m_dwDiceRollingStartTime;
	int		m_iStartPosX;
	int		m_iStartPosY;
	int		m_iWidthTerm;
	int		m_iHeightTerm;
	float	m_fPlayerPieceXPos;
	float	m_fPlayerPieceYPos;
	LineDirectionList	m_LineDirectionList;

	float	m_fForceX;
	float	m_fForceY;

	DWORD	m_dwPrevMoveTime;
	DWORD	m_dwPrevJumpTime;

	int		m_iDiceEffectNumber;

	ioHashString	m_szRollingSound;
	ioHashString	m_szResultSound;
	ioHashString	m_szGetItemSound;

protected:
	bool	MoveToNextPosition( float fForceX, float fForceY );
	int		GetDirection( int iPosition );
	bool	MoveToSnakeLadderPosition( int iPosition, int iJumpPosition );
	JumpPoint	GetJumpPosition( SHORT sPosition );

	void	Reset();

	void	SetRandomReward();
	void	SetReward();

public:
	virtual	void	AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );
	virtual void	AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void	OnProcess( float fTimePerSec );

public:
	virtual void	ParseExtraInfo( ioXMLElement &xElement );
	virtual	void	iwm_create();
	virtual void	iwm_show();
	virtual	void	iwm_hide();
	virtual void	iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual bool	iwm_esc();
	virtual	void	iwm_destroy();

	virtual void	OnRender();
	virtual	void	OnRenderAfterChild();

public:
	void	SetRewardBtn( int iIndex, char* szName, char* szIconName );

	void	SetDiceRoll( int iDice, int iMidPosition, int iEndPosition );

	void	ChangeRewardList();
	void	ChangeBoard();

	void	SetRewardChangeItemCount();
	void	SetBoardChangeItemCount();

	void	SetErrorMsg( int iType );

	DiceGameStatus	GetStatus()		{	return	m_iStatus;	}

public:
	DiceGameWnd(void);
	virtual ~DiceGameWnd(void);
};

