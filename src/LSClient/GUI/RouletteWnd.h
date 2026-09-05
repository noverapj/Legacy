#pragma once

#include "../io2DRotateQuad.h"

class RouletteWnd : public ioMovingWnd
{
public:
	enum UIState
	{
		US_READY,
		US_SPIN,
		US_JOIN_END,
		US_END,
	};

protected:
	enum
	{
		ID_HELP = 1,
	};

	struct UIEffectAniInfo
	{
		enum AniState{ AS_PLAY, AS_END };
		AniState m_State;
		int   m_iXPos;
		int   m_iYPos;
		int   m_iAlpha;
		float m_fScale;
		int   m_iNum;
		DWORD m_dwEndTime;
		ioUIRenderImage *m_pImage;
		UIEffectAniInfo( ioUIRenderImage *pImage, int x, int y, int iNum = -1, int iAlpha = 0, float fScale = 2.0f )
		{
			m_State = AS_PLAY;
			m_pImage = pImage;
			m_iXPos  = x;
			m_iYPos  = y;
			m_iNum   = iNum;
			m_iAlpha = iAlpha;
			m_fScale = fScale;
			m_dwEndTime = FRAMEGETTIME() + 500;
		}
		ioUIRenderImage *GetImage() { return m_pImage; }
	};
	typedef std::vector<UIEffectAniInfo> UIEffectAniInfoList;

protected:
	UIState m_State;
	UIEffectAniInfoList m_UIEffectList;

	ioUIRenderImage *m_pRouletteHelpBg;
	ioUIRenderImage *m_pTextBg;
	ioUIRenderImage *m_pWheel;

	ioUIRenderImage *m_pTextTitle;
	ioUIRenderImage *m_pTextEnableJoin;
	ioUIRenderImage *m_pTextJoin;
	ioUIRenderImage *m_pTextEnd;
	ioUIRenderImage *m_pTextEndSec;
	ioUIRenderImage *m_pTextEndNum;
	ioUIRenderImage *m_pTextPresent;
	ioUIRenderImage *m_pTextReward[3];
	ioUIRenderImage *m_pTextRewardBg[3];

	ioUIRenderImage *m_pUpperCoupon;
	ioUIRenderImage *m_pUpperCouponNumber;

	ioUIRenderImage *m_pUnderJoin;
	ioUIRenderImage *m_pUnderCount;

	ioUIRenderImage *m_pArrow;

	io2DRotateQuad *m_pWheelQuad;

	ioHashStringVec m_vRouletteTexName;

protected:
	int m_iCurResultArray;
	int m_iCoin;
	int m_iJoinUser;

	bool m_bSetUIEffect;
	bool m_bMainInfo;
	DWORD m_dwInfoDelayTime;

	int m_iInfoAlphaRate;
	bool m_bFadeIn;

	bool m_bSpinEndEffect;

	DWORD m_dwRewardStartTime;

	bool m_bJoinEnd;

	static bool m_bPosToDefualt;
	int m_iDefaultX;
	int m_iDefaultY;

public:
	virtual void iwm_create();
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

public:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void ParseExtraInfo( ioXMLElement &xElement );
	virtual void SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate = false );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

protected:
	void RenderReward( int iXPos, int iYPos );
	void RenderCenterInfo( int iXPos, int iYPos );
	void RenderUpperInfo( int iXPos, int iYPos );
	void RenderUnderInfo( int iXPos, int iYPos );
	void RenderUIEffect();
	void SetUIEffect( ioUIRenderImage *pImage, int x, int y, int iNum = -1 );

public:
	void SetState( UIState eState );

public:
	static bool GetPosToDefualt() { return m_bPosToDefualt; }
	static void SetPosToDefualt(bool PosToDefualt) { m_bPosToDefualt = PosToDefualt; }

public:
	RouletteWnd();
	virtual ~RouletteWnd();
};
