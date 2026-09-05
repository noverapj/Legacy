#include "stdafx.h"

#include "../io3DEngine/ioWnd.h"
#include "../io3DEngine/ioScroll.h"
#include "../io3DEngine/ioFontManager.h"
#include "../io3DEngine/ioUIRenderer.h"
#include "../io3DEngine/ioMath.h"
#include "../io3DEngine/ioUIImageSetManager.h"
#include "../io3DEngine/ioUIRenderImage.h"
#include "../io3DEngine/ioUIRenderFrame.h"
#include "../io3DEngine/ioOrientBox.h"
#include "../io3DEngine/HelpFunc.h"
#include "../io3DEngine/ioXMLElement.h"
#include "../io3DEngine/iostringmanager.h"
#include "../io3dengine/safesprintf.h"
#include "../io3dengine/ioUITitle.h"

#include "../WndID.h"
#include "../Setting.h"
#include "../ioMyInfo.h"
#include "../ioApplication.h"

#include "../ioMyLevelMgr.h"
#include "../ioClassPrice.h"

#include "../EtcHelpFunc.h"
#include "../TextColorTable.h"

#include "../ioPlayStage.h"

#include "../ioBaseChar.h"
#include "../ioCreateChar.h"
#include "../ioSkillMaker.h"

#include "../ioGrowthLevel.h"
#include "../ioGrowthUpInfo.h"
#include "../ioGrowthInfoManager.h"

#include "../ioItem.h"
#include "../ioItemMaker.h"
#include "../ioSetItemInfo.h"
#include "../ioSetItemInfoManager.h"

#include "../ioUserEtcItem.h"
#include "../ioEtcItemManager.h"
#include "../ioUserExtraItem.h"
#include "../ioExtraItemInfoManager.h"

#include "../ioPresentMgr.h"
#include "../ioDecorationPrice.h"

#include "../NetworkWrappingFunc.h"
#include "../ioMedalItemInfoManager.h"

#include "../ioQuestManager.h"

#include "..\Local\ioLocalManager.h"
#include "..\Local\ioLocalParent.h"

#include "ioUI3DRender.h"
#include "MySoldierWnd.h"
#include "ioSP2GUIManager.h"
#include "GlobalWnd.h"

#include "NewShopWnd.h"
#include "MyInventoryWnd.h"
#include "MyInventoryInfoWnd.h"
#include "ItemCustomWnd.h"

#include "ChangeTradeStateWnd.h"

#include <strsafe.h>
#include "../Local/ioLocalManager.h"


//////////////////////////////////////////////////////////////////////////
TradeIconBtn::TradeIconBtn()
{
	m_pIcon       = NULL;
	m_pIconBack   = NULL;
	m_pCustomIcon = NULL;

	m_pNumText = NULL;
	m_pPlusText = NULL;

	m_pStateOrangeFrm = NULL;
	m_pStateGreenFrm = NULL;

	m_iMarkType = TRADE_MARK_TYPE_DISABLE;
	m_bCustomItem = false;

	m_iNum = 0;
	m_fScale = FLOAT1;
	m_fBackScale = FLOAT1;
	m_nGradeType = 0;
}

TradeIconBtn::~TradeIconBtn()
{
	SAFEDELETE( m_pIcon );
	SAFEDELETE( m_pIconBack );
	SAFEDELETE( m_pCustomIcon );

	SAFEDELETE( m_pNumText );
	SAFEDELETE( m_pPlusText );

	SAFEDELETE( m_pStateOrangeFrm );
	SAFEDELETE( m_pStateGreenFrm );
}

void TradeIconBtn::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "IconBack" )
	{
		SAFEDELETE( m_pIconBack );
		m_pIconBack = pImage;
	}
	else if( szType == "custom_image" )
	{
		SAFEDELETE( m_pCustomIcon );
		m_pCustomIcon = pImage;
	}
	else if( szType == "NumText" )
	{
		SAFEDELETE( m_pNumText );
		m_pNumText = pImage;
	}
	else if( szType == "PlusText" )
	{
		SAFEDELETE( m_pPlusText );
		m_pPlusText = pImage;
	}
	else
	{
		ioButton::AddRenderImage( szType, pImage );	
	}	
}

void TradeIconBtn::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( pFrame )
	{
		if( szType == "StateOrangeFrm" )
		{
			SAFEDELETE( m_pStateOrangeFrm );
			m_pStateOrangeFrm = pFrame;
		}
		else if( szType == "StateGreenFrm" )
		{
			SAFEDELETE( m_pStateGreenFrm );
			m_pStateGreenFrm = pFrame;
		}
		else
		{	
			ioButton::AddRenderFrame( szType, pFrame );
		}
	}
}

void TradeIconBtn::OnDrawNormal( int iXPos, int iYPos )
{
	ioButton::OnDrawNormal( iXPos, iYPos );

	RenderBack( iXPos, iYPos );
	RenderIcon( iXPos, iYPos , UI_RENDER_NORMAL );
	RenderOtherInfo( iXPos, iYPos );
}

void TradeIconBtn::OnDrawOvered( int iXPos, int iYPos )
{
	ioButton::OnDrawOvered( iXPos, iYPos );

	RenderBack( iXPos, iYPos );
	RenderIcon( iXPos, iYPos , UI_RENDER_NORMAL );
	RenderIcon( iXPos, iYPos , UI_RENDER_SCREEN );
	RenderOtherInfo( iXPos, iYPos );
}

void TradeIconBtn::OnDrawPushed( int iXPos, int iYPos )
{
	ioButton::OnDrawPushed( iXPos, iYPos );

	RenderBack( iXPos, iYPos );
	RenderIcon( iXPos, iYPos , UI_RENDER_NORMAL );
	RenderIcon( iXPos, iYPos , UI_RENDER_SCREEN );
	RenderOtherInfo( iXPos, iYPos );
}

void TradeIconBtn::OnDrawDisabled( int iXPos, int iYPos )
{
	ioButton::OnDrawDisabled( iXPos, iYPos );

	RenderBack( iXPos, iYPos );
	RenderIcon( iXPos, iYPos , UI_RENDER_NORMAL );
	RenderOtherInfo( iXPos, iYPos );
}

