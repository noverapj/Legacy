#pragma once

class GameOptionSettingKeyInput : public ioWnd
{
public:
	enum
	{
		ID_DONT_SETTING = 1,
		ID_EXIT         = 2,
		ID_USE          = 3,
		ID_CANCEL       = 4,
	};

	enum
	{
		ID_BTNUP_DONT_SETTING = 999999999,
	};

protected:
	ioUIRenderFrame *m_pTextBoxFrm;
	ioHashString     m_sCurKeyTitle;
	ioHashString     m_sExistKeyTitle;
	WORD             m_wPressKey;
    ioWnd           *m_pReturnWnd;
	
public:
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );

public:
	virtual void iwm_create();
	virtual bool iwm_esc();
	virtual bool iwm_spacebar();

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

public:
	void SetCurKeyTitle( const ioHashString &rsCurKeyTitle) { m_sCurKeyTitle = rsCurKeyTitle; }
	void SetExistKeyTitle(const ioHashString &rsExistKeyTitle) { m_sExistKeyTitle = rsExistKeyTitle; }
	void SetReturnWnd(ioWnd *pReturnWnd) { m_pReturnWnd = pReturnWnd; }

public:
	GameOptionSettingKeyInput();
	virtual ~GameOptionSettingKeyInput();
};

