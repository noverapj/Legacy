#pragma once
#include "ioWnd.h"
#include "MyInventoryWnd.h"
#include "ioButton.h"

class InventorySelectItemRadioButton : public ioRadioButton 
{
protected:
	int			 m_iTabType;
public:
	virtual void ParseExtraInfo( ioXMLElement &xElement );
	const int& GetTabType(){ return m_iTabType; }
public:
	InventorySelectItemRadioButton();
	virtual ~InventorySelectItemRadioButton();
};

///////////////////////////////////////////////////////////////////////////////////
class InventorySelectItemBtn : public ioButton
{
protected:
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
	ioComplexStringPrinter m_szDescArray[MAX_DESC];
	ioUIRenderImage *m_pIconEmptyImg;
	ioUIRenderImage *m_pIconImg;

	InventorySelectItem m_ItemInfo;
	bool m_bIsCostume;

protected:
	void ClearDesc();
	bool SetInfo();
	void RenderDesc( int iXPos, int iYPos );
public:
	void SetBlank();
	void SetItemInfo( const InventorySelectItem& rkItem, int iEtcItemCount, bool bIsCostume );

	const ioHashString& GetItemName() { return m_ItemInfo.m_szName; }
protected:
	virtual void OnRender();
public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void iwm_lbuttonup( const ioMouse& mouse );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
public:
	InventorySelectItemBtn();
	virtual ~InventorySelectItemBtn();
};
///////////////////////////////////////////////////////////////////////////////////

class InventorySelectItemWnd : public ioWnd
{
protected:
	enum
	{
		ID_EXIT_BTN				= 1,
		ID_FIND_EDIT			= 2,
		ID_FIND_BTN				= 3,

		ID_TAB1_1_BTN			= 111,

		ID_TAB2_1_BTN			= 121,
		ID_TAB2_2_BTN			= 122,

		ID_TAB3_1_BTN			= 131,
		ID_TAB3_2_BTN			= 132,
		ID_TAB3_3_BTN			= 133,

		ID_TAB4_1_BTN			= 141,
		ID_TAB4_2_BTN			= 142,
		ID_TAB4_3_BTN			= 143,
		ID_TAB4_4_BTN			= 144,

		ID_TAB5_1_BTN			= 151,
		ID_TAB5_2_BTN			= 152,
		ID_TAB5_3_BTN			= 153,
		ID_TAB5_4_BTN			= 154,
		ID_TAB5_5_BTN			= 155,

		ID_TAB6_1_BTN			= 161,
		ID_TAB6_2_BTN			= 162,
		ID_TAB6_3_BTN			= 163,
		ID_TAB6_4_BTN			= 164,
		ID_TAB6_5_BTN			= 165,
		ID_TAB6_6_BTN			= 166,

		ID_ITEM_INFO_WND_1_BTN	= 201,
		ID_ITEM_INFO_WND_2_BTN	= 202,
		ID_ITEM_INFO_WND_3_BTN	= 203,
		ID_ITEM_INFO_WND_4_BTN	= 204,
		ID_ITEM_INFO_WND_5_BTN	= 205,
		ID_ITEM_INFO_WND_6_BTN	= 206,
		ID_ITEM_INFO_WND_7_BTN	= 207,
		ID_ITEM_INFO_WND_8_BTN	= 208,

		ID_TOOLTIP				= 300,

		ID_VERT_SCROLL			= 1001,
	};

	enum 
	{
		MAX_TAB_BTN				= 6,

		MAX_ITEM_INFO_WND_BTN	= 8,
		PAGE_PER_ITEM_NUM		= 4,
	};
protected:
	int							m_iCurPage;
	DWORD						m_dwCurTabID;
	DWORD						m_dwCurMinTabID;
	DWORD						m_dwCurMaxTabID;
	int							m_iMaxRadioBtn;
	ioUIRenderFrame				*m_pEditFrm;
	ioHashString				m_szCurrFindKeyword;
	vInventorySelectItemList	m_vAllItemList;
	vInventorySelectItemList	m_vCurItemList;
	ioWnd*						m_pPreSelectedWnd;
	int							m_iEtcItemCount;
	bool						m_bIsCostume;

public:
	void ShowInventorySelectItemWnd( vInventorySelectItemList vitemList, ioWnd* pPreSelectedWnd, ioHashStringVec vRadioBtnNameList, ioHashString szTitleName, int iEtcItemCount, bool bIsCostume = false );

protected:
	void ChangeTab( DWORD dwID, const char *szFind );
	void SetRadioBtn( const ioHashStringVec& vRadioBtnNameList );
	void SetCurItemInfoVec( DWORD dwTabID, const char *szFind );
	void SetFind();
	bool IsRightTab( DWORD dwTabID, int iType );
	void UpdatePage( int iNewPage );
public:
	virtual void iwm_vscroll(DWORD dwID, int curPos );
	virtual void iwm_wheel( int zDelta );
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void iwm_hide();
protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );
public:
	InventorySelectItemWnd(void);
	virtual ~InventorySelectItemWnd(void);
};

///////////////////////////////////////////////////////////////////
class InventorySelectItemToolTip : public ioWnd
{
protected:
	enum
	{
		MIN_WIDTH		= 0,
		MIN_HEIGHT		= 26,
		TEXT_LINE_GAP   = 18,
		SIDE_TEXT_GAP	= 14,
	};
	ioComplexStringPrinter m_szName;
	ioHashString		   m_szOverSnd;
public:
	bool		 SetInfo( const ioHashString& szName );
public:
	virtual void ParseExtraInfo( ioXMLElement &xElement );
protected:
	virtual void OnRender();
	virtual void OnProcess( float fTimePerSec );
public:
	InventorySelectItemToolTip();
	virtual ~InventorySelectItemToolTip();
};
///////////////////////////////////////////////////////////////////