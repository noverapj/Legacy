#pragma once

#include "../ioExtraItemInfoManager.h"
#include "../io3DEngine/ioButton.h"


struct DecoItemListInfo
{
	int m_iTypeCode;
	ioHashString m_Name;
	ioHashString m_Icon;
	int          m_iShopOrder;    // 상점에서 정렬 순서
	int          m_iShopMarkType; // SHOP_MARK_TYPE_NONE = 0, SHOP_MARK_TYPE_SALE = 1, SHOP_MARK_TYPE_NEW = 2, SHOP_MARK_TYPE_HIT = 3, SHOP_MARK_TYPE_EVENT = 4,

	DecoItemListInfo()
	{
		m_iTypeCode     = 0;
		m_iShopOrder    = 999999999; // ORDER_DEFAULT 
		m_iShopMarkType = 0;         // SHOP_MARK_TYPE_NONE
	}
};
typedef std::vector< DecoItemListInfo > DecoItemListInfoVector;

///////////////////////////////////////////////////////////////////
class DecoItemListSort : public std::binary_function< const DecoItemListInfo& , const DecoItemListInfo&, bool >
{
public:
	bool operator() ( const DecoItemListInfo &lhs, const DecoItemListInfo &rhs ) const
	{
		if( lhs.m_iShopOrder < rhs.m_iShopOrder )
			return true;
		if( lhs.m_iShopOrder == rhs.m_iShopOrder )
		{
			if( lhs.m_iTypeCode < rhs.m_iTypeCode )
				return true;
		}

		return false;
	}
};

///////////////////////////////////////////////////////////////////
class ioDecoItemListWnd : public ioWnd
{
public:
	enum
	{
		ID_PRE_BTN	= 1,
		ID_NEXT_BTN	= 2,
		ID_X		= 3,

		ID_ALL_BTN         = 10,
		ID_KINDRED_BTN     = 11,
		ID_HAIR_BTN        = 12,
		ID_HAIR_COLOR_BTN  = 13,
		ID_FACE_BTN        = 14,
		ID_SKIN_COLOR_BTN  = 15,
		ID_UNDERWEAR_BTN   = 16,

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
	DecoItemListInfoVector m_vMaleDecoInfoList;
	DecoItemListInfoVector m_vFemaleDecoInfoList;

	int m_iMaxPage;
	int m_iCurPage;

	bool m_bFemale;

	DWORD m_dwCurSubBtn;

	ioHashString m_szPageText;

	DecoItemListInfoVector m_vCurList;

	ioHashString m_szEmptyIcon;
	ioHashString m_szDesc1;
	ioHashString m_szDesc2;

public:
	void ShowItemList( bool bFeMale );

protected:
	void UpdatePage( int iPage );
	void ChangeSubBtn( DWORD dwSubBtn );

	void UpdateIconBtn();

public:
	virtual void iwm_create();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnRender();

public:
	ioDecoItemListWnd();
	virtual ~ioDecoItemListWnd();
};
////////////////////////////////////////////////////////////////////////////////////////////////
