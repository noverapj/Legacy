#include "StdAfx.h"

#include "BillingWantWnd.h"

#include "../ioPlayStage.h"

BillingWantWnd::BillingWantWnd()
{
	m_pPlayStage	= NULL;

	m_pBottomFrm	= NULL;
	m_pIcon			= NULL;
	m_pIconBack		= NULL;

	m_bPresent		= false;
}

BillingWantWnd::~BillingWantWnd()
{
	SAFEDELETE( m_pBottomFrm );
	SAFEDELETE( m_pIcon );
	SAFEDELETE( m_pIconBack );
}

void BillingWantWnd::SetPlayStage( ioPlayStage *pPlayStage )
{
	m_pPlayStage = pPlayStage;
}

void BillingWantWnd::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( pFrame )
	{
		if( szType == "BottomFrm" )
		{
			SAFEDELETE( m_pBottomFrm );
			m_pBottomFrm = pFrame;
		}
		else
		{	
			ioWnd::AddRenderFrame( szType, pFrame );
		}
	}
}

void BillingWantWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "Icon" )
	{
		SAFEDELETE( m_pIcon );
		m_pIcon = pImage;
	}
	else if( szType == "IconBack" )
	{
		SAFEDELETE( m_pIconBack );
		m_pIconBack = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );	
	}
}

void BillingWantWnd::iwm_show()
{
	ReSizeUI( m_bPresent );

	if( g_MyInfo.GetChannelingType() == CNT_NEXON )
	{
		SetChildInActive( ID_GOLD_CHARGE );
		SetChildInActive( ID_GOLD_CHARGE_PRESENT );
	}
}

void BillingWantWnd::iwm_hide()
{
	m_bPresent = false;
}

void BillingWantWnd::ShowPresent()
{
	m_bPresent = true;
	ShowWnd(); 
}

void BillingWantWnd::ReSizeUI( bool bPresent )
{
	int iWidth  = 0;
	int iHeight = 0;
	int iXBtnXPos = 0;
	if( bPresent )
	{
		iWidth  = PRESENT_WIDTH;
		iHeight = PRESENT_HEIGHT;
		ShowChildWnd( ID_GOLD_CHARGE_PRESENT );
		ShowChildWnd( ID_CANCEL_PRESENT );
		HideChildWnd( ID_GOLD_CHARGE );
		HideChildWnd( ID_CANCEL );
		iXBtnXPos = PRESENT_X_OFFSET;
	}
	else
	{
		iWidth  = NORMAL_WIDTH;
		iHeight = NORMAL_HEIGHT;
		ShowChildWnd( ID_GOLD_CHARGE );
		ShowChildWnd( ID_CANCEL );
		HideChildWnd( ID_GOLD_CHARGE_PRESENT );
		HideChildWnd( ID_CANCEL_PRESENT );
		iXBtnXPos = NORMAL_X_OFFSET;
	}

	SetSize( iWidth, iHeight );

	ioWnd *pWnd = FindChildWnd( ID_EXIT );
	if( pWnd )
		pWnd->SetWndPos( iXBtnXPos, pWnd->GetYPos() );

	if( m_pBottomFrm )
		m_pBottomFrm->SetSize( iWidth, m_pBottomFrm->GetHeight() );

}

void BillingWantWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();

	switch( dwID )
	{
	case ID_EXIT:
	case ID_CANCEL:
	case ID_CANCEL_PRESENT:
		if( cmd == IOBN_BTNUP )
			HideWnd();
		break;
	case ID_GOLD_CHARGE:
	case ID_GOLD_CHARGE_PRESENT:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();

			if( g_App.GetConnectedMoveMode() == ioApplication::CMM_LOBBY )
			{
				if( m_pPlayStage && m_pPlayStage->GetModeType() != MT_MYROOM )
				{
					g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
					return;
				}
			}
			else
			{
				if( m_pPlayStage && m_pPlayStage->GetModeType() != MT_TRAINING && m_pPlayStage->GetModeType() != MT_HEADQUARTERS && m_pPlayStage->GetModeType() != MT_HOUSE )
				{
					g_GUIMgr.SetMsgBox( MB_OK, NULL, "골드충전은 광장/개인본부/훈련소에서만 가능합니다." );
					return;
				}
			}

			ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
			if( pLocal )
				pLocal->OpenFillCashPage();
		}
		break;
	}
}

bool BillingWantWnd::iwm_spacebar()
{
	ioWnd *pChargeBtn = FindChildWnd( ID_GOLD_CHARGE );
	if( pChargeBtn && pChargeBtn->IsShow() )
	{
		iwm_command( pChargeBtn, IOBN_BTNUP, 0 );
		return true;
	}

	pChargeBtn = FindChildWnd( ID_GOLD_CHARGE_PRESENT );
	if( pChargeBtn && pChargeBtn->IsShow() )
	{
		iwm_command( pChargeBtn, IOBN_BTNUP, 0 );
		return true;
	}

	return false;
}

void BillingWantWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );

	int iIconXOffset = 0;
	if( m_bPresent )
	{
		int iTextYPos = iYPos + Y_OFFSET;
		g_FontMgr.PrintText( iXPos + X_PRESENT_OFFSET, iTextYPos, FONT_SIZE_13, STR(1) );
		g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
		iTextYPos += Y_LINE_GAP;
		g_FontMgr.PrintText( iXPos + X_PRESENT_OFFSET, iTextYPos, FONT_SIZE_13, STR(2) );
		iTextYPos += Y_LINE_GAP;
		g_FontMgr.PrintText( iXPos + X_PRESENT_OFFSET, iTextYPos, FONT_SIZE_13, STR(3) );
		iTextYPos += Y_LINE_GAP;
		g_FontMgr.PrintText( iXPos + X_PRESENT_OFFSET, iTextYPos, FONT_SIZE_13, STR(4) );

		char szNumComma[MAX_PATH]="";
		Help::ConvertNumToStrComma( g_MyInfo.GetPurchasedCash(), szNumComma, sizeof( szNumComma ) );
		iTextYPos += Y_LINE_GAP;
		g_FontMgr.SetTextColor( TCT_DEFAULT_GREEN );
		g_FontMgr.PrintTextWidthCut( iXPos + X_PRESENT_OFFSET, iTextYPos, FONT_SIZE_13, WIDTH_CUT, STR(5), szNumComma );
		iIconXOffset = ICON_X_PRESENT_OFFSET;
	}
	else
	{
		int iTextYPos = iYPos + Y_OFFSET;
		g_FontMgr.PrintText( iXPos + X_OFFSET, iTextYPos, FONT_SIZE_13, STR(6) );
		g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
		iTextYPos += Y_LINE_GAP;
		g_FontMgr.PrintText( iXPos + X_OFFSET, iTextYPos, FONT_SIZE_13, STR(7) );
		iIconXOffset = ICON_X_OFFSET;
	}

	if( m_pIconBack )
		m_pIconBack->Render( iXPos + iIconXOffset, iYPos + ICON_Y_OFFSET, UI_RENDER_MULTIPLY );

	if( m_pIcon )
		m_pIcon->Render( iXPos + iIconXOffset, iYPos + ICON_Y_OFFSET );

	if( m_pBottomFrm )
		m_pBottomFrm->Render( iXPos, iYPos + GetHeight() - m_pBottomFrm->GetHeight() , UI_RENDER_MULTIPLY );
}