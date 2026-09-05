
#include "StdAfx.h"

#include "../io3DEngine/ioINILoader.h"
#include "../io3DEngine/ioWnd.h"
#include "../io3DEngine/ioEdit.h"
#include "../io3DEngine/InputBox.h"
#include "../io3DEngine/ioXMLDocument.h"
#include "../io3DEngine/ioXMLElement.h"
#include "../io3DEngine/ioFontManager.h"
#include "../io3DEngine/ioUIRenderImage.h"
#include "../io3DEngine/ioUIRenderFrame.h"
#include "../io3DEngine/ioUIImageSetManager.h"
#include "../io3DEngine/ioSoundManager.h"
#include "../io3DEngine/iostringmanager.h"
#include "../io3DEngine/Safesprintf.h"

#include "../TextColorTable.h"

#include "../WndID.h"
#include "ioSP2GUIManager.h"
#include "MyInventoryWnd.h"

#include "ItemResellWnd.h"

#include <strsafe.h>


//////////////////////////////////////////////////////////////////////////
ItemResellWnd::ItemResellWnd()
{
	m_pSellMark = NULL;
	m_pReturnWnd = NULL;

	m_pBottomFrm = NULL;
	m_pEditFrm = NULL;

	m_pPreEdit = NULL;
	m_pSellEdit = NULL;

	for( int i=0; i < MAX_INFO_LINE; ++i )
	{
		m_Desc[i].ClearList();
	}
}

ItemResellWnd::~ItemResellWnd()
{
	SAFEDELETE(m_pSellMark);
	SAFEDELETE(m_pEditFrm);
	SAFEDELETE(m_pBottomFrm);

	for( int i=0; i < MAX_INFO_LINE; ++i )
	{
		m_Desc[i].ClearList();
	}
}

void ItemResellWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "sell_mark" )
	{
		SAFEDELETE( m_pSellMark );
		m_pSellMark = pImage;
	}
	else
		ioWnd::AddRenderImage( szType, pImage );
}

void ItemResellWnd::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( szType == "BottomFrm" )
	{
		SAFEDELETE( m_pBottomFrm );
		m_pBottomFrm = pFrame;
	}
	else if( szType == "EditFrm" )
	{
		SAFEDELETE( m_pEditFrm );
		m_pEditFrm = pFrame;
	}
	else
	{
		ioWnd::AddRenderFrame( szType, pFrame );
	}
}

void ItemResellWnd::iwm_show()
{
	if( m_pBottomFrm )
		m_pBottomFrm->SetPosition( 0, GetHeight() - BOTTOM_FRAME_GAP );

	ioWnd *pSellBtn = FindChildWnd( ID_RESELL );
	if( pSellBtn )
	{
		pSellBtn->SetWndPos( pSellBtn->GetXPos(), GetHeight() - BUTTON_GAP );

		switch( m_dwSellActionType )
		{
		case ACST_SELL:
		case ACST_SELL_MEDAL:
		case ACST_SELL_COSTUME:
		case ACST_SELL_ACCESSORY:
			pSellBtn->SetTitleText( "팔기" );
			break;
		case ACST_DISASSEMBLE:
			pSellBtn->SetTitleText( "분해" );
			break;
		}
	}

	ioWnd *pCancelBtn = FindChildWnd( ID_CANCEL );
	if( pCancelBtn )
		pCancelBtn->SetWndPos( pCancelBtn->GetXPos(), GetHeight() - BUTTON_GAP );

	ioEdit *pEdit = dynamic_cast<ioEdit*>( FindChildWnd( ID_SELL_EDIT ) );
	if( pEdit && pEdit->IsShow() )
	{
		pEdit->SetWndPos( pEdit->GetXPos(), GetHeight() - EDIT_GAP );

		m_pPreEdit = ioEdit::m_pKeyFocusEdit;
		pEdit->ClearString();
		pEdit->SetKeyFocus();
		g_InputBox.SetString( "" );

		m_pSellEdit = pEdit;
	}
}

void ItemResellWnd::iwm_hide()
{
	if( m_pPreEdit )	
	{
		ioWnd *pParent = m_pPreEdit->GetParent();
		if( pParent )
		{
			if( pParent->IsShow() )
				((ioEdit*)m_pPreEdit)->SetKeyFocus();	
		}
	}	
	m_pPreEdit = NULL;
}

