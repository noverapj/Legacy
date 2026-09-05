#pragma once

#include "../io3DEngine/ioMovingWnd.h"

class ExcavationResultWnd : public ioMovingWnd
{
public:
	enum
	{
		ID_IDENTIFY = 1,
		ID_OK       = 2,
		ID_EXIT		= 3
	};

	enum
	{
		STATE_IDENTIFING,
		STATE_IDENTIFY_COMPLETE,
	};

	enum
	{
		TEXT_PRICE_ROLLING = 0,
		TEXT_PRICE,
		TEXT_ETCITEM_DESC,
		TEXT_AUTO_CLOSE,
		TEXT_PRICE1_BUTTON,
		TEXT_PRICE2_BUTTON,
		TEXT_PRICE3_BUTTON,
		TEXT_MAX,
	};

	enum
	{
		GRADE_SSS,
		GRADE_SS,
		GRADE_S,
		GRADE_A,
		GRADE_B,
		GRADE_C,
		GRADE_D,
		GRADE_E,
		GRADE_F,
		GRADE_MAX,
	};

	enum
	{
		RANK_BLUE,
		RANK_YELLOW,
		RANK_RED,
		RANK_MAX
	};

	enum
	{
		EFFECT_STATE_NONE = 0,
		EFFECT_STATE_RANK = 1,
		EFFECT_STATE_COMPLETE = 2,
		EFFECT_STATE_DELAY = 2,
		EFFECT_STATE_EFFECT = 3,
	};

private:
	ioUIRenderImage *m_pIcon;
	ioUIRenderImage *m_pIconBG;
	ioUIRenderImage *m_pIconBGRolling;
	ioUIRenderImage *m_pResultGrade;
	ioUIRenderImage *m_pGrade[GRADE_MAX];
	ioUIRenderImage *m_pResultRank;
	ioUIRenderImage *m_pRank[RANK_MAX];
	ioUIRenderImage *m_pMultiple2;
	ioUIRenderImage *m_pMultiple3;
	ioUIRenderImage *m_pMultiple4;
	ioUIRenderImage *m_pEvent;
	ioUI3DEffectRender *m_pRankEffect;

	int m_iState;

	int m_iRollingIndex;
	int m_iRollingGrade;

	DWORD m_dwRollingTime;
	DWORD m_dwRollingChangeTime;
	DWORD m_dwWindowCloseTime;
	DWORD m_dwRollingEndTime;
	DWORD m_dwRollingCheckTime;

	int m_iResultItemType;
	int m_iResultGradeIdx;
	int m_iResultArtifactIdx;
	int m_iResultPrice;
	int m_iResultIdentifyPrice;
	int m_iResultCount;

	ioHashString m_Text[TEXT_MAX];
	ioHashString m_szCloseTime;
	ioHashString m_szRollingSound;
	ioHashString m_szRollingStopSound;
	ioHashString m_szMultipleSound;
	IoString m_pCloseTime;
	ioComplexStringPrinter m_PopupClosePrinter;

	DWORD m_dwRollingSoundID;
	DWORD m_dwCloseWndTime;

	bool m_bReappraisal;
	bool m_bReserve;

	DWORD m_dwTwinkleTime;
	DWORD m_dwTwinkleCheckTime;

	bool m_bTwinkle;
	bool m_bEndGradeEvent;
	bool m_bEndEventDelay;
	bool m_bShowMultiple;
	bool m_bShow3DEffect;

	DWORD m_dwEventCheckTime;

	DWORD m_dwNormalGradeEventTime;
	DWORD m_dwHighGradeEventTime;
	DWORD m_dwEventDelayTimeToMulti;
	DWORD m_dwEventDelayTimeTo3DEffect;
	DWORD m_dwMultipleEventTime;

	float m_fGradeEventRate;
	float m_fMultipleEventRate;

public:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void AddEffect( const ioHashString &szType, ioUI3DEffectRender *pUI3DRender );
	virtual void ParseExtraInfo( ioXMLElement &xElement );
	virtual void SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate /* = false */ );

	virtual void iwm_show();
	virtual void iwm_hide();
	virtual bool iwm_spacebar();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();
	void SetArtifactInfo();

private:
	void StartEventDelay();
	void StartMultipleEvent();
	void Start3DEffectEvent();

	void ProcessGradeEvent();
	void ProcessEventDelay();
	void ProcessMultipleEvent();
	void Process3DEffectEvent( float fTimePerSec );
	void RenderGradeEvent( int iXPos, int iYPos );
	void RenderMultipleEvent( int iXPos, int iYPos );
	void Render3DEffectEvent( int iXPos, int iYPos );

public:
	void SetReserveResultItem( int iItemType, int iArtifactIdx, int iGradeIdx, int iPrice, int iIdentifyPrice, int iGetCnt );
	void SetReappraisal( int iItemType, int iArtifactIdx, int iGradeIdx, int iPrice, int iIdentifyPrice, int iGetCnt );

private:
	void Reset();
	void ClearResultData();
	void CheckButton();
	void SetButtonPrice();
	void SetRankIcon( int iGradeIndex );

public:
	bool IsReserved() { return m_bReserve; }

public:
	ExcavationResultWnd();
	virtual ~ExcavationResultWnd();
};
