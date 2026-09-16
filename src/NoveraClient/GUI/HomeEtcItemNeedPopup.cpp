#include "stdafx.h"

#include "HomeEtcItemNeedPopup.h"

HomeEtcItemNeedPopup::HomeEtcItemNeedPopup()
{
	m_pIconBack = NULL;
	m_pIcon		= NULL;
}

HomeEtcItemNeedPopup::~HomeEtcItemNeedPopup()
{
	SAFEDELETE( m_pIconBack );
	SAFEDELETE( m_pIcon );
}

void HomeEtcItemNeedPopup::iwm_show()
{
}

void HomeEtcItemNeedPopup::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_EXIT:
	case ID_CANCEL:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
		}
		break;
	case ID_CREATE_ITEM_BUY:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
			NewShopWnd *pNewShopWnd = dynamic_cast<NewShopWnd*>( g_GUIMgr.FindWnd( NEW_SHOP_WND ) );
			if( pNewShopWnd )
			{
				pNewShopWnd->ShowWnd();
				pNewShopWnd->SetDirectTab( NewShopWnd::ID_ETC_TAB_BTN, ioEtcItem::EIT_ETC_CREATE_HOME, 0 );
			}
		}
		break;
	}
}

void HomeEtcItemNeedPopup::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "IconBack" )
	{
		SAFEDELETE( m_pIconBack );
		m_pIconBack = pImage;
	}
	else if( szType == "Icon" )
	{
		SAFEDELETE( m_pIcon );
		m_pIcon = pImage;
	}
	else 
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void HomeEtcItemNeedPopup::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( m_pIconBack )
		m_pIconBack->Render( iXPos + 151, iYPos + 98, UI_RENDER_MULTIPLY );

	if( m_pIcon )
		m_pIcon->Render( iXPos + 151, iYPos + 98 );

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );

	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 151, iYPos + 144, FONT_SIZE_13, STR(1) );

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_RED );	
	g_FontMgr.PrintText( iXPos + 151, iYPos + 164, FONT_SIZE_13, STR(2) );

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );	
	g_FontMgr.PrintText( iXPos + 151, iYPos + 184, FONT_SIZE_13, STR(3) );

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );	
	g_FontMgr.PrintText( iXPos + 151, iYPos + 204, FONT_SIZE_13, STR(4) );

}