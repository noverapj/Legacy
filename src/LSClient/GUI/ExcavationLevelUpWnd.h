#pragma once


class ExcavationLevelUpWnd :
	public ioWnd
{
public:
	enum
	{
		ID_EXIT = 1,
		ID_CLOSE = 2,
		ID_ALPHA_WND = 100
	};

	enum
	{
		TEXT_RESULT_LEVEL_UP_1 = 0,
		TEXT_RESULT_LEVEL_UP_2,
		TEXT_RESULT_LEVEL_UP_3,
		TEXT_RESULT_KIT_UP,
		TEXT_MAX
	};

	enum
	{
		WINDOW_WIDTH = 321,
		WINDOW_HEIGHT_NORMAL = 263,
		WINDOW_HEIGHT_KITUP = 283,
		BOTTOM_FRAME_YPOS = 198,
		BOTTOM_FRAME_KITUP_YPOS = 218,
		BTN_CLOSE_YPOS = 215,
		BTN_CLOSE_KITUP_YPOS = 235,
	};

private:
	ioUIRenderImage *m_pIcon;
	ioUIRenderImage *m_pIconBack;

protected:
	ioComplexStringPrinter	m_LevelUpPrinter;
	ioComplexStringPrinter	m_KitUpPrinter;
	int m_iLevel;
	bool m_bKitUp;

	ioHashString m_Text[TEXT_MAX];
	ioHashString m_szUpSound;

protected:
	virtual void OnRender();

public:
	virtual void ParseExtraInfo( ioXMLElement &xElement );

	virtual void iwm_show();
	virtual void iwm_hide();
	virtual bool iwm_esc();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

	void SetLevel( int iLevel, bool bKitUp );
	void SetIcon();

	void SetFrameSize( bool pKitUp );

public:
	ExcavationLevelUpWnd(void);
	virtual ~ExcavationLevelUpWnd(void);
};

