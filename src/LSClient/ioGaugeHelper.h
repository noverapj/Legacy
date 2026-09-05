#pragma once

class ioUIRenderImage;

class ioGaugeHelper
{
private:
	ioUIRenderImage *m_pGaugeBack;
	ioUIRenderImage *m_pGauge;
	ioUIRenderImage *m_pGaugeScreenLeft;
	ioUIRenderImage *m_pGaugeScreenCenter;
	ioUIRenderImage *m_pGaugeScreenRight;

	ioUIRenderImage *m_pObserverGaugeBack;
	ioUIRenderImage *m_pObserverGauge;
	ioUIRenderImage *m_pObserverGaugeScreenLeft;
	ioUIRenderImage *m_pObserverGaugeScreenCenter;
	ioUIRenderImage *m_pObserverGaugeScreenRight;

	TeamType m_TeamType;
	int		m_iRenderAmt;

	bool m_bSetAlpha;

public:
	void InitResource();
	void SetTeam( TeamType eTeam );
	void SetGaugeAlpha( bool bSetAlpha );
	void SetScale(float fScaleX, float fScaleY);

public:
	void UpdateGauge( float fCurAmt, float fMaxAmt, bool bObserver=false );
	void RenderGauge( float fXPos, float fYPos, bool bObserver=false );

protected:
	void RenderNormal( float fXPos, float fYPos );
	void RenderOvserver( float fXPos, float fYPos );

private:
	bool IsResouceOK() const;
	void ClearResource();

public:
	int GetGaugeBackWidth( bool bObserver=false ) const;
	int GetGaugeBackHeight( bool bObserver=false ) const;

public:
	ioGaugeHelper();
	~ioGaugeHelper();
};

