#pragma once

//////////////////////////////////////////////////////////////////////////
class NewShopPullDownItemSort : public std::binary_function< const NewShopPullDownItem& , const NewShopPullDownItem&, bool >
{
public:
	bool operator() ( const NewShopPullDownItem &lhs, const NewShopPullDownItem &rhs ) const
	{
		if( lhs.m_iMagicCode < rhs.m_iMagicCode )
			return true;

		return false;
	}
};

class SlotIconBtn : public ioButton
{
protected:
	ioUIRenderFrame *m_pStatePinkFrm;

	ioUIRenderImage *m_pIcon;
	ioUIRenderImage *m_pSubIcon;
	ioUIRenderImage *m_pIconBack;
	ioUIRenderImage *m_pIconDisable;
	ioUIRenderImage *m_pIconLock;
	ioUIRenderImage *m_pIconLockOver;
	ioUIRenderImage *m_pIconEffect;
	ioUIRenderImage *m_pCustomIcon;
	ioUIRenderImage *m_pCostumeMark;

	ioUIRenderImage *m_pNumText;
	ioUIRenderImage *m_pPlusText;

	int m_iNum;
	int m_iMagicCode;	// extra item : slot index, medal item : item type, rental item : -slot index
	int m_iMagicCodeEx;	// rental item : extra item code - tooltip
	int m_iMagicCodeEx2;// rental item : reinforce - tooltip
	int m_iPeriodType;
	DWORD m_dwMaleCustom;
	DWORD m_dwFemaleCustom;
	
	float m_fScale;
	float m_fBackScale;
	float m_fDisableScale;

	bool m_bLock;
	bool m_bIsCostume;
	bool m_bRenderGray;

	int m_nGradeType;

protected:
	void RenderIcon(int iXPos, int iYPos, UIRenderType eType);
	void RenderBack(int iXPos, int iYPos );
	void RenderIconDisable( int iXPos, int iYPos );
	void RenderIconEffect( int iXPos, int iYPos );
	void RenderNumInfo( int iXPos, int iYPos );

public:
	bool SetIcon( ioUIRenderImage *pIcon, float fScale, float fBackScale, float fDisableScale, int iPeriodType = 0, int iNum=0, DWORD dwMaleCustom = 0, DWORD dwFemaleCustom = 0, bool bIsCostume = false );
	void SetSubIcon( ioUIRenderImage *pIcon );
	void SetMagicCode(int iMagicCode) { m_iMagicCode = iMagicCode; }
	void SetMagicCodeEx(int iMagicCodeEx ){ m_iMagicCodeEx = iMagicCodeEx; }
	void SetMagicCodeEx2(int iMagicCodeEx2 ){ m_iMagicCodeEx2 = iMagicCodeEx2; }

	int GetMagicCode() const { return m_iMagicCode; }
	int GetMagicCodeEx() const { return m_iMagicCodeEx; }
	int GetMagicCodeEx2() const { return m_iMagicCodeEx2; }

	void SetLockIcon( bool bLock ) { m_bLock = bLock; }
	bool IsLockIcon() const { return m_bLock; }

	void SetGradeType( int nGradeType ) { m_nGradeType = nGradeType; }
	void SetIconGray( bool bGray )	{ m_bRenderGray = bGray; }

public:
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	

protected:
	virtual void OnProcess( float fTimePerSec );

protected:
	virtual void OnDrawNormal( int iXPos, int iYPos );
	virtual void OnDrawOvered( int iXPos, int iYPos );
	virtual void OnDrawPushed( int iXPos, int iYPos );
	virtual void OnDrawDisabled( int iXPos, int iYPos );

public:
	SlotIconBtn();
	virtual ~SlotIconBtn();
};
//////////////////////////////////////////////////////////////////////////
#define MY_INVENTORY_INFO_EXTRA_SCALE   0.85f
#define MY_INVENTORY_INFO_ETC_SCALE     FLOAT1
#define MY_INVENTORY_INFO_BACK_SCALE    FLOAT1
#define MY_INVENTORY_INFO_DISABLE_SCALE 0.85f
#define MY_INVENTORY_INFO_LOCK_SCALE    0.85f
#define MY_INVENTORY_INFO_SMALL_COSTUME_SCALE		  0.4f
#define MY_INVENTORY_INFO_SMALL_COSTUME_BACK_SCALE    FLOAT1
#define MY_INVENTORY_INFO_SMALL_COSTUME_DISABLE_SCALE 0.4f

class MyInventoryInfoWnd : public ioWnd, public PullDownEventListener
{
public:
	enum 
	{
		ID_HIRE_BTN				     = 1,
		ID_DISMISSAL_BTN		     = 2,
		ID_MY_SOLDIER_LIST_BTN	     = 3,
		ID_ETC_SHOP_BTN			     = 4,
		ID_ITEM_SELL_BTN		     = 5,
		ID_ETC_USE_BTN			     = 6,
		ID_ITEM_ICON			     = 7,
		ID_ITEM_RECV			     = 8,
		ID_ETC_PRESENT_SEND_BTN	     = 9,	// 현재 사용안함. 향후 선물보내기를 위해 남겨둠.
		ID_ETC_USE_SMALL_BTN	     = 10,
		ID_DISASSEMBLE_BTN		     = 11,
		ID_ITEM_RETR			     = 12,	// 청약철회 버튼
		ID_PCROOM_SOLDIER_CHANGE_BTN = 13,  // PC방 용병 교체 버튼
		ID_PRESENT_DISASSEMBLE_BTN	 = 14,	// 용병선물 분해

		ID_POWERUP_BTN				 = 16,	//진화 버튼
		ID_POWERUP_HIRE_BTN			 = 17,  //진화 버튼 active시 사용될 연장버튼
		ID_POWERUP_DISMISSAL_BTN 	 = 18,  //진화 버튼 active시 사용될 해고 버튼
		ID_POWERUP_DISASSEMBLE_BTN	 = 19,  //진화 버튼 active시 사용될 분해 버튼

		ID_MY_SOLDIER_LEFT_BTN = 20,
		ID_MY_SOLDIER_RIGHT_BTN = 21,
		ID_INNER_TAB1 = 22,
		ID_INNER_TAB2 = 23,

		ID_KINDRED_BTN			     = 41,
		ID_HAIR_BTN				     = 42,
		ID_HAIR_COLOR_BTN		     = 43,
		ID_FACE_BTN				     = 44,
		ID_SKIN_COLOR_BTN		     = 45,
		ID_UNDERWEAR_BTN		     = 46,

		ID_ETC_SOULSTONE_USE_BTN	 = 47,
		ID_ETC_SOULSTONE_ENABLE_LIST = 48,
		ID_FREEDAY_SOLDIER_CHANGE_BTN = 49,
		
		ID_SOLDIER_WND               = 102,
		ID_COSTUME_SOLDIER_WND       = 103,
			
		ID_POWERUP_TIP				 = 150,
		ID_CHAR_GRADE_TOOLTIP		 = 151,

		ID_WEAPON_COSTUME_BTN		 = 201,
		ID_ARMOR_COSTUME_BTN		 = 202,
		ID_HELMET_COSTUME_BTN		 = 203,
		ID_CLOAK_COSTUME_BTN		 = 204,
	};

	enum SOLDIERINFO
	{
		SOLDIER_TITLE_X_OFFSET	= 129,
		SOLDIER_TITLE_Y_OFFSET	= 331,

		SOLDIER_LINE_GAP		= 23,

		POWERUP_TOOLTIP_OFFSET_X		= 6,
		POWERUP_TOOLTIP_OFFSET_Y		= 3,
	};

	enum BtnSizeByPower
	{
		//넓이
		BTN_WIDTH								= 66,
		BTN_WIDTH_BY_POWERUP_BTN				= 49,

		//연장 텍스트 위치
		HIRE_BTN_TEXT_X							= 34,
		HIRE_BTN_TEXT_Y							= 5,
		HIRE_BTN_TEXT_X_BY_POWERUP 				= 24,
		HIRE_BTN_TEXT_Y_BY_POWERUP 				= 6,

		//분해(진화가능 용병 일 때)
		DISASSEMBLE_BTN_POS_X_BY_POWERUP_BTN	= 129,
		//분해(진화가 가능하지 않는 용병 일 때)
		DISASSEMBLE_BTN_POS_X					= 97,
		//분해 텍스트 위치
		DISASSEMBLE_BTN_TEXT_X					= 33,
		DISASSEMBLE_BTN_TEXT_Y					= 7,
		DISASSEMBLE_BTN_TEXT_X_BY_POWERUP 		= 24,
		DISASSEMBLE_BTN_TEXT_Y_BY_POWERUP		= 7,
		
		//해고(진화가능 용병 일 때)
		DISMISSAL_BTN_POS_X_BY_POWERUP_BTN		= 180,
		//해고(진화가 가능하지 않는 용병 일 때)
		DISMISSAL_BTN_POS_X						= 165,
		//해고 텍스트 위치
		DISMISSAL_BTN_TEXT_X					= 33,
		DISMISSAL_BTN_TEXT_Y					= 7,
		DISMISSAL_BTN_TEXT_X_BY_POWERUP 		= 24,
		DISMISSAL_BTN_TEXT_Y_BY_POWERUP 		= 7,
	};
	enum ManualListNum
	{
		MLN_POWERUP_MANUAL = 802,
	};

protected:
	CHARACTER m_CharInfo;

	int m_iClassType;
	int m_iMainType;

	bool m_bSoldierTab;

	InventoryItemInfo m_ItemInfo;

	// soldier
	int m_iCurLevel;
	int m_iCurLevelExp;
	int m_iNextLevelExp;
	int m_iExpRate;
	ioComplexStringPrinter m_SoldierInfo[2];

	ioUIRenderImage *m_pGaugeMainBack;
	ioUIRenderImage *m_pGaugeBack;	
	ioUIRenderImage *m_pExpGauge;

	// deco
	ioHashString m_DecoName[6];

	// etc
	int m_iResellType;
	ioComplexStringPrinter m_EtcInfo;

	// present
	ioComplexStringPrinter m_PresentInfo[2];
	int m_iPresentSlotIndex;

	// subscription
	ioComplexStringPrinter m_SubscriptionInfo[3];
	
	ManualMgr* m_pManual;

	// Default Action Btn
	DWORD m_dwDefaultActionBtn;

	// Powerup ToolTip
	bool			 m_bShowPowerUpToolTip;
	ioUIRenderImage *m_pPowerUpToolTip;
	DWORD			 m_dwPowerUpToolTipLeaveColor;
	DWORD			 m_dwPowerUpToolTipOverColor;

protected:
	ClickPullDownEvent	m_AwakePullDownClick;
	int					m_iAwakeSelectArray;

protected:
	void PrintManual( int a_iXPos, int a_iYPos , float fScale );
	void SetCharInfo( OUT CHARACTER &rkCharInfo, IN int iType, IN int iCode );
	void GetItemList( DWORD dwID, vNewShopPullDownItem &rkList );

	void GetInvenDecoInfo( OUT int &riInvenDecoType, OUT int &rInvenDecoCode, IN int iClassType, IN int iDecoSexType, IN int iDecoType, IN int iDecoCode ); // iDecoSexType : 0 man, 1 woman
	void GetDecoInfo( DWORD dwID, OUT int &riDecoType, OUT int &riDecoCode );

	void SetEtcItemResell();
	void SetPresentItemResell();

	// present
	void SetPresentSoldier( int iValue1, int iValue2 );
	void SetPresentDeco( int iValue1, int iValue2 );
	void SetPresentEtcItem( int iValue1, int iValue2 );
	void SetPresentPeso( int iValue1, int iValue2 );
	void SetPresentExtraItem( int iValue1, int iValue2, int iValue3, int iValue4, short sPresentMent );
	void SetPresentExtraItemBox( int iValue1, int iValue2 );
	void SetPresentRandomDeco( int iValue1, int iValue2 );
	void SetPresentGradeExp( int iValue1, int iValue2 );
	void SetPresentMedalItem( int iValue1, int iValue2 );
	void SetPresentPet( int iValue1, int iValue2 );
	void SetPresentCostume( int iValue1, int iValue2 );
	void SetPresentCostumeBox( int iValue1, int iValue2 );
	void SetPresentBonusCash( int iValue1, int iValue2 );
	void SetPresentAccessory( int iValue1, int iValue2 );
	void SetPresentSpiritItem( int iValue1, int iValue2 );

	void RenderSoldierInfo();	// soldier
	void RenderDecoInfo();		// decoitem
	void RenderEtcInfo();		// etc
	void RenderPresentInfo();	// present

	bool CheckPresentSell( int iType, int iValue );
	void UISoldierSelectWndBtnUp( int iSoldierArray );

	void CheckRecvBtnAction();
	void CheckRetractBtn();
	void CheckPowerUpHeroInfo();

public:
	void SetBlank();
	void SetItemBlank( int iMainTab, int iSmallTab, int iClassType );

	void SetSlodierInfo( int iClassType );
	void SetInvenItemInfo( int iClassType, const InventoryItemInfo &rkInfo );
	void SetDecoItemInfo( int iClassType, const InventoryItemInfo &rkInfo );
	void SetEtcItemInfo( int iClassType, const InventoryItemInfo &rkInfo );
	void SetPresentItemInfo( int iClassType, const InventoryItemInfo &rkInfo );
	void SetSubscriptionItemInfo( int iClassType, const InventoryItemInfo &rkInfo );
	void SetCostumeInfo( int iClassType, const InventoryItemInfo &rkInfo );
	void SetCostumeSubInfo( DWORD dwWndID, int nSlotNum, float fScale, float fBackScale, float fDisableScale );

	void ShowSubscriptionRetrCheckLimit();
	void ShowSubscriptionRetrCheck( int iIndex, const ioHashString& szSubscriptionID, int iSubscriptionGold, int iRetraGold );

	const CHARACTER &GetCharInfo() const { return m_CharInfo; }

	void CheckDefaultActionBtn();

public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_create();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void iwm_pulldown_event( const PullDownEvent& Event );

	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void ParseExtraInfo( ioXMLElement &xElement );
	virtual void SetPositionToDefault( int iParentWidth, int iParentHeight,	bool bChildMove, bool bCreate = false );
	
protected:
	virtual void OnRender();
	virtual void OnProcess( float fTimePerSec );

public:
	void SetPowerUpBtn( bool bHire );

public:
	MyInventoryInfoWnd(void);
	virtual ~MyInventoryInfoWnd(void);
};
//////////////////////////////////////////////////////////////////////////
class SoldierDismissalWnd : public ioWnd
{
public:
	enum
	{
		ID_EXIT		 = 1,
		ID_DISMISSAL = 2,
		ID_CANCEL	 = 3,
		ID_DISMISSAL_EDIT = 4,
	};

	enum
	{
		BOTTOM_FRAME_GAP = 65,
		BUTTON_GAP       = 48,
		EDIT_GAP		= 98,

		NORMAL_HEIGHT    = 353,
		MORTMAIN_HEIGHT  = 410,

		DIS_NORMAL_HEIGHT    = 333,
		DIS_MORTMAIN_HEIGHT  = 421,
		DIS_PRESENT_HEIGHT	= 362,
	};

	enum DismissalType
	{
		SDT_DISMISSAL	= 1,
		SDT_DISASSEMBLE	= 2,
		SDT_DISASSEMBLE_PRESENT = 3,
		SDT_DISASSEMBLE_SUBSCRIPTION = 4,
	};

protected:
	ioUIRenderFrame *m_pBottomFrm;
	ioUIRenderFrame *m_pEditFrm;
	ioUIRenderImage *m_pIconBack;
	ioUIRenderImage *m_pFeelIcon;	
	ioUIRenderImage *m_pCharIcon;
	ioUIRenderImage *m_pCharSubIcon;

protected:
	ioWnd *m_pPreEdit;
	ioWnd *m_pDismissalEdit;
	int		m_iClassType;
	int		m_iPresentIndex;
	int		m_iPresentSlot;
	DismissalType m_DismissalType;

	int m_nGradeType;
	ioHashString m_szDisassembleFail;
	ioHashString m_szSubscriptionID;

protected:
	bool CheckDismissal();
	void SendDismissal();

	bool CheckDisassemble();
	void SendDisassemble();
	void SendDisassemblePresent();

public:
	void SetDismissalChar( int iClassType );
	void SetDisassembleChar( int iClassType );
	void SetDisassemblePresentChar( int iType, int iPresentIndex, int iPresentSlot, ioHashString szID );

public:
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void ParseExtraInfo( ioXMLElement &xElement );

public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnRender();

public:
	SoldierDismissalWnd();
	virtual ~SoldierDismissalWnd();
};
//////////////////////////////////////////////////////////////////////////
class ItemRecvSellInfoWnd : public ioWnd
{
public:
	enum CommandType
	{
		ITEM_SELL			= 1,
		ITEM_RECV_NORMAL	= 2,
		ITEM_RECV_NOT_GO	= 3,
		ITEM_DISASSEMBLE	= 4,
	};

	enum ItemInfoType
	{
		ITEM_NONE		= 0,
		ITEM_SOLDIER	= 1,
		ITEM_EXTRAITEM	= 2,
		ITEM_ETCITEM	= 3,
		ITEM_PRESENT	= 4,
		ITEM_DECO		= 5,
		ITEM_ALCHEMIC	= 6,
		ITEM_MEDAL		= 7,
		ITEM_PET		= 8,
		ITEM_COSTUME	= 9,
		ITEM_ACCESSORY	= 10,
	};

	enum
	{
		ID_GO			= 1,
	};

protected:
	CommandType m_CommandType;
	ItemInfoType m_ItemInfoType;

	ioUIRenderImage *m_pIcon;
	ioUIRenderImage *m_pSubIcon;
	ioUIRenderImage *m_pBackEffect;
	ioUIRenderImage *m_pNumText;
	ioUIRenderImage *m_pPlusText;
	ioUIRenderImage *m_pCustomIcon;
	ioUIRenderImage *m_pCostumeMark;

	int m_iClassType;
	int m_iReinforce;
	bool m_bExtraItemCustom;

	ComplexStringPrinterVec m_vTitle;
	ComplexStringPrinterVec m_vDesc;

	ioHashString m_sDescMileage;

	bool m_bIsCostume;
	int m_nGradeType;
	
public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

protected:
	virtual void OnRender();

public:
	void SetItemRecvSellInfo( int iCommandType,
							  int iItemInfoType,
							  const ioHashString szIconName,
							  const ioHashString szSubIconName,
							  const ComplexStringPrinterVec& pTitleVec,
							  const ComplexStringPrinterVec& pDescVec,
							  int iClassType,
							  int iReinforce,
							  bool bExtraItemCustom,
							  bool bIsCostume,
							  int nGradeType );

	void SetDescMileage( const ioHashString &rsDescMileage ) { m_sDescMileage = rsDescMileage; }

protected:
	void ReBatchButton();

public:
	ItemRecvSellInfoWnd();
	virtual ~ItemRecvSellInfoWnd();
};