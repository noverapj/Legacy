#pragma once

class RenewalPriateNewStartButton : public ioButton
{
protected:
	DWORD				m_dwBlinkAnimationStart;
	int					m_nBlinkCount;

public:
	void Clear();

public:
	virtual void iwm_show();
	virtual void iwm_hide();	
	virtual void OnProcess( float fTimePerSec );

public:
	void BlinkAnimationStart();
	void ProcessBlinkAnimation();

protected:
	virtual void OnRender();

public:
	RenewalPriateNewStartButton();
	virtual ~RenewalPriateNewStartButton();
};

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class RenewalPrivateRulletRewardButton : public ioButton
{
protected:
	enum { FAIL_EFFECT_MAX = 10, };
	enum { ROLLING_EFFECT_MAX = 9, REWARD_EFFECT_MAX = 10, };
protected:
	ioUIRenderImage*				m_pAcceptIcon;
	ioUIRenderImage*				m_pSubIcon;
	std::vector<ioUIRenderImage*>	m_vRollingIcon;
	std::vector<ioUIRenderImage*>	m_vSubIcon;

	ioUIRenderImage*				m_pSecret;
	ioHashString					m_Name;

	int								m_CurrMax;
	int								m_CurrMin;
	int								m_CurrHP;
	bool							m_bReward;
	bool							m_bGray;
	bool							m_bAccept;

	float							m_fScaleRate;
	float							m_fCurrScaleRate;
	DWORD							m_dwStampBlinkAnimationStart;
	int								m_nStampBlinkCount;
	DWORD							m_dwSAlphaAnimationStart;
	float							m_fCurrAlphaRate;
	
	ioUIRenderImage*				m_FailEffect[FAIL_EFFECT_MAX];
	DWORD							m_dwFailEffectStart;
	int								m_iFailIndex;

	ioUIRenderImage*				m_RollingEffect[ROLLING_EFFECT_MAX];
	DWORD							m_dwRollingEffectStart;
	int								m_iRollingEffectIndex;
	DWORD							m_dwRollingIconStart;
	int								m_iRollingIconIndex;

	ioUIRenderImage*				m_RewardEffect[REWARD_EFFECT_MAX];
	DWORD							m_dwRewardEffectStart;
	int								m_iRewardEffectIndex;

	int								m_iEffectXPos;
	int								m_iEffectYPos;

public:
	void SetRewardInfo( int Index );

public:
	inline const ioHashString& GetName(){ return m_Name; }
	inline const bool& IsAccept() { return m_bAccept; }
public:
	virtual void iwm_show();
	virtual void iwm_hide();	
	virtual void OnProcess( float fTimePerSec );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

	virtual void ParseExtraInfo( ioXMLElement &xElement );
public:
	bool IsEnableStampAnimation( int iValue );
	void StampAnimationStart( int nRewardCount );

	bool IsEnableStampBlinkAnimation();
	void StampBlinkAnimationStart();
	void ProcessBlinkAnimation();

	void StartAlphaAnimation();
	void PorcessAlphaAnimation();

	void FailAnimationStart();
	void ProcessFailEffectAnimation();

	void RollingAnimationStart( int iIndex );
	void ProcessRollingAnimation();

	void SuccessAnimationStart( int iIndex, int iRewardIndex );
	void ProcessSuccessEffectAnimation();

public:
	int GetEffectXPos();
	int GetEffectYPos();

protected:
	virtual void OnRender();

public:
	RenewalPrivateRulletRewardButton();
	virtual ~RenewalPrivateRulletRewardButton();
};

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class RenewalOakHoleButton : public ioButton
{
public:
	enum StickType
	{
		ST_NONE,
		ST_WOOD,
		ST_SILVER,
		ST_GOLD,
	};

	enum AnimationType
	{
		AT_NONE,
		AT_START,
		AT_MOVE,
	};

protected:
	ioUIRenderImage*	m_OverSword;

	ioUIRenderImage*	m_WoodClickSword;
	ioUIRenderImage*	m_WoodClickMoveSword;
	ioUIRenderImage*	m_WoodStickedSword;

	int					m_Index;
public:
	bool				m_bSticked;
protected:
	AnimationType		m_AnimationType;
	DWORD				m_dwAnimationTime;
	int					m_eCritical;

	ioHashString		m_StickedWoodSound;
	ioHashString		m_StickedMetalSound;
	ioHashString		m_DamageSound;
	ioHashString		m_EndDamageSound;

	DWORD				m_dwSAlphaAnimationStart;
	float				m_fCurrAlphaRate;

	int					m_nEffectXPos;
	int					m_nEffectYPos;

public:
	virtual void iwm_show();
	virtual void iwm_hide();	
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void ParseExtraInfo( ioXMLElement &xElement );

	virtual void OnProcess( float fTimePerSec );

public:
	void AnimationStart();
	void UpdateStick();

public:
	int GetEffectXPos();
	int GetEffectYPos();

public:
	void SetOakHole( int Index );

public:
	inline bool IsSticked(){ return m_bSticked; }

public:
	void StartAlphaAnimation();
	void PorcessAlphaAnimation();
	void ProcessStickAnimation();

protected:
	virtual void OnDrawPushed( int iXPos, int iYPos );
	virtual void OnDrawOvered( int iXPos, int iYPos );	
	virtual void OnRender();

	void OnRenderOver( int iXPos, int iYPos );
	void OnStickedRender( int iXPos, int iYPos );
	void OnRenderPushAnimation( int iXPos, int iYPos );

public:
	RenewalOakHoleButton();
	virtual ~RenewalOakHoleButton();
};

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class PriateRulletRenewalWnd : public ioWnd
{
public:
	enum
	{
		//RS_START STATE
		ID_EXIT						= 1,
		ID_GAME_RULE_TITLE			= 2,
		ID_GAME_RULE_SUB_TITLE		= 3,
		ID_GAME_RULE_RICH_LABEL1	= 4,
		ID_GAME_RULE_RICH_LABEL2	= 5,
		ID_GAME_RULE_RICH_LABEL3	= 6,

		ID_GAME_RULE_RICH_LABEL4	= 7,
		ID_GAME_RULE_RICH_LABEL5	= 8,
		ID_GAME_RULE_RICH_LABEL6	= 9,
		
		ID_START					= 10,

		//RS_PLAY STATE
		ID_REWARD_BUTTON01			= 11,
		ID_REWARD_BUTTON02			= 12,
		ID_REWARD_BUTTON03			= 13,
		ID_REWARD_BUTTON04			= 14,
		ID_REWARD_BUTTON05			= 15,
		ID_REWARD_BUTTON06			= 16,
		ID_REWARD_BUTTON07			= 17,
		ID_REWARD_BUTTON08			= 18,
		ID_REWARD_BUTTON09			= 19,
		ID_REWARD_BUTTON10			= 20,
		ID_REWARD_BUTTON11			= 21,

		ID_HELP_BUTTON				= 41,
		ID_RETURN_PLAY_BUTTON		= 42,
		ID_HELP_PRE_BTN				= 43,
		ID_HELP_NEXT_BTN			= 44,

		ID_NEW_START				= 60,
		ID_ACCEPT_REWARD			= 61,
		ID_CHECKING					= 62,

		//드럼통 구멍 버튼
		ID_OAK_HOLE_BUTTON01		= 101,
		ID_OAK_HOLE_BUTTON02		= 102,
		ID_OAK_HOLE_BUTTON03		= 103,
		ID_OAK_HOLE_BUTTON04		= 104,
		ID_OAK_HOLE_BUTTON05		= 105,
		ID_OAK_HOLE_BUTTON06		= 106,
		ID_OAK_HOLE_BUTTON07		= 107,
		ID_OAK_HOLE_BUTTON08		= 108,
		ID_OAK_HOLE_BUTTON09		= 109,
		ID_OAK_HOLE_BUTTON10		= 110,
		ID_OAK_HOLE_BUTTON11		= 111,
		ID_OAK_HOLE_BUTTON12		= 112,

		ID_GLOBAL_TOOLTIP			= 500,
		ID_EFFECT_WND				= 501,
		ID_RIGHT_BUBBLE				= 2000,
	};

	enum RulletState
	{
		RS_START,
		RS_PRE,
		RS_PLAY,
		RS_HELP,
		RS_ROLLING,
		RS_ROLLING_END,
		RS_FAIL,
		RS_PRE_END,
		RS_END,
	};

	enum DevelopKState
	{
		DK_NONE,
		DK_LEFT,
		DK_RIGHT,
		DK_KNOCKOUT,
	};

	enum FaceState
	{
		FS_NORMAL,
		FS_PROVOKE,
		FS_WOUNDED,
	};

	enum
	{
		UP_MAX		= 2,
		SHAKE_MAX	= 1,
		EFFECT_MAX	= 4,
	};

	typedef std::vector<ioUIRenderImage*> UIRenderImageList;

protected:
	RulletState			m_RulletState;

	ioUIRenderImage*	m_OakImageCap;
	ioUIRenderImage*	m_OakImageBody;
	ioUIRenderImage*	m_DevelopK;
	ioUIRenderImage*	m_SwordBar;
	ioUIRenderImage*	m_RewardMap;
	ioUIRenderImage*	m_FailMap;
	ioUIRenderFrame*	m_BubbleRight;
	ioUIRenderFrame*	m_BubbleLeft;
	ioUIRenderImage*	m_DevelopKFlyStart;
	ioUIRenderImage*	m_DevelopKFlyEnd;
	ioUIRenderImage*	m_CurrDevelopKFace;
	ioUIRenderImage*	m_DevelopKWounded;
	ioUIRenderImage*	m_DevelopKWoundedBack;
	ioUIRenderImage*	m_DevelopKProvoke;
	ioUIRenderImage*	m_DevelopKProvokeBack;
	ioUIRenderImage*	m_MonKeyFace;
	ioUIRenderImage*	m_ShipTreeLight;
	ioUIRenderImage*	m_ShipTreeInvLight;
	ioUIRenderImage*	m_BombLeftFire;
	ioUIRenderImage*	m_BombLeftInvFire;
	ioUIRenderImage*	m_BombRightFire;
	ioUIRenderImage*	m_BombRightInvFire;

	ioUIRenderImage*	m_FireWorkEffect[EFFECT_MAX];
	ioUIRenderImage*	m_WoodSword;
	ioUIRenderImage*	m_ClearStamp;

protected:
	DWORD				m_StickedSwordCode;
	int					m_iStickedSwordCount;

protected:
	DevelopKState		m_DevelopKState;

	//등장 애니메이션
	DWORD				m_dwPreStartTime;

	//Shake 애니메이션
	DWORD				m_dwDevelopAnimationStartTime;
	DWORD				m_dwDevelopAnimationTime;
	int					m_nAnimationX;
	bool				m_bDevelopKShake;

	//표정 애니메이션
	FaceState			m_FaceState;

	int					m_CurrState;
	UIRenderImageList	m_CurrFaceList;
	int					m_CurrFaceIndex;

	DWORD				m_dwFaceAnimationStartTime;
	DWORD				m_dwFaceAnimationTime;
	int					m_nFaceAnimationIndex;

	//종료 준비 애니메이션
	DWORD				m_dwPreEndCheckStartTime;
	DWORD				m_dwPreEndStartTime;

	//종료 애니메이션
	DWORD				m_dwEndStartTime;

protected:
	DWORD				m_MonkeyFaceTime;
	DWORD				m_TreeBlinkTime;
	float				m_fCurrTreeAlphaRate;
	DWORD				m_LeftBombBlinkTime;
	float				m_fLeftCurrBombAlphaRate;
	DWORD				m_RightBombBlinkTime;
	float				m_fRightCurrBombAlphaRate;
	int					m_nSwordUseCount;

	float				m_fClearStampScaleSpeed;
	float				m_fCurClearStampScale;
	DWORD				m_dwClearStampLoopTime;
	DWORD				m_dwClearStampStartTime;

protected:
	ioHashString		m_ProvocationSound;
	ioHashString		m_RewardSound;
	ioHashString		m_DevelopKSound;
	ioHashString		m_FailSound;
	ioHashString		m_RollingSound;
	ioHashString		m_ClearStampSound;

protected:
	ioUI3DEffectRender*	m_pCriticalEffect;
	DWORD				m_dwCriticalEffectStart;

	ioUI3DEffectRender*	m_pEnableRewardEffect;
	DWORD				m_dwEnableRewardEffectStart;

	DWORD				m_dwRollingEndTime;

	std::vector<int>	m_vRewardList;

protected:
	DWORD				m_dwFailStartTime;
	ioUIRenderImage*	m_pFailAcceptIcon;
	ioUIRenderImage*	m_pSubIcon;
	ioHashString		m_szFailAcceptIconName;

	int					m_iCurPage;
	int					m_iMaxPage;

public:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );
	virtual void ParseExtraInfo( ioXMLElement &xElement );

