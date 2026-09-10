#pragma once

#include "ioSprite.h"

class ItemIconButton : public ioButton
{
protected:
	ioUIRenderImage *m_pIcon;
	ioUIRenderImage *m_pIconBack;

	ioUIRenderImage *m_pNumText;
	ioUIRenderImage *m_pPlusText;

	ioUIRenderFrame *m_pStateBlueFrm;
	bool m_bEquip;
	bool m_bRenderGray;

	int m_iNum;
	int m_nGradeType;

protected:
	void RenderIcon(int iXPos, int iYPos, UIRenderType eType);
	void RenderBack(int iXPos, int iYPos );
	void RenderOtherInfo( int iXPos, int iYPos );

public:
	bool SetIcon( ioUIRenderImage *pIcon, int iNum=0 );
	void SetEquip( bool bEquip ){ m_bEquip = bEquip; }
	void SetGradeType( int nGradeType ){ m_nGradeType = nGradeType; }
	void SetIconGray( bool bGray )	{ m_bRenderGray = bGray; }

public:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );

protected:
	virtual void OnDrawNormal( int iXPos, int iYPos );
	virtual void OnDrawOvered( int iXPos, int iYPos );
	virtual void OnDrawPushed( int iXPos, int iYPos );
	virtual void OnDrawDisabled( int iXPos, int iYPos );

public:
	ItemIconButton();
	virtual ~ItemIconButton();
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ItemInfoButton : public ioButton
{
public:
	enum 
	{
		ID_ICON_BTN	= 1,
	};

	enum
	{
		MAX_DESC = 3,
	};

protected:
	ioComplexStringPrinter m_szDescArray[MAX_DESC];

	InventoryItemInfo m_ItemInfo;
	ioUIRenderImage *m_pEmptyImg;

	int m_iExtraItemIndex;

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	void SetBlank();
	void ClearDesc();
	void RenderDesc( int iXPos, int iYPos );

	bool SetExtraItemInfo();

public:
	void SetInfo( InventoryItemInfo &rkInfo );

	InventoryItemInfo& GetInfo();

	inline int GetExtraItemIndex() const { return m_iExtraItemIndex; }
	inline int GetMagicCode() const { return m_ItemInfo.m_iMagicCode; }
	bool IsIconOver();

protected:
	virtual void OnRender();

public:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

public:
	ItemInfoButton();
	virtual ~ItemInfoButton();
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ItemRechargeWnd : public ioMovingWnd
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
		ID_TOOL_TIP             = 1002,
	};

	enum 
	{
		MAX_TAB_BTN				= 5,

		MAX_ITEM_INFO_WND_BTN	= 8,
		PAGE_PER_ITEM_NUM		= 4,
	};

	enum ItemType
	{
		IT_NONE,

		IT_NORMAL_WEAPON,
		IT_NORMAL_ARMOR,
		IT_NORMAL_HELMET,
		IT_NORMAL_CLOAK,

		IT_EXTRA_WEAPON,
		IT_EXTRA_ARMOR,
		IT_EXTRA_HELMET,
		IT_EXTRA_CLOAK,

		IT_RARE_WEAPON,
		IT_RARE_ARMOR,
		IT_RARE_HELMET,
		IT_RARE_CLOAK,
	};

protected:
	vInventoryItemInfoList m_vInventoryItemInfoList;

	int m_iCurPage;
	DWORD m_dwCurTabID;

	DWORD m_dwEtcItemIndex;

public:
	virtual void iwm_create();
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

	virtual void iwm_vscroll(DWORD dwID, int curPos );
	virtual void iwm_wheel( int zDelta );

protected:
	virtual void OnProcess( float fTimePerSec );
	void ProcessCustomBtnTooltip();

protected:
	void UpdatePage( int iNewPage );

	void SetExtraItemInfoVec( DWORD dwTabID );
	bool IsRightTab( DWORD dwTabID, int iValue1 );
	int  GetTabType( DWORD dwTabID );
	ItemType GetItemType( int iItemCode );

public:
	void ChangeTab( DWORD dwID = 0 );
	void Update();
	void SetInfo( DWORD dwEtcItemIndex );

	void SendItemRecharge( int iItemIndex );

