#pragma once

class BankruptcyCharWnd : public ioWnd
{
public:
	enum 
	{	
		ID_RECEIVE_CHAR_BTN = 1,
	};

protected:
	ioUIRenderImage *m_pBackGround;
	ioUIRenderImage *m_pSoldierIcon;
	ioUIRenderImage *m_pSoldierSubIcon;

protected:
	static int m_iClassSelect;
	
protected:
	// Help
	ioHashString    m_szClassName;
	ioHashStringVec m_szHelpVec;

protected:
	void UpdateFirstClass();

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	

public:
	virtual void iwm_show();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnRender();

public:
	BankruptcyCharWnd();
	virtual ~BankruptcyCharWnd();
};