void TradeIconBtn::RenderBack( int iXPos, int iYPos )
{
	int iNewXPos = iXPos + GetWidth() / 2;
	int iNewYPos = iYPos + GetHeight() / 2;

	// Icon
	if( m_pIconBack )
	{
		m_pIconBack->SetScale( m_fBackScale );

		if( m_fBackScale != FLOAT1 )
			m_pIconBack->Render( iNewXPos, iNewYPos, UI_RENDER_MULTIPLY, TFO_BILINEAR );
		else
			m_pIconBack->Render( iNewXPos, iNewYPos, UI_RENDER_MULTIPLY );
	}
}

void TradeIconBtn::RenderIcon( int iXPos, int iYPos, UIRenderType eType )
{
	enum 
	{
		X_TEXT_OFFSET   = 20,
		Y_TEXT_OFFSET   = 2,
	};

	int iNewXPos = iXPos + GetWidth() / 2;
	int iNewYPos = iYPos + GetHeight() / 2;

	if( m_pIcon )
	{
		m_pIcon->SetScale( m_fScale );

		UIRenderType eNewType = eType;
		if( HasWndStyle( IWS_INACTIVE ) )
			eNewType = UI_RENDER_GRAY;

		if ( eNewType != UI_RENDER_SCREEN )
			g_UIImageRenderMgr.RenderGradeIconBack( m_nGradeType, iNewXPos, iNewYPos, m_fScale, eNewType );

		if( m_fScale != FLOAT1 )
			m_pIcon->Render( iNewXPos, iNewYPos, eNewType, TFO_BILINEAR );
		else
			m_pIcon->Render( iNewXPos, iNewYPos, eNewType );
	}

	if( m_bCustomItem && m_pCustomIcon )
	{
		iNewXPos = iNewXPos + ( 7.0f * m_fScale );
		iNewYPos = iNewYPos - ( 43.0f * m_fScale );
		m_pCustomIcon->SetScale( m_fScale );
		if( m_fScale != FLOAT1 )
			m_pCustomIcon->Render( iNewXPos, iNewYPos, UI_RENDER_NORMAL, TFO_BILINEAR );
		else
			m_pCustomIcon->Render( iNewXPos, iNewYPos );
	}
}

void TradeIconBtn::RenderOtherInfo( int iXPos, int iYPos )
{
	enum 
	{
		X_TEXT_OFFSET   = 20,
		Y_TEXT_OFFSET   = 2,
	};

	int iNewXPos = iXPos + GetWidth() / 2;
	int iNewYPos = iYPos + GetHeight() / 2;


	// Mark
	int iMarkX = iNewXPos;
	int iMarkY = iNewYPos;

	int iTextX = iMarkX;
	int iTextY = iMarkY;

	char szName[MAX_PATH]="";
	if( m_iMarkType == TRADE_MARK_TYPE_ENABLE )
	{
		iMarkX = iNewXPos-45;
		iMarkY = iNewYPos-41;

		if( m_pStateGreenFrm )
			m_pStateGreenFrm->Render( iMarkX, iMarkY, UI_RENDER_NORMAL );

		iTextX = iMarkX+27;
		iTextY = iMarkY+Y_TEXT_OFFSET;
		SafeSprintf( szName, sizeof( szName ), STR(1) );

		g_FontMgr.SetTextStyle( TS_BOLD );
		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
		g_FontMgr.PrintText( iTextX, iTextY, FONT_SIZE_11, szName );
	}
	else if( m_iMarkType == TRADE_MARK_TYPE_WAIT )
	{
		iMarkX = iNewXPos-45;
		iMarkY = iNewYPos-41;

		if( m_pStateOrangeFrm )
			m_pStateOrangeFrm->Render( iMarkX, iMarkY, UI_RENDER_NORMAL );

		iTextX = iMarkX+27;
		iTextY = iMarkY+Y_TEXT_OFFSET;
		SafeSprintf( szName, sizeof( szName ), STR(2) );

		g_FontMgr.SetTextStyle( TS_BOLD );
		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
		g_FontMgr.PrintText( iTextX, iTextY, FONT_SIZE_11, szName );
	}
	else if( m_iMarkType == TRADE_MARK_TYPE_DISABLE )
	{
		// 현재는 출력할 것 없음
	}

	// Num
	enum
	{
		REINFORCE_Y	= 14,

		NUM_SIZE	= 20,
		PLUS_SIZE	= 14,

		NUM_GAP		= 4,
	};

	if( m_pNumText && m_pPlusText && m_iNum > 0 )
	{
		int iCurPos = 0;
		int iDefaultPos = iNewXPos;

		int iCurYPos = iNewYPos+(REINFORCE_Y * m_fScale);

		int iTotalSize = 0;
		if( m_iNum < 10 )
			iTotalSize = PLUS_SIZE + NUM_SIZE - NUM_GAP;
		else if( m_iNum < 100 )
			iTotalSize = PLUS_SIZE + NUM_SIZE*2 - NUM_GAP*2;
		else
			iTotalSize = PLUS_SIZE + NUM_SIZE*3 - NUM_GAP*3;

		iCurPos = iDefaultPos - (iTotalSize / 2);

		m_pPlusText->Render( iCurPos, iCurYPos );

		iCurPos += (PLUS_SIZE-NUM_GAP);

		m_pNumText->RenderNum( iCurPos, iCurYPos, m_iNum, -NUM_GAP );
	}
}

bool TradeIconBtn::SetIcon( ioUIRenderImage *pIcon, float fScale, float fBackScale, int iNum )
{
	m_iNum = iNum;
	m_fScale = fScale;
	m_fBackScale = fBackScale;

	m_iMarkType = 0;
	m_nGradeType = 0;

	SAFEDELETE( m_pIcon );
	if( !pIcon )
	{
		m_iNum = 0;
		return false;
	}

	m_pIcon = pIcon;

	return true;
}

void TradeIconBtn::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;

	if( !m_pParent )
		return;
}

//////////////////////////////////////////////////////////////////////////

ChangeTradeStateConfirmWnd::ChangeTradeStateConfirmWnd()
{
	m_pStateOrangeFrm = NULL;
	m_pStateGreenFrm = NULL;

	m_pArrow = NULL;
}

