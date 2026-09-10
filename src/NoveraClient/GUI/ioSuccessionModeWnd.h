#pragma once
#include "ioWnd.h"
#include "../CharacterInfo.h"

class SuccessionBattleWnd:	public ioWnd
{
protected:
	enum
	{
		ID_EXIT		= 1,
		ID_START	= 2,
		ID_CANCEL	= 3,
	};

	enum
	{
		MAX_DESC	= 4,
	};

	enum
	{
		STATE_NONE,
		STATE_MATCHING,
	};

protected:
	ioComplexStringPrinter	m_szDescArray[MAX_DESC];
	ioUIRenderImage*		m_pIconBack;
	ioUIRenderImage*		m_pIcon;

	ioUIRenderImage*		m_pMaxWinNum;
	ioUIRenderImage*		m_pCurWinNum;

	//Wait
protected:
	bool					m_bWaitShow;
	DWORD					m_dwMatchingSearchTime;
	ioHashString			m_szMatchingSearchMent;
	ioHashString			m_szMatchingEventMent;

private:
	int m_iMatchState;
	DWORD m_dwMatchingCancelTime;
	DWORD m_dwMatchingCancelCheckTime;
	
public:
	virtual void iwm_create();
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void ParseExtraInfo( ioXMLElement &xElement );
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual bool iwm_spacebar();
	virtual bool iwm_esc();
	
protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();
	virtual void OnRenderAfterChild();

public:
	void MatchingSuccess();
	void MatchingCancel();
	void MatchingFail();

protected:
	void AddModal();
	void RemoveModal();
	
public:
	SuccessionBattleWnd(void);
	virtual ~SuccessionBattleWnd(void);
};

//--------------------------------------------------------------------------------------------------
class SuccessionBattleRequestWnd:	public ioWnd
{
protected:
	enum
	{
		ID_EXIT		= 1,
		ID_REQUEST	= 2,
		ID_WAIT		= 3,
		ID_ROBBY	= 4,
	};
	enum
	{
		MAX_DESC	= 3,
	};

protected:
	ioComplexStringPrinter	m_szDescArray[MAX_DESC];
	ioUIRenderImage*		m_pIconBack;
	ioUIRenderImage*		m_pIcon;
	int						m_iFullWaitTime;
	float					m_fCurWaitTime;
	
protected:
	bool					m_bWaitShow;
	DWORD					m_dwReplyWaitTime;
	ioHashString			m_szReplyWaitMent;
public:
	virtual void iwm_create();
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void ParseExtraInfo( ioXMLElement &xElement );
	virtual void iwm_show();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual bool iwm_spacebar();
protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();
public:
	SuccessionBattleRequestWnd(void);
	virtual ~SuccessionBattleRequestWnd(void);
};

//--------------------------------------------------------------------------------------------------
class SuccessionBattleReplyWnd:	public ioWnd
{
protected:
	enum
	{
		ID_EXIT		= 1,
		ID_REPLY	= 2,
		ID_REJECT	= 3,
	};
	enum
	{
		MAX_DESC	= 3,
	};

protected:
	ioComplexStringPrinter	m_szDescArray[MAX_DESC];
	ioUIRenderImage*		m_pIconBack;
	ioUIRenderImage*		m_pIcon;
	int						m_iFullWaitTime;
	float					m_fCurWaitTime;
public:
	virtual void iwm_create();
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void ParseExtraInfo( ioXMLElement &xElement );
	virtual void iwm_show();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual bool iwm_spacebar();
protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	SuccessionBattleReplyWnd(void);
	virtual ~SuccessionBattleReplyWnd(void);
};

//--------------------------------------------------------------------------------------------------
class SuccessionMatchVersusSceneWnd : public ioWnd
{
protected:
	struct HeroData
	{
		ioHashString	m_szUserID;
		int				m_iMMRPoint;	//개발자 전용
		int				m_iMaxSuccessionWin;
		int				m_iCurSuccessionWin;
		CHARACTER		m_CharInfo;
		ITEM_DATA		m_EquipItem[MAX_INVENTORY];

		
		// ANI
		DWORD			m_dwAniState;
		DWORD			m_dwMoveTime;

		float			m_fAniRate;
		int				m_iStartXPos;
		int				m_iCurrentXPos;
		int				m_iCurrentYPos;
		HeroData()
		{
			Init();
		}

		void Init()
		{
			m_szUserID.Clear();
			m_CharInfo.Init();
			m_dwAniState = CARD_ANI_START;
			m_iMaxSuccessionWin = m_iCurSuccessionWin = m_iMMRPoint = 0;
			m_dwMoveTime = 0;
			m_fAniRate   = 0.0f;
			m_iStartXPos = m_iCurrentXPos = m_iCurrentYPos = 0;
			for(int i = 0;i < MAX_INVENTORY;i++)
				m_EquipItem[i].Initialize();
		}
	};
public:
	enum
	{
		MAX_EFFECT_SPR = 10,