void ItemResellWnd::SetInfoAndShow( const ioHashString &szTitle, ioUIRenderImage *pIcon, ioUIRenderImage *pSubIcon, 
									int iIconNum, ioComplexStringPrinter *pPrinter, ioWnd *pReturnWnd, DWORD dwEditType,
									DWORD dwActionType, bool bIsCostume, int nGradeType )
{
	m_dwSellEditType = dwEditType;
	m_dwSellActionType = dwActionType;

	m_pReturnWnd = pReturnWnd;

	SetTitleText( szTitle.c_str() );

	InventoryIconBtn *pIconBtn = dynamic_cast<InventoryIconBtn*>(FindChildWnd(ID_ITEM_ICON));
	if( pIconBtn )
	{
		pIconBtn->SetIcon( pIcon, FLOAT1, FLOAT1, iIconNum );
		pIconBtn->SetSubIcon( pSubIcon );
		pIconBtn->SetCostume( bIsCostume );
		pIconBtn->SetGradeType( nGradeType );
	}

	for( int i=0; i < MAX_INFO_LINE; ++i )
	{
		m_Desc[i].ClearList();

		if( pPrinter[i].IsEmpty() )
			break;

		pPrinter[i].CopyFullText( m_Desc[i] );
	}

	switch( m_dwSellActionType )
	{
	case ACST_SELL:
	case ACST_SELL_MEDAL:
	case ACST_SELL_COSTUME:
	case ACST_SELL_ACCESSORY:
	case ACST_SELL_DECO:
		if( m_dwSellEditType == SELL_EDIT_NONE )
		{
			SetSize( GetWidth(), NORMAL_HEIGHT );
			HideChildWnd( ID_SELL_EDIT );
		}
		else if( m_dwSellEditType == SELL_EDIT_EXTRAITEM )
		{
			SetSize( GetWidth(), EDIT_HEIGHT );
			ShowChildWnd( ID_SELL_EDIT );
		}
		break;
	case ACST_DISASSEMBLE:
		if( m_dwSellEditType == SELL_EDIT_NONE )
		{
			SetSize( GetWidth(), DIS_NORMAL_HEIGHT );
			HideChildWnd( ID_SELL_EDIT );
		}
		else if( m_dwSellEditType == SELL_EDIT_EXTRAITEM )
		{
			SetSize( GetWidth(), DIS_EDIT_HEIGHT );
			ShowChildWnd( ID_SELL_EDIT );
		}
		break;
	}

	ShowWnd();
}

void ItemResellWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	char szText1[MAX_PATH] = "";
	char szText2[MAX_PATH] = "";
	char szText3[MAX_PATH] = "";

	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_EXIT:
	case ID_CANCEL:
		if( cmd == IOBN_BTNUP )
			HideWnd();
		break;
	case ID_RESELL:
		if( cmd == IOBN_BTNUP )
		{
			if( m_dwSellEditType == SELL_EDIT_EXTRAITEM )
			{
				switch( m_dwSellActionType )
				{
				case ACST_SELL:
					StringCbCopy( szText1, sizeof(szText1), STR(1) );
					StringCbCopy( szText2, sizeof(szText2), STR(2) );
					StringCbCopy( szText3, sizeof(szText3), STR(3) );
					break;
				case ACST_DISASSEMBLE:
					StringCbCopy( szText1, sizeof(szText1), "영구장비분해를 입력하세요" );
					StringCbCopy( szText2, sizeof(szText2), "영구장비분해" );
					StringCbCopy( szText3, sizeof(szText3), "영구장비분해를 입력하세요" );
					break;
				case ACST_SELL_MEDAL:
					StringCbCopy( szText1, sizeof(szText1), "영구메달팔기를 입력하세요" );
					StringCbCopy( szText2, sizeof(szText2), "영구메달팔기" );
					StringCbCopy( szText3, sizeof(szText3), "영구메달팔기를 입력하세요" );
					break;
				case ACST_SELL_COSTUME:
					StringCbCopy( szText1, sizeof(szText1), "영구코스튬팔기를 입력하세요" );
					StringCbCopy( szText2, sizeof(szText2), "영구코스튬팔기" );
					StringCbCopy( szText3, sizeof(szText3), "영구코스튬팔기를 입력하세요" );
					break;
				case ACST_SELL_ACCESSORY:
					StringCbCopy( szText1, sizeof(szText1), "액세서리팔기를 입력하세요" );
					StringCbCopy( szText2, sizeof(szText2), "액세서리팔기" );
					StringCbCopy( szText3, sizeof(szText3), "액세서리팔기를 입력하세요" );
					break;
				}

				if( !m_pSellEdit || !m_pSellEdit->IsShow() )
				{
					g_GUIMgr.SetMsgBox( MB_OK, NULL, szText1 );
					return;
				}

				ioEdit *pEdit = dynamic_cast<ioEdit*>(m_pSellEdit);
				if( strcmp( pEdit->GetText(), szText2 ) != 0 )
				{
					g_GUIMgr.SetMsgBox( MB_OK, NULL, szText3 );
					return;
				}
			}

			HideWnd();
			if( m_pReturnWnd )
			{
				m_pReturnWnd->iwm_command( this, cmd,  m_dwSellActionType );
			}
		}
		break;
	case ID_SELL_EDIT:
		if( cmd == IOED_INPUT )
		{
			if( param == 0 || param == 1 )
			{
				ioEdit *pEdit = dynamic_cast<ioEdit*>(pWnd);
				pEdit->SetKeyFocus();
			}
		}
		break;
	}
}

void ItemResellWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	enum
	{
		DESC1_X		= 125,
		DESC1_Y		= 146,
		LINE1_GAP	= 23,

		DESC2_X		= 125,
		DESC2_Y		= 199,
		LINE2_GAP	= 20,
	};

	if( m_pBottomFrm )
		m_pBottomFrm->Render( iXPos, iYPos, UI_RENDER_MULTIPLY );


	// DESC1
	int i=0;
	for( i=0; i < 2; ++i )
	{
		if( m_Desc[i].IsEmpty() )
			continue;

		m_Desc[i].PrintFullText( iXPos+DESC1_X, iYPos+DESC1_Y+(LINE1_GAP*i), TAT_CENTER );
	}

	// DESC2
	for( i=2; i < MAX_INFO_LINE; ++i )
	{
		if( m_Desc[i].IsEmpty() )
			continue;

		m_Desc[i].PrintFullText( iXPos+DESC2_X, iYPos+DESC2_Y+(LINE2_GAP*(i-2)), TAT_CENTER );
	}

	char szText1[MAX_PATH] = "";
	char szText2[MAX_PATH] = "";
	char szText3[MAX_PATH] = "";

	if( m_dwSellEditType == SELL_EDIT_EXTRAITEM )
	{
		int iText1YPos = iYPos + 269;
		int iText2YPos = iYPos + 292;

		int iEditFrmXPos = iXPos + 17;
		int iEditFrmYPos = iYPos + 288;

		switch( m_dwSellActionType )
		{
		case ACST_SELL:
			StringCbCopy( szText1, sizeof(szText1), STR(1) );
			StringCbCopy( szText2, sizeof(szText2), STR(2) );
			StringCbCopy( szText3, sizeof(szText3), STR(3) );
			break;
		case ACST_DISASSEMBLE:
			StringCbCopy( szText1, sizeof(szText1), "영구장비분해" );
			StringCbCopy( szText2, sizeof(szText2), "] 입력후 분해를 누르세요" );
			StringCbCopy( szText3, sizeof(szText3), "영구장비분해를 입력하세요" );

			iText1YPos = iYPos + 249;
			iText2YPos = iYPos + 272;

			iEditFrmYPos = iYPos + 268;
			break;
		case ACST_SELL_MEDAL:
			StringCbCopy( szText1, sizeof(szText1), "영구메달팔기" );
			StringCbCopy( szText2, sizeof(szText2), "] 입력후 팔기를 누르세요" );
			StringCbCopy( szText3, sizeof(szText3), "영구메달팔기를 입력하세요" );
			break;
		case ACST_SELL_COSTUME:
			StringCbCopy( szText1, sizeof(szText1), "영구코스튬팔기" );
			StringCbCopy( szText2, sizeof(szText2), "] 입력후 팔기를 누르세요" );
			StringCbCopy( szText3, sizeof(szText3), "영구코스튬팔기를 입력하세요" );
			break;
		case ACST_SELL_ACCESSORY:
			StringCbCopy( szText1, sizeof(szText1), "액세서리팔기" );
			StringCbCopy( szText2, sizeof(szText2), "] 입력후 팔기를 누르세요" );
			StringCbCopy( szText3, sizeof(szText3), "액세서리팔기를 입력하세요" );
			break;
		}

		ioComplexStringPrinter kPrinter;
		kPrinter.SetBkColor( 0, 0, 0 );
		kPrinter.SetTextStyle( TS_NORMAL );
		kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
		kPrinter.AddTextPiece( FONT_SIZE_13, "[" );
		kPrinter.SetTextColor( TCT_DEFAULT_RED );
		kPrinter.AddTextPiece( FONT_SIZE_13, szText1 );
		kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
		kPrinter.AddTextPiece( FONT_SIZE_13, szText2 );
		kPrinter.PrintFullText( iXPos + 125, iText1YPos, TAT_CENTER );
		kPrinter.ClearList();

		if( m_pEditFrm )
			m_pEditFrm->Render( iEditFrmXPos, iEditFrmYPos );

		if( m_pSellEdit != ioEdit::m_pKeyFocusEdit )
		{
			kPrinter.SetBkColor( 0, 0, 0 );
			kPrinter.SetTextStyle( TS_NORMAL );
			kPrinter.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
			kPrinter.AddTextPiece( FONT_SIZE_12, szText3 );
			kPrinter.PrintFullText( iXPos + 125, iText2YPos, TAT_CENTER );
			kPrinter.ClearList();
		}
	}
}

void ItemResellWnd::OnRenderAfterChild()
{
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	enum
	{
		ICON_X		= 125,
		ICON_Y		= 98,
	};

	if( m_pSellMark )
		m_pSellMark->Render( iXPos+ICON_X, iYPos+ICON_Y );
}

//////////////////////////////////////////////////////////////////////////


