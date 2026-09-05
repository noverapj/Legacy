#pragma once

#include "../CharacterInfo.h"
#include "../io3DEngine/ioStringConverter.h"
#include "../ioComplexStringPrinter.h"

#include "ManualWnd.h"
#include "MyInventoryWnd.h"

class ioUI3DRender;
class ioUIRenderFrame;
class ioCreateChar;
class ioItem;
class ioSetItemInfo;
class ioPlayStage;
class ManualMgr;

enum 
{
	TRADE_MARK_TYPE_DISABLE	= 0,
	TRADE_MARK_TYPE_WAIT	= 1,
	TRADE_MARK_TYPE_ENABLE	= 2,
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class TradeIconBtn : public ioButton
{
protected:
	ioUIRenderImage *m_pIcon;
	ioUIRenderImage *m_pIconBack;
	ioUIRenderImage *m_pCustomIcon;

	ioUIRenderImage *m_pNumText;
	ioUIRenderImage *m_pPlusText;

	ioUIRenderFrame *m_pStateOrangeFrm;
	ioUIRenderFrame *m_pStateGreenFrm;

	int m_iMarkType; //
	bool m_bCustomItem;

	int m_iNum;
	float m_fScale;
	float m_fBackScale;
	int m_nGradeType;

protected:
	void RenderIcon(int iXPos, int iYPos, UIRenderType eType);
	void RenderBack(int iXPos, int iYPos );
	void RenderOtherInfo( int iXPos, int iYPos );

public:
	bool SetIcon( ioUIRenderImage *pIcon, float fScale, float fBackScale, int iNum=0 );
	void SetMarkType( int iMarkType ) { m_iMarkType = iMarkType; } 
	void SetCustomItem( bool bCustomItem ){ m_bCustomItem = bCustomItem; }
	void SetGradeType( int nGradeType ){ m_nGradeType = nGradeType; }

public:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );

protected:
	virtual void OnDrawNormal( int iXPos, int iYPos );
	virtual void OnDrawOvered( int iXPos, int iYPos );
	virtual void OnDrawPushed( int iXPos, int iYPos );
	virtual void OnDrawDisabled( int iXPos, int iYPos );

protected:
	virtual void OnProcess( float fTimePerSec );

public:
	TradeIconBtn();
	virtual ~TradeIconBtn();
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class ChangeTradeStateConfirmWnd : public ioMovingWnd
{
public:
	enum
	{
		ID_ICON_BTN		= 1,
		ID_CHANGE_BTN	= 2,
		ID_CANCEL_BTN	= 3,
		ID_EXIT_BTN		= 4,
	};

protected:
	ioHashString m_szName;
	int m_iExtraItemIndex;

	ioUIRenderImage *m_pArrow;

	ioUIRenderFrame *m_pStateOrangeFrm;
	ioUIRenderFrame *m_pStateGreenFrm;

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnRender();
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

public:
	void SetChangeItemInfo( int iIndex );

public:
	ChangeTradeStateConfirmWnd();
	virtual ~ChangeTradeStateConfirmWnd();
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class ChangeTradeStateResultWnd : public ioWnd
{
public:
	enum
	{
		ID_ICON_BTN		= 1,
		ID_USE			= 2,
		ID_GO			= 3,
	};

protected:
	ioHashString m_szName;
	int m_iExtraItemIndex;

	ManualMgr* m_pManual;

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnRender();

public:
	void SetChangeResultInfo( int iIndex );

protected:
	void ReBatchButton();
	void PrintManual( int a_iXPos, int a_iYPos , float fScale );

public:
	ChangeTradeStateResultWnd();
	virtual ~ChangeTradeStateResultWnd();
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class TradeItemInfoWnd : public ioButton
{
public:
	enum 
	{
		ID_ICON_BTN			= 1,
		ID_ACTION_BTN		= 2,
		ID_CUSTOM_BTN		= 3,
	};

	enum
	{
		MAX_DESC			= 3,
	};

protected:
	bool m_bWidthCutDescArray[MAX_DESC];
	ioComplexStringPrinter m_szDescArray[MAX_DESC];

	InventoryItemInfo m_ItemInfo;
	ioUIRenderImage *m_pEmptyImg;

	int m_iExtraItemIndex;
	int m_nGradeType;

protected:
	void SetBlank();
	void ClearDesc();
	void RenderDesc( int iXPos, int iYPos );

	void SetIsWidthCutDesc( int iArray , bool bWidthCut );
	bool IsWidthCutDesc( int iArray );

	bool SetExtraItemInfo();

public:
	void SetInfo( InventoryItemInfo &rkInfo );

	InventoryItemInfo& GetInfo();

	inline int GetExtraItemIndex() const { return m_iExtraItemIndex; }	
	bool IsCustomToolTip();

protected:
	virtual void OnRender();

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

public:
	TradeItemInfoWnd();
	virtual ~TradeItemInfoWnd();
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class ChangeTradeStateWnd : public ioMovingWnd
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
	bool GetCurItemInfo( OUT InventoryItemInfo &rkInfo, IN DWORD dwItemInfoID );
	void SendTradeStateChange( int iIndex );

public:
	ChangeTradeStateWnd();
	virtual ~ChangeTradeStateWnd();
};
