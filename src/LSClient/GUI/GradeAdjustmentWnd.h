#pragma once

#include "../ioPowerUpManager.h"

class ioUIRenderImage;
class ioUIRenderFrame;

struct ReinforceMagicEffectInfo
{
	DWORD m_dwCreateTime;

	int m_iCurAlphaRate;
	float m_fCurScaleRate;


	ReinforceMagicEffectInfo()
	{
		Init();
	}

	void Init()
	{
		m_dwCreateTime = 0;
		m_fCurScaleRate = FLOAT1;
		m_iCurAlphaRate = MAX_ALPHA_RATE;
	}
};
typedef std::vector< ReinforceMagicEffectInfo > vReinforceMagicEffectInfo;

class GradeInfo
{
public:
	//해당 정보가 아이템 정보인지 영웅 정보인지
	int   m_iType;
	int	  m_iTabType;
	//char의 경우 class type, item의 경우 index값(가지고 있지 않은 장비의 경우 1000 + n순번으로 정해짐)
	int   m_iValue;
	//아이템 강화 정보( 아이템 강화 수치 )
	int   m_iValue2;
	//char의 경우, item의 경우 둘다 code
	int   m_iValue3;

	//우선순위
	int   m_iPriorityOrder;

	// 보유 유무
	bool m_bHasPowerUp;

	ioHashString m_szName;
	ioHashString m_szSubDesc;
	ioHashString m_szWeaponDesc;
	ioHashString m_szIconName;

public:
	void Clear()
	{
		m_iType = -1;
		m_iTabType = -1;
		m_iValue = 0;
		m_iValue2 = 0;
		m_iValue3 = 0;
		m_iPriorityOrder = 0;
		m_bHasPowerUp = true;
		m_szName.Clear();
		m_szSubDesc.Clear();
		m_szIconName.Clear();
		m_szWeaponDesc.Clear();
	}
	bool IsEmpty()
	{
		if ( m_iType == -1 && m_iTabType == -1 && m_iValue == 0 && m_iValue2 == 0 && m_iPriorityOrder == 0 )
			return true;
		return false;
	}

public:
	GradeInfo()
	{
		Clear();
	}
	~GradeInfo()	{ }
};
typedef std::vector< GradeInfo > vGradeInfo;

class GradeAdjustmentInfoWnd : public ioButton
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
	ioComplexStringPrinter m_szDescArray[MAX_DESC];
	GradeInfo m_PowerUpInfo;
	ioUIRenderImage *m_pEmptyImg;
	ioUIRenderFrame *m_pSelectFrm;
	int m_nGradeType;
	bool m_bSelect;

protected:	
	void ClearDesc();
	void RenderDesc( int iXPos, int iYPos );

	bool SetInfo();

public:
	void SetBlank();
	void SetPowerUpInfo( const GradeInfo& rkItem );
	void SetItem( );
	void SetSelect(bool bSelect);

	ioHashString& GetSlotItemName(){ return m_PowerUpInfo.m_szName; }
	ioHashString& GetSlotItemIconName(){ return m_PowerUpInfo.m_szIconName; }
	int GetPowerUpType(){ return m_PowerUpInfo.m_iType; }
	int GetPowerUpValue1(){ return m_PowerUpInfo.m_iValue; }
	int GetPowerUpValue3(){ return m_PowerUpInfo.m_iValue3; }

protected:
	virtual void OnRender();
	virtual void OnRenderAfterChild();

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );

public:
	GradeAdjustmentInfoWnd();
	virtual ~GradeAdjustmentInfoWnd();
};

/////////////////////////////////////////////////////////////////////////////////////////////////////
class GradeAdjustmentSelectIconBtn : public ioButton
{
protected:
	ioUIRenderImage *m_pIcon;
	ioUIRenderImage *m_pIconBack;
	ioUIRenderImage *m_pNumText;
	ioUIRenderImage *m_pPlusText;
	//캐릭터 레벨 및 아이템 레벨
	int				m_iItemReinforce;

	float m_fScale;
	float m_fBackScale;

	int m_nGradeType;

protected:
	void RenderIcon(int iXPos, int iYPos, UIRenderType eType);
	void RenderBack(int iXPos, int iYPos );

public:
	bool SetIcon( ioUIRenderImage *pIcon, float fScale, float fBackScale, int ireinforce );
	void SetGradeType( int nGradeType ){ m_nGradeType = nGradeType; }

public:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	

protected:
	virtual void OnDrawNormal( int iXPos, int iYPos );
	virtual void OnDrawOvered( int iXPos, int iYPos );
	virtual void OnDrawPushed( int iXPos, int iYPos );
	virtual void OnDrawDisabled( int iXPos, int iYPos );

public:
	GradeAdjustmentSelectIconBtn();
	virtual ~GradeAdjustmentSelectIconBtn();
};

