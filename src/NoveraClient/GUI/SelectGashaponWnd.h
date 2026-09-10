#pragma once
#include "../ioGashaponMgr.h"

class SelectGashaponBtn : public ioButton
{
public:
	enum AniType
	{
		AT_NONE       = 0,
		AT_BACK_WHITE = 1,
		AT_BACK_WAIT  = 2,
		AT_OUT        = 3, // 감속
		AT_IN         = 4, // 가속
		AT_WHITE      = 5,
		AT_END        = 6,
		AT_BACK_OUT   = 7,
		AT_BACK_IN    = 8,
		AT_BACK_END   = 9,
	};

	enum
	{
		MAX_SPLIT_LINE = 2,
	};

protected:
	ioUIRenderImage *m_pCardCover;
	ioUIRenderImage *m_pCoverOverAdd;
	ioUIRenderFrame *m_pSelect;
	ioUIRenderFrame *m_pSelectEdge;
	ioUIRenderFrame *m_pLight;

protected:
	ioUIRenderImage *m_pPresentIcon;
	ioUIRenderImage *m_pSubIcon;

protected:
	GashaponPresent m_GashaponPresent;

protected:
	FSMStateMgr m_AniState;

	DWORD m_dwAniInDelay;
	DWORD m_dwAniOutDelay;
	DWORD m_dwAniWhiteDelay;
	DWORD m_dwAniEndDelay;
	DWORD m_dwAniBackWhiteDelay;
	DWORD m_dwAniBackWaitDelay;
	DWORD m_dwAniCardMaxPercent;

	bool  m_bGetCard;
	bool  m_bSelect;

	ioUIRenderImage *m_pEffect;

	ioHashStringVec m_vEndEfffect_S;
	int m_iCurEffectArray;
	DWORD m_dwRollingEffectTime;
	DWORD m_dwRollingEffectCheckTime;

	ioHashString m_szSuccessSound;

	ioHashString m_szTitles[MAX_SPLIT_LINE];

protected:
	void ProcessAni();
	void InitAniState();
	void RollingEffect( float fTimePerSec );
	void RenderInfo(int iXPos, int iYPos, UIRenderType eType );
	void RenderCover( int iXPos, int iYPos, UIRenderType eType );
	void RenderCoverWhite( int iXPos, int iYPos, UIRenderType eType );
	void RenderCoverOverAdd( int iXPos, int iYPos, UIRenderType eType );
	void RenderEffect( int iXPos, int iYPos, UIRenderType eType );
	void RenderSelect( int iXPos, int iYPos, UIRenderType eType );
	void RenderLight( int iXPos, int iYPos, UIRenderType eType );

	void SplitText( const char *szText, float fSplitWidth  , OUT char szSplitText[MAX_SPLIT_LINE][MAX_PATH]);
	bool CheckHanByte( char *szText );

public:
	virtual void iwm_create();
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void ParseExtraInfo( ioXMLElement &xElement );

protected:
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

protected:
	virtual void OnProcess( float fTimePerSec );

protected:
	virtual void OnDrawNormal( int iXPos, int iYPos );
	virtual void OnDrawOvered( int iXPos, int iYPos );
	virtual void OnDrawPushed( int iXPos, int iYPos );
	virtual void OnDrawDisabled( int iXPos, int iYPos );

protected:
	void OnRenderPresent( int iXPos, int iYPos );

public:
	void SetPresent( const GashaponPresent& rkPresent );

public:
	bool IsEmptyCard(){ return m_GashaponPresent.IsEmpty(); }
	void SetEmptyCard();
	void SetOutAni();
	void SetNoneAni();
	void SetBackWhiteAni();
	void SetBackOutAni();
	const GashaponPresent& GetPresent(){ return m_GashaponPresent; }

	void SetGetCard( bool bGetCard );
	bool IsGetCard() { return m_bGetCard; }

	void SetSelect( bool bSelect );
	bool IsSelect() { return m_bSelect; }
	void SetGetEffect();

public:
	SelectGashaponBtn();
	virtual ~SelectGashaponBtn();
};

///////////////////////////////////////////////////////////////////////////
class SelectGashaponWnd : public ioWnd
{
public:
	enum
	{
		ID_EXIT   = 1,
		ID_BUY    = 2,
		ID_CANCEL = 3,
		ID_WAITING= 4,
		ID_AGAIN  = 5,
		ID_GO     = 6,
		ID_OK	  = 7,

		ID_ITEM_1  = 1001,
		ID_ITEM_2  = 1002,
		ID_ITEM_3  = 1003,
		ID_ITEM_4  = 1004,
		ID_ITEM_5  = 1005,
		ID_ITEM_6  = 1006,
		ID_ITEM_7  = 1007,
		ID_ITEM_8  = 1008,
		ID_ITEM_9  = 1009,
		ID_ITEM_10 = 1010,
	};

	enum 
	{
		MAX_ITEM_BTN = 10,
		MAX_SELECT   = 10,
	};

	enum AniType
	{
		AT_NONE  = 0,
		AT_WAIT  = 1,
		AT_SOUND = 2,
		AT_ROLL  = 3, 
		AT_CHANGE= 4,
		AT_END   = 5,
	};

	enum UseType
	{
		UT_NONE,
		UT_BUY_BTN, // 상점에서 구매
		UT_USE_BTN, // 선물받은 가챠 사용
	};

protected:	
	vGashaponPresent m_SelectGashaponPresent;
	ioHashString m_sDescCard;
	
	ioUIRenderImage *m_pLineLeft;
	ioUIRenderImage *m_pLineCenter;
	ioUIRenderImage *m_pLineRight;

	int m_iSelectItemBtnID;

	ioHashString m_szTurnSound;
	ioHashString m_szRollingSound;

