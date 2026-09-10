#include "StdAfx.h"
#include "ReinforceSelectWnd.h"

#include "MyInventoryWnd.h"
#include "../ioPowerUpManager.h"
#include "../ioEtcItem.h"
#include "../ioSaleManager.h"

ReinforceSelectWnd::ReinforceSelectWnd(void)
{
	m_iSoulStoneCount = 0;
}


ReinforceSelectWnd::~ReinforceSelectWnd(void)
{
}

void ReinforceSelectWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	ioWnd::AddRenderImage( szType, pImage );
}

bool ReinforceSelectWnd::iwm_esc()
{
	if( HasWndStyle( IWS_EXIT_ESC ) )
	{
		iwm_command( FindChildWnd( ID_EXIT_BTN ), IOBN_BTNUP, 0 );
		return true;
	}

	return false;
}

void ReinforceSelectWnd::iwm_show()
{
	ShowChildWnd( HERO_REINFORCE_WND );
	ShowChildWnd( WEAPON_REINFORCE_WND );

	SetWndPos( (Setting::Width()/2) - GetWidth()/2, ((Setting::Height()/2) - GetHeight()/2) - 20 );
}

void ReinforceSelectWnd::iwm_hide()
{
	HideChildWnd( HERO_REINFORCE_WND );
	HideChildWnd( WEAPON_REINFORCE_WND );
}

void ReinforceSelectWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();	
	switch( dwID )
	{
	case ID_EXIT_BTN:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
		}
		break;
	case HERO_REINFORCE_WND:
		if( cmd == IOBN_BTNUP )
		{
			if (  param == ReinforceSelectSubWnd::ID_SELECT )
			{
				ioHashString szError;
				szError.Clear();
				int iClassType = 0;
				bool bIsChar = false;

				// PowerUp이 가능한지 체크
				int iUseCount,iCurCount;
				if ( !CheckEnableHeroPowerup( szError, iClassType, bIsChar, iUseCount, iCurCount ) )
				{
					if ( bIsChar )
					{
						SoulStoneBuyWnd *pSoulstoneBuyWnd = dynamic_cast<SoulStoneBuyWnd*> ( g_GUIMgr.FindWnd( SOULSTONE_BUY_WND ) );
						if ( pSoulstoneBuyWnd )
							pSoulstoneBuyWnd->ShowBuyWnd( iUseCount, iCurCount );

						HideWnd();
					}
					else
					{
						ioComplexStringPrinter kPrinter[MAX_MSG_LIST_SIZE];
						kPrinter[0].SetTextStyle( TS_NORMAL );
						kPrinter[0].SetBkColor( 0, 0, 0 );
						kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
						kPrinter[0].AddTextPiece( FONT_SIZE_13, STR(1) );

						kPrinter[1].SetTextStyle( TS_NORMAL );
						kPrinter[1].SetBkColor( 0, 0, 0 );
						kPrinter[1].SetTextColor( TCT_DEFAULT_DARKGRAY );
						kPrinter[1].AddTextPiece( FONT_SIZE_13, STR(2) );

						kPrinter[2].SetTextStyle( TS_NORMAL );
						kPrinter[2].SetBkColor( 0, 0, 0 );
						kPrinter[2].SetTextColor( TCT_DEFAULT_GRAY );
						kPrinter[2].AddTextPiece( FONT_SIZE_13, STR(3) );
						g_GUIMgr.SetPrevMsgListBox( NULL, MB_OK, this, kPrinter );
					}
					break;
				}

				HideWnd();
				MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
				if( pInvenWnd && pInvenWnd->IsShow() )
					pInvenWnd->ShowHeroReinforceDownWnd( iClassType );
				
			}
		}
		break;
	case WEAPON_REINFORCE_WND:
		if( cmd == IOBN_BTNUP )
		{
			if (  param == ReinforceSelectSubWnd::ID_SELECT )
			{
				ioHashString szError;
				szError.Clear();
				int iIndex = 0;
				bool bIsWeapon = false;

				// PowerUp이 가능한지 체크
				int iUseCount,iCurCount;
				if ( !CheckEnableWeaponPowerup( szError, iIndex, bIsWeapon, iUseCount, iCurCount ) )
				{
					if ( bIsWeapon )
					{
						SoulStoneBuyWnd *pSoulstoneBuyWnd = dynamic_cast<SoulStoneBuyWnd*> ( g_GUIMgr.FindWnd( SOULSTONE_BUY_WND ) );
						if ( pSoulstoneBuyWnd )
							pSoulstoneBuyWnd->ShowBuyWnd( iUseCount, iCurCount );

						HideWnd();
					}
					else
					{
						ioComplexStringPrinter kPrinter[MAX_MSG_LIST_SIZE];
						kPrinter[0].SetTextStyle( TS_NORMAL );
						kPrinter[0].SetBkColor( 0, 0, 0 );
						kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
						kPrinter[0].AddTextPiece( FONT_SIZE_13, STR(4) );

						kPrinter[1].SetTextStyle( TS_NORMAL );
						kPrinter[1].SetBkColor( 0, 0, 0 );
						kPrinter[1].SetTextColor( TCT_DEFAULT_DARKGRAY );
						kPrinter[1].AddTextPiece( FONT_SIZE_13, STR(5) );

						kPrinter[2].SetTextStyle( TS_NORMAL );
						kPrinter[2].SetBkColor( 0, 0, 0 );
						kPrinter[2].SetTextColor( TCT_DEFAULT_GRAY );
						kPrinter[2].AddTextPiece( FONT_SIZE_13, STR(6) );
						g_GUIMgr.SetPrevMsgListBox( NULL, MB_OK, this, kPrinter );
					}
					break;
				}

				HideWnd();
				MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
				if( pInvenWnd && pInvenWnd->IsShow() )
					pInvenWnd->ShowItemReinforceDownWnd( 0 );
			}
		}
		break;
	}
}

