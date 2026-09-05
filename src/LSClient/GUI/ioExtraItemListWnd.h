#pragma once

#include "../ioExtraItemInfoManager.h"
#include "../io3DEngine/ioMovingWnd.h"

#include "NewShopIconBtn.h"

///////////////////////////////////////////////////////////////////
class ItemListIconBtn : public NewShopIconBtn
{
protected:
	StringVector m_szNameList;
	int          m_iItemCode;
	
protected:
	virtual void OnRender();

protected:
	void SplitDescription( const ioHashString &szName );

public:
	void SetName( const ioHashString &rName);
	void SetItemCode(int iItemCode) { m_iItemCode = iItemCode; }

public:
	ItemListIconBtn();
	virtual ~ItemListIconBtn();
};
///////////////////////////////////////////////////////////////////
class ShopExtraItemSort : public std::binary_function< const ItemListInfoSort& , const ItemListInfoSort&, bool >
{
public:
	bool operator() ( const ItemListInfoSort &lhs, const ItemListInfoSort &rhs ) const
	{
		if( lhs.m_iOrder > rhs.m_iOrder )
			return true;
		if( lhs.m_iOrder == rhs.m_iOrder )
		{
			if( lhs.m_iItemCode > rhs.m_iItemCode )
				return true;
		}

		return false;
	}
};
///////////////////////////////////////////////////////////////////
class ioExtraItemListWnd : public ioWnd
{
public:
	enum
	{
		ID_PRE_BTN	= 1,
		ID_NEXT_BTN	= 2,
		ID_X		= 3,

		ID_ALL_BTN		= 10,
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
	};

protected:
	int m_iMaxPage;
	int m_iCurPage;
	int m_iMachineCode;
	DWORD m_dwCurSubBtn;

	ioHashString m_szPageText;

	ItemListInfoSortVector m_vCurList;

public:
	void ShowItemList( int iMachineCode );

protected:
	void UpdatePage( int iPage );
	void ChangeSubBtn( DWORD dwSubBtn );

	void UpdateIconBtn();

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnRender();

public:
	ioExtraItemListWnd();
	virtual ~ioExtraItemListWnd();
};
////////////////////////////////////////////////////////////////////////////////////////////////
