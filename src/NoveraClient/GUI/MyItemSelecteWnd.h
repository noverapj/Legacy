#pragma once

class ioUIRenderImage;
class ioUIRenderFrame;

class MyItemSelectRadioButton : public ioRadioButton 
{
protected:
	int	m_iTabType;
public:
	virtual void ParseExtraInfo( ioXMLElement &xElement );
	const int& GetTabType(){ return m_iTabType; }
public:
	MyItemSelectRadioButton();
	virtual ~MyItemSelectRadioButton();
};

//////////////////////////////////////////////////////////////////////////

class ItemSelectInfoWnd : public ioButton
{
public:
	enum 
	{
		ID_ICON_BTN			= 1,
		ID_SELECT_BTN		= 2,
	};
	enum
	{
		MAX_DESC	= 3,
	};

protected:
	bool m_bWidthCutDescArray[MAX_DESC];
	ioComplexStringPrinter m_szDescArray[MAX_DESC];
	EXTRAITEMSLOT m_ItemInfo;
	AccessorySlot m_AccessoryInfo;
	AlchemicItemInfo m_SpiritInfo;
	bool m_bSoldier;

	ioUIRenderImage *m_pEmptyImg;
	ioUIRenderFrame *m_pBlueStateFrm;

protected:	
	void ClearDesc();
	void RenderDesc( int iXPos, int iYPos );

	bool SetInfo();
	bool SetAccessoryInfo();
	bool SetSpiritInfo();

	void ProcessAccessoryDesc();

public:
	void SetBlank();
	void SetItemInfo( const EXTRAITEMSLOT& rkItem );
	void SetAccessoryInfo( AccessorySlot& rkItem );
	void SetSpiritInfo( AlchemicItemInfo& rkItem );

protected:
	virtual void OnRender();
	virtual void OnRenderAfterChild();
	virtual void OnProcess( float fTimePerSec );

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );

public:
	ItemSelectInfoWnd();
	virtual ~ItemSelectInfoWnd();
};
//////////////////////////////////////////////////////////////////////////
class MyItemSelecteWnd :public ioWnd
{
public:
	enum
	{
		ID_EXIT_BTN			= 1,
		ID_FIND_EDIT        = 2,
		ID_FIND_BTN         = 3,

		ID_ITEM_INFO_WND_1_BTN	= 201,
		ID_ITEM_INFO_WND_2_BTN	= 202,
		ID_ITEM_INFO_WND_3_BTN	= 203,
		ID_ITEM_INFO_WND_4_BTN	= 204,
		ID_ITEM_INFO_WND_5_BTN	= 205,
		ID_ITEM_INFO_WND_6_BTN	= 206,
		ID_ITEM_INFO_WND_7_BTN	= 207,
		ID_ITEM_INFO_WND_8_BTN	= 208,

		ID_VERT_SCROLL			= 1001,
	};

	enum
	{
		ID_TAB_1_BTN	 = 101, // all
		ID_TAB_2_BTN	 = 102, //
		ID_TAB_3_BTN	 = 103, //
		ID_TAB_4_BTN	 = 104, //
		ID_TAB_5_BTN	 = 105, //
	};

	enum 
	{
		MAX_TAB_BTN				= 5,

		MAX_ITEM_INFO_WND_BTN	= 8,
		PAGE_PER_ITEM_NUM		= 4,
	};

protected:
	int m_iCurPage;

	DWORD m_dwCurTabID;

	ioUIRenderFrame *m_pEditFrm;
	ioHashString m_szCurrFindKeyword;

	ExtraItemList m_vAllItemList;
	ExtraItemList m_vCurItemList;

	ioWnd*			m_pPreSelectedWnd;

public:
	void ShowItemSeletWnd( ExtraItemList vExtraitemList, ioWnd* pPreSelectedWnd );

public:
	virtual void iwm_vscroll(DWORD dwID, int curPos );
	virtual void iwm_wheel( int zDelta );
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );

protected:
	void ChangeTab( DWORD dwID, const char *szFind );
	void SetCurItemInfoVec( DWORD dwTabID, const char *szFind );
	void SetFind();
	bool IsRightTab( DWORD dwTabID, int iType );
	void UpdatePage( int iNewPage );

public:
	MyItemSelecteWnd(void);
	virtual ~MyItemSelecteWnd(void);
};
//////////////////////////////////////////////////////////////////////////
class AccessorySelectWnd :public ioWnd
{
public:
	enum
	{
		ID_EXIT_BTN				= 1,
		ID_FIND_EDIT			= 2,
		ID_FIND_BTN				= 3,

