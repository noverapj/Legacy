#pragma once

class AniTextWnd : public ioMovingWnd
{
public:
	enum SubWnd
	{
		ARRIVAL_SUBWND = 1,
		DESTROY_SUBWND,
		RUNAWAY_SUBWND,
		AWAKENING_SUBWND,
	};

	enum ShowAniType 
	{
		SAT_NONE		= 0,
		SAT_START_ANI	= 1,
		SAT_END			= 2,
	};

protected:
	ShowAniType			m_eState;
	SubWnd				m_eType;

public:
	ShowAniType GetState() const { return m_eState; }
	void SetState(ShowAniType eState);

	void StartAniText( SubWnd eSub );

public:
	AniTextWnd(void);
	virtual ~AniTextWnd(void);
	
};

class AniLetterSubWnd : public ioMovingWnd
{
public:
	enum ShowAniType 
	{
		SAT_NONE		= 0,
		SAT_START_ANI,
		SAT_DELAY,
		SAT_ZOOM_OUT,
		SAT_VIB,
		SAT_ANI_COLOR,
		SAT_ZOOM_IN,
		SAT_END,
	};

protected:
	ioUIRenderImage*	m_pLetter;
	ShowAniType			m_eState;

	int					m_nOffSet;

	DWORD				m_dwAniTime;
	DWORD				m_dwZoomTime;

	float				m_fSpeed;
	float				m_fScaleSpeed;
	float				m_fAlphaRate;

	bool				m_bLeftToRight;

	struct stExtraFactor
	{
		DWORD	dwStartTime;
		DWORD	dwStartDuration;
		DWORD	dwDelayDuration;
		DWORD	dwVibDuration;
		DWORD	dwTextClrDuration;
		DWORD	dwZoomInDuration;
		
		int		nStartXPos;
		int		nZoomOutXPos;

		float	fStartScale;
		float	fStartYPosRatio;
		
	};

	struct stAniText
	{
		float	fX;
		float 	fY;
		float	fScale;
	};

	stAniText			m_stStart;
	stAniText			m_stDest;
	stAniText			m_stCurrPos;

	stExtraFactor		m_stExFactor;

	ioHashString		m_szBGM;
	ioHashString		m_szEffect;

public:
	ShowAniType GetState() const { return m_eState; }
	void SetState(ShowAniType eState);

public:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void ParseExtraInfo( ioXMLElement &xElement );

protected:
	virtual void OnRender();
	virtual void OnProcess( float fTimePerSec );
	virtual void iwm_create();

public:
	AniLetterSubWnd(void);
	virtual ~AniLetterSubWnd(void);
	

};