ChangeTradeStateConfirmWnd::~ChangeTradeStateConfirmWnd()
{
	SAFEDELETE( m_pStateOrangeFrm );
	SAFEDELETE( m_pStateGreenFrm );

	SAFEDELETE( m_pArrow );
}

void ChangeTradeStateConfirmWnd::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( pFrame )
	{
		if( szType == "StateOrangeFrm" )
		{
			SAFEDELETE( m_pStateOrangeFrm );
			m_pStateOrangeFrm = pFrame;
		}
		else if( szType == "StateGreenFrm" )
		{
			SAFEDELETE( m_pStateGreenFrm );
			m_pStateGreenFrm = pFrame;
		}
		else
		{	
			ioMovingWnd::AddRenderFrame( szType, pFrame );
		}
	}
}

void ChangeTradeStateConfirmWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "Arrow" )
	{
		SAFEDELETE( m_pArrow );
		m_pArrow = pImage;
	}
	else
	{
		ioMovingWnd::AddRenderImage( szType, pImage );	
	}
}

void ChangeTradeStateConfirmWnd::SetChangeItemInfo( int iIndex )
{
	ioUserExtraItem *pExtraItem = g_MyInfo.GetUserExtraItem();
	if( pExtraItem )
	{
		EXTRAITEMSLOT kSlot;
		if( pExtraItem->GetExtraItem( iIndex, kSlot ) )
		{
			m_szName = kSlot.m_Name;
			m_iExtraItemIndex = iIndex;

			ioUIRenderImage *pIcon = g_UIImageSetMgr.CreateImageByFullName( kSlot.m_IconName );
			TradeIconBtn *pIconBtn = dynamic_cast<TradeIconBtn*>(FindChildWnd(ID_ICON_BTN));
			if( pIconBtn )
			{
				pIconBtn->SetIcon( pIcon, FLOAT1, FLOAT1, kSlot.m_iReinforce );
				pIconBtn->SetMarkType( TRADE_MARK_TYPE_DISABLE );

				const ioItem *pItem = g_ItemMaker.GetItemConst( kSlot.m_iItemCode, __FUNCTION__ );
				if( pItem )
					pIconBtn->SetGradeType( pItem->GetGradeType() );

				if( kSlot.m_dwMaleCustom > 0 || kSlot.m_dwFemaleCustom > 0 )
					pIconBtn->SetCustomItem( true );
				else
					pIconBtn->SetCustomItem( false );
			}
		}
	}

	if( !IsShow() )
		ShowWnd();
}

void ChangeTradeStateConfirmWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_CHANGE_BTN:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();

			ChangeTradeStateWnd *pChangeWnd = dynamic_cast<ChangeTradeStateWnd*>(g_GUIMgr.FindWnd(CHANGE_TRADE_STATE_WND));
			if( pChangeWnd )
			{
				pChangeWnd->SendTradeStateChange( m_iExtraItemIndex );
			}
		}
		break;
	case ID_CANCEL_BTN:
	case ID_EXIT_BTN:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
		}
		break;
	}
}

void ChangeTradeStateConfirmWnd::OnRender()
{
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	ioMovingWnd::OnRender();

	enum
	{
		NAME_X		= 103,
		NAME_Y		= 149,

		WAIT_X		= 44,
		WAIT_Y		= 173,

		ARROW_X		= 98,
		ARROW_Y		= 176,

		ENABLE_X	= 109,
		ENABLE_Y	= 173,

		TEXT_X		= 103,
		TEXT_Y		= 199,
	};

	if( m_pStateOrangeFrm )
	{
		int iNewXPos = iXPos+WAIT_X;
		int iNewYPos = iYPos+WAIT_Y;

		m_pStateOrangeFrm->Render( iNewXPos, iNewYPos );

		g_FontMgr.SetTextStyle( TS_BOLD );
		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
		g_FontMgr.PrintText( iNewXPos+26, iNewYPos+3, FONT_SIZE_10, STR(1) );
	}

	if( m_pArrow )
	{
		int iNewXPos = iXPos+ARROW_X;
		int iNewYPos = iYPos+ARROW_Y;

		m_pArrow->SetReverseFormatting( ioUIRenderImage::RF_HORZ_REVERSE );
		m_pArrow->Render( iNewXPos, iNewYPos );
	}

	if( m_pStateGreenFrm )
	{
		int iNewXPos = iXPos+ENABLE_X;
		int iNewYPos = iYPos+ENABLE_Y;

		m_pStateGreenFrm->Render( iNewXPos, iNewYPos );

		g_FontMgr.SetTextStyle( TS_BOLD );
		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
		g_FontMgr.PrintText( iNewXPos+26, iNewYPos+3, FONT_SIZE_10, STR(2) );
	}

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_17, STR(3), m_szName.c_str() );
	kPrinter.PrintFullText( iXPos+NAME_X, iYPos+NAME_Y, TAT_CENTER );

	kPrinter.ClearList();

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_GREEN );
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(4) );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(5) );
	kPrinter.PrintFullText( iXPos+TEXT_X, iYPos+TEXT_Y, TAT_CENTER );

	kPrinter.ClearList();

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(6) );
	kPrinter.PrintFullText( iXPos+TEXT_X, iYPos+TEXT_Y+20, TAT_CENTER );

	kPrinter.ClearList();

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, STR(7) );
	kPrinter.PrintFullText( iXPos+TEXT_X, iYPos+TEXT_Y+40, TAT_CENTER );

	kPrinter.ClearList();
}

//////////////////////////////////////////////////////////////////////////

ChangeTradeStateResultWnd::ChangeTradeStateResultWnd()
{
	m_pManual = NULL;
}

ChangeTradeStateResultWnd::~ChangeTradeStateResultWnd()
{
}