		ID_TAB_1_BTN			= 101,
		ID_TAB_2_BTN			= 102,
		ID_TAB_3_BTN			= 103,
		ID_TAB_4_BTN			= 104,

		ID_ITEM_INFO_WND_1_BTN	= 201,
		ID_ITEM_INFO_WND_2_BTN	= 202,
		ID_ITEM_INFO_WND_3_BTN	= 203,
		ID_ITEM_INFO_WND_4_BTN	= 204,
		ID_ITEM_INFO_WND_5_BTN	= 205,
		ID_ITEM_INFO_WND_6_BTN	= 206,
		ID_ITEM_INFO_WND_7_BTN	= 207,
		ID_ITEM_INFO_WND_8_BTN	= 208,

		ID_VERT_SCROLL			= 1001,
	};
	enum 
	{
		MAX_TAB_BTN				= 4,
		MAX_ITEM_INFO_WND_BTN	= 8,
		PAGE_PER_ITEM_NUM		= 4,
	};

protected:
	int m_iCurPage;

	DWORD m_dwCurTabID;

	ioUIRenderFrame *m_pEditFrm;
	ioHashString m_szCurrFindKeyword;

	ioWnd*			m_pPreSelectedWnd;

	AccessorySlotList m_vCurItemList;
	int m_iSelectIndex[4];
	bool m_bShowCompound;

public:
	void ShowSeletWnd( ioWnd* pPreSelectedWnd, int iSelectIndex1, int iSelectIndex2, int iSelectIndex3, int iSelectIndex4, bool bShowCompound );

public:
	virtual void iwm_vscroll(DWORD dwID, int curPos );
	virtual void iwm_wheel( int zDelta );
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void iwm_hide();

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );

protected:
	void ChangeTab( DWORD dwID, const char *szFind );
	void SetCurItemInfoVec( DWORD dwTabID, const char *szFind );
	void SetFind();
	bool IsRightTab( DWORD dwTabID, int iType );
	void UpdatePage( int iNewPage );

public:
	AccessorySelectWnd(void);
	virtual ~AccessorySelectWnd(void);
};

class MySpiritSelectWnd :public ioWnd
{
public:
	enum
	{
		ID_EXIT_BTN			= 1,
		ID_FIND_EDIT        = 2,
		ID_FIND_BTN         = 3,
		
		ID_TAB_BTN			= 101,

		ID_ITEM_INFO_WND_1_BTN	= 201,
		ID_ITEM_INFO_WND_2_BTN	= 202,
		ID_ITEM_INFO_WND_3_BTN	= 203,
		ID_ITEM_INFO_WND_4_BTN	= 204,
		ID_ITEM_INFO_WND_5_BTN	= 205,
		ID_ITEM_INFO_WND_6_BTN	= 206,
		ID_ITEM_INFO_WND_7_BTN	= 207,
		ID_ITEM_INFO_WND_8_BTN	= 208,

		ID_VERT_SCROLL			= 1001,
	};
	enum 
	{
		MAX_ITEM_INFO_WND_BTN	= 8,
		PAGE_PER_ITEM_NUM		= 4,
		SPIRIT_TYPE_COUNT = 5,
	};

protected:
	int m_iCurPage;
	int m_iSpiritCode;

	ioUIRenderFrame *m_pEditFrm;
	ioHashString m_szCurrFindKeyword;
	ioHashString m_szSpiritTypeName[SPIRIT_TYPE_COUNT];
	ioHashString m_szDesc;

	vAlchemicItemInfoList m_vAllItemList;
	vAlchemicItemInfoList m_vCurItemList;

	ioWnd*			m_pPreSelectedWnd;

public:
	void ShowItemSeletWnd( int iSpiritCode, ioWnd* pPreSelectedWnd );

public:
	virtual void iwm_create();
	virtual void iwm_show();
	virtual void iwm_vscroll(DWORD dwID, int curPos );
	virtual void iwm_wheel( int zDelta );
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );
	virtual void ParseExtraInfo( ioXMLElement &xElement );

protected:
	void SearchText( const char *szFind );
	void SetCurItemInfoVec( const char *szFind );
	void SetFind();
	void UpdatePage( int iNewPage );

public:
	MySpiritSelectWnd(void);
	virtual ~MySpiritSelectWnd(void);
};