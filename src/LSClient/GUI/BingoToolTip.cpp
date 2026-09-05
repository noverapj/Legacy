
#include "StdAfx.h"

#include "../ioBingo.h"
#include "BingoToolTip.h"

BingoToolTip::BingoToolTip()
{
	m_pIconBg      = NULL;
	m_pIcon        = NULL;
	m_pSubIcon = NULL;
	m_pHiddenIcon  = NULL;
	m_iManualIndex = 0;
	m_szTtitle.Clear();

	m_bHidden = false;
}

BingoToolTip::~BingoToolTip()
{
	SAFEDELETE( m_pIconBg );
	SAFEDELETE( m_pHiddenIcon );
}

void BingoToolTip::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "IconBg" )
	{
		SAFEDELETE( m_pIconBg );
		m_pIconBg = pImage;
	}
	else
		AddRenderImage( szType, pImage );
}

void BingoToolTip::iwm_show()
{
}

void BingoToolTip::iwm_hide()
{
	m_pIcon = NULL;
	m_pSubIcon = NULL;
	m_iManualIndex = 0;
	m_szTtitle.Clear();
	m_bHidden = false;
}

void BingoToolTip::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;

	UpdateWndSize();

	ioLSMouse *pMouse = g_App.GetMouse();
	if( !pMouse )
		return;
	
	pMouse->SetToolTipCursor();
	POINT pt = pMouse->GetMousePos();
	int iXPos = pt.x + 8;
	int iYPos = pt.y + 8;
	int iYOffset = pt.y + GetHeight() + 8;
	if( Setting::Height() < iYOffset )
	{
		iYPos -= ( iYOffset - Setting::Height() ) + 8;
	}

	SetScreenWndPos( iXPos, iYPos );
}

void BingoToolTip::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	enum { ICON_XOFFSET = 48, ICON_YOFFSET = 49,
		   PRINT_XOFFSET = 14, FIRST_YOFFSET = 99, };

	if( m_pIconBg )
		m_pIconBg->Render( iXPos + ICON_XOFFSET, iYPos + ICON_YOFFSET, UI_RENDER_MULTIPLY );

	if( !m_bHidden && m_pIcon )
	{
		m_pIcon->SetScale( FLOAT1 );
		m_pIcon->Render( iXPos + ICON_XOFFSET, iYPos + ICON_YOFFSET );
		if( m_pSubIcon )
		{
			m_pSubIcon->SetScale( FLOAT1 );
			m_pSubIcon->Render( iXPos + ICON_XOFFSET, iYPos + ICON_YOFFSET );
		}
	}
	else if( m_bHidden && m_pHiddenIcon )
	{
		m_pHiddenIcon->SetScale( FLOAT1 );
		m_pHiddenIcon->Render( iXPos + ICON_XOFFSET, iYPos + ICON_YOFFSET );
	}

	PrintManual( iXPos + PRINT_XOFFSET, iYPos + FIRST_YOFFSET, FONT_SIZE_12);
}

void BingoToolTip::PrintManual( int iXPos, int iYPos, float fScale )
{
	ioBingo *pBingo = g_MyInfo.GetBingo();
	if( !pBingo )
		return;

	if( !m_szTtitle.IsEmpty() && !m_bHidden )
	{
		ioComplexStringPrinter kTitle;
		kTitle.SetTextStyle( TS_NORMAL );
		kTitle.SetBkColor( 0, 0, 0 );	
		kTitle.SetTextColor( TCT_DEFAULT_DARKGRAY );
		kTitle.AddTextPiece( FONT_SIZE_17, "%s", m_szTtitle.c_str() );
		kTitle.PrintFullTextWidthCut( GetDerivedPosX() + 99, GetDerivedPosY() + 41, TAT_LEFT, 132 );
	}

	ioComplexStringPrinter kPrinter;
	int iMaxLine = pBingo->GetToolTipMaxLine( m_iManualIndex );
	for( int iLine=0; iLine<iMaxLine; ++iLine )
	{
		pBingo->SetPrinter( m_iManualIndex, iLine, fScale, kPrinter );
		enum { MAX_LOOP = 20, Y_GAP = 18, };
		bool bFirstGap = true;
		for ( int i = 0; i < MAX_LOOP ; i++ )
		{
			if( kPrinter.GetFullWidth() < DESC_WIDTH )
			{
				if( bFirstGap )
				{
					iYPos += pBingo->GetToolTipGetSpaceLine( m_iManualIndex, iLine );
					bFirstGap = false;
				}
				else
					iYPos += Y_GAP;
				kPrinter.PrintFullText( iXPos, iYPos, TAT_LEFT );
				kPrinter.ClearList();
				break;
			}

			ioComplexStringPrinter kPrinterA, kPrinterB;
			kPrinter.SplitFullText( kPrinterA, kPrinterB, DESC_WIDTH );
			if( bFirstGap )
			{
				iYPos += pBingo->GetToolTipGetSpaceLine( m_iManualIndex, iLine );
				bFirstGap = false;
			}
			else
				iYPos += Y_GAP;

			kPrinterA.PrintFullText( iXPos, iYPos, TAT_LEFT );
			kPrinterB.CopyFullText( kPrinter );
			kPrinterA.ClearList();
			kPrinterB.ClearList();
		}
	}
}

void BingoToolTip::UpdateWndSize()
{
	ioBingo *pBingo = g_MyInfo.GetBingo();
	if( !pBingo )
		return;

	ioComplexStringPrinter kPrinter;
	int iTotalLineGap = 18;
	int iMaxLine = pBingo->GetToolTipMaxLine( m_iManualIndex );

	for( int iLine = 0; iLine < iMaxLine; ++iLine )
	{
		enum { MAX_LOOP = 20, Y_GAP = 18, };
		bool bFirstGap = true;
		for( int i = 0; i < MAX_LOOP ; i++ )
		{
			if( kPrinter.GetFullWidth() < DESC_WIDTH )
			{
				if( bFirstGap )
				{
					iTotalLineGap += pBingo->GetToolTipGetSpaceLine( m_iManualIndex, iLine );
					bFirstGap = false;
				}
				else
					iTotalLineGap += Y_GAP;

				kPrinter.ClearList();
				break;
			}

			ioComplexStringPrinter kPrinterA, kPrinterB;
			kPrinter.SplitFullText( kPrinterA, kPrinterB, DESC_WIDTH );
			if( bFirstGap )
			{
				iTotalLineGap += pBingo->GetToolTipGetSpaceLine( m_iManualIndex, iLine );
				bFirstGap = false;
			}
			else
				iTotalLineGap += Y_GAP;

			kPrinterB.CopyFullText( kPrinter );
			kPrinterA.ClearList();
			kPrinterB.ClearList();
		}
	}

	SetSize( GetWidth(), 123 + iTotalLineGap - 12 );
}

void BingoToolTip::SetInfo( int iIndex, int iManualIndex, ToolTipType eType )
{
	if( iIndex == 0 )
		return;

	if( !IsShow() )
		ShowWnd();

	m_ToolTipType = eType;

	ioBingo *pBingo = g_MyInfo.GetBingo();
	if( pBingo )
	{
		m_bHidden = pBingo->IsRewardHidden( iIndex );
		if( m_bHidden )
		{
			SAFEDELETE( m_pHiddenIcon );
			m_pHiddenIcon = g_UIImageSetMgr.CreateImageByFullName( pBingo->GetHiddenIconName() );
		}
		else
		{
			m_pSubIcon = NULL;
			switch( m_ToolTipType )
			{
			case TTT_REWARD:
				{
					m_pIcon        = pBingo->GetRewardIcon( iIndex );
					m_szTtitle     = pBingo->GetRewardName( iIndex );
					m_iManualIndex = iManualIndex;
				}
				break;
			case TTT_ALL_BINGO_REWARD:
				{
					m_pIcon        = pBingo->GetAllBingoRewardIcon( iIndex );
					m_szTtitle     = pBingo->GetAllBingoRewardName( iIndex );
					m_iManualIndex = iManualIndex;
				}
				break;
			case TTT_DUMMY:
				{
					m_pIcon        = pBingo->GetBingoDummyIcon( iIndex );
					m_pSubIcon = pBingo->GetBingoSubIcon( iIndex );
					m_szTtitle     = pBingo->GetBingoDummyName( iIndex );
					m_iManualIndex = pBingo->GetBingoDummyManualIndex( iIndex );
				}
				break;
			}
		}
	}	
}