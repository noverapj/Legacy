#pragma once

class CQuickModeToolTip : public ioWnd
{
public:
	enum
	{
		DEFAULT_WIDTH = 132,
		DEFAULT_HEIGHT= 60,

		START_TITLE_X  = 7,
		START_TITLE_Y  = 6,

		START_HELP_X   = 7,
		START_HELP_Y   = 22,

		LINE_GAP_HEIGHT= 16,
	};

protected:
	int m_iQuickstartModeHelpIndex;
	
	int m_nMinPlayer;
	int m_nMaxPlayer;

	bool m_bSamePlayer;

	ioHashString m_szModeTitle;
	
protected:
	ioWnd *m_pCallWnd;

public:
	void SetModeHelpTooltip( ioWnd *pCallWnd, const int nMode );

protected:
	void ProcessPosition();
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	CQuickModeToolTip(void);
	virtual ~CQuickModeToolTip(void);

};

