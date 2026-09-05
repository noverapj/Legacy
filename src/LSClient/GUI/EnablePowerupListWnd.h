#pragma once
#include "NewShopIconBtn.h"

///////////////////////////////////////////////////////////////////
class PowerUpToolTip : public ioWnd
{
public:
	enum
	{
		Power_Char,
		Power_Item,
	};

public:
	IoString m_szToolTip;

public:
	void SetInfo( int nType, int iIndex );

public:
	virtual void OnRender();

protected:
	virtual void OnProcess( float fTimePerSec );

protected:
	void SetCharInfo( int nClassType );
	void SetItemInfo( int nItemCode );

public:
	PowerUpToolTip();
	virtual ~PowerUpToolTip();
};

///////////////////////////////////////////////////////////////////
class EnablePowerupListIconBtn : public NewShopIconBtn
{
protected:
	StringVector m_szNameList;
	int          m_iItemCode;
	int			 m_iType;
protected:
	virtual void OnRender();

protected:
	void SplitDescription( const ioHashString &szName );

public:
	void SetName( const ioHashString &rName);
	void SetItemCode(int iItemCode ) { m_iItemCode = iItemCode; }
	const int  GetItemCode()	const { return m_iItemCode; }
	

public:
	EnablePowerupListIconBtn();
	virtual ~EnablePowerupListIconBtn();
};
///////////////////////////////////////////////////////////////////
struct PowerupListInfo
{
	int m_iData;
	int m_iShopMarkType; // SHOP_MARK_TYPE_NONE = 0, SHOP_MARK_TYPE_SALE = 1, SHOP_MARK_TYPE_NEW = 2, SHOP_MARK_TYPE_HIT = 3, SHOP_MARK_TYPE_EVENT = 4,
	int m_iOrder;

	PowerupListInfo()
	{
		m_iData     = 0;
		m_iShopMarkType = SHOP_MARK_TYPE_NONE;
		m_iOrder;
	}
};
typedef std::vector< PowerupListInfo > PowerupListInfoVector;
//////////////////////////////////////////////////////////////////
class EnablePowerupListWnd :public ioWnd
{
public:
	enum
	{
		ID_PRE_BTN	= 1,
		ID_NEXT_BTN	= 2,
		ID_X		= 3,

		ID_HERO_BTN		= 10,
		ID_WEAPON_BTN	= 11,
		ID_ARMOR_BTN	= 12,
		ID_HELMET_BTN	= 13,
		ID_CLOAK_BTN	= 14,
		
		ID_ICON_BTN1	= 100,
		ID_ICON_BTN2	= 101,
		ID_ICON_BTN3	= 102,
		ID_ICON_BTN4	= 103,
		ID_ICON_BTN5	= 104,
		ID_ICON_BTN6	= 105,
		ID_ICON_BTN7	= 106,
		ID_ICON_BTN8	= 107,
		ID_ICON_BTN9	= 108,
		ID_ICON_BTN10	= 109,
		ID_ICON_BTN11	= 110,
		ID_ICON_BTN12	= 111,

		ICON_BTN_MAX	= 12,

		ID_POWERUP_TOOLTIP = 150,
	};

	enum
	{
		MAX_TEXT    = 2,
	};

protected:
	int m_iMaxPage;
	int m_iCurPage;
	DWORD m_dwCurSubBtn;

	ioHashString m_szPageText;
	PowerupListInfoVector m_vCurList;
	ioComplexStringPrinter m_UnderLinePrinter[MAX_TEXT];
	ioComplexStringPrinter m_PagePrinter;
public:
	void ShowItemList();

protected:
	void UpdatePage( int iPage );
	void ChangeSubBtn( DWORD dwSubBtn );
	void SettingText();

	void UpdateIconBtn();
	void SetItemList( const IntVec EnablePowerUpNumVec, PowerupListInfoVector &vItemList );

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
protected:
	virtual void OnRender();
public:
	EnablePowerupListWnd(void);
	virtual ~EnablePowerupListWnd(void);
};

class EnablePowerupListSort : public std::binary_function< const PowerupListInfo& , const PowerupListInfo&, bool >
{
public:
	bool operator() ( const PowerupListInfo &lhs, const PowerupListInfo &rhs ) const
	{
		if( lhs.m_iOrder > rhs.m_iOrder )
			return true;
		if( lhs.m_iOrder == rhs.m_iOrder )
		{
			if( lhs.m_iData < rhs.m_iData )
				return true;
		}

		return false;
	}
};