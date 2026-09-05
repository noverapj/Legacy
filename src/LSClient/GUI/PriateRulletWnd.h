#pragma once

struct PriateRulletReward;

class PriateNewStartButton : public ioButton
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
	PriateNewStartButton();
	virtual ~PriateNewStartButton();
};

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class SwordFrameButton : public ioRadioButton
{
protected:
	ioUIRenderImage*	m_pChecked;
	ioUIRenderFrame*	m_pSelectFrame;

	int					m_Index;
	int					m_Count;

public:
	DWORD				m_dwSAlphaAnimationStart;
	float				m_fCurrAlphaRate;

public:
	void SetSwordInfo( int Index );

public:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );

public:
	virtual void iwm_show();
	virtual void iwm_hide();	
	virtual void OnProcess( float fTimePerSec );

public:
	void StartAlphaAnimation();
	void PorcessAlphaAnimation();

protected:
	virtual void OnDrawPushed( int iXPos, int iYPos );
	virtual void OnRender();

public:
	SwordFrameButton();
	virtual ~SwordFrameButton();
};

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class PrivateRulletRewardButton : public ioButton
{
protected:
	ioUIRenderImage*	m_pIcon;
	ioUIRenderImage*	m_pSubIcon;
	ioUIRenderImage*	m_pStamp;
	ioUIRenderImage*	m_pSecret;
	ioHashString		m_Name;

	int					m_CurrMax;
	int					m_CurrMin;
	int					m_CurrHP;
	bool				m_bReward;
	bool				m_bGray;
	bool				m_bHidden;

	DWORD				m_dwStampScaleAnimationStart;
	float				m_fScaleRate;
	float				m_fCurrScaleRate;
	DWORD				m_dwStampBlinkAnimationStart;
	int					m_nStampBlinkCount;
	DWORD				m_dwSAlphaAnimationStart;
	float				m_fCurrAlphaRate;

public:
	void SetRewardInfo( int Index, int eRewardType );

public:
	inline const ioHashString& GetName(){ return m_Name; }

public:
	virtual void iwm_show();
	virtual void iwm_hide();	
	virtual void OnProcess( float fTimePerSec );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	
public:
	bool IsEnableStampAnimation( int nHP );
	void StampAnimationStart( int nRewardCount );
	void ProcessStampAnimation();

	bool IsEnableStampBlinkAnimation();
	void StampBlinkAnimationStart();
	void ProcessBlinkAnimation();

	void StartAlphaAnimation();
	void PorcessAlphaAnimation();

protected:
	virtual void OnRender();

public:
	PrivateRulletRewardButton();
	virtual ~PrivateRulletRewardButton();
};

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class OakHoleButton : public ioButton
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

	ioUIRenderImage*	m_GoldClickSword;
	ioUIRenderImage*	m_GoldClickMoveSword;
	ioUIRenderImage*	m_GoldStickedSword;
		
	ioUIRenderImage*	m_SilverClickSword;
	ioUIRenderImage*	m_SilverClickMoveSword;
	ioUIRenderImage*	m_SilverStickedSword;
		
	ioUIRenderImage*	m_WoodClickSword;
	ioUIRenderImage*	m_WoodClickMoveSword;
	ioUIRenderImage*	m_WoodStickedSword;

	int					m_Index;
	bool				m_bSticked;
	DWORD				m_StickedSwordCode;
	AnimationType		m_AnimationType;
	DWORD				m_dwAnimationTime;
	int					m_eCritical;

	ioHashString		m_StickedWoodSound;
	ioHashString		m_StickedMetalSound;
	ioHashString		m_StickedCriticalWoodSound;
	ioHashString		m_StickedCriticalMetalSound;
	ioHashString		m_DamageSound;
	ioHashString		m_CriticalDamageSound;
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
	void AnimationStart( int eCritical );
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
	OakHoleButton();
	virtual ~OakHoleButton();
};

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class PriateRulletWnd : public ioWnd
{
public:
	enum
	{
		//RS_START STATE
		ID_EXIT					= 1,
		ID_GAME_RULE_TITLE		= 2,
		ID_GAME_RULE_RICH_LABEL	= 3,
		ID_START				= 4,

		//RS_PLAY STATE
		ID_REWARD_BUTTON01		= 11,
		ID_REWARD_BUTTON02		= 12,
		ID_REWARD_BUTTON03		= 13,
		ID_REWARD_BUTTON04		= 14,
		ID_REWARD_BUTTON05		= 15,
		ID_REWARD_BUTTON06		= 16,
		ID_REWARD_BUTTON07		= 17,
		ID_REWARD_BUTTON08		= 18,
		ID_REWARD_BUTTON09		= 19,
		ID_REWARD_BUTTON10		= 20,
		ID_BONUS_REWARD_BUTTON	= 21,
				
		ID_WOOD_SWORD_RADIO		= 22,
		ID_SILVER_SWORD_RADIO	= 23,
		ID_GOLD_SWORD_RADIO		= 24,
				
		ID_RESET_BUTTON			= 25,
		ID_HELP_BUTTON			= 26,
		ID_RETURN_PLAY_BUTTON	= 27,

		ID_ROTATE_LEFT			= 28,
		ID_ROTATE_RIGHT			= 29,

		ID_NEW_START			= 30,

		//드럼통 구멍 버튼
		ID_OAK_HOLE_BUTTON01	= 101,
		ID_OAK_HOLE_BUTTON02	= 102,
		ID_OAK_HOLE_BUTTON03	= 103,
		ID_OAK_HOLE_BUTTON04	= 104,
		ID_OAK_HOLE_BUTTON05	= 105,
		ID_OAK_HOLE_BUTTON06	= 106,
		ID_OAK_HOLE_BUTTON07	= 107,
		ID_OAK_HOLE_BUTTON08	= 108,
		ID_OAK_HOLE_BUTTON09	= 109,
		ID_OAK_HOLE_BUTTON10	= 110,
		ID_OAK_HOLE_BUTTON11	= 111,
		ID_OAK_HOLE_BUTTON12	= 112,
		ID_OAK_HOLE_BUTTON13	= 113,
		ID_OAK_HOLE_BUTTON14	= 114,
		ID_OAK_HOLE_BUTTON15	= 115,
		ID_OAK_HOLE_BUTTON16	= 116,
		ID_OAK_HOLE_BUTTON17	= 117,
		ID_OAK_HOLE_BUTTON18	= 118,
		ID_OAK_HOLE_BUTTON19	= 119,
		ID_OAK_HOLE_BUTTON20	= 120,

		ID_GLOBAL_TOOLTIP		= 500,
		ID_EFFECT_WND			= 501,
		ID_BONUS_REWARD_TOOLTIP	= 1000,
		ID_RIGHT_BUBBLE			= 2000,
		ID_LEFT_BUBBLE			= 2001,
	};

	enum RulletState
	{
		RS_START,
		RS_PRE,
		RS_PLAY,
		RS_HELP,
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
	bool				m_bDevelopKBack;
	DWORD				m_dwDevelopKBackTime;

	ioUIRenderImage*	m_OakImageCap;
	ioUIRenderImage*	m_OakImageBody;
	ioUIRenderImage*	m_DevelopK;
	ioUIRenderImage*	m_DevelopKBack;
	ioUIRenderImage*	m_DevelopKBackAnimation;
	ioUIRenderImage*	m_DevelopKBackToolTip;
	ioUIRenderImage*	m_RewardMap;
	ioUIRenderImage*	m_SwordBorderTitle;
	ioUIRenderFrame*	m_SwordBorder;
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

protected:
	DWORD				m_StickedSwordCode;

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

	int					m_CurrHP;
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

protected:
	ioHashString		m_ProvocationSound;
	ioHashString		m_RewardSound;
	ioHashString		m_DevelopKSound;

protected:
	ioUI3DEffectRender*	m_pSemiCriticalEffect;
	DWORD				m_dwSemiCriticalEffectStart;
	ioUI3DEffectRender*	m_pCriticalEffect;
	DWORD				m_dwCriticalEffectStart;

public:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );
	virtual void ParseExtraInfo( ioXMLElement &xElement );

public:
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

public:
	void SetStickType( DWORD dwID );
	void CheckSwordButton( DWORD dwID );
	void CheckOakHole();
	void OnSwordUse( int Index, bool bCritical, bool bProvoke, int nCurrHP, int nRewardCount, int eCritical );

public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void OnProcess( float fTimePerSec );

public:
	void InitDevelopKAnimation();

	void StartDevelopKShakeAnimation();
	void ProcessDevelopKShakeAnimation();

	void ProcessOakRotateAnimation();
	void ProcessDevelopKFaceCheck( bool bMustChange = false );
	void ProcessDevelopKProvokeFaceCheck();
	void ProcessDevelopKWoundedFaceCheck();
	
	void ProcessRulletStateCheck();
	void ProcessPreEndState();
	void ProcessEndState();

	void ProcessBackGround();

protected:
	virtual void OnRender();

public:	
	void StartRender( int iXPos, int iYPos );
	void PreRender( int iXPos, int iYPos );
	void PlayRender( int iXPos, int iYPos );
	void DevelopKRender( int iXPos, int iYPos );
	void EndPreRender( int iXPos, int iYPos );

public:
	PriateRulletWnd();
	virtual ~PriateRulletWnd();
};

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class PrivateRulletBonusRewardTooltip : public ioWnd
{
public:
	enum
	{
		REWARD_MAX	= 6,
	};

	struct RewardSet
	{
		ioUIRenderImage*	pImage;
		ioUIRenderImage*	pSubImage;
		ioHashString		szName;
		int					m_nMax;
		bool				m_bHidden;

		RewardSet()
		{
			pImage		= NULL;
			pSubImage = NULL;
			m_nMax		= 0;
			m_bHidden	= false;
		}
	};
	typedef std::vector<RewardSet> RewardSetList;

protected:
	ioUIRenderImage*	m_pIcon;
	ioUIRenderImage*	m_pSubIcon;
	ioUIRenderImage*	m_pIconBack;
	ioUIRenderImage*	m_pNoneIcon;
	ioUIRenderImage*	m_pSecret;
	ioHashString		m_RewardName;
	bool				m_bHidden;

protected:
	int					m_nStickedCount;
	RewardSetList		m_RewardSetList;

public:
	virtual void iwm_show();
	virtual void iwm_hide();	
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void OnProcess( float fTimePerSec );

public:
	void DestroyAllReward();

public:
	void ProcessPosition();
	void ProcessOnMainReward();
	void ProcessOnAllReward();

protected:
	virtual void OnRender();

public:
	PrivateRulletBonusRewardTooltip();
	virtual ~PrivateRulletBonusRewardTooltip();
};

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class DevelopKBubble : public ioWnd
{
protected:
	ioUIRenderImage*	m_pBubbleTail;

protected:
	int					m_CurrHP;
	int					m_CurrBubbleIndex;
	int					m_CurrBubbleTime;
	DWORD				m_StartBubbleTime;
	StringVector		m_CurrBubble;
	ioHashStringVec		m_CurrBubbleList;
	bool				m_bReverseTail;

public:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

public:
	void ProcessDevelopKBubble( bool bMustChange = false );
	void ProcessBubbleMessage();

public:
	virtual void ParseExtraInfo( ioXMLElement &xElement );
	virtual void iwm_show();
	virtual void iwm_hide();	
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	DevelopKBubble();
	virtual ~DevelopKBubble();
};

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class PrivateRulletResetPopup : public ioWnd
{
public:
	enum
	{
		ID_EXIT		= 1,
		ID_SEND		= 2,
		ID_CANCEL	= 3,
	};

protected:
	ioUIRenderImage *m_pIconBack;
	ioUIRenderImage *m_pIcon;

public:
	virtual bool iwm_spacebar();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void OnRender();

public:
	PrivateRulletResetPopup();
	virtual ~PrivateRulletResetPopup();
};