private:
	void SetState( RulletState eRulletState, bool bAnimation = true );

	void ShowStartState();
	void HideStartState();

	void ShowPreState();

	void ShowPlayState();
	void HidePlayState();
	void HidePlayOakPart();

	void ShowHelpState();
	void HideHelpState();

	void ShowPreEndState( bool bAnimation );
	void HidePreEndState();

	void ShowEndState( bool bAnimation );
	void HideEndState();

	void ShowRollingState();
	void HideRollingState();

	void ShowRollingEndState();
	void HideRollingEndState();

	void ShowRollingEnd();

	void ShowFailState( bool bAnimation );
	void HideFailState();

	void SetToolTipText( DWORD dwID );

	void UpdateHelpInfo();

public:
	void StartRollingEndTime( const IntVec& vRewardList );

public:
	void CheckOakHole();
	void OnSuccessSwordUse( int iIndex, int iRewardCount );
	void OnFailSwordUse( int iIndex, int iRewardIndex, int iRewardCount );
	void SetRollingEnd( int Index, int nRewardCount, bool bSuccess );

public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void OnProcess( float fTimePerSec );

public:
	void InitDevelopKAnimation();

	void StartDevelopKShakeAnimation();
	void ProcessDevelopKShakeAnimation();

	void ProcessDevelopKFaceCheck( bool bMustChange = false );
	void ProcessDevelopKProvokeFaceCheck();
	void ProcessDevelopKWoundedFaceCheck();

	void ProcessPreEndState();
	void ProcessEndState();

	void ProcessBackGround();

	void ProcessRollingState();

	void ProcessRollingEndState( float fTimePerSec );

