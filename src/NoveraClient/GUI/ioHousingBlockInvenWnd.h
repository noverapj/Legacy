#pragma once

class ioHousingBlockInvenIcon : public ioButton
{
protected:
	ioUIRenderImage* m_pIconBack;
	ioUIRenderImage* m_pIcon;

public:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	

public:
	virtual void OnDrawNormal( int iXPos, int iYPos );
	virtual void OnDrawOvered( int iXPos, int iYPos );
	virtual void OnDrawPushed( int iXPos, int iYPos );
	virtual void OnDrawDisabled( int iXPos, int iYPos );

public:
	void SetIcon( const ioHashString& szName );

public:
	ioHousingBlockInvenIcon();
	virtual ~ioHousingBlockInvenIcon();
};

//---------------------------------------------------------------------------------------------------

class ioHousingBlockInvenItemButton : public ioButton
{
protected:
	ioHashString	m_szName;
	ioHashString	m_szIcon;
	int				m_nBlockCode;
	int				m_nEtcCode;
	int				m_nCount;
	bool			m_bGray;

public:
	enum
	{
		ID_ICON = 1,
	};

	virtual void iwm_show();
	virtual void iwm_hide();	
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

	virtual void OnProcess( float fTimePerSec );

public:
	void SetInfo( const ioHashString& szName, const ioHashString& szIcon, int nBlockCode, int nEtcCode, int nCount );

public:
	bool IsOver( ioLSMouse *pMouse );

public:
	inline int GetCode(){ return m_nBlockCode; }
	inline int GetEtcCode(){ return m_nEtcCode; }
	inline int GetCount(){ return m_nCount; }

public:
	virtual void OnRender();	

public:
	ioHousingBlockInvenItemButton();
	virtual ~ioHousingBlockInvenItemButton();
};

//---------------------------------------------------------------------------------------------------

class ioHousingBlockInvenWnd : public ioWnd
{
public:
	enum
	{
		ID_EXIT		= 1,
		ID_SHOP		= 2,
		ID_EDITBOX	= 3,
		ID_EDIT_OK	= 4,

		ID_TAB01	= 11,
		ID_TAB02	= 12,
		ID_TAB03	= 13,
		ID_TAB04	= 14,
		ID_TAB05	= 15,

		ID_ITEM101	= 101,
		ID_ITEM102	= 102,
		ID_ITEM103	= 103,
		ID_ITEM104	= 104,
		ID_ITEM105	= 105,
		ID_ITEM106	= 106,
		ID_ITEM107	= 107,

		ID_ITEM108	= 108,
		ID_ITEM109	= 109,
		ID_ITEM110	= 110,
		ID_ITEM111	= 111,
		ID_ITEM112	= 112,
		ID_ITEM113	= 113,
		ID_ITEM114	= 114,

		ID_ITEM115	= 115,
		ID_ITEM116	= 116,
		ID_ITEM117	= 117,
		ID_ITEM118	= 118,
		ID_ITEM119	= 119,
		ID_ITEM120	= 120,
		ID_ITEM121	= 121,

		ID_ITEM_MAX	= 122,

		ID_SCROLL	= 1000,
		ID_TOOLTIP	= 2000,
	};
	
	enum
	{
		PAGE_ELEMENT_COUNT = 21,
	};

protected:
	int				m_nCurrPage;
	int				m_eCategory;
	int				m_eBlockModeType;
	int				m_nCurrnItemMax;

	DWORD			m_dwCurrTab;

	ioEdit*			m_pPreEdit;
	ioHashString	m_KeyWord;


public:
	virtual void iwm_show();
	virtual void iwm_hide();

	virtual bool iwm_esc();

	virtual void iwm_wheel( int zDelta );
	virtual void iwm_vscroll( DWORD dwID, int curPos );
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

	virtual void OnProcess( float fTimePerSec );

public:
	void ParseItem();
	void ParseButton();
	void ParseScroll();

	void ProcessToolTip();
	void ProcessEdit();

	void UpdateItem();

	void EnableEditFocus();
	void DisableEditFocus();
	bool IsKeyFocus();

public:
	ioUserBlockStorage* GetStorage();

public:
	void UpdateTab( DWORD dwID );

public:
	int	GetMaxPage();

public:
	void SetBlockModeType(int eType ){ m_eBlockModeType = eType; }

public:
	virtual void OnRender();	

public:
	ioHousingBlockInvenWnd();
	virtual ~ioHousingBlockInvenWnd();
};

//---------------------------------------------------------------------------------------------------

class ioHousingBlockInvenToolTip : public ioWnd
{
public:
	enum
	{
		ID_ICON = 1,
	};

protected:
	ioHashString	m_Name;
	ioHashString	m_Count;
	int				m_nManualIndex;
	int				m_nBlockCode;

public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void OnProcess( float fTimePerSec );

public:
	void SetBlockInvenItemInfo( int nBlockCode );
	void UpdateWndSize();

public:
	virtual void OnRender();	
	
public:
	void RenderDesc( int iXPos, int iYPos );
	void PrintManual( int iXPos, int iYPos, float fScale );

public:
	ioHousingBlockInvenToolTip();
	virtual ~ioHousingBlockInvenToolTip();
};