		VIEW_WIDTH = 141,
		VIEW_HEIGHT= 240,
	};

protected:
	static HeroData m_BlueData;
	static HeroData m_RedData;

	ioCreateChar*	m_pBlueChar;
	ioCreateChar*	m_pRedChar;
	ioUI3DRender *m_pBlue3DRender;
	ioUI3DRender *m_pRed3DRender;
protected:
	ioUIRenderFrame *m_pBlueFrame;
	ioUIRenderFrame *m_pRedFrame;
	ioUIRenderFrame *m_pBackFrame;

	ioUIRenderImage *m_pBlueGra;
	ioUIRenderImage *m_pRedGra;
	ioUIRenderImage *m_pDarkBar;
	ioUIRenderImage *m_pLightBar;
	ioUIRenderImage *m_pBlackStrip;
	ioUIRenderImage *m_pWhiteStrip;
	ioUIRenderImage *m_pSuccessionBattleMark;
	ioUIRenderImage *m_pCharShadow;
	ioUIRenderImage *m_pEffectSprite[MAX_EFFECT_SPR];

protected:
	enum
	{
		CARD_ANI_START = 0,
		CARD_ANI_BACK  = 1,
		CARD_ANI_DELAY = 2,
		CARD_ANI_FADEOUT= 3,

		START_MOVE_TIME= 230,
		BACK_MOVE_TIME = 500,
		DELAY_TIME     = 500,
		FADEOUT_MOVE_TIME = 200,
	};

	

	//////////////////////////////////////////////////////////////////////////
	enum
	{
		VS_ANI_NONE   = 0,
		VS_ANI_START  = 1,		
		VS_ANI_FADEOUT= 2,

		VS_START_TIME = 500,		
		VS_FADEOUT_TIME = 100,
	};

	struct VersusAni
	{
		DWORD m_dwAniState;
		DWORD m_dwAniTime;
		float m_fAniRate;

		// effect
		DWORD m_dwEffectTime;
		int   m_iEffectArray;
		VersusAni()
		{
			Init();
		}

		void Init()
		{
			m_dwAniState = VS_ANI_NONE;
			m_dwAniTime  = 0;
			m_fAniRate   = 0.0f;

			m_dwEffectTime = 0;
			m_iEffectArray = 0;
		}
	};
	VersusAni m_VersusAni;

protected:
	bool  m_bFadeOutStart;
	ioPlayMode *m_pPlayMode;
	int   m_iSelectChar;
	DWORD m_dwSelectTime;
	DWORD m_dwStartTime;
public:
	virtual void iwm_create();
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate = false );

protected:
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

protected:
	void OnRenderBack( int iXPos, int iYPos );
	void OnRenderBlueChar( int iXPos, int iYPos );
	void OnRenderRedChar( int iXPos, int iYPos );
	void OnRenderVersus( int iXPos, int iYPos );

protected:
	void OnProcessChar();
	void OnProcessVS();
public:
	void StartFadeOut( ioPlayMode *pPlayMode, int now_select_char, DWORD dwSelectTime );

protected:
	static ioCreateChar *GetCreateCharacter( ioUI3DRender *p3DRender, HeroData &rkData );

public:
	static void MyHeroInfoUpdate( int iOtherTeamType );
	static void OtherHeroInfoUpdate( int iOtherTeamType, SP2Packet &rkPacket );

	static const int& GetMMRPoint( int iTeamType );

public:
	SuccessionMatchVersusSceneWnd();
	virtual ~SuccessionMatchVersusSceneWnd();
};

//--------------------------------------------------------------------------------------------------
class WaitEffectWnd:	public ioWnd
{
protected:
	DWORD					m_dwCurSearchTime;
	DWORD					m_dwFullSearchTime;
	int						m_iCurSearchSec;
	DWORD					m_dwChangeCountTime;

	ioUIRenderImage*		m_pWarCountWait;
	ioUIRenderImage*		m_pWarCountCircle;
	ioUIRenderImage*		m_pTextBack;

	bool					m_bStart;
	ioHashString			m_szMent;

	ioWnd*					m_pParent;
	bool					m_bNullParent;

public:
	void StartSearch( ioWnd* pParent, bool bNullParent, DWORD dwFullTime, ioHashString szMent );
protected:
	void Clear();
	bool CheckParentShow();
protected:
	virtual void iwm_hide();
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();
public:
	void RenderWaitAni();
public:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void ParseExtraInfo( ioXMLElement &xElement );
public:
	WaitEffectWnd(void);
	virtual ~WaitEffectWnd(void);
};