void ChangeTradeStateResultWnd::SetChangeResultInfo( int iIndex )
{
	m_pManual = ioEtcInvenSubManualMgr::GetSingletonPtr();

	ioUserExtraItem *pExtraItem = g_MyInfo.GetUserExtraItem();
	if( pExtraItem )
	{
		EXTRAITEMSLOT kSlot;
		if( pExtraItem->GetExtraItem( iIndex, kSlot ) )
		{
			m_szName = kSlot.m_Name;
			m_iExtraItemIndex = iIndex;

			ioUIRenderImage *pIcon = g_UIImageSetMgr.CreateImageByFullName( kSlot.m_IconName );
			TradeIconBtn *pIconBtn = dynamic_cast<TradeIconBtn*>(FindChildWnd(ID_ICON_BTN));
			if( pIconBtn )
			{
				pIconBtn->SetIcon( pIcon, FLOAT1, FLOAT1, kSlot.m_iReinforce );
				pIconBtn->SetMarkType( TRADE_MARK_TYPE_ENABLE );

				const ioItem *pItem = g_ItemMaker.GetItemConst( kSlot.m_iItemCode, __FUNCTION__ );
				if( pItem )
					pIconBtn->SetGradeType( pItem->GetGradeType() );

				if( kSlot.m_dwMaleCustom > 0 || kSlot.m_dwFemaleCustom > 0 )
					pIconBtn->SetCustomItem( true );
				else
					pIconBtn->SetCustomItem( false );
			}
		}
	}

	if( !IsShow() )
		ShowWnd();

	ReBatchButton();
}

void ChangeTradeStateResultWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_USE:
		if( cmd == IOBN_BTNUP )
		{
			ioUserEtcItem *pUserEtcItem =  g_MyInfo.GetUserEtcItem();
			if( pUserEtcItem )
			{
				ioEtcItem *pEtcItem = g_EtcItemMgr.FindEtcItem( ioEtcItem::EIT_ETC_TRADE_STATE_CHANGE );
				if( !pEtcItem )
				{
					LOG.PrintTimeAndLog( 0, "%s pEtcItem == NULL : %d", __FUNCTION__, ioEtcItem::EIT_ETC_TRADE_STATE_CHANGE );
					return;
				}
				HideWnd();

				pEtcItem->OnUseBtnPush( ioEtcItem::EIT_ETC_TRADE_STATE_CHANGE, pUserEtcItem );
			}
		}
		break;
	case ID_GO:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();

			MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
			if( pInvenWnd )
			{
				pInvenWnd->ShowExtraItemTabDirect( g_MyInfo.GetClassType(), m_iExtraItemIndex );
			}
		}
		break;
	}
}

void ChangeTradeStateResultWnd::OnRender()
{
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	ioWnd::OnRender();

	enum
	{
		ICON_X		= 129,
		ICON_Y		= 105,

		DESC_BIG_X	= 129,
		DESC_BIG_Y	= 152,

		DESC_X		= 27,
		DESC_Y		= 206,

		FINAL_DESC_X	= 128,
		FINAL_DESC_Y	= 421,
	};

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos+DESC_BIG_X, iYPos+DESC_BIG_Y, FONT_SIZE_17, STR(6), m_szName.c_str() );

	PrintManual( iXPos+DESC_X, iYPos+DESC_Y, FONT_SIZE_12 );

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GREEN );
	g_FontMgr.PrintText( iXPos+FINAL_DESC_X, iYPos+FINAL_DESC_Y, FONT_SIZE_17, STR(7) );
}

void ChangeTradeStateResultWnd::ReBatchButton()
{
	enum
	{
		ONE_BTN_SIZE	= 204,
		TWO_BTN_SIZE	= 102,

		ONE_BTN_X		= 27,
		TWO_BTN_X		= 131,

		ONE_TEXT_X		= 102,
		TWO_TEXT_X		= 51,
	};

	HideChildWnd(ID_USE);
	HideChildWnd(ID_GO);

	// 예외처리
	ioUserEtcItem *pUserEtcItem = g_MyInfo.GetUserEtcItem();
	if( !pUserEtcItem )
	{
		ioButton *pButton = dynamic_cast<ioButton*>(FindChildWnd(ID_GO));
		if( pButton )
		{
			pButton->SetSize( ONE_BTN_SIZE, pButton->GetHeight() );
			pButton->SetWndPos( ONE_BTN_X, pButton->GetYPos() );

			ioUITitle *pTitle =  pButton->GetTitle();
			if( pTitle )
			{
				pTitle->SetOffset( ONE_TEXT_X, pTitle->GetTextYOffSet() );
			}

			pButton->ShowWnd();
		}
	}

	ioUserEtcItem::ETCITEMSLOT kSlot;
	if( !pUserEtcItem->GetEtcItem( ioEtcItem::EIT_ETC_TRADE_STATE_CHANGE, kSlot) )
	{
		ioButton *pButton = dynamic_cast<ioButton*>(FindChildWnd(ID_GO));
		if( pButton )
		{
			pButton->SetSize( ONE_BTN_SIZE, pButton->GetHeight() );
			pButton->SetWndPos( ONE_BTN_X, pButton->GetYPos() );

			ioUITitle *pTitle =  pButton->GetTitle();
			if( pTitle )
			{
				pTitle->SetOffset( ONE_TEXT_X, pTitle->GetTextYOffSet() );
			}

			pButton->ShowWnd();
		}
	}
	else
	{
		ioButton *pButton = dynamic_cast<ioButton*>(FindChildWnd(ID_USE));
		if( pButton )
		{
			pButton->SetSize( TWO_BTN_SIZE, pButton->GetHeight() );
			pButton->SetWndPos( ONE_BTN_X, pButton->GetYPos() );

			ioUITitle *pTitle =  pButton->GetTitle();
			if( pTitle )
			{
				pTitle->SetOffset( TWO_TEXT_X, pTitle->GetTextYOffSet() );
			}

			pButton->ShowWnd();
		}

		pButton = dynamic_cast<ioButton*>(FindChildWnd(ID_GO));
		if( pButton )
		{
			pButton->SetSize( TWO_BTN_SIZE, pButton->GetHeight() );
			pButton->SetWndPos( TWO_BTN_X, pButton->GetYPos() );

			ioUITitle *pTitle =  pButton->GetTitle();
			if( pTitle )
			{
				pTitle->SetOffset( TWO_TEXT_X, pTitle->GetTextYOffSet() );
			}

			pButton->ShowWnd();
		}
	}
}

