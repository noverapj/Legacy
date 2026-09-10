#pragma once

class ioUIRenderImage;
class io2DCircleGauge;

class ioFlagPointGaugeHelper
{
private:
	enum
	{
		CIRCLE_GAUGE_OFFSET_X = 64,
		CIRCLE_GAUGE_OFFSET_Y = 64
	};

private:
	ioUIRenderImage *m_pGaugeBack;
	ioUIRenderImage *m_pGaugeInnerBack;
	ioUIRenderImage *m_pTimeNumber;
	ioUIRenderImage *m_pScoreNumber;
	ioUIRenderImage *m_pScorePlus;
	ioUIRenderImage *m_pFlagBlue;
	ioUIRenderImage *m_pFlagRed;
	ioUIRenderImage *m_pFlagMine;
	ioUIRenderImage *m_pCurFlag;

	io2DCircleGauge *m_pCircleGauge;

	int m_iCurScore;
	DWORD m_dwPassedTime;

	float m_fGaugeRate;

public:
	void InitResource();
	void ClearResource();

	void SetTeam( TeamType eType, bool bIsOwner );

	void UpdateFlagPointGauge( int iXPos, int iYPos, float fCurAmt, float fMaxAmt, DWORD dwPassedTime );
	void RenderFlagPointGauge( float fXPos, float fYPos );

protected:
	void UpdateGauge( int iXPos, int iYPos, float fCurAmt, float fMaxAmt );
	void UpdatePoint( float fCurAmt, DWORD dwPassedTime );
	void RenderGauge( float fXPos, float fYPos );
	void RenderPoint( float fXPos, float fYPos );

public:
	ioFlagPointGaugeHelper(void);
	virtual ~ioFlagPointGaugeHelper(void);
};

