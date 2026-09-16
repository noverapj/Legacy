#include "stdafx.h"

#include "GuildHQEtcItemNeedPopup.h"

GuildHQEtcItemNeedPopup::GuildHQEtcItemNeedPopup()
{
	m_PopUpType	= PUT_NEED_HQ;
	m_pIconBack = NULL;
	m_pIcon		= NULL;
}

GuildHQEtcItemNeedPopup::~GuildHQEtcItemNeedPopup()
{
	SAFEDELETE( m_pIconBack );
	SAFEDELETE( m_pIcon );
}

void GuildHQEtcItemNeedPopup::SetType( GuildHQEtcItemNeedPopup::PopUpType eType )
{
	m_PopUpType = eType;
}

void GuildHQEtcItemNeedPopup::iwm_show()
{
	switch( m_PopUpType )
	{
	case PUT_NEED_HQ:
		{
			HideChildWnd( ID_USE_ITEM ); 
			ShowChildWnd( ID_CREATE_ITEM_BUY ); 
			SetTitleText( STR(1) );
		}
		break;
	case PUT_USE_HQ:
		{
			HideChildWnd( ID_CREATE_ITEM_BUY );
			ShowChildWnd( ID_USE_ITEM );
			SetTitleText( STR(2) );
		}
		break;
	}
}

void GuildHQEtcItemNeedPopup::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
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
				pNewShopWnd->SetDirectTab( NewShopWnd::ID_ETC_TAB_BTN, ioEtcItem::EIT_ETC_CREATE_GUILD_HQ, 0 );
			}
		}
		break;
	case ID_USE_ITEM:
		if( cmd == IOBN_BTNUP )
		{
			ioUserEtcItem *pUserEtcItem = g_MyInfo.GetUserEtcItem();
			if( pUserEtcItem && pUserEtcItem->IsCanUse( ioEtcItem::EIT_ETC_CREATE_GUILD_HQ ) )
			{
				ioEtcItem *pEtcItem = g_EtcItemMgr.FindEtcItem( ioEtcItem::EIT_ETC_CREATE_GUILD_HQ );
				if( pEtcItem )
					pEtcItem->OnUseBtnPush( ioEtcItem::EIT_ETC_CREATE_GUILD_HQ, pUserEtcItem );
			}
			HideWnd();
		}
	}
}

void GuildHQEtcItemNeedPopup::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
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

void GuildHQEtcItemNeedPopup::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( m_pIconBack )
		m_pIconBack->Render( iXPos + 151, iYPos + 98, UI_RENDER_MULTIPLY );

	if( m_pIcon )
		m_pIcon->Render( iXPos + 151, iYPos + 98 );

	switch( m_PopUpType )
	{
	case PUT_NEED_HQ:
		{
			g_FontMgr.SetTextStyle( TS_NORMAL );
			g_FontMgr.SetAlignType( TAT_CENTER );
			g_FontMgr.SetBkColor( 0, 0, 0 );

			g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
			g_FontMgr.PrintText( iXPos + 151, iYPos + 144, FONT_SIZE_13, STR(1) );

			ioComplexStringPrinter kPrinter;
			kPrinter.SetTextStyle( TS_NORMAL );
			kPrinter.SetBkColor( 0, 0, 0 );
			kPrinter.SetTextColor( TCT_DEFAULT_RED );
			kPrinter.AddTextPiece( FONT_SIZE_13, STR(2));

			kPrinter.SetTextStyle( TS_NORMAL );
			kPrinter.SetBkColor( 0, 0, 0 );
			kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
			kPrinter.AddTextPiece( FONT_SIZE_12, STR(3) );
			kPrinter.PrintFullText( iXPos + 151, iYPos + 164, TAT_CENTER );

			g_FontMgr.SetTextStyle( TS_NORMAL );
			g_FontMgr.SetAlignType( TAT_CENTER );
			g_FontMgr.SetBkColor( 0, 0, 0 );
			g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );	
			g_FontMgr.PrintText( iXPos + 151, iYPos + 184, FONT_SIZE_13, STR(4) );
		}
		break;
	case PUT_USE_HQ:
		{
			g_FontMgr.SetTextStyle( TS_NORMAL );
			g_FontMgr.SetAlignType( TAT_CENTER );
			g_FontMgr.SetBkColor( 0, 0, 0 );

			g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
			g_FontMgr.PrintText( iXPos + 151, iYPos + 144, FONT_SIZE_13, STR(5) );

			g_FontMgr.SetTextStyle( TS_NORMAL );
			g_FontMgr.SetAlignType( TAT_CENTER );
			g_FontMgr.SetBkColor( 0, 0, 0 );
			g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );	
			g_FontMgr.PrintText( iXPos + 151, iYPos + 164, FONT_SIZE_13, STR(6) );

			g_FontMgr.SetTextStyle( TS_NORMAL );
			g_FontMgr.SetAlignType( TAT_CENTER );
			g_FontMgr.SetBkColor( 0, 0, 0 );
			g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );	
			g_FontMgr.PrintText( iXPos + 151, iYPos + 184, FONT_SIZE_13, STR(7) );
		}
		break;
	}


}