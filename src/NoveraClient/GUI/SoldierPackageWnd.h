#pragma once

#include "NewShopBuyWnd.h"

struct PackageItem
{
	ioHashString m_szName;
	int          m_iMagicCode; 
	bool         m_bKeepItem;
	bool         m_bBasicItem;
	int			 m_nGradeType;
	PackageItem()
	{
		m_iMagicCode = 0;
		m_bKeepItem  = false;
		m_bBasicItem = false;
		m_nGradeType = 0;
	}
};
typedef std::vector< PackageItem > vPackageItem;

class PackageItemSort : public std::binary_function< const PackageItem& , const PackageItem&, bool >
{
public:
	bool operator() ( const PackageItem &lhs, const PackageItem &rhs ) const
	{
		if( lhs.m_iMagicCode > rhs.m_iMagicCode )
			return true;
		return false;
	}
};
//////////////////////////////////////////////////////////////////////////
class PackageItemBtn : public ioButton
{
protected:
	PackageItem m_PackageItem;
	int         m_iKeepAndBasicItemBonus;

	ioComplexStringPrinter m_Title;

public:
	void SetPackageItem( int iSexType, PackageItem &rkItem, int iPackageType );

public:
	inline ioHashString GetItemName(){ return m_PackageItem.m_szName; }
	inline int GetMagicCode(){ return m_PackageItem.m_iMagicCode; }
	inline bool IsKeepItem(){ return m_PackageItem.m_bKeepItem; }
	inline bool IsBasicItem(){ return m_PackageItem.m_bBasicItem; }
	inline int GetKeepAndBasicItemBonus(){ return m_iKeepAndBasicItemBonus; }

protected:
	virtual void OnRender();

public:
	PackageItemBtn();
	virtual ~PackageItemBtn();
};
//////////////////////////////////////////////////////////////////////////
class PackageItemBuyWnd : public ioWnd
{
public:
	enum
	{
		ID_BUY    = 1,

		ID_CHAR_SELECT_BTN	     = 101,
		ID_KINDRED_SELECT_BTN    = 102,
		ID_HAIR_SELECT_BTN	     = 103,
		ID_HAIR_COLOR_SELECT_BTN = 104,
		ID_FACE_SELECT_BTN       = 105,
		ID_SKIN_COLOR_SELECT_BTN = 106,
		ID_UNDERWEAR_SELECT_BTN  = 107,

		ID_SOLDIER_WND  = 1000,
		ID_PULLDOWN_WND = 1001,
	};

	enum PackageStyle
	{
		PACKAGE_SOLDIER         = 1,
		PACKAGE_DECORATION      = 2,
		PACKAGE_DECO_FACE       = 3,  // 아직 권한 아이템은 미구현됨
		PACKAGE_DECO_HAIR       = 4,  // 아직 권한 아이템은 미구현됨
		PACKAGE_DECO_SKIN_COLOR = 5,  // 아직 권한 아이템은 미구현됨
		PACKAGE_DECO_HAIR_COLOR = 6,  // 아직 권한 아이템은 미구현됨
		PACKAGE_DECO_KINDRED    = 7,  // 아직 권한 아이템은 미구현됨
		PACKAGE_DECO_UNDERWEAR  = 8,
		PACKAGE_PREMIUM_SOLDIER = 9,

	};

protected:
	PackageStyle	m_PackageStyle;
	CEncrypt<DWORD>	m_dwEtcItemType;
	CEncrypt<int>	m_iActiveFilter;

	ManualMgr* m_pManual;
	
protected:
	CHARACTER m_CharInfo;
	CHARACTER m_OverCharInfo;

protected:		
	void GetPackageItemList( DWORD dwID, int iSex, vPackageItem &rkList );
	void GetClassList( vPackageItem &rkList );
	void GetDecorationList( int iSearchSexType, int iSearchDecoType, vPackageItem &rkList );