void ChangeTradeStateResultWnd::PrintManual( int a_iXPos, int a_iYPos , float fScale )
{
	if( !m_pManual )
		return;

	ioEtcItem *pItem = NULL;
	pItem = g_EtcItemMgr.FindEtcItem( ioEtcItem::EIT_ETC_TRADE_STATE_CHANGE );

	if( !pItem ) return;

	int iSubManual = pItem->GetInventorySubManual() - 1;
	ioComplexStringPrinter kPrinter;
	int iMaxLine = m_pManual->GetMaxLine( iSubManual );
	for(int iLine = 1; iLine < iMaxLine; iLine++)
	{
		m_pManual->SetPrinter( iSubManual, iLine, fScale, kPrinter);
		a_iYPos += m_pManual->GetSpaceLine( iSubManual, iLine);
		kPrinter.PrintFullTextWidthCut( a_iXPos, a_iYPos, TAT_LEFT, DESC_WIDTH );
		kPrinter.ClearList();
	}
}

//////////////////////////////////////////////////////////////////////////

TradeItemInfoWnd::TradeItemInfoWnd()
{
	m_ItemInfo.Init();
	m_iExtraItemIndex = 0;

	m_pEmptyImg = NULL;
	m_nGradeType = 0;
}

TradeItemInfoWnd::~TradeItemInfoWnd()
{
	ClearDesc();

	SAFEDELETE(m_pEmptyImg);
}

void TradeItemInfoWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();

	switch( dwID )
	{
	case ID_ACTION_BTN:
		if( cmd == IOBN_BTNUP )
		{
			if( GetParent() )
				GetParent()->iwm_command( this, cmd, dwID );
		}
		break;
	case ID_CUSTOM_BTN:
		if( cmd == IOBN_BTNUP )
		{
			ItemCustomGlobalDetailViewWnd *pDetailViewWnd = dynamic_cast< ItemCustomGlobalDetailViewWnd * >( g_GUIMgr.FindWnd( ITEM_CUSTOM_GLOBAL_DETAIL_VIEW_WND ) );
			if( pDetailViewWnd )
			{
				EXTRAITEMSLOT kSlot;
				ioUserExtraItem *pExtraItem = g_MyInfo.GetUserExtraItem();
				if( !pExtraItem )
					return;

				if( !pExtraItem->GetExtraItem( m_ItemInfo.m_iMagicCode, kSlot ) )
					return;

				pDetailViewWnd->CreateCharacter( kSlot.m_iItemCode, kSlot.m_dwMaleCustom, kSlot.m_dwFemaleCustom );
			}
		}
		break;
	}
}

void TradeItemInfoWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "empty_image" )
	{
		SAFEDELETE( m_pEmptyImg );
		m_pEmptyImg = pImage;
	}
	else
	{
		ioButton::AddRenderImage( szType, pImage );
	}
}

void TradeItemInfoWnd::SetBlank()
{
	m_ItemInfo.Init();
	m_iExtraItemIndex = 0;

	SetInActive();

	HideChildWnd( ID_ICON_BTN );
	HideChildWnd( ID_ACTION_BTN );
	HideChildWnd( ID_CUSTOM_BTN );

	ClearDesc();

	m_nGradeType = 0;
}

bool TradeItemInfoWnd::IsCustomToolTip()
{
	if( g_GUIMgr.GetPreOverWnd() == FindChildWnd( ID_CUSTOM_BTN ) )
		return true;
	return false;
}

void TradeItemInfoWnd::ClearDesc()
{
	for (int i = 0; i < MAX_DESC ; i++)
	{
		m_szDescArray[i].ClearList();
	}
}

void TradeItemInfoWnd::OnRender()
{
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( HasWndStyle( IWS_INACTIVE ) )
	{	
		if( m_pDisableFrm )
			m_pDisableFrm->Render( iXPos, iYPos );
		else if( m_pNormalFrm )
			m_pNormalFrm->Render( iXPos, iYPos );

		// 기타 이미지 추가.
		if( m_pEmptyImg )
		{
			m_pEmptyImg->Render( iXPos+52, iYPos+49, UI_RENDER_MULTIPLY );
		}
	}
	else
		ioButton::OnRender();

	RenderDesc( iXPos, iYPos );
}

void TradeItemInfoWnd::RenderDesc( int iXPos, int iYPos )
{
	enum 
	{
		X_OFFSET           = 52,
		Y_OFFSET           = 105,
		Y_LINE_GAP         = 16,
		Y_OFFSET_SECOND    = -8,
	};

	iXPos += X_OFFSET;
	iYPos += Y_OFFSET;


	if( HasWndStyle( IWS_INACTIVE ) )
	{
		for( int i = 0 ; i < MAX_DESC ; i++ )
		{
			m_szDescArray[i].ClearList();
		}

		m_szDescArray[0].SetTextStyle( TS_NORMAL );
		m_szDescArray[0].SetBkColor( 0, 0, 0 );	
		m_szDescArray[0].SetTextColor( TCT_DEFAULT_GRAY );
		m_szDescArray[0].AddTextPiece( FONT_SIZE_13, STR(1) );
		m_szDescArray[0].PrintFullTextWidthCut( iXPos, iYPos, TAT_CENTER, INVENTORY_ITEM_INFO_WIDTHCUT_SIZE );

		m_szDescArray[1].SetTextStyle( TS_NORMAL );
		m_szDescArray[1].SetBkColor( 0, 0, 0 );	
		m_szDescArray[1].SetTextColor( TCT_DEFAULT_GRAY );
		m_szDescArray[1].AddTextPiece( FONT_SIZE_11, "-" );
		m_szDescArray[1].PrintFullTextWidthCut( iXPos, iYPos+Y_LINE_GAP, TAT_CENTER, INVENTORY_ITEM_INFO_WIDTHCUT_SIZE );
	}
	else
	{
		int iDescCnt = 0;
		for( int i = 0 ; i < MAX_DESC ; i++ )
		{
			if( m_szDescArray[i].IsEmpty() )
				continue;

			iDescCnt++;
		}

		if( iDescCnt == 3 )
			iYPos += Y_OFFSET_SECOND;

		int iCnt  = 0;
		for( int i = 0 ; i < MAX_DESC ; i++ )
		{
			if( m_szDescArray[i].IsEmpty() )
				continue;

			m_szDescArray[i].PrintFullTextWidthCut( iXPos, iYPos + ( Y_LINE_GAP * iCnt ), TAT_CENTER, INVENTORY_ITEM_INFO_WIDTHCUT_SIZE );
			iCnt++;
		}
	}
}

