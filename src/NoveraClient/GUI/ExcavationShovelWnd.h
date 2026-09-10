#pragma once

class ExcavationShovelWnd : public ioWnd
{
public:
	enum
	{
		ID_EXIT_BTN   = 1,
		ID_CHARGE_BTN = 2,
		ID_BUY_BTN    = 3,
		ID_CLOSE_BTN  = 4,
	};

	enum
	{
		TEXT_CUR_ITEM,
		TEXT_CHARGE_SHOVEL,
		TEXT_BUY_SHOVEL,
		TEXT_IMPOSSIBLE_CHARGE,
		TEXT_MAX,
	};

private:
	ioUIRenderImage *m_pIcon;
	ioUIRenderImage *m_pIconBack;

	int m_iShovelCnt;

	/*IoString m_sDesc1;
	IoString m_sDesc2;
	IoString m_sDesc3;

	ioHashString m_Text[TEXT_MAX];*/
	ioHashString m_Text[TEXT_MAX];

public:
	virtual void ParseExtraInfo( ioXMLElement &xElement );
	virtual bool IsInWndRect( const POINT &vPos );

	virtual void iwm_show();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual bool iwm_spacebar();

protected:
	virtual void OnRender();

public:
	void UpdateUI();

public:
	ExcavationShovelWnd();
	virtual ~ExcavationShovelWnd();
};