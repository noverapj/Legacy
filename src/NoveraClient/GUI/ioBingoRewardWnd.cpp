

#include "StdAfx.h"

#include "../io3DEngine/ioWnd.h"
#include "../io3DEngine/ioFontManager.h"
#include "../io3DEngine/ioStringConverter.h"
#include "../io3DEngine/ioUIRenderImage.h"
#include "../io3DEngine/ioUIRenderFrame.h"
#include "../io3DEngine/ioUIImageSetManager.h"
#include "../io3DEngine/ioUITitle.h"
#include "../io3DEngine/ioSoundManager.h"

#include "../io3DEngine/ioXMLDocument.h"
#include "../io3DEngine/ioXMLElement.h"

#include "../WndID.h"
#include "../ioMyInfo.h"
#include "../ioBingo.h"

#include "ioBingoRewardWnd.h"
#include "ioSP2GUIManager.h"

#include <strsafe.h>
#include "../io3DEngine/ioStringManager.h"

//////////////////////////////////////////////////////////////////////////

ioBingoIconButton::ioBingoIconButton()
{
	m_Index			= 0;
	m_pIcon			= NULL;
	m_pIconBack	= NULL;
	m_pIconEmpty	= NULL;

	m_szName.Clear();
}

ioBingoIconButton::~ioBingoIconButton()
{
	SAFEDELETE( m_pIcon );
	SAFEDELETE( m_pIconBack );
	SAFEDELETE( m_pIconEmpty );
}

void ioBingoIconButton::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "IconBack" )
	{
		SAFEDELETE( m_pIconBack );
		m_pIconBack = pImage;
	}
	else if( szType == "IconEmpty" )
	{
		SAFEDELETE( m_pIconEmpty );
		m_pIconEmpty = pImage;
	}
	else
	{
		ioButton::AddRenderImage( szType, pImage );	
	}
}

void ioBingoIconButton::SetBingoInfo( int Index )
{
	ioBingo* pBingo = g_MyInfo.GetBingo(); 
	if( !pBingo )
		return;

	m_Index = Index + 1;
	SAFEDELETE( m_pIcon );
	if( m_Index <= pBingo->GetAllBingoRewardListCount() )
	{
		const ioBingo::RewardInfo& rkInfo = pBingo->GetAllBingoRewardInfo( m_Index );
		m_pIcon = g_UIImageSetMgr.CreateImageByFullName( rkInfo.m_IconName );
		
		m_szName = pBingo->GetAllBingoRewardName( m_Index );
	}
	else
	{
		ioBingo::RewardInfo Info = pBingo->GetRewardInfo( m_Index - pBingo->GetAllBingoRewardListCount());
		if( Info.m_bHidden )
		{
			m_pIcon = g_UIImageSetMgr.CreateImageByFullName( pBingo->GetHiddenIconName() );
			m_szName = "???";
		}
		else
		{
			m_pIcon = g_UIImageSetMgr.CreateImageByFullName( Info.m_IconName );
			m_szName = pBingo->GetRewardName( m_Index - pBingo->GetAllBingoRewardListCount() );
		}
	}
}

void ioBingoIconButton::OnRender()
{
	ioButton::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	enum { X_TEXT_OFFSET = 32, Y_TEXT_OFFSET = 79, };
	g_FontMgr.PrintTextWidthCut( iXPos+X_TEXT_OFFSET, iYPos+Y_TEXT_OFFSET, FONT_SIZE_13, 104.0f, m_szName.c_str() );

	OnIconRender( iXPos, iYPos );
}

void ioBingoIconButton::OnIconRender( int iXPos, int iYPos )
{
	if( m_pIconBack )
		m_pIconBack->Render( iXPos + GetWidth()/2, iYPos + GetHeight()/2, UI_RENDER_MULTIPLY, TFO_BILINEAR );

	if(  m_szName.IsEmpty() )
	{
		if( m_pIconEmpty )
			m_pIconEmpty->Render( iXPos + GetWidth()/2, iYPos + GetHeight()/2, UI_RENDER_MULTIPLY );
	}
	else 
	{
		if( m_pIcon )
			m_pIcon->Render( iXPos + GetWidth()/2, iYPos + GetHeight()/2, UI_RENDER_NORMAL,  TFO_BILINEAR );
	}
}


//////////////////////////////////////////////////////////////////////////

ioBingoRewardWnd::ioBingoRewardWnd()
{
	m_iMaxPage = 0;
	m_iCurPage = 0;
}

ioBingoRewardWnd::~ioBingoRewardWnd()
{
}

void ioBingoRewardWnd::iwm_show()
{
	m_iMaxPage = 0;

	ioBingo* pBingo = g_MyInfo.GetBingo();
	if( pBingo )
	{
		m_iMaxPage = pBingo->GetAllBingoRewardListCount() + pBingo->GetRewardCount();
		m_iMaxPage = m_iMaxPage / ICON_BTN_MAX;
	}
	UpdatePage( 0 );
	GoToTop();
}

void ioBingoRewardWnd::UpdatePage( int iPage )
{
	m_iCurPage = iPage;

	char szBuf[MAX_PATH] = "";
	wsprintf_e( szBuf, "%d/%d", m_iCurPage+1, m_iMaxPage+1 );
	m_szPageText = szBuf;

	SetChildActive( ID_PRE_BTN );
	SetChildActive( ID_NEXT_BTN );

	if( m_iCurPage == 0 )
		SetChildInActive( ID_PRE_BTN );

	if( m_iCurPage == m_iMaxPage )
		SetChildInActive( ID_NEXT_BTN );

	UpdateIconBtn();
}

void ioBingoRewardWnd::UpdateIconBtn()
{
	ioBingo* pBingo = g_MyInfo.GetBingo(); 
	if( !pBingo )
		return;

	int iStartArray = m_iCurPage * ICON_BTN_MAX;
	for( int i = 0; i < ICON_BTN_MAX; ++i )
	{		
		int iCurArray = iStartArray + i;
		if( i < pBingo->GetAllBingoRewardListCount() )
		{
			UpdateIconAllBingoBtn( i, iCurArray );
		}
		else
		{
			UpdateIconBingoBtn( i, iCurArray );
		}		
	}
}

void ioBingoRewardWnd::UpdateIconBingoBtn( int iPos, int index )
{
	ioBingo* pBingo = g_MyInfo.GetBingo();
	if( !pBingo )
		return;

	ioBingoIconButton* pBtn = dynamic_cast<ioBingoIconButton*>( FindChildWnd( ID_ICON_BTN1 + iPos ) );
	if( pBtn )
	{
		pBtn->SetBingoInfo( index );
	}
}

void ioBingoRewardWnd::UpdateIconAllBingoBtn( int iPos, int index )
{
	ioBingo* pBingo = g_MyInfo.GetBingo();
	if( !pBingo )
		return;

	ioBingoIconButton* pBtn = dynamic_cast<ioBingoIconButton*>( FindChildWnd( ID_ICON_BTN1 + iPos ) );
	if( pBtn )
	{
		pBtn->SetBingoInfo( index );
	}	
}

void ioBingoRewardWnd::OnRender()
{
	ioMovingWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	enum
	{
		DESC_X		= 238,
		DESC_Y		= 440,

		LINE_GAP	= 20,

		PAGE_X		= 238,
		PAGE_Y		= 395,

		MAX_TEXT    = 2,
	};

	ioComplexStringPrinter kPrinter[MAX_TEXT];
	kPrinter[0].SetTextStyle( TS_NORMAL);
	kPrinter[0].SetBkColor( 0, 0, 0 );	
	kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY ); 
	kPrinter[0].AddTextPiece( FONT_SIZE_13, STR(1) );

	kPrinter[0].SetTextStyle( TS_NORMAL);
	kPrinter[0].SetBkColor( 0, 0, 0 );	
	kPrinter[0].SetTextColor( TCT_DEFAULT_RED ); 
	kPrinter[0].AddTextPiece( FONT_SIZE_13, STR(2) );

	kPrinter[0].SetTextStyle( TS_NORMAL);
	kPrinter[0].SetBkColor( 0, 0, 0 );	
	kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY ); 
	kPrinter[0].AddTextPiece( FONT_SIZE_13, STR(3) );

	kPrinter[1].SetTextStyle( TS_NORMAL);
	kPrinter[1].SetBkColor( 0, 0, 0 );	
	kPrinter[1].SetTextColor( TCT_DEFAULT_DARKGRAY ); 
	kPrinter[1].AddTextPiece( FONT_SIZE_13, STR(4) );

	kPrinter[1].SetTextStyle( TS_NORMAL);
	kPrinter[1].SetBkColor( 0, 0, 0 );	
	kPrinter[1].SetTextColor( TCT_DEFAULT_RED ); 
	kPrinter[1].AddTextPiece( FONT_SIZE_13, STR(5) );

	kPrinter[1].SetTextStyle( TS_NORMAL);
	kPrinter[1].SetBkColor( 0, 0, 0 );	
	kPrinter[1].SetTextColor( TCT_DEFAULT_DARKGRAY ); 
	kPrinter[1].AddTextPiece( FONT_SIZE_13, STR(6));


	for (int i = 0; i < MAX_TEXT ; i++)
	{	
		kPrinter[i].PrintFullText( iXPos + DESC_X, iYPos + DESC_Y + ( i*LINE_GAP), TAT_CENTER );
	}
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos+PAGE_X, iYPos+PAGE_Y, FONT_SIZE_13, m_szPageText.c_str() );
}

void ioBingoRewardWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();

	switch( dwID )
	{
	case ID_PRE_BTN:
		if( cmd == IOBN_BTNUP && !pWnd->HasWndStyle( IWS_INACTIVE ) )
		{
			int iNewPage = m_iCurPage - 1;
			UpdatePage( iNewPage );
		}
		break;
	case ID_NEXT_BTN:
		if( cmd == IOBN_BTNUP && !pWnd->HasWndStyle( IWS_INACTIVE ) )
		{
			int iNewPage = m_iCurPage + 1;
			UpdatePage( iNewPage );
		}
		break;
	case ID_X:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
		}
		break;
	}
}