public:
	ItemRechargeWnd();
	virtual ~ItemRechargeWnd();
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ItemRechargeProcessWnd : public ioWnd
{
protected:
	typedef std::vector<ioUIRenderImage*> ioUIRenderImageList;

protected:
	enum
	{
		ID_EXIT_BTN      = 1,
		ID_OK_BTN        = 2,
		ID_CANCEL_BTN    = 3,
		ID_PROCESS_BTN   = 4,
		ID_AGAIN_BTN     = 5,
		ID_ITEM_LIST_BTN1= 6,
		ID_ITEM_LIST_BTN2= 7,
	};

	enum UIState
	{
		US_START,
		US_PROCESS,
		US_END,
	};

protected:
	ioWnd           *m_pReturnWnd;

	ioUIRenderImage *m_pIcon;
	ioUIRenderImage *m_pNumText;
	ioUIRenderImage *m_pPlusText;
	ioUIRenderImage *m_vRollingEffect[9];
	ioUIRenderImage *m_vStopEffect[10];
	ioUIRenderImage *m_vSuccessEffect[10];
	ioUIRenderImage *m_pIconBackProcess;
	ioUIRenderImage *m_pIconBackResult;

	ioSprite m_RollingEffect;
	ioSprite m_StopEffect;
	ioSprite m_SuccessEffect;

	ioHashString m_ItemName;
	int          m_iReinForce;
	int          m_iItemCode;
	int          m_iItemIndex;

	UIState      m_UIState;

	int m_iRechargeHour;
	ioHashString m_RechargeTime;

	DWORD m_dwProcessTime;
	DWORD m_dwProcessEndTime;

	ioHashStringVec m_RollingText;
	DWORD m_dwRollingCheckTime;
	DWORD m_dwNextRollingTime;
	int m_iCurRoll;
	int m_iMaxRoll;

	DWORD m_dwRollingEffectGapTime;
	DWORD m_dwStopEffectGapTime;

	DWORD m_dwUseEtcItemIndex;

	ioHashString m_RollingSound;
	ioHashString m_StopSound;
	ioHashString m_SuccessSound;

	int m_nGradeType;

public:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void ParseExtraInfo( ioXMLElement &xElement );

public:
	virtual void iwm_create();
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnRender();
	virtual void OnProcess( float fTimePerSec );

protected:
	void RenderIcon();

public:
	void SetInfo( ioWnd *pReturnWnd, int iItemCode, int iItemIndex, DWORD dwUseEtcItemIndex );

public:
	void SetUIStartState();
	void SetUIProcessState();
	void SetUIEndState( int iRechargeHour );

public:
	ItemRechargeProcessWnd();
	virtual ~ItemRechargeProcessWnd();
};

//////////////////////////////////////////////////////////////////////////