void TradeItemInfoWnd::SetInfo( InventoryItemInfo &rkInfo )
{
	SetBlank();

	ioHashString szName, szIcon;

	SetActive();

	m_ItemInfo = rkInfo;
	if( !SetExtraItemInfo() )
	{
		SetBlank();
		return;
	}
}

InventoryItemInfo& TradeItemInfoWnd::GetInfo()
{
	return m_ItemInfo;
}

bool TradeItemInfoWnd::SetExtraItemInfo()
{
	if( m_ItemInfo.m_iMagicCode <= 0 )
		return false;

	EXTRAITEMSLOT kSlot;
	ioUserExtraItem *pExtraItem = g_MyInfo.GetUserExtraItem();
	if( !pExtraItem )
		return false;

	if( !pExtraItem->GetExtraItem( m_ItemInfo.m_iMagicCode, kSlot ) )
		return false;

	// icon btn
	ShowChildWnd( ID_ICON_BTN );
	ShowChildWnd( ID_ACTION_BTN );

	TradeIconBtn *pBtn = dynamic_cast< TradeIconBtn* > ( FindChildWnd( ID_ICON_BTN ) );
	if( !pBtn )
	{
		LOG.PrintTimeAndLog( 0, "%s pBtn == NULL.", __FUNCTION__ );
	}
	else
	{
		ioUIRenderImage *pImg = NULL;
		pImg = g_UIImageSetMgr.CreateImageByFullName( kSlot.m_IconName );
		pBtn->SetIcon( pImg, FLOAT1, FLOAT1, kSlot.m_iReinforce );

		switch( kSlot.m_iTradeState )
		{
		case EET_NORMAL:
			pBtn->SetMarkType( TRADE_MARK_TYPE_WAIT );
			break;
		case EET_DISABLE:
			pBtn->SetMarkType( TRADE_MARK_TYPE_DISABLE );
			break;
		case EET_ENABLE:
			pBtn->SetMarkType( TRADE_MARK_TYPE_ENABLE );
			break;
		}
	}

	if( kSlot.m_dwMaleCustom > 0 || kSlot.m_dwFemaleCustom > 0 )
		ShowChildWnd( ID_CUSTOM_BTN );

	m_iExtraItemIndex = kSlot.m_iIndex;

	// desc
	ClearDesc();
	SetIsWidthCutDesc( 0, true );
	SetIsWidthCutDesc( 2, true );

	// Name
	m_szDescArray[0].SetTextStyle( TS_NORMAL );
	m_szDescArray[0].SetBkColor( 0, 0, 0 );	
	m_szDescArray[0].SetTextColor( TCT_DEFAULT_DARKGRAY ); // 진회
	m_szDescArray[0].AddTextPiece( FONT_SIZE_13, STR(1), kSlot.m_Name.c_str() );

	m_szDescArray[1].ClearList();
	m_szDescArray[1].SetTextStyle( TS_NORMAL );
	m_szDescArray[1].SetBkColor( 0, 0, 0 );	
	m_szDescArray[1].SetTextColor( TCT_DEFAULT_BLUE );
	m_szDescArray[1].AddTextPiece( FONT_SIZE_13, STR(2) );

	// Limit
	int iLimitLevel = kSlot.m_iReinforce * g_ExtraItemInfoMgr.GetLevelLimitConst();
	int iEnableClass = 0;
	int iItemGrade = ConvertPowerUpItemToItemGrade( kSlot.m_iItemCode );

	const ioItem *pItem = g_ItemMaker.GetItemConst( kSlot.m_iItemCode, __FUNCTION__ );
	if( pItem )
	{
		iEnableClass = pItem->GetEnableClass();
		m_nGradeType = pItem->GetGradeType();

		if ( pBtn )
			pBtn->SetGradeType( m_nGradeType );
	}

	m_szDescArray[2].SetTextStyle( TS_NORMAL );
	m_szDescArray[2].SetBkColor( 0, 0, 0 );	
	m_szDescArray[2].SetTextColor( TCT_DEFAULT_GRAY );

	if( iEnableClass > 0 )
		m_szDescArray[2].AddTextPiece( FONT_SIZE_11, STR(3), iLimitLevel, g_MyInfo.GetClassName(iEnableClass,iItemGrade) );
	else
		m_szDescArray[2].AddTextPiece( FONT_SIZE_11, STR(4), iLimitLevel );

	return true;
}

void TradeItemInfoWnd::SetIsWidthCutDesc( int iArray , bool bWidthCut )
{
	if( COMPARE( iArray, 0, MAX_DESC ) )
		m_bWidthCutDescArray[iArray] = bWidthCut;
}

bool TradeItemInfoWnd::IsWidthCutDesc( int iArray )
{
	if( COMPARE( iArray, 0, MAX_DESC ) )
		return m_bWidthCutDescArray[iArray];

	return false;
}

//////////////////////////////////////////////////////////////////////////

ChangeTradeStateWnd::ChangeTradeStateWnd()
{
	m_dwCurTabID = ID_TAB_1_BTN;
}

ChangeTradeStateWnd::~ChangeTradeStateWnd()
{
	m_vInventoryItemInfoList.clear();
}

void ChangeTradeStateWnd::iwm_create()
{
}

void ChangeTradeStateWnd::iwm_show()
{
	ChangeTab( ID_TAB_1_BTN );

	if( m_vInventoryItemInfoList.empty() )
	{
		HideWnd();
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		return;
	}
}

void ChangeTradeStateWnd::iwm_hide()
{
	for (int i = 0; i < MAX_ITEM_INFO_WND_BTN ; i++)
		HideChildWnd( ID_ITEM_INFO_WND_1_BTN + i );
}

void ChangeTradeStateWnd::iwm_vscroll( DWORD dwID, int curPos )
{
	UpdatePage( curPos );
}

