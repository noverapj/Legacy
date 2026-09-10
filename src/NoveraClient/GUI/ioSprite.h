#pragma once

class ioSprite
{
protected:
	typedef std::vector<ioUIRenderImage*> ioUIRenderImageList;

public:
	enum AniType
	{
		AT_PLAY_ONCE,
		AT_LOOP,
	};

	enum SpriteState
	{
		SS_READY,
		SS_PLAY,
		SS_STOP,
	};

protected:
	AniType m_AniType;
	SpriteState m_State;

	DWORD m_dwCheckTime;
	DWORD m_dwAniGapTime;
	int m_iCurSprite;
	int m_iMaxSpriteCnt;
	ioUIRenderImageList m_vSpriteList;

	DWORD m_dwAniDelayTime;
	DWORD m_dwAniStartTime;
	DWORD m_dwAniEndTIme;
	DWORD m_dwDuration;

	int m_iCurAlpha;
	int m_iStartAlpha;
	int m_iEndAlpha;

	float m_fCurScale;
	float m_fStartScale;
	float m_fEndScale;

public:
	void Init();
	void AddSpriteList( ioUIRenderImageList vList );
	void DestroySpriteList();
	
	void SetType( AniType eType );
	void SetAnimationGapTime( DWORD dwGapTime );
	void SetAnimationDuration( DWORD dwDuration );
	void SetAlphaAnimation( int iStartAlpha = MAX_ALPHA_RATE, int iEndAlpha = MAX_ALPHA_RATE );
	void SetScaleAnimation( float fStartScale = FLOAT1, float fEndScale = FLOAT1 );

	void SetPlay( DWORD dwDelayTime = 0 );
	void SetStop() { m_State = SS_STOP; }
	bool IsStop();

	void Process();
	void Render( int iXpos, int iYPos, float fScale = FLOAT1, UIRenderType eRenderType = UI_RENDER_NORMAL, TextureFilterOption eFilterOption = TFO_NEAREST);
	int  GetState() { return m_State; }

public:
	ioSprite();
	~ioSprite();
};