void ReinforceSelectWnd::OnRender()
{
	ioWnd::OnRender();

	enum
	{
		X_OFFSET	= 163,
		Y_OFFSET	= 284,
	};

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	ioHashString szSoulStoneText, szCountstring, szLastText;
	szSoulStoneText = STR(1);
	szLastText = STR(3);

	char szCount[MAX_PATH];
	
	SafeSprintf( szCount, sizeof( szCount ), STR(2), m_iSoulStoneCount );
	szCountstring = szCount;

	float fSoulStoneTextSize = g_FontMgr.GetTextWidth( szSoulStoneText.c_str(), TS_NORMAL, FONT_SIZE_14 );
	float fCountSize = g_FontMgr.GetTextWidth( szCountstring.c_str(), TS_NORMAL, FONT_SIZE_14 );
	float fLastTextSize = g_FontMgr.GetTextWidth( szLastText.c_str(), TS_NORMAL, FONT_SIZE_14 );
	float fTotalSize = fSoulStoneTextSize + fCountSize + fLastTextSize;
	float fHalfSize = fTotalSize * FLOAT05;

	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );

	int iCurPos = iXPos + X_OFFSET - fHalfSize;
	int iNewYPos = iYPos + Y_OFFSET;
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iCurPos, iNewYPos, FONT_SIZE_14, szSoulStoneText.c_str() );

	iCurPos += fSoulStoneTextSize;
	g_FontMgr.SetTextColor( TCT_DEFAULT_BLUE );
	g_FontMgr.PrintText( iCurPos, iNewYPos, FONT_SIZE_14, szCountstring.c_str() );

	iCurPos += fCountSize;
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iCurPos, iNewYPos, FONT_SIZE_14, szLastText.c_str() );
}

void ReinforceSelectWnd::SetInfo()
{
	m_iSoulStoneCount = 0;

	ReinforceSelectSubWnd* pSubWnd = dynamic_cast<ReinforceSelectSubWnd*>(FindChildWnd(HERO_REINFORCE_WND));
	if( pSubWnd )
		pSubWnd->SetInfo( HERO_REINFORCE );

	pSubWnd = dynamic_cast<ReinforceSelectSubWnd*>(FindChildWnd(WEAPON_REINFORCE_WND));
	if( pSubWnd )
		pSubWnd->SetInfo( WEAPON_REINFORCE );

	ioUserEtcItem *pUserEtcItem = g_MyInfo.GetUserEtcItem();
	if( pUserEtcItem )
	{
		ioUserEtcItem::ETCITEMSLOT kSlot;
		if( pUserEtcItem->GetEtcItem( ioEtcItem::EIT_ETC_SOUL_STONE, kSlot ) )
			m_iSoulStoneCount = kSlot.m_iValue1;
	}
}

