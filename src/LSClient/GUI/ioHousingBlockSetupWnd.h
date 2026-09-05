#pragma once
class ioUserBlockStorage;

class ioHousingShortCutKeyButton : public ioButton
{
protected:
	ioUIRenderImage* m_pShortcutKey;

public:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	

public:
	virtual void OnDrawNormal( int iXPos, int iYPos );
	virtual void OnDrawOvered( int iXPos, int iYPos );
	virtual void OnDrawPushed( int iXPos, int iYPos );
	virtual void OnDrawDisabled( int iXPos, int iYPos );

public:
	ioHousingShortCutKeyButton();
	virtual ~ioHousingShortCutKeyButton();
};

class ioHousingIconButton : public ioButton
{
protected:
	ioUIRenderImage* m_pIcon;

public:
	virtual void OnRender();

public:
	void SetIcon( const ioHashString& szName );

public:
	ioHousingIconButton();
	virtual ~ioHousingIconButton();
};

class ioHousingBlockSetupWnd : public ioButton
{
public:
	enum
	{
		ID_EXIT_SETUP	= 1,
		ID_EXIT_REMOVE	= 2,
		ID_PREV			= 3,
		ID_NEXT			= 4,
		ID_SETUP_OK		= 5,
		ID_REMOVE_OK	= 6,
		ID_ICON			= 7,
		ID_ITEM_LIST	= 8,
		ID_SETUP_DESC	= 9,
		ID_REMOVE_DESC	= 10,
	};

protected:
	ioUIRenderImage*	m_pSetUpBack;
	ioUIRenderImage*	m_pRemoveBack;

	int					m_State;
	int					m_nCurrRemoveCode;
	int					m_nCurrSetUpIndex;

protected:
	ioHashString		m_Name;

	int					m_nInvenItemCount;
	int					m_nWidth;
	int					m_nDepth;
	int					m_nHeight;

protected:
	int					m_eBlockModeType;

public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual bool iwm_esc();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void OnProcess( float fTimePerSec );

public:
	void SetBlockModeType(int eType );

public:
	void ChangeErase( int nRemoveCode );
	void RefreshErase( int nRemoveCode );

	void ChangeSetUp( int nCurrSetUpIndex );
	void RefreshSetUp( int nCurrSetUpIndex );

public:
	ioUserBlockStorage* GetStorage();

public:
	virtual void OnRender();	
	
public:
	void OnBackRender( int nXPos, int nYPos );
	void OnBlockDescRender( int nXPos, int nYPos );
	void OnRemoveDescRender( int nXPos, int nYPos );

public:
	ioHousingBlockSetupWnd();
	virtual ~ioHousingBlockSetupWnd();
};