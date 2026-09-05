#pragma once

#include "ioSprite.h"

class BingoRightWnd : public ioWnd
{
public:
	enum
	{
		ID_BINGO_START_BTN        = 1,
		ID_GET_NUMBER_BTN         = 2,
		ID_SHUFFLE_NUMBER_BTN     = 3,
		ID_SHUFFLE_REWARD_BTN     = 4,
		ID_GET_SPECIAL_NUMBER_BTN = 5,
		ID_INIT_BTN               = 6,
		ID_REWARD_LIST_BTN        = 7,
		ID_NEW_BINGO_BTN          = 8,
		ID_CLOSE_BTN			  = 9,

		ID_HELP_BTN_1 = 20,
		ID_HELP_BTN_2 = 21,
		ID_HELP_BTN_3 = 22,

		ID_MESSAGE_BOX = 100,
	};

	enum BallAniState
	{
		BAS_NONE,
		BAS_POP,
		BAS_SHINING,
		BAS_WAIT,
		BAS_MOVING,
		BAS_NUMBER_EFFECT,
		BAS_REWARD_EFFECT,
	};

	enum RightWndType
	{
		RWT_NONE,
		RWT_START,
		RWT_RAND_PLAY,
		RWT_FIXED_PLAY,
		RWT_ALL_BINGO,
	};

	struct BallAfterImage
	{
		D3DXVECTOR2 m_vPosition;
		int m_iAlpha;
		float m_fScale;
		DWORD m_dwEndTime;
		BallAfterImage()
		{
			m_vPosition = D3DXVECTOR2( 0, 0 );
			m_iAlpha = 0;
			m_fScale = FLOAT1;
		}
		BallAfterImage( D3DXVECTOR2 vPos, int iAlpha )
		{
			m_vPosition = vPos;
			m_iAlpha = iAlpha;
			m_fScale = FLOAT1;
			m_dwEndTime = FRAMEGETTIME() + 250;
		}
	};
	typedef std::vector<BallAfterImage> BallAfterImageList;

protected:
	ioUIRenderImage *m_pUnderLine;
	ioUIRenderImage *m_pTextBingo;
	ioUIRenderImage *m_pTextAll;
	ioUIRenderImage *m_pTextClear;
	ioUIRenderImage *m_pTextNumber;
	ioUIRenderImage *m_pTextCount;
	ioUIRenderImage *m_pTextSelect;
	ioUIRenderImage *m_pTextGet;
	ioUIRenderImage *m_pRedBall;
	ioUIRenderImage *m_pSubIcon;
	ioUIRenderImage *m_pLightBall;
	ioUIRenderImage *m_pBallBg;
	ioUIRenderImage *m_pBallEffect[10];

protected:
	RightWndType m_WindowType;
	DWORD m_dwCheckTime;
	bool m_bLightIcon;

	int m_iLastGetNumber;

	BallAniState m_BallAniState;
	DWORD m_dwBallAniCheckTime;

	ioSprite m_BallEffect;
	D3DXVECTOR2 m_vBallPos;
	D3DXVECTOR2 m_vCurPos;
	D3DXVECTOR2 m_vTargetPos;

	ioHashString m_ShuffleNumberItemName;
	ioHashString m_ShuffleRewardItemName;
	ioHashString m_SpecialNumberItemName;

	ioHashString m_szBallAniSound1;
	ioHashString m_szBallAniSound2;
	ioHashString m_szBallAniSound3;
	ioHashString m_szAllBingoSound;

	BallAfterImageList m_vBallAfterImageList;

protected:
	int m_iBingoType;

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void ParseExtraInfo( ioXMLElement &xElement );

public:
	virtual void iwm_create();
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnRender();
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRenderAfterChild();

protected:
	void RenderStart( int x, int y );
	void RenderRandPlay( int x, int y );
	void RenderFixedPlay( int x, int y );
	void RenderAllBingo( int x, int y, ioUIRenderImage* pTextImage );
	void ProcessBallAni();
	void RenderBallAni( int x, int y );

public:
	void SetWindowType( RightWndType eType );
	void SetBallAni( int iNumber, ioBingo *pBingo, const ioHashString& szNewBallImage, const ioHashString& szSubImage );
	void ShowGetNumber( int iNumber );

public:
	void SetActvieChildBtn();
	void SetInActvieChildBtn();

protected:
	D3DXVECTOR2 PointOnCubicBezier( D3DXVECTOR2* vP, float t );
	D3DXVECTOR2 CalcLinear( D3DXVECTOR2 vPos0, D3DXVECTOR2 vPos1, float t );

public:
	BingoRightWnd();
	virtual ~BingoRightWnd();
};

class BingoHelpButton : public ioButton
{
public:
	enum
	{
		ID_HELP_TOOL_TIP = 1,
	};

public:
	virtual void iwm_hide();
	virtual void iwm_mouseover( const ioMouse& mouse );
	virtual void iwm_mouseleave( const ioMouse& mouse );

protected:
	virtual void OnRender();

public:
	BingoHelpButton();
	virtual ~BingoHelpButton();
};