bool ReinforceSelectWnd::CheckEnableHeroPowerup( ioHashString& szError, int &iClassType, bool& bIschar, int& iUseMtrlCount, int& iCurMtrlCount )
{
	bool bIsCharInfo = false;
	int iCharCnt = g_MyInfo.GetCharCount();
	for ( int i= 0 ; i < iCharCnt ; ++i )
	{
		CHARACTER charinfo;
		charinfo = g_MyInfo.GetCharacter( (DWORD)i );
		if ( g_PowerUpManager.CheckEnableCharPowerUp( charinfo.m_class_type, szError ) )
		{
			bIschar = true;
			iClassType = charinfo.m_class_type;
			if( !g_PowerUpManager.CheckEnableSoulStone( ioPowerUpManager::PIT_HERO, iClassType, iUseMtrlCount, iCurMtrlCount ) )
				continue;

			return true;
		}
	}	
	return false;
}

bool ReinforceSelectWnd::CheckEnableWeaponPowerup( ioHashString& szError, int &iIndex, bool& bIsWeapon, int& iUseMtrlCount, int& iCurMtrlCount )
{
	ioUserExtraItem *pExtraItem = g_MyInfo.GetUserExtraItem();
	if( !pExtraItem )
		return false;

	int iItemCnt = pExtraItem->GetExtraItemCnt();

	bIsWeapon = false;
	for ( int i= 0 ; i < iItemCnt ; ++i )
	{
		EXTRAITEMSLOT Slot;
		if( !pExtraItem->GetExtraItemArray( i, Slot ) )
			continue;
		if ( g_PowerUpManager.CheckEnableItemPowerUp( Slot.m_iIndex, szError ) )
		{
			bIsWeapon = true;
			iIndex = Slot.m_iIndex;

			if( !g_PowerUpManager.CheckEnableSoulStone( ioPowerUpManager::PIT_ITEM, Slot.m_iItemCode, iUseMtrlCount, iCurMtrlCount ) )
				continue;

			return true;
		}
	}

	return false;
	
}

////////////////////////////////////////////////////////////////////////////////////////////////
ReinforceSelectSubWnd::ReinforceSelectSubWnd()
{

}

ReinforceSelectSubWnd::~ReinforceSelectSubWnd()
{

}

void ReinforceSelectSubWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	ioButton::AddRenderImage( szType, pImage );
}

void ReinforceSelectSubWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();

	switch( dwID )
	{
	case ID_SELECT:
		if( cmd == IOBN_BTNUP )
		{
			if( GetParent() )
				GetParent()->iwm_command( this, cmd, dwID );
		}
	}
}

void ReinforceSelectSubWnd::ClearList()
{
	for( int i=0; i < SUB_DESC_LINE; ++i )
		m_ReinforceDesc[i].ClearList();
}

