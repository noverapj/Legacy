#pragma once

#include "../ioComplexStringPrinter.h"

class ioPlayMode;
class ioUIRenderImage;
class CenterHelpWnd : public ioWnd
{
protected:
	ioUIRenderImage *m_pHelpBack;

protected:
	ioHashString m_szShowSound;

protected:
	ioComplexStringPrinter m_Printer;
	DWORD m_dwFirstDelay;
	DWORD m_dwShowTime;
	DWORD m_dwCurrentTime;	
	DWORD m_dwFadeTime;
	int   m_iAlphaRate;

protected:
	enum
	{
		STATE_DELAY  = 0,
		STATE_FADEIN = 1,
		STATE_VIEW   = 2,
		STATE_FADEOUT= 3,
	};
	DWORD m_dwState;

public:
	void SetCenterHelp( ioComplexStringPrinter &rkPrinter, DWORD dwFirstDelay, DWORD dwShowTime );

public:
	virtual void iwm_show();
	virtual void iwm_hide();

public:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void ParseExtraInfo( ioXMLElement &xElement );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

protected:
	void SetState( DWORD dwState );

public:
	CenterHelpWnd();
	virtual ~CenterHelpWnd();
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class MonsterSurvivalHelWnd : public ioWnd
{
protected:
	ioUIRenderImage *m_pContinueText;
	ioUIRenderImage *m_pContinueCount;
	ioUIRenderImage *m_pClearText;
	ioUIRenderImage *m_pGameOverText;
	ioUIRenderImage *m_pKOText;
	ioUIRenderImage *m_pRevivalDelay;
	ioUIRenderImage *m_pAttackKeyText;
	ioUIRenderImage *m_pDeathTimeText;

protected:
	ioPlayMode *m_pPlayMode;

protected: // 종료 카운트
	DWORD m_dwContinueTime;
	DWORD m_dwCurContinueTime;
	DWORD m_dwCurContinueSec;
	DWORD m_dwCurContinueSecTime;

protected: // 마법진
	DWORD m_dwZoneCollisionTime;
	DWORD m_dwZoneCurCollisionTime;

protected: // 골드 몬스터 코인
	DWORD m_dwCurGoldMonsterCoinTime;

protected: // KO
	DWORD m_dwCurKOTime;

protected: // DeathTime
	DWORD m_dwCurDeathTime;

public:
	void SetPlayMode( ioPlayMode *pPlayMode );

public:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	
protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();
	void OnKOProcess();
	void OnDeathTimeProcess();

protected:
	bool IsRenderGoldMonsterCoinHelp();
	void RenderGoldMonsterCoinHelp( int iXPos, int iYPos );
	void RenderContinueHelp( int iXPos, int iYPos );
	void RenderMagicZoneHelp( int iXPos, int iYPos );
	void RenderKOHelp( int iXPos, int iYPos );
	void RenderDeathTimeHelp( int iXPos, int iYPos );

public:
	void SetContinueTime( DWORD dwContinueTime );
	void SetZoneCollisionTime( DWORD dwZoneCollisionTime );
	void SetKOTime( DWORD dwCurTime );
	void SetDeathTime( DWORD dwCurTime );

public:
	MonsterSurvivalHelWnd();
	virtual ~MonsterSurvivalHelWnd();
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct ShuffleHelpMent
{
	ioHashString m_szGenerateMent;
	DWORD m_dwGenerateMentTime;
	DWORD m_dwGenerateColor;
	float m_fGenerateScale;

	ShuffleHelpMent()
	{
		m_dwGenerateMentTime = 3000;
		m_dwGenerateColor    = 0xffffffff;
		m_fGenerateScale     = 0.83f;
	}
};

class ShuffleCenterHelpWnd : public ioWnd
{
public:
	enum SHOWTYPE
	{
		ST_NONE,
		ST_ADD_MONSTER,
		ST_ADD_STAR,
		ST_ADD_BUFF_BOX,
		ST_GET_BUFF_BOX,
	};

protected:
	ioUIRenderImage *m_pAddMonsterText;

	ioUIRenderImage *m_pAddStarText;
	ioUIRenderImage *m_pAddStarIcon;

	ioUIRenderImage *m_pAddBuffBoxIcon;
	ioUIRenderImage *m_pAddBuffBoxText;
	
	ioUIRenderImage *m_pGetBuffBoxIcon;
	ioUIRenderImage *m_pGetBuffBoxText;

protected:
	ioHashString m_szShowSound;

protected:
	int m_iShowType;

	ioComplexStringPrinter m_Printer;
	DWORD m_dwFirstDelay;
	DWORD m_dwShowTime;
	DWORD m_dwCurrentTime;	
	DWORD m_dwFadeTime;
	int   m_iAlphaRate;

protected:
	enum
	{
		STATE_DELAY  = 0,
		STATE_FADEIN = 1,
		STATE_VIEW   = 2,
		STATE_FADEOUT= 3,
	};
	DWORD m_dwState;

public:
	void SetCenterHelp( int iShowType, const ioHashString& szMentText, float fScale, DWORD dwColor, DWORD dwFirstDelay, DWORD dwShowTime );
	void SetCenterHelpGetBuffItem( int iShowType, const ioHashString& szChrName, const ioHashString& szItemName, const ioHashString& szIconName, float fScale, DWORD dwColor, DWORD dwFirstDelay, DWORD dwShowTime );
	void SetGetBuffItemIcon( const ioHashString& szIconName );

public:
	virtual void iwm_show();
	virtual void iwm_hide();

public:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void ParseExtraInfo( ioXMLElement &xElement );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	void OnAddMonsterRender( int iXPos, int iYPos );	
	void OnIconAndTextRender( int iXPos, int iYPos, ioUIRenderImage* pIcon, ioUIRenderImage* pText );

protected:
	void SetState( DWORD dwState );

public:
	ShuffleCenterHelpWnd();
	virtual ~ShuffleCenterHelpWnd();
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class RaidReadyCountWnd : public ioWnd
{
protected:
	enum RenderType
	{
		RT_None,
		RT_Count,
		RT_Go,
	};

	enum FadeTime
	{
		FT_Count_Text = 300,
		FT_Count_Num = 200,

		FT_Go_Start = 100,
		FT_Go_Delay = 300,
		FT_Go_Text = 100,
		FT_Go_Effect = 500,
	};

	enum GoState
	{
		GS_Start,
		GS_Delay,
		GS_Text_Fade,
	};

protected:
	ioUIRenderImage *m_pCountText;
	ioUIRenderImage *m_pCountNum[10];
	ioUIRenderImage *m_pBgEffectText;
	ioUIRenderImage *m_pGoText;

protected:
	ioPlayMode *m_pPlayMode;

protected:
	RenderType m_RenderType;
	GoState m_GoState;

protected:
	DWORD m_dwMaxCountSec;
	DWORD m_dwCurCountSec;
	DWORD m_dwCurCountTime;
	DWORD m_dwCurCountChangeTime;

	int m_nCountTextAlpha;
	int m_nCountNumAlpha;

protected:
	DWORD m_dwGoTime;
	DWORD m_dwGoEffectTime;
	float m_fGoTextAlphaRate;
	float m_fGoBgAlphaRate;
	float m_fGoEffectAlphaRate;
	float m_fGoEffectScaleRate;

protected:
	ioHashString m_szCountSound;
	ioHashString m_szGoSound;

public:
	void SetPlayMode( ioPlayMode *pPlayMode );

public:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void ParseExtraInfo( ioXMLElement &xElement );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

protected:
	void ProcessCount();
	void ProcessGO();

protected:
	void RenderCount( int iXPos, int iYPos );
	void RenderGO( int iXPos, int iYPos );
	
public:
	void SetCountState( DWORD dwMaxCountTime );

protected:
	void SetGoState();

public:
	RaidReadyCountWnd();
	virtual ~RaidReadyCountWnd();
};