protected:
	void UpdateSwordCount();
	void ProcessFailIconTooltip();

protected:
	virtual void OnRender();

public:	
	void StartRender( int iXPos, int iYPos );
	void PreRender( int iXPos, int iYPos );
	void PlayRender( int iXPos, int iYPos );
	void DevelopKRender( int iXPos, int iYPos );
	void EndPreRender( int iXPos, int iYPos );
	void RollingRender( int iXPos, int iYPos );
	void RollingEndRender( int iXPos, int iYPos );
	void FailRender( int iXPos, int iYPos );

public:
	PriateRulletRenewalWnd();
	virtual ~PriateRulletRenewalWnd();
};

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class RenewalDevelopKBubble : public ioWnd
{
protected:
	ioUIRenderImage*	m_pBubbleTail;

protected:
	int					m_CurrState;
	int					m_CurrBubbleIndex;
	int					m_CurrBubbleTime;
	DWORD				m_StartBubbleTime;
	StringVector		m_CurrBubble;
	ioHashStringVec		m_CurrBubbleList;
	bool				m_bReverseTail;

public:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

public:
	void ProcessRenewalDevelopKBubble( bool bMustChange = false );
	void ProcessBubbleMessage();

public:
	virtual void ParseExtraInfo( ioXMLElement &xElement );
	virtual void iwm_show();
	virtual void iwm_hide();	
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	RenewalDevelopKBubble();
	virtual ~RenewalDevelopKBubble();
};

///////////////////////////////////////////////
#define  TEXT_TOOLTIP_LINE 10
class PriateTextTooltip : public ioWnd
{
public:
	enum
	{
		MIN_WIDTH		= 0,
		MIN_HEIGHT		= 26,
		TEXT_LINE_GAP   = 18,
		SIDE_TEXT_GAP	= 14,

	};

protected:
	ioComplexStringPrinter m_Printer[TEXT_TOOLTIP_LINE];

protected:
	void ProcessPosition();
	void ClearList();

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	void SetTooltipText( ioComplexStringPrinter *rkPrinter );

public:
	PriateTextTooltip();
	virtual ~PriateTextTooltip();
};