void ReinforceSelectSubWnd::SetInfo( ReinforceSubType eType )
{
	m_SubType = eType;
	ClearList();
	if ( eType == HERO_REINFORCE )
	{
		for( int i=0; i < SUB_DESC_LINE; ++i )
		{
			m_ReinforceDesc[i].SetTextStyle( TS_NORMAL );
			m_ReinforceDesc[i].SetBkColor( 0, 0, 0 );
			m_ReinforceDesc[i].SetTextColor( TCT_DEFAULT_GRAY );
			//STR(1) ~ STR(6)
			if( _strnicmp( STR( i + 1 ), STR(13), 2 ) == 0 )
				m_ReinforceDesc[i].AddTextPiece( FONT_SIZE_11, STR( i + 1 ) );
			else
				m_ReinforceDesc[i].AddTextPiece( FONT_SIZE_11, "   %s",STR( i + 1 ) );
				
		}
	}
	else if ( eType == WEAPON_REINFORCE )
	{
		for( int i=0; i < SUB_DESC_LINE; ++i )
		{
			m_ReinforceDesc[i].SetTextStyle( TS_NORMAL );
			m_ReinforceDesc[i].SetBkColor( 0, 0, 0 );
			m_ReinforceDesc[i].SetTextColor( TCT_DEFAULT_GRAY );
			//STR(7) ~ STR(12)
			if( _strnicmp( STR( i + 1 + SUB_DESC_LINE ), STR(13), 2 ) == 0 )
				m_ReinforceDesc[i].AddTextPiece( FONT_SIZE_11, STR( i + 1 + SUB_DESC_LINE ) );
			else
				m_ReinforceDesc[i].AddTextPiece( FONT_SIZE_11, "   %s",STR( i + 1  + SUB_DESC_LINE) );
		}
	}
}

void ReinforceSelectSubWnd::OnRender()
{
	ioButton::OnRender();
	SubDescRender();
}

void ReinforceSelectSubWnd::SubDescRender()
{
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	enum
	{
		X_OFFSET	= 12,
		Y_OFFSET	= 88,
		LINE_GAP = 16,
	};

	if ( m_SubType == HERO_REINFORCE )
	{
		for( int i=0; i < SUB_DESC_LINE; ++i )
		{
			if( m_ReinforceDesc[i].IsEmpty() )
				break;

			m_ReinforceDesc[i].PrintFullText( iXPos+X_OFFSET, iYPos+Y_OFFSET+(LINE_GAP*i), TAT_LEFT );
		}
	}
	else if ( m_SubType == WEAPON_REINFORCE )
	{
		for( int i=0; i < SUB_DESC_LINE; ++i )
		{
			if( m_ReinforceDesc[i].IsEmpty() )
				break;

			m_ReinforceDesc[i].PrintFullText( iXPos+X_OFFSET, iYPos+Y_OFFSET+(LINE_GAP*i), TAT_LEFT );
		}
	}
}

void ReinforceSelectSubWnd::OnRenderAfterChild()
{
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	enum
	{
		X_OFFSET	= 73,

		Y_OFFSET	= 22,
		Y_OFFSET2	= 45,
	};

	DWORD SkyBlueColor = D3DCOLOR_XRGB( 131, 231 , 247 );

	if ( m_SubType == HERO_REINFORCE )
	{
		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_2X );
		g_FontMgr.SetBkColor( TCT_DEFAULT_GREEN );
		g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
		g_FontMgr.PrintText( iXPos + X_OFFSET, iYPos + Y_OFFSET, FONT_SIZE_16, STR(1) );
		
		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_2X );
		g_FontMgr.SetBkColor( TCT_DEFAULT_GREEN );
		g_FontMgr.SetTextColor( SkyBlueColor );
		g_FontMgr.PrintText( iXPos + X_OFFSET, iYPos + Y_OFFSET2, FONT_SIZE_14, STR(2) );
		
	}
	else if ( m_SubType == WEAPON_REINFORCE )
	{
		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_2X );
		g_FontMgr.SetBkColor( TCT_DEFAULT_GREEN );
		g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
		g_FontMgr.PrintText( iXPos + X_OFFSET, iYPos + Y_OFFSET, FONT_SIZE_16, STR(3) );
		
		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_2X );
		g_FontMgr.SetBkColor( TCT_DEFAULT_GREEN );
		g_FontMgr.SetTextColor( SkyBlueColor );
		g_FontMgr.PrintText( iXPos + X_OFFSET, iYPos + Y_OFFSET2, FONT_SIZE_14, STR(4) );
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SoulStoneBuyWnd::SoulStoneBuyWnd()
{
	m_pIcon = NULL;
	m_pSubIcon = NULL;
}

SoulStoneBuyWnd::~SoulStoneBuyWnd()
{
	SAFEDELETE( m_pIcon );
	SAFEDELETE( m_pSubIcon );
}

void SoulStoneBuyWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
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
			NewShopWnd *pShopWnd = dynamic_cast<NewShopWnd*>(g_GUIMgr.FindWnd(NEW_SHOP_WND));
			if( pShopWnd )
			{
				int iMagicCode = g_PowerUpManager.GetMtrlEtcItemCode(); // shop show시 내아이템이 hide 되므로 m_ItemInfo.m_iMagicCode 초기화 되어서 저장후 셋팅
				pShopWnd->ShowWnd();
				pShopWnd->SetDirectTab( NewShopWnd::ID_ETC_TAB_BTN, iMagicCode, 0 );
			}
			HideWnd();
		}
		break;		
	}
}

void SoulStoneBuyWnd::OnRender()
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

	for( int i=0; i < SOULSTONE_DESC_LINE; ++i )
	{
		if( m_SoulstoneDesc[i].IsEmpty() )
			break;

		m_SoulstoneDesc[i].PrintFullText( iXPos+DESC_X_OFFSET, iYPos+DESC_Y_OFFSET+(DESC_Y_GAP*i), TAT_CENTER );
	}
}

void SoulStoneBuyWnd::ShowBuyWnd( int iUseMtrlCount, int iCurMtrlCount )
{
	SAFEDELETE( m_pIcon );
	SAFEDELETE( m_pSubIcon );

	for( int i=0; i < SOULSTONE_DESC_LINE; ++i )
		m_SoulstoneDesc[i].ClearList();

	ioEtcItemSoulStone* pEtcItem = dynamic_cast<ioEtcItemSoulStone*>( g_EtcItemMgr.FindEtcItem( g_PowerUpManager.GetMtrlEtcItemCode() ) );
	if ( !pEtcItem )
		return;
	m_pIcon = g_UIImageSetMgr.CreateImageByFullName( pEtcItem->GetIconName() );
	m_pSubIcon = g_UIImageSetMgr.CreateImageByFullName( pEtcItem->GetSubIconName() );

	char szBuf[MAX_PATH];
	memset( szBuf, 0, sizeof(szBuf) );
	m_SoulstoneDesc[0].SetTextStyle( TS_NORMAL );
	m_SoulstoneDesc[0].SetBkColor( 0, 0, 0 );
	m_SoulstoneDesc[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
	m_SoulstoneDesc[0].AddTextPiece( FONT_SIZE_13, STR(1) );
	SafeSprintf( szBuf, sizeof( szBuf ), STR(2), iUseMtrlCount );
	m_SoulstoneDesc[0].SetTextColor( TCT_DEFAULT_RED );
	m_SoulstoneDesc[0].AddTextPiece( FONT_SIZE_13, szBuf );
	m_SoulstoneDesc[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
	m_SoulstoneDesc[0].AddTextPiece( FONT_SIZE_13, STR(3) );

	m_SoulstoneDesc[1].SetTextStyle( TS_NORMAL );
	m_SoulstoneDesc[1].SetBkColor( 0, 0, 0 );
	memset( szBuf, 0, sizeof(szBuf) );
	SafeSprintf( szBuf, sizeof( szBuf ), STR(4), iCurMtrlCount );
	m_SoulstoneDesc[1].SetTextColor( TCT_DEFAULT_RED );
	m_SoulstoneDesc[1].AddTextPiece( FONT_SIZE_13, szBuf );

	m_SoulstoneDesc[2].SetTextStyle( TS_NORMAL );
	m_SoulstoneDesc[2].SetBkColor( 0, 0, 0 );
	m_SoulstoneDesc[2].SetTextColor( TCT_DEFAULT_GRAY );
	m_SoulstoneDesc[2].AddTextPiece( FONT_SIZE_13, STR(5) );

	m_SoulstoneDesc[3].SetTextStyle( TS_NORMAL );
	m_SoulstoneDesc[3].SetBkColor( 0, 0, 0 );
	m_SoulstoneDesc[3].SetTextColor( TCT_DEFAULT_GRAY );
	m_SoulstoneDesc[3].AddTextPiece( FONT_SIZE_13, STR(6) );
	
	if( !IsShow() )
		ShowWnd();
}
