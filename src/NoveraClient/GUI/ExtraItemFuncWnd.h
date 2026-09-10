#pragma once

#include "ItemReinforceDownWnd.h"

class ExtraItemFuncWnd : public ioWnd
{
public:
	enum
	{
		ID_INFO_TAB_BTN				= 1,
		ID_FUNC_TAB_BTN				= 2,
		ID_MY_SOLDIER_LIST_BTN		= 3,
		ID_MY_SOLDIER_LEFT_BTN		= 4,
		ID_MY_SOLDIER_RIGHT_BTN		= 5,
		
		ID_WEAPON_ITEM_BTN		    = 21,
		ID_ARMOR_ITEM_BTN		    = 22,
		ID_HELMET_ITEM_BTN		    = 23,
		ID_CLOAK_ITEM_BTN		    = 24,
		
		ID_MEDAL_ITEM_1_BTN         = 25,
		ID_MEDAL_ITEM_2_BTN         = 26,
		ID_MEDAL_ITEM_3_BTN         = 27,
		ID_MEDAL_ITEM_4_BTN         = 28,
		ID_MEDAL_ITEM_5_BTN         = 29,
		ID_MEDAL_ITEM_6_BTN         = 30,

		ID_REINFORCE_BTN			= 31,
		ID_REINFORCE_ROLLING		= 32,
		ID_TARGET_ICON				= 33,
		ID_MATERIALITEM_BTN			= 34,
		ID_MATERIALITEM_WND			= 35,
		ID_MATERIALITEM_ICON		= 36,	
		ID_FAIL_MANUAL_BTN			= 37,
		ID_FAIL_TOOL_TIP			= 38,
		ID_USE						= 39,
		ID_GO						= 40,

		ID_SOLDIER_WND              = 102,

		ID_WEAPON_COSTUME_SMALL_BTN	= 251,
		ID_ARMOR_COSTUME_SMALL_BTN	= 252,
		ID_HELMET_COSTUME_SMALL_BTN	= 253,
		ID_CLOAK_COSTUM_SMALL_BTN	= 254,
	};
	enum
	{
		eReady = 0,
		eSendPacket,
		eEnd,
	};

private:
	int m_iClassType;
	CHARACTER m_CharInfo;

	int m_iTabState;
	int m_iFuncState;
	ioHashString m_szErrMsg[9];
	ioHashString m_szInfoText[14];

	ioUIRenderImage *m_pExtraSubBack;
	ioUIRenderImage *m_pManualMark;
	ioUIRenderImage *m_pGaugeMainBack;
	ioUIRenderImage *m_pGaugeBack;
	ioUIRenderImage *m_pGauge;
	ioUIRenderImage *m_pNewGauge;
	ioUIRenderImage *m_pMagicCircle;
	ioUIRenderImage *m_pItemBack;
	ioUIRenderImage *m_pStripDark;
	ioUIRenderImage *m_pEffect;
	ioUIRenderImage *m_pBackEffect;
	ioUIRenderImage *m_pFinalBackEffect;

	ManualMgr* m_pManual;

	DWORD m_dwMagicEffectCreateTime;
	DWORD m_dwRollingEndTime;
	DWORD m_dwRollingTotalTime;
	DWORD m_dwRollingEffectCheckTime;
	DWORD m_dwRollingEffectTime;
	DWORD m_dwScaleStartTime;
	DWORD m_dwIconScaleTime;

	vMagicEffectInfo m_vMagicEffectInfoList;
	InvenItemNewInfoList m_vTotalItemList;

	int m_iSelectExtraItemIndex;
	int m_nResultReinforce;
	int m_nPreReinforce;
	int m_nReUseSlot;
	int m_nMaxFailExp;
	int m_nCurFailExp;
	int m_nCurReinforce;
	int m_nExtraSubManualIdx;
	int m_nHelpTipIdx;
	int m_nItemCode;
	int m_iCurEffectArray;
	bool m_bIsSelectMaterial;
	bool m_bIsSelectItem;
	bool m_bSelectAdditive;
	bool m_bSuccess;
	float m_fSuccessRate;	
	float m_fFailExpRate;
	float m_fNewFailExpRate;
	float m_fIconScale;

	ioHashString m_ItemName;
	ioHashString m_szSelectItemMsg;
	ioHashString m_szLackMaterialMsg;
	ioHashString m_szExtraSubManual;
	ioHashString m_szRollingSound;
	ioHashString m_szSuccessSound;
	ioHashString m_szExtraSuccessSound;
	ioHashString m_szFailSound;
	ioHashString m_szBtnText;

	ioHashStringVec m_vRollingEffect;
	ioHashStringVec m_vExtraRollingEffect;
	ioHashStringVec m_vEndEfffect_S;	
	ioHashStringVec m_vEndEfffect_ExtraS;
	ioHashStringVec m_vEndEfffect_F;	

	DWORD m_dwExpandMedalSlotItemType;

public:
	void SetExtraItemInfo( int iClassType );
	void SetCostumeSubInfo( DWORD dwWndID, int nSlotNum, float fScale, float fBackScale, float fDisableScale );
	void SelectClickAction( int iIndex );
	void SetReinforceResultInfo( bool bSuccess, int nTargetSlot, int nReinforce );
	void SetFailExp( int nPreFailExp, int nCurFailExp );

	const CHARACTER &GetCharInfo() const	{ return m_CharInfo; }
	int GetTabState()						{ return m_iTabState; }
	bool GetCurAlchemicItemInfo( OUT AlchemicToolTipInfo &rkInfo, IN DWORD dwItemInfoID );

	void SetExpandMedalSlotItemData( DWORD dwItemIdx );

	// 2019-04-30 
	void SetRollingStopSound(); 

protected:
	void InitData();
	void SelectExpandMedalWnd( DWORD dwId );
	void UISoldierSelectWndBtnUp( int iSoldierArray );
	void SelectTab( DWORD dwID, bool bReUse = false );
	void SetMaterialCode( int nMaterialCode );

	void CreateMagicCircle();
	void ProcessMagicCircle();
	void OnRenderMagicCircle();

	void SuccessRateDescRender();
	void RenderReadyFailExp( int iXPos, int iYPos );
	void RenderResultFailExp( int iXPos, int iYPos );
	void PrintManual( int a_iXPos, int a_iYPos , float fScale );

	void LoadTotalItemList();
	void UpdateItemData( bool bReUse = false );	
	void TargetItemInit();
	bool SlotReload();
	void GetTargetList( vNewShopPullDownItem &rkList );
	void GetMaterialList( vNewShopPullDownItem &rkList );
	void MaterialListBtnUp( ioWnd *pOpenBtn, int iListArray );
	void ApplySuccessRate();

	void SendItemReinforce();
	void UpdateReinforceInfo();
	void RollingEffect( float fTimePerSec );

public:
	virtual void iwm_create();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void iwm_show();
	virtual	void iwm_hide();

	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void ParseExtraInfo( ioXMLElement &xElement );

	virtual void OnRender();
	virtual void OnProcess( float fTimePerSec );

public:
	ExtraItemFuncWnd(void);
	virtual ~ExtraItemFuncWnd(void);
};