class GradeAdjustmentWnd : public ioWnd
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

		ID_VERT_SCROLL			= 1001
	};

	enum
	{
		ID_TAB_1_BTN	 = 101, // Hero
		ID_TAB_2_BTN	 = 102, // Equip Item
		ID_TAB_COUNT_MAX = 2,
	};

	enum
	{
		ID_SMALL_TAB_1_BTN						= 401,
		ID_SMALL_TAB_2_BTN						= 402,
		ID_SMALL_TAB_3_BTN						= 403,
		ID_SMALL_TAB_4_BTN						= 404,
		ID_SMALL_TAB_5_BTN						= 405,
		ID_SMALL_TAB_6_BTN						= 406,

		ID_HERO_SUB_TAB_COUNT_MAX = 6,
		ID_ITEM_SUB_TAB_COUNT_MAX = 5,
	};

	enum 
	{
		ID_REINFORCE_BTN						= 300,
		ID_DEGENERATION_BTN						= 301,
		ID_SOLDIER_WND							= 302,
	};

	enum 
	{
		MAX_HERO_TAB_BTN		= 6,
		MAX_ITEM_TAB_BTN		= 5,
		MAX_SMALL_TAB_BTN		= 11,

		MAX_ITEM_INFO_WND_BTN	= 8,
		PAGE_PER_ITEM_NUM		= 4,
	};

protected:
	ioPowerUpManager::PowerUpInfoType m_InfoType;
	int m_iTabLineSize;
	int m_iTabPosX;
	int m_iCurPage;

	DWORD m_dwCurTabID;
	DWORD m_dwCurSmallTabID;
	DWORD m_dwSmallTabEndID;
	int m_iSelectArray;		// 선택한 UI 슬롯 배열 인덱스
	int m_iSelectValue;		// 선택한 용병 또는 장비 코드 값
	ioHashStringVec     m_vSmallTabNameVecArray[ID_TAB_COUNT_MAX];
	IntVec              m_vSmallTabTypeVecArray[ID_TAB_COUNT_MAX];
	int                 m_iDefaultTypeArray[ID_TAB_COUNT_MAX];

	// Item 진화 퇴화 ////////////////////////////////
	int		  m_iItemIndex;
	// magic effect
	vReinforceMagicEffectInfo m_vMagicEffectInfoList;
	DWORD m_dwMagicEffectCreateTime;
	//result effect
	bool m_bShowResultEffect;
	DWORD m_dwResultStartTime;
	DWORD m_dwResultEndTime;
	DWORD m_dwResultEffectEndTime;
	DWORD m_dwResultEffectTime;
	ioUIRenderImage *m_pLightImage;
	float m_fScaleRate;
	float m_fAlphaRate;
	float m_fSlopeRate;
	BYTE m_iAlpha1;
	BYTE m_iAlpha2;
	float m_fCurEffectScale;
	//result sound
	ioHashString m_szRollingSound;
	UIRenderType m_iIconRenderType;
	//Success light effect
	ioUIRenderImage *m_pSuccessLightImage;
	ioHashStringVec m_vSuccessLightImage;	// 성공
	DWORD   m_dwFinalResultStartTime;
	DWORD	m_dwSuccessEffectTime;
	int		m_iCurSuccessEffectArray;
	DWORD   m_dwRollingEffectCheckTime;
	DWORD   m_dwRollingEffectTime;
	//Success Effect
	ioHashString m_szEffectName;
	ioHashString m_szSuccessSound;
	int m_nGradeType;

	ioUIRenderImage *m_pMagicCircle;
	ioUIRenderImage *m_pIconBack;
	ioUIRenderImage *m_pIconSlot;
	ioUIRenderImage *m_pIcon;
	ioUIRenderImage *m_pNumText;
	ioUIRenderImage *m_pPlusText;
	ioUIRenderImage *m_pItemInfoBG;

	IoString m_ItemReinforceInfo;
	ioComplexStringPrinter m_ItemSelectPrinter;
	ioHashString m_szNoItemDesc;
	ioHashString m_ItemInfo;

	//아이템 강화 수치
	int m_iItemReinforce;
	////////////////////////////////////////////////////////////////
	bool m_bOpen;
	bool m_bReinforce;

	PowerUpTargetType m_eGradeUp;
	//
	ioUIRenderImage *m_pDotLineRight;
	ioUIRenderImage *m_pDotLineBottom;
	ioUIRenderFrame *m_pEditFrm;
	ioComplexStringPrinter m_TitlePrinter;
