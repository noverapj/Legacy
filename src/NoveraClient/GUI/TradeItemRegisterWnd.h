#pragma once

#include "../io3DEngine/ioStringConverter.h"
#include "../ioComplexStringPrinter.h"

#include "MyInventoryWnd.h"
#include "ChangeTradeStateWnd.h"


//////////////////////////////////////////////////////////////////////////////

class TradeItemRegisterConfirmWnd : public ioMovingWnd
{
public:
	enum
	{
		ID_ICON_BTN		= 1,
		ID_REGISTER_BTN	= 2,
		ID_CANCEL_BTN	= 3,
		ID_EXIT_BTN		= 4,
		ID_PRICE_EDIT	= 5,

		MAX_DESC		= 3,
	};

protected:
	int m_iExtraItemIndex;
	__int64 m_iPrice;
	__int64 m_iTexPrice;
	float	m_fTexRate;

	ioComplexStringPrinter m_szDescArray[MAX_DESC];

	ioEdit *m_pPreEdit;
	ioUIRenderFrame *m_pEditFrm;
	ioUIRenderFrame *m_pTexFrm;

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnRender();
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

protected:
	void ClearDesc();

	void SetPriceEditFocus();

public:
	void SetRegisterItemInfo( int iIndex );

public:
	TradeItemRegisterConfirmWnd();
	virtual ~TradeItemRegisterConfirmWnd();
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class TradeItemRegisterResultWnd : public ioMovingWnd
{
public:
	enum
	{
		ID_ICON_BTN	= 1,
		ID_OK_BTN	= 2,
		ID_EXIT_BTN	= 3,
	};

protected:
	ioHashString m_szName;
	__int64 m_iPrice;

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnRender();

	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

public:
	void SetRegisterResultInfo( DWORD dwItemType, DWORD dwMagicCode, DWORD dwValue, DWORD dwMaleCustom, DWORD dwFemaleCustom, __int64 iPrice );

public:
	TradeItemRegisterResultWnd();
	virtual ~TradeItemRegisterResultWnd();
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class TradeItemRegisterWnd : public ioMovingWnd
{
public:
	enum
	{
		ID_EXIT_BTN			= 1,

		ID_TAB_1_BTN	= 101, // extra : all
		ID_TAB_2_BTN	= 102, // extra : weapon
		ID_TAB_3_BTN	= 103, // extra : armor
		ID_TAB_4_BTN	= 104, // extra : helmet
		ID_TAB_5_BTN	= 105, // extra : cloak

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
		MAX_TAB_BTN				= 5,

		MAX_ITEM_INFO_WND_BTN	= 8,
		PAGE_PER_ITEM_NUM		= 4,
	};

protected:
	vInventoryItemInfoList m_vInventoryItemInfoList;

	int m_iCurPage;
	DWORD m_dwCurTabID;

public:
	virtual void iwm_create();
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

	virtual void iwm_vscroll(DWORD dwID, int curPos );
	virtual void iwm_wheel( int zDelta );

protected:
	virtual void OnRender();
	virtual void OnProcess( float fTimePerSec );
	void ProcessCustomBtnTooltip();

protected:
	void ChangeTab( DWORD dwID );
	void UpdatePage( int iNewPage );

	void SetExtraItemInfoVec( DWORD dwTabID );
	bool IsRightTab( DWORD dwTabID, int iValue1 );
	int  GetTabType( DWORD dwTabID );

public:
	void RegisterWndShow();
	bool GetCurItemInfo( OUT InventoryItemInfo &rkInfo, IN DWORD dwItemInfoID );

	void SendTradeItemRegister( int iIndex, __int64 iPrice );

public:
	TradeItemRegisterWnd();
	virtual ~TradeItemRegisterWnd();
};

