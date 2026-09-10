#include "StdAfx.h"

#include "../io3DEngine/ioWnd.h"
#include "../io3DEngine/ioScroll.h"
#include "../io3DEngine/ioXMLDocument.h"
#include "../io3DEngine/ioXMLElement.h"
#include "../io3DEngine/ioFontManager.h"
#include "../io3DEngine/ioUIRenderImage.h"
#include "../io3DEngine/ioUIRenderFrame.h"
#include "../io3DEngine/ioUIImageSetManager.h"
#include "../io3DEngine/Safesprintf.h"
#include "..\io3DEngine\ioINILoader.h"
#include "..\io3DEngine\ioUITitle.h"

#include "../ioMyInfo.h"
#include "../NetworkWrappingFunc.h"
#include "../ioDecorationPrice.h"
#include "../TextColorTable.h"
#include "../WndID.h"
#include "..\ioApplication.h"
#include "..\ioClassPrice.h"

#include "ioUI3DRender.h"
#include "ioSP2GUIManager.h"

#include ".\firstsoldierselectwnd.h"

#include <strsafe.h>

FirstSoldierSelectBtn::FirstSoldierSelectBtn()
{
	m_iMagicCode = -1;
}

FirstSoldierSelectBtn::~FirstSoldierSelectBtn()
{

}
////////////////////////////////////////////////////////////////////////////
FirstSoldierSelectWnd::FirstSoldierSelectWnd(void)
{
	m_eWndType       = WT_START;
	m_pBottomFrm     = NULL;
	m_pStripGrayDark = NULL;
	m_pStripGrayLight= NULL;
	m_pStartImg      = NULL;
	m_pEndTextImg    = NULL;
	m_pEndImg        = NULL;
	m_pDecoBack      = NULL;
	for (int i = 0; i < MAX_DECO ; i++)
	{
		m_pDeco[i] = NULL;	
	}
	m_pDot           = NULL;
}

FirstSoldierSelectWnd::~FirstSoldierSelectWnd(void)
{
	SAFEDELETE( m_pBottomFrm );
	SAFEDELETE( m_pStripGrayDark );
	SAFEDELETE( m_pStripGrayLight );
	SAFEDELETE( m_pStartImg );
	SAFEDELETE( m_pEndTextImg );
	SAFEDELETE( m_pEndImg );
	SAFEDELETE( m_pDecoBack );
	for (int i = 0; i < MAX_DECO ; i++)
	{
		SAFEDELETE( m_pDeco[i] );
	}
	SAFEDELETE( m_pDot );
	Clear();
}


void FirstSoldierSelectWnd::iwm_create()
{
	g_GUIMgr.AddWnd( "XML/FirstSoldierSelectHelpWnd.xml", this );

	for (int i = ID_CHAR_SELECT_BTN; i < ID_UNDERWEAR_SELECT_BTN+1 ; i++)
	{
		ioWnd *pWnd = FindChildWnd( i );
		if( !pWnd )
			continue;
		ioUITitle *pTitle = pWnd->GetTitle();
		if( !pTitle )
			continue;
		pTitle->SetPrintTextStyle( ioUITitle::PTS_WIDTHCUT );
		pTitle->SetWidthCutSize( FSS_BTN_TITLE_SIZE );
	}
}

void FirstSoldierSelectWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "StripGrayDark" )
	{
		SAFEDELETE( m_pStripGrayDark );
		m_pStripGrayDark = pImage;
		if( m_pStripGrayDark )
		{
			m_pStripGrayDark->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
			m_pStripGrayDark->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
		}
	}
	else if( szType == "StripGrayLight" )
	{
		SAFEDELETE( m_pStripGrayLight );
		m_pStripGrayLight = pImage;
		if( m_pStripGrayLight )
		{
			m_pStripGrayLight->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
			m_pStripGrayLight->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
		}
	}
	else if( szType == "StartImg" )
	{
		SAFEDELETE( m_pStartImg );
		m_pStartImg = pImage;
	}
	else if( szType == "EndTextImg" )
	{
		SAFEDELETE( m_pEndTextImg );
		m_pEndTextImg = pImage;
	}
	else if( szType == "EndImg" )
	{
		SAFEDELETE( m_pEndImg );
		m_pEndImg = pImage;
	}
	else if( szType == "DecoBack" )
	{
		SAFEDELETE( m_pDecoBack );
		m_pDecoBack = pImage;
		if( m_pDecoBack )
			m_pDecoBack->SetScale( FSS_DECO_ICON_SCALE, FSS_DECO_ICON_SCALE );
	}
	else if( strcmp(szType.c_str(), "Dot") == 0 )
	{
		SAFEDELETE( m_pDot );
		m_pDot = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );	
	}	
}

void FirstSoldierSelectWnd::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
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

void FirstSoldierSelectWnd::ParseExtraInfo( ioXMLElement &xElement )
{
}

void FirstSoldierSelectWnd::OnRender()
{
	ioWnd::OnRender();

	if( m_eWndType == WT_START )
		OnRenderStart();
	else if( m_eWndType == WT_SELECT )
		OnRenderSelect();
	else if( m_eWndType == WT_END )
		OnRenderEnd();
}

void FirstSoldierSelectWnd::OnRenderAfterChild()
{
	if( m_eWndType == WT_SELECT )
		OnRenderSelectAfterChild();
}


void FirstSoldierSelectWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;	

	NewShopPullDownWnd *pPullDownWnd = dynamic_cast<NewShopPullDownWnd *>( FindChildWnd( ID_PULLDOWN_WND ));
	if( !pPullDownWnd ) return;
	NewShopHelpWnd *pCharWnd = dynamic_cast<NewShopHelpWnd *>( FindChildWnd( ID_SOLDIER_WND ));
	if( !pCharWnd ) return;

	if( !pPullDownWnd->IsShow() )
	{
		if( pCharWnd->IsReleaseHelm() == false || pCharWnd->IsReleaseAllItem() == true || pCharWnd->IsShopMotion() == false )
		{
			pCharWnd->SetZPosition( 600.0f );
			pCharWnd->SetChar( m_CharInfo );
			pCharWnd->SetCharScale( FSS_CHAR_SCALE );
			pCharWnd->SetEquip( true, false, true );
		}
	}
	else if( pPullDownWnd->GetOpenBtn() && pPullDownWnd->GetListArray() == -1 )
	{
		if( pPullDownWnd->GetOpenBtn()->GetID() == ID_CHAR_SELECT_BTN )
		{
			if( pCharWnd->IsReleaseHelm() == true || pCharWnd->IsReleaseAllItem() == true || pCharWnd->IsShopMotion() == false )
			{
				pCharWnd->SetZPosition( 600.0f );
				pCharWnd->SetChar( m_CharInfo );
				pCharWnd->SetCharScale( FSS_CHAR_SCALE );
				pCharWnd->SetEquip( false, false, true );
			}
		}
		else if( pPullDownWnd->GetOpenBtn()->GetID() == ID_UNDERWEAR_SELECT_BTN )
		{
			if( pCharWnd->IsReleaseHelm() == false || pCharWnd->IsReleaseAllItem() == false || pCharWnd->IsShopMotion() == true )
			{
				pCharWnd->SetZPosition( 600.0f );
				pCharWnd->SetChar( m_CharInfo );
				pCharWnd->SetCharScale( FSS_CHAR_SCALE );
				pCharWnd->SetEquip( true, true, false );
			}
		}
		else
		{
			if( pCharWnd->IsReleaseHelm() == false || pCharWnd->IsReleaseAllItem() == true || pCharWnd->IsShopMotion() == true )
			{
				pCharWnd->SetZPosition( 600.0f );
				pCharWnd->SetChar( m_CharInfo );
				pCharWnd->SetCharScale( FSS_CHAR_SCALE );
				pCharWnd->SetEquip( true, false, false );
			}
		}
	}
}

void FirstSoldierSelectWnd::ChangeWnd( FSSWndType eWndType )
{
	int iSize = m_vDesc.size();

	int iWidthStrip   = 0;
	int iHeightStrip  = 0;
	int iWidthBottom  = 0;
	int iHeightBottom = 0;
	int iTilteArray   = -1;
	int iBtnTitleArray= -1;
	if( eWndType == WT_START )
	{
		m_eWndType = WT_START;	

		iWidthStrip   = 276;
		iHeightStrip  = 18;
		iWidthBottom  = 526;
		iHeightBottom = 65;

		iTilteArray    = START_TITLE_ARRAY;
		iBtnTitleArray = START_SELECT_BTN_ARRAY;

		for (int i = ID_CHAR_SELECT_BTN; i < ID_UNDERWEAR_SELECT_BTN+1 ; i++)
			HideChildWnd( i );
		for (int i = ID_SOLDIER_WND; i < ID_PULLDOWN_WND+1 ; i++)
			HideChildWnd( i );
	}
	else if( eWndType == WT_SELECT )
	{
		m_eWndType = WT_SELECT;

		iWidthStrip   = 230;
		iHeightStrip  = 24;
		iWidthBottom  = 526;
		iHeightBottom = 164;

		iTilteArray    = SELECT_TITLE_ARRAY;
		iBtnTitleArray = START_SELECT_BTN_ARRAY;

		for (int i = ID_CHAR_SELECT_BTN; i < ID_UNDERWEAR_SELECT_BTN+1 ; i++)
			ShowChildWnd( i );
		for (int i = ID_SOLDIER_WND; i < ID_PULLDOWN_WND+1 ; i++)
			ShowChildWnd( i );

		int iClassType = GetFirstClassType();
		SetAllDefaultTitleAndCharInfoByCharKindred( m_CharInfo, ID_CHAR_SELECT_BTN, iClassType );

		NewShopHelpWnd *pCharWnd = dynamic_cast<NewShopHelpWnd *>( FindChildWnd( ID_SOLDIER_WND ));
		if( pCharWnd )
		{
			pCharWnd->SetZPosition( 600.0f );
			pCharWnd->SetChar( m_CharInfo );
			pCharWnd->SetCharScale( FSS_CHAR_SCALE );
			pCharWnd->SetEquip( true, false, true );
			pCharWnd->ShowWnd();
		}
	}
	else if( eWndType == WT_END )
	{
		
		m_eWndType = WT_END;

		iWidthStrip   = 472;
		iHeightStrip  = 18;
		iWidthBottom  = 526;
		iHeightBottom = 65;

		iTilteArray    = END_TITLE_ARRAY;
		iBtnTitleArray = END_BTN_ARRAY;

		for (int i = ID_CHAR_SELECT_BTN; i < ID_UNDERWEAR_SELECT_BTN+1 ; i++)
			HideChildWnd( i );
		for (int i = ID_SOLDIER_WND; i < ID_PULLDOWN_WND+1 ; i++)
			HideChildWnd( i );
	}

	if( m_pStripGrayDark )
		m_pStripGrayDark->SetSize( iWidthStrip, iHeightStrip );

	if( m_pStripGrayLight )
		m_pStripGrayLight->SetSize( iWidthStrip, iHeightStrip );

	if( m_pBottomFrm )
		m_pBottomFrm->SetSize( iWidthBottom, iHeightBottom );

	if( COMPARE( iTilteArray, 0, iSize ) )
		SetTitleText( m_vDesc[iTilteArray].c_str() );

	if( COMPARE( iBtnTitleArray,0,  iSize ) )
	{
		SetChildTitleText( ID_NEXT_BTN, m_vDesc[iBtnTitleArray].c_str() );
	}
}

void FirstSoldierSelectWnd::OnRenderStart()
{
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	// img
	enum { X_IMG_OFFSET = 25, Y_IMG_OFFSET = 46,  };
	if( m_pStartImg )
		m_pStartImg->Render( iXPos + X_IMG_OFFSET, iYPos + Y_IMG_OFFSET );

	// back strip
	enum { MAX_LOOP = 8, X_OFFSET = 223, Y_OFFSET = 79, Y_GAP = 18, };
	int iLineCnt = 0;
	for (int i = 0; i < MAX_LOOP ; i++)
	{
		if( m_pStripGrayDark )
			m_pStripGrayDark->Render( iXPos + X_OFFSET, iYPos + Y_OFFSET + ( Y_GAP*iLineCnt ), UI_RENDER_MULTIPLY );
		iLineCnt++;
		if( m_pStripGrayLight )
			m_pStripGrayLight->Render( iXPos + X_OFFSET, iYPos + Y_OFFSET + ( Y_GAP*iLineCnt ), UI_RENDER_MULTIPLY );	
		iLineCnt++;
	}

	// bottom
	if( m_pBottomFrm )
		m_pBottomFrm->Render( iXPos, iYPos + GetHeight() - m_pBottomFrm->GetHeight() , UI_RENDER_MULTIPLY );

	// text
	enum { X_DESC_OFFSET = 227, Y_DESC_OFFSET = 81, Y_DESC_GAP = 18, };
	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );

	int iSize = m_vDesc.size();
	for (int i = 0; i < MAX_START_DESC ; i++)
	{
		if( !COMPARE( i, 0, iSize ) )
			break;

		g_FontMgr.PrintText( iXPos + X_DESC_OFFSET, iYPos + Y_DESC_OFFSET + ( Y_DESC_GAP*i ), FONT_SIZE_13, m_vDesc[i].c_str() );
	}
}

void FirstSoldierSelectWnd::OnRenderSelect()
{
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	// back strip
	enum { MAX_LOOP = 5, X_OFFSET = 279, Y_OFFSET = 54, Y_GAP = 24, };
	int iLineCnt = 0;
	for (int i = 0; i < MAX_LOOP ; i++)
	{
		if( m_pStripGrayDark )
			m_pStripGrayDark->Render( iXPos + X_OFFSET, iYPos + Y_OFFSET + ( Y_GAP*iLineCnt ), UI_RENDER_MULTIPLY );
		iLineCnt++;
		if( m_pStripGrayLight )
			m_pStripGrayLight->Render( iXPos + X_OFFSET, iYPos + Y_OFFSET + ( Y_GAP*iLineCnt ), UI_RENDER_MULTIPLY );	
		iLineCnt++;
	}

	// bottom
	if( m_pBottomFrm )
		m_pBottomFrm->Render( iXPos, iYPos + GetHeight() - m_pBottomFrm->GetHeight() , UI_RENDER_MULTIPLY );

	// text
	enum { X_DESC_OFFSET = 23, Y_DESC_OFFSET = 325, Y_DESC_GAP = 18, };
	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );

	int iSize = m_vDesc.size();
	int iGapCnt = 0;
	for (int i = SELECT_FIRST_ARRAY; i < SELECT_FIRST_ARRAY+MAX_SELECT_DESC ; i++)
	{
		if( !COMPARE( i, 0, iSize ) )
			break;

		g_FontMgr.PrintText( iXPos + X_DESC_OFFSET, iYPos + Y_DESC_OFFSET + ( Y_DESC_GAP*iGapCnt ), FONT_SIZE_13, m_vDesc[i].c_str() );
		iGapCnt++;
	}

	// slect text
	enum { X_SELECT_TEXT_OFFSET = 394, Y_SELECT_TEXT_OFFSET = 58, Y_SELECT_TEXT_GAP = 24, };
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );

	if( COMPARE( SELECT_MENT1_ARRAY, 0,  iSize ) )
		g_FontMgr.PrintText( iXPos + X_SELECT_TEXT_OFFSET, iYPos + Y_SELECT_TEXT_OFFSET , FONT_SIZE_13, m_vDesc[SELECT_MENT1_ARRAY].c_str() );
	if( COMPARE( SELECT_MENT2_ARRAY, 0,  iSize ) )
		g_FontMgr.PrintText( iXPos + X_SELECT_TEXT_OFFSET, iYPos + Y_SELECT_TEXT_OFFSET + (Y_SELECT_TEXT_GAP*3), FONT_SIZE_13, m_vDesc[SELECT_MENT2_ARRAY].c_str() );


	// Á¡¼±
	enum { X_DOT_OFFSET = 262, Y_DOT_OFFSET = 54, Y_DOT_GAP = 5, MAX_DOT = 48, };
	for (int i = 0; i < MAX_DOT ; i++)
	{
		if( m_pDot )
			m_pDot->Render( iXPos + X_DOT_OFFSET, iYPos + Y_DOT_OFFSET + (Y_DOT_GAP*i), UI_RENDER_MULTIPLY );
	}
}

void FirstSoldierSelectWnd::OnRenderEnd()
{
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	// img text
	enum { X_TEXTIMG_OFFSET = 146, Y_TEXTIMG_OFFSET = 226,  };
	if( m_pEndTextImg )
		m_pEndTextImg->Render( iXPos + X_TEXTIMG_OFFSET, iYPos + Y_TEXTIMG_OFFSET );

	// img
	enum { X_IMG_OFFSET = 29, Y_IMG_OFFSET = 255, };
	if( m_pEndImg )
		m_pEndImg->Render( iXPos + X_IMG_OFFSET , iYPos + Y_IMG_OFFSET );

	// back strip
	enum { MAX_LOOP = 4, X_OFFSET = 27, Y_OFFSET = 61, Y_GAP = 18, };
	int iLineCnt = 0;
	for (int i = 0; i < MAX_LOOP ; i++)
	{
		if( m_pStripGrayDark )
			m_pStripGrayDark->Render( iXPos + X_OFFSET, iYPos + Y_OFFSET + ( Y_GAP*iLineCnt ), UI_RENDER_MULTIPLY );
		iLineCnt++;
		if( m_pStripGrayLight )
			m_pStripGrayLight->Render( iXPos + X_OFFSET, iYPos + Y_OFFSET + ( Y_GAP*iLineCnt ), UI_RENDER_MULTIPLY );	
		iLineCnt++;
	}

	// bottom
	if( m_pBottomFrm )
		m_pBottomFrm->Render( iXPos, iYPos + GetHeight() - m_pBottomFrm->GetHeight() , UI_RENDER_MULTIPLY );

	// text
	enum { X_DESC_OFFSET = 31, Y_DESC_OFFSET = 63, Y_DESC_GAP = 18, };
	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );

	int iGapCnt = 0;
	int iSize = m_vDesc.size();
	for (int i = END_FIRST_ARRAY; i < END_FIRST_ARRAY+MAX_END_DESC ; i++)
	{
		if( !COMPARE( i, 0, iSize ) )
			break;

		g_FontMgr.PrintText( iXPos + X_DESC_OFFSET, iYPos + Y_DESC_OFFSET + ( Y_DESC_GAP*iGapCnt ), FONT_SIZE_13, m_vDesc[i].c_str() );
		iGapCnt++;
	}
}

void FirstSoldierSelectWnd::OnRenderSelectAfterChild()
{
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();


	// Deco icon
	enum { X_DECO_OFFSET = 230, Y_DECO_OFFSET = 72, Y_DECO_GAP = 41, };
	for (int i = 0; i < MAX_DECO ; i++)
	{
		if( m_pDecoBack )	
			m_pDecoBack->Render( iXPos + X_DECO_OFFSET, iYPos +Y_DECO_OFFSET + (Y_DECO_GAP*i), UI_RENDER_MULTIPLY, TFO_BILINEAR );
		if( m_pDeco[i] )
			m_pDeco[i]->Render( iXPos + X_DECO_OFFSET, iYPos +Y_DECO_OFFSET + (Y_DECO_GAP*i), UI_RENDER_NORMAL, TFO_BILINEAR );
	}
}

void FirstSoldierSelectWnd::iwm_show()
{
	LoadINI();
	ChangeWnd( WT_START );
}


void FirstSoldierSelectWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_NEXT_BTN:
		if( cmd == IOBN_BTNUP )
		{
			SetNextWnd();
		}
		break;
	case ID_CHAR_SELECT_BTN:
	case ID_KINDRED_SELECT_BTN:
	case ID_HAIR_SELECT_BTN:
	case ID_HAIR_COLOR_SELECT_BTN:
	case ID_FACE_SELECT_BTN:
	case ID_SKIN_COLOR_SELECT_BTN:
	case ID_UNDERWEAR_SELECT_BTN:
		if( cmd == IOBN_BTNUP )
		{
			SetPullDownMenu( pWnd );
		}
		break;
	case ID_PULLDOWN_WND:
		if( cmd == IOBN_BTNUP )
		{
			NewShopPullDownWnd *pPullDownWnd = dynamic_cast<NewShopPullDownWnd *>(pWnd);
			if( pPullDownWnd )
			{
				PullDownBtnUP( pPullDownWnd->GetOpenBtn(), param );
				pPullDownWnd->HideWnd();
			}
		}
		else if( cmd == IOWN_OVERED )
		{
			NewShopPullDownWnd *pPullDownWnd = dynamic_cast<NewShopPullDownWnd *>(pWnd);
			if( pPullDownWnd )
				PullDownBtnOver( pPullDownWnd->GetOpenBtn(), param );
		}
		break;
	case MESSAGE_BOX: // g_GUIMgr.SetMsgBox();
		if( cmd == IOBN_BTNUP )
		{
			if( param == IDYES )
			{
				ChangeWnd( WT_END );
			}
		}
		break;
	}
}

void FirstSoldierSelectWnd::SetChildTitleText( DWORD dwID, const char *szTitle )
{
	if( szTitle == NULL )
		return;

	ioWnd *pWnd = FindChildWnd( dwID );
	if( pWnd )
	{
		pWnd->SetTitleText( szTitle );
	}
}

void FirstSoldierSelectWnd::SetChildMagicCode( DWORD dwID , int iMagicCode )
{
	if( !COMPARE( dwID, ID_CHAR_SELECT_BTN, ID_UNDERWEAR_SELECT_BTN + 1 ) )
		return;

	FirstSoldierSelectBtn *pBtn = dynamic_cast<FirstSoldierSelectBtn*> ( FindChildWnd( dwID ) );
	if( pBtn )
	{
		pBtn->SetMagicCode( iMagicCode );
	}
}

void FirstSoldierSelectWnd::SetPullDownMenu( ioWnd *pOpenBtn )
{
	if( !pOpenBtn ) return;

	NewShopPullDownWnd *pPullDownWnd = dynamic_cast<NewShopPullDownWnd *>(FindChildWnd( ID_PULLDOWN_WND ));
	if( pPullDownWnd )
	{
		if( pPullDownWnd->IsShow() )
			pPullDownWnd->HideWnd();
		else
		{
			m_OverCharInfo = m_CharInfo;

			pPullDownWnd->CreatePullDown( pOpenBtn, pOpenBtn->GetXPos(), pOpenBtn->GetYPos() + 24 );
			
			vNewShopPullDownItem kItemList;
			SetPullDownItemList( pOpenBtn->GetID(), kItemList );

			for(int i = 0;i < (int)kItemList.size();i++)
			{
				pPullDownWnd->InsertItem( kItemList[i] );
			}
			pPullDownWnd->ShowWnd();

			int iHeight = pPullDownWnd->GetHeight();
			int iEndYPos = pPullDownWnd->GetYPos() + iHeight;
			if( iEndYPos >= GetHeight() )
			{
				int iNewYPos = pOpenBtn->GetYPos() - iHeight - 1;
				pPullDownWnd->SetWndPos( pOpenBtn->GetXPos(), iNewYPos );
			}
		}
	}
}

void FirstSoldierSelectWnd::SetPullDownItemList( IN DWORD dwID, OUT vNewShopPullDownItem &rkList )
{
	rkList.clear();
	
	NewShopPullDownItem kItem;
	kItem.m_dwTextColor = TCT_DEFAULT_DARKGRAY;

	IntVec vMagicCode;
	GetMagicCodeVector( vMagicCode, dwID, m_CharInfo.m_class_type, m_CharInfo.m_sex-1 );

	int iSize = vMagicCode.size();
	for (int i = 0; i < iSize ; i++)
	{
		int iMagicCode = vMagicCode[i];
		ioHashString sName;
		GetMagicName( sName, dwID, m_CharInfo.m_sex-1, iMagicCode );
		if( sName.IsEmpty() )
			continue;
		kItem.m_iMagicCode  = iMagicCode;
		kItem.m_szName      = sName;
		rkList.push_back( kItem );
	}
}

void FirstSoldierSelectWnd::PullDownBtnUP( ioWnd *pOpenBtn, int iListArray )
{
	FirstSoldierSelectBtn *pBtn = dynamic_cast<FirstSoldierSelectBtn*>( pOpenBtn );
	if( !pBtn ) return;	

	DWORD dwID = pBtn->GetID();
	IntVec vMagicCode;
	GetMagicCodeVector( vMagicCode, dwID, m_CharInfo.m_class_type, m_CharInfo.m_sex-1 );

	if( !COMPARE( iListArray, 0, (int)vMagicCode.size() ) ) return;			
	int iMagicCode = vMagicCode[iListArray];
	if( pBtn->GetMagicCode() == iMagicCode ) return;

	SetTitleAndCharInfo( m_CharInfo, dwID, iMagicCode );
	SetAllDefaultTitleAndCharInfoByCharKindred( m_CharInfo, dwID, iMagicCode );

	NewShopHelpWnd *pCharWnd = dynamic_cast<NewShopHelpWnd *>( FindChildWnd( ID_SOLDIER_WND ));
	if( pCharWnd )
	{
		pCharWnd->SetZPosition( 600.0f );
		pCharWnd->SetChar( m_CharInfo );
		pCharWnd->SetCharScale( FSS_CHAR_SCALE );
		pCharWnd->SetEquip( true, false, true );
	}
}


void FirstSoldierSelectWnd::PullDownBtnOver( ioWnd *pOpenBtn, int iListArray )
{
	FirstSoldierSelectBtn *pBtn = dynamic_cast<FirstSoldierSelectBtn*>( pOpenBtn );
	if( !pBtn ) return;	

	NewShopHelpWnd *pCharWnd = dynamic_cast<NewShopHelpWnd *>( FindChildWnd( ID_SOLDIER_WND ));
	if( !pCharWnd ) return;		

	DWORD dwID = pBtn->GetID();
	IntVec vMagicCode;
	GetMagicCodeVector( vMagicCode, dwID, m_CharInfo.m_class_type, m_CharInfo.m_sex-1 );

	bool bReleaseHelm = false;
	bool bReleaseAll  = false;
	int  iPreMagicCode   = 0;
	switch( dwID )
	{
	case ID_CHAR_SELECT_BTN:
		iPreMagicCode = m_OverCharInfo.m_class_type;
		break;
	case ID_KINDRED_SELECT_BTN:
		bReleaseHelm = true;
		iPreMagicCode = m_OverCharInfo.m_sex - 1;
		break;
	case ID_HAIR_SELECT_BTN:
		bReleaseHelm = true;
		iPreMagicCode = m_OverCharInfo.m_hair;
		break;
	case ID_HAIR_COLOR_SELECT_BTN:
		bReleaseHelm = true;
		iPreMagicCode = m_OverCharInfo.m_hair_color;
		break;
	case ID_FACE_SELECT_BTN:
		bReleaseHelm = true;
		iPreMagicCode = m_OverCharInfo.m_face;
		break;
	case ID_SKIN_COLOR_SELECT_BTN:
		bReleaseHelm = true;
		iPreMagicCode = m_OverCharInfo.m_skin_color;
		break;
	case ID_UNDERWEAR_SELECT_BTN:
		bReleaseHelm = true;
		bReleaseAll  = true;
		iPreMagicCode = m_OverCharInfo.m_underwear;
		break;
	}

	if( !COMPARE( iListArray, 0, (int)vMagicCode.size() ) ) return;	
	int iMagicCode = vMagicCode[iListArray];
	if( iPreMagicCode == iMagicCode ) return;

	SetCharInfo( m_OverCharInfo, dwID, iMagicCode );
	SetAllDefaultTitleAndCharInfoByCharKindred( m_OverCharInfo, dwID, iMagicCode, true );

	pCharWnd->SetZPosition( 600.0f );
	pCharWnd->SetChar( m_OverCharInfo );
	pCharWnd->SetCharScale( FSS_CHAR_SCALE );
	pCharWnd->SetEquip( bReleaseHelm, bReleaseAll, ( pBtn->GetID() == ID_CHAR_SELECT_BTN ) );
}

int FirstSoldierSelectWnd::GetDecoType( DWORD dwID )
{
	if( dwID == ID_KINDRED_SELECT_BTN )
		return UID_KINDRED;
	else if( dwID == ID_HAIR_SELECT_BTN )
		return UID_HAIR;
	else if( dwID == ID_HAIR_COLOR_SELECT_BTN )
		return UID_HAIR_COLOR;
	else if( dwID == ID_FACE_SELECT_BTN )
		return UID_FACE;
	else if( dwID == ID_SKIN_COLOR_SELECT_BTN )
		return UID_SKIN_COLOR;
	else if( dwID == ID_UNDERWEAR_SELECT_BTN )
		return UID_UNDERWEAR;

	return -1;
}

void FirstSoldierSelectWnd::SetCharInfo( OUT CHARACTER &rkCharInfo, IN DWORD dwID, IN int iMagicCode )
{
	if( dwID == ID_CHAR_SELECT_BTN )
	{
		rkCharInfo.m_class_type = iMagicCode;
	}
	else if( dwID == ID_KINDRED_SELECT_BTN )
	{
		if( iMagicCode == RDT_HUMAN_MAN )
		{
			rkCharInfo.m_kindred = 1;
			rkCharInfo.m_sex = 1;
		}
		else if( iMagicCode == RDT_HUMAN_WOMAN )
		{
			rkCharInfo.m_kindred = 1;
			rkCharInfo.m_sex = 2;
		}
		else if( iMagicCode == RDT_ELF_MAN )
		{
			rkCharInfo.m_kindred = 2;
			rkCharInfo.m_sex = 1;
		}	
		else if( iMagicCode == RDT_ELF_WOMAN )
		{
			rkCharInfo.m_kindred = 2;
			rkCharInfo.m_sex = 2;
		}
		else if( iMagicCode == RDT_DWARF_MAN )
		{
			rkCharInfo.m_kindred = 3;
			rkCharInfo.m_sex = 1;
		}
		else if( iMagicCode == RDT_DWARF_WOMAN )
		{
			rkCharInfo.m_kindred = 3;
			rkCharInfo.m_sex = 2;
		}
	}
	else if( dwID == ID_HAIR_SELECT_BTN )
	{
		rkCharInfo.m_hair = iMagicCode;
	}
	else if( dwID == ID_HAIR_COLOR_SELECT_BTN )
	{
		rkCharInfo.m_hair_color = iMagicCode;
	}
	else if( dwID == ID_FACE_SELECT_BTN )
	{
		rkCharInfo.m_face = iMagicCode;
	}
	else if( dwID == ID_SKIN_COLOR_SELECT_BTN )
	{
		rkCharInfo.m_skin_color = iMagicCode;
	}
	else if( dwID == ID_UNDERWEAR_SELECT_BTN )
	{
		rkCharInfo.m_underwear = iMagicCode;
	}
}

int FirstSoldierSelectWnd::GetFirstClassType()
{
	for(vDecoVectorInfo::iterator iter = m_vDecoVectorInfo.begin(); iter != m_vDecoVectorInfo.end(); ++iter)
	{
		DecoVectorInfo *pInfo = (*iter);
		if( !pInfo )
			continue;
		return pInfo->m_iClassType;
	}

	return -1;
}

int FirstSoldierSelectWnd::GetFirstDecoCode( int iClassType, int iSexType, int iDecoType )
{
	for(vDecoVectorInfo::iterator iter = m_vDecoVectorInfo.begin(); iter != m_vDecoVectorInfo.end(); ++iter)
	{
		DecoVectorInfo *pInfo = (*iter);
		if( !pInfo )
			continue;
		if( pInfo->m_iClassType != iClassType )
			continue;
		if( pInfo->m_iDecoType != UID_KINDRED )
		{
			if( pInfo->m_iSexType != iSexType )
				continue;
		}
		if( pInfo->m_iDecoType != iDecoType )
			continue;
		if( pInfo->m_vDecoCode.empty() )
			return -1;
		return pInfo->m_vDecoCode[0];
	}

	return -1;
}

void FirstSoldierSelectWnd::GetClassTypeVector( OUT IntVec &rvClassType )
{
	rvClassType.clear();

	for(vDecoVectorInfo::iterator iter = m_vDecoVectorInfo.begin(); iter != m_vDecoVectorInfo.end(); ++iter)
	{
		DecoVectorInfo *pInfo = (*iter);
		if( !pInfo )
			continue;
		if( pInfo->m_iDecoType != UID_KINDRED )
			continue;

		bool bContinue = false;
		int iSize = rvClassType.size();
		for (int i = 0; i < iSize ; i++)
		{
			if( pInfo->m_iClassType == rvClassType[i] )
			{
				bContinue = true;
				break;
			}
		}
		if( bContinue )
			continue;

		rvClassType.push_back( pInfo->m_iClassType );
	}
}

void FirstSoldierSelectWnd::GetDecoCodeVector( OUT IntVec &rvDecoCode, IN int iClassType, IN int iSexType, IN int iDecoType )
{
	rvDecoCode.clear();
	for(vDecoVectorInfo::iterator iter = m_vDecoVectorInfo.begin(); iter != m_vDecoVectorInfo.end(); ++iter)
	{
		DecoVectorInfo *pInfo = (*iter);
		if( !pInfo )
			continue;
		if( pInfo->m_iClassType != iClassType )
			continue;
		if( pInfo->m_iDecoType != UID_KINDRED )
		{
			if( pInfo->m_iSexType != iSexType )
				continue;
		}
		if( pInfo->m_iDecoType != iDecoType )
			continue;
		if( pInfo->m_vDecoCode.empty() )
			return;
		rvDecoCode = pInfo->m_vDecoCode;
		return;
	}

	return;
}

void FirstSoldierSelectWnd::SetAllDefaultTitleAndCharInfo( OUT CHARACTER &rkCharInfo, IN int iClassType, IN int iSexType, IN bool bChangeOnlyCharInfo )
{
	for (int i = ID_CHAR_SELECT_BTN; i < ID_UNDERWEAR_SELECT_BTN+1 ; i++)
	{
		int iMagicCode = -1;
		if( i == ID_CHAR_SELECT_BTN )
		{
			iMagicCode = iClassType;
		}
		else if( i == ID_KINDRED_SELECT_BTN )
		{
			iMagicCode = iSexType;
		}
		else
		{
			iMagicCode = GetFirstDecoCode( iClassType, iSexType, GetDecoType( i ) );
		}

		if( bChangeOnlyCharInfo )
			SetCharInfo( rkCharInfo, i, iMagicCode );
		else
			SetTitleAndCharInfo( rkCharInfo, i, iMagicCode );
	}
}

int FirstSoldierSelectWnd::GetDecoImgArray( DWORD dwID )
{
	if( dwID == ID_KINDRED_SELECT_BTN )
		return DECO_KINDRED_ARRAY;
	else if( dwID == ID_HAIR_SELECT_BTN )
		return DECO_HAIR_ARRAY;
	else if( dwID == ID_HAIR_COLOR_SELECT_BTN )
		return DECO_HAIR_COLOR_ARRAY;
	else if( dwID == ID_FACE_SELECT_BTN )
		return DECO_FACE_ARRAY;
	else if( dwID == ID_SKIN_COLOR_SELECT_BTN )
		return DECO_SKIN_COLOR_ARRAY;
	else if( dwID == ID_UNDERWEAR_SELECT_BTN )
		return DECO_UNDERWEAR_ARRAY;

	return -1;
}

void FirstSoldierSelectWnd::GetMagicCodeVector( OUT IntVec &rvMagicCode, IN DWORD dwID, IN int iClassType, int iSexType )
{
	if( dwID == ID_CHAR_SELECT_BTN )
	{		
		GetClassTypeVector( rvMagicCode );
	}
	else
	{
		GetDecoCodeVector( rvMagicCode, iClassType, iSexType, GetDecoType( dwID ) );
	}
}

void FirstSoldierSelectWnd::GetMagicName( OUT ioHashString &rsName, IN DWORD dwID, IN int iSexType, IN int iMagicCode )
{
	rsName.Clear();
	if( dwID == ID_CHAR_SELECT_BTN )
	{
		int iClassType = iMagicCode;
		rsName = g_MyInfo.GetClassName( iClassType );
	}
	else
	{
		int iDecoCode = iMagicCode;
		const DecoData &kData = g_DecorationPrice.GetDecoData( iSexType, GetDecoType( dwID ) , iMagicCode );
		rsName = kData.m_szName;
	}
}

void FirstSoldierSelectWnd::SetAllDefaultTitleAndCharInfoByCharKindred( OUT CHARACTER &rkCharInfo, IN DWORD dwID,IN int iMagicCode, IN bool bChangeOnlyCharInfo /*= false */ )
{
	if( dwID == ID_CHAR_SELECT_BTN )
	{
		int iClassType  = iMagicCode;
		int iSexType    = GetFirstDecoCode( iClassType, -1, UID_KINDRED );
		SetAllDefaultTitleAndCharInfo( rkCharInfo, iClassType, iSexType, bChangeOnlyCharInfo );
	}
	else if( dwID == ID_KINDRED_SELECT_BTN )
	{
		int iSexType = iMagicCode;
		SetAllDefaultTitleAndCharInfo( rkCharInfo, rkCharInfo.m_class_type, iSexType, bChangeOnlyCharInfo );
	}
}

void FirstSoldierSelectWnd::SetDecoImg( DWORD dwID, int iSexType, int iDecoCode )
{
	if( dwID == ID_CHAR_SELECT_BTN )
		return;

	const DecoData &kData = g_DecorationPrice.GetDecoData( iSexType, GetDecoType( dwID ) , iDecoCode );
	if( kData.m_szIcon.IsEmpty() )
		return;

	ioUIRenderImage *pImg = g_UIImageSetMgr.CreateImageByFullName( kData.m_szIcon );
	if( !pImg )
		return;
	
	int iArray = GetDecoImgArray( dwID );
	if( !COMPARE( iArray, 0, MAX_DECO ) )
		return;
		
	SAFEDELETE( m_pDeco[iArray] );
	m_pDeco[iArray] = pImg;
	if( m_pDeco[iArray] )
		m_pDeco[iArray]->SetScale( FSS_DECO_ICON_SCALE, FSS_DECO_ICON_SCALE );
}

void FirstSoldierSelectWnd::SetTitleAndCharInfo( OUT CHARACTER &rkCharInfo, IN DWORD dwID, IN int iMagicCode )
{
	ioHashString sName;
	GetMagicName( sName, dwID, rkCharInfo.m_sex-1, iMagicCode );
	SetChildTitleText( dwID, sName.c_str() ); 
	SetChildMagicCode( dwID, iMagicCode );
	SetCharInfo( rkCharInfo, dwID, iMagicCode );
	SetDecoImg( dwID, rkCharInfo.m_sex-1, iMagicCode );
}

void FirstSoldierSelectWnd::SetNextWnd()
{
	if( m_eWndType == WT_START )
		ChangeWnd( WT_SELECT );
	else if( m_eWndType == WT_SELECT )
	{
		if( COMPARE( SELECT_CHECK_ARRAY, 0, (int) m_vDesc.size() ) )
			g_GUIMgr.SetMsgBox( MB_YESNO, this, m_vDesc[SELECT_CHECK_ARRAY].c_str() );
	}
	else if( m_eWndType == WT_END )
	{
		SendFirstChar();
		HideWnd();
	}
}

void FirstSoldierSelectWnd::SendFirstChar()
{
	if( !g_MyInfo.IsTutorialUser() )
		return;
	
	SP2Packet kPacket( CTPK_CHAR_CREATE );
	kPacket << CHAR_CREATE_FIRST_HIRE;
	kPacket << m_CharInfo.m_class_type;
	kPacket << m_CharInfo.m_kindred;
	kPacket	<< m_CharInfo.m_sex;
	kPacket	<< g_ClassPrice.GetFirstHireLimit();
	kPacket	<< 0;     // iPrice
	kPacket << false; // bCash
	kPacket << CPT_TIME;
	kPacket << m_CharInfo.m_hair;
	kPacket << m_CharInfo.m_hair_color;
	kPacket << m_CharInfo.m_face;
	kPacket << m_CharInfo.m_skin_color;
	kPacket << m_CharInfo.m_underwear;
	TCPNetwork::SendToServer( kPacket );
	g_App.SetMouseBusy( true );
}

void FirstSoldierSelectWnd::LoadINI()
{
	Clear();

	ioINILoader_e kLoader( "config/sp2_first_soldier.ini" );
	kLoader.SetTitle_e( "Desc" );

	char szKeyName[MAX_PATH]="";
	char szBuf[MAX_PATH]="";
	ioHashString sDesc;
	for(int i = 0;i < MAX_DESC;i++)
	{
		StringCbPrintf_e( szKeyName, sizeof(szKeyName), "Desc%d", i+1 );
		kLoader.LoadString( szKeyName, "", szBuf, sizeof(szBuf) );
		sDesc = szBuf;
		m_vDesc.push_back( sDesc );
	}		

	enum { MAX_LOAD = 11, };
	int  iDecoTypeArray[MAX_LOAD] = { UID_KINDRED, UID_HAIR, UID_HAIR_COLOR, UID_FACE, UID_SKIN_COLOR, UID_UNDERWEAR, UID_HAIR, UID_HAIR_COLOR, UID_FACE, UID_SKIN_COLOR, UID_UNDERWEAR };
	int  iSexTypeArray[MAX_LOAD]  = { -1, RDT_HUMAN_MAN, RDT_HUMAN_MAN, RDT_HUMAN_MAN, RDT_HUMAN_MAN, RDT_HUMAN_MAN, RDT_HUMAN_WOMAN, RDT_HUMAN_WOMAN, RDT_HUMAN_WOMAN, RDT_HUMAN_WOMAN, RDT_HUMAN_WOMAN };
	char szMaxArray[MAX_LOAD][MAX_PATH] ={ "MaxKindred", "MaxManHair", "MaxManHairColor", "MaxManFace", "MaxManSkinColor", "MaxManUnderwear",  "MaxWomanHair", "MaxWomanHairColor", "MaxWomanFace", "MaxWomanSkinColor", "MaxWomanUnderwear"};
	char szNameArray[MAX_LOAD][MAX_PATH]={ "Kindred%d",  "ManHair%d",  "ManHairColor%d",  "ManFace%d",  "ManSkinColor%d",  "ManUnderwear%d",   "WomanHair%d",  "WomanHairColor%d",  "WomanFace%d",  "WomanSkinColor%d",  "WomanUnderwear%d" };

	kLoader.SetTitle_e( "Common" );
	int iMaxSoldier = kLoader.LoadInt_e( "MaxSoldier", 0 );
	for (int i = 0; i < iMaxSoldier ; i++)
	{
		ZeroMemory( szKeyName, sizeof( szKeyName ) );
		StringCbPrintf_e( szKeyName, sizeof( szKeyName ), "Soldier%d", i+1 );
		kLoader.SetTitle( szKeyName );

		int iClassType = kLoader.LoadInt_e( "ClassType", 0 );
		if( iClassType == 0 )
			continue;

		for (int j = 0; j < MAX_LOAD ; j++)
		{
			DecoVectorInfo *pInfo = new DecoVectorInfo;
			if( !pInfo )
				continue;
			pInfo->m_iClassType= iClassType;
			pInfo->m_iDecoType = iDecoTypeArray[j];
			pInfo->m_iSexType  = iSexTypeArray[j];
			ZeroMemory( szKeyName, sizeof( szKeyName ) );
			StringCbCopy( szKeyName, sizeof( szKeyName ), szMaxArray[j] );
			int iMaxDeco = kLoader.LoadInt( szKeyName, 0 );
			for (int k = 0; k < iMaxDeco ; k++)
			{
				ZeroMemory( szKeyName, sizeof( szKeyName ) );
				StringCbPrintf( szKeyName, sizeof( szKeyName ), szNameArray[j], k+1 );
				int iMagicCode = kLoader.LoadInt( szKeyName, 0 );
				pInfo->m_vDecoCode.push_back( iMagicCode );
			}	
			m_vDecoVectorInfo.push_back( pInfo );
		}	
	}	
}

void FirstSoldierSelectWnd::Clear()
{
	m_vDesc.clear();
	for(vDecoVectorInfo::iterator iter = m_vDecoVectorInfo.begin(); iter != m_vDecoVectorInfo.end(); ++iter)
	{
		DecoVectorInfo *pInfo = (*iter);
		if( !pInfo )
			continue;
		pInfo->m_vDecoCode.clear();
		SAFEDELETE( pInfo );
	}
	m_vDecoVectorInfo.clear();
}