void ChangeTradeStateWnd::iwm_wheel( int zDelta )
{
	if( !IsShow() )
	{
		ioMovingWnd::iwm_wheel( zDelta );
	}
	else
	{ 
		// 휠 이벤트 처리를 했다면 부모로 이벤트를 날리지 않는다.
		if( zDelta == WHEEL_DELTA )
		{
			ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
			if( pScroll )
				pScroll->SetScrollPos( m_iCurPage - 1 );

			if( g_GUIMgr.GetPreOverWnd() )
			{
				ioMouse NullMouse;
				g_GUIMgr.GetPreOverWnd()->iwm_mouseover( NullMouse );
			}
		}
		else if( zDelta == -WHEEL_DELTA )
		{
			ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
			if( pScroll )
				pScroll->SetScrollPos( m_iCurPage + 1 );

			if( g_GUIMgr.GetPreOverWnd() )
			{
				ioMouse NullMouse;
				g_GUIMgr.GetPreOverWnd()->iwm_mouseover( NullMouse );
			}
		}
	}
}

void ChangeTradeStateWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
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
	case ID_TAB_1_BTN:
	case ID_TAB_2_BTN:
	case ID_TAB_3_BTN:
	case ID_TAB_4_BTN:
	case ID_TAB_5_BTN:
		if( cmd == IOBN_BTNUP )
		{
			ChangeTab( dwID );
		}
		break;
	case ID_ITEM_INFO_WND_1_BTN:
	case ID_ITEM_INFO_WND_2_BTN:
	case ID_ITEM_INFO_WND_3_BTN:
	case ID_ITEM_INFO_WND_4_BTN:
	case ID_ITEM_INFO_WND_5_BTN:
	case ID_ITEM_INFO_WND_6_BTN:
	case ID_ITEM_INFO_WND_7_BTN:
	case ID_ITEM_INFO_WND_8_BTN:
		if( cmd == IOBN_BTNUP )
		{
			if( param == TradeItemInfoWnd::ID_ACTION_BTN )
			{
				int iStartArray = m_iCurPage * PAGE_PER_ITEM_NUM;
				int iCurArray   = iStartArray + ( dwID - ID_ITEM_INFO_WND_1_BTN );
				int iSize       = m_vInventoryItemInfoList.size();

				if( COMPARE( iCurArray, 0, iSize ) )
				{
					ChangeTradeStateConfirmWnd *pConfirmWnd = dynamic_cast<ChangeTradeStateConfirmWnd*>(g_GUIMgr.FindWnd(CHANGE_CONFIRM_WND));
					if( pConfirmWnd )
						pConfirmWnd->SetChangeItemInfo( m_vInventoryItemInfoList[iCurArray].m_iMagicCode );
				}
			}
		}
		break;
	}
}

void ChangeTradeStateWnd::UpdatePage( int iNewPage )
{
	for (int i = 0; i < MAX_ITEM_INFO_WND_BTN ; i++)
		HideChildWnd( ID_ITEM_INFO_WND_1_BTN + i );

	m_iCurPage = iNewPage;

	int iItemSize = m_vInventoryItemInfoList.size();
	int iStartArray = m_iCurPage * PAGE_PER_ITEM_NUM;
	for( int i = 0 ; i < MAX_ITEM_INFO_WND_BTN ; i++ )
	{
		TradeItemInfoWnd *pItem = dynamic_cast<TradeItemInfoWnd*>(FindChildWnd(ID_ITEM_INFO_WND_1_BTN + i));
		if( !pItem )
			continue;

		pItem->ShowWnd();

		int iCurArray = iStartArray +i;
		if( !COMPARE( iCurArray, 0, iItemSize ) )
		{
			// 초기화
			InventoryItemInfo kInfo;
			pItem->SetInfo( kInfo );
			continue;
		}

		pItem->SetInfo( m_vInventoryItemInfoList[iCurArray] );
	}
}

void ChangeTradeStateWnd::ChangeTab( DWORD dwID )
{
	CheckRadioButton( ID_TAB_1_BTN, ID_TAB_5_BTN, dwID );
	m_dwCurTabID = dwID;

	SetExtraItemInfoVec( m_dwCurTabID );

	m_iCurPage = 0;

	ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
	if( pScroll )
	{
		int iItemSize = m_vInventoryItemInfoList.size();

		int iMaxPos =  iItemSize / PAGE_PER_ITEM_NUM;
		if( iItemSize % PAGE_PER_ITEM_NUM )
			iMaxPos++;

		pScroll->SetPageRange(2);
		pScroll->SetScrollRange( 0, iMaxPos );
		pScroll->SetScrollPos( m_iCurPage );
	}
}

void ChangeTradeStateWnd::SetExtraItemInfoVec( DWORD dwTabID )
{
	m_vInventoryItemInfoList.clear();

	ioUserExtraItem *pItem = g_MyInfo.GetUserExtraItem();
	if( !pItem ) return;

	int iSize = pItem->GetExtraItemCnt();
	for( int i=0; i < iSize; ++i )
	{
		EXTRAITEMSLOT kSlot;
		if( !pItem->GetExtraItemArray( i, kSlot ) )
			continue;

		if( kSlot.m_PeriodType != EPT_MORTMAIN )
			continue;

		if( kSlot.m_iTradeState != EET_NORMAL )
			continue;

		int iGroup = kSlot.m_iItemCode/DEFAULT_BASIC_ITEM_CODE + 1;
		if( !IsRightTab( dwTabID, iGroup ) )
			continue;

		if( kSlot.m_bCharEquip )
			continue;

		InventoryItemInfo kInfo;
		kInfo.m_iMainType = INVENTORY_ITEM_EXTRAITEM;
		kInfo.m_iMagicCode = kSlot.m_iIndex;
		kInfo.m_bEquip = kSlot.m_bCharEquip;

		int iSlot = kSlot.m_iItemCode/DEFAULT_BASIC_ITEM_CODE;
		int iValue = kSlot.m_iReinforce;

		if( !kSlot.m_bCharEquip )
			iValue += 100;

		kInfo.m_iPriorityOrder = iSlot * 10000 + iValue;
		kInfo.m_iPriorityOrder2 = kSlot.m_iItemCode;

		m_vInventoryItemInfoList.push_back( kInfo );
	}

	if( m_vInventoryItemInfoList.empty() )
		return;

	// 각 하위텝 마다 다르게 정렬한다.
	if( dwTabID == ID_TAB_1_BTN )	// 전체
	{
		// m_iMagicCode 역순으로 정렬
		std::sort( m_vInventoryItemInfoList.begin(), m_vInventoryItemInfoList.end(), InventoryItemInfoSort2() );
	}
	else
	{
		std::sort( m_vInventoryItemInfoList.begin(), m_vInventoryItemInfoList.end(), InventoryItemInfoSort3() );
	}
}

void ChangeTradeStateWnd::OnRender()
{
	ioMovingWnd::OnRender();
}

void ChangeTradeStateWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;

	ProcessCustomBtnTooltip();
	ioMovingWnd::OnProcess( fTimePerSec );
}

void ChangeTradeStateWnd::ProcessCustomBtnTooltip()
{
	int i = 0;
	EXTRAITEMSLOT kSlot;
	ioComplexStringPrinter kPrinter[GLOBAL_TEXT_TOOLTIP_LINE];
	for(i = 0;i < MAX_ITEM_INFO_WND_BTN;i++)
	{
		TradeItemInfoWnd *pItemWnd = dynamic_cast<TradeItemInfoWnd*>(FindChildWnd( ID_ITEM_INFO_WND_1_BTN+i ));
		if( !pItemWnd ) continue;
		if( !pItemWnd->IsShow() ) continue;
		if( !pItemWnd->IsCustomToolTip() ) continue;

		InventoryItemInfo kItemInfo = pItemWnd->GetInfo();

		ioUserExtraItem *pUserExtraItem = g_MyInfo.GetUserExtraItem();
		if( !pUserExtraItem ) continue;

		pUserExtraItem->GetExtraItem( kItemInfo.m_iMagicCode, kSlot );		
		break;
	}

	if( kSlot.m_dwMaleCustom > 0 || kSlot.m_dwFemaleCustom > 0 )
	{
		kPrinter[0].SetTextStyle( TS_NORMAL );
		kPrinter[0].SetBkColor( 0, 0, 0 );
		kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
		kPrinter[0].AddTextPiece( FONT_SIZE_12, STR(1) );

		if( kSlot.m_dwMaleCustom > 0 && kSlot.m_dwFemaleCustom > 0 )
		{
			kPrinter[0].SetTextColor( TCT_DEFAULT_BLUE );
			kPrinter[0].AddTextPiece( FONT_SIZE_12, STR(2) );
			kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
			kPrinter[0].AddTextPiece( FONT_SIZE_12, ", " );
			kPrinter[0].SetTextColor( TCT_DEFAULT_RED );
			kPrinter[0].AddTextPiece( FONT_SIZE_12, STR(3) );
		}		
		else if( kSlot.m_dwMaleCustom > 0 )
		{
			kPrinter[0].SetTextColor( TCT_DEFAULT_BLUE );
			kPrinter[0].AddTextPiece( FONT_SIZE_12, STR(4) );
		}
		else if( kSlot.m_dwFemaleCustom > 0 )
		{
			kPrinter[0].SetTextColor( TCT_DEFAULT_RED );
			kPrinter[0].AddTextPiece( FONT_SIZE_12, STR(5) );
		}
		kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
		kPrinter[0].AddTextPiece( FONT_SIZE_12, STR(6) );

		kPrinter[1].SetTextStyle( TS_NORMAL );
		kPrinter[1].SetBkColor( 0, 0, 0 );
		kPrinter[1].SetTextColor( TCT_DEFAULT_GRAY );
		kPrinter[1].AddTextPiece( FONT_SIZE_12, STR(7) );
		kPrinter[1].SetTextColor( TCT_DEFAULT_GREEN );
		kPrinter[1].AddTextPiece( FONT_SIZE_12, STR(8) );
		kPrinter[1].SetTextColor( TCT_DEFAULT_GRAY );
		kPrinter[1].AddTextPiece( FONT_SIZE_12, STR(9) );
	}

	ItemCustomThumbnailTooltip *pTooltip = dynamic_cast<ItemCustomThumbnailTooltip*>(g_GUIMgr.FindWnd( TRADE_ITEM_CHANGE_TOOLTIP ));
	if( pTooltip )
	{
		pTooltip->SetTooltipText( kPrinter, kSlot );
	}
}

int ChangeTradeStateWnd::GetTabType( DWORD dwTabID )
{
	int iArray = ( dwTabID - ID_TAB_1_BTN );
	if( !COMPARE( iArray, 0, MAX_TAB_BTN ) )
		return -1;

	switch( dwTabID )
	{
	case ID_TAB_2_BTN:
		return 1;
	case ID_TAB_3_BTN:
		return 2;
	case ID_TAB_4_BTN:
		return 3;
	case ID_TAB_5_BTN:
		return 4;
	}

	return -1;
}

bool ChangeTradeStateWnd::IsRightTab( DWORD dwTabID, int iValue1 )
{
	if( dwTabID == ID_TAB_1_BTN )
		return true;
	else if( iValue1 == GetTabType(dwTabID) )
		return true;

	return false;
}

bool ChangeTradeStateWnd::GetCurItemInfo( OUT InventoryItemInfo &rkInfo, IN DWORD dwItemInfoID )
{
	if( !COMPARE( dwItemInfoID, ID_ITEM_INFO_WND_1_BTN, ID_ITEM_INFO_WND_8_BTN+1) )
		return false;

	int iStartArray = m_iCurPage * PAGE_PER_ITEM_NUM;
	int iCurArray   = iStartArray + ( dwItemInfoID - ID_ITEM_INFO_WND_1_BTN );
	int iSize       = m_vInventoryItemInfoList.size();
	if( !COMPARE( iCurArray, 0, iSize ) )
		return false;

	rkInfo = m_vInventoryItemInfoList[iCurArray];
	return true;
}

void ChangeTradeStateWnd::SendTradeStateChange( int iIndex )
{
	SP2Packet kPacket( CTPK_ETCITEM_USE );
	kPacket << (int)ioEtcItem::EIT_ETC_TRADE_STATE_CHANGE;
	kPacket << iIndex;
	TCPNetwork::SendToServer( kPacket );
	TCPNetwork::MouseBusy( true );

	HideWnd();
}