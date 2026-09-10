#pragma once

#include "ioMessageBox.h"
#include "NewShopWnd.h"
#include "FSMStateMgr.h"

struct NewShopPullDownItem
{
	ioHashString m_szName;
	int          m_iMagicCode; 
	DWORD        m_dwTextColor;
	bool         m_bGrowthCatalystItem;
	int          m_iGrowthCatalystLevel;
	bool		 m_bCount;
	ioHashString m_szCnt;
	DWORD        m_dwCntTextColor;
	bool		 m_bPetLevel;
	ioHashString m_szPetLevel;
	DWORD		 m_dwPetLevelTextColor;
	NewShopPullDownItem()
	{
		m_iMagicCode = 0;
		m_dwTextColor= 0; 
		m_bGrowthCatalystItem = false;
		m_iGrowthCatalystLevel = 0;
		m_bCount = false;
		m_dwCntTextColor = 0;
		m_bPetLevel = false;
		m_dwPetLevelTextColor = 0;
	}
};
typedef std::vector< NewShopPullDownItem > vNewShopPullDownItem;
//-------------------------------------------------------------------------
class NewShopPullDownWnd : public ioWnd
{
public:
	enum
	{
		//SCROLL
		ID_VERT_SCROLL    = 1000,
	};

protected:
	ioUIRenderImage *m_pDarkLine;
	ioUIRenderImage *m_pLightLine;
	ioUIRenderImage *m_pOverLine;
	ioUIRenderFrame *m_pStatePinkFrm;

protected:       // const
	int  m_iStartHGap;
	int  m_iEndHGap;
	int  m_iWidthGap;
	int  m_iItemHSize;
	int  m_iItemMaxPage;

protected:
	vNewShopPullDownItem m_PullDownItem;

protected:
	ioWnd*	m_pScrollWnd;
	ioWnd*	m_pOpenBtn;
	int		m_iCurPage;
	int		m_iOverListIndex;
	float	m_fFontSize;
	float	m_fWidthCutSize;

public:
	void CreatePullDown( ioWnd *pOpenBtn, int iXPos, int iYPos );
	void InsertItem( NewShopPullDownItem &rkItem );

public:
	ioWnd *GetOpenBtn(){ return m_pOpenBtn; }
	int GetListArray(){ return m_iOverListIndex; }
	bool GetOverItem( OUT NewShopPullDownItem &rItem );

public:
	virtual void iwm_show();
	virtual void iwm_hide();
public:
	virtual void iwm_vscroll( DWORD dwID, int curPos );
	virtual void iwm_wheel( int zDelta );

public:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );
	virtual void ParseExtraInfo( ioXMLElement &xElement );

protected:
	bool PrevClickCheck();
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	NewShopPullDownWnd();
	virtual ~NewShopPullDownWnd();
};
//-------------------------------------------------------------------------
class NewShopHelpControlBtn : public ioButton
{
protected:
	ioUIRenderImage *m_pBtn;

protected:
	void RenderInfo(int iXPos, int iYPos, UIRenderType eType);

public:
	void SetBtnAlphaRate( int iAlphaRate );

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void OnDrawNormal( int iXPos, int iYPos );
	virtual void OnDrawOvered( int iXPos, int iYPos );
	virtual void OnDrawPushed( int iXPos, int iYPos );
	virtual void OnDrawDisabled( int iXPos, int iYPos );

public:
	NewShopHelpControlBtn();
	virtual ~NewShopHelpControlBtn();
};
//-------------------------------------------------------------------------
class NewShopHelpControlWnd : public ioWnd
{
public:
	enum
	{
		ID_LEFT_ROTATE_BTN  = 1,
		ID_RIGHT_ROTATE_BTN = 2,
		ID_DEFAULT_POS_BTN  = 3,
		MAX_BTN = 3,
	};

	enum AniType
	{
		AT_NONE      = 0,
		AT_ALPHA_IN  = 1,
		AT_NORMAL    = 2,
		AT_ALPHA_OUT = 3,
		AT_HIDE      = 4,
	};

protected:
	FSMStateMgr m_AniState;

protected:
	void InitAniState();
	void SetChildAlpha( int iAlphaRate );

public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

	void SetDefaultPosTitle( const char* szTitle ); 
	void HideWndAnimation();

protected:
	virtual void OnProcess( float fTimePerSec );

public:
	NewShopHelpControlWnd();
	virtual ~NewShopHelpControlWnd();
};
//--------------------------------------------------------------------------
#define DESC_WIDTH 204.0f

class NewShopHelpWnd : public ioWnd
{
public:
	enum 
	{
		ID_CHAR_CONTROL_WND     = 1,
	};

protected:
	ioUI3DRender *m_pUI3DRender;
	ioCreateChar *m_aMyChar;
	CHARACTER     m_CharInfo;

	ioUIRenderImage *m_p3DBack;

	float         m_fRotateYaw;
	float         m_fPreYaw;
	int           m_iClassType;
	float         m_fXMLRotateYaw;
	float		  m_fZPosition;

	bool          m_bLeftRotateOn;
	bool          m_bRightRotateOn;

	bool          m_bReleaseHelm;
	bool          m_bReleaseAllItem;
	bool          m_bShopMotion;
	bool          m_bUserInfoToolTip;

	ioHashString	m_szAwakeEffect;

protected:
	void CreateCharacter( const CHARACTER &rkInfo, bool bEffect );
	void RotateY( float fYaw );

public:
	void SetChar( const CHARACTER &rkInfo, bool bEffect = true );
	void SetEquip( bool bReleaseHelm, bool bReleaseAll, bool bShopMotion );

	void InitRotate();
	void SetCharScale( float fScale );

	void ShowNoneControlWnd();
	void SetZPosition( float fZPos );
	void SetUserInfoToolTip(bool bUserInfoToolTip) { m_bUserInfoToolTip = bUserInfoToolTip; }
	void SetCharAnimation( bool bShopMotion );
	void SetCharAnimation( DWORD dwEtcItem, bool bMale );
	void SetCharLoopAnimation( const ioHashString &szAniName, float fFadeIn = 0.0f );
	void SetCharActionAnimation( const ioHashString &szAniName, float fFadeIn = FLOAT100, float fFadeOut = FLOAT100 );
	void SetCharMotionItemAnimation( const ioHashString &szAniName );
	void SetChangeSkeleton( DWORD dwEtcItem, bool bMale );

	void EquipItem( DWORD dwItemCode, DWORD dwItemMaleCustom, DWORD dwItemFemaleCustom, int nCostumCode=0 );
	void ReleaseItem( int iSlot );

	ioEffect* CharAttachEffect( const ioHashString &rkEffect, ioEntityEvent *pEntityEventOwner = NULL );
	void CharEndEffect( const ioHashString &rkEffect, bool bGrace );

public:
	inline bool IsReleaseHelm(){ return m_bReleaseHelm; }
	inline bool IsReleaseAllItem(){ return m_bReleaseAllItem; }
	inline bool IsShopMotion(){ return m_bShopMotion; }

public:
	virtual void iwm_create();
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void ParseExtraInfo( ioXMLElement &xElement );

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	NewShopHelpWnd();
	virtual ~NewShopHelpWnd();
};
//-------------------------------------------------------------------------
class NewShopNeedLevelWnd : public ioWnd
{
public:
	enum 
	{
		ID_PRICE1_RBTN         = 1,
		ID_PRICE2_RBTN         = 2,

		ID_X_BTN               = 101,
		ID_BUY_BTN             = 102,
		ID_EXIT_BTN            = 103,
	};

	enum 
	{
		MAX_RBTN = 2,
	};
public:
	struct NeedLevelInfo
	{
		ioHashString    m_sTitle;
		ioHashString    m_sNeedLevel;
		ioHashString    m_sIcon;
		ioHashStringVec m_vNameVec;
		ioHashStringVec m_vPriceVec;

		void Clear()
		{
			m_sTitle.Clear();
			m_sNeedLevel.Clear();
			m_sIcon.Clear();
			m_vNameVec.clear();
			m_vPriceVec.clear();
		}

		NeedLevelInfo()
		{
			Clear();
		}
	};
protected:
	ioUIRenderImage      *m_pIcon;
	NeedLevelInfo         m_kNeedLevelInfo;
	DWORD                 m_dwSelectPriceBtnID;
	DWORD                 m_dwLastPriceBtnID;

public:
	bool SetInfo( const NeedLevelInfo &rPriceInfo );

protected:
	virtual void OnRender();

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

public:
	NewShopNeedLevelWnd(void);
	virtual ~NewShopNeedLevelWnd(void);
};
//-------------------------------------------------------------------------
class NewShopNeedPesoWnd : public ioWnd
{
public:
	enum
	{
		ID_ITEM_1_BTN = 1,
		ID_ITEM_2_BTN = 2,
		ID_ITEM_3_BTN = 3,

		ID_X_BTN      = 101,
		ID_LEFT_BTN   = 102,
		ID_RIGHT_BTN  = 103,
		ID_ESC_BTN    = 104,
	};

	enum
	{
		MAX_ITEM_BTN     = 3,
		PAGE_PER_BTN_NUM = 3,
	};

public:
	struct NeedPesoInfo
	{
		ioHashString    m_sName;
		ioHashString    m_sIcon;
		ioHashString    m_sPrice;
		ioHashString    m_sMinusPrice;

		void Clear()
		{
			m_sName.Clear();
			m_sIcon.Clear();
			m_sPrice.Clear();
			m_sMinusPrice.Clear();
		}

		NeedPesoInfo()
		{
			Clear();
		}
	};

protected:
	ioUIRenderImage*		m_pIcon;

	NeedPesoInfo			m_kNeedPesoInfo;
	ioHashString			m_sPage;
	int						m_iMaxPage;
	int						m_iCurPage;

	vNewShopItemInfoVec		m_vNewShopItemInfoVec;

protected:
	void UpdatePage( int iPage );
	void UpdateItemBtn();

public:
	bool SetInfo( const NeedPesoInfo &rkNeedPesoInfo );
	bool GetCurItemInfo( OUT NewShopItemInfo &rkInfo, IN DWORD dwItemInfoID );

protected:
	virtual void OnRender();

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

public:
	NewShopNeedPesoWnd(void);
	virtual ~NewShopNeedPesoWnd(void);
};
//-------------------------------------------------------------------------
#define NEW_SHOP_BUY_ICON_BTN_SCALE 0.53f
#define SHOP_HELP_CHAR_SCALE        0.90f

class NewShopBuyWnd : public ioWnd
{
public:
	enum 
	{
		ID_BUY_PESO_BTN         = 1,
		ID_BUY_CASH_BTN         = 2,
		ID_BUY_PRESENT_BTN      = 3,
		ID_BUY_GO_DECO_BTN      = 4,
		ID_MY_SOLDIER_LIST_BTN1  = 5,
		ID_BUY_GOODS_LIST_BTN   = 6,
		ID_BUY_ADD_BTN          = 7,
		ID_SHOW_BOUGHT_ITEM_BTN = 8,
		ID_DECO_CHANGE_SEXTYPE_BTN = 9,
		ID_RECOMMEND_DECO_BTN   = 10,
		ID_BUY_STAMP_BTN         = 11,
		ID_MY_SOLDIER_LIST_BTN2  = 12,
		ID_EQUIP_CHANGE_SEXTYPE_BTN = 13,
		ID_ITEM_MOVIE_BTN = 14,
		ID_PRESENT_EVENT_BTN = 15,

		ID_PRICE1_RBTN          = 51,
		ID_PRICE2_RBTN          = 52,
		ID_PRICE3_RBTN          = 53,
		ID_PRICE4_RBTN          = 54,

		ID_SOLDIER_WND          = 102,
		ID_FLASH_WND            = 103,

		UID_FACE		= 1,
		UID_HAIR		= 2,
		UID_SKIN_COLOR	= 3,
		UID_HAIR_COLOR	= 4,
		UID_KINDRED     = 5,
		UID_CLASS       = 6,
		UID_UNDERWEAR   = 7,
		UID_WEAPON       = 10,
		UID_ARMOR        = 11,
		UID_HELM         = 12,
		UID_CLOAK        = 13,

		ID_DECO_PREVIEW_KINDRED_BTN     = 1001,
		ID_DECO_PREVIEW_HAIR_BTN        = 1002,
		ID_DECO_PREVIEW_HAIRCOLOR_BTN   = 1003,
		ID_DECO_PREVIEW_FACE_BTN        = 1004,
		ID_DECO_PREVIEW_SKINCOLOR_BTN   = 1005, 
		ID_DECO_PREVIEW_UNDERWEAR_BTN   = 1006,

		ID_DECO_EQUIP_KINDRED_BTN       = 2001,
		ID_DECO_EQUIP_HAIR_BTN          = 2002,
		ID_DECO_EQUIP_HAIRCOLOR_BTN     = 2003,
		ID_DECO_EQUIP_FACE_BTN          = 2004,
		ID_DECO_EQUIP_SKINCOLOR_BTN     = 2005, 
		ID_DECO_EQUIP_UNDERWEAR_BTN     = 2006,
	};

	enum 
	{
		MAGIC_CODE_CASH_START = 100,
		MAX_PRICE             = 4,
		MAX_DECO_BTN          = 6,
		MAX_LOW_BTN_DATA      = 3,
	};

protected:
	struct PriceInfo
	{
		bool         m_bPeso;
		bool         m_bStamp;
		int          m_iPriceArray;
		ioHashString m_sName;
		ioHashString m_sPrice;
		int          m_iPrice;
		DWORD        m_dwPriceColor;
		bool         m_bBonus;
		bool         m_bEvent;

		void Clear()
		{
			m_bPeso        = false;
			m_bStamp       = false;
			m_iPriceArray  = 0;
			m_sName.Clear();
			m_sPrice.Clear();
			m_iPrice       = 0;
			m_dwPriceColor = 0;
			m_bBonus       = false;
			m_bEvent       = false;
		}

		PriceInfo()
		{
			Clear();
		}
	};
protected:
	struct BtnInfo
	{
		int m_iBtnWidth[MAX_LOW_BTN_DATA];
		int m_iBtnPosX[MAX_LOW_BTN_DATA];
		int m_iBtnTitleX[MAX_LOW_BTN_DATA];

		BtnInfo()
		{
			for (int i = 0; i < MAX_LOW_BTN_DATA ; i++)
			{
				m_iBtnWidth[i] = 0;
				m_iBtnPosX[i] = 0;
				m_iBtnTitleX[i] = 0;
			}
		}
	};

	typedef std::vector<BtnInfo> vBtnInfo;

protected:
	ioUIRenderImage *m_pIcon;
	ioUIRenderImage *m_pIconBack;
	ioUIRenderImage *m_pIconBack2;

	ioUIRenderImage *m_pNumText;
	ioUIRenderImage *m_pPlusText;
	ioUIRenderImage *m_pEventMark;
	ioUIRenderImage *m_pBonusMark;
	ioUIRenderImage *m_pTitleLeft;
	ioUIRenderImage *m_pTitleCenter;
	ioUIRenderImage *m_pLock;
	ioUIRenderImage *m_pStripGrayDark;
	ioUIRenderImage *m_pStripGrayLight;
	ioUIRenderImage *m_pCostumeMark;
	ioHashString     m_sTitle;
	ioHashString     m_sBuySound;

	NewShopItemInfo  m_kItemInfo;
	CHARACTER        m_CharInfo;
	DWORD			 m_dwSelectWndID;

	PriceInfo        m_kPriceInfoArray[MAX_PRICE];
	DWORD            m_dwLastPriceBtnID;
	DWORD            m_dwSelectPriceBtnID;

	DWORD            m_dwChangeMarkDelay;
	DWORD            m_dwCurChangeMarkTime;
	bool             m_bChangeMark;
	int              m_iBoughtBonusPeso;
	bool             m_bEtcItem3DChar;
	bool m_bSubscriptionResult;

	bool m_bEquipChangeFemale;
	int m_nEquipChangeSex;
	bool m_bSetPreview;

	bool m_bFlashDown;

	int m_nGradeType;

protected:
	void PrintManual( int a_iXPos, int a_iYPos , float fScale );
	void SetDecoChangeSexTypeTitle();
	void SetPriceInfo();
	void GetGoodsUseName( OUT char *szName, IN int iNameSize, IN bool bPeso, IN int iPriceArray );
	void SetPriceRadioBtn( DWORD dwBtnID );
	void SetBuyBtns( bool bPeso, bool bStamp );
	void SetDecoPreviewBtn( const NewShopItemInfo &rkNewShopItemInfo, bool bInit );
	void ReleaseDecoPreviewBtn( int iDecoType );
	void PullDownBtnUP( ioWnd *pOpenBtn, int iListArray );
	void UISoldierSelectWndBtnUp( int iSoldierArray );
	void SetDecoChangeSexTypeBtnUp();
	void SetRecommendDecoBtnUp();

	void OnBuyBtnUp( DWORD dwID );

	enum { CHECK_PRICE_ERROR = 0, CHECK_PRICE_OK = 1, CHECK_PRICE_PRESENT_OK = 2, };
	int  CheckPrice( DWORD dwID );    
	bool CheckSoldierBuy( DWORD dwID );
	bool CheckExtraBuy( DWORD dwID );
	bool CheckDecoBuy( DWORD dwID );
	bool CheckEtcBuy( DWORD dwID );
	bool CheckEventGoodsBuy( DWORD dwID );
	bool CheckCostumeBuy( DWORD dwID );

	void ShowBuyMsgBox( DWORD dwID );
	void ShowBuyConfirmMsg( DWORD dwID, const char *szName, int iType );
	void ShowNeedLevelMsg();
	void ShowNeedPesoMsg();
	void ShowSendPresentIdWnd( short isPresentType, int iPresentValue1, int iPresentValue2 );

	bool SendPresentBuy( short isPresentType, int iPresentValue1, int iPresentValue2 );

	void GetCharInfoByDeco( OUT CHARACTER &rkCharInfo, IN int iType, IN int iCode );
	void GetInvenDecoInfo( OUT int &riInvenDecoType, OUT int &rInvenDecoCode, IN int iClassType, IN int iDecoSexType, IN int iDecoType, IN int iDecoCode ); // iDecoSexType : 0 man, 1 woman

	void ReSettingBtn( DWORD dwID, int iWidth, int iPosX, int iTitleX );

	bool IsSelectPricePeso( DWORD dwSelectPriceBtnID );
	bool IsSelectPriceStamp( DWORD dwSelectPriceBtnID );
	int GetSelectPriceArray( DWORD dwSelectPriceBtnID );

	void SetBoughtBonusPeso();
	void SetDecoPreviewBtnUp( DWORD dwID );
	void SetCharInfoByPreview( OUT CHARACTER &rkCharInfo );

	void SetChildExActive( DWORD dwID );
	void SetChildExInActive( DWORD dwID );

protected:
	void SetEquipChangeSexTypeTitle( int nType );
	void SetEquipChangeSexTypeBtnUp();
	void SetMovieBtnFunc();
	void SetMovieFile( const ioHashString& szFlashDir, const ioHashString& szFlashFile, const ioHashString& szUrl, bool bAfterProcess );

public:
	void SetInfo( const NewShopItemInfo &rkInfo );
	void ShowOnlyBoughtBtns( bool bSubscription=false );
	void InitCharRotate();
	void SendBuyProcess();
	bool SendBuy();

	void SetPriceRadioBtnByCash( int iCash );
	void GetMyClassTypeVec( OUT IntVec &rvClassTypeVec );

	void SetDecoEquipBtns();

	bool EnableDecoChangeByEquipItem( CHARACTER &rkCharInfo );

	int GetSubscriptionType();

public:
	virtual void iwm_create();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void ParseExtraInfo( ioXMLElement &xElement );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

protected:
	void OnRenderNumber( int iXPos, int iYPos );

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

public:
	bool QuestGuide( int iBuyValue1, int iBuyValue2, int iBuyValue3, int iBuyValue4, int iBuyValue5, float &rfXPos, float &rfYPos );

protected:
	bool SendSubscriptionBuy( short isPresentType, int iPresentValue1, int iPresentValue2 );

public:
	NewShopBuyWnd(void);
	virtual ~NewShopBuyWnd(void);
};
//---------------------------------------------------------------------------------------------------------------------------------
class NewShopBuyConfirmWnd : public ioWnd
{
public:
	enum
	{
		ID_EXIT_BTN       = 1,
		ID_OK_BTN         = 2,
		ID_CANCEL_BTN     = 3, 
		ID_CLAUSE_VIEW_BTN= 4,
	};

	enum
	{
		NORMAL_HEIGHT     = 127,
		MGAME_HEIGHT      = 147,
	};

protected:
	ioComplexStringPrinter m_PrinterList[MAX_MSG_LIST_SIZE];
	ioWnd *m_pReturnWnd;

	int m_iSubscriptionType;

protected:
	void ClearList();
	void SendBuyFromReturnWnd();

public:
	void SetPrintMessage( ioWnd *pReturnWnd, ioComplexStringPrinter *MsgPrinter, int iType );

public:
	virtual bool iwm_spacebar();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnRender();

public:
	NewShopBuyConfirmWnd();
	virtual ~NewShopBuyConfirmWnd();
};
