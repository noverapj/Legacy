#pragma once

#include "../ioPowerUpManager.h"

class PowerUpInfo
{
public:
	//해당 정보가 아이템 정보인지 영웅 정보인지
	int   m_iType;
	int	  m_iTabType;
	//char의 경우 class type, item의 경우 code
	int   m_iValue;
	//아이템 강화 정보( 아이템 강화 수치 )
	int   m_iValue2;
	
	//우선순위
	int   m_iPriorityOrder;

	ioHashString m_szName;
	ioHashString m_szSubDesc;
	ioHashString m_szWeaponDesc;
	ioHashString m_szIconName;
	ioHashString m_szSubIconName;

public:
	void Clear()
	{
		m_iType = -1;
		m_iTabType = -1;
		m_iValue = 0;
		m_iValue2 = 0;
		m_iPriorityOrder = 0;
		m_szName.Clear();
		m_szSubDesc.Clear();
		m_szIconName.Clear();
		m_szSubIconName.Clear();
		m_szWeaponDesc.Clear();
	}
	bool IsEmpty()
	{
		if ( m_iType == -1 && m_iTabType == -1 && m_iValue == 0 && m_iValue2 == 0 && m_iPriorityOrder == 0 )
			return true;
		return false;
	}

public:
	PowerUpInfo()
	{
		Clear();
	}
	~PowerUpInfo()	{ }
};
typedef std::vector< PowerUpInfo > vPowerUpInfo;

class PowerUpSelectWnd : public ioWnd
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
		ID_TAB_6_BTN	 = 106, //

		ID_TAB_7_BTN	 = 107, //
		ID_TAB_8_BTN	 = 108, //
		ID_TAB_9_BTN	 = 109, //
		ID_TAB_10_BTN	 = 110, //
		ID_TAB_11_BTN	 = 111, //
		ID_TAB_COUNT_MAX = 10,
	};

	enum 
	{
		MAX_HERO_TAB_BTN		= 6,
		MAX_ITEM_TAB_BTN		= 5,

		MAX_ITEM_INFO_WND_BTN	= 8,
		PAGE_PER_ITEM_NUM		= 4,
	};

protected:
	ioPowerUpManager::PowerUpInfoType m_InfoType;
	int m_iTabLineSize;
	int m_iTabPosX;
	int m_iCurPage;

	DWORD m_dwCurTabID;

	ioUIRenderImage *m_pDotLine;
	ioUIRenderFrame *m_pEditFrm;
	ioComplexStringPrinter m_TitlePrinter;
protected:
	ioHashString m_szCurrFindKeyword;

	vPowerUpInfo m_SelectPowerUpInfoVec;
	vPowerUpInfo m_CurrSelectPowerUpInfoVec;
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

	void SetAllItemInfoVec();
	void SetCurItemInfoVec( DWORD dwTabID, const char *szFind );

	bool IsRightTab( DWORD dwTabID, int iType );
	void SetFind();

public:
	void ShowSubWnd( int iPowerUpInfoType );

public:
	void Update();

public:
	PowerUpSelectWnd();
	virtual ~PowerUpSelectWnd();
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class PowerUpSelectInfoWnd : public ioButton
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
	PowerUpInfo m_PowerUpInfo;
	ioUIRenderImage *m_pEmptyImg;
	int m_nGradeType;

protected:	
	void ClearDesc();
	void RenderDesc( int iXPos, int iYPos );

	bool SetInfo();

public:
	void SetBlank();
	void SetPowerUpInfo( const PowerUpInfo& rkItem );
	void SetItem( );

protected:
	virtual void OnRender();

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

public:
	PowerUpSelectInfoWnd();
	virtual ~PowerUpSelectInfoWnd();
};
/////////////////////////////////////////////////////////////////////////////////////////////////////
class PowerUpSelectIconBtn : public ioButton
{
protected:
	ioUIRenderImage *m_pIcon;
	ioUIRenderImage *m_pSubIcon;
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
	bool SetSubIcon( ioUIRenderImage *pSubIcon );
	void SetGradeType( int nGradeType ){ m_nGradeType = nGradeType; }

public:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	

protected:
	virtual void OnDrawNormal( int iXPos, int iYPos );
	virtual void OnDrawOvered( int iXPos, int iYPos );
	virtual void OnDrawPushed( int iXPos, int iYPos );
	virtual void OnDrawDisabled( int iXPos, int iYPos );

public:
	PowerUpSelectIconBtn();
	virtual ~PowerUpSelectIconBtn();
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class PowerUpCategoryTab : public ioRadioButton 
{
protected:
	TabUISetting m_Setting;

public:
	void ClearTab();
	void SetTab( const TabUISetting& rkTab );
	const TabUISetting& GetTab(){ return m_Setting; }

public:
	PowerUpCategoryTab();
	virtual ~PowerUpCategoryTab();
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class PowerUpInfoSortByPriority : public std::binary_function< const PowerUpInfo&, const PowerUpInfo&, bool >
{
public:
	bool operator()( const PowerUpInfo &lhs , const PowerUpInfo &rhs ) const
	{
		if( lhs.m_iPriorityOrder < rhs.m_iPriorityOrder )
			return true;
		else if ( lhs.m_iPriorityOrder == rhs.m_iPriorityOrder )
		{
			if ( lhs.m_iValue < rhs.m_iValue )
				return true;
		}

		return false;
	}
};