#define  GLOBAL_TEXT_TOOLTIP_LINE 5
class RechargeItemTooltip : public ioWnd
{
public:
	enum
	{
		MIN_WIDTH		= 0,
		MIN_HEIGHT		= 26,
		TEXT_LINE_GAP   = 18,
		SIDE_TEXT_GAP	= 14,

	};

protected:
	ioComplexStringPrinter m_Printer[GLOBAL_TEXT_TOOLTIP_LINE];
	IoString m_Desc;
	int m_iLine;

protected:
	void ProcessPosition();
	void ClearList();

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	void SetTooltipText( ioComplexStringPrinter *rkPrinter );
	void SetTooltipText( ioComplexStringPrinter *rkPrinter, ioHashString szTooltip, int iLine );

public:
	RechargeItemTooltip();
	virtual ~RechargeItemTooltip();
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//    Accessory recharger
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class AccessoryRechargeProcessWnd : public ioWnd
{
protected:
	typedef std::vector<ioUIRenderImage*> ioUIRenderImageList;

protected:
	enum
	{
		ID_EXIT_BTN      = 1,
		ID_OK_BTN        = 2,
		ID_CANCEL_BTN    = 3,
		ID_PROCESS_BTN   = 4,
		ID_AGAIN_BTN     = 5,
		ID_ITEM_LIST_BTN1= 6,
		ID_ITEM_LIST_BTN2= 7,
	};

	enum UIState
	{
		US_START,
		US_PROCESS,
		US_END,
	};

protected:
	ioWnd           *m_pReturnWnd;

	ioUIRenderImage *m_pIcon;
	ioUIRenderImage *m_pNumText;
	ioUIRenderImage *m_pPlusText;
	ioUIRenderImage *m_vRollingEffect[9];
	ioUIRenderImage *m_vStopEffect[10];
	ioUIRenderImage *m_vSuccessEffect[10];
	ioUIRenderImage *m_pIconBackProcess;
	ioUIRenderImage *m_pIconBackResult;

	ioSprite m_RollingEffect;
	ioSprite m_StopEffect;
	ioSprite m_SuccessEffect;

	ioHashString m_ItemName;
	int          m_iItemCode;
	int          m_iItemIndex;

	UIState      m_UIState;

	int m_iRechargeHour;
	ioHashString m_RechargeTime;

	DWORD m_dwProcessTime;
	DWORD m_dwProcessEndTime;

	ioHashStringVec m_RollingText;
	DWORD m_dwRollingCheckTime;
	DWORD m_dwNextRollingTime;
	int m_iCurRoll;
	int m_iMaxRoll;

	DWORD m_dwRollingEffectGapTime;
	DWORD m_dwStopEffectGapTime;

	DWORD m_dwUseEtcItemIndex;

	ioHashString m_RollingSound;
	ioHashString m_StopSound;
	ioHashString m_SuccessSound;

public:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void ParseExtraInfo( ioXMLElement &xElement );

public:
	virtual void iwm_create();
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnRender();
	virtual void OnProcess( float fTimePerSec );

protected:
	void RenderIcon();

public:
	void SetInfo( ioWnd *pReturnWnd, int iItemCode, int iItemIndex, DWORD dwUseEtcItemIndex );

public:
	void SetUIStartState();
	void SetUIProcessState();
	void SetUIEndState( int iRechargeHour );

public:
	AccessoryRechargeProcessWnd();
	virtual ~AccessoryRechargeProcessWnd();
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class AccessoryInfoButton : public ioButton
{
public:
	enum 
	{
		ID_ICON_BTN	= 1,
	};

	enum
	{
		MAX_DESC = 3,
	};

protected:
	ioComplexStringPrinter m_szDescArray[MAX_DESC];

	InventoryItemInfo m_ItemInfo;
	ioUIRenderImage *m_pEmptyImg;

	int m_nAccItem;

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	void SetBlank();
	void ClearDesc();
	void RenderDesc( int iXPos, int iYPos );

	bool SetAccessoryInfo();

public:
	void SetInfo( InventoryItemInfo &rkInfo );

	InventoryItemInfo& GetInfo();

	inline int GetAccIndex() const { return m_nAccItem; }
	inline int GetMagicCode() const { return m_ItemInfo.m_iMagicCode; }
	bool IsIconOver();

protected:
	virtual void OnRender();

public:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

public:
	AccessoryInfoButton();
	virtual ~AccessoryInfoButton();
};


class AccessoryRechargeWnd : public ioMovingWnd
{
public:
	enum
	{
		ID_EXIT_BTN			= 1,

		ID_TAB_1_BTN	= 101, // extra : all
		ID_TAB_2_BTN	= 102, // extra : ring
		ID_TAB_3_BTN	= 103, // extra : necklace
		ID_TAB_4_BTN	= 104, // extra : earring

		ID_ITEM_INFO_WND_1_BTN	= 201,
		ID_ITEM_INFO_WND_2_BTN	= 202,
		ID_ITEM_INFO_WND_3_BTN	= 203,
		ID_ITEM_INFO_WND_4_BTN	= 204,
		ID_ITEM_INFO_WND_5_BTN	= 205,
		ID_ITEM_INFO_WND_6_BTN	= 206,
		ID_ITEM_INFO_WND_7_BTN	= 207,
		ID_ITEM_INFO_WND_8_BTN	= 208,

		ID_VERT_SCROLL			= 2001,
		ID_TOOL_TIP             = 1002,
	};


	enum AccessoryType
	{
		ACC_NONE = 0,

		ACC_RING,
		ACC_NECKLACE,
		ACC_EARRING,
	};
	

	enum 
	{
		MAX_TAB_BTN				= 4,

		MAX_ITEM_INFO_WND_BTN	= 8,
		PAGE_PER_ITEM_NUM		= 4,
	};


protected:
	vInventoryItemInfoList m_vInventoryItemInfoList;

	int m_iCurPage;
	DWORD m_dwCurTabID;

	DWORD m_dwEtcItemIndex;

public:
	virtual void iwm_create();
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

	virtual void iwm_vscroll(DWORD dwID, int curPos );
	virtual void iwm_wheel( int zDelta );

protected:
	virtual void OnProcess( float fTimePerSec );
	void ProcessCustomBtnTooltip();

protected:
	void UpdatePage( int iNewPage );

	void SetAccessoryInfoVec( DWORD dwTabID );
	bool IsRightTab( DWORD dwTabID, int iValue1 );
	int  GetTabType( DWORD dwTabID );
	AccessoryType GetItemType( int iItemCode );

public:
	void ChangeTab( DWORD dwID = 0 );
	void Update();
	void SetInfo( DWORD dwEtcItemIndex );

	void SendItemRecharge( int iItemIndex );

public:
	AccessoryRechargeWnd();
	virtual ~AccessoryRechargeWnd();
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