	void ConvertItemList( IN vPackageItem &rkPackageList, OUT vNewShopPullDownItem &rkList );

protected:
	void SetPullDownMenu( ioWnd *pOpenBtn );
	void SetSelectItemBtn( DWORD dwID, CHARACTER &rkCharInfo, PackageItem &rkItem );
	void SetSelectCharInfo( DWORD dwID, CHARACTER &rkCharInfo, PackageItem &rkItem );
	void DefaultDecoration( bool bChangekindred, bool bSetBtn, CHARACTER &rkCharInfo );
	void DefaultDecoration( DWORD dwID, int iDecoType, CHARACTER &rkCharInfo, PackageItem &rkReturnItem );

protected:
	void PullDownBtnUP( ioWnd *pOpenBtn, int iListArray );
	void PullDownBtnOver( ioWnd *pOpenBtn, int iListArray );

protected:
	int GetKeepAndBasicItemBonus();
	bool IsRightDecoList( int iDecoType );

	void PrintManual( int a_iXPos, int a_iYPos , float fScale );

public:
	virtual void iwm_show();
	virtual void iwm_hide();

public:
	virtual void iwm_create();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

public:
	void ShowPackage( PackageStyle ePackageStyle, DWORD dwType, int eActiveFilter = 1 );
	
	static bool IsBought( int iClassType, int iSexType, int iDecoType, int iDecoCode );

protected:
	virtual void OnProcess( float fTimePerSec );

public:
	PackageItemBuyWnd();
	virtual ~PackageItemBuyWnd();
};
//////////////////////////////////////////////////////////////////////////
class PackageItemBuyAgreeWnd : public ioWnd
{
public:
	enum
	{
		ID_EXIT  = 1,
		ID_AGREE = 2,
		ID_CANCEL= 3,
	};

	enum
	{
		MAX_AGREE_LIST	= 8,
	};

protected:
	ioUIRenderImage *m_pCharIcon;
	ioUIRenderImage *m_pCharSubIcon;
	ioUIRenderImage *m_pBackEffect;

protected:
	CHARACTER  m_CharInfo;
	PackageItemBuyWnd::PackageStyle m_PackageStyle;
	CEncrypt<DWORD> m_dwEtcItemType;

	ioComplexStringPrinter m_AgreeList[MAX_AGREE_LIST];
	ioComplexStringPrinter m_PesoList[MAX_AGREE_LIST];

	int m_nGradeType;

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

public:
	void SetAgreeSoldierPackage( CHARACTER &rkCharInfo, PackageItemBuyWnd::PackageStyle ePackageStyle, DWORD dwEtcType );
	void SetAgreeDecorationPackage( CHARACTER &rkCharInfo , PackageItemBuyWnd::PackageStyle ePackageStyle, DWORD dwEtcType );
	
protected:
	int GetDecoKeepAndBasicCheck( int iDecoType, int iDecoCode, ioHashString szDecoName, char *pReturn );
	bool FillPackageSet( SP2Packet &rkPacket );
	void SendSoldierPackage();
	void SendDecorationPackage();
    bool IsRightDecoType( int iDecoType );

protected:
	virtual void OnRender();

public:
	PackageItemBuyAgreeWnd();
	virtual ~PackageItemBuyAgreeWnd();
};
//////////////////////////////////////////////////////////////////////////
class PackageItemBuyResultWnd : public ioWnd
{
public:
	enum
	{
		ID_GO	= 1,
	};

	enum
	{
		MAX_AGREE_LIST	= 8,
	};

protected:
	ioUIRenderImage *m_pCharIcon;
	ioUIRenderImage *m_pCharSubIcon;
	ioUIRenderImage *m_pBackEffect;

	int m_iClassType;
	ioHashString m_TitleName;
	ioComplexStringPrinter m_AgreeList[MAX_AGREE_LIST];

	int m_nGradeType;

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

public:
	void SetResultSoldierPackage( CHARACTER &rkCharInfo, int iKeepBonusPeso );
	void SetResultDecorationPackage( CHARACTER &rkCharInfo, int iKeepBonusPeso );

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void OnRender();

public:
	PackageItemBuyResultWnd();
	virtual ~PackageItemBuyResultWnd();
};