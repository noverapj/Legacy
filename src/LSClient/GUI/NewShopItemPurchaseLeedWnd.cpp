#include "StdAfx.h"
#include "NewShopItemPurchaseLeedWnd.h"

NewShopItemPurchaseLeedWnd::NewShopItemPurchaseLeedWnd()
{
	m_pIcon = NULL;
	m_pSubIcon = NULL;
	m_dwEtcitemType = 0;
}

NewShopItemPurchaseLeedWnd::~NewShopItemPurchaseLeedWnd()
{
	SAFEDELETE( m_pIcon );
	SAFEDELETE( m_pSubIcon );
}

bool NewShopItemPurchaseLeedWnd::SetInfo( DWORD dwEtcitemType, const ioHashString &rsIcon, const ioHashString &rsSubIcon, const ioHashString &rsTitle, const ioHashStringVec &rvDescVec )
{
	m_dwEtcitemType = 0;
	ioEtcItem *pEtcItem = g_EtcItemMgr.FindEtcItem( dwEtcitemType );
	if ( !pEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s Etc item code is not Exist : %d", __FUNCTION__, dwEtcitemType );
		return false;
	}

	m_dwEtcitemType = dwEtcitemType;
	if( rsIcon.IsEmpty() )
	{
		LOG.PrintTimeAndLog( 0, "%s icon is empty", __FUNCTION__ );
		return false;
	}

	SAFEDELETE( m_pIcon );
	SAFEDELETE( m_pSubIcon );
	m_pIcon = g_UIImageSetMgr.CreateImageByFullName( rsIcon );
	m_pSubIcon = g_UIImageSetMgr.CreateImageByFullName( rsSubIcon );

	m_vDescVec.clear();
	m_vDescVec = rvDescVec;
	SetTitleText( rsTitle.c_str() );

	return true;
}

void NewShopItemPurchaseLeedWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_EXIT:
	case ID_CANCEL:
		if( cmd == IOBN_BTNUP )
			HideWnd();
		break;
	case ID_SHOP:
		if( cmd == IOBN_BTNUP )
		{
			// 상점 특별아이템 열기
			NewShopWnd *pNewShopWnd = dynamic_cast<NewShopWnd*>( g_GUIMgr.FindWnd( NEW_SHOP_WND ) );
			if( pNewShopWnd )
			{
				pNewShopWnd->ShowWnd();
				pNewShopWnd->SetDirectTab( NewShopWnd::ID_ETC_TAB_BTN, m_dwEtcitemType, 0 );
				HideWnd();
			}
		}
		break;		
	}
}


void NewShopItemPurchaseLeedWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	enum
	{
		ICON_X_OFFSET = 151,
		ICON_Y_OFFSET = 98,

		DESC_X_OFFSET = 151,
		DESC_Y_OFFSET = 144,
		DESC_Y_GAP    = 20,
	};

	if( m_pIcon )
		m_pIcon->Render( iXPos + ICON_X_OFFSET, iYPos + ICON_Y_OFFSET );
	if( m_pSubIcon )
		m_pSubIcon->Render( iXPos + ICON_X_OFFSET, iYPos + ICON_Y_OFFSET );

	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );

	int iSize = m_vDescVec.size();
	for (int i = 0; i < iSize ; i++)
	{
		if( i == 0 )
			g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
		else
			g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );

		ioHashString &rsDesc = m_vDescVec[i];
		g_FontMgr.PrintText( iXPos + DESC_X_OFFSET, iYPos + DESC_Y_OFFSET + (DESC_Y_GAP*i), FONT_SIZE_13, rsDesc.c_str() );
	}
}