	FSMStateMgr m_AniState;

	DWORD m_dwAniWaitDelay;
	DWORD m_dwAniRollDelay;
	DWORD m_dwAniRollCnt;
	DWORD m_dwCurAniRollCnt;

	int   m_iMaxSelectItem;
	int   m_iEtcItemType;

	ioHashString m_szMent1;
	ioHashString m_szMent2;

	// 선택가챠 내아이템에서 사용가능하게끔 수정!
	UseType m_UseType;

protected:
	vGashaponPresent m_vRandomList;

public:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	

protected:
	void CheckBuy();
	void CheckUse();
	void SetTitleBuyBtn( int iCashArray );
	void InitAniState();
	void ProcessAni();
	void Init( bool bAni );

public:
	void SendBuy();
	void SendUse();
	void SetSelectItem( const GashaponPresent& rkPresent );
	void SetResult( const GashaponPresent& rkResultPresent );
	void SetEmptyCardAni();

public:
	virtual void iwm_create();
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void ParseExtraInfo( ioXMLElement &xElement );

public:
	void ShowSelectGashaponWnd( int iEtcItemType, UseType usetype );

public:
	const GashaponPresent& GetPresent( int iSelectIndex );
	inline int GetSelectGashaponCode(){ return m_iEtcItemType; }

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

protected:
	bool	CheckExamination();	//  아이템 검사 2020-03-26
	bool	CheckPresentLimited( const GashaponPresent& rkPresent, int nPresentLimitedQuantity );	// 유니크 체크 제한 2020-03-26

public:
	SelectGashaponWnd(void);
	virtual ~SelectGashaponWnd(void);
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class SelectGashaponItemInfoWnd : public ioButton
{
public:
	enum 
	{
		ID_ICON_BTN			= 1,
		ID_SELECT_BTN		= 2,
	};

	enum
	{
		MAX_DESC	= 2,
	};

protected:
	bool m_bWidthCutDescArray[MAX_DESC];
	ioComplexStringPrinter m_szDescArray[MAX_DESC];
	GashaponPresent m_GashaponItem;
	ioUIRenderImage *m_pEmptyImg;

protected:	
	void ClearDesc();
	void RenderDesc( int iXPos, int iYPos );

	void SetIsWidthCutDesc( int iArray , bool bWidthCut );
	bool IsWidthCutDesc( int iArray );
		
	bool SetInfo();

public:
	void SetBlank();
	void SetGashaponItem( const GashaponPresent& rkItem );
	inline const GashaponPresent& GetGashaponItem() const { return m_GashaponItem; }

	bool IsIconOver();

protected:
	virtual void OnRender();

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

public:
	SelectGashaponItemInfoWnd();
	virtual ~SelectGashaponItemInfoWnd();
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class GashaponCategoryTab : public ioRadioButton 
{
protected:
	TabUISetting m_Setting;

public:
	void ClearTab();
	void SetTab( const TabUISetting& rkTab );
	const TabUISetting& GetTab(){ return m_Setting; }

public:
	GashaponCategoryTab();
	~GashaponCategoryTab();
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class SelectGashaponSubWnd : public ioWnd
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
		ID_TAB_COUNT_MAX = 5,
	};

	enum 
	{
		MAX_TAB_BTN				= 5,

		MAX_ITEM_INFO_WND_BTN	= 8,
		PAGE_PER_ITEM_NUM		= 4,
	};

protected:
	vGashaponPresent m_SelectItemInfoVec;
	vGashaponPresent m_SelectCurrVec;
	
	int m_iTabLineSize;
	int m_iTabPosX;
	int m_iCurPage;

	DWORD m_dwCurTabID;

	ioUIRenderImage *m_pDotLine;
	ioUIRenderFrame *m_pEditFrm;

protected:
	ioHashString m_szCurrFindKeyword;

public:
	void LoadINI( int iGashaponIndex );
	void LoadGashaponList( ioINILoader& rkLoader, int iGashaponIndex );

public:
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

	virtual void iwm_vscroll(DWORD dwID, int curPos );
	virtual void iwm_wheel( int zDelta );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

protected:
	void ChangeTab( DWORD dwID, const char *szFind );
	void UpdatePage( int iNewPage );
		
	void SetAllItemInfoVec( DWORD dwTabID, const char *szFind );

	bool IsRightTab( DWORD dwTabID, int iType, int iValue1, int iValue2, int iSpecialAddType );
	void SetFind();

public:
	void ShowSubWnd( int iEtcItemType );

public:
	void Update();

public:
	SelectGashaponSubWnd();
	virtual ~SelectGashaponSubWnd();
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class SelectGashaponItemIconBtn : public ioButton
{
protected:
	ioUIRenderImage *m_pIcon;
	ioUIRenderImage *m_pSubIcon;
	ioUIRenderImage *m_pIconBack;

	float m_fScale;
	float m_fBackScale;


protected:
	void RenderIcon(int iXPos, int iYPos, UIRenderType eType);
	void RenderBack(int iXPos, int iYPos );

public:
	bool SetIcon( ioUIRenderImage *pIcon, float fScale, float fBackScale );
	void SetSubIcon( ioUIRenderImage *pIcon );

public:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	

protected:
	virtual void OnDrawNormal( int iXPos, int iYPos );
	virtual void OnDrawOvered( int iXPos, int iYPos );
	virtual void OnDrawPushed( int iXPos, int iYPos );
	virtual void OnDrawDisabled( int iXPos, int iYPos );

protected:
	virtual void OnProcess( float fTimePerSec );	// 2020-04-08 툴팁

public:
	SelectGashaponItemIconBtn();
	virtual ~SelectGashaponItemIconBtn();
};