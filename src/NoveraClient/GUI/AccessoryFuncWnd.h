#pragma once

class AccessoryFuncWnd : public ioWnd
{
public:
	enum
	{
		ID_INFO_TAB_BTN	= 1,
		ID_FUNC_TAB_BTN	= 2,
		ID_MY_SOLDIER_LIST_BTN = 3,
		ID_MY_SOLDIER_LEFT_BTN = 4,
		ID_MY_SOLDIER_RIGHT_BTN = 5,

		ID_REINFORCE_ACCESSORY_ICON = 11,
		ID_REINFORCE_MATERIAL_ICON = 12,
		ID_REINFORCE = 14,
		ID_SHOW = 15,
		ID_AGAIN = 16,
		ID_REINFORCE_ROLLING = 19,

		ID_COSTUME_SOLDIER_WND       = 103,

		ID_ACCESSORY_RING_BTN		 = 351,
		ID_ACCESSORY_NECKLACE_BTN	 = 352,
		ID_ACCESSORY_BRACELET_BTN	 = 353,
	};
	enum
	{
		eReady = 0,
		eMax,
		eNot,
		eSet,
		eSendPacket,
		eEnd,
	};
	enum
	{
		Type_Accessory = 0,
		Type_Material = 1,
	};

private:
	CHARACTER m_CharInfo;

	int m_iTabState;
	int m_iFuncState;
	ioHashString m_szReinforceStateMsg[5];
	ioHashString m_szReinforceText;
	ioHashString m_szReinforceAlarm;
	ioHashString m_szAccessoryDesc[5];
	ioHashString m_szReinforceRollingSound;
	ioHashString m_szReinforceResultSound;
	ioHashStringVec m_vRollingEffect;
	ioHashStringVec m_vEndEfffect_S;	// ¼º°ø
	ioHashString m_szReinforceResultMsg[2];
	ioHashString m_szReinforceResultDesc;
	ioHashString m_szReinforceResultText;

	ioUIRenderImage *m_pIconBack;
	ioUIRenderImage *m_pIconBG;
	ioUIRenderImage *m_pFinalBack;
	ioUIRenderImage *m_pStripDark;
	ioUIRenderImage *m_pStripLight;
	ioUIRenderImage *m_pEffect;
	ioUIRenderImage *m_pBlackStrip;

	ioUIRenderFrame *m_pItemSlot;

	AccessorySlot m_Accessory[2];
	ioHashString m_szAccessoryTimeText[2];
	ioHashString m_szAccessoryTooltip[2];
	ioHashString m_szAccessoryReinforeTimeOut;

	int m_iCurEffectArray;
	int m_iAbilityMax;
	int m_iAbilityGap;
	bool m_bAbilitySign;

	DWORD m_dwRollingTime;
	DWORD m_dwRollingEndTime;
	DWORD m_dwRollingEffectTime;
	DWORD m_dwRollingEffectCheckTime;

private:
	void SelectTab( DWORD dwID );

public:
	virtual void iwm_create();
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

	virtual void OnRender();
	virtual void OnProcess( float fTimePerSec );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );
	virtual void ParseExtraInfo( ioXMLElement &xElement );

	void SetAccessoryInfo( int iClassType );
	void SetAccessorySubInfo( DWORD dwWndID, int iSlotNum, float fScale, float fBackScale, float fDisableScale );
	void UISoldierSelectWndBtnUp( int iSoldierArray );

	const CHARACTER &GetCharInfo() const	{ return m_CharInfo; }
	int GetTabState()						{ return m_iTabState; }

	void SelectClickAction( int iIndex );
	void OnAccessoryReinforceResult( int iIndex );

	void ResetAccessoryFuncTab();

protected:
	void SetReinforceResult();
	void SendReinforce();

	void RollingEffect( float fTimePerSec );

public:
	AccessoryFuncWnd(void);
	virtual ~AccessoryFuncWnd(void);
};