#pragma once

#include "ioSprite.h"

typedef std::vector<ioUIRenderImage*> ioUIRenderImageList;

//////////////////////////////////////////////////////////////////////////
class BingoWnd : public ioWnd
{
public:
	enum
	{
		ID_EXIT                = 100,
		ID_RIGHT_WND           = 1000,
		ID_TOOLTIP             = 1001,

		ID_REWARD_BTN_1         = 11,
		ID_REWARD_BTN_12        = 22,
		ID_ALL_BINGO_REWARD_BTN = 23,

		ID_NUMBER_BTN_1         = 101,
		ID_NUMBER_BTN_25        = 125,
	};

protected:
	enum UIState
	{
		US_NONE,
		US_ROLLING,
		US_STOP,
	};

protected:
	int m_iBingoType;

protected:
	ioUIRenderImage *m_pStopEffect[10];

protected:
	UIState m_State;
	DWORD m_dwRollingGapTime;
	DWORD m_dwRollingStartTime;
	DWORD m_dwNumberRollingEndTime;
	DWORD m_dwRewardRollingEndTime;
	DWORD m_dwRollingCheckTime;

	ioHashString m_szRollingSound;
	ioHashString m_szStopSound;

protected:
	int m_iSelectDummyCode;
	int m_iSelectNumber;

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void ParseExtraInfo( ioXMLElement &xElement );

public:
	virtual void iwm_create();
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

public:
	void iwm_bingo_btn( DWORD dwID );

public:
	void ReloadBtnEffect();
	void ShowBingoWnd( const ioHashString& szTitle );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	void SetRollingNumber();
	void SetRollingReward();
	void SetStopNumber();
	void SetStopReward();

public:
	void SetNumber( bool bCheckClear );
	void SetReward( bool bCheckClear);
	void SetBingo( int iLineIndex );

public:
	DWORD GetRollingGapTime() { return m_dwRollingGapTime; }
	void GetEffectList( OUT ioUIRenderImageList &rkImageList );

	const ioHashString &GetStopSound() { return m_szStopSound; }

public:
	bool IsNumberBingoHorz( int iIndex );
	bool IsNumberBingoVert( int iIndex );
	bool IsNumberBingoLT( int iIndex );
	bool IsNumberBingoRB( int iIndex );

protected:
	bool IsAllStop();

public:
	BingoWnd();
	virtual ~BingoWnd();
};

//////////////////////////////////////////////////////////////////////////
class RewardButton : public ioWnd
{
protected:
	enum UIState
	{
		US_NONE,
		US_ROLLING,
		US_STOP,
	};

protected:
	ioUIRenderImage *m_pIcon;
	ioUIRenderImage *m_pHiddenIcon;
	ioUIRenderImage *m_pIconBg;
	ioUIRenderImage *m_pButtonBg;
	ioUIRenderImage *m_pGettedItem;

protected:
	UIState m_State;
	int m_iRewardIndex;
	int m_iRollingIndex;

	DWORD m_dwRollingCheckTime;
	DWORD m_dwRollingGapTime;
	DWORD m_dwRollingStopTime;

	ioSprite m_OpenEffect;
	ioSprite m_GetEffect;

	bool m_bClear;

	DWORD m_dwCheckTime;
	bool m_bLightIcon;


protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

public:
	virtual void iwm_create();
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_mouseover( const ioMouse& mouse );
	virtual void iwm_mouseleave( const ioMouse& mouse );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	void SetInfo( int iIndex, bool bCheckClear );
	void SetClear();
	void SetRolling();
	void SetStop( DWORD dwStopTime );
	bool IsStop();
	bool IsRolling();

	void ReLoadEffect();

public:
	RewardButton();
	virtual ~RewardButton();
};


//////////////////////////////////////////////////////////////////////////
class NumberButton : public ioButton
{
protected:
	enum UIState
	{
		US_NONE,
		US_ROLLING,
		US_STOP,
	};

protected:
	ioUIRenderImage *m_pButtonBg;
	ioUIRenderImage *m_pButtonBgLight;
	ioUIRenderImage *m_pButtonBgClear;
	ioUIRenderImage *m_pButtonBgClearTop;
	ioUIRenderImage *m_pButtonBgClearLeft;
	ioUIRenderImage *m_pButtonBgClearLeftTop;
	ioUIRenderImage *m_pButtonBingoEffect;
	ioUIRenderImage *m_pHasDummy;

	ioUIRenderImage *m_pUnCheckIconImg;
	ioUIRenderImage *m_pCheckIconImg;	

protected:
	UIState m_State;
	int m_iNumber;
	int m_iRollingNumber;
	int m_iBingoType;

	DWORD m_dwRollingCheckTime;
	DWORD m_dwRollingGapTime;
	DWORD m_dwRollingStopTime;

	ioSprite m_OpenEffect;
	ioSprite m_BingoEffect;

	bool m_bClear;

	DWORD m_dwCheckTime;
	bool m_bLightIcon;
	bool m_bHasDummy;

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

public:
	virtual void iwm_create();
	virtual void iwm_show();
	virtual void iwm_hide();

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

	virtual void iwm_mouseover( const ioMouse& mouse );
	virtual void iwm_mouseleave( const ioMouse& mouse );

protected:
	void OnRenderByRandType();
	void OnRenderByFixedType();

public:
	void SetInfo( int iNumber, bool bCheckClear );
	void SetClear();
	void SetRolling();
	void SetStop( DWORD dwStopTime );
	int GetNumber() { return m_iNumber; }
	bool IsRolling();
	bool IsClear() { return m_bClear; }

	void ReLoadEffect();

public:
	NumberButton();
	virtual ~NumberButton();
};