protected:
	ioHashString m_szCurrFindKeyword;

	vGradeInfo m_SelectCharInfoVec;
	vGradeInfo m_SelectItemInfoVec;
	vGradeInfo m_HasNotItemVec;
	vGradeInfo m_CurrSelectInfoVec;
public:
	virtual void iwm_create();
	virtual void iwm_hide();
	virtual void iwm_show();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

	virtual void iwm_vscroll(DWORD dwID, int curPos );
	virtual void iwm_wheel( int zDelta );

	ioHashString CheckGrade( int iCode );

	bool SetSelectedReinforceItem( GradeAdjustmentInfoWnd *pSelectItem, bool bSlotClick );
	void CheckReinforceButton();
	void Reinforce();
	virtual void ParseExtraInfo( ioXMLElement &xElement );

protected:
	void CreateMagicCircle();
	virtual void OnProcess( float fTimePerSec );
	void ProcessResultHeroEffect();
	void ProcessMagicCircle();
	void ProcessSuccessLightEffect();
	void ProcessResultItemEffect();
	virtual void OnRender();
	virtual void OnRenderAfterChild();
	void OnRenderMagicCircle();
	void OnRenderResultEffect();
	void OnRenderSuccessLightEffect();
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

protected:
	void ChangeTab( DWORD dwID, const char *szFind );
	DWORD GetDefaultSmallTab( DWORD dwMainTab );
	int UpdateSmallTabName( ioWnd* pWnd, int iXOffset, const ioHashString& szTitle );
	void ChangeSmallTab( DWORD dwID );
	void UpdatePage( int iNewPage, bool bUpdateRightItem = true );

	void SetAllItemInfoVec();
	bool SetItemInfoVec(int iItemCode, bool bNotItemAdd = true);					// 진화 장비 아이템 보유/미보유 구분하여 담는 함수
	void SetCurItemInfoVec( DWORD dwTabID, const char *szFind );

	bool IsRightTab( DWORD dwTabID, int iType );
	void RightButtonControl( ioPowerUpManager::PowerUpImpossibleFunc eCheckFunc );
	void SetFind();

protected:
	CHARACTER m_CharInfo;
	ioUIRenderImage *m_pDarkTextBack;
	ioUIRenderImage *m_pLightTextBack;
	ioUIRenderImage *m_pSoldierInfoBG;

	IoString m_SoldierReinforceInfo;
	IoString m_SoulStoneCountText;

	ioComplexStringPrinter m_SoldierSelectPrinter;
	ioComplexStringPrinter m_SoldierReinforcePrinter;
	ioHashString m_szSoulStoneText;
	ioHashString m_SoldierInfo;
	IoString m_ReinforcedSoldierInfo;
	ioComplexStringPrinter m_ReinforcedPrinter;

public:
	void ShowSubWnd( int iGradeAdjust_InfoType, int iItemValue = -1 );	// syh 마지막 파라메터는 용병 번호, 아이템 번호
	void SetSoldierInfo( int iClassType );
	void SetItemInfo( int iIndex, bool bClicked = false, GradeAdjustmentInfoWnd *pSelectItem = NULL );
	void ResetSoulStone();
	void HeroReinforceClearListAll();
	void ItemReinforceClearListAll();

	void CheckReinforceEnd( ioPowerUpManager::PowerUpInfoType eType );
public:
	void Update();

public:
	GradeAdjustmentWnd();
	virtual ~GradeAdjustmentWnd();
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class GradeAdjustmentCategoryTab : public ioRadioButton 
{
protected:
	TabUISettingVec m_Setting;

public:
	void ClearTab(int iIndex = -1);
	void SetTab( const TabUISettingVec& rkTabVec );
	const TabUISetting& GetTab(int iIndex = 0);

public:
	GradeAdjustmentCategoryTab();
	virtual ~GradeAdjustmentCategoryTab();
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class GradeAdjustmentInfoSortByPriority : public std::binary_function< const GradeInfo&, const GradeInfo&, bool >
{
public:
	bool operator()( const GradeInfo &lhs , const GradeInfo &rhs ) const
	{
		if( lhs.m_iPriorityOrder < rhs.m_iPriorityOrder )
			return true;
		else if ( lhs.m_iPriorityOrder == rhs.m_iPriorityOrder )
		{
			if ( lhs.m_iValue2 > rhs.m_iValue2 )
				return true;
